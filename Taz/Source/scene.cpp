// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : scene.cpp
//   Purpose : scene initialise, setup and draw functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"

#include "actors.h"
#include "physics.h"
#include "animation.h"
#include "collisioncache.h"
#include "mapfile.h"
#include "status.h"
#include "paths.h"
#include "genericai.h"
#include "camera.h"
#include "control.h"
#include "maths.h"
#include "characters.h"
#include "shadow.h"
#include "scene.h"
#include "trigger.h"
#include "destructibles.h"
#include "water.h"
#include "projectile.h"
#include "springyobjects.h"
#include "mailbox.h"
#include "wantedposter.h"
#include "fallingobjects.h"
#include "main.h"
#include "lights.h"
#include "cutscene.h"
#include "envmap.h"
#include "sfx.h"
#include "textures.h"
#include "swingingobjects.h"
#include "playerstats.h"
#include "tonyscene.h"
#include "claudescene.h"
#include "chrisscene.h"
#include "philscene.h"
#include "precipitation.h"
#include "thoughtbubble.h"
#include "collectibles.h"
#include "debris.h"
#include "tiltingobjects.h"
#include "steppingstone.h"
#include "gui.h"
#include "effects.h"
#include "cameracase.h"
#include "waterrings.h"
#include "clouds.h"
#include "precipitation.h"
#include "timer.h"
#include "fire.h"
#include "special.h"
#include "watersplash.h"
#include "snowball.h"
#include "files.h"
#include "waterfall.h"
#include "attach.h"
#include "polyeffects.h"
#include "trail.h"
#include "floorpolisherwantedposter.h"
#include "tweety.h"
#include "music.h"
#include "decals.h"
#include "player.h"
#include "Boss.h"							// PP: General boss game stuff
#include "Shockwave.h"						// PP: Shockwave effect
#include "subtitle.h"
#include "endoflevelstats.h"
#include "Flies.h"							// PP: Midges and the like
#include "costumes.h"						// PP: Taz costume code
#include "MonitorFX.h"						// PP: Computer monitor effects
#include "securitybox.h"
#include "quakecam.h"						// PP: camera-shake stuff
#include "vehicles.h"						// PP: vehicle stuff
#include "frontendmain.h"
#include "display.h"						// PP: display stuff
#include "VideoFX.h"						// PP: Video effects - motion blur, defocus, whatever.  Top-of-the-Pops-TASTIC, Mate!
#include "explode.h"						// PP: explosions
#include "rumble.h"
#include "cheesegun.h"
#include "display.h"						// PP: display stuff
#include "WestBoss.h"						// PP: Wile E West Boss - Yosemite Sam's ammo dump
#include "Fireball.h"						// PP: Particle emitters used for explosion effects, eg. by WestBossExp
#include "apes.h"
#include "smashcamera.h"
#include "pause.h"
#include "memcard.h"
#include "halo.h"
#include "buzzard.h"
#include "buzzardscripts.h"
#include "doors.h"
#include "multiplayer.h"
#include "phonebox.h"
#include "music.h"
#include "demo.h"
#include "thwap.h"
#include "glowaroundplinth.h"
#include "atlassphere.h"
#include "PageItemFX.h"						// PP: Special effects for page items
#include "prompt.h"
#include "XboxStrings.h"					// PP: Xbox versions of some strings
#include "costumes.h"
#include "endgamecredits.h"
#include "PongDust.h"						// PP: Cartoon dust effect, for Elephant Pong

// PP: duration for which demo "time up" message stays on screen
#define DEMO_TIMEUP_DURATION	2.5f

#define MAX_SCENE_EVENTS		(50)

//#define LEVELCHANGETEST

ACTORINSTANCELIST		characterInstances;
ACTORINSTANCELIST		levelInstances;
ACTORINSTANCELIST		animatedInstances;
ACTORINSTANCELIST		tempInstances;
ACTORINSTANCELIST		cameraInstances;   
ACTORINSTANCELIST		invisibleInstances;
ACTORINSTANCELIST		rockInstances;
ACTORINSTANCELIST		statsInstances;
COLLISIONCACHE			collisionCache;

ACTORINSTANCE			*tempInstance;

//CUTSCENE				cutScene;
MAPINFO					map;
ENVMAP					environmentMap;

#define HOLD_DEMO_OBJECTIVES_DURATION		30.0f
static float			demoObjectivesTimer=0.0f;

#ifndef STANDALONE
#ifdef CONSUMERDEMO
	BOOK				*demoTimerBook = 0;
	float				demoGameTimer;
	TEXTBOX				*txtTimer, *txtTimeUp;
#endif
#endif

int	PredictAmountToLoad(int sceneNumber);

char *sceneText[] = 
{
	"Language Select",
	"Front End",
	"Enemy - Playpen",
	
	"Zoo - Hub",
	"Zoo - Ice Kingdome",
	"Zoo - Safari",
	"Zoo - Aqua",
	"Zoo - Boss",

	"City - Hub",
	"City - Department Store",
	"City - Museum",
	"City - Construction Site",
	"City - Boss",

	"West - Hub",
	"West - Ghost Town",
	"West - Gold Mine",
	"West - Grand Canyon",
	"West - Boss",

	"Taz - Tazmania",
	"Taz - Boss1",
	"Taz - Boss2",

	"Vehicle Race - Museum",
	"Vehicle Race - Grand C",
	"Vehicle Race - Dept Store",

	"Destruction - Aqua",
	"Destruction - Construct",
	"Destruction - Tasmania",

	"Race - Ice Dome",
	"Race - Gold Mine",
	"Race - Safari",

	"Map",
	"Test Map",
	"Null Scene",
	"Object Viewer Only",
	"Taz Intro Cutscene",
	"Start game intro",

	"Department Store Demo Level",
};

char *sceneFileName[] = 
{
	"language",
	"frontend",
	"playpen",

	"zoohub",
	"icedome",
	"safari",
	"aqua",
	"zooboss",

	"cityhub",
	"deptstr",
	"museum",
	"contruct",
	"cityboss",

	"westhub",
	"ghost",
	"goldmine",
	"grandc",
	"westboss",

	"tazhub",
	"tazboss1",
	"tazboss2",

	"vrmuseum",
	"vrgrandc",
	"vrdeptstr",

	"deaqua",
	"deconstruct",
	"detasmania",

	"rcicedome",
	"rcgoldmine",
	"rcsafari",

	"null",
	"null",
	"null",
	"null",
	"tasmania_intro",
	"zoohub_startgame",

	"demodept",
};

TBLightsource *dirLight1,*dirLight2,*dirLight3,*ambient;

DemoModeData	*dmData;
float			*fpsStore;
TBVector		*pos1Store;
TBVector		*pos2Store;

BOOK			*demoBook = 0;

float		snowballClock;

#ifdef _TAZDEBUG
	//#ifndef INFOGRAMES// PP: don't want this on any disk
		#define ENABLESNAPSHOT
	//#endif// PP: ndef INFOGRAMES
#endif

#ifdef ENABLESNAPSHOT
	static TBHeapSnapshot		*snap = NULL;
	static TBResourceSnapshot		*ressnap = NULL;
#endif

/*	--------------------------------------------------------------------------------
	Function 	: InitialiseScene
	Purpose 	: Initialise a scene loading actors creating instances and setting up camera
	Parameters 	: scene number, camera to setup
	Returns 	: scene type - cutscene or ingame, -1 fail
	Info 		:
*/

// PP: scale of env-map box
// PP: I upped this so that it writes to the back of the zbuffer and thus defocuses properly
#define ENVMAPSCALE		METERTOUNIT(150)// PP: METERTOUNIT(20)

int InitialiseScene(int scene)
{
	TBPackageIndex		*pakIndex;
	int32				lastHandle;
	uint32				crc;
	TBActor				*actor;
	int					i,tempInt;
	bool				timedOutToDemoFrontEnd=false;// PP: TEMP TEST

	bkPrintf("InitialiseScene(%d,)\n",scene);

	// PP: cancel any left-over fov fading
	SetFadeFieldOfView();

	// PP: Hmmm.  See you might have gui'd out of a paperbook or that, in which case the fov'll be wrong.
	// PP: restore normal fov
	setFOV(NORMAL_INGAME_FOV);

	// PP: for the benefit of monitor effect initialisation
	gameStatus.gameTime = 0;

	// TP: Reset any callback functions
	ResetSoundCallbacks();

	UnPauseRumble(&controller1);
	UnPauseRumble(&controller2);

	// CMD: need to do this before map is loaded
	InitialiseGlowAroundPlinth();

	InitialisePromptBookInfo();

#ifdef TONY
	//bkHeapShow(BALLGROUPS, 10);
#endif

	switch(scene)
	{
	default:
		if (scene==SCENE_LANGUAGESELECT)
		{
			// TP: if loading the language select then offset loading bar by restex and resobjs to follow on from initial load
			InitialiseLoadingBar(PackageSize(masterPakfile,"restex")+PackageSize(masterPakfile,"resobjs")+PackageSize(masterPakfile,"glyphs"));
		}
		else
		{
			if (scene!=SCENE_NOSCENE)
			{
				InitialiseLoadingBar();
			}
		}

		if (scene != map.sceneNumber)
			map.lastScene = map.sceneNumber;

		switch(scene)
		{
		case SCENE_FRONTEND:	
			map.sceneNumber = scene;
			gameStatus.multiplayer.numPlayers = 1;
			strcpy(map.mapName,sceneFileName[scene]);
			setNumScreens(1);
			break;

		case SCENE_TESTMAP:
			// TP: Must extract level name from command line and find scene number
			if (commandLine[0]==0)	strcpy(map.mapName,"frontend");
			else strcpy(map.mapName,commandLine);

			for (i=0;i<ENDOFSCENES;i++)
			{
				if (strcmpi(map.mapName,sceneFileName[i]) == 0)
				{
					map.sceneNumber = i;
				}
			}
			break;
		case SCENE_MAP:
			// TP: Map change must find scene number
			for (i=0;i<ENDOFSCENES;i++)
			{
				if (strcmpi(map.mapName,sceneFileName[i]) == 0)
				{
					map.sceneNumber = i;
				}
			}
			break;
		case SCENE_NOSCENE:
			return (-1);
			break;
		default:
			// TP: Level chosen from gui
			strcpy(map.mapName,sceneFileName[scene]);
			map.sceneNumber = scene;
			break;
		}

		// TP: get rid of any existing splashes
		//FreeSplashScreen(&gameStatus.splashScreen);

#ifdef ENABLESNAPSHOT
		snap = bkTakeHeapSnapshot(BALLGROUPS);
#endif

		// JW: Draw Demo Book and set save slot to empty

		if (gameStatus.demoMode)
		{
			bkPrintf("Trying to create demoBook");

			demoBook = new BOOK;

			PAGE	*pagePtr = demoBook->addPage("DemoPage");

			TEXTBOX *textBox;

			ICON	*iconPtr;

			if (gameStatus.demoMode == -1)
				pagePtr->insertItem(textBox = new TEXTBOX(STR_RECORD_DEMO))->setEffect(&PIE_flash, PIEMODE_ON);
			else
				pagePtr->insertItem(textBox = new TEXTBOX(STR_PLAY_DEMO))->setEffect(&PIE_flash, PIEMODE_ON);

			textBox->setYAlign(PIFLAG_YALIGN_TOP);

#if(BPLATFORM != XBOX)// PP: Infogrames QA have asked for the "press start" message to be taken out on Xbox, since you should be able to interrupt the attract by pressing any button

#if(BPLATFORM==PC)
			pagePtr->insertItem(textBox = new TEXTBOX(STR_PRESS_JUMP_TO_CONTINUE))->setEffect(&PIE_flash, PIEMODE_ON);
#else
			pagePtr->insertItem(textBox = new TEXTBOX(STR_PRESS_START_TO_CONTINUE))->setEffect(&PIE_flash, PIEMODE_ON);
#endif // NH: #if(BPLATFORM==PC)
			textBox->setYAlign(PIFLAG_YALIGN_BOTTOM);

#endif// PP: xbox

			pagePtr->insertFreeItem(iconPtr = new ICON("tazbiglogo.bmp"), RECTANGLE(-0.55f, 0.0f, 0.0f, 0.55f))->setEffect(&PIE_flash, PIEMODE_ON);

			iconPtr->tilt;
			iconPtr->setScale(1.6f);
			iconPtr->setXAlign(PIFLAG_XALIGN_LEFT);
			iconPtr->setYAlign(PIFLAG_YALIGN_TOP);


// PP: REMOUT: probably not needed now (PAGES now align their contents when they are opened for the first time)					demoBook->realign();
			
			//demoBook->open();

			InitialiseSaveGameSlot(3, DIFF_MEDIUM);
			gameStatus.globalSettings.currentSaveSlot = 3;
		}

		LoadCorrectSplashScreen(&gameStatus.splashScreen,(int)scene);

		// TP: Start the loading music for the scene
		musicSystem.ChooseMusic(scene,MUSICTYPE_LOAD,MUSICSTATE_START);

#ifndef CONSUMERDEMO
		if(map.sceneNumber == SCENE_WESTBOSS)
		{
			// PP: switch into to 32 bit display before anything's loaded
			WestBoss_game.preInit();
		}
#endif

		if ((scene!=SCENE_STARTGAMEINTRO)&&(scene!=SCENE_TAZINTROCUTSCENE))
		{
			LoadWholePackageBkg(map.mapName);
		}

		// TP: starts to load the appropriate cutscene for level if needed
		LoadLevelCutScene(map.sceneNumber);

		StartLoading(map.sceneNumber);

#if BPLATFORM!=GAMECUBE
		// TP: Keep the samples as the last package loaded so that they do not fragment memory when freed
		LoadLevelSamplesBackground();				//Load samples for memory during bar load
#else
		LoadAllLevelSamplesBackground();
#endif

		// PP: set up and open the books to be displayed during loading
		openLoadingBooks();

#if BPLATFORM!=PS2
		tempInt = BytesOnLoadingBar();
		AddToLoadingBar(PackageSize(masterPakfile,"taz"));
#endif

		while (GetCurrentBkgLoadingStatus()==TRUE)
		{
			DrawLoadLoop();
		}

		map.mapPackage = FindPackage(map.mapName);

#if BPLATFORM!=PS2
		InitialiseLoadingBar(tempInt);
#endif

#if BPLATFORM==GAMECUBE
		MoveAllLevelSamplesToARAM();
#endif

		// PP: close the books that were displayed during loading
		// TP: leave this until later, it stops that little flick with the text
		// TP: closeLoadingBooks();
		
		// JW: Load Tweety Intro Cam Samples

#ifndef CONSUMERDEMO
	#if(BPLATFORM != XBOX && BPLATFORM != PC)
		// TP: Temp fix to save memory, the artists have eaten all the pie!
		if (gameStatus.splashScreen.resource)
		{
			FreeSplashScreen(&gameStatus.splashScreen);
		}
	#endif// PP: (BPLATFORM != XBOX)
#endif


		bkHeapStats();

		// PP: initialise video effects for the scene
		VFX_initScene();

		// PP: initialise stuff for monitor effects
		MonitorFX_init();

		// PP: why the hell was this taken out?
		//InitFloorPolishers();

		LoadGenericObjects(map.sceneNumber);

		// create 3 meg collision cache, doesn't really matter anymore as the collision cache will take 75percent of available mem on level start
		CreateCollisionCache(&collisionCache,3*1024*1024);
		
		// init actor lists
		InitActorInstanceList(&characterInstances);
	
		// init pathlist
		InitialisePathList();

		// initialise camera case list
		InitCameraCaseList();

		initThwap();
		InitFallingObjectList();
		InitBountyList();
		InitPrecipitationList();
		InitialiseWaterRingList();
		InitialiseCloudBank();
		InitEffectsList();
		InitPolyEffectsList(&polyEffectsList);

#ifndef CONSUMERDEMO
		InitFloorPolisherWantedPoster(&floorPolisherWantedPosterInfo);
#endif

		InitBarrierSystem();
		InitialisePlayerStatsAtStartOfLevel();

#ifndef CONSUMERDEMO		
		InitialiseFrontEndCameraStore();
#endif

//		numPickupsRegen = 0;	// JW: Reset number of pickups to regenerate
		
		controller1.idleTime = 0.0f;	// JW: Set idle time on controllers to zero, now she-dev won't fall asleep as soon as we load up a two-player game
		controller2.idleTime = 0.0f;
		
		// init character sounds etc.
		SFX_sceneInit();

		// Setup light for cartoon mode
//		InitCartoonLight("textures\\ramps.bmp");	// uses a texture from the taz.pak as a lighting ramp


		// create event managers
		CreateEventManagers();
		CreateEventHandlers();

		CreatePhoneBoxBook();	// JW: Create "can't enter police box" book
	
		// load map
		map.collisionCache = &collisionCache;

		if (map.mapPackage)
		{
			bkPrintf("Map loaded ok\n");
			InitialiseMap(&map);
		}
		else
		{
			bkPrintf("*** ERROR *** Could not load map package\n");
#ifdef ENABLESNAPSHOT
			if (bkCompareHeapSnapshot(snap))
			{
				//DrawMemLeakWarning();
			}
			bkDeleteHeapSnapshot(snap);
#endif
			return -1;
		}

		LoadWholePackageBkg("taz");

		// PP: set up and open the books to be displayed during loading
		// TP: already open
		// TP: openLoadingBooks();

		while (GetCurrentBkgLoadingStatus()==TRUE)
		{
#if(BPLATFORM != XBOX)
			// TP: cant flip screen 'cause I don't have splash screen in memory anymore and the screen will go black
			DrawLoadLoop(false);
#else
			DrawLoadLoop();
#endif
		}

		// PP: close the books that were displayed during loading
		closeLoadingBooks();

		LoadActor("tazdevil.obe","taz");
		LoadActor("spindevil.obe","taz");
		// Setup light for cartoon mode
		InitCartoonLight("textures\\ramps.bmp");	// uses a texture from the taz.pak as a lighting ramp

		// Setup environment map
		if (SetupEnvMapSpecial(&environmentMap,FARCLIP/2.0f,map.background1,map.background2,map.background3,map.backgroundAngle,map.backgroundYOffset)==FALSE)
		{
			bkPrintf("*** WARNING *** No background specified or textures %s, %s and %s not present\n",map.background1,map.background2,map.background3);
			SetupEnvMapSpecial(&environmentMap,FARCLIP/2.0f,"main.bmp","second.bmp",NULL,map.backgroundAngle,map.backgroundYOffset);
		}

		//initialise taztops
		for (i=0;i<MAX_PLAYERS;i++) InitialisePlayerStatsAtStartOfLevel(gameStatus.player[i].stats);

		// TP: Create the actors, cameras and sets up the players
		CreatePlayers(scene);

		// PP: Setup costumes even if none are being worn - this sets up any costume accessories specified by cheats, eg. the DIGITALWATCH cheat
		for(i=0;i<MAX_PLAYERS;i++)
		{
			if(gameStatus.player[i].actorInstance != NULL)
			{
				SetupCostume(gameStatus.player[i].actorInstance, gameStatus.player[i].actorInstance->characterInfo->costume);
			}
		}

		// PP: if we have the costume cheat on...
		if(CheatActive(CHEAT_COSTUME))
		{
			// PP: ...put on this level's costume now
			SetupLevelCostume(gameStatus.player1);
		}

		// TP: do any special initialisations
		InitialiseSpecial(map.sceneNumber);

		// TP: do any hub setup needed
		SetupHub(map.sceneNumber);

		bkHeapStats();

		// TP: Find intro camera sequences
		char	camName[12];
		map.intro.currentPan = 0;
		for (i=0;i<10;i++)
		{
			sprintf(camName,"introcam%d",i+1);
			if (!(map.intro.camCase[i] = FindCameraCase(camName)))
			{
				map.intro.numPans = i;
				break;
			}
			else
			{
				// TP: Override the intro camera cases
				CameraCaseAttribs(map.intro.camCase[i],SUSPENDFLAG_TAZ,CAMERACASEFLAG_SMOOTH);
			}
		}

		// TP: Use only 75 percent of available memory left
		collisionCache.maxMemoryAvailable = (int)((float)(bkHeapFreeSpace(NULL) + collisionCache.memoryUsed)*0.75f);

#ifdef CONSUMERDEMO
		gameStatus.playTime = 0;
#else
		gameStatus.playTime = gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[scene].time;
#endif
		gameStatus.multiplayer.playerPause = 0;

#ifdef TONY
		InitialiseTonyScene();
#endif

		// PP: initialise Phil-specific stuff
#ifdef PHIL
		InitialisePhilScene();
#endif// PP: def PHIL

		// TP: Fire up the intro music
		musicSystem.ChooseMusic(map.sceneNumber,MUSICTYPE_MAIN,MUSICSTATE_START);

		SendPosterCompleteMessages();

#ifdef CONSUMERDEMO

		// PP: If the loading time is too quick, we don't get a chance to read all the demo objectives.
		// PP: define HOLD_DEMO_OBJECTIVES to wait for a button press before starting the game
#if((BPLATFORM == PC) || (BPLATFORM == XBOX) || (BPLATFORM == PS2))
#define HOLD_DEMO_OBJECTIVES
#endif// PP: platform

#ifdef HOLD_DEMO_OBJECTIVES

		// PP: don't hold the demo objectives if we're in attract mode
		if(gameStatus.demoMode != 1)// PP: '1' meaning 'playing'
		{
			// NH: If in one of the demos, hold onto the loading (what to do) screen until player presses a continue key
			if ((scene == SCENE_DEPTSTR || scene == SCENE_DEPTSTRVEHICLERACE) && (gameStatus.demoMode == 0))
			{
				BOOK	*bookPtr;	
				PAGE	*pagePtr;
				PAGE	*subPage;
				TEXTBOX *textBoxPtr;

				bookPtr = new BOOK("demo objectives prompt");

				// NH: Disable Safe Area so that text aligns with splash correctly
				bookPtr->flags &= ~BKFLAG_SAFEAREA;

				pagePtr = bookPtr->addPage("continue page");

				// PP: insert a sub-page to group the 2 textboxes
				
#if(BPLATFORM == XBOX)

				float	McTwico=-0.419f;
				pagePtr->insertFreeItem(subPage=new PAGE, RECTANGLE(0.0f, 0.5f, McTwico, 0.5f));

#else// PP: !xbox

				// NH: John, Phil.  If the PRESS START text is in the wrong position then what I've changed didn't work for you. 
				// NH: This is what was there before...
				// NH: pagePtr->insertFreeItem(subPage=new PAGE, RECTANGLE(0.0f, 0.5f, -0.5f, 0.5f));
				// NH: Also I set The BKFLAG_SAFEAREA thingy above which shouldn't be messing you about but you never know.
				pagePtr->insertFreeItem(subPage=new PAGE, RECTANGLE(0.0f, 0.5f, -0.419f, 0.5f));

#endif// PP: !xbox

				subPage->setSlack(false);
				subPage->setYAlign(PIFLAG_YALIGN_BOTTOM);
				
#ifdef ALLOW_CONTROLLERS_IN_ANY_PORTS
		
				if(!gameStatus.multiplayer.versusCPU)
				{
					// PP: 2 players

					// PP: if controllers can be used in any ports,
					// PP: ask player 2 to press start so's we know what port they're in

					subPage->insertItem(STR_PLAYER_2,true)
							->setStyle(TS_control)
							->setEffect(&PIE_flash);
				}

#endif// PP: def ALLOW_CONTROLLERS_IN_ANY_PORTS

#if(BPLATFORM==PC)
				subPage->insertItem(STR_PRESS_JUMP_TO_CONTINUE)
#else
				subPage->insertItem(STR_PRESS_START_TO_CONTINUE)
#endif // NH: #if(BPLATFORM==PC)
						->setWrap(false)
						->setStyle(TS_control)
						->setEffect(&PIE_flash);

				bookPtr->open();

				// NH: // PP: set up and open the books to be displayed during loading
				openLoadingBooks();

				demoObjectivesTimer=0.0f;

#ifdef ALLOW_CONTROLLERS_IN_ANY_PORTS

				if(!gameStatus.multiplayer.versusCPU)
				{
					// PP: multiplayer

					padCheckMode=PADCHECK_WAITING_FOR_PLAYER2;
					
					// PP: reset controller bindings where appropriate, to allow the appropriate player to choose their pad
					bindControllersToDefaultPorts();
				}

				while(!((gameStatus.multiplayer.versusCPU/*1p*/ && (controller1.startDebounceChannel->value||controller1.crossDebounceChannel->value))
						||(!gameStatus.multiplayer.versusCPU/*2p*/ && checkForStartAndBind(controller2, &controller1))))// PP: 2nd param: controller1 already bound; ignore any START signal from it

#else// PP: ifndef ALLOW_CONTROLLERS_IN_ANY_PORTS

				// NH: Keep drawing the splashscreen and all it's bits (minus the loading bar) until key is pressed
				while (!(controller1.startDebounceChannel->value || OKBUTTON))
					
#endif// PP: ndef ALLOW_CONTROLLERS_IN_ANY_PORTS
				{
					DrawLoadLoop(DRAW_ALL&(~DRAW_LOADBAR)&(~DRAW_LEVELNAME)|DRAW_MISSINGPAD);

					if((demoObjectivesTimer+=rawFTime) > HOLD_DEMO_OBJECTIVES_DURATION)
					{
#ifdef CONSUMERDEMO
#if(BPLATFORM == XBOX)// PP: NOT READY TO COMMENT THIS YET, GIMME TIME
						timedOutToDemoFrontEnd=true;
						// PP: TEMP TEST
						//demoIdleTime=60.0f;
#endif// PP: xbox
#endif// PP: def CONSUMERDEMO

						break;// PP: out of the while
					}
				}

				// PP: force-debounce them start buttons, otherwise it'll go STRAIGHT into pause
				biForceDebounce(controller1.startDebounceChannel);
				biForceDebounce(controller2.startDebounceChannel);

				// PP: found player 1/2
				padCheckMode=PADCHECK_NORMAL;

				// NH: // PP: close the books that were displayed during loading
				closeLoadingBooks();

				// NH: Close and free book nice and tidily
				BookList_closeAndFree(bookPtr);
			}
		}

#endif// PP: def HOLD_DEMO_OBJECTIVES
#endif// PP: def CONSUMERDEMO

		// PP: start timing the frame
		startTimingFrame();

		if (gameStatus.demoMode == -1)
		{// JW: Recording Demo
			dmData = new DemoModeData;

			dmData->session = biStartRecording(4);

			dmData->fps = (float *) ZALLOC(sizeof(float) * dmData->session->maxSamples);

			fpsStore = dmData->fps;

			bmSRand(721979);
		}
		else if (gameStatus.demoMode == 1)
		{// JW: Playing Demo from level pak file
			int noofBytes;

			uchar *sessionData, *sessPtr;

			dmData = new DemoModeData;

			bkHeapCheck();

			sessionData = bkLoadFile(map.mapPackage, "extras\\session.dat", NULL, NULL, NULL, NULL);

			sessPtr = sessionData;

			bkHeapCheck();

			memcpy(&dmData->numBytes, sessPtr, sizeof(int));
			sessPtr += sizeof(int);
			memcpy(&dmData->numSessBytes, sessPtr, sizeof(int));
			sessPtr += sizeof(int);
			memcpy(&dmData->numFpsBytes, sessPtr, sizeof(int));
			sessPtr += sizeof(int);
			
			dmData->session = (TBInputRecording *) ZALLOC(dmData->numSessBytes);
			dmData->fps = (float *) ZALLOC(dmData->numFpsBytes);

			memcpy(dmData->session, sessPtr, dmData->numSessBytes);
			sessPtr += dmData->numSessBytes;
			memcpy(dmData->fps, sessPtr, dmData->numFpsBytes);
			sessPtr += dmData->numFpsBytes;
			
			fpsStore = dmData->fps;
			
			bkHeapCheck();
			
			//FREE(sessionData);
			
			biStartPlayback(dmData->session);

			bmSRand(721979);

			DisableRumble(&controller1);
			DisableRumble(&controller2);
		}
		else if (gameStatus.demoMode == 2)
		{	// JW: playing demo from host session.dat file
			int noofBytes;

			dmData = new DemoModeData;

			TBHostFileHandle fpPtr;

			bkHostOpenFileReadOnly("session.dat", &fpPtr);

			noofBytes = bkHostReadFromFile(fpPtr, &dmData->numBytes, 4);
			noofBytes = bkHostReadFromFile(fpPtr, &dmData->numSessBytes, 4);
			noofBytes = bkHostReadFromFile(fpPtr, &dmData->numFpsBytes, 4);

			dmData->session = (TBInputRecording *) ZALLOC(dmData->numSessBytes);
			dmData->fps = (float *) ZALLOC(dmData->numFpsBytes);

			noofBytes = bkHostReadFromFile(fpPtr, dmData->session, dmData->numSessBytes);
			noofBytes = bkHostReadFromFile(fpPtr, dmData->fps, dmData->numFpsBytes);
			
			fpsStore = dmData->fps;

			bkHostCloseFile(fpPtr);
			
			biStartPlayback(dmData->session);

			bmSRand(721979);

			DisableRumble(&controller1);
			DisableRumble(&controller2);
		}
		else
		{
			// CMD: enable rumble for any scene which is not a demo scene
			// CMD: note: rumble being enabled will not override gameStatus.globalSettings.vibration
			EnableRumble(&controller1);
			EnableRumble(&controller2);
		}

		camera[0].fade.SetFadeDown();

		if (gameStatus.multiplayer.numScreens == 2 || !gameStatus.multiplayer.versusCPU)
		{
			camera[1].fade.SetFadeDown();
		}

		if (/*map.sceneNumber == SCENE_ZOOBOSS || 
			map.sceneNumber == SCENE_CITYBOSS || 
			map.sceneNumber == SCENE_WESTBOSS || 
			map.sceneNumber == SCENE_TAZPREBOSS || 
			map.sceneNumber == SCENE_TAZBOSS || */
			map.sceneNumber == SCENE_MUSEUMVEHICLERACE ||
			map.sceneNumber == SCENE_GRANDCVEHICLERACE ||
			map.sceneNumber == SCENE_DEPTSTRVEHICLERACE ||
			map.sceneNumber == SCENE_DESTRUCTIONAQUA ||
			map.sceneNumber == SCENE_DESTRUCTIONCONSTRUCT ||
			map.sceneNumber == SCENE_DESTRUCTIONTASMANIA ||
			map.sceneNumber == SCENE_ICEDOMERACE ||
			map.sceneNumber == SCENE_GOLDMINERACE ||
			map.sceneNumber == SCENE_SAFARIRACE)
		{
			if (!gameStatus.demoMode)
			{
				gameStatus.introState = INTRO_RULES;

#ifndef CONSUMERDEMO
				// JW: Show the instructions for the boss
				StartRuleBook();

				gameStatus.player1->actorStatus->tutorialBookFlag = 1;
#endif
			}
		}
		else
		{
			// TP: start the intro in cutscene mode
			gameStatus.introState = INTRO_CUTSCENE;
		}

		// CMD: place taz in costume when in bonus games
		// CMD: and request some mood music
		switch(scene)
		{
		case SCENE_MUSEUMVEHICLERACE:
		case SCENE_GRANDCVEHICLERACE:
		case SCENE_DEPTSTRVEHICLERACE:
		case SCENE_DESTRUCTIONAQUA:
		case SCENE_DESTRUCTIONCONSTRUCT:
		case SCENE_DESTRUCTIONTASMANIA:
		case SCENE_ICEDOMERACE:
		case SCENE_GOLDMINERACE:
		case SCENE_SAFARIRACE:
			if(gameStatus.multiplayer.numPlayers == 1)
			{
				SetupLevelCostume(gameStatus.player[i].actorInstance);

				PlayAnimationByName(gameStatus.player1,"idle1",4.0f,0,0,0.0f,ATTACK);
			}

			gameStatus.controlSuspend = true;

			break;
		}

#ifndef STANDALONE
#ifdef CONSUMERDEMO
		if (gameStatus.multiplayer.versusCPU && map.sceneNumber == SCENE_DEPTSTR)
		{
			PAGE	*page;

			char	timeString[8];

			demoTimerBook = new BOOK();

			page = demoTimerBook->addPage("timer page");

			page->insertItem(txtTimer = new TEXTBOX());

			demoGameTimer = to_gameplay;

			utilGetPrettyTime(timeString, demoGameTimer);

			txtTimer->sprintf8(timeString);
			txtTimer->setXAlign(PIFLAG_XALIGN_LEFT);
			txtTimer->setYAlign(PIFLAG_YALIGN_BOTTOM);
			txtTimer->setScale(1.5f);

			page = demoTimerBook->addPage("times up");

			page->insertItem(txtTimeUp = new TEXTBOX(STR_TIMES_UP));

			// PP: NEW: use proper page item effects on the timeup textbox
			txtTimeUp->setFontSize(3.0f)
				->setWrap(true)
				->setEffect(&PIE_iconSpin, PIEMODE_ENTER)
				->setEffect(&PIE_textLife, PIEMODE_ON)
				->setEffect(&PIE_textExplode, PIEMODE_EXIT);
		}

		// PP: move this to somewhere...
		// PP: in consumer demo, time out to the demo front end
		// PP: TEMP TEST ( = DEMO BODGE THAT I'M TOO TIRED TO COMMENT)
		if(timedOutToDemoFrontEnd)
		{
			//StartDemoMenuFadeDownCallback(&camera[0].fade);

			padCheckMode=PADCHECK_ALL;
			bindControllersToDefaultPorts();
			gameStatus.multiplayer.numPlayers=1;// PP: ?
			gameStatus.multiplayer.versusCPU=true;// PP: ?

			camera[0].fade.StartFadeDown(FADETYPE_NORMAL, 1.0f);

			camera[0].fade.SetFadeDownFinishedCallback(StartDemoMenuFadeDownCallback );

			// PP: TEMP TEST
			//demoIdleTime=60.0f;

			/*to_gameplay=0;
			demoGameTimer=0;*/
		}

#endif
#endif

#if(BPLATFORM == XBOX)
		if (gameStatus.splashScreen.resource)
		{
			FreeSplashScreen(&gameStatus.splashScreen);
		}
#endif	

		return SCENETYPE_INTRO;

		break;

	case SCENE_OBJECTVIEWONLY:
	
		pakIndex = LoadWholePackage("chars",1);

//		LoadTextmures("chars");
		InitCartoonLight("textures\\ramps.bmp");	// uses a texture from the taz.pak as a lighting ramp
		if (pakIndex)//(pakIndex = bkOpenPackage("chars")))
		{
			lastHandle = 0;
			while ((lastHandle = bkEnumPackageContents(pakIndex, lastHandle, FOURCC("ACTR"), &crc, NULL)))
			{
				actor = bkLoadActorByCRC(pakIndex, crc,0);
				AddActorToActorListByCRC(actor,crc,ACTORTYPE_ACTOR|ACTOR_MALLOCED);
			}

//			bkClosePackage(pakIndex);
		}
		strcpy(map.mapName,"chars");
		ListAnimatingTextures();
		break;
	}

// PP: TEMP TEST
#if(BPLATFORM == XBOX)
	if (gameStatus.splashScreen.resource)
	{
		FreeSplashScreen(&gameStatus.splashScreen);
	}
#endif	

	return -1;
}

/*	--------------------------------------------------------------------------------
	Function 	: PrepareScene
	Purpose 	: Prepares all the actors in a scene
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
int PrepareScene(int scene)
{
	switch(scene)
	{
	default:
		StartProfileTimer(&profile,1,"Prep");

		PrepareMap(&map);
		StartProfileTimer(&tonyProfile,1,"PrepScene");

		StartProfileTimer(&profile,4,"Shadow");
		CreateActorShadows(&characterInstances, &collisionCache);
		EndProfileTimer(&profile,4);
	
		PrepSpecial(map.sceneNumber);

		EndProfileTimer(&tonyProfile,1);
		EndProfileTimer(&profile,1);
		break;

	case SCENE_NOSCENE:
		break;
	case SCENE_OBJECTVIEWONLY:
		break;
	}

	// PP: now that the attached objects have been prepped for drawing, update the watch display if it exists
	if(CheatActive(CHEAT_DIGITALWATCH))
	{
		MonitorFX_watch.update();
	}	

	return TRUE;
}

/*	--------------------------------------------------------------------------------
	Function 	: StartSceneLevelDraw
	Purpose 	: Draws the background and then initiates level draw
	Parameters 	: scene number, camera to use
	Returns 	: 
	Info 		: This only starts the level drawing, it does not wait for it to finish
*/
void StartSceneLevelDraw(int scene,CAMERAINFO *cameraInfo)
{
	StartProfileTimer(&profile,2,"Draw");
	switch(scene)
	{
	default:
		DrawMapSet(&map,cameraInfo);
		break;
	case SCENE_OBJECTVIEWONLY:
		break;
	case SCENE_NOSCENE:
		break;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: StartSceneLevelDrawAlpha
	Purpose 	: Draws the background and then initiates level draw
	Parameters 	: scene number, camera to use
	Returns 	: 
	Info 		: This only starts the level drawing, it does not wait for it to finish
*/
void StartSceneLevelDrawAlpha(int scene,CAMERAINFO *cameraInfo)
{
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	baSetGlobalAlpha(256);
	bdSetIdentityObjectMatrix();

	switch(scene)
	{
	default:
		DrawMapSetAlpha(&map,cameraInfo);
		break;
	case SCENE_OBJECTVIEWONLY:
		break;
	case SCENE_NOSCENE:
		break;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: DrawScene
	Purpose 	: Draws all actors in a scene
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
//int	myr=0,myg=0,myb=0;
int DrawScene(int scene,CAMERAINFO *cameraInfo)
{
/*
	if (controller1.dPadXDebounceChannel->value < 0)
	{
		lighting = 1-lighting;
	}
*/
//	baSetLightColour(&cartoonLight,myr,myg,myb);

	SETZWRITE(TRUE);
	SETZTEST(LESSTHAN);
	ENABLEBLEND(TRUE);
	SETBLEND(BLEND);
/*	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);*/
	baSetGlobalAlpha(256);
	bdSetIdentityObjectMatrix();

	switch(scene)
	{
	default:
		// render environment must go first
// PP: REMOUT: TEMP		EndProfileTimer(&profile,6);

#ifdef CONSUMERDEMO
/* PP: REMOUT: Now handled by the Book List
		if (gameStatus.multiplayer.versusCPU && map.sceneNumber == SCENE_DEPTSTR && gameStatus.currentState == GAMESTATE_INGAME)
		{
			int		intaT = (int) demoGameTimer;
			float	faT = demoGameTimer - (float) intaT;

			if ((demoGameTimer > 10.0f) || (faT > 0.5f && faT < 0.75f) || (faT > 0.0f && faT < 0.25f) || (demoGameTimer < 0.0f))
			{
				bdSetIdentityObjectMatrix();
	
				SAVESTATES;

				// PP: draw navigation instructions...

				SETPROJECTION(2D);
				SETZTEST(ALWAYS);

//				demoTimerBook->draw();

				SETPROJECTION(3D);

				RESTORESTATES;
			}
		}*/
#endif

		StartProfileTimer(&profile,2,"Draw");
		//-------------------------------------Draw opaque stuff first

		if (map.introCutScene)
		{
			map.introCutScene->Draw();
		}
		if (map.exitCutScene)
		{
			map.exitCutScene->Draw();
		}

		DrawMap(&map);

		bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
		baSetGlobalAlpha(256);
		bdSetIdentityObjectMatrix();
		
		StartProfileTimer(&profile,7,"Taz");

		if (lighting)
		{
			bdSetRenderState(BDRENDERSTATE_LIGHTING, TRUE, 0);
			bdSetRenderState(BDRENDERSTATE_LIGHTING, TRUE, 1);
		}
		
		DrawActorInstances(&characterInstances);
		DrawAllMirrors(gameStatus.player[0].actorInstance);

		if (lighting)
		{
			bdSetRenderState(BDRENDERSTATE_LIGHTING, FALSE, 0);
			bdSetRenderState(BDRENDERSTATE_LIGHTING, FALSE, 1);
		}
		EndProfileTimer(&profile,7);
		
		StartProfileTimer(&profile,11,"floor");

#ifndef CONSUMERDEMO
		DrawFloorPolisherWantedPoster(floorPolisherWantedPosterInfo);
#endif

		EndProfileTimer(&profile,11);
//		DrawActorShadows(&characterInstances);
		
		DrawInstancesCharacterInfo(&characterInstances);

		// PP: if we're wearing the watch, draw the display
		if(watchLook)// PP: new optimisation
		{
			if(CheatActive(CHEAT_DIGITALWATCH))
			{
				MonitorFX_watch.draw();
			}
		}

		DrawSpecial(map.sceneNumber, cameraInfo);

		// TP: Draw extras on the security boxes
		DrawSecurityBoxes();

#ifndef CONSUMERDEMO
		// TP: Draw stinky cheese
		DrawStinkyCheese();
#endif

		CAMERAINFO*		cam;

		if(cameraInfo->cameraCase == NULL)
		{
			cam=cameraInfo;
		}
		else
		{
			cam=cameraInfo->cameraCase->camera;
		}

		// PP: draw flies...
		Flies_draw(cam);

		// PP: TEMP? draw first-person effects if we're in that mode
		if(firstPersonMode)
		{
			drawFirstPersonMode();
		}

		// PP: There's a few things that don't need to be done while we're in a boss game, at least not yet.  hmm
		if(!IN_BOSS_GAME)
		{
//			DrawTweety();
		}

		EndProfileTimer(&profile,2);
		break;
	case SCENE_OBJECTVIEWONLY:
		UpdateAnimatingTextures();
		break;
	case SCENE_NOSCENE:
		break;
	case SCENE_LANGUAGESELECT:
		if (map.introCutScene)
		{
			map.introCutScene->Draw();
		}
		if (map.exitCutScene)
		{
			map.exitCutScene->Draw();
		}

		DrawSpecial(scene, cameraInfo);
		break;
	}
	return TRUE;
}  

/*	--------------------------------------------------------------------------------
	Function 	: DrawScene2
	Purpose 	: Draws all alpha polygons in scene
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
int DrawScene2(int scene,CAMERAINFO *cameraInfo, int options)
{
	int	i;

	SETZWRITE(TRUE);
	SETZTEST(LESSTHAN);
	ENABLEBLEND(TRUE);
	SETBLEND(BLEND);

	baSetGlobalAlpha(256);
	bdSetIdentityObjectMatrix();

	switch(scene)
	{
	default:
		// render environment must go first
		DrawHaloObject(&map.levelInstances);

		//-------------------------------------Draw alpha and colour key below

		DrawMap2(&map);
		DrawActorShadows(&characterInstances);

		// TP: Draw decals should be done after shadows
		DrawAllDecals();

		DrawWaterRing();

		// PP: Draw vehicle effects (eg. floor polisher trail)
		DrawVehicles(&map.levelInstances);

		// PP: draw shockwaves after shadows
		Shockwave_draw();

		// PP: draw fireballs
// PP: REMOUT: gonna draw this after spin effect		Fireball_draw();

		drawThwap();
		
		// PP: hmm not too sure where to put this
		// PP: draw timeslice effect
		if(VFX_effectActive(VFX_TIMESLICE))
		{
			VFX_timesliceDraw();
		}

// PP: REMOUT: gonna draw this after spin effect				DrawSubtitles();
// PP: REMOUT: gonna draw this after spin effect				DrawSpecial2(map.sceneNumber, cameraInfo);

		DrawMagnetStuff(&map.invisibleInstances);
		DrawInstancesCharacterInfo2(&characterInstances);

		if(gameStatus.player[0].camera != NULL)// PP: added this check for the main game intro
		{
			DrawScreenSmashEffect(gameStatus.player[0].camera->smashInfo);		// CMD: draw smash for player1's camera
		}

		// PP: FILM BORDERS
		if(videoMode.filmBorders)
		{
			// PP: not too sure where to put this
			if(CURRENTLY_DRAWING_FULL_SCREEN)
			{
				drawFilmBorders();
			}
		}

		// PP: draw Tony-spcific things
#ifdef TONY
		DrawTonyScene(scene, cameraInfo);
#endif // PP: def TONY

		// PP: draw Phil-spcific things
#ifdef PHIL
		DrawPhilScene(scene, cameraInfo);
#endif // PP: def PHIL

		break;
	case SCENE_OBJECTVIEWONLY:
		break;
	case SCENE_NOSCENE:
		break;
	}

	SETZWRITE(TRUE);
	SETZTEST(LESSTHAN);
	ENABLEBLEND(TRUE);
	SETBLEND(BLEND);

	DrawSpecial2(map.sceneNumber, cameraInfo, TRUE);

	if (options&ML_DRAWEXTRAS) baRenderExtraSystem(NULL, (void*)EXTRA_ALPHA);

	// CMD: always draw spin last
	for (i=0;i<gameStatus.multiplayer.numPlayers;i++) 
	{
		if (gameStatus.player[i].actorInstance)
		{
			DrawSpinEffectForActor(gameStatus.player[i].actorInstance);
		}
	}

	// PP: ======== I moved these here from before the spin effect
	Fireball_draw();
	DrawSubtitles();
	DrawSpecial2(map.sceneNumber, cameraInfo, FALSE);
	// PP: ========

	DrawLoseCostumeEffect(&map.characterInstances);
	DrawLoseCostumeEffect(&map.inactiveCharacterInstances);
//	if(gameStatus.player[0].actorInstance->characterInfo->costumeChangeInfo)
//	{
//		DrawLoseCostumeEffect(gameStatus.player[0].actorInstance, gameStatus.player[0].actorInstance->characterInfo->costumeChangeInfo);
//		if(gameStatus.player[0].actorInstance->characterInfo->costumeChangeInfo->spellEffectInfo)
//		{
//			DrawVoodooSpellEffect(gameStatus.player[0].actorInstance->characterInfo->costumeChangeInfo->spellEffectInfo);
//		}
//	}

	DrawScreenScoreList();

	// TP: put this at end so it doesn't mess up any render states
	if (map.introCutScene)
	{
		map.introCutScene->Draw2();
	}
	if (map.exitCutScene)
	{
		map.exitCutScene->Draw2();

#ifndef CONSUMERDEMO
		// PP: SPECIAL-CASE - SORRY!: pong dust needs to get drawn after the cutscene spin effect!
		if(map.sceneNumber == SCENE_ZOOBOSS)// PP: sorry
		{
			PongDust_draw();
		}
#endif
	}
	return TRUE;
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateScenePreLevel
	Purpose 	: Update scene prior to beginning drawing level
	Parameters 	: scene number
	Returns 	: 
	Info 		: This is a safe place to request level change etc.
*/
void UpdateScenePreLevel(int scene)
{
	char		message[256];

#ifdef LEVELCHANGETEST
	if (gameStatus.gameTime > 2.0f)
	{
		int level = bmRand()%SCENE_TAZBOSS;
		if ((level != SCENE_ZOOBOSS)&&(level != SCENE_WESTBOSS)&&(level != SCENE_CITYBOSS))
		{
			sprintf(message, "_%s", sceneFileName[level]);
			bkGenerateEvent("scenechange",message);
		}
	}
#endif
	switch(scene)
	{
	default:
		// check for messages
		RespondToSoundMessages();
		RespondToSceneChangeMessages();
		RespondToTriggerMessages();
	case SCENE_OBJECTVIEWONLY:
		break;
	case SCENE_NOSCENE:
		{
			// PP: TEMP TEST
			int x=0;
		}
		break;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateScene
	Purpose 	: Update scene animations and status etc.
	Parameters 	: scene number
	Returns 	: 
	Info 		: This update occurs during the level draw, hence requesting level change in here would be bad.
*/
int UpdateScene(int scene)
{
	int i;

#ifdef CONSUMERDEMO	
	if (!gameStatus.demoMode && ((gameStatus.multiplayer.numPlayers == 1 && controller1.idleTime > to_inactive) || (gameStatus.multiplayer.numPlayers == 2 && controller1.idleTime > to_inactive && controller2.idleTime > to_inactive)))
	{
		if (gameStatus.currentState != GAMESTATE_INTRO && gameStatus.currentState != GAMESTATE_OUTRO && !camera[0].fade.GetFadeSpeed())
		{
			//StartFadeDown(&camera[0].fade, FADETYPE_NORMAL, 1.0f);
			
			//SetFadeDownFinishedCallback(&camera[0].fade, DemoAttractFadeDownCallback);
			
			//if (gameStatus.multiplayer.numPlayers == 2)
			//	StartFadeDown(&camera[1].fade, FADETYPE_NORMAL, 1.0f);

			#ifndef STANDALONE
				if (demoTimerBook)
					demoTimerBook->close();
			#endif

			bkPrintf("StartDemoAttract called from Update Scene timeout");

			StartDemoAttract();
		}
	}
#endif

	switch(scene)
	{
	default:

		StartProfileTimer(&profile,3,"Update");
		// animate all animated actors

		if (gameStatus.currentState != GAMESTATE_CAMERACASE)
			AnimateActorInstances(&characterInstances);

		// TP: Like it sounds
		UpdateSecurityBoxes();

#ifndef CONSUMERDEMO
		// TP: update the stinky cheese
		UpdateStinkyCheese();

		// TP: update the pirate ape's banana skins
		UpdateAllBananaSkins();

		// CMD: update buzzard ai scripts
		UpdateBuzzardAIScripts(&map.characterInstances);
#endif

		// update mape
		UpdateMap(&map);

		// update any actor status info, THIS MUST BE DONE AFTER UPDATEMAP FOR THE PLATFORMS TO WORK
		UpdateActorStatusInstances(&characterInstances);

		StartProfileTimer(&tonyProfile,4,"UpScene");

		// update collision cache
		UpdateCollisionCache(&collisionCache);

		//UpdateFallingObjectList(&fallingObjectList);
		UpdateWaterRing();
		UpdateCloudsInSky();
		UpdatePrecipitationList();

#ifndef CONSUMERDEMO
		UpdateFloorPolisherWantedPoster(floorPolisherWantedPosterInfo);
#endif
		
		updateThwap();

		UpdateScrollingTextures();

		UpdatePromptBook();

		// PP: update shockwaves
		Shockwave_update();

		// PP: update fireballs
		Fireball_update();

		// PP: update flies
		Flies_update();

		// PP: update shadow light stuff if appropriate
		if(shadowLightDebug)
		{
			updateShadowLights();
		}

		// PP: Update vehicles
		UpdateVehicles(&map.levelInstances);


		// PP: update video effects
		VFX_update();

		UpdateDecals();

		if ((gameStatus.currentState!=GAMESTATE_INTRO)&&(gameStatus.currentState!=GAMESTATE_OUTRO))
		{
			UpdateSpecial(map.sceneNumber);
		}
		for (i=0;i<gameStatus.multiplayer.numPlayers;i++) 
		{
			if (gameStatus.player[i].actorInstance)
			{
				UpdateSpinEffectForActor(gameStatus.player[i].actorInstance);
			}
		}

		// TP: update any subtitle overlays
		UpdateSubtitles();

		// PP: TEMP?: update cheats
		UpdateCheats();

/* PP: REMOUT: Now Handled by the Book List			
		if (gameStatus.demoMode && demoBook)
			demoBook->update();
*/
		// CMD: update any rumble effects
		UpdateRumble(&controller1);

		// PP: don't update controller 2's rumble unless we're in 2 player
		// PP: (could alternatively check controller's 'player' index inside UpdateRumble)
		if(!gameStatus.multiplayer.versusCPU)// PP: 2 player
		{
			UpdateRumble(&controller2);
		}

		// CMD: update screen scores stuff
		UpdateScreenScoreList();

		// PP: update animating textures.
		// PP: This used to get done in the draw stage (so that it worked in pause mode, for some reason),
		// PP: but I think it looks better to freeze them for pause mode - don't you?
		UpdateAnimatingTextures();

#ifdef TONY
		UpdateTonyScene(scene);
#endif

		// PP: udpate Phil-specific stuff
#ifdef PHIL
		UpdatePhilScene(scene);
#endif

#ifndef STANDALONE
#ifdef CONSUMERDEMO
		if (gameStatus.multiplayer.versusCPU && map.sceneNumber == SCENE_DEPTSTR && gameStatus.currentState == GAMESTATE_INGAME)
		{
// PP: REMOUT: Now handled by the Book List			demoTimerBook->update();
			if (demoGameTimer > -DEMO_TIMEUP_DURATION)
				demoGameTimer -= rawFTime;// PP: REMOUT: needs to be rawFTime because we don't want to see the timer slow down during slomo poster destructions	fTime;
			
			if(demoTimerBook->getCurrentPage() == demoTimerBook->getPage("timer page"))
			{	// JW: Timer Page
				if (demoGameTimer <= 0.0f)
				{
					PauseRumble(&controller1);
					PauseRumble(&controller2);

					FlushRumbleQueue(&controller1);
					FlushRumbleQueue(&controller2);

					UpdateRumble(&controller1);
					UpdateRumble(&controller2);

					//gameStatus.controlSuspend = true;

					killThwap();

					demoTimerBook->nextPage();
				}

				char string[16];

				utilGetPrettyTime(string, demoGameTimer);
				
				txtTimer->sprintf8(string);
			}
			else
			{	// JW: Times Up Page
				// PP: REMOUT: now using prper page item effects on the time-up page	txtTimeUp->setScale(txtTimeUp->getScale()+0.15f);

				if (demoGameTimer < -DEMO_TIMEUP_DURATION)
				{
					// TP: make sure we have secure access to the fade
					if (camera[0].fade.Lock())
					{
						demoGameTimer = -1;

						// PP: close the demo timer book as we fade out
						demoTimerBook->close();

						camera[0].fade.StartFadeDown(FADETYPE_NORMAL, 0.5f, true);
						camera[0].fade.SetFadeDownFinishedCallback(StopDemoGameFadeDownCallback);
						camera[0].fade.Unlock();
					}
				}
			}
		}
#endif
#endif

		// PP: I put this in temporarily to test stuff // PP: udpate Phil-specific stuff
/*#ifdef CLAUDE
		UpdateClaudeScene(scene);
#endif*/

		// PP: update time effects
		TimeFX_update();

		EndProfileTimer(&tonyProfile,4);
		EndProfileTimer(&profile,3);
		break;
	case SCENE_OBJECTVIEWONLY:
		break;
	case SCENE_NOSCENE:
		break;
	case SCENE_LANGUAGESELECT:
		UpdateSpecial(scene);
		break;
	}

	return TRUE;
}

/*	--------------------------------------------------------------------------------
	Function 	: FreeScene
	Purpose 	: Free all scene data, instances, actors and textures
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
int FreeScene(int scene)
{
	int i;

	if (scene < SCENE_NUMSCENES)
	{
		gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[scene].time = gameStatus.playTime;
	}

	bkHeapCheck();
	// TP: just in case the level was exited with the pause menu active
	// PP: MOVED THIS HERE
	ExitPauseMode();

	bkPrintf("FreeScene(%d,)\n",scene);

	// CMD: flush the rumble queue
	for(i = 0;i < gameStatus.multiplayer.numPlayers;i++)
	{
		// PP: Player i's controller could be NULL at this point
		if(gameStatus.player[i].controller != NULL)
		{
			if(gameStatus.player[i].controller->rumbleController)
			{
				FlushRumbleQueue(gameStatus.player[i].controller);
			}
		}
	}
	biReadDevices();

	StopAllChannels();
	
	switch(scene)
	{
	default:

#ifndef STANDALONE
#ifdef CONSUMERDEMO
		if (demoTimerBook)
		{
			SAFE_DELETE(demoTimerBook);
		}
#endif
#endif

		bkHeapCheck();

		if (gameStatus.demoMode == -1)
		{// JW: Recording Demo
			if (dmData)
			{
				TBHostFileHandle fpPtr;

				dmData->numFpsBytes = sizeof(float) * dmData->session->noofSamples;

				// close the session and find out how many bytes it is taking
				dmData->numSessBytes = biStopRecording(dmData->session);

				dmData->numBytes =  dmData->numFpsBytes +
									dmData->numSessBytes +
									(sizeof(int) * 5);

				// write the session to a file
				bkHostCreateFile("session.dat", &fpPtr);
				bkHostWriteToFile(fpPtr, &dmData->numBytes, sizeof(int));
				bkHostWriteToFile(fpPtr, &dmData->numSessBytes, sizeof(int));
				bkHostWriteToFile(fpPtr, &dmData->numFpsBytes, sizeof(int));
				bkHostWriteToFile(fpPtr, dmData->session, dmData->numSessBytes);
				bkHostWriteToFile(fpPtr, dmData->fps, dmData->numFpsBytes);
				bkHostCloseFile(fpPtr);
			}
		}		
		
		if (gameStatus.demoMode)
		{
			if (dmData)
			{
				bkPrintf("Close Demo Book");

				// PP: clear any fade-finished callback, eg in case taz has just fallen in water or that
				// PP: NOTE: this doesn't fix everything...
				camera[0].fade.SetFadeDownFinishedCallback(NULL);

				demoBook->shutdown();

				gameStatus.demoMode = 0;

				SAFE_DELETE(demoBook);
				FREE(dmData->fps);
				FREE(dmData->session);
				FREE(dmData);

				demoBook = 0;
			}
		}

	bkHeapCheck();
		// TP: Kill off any security boxes on the level
		KillSecurityBoxes();

#ifndef CONSUMERDEMO
		// TP: remove any left over cheese
		KillStinkyCheese();

		// TP: Kill the banana skins
		KillAllBananaSkins();
#endif

		killThwap();

		KillPromptBook();

		// TP: Delete pointers to all animating textures
		FreeAnimatingTextures();

		// TP: just in case the level was exited with the pause menu active
// PP: REMOUT: MOVED FURTHER BACK - THIS WAS TOO LATE		ExitPauseMode();

		// delete event clients and managers
		DeleteEventHandlers();
		DeleteEventManagers();

		ShutdownSpecial(map.sceneNumber);

		// PP: shutdown flies
		Flies_shutdown();

		DestroySubtitles();

		// delete the cameras
		for (i=0;i<MAX_SCREENS;i++)
		{
			DestroyCamera(&camera[i]);
		}

		// delete camera cases
		DeleteAllCameraCases();

		// delete the collision cache
		FreeCollisionCache(&collisionCache);

		FreeAllFallingObjectsFromList();
		FreeAllPrecipitationFromList();
		FreeWaterRingList(WATEREFFECT_ALL);
		FreeCloudBank();
		FreeAllEffectsFromList();
		FreeAllFallingObjectScaleInfo(&map.levelInstances);

#ifndef CONSUMERDEMO
		FreeFloorPolisherWantedPosterInfo(&floorPolisherWantedPosterInfo);
#endif

		ShutdownBarrierSystem();

		// PP: shutdown shockwaves
		Shockwave_shutdown();

		// PP: Shutdown fireballs
		Fireball_shutdown();

		// TP: delete all decals
		DeleteAllDecals();

//		DestroyTweety();

		// delete the path list
		DestroyPathList();

		// delete and re-initialise the extras
		baResetExtraSystem(NULL);

		KillPhoneBoxBook();	// JW: Destroy "can't enter police box" book

		// destroy cartoon 
		DestroyCartoonLight();

		// free map
		FreeMap(&map);

		// remove all actor instances
		FreeAllActorInstances(&characterInstances);

		// remove all actors not loaded from LoadPackage
		FreeAllActors();

		FreeLevelSpecificObjects(map.sceneNumber);

		FreeWholePackage(map.mapName);
		FreeWholePackage("taz");
		FreeStatsDisplays();
		FreeScreenScoreList();

#ifndef CONSUMERDEMO
		FreeEndGameCredits();
#endif

#ifndef CONSUMERDEMO
		if(map.sceneNumber == SCENE_WESTBOSS)
		{
			// PP: switch back to 16 bit display now that everything's shut down
			WestBoss_game.postShutdown();
		}
#endif


		//Clean up sounds
#if BPLATFORM!=GAMECUBE
		FreeLevelSamples();
#else
		FreeAllLevelSamplesFromARAM();
#endif

		// TP: kill of any books awaiting closing
		BookList_finishCloses();

		break;
	case SCENE_OBJECTVIEWONLY:
		FreeAnimatingTextures();
		DestroyCartoonLight();
		FreeAllActors();
		break;
	case SCENE_NOSCENE:
		break;
	//case SCENE_LANGUAGESELECT:
	//	ShutdownSpecial(scene);
		break;
	}

#ifdef PHIL// PP: I want to see what's going on
#define MEMLEAKS_TO_DEBUG_WINDOW
#endif

#ifdef ENABLESNAPSHOT

#ifndef MEMLEAKS_TO_DEBUG_WINDOW
	bkSetDebugStream(&map.errorStream);
#endif// PP: ndef MEMLEAKS_TO_DEBUG_WINDOW

	bkPrintf("Level %s exit snapshot:\n", sceneText[map.sceneNumber]);
	bkPrintf("---------------------------------------------------------------------------\n");

	// NH: In Demo when quitting snap was NULL and crashed PC, so if snap is NULL skip this section
	if (!snap)
		bkPrintf("Null Snap.  Skipping Compare\n");
	else
	{
		if (bkCompareHeapSnapshot(snap))
		{
			//DrawMemLeakWarning();
		}
		bkDeleteHeapSnapshot(snap);
		bkHeapCheck();
		bkHeapStats();
		// TP: display list of packages left in memory
		PrintPackageStates();
	}
	
	bkPrintf("\n\n\n\n");

#ifndef MEMLEAKS_TO_DEBUG_WINDOW
	bkFlushDebugStreamToFile(&map.errorStream);
	bkSetDebugStream(NULL);
	bkFlushDebugStreamToFile(&map.optimizeStream);
	bkFlushDebugStreamToFile(&map.memoryStream);
#endif// PP: ndef MEMLEAKS_TO_DEBUG_WINDOW

#endif// PP: def ENABLESNAPSHOT
	
	return TRUE;
}

#define MAX_ENEMIES_PER_LEVEL (3)

// TP: lists the characters that should be loaded for each scene
char	*charsToLoad[SCENE_TAZPREBOSS][MAX_ENEMIES_PER_LEVEL]=
{
	{NULL},	//SCENE_LANGUAGESELECT,
	{NULL},	//SCENE_FRONTEND,
	{NULL},	//SCENE_PLAYPEN,	

	{"keeper",NULL},					//SCENE_ZOOHUB,
	{"polarbear","browbear","keeper"},	//SCENE_ICEDOME,
	{"alligator","browbear","keeper"},	//SCENE_SAFARI,
	{"pirateape","shark","keeper"},		//SCENE_AQUA,
	{NULL},								//SCENE_ZOOBOSS,

	{NULL},								//SCENE_CITYHUB,
	{"doggy","keeper","greenguard"},// TP: !!!!!!!!!!!!!!!!!!!!!!! THIS IS MEANT TO BE GREENGUARD, LEAVE IT ALONE!!!!!!!!!!!!!!	//SCENE_DEPTSTR,	
	{"security bot","guard","keeper"},	//SCENE_MUSEUM,
	{"keeper","construction worker","construction bot"},	//SCENE_CONSTRUCT,
	{NULL},								//SCENE_CITYBOSS,

	{"beaky",NULL},						//SCENE_WILDWESTHUB,
	{"beaky","demondog","keeper"},		//SCENE_GHOSTTOWN,
	{"bjscargill","minerbot","keeper"},	//SCENE_GOLDMINE,
	{"keeper","bjshellac","nastycan"},	//SCENE_GRANDCANYON,
	{NULL},								//SCENE_WESTBOSS,

	{"tribalchief","keeper","parrot"},	//SCENE_TAZHUB,
};

/*	--------------------------------------------------------------------------------
	Function 	: StartLoading
	Purpose 	: Start the background load of needed items
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void StartLoading(int sceneNumber)
{
	char	tempString[256];

	if (sceneNumber <= SCENE_TAZHUB)
	{
		// TP: load enemies in charsToLoad
		for (int i=0;i<MAX_ENEMIES_PER_LEVEL;i++)
		{
			if (charsToLoad[sceneNumber][i])
			{
				sprintf(tempString,"%s.obe",charsToLoad[sceneNumber][i]);
				LoadWholePackageBkg(charsToLoad[sceneNumber][i],tempString,1);
				LoadCharacterSamplesBkg(charsToLoad[sceneNumber][i]);
			}
			else
			{
				// TP: all done
				break;
			}
		}
	}

	// TP: anything else to load
	switch(sceneNumber)
	{
	case SCENE_MUSEUM:
		LoadWholePackageBkg("floorpolisher", NULL, 1);					// PP: load floor-polisher resources
		break;

	case SCENE_PLAYPEN:
		LoadWholePackageBkg("keeper","keeper.obe");
		LoadWholePackageBkg("polarbear","polarbear.obe");
		LoadWholePackageBkg("coyote","coyote.obe");
		LoadWholePackageBkg("daft","daft.obe");
		LoadWholePackageBkg("gossamer","gossamer.obe");
		LoadWholePackageBkg("sylvester","sylvester.obe");
		LoadWholePackageBkg("tycoon","tycoon.obe");
		LoadWholePackageBkg("yosemite","yosemite.obe");
		LoadWholePackageBkg("alligator","alligator.obe");
		LoadWholePackageBkg("shark","shark.obe");
		LoadWholePackageBkg("elephant","elephant.obe");
		LoadWholePackageBkg("mrfudd","mrfudd.obe");
		LoadWholePackageBkg("catcher","catcher.obe");
		LoadWholePackageBkg("ape","ape.obe");
		LoadWholePackageBkg("browbear","browbear.obe");
		LoadWholePackageBkg("pirateape","pirateape.obe");
		LoadWholePackageBkg("security bot","security bot.obe");
		LoadWholePackageBkg("construction worker", "construction worker.obe");
		LoadWholePackageBkg("construction bot", "construction bot .obe");
		LoadWholePackageBkg("guard","guard.obe");
		LoadWholePackageBkg("beaky","beaky.obe");
		LoadWholePackageBkg("bjshellac","bjshellac.obe");
		LoadWholePackageBkg("construction worker","construction worker.obe");
		LoadWholePackageBkg("doggy","doggy.obe");
		LoadWholePackageBkg("parrot","parrot.obe");
		break;
	case SCENE_MUSEUMVEHICLERACE:
		LoadWholePackageBkg("floorpolisher", NULL, 1);					// PP: load floor-polisher resources
		break;
	case SCENE_TAZBOSS:
	case SCENE_TAZPREBOSS:
	case SCENE_WESTBOSS:
		LoadWholePackageBkg("crashhelmet", "crashhelmet.obe", 1);		// PP: load crash-helmet resources
	}
}

/*  --------------------------------------------------------------------------------
	Function	: PredictAmountToLoad
	Purpose		: to return the amount of bytes to be loaded
	Parameters	: sceneNumber
	Returns		: 
	Info		: 
*/

int	PredictAmountToLoad(int sceneNumber)
{
	int	bytes = 0;
	char	name[256];

	if (sceneNumber <= SCENE_TAZHUB)
	{
		// TP: load enemies in charsToLoad
		for (int i=0;i<MAX_ENEMIES_PER_LEVEL;i++)
		{
			if (charsToLoad[sceneNumber][i])
			{
				bytes+=PackageSize(masterPakfile, charsToLoad[sceneNumber][i]);
				sprintf(name, "%ssound", charsToLoad[sceneNumber][i]);
				bytes+=PackageSize(masterPakfile, name);
			}
			else
			{
				// TP: all done
				break;
			}
		}
	}
	return bytes;
}

/*	--------------------------------------------------------------------------------
	Function 	: CompleteLoad
	Purpose 	: The items are loaded so read contents
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void CompleteLoad(int sceneNumber)
{
	// TP: complete the load
	if (sceneNumber <= SCENE_TAZHUB)
	{
		// TP: load enemies in charsToLoad
		for (int i=0;i<MAX_ENEMIES_PER_LEVEL;i++)
		{
			if (charsToLoad[sceneNumber][i])
			{
				LoadCharacterSamples(charsToLoad[sceneNumber][i]);
			}
			else
			{
				// TP: all done
				break;
			}
		}
	}

	// TP: anything else
	switch(sceneNumber)
	{
	case SCENE_TAZHUB:
		LoadActor("egg.obe", "parrot", 0);
		LoadActor("friedegg.obe", "parrot", 0);
		break;

	case SCENE_PLAYPEN:
		break;
	case SCENE_WILDWESTHUB:
		break;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: FreeLevelSpecificObjects
	Purpose 	: Free all level specific objects
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void FreeLevelSpecificObjects(int sceneNumber)
{

	switch(sceneNumber)
	{
	case SCENE_PLAYPEN:
		FreeWholePackage("keeper");
		FreeWholePackage("polarbear");
		FreeWholePackage("coyote");
		FreeWholePackage("daft");
		FreeWholePackage("gossamer");
		FreeWholePackage("sylvester");
		FreeWholePackage("tycoon");
		FreeWholePackage("yosemite");
		FreeWholePackage("alligator");
		FreeWholePackage("shark");
		FreeWholePackage("elephant");
		FreeWholePackage("mrfudd");
		FreeWholePackage("catcher");
		FreeWholePackage("ape");
		FreeWholePackage("browbear");
		FreeWholePackage("pirateape");
		FreeWholePackage("security bot");
		FreeWholePackage("construction worker");
		FreeWholePackage("construction bot");
		FreeWholePackage("guard");
		FreeWholePackage("beaky");
		FreeWholePackage("bjshellac");
		FreeWholePackage("construction worker");
		FreeWholePackage("doggy");
		FreeWholePackage("parrot");
		break;
	case SCENE_WILDWESTHUB:
		FreeWholePackage("buzzard");
		break;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: LoadGenericObjects
	Purpose 	: Load all generic objects
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void LoadGenericObjects(int sceneNumber)
{
	switch(sceneNumber)
	{
	case SCENE_FRONTEND:
		LoadActor("extras\\tazdevil_frontend.obe","frontend",0);
		LoadActor("extras\\tazpath_opt01.obe","frontend",0);
		LoadActor("extras\\tazpath_opt02.obe","frontend",0);
		LoadActor("extras\\tazpath_sound01.obe","frontend",0);
		LoadActor("extras\\tazpath_sound02.obe","frontend",0);
		LoadActor("extras\\tazpath_save01.obe","frontend",0);
		LoadActor("extras\\tazpath_save02.obe","frontend",0);
		LoadActor("extras\\tazpath_savea01.obe","frontend",0);
		LoadActor("extras\\tazpath_saveb01.obe","frontend",0);
		LoadActor("extras\\tazpath_savec01.obe","frontend",0);
		LoadActor("extras\\tazpath_savea02.obe","frontend",0);
		LoadActor("extras\\tazpath_savea201.obe","frontend",0);
		LoadActor("extras\\tazpath_saveatob.obe","frontend",0);
		LoadActor("extras\\tazpath_saveb02.obe","frontend",0);
		LoadActor("extras\\tazpath_saveb201.obe","frontend",0);
		LoadActor("extras\\tazpath_savebtoa.obe","frontend",0);
		LoadActor("extras\\tazpath_savebtoc.obe","frontend",0);
		LoadActor("extras\\tazpath_savec02.obe","frontend",0);
		LoadActor("extras\\tazpath_savec201.obe","frontend",0);
		LoadActor("extras\\tazpath_savectob.obe","frontend",0);
		LoadActor("extras\\tazpath_multiplayer01.obe","frontend",0);
		LoadActor("extras\\tazpath_multiplayer02.obe","frontend",0);
		LoadActor("extras\\tazpath_records01.obe","frontend",0);
		LoadActor("extras\\tazpath_records02.obe","frontend",0);
		LoadActor("extras\\tazpath_extras01.obe","frontend",0);
		LoadActor("extras\\tazpath_extras02.obe","frontend",0);
		LoadActor("extras\\alarmclock01.obe","frontend",0);
		LoadActor("extras\\television01.obe","frontend",0);
		LoadActor("extras\\gramophone.obe","frontend",0);
		LoadActor("extras\\tntplunger01.obe","frontend",0);
		LoadActor("extras\\pizzaboxes.obe","frontend",0);
		LoadActor("extras\\camerastartpos.obe","frontend",0);
		LoadActor("extras\\deckchair01.obe","frontend",0);
		break;
	case SCENE_AQUA	:
		LoadActor("extras\\ball.obe", "aqua", 0);
		break;
	case SCENE_GOLDMINE:
		LoadActor("extras\\minecart.obe", "goldmine", 0);
		break;
	case SCENE_GHOSTTOWN:
		LoadActor("extras\\minecart.obe", "ghost", 0);
		LoadActor("extras\\mouse.obe", "ghost", 0);
		LoadActor("extras\\ball.obe", "ghost", 0);
		LoadActor("extras\\crate.obe", "ghost", 0);
		break;
	case SCENE_WILDWESTHUB:
		LoadActor("extras\\minecart.obe", "westhub", 0);
		LoadActor("extras\\policebox.obe", map.mapName, 0);
		LoadActor("extras\\policehiderbox.obe", map.mapName, 0);
		break;
	case SCENE_ZOOHUB:
	case SCENE_CITYHUB:
		LoadActor("extras\\policebox.obe", map.mapName, 0);
		LoadActor("extras\\policehiderbox.obe", map.mapName, 0);
		break;
	case SCENE_DEPTSTR:
		LoadActor("extras\\rapperblast.obe", map.mapName, 0);
		break;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: LoadPermanentResources
	Purpose 	: Load all generic objects
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void LoadPermanentResources(void)
{
	LoadActor("capturecage_base.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("capturecage_top.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("bubble.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("drumstickfull.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("drumstickhalf.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("drumstickempty.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("cloudwacky.obe", "resobjs",ACTOR_PERMANENT);	
	LoadActor("injuredstar.obe", "resobjs",ACTOR_PERMANENT);
	LoadActor("tazspark.obe", "resobjs",ACTOR_PERMANENT);
	LoadActor("agentwatch.obe","resobjs",ACTOR_PERMANENT);					// PP: Load the old Secret Agent wristwatch - this can be worn by activating the DIGITALWATCH cheat

	LoadActor("food_dynamite.obe", "resobjs",ACTOR_PERMANENT);
	LoadActor("clubsandwich.obe", "resobjs",ACTOR_PERMANENT);
	LoadActor("dynamite.obe", "resobjs",ACTOR_PERMANENT);
	LoadActor("snowball.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("rivet.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("voodoocloud.obe", "resobjs", ACTOR_PERMANENT);

	LoadActor("postcard.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("explosionyellow.obe", "resobjs",ACTOR_PERMANENT);
	LoadActor("explosionred.obe", "resobjs",ACTOR_PERMANENT);
	LoadActor("phonebox.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("phonebox_collision.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("phonehiderbox.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("burpcan.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("testtube.obe", "resobjs",ACTOR_PERMANENT);
	LoadActor("bubblegum.obe", "resobjs",ACTOR_PERMANENT);
	LoadActor("hiccupcan.obe", "resobjs",ACTOR_PERMANENT);
	LoadActor("chillipepper.obe", "resobjs",ACTOR_PERMANENT);
	LoadActor("blastradius.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("gumbubble.obe","resobjs",ACTOR_PERMANENT);

	LoadActor("arrow.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("checkpoint.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("startpoint.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("target.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("secbox.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("spinpad.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("tazeyeblink.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("boxing.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("cheeese.obe","resobjs",ACTOR_PERMANENT);

	LoadActor("ice clock.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("speed clock.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("mplayer box.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("yosamiteskull.obe","resobjs",ACTOR_PERMANENT);

	LoadActor("capturecagedebris1.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("capturecagedebris2.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("capturecagedebris3.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("capturecagedebris4.obe","resobjs",ACTOR_PERMANENT);
	LoadActor("capturecagedebris5.obe","resobjs",ACTOR_PERMANENT);
}

/*	--------------------------------------------------------------------------------
	Function 	: FreePermanentResources
	Purpose 	: Free all permanent resources
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void FreePermanentResources(void)
{
	FreeActor("tazdevil.obe");
	FreeActor("spindevil.obe");
	FreeAllPermanentActors();
	FreeWholePackage("taz");
	FreeWholePackage("resobjs");
}

/*  --------------------------------------------------------------------------------
	Function	: DrawMemLeakWarning
	Purpose		: If there are memory leaks in the level this will display a warning
	Parameters	: 
	Returns		: 
	Info		: 
*/
void DrawMemLeakWarning(void)
{
	char		buf[256];
	ushort		string16[256];
	TBMatrix	matrix;

	while(!controller1.crossDebounceChannel->value)
	{
		musicSystem.UpdateMusicSystem();

		// PP: update memory card display
		MemCard_update();

		biReadDevices();

		// prepare scene
		bdSetupView();
		bdBeginScene();

		//draw fade
		bdSetIdentityObjectMatrix();
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);

		bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
		bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
//			MemCard_draw();

		bmMatScale(matrix,0.5f,0.5f,0.5f);
		bdSetObjectMatrix(matrix);
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
		bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);

		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

		sprintf(buf, "Memory leaks detected on %s (Please examine debug log)!",sceneText[map.sceneNumber]);
		bkString8to16(string16, buf);
		TextFX_vGrad(128,128,0,110,40,0);
		bdPrintFontLite(standardFont, string16, strlen(buf), -600.0f,0.0f, 128,128,128, 128, TextFX_escapeCallback, NULL);
		TextFX_resetColour();

		bdEndScene();

		// PP: finish timing the frame; calculate frames per second, frame length, etc.
		finishTimingFrame();

		if (controller1.f10DebounceChannel->value)
		{
			// take screen shot
			TakeScreenShot();
		}

#ifdef _TAZDEBUG// PP: bkHeapShowGraphical doesn't seem to exist in debug mode
#if BPLATFORM == GAMECUBE
		if (heapshow)
		{
			bkHeapShowGraphical(BALLGROUPS, 0.0f, standardFont);
			// IH: TEMPORARY: This will make it chug!
			bkPrintf( "********** Standard Memory Usage **********\n" );
			bkHeapStats();
			bkPrintf( "********** End Standard Memory Usage **********\n" );
//			bkHeapShow();
		}
#endif
#endif// PP: def _TAZDEBUG

		// PP: start timing the frame
		startTimingFrame();

		Flip(0, 0, 0, 128);
	}
}


/*  --------------------------------------------------------------------------------
	Function	: ApplyDecalZMask
	Purpose		: apply the specified actor instance as a z-mask for subsequent decal effects (eg. shadows)
	Parameters	: actor instance to use as a z-mask, maximum Y offset of decals to mask
	Returns		: 
	Info		: // PP: This draws the actor insatnce, with some z bias towards the viewer, to the z-buffer only.
					It makes its own sensible judgement about how far to zbias, based on the platform.
*/
void ApplyDecalZMask(TBActorInstance* const actorInstance, const float yOffset)
{
	SETPROJECTION(3D);

	bdSetIdentityObjectMatrix();

	// PP: z-test against the landscape
	SETZTEST(LESSTHAN);

	// PP: write to the zbuffer
	SETZWRITE(TRUE);

	// PP: but not the framebuffer
	DISABLE_FRAMEWRITE;

	// PP: mask at reduced depth
#if(BPLATFORM == PS2)
	bdSetZBias(-16000.0f);// PP: this will need tweaking - I have no idea what sort of value to use
#else// PP: if (BPLATFORM == PS2) else
	SETZBIAS(-0.4f);// PP: 0.4
#endif// PP: platform

	// PP: offset the zmask on the Y axis as specified
	actorInstance->position[Y]=yOffset;

	// PP: NEW TEST - prep the actor instance
	baPrepActorInstance(actorInstance,0);

	// PP: draw the actor instance
	baDrawActorInstance(actorInstance);

	bdSetZBias(0.0f);

	// PP: re-enable frame writing
	REENABLE_FRAMEWRITE;
}
