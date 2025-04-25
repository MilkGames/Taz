// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : main.cpp
//   Purpose : top level code
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"

#if BPLATFORM == PS2
extern uint _end;
#endif

#if BPLATFORM == GAMECUBE
#define TESTDECK
#endif

#include "actors.h"
#include "status.h"
#include "display.h"
#include "control.h"
#include "fade.h"
#include "scene.h"
#include "main.h"
#include "maths.h"
#include "map.h"
#include "debris.h"
#include "textures.h"
#include "sfx.h"
#include "gui.h"
#include "collectibles.h"
#include "languagemain.h"
#include "pause.h"
#include "font.h"
#include "objectviewer.h"
#include "cutscene.h"
#include "lights.h"
#include "playerstats.h"
#include "timer.h"
#include "fire.h"
#include "watersplash.h"
#include "files.h"
#include "explode.h"
#include "floorpolisher.h"
#include "fountainextrasystem.h"
#include "zzz.h"
#include "electrocution.h"
#include "textfx.h"
#include "voodoocloud.h"
#include "music.h"
#include "MemCard.h"				// PP: memory card operations and the dialogues for them
#include "icon.h"
#include "player.h"
#include "cameracase.h"
#include "phoneboxeffect.h"
#include "Shockwave.h"				// PP: Shockwave effect
#include "Boss.h"					
#include "multiplayer.h"
#include "subtitle.h"
#include "events.h"
#include "special.h"				// PP: "SPEcial!"
#include "util.h"					// PP: Utility functions
#include "VideoFX.h"				// PP: Video effects - motion blur, defocus, whatever.  Top-of-the-Pops-TASTIC, Mate!
#include "characters.h"
#include "PhilScene.h"				// PP: Phil-specific stuff
#include "TonyScene.h"				// PP: Tony-specific stuff
#include "sparks.h"
#include "fireworks.h"
#include "haar.h"
#include "cheesegun.h"
#include "waterfall.h"
#include "musicandsavagebeasts.h"
#include "collision.h"
#include "mouse.h"
#include "Fireball.h"				// PP: Particle emitters used for explosion effects, eg. by WestBossExp
#include "mtweetymagnet.h"
#include "tournament.h"
#include "shadow.h"					// PP: shadows
#include "tarbubble.h"
#include "PaperBook.h"				// PP: 'Paper' book type, ie. one that looks and acts like a real physical book
#include "aircon.h"
#include "rapperblast.h"
#include "demo.h"
#include "tazboss1.h"
#include "glowaroundplinth.h"
#include "lava.h"
#include "control.h"				// PP: provide basic input and control functions
#include "BookList.h"				// PP: List of books - handles drawing, update etc.
#include "demo.h"					// PP: consumer demo specifc stuff
#include "frontendmain.h"			// PP: frontend main menu
#include "files.h"
#include "bossgamecontinue.h"		// PP: boss game continue stuff
#include "endgamecredits.h"
#include "scenicobjects.h"
#include "prompt.h"
#include "subtitle.h"

#if(BPLATFORM == PC)
#include "binkVideo.h"				// NH: Bink video routines
#endif

#if(BPLATFORM == PC)
#include "binkVideo.h"				// NH: Bink video routines
#endif

#if(BPLATFORM == PS2)
#include <libsceedemo.h>			// PP: SCEE Demo Disk SDK Version 1.0 Pre-release 1
#else// PP: not ps2 // NH: and not PC.  Why should this be needed for any other machines?  I'll check with Phil later	// PP: It just is.  Don't make me get the duct tape!
#include "libsceedemo.h"			// PP: SCEE Demo Disk SDK Version 1.0 Pre-release 1
#endif// PP: ps2

// TP: If start_scene has not already been defined the define now.
#ifdef CONSUMERDEMO
	#if(BPLATFORM == XBOX)
		// PP: TODO: Not allowed to start on Language Select; go straight to the demo menu
		#define START_SCENE 	SCENE_LANGUAGESELECT
	#elif(BPLATFORM == PC) // NH: PC
		#define START_SCENE 	SCENE_LANGUAGESELECT
	#else// NH: not xbox or pc
		#define START_SCENE 	SCENE_LANGUAGESELECT
	#endif// PP: not xbox

	#if(BPLATFORM == PS2)// PP: PS2 demo stuff I presume

		#ifdef USE_HOST0
			#define FILENAME_IOPIMG		"host0:/Babel/PS2/modules/ioprp243.img"
			#define FILENAME_SIO2MAN	"host0:/Babel/PS2/modules/sio2man.irx"
			#define FILENAME_PADMAN		"host0:/Babel/PS2/modules/padman.irx"
			#define FILENAME_SDRDRV		"host0:/Babel/PS2/modules/sdrdrv.irx"
			#define FILENAME_LIBSD		"host0:/Babel/PS2/modules/libsd.irx"
			#define	FILENAME_ROLLINGVID	"host0:/taz/taz_demo.pss"
		#else
			#ifdef STANDALONE
				#define FILENAME_IOPIMG		"cdrom0:\\MODULES\\IOPRP243.IMG;1"
				#define FILENAME_SIO2MAN	"cdrom0:\\MODULES\\SIO2MAN.IRX;1"
				#define FILENAME_PADMAN		"cdrom0:\\MODULES\\PADMAN.IRX;1"
				#define FILENAME_SDRDRV		"cdrom0:\\MODULES\\SDRDRV.IRX;1"
				#define FILENAME_LIBSD		"cdrom0:\\MODULES\\LIBSD.IRX;1"
			#else
				#define FILENAME_IOPIMG		"cdrom0:\\TAZDEMO\\MODULES\\IOPRP243.IMG;1"
				#define FILENAME_SIO2MAN	"cdrom0:\\TAZDEMO\\MODULES\\SIO2MAN.IRX;1"
				#define FILENAME_PADMAN		"cdrom0:\\TAZDEMO\\MODULES\\PADMAN.IRX;1"
				#define FILENAME_SDRDRV		"cdrom0:\\TAZDEMO\\MODULES\\SDRDRV.IRX;1"
				#define FILENAME_LIBSD		"cdrom0:\\TAZDEMO\\MODULES\\LIBSD.IRX;1"
				#define	FILENAME_ROLLINGVID	"cdrom0:\\TAZDEMO\\TAZ_DEMO.PSS"
			#endif
			//#define	FILENAME_ROLLINGVID	"host0:/taz/taz_demo.pss"
		#endif

		static u_long128 pad_dma_buf[scePadDmaBufferMax] __attribute__ ((aligned (64)));

	#endif// PP: (BPLATFORM==PS2)
	
	// JW: I know that technically these are PS2 only but I imagine that the XBOX has 
	// JW: similar variables it needs so if you could use them as well it would help
	unsigned short language = SCEE_DEMO_LANGUAGE_ENGLISH;
	unsigned short aspect = SCEE_DEMO_ASPECT_4_3;
	unsigned short playmode = SCEE_DEMO_PLAYMODE_PLAYABLE;
	unsigned short to_inactive = 60;
	unsigned short to_gameplay = 240;
	
	char keepOnLooping = 1;	

#else
	#ifndef START_SCENE
		#if(BPLATFORM == XBOX)
			// PP: NOTE: lanugage select scene gets skipped so we actually appear to start on the main game intro
			#define START_SCENE				SCENE_LANGUAGESELECT
		#elif(BPLATFORM == PC)
			#define START_SCENE				SCENE_LANGUAGESELECT
		#else
			// PP: (not Xbox or PC) Start on Language Select as usual
			#define START_SCENE				SCENE_LANGUAGESELECT
		#endif// PP: not Xbox
	#endif// PP: not Xbox

	#ifdef JWEEKS
		#define START_SCENE			SCENE_LANGUAGESELECT
	#endif

	#ifdef NHIGGS
//		#define START_SCENE			SCENE_ZOOBOSS//SCENE_FRONTEND
	#endif

	#ifdef IAN
		#define START_SCENE			SCENE_LANGUAGESELECT
	#endif

	#ifdef PHIL
		#undef START_SCENE
	#define START_SCENE				SCENE_LANGUAGESELECT
	#endif// PP: def PHIL

	#ifndef START_SCENE
		#if BPLATFORM == PC
			#define START_SCENE		SCENE_LANGUAGESELECT
		#else
			#define START_SCENE 	SCENE_LANGUAGESELECT
		#endif
	#endif
#endif// PP: ndef START_SCENE
//#endif

// PP: when this is set to true, Babel has shutdown so DON'T TRY TO DELETE OR FREE ANYTHING!!!
bool			BabelHasShutdown;

#ifndef ROLLINGDEMO

#ifndef INFOGRAMES
//#define PROFILING
#endif

//#define PROVIEW
//#define OBJECTVIEWONLY
//#define DEBUG_TEXT
#define CONSOLEDEBUG

#ifdef INFOGRAMES
//#ifndef PHIL// PP: TEMP TEST
#define ENABLEVIDEO				// PP: enable videos
//#endif// PP: def PHIL
#endif

#ifdef NHIGGS
#define ENABLEVIDEO				// PP: enable videos
#endif

// PP: are we allowed to do this?
// PP: define SUGAR_COATED_CRASHES to put up the "bad disk" message should the final game crash somehow.
// PP: It's not a pleasant thought but, y'know...
// PP: Blame it on a dirty disk, that's what I say!  No-one will know any different.
// PP: #define SUGAR_COATED_CRASHES

char			buf[256];
float			fps=60.0f;		// PP: *whistling innocently* // PP: this prevents divisions by zero before fps is first updated
float			rawFPS=60.0f;	// PP: frames-per-second before game speed adjustments are applied
float			speedRatio;
int				debugVal=0;


// Main game structures
DISPLAYSETTINGS			videoMode;
PADCONTROLSTATUS		controller1,controller2;
PADCONTROLSTATUS		controller3,controller4;	// PP: I added these - YES THEY *ARE* NEEDED
GAMESTATUS				gameStatus;
TBFont					*standardFont=NULL;

int						screenShotNumber=0;
uint32					mailboxTotal, posterTotal;
uint32					mailboxLevelTotal, posterLevelTotal;

TBSample				*testSample;
ushort					string16[256];

char					commandLine[256];
int						elevatorHandle = -1;
int						tcount;
float					fTime;
float					rawFTime;					// PP: frame length before game speed adjustments are applied

float					sfxTimer = 0;	// JW: Wait 1 second after change of introcam to load sfx

bool					videoSkipEnable;

#if(BPLATFORM==PC)
UINT					previousState;
#endif

// PP: === I'm initialising currentLanguage to be an invalid value. ===
// PP: This forces statically-allocated books to update for our chosen language.
// PP: Previously, they might have been created as English books at startup,
// PP: then have foreign text inserted when the language becomes known,
// PP: then not think they have to update when the player switches back to English!
// PP: Initialising with an invalid value is safe because it will be set to a
// PP: valid value before bkSetLanguage is first called.
// PP: === Don't worry about any book realignment pauses - I'll get rid of them soon. ===
EBLanguageID			GAMESTATUS::currentLanguage = (EBLanguageID)(-1);

PROFILE					profile,tonyProfile;
TBStringTable			*mainStringTable;
char					*currentDate = __DATE__;
static char				parmStringBuf[256];
CHARACTERSOUNDHANDLES	characterSounds;

// PP: pointer to an update function for custom pause state.
// PP: You must set this before switching into GAMESTATE_CUSTOMPAUSE!
static PauseUpdateFunc	customPauseUpdateFunc=NULL;
// PP: context value passed through to the update function used for the custom pause state
static int32				customPauseUpdateFuncContext=0;

// TP: Choose your videos
#ifdef INFOGRAMES
	char debugDPad = 0;
#else
	char debugDPad = 1;
#endif

char	vidNumber = 0;
TBPrimVertex2D		global2DVertexBuffer[GLOBAL2DVERTEXBUFFERSIZE];

// JW: Declaration for the rolling demo test function.
void rollMain(void *context);

/* --------------------------------------------------------------------------------
   Function : Main
   Purpose : our main function
   Parameters : context
   Returns : 
   Info : 
*/

void Main(void *context)
{
#ifdef SUGAR_COATED_CRASHES
#ifdef _MASTER
#if BPLATFORM!=PS2	// TP: Not on PS2, exception handling takes up to 250k apparently so I have disabled it
	// PP: Should the unthinkable happen - should the final game crash for any reason -
	// PP: let's put up the "damaged disk" message.
	// PP: It's a whole lot prettier than a frozen screen!
	try
	{
#endif
#endif// PP: def _MASTER
#endif// PP: def SUGAR_COATED_CRASHES

#if BPLATFORM == PC
	// TP: move to files to tidy up main
	if (CheckForOtherTazInstances()) exit(true);
#endif

	bool exitGameIntro=false;
	int initState;
	int i;
	int	padState;
	static PLAYERSTATS	playerStats[MAX_PLAYERS];
	int memory = 50;
	TBClock		clock;

	// PP: initialise display settings (moved this to display.cpp to keep things tidy, hope you don't mind)
	initDisplay();
	// PP: initialise game timer
	gameStatus.appTime = 0.0f;

	for (i=0;i<MAX_PLAYERS;i++)
	{
		gameStatus.player[i].stats = &playerStats[i];
	}
	InitialisePlayers();

#if BPLATFORM == PS2
#ifdef PROFILING
	SetupProfiler();
#endif
#endif
	
	InitGameState(&gameStatus);

	// TP: Moved to fonts
	ChooseCurrentLanguage();

	// PP: Cool, we know our language - tell Babel about it without delay
	bkSetLanguage(GAMESTATUS::currentLanguage);
	
	/*for (i=0;i<MAX_SCREENS;i++)
	{
		camera[i].fade.InitFade();
	}*/
	
	setNumScreens(1, DEFAULT_SPLIT_SCREEN_MODE);	// PP: this sets gameStatus.multiplayer.splitScreenMode
	SetGameState(&gameStatus,GAMESTATE_INITIALISE,GAMESUBSTATE_NORMAL);

#if BPLATFORM == PS2
	// 0x1fa0000 - 256
	// 0x1fc0000 - 128
	initState = bkInit(NULL, (0x1fa0000 - (uint)(&_end))-(1*1024), BKINITFLAG_NONEXCLUSIVE);

#elif(BPLATFORM == XBOX)

	// PP: this frees up memory consumed by the XDemo splash screen thingy
	bdDoDummyPresent();
	initState = bkInit(NULL, 45*1024*1024, BKINITFLAG_NONEXCLUSIVE// PP: was 50 megs, but that prevented video from playing on green debug kits.  Hopefully it should work now.
#if((defined INFOGRAMES) && (defined _MASTER))// PP: no debug output in milestones from now on, and none in anything we submit
		|BKINITFLAG_NODEBUGOUTPUT
#endif// PP: ((defined INFOGRAMES) && (defined _MASTER))

#ifdef PHIL
		// PP: this is cos I tend to use an infogrames build when I shouldn't do, but need to see debug output
//		& ~BKINITFLAG_NODEBUGOUTPUT
#endif// PP: def PHIL

		);

#ifdef CONSUMERDEMO// PP: TEMP?
	CheckLaunchInfo();
#endif// PP: def CONSUMERDEMO// PP: TEMP?


#elif BPLATFORM == GAMECUBE
	#ifdef TESTDECK
 		initState = bkInit(NULL, (40*1024*1024), BKINITFLAG_NONEXCLUSIVE);	// 40M 
	#else
	    initState = bkInit(NULL, (18*1024*1024), BKINITFLAG_NONEXCLUSIVE);	// IH: 18M available in debug build
	#endif
#else // NH: PC
	// NH: Set Window Name (Safe to call before bkInit)
	bkSetAppName("Taz:Wanted");

	// NH: Init Babel with Mouse and Keyboard exclusive to the game
	initState = bkInit(NULL, 60*1024*1024, NULL);

	// NH: Set The Focus to our window
	SetFocus(bMainWindow);

	// NH: Disable Task Switching
	SystemParametersInfo (SPI_SETSCREENSAVERRUNNING, TRUE, &previousState, 0);

	// NH: Eeeek!!!! Mouse!! Sylvester!!!!! Get that mouse or your out of this house for good!
	// NH: Just incase you didn't guess this should get rid of the mouse pointer/cursor
	ShowCursor(false);
#endif

	if (!initState)
	{
		bkAlert("Error during Babel initialisation");
		return;
	}

	// PP: ********************************** TEMPORARY FRIG *************************************
	// PP: I'm using a stopwatch to smooth-out the framerate on Xbox.
	// PP: This supresses the mystery framerate jitter which I'm still no closer to understanding.
	INIT_XBOX_JITTER_SUPPRESSION_STOPWATCH;
	// PP: ***************************************************************************************

#ifndef _MASTER// PP: I put this here - isn't file access kinda slow?!
	bkCreateDebugStream(&map.errorStream,"error.txt",BDEBUGSTREAMFLAG_FLUSH|BDEBUGSTREAMFLAG_TODEBUGGER);
	bkCreateDebugStream(&map.optimizeStream,"optimisations.txt",BDEBUGSTREAMFLAG_FLUSH);
	bkCreateDebugStream(&map.memoryStream,"memory.txt",BDEBUGSTREAMFLAG_FLUSH);
#ifdef CONSOLEDEBUG
	bkSetDebugStreamFlags(&map.errorStream, BDEBUGSTREAMFLAG_TOCONSOLEWINDOW, BDEBUGSTREAMFLAG_TOCONSOLEWINDOW);
	bkSetDebugStreamFlags(&map.optimizeStream, BDEBUGSTREAMFLAG_TOCONSOLEWINDOW, BDEBUGSTREAMFLAG_TOCONSOLEWINDOW);
	bkSetDebugStreamFlags(&map.memoryStream, BDEBUGSTREAMFLAG_TOCONSOLEWINDOW, BDEBUGSTREAMFLAG_TOCONSOLEWINDOW);
	bkSetDebugStreamFlags(NULL, BDEBUGSTREAMFLAG_TOCONSOLEWINDOW, BDEBUGSTREAMFLAG_TOCONSOLEWINDOW);
#endif
#endif// PP: _TAZDEBUG

#if BPLATFORM == XBOX
	// PP: deactivate the heap-checker thread cos it's making things slow on Xbox
	bActivateHeapCheckerThread(FALSE);
#endif

	videoSkipEnable = false;

	ProcessCommandLine();

	bkReadClock(&clock);
	bmSRand(clock.second);

	// TP: Init master file system
	InitFileSystem();

	// init actor lists
	InitActorList();
	InitialisePackfileIndex();

	// Setup and open display
	if(!SetupDisplay(&videoMode))
	{
		bkAlert("Unable to open display, please restart computer.");
		return;
	}

	for (i=0;i<MAX_SCREENS;i++)
	{
		camera[i].fade.InitFade();
	}

// NH: The PS2 Version is done in ProcessCommandLine() as it needs to be done before SetupDisplay() for some odd PS2 only reasons!
#if BPLATFORM != PS2 && BPLATFORM != PC
#ifdef	ENABLEVIDEO
	doCorporateStuff();
#endif
#endif

	// PP: Initial render target is the backbuffer
	pushRenderTarget(NULL);

	bgInitGui();

	// setup and create all input channels
	CreateInputChannels(&controller1,0);

#if BPLATFORM == PC
	// NH: Seems to be needed on PC
	if (BindInputChannels(&controller1,0)) 
	{
		gameStatus.pad1Present = TRUE;
	}
#endif // NH: BPLATFORM

	CreateInputChannels(&controller2,1);

#if(NUM_CONTROLLER_PORTS == 4)

	// PP: YES, THESE *ARE* NEEDED

	CreateInputChannels(&controller3,2);
	CreateInputChannels(&controller4,3);

#endif// PP: 4 ports

#if BPLATFORM == PC
	// NH: Seems to be needed on PC
	if (BindInputChannels(&controller2,1)) gameStatus.pad2Present = TRUE;
#endif // NH: #if BPLATFORM == PC

#ifndef ALLOW_CONTROLLERS_IN_ANY_PORTS

		padCheckMode=PADCHECK_NORMAL;// PP: new
		bindControllersToDefaultPorts();// PP: new
		gameStatus.pad1Present=true;// PP: test - shouldn't need this if checkForMissingControllers2 is working as it should
		gameStatus.pad2Present=true;// PP: test - shouldn't need this if checkForMissingControllers2 is working as it should
#else
		padCheckMode=PADCHECK_ALL;// PP: new
		bindControllersToDefaultPorts();// PP: new
#endif

#if BPLATFORM == PC
#ifdef	ENABLEVIDEO
	// NH: If on PC and Video is Enabled do the Corp Logos.  This is after control setup, so that we can skip.
	doCorporateStuff();
#endif
#endif // NH: #if BPLATFORM == PC

//#ifdef CONSUMERDEMO	// JW: Little 3 second pasue in the demo cos Pesty can't be arsed to alter his precious lil splash screen
	// TP: Move to demo.cpp, lets keep things tidy
	DrawLegalSplashScreen();
//#endif


#ifndef CONSUMERDEMO
	LoadSplashScreen(&gameStatus.splashScreen,"splash04.bmp","splshscr");

	camera[0].fade.StartFadeUp(FADETYPE_NORMAL,1);
#else
	LoadSplashScreen(&gameStatus.splashScreen,"demo00.bmp","splshscr");
#endif

	// load string table
	mainStringTable = LoadStringTable("main.txt","text");

	// TP: load standard resident packages
	InitialiseLoadingBar();

// PP: TEMP TEST!!!!!!
#if(!((defined CONSUMERDEMO)&&(BPLATFORM == XBOX)))

#if BPLATFORM==PS2
	// TP: setup loading bar for language select
	AddToLoadingBar(PackageSize(masterPakfile,"language")+PackageSize(masterPakfile,"languagesound")+PackageSize(masterPakfile,"taz")
		+PackageSize(masterPakfile,"maingameintro")+PackageSize(masterPakfile,"ressounds"));
#else
	// TP: setup loading bar for language select
	AddToLoadingBar(PackageSize(masterPakfile,"language")+PackageSize(masterPakfile,"languagesound")+PackageSize(masterPakfile,"taz")
		+PackageSize(masterPakfile,"maingameintro")+PackageSize(masterPakfile,AppendCountryCode(parmStringBuf,"maingameintrosound"))+PackageSize(masterPakfile,"ressounds"));
#endif

#endif// PP: not xbox demo

	LoadWholePackageBkg("restex");
	LoadWholePackageBkg("glyphs");		// PP: load text glyphs at the same time as other resident textures
	LoadWholePackageBkg("resobjs");

	// IH: Load all of the resident sounds
#if BPLATFORM==GAMECUBE
	LoadAllResidentSamplesBackground();
#endif

	while (GetCurrentBkgLoadingStatus()==TRUE)
	{
		DrawLoadLoop(DRAW_SPLASH|DRAW_LOADBAR|DRAW_FADE);
	}

	// IH: Upload all of the resident sounds
#if BPLATFORM==GAMECUBE
	MoveAllResidentSamplesToARAM();
#endif

	// load font
	standardFont = LoadFont("yikes.ttf","restex");

	// PP: Initialise text effects// PP: this has gotter be done real early like,
	// PP: certainly before any books are set up
	TextFX_init();

	// TP: Initialise the music system
	musicSystem.InitialiseMusicSystem();

	// TP: Initialise the sound effects
	InitialiseSoundFX();

	#if((defined INFOGRAMES) || (defined PHIL) || (defined PHILFOGRAMES))
	// TP: Only music on final version, it pumps my drive baby
	gameStatus.enableMusic = true;
	#endif
	gameStatus.enableMusic = true;

	// init extra systems
	InitCollectibleSystem();
	InitDebrisSystem();
	InitFireExtraSystem();
	InitScenicFireworksExtraSystem();
	InitZzzExtraSystem();
	InitSplashExtraSystem();
	InitExplosionExtraSystem();
	InitFountainExtraSystem();
#ifndef CONSUMERDEMO
	InitVoodooCloudExtraSystem();
	InitCheeseExtraSystem();
#endif
	InitPhoneBoxExtraSystem();
	InitCrazySparksExtraSystem();
	InitFireworksExtraSystem();
	InitHaarExtraSystem();
	InitWaterfallExtraSystem();
	InitStaveExtraSystem();
	InitTarBubbleExtraSystem();
	InitAirconExtraSystem();
	InitPlinthGlowExtraSystem();
	InitLavaSpitExtraSystem();
	InitSquishExtraSystem();
	InitPosterBounties();
	InitialiseSubtitles();

	// init profiler
	InitialiseProfile(&profile);
	InitialiseProfile(&tonyProfile);

	SetGameState(&gameStatus,GAMESTATE_SPLASH,GAMESUBSTATE_NORMAL);

	camera[0].fade.SetFadeUp(); 
	
	SetScene(&gameStatus, SCENE_NOSCENE);

	// PP: start timing the frame
	startTimingFrame();

	gameStatus.quitNow = 0;
	gameStatus.multiplayer.versusCPU = true;
	gameStatus.gfxSettings.filterMode = BDTEXTUREFILTER_LINEAR;

	// PP: I REALISE THIS ISN'T A GREAT PLACE TO PUT THIS - PLEASE FEEL FREE TO MOVE IT SOMEWHERE NICER IF YOU KNOW OF SUCH A PLACE
	// PP: nothing in the scene should be drawn with alpha framewrite, because if the rendering is to an offscreen target you'll get nasty see-through bits
	SETFRAMEWRITE(rgbFramewriteChannels);

	// PP: ...better to use gameStatus.globalSettings volumes which will have been initialised to defaults
	bsSetGlobalAudioStreamVolume(gameStatus.globalSettings.musicVolume);

	bsSetGlobalSampleVolume(gameStatus.globalSettings.sfxVolume);

	// PP: Initialise cheats (before setting up gui)
	InitialiseCheats();

	CreateMenuBar();
	CreateSceneMenu();

	LoadPermanentResources();
	LoadActor("tazcube.obe","resobjs",ACTOR_PERMANENT);

	// PP: Sound initialisation at the beginning of the game
	SFX_gameInit();

	// PP: Initialise pause menu
	Pause_init();

	// PP: Initialise control
	Control_init();

	// PP: Initialise video effects
	VFX_init();

	// PP: initialise fireballs
	Fireball_init();

	// PP: Initialise the memory card load/save dialogues
	MemCard_init();

	// PP: initialise stuff for paper books
	PaperBook_init();

	// PP: initialise stuff for shadow-casting lights
	initShadowLights();

#ifdef PHIL
	// PP: Initialise stuff for me at the beginning of the game
	InitialisePhilGame();
#endif// PP: def PHIL

#ifdef TONY
	// PP: Initialise stuff for Tony at the beginning of the game
	InitialiseTonyGame();
#endif// PP: def TONY

	// PP: Initialise score overlays book
	for (i=0;i<MAX_PLAYERS;i++)	InitialisePlayerStats(&gameStatus.player[i]);

	// TP: Make sure there is some kind of camera setup for the first time round the main loop.
	SendCamera(&camera[0]);
	bdSetupView();

	InitLoadBook();
	InitRuleBook();	// JW: Boss rule book

#ifndef CONSUMERDEMO
	// TP: For debug purposes make sure the loadgame is performed even if not entering via the languageselect.
	// PP: 'skipLanguage' will initially be true on platforms that bypass language select on bootup.
	// PP:		on these platforms, always check the save game on bootup.
	if(skipLanguage || (START_SCENE!=SCENE_LANGUAGESELECT))
	{
		CheckMemCardOnBoot();
	}
#endif// PP: def CONSUMERDEMO
	
	if(i == 0)
	{
		// PP: update sound according to viewpoint of camera 0
		bsUpdate((int)(gameStatus.deltaTime));
	}

#ifdef CONSUMERDEMO
#if(BPLATFORM == XBOX || BPLATFORM == PC)

	// PP: *** SKIP THE LANGUAGE SELECT LOADUP - NO BENEFIT TO IT ON XBOX DEMO ***

	// PP: create event managers - need to do this here because Xbox consumer demo skips Language Select loadup
	CreateEventManagers();
	CreateEventHandlers();

	if(CONSUMER_DEMO_PLAYABLE)
	{
		// PP: Pretend the Language Select scene has just finished
		FADE	tempFade;
		StartDemoMenuFadeDownCallback(&tempFade);
	}

#endif// PP: xbox
#endif// PP: demo

	while(true)
	{
		ResetCollStats();

		// TP: Read controller input
		padState = biReadDevices();

		if (gameStatus.demoMode == -1)
		{	
			// JW: Recording
			// TP: Just having a bit of a spring clean
			RecordingDemo();
		}
	
		if(ROLLING_DEMO_ACTIVELY_PLAYING)// PP: (if the rolling demo is actively playing right now)
		{	
			// JW: Playing
			if (dmData)
			{
				PlayingDemo();
			}
		}
		
		// TP: Read the controllers
		// PP: update controller1 even if gameStatus.pad1Present is false, because
		// PP: controller1's idleTime is relied upon even in situations where the pad might not be present.
		// PP: REMOUT:  if(gameStatus.pad1Present || (gameStatus.demoMode == 1/*running*/))
		UpdateController(&controller1);// PP: demoMode: horrible bodge.  Unless we update controller1, the key-record data won't get used properly.

		if (gameStatus.pad2Present) UpdateController(&controller2);

		// PP: also read controllers 3 & 4
		// PP: this is needed in order to determine which pads the two players have chosen to use
#if(NUM_CONTROLLER_PORTS == 4)
		if(padCheckMode != PADCHECK_NORMAL)
		{
			UpdateController(&controller3);
			UpdateController(&controller4);
		}
#endif// PP: 4 ports

#ifdef CONSUMERDEMO
#if (BPLATFORM == PS2)
	if (controller1.startChannel->value && controller1.selectChannel->value)
	{
		skipLanguage = 0;
		
		keepOnLooping = 0;
		
		videoSkipEnable = false;

		FlushRumbleQueue(&controller1);
		FlushRumbleQueue(&controller2);

		biForceDebounce(controller1.startChannel);
		biForceDebounce(controller1.selectChannel);

		DemoForceQuit();
	}
#endif
#endif
		
		// TP: Update music system
		musicSystem.UpdateMusicSystem();

		// PP: update memory card display
		MemCard_update();

		// PP: set the current eye to draw (in stereoscopic mode)
		VFX_stereoEye=0;

		// PP: MOVE THIS!!!!!
		FadeFieldOfView();

		// PP: Update the book list
		BookList_update();

		switch(GetCurrentGameState(&gameStatus))
		{
		case GAMESTATE_INGAME:

			// check for pause
			// JW: Check for multiplayer pause - not allowed
			// JW: Or on the demo mode
			if ((controller1.startDebounceChannel->value)&&OkToEnterPause())
			{
				if(CheatActive(CHEAT_TIMEFREEZE))
				{
					// PP: start button takes us out of time-freeze
					DeactivateCheat(CHEAT_TIMEFREEZE);
				}
				else if(flyCamActive)
				{
#ifdef PHIL// PP: me only 
					// PP: Aye, if you is in fly cam and you presses start, you get da fing wot I like to call "SLED"
					toggleShadowLightDebug();
#endif// PP: me only
				}
				else
				{
					if (gameStatus.multiplayer.versusCPU)
						EnterPauseMode();
					else
					{

#if BPLATFORM != PC
						EnterMPPause(0);
#else
						// NH: On PC use a Fullscreen Multiplayer Pause
						EnterMPPause(0, true);					
#endif // NH: #if BPLATFORM != PC
					}

					// PP: REMOUT: now done in EnterPauseMode/EnterMPPause	SetGameState(&gameStatus,GAMESTATE_PAUSE,GAMESUBSTATE_NORMAL);
				}
			}

			gameStatus.playTime += rawFTime;

			PlayRandomAmbients(map.sceneNumber);

			MainLoop(ML_ALL&(~ML_PAPERBOOK));// PP: ~ML_PAPERBOOK: paperbook draw & update should only get done when we're in GAMESTATE_PAPERBOOK			

			// NH: NOT NEEDED ON PC: P1 Will ALWAYS control pause on PC.  Pause button is the same for both players, so what else could I do?
#if BPLATFORM != PC
			if (controller2.startDebounceChannel->value && !gameStatus.multiplayer.versusCPU&&OkToEnterPause())
			{
				EnterMPPause(1);
			}
#endif // NH: #if BPLATFORM != PC
 
			break;

		case GAMESTATE_INTRO:
			// PP: subdued music during intros
			musicSystem.RequestMood(0,STREAMMOOD_SLOW);

			if (gameStatus.introState==INTRO_CUTSCENE)
			{
				MainLoop(ML_CUTSCENE|(ML_ALL&(~(ML_PROCESSINPUT|ML_PAPERBOOK))));// PP: ~ML_PAPERBOOK: paperbook draw & update should only get done when we're in GAMESTATE_PAPERBOOK
			}
			else
			{
				MainLoop(ML_ALL&(~(ML_PROCESSINPUT|ML_PAPERBOOK)));// PP: ~ML_PAPERBOOK: paperbook draw & update should only get done when we're in GAMESTATE_PAPERBOOK
			}

			GameStateIntro();

			break;

		case GAMESTATE_PAPERBOOK:// PP: reading a paperbook
			{
				MainLoop(ML_PAPERBOOK);
			}
			break;

		case GAMESTATE_OUTRO:
			if (gameStatus.outroState==OUTRO_ENDCUTSCENE)
			{
				MainLoop(ML_CUTSCENE|(ML_ALL&(~(ML_PROCESSINPUT|ML_PAPERBOOK))));// PP: ~ML_PAPERBOOK: paperbook draw & update should only get done when we're in GAMESTATE_PAPERBOOK
			}
			else
			{
				MainLoop(ML_ALL&(~(ML_PROCESSINPUT|ML_PAPERBOOK)));// PP: ~ML_PAPERBOOK: paperbook draw & update should only get done when we're in GAMESTATE_PAPERBOOK
			}

			if (map.sceneNumber == SCENE_FRONTEND)
			{
				SetGameState(&gameStatus,GAMESTATE_INGAME,GAMESUBSTATE_NORMAL);
				break;
			}

			ProcessLevelOutro();
			break;

		case GAMESTATE_CUSTOMPAUSE:
			MainLoop(ML_DRAWEXTRAS|ML_DRAWSCENE|ML_UPDATEFADE, customPauseUpdateFunc, customPauseUpdateFuncContext);
			break;

		case GAMESTATE_PAUSE:
			if (gameStatus.multiplayer.versusCPU)
				MainLoop(ML_DRAWEXTRAS|ML_DRAWSCENE|ML_UPDATEFADE, DefaultPauseUpdateFunc, 0, DefaultPausePredrawFunc, 0);
			else
				MainLoop(ML_DRAWEXTRAS|ML_DRAWSCENE|ML_UPDATEFADE, DefaultMPPauseUpdateFunc, 0, DefaultPausePredrawFunc, 0);
			
			UpdateSubtitles();			
			break;

		case GAMESTATE_CAMERACASE:
			{
				MainLoop(ML_ALL&(~(ML_PROCESSINPUT|ML_PAPERBOOK)));// PP: ~ML_PAPERBOOK: paperbook draw & update should only get done when we're in GAMESTATE_PAPERBOOK

				UpdateInGameCameraCase();
			}
			break;

		case GAMESTATE_MEMCARD:
			MainLoop(ML_UPDATEEXTRAS|ML_DRAWSCENE|ML_DRAWEXTRAS);
			break;
		case GAMESTATE_OBJECTVIEWER:
			MainObjectViewer();

			break;
#ifndef CONSUMERDEMO
		case GAMESTATE_BOSSCONTINUE:
			MainBossContinue();
			break;
		case GAMESTATE_CREDITS:
			MainCredits();
			break;
#endif
		case GAMESTATE_FADING:
			MainLoop(ML_DRAWSCENE|ML_UPDATEFADE|ML_DRAWEXTRAS);

			if ((camera[0].fade.GetFadeState() == FADESTATE_UP)||(camera[0].fade.GetFadeState() == FADESTATE_OFF))//(FinishedFade(&camera[0].fade))
			{
				if (gameStatus.currentSceneType==SCENETYPE_INGAME)
				{
					SetGameState(&gameStatus,GAMESTATE_INGAME,GAMESUBSTATE_NORMAL);
				}
				else
				{
					if (gameStatus.currentSceneType==SCENETYPE_INTRO)
					{
						SetGameState(&gameStatus,GAMESTATE_INTRO,GAMESUBSTATE_NORMAL);
					}
					else
					{
						SetGameState(&gameStatus,GAMESTATE_CUTSCENE,GAMESUBSTATE_NORMAL);
					}
				}
			}
			break;
		case GAMESTATE_SPLASH:
			MainSplash();

			// PP: MOVEME?: Update and draw the Book List
			BookList_update();
			BookList_draw();

			break;

// PP: This stuff's only done if it's a consumer demo build
#ifdef CONSUMERDEMO

		case GAMESTATE_DEMOMENU:
			DoDemoMenu();
			// PP: MOVEME?: Update and draw the Book List
			BookList_update();
			BookList_draw();

			break;
		case GAMESTATE_DEMOQUIT:
			DoDemoQuit();

			// PP: MOVEME?: Update and draw the Book List
			BookList_update();
			BookList_draw();

			break;

#endif// PP: def CONSUMERDEMO	// PP: This stuff's only done if it's a consumer demo build

		}// PP: end switch(GetCurrentGameState(&gameStatus))

#ifdef INFOGRAMES	// JW: This is a BIG if statement
		if ((controller1.selectDebounceChannel->value)&&OkToEnterMap())
		{
			if (map.sceneNumber == SCENE_ZOOHUB || map.sceneNumber == SCENE_CITYHUB || map.sceneNumber == SCENE_WILDWESTHUB || 
				map.sceneNumber == SCENE_ZOOBOSS || map.sceneNumber == SCENE_CITYBOSS || map.sceneNumber == SCENE_WESTBOSS || map.sceneNumber == SCENE_TAZPREBOSS || map.sceneNumber == SCENE_TAZBOSS ||
				map.sceneNumber == SCENE_FRONTEND || map.sceneNumber == SCENE_LANGUAGESELECT || 
				map.sceneNumber == SCENE_MUSEUMVEHICLERACE || map.sceneNumber == SCENE_GRANDCVEHICLERACE || map.sceneNumber == SCENE_DEPTSTRVEHICLERACE ||
				map.sceneNumber == SCENE_DESTRUCTIONAQUA || map.sceneNumber == SCENE_DESTRUCTIONCONSTRUCT || map.sceneNumber == SCENE_DESTRUCTIONTASMANIA ||
				map.sceneNumber == SCENE_ICEDOMERACE || map.sceneNumber == SCENE_GOLDMINERACE || map.sceneNumber == SCENE_SAFARIRACE)						
			{}
			else
			{
				EnterPauseMode(PAUSE_MAP);
			}
		}

#ifdef CONSUMERDEMO_RECORD_DEMO

		// PP: if we're intending to record the demo, let us at the gui via pad 1

		if (controller1.guiDebounceChannel->value)
		{
			biZeroMap(controller1.inputMap);
			UpdateGuiItems();
			bgEnterGui(GuiCallback);
		}

#endif// PP: def CONSUMERDEMO_RECORD_DEMO

#ifndef CONSUMERDEMO

		if (controller2.guiDebounceChannel->value)
		{
			biZeroMap(controller1.inputMap);
			UpdateGuiItems();
			bgEnterGui(GuiCallback);
		}

#endif// PP: ndef CONSUMERDEMO

	#else// PP: ifndef INFOGRAMES

		if ((controller1.guiDebounceChannel->value))
		{
			biZeroMap(controller1.inputMap);
			UpdateGuiItems();
			bgEnterGui(GuiCallback);
		}

	#endif// PP: ndef INFOGRAMES

#if BPLATFORM == PC		
		DrawMouse();
#endif // NH: #if BPLATFORM == PC

		bgPoll();
		DisplayGuiInfo();

#if BPLATFORM==PS2
		tcount = *T0_COUNT;
#endif

		EndProfileTimer(&profile,0);
		EndProfile(&profile);
		EndProfile(&tonyProfile);

#if BPLATFORM==GAMECUBE
		if(heapshow)
			bkHeapStats();
#endif

#ifdef _TAZDEBUG// PP: bkHeapShowGraphical doesn't seem to exist in debug mode
#ifndef INFOGRAMES
		if (heapshow)
 		{
 			bkHeapShowGraphical(BALLGROUPS, 0.0f, standardFont);
// 			bkHeapShow();
 		}
#endif
#endif// PP: def _TAZDEBUG

#if (BPLATFORM != PC)
		// NH: Check for missing controllers on all platforms EXCEPT PC
		if ((gameStatus.demoMode)||((map.sceneNumber==SCENE_FRONTEND)&&frontEndMenu.mainMenuPage&&frontEndMenu.mainMenuPage->subState==MM_INTRO))
		{
			checkForMissingControllers2(false);
		}
		else
		{
			checkForMissingControllers2(true);
		}
#endif // NH: #if (BPLATFORM != PC)

		switch(GetCurrentGameState(&gameStatus))
		{
		case GAMESTATE_SPLASH:
			Flip(255, 255, 255, 128);
			break;
		case GAMESTATE_OBJECTVIEWER:
			Flip(64, 64, 64, 128);
			break;
		case GAMESTATE_CREDITS:
			Flip(0, 0, 0, 128);
			break;
		default:
#if BPLATFORM == PC
			// NH: Clear to fogging colour so that when envmap toggled off background colour matches fog
			Flip(map.fogColour.r, map.fogColour.g, map.fogColour.b, 128);
#else
			Flip(0, 0, 0, 128);
#endif

			break;
		}

		StartProfile(&profile);
		StartProfile(&tonyProfile);

#if BPLATFORM==PS2
		*T0_COUNT = 0;
#endif

		StartProfileTimer(&profile,0,"frame");

		// CPW: it remains to be seen what happens if we 'quit' the program on a PS2...
		if(gameStatus.quitNow)
			break;

		// PP: increment frame number
		gameStatus.frameNumber++;

	}// PP: end while(keepOnLooping)

	for (i=0;i<MAX_PLAYERS;i++)
	{
		DeletePlayerStats(&gameStatus.player[i]);
	}

	KillRuleBook();

	shadowLightEditBook.shutdown();

	// PP: shutdown the memory card load/save dialogues
	MemCard_shutdown();

	// PP: shutdown shockwaves
	Shockwave_shutdown();

	// PP: shutdown cheat stuff
	ShutdownCheats();

	// PP: shutdown video effects
	VFX_shutdown();

	// PP: Shutdown control
	Control_shutdown();

	// PP: shutdown the pause menu
	Pause_shutdown();

	FreePermanentResources();

	musicSystem.ShutdownMusicSystem();

	DeleteInputChannels(&controller1);
	DeleteInputChannels(&controller2);

	baDeleteExtraSystem(NULL);

	FreeSplashScreen(&gameStatus.splashScreen);
	FreeScene(gameStatus.currentScene);
	FreeStringTable(mainStringTable);
	FreeFont(standardFont);

	bkDeleteAllResources();

	FreeWholePackage("glyphs");			// PP: free glyphs at the same time as other resident textures
	FreeWholePackage("restex");
	FreeWholePackage("ressounds");

#if BPLATFORM==GAMECUBE
	FreeAllResidentSamplesFromARAM();
#endif

	bgShutdownGui();

	// TP: Close the master file system
	CloseFileSystem();

	bdCloseDisplay();

#if BPLATFORM == PC
	// NH: Just incase the cursor is still hidden, make sure it comes back. Having no cursor was a right bitch.
	ShowCursor(true);

	// NH: Enables task switching
	SystemParametersInfo (SPI_SETSCREENSAVERRUNNING, FALSE, &previousState, 0);
#endif // NH: #if BPLATFORM == PC

	// PP: babel is about to shut down; don't try to free anything after this point
	BabelHasShutdown=true;

	bkHeapShow();

	bkShutdown();

#if BPLATFORM == PC
	// NH: If game quit because the game CD was not found in a drive, display an appropriate error
	if (gameStatus.noCDFound)
	{
	}
#endif // NH: #if BPLATFORM == PC

#ifdef SUGAR_COATED_CRASHES
#ifdef _MASTER
#if BPLATFORM!=PS2	// TP: Not on PS2, exception handling takes up to 250k apparently so I have disabled it
	}
	catch(...)
	{
		// PP: Should the unthinkable happen - should the final game crash for any reason -
		// PP: let's put up the "damaged disk" message.
		// PP: It's a whole lot prettier than a frozen screen!
		badDisk();
	}
#endif
#endif// PP:  _MASTER
#endif// PP: def SUGAR_COATED_CRASHES

}// PP: end func 'main'


/*  --------------------------------------------------------------------------------
	Function	: Flip
	Purpose		: Called to flip the screen, and in the case of xbox render the consolewindow
	Parameters	: r,g,b,a,flags
	Returns		: 
	Info		: 
*/

void Flip(int red, int green, int blue, int alpha, int flags)
{
#if BPLATFORM==XBOX
	bRenderConsoleWindow();
#endif
	bdFlip(red, green, blue, alpha, flags);
}


/*  --------------------------------------------------------------------------------
	Function	: ProcessLevelOutro
	Purpose		: Handles the end of a level
	Parameters	: 
	Returns		: 
	Info		: 
*/

void ProcessLevelOutro(void)
{
	int i;

	switch(gameStatus.outroState)
	{
	case OUTRO_ENDCUTSCENE:
		if (map.exitCutScene==NULL)
		{
// PP: REMOUT: GOT PROPER CUTSCENE MUSIC NOW; GONNA MOVE THIS TO OUTRO_STATS			musicSystem.RequestMood(0,STREAMMOOD_SLOW);

			if ((IN_HUB)||(IN_BOSS_GAME)||(map.sceneNumber==SCENE_LANGUAGESELECT))
			{
				gameStatus.outroState = OUTRO_EXIT;
			}
			else
			{
				gameStatus.outroState = OUTRO_STATS;
				CreateStatsAtEndOfLevel(gameStatus.player[0].stats);
			}
		}
		else
		{
			// PP: frantic music during outro cutscene
			musicSystem.RequestMood(0,STREAMMOOD_FAST);

			if (map.exitCutScene->State()==CUTSCENE_PLAYING)
			{
				// PP: if the player has pressed a button to skip the cutscene
				if (map.sceneNumber == SCENE_LANGUAGESELECT && map.lastScene == SCENE_FRONTEND)
				{	// JW: We're playing the main game intro again...if we're on x-box check all the buttons
					if (AttractInterruptButton()
						||attractInterrupt)// PP: attractInterrupt will be set true by actions that must also interrupt the attract sequence, eg. controller insertion on Xbox (Xbox TCR 3-40)
					{
						attractInterrupt = false;
						map.exitCutScene->Skip();
					}
				}
				else
				{	// JW: we ain't...so don't
					if(CUTSCENE_SKIP)
					{
						map.exitCutScene->Skip();
					}
				}

				if (map.exitCutScene->FinishedPlaying())
				{
					map.exitCutScene->Stop();

					controller1.idleTime = 0.0f;

					if (map.exitCutScenePt2)
					{
						SAFE_DELETE(map.exitCutScene);
						map.exitCutScene = map.exitCutScenePt2;
						map.exitCutScenePt2 = NULL;
						// TP: Load part two
						map.exitCutScene->Load();
					}
					else
					{
						if ((IN_HUB)||(IN_BOSS_GAME)||(map.sceneNumber==SCENE_LANGUAGESELECT))
						{
							gameStatus.outroState = OUTRO_EXIT;
						}
						else
						{
							// PP: subdued music during outro menus
							musicSystem.RequestMood(0,STREAMMOOD_SLOW);

							gameStatus.outroState = OUTRO_STATS;
							CreateStatsAtEndOfLevel(gameStatus.player[0].stats);
						}
						bkGenerateEvent("camera", "_case=stat_cam");
						RespondToCameraMessages();
					}
				}
				else
				{
					map.exitCutScene->Update();
				}
			}
			else
			{
				// TP: if the end of level cameracase is playing this will start the cutscene automatically
				if ((!camera[0].cameraCase)||(~camera[0].cameraCase->flags&CAMERACASE_EXITCUTSCENE))
				{
					map.exitCutScene->Setup();
					map.exitCutScene->Play(&camera[0]);
				}
			}
		}
		break;
	case OUTRO_STATS:

		// PP: use subdued music for outro stats
		musicSystem.RequestMood(0,STREAMMOOD_SLOW);

#ifdef CONSUMERDEMO
		if ((controller1.idleTime > to_inactive) && (camera[0].fade.GetFadeValue() == 0.0f))
		{
			BookList_closeAll();
			camera[0].fade.StartFadeDown(FADETYPE_NORMAL, 0.5f);

			camera[0].fade.SetFadeDownFinishedCallback(StopDemoGameFadeDownCallback);
		}
#endif

		//gameStatus.player[0].stats->outroState = OUTRO_HIGHSCORES;
		if(OKBUTTON)// PP: REMOUT: can't accept start button cos it says press X (bug 349):		(controller1.startDebounceChannel->value)||
		{
#ifdef CONSUMERDEMO
			BookList_closeAll();
			camera[0].fade.StartFadeDown(FADETYPE_NORMAL, 0.5f);

			camera[0].fade.SetFadeDownFinishedCallback(StopDemoGameFadeDownCallback);
#else
			gameStatus.player[0].stats->letters[0] = gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].lastName[0];
			gameStatus.player[0].stats->letters[1] = gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].lastName[1];
			gameStatus.player[0].stats->letters[2] = gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].lastName[2];
			gameStatus.player[0].stats->iconNumber = gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].lastIcon;
			if(CheckRankAgainstCurrentTopTen(gameStatus.player[0].stats,(EScene)map.sceneNumber))
			{
				/*gameStatus.outroState = OUTRO_NAMEENTRY;
				CreateNameEntryPageAtEndOfLevel(gameStatus.player[0].stats);
				gameStatus.player[0].stats->endOfLevelStatsBook->gotoPage("SelectIcon");*/
				AddNewRecordToStats(gameStatus.player[0].stats,(EScene)map.sceneNumber);
			}
			CreateHighScoreTablePage(gameStatus.player[0].stats,(EScene)map.sceneNumber);
			gameStatus.player[0].stats->endOfLevelStatsBook->gotoPage("highScores");
			gameStatus.player[0].stats->nameEntryStage = HSE_CHOOSEICON;
			gameStatus.outroState = OUTRO_HIGHSCORES;
			break;
#endif
		}
		break;
	case OUTRO_NAMEENTRY:
		break;
	case OUTRO_HIGHSCORES:
		if((gameStatus.player[0].stats->statsBookClock += fTime) > 2.0f)
		{
			if(OKBUTTON)// PP: REMOUT: nicer & safer to use OKBUTTON
			{						
				if (processGalleryScore())
				{
					CreateInformPageAtEndOfLevel(gameStatus.player[0].stats);
					gameStatus.player[0].stats->endOfLevelStatsBook->gotoPage("GalleryInform");

					gameStatus.outroState = OUTRO_INFORM;
				}
				else
					gameStatus.outroState = OUTRO_EXIT;
			}
		}
		break;
	case OUTRO_INFORM:
		if((gameStatus.player[0].stats->statsBookClock += fTime) > 2.0f)
		{
			if(OKBUTTON)
			{
				gameStatus.outroState = OUTRO_EXIT;
			}
		}
		break;
	case OUTRO_EXIT:
		
		camera[0].fade.StartFadeUp(FADETYPE_NORMAL,1.0f);

		if (gameStatus.multiplayer.numScreens == 2)
			camera[1].fade.StartFadeUp(FADETYPE_NORMAL,1.0f);

		SetGameState(&gameStatus,GAMESTATE_INGAME,GAMESUBSTATE_NORMAL);
		StopCameraCase(&camera[0],camera[0].cameraCase);
		switch(map.sceneNumber)
		{
		case SCENE_LANGUAGESELECT:
			bkGenerateEvent("scenechange","_frontend");
			break;
		case SCENE_ZOOHUB:
			bkGenerateEvent("scenechange","_zooboss");
			break;
		case SCENE_WILDWESTHUB:
			bkGenerateEvent("scenechange","_westboss");
			break;
		case SCENE_CITYHUB:
			gameStatus.multiplayer.numPlayers = 2;
			bkGenerateEvent("scenechange","_cityboss");
			break;
		case SCENE_ICEDOME:
		case SCENE_SAFARI:
		case SCENE_AQUA:
			bkGenerateEvent("scenechange","_zoohub");
			break;
		case SCENE_DEPTSTR:
		case SCENE_MUSEUM:
		case SCENE_CONSTRUCT:
		case SCENE_ZOOBOSS:		// TP: assumes level was completed
			bkGenerateEvent("scenechange","_cityhub");
			break;
		case SCENE_GHOSTTOWN:
		case SCENE_GOLDMINE:
		case SCENE_GRANDCANYON:
		case SCENE_CITYBOSS:		// TP: assumes level was completed
			bkGenerateEvent("scenechange","_westhub");
			break;
		case SCENE_WESTBOSS:		// TP: assumes level was completed
			if (!(gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[SCENE_TAZHUB].flags & LEVELFLAG_SEENINTRO))
			{
				bkGenerateEvent("scenechange","_tasmania_intro");
			}
			else
			{
				bkGenerateEvent("scenechange","_tazhub");
			}
			break;
		case SCENE_TAZHUB:
			bkGenerateEvent("scenechange","_tazboss1");
			break;
		case SCENE_TAZPREBOSS:
			bkGenerateEvent("scenechange","_tazboss2");
			break;
#ifndef CONSUMERDEMO
		case SCENE_TAZBOSS:
// CMD: 	bkGenerateEvent("scenechange","frontend");
			InitialiseEndGameCredits();
			SetGameState(&gameStatus, GAMESTATE_CREDITS, GAMESUBSTATE_NORMAL);
			break;
#endif
		case SCENE_ICEDOMERACE:
		case SCENE_SAFARIRACE:
		case SCENE_DESTRUCTIONAQUA:
			bkGenerateEvent("scenechange","_zoohub");
			break;

		case SCENE_MUSEUMVEHICLERACE:
		case SCENE_DEPTSTRVEHICLERACE:
		case SCENE_DESTRUCTIONCONSTRUCT:
			bkGenerateEvent("scenechange","_cityhub");
			break;

		case SCENE_GRANDCVEHICLERACE:
		case SCENE_DESTRUCTIONTASMANIA:
		case SCENE_GOLDMINERACE:
			bkGenerateEvent("scenechange","_westhub");
			break;
		}
		for(i=0;i<gameStatus.multiplayer.numPlayers;i++)
		{
			if (gameStatus.player[i].actorInstance)
			{
				RequestStateChange(gameStatus.player[i].actorInstance, STATE_MOVE);
			}
			if((gameStatus.player[i].stats)&&(gameStatus.player[i].stats->endOfLevelStatsBook))
			{
				gameStatus.player[i].stats->endOfLevelStatsBook->close();
			}
		}
		break;
	}
}


/*  --------------------------------------------------------------------------------
	Function	: MainLoop
	Purpose		: The main game loop
	Parameters	: flag options for update, custom callbacks..
	Returns		: 
	Info		: 
*/

void MainLoop(int options,PauseUpdateFunc updateFunc, const int32 updateFuncContext, PauseUpdateFunc predrawUpdateFunc, const int32 predrawFuncContext)
{
	int				i,j;
	extern bool		g_overrideStereoscopy;			// PP: flag specifying that stereoscopy is to be overridden (display,cpp)

	// update structs etc
	if (options&ML_UPDATEPRELEVEL) UpdateScenePreLevel(gameStatus.currentScene);

	// TP: Check and update sample callbacks
	MonitorSampleCallbacks();

	// PP: update the fly-around debug camera if it's active...

	if(flyCamActive)
	{
		updateFlyCam();
	}

	if (gameStatus.currentState == GAMESTATE_CAMERACASE)
		pushRenderTarget(target, 0,0,0,255, 1, BSETRENDERTARGET_CLEAR);

	// PP: repeat each draw for stereo vision
	for(videoMode.drawNum=0; videoMode.drawNum<videoMode.numDraws; videoMode.drawNum++)
	{
		for (i=0;i<gameStatus.multiplayer.numScreens;i++)
		{
			// PP: record which screen we're drawing
			gameStatus.multiplayer.currentScreen=i;

			// PP: apply this screen's fov
			applyFOV();

			bdSetViewClipPlanes(videoMode.nearPlane, videoMode.farPlane);

			// PP: start DOF rendering
			if(VFX_effectActive(VFX_FOCUS))
			{
				VFX_focusPreDraw();
			}

			if (gameStatus.currentState == GAMESTATE_CAMERACASE)
			{	
				bdSetViewport(0, 0, 256, 256);
				bdSetupView();
			}
			else
			{
				bdSetViewport(int(videoMode.screens[i].viewport.left), int(videoMode.screens[i].viewport.top), int(videoMode.screens[i].viewport.widthAbs()), int(videoMode.screens[i].viewport.heightAbs()));
			}

			// PP: TODO?: CAMERA UPDATE HERE, BEFORE SENDING CAMERA ON FIRST DRAW PASS

			// PP: ===========
			// PP: SEND CAMERA
			// PP: ===========

			if(flyCamActive)
			{
				// PP: send the fly-around debug camera if it's active...
				SendCamera(&camera[i]);
				SendCamera(&flyCam);
			}
			else if(CheatActive(CHEAT_TAZCAM))
			{
				// PP: if the Taz-Cam cheat is active, send the Taz-Cam
				SendCamera(&tazCam);
			}
			else
			{
				if(~options & ML_PAPERBOOK)// PP: don't send camera 0 if we're reading a paper book
				{
					// PP: send the normal camera
					SendCamera(&camera[i]);
				}
			}

			bdSetupView();
			bdBeginScene();

			if((~options & ML_PAPERBOOK) && (gameStatus.currentState != GAMESTATE_CAMERACASE))// PP: I don't want the position of the paperbook camera to have any effect on sound
			{
				// PP: update sound according to viewpoint of camera 0

				if(i == 0)
				{
					bsUpdate((int)(gameStatus.deltaTime));
					// IH: Update the effects, didn't know where else to put this
					#if BPLATFORM==GAMECUBE
						UpdateEffects();
					#endif
				}
			}

			// PP: ==========
			// PP: LEVEL DRAW
			// PP: ==========

			if (predrawUpdateFunc)
			{
				predrawUpdateFunc(predrawFuncContext);
			}

			if(gameStatus.multiplayer.numScreens == 1)// PP: stereo vision & motion blur only allowed in single screen
			{
				if(VFX_effectActive(VFX_STEREO))
				{
					if(!g_overrideStereoscopy)
					{
						// PP: Prepare to draw a scene stereoscopically
						VFX_stereoPreDraw();
					}
				}
				
				// PP: Prepare to draw with motion blur
				if(VFX_effectActive(VFX_MOTIONBLUR))
				{
					VFX_motionBlurPreDraw();
				}
			}

			// PP: if we're preparing a spinoff, draw the scene to the VFX_renderTarget
			if(VFX_spinoffPrePreparing)
			{
				VFX_spinoffPreparePreDraw();
			}

			if (options&ML_DRAWSCENE) StartSceneLevelDraw(gameStatus.currentScene,&camera[i]);
		

			// PP: ======
			// PP: UPDATE
			// PP: ======

			// PP: only update for one eye in stereo vision - we don't want lag between the two images
			if(videoMode.drawNum == 0)// PP: update on first pass because UpdateScene comes before DrawScene
			{
				// prepare all data in current scene first time only
				if (i==0)
				{
					// sort out extra's update
					StartProfileTimer(&profile,9,"Update extra");

					// update extra system
					if (options&ML_UPDATEEXTRAS) baUpdateExtraSystem(NULL,fTime);

					EndProfileTimer(&profile,9);

					if (options&ML_UPDATESCENE) UpdateScene(gameStatus.currentScene);

					if(options & ML_PAPERBOOK)
					{
						// PP: update the current paper book
						PaperBook_update();
					}
				}
				
				for (j=0;j<gameStatus.multiplayer.numPlayers;j++)
				{
					if (gameStatus.player[j].camera == &camera[i])
					{
						// TP: If player is using the current camera then process their input
						if (j == 0 || !gameStatus.multiplayer.versusCPU)
						{
							if (options&ML_PROCESSINPUT) ProcessInput(&gameStatus.player[j]);
						}
					}
				}

				if(ROLLING_DEMO_ACTIVELY_PLAYING)// PP: (if the rolling demo is actively playing right now)
				{	
					// PP: Playing
					if(dmData)
					{
						// PP: check for, and respond to, interruption of the rolling demo
						CheckForDemoInterrupt();
					}
				}

				if (i==0)
				{
					// TP: Need prepare to come after the process input for the platform code to work properly
					if ((options&ML_PREPARESCENE)&&(~options&ML_CUTSCENE)) PrepareScene(gameStatus.currentScene);
				}
			}

			// PP: =============
			// PP: CAMERA UPDATE
			// PP: =============

			/*TBVector	oldCamPos;
			bmVectorCopy(oldCamPos, camera[i].pos);*/
			// PP: only update the camera upon leaving the draw, not after drawing first eye
			if(videoMode.drawNum == (videoMode.numDraws-1))
			{
				StartProfileTimer(&profile,5,"Camera");

				if (options&ML_UPDATECAMERA) UpdateCamera(&camera[i]);

				EndProfileTimer(&profile,5);

				// PP: update the Taz Cam if the Taz Cam cheat is on
				if(CheatActive(CHEAT_TAZCAM))
				{
					updateTazCam();
				}
			}

			/*if (options&ML_CUTSCENE)
			{
				if (bmVectorDistance(oldCamPos, camera[i].pos) > METERTOUNIT(10.0f))
				{
					bkPrintf("Camera cut\n");
					bdClear(0,0,0,255,0,0,BDCLEARMODE_FRAME);
					options &= ~ML_DRAWSCENE;
				}
			}*/

			// PP: =========
			// PP: DRAWSCENE
			// PP: =========

			//draw scene
			if (options&ML_DRAWSCENE) DrawScene(gameStatus.currentScene,&camera[i]);

			// draw collectibles last because of shadows
			StartProfileTimer(&profile,8,"Draw extra");
			if (options&ML_DRAWEXTRAS) baRenderExtraSystem(NULL, (void*)EXTRA_SOLID);
			EndProfileTimer(&profile,8);

			if (options&ML_DRAWSCENE)
			{
				StartSceneLevelDrawAlpha(gameStatus.currentScene,&camera[i]);
				DrawScene2(gameStatus.currentScene,&camera[i],options);
			}

			if (options&ML_CUTSCENE)
			{
				if (i==0)
				{
					// PP: only prep actors after last draw pass, not after each draw pass
					if(videoMode.drawNum == (videoMode.numDraws-1))
					{
						if (options&ML_PREPARESCENE) PrepareScene(gameStatus.currentScene);
					}
				}
			}

			// PP: end DOF rendering
			if(VFX_effectActive(VFX_FOCUS))
			{
				VFX_focusPostDraw();
			}

			// draw text
	//		if(!IN_BOSS_GAME)										// PP: I don't want standard overlays in the boss games
			{
				for (j=0;j<gameStatus.multiplayer.numPlayers;j++)
				{
					if (gameStatus.player[j].camera == &camera[i])
					{
						PrintGameText(&gameStatus.player[j]);
					}
				}
			}

			// draw timers
			DisplayProfile(&profile);

			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
			
			// draw fade
			camera[i].fade.DrawFade();

			// PP: perhaps not the neatest way of doing it;
			// PP: no blink eyes in split screen (it causes problems)
			if(gameStatus.multiplayer.numScreens == 1)
			{
				// PP: paperbooks only allowed in 1-screen
				if(options & ML_PAPERBOOK)
				{
					// PP: Draw current paper book
					PaperBook_draw();
				}

				if(!VFX_effectActive(VFX_STEREO))
				{
					// PP: don't draw blink eyes in stereoscopic mode - they'd need attention for a start
					DrawBlinkEyes(&camera[i]);
				}
			}

			// PP: update goes after last draw pass, not after each draw pass
			if(videoMode.drawNum == (videoMode.numDraws-1))
			{
				if(updateFunc != NULL)
				{
					updateFunc(updateFuncContext);
				}
			}

			bdEndScene();

			// update fade
			if (options&ML_UPDATEFADE) camera[i].fade.UpdateFade();

		}// PP: end for each screen
		

		if(gameStatus.multiplayer.numScreens > 1)
		{
	//		setFOV(CURRENT_SCREEN.fov);
			bdSetViewClipPlanes(videoMode.nearPlane, videoMode.farPlane);
			bdSetViewport(0, 0, videoMode.xScreen, videoMode.yScreen);
			SendCamera(&camera[0]);
			bdSetupView();

			// PP: DRAW ANYTHING THAT NEEDS TO GET DRAWN OVER THE TOP OF ALL SPLIT SCREENS USING ONLY THE BOUNDARIES OF THE FULL SCREEN...

			// PP: ~~~~~~~~~~~~~~~~~~~~~~~~

			gameStatus.multiplayer.currentScreen=-1;// PP: -1 means we're drawing over the whole screen, even if we've already drawn split-screens

			bdBeginScene();

			DrawSpecial(map.sceneNumber, &camera[0]);
			DrawSpecial2(map.sceneNumber, &camera[0], TRUE);
			DrawSpecial2(map.sceneNumber, &camera[0], FALSE);

			bdEndScene();
			
			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
		}
		
		// PP: final motion blur draw...

		if(VFX_effectActive(VFX_MOTIONBLUR))
		{
			VFX_motionBlurPostDraw();

			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
		}

		// PP: a bit of update for VFX_SPINOFF...

		/* PP: REMOUT: gonna try clearing this AS IT'S USED now, otherwise it could get cleared during pause and therefore not picked up after the pause	
		if(VFX_spinoffPrepared)
		{
			VFX_spinoffNewlyPrepared=false;
		}
		*/

		if(VFX_spinoffPreparing)
		{
			VFX_spinoffPostPrepare();

			// PP: got to paste this frame on from the VFX_renderTarget otherwise we'll get a black frame
			VFX_spinoffPostDraw();

			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
		}

		// PP: final screen spin-off draw...

		if(VFX_effectActive(VFX_SPINOFF)&&VFX_spinoffStarted)
		{
			VFX_spinoffPostDraw();

			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
		}

		if (gameStatus.currentState != GAMESTATE_CAMERACASE)	// JW: Don't draw any books if we're on a polaroid
		{
			// PP: Draw the book list
 			BookList_draw();
		}

		if (gameStatus.currentState == GAMESTATE_CAMERACASE)
		{
			// TP: A nice function would tidy this up, functions? what are they
			DrawPolaroid();
		}

		// PP: =====================================================================================
		// PP: ALL DRAWING SHOULD BE DONE BEFORE THIS POINT IF STEREOSCOPIC MODE IS TO WORK PROPERLY
		// PP: =====================================================================================

		// PP: stereo vision only allowed in single screen
		if(gameStatus.multiplayer.numScreens == 1)
		{				
			if(VFX_effectActive(VFX_STEREO))
			{
				if(!g_overrideStereoscopy)
				{
					// PP: finish the stereoscopic draw pass
					VFX_stereoPostDraw();
				}
			}
		}

	}// PP: end for each draw pass

	// PP: LEAVE IN: *** USE THIS TO TWEAK SHADOW MAP RADII ***

	/*if(VFX_effectActive(VFX_SHADOWMAP))
	{
//		if(shadowLightDebug)
		{
			VFX_shadowMapPostDrawDebug();

			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
		}
	}*/



	// TP: Called once a frame to make sure any background loading is performed
	GetCurrentBkgLoadingStatus();

	// PP: finish timing the frame; calculate frames per second, frame length, etc.
	finishTimingFrame();


#if(BPLATFORM != PC)
//#ifdef PROVIEW
	if (controller1.r3DebounceChannel->value)
	{
		// take screen shot
		TakeScreenShot();
	}
//#endif
#else // NH: BPLATFORM
	if (controller1.f10DebounceChannel->value)
	{
		// take screen shot
		TakeScreenShot();
	}
#endif // NH: BPLATFORM

	if (controller1.f6DebounceChannel->value) SwitchBetweenScreenModes(&videoMode);

	// end of null time
	// PP: start timing the frame
	startTimingFrame();
}


/*  --------------------------------------------------------------------------------
	Function	: GameStateIntro
	Purpose		: Called during the intro phase of the level
	Parameters	: 
	Returns		: 
	Info		: 
*/
void GameStateIntro(void)
{
	bool exitGameIntro=false;

	if (gameStatus.demoMode)
	{
		int	i;

		setNumScreens(gameStatus.multiplayer.numPlayers);

		SetGameState(&gameStatus,GAMESTATE_INGAME,GAMESUBSTATE_NORMAL);
		
		gameStatus.controlSuspend = FALSE;
		
		if (map.sceneNumber!=SCENE_FRONTEND && map.sceneNumber!=SCENE_LANGUAGESELECT)
		{
			for (i = 0; i < gameStatus.multiplayer.numScreens; i++)
				gameStatus.player[i].actorInstance->flags &= ~(ACTORFLAG_DONTDRAW|ACTORFLAG_NOCOLLISION|ACTORFLAG_NOVELOCITY);
		}

		// TP: start ingame music
		musicSystem.ChooseMusic(map.sceneNumber,MUSICTYPE_MAIN,MUSICSTATE_START);

		for (i = 0; i < gameStatus.multiplayer.numScreens; i++)
			camera[i].fade.StartFadeUp(FADETYPE_NORMAL,1.0f);

		// TP: Needed to add this to solve sound problem when exiting level quickly.
		StopAllChannels();

#if BPLATFORM!=GAMECUBE
		LoadLevelIntroSamplesMemory();
		FreeLevelIntroSamples();
		LoadLevelSamplesMemory();			//Loads samples into sound memory
#else
		MoveAllLevelSamplesToARAM();
#endif
		CompleteLoad(map.sceneNumber);		// JW: Load Enemy Samples
		PlayAmbientSounds(map.sceneNumber);

		demoBook->open();

		return;
	}

	switch(gameStatus.introState)
	{
	case INTRO_RULES:	// JW: Show rules for boss games

// PP: REMOUT: OOD		DrawRuleBook();

#ifdef CONSUMERDEMO

		if (!gameStatus.multiplayer.versusCPU)
			setNumScreens(2);
		else
			setNumScreens(1);

		sfxTimer = -1;
		
		gameStatus.introState = INTRO_LOADSFX;
#endif
		break;

	case INTRO_CUTSCENE:
		// TP: If the player has already seen the intro cutscene then skip
		if (!map.introCutScene)
		{
			// TP: Head on to the intro camera cases
			if ((map.sceneNumber==SCENE_TAZINTROCUTSCENE)||(map.sceneNumber==SCENE_STARTGAMEINTRO))
			{
				gameStatus.introState = INTRO_LOADSFX;
				sfxTimer = -1;
				camera[0].fade.SetFadeDown();
			}
			else if ((map.sceneNumber==SCENE_ZOOBOSS)||(map.sceneNumber==SCENE_CITYBOSS)||(map.sceneNumber==SCENE_WESTBOSS)||
					 (map.sceneNumber==SCENE_TAZPREBOSS)||(map.sceneNumber==SCENE_TAZBOSS))
			{
				gameStatus.introState = INTRO_RULES;

				controller1.startDebounceChannel->value = 0;

				StartRuleBook();

				sfxTimer = -1;
				camera[0].fade.SetFadeDown();
			}
			else
			{
				gameStatus.introState = INTRO_UPLOADCAMERACASESOUND;
				camera[0].fade.SetFadeDown();
			}
			break;
		}
		else
		{
			if (map.introCutScene->State()==CUTSCENE_PLAYING)
			{
				// TP: Has the cutscene finished or the player chosen to skip
				if(CUTSCENE_SKIP)
				{
					map.introCutScene->Skip();
				}

				if (map.introCutScene->FinishedPlaying())
				{
					map.introCutScene->Stop();

					// TP: If there is a second part to the cutscene then kill first and swap with second
					if (map.introCutScenePt2)
					{
						SAFE_DELETE(map.introCutScene);
						map.introCutScene = map.introCutScenePt2;
						map.introCutScenePt2 = NULL;
						// TP: Load part two
						map.introCutScene->Load();
					}
					else
					{
						// TP: Cutscene over
						camera[0].fade.SetFadeDown();
						
						if ((map.sceneNumber==SCENE_TAZINTROCUTSCENE)||(map.sceneNumber==SCENE_STARTGAMEINTRO))
						{
							gameStatus.introState = INTRO_LOADSFX;
						}
						else if ((map.sceneNumber==SCENE_ZOOBOSS)||(map.sceneNumber==SCENE_CITYBOSS)||(map.sceneNumber==SCENE_WESTBOSS)||
								(map.sceneNumber==SCENE_TAZPREBOSS)||(map.sceneNumber==SCENE_TAZBOSS))
						{
							controller1.startDebounceChannel->value = 0;
							
							gameStatus.introState = INTRO_RULES;
							
							StartRuleBook();
							
							sfxTimer = -1;
						}
						else
						{
							gameStatus.introState = INTRO_UPLOADCAMERACASESOUND;
						}
					}
					break;
				}
				else
				{
					map.introCutScene->Update();
				}
			}
			else
			{
				// TP: If there is a cutscene but it is not playing then play now
				map.introCutScene->Setup();
				map.introCutScene->Play(&camera[0]);
			}

		}
		break;

	case INTRO_UPLOADCAMERACASESOUND:
	// IH: sounds are already in ram, so play some ambients instead
#if BPLATFORM!=GAMECUBE
		LoadLevelIntroSamplesMemory();
#else
		PlayAmbientSounds(map.sceneNumber);
#endif
		gameStatus.introState = INTRO_CAMERACASES;
		break;
	
	case INTRO_CAMERACASES:
		if ((map.sceneNumber == SCENE_FRONTEND)||(map.intro.numPans==0)||(gameStatus.multiplayer.numPlayers==2)||((map.sceneNumber==SCENE_TAZHUB)&&(camera[0].fade.xDoorCRC)))
		{
			// TP: No cameracases in these circumstances
			sfxTimer = -1.0f;
			exitGameIntro=true;
		}
		else
		{
			// TP: If there is a cameracase playing and it has only a second left to play the start a 1 second fade
			if ((camera[0].cameraCase)&&(camera[0].cameraCase->time > camera[0].cameraCase->caseTime-1.0f)&&(camera[0].fade.GetFadeState()==FADESTATE_OFF))
			{
				camera[0].fade.StartFadeDown(FADETYPE_NORMAL,1.0f);
			}

			// TP: The player can choose to skip all the cameracases
			if ((controller1.startDebounceChannel->value)
#if(BPLATFORM == PC)
				|| (controller1.menuSelectDebounceChannel->value) || (controller1.crossDebounceChannel->value) || (controller1.mb1DebounceChannel->value)
#elif (BPLATFORM==XBOX)
				||(controller1.crossDebounceChannel->value)
#endif
				)
				
			{
				exitGameIntro=true;
			}
			else
			{
				if ((camera[0].fade.GetFadeState() == FADESTATE_ON)||(camera[0].cameraCase==NULL))
				{
					if (camera[0].cameraCase==NULL)
					{
						if (map.intro.currentPan >= map.intro.numPans) 
						{
							// TP: Finished playing intro cams
							exitGameIntro=true;
						}
						else
						{
							// TP: skip any cutscenes that are already completed
							for(int i=map.intro.currentPan;i<map.intro.numPans;i++)
							{
								int j = 0;
								int post = -1;

								// TP: Find which poster the camera case refers to using the mapCameraCases table
								while (mapCameraCases[map.sceneNumber][j]!=-1)
								{
									if (mapCameraCases[map.sceneNumber][j] == map.intro.currentPan)
									{
										post = j;
										break;
									}
									j++;
								}
								if ((post!=-1)&&(post < map.mapStats.numPosters))
								{
									if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].posters[post])
									{
										// TP: skip
										map.intro.currentPan++;
									}
									else
									{
										// TP: ok to play
										break;
									}
								}
								else
								{
									// TP: ok to play
									break;
								}
							}

							if (map.intro.currentPan >= map.intro.numPans) 
							{
								// TP: Finished playing intro cams
								exitGameIntro=true;
							}
							else
							{
								// TP: play next intro camera and subtitle
								CloseCurrentSubtitle();
							#ifdef CONSUMERDEMO
								Subtitle(introCamSubtitles[map.sceneNumber][mapDemoSubs[map.intro.currentPan]],map.intro.camCase[map.intro.currentPan]->caseTime-1.0f);
							#else
								Subtitle(introCamSubtitles[map.sceneNumber][map.intro.currentPan],map.intro.camCase[map.intro.currentPan]->caseTime-1.0f);
							#endif

								if (StartCameraCase(map.intro.camCase[map.intro.currentPan],&camera[0]))
								{
									camera[0].fade.StartFadeUp(FADETYPE_NORMAL,1.0f);
								}
								map.intro.currentPan++;
							}
						}
					}
				}
			}
		}

		// TP: Had enough of the camera case intros
		if (exitGameIntro)
		{
			// TP: If the fade is not already fading down then start fade now
			if ((camera[0].fade.GetFadeState() == FADESTATE_OFF)||(camera[0].fade.GetFadeState() == FADESTATE_UP))
			{
				camera[0].fade.StartFadeDown(FADETYPE_NORMAL,1.0f);
			}

			StopChannel(&characterSounds.tweety);

			// TP: if any subtitles are still displayed then kill
			CloseCurrentSubtitle();

			gameStatus.introState = INTRO_LOADSFX;
			sfxTimer = -1;
		}
		break;

	case INTRO_LOADSFX:

		if (sfxTimer == 2)
		{
			camera[0].fade.StartFadeUp(FADETYPE_NORMAL,1.0f);
			camera[0].fade.SetFadeUpFinishedCallback(PlayOpeningSoundCallback);
			
			if (gameStatus.multiplayer.numPlayers==2 && !gameStatus.multiplayer.versusCPU)
				camera[1].fade.StartFadeUp(FADETYPE_NORMAL,1.0f);
			
			SetGameState(&gameStatus,GAMESTATE_INGAME,GAMESUBSTATE_NORMAL);
			
			if (map.sceneNumber < SCENE_MUSEUMVEHICLERACE)
				gameStatus.controlSuspend = FALSE;
			
			if (map.sceneNumber!=SCENE_FRONTEND && map.sceneNumber!=SCENE_LANGUAGESELECT)
			{
				gameStatus.player1->flags &= ~(ACTORFLAG_DONTDRAW|ACTORFLAG_NOCOLLISION|ACTORFLAG_NOVELOCITY);
			}
			
			musicSystem.ChooseMusic(map.sceneNumber,MUSICTYPE_MAIN,MUSICSTATE_START);
			PlayAmbientSounds(map.sceneNumber);
			sfxTimer = 0;

			bkPrintf("\n\n Sound memory left %d\n\n",bkARAMHeapFreeSpace());

			break;
		}

		if (sfxTimer==1) sfxTimer=2;

		if (camera[0].fade.FinishedFade() && (sfxTimer == -1))
		{
			StopCameraCase(&camera[0],camera[0].cameraCase);

			while(map.intro.currentPan < map.intro.numPans)
			{
				SendCameraCaseMessages(&camera[0], map.intro.camCase[map.intro.currentPan++]);
			}

			StopAllChannels();
#if BPLATFORM!=GAMECUBE
			FreeLevelIntroSamples();
			LoadLevelSamplesMemory();			//Loads samples into sound memory
												//On PS2 clear pak from main mem
#endif
			CompleteLoad(map.sceneNumber);		// JW: Load Enemy Samples

			if (map.sceneNumber==SCENE_TAZINTROCUTSCENE)
			{
				bkGenerateEvent("scenechange","_tazhub");
			}
			if (map.sceneNumber==SCENE_STARTGAMEINTRO)
			{
				bkGenerateEvent("scenechange","_zoohub");
			}
			sfxTimer = 1;//gameStatus.gameTime;// + 2.0f;

			controller1.idleTime = 0.0f;
			controller2.idleTime = 0.0f;

#ifndef STANDALONE
#ifdef CONSUMERDEMO
			if (gameStatus.multiplayer.versusCPU && map.sceneNumber == SCENE_DEPTSTR)
				demoTimerBook->open();
#endif
#endif
		}
		break;
	}
}


/* --------------------------------------------------------------------------------
   Function : MainSplash
   Purpose : main loop for boot up splash screen
   Parameters : 
   Returns : 
   Info : This routine also loads the first scene
*/
void MainSplash(void)
{
	// prepare scene
	bdSetupView();
	bdBeginScene();

	applyFOV();
	bdSetViewClipPlanes(videoMode.nearPlane, videoMode.farPlane);
	bdSetViewport(int(videoMode.screens[0].viewport.left), int(videoMode.screens[0].viewport.top), int(videoMode.screens[0].viewport.widthAbs()), int(videoMode.screens[0].viewport.heightAbs()));

	DrawSplashScreen(&gameStatus.splashScreen);
	//draw fade
	camera[0].fade.DrawFade();

	bdSetIdentityObjectMatrix();
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	sprintf(buf, "%s", currentDate);
	bkString8to16(string16, buf);
	bdPrintFontLite(standardFont, string16, strlen(buf), (float)((-videoMode.xScreen)/2.2f),(float)((videoMode.yScreen)/2.0f), 0,0,0, 128, NULL, NULL);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);

	bdEndScene();

	// PP: finish timing the frame; calculate frames per second, frame length, etc.
	finishTimingFrame();

	// null time, anything between here and end is not included in the timer
	camera[0].fade.UpdateFade();

	if (camera[0].fade.GetFadeState()==FADESTATE_OFF)
	{
#ifdef CONSUMERDEMO
#if(BPLATFORM!=PC)	
	if(CONSUMER_DEMO_PLAYABLE && 
#ifdef ALLOW_CONTROLLERS_IN_ANY_PORTS
		(biFindPad(0) != -1))
#else // NH: ALLOW_CONTROLLERS_IN_ANY_PORTS
		(biGetPortStatus(0) == EBPORTSTATUS_CONNECTED))
#endif // NH: ALLOW_CONTROLLERS_IN_ANY_PORTS
	{
		SetScene(&gameStatus, START_SCENE);
	}
	else
	{
		bkPrintf("demoMode set to Disk Play in Demo Attract Startup");

		gameStatus.demoMode = 1;

/*		if (bmRand()&1)
		{
			gameStatus.multiplayer.numPlayers = 2;
			gameStatus.multiplayer.versusCPU = false;

			SetScene(&gameStatus, SCENE_DEPTSTRVEHICLERACE);
		}
		else*/
			SetScene(&gameStatus, SCENE_DEPTSTR);

	}
#else // NH: #if(BPLATFORM!=PC)
	StartDemoMenuFadeDownCallback(&camera[0].fade);
#endif // NH: #if(BPLATFORM!=PC)
#else
	#ifndef INFOGRAMES
			if (commandLine[0]==0)
			{
		#ifdef OBJECTVIEWONLY
					SetScene(&gameStatus,SCENE_OBJECTVIEWONLY);
		#else
					SetScene(&gameStatus, START_SCENE);
		#endif
			}
			else
			{
		#ifdef OBJECTVIEWONLY
					SetScene(&gameStatus,SCENE_OBJECTVIEWONLY);
		#else
					SetScene(&gameStatus, SCENE_TESTMAP);
		#endif
			}
	#else
		#ifdef PROVIEW
				if (commandLine[0]!=0)
				{
					SetScene(&gameStatus, SCENE_TESTMAP);
				}
				else
				{
					SetScene(&gameStatus, START_SCENE);
				}
		#else
				SetScene(&gameStatus, START_SCENE);
		#endif
	#endif
#endif
//		FreeSplashScreen(&gameStatus.splashScreen);
		//StartFadeUp(&camera[0].fade,FADETYPE_NORMAL,0.5);

		//if (gameStatus.multiplayer.numPlayers == 2)
		//	StartFadeUp(&camera[1].fade,FADETYPE_NORMAL,0.5);

#ifdef OBJECTVIEWONLY
		setFOV(OBJECT_VIEWER_FOV);
		bdSetViewClipPlanes(videoMode.nearPlane, videoMode.farPlane);
		SetGameState(&gameStatus,GAMESTATE_OBJECTVIEWER,GAMESUBSTATE_NORMAL);
		InitObjectViewer();
#else

		// PP: only go into INTRO state if we're still in SPLASH state - otherwise it'd override PAPERBOOK state, entered through InitialiseScene eg. at beginning of boss game
		if(gameStatus.currentState == GAMESTATE_SPLASH)
		{
			SetGameState(&gameStatus,GAMESTATE_INTRO,GAMESUBSTATE_NORMAL);
		}
#endif
	}

#if(BPLATFORM != PC)
#ifdef PROVIEW
	if (controller1.r3DebounceChannel->value)
	{
		// take screen shot
		TakeScreenShot();
	}
#endif
#else // NH: BPLATFORM
	if (controller1.f10DebounceChannel->value)
	{
		// take screen shot
		TakeScreenShot();
	}
#endif // NH: BPLATFORM

	// end of null time
	// PP: start timing the frame
	startTimingFrame();
}


/* --------------------------------------------------------------------------------
   Function		: DefaultPausePredrawFunc
   Purpose		: update function used for default pause
   Parameters	: context value, required but currently not used
   Returns		: 
   Info			: // PP: this is the one that gets called as a result of setting GAMESTATE_PAUSE
					// PP: Must match the PauseUpdateFunc typedef (main.h)
*/
void DefaultPausePredrawFunc(const int32 context)
{
}


/* --------------------------------------------------------------------------------
   Function : MainObjectViewer
   Purpose : main loop for objectViewer mode within pause mode
   Parameters : 
   Returns : 
   Info : 
*/
void MainObjectViewer(void)
{
	// prepare scene
	UpdateObjectViewer();
	bdSetupView();
	bdBeginScene();

	//stop drawing scene and draw objects
	DrawObjectViewer( standardFont );

	//draw fade
	camera[0].fade.DrawFade();

	bdEndScene();

	// PP: finish timing the frame; calculate frames per second, frame length, etc.
	finishTimingFrame();
	
	// null time, anything between here and end is not included in the timer
	camera[0].fade.UpdateFade();

#if(BPLATFORM != PC)
#ifdef PROVIEW
	if (controller1.r3DebounceChannel->value)
	{
		// take screen shot
		TakeScreenShot();
	}
#endif
#else // NH: BPLATFORM
	if (controller1.f10DebounceChannel->value)
	{
		// take screen shot
		TakeScreenShot();
	}
#endif // NH: BPLATFORM
	
	// end of null time
	if (controller1.f6DebounceChannel->value) SwitchBetweenScreenModes(&videoMode);

	// PP: start timing the frame
	startTimingFrame();
}


/* --------------------------------------------------------------------------------
   Function : InitGameState
   Purpose : Initialises the gameState structure
   Parameters : gameStatus
   Returns : 
   Info : 
*/
void InitGameState(GAMESTATUS *gameStatus)
{
	TBVector	zeroVector = {0.0f, 0.0f, 0.0f, 1.0f};

	gameStatus->player1 = NULL;
	gameStatus->player2 = NULL;
	// CPW: gameStatus->gameMode = GAMEMODE_SINGLEPLAYER;
	gameStatus->multiplayer.numPlayers = 1;
	// NH: Moved out of the function as the init needs to be called before I load data, but this line needs to be done after. (erk!)
	// setNumScreens(1, DEFAULT_SPLIT_SCREEN_MODE);// PP: this sets gameStatus.multiplayer.splitScreenMode
	gameStatus->pad1Present = FALSE;
	gameStatus->pad2Present = FALSE;
	gameStatus->controlSuspend = FALSE;
	gameStatus->suspendEnemies = FALSE;
	gameStatus->suspendWorld = FALSE;
	gameStatus->customOptions = 0;
	gameStatus->globalSettings.InitialiseSaveData();

	bmVectorCopy(gameStatus->windVelocity, zeroVector);					//note temporary;
	
	gameStatus->chrisCam = CHRISCAM_DEFAULTSTATE;

	// PP: Start with game speed factor 1.0f Keptin
	gameStatus->gameSpeed=1.0f;

	// PP: intialise available cheats
	gameStatus->cheatsAvailable=NO_CHEATS;

	// PP: set frame number to 0
	gameStatus->frameNumber=0;
}


/* --------------------------------------------------------------------------------
   Function : SetGameState
   Purpose : sets the current game status
   Parameters : gameStatus, EGameState, EGameSubStatem,
				// PP: (opt/0) other numerical info (eg. pointer to update func for GAMESTATE_CUSTOMPAUSE),
				// PP: (opt/0) other numerical info (eg. context value for update func for GAMESTATE_CUSTOMPAUSE)
   Returns : 
   Info : 
*/
void SetGameState(GAMESTATUS *gameStatus, EGameState state, ESubGameState subState, const int32 parm1, const int32 parm2)
{
	gameStatus->lastState = gameStatus->currentState;
	gameStatus->currentState = state;	

	// PP: make use of 'other info'...

	switch(gameStatus->currentState)
	{
	case GAMESTATE_CUSTOMPAUSE:
		{
			customPauseUpdateFunc			=(PauseUpdateFunc)parm1;
			customPauseUpdateFuncContext	=parm2;
		}
		// CMD: no break
	case GAMESTATE_PAUSE:
		if((americanEvents)&&(americanEvents->book))
		{
			americanEvents->book->close();
		}
		break;

	case GAMESTATE_PAPERBOOK:
		{
			musicSystem.RequestMood(0,STREAMMOOD_SLOW);
		}
		break;

	// PP: if otherInfo is specified but not handled, let's shout about it in the style of the robot from Lost In Space - that oudda teach 'em!
	default:
		{
			if(parm1 != 0)
			{
				bkPrintf("WARNING! WARNING! DANGER! DANGER! *************** SetGameState called with un-handled parameter 1 value of %d for state %d.\n", parm1, state);
			}

			if(parm2 != 0)
			{
				bkPrintf("WARNING! WARNING! DANGER! DANGER! *************** SetGameState called with un-handled parameter 2 value of %d for state %d.\n", parm2, state);
			}
		}
		break;
	}

	/*switch(gameStatus->lastState)
	{
	case GAMESTATE_CUSTOMPAUSE:
	case GAMESTATE_PAUSE:
		if((americanEvents)&&(americanEvents->book))
		{
			if(americanEvents->localFlags & MORRON_BOOKOPEN)
			{
				americanEvents->book->open();
			}
		}
		break;
	}*/

	// PP: TEST

	if(gameStatus->currentState == GAMESTATE_INTRO)
	{

		// PP: ********************************** TEMPORARY FRIG *************************************
		// PP: I'm using a stopwatch to smooth-out the framerate on Xbox.
		// PP: This supresses the mystery framerate jitter which I'm still no closer to understanding.
		INIT_XBOX_JITTER_SUPPRESSION_STOPWATCH;
		// PP: ***************************************************************************************
	}

	SetGameSubState(gameStatus,subState);
}


/* --------------------------------------------------------------------------------
   Function : SetGameSubState
   Purpose : sets the current game substatus
   Parameters : gameStatus, EGameState
   Returns : 
   Info : 
*/
void SetGameSubState(GAMESTATUS *gameStatus, ESubGameState subState)
{
	gameStatus->currentSubState = subState;
}


/* --------------------------------------------------------------------------------
   Function : GetCurrentGameState
   Purpose : returns the current game status
   Parameters : GAMESTATUS
   Returns : 
   Info : 
*/
int GetCurrentGameState(GAMESTATUS *gameStatus)
{
	return gameStatus->currentState;
}


/* --------------------------------------------------------------------------------
   Function : GetCurrentGameSubState
   Purpose : returns the current sub game status
   Parameters : GAMESTATUS
   Returns : 
   Info : 
*/
int GetCurrentGameSubState(GAMESTATUS *gameStatus)
{
	return gameStatus->currentSubState;
}


/* --------------------------------------------------------------------------------
   Function : ChangeScene
   Purpose : destroys the current scene and loads another
   Parameters : GAMESTATUS,scene, scene type
   Returns : 
   Info : Must be careful where this is called, keep it at the end of the main loop if pos
*/
int ChangeScene(GAMESTATUS *gameStatus, int scene)
{
	int				returnValue,i;
	EGameState		gameStateCopy;		// PP: a copy of the game state in effect as this function starts

	// TP: Stop sound before the memory card access
	StopAllChannels();

	// PP: take note of the game state in effect as this function starts
	gameStateCopy=(EGameState)GetCurrentGameState(gameStatus);

	PauseRumble(&controller1);
	PauseRumble(&controller2);

	FlushRumbleQueue(&controller1);
	FlushRumbleQueue(&controller2);

	setNumScreens(1);

	BookList_finishCloses();

	if (!gameStatus->demoMode)
		SaveGame(scene);

	if (gameStatus->quitNow)
		return false;

	// PP: Open the level-name book, to show the name of the level we're loading
	levelBook->open();

	while (GetCurrentBkgLoadingStatus()==TRUE)
	{
		DrawLoadLoop(DRAW_FADE|DRAW_MEMCARDUPDATE);
	}

	// PP: Close the level-name book now that the level has loaded
	levelBook->close(0.0f, true);// PP: 0.0f true: new test

#if BPLATFORM == PC
	mouse.active = false;
#endif

	if (scene >= ENDOFSCENES) return FALSE;
	if (scene < 0) return FALSE;

	FreeScene(gameStatus->currentScene);

	// JW: Little check so that the language select gets shown twice in a soft reset
	if ((gameStatus->currentScene == SCENE_LANGUAGESELECT) && (scene == SCENE_LANGUAGESELECT))
		skipLanguage = false;

	// TP: Put this in because some things during the free scene are changing the state from fading
	SetGameState(gameStatus, gameStateCopy, GAMESUBSTATE_NORMAL);

	if (videoMode.flagRequest!=videoMode.flags)
	{
		// TP: a change in the video mode has been requested
		bdCloseDisplay();
		videoMode.flags = videoMode.flagRequest;
#if BPLATFORM == PS2
		if (videoMode.flags&BDISPLAYFLAG_NTSC)
		{
			videoMode.yScreen = 448;
		}
		else
		{
			videoMode.yScreen = 512;
		}
#endif
		SetupDisplay(&videoMode);
		videoMode.screens[0].viewport=RECTANGLE(0, videoMode.xScreen, videoMode.yScreen, 0);
		for (i=0;i<MAX_SCREENS;i++)
		{
			camera[i].fade.InitFade();
		}
	}
	
	if(gameStatus->multiplayer.numPlayers == 1)
	{
		setFOV(videoMode.screens[0].fov);
		camera[0].fade.SetCircleFadeTo(0.0f,0.0f);
		returnValue = InitialiseScene(scene);
	}
	else
	{
		returnValue = InitialiseScene(scene);
	}

	sfxTimer = -1;

	if (returnValue!=-1)
	{
		gameStatus->currentScene = scene;
		gameStatus->currentSceneType = returnValue;
	}
	else
	{
		InitialiseScene(gameStatus->currentScene);
	}

	UpdateCamera(&camera[0]);

	if (gameStatus->multiplayer.numScreens == 2)
		UpdateCamera(&camera[1]);	

	PrepareScene(gameStatus->currentScene);

	UpdateScene(gameStatus->currentScene);

	camera[0].fade.SetSceneToLoad(FADESCENE_NONE);

	if(GetCurrentGameState(gameStatus) == gameStateCopy)
	{
		SetGameState(gameStatus,GAMESTATE_INTRO,GAMESUBSTATE_NORMAL);
	}
	// PP: otherwise, somewhere in the scene initialisation, a new game state was set (eg. a paper book was opened, putting us into GAMESTATE_PAPERBOOK)
	// PP: Let that new state override the INTRO state

	controller1.idleTime = 0.0f;
	controller2.idleTime = 0.0f;

	return TRUE;
}


/* --------------------------------------------------------------------------------
   Function : SetScene
   Purpose : Creates the first scene
   Parameters : GAMESTATUS, scene
   Returns : 
   Info : 
*/
int SetScene(GAMESTATUS *gameStatus, int scene)
{
	map.startPos = 0;
	gameStatus->currentScene = scene;

	gameStatus->currentSceneType = InitialiseScene(gameStatus->currentScene);

	UpdateCamera(&camera[0]);
	PrepareScene(gameStatus->currentScene);
	UpdateScene(gameStatus->currentScene);

	return TRUE;
}


/* --------------------------------------------------------------------------------
   Function : PrintDebugText
   Purpose : Outputs debug text to the screen
   Parameters : 
   Returns : 
   Info : 
*/
void PrintGameText(PLAYER *player)
{
	static	char	debugTextOn = FALSE;

	// PP: don't respond to this control if we're navigating in-game menus
	if(!gameStatus.navigatingMenus && debugDPad)
	{
		if ((controller1.dPadYDebounceChannel->value < 0.0f)&&(GetCurrentGameState(&gameStatus)==GAMESTATE_INGAME)
			&&(map.sceneNumber != SCENE_FRONTEND))
		{
			debugTextOn = 1-debugTextOn;

			bkHeapCheck();
			bkPrintf("Memory free %i\n",bkHeapFreeSpace(NULL));
		}
	}

	bdSetIdentityObjectMatrix();
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
#if BPLATFORM == PS2
	bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
#endif
	
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);			// PP: this should anti-alias the text in the book
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	DrawPlayerStats(player);
	UpdatePlayerStats(player);

	bdSetIdentityObjectMatrix();
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
#if BPLATFORM == PS2
	bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
#endif
	
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	if (debugTextOn)
	{
		sprintf(buf, "%f", GetAngleAroundYFromQuat(gameStatus.player1->actorInstance.orientation));
		bkString8to16(string16, buf);
		TextFX_vGrad(128,128,0,110,40,0);
		bdPrintFontLite(standardFont, string16, strlen(buf), (float)((-videoMode.xScreen+60)/2),(float)((videoMode.yScreen)/2), 128,128,128, 128, TextFX_escapeCallback, NULL);
		TextFX_resetColour();

		sprintf(buf, "(%0.1f,%0.1f,%0.1f)", gameStatus.player1->actorInstance.position[X]/250,
			gameStatus.player1->actorInstance.position[Y]/250,gameStatus.player1->actorInstance.position[Z]/250);
		bkString8to16(string16, buf);
		TextFX_vGrad(128,128,0,110,40,0);
		bdPrintFontLite(standardFont, string16, strlen(buf), -80.0f,(float)((videoMode.yScreen)/2), 128,128,128, 128, TextFX_escapeCallback, NULL);
		TextFX_resetColour();
	#if		BPLATFORM==PS2
		sprintf(buf, "%d", tcount);
		bkString8to16(string16, buf);
		TextFX_vGrad(128,128,0,110,40,0);
		bdPrintFontLite(standardFont, string16, strlen(buf), (float)((videoMode.xScreen-150)/2),(float)((videoMode.yScreen)/2), 128,128,128, 128, TextFX_escapeCallback, NULL);
		TextFX_resetColour();
	#else
		sprintf(buf, "%d(%d)", (int)fps,bdGetPolyCount());
		bkString8to16(string16, buf);
		TextFX_vGrad(128,128,0,110,40,0);
		bdPrintFontLite(standardFont, string16, strlen(buf), (float)((videoMode.xScreen-300)/2),(float)((videoMode.yScreen)/2), 128,128,128, 128, TextFX_escapeCallback, NULL);
		TextFX_resetColour();
	#endif
	}

	if(gameStatus.player1->actorStatus->currentState == STATE_DEBUGMOVE)
	{
		sprintf(buf, "fly mode");
		bkString8to16(string16, buf);
		bdPrintFontLite(standardFont, string16, strlen(buf), (float)((videoMode.xScreen-750)/2),(float)((videoMode.yScreen-20)/2), 128,128,128, 128, NULL, NULL);
	}

	if(flyCamActive)
	{
		if(!shadowLightDebug)// PP: don't print these instructions if the light editor is open
		{
			sprintf(buf, "fly cam");
			bkString8to16(string16, buf);
			bdPrintFontLite(standardFont, string16, strlen(buf), (float)((videoMode.xScreen-750)/2),(float)((videoMode.yScreen-20)/2), 128,128,128, 128, NULL, NULL);
			
			if(!CheatActive(CHEAT_TIMEFREEZE))// PP: can't drop taz in timefreeze mode
			{
				sprintf(buf, "triangle: drop Taz");
				bkString8to16(string16, buf);
				bdPrintFontLite(standardFont, string16, strlen(buf), (float)((videoMode.xScreen-750)/2),(float)((videoMode.yScreen-100)/2), 128,128,128, 128, NULL, NULL);
			}
		}
	}
	
#if BPLATFORM == PS2
// PP: REMOUT: I really don't like this render state	bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,FALSE,FALSE);
#endif

}


/* --------------------------------------------------------------------------------
   Function		: ApplySpeedAdjustments
   Purpose		: Effect any game-speed alterations, such as the TURBO and SLOWMO
					cheats and the global game-speed factor which can be used for
					special effects
   Parameters	: 
   Returns		: 
   Info			: 
*/
void ApplyGameSpeedAdjustments(void)
{
	if(TimeFX_jumpTime == 0.0f)
	{
		gameStatus.deltaTime = TBTimerValue(gameStatus.deltaTime*gameStatus.gameSpeed);
	}
	else
	{
		// PP: use jump time
		gameStatus.deltaTime= TBTimerValue((gameStatus.deltaTime*gameStatus.gameSpeed)+(TimeFX_jumpTime*gameStatus.gameSpeed*bkTimerGetFrequency()));

		// PP: clear jump time
		TimeFX_jumpTime=0.0f;
	}

	if(CheatActive(CHEAT_TURBO))
	{
		gameStatus.deltaTime = TBTimerValue(gameStatus.deltaTime*CHEATS_TURBO_SPEED_FACTOR);
	}
	else if(CheatActive(CHEAT_SLOWMO))
	{
		gameStatus.deltaTime = TBTimerValue(gameStatus.deltaTime*CHEATS_SLOWMO_SPEED_FACTOR);
	}
	else if(CheatActive(CHEAT_SUPERSLOWMO))
	{
		gameStatus.deltaTime = TBTimerValue(gameStatus.deltaTime*CHEATS_SUPERSLOWMO_SPEED_FACTOR);
	}
	/*else if(CheatActive(CHEAT_SUPERSUPERSLOWMO))
	{
		gameStatus.deltaTime = TBTimerValue(gameStatus.deltaTime*CHEATS_SUPERSUPERSLOWMO_SPEED_FACTOR);
	}*/
}


/* --------------------------------------------------------------------------------
   Function : TakeScreenShot
   Purpose : Outputs screen shots to screen[number].bmp in exe directory
   Parameters : 
   Returns : 
   Info : 
*/

void TakeScreenShot(void)
{
	char	mystring[256];

// NH: Changed this from ndef from INFOGRAMES to _MASTER, as Infogrames wanted to take screenshots
#ifndef _MASTER
#if BPLATFORM == PC || BPLATFORM == XBOX || BPLATFORM == PS2
	bdScreenshot(bkGenerateUniqueFilename(mystring,"Screenshot",".bmp"));
#endif // NH: #if BPLATFORM == PC || BPLATFORM == XBOX
#endif // NH: #ifndef _MASTER
}


/*  --------------------------------------------------------------------------------
	Function	: BDECLAREMAIN
	Purpose		: Game entry point
	Parameters	: 
	Returns		: 
	Info		: 
*/

BDECLAREMAIN
{
#ifdef CONSUMERDEMO
	#if (BPLATFORM == PS2)
		#ifndef STANDALONE	// JW: Don't do sony's demo launcher if we're stand alone
			#ifndef USE_HOST0
			{
				bkSetModulePath("\\TAZDEMO\\MODULES\\");
			
				scePrintf("Enter Main");

 				sceeDemoStart(argc,argv,&language,&aspect,&playmode,&to_inactive,&to_gameplay);

				if (language < SCEE_DEMO_LANGUAGE_ENGLISH || language > SCEE_DEMO_LANGUAGE_ITALIAN)
					language = SCEE_DEMO_LANGUAGE_ENGLISH;

				sceSifInitRpc(0);
			  

				sceCdInit(SCECdINIT);
			
				switch(sceCdGetDiskType())
				{
				case SCECdPS2DVD:	sceCdMmode(SCECdDVD);
									bkSetMediaType(BMEDIA_DVD);
									break;
				
				case SCECdPS2CD:	sceCdMmode(SCECdCD);
									bkSetMediaType(BMEDIA_CD);
									break;
				
				default:			printf("Unknown disc type");
									break;
				}

			  
				while (!sceSifRebootIop(FILENAME_IOPIMG));
				while (!sceSifSyncIop());
					  
				sceSifInitRpc(0);
			  
		
				sceCdInit(SCECdINIT);
				
				switch(sceCdGetDiskType())
				{
				case SCECdPS2DVD:	sceCdMmode(SCECdDVD);
									bkSetMediaType(BMEDIA_DVD);
									break;
				
				case SCECdPS2CD:	sceCdMmode(SCECdCD);
									bkSetMediaType(BMEDIA_CD);
									break;
					
				default:			printf("Unknown disc type");
									break;
				}
					  
				sceFsReset();
						  
				if (sceSifLoadModule(FILENAME_SIO2MAN,0,NULL) < 0)
				{
					printf("Fatal error: Can't load module " FILENAME_SIO2MAN "\n");
					//goto end;
				}
					  
				if (sceSifLoadModule(FILENAME_PADMAN,0,NULL) < 0)
				{
					printf("Fatal error: Can't load module " FILENAME_PADMAN "\n");
					//goto end;
				}
					  
				if (sceSifLoadModule(FILENAME_LIBSD,0,NULL) < 0)
				{
					printf("Fatal error: Can't load module " FILENAME_LIBSD "\n");
					//goto end;
				}
					  
				if (sceSifLoadModule(FILENAME_SDRDRV,0,NULL) < 0)
				{
					printf("Fatal error: Can't load module " FILENAME_SDRDRV "\n");
					//goto end;
				}
					  
				scePadInit(0);
					  
				scePadPortOpen(0,0,pad_dma_buf);
			}
			#endif
		#endif
	#else
		#if BPLATFORM == PS2
			bkSetMediaType(BMEDIA_CD);
		#endif
	#endif
#else

	#if (BPLATFORM == PS2)
		// TP: Note this should be changed to BMEDIA_DVD or CD for final build
		bkSetMediaType(BMEDIA_AUTOSELECT);
	#endif

#endif
			
#ifdef STANDALONE
	do	{
#endif
		bkRun(Main, NULL);
#ifdef STANDALONE
	}	while(true);	// JW: For the stand alone, make sure it loops around ad infinitum
#endif

#if(BPLATFORM == XBOX)// PP: 'main' is a void function on Xbox
	return;
#elif (BPLATFORM == PS2)// PP: platforms other than Xbox
	#ifdef CONSUMERDEMO
		end:
		
		#ifndef STANDALONE
			sceeDemoEnd();
		#endif
	#endif

    return 0;
#else
	return 0;
#endif// PP: (BPLATFORM == XBOX) else
}


/*  --------------------------------------------------------------------------------
	Function	: ProcessCommandLine
	Purpose		: Read in the command line parameters
	Parameters	: 
	Returns		: 
	Info		: 
*/

void ProcessCommandLine(void)
{
    int argc;
    char **argv;

#if BPLATFORM == PS2// || BPLATFORM == PC
#ifdef	ENABLEVIDEO
	doCorporateStuff();
#endif
#endif

#ifdef INFOGRAMES

	commandLine[0] = 0;

#else

	bkGetCommandLine(&argc, &argv);
	commandLine[0] = 0;

	for(int loop=0; loop<argc; loop++)
		bkPrintf("Arg#%d: '%s'\n", loop,argv[loop]);

	if (argc==0) bkPrintf("No args found.\n");

#if		BPLATFORM == PS2
	#ifndef PROVIEW
		// TP: if ps2 and no proview then clear args
		argc = 0;
	#endif
#endif

	if (argc > 0)
	{
#ifdef PROVIEW
		strcpy(commandLine,argv[1]);
#else
		strcpy(commandLine,argv[0]);
#endif
		strlwr(commandLine);
	}
#endif
}

// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ TEXT ROUTINES - should these be here? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/*	--------------------------------------------------------------------------------
	Function 	: strcatf
	Purpose 	: sprintf onto the end of a string
	Parameters 	: destination buffer, format string, argument list
	Returns 	: completed string
	Info 		: 
*/
char* strcatf(char * const buffer, const char * const format, ...)
{
	va_list			argp;

	va_start(argp, format);
	vsprintf(&buffer[strlen(buffer)], format, argp);
	va_end(argp);

	return buffer;
}

// PP: TEMP DEBUG TEST - will remove
int32	videoReturnCodes[8]= {	-100,
								-100,
								-100,
								-100,

								-100,
								-100,
								-100,
								-100};

void doCorporateStuff()
{
	int	vidNo;

	uint32	flags = 0;

	if (mainStringTable)
	{
		// CMD: hide everything
		HideActorInstanceList(&map.animatedInstances);
		HideActorInstanceList(&map.levelInstances);
		HideActorInstanceList(&map.characterInstances);
		HideActorInstanceList(&characterInstances);
	}

	if (videoMode.flags & BDISPLAYFLAG_PAL)
		flags |= BVIDEOPLAY_PLAYPALSTREAM;

#if(BPLATFORM != XBOX)// PP: platform
	if (!videoSkipEnable)
		flags |= BVIDEOPLAY_NOAUTOQUIT;
#endif// PP: !xbox
	
#if(BPLATFORM != PC)

	// NH: Play the corporate videos.
	for (vidNo=0;vidNo<numVideos;vidNo++)
	{
		videoReturnCodes[vidNo]=bdVideoPlayStream(videos[vidNo],flags);/* PP: REMOUT: problematic; JC says sod it	|BVIDEOPLAY_QUITONPADEVENT*/	
	}

#else // NH: #if(BPLATFORM != PC)
	{
		// NH: For all corporate videos...
		for (vidNo=0;vidNo<numVideos;vidNo++)
		{
			// NH: Play the bink video
			if (!binkPlayVideo(videos[vidNo], bDisplayInfo.backBuffer))
				bkPrintf("*** ERROR *** There was an error in displaying the \"%s\" Bink video\n", videos[vidNo]);
		}
	}
#endif // NH: #if(BPLATFORM != PC)

	if (mainStringTable && videoMode.flags & BDISPLAYFLAG_NTSC)
	{
		FORCE_OKBUTTON_DEBOUNCE;

#ifdef STANDALONE
		LoadSplashScreen(&gameStatus.splashScreen,"legal.bmp","splshscr");
#else
		LoadSplashScreen(&gameStatus.splashScreen,"legal2.bmp","splshscr");
#endif

		TBTimerValue	startTime, finishTime;
				
		float	elapsedTime = 0.0f;

		startTime = bkTimerRead();

		do
		{
			// prepare scene
			bdSetupView();
			bdBeginScene();
			
			applyFOV();
			bdSetViewClipPlanes(videoMode.nearPlane, videoMode.farPlane);
			bdSetViewport(int(videoMode.screens[0].viewport.left), int(videoMode.screens[0].viewport.top), int(videoMode.screens[0].viewport.widthAbs()), int(videoMode.screens[0].viewport.heightAbs()));

			DrawSplashScreen(&gameStatus.splashScreen);

			//DrawFade(&camera[0]);
			
			bdEndScene();

			finishTime = bkTimerRead();

			fps = bkTimerToFPS(bkTimerDelta(startTime,finishTime));
			
			fTime = 1.0f / fps;

			speedRatio = 60/fps;
			
			elapsedTime += fTime;
		
			//UpdateFade(&camera[0]);

			biReadDevices();

			if (OKBUTTON && videoSkipEnable)
				break;

			startTime = bkTimerRead();

			Flip(0, 0, 0, 128);
		}
		while (elapsedTime < 5.0f);
	}

#ifdef CONSUMERDEMO
	keepOnLooping = 1;
#endif

	controller1.idleTime = 0.0f;
	controller2.idleTime = 0.0f;

	videoSkipEnable = true;
}


#else


/*  --------------------------------------------------------------------------------
	Function	: rollMain
	Purpose		: Main function for playing the rolling demo
	Parameters	: NULL
	Returns		: 
	Info		: 
*/

void rollMain(void *context)
{
	int initState;

	initState = bkInit(NULL, (0x1fa0000 - (uint)(&_end))-(1*1024), BKINITFLAG_NONEXCLUSIVE);

	if (!initState)
	{
		bkAlert("Error during Babel initialisation");
		return;
	}

	bdVideoPlayStream(FILENAME_ROLLINGVID, BVIDEOPLAY_PLAYPALSTREAM);

	// PP: babel is about to shut down; don't try to free anything after this point
	BabelHasShutdown=true;

	bkShutdown();

	return;
}


/*  --------------------------------------------------------------------------------
	Function	: BDECLAREMAIN
	Purpose		: game entry point
	Parameters	: 
	Returns		: 
	Info		: 
*/

BDECLAREMAIN
{
#ifdef CONSUMERDEMO
	#if (BPLATFORM == PS2)
		#ifndef STANDALONE	// JW: Don't do sony's demo launcher if we're stand alone
			#ifndef USE_HOST0
			{
				bkSetModulePath("\\TAZDEMO\\MODULES\\");
			
				scePrintf("Enter Main");

 				sceeDemoStart(argc,argv,&language,&aspect,&playmode,&to_inactive,&to_gameplay);

				if (language < SCEE_DEMO_LANGUAGE_ENGLISH || language > SCEE_DEMO_LANGUAGE_ITALIAN)
					language = SCEE_DEMO_LANGUAGE_ENGLISH;

				sceSifInitRpc(0);
			  

				sceCdInit(SCECdINIT);
			
				switch(sceCdGetDiskType())
				{
				case SCECdPS2DVD:	sceCdMmode(SCECdDVD);
									bkSetMediaType(BMEDIA_DVD);
									break;
				
				case SCECdPS2CD:	sceCdMmode(SCECdCD);
									bkSetMediaType(BMEDIA_CD);
									break;
				
				default:			printf("Unknown disc type");
									break;
				}

			  
				while (!sceSifRebootIop(FILENAME_IOPIMG));
				while (!sceSifSyncIop());
					  
				sceSifInitRpc(0);
			  
		
				sceCdInit(SCECdINIT);
				
				switch(sceCdGetDiskType())
				{
				case SCECdPS2DVD:	sceCdMmode(SCECdDVD);
									bkSetMediaType(BMEDIA_DVD);
									break;
				
				case SCECdPS2CD:	sceCdMmode(SCECdCD);
									bkSetMediaType(BMEDIA_CD);
									break;
					
				default:			printf("Unknown disc type");
									break;
				}
					  
				sceFsReset();
						  
				if (sceSifLoadModule(FILENAME_SIO2MAN,0,NULL) < 0)
				{
					printf("Fatal error: Can't load module " FILENAME_SIO2MAN "\n");
					//goto end;
				}
					  
				if (sceSifLoadModule(FILENAME_PADMAN,0,NULL) < 0)
				{
					printf("Fatal error: Can't load module " FILENAME_PADMAN "\n");
					//goto end;
				}
					  
				if (sceSifLoadModule(FILENAME_LIBSD,0,NULL) < 0)
				{
					printf("Fatal error: Can't load module " FILENAME_LIBSD "\n");
					//goto end;
				}
					  
				if (sceSifLoadModule(FILENAME_SDRDRV,0,NULL) < 0)
				{
					printf("Fatal error: Can't load module " FILENAME_SDRDRV "\n");
					//goto end;
				}
					  
				scePadInit(0);
					  
				scePadPortOpen(0,0,pad_dma_buf);
			}
			#endif
		#endif
	#else
		#if BPLATFORM == PS2
			bkSetMediaType(BMEDIA_CD);
		#endif
	#endif
#else
	#if BPLATFORM == PS2
		bkSetMediaType(BMEDIA_CD);
	#endif
#endif
			
	bkRun(rollMain, NULL);

#if(BPLATFORM == XBOX)// PP: 'main' is a void function on Xbox
	return;
#elif (BPLATFORM == PS2)// PP: platforms other than Xbox
	#ifdef CONSUMERDEMO
		end:
		sceeDemoEnd();
	#endif

    return 0;
#else
	return 0;
#endif// PP: (BPLATFORM == XBOX) else
}

#endif