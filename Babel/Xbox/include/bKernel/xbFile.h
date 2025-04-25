// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : xbFile.h
//   Purpose : file access functions
// Component : Xbox Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BKERNEL_XBFILE_H__
#define __BKERNEL_XBFILE_H__

// ********************************************************************************
// Constants and macros

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
