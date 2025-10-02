// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcKernel.cpp
//   Purpose : kernel functions
// Component : Xbox Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/* --------------------------------------------------------------------------------
   Function : bInitKernel
   Purpose : initialise the kernel module
   Parameters : 
   Returns : OK/FAIL
   Info : 
*/

int bInitKernel()
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bShutdownKernel
   Purpose : shutdown the kernel module
   Parameters : 
   Returns : 
   Info : 
*/

void bShutdownKernel()
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bAddOSEvent
   Purpose : add an OS event
   Parameters : callback, context for callback, event description
   Returns : event handle or NULL for failure
   Info : 
*/

HANDLE bAddOSEvent(TBOSEventCallback callback, void *context, char *description)
{
    return NULL;
}


/* --------------------------------------------------------------------------------
   Function : bDeleteOSEvent
   Purpose : delete an OS event
   Parameters : event to delete
   Returns : 
   Info : 
*/

void bDeleteOSEvent(HANDLE event)
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bHandleOSEvents
   Purpose : handle any active OS events
   Parameters : 
   Returns : 
   Info : 
*/

void bHandleOSEvents()
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkRun
   Purpose : run the babel program
   Parameters : main function, context
   Returns : 
   Info : 
*/

void bkRun(TBabelMainFunction mainFunc, void *context)
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkCreateMutex
   Purpose : Creates a mutex object
   Parameters : mutex object pointer
   Returns : mutex object pointer, or NULL for failure
   Info : 
*/

TBMutex *bkCreateMutex(TBMutex *mutex)
{
    return NULL;
}


/* --------------------------------------------------------------------------------
   Function : bkWaitMutex
   Purpose : Waits for a mutex object to become available and gains control
   Parameters : Mutex object pointer
   Returns : OK/FAIL
   Info : 
*/

int bkWaitMutex(TBMutex *mutex)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkReleaseMutex
   Purpose : Release a mutex object
   Parameters : Mutex object pointer
   Returns : OK/FAIL
   Info : 
*/

int bkReleaseMutex(TBMutex *mutex)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkDeleteMutex
   Purpose : Delete a mutex object
   Parameters : Mutex object pointer
   Returns : OK/FAIL
   Info : 
*/

int bkDeleteMutex(TBMutex *mutex)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bInitCommandLine
   Purpose : read and parse command line
   Parameters : 
   Returns : 1 pass/ 0 fail
   Info : 
*/

int bInitCommandLine()
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bShutdownCommandLine
   Purpose : free command line data
   Parameters : 
   Returns :
   Info : 
*/

void bShutdownCommandLine()
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkGetCommandLine
   Purpose : get command line
   Parameters : ptr to number of arguments, ptr to argument list
   Returns : 
   Info : Cannot be called before bkInit
*/

void bkGetCommandLine(int *argc, char **argv[])
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bFlushCPUCache
   Purpose : flush the CPU L1 and L2 caches
   Parameters : 
   Returns : 
   Info : 
*/

void bFlushCPUCache()
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bFlushWrites
   Purpose : flush all pending writes for an area of memory from the CPU cache
   Parameters : start of area, #dwords to flush
   Returns : 
   Info : 
*/

void bFlushWrites(uint32 *ptr, int noofDwords)
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkSetAppName
   Purpose : set the application name
   Parameters : name for application
   Returns : 
   Info : 
*/

void bkSetAppName(char *appName)
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bCheckMemAgainstFile
   Purpose : check a memory buffer against the contents of a file
   Parameters : memory buffer, filename, #bytes to compare
   Returns : TRUE if same, FALSE otherwise, -1=error
   Info : 
*/

int bCheckMemAgainstFile(void *ptr, char *fname, int noofBytes)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bCountSystemAllocs
   Purpose : count the number of system allocations
   Parameters : 
   Returns : number of system allocations
   Info : 
*/

int bCountSystemAllocs()
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bDumpSystemAllocs
   Purpose : dump a list of system allocs to the debug stream
   Parameters : 
   Returns : 
   Info : 
*/

void bDumpSystemAllocs()
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bActivateHeapCheckerThread
   Purpose : activate/deactivate the heap checker thread
   Parameters : TRUE to activate, FALSE to deactivate
   Returns : 
   Info : 
*/

void bActivateHeapCheckerThread(int status)
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bkSetModulePath
	Purpose : set path to runtime modules
	Parameters : path name (default: "c:/babel/ps2/modules/" or "\\MODULES\\")
	Returns :
	Info : Currently only required on PlayStation 2
*/

//void bkSetModulePath(char *pathName); // probably not used on PC