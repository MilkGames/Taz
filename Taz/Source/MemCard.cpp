// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : MemCard.cpp
//   Purpose : memory-card loading & saving, and the graphical interface for it
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "maths.h"
#include "main.h"
#include "actors.h"				// PP: needed by control.h
#include "control.h"			// PP: pad input stuff
#include "Timer.h"				// PP: rectangles
#include "SFX.h"				// PP: sound effects
#include "Text2.h"				// PP: text string enumerations
#include "MemCard.h"			// PP: memory-card loading & saving, and the graphical interface for it
#include "Icon.h"				// PP: icons, just for testing at the moment
#include "scene.h"
#include "files.h"
#include "pageitemfx.h"
#include "prompt.h"

#ifdef LOAD_AFTER_START_SCREEN// PP: MemCard.h
#include "frontendmenu.h"		// PP: functions to create and control the front end menu system
#include "startgamemenu.h"		// PP: functions to create and control the start game menu
#endif// PP: def LOAD_AFTER_START_SCREEN

// PP: ptr to default function called when the damaged-save message is OK'd
// PP:	(ie. player chooses to continue without saving)
#define MEMCARD_DEFAULT_DAMAGED_SAVE_OK_CALLBACK		NULL

// PP: ptr to default function called when the disk-full message is OK'd
// PP:	(ie. player chooses to continue without saving)
#define MEMCARD_DEFAULT_FULL_DISK_OK_CALLBACK			NULL

// PP: ptr to function called when the damaged-save message is OK'd
// PP:	(ie. player chooses to continue without saving)
DamagedSaveOKCallback		MemCard_damagedSaveOKCallback;

// PP: ptr to function called when the disk-full message is OK'd
// PP:	(ie. player chooses to continue without saving)
FullDiskOKCallback			MemCard_fullDiskOKCallback;

#define REGION_E

#if BPLATFORM==XBOX

#define MCARDPREFIX "TazWanted"

#else

#ifdef REGION_A
	#define PRODUCTCODE "SLUS-20236"
	#define MCARDPREFIX "BA" PRODUCTCODE
#endif
#ifdef REGION_E
	#define PRODUCTCODE	"SLES-50649"
	#define MCARDPREFIX "BE" PRODUCTCODE
#endif

#endif

#if BPLATFORM == PC
	#define TAZFILENAME "TazWanted.sav"
	#define TAZBACKUPFILENAME "TazWanted.bak"
	#define TAZDISPLAYNAME "Taz:Wanted"
	#define MCARDFULLNAME TAZFILENAME
#elif(BPLATFORM == XBOX)
	#define TAZFILENAME		"TazWanted"		// PP: NOTE: THIS CANNOT BE AN EMPTY STRING! (Babel no like)
	#define TAZDISPLAYNAME	"Taz: Wanted"		
	#define MCARDFULLNAME	TAZFILENAME
#else
	#define TAZFILENAME "TAZWNT"
	#define TAZDISPLAYNAME "Taz:Wanted"
	#define MCARDFULLNAME MCARDPREFIX TAZFILENAME
#endif

#ifdef ALLOW_CONTROLLERS_IN_ANY_PORTS
#define MEMCARD_OKBUTTON						OKBUTTON_ANY_PAD
#define MEMCARD_CANCELBUTTON					CANCELBUTTON_ANY_PAD
#define MEMCARD_FORCE_OKBUTTON_DEBOUNCE			FORCE_OKBUTTON_DEBOUNCE_ALL_PADS
#define MEMCARD_FORCE_CANCELBUTTON_DEBOUNCE		FORCE_CANCELBUTTON_DEBOUNCE_ALL_PADS
#else// PP: ifndef ALLOW_CONTROLLERS_IN_ANY_PORTS
#define MEMCARD_OKBUTTON		OKBUTTON
#define MEMCARD_CANCELBUTTON	CANCELBUTTON
#define MEMCARD_FORCE_OKBUTTON_DEBOUNCE			FORCE_OKBUTTON_DEBOUNCE
#define MEMCARD_FORCE_CANCELBUTTON_DEBOUNCE		FORCE_CANCELBUTTON_DEBOUNCE
#endif// PP: ndef ALLOW_CONTROLLERS_IN_ANY_PORTS


// PP: define SILENT_NONSAVE to bypass ALL messages regarding game saves
// PP: once the player has opted to play without saving
#if(BPLATFORM == XBOX)
#define SILENT_NONSAVE
#endif// PP: platform

#ifdef XBOX_DAMAGED_SAVE_HANDLING// PP: MemCard.h
// PP: MemCard_corruptedSave is set true when corrupted saves are found on the storage medium
bool MemCard_corruptedSave=false;
#endif// PP: def XBOX_DAMAGED_SAVE_HANDLING

#ifdef XBOX_FULL_MEDIUM_HANDLING// PP: MemCard.h
// PP: MemCard_fullMedium is set true when the storage medium is found to have insufficient space to create a save
bool MemCard_fullMedium=false;
#endif// PP: def XBOX_DAMAGED_SAVE_HANDLING

#define GAMESAVESIZE	(150*1024)

// PP: ====== XBOX SPECIFICS ======

// PP: number of blocks needed for the Xbox Taz saved-game
#define XBOX_GAMESAVE_NUM_BLOCKS		14// PP: 21 blocks are needed in total, if the has successfully booted, only 14 more are needed for a save

// PP: drive on which the Xbox Taz game is saved
#define XBOX_GAMESAVE_DRIVE				'U'

// PP: ============================

ushort	mCardFullName16[128];
ushort	mCardPrefix16[64];

static EBGameSaveStatus status=BGAMESAVE_NOSTATE;

bool MemCard_busy=false;		// PP: whether or not there's a memory card operation in progress at the moment
char MemCard_bootUp=1;				// TP: memcard has not been used yet so perform bootup check


// PP: do we want debug messages showing what's going on with the load/save?
#ifdef TONY
	bool		MemCard_debug=false;
#elif defined PHIL
	bool		MemCard_debug=true;
#else
	bool		MemCard_debug=false;
#endif

// PP: Moved these here
static float	timeInState = 0.0f;				// PP: how long we've been in the current state
static float	timeInLastState = 0.0f;			// PP: how long we were in the last state

enum EMemCardState
{
	MEMCARD_NONE,
	MEMCARD_INSERTED,
	MEMCARD_PRESENT,
	MEMCARD_REMOVED,
};

enum EMemCardBootState
{
	MEMCARDBOOT_NOCARD,
	MEMCARDBOOT_DATAUNKNOWN,
	MEMCARDBOOT_NODATA,
	MEMCARDBOOT_EXISTINGDATA,
	MEMCARDBOOT_CORRUPTDATA,
	MEMCARDBOOT_FULL,
	MEMCARDBOOT_UNFORMATED,
};

static EMemCardBootState MemCard_bootStatus;

#define CARDOP_DPRINT(_status, _description)		{if(MemCard_debug) bkPrintf("%s (%s)\n", #_status, #_description);}
#define CHECKOPEN(_book) {if (!(_book.flags&(BKFLAG_OPEN))) _book.open();}

static  int					stateOverride = 0;
bool	MemCard_allowAccess = false;

#if(BPLATFORM == XBOX)
#define MIN_MESSAGE_DISPLAY	(3.0f)// PP: DO NOT TOUCH
#else// PP: platform
#define MIN_MESSAGE_DISPLAY	(3.0f)
#endif// PP: platform

//#define WAIT_FOR_LAST_MSG		if ((timeInLastState+timeInState) > MIN_MESSAGE_DISPLAY)
// TP: Can't do this anymore as the messages will not be in sync with the memory card actions, Sony will not approve
#if BPLATFORM == XBOX
#define WAIT_FOR_LAST_MSG		if ((timeInLastState+timeInState) > MIN_MESSAGE_DISPLAY)
#define WAIT_FOR_MIN_TIME		if (timeInState > MIN_MESSAGE_DISPLAY)
#define NOWAIT					{timeInState = MIN_MESSAGE_DISPLAY;}
#else// PP: not xbox
#define WAIT_FOR_LAST_MSG		if ((timeInLastState+timeInState) > MIN_MESSAGE_DISPLAY)
#define WAIT_FOR_MIN_TIME		if (timeInState > MIN_MESSAGE_DISPLAY)
#define NOWAIT					{timeInState = MIN_MESSAGE_DISPLAY;}
#endif// PP: not xbox


EMemCardState MemCard_changed = MEMCARD_PRESENT;	// TP: by default a memory card is assumed to be present
static	bool	saveOverwrite = false;



// PP: UPDATE FUNCTIONS for MEMORY CARD BOOK PAGES...

#define MCD_PAGEUPDATEFUNC(_name)	void MemCard_updateFunc_##_name(PAGE* const page, const int32 context)

//MCD_PAGEUPDATEFUNC(CHECKINGMEDIA);
MCD_PAGEUPDATEFUNC(7_0_1);

//MCD_PAGEUPDATEFUNC(NOMEDIA);
MCD_PAGEUPDATEFUNC(2_0_1);

// TP: no space on boot
MCD_PAGEUPDATEFUNC(2_0_2);

// TP: no media in slot 1, no on bootup
MCD_PAGEUPDATEFUNC(12_5_8_1);

MCD_PAGEUPDATEFUNC(4_0_3);

MCD_PAGEUPDATEFUNC(CREATESAVE);

//MCD_PAGEUPDATEFUNC(FORMATMEDIAYN);
MCD_PAGEUPDATEFUNC(3_0_1);

//MCD_PAGEUPDATEFUNC(FORMATINPROGRESS);
MCD_PAGEUPDATEFUNC(8_0_3);

//MCD_PAGEUPDATEFUNC(FORMATFAILED);
MCD_PAGEUPDATEFUNC(8_0_7);

// TP: format successful
MCD_PAGEUPDATEFUNC(8_0_6);

//MCD_PAGEUPDATEFUNC(MEDIAFULL);
MCD_PAGEUPDATEFUNC(5_0_3);

// TP: Overwrite previous data
MCD_PAGEUPDATEFUNC(5_0_8);

//MCD_PAGEUPDATEFUNC(SAVEINPROGRESS);
MCD_PAGEUPDATEFUNC(3_1_1);

//MCD_PAGEUPDATEFUNC(SAVEOK);
MCD_PAGEUPDATEFUNC(5_1_3);

//MCD_PAGEUPDATEFUNC(SAVEFAILED);
MCD_PAGEUPDATEFUNC(5_0_9);

// TP: card data is corrupt
MCD_PAGEUPDATEFUNC(7_0_5);

//MCD_PAGEUPDATEFUNC(LOADNOTFOUND);
MCD_PAGEUPDATEFUNC(4_0_1);

//MCD_PAGEUPDATEFUNC(LOADINPROGRESS);
MCD_PAGEUPDATEFUNC(4_0_7);

//MCD_PAGEUPDATEFUNC(LOADOK);
MCD_PAGEUPDATEFUNC(4_0_8);

//MCD_PAGEUPDATEFUNC(LOADFAILED);
MCD_PAGEUPDATEFUNC(4_0_5);

//MCD_PAGEUPDATEFUNC(CANCELED);
MCD_PAGEUPDATEFUNC(CANCELED);

//MCD_PAGEUPDATEFUNC(LOADFAILED);
MCD_PAGEUPDATEFUNC(11_0_1);


static EBGameSaveStatus MemCard_cardStatus = BGAMESAVE_NOSTATE;

void MemCardChangeCallback(char *name, char *parms, void *data, void *context);

MEMCARDBOOK					MemCard_book;

// PP: copies of what's being loaded/saved
static void*	MemCard_data;
static ushort*	MemCard_name;
static int		MemCard_size;
static int MemCard_operation;

#if((BPLATFORM == PS2)||(BPLATFORM == XBOX)||(BPLATFORM == PC))
bool	usingMemCard = true;							// TP: true/false if player has opted to use the memory card or it is not present
#else
bool	usingMemCard = false;							// TP: true/false if player has opted to use the memory card or it is not present
#endif

// PP: function that gets called when the current load/save has finished
static MemCardFinishFunc	MemCard_finishFunc=NULL;

static bool createSave;


// PP: these two are roughly 6:7 in a first class stamp...
#define NUM_STAMP_EDGES_X			12							// PP: number of notches along horizontal sides of the stamp
#define NUM_STAMP_EDGES_Y			14							// PP: number of notches along vertical sides of the stamp

#define MCD_MAXIMIZED_WIDTH			(NUM_STAMP_EDGES_X*0.058f/* PP: REMOUT: must now be a fraction of screen height		26.0f*/)	// PP: width of display when full
#define MCD_MAXIMIZED_HEIGHT		(NUM_STAMP_EDGES_Y*0.038f/* PP: REMOUT: must now be a fraction of screen height		26.0f*/)	// PP: height of display when full
#define MCD_MAXIMIZED_X				0.0f						// PP: centre x coord of display when full
#define MCD_MAXIMIZED_Y				0.0f						// PP: centre y coord of display when full

#define MCD_MINIMIZED_WIDTH			(NUM_STAMP_EDGES_X*0.029f/* PP: REMOUT: must now be a fraction of screen height		13.0f*/)	// PP: width of display when minimized
#define MCD_MINIMIZED_HEIGHT		(NUM_STAMP_EDGES_Y*0.029f/* PP: REMOUT: must now be a fraction of screen height		13.0f*/)	// PP: height of display when minimized
#define MCD_MINIMIZED_X				230							// PP: centre x coord of display when minimized
#define MCD_MINIMIZED_Y				150							// PP: centre y coord of display when minimized

#define MCD_DURATION_OK				1.2f						// PP: duration of 'OK' message, in seconds
#define MCD_DURATION_FADEOUT		0.5f						// PP: duration of fade out, in seconds
#define	MCD_DURATION_FADEIN			0.5f						// PP: duration of fade in, in seconds
#define MCD_DURATION_RESIZE			0.5f
#define MCD_DURATION_PAGEFADE		0.5f

#define STAMP_R						75							// PP: red value of stamp
#define STAMP_G						75							// PP: green value of stamp
#define STAMP_B						75							// PP: blue value of stamp

#define CARDICON_WIDTH				105.0f
#define CARDICON_HEIGHT				CARDICON_WIDTH
#define CARDICON_X					-155.0f
#define CARDICON_Y					-75.0f

#define MCD_X_TURNS					0.5f
#define MCD_Y_TURNS					0.5f
#define MCD_Z_TURNS					0.5f

#define MCD_MINIMIZED_SCALE			0.2f
#define MCD_MAXIMIZED_SCALE			0.8f

// PP: these two control the size of text on the pages...

// PP: ****** THIS HERE DEFINE CONTROLS THE SIZE OF MEMORY CARD DIALOGUE TEXT ******
#define MCD_PAGESCALE_MAXIMIZED		0.8f

// PP: this one just cancels out the scaling of the book to keep text the same size in minimized mode as in maximized mode
#define MCD_PAGESCALE_MINIMIZED		(MCD_PAGESCALE_MAXIMIZED*(MCD_MAXIMIZED_SCALE/(float)MCD_MINIMIZED_SCALE))

#define Z_SCALE						0.0f

#if BPLATFORM==PS2
#define MCD_MAX_ALPHA				128.0f
#else // PP: if BPLATFORM==PS2 else
#define MCD_MAX_ALPHA				128.0f
#endif // PP: BPLATFORM==PS2 (else)

TBGameSaveInfo 		mySaveGameInfo;



/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PRIVATE MAIN FUNCTIONS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: MemCard_getNumFreeMemoryUnits
	Purpose 	: get the number of free 'memory units' available to the game
	Parameters 	: 
	Returns 	: the number of free 'memory units' available to the game
	Info 		: eg., the number of free Kb on the PS2 memory card,
					or the number of free 'blocks' on the Xbox hard drive.
*/
static uint32 MemCard_getNumFreeMemoryUnits(void)
{
#if(BPLATFORM == XBOX)
	
	ULARGE_INTEGER	freeBytesAvailable;
	ULARGE_INTEGER	bytes;				// PP: just out of curiosity
	ULARGE_INTEGER	freeBytes;			// PP: just out of curiosity
	static char		directory[]			={XBOX_GAMESAVE_DRIVE, ':', '\0'};

	if(GetDiskFreeSpaceEx(directory, &freeBytesAvailable, &bytes, &freeBytes))
	{
		uint32	freeBlocksAvailable;
		uint32	blocks;					// PP: just out of curiosity
		uint32	freeBlocks;				// PP: just out of curiosity

#define BYTES_TO_XBOX_BLOCKS(_bytes)	uint32(__int64((_bytes).QuadPart)/16/1024);// PP: 'block'=16Kb, 'Kb'=1024 bytes

		freeBlocksAvailable=BYTES_TO_XBOX_BLOCKS(freeBytesAvailable);
		blocks=BYTES_TO_XBOX_BLOCKS(bytes);
		freeBlocks=BYTES_TO_XBOX_BLOCKS(freeBytes);

		//bkPrintf("MemCard_getNumFreeMemoryUnits: BYTES: total %d, free %d, freeAvailable %d\n", bytes.QuadPart, freeBytes.QuadPart, freeBytesAvailable.QuadPart);
		//bkPrintf("MemCard_getNumFreeMemoryUnits: BLOCKS: total %d, free %d, freeAvailable %d\n", blocks, freeBlocks, freeBlocksAvailable);

		return freeBlocksAvailable;
	}
	else
	{
		ASSERTM(0, "MemCard_getNumFreeMemoryUnits: GetDiskFreeSpaceEx failed"); 

		return 0;
	}

#else

	// PP: OTHER PLATFORMS NOT SUPPORTED
	return 0;

#endif// PP: platform
}


/*	--------------------------------------------------------------------------------
	Function 	: MemCard_getLoadSaveFlags
	Purpose 	: get flags to use for loading and saving
	Parameters 	: 
	Returns 	: flags to use for loading and saving
	Info 		: 
*/
static int MemCard_getLoadSaveFlags(void)
{
#if(BPLATFORM == XBOX)
	// PP: Do not automatically select the first memory card. When this is set the BGAMESAVE_SELECTYN state will be used.
	// PP: This is needed to allow us to select the hard-drive 'slot'.
	return BGAMESAVE_NOMEDIAAUTOSELECT;
#else// PP: platform
	return 0;
#endif// PP: platform
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PUBLIC MAIN FUNCTIONS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


// PP: REMOUT: OOD		#define MEMCARDBOOK_TBFLAGS							TBFLAG_DEFAULTS
// PP: REMOUT: OOD		#define MEMCARDBOOK_TEXTBOX_PARAMS					TEXTFX_DEFAULT_COLOUR1, TEXTFX_DEFAULT_COLOUR2, MEMCARDBOOK_TBFLAGS
#define NEW_MEMCARDBOOK_TEXTBOX(_str)				((new TEXTBOX(_str))->setWrap(true))
#define NEW_MEMCARDBOOK_TEXTBOX_NOWRAP(_str)		(new TEXTBOX(_str))

TBEventClient *memCardEvent;

/*	--------------------------------------------------------------------------------
	Function 	: MemCard_init
	Purpose 	: initialise memory card stuff
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/

void MemCard_init(void)
{
	PAGE*						page;

	// PP: standard colour used as a background for messages such as bad disk and missing pad, to subdue whatever is on screen at the time (might even be other text)
	extern COLOUR				standardMessageBackgroundTint;// PP: pause.cpp

	if (sizeof(gameStatus.globalSettings.saveInfo) > (MIN_MEMCARD_SAVESIZE))
	{
		ASSERTM(0, "***ERROR*** Save data is too large\n");
	}

	bkString8to16(mCardFullName16,MCARDFULLNAME);
	bkString8to16(mCardPrefix16,MCARDPREFIX);

	// TP: Clear the padded memory
	memset(&gameStatus.globalSettings.savePad[sizeof(SAVEINFO)], 0, (MIN_MEMCARD_SAVESIZE)-sizeof(SAVEINFO));

	// TP: Setup the save info for PS2// PP: Xbox setup done here as well
	MemCard_setupSaveInfo();

#if(BPLATFORM != XBOX)// PP: we don't use memory cards on Xbox; MemCardChangeCallback will crash on Xbox when you insert an MU
	memCardEvent = bkTrapEventCallback("_MemCardChange",MemCardChangeCallback,NULL);
#endif// PP: xbox
	
	// PP: add a full-screen tinting background to the book

	// TP: Changed this to a full black background as the semi-transparent blended with the splashscreen during a load, looked a little odd.
	// PP: REMOUT: TEMP TESTMemCard_book.setFullScreenBackground(COLOUR(0,0,0,255));
	MemCard_book.setFullScreenBackground(standardMessageBackgroundTint);

	MemCard_book.setRect(RECTANGLE(MCD_MAXIMIZED_X-(MCD_MAXIMIZED_WIDTH/2.0f), MCD_MAXIMIZED_X+(MCD_MAXIMIZED_WIDTH/2.0f), MCD_MAXIMIZED_Y-(MCD_MAXIMIZED_HEIGHT/2.0f), MCD_MAXIMIZED_Y+(MCD_MAXIMIZED_HEIGHT/2.0f)));

	// PP: use the Memory Card Pie chooser on the book
	MemCard_book.setEffectChooser(&memcardPIEChooser);

	// PP: give the book a name, for debugging
	MemCard_book.setName("MEMCARDBOOK");

	page = MemCard_book.addPage(NULL,"BLANK");

#if(BPLATFORM == PS2)// PP: Add PS2 memory card messages...

#define ADD_MEMCARD_PAGE(_name) \
	{ \
		page=MemCard_book.addPage(MemCard_updateFunc_##_name, #_name); \
		page->insertItem(NEW_MEMCARDBOOK_TEXTBOX(STR_MEMORYCARD_TRC_##_name)->setDrawScale(MCD_PAGESCALE_MAXIMIZED)); \
	}

	ADD_MEMCARD_PAGE(7_0_1);// TP: Checking media

	ADD_MEMCARD_PAGE(2_0_1);// TP: no media on boot up
	page->insertItem(NEW_MEMCARDBOOK_TEXTBOX_NOWRAP(STR_DIALOGUE_CROSSOK))->setDrawScale(MCD_PAGESCALE_MAXIMIZED);

	ADD_MEMCARD_PAGE(3_0_1);// TP: format y/n
	page->insertItem(NEW_MEMCARDBOOK_TEXTBOX(STR_MEMCARD_RETRYFORMATCANCEL))->setDrawScale(MCD_PAGESCALE_MAXIMIZED);

	ADD_MEMCARD_PAGE(8_0_3);// TP: format in progress

	ADD_MEMCARD_PAGE(8_0_7);// TP: format failed
	page->insertItem(NEW_MEMCARDBOOK_TEXTBOX_NOWRAP(STR_MEMCARD_RETRYCANCEL))->setDrawScale(MCD_PAGESCALE_MAXIMIZED);

	ADD_MEMCARD_PAGE(8_0_6);// TP: format succeded
	page->insertItem(NEW_MEMCARDBOOK_TEXTBOX_NOWRAP(STR_DIALOGUE_CROSSOK))->setDrawScale(MCD_PAGESCALE_MAXIMIZED);

	ADD_MEMCARD_PAGE(5_0_3);// TP: media full
	page->insertItem(NEW_MEMCARDBOOK_TEXTBOX_NOWRAP(STR_MEMCARD_RETRYCANCEL))->setDrawScale(MCD_PAGESCALE_MAXIMIZED);

	ADD_MEMCARD_PAGE(3_1_1);// TP: save in progress

	ADD_MEMCARD_PAGE(5_1_3);// TP: save ok

	ADD_MEMCARD_PAGE(5_0_8);// TP: overwrite previous
	page->insertItem(NEW_MEMCARDBOOK_TEXTBOX_NOWRAP(STR_DIALOGUE_OKCANCEL))->setDrawScale(MCD_PAGESCALE_MAXIMIZED);

	ADD_MEMCARD_PAGE(5_0_9);// TP: save failed
	page->insertItem(NEW_MEMCARDBOOK_TEXTBOX_NOWRAP(STR_MEMCARD_RETRYCANCEL))->setDrawScale(MCD_PAGESCALE_MAXIMIZED);

	ADD_MEMCARD_PAGE(4_0_1);// TP: load not found STR_MEMCARD_RETRYCANCEL
	page->insertItem(NEW_MEMCARDBOOK_TEXTBOX_NOWRAP(STR_MEMCARD_RETRYCONTINUE))->setDrawScale(MCD_PAGESCALE_MAXIMIZED);

	ADD_MEMCARD_PAGE(4_0_3);// TP: attempt load but no media, NOT on boot up
	page->insertItem(NEW_MEMCARDBOOK_TEXTBOX_NOWRAP(STR_MEMCARD_RETRYCANCEL))->setDrawScale(MCD_PAGESCALE_MAXIMIZED);

	ADD_MEMCARD_PAGE(12_5_8_1);// TP: attempt load but no media, on boot up
	page->insertItem(NEW_MEMCARDBOOK_TEXTBOX_NOWRAP(STR_TONYS_RETRY_STARTGAME_STRING))->setDrawScale(MCD_PAGESCALE_MAXIMIZED);

	ADD_MEMCARD_PAGE(4_0_7);// TP: load in progress

	ADD_MEMCARD_PAGE(4_0_8);// TP: load ok

	ADD_MEMCARD_PAGE(4_0_5);// TP: load failed
	page->insertItem(NEW_MEMCARDBOOK_TEXTBOX_NOWRAP(STR_MEMCARD_RETRYCANCEL))->setDrawScale(MCD_PAGESCALE_MAXIMIZED);

	ADD_MEMCARD_PAGE(7_0_5);// TP: card data is corrupt
	page->insertItem(NEW_MEMCARDBOOK_TEXTBOX_NOWRAP(STR_MEMCARD_RETRYCANCEL))->setDrawScale(MCD_PAGESCALE_MAXIMIZED);

	page=MemCard_book.addPage(MemCard_updateFunc_CREATESAVE, "CREATESAVE");
	page->insertItem(NEW_MEMCARDBOOK_TEXTBOX(STR_MEMORY_CARD_TRC_CREATESAVE))->setDrawScale(MCD_PAGESCALE_MAXIMIZED);
	page->insertItem(NEW_MEMCARDBOOK_TEXTBOX_NOWRAP(STR_DIALOGUE_OKCANCEL))->setDrawScale(MCD_PAGESCALE_MAXIMIZED);

	ADD_MEMCARD_PAGE(2_0_2);
	page->insertItem(NEW_MEMCARDBOOK_TEXTBOX_NOWRAP(STR_MEMCARD_RETRYCANCEL))->setDrawScale(MCD_PAGESCALE_MAXIMIZED);

	ADD_MEMCARD_PAGE(11_0_1);

#elif(BPLATFORM == XBOX)// PP: Add Xbox memory card messages...

#define ADD_MEMCARD_PAGE(_name) \
	{ \
		page=MemCard_book.addPage(MemCard_XboxUpdateFunc_##_name, #_name); \
		page->insertItem(STR_XBOX_TCR_##_name) \
			->setWrap(true) \
			->setFontSize(MCD_PAGESCALE_MAXIMIZED); \
	}
/*
	// PP: 1.4-6-1		Surprise Xbox MU Removal
	ADD_MEMCARD_PAGE(SAVE_FAIL_REMOVE);// PP: dismiss a/start
	ADD_MEMCARD_PAGE(LOAD_FAIL_REMOVE);// PP: dismiss a/start

	// PP: 1.4-6-2		Xbox MU Write Warning
	ADD_MEMCARD_PAGE(SAVE);// PP: dismiss MAX(save duration, 3 seconds)

	// PP: 1.4-6-3		Xbox MU Read Warning
	ADD_MEMCARD_PAGE(LOAD);// PP: dismiss MAX(load duration, 3 seconds)



	// PP: ~~~~~~~~

	ADD_MEMCARD_PAGE(XBOX_TCR_MU_LOAD_FAIL);

	ADD_MEMCARD_PAGE(XBOX_TCR_MU_SAVE_DELETE);

	ADD_MEMCARD_PAGE(XBOX_TCR_MU_BLOCK_COUNT);

	ADD_MEMCARD_PAGE(XBOX_TCR_MU_OVERWRITE);

	ADD_MEMCARD_PAGE(XBOX_TCR_MU_ERASED_FAIL);

	ADD_MEMCARD_PAGE(XBOX_TCR_MU_DAMAGED);

	ADD_MEMCARD_PAGE(XBOX_TCR_MU_INSERT);*/
	// PP: 1.4_6_4		Hard Disk Write Warning
	ADD_MEMCARD_PAGE(HARDDISK_SAVE);// PP: dismissal MAX(3 seconds, save duration)

	// PP: ~~~~~~~~~

	// PP: 1.4-6-5		Content Protection Fault
	// PP: ADD_MEMCARD_PAGE(LOAD_FAIL);// PP: dismissal A/START
	ADD_MEMCARD_PAGE(DAMAGED_GAME);

	// PP: also gotter add options to go to dashboard or to continue without saving...

	page->insertItem(STR_XBOX_TCR_B_DASHBOARD)
		->setFontSize(MCD_PAGESCALE_MAXIMIZED)
		->setYAlign(PIFLAG_YALIGN_BOTTOM);

	page->insertItem(STR_XBOX_TCR_A_CONTINUE)
		->setFontSize(MCD_PAGESCALE_MAXIMIZED)
		->setYPadding(false);

	// PP: ~~~~~~~~~

	// PP: 1.4-6-8		Low Storage
	ADD_MEMCARD_PAGE(HARDDISK_FULL);// PP: dismissal A/START, B launches dashboard to make space

	// PP: also gotter add blocks-free and blocks-short...

	page->insertItem(STR_XBOX_TCR_BLOCKS_FREE)
		->setFontSize(MCD_PAGESCALE_MAXIMIZED)
		->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setName("blocks free");

	page->insertItem(STR_XBOX_TCR_BLOCKS_SHORT)
		->setFontSize(MCD_PAGESCALE_MAXIMIZED)
		->setYPadding(false)
		->setName("blocks short");

	// PP: ...and set the 'open' PageFunc that'll update those numbers
	page->openFunc=MemCard_XboxOpenFunc_HARDDISK_FULL;

	// PP: ~~~~~~~~~

#endif// PP: platform

	// PP: canceled page
	page=MemCard_book.addPage(MemCard_updateFunc_CANCELED, "CANCELED");
	page->insertItem(NEW_MEMCARDBOOK_TEXTBOX(STR_MEMCARD_CANCELED)->setDrawScale(MCD_PAGESCALE_MAXIMIZED));

	page=MemCard_book.addPage("MISSING CONTROLLER");
	page->insertItem(NEW_MEMCARDBOOK_TEXTBOX(STR_PS2_CONTROLLER1_REMOVAL)->setDrawScale(MCD_PAGESCALE_MAXIMIZED));

//	ADD_MEMCARD_PAGE(CANCELED);
}

#if (BPLATFORM!=XBOX)
/*	--------------------------------------------------------------------------------
	Function 	: MemCard_updatePS2
	Purpose 	: update PS2 memory card operations
	Parameters 	: current save status (EBGameSaveStatus BGAMESAVE_...)
	Returns 	: 
	Info 		: // PP: 
*/
static void MemCard_updatePS2(const EBGameSaveStatus status)
{
	switch(status)
	{
	case BGAMESAVE_NOSTATE:
		{
			// the process is in no state at the moment
			// PP: CARDOP_DPRINT(BGAMESAVE_NOSTATE, the process is in no state at the moment);
			MemCard_busy = false;
			// TP: MemCard_book.close();		
		}
		break;
	case BGAMESAVE_CHECKINGMEDIA:// PP: min
		{
			usingMemCard = true;

			CHECKOPEN(MemCard_book);

			// the process is checking the card
			//WAIT_FOR_LAST_MSG
			{
				CARDOP_DPRINT(BGAMESAVE_CHECKINGMEDIA, the process is checking the card);

				MemCard_book.gotoPage("7_0_1");
			}

			MemCard_book.maximize();// TP: minimize
		}
		break;
	case BGAMESAVE_NOMEDIA:// PP: full, offer retry
		{
			MemCard_changed = MEMCARD_REMOVED;
			if (usingMemCard)	// TP: dont worry if not using memcard
			{
				CHECKOPEN(MemCard_book);

				// there is no card present
				WAIT_FOR_LAST_MSG
				{
					if(MemCard_bootUp)
					{
						MemCard_bootStatus = MEMCARDBOOT_NOCARD;
						if (MemCard_bootUp==1)
						{
							CARDOP_DPRINT(BGAMESAVE_NOMEDIA, there is no card present on boot up);
	
							MemCard_book.gotoPage("12_5_8_1");
						}
						else
						{
							CARDOP_DPRINT(BGAMESAVE_NOMEDIA, there is no card present);

							MemCard_book.gotoPage("2_0_1");
						}
					}
					else
					{
						CARDOP_DPRINT(BGAMESAVE_NOMEDIA, there is no card present);

						MemCard_book.gotoPage("4_0_3");
					}
				}

				MemCard_book.maximize();
			}
		}
		break;
	case BGAMESAVE_INVALIDMEDIA:
		if (usingMemCard)	// TP: dont worry if not using memcard
		{
			CHECKOPEN(MemCard_book);

			if (MemCard_bootUp)
			{
				MemCard_bootStatus = MEMCARDBOOT_NOCARD;
			}

			// there is no card present
			WAIT_FOR_LAST_MSG
			{
				CARDOP_DPRINT(BGAMESAVE_NOMEDIA, there is an invalid card present);

				MemCard_book.gotoPage("4_0_3");
			}

			MemCard_book.maximize();
		}
		break;

	case BGAMESAVE_DAMAGEDSAVE:
		// TP: Card is corrupt, this could occur during any checking of the card
		if (MemCard_bootUp)
		{
			MemCard_bootStatus = MEMCARDBOOT_CORRUPTDATA;
		}
		WAIT_FOR_LAST_MSG
		{
			CARDOP_DPRINT(BGAMESAVE_DAMAGEDSAVE, the card contains corrupt data offer retry cancel);

			MemCard_book.gotoPage("7_0_5");
		}

		MemCard_book.maximize();
		break;

	case BGAMESAVE_FORMATMEDIAYN:
		{
			if (MemCard_bootUp)
			{
				MemCard_bootStatus = MEMCARDBOOT_UNFORMATED;
				biGameSaveSetStatus(BGAMESAVE_CANCELRESPONSE,mCardFullName16,0,&mySaveGameInfo);
				biGameSaveEnd();
				MemCard_busy = false;
				MemCard_book.close();		
				break;
			}

			// the card is unformatted, request permission from user to format it...
			WAIT_FOR_LAST_MSG
			{
				CARDOP_DPRINT(BGAMESAVE_FORMATMEDIAYN, the card is unformatted - request permission from user to format it);

				MemCard_book.gotoPage("3_0_1");
			}

			MemCard_book.maximize();
		}
		break;

	case BGAMESAVE_FORMATINPROGRESS:// PP: min
		{
			// the card is being formatted
			//WAIT_FOR_LAST_MSG
			{
				CARDOP_DPRINT(BGAMESAVE_FORMATINPROGRESS, the card is being formatted);

				MemCard_book.gotoPage("8_0_3");
			}

			MemCard_book.maximize();
		}
		break;
	case BGAMESAVE_FORMATFAILED:// PP: full, offer retry
		{
			// the format failed
			//WAIT_FOR_LAST_MSG
			{
				CARDOP_DPRINT(BGAMESAVE_FORMATFAILED, the format failed);

				MemCard_book.gotoPage("8_0_7");

				NOWAIT
			}

			MemCard_book.maximize();
		}
		break;
	case BGAMESAVE_FORMATSUCCESSFUL:
		{
			// the format failed
			//WAIT_FOR_LAST_MSG
			{
				CARDOP_DPRINT(BGAMESAVE_FORMATSUCCESSFUL, the format succeded);

				MemCard_book.gotoPage("8_0_6");

				NOWAIT
			}

			MemCard_book.maximize();
		}
		break;
	case BGAMESAVE_MEDIAFULL:// PP: full, offer retry
		{
			if (usingMemCard)	// TP: dont worry if not using memcard
			{
				CHECKOPEN(MemCard_book);
				// the card is full
				WAIT_FOR_LAST_MSG
				{
					CARDOP_DPRINT(BGAMESAVE_MEDIAFULL, the card is full);

					if (MemCard_bootUp)
					{
						MemCard_bootStatus = MEMCARDBOOT_FULL;

						MemCard_book.gotoPage("2_0_2");
					}
					else
					{
						MemCard_book.gotoPage("5_0_3");
					}

					NOWAIT
				}

				MemCard_book.maximize();
			}
		}
		break;
	case BGAMESAVE_SAVEYN:// PP: skipped
		{
			// TP: if doing boot check
			if (MemCard_bootUp)
			{
				MemCard_bootStatus = MEMCARDBOOT_DATAUNKNOWN;

				biGameSaveSetStatus(BGAMESAVE_CANCELRESPONSE,mCardFullName16,0,&mySaveGameInfo);
				biGameSaveEnd();
				break;
			}

			if (saveOverwrite)
			{
				CHECKOPEN(MemCard_book);
				// request permission from user to continue with save
				CARDOP_DPRINT(BGAMESAVE_SAVEYN, request permission from user to continue with save);

				MemCard_book.maximize();

				MemCard_book.gotoPage("5_0_8");
				if (MEMCARD_OKBUTTON)
				{
					biGameSaveSetStatus(BGAMESAVE_OKRESPONSE,mCardFullName16,0,&mySaveGameInfo);
				}
				else
				{
					if (MEMCARD_CANCELBUTTON)
					{
						biGameSaveSetStatus(BGAMESAVE_CANCELRESPONSE,mCardFullName16,0,&mySaveGameInfo);
					}
					else
					{
						biGameSaveSetStatus(BGAMESAVE_NORESPONSE,mCardFullName16,0,&mySaveGameInfo);
					}
				}
			}
			else
			{
				if (!MemCard_busy)
				{
					// TP: not actually trying to do anything
					biGameSaveSetStatus(BGAMESAVE_CANCELRESPONSE,mCardFullName16,0,&mySaveGameInfo);
					break;
				}
				// request permission from user to continue with save
				CARDOP_DPRINT(BGAMESAVE_SAVEYN, request permission from user to continue with save);

				MemCard_book.maximize();

				// PP: TEMP?: JUST SAY YES KIDS
				biGameSaveSetStatus(BGAMESAVE_OKRESPONSE,mCardFullName16,0,&mySaveGameInfo);
			}
		}
		break;
	case BGAMESAVE_SAVEINPROGRESS:// PP: min
		{
			saveOverwrite = false;
			// saving is in progress
			CARDOP_DPRINT(BGAMESAVE_SAVEINPROGRESS, saving is in progress);

			MemCard_book.gotoPage("3_1_1");

			MemCard_book.maximize();
		}
		break;
	case BGAMESAVE_SAVEOK:// PP: min, then it fades off
		{
			// the save process completed successfully
			CARDOP_DPRINT(BGAMESAVE_SAVEOK, the save process completed successfully);

			//WAIT_FOR_LAST_MSG
			{
				MemCard_book.gotoPage("5_1_3");
				MemCard_book.maximize();
				WAIT_FOR_MIN_TIME
				{
					MemCard_book.ok();

					biGameSaveEnd();
					if(MemCard_finishFunc != NULL) MemCard_finishFunc(true);
					NOWAIT
				}
			}
		}
		break;
	case BGAMESAVE_SAVEFAILED:// PP: full, offer retry
		{
			// the save process failed
			//WAIT_FOR_LAST_MSG
			{
				CARDOP_DPRINT(BGAMESAVE_SAVEFAILED, the save process failed);

				MemCard_book.gotoPage("5_0_9");

				NOWAIT
			}

			MemCard_book.maximize();
		}
		break;
	case BGAMESAVE_SAVECANCELED:// PP: I won't support this
		{
			// the save process was canceled
			WAIT_FOR_LAST_MSG
			{
				CARDOP_DPRINT(BGAMESAVE_SAVECANCELED, the save process was canceled);

				biGameSaveEnd();
				if (MemCard_bootUp==0)
				{
					usingMemCard = false;
				}
				if(MemCard_finishFunc != NULL) MemCard_finishFunc(false);
				MemCard_book.close();

				NOWAIT
			}
		}
		break;
	case BGAMESAVE_LOADYN:// PP: skipped
		{
			if (MemCard_bootUp)
			{
				MemCard_bootStatus = MEMCARDBOOT_EXISTINGDATA;
				biGameSaveSetStatus(BGAMESAVE_CANCELRESPONSE,mCardFullName16,0,&mySaveGameInfo);
				biGameSaveEnd();
				break;
			}
			WAIT_FOR_LAST_MSG
			{
				if (!MemCard_busy)
				{
					// TP: not actually trying to do anything
					biGameSaveSetStatus(BGAMESAVE_CANCELRESPONSE,mCardFullName16,-1,&mySaveGameInfo);
					break;
				}
				// request permission from the user to continue with the load
				CARDOP_DPRINT(BGAMESAVE_LOADYN, request permission from the user to continue with the load);

				// PP: TEMP?: JUST SAY YES KIDS
				biGameSaveSetStatus(BGAMESAVE_OKRESPONSE,mCardFullName16,-1,&mySaveGameInfo);
			}
		}
		break;
	case BGAMESAVE_LOADNOTFOUND:// TP: No taz saves found, should save current state
		{
			if (MemCard_bootUp)
			{
				MemCard_bootStatus = MEMCARDBOOT_NODATA;
				biGameSaveSetStatus(BGAMESAVE_CANCELRESPONSE,mCardFullName16,0,&mySaveGameInfo);
				biGameSaveEnd();
				break;
			}
			// a saved game was not found
			if (createSave==false)
			{
				CARDOP_DPRINT(BGAMESAVE_LOADNOTFOUND, a saved game was not found);

				MemCard_book.gotoPage("4_0_1");

				MemCard_book.maximize();
			}
			else
			{
				// TP: on boot up with a memory card but no save data
				CARDOP_DPRINT(BGAMESAVE_LOADNOTFOUND, a saved game was not found);

				MemCard_book.gotoPage("CREATESAVE");

				MemCard_book.maximize();
			}
		}
		break;
	case BGAMESAVE_LOADINPROGRESS:// PP: min
		{
			// loading is in progress
			CARDOP_DPRINT(BGAMESAVE_LOADINPROGRESS, loading is in progress);

			MemCard_book.gotoPage("4_0_7");

			MemCard_book.maximize();//minimize();
		}
		break;
	case BGAMESAVE_LOADOK:// PP: min, then fade off
		{
			//WAIT_FOR_LAST_MSG
			{
				BOOL	loadok=TRUE;
				biGameSaveEnd();
				if(MemCard_finishFunc != NULL) 
				{

					loadok = MemCard_finishFunc(true);
				}

				if (loadok)
				{
					// the load process completed successfully
					CARDOP_DPRINT(BGAMESAVE_LOADOK, the load process completed successfully);

					MemCard_book.gotoPage("4_0_8");

					MemCard_book.maximize();
					MemCard_book.ok();
				}
				else
				{
					stateOverride = BGAMESAVE_LOADFAILED;
					CARDOP_DPRINT(BGAMESAVE_LOADFAILED, the load process failed);

					MemCard_book.gotoPage("4_0_5");

					MemCard_book.maximize();
				}
			}
		}
		break;
	case BGAMESAVE_LOADFAILED:// PP: full, offer retry
		{
			// the load process failed
			//WAIT_FOR_LAST_MSG
			{
				CARDOP_DPRINT(BGAMESAVE_LOADFAILED, the load process failed);

				MemCard_book.gotoPage("4_0_5");

				NOWAIT
			}

			MemCard_book.maximize();
		}
		break;
	case BGAMESAVE_LOADCANCELED:// PP: I won't support this
		{
			// the load process was canceled
			CARDOP_DPRINT(BGAMESAVE_LOADCANCELED, the load process was canceled);

			biGameSaveEnd();

			MemCard_book.close();

		}
		break;
	}// PP: end switch
}
#endif// PP: (BPLATFORM!=XBOX)

/*	--------------------------------------------------------------------------------
	Function 	: MemCard_updateXbox
	Purpose 	: update Xbox memory card operations
	Parameters 	: current save status (EBGameSaveStatus BGAMESAVE_...)
	Returns 	: 
	Info 		: // PP: 
*/
static void MemCard_updateXbox(const EBGameSaveStatus status)
{
	switch(status)
	{
	case BGAMESAVE_NOSTATE:
		{
			// the process is in no state at the moment
			// PP: CARDOP_DPRINT(BGAMESAVE_NOSTATE, the process is in no state at the moment);
			MemCard_busy = false;
		}
		break;
	case BGAMESAVE_CHECKINGMEDIA:
		{
			// the process is checking the media

			// PP: set save 'slot' to be the hard drive
			if(biGameSaveSetActiveCardSlot(BGAMESAVE_CARDSLOT_HD) == OK)
			{
				bkPrintf("MemCard_updateXbox:BGAMESAVE_CHECKINGMEDIA: setting hard drive as active slot\n");
			}
			else
			{
				bkPrintf("problem\n");
			}

			usingMemCard = true;
		}
		break;
	case BGAMESAVE_NOMEDIA:
		// there is no media present
		{
			// PP: retry: start the load/save again


			// PP: set save 'slot' to be the hard drive
			if(biGameSaveSetActiveCardSlot(BGAMESAVE_CARDSLOT_HD) == OK)
			{
				bkPrintf("MemCard_updateXbox:BGAMESAVE_NOMEDIA: setting hard drive as active slot\n");
			}
			else
			{
				bkPrintf("problem\n");
			}
//			biGameSaveEnd();
//			MemCard_busy = false;
			if(MemCard_operation == BGAMESAVE_LOAD)
			{
				MemCard_load(MemCard_data, MemCard_name, MemCard_size, MemCard_finishFunc);
			}
			else
			{
				MemCard_save(MemCard_data, MemCard_name, MemCard_size, MemCard_finishFunc);
			}
		}
		break;
	case BGAMESAVE_SELECTYN:
		{
			// there is media present, ask the user if they wish to select this for use ?
			// (this will only occur if the flag passed to biGameSaveStart contains BGAMESAVE_NOMEDIAAUTOSELECT)
		
			// PP: set save 'slot' to be the hard drive
			if(biGameSaveSetActiveCardSlot(BGAMESAVE_CARDSLOT_HD) == OK)
			{
				bkPrintf("MemCard_updateXbox:BGAMESAVE_SELECTYN: setting hard drive as active slot\n");
			}
			else
			{
				bkPrintf("PROBLEM\n");
			}

			CARDOP_DPRINT(BGAMESAVE_SELECTYN, there is media present - ask the user if they wish to select this for use ?);

			// PP: Ar, 'tis a fine medium...  I'LL TAKE HER!
			biGameSaveSetStatus(BGAMESAVE_OKRESPONSE,mCardFullName16,-1,&mySaveGameInfo);
		}
		break;
	case BGAMESAVE_READINGFILESYSTEM:
		// the process is checking the file system on the selected media
		break;
	case BGAMESAVE_UNFORMATED:
		// the media is unformatted , this OS is unable to format it (Xbox!) so prompt the user to enter a new card and hit 'y'.
		break;
	case BGAMESAVE_BROKENFORMATMEDIAYN:
		// the media is a broken state, request permission from user to format it
		{
		//	bkPrintf("BGAMESAVE_BROKENFORMATMEDIAYN\n");
		}
		break;
	case BGAMESAVE_FORMATMEDIAYN:
		// the media is unformatted, request permission from user to format it
		{
		//	bkPrintf("BGAMESAVE_FORMATMEDIAYN\n");
		}
		break;
	case BGAMESAVE_FORMATINPROGRESS:
		// the media is being formatted
		break;
	case BGAMESAVE_FORMATFAILED:
		// the format failed
		break;
	case BGAMESAVE_SAVEFAILED:
		{
			CARDOP_DPRINT(BGAMESAVE_SAVEFAILED, PP: save failed; assuming BGAMESAVE_MEDIAFULL...);
		}
	case BGAMESAVE_MEDIAFULL:
		{
			CARDOP_DPRINT(BGAMESAVE_MEDIAFULL, the media is full);

			// the media is full - ask if user wishes to manage the media (this is currently a reboot on the GameCube)

			// PP: reset ptr to function called when the disk-full message is OK'd
			// PP:	(ie. player chooses to continue without saving)
			MemCard_fullDiskOKCallback=MEMCARD_DEFAULT_FULL_DISK_OK_CALLBACK;

			// PP: disk is full
#ifdef XBOX_FULL_MEDIUM_HANDLING
			MemCard_fullMedium=true;
#endif

			// PP: open the disk-full message
			MemCard_book.open("HARDDISK_FULL");
		}
		break;
/* PP: REMOUT: TEMP TEST	case BGAMESAVE_MEDIAFULL_MANAGEYN:
		{
			CARDOP_DPRINT(BGAMESAVE_MEDIAFULL_MANAGEYN, the media is full - ask if user wishes to manage the media (this is currently a reboot on the GameCube));

			// the media is full - ask if user wishes to manage the media (this is currently a reboot on the GameCube)
			MemCard_book.gotoPage("HARDDISK_FULL");
		}
		break;*/
	case BGAMESAVE_SAVEYN:
		{
			// request permission from user to continue with save
			// directory entries are valid now, so we can check for saved games already on the card

			// request permission from user to continue with save
			CARDOP_DPRINT(BGAMESAVE_SAVEYN, request permission from user to continue with save);

			// PP: Yes please Babe's - if ye could
			biGameSaveSetStatus(BGAMESAVE_OKRESPONSE,mCardFullName16,0,&mySaveGameInfo);// PP: 0: overwrite current save game
		}
		break;
	case BGAMESAVE_SAVEINPROGRESS:
		{
			// saving is in progress
			MemCard_book.open("HARDDISK_SAVE");
		}
		break;
	case BGAMESAVE_SAVEOK:
		{
			// the save process completed successfully

			CARDOP_DPRINT(BGAMESAVE_SAVEOK, the save process completed successfully);

			WAIT_FOR_LAST_MSG
			{
//				MemCard_book.gotoPage("5_1_3");
//				MemCard_book.maximize();
			// PP: REMOUT: TEMP TEST	WAIT_FOR_MIN_TIME
				{
					MemCard_book.ok();

					biGameSaveEnd();
					if(MemCard_finishFunc != NULL) MemCard_finishFunc(true);
					NOWAIT
				}
			}
			else
			{
				timeInLastState += timeInState;
				timeInState = 0.0f;
			}
/* PP: REMOUT: 
			// PP: NEW TEST
			MemCard_book.close();

			// PP: NEW TEST
			MemCard_allowAccess = false;*/
		}
		break;
/* PP: REMOUT: new test - trying to get hd full messages workin	case BGAMESAVE_SAVEFAILED:
		{
			// the save process failed


			WAIT_FOR_LAST_MSG
			{
				CARDOP_DPRINT(BGAMESAVE_SAVEFAILED, the save process failed);

				// PP: No error message [yet]

				// PP: cancel: close the book
				biGameSaveEnd();
				if(MemCard_finishFunc != NULL) MemCard_finishFunc(false);
				MemCard_book.gotoPage("CANCELED");
				MemCard_book.ok();
			}
		}
		break;*/
	case BGAMESAVE_SAVECANCELED:
		{
			// the save process was canceled
			WAIT_FOR_LAST_MSG
			{
				CARDOP_DPRINT(BGAMESAVE_SAVECANCELED, the save process was canceled);

				biGameSaveEnd();

				MemCard_book.close();

				NOWAIT
			}
		}
		break;
	case BGAMESAVE_LOADYN:
		{
			// request permission from the user to continue with the load
			// directory entries are valid now, so we can check for saved games already on the card

			CARDOP_DPRINT(BGAMESAVE_LOADYN, request permission from the user to continue with the load);

			// PP: Yes please Babe's - if ye could
			biGameSaveSetStatus(BGAMESAVE_OKRESPONSE,mCardFullName16,-1,&mySaveGameInfo);
		}
		break;
	case BGAMESAVE_LOADNOTFOUND:
		{

			// PP: cancel: close the book
			biGameSaveEnd();
			if(MemCard_finishFunc != NULL) MemCard_finishFunc(false);
			stateOverride = 0;

			// a saved game was not found
			CARDOP_DPRINT(BGAMESAVE_LOADNOTFOUND, a saved game was not found);
		}
		break;
	case BGAMESAVE_LOADINPROGRESS:
		// loading is in progress
		break;
	case BGAMESAVE_LOADOK:
		{
			// the load process completed successfully

			//WAIT_FOR_LAST_MSG
			{
				BOOL	loadok=TRUE;
				biGameSaveEnd();
				if(MemCard_finishFunc != NULL) 
				{

					loadok = MemCard_finishFunc(true);
				}

				if (loadok)
				{
					// the load process completed successfully
					CARDOP_DPRINT(BGAMESAVE_LOADOK, the load process completed successfully);

				//	MemCard_book.gotoPage("4_0_8");

				//	MemCard_book.maximize();
					MemCard_book.close();
				}
				else
				{
					stateOverride = BGAMESAVE_LOADFAILED;
					CARDOP_DPRINT(BGAMESAVE_LOADFAILED, the load process failed);

#ifdef XBOX_DAMAGED_SAVE_HANDLING
					// PP: the save is corrupted
					MemCard_corruptedSave=true;
#endif

					// PP: reset ptr to function called when the damaged-save message is OK'd
					// PP:	(ie. player chooses to continue without saving)
					MemCard_damagedSaveOKCallback=MEMCARD_DEFAULT_DAMAGED_SAVE_OK_CALLBACK;

					// PP: open the damaged game message
					MemCard_book.open("DAMAGED_GAME");

					NOWAIT

				//	MemCard_book.maximize();
				}
			}
		}
		break;
	case BGAMESAVE_LOADFAILED:// the load process failed
		{
			//WAIT_FOR_LAST_MSG
			{
#ifdef XBOX_DAMAGED_SAVE_HANDLING
				// PP: the save is corrupted
				MemCard_corruptedSave=true;
#endif

				// PP: reset ptr to function called when the damaged-save message is OK'd
				// PP:	(ie. player chooses to continue without saving)
				MemCard_damagedSaveOKCallback=MEMCARD_DEFAULT_DAMAGED_SAVE_OK_CALLBACK;

				// PP: open the damaged game message
				MemCard_book.open("DAMAGED_GAME");

				NOWAIT
			}

		}
		break;
	case BGAMESAVE_LOADFAILEDCRC:// the load process failed with the crc check - i.e. the data is corrupt
		{
			//WAIT_FOR_LAST_MSG
			{
#ifdef XBOX_DAMAGED_SAVE_HANDLING
				// PP: the save is corrupted
				MemCard_corruptedSave=true;
#endif

				// PP: reset ptr to function called when the damaged-save message is OK'd
				// PP:	(ie. player chooses to continue without saving)
				MemCard_damagedSaveOKCallback=MEMCARD_DEFAULT_DAMAGED_SAVE_OK_CALLBACK;

				// PP: open the damaged game message
				MemCard_book.open("DAMAGED_GAME");

				NOWAIT
			}
		}
		break;
	case BGAMESAVE_LOADCANCELED:
		// the load process was canceled
		break;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: MemCard_update
	Purpose 	: check up on and update the memory card load or save
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/

#if(BPLATFORM == XBOX)
#define MIN_MESSAGE_DISPLAY	(3.0f)// PP: DO NOT TOUCH
#else// PP: platform
#define MIN_MESSAGE_DISPLAY	(3.0f)
#endif// PP: platform

static EBGameSaveStatus	globalStatus;

void MemCard_update(void)
{
	static	TBGameSaveDirEntry	dirEntries;
	static  int					numFound;
	static	EBGameSaveStatus	lastStatus = BGAMESAVE_NOSTATE;

#if BPLATFORM==GAMECUBE
	return;
#endif

	// TP: Do we currently allow memcard operations
	if (!MemCard_allowAccess) 
	{
		globalStatus = biGameSaveGetStatus(&dirEntries,1,&numFound,&mySaveGameInfo);
// PP: REMOUT: Now handled by the Book List			MemCard_book.update();
		return;
	}

	// get current status of load/save
	lastStatus = status;
	globalStatus = status = biGameSaveGetStatus(&dirEntries,1,&numFound,&mySaveGameInfo);

	if (stateOverride) status = (EBGameSaveStatus)stateOverride;

	if (status!=lastStatus) 
	{
		timeInLastState = timeInState;
		timeInState = 0.0f;
	}

	timeInState += fTime;

	// TP: shuffle game state if needs be
	/*
	if ((usingMemCard)&&(status!=BGAMESAVE_NOSTATE)&&(GetCurrentGameState(&gameStatus)!=GAMESTATE_MEMCARD))
	{
		SetGameState(&gameStatus,GAMESTATE_MEMCARD,GAMESUBSTATE_NORMAL);
	}

	if ((GetCurrentGameState(&gameStatus)==GAMESTATE_MEMCARD) && ((!usingMemCard)||(status==BGAMESAVE_NOSTATE)))
	{
		SetGameState(&gameStatus,(EGameState)gameStatus.lastState,GAMESUBSTATE_NORMAL);
	}*/

#if(BPLATFORM == PS2)
	MemCard_updatePS2(status);
#elif(BPLATFORM == XBOX)
	MemCard_updateXbox(status);
#endif// PP: platform

	MemCard_book.update();
}

/*	--------------------------------------------------------------------------------
	Function 	: MemCard_save
	Purpose 	: start saving some data to the card, using the specified filename
	Parameters 	: data to save, filename to save as, number of bytes to save,
					(opt/NULL)ptr to function to be called when the operation completes
	Returns 	: 
	Info 		: 
*/
void MemCard_save(void* const data, ushort* name, const int size, const MemCardFinishFunc finishFunc)
{

#if BPLATFORM==GAMECUBE
	return;
#endif

	if (!usingMemCard) return;

	bkPrintf("MemCard_save.\n");

	// PP: copy details of what's being saved, so we can retry if needs be
	MemCard_data=data;
	MemCard_name=name;
	MemCard_size=size;
	MemCard_operation=BGAMESAVE_SAVE;

	MemCard_busy=true;

	biGameSaveStart(BGAMESAVE_SAVE, data, mCardPrefix16, size, MemCard_getLoadSaveFlags());
	MemCard_book.open();
	MemCard_book.changeMode(MCDMODE_MAXIMIZED);

	MemCard_finishFunc=finishFunc;
}

 
/*	--------------------------------------------------------------------------------
	Function 	: MemCard_load
	Purpose 	: start loading some data from the card, using the specified filename
	Parameters 	: buffer to receive data, filename to load as, number of bytes to load,
					(opt/NULL)ptr to function to be called when the operation completes
	Returns 	: 
	Info 		: 
*/
void MemCard_load(void* const data, ushort* name, const int size, const MemCardFinishFunc finishFunc)// PP: a bit like Bulgarian Blues, only more up-beat
{

#if BPLATFORM==GAMECUBE
	return;
#endif

	if (!usingMemCard) return;
	
	bkPrintf("MemCard_load.\n");

	// PP: copy details of what's being loaded, so we can retry if needs be
	MemCard_data=data;
	MemCard_name=name;
	MemCard_size=size;
	MemCard_operation=BGAMESAVE_LOAD;

	MemCard_busy=true;

	biGameSaveStart(BGAMESAVE_LOAD, data, mCardPrefix16, size, MemCard_getLoadSaveFlags());

#ifdef MEMCARD_DISPLAY_MESSAGE_WHILE_LOADING

	MemCard_book.open();
	// PP: REMOUT: OOD		MemCard_book.changeMode(MCDMODE_MAXIMIZED);

#endif// PP: def MEMCARD_DISPLAY_MESSAGE_WHILE_LOADING

	MemCard_finishFunc=finishFunc;
}


/*	--------------------------------------------------------------------------------
	Function 	: MemCard_shutdown
	Purpose 	: free up any memory used by this module
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void MemCard_shutdown(void)
{
	if (memCardEvent) bkDeleteEventClient(memCardEvent);
	MemCard_book.shutdown();
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														MEMCARDBOOK methods

								This is the 'stamp' book type used to display memory card dialogue.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: MEMCARDBOOK::init
	Purpose 	: initialise the MEMCARDBOOK
	Parameters 	: 
	Returns 	: 
	Info 		: called by the constructor.  Overrides BOOK:init
*/
void MEMCARDBOOK::init(void)
{
	BOOK::init();

	this->okProgress=0.0f;
}


/*	--------------------------------------------------------------------------------
	Function 	: MEMCARDBOOK::TAG_MEMCARDBOOK
	Purpose 	: default constructor; calls MEMCARDBOOK::init
	Parameters 	: 
	Returns 	: new MEMCARDBOOK instance
	Info 		: 
*/
MEMCARDBOOK::TAG_MEMCARDBOOK(void)
{
	this->init();
}


/*	--------------------------------------------------------------------------------
	Function 	: MEMCARDBOOK::minimize
	Purpose 	: start the book shrinking to its small size, for non-critical messages
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void MEMCARDBOOK::minimize(void)
{
	this->changeMode(MCDMODE_MINIMIZED, MCD_DURATION_RESIZE);
}


/*	--------------------------------------------------------------------------------
	Function 	: MEMCARDBOOK::maximize
	Purpose 	: start the book expanding to its full size, for error messages
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void MEMCARDBOOK::maximize(void)
{
	this->changeMode(MCDMODE_MAXIMIZED, MCD_DURATION_RESIZE);
}


/*	--------------------------------------------------------------------------------
	Function 	: MEMCARDBOOK::ok
	Purpose 	: start the book displaying a final (ok) message before fading out
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void MEMCARDBOOK::ok(void)
{
	MemCard_book.minimize();
	this->MCDflags=MCDFLAG_OK;
	this->okProgress=0.0f;
}


/*	--------------------------------------------------------------------------------
	Function 	: MEMCARDBOOK::update
	Purpose 	: update the memory card book
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides BOOK::update
*/
void MEMCARDBOOK::update(void)
{
	BOOK::update();

	if(this->MCDflags & MCDFLAG_OK)
	{
		this->okProgress=CONSTRAIN(this->okProgress+((1/fps)/(float)MCD_DURATION_OK), 0.0f, 1.0f);

		if(this->okProgress>=1.0f)
		{
			this->okProgress=0.0f;
			this->MCDflags&= ~MCDFLAG_OK;
			this->close();
		}
	}
}

/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												UPDATE FUNCTIONS for MEMORY CARD BOOK PAGES
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// TP: CHECKINGMEDIA
void MemCard_updateFunc_7_0_1(PAGE* const page, const int32 context)
{
	// PP: nothing to do here
}


// TP: NOMEDIA on bootup
void MemCard_updateFunc_2_0_1(PAGE* const page, const int32 context)
{
	// PP: X retry, T cancel
	if(MEMCARD_OKBUTTON)
	{
		// PP: cancel: close the book
		biGameSaveEnd();
		usingMemCard = false;
		MemCard_busy = false;
		/*if ((GetCurrentGameState(&gameStatus)==GAMESTATE_MEMCARD) && ((!usingMemCard)||(status==BGAMESAVE_NOSTATE)))
		{
			SetGameState(&gameStatus,(EGameState)gameStatus.lastState,GAMESUBSTATE_NORMAL);
		}*/
		if(MemCard_finishFunc != NULL) MemCard_finishFunc(true);
		MemCard_book.close();
	}	
}

// TP: NOMEDIA
void MemCard_updateFunc_4_0_3(PAGE* const page, const int32 context)
{
	// PP: X retry, T cancel
	if(MEMCARD_OKBUTTON)
	{
		// PP: retry: start the load/save again
		biGameSaveEnd();
		MemCard_busy = false;

		if(MemCard_operation == BGAMESAVE_LOAD)
		{
			MemCard_load(MemCard_data, MemCard_name, MemCard_size, MemCard_finishFunc);
		}
		else
		{
			MemCard_save(MemCard_data, MemCard_name, MemCard_size, MemCard_finishFunc);
		}
	}
	else if(CANCELBUTTON)
	{
		// PP: cancel: close the book
		biGameSaveEnd();
		usingMemCard = false;
		MemCard_busy = false;

		/*if ((GetCurrentGameState(&gameStatus)==GAMESTATE_MEMCARD) && ((!usingMemCard)||(status==BGAMESAVE_NOSTATE)))
		{
			SetGameState(&gameStatus,(EGameState)gameStatus.lastState,GAMESUBSTATE_NORMAL);
		}*/
		if(MemCard_finishFunc != NULL) MemCard_finishFunc(true);
		MemCard_book.close();
	}	
}

// TP: NOMEDIA but not at boot up
void MemCard_updateFunc_12_5_8_1(PAGE* const page, const int32 context)
{
	// PP: X retry, T cancel
	if(MEMCARD_OKBUTTON)
	{
		// PP: retry: start the load/save again
		biGameSaveEnd();
		MemCard_busy = false;

		if(MemCard_operation == BGAMESAVE_LOAD)
		{
			MemCard_load(MemCard_data, MemCard_name, MemCard_size, MemCard_finishFunc);
		}
		else
		{
			MemCard_save(MemCard_data, MemCard_name, MemCard_size, MemCard_finishFunc);
		}
	}
	else if(CANCELBUTTON)
	{
		MemCard_bootUp = 2;

	}	
}

// TP: DATA IS CORRUPT
void MemCard_updateFunc_7_0_5(PAGE* const page, const int32 context)
{
	// PP: X retry, T cancel
	if(MEMCARD_OKBUTTON)
	{
		// PP: retry: start the load/save again
		biGameSaveEnd();
		MemCard_busy = false;

		if(MemCard_operation == BGAMESAVE_LOAD)
		{
			MemCard_load(MemCard_data, MemCard_name, MemCard_size, MemCard_finishFunc);
		}
		else
		{
			MemCard_save(MemCard_data, MemCard_name, MemCard_size, MemCard_finishFunc);
		}
	}
	else if(CANCELBUTTON)
	{
		// PP: cancel: close the book
		biGameSaveEnd();
		usingMemCard = false;
		MemCard_busy = false;
/*
		if ((GetCurrentGameState(&gameStatus)==GAMESTATE_MEMCARD) && ((!usingMemCard)||(status==BGAMESAVE_NOSTATE)))
		{
			SetGameState(&gameStatus,(EGameState)gameStatus.lastState,GAMESUBSTATE_NORMAL);
		}*/
		if(MemCard_finishFunc != NULL) MemCard_finishFunc(true);
		MemCard_book.close();
	}	
}


// TP: FORMATMEDIAYN
void MemCard_updateFunc_3_0_1(PAGE* const page, const int32 context)
{
	// PP: X ok, O format, T cancel
	if(MEMCARD_OKBUTTON)
	{
		// PP: retry: start the load/save again
		biGameSaveEnd();

		if(MemCard_operation == BGAMESAVE_LOAD)
		{
			MemCard_load(MemCard_data, MemCard_name, MemCard_size, MemCard_finishFunc);
		}
		else
		{
			MemCard_save(MemCard_data, MemCard_name, MemCard_size, MemCard_finishFunc);
		}
	}
	else if(controller1.circleDebounceChannel->value != 0)
	{
		// PP: format
		biGameSaveSetStatus(BGAMESAVE_OKRESPONSE,mCardFullName16,-1,&mySaveGameInfo);
	}
	else if(CANCELBUTTON)
	{
		// PP: cancel: close the book
		biGameSaveSetStatus(BGAMESAVE_CANCELRESPONSE,mCardFullName16,-1,&mySaveGameInfo);
		biGameSaveEnd();
		usingMemCard = false;
		MemCard_busy = false;
		if(MemCard_finishFunc != NULL) MemCard_finishFunc(false);
		MemCard_book.close();		
	}	
}

// TP: FORMATINPROGRESS
void MemCard_updateFunc_8_0_3(PAGE* const page, const int32 context)
{
	// PP: nothing to do here
}

// TP: FORMATFAILED
void MemCard_updateFunc_8_0_7(PAGE* const page, const int32 context)
{
	// PP: X retry, T cancel
	if(MEMCARD_OKBUTTON)
	{
		// PP: NO NOT YET.
		if(MemCard_operation == BGAMESAVE_LOAD)
		{
			MemCard_load(MemCard_data, MemCard_name, MemCard_size, MemCard_finishFunc);
		}
		else
		{
			MemCard_save(MemCard_data, MemCard_name, MemCard_size, MemCard_finishFunc);
		}
	}
	else if(CANCELBUTTON)
	{
		// PP: cancel: close the book
		biGameSaveEnd();
		if(MemCard_finishFunc != NULL) MemCard_finishFunc(false);
		MemCard_book.close();		
	}
}

void MemCard_updateFunc_8_0_6(PAGE* const page, const int32 context)
{
	// PP: X retry, T cancel
	if(MEMCARD_OKBUTTON)
	{
		// PP: cancel: close the book
		biGameSaveEnd();
		if(MemCard_finishFunc != NULL) MemCard_finishFunc(false);
		MemCard_book.close();		
	}
}

// TP: MEDIAFULL
void MemCard_updateFunc_5_0_3(PAGE* const page, const int32 context)
{
	// PP: X retry, O make space, T cancel

	if(MEMCARD_OKBUTTON)
	{
		// PP: retry: start the save again
		biGameSaveEnd();
		MemCard_save(MemCard_data, MemCard_name, MemCard_size, MemCard_finishFunc);

	}
	else if(CANCELBUTTON)
	{
		// PP: cancel: close the book
		biGameSaveEnd();
		usingMemCard = false;
		if(MemCard_finishFunc != NULL) MemCard_finishFunc(false);
		MemCard_book.close();
	}
}

// TP: MEDIAFULL on boot
void MemCard_updateFunc_2_0_2(PAGE* const page, const int32 context)
{
	// PP: X retry, O make space, T cancel

	if(MEMCARD_OKBUTTON)
	{
		// PP: retry: start the save again
		biGameSaveEnd();
		if(MemCard_operation == BGAMESAVE_LOAD)
		{
			MemCard_load(MemCard_data, MemCard_name, MemCard_size, MemCard_finishFunc);
		}
		else
		{
			MemCard_save(MemCard_data, MemCard_name, MemCard_size, MemCard_finishFunc);
		}
	}
	else 
	{
		if(CANCELBUTTON)
		{
			// PP: cancel: close the book
			biGameSaveEnd();
			usingMemCard = false;
			if(MemCard_finishFunc != NULL) MemCard_finishFunc(false);
			MemCard_book.close();
		}
	}
}

// TP: overwrite
void MemCard_updateFunc_5_0_8(PAGE* const page, const int32 context)
{
	/*
	if (MEMCARD_OKBUTTON)
	{
		biGameSaveSetStatus(BGAMESAVE_OKRESPONSE,mCardFullName16,0,&mySaveGameInfo);
	}
	else
	{
		if (CANCELBUTTON)
		{
			biGameSaveSetStatus(BGAMESAVE_CANCELRESPONSE,mCardFullName16,0,&mySaveGameInfo);
		}
		else
		{
			biGameSaveSetStatus(BGAMESAVE_NORESPONSE,mCardFullName16,0,&mySaveGameInfo);
		}
	}
	*/
}

// TP: SAVEINPROGRESS
void MemCard_updateFunc_3_1_1(PAGE* const page, const int32 context)
{
	// PP: nothing to do here
}

// TP: SAVEOK
void MemCard_updateFunc_5_1_3(PAGE* const page, const int32 context)
{
	// PP: nothing to do here
}

// TP: SAVEFAILED
void MemCard_updateFunc_5_0_9(PAGE* const page, const int32 context)
{
	// PP: X retry, T cancel

	if(MEMCARD_OKBUTTON)
	{
		// PP: retry: start the save again
		biGameSaveEnd();
		MemCard_save(MemCard_data, MemCard_name, MemCard_size, MemCard_finishFunc);

	}
	else if(CANCELBUTTON)
	{
		// PP: cancel: close the book
		biGameSaveEnd();
		if(MemCard_finishFunc != NULL) MemCard_finishFunc(false);
		MemCard_book.gotoPage("CANCELED");
		MemCard_book.ok();
	}
}

// TP: LOADNOTFOUND
void MemCard_updateFunc_4_0_1(PAGE* const page, const int32 context)
{
	// PP: X retry, T cancel
	if(MEMCARD_OKBUTTON)
	{
		// PP: retry: start the load again
		biGameSaveEnd();

		// PP: I'D iMAGINE THIS MESSAGE COULD CROP UP WHILE TRYING TO SAVE AS WELL AS WHILE TRYING TO LOAD
		if(MemCard_operation == BGAMESAVE_LOAD)
		{
			MemCard_load(MemCard_data, MemCard_name, MemCard_size, MemCard_finishFunc);
		}
		else
		{
			MemCard_save(MemCard_data, MemCard_name, MemCard_size, MemCard_finishFunc);
		}

	}
	else if(CANCELBUTTON)
	{
		createSave = true;
	}
}

// TP: Create save
void MemCard_updateFunc_CREATESAVE(PAGE* const page, const int32 context)
{
	// PP: X retry, T cancel
	if(MEMCARD_OKBUTTON)
	{
		biGameSaveEnd();

		MemCard_busy = false;
		SaveGame(0, true);
	}
	else if(CANCELBUTTON)
	{
		// PP: cancel: close the book
		biGameSaveEnd();
		usingMemCard = false;
		if(MemCard_finishFunc != NULL) MemCard_finishFunc(false);
		MemCard_book.gotoPage("CANCELED");
		MemCard_book.ok();
	}
}

// TP: LOADINPROGRESS
void MemCard_updateFunc_11_0_1(PAGE* const page, const int32 context)
{
	// PP: nothing to do here
}

// TP: LOADINPROGRESS
void MemCard_updateFunc_4_0_7(PAGE* const page, const int32 context)
{
	// PP: nothing to do here
}

// TP: LOADOK
void MemCard_updateFunc_4_0_8(PAGE* const page, const int32 context)
{
	// PP: nothing to do here
}

// TP: LOADFAILED
void MemCard_updateFunc_4_0_5(PAGE* const page, const int32 context)
{
	// PP: X retry, T cancel

	if(MEMCARD_OKBUTTON)
	{
		// PP: retry: start the load again
		biGameSaveEnd();
		MemCard_load(MemCard_data, MemCard_name, MemCard_size, MemCard_finishFunc);
		stateOverride = 0;

	}
	else if(CANCELBUTTON)
	{
		// PP: cancel: close the book
		biGameSaveEnd();
		if(MemCard_finishFunc != NULL) MemCard_finishFunc(false);
		MemCard_book.close();
		stateOverride = 0;
	}
}


void MemCard_updateFunc_CANCELED(PAGE* const page, const int32 context)
{
	// PP: nothing to do here
}

void MemCard_updateFunc_CONTROLLER(PAGE* const page, const int32 context)
{
	// PP: nothing to do here
}

#if BPLATFORM==PS2
void asciiStringToSJIS(unsigned char *string, unsigned char *dest);
#endif

void MemCard_setupSaveInfo(void)
{
	char				*prefix = MCARDPREFIX;
	char				*fullname = MCARDFULLNAME;
	char				*displayname = TAZDISPLAYNAME;

#if BPLATFORM == PS2
	mySaveGameInfo.iconData = bkLoadFile(NULL,"icon.icn",NULL,&mySaveGameInfo.iconSize,NULL,0);
	if (mySaveGameInfo.iconData==NULL)
	{
		mySaveGameInfo.iconData = bkLoadFile(NULL,"..\\paks\\icon.icn",NULL,&mySaveGameInfo.iconSize,NULL,0);
	}

	memset(&mySaveGameInfo.iconSys, 0, sizeof(sceMcIconSys));

	mySaveGameInfo.iconSys.Head[0] = 'P';
	mySaveGameInfo.iconSys.Head[1] = 'S';
	mySaveGameInfo.iconSys.Head[2] = '2';
	mySaveGameInfo.iconSys.Head[3] = 'D';
	mySaveGameInfo.iconSys.OffsLF = 20;
	mySaveGameInfo.iconSys.TransRate = 64;
	mySaveGameInfo.iconSys.BgColor[0][0] = 0;
	mySaveGameInfo.iconSys.BgColor[0][1] = 0;
	mySaveGameInfo.iconSys.BgColor[0][2] = 128;

	mySaveGameInfo.iconSys.BgColor[1][0] = 128;
	mySaveGameInfo.iconSys.BgColor[1][1] = 0;
	mySaveGameInfo.iconSys.BgColor[1][2] = 0;

	mySaveGameInfo.iconSys.BgColor[2][0] = 128;
	mySaveGameInfo.iconSys.BgColor[2][1] = 128;
	mySaveGameInfo.iconSys.BgColor[2][2] = 128;

	mySaveGameInfo.iconSys.BgColor[3][0] = 128;
	mySaveGameInfo.iconSys.BgColor[3][1] = 128;
	mySaveGameInfo.iconSys.BgColor[3][2] = 128;

	mySaveGameInfo.iconSys.LightDir[0][0] = 1.0f;
	mySaveGameInfo.iconSys.LightDir[0][1] = 0.0f;
	mySaveGameInfo.iconSys.LightDir[0][2] = 1.0f;
	mySaveGameInfo.iconSys.LightDir[0][3] = 1.0f;

	mySaveGameInfo.iconSys.LightDir[1][0] = -1.0f;
	mySaveGameInfo.iconSys.LightDir[1][1] = 0.0f;
	mySaveGameInfo.iconSys.LightDir[1][2] = 0.0f;
	mySaveGameInfo.iconSys.LightDir[1][3] = 1.0f;

	mySaveGameInfo.iconSys.LightDir[2][0] = 0.0f;
	mySaveGameInfo.iconSys.LightDir[2][1] = 0.0f;
	mySaveGameInfo.iconSys.LightDir[2][2] = 0.0f;
	mySaveGameInfo.iconSys.LightDir[2][3] = 1.0f;

	mySaveGameInfo.iconSys.LightColor[0][0] = 0.8f;
	mySaveGameInfo.iconSys.LightColor[0][1] = 0.8f;
	mySaveGameInfo.iconSys.LightColor[0][2] = 0.8f;

	mySaveGameInfo.iconSys.LightColor[1][0] = 0.7f;
	mySaveGameInfo.iconSys.LightColor[1][1] = 0.7f;
	mySaveGameInfo.iconSys.LightColor[1][2] = 0.7f;

	mySaveGameInfo.iconSys.LightColor[2][0] = 0.0f;
	mySaveGameInfo.iconSys.LightColor[2][1] = 0.0f;
	mySaveGameInfo.iconSys.LightColor[2][2] = 0.0f;

	mySaveGameInfo.iconSys.Ambient[0] = 0.5f;
	mySaveGameInfo.iconSys.Ambient[1] = 0.5f;
	mySaveGameInfo.iconSys.Ambient[2] = 0.5f;

	asciiStringToSJIS((uchar*)displayname, mySaveGameInfo.iconSys.TitleName);
	strcpy((char *)mySaveGameInfo.iconSys.FnameView, "icon.icn");
	strcpy((char *)mySaveGameInfo.iconSys.FnameCopy, "icon.icn");
	strcpy((char *)mySaveGameInfo.iconSys.FnameDel, "icon.icn");

#elif(BPLATFORM == XBOX)

	mySaveGameInfo.cardName[0]='\0';// PP: pah?

	// PP: set icon image...

	TBPackageIndex *packageIndexPtr;

	// open the package file, reading in the index portion and making
	// a TBPackageIndex structure from it
	packageIndexPtr = bkOpenPackage("xboxsaveicons");

	/* The icon graphic.
	A 64*64, texture.
	This must be a linear format BTEXTUREFORMAT_8888ARGB_LINEAR, BTEXTUREFORMAT_565RGB_LINEAR or BTEXTUREFORMAT_1555ARGB_LINEAR.
	(Or check the linear flag in the Gizmo texel command).*/
	mySaveGameInfo.icon=bkLoadTexture(packageIndexPtr, "xboxsave.bmp", 0);

	// remember to close the package to prevent resource leakage
	bkClosePackage(packageIndexPtr);

#endif// PP: platform

}


/*  --------------------------------------------------------------------------------
	Function	: LoadFinished
	Purpose		: Called when the load process is finished
	Parameters	: bool success
	Returns		: true if I'm happy with the data else false
	Info		: 
*/
int LoadFinished(const bool success)
{
	if (!usingMemCard) return FALSE;// PP: was keyword 'false'

	int		rtn;		// PP: return value

	if (success)
	{
		rtn=gameStatus.globalSettings.ReadFromGameSave();
	}
	else
	{
		// PP: return FALSE if(!success) - just a guess - I hope this doesn't bugger anything up
		rtn=FALSE;
	}

#ifdef LOAD_AFTER_START_SCREEN

	// PP: this should only be called once the loading has been done
	CreateStartGameMenuPage(&frontEndMenu);

	// PP: and records menu, oops
	CreateRecordsMenuPage(&frontEndMenu);

#endif// PP: def LOAD_AFTER_START_SCREEN

	return rtn;
}

/*  --------------------------------------------------------------------------------
	Function	: SaveFinished
	Purpose		: Called when the save process is finished
	Parameters	: bool success
	Returns		: 
	Info		: 
*/

int SaveFinished(const bool success)
{
	return TRUE;
}


/*  --------------------------------------------------------------------------------
	Function	: MemCardChangeCallback
	Purpose		: Called if the user changes the mem card in any way
	Parameters	: 
	Returns		: 
	Info		: 
*/

void MemCardChangeCallback(char *name, char *parms, void *data, void *context)
{
	int	slot, type;

	// PP: XBOX NOTE: On Xbox, no need to respond to MU insertion/removal in any way, because we don't support them.
	// PP: TCR 3-40 states that we don't need to allow MU actions to interrupt the attract cycle.  Which is nice.

	sscanf(parms, "%i %i", &slot, &type);

	if (type==0)
	{
		MemCard_changed = MEMCARD_REMOVED;
	}
	else
	{
		MemCard_changed = MEMCARD_INSERTED;
	}
}


/*  --------------------------------------------------------------------------------
	Function	: GetMemCardStatus
	Purpose		: Elaborate function to try and extract the state of the memory card from Babel
	Parameters	: none
	Returns		: EMemCardBootState
	Info		: this function will do a dummy save to check for memcard free space and unformatted card etc,
				: if the save have no problems then it will do a dummy load to see if existing data is on the card.
				: NOTE: this function will display some memcard messages
*/

EMemCardBootState	GetMemCardStatus(void)
{
	// TP: start with dummy save
	MemCard_bootStatus=MEMCARDBOOT_NOCARD;
	MemCard_data=&gameStatus.globalSettings.saveInfo;
	MemCard_name=mCardPrefix16;
	MemCard_size=MIN_MEMCARD_SAVESIZE;
	MemCard_operation=BGAMESAVE_SAVE;
	MemCard_busy=true;
	MemCard_bootUp = 1;
	MemCard_allowAccess = true;

	biGameSaveStart(BGAMESAVE_SAVE, &gameStatus.globalSettings.saveInfo, mCardPrefix16, MIN_MEMCARD_SAVESIZE);

	while ((MemCard_busy)||((!usingMemCard)&&MemCard_book.flags&BKFLAG_OPEN))
	{
		DrawLoadLoop(DRAW_MEMCARD|DRAW_MEMCARDUPDATE|/*Not any more DRAW_MISSINGPAD|*//*PP*/(DRAW_SPLASH|DRAW_LOADBAR|DRAW_LEVELNAME));
	}

	MemCard_bootUp = false;
	MemCard_allowAccess = false;

	// TP: we now know if the card is not present, full or ok but with unknown data on it
	// TP: now we need to know if there is Taz data on it
	if (MemCard_bootStatus==MEMCARDBOOT_DATAUNKNOWN)
	{
		MemCard_bootStatus = MEMCARDBOOT_NODATA;

		MemCard_data=&gameStatus.globalSettings.saveInfo;
		MemCard_name=mCardPrefix16;
		MemCard_size=MIN_MEMCARD_SAVESIZE;
		MemCard_operation=BGAMESAVE_LOAD;
		MemCard_busy=true;
		MemCard_bootUp = 1;
		MemCard_allowAccess = true;

		biGameSaveStart(BGAMESAVE_LOAD, &gameStatus.globalSettings.saveInfo, mCardPrefix16, MIN_MEMCARD_SAVESIZE);

		while ((MemCard_busy)||((!usingMemCard)&&MemCard_book.flags&BKFLAG_OPEN))
		{
			DrawLoadLoop(DRAW_MEMCARD|DRAW_MEMCARDUPDATE|/*Not any more DRAW_MISSINGPAD|*//*PP*/(DRAW_SPLASH|DRAW_LOADBAR|DRAW_LEVELNAME));
		}
		MemCard_bootUp = false;
		MemCard_allowAccess = false;
	}

	return MemCard_bootStatus;
}



/*  --------------------------------------------------------------------------------
	Function	: CheckMemCardOnBoot
	Purpose		: Called during boot up to check the memory card, this function will then either load or save data
	Parameters	: 
	Returns		: 
	Info		: 
*/
void CheckMemCardOnBoot(bool wait, int mode)
{
#ifndef LOAD_AFTER_START_SCREEN

	MemCard_book.gotoPage("BLANK",true);

#if (BPLATFORM==PS2)
	// TP: do dummy boot up tests
	GetMemCardStatus();

	if (usingMemCard)
	{
		MemCard_book.open();
	}

	if (mode == BGAMESAVE_LOAD)
	{
		createSave = false;
		LoadGame(wait);
		MemCard_bootUp = 0;
	}

#else
	createSave = false;
	LoadGame(wait);
	MemCard_bootUp = 0;
#endif

#endif// PP: ndef LOAD_AFTER_START_SCREEN
}


/*  --------------------------------------------------------------------------------
	Function	: CheckMemCardOnStart
	Purpose		: perform initial global load after the PRESS START screen
	Parameters	: 
	Returns		: 
	Info		: // PP: named to match CheckMemCardOnBoot, since it's the same function really
*/
void CheckMemCardOnStart(void)
{
#ifdef LOAD_AFTER_START_SCREEN
	
	static bool alreadyDone=false;		// PP: have we already done this?  We only do it once in the whole game

	if(alreadyDone)
	{
		// PP: no loading to do, but we still have to create the start-game menu
		CreateStartGameMenuPage(&frontEndMenu);

		// PP: and records menu, oops
		CreateRecordsMenuPage(&frontEndMenu);
	}
	else
	{
		FORCE_START_DEBOUNCE_ALL_PADS;// PP: need this

		createSave = false;
		LoadGame(false);
		MemCard_bootUp = 0;
	}

	alreadyDone=true;

#endif// PP: def LOAD_AFTER_START_SCREEN
}


/*  --------------------------------------------------------------------------------
	Function	: LoadGame
	Purpose		: Load from the memcard and wait for completion then transfer data to ingame structs
	Parameters	: 
	Returns		: 
	Info		: 
*/

void LoadGame(bool wait)
{
	// PP: on platforms that don't warn the player that they've opted not to save,
	// PP: if the player has opted not to save,
	// PP: simply return from this function.
#ifdef SILENT_NONSAVE

	if(!usingMemCard)
	{
		return;
	}

#endif// PP: def SILENT_NONSAVE


#if BPLATFORM == PC
	HANDLE hFile;
	DWORD bytesRead;
	DWORD bytesWritten;
	char fileToLoad[80];
	char fileToSave[80];

	sprintf(fileToLoad, "%s\\%s", szProductLoc, TAZFILENAME);
	sprintf(fileToSave, "%s\\%s", szProductLoc, TAZBACKUPFILENAME);
		
	// Load the taz game data file
	hFile = CreateFile(fileToLoad, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if(hFile != INVALID_HANDLE_VALUE)
	{
		// Read DataFile from disk
		ReadFile(hFile, &gameStatus.globalSettings.saveInfo, MIN_MEMCARD_SAVESIZE, &bytesRead, NULL);

		// Close file
		CloseHandle(hFile);

		// NH: If CRC check fails
		if (!gameStatus.globalSettings.ReadFromGameSave())
		{
			// NH: Write out a backup of the GlobalSettings
			hFile = CreateFile(fileToSave, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				// Write DataFile to disk
				WriteFile(hFile, &gameStatus.globalSettings.saveInfo, MIN_MEMCARD_SAVESIZE, &bytesWritten, NULL);

				// NH: Give error
				bkPrintf("CRC Failed! - Copying .sav to .bak file");

				// Close file
				CloseHandle(hFile);
			}
		}
	}

	return;
#endif


	if (!usingMemCard) gameStatus.globalSettings.ReadFromGameSave();
	if ((MemCard_busy)||(!usingMemCard)) return;
	bkPrintf("LoadGame.\n");

	//MemCard_book.gotoPage("BLANK",true);

	MemCard_allowAccess = true;

	MemCard_load(&gameStatus.globalSettings.saveInfo, mCardPrefix16, MIN_MEMCARD_SAVESIZE, LoadFinished);
	if (wait)
	{
		while ((MemCard_busy)||(MemCard_book.flags&BKFLAG_OPEN))
		{
			DrawLoadLoop(DRAW_MEMCARD|DRAW_MEMCARDUPDATE|/*Not any more DRAW_MISSINGPAD|*//*PP*/(DRAW_SPLASH|DRAW_LOADBAR|DRAW_LEVELNAME));
		}
		MemCard_allowAccess = false;
	}
}


/*  --------------------------------------------------------------------------------
	Function	: SaveGame
	Purpose		: Save to the memcard and wait for completion then transfer data to ingame structs
	Parameters	: 
	Returns		: 
	Info		: 
*/
void SaveGame(int scene, bool forceSave)
{
	float	time;
	long	size,free;
	TBGameSaveDirEntry	dirEntries;
	int		numFound;

#if(BPLATFORM==PC)
	// NH: Check validity and presence of TazWanted CD
	CheckCD();
	
	 // NH: If CD not valid and user quit.... Quit
	if (gameStatus.quitNow)
		return;
#endif // NH: #if(BPLATFORM==PC)

	// PP: on platforms that don't warn the player that they've opted not to save,
	// PP: if the player has opted not to save,
	// PP: simply return from this function.
#ifdef SILENT_NONSAVE

	if(!usingMemCard)
	{
		return;
	}

#endif// PP: def SILENT_NONSAVE

#ifdef CONSUMERDEMO
	return;
#endif

	if ((forceSave==false)&&((map.sceneNumber==SCENE_TAZINTROCUTSCENE)||(map.sceneNumber==SCENE_STARTGAMEINTRO)||(map.sceneNumber > SCENE_TAZBOSS && map.lastScene == SCENE_FRONTEND)))
		return;

	// TP: if (((gameStatus.demoMode!=0)||(IN_HUB))&&(forceSave==false))	return;
	// TP: Hubs now also save on exit
	if ((gameStatus.demoMode!=0)&&(forceSave==false))	return;

	// JW: Don't Save on a reset
	if ((map.sceneNumber!=SCENE_LANGUAGESELECT)&&(map.sceneNumber == scene)&&(forceSave==false)) return;

	// TP: Don't save on exit of the language selection screen unless it is a forced save
	if ((map.sceneNumber==SCENE_LANGUAGESELECT)&&(forceSave==false)) return;

	// JW: Don't save if we're in the midst of a tourney
	if ((gameStatus.multiplayer.Tournament && map.sceneNumber != SCENE_FRONTEND)&&(forceSave==false)) return;

	// TP: Don't save if leaving the frontend and heading for the language select scene to play the game intro
	if ((forceSave==false)&&(scene==SCENE_LANGUAGESELECT)) return;

	if (gameStatus.globalSettings.currentSaveSlot!=-1)
	{
		bkReadClock(&gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].creation);
	}

	gameStatus.globalSettings.WriteToGameSave();

	// TP: moved this here so the same no save exclusions are tested
#if BPLATFORM == PC
	HANDLE hFile;
	DWORD bytesWritten;
	char fileToSave[80];

	sprintf(fileToSave, "%s\\%s", szProductLoc, TAZFILENAME);
	
	// Create a file to store taz game data
	hFile = CreateFile(fileToSave, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		// Write DataFile to disk
		WriteFile(hFile, &gameStatus.globalSettings.saveInfo, MIN_MEMCARD_SAVESIZE, &bytesWritten, NULL);

		// Close file
		CloseHandle(hFile);
	}
	
	return;
#endif

#if(BPLATFORM == PS2)
	// TP: if there is a memory card present then we must check it
	if ((MemCard_changed==MEMCARD_INSERTED)||((MemCard_changed==MEMCARD_PRESENT)&&(!usingMemCard)))
	{
		// TP: If the memcard has been changed then try and use card again
		usingMemCard = true;
		MemCard_changed = MEMCARD_PRESENT;

		CheckMemCardOnBoot(true, BGAMESAVE_SAVE);

		switch(MemCard_bootStatus)
		{
		case MEMCARDBOOT_UNFORMATED:
		case MEMCARDBOOT_NODATA:
			// TP: do a load
			saveOverwrite = false;
			createSave = false;
			LoadGame(true);
			MemCard_bootUp = 0;
			return;

		default:
			saveOverwrite = true;
			// TP: continue with save
			break;
		}
	}
#else// PP: not ps2

	// PP: TEMP?

	if ((!usingMemCard)&&(globalStatus!=BGAMESAVE_NOMEDIA))
	{
		// TP: If the memcard has been changed then try and use card again
		usingMemCard = true;
		MemCard_changed = MEMCARD_NONE;
	}
#endif// PP: not ps2

	if (!usingMemCard)
	{
		// TP: print write to internal memory message
		CHECKOPEN(MemCard_book);

		MemCard_book.gotoPage("11_0_1");

		MemCard_book.maximize();// TP: minimize

		time=4.0f;

		while ((time-=fTime)>0.0f)
		{
			DrawLoadLoop(DRAW_MEMCARD|DRAW_MEMCARDUPDATE|/*Not any more DRAW_MISSINGPAD|*//*PP*/(DRAW_SPLASH|DRAW_LOADBAR|DRAW_LEVELNAME));
		}

		MemCard_book.close();
	}
	
	if ((MemCard_busy)||(!usingMemCard)) return;
	bkPrintf("SaveGame.\n");

	MemCard_book.gotoPage("BLANK",true);

	MemCard_allowAccess = true;
	MemCard_save(&gameStatus.globalSettings.saveInfo, mCardPrefix16, MIN_MEMCARD_SAVESIZE, SaveFinished);
	
	while ((MemCard_busy)||(MemCard_book.flags&BKFLAG_OPEN))
	{
		DrawLoadLoop(DRAW_FADE|DRAW_MEMCARD|DRAW_MEMCARDUPDATE);

// PP: REMOUT: OOD		checkForMissingControllers(false);
	}
	MemCard_allowAccess = false;
}

/*  --------------------------------------------------------------------------------
	Function	: InitialiseSaveGameSlot
	Purpose		: Initialises data in a slot before a new game is chosen
	Parameters	: 
	Returns		: 
	Info		: 
*/
void InitialiseSaveGameSlot(uint32 slotNumber, EDifficulty difficulty)
{
	BOOK	*ptr;

	int	i,j;

	gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].inUse = TRUE;
	gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].bounty = 0;
	gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].difficulty = difficulty;
	bkReadClock(&gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].creation);
	gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].helpFlags = MORRON_SAFARIVIRGIN|MORRON_KEEPERVIRGIN|MORRON_POSTERVIRGIN|MORRON_NOTALLSNACKS|MORRON_NOTALLDAMAGE|MORRON_NOSAMHEAD;

	 // NH: Not needed here.  This is a global set of lags and if anywhere should be set before any game slots are loaded, and modified according to all of the slots
//	for(i = 0;i < 10;++i)
//	{
//		gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlags[i] = false;
//	}

	for(i = 0;i < SCENE_NUMSCENES;i++)
	{
		gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[i].levelBounty = 0;
		gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[i].completed = false;
		gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[i].pickupsCollected = 0;
		gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[i].postersCollected = 0;
		gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[i].timesCaptured = 0;
		gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[i].objectsDestroyed = 0;
		gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[i].secretItemCollected = false;
		gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[i].destructBonusCollected = false;
		gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[i].bonusgameOption = false;
		gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[i].time = 0.0f;

		for (j=0;j<120;j++)
		{
			gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[i].swirls[j] = false;
		}
		for (j=0;j<10;j++)
		{
			gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[i].posters[j] = false;
		}
		gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[i].flags = 0;
	}
}


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
void MemCard_XboxUpdateFunc_HARDDISK_SAVE(struct TAG_PAGE* const page, const int32 context)
{
	// PP: nothing to do
}


/*	--------------------------------------------------------------------------------
	Function 	: MemCard_XboxUpdateFunc_DAMAGED_GAME
	Purpose 	: update function for an Xbox memory dialogue page:
					Failed to load / loaded bad data
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: named so as to be macro-friendly
					// PP: Must match the PageFunc typedef (TextFX.h)
*/
void MemCard_XboxUpdateFunc_DAMAGED_GAME(struct TAG_PAGE* const page, const int32 context)
{
	if(MEMCARD_OKBUTTON)// PP: A continue without saving
	{
		// PP: ===================================================================
		// PP: NOTE: THIS IS DUPLICATED IN MemCard_XboxUpdateFunc_HARDDISK_FULL...
		// PP: ===================================================================

		MEMCARD_FORCE_OKBUTTON_DEBOUNCE;

/*		if ((GetCurrentGameState(&gameStatus)==GAMESTATE_MEMCARD) && ((!usingMemCard)||(status==BGAMESAVE_NOSTATE)))
		{
			SetGameState(&gameStatus,(EGameState)gameStatus.lastState,GAMESUBSTATE_NORMAL);
		}*/

		// PP: finish the load
		biGameSaveEnd();

		// PP: *** CLEAR THE CORRUPT GAME DATA FROM MEMORY ****
		gameStatus.globalSettings.InitialiseSaveData();

		// PP: must only call MemCard_finishFunc after InitialiseSaveData cos it prepares the start-game menu which displays the loaded/blanked data
		if(MemCard_finishFunc != NULL) MemCard_finishFunc(false);// PP: false: was true, but false is correct cos it means we're unhappy with the loaded data

		// PP: cancel: close the book
		usingMemCard = false;// PP: must only set this false after MemCard_finishFunc
		MemCard_busy = false;

		MemCard_book.close();

		// PP: if there's a callback, yadda yadda, call it...
		if(MemCard_damagedSaveOKCallback != NULL)
		{
			MemCard_damagedSaveOKCallback();
		}
	}

#if(BPLATFORM == XBOX)

	else if(CANCELBUTTON_ANY_PAD)// PP: B go to dashboard to delete the save
	{
		LD_LAUNCH_DASHBOARD		dashLaunchData;

		dashLaunchData.dwReason=XLD_LAUNCH_DASHBOARD_MEMORY;
		dashLaunchData.dwContext=0;								// PP: A title-defined value that will be returned to the title in the LD_FROM_DASHBOARD structure (see XGetLaunchInfo) when the Dashboard reboots to the title after performing whatever service the title has asked of the Dashboard.
		dashLaunchData.dwParameter1=XBOX_GAMESAVE_DRIVE;		// PP: drive to start in	// PP: Parameter passed to the Dashboard. The meaning of this parameter depends on the value specified in dwReason. 
		dashLaunchData.dwParameter2=XBOX_GAMESAVE_NUM_BLOCKS;	// PP: total #blocks needed	// PP: Parameter passed to the Dashboard. The meaning of this parameter depends on the value specified in dwReason. 

		XLaunchNewImage(NULL,// PP: dashboard
						(PLAUNCH_DATA)&dashLaunchData);		
	}

#endif// PP: xbox
}


/*	--------------------------------------------------------------------------------
	Function 	: MemCard_XboxOpenFunc_HARDDISK_FULL
	Purpose 	: page-open function for an Xbox memory dialogue page:
					Tried to save but the hard drive is full
	Parameters 	: ptr to the page, context value
	Returns 	: 
	Info 		: // PP: Must match the PageFunc typedef (TextFX.h)
*/
void MemCard_XboxOpenFunc_HARDDISK_FULL(struct TAG_PAGE* const page, const int32 context)
{
	// PP: get the number of blocks free and the number of blocks short...

	uint32	blocksFree;
	uint32	blocksShort;
	
	blocksFree=MemCard_getNumFreeMemoryUnits();

	blocksShort=XBOX_GAMESAVE_NUM_BLOCKS-blocksFree;

	// PP: (removed rant)

	// PP: update the number of blocks free
	page->getTextbox(STR_XBOX_TCR_BLOCKS_FREE)->setText(STR_XBOX_TCR_BLOCKS_FREE, blocksFree);

	// PP: update the number of blocks short
	page->getTextbox(STR_XBOX_TCR_BLOCKS_SHORT)->setText(STR_XBOX_TCR_BLOCKS_SHORT, blocksShort);
}


/*	--------------------------------------------------------------------------------
	Function 	: MemCard_XboxUpdateFunc_HARDDISK_FULL
	Purpose 	: update function for an Xbox memory dialogue page:
					Tried to save but the hard drive is full
	Parameters 	: ptr to the page, context value
	Returns 	: 
	Info 		: // PP: named so as to be macro-friendly
					// PP: Must match the PageFunc typedef (TextFX.h)
*/
void MemCard_XboxUpdateFunc_HARDDISK_FULL(struct TAG_PAGE* const page, const int32 context)
{
	// PP: DISMISSAL BY A OR START...

	// PP: FIXME?: DUPLICATED CODE!
		
	if(MEMCARD_OKBUTTON)
	{
		// PP: ==================================================================
		// PP: NOTE: THIS IS DUPLICATED IN MemCard_XboxUpdateFunc_DAMAGED_GAME...
		// PP: ==================================================================

		MEMCARD_FORCE_OKBUTTON_DEBOUNCE;

/*		if ((GetCurrentGameState(&gameStatus)==GAMESTATE_MEMCARD) && ((!usingMemCard)||(status==BGAMESAVE_NOSTATE)))
		{
			SetGameState(&gameStatus,(EGameState)gameStatus.lastState,GAMESUBSTATE_NORMAL);
		}*/

		// PP: finish the load
		biGameSaveEnd();

		// PP: *** CLEAR THE CORRUPT GAME DATA FROM MEMORY ****
		gameStatus.globalSettings.InitialiseSaveData();

		// PP: must only call MemCard_finishFunc after InitialiseSaveData cos it prepares the start-game menu which displays the loaded/blanked data
		if(MemCard_finishFunc != NULL) MemCard_finishFunc(false);// PP: false: was true, but false is correct cos it means we're unhappy with the loaded data

		// PP: cancel: close the book
		usingMemCard = false;// PP: must only set this false after MemCard_finishFunc
		MemCard_busy = false;

		MemCard_book.close();

		// PP: if there's a callback, yadda yadda, call it...
		if(MemCard_fullDiskOKCallback != NULL)
		{
			MemCard_fullDiskOKCallback();
		}
	}

	// PP: B LAUNCHES DASHBOARD TO FREE-UP SPACE...

#if(BPLATFORM == XBOX)

	if(MEMCARD_CANCELBUTTON)// PP: Player has seen sense and agreed to cough up the blocks
	{
		// PP: NOTE: IN ORDER TO LAUNCH THE DASHBOARD PROPERLY, THE XBOX MUST HAVE ITS DEFAULT PROGRAM AS THE DASHBOARD.
		// PP: RETAIL BOXES WILL BE LIKE THIS.
		// PP: TO GET A DEV KIT TO DO THIS, TYPE 'xbsetcfg -d' INTO A DOS PROMPT.  TO RESET IT TO USE THE LAUNCHER, TYPE 'xbsetcfg -l' (-L)
		// PP: NOTE: THE DASHBOARD WILL ONLY RETURN TO THE GAME IF THE GAME WAS RUN FROM A DISK

		LD_LAUNCH_DASHBOARD		dashLaunchData;

		//*
		dashLaunchData.dwReason=XLD_LAUNCH_DASHBOARD_MEMORY;
		dashLaunchData.dwContext=0;								// PP: A title-defined value that will be returned to the title in the LD_FROM_DASHBOARD structure (see XGetLaunchInfo) when the Dashboard reboots to the title after performing whatever service the title has asked of the Dashboard.
		dashLaunchData.dwParameter1=XBOX_GAMESAVE_DRIVE;		// PP: drive to start in	// PP: Parameter passed to the Dashboard. The meaning of this parameter depends on the value specified in dwReason. 
		dashLaunchData.dwParameter2=XBOX_GAMESAVE_NUM_BLOCKS;	// PP: total #blocks needed	// PP: Parameter passed to the Dashboard. The meaning of this parameter depends on the value specified in dwReason. 
		/*/
		// PP: LEAVE IN: TEMP TEST
		dashLaunchData.dwReason=XLD_LAUNCH_DASHBOARD_SETTINGS;
		dashLaunchData.dwContext=0;
		dashLaunchData.dwParameter1=XLD_SETTINGS_CLOCK|XLD_SETTINGS_TIMEZONE|XLD_SETTINGS_LANGUAGE;
		dashLaunchData.dwParameter2=0;
		//*/

		XLaunchNewImage(NULL,// PP: dashboard
						(PLAUNCH_DATA)&dashLaunchData);
	}

#endif// PP: (BPLATFORM == XBOX)
}
