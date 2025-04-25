// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : debug.h
//   Purpose : debugging functions
// Component : Generic Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BKERNEL_DEBUG_H__
#define __BKERNEL_DEBUG_H__

// ********************************************************************************
// Constants and Macros

// debug stream flags
#define BDEBUGSTREAMFLAG_DYNAMIC			0x00000001		// stream block has been dynamically allocated
#define BDEBUGSTREAMFLAG_TODEBUGGER			0x00000002		// send output to debugger window
#define BDEBUGSTREAMFLAG_FLUSH				0x00000004		// flush the logfile when it is created
#define BDEBUGSTREAMFLAG_TIMESTAMP			0x00000008		// timestamp each log entry
#define BDEBUGSTREAMFLAG_TOCONSOLEWINDOW	0x00000010		// send output to console window (needs to be active)

#ifdef _DEBUG
#define DPRINTF bkPrintf
#else
#define	DPRINTF	BNULLFUNCTION
#endif


// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bkPrintf
   Purpose : print a string to the debug stream
   Parameters : as for printf
   Returns : 
   Info : 
*/

void bkPrintf(char *format, ...);


/* --------------------------------------------------------------------------------
   Function : bkCreateDebugStream
   Purpose : create a new debug stream
   Parameters : ptr to stream block or NULL for dynamic, log filename, flags (see BDEBUGSTREAMFLAG_)
   Returns : ptr to stream block
   Info : 
*/

TBDebugStream *bkCreateDebugStream(TBDebugStream *stream, char *filename, uint32 flags);


/* --------------------------------------------------------------------------------
   Function : bkDeleteDebugStream
   Purpose : delete a debug stream
   Parameters : ptr to stream block
   Returns : 
   Info : 
*/

void bkDeleteDebugStream(TBDebugStream *stream);


/* --------------------------------------------------------------------------------
   Function : bkSetDebugStream
   Purpose : set the current debug stream
   Parameters : ptr to stream to make current or NULL to return to using the default stream
   Returns : 
   Info : 
*/

void bkSetDebugStream(TBDebugStream *stream);


/* --------------------------------------------------------------------------------
   Function : bkSetDebugStreamFlags
   Purpose : set flags for a debug stream
   Parameters : ptr to stream (or NULL for current one), flags to change, new values for flags
   Returns : 
   Info : 
*/

void bkSetDebugStreamFlags(TBDebugStream *stream, uint32 flagMask, uint32 newValues);


/* --------------------------------------------------------------------------------
   Function : bkStreamPrintf
   Purpose : print a string to a debug stream
   Parameters : debug stream, as for printf
   Returns : 
   Info : 
*/

void bkStreamPrintf(TBDebugStream *stream, char *format, ...);


/* --------------------------------------------------------------------------------
   Function : bkFlushDebugStreamToFile
   Purpose : flush all currently buffered debug output to stream's log file
   Parameters : ptr to stream
   Returns : 
   Info : 
*/

void bkFlushDebugStreamToFile(TBDebugStream *stream);


/*	--------------------------------------------------------------------------------
	Function : bkDataToSafeString
	Purpose : convert some data to a print-safe string
	Parameters : data source, size of data, buffer to fill, size of buffer
	Returns : ptr to buffer
	Info : a maximum of (MIN(dataSize, bufferSize-1)) bytes of data will be processed and appended with a NULL
*/

char *bkDataToSafeString(uchar *data, int dataSize, char *buffer, int bufferSize);


#endif		// __BKERNEL_DEBUG_H__
