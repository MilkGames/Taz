// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : special.cpp
//   Purpose : One of special case code
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "display.h"
#include "actors.h"
#include "special.h"
#include "mapfile.h"
#include "scene.h"
#include "maths.h"
#include "main.h"
#include "genericai.h"
#include "alligator.h"
#include "status.h"
#include "control.h"
#include "animation.h"
#include "status.h"
#include "cameracase.h"
#include "phonebox.h"
#include "costumes.h"
#include "multiplayer.h"
#include "camera.h"
#include "floorpolisher.h"					// PP: floorpolisher functions
#include "floorpolisherwantedposter.h"
#include "frontendmain.h"
#include "fireworks.h"
#include "mechatweety.h"
#include "mtweetyscripts.h"
#include "files.h"
#include "textures.h"
#include "languagemain.h"
#include "tutorial.h"
#include "minecart.h"
#include "trigger.h"
#include "zapper.h"
#include "demo.h"
#include "glowaroundplinth.h"
#include "bossgamecontinue.h"
#include "mirrorball.h"
#include "polyeffects.h"
#include "prompt.h"
#include "endgamecredits.h"

#if(BPLATFORM == PS2)// PP: ps2 specific demo stuff
#include <libsceedemo.h>
#endif// PP: (BPLATFORM == PS2)

#ifdef PHIL
//#include "MonitorFX.h"					// PP: computer monitor effects
#endif // PP: def PHIL

#include "ZooBoss.h"						// PP: Zoo Boss - Gossamer vs. Taz in an Elephant Pong showdown
#include "cityboss.h"
#include "WestBoss.h"						// PP: Wile E West Boss - Yosemite Sam's ammo dump
#include "tazboss1.h"
#include "PaperBook.h"						// PP: 'Paper' book type, ie. one that looks and acts like a real physical book
//#include "atlassphere.h"

#define MP_NO	(0)
#define MP_YES	(1)
#define MP_TEST	(2)

static int allowMultiplayer = 0;
float	fireworkClock = 0.0f;

int32	showDebugInfo = FALSE;

TBVector	CageCentralPos;
int			curTutIP;
int			nxtTutIP;
int			inTutArea;
bool frankenMachineActive;
ACTORINSTANCE	*frankenMachine;

static SPLASHSCREEN		tempSplash;			// PP: temp splash-screen used to hold the paperbook background for the level
//ATLASSPHERE			*rollerBall;

BOOK	*tutInfBook;


char	*tutorialPointNames[] =
{
	{"tweety trigger"},
	{"tweety trigger01"},
	{"tweety trigger02"},
	{"tweety trigger03"},
};

char	*tutorialBoundBoxes[] =
{
	{"tutorialbox1"},
	{"tutorialbox02"},
	{"tutorialbox03"},
	{"tutorialbox04"},
};

/*	--------------------------------------------------------------------------------
	Function 	: InitialiseSpecial
	Purpose 	: Initialise the special items for a particular scene
	Parameters 	: scene number
	Returns 	: 
	Info 		: 0
*/
void InitialiseSpecial(int scene)
{
	int multiplayerGame = -1;
	ACTORINSTANCE	*actorInstance;
	TBVector		lightVec;
	EScene			index;
	char			userName[32];
	unsigned long int		length = 32;

#if BPLATFORM == PC

	if(GetUserName(userName, &length))
	{
		if(strcmpi(userName, "tadams") == 0)
		{
			user_jchilds = TRUE;
//			CStrikeSearchAndReplace("c:\\games\\sierra\\half-life\\cstrike\\config.cfg", "name", insultNames[bmRand()%5]);
//			CStrikeSearchAndReplace("c:\\program files\\sierra\\half-life\\cstrike\\config.cfg", "name", "Mike");
//			CStrikeSearchAndReplace("c:\\games\\sierra\\half-life\\cstrike\\config.cfg", "name", "Biryani Boy");
		}
	}
#endif

	// CMD: set plinth info to NULL just to avoid complications
	zooHubPlinthGlow = NULL;

#ifndef CONSUMERDEMO
	tazboss1Mirrorball = NULL;
	creditsInfo = NULL;
	InitialiseBossGameContinue();
#endif

	InitialiseNakedness((EScene)map.sceneNumber);
	StartHelpForMorons();
// CMD: lighting primitives not supported 
//	if(baCreateLight(&polyEffectsLight, 0))
//	{
//		baSetLightFlags(&polyEffectsLight, 0, 0);
//		bmVectorSet(lightVec, 0.0f, -1.0f, 0.0f, 1.0f);
//		baSetDirectionalLight(&polyEffectsLight, lightVec);
//		baSetLightColour(&polyEffectsLight,127,127,127);
//	}
//	else
//	{
//		bkPrintf("*** WARNING *** polyEffectsLight could not be initialised\n");
//	}

	// PP: initialise the temp splash-screen so's we can safely shut it down even if it's not used
	tempSplash.resource=NULL;

	index = (EScene)scene;

	switch(scene)
	{
	case SCENE_LANGUAGESELECT:
#ifdef CONSUMERDEMO
	#if(BPLATFORM == PS2)// PP: ps2 launch info specifies language
		switch (language)
		{
		case SCEE_DEMO_LANGUAGE_ENGLISH:	InitialiseLanguageSelect(0);	break;
		case SCEE_DEMO_LANGUAGE_FRENCH:		InitialiseLanguageSelect(2);	break;
		case SCEE_DEMO_LANGUAGE_GERMAN:		InitialiseLanguageSelect(3);	break;
		case SCEE_DEMO_LANGUAGE_SPANISH:	InitialiseLanguageSelect(1);	break;
		case SCEE_DEMO_LANGUAGE_ITALIAN:	InitialiseLanguageSelect(4);	break;
		}
	#else // NH: Would need to on other formats too! if(BPLATFORM == XBOX)			
			InitialiseLanguageSelect(BLANG_TO_FLAGNUM(GAMESTATUS::currentLanguage));
	#endif// PP: platform
#else
		InitialiseLanguageSelect(BLANG_TO_FLAGNUM(GAMESTATUS::currentLanguage));
#endif

		break;
#ifndef CONSUMERDEMO
	case SCENE_FRONTEND:
		InitialiseFrontEnd();
		break;
	case SCENE_ZOOBOSS:
		{
			if(!LoadBossGameContinue())
			{
				ASSERTM(NULL, "could not load boss game continue obe's");
			}

			// PP: initialisation for the boss game, before the intro cutscene has run
			ZooBoss_game.initPreCutscene();// PP: REMOUT: eg. set up shadows for cutscene

			// PP: load the paperbook's background
			LoadSplashScreen(&tempSplash, "boss.bmp", "splshscr");
		}
		break;

	case SCENE_CITYBOSS:
		{
			if(!LoadBossGameContinue())
			{
				ASSERTM(NULL, "could not load boss game continue obe's");
			}

			CityBoss_game.initPreCutscene();

			// PP: load the paperbook's background
			LoadSplashScreen(&tempSplash, "boss.bmp", "splshscr");
		}
		break;
	case SCENE_TAZPREBOSS:
		{
			if(!LoadBossGameContinue())
			{
				ASSERTM(NULL, "could not load boss game continue obe's");
			}

			tazBoss1Game.initPreCutscene();
	//		SetupLevelCostume(gameStatus.player[0].actorInstance);

			// PP: load the paperbook's background
			LoadSplashScreen(&tempSplash, "boss.bmp", "splshscr");
#if BPLATFORM!=GAMECUBE
			tazboss1Mirrorball = CreateMirrorballLights();
#endif
		}
		break;

	case SCENE_WESTBOSS:
		{
			if(!LoadBossGameContinue())
			{
				ASSERTM(NULL, "could not load boss game continue obe's");
			}

			// PP: initialisation for the boss game, before the intro cutscene has run
			WestBoss_game.initPreCutscene();

			// PP: load the paperbook's background
			LoadSplashScreen(&tempSplash, "boss.bmp", "splshscr");
		}
		break;
	case SCENE_DEPTSTR:
		//multiplayerGame = MPMODE_TIMEATTACK;
		break;
	case SCENE_TAZBOSS:
		// NH: Should this Reallllllly be needed?  Should get set to off in the LoadLOMFile function		map.fogState = 0;
		bdSetRenderState(BDRENDERSTATE_DISTANCEFOG, FALSE, FALSE);
		allowDebugSkip = FALSE;
		LoadWholePackage("mechatweety", 1);
		LoadActor("mechatweety.obe", "mechatweety", 0);
		LoadActor("mechatweetylight.obe", "mechatweety", 0);
		LoadActor("extras\\ninjathrowingcratetester.obe", "westhub", 0);
		LoadActor("extras\\ninjathrowingcratetester.obe", "tazboss2", 0);
		LoadActor("extras\\tweetyshield.obe", "tazboss2", 0);
//		LoadActor("extras\\rocket.obe", "tazboss2", 0);
		LoadActor("crashhelmet.obe", "tazboss2", 0);
		LoadActor("extras\\afootball_projectile.obe", "tazboss2", 0);
		LoadActor("extras\\boxingglove_projectile.obe", "tazboss2", 0);
		LoadActor("extras\\dart_projectile.obe", "tazboss2", 0);
		LoadActor("extras\\plunger_projectile.obe", "tazboss2", 0);
		LoadActor("extras\\rubberchicken_projectile.obe", "tazboss2", 0);
		LoadActor("extras\\shuttlecock_projectile.obe", "tazboss2", 0);
		LoadActor("extras\\teeth_projectile.obe", "tazboss2", 0);
		if(!LoadBossGameContinue())
		{
			ASSERTM(NULL, "could not load boss game continue obe's");
		}
//#ifdef _TAZDEBUG
//		LoadActor("extras\\plane.obe", "tazboss2", 0);
//#endif
		
		RandomiseButtonTimes();
		InitMangaMagnetExtraSystem();
		InitialiseMTweetyGenericInfo();
		for(actorInstance = map.invisibleInstances.head.next;actorInstance != &map.invisibleInstances.head;actorInstance = actorInstance->next)
		{
			if(actorInstance->special == SPECIALTYPE_MECHATWEETY)
			{
				if(actorInstance->special == SPECIALTYPE_MECHATWEETY)
				{
					actorInstance->special = (ESpecialType)-1;
					CreateActorInstanceMechaTweety(actorInstance);
					break;				//only one mecha tweety
				}
			}
		}
		// PP: load the paperbook's background
		LoadSplashScreen(&tempSplash, "boss.bmp", "splshscr");

		baDeleteExtraSystem(fireworksExtraSystem);
		InitFireworksExtraSystem(400);
		SetHearingRange(METERTOUNIT(25.0f), METERTOUNIT(500.0f));
		break;

	case SCENE_GOLDMINE:
		frankenMachineActive = false;
		frankenMachine = NULL;
		LoadActor("costume\\indy_flyinghatobject.obe",map.mapName);
		break;

	case SCENE_MUSEUM:
		{
			floorPolisherWantedPosterInfo = CreateFloorPolisherWantedPoster(VECM(4.8f, 7.8f, 17.4f), METERTOUNIT(4.0f), METERTOUNIT(4.0f), RAD(0.0f));

			InitFloorPolishers();				// PP: initialise stuff for any floor polishers

			// PP: end of project bodge: hardcode the two monitor screens in...

			MonitorFX_addLevelMonitor(	3833.985352f, 2421.609619f, -10543.109375f,
										0.244347f, 0.942479f);

			MonitorFX_addLevelMonitor(	3095.694824f, 2421.609619f, -10540.999023f,
										0.244349f, -0.942475f);

		}
		break;
	case SCENE_ZOOHUB:
		{
			ACTORINSTANCE	*GardenCent;

			LoadWholePackage("tweety", 1);
			LoadActor("tweety.obe", "tweety");

			GardenCent = FindActorInstanceInMap(tutorialPointNames[0], 0, &map);
			
			bmVectorCopy(CageCentralPos, GardenCent->actorInstance.position);
			
			CageCentralPos[X] += METERTOUNIT(2.0f);
			CageCentralPos[Y] += METERTOUNIT(2.0f);
			CageCentralPos[Z] += METERTOUNIT(2.0f);

			CreateTweety(CageCentralPos, bIdentityQuaternion);

			SetTweetyAOI(FindActorInstanceInMap(tutorialPointNames[0], 0, &map));

			curTutIP = -1;
			inTutArea = -1;

			PAGE	*pPage;
			TEXTBOX	*pText;

			tutInfBook = new BOOK();

			for (int i = 0; i < 4; i ++)
			{
				pPage = tutInfBook->addPage("page 1");

				pPage->insertItem(pText = new TEXTBOX(STR_ZOOHUB_TUTORIAL));

				pText->strcatf(" %d", i + 1);

				pText->setYAlign(PIFLAG_YALIGN_TOP);

				pText->setFontSize(2.0f);
			}

			initTutorial();

			// CMD: initialise stuff to do with effect around tutorial books
			CreateGlowAroundPlinthInZooHub();
			//CreateGlowAroundPoliceBoxInHub();

			// PP: load the paperbook's background
			LoadSplashScreen(&tempSplash, "guide.bmp", "splshscr");
		}
		
		break;
	case SCENE_AQUA:
		//LoadActor("extras\\rollerball.obe", map.mapName);

		//rollerBall = new ATLASSPHERE(NULL, "extras\\rollerball.obe");
		
		//SETVECTOR(rollerBall->pSphereActor->actorInstance.position, METERTOUNIT(-3.5f), METERTOUNIT(6.5f), METERTOUNIT(11.4f), 1.0f)
		break;
	case SCENE_TAZHUB:
		LoadActor("extras\\littlepiano.obe", "tazhub", 0);
		LoadActor("extras\\drumstickfull.obe", "tazhub", 0);
		LoadActor("extras\\anvil.obe", "tazhub", 0);
		break;

	case SCENE_MUSEUMVEHICLERACE:
		{
			InitFloorPolishers();				// PP: initialise stuff for any floor polishers

			// PP: end of project bodge: hardcode the two monitor screens in...

			MonitorFX_addLevelMonitor(	246.624023f, -1894.329712f, -1393.850586f,
										0.244347f, 0.942479f);

			MonitorFX_addLevelMonitor(	-491.666505f, -1894.329712f, -1393.850586f,
										0.244349f, -0.942475f);

			// PP: load the paperbook's background
			LoadSplashScreen(&tempSplash, "bonus.bmp", "splshscr");
		}// PP: don't break
#endif

	case SCENE_GRANDCVEHICLERACE:
	case SCENE_DEPTSTRVEHICLERACE:
		{
			multiplayerGame = MPMODE_RACE;
			allowMultiplayer = MP_YES;

			// PP: load the paperbook's background
			LoadSplashScreen(&tempSplash, "bonus.bmp", "splshscr");
		}
		break;

	case SCENE_DESTRUCTIONAQUA:
	case SCENE_DESTRUCTIONCONSTRUCT:
	case SCENE_DESTRUCTIONTASMANIA:
		{
			multiplayerGame = MPMODE_DESTRUCT;
			allowMultiplayer = MP_YES;

			// PP: load the paperbook's background
			LoadSplashScreen(&tempSplash, "bonus.bmp", "splshscr");
		}
		break;

	case SCENE_ICEDOMERACE:
	case SCENE_GOLDMINERACE:
	case SCENE_SAFARIRACE:
		{
			multiplayerGame = MPMODE_TIMEATTACK;
			allowMultiplayer = MP_YES;

			// PP: load the paperbook's background
			LoadSplashScreen(&tempSplash, "bonus.bmp", "splshscr");
		}
		break;
	}

	if(multiplayerGame == -1)
		allowMultiplayer = MP_NO;

	switch(allowMultiplayer)
	{
	case MP_NO:
		break;

	case MP_YES:
		mpInit(multiplayerGame, scene, gameStatus.player1, gameStatus.player2);
		break;

	case MP_TEST:
		mpInit(multiplayerGame, scene, gameStatus.player1, NULL);
		break;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: ShutdownSpecial
	Purpose 	: Shutdown any special items on the level
	Parameters 	: scene number
	Returns 	: 
	Info 		: 
*/
void ShutdownSpecial(int scene)
{
	switch(scene)
	{
	case SCENE_LANGUAGESELECT:
		FreeLanguageSelect();
		break;
#ifndef CONSUMERDEMO
	case SCENE_FRONTEND:
		FreeFrontEnd();
		break;
	case SCENE_ZOOHUB:
		DestroyTweety();
		KillTutorial();	
		FreeGlowAroundPlinth(zooHubPlinthGlow);

		SAFE_DELETE(tutInfBook);
		break;
	case SCENE_AQUA:
		//delete(rollerBall);
		break;
	case SCENE_ZOOBOSS:
		{
			ZooBoss_game.shutdown();
		}
		break;
	case SCENE_CITYBOSS:
		CityBoss_game.shutdown();
		break;
	case SCENE_WESTBOSS:
		{
			WestBoss_game.shutdown();
		}
		break;
	case SCENE_TAZPREBOSS:
		tazBoss1Game.shutdown();
#if BPLATFORM!=GAMECUBE
		baDeleteLight(&tazboss1Mirrorball->red);
		baDeleteLight(&tazboss1Mirrorball->green);
		baDeleteLight(&tazboss1Mirrorball->blue);
		baDeleteLight(&tazboss1Mirrorball->yellow);
		baDeleteLight(&tazboss1Mirrorball->bottom);
#endif
		SAFE_FREE(tazboss1Mirrorball);
		break;
	case SCENE_TAZBOSS:
		FreeMechaTweetySpecial();
		FreeMTweetyGenericInfo();
		FreeWholePackage("mechatweety");
		baDeleteExtraSystem(mangaMagnetExtraSystem);
		baDeleteExtraSystem(fireworksExtraSystem);
		InitFireworksExtraSystem();
		FreeEndGameCredits();
		// TP: just incase we gui out of boss
		SetHearingRange(METERTOUNIT(2.5f), METERTOUNIT(10.0f));
		break;
	case SCENE_MUSEUM:
	case SCENE_MUSEUMVEHICLERACE:
		{
			// PP: free floor polisher stuff
			ShutdownFloorPolishers();

			// PP: end of project bodge: delete the two monitor screens
			MonitorFX_deleteLevelMonitors();
		}
		break;
#endif
	}

	FreeHelpForMorons();

	// PP: free the paperbook background if it exists
	FreeSplashScreen(&tempSplash);

//	BookList_closeAndFree(pEndGameBook)
/* PP: REMOUT: OOD
	if (pEndGameBook)
	{
		BookList_closeAndFree(pEndGameBook);
	 	pEndGameBook = NULL;
	}*/

	BookList_closeAndFree(pEndMPGameStats);
/* PP: REMOUT: OOD
	if (pEndMPGameStats)
	{
		delete(pEndMPGameStats);
 		pEndMPGameStats = NULL;
	}*/

#ifndef CONSUMERDEMO
	FreeBossGameContinue();
#endif

	if(allowMultiplayer!=MP_NO)
		mpKill();
}

/*	--------------------------------------------------------------------------------
	Function 	: PrepSpecial
	Purpose 	: Prepare any special items on the level
	Parameters 	: scene number
	Returns 	: 
	Info 		: 
*/
void PrepSpecial(int scene)
{
	switch(scene)
	{
	case SCENE_LANGUAGESELECT:
		PrepLanguageSelect();
		break;
	case SCENE_FRONTEND:
#ifndef CONSUMERDEMO
		PrepFrontEnd();
#endif
		break;
	}

	if(allowMultiplayer!=MP_NO)
		mpPrep();

}

/*	--------------------------------------------------------------------------------
	Function 	: DrawSpecial
	Purpose 	: Draw any special items
	Parameters 	: scene number
	Returns 	: 
	Info 		: // PP: PLEASE NOTE: THIS GETS CALLED FOR SCREENS 1&2, THEN FOR SCREEN -1 (TO DRAW OVER BOTH SCREENS).
							BEFORE DRAWING, CHECK gameStatus.multiplayer.currentScreen
*/
void DrawSpecial(int scene, CAMERAINFO *cameraInfo)
{
	switch(scene)
	{
	case SCENE_LANGUAGESELECT:
		DrawLanguageSelect();
		break;
#ifndef CONSUMERDEMO
	case SCENE_FRONTEND:
		DrawFrontEnd();
		break;

	case SCENE_ZOOHUB:
		DrawTweety();
		break;

	case SCENE_MUSEUM:
	case SCENE_MUSEUMVEHICLERACE:
		{
			// PP: see function info
			if(gameStatus.multiplayer.currentScreen != -1)
			{
				// PP: end of project bodge: draw the two monitor screens
				MonitorFX_drawLevelMonitors();
			}
		}
		break;

	case SCENE_AQUA:
		//rollerBall->draw();
		break;

	case SCENE_CITYBOSS:
		{
			CityBoss_game.draw();// PP: draw City Boss solid effects
		}
		break;
	case SCENE_TAZPREBOSS:
		tazBoss1Game.draw();
		// CMD: draw mirrorball
		DrawMirrorball(&map.animatedInstances);
		break;

	case SCENE_WESTBOSS:
		{
			WestBoss_game.draw();// PP: draw West Boss solid effects
		}
		break;

	case SCENE_ZOOBOSS:
		{
			ZooBoss_game.draw();// PP: draw Zoo Boss solid effects
		}
		break;
	case SCENE_ICEDOME:
		break;
	case SCENE_TAZBOSS:
		break;
	case SCENE_GOLDMINE:
#ifndef INFOGRAMES
#ifdef _TAZDEBUG
		if(showDebugInfo)
		{
			DrawMinecartDebugInfo();
		}
#endif
#endif
		break;
#endif
	}

#if BPLATFORM == PS2
	if(showDebugInfo)
	{
		DrawRumbleDebugInfo(&controller1);
	}
#endif

	// PP: REMOUT - done in DrawSpecial2
/*	if(allowMultiplayer!=MP_NO)
		mpDraw(cameraInfo);*/
}

/*	--------------------------------------------------------------------------------
	Function 	: DrawSpecial
	Purpose 	: Draw any special items
	Parameters 	: scene number
	Returns 	: 
	Info 		: // PP: PLEASE NOTE: THIS GETS CALLED FOR SCREENS 1&2, THEN FOR SCREEN -1 (TO DRAW OVER BOTH SCREENS).
							BEFORE DRAWING, CHECK gameStatus.multiplayer.currentScreen
*/
void DrawSpecial2(int scene, CAMERAINFO *cameraInfo, int drawBeforeExtras)
{
#ifdef _TAZDEBUG
#ifndef INFOGRAMES
	// CMD: draw debug state text
	if(showDebugInfo)
	{
		DrawStateChangeDebugInfo(gameStatus.player[0].actorInstance);
	}
#endif
#endif

	switch(scene)
	{
/* PP: REMOUT: OOD, now handled by the PaperBook system itself

  case SCENE_ZOOHUB:
		{
			// PP: if we're in a tutorial...
			if (curTutIP != -1)
			{
				// PP: ...if the tutorial paperbook is ready to draw...
				if(PAPERBOOK::cameraActive())
				{
					// PP: ...draw the tutorial paperbook
					drawTutorial();
				}
			}
		}
		break;*/
#ifndef CONSUMERDEMO
	case SCENE_FRONTEND:
		if(!drawBeforeExtras) DrawFrontEnd2();
		break;
	case SCENE_CITYBOSS:
		if(!drawBeforeExtras)
		{
			CityBoss_game.draw2();// PP: draw City Boss alpha effects
		}
		break;
	case SCENE_GOLDMINE:
		if(drawBeforeExtras) DrawMinecartSparks(&map.animatedInstances);
		break;
	case SCENE_WESTBOSS:
		if(!drawBeforeExtras)
		{
			WestBoss_game.draw2();// PP: draw West Boss alpha effects
		}
		break;

	case SCENE_ZOOBOSS:
		if(!drawBeforeExtras)
		{
			// PP: HEY GUYS - THE PARTY'S IN HERE!!! WOOOOHOOOO!!!!! THERE'S AN ALPHA PAR-TAY ... AND EVERYONE'S INVI-TAY-DAH!!!
			ZooBoss_game.draw2();// PP: draw Zoo Boss alpha effects
		}
		break;
	case SCENE_TAZPREBOSS:
		if(!drawBeforeExtras) tazBoss1Game.draw2();
		break;
	case SCENE_TAZBOSS:
		if(drawBeforeExtras)
		{
			DrawMechaTweetySpecial(&characterInstances);
			DrawTazBossItemStuff(&map.animatedInstances);
		}
		break;
#endif
	case SCENE_DEPTSTR:
		if(drawBeforeExtras) DrawRapperBlast(gameStatus.player[0].actorInstance);
		break;
	}

	if(allowMultiplayer!=MP_NO)
		mpDraw(cameraInfo);

	if(drawBeforeExtras) DrawGlowAroundPlinth(zooHubPlinthGlow);
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateSpecial
	Purpose 	: Update any special items on the level
	Parameters 	: scene number
	Returns 	: 
	Info 		: 
*/
void UpdateSpecial(int scene)
{
	TBVector	tempVector;

	switch(scene)
	{
	case SCENE_LANGUAGESELECT:
		UpdateLanguageSelect();
		break;
#ifndef CONSUMERDEMO
	case SCENE_ZOOHUB:
		{
			float			distToTut;

			ACTORINSTANCE	*pTutPoint;

			TBVector	tempVector;
			TBMatrix	tempMatrix;

			if (curTutIP != -1)
			{
				// JW: Taz is next to a tutorial info point
				if (nxtTutIP != curTutIP)
				{
					// JW: Taz is next to a different info point than the next one

					UpdateTweetyNormal(fTime);
							
					SetTweetyAOI(FindActorInstanceInMap(tutorialPointNames[curTutIP], 0, &map));
					nxtTutIP = curTutIP;
				}
				else
				{
					// JW: Taz is at the next info point

					if (UpdateTweetyNormal(fTime) && gameStatus.player1->actorStatus->currentState != STATE_JUMP)
					{
						// JW: Tweety is hovering around

						// JW: Play tutorial

						if (!gameStatus.player1->actorStatus->tutorialBookFlag)
						{
							pTutPoint = FindActorInstanceInMap(tutorialPointNames[curTutIP], 0, &map);

							if (pTutPoint)
							{
								distToTut = bmVectorDistance(pTutPoint->actorInstance.position, gameStatus.player1->actorInstance.position);

								if ((distToTut < METERTOUNIT(1.0)) && (gameStatus.player1->actorStatus->currentState != STATE_SPIN))
								{
									EnterTutorialArea(curTutIP);

									CloseTutInfBook();

// PP: REMOUT: OOD, now handled by GAMESTATE_PAPERBOOK									gameStatus.controlSuspend = true;
									gameStatus.player1->actorStatus->tutorialBookFlag = 1;
								}
							}
						}

						if (gameStatus.player1->actorStatus->tutorialBookFlag == -1)
						{
							pTutPoint = FindActorInstanceInMap(tutorialPointNames[curTutIP], 0, &map);

							if (pTutPoint)
							{
								distToTut = bmVectorDistance(pTutPoint->actorInstance.position, gameStatus.player1->actorInstance.position);

								if (distToTut > METERTOUNIT(2.0))
									gameStatus.player1->actorStatus->tutorialBookFlag = 0;
							}
						}
					}
				}
			}
			else
			{
				DoTweetyIntro();
				UpdateTweetyNormal(fTime);

				int	i;

				// JW: See if Taz is near a tutorial point

				for (i = 0; i < 4; i++)
				{
					pTutPoint = FindActorInstanceInMap(tutorialPointNames[i], 0, &map);

					if (pTutPoint)
					{
						distToTut = bmVectorDistance(pTutPoint->actorInstance.position, gameStatus.player1->actorInstance.position);

						if (distToTut < METERTOUNIT(2.0))
						{
							curTutIP = i;

							if (curTutIP != nxtTutIP)
							{
								// JW: Move tweety behind player
								SETVECTOR(tempVector,0.0f,0.0f,METERTOUNIT(1.0f),1.0f);

								bmQuatToMatrix(tempMatrix,gameStatus.player1->actorInstance.orientation);
								bmMatMultiplyVector(tempMatrix,tempVector);

								bmVectorAdd(tempVector,gameStatus.player1->actorInstance.position,tempVector);

								tempVector[Y] += METERTOUNIT(2);

								TeleportTweety(tempVector);
							}
						}
					}
				}
			}

			if (inTutArea > -1)
			{
				if (!IsTriggerActive(bkCRC32((uchar *) tutorialBoundBoxes[inTutArea], strlen(tutorialBoundBoxes[inTutArea]), 0), &map.triggerInstances)) 
				{
					if (curTutIP != -1)
					{
						ExitTutorialArea(curTutIP);

						curTutIP = -1;
					}

					tutInfBook->close();

					inTutArea = -1;
				}
			}
			else
			{
				for (int i = 0; i < 4; i++)
				{
					if (IsTriggerActive(bkCRC32((uchar *) tutorialBoundBoxes[i], strlen(tutorialBoundBoxes[i]), 0), &map.triggerInstances))
					{
						inTutArea = i;

						tutInfBook->gotoPageNumber(i);

						tutInfBook->open();

						i = 4;
					}
				}
			}

			curTutIP = doTutorial(curTutIP);
		}
		break;
	case SCENE_FRONTEND:
		UpdateFrontEnd();
		break;
	case SCENE_ZOOBOSS:
		ZooBoss_game.update();
		break;
	case SCENE_AQUA:
		//if (gameStatus.player1->actorStatus->currentState != STATE_ATLASSPHERES)
	//	{
	//		float dist = bmVectorDistance(rollerBall->pSphereActor->actorInstance.position, gameStatus.player1->actorInstance.position);

	//		if (dist < METERTOUNIT(1.5f))
	//			rollerBall->setBoundToActor(gameStatus.player1);
	//	}
	//	else
	//	{
	//		rollerBall->beforeYouCallAnythingElseCallThis();
	//		rollerBall->calculateRoll();
	//	}
		break;
	case SCENE_CITYBOSS:
		{
			CityBoss_game.update();
		}
		break;
	case SCENE_TAZPREBOSS:
		tazBoss1Game.update();
		break;

	case SCENE_MUSEUM:
	case SCENE_MUSEUMVEHICLERACE:
		{
			// PP: end of project bodge: update the two monitor screens
			MonitorFX_updateLevelMonitors();
		}
		break;

	case SCENE_WESTBOSS:
		{
			WestBoss_game.update();
		}
		break;
	case SCENE_CITYHUB:
		if((fireworkClock += fTime) > 1.0f)
		{
			fireworkClock -= 1.0f;
			SETVECTOR(tempVector, METERTOUNIT(-1.3f), METERTOUNIT(19.0f), METERTOUNIT(55.8f), 1.0f);
			CreateFireworkBurst(tempVector, FIREWORK_RANDOMTYPE, 0,0,0,FIREWORKS_RANDOMCOLOUR);
		}
		break;
	case SCENE_TAZBOSS:
		UpdateMechaTweetyBossGameLogic(&characterInstances);
		UpdateAndChooseMechaTweetySequences(&characterInstances);
		UpdateMTweetyGenericInfo();
		break;
	case SCENE_GOLDMINE:
		// TP: keep eye on franken machine special case
		if (frankenMachineActive)
		{
			UpdateFrankenMachine();
		}
		break;
#endif
	case SCENE_DEPTSTR:
		// CMD: create and update the 'special' effect
		UpdateRapperBlast(gameStatus.player[0].actorInstance);
		break;
	}

	if(allowMultiplayer!=MP_NO)
		mpPoll();

	UpdateGlowAroundPlinth(zooHubPlinthGlow);
	UpdateHelpForMorons();

#ifndef CONSUMERDEMO
	if(bossGameContinueInfo)
	{
		UpdateBossGameContinue();
	}
#endif
}

// JW: Couple of functions to control the Tutorial Info Book if open another book on top of it.

void CloseTutInfBook()
{
	if (inTutArea > -1)
		tutInfBook->close();
}

void OpenTutInfBook()
{
	if (inTutArea > -1)
		tutInfBook->open();
}

void DrawStateChangeDebugInfo(ACTORINSTANCE *actorInstance)
{
	char			stringBuf[64];
	ushort			string16Buf[64];
	TBMatrix	matrix;

	bmMatScale(matrix, 0.5f, 0.5f, 0.5f);

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdSetObjectMatrix(matrix);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	sprintf(stringBuf, "Last State = %s", CharState[(int)actorInstance->actorStatus->lastState]);
	bkString8to16(string16Buf, stringBuf);
	bdPrintFontLite(standardFont, string16Buf, strlen(stringBuf), -500.0f, 350.0f, 127,127,127, 127, NULL, NULL);

	sprintf(stringBuf, "Current State = %s", CharState[(int)actorInstance->actorStatus->currentState]);
	bkString8to16(string16Buf, stringBuf);
	bdPrintFontLite(standardFont, string16Buf, strlen(stringBuf), -500.0f, 300.0f, 127,127,127, 127, NULL, NULL);

	if(actorInstance->actorStatus->stateRequest != -1)
	{
		sprintf(stringBuf, "State Request = %s", CharState[(int)actorInstance->actorStatus->stateRequest]);
		bkString8to16(string16Buf, stringBuf);
		bdPrintFontLite(standardFont, string16Buf, strlen(stringBuf), -500.0f, 250.0f, 127,127,127, 127, NULL, NULL);
	}

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
}

/*  --------------------------------------------------------------------------------
	Function	: StartFrankenMachine
	Purpose		: Start the franken machine cutscene with SPECIAL! taz anim
	Parameters	: name of machine
	Returns		: 
	Info		: 
*/

void StartFrankenMachine(char *machine)
{
	TBVector	tempVector;
	TBMatrix	tempMatrix;

	frankenMachine = FindActorInstanceInMap(machine, 0, &map);
	if (!frankenMachine) return;

	frankenMachineActive = true;

	// TP: get taz into move state
	if (RequestStateChange(gameStatus.player1, STATE_MOVE))
	{
		if (UpdateCharacterState(&gameStatus.player[0], true))
		{
			// TP: change state ok
			// TP: disable control of taz
			gameStatus.controlSuspend = true;

			// TP: turn off collision and velocity processing on taz to stop him from moving until cutscene is finished
			gameStatus.player1->flags |= ACTORFLAG_NOCOLLISION|ACTORFLAG_NOVELOCITY;

			// TP: set taz in correct position and hold
			bmVectorSet(tempVector, METERTOUNIT(-9.283f), METERTOUNIT(4.009f), METERTOUNIT(0.419f), 1.0f);
			bmQuatToMatrix(tempMatrix, frankenMachine->actorInstance.orientation);
			bmMatMultiplyVector(tempMatrix, tempVector);
			bmVectorAdd(gameStatus.player1->actorInstance.position, frankenMachine->actorInstance.position, tempVector);

			// TP: set correct orientation
			bmQuatCopy(gameStatus.player1->actorInstance.orientation,frankenMachine->actorInstance.orientation);
			baPrepActorInstance(&gameStatus.player1->actorInstance,0);

			// TP: play anim
			FlushAnimationQueue(gameStatus.player1);
			PlayAnimationByName(gameStatus.player1, "frankenmachine", 1.0f, 0, 0, 0.0f, NONE);

			// TP: set the shadow to follow taz root node
			SetShadowNode(gameStatus.player1,gameStatus.player1->actorInstance.rootNodeInstance->children,NULL);
		}
		else
		{
			ClearStateChangeRequest(gameStatus.player1);
		}
	}

}

/*  --------------------------------------------------------------------------------
	Function	: UpdateFrankenMachine
	Purpose		: Hold taz in the correct position and wait for the animation to finish
	Parameters	: 
	Returns		: 
	Info		: 
*/

void UpdateFrankenMachine()
{
	TBVector	tempVector;
	TBMatrix	tempMatrix;

	// TP: set taz in correct position and hold
	bmVectorSet(tempVector, METERTOUNIT(-9.283f), METERTOUNIT(4.009f), METERTOUNIT(0.419f), 1.0f);
	bmQuatToMatrix(tempMatrix, frankenMachine->actorInstance.orientation);
	bmMatMultiplyVector(tempMatrix, tempVector);
	bmVectorAdd(gameStatus.player1->actorInstance.position, frankenMachine->actorInstance.position, tempVector);

	// TP: set correct orientation
	bmQuatCopy(gameStatus.player1->actorInstance.orientation,frankenMachine->actorInstance.orientation);

	// TP: if anim finished then call EndFrankedMachine
	if (!CheckIfPlayingAnimation(gameStatus.player1, "frankenmachine"))
	{
		EndFrankenMachine();
	}
}


/*  --------------------------------------------------------------------------------
	Function	: EndFrankenMachine
	Purpose		: Resets taz and re-enables control
	Parameters	: 
	Returns		: 
	Info		: 
*/

void EndFrankenMachine()
{
	TBVector	tempVector;
	TBMatrix	tempMatrix;

	frankenMachineActive = false;

	gameStatus.player1->flags &= ~(ACTORFLAG_NOCOLLISION|ACTORFLAG_NOVELOCITY);
	gameStatus.controlSuspend = false;

	// TP: set taz in correct position and hold
	bmVectorSet(tempVector, METERTOUNIT(15.541f), METERTOUNIT(-3.743f), METERTOUNIT(-1.321f), 1.0f);
	bmQuatToMatrix(tempMatrix, frankenMachine->actorInstance.orientation);
	bmMatMultiplyVector(tempMatrix, tempVector);
	bmVectorAdd(gameStatus.player1->actorInstance.position, gameStatus.player1->actorInstance.position, tempVector);

	// TP: set correct orientation
	SetActorOrientation(gameStatus.player1, frankenMachine->actorInstance.orientation);

	// TP: stop taz from going straight into an idle
	controller1.idleTime = 0.0f;

	FlushAnimationQueue(gameStatus.player1);
	PlayAnimationByName(gameStatus.player1, "idle1", 1.0f, TRUE, 0, 0.0f, NONE);

	// TP: prep taz in new position
	AnimateActorInstance(gameStatus.player1);
	baPrepActorInstance(&gameStatus.player1->actorInstance, TRUE);

	// TP: set the shadow back to normal
	SetShadowNode(gameStatus.player1,NULL,NULL);
}
