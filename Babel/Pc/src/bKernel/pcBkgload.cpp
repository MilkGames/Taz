// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcBkgload.cpp
//   Purpose : Background loading support
// Component : Babel Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Globals

volatile int bBytesTransferred = 0;
volatile int bLastBytesTransferred = 0;

// ********************************************************************************
// Locals

TBBkgLoadCmd bBkgLoadCmd[BKG_SLOT_COUNT];   // base of the N-slot queue
TBMutex      bBkgMutex;						// created/deleted here
HANDLE       bBkgKickEvent;					// CreateEventA("BabelKickWorkerThread")
volatile int bBkgStop;						// 0 -> run, 1 -> stop
volatile int bBkgThreadRunning;				// set by worker while alive

// ********************************************************************************
// Helper Functions

// CreateThread entry (yep, we need __stdcall for that)
DWORD __stdcall bKernelWorkerThread(void *)
{
    int spinTries = 0;
    bBkgThreadRunning = 1;

    if (bBkgStop) {
        bBkgThreadRunning = 0;
        return 0;
    }

    for (;;) {
        // wait to be kicked
        WaitForSingleObject(bBkgKickEvent, INFINITE);
        if (bBkgStop) break;

        // throttle: ticks per KiB = (freq / 0x06400000) * 0x400  (all 64-bit)
        TBTimerValue freq        = bkTimerGetFrequency();
        TBTimerValue ticksPerKiB = (freq / 0x06400000) * 0x400;

        spinTries = 0;

        for (;;) {
            if (bBkgStop) goto exit_worker;

            if (bkWaitMutex(&bBkgMutex) != 1) {
                bkSleep(10);
                if (++spinTries != BKG_SLOT_COUNT)
                    continue;
                // give up this cycle; back to outer WaitForSingleObject
                break;
            }

            // find first occupied slot (channel != -1)
            int slot = -1;
            int checked = 0;
            {
                int i;
                for (i = 0; i < BKG_SLOT_COUNT; ++i) {
                    if (*(int*)&bBkgLoadCmd[i].channel != -1) { slot = i; break; }
                    ++checked;
                }
            }

            if (slot < 0) {
                bkReleaseMutex(&bBkgMutex);
                if (checked == BKG_SLOT_COUNT) break; // nothing to do — go back to wait
                continue;                              // retry within this wake
            }

            // ================== process the job at 'slot' ==================
            TBFileHandle fh = 0;
            if (!bkOpenFileReadOnlyWithSearch(bBkgLoadCmd[slot].filename, &fh, 0, 0, 0)) {
                bkPrintf("  > bKernelWorkerThread: could not open file '%s' for bkg load\n",
                         bBkgLoadCmd[slot].filename);
                bBkgLoadCmd[slot].resultCode = EBBKGERROR_NOTFOUND;
                bkReleaseMutex(&bBkgMutex);
            } else {
                const int fileLen = bFileLength(fh);

                if (bBkgLoadCmd[slot].noofBytes == 0)
                    bBkgLoadCmd[slot].noofBytes = fileLen;

                void* dst = bBkgLoadCmd[slot].fp;
                const int blk = (dst != 0) ? bkHeapGetBlockSize(dst) : 0;

                if (dst == 0 || blk < fileLen) {
                    bkPrintf("Load data area too small %d < %d\n", blk, fileLen);
                    bBkgLoadCmd[slot].resultCode = EBBKGERROR_LOADBUFFERTOSMALL;
                    bkReleaseMutex(&bBkgMutex);
                    fclose((FILE*)fh);
                } else {
                    // Prepare IO, then release the mutex for long read
                    fseek((FILE*)fh, bBkgLoadCmd[slot].offset, SEEK_SET);
                    unsigned       remaining = (unsigned)bBkgLoadCmd[slot].noofBytes;
                    unsigned char* out       = (unsigned char*)dst;

                    bkReleaseMutex(&bBkgMutex);

                    TBTimerValue t0 = bkTimerRead();
                    int copied = 0;
                    bLastBytesTransferred = 0;

                    while (remaining && !bBkgStop) {
                        unsigned chunk = remaining;
                        if ((int)chunk > BKG_CHUNK_SIZE) chunk = BKG_CHUNK_SIZE;

                        fread(out, 1, chunk, (FILE*)fh);

                        remaining -= chunk;
                        out       += chunk;
                        copied    += (int)chunk;

                        // progress (flags bit0)
                        if ((bBkgLoadCmd[slot].flags & 1) != 0) {
                            bBytesTransferred     += (int)chunk;
                            bLastBytesTransferred += (int)chunk;
                        }

                        // throttle to target: need = (copied>>10) * ticksPerKiB
                        TBTimerValue need = ((TBTimerValue)(copied >> 10)) * ticksPerKiB;
                        TBTimerValue now  = bkTimerRead();
                        TBTimerValue got  = bkTimerDelta(t0, now);

                        if (got < need) {
                            TBTimerValue spare = need - got;
                            // convert ticks->ms: (spare * 1000) / freq
                            TBTimerValue ms64 = (spare * 1000) / freq;
                            int ms = (int)ms64;
                            if (ms > 0) bkSleep(ms);

                            // fine-wait: yield until got >= need
                            do {
                                bkSleep(0);
                                now = bkTimerRead();
                                got = bkTimerDelta(t0, now);
                            } while (got < need);
                        }
                    }

                    fclose((FILE*)fh);
                }
            }

            // ------------------ post stage (same slot) ------------------
            {
                int      res   = (int)bBkgLoadCmd[slot].resultCode;
                unsigned cflag = (unsigned)(*(int*)&bBkgLoadCmd[slot].channel); // low8=channel; bit8=keep

                if (res == 0) {
                    // if channel == 1 > fixup package
                    if ( (cflag & BKG_CHAN_MASK) == 1 ) {
                        uchar* base = (uchar*)bBkgLoadCmd[slot].fp;
                        if (bFixupPackage(bBkgLoadCmd[slot].filename,
                                          (int)(cflag & BKG_CHAN_MASK), base) == 0) {
                            // NOTE: binary writes numeric 4 here; in our headers that maps to EBBKGERROR_PACKAGEEMPTY.
                            bBkgLoadCmd[slot].resultCode = EBBKGERROR_PACKAGEEMPTY;
                            res = (int)bBkgLoadCmd[slot].resultCode;
                        }
                    }
                } else {
                    // on error: free buffer unless KEEPBUFFER flag set in the 'channel' dword
                    if ( (cflag & BKG_FLAG_KEEPBUFFER) == 0 ) {
                        void* p = bBkgLoadCmd[slot].fp;
                        if (p) bkHeapFree(p);
                        bBkgLoadCmd[slot].fp = 0;
                    }
                }

                // strip extension from target for packages
                if ( (cflag & BKG_CHAN_MASK) == 1 ) {
                    char* dot = strrchr(bBkgLoadCmd[slot].target, '.');
                    if (dot) *dot = '\0';
                }

                // raise event with payload { resultCode, crc } (2 DWORDs)
                struct { int result; unsigned crc; } payload;
                payload.result = (int)bBkgLoadCmd[slot].resultCode;
                payload.crc    = bBkgLoadCmd[slot].crc;

                bkGenerateEvent(bBkgLoadCmd[slot].eventName,
                                bBkgLoadCmd[slot].target,
                                &payload, 1);

                // free the slot (write -1 to the dword that includes 'channel')
                if (bkWaitMutex(&bBkgMutex) == 1) {
                    *(int*)&bBkgLoadCmd[slot].channel = -1;
                    bkReleaseMutex(&bBkgMutex);
                }
            }

            // try to pick next job in this wake cycle
            continue;
        } // inner loop
    } // outer loop

exit_worker:
    bBkgThreadRunning = 0;
    return 0;
}

static uchar* EnsureAllocBkg(uchar* buf, int sizeBytes)
{
    // Resolve group: if current is BDEFAULTGROUP > use literal "Package"
    uint32 group = bGetCurrentGroup();
    if (group == BDEFAULTGROUP)
        group = (uint32)"Package";

    if (!buf) {
        uchar* p = (uchar*)MALLOCEX((uint)sizeBytes, group);
        if (!p) {
            bkPrintf("EnsureAllocBkg: *** Out of memory on Babel heap for file (need %d bytes) ***\n",
                     sizeBytes);
        }
        return p;
    }

    const int blkSize = bkHeapGetBlockSize(buf);
    if (blkSize >= sizeBytes)
        return buf;

    // Not enough space in provided buffer
    return NULL;
}

// NOTE: first parameter is a path string with extension (e.g. "<name>.pc")
static TBPackageIndex* LoadSingleFileBkg(char* filenameWithExt,
                                         uchar* dataPtr,
                                         int*   outSizeBytes,
                                         char*  eventName,
                                         int    channel,
                                         uint   crc)
{
    TBPackageIndex* dest = (TBPackageIndex*)dataPtr;
    uint sizeBytes = 0;

    // If caller wants size back OR no prebuffer provided — open to get exact size
    if (outSizeBytes != NULL || dataPtr == NULL) {
        TBFileHandle fh = 0;
        if (!bkOpenFileReadOnlyWithSearch(filenameWithExt, &fh, NULL, 0, 0)) {
            bkPrintf("LoadSingleFileBkg: *** Could not load file '%s' ***\n", filenameWithExt);
            return NULL;
        }
        sizeBytes = (uint)bFileLength(fh);
        bkCloseFile(fh);

        dest = (TBPackageIndex*)EnsureAllocBkg((uchar*)dest, (int)sizeBytes);
        if (!dest) {
            bkPrintf("EnsureAllocBkg: *** Out of memory on Babel heap for file (need %d bytes) ***\n",
                     (int)sizeBytes);
            return NULL;
        }
    }

    // Queue background job
    if (bkWaitMutex(&bBkgMutex) != 1) {
        if (outSizeBytes) *outSizeBytes = (int)sizeBytes;
        return dest;
    }

    int slot = -1;
    for (int i = 0; i < BKG_SLOT_COUNT; ++i) {
        if (*(int*)&bBkgLoadCmd[i].channel == -1) { slot = i; break; }
    }
    if (slot < 0) {
        bkReleaseMutex(&bBkgMutex);
        if (outSizeBytes) *outSizeBytes = (int)sizeBytes;
        return dest;
    }

    // Strings
    bBkgLoadCmd[slot].filename[0] = 0;
    bBkgLoadCmd[slot].target[0]   = 0;
    bBkgLoadCmd[slot].eventName[0]= 0;

    strncpy(bBkgLoadCmd[slot].filename, filenameWithExt, sizeof(bBkgLoadCmd[slot].filename)-1);
    bBkgLoadCmd[slot].filename[sizeof(bBkgLoadCmd[slot].filename)-1] = 0;

    strncpy(bBkgLoadCmd[slot].target,   filenameWithExt, sizeof(bBkgLoadCmd[slot].target)-1);
    bBkgLoadCmd[slot].target[sizeof(bBkgLoadCmd[slot].target)-1] = 0;

    if (eventName) {
        strncpy(bBkgLoadCmd[slot].eventName, eventName, sizeof(bBkgLoadCmd[slot].eventName)-1);
        bBkgLoadCmd[slot].eventName[sizeof(bBkgLoadCmd[slot].eventName)-1] = 0;
    } else {
        bBkgLoadCmd[slot].eventName[0] = 0;
    }

    // Numeric fields (match worker expectations)
    bBkgLoadCmd[slot].offset     = 0;
    bBkgLoadCmd[slot].noofBytes  = (int)sizeBytes;
    bBkgLoadCmd[slot].crc        = crc;
    bBkgLoadCmd[slot].fp         = dest;                 // destination buffer
    *(int*)&bBkgLoadCmd[slot].channel = channel;              // ((dataPtr ? 0x100:0) | 1)
    bBkgLoadCmd[slot].resultCode = EBBKGERROR_NONE;
    if (outSizeBytes) bBkgLoadCmd[slot].flags = 1;       // mirrors retail “set if size requested”

    SetEvent(bBkgKickEvent);
    bkReleaseMutex(&bBkgMutex);

    if (outSizeBytes) *outSizeBytes = (int)sizeBytes;
    return dest;
}

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bKernelInitBkgLoad
	Purpose : Initialise background loading resources
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/

int bKernelInitBkgLoad()
{
    // mark all 16 slots as free (-1 at +0x12C)
    for (int i = 0; i < BKG_SLOT_COUNT; ++i) {
        *(int*)&bBkgLoadCmd[i].channel = BKG_SLOT_FREE;
    }

    bBkgThreadRunning = 0;
    bBkgStop = 0;

    // create the mutex
    if (!bkCreateMutex(&bBkgMutex))
        return FAIL;

    // kick event (auto-reset, initially non-signaled)
    bBkgKickEvent = CreateEventA(NULL, FALSE, FALSE, "BabelKickWorkerThread");

    // start worker thread and raise its priority a bit
    DWORD tid = 0;
    HANDLE th = CreateThread(NULL, 0, bKernelWorkerThread, NULL, 0, &tid);
    SetThreadPriority(th, THREAD_PRIORITY_ABOVE_NORMAL);
    CloseHandle(th); // we don't need the handle (shutdown waits via bBkgThreadRunning)

    return OK;
}

/*	--------------------------------------------------------------------------------
	Function : bKernelShutdownBkgLoad
	Purpose : Shutdown background loading resources
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/

int bKernelShutdownBkgLoad()
{
    // ask worker to stop and wake it if sleeping
    bBkgStop = 1;

	bkPrintf("bKernelShutdownBkgLoad: stopping worker thread...\n");
    SetEvent(bBkgKickEvent);

    // wait until the worker clears its running flag
    while (bBkgThreadRunning != 0) {
        bkSleep(10); // small yield
    }

	bkPrintf("bKernelShutdownBkgLoad: worker thread stopped\n");
    CloseHandle(bBkgKickEvent);

    return bkDeleteMutex(&bBkgMutex) != 0;
}

/* --------------------------------------------------------------------------------
   Function : bkLoadFileBkg
   Purpose : load a file from a package / disc in the background
   Parameters : package index ptr, filename, ptr to buffer (or NULL for dynamic), ptr for returned size (can be NULL), ptr for tag info (or NULL), event to trigger when complete, resource type
   Returns : ptr to buffer or NULL for failure
   Info : package index = NULL for non-package files
*/

uchar *bkLoadFileBkg(TBPackageIndex *index, char *filename, uchar *dataPtr, int *retSize, TBFileTagInfo *tagInfo, char *eventName)
{
        bkPrintf("*** WARNING *** bkLoadFileBkg was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}


/* --------------------------------------------------------------------------------
   Function : bkLoadPackageBkg
   Purpose : load a package file in the background
   Parameters : parent package index (or NULL), package filename, event to generate when complete, pointer to return size, pointer to load location (defaults to NULL)
   Returns : handle to package (ptr to index)
   Info : 
*/

TBPackageIndex *bkLoadPackageBkg(TBPackageIndex *parentIndex, char *filename, char *eventName, int *retSize, uchar *dataPtr)
{
    // Build "<name>.pc"
    char nameWithExt[260];
    sprintf(nameWithExt, "%s%s", filename, BPACKAGE_EXT);

    const uint crc = bkStringCRC(filename);

    // No parent > load single file
    if (!parentIndex) {
        int ch = ((dataPtr != NULL) ? 0x100 : 0) | 1;
        return LoadSingleFileBkg(nameWithExt, dataPtr, retSize, eventName, ch, crc);
    }

    // Inside a package
    int* entry = (int*)bFindIndexFile(parentIndex, nameWithExt);
    if (!entry) return NULL;

    // RAM package (id < 0) > return direct pointer into parent memory
    if (*(int*)&parentIndex->id < 0) {
        TBBkgLoadResult payload; payload.resultCode = EBBKGERROR_NONE; payload.crc = crc;
        bkGenerateEvent(eventName ? eventName : "", filename, &payload, 1);

        // base + PAU * ordinal
        return (TBPackageIndex*)((uchar*)parentIndex->fp + (parentIndex->pauSize * (*entry)) * 320);
    }

    // On-disk: ensure/allocate destination buffer of exact size from index
    uint sizeBytes = (uint)entry[2];
    TBPackageIndex* dest = (TBPackageIndex*)EnsureAllocBkg((uchar*)dataPtr, (int)sizeBytes);
    if (!dest) {
        bkPrintf("EnsureAllocBkg: *** Out of memory on Babel heap for file (need %d bytes) ***\n",
                 (int)sizeBytes);
        return NULL;
    }

    // Queue a package read job
    if (bkWaitMutex(&bBkgMutex) != 1) {
        if (retSize) *retSize = (int)sizeBytes;
        return dest;
    }

    int slot = -1;
    for (int i = 0; i < BKG_SLOT_COUNT; ++i) {
        if (*(int*)&bBkgLoadCmd[i].channel == BKG_SLOT_FREE) { slot = i; break; }
    }
    if (slot < 0) {
        bkReleaseMutex(&bBkgMutex);
        if (retSize) *retSize = (int)sizeBytes;
        return dest;
    }

    // Compose parent pak filename "<parent>.pc"
    char parentPak[260];
    strncpy(parentPak, parentIndex->pakFilename, sizeof(parentPak)-1);
    parentPak[sizeof(parentPak)-1] = 0;
    strncat(parentPak, BPACKAGE_EXT, sizeof(parentPak)-1 - strlen(parentPak));

    // Strings
    bBkgLoadCmd[slot].filename[0] = 0;
    bBkgLoadCmd[slot].target[0]   = 0;
    bBkgLoadCmd[slot].eventName[0]= 0;

    strncpy(bBkgLoadCmd[slot].filename, parentPak, sizeof(bBkgLoadCmd[slot].filename)-1);
    bBkgLoadCmd[slot].filename[sizeof(bBkgLoadCmd[slot].filename)-1] = 0;

    strncpy(bBkgLoadCmd[slot].target,   filename,  sizeof(bBkgLoadCmd[slot].target)-1);
    bBkgLoadCmd[slot].target[sizeof(bBkgLoadCmd[slot].target)-1] = 0;

    if (eventName) {
        strncpy(bBkgLoadCmd[slot].eventName, eventName, sizeof(bBkgLoadCmd[slot].eventName)-1);
        bBkgLoadCmd[slot].eventName[sizeof(bBkgLoadCmd[slot].eventName)-1] = 0;
    }

    // Numerics
    bBkgLoadCmd[slot].offset     = (parentIndex->pauSize * (*entry)) * 320;   // byte offset
    bBkgLoadCmd[slot].noofBytes  = (int)sizeBytes;
    bBkgLoadCmd[slot].crc        = crc;
    bBkgLoadCmd[slot].fp         = dest;                               // destination buffer
    // channel in retail stays (dataPtr?0x100:0) | 1 as well
   *(int*)&bBkgLoadCmd[slot].channel = ((dataPtr != NULL) ? 0x100 : 0) | 1;
    bBkgLoadCmd[slot].resultCode = EBBKGERROR_NONE;
    if (retSize) bBkgLoadCmd[slot].flags = 1;

    SetEvent(bBkgKickEvent);
    bkReleaseMutex(&bBkgMutex);

    if (retSize) *retSize = (int)sizeBytes;
    return dest;
}


/* --------------------------------------------------------------------------------
   Function : bkSetBkgLoadSpeed
   Purpose : set the desired background loading transfer speed
   Parameters : desired speed in Kb per seconds
   Returns : 
   Info : 
*/

void bkSetBkgLoadSpeed(int kbPerSec)
{
        bkPrintf("*** WARNING *** bkSetBkgLoadSpeed was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bIsBkgChannelBusy
   Purpose : determine if a background scheduler channel is busy
   Parameters : channel
   Returns : TRUE if busy, FALSE otherwise
   Info : 
*/

int bIsBkgChannelBusy(EBBkgChannel channel)
{
        bkPrintf("*** WARNING *** bIsBkgChannelBusy was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bBkgChannelCancelRequest
   Purpose : cancel channel request (if cancelable)
   Parameters : channel
   Returns : 
   Info : 
*/

void bBkgChannelCancelRequest(EBBkgChannel channel)
{
        bkPrintf("*** WARNING *** bBkgChannelCancelRequest was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bQueueBackgroundLoad
	Purpose : queue a background load
	Parameters : channel, target area, handle of file, 
				 on disk filename, requested filename crc,
	             start offset, no of bytes, flags (see BBKGFLAG_*)
				 completion event, resource type
	Returns : OK/FAIL
	Info : Uses filename only if file handle is not set
*/

int bQueueBackgroundLoad(EBBkgChannel channel, char *dest, HANDLE fp, 
						 char *filename, uint crc,
						 int offset, int noofBytes, int flags,
						 char *event, int resType)
{
        bkPrintf("*** WARNING *** bQueueBackgroundLoad was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bQueueBackgroundWrite
	Purpose : queue a background load
	Parameters : channel, source area, handle of file, 
	             write offset, no of bytes, flags (see BKGFLAG_*)
				 completion event, resource type
	Returns : OK/FAIL
	Info : Uses filename only if file handle is not set
*/

int bQueueBackgroundWrite(EBBkgChannel channel, char *dest, HANDLE fp, int offset, int noofBytes, int flags,
																								char *event, int resType)
{
        bkPrintf("*** WARNING *** bQueueBackgroundWrite was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bIsBkgChannelQueueEmpty
   Purpose : determine if a background scheduler channel is empty
   Parameters : channel
   Returns : TRUE if empty, FALSE otherwise
   Info : 
*/

int bIsBkgChannelQueueEmpty(EBBkgChannel channel)
{
        bkPrintf("*** WARNING *** bIsBkgChannelQueueEmpty was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bPauseBackgroundFileAccess
   Purpose : pause/resume background file access
   Parameters : TRUE to pause, FALSE to resume
   Returns : TRUE if attained desired state, FALSE if still pending
   Info : 
*/

int bPauseBackgroundFileAccess(int pause)
{
        bkPrintf("*** WARNING *** bPauseBackgroundFileAccess was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}