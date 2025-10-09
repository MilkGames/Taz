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
    return 0;
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
    return NULL;
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
    return 0;
}