// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : wave.cpp
//   Purpose : generic sample playback functions
// Component : Generic Sound
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bInitGenericWave
	Purpose : initialise the generic wave functions
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/

int bInitGenericWave()
{
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bShutdownGenericWave
	Purpose : shutdown the generic wave functions
	Parameters : 
	Returns : 
	Info : 
*/

void bShutdownGenericWave()
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bsPlaySample
	Purpose : play a sample
	Parameters : sample, overall volume (0..255), pan (-255..255), frequency Hz (0=default), flags
	Returns : channel ID or -1 for failure
	Info : 
*/

int bsPlaySample(TBSample *sample, int volume, int pan, int frequency, uint flags)
{
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bsPlayPositionalSample
	Purpose : play a positional sample
	Parameters : sample, position, volume (0..255), frequency Hz (0=default), flags, min distance, max distance
	Returns : channel ID or -1 for failure
	Info : 
*/

int bsPlayPositionalSample(TBSample *sample, TBVector pos, int volume, int frequency, uint flags, float minDistance, float maxDistance)
{
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bsPlayTrackingSample
	Purpose : play a tracking sample
	Parameters : sample, position, volume (0..255), frequency Hz (0=default), flags, callback (or NULL), callback context,
																										min distance, max distance
	Returns : channel ID or -1 for failure
	Info : 
*/

int bsPlayTrackingSample(TBSample *sample, TBVector pos, int volume, int frequency, uint flags, TBTrackingSampleCallback callback,
																					void *context, float minDistance, float maxDistance)
{
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bsPlaySampleOnChannel
	Purpose : play a sample on a particular channel
	Parameters : channel number, sample, overall volume, pan (-255..255), frequency Hz (0=default), flags
	Returns : channel ID or -1 for failure
	Info : 
*/

int bsPlaySampleOnChannel(int channel, TBSample *sample, int volume, int pan, int frequency, uint flags)
{
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bsPlayPositionalSampleOnChannel
	Purpose : play a positional sample on a particular channel
	Parameters : channel number, sample, position, volume (0..255), frequency Hz (0=default), flags, min distance, max distance
	Returns : channel ID or -1 for failure
	Info : 
*/

int bsPlayPositionalSampleOnChannel(int channel, TBSample *sample, TBVector pos, int volume, int frequency, uint flags,
																								float minDistance, float maxDistance)
{
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bsPlayTrackingSampleOnChannel
	Purpose : play a tracking sample on a particular channel
	Parameters : channel number, sample, position, volume (0..255), frequency Hz (0=default), flags, callback (or NULL), callback context, min distance, max distance
	Returns : channel ID or -1 for failure
	Info : 
*/

int bsPlayTrackingSampleOnChannel(int channel, TBSample *sample, TBVector pos, int volume, int frequency, uint flags,
												TBTrackingSampleCallback callback, void *context, float minDistance, float maxDistance)
{
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bsGetMaxSampleChannel
	Purpose : return the maximum channel number available for sample playback
	Parameters : 
	Returns : channel number (so 0..X are available)
	Info : 
*/

int bsGetMaxSampleChannel()
{
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bsReserveChannel
	Purpose : reserve a channel number to prevent the dynamic allocator from using it
	Parameters : channel number, TRUE to reserve | FALSE to release
	Returns : 
	Info : 
*/

void bsReserveChannel(int channel, int newState)
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bsSetGlobalSampleVolume
	Purpose : set the global sample playback volume
	Parameters : volume (0..255)
	Returns : 
	Info : 
*/

void bsSetGlobalSampleVolume(int volume)
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bsStopChannel
	Purpose : stop a channel from playing
	Parameters : channel number
	Returns : 
	Info : 
*/

void bsStopChannel(int channel)
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bsStopAllChannels
	Purpose : stop all channels from playing
	Parameters : 
	Returns : 
	Info : 
*/

void bsStopAllChannels()
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bsSetChannelVolume
	Purpose : set the volume for a channel
	Parameters : channel, volume
	Returns : 
	Info : 
*/

void bsSetChannelVolume(int channel, int volume)
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bsSetChannelPan
	Purpose : set the pan-pot for a channel
	Parameters : channel, pan (-255...255)
	Returns : 
	Info : 
*/

void bsSetChannelPan(int channel, int pan)
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bsSetChannelFrequency
	Purpose : set the frequency of a channel
	Parameters : channel, frequency Hz (0=default)
	Returns : 
	Info : 
*/

void bsSetChannelFrequency(int channel, int frequency)
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bsUpdate
	Purpose : update all tracking samples
	Parameters : time delta for this frame
	Returns : 
	Info : 
*/

void bsUpdate(int timeDelta)
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bsGetNoofActiveChannels
	Purpose : return the number of channels that are currently playing
	Parameters : 
	Returns : #channels
	Info : 
*/

int bsGetNoofActiveChannels()
{
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bsSetChannelRange
	Purpose : set the minimum and maximum sound ranges for a tracking or positional sample
	Parameters : channel number, min distance in world units, max distance in world units
	Returns : 
	Info : 
*/

void bsSetChannelRange(int channel, float minDistance, float maxDistance)
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bsGetChannelStatus
	Purpose : retrieve the current status of a sound channel
	Parameters : channel number
	Returns : status
	Info : 
*/

EBSoundChannelStatus bsGetChannelStatus(int channel)
{
    return BSCHSTATUS_IDLE;
}


/*	--------------------------------------------------------------------------------
	Function : bsStopSample
	Purpose : Stop a sample playing any number of times on any channel(s).
	Parameters : [in] the sample to stop.
	Returns : 
	Info : 
*/

void bsStopSample(const TBSample * const sample)
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bsStopSampleGroup
	Purpose : Stops any samples being played with the specified group.
	Parameters : [in] the group to terminate.
	Returns : 
	Info : 
*/

void bsStopSampleGroup(const ushort groupId)
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bsPauseChannel
	Purpose : pause a channel
	Parameters : channel number, BSPAUSE or BSUNPAUSE
	Returns : 
	Info : 
*/

void bsPauseChannel(int channel, int pauseState)
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bsPauseAllChannels
	Purpose : pause or unpause all channels
	Parameters : BSPAUSE or BSUNPAUSE
	Returns : 
	Info : 
*/

void bsPauseAllChannels(int pauseState)
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bsPauseSample
	Purpose : Pause or unpause all playing instances of a sample
	Parameters : the sample to pause, BSPAUSE or BSUNPAUSE
	Returns : 
	Info : 
*/

void bsPauseSample(TBSample *sample, int pauseState)
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bsPauseSampleGroup
	Purpose : Pause or unpause all playing instances of all samples in a group
	Parameters : the group to pause/unpause, BSPAUSE or BSUNPAUSE
	Returns : 
	Info : 
*/

void bsPauseSampleGroup(ushort groupId, int pauseState)
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bsSetSampleFlags
	Purpose : Set and/or clear playback flags for a sample
	Parameters : sample to change flags for, bitmask of flags to change, new states for flags in bitmask
	Returns : 
	Info : 
*/

void bsSetSampleFlags(TBSample *sample, uint32 flagMask, uint32 newStates)
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bsSet3DDistanceFactor
	Purpose : Set the global distance scale for 3D sound
	Parameters : scale factor to convert meters into desired units (so to use feet pass 0.3048)
	Returns : 
	Info : 
*/

void bsSet3DDistanceFactor(float scaleToMeters)
{
    return;
}