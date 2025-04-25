// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : xbWave.h
//   Purpose : Xbox specific Sample playback routines
// Component : Xbox Sound
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BSOUND_XBWAVE_H__
#define __BSOUND_XBWAVE_H__

// ********************************************************************************
// Constants and Macros

// number of sound channels available
#define BNOOF_SOUND_CHANNELS	64


// ********************************************************************************
// Types, Structures and Classes

// info for a sound channel
typedef struct _TBSoundChannelInfoHAL {
	LPDIRECTSOUNDBUFFER8	soundBuffer;			// sound buffer we are using for this channel
} TBSoundChannelInfoHAL;


// info for a sample
typedef struct _TBSampleHAL {
	DSBUFFERDESC			bufferDesc;				// buffer description for this sample (16 bytes)
	WAVEFORMATEX			format;					// sample format (18 bytes)
	ushort					samplesPerBlock;		// #samples per block for ADPCM
	void					*data;					// sample data
	int32					noofSamples;			// #samples
	int32					dataSize;				// size of sample data in bytes
} TBSampleHAL;


// general sound information container
typedef struct TBSoundInfo {
	IDirectSound8			*dsound;				// primary DirectSound interface
	int						support3DSound;			// TRUE if our platform supports 3D sound
} TBSoundInfo;


// Sound modes
enum EBSoundMode {
	BSSOUNDMODE_MONO,				// Mono (single channel)
	BSSOUNDMODE_STEREO,				// Stereo (2 channel)
	BSSOUNDMODE_SURROUND,			// Surround	(Dolby ProLogic 5.1)

	BSSOUNDMODE_NOOF,
};

// ********************************************************************************
// Globals

// general sound info container
extern TBSoundInfo	bSoundInfo;

// LUT to convert 0.255 range value to decibels (0=silence, 255=full volume)
extern int bByteToDecibelLUT[256];


// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : bInitSpecificWave
	Purpose : initialise the specific wave functions
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/

int bInitSpecificWave();


/*	--------------------------------------------------------------------------------
	Function : bShutdownSpecificWave
	Purpose : shutdown the specific wave functions
	Parameters : 
	Returns : 
	Info : 
*/

void bShutdownSpecificWave();


/*	--------------------------------------------------------------------------------
	Function : bPlaySample
	Purpose : play a sample
	Parameters : sample, overall volume (0..255), pan (+-255), frequency Hz (0=default), flags, channel ID
	Returns : OK/FAIL
	Info : 
*/

int bPlaySample(struct _TBSample *sample, int volume, int pan, int frequency, uint flags, int channel);


/*	--------------------------------------------------------------------------------
	Function : bUpdateChannelVolume
	Purpose : update the volume status of a channel
	Parameters : channel
	Returns : 
	Info : 
*/

void bUpdateChannelVolume(int channel);


/*	--------------------------------------------------------------------------------
	Function : bStopChannel
	Purpose : stop a sample playing on a channel
	Parameters : channel
	Returns : 
	Info : 
*/

void bStopChannel(int channel);


/*	--------------------------------------------------------------------------------
	Function : bLoadSampleByCRC
	Purpose : load a sample referenced by CRC
	Parameters : package index (null for search only), crc
	Returns : ptr to sample or NULL for failure
	Info : 
*/

struct _TBSample *bLoadSampleByCRC(TBPackageIndex *index, uint32 crc);


/*	--------------------------------------------------------------------------------
	Function : bDeleteSample
	Purpose : delete a sample
	Parameters : sample ptr
	Returns : 
	Info : 
*/

void bDeleteSample(struct _TBSample *sample);


/*	--------------------------------------------------------------------------------
	Function : bUpdateSoundChannelStatus
	Purpose : update the playing status of a sound channel
	Parameters : channel number
	Returns : 
	Info : 
*/

void bUpdateSoundChannelStatus(int channelId);


/*	--------------------------------------------------------------------------------
	Function : bPauseChannel
	Purpose : Pause or unpause a channel
	Parameters : channel, BSPAUSE or BSUNPAUSE
	Returns : 
	Info : 
*/

void bPauseChannel(int channel, int pauseStatus);


/*	--------------------------------------------------------------------------------
	Function : bPlaySample3D
	Purpose : play a sample
	Parameters : sample, position, overall volume (0..255), frequency Hz (0=default), flags, channel ID, min distance, max distance
	Returns : OK/FAIL
	Info : 
*/

int bPlaySample3D(struct _TBSample *sample, TBVector pos, int volume, int frequency, uint flags, int channel, float minDistance, float maxDistance);


/*	--------------------------------------------------------------------------------
	Function : bUpdate3DSoundPosition
	Purpose : update the position of a 3D sound
	Parameters : channel, old position, new position
	Returns : 
	Info : 
*/

void bUpdate3DSoundPosition(int channel, TBVector oldPos, TBVector newPos);


/*	--------------------------------------------------------------------------------
	Function : bFinaliseUpdated3DSounds
	Purpose : finalise any changes made to 3D sound parameters during the bsUpdate function
	Parameters : 
	Returns : 
	Info : 
*/

void bFinaliseUpdated3DSounds();


/*	--------------------------------------------------------------------------------
	Function : bsSetSoundMode
	Purpose : select sound mode
	Parameters : sound mode to select
	Returns : OK/FAIL
	Info : 
*/

int bsSetSoundMode(EBSoundMode soundMode);


/*	--------------------------------------------------------------------------------
	Function : bsGetSoundMode
	Purpose : return sound mode
	Parameters : 
	Returns : sound mode
	Info : 
*/

EBSoundMode bsGetSoundMode();


#endif		// __BSOUND_XBWAVE_H__
