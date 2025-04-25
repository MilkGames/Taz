// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : sfx.cpp
//   Purpose : sound effect rountines
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "main.h"

#include "animation.h"
#include "sfx.h"
#include "files.h"
#include "actors.h"
#include "status.h"
#include "vehicles.h"
#include "maths.h"
#include "events.h"
#include "cutscene.h"
#include "securitybox.h"

//#define DISABLE_SOUNDFX

int		maxSoundChannels=0;

#define LOADCOSTUMESOUND

#ifdef TONY
//#define DONTUPLOADSOUND
#endif

// IH: --------------- GameCube specific reverb stuff ---------------
#if BPLATFORM==GAMECUBE
//#define USE_REVERB
#include <dolphin/axfx.h>
AXFX_REVERBSTD		reverbSmallRoom;
AXFX_REVERBSTD		reverbBigRoom;
AXFX_DELAY			delayBigCavern;
int					soundEffect = 0;
int					auxAFadeType;
float				auxAVolume = -AUXA_MAX_VOLUME;
#endif
// IH: --------------- End GameCube specific reverb stuff ---------------


#if(BPLATFORM == XBOX)

	// PP: default maximum distance that positional sound can travel and still be heard
	#define SFX_DEFAULT_HEARING_RANGE		METERTOUNIT(10.0f)

	// PP: default distance farther than which positional sound starts to fade
	#define SFX_DEFAULT_MIN_DIST			METERTOUNIT(2.5f)

#else// PP: platform

	// PP: default maximum distance that positional sound can travel and still be heard
	#define SFX_DEFAULT_HEARING_RANGE		2500.0f		// PP: 10 metres, as it's always been

	// PP: default distance farther than which positional sound starts to fade
	#define SFX_DEFAULT_MIN_DIST			125.0f		// PP: 0.5 metres, as before

#endif// PP: platform

#if(BPLATFORM == XBOX)// PP: TT: This is the only platform that currently supports the BSPLAYFLAG_CLAMPATMAXDIST flag - other platforms just ignore it.

	// PP: define SFX_CLAMP_AT_MAX_DIST to (TT:) Clamp volume to zero at the maximum distance
	// PP:		- This flags is only for use with 3D sounds. Without this flag the volume may
	// PP:		just get set low rather than being clamped right to zero

	// PP: NOTE: xbox 3d sound attenuation is not linear, and at the max dist, volume is not naturally at 0.
	// PP: That's why you can hear the cutoff.  Nothing I can do about it though, without writing my own positional/tracking sound code.
	// PP: Tech Team say the falloff curve can't be changed.
	#define	SFX_CLAMP_AT_MAX_DIST

#endif// PP: platform

// PP: the maximum distance that positional sound can travel and still be heard
float SFX_maxDist		=SFX_DEFAULT_HEARING_RANGE;

// PP: distance at which sound starts to fade out - THIS CAN'T BE ZERO!
float SFX_minDist		=SFX_DEFAULT_MIN_DIST;

typedef struct TAG_SOUNDCALLBACK
{
	void (*soundCallback)(void *context);
	void *soundCallbackContext;
} SOUNDCALLBACK;

SOUNDCALLBACK  *soundCallbacks=NULL;

// PP: #define CORRECT_XBOX_NONPOSITIONAL_VOLUME(_volume)	((_volume)/=3)
#define CORRECT_XBOX_NONPOSITIONAL_VOLUME(_volume)	((_volume)/*/=3*/)
int		randomAmbients[5];
float	randomTimers[5];


/*  --------------------------------------------------------------------------------
	Function	: InitialiseSoundFX
	Purpose		: Setup the sound system callbacks
	Parameters	: 
	Returns		: true/false
	Info		: 
*/

bool InitialiseSoundFX(void)
{
	int	i;

	maxSoundChannels = bsGetMaxSampleChannel();
	bkPrintf("%d sound channels available for sound effects.\n", maxSoundChannels);

	soundCallbacks = (SOUNDCALLBACK*)MALLOC(maxSoundChannels*sizeof(SOUNDCALLBACK));
	ASSERTM(soundCallbacks, "*** ERROR *** Could not allocate memory for the sound system callbacks\n");

	for (i=0; i<maxSoundChannels; i++)
	{
		soundCallbacks[i].soundCallback = NULL;
		soundCallbacks[i].soundCallbackContext = 0;
	}

	return true;
}


/*	--------------------------------------------------------------------------------
	Function 	: LoadSamples
	Purpose 	: Load all samples from a pak file
	Parameters 	: pakfile
	Returns 	: 
	Info 		:
*/

int LoadSamples(char *package)
{
#ifndef DISABLE_SOUNDFX
	TBPackageIndex	*pakIndex;

	bkPrintf("LoadSamples(%s)\n",package);

	if (pakIndex = FindPackage(package))
	{
		// package already resident
#ifndef DONTUPLOADSOUND
		bkLoadPackageSamples(pakIndex);
#endif
	}
	else
	{
		bkPrintf("*** ERROR *** Package %s not loaded\n",package);
		return FALSE;
	}
#endif
	return FALSE;
}

/*	--------------------------------------------------------------------------------
	Function 	: FreeSamples
	Purpose 	: Free all samples loaded from a pak file
	Parameters 	: pakfile
	Returns 	: 
	Info 		:
*/
void FreeSamples(char *package)
{
#ifndef DISABLE_SOUNDFX
	TBPackageIndex	*pakIndex;

	// return if loaded from resident package
	if (pakIndex = FindPackage(package))
	{
		return;
	}
/*
	bkPrintf("FreeSamples(%s)\n",package);
	if ((pakIndex = bkOpenPackage(package)))
	{
		bkDeletePackageSamples(bkGetPackageID(pakIndex));
		bkClosePackage(pakIndex);
	}*/
#endif
}


/*  --------------------------------------------------------------------------------
	Function	: LoadSample
	Purpose		: Used to find a sample in memory, this function will also lose the loop\\ if need be
	Parameters	: name of sample
	Returns		: TBSample
	Info		: 
*/

TBSample	*LoadSample(char *name)
{
	TBSample	*sample;
	char		*tempChar;
	char		tempString[256];

	sample = bkLoadSample(NULL, name, NULL);
	if (!sample) 
	{
		if (tempChar = FindChar(name,'\\'))
		{
			// TP: try removing loop
			tempChar += 1; //+= 2;		// CMD: should we not be removing just one \ from the string?
			sample = bkLoadSample(NULL, tempChar, NULL);
		}
		else
		{
			// TP: try adding loop
			sprintf(tempString, "loop\\%s", name);
			sample = bkLoadSample(NULL, tempString, NULL);
		}
		if (!sample) return NULL;
	}
	return sample;
}


/*	--------------------------------------------------------------------------------
	Function 	: PlaySample
	Purpose 	: Play a sample
	Parameters 	: sample name, (opt/default)volume, (opt/NULL=non-positional)position, (opt/0=none)flags
	Returns 	: channel 
	Info 		:
*/
int PlaySample(char *name, int volume, TBVector position, uint32 flags)
{
#ifdef DISABLE_SOUNDFX
	return -1;
#else
	
	// IH: GameCube only reverb
#if BPLATFORM==GAMECUBE
	int		 retVal;
#endif

	TBSample *sample;

	if (!(sample = LoadSample(name))) return -1;

	if (position)
	{

		// PP: on Xbox, we have to clamp the volume to 0 at maximum distance (the falloff curve is crap)
#ifdef SFX_CLAMP_AT_MAX_DIST
		flags |= BSPLAYFLAG_CLAMPATMAXDIST;
#endif// PP: def SFX_CLAMP_AT_MAX_DIST

	#ifdef USE_REVERB
		// IH: If it is not in Babel, do it by hand
		retVal = bsPlayPositionalSample(sample,position,volume,0,flags,SFX_minDist,SFX_maxDist);
		if( retVal >= 0 )
			SetupSoundEffect(retVal);
		return retVal;
	#else
		int channel;
		channel = bsPlayPositionalSample(sample,position,volume,0,flags,SFX_minDist,SFX_maxDist);
#if BPLATFORM==XBOX
		if (channel!=-1) 
		{
			// TP: temp fix for Babel 3d sound
			bSoundChannel[channel].positionPtr = bSoundChannel[channel].position;
			bSoundChannel[channel].flags |= BSCHANFLAG_TRACKING;
			bSoundChannel[channel].trackingCallback = NULL;
		}
#endif
		return channel;
	#endif
		//return bsPlayPositionalSample(sample,position,volume,0,flags,METERTOUNIT(1000),METERTOUNIT(1000));
	}
	else
	{
/*#if(BPLATFORM == XBOX)
		CORRECT_XBOX_NONPOSITIONAL_VOLUME(volume);
#endif// PP: (BPLATFORM == XBOX)*/

	#ifdef USE_REVERB
		// IH: If it is not in Babel, do it by hand
		retVal = bsPlaySample(sample,volume,0,0,flags);
		if( retVal >= 0 )
			SetupSoundEffect(retVal);
		return retVal;
	#else
		return bsPlaySample(sample,volume,0,0,flags);
	#endif

	}
#endif
}

/*	--------------------------------------------------------------------------------
	Function 	: PlaySample
	Purpose 	: Play a sample
	Parameters 	: (char *name, int volume, int pan, int frequency, TBVector position, uint32 flags)
	Returns 	: channel
	Info 		:
*/
int PlaySample(char *name, int volume, int pan, int frequency, TBVector position, uint32 flags)
{
#ifndef DISABLE_SOUNDFX
	TBSample *sample;
	char	*tempChar;

#ifdef USE_REVERB
	int		retVal;
#endif

	if (!(sample = LoadSample(name))) return -1;

	if (position)
	{
		// PP: on Xbox, we have to clamp the volume to 0 at maximum distance (the falloff curve is crap)
#ifdef SFX_CLAMP_AT_MAX_DIST
		flags |= BSPLAYFLAG_CLAMPATMAXDIST;
#endif// PP: def SFX_CLAMP_AT_MAX_DIST

	#ifdef USE_REVERB
		// IH: If it is not in Babel, do it by hand
		retVal = bsPlayPositionalSample(sample,position,volume,frequency,flags,SFX_minDist,SFX_maxDist);
		if( retVal >= 0 )
			SetupSoundEffect(retVal);
		return retVal;
	#else
		int channel;

		channel = bsPlayPositionalSample(sample,position,volume,frequency,flags,SFX_minDist,SFX_maxDist);
#if BPLATFORM==XBOX
		if (channel!=-1) 
		{
			// TP: temp fix for Babel 3d sound
			bSoundChannel[channel].positionPtr = bSoundChannel[channel].position;
			bSoundChannel[channel].flags |= BSCHANFLAG_TRACKING;
			bSoundChannel[channel].trackingCallback = NULL;
		}
#endif
		return channel;

	#endif
	}
	else
	{
/*#if(BPLATFORM == XBOX)
		CORRECT_XBOX_NONPOSITIONAL_VOLUME(volume);
#endif// PP: (BPLATFORM == XBOX)*/
	#ifdef USE_REVERB
		// IH: If it is not in Babel, do it by hand
		retVal = bsPlaySample(sample,volume,pan,frequency,flags);
		if( retVal >= 0 )
			SetupSoundEffect(retVal);
		return retVal;
	#else
		return bsPlaySample(sample,volume,pan,frequency,flags);
	#endif
	}
#else
	return -1;
#endif
}


/*  --------------------------------------------------------------------------------
	Function	: StopChannel
	Purpose		: Called to stop a currently playing sample
	Parameters	: pointer to sampleHandle
	Returns		: 
	Info		: the sampleHandle will be set to -1 when stopped
*/

void StopChannel(int *sampleHandle)
{
	int	sampleHandleCopy = *sampleHandle;

	if ((*sampleHandle < -1)||(*sampleHandle >= maxSoundChannels)) return;

	if (*sampleHandle != -1)
	{
		// TP: check if there is a callback on this channel
		if (soundCallbacks[*sampleHandle].soundCallback)
		{
			soundCallbacks[sampleHandleCopy].soundCallback(soundCallbacks[sampleHandleCopy].soundCallbackContext);
			soundCallbacks[sampleHandleCopy].soundCallback = NULL;
			bsReserveChannel(sampleHandleCopy,FALSE);
		}

		// TP: stop channel if callback hasn't already done so
		bsStopChannel(sampleHandleCopy);
		*sampleHandle = -1;

		bkPrintf("Stop channel %d\n", sampleHandleCopy);
	}
}


/*  --------------------------------------------------------------------------------
	Function	: GetCossiePakfileForLevel
	Purpose		: Returns the name of the pakfile containing the costume sounds for this level
	Parameters	: char *name = buffer for the name of the pakfile to be returned, level to find sounds for
	Returns		: 
	Info		: 
*/

void GetCossiePakfileForLevel(char *name, int level)
{
	switch(level)
	{
	case SCENE_MUSEUMVEHICLERACE:
		strcpy(name, "museum");
		break;
	case SCENE_GRANDCVEHICLERACE:
		strcpy(name, "grandc");
		break;
	case SCENE_DEPTSTRVEHICLERACE:
		strcpy(name, "deptstr");
		break;
	case SCENE_DESTRUCTIONAQUA:
		strcpy(name, "aqua");
		break;
	case SCENE_DESTRUCTIONCONSTRUCT:
		strcpy(name, "contruct");
		break;
	case SCENE_DESTRUCTIONTASMANIA:
		strcpy(name, "ghost");
		break;
	case SCENE_ICEDOMERACE:
		strcpy(name, "icedome");
		break;
	case SCENE_GOLDMINERACE:
		strcpy(name, "goldmine");
		break;
	case SCENE_SAFARIRACE:
		strcpy(name, "safari");
		break;
	default:
		strcpy(name, map.mapName);
		break;
	}
	strcat(name, "cossie");
}


/*	--------------------------------------------------------------------------------
	Function 	: SetupSoundEffect
	Purpose 	: Setup a sound effect on a specific channel
	Parameters 	: sound channel
	Returns 	: channel
	Info 		: GameCube only (although it could also be done on XBox if you have the time....)
				  Assumes channel is > -1
*/
#if BPLATFORM==GAMECUBE
void SetupSoundEffect(int channel)
{
	// IH: Set the Aux A channel to be a post fader
	MIXAuxAPostFader(bSoundChannel[channel].hal.soundBuffer);
	// IH: Set the mix volume on the Aux A channel
	MIXSetAuxA(bSoundChannel[channel].hal.soundBuffer, auxAVolume);
}
#endif

/*	--------------------------------------------------------------------------------
	Function 	: PlayTrackingSample
	Purpose 	: Play a sample at a movable position in world space
	Parameters 	: sample name; volume (0..255); position (MUST STAY RESIDENT WHILE SOUND PLAYS);
					(opt/0)flags; (opt/0=normal)frequency;
					(opt/SFX_minDist)minimum distance, any closer than which the sound will be heard at full volume;
					(opt/SFX_maxDist)maximum distance, any farther than which the sound will not be heard
	Returns 	: the channel on which the sound plays
	Info 		:
*/
int PlayTrackingSample(char *name, int volume, TBVector position, uint32 flags, int frequency, const float min, const float max, void (*TBTrackingSampleCallback)(TBVector pos, int timeDelta, void *context), void *context)
{
#ifdef DISABLE_SOUNDFX

return -1;

#else

	TBSample *sample;

	float smin = min;
	float srange = max;

	if (smin == -1)
		smin = SFX_minDist;

	if (srange == -1)
		srange = SFX_maxDist;

	if (!(sample = LoadSample(name))) return -1;

		// PP: on Xbox, we have to clamp the volume to 0 at maximum distance (the falloff curve is crap)
#ifdef SFX_CLAMP_AT_MAX_DIST
	flags |= BSPLAYFLAG_CLAMPATMAXDIST;
#endif// PP: def SFX_CLAMP_AT_MAX_DIST

	return bsPlayTrackingSample(sample,position,volume,frequency,flags,TBTrackingSampleCallback,context,smin,srange);
	//return bsPlayTrackingSample(sample,position,volume,frequency,flags,NULL,0,METERTOUNIT(1000),METERTOUNIT(1000));
	
#endif
}


/*	--------------------------------------------------------------------------------
	Function 	: ResetSoundCallbacks
	Purpose 	: Reset the status of all soundcallbacks
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void ResetSoundCallbacks(void)
{
	int i;

	for (i=0;i<maxSoundChannels;i++)
	{
		soundCallbacks[i].soundCallback = NULL;
		soundCallbacks[i].soundCallbackContext = NULL;
		bsReserveChannel(i,FALSE);
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: CallbackWhenSampleFinished
	Purpose 	: Add callback to sound handle
	Parameters 	: sound handle, callback function (format void Function(void *context)), context
	Returns 	: channel
	Info 		:
*/
void CallbackWhenSampleFinished(int handle,void (*callback)(void *context),void *context)
{
	if ((handle<=-1)||(handle>=maxSoundChannels)) return;
	bsReserveChannel(handle,TRUE);
	soundCallbacks[handle].soundCallback = callback;
	soundCallbacks[handle].soundCallbackContext = context;
}


/*	--------------------------------------------------------------------------------
	Function 	: MonitorSampleCallbacks
	Purpose 	: Check if samples have finished and callbacks are waiting
	Parameters 	: 
	Returns 	: 
	Info 		: Called once a frame.
*/

void MonitorSampleCallbacks(void)
{
	int i;

	for (i=0;i<maxSoundChannels;i++)
	{
		if (soundCallbacks[i].soundCallback)
		{
			// TP: There is a callback on channel i
			if (bsGetChannelStatus(i) == BSCHSTATUS_IDLE)
			{
				bsReserveChannel(i,FALSE);
				soundCallbacks[i].soundCallback(soundCallbacks[i].soundCallbackContext);
				soundCallbacks[i].soundCallback = NULL;
			}
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: InitCharacterSoundHandles
	Purpose 	: Initialises character sound handles structure
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void InitCharacterSoundHandles(CHARACTERSOUNDHANDLES *handles)
{
	if(handles)
	{
		handles->taz = -1;
		handles->sheDevil = -1;
		handles->sam = -1;
		handles->tweety = -1;
		handles->daffy = -1;
		handles->tribal = -1;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: SFX_sceneInit
	Purpose 	: Sound initialisation at the beginning of each scene
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: 
*/
void SFX_sceneInit(void)
{
	// init character sounds
	InitCharacterSoundHandles(&characterSounds);
	
	// PP: Reset hearing range to defaults, since that's probably going to be a sorta per-scene thing (eg. for West Boss, cos the camera's so far away from the action)
	SetHearingRange(SFX_DEFAULT_HEARING_RANGE);

	//SFX_maxDist=METERTOUNIT(1);
	//SFX_minDist=METERTOUNIT(1);

//	bsSetGlobalSampleVolume(0);
}


/*	--------------------------------------------------------------------------------
	Function 	: SFX_gameInit
	Purpose 	: Sound initialisation at the beginning of the game
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: 
*/
void SFX_gameInit(void)
{
	// PP: Set our distance scale for 3D surround sound
	// PP: TT: "The factor by which to scale your world units to get them into meters"
	// PP: = METERTOUNIT(1) = 250
	// PP: But of course, what they mean is
	// PP: "The factor by which to scale a DISTANCE in world units to get it into meters"
	// PP: = UNITTOMETER(1) = 0.025f
	bsSet3DDistanceFactor(UNITTOMETER(1));

#if(BPLATFORM == XBOX)
	// PP: this makes positional sound attenuation linear, making it more like the PS2 version, which can't be bad thing

	/* PP: REMOUT: TEST		
	bSoundInfo.support3DSound = FALSE;
	/*/
	bSoundInfo.support3DSound = TRUE;
	//*/

#endif// PP: (BPLATFORM == XBOX)
}


/*	--------------------------------------------------------------------------------
	Function 	: CharacterSoundsFinishedCallback
	Purpose 	: Callback when character sound has finished playing
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void CharacterSoundsFinishedCallback(void *context)
{
	int			*characterHandle;

	characterHandle = (int*)context;
	if(characterHandle)
	{
		*characterHandle = -1;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: SetHearingRange
	Purpose 	: Set the maximum distance that positional sound can travel and still be heard
	Parameters 	: the maximum distance that positional sound can travel and still be heard (in units),
					or INFINTE_HEARING_RANGE if you want infinite range
	Returns 	: 
	Info 		: // PP: removed default parameter cos I wanted to move sound range defines out of header
*/
void SetHearingRange(const float range)
{
//#if(BPLATFORM != XBOX)// PP: TEMP TEST

	if(range == INFINTE_HEARING_RANGE)
	{
		SFX_maxDist=FLOATMAX;
	}
	else
	{
		SFX_maxDist=range;
	}

//#endif
}

/*	--------------------------------------------------------------------------------
	Function 	: SetHearingRange
	Purpose 	: Set the minimum and maximum distance that positional sound can travel and still be heard
	Parameters 	: the maximum distance that positional sound can travel and still be heard (in units),
					or INFINTE_HEARING_RANGE if you want infinite range
	Returns 	: 
	Info 		: 
*/

void SetHearingRange(const float minRange, const float maxRange)
{
	SFX_minDist = minRange;

	if(maxRange == INFINTE_HEARING_RANGE)
	{
		SFX_maxDist=FLOATMAX;
	}
	else
	{
		SFX_maxDist=maxRange;
	}
}

/*  --------------------------------------------------------------------------------
	Function	: LoadLevelSamplesBackground
	Purpose		: Start the background load for ressound and any level sound packages used
	Parameters	: 
	Returns		: 
	Info		: 
*/

void LoadLevelSamplesBackground()
{
	char	tempChar[256];
	char	tempChar2[256];
	bool	costume = false;

	// TP: Load the generic samples
	if (map.sceneNumber != SCENE_ZOOHUB)
		LoadWholePackageBkg(AppendCountryCode(tempChar,"ressounds"));

	switch (map.sceneNumber)
	{
		case SCENE_FRONTEND:
			LoadWholePackageBkg("frontendsound");	
			break;

		case SCENE_LANGUAGESELECT:
			LoadWholePackageBkg("languagesound");
			break;
		
		case SCENE_ZOOBOSS:
			LoadWholePackageBkg(AppendCountryCode(tempChar,"zoobosssound"));
			break;

		case SCENE_AQUA:
			LoadWholePackageBkg(AppendCountryCode(tempChar,"aquaintrosound"));
			LoadWholePackageBkg("aquasound");
			costume = true;
			break;

		case SCENE_SAFARI:
			LoadWholePackageBkg(AppendCountryCode(tempChar,"safariintrosound"));
			LoadWholePackageBkg("safarisound");
			costume = true;
			break;

		case SCENE_ICEDOME:		
			LoadWholePackageBkg(AppendCountryCode(tempChar,"icedomeintrosound"));
			LoadWholePackageBkg("icedomesound");
			costume = true;
			break;

		case SCENE_ZOOHUB:
			LoadWholePackageBkg(AppendCountryCode(tempChar,"zoohubsound"));		
			break;
		
		case SCENE_CITYHUB:		
			LoadWholePackageBkg("cityhubsound");	
			break;

		case SCENE_DEPTSTR:		
			LoadWholePackageBkg(AppendCountryCode(tempChar,"deptstrintrosound"));	
			LoadWholePackageBkg("deptstrsound");
			costume = true;
			break;

		case SCENE_MUSEUM:		
			LoadWholePackageBkg(AppendCountryCode(tempChar,"museumintrosound"));
			LoadWholePackageBkg("museumsound");		
			costume = true;
			break;

		case SCENE_CONSTRUCT:	
			LoadWholePackageBkg(AppendCountryCode(tempChar,"constructintrosound"));
			LoadWholePackageBkg("constructsound");	
			costume = true;
			break;

		case SCENE_CITYBOSS:	
			LoadWholePackageBkg(AppendCountryCode(tempChar,"citybosssound"));	
			break;

		case SCENE_WILDWESTHUB:	
			LoadWholePackageBkg("westhubsound");
			break;

		case SCENE_GOLDMINE:	
			LoadWholePackageBkg(AppendCountryCode(tempChar,"goldmineintrosound"));
			LoadWholePackageBkg("goldminesound");
			costume = true;
			break;

		case SCENE_GHOSTTOWN:	
			LoadWholePackageBkg(AppendCountryCode(tempChar,"ghosttownintrosound"));
			LoadWholePackageBkg("ghosttownsound");	
			costume = true;
			break;

		case SCENE_GRANDCANYON:	
			LoadWholePackageBkg(AppendCountryCode(tempChar,"canyonintrosound"));
			LoadWholePackageBkg("canyonsound");		
			costume = true;
			break;

		case SCENE_WESTBOSS:
			LoadWholePackageBkg(AppendCountryCode(tempChar,"westbosssound"));	
			break;

		case SCENE_TAZHUB:		
			LoadWholePackageBkg(AppendCountryCode(tempChar,"tasmaniaintrosound"));
			LoadWholePackageBkg("tasmaniasound");	
			costume = true;
			break;

		case SCENE_TAZPREBOSS:	
			LoadWholePackageBkg(AppendCountryCode(tempChar,"tazboss1sound"));	
			break;

		case SCENE_TAZBOSS:		
			LoadWholePackageBkg(AppendCountryCode(tempChar,"tazboss2sound"));	
			break;

		case SCENE_MUSEUMVEHICLERACE:		
			LoadWholePackageBkg(AppendCountryCode(tempChar,"museumvrsound"));		
			break;

		case SCENE_GRANDCVEHICLERACE:		
			LoadWholePackageBkg(AppendCountryCode(tempChar,"grandcvrsound"));		
			break;

		case SCENE_DEPTSTRVEHICLERACE:		
			LoadWholePackageBkg(AppendCountryCode(tempChar,"deptstrvrsound"));		
			break;

		case SCENE_DESTRUCTIONAQUA:			
			LoadWholePackageBkg(AppendCountryCode(tempChar,"aquadesound"));			
			break;

		case SCENE_DESTRUCTIONCONSTRUCT:	
			LoadWholePackageBkg(AppendCountryCode(tempChar,"constructdesound"));	
			break;

		case SCENE_DESTRUCTIONTASMANIA:		
			LoadWholePackageBkg(AppendCountryCode(tempChar,"tasmaniadesound"));		
			break;

		case SCENE_ICEDOMERACE:				
			LoadWholePackageBkg(AppendCountryCode(tempChar,"icedometasound"));		
			break;

		case SCENE_GOLDMINERACE:			
			LoadWholePackageBkg(AppendCountryCode(tempChar,"goldminetasound"));		
			break;

		case SCENE_SAFARIRACE:				
			LoadWholePackageBkg(AppendCountryCode(tempChar,"safaritasound"));		
			break;
	}

#ifdef LOADCOSTUMESOUND
	if (costume)
	{
		GetCossiePakfileForLevel(tempChar2, map.sceneNumber);
		LoadWholePackageBkg(AppendCountryCode(tempChar,tempChar2));
	}
#endif
}


/*  --------------------------------------------------------------------------------
	Function	: LoadLevelSamplesMemory
	Purpose		: Load the level sounds into memory and free the main memory used
	Parameters	: 
	Returns		: 
	Info		: 
*/

void LoadLevelSamplesMemory()
{
	TBPackageIndex	*pak;
	char			samplepakname[256];
	char			tempChar2[256];
	char			tempChar[256];

	if (map.sceneNumber != SCENE_ZOOHUB)
	{
		// TP: Load samples into sound ram
		if (pak = FindPackage(AppendCountryCode(samplepakname,"ressounds")))
		{
			bkLoadPackageSamples(pak);
		}
	}

	bkPrintf("\n\n Sound memory left %d\n\n",bkARAMHeapFreeSpace());

	#if BPLATFORM==GAMECUBE
		// IH: Can only free a pak that exists
		if( pak )
		{
			// IH: This is a bit of a bodge: Loop until the DMA has finished doing its magic
			while(!bkFreePackageMemory(&pak))
			{
				bkUpdate();
			}
			UpdateIndex(samplepakname, pak);
		}
	#endif

#ifdef LOADCOSTUMESOUND
	GetCossiePakfileForLevel(tempChar2, map.sceneNumber);
	if (pak = FindPackage(AppendCountryCode(samplepakname,tempChar2)))
	{
#ifndef DONTUPLOADSOUND
		bkLoadPackageSamples(pak);
#endif
	}
	
	bkPrintf("\n\n Sound memory left %d\n\n",bkARAMHeapFreeSpace());

	#if BPLATFORM==GAMECUBE
		// IH: Can only free a pak that exists
		if( pak )
		{
			// IH: This is a bit of a bodge: Loop until the DMA has finished doing its magic
			while(!bkFreePackageMemory(&pak))
			{
				bkUpdate();
			}
			UpdateIndex(samplepakname, pak);
		}
	#endif
#endif

	samplepakname[0] = 0;

	// TP: List for localised sound packages
	switch(map.sceneNumber)
	{
		case SCENE_CITYBOSS:	strcpy(samplepakname, "citybosssound");		break;
		case SCENE_ZOOBOSS:		strcpy(samplepakname, "zoobosssound");		break;
		case SCENE_WESTBOSS:	strcpy(samplepakname, "westbosssound");		break;
		case SCENE_TAZPREBOSS:	strcpy(samplepakname, "tazboss1sound");		break;
		case SCENE_TAZBOSS:		strcpy(samplepakname, "tazboss2sound");		break;
		case SCENE_ZOOHUB:		strcpy(samplepakname, "zoohubsound");		break;

		case SCENE_MUSEUMVEHICLERACE:		strcpy(samplepakname, "museumvrsound");		break;
		case SCENE_GRANDCVEHICLERACE:		strcpy(samplepakname, "grandcvrsound");		break;
		case SCENE_DEPTSTRVEHICLERACE:		strcpy(samplepakname, "deptstrvrsound");	break;

		case SCENE_DESTRUCTIONAQUA:			strcpy(samplepakname, "aquadesound");		break;
		case SCENE_DESTRUCTIONCONSTRUCT:	strcpy(samplepakname, "constructdesound");	break;
		case SCENE_DESTRUCTIONTASMANIA:		strcpy(samplepakname, "tasmaniadesound");	break;

		case SCENE_ICEDOMERACE:				strcpy(samplepakname, "icedometasound");	break;
		case SCENE_GOLDMINERACE:			strcpy(samplepakname, "goldminetasound");	break;
		case SCENE_SAFARIRACE:				strcpy(samplepakname, "safaritasound");		break;

	}

	// IH: Note to self: The list of cases above is actually different to the list below
	if (samplepakname[0]!=0)
	{
		AppendCountryCode(samplepakname, samplepakname);
	}

	// TP: List for generic sound packages (NOT LOCALISED)
	switch (map.sceneNumber)
	{
		case SCENE_FRONTEND:	strcpy(samplepakname, "frontendsound");		break;
		case SCENE_LANGUAGESELECT:strcpy(samplepakname, "languagesound");	break;
		
		case SCENE_AQUA:		strcpy(samplepakname, "aquasound");			break;
		case SCENE_SAFARI:		strcpy(samplepakname, "safarisound");		break;
		case SCENE_ICEDOME:		strcpy(samplepakname, "icedomesound");		break;
		
		case SCENE_CITYHUB:		strcpy(samplepakname, "cityhubsound");		break;
		case SCENE_DEPTSTR:		strcpy(samplepakname, "deptstrsound");		break;
		case SCENE_MUSEUM:		strcpy(samplepakname, "museumsound");		break;
		case SCENE_CONSTRUCT:	strcpy(samplepakname, "constructsound");	break;

		case SCENE_WILDWESTHUB:	strcpy(samplepakname, "westhubsound");		break;
		case SCENE_GOLDMINE:	strcpy(samplepakname, "goldminesound");		break;
		case SCENE_GHOSTTOWN:	strcpy(samplepakname, "ghosttownsound");	break;
		case SCENE_GRANDCANYON:	strcpy(samplepakname, "canyonsound");		break;

		case SCENE_TAZHUB:		strcpy(samplepakname, "tasmaniasound");		break;

	}

	if (pak = FindPackage(samplepakname))
	{
#ifndef DONTUPLOADSOUND

		int32	numSamplesLoaded;

		numSamplesLoaded=bkLoadPackageSamples(pak);

		if(numSamplesLoaded == 0)
		{
			bkPrintf("LoadLevelSamplesMemory: NO SAMPLES LOADED FROM PACKAGE \"%s\"!\n", samplepakname);
		}
		else
		{
			bkPrintf("LoadLevelSamplesMemory: %d samples loaded from package \"%s\"\n", numSamplesLoaded, samplepakname);
		}
#endif
	}

#if BPLATFORM == PS2
	// TP: Free the samples
	FreeWholePackage(samplepakname);

	if (map.sceneNumber != SCENE_ZOOHUB)
		FreeWholePackage(AppendCountryCode(samplepakname,"ressounds"));

#ifdef LOADCOSTUMESOUND
	GetCossiePakfileForLevel(tempChar2, map.sceneNumber);
	FreeWholePackage(AppendCountryCode(tempChar,tempChar2));
#endif 
#endif

#if BPLATFORM == GAMECUBE
	// IH: Can only free a pak that exists
	if( pak )
	{
		// IH: This is a bit of a bodge: Loop until the DMA has finished doing its magic
		while(!bkFreePackageMemory(&pak))
		{
			bkUpdate();
		}
		UpdateIndex(samplepakname, pak);
	}
#endif

	bkPrintf("\n\n Sound memory left %d\n\n",bkARAMHeapFreeSpace());
}

void LoadLevelIntroSamplesMemory()
{
	TBPackageIndex	*pak;
	char			samplepakname[256];

	samplepakname[0] = 0;

	switch (map.sceneNumber)
	{
		case SCENE_AQUA:		strcpy(samplepakname, "aquaintrosound");		break;
		case SCENE_SAFARI:		strcpy(samplepakname, "safariintrosound");		break;
		case SCENE_ICEDOME:		strcpy(samplepakname, "icedomeintrosound");		break;
		
		case SCENE_DEPTSTR:		strcpy(samplepakname, "deptstrintrosound");		break;
		case SCENE_MUSEUM:		strcpy(samplepakname, "museumintrosound");		break;
		case SCENE_CONSTRUCT:	strcpy(samplepakname, "constructintrosound");	break;

		case SCENE_GOLDMINE:	strcpy(samplepakname, "goldmineintrosound");	break;
		case SCENE_GHOSTTOWN:	strcpy(samplepakname, "ghosttownintrosound");	break;
		case SCENE_GRANDCANYON:	strcpy(samplepakname, "canyonintrosound");		break;

		case SCENE_TAZHUB:		strcpy(samplepakname, "tasmaniaintrosound");	break;
	}

	if (samplepakname[0]!=0)
	{
		AppendCountryCode(samplepakname, samplepakname);
	}

	if (pak = FindPackage(samplepakname))
	{
#ifndef DONTUPLOADSOUND
		bkLoadPackageResources(pak,BRESMASK_SAMPLE,255,NULL);
#endif
	}

#if BPLATFORM == PS2
	// TP: Free the samples
	FreeWholePackage(samplepakname);
#endif

#if BPLATFORM == GAMECUBE
	// IH: Can only free a pak if it exists
	if( pak )
	{
		// IH: This is a bit of a bodge: Loop until the DMA has finished doing its magic
		while(!bkFreePackageMemory(&pak))
		{
			bkUpdate();
		}
		UpdateIndex(samplepakname, pak);
	}
#endif

	bkPrintf("\n\n Sound memory left %d\n\n",bkARAMHeapFreeSpace());
}


/*  --------------------------------------------------------------------------------
	Function	: FreeLevelIntroSamples
	Purpose		: Delete all samples associated with the package ground 255
	Parameters	: 
	Returns		: 
	Info		: 
*/

void FreeLevelIntroSamples()
{
	char	samplepakname[256];

	samplepakname[0] = 0;

	switch (map.sceneNumber)
	{
		case SCENE_AQUA:		strcpy(samplepakname, "aquaintrosound");		break;
		case SCENE_SAFARI:		strcpy(samplepakname, "safariintrosound");		break;
		case SCENE_ICEDOME:		strcpy(samplepakname, "icedomeintrosound");		break;
		
		case SCENE_DEPTSTR:		strcpy(samplepakname, "deptstrintrosound");		break;
		case SCENE_MUSEUM:		strcpy(samplepakname, "museumintrosound");		break;
		case SCENE_CONSTRUCT:	strcpy(samplepakname, "constructintrosound");	break;

		case SCENE_GOLDMINE:	strcpy(samplepakname, "goldmineintrosound");	break;
		case SCENE_GHOSTTOWN:	strcpy(samplepakname, "ghosttownintrosound");	break;
		case SCENE_GRANDCANYON:	strcpy(samplepakname, "canyonintrosound");		break;

		case SCENE_TAZHUB:		strcpy(samplepakname, "tasmaniaintrosound");	break;
	}

	if (samplepakname[0]!=0)
	{
		AppendCountryCode(samplepakname, samplepakname);
	}

	FreeWholePackage(samplepakname);
	
	bkDeleteResourceGroup(BRESMASK_SAMPLE, 255);

	bkPrintf("\n\n Sound memory left %d\n\n",bkARAMHeapFreeSpace());
}


/*  --------------------------------------------------------------------------------
	Function	: FreeLevelSamples
	Purpose		: Free any level packages and clear out sound ram
	Parameters	: 
	Returns		: 
	Info		: 
*/

void FreeLevelSamples()
{
	char	samplepakname[256];
	char	tempChar[256];

	samplepakname[0] = 0;

	// TP: List for localised sound packages
	switch(map.sceneNumber)
	{
		case SCENE_CITYBOSS:	strcpy(samplepakname, "citybosssound");		break;
		case SCENE_ZOOBOSS:		strcpy(samplepakname, "zoobosssound");		break;
		case SCENE_WESTBOSS:	strcpy(samplepakname, "westbosssound");		break;
		case SCENE_TAZPREBOSS:	strcpy(samplepakname, "tazboss1sound");		break;
		case SCENE_TAZBOSS:		strcpy(samplepakname, "tazboss2sound");		break;		
		case SCENE_ZOOHUB:		strcpy(samplepakname, "zoohubsound");		break;

		case SCENE_MUSEUMVEHICLERACE:		strcpy(samplepakname, "museumvrsound");		break;
		case SCENE_GRANDCVEHICLERACE:		strcpy(samplepakname, "grandcvrsound");		break;
		case SCENE_DEPTSTRVEHICLERACE:		strcpy(samplepakname, "deptstrvrsound");	break;

		case SCENE_DESTRUCTIONAQUA:			strcpy(samplepakname, "aquadesound");		break;
		case SCENE_DESTRUCTIONCONSTRUCT:	strcpy(samplepakname, "constructdesound");	break;
		case SCENE_DESTRUCTIONTASMANIA:		strcpy(samplepakname, "tasmaniadesound");	break;

		case SCENE_ICEDOMERACE:				strcpy(samplepakname, "icedometasound");	break;
		case SCENE_GOLDMINERACE:			strcpy(samplepakname, "goldminetasound");	break;
		case SCENE_SAFARIRACE:				strcpy(samplepakname, "safaritasound");		break;

	}

	if (samplepakname[0]!=0)
	{
		AppendCountryCode(samplepakname, samplepakname);
	}

	// TP: List for generic sound packages (NOT LOCALISED)
	switch (map.sceneNumber)
	{
		case SCENE_FRONTEND:	strcpy(samplepakname, "frontendsound");		break;
		case SCENE_LANGUAGESELECT:strcpy(samplepakname, "languagesound");	break;
		
		case SCENE_AQUA:		strcpy(samplepakname, "aquasound");			break;
		case SCENE_SAFARI:		strcpy(samplepakname, "safarisound");		break;
		case SCENE_ICEDOME:		strcpy(samplepakname, "icedomesound");		break;
		
		case SCENE_CITYHUB:		strcpy(samplepakname, "cityhubsound");		break;
		case SCENE_DEPTSTR:		strcpy(samplepakname, "deptstrsound");		break;
		case SCENE_MUSEUM:		strcpy(samplepakname, "museumsound");		break;
		case SCENE_CONSTRUCT:	strcpy(samplepakname, "constructsound");	break;

		case SCENE_WILDWESTHUB:	strcpy(samplepakname, "westhubsound");		break;
		case SCENE_GOLDMINE:	strcpy(samplepakname, "goldminesound");		break;
		case SCENE_GHOSTTOWN:	strcpy(samplepakname, "ghosttownsound");	break;
		case SCENE_GRANDCANYON:	strcpy(samplepakname, "canyonsound");		break;

		case SCENE_TAZHUB:		strcpy(samplepakname, "tasmaniasound");		break;

	}

	FreeWholePackage(samplepakname);

	if (map.sceneNumber != SCENE_ZOOHUB)
		FreeWholePackage(AppendCountryCode(samplepakname,"ressounds"));

#ifdef LOADCOSTUMESOUND
	GetCossiePakfileForLevel(samplepakname, map.sceneNumber);
	FreeWholePackage(AppendCountryCode(tempChar,samplepakname));
#endif

	bkDeleteAllSamples();

	bkPrintf("\n\n Sound memory left %d\n\n",bkARAMHeapFreeSpace());
}


/*  --------------------------------------------------------------------------------
	Function	: LoadCharacterSamplesBkg
	Purpose		: Start the background load of samples for the character specified
	Parameters	: namein - name of the character
	Returns		: 
	Info		: 
*/

void LoadCharacterSamplesBkg(char *namein)
{
	TBPackageIndex	*pak;
	char			name[256];

	// TP: did this incase the string being passed in only has memory space enough for the namein string itself
	sprintf(name, "%ssound", namein);
	// TP: strcat(name, "sound");

	AppendCountryCode(name, name);

	LoadWholePackageBkg(name,NULL, 2);
}


/*  --------------------------------------------------------------------------------
	Function	: LoadCharacterSamples
	Purpose		: Loads the characters samples into sound ram and frees the main ram used
	Parameters	: namein - name of character
	Returns		: 
	Info		: 
*/

void LoadCharacterSamples(char *namein)
{
	TBPackageIndex	*pak;
	char			name[256];

	// TP: did this incase the string being passed in only has memory space enough for the namein string itself
	sprintf(name, "%ssound", namein);

	AppendCountryCode(name, name);
	// TP: strcat(name, "sound");
	if (pak = FindPackage(name))
	{
#ifndef DONTUPLOADSOUND
		bkLoadPackageSamples(pak);
#endif

	#if BPLATFORM == PS2
		FreeWholePackage(name);
	#endif

	#if BPLATFORM==GAMECUBE
		// IH: Can only free a pak that exists
		if( pak )
		{
			// IH: This is a bit of a bodge: Loop until the DMA has finished doing its magic
			while(!bkFreePackageMemory(&pak))
			{
				bkUpdate();
			}
			UpdateIndex(name, pak);
		}
	#endif
	}

	return;

	bkPrintf("\n\n Sound memory left %d\n\n",bkARAMHeapFreeSpace());
}


/*  --------------------------------------------------------------------------------
	Function	: PlayAmbientSounds
	Purpose		: Start any programmer controlled ambient sounds for the level
	Parameters	: current scene
	Returns		: 
	Info		: 
*/

void PlayAmbientSounds(int scene)
{
	TBVector			pVector;

	ACTORINSTANCE		*pActor;

	switch (scene)
	{
	case SCENE_FRONTEND:	PlaySample("loop\\ambisland.wav", 255);
							break;

	case SCENE_ZOOHUB:		// TP: just changed this cause I don't have the right level yet
							if (pActor = FindActorInstanceInMap("aquaentrance", 0, &map))
							{
								PlayTrackingSample("ambbeach.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(12.5f));		//Aqua ambience
							}
							
							if (pActor = FindActorInstanceInMap("safarientrance", 0, &map))
							{
								PlayTrackingSample("ambjungle.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(15.0f), METERTOUNIT(35.0f));	//Safari ambience
							}
							
							if (pActor = FindActorInstanceInMap("iceentrance", 0, &map))
							{
								PlayTrackingSample("ambwinter.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(8.0f), METERTOUNIT(20.0f));	//Ice ambience
							}
							
							if (pActor = FindActorInstanceInMap("gardencenter", 0, &map))
							{
								PlayTrackingSample("ambgarden.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(10.0f), METERTOUNIT(30.0f));	//Garden ambience
							}

							if (pActor = FindActorInstanceInMap("watercenter01", 0, &map))
							{
								PlayTrackingSample("waterfall.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(10.0f), METERTOUNIT(15.0f));	//Garden ambience
							}

							break;

	case SCENE_CITYHUB:		// TP: just changed this cause I don't have the right level yet
							if (pActor = FindActorInstanceInMap("flower sprites", 0, &map))
							{
								PlayTrackingSample("loop\\ambcitynight.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(12.5f), METERTOUNIT(20.0f));		//Centre ambience
							}
							
							if (pActor = FindActorInstanceInMap("trolley02", 0, &map))
							{
								PlayTrackingSample("loop\\ambmall.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(15.0f), METERTOUNIT(20.0f));	//Mall ambience
							}

							if (pActor = FindActorInstanceInMap("gate outer left", 0, &map))
							{
								PlayTrackingSample("loop\\ambconstruction.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(15.0f), METERTOUNIT(20.0f));	//Ice ambience
							}

							if (pActor = FindActorInstanceInMap("tram(reversed)", 0, &map))
							{
								PlayTrackingSample("loop\\tram1.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(2.5f), METERTOUNIT(10.0f));	//Garden ambience
							}

							if (pActor = FindActorInstanceInMap("mesh04", 0, &map))
							{
								PlayTrackingSample("loop\\tram1.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(2.5f), METERTOUNIT(10.0f));	//Garden ambience
							}

							if (pActor = FindActorInstanceInMap("bank - mixer lorry", 0, &map))
							{
								PlayTrackingSample("loop\\cmixer.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(10.0f), METERTOUNIT(15.0f));	//Garden ambience
							}

							if (pActor = FindActorInstanceInMap("siren_i01", 0, &map))
							{
								PlayTrackingSample("loop\\siren.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(10.0f));	//Garden ambience
							}

							if (pActor = FindActorInstanceInMap("siren_i02", 0, &map))
							{
								PlayTrackingSample("loop\\siren.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(10.0f));	//Garden ambience
							}

							if (pActor = FindActorInstanceInMap("siren_i03", 0, &map))
							{
								PlayTrackingSample("loop\\siren.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(10.0f));	//Garden ambience
							}

							if (pActor = FindActorInstanceInMap("siren_i04", 0, &map))
							{
								PlayTrackingSample("loop\\siren.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(10.0f));	//Garden ambience
							}

							if (pActor = FindActorInstanceInMap("siren_i05", 0, &map))
							{
								PlayTrackingSample("loop\\siren.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(10.0f));	//Garden ambience
							}

							if (pActor = FindActorInstanceInMap("van_wheels01", 0, &map))
							{
								PlayTrackingSample("loop\\hydrant.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(12.0f));	//Garden ambience
							}
	
							PlaySample("loop\\rain2.wav", 255);

							break;

	case SCENE_SAFARI:		if (pActor = FindActorInstanceInMap("bank - s_truck new null01", 0, &map))
							{		
								PlayTrackingSample("loop\\jeep.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(20.0f));		//Centre ambience
							}

							if (pActor = FindActorInstanceInMap("flock_box bees01", 0, &map))
							{
								PlayTrackingSample("loop\\ambbees.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(12.5f));		//Centre ambience
							}

							if (pActor = FindActorInstanceInMap("flock_box bees02", 0, &map))
							{
								PlayTrackingSample("loop\\ambbees.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(12.5f));		//Centre ambience
							}

							break;

	case SCENE_AQUA:		if (pActor = FindActorInstanceInMap("washing_machine", 0, &map))
							{		
								PlayTrackingSample("loop\\washloop.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(20.0f));		//Centre ambience
							}

							PlaySample("loop\\ambbeach.wav", 128);

							break;

	case SCENE_ICEDOME:		if (pActor = FindActorInstanceInMap("wocket_inst", 0, &map))
							{		
								//PlayTrackingSample("loop\\washloop.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(20.0f));		//Centre ambience
							}

							if (pActor = FindActorInstanceInMap("cable_car_new01", 0, &map))
							{
								PlayTrackingSample("loop\\cablecar.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(2.5f), METERTOUNIT(10.0f));		//Centre ambience
							}

							if (pActor = FindActorInstanceInMap("square_table-inst", 0, &map))
							{
								PlayTrackingSample("loop\\subsonar.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(3.0f), METERTOUNIT(6.0f));		//Centre ambience
								PlayTrackingSample("loop\\secretroom.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(4.0f), METERTOUNIT(8.0f));	//Centre ambience
							}

							break;

	case SCENE_MUSEUM:		if (pActor = FindActorInstanceInMap("hotdog_base02", 0, &map))
							{
								PlayTrackingSample("loop\\laserhum.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(2.5f), METERTOUNIT(10.0f));		//laser beams
							}

							if (pActor = FindActorInstanceInMap("pouring_boucket_molten01", 0, &map))
							{
								PlayTrackingSample("loop\\redgoo.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(2.5f), METERTOUNIT(10.0f));		//lava bucket
							}

							break;

	case SCENE_DEPTSTRVEHICLERACE:
	case SCENE_DEPTSTR:		if (pActor = FindActorInstanceInMap("escalator1l", 0, &map))
							{
								PlayTrackingSample("loop\\escalator.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(20.0f));
							}

							if (pActor = FindActorInstanceInMap("escalator1r", 0, &map))
							{
								PlayTrackingSample("loop\\escalator.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(20.0f));
							}

							if (pActor = FindActorInstanceInMap("escalator2l", 0, &map))
							{
								PlayTrackingSample("loop\\escalator.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(2.5f), METERTOUNIT(10.0f));
							}

							if (pActor = FindActorInstanceInMap("escalator2r", 0, &map))
							{
								PlayTrackingSample("loop\\escalator.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(2.5f), METERTOUNIT(10.0f));
							}

							if (pActor = FindActorInstanceInMap("escalator3l", 0, &map))
							{
								PlayTrackingSample("loop\\escalator.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(2.5f), METERTOUNIT(10.0f));
							}

							if (pActor = FindActorInstanceInMap("escalator3r", 0, &map))
							{
								PlayTrackingSample("loop\\escalator.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(2.5f), METERTOUNIT(10.0f));
							}

							break;

	case SCENE_CONSTRUCT:	if (pActor = FindActorInstanceInMap("bank - zeppelin null", 0, &map))
							{
								PlayTrackingSample("loop\\airship.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(2.5f), METERTOUNIT(10.0f));
							}

							break;

	case SCENE_WILDWESTHUB:	if (pActor = FindActorInstanceInMap("fire01", 0, &map))
							{
								PlayTrackingSample("loop\\firelp2.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(2.5f), METERTOUNIT(10.0f));
							}

							if (pActor = FindActorInstanceInMap("stats_ghost", 0, &map))
							{
								PlayTrackingSample("loop\\spookywindloop.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(20.0f));
							}

							if (pActor = FindActorInstanceInMap("stats_goldmine", 0, &map))
							{
								PlayTrackingSample("loop\\mineambloop.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(20.0f));
							}

							if (pActor = FindActorInstanceInMap("stat_grand canyon", 0, &map))
							{
								PlayTrackingSample("loop\\windloop.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(20.0f));
							}

							if (pActor = FindActorInstanceInMap("stat_westboss", 0, &map))
							{
								PlayTrackingSample("loop\\windloop.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(20.0f));
							}

							if (pActor = FindActorInstanceInMap("train_root_object01", 0, &map))
							{
								PlayTrackingSample("loop\\tram2.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(20.0f));
							}

							if (pActor = FindActorInstanceInMap("buzzard_bounding_box01", 0, &map))
							{
								PlayTrackingSample("loop\\crow1.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(10.0f), METERTOUNIT(40.0f));
							}

							if (pActor = FindActorInstanceInMap("boss sign", 0, &map))
							{
								PlayTrackingSample("loop\\shopsign.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(2.5f), METERTOUNIT(10.0f));
							}

							break;

	case SCENE_GRANDCANYON:	if (pActor = FindActorInstanceInMap("buzzard_bounding_box01", 0, &map))
							{
								PlayTrackingSample("loop\\crow1.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(10.0f), METERTOUNIT(40.0f));
							}

							if (pActor = FindActorInstanceInMap("camp_fire01", 0, &map))
							{
								PlayTrackingSample("loop\\firelp2.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(10.0f), METERTOUNIT(40.0f));
							}

							break;

	case SCENE_GHOSTTOWN:	if (pActor = FindActorInstanceInMap("wheel_inst", 0, &map))
							{
								PlayTrackingSample("loop\\waterwheel.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(2.5f), METERTOUNIT(10.0f));
							}
						
							if (pActor = FindActorInstanceInMap("blade_supports20", 0, &map))
							{
								PlayTrackingSample("loop\\spinningblade1.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(10.0f));
							}

							if (pActor = FindActorInstanceInMap("blade_supports16", 0, &map))
							{
								PlayTrackingSample("loop\\spinningblade1.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(2.5f), METERTOUNIT(10.0f));
							}

							if (pActor = FindActorInstanceInMap("blade_supports18", 0, &map))
							{
								PlayTrackingSample("loop\\spinningblade1.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(2.5f), METERTOUNIT(10.0f));
							}

							if (pActor = FindActorInstanceInMap("blade_supports09", 0, &map))
							{
								PlayTrackingSample("loop\\spinningblade1.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(2.5f), METERTOUNIT(10.0f));
							}

							if (pActor = FindActorInstanceInMap("blade_supports13", 0, &map))
							{
								PlayTrackingSample("loop\\spinningblade1.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(10.0f));
							}

							break;
	}

	int	i;

	for (i = 0; i < 5; i++)
	{
		randomAmbients[i] = -1;
		randomTimers[i] = 0.0f;
	}
}


/*  --------------------------------------------------------------------------------
	Function	: PlayRandomAmbients
	Purpose		: Play other random level sounds
	Parameters	: current scene
	Returns		: 
	Info		: 
*/

void PlayRandomAmbients(int scene)
{
	ACTORINSTANCE		*pActor;

	switch (scene)
	{
	case SCENE_SAFARI:	if (randomAmbients[0] == -1)	// JW: Rope bridge creak
						{
							if (randomTimers[0] < gameStatus.gameTime)
							{
								if (pActor = FindActorInstanceInMap("bridge", 0, &map))
								{
									CallbackWhenSampleFinished((randomAmbients[0]=PlayTrackingSample("ropebridgecreak.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(20.0f))),CharacterSoundsFinishedCallback,&randomAmbients[0]);

									randomTimers[0] = gameStatus.gameTime + UtilGenerateRandomNumber(4.0f, 6.0f);
								}
							}
						}

						if (randomAmbients[1] == -1)	// JW: eye blinks
						{
							if (randomTimers[1] < gameStatus.gameTime)
							{
								if (bmRand()%2)
								{
									if (pActor = FindActorInstanceInMap("crate animalb02", 0, &map))
									{
										CallbackWhenSampleFinished((randomAmbients[1]=PlayTrackingSample("eyesblink1.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(2.5f), METERTOUNIT(5.0f))),CharacterSoundsFinishedCallback,&randomAmbients[1]);
									}
								}
								else
								{
									if (pActor = FindActorInstanceInMap("crate animal201", 0, &map))
									{
										CallbackWhenSampleFinished((randomAmbients[1]=PlayTrackingSample("eyesblink2.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(2.5f), METERTOUNIT(5.0f))),CharacterSoundsFinishedCallback,&randomAmbients[1]);
									}
								}

								randomTimers[1] = gameStatus.gameTime + UtilGenerateRandomNumber(0.5f, 1.0f);
							}
						}

						break;

	case SCENE_ICEDOME:	if (randomAmbients[0] == -1)	// JW: Jingle Bells in Santas Grotto
						{
							if (randomTimers[0] < gameStatus.gameTime)
							{
								if (pActor = FindActorInstanceInMap("slippy_roof_inst", 0, &map))
								{
									CallbackWhenSampleFinished((randomAmbients[0]=PlayTrackingSample("jinglebells.wav", 200, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(20.0f))),CharacterSoundsFinishedCallback,&randomAmbients[0]);

									randomTimers[0] = gameStatus.gameTime + UtilGenerateRandomNumber(6.0f, 9.0f);
								}
							}
						}
						
						break;

	case SCENE_CITYHUB:	if (randomAmbients[0] == -1)	// JW: Tram 1 shake/bell
						{
							if (randomTimers[0] < gameStatus.gameTime)
							{
								if (pActor = FindActorInstanceInMap("tram(reversed)", 0, &map))
								{
									if (bmRand()%2)
										CallbackWhenSampleFinished((randomAmbients[0]=PlayTrackingSample("tram2.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(20.0f))),CharacterSoundsFinishedCallback,&randomAmbients[0]);
									else
										CallbackWhenSampleFinished((randomAmbients[0]=PlayTrackingSample("trambell.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(20.0f))),CharacterSoundsFinishedCallback,&randomAmbients[0]);

									randomTimers[0] = gameStatus.gameTime + UtilGenerateRandomNumber(5.0f, 8.0f);
								}
							}
						}

						if (randomAmbients[1] == -1)	// JW: Tram 2 shake/bell
						{
							if (randomTimers[1] < gameStatus.gameTime)
							{
								if (pActor = FindActorInstanceInMap("mesh04", 0, &map))
								{
									if (bmRand()%2)
										CallbackWhenSampleFinished((randomAmbients[1]=PlayTrackingSample("tram2.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(20.0f))),CharacterSoundsFinishedCallback,&randomAmbients[1]);
									else
										CallbackWhenSampleFinished((randomAmbients[1]=PlayTrackingSample("trambell.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(20.0f))),CharacterSoundsFinishedCallback,&randomAmbients[1]);

									randomTimers[1] = gameStatus.gameTime + UtilGenerateRandomNumber(5.0f, 8.0f);
								}
							}
						}

						if (randomAmbients[2] == -1)	// JW: Thunder
						{
							if (randomTimers[2] < gameStatus.gameTime)
							{
								CallbackWhenSampleFinished(randomAmbients[2]=PlaySample("thunder.wav", 255),CharacterSoundsFinishedCallback,&randomAmbients[2]);

								randomTimers[2] = gameStatus.gameTime + UtilGenerateRandomNumber(12.0f, 16.0f);
							}
						}

						break;

	case SCENE_DEPTSTRVEHICLERACE:
	case SCENE_DEPTSTR:	
						if (randomAmbients[0] == -1)	// JW: Toy Plane Sound
						{
							if (pActor = FindActorInstanceInMap("plane ride", 0, &map))
							{
								if (CheckIfPlayingAnimation(pActor, "fly", bkCRC32((const uchar *) "fly", 3, 0)))
								{	// JW: we're not playing a sound and the plane is flying
									randomAmbients[0] = PlayTrackingSample("loop\\kidsride.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(2.5f), METERTOUNIT(10.0f));
								}
							}
						}
						else
						{
							if (pActor = FindActorInstanceInMap("plane ride", 0, &map))
							{
								if (!CheckIfPlayingAnimation(pActor, "fly", bkCRC32((const uchar *) "fly", 3, 0)))
								{	// JW: we're not playing a sound and the plane is flying
									StopChannel(&randomAmbients[0]);
								}
							}
						}

						if (randomAmbients[1] == -1)	// JW: fountain
						{
							if (pActor = FindActorInstanceInMap("fountain water jet", 0, &map))
							{
								randomAmbients[1] = PlayTrackingSample("fountain.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(2.5f), METERTOUNIT(10.0f));

								if (randomAmbients[1] == -1) 
									randomAmbients[1] = PlayTrackingSample("loop\\fountain.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(2.5f), METERTOUNIT(10.0f));
							}
						}

						break;

	case SCENE_CONSTRUCT:	if (randomAmbients[0] == -1)
							{
								if (randomTimers[0] < gameStatus.gameTime)
								{
									randomAmbients[0] = PlaySample("loop\\jackhammer.wav", (int) UtilGenerateRandomNumber(155.0f, 255.0f));

									randomTimers[0] = gameStatus.gameTime + UtilGenerateRandomNumber(2.0f, 4.0f);
								}
							}
							else
							{
								if (randomTimers[0] < gameStatus.gameTime)
								{
									StopChannel(&randomAmbients[0]);

									randomTimers[0] = gameStatus.gameTime + UtilGenerateRandomNumber(12.0f, 15.0f);
								}
							}
							
							if (randomAmbients[1] == -1)
							{
								if (randomTimers[1] < gameStatus.gameTime)
								{
									CallbackWhenSampleFinished(randomAmbients[1]=PlaySample("drilling.wav", (int) UtilGenerateRandomNumber(155.0f, 255.0f)),CharacterSoundsFinishedCallback,&randomAmbients[1]);

									randomTimers[1] = gameStatus.gameTime + UtilGenerateRandomNumber(12.0f, 15.0f);
								}
							}
							
							if (randomAmbients[2] == -1)
							{
								if (randomTimers[2] < gameStatus.gameTime)
								{
									randomAmbients[2] = PlaySample("loop\\sawing.wav", (int) UtilGenerateRandomNumber(155.0f, 255.0f));

									randomTimers[2] = gameStatus.gameTime + UtilGenerateRandomNumber(2.0f, 4.0f);
								}
							}
							else
							{
								if (randomTimers[2] < gameStatus.gameTime)
								{
									StopChannel(&randomAmbients[2]);

									randomTimers[2] = gameStatus.gameTime + UtilGenerateRandomNumber(12.0f, 15.0f);
								}
							}
		
							break;

	case SCENE_WILDWESTHUB:	if (randomAmbients[0] == -1)	// JW: Ghost town howl
							{
								if (randomTimers[0] < gameStatus.gameTime)
								{
									if (pActor = FindActorInstanceInMap("stats_ghost", 0, &map))
									{
										CallbackWhenSampleFinished((randomAmbients[0]=PlayTrackingSample("howl.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(20.0f))),CharacterSoundsFinishedCallback,&randomAmbients[0]);

										randomTimers[0] = gameStatus.gameTime + UtilGenerateRandomNumber(8.0f, 12.0f);
									}
								}
							}

							if (randomAmbients[1] == -1)	// JW: Ghost town church bell
							{
								if (randomTimers[1] < gameStatus.gameTime)
								{
									if (pActor = FindActorInstanceInMap("stats_ghost", 0, &map))
									{
										CallbackWhenSampleFinished((randomAmbients[1]=PlayTrackingSample("churchbell.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(20.0f))),CharacterSoundsFinishedCallback,&randomAmbients[1]);

										randomTimers[1] = gameStatus.gameTime + UtilGenerateRandomNumber(6.0f, 10.0f);
									}
								}
							}

							if (randomAmbients[2] == -1)	// JW: Train toot
							{
								if (randomTimers[2] < gameStatus.gameTime)
								{
									if (pActor = FindActorInstanceInMap("train_root_object01", 0, &map))
									{
										CallbackWhenSampleFinished((randomAmbients[2]=PlayTrackingSample("trainhornwav.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(20.0f))),CharacterSoundsFinishedCallback,&randomAmbients[2]);

										randomTimers[2] = gameStatus.gameTime + UtilGenerateRandomNumber(10.0f, 15.0f);
									}
								}
							}

							if (randomAmbients[3] == -1)	// JW: random crow creak
							{
								if (randomTimers[3] < gameStatus.gameTime)
								{
									if (pActor = FindActorInstanceInMap("buzzard_bounding_box01", 0, &map))
									{
										CallbackWhenSampleFinished((randomAmbients[3]=PlayTrackingSample("birdsquark.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(10.0f), METERTOUNIT(40.0f))),CharacterSoundsFinishedCallback,&randomAmbients[3]);

										randomTimers[3] = gameStatus.gameTime + UtilGenerateRandomNumber(10.0f, 15.0f);
									}
								}
							}
		
							break;

	case SCENE_GRANDCANYON:	if (randomAmbients[0] == -1)	// JW: random crow creak
							{
								if (randomTimers[0] < gameStatus.gameTime)
								{
									if (pActor = FindActorInstanceInMap("buzzard_bounding_box01", 0, &map))
									{
										CallbackWhenSampleFinished((randomAmbients[0]=PlayTrackingSample("birdsquark.wav", 255, pActor->actorInstance.position, 0, 0, METERTOUNIT(10.0f), METERTOUNIT(40.0f))),CharacterSoundsFinishedCallback,&randomAmbients[0]);

										randomTimers[0] = gameStatus.gameTime + UtilGenerateRandomNumber(10.0f, 15.0f);
									}
								}
							}
		
							break;
						
	}
}

void PlayOpeningSoundCallback(FADE *fade)
{
	ACTORINSTANCE		*pActor;

	switch (map.sceneNumber)
	{
	case SCENE_TAZBOSS:		PlaySample("tweety_rewardismine.wav", 255);	

							break;

	case SCENE_MUSEUMVEHICLERACE:	if (pActor = FindActorInstanceInInstanceList("taz", 0, &characterInstances))
									{
										PlaySample("obj_mus_03a.wav", 255);
										pActor->actorStatus->vehicleInfo->soundHandle = PlaySample("obj_mus_03b.wav", 160, 0, LOWEST_FREQUENCY);
									}
		
									if (pActor = FindActorInstanceInInstanceList("shedevil", 0, &characterInstances))
									{
										PlaySample("obj_mus_03a.wav", 255);
										pActor->actorStatus->vehicleInfo->soundHandle = PlaySample("obj_mus_03b.wav", 160, 0, LOWEST_FREQUENCY);
									}		

									break;

	case SCENE_GRANDCVEHICLERACE:	if (pActor = FindActorInstanceInInstanceList("taz", 0, &characterInstances))
									{
										pActor->actorStatus->vehicleInfo->soundHandle = PlaySample("jetbikeloop2.wav", 160, 0, LOWEST_FREQUENCY);
									}

									if (pActor = FindActorInstanceInInstanceList("shedevil", 0, &characterInstances))
									{
										pActor->actorStatus->vehicleInfo->soundHandle = PlaySample("jetbikeloop2.wav", 160, 0, LOWEST_FREQUENCY);
									}

									break;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: LoadAllResidentSamplesBackground
	Purpose 	: The clue is in the name
	Parameters 	: 
	Returns 	: 
	Info 		: GameCube only, because it has a bucket load of ARAM
				  Only free them at the end
*/
#if BPLATFORM==GAMECUBE
void LoadAllResidentSamplesBackground(void)
{
	char	tempChar[256];

	// IH: This no longer works as it uses too much ram
#ifdef USE_REVERB
	InitEffects();
#endif

	// TP: Load the generic samples
	LoadWholePackageBkg(AppendCountryCode(tempChar,"ressounds"));
}
#endif


/*	--------------------------------------------------------------------------------
	Function 	: MoveAllResidentSamplesToARAM
	Purpose 	: The clue is in the name
	Parameters 	: 
	Returns 	: 
	Info 		: GameCube only, because it has a bucket load of ARAM
				  Only free them at the end
*/
#if BPLATFORM==GAMECUBE
void MoveAllResidentSamplesToARAM(void)
{
	TBPackageIndex	*pak;
	char			samplepakname[256];
	// IH: Resident sounds

	// TP: Load samples into sound ram
	if (pak = FindPackage(AppendCountryCode(samplepakname,"ressounds")))
	{
		bkLoadPackageSamples(pak);
	}

	// IH: Can only free a pak that exists
	if( pak )
	{
		// IH: This is a bit of a bodge: Loop until the DMA has finished doing its magic
		while(!bkFreePackageMemory(&pak))
		{
			bkUpdate();
		}
		UpdateIndex(samplepakname, pak);
	}
}
#endif


/*	--------------------------------------------------------------------------------
	Function 	: FreeAllResidentSamplesFromARAM
	Purpose 	: The clue is in the name
	Parameters 	: 
	Returns 	: 
	Info 		: GameCube only, because it has a bucket load of ARAM
*/
#if BPLATFORM==GAMECUBE
void FreeAllResidentSamplesFromARAM(void)
{
	char			samplepakname[256];

	// IH: This no longer works as it uses too much ram
#ifdef USE_REVERB
	ShutdownEffects();
#endif

	// IH: Resident sounds
	FreeWholePackage(AppendCountryCode(samplepakname,"ressounds"));
}
#endif



/*	--------------------------------------------------------------------------------
	Function 	: LoadAllLevelSamplesBackground
	Purpose 	: Load all of the samples for a scene
	Parameters 	: 
	Returns 	: 
	Info 		: GameCube only, because it has a bucket load of ARAM
*/
#if BPLATFORM==GAMECUBE
void LoadAllLevelSamplesBackground(void)
{
	char	tempChar[256];

	switch (map.sceneNumber)
	{
		case SCENE_FRONTEND:	LoadWholePackageBkg("frontendsound");	break;
		case SCENE_LANGUAGESELECT:	LoadWholePackageBkg("languagesound");	break;
		
		case SCENE_ZOOBOSS:		LoadWholePackageBkg(AppendCountryCode(tempChar,"zoobosssound"));	break;
		case SCENE_AQUA:		LoadWholePackageBkg(AppendCountryCode(tempChar,"aquaintrosound"));
								LoadWholePackageBkg("aquasound");		break;
		case SCENE_SAFARI:		LoadWholePackageBkg(AppendCountryCode(tempChar,"safariintrosound"));
								LoadWholePackageBkg("safarisound");		break;
		case SCENE_ICEDOME:		LoadWholePackageBkg(AppendCountryCode(tempChar,"icedomeintrosound"));
								LoadWholePackageBkg("icedomesound");	break;
		case SCENE_ZOOHUB:		LoadWholePackageBkg(AppendCountryCode(tempChar,"zoohubsound"));		break;
		
		case SCENE_CITYHUB:		LoadWholePackageBkg("cityhubsound");	break;
		case SCENE_DEPTSTR:		LoadWholePackageBkg(AppendCountryCode(tempChar,"deptstrintrosound"));	
								LoadWholePackageBkg("deptstrsound");	break;
		case SCENE_MUSEUM:		LoadWholePackageBkg(AppendCountryCode(tempChar,"museumintrosound"));
								LoadWholePackageBkg("museumsound");		break;
		case SCENE_CONSTRUCT:	LoadWholePackageBkg(AppendCountryCode(tempChar,"constructintrosound"));
								LoadWholePackageBkg("constructsound");	break;
		case SCENE_CITYBOSS:	LoadWholePackageBkg(AppendCountryCode(tempChar,"citybosssound"));	break;

		case SCENE_WILDWESTHUB:	LoadWholePackageBkg("westhubsound");	break;
		case SCENE_GOLDMINE:	LoadWholePackageBkg(AppendCountryCode(tempChar,"goldmineintrosound"));
								LoadWholePackageBkg("goldminesound");	break;
		case SCENE_GHOSTTOWN:	LoadWholePackageBkg(AppendCountryCode(tempChar,"ghosttownintrosound"));
								LoadWholePackageBkg("ghosttownsound");	break;
		case SCENE_GRANDCANYON:	LoadWholePackageBkg(AppendCountryCode(tempChar,"canyonintrosound"));
								LoadWholePackageBkg("canyonsound");		break;
		case SCENE_WESTBOSS:	LoadWholePackageBkg(AppendCountryCode(tempChar,"westbosssound"));	break;

		case SCENE_TAZHUB:		LoadWholePackageBkg(AppendCountryCode(tempChar,"tasmaniaintrosound"));
								LoadWholePackageBkg("tasmaniasound");	break;

		case SCENE_MUSEUMVEHICLERACE:		LoadWholePackageBkg(AppendCountryCode(tempChar,"museumvrsound"));		break;
		case SCENE_GRANDCVEHICLERACE:		LoadWholePackageBkg(AppendCountryCode(tempChar,"grandcvrsound"));		break;
		case SCENE_DEPTSTRVEHICLERACE:		LoadWholePackageBkg(AppendCountryCode(tempChar,"deptstrvrsound"));		break;

		case SCENE_DESTRUCTIONAQUA:			LoadWholePackageBkg(AppendCountryCode(tempChar,"aquadesound"));			break;
		case SCENE_DESTRUCTIONCONSTRUCT:	LoadWholePackageBkg(AppendCountryCode(tempChar,"constructdesound"));	break;
		case SCENE_DESTRUCTIONTASMANIA:		LoadWholePackageBkg(AppendCountryCode(tempChar,"tasmaniadesound"));		break;

		case SCENE_ICEDOMERACE:				LoadWholePackageBkg(AppendCountryCode(tempChar,"icedometasound"));		break;
		case SCENE_GOLDMINERACE:			LoadWholePackageBkg(AppendCountryCode(tempChar,"goldminetasound"));		break;
		case SCENE_SAFARIRACE:				LoadWholePackageBkg(AppendCountryCode(tempChar,"safaritasound"));		break;
	}
}
#endif


/*	--------------------------------------------------------------------------------
	Function 	: MoveAllLevelSamplesToARAM
	Purpose 	: The clue is in the name
	Parameters 	: 
	Returns 	: 
	Info 		: GameCube only, because it has a bucket load of ARAM
				  Should only be called when all of samples the have been loaded into main ram
*/
#if BPLATFORM==GAMECUBE
void MoveAllLevelSamplesToARAM(void)
{
	TBPackageIndex	*pak;
	char			samplepakname[256];
	
	// TP: List for localised sound packages
	switch(map.sceneNumber)
	{
		case SCENE_CITYBOSS:	strcpy(samplepakname, "citybosssound");		break;
		case SCENE_ZOOBOSS:		strcpy(samplepakname, "zoobosssound");		break;
		case SCENE_WESTBOSS:	strcpy(samplepakname, "westbosssound");		break;
		case SCENE_ZOOHUB:		strcpy(samplepakname, "zoohubsound");		break;

		case SCENE_MUSEUMVEHICLERACE:		strcpy(samplepakname, "museumvrsound");		break;
		case SCENE_GRANDCVEHICLERACE:		strcpy(samplepakname, "grandcvrsound");		break;
		case SCENE_DEPTSTRVEHICLERACE:		strcpy(samplepakname, "deptstrvrsound");	break;

		case SCENE_DESTRUCTIONAQUA:			strcpy(samplepakname, "aquadesound");		break;
		case SCENE_DESTRUCTIONCONSTRUCT:	strcpy(samplepakname, "constructdesound");	break;
		case SCENE_DESTRUCTIONTASMANIA:		strcpy(samplepakname, "tasmaniadesound");	break;

		case SCENE_ICEDOMERACE:				strcpy(samplepakname, "icedometasound");	break;
		case SCENE_GOLDMINERACE:			strcpy(samplepakname, "goldminetasound");	break;
		case SCENE_SAFARIRACE:				strcpy(samplepakname, "safaritasound");		break;
	}

	// IH: Note to self: The list of cases above is actually different to the list below
	if (samplepakname[0]!=0)
	{
		AppendCountryCode(samplepakname, samplepakname);
	}

	// TP: List for generic sound packages (NOT LOCALISED)
	switch (map.sceneNumber)
	{
		case SCENE_FRONTEND:	strcpy(samplepakname, "frontendsound");		break;
		case SCENE_LANGUAGESELECT:strcpy(samplepakname, "languagesound");	break;
		
		case SCENE_AQUA:		strcpy(samplepakname, "aquasound");			break;
		case SCENE_SAFARI:		strcpy(samplepakname, "safarisound");		break;
		case SCENE_ICEDOME:		strcpy(samplepakname, "icedomesound");		break;
		
		case SCENE_CITYHUB:		strcpy(samplepakname, "cityhubsound");		break;
		case SCENE_DEPTSTR:		strcpy(samplepakname, "deptstrsound");		break;
		case SCENE_MUSEUM:		strcpy(samplepakname, "museumsound");		break;
		case SCENE_CONSTRUCT:	strcpy(samplepakname, "constructsound");	break;

		case SCENE_WILDWESTHUB:	strcpy(samplepakname, "westhubsound");		break;
		case SCENE_GOLDMINE:	strcpy(samplepakname, "goldminesound");		break;
		case SCENE_GHOSTTOWN:	strcpy(samplepakname, "ghosttownsound");	break;
		case SCENE_GRANDCANYON:	strcpy(samplepakname, "canyonsound");		break;

		case SCENE_TAZHUB:		strcpy(samplepakname, "tasmaniasound");		break;
	}

	if (pak = FindPackage(samplepakname))
	{
		bkLoadPackageSamples(pak);
	}

	// IH: Can only free a pak that exists
	if( pak )
	{
		// IH: This is a bit of a bodge: Loop until the DMA has finished doing its magic
		while(!bkFreePackageMemory(&pak))
		{
			bkUpdate();
		}
		UpdateIndex(samplepakname, pak);
	}


	// IH: Move intro samples to ARAM
	samplepakname[0] = 0;

	switch (map.sceneNumber)
	{
		case SCENE_AQUA:		strcpy(samplepakname, "aquaintrosound");		break;
		case SCENE_SAFARI:		strcpy(samplepakname, "safariintrosound");		break;
		case SCENE_ICEDOME:		strcpy(samplepakname, "icedomeintrosound");		break;
		
		case SCENE_DEPTSTR:		strcpy(samplepakname, "deptstrintrosound");		break;
		case SCENE_MUSEUM:		strcpy(samplepakname, "museumintrosound");		break;
		case SCENE_CONSTRUCT:	strcpy(samplepakname, "constructintrosound");	break;

		case SCENE_GOLDMINE:	strcpy(samplepakname, "goldmineintrosound");	break;
		case SCENE_GHOSTTOWN:	strcpy(samplepakname, "ghosttownintrosound");	break;
		case SCENE_GRANDCANYON:	strcpy(samplepakname, "canyonintrosound");		break;

		case SCENE_TAZHUB:		strcpy(samplepakname, "tasmaniaintrosound");	break;
	}

	if (samplepakname[0]!=0)
	{
		AppendCountryCode(samplepakname, samplepakname);
	}

	if (pak = FindPackage(samplepakname))
	{
//		bkLoadPackageResources(pak,BRESMASK_SAMPLE,255,NULL);
		bkLoadPackageSamples(pak);
	}

	// IH: Can only free a pak if it exists
	if( pak )
	{
		// IH: This is a bit of a bodge: Loop until the DMA has finished doing its magic
		while(!bkFreePackageMemory(&pak))
		{
			bkUpdate();
		}
		UpdateIndex(samplepakname, pak);
	}
}
#endif


/*	--------------------------------------------------------------------------------
	Function 	: FreeAllLevelSamplesFromARAM
	Purpose 	: The clue is in the name
	Parameters 	: 
	Returns 	: 
	Info 		: GameCube only, because it has a bucket load of ARAM
*/
#if BPLATFORM==GAMECUBE
void FreeAllLevelSamplesFromARAM(void)
{
	char	samplepakname[256];

	// TP: List for localised sound packages
	switch(map.sceneNumber)
	{
		case SCENE_CITYBOSS:	strcpy(samplepakname, "citybosssound");		break;
		case SCENE_ZOOBOSS:		strcpy(samplepakname, "zoobosssound");		break;
		case SCENE_WESTBOSS:	strcpy(samplepakname, "westbosssound");		break;
		case SCENE_ZOOHUB:		strcpy(samplepakname, "zoohubsound");		break;

		case SCENE_MUSEUMVEHICLERACE:		strcpy(samplepakname, "museumvrsound");		break;
		case SCENE_GRANDCVEHICLERACE:		strcpy(samplepakname, "grandcvrsound");		break;
		case SCENE_DEPTSTRVEHICLERACE:		strcpy(samplepakname, "deptstrvrsound");	break;

		case SCENE_DESTRUCTIONAQUA:			strcpy(samplepakname, "aquadesound");		break;
		case SCENE_DESTRUCTIONCONSTRUCT:	strcpy(samplepakname, "constructdesound");	break;
		case SCENE_DESTRUCTIONTASMANIA:		strcpy(samplepakname, "tasmaniadesound");	break;

		case SCENE_ICEDOMERACE:				strcpy(samplepakname, "icedometasound");	break;
		case SCENE_GOLDMINERACE:			strcpy(samplepakname, "goldminetasound");	break;
		case SCENE_SAFARIRACE:				strcpy(samplepakname, "safaritasound");		break;

	}

	if (samplepakname[0]!=0)
	{
		AppendCountryCode(samplepakname, samplepakname);
	}

	// TP: List for generic sound packages (NOT LOCALISED)
	switch (map.sceneNumber)
	{
		case SCENE_FRONTEND:	strcpy(samplepakname, "frontendsound");		break;
		case SCENE_LANGUAGESELECT:strcpy(samplepakname, "languagesound");	break;
		
		case SCENE_AQUA:		strcpy(samplepakname, "aquasound");			break;
		case SCENE_SAFARI:		strcpy(samplepakname, "safarisound");		break;
		case SCENE_ICEDOME:		strcpy(samplepakname, "icedomesound");		break;
		
		case SCENE_CITYHUB:		strcpy(samplepakname, "cityhubsound");		break;
		case SCENE_DEPTSTR:		strcpy(samplepakname, "deptstrsound");		break;
		case SCENE_MUSEUM:		strcpy(samplepakname, "museumsound");		break;
		case SCENE_CONSTRUCT:	strcpy(samplepakname, "constructsound");	break;

		case SCENE_WILDWESTHUB:	strcpy(samplepakname, "westhubsound");		break;
		case SCENE_GOLDMINE:	strcpy(samplepakname, "goldminesound");		break;
		case SCENE_GHOSTTOWN:	strcpy(samplepakname, "ghosttownsound");	break;
		case SCENE_GRANDCANYON:	strcpy(samplepakname, "canyonsound");		break;

		case SCENE_TAZHUB:		strcpy(samplepakname, "tasmaniasound");		break;

	}

	FreeWholePackage(samplepakname);

	// IH: Now free the intro samples
	samplepakname[0] = 0;

	switch (map.sceneNumber)
	{
		case SCENE_AQUA:		strcpy(samplepakname, "aquaintrosound");		break;
		case SCENE_SAFARI:		strcpy(samplepakname, "safariintrosound");		break;
		case SCENE_ICEDOME:		strcpy(samplepakname, "icedomeintrosound");		break;
		
		case SCENE_DEPTSTR:		strcpy(samplepakname, "deptstrintrosound");		break;
		case SCENE_MUSEUM:		strcpy(samplepakname, "museumintrosound");		break;
		case SCENE_CONSTRUCT:	strcpy(samplepakname, "constructintrosound");	break;

		case SCENE_GOLDMINE:	strcpy(samplepakname, "goldmineintrosound");	break;
		case SCENE_GHOSTTOWN:	strcpy(samplepakname, "ghosttownintrosound");	break;
		case SCENE_GRANDCANYON:	strcpy(samplepakname, "canyonintrosound");		break;

		case SCENE_TAZHUB:		strcpy(samplepakname, "tasmaniaintrosound");	break;
	}

	if (samplepakname[0]!=0)
	{
		AppendCountryCode(samplepakname, samplepakname);
	}

	FreeWholePackage(samplepakname);
}
#endif

/*	--------------------------------------------------------------------------------
	Function 	: SetSoundSpeed
	Purpose 	: Sets the sound speed to a value passed in.
	Parameters 	: 
	Returns 	: 
	Info 		: 1.0f = standard speed.
*/

void SetSoundSpeed(float fValue)
{
	static float lastFValue = 0.0f;

	int frequency;

	if (fValue != lastFValue)
	{
		if (fValue == 1.0f)
			frequency = 0;
		else
		{
			// NH: Calculate frequency
			frequency = (int) ((STANDARD_FREQUENCY - LOWEST_FREQUENCY) * fValue) + LOWEST_FREQUENCY;
		}

		for(int channel=0;channel<maxSoundChannels;channel++)
		{
			if (bsGetChannelStatus(channel) == BSCHSTATUS_PLAYING)
				bsSetChannelFrequency(channel, frequency);
		}

		lastFValue = fValue;
	}
}


/*  --------------------------------------------------------------------------------
	Function	: SetSoundPos
	Purpose		: Set the position of sounds for any following messages
	Parameters	: position
	Returns		: 
	Info		: 
*/
void SetSoundPos(TBVector position)
{
	char	message[256];

	sprintf(message, "_POS=%d %d %d", bmFloatToInt(position[X]), bmFloatToInt(position[Y]), bmFloatToInt(position[Z]));
	bkGenerateEvent("sound",message);
}


/*  --------------------------------------------------------------------------------
	Function	: StopAllChannels
	Purpose		: Stop all channels and clear any callbacks etc.
	Parameters	: 
	Returns		: 
	Info		: 
*/

void StopAllChannels(void)
{
	bsStopAllChannels();
	ResetSoundCallbacks();
}


/*  --------------------------------------------------------------------------------
	Function	: LoopingSampleActorInstanceCallback
	Purpose		: called when a sample applied to an actorinstance finishes, this function would be called if the sample
					played in RespondToSoundMessages was not a proper looping sample.
	Parameters	: ptr to actorinstance
	Returns		: 
	Info		: 
*/

void LoopingSampleActorInstanceCallback(void *voidPtr)
{
	ACTORINSTANCE *actorInstance = (ACTORINSTANCE*)voidPtr;

	if (IsActorInstanceValid(actorInstance))
	{
		actorInstance->soundHandle = -1;
	}
}


enum ESoundFlag
{
	SOUNDFLAG_PLAY = 1<<0,
	SOUNDFLAG_SETPOS = 1<<1,
	SOUNDFLAG_LOOP = 1<<2,
	SOUNDFLAG_NOPOS = 1<<3,
	SOUNDFLAG_TRACK = 1<<4,
};


/*  --------------------------------------------------------------------------------
	Function	: GetSoundFlagsFromString
	Purpose		: Scans the string and sets the appropriate flags
	Parameters	: string
	Returns		: int containing flags
	Info		: 
*/

int GetSoundFlagsFromString(char *string)
{
	int	flags = 0;

	if (strstr(string, "play"))
	{
		flags |= SOUNDFLAG_PLAY;
	}
	if (strstr(string, "loop"))
	{
		flags |= SOUNDFLAG_LOOP;
	}
	if (strstr(string, "nopos"))
	{
		flags |= SOUNDFLAG_NOPOS;
	}
	if (strstr(string, "track"))
	{
		flags |= SOUNDFLAG_TRACK;
	}
	if (strcmpi(string, "pos")==0)
	{
		flags |= SOUNDFLAG_SETPOS;
	}

	return flags;
}


/*  --------------------------------------------------------------------------------
	Function	: TrackingSampleCallback
	Purpose		: Used to update a tracking sample that uses a node for position
	Parameters	: position to update, time since last update, context
	Returns		: 
	Info		: 
*/

void TrackingSampleCallback(TBVector pos, int timeDelta, void *context)
{
	ACTORINSTANCE *actorInstance = (ACTORINSTANCE*)context;

	if ((!actorInstance)||(!context)) return;

	baGetNodesWorldPosition(&actorInstance->actorInstance, actorInstance->soundNodeInstance, pos);
}


/*	--------------------------------------------------------------------------------
	Function 	: RespondToSoundMessages
	Purpose 	: Respond to sound messages generated
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void RespondToSoundMessages(void)
{
	char				parmStringBuf[256];
	char				*sampleName;
	char				*messageType,*instanceName,*nodeName;
	TBVector			currentSoundPosition,positionVector;
	char				soundPositionSet=FALSE;
	TBActorInstance		*messageInstance=NULL;
	TBActorNodeInstance	*messageNodeInstance=NULL;
	bool				soundFromNoteTrack;
	TBActorNodeInstance *actorNodeInstance;
	ACTORINSTANCE		*playOnInstance;
	int					playFlags;

	while (bkPopEvent(soundEventClient, parmStringBuf) == OK)
	{
		messageInstance = NULL;
		messageNodeInstance = NULL;
		playOnInstance = NULL;
		soundFromNoteTrack = false;
		nodeName = NULL;

		messageType = DecodeMessage(parmStringBuf, &messageInstance, &messageNodeInstance);

		// TP: Check that the instance that sent the message still exists
		if (messageInstance)
		{
			if (IsActorInstanceValid((ACTORINSTANCE*)messageInstance))
			{
				if ((void*)messageNodeInstance != (void*)messageInstance)
				{
					if (messageNodeInstance)
					{
						messageNodeInstance = baFindNodeByCRC(messageInstance->rootNodeInstance, messageNodeInstance->actorNode->crc);
					}
				}

				if (!messageNodeInstance)
				{
					// TP: not valid actor
					continue;
				}
			}
			else
			{
				// TP: not valid actor
				continue;
			}
		}

		sampleName = FindChar(messageType,'=');
		if (sampleName==NULL) 
		{
			if (strcmpi(messageType, "STOP")==0)
			{
				playOnInstance = (ACTORINSTANCE*)messageInstance;
				if (playOnInstance) StopChannel(&playOnInstance->soundHandle);
			}
			continue;
		}
		*sampleName++ = 0;

		if (strcmpi(messageType, "STOP")==0)
		{
			playOnInstance = FindActorInstanceInMap(sampleName,0,&map);
			if (playOnInstance) StopChannel(&playOnInstance->soundHandle);
			continue;
		}

		playFlags = GetSoundFlagsFromString(messageType);

		if ((PLAYING_INTRO_CUTSCENE)||(PLAYING_OUTRO_CUTSCENE))
		{
			// TP: type "PLAY" sound be PLAYNOPOS in cutscenes, this is done to save a lot of work changing the cutscenes
			playFlags |= SOUNDFLAG_NOPOS;
		}

		if (playFlags & SOUNDFLAG_SETPOS)
		{
			// set current position
			soundPositionSet = TRUE;
			sscanf(sampleName,"%f %f %f",&currentSoundPosition[X],&currentSoundPosition[Y],&currentSoundPosition[Z]);
			continue;
		}

		// TP: find actorinstance to assign the sound to
		instanceName = FindChar(sampleName,':');
		if (instanceName)
		{
			*instanceName = 0;
			instanceName++;
			nodeName = FindChar(instanceName,':');
			if (nodeName)
			{
				*nodeName = 0;
				nodeName++;
			}
			playOnInstance = FindActorInstanceInMap(instanceName,0,&map);
			if (!playOnInstance) bkPrintf("Could not find actorInstance %s to play sound on\n", instanceName);
		}
		else
		{
			if (messageInstance) playOnInstance = (ACTORINSTANCE*)messageInstance;
		}

		// TP: positionVector should contain the position the sound is to be played at
		if (playOnInstance)
		{
			if (messageNodeInstance)
			{
				baGetNodesWorldPosition(messageInstance, messageNodeInstance, positionVector);
			}
			else
			{
				bmVectorCopy(positionVector, playOnInstance->actorInstance.position);
			}
		}
		else 
		{
			bmVectorCopy(positionVector, currentSoundPosition);
		}

		if (playFlags & (SOUNDFLAG_LOOP|SOUNDFLAG_TRACK)) 
		{
			if ((!playOnInstance)||(playOnInstance->soundHandle!=-1))
			{
				// TP: if there is no playOnInstance or the playOnInstance is already playing a sound
				continue;
			}
		}
			
		// TP: remap sound if needs be
		if ((strcmp(sampleName, "crash_gen_02a.wav")==0)||
			(strcmp(sampleName, "crash_gen_02b.wav")==0)||
			(strcmp(sampleName, "crash_gen_02c.wav")==0)||
			(strcmp(sampleName, "crash_gen_03c.wav")==0)||
			(strcmp(sampleName, "obj_mus_05a.wav")==0))
		{
			sampleName = "crashwood 1-1.wav";
		}

		positionVector[W] = 1.0f;

		if (playFlags & SOUNDFLAG_PLAY)
		{
			if (playFlags & (SOUNDFLAG_TRACK|SOUNDFLAG_LOOP))
			{
				StopChannel(&playOnInstance->soundHandle);
				if (playFlags & SOUNDFLAG_NOPOS)
				{
					CallbackWhenSampleFinished(playOnInstance->soundHandle = PlaySample(sampleName, 255), LoopingSampleActorInstanceCallback, (void*)playOnInstance);
				}
				else
				{
					if (nodeName)
					{
						
						if (playOnInstance->soundNodeInstance = baFindNode(playOnInstance->actorInstance.rootNodeInstance, nodeName))
						{
							CallbackWhenSampleFinished(playOnInstance->soundHandle = PlayTrackingSample(sampleName, 255, playOnInstance->actorInstance.position, 0, 0, SFX_minDist, SFX_maxDist, TrackingSampleCallback, (void*)playOnInstance), LoopingSampleActorInstanceCallback, (void*)playOnInstance);
						}
						else
						{
							CallbackWhenSampleFinished(playOnInstance->soundHandle = PlayTrackingSample(sampleName, 255, playOnInstance->actorInstance.position), LoopingSampleActorInstanceCallback, (void*)playOnInstance);
						}
					}
					else
					{
						CallbackWhenSampleFinished(playOnInstance->soundHandle = PlayTrackingSample(sampleName, 255, playOnInstance->actorInstance.position), LoopingSampleActorInstanceCallback, (void*)playOnInstance);
					}
				}
			}
			else
			{
				if (playFlags & SOUNDFLAG_NOPOS)
				{
					PlaySample(sampleName, 255);
				}
				else
				{
					if ((soundPositionSet)||(playOnInstance))
					{
						PlaySample(sampleName, 255, positionVector);
					}
					else
					{
						PlaySample(sampleName, 255);
					}
				}
			}
		}
	}  
}


/*  --------------------------------------------------------------------------------
	Function	: EnableReverb
	Purpose		: Sets up the effects
	Parameters	: 
	Returns		: 
	Info		: GameCube Exclusive
*/
#if BPLATFORM==GAMECUBE
void InitEffects(void)
{
    reverbSmallRoom.tempDisableFX	= FALSE;
    reverbSmallRoom.time			= 3.0f;
    reverbSmallRoom.preDelay		= 0.1f;
    reverbSmallRoom.damping			= 0.5f;
    reverbSmallRoom.coloration		= 0.5f;
    reverbSmallRoom.mix				= 0.5f;

    reverbBigRoom.tempDisableFX		= FALSE;
    reverbBigRoom.time				= 3.0f;
    reverbBigRoom.preDelay			= 0.2f;
    reverbBigRoom.damping			= 0.3f;
    reverbBigRoom.coloration		= 0.5f;
    reverbBigRoom.mix				= 0.5f;

	delayBigCavern.delay[0]			= 900;
    delayBigCavern.delay[1]			= 900;
    delayBigCavern.delay[2]			= 900;
    delayBigCavern.feedback[0]		= 50;
    delayBigCavern.feedback[1]		= 50;
    delayBigCavern.feedback[2]		= 50;
    delayBigCavern.output[0]		= 100;
    delayBigCavern.output[1]		= 100;
    delayBigCavern.output[2]		= 100;
	
	// IH: initialize reverb
	AXFXReverbStdInit(&reverbSmallRoom);
	AXFXReverbStdInit(&reverbBigRoom);
	// IH: initialize delay
	AXFXDelayInit(&delayBigCavern);
}
#endif


/*  --------------------------------------------------------------------------------
	Function	: DisableReverb
	Purpose		: Shuts down the effects, yes, all of them
	Parameters	: 
	Returns		: 
	Info		: GameCube Exclusive
*/
#if BPLATFORM==GAMECUBE
void ShutdownEffects(void)
{
	// IH: Shut that reverb down
	AXFXReverbStdShutdown(&reverbSmallRoom);
	AXFXReverbStdShutdown(&reverbBigRoom);
	// IH: Shut the delay down
	AXFXDelayShutdown(&delayBigCavern);
}
#endif



/*  --------------------------------------------------------------------------------
	Function	: FadeInEffect
	Purpose		: Fades in effect
	Parameters	: Type of effect
	Returns		: 
	Info		: GameCube Exclusive
*/
#if BPLATFORM==GAMECUBE
void FadeInEffect(int effectType)
{
	// IH: TEMPORARY: TODO: FADE THE EFFECT IN

	// IH: TODO: Allow an effect to be faded in if there is already one playing
	
	// IH: Can't fade an effect in if there is already one playing
	if(soundEffect)
		return;

	switch(effectType)
	{
	case( ESFX_NO_EFFECTS ):
		// IH: Turn off all effects
		AXRegisterAuxACallback(NULL, NULL);
		break;
	case( ESFX_SMALL_ROOM_REVERB ):
		// IH: Turn on the small room reverb
		AXRegisterAuxACallback(&AXFXReverbStdCallback, &reverbSmallRoom);
		break;
	case( ESFX_LARGE_ROOM_REVERB ):
		// IH: Turn on the big room reverb
		AXRegisterAuxACallback(&AXFXReverbStdCallback, &reverbBigRoom);
		break;
	case( ESFX_BIG_CAVERN_ECHO ):
		// IH: Turn on the big cavern echo
		AXRegisterAuxACallback(&AXFXDelayCallback, &delayBigCavern);
		break;
	default:
		bkPrintf( "Error: default case reached in SetupSoundEffect()\n" );
		break;
	}

	// IH: TEMPORARY: Store the effect type (why?) 
	soundEffect = effectType;

	// IH: Set the fading type to be fading in
	auxAFadeType = ESFX_FADING_IN;

}
#endif


/*  --------------------------------------------------------------------------------
	Function	: FadeOutEffect
	Purpose		: Fades out effect
	Parameters	: 
	Returns		: 
	Info		: GameCube Exclusive
*/
#if BPLATFORM==GAMECUBE
void FadeOutEffect(void)
{
	// IH: Just fade out
	auxAFadeType = ESFX_FADING_OUT;	
}
#endif


/*  --------------------------------------------------------------------------------
	Function	: UpdateEffect
	Purpose		: Updates the effects
	Parameters	: 
	Returns		: 
	Info		: GameCube Exclusive
*/
#if BPLATFORM==GAMECUBE
void UpdateEffects(void)
{
	switch(auxAFadeType)
	{
	case(ESFX_FADING_IN):

		// IH: Effect is alread switched on, just increase the volume of Aux A
		auxAVolume += (AUXA_MAX_VOLUME / AUXA_FADE_SPEED) / fps;
		// IH: Put an upper limit on it
		if( auxAVolume > 0.0f )
		{
			auxAVolume = 0.0f;
			auxAFadeType = ESFX_NOT_FADING;
			soundEffect = ESFX_NO_EFFECTS;
		}
		break;

	case(ESFX_FADING_OUT):

		// IH: Decrease and check for the bottom value
		auxAVolume -= (AUXA_MAX_VOLUME / AUXA_FADE_SPEED) / fps;
		if( auxAVolume < -AUXA_MAX_VOLUME )
		{
			auxAVolume = -AUXA_MAX_VOLUME;
			AXRegisterAuxACallback(NULL, NULL);
			auxAFadeType = ESFX_NOT_FADING;
			soundEffect = ESFX_NO_EFFECTS;
		}
		break;

	case(ESFX_NOT_FADING):
	default:
		// IH: Do nothing
		break;
	}
}
#endif
