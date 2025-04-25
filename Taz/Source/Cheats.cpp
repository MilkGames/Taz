// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Cheats.cpp
//   Purpose : Cheats
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"				// PP: Global stuff
#include "main.h"				// PP: Main stuff
#include "gui.h"				// PP: Menu stuff

// PP: for DIGITALWATCH...
#include "actors.h"				// PP: Theatrical stuff
#include "attach.h"				// PP: provides functions to allow objects to be attached to characters
#include "costumes.h"			// PP: Taz costume code
#include "MonitorFX.h"			// PP: Computer monitor effects
#include "characters.h"			// PP: character stuff

// PP: for DISABLESECURITYBOXES...
#include "securitybox.h"		// PP: Security box system

// PP: for SHADOWMAP2...
#include "shadow.h"				// PP: shadow stuff


#include "control.h"			// PP: provide basic input and control functions
#include "VideoFX.h"			// PP: Video effects - motion blur, defocus, whatever.  Top-of-the-Pops-TASTIC, Mate!
#include "status.h"				// PP: character status type stuff
#include "animation.h"			// PP: animation stuff

#include "VideoFX.h"			// PP: Video effects - motion blur, defocus, whatever.  Top-of-the-Pops-TASTIC, Mate!
#include "playerstats.h"

#include "Cheats.h"				// PP: Cheats

#include "display.h"			// PP: provide basic display setup and managment functions
#include "cutscene.h"			// TP: end of level cutscene functions
#include "mapfile.h"			// TP: The world in a cup
#include "cameracase.h"			// TP: In-game camera case code
#include "files.h"				// TP: file handling
#include "boss.h"
#include "westboss.h"
#include "events.h"
#include "cityboss.h"
#include "zooboss.h"
#include "multiplayer.h"
#include "special.h"

// PP: for AIRBRUSH...
#include "lights.h"				// PP: lighting code

// PP: for TIMEFREEZE...
#include "pause.h"				// PP: pause stuff


// PP: Cheat sounds (activate/deactivate)
#define CHEAT_ACTIVATE_SOUND	"tazeat_2 alt.wav"		// PP: "Mmm, tasty!"
#define CHEAT_DEACTIVATE_SOUND	"burp2.wav"				// PP: "BURP!"

#define CHEAT_MOTIONBLUR_LEVEL				0.4f	// PP: motion blur level for CHEAT_MOTIONBLUR


// PP: index of effect with exclusive use of the VFX_renderTarget, or VFX_NONE if it's free
extern EVideoEffect				VFX_renderTargetUser;


// PP: cheats that are available in the actual game
static uint32 CheatsImplemented=ALL_CHEATS;

// PP: cheats that are available from the gui
static uint32 guiCheatsImplemented=ALL_CHEATS;

// PP: macro to make a cheat unavailable in the actual game
// PP: pass it only the unique suffix of the cheat name, eg. INVISIBLE instead of CHEAT_INVISIBLE
#define UNIMPLEMENT_CHEAT_INGAME(_cheat)			(CheatsImplemented&= ~(1<<(CHEAT_##_cheat-1)))

// PP: macro to make a cheat both unavailable from the gui and unavailable in the actual game
// PP: pass it only the unique suffix of the cheat name, eg. INVISIBLE instead of CHEAT_INVISIBLE
#define UNIMPLEMENT_CHEAT(_cheat)					UNIMPLEMENT_CHEAT_INGAME(_cheat); \
													(guiCheatsImplemented&= ~(1<<(CHEAT_##_cheat-1)))

float g_testDOFNear=250;
float g_testDOFFar=100000;

#ifndef INGAME_CHEATS_MENU
// PP: #ifndef PHIL// PP: IFOUT: it's getting in the way

// PP: define CHEAT_ACTIVATED_MESSAGES to get the expandy messages when cheats are activated
#define CHEAT_ACTIVATED_MESSAGES

// PP: #endif// PP: ndef PHIL
#endif// PP: ndef INGAME_CHEATS_MENU

#ifdef CHEAT_ACTIVATED_MESSAGES
CHEATSSTRUCT cheatsStruct;
#endif// PP: def CHEAT_ACTIVATED_MESSAGES

//#define MARKETING

/*	--------------------------------------------------------------------------------
	Function 	: InitialiseCheats
	Purpose 	: initialise stuff for cheats
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void InitialiseCheats(void)
{
	BOOK			*bookPtr;
	PAGE			*pagePtr, *subPage;

#ifdef CHEAT_ACTIVATED_MESSAGES

	cheatsStruct.cheatsBook = bookPtr = new BOOK;
	cheatsStruct.cheatsBook->init();
	pagePtr = cheatsStruct.cheatsPage = bookPtr->addPage("Cheats Page");
	pagePtr->setNavigation(false);
	
	// NH: Add Cheat Text
	pagePtr->insertItem(cheatsStruct.cheatsTextTb = new TEXTBOX());
	cheatsStruct.cheatsTextTb->setStyle(TS_info)
		->setSelectable(false);

#endif// PP: def CHEAT_ACTIVATED_MESSAGES

	// PP: TEMP - award all cheats
	// PP: TODO: distribute these 'AwardCheat's throughout the game, yeah?
	// PP: NOTE: make sure that the cheat is 'available in-game' before awarding it!!!!!
	AwardCheat(CHEAT_OPEN_GALLERY);
	AwardCheat(CHEAT_OPEN_BOSS_GAME);
	AwardCheat(CHEAT_OPEN_BONUS_GAMES);
	AwardCheat(CHEAT_DIGITALWATCH);	
	AwardCheat(CHEAT_TURBO);
	AwardCheat(CHEAT_SLOWMO);
	AwardCheat(CHEAT_SUPERSLOWMO);
	AwardCheat(CHEAT_INVISIBLE);
	AwardCheat(CHEAT_INVINCIBLE);
	AwardCheat(CHEAT_COLOURSTEREOSCOPIC);
	AwardCheat(CHEAT_GREYSTEREOSCOPIC);
	AwardCheat(CHEAT_QWILSON);	
	AwardCheat(CHEAT_DISABLESECURITYBOXES);
	AwardCheat(CHEAT_MOTIONBLUR);
	AwardCheat(CHEAT_COSTUME);
	AwardCheat(CHEAT_TIMESLICENINJAKICK);
	AwardCheat(CHEAT_TIMESLICEJUMP);
	AwardCheat(CHEAT_INSOMNIA);
	AwardCheat(CHEAT_OPENLEVELS);
	AwardCheat(CHEAT_SHADOWMAP);
	AwardCheat(CHEAT_AIRBRUSH);
	AwardCheat(CHEAT_NEARFOCUS);
	AwardCheat(CHEAT_FARFOCUS);
	AwardCheat(CHEAT_DEFOCUS);
	AwardCheat(CHEAT_FILTERING);
	AwardCheat(CHEAT_TIMEFREEZE);
	AwardCheat(CHEAT_GLOVER_MODE);
	AwardCheat(CHEAT_SHOWDEBUGINFO);
	AwardCheat(CHEAT_TAZCAM);

	// PP: available from the gui but definietly won't be made available in-game...

	UNIMPLEMENT_CHEAT_INGAME(TIMEFREEZE);

	// PP: not YET available in-game (ie. I still have to make sure they're 100% safe)...

	UNIMPLEMENT_CHEAT_INGAME(OPEN_GALLERY);
	UNIMPLEMENT_CHEAT_INGAME(OPEN_BOSS_GAME);
	UNIMPLEMENT_CHEAT_INGAME(OPEN_BONUS_GAMES);
	UNIMPLEMENT_CHEAT_INGAME(DIGITALWATCH);	
// PP: UNIMPLEMENT_CHEAT_INGAME(TURBO);// PP: REMOUT: SAFE
// PP: UNIMPLEMENT_CHEAT_INGAME(SLOWMO);// PP: REMOUT: SAFE
// PP: UNIMPLEMENT_CHEAT_INGAME(SUPERSLOWMO);// PP: REMOUT: SAFE
	UNIMPLEMENT_CHEAT_INGAME(INVISIBLE);
	UNIMPLEMENT_CHEAT_INGAME(INVINCIBLE);
//	UNIMPLEMENT_CHEAT_INGAME(COLOURSTEREOSCOPIC);
	UNIMPLEMENT_CHEAT_INGAME(GREYSTEREOSCOPIC);
// PP: UNIMPLEMENT_CHEAT_INGAME(QWILSON);// PP: REMOUT: SAFE
// PP: 	UNIMPLEMENT_CHEAT_INGAME(DISABLESECURITYBOXES);// PP: REMOUT: WOW, SAFE (Tony musta fixed this)
	UNIMPLEMENT_CHEAT_INGAME(MOTIONBLUR);
	UNIMPLEMENT_CHEAT_INGAME(COSTUME);
	UNIMPLEMENT_CHEAT_INGAME(TIMESLICENINJAKICK);// PP: TODO: change this to a 360° timeslice because some languages call it '360°'; tone down motion blur on Xbox
	UNIMPLEMENT_CHEAT_INGAME(TIMESLICEJUMP);// PP: TODO: change this to a 360° timeslice because some languages call it '360°'; tone down motion blur on Xbox
// PP: UNIMPLEMENT_CHEAT_INGAME(INSOMNIA);// PP: REMOUT: SAFE
	UNIMPLEMENT_CHEAT_INGAME(OPENLEVELS);
	UNIMPLEMENT_CHEAT_INGAME(AIRBRUSH);
// PP: 	UNIMPLEMENT_CHEAT_INGAME(FILTERING);// PP: VERY SAFE, despite being Qwilson's idea
	UNIMPLEMENT_CHEAT_INGAME(GLOVER_MODE);// PP: doesn't seem to do anything
	UNIMPLEMENT_CHEAT_INGAME(SHOWDEBUGINFO);
	UNIMPLEMENT_CHEAT_INGAME(TAZCAM);

	// PP: not available at all (either ditched or temporarily buggered)...

	UNIMPLEMENT_CHEAT_INGAME(SHADOWMAP);	// PP: doesn't update properly as you toggle it + make sure all enemies have suitable radii
	UNIMPLEMENT_CHEAT(INVISIBLE);			// PP: this'll need some work, but the irony of it being greyed-out on the gui is kinda funny
	UNIMPLEMENT_CHEAT(GREYSTEREOSCOPIC);	// PP: BCREATERENDERTARGET_LOCKABLE creates targets that badly leak/fragment memory - should maybe talk to tech team at some point
	UNIMPLEMENT_CHEAT(COLOURSTEREOSCOPIC);	// PP: text is temporarily buggered - nothing serious I'm sure
	UNIMPLEMENT_CHEAT(NEARFOCUS);			// PP: ditched, for a number of good reasons
	UNIMPLEMENT_CHEAT(FARFOCUS);			// PP: ditched, for a number of good reasons
	UNIMPLEMENT_CHEAT(DEFOCUS);				// PP: ditched, for a number of good reasons

	// TP: temporarly removed
#ifdef MARKETING

	UNIMPLEMENT_CHEAT(QWILSON);
	UNIMPLEMENT_CHEAT(DIGITALWATCH);

	UNIMPLEMENT_CHEAT(SHADOWMAP);
	UNIMPLEMENT_CHEAT(TIMESLICENINJAKICK);

#endif// PP: marketing

	// PP: ****** CHEAT_QWILSON ******

	// PP: get a ptr to Qwilson
	QwilsonTexture=bkLoadTexture(NULL,"qwilson.bmp",0);

	// PP: and to the back of the goth mask
	QwilsonBackTexture=bkLoadTexture(NULL,"qwilsonmaskback.bmp",0);

	// PP: ****** CHEAT_TAZCAM ******

	SetCameraSmooth(&tazCam, 0.5f, 0.1f);
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateCheats
	Purpose 	: per-frame update of cheats
	Parameters 	: 
	Returns 	: 
	Info 		: may be temp
*/
void UpdateCheats(void)
{
#ifdef CHEAT_ACTIVATED_MESSAGES

	if(cheatsStruct.cheatTextShowing)
	{
		if (cheatsStruct.cheatsTextScale < 5.0f)
		{
			cheatsStruct.cheatsTextTb->setScale(cheatsStruct.cheatsTextScale);
			cheatsStruct.cheatsTextScale+=0.15f;
		}
		else
		{
			cheatsStruct.cheatsBook->close();
			cheatsStruct.cheatTextShowing = false;
		}
	}

#endif// PP: def CHEAT_ACTIVATED_MESSAGES

	if(CheatActive(CHEAT_MOTIONBLUR))
	{
		VFX_motionBlur(CHEAT_MOTIONBLUR_LEVEL);
	}

	// PP: ALL TEMP
	if(CheatActive((ECheat)CHEAT_NEARFOCUS))
	{
		// PP: temp - todo: put DOF stuff in VideoFX
//		bdSetDOFRange(g_testDOFNear, g_testDOFFar);
	}

	if(CheatActive((ECheat)CHEAT_FARFOCUS))
	{
		// PP: temp - todo: put DOF stuff in VideoFX
//		bdSetDOFRange(g_testDOFFar, g_testDOFNear);
	}

/*	if(CheatActive(CHEAT_COMPLETELEVEL))
	{
		if (!map.exitCutScene) DeactivateCheat(CHEAT_COMPLETELEVEL);
		else
		{
			if (map.exitCutScene->GetFlags() & CUTSCENEFLAG_AUTOLOAD)
			{
				if (map.exitCutScene->FinishedLoading())
				{
					DeactivateCheat(CHEAT_COMPLETELEVEL);
					map.exitCutScene->Setup();
					map.exitCutScene->Play(&camera[0]);
				}
			}
		}
	}*/
}


/*	--------------------------------------------------------------------------------
	Function 	: AwardCheat
	Purpose 	: award a cheat, make it available to the player
	Parameters 	: cheat to award
	Returns 	: 
	Info 		: 
*/
void AwardCheat(const ECheat cheat)
{
	gameStatus.cheatsAvailable |= (1<<(cheat-1));
}


/*	--------------------------------------------------------------------------------
	Function 	: CheatAvailable
	Purpose 	: determine if a particular cheat is available to the player
	Parameters 	: cheat to check for
	Returns 	: true if the cheat is available to the player, false if not
	Info 		: 
*/
bool CheatAvailable(const ECheat cheat)
{
	return ((gameStatus.cheatsAvailable & (1<<(cheat-1))) != 0);
}


/*	--------------------------------------------------------------------------------
	Function 	: ActivateCheat
	Purpose 	: activate a cheat, switch it on
	Parameters 	: index of cheat to activate
	Returns 	: 
	Info 		: 
*/
void ActivateCheat(const ECheat cheat)
{
	if(CheatActive(cheat))
	{
		// PP: OI! What's your game?!
		return;
	}

	gameStatus.globalSettings.cheatsActive |= (1<<(cheat-1));
	guiSetCheatTick(cheat);

	switch(cheat)
	{
	case CHEAT_NONE:
		{
			DeactivateAllCheats();
		}
		break;
	case CHEAT_DIGITALWATCH:
		{
			// PP: we've turned the watch on, so we've got to attach the watch model to player 1
			SetupCostume(gameStatus.player1, gameStatus.player1->characterInfo->costume);
		}
		break;
	case CHEAT_TURBO:
		{
			// PP: each speed cheat disables the others
			DeactivateCheat(CHEAT_SLOWMO);
			DeactivateCheat(CHEAT_SUPERSLOWMO);
		}
		break;
	case CHEAT_SLOWMO:
		{
			// PP: each speed cheat disables the others
			DeactivateCheat(CHEAT_TURBO);
			DeactivateCheat(CHEAT_SUPERSLOWMO);
		}
		break;
	case CHEAT_SUPERSLOWMO:
		{
			// PP: each speed cheat disables the others
			DeactivateCheat(CHEAT_TURBO);
			DeactivateCheat(CHEAT_SLOWMO);
		}
		break;
	case CHEAT_COLOURSTEREOSCOPIC:
		{
			// PP: stereo cheats are mut-ex
			DeactivateCheat(CHEAT_GREYSTEREOSCOPIC);

			// PP: activate the stereo vision effect
			VFX_setStereo(true, false, true);

			// PP: initialise eye index to zero
			VFX_stereoEye=0;
		}
		break;
	case CHEAT_GREYSTEREOSCOPIC:
		{
			// PP: stereo cheats are mut-ex
			DeactivateCheat(CHEAT_COLOURSTEREOSCOPIC);

			// PP: activate the stereo vision effect
			VFX_setStereo(true, true, true);

			// PP: initialise eye index to zero
			VFX_stereoEye=0;
		}
		break;
	case CHEAT_DISABLESECURITYBOXES:
		{
			// PP: shutdown all security boxes - sleeep, my pretties, sleeep.
			ShutdownSecurityBoxes(true);
		}
		break;
/*	case CHEAT_COMPLETELEVEL:
		{
			if (!map.exitCutScene)
			{
				// TP: just incase there is a cutscene and it has not been camera cased yet
				char	tempChar[256];
				sprintf(tempChar, "%s_ecs", map.mapName);
				map.exitCutScene = new CUTSCENE(tempChar,CUTSCENEFLAG_AUTOLOAD);
			}
			else
			{
				DeactivateCheat(cheat);
			}

			if (IN_BOSS_GAME)
			{
				switch(map.sceneNumber)
				{
				case SCENE_ZOOBOSS:
					ZooBoss_game.setState(BGSTATE_OUTRO);
					break;
				case SCENE_CITYBOSS:
					CityBoss_game.forceComplete();
					break;
				case SCENE_WESTBOSS:
					WestBoss_game.setState(BGSTATE_OUTRO);
					break;
				default:
					if (map.onLevelComplete)
					{
						SendTrigger(map.onLevelComplete);
					}
					else
					{
						LevelCompleted(gameStatus.player[0].actorInstance,LEVELCOMPLETE_NOSTATS);
					}
					break;
				}
			}
			else
			{
				if (map.onLevelComplete)
				{
					SendTrigger(map.onLevelComplete);
				}
				else
				{
					LevelCompleted(gameStatus.player[0].actorInstance);
				}
			}
		}
		break;
*/	case CHEAT_COSTUME:
		{
			SetupLevelCostume(gameStatus.player1);
		}
		break;
	case CHEAT_OPENLEVELS:
		{
			openLevels = true;
		}
		break;
	case CHEAT_SHADOWMAP:
		{
			VFX_activateEffect(VFX_SHADOWMAP, true);

			// PP: TEMP: if there are no shadow-casting lights in the scene, set up a default one now so's we can see some shadows...
			if(shadowLights.size == 0)
			{
				addShadowLight(camera[0].pos, SHADOWLIGHTTYPE_DIRECTIONAL, VEC(1.1f, 0.8f, 0.0f), SHADOWLIGHT_DEFAULT_COLOUR, SHADOWLIGHT_DEFAULT_SHADOW_DARKNESS, SHADOWLIGHT_INFINITE_RANGE, SHADOWLIGHT_DEFAULT_PLAYER_FLAGS);
			}
		}
		break;
	case CHEAT_AIRBRUSH:
		{
			// PP: set a more suitable ramp for the cel shading
/* PP: REMOUT: ramp doesn't seem to exist any more			SetLightRamp(RAMP_AIRBRUSH);

			// PP: apply the change to Taz
			EnableActorCartoonRenderMode(&gameStatus.player1->actorInstance);*/
		}
		break;		
	case CHEAT_NEARFOCUS:
		{
			if(VFX_focusPrepare())
			{
				// PP: focus cheats are mut-ex
				DeactivateCheat(CHEAT_FARFOCUS);
				DeactivateCheat(CHEAT_DEFOCUS);

				VFX_focusSetBlurFactor(3);
				VFX_focusNear(METERTOUNIT(30));
			}
			else
			{
				// PP: abort
				DeactivateCheat(cheat);
			}
		}
		break;
	case CHEAT_FARFOCUS:
		{
			if(VFX_focusPrepare())
			{
				// PP: focus cheats are mut-ex
				DeactivateCheat(CHEAT_NEARFOCUS);
				DeactivateCheat(CHEAT_DEFOCUS);

				VFX_focusSetBlurFactor(6);
				VFX_focusFar(METERTOUNIT(1));
			}
			else
			{
				// PP: abort
				DeactivateCheat(cheat);
			}
		}
		break;
	case CHEAT_DEFOCUS:
		{
			if(VFX_focusPrepare())
			{
				// PP: focus cheats are mut-ex
				DeactivateCheat(CHEAT_NEARFOCUS);
				DeactivateCheat(CHEAT_FARFOCUS);

				VFX_focusSetBlurFactor(3);
				VFX_focusBlur();
			}
			else
			{
				// PP: abort
				DeactivateCheat(cheat);
			}
		}
		break;
	case CHEAT_FILTERING:
		{
			gameStatus.gfxSettings.filterMode = BDTEXTUREFILTER_POINT;
		}
		break;
	case CHEAT_TIMEFREEZE:
		{
			setFlyCam(true);

			// PP: NEW TEST: get normal flycam control back
			gameStatus.navigatingMenus=false;

			// PP: freeze time
			SetGameState(&gameStatus, GAMESTATE_CUSTOMPAUSE, GAMESUBSTATE_NORMAL, int32(timeFreezePauseUpdateFunc), NULL);
		}
		break;
	case CHEAT_GLOVER_MODE:
		if((map.sceneNumber == SCENE_GHOSTTOWN)||(map.sceneNumber == SCENE_AQUA))
		{
			ChangeActorInstance(gameStatus.player[0].actorInstance,FindActorInActorList("extras\\ball.obe"),5,NULL);
			RequestStateChangePlus(gameStatus.player[0].actorInstance, STATE_BALL, BallStateChangeCallback);
		}
		break;
	case CHEAT_SHOWDEBUGINFO:
		showDebugInfo = TRUE;
		break;
	}

#ifdef CHEAT_ACTIVATED_MESSAGES
	// PP: display cheat-activated message
	cheatsStruct.cheatsTextTb->setText(CHEAT_NAME(cheat));
	cheatsStruct.cheatTextShowing = true;
	cheatsStruct.cheatsTextScale = 0.1f;
	cheatsStruct.cheatsBook->gotoPage("Cheats Page");
	cheatsStruct.cheatsBook->open();
#endif// PP: def CHEAT_ACTIVATED_MESSAGE
}


/*	--------------------------------------------------------------------------------
	Function 	: DeactivateCheat
	Purpose 	: deactivate a cheat, switch it off
	Parameters 	: index of cheat to deactivate
	Returns 	: true if the deactivation succeeded, false if the cheat wasn't on in the first place
	Info 		: 
*/
bool DeactivateCheat(const ECheat cheat)
{
	if(!CheatActive(cheat))
	{
		// PP: OI! What's your game?!
		return false;
	}

	gameStatus.globalSettings.cheatsActive &= ~(1<<(cheat-1));
	guiClearCheatTick(cheat);

	switch(cheat)
	{
	case CHEAT_OPENLEVELS:
		{
			openLevels = false;
		}
		break;

	case CHEAT_DIGITALWATCH:
		{
			if(MonitorFX_watch.handle != -1)
			{
				// PP: we've turned the watch off, so we've got to remove the watch model from player 1...
				RemoveObjectFromCharacter(gameStatus.player[0].actorInstance, MonitorFX_watch.handle);
			}

			// PP: ...and dectivate the watch display effect
			MonitorFX_watch.shutdown();
		}
		break;

	case CHEAT_COLOURSTEREOSCOPIC:
	case CHEAT_GREYSTEREOSCOPIC:
		{
			// PP: deactivate the stereo vision effect
			VFX_setStereo(false, false, true);// PP: SILLY: 2nd param is inconsequential if 3rd param is false!
		}
		break;

	case CHEAT_DISABLESECURITYBOXES:
		{
			// PP: shutdown all security boxes - sleeep, my pretties, sleeep.
			ReactivateSecurityBoxes();
		}
		break;

	case CHEAT_SHADOWMAP:
		{
			VFX_deactivateEffect(VFX_SHADOWMAP);
		}
		break;

	case CHEAT_COSTUME:
		{
			RemoveCostume(gameStatus.player1);
		}
		break;

	case CHEAT_AIRBRUSH:
		{
			// PP: reset ramp for cel shading
			SetLightRamp(LIGHTS_DEFAULT_RAMP);

			// PP: apply the change to Taz
			EnableActorCartoonRenderMode(&gameStatus.player1->actorInstance);
		}
		break;

	case CHEAT_NEARFOCUS:
	case CHEAT_FARFOCUS:
	case CHEAT_DEFOCUS:
		{
			VFX_focusReset();

			// PP: if no other focus cheats are active...
			if(!(CheatActive(CHEAT_NEARFOCUS)||CheatActive(CHEAT_FARFOCUS)||CheatActive(CHEAT_DEFOCUS)))
			{
				VFX_focusUnprepare();
			}
		}
		break;
	case CHEAT_FILTERING:
		{
			gameStatus.gfxSettings.filterMode = BDTEXTUREFILTER_LINEAR;
		}
		break;

	case CHEAT_TIMEFREEZE:
		{
			// PP: just in case
			ExitPauseMode();

			setFlyCam(false);
		}
		break;
	case CHEAT_GLOVER_MODE:
		if(map.sceneNumber == SCENE_GHOSTTOWN)
		{
			if(gameStatus.player[0].actorInstance->characterInfo->costume != -1)
			{
				SetupCostume(gameStatus.player[0].actorInstance, gameStatus.player[0].actorInstance->characterInfo->costume);
			}
			else
			{
				ChangeActorInstance(gameStatus.player[0].actorInstance,FindActorInActorList("tazdevil.obe"),5,NULL);
			}
			RequestStateChange(gameStatus.player[0].actorInstance, STATE_MOVE);
		}
		break;
	case CHEAT_SHOWDEBUGINFO:
		showDebugInfo = FALSE;
		break;
	}

	return true;
}


/*	--------------------------------------------------------------------------------
	Function 	: ToggleCheat
	Purpose 	: toggle a cheat on/off
	Parameters 	: ECheat to toggle, (opt/false) give the player feedback to confirm that the action has been taken
	Returns 	: 
	Info 		: 
*/
void ToggleCheat(const ECheat cheat, const bool playerNotification)
{
	if(CheatActive(cheat))
	{
		DeactivateCheat(cheat);

		if(playerNotification)
		{
			PlaySample(CHEAT_DEACTIVATE_SOUND);
		}
	}
	else
	{
		ActivateCheat(cheat);

		if(playerNotification)
		{
			PlaySample(CHEAT_ACTIVATE_SOUND);
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: DeactivateAllCheats
	Purpose 	: deactivate all cheats, switch them all off
	Parameters 	: (opt/false) give the player feedback to confirm that the action has been taken
	Returns 	: 
	Info 		: All Special dishes are served with Pilau Rice or Naan
*/
void DeactivateAllCheats(const bool playerNotification)
{
	for(int cheat=CHEAT_NONE+1; cheat<NUM_CHEATS; cheat++)
	{
		DeactivateCheat((ECheat)cheat);
	}

	if(playerNotification)
	{
		PlaySample(CHEAT_DEACTIVATE_SOUND);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: CheatImplemented
	Purpose 	: find out if a cheat is earnable by the player
	Parameters 	: cheat to check for
	Returns 	: 
	Info 		: 
*/
bool CheatImplemented(const ECheat cheat)
{
	if(cheat == CHEAT_NONE) return true;

	return (((1<<(cheat-1)) & CheatsImplemented) != 0);
}


/*	--------------------------------------------------------------------------------
	Function 	: guiCheatImplemented
	Purpose 	: find out if a cheat is available from the gui
	Parameters 	: cheat to check for
	Returns 	: 
	Info 		: 
*/
bool guiCheatImplemented(const ECheat cheat)
{
	if(cheat == CHEAT_NONE) return true;

	return (((1<<(cheat-1)) & guiCheatsImplemented) != 0);
}


/*	--------------------------------------------------------------------------------
	Function 	: checkCheats
	Purpose 	: checks to see if a cheat was entered in as a player name
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void checkCheats(PLAYERSTATS *stats)
{
	// PP: NOTE: I changed this so that the codes TOGGLE
	// PP: the cheats rather than ACTIVATE them.
	// PP: Otherwise you'd be stuck with it...

	// PP: NOTE: I also changed this function to (if necessary) ignore the stats param and use the name and icon of the current slot
	// PP: I hope you don't mind.
	// PP: This was so's we could checkCheats without being in thename entry screen - which we really needed to be able to do...
	SAVESLOT*	slot;
	char*		letters;
	uint32		iconNumber;

	slot= &(gameStatus.globalSettings.saveInfo.saveSlot[2]);// PP: 2: we can only use the third cave to enter cheats

	if(slot->inUse)
	{
		letters=slot->lastName;
		iconNumber=slot->lastIcon;
	}
	else
	{
		letters=stats->letters;
		iconNumber=stats->iconNumber;
	}

	// PP: Disable all cheats
	if (letters[0] == '*'
		&& letters[1] == 'N'
		&& letters[2] == 'O'
		&& iconNumber == 0)				// PP: Taz 1 (default)
	{
		DeactivateAllCheats(true);// PP: true = sound or that
	}
	// NH: Open All Levels
	else if (letters[0] == '#'
		&& letters[1] == 'O'
		&& letters[2] == 'P'
		&& iconNumber == 5) // NH: Marvin
	{
		ToggleCheat(CHEAT_OPENLEVELS, true);// PP: true = sound or that
	}
	// NH: Disable Whackaboxes
	else if (letters[0] == '!'
		&& letters[1] == 'W'
		&& letters[2] == 'B'
		&& iconNumber == 1) // NH: Taz 2
	{
		ToggleCheat(CHEAT_DISABLESECURITYBOXES, true);// PP: true = sound or that
	}
	// NH: Open All Gallerys
	else if (letters[0] == '.'
		&& letters[1] == 'R'
		&& letters[2] == 'T'
		&& iconNumber == 12) // NH: Tweety
	{
		ToggleCheat(CHEAT_OPEN_GALLERY, true);// PP: true = sound or that
	}
	// NH: Open 2 Player Bosses
	else if (letters[0] == '*'
		&& letters[1] == 'J'
		&& letters[2] == 'C'
		&& iconNumber == 10) // NH: Red Guy
	{
		ToggleCheat(CHEAT_OPEN_BOSS_GAME, true);// PP: true = sound or that
	}
	// NH: Open All Bonus Games
	else if (letters[0] == '?'
		&& letters[1] == 'B'
		&& letters[2] == 'N'
		&& iconNumber == 8) // NH: Daffy 1
	{
		ToggleCheat(CHEAT_OPEN_BONUS_GAMES, true);// PP: true = sound or that
	}
	// PP: Digital Watch
	else if (letters[0] == '?'
		&& letters[1] == 'D'
		&& letters[2] == 'W'
		&& iconNumber == 14)				// PP: Granny
	{
		ToggleCheat(CHEAT_DIGITALWATCH, true);// PP: true = sound or that
	}
	// PP: Cyan-Red Anaglyphic Stereoscopy
	else if (letters[0] == 'B'				// PP: "blue"
		&& letters[1] == '!'
		&& letters[2] == 'R'				// PP: red
		&& iconNumber == 13)				// PP: Speedy Gonzales
	{
		// PP: DIRTY FRIG
		if(VFX_stereoAnaglyphType != ANAGLYPH_CYAN_RED)
		{
			// PP: in case the other stereo cheat is already on
			DeactivateCheat(CHEAT_COLOURSTEREOSCOPIC);
		}

		// PP: set the cyan-red anaglyph type
		VFX_stereoAnaglyphType=ANAGLYPH_CYAN_RED;

		if(COLOUR_CHANNEL_MASKING_SUPPORTED)
		{
			ToggleCheat(CHEAT_COLOURSTEREOSCOPIC, true);// PP: true = sound or that
		}
		else
		{
			// PP: sorry, no stereoscopy if you have a shit graphics card - too much hassle for me in the time remaining (none)
			PlaySample("pickupbadfood1.wav");// PP: "uh-oh!"
		}
	}
	// PP: Red-Cyan Anaglyphic Stereoscopy
	else if (letters[0] == 'R'				// PP: red
		&& letters[1] == '!'
		&& letters[2] == 'B'				// PP: "blue"
		&& iconNumber == 13)				// PP: Speedy Gonzales
	{
		// PP: DIRTY FRIG
		if(VFX_stereoAnaglyphType != ANAGLYPH_RED_CYAN)
		{
			// PP: in case the other stereo cheat is already on
			DeactivateCheat(CHEAT_COLOURSTEREOSCOPIC);
		}

		// PP: set the red-cyan anaglyph type
		VFX_stereoAnaglyphType=ANAGLYPH_RED_CYAN;

		// PP: duplicated from above.  Do we care? ...

		if(COLOUR_CHANNEL_MASKING_SUPPORTED)
		{
			ToggleCheat(CHEAT_COLOURSTEREOSCOPIC, true);// PP: true = sound or that
		}
		else
		{
			// PP: sorry, no stereoscopy if you have a shit graphics card - too much hassle for me in the time remaining (none)
			PlaySample("pickupbadfood1.wav");// PP: "uh-oh!"
		}
	}
/*

#ifdef CHEAT_ACTIVATED_MESSAGES

	// NH: GF (Please Don't remove!)
	else if (letters[0] == '?'
		&& letters[1] == 'G'
		&& letters[2] == 'F'
		&& iconNumber == 12) // NH: Tweety
	{
		cheatsStruct.cheatsTextTb->setText("S.A.R");
		cheatsStruct.cheatTextShowing = true;
		cheatsStruct.cheatsTextScale = 0.1f;
		cheatsStruct.cheatsBook->gotoPage("Cheats Page");
		cheatsStruct.cheatsBook->open();
	}

#endif// PP: def CHEAT_ACTIVATED_MESSAGES

*/
}

/*	--------------------------------------------------------------------------------
	Function 	: ShutdownCheats
	Purpose 	: shutdown stuff for cheats
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void ShutdownCheats(void)
{
#ifdef CHEAT_ACTIVATED_MESSAGES

	delete cheatsStruct.cheatsBook;
	cheatsStruct.cheatsBook = NULL;

#endif// PP: def CHEAT_ACTIVATED_MESSAGES
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												stuff for CHEAT_DIGITALWATCH

									Taz wears his digital watch that tells the system time
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: #define WATCHLOOK_ALTER_FOV

bool			watchLook=false;		// PP: just in case
static bool		watchLookLatch=false;
static float	watchLookTimer;
static float	watchLookProgress;
float			watchLookFOV;

#include "characters.h"			// PP: character stuffffffffffffffff...fffffff.....fff.....ffff............f.................FFFFFFFFFFFFFFFFFFFFFFFFFF!!!!!!!!!!	// PP: SHE'S GONNA BLOW!  FOR GOD'S SAKE STRAP YOURSELVES DOWN!
#include "display.h"			// PP: provide basic display setup and managment functions

#define WATCH_ZOOMIN_TIME		5.0f
#define WATCH_ZOOMOUT_TIME		0.0f
#define WATCH_ZOOMIN_FOV		RAD(5.0f)//10
#define WATCH_LOOKAT_TILT		(-1.6f)

/*	--------------------------------------------------------------------------------
	Function 	: LookAtWatch
	Purpose 	: point the camera at the watch, close enough that you can read the time off it
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: this is real dirty, the way it sets stuff up each frame, but it seems to be the norm and by God it saves hassle
*/
void LookAtWatch(void)
{
	TBVector	rotVec, tempVec, camPos;
	float		yAng;

	SETVECTOR(camPos, 0,METERTOUNIT(1),METERTOUNIT(1.5f),1.0f);// PP: test

	bmQuatToRotation(rotVec, gameStatus.player1->actorInstance.orientation);
	yAng=Aabs(rotVec[W]*rotVec[Y]);
	yAng=PI-yAng;
	yAng-=0.8f;
	bmVectorRotateY(tempVec, camPos, yAng);

	bmVectorAdd(camPos, tempVec, gameStatus.player1->actorInstance.position);

	watchLookProgress=MIN(1.0f, watchLookTimer/(float)WATCH_ZOOMIN_TIME);

	SetCameraSmooth(&camera[0], 0.02f, watchLookProgress);
	camera[0].tilt=watchLookProgress*WATCH_LOOKAT_TILT;

	SetCameraPositionDestination(&camera[0], camPos);
	SetCameraPositionOffset(&camera[0]);
	SetCameraLookAtDestination(&camera[0], MonitorFX_watch.pos);//gameStatus.player1->characterInfo->attachedObjects.actualWorldPosition[MonitorFX_watch.handle]);
	SetCameraLookAtOffset(&camera[0]);

	// PP: NEW TEST
	watchLookFOV=NORMAL_INGAME_FOV-(watchLookProgress*(NORMAL_INGAME_FOV-WATCH_ZOOMIN_FOV));
	setFOV(watchLookFOV);

	watchLookTimer+=fTime;
}


/*	--------------------------------------------------------------------------------
	Function 	: CheckForWatchLook
	Purpose 	: look at pad input to determine whether or not the player is
					trying to look at the watch
	Parameters 	: 
	Returns 	: 
	Info 		: This function sets 'watchLook'.
					check before calling that CHEAT_DIGITALWATCH is active
*/
void CheckForWatchLook(void)
{
	// PP: we've already checked that CHEAT_DIGITALWATCH is enabled

	bool	newWatchLook;

	// PP: hold down L1 and L2, then click the left stick

	if(watchLookLatch)
	{
 		newWatchLook=watchLookLatch=(controller1.l1Channel->value
					&&controller1.r1Channel->value
					&&controller1.l3Channel->value);
	}
	else
	{
		newWatchLook=watchLookLatch=(controller1.l1Channel->value
			&&controller1.r1Channel->value
			&&controller1.l3DebounceChannel->value);
	}

	if(newWatchLook)
	{
		if(!watchLook)
		{
			// PP: right then, set up the camera to point at the watch...

			watchLookTimer=0.0f;
			watchLookProgress=0.0f;

			FlushAnimationQueue(gameStatus.player1);
			PlayAnimationByName(gameStatus.player1, "idle1", 1.0f,1,0,0.2f,NONE);

#ifdef WATCHLOOK_ALTER_FOV
			SetFadeFieldOfView(WATCH_ZOOMIN_FOV, WATCH_ZOOMIN_TIME);
#endif// PP: def WATCHLOOK_ALTER_FOV
		}
	}
	else
	{
		if(watchLook)
		{
#ifdef WATCHLOOK_ALTER_FOV
			// PP: zoom back out again
			SetFadeFieldOfView(NORMAL_INGAME_FOV);
#endif// PP: def WATCHLOOK_ALTER_FOV

			// PP: NEW TEST
			setFOV(NORMAL_INGAME_FOV);

			camera[0].tilt=0.0f;

			// PP: reset camera smoothing // PP: TODO!!!!!: DEFINE THESE FOR GOD'S SAKE!!!
			SetCameraSmooth(&camera[0], defaultPosSmooth, defaultLookatSmooth);
		}
	}

	watchLook=newWatchLook;
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												stuff for CHEAT_QWILSON

									Pictures of wilson appear in interesting places.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: ptr to Qwilson's pic
TBTexture* QwilsonTexture=NULL;

// PP: and to the back of the goth mask
TBTexture* QwilsonBackTexture=NULL;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												stuff for CHEAT_TIMEFREEZE

									Time freezes completely, but the flycam can still be used.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: timeFreezePauseUpdateFunc
	Purpose 	: pause update function for the time-freeze cheat
	Parameters 	: parameter is not used
	Returns 	: 
	Info 		: updates fly cam
*/
void timeFreezePauseUpdateFunc(const int32 parameter)
{
	updateFlyCam();
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												stuff for CHEAT_TAZCAM

											Attaches the camera to Taz's heed.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


CAMERAINFO				tazCam;				// PP: the camera attached to Taz's heed


/*	--------------------------------------------------------------------------------
	Function 	: updateTazCam
	Purpose 	: update the position and orientation of the Taz Cam
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: gets called if CheatActive(CHEAT_TAZCAM)
*/
void updateTazCam(void)
{
	// PP: TEMP TEST
	setFOV(RAD(80));

#define TAZCAM_TILT			0.2f		// PP: tilt of camera

	VEC						headPos;
	VEC						lookat;
	TBActorNodeInstance*	headBone;
	TBMatrix				rotMat;
	TBMatrix				tempMat;
	TBMatrix				finalMat;

	ACTORINSTANCE*			actorInstance;

	actorInstance=gameStatus.player1;

	headBone=baFindNode(actorInstance->actorInstance.rootNodeInstance,"snoutstem");

	if(headBone != NULL)
	{
		baGetNodesWorldPosition(&(actorInstance->actorInstance), headBone, headPos);
		headPos.moveY(80.0f);

	/*	bmMatXRotation(rotMat, TAZCAM_TILT);

		bmMatMultiply(tempMat, headBone->nodeToLocalWorld, rotMat);

		bmMatMultiply(finalMat, actorInstance->actorInstance.objectToWorld, tempMat);
		
		// PP: bdSetObjectMatrix(finalMat);

		headPos=ZVEC*finalMat;

		lookat=(VECM(0,0,-1)*finalMat);

		SetCameraPositionDestination(&tazCam, headPos);
		SetCameraLookAtDestination(&tazCam, lookat);*/
	}

	TBVector		rotVec;
	TBQuaternion	rotQuat;

	bmQuatCopy(rotQuat, actorInstance->actorInstance.orientation);

	utilNormaliseQuaternion(rotQuat);

	bmQuatToRotation(rotVec, rotQuat);

	float yang=Aabs(rotVec[W]*rotVec[Y]);

//	headPos=VEC(actorInstance->actorInstance.position).moveY(METERTOUNIT(1));
	lookat=headPos+(VECM(0,0,5).rotateY(PI-yang));

	SetCameraPositionDestination(&tazCam, headPos);
	SetCameraLookAtDestination(&tazCam, lookat);

	UpdateCamera(&tazCam);
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												NAUGHTY STUFF (teehee!)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: SetWallpaper
	Purpose 	: Set the specified user's wallpaper
	Parameters 	: user name string eg "ahodgson", wallpaper file eg "\\\\ppalmer\\c\\box_01.bmp", (opt/WALLPAPER_STRETCH)wallpaper mode (_STRETCH/_TILE/_CENTRE)
	Returns 	: 
	Info 		: this is probably set up for Win 2000, so check those paths first
*/
void SetWallpaper(const char* const userName, const char* const wallpaper, const EWallpaperMode mode)
{
#if(BPLATFORM == PC)

	char				nameBuffer[32];
	unsigned long		nameSize=32;

	// PP: check for matching user name...

	GetUserName(nameBuffer, &nameSize);

	if(stricmp(nameBuffer, userName)==0)
	{
		// PP: user name matches - set wallpaper...

		HKEY	hKey;
		DWORD	dwDisposition;
		int32	tile;
		int32	style;

		tile=((mode == WALLPAPER_TILE)?1:0);
		style=((mode == WALLPAPER_STRETCH)?2:0);

#define SET_WALLPAPER(_key, _value) \
		/* Open the registry for writing */ \
		if( ERROR_SUCCESS == RegCreateKeyEx(	_key, _value, 0, "", \
		                                 		REG_OPTION_NON_VOLATILE, \
												KEY_ALL_ACCESS, NULL, &hKey, \
												&dwDisposition ) ) \
		{ \
			/* write the data to the registry */ \
			RegSetValueEx( hKey, "Wallpaper",  0, REG_SZ, (const unsigned char*)wallpaper, strlen(wallpaper)+1); \
			RegSetValueEx( hKey, "TileWallpaper", 0, REG_DWORD, (const unsigned char*)(&tile), sizeof(tile)); \
			RegSetValueEx( hKey, "WallpaperStyle", 0, REG_DWORD, (const unsigned char*)(&style), sizeof(style)); \
			RegCloseKey( hKey ); \
		}

		// PP: write the wallpaper filename string to the various different locations in the registry...

		SET_WALLPAPER(HKEY_CURRENT_USER, "Control Panel\\Desktop");
		SET_WALLPAPER(HKEY_CURRENT_USER, "Software\\Microsoft\\Internet Explorer\\Desktop\\General");
		SET_WALLPAPER(HKEY_USERS, ".DEFAULT\\Control Panel\\Desktop");
		SET_WALLPAPER(HKEY_USERS, ".DEFAULT\\Software\\Microsoft\\Internet Explorer\\Desktop\\General");
	}

#endif// PP: (BPLATFORM == PC)
}

