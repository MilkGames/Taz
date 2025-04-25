// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : bkgload.h
//   Purpose : Background loading support
// Component : Generic Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BKERNEL_BKGLOAD_H__
#define __BKERNEL_BKGLOAD_H__


//*********************************************************************************
//	constants and macros

// Background loading errors
typedef enum EBBkgError
{
	EBBKGERROR_NONE,
	EBBKGERROR_NOTFOUND,
	EBBKGERROR_LOADBUFFERTOSMALL,
	EBBKGERROR_READERROR,
	EBBKGERROR_PACKAGEEMPTY,
	EBBKGERROR_SYSTEMERROR,
};

//*********************************************************************************
// types, structures and classes

// Background loading event data
typedef struct _TBBkgLoadResult
{
	EBBkgError			resultCode;			// load return code (see EBBKGERROR_)
	uint				crc;				// CRC of file
} TBBkgLoadResult;	// 8 bytes
// Note this must not exceed BMAXEVENTDATALEN(=16) bytes if the event queue are to work

//*********************************************************************************
// globals

// total number of bytes transferred
extern volatile int		bBytesTransferred;

// number of bytes transferred on last file
extern volatile int		bLastBytesTransferred;


//*********************************************************************************
// prototypes



#endif /* __BKERNEL_BKGLOAD_H__ */
