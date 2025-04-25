// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : GlobalSettings.h
//   Purpose : Global settings adjustable from the front end, eg. volume levels
// Component : Taz
// Info      : 
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_GLOBALSETTINGS
#define __H_GLOBALSETTINGS

#include "highscores.h"

/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														MULTIPLAYER SETUP
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


// CMD: difficulty levels
// PP: NOTE: In places I assume that easy=0, medium=1 & hard=2
// PP:			So If you're thinking of changing it please let me know
enum EDifficulty
{
	DIFF_EASY,
	DIFF_MEDIUM,
	DIFF_HARD,

	NUM_DIFFICULTY_LEVELS		// PP: Sorry, but this was needed
};

// CMD: audio modes
enum EAudioMode
{
	AU_MONO,
	AU_STEREO,
	AU_DOLBYPROLOGIC,
};


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														GLOBAL SETTINGS

											Settings adjustable from the front-end.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#define MAX_MUSIC_VOLUME				255					// PP: maximum music volume
#define MAX_SFX_VOLUME					255					// PP: maximum sound effects volume

// TP: 8 available
#define LEVELFLAG_SEENINTRO				(1<<0)				// TP: this flag is set in the saveslot structure if the player has viewed the intro cutscene in the level completely
#define LEVELFLAG_SEENOUTRO				(1<<1)				// TP: this flag is set in the saveslot structure if the player has viewed the outro cutscene in the level completely
#define LEVELFLAG_SEENLEVEL1COMPLETE    (1<<2)
#define LEVELFLAG_SEENLEVEL2COMPLETE    (1<<3)
#define LEVELFLAG_SEENLEVEL3COMPLETE    (1<<4)
#define LEVELFLAG_HASVISITEDLEVEL		(1<<5)				// TP: has player visited this level before

typedef struct TAG_GALLERYFLAGS
{
	bool galleryFlags[10];
} GALLERYFLAGS;

typedef struct TAG_SFXFLAGS
{
	short posterHelpIntro:1;
	short posterHelpFirst:1;
	short posterHelpFinal:1;
} SFXFLAGS;

typedef struct TAG_LEVELSAVEINFO
{
	uint32				completed;
	bool				swirls[120];
	int					pickupsCollected;
	bool				posters[10];
	int					postersCollected;
	int					timesCaptured;
	int32				levelBounty;
	int					objectsDestroyed;					// JW: Number of object destroyed in this level
	uint32				totalDestructiblesInMap;			// CMD: the total number of destructible objects in this level
	uchar				flags;								// TP: level flags of interest
	bool				secretItemCollected;				// TP: has the secret item been collected
	bool				destructBonusCollected;				// JW: Has the destruct bonus been won?
	bool				bonusgameOption;					// JW: Has player been given the chance to play the bonus game?
	float				time;
} LEVELSAVEINFO;

typedef struct TAG_SAVESLOT
{
	LEVELSAVEINFO		level[SCENE_NUMSCENES];				// TP: Level specific information
	uint32				inUse;								// CMD: does the slot contain save info?
	EDifficulty			difficulty;							// CMD: difficulty level of selected game
	TBClock				creation;							// CMD: time and date at which the file was created
	int32				bounty;								// current bounty on taz
	uint32				telephoneBountyCounter;				// CMD: bounty counter for telephone boxes only
	int					lastIcon;							// JW: Last icon selected on icon select screen
	char				lastName[3];						// JW: Last name entered on name entry screen
	uint32				helpFlags;							// CMD: flags used by help system in 'prompt.h'
} SAVESLOT;

// TP: Front end stats keeps track of the best completion of a level
typedef struct TAG_FRONTENDSTATS
{
	int		totalPosters;
	int		postersTriggered;
	int		collectibles;				//number of colectibles collected
	float	destructibles;				//percentage of destructibles destroyed

	int		money;
	int		levelCompleted;				//has the level been completed?
	float	time;					//[hours][minutes][seconds]
} FRONTENDSTATS, _FRONTENDSTATS BALIGN16;

typedef struct TAG_SAVEINFO
{
	// TP: CRC MUST be the first member of this structure
	uint32				saveCRC;							// TP: crc of save information
	// TP: Contains all information for load and save structure, note this is kept seperate from the 
	// TP: ingame versions of the variables so there is no possibility of simultanious access.
	unsigned char		musicVolume;						// PP: music volume (0..255)
	unsigned char		sfxVolume;							// PP: sound effects volume (0..255)
	bool				vibration;							// PP: whether or not pad vibration is enabled
	int					scrXadj;
	int					scrYadj;
	int					subtitles;							// CMD: whether or not subtitles are desired
	EAudioMode			audioMode;							// CMD: current audio mode

	HIGHSCORES			highScores[SCORES_TOTALNUM];		// TP: high scores
	FRONTENDSTATS		stats[SCORES_TOTALNUM];
	GALLERYFLAGS		galleryFlags;						// JW: Flags for the gallery pages

	SAVESLOT			saveSlot[4];
} SAVEINFO;

#if BPLATFORM==PS2
	#define ICON_SIZE (94592)
#else
	// TP: this needs to be set to an appropriate value for the other platforms
	#define ICON_SIZE (0)
#endif

// TP: memcard size frigged to use 175k only (this was hell on toast)
#if BPLATFORM == PS2
	#define MIN_MEMCARD_SAVESIZE ((175*1024)-ICON_SIZE-sizeof(sceMcIconSys)-(5*1024))	
#else
	#define MIN_MEMCARD_SAVESIZE ((175*1024)-ICON_SIZE)	
#endif

typedef struct TAG_GLOBALSETTINGS
{
	// PP: System settings ~~~~~~~~~~~~
	unsigned char		musicVolume;						// PP: music volume (0..255)
	unsigned char		sfxVolume;							// PP: sound effects volume (0..255)
	bool				vibration;							// PP: whether or not pad vibration is enabled
	int					scrXadj;
	int					scrYadj;
	int					subtitles;
	EDifficulty			difficulty;							// CMD: difficulty level of selected game
	EAudioMode			audioMode;							// CMD: current audio mode
															
	// PP: Cheat stuff ~~~~~~~~~~~~~~~~						
	int					cheatsActive;						// PP: cheats wot are switched on

	char				currentSaveSlot;					// TP: current save slot/ -1 none selected

	union
	{
		char				savePad[MIN_MEMCARD_SAVESIZE];				// TP: pad save game so it takes 175kb on PS2
		SAVEINFO			saveInfo;
	};

	/*  --------------------------------------------------------------------------------
		Function	: ReadFromGameSave
		Purpose		: When a load from memory card is complete this must be called to transfer the data to the ingame structure
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	int ReadFromGameSave(void);

	/*  --------------------------------------------------------------------------------
		Function	: WriteToGameSave
		Purpose		: Write information from the ingame structure into the save structure ready for saving
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	int WriteToGameSave(void);


	/*  --------------------------------------------------------------------------------
		Function	: InitialiseSaveData
		Purpose		: This function will clear and initialise all the save data and structures
		Parameters	: 
		Returns		: 
		Info		: WARNING: this will wipe all save data from memory
	*/
	void InitialiseSaveData(void);


	// PP: TODO: multiplayer games available
	/*	--------------------------------------------------------------------------------
		Function 	: GLOBALSETTINGS::init
		Purpose 	: initialise the game settings struct with default values
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void init(void);


	/*	--------------------------------------------------------------------------------
		Function 	: GLOBALSETTINGS::TAG_GLOBALSETTINGS
		Purpose 	: default constructor, initialise the game settings struct with default values
		Parameters 	: 
		Returns 	: new GLOBALSETTINGS instance
		Info 		: 
	*/
	TAG_GLOBALSETTINGS(void);


}GLOBALSETTINGS, _GLOBALSETTINGS BALIGN16;

/*  --------------------------------------------------------------------------------
	Function	: IsLevelComplete
	Purpose		: Determine if the specified level has been completed
	Parameters	: level
	Returns		: true/false
	Info		: 
*/
bool IsLevelComplete(int level);


#endif // PP: ndef __H_GLOBALSETTINGS