#ifndef __H_SFX
#define __H_SFX

#include "fade.h"

// PP: volume at which a sample will play if you don't specify a volume
#define SFX_DEFAULT_VOLUME				255			// PP: maximum volume

// PP: the maximum distance that positional sound can travel and still be heard
extern float SFX_maxDist;

// PP: distance at which sound starts to fade out - THIS CAN'T BE ZERO!
extern float SFX_minDist;

typedef struct TAG_MENU_SOUND
{
	char				name[20];		// Sample name
	int					volume;			// Sample volume
} MENU_SOUND, _MENU_SOUND;

typedef struct TAG_MENU_SOUNDS
{
	struct TAG_MENU_SOUND			navigateSound;
	struct TAG_MENU_SOUND			selectSound;
	struct TAG_MENU_SOUND			backupSound;
	struct TAG_MENU_SOUND			errorSound;
} MENU_SOUNDS, _MENU_SOUNDS;

extern MENU_SOUNDS menuSounds;

// NH: Enumed Menu Sounds
enum
{
	EMSFX_NAVIGATE = 0,
	EMSFX_SELECT,
	EMSFX_BACK,
	EMSFX_ERROR,
};

#if BPLATFORM==GAMECUBE
// IH: Some GameCube only effects (although they could also be done on XBox)
enum
{
	ESFX_NO_EFFECTS = 0,
	ESFX_SMALL_ROOM_REVERB,
	ESFX_LARGE_ROOM_REVERB,
	ESFX_BIG_CAVERN_ECHO,

	ESFX_NOOF,
};

enum
{
	ESFX_NOT_FADING = 0,
	ESFX_FADING_IN,
	ESFX_FADING_OUT,
};

#define AUXA_MAX_VOLUME		904.0f
#define AUXA_FADE_SPEED		0.5f	// IH: seconds

#endif

#define STANDARD_FREQUENCY 11000
#define LOWEST_FREQUENCY 4000


/*  --------------------------------------------------------------------------------
	Function	: InitialiseSoundFX
	Purpose		: Setup the sound system callbacks
	Parameters	: 
	Returns		: true/false
	Info		: 
*/

bool InitialiseSoundFX(void);


/*	--------------------------------------------------------------------------------
	Function 	: LoadSamples
	Purpose 	: Load all samples from a pak file
	Parameters 	: pakfile
	Returns 	: 
	Info 		:
*/

int LoadSamples(char *pakfile);


/*	--------------------------------------------------------------------------------
	Function 	: FreeSamples
	Purpose 	: Free all samples loaded from a pak file
	Parameters 	: pakfile
	Returns 	: 
	Info 		:
*/

void FreeSamples(char *pakfile);


/*	--------------------------------------------------------------------------------
	Function 	: PlaySample
	Purpose 	: Play a sample
	Parameters 	: sample name, (opt/default)volume, (opt/NULL=non-positional)position, (opt/0=none)flags
	Returns 	: 
	Info 		:
*/

int PlaySample(char *name, int volume = SFX_DEFAULT_VOLUME, TBVector position = NULL, uint32 flags = 0);


/*	--------------------------------------------------------------------------------
	Function 	: PlaySample
	Purpose 	: Play a sample
	Parameters 	: (char *name, int volume, int pan, int frequency, TBVector position, uint32 flags)
	Returns 	: channel
	Info 		:
*/

int PlaySample(char *name, int volume, int pan, int frequency, TBVector position = 0, uint32 flags = 0);


#define STOPSAMPLE(_handle) {if (_handle!=-1) bsStopChannel(_handle); _handle = -1;}

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
int PlayTrackingSample(char *name, int volume, TBVector position, uint32 flags=0, const int frequency=0, const float min = SFX_minDist, const float range = SFX_maxDist, void (*TBTrackingSampleCallback)(TBVector pos, int timeDelta, void *context) = NULL, void *context = NULL);

/*	--------------------------------------------------------------------------------
	Function 	: InitialiseSoundHandler
	Purpose 	: Set up event list etc, to handle sound effect messages
	Parameters 	: 
	Returns 	: TRUE/FALSE
	Info 		: must be called after the event managers have been created
*/
int InitialiseSoundHandler(void);

/*	--------------------------------------------------------------------------------
	Function 	: DeleteSoundHandler
	Purpose 	: Remove sound effect handler
	Parameters 	: 
	Returns 	: 
	Info 		: must be called before the event managers have been deleted
*/
void DeleteSoundHandler(void);

/*	--------------------------------------------------------------------------------
	Function 	: MonitorSampleCallbacks
	Purpose 	: Check if samples have finished and callbacks are waiting
	Parameters 	: 
	Returns 	: 
	Info 		: Called once a frame.
*/
void MonitorSampleCallbacks(void);

/*	--------------------------------------------------------------------------------
	Function 	: ResetSoundCallbacks
	Purpose 	: Reset the status of all soundcallbacks
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void ResetSoundCallbacks(void);

/*	--------------------------------------------------------------------------------
	Function 	: CallbackWhenSampleFinished
	Purpose 	: Add callback to sound handle
	Parameters 	: sound handle, callback function (format void Function(void *context)), context
	Returns 	: channel
	Info 		:
*/
void CallbackWhenSampleFinished(int handle,void (*callback)(void *context),void *context);

/*	--------------------------------------------------------------------------------
	Function 	: InitCharacterSoundHandles
	Purpose 	: Initialises character sound handles structure
	Parameters 	: s
	Returns 	: 
	Info 		:
*/

void InitCharacterSoundHandles(struct TAG_CHARACTERSOUNDHANDLES *handles);

/*	--------------------------------------------------------------------------------
	Function 	: SFX_sceneInit
	Purpose 	: Sound initialisation at the beginning of each scene
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: 
*/
void SFX_sceneInit(void);

/*	--------------------------------------------------------------------------------
	Function 	: SFX_gameInit
	Purpose 	: Sound initialisation at the beginning of the game
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: 
*/
void SFX_gameInit(void);

/*	--------------------------------------------------------------------------------
	Function 	: CharacterSoundsFinishedCallback
	Purpose 	: Callback when character sound has finished playing
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void CharacterSoundsFinishedCallback(void *context);

/*	--------------------------------------------------------------------------------
	Function 	: SetSoundSpeed
	Purpose 	: Sets the sound speed to a value passed in.
	Parameters 	: 
	Returns 	: 
	Info 		: 1.0f = standard speed.
*/

void SetSoundSpeed(float fValue = 1.0f);

/*	--------------------------------------------------------------------------------
	Function 	: SetHearingRange
	Purpose 	: Set the maximum distance that positional sound can travel and still be heard
	Parameters 	: the maximum distance that positional sound can travel and still be heard (in units),
					or INFINTE_HEARING_RANGE if you want infinite range
	Returns 	: 
	Info 		: // PP: removed default parameter cos I wanted to move sound range defines out of header
*/

#define INFINTE_HEARING_RANGE		(-1.0f)

/*	--------------------------------------------------------------------------------
	Function 	: SetHearingRange
	Purpose 	: Set the minimum and maximum distance that positional sound can travel and still be heard
	Parameters 	: the maximum distance that positional sound can travel and still be heard (in units),
					or INFINTE_HEARING_RANGE if you want infinite range
	Returns 	: 
	Info 		: 
*/

void SetHearingRange(const float minRange, const float maxRange);

/*  --------------------------------------------------------------------------------
	Function	: SetSoundPos
	Purpose		: Set the position of sounds for any following messages
	Parameters	: position
	Returns		: 
	Info		: 
*/

void SetSoundPos(TBVector position);

/*  --------------------------------------------------------------------------------
	Function	: StopAllChannels
	Purpose		: Stop all channels and clear any callbacks etc.
	Parameters	: 
	Returns		: 
	Info		: 
*/

void StopAllChannels(void);

/*  --------------------------------------------------------------------------------
	Function	: StopChannel
	Purpose		: Called to stop a currently playing sample
	Parameters	: pointer to sampleHandle
	Returns		: 
	Info		: the sampleHandle will be set to -1 when stopped
*/
void StopChannel(int *sampleHandle);


/*	--------------------------------------------------------------------------------
	Function 	: RespondToSoundMessages
	Purpose 	: Respond to sound messages generated
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void RespondToSoundMessages(void);


void SetHearingRange(const float range);


void LoadLevelSamplesBackground();
void LoadLevelSamplesMemory();
void FreeLevelSamples();

void LoadLevelIntroSamplesMemory();
void FreeLevelIntroSamples();

void LoadCharacterSamples(char *name);
void LoadCharacterSamplesBkg(char *namein);

void PlayOpeningSoundCallback(FADE *fade);

void PlayAmbientSounds(int);
void PlayRandomAmbients(int);


#if BPLATFORM==GAMECUBE
// IH: Residents
void LoadAllResidentSamplesBackground(void);
void MoveAllResidentSamplesToARAM(void);
void FreeAllResidentSamplesFromARAM(void);

// IH: Level samples
void LoadAllLevelSamplesBackground(void);
void MoveAllLevelSamplesToARAM(void);
void FreeAllLevelSamplesFromARAM(void);

// IH: Reverb
void InitEffects(void);
void ShutdownEffects(void);
void FadeInEffect(int effectType);
void FadeOutEffect(void);
void SetupSoundEffect(int channel);
void UpdateEffects(void);
#endif

#endif