// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcCache.cpp
//   Purpose : file cache functions
// Component : Xbox Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/* --------------------------------------------------------------------------------
   Function : bInitCache
   Purpose : read current cache status
   Parameters : 
   Returns : 
   Info : Internal usage - do not use
*/

void bInitCache()
{
        bkPrintf("*** WARNING *** bInitCache was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bShutdownCache
   Purpose : 
   Parameters : 
   Returns : 
   Info : Internal usage - do not use
*/

void bShutdownCache()
{
        bkPrintf("*** WARNING *** bShutdownCache was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bCacheUpdate
   Purpose : schedule cache files for background loading if safe to do so
   Parameters : 
   Returns : 
   Info : Internal usage - do not use
*/

void bCacheUpdate()
{
        bkPrintf("*** WARNING *** bCacheUpdate was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkCacheFlush
   Purpose : flush the disc cache
   Parameters : flush ALL files?
   Returns : 
   Info : 
*/

void bkCacheFlush(int flushEverything)
{
        bkPrintf("*** WARNING *** bkCacheFlush was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkCacheFileListAdd
   Purpose : add a file to the scheduler
   Parameters : filename, priority, group id, flags
   Returns : OK/FAIL
   Info : 
*/

int bkCacheFileListAdd(char *filename, int priority, int groupId, int flags)
{
        bkPrintf("*** WARNING *** bkCacheFileListAdd was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkCacheFileListRemove
   Purpose : remove a file from the cache (to free up space)
   Parameters : filename
   Returns : 
   Info : 
*/

void bkCacheFileListRemove(char *filename)
{
        bkPrintf("*** WARNING *** bkCacheFileListRemove was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkCacheFileListAddGroup
   Purpose : add a list of files to the scheduler
   Parameters : priority, group id, noofFiles, file list
   Returns : #files added
   Info : 
*/

int bkCacheFileListAddGroup(int priority, int groupId, int noofFiles, char **fileList)
{
        bkPrintf("*** WARNING *** bkCacheFileListAddGroup was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkCacheFileListRemoveGroup
   Purpose : remove a group of file from the scheduler
   Parameters : group id
   Returns : 
   Info : 
*/

void bkCacheFileListRemoveGroup(int groupId)
{
        bkPrintf("*** WARNING *** bkCacheFileListRemoveGroup was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkCacheEnable
   Purpose : enable/disable cache
   Parameters : newstate
   Returns : 
   Info : 
*/

void bkCacheEnable(int state)
{
        bkPrintf("*** WARNING *** bkCacheEnable was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkCacheIsEnabled
   Purpose : determine if cache is enabled
   Parameters : 
   Returns : TRUE/FALSE
   Info : 
*/

int bkCacheIsEnabled()
{
        bkPrintf("*** WARNING *** bkCacheIsEnabled was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkCacheIsLoading
   Purpose : determine if cache is currently loading
   Parameters : 
   Returns : TRUE/FALSE
   Info : 
*/

int bkCacheIsLoading()
{
        bkPrintf("*** WARNING *** bkCacheIsLoading was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkCacheFreeSpace
   Purpose : get space free in disk cache
   Parameters : 
   Returns : no of bytes
   Info : 
*/

ulong bkCacheFreeSpace()
{
        bkPrintf("*** WARNING *** bkCacheFreeSpace was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkCacheFindFile
   Purpose : locate a file in the cache
   Parameters : file to locate
   Returns : index of file in cache or -1 if not cached
   Info : 
*/

int bkCacheFindFile(char *filename)
{
        bkPrintf("*** WARNING *** bkCacheFindFile was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return -1;
}


/* --------------------------------------------------------------------------------
   Function : bCacheLoadingFileNotification
   Purpose : called by the background loader functions to notify the cache manager that it's about to load a file
   Parameters : filename
   Returns : 
   Info : 
*/

void bCacheLoadingFileNotification(char *fname)
{
        bkPrintf("*** WARNING *** bCacheLoadingFileNotification was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bCacheOpenFileFromCache
   Purpose : try to open a file from the cache
   Parameters : filename to open, ptr to file handle to return, buffer to fill with full path, length of buffer
   Returns : OK/FAIL
   Info : 
*/

int bCacheOpenFileFromCache(char *filename, TBFileHandle *fp, char *fullpath, int fpLen)
{
        bkPrintf("*** WARNING *** bCacheOpenFileFromCache was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}
