// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcWave.cpp
//   Purpose : Xbox specific Sample playback routines
// Component : Xbox Sound
//   Comment : EXPECTED TO BE THE SAME AS xbWave.h, DIRECT REWRITE!
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bInitSpecificWave
	Purpose : initialise the specific wave functions
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/

int bInitSpecificWave()
{
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bShutdownSpecificWave
	Purpose : shutdown the specific wave functions
	Parameters : 
	Returns : 
	Info : 
*/

void bShutdownSpecificWave()
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bPlaySample
	Purpose : play a sample
	Parameters : sample, overall volume (0..255), pan (+-255), frequency Hz (0=default), flags, channel ID
	Returns : OK/FAIL
	Info : 
*/

int bPlaySample(struct _TBSample *sample, int volume, int pan, int frequency, uint flags, int channel)
{
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bUpdateChannelVolume
	Purpose : update the volume status of a channel
	Parameters : channel
	Returns : 
	Info : 
*/

void bUpdateChannelVolume(int channel)
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bStopChannel
	Purpose : stop a sample playing on a channel
	Parameters : channel
	Returns : 
	Info : 
*/

void bStopChannel(int channel)
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bLoadSampleByCRC
	Purpose : load a sample referenced by CRC
	Parameters : package index (null for search only), crc
	Returns : ptr to sample or NULL for failure
	Info : 
*/

struct _TBSample *bLoadSampleByCRC(TBPackageIndex *index, uint32 crc)
{
    return NULL;
}


/*	--------------------------------------------------------------------------------
	Function : bDeleteSample
	Purpose : delete a sample
	Parameters : sample ptr
	Returns : 
	Info : 
*/

void bDeleteSample(struct _TBSample *sample)
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bUpdateSoundChannelStatus
	Purpose : update the playing status of a sound channel
	Parameters : channel number
	Returns : 
	Info : 
*/

void bUpdateSoundChannelStatus(int channelId)
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bPauseChannel
	Purpose : Pause or unpause a channel
	Parameters : channel, BSPAUSE or BSUNPAUSE
	Returns : 
	Info : 
*/

void bPauseChannel(int channel, int pauseStatus)
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bPlaySample3D
	Purpose : play a sample
	Parameters : sample, position, overall volume (0..255), frequency Hz (0=default), flags, channel ID, min distance, max distance
	Returns : OK/FAIL
	Info : 
*/

int bPlaySample3D(struct _TBSample *sample, TBVector pos, int volume, int frequency, uint flags, int channel, float minDistance, float maxDistance)
{
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bUpdate3DSoundPosition
	Purpose : update the position of a 3D sound
	Parameters : channel, old position, new position
	Returns : 
	Info : 
*/

void bUpdate3DSoundPosition(int channel, TBVector oldPos, TBVector newPos)
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bFinaliseUpdated3DSounds
	Purpose : finalise any changes made to 3D sound parameters during the bsUpdate function
	Parameters : 
	Returns : 
	Info : 
*/

void bFinaliseUpdated3DSounds()
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bsSetSoundMode
	Purpose : select sound mode
	Parameters : sound mode to select
	Returns : OK/FAIL
	Info : 
*/

int bsSetSoundMode(EBSoundMode soundMode)
{
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bsGetSoundMode
	Purpose : return sound mode
	Parameters : 
	Returns : sound mode
	Info : 
*/

EBSoundMode bsGetSoundMode()
{
    return BSSOUNDMODE_STEREO;
}