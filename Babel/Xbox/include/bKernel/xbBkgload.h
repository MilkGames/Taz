// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : xbBkgload.h
//   Purpose : Background loading support
// Component : Xbox Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BKERNEL_XBBKGLOAD_H__
#define __BKERNEL_XBBKGLOAD_H__

//*********************************************************************************
//	constants and macros

// max size of audio channel block
#define BKG_AUDIO_BLOCK_SIZE		(128*1024)

// max size of data channel block
#define BKG_DATA_BLOCK_SIZE			(256*1024)

// background load scheduler flags
#define BKGFLAG_UPDATENOOFBYTES		0x00000001			// update the bBytesTransferred variable
#define BKGFLAG_CLOSEFILEATEND		0x00000002			// close filehandle at finish
#define BKGFLAG_INPROGRESS			0x00000004			// set if this request being loaded right now
#define BKGFLAG_WRITE				0x00000008			// is a write rather than a read

// background load scheduler channels
typedef enum EBBkgChannel {
	EBBKGCHANNEL_DATA,
	EBBKGCHANNEL_AUDIO1,
	EBBKGCHANNEL_AUDIO2,

	EBBKGCHANNEL_CHANNELS,			// symbolic constant indicating the number of valid channels

	EBBKGCHANNEL_INVALID			// symbolic constant representing an invalid channel number
};


//*********************************************************************************
// types and structures

// scheduler 'load complete' callback
typedef void (*TBBkgLoadComplete)(void *context);

// overlapped IO extended structure
typedef struct _TBBkgLoadCmd {
	char				filename[256];		// on disk filename
	char				target[256];		// destination for copy commands
	char				eventName[128];		// event name

	int					resType;			// resource type
	void				*address;			// address being loaded into
	uint32				uid;				// unique ID
	int					offset;				// file offset to read from

	int					noofBytes;			// #bytes to load/copy
	HANDLE				fp;					// file handle (source)
	EBBkgChannel		channel;			// channel ID
	int					flags;				// flags (see BKGFLAG_*)

	EBBkgError			resultCode;			// success/failure flags (see EBBKGERROR_)
	uint				crc;				// requested filename crc
} TBBkgLoadCmd;


//*********************************************************************************
// globals


/*	--------------------------------------------------------------------------------
	Function : bKernelInitBkgLoad
	Purpose : Initialise background loading resources
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/

int bKernelInitBkgLoad();


/* --------------------------------------------------------------------------------
   Function : bkLoadFileBkg
   Purpose : load a file from a package / disc in the background
   Parameters : package index ptr, filename, ptr to buffer (or NULL for dynamic), ptr for returned size (can be NULL), ptr for tag info (or NULL), event to trigger when complete, resource type
   Returns : ptr to buffer or NULL for failure
   Info : package index = NULL for non-package files
*/

uchar *bkLoadFileBkg(TBPackageIndex *index, char *filename, uchar *dataPtr, int *retSize, TBFileTagInfo *tagInfo, char *eventName);


/* --------------------------------------------------------------------------------
   Function : bkLoadPackageBkg
   Purpose : load a package file in the background
   Parameters : parent package index (or NULL), package filename, event to generate when complete, pointer to return size, pointer to load location (defaults to NULL)
   Returns : handle to package (ptr to index)
   Info : 
*/

TBPackageIndex *bkLoadPackageBkg(TBPackageIndex *parentIndex, char *filename, char *eventName, int *retSize, uchar *dataPtr = NULL);


/* --------------------------------------------------------------------------------
   Function : bkSetBkgLoadSpeed
   Purpose : set the desired background loading transfer speed
   Parameters : desired speed in Kb per seconds
   Returns : 
   Info : 
*/

void bkSetBkgLoadSpeed(int kbPerSec);


/* --------------------------------------------------------------------------------
   Function : bIsBkgChannelBusy
   Purpose : determine if a background scheduler channel is busy
   Parameters : channel
   Returns : TRUE if busy, FALSE otherwise
   Info : 
*/

int bIsBkgChannelBusy(EBBkgChannel channel);


/* --------------------------------------------------------------------------------
   Function : bBkgChannelCancelRequest
   Purpose : cancel channel request (if cancelable)
   Parameters : channel
   Returns : 
   Info : 
*/

void bBkgChannelCancelRequest(EBBkgChannel channel);


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
						 char *event, int resType);


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
																								char *event, int resType);


/* --------------------------------------------------------------------------------
   Function : bIsBkgChannelQueueEmpty
   Purpose : determine if a background scheduler channel is empty
   Parameters : channel
   Returns : TRUE if empty, FALSE otherwise
   Info : 
*/

int bIsBkgChannelQueueEmpty(EBBkgChannel channel);


/* --------------------------------------------------------------------------------
   Function : bPauseBackgroundFileAccess
   Purpose : pause/resume background file access
   Parameters : TRUE to pause, FALSE to resume
   Returns : TRUE if attained desired state, FALSE if still pending
   Info : 
*/

int bPauseBackgroundFileAccess(int pause);

#endif /* __BKERNEL_XBBKGLOAD_H__ */



