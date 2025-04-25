// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : xbCache.h
//   Purpose : file cache functions
// Component : XBOX Cache
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BKERNEL_XBCACHE_H__
#define __BKERNEL_XBCACHE_H__

// ********************************************************************************
// Constants and macros

// Cache flags
#define BCACHEFLAG_ENABLED				0x00000001	// Enable cache

// Cache file flags
#define BCACHEFILEFLAG_LOADED			0x00000001	// File is loaded
#define BCACHEFILEFLAG_REMOVE			0x00000002	// Remove this file when loading complete


// ********************************************************************************
// Globals

// global used to control the generation of file cache messages
extern int		bFileCacheMessages;


// ********************************************************************************
// Prototypes


/* --------------------------------------------------------------------------------
   Function : bInitCache
   Purpose : read current cache status
   Parameters : 
   Returns : 
   Info : Internal usage - do not use
*/

void bInitCache();


/* --------------------------------------------------------------------------------
   Function : bShutdownCache
   Purpose : 
   Parameters : 
   Returns : 
   Info : Internal usage - do not use
*/

void bShutdownCache();


/* --------------------------------------------------------------------------------
   Function : bCacheUpdate
   Purpose : schedule cache files for background loading if safe to do so
   Parameters : 
   Returns : 
   Info : Internal usage - do not use
*/

void bCacheUpdate();


/* --------------------------------------------------------------------------------
   Function : bkCacheFlush
   Purpose : flush the disc cache
   Parameters : flush ALL files?
   Returns : 
   Info : 
*/

void bkCacheFlush(int flushEverything=FALSE);


/* --------------------------------------------------------------------------------
   Function : bkCacheFileListAdd
   Purpose : add a file to the scheduler
   Parameters : filename, priority, group id, flags
   Returns : OK/FAIL
   Info : 
*/

int bkCacheFileListAdd(char *filename, int priority=0, int groupId=BDEFAULTGROUP, int flags=0);


/* --------------------------------------------------------------------------------
   Function : bkCacheFileListRemove
   Purpose : remove a file from the cache (to free up space)
   Parameters : filename
   Returns : 
   Info : 
*/

void bkCacheFileListRemove(char *filename);


/* --------------------------------------------------------------------------------
   Function : bkCacheFileListAddGroup
   Purpose : add a list of files to the scheduler
   Parameters : priority, group id, noofFiles, file list
   Returns : #files added
   Info : 
*/

int bkCacheFileListAddGroup(int priority, int groupId, int noofFiles, char **fileList);


/* --------------------------------------------------------------------------------
   Function : bkCacheFileListRemoveGroup
   Purpose : remove a group of file from the scheduler
   Parameters : group id
   Returns : 
   Info : 
*/

void bkCacheFileListRemoveGroup(int groupId);


/* --------------------------------------------------------------------------------
   Function : bkCacheEnable
   Purpose : enable/disable cache
   Parameters : newstate
   Returns : 
   Info : 
*/

void bkCacheEnable(int state);


/* --------------------------------------------------------------------------------
   Function : bkCacheIsEnabled
   Purpose : determine if cache is enabled
   Parameters : 
   Returns : TRUE/FALSE
   Info : 
*/

int bkCacheIsEnabled();


/* --------------------------------------------------------------------------------
   Function : bkCacheIsLoading
   Purpose : determine if cache is currently loading
   Parameters : 
   Returns : TRUE/FALSE
   Info : 
*/

int bkCacheIsLoading();


/* --------------------------------------------------------------------------------
   Function : bkCacheFreeSpace
   Purpose : get space free in disk cache
   Parameters : 
   Returns : no of bytes
   Info : 
*/

ulong bkCacheFreeSpace();


/* --------------------------------------------------------------------------------
   Function : bkCacheFindFile
   Purpose : locate a file in the cache
   Parameters : file to locate
   Returns : index of file in cache or -1 if not cached
   Info : 
*/

int bkCacheFindFile(char *filename);


/* --------------------------------------------------------------------------------
   Function : bCacheLoadingFileNotification
   Purpose : called by the background loader functions to notify the cache manager that it's about to load a file
   Parameters : filename
   Returns : 
   Info : 
*/

void bCacheLoadingFileNotification(char *fname);


/* --------------------------------------------------------------------------------
   Function : bCacheOpenFileFromCache
   Purpose : try to open a file from the cache
   Parameters : filename to open, ptr to file handle to return, buffer to fill with full path, length of buffer
   Returns : OK/FAIL
   Info : 
*/

int bCacheOpenFileFromCache(char *filename, TBFileHandle *fp, char *fullpath, int fpLen);

#endif		// __BKERNEL_XBCACHE_H__
