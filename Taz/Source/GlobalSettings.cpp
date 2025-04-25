// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : GlobalSettings.cpp
//   Purpose : Global settings adjustable from the front end, eg. volume levels
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"					// PP: Global stuff
#include "actors.h"
#include "main.h"					// PP: Main stuff
#include "memcard.h"
#include "frontendmain.h"			// PP: for some default defines
#include "Cheats.h"					// PP: Cheats
#include "GlobalSettings.h"			// PP: Global settings adjustable from the front end, eg. volume levels
#include "music.h"					// PP: music

#define SAVE_CRC_START	(0)			// TP: increment this to invalidate old save games


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														GLOBAL SETTINGS

											Settings adjustable from the front-end.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*  --------------------------------------------------------------------------------
	Function	: IsLevelComplete
	Purpose		: Determine if the specified level has been completed
	Parameters	: level
	Returns		: true/false
	Info		: 
*/
bool IsLevelComplete(int level)
{
	// TP: If there is a slot chosen then return level complete based on slot information
	if (gameStatus.globalSettings.currentSaveSlot!=-1)
	{
		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[level].completed)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	// TP: If no slot has been chosen then return the global stats information of levels completed
	if (gameStatus.globalSettings.saveInfo.stats[ConvertSceneRefToLevelScoreRef((EScene)level)].levelCompleted)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*	-------------------------------------------------------------------------------
	Function 	: GLOBALSETTINGS::init
	Purpose 	: initialise the game settings struct with default values
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void GLOBALSETTINGS::init(void)
{
	// PP: milestone bodge ahoy!
#ifdef STREAM_VOLUME_WORKAROUND
	this->musicVolume	=	170;// PP: QUICK MILESTONE BODGE
#else// PP: ifndef STREAM_VOLUME_WORKAROUND
	this->musicVolume	=	DEFAULT_MUSIC_VOLUME;
#endif// PP: ndef STREAM_VOLUME_WORKAROUND

	this->sfxVolume		=	DEFAULT_SFX_VOLUME;
	this->vibration		=	DEFAULT_VIBRATION;
	this->subtitles		=	TRUE;

	this->cheatsActive	=	NO_CHEATS;
#ifdef INFOGRAMES
	this->difficulty	=	DIFF_EASY;
#else
#ifdef JWEEKS
	this->difficulty	=	DIFF_MEDIUM;
#else
	this->difficulty	=	DIFF_HARD;
#endif
#endif
	// IH: Put it in easy as there are more sangers and therefore more ram is used (worst case)
	// IH: In facyt, put it in medium so I don't have to go through all of those pesky hints
#if BPLATFORM==GAMECUBE
#ifndef INFOGRAMES
	this->difficulty	=	DIFF_MEDIUM;
#endif
#endif
	this->audioMode		=	AU_STEREO;
	this->currentSaveSlot = 0;	// TP: must always have a slot chosen
	// CMD: mark save slots as being not in use
	InitialiseSaveData();
}


/*	--------------------------------------------------------------------------------
	Function 	: GLOBALSETTINGS::TAG_GLOBALSETTINGS
	Purpose 	: default constructor, initialise the game settings struct with default values
	Parameters 	: 
	Returns 	: new GLOBALSETTINGS instance
	Info 		: 
*/
GLOBALSETTINGS::TAG_GLOBALSETTINGS(void)
{
	this->init();
}


/*  --------------------------------------------------------------------------------
	Function	: GLOBALSETTINGS::InitialiseSaveData
	Purpose		: This function will clear and initialise all the save data and structures
	Parameters	: 
	Returns		: 
	Info		: WARNING: this will wipe all save data from memory
*/

void GLOBALSETTINGS::InitialiseSaveData(void)
{
	InitialiseSaveGameSlot(0, DIFF_MEDIUM);
	InitialiseSaveGameSlot(1, DIFF_MEDIUM);
	InitialiseSaveGameSlot(2, DIFF_MEDIUM);

	saveInfo.saveSlot[0].inUse = FALSE;
	saveInfo.saveSlot[1].inUse = FALSE;
	saveInfo.saveSlot[2].inUse = FALSE;
	saveInfo.saveSlot[0].lastIcon = -1;
	saveInfo.saveSlot[1].lastIcon = -1;
	saveInfo.saveSlot[2].lastIcon = -1;

	// PP: NOTE: The assumption is made in UpdateNameEntryPage (endoflevelstats.cpp)
	// PP: that the initials all get initialised to {0,0,0}.
	// PP: So we'd better do that now then!
	{
		saveInfo.saveSlot[0].lastName[0]=0;
		saveInfo.saveSlot[0].lastName[1]=0;
		saveInfo.saveSlot[0].lastName[2]=0;

		saveInfo.saveSlot[1].lastName[0]=0;
		saveInfo.saveSlot[1].lastName[1]=0;
		saveInfo.saveSlot[1].lastName[2]=0;

		saveInfo.saveSlot[2].lastName[0]=0;
		saveInfo.saveSlot[2].lastName[1]=0;
		saveInfo.saveSlot[2].lastName[2]=0;
	}

	memset(&saveInfo.galleryFlags, 0, sizeof(GALLERYFLAGS));

	saveInfo.musicVolume = musicVolume;
	saveInfo.sfxVolume = sfxVolume;	
	saveInfo.vibration = vibration;	
	saveInfo.scrXadj = scrXadj;
	saveInfo.scrYadj = scrYadj;
	saveInfo.subtitles = subtitles;	
	saveInfo.audioMode = audioMode;	

	InitialiseHighScores();
}


// PP: TEMP TEST
/*  --------------------------------------------------------------------------------
	Function	: GLOBALSETTINGS::InitialiseSaveDataCorrupt
	Purpose		: Initialise save data in memory in response to finding corrupt save
					data on the storage medium.
	Parameters	: 
	Returns		: 
	Info		: // PP: Microsoft want corrupt games to be visible but unuseable,
					rather than being shown as 'empty'
					WARNING: this will wipe all save data from memory
*/
void GLOBALSETTINGS_InitialiseSaveDataCorrupt(void)// PP: TEMP TEST
{
	//*

	// PP: TEMP TEST

	// PP: clear everything to defaults
	gameStatus.globalSettings.InitialiseSaveData();

	// PP: but also create three visible default game slots
	for(int i=0; i<3; i++)
	{
		gameStatus.globalSettings.saveInfo.saveSlot[i].inUse = TRUE;// PP: make the slot visible
		gameStatus.globalSettings.saveInfo.saveSlot[i].lastIcon = 0;// PP: taz 1
	}

	/*/

	// PP: clear everything to defaults
	this->InitialiseSaveData();

	// PP: but also create three visible default game slots
	for(int i=0; i<3; i++)
	{
		saveInfo.saveSlot[i].inUse = TRUE;// PP: make the slot visible
		saveInfo.saveSlot[i].lastIcon = 0;// PP: taz 1
	}

  //*/
}



/*  --------------------------------------------------------------------------------
	Function	: GLOBALSETTINGS::ReadFromGameSave
	Purpose		: When a load from memory card is complete this must be called to transfer the data to the ingame structure
	Parameters	: 
	Returns		: 
	Info		: 
*/

int GLOBALSETTINGS::ReadFromGameSave(void)
{
	uint32		newCRC;
	// TP: must check data is valid firs
	newCRC = bkCRC32(((uchar*)(&saveInfo.saveCRC))+4,MIN_MEMCARD_SAVESIZE-4,SAVE_CRC_START);
	if (newCRC != saveInfo.saveCRC)
	{
		// TP: WARNING save data is corrupt, must now clear the save data structure
		InitialiseSaveData();
		bkPrintf("*** WARNING *** save data is corrupt.\n");
		return FALSE;
	}
	else
	{
		bkPrintf("Save data is valid.\n");
	}

	musicVolume = saveInfo.musicVolume;
	sfxVolume = saveInfo.sfxVolume;
	vibration = saveInfo.vibration;
	scrXadj = saveInfo.scrXadj;
	scrYadj = saveInfo.scrYadj;
	subtitles = saveInfo.subtitles;

// NH(28/05/02): Don't need this. Just caused problems.  Already set up to the default, and the new games are set to the global, and old games copy their difficulty across to the current difficulty.
/*
	if (this->currentSaveSlot!=-1)
	{
		difficulty = saveInfo.saveSlot[this->currentSaveSlot].difficulty;
	}
*/
//	fmemcpy(highScores,saveInfo.highScores,sizeof(highScores));
//	fmemcpy(stats,saveInfo.stats,sizeof(stats));

	bsSetGlobalAudioStreamVolume(musicVolume);
	bsSetGlobalSampleVolume(sfxVolume);
	bdSetScreenOffset(scrXadj, scrYadj);

	return TRUE;
}

/*  --------------------------------------------------------------------------------
	Function	: GLOBALSETTINGS::WriteToGameSave
	Purpose		: Write information from the ingame structure into the save structure ready for saving
	Parameters	: 
	Returns		: 
	Info		: 
*/
int GLOBALSETTINGS::WriteToGameSave(void)
{
	saveInfo.musicVolume = musicVolume;
	saveInfo.sfxVolume = sfxVolume;
	saveInfo.vibration = vibration;
	saveInfo.scrXadj = scrXadj;
	saveInfo.scrYadj = scrYadj;
	saveInfo.subtitles = subtitles;

	if (this->currentSaveSlot!=-1)
	{
		saveInfo.saveSlot[this->currentSaveSlot].difficulty = difficulty;
	}

//	fmemcpy(saveInfo.highScores,highScores,sizeof(highScores));
//	fmemcpy(saveInfo.stats,stats,sizeof(stats));

	// TP: NOTE: this line MUST come last!!!!
	saveInfo.saveCRC = bkCRC32(((uchar*)(&saveInfo.saveCRC))+4,MIN_MEMCARD_SAVESIZE-4,SAVE_CRC_START);

	// PP: must return a value
	return 0xB00B5;
}
