// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcFile.h
//   Purpose : file access functions
// Component : Xbox Kernel
//   Comment : EXPECTED TO BE THE SAME AS xbFile.h, DIRECT REWRITE!
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BKERNEL_PCFILE_H__
#define __BKERNEL_PCFILE_H__

// ********************************************************************************
// Constants and macros

// in the original PC build this suffix (DAT_0075365C) is effectively an empty C-string
// check bkOpenFileReadOnlyWithSearch for additional info
#define BFILE_SEARCH_SUFFIX ""

// Enums for disk failures trigging a "_DiskError" event
typedef enum
{
	BDISKERROR_COVER_OPEN = 1,	// Drive open
	BDISKERROR_NO_DISK,			// Not used on Xbox
	BDISKERROR_WRONG_DISK,		// Wrong disk in drive
	BDISKERROR_RETRY,			// Device IO failure: retrying
	BDISKERROR_FATAL_ERROR,		// Not used on Xbox
	BDISKERRORS
} EBDiskErrorReasonCode;

// ********************************************************************************
// Types, structures and classes

// local and host file handle types (must be setup for the generic file header)
typedef FILE		*TBFileHandle;
typedef FILE		*TBHostFileHandle;

// Find file data structure
typedef struct _TBFindFileInfo
{
	HANDLE handle;
} TBFindFileInfo;


// ********************************************************************************
// Globals

extern char			bDiskErrorString[2];	// global passed to _diskError events


// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : bPrepareDiskEvents
	Purpose : Setup disk event 
	Parameters :
	Returns : 
	Info : 
*/

void bPrepareDiskEvents();


/* --------------------------------------------------------------------------------
   Function : bGetFreePhysicalMemory
   Purpose : return the amount of free physical memory
   Parameters : 
   Returns : free physical memory in bytes
   Info : 
*/

int bGetFreePhysicalMemory();


#endif		// __BKERNEL_XBFILE_H__
