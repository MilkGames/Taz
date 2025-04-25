// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : heap.h
//   Purpose : memory management and heap control
// Component : Generic Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BKERNEL_HEAP_H__
#define __BKERNEL_HEAP_H__


#if defined _DEBUG || BPLATFORM==GAMECUBE
#define MEMORY_DEBUG
#endif

// ********************************************************************************
// Prototypes

void *bkHeapAlloc(uint size, char *file, int line, ushort flags);
void *bkHeapAllocEx(uint size, char *file, int line, ushort flags, uint32 group, int alignment=0);
#ifdef MEMORY_DEBUG
void bkHeapFree(void *blk, char *name=0);
#else
void bkHeapFree(void *blk);
#endif


// ********************************************************************************
// Constants and Macros



// module IDs
enum
{
	bExternalModule,
	bUserModule,
	bActorModule,
	bDisplayModule,
	bGUIModule,
	bInputModule,
	bKernelModule,
	bMathsModule,
	bSoundModule,
	fCameraModule,
	fGUIModule,
	fSceneModule,
	fEffectsModule,

	bModuleCount
};

// heap block flags (low byte contains module)
#define BALLOC_NEW			0x1000		// block was allocated by 'new' operator
#define BALLOC_MALLOC		0x2000		// block was allocated by malloc
#define BALLOC_UNMARKED		0x4000		// block has no deadzone marks

// overload 'new' operator to use our heap routines
inline void *operator new(size_t size) { return bkHeapAlloc(size, "External Object", 0, BALLOC_NEW | bExternalModule); }
inline void *operator new[](size_t size) { return bkHeapAlloc(size, "External Object", 0, BALLOC_NEW | bExternalModule); }
inline void *operator new(size_t size, char *file, int line, ushort flags) { return bkHeapAlloc(size, file, line, flags); }
inline void *operator new[](size_t size, char *file, int line, ushort flags) { return bkHeapAlloc(size, file, line, flags); }

// overload 'delete' operator to use our heap routines
inline void operator delete(void *block, char *file, int line, ushort flags) { bkHeapFree(block); }
inline void operator delete(void *block) { bkHeapFree(block); }
inline void operator delete[](void *block, char *file, int line, ushort flags) { bkHeapFree(block); }
inline void operator delete[](void *block) { bkHeapFree(block); }

#define MALLOCEX(SIZE, GROUP)			bkHeapAllocEx(SIZE, __FILE__, __LINE__, BALLOC_MALLOC | bUserModule,  GROUP)
#define CALLOCEX(SIZE, VALUE, GROUP)	bkHeapCallocEx(SIZE, VALUE, __FILE__, __LINE__, BALLOC_MALLOC | bUserModule,  GROUP)

#define REALLOC(PTR, SIZE)				bkHeapRealloc(PTR, SIZE)

#ifdef MEMORY_DEBUG
#define FREE(PTR)						{ bkHeapFree(PTR,#PTR); (PTR) = NULL; }
#else
#define FREE(PTR)						{ bkHeapFree(PTR); (PTR) = NULL; }
#endif

#define BSAFEDELETE(PTR)				{ if (PTR) { delete (PTR); (PTR) = NULL; } }

// macros for memory allocation and release
#include "heapNew.h"

// byte unused memory is filled with
#define BAB_HEAP_INIT_BYTE	0xed

// special value to indicate all groups should be included
#define BALLGROUPS			((ushort)0xffff)


// ********************************************************************************
// Types

// an entry in a resource snapshot structure
typedef struct _TBHeapSnapshotEntry {
	int32				offset;					// offset to start of block
	uint32				size;					// size of block (including header)
	uint32				group;					// group this block belongs to
	int32				lineno;					// line number where this block was malloc'd
	uint32				uid;					// malloc Unique ID
	char				*file;					// filename where this block was malloc'd
	void				*ptr;					// ptr to block
} TBHeapSnapshotEntry;


// a resource snapshot structure (followed by array of TBResourceSnapshotEntry structs to make a snapshot)
typedef struct _TBHeapSnapshot {
	uint32				group;					// group used to gather the snapshot
	int32				noofEntries;			// total number of entries in snapshot
#if (BPLATFORM == PC) || (BPLATFORM == XBOX)
	_CrtMemState		sysCheckpoint;			// system memory checkpoint
#endif
} TBHeapSnapshot;


// ********************************************************************************
// Globals

#if BPLATFORM == GAMECUBE
extern OSHeapHandle	bOSHeap;					// Global heap fo OSAlloc calls
#endif


// ********************************************************************************
// Prototypes


/* --------------------------------------------------------------------------------
   Function : bInitHeap
   Purpose : initialise the heap
   Parameters : base address (NULL for dynamic), size of heap
   Returns : OK/FAIL
   Info : 
*/

int bInitHeap(uint base, uint size);


/* --------------------------------------------------------------------------------
   Function : bShutdownHeap
   Purpose : shutdown the global heap
   Parameters : 
   Returns : 
   Info : 
*/

void bShutdownHeap();


/* --------------------------------------------------------------------------------
   Function : bkHeapReset
   Purpose : reset the heap to be empty
   Parameters : 
   Returns : 
   Info : 
*/

void bkHeapReset();


/* --------------------------------------------------------------------------------
   Function : bkHeapAlloc
   Purpose : allocate a memory block
   Parameters : block size, file doing the allocation, line number doing the allocation, flags
   Returns : ptr to block or NULL for failure
   Info : 
*/

void *bkHeapAlloc(uint size, char *file, int line, ushort flags);


/*	--------------------------------------------------------------------------------
	Function : bkHeapAllocEx
	Purpose : allocate a memory block with a group other than the current level of the group stack.
	Parameters : block size, file doing the allocation, line number doing the allocation, flags, group id, alignment
	Returns : 
	Info : 
*/

void *bkHeapAllocEx(uint size, char *file, int line, ushort flags, uint32 group, int alignment);


/* --------------------------------------------------------------------------------
   Function : bkHeapCalloc
   Purpose : allocate a memory block and initialise it to a value
   Parameters : block size, value to initialise with, file doing the allocation, line number doing the allocation, flags
   Returns : ptr to block or NULL for failure
   Info : 
*/

void *bkHeapCalloc(uint size, int32 value, char *file, int line, ushort flags);


/* --------------------------------------------------------------------------------
   Function : bkHeapCallocEx
   Purpose : allocate a memory block and initialise it to a value
   Parameters : block size, value to initialise with, file doing the allocation, line number doing the allocation, flags, group number
   Returns : ptr to block or NULL for failure
   Info : 
*/

void *bkHeapCallocEx(uint size, int32 value, char *file, int line, ushort flags, uint32 group);


/* --------------------------------------------------------------------------------
   Function : bkHeapFree
   Purpose : free a memory block
   Parameters : ptr to block
   Returns : 
   Info : 
*/

#ifdef MEMORY_DEBUG
void bkHeapFree(void *blk, char *name);
#else
void bkHeapFree(void *blk);
#endif


/*	--------------------------------------------------------------------------------
	Function : bkHeapRealloc
	Purpose : Reallocate a block of memory to become a new size.
	Parameters : [in] the pointer to reallocate
				[in] the new size of the block.
	Returns : pointer to the block (possibly moved) or NULL on error or when newSize == 0.
	Info : 
*/

void *bkHeapRealloc(void *ptr, int32 newSize);


/*	--------------------------------------------------------------------------------
	Function : bkHeapGetBlockSize
	Purpose : Return size of the memory block
	Parameters : pointer to memory
	Returns : size of block (user area)
	Info : 
*/

int bkHeapGetBlockSize(void *ptr);


/* --------------------------------------------------------------------------------
   Function : bkHeapShow
   Purpose : send a list of heap blocks to debug output
   Parameters : group to show (or BALLGROUPS), millisecond pause between prints
   Returns : 
   Info : 
*/

void bkHeapShow(uint32 group=BALLGROUPS, int pause=0);


/* --------------------------------------------------------------------------------
   Function : bkHeapShowGraphical
   Purpose : Show a graphical list of heap blocks on screen
   Parameters : group to show (or BALLGROUPS), zoom in from this percentage offset, font
   Returns : 
   Info : 
*/

void bkHeapShowGraphical(uint32 group=BALLGROUPS, float zoom = 0.0f, const struct _TBFont *font=0);


/* --------------------------------------------------------------------------------
   Function : bkHeapStats
   Purpose : show memory usage statistics
   Parameters : 
   Returns : 
   Info : 
*/

#ifdef MEMORY_DEBUG
	void bkHeapStats();
#else
	#define bkHeapStats()
#endif // #ifdef MEMORY_DEBUG


/* --------------------------------------------------------------------------------
   Function : bkHeapCheck
   Purpose : check allocated blocks for overwriting
   Parameters : 
   Returns : OK=no overwriting, FAIL=overwriting
   Info : 
*/

#ifdef MEMORY_DEBUG
	int bkHeapCheck();
#else
	#define bkHeapCheck()
#endif	// #ifdef MEMORY_DEBUG


/* --------------------------------------------------------------------------------
   Function : bkHeapFreeGroup
   Purpose : free all memory of a given group
   Parameters : group number
   Returns : 
   Info : 
*/

void bkHeapFreeGroup(uint32 group);


/* --------------------------------------------------------------------------------
   Function : bkHeapSetBreakAlloc
   Purpose : set the ID number of a block to break when allocated
   Parameters : block ID
   Returns : 
   Info : 
*/

void bkHeapSetBreakAlloc(uint32 blockId=0);


/* --------------------------------------------------------------------------------
   Function : bkTakeHeapSnapshot
   Purpose : take a snapshot of the current heap status
   Parameters : group to check. (or BALLGROUPS)
   Returns : ptr to snapshot
   Info : 
*/

#ifdef MEMORY_DEBUG
	TBHeapSnapshot *bkTakeHeapSnapshot(uint32 snapGroup);
#else
	#define bkTakeHeapSnapshot  BNULLFUNCTION
#endif // MEMORY_DEBUG


/* --------------------------------------------------------------------------------
   Function : bkDeleteHeapSnapshot
   Purpose : delete a resource snapshot
   Parameters : ptr to snapshot
   Returns : 
   Info : 
*/

#ifdef MEMORY_DEBUG
	void bkDeleteHeapSnapshot(TBHeapSnapshot *snapshot);
#else
	#define bkDeleteHeapSnapshot BNULLFUNCTION
#endif // MEMORY_DEBUG


/* --------------------------------------------------------------------------------
   Function : bkCompareHeapSnapshot
   Purpose : compare a resource snapshot against the current resource list
   Parameters : snapshot
   Returns : 1=there were differences, 0=they were identical
   Info : 
*/

#ifdef MEMORY_DEBUG
	int bkCompareHeapSnapshot(TBHeapSnapshot *snap);
#else
	#define bkCompareHeapSnapshot BNULLFUNCTION_RET
#endif


/*	--------------------------------------------------------------------------------
	Function : bkHeapGroupPush
	Purpose : Push a new group onto the heap group stack. This then becomes the current heap group.
	Parameters : the group to push.
	Returns : 1 if successful, 0 otherwise.
	Info : 
*/

int bkHeapGroupPush(const char * const group);


/*	--------------------------------------------------------------------------------
	Function : bkHeapGroupPop
	Purpose : Pop the current group from the heap.
	Parameters : 
	Returns : 1 on success, 0 otherwise.
	Info : 
*/

int bkHeapGroupPop(void);


/*	--------------------------------------------------------------------------------
	Function : bkHeapFreeSpace
	Purpose : Reports the space that is left to fill in the heap.
	Parameters : ptr to fill with largest free block or NULL
	Returns : total bytes free
	Info : 
*/

int bkHeapFreeSpace(int *largestFreeBlock=NULL);


/*	--------------------------------------------------------------------------------
	Function : bGetCurrentGroup
	Purpose : Gets the current group id
	Parameters : 
	Returns : the current group id from thr top of the stack.
	Info : 
*/

uint32 bGetCurrentGroup(void);


/* --------------------------------------------------------------------------------
   Function : bkShowHeapStructure
   Purpose : show the used/free block structure of the heap
   Parameters : 
   Returns : 
   Info : 
*/

void bkShowHeapStructure();


/* --------------------------------------------------------------------------------
   Function : bkIsHeapBlock
   Purpose : determine if a pointer is a valid heap block
   Parameters : ptr to test
   Returns : TRUE if is a heap block, FALSE otherwise
   Info : 
*/

int bkIsHeapBlock(void *ptr);


/* --------------------------------------------------------------------------------
   Function : bkGetSystemFreeMemory
   Purpose : return the amount of system RAM that is free (outside the Babel heap)
   Parameters : 
   Returns : #bytes free or -1 for failure
   Info : 
*/

int bkGetSystemFreeMemory();


/*	--------------------------------------------------------------------------------
	Function : bkHeapSetLargeBlockThreshold
	Purpose : Set threshold size at which blocks are allocated from the end of the heap (>=size)
	Parameters : new threshold value
	Returns : 
	Info : 
*/

void bkHeapSetLargeBlockThreshold(uint value);


/*	--------------------------------------------------------------------------------
	Function : bkHeapGetLargeBlockThreshold
	Purpose : Get threshold size at which blocks are allocated from the end of the heap (>=size)
	Parameters : 
	Returns : current threshold value
	Info : The default is currently 1024*1024 (1M)
*/

uint bkHeapGetLargeBlockThreshold();


#endif		// __BKERNEL_HEAP_H__