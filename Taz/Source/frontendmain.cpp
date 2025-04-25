// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : frontendmain.cpp
//   Purpose : functions to control the frontendTaz
// Component : 
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "main.h"
#include "actors.h"
#include "physics.h"
#include "control.h"
#include "status.h"
#include "animation.h"
#include "shadow.h"
#include "frontendmain.h"
#include "memcard.h"
#include "icon.h"
#include "util.h"				// PP: Utility functions
#include "mouse.h"
#include "sfx.h"
#include "pause.h"
#include "languagemain.h"
#include "files.h"
#include "tournament.h"
#include "demo.h"

FRONTENDCAMERAINFO	frontEndCameraInfo;

// PP: moved from frontend.cpp
// TP: 78k and not really used. static GLOBALSETTINGS		tempGlobalSettings;		// PP: temporary values of the global settings: this allows the values to be changed but with the options both to accept or to cancel

// PP: the value last update of the volume being adjusted
static bool		initialIncedentalsUpdate = true;
float	timetoDemo;

/*	--------------------------------------------------------------------------------
	Function 	: InitialiseFrontEnd
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

int InitialiseFrontEnd(void)
{
	float			tntProgress = 0.0f;
	
	TBActorNodeInstance	*node;

	gameStatus.multiplayer.numPlayers = 1;
	setNumScreens(1);

/*
 	LoadGame();

	while(MemCard_busy)
	{
		DrawLoadLoop();
	}
*/

#if BPLATFORM == PC
	InitMouse(&mouse);
	mouse.active = true;
#endif

	// CMD: because i'm paranoid, lets enable rumble ... just in case
	if((gameStatus.player[0].controller)&&(gameStatus.player[0].controller->rumbleController)) EnableRumble(gameStatus.player[0].controller);
	if((gameStatus.player[1].controller)&&(gameStatus.player[1].controller->rumbleController)) EnableRumble(gameStatus.player[1].controller);

	//put taz in correct state
	gameStatus.player[0].actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_NOGRAVITY;
	gameStatus.player[0].actorInstance->flags |= ACTORFLAG_NOCOLLISION;
	gameStatus.player[0].actorInstance->flags |= ACTORFLAG_NOVELOCITY;
	gameStatus.player[0].actorInstance->actorStatus->lastState = gameStatus.player[0].actorInstance->actorStatus->currentState;
	gameStatus.player[0].actorInstance->actorStatus->currentState = STATE_FRONTEND;
	SETVECTOR(gameStatus.player[0].actorInstance->actorBehaviour->physics.velocity, 0.0f, 0.0f, 0.0f, 1.0f);

	//initialise front end camera positions	
	for(int i=FECAM_MAIN;i<FECAM_NUMOFCAMERAS;i++)
	{
		AcceptFrontEndCamerasFromStore(frontEndCamStore[i].actorInstance,frontEndCamStore[i].position,frontEndCamStore[i].lookAtInstanceCRC);
	}
	InitCamera(&camera[0],frontEndCameraInfo.cameras[FECAM_MAIN][FECAMPOS],frontEndCameraInfo.cameras[FECAM_MAIN][FELOOKATPOS],FALSE);
	frontEndCameraInfo.cameraInfo = &camera[0];
	frontEndCameraInfo.cameraInfo->flags &= ~CAMERA_PADCONTROL;
	bmVectorCopy(frontEndCameraInfo.cameraInfo->pos, frontEndCameraInfo.cameras[FECAM_MAIN][FECAMPOS]);
	bmVectorCopy(frontEndCameraInfo.cameraInfo->destinationPos, frontEndCameraInfo.cameraInfo->pos);
	bmVectorCopy(frontEndCameraInfo.cameraInfo->lookAt, frontEndCameraInfo.cameras[FECAM_MAIN][FELOOKATPOS]);
	bmVectorCopy(frontEndCameraInfo.cameraInfo->destinationLookAt, frontEndCameraInfo.cameraInfo->lookAt);
	frontEndCameraInfo.currentPoint = FE_PT1;
	frontEndCameraInfo.time = 0.0f;
	frontEndCameraInfo.cameraInfo->posSmooth = 0.5f;
	frontEndCameraInfo.cameraInfo->lookAtSmooth= 0.5f;
	frontEndCameraInfo.destinationMenu = frontEndCameraInfo.currentMenu = FECAM_MAIN;

	SetCameraCollision(frontEndCameraInfo.cameraInfo, FALSE);
	gameStatus.chrisCam = FALSE;

	//initialise animations
	InitialiseFrontEndAnimationPaths();
	frontEndAnimInfo.animInfo.currentBlend = 0.0f;
	frontEndAnimInfo.animInfo.progress = 0.0f;
	frontEndAnimInfo.animInfo.smooth = 1.0f;
	frontEndAnimInfo.tntBlendInfo.currentBlend = 0.0f;
	frontEndAnimInfo.tntBlendInfo.progress = 0.0f;
	frontEndAnimInfo.tntBlendInfo.smooth = 1.0f;
	ChangeActorInstance(gameStatus.player[0].actorInstance,FindActorInActorList("extras\\tazdevil_frontend.obe"),225,NULL);
	CreateActorInstanceShadow(gameStatus.player[0].actorInstance,METERTOUNIT(0.7f),METERTOUNIT(1.5f));
	SetShadowNode(gameStatus.player[0].actorInstance,NULL,"snoutstem");

	//find startpos node on path
	node = baFindNode(frontEndAnimInfo.paths[MENU_STARTGAME][FEPATH_THERE]->actorInstance.rootNodeInstance,"taz_path_save01");
	if(node)
	{
		baGetNodesWorldPosition(&frontEndAnimInfo.paths[MENU_STARTGAME][FEPATH_THERE]->actorInstance,
									frontEndAnimInfo.paths[MENU_STARTGAME][FEPATH_THERE]->actorInstance.rootNodeInstance,
									gameStatus.player[0].actorInstance->actorInstance.position);
		bmQuatCopy(gameStatus.player[0].actorInstance->actorInstance.orientation,node->animState.orientation);
	}
//	bmQuatYRotation(tempQuat,RAD(180.0f));
//	bmQuatMultiply(gameStatus.player[0].actorInstance->actorInstance.orientation,tempQuat,gameStatus.player[0].actorInstance->actorInstance.orientation);
	ResetFrontEndAnimPath(gameStatus.player[0].actorInstance,"tazenter");
	ResetFrontEndAnimPath(frontEndAnimInfo.incedentals[FE_DECKCHAIR],"tazenter");
	ResetFrontEndAnimPath(frontEndAnimInfo.incedentals[FE_INITCAMERA],"tazenter");

	CreateFrontEndMenuBook();

	timetoDemo = gameStatus.gameTime += 40;

	// NH: If last scene was a multiplayer one...
	if ((map.lastScene == SCENE_DESTRUCTIONAQUA ||
		map.lastScene == SCENE_DESTRUCTIONCONSTRUCT ||
		map.lastScene == SCENE_DESTRUCTIONTASMANIA ||
		map.lastScene == SCENE_MUSEUMVEHICLERACE ||
		map.lastScene == SCENE_GRANDCVEHICLERACE ||
		map.lastScene == SCENE_DEPTSTRVEHICLERACE ||
		map.lastScene == SCENE_ICEDOMERACE ||
		map.lastScene == SCENE_GOLDMINERACE ||
		map.lastScene == SCENE_SAFARIRACE ||
		map.lastScene == SCENE_ZOOBOSS ||
		map.lastScene == SCENE_CITYBOSS ||
		map.lastScene == SCENE_WESTBOSS) && !gameStatus.multiplayer.versusCPU)
	{
		TWOPLAYERPAGE	*ptr = frontEndMenu.twoPlayerPage;

		// NH: Move Taz to multiplayer area
		node = baFindNode(frontEndAnimInfo.paths[MENU_TWOPLAYER][FEPATH_BACK]->actorInstance.rootNodeInstance,"taz_path_multiplayer02");
		if(node)
		{
			baGetNodesWorldPosition(&frontEndAnimInfo.paths[MENU_TWOPLAYER][FEPATH_BACK]->actorInstance,
								frontEndAnimInfo.paths[MENU_TWOPLAYER][FEPATH_BACK]->actorInstance.rootNodeInstance,
								gameStatus.player[0].actorInstance->actorInstance.position);
			bmQuatCopy(gameStatus.player[0].actorInstance->actorInstance.orientation,node->animState.orientation);
		}

		// TP: if last scene was a multiplayer game then the front end will NOT start in the intro state and the CheckMemCardOnStart function will not be called
		// TP: this means the createstartgamemenu function will also not be called
		CheckMemCardOnStart();
			
		// NH: Set up front end to start at multiplayer location
		frontEndAnimInfo.requestState = TP_IDLE;
		frontEndMenu.mainMenuPage->subState = MM_TWOPLAYER;
		frontEndAnimInfo.playingInitialAnim = FALSE;
		frontEndAnimInfo.delayOver = TRUE;
		frontEndAnimInfo.lastMenu = frontEndAnimInfo.targetMenu = MENU_TWOPLAYER;

		// NH: Set camera to be at and view the correct option
		bmVectorCopy(frontEndCameraInfo.cameraInfo->pos, frontEndCameraInfo.cameras[FECAM_TWOPLAYER][FECAMPOS]);
		bmVectorCopy(frontEndCameraInfo.cameraInfo->destinationPos, frontEndCameraInfo.cameraInfo->pos);
		bmVectorCopy(frontEndCameraInfo.cameraInfo->lookAt, frontEndCameraInfo.cameras[FECAM_TWOPLAYER][FELOOKATPOS]);
		bmVectorCopy(frontEndCameraInfo.cameraInfo->destinationLookAt, frontEndCameraInfo.cameraInfo->lookAt);

		// NH: Open Two Player Book
		frontEndMenu.frontEndMenuBook->open();
		frontEndMenu.frontEndMenuBook->gotoPage(menuNames[MENU_TWOPLAYER]);
		ptr->page->selectItem(ptr->destructionTb);
		ptr->infoSubBook->gotoPage("DESTRUCTION");
		ptr->controlSubBook->gotoPage("RETURN");
						
		// NH: Set submenu flag to show that we are NOT in a two player submenu
		twoPlayerSubMenu = false;				
		frontEndMenu.currentPage = MENU_TWOPLAYER;	
		
		gameStatus.multiplayer.versusCPU = true;
	}
	else
	{
		frontEndAnimInfo.playingInitialAnim = TRUE;

		frontEndAnimInfo.delayOver = FALSE;

		frontEndAnimInfo.lastMenu = frontEndAnimInfo.targetMenu = MENU_INTROMAIN;
	}

	return TRUE;
}

/*	--------------------------------------------------------------------------------
	Function 	: StartFrontEndFunctionality
	Purpose 	: Starts the front end after the initial anim has played
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void StartFrontEndFunctionality(void)
{
//	CreateFrontEndMenuBook();
	frontEndMenu.frontEndMenuBook->open(menuNames[MENU_INTROMAIN],false);
	frontEndCameraInfo.cameraInfo->posSmooth = 0.1f;
	frontEndCameraInfo.cameraInfo->lookAtSmooth= 0.1f;
}

/*	--------------------------------------------------------------------------------
	Function 	: PrepFrontEnd
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void PrepFrontEnd(void)
{
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateFrontEndDelay
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

int UpdateFrontEndDelay(void)
{
	if(gameStatus.gameTime > 0.0f)
	{
		PlayAnimationByName(gameStatus.player[0].actorInstance,"tazenter",1.0f,0,0,0.1f,NONE);
		PlayAnimationByName(frontEndAnimInfo.incedentals[FE_DECKCHAIR],"tazenter",1.0f,0,0,0.1f,NONE);
		PlayAnimationByName(frontEndAnimInfo.incedentals[FE_INITCAMERA],"tazenter",1.0f,0,0,0.1f,NONE);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateFrontEnd
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateFrontEnd(void)
{
	// PP: actions that can be performed when the front-end times out
	enum
	{
		TIMEOUT_DEMO,								// PP: rolling demo
		TIMEOUT_INTRO,								// PP: game intro cutscene
	};

	// PP: the action to be performed when the front-end times out (either rolling demo or intro cutscene)
	static bool		timeoutAction=TIMEOUT_DEMO;		// PP: start with the demo, because when the game starts we get the intro cutscene

	if(initialIncedentalsUpdate)
	{
		UpdateIncedentalsFrontEndAnimation();
		initialIncedentalsUpdate = false;
	}
	if(!frontEndAnimInfo.delayOver)
	{
		frontEndAnimInfo.delayOver = UpdateFrontEndDelay();         
	}
	else
	if(frontEndAnimInfo.playingInitialAnim)
	{
		UpdateIncedentalsFrontEndAnimation();
		if(!(frontEndAnimInfo.playingInitialAnim = UpdateFrontEndInitialAnim()))
		{
			RequestTazFrontEndAnimation("idle");
		}
	}
	else
	{
// PP: REMOUT: don't all the platforms need the attract cycle?	#if BPLATFORM == PS2
#ifndef NHIGGS
#define INFOGRAMES
#if((defined INFOGRAMES)||(defined PHIL)||(defined PHILFOGRAMES))
		if ((controller1.idleTime > 30)
			&& (gameStatus.gameTime > timetoDemo)
			&& (frontEndMenu.currentPage==MENU_MAIN || frontEndMenu.currentPage==MENU_INTROMAIN)// TP: Only allow demo to run if on main menu (or intro menu), not sub menus
			&& (!(MemCard_book.flags & BKFLAG_OPEN)))// PP: (a bit dirty, sorry) - don't time-out the front end while the memory card book is open (eg. on a corrupted save or full disk message)
		{
			// PP: Front-end has timed out

			// PP: REMOUT: surely it's nicer to alternate between the two?	if (bmRand()%2)
			// PP: Also, demo playback seeds the random number generation - could it not be a bit unsafe to use
			// PP: bmRandf here, with (theoretically at least) the same amount of time having passed since each seed?
			// PP: I certainly seemed to be getting the rolling demo every time on Xbox.
			// IH: TEMPORARY: Currently the demo mode is breaking on GameCube due to an Endian problem

			// TP: clear this flag incase the pad is removed on front end
			attractInterrupt = false;
#if BPLATFORM!=GAMECUBE
			if(timeoutAction ^= 1)
			{
				// JW: Goto Demo mode

				gameStatus.demoMode = 1;

				bkGenerateEvent("scenechange","_safari");
			}
			else
			{// JW: Show game intro*/
#else
			{
				timeoutAction = 0;
#endif
				//InitialiseLanguageSelect(0, 1);
			
				bkGenerateEvent("scenechange","_language");
			}

			controller1.idleTime = 0.0f;
		}
		else
#endif// PP: ((defined INFOGRAMES)||(defined PHIL)||(defined PHILFOGRAMES))
#endif // NH: #ifndef NHIGGS

// PP: REMOUT: #endif// PP: ps2
		{
			UpdateFrontEndMenuBook();
			//update camera
			if(frontEndCameraInfo.destinationMenu != frontEndCameraInfo.currentMenu)
			{
				if((frontEndCameraInfo.time += fTime/FE_CAMERATIME) > 1.0f) frontEndCameraInfo.time = 1.0f;
				UpdateFrontEndCamera();
			}
			//update paths
			// NH: TODO: Check paths and anims ALWAYS work.	
			if(frontEndAnimInfo.targetMenu != frontEndAnimInfo.lastMenu)
			{
				UpdateFrontEndAnimPaths();
			}
			else if((frontEndAnimInfo.targetMenu == MENU_SLOTONE)||(frontEndAnimInfo.targetMenu == MENU_SLOTTWO)
				||(frontEndAnimInfo.targetMenu == MENU_SLOTTHREE))
			{
				if(frontEndMenu.currentPage == MENU_MAIN)
				{
	//				ChooseFrontEndAnimationPath(MENU_MAIN);
				}
			}
//			else if(frontEndAnimInfo.lastMenu != frontEndMenu.currentPage)
//			{
//				ChooseFrontEndAnimationPath(frontEndMenu.currentPage);
//			}
			UpdateTazFrontEndAnimation();
			UpdateIncedentalsFrontEndAnimation();
		}
	}

	#ifdef DEBUG_ANIMS
		bkPrintf("fin.\n");
	#endif
}

/*	--------------------------------------------------------------------------------
	Function 	: DrawFrontEnd
	Purpose 	: Draws front end specific stuff
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void DrawFrontEnd(void)
{
}

/*	--------------------------------------------------------------------------------
	Function 	: DrawFrontEnd2
	Purpose 	: Draws front end specific stuff
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void DrawFrontEnd2(void)
{
	TBVector	screenPos;
	TBSavedRenderStates		save;

#define	DIST_FROM_SIDE		(((videoMode.xScreen - (videoMode.xScreen*videoMode.safeWidthPortion))/2.0f)+55.0f)
#define	DIST_FROM_TOP		(((videoMode.yScreen - (videoMode.yScreen*videoMode.safeHeightPortion))/2.0f)+55.0f)
#define HALF_SCREEN_WIDTH	(videoMode.xScreen/2.0f)
#define HALF_SCREEN_HEIGHT	(videoMode.yScreen/2.0f)

	if(frontEndAnimInfo.playingInitialAnim)
	{
	}
	else
	{
		if(frontEndMenu.gameOptionsPage->corner)
		{
			if((frontEndMenu.gameOptionsPage->subState == GO_SUB_MOVESCREEN)||
				(frontEndMenu.gameOptionsPage->subState == GO_WAITFORRESET))
			{
				bdSaveRenderStates(&save);

				bdSetIdentityObjectMatrix();
				bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
				bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
				bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
				bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
				bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
				bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
				bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);

				SETVECTOR(screenPos, -HALF_SCREEN_WIDTH+DIST_FROM_SIDE, HALF_SCREEN_HEIGHT-DIST_FROM_TOP,0.5f,1.0f);
				utilDraw2DSprite(frontEndMenu.gameOptionsPage->corner, screenPos, 100.0f, 100.0f, 0.0f);
				
				SETVECTOR(screenPos, HALF_SCREEN_WIDTH-DIST_FROM_SIDE, HALF_SCREEN_HEIGHT-DIST_FROM_TOP,0.5f,1.0f);
				utilDraw2DSprite(frontEndMenu.gameOptionsPage->corner, screenPos, 100.0f, 100.0f, RAD(90.0f));
				
				SETVECTOR(screenPos, HALF_SCREEN_WIDTH-DIST_FROM_SIDE, -HALF_SCREEN_HEIGHT+DIST_FROM_TOP,0.5f,1.0f);
				utilDraw2DSprite(frontEndMenu.gameOptionsPage->corner, screenPos, 100.0f, 100.0f, RAD(180.0f));

				SETVECTOR(screenPos, -HALF_SCREEN_WIDTH+DIST_FROM_SIDE, -HALF_SCREEN_HEIGHT+DIST_FROM_TOP,0.5f,1.0f);
				utilDraw2DSprite(frontEndMenu.gameOptionsPage->corner, screenPos, 100.0f, 100.0f, RAD(270.0f));
				bdRestoreRenderStates(&save);
			}
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: DrawFrontEnd
	Purpose 	: Draws front end specific stuff
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void FreeFrontEnd(void)
{
	int		i;

	// PP: if we are leaving the front-end from the PRESS START screen,
	// PP: return padCheckMode to PADCHECK_ALL, ie. stop waiting for player 1 to choose a pad.
	// PP: player 1 will be on the pad in the first port for the mean time.
	if(frontEndMenu.mainMenuPage->subState == MM_INTRO)
	{
		padCheckMode=PADCHECK_ALL;
	}
	
	// NH: Get rid of all stuff create for the tournament spinner
	CleanUpTournamentPage();

	SetCameraCollision(frontEndCameraInfo.cameraInfo, TRUE);
	gameStatus.chrisCam = TRUE;

	FreeFrontEndMenuBook();
	//free paths
	for(i=MENU_MAIN;i<MENU_NUMSUBMENUS;i++)
	{
		if(frontEndAnimInfo.paths[i][FEPATH_THERE])
		{
			FreeActorInstanceMemory(frontEndAnimInfo.paths[i][FEPATH_THERE],0);
		}
		if(frontEndAnimInfo.paths[i][FEPATH_BACK])
		{
			FreeActorInstanceMemory(frontEndAnimInfo.paths[i][FEPATH_BACK],0);
		}
	}
	//free incedentals
	for(i=0;i<MENU_NUMINCEDENTALS;i++)
	{
		if(frontEndAnimInfo.incedentals[i])
		{
			FreeActorInstanceMemory(frontEndAnimInfo.incedentals[i],0);
		}
	}
	//free save slot paths
	for(i=0;i<NUM_SAVE_SLOT_PATHS;i++)
	{
		if(frontEndAnimInfo.savePaths[i])
		{
			FreeActorInstanceMemory(frontEndAnimInfo.savePaths[i],0);
		}
	}
	FreeFrontEndCameraStore();
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateFrontEndInitialAnim
	Purpose 	: updates the anim which is played on entering the front end
	Parameters 	: 
	Returns 	: true if anim is stil playing
	Info 		:
*/

int UpdateFrontEndInitialAnim(void)
{
	TBActorNodeInstance		*node;

#ifdef ALLOW_CONTROLLERS_IN_ANY_PORTS
	if(OKBUTTON_ANY_PAD)
#else// PP: ifndef ALLOW_CONTROLLERS_IN_ANY_PORTS
	if(OKBUTTON)
#endif// PP: def ALLOW_CONTROLLERS_IN_ANY_PORTS
	{
		frontEndAnimInfo.playingInitialAnim = false;
	}

	if(frontEndAnimInfo.playingInitialAnim && CheckIfPlayingAnimation(gameStatus.player[0].actorInstance,"tazenter"))
	{
		node = baFindNode(frontEndAnimInfo.incedentals[FE_INITCAMERA]->actorInstance.rootNodeInstance, "camera pos - start");
		if(node) baGetNodesWorldPosition(&frontEndAnimInfo.incedentals[FE_INITCAMERA]->actorInstance,node,frontEndCameraInfo.cameraInfo->destinationPos);
		else return FALSE;

		node = baFindNode(frontEndAnimInfo.incedentals[FE_INITCAMERA]->actorInstance.rootNodeInstance, "camera lookat - start");
		if(node) baGetNodesWorldPosition(&frontEndAnimInfo.incedentals[FE_INITCAMERA]->actorInstance,node,frontEndCameraInfo.cameraInfo->destinationLookAt);
		else return FALSE;

		return TRUE;
	}
	else
	{
		StartFrontEndFunctionality();
		return FALSE;
	}
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
											CALLBACK FUNCTIONS for CONTROL PAGEITEMS

													moved from frontend.h
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: FrontEnd_vibrationICSetFunc
	Purpose 	: callback function for the vibration toggle-control
	Parameters 	: the state shown by the toggle-control (0/1)
	Returns 	: 
	Info 		: called only when the control toggles
*/
/*
void FrontEnd_vibrationICSetFunc(const int state)
{
	tempGlobalSettings.vibration=state?false:true;

	bkPrintf("FrontEnd_vibrationICSetFunc setting tempGlobalSettings.vibration to %s; gameStatus.globalSettings.vibration is %s\n", tempGlobalSettings.vibration?"ON":"OFF", gameStatus.globalSettings.vibration?"ON":"OFF");// PP: remember a state of 1 means the right-hand (off) icon is active!
}*/
