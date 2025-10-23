// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcAudioStream.h
//   Purpose : Xbox specific audio streaming routines
// Component : Xbox Sound
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bInitSpecificMusic
	Purpose : initialise the specific music functions
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/

int bInitSpecificMusic()
{
        bkPrintf("*** WARNING *** bInitSpecificMusic was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return OK;
}


/* --------------------------------------------------------------------------------
   Function : bLoadAudioStreamByCRC
   Purpose : load and add audio stream info
   Parameters : package file, texture crc
   Returns : handle to stream, 0 if fail
   Info : 
*/

TBAudioStream *bLoadAudioStreamByCRC(TBPackageIndex *index, uint32 crc)
{
        bkPrintf("*** WARNING *** bLoadAudioStreamByCRC was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}


/* --------------------------------------------------------------------------------
   Function : bDeleteAudioStream
   Purpose : delete audio stream
   Parameters : stream handle
   Returns :
   Info : 
*/

void bDeleteAudioStream(TBAudioStream *stream)
{
        bkPrintf("*** WARNING *** bDeleteAudioStream was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bsPlayAudioStream
   Purpose : play audio stream
   Parameters : stream handle, flags, volume (0-255), panning (+/-255), frequency (0 for default), callback function
   Returns : OK/FAIL
   Info : 
*/

int bsPlayAudioStream(TBAudioStream *stream, uint flags, uint volume, int pan, uint frequency, TBAudioStreamCallback callback, HANDLE fileHandle)
{
        bkPrintf("*** WARNING *** bsPlayAudioStream was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bsStopAudioStream
   Purpose : stop audio stream
   Parameters : stream handle
   Returns :
   Info : 
*/

void bsStopAudioStream(TBAudioStream *stream)
{
        bkPrintf("*** WARNING *** bsStopAudioStream was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bsSetAudioStreamVolume
   Purpose : set audio stream volume and panning
   Parameters : stream handle, volume (0-255), panning(+/-255), subStream index or BSUBSTREAM_ALL
   Returns :
   Info : 
*/

int bsSetAudioStreamVolume(TBAudioStream *stream, uint volume, int pan, int subStream)
{
        bkPrintf("*** WARNING *** bsSetAudioStreamVolume was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bsSetGlobalAudioStreamVolume
	Purpose : set the global stream playback volume
	Parameters : volume (0..255)
	Returns : 
	Info : 
*/

void bsSetGlobalAudioStreamVolume(int volume)
{
        bkPrintf("*** WARNING *** bsSetGlobalAudioStreamVolume was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bsGetGlobalAudioStreamVolume
	Purpose : get the global stream playback volume
	Parameters : 
	Returns : volume (0..255)
	Info : 
*/

int bsGetGlobalAudioStreamVolume()
{
        bkPrintf("*** WARNING *** bsGetGlobalAudioStreamVolume was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bsSetAudioStreamFrequency
   Purpose : set audio stream playback frequency
   Parameters : stream handle, frequency (8000 - 48000), 0 reset to default
   Returns :
   Info : 
*/

int bsSetAudioStreamFrequency(TBAudioStream *stream, uint frequency)
{
        bkPrintf("*** WARNING *** bsSetAudioStreamFrequency was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bsSetAudioStreamFlags
   Purpose : set audio stream flags
   Parameters : stream handle, flags
   Returns :OK/FAIL
   Info : 
*/

int bsSetAudioStreamFlags(TBAudioStream *stream, uint flags)
{
        bkPrintf("*** WARNING *** bsSetAudioStreamFlags was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bsClearAudioStreamFlags
   Purpose : clear audio stream flags
   Parameters : stream handle, flags
   Returns :OK/FAIL
   Info : 
*/

int bsClearAudioStreamFlags(TBAudioStream *stream, uint flags)
{
        bkPrintf("*** WARNING *** bsClearAudioStreamFlags was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bsGetAudioStreamFlags
   Purpose : get audio stream flags
   Parameters : stream handle, flags
   Returns :flags status
   Info : 
*/

int bsGetAudioStreamFlags(TBAudioStream *stream, uint flags)
{
        bkPrintf("*** WARNING *** bsGetAudioStreamFlags was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bsGetAudioStreamPosition
   Purpose : return the current play position of the stream in samples
   Parameters : stream
   Returns :current play position in samples
   Info : is an approximate indication
*/

int bsGetAudioStreamPosition(TBAudioStream *stream)
{
        bkPrintf("*** WARNING *** bsGetAudioStreamPosition was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bsGetAudioStreamInfo
   Purpose : return info about an audio stream
   Parameters : stream, ptr for original frequency, ptr for #samples, ptr for bit-depth, 
                ptr for compression ratio, ptr for #markers, ptr for markers array
   Returns :
   Info : all ptrs except 'stream' can be NULL
*/

void bsGetAudioStreamInfo(TBAudioStream *stream, 
						  int *origFrequency, int *noofSamples, int *bitDepth, 
						  int *compressionRatio, int *noofMarkers, int **markers)
{
        bkPrintf("*** WARNING *** bsGetAudioStreamInfo was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
   Function : bsGetAudioStreamStatus
   Purpose : retrieve the current status of a sound stream
   Parameters : stream handle
 				 ptr for volume, ptr for pan, ptr for current frequency
   Returns : status
   Info : all ptrs except 'stream' can be NULL
*/

EBSoundStreamStatus bsGetAudioStreamStatus(TBAudioStream *stream,
										   int *volume, int *pan, int *frequency)
{
        bkPrintf("*** WARNING *** bsGetAudioStreamStatus was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return BSSHSTATUS_IDLE;
}


/* --------------------------------------------------------------------------------
   Function : bsFindAudioStreamMarker
   Purpose : return the marker at or immediately before a given sample offset
   Parameters : stream, sample offset
   Returns : marker number or -1 for none
   Info : uses a binary search for speed
*/

int bsFindAudioStreamMarker(TBAudioStream *stream, int sampleOffset)
{
        bkPrintf("*** WARNING *** bsFindAudioStreamMarker was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return -1;
}


/* --------------------------------------------------------------------------------
   Function : bHandleFillAudioStreams
   Purpose : Fill the audio streams as required
   Parameters : 
   Returns : 
   Info : WMA streams must be filled each frame
*/

void bHandleFillAudioStreams()
{
        bkPrintf("*** WARNING *** bHandleFillAudioStreams was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bsAllocAudioStreamData
   Purpose : allocate static data for an audio stream
   Parameters : stream handle
   Returns : 1/0
   Info : 
*/

int bsAllocAudioStreamData(TBAudioStream *stream)
{
        bkPrintf("*** WARNING *** bsAllocAudioStreamData was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bsFreeAudioStreamData
   Purpose : free static data for an audio stream
   Parameters : stream handle
   Returns :
   Info : 
*/

void bsFreeAudioStreamData(TBAudioStream *stream)
{
        bkPrintf("*** WARNING *** bsFreeAudioStreamData was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bGetAudioStreamBufferInfo
   Purpose : return info about buffers used by an audio stream
   Parameters : stream to get info from, 
				memory buffer size, no of memory buffers,
				sound buffer size, no of sound buffers,
				disk read position,	memory position, sound playback position
				ptr to loading
   Returns :
   Info : all ptrs except 'stream' can be NULL
*/

void bGetAudioStreamBufferInfo(TBAudioStream *stream, 
						  int *memBufSize, int *memBuffers, 
						  int *soundBufSize, int *soundBuffers, 
						  int *diskPos, int *memPos, int *soundPos,
						  int *loading)
{
        bkPrintf("*** WARNING *** bGetAudioStreamBufferInfo was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bUpdateSound
   Purpose : Update all sound processing
   Parameters : 
   Returns :
   Info : Internal only
*/

void bUpdateSound()
{
        bkPrintf("*** WARNING *** bUpdateSound was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}