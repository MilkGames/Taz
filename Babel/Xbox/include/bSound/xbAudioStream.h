// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : xbAudioStream.h
//   Purpose : Xbox specific audio streaming routines
// Component : Xbox Sound
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BSOUND_PCXBAUDIOSTREAM_H__
#define __BSOUND_PCXBAUDIOSTREAM_H__

// ********************************************************************************
// Constants and macros

// PC stream buffer size for WMA (play buffer is two of these)
#define BXBAS_WMA_BUFFER_SIZE			(128*1024)

// PC stream buffer size for ADPCM (play buffer is two of these)
// NOTE: ADPCM buffer must be divisible by 36 (the size of an ADPCM packet), hence 126 rather than 128
#define BXBAS_ADPCM_BUFFER_SIZE			(126*1024)

// PC stream sub buffer size for ADPCM 
// NOTE: ADPCM buffer must be divisible by 36 (the size of an ADPCM packet), [hence 63 rather than 64]
// This must also be a multiple of 2*1024 for the streaming - so reduced from 63 (1.75*36) to 54 (1.5*36)
#define BXBAS_ADPCM_SUBBUFFER_SIZE		(54*1024)	//(63*1024)

// max number of interleaved substreams
#define BMAX_SUBSTREAMS					4

// constant indicating all substreams
#define BSUBSTREAM_ALL					-1

// stream type values
enum EBSoundStreamType
{
	BSTREAMTYPE_ADPCM,
	BSTREAMTYPE_WMA,
};

// stream status values
enum EBSoundStreamStatus {
	BSSHSTATUS_IDLE,				// stream is idle (the default state)
	BSSHSTATUS_PLAYING,				// stream is playing
	BSSHSTATUS_PAUSED,				// stream is paused
	BSSHSTATUS_PRELOADING,			// stream is loading first block
};

// callback reason
enum EBSoundStreamCallbackReason {
	BAUDIOSTREAM_FINISHED,
	BAUDIOSTREAM_LOOPED,
};

// maximum number of simultaneous audio-streams
#define	BAUDIOSTREAM_MAX		2

// Number of buffers - currently double buffered
#define BXBAS_NOOFBUFFERS		2

// platform specific audio stream flags
#define BSTREAMFLAG_LOADPENDING		0x01000000				// set when we are waiting for a block to be loaded
#define BSTREAMFLAG_PLAYLIST		0x02000000				// set if this is a playlist stream rather than a package resource stream
#define BSTREAMFLAG_LOOPPENDING		0x04000000				// flag used to trigger looping callback


// ********************************************************************************
// Types, Structures and Classes

// audio stream callback type
typedef void (*TBAudioStreamCallback)(struct _TBAudioStream *stream, EBSoundStreamCallbackReason reason);

// audio stream header
typedef struct _TBAudioStream {
	TBResourceInfo			resInfo;						// resource info header

	int32					noofSamples;					// total #samples in stream
	int32					originalFrequency;				// original sample freqency
	int32					bitDepth;						// depth of each sample (8 or 16)
	uint32					originalFlags;					// flags set by packager (see BSTREAMFLAG_)

	int32					groupId;						// resource group ID
	TBPackageIndex			*package;						// pointer to package
	uint32					pakOffset;						// offset within pakfile (in pau's)
	uchar					*data;							// data to play

	LPDIRECTSOUNDBUFFER		soundBuffer[BMAX_SUBSTREAMS];	// sound buffers we are playing/streaming into

	int						noofStreams;
	HANDLE					event;							// OS event
	int32					noofBlocks;						// #blocks in stream
	int32					nextBlock;						// number of next block to read

	int32					nextWritePos;					// next write position
	uchar					*encodedData;					// WMA encoded data
	XMediaObject			*decoder;						// WMA decoder
	union {
		uint32				noofDecodedBytes;				// WMA no of decoded bytes
		int32				adpcmPacketsRead;				// #ADPCM packets read
	};

	EBSoundStreamType		type;							// type of stream (see BSTREAMTYPE)
	TBEventClient			*backgroundLoadEvent;			// backgound load event handler
	HANDLE					backgroundLoadHandle;			// handle to file background loading
	EBBkgChannel			backgroundChannel;				// background audio channel used

	uint32					loading;						// buffer background loading
	EBSoundStreamStatus		status;							// status of stream (playing, paused, ...)
	int32					volume;							// intended volume for playback
	int32					pan;							// intended pan for playback

	int32					frequency;						// intended frequency for playback
	volatile uint32			flags;							// current flags (see BSTREAMFLAG_)
	TBAudioStreamCallback	callback;						// finish/loop callback
	uint32					noofMarkers;					// #markers

	int32					*markers;						// list of markers (markers are stored as sample offsets)
	char					*eventName;						// name of background loading event
	int						streamNumber;					// zero-based index into currently active stream array
	uint32					nextPlayBuffer;					// next play buffer [0, BXBAS_NOOFBUFFERS)

	int32					lastDecodeBuffer;				// buffer we decoded from last time
	int32					nextDecodeWritePos;				// offset to next position in data buffer we will be writing to
	int32					noofAdpcmPackets;				// #adpcm packets in stream
	int32					dyingCounter;					// counter used when dying
} TBAudioStream;



// ********************************************************************************
// Globals

// global volume setting for audio streams
extern int				bGlobalStreamVolume;

// record of how many streams are active and what they are
extern int				bNoofActiveAudioStreams;
extern TBAudioStream	*bActiveAudioStream[BAUDIOSTREAM_MAX];

// array of names for audio-stream background loading events
extern char				*bAudioBkgLoadEventName[BAUDIOSTREAM_MAX];

// workspace buffer for WMA decoders so they don't leak like sieves
extern void				*bWMADecoderBuffer;


// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : bInitSpecificMusic
	Purpose : initialise the specific music functions
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/

int bInitSpecificMusic();


/* --------------------------------------------------------------------------------
   Function : bLoadAudioStreamByCRC
   Purpose : load and add audio stream info
   Parameters : package file, texture crc
   Returns : handle to stream, 0 if fail
   Info : 
*/

TBAudioStream *bLoadAudioStreamByCRC(TBPackageIndex *index, uint32 crc);


/* --------------------------------------------------------------------------------
   Function : bDeleteAudioStream
   Purpose : delete audio stream
   Parameters : stream handle
   Returns :
   Info : 
*/

void bDeleteAudioStream(TBAudioStream *stream);


/* --------------------------------------------------------------------------------
   Function : bsPlayAudioStream
   Purpose : play audio stream
   Parameters : stream handle, flags, volume (0-255), panning (+/-255), frequency (0 for default), callback function
   Returns : OK/FAIL
   Info : 
*/

int bsPlayAudioStream(TBAudioStream *stream, uint flags, uint volume, int pan, uint frequency, TBAudioStreamCallback callback=NULL, HANDLE fileHandle=NULL);


/* --------------------------------------------------------------------------------
   Function : bsStopAudioStream
   Purpose : stop audio stream
   Parameters : stream handle
   Returns :
   Info : 
*/

void bsStopAudioStream(TBAudioStream *stream);


/* --------------------------------------------------------------------------------
   Function : bsSetAudioStreamVolume
   Purpose : set audio stream volume and panning
   Parameters : stream handle, volume (0-255), panning(+/-255), subStream index or BSUBSTREAM_ALL
   Returns :
   Info : 
*/

int bsSetAudioStreamVolume(TBAudioStream *stream, uint volume, int pan, int subStream=BSUBSTREAM_ALL);


/*	--------------------------------------------------------------------------------
	Function : bsSetGlobalAudioStreamVolume
	Purpose : set the global stream playback volume
	Parameters : volume (0..255)
	Returns : 
	Info : 
*/

void bsSetGlobalAudioStreamVolume(int volume);


/*	--------------------------------------------------------------------------------
	Function : bsGetGlobalAudioStreamVolume
	Purpose : get the global stream playback volume
	Parameters : 
	Returns : volume (0..255)
	Info : 
*/

int bsGetGlobalAudioStreamVolume();


/* --------------------------------------------------------------------------------
   Function : bsSetAudioStreamFrequency
   Purpose : set audio stream playback frequency
   Parameters : stream handle, frequency (8000 - 48000), 0 reset to default
   Returns :
   Info : 
*/

int bsSetAudioStreamFrequency(TBAudioStream *stream, uint frequency);


/* --------------------------------------------------------------------------------
   Function : bsSetAudioStreamFlags
   Purpose : set audio stream flags
   Parameters : stream handle, flags
   Returns :OK/FAIL
   Info : 
*/

int bsSetAudioStreamFlags(TBAudioStream *stream, uint flags);


/* --------------------------------------------------------------------------------
   Function : bsClearAudioStreamFlags
   Purpose : clear audio stream flags
   Parameters : stream handle, flags
   Returns :OK/FAIL
   Info : 
*/

int bsClearAudioStreamFlags(TBAudioStream *stream, uint flags);


/* --------------------------------------------------------------------------------
   Function : bsGetAudioStreamFlags
   Purpose : get audio stream flags
   Parameters : stream handle, flags
   Returns :flags status
   Info : 
*/

int bsGetAudioStreamFlags(TBAudioStream *stream, uint flags);


/* --------------------------------------------------------------------------------
   Function : bsGetAudioStreamPosition
   Purpose : return the current play position of the stream in samples
   Parameters : stream
   Returns :current play position in samples
   Info : is an approximate indication
*/

int bsGetAudioStreamPosition(TBAudioStream *stream);


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
						  int *compressionRatio, int *noofMarkers, int **markers);


/*	--------------------------------------------------------------------------------
   Function : bsGetAudioStreamStatus
   Purpose : retrieve the current status of a sound stream
   Parameters : stream handle
 				 ptr for volume, ptr for pan, ptr for current frequency
   Returns : status
   Info : all ptrs except 'stream' can be NULL
*/

EBSoundStreamStatus bsGetAudioStreamStatus(TBAudioStream *stream,
										   int *volume=0, int *pan=0, int *frequency=0);


/* --------------------------------------------------------------------------------
   Function : bsFindAudioStreamMarker
   Purpose : return the marker at or immediately before a given sample offset
   Parameters : stream, sample offset
   Returns : marker number or -1 for none
   Info : uses a binary search for speed
*/

int bsFindAudioStreamMarker(TBAudioStream *stream, int sampleOffset);


/* --------------------------------------------------------------------------------
   Function : bHandleFillAudioStreams
   Purpose : Fill the audio streams as required
   Parameters : 
   Returns : 
   Info : WMA streams must be filled each frame
*/

void bHandleFillAudioStreams();


/* --------------------------------------------------------------------------------
   Function : bsAllocAudioStreamData
   Purpose : allocate static data for an audio stream
   Parameters : stream handle
   Returns : 1/0
   Info : 
*/

int bsAllocAudioStreamData(TBAudioStream *stream);


/* --------------------------------------------------------------------------------
   Function : bsFreeAudioStreamData
   Purpose : free static data for an audio stream
   Parameters : stream handle
   Returns :
   Info : 
*/

void bsFreeAudioStreamData(TBAudioStream *stream);


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
						  int *loading);


/* --------------------------------------------------------------------------------
   Function : bUpdateSound
   Purpose : Update all sound processing
   Parameters : 
   Returns :
   Info : Internal only
*/

void bUpdateSound();


#endif //__BSOUND_PCXBAUDIOSTREAM_H__
