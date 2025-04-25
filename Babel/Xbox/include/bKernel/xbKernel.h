// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : xbKernel.h
//   Purpose : kernel functions
// Component : Xbox Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BKERNEL_XBKERNEL_H__
#define __BKERNEL_XBKERNEL_H__

// ********************************************************************************
// Constants and macros

// define away PS2 style structure alignment qualifiers
#define BALIGN16

// maximum number of entries in the OS event queue
#define BMAXOSEVENTS		16


// ********************************************************************************
// Types, structures and classes

// function pointer to the client's main function
typedef void (*TBabelMainFunction)(void *context);

// 32bit context union
typedef union {
	int32				iContext;
	uint32				uiContext;
	short				sContext[2];
	ushort				usContext[2];
	float				fContext;
	char				cContext[4];
	uchar				ucContext[4];
	void				*pContext;
} UBContext32;

// system mutex type
typedef HANDLE TBMutex;

// OS event callback function (return FALSE if event was deleted)
typedef int (*TBOSEventCallback)(void *context);

// an OS event record
typedef struct _TBOSEvent {
	TBOSEventCallback	callback;			// callback function
	void				*context;			// context for callback
	char				*description;		// description (for debugging purposes)
	uint32				pad;
} TBOSEvent;


// ********************************************************************************
// Include the generic file header

#include "kernel.h"


// ********************************************************************************
// Globals

extern char			bHomeDirectory[256];			// project home directory (where the EXE lives)
extern char			bHomeSuffix[8];					// No used on XBOX
extern TBOSEvent	bOSEvents[BMAXOSEVENTS];		// OS event list
extern HANDLE		bOSEventHandles[BMAXOSEVENTS];	// OS event handle array
extern int			bNoofOSEvents;					// #OS events in array
extern uint32		bBkInitFlags;					// flags used when bkInit was called
extern LAUNCH_DATA	bXBLaunchData;					// Xbox launch info
extern DWORD		bXBLaunchType;					// type of launch info
extern int			bForceUseSystemHeap;			// set to force use of system heap

extern int			bForceStandardMallocFree;		// set to force use of standard malloc and free


// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bInitKernel
   Purpose : initialise the kernel module
   Parameters : 
   Returns : OK/FAIL
   Info : 
*/

int bInitKernel();


/* --------------------------------------------------------------------------------
   Function : bShutdownKernel
   Purpose : shutdown the kernel module
   Parameters : 
   Returns : 
   Info : 
*/

void bShutdownKernel();


/* --------------------------------------------------------------------------------
   Function : bAddOSEvent
   Purpose : add an OS event
   Parameters : callback, context for callback, event description
   Returns : event handle or NULL for failure
   Info : 
*/

HANDLE bAddOSEvent(TBOSEventCallback callback, void *context, char *description);


/* --------------------------------------------------------------------------------
   Function : bDeleteOSEvent
   Purpose : delete an OS event
   Parameters : event to delete
   Returns : 
   Info : 
*/

void bDeleteOSEvent(HANDLE event);


/* --------------------------------------------------------------------------------
   Function : bHandleOSEvents
   Purpose : handle any active OS events
   Parameters : 
   Returns : 
   Info : 
*/

void bHandleOSEvents();


/* --------------------------------------------------------------------------------
   Function : bkRun
   Purpose : run the babel program
   Parameters : main function, context
   Returns : 
   Info : 
*/

void bkRun(TBabelMainFunction mainFunc, void *context);


/* --------------------------------------------------------------------------------
   Function : bkCreateMutex
   Purpose : Creates a mutex object
   Parameters : mutex object pointer
   Returns : mutex object pointer, or NULL for failure
   Info : 
*/

TBMutex *bkCreateMutex(TBMutex *mutex);


/* --------------------------------------------------------------------------------
   Function : bkWaitMutex
   Purpose : Waits for a mutex object to become available and gains control
   Parameters : Mutex object pointer
   Returns : OK/FAIL
   Info : 
*/

int bkWaitMutex(TBMutex *mutex);


/* --------------------------------------------------------------------------------
   Function : bkReleaseMutex
   Purpose : Release a mutex object
   Parameters : Mutex object pointer
   Returns : OK/FAIL
   Info : 
*/

int bkReleaseMutex(TBMutex *mutex);


/* --------------------------------------------------------------------------------
   Function : bkDeleteMutex
   Purpose : Delete a mutex object
   Parameters : Mutex object pointer
   Returns : OK/FAIL
   Info : 
*/

int bkDeleteMutex(TBMutex *mutex);


/* --------------------------------------------------------------------------------
   Function : bInitCommandLine
   Purpose : read and parse command line
   Parameters : 
   Returns : 1 pass/ 0 fail
   Info : 
*/

int bInitCommandLine();


/* --------------------------------------------------------------------------------
   Function : bShutdownCommandLine
   Purpose : free command line data
   Parameters : 
   Returns :
   Info : 
*/

void bShutdownCommandLine();


/* --------------------------------------------------------------------------------
   Function : bkGetCommandLine
   Purpose : get command line
   Parameters : ptr to number of arguments, ptr to argument list
   Returns : 
   Info : Cannot be called before bkInit
*/

void bkGetCommandLine(int *argc, char **argv[]);


/* --------------------------------------------------------------------------------
   Function : bFlushCPUCache
   Purpose : flush the CPU L1 and L2 caches
   Parameters : 
   Returns : 
   Info : 
*/

void bFlushCPUCache();


/* --------------------------------------------------------------------------------
   Function : bFlushWrites
   Purpose : flush all pending writes for an area of memory from the CPU cache
   Parameters : start of area, #dwords to flush
   Returns : 
   Info : 
*/

void bFlushWrites(uint32 *ptr, int noofDwords);


/* --------------------------------------------------------------------------------
   Function : bkSetAppName
   Purpose : set the application name
   Parameters : name for application
   Returns : 
   Info : 
*/

inline void bkSetAppName(char *appName) { };


/* --------------------------------------------------------------------------------
   Function : bCheckMemAgainstFile
   Purpose : check a memory buffer against the contents of a file
   Parameters : memory buffer, filename, #bytes to compare
   Returns : TRUE if same, FALSE otherwise, -1=error
   Info : 
*/

int bCheckMemAgainstFile(void *ptr, char *fname, int noofBytes);


/* --------------------------------------------------------------------------------
   Function : bCountSystemAllocs
   Purpose : count the number of system allocations
   Parameters : 
   Returns : number of system allocations
   Info : 
*/

int bCountSystemAllocs();


/* --------------------------------------------------------------------------------
   Function : bDumpSystemAllocs
   Purpose : dump a list of system allocs to the debug stream
   Parameters : 
   Returns : 
   Info : 
*/

void bDumpSystemAllocs();


/* --------------------------------------------------------------------------------
   Function : bActivateHeapCheckerThread
   Purpose : activate/deactivate the heap checker thread
   Parameters : TRUE to activate, FALSE to deactivate
   Returns : 
   Info : 
*/

void bActivateHeapCheckerThread(int status);


/*	--------------------------------------------------------------------------------
	Function : bkSetModulePath
	Purpose : set path to runtime modules
	Parameters : path name (default: "c:/babel/ps2/modules/" or "\\MODULES\\")
	Returns :
	Info : Currently only required on PlayStation 2
*/

void bkSetModulePath(char *pathName);


#endif		// __BKERNEL_XBKERNEL_H__
