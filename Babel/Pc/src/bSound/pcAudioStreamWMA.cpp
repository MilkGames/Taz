// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcAudioStreamWMA.cpp
//   Purpose : Xbox specific audio streaming routines (WMA format)
// Component : Xbox Sound
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/* --------------------------------------------------------------------------------
   Function : bDeleteAudioStreamWMA
   Purpose : delete audio stream
   Parameters : stream handle
   Returns :
   Info : 
*/

void bDeleteAudioStreamWMA(TBAudioStream *stream)
{
        bkPrintf("*** WARNING *** bDeleteAudioStreamWMA was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bPlayAudioStreamWMA
   Purpose : Frees a WMA autostream
   Parameters : stream handle, flags, volume (0-255), panning (+/-255), frequency (0 for default), callback function
																soundtrack song file handle or NULL for package streams
   Returns : OK/FAIL
   Info : 
*/

int bPlayAudioStreamWMA(TBAudioStream *stream, uint flags, uint volume, int pan, uint frequency, TBAudioStreamCallback callback, HANDLE wmaFileHandle)
{
        bkPrintf("*** WARNING *** bPlayAudioStreamWMA was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bStopAudioStreamWMA
   Purpose : Stops a WMA autostream
   Parameters : audiostream
   Returns : 
   Info : 
*/

void bStopAudioStreamWMA(TBAudioStream *stream)
{
        bkPrintf("*** WARNING *** bStopAudioStreamWMA was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bStartWMAProcessor
   Purpose : start the WMA processor thread
   Parameters : 
   Returns : 
   Info : 
*/

void bStartWMAProcessor()
{
        bkPrintf("*** WARNING *** bStartWMAProcessor was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bDecodeWMA
   Purpose : decode WMA data for currently playing streams
   Parameters : 
   Returns : 
   Info : 
*/

void bDecodeWMA()
{
        bkPrintf("*** WARNING *** bDecodeWMA was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}