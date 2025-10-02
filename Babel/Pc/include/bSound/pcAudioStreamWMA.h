// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : xbAudioStreamWMA.h
//   Purpose : Xbox specific audio streaming routines (WMA format)
// Component : Xbox Sound
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BSOUND_PCXBAUDIOSTREAMWMA_H__
#define __BSOUND_PCXBAUDIOSTREAMWMA_H__

// ********************************************************************************
// Constants and macros

// size of block we read from disc at a time (64Kb = space for ~3.7 secs of 10:1 44Khz 16bit stereo)
#define BXBAS_WMA_READ_SIZE			(64*1024)

// define this symbol for extra WMA integrity checking
//#define	BSUPER_ROBUST_WMA


// ********************************************************************************
// Globals

extern int		bWMAMessages;			// global used to control WMA debug messages


// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bDeleteAudioStreamWMA
   Purpose : delete audio stream
   Parameters : stream handle
   Returns :
   Info : 
*/

void bDeleteAudioStreamWMA(TBAudioStream *stream);


/* --------------------------------------------------------------------------------
   Function : bPlayAudioStreamWMA
   Purpose : Frees a WMA autostream
   Parameters : stream handle, flags, volume (0-255), panning (+/-255), frequency (0 for default), callback function
																soundtrack song file handle or NULL for package streams
   Returns : OK/FAIL
   Info : 
*/

int bPlayAudioStreamWMA(TBAudioStream *stream, uint flags, uint volume, int pan, uint frequency,
																TBAudioStreamCallback callback, HANDLE wmaFileHandle=NULL);


/* --------------------------------------------------------------------------------
   Function : bStopAudioStreamWMA
   Purpose : Stops a WMA autostream
   Parameters : audiostream
   Returns : 
   Info : 
*/

void bStopAudioStreamWMA(TBAudioStream *stream);


/* --------------------------------------------------------------------------------
   Function : bStartWMAProcessor
   Purpose : start the WMA processor thread
   Parameters : 
   Returns : 
   Info : 
*/

void bStartWMAProcessor();


/* --------------------------------------------------------------------------------
   Function : bDecodeWMA
   Purpose : decode WMA data for currently playing streams
   Parameters : 
   Returns : 
   Info : 
*/

void bDecodeWMA();

#endif //__BSOUND_PCXBAUDIOSTREAMWMA_H__
