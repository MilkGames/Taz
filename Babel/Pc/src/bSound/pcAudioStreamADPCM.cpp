// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcAudioStreamADPCM.h
//   Purpose : Xbox specific audio streaming routines
// Component : Xbox Sound
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/* --------------------------------------------------------------------------------
   Function : bDeleteAudioStreamADPCM
   Purpose : delete audio stream
   Parameters : stream handle
   Returns :
   Info : 
*/

void bDeleteAudioStreamADPCM(TBAudioStream *stream)
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bPlayAudioStreamADPCM
   Purpose : play audio stream
   Parameters : stream handle, flags, volume (0-255), panning (+/-255), frequency (0 for default), callback function
   Returns : OK/FAIL
   Info : 
*/

int bPlayAudioStreamADPCM(TBAudioStream *stream, uint flags, uint volume, int pan, uint frequency, TBAudioStreamCallback callback)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bStopAudioStreamADPCM
   Purpose : stop audio stream (ADPCM format)
   Parameters : stream handle
   Returns :
   Info : 
*/

void bStopAudioStreamADPCM(TBAudioStream *stream)
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bNotifyADPCM
   Purpose : Perform manual checking of stream location (since notify not working ??? 23/11/01)
   Parameters : 
   Returns :
   Info : Internal
*/

void bNotifyADPCM()
{
    return;
}