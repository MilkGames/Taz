// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : wave.h
//   Purpose : generic sample playback functions
// Component : Generic Sound
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BSOUND_WAVE_H__
#define __BSOUND_WAVE_H__

// ********************************************************************************
// Constants and Macros

// channel status values
enum EBSoundChannelStatus {
	BSCHSTATUS_IDLE,				// channel is idle (the default state)
	BSCHSTATUS_PLAYING,				// channel is playing
	BSCHSTATUS_PAUSED				// channel is paused
};


// channel flags
#define BSCHANFLAG_RESERVED			0x00000001					// channel has been reserved by client
#define BSCHANFLAG_TRACKING			0x00000002					// channel is tracking
#define BSCHANFLAG_3D				0x00000004					// is a 3D channel


// sample playback flags
#define BSPLAYFLAG_EXCLUSIVE		0x00000001					// stop any other instances that are playing
#define BSPLAYFLAG_IGNORE			0x00000002					// don't play if already playing
#define BSPLAYFLAG_GROUPEXCLUSIVE	0x00000004					// stop any other instances of samples in the same groups that are playing
#define BSPLAYFLAG_GROUPIGNORE		0x00000008					// don't play if already playing any sample from the same groups
#define BSPLAYFLAG_LOOPING			0x00000010					// this is a looping sample (set by packager)
#define BSPLAYFLAG_FORCELOOPING		0x00000020					// force this sample to loop (if possible)
#define BSPLAYFLAG_FORCENOTLOOPING	0x00000040					// force this sample not to loop (if possible)
#define BSPLAYFLAG_CLAMPATMAXDIST	0x00000080					// Zero 3D volume when beyond the max distance
#define BSPLAYFLAG_CLAMPEDATMAXDIST	0x00000100					// Its been clamped beyond the max distance
#define BSPLAYFLAG_ONESHOT_ZEROARAM	0x10000000					// (GameCube only) use the zero ARAM area on one shot samples

// invalid channel number
#define BSINVALID_CHANNEL			-1

// pause flags
#define BSPAUSE						1							// pause playback
#define BSUNPAUSE					0							// unpause playback

// stream flags
#define BSTREAMFLAG_LOOPING			0x00000001					// stream should loop
#define BSTREAMFLAG_STEREO			0x00000002					// stream is stereo
#define BSTREAMFLAG_COMPRESSED		0x00000004					// stream is compressed (should always be set)
#define BSTREAMFLAG_STATICDATA		0x00000008					// set if stream is using static data buffer
#define BSTREAMFLAG_PAUSE			0x00000010					// pause stream
#define BSTREAMFLAG_PRESEEK			0x00000020					// pre-seek stream at the beginning
#define BSTREAMFLAG_SEEK			0x00000040					// seek within stream
#define BSTREAMFLAG_PRELOADING		0x00000080					// the stream is still pre-loading data [PS2 only]
#define BSTREAMFLAG_HALMASK			0xff000000					// top eight bits are reserved for hardware specific flags used internally during playback


// ********************************************************************************
// Types, Structures and Classes

// tracking sample update callback
typedef void (*TBTrackingSampleCallback)(TBVector pos, int timeDelta, void *context);


// info for a sound channel
typedef struct _TBSoundChannelInfo {
	TBVector						position;					// stored position
	TBVector						oldPosition;				// old stored position (for doppler)

	EBSoundChannelStatus			status;						// status of channel
	struct _TBSample				*sample;					// the sample that is currently playing/played last
	float							*positionPtr;				// position for dynamic tracking
	TBTrackingSampleCallback		trackingCallback;			// tracking update callback

	void							*trackingContext;			// context for tracking callback
	uint32							flags;						// channel flags (see BSCHANFLAG_)
	ushort							volume, clientVolume;		// current volume, client specified volume
	int32							pan;						// pan position (+-255)

	float							maxSoundDistance;			// maximum distance from which a sound can be heard (tracking only)
	float							minSoundDistance;			// minimum distance within which stereo effect is scaled (tracking only)
	uint32							playFlags;					// playback flags (BSPLAYFLAG_)
	uint32							pad2;
	
	TBSoundChannelInfoHAL			hal;						// hardware specific info
} TBSoundChannelInfo BALIGN16;


// info for a sample
typedef struct _TBSample {
	TBResourceInfo					resInfo;					// global resource info

	int32							frequency;					// default frequency
	uint32							playbackFlags;				// default flags for playback (XOR'ed with supplied flags)
	uchar							noofChannels;				// #channels (1=mono, 2=stereo)
	uchar							pad[3];
	uint32							pad2;

	TBSampleHAL						hal;						// hardware specific info
} TBSample;


// ********************************************************************************
// Globals


extern TBSoundChannelInfo				bSoundChannel[BNOOF_SOUND_CHANNELS];		// sound channel info array
extern int								bGlobalSampleVolume;						// global sample volume


// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : bInitGenericWave
	Purpose : initialise the generic wave functions
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/

int bInitGenericWave();


/*	--------------------------------------------------------------------------------
	Function : bShutdownGenericWave
	Purpose : shutdown the generic wave functions
	Parameters : 
	Returns : 
	Info : 
*/

void bShutdownGenericWave();


/*	--------------------------------------------------------------------------------
	Function : bsPlaySample
	Purpose : play a sample
	Parameters : sample, overall volume (0..255), pan (-255..255), frequency Hz (0=default), flags
	Returns : channel ID or -1 for failure
	Info : 
*/

int bsPlaySample(TBSample *sample, int volume, int pan, int frequency, uint flags);


/*	--------------------------------------------------------------------------------
	Function : bsPlayPositionalSample
	Purpose : play a positional sample
	Parameters : sample, position, volume (0..255), frequency Hz (0=default), flags, min distance, max distance
	Returns : channel ID or -1 for failure
	Info : 
*/

int bsPlayPositionalSample(TBSample *sample, TBVector pos, int volume, int frequency, uint flags, float minDistance, float maxDistance);


/*	--------------------------------------------------------------------------------
	Function : bsPlayTrackingSample
	Purpose : play a tracking sample
	Parameters : sample, position, volume (0..255), frequency Hz (0=default), flags, callback (or NULL), callback context,
																										min distance, max distance
	Returns : channel ID or -1 for failure
	Info : 
*/

int bsPlayTrackingSample(TBSample *sample, TBVector pos, int volume, int frequency, uint flags, TBTrackingSampleCallback callback,
																					void *context, float minDistance, float maxDistance);


/*	--------------------------------------------------------------------------------
	Function : bsPlaySampleOnChannel
	Purpose : play a sample on a particular channel
	Parameters : channel number, sample, overall volume, pan (-255..255), frequency Hz (0=default), flags
	Returns : channel ID or -1 for failure
	Info : 
*/

int bsPlaySampleOnChannel(int channel, TBSample *sample, int volume, int pan, int frequency, uint flags);


/*	--------------------------------------------------------------------------------
	Function : bsPlayPositionalSampleOnChannel
	Purpose : play a positional sample on a particular channel
	Parameters : channel number, sample, position, volume (0..255), frequency Hz (0=default), flags, min distance, max distance
	Returns : channel ID or -1 for failure
	Info : 
*/

int bsPlayPositionalSampleOnChannel(int channel, TBSample *sample, TBVector pos, int volume, int frequency, uint flags,
																								float minDistance, float maxDistance);


/*	--------------------------------------------------------------------------------
	Function : bsPlayTrackingSampleOnChannel
	Purpose : play a tracking sample on a particular channel
	Parameters : channel number, sample, position, volume (0..255), frequency Hz (0=default), flags, callback (or NULL), callback context, min distance, max distance
	Returns : channel ID or -1 for failure
	Info : 
*/

int bsPlayTrackingSampleOnChannel(int channel, TBSample *sample, TBVector pos, int volume, int frequency, uint flags,
												TBTrackingSampleCallback callback, void *context, float minDistance, float maxDistance);


/*	--------------------------------------------------------------------------------
	Function : bsGetMaxSampleChannel
	Purpose : return the maximum channel number available for sample playback
	Parameters : 
	Returns : channel number (so 0..X are available)
	Info : 
*/

int bsGetMaxSampleChannel();


/*	--------------------------------------------------------------------------------
	Function : bsReserveChannel
	Purpose : reserve a channel number to prevent the dynamic allocator from using it
	Parameters : channel number, TRUE to reserve | FALSE to release
	Returns : 
	Info : 
*/

void bsReserveChannel(int channel, int newState);


/*	--------------------------------------------------------------------------------
	Function : bsSetGlobalSampleVolume
	Purpose : set the global sample playback volume
	Parameters : volume (0..255)
	Returns : 
	Info : 
*/

void bsSetGlobalSampleVolume(int volume);


/*	--------------------------------------------------------------------------------
	Function : bsStopChannel
	Purpose : stop a channel from playing
	Parameters : channel number
	Returns : 
	Info : 
*/

void bsStopChannel(int channel);


/*	--------------------------------------------------------------------------------
	Function : bsStopAllChannels
	Purpose : stop all channels from playing
	Parameters : 
	Returns : 
	Info : 
*/

void bsStopAllChannels();


/*	--------------------------------------------------------------------------------
	Function : bsSetChannelVolume
	Purpose : set the volume for a channel
	Parameters : channel, volume
	Returns : 
	Info : 
*/

void bsSetChannelVolume(int channel, int volume);


/*	--------------------------------------------------------------------------------
	Function : bsSetChannelPan
	Purpose : set the pan-pot for a channel
	Parameters : channel, pan (-255...255)
	Returns : 
	Info : 
*/

void bsSetChannelPan(int channel, int pan);


/*	--------------------------------------------------------------------------------
	Function : bsSetChannelFrequency
	Purpose : set the frequency of a channel
	Parameters : channel, frequency Hz (0=default)
	Returns : 
	Info : 
*/

void bsSetChannelFrequency(int channel, int frequency);


/*	--------------------------------------------------------------------------------
	Function : bsUpdate
	Purpose : update all tracking samples
	Parameters : time delta for this frame
	Returns : 
	Info : 
*/

void bsUpdate(int timeDelta);


/*	--------------------------------------------------------------------------------
	Function : bsGetNoofActiveChannels
	Purpose : return the number of channels that are currently playing
	Parameters : 
	Returns : #channels
	Info : 
*/

int bsGetNoofActiveChannels();


/*	--------------------------------------------------------------------------------
	Function : bsSetChannelRange
	Purpose : set the minimum and maximum sound ranges for a tracking or positional sample
	Parameters : channel number, min distance in world units, max distance in world units
	Returns : 
	Info : 
*/

void bsSetChannelRange(int channel, float minDistance, float maxDistance);


/*	--------------------------------------------------------------------------------
	Function : bsGetChannelStatus
	Purpose : retrieve the current status of a sound channel
	Parameters : channel number
	Returns : status
	Info : 
*/

EBSoundChannelStatus bsGetChannelStatus(int channel);


/*	--------------------------------------------------------------------------------
	Function : bsStopSample
	Purpose : Stop a sample playing any number of times on any channel(s).
	Parameters : [in] the sample to stop.
	Returns : 
	Info : 
*/

void bsStopSample(const TBSample * const sample);


/*	--------------------------------------------------------------------------------
	Function : bsStopSampleGroup
	Purpose : Stops any samples being played with the specified group.
	Parameters : [in] the group to terminate.
	Returns : 
	Info : 
*/

void bsStopSampleGroup(const ushort groupId);


/*	--------------------------------------------------------------------------------
	Function : bsPauseChannel
	Purpose : pause a channel
	Parameters : channel number, BSPAUSE or BSUNPAUSE
	Returns : 
	Info : 
*/

void bsPauseChannel(int channel, int pauseState);


/*	--------------------------------------------------------------------------------
	Function : bsPauseAllChannels
	Purpose : pause or unpause all channels
	Parameters : BSPAUSE or BSUNPAUSE
	Returns : 
	Info : 
*/

void bsPauseAllChannels(int pauseState);


/*	--------------------------------------------------------------------------------
	Function : bsPauseSample
	Purpose : Pause or unpause all playing instances of a sample
	Parameters : the sample to pause, BSPAUSE or BSUNPAUSE
	Returns : 
	Info : 
*/

void bsPauseSample(TBSample *sample, int pauseState);


/*	--------------------------------------------------------------------------------
	Function : bsPauseSampleGroup
	Purpose : Pause or unpause all playing instances of all samples in a group
	Parameters : the group to pause/unpause, BSPAUSE or BSUNPAUSE
	Returns : 
	Info : 
*/

void bsPauseSampleGroup(ushort groupId, int pauseState);


/*	--------------------------------------------------------------------------------
	Function : bsSetSampleFlags
	Purpose : Set and/or clear playback flags for a sample
	Parameters : sample to change flags for, bitmask of flags to change, new states for flags in bitmask
	Returns : 
	Info : 
*/

void bsSetSampleFlags(TBSample *sample, uint32 flagMask, uint32 newStates);


/*	--------------------------------------------------------------------------------
	Function : bsSet3DDistanceFactor
	Purpose : Set the global distance scale for 3D sound
	Parameters : scale factor to convert meters into desired units (so to use feet pass 0.3048)
	Returns : 
	Info : 
*/

void bsSet3DDistanceFactor(float scaleToMeters);

#endif		// __BSOUND_WAVE_H__
