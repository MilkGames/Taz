// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : pause.cpp
//   Purpose : pause mode functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "actors.h"
#include "main.h"
#include "pause.h"
#include "textfx.h"
#include "control.h"
#include "status.h"
#include "playerstats.h"
#include "endoflevelstats.h"
#include "display.h"
#include "thwap.h"
#include "icon.h"
#include "scene.h"
#include "map.h"
#include "mouse.h"
#include "Slider.h"					// PP: Horizontal slider page-item used for controlling values, eg. music/sound fx volume.
//#include "frontendmain.h"			// PP: main front-end stuff
#include "music.h"					// PP: music stuff
#include "util.h"
#include "string.h"
#include "popup.h"
#include "subtitle.h"
#include "prompt.h"
#include "tutorial.h"
#include "files.h"
#include "timer.h"
#include "PageItemFX.h"				// PP: Special effects for page items
//#include "control.h"				// PP: for #define ALLOW_CONTROLLERS_IN_ANY_PORTS // PP: IMPORTANT!!!
#include "demo.h"					// PP: consumer demo specifc stuff
#include "CheckBox.h"				// PP: Check box control page-item - lets you toggle something on & off
#include "Table.h"					// PP: Table page-item type - lets you line up items into a grid arrangement
#include "Cheats.h"					// PP: Cheats
#include "multiplayer.h"			// PP: multiplayer stuff
#include "XboxStrings.h"			// PP: Xbox versions of some strings
#include "philscene.h"				// PP: phil's stuff
#include "bossgamecontinue.h"		// PP: boss game continue stuff
#include "ZooBoss.h"				// PP: Zoo Boss - Gossamer vs. Taz in an Elephant Pong showdown
#include "events.h"


#define BACKGROUNDALPHA 64

// PP: define TWEAKNOTE to allow easy tweaking of control note position/scale/rotation
//#define TWEAKNOTE

// PP: standard colour used as a background for messages such as bad disk and missing pad, to subdue whatever is on screen at the time (might even be other text)
COLOUR									standardMessageBackgroundTint(0,0,0,100);// PP: can't extern this (won't link) if it's const - any idea why?

// PP: I do not like repeated anonymous constants.
#define PAUSEMENU_TEXT_ALIGN_SCALE		1.2f

// PP: dimensions of render target for postit-note books
#define POSTIT_ORT_WIDTH				256
#define POSTIT_ORT_HEIGHT				256

// PP: books holding "controller missing" messages (one per player)
BOOK	missingControllerBooks[MAX_PLAYERS];

EPausePopups currentPausePopup;
bool changingMusicVolume, changingSfxVolume = false;

// PP: a record of the music mood in effect as the pause mode was entered, so that we can return to that mood when we exit the pause mode
static EStreamMood		musicMoodBeforePause;

/*static	BOOK	pauseBook;
static  PAGE	*page1,*page2,*page3,*page4;
static	char	currentPage;

static	BOOK	pauseOptions;
static	BOOK	pauseExit;
static	BOOK	exitMenu;
static	char	exitOption;
*/

// PP: TEMP TEST!!!!
#ifdef PHIL
#define NO_MAP_ORTS
//#define NO_MAP_ORT_DRAWS
#endif// PP: def pHIL

#if(BPLATFORM == XBOX)
// PP: TEMP DEMO BODGE!!!!!!!!!!: use a render target that does the job of both hintTarget and controlTarget,
// PP: but which is created only once at the beginning of the game.
// PP: This will hopefully patch up a horrendous memory trampling bug on Xbox release
// PP: NOTE: *** IF FOR ANY REASON YOU WANT TO DEFINE THIS ON ANOTHER PLATFORM, YOU'LL HAVE TO MAKE ADJUSTMENTS... ***
#define FRIGTARGET
#endif// PP: (BPLATFORM == XBOX)

#define SUBTITLE_NONE	((EStringIndex)-1)
#define	NUM_STRIPS		8

float	CURL_ANGLE	=	10.0f;
float	CURL_INC	=	0.5f;

float	rotation	= 0.0f;
float	piRotation	= 0.0f;
float	controlx	= 0.0f;
float	controly	= 0.0f;
float	controlw;							// PP: initialised in InitMapControls
float	controlh;							// PP: initialised in InitMapControls

#if(BPLATFORM == XBOX)
#define CONTROL_NOTE_WIDTH		0.227f		// PP: the width of the control note, as a fraction of the screen height
#define CONTROL_NOTE_HEIGHT		0.227f		// PP: the height of the control note, as a fraction of the screen height
#else// PP: !xbox							
#define CONTROL_NOTE_WIDTH		0.346f		// PP: the width of the control note, as a fraction of the screen height
#define CONTROL_NOTE_HEIGHT		0.346f		// PP: the height of the control note, as a fraction of the screen height
#endif// PP: !xbox

char *pauseMenuNames[] = 
{
	{"menu"},
	{"map"},
	{"stats"},
	{"options"},
#ifdef INGAME_CHEATS_MENU
	{"cheats"},				// PP: the cheats menu
#endif// PP: def INGAME_CHEATS_MENU
};

EStringIndex mapEasyHints[19][8] = 
{
	{SUBTITLE_NONE},
	{SUBTITLE_NONE},
	{SUBTITLE_NONE},
	
	{SUBTITLE_NONE},
	{STR_ICEDOME_HINT5_EASY,STR_ICEDOME_HINT7_EASY,STR_ICEDOME_HINT2_EASY,STR_ICEDOME_HINT6_EASY,
	STR_ICEDOME_HINT3_EASY,STR_ICEDOME_HINT1_EASY,STR_ICEDOME_HINT4_EASY},	// TP: SCENE_ICEDOME
	{STR_SAFARI_HINT1_EASY,STR_SAFARI_HINT2_EASY,STR_SAFARI_HINT3_EASY,STR_SAFARI_HINT4_EASY,
	STR_SAFARI_HINT5_EASY,STR_SAFARI_HINT6_EASY,STR_SAFARI_HINT7_EASY},	// TP: SCENE_SAFARI
	{STR_AQUA_HINT1_EASY,STR_AQUA_HINT2_EASY,STR_AQUA_HINT3_EASY,STR_AQUA_HINT4_EASY,
	STR_AQUA_HINT5_EASY,STR_AQUA_HINT6_EASY,STR_AQUA_HINT7_EASY},	// TP: SCENE_AQUA
	{SUBTITLE_NONE},				// TP: SCENE_ZOOBOSS

	{SUBTITLE_NONE},				// TP: SCENE_CITYHUB

#ifdef CONSUMERDEMO
	{STR_DEPTSTR_HINT1_EASY,STR_DEPTSTR_HINT7_EASY,STR_DEPTSTR_HINT3_EASY,STR_DEPTSTR_HINT6_EASY},
#else
	{STR_DEPTSTR_HINT1_EASY,STR_DEPTSTR_HINT2_EASY,STR_DEPTSTR_HINT3_EASY,STR_DEPTSTR_HINT4_EASY,
	STR_DEPTSTR_HINT5_EASY,STR_DEPTSTR_HINT6_EASY,STR_DEPTSTR_HINT7_EASY},	// TP: SCENE_DEPTSTR
#endif

	{STR_MUSEUM_HINT6_EASY,STR_MUSEUM_HINT1_EASY,STR_MUSEUM_HINT2_EASY,STR_MUSEUM_HINT3_EASY,
	STR_MUSEUM_HINT4_EASY,STR_MUSEUM_HINT7_EASY,STR_MUSEUM_HINT5_EASY},	// TP: SCENE_MUSEUM
	{STR_CONSTRUCT_HINT2_EASY,STR_CONSTRUCT_HINT4_EASY,STR_CONSTRUCT_HINT3_EASY,STR_CONSTRUCT_HINT6_EASY,
	STR_CONSTRUCT_HINT7_EASY,STR_CONSTRUCT_HINT1_EASY,STR_CONSTRUCT_HINT5_EASY},	// TP: SCENE_CONSTRUCT
	{SUBTITLE_NONE},				// TP: SCENE_CITYBOSS

	{SUBTITLE_NONE},				// TP: SCENE_WILDWESTHUB
	{STR_GHOST_HINT1_EASY,STR_GHOST_HINT7_EASY,STR_GHOST_HINT2_EASY,STR_GHOST_HINT6_EASY,
	STR_GHOST_HINT4_EASY,STR_GHOST_HINT5_EASY,STR_GHOST_HINT3_EASY},	// TP: SCENE_GHOST
	{STR_GOLDMINE_HINT1_EASY,STR_GOLDMINE_HINT2_EASY,STR_GOLDMINE_HINT3_EASY,STR_GOLDMINE_HINT6_EASY,
	STR_GOLDMINE_HINT4_EASY,STR_GOLDMINE_HINT5_EASY,STR_GOLDMINE_HINT7_EASY},	// TP: SCENE_GOLDMINE
	{STR_GRANDC_HINT1_EASY,STR_GRANDC_HINT2_EASY,STR_GRANDC_HINT3_EASY,STR_GRANDC_HINT4_EASY,
	STR_GRANDC_HINT5_EASY,STR_GRANDC_HINT6_EASY,STR_GRANDC_HINT7_EASY},	// TP: SCENE_GRANDC
	{SUBTITLE_NONE},				// TP: SCENE_WESTBOSS

	{STR_TASMANIA_HINT1_EASY,STR_TASMANIA_HINT2_EASY,STR_TASMANIA_HINT3_EASY,STR_TASMANIA_HINT4_EASY,
	STR_TASMANIA_HINT5_EASY,STR_TASMANIA_HINT6_EASY,STR_TASMANIA_HINT7_EASY},	// TP: SCENE_TASMANIA
};

EStringIndex mapMedHints[19][8] = 
{
	{SUBTITLE_NONE},
	{SUBTITLE_NONE},
	{SUBTITLE_NONE},
	
	{SUBTITLE_NONE},
	{STR_ICEDOME_HINT5_MEDIUM,STR_ICEDOME_HINT7_MEDIUM,STR_ICEDOME_HINT2_MEDIUM,STR_ICEDOME_HINT6_MEDIUM,
	STR_ICEDOME_HINT3_MEDIUM,STR_ICEDOME_HINT1_MEDIUM,STR_ICEDOME_HINT4_MEDIUM},	// TP: SCENE_ICEDOME
	{STR_SAFARI_HINT1_MEDIUM,STR_SAFARI_HINT2_MEDIUM,STR_SAFARI_HINT3_MEDIUM,STR_SAFARI_HINT4_MEDIUM,
	STR_SAFARI_HINT5_MEDIUM,STR_SAFARI_HINT6_MEDIUM,STR_SAFARI_HINT7_MEDIUM},	// TP: SCENE_SAFARI
	{STR_AQUA_HINT1_MEDIUM,STR_AQUA_HINT2_MEDIUM,STR_AQUA_HINT3_MEDIUM,STR_AQUA_HINT4_MEDIUM,
	STR_AQUA_HINT5_MEDIUM,STR_AQUA_HINT6_MEDIUM,STR_AQUA_HINT7_MEDIUM},	// TP: SCENE_AQUA
	{SUBTITLE_NONE},				// TP: SCENE_ZOOBOSS

	{SUBTITLE_NONE},				// TP: SCENE_CITYHUB

#ifdef CONSUMERDEMO
	{STR_DEPTSTR_HINT1_MEDIUM,STR_DEPTSTR_HINT7_MEDIUM,STR_DEPTSTR_HINT3_MEDIUM,STR_DEPTSTR_HINT6_MEDIUM},
#else
	{STR_DEPTSTR_HINT1_MEDIUM,STR_DEPTSTR_HINT2_MEDIUM,STR_DEPTSTR_HINT3_MEDIUM,STR_DEPTSTR_HINT4_MEDIUM,
	STR_DEPTSTR_HINT5_MEDIUM,STR_DEPTSTR_HINT6_MEDIUM,STR_DEPTSTR_HINT7_MEDIUM},	// TP: SCENE_DEPTSTR
#endif

	{STR_MUSEUM_HINT6_MEDIUM,STR_MUSEUM_HINT1_MEDIUM,STR_MUSEUM_HINT2_MEDIUM,STR_MUSEUM_HINT3_MEDIUM,
	STR_MUSEUM_HINT4_MEDIUM,STR_MUSEUM_HINT7_MEDIUM,STR_MUSEUM_HINT5_MEDIUM},	// TP: SCENE_MUSEUM
	{STR_CONSTRUCT_HINT2_MEDIUM,STR_CONSTRUCT_HINT4_MEDIUM,STR_CONSTRUCT_HINT3_MEDIUM,STR_CONSTRUCT_HINT6_MEDIUM,
	STR_CONSTRUCT_HINT7_MEDIUM,STR_CONSTRUCT_HINT1_MEDIUM,STR_CONSTRUCT_HINT5_MEDIUM},	// TP: SCENE_CONSTRUCT
	{SUBTITLE_NONE},				// TP: SCENE_CITYBOSS

	{SUBTITLE_NONE},				// TP: SCENE_WILDWESTHUB
	{STR_GHOST_HINT1_MEDIUM,STR_GHOST_HINT7_MEDIUM,STR_GHOST_HINT2_MEDIUM,STR_GHOST_HINT6_MEDIUM,
	STR_GHOST_HINT4_MEDIUM,STR_GHOST_HINT5_MEDIUM,STR_GHOST_HINT3_MEDIUM},	// TP: SCENE_GHOST
	{STR_GOLDMINE_HINT1_MEDIUM,STR_GOLDMINE_HINT2_MEDIUM,STR_GOLDMINE_HINT3_MEDIUM,STR_GOLDMINE_HINT6_MEDIUM,
	STR_GOLDMINE_HINT4_MEDIUM,STR_GOLDMINE_HINT5_MEDIUM,STR_GOLDMINE_HINT7_MEDIUM},	// TP: SCENE_GOLDMINE
	{STR_GRANDC_HINT1_MEDIUM,STR_GRANDC_HINT2_MEDIUM,STR_GRANDC_HINT3_MEDIUM,STR_GRANDC_HINT4_MEDIUM,
	STR_GRANDC_HINT5_MEDIUM,STR_GRANDC_HINT6_MEDIUM,STR_GRANDC_HINT7_MEDIUM},	// TP: SCENE_GRANDC
	{SUBTITLE_NONE},				// TP: SCENE_WESTBOSS

	{STR_TASMANIA_HINT1_MEDIUM,STR_TASMANIA_HINT2_MEDIUM,STR_TASMANIA_HINT3_MEDIUM,STR_TASMANIA_HINT4_MEDIUM,
	STR_TASMANIA_HINT5_MEDIUM,STR_TASMANIA_HINT6_MEDIUM,STR_TASMANIA_HINT7_MEDIUM},	// TP: SCENE_TASMANIA
};

// PP: ====================================================
// PP: POSITIONS OF THE MAP CONTROL NOTE, ON EACH LEVEL,
// PP: -0.5 VERY LEFT OF SCREEN TO 0.5 VERY RIGHT OF SCREEN
// PP: ====================================================

#if(BPLATFORM == XBOX)

// PP: I'm having to use a completely different set of positions on Xbox.
// PP: The safe area is much smaller than the PS2's, and Microsoft are pretty strict
// PP: about instructional text going outside it.
// PP: If you want to tweak these positions, there's a nice mechanism in philscene.cpp
// PP: for doing so.  Search for CONTROLNOTEPOS.
float controlNotePos[][2] = 
{
	{0.000f,		0.000f},
	{0.000f,		0.000f},
	{0.000f,		0.000f},
	{0.000f,		0.000f},
	{0.099f,		0.236f},
	{-0.293f,		0.099f},
	{-0.284f,		0.233f},
	{0.000f,		0.000f},
	{0.000f,		0.000f},
	{0.241f,		-0.073f},
	{-0.283f,		-0.200f},
	{-0.279f,		-0.208f},
	{0.000f,		0.000f},
	{0.000f,		0.000f},
	{0.263f,		0.229f},
	{-0.286f,		-0.209f},
	{0.281f,		-0.210f},
	{0.000f,		0.000f},
	{0.115f,		0.233f}
};

#else// PP: not xbox

// PP: *** NOTE *** These fractions came from dividing the X's by 640 and the Y's by 448
float controlNotePos[][2] = 
{
	{0},
	{0},
	{0},
	
	{0},
	{-0.390f, 0.292f},		// PP: {-250.0f, 131.0f},
	{0.378f, 0.321f},		// PP: {242.0f, 144.0f},
	{-0.391f, 0.295f},		// PP: {-250.0f, 132.0f},
	{0},

	{0},
	{-0.373f, -0.319f},		// PP: {-239.0f, -143.0f},
	{-0.373f, -0.319f},		// PP: {-239.0f, -143.0f},
	{-0.370f, -0.324f},		// PP: {-237.0f, -145.0f},
	{0},

	{0},
	{0.377f, 0.304f},		// PP: {241.0f, 136.0f},
	{-0.373f, -0.335f},		// PP: {-239.0f, -150.0f},
	{0.380f, -0.310f},		// PP: {243.0f, -139.0f},
	{0},

	{-0.083f, 0.388f}	// PP: {-53.0f, 174.0f},
};

#endif// PP: not xbox

bool	pauseMenuActive = false;
static  PAGE	*ptrOptionsPage	;
static	CONFIRM	*confirm = NULL;


#ifdef FRIGTARGET
// PP: // PP: TEMP DEMO BODGE!!!!!!!!!!: a render target that does the job of both hintTarget and controlTarget,
// PP: but which is created only once at the beginning of the game.
// PP: This will hopefully patch up a horrendous memory trampling bug on Xbox release
static TBRenderTarget	*frigTarget=NULL;
#else// PP: ifndef FRIGTARGET
TBRenderTarget			*hintTarget=NULL;
TBRenderTarget			*controlTarget=NULL;
#endif// PP: def FRIGTARGET

char		hintMode;
char		hintFlag;
float		hintyOff;
float		hintTime;

BOOK		*hintBook;
BOOK		*controlBook;

PAUSEMENU	pauseMenu;

TBTexture*	hintTexture;
TBTexture*	cntrlTexture;

TBPrimVertex	*postitVerts;
TBPrimVertex	*shadowVerts;

BOOK		*mpPauseBook;
PAUSEMULTIPLAYERPAGE	mppausePage;

#define	MAPHINT_APPEAR			(0)
#define	MAPHINT_DISAPPEARUP	(1)
#define	MAPHINT_DISAPPEARDOWN	(2)

void BuildPostit(TBVector, float);

#if BPLATFORM==GAMECUBE
void DrawPostit(void);
#endif

// PP: TEMP DEBUG TEST - will remove
char* getVideoReturnCodeString(const EBVideoPlayStream code)
{
	static char tempStr[256];

	switch(code)
	{
	case EBVIDEOPLAYSTREAM_FINISHED: return "EBVIDEOPLAYSTREAM_FINISHED: The video stream finished normally";
	case EBVIDEOPLAYSTREAM_MEMCARDCHANGE: return "EBVIDEOPLAYSTREAM_MEMCARDCHANGE: The video aborted due to a memory card being inserted or removed";
	case EBVIDEOPLAYSTREAM_PADCHANGE: return "EBVIDEOPLAYSTREAM_PADCHANGE: The video aborted due to a pad being inserted or removed";
	case EBVIDEOPLAYSTREAM_USERQUIT: return "EBVIDEOPLAYSTREAM_USERQUIT: The video aborted due to the user pressing a pad button";
	case EBVIDEOPLAYSTREAM_CALLBACKQUIT: return "EBVIDEOPLAYSTREAM_CALLBACKQUIT: The video aborted due to the client callback responding to the EBVIDEOREASONCODE_TESTQUIT reason code";
	case EBVIDEOPLAYSTREAM_INITFAILED: return "EBVIDEOPLAYSTREAM_INITFAILED: The video never played due to the client callback responding to the EBVIDEOREASONCODE_INIT reason code";
	case EBVIDEOPLAYSTREAM_ERROR: return "EBVIDEOPLAYSTREAM_ERROR: The video never played due to an error, either the file could not be found or there was insufficient memory for playback.";
	default:
		{
			sprintf(tempStr, "code %d - what does that mean?!");
			return tempStr;
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: initMissingControllerBooks
	Purpose 	: initialise the books for missing-controller messages
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/

#define MISSINGCONTROLLER_FONTSIZE		1.0f	// PP: fontsize for controller-removal messages

#ifdef PHIL
//#define SHOW_VIDEO_ERROR// PP: TEMP DEBUG TEST
#endif// PP: def PHIL

void initMissingControllerBooks(void)
{
	BOOK*			book;
	PAGE*			page;
	TEXTBOX*		textbox;
	EStringIndex	string;	// PP: hmm, might have to use different messages on different platforms

#ifdef ALLOW_CONTROLLERS_IN_ANY_PORTS

	// PP: " Please reconnect the controller and press @(start) to continue. "
	string=STR_XBOX_TCR_DISCONNECT_CONTROLLER;

#endif// PP: def ALLOW_CONTROLLERS_IN_ANY_PORTS

	for(int i=0; i<MAX_PLAYERS; i++)
	{
		book= &missingControllerBooks[i];

		// PP: NEW TEST
		// PP: The missing-controller books will always open as the exclusive book.
		// PP: Furthermore, no exclusive book can close them (hence BKFLAG_COEXCLUSIVE).
		// PP: This prevents each one from closing the other.

#ifdef SHOW_VIDEO_ERROR// PP: TEMP DEBUG TEST
		book->shutdown();
		book->init();
#endif// PP: def SHOW_VIDEO_ERROR

		book->setName("controllerRemovalBook\n");

		// PP: Add the page...

		page=book->addPage();

#ifdef SHOW_VIDEO_ERROR// PP: TEMP DEBUG TEST

		// PP: TEMP DEBUG TEST - will remove
		{
			extern int32	videoReturnCodes[];

			for(int vid=0; vid<7; vid++)
			{
				page->insertItem(new TEXTBOX)->textbox()
					->sprintf8("video %d returned %d", vid, videoReturnCodes[vid])// PP: ->sprintf8("video %d: %s", vid, getVideoReturnCodeString(videoReturnCodes[vid]))
					->setFontSize(1.0f)
					->setWrap(true);
			}
		}

#else// PP: ifndef SHOW_VIDEO_ERROR

#ifdef ALLOW_CONTROLLERS_IN_ANY_PORTS

		if(i==0)
		{
			page->insertItem(STR_PLAYER_1)
				->setStyle(TS_title);
		}
		else// PP: i==1
		{
			page->insertItem(STR_PLAYER_2)
				->setStyle(TS_title);
		}

#else// PP: ifndef ALLOW_CONTROLLERS_IN_ANY_PORTS

		if(i==0)
		{
			// PP: "Please (re)insert a Controller into controller port 1 and press @(start) to continue."
			string=STR_PS2_CONTROLLER1_REMOVAL;
		}
		else
		{
			// PP: "Please (re)insert a Controller into controller port 2 and press @(start) to continue."
			string=STR_PS2_CONTROLLER2_REMOVAL;
		}

#endif// PP: ndef ALLOW_CONTROLLERS_IN_ANY_PORTS

		page->insertItem(textbox=new TEXTBOX(string));

		textbox->setFontSize(MISSINGCONTROLLER_FONTSIZE);
		textbox->setWrap(true);

#ifdef ALLOW_CONTROLLERS_IN_ANY_PORTS
		textbox->setYAlign(PIFLAG_YALIGN_TOP);
#endif// PP: def ALLOW_CONTROLLERS_IN_ANY_PORTS

#endif// PP: def SHOW_VIDEO_ERROR

		// PP: set a full-screen background tint
		book->setFullScreenBackground(standardMessageBackgroundTint);

		// PP: REMOUT: TEMP TESTbook->setScreen(i);

		// PP: REMOUT: TEMP TESTbook->setEffectChooser(&memcardPIEChooser);// PP: and why the fook not?!

		// PP: NEW
		book->setEffectChooser(&basePIEChooser);
		book->flags &= ~BKFLAG_BOOKLIST;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: RefreshCheatsPage
	Purpose 	: update the cheats menu after a change of cheats status
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
static void RefreshCheatsPage(void)
{
	int				boxNum=1;
	CHECKBOX*		checkbox;

	PAGETRAVERSAL	trav(pauseMenu.pauseCheatsTable);

	checkbox=trav.next()->checkbox();

	if(gameStatus.globalSettings.cheatsActive == NO_CHEATS)
	{
		checkbox->setCheck(CHECKBOXSTATE_CHECKED);
	}
	else
	{
		checkbox->setCheck(CHECKBOXSTATE_UNCHECKED);
	}

	for(int i=1; i<NUM_CHEATS; i++)
	{
		if(!CheatImplemented(ECheat(i)))
		{
			continue;
		}
		
		checkbox=trav.next()->checkbox();

#ifdef _TAZDEBUG
		// PP: make absolutely sure we have the right item
		ASSERT(checkbox->value == i);
#endif// PP: debug

		// PP: hide the cheat name if the cheat hasn't been earned yet
		checkbox->setConcealed(!CheatAvailable(ECheat(i)));

		// PP: check/uncheck the cheat name to show if it's active or not
		if(CheatActive(i))
		{
			checkbox->setCheck(CHECKBOXSTATE_CHECKED);
		}
		else
		{
			checkbox->setCheck(CHECKBOXSTATE_UNCHECKED);
		}

		boxNum++;
	}
}


/*  --------------------------------------------------------------------------------
	Function	: LevelForExit
	Purpose		: Return the level to exit to based on current level and settings
	Parameters	: address of uint32 to receive start point crc
	Returns		: level to exit to
	Info		: 
*/

int	LevelForExit(uint32 *startPointCRC=NULL)
{
	uint32	temp;

	// PP: A bit dirty / unclear
	// PP: If we're on a demo disk, we'll want the "exit game" option to appear in the pause menu,
	// PP: rather than the "return to hub" message.
	// PP: It's either return SCENE_FRONTEND here as the exit level, or check CONSUMERDEMO in each
	// PP: place where the exit level is checked.  Which do *you* prefer?
	// PP: Of course, we're not actually going to return to any level if we're on a demo disk.
#ifdef CONSUMERDEMO// PP: any sort of demo disk
	return SCENE_FRONTEND;
#endif// PP: def CONSUMERDEMO

	if (!startPointCRC) startPointCRC = &temp;

	if ((gameStatus.multiplayer.versusCPU)								// PP: REMOUT: might be unsafe		==true
		&&(IN_ZOO||IN_CITY||IN_WEST||IN_TAZ||IN_VR||IN_RACE||IN_DEST)	// PP: was ...IN_WEST|IN_TAZ... - I assume it wanted a ||
		&&(!IN_HUB)
		&&(map.sceneNumber!=SCENE_TAZHUB))
	{
		if ((IN_ZOO)||(((map.sceneNumber==SCENE_SAFARIRACE)||(map.sceneNumber==SCENE_ICEDOMERACE)||(map.sceneNumber==SCENE_DESTRUCTIONAQUA))&&map.lastScene!=SCENE_FRONTEND)) 
		{
			switch(map.sceneNumber)
			{
			case SCENE_SAFARI:
				*startPointCRC = bkCRC32((uchar*)"safarientrance", strlen("safarientrance"), 0);
				break;
			case SCENE_ICEDOME:
				*startPointCRC = bkCRC32((uchar*)"iceentrance", strlen("iceentrance"), 0);
				break;
			case SCENE_AQUA:
				*startPointCRC = bkCRC32((uchar*)"aquaentrance", strlen("aquaentrance"), 0);
				break;
			default:
				*startPointCRC = 0;
				break;
			}
			return SCENE_ZOOHUB;
		}
		else if ((IN_CITY)||(((map.sceneNumber==SCENE_DEPTSTRVEHICLERACE)||(map.sceneNumber==SCENE_DESTRUCTIONCONSTRUCT)||(map.sceneNumber==SCENE_MUSEUMVEHICLERACE))&&map.lastScene!=SCENE_FRONTEND)) 
		{
			switch(map.sceneNumber)
			{
			case SCENE_DEPTSTR:
				*startPointCRC = bkCRC32((uchar*)"storeentrance", strlen("storeentrance"), 0);
				break;
			case SCENE_CONSTRUCT:
				*startPointCRC = bkCRC32((uchar*)"constructionentrance", strlen("constructionentrance"), 0);
				break;
			case SCENE_MUSEUM:
				*startPointCRC = bkCRC32((uchar*)"museumentrance", strlen("museumentrance"), 0);
				break;
			default:
				*startPointCRC = 0;
				break;
			}
			return SCENE_CITYHUB;
		}
		else if ((IN_WEST)||(((map.sceneNumber==SCENE_GOLDMINERACE)||(map.sceneNumber==SCENE_DESTRUCTIONTASMANIA)||(map.sceneNumber==SCENE_GRANDCVEHICLERACE))&&map.lastScene!=SCENE_FRONTEND)) 
		{
			switch(map.sceneNumber)
			{
			case SCENE_GOLDMINE:
				*startPointCRC = bkCRC32((uchar*)"goldmine", strlen("goldmine"), 0);
				break;
			case SCENE_GRANDCANYON:
				*startPointCRC = bkCRC32((uchar*)"grandcanyon", strlen("grandcanyon"), 0);
				break;
			case SCENE_GHOSTTOWN:
				*startPointCRC = bkCRC32((uchar*)"ghosttown", strlen("ghosttown"), 0);
				break;
			default:
				*startPointCRC = 0;
				break;
			}
			return SCENE_WILDWESTHUB;
		}
		else if (map.sceneNumber==SCENE_TAZHUB) return SCENE_FRONTEND;
		else if (IN_TAZ) return SCENE_TAZHUB;
	}

	return SCENE_FRONTEND;
}


/*	--------------------------------------------------------------------------------
	Function 	: cheatsMenuActionFunc
	Purpose 	: callback that responds to menu actions on the cheats menu
	Parameters 	: ref to a MENUACTION_INFO structure (TextFX.h)
	Returns 	: 
	Info 		: // PP: Must match the MenuActionFunc typedef (TextFX.h)
*/
void cheatsMenuActionFunc(MENUACTION_INFO& info)
{
	switch(info.action)
	{
	case MENUACTION_BACK:
		{
			// PP: Player wants to go back to the previous menu.
			// PP: I happen to know that the previous menu is the Options menu.
			// PP: However, ideally we'd have a system where you could construct menu hierarchies, and then the navigation from menu to menu would be automatic.
			// PP: **** But I haven't been the one writing the front end, so I haven't needed it. See? ****

			// PP: DIRTY, BUT I WAS FORCED INTO IT BY CIRCUMSTANCES OUTWITH MY CONTROL!!!
			FORCE_CANCELBUTTON_DEBOUNCE;

			pauseMenu.currentPage = PAUSE_OPTIONS;

			pauseMenu.pauseMenuBook->gotoPage(pauseMenuNames[pauseMenu.currentPage]);
		}
		break;


	case MENUACTION_PRESS:
		{
			ECheat	cheat;		// PP: the cheat represented by the item

			cheat=ECheat(info.item->value);

			// PP: if we've chosen 'none'...
			if(cheat == CHEAT_NONE)
			{
				if(gameStatus.globalSettings.cheatsActive != NO_CHEATS)// PP: if some cheats are on...
				{
					// PP: ...deactivate all cheats
					DeactivateAllCheats();

					// PP: bring the cheats table up to date.
					RefreshCheatsPage();
				}
			}
			else
			{
				// PP: ...otherwise toggle the cheat on / off

				if(CheatActive(cheat))
				{
					DeactivateCheat(cheat);

					// PP: bring the cheats table up to date.
					// PP: this is needed because, conceivably, deactivating one cheat could reactivate others
					RefreshCheatsPage();
				}
				else
				{
					ActivateCheat(cheat);

					// PP: bring the cheats table up to date.
					// PP: this is needed because activating one cheat can deactivate others
					RefreshCheatsPage();
				}
			}
		}
		break;
	}
}


#ifdef INGAME_CHEATS_MENU
/*	--------------------------------------------------------------------------------
	Function 	: CreateCheatsPage
	Purpose 	: Create the cheat options page for the pause menu
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: TODO: Surely we could do with putting options (volume, cheats, etc.) into the multiplayer pause menu too?
*/
static void CreateCheatsPage(void)
{
	PAGE*	page;

	page=pauseMenu.pauseMenuBook->addPage(pauseMenuNames[PAUSE_CHEATS])
		->setNavigation(true);

	// PP: set the page's menu-action callback (this'll recieve notification when an item is pressed or that)
	page->setMenuActionFunc(cheatsMenuActionFunc);

	// PP: this page must not allow the START button to trigger MENUACTION_PRESS, because START is used to pause and unpause
	page->ignoreStartButton();

	// PP: Add title
	page->insertItem(STR_CHEATS)
		->setStyle(TS_title)
		->setYAlign(PIFLAG_YALIGN_TOP)
		->setSelectable(false);

	// PP: Add a checkbox for each cheat...

	// PP: The page is going divide all the implemented cheats into 2 even columns.

	TABLE*		table;
	CHECKBOX*	checkbox;

	// PP: insert the cheats table with 2 columns...

	// PP: "A table for two, please."
	(table=pauseMenu.pauseCheatsTable=new TABLE(2))
		->setNavigation(true)
		//->setBoxStyle(false, true)
		->setBackgroundColour(COLOUR(0,0,80,70))
		->setBorderColour(COLOUR(255, 255, 0, 255));

	page->insertItem(table);

#define CHEATMENU_FONT_SIZE		0.65f// PP: 0.8f

	for(int i=0; i<NUM_CHEATS; i++)
	{
		if((i==CHEAT_NONE) || CheatImplemented(ECheat(i)))
		{
			table->insertItem(checkbox=new CHECKBOX(CHEAT_NAME(i)))->checkbox()
				//->setWrap(true)
				->setFontSize(CHEATMENU_FONT_SIZE)
				->setXAlign(PIFLAG_XALIGN_LEFT)
				->setValue(i);// PP: store the index of the cheat corresponding to each checkbox.  In this way we can make sense of each checkbox.
		}
	}

	// PP: add a control prompt
	page->insertItem(STR_RESUME_OR_SELECT)
		->setStyle(TS_control)
		->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setSelectable(false);
}
#endif// PP: def INGAME_CHEATS_MENU


/*	--------------------------------------------------------------------------------
	Function 	: Pause_init
	Purpose 	: Initialise stuff for the pause mode, eg. set up the pause menu book
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: 
*/
void Pause_init(void)
{
	BOOK*	bookPtr;
	MENU_SOUNDS	*pauseSoundsPtr;
	
	pauseMenu.pauseMenuBook = bookPtr = new BOOK("pauseMenu.pauseMenuBook");// PP: SAFE_DELETEd in Pause_shutdown

	// NH: Setup sounds
	pauseMenu.pauseMenuBook->menuSounds = pauseSoundsPtr = new MENU_SOUNDS;

	strcpy(pauseSoundsPtr->navigateSound.name, "moveselection.wav");
	strcpy(pauseSoundsPtr->selectSound.name, "select.wav");
	strcpy(pauseSoundsPtr->errorSound.name, "cancel.wav");
	strcpy(pauseSoundsPtr->backupSound.name, "deselect.wav");
	pauseSoundsPtr->navigateSound.volume = 255;
	pauseSoundsPtr->selectSound.volume = 255;
	pauseSoundsPtr->errorSound.volume = 255;
	pauseSoundsPtr->backupSound.volume = 255;

	//CreatePauseMenuPage(&pauseMenu);
	CreatePauseMapPage(&pauseMenu);
	CreatePauseStatsPage(&pauseMenu);
	CreatePauseOptionsPage(&pauseMenu);

#ifdef INGAME_CHEATS_MENU
	// PP: Create the Cheats menu, acessible via the Pause menu
	CreateCheatsPage();
#endif// PP: def INGAME_CHEATS_MENU

	mpPauseBook = new BOOK;

	CreateMPPausePage(mpPauseBook);

	postitVerts = new TBPrimVertex[(NUM_STRIPS + 1) * 2];
	shadowVerts = new TBPrimVertex[(NUM_STRIPS + 1) * 2];

	// PP: initialise books for "missing controller" messages
	initMissingControllerBooks();

#if(BPLATFORM == PC)// PP: (this only exists on PC)
	// NH: Hmmm.. Should find somewhere better for this!
	InitMissingCdWarning();
#endif// PP: pc

#ifdef FRIGTARGET
	frigTarget = bdCreateRenderTarget(POSTIT_ORT_WIDTH, POSTIT_ORT_HEIGHT, 16, 16, BCREATERENDERTARGET_PRIVATEZ);
#endif// PP: def FRIGTARGET
}


/*	--------------------------------------------------------------------------------
	Function 	: CreatePauseMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void CreatePauseMenuPage(PAUSEMENU *menu)
{
	PAUSEMENUPAGE		*ptr;
	PAGE				*pagePtr, *subPage;

	TEXTBOX				*dummyBox;

	ptr = menu->pauseMenuPage = (PAUSEMENUPAGE*)ZALLOC(sizeof(PAUSEMENUPAGE));
	if(!ptr)
	{
		bkPrintf("*** WARNING ***, could not malloc pause menu page/n");
		return;
	}
	// NH: Add Page (Pause)
	pagePtr = ptr->page = menu->pauseMenuBook->addPage(pauseMenuNames[PAUSE_MENU]);
	pagePtr->setNavigation(true);
	
	// NH: Add Title
	pagePtr->insertItem(ptr->titleTb = new TEXTBOX(STR_PAUSE_TITLE),false);
	ptr->titleTb->setStyle(TS_title)
		->setYAlign(PIFLAG_YALIGN_TOP)
		->setSelectable(false);

	// NH: Add Menu Items (Map)
	pagePtr->insertItem(ptr->mapTb = new TEXTBOX(STR_PAUSE_MAP),true);
	ptr->mapTb->setAlignScale(PAUSEMENU_TEXT_ALIGN_SCALE)
		->setYPadding(false);

	// NH: Add Menu Items (Stats)
	pagePtr->insertItem(ptr->statsTb = new TEXTBOX(STR_PAUSE_STATS),true);
	ptr->statsTb->setAlignScale(PAUSEMENU_TEXT_ALIGN_SCALE)
		->setYPadding(false);

	// NH: Add Menu Items (Options)
	pagePtr->insertItem(ptr->optionsTb = new TEXTBOX(STR_PAUSE_OPTIONS),true);
	ptr->optionsTb->setAlignScale(PAUSEMENU_TEXT_ALIGN_SCALE)
		->setYPadding(false);

	// NH: INFO
	// NH: Add Info Text Sub Book
	pagePtr->insertItem(ptr->infoSubBook=new BOOK("INFO SUB BOOK"));
	ptr->infoSubBook->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setSelectable(false);

	// NH: Add Info Text (Using Macro)
#define ADD_INFOSUBBOOK_PAGE(name, infoText) \
	subPage=ptr->infoSubBook->addPage(#name); \
	subPage->insertItem(infoText) \
		->setStyle(TS_info) \
		->setYAlign(PIFLAG_YALIGN_BOTTOM) \
		->setAlignScale(1.2f) \
		->setSelectable(false);

		

	// NH: Add all info pages (using the macro).  Seperate pages allow for fading between the actions
	ADD_INFOSUBBOOK_PAGE(MAP,STR_PAUSE_VIEW_MAP);
	ADD_INFOSUBBOOK_PAGE(STATS,STR_PAUSE_VIEW_STATS);
	ADD_INFOSUBBOOK_PAGE(OPTIONS,STR_PAUSE_VIEW_OPTIONS);

	// NH: CONTROL
	// NH: Add Control Text Sub Book
	pagePtr->insertItem(ptr->controlSubBook=new BOOK("CONTROL SUB BOOK"));
	ptr->controlSubBook->setYPadding(false)
		->setSelectable(false);

	// NH: Add Control Text (Using Macro)
#define ADD_CONTROLSUBBOOK_PAGE(name, controlText) \
	subPage=ptr->controlSubBook->addPage(#name); \
	subPage->insertItem(controlText) \
		->setStyle(TS_control) \
		->setYPadding(false) \
		->setSelectable(false);

	// NH: Add all control pages (using the macro).  Seperate pages allow for fading between the actions
	ADD_CONTROLSUBBOOK_PAGE(RESUMEORSELECT,STR_RESUME_OR_SELECT);

	ptr->infoSubBook->open();// PP: books default to closed
	ptr->controlSubBook->open();
}

/*	--------------------------------------------------------------------------------
	Function 	: CreatePauseMapPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void CreatePauseMapPage(PAUSEMENU *menu)
{
	PAUSEMAPPAGE		*ptr;
	PAGE				*pagePtr, *subPage;

	TEXTBOX				*dummyBox;

	ptr = menu->pauseMapPage = (PAUSEMAPPAGE*)ZALLOC(sizeof(PAUSEMAPPAGE));
	if(!ptr)
	{
		bkPrintf("*** WARNING ***, could not malloc pause map page/n");
		return;
	}
	// NH: Add Page (Pause Map)
	pagePtr = ptr->page = menu->pauseMenuBook->addPage(pauseMenuNames[PAUSE_MAP]);
//	pagePtr->setNavigation(true);

	// NH: INFO
	// NH: Add Info Text Sub Book
	pagePtr->insertItem(ptr->infoSubBook=new BOOK("INFO SUB BOOK"));
	ptr->infoSubBook->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setSelectable(false);

	// NH: Add Info Text (Using Macro)
#define ADD_INFOSUBBOOK_PAGE(name, infoText) \
	subPage=ptr->infoSubBook->addPage(#name); \
	subPage->insertItem(infoText) \
		->setStyle(TS_info) \
		->setYAlign(PIFLAG_YALIGN_BOTTOM) \
		->setAlignScale(1.2f) \
		->setSelectable(false);

	// NH: Add all info pages (using the macro).  Seperate pages allow for fading between the actions
	//ADD_INFOSUBBOOK_PAGE(CROSSANDHINT,STR_CROSS_FLYBY_SQUARE_HINT);

	// NH: CONTROL
	// NH: Add Control Text Sub Book
	pagePtr->insertItem(ptr->controlSubBook=new BOOK("CONTROL SUB BOOK"));
	ptr->controlSubBook->setYPadding(false)
		->setSelectable(false);

	// NH: Add Control Text (Using Macro)
#define ADD_CONTROLSUBBOOK_PAGE(name, controlText) \
	subPage=ptr->controlSubBook->addPage(#name); \
	subPage->insertItem(controlText) \
		->setStyle(TS_control) \
		->setYPadding(false) \
		->setSelectable(false);

	// NH: Add all control pages (using the macro).  Seperate pages allow for fading between the actions
	//ADD_CONTROLSUBBOOK_PAGE(RESUMEORBACK,STR_RESUME_OR_BACK);
	//ADD_CONTROLSUBBOOK_PAGE(FLYBYORHNT,STR_CROSS_FLYBY_SQUARE_HINT);

	ptr->infoSubBook->open();// PP: books default to closed
	ptr->controlSubBook->open();
}

/*	--------------------------------------------------------------------------------
	Function 	: CreatePauseStatsPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void CreatePauseStatsPage(PAUSEMENU *menu)
{
	PAUSESTATSPAGE		*ptr;
	PAGE				*pagePtr, *subPage;

	TEXTBOX				*dummyBox, *Tb;

	ptr = menu->pauseStatsPage = (PAUSESTATSPAGE*)ZALLOC(sizeof(PAUSESTATSPAGE));

	if(!ptr)
	{
		bkPrintf("*** WARNING ***, could not malloc pause stats page/n");
		return;
	}
	
	// NH: Add Page (Stats)
	pagePtr = ptr->page = menu->pauseMenuBook->addPage(pauseMenuNames[PAUSE_STATS]);
	pagePtr->setNavigation(true);

	// NH: Add Title
	pagePtr->insertItem(ptr->titleTb = new TEXTBOX(STR_PAUSE_TITLE),false);
	ptr->titleTb->setStyle(TS_title)
		->setYAlign(PIFLAG_YALIGN_TOP)
		->setSelectable(false);

		// NH: Add Menu Items (Map)
	pagePtr->insertFreeItem(ptr->mapTb = new TEXTBOX(STR_PAUSE_MAP),RECTANGLE(-0.3f, 0.0f, -0.375f, -0.275f));
	ptr->mapTb->setAlignScale(PAUSEMENU_TEXT_ALIGN_SCALE)
		->setSelectable(true);
		//->setYPadding(false);

	// NH: Add Menu Items (Options)
	pagePtr->insertFreeItem(ptr->optionsTb = new TEXTBOX(STR_PAUSE_OPTIONS),RECTANGLE(0.0f, 0.3f, -0.375f, -0.275f));
	ptr->optionsTb->setAlignScale(PAUSEMENU_TEXT_ALIGN_SCALE)
		->setSelectable(true);
		//->setYPadding(false);

	// NH: INFO
	// NH: Add Info Text Sub Book
	pagePtr->insertItem(ptr->infoSubBook=new BOOK("INFO SUB BOOK"));
	ptr->infoSubBook->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setSelectable(false);

	// NH: Add Info Text (Using Macro)
#define ADD_INFOSUBBOOK_PAGE(name, infoText) \
	subPage=ptr->infoSubBook->addPage(#name); \
	subPage->insertItem(infoText) \
		->setStyle(TS_info) \
		->setYAlign(PIFLAG_YALIGN_BOTTOM) \
		->setAlignScale(1.2f) \
		->setSelectable(false);

	// NH: Add all info pages (using the macro).  Seperate pages allow for fading between the actions
	ADD_INFOSUBBOOK_PAGE(MAP,STR_PAUSE_VIEW_MAP);
	ADD_INFOSUBBOOK_PAGE(OPTIONS,STR_PAUSE_VIEW_OPTIONS);

	// NH: CONTROL
	// NH: Add Control Text Sub Book
	pagePtr->insertItem(ptr->controlSubBook=new BOOK("CONTROL SUB BOOK"));
	ptr->controlSubBook->setYPadding(false)
		->setSelectable(false);

	// NH: Add Control Text (Using Macro)
#define ADD_CONTROLSUBBOOK_PAGE(name, controlText) \
	subPage=ptr->controlSubBook->addPage(#name); \
	subPage->insertItem(controlText) \
		->setStyle(TS_control) \
		->setYPadding(false) \
		->setSelectable(false);

	// NH: Add all control pages (using the macro).  Seperate pages allow for fading between the actions
	ADD_CONTROLSUBBOOK_PAGE(RESUMEORSELECT,STR_RESUME_OR_SELECT);

	ptr->infoSubBook->open();// PP: books default to closed
	ptr->controlSubBook->open();
}

void CreateMPPausePage(BOOK* book)
{
	PAGE	*subPage;

	mppausePage.page = book->addPage("mp pause");
	mppausePage.page->setNavigation(true);

	mppausePage.page->insertItem(mppausePage.titleTb = new TEXTBOX(STR_PAUSE_TITLE),true);
	mppausePage.titleTb->setStyle(TS_title)
		->setScale(1.5f)
		->setYAlign(PIFLAG_YALIGN_TOP)
		->setSelectable(false);

	mppausePage.page->insertItem(mppausePage.resumeTb = new TEXTBOX(STR_RESUME),true);
	mppausePage.resumeTb->setScale(1.25f)
		->setYPadding(false);

#if(!((defined CONSUMERDEMO) && (BPLATFORM == PS2)))
	mppausePage.page->insertItem(mppausePage.quitTb = new TEXTBOX(STR_EXIT_EXITGAME),true);
	mppausePage.quitTb->setScale(1.25f)
		->setYPadding(false);
#endif

	mppausePage.page->insertItem(mppausePage.infoSubBook=new BOOK("INFO SUB BOOK"));
	mppausePage.infoSubBook->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setSelectable(false);

	// NH: Add Info Text (Using Macro)
#define ADD_MPINFOSUBBOOK_PAGE(name, infoText) \
	subPage=mppausePage.infoSubBook->addPage(#name); \
	subPage->insertItem(infoText) \
		->setStyle(TS_info) \
		->setYAlign(PIFLAG_YALIGN_BOTTOM) \
		->setAlignScale(1.2f) \
		->setSelectable(false);	

	ADD_MPINFOSUBBOOK_PAGE(RESUMEMULTIPLAYER,STR_RESUME_TO_RETURN);

#if(!((defined CONSUMERDEMO) && (BPLATFORM == PS2)))
	ADD_MPINFOSUBBOOK_PAGE(QUITMULTIPLAYER,STR_PAUSE_QUIT_GAME);
#endif

	mppausePage.page->insertItem(mppausePage.controlSubBook=new BOOK("CONTROL SUB BOOK"));
	mppausePage.controlSubBook->setYPadding(false)
		->setSelectable(false);

	// NH: Add Control Text (Using Macro)
#define ADD_MPCONTROLSUBBOOK_PAGE(name, controlText) \
	subPage=mppausePage.controlSubBook->addPage(#name); \
	subPage->insertItem(controlText) \
		->setStyle(TS_control) \
		->setYPadding(false) \
		->setSelectable(false);

	// NH: Add all control pages (using the macro).  Seperate pages allow for fading between the actions
	ADD_MPCONTROLSUBBOOK_PAGE(RESUMEORSELECT,/* PP: STR_RESUME_OR_SELECT*/STR_PRESS_START_TO_RESUME);// PP: JC suggested we change to just "(START) resume"

	mppausePage.infoSubBook->open();// PP: books default to closed
	mppausePage.controlSubBook->open();

	mppausePage.page->selectItem(mppausePage.resumeTb);

	subPage = book->addPage("quit");
	subPage->insertFreeItem(new TEXTBOX(STR_EXIT_EXITGAME), RECTANGLE(-0.5f, 0.5f, -0.1f, 0.5f))->textbox()
		->setFontSize(2.0f)
		->setWrap(true);// PP: PLEASE TEST THIS ON NORMAL MULTIPLAYER QUIT!!!!
}


/*	--------------------------------------------------------------------------------
	Function 	: pauseOptionsMenuActionFunc
	Purpose 	: callback that responds to menu actions on the pause options menu
	Parameters 	: ref to a MENUACTION_INFO structure (TextFX.h)
	Returns 	: 
	Info 		: // PP: Must match the MenuActionFunc typedef (TextFX.h)
					// PP: I realise I'm being a bit unsociable here, splitting my little option off into its own callback,
					// PP: it's just that I think this way is nicer.
					// PP: I could change the whole pause menu over to use this system, but it'd be kinda pointless at this late stage.
*/
void pauseOptionsMenuActionFunc(MENUACTION_INFO& info)
{
	if(info.item != NULL)
	{
#ifdef INGAME_CHEATS_MENU
		if(*item == "CHEATS")
		{
			switch(action)
			{
			case MENUACTION_SELECT:
				{
					// PP: Cheats option has been selected.
					// PP: Show some info text to explain what the Cheats option is.  Some people might not know.
					pauseMenu.pauseOptionsPage->infoSubBook->gotoPage("CHEATS");
				}
				break;

			case MENUACTION_PRESS:
				{
					// PP: Cheats option has been pressed.
					// PP: Turn to the Cheats page.

					pauseMenu.currentPage = PAUSE_CHEATS;

					// PP: make sure the cheats page is up to date
					RefreshCheatsPage();

					pauseMenu.pauseMenuBook->gotoPage(pauseMenuNames[pauseMenu.currentPage]);

					// PP: DIRTY, BUT I WAS FORCED INTO IT BY CIRCUMSTANCES OUTWITH MY CONTROL!!!
					// PP: (should be PAGE::debounceMenuActionPress, called from PAGE::respondToNavigation)
					FORCE_OKBUTTON_DEBOUNCE;
				}
				break;
			}
		}
#endif// PP: def INGAME_CHEATS_MENU
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: CreatePauseOptionsPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void CreatePauseOptionsPage(PAUSEMENU *menu)
{
	PAUSEOPTIONSPAGE	*ptr;
	PAGE				*pagePtr, *subPage;

	TEXTBOX				*dummyBox;

	ptr = menu->pauseOptionsPage = (PAUSEOPTIONSPAGE*)ZALLOC(sizeof(PAUSEOPTIONSPAGE));
	if(!ptr)
	{
		bkPrintf("*** WARNING ***, could not malloc pause options page/n");
		return;
	}
	// NH: Add Page (Options)
	pagePtr = ptr->page = menu->pauseMenuBook->addPage(pauseMenuNames[PAUSE_OPTIONS]);
	pagePtr->setNavigation(true);

	// PP: set the page's menu-action callback (this'll recieve notification when an item is pressed or that)
	pagePtr->setMenuActionFunc(pauseOptionsMenuActionFunc);

	// PP: this page must not allow the START button to trigger MENUACTION_PRESS, because START is used to pause and unpause
	pagePtr->ignoreStartButton();
	
	// NH: Add Title
	pagePtr->insertItem(ptr->titleTb = new TEXTBOX(STR_PAUSE_OPTIONS),false);
	ptr->titleTb->setStyle(TS_title)
		->setYAlign(PIFLAG_YALIGN_TOP)
		->setSelectable(false);
	
	// NH: Add Menu Items (Music Vol)
	pagePtr->insertItem(ptr->musicVolTb = new TEXTBOX(STR_AU_MUSIC_VOLUME),true);
	ptr->musicVolTb->setAlignScale(PAUSEMENU_TEXT_ALIGN_SCALE)
		->setYPadding(false);

	// NH: Add Menu Item (SFX Text)
	pagePtr->insertItem(ptr->sfxVolTb = new TEXTBOX(STR_AU_SFX_VOLUME),true);
	ptr->sfxVolTb->setAlignScale(PAUSEMENU_TEXT_ALIGN_SCALE)
		->setYPadding(false);

#if BPLATFORM!=PC
	// NH: Add Menu Items (Vibration)
	pagePtr->insertItem(ptr->vibrationTb = new TEXTBOX(STR_GO_VIBRATION),true);
	ptr->vibrationTb->setAlignScale(PAUSEMENU_TEXT_ALIGN_SCALE)
		->setYPadding(false);
#endif

	// NH: Add Menu Items (Subtitles)
	pagePtr->insertItem(ptr->subtitlesTb = new TEXTBOX(STR_GO_SUBTITLES),true);
	ptr->subtitlesTb->setAlignScale(PAUSEMENU_TEXT_ALIGN_SCALE)
		->setYPadding(false);

#ifdef INGAME_CHEATS_MENU
	// PP: Add Cheats option - come on - guys - let me do this - please?
	pagePtr->insertItem(STR_CHEATS)
		->setName("CHEATS")
		->setAlignScale(PAUSEMENU_TEXT_ALIGN_SCALE)
		->setYPadding(false);
#endif// PP: def INGAME_CHEATS_MENU

#if(!((defined CONSUMERDEMO) && (BPLATFORM == PS2)))
	// NH: Add Menu Items (Exit)
	pagePtr->insertItem(ptr->exitTb = new TEXTBOX(STR_EXIT_EXITGAME),true);
	ptr->exitTb->setAlignScale(PAUSEMENU_TEXT_ALIGN_SCALE)
		->setYPadding(false);
#endif// PP: (!((defined CONSUMERDEMO) && (BPLATFORM == PS2)))

	// NH: INFO
	// NH: Add Info Text Sub Book
	pagePtr->insertItem(ptr->infoSubBook=new BOOK("INFO SUB BOOK"));
	ptr->infoSubBook->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setSelectable(false);

	// NH: Add Info Text (Using Macro)
#define ADD_INFOSUBBOOK_PAGE(name, infoText) \
	subPage=ptr->infoSubBook->addPage(#name); \
	subPage->insertItem(infoText) \
		->setStyle(TS_info) \
		->setYAlign(PIFLAG_YALIGN_BOTTOM) \
		->setAlignScale(1.2f) \
		->setSelectable(false);

	// NH: Add all info pages (using the macro).  Seperate pages allow for fading between the actions
	ADD_INFOSUBBOOK_PAGE(MUSICVOLUME,STR_ADJUST_MUSIC_VOLUME);
	ADD_INFOSUBBOOK_PAGE(SFXVOLUME,STR_ADJUST_SFX_VOLUME);
#if(BPLATFORM == XBOX)
	ADD_INFOSUBBOOK_PAGE(VIBRATION,STR_XBOX_ADJUST_VIBRATION);
#else// PP: ps2
	ADD_INFOSUBBOOK_PAGE(VIBRATION,STR_ADJUST_VIBRATION);
#endif// PP: ps2
	ADD_INFOSUBBOOK_PAGE(SUBTITLES,STR_GO_SUBTITLES);
	ADD_INFOSUBBOOK_PAGE(CHEATS, STR_CHEATS_INFO);	
	ADD_INFOSUBBOOK_PAGE(FRONTEND_ON,STR_FRONTEND_ON);
	ADD_INFOSUBBOOK_PAGE(FRONTEND_OFF,STR_FRONTEND_OFF);
#if(!((defined CONSUMERDEMO) && (BPLATFORM == PS2)))
	ADD_INFOSUBBOOK_PAGE(QUIT,STR_PAUSE_QUIT_GAME);
	ADD_INFOSUBBOOK_PAGE(RETURNTOHUB,STR_PAUSE_RETURNTOHUB_ACTION);
#endif// PP: (!((defined CONSUMERDEMO) && (BPLATFORM == PS2)))

	// NH: CONTROL
	// NH: Add Control Text Sub Book
	pagePtr->insertItem(ptr->controlSubBook=new BOOK("CONTROL SUB BOOK"));
	ptr->controlSubBook->setYPadding(false)
		->setSelectable(false);

	// NH: Add Control Text (Using Macro)
#define ADD_CONTROLSUBBOOK_PAGE(name, controlText) \
	subPage=ptr->controlSubBook->addPage(#name); \
	subPage->insertItem(controlText) \
		->setStyle(TS_control) \
		->setYPadding(false) \
		->setSelectable(false);

	// NH: Add all control pages (using the macro).  Seperate pages allow for fading between the actions
	ADD_CONTROLSUBBOOK_PAGE(RESUMESELECTORBACK,STR_RESUME_SELECT_OR_BACK);

	ptr->infoSubBook->open();// PP: books default to closed
	ptr->controlSubBook->open();

	ptr->page->selectItem(ptr->musicVolTb);
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdatePauseMenuPage
	Purpose 	: i'm not going to even bother with this one, work it out yourself
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void UpdatePauseMenuPage(PAUSEMENU *menu, PADCONTROLSTATUS *controller)
{
	PAUSEMENUPAGE	*ptr = menu->pauseMenuPage;

	if (CANCELBUTTON)
	{
		menu->pauseMenuBook->gotoPage(pauseMenuNames[PAUSE_MENU]);
		menu->currentPage = PAUSE_MENU;
	}
	else if (controller->startDebounceChannel->value)
	{
		SetGameState(&gameStatus,GAMESTATE_INGAME,GAMESUBSTATE_NORMAL);
		ExitPauseMode();

		// PP: ExitPauseMenu shuts-down the pause book.  Return here to avoid lines that use the pause book.
		return;
	}
	else if (ptr->page->selectedItem == ptr->mapTb)
	{
		if (OKBUTTON_NOTSTART)
		{
			menu->pauseMenuBook->gotoPage(pauseMenuNames[PAUSE_MAP]);
			menu->currentPage = PAUSE_MAP;

			// PP: override stereoscopy in map screen
			overrideStereoscopy(true);
		}
		ptr->infoSubBook->gotoPage("MAP");
	}
	else if (ptr->page->selectedItem == ptr->statsTb)
	{
		if (OKBUTTON_NOTSTART)
		{
			menu->pauseMenuBook->gotoPage(pauseMenuNames[PAUSE_STATS]);

			gameStatus.player[0].stats->endOfLevelStatsBook->open();

			menu->currentPage = PAUSE_STATS;
		}
		ptr->infoSubBook->gotoPage("STATS");
	}
	else if (ptr->page->selectedItem == ptr->optionsTb)
	{
		if (OKBUTTON_NOTSTART)
		{
			menu->pauseMenuBook->gotoPage(pauseMenuNames[PAUSE_OPTIONS]);
			menu->currentPage = PAUSE_OPTIONS;
			pauseMenu.pauseOptionsPage->page->selectItem(pauseMenu.pauseOptionsPage->musicVolTb);
			pauseMenu.pauseMenuBook->selectItem(pauseMenu.pauseOptionsPage->musicVolTb);
		}
		ptr->infoSubBook->gotoPage("OPTIONS");
	}

	ptr->controlSubBook->gotoPage("RESUMEORSELECT");
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdatePauseMapPage
	Purpose 	: i'm not going to even bother with this one, work it out yourself
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void UpdatePauseMapPage(PAUSEMENU *menu, PADCONTROLSTATUS *controller)
{
	PAUSEMAPPAGE	*ptr = menu->pauseMapPage;

	// PP: UNREMOUT: these are ORT books and as such are not on the book list (yet)
	if (hintBook)
		hintBook->privateUpdate();
	if (controlBook)
		controlBook->privateUpdate();

 	if (map.pauseMap)
	{
//#if BPLATFORM != PC
		if (gameStatus.currentState != GAMESTATE_CAMERACASE && !hintFlag)// && !ShowFlyBy)
		{
			map.pauseMap->Update();
		}

		if (hintFlag)
		{
			UpdateMapHint();

			if (controller1.squareDebounceChannel->value || (mouse.active && controller1.mb1DebounceChannel->value && !ShowFlyBy))
			{
				StartMapHint(0);
				controller1.squareDebounceChannel->value = controller1.mb1DebounceChannel->value = 0;
			}

			if (CANCELBUTTON)
			{
				ShowFlyBy = true;
				StopMapHint();

				// PP: Force-debounce the cancel button(s)
				// PP: (should be PAGE::debounceMenuActionBack, called from PAGE::respondToNavigation)
				FORCE_CANCELBUTTON_DEBOUNCE;

				controller1.triangleDebounceChannel->value = controller1.mb2DebounceChannel->value = 0;
			}
		}

		// IH: This has to go in here due to the render target using the main screen buffer
#if BPLATFORM!=GAMECUBE
		map.pauseMap->Draw(gameStatus.player[0].actorInstance->actorInstance.position);
#endif

		// IH: Calculate the hint and render to the render target, but don't draw it
#if BPLATFORM==GAMECUBE
		if (gameStatus.currentState != GAMESTATE_CAMERACASE && hintFlag)
			DrawMapHint();
#endif
		UpdateMapControls();
		DrawMapControls();

		// IH: Draw the hint normally on every platform but GameCube
#if BPLATFORM!=GAMECUBE
		if (gameStatus.currentState != GAMESTATE_CAMERACASE && hintFlag)
			DrawMapHint();
#endif

		// IH: On GameCube we have already prepped the render target, so draw it now
#if BPLATFORM==GAMECUBE
		if (gameStatus.currentState != GAMESTATE_CAMERACASE && hintFlag)
			DrawPostit();
#endif

		if (ShowFlyBy)
			NowShowAPostIt = false;
	}
	
	// NH: Just in the map.  Nothing special happening
	if (gameStatus.currentState != GAMESTATE_CAMERACASE)
	{
		if (CANCELBUTTON)
		{
			if (pauseMenu.enteredMapUsingShortCut)
			{
				// TP: go straight back to game using back button if we enter the map using the shortcut
				biForceDebounce(controller->startDebounceChannel);
				biForceDebounce(controller->selectDebounceChannel);
				
				ShowFlyBy = true;
				SetGameState(&gameStatus,GAMESTATE_INGAME,GAMESUBSTATE_NORMAL);
				ExitPauseMode();

				// PP: finish overriding stereoscopy on leaving map screen
				overrideStereoscopy(false);

				// PP: ExitPauseMenu shuts-down the pause book.  Return here to avoid lines that use the pause book.
				return;
			}
			else
			{
				ShowFlyBy = true;
				menu->pauseMenuBook->gotoPage(pauseMenuNames[PAUSE_STATS]);
				menu->currentPage = PAUSE_STATS;

				// PP: finish overriding stereoscopy on leaving map screen
				overrideStereoscopy(false);

				gameStatus.player[0].stats->endOfLevelStatsBook->open();
				biForceDebounce(controller->startDebounceChannel);
				biForceDebounce(controller->selectDebounceChannel);
			}
		}
		else if (/*TP:Not any more it doesn't :controller->startDebounceChannel->value ||*/ controller->selectDebounceChannel->value)
		{
			biForceDebounce(controller->startDebounceChannel);
			biForceDebounce(controller->selectDebounceChannel);
			
			ShowFlyBy = true;
			SetGameState(&gameStatus,GAMESTATE_INGAME,GAMESUBSTATE_NORMAL);
			ExitPauseMode();

			// PP: finish overriding stereoscopy on leaving map screen
			overrideStereoscopy(false);

			// PP: ExitPauseMenu shuts-down the pause book.  Return here to avoid lines that use the pause book.
			return;
		}
	}	

	ptr->infoSubBook->gotoPage("TEST");
	ptr->controlSubBook->gotoPage("RESUMEORBACK");
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdatePauseStatsPage
	Purpose 	: i'm not going to even bother with this one, work it out yourself
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void UpdatePauseStatsPage(PAUSEMENU *menu, PADCONTROLSTATUS *controller)
{
	PAUSESTATSPAGE	*ptr = menu->pauseStatsPage;

	char	tempString[16];

	/*if (gameStatus.player[0].stats->endOfLevelStatsBook)
		gameStatus.player[0].stats->endOfLevelStatsBook->open();*/

	//if (CANCELBUTTON)
	//{
	//	gameStatus.player[0].stats->endOfLevelStatsBook->close();
	//	menu->pauseMenuBook->gotoPage(pauseMenuNames[PAUSE_MENU]);
	//	menu->currentPage = PAUSE_MENU;
	//}

	//utilGetPrettyTime(tempString, gameStatus.gameTime);
	
	//gameStatus.player[0].stats->gameTimeScorebox->sprintf8(" %s",tempString);

	if ((controller->startDebounceChannel->value)
#if BPLATFORM == PC
		|| (CANCELBUTTON)
#endif
)
	{
		if (gameStatus.player[0].stats->endOfLevelStatsBook)
			gameStatus.player[0].stats->endOfLevelStatsBook->close();

		SetGameState(&gameStatus,GAMESTATE_INGAME,GAMESUBSTATE_NORMAL);
		ExitPauseMode();

		// PP: ExitPauseMenu shuts-down the pause book.  Return here to avoid lines that use the pause book.
		return;
	}
	else if (ptr->page->selectedItem == ptr->mapTb)
	{
		if (OKBUTTON_NOTSTART)
		{
			if (gameStatus.player[0].stats->endOfLevelStatsBook)
				gameStatus.player[0].stats->endOfLevelStatsBook->close();

			menu->pauseMenuBook->gotoPage(pauseMenuNames[PAUSE_MAP]);
			menu->currentPage = PAUSE_MAP;

			// PP: override stereoscopy in map screen
			overrideStereoscopy(true);
		}
		ptr->infoSubBook->gotoPage("MAP");
	}
	else if (ptr->page->selectedItem == ptr->optionsTb)
	{
		if (OKBUTTON_NOTSTART)
		{
			if (gameStatus.player[0].stats->endOfLevelStatsBook)
				gameStatus.player[0].stats->endOfLevelStatsBook->close();

			menu->pauseMenuBook->gotoPage(pauseMenuNames[PAUSE_OPTIONS]);
			menu->currentPage = PAUSE_OPTIONS;
			pauseMenu.pauseOptionsPage->page->selectItem(pauseMenu.pauseOptionsPage->musicVolTb);
			pauseMenu.pauseMenuBook->selectItem(pauseMenu.pauseOptionsPage->musicVolTb);
		}
		ptr->infoSubBook->gotoPage("OPTIONS");
	}

	ptr->infoSubBook->gotoPage("TEST");
	ptr->controlSubBook->gotoPage("RESUMEORBACK");
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdatePauseOptionsPage
	Purpose 	: i'm not going to even bother with this one, work it out yourself
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

int	rumbleHand = 0;

void UpdatePauseOptionsPage(PAUSEMENU *menu, PADCONTROLSTATUS *controller)
{
	static int storedVolume;
	int retval;
	PAUSEOPTIONSPAGE	*ptr = menu->pauseOptionsPage;

	// NH: 'Grey out' vibration option if not available
#if BPLATFORM == PS2
	switch(biQueryDevice(controller->port, NULL))
	{
	case EBIDEVICETYPE_PSDUALSHOCK:
	case EBIDEVICETYPE_PS2DUALSHOCK2:
		//rumble option ok
		ptr->vibrationTb->enable(true);
		ptr->vibrationTb->setSelectable(true);
		break;
	default:
		//grey out rumble
		if (ptr->page->selectedItem == ptr->vibrationTb)
		{
			ptr->page->selectItem(ptr->sfxVolTb);
		}
		ptr->vibrationTb->enable(false);
		ptr->vibrationTb->setSelectable(false);

		break;
	}
#endif

	// NH: If popup open
	if (menu->pausePopUp)
	{
		// PP: get player's response to the popup
		retval = menu->pausePopUp->getResponse();

		if ((CANCELBUTTON || OKBUTTON_NOTSTART) && (retval!=-1))
		{
			// NH: Close pop up
			BookList_closeAndFree(menu->pausePopUp);
			menu->pauseMenuBook->getPage(pauseMenuNames[PAUSE_OPTIONS])->setNavigation(true);
			menu->pauseOptionsPage->infoSubBook->open();
			menu->pauseOptionsPage->controlSubBook->open();
			
			if (currentPausePopup == PAUSE_POPUP_VIBRATION)
			{
				switch (retval)
				{
				case 1:
					FlushRumbleQueueMember(&controller1, rumbleHand);
					UnPauseRumble(&controller1);

					rumbleHand = RequestRumbleEffect(&controller1,255,0.0f,0.5f,1.0f,0.0f,WAVEFORM_NORMAL,0.0f,0.0f,0);
					gameStatus.globalSettings.vibration = true;		 

					break;
				case 2:
					FlushRumbleQueueMember(&controller1, rumbleHand);
					gameStatus.globalSettings.vibration = false;
					break;			
				}
			}
			else if (currentPausePopup == PAUSE_POPUP_SUBTITLES)
			{
				switch (retval)
				{
				case 1:
					gameStatus.globalSettings.subtitles = true;
					break;
				case 2:
					gameStatus.globalSettings.subtitles = false;
					break;
				}
			}
			else if (currentPausePopup == PAUSE_POPUP_EXIT)
			{
				switch (retval)
				{
				case 1:

					// PP: fade out and go to outro blurb screens
					BookList_closeAll();

#ifdef CONSUMERDEMO
					{
						// PP: NEW: prevent multi-popup hoy as in the non-demo block
						SetGameState(&gameStatus,GAMESTATE_INGAME,GAMESUBSTATE_NORMAL);
						ExitPauseMode();

						// PP: fade out and go to outro blurb screens
						//BookList_closeAll();
						camera[0].fade.StartFadeDown(FADETYPE_NORMAL, 1.0f);
						camera[0].fade.SetFadeDownFinishedCallback(StopDemoGameFadeDownCallback);
					}
					break;
#else// PP: not consumer demo
					SetGameState(&gameStatus,GAMESTATE_INGAME,GAMESUBSTATE_NORMAL);

					StopAllChannels();
					ExitPauseMode(FALSE);

					// TP: Exit to correct place
					int levelnum = LevelForExit(&map.startPos);

					if ((levelnum==SCENE_TAZHUB)&&(map.sceneNumber==SCENE_TAZPREBOSS))
					{
						camera[0].fade.FadeAndChangeScene(&gameStatus,FADETYPE_NORMAL,levelnum,1.0f,bkCRC32((uchar*)"tazboss1door", strlen("tazboss1door"), 0));
					}
					else if ((levelnum==SCENE_TAZHUB)&&(map.sceneNumber==SCENE_TAZBOSS))
					{
						camera[0].fade.FadeAndChangeScene(&gameStatus,FADETYPE_NORMAL,levelnum,1.0f,bkCRC32((uchar*)"tazboss2door", strlen("tazboss2door"), 0));
					}
					else
					{
						camera[0].fade.FadeAndChangeScene(&gameStatus,FADETYPE_NORMAL,levelnum,1.0f);
					}
					SetGameState(&gameStatus,GAMESTATE_FADING,GAMESUBSTATE_NORMAL);
					break;
#endif// PP: not consumer demo
				}
			}
			else if (currentPausePopup == PAUSE_POPUP_MUSICVOLUME)
			{
				// NH: Restore Old volume
				if (CANCELBUTTON)
				{
					gameStatus.globalSettings.musicVolume = storedVolume;
					bsSetGlobalAudioStreamVolume(gameStatus.globalSettings.musicVolume);
				}				
			}
			else if (currentPausePopup == PAUSE_POPUP_SFXVOLUME)
			{
				// NH: Restore Old volume
				if (CANCELBUTTON)
				{
					gameStatus.globalSettings.sfxVolume = storedVolume;		 
					bsSetGlobalSampleVolume(gameStatus.globalSettings.sfxVolume);
				}				
			}
		}
	}
	else if (CANCELBUTTON)
	{
		menu->pauseMenuBook->gotoPage(pauseMenuNames[PAUSE_STATS]);

		if(map.sceneNumber == SCENE_ZOOHUB || map.sceneNumber == SCENE_CITYHUB || map.sceneNumber == SCENE_WILDWESTHUB || map.sceneNumber == SCENE_CITYHUB ||
		   map.sceneNumber == SCENE_ZOOBOSS || map.sceneNumber == SCENE_CITYBOSS || map.sceneNumber == SCENE_WESTBOSS || map.sceneNumber == SCENE_TAZPREBOSS || map.sceneNumber == SCENE_TAZBOSS ||
		   map.sceneNumber == SCENE_MUSEUMVEHICLERACE || map.sceneNumber == SCENE_GRANDCVEHICLERACE || map.sceneNumber == SCENE_DEPTSTRVEHICLERACE ||
		   map.sceneNumber == SCENE_DESTRUCTIONAQUA || map.sceneNumber == SCENE_DESTRUCTIONCONSTRUCT || map.sceneNumber == SCENE_DESTRUCTIONTASMANIA ||
		   map.sceneNumber == SCENE_ICEDOMERACE || map.sceneNumber == SCENE_GOLDMINERACE || map.sceneNumber == SCENE_SAFARIRACE ||
		   gameStatus.player[0].actorInstance->flags & ACTORFLAG_DONTCHECKBOUNDINGBOX)
		{}
		else
		{	gameStatus.player[0].stats->endOfLevelStatsBook->open();	}

		menu->currentPage = PAUSE_STATS;
		pauseMenu.pauseStatsPage->page->selectItem(pauseMenu.pauseStatsPage->optionsTb);
		pauseMenu.pauseMenuBook->selectItem(pauseMenu.pauseStatsPage->optionsTb);		
	}
	else if (controller->startDebounceChannel->value)
	{
		SetGameState(&gameStatus,GAMESTATE_INGAME,GAMESUBSTATE_NORMAL);
		ExitPauseMode();

		// PP: ExitPauseMenu shuts-down the pause book.  Return here to avoid lines that use the pause book.
		return;
	}
	// NH: Selected items routines
	// NH: Music Vol Selected
	else if (ptr->page->selectedItem == ptr->musicVolTb)
	{
		if (OKBUTTON_NOTSTART)
		{
			if (!pauseMenu.pausePopUp)
			{
				storedVolume = gameStatus.globalSettings.musicVolume;
				currentPausePopup = PAUSE_POPUP_MUSICVOLUME;
				menu->pauseMenuBook->getPage(pauseMenuNames[PAUSE_OPTIONS])->setNavigation(false);
				ptr->infoSubBook->close();
				ptr->controlSubBook->close();

				pauseMenu.pausePopUp = new POPUP(POPUP_MUSICVOLUME, 0);
			}
		}
		ptr->infoSubBook->gotoPage("MUSICVOLUME");
	}
	// NH: SFX Vol Selected
	else if (ptr->page->selectedItem == ptr->sfxVolTb)
	{
		if (OKBUTTON_NOTSTART)
		{
			if (!pauseMenu.pausePopUp)
			{
				storedVolume = gameStatus.globalSettings.sfxVolume;
				currentPausePopup = PAUSE_POPUP_SFXVOLUME;
				menu->pauseMenuBook->getPage(pauseMenuNames[PAUSE_OPTIONS])->setNavigation(false);
				ptr->infoSubBook->close();
				ptr->controlSubBook->close();

				pauseMenu.pausePopUp = new POPUP(POPUP_SFXVOLUME, 0);
			}
		}
		ptr->infoSubBook->gotoPage("SFXVOLUME");
	}
	// NH: Vibration Selected
	else if (ptr->page->selectedItem == ptr->vibrationTb)
	{
		if (OKBUTTON_NOTSTART)
		{
			currentPausePopup = PAUSE_POPUP_VIBRATION;
			menu->pauseMenuBook->getPage(pauseMenuNames[PAUSE_OPTIONS])->setNavigation(false);
			ptr->infoSubBook->close();
			ptr->controlSubBook->close();

			menu->pausePopUp = new POPUP(POPUP_ON_OFF, gameStatus.globalSettings.vibration);
		}

		ptr->infoSubBook->gotoPage("VIBRATION");
	}
	// NH: Subtitles Selected
	else if (ptr->page->selectedItem == ptr->subtitlesTb)
	{
		if (OKBUTTON_NOTSTART)
		{
			currentPausePopup = PAUSE_POPUP_SUBTITLES;
			menu->pauseMenuBook->getPage(pauseMenuNames[PAUSE_OPTIONS])->setNavigation(false);
			ptr->infoSubBook->close();
			ptr->controlSubBook->close();

			menu->pausePopUp = new POPUP(POPUP_ON_OFF, gameStatus.globalSettings.subtitles);
		}

		ptr->infoSubBook->gotoPage("SUBTITLES");
	}

	// NH: Exit game to frontend
	else if (ptr->page->selectedItem == ptr->exitTb)
	{
		if (OKBUTTON_NOTSTART)
		{
			currentPausePopup = PAUSE_POPUP_EXIT;
			menu->pauseMenuBook->getPage(pauseMenuNames[PAUSE_OPTIONS])->setNavigation(false);
			ptr->infoSubBook->close();
			ptr->controlSubBook->close();

			pauseMenu.pausePopUp = new POPUP(POPUP_CONFIRM, 0);
/*			SetGameState(&gameStatus,GAMESTATE_INGAME,GAMESUBSTATE_NORMAL);
			ExitPauseMode();
			FadeAndChangeScene(&camera[0].fade,&gameStatus,FADETYPE_NORMAL,SCENE_FRONTEND,1.0f);
			SetGameState(&gameStatus,GAMESTATE_FADING,GAMESUBSTATE_NORMAL);

//			bkGenerateEvent("scenechange","_frontend");

			// PP: ExitPauseMenu shuts-down the pause book.  Return here to avoid lines that use the pause book.
		
			return;
		*/
		}

		// TP: choose correct message
		if (LevelForExit()==SCENE_FRONTEND)
		{
			ptr->infoSubBook->gotoPage("QUIT");
		}
		else
		{
			ptr->infoSubBook->gotoPage("RETURNTOHUB");
		}
	}
	ptr->controlSubBook->gotoPage("RESUMESELECTORBACK");
}

void UpdateMPPausePage(PADCONTROLSTATUS *controller)
{
	int	retval = 0;

	UpdateRumble(&controller1);
	UpdateRumble(&controller2);

	if (mppausePage.pausePopUp)
	{
		// PP: get player's response to the popup
		retval = mppausePage.pausePopUp->getResponse();

		if (/* PP: REMOUT: not needed	(CANCELBUTTON || OKBUTTON_NOTSTART)&&*/
		(retval != -1))
		{
			// NH: Close pop up

			// PP: Force-debounce cancel button(s)
			FORCE_CANCELBUTTON_DEBOUNCE;

			// PP: Force-debounce ok button(s)
			FORCE_OKBUTTON_DEBOUNCE;

			BookList_closeAndFree(mppausePage.pausePopUp);

			switch (retval)
			{
			case 1:
#ifdef CONSUMERDEMO
				{
					// PP: NEW TEST - to prevent multipopup hoy-hoy
					ExitMPPauseMode();

					// PP: fade out and go to outro blurb screens

					// PP: close the demo timer book as we fade out
					//demoTimerBook->close();
					BookList_closeAll();
					camera[0].fade.StartFadeDown(FADETYPE_NORMAL, 1.0f);
					camera[1].fade.StartFadeDown(FADETYPE_NORMAL, 1.0f);

					camera[0].fade.SetFadeDownFinishedCallback(StopDemoGameFadeDownCallback);
				}
				break;
#else// PP: not consumer demo
				{
					SetGameState(&gameStatus,GAMESTATE_INGAME,GAMESUBSTATE_NORMAL);

					gameStatus.multiplayer.numPlayers = 1;
					gameStatus.multiplayer.Tournament = 0;
					//gameStatus.multiplayer.versusCPU = true;
					//setNumScreens(1);

					StopAllChannels();

					camera[0].fade.FadeAndChangeScene(&gameStatus,FADETYPE_NORMAL,SCENE_FRONTEND,1.0f);

					SetGameState(&gameStatus,GAMESTATE_FADING,GAMESUBSTATE_NORMAL);

					ExitMPPauseMode();
				}
				break;
#endif// PP: not consumer demo

			default:	mpPauseBook->getPage("mp pause")->setNavigation(true);
						mpPauseBook->gotoPage("mp pause");

						mppausePage.infoSubBook->open();
						mppausePage.controlSubBook->open();

						break;
			}
		}
	}
	else
	{
		if (controller->startDebounceChannel->value	&& PADSCONNECTED)
		{
			SetGameState(&gameStatus,GAMESTATE_INGAME,GAMESUBSTATE_NORMAL);
			ExitMPPauseMode();

			// PP: ExitPauseMenu shuts-down the pause book.  Return here to avoid lines that use the pause book.
			return;
		}
		else if (mppausePage.page->selectedItem == mppausePage.resumeTb)
		{
			// PP: NOTE: playerPause is 1-based
			if ((OKBUTTON_NOTSTART&&(gameStatus.multiplayer.playerPause == 1))
				||(OKBUTTON2_NOTSTART&&(gameStatus.multiplayer.playerPause == 2)))
			{
				SetGameState(&gameStatus,GAMESTATE_INGAME,GAMESUBSTATE_NORMAL);
				ExitMPPauseMode();

				// PP: ExitPauseMenu shuts-down the pause book.  Return here to avoid lines that use the pause book.
				return;
			}
			mppausePage.infoSubBook->gotoPage("RESUMEMULTIPLAYER");
		}
		else if (mppausePage.page->selectedItem == mppausePage.quitTb)
		{
			// PP: NOTE: playerPause is 1-based
			if ((OKBUTTON_NOTSTART&&(gameStatus.multiplayer.playerPause == 1))
				||(OKBUTTON2_NOTSTART&&(gameStatus.multiplayer.playerPause == 2)))
			{
				mpPauseBook->getPage("mp pause")->setNavigation(false);

				mpPauseBook->gotoPage("quit");
				
				mppausePage.infoSubBook->close();
				mppausePage.controlSubBook->close();

				mppausePage.pausePopUp = new POPUP(POPUP_CONFIRM, 0, STR_ARE_YOU_SURE, STR_DIALOGUE_YES, STR_DIALOGUE_NO, STR_X_TO_SELECT_Y_TO_RETURN, controller->padID);

				// PP: Force-debounce ok button(s)
				FORCE_OKBUTTON_DEBOUNCE;
			}
			mppausePage.infoSubBook->gotoPage("QUITMULTIPLAYER");
		}

		mppausePage.controlSubBook->gotoPage("RESUMEORSELECT");
	}
}

/* --------------------------------------------------------------------------------
   Function		: DefaultPauseUpdateFunc
   Purpose		: update function used for default pause
   Parameters	: context value, required but currently not used
   Returns		: 
   Info			: // PP: this is the one that gets called as a result of setting GAMESTATE_PAUSE
					// PP: Must match the PauseUpdateFunc typedef (main.h)
*/
void DefaultPauseUpdateFunc(const int32 context)
{
	PAGE	*pagePtr;

	TBSavedRenderStates storedRenderState;

	// PP: request subdued music
	musicSystem.RequestMood(0,STREAMMOOD_SLOW);

#ifdef PHIL
	UpdatePhilScene(map.sceneNumber);
#endif// PP: def PHIL

	if(!pauseMenu.pauseMenuBook) return;

#ifdef CONSUMERDEMO								// JW: No activity on the pause for 60 seconds? sling yer hook!
	if (controller1.idleTime > to_inactive)
	{
		ExitPauseMode();

		camera[0].fade.StartFadeDown(FADETYPE_NORMAL, 1.0f);
		
		camera[0].fade.SetFadeDownFinishedCallback(StopDemoGameFadeDownCallback);

		return;
	}
#endif

	/*if (pauseMenu.currentPage != PAUSE_MAP)
	{
		pauseMenu.pauseMenuBook->setBackgroundColour(COLOUR(0,0,0,BACKGROUNDALPHA));
		pauseMenu.pauseMenuBook->setBorderColour(COLOUR(0,0,255,255));
	}
	else
	{*/
		pauseMenu.pauseMenuBook->setBackgroundColour(COLOUR(0,0,0,0));
		pauseMenu.pauseMenuBook->setBorderColour(COLOUR(0,0,0,0));
	//}

	UpdateRumble(&controller1);
	UpdateRumble(&controller2);

	switch(pauseMenu.currentPage)
	{
	case PAUSE_MENU:
		UpdatePauseMenuPage(&pauseMenu, &controller1);
		break;
	case PAUSE_MAP:
		if (hintBook)
			UpdatePauseMapPage(&pauseMenu, &controller1);
		break;
	case PAUSE_STATS:
		UpdatePauseStatsPage(&pauseMenu, &controller1);
		break;
	case PAUSE_OPTIONS:
		UpdatePauseOptionsPage(&pauseMenu, &controller1);
		break;
	}

	// PP: moved this here from main.cpp where it was making a nuisance of itself...
	// PP: ~~~~~~~

	// check for unpause
	if (pauseMenu.currentPage!=PAUSE_MAP)
	{
		// TP: Don't use start to exit map
		if (gameStatus.multiplayer.versusCPU)
		{
			if ((controller1.startDebounceChannel->value)&&(PADSCONNECTED)&&(gameStatus.currentState != GAMESTATE_CAMERACASE))
			{
				ExitPauseMode();
				SetGameState(&gameStatus,GAMESTATE_INGAME,GAMESUBSTATE_NORMAL);
			}
		}
		else
		{
			if ((controller1.startDebounceChannel->value)&&(PADSCONNECTED)&&(gameStatus.multiplayer.playerPause == 1))
			{
				ExitMPPauseMode();
				SetGameState(&gameStatus,GAMESTATE_INGAME,GAMESUBSTATE_NORMAL);
			}
			else
			if ((controller2.startDebounceChannel->value)&&(PADSCONNECTED)&&(gameStatus.multiplayer.playerPause == 2))
			{
				ExitMPPauseMode();
				SetGameState(&gameStatus,GAMESTATE_INGAME,GAMESUBSTATE_NORMAL);
			}
		}
	}
}

void DefaultMPPauseUpdateFunc(const int32 context)
{
	PAGE	*pagePtr;

	if(!mpPauseBook) return;

	/*if (pauseMenu.currentPage != PAUSE_MAP)
	{
		pauseMenu.pauseMenuBook->setBackgroundColour(COLOUR(0,0,0,BACKGROUNDALPHA));
		pauseMenu.pauseMenuBook->setBorderColour(COLOUR(0,0,255,255));
	}
	else
	{*/
		mpPauseBook->setBackgroundColour(COLOUR(0,0,0,0));
		mpPauseBook->setBorderColour(COLOUR(0,0,0,0));
	//}

	if(map.sceneNumber == SCENE_ZOOBOSS)
	{
		ZooBoss_game.prepareForPause();
	}

#ifdef CONSUMERDEMO
	if (controller1.idleTime > to_inactive && controller2.idleTime > to_inactive)
	{
		camera[0].fade.StartFadeDown(FADETYPE_NORMAL, 1.0f);

		camera[0].fade.SetFadeDownFinishedCallback(StopDemoGameFadeDownCallback);

		camera[1].fade.StartFadeDown(FADETYPE_NORMAL, 1.0f);

		ExitMPPauseMode();

		return;
	}
#endif

#if BPLATFORM != PC
	if (gameStatus.multiplayer.playerPause == 1)
		UpdateMPPausePage(&controller1);
	else if (gameStatus.multiplayer.playerPause == 2)
		UpdateMPPausePage(&controller2);
#else
	UpdateMPPausePage(&controller1);
#endif
}


/*  --------------------------------------------------------------------------------
	Function	: GenericPauseModePrepare
	Purpose		: generic preparations for entering any sort of pause mode
	Parameters	: 
	Returns		: 
	Info		: // PP: pauses sound and rumbles, subdues music, etc.
					// PP: Separated this off from ExitPauseMode, to allow for other pause modes
							such as when a controller is removed.
*/

// PP: duration of mood change as we enter the pause mode
#define ENTERPAUSEMODE_MUSICMOODCHANGE_FADETIME		MUSIC_NO_SPECIAL_FADE_TIME// PP: 12.0f

void GenericPauseModePrepare(void)
{
	bsPauseAllChannels(BSPAUSE);

	// PP: Pause pad vibration
	PauseRumble(&controller1);
	PauseRumble(&controller2);
	// CMD: no need to update the rumble controller anymore

	if (characterSounds.tweety == -1)
		CallbackWhenSampleFinished((characterSounds.sam=PlaySample("tweetypause_1.wav",200)),CharacterSoundsFinishedCallback,&characterSounds.sam);	

	// PP: ~~~

	// PP: get a record of the music mood in effect as the pause mode was entered, so that we can return to that mood when we exit the pause mode
	musicMoodBeforePause=musicSystem.GetMood();

	// PP: subdued music while paused
	musicSystem.RequestMood(0,STREAMMOOD_SLOW, ENTERPAUSEMODE_MUSICMOODCHANGE_FADETIME);

	// PP: ~~~
}


/*  --------------------------------------------------------------------------------
	Function	: EnterPauseMode
	Purpose		: Called when the player enters the pause mode, init pause book etc.
	Parameters	: index of page on which to open the pause menu book, or PAUSEPAGE_NONE not to open the pause menu book (eg. using controller-missing book instead)
	Returns		: 
	Info		: 
*/
void EnterPauseMode(int	page)
{
	// TP: Did we enter pause using the shortcut
	if (page==PAUSE_MAP)
	{
		pauseMenu.enteredMapUsingShortCut = true;
	}
	else
	{
		pauseMenu.enteredMapUsingShortCut = false;
	}
	// PP: do stuff that needs to be done whenever any kind of pause mode is entered
	GenericPauseModePrepare();

#ifndef CONSUMERDEMO
	// PP: Let me explain this nasty special-case bodge.
	// PP: When the pause menu opens, it clashes with the "SCARE!" prompt in the Zoo Boss game.
	// PP: If there are a lot of similar cases throughout the game, I'll finish off the 'Exclusive Book' stuff which will
	// PP: fix them all in one fell swoop.  Otherwise, I'll just deal with it case-by-case.
	// PP: The question is simply 'do you want the quick fix or the neat fix?'
	if(map.sceneNumber == SCENE_ZOOBOSS)
	{
		ZooBoss_game.prepareForPause();
	}
#endif

	BOOK	*bookPtr;

	hintFlag = 0;

	if (raceBook)
	{
		raceBook->close();
		mpTimerFlash = false;
	}
	// CMD: always close enter level book
	enterLevelBook.close();

	if(levelData.oneBook)
	{
		levelData.oneBook->close();
	}

	if(levelData.twoBook)
	{
		levelData.twoBook->close();
	}

	if (gameStatus.player1->actorStatus->multiplayer.pStatsBook)
		gameStatus.player1->actorStatus->multiplayer.pStatsBook->close();
	
	hintBook = new BOOK;// PP: SAFE_DELETEd in ExitPauseMode
	controlBook = new BOOK;// PP: SAFE_DELETEd in KillMapControls, from ExitPauseMode

	hintBook->flags &= ~(BKFLAG_SCREENADJUST|BKFLAG_BOOKLIST);
	controlBook->flags &= ~(BKFLAG_SCREENADJUST|BKFLAG_BOOKLIST);

	hintBook->setEffectChooser(&basePIEChooser);
	controlBook->setEffectChooser(&basePIEChooser);

	// JW: Load post-it note texture for hints
	hintTexture = bkLoadTexture(NULL, "postit.bmp", BDEFAULTGROUP);
	cntrlTexture = bkLoadTexture(NULL, "scrapaper.bmp", BDEFAULTGROUP);


#if BPLATFORM == PC
	InitMouse(&mouse);
	mouse.active = true;
#endif

	// PP: a bit misleadingly named now, this just seems to indicate that we're paused
	pauseMenuActive = true;

	if(page != PAUSEPAGE_NONE)
	{
		pauseMenu.currentPage = (enum EPauseMenuPage) page;
		
		if (pauseMenu.pauseOptionsPage->exitTb)
		{
			// TP: should exit return to hub or frontend, what a test!
			if (LevelForExit()==SCENE_FRONTEND)
			{
				// TP: change exit option to be exit game
				pauseMenu.pauseOptionsPage->exitTb->setText(STR_EXIT_EXITGAME);
			}
			else
			{
				// TP: change exit option to be exit to hub
				pauseMenu.pauseOptionsPage->exitTb->setText(STR_PAUSE_RETURNTOHUB);
			}
		}


		// NH: Disable Map & Stats if in Hub, Boss or Multiplayer
		if(map.sceneNumber == SCENE_ZOOHUB || map.sceneNumber == SCENE_CITYHUB || map.sceneNumber == SCENE_WILDWESTHUB || map.sceneNumber == SCENE_CITYHUB ||
		   map.sceneNumber == SCENE_ZOOBOSS || map.sceneNumber == SCENE_CITYBOSS || map.sceneNumber == SCENE_WESTBOSS || map.sceneNumber == SCENE_TAZPREBOSS || map.sceneNumber == SCENE_TAZBOSS ||
		   map.sceneNumber == SCENE_MUSEUMVEHICLERACE || map.sceneNumber == SCENE_GRANDCVEHICLERACE || map.sceneNumber == SCENE_DEPTSTRVEHICLERACE ||
		   map.sceneNumber == SCENE_DESTRUCTIONAQUA || map.sceneNumber == SCENE_DESTRUCTIONCONSTRUCT || map.sceneNumber == SCENE_DESTRUCTIONTASMANIA ||
		   map.sceneNumber == SCENE_ICEDOMERACE || map.sceneNumber == SCENE_GOLDMINERACE || map.sceneNumber == SCENE_SAFARIRACE ||
		   gameStatus.player[0].actorInstance->flags & ACTORFLAG_DONTCHECKBOUNDINGBOX)
		{
			pauseMenu.pauseStatsPage->mapTb->enable(false)
				->setSelectable(false);
//			pauseMenu.pauseStatsPage->statsTb->enable(false)
//				->setSelectable(false);
			pauseMenu.pauseStatsPage->page->selectItem(pauseMenu.pauseStatsPage->optionsTb);
			pauseMenu.pauseMenuBook->selectItem(pauseMenu.pauseStatsPage->optionsTb);
			bkPrintf("HUB/BOSS OR MULTIPLAYER (Hide Some Options)\n");

			if (gameStatus.multiplayer.numPlayers > 1)
				bkPrintf("  MULTIPLAYER\n");
			if (map.sceneNumber == SCENE_ZOOHUB)
				bkPrintf("  ZOOHUB\n");
			if (map.sceneNumber == SCENE_ZOOBOSS)
				bkPrintf("  ZOOBOSS\n");
			if (map.sceneNumber == SCENE_CITYHUB)
				bkPrintf("  SCENE_CITYHUB\n");
			if (map.sceneNumber == SCENE_CITYBOSS)
				bkPrintf("  SCENE_CITYBOSS\n");
			if (map.sceneNumber == SCENE_WILDWESTHUB)
				bkPrintf("  SCENE_WILDWESTHUB\n");
			if (map.sceneNumber == SCENE_WESTBOSS)
				bkPrintf("  SCENE_WESTBOSS\n");
			if (map.sceneNumber == SCENE_CITYHUB)
				bkPrintf("  SCENE_CITYHUB\n");
			if (map.sceneNumber == SCENE_TAZPREBOSS)
				bkPrintf("  SCENE_TAZPREBOSS\n");
			if (map.sceneNumber == SCENE_TAZBOSS)
				bkPrintf("  SCENE_TAZBOSS\n");
		}
		else
		{
			//CalculateGameTimeFormatted(gameStatus.gameTime,&gameStatus.player[0].stats->hours,&gameStatus.player[0].stats->minutes,&gameStatus.player[0].stats->seconds);

			gameStatus.player[0].stats->time = gameStatus.playTime;

			CreateStatsAtEndOfLevel(gameStatus.player[0].stats, 1);
			
			gameStatus.player[0].stats->endOfLevelStatsBook->setScale(0.7f);
			gameStatus.player[0].stats->endOfLevelStatsBook->setBookRect(RECTANGLE(-0.6f, 0.6f, -0.40f, 0.6f));

			if (page == 2)
				gameStatus.player[0].stats->endOfLevelStatsBook->open();

			pauseMenu.pauseStatsPage->mapTb->enable(true)
				->setSelectable(true);
			//pauseMenu.pauseMenuPage->statsTb->enable(true)
			//	->setSelectable(true);
			pauseMenu.pauseStatsPage->page->selectItem(pauseMenu.pauseStatsPage->mapTb);
			bkPrintf("NOT HUB/BOSS OR MULTIPLAYER (Show All Options)\n");
		}
		
		// PP: override stereoscopy on entering map screen
		if(pauseMenu.currentPage == PAUSE_MAP)
		{
			overrideStereoscopy(true);
		}

		//menu->pauseMenuBook->gotoPage(pauseMenuNames[PAUSE_STATS]);

		// PP: turn instantly to the specified page in the pause menu book
		pauseMenu.pauseMenuBook->gotoPage(pauseMenuNames[page], true);

		// PP: open the pause menu book
		pauseMenu.pauseMenuBook->open();

		InitMapControls();

		// NH: Redundent line? Doesn't seem to be used, but should it be?
		// NH: PLAYERSTATS	*stats = gameStatus.player[0].stats;
	}

	// PP: I put this here cos, well, it's a nice place for it no?
	SetGameState(&gameStatus,GAMESTATE_PAUSE,GAMESUBSTATE_NORMAL);

	closePromptBook();
	closeSubtitles();

	if (map.sceneNumber == SCENE_ZOOHUB)
		CloseTutInfBook();

// PP: REMOUT: 	if (gameStatus.multiplayer.versusCPU && map.sceneNumber == SCENE_DEPTSTR)
#ifdef CONSUMERDEMO// PP: this book will only be open in consumer demo
#ifndef STANDALONE
	if(demoTimerBook != NULL)
	{
		demoTimerBook->close();// PP: NOTE: this was remmed out, presumably for some reason - find out what it was
	}
	
#endif

	controller1.idleTime = 0.0f;

#endif// PP: def CONSUMERDEMO
}


/*  --------------------------------------------------------------------------------
	Function	: GenericPauseModeEnd
	Purpose		: generic stuff that has to be done when exiting any sort of pause mode
	Parameters	: 
	Returns		: 
	Info		: // PP: resumes sound and rumbles, restores music mood, etc.
					// PP: Separated this off from ExitPauseMode, to allow for other pause modes
							such as when a controller is removed.
*/

// PP: duration of mood change as we enter the pause mode
#define EXITPAUSEMODE_MUSICMOODCHANGE_FADETIME		MUSIC_NO_SPECIAL_FADE_TIME// PP: 1.0f

void GenericPauseModeEnd(void)
{
	// PP: Unpause sound
	bsPauseAllChannels(BSUNPAUSE);

	// PP: Unpause pad vibration
	UnPauseRumble(&controller1);
	UnPauseRumble(&controller2);

	// PP: ~~~

	// PP: return to music mood in effect as the pause mode was entered (if it was valid)
	if(musicMoodBeforePause != STREAMMOOD_NONE)
	{
		musicSystem.RequestMood(0,musicMoodBeforePause, EXITPAUSEMODE_MUSICMOODCHANGE_FADETIME);
	}
}


/*  --------------------------------------------------------------------------------
	Function	: ExitPauseMode
	Purpose		: Called when the player exits the pause mode, shutdown the pause book.
	Parameters	: 
	Returns		: 
	Info		: 
*/
void ExitPauseMode(char openBooks)
{
// PP: MILESTONE BODGE
/* PP: REMOUT: #if(BPLATFORM != XBOX)
	if (!pauseMenuActive) return;
#endif// PP: */

	// NH: Force the Pause key to be release before entering pause again.
	biForceDebounce(controller1.startDebounceChannel);

	// PP: do generic stuff that has to be done when exiting any sort of pause mode
	GenericPauseModeEnd();

	// NH: Close pop up
	if (pauseMenu.pausePopUp)
	{
		BookList_closeAndFree(pauseMenu.pausePopUp);
		pauseMenu.pauseMenuBook->getPage(pauseMenuNames[PAUSE_OPTIONS])->setNavigation(true);
		if(openBooks) pauseMenu.pauseOptionsPage->infoSubBook->open();
		if(openBooks) pauseMenu.pauseOptionsPage->controlSubBook->open();
	}
	// CMD: if hub stats were open before pause reopen them
	if((showHubStats)&&(openBooks)) enterLevelBook.open();

#if BPLATFORM == PC
	mouse.active = false;
#endif
	
	// TP: Can I keep this for now, it causes bad memory leaks if not
	// PP: REMOUT: BookList_closeAndFree should do the trick	SAFE_DELETE(gameStatus.player[0].stats->endOfLevelStatsBook);

	// PP: start the book closing and have it freed once it finishes closing
	BookList_closeAndFree(gameStatus.player[0].stats->endOfLevelStatsBook);

	if (pauseMenu.pauseMenuBook)
		pauseMenu.pauseMenuBook->close();// PP: REMOUT: testing		0.0f,TRUE);

	KillMapControls();

	pauseMenuActive = false;

	SAFE_DELETE(hintBook);

#ifndef FRIGTARGET

	if (hintFlag)
	{
		bdDeleteRenderTarget(hintTarget);
		hintTarget = 0;
		hintFlag = 0;
	}

#endif// PP: ndef FRIGTARGET

	if (gameStatus.pad1Present)
	{
		if ((camera[0].fade.GetSceneToLoad() != SCENE_FRONTEND) && (camera[0].fade.GetFadeState() != FADESTATE_DOWN))
		{
			if((raceBook)&&(openBooks)) raceBook->open();

			if(gameStatus.player1)
			{
				if(gameStatus.player1->actorStatus)
				{
					if((gameStatus.player1->actorStatus->multiplayer.pStatsBook)&&(openBooks))
						gameStatus.player1->actorStatus->multiplayer.pStatsBook->open();

					if(levelData.oneBook)
					{
						levelData.oneBook->close();
					}
					
					if(levelData.twoBook)
					{
						levelData.twoBook->close();
					}
				}
			}

			if((map.sceneNumber == SCENE_ZOOHUB)&&(openBooks))
				OpenTutInfBook();
		}
	}

	if(openBooks) openPromptBook();
	if(openBooks) openSubtitles();

#ifndef STANDALONE
	#ifdef CONSUMERDEMO
		if((gameStatus.multiplayer.versusCPU)&&(map.sceneNumber == SCENE_DEPTSTR)&&(controller1.idleTime < to_inactive)&&(openBooks))
			demoTimerBook->open();
	#endif
#endif

	if (gameStatus.pad1Present)
		gameStatus.multiplayer.playerPause = 0;
	
	controller1.idleTime = 0.0f;

	//bdDeleteTexture(hintTexture);
}


/*	--------------------------------------------------------------------------------
	Function 	: Pause_shutdown
	Purpose 	: shutdown stuff for the pause mode, eg. delete the pause menu book
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: 
*/
void Pause_shutdown(void)
{
	SAFE_DELETE(mpPauseBook);
	SAFE_DELETE(pauseMenu.pauseMenuBook);
	SAFE_DELETE(gameStatus.player[0].stats->endOfLevelStatsBook);

	SAFE_FREE(pauseMenu.pauseMenuPage);
	SAFE_FREE(pauseMenu.pauseMapPage);
	SAFE_FREE(pauseMenu.pauseStatsPage);
	SAFE_FREE(pauseMenu.pauseOptionsPage);

	SAFE_DELETE(postitVerts);
	SAFE_DELETE(shadowVerts);
}


/*	--------------------------------------------------------------------------------
	Function 	: VibrateCallback
	Purpose 	: callback function for the vibration toggle-control
	Parameters 	: the state shown by the toggle-control (0/1)
	Returns 	: 
	Info 		: called only when the control toggles
*/
static void VibrateCallback(const int state)
{
	gameStatus.globalSettings.vibration=state?false:true;
}

/*	--------------------------------------------------------------------------------
	Function 	: musicSliderUpdateFunc
	Purpose 	: callback function for the music volume slider
	Parameters 	: the value represented by the slider (0..1), ptr to slider
	Returns 	: 
	Info 		: called on every update of the slider, whether the value has changed or not
*/
void musicSliderUpdateFunc(const float sliderValue, SLIDER* const slider)
{
	gameStatus.globalSettings.musicVolume=(int)(sliderValue*MAX_MUSIC_VOLUME);
	bsSetGlobalAudioStreamVolume(gameStatus.globalSettings.musicVolume);
//	bsSetGlobalMusicVolume(gameStatus.globalSettings.musicVolume);
}

// TP: temp just so I don't have to get any new stuff yet, am doing disk.
void musicSliderUpdateFunc(const float sliderValue)
{
	gameStatus.globalSettings.musicVolume=(int)(sliderValue*MAX_MUSIC_VOLUME);
	bsSetGlobalAudioStreamVolume(gameStatus.globalSettings.musicVolume);
//	bsSetGlobalMusicVolume(gameStatus.globalSettings.musicVolume);
}

/*	--------------------------------------------------------------------------------
	Function 	: sfxSliderUpdateFunc
	Purpose 	: callback function for the music volume slider
	Parameters 	: the value represented by the slider (0..1), ptr to slider
	Returns 	: 
	Info 		: called on every update of the slider, whether the value has changed or not
*/
void sfxSliderUpdateFunc(const float sliderValue, SLIDER* const slider)
{
	gameStatus.globalSettings.sfxVolume=(int)(sliderValue*MAX_SFX_VOLUME);
	bsSetGlobalSampleVolume(gameStatus.globalSettings.sfxVolume);
}

// TP: temp just so I don't have to get any new stuff yet, am doing disk.
void sfxSliderUpdateFunc(const float sliderValue)
{
	gameStatus.globalSettings.sfxVolume=(int)(sliderValue*MAX_SFX_VOLUME);
	bsSetGlobalSampleVolume(gameStatus.globalSettings.sfxVolume);
}



/* --------------------------------------------------------------------------------
   Function		: UpdatePauseOptions
   Purpose		: update the options menu of the pause screen
   Parameters	: 
   Returns		: 
   Info			: 
				
*/
void UpdatePauseOptions(void)
{
}


/* --------------------------------------------------------------------------------
   Function		: OpenExitMenu
   Purpose		: Open the exit options screen and create the book
   Parameters	: 
   Returns		: 
   Info			: 
*/
void OpenExitMenu(void)
{

}


/* --------------------------------------------------------------------------------
   Function		: PauseToExitMenu
   Purpose		: enter pause mode and go straight to the exit options screen
   Parameters	: 
   Returns		: 
   Info			: // PP: probably a temp thang
*/
void PauseToExitMenu(void)
{

}


/* --------------------------------------------------------------------------------
   Function		: CloseExitMenu
   Purpose		: closedown the exit menu of the pause screen
   Parameters	: 
   Returns		: 
   Info			: 
*/
void CloseExitMenu(void)
{

}

/* --------------------------------------------------------------------------------
   Function		: UpdateExitMenu
   Purpose		: update the exit menu of the pause screen
   Parameters	: 
   Returns		: 
   Info			: 
				
*/
void UpdateExitMenu(void)
{
}



/*  --------------------------------------------------------------------------------
	Function	: ReturnToHub
	Purpose		: Send the appropriate scenechange message
	Parameters	: 
	Returns		: 
	Info		: 
*/
void ReturnToHub(void)
{
	switch(map.sceneNumber)
	{
	case SCENE_ICEDOME:
	case SCENE_SAFARI:
	case SCENE_AQUA:
	case SCENE_ZOOBOSS:
		bkGenerateEvent("scenechange","_zoohub");
		break;
	case SCENE_DEPTSTR:
	case SCENE_MUSEUM:
	case SCENE_CONSTRUCT:
	case SCENE_CITYBOSS:
		bkGenerateEvent("scenechange","_cityhub");
		break;
	case SCENE_GHOSTTOWN:
	case SCENE_GOLDMINE:
	case SCENE_GRANDCANYON:
	case SCENE_WESTBOSS:
		bkGenerateEvent("scenechange","_westhub");
		break;
	case SCENE_TAZPREBOSS:
	case SCENE_TAZBOSS:
		bkGenerateEvent("scenechange","_tazhub");
		break;
	}
}

void StartMapHint(int cameraNum)
{
	PAGE	*pagePtr;

	TEXTBOX *textPtr;

	int width, height, rgbDepth, zDepth;

	if (gameStatus.currentState == GAMESTATE_CAMERACASE)
		return;

	if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].difficulty == DIFF_HARD)
		return;	// JW: No hints on hard

	if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].difficulty == DIFF_MEDIUM)
	{
		if (hintFlag)
			return;
		else
		{
			hintBook->init();

			// PP: gotter clear that effect chooser again after the init
			hintBook->setEffectChooser(&basePIEChooser);

			hintBook->flags &= ~(BKFLAG_SCREENADJUST|BKFLAG_BOOKLIST);

		// PP: margin for hint book
#define HINT_MARGIN			0.07f

			hintBook->setBookRect(RECTANGLE(-0.5f+HINT_MARGIN, 0.5f-HINT_MARGIN, -0.5f+HINT_MARGIN, 0.5f-HINT_MARGIN));
			
			pagePtr = hintBook->addPage("hint");
			pagePtr->insertItem(textPtr = new TEXTBOX(mapMedHints[map.sceneNumber][cameraNum]));

#define HINT_FONTSIZE		1.5f// PP: REMOUT: too big (German)	1.8f

			textPtr->setFontSize(HINT_FONTSIZE);
			textPtr->setColour(COLOUR(0,0,0,128), COLOUR(0,0,0,128));
			textPtr->flags&= ~TBFLAG_OUTLINE;
			textPtr->setWrap(true);
		}
	}	// JW: Give first hint only

	if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].difficulty == DIFF_EASY)
	{
		if (hintFlag)
		{	
			if (hintMode == MAPHINT_APPEAR)
			{
				// JW: Flick between pages

				if (mouse.active && controller1.mb1DebounceChannel->value && !ShowFlyBy)
				{
					if (hintBook->getCurrentPageIndex() < hintBook->getNumPages()-1)
					{
						hintBook->nextPage();
						hintTime = gameStatus.gameTime + 3.0f;
					}
					else
					{
						StopMapHint();
						ShowFlyBy = true;
					}
				}
				else
				{
					hintBook->nextPage();

					hintTime = gameStatus.gameTime + 3.0f;				
				}
			}

			return;
		}
		else
		{
// PP: REMOUT: NEW: hmm this is killing off the previous setup we do 			
			hintBook->init();	// JW: Oh no it isn't. Thats if the game is in medium difficulty. This is if the games in easy.

			hintBook->flags &= ~(BKFLAG_SCREENADJUST|BKFLAG_BOOKLIST);

			hintBook->setBookRect(RECTANGLE(-0.5f+HINT_MARGIN, 0.5f-HINT_MARGIN, -0.5f+HINT_MARGIN, 0.5f-HINT_MARGIN));
			
			pagePtr = hintBook->addPage("hint");
			pagePtr->insertItem(textPtr = new TEXTBOX(mapMedHints[map.sceneNumber][cameraNum]));

			textPtr->setFontSize(HINT_FONTSIZE);

			textPtr->setColour(COLOUR(0,0,0), COLOUR(0,0,0));
			textPtr->flags&= ~TBFLAG_OUTLINE;
			textPtr->setWrap(true);

			pagePtr = hintBook->addPage("hint2");
			pagePtr->insertItem(textPtr = new TEXTBOX(mapEasyHints[map.sceneNumber][cameraNum]));

			textPtr->setFontSize(HINT_FONTSIZE);

			textPtr->setColour(COLOUR(0,0,0), COLOUR(0,0,0));
			textPtr->flags&= ~TBFLAG_OUTLINE;
			textPtr->setWrap(true);
		}	
	}	// JW: Give 

#ifndef FRIGTARGET

	// create a shared target (320x240) with 16bit colour & 16bit Z (Unless on PC then this must match the current screen colour depth)
#if(BPLATFORM == PC)
	if (hintTarget = bdCreateRenderTarget(POSTIT_ORT_WIDTH, POSTIT_ORT_HEIGHT, videoMode.bppScreen, videoMode.bppScreen))
#else
	if (hintTarget = bdCreateRenderTarget(POSTIT_ORT_WIDTH, POSTIT_ORT_HEIGHT, 16, 16))
#endif
	{
		// retrieve the dimensions and depths that were actually used
		bdGetRenderTargetInfo(hintTarget, &width,&height, &rgbDepth, &zDepth);

		bkPrintf("StartMapHint: created RT ok (%d*%d, rgb %d, z %d)\n", width, height, rgbDepth, zDepth);
	}
	else
	{
		bkPrintf("StartMapHint: couldn't create RT\n");
		return;
	}

#endif// PP: ndef FRIGTARGET

	//PlaySample("cameraclick.wav", 255);
	
	hintyOff = 1;

	hintBook->gotoPage("hint");

	hintBook->open();

	hintTime = gameStatus.gameTime + 4.0f;

	hintFlag = 1;

	hintMode = MAPHINT_APPEAR;
}

void InitMapControls()
{
	PAGE			*page;
	TEXTBOX			*text;

	controlx = controlNotePos[map.sceneNumber][X]*videoMode.xScreen;
	controly = controlNotePos[map.sceneNumber][Y]*videoMode.yScreen;

	controlw=CONTROL_NOTE_WIDTH*videoMode.yScreen;// PP: this should indeed be yScreen
	controlh=CONTROL_NOTE_HEIGHT*videoMode.yScreen;
	
	rotation = (controlx > 0 ? 0.2f : -0.2f);

	int width, height, rgbDepth, zDepth;

#ifndef NO_MAP_ORTS// PP: TEMP TEST

	// create a shared target (256x256) with 16bit colour & 16bit Z (Unless on PC then this must match the current screen colour depth)
#ifndef FRIGTARGET
	#if(BPLATFORM == PC)
		if (controlTarget = bdCreateRenderTarget(POSTIT_ORT_WIDTH, POSTIT_ORT_HEIGHT, videoMode.bppScreen, videoMode.bppScreen, BCREATERENDERTARGET_PRIVATEZ))
	#else
		if (controlTarget = bdCreateRenderTarget(POSTIT_ORT_WIDTH, POSTIT_ORT_HEIGHT, 16, 16, BCREATERENDERTARGET_PRIVATEZ))
	#endif
		{
			// retrieve the dimensions and depths that were actually used
			bdGetRenderTargetInfo(controlTarget, &width,&height, &rgbDepth, &zDepth);

			bkPrintf("InitMapControl: created RT ok (%d*%d, rgb %d, z %d)\n", width, height, rgbDepth, zDepth);
		}
		else
		{
			bkPrintf("InitMapControl: couldn't create RT\n");
			return;
		}

	#endif// PP: ndef NO_MAP_ORTS
#endif

	// TP: controlBook = new BOOK;

	// PP: margin for control book
	#define CONTROL_MARGIN			0.07f

	controlBook->setBookRect(RECTANGLE(-0.5f+HINT_MARGIN, 0.5f-HINT_MARGIN, -0.5f+HINT_MARGIN, 0.5f-HINT_MARGIN));

	controlBook->flags &= ~(BKFLAG_SCREENADJUST|BKFLAG_BOOKLIST);

	page = controlBook->addPage("controls");

#define CONTROL_FONTSIZE	1.5f// PP: REMOUT: too big (German)	1.8f

// TP: 1.25f

#define	ADDMAPCONTROLKEY(_string) \
	page->insertItem(text = new TEXTBOX(_string)); \
	text->setFontSize(CONTROL_FONTSIZE); \
	text->setXAlign(PIFLAG_XALIGN_CENTRE); \
	text->setYAlign(PIFLAG_YALIGN_CENTRE); \
	text->setYPadding(false); \
	text->setScale(1.5f); \
	text->setWrap(true); \
	text->setColour(COLOUR(0,0,0), COLOUR(0,0,0)); \
	text->flags&= ~TBFLAG_OUTLINE;

	ADDMAPCONTROLKEY(STR_CROSS_FLYBY);
	
	if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].difficulty != DIFF_HARD)
		ADDMAPCONTROLKEY(STR_SQUARE_HINT);

	// TP: start no longer has function on this screen ADDMAPCONTROLKEY(STR_PRESS_START_TO_RESUME);
	ADDMAPCONTROLKEY(STR_PRESS_Y_TO_GO_BACK);
	
	controlBook->open();
}

void UpdateMapHint()
{
	if (hintMode == MAPHINT_DISAPPEARUP)
	{
		hintyOff -= 0.05f;

		if (hintyOff < 0)
			hintyOff = 0.0f;
	}

	if (hintTime < gameStatus.gameTime && hintMode == MAPHINT_APPEAR)
	{
		ShowFlyBy = true;
		
		hintBook->close();

		hintMode = MAPHINT_DISAPPEARUP;

		hintTime = gameStatus.gameTime + 1.25f;
	}

	if (hintTime < gameStatus.gameTime && hintMode != MAPHINT_APPEAR)
	{
		hintBook->shutdown();

		hintTime = -1;

		hintFlag = 0;

#ifndef FRIGTARGET

		bdDeleteRenderTarget(hintTarget);
		hintTarget = 0;

#endif// PP: ndef FRIGTARGET

	}

	if (bmRand()%2)
	{
		if (bmRand()%2)
			CURL_INC = 0.35f;
		else
			CURL_INC = -0.35f;
	}

	if (CURL_ANGLE < 5.0f)
		CURL_INC = 0.35f;

	if (CURL_ANGLE > 15.0f)
		CURL_INC = -0.35f;

	CURL_ANGLE += CURL_INC;
}

void UpdateMapControls()
{
#ifdef TWEAKNOTE
	if (controller1.l1Channel->value)
	{
		if (controller1.x1Channel->value < 0)
			controlx -= (controller1.crossChannel->value ? 5.0f : 1.0f);
		else if (controller1.x1Channel->value > 0)
			controlx += (controller1.crossChannel->value ? 5.0f : 1.0f);

		if (controller1.y1Channel->value < 0)
			controly += (controller1.crossChannel->value ? 5.0f : 1.0f);
		else if (controller1.y1Channel->value > 0)
			controly -= (controller1.crossChannel->value ? 5.0f : 1.0f);

		if (controller1.x2Channel->value < 0)
			rotation += 0.1f;
		else if (controller1.x2Channel->value > 0)
			rotation -= 0.1f;

		if (controller1.squareChannel->value)
		{
			controlw -= 1.0f;
			controlh -= 1.0f;
		}

#if(BPLATFORM == XBOX)// PP: circle=B=back on Xbox
		if (controller1.triangleChannel->value)
#else// PP: !xbox
		if (controller1.circleChannel->value)
#endif// PP: !xbox
		{
			controlw += 1.0f;
			controlh += 1.0f;
		}
	}
#endif// PP: def TWEAKNOTE

	if (bmRand()%2)
	{
		if (bmRand()%2)
			CURL_INC = 0.35f;
		else
			CURL_INC = -0.35f;
	}

	if (CURL_ANGLE < 5.0f)
		CURL_INC = 0.35f;

	if (CURL_ANGLE > 15.0f)
		CURL_INC = -0.35f;

	CURL_ANGLE += CURL_INC;
}

#include "envmap.h"	

#if(BPLATFORM == XBOX)
	#define POSTIT_MAX_X	120
	#define POSTIT_MAX_Y	90
	
	//#if (videoMode.flags&BDISPLAYFLAG_NTSC)
//		#define POSTIT_MAX_Y	90
//	#else
//		#define POSTIT_MAX_Y	102
//	#endif
#else// PP: platform
	#define POSTIT_MAX_X	160
	#define POSTIT_MAX_Y	90

	//#if (videoMode.flags&BDISPLAYFLAG_NTSC)
	//	#define POSTIT_MAX_Y	90
	//#else
	//	#define POSTIT_MAX_Y	102
	//#endif
#endif// PP: platform

void DrawMapHint()
{
	TBVector2D		pos;		// TP: changed this 'cause its not a good idea to rely on the return address of a local
	TBRenderTarget	*target;

#ifdef FRIGTARGET
	target=frigTarget;
#else// PP: ifndef FRIGTARGET
	target=hintTarget;
#endif// PP: ndef FRIGTARGET

	pushRenderTarget(target, 0,255,0,255,1.0f, BSETRENDERTARGET_CLEAR);

	pos[X] = 0;
	pos[Y] = 0;

	bdBeginScene();

	bdSetIdentityObjectMatrix();
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, 0);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, 0);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, 1);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, 1);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	// JW: Post-it no

	// PP: PLATFORM: BABEL WORKAROUND!!!! (PS2 IS WRONG!!!!!)
#if(BPLATFORM == PS2)
	utilDraw2DSprite(hintTexture, pos, POSTIT_ORT_WIDTH, POSTIT_ORT_HEIGHT, 0);
#else// PP: not ps2
	utilDraw2DSprite(hintTexture, pos, float(videoMode.xScreen), float(videoMode.yScreen), 0);
#endif// PP: not ps2

	// PP: ***** PS2 BABEL WORKAROUND *****
#if(BPLATFORM == PS2)
	BOOK::PS2BabelWorkaround_setRenderTargetDimensions(POSTIT_ORT_WIDTH, POSTIT_ORT_HEIGHT);
#endif// PP: sodding ps2
	// PP: ********************************

	if (hintBook)
		hintBook->privateDraw();// PP: BOOKLIST EXCEPTION

	bdEndScene();
	
	popRenderTarget(0,0,0,255, 1, 0);
	
	map.pauseMap->GetMapPositionOfCurrentPoster(pos);
	
	TBVector2D	TVPos, PPos, EndPos;
	
	PPos[X] = pos[X];
	PPos[Y] = pos[Y];

	if (PPos[X] > 0 && PPos[Y] > 0)			// JW: NE
	{	
		TVPos[X] = -POSTIT_MAX_X;
		TVPos[Y] = -POSTIT_MAX_Y*1.4f;
	}
	else if (PPos[X] <= 0 && PPos[Y] > 0)	// JW: NW
	{	
		TVPos[X] = POSTIT_MAX_X*1.1f;
		TVPos[Y] = -POSTIT_MAX_Y*1.1f;
	}
	else if (PPos[X] > 0 && PPos[Y] < 0)	// JW: SE
	{	TVPos[X] = -POSTIT_MAX_X *0.9f;
		TVPos[Y] = POSTIT_MAX_Y * 1.2f;
	}
	else									// JW: SW
	{	TVPos[X] = POSTIT_MAX_X*0.9f;
		TVPos[Y] = POSTIT_MAX_Y*1.4f;
	}

	piRotation = (TVPos[X] > 0 ? 0.125f : -0.125f);
	
	bdBeginScene();

	EndPos[X] = 0;
	EndPos[Y] = 0;
	
	// PP: There seems to be some problem with wrapping even pow-2 linear textures on Xbox (nasty horrible lock)	// PP: TODO: investigate this.  for now, I'll just clamp
	SETWRAP(CLAMP);

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	BuildPostit(TVPos, piRotation);

#if BPLATFORM!=GAMECUBE
	if((NUM_STRIPS + 1)*2)
	{
		bdSetTexture(0, NULL);
		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, shadowVerts, (NUM_STRIPS + 1) * 2);
		
		bdSetRenderTargetAsTexture(target, 0);
		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, postitVerts, (NUM_STRIPS + 1) * 2);
	}
#endif

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	
	bdEndScene();
}

void DrawMapControls()
{
	TBVector2D		pos;		// TP: changed this 'cause its not a good idea to rely on the return address of a local
	TBRenderTarget	*target;

#ifdef FRIGTARGET
	target=frigTarget;
#else// PP: ifndef FRIGTARGET
	target=controlTarget;
#endif// PP: ndef FRIGTARGET

	if(target != NULL)
	{
		// PP: NOTE: Babel gets unhappy if you try to clear a non-32-bit render target to anything other than full alpha (255)
		pushRenderTarget(target, 0,0,0,255,1.0f, BSETRENDERTARGET_CLEARRGB);
	}

#ifndef NO_MAP_ORT_DRAWS

	pos[X] = 0;
	pos[Y] = 0;

	// NH: Fails for some reason on PC!
	bdBeginScene();

	bdSetIdentityObjectMatrix();
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, 0);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, 0);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, 1);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, 1);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	SETFRAMEWRITE(rgbFramewriteChannels|BDFRAMEWRITE_ALPHA);

	// JW: Post-it no

	// PP: PLATFORM: BABEL WORKAROUND!!!! (PS2 IS WRONG!!!!!)
#if(BPLATFORM == PS2)
	utilDraw2DSprite(cntrlTexture, pos, POSTIT_ORT_WIDTH, POSTIT_ORT_HEIGHT, 0);
#else// PP: not ps2
	utilDraw2DSprite(cntrlTexture, pos, float(videoMode.xScreen), float(videoMode.yScreen), 0);
#endif// PP: not ps2

	// PP: ***** PS2 BABEL WORKAROUND *****
#if(BPLATFORM == PS2)
	BOOK::PS2BabelWorkaround_setRenderTargetDimensions(POSTIT_ORT_WIDTH, POSTIT_ORT_HEIGHT);
#endif// PP: sodding ps2
	// PP: ********************************

	if (controlBook)
		controlBook->privateDraw();// PP: BOOKLIST EXCEPTION
	//controlBook->drawDebug();

	bdEndScene();

	SETFRAMEWRITE(rgbFramewriteChannels);

//	bdSetViewport(0, 0, videoMode.xScreen, videoMode.yScreen);
//	bdSetupView();

#endif// PP: ndef NO_MAP_ORT_DRAWS	

	if(target != NULL)
	{
		popRenderTarget(0,0,0,255, 1, 0);
	}

	/* PP: LEAVE IN: this must be unremmed when I do my tweaks to controlNotePos
	controlx=controlNotePos[map.sceneNumber][X];
	controly=controlNotePos[map.sceneNumber][Y];*/
	
	
	pos[X] = (controlx / 640.0f) * videoMode.xScreen;
	pos[Y] = (controly / 448.0f) * videoMode.yScreen;

	bdBeginScene();

	// PP: There seems to be some problem with wrapping even pow-2 linear textures on Xbox (nasty horrible lock)	// PP: TODO: investigate this.  for now, I'll just clamp
	SETWRAP(CLAMP);

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	// IH: This has to go in here due to the render target using the main screen buffer
#if BPLATFORM==GAMECUBE
		map.pauseMap->Draw(gameStatus.player[0].actorInstance->actorInstance.position);
#endif


	//BuildPostit(pos, rotation);

	//if((NUM_STRIPS + 1)*2)
	//{
		bdSetTexture(0, NULL);
		utilDraw2DSprite(0, pos, controlw, controlh, rotation, COLOUR(0,0,0,64), 0, 0, 0,0,-0.1f,0.1f);
		//bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, shadowVerts, (NUM_STRIPS + 1) * 2);
		
		if(target != NULL)
		{
			bdSetRenderTargetAsTexture(target, 0);
		}
		
		utilDraw2DSprite(0, pos, controlw, controlh, rotation);

		// PP: unset RT as texture
		NOTEXTURE;
		//bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, postitVerts, (NUM_STRIPS + 1) * 2);
	//}

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	
	bdEndScene();
}

void BuildPostit(TBVector postPos, float rot)
{
	float pos[NUM_STRIPS + 1][2];
	
	float lastY = 0.0f;

	TBVector	vector;

	int i;

	for (i = 0; i < NUM_STRIPS + 1; i++)
	{
		pos[i][X] = bmCos(RAD(-(CURL_ANGLE / NUM_STRIPS) * i));
		pos[i][Y] = bmSin(RAD(-(CURL_ANGLE / NUM_STRIPS) * i));
	}

	lastY = pos[NUM_STRIPS][Y];

	for (i = 0; i < NUM_STRIPS + 1; i++)
	{
		pos[i][X] -= 1.0f;
		pos[i][X] *= bmFAbs(1.0f/lastY);
		pos[i][X] -= 0.5f;
		pos[i][X] *= (videoMode.xScreen / 2.0f) * (hintFlag ? 0.75f : 0.5625f);
		pos[i][X] += postPos[X];
		
		pos[i][Y] *= bmFAbs(1.0f/lastY);
		pos[i][Y] *= (1 - (CURL_ANGLE / 90));
		pos[i][Y] += 0.5f;
		pos[i][Y] *= (videoMode.yScreen / 2.0f) * (hintFlag ? 0.714f : 0.5357f);
		pos[i][Y] += postPos[Y];

		SETVECTOR(vector, pos[i][X], pos[i][Y], 0.0f, 1.0f);

		bmVectorRotateZ(vector, vector, rot);
		
		BDVERTEX_SETXYZWVEC(&postitVerts[i * 2], vector);
		BDVERTEX_SETRGBA(&postitVerts[i * 2], 128, 128, 128, ((hintFlag) ? uchar(128 * hintyOff) : 128));
		BDVERTEX_SETUV(&postitVerts[i * 2], (float) 0, (float) (i * (1.0f / (float) NUM_STRIPS)));

		pos[i][X] += (videoMode.xScreen / 2.0f) * (hintFlag ? 0.75f : 0.5625f);

		SETVECTOR(vector, pos[i][X], pos[i][Y], 0.0f, 1.0f);

		bmVectorRotateZ(vector, vector, rot);

		BDVERTEX_SETXYZWVEC(&postitVerts[i * 2] + 1, vector);
		BDVERTEX_SETRGBA(&postitVerts[i * 2] + 1, 128, 128, 128, ((hintFlag) ? uchar(128 * hintyOff) : 128));
		BDVERTEX_SETUV(&postitVerts[i * 2] + 1, (float) 1, (float) (i * (1.0f / (float) NUM_STRIPS)));
	}

	for (i = 0; i < NUM_STRIPS + 1; i++)
	{
		pos[i][X] = bmCos(RAD(-((CURL_ANGLE / 2)/ NUM_STRIPS) * i));
		pos[i][Y] = bmSin(RAD(-((CURL_ANGLE / 2) / NUM_STRIPS) * i));
	}

	lastY = pos[NUM_STRIPS][Y];

	for (i = 0; i < NUM_STRIPS + 1; i++)
	{
		pos[i][X] -= 1.0f;
		pos[i][X] *= bmFAbs(1.0f/lastY);
		pos[i][X] -= 0.5f;
		pos[i][X] *= (videoMode.xScreen / 2.0f) * (hintFlag ? 0.75f : 0.5625f);
		pos[i][X] += postPos[X];
		
		pos[i][Y] *= bmFAbs(1.0f/lastY);
		pos[i][Y] *= (1 - ((CURL_ANGLE / 2) / 90));
		pos[i][Y] += 0.5f;
		pos[i][Y] *= (videoMode.yScreen / 2.0f) * (hintFlag ? 0.714f : 0.5357f);
		pos[i][Y] += postPos[Y];

		SETVECTOR(vector, pos[i][X], pos[i][Y], 0.0f, 1.0f);

		bmVectorRotateZ(vector, vector, rot);

		BDVERTEX_SETXYZWVEC(&shadowVerts[i * 2], vector);
		BDVERTEX_SETRGBA(&shadowVerts[i * 2], 0, 0, 0, ((hintFlag) ? (64 * hintyOff) : 64));

		pos[i][X] += (videoMode.xScreen / 2.0f) * (hintFlag ? 0.75f : 0.5625f);

		SETVECTOR(vector, pos[i][X], pos[i][Y], 0.0f, 1.0f);

		bmVectorRotateZ(vector, vector, rot);

		BDVERTEX_SETXYZWVEC(&shadowVerts[i * 2] + 1, vector);
		BDVERTEX_SETRGBA(&shadowVerts[i * 2] + 1, 0, 0, 0, ((hintFlag) ? (64 * hintyOff) : 64));
	}
}

#if BPLATFORM==GAMECUBE
void DrawPostit(void)
{
	TBRenderTarget*		target;

	bdBeginScene();

	// PP: There seems to be some problem with wrapping even pow-2 linear textures on Xbox (nasty horrible lock)	// PP: TODO: investigate this.  for now, I'll just clamp
	SETWRAP(CLAMP);

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	if((NUM_STRIPS + 1)*2)
	{
		bdSetTexture(0, NULL);
		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, shadowVerts, (NUM_STRIPS + 1) * 2);
		
		bdSetRenderTargetAsTexture(hintTarget, 0);
		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, postitVerts, (NUM_STRIPS + 1) * 2);
	}

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	
	bdEndScene();
}
#endif

void StopMapHint()
{
	hintBook->close();
	hintBook->shutdown();

	hintMode = MAPHINT_DISAPPEARUP;

	hintTime = -1;

	hintFlag = 0;

#ifndef FRIGTARGET

	bdDeleteRenderTarget(hintTarget);
	bkPrintf("Hint Book RT Deleted\n");
	hintTarget = 0;

#endif// PP: ndef FRIGTARGET
}

void KillMapControls()
{
	if (controlBook)
	{
		controlBook->close();
		controlBook->shutdown();

#ifndef FRIGTARGET

		if(controlTarget != NULL)// PP: will be null if the Pause Menu book wasn't opened
		{
			bdDeleteRenderTarget(controlTarget);
			bkPrintf("Control Book RT Deleted\n");
			// PP: important!
			controlTarget=NULL;
		}

#endif// PP: ndef FRIGTARGET

		SAFE_DELETE(controlBook);
	}
}

/*  --------------------------------------------------------------------------------
	Function	: CONFIRM::TAG_CONFIRM
	Purpose		: Default constructor
	Parameters	: 
	Returns		: 
	Info		: 
*/
CONFIRM::TAG_CONFIRM(int header)
{
	TEXTBOX	*tb;

	// TP: Set up rect
	setRect(RECTANGLE(-0.1875f,0.1875f,-0.089f,0.089f));// PP: REMOUT: must now be expressed as fractions of the screen dimensions	setRect(RECTANGLE(-120,120,-40,40));
	setBackgroundColour(COLOUR(0,0,0,64));

	// TP: Set up textboxes
	page = addPage("CONFIRMPAGE");						// PP: added page name for for debugging

	if (header!=-1)
	{
		page->insertItem(tb = new TEXTBOX((EStringIndex)header));
		tb->setWrap(true);
	}
	page->insertItem(new TEXTBOX(STR_DIALOGUE_OKCANCEL));

	open();
}

/*  --------------------------------------------------------------------------------
	Function	: CONFIRM::~TAG_CONFIRM
	Purpose		: Default destructor
	Parameters	: 
	Returns		: 
	Info		: 
*/
CONFIRM::~TAG_CONFIRM()
{
}

/*  --------------------------------------------------------------------------------
	Function	: CONFIRM::update
	Purpose		: respond to control input
	Parameters	: 
	Returns		: 
	Info		: 
*/
int CONFIRM::Update()
{
	BOOK::update();

	if(OKBUTTON)
	{
		camera[0].fade.StartFadeUp(FADETYPE_NORMAL,1.0f);
		return TRUE;
	}
	else if (CANCELBUTTON)
	{
		return FALSE;
	}
	return -1;
}

void EnterMPPause(const uchar playerIndex, bool fullScreen)
{
	// PP: set the multiplayer pause book's screen and controller to correspond to specified player
#if BPLATFORM == PC
	// NH: Unless on PC then make the pause screen apprear over both screens
	mpPauseBook->setScreen(BKSCREEN_FULL);
#else
	mpPauseBook->setScreen(playerIndex);
#endif // NH: #if BPLATFORM == PC

	// PP: clear the button press that initiated the pause
	biForceDebounce(controller1.startDebounceChannel);
	biForceDebounce(controller2.startDebounceChannel);

	// PP: MILESTONE BODGE...

	if(levelData.oneBook)
	{
		levelData.oneBook->close();
	}

	if(levelData.twoBook)
	{
		levelData.twoBook->close();
	}

	mpPauseBook->setController(playerIndex);

	gameStatus.multiplayer.playerPause = (playerIndex + 1);

	// PP: do stuff that needs to be done whenever any kind of pause mode is entered
	GenericPauseModePrepare();

#if BPLATFORM == PC
	InitMouse(&mouse);
	mouse.active = true;
#endif

	pauseMenuActive = true;

	// PP: if the demo timer book is open, close it
// JW: #ifdef CONSUMERDEMO	// JW: Probably best to have the book closed whether it's demo or no
	//demoTimerBook->close();

	if (raceBook)
	{
		raceBook->close();
		mpTimerFlash = false;
	}

	if (gameStatus.player1->actorStatus->multiplayer.pStatsBook)
		gameStatus.player1->actorStatus->multiplayer.pStatsBook->close();
	if (gameStatus.player2->actorStatus->multiplayer.pStatsBook)
		gameStatus.player2->actorStatus->multiplayer.pStatsBook->close();

//#endif// PP: def CONSUMERDEMO

	mpPauseBook->getPage("mp pause")->setNavigation(true);
	mpPauseBook->gotoPage("mp pause");

	mppausePage.infoSubBook->open();
	mppausePage.controlSubBook->open();
	mpPauseBook->open();

	mppausePage.page->selectItem(mppausePage.resumeTb);

/*	if (gameStatus.multiplayer.playerPause == 1)
		mppausePage.page->setController(0);
	else if (gameStatus.multiplayer.playerPause == 2)
		mppausePage.page->setController(1);*/

	// PP: I put this here cos, well, it's a nice place for it no?
	SetGameState(&gameStatus,GAMESTATE_PAUSE,GAMESUBSTATE_NORMAL);
}

void ExitMPPauseMode(void)
{
// PP: MILESTONE BODGE
/* PP: REMOUT: #if(BPLATFORM != XBOX)
	if (!pauseMenuActive) return;
#endif*/// PP: 

	// PP: do generic stuff that has to be done when exiting any sort of pause mode

	GenericPauseModeEnd();

	// NH: Close pop up
	/*if (pauseMenu.pausePopUp)
	{
		pauseMenu.pausePopUp->close(0.0f,TRUE);
		SAFE_DELETE(pauseMenu.pausePopUp);
		pauseMenu.pauseMenuBook->getPage(pauseMenuNames[PAUSE_OPTIONS])->setNavigation(true);
		pauseMenu.pauseOptionsPage->infoSubBook->open();
		pauseMenu.pauseOptionsPage->controlSubBook->open();
	}*/

	// PP: Close two-player pause menu
	mpPauseBook->close();

//#ifdef CONSUMERDEMO	// JW: Again, best to do this even if it's not the demo

	// PP: DEMO FRIG - I don't know where is the best place to put this

	if (gameStatus.pad1Present && gameStatus.pad2Present)
	{
		if (camera[0].fade.GetSceneToLoad() != SCENE_FRONTEND && camera[0].fade.GetFadeState() != FADESTATE_DOWN)
		{
			if (raceBook
#ifndef CONSUMERDEMO
				&& (map.sceneNumber > SCENE_DEPTSTRVEHICLERACE || map.sceneNumber == SCENE_CITYBOSS)
#endif
				)
				raceBook->open();

			if (gameStatus.player1->actorStatus->multiplayer.pStatsBook)
				gameStatus.player1->actorStatus->multiplayer.pStatsBook->open();
			if (gameStatus.player2->actorStatus->multiplayer.pStatsBook)
				gameStatus.player2->actorStatus->multiplayer.pStatsBook->open();
		}
	}

//#endif// PP: def CONSUMERDEMO


#if BPLATFORM == PC
	mouse.active = false;
#endif
	
	// PP: return to music mood in effect as the pause mode was entered (if it was valid)
	if(musicMoodBeforePause != STREAMMOOD_NONE)
	{
		musicSystem.RequestMood(0,musicMoodBeforePause, EXITPAUSEMODE_MUSICMOODCHANGE_FADETIME);
	}

	pauseMenuActive = false;

	// PP: this '=' was '=='
	if (gameStatus.pad1Present && gameStatus.pad2Present)
		gameStatus.multiplayer.playerPause = 0;

	controller1.idleTime = 0.0f;
	controller2.idleTime = 0.0f;
}


/*  --------------------------------------------------------------------------------
	Function	: OkToEnterPause
	Purpose		: Determine if it is ok to enter the pause or map mode
	Parameters	: 
	Returns		: true/false
	Info		: 
*/

bool OkToEnterPause()
{
	if ((camera[0].fade.GetFadeValue() == 0 || map.sceneNumber == SCENE_WESTBOSS)
		&&(gameStatus.currentState == GAMESTATE_INGAME)
		&&(map.sceneNumber != SCENE_LANGUAGESELECT)&&(map.sceneNumber != SCENE_FRONTEND)
		&&(map.sceneNumber < SCENE_NUMSCENES)&&(gameStatus.player1->actorStatus->tutorialBookFlag!=1)
		&&(!gameStatus.controlSuspend)
		&&(!gameStatus.demoMode)
		&&(getThwapStage() == Off)
		&&(gameStatus.player1)
		&&(gameStatus.player1->actorStatus->currentState != STATE_ENTERLOOKAROUND)
		&&(gameStatus.player1->actorStatus->currentState != STATE_LOOKAROUND)
		&&(gameStatus.player1->actorStatus->currentState != STATE_ENTERINGXDOOR)
		&&((!camera[0].cameraCase)||(camera[0].cameraCase->lockTazCRC==0))
		&&((map.sceneNumber!=SCENE_ZOOBOSS)||(VFX_spinoffStarted==false))
#ifdef CONSUMERDEMO
				)
#else
				&&(bossGameContinueInfo == NULL))// PP: no need to pause in boss game continue.  IT'S NOT GONNA GO ANYWHERE!!!
#endif
	{
		// JW: A little extra check for the multiplayer games
		if (map.sceneNumber > SCENE_TAZBOSS || map.sceneNumber == SCENE_CITYBOSS)
			if (levelData.RaceState == Race)
				return true;
			else
				return false;
		else
			return true;
	}
	else return false;
}


/*  --------------------------------------------------------------------------------
	Function	: OkToEnterMap
	Purpose		: check if its ok to enter the map
	Parameters	: 
	Returns		: 
	Info		: 
*/

bool OkToEnterMap()
{
	if ((gameStatus.player[0].actorInstance)&&(~gameStatus.player[0].actorInstance->flags & ACTORFLAG_DONTCHECKBOUNDINGBOX))
	{
		if (OkToEnterPause()) return true;
	}
	return false;
}