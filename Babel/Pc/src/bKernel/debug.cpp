// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : debug.cpp
//   Purpose : Debugging support
// Component : Babel Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/* --------------------------------------------------------------------------------
   Function : bkDeleteDebugStream
   Purpose : delete a debug stream
   Parameters : ptr to stream block
   Returns : 
   Info : 
*/

void bkDeleteDebugStream(TBDebugStream *stream)
{
        bkPrintf("*** WARNING *** bkDeleteDebugStream was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkSetDebugStream
   Purpose : set the current debug stream
   Parameters : ptr to stream to make current or NULL to return to using the default stream
   Returns : 
   Info : 
*/

void bkSetDebugStream(TBDebugStream *stream)
{
        bkPrintf("*** WARNING *** bkSetDebugStream was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkSetDebugStreamFlags
   Purpose : set flags for a debug stream
   Parameters : ptr to stream (or NULL for current one), flags to change, new values for flags
   Returns : 
   Info : 
*/

void bkSetDebugStreamFlags(TBDebugStream *stream, uint32 flagMask, uint32 newValues)
{
	if (stream == NULL) {
		stream = bCurrentDebugStream;
	}
	stream->flags = ~flagMask & stream->flags | flagMask & newValues;
}


/* --------------------------------------------------------------------------------
   Function : bkStreamPrintf
   Purpose : print a string to a debug stream
   Parameters : debug stream, as for printf
   Returns : 
   Info : 
*/

void bkStreamPrintf(TBDebugStream *stream, char *format, ...)
{
        bkPrintf("*** WARNING *** bkStreamPrintf was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkFlushDebugStreamToFile
   Purpose : flush all currently buffered debug output to stream's log file
   Parameters : ptr to stream
   Returns : 
   Info : 
*/

void bkFlushDebugStreamToFile(TBDebugStream *stream)
{
        bkPrintf("*** WARNING *** bkFlushDebugStreamToFile was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bkDataToSafeString
	Purpose : convert some data to a print-safe string
	Parameters : data source, size of data, buffer to fill, size of buffer
	Returns : ptr to buffer
	Info : a maximum of (MIN(dataSize, bufferSize-1)) bytes of data will be processed and appended with a NULL
*/

char *bkDataToSafeString(uchar *data, int dataSize, char *buffer, int bufferSize)
{
    char *ret = buffer;

    if (!buffer || bufferSize <= 0) {
        return ret; // no where to write - leave
    }

    if (!data || dataSize <= 0) {
        *buffer = '\0';
        return ret;
    }

    int n = dataSize;
    if (n >= bufferSize) n = bufferSize - 1; // space for '\0'

    for (int i = 0; i < n; ++i) {
        unsigned char c = data[i];
        buffer[i] = (c < 0x20) ? '.' : (char)c;
    }
    buffer[n] = '\0';
    return ret;
}