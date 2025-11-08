// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : heap.cpp
//   Purpose : memory management and heap control
// Component : Babel Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>
#include <malloc.h>

// ********************************************************************************
// Locals

typedef struct _THeapBlock {
	uint32      offset;     // +0x00  Offset from bHeapBase to start of this block (i.e., header)
	uint32      size;       // +0x04  Total block size INCLUDING header
	ushort      used;       // +0x08  1 = allocated, 0 = free (uint16)
	ushort      flags;      // +0x0A  High bits hold BALLOC_* flags, low byte holds module id (uint16)
	uint32      group;      // +0x0C  0xDEFA = "Default"; otherwise pointer to C-string
	_THeapBlock *freePrev;  // +0x10  Free-list prev (if free) or Used-list prev (if used)
	_THeapBlock *freeNext;  // +0x14  Free-list next (if free) or Used-list next (if used)
	_THeapBlock *orderPrev; // +0x18  Previous block in physical-address order
	_THeapBlock *orderNext; // +0x1C  Next block in physical-address order
} THeapBlock;

// Default "group" marker used by original PC build
static const uint32 kDefaultGroup = 0xDEFA;

// Minimum leftover size after a split to form a valid free block (incl. header).
static const uint32 kMinSplit = 0x40;

// Align to 16 bytes (the original code aligned sizes and base to 16).
static inline uint32 Align16(uint32 v) { return (v + 0x0F) & ~0x0F; }

static UINT8      *bHeapBase      = 0;     // aligned heap start (== (UINT8*)bFirstBlock)
static LPVOID      bRealHeapBase  = 0;     // pointer returned by VirtualAlloc or provided base
static uint        bHeapSize      = 0;     // total size of the managed heap region
static uint        bBytesAllocated= 0;     // sum of all allocated block sizes (incl. headers)
static uint        bHeapPeakUsage = 0;     // high-water mark (incl. headers)
static int         bIsDynamicHeap = 0;     // 1 if VirtualAlloc was used
static uint        bSmallBlockThreshold = 1024 * 1024; // default 1MB (tunable)

// Physical order list anchor: first block (points to itself at init)
static THeapBlock *bFirstBlock    = 0;

// Free-list sentinel (used==0); head/tail via freeNext/freePrev
static THeapBlock  bFreeRoot;

// Used-list sentinel (used==1); head/tail via freeNext/freePrev (repurposed links)
static THeapBlock  bUsedSentinel;

// Group stack (top used for bkHeapAlloc, etc.). Stores either 0xDEFA or (uint32)char*.
static uint32      bGroupStack[32];
static int         bGroupSP = 0;

// ********************************************************************************
// Local Helpers

// Insert a FREE block into the free-list sorted by ascending offset
static void ConnectFreeBlock(THeapBlock *blk)
{
    THeapBlock* tail = bFreeRoot.freePrev;
    if (tail == &bFreeRoot) {
        // empty list -> make blk both head and tail
        bFreeRoot.freeNext = blk;
        bFreeRoot.freePrev = blk;
        blk->freeNext = &bFreeRoot;
        blk->freePrev = &bFreeRoot;
        return;
    }

    THeapBlock* cur = tail;
    const int key = (int)blk->offset; // signed compares per JGE/JG

    for (;;) {
        // cur->offset < key ?
        if ((int)cur->offset < key) {
            THeapBlock* prev = cur->freePrev;
            // prev == root || prev->offset > key ?
            if (prev == &bFreeRoot || (int)prev->offset > key) {
                // insert between prev and cur
                blk->freeNext = cur;
                blk->freePrev = prev;
                prev->freeNext = blk;
                cur->freePrev  = blk;
                return;
            }
        }

        // step backward
        cur = cur->freePrev;

        // fall-through: append at tail if we wrapped to root
        if (cur == &bFreeRoot) {
            blk->freePrev      = tail;
            blk->freeNext      = &bFreeRoot;
            tail->freeNext     = blk;
            bFreeRoot.freePrev = blk;
            return;
        }
    }
}

// Unlink a block (free or used) from whichever list its freePrev/freeNext currently reference.
static inline void UnlinkListNode(THeapBlock *blk)
{
	blk->freePrev->freeNext = blk->freeNext;
	blk->freeNext->freePrev = blk->freePrev;
}

// Coalesce with previous physical neighbor if it is FREE. Returns the (possibly new) block.
static THeapBlock* TryCoalescePrev(THeapBlock *blk)
{
	THeapBlock *prev = blk->orderPrev;
	if (prev != bFirstBlock && prev->used == 0)
	{
		// Merge prev into blk
		UnlinkListNode(prev);                 // remove prev from free-list
		blk->size += prev->size;              // grow blk
		blk->orderPrev = prev->orderPrev;     // bypass prev in physical order
		prev->orderPrev->orderNext = blk;

		// Keep free-list link stable by stealing prev’s position
		blk->freePrev = prev->freePrev;
		blk->freePrev->freeNext = blk;
	}
	return blk;
}

// Coalesce with next physical neighbor if it is FREE. Returns the (possibly new) block.
static THeapBlock* TryCoalesceNext(THeapBlock *blk)
{
	THeapBlock *next = blk->orderNext;
	if (next != bFirstBlock && next->used == 0)
	{
		UnlinkListNode(next);
		blk->size += next->size;
		blk->orderNext = next->orderNext;
		next->orderNext->orderPrev = blk;
	}
	return blk;
}

// Insert an ALLOCATED block into the used-list in a way that mirrors the original:
// - First block is a special-case: appended to the tail.
// - Small blocks are linked near their orderNext neighbor.
// - Large blocks are linked near their orderPrev neighbor.
/* Behavior mirrored from bkHeapAllocEx epilogue. */
static void LinkUsedBlock(THeapBlock *blk, uint userSizeAligned)
{
	THeapBlock *tail = bUsedSentinel.freePrev;

	if (blk == bFirstBlock)
	{
		blk->freeNext = &bUsedSentinel;
		blk->freePrev = tail;
		tail->freeNext = blk;
		blk->freeNext->freePrev = blk;
		return;
	}

	THeapBlock *anchor;
	if (userSizeAligned < bSmallBlockThreshold)
	{
		// Link before orderNext’s insertion point
		anchor = blk->orderNext;
		blk->freePrev = anchor->freePrev;
	}
	else
	{
		// Link after orderPrev’s insertion point
		blk->freePrev = blk->orderPrev;
		anchor = blk->orderPrev->freeNext;
	}
	blk->freeNext = anchor;
	blk->freePrev->freeNext = blk;
	blk->freeNext->freePrev = blk;
}

// Create a system block for CRT malloc/realloc path (when heap isn’t initialised).
static void* MakeSystemAlloc(uint sizeAligned, ushort flags)
{
	size_t total = sizeAligned + sizeof(THeapBlock);
	void *mem = malloc(total);
	if (!mem) return 0;

	THeapBlock *b = (THeapBlock*)mem;
	b->offset    = 0;                 // not used for system blocks
	b->size      = (uint32)total;
	b->used      = 1;
	b->flags     = flags;
	b->group     = kDefaultGroup;
	b->freePrev  = 0;
	b->freeNext  = 0;
	b->orderPrev = 0;                 // orderPrev == NULL => not managed by Babel heap
	b->orderNext = 0;
	return (void*)(b + 1);
}

// ********************************************************************************
// Function Implementations

/* --------------------------------------------------------------------------------
   Function : bInitHeap
   Purpose : initialise the heap
   Parameters : base address (NULL for dynamic), size of heap
   Returns : OK/FAIL
   Info : 
*/

int bInitHeap(uint base, uint size)
{
	_MEMORYSTATUS memStatus;

	// Optional heuristic: ensure a decent dynamic heap for tools/debug builds
	if ( ((bBkInitFlags & 0x04) != 0) )
	{
		GlobalMemoryStatus(&memStatus);
		if (memStatus.dwTotalPhys != 0xFFFFFFFF)
		{
			uint half = (uint)(memStatus.dwTotalPhys >> 1);
			if (size < half) size = half;
		}
	}

	bHeapSize = Align16(size);

	LPVOID heapBase = (LPVOID)base;
	if (base == 0)
		heapBase = VirtualAlloc(NULL, bHeapSize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

	bRealHeapBase = heapBase;
	if (!heapBase)
		return 0;

	bIsDynamicHeap = (base == 0);

	// Basic logging
	bkPrintf("bInitHeap: Allocated %d Kb %s heap at 0x%08x\n",
		(int)(bHeapSize >> 10),
		bIsDynamicHeap ? "DYNAMIC" : "STATIC",
		heapBase);

	// Align the first block start to 16, but keep offset of first block == 0
	bFirstBlock  = (THeapBlock*)(((DWORD)bRealHeapBase + 0x0F) & ~0x0F);
	bHeapBase    = (UINT8*)bFirstBlock;

	// Init sentinels
	memset(&bFreeRoot, 0, sizeof(bFreeRoot));
	memset(&bUsedSentinel, 0, sizeof(bUsedSentinel));
	bFreeRoot.used       = 0;
	bUsedSentinel.used   = 1;
	bUsedSentinel.freeNext = &bUsedSentinel;
	bUsedSentinel.freePrev = &bUsedSentinel;

	// Build a single large free block covering the region
	bFirstBlock->offset    = 0;
	bFirstBlock->size      = bHeapSize;
	bFirstBlock->used      = 0;
	bFirstBlock->flags     = 0;
	bFirstBlock->group     = 0;

	// Physical order points to itself
	bFirstBlock->orderNext = bFirstBlock;
	bFirstBlock->orderPrev = bFirstBlock;

	// Free list: root <-> firstBlock
	bFirstBlock->freeNext  = &bFreeRoot;
	bFirstBlock->freePrev  = &bFreeRoot;
	bFreeRoot.freeNext     = bFirstBlock;
	bFreeRoot.freePrev     = bFirstBlock;

	// Stats & group stack
	bBytesAllocated = 0;
	bHeapPeakUsage  = 0;

	bGroupSP = 0;
	bGroupStack[0] = kDefaultGroup;

	return 1;
}


/* --------------------------------------------------------------------------------
   Function : bShutdownHeap
   Purpose : shutdown the global heap
   Parameters : 
   Returns : 
   Info : 
*/

void bShutdownHeap()
{
	// Leak report
	if (bUsedSentinel.freePrev == &bUsedSentinel)
	{
		bkPrintf("Heap is clean (usage peaked at %d Kb)\n", (int)(bHeapPeakUsage >> 10));
	}
	else
	{
		bkPrintf("Memory leaks detected!\n");
		for (THeapBlock *it = bUsedSentinel.freePrev; it != &bUsedSentinel; it = it->freePrev)
		{
			const char *groupStr = (it->group == kDefaultGroup) ? "Default" : (const char*)it->group;

			// Build a short data preview
			char preview[32] = {0};
			bkDataToSafeString((unsigned char*)(it + 1),
                   (int)it->size - (int)sizeof(THeapBlock),
                   preview, sizeof(preview));

			const char *by = (it->flags & BALLOC_MALLOC) ? "Malloc"
			                  : (it->flags & BALLOC_NEW) ? "New"
			                  : "";

			bkPrintf("%s 0x%08X, %8d bytes [%s %s]\n    \"%s\"\n",
				by,
				(unsigned long)(it + 1),
				(int)it->size - (int)sizeof(THeapBlock),
				groupStr, "(module)",
				preview);
		}
		bkPrintf("Heap usage peaked at %d Kb.\n", (int)(bHeapPeakUsage >> 10));
	}

	// Release dynamic heap region
	if (bIsDynamicHeap)
	{
		VirtualFree(bRealHeapBase, 0, MEM_RELEASE);
	}

	// Reset globals (safety)
	bHeapBase = 0;
	bRealHeapBase = 0;
	bFirstBlock = 0;
	bHeapSize = 0;
	bBytesAllocated = 0;
	bHeapPeakUsage = 0;
	bIsDynamicHeap = 0;
}


/* --------------------------------------------------------------------------------
   Function : bkHeapReset
   Purpose : reset the heap to be empty
   Parameters : 
   Returns : 
   Info : 
*/

void bkHeapReset()
{
        bkPrintf("*** WARNING *** bkHeapReset was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkHeapAlloc
   Purpose : allocate a memory block
   Parameters : block size, file doing the allocation, line number doing the allocation, flags
   Returns : ptr to block or NULL for failure
   Info : 
*/

void *bkHeapAlloc(uint size, char *file, int line, ushort flags)
{
    return bkHeapAllocEx(size, file, line, flags, bGroupStack[bGroupSP]);
}

/*	--------------------------------------------------------------------------------
	Function : bkHeapAllocEx
	Purpose : allocate a memory block with a group other than the current level of the group stack.
	Parameters : block size, file doing the allocation, line number doing the allocation, flags, group id, alignment
	Returns : 
	Info : 
*/

void *bkHeapAllocEx(uint size, char *file, int line, ushort flags, uint32 group, int)
{
	// If heap is not initialised, fall back to CRT
	if (bHeapSize == 0)
	{
		return MakeSystemAlloc(Align16(size), flags);
	}

	uint userSize = Align16(size);
	uint need     = userSize + sizeof(THeapBlock);

	// Free-list walk: small blocks from tail backwards, large from head forwards
	THeapBlock *fit = 0;
	if (userSize < bSmallBlockThreshold)
	{
		for (THeapBlock *p = bFreeRoot.freePrev; p != &bFreeRoot; p = p->freePrev)
		{
			if (p->size >= need) { fit = p; break; }
		}
	}
	else
	{
		for (THeapBlock *p = bFreeRoot.freeNext; p != &bFreeRoot; p = p->freeNext)
		{
			if (p->size >= need) { fit = p; break; }
		}
	}

	if (!fit)
	{
		// Debug info, mirroring the original two-line out-of-memory report
		int largest = 0;
		for (THeapBlock *p = bFreeRoot.freePrev; p != &bFreeRoot; p = p->freePrev)
		{
			int avail = (int)p->size - (int)sizeof(THeapBlock);
			if (avail > largest) largest = avail;
		}
		int totalFree = (int)bHeapSize - (int)bBytesAllocated - (int)sizeof(THeapBlock);
		bkPrintf("%s(%d): bkHeapAlloc: Out of memory asking for %u\n", file ? file : "?", line, (unsigned)userSize);
		bkPrintf("bkHeapAlloc: (only %d bytes free; short by %d) (largest %d; short by %d)\n",
			totalFree, (int)userSize - totalFree, largest, (int)userSize - largest);
		return 0;
	}

	THeapBlock *blk = fit;

	// If big enough, split according to policy
	if (fit->size >= need + kMinSplit)
	{
		if (userSize < bSmallBlockThreshold)
		{
			// SMALL: allocate from FRONT -> remainder placed AFTER
			THeapBlock *rem = (THeapBlock*)(bHeapBase + fit->offset + need);
			rem->offset = fit->offset + need;
			rem->size   = fit->size - need;
			rem->used   = 0;
			rem->flags  = 0;
			rem->group  = group;

			// Insert remainder in physical order between fit->orderPrev and fit
			THeapBlock *op = fit->orderPrev;
			rem->orderPrev = op;
			rem->orderNext = fit;
			op->orderNext  = rem;
			fit->orderPrev = rem;

			// Shrink the allocated part to the exact size
			fit->size = need;

			// Link new remainder into free-list and try coalescing with previous
			ConnectFreeBlock(rem);
			THeapBlock *prev = rem->orderPrev;
			if (prev != bFirstBlock && prev->used == 0)
			{
				// Merge remainder with previous free block
				prev = prev->freePrev;
				rem->freePrev = prev;
				prev->freeNext = rem;
				rem->size += rem->orderPrev->size;
				THeapBlock *opp = rem->orderPrev->orderPrev;
				rem->orderPrev = opp;
				opp->orderNext = rem;
			}
			blk = fit;
		}
		else
		{
			// LARGE: allocate from END -> remainder stays BEFORE
			int prefix = (int)fit->size - (int)need;
			THeapBlock *take = (THeapBlock*)(bHeapBase + fit->offset + prefix);

			// Copy list/flags/group from the old node to the allocated tail node
			take->offset   = fit->offset + prefix;
			take->size     = need;
			take->used     = fit->used;
			take->flags    = fit->flags;
			take->group    = fit->group;

			// Preserve position in free/used list while we relink
			take->freePrev = fit->freePrev;
			take->freeNext = fit->freeNext;
			take->freePrev->freeNext = take;
			take->freeNext->freePrev = take;

			// Insert in physical order between fit->orderPrev and fit
			THeapBlock *op = fit->orderPrev;
			take->orderPrev = op;
			take->orderNext = fit;
			op->orderNext   = take;
			fit->orderPrev  = take;

			// The leftover prefix becomes the free block
			fit->size  = (uint32)prefix;
			fit->used  = 0;
			fit->flags = 0;
			fit->group = group;
			ConnectFreeBlock(fit);

			// Try coalescing with next free neighbor
			if (fit->orderNext != bFirstBlock && fit->orderNext->used == 0)
			{
				// Merge fit with its next neighbor
				UnlinkListNode(fit);
				fit->orderNext->size += fit->size;
				fit->orderNext->orderPrev = fit->orderPrev;
				fit->orderPrev->orderNext = fit->orderNext;
			}
			blk = take;
		}
	}

	// Account & move chosen block to used-list
	bBytesAllocated += blk->size;
	if (bBytesAllocated > bHeapPeakUsage) bHeapPeakUsage = bBytesAllocated;

	// Remove from free-list (if we didn’t allocate from the tail case above, it still sits there)
	UnlinkListNode(blk);

	blk->group = group;
	blk->used  = 1;
	blk->flags = flags;

	LinkUsedBlock(blk, userSize);

	return (void*)(blk + 1);
}


/* --------------------------------------------------------------------------------
   Function : bkHeapCalloc
   Purpose : allocate a memory block and initialise it to a value
   Parameters : block size, value to initialise with, file doing the allocation, line number doing the allocation, flags
   Returns : ptr to block or NULL for failure
   Info : 
*/

void *bkHeapCalloc(uint size, int32 value, char *file, int line, ushort flags)
{
        bkPrintf("*** WARNING *** bkHeapCalloc was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}


/* --------------------------------------------------------------------------------
   Function : bkHeapCallocEx
   Purpose : allocate a memory block and initialise it to a value
   Parameters : block size, value to initialise with, file doing the allocation, line number doing the allocation, flags, group number
   Returns : ptr to block or NULL for failure
   Info : 
*/

void *bkHeapCallocEx(uint size, int32 value, char *file, int line, ushort flags, uint32 group)
{
    // Allocate with the exact parameter order observed (last implicit arg = 0)
    void* p = bkHeapAllocEx(size, file, line, flags, group, 0);
    if (p) {
        // Fill using the low byte of 'value' (same effect as the REP STOSD/STOSB path)
        memset(p, (int)value, size);
    }
    return p;
}



/* --------------------------------------------------------------------------------
   Function : bkHeapFree
   Purpose : free a memory block
   Parameters : ptr to block
   Returns : 
   Info : 
*/

void bkHeapFree(void* user)
{
    if (!user) return;

    // header is exactly 0x20 bytes before user pointer
    THeapBlock* blk = (THeapBlock*)((unsigned char*)user - 0x20);

    // CRT-allocated chunk (orderPrev == NULL) -> free(header) and return
    if (blk->orderPrev == NULL) {
        free(blk);
        return;
    }

    // bookkeeping
    bBytesAllocated -= blk->size;

    // unlink from current free ring (even if previously "used" – matches retail)
    THeapBlock* prev = blk->freePrev;
    THeapBlock* next = blk->freeNext;
    prev->freeNext = next;
    next->freePrev = prev;

    // mark free
    blk->used = 0;

    // insert by offset into free ring
    ConnectFreeBlock(blk);

    // --- coalesce RIGHT (retail: first right, then left)
    if (blk != bFirstBlock) {
        THeapBlock* right = blk->orderNext;
        if (right && right->used == 0) {
            // absorb blk into right
            right->size += blk->size;

            // repair linear chain
            right->orderPrev = blk->orderPrev;
            blk->orderPrev->orderNext = right;

            // move free "anchor" from blk to right (prev side only, as per asm)
            right->freePrev = blk->freePrev;
            blk->freePrev->freeNext = right;

            blk = right; // continue with the merged block
        }
    }

    // --- coalesce LEFT
    THeapBlock* left = blk->orderPrev;
    if (left != bFirstBlock && left && left->used == 0) {
        blk->size += left->size;

        // repair linear chain
        blk->orderPrev = left->orderPrev;
        left->orderPrev->orderNext = blk;

        // unlink left from free ring, reconnect prev -> blk
        blk->freePrev = left->freePrev;
        left->freePrev->freeNext = blk;
    }
}


/*	--------------------------------------------------------------------------------
	Function : bkHeapRealloc
	Purpose : Reallocate a block of memory to become a new size.
	Parameters : [in] the pointer to reallocate
				[in] the new size of the block.
	Returns : pointer to the block (possibly moved) or NULL on error or when newSize == 0.
	Info : 
*/

void *bkHeapRealloc(void *ptr, int32 newSize)
{
	if (ptr == 0)
		return MALLOCEX((uint)newSize, bGroupStack[bGroupSP]);

	if (newSize == 0)
	{
		bkHeapFree(ptr);
		return 0;
	}

	THeapBlock *_Memory = (THeapBlock*)((UINT8*)ptr - sizeof(THeapBlock));
	THeapBlock *orderPrev = _Memory->orderPrev;
	uint userNew = Align16((uint)newSize);
	uint needNew = userNew + sizeof(THeapBlock);

	// System block => CRT path
	if (orderPrev == NULL)
	{
		void *r = realloc(_Memory, needNew);
		return r;
	}

	uint curSize = _Memory->size;

	if (curSize == needNew)
		return ptr;

	if (curSize < needNew)
	{
		// Try to grow backward using previous physical neighbor if it is FREE
		if (orderPrev->used == 0 && (needNew - curSize) <= orderPrev->size)
		{
			THeapBlock *pprev = orderPrev->orderPrev;
			int remainder = (int)(curSize + orderPrev->size) - (int)needNew;
			if (remainder < (int)kMinSplit)
			{
				// Absorb previous entirely
				_Memory->size = curSize + orderPrev->size;
				pprev->orderNext = _Memory;
				_Memory->orderPrev = pprev;

				// Unlink previous from free-list
				orderPrev->freePrev->freeNext = orderPrev->freeNext;
				orderPrev->freeNext->freePrev = orderPrev->freePrev;
				return ptr;
			}

			// Split previous: leave a small free block before our current
			THeapBlock *split = (THeapBlock*)(bHeapBase + _Memory->offset + userNew);
			THeapBlock *fp = orderPrev->freePrev;
			THeapBlock *fn = orderPrev->freeNext;

			// Install the new free block (replacing the old previous)
			split->freePrev = fp;
			split->freeNext = fn;
			fp->freeNext = split;
			fn->freePrev = split;

			// Insert split in physical order between pprev and _Memory
			split->orderPrev = pprev;
			split->orderNext = _Memory;
			pprev->orderNext = split;

			// Update our back link & size/offset of split remainder
			_Memory->orderPrev = split;
			split->offset = _Memory->offset + userNew;
			split->size   = (uint32)remainder;
			split->used   = 0;
			split->flags  = 0;
			split->group  = 0;

			_Memory->size = needNew;
			return ptr;
		}

		// Could not grow in place: allocate+copy
		void *np = MALLOCEX(userNew, _Memory->group);
		if (np)
		{
			uint toCopy = _Memory->size - sizeof(THeapBlock);
			memcpy(np, ptr, toCopy);
			bkHeapFree(ptr);
			return np;
		}
		return 0;
	}
	else
	{
		// Shrink: if prev is used OR remainder is sizable, create trailing free block
		uint remainder = curSize - needNew;
		if (orderPrev->used == 0 || remainder > 0x20)
		{
			THeapBlock *blk = (THeapBlock*)(bHeapBase + _Memory->offset + userNew);
			blk->used   = 0;
			blk->group  = 0;
			blk->flags  = 0;
			blk->offset = _Memory->offset + userNew;
			blk->size   = remainder;

			THeapBlock *prev = _Memory->orderPrev;
			if (prev->used == 0)
			{
				// Merge new trailing free block with previous free
				blk->size += prev->size;
				blk->orderPrev = prev->orderPrev;

				// Remove prev from free-list
				prev->freePrev->freeNext = prev->freeNext;
				prev->freeNext->freePrev = prev->freePrev;
			}
			else
			{
				blk->orderPrev = prev;
			}

			// Stitch blk into physical order before _Memory
			_Memory->orderPrev = blk;
			blk->orderNext = _Memory;
			blk->orderPrev->orderNext = blk;

			// Shrink current
			_Memory->size = needNew;

			// Link new free space
			ConnectFreeBlock(blk);
		}
		return ptr;
	}
}


/*	--------------------------------------------------------------------------------
	Function : bkHeapGetBlockSize
	Purpose : Return size of the memory block
	Parameters : pointer to memory
	Returns : size of block (user area)
	Info : 
*/

int bkHeapGetBlockSize(void *ptr)
{
    return *(int *)((int)ptr + -0x1c) + -0x20; // insert Knuckles idk meme
}


/* --------------------------------------------------------------------------------
   Function : bkHeapShow
   Purpose : send a list of heap blocks to debug output
   Parameters : group to show (or BALLGROUPS), millisecond pause between prints
   Returns : 
   Info : 
*/

void bkHeapShow(uint32 group, int pause)
{
        bkPrintf("*** WARNING *** bkHeapShow was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkHeapShowGraphical
   Purpose : Show a graphical list of heap blocks on screen
   Parameters : group to show (or BALLGROUPS), zoom in from this percentage offset, font
   Returns : 
   Info : 
*/

void bkHeapShowGraphical(uint32 group=BALLGROUPS, float zoom = 0.0f, const struct _TBFont *font=0)
{
        bkPrintf("*** WARNING *** bkHeapShowGraphical was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkHeapStats
   Purpose : show memory usage statistics
   Parameters : 
   Returns : 
   Info : 
*/

// defined in bKernel/heap.h


/* --------------------------------------------------------------------------------
   Function : bkHeapCheck
   Purpose : check allocated blocks for overwriting
   Parameters : 
   Returns : OK=no overwriting, FAIL=overwriting
   Info : 
*/

// defined in bKernel/heap.h


/* --------------------------------------------------------------------------------
   Function : bkHeapFreeGroup
   Purpose : free all memory of a given group
   Parameters : group number
   Returns : 
   Info : 
*/

void bkHeapFreeGroup(uint32 group)
{
        bkPrintf("*** WARNING *** bkHeapFreeGroup was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : bkHeapSetBreakAlloc
   Purpose : set the ID number of a block to break when allocated
   Parameters : block ID
   Returns : 
   Info : 
*/

void bkHeapSetBreakAlloc(uint32 blockId)
{
        bkPrintf("*** WARNING *** bkHeapSetBreakAlloc was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkTakeHeapSnapshot
   Purpose : take a snapshot of the current heap status
   Parameters : group to check. (or BALLGROUPS)
   Returns : ptr to snapshot
   Info : 
*/

// defined in bKernel/heap.h

/* --------------------------------------------------------------------------------
   Function : bkDeleteHeapSnapshot
   Purpose : delete a resource snapshot
   Parameters : ptr to snapshot
   Returns : 
   Info : 
*/

// defined in bKernel/heap.h


/* --------------------------------------------------------------------------------
   Function : bkCompareHeapSnapshot
   Purpose : compare a resource snapshot against the current resource list
   Parameters : snapshot
   Returns : 1=there were differences, 0=they were identical
   Info : 
*/

// defined in bKernel/heap.h


/*	--------------------------------------------------------------------------------
	Function : bkHeapGroupPush
	Purpose : Push a new group onto the heap group stack. This then becomes the current heap group.
	Parameters : the group to push.
	Returns : 1 if successful, 0 otherwise.
	Info : 
*/

int bkHeapGroupPush(const char * const group)
{
	if (bGroupSP == 0x1f) return 0; // MG: what??????
	bGroupSP++;
	bGroupStack[bGroupSP] = (uint32)group;
	return 1;
}


/*	--------------------------------------------------------------------------------
	Function : bkHeapGroupPop
	Purpose : Pop the current group from the heap.
	Parameters : 
	Returns : 1 on success, 0 otherwise.
	Info : 
*/

int bkHeapGroupPop(void)
{
	if (!bGroupSP) return 0;
	bGroupSP--;
	return 1;
}


/*	--------------------------------------------------------------------------------
	Function : bkHeapFreeSpace
	Purpose : Reports the space that is left to fill in the heap.
	Parameters : ptr to fill with largest free block or NULL
	Returns : total bytes free
	Info : 
*/

int bkHeapFreeSpace(int *largestFreeBlock /*=NULL*/)
{
	if (largestFreeBlock)
	{
		int largest = 0;
		for (THeapBlock *p = bFreeRoot.freePrev; p != &bFreeRoot; p = p->freePrev)
		{
			int avail = (int)p->size - (int)sizeof(THeapBlock);
			if (avail > largest) largest = avail;
		}
		*largestFreeBlock = largest;
	}
	return (int)bHeapSize - (int)bBytesAllocated - (int)sizeof(THeapBlock);
}

/*	--------------------------------------------------------------------------------
	Function : bGetCurrentGroup
	Purpose : Gets the current group id
	Parameters : 
	Returns : the current group id from thr top of the stack.
	Info : 
*/

uint32 bGetCurrentGroup(void)
{
	return bGroupStack[bGroupSP];
}


/* --------------------------------------------------------------------------------
   Function : bkShowHeapStructure
   Purpose : show the used/free block structure of the heap
   Parameters : 
   Returns : 
   Info : 
*/

void bkShowHeapStructure()
{
        bkPrintf("*** WARNING *** bkShowHeapStructure was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkIsHeapBlock
   Purpose : determine if a pointer is a valid heap block
   Parameters : ptr to test
   Returns : TRUE if is a heap block, FALSE otherwise
   Info : 
*/

int bkIsHeapBlock(void *ptr)
{
        bkPrintf("*** WARNING *** bkIsHeapBlock was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkGetSystemFreeMemory
   Purpose : return the amount of system RAM that is free (outside the Babel heap)
   Parameters : 
   Returns : #bytes free or -1 for failure
   Info : 
*/

int bkGetSystemFreeMemory()
{
        bkPrintf("*** WARNING *** bkGetSystemFreeMemory was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bkHeapSetLargeBlockThreshold
	Purpose : Set threshold size at which blocks are allocated from the end of the heap (>=size)
	Parameters : new threshold value
	Returns : 
	Info : 
*/

void bkHeapSetLargeBlockThreshold(uint value)
{
        bkPrintf("*** WARNING *** bkHeapSetLargeBlockThreshold was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bkHeapGetLargeBlockThreshold
	Purpose : Get threshold size at which blocks are allocated from the end of the heap (>=size)
	Parameters : 
	Returns : current threshold value
	Info : The default is currently 1024*1024 (1M)
*/

uint bkHeapGetLargeBlockThreshold()
{
        bkPrintf("*** WARNING *** bkHeapGetLargeBlockThreshold was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}