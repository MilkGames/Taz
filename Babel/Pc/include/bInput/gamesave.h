// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games
//
//      File : gamesave.h
//   Purpose : Loading & saving of game data
// Component : Generic Input
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BINPUT_GAMESAVE_H__
#define __BINPUT_GAMESAVE_H__


// ********************************************************************************
// Constants and Macros

// maximum number of game saves supported
#define BMAXGAMESAVES				64

// File version number
#define BGAMESAVE_VERSION			1

// load game or save game for biGameSaveStart
typedef enum EBGameSaveMode
{
	BGAMESAVE_LOAD = 0,
	BGAMESAVE_SAVE,
};

// Save game flags
#define BGAMESAVE_NOMEDIAAUTOSELECT	0x00000001

// response values for biGameSaveGetCardSlotStatus
typedef enum EBGameSaveSlotStatus 
{
	BGAMESAVE_SLOT_NOMEDIA,							// Nothing is inseted
	BGAMESAVE_SLOT_MEDIAPRESENT,					// Media is inserted
	BGAMESAVE_SLOT_UNFORMATED,						// Unformated media is inserted
	BGAMESAVE_SLOT_ACTIVESLOT,						// Media present and this slot is active for saves
};

// response values for biGameSaveSetStatus
typedef enum EBGameSaveResponse 
{
	BGAMESAVE_NORESPONSE,
	BGAMESAVE_OKRESPONSE,
	BGAMESAVE_CANCELRESPONSE,
};

// gamesave state values
typedef enum EBGameSaveStatus 
{
	BGAMESAVE_NOSTATE,
	
	BGAMESAVE_CHECKINGMEDIA,		// media is currently being checked
	BGAMESAVE_NOMEDIA,				// no media is present, present user with error message

	BGAMESAVE_INVALIDMEDIA,			// invalid media present, present user with error message (PS2)

	BGAMESAVE_SELECTYN,				// select this media y/n ?
	BGAMESAVE_READINGFILESYSTEM,	// Reading directory/file usage on the media

	BGAMESAVE_UNFORMATED,			// unformated memory card and OS is unable to format it (Xbox)
	BGAMESAVE_BROKENFORMATMEDIAYN,	// media is unformatted, present user with format y/n? (GameCube: broken card)
	BGAMESAVE_FORMATMEDIAYN,		// media is unformatted, present user with format y/n?
	BGAMESAVE_FORMATINPROGRESS,		// formatting memory card in progress
	BGAMESAVE_FORMATFAILED,			// format failed, present user with error message
	BGAMESAVE_FORMATSUCCESSFUL,		// format successful. This state is used just to inform the user that the format
									// successfully finished. On the users next call to biGameSaveGetStatus(), the state
									// is immediately switched to BGAMESAVE_CHECKINGMEDIA to continue with the save. (PS2 and GameCube only)
	BGAMESAVE_MEDIAFULL,			// media is full, present user with error message
	BGAMESAVE_MEDIAFULL_MANAGEYN,	// media is full, present user with option to reboot to manage the card
	BGAMESAVE_SAVEYN,				// media is ok, present user with save y/n?
	BGAMESAVE_SAVEINPROGRESS,		// save is in progress, notify user of progress
	BGAMESAVE_SAVEOK,				// save completed sucessfully
	BGAMESAVE_SAVEFAILED,			// save failed, present user with error message
	BGAMESAVE_SAVECANCELED,			// the user canceled the save

	BGAMESAVE_LOADYN,				// media is ok, present user with load y/n?
	BGAMESAVE_LOADNOTFOUND,			// save game not found
	BGAMESAVE_LOADINPROGRESS,		// load is in progress, notify user of progress
	BGAMESAVE_LOADOK,				// load completed sucessfully
	BGAMESAVE_LOADFAILED,			// load failed, present user with error message
	BGAMESAVE_LOADFAILEDCRC,		// load failed crc check, present user with error message
	BGAMESAVE_LOADCANCELED,			// the user canceled the load

	BGAMESAVE_DAMAGEDSAVE,			// an attempt was made to load or overwrite a damaged save, present user with error
};


// ********************************************************************************
// Types and Structures

// directory entry, used for reporting game save files
typedef struct _TBGameSaveDirEntry
{
	ushort	filename[128];		// filename of save game
	int		size;				// size in bytes
	char	cSec, cMin, cHour;	// creation time
	char	cDay, cMonth;
	short	cYear;				// creation date
	char	mSec, mMin, mHour;	// modification time
	char	mDay, mMonth;
	short	mYear;				// modification date
} TBGameSaveDirEntry;


// ********************************************************************************
// Globals

extern EBGameSaveMode	bCurrentGameSaveMode;	// are we loading or saving?
extern EBGameSaveStatus	bCurrentGameSaveStatus;	// what is the current state?


// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : biGameSaveStart
   Purpose : Start the gamesave process
   Parameters : load game or save game?, pointer to gamesave data, gamesave name prefix, gamesave size in bytes, flags
   Returns :
   Info : 
*/

void biGameSaveStart(EBGameSaveMode loadSave, void *saveData, ushort *saveNamePrefix, int saveSize, int flags=0);


/* --------------------------------------------------------------------------------
   Function : biGameSaveGetStatus
   Purpose : Get current gamesave status
   Parameters :
   Returns : current gamesave status (EBGameSaveStatus), directory entries to fill, number of dir entries to get
   Info : 
*/

EBGameSaveStatus biGameSaveGetStatus(TBGameSaveDirEntry *dirEntries, int numDirEntries, int *numFiles, struct _TBGameSaveInfo *saveInfo);


/* --------------------------------------------------------------------------------
   Function : biGameSaveSetStatus
   Purpose : Set the current gamesave status
   Parameters : response value (EBGameSaveResponse), full load/save filename, file to delete, platform specific extra info
   Returns : 
   Info : 
*/

void biGameSaveSetStatus(EBGameSaveResponse response, ushort *saveName, int deleteFile, struct _TBGameSaveInfo *saveInfo);


/* --------------------------------------------------------------------------------
   Function : biGameSaveEnd
   Purpose : End the gamesave process
   Parameters : 
   Returns :
   Info : 
*/

void biGameSaveEnd();


#endif // __BINPUT_GAMESAVE_H__