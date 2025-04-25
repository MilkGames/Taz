// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : MemCard.h
//   Purpose : memory-card loading & saving, and the graphical interface for it
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_MEMCARD
#define __H_MEMCARD

#include "TextFX.h"		// PP: for BOOK base class


// PP: define MEMCARD_DISPLAY_MESSAGE_WHILE_LOADING if a message is to be displayed while loading a saved game
#if(BPLATFORM != XBOX)
#define MEMCARD_DISPLAY_MESSAGE_WHILE_LOADING
#endif// PP: platform

// PP: define LOAD_AFTER_START_SCREEN to delay global loading till after the PRESS START screen
#if(BPLATFORM == XBOX)// PP: TCR 3-41 or 3-53
#define LOAD_AFTER_START_SCREEN
#endif// PP: platform

// PP: define XBOX_DAMAGED_SAVE_HANDLING to handle corrupted saves in the way that the Xbox does,
// PP: ie. showing "Damaged game" in each save slot rather than "Empty game", and not showing temporary 'saves' if the player decides not to save,
#if(BPLATFORM == XBOX)
#define XBOX_DAMAGED_SAVE_HANDLING
#endif// PP: platform

// PP: define XBOX_FULL_MEDIUM_HANDLING to handle a save medium with insufficient space in the way that the Xbox does,
// PP: ie. not showing temporary 'saves' if the player decides not to save.
#if(BPLATFORM == XBOX)
#define XBOX_FULL_MEDIUM_HANDLING
#endif// PP: platform

// PP: type for a ptr to a function called when the damaged-save message is OK'd
// PP:	(ie. player chooses to continue without saving)
typedef void(*DamagedSaveOKCallback)(void);

// PP: type for a ptr to a function called when the disk-full message is OK'd
// PP:	(ie. player chooses to continue without saving)
typedef void(*FullDiskOKCallback)(void);

// PP: ptr to function called when the damaged-save message is OK'd
// PP:	(ie. player chooses to continue without saving)
extern DamagedSaveOKCallback		MemCard_damagedSaveOKCallback;

// PP: ptr to function called when the disk-full message is OK'd
// PP:	(ie. player chooses to continue without saving)
extern FullDiskOKCallback			MemCard_fullDiskOKCallback;

// PP: type for callback that gets called when the load/save operation completes
typedef int(*MemCardFinishFunc)(const bool success);

#ifdef XBOX_DAMAGED_SAVE_HANDLING// PP: MemCard.h
// PP: MemCard_corruptedSave is set true when corrupted saves are found on the storage medium
extern bool MemCard_corruptedSave;
#endif// PP: def XBOX_DAMAGED_SAVE_HANDLING

#ifdef XBOX_FULL_MEDIUM_HANDLING// PP: MemCard.h
// PP: MemCard_fullMedium is set true when the storage medium is found to have insufficient space to create a save
extern bool MemCard_fullMedium;
#endif// PP: def XBOX_DAMAGED_SAVE_HANDLING

extern bool MemCard_busy;							// PP: whether or not there's a memory card operation in progress at the moment
extern bool MemCard_allowAccess;
extern bool usingMemCard;
extern TBGameSaveInfo 		mySaveGameInfo;

/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
															MEMCARDBOOK

									This is the 'stamp' book type used to display memory card dialogue.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: **** DEFINE THIS TO ENABLE THE OLD 'STAMP' BOOK ****
// PP: #define MEMCARD_STAMP_BOOK
// PP: ****************************************************

#define MCD_BIG_FONT_SCALE		4.0f				// PP: scale used for text on minimized pages

// PP: subclass-specific flags
enum
{
	MCDFLAG_OK				=	1,					// PP: book is showing, or is due to show, a final (ok) message before fading out

};

// PP: modes
enum
{
	MCDMODE_MINIMIZED,								// PP: book is in the form of a small stamp in the top-right corner of the screen
	MCDMODE_MAXIMIZED								// PP: book is expanded to show proper dialogues, and centred on the screen

};

typedef struct TAG_MEMCARDBOOK	:	public BOOK
{

	// PP: this needs to initialise the static texture pointers
	friend void MemCard_init(void);

private:

	int					MCDflags;					// PP: subclass-specific flags (MCDFLAG_)
	float				okProgress;					// PP: timer for the OK state

	/*	--------------------------------------------------------------------------------
		Function 	: MEMCARDBOOK::init
		Purpose 	: initialise the MEMCARDBOOK
		Parameters 	: 
		Returns 	: 
		Info 		: called by the constructor.  Overrides BOOK:init
	*/
	void init(void);

public:

	/*	--------------------------------------------------------------------------------
		Function 	: MEMCARDBOOK::TAG_MEMCARDBOOK
		Purpose 	: default constructor; calls MEMCARDBOOK::init
		Parameters 	: 
		Returns 	: new MEMCARDBOOK instance
		Info 		: 
	*/
	TAG_MEMCARDBOOK(void);


	/*	--------------------------------------------------------------------------------
		Function 	: MEMCARDBOOK::ok
		Purpose 	: start the book displaying a final (ok) message before fading out
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void ok(void);


	/*	--------------------------------------------------------------------------------
		Function 	: MEMCARDBOOK::update
		Purpose 	: update the memory card book
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOOK::update
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: MEMCARDBOOK::minimize
		Purpose 	: start the book shrinking to its small size, for non-critical messages
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void minimize(void);


	/*	--------------------------------------------------------------------------------
		Function 	: MEMCARDBOOK::maximize
		Purpose 	: start the book expanding to its full size, for error messages
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void maximize(void);


}MEMCARDBOOK, _MEMCARDBOOK BALIGN16;

extern MEMCARDBOOK MemCard_book;
extern char MemCard_bootUp;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PUBLIC MAIN FUNCTIONS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: MemCard_init
	Purpose 	: initialise memory card stuff
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void MemCard_init(void);


/*	--------------------------------------------------------------------------------
	Function 	: MemCard_update
	Purpose 	: check up on and update the current memory card load or save
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void MemCard_update(void);


/*	--------------------------------------------------------------------------------
	Function 	: MemCard_save
	Purpose 	: start saving some data to the card, using the specified filename
	Parameters 	: data to save, filename to save as, number of bytes to save,
					(opt/NULL)ptr to function to be called when the operation completes
	Returns 	: 
	Info 		: 
*/
void MemCard_save(void* const data, ushort* name, const int size, const MemCardFinishFunc finishFunc=NULL);


/*	--------------------------------------------------------------------------------
	Function 	: MemCard_load
	Purpose 	: start loading some data from the card, using the specified filename
	Parameters 	: buffer to receive data, filename to load as, number of bytes to load,
					(opt/NULL)ptr to function to be called when the operation completes
	Returns 	: 
	Info 		: 
*/
void MemCard_load(void* const data, ushort* name, const int size, const MemCardFinishFunc finishFunc=NULL);


/*	--------------------------------------------------------------------------------
	Function 	: MemCard_shutdown
	Purpose 	: free up any memory used by this module
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void MemCard_shutdown(void);

/*	--------------------------------------------------------------------------------
	Function 	: MemCard_setupSaveInfo
	Purpose 	: setup to save icon info
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void MemCard_setupSaveInfo(void);

/*  --------------------------------------------------------------------------------
	Function	: MemCard_updateSaveInfo
	Purpose		: Update game save information before a save
	Parameters	: 
	Returns		: 
	Info		: // PP: need to do this on Xbox to get the right icon to save to the card
*/
void MemCard_updateSaveInfo(void);


/*  --------------------------------------------------------------------------------
	Function	: LoadGame
	Purpose		: Load from the memcard and wait for completion then transfer data to ingame structs
	Parameters	: 
	Returns		: 
	Info		: 
*/

void LoadGame(bool wait=true);


/*  --------------------------------------------------------------------------------
	Function	: CheckMemCardOnBoot
	Purpose		: Called during boot up to check the memory card, this function will then either load or save data
	Parameters	: wait for completion
	Returns		: 
	Info		: 
*/

void CheckMemCardOnBoot(bool wait = true, int mode = BGAMESAVE_LOAD);


/*  --------------------------------------------------------------------------------
	Function	: CheckMemCardOnStart
	Purpose		: perform initial global load after the PRESS START screen
	Parameters	: 
	Returns		: 
	Info		: // PP: named to match CheckMemCardOnBoot, since it's the same function really
*/
void CheckMemCardOnStart(void);


/*  --------------------------------------------------------------------------------
	Function	: SaveGame
	Purpose		: Save to the memcard and wait for completion then transfer data to ingame structs
	Parameters	: 
	Returns		: 
	Info		: 
*/
void SaveGame(int scene, bool forceSave = false);

/*  --------------------------------------------------------------------------------
	Function	: InitialiseSaveGameSlot
	Purpose		: Initialises data in a slot before a new game is chosen
	Parameters	: 
	Returns		: 
	Info		: 
*/

void InitialiseSaveGameSlot(uint32 slotNumber, EDifficulty difficulty);


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
											XBOX LOAD/SAVE DIALOGUE PAGE UPDATE CALLBACKS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: MemCard_XboxUpdateFunc_HARDDISK_SAVE
	Purpose 	: update function for an Xbox memory dialogue page:
					Saving to the hard drive
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: named so as to be macro-friendly
					// PP: Must match the PageFunc typedef (TextFX.h)
*/
void MemCard_XboxUpdateFunc_HARDDISK_SAVE(struct TAG_PAGE* const page, const int32 context);


/*	--------------------------------------------------------------------------------
	Function 	: MemCard_XboxUpdateFunc_DAMAGED_GAME
	Purpose 	: update function for an Xbox memory dialogue page:
					Failed to load / loaded bad data
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: named so as to be macro-friendly
					// PP: Must match the PageFunc typedef (TextFX.h)
*/
void MemCard_XboxUpdateFunc_DAMAGED_GAME(struct TAG_PAGE* const page, const int32 context);


/*	--------------------------------------------------------------------------------
	Function 	: MemCard_XboxOpenFunc_HARDDISK_FULL
	Purpose 	: page-open function for an Xbox memory dialogue page:
					Tried to save but the hard drive is full
	Parameters 	: ptr to the page, context value
	Returns 	: 
	Info 		: // PP: Must match the PageFunc typedef (TextFX.h)
*/
void MemCard_XboxOpenFunc_HARDDISK_FULL(struct TAG_PAGE* const page, const int32 context);


/*	--------------------------------------------------------------------------------
	Function 	: MemCard_XboxUpdateFunc_HARDDISK_FULL
	Purpose 	: update function for an Xbox memory dialogue page:
					Tried to save but the hard drive is full
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: named so as to be macro-friendly
					// PP: Must match the PageFunc typedef (TextFX.h)
*/
void MemCard_XboxUpdateFunc_HARDDISK_FULL(struct TAG_PAGE* const page, const int32 context);


#endif // PP: __H_MEMCARD