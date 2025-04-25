// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : control.cpp
//   Purpose : provide basic input and control functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "stdio.h"

#include "actors.h"
#include "control.h"
#include "maths.h"
#include "status.h"
#include "camera.h"
#include "holysmoke.h"
#include "physics.h"
#include "animation.h"
#include "characters.h"
#include "display.h"
#include "main.h"
#include "pneumaticplunger.h"
#include "projectile.h"
#include "sfx.h"
#include "scene.h"
#include "clouds.h"
#include "vehicles.h"
#include "explode.h"
#include "fountain.h"
#include "zzz.h"
#include "electrocution.h"
#include "fade.h"
#include "polyeffects.h"
#include "costumes.h"
#include "voodoocloud.h"
#include "endoflevelstats.h"
#include "superburp.h"
#include "superburpeffect.h"
#include "music.h"
#include "frontendmenu.h"
#include "chillipepper.h"
#include "boss.h"
#include "rumble.h"
#include "fireworks.h"
#include "VideoFX.h"					// PP: Video effects - motion blur, defocus, whatever.  Top-of-the-Pops-TASTIC, Mate!
#include "collectibles.h"
#include "fallingobjects.h"
#include "portal.h"
#include "files.h"
#include "smashcamera.h"
#include "multiplayer.h"
#include "genericai.h"
#include "doors.h"
#include "lights.h"
#include "phonebox.h"
#include "special.h"
#include "rapperblast.h"
#include "tartantaz.h"
#include "transportereffect.h"
#include "watersplash.h"
#include "Pause.h"						// PP: pause mode functions
#include "prompt.h"
#include "zookeeper.h"
#include "musicandsavagebeasts.h"
#include "cheesegun.h"
#include "Cheats.h"						// PP: Cheats
#include "endgamecredits.h"
#include "gui.h"
// PP: TEMP?
//#include "WestBoss.h"					// PP: Wile E West Boss - Yosemite Sam's ammo dump
#include "timer.h"						// PP: timer functions
#include "demo.h"						// PP: consumer demo specifc stuff
#include "WestBoss.h"					// PP: Wile E West Boss - Yosemite Sam's ammo dump
#include "cutscene.h"					// PP: provides functions to initialise,play and free cutscenes

#include "libsceedemo.h"

#ifdef TONY
//#define FORCEGUION
#endif

// TP: #define STATE_DEBUG

TBVector				startVel;
TBVector				landingVel;
int						numFrames;
float					totalTime;
float					maxHeight;
						
int						noofBytes;
						
int						slideChannel;
						
static bool				stateChanged;
static bool				noExitState;

bool					attractInterrupt=false;	// PP: flag to interrupt the attract cycle, eg. on controller insertion 

// PP: current pad-checking mode
EPadCheckMode			padCheckMode=PADCHECK_ALL;

static TBEventClient	*padEvent=NULL;		// PP: event client used to catch the _PadChange event

// PP: custom callback function for projectile-state collisions
ProjectileStateCollisionCallback			projectileStateCustomCollisionCallback=NULL;

#if BPLATFORM == PC
static GUID		voidGuid = { -1, -1, -1, { -1, -1, -1, -1, -1, -1, -1, -1 } };	// Void Guid
static GUID		nullGuid = { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } };	// NULL guid
#endif


// PP: define MOTIONBLUR_AT_HIGH_SPEED to enable motionblur when Taz is going at high speed
// PP: #define MOTIONBLUR_AT_HIGH_SPEED


static		int	numPadsConnected=0;	// TP: keep track of the number of pads connected
/*  --------------------------------------------------------------------------------
	Function	: padEventCallback
	Purpose		: called whenever a pad is inserted or removed
	Parameters	: The name of the event being triggered - this is case sensitive,
					the parameter string associated with the event,
					the data associated with the event,
					a client specified context value (set in bkTrapEvenCallback when callback was attached)
	Returns		: 
	Info		: // PP: must match the TBEventCallback typedef
*/
static void padEventCallback(char *name, char *parms, void* data, void *context)
{
#if (BPLATFORM == XBOX)
	// PP: Xbox TCR 3-40 states that controller insertion must interrupt the attract cycle.
	// PP: I don't see anything stopping us from using controller REMOVAL to interrupt it as well.

	// PP: long-winded but (hopefully) reliable way of telling if we're on the main game intro cutscene as part of the attract cycle
#define PLAYING_MAIN_GAME_INTRO_CUTSCENE	((gameStatus.currentState == GAMESTATE_OUTRO)\
											&&(gameStatus.outroState == OUTRO_ENDCUTSCENE)\
											&&(map.exitCutScene!=NULL)\
											&&(map.exitCutScene->State()==CUTSCENE_PLAYING)\
											&&(map.sceneNumber == SCENE_LANGUAGESELECT && map.lastScene == SCENE_FRONTEND))

	// PP: set the attract mode interrupt flag
	if (bInputInfo.noofConnectedDevices > numPadsConnected)
	{
		if((((gameStatus.currentState == GAMESTATE_INGAME)
			||(gameStatus.currentState == GAMESTATE_DEMOQUIT)
			||(gameStatus.currentState == GAMESTATE_CUTSCENE))
			&&(gameStatus.demoMode==1))// PP: 1=playing demo
			||PLAYING_MAIN_GAME_INTRO_CUTSCENE)// PP: attract mode main game intro cutscene must also be skippable in the same way as the rolling demo, regardless of gameStatus.demoMode
		{
			attractInterrupt=true;
		}
	}
	numPadsConnected = bInputInfo.noofConnectedDevices;

#endif
}


/* --------------------------------------------------------------------------------
   Function		: Control_init
   Purpose		: initialisation at the beginning of the game for the Control module
   Parameters	: 
   Returns		: 
   Info			: // PP: 
*/
void Control_init(void)
{
	// PP: set up the controller insertion/removal event client
	padEvent = bkTrapEventCallback("_PadChange",padEventCallback,NULL);
}


/* --------------------------------------------------------------------------------
   Function		: Control_shutdown
   Purpose		: shutdown at the end of the game for the Control module
   Parameters	: 
   Returns		: 
   Info			: // PP: 
*/
void Control_shutdown(void)
{
	// PP: delete the controller insertion/removal event client
	if(padEvent != NULL)
	{
		bkDeleteEventClient(padEvent);
	}
}


/* --------------------------------------------------------------------------------
   Function : ProcessInput
   Purpose : Responds to pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO or NULL, ACTOR or NULL
   Returns : 
   Info : will modify the camera and actor if supplied
*/
void ProcessInput(PLAYER *player)
{
	PADCONTROLSTATUS	*controller = player->controller;
	CAMERAINFO			*cameraInfo = player->camera;
	ACTORINSTANCE		*actorInstance = player->actorInstance;
	TBQuaternion		tempQuat;
	TBVector			tempVector;
	ACTOR_PHYSICS		*actorPhysics = &actorInstance->actorBehaviour->physics;

	float	oldSpdRto = speedRatio;

	if (player->actorInstance->actorStatus->multiplayer.effect == PLAYEREFFECTSLOMO)
	{
		speedRatio = 15/fps;
	}

	// PP: don't respond to these controls if we're navigating in-game menus
	if(!gameStatus.navigatingMenus)
	{
		if (debugDPad)
		{
			// Debug input
			if (controller->dPadYDebounceChannel->value > 0.0f)
			{
				//TBVector tvec = { 0.0f,1.0f,0.0f,0.0f };
				//AddActorSpeed( actorInstance->actorBehaviour, tvec, METERTOUNIT(3) );
				if((actorInstance->actorStatus->currentState == actorInstance->actorStatus->currentMoveState)||(actorInstance->actorStatus->currentState == STATE_JUMP)
					||(actorInstance->actorStatus->currentState == STATE_SLIDE))
				{
					actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_NOGRAVITY;
					actorPhysics->flags |= PHYSFLAG_WALLGROUNDCOLLFIX;
					actorPhysics->flags &= ~PHYSFLAG_WALLVELCORRECTION;
					SETVECTOR(tempVector, 0.0f, 0.0f, 0.0f, 1.0f);
					SetActorVelocity(actorInstance->actorBehaviour, tempVector);
					ChangeState(actorInstance, STATE_DEBUGMOVE);
					return;
				}
			}

			// PP: fly cam on/off with d-left
			if(controller->dPadXDebounceChannel-> value < 0.0f)
			{
				if((actorInstance->actorStatus->currentState == actorInstance->actorStatus->currentMoveState)
					||(actorInstance->actorStatus->currentState == STATE_DEBUGMOVE)
					||(actorInstance->actorStatus->currentState == STATE_VEHICLE)
					||(actorInstance->actorStatus->currentState == STATE_DONOTHING))
				{
					toggleFlyCam();
				}
			}
		}
	}

	// PP: toggle first-person by clicking left stick
/*REMOUT: not ready yet
	if(controller->l3DebounceChannel->value)
	{
		toggleFirstPersonMode();
	}*/

	if (cameraInfo)
	{
		RespondToCameraControl(player);
		if (actorInstance)
		{
			if (actorInstance->actorStatus)
			{
				if (gameStatus.controlSuspend==FALSE)
				{
					// PP: if we're in the zoo boss game, this stuff is done by ZBBAT::update (ZooBoss.cpp)
					//if((map.sceneNumber != SCENE_ZOOBOSS) /*&& !((map.sceneNumber == SCENE_WESTBOSS)&&(player==WestBoss_game.sam.player))*/)
					if(player->flags & PLAYERFLAG_DEFAULTCONTROL)
					{
						UpdateCharacterState(player);
					}
				}
				else
				{	
					switch(gameStatus.currentScene)
					{
#ifndef CONSUMERDEMO
					case SCENE_CITYBOSS:	
						if (CheckIfPlayingAnimation(actorInstance,"spherestatic"))
								break;

						PlayAnimationByName(actorInstance,"spherestatic",1.0f,1,0,0.0f,NONE);
								break;
					case SCENE_TAZBOSS:
						// TP: Do quick update so as to avoid the single frame without animation
						AnimateActorInstance(actorInstance);
						baPrepActorInstance(&actorInstance->actorInstance,true);
						break;
					case SCENE_WESTBOSS:
						{
							// PP: override input if westboss game wants us to
							if(WestBoss_game.overrideInput())
							{
								break;
							}
						}// PP: no break
#endif
					default:
						if (gameStatus.player1->actorStatus->currentState == STATE_VEHICLE)
							break;

						if (CheckIfPlayingAnimation(actorInstance,"idle1"))
							break;

						if(actorInstance->actorStatus->currentState == STATE_BALL) break;
						if ((map.sceneNumber==SCENE_GOLDMINE)&&(frankenMachineActive==true)) break;

						SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
						actorInstance->actorAnimation->useMotionBone = TRUE;

						bmQuatCopy(tempQuat,actorInstance->actorInstance.orientation);
						ChangeActorInstance(actorInstance,FindActorInActorList(actorInstance->actorName),5,NULL);
						
						if(strcmpi(actorInstance->actorName, "shedevil.obe") == 0)// CPW: stinky bodge
							AttachObjectToCharacter(actorInstance,"shedevilbowtie.obe","spinetop",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);

						bmQuatCopy(actorInstance->actorInstance.orientation,tempQuat);
						ResetCharacterAfterJump(actorInstance);
						actorPhysics->flags &= ~PHYSFLAG_WALLGROUNDCOLLFIX;
						actorPhysics->flags |= PHYSFLAG_WALLVELCORRECTION;

						// PP: Setup costume even if none is being worn - this sets up any costume accessories specified by cheats, eg. the DIGITALWATCH cheat
						SetupCostume(actorInstance,actorInstance->characterInfo->costume);

						FlushAnimationQueue(actorInstance);

						if (actorInstance->actorStatus->currentState == STATE_SPIN)								
						{
							SpinDown(&gameStatus.player[actorInstance->playerNo]);
							PlayAnimationByName(actorInstance,"idle1",1.0f,1,1,0.0f,NONE);								
						}
						else
						{
							PlayAnimationByName(actorInstance,"idle1",1.0f,1,0,0.0f,NONE);
#ifdef PHIL// PP: TEMP TEST
							bkPrintf("STARTING IDLE ANIM\n");
#endif// PP: def PHIL
						}

						// TP: Do quick update of the costumes
						UpdateAttachedObjects(actorInstance);
						PrepAttachedObjects(actorInstance);

						// TP: Do quick update so as to avoid the single frame without animation
						AnimateActorInstance(actorInstance);
						baPrepActorInstance(&actorInstance->actorInstance,true);

						break;
					}
				}
			}
		}
	}

	speedRatio = oldSpdRto;
}


/*  --------------------------------------------------------------------------------
	Function	: ClearSoundHandle
	Purpose		: Callback used to reset a soundHandle to -1 when sample is finished
	Parameters	: pointer to int for handle
	Returns		: 
	Info		: ***WARNING*** you must make sure that the soundHandle pointed to does not go out of scope
*/

void ClearSoundHandle(void *voidPtr)
{
	int *intPtr = (int*)voidPtr;
	*intPtr = -1;
}


/* --------------------------------------------------------------------------------
   Function		: HandleSquashedState
   Purpose		: Handle the squashed state in the normal way
   Parameters	: ptr to PLAYER who's squashed,
   Returns		: 
   Info			: // PP: this saves cutting & pasting for alternative (eg. boss game) UpdateCharacterState-type functions.
					It also cuts down on the size of UpdateCharacterState.
*/
void HandleSquashedState(PLAYER* const player)
{
	ACTORINSTANCE			*actorInstance = player->actorInstance;
	ACTOR_STATUS			*actorStatus = actorInstance->actorStatus;

	if(RespondToStateExit(actorInstance))
	{
		// TP: ReduceCharactersHealth(actorInstance,0.5f);
		DestroyInjuryStars(actorInstance);
		RespondToStateExitPost(actorInstance,actorStatus);
		return;
	}
	if(!CheckIfPlayingAnimation(actorInstance, NULL))
	{
		// TP: ReduceCharactersHealth(actorInstance,0.5f);
		DestroyInjuryStars(actorInstance);
		PlayAnimationByName(actorInstance,"idle1",1.0f,1,0,0.1f,NONE);

		ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
		//-----------------STATE CHANGE-----------------
		return;
	}
}


/* --------------------------------------------------------------------------------
   Function		: CheckForJump
   Purpose		: Check if the player is trying to jump; start the jump if so
   Parameters	: ptr to PLAYER for which to check
   Returns		: true if the player is trying to jump
   Info			: // PP: this saves cutting & pasting for alternative (eg. boss game) UpdateCharacterState-type functions.
					It also cuts down on the size of UpdateCharacterState.
*/
bool CheckForJump(PLAYER* const player)
{
	PADCONTROLSTATUS		*controller = player->controller;
	ACTORINSTANCE			*actorInstance = player->actorInstance;
	ACTOR_PHYSICS			*actorPhysics = &actorInstance->actorBehaviour->physics;
	ACTOR_STATUS			*actorStatus = actorInstance->actorStatus;
	TBVector				tvec;

	if (controller->crossDebounceChannel->value)
	{
		CalculateSpeedBeforeJump(actorInstance);							// fill in current speed before jump
		PlayAnimationByName(actorInstance,"jumprise",0.6f,0,0,0.1f,JUMP);	// If I could play jump then
		actorInstance->actorInstance.position[Y] += 1.0f;					// bodge to avoid sticking to ground

		// zero vertical speed before adding jump
		actorInstance->actorBehaviour->physics.velocity[Y] = 0.0f;
		SETVECTOR(tvec,0.0f,1.0f,0.0f,1.0f);
		
		if (player->actorInstance->actorStatus->multiplayer.effect == PLAYEREFFECTSLOMO)
			AddActorSpeed( actorInstance->actorBehaviour, tvec, METERTOUNIT(3.0f));//METERTOUNIT(6.324f)
		else
			AddActorSpeed( actorInstance->actorBehaviour, tvec, METERTOUNIT(7.0f));//METERTOUNIT(6.324f)

		// Queue fall animation
		PlayAnimationByName(actorInstance,"jumpfall",0.4f,1,1,0.4f,JUMP);

		SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
		actorPhysics->flags &= ~PHYSFLAG_WALLVELCORRECTION;
		actorInstance->actorAnimation->useMotionBone = FALSE;

		//switch (bmRand()%2)
		//{
		//case 0:	
			PlaySample("tazjump.wav",255,actorInstance->actorInstance.position);	//break;
		//case 1:	PlaySample("tazjump_2.wav",255,actorInstance->actorInstance.position);	break;
		//}

		switch(bmRand()%25)
		{
		case 0:
			PlaySample("fart1.wav",255,actorInstance->actorInstance.position);
			break;
		case 1:
			PlaySample("fart2.wav",255,actorInstance->actorInstance.position);
			break;
		//case 2:
		//	PlaySample("fart3.wav",255,actorInstance->actorInstance.position);
		//	break;
		case 3:
			PlaySample("burp1.wav",255,actorInstance->actorInstance.position);
			break;
		case 4:
			PlaySample("burp2.wav",255,actorInstance->actorInstance.position);
			break;
		}

		ChangeState(actorInstance,STATE_JUMP);

		// PP: (for CHEAT_TIMESLICEJUMP) unlatch the timeslice
		VFX_timesliceLatch=false;

		//-----------------STATE CHANGE-----------------
		return true;
	}

	return false;
}

/*  --------------------------------------------------------------------------------
	Function	: EnterSpinState
	Purpose		: Start the spin move
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
static void EnterSpinState(ACTORINSTANCE *actorInstance)
{
	ACTOR_STATUS			*actorStatus = actorInstance->actorStatus;

	CalculateSpeedBeforeJump(actorInstance);	// fill in current speed before spin
	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance, "spinup", 2.0f, 0, 0, 0.1f, NONE);
	actorInstance->actorAnimation->useMotionBone = FALSE;

	StopChannel(&actorStatus->soundHandle[0]);	

	actorStatus->soundHandle[0] =

	// PP: TEMP BABEL WORKAROUND!!!!! TEMP!!!!!...
#if(BPLATFORM == XBOX)
	PlaySample("tazspin.wav",160);
#else// PP: if(BPLATFORM == XBOX) else
	PlaySample("tazspin.wav",150,actorInstance->actorInstance.position);
#endif// PP: (BPLATFORM == XBOX)

	ChangeState(actorInstance,STATE_SPINUP);
}

/*  --------------------------------------------------------------------------------
	Function	: EnterSpinState
	Purpose		: Start the spin move
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
static void EnterLoseCostumeState(ACTORINSTANCE *actorInstance)
{
	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance, "spinup", 1.0f, 0, 0, 0.1f, REACT);

//	if(!actorInstance->characterInfo->costumeChangeInfo) actorInstance->characterInfo->costumeChangeInfo = CreateLoseCostumeEffect(actorInstance, "spinemid", 2.0f);
	ChangeState(actorInstance, STATE_LOSECOSTUME);
}

/* --------------------------------------------------------------------------------
   Function		: HandleJumpState
   Purpose		: Handle the jump state in the normal way
   Parameters	: ptr to PLAYER who's jumping,
					(opt/false) whether or not the character's Y angle is locked during the jump,
					(opt/true) whether or not to go into a move anim on landing
   Returns		: 
   Info			: // PP: this saves cutting & pasting for alternative (eg. boss game) UpdateCharacterState-type functions.
					It also cuts down on the size of UpdateCharacterState.
*/
#define MAX_FALL_HEIGHT (METERTOUNIT(5.0f))

void HandleJumpState(PLAYER* const player, const bool lockYAng, const bool intoMoveAnim)
{
	PADCONTROLSTATUS		*controller = player->controller;
	CAMERAINFO				*cameraInfo = player->camera;
	ACTORINSTANCE			*actorInstance = player->actorInstance;
	ACTOR_STATUS			*actorStatus = actorInstance->actorStatus;
	ACTOR_PHYSICS			*actorPhysics = &actorInstance->actorBehaviour->physics;
	TBActorAnimSegment		*animationToPlay;
	bool					onWayUp;

	if (CheckIfPlayingAnimation(actorInstance,"jumprise"))
	{
		onWayUp = true;
	}
	else
	{
		onWayUp = false;
	}

	if (RespondToStateInit(actorInstance))
	{
		if ((!CheckIfPlayingAnimation(actorInstance,"bouncedown"))&&(!CheckIfPlayingAnimation(actorInstance,"bouncedown2")))
		{
			// TP: get the initial position of the character before jump
			bmVectorCopy(actorStatus->positionBeforeJump,actorInstance->actorInstance.position);
		}

		// TP: use positionBeforeJump temporarly to store position before slide
		bmVectorCopy(actorInstance->actorStatus->stuckCheck,actorInstance->actorInstance.position);
	}

	if (RespondToStateExit(actorInstance)) 
	{
		// TP: make sure we get rid of the hiccup mode
		actorInstance->characterInfo->powerUpInfo.hiccupJumpState = false;

		SetCameraSmooth(camera,defaultPosSmooth,defaultLookatSmooth);
		SetDefaultCameraAngleAboveHorizontal(camera,CAMERAANGLE);
		ResetCharacterAfterJump(actorInstance);
		actorPhysics->flags |= PHYSFLAG_WALLVELCORRECTION;
		actorInstance->actorAnimation->useMotionBone = TRUE;
		actorStatus->positionBeforeJump[Y] = METERTOUNIT(1000.0f);
		RespondToStateExitPost(actorInstance,actorStatus);

		// PP: (for timeslice cheats) unlatch the timeslice
		VFX_timesliceLatch=false;

		return;
	}

	// TP: Anti swimmy device, stops the camera from making you sick when jumping
	if (actorInstance->actorInstance.position[Y] < actorStatus->positionBeforeJump[Y])
	{
		// TP: have dropped below position of origional jump point
		SetCameraSmooth(camera,defaultPosSmooth,defaultLookatSmooth);
	}
	else
	{
		// TP: keep the two smooth values the same so as to stop that swimming feeling, ooohhh I feel sick...
		SetCameraSmooth(camera,defaultLookatSmooth/*yes this is deliberate*/,defaultLookatSmooth);
	}

	// Have I collided with something
	if (actorPhysics->coll)
	{
		//actorStatus->timeInCurrentState = 0.0f;

		// TP: If I have collided with the ground and not on the way up
		if ((actorPhysics->flags&PHYSFLAG_GROUNDCOLL)&&(!onWayUp))
		{
			// TP: get rid of hiccup flag
			actorInstance->characterInfo->powerUpInfo.hiccupJumpState = false;
			// TP: Reset camera angle
			SetDefaultCameraAngleAboveHorizontal(camera,CAMERAANGLE);
			SetCameraSmooth(camera,defaultPosSmooth,defaultLookatSmooth);
			// Have hit a ground poly, but may have hit a wall poly too
			// Reset character after jump
			ResetCharacterAfterJump(actorInstance);
			// flush anim ready for move state
			// TP: FlushAnimationQueue(actorInstance);
			ClearCurrentAnimationPriority(actorInstance);

			// -------play movement immediately to avoid a pause on landing---------
			// find the appropriate movement animation to play

			// TP: Quick move into spin
			if(CheckForSpin(player)) return;

			if(intoMoveAnim)
			{
				// TP: check for move straight into move
				animationToPlay = ChooseMovementAnimationForTaz(controller,actorInstance);
				if (actorInstance->actorStatus->timeInRunAnimation > 0.5f) // if running for more that 0.5 secs
				{
					// check to see if going from a run to a stand
					if (animationToPlay==baFindAnimSegment(&actorInstance->actorInstance, "idle1"))
					{
						if (CheckIfPlayingAnimation(actorInstance,"run"))
						{
							PlaySample("skidstop.wav",255,actorInstance->actorInstance.position);
							PlayAnimationByName(actorInstance,"runstop",1.5f,0,0,0.1f,SLIDESTOP);
						}
					}
				}
				// play new animation
				if (CurrentAnimation(actorInstance)!=animationToPlay)	// am i already playing this anim
					PlayAnimationBySegmentPointer(actorInstance,animationToPlay,1.0f,1,0,0.2f,MOVE);
			}
			else
			{
				// PP: NEW: if we don't want to exit to a move anim, exit to no anim - stop the falling anim
				FlushAnimationQueue(actorInstance);
			}

			//check for hiccup jump, and create smoke effect
			if(actorInstance->characterInfo->powerUpInfo.hiccup)
			{
				CreateImpactClouds(actorInstance->actorInstance.position,gameStatus.windVelocity,NULL,METERTOUNIT(0.2f),6.0f,METERTOUNIT(1.0f), SMOKE_BIGFALL);
				RequestRumbleEffect(gameStatus.player[actorInstance->playerNo].controller, 200, 0.0f, 0.5f, 0.5f, 0.0f, WAVEFORM_NORMAL);
			}

			PlaySample("thudlight.wav",255,actorInstance->actorInstance.position);
			
			actorStatus->positionBeforeJump[Y] = METERTOUNIT(1000.0f);
			actorPhysics->flags |= PHYSFLAG_WALLVELCORRECTION;
			actorInstance->actorAnimation->useMotionBone = TRUE;
			
			//-----------------STATE CHANGE-----------------
			if(actorInstance->characterInfo)
			{
				if(actorInstance->characterInfo->powerUpInfo.bubbleGum)
				{
					ChangeState(actorInstance,STATE_BUBBLEGUM);
					myStateChangeBubble(actorInstance);
				}
				else
				{
					ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
				}
				return;
			}
			else
			{
				ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			}


			if ((actorPhysics->flags&PHYSFLAG_WALLCOLL)&&(actorInstance->actorStatus->timeWithoutGroundCollision > 0.3f)
				&&((!actorPhysics->coll)||(actorPhysics->lastAverageNormal[Y]>0.4f))&&(!CheckIfPlayingAnimation(actorInstance,"jumprise"))&&(!controller->l3Channel->value))
			{
				// Have hit a wall poly, should do slide move
				ResetCharacterAfterJump(actorInstance);
				// flush anim ready for move state
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance,"slide",0.1f,1,0,0.1f,JUMP);
				actorPhysics->flags |= PHYSFLAG_WALLVELCORRECTION;
				actorStatus->positionBeforeJump[Y] = METERTOUNIT(1000.0f);
				ZEROVECTOR(actorInstance->actorBehaviour->physics.velocity);
				actorInstance->actorAnimation->useMotionBone = FALSE;
				if (actorStatus->soundHandle[0]==-1)
				{
					actorStatus->soundHandle[0] = PlaySample("taz_gen_08a.wav",255,actorInstance->actorInstance.position);
				}
				ChangeState(actorInstance,STATE_SLIDE);
				//-----------------STATE CHANGE-----------------
				return;
			}
		}
	}
	else
	{
		// TP: Don't do check for bigfall if it is because of a hiccup powerup
		if (!actorInstance->characterInfo->powerUpInfo.hiccup)
		{
			if (actorInstance->actorInstance.position[Y] < (actorStatus->positionBeforeJump[Y]-MAX_FALL_HEIGHT))
			{
				// TP: monitor the progress of the fall in case it is a big one
				if (!CheckIfPlayingAnimation(actorInstance,"fall"))
				{
					SetCameraSmooth(camera,defaultPosSmooth,defaultLookatSmooth);
					ResetCharacterAfterJump(actorInstance);
					actorPhysics->flags |= PHYSFLAG_WALLVELCORRECTION;
					actorInstance->actorAnimation->useMotionBone = TRUE;
					PlayAnimationByName(actorInstance,"fall",1,1,0,0.7f,REACT);
					ChangeState(actorInstance, STATE_BIGFALL);
					return;
				}
			}
		}
	}

	// If there is no collision then process jumping control
	bmVectorCopy(landingVel,actorInstance->actorBehaviour->physics.velocity);

	// TP: Better do check for infernal slide, for example if Taz falls in the crack between three trees you can't escape
	// TP: if (have not move significantly in last second) then try quick spin to solve the problem
	if (actorStatus->timeInCurrentState > 1.0f)
	{
		// TP: see if player has moved much
		if (bmVectorDistance(actorInstance->actorInstance.position,actorStatus->stuckCheck) < METERTOUNIT(0.25f))
		{
			bmVectorSet(actorInstance->actorBehaviour->physics.velocity, 0.0f, 0.0f, 0.0f, 1.0f);
			// TP: Haven't moved enough, must be stuck
			EnterSpinState(actorInstance);
			return;
		}
		bmVectorCopy(actorStatus->stuckCheck,actorInstance->actorInstance.position);
		actorStatus->timeInCurrentState = 0.0f;
	}

	// PP: go all Matrix if you're that way inclined...
	if(CheatActive(CHEAT_TIMESLICEJUMP))
	{
		if(!VFX_timesliceLatch)
		{
			if(actorInstance->actorBehaviour->physics.velocity[Y]<0.0f)
			{
				VFX_timeslice();
			}
		}
	}

	CharacterRespondToMovementWhileJumping(controller,cameraInfo,actorInstance,lockYAng);
}

/* --------------------------------------------------------------------------------
   Function		: CheckForSpin
   Purpose		: Check if the player is trying to spin; start the spin if so
   Parameters	: ptr to PLAYER for which to check
   Returns		: true if the player is trying to spin
   Info			: // PP: this saves cutting & pasting for alternative (eg. boss game) UpdateCharacterState-type functions.
					It also cuts down on the size of UpdateCharacterState.
*/
bool CheckForSpin(PLAYER* const player)
{
	PADCONTROLSTATUS		*controller = player->controller;
	ACTORINSTANCE			*actorInstance = player->actorInstance;

	if (controller->circleChannel->value)
	{
		EnterSpinState(actorInstance);
		//-----------------STATE CHANGE-----------------
		return true;
	}

	return false;
}

/*  --------------------------------------------------------------------------------
	Function	: DoSpitMove
	Purpose		: Like is says
	Parameters	: ACTORINSTANCE
	Returns		: 
	Info		: 
*/
static void DoSpitMove(ACTORINSTANCE *actorInstance)
{
	if (actorInstance->characterInfo->inMouth)
	{
		PlaySample("tazbitespit.wav",255,actorInstance->actorInstance.position);
		PlayAnimationByName(actorInstance,"spit",1,0,0,0.2f,ATTACK);
		TazSpit(actorInstance);
		ChangeState(actorInstance,STATE_SPIT);
		//-----------------STATE CHANGE-----------------
	}
	else
	{
		RequestMorronicMessage(MEVENT_SPIT);
		PlaySample("tazbite.wav",255,actorInstance->actorInstance.position);
		PlayAnimationByName(actorInstance,"attack1",1,0,0,0.2f,ATTACK);
		ChangeState(actorInstance,STATE_BITE);
	}
}

/* --------------------------------------------------------------------------------
   Function		: HandleSpinUpState
   Purpose		: Handle the spin-up state in the normal way
   Parameters	: ptr to PLAYER who's spinning-up, (opt/false) whether or not to lock Y angle during spinup
   Returns		: 
   Info			: // PP: this saves cutting & pasting for alternative (eg. boss game) UpdateCharacterState-type functions.
					It also cuts down on the size of UpdateCharacterState.
*/
void HandleSpinUpState(PLAYER* const player, const bool lockYAng)
{
	PADCONTROLSTATUS		*controller = player->controller;
	CAMERAINFO				*cameraInfo = player->camera;
	ACTORINSTANCE			*actorInstance = player->actorInstance;
	ACTOR_STATUS			*actorStatus = actorInstance->actorStatus;
	ACTOR_PHYSICS			*actorPhysics = &actorInstance->actorBehaviour->physics;

	// PP: if the player is initiating a super-spin, fade down to tiptoe music over super-spin build-up duration,
	// PP:	else just switch to spin music...

	musicSystem.RequestMood(0,STREAMMOOD_FAST);

	EnableCameraAngleAboveHorizontal(cameraInfo,0);

	if (RespondToStateExit(actorInstance)) 
	{
		actorInstance->actorAnimation->useMotionBone = TRUE;
		StopChannel(&actorStatus->soundHandle[0]);	
		RespondToStateExitPost(actorInstance,actorStatus);
		return;
	}
	
	if(!CheckIfPlayingAnimation(actorInstance, "spinup"))
	{
		// set spin behaviour
		SetActorMinFricAngle(actorInstance->actorBehaviour,1.0f);

		ChangeActorInstance(actorInstance,FindActorInActorList("spindevil.obe"),5,NULL);

		ENABLE_LINE_OR_CARTOON_RENDER_MODE(&actorInstance->actorInstance);

		if(actorInstance->characterInfo)
		{
			if(actorInstance->characterInfo->powerUpInfo.invisible)
			{
				EnableActorLineOnlyRenderMode(&actorInstance->actorInstance, &invisLineParms);
				// NH: baSetActorRenderMode(&actorInstance->actorInstance, 0, BACTRENDERMODE_ZONLY, NULL);
				// NH: baSetActorRenderMode(&actorInstance->actorInstance, 1, BACTRENDERMODE_OUTLINED, &invisLineParms);
			}
		}
		PlayAnimationByName(actorInstance,"spin1",6.0f,1,1,0.0f,JUMP);

		// TP: Do quick update so as to avoid the single frame without animation
		AnimateActorInstance(actorInstance);
		baPrepActorInstance(&actorInstance->actorInstance,true);

		actorPhysics->flags |= PHYSFLAG_WALLGROUNDCOLLFIX;
		actorPhysics->flags &= ~PHYSFLAG_WALLVELCORRECTION;
		ChangeState(actorInstance,STATE_SPIN);
		SetActorVelocity(actorInstance->actorBehaviour,actorStatus->velBeforeJump);
		// CMD: add some rumble
#if BPLATFORM!=GAMECUBE
		actorInstance->characterInfo->rumbleHandle = RequestRumbleEffect(controller, 255, 0.25f, 0.0f, 3.0f, 0.0f, WAVEFORM_SQUAREDSINUSOIDAL, 5.0f, 0.0f, 0);
#else
		actorInstance->characterInfo->rumbleHandle = RequestRumbleEffect(controller, 100, 0.25f, 0.0f, 3.0f, 0.0f, WAVEFORM_NORMAL, 5.0f, 0.0f, 0);
#endif
	}
	CharacterRespondToSpinMovementInput(controller,cameraInfo,actorInstance, lockYAng);
}


/* --------------------------------------------------------------------------------
   Function		: HandleSpinState
   Purpose		: Handle the spin state in the normal way
   Parameters	: ptr to PLAYER who's spinning, (opt/false) lock Y angle during spin
   Returns		: 
   Info			: // PP: this saves cutting & pasting for alternative (eg. boss game) UpdateCharacterState-type functions.
					It also cuts down on the size of UpdateCharacterState.
*/
void HandleSpinState(PLAYER* const player, const bool lockYAng)
{
	PADCONTROLSTATUS		*controller = player->controller;
	CAMERAINFO				*cameraInfo = player->camera;
	ACTORINSTANCE			*actorInstance = player->actorInstance;
	ACTOR_STATUS			*actorStatus = actorInstance->actorStatus;
	ACTOR_PHYSICS			*actorPhysics = &actorInstance->actorBehaviour->physics;
	TBQuaternion			tempQuat;

	EnableCameraAngleAboveHorizontal(cameraInfo,0);
	if (RespondToStateExit(actorInstance)) 
	{
		StopChannel(&actorStatus->soundHandle[0]);	
		StopChannel(&actorStatus->soundHandle[1]);	

		SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);

		FlushAnimationQueue(actorInstance);
		bmQuatCopy(tempQuat,actorInstance->actorInstance.orientation);

		ChangeActorInstance(actorInstance,FindActorInActorList(actorInstance->actorName),5,NULL);
		if(strcmpi(actorInstance->actorName, "shedevil.obe") == 0)// CPW: stinky bodge
			AttachObjectToCharacter(actorInstance,"shedevilbowtie.obe","spinetop",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);

		// CPW: ChangeActorInstance(actorInstance,FindActorInActorList("tazdevil.obe"),5,NULL);

		bmQuatCopy(actorInstance->actorInstance.orientation,tempQuat);

		ResetCharacterAfterJump(actorInstance);
		actorPhysics->flags &= ~PHYSFLAG_WALLGROUNDCOLLFIX;
		actorPhysics->flags |= PHYSFLAG_WALLVELCORRECTION;

		// PP: Setup costume even if none is being worn - this sets up any costume accessories specified by cheats, eg. the DIGITALWATCH cheat
		SetupCostume(actorInstance,actorInstance->characterInfo->costume);

		FlushAnimationQueue(actorInstance);

		PlayAnimationByName(actorInstance, "spindown", 2.0f, 0, 0, 0.0f, MOVE);

		// TP: Do quick update so as to avoid the single frame without animation
		AnimateActorInstance(actorInstance);
		baPrepActorInstance(&actorInstance->actorInstance,true);

		// TP: Do quick update of the costumes
		UpdateAttachedObjects(actorInstance);
		PrepAttachedObjects(actorInstance);

		actorInstance->actorAnimation->useMotionBone = TRUE;
		
		RespondToStateExitPost(actorInstance,actorStatus);
		return;
	}
	
	// if no longer pressing circle then stop
	if ((!controller->circleChannel->value)||(controller->crossDebounceChannel->value))
	{
		if((map.sceneNumber != SCENE_LANGUAGESELECT)&&(map.sceneNumber != SCENE_FRONTEND))
		{
			SpinDown(player);
			return;// PP: new test
		}
	}

	//look for bounce
	if ((actorStatus->timeWithoutWallCollisionLastFrame > 0.2f)&&(actorPhysics->coll))
	{
		if (!CheckIfPlayingAnimation(actorInstance, "spinbounce1"))
		{
			FlushAnimationQueue(actorInstance);
			PlayAnimationByName(actorInstance,"spinbounce1",1,0,0,0,JUMP);
		}
	}

	if (!CheckIfPlayingAnimation(actorInstance, "spinbounce1"))
	{
		if (actorInstance->actorBehaviour->physics.speed > METERTOUNIT(3.0f))
		{
			if (!CheckIfPlayingAnimation(actorInstance, "spin1"))
			{
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance,"spin1",1,1,1,0.2f,JUMP);
			}
		}
		else
		{
			if (!CheckIfPlayingAnimation(actorInstance, "idle1"))
			{
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance,"idle1",1,1,1,0.2f,JUMP);
			}
		}
	}
	
	if(bmVectorLen(actorInstance->actorBehaviour->physics.velocity) > METERTOUNIT(0.5f))
	{
		CreateImpactClouds(actorInstance->actorInstance.position, gameStatus.windVelocity, NULL, METERTOUNIT(0.2f), 0.18f, 0.0f, SMOKE_TAZSPIN);
	}
	// character respond to spin control
	CharacterRespondToSpinMovementInput(controller,cameraInfo,actorInstance,lockYAng);
}

/* --------------------------------------------------------------------------------
   Function		: HandHandleLoseCostumeSpinleSpinState
   Purpose		: Handle the spin state without any control
   Parameters	: ptr to PLAYER who's spinning, (opt/false) lock Y angle during spin
   Returns		: 
   Info			: 
*/
void HandleLoseCostumeSpin(PLAYER* const player, const bool lockYAng)
{
	PADCONTROLSTATUS		*controller = player->controller;
	CAMERAINFO				*cameraInfo = player->camera;
	ACTORINSTANCE			*actorInstance = player->actorInstance;
	ACTOR_STATUS			*actorStatus = actorInstance->actorStatus;
	ACTOR_PHYSICS			*actorPhysics = &actorInstance->actorBehaviour->physics;
	TBQuaternion			tempQuat;

	EnableCameraAngleAboveHorizontal(cameraInfo,0);
	
	if(bmVectorLen(actorInstance->actorBehaviour->physics.velocity) > METERTOUNIT(0.5f))
	{
		CreateImpactClouds(actorInstance->actorInstance.position, gameStatus.windVelocity, NULL, METERTOUNIT(0.2f), 0.75f, 0.0f, SMOKE_TAZSPIN);
	}
	// character respond to spin control
}

/* --------------------------------------------------------------------------------
   Function		: SpinDown
   Purpose		: Break out of STATE_SPIN into STATE_SPINDOWN
   Parameters	: ptr to PLAYER who's gonna break out of a spin
   Returns		: 
   Info			: // PP: this saves cutting & pasting for alternative (eg. boss game) UpdateCharacterState-type functions.
					It also cuts down on the size of UpdateCharacterState.
*/
void SpinDown(PLAYER* const player)
{
	ACTORINSTANCE			*actorInstance = player->actorInstance;
	ACTOR_STATUS			*actorStatus = actorInstance->actorStatus;
	ACTOR_PHYSICS			*actorPhysics = &actorInstance->actorBehaviour->physics;
	TBQuaternion			tempQuat;

	// reset behaviour
	SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
	if(actorInstance->characterInfo->spinInfo)
	{
		actorInstance->characterInfo->spinInfo->spinUpdated = FALSE;
	}

	actorStatus->targetSpinSpeed = 0.0f;

	FlushAnimationQueue(actorInstance);
	
	// TP: if character has velocity the get orientation from that, if not then take current orientation of spin model
	if (actorInstance->actorBehaviour->physics.speed)
	{
		// TP: get quat from the velocity
		bmDirectionToQuat(tempQuat,actorInstance->actorBehaviour->physics.velocity,0.0f);
	}
	else
	{
		bmQuatCopy(tempQuat,actorInstance->actorInstance.orientation);
	}

	ChangeActorInstance(actorInstance,FindActorInActorList(actorInstance->actorName),5,NULL);
	if(strcmpi(actorInstance->actorName, "shedevil.obe") == 0)// CPW: stinky bodge
		AttachObjectToCharacter(actorInstance,"shedevilbowtie.obe","spinetop",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);

	EnableActorCartoonRenderMode(&actorInstance->actorInstance);
	if(actorInstance->characterInfo)
	{
		if(actorInstance->characterInfo->powerUpInfo.invisible)
		{
			EnableActorLineOnlyRenderMode(&actorInstance->actorInstance, &invisLineParms);
			// NH: baSetActorRenderMode(&actorInstance->actorInstance, 0, BACTRENDERMODE_ZONLY, NULL);
			// NH: baSetActorRenderMode(&actorInstance->actorInstance, 1, BACTRENDERMODE_OUTLINED, &invisLineParms);
		}
	}
	bmQuatCopy(actorInstance->actorInstance.orientation,tempQuat);
	ResetCharacterAfterJump(actorInstance);
	actorPhysics->flags &= ~PHYSFLAG_WALLGROUNDCOLLFIX;
	actorPhysics->flags |= PHYSFLAG_WALLVELCORRECTION;

	// PP: Setup costume even if none is being worn - this sets up any costume accessories specified by cheats, eg. the DIGITALWATCH cheat
	SetupCostume(actorInstance,actorInstance->characterInfo->costume);

	FlushAnimationQueue(actorInstance);

	PlayAnimationByName(actorInstance, "spindown", 2.0f, 0, 0, 0.0f, MOVE);

	// TP: Do quick update so as to avoid the single frame without animation
	AnimateActorInstance(actorInstance);
	baPrepActorInstance(&actorInstance->actorInstance,true);

	// TP: Do quick update of the costumes
	UpdateAttachedObjects(actorInstance);
	PrepAttachedObjects(actorInstance);

	actorInstance->actorAnimation->useMotionBone = TRUE;

	StopChannel(&actorStatus->soundHandle[0]);	
	StopChannel(&actorStatus->soundHandle[1]);	
	PlaySample("skidstop.wav",255,actorInstance->actorInstance.position);
	
	ChangeState(actorInstance,STATE_SPINDOWN);
	//-----------------STATE CHANGE-----------------
}


/* --------------------------------------------------------------------------------
   Function		: HandleSpinDownState
   Purpose		: Handle the spin-down state in the normal way
   Parameters	: ptr to PLAYER who's spinning-down
   Returns		: 
   Info			: // PP: this saves cutting & pasting for alternative (eg. boss game) UpdateCharacterState-type functions.
					It also cuts down on the size of UpdateCharacterState.
*/
void HandleSpinDownState(PLAYER* const player)
{
	PADCONTROLSTATUS		*controller = player->controller;
//	CAMERAINFO				*cameraInfo = player->camera;
	ACTORINSTANCE			*actorInstance = player->actorInstance;
	ACTOR_STATUS			*actorStatus = actorInstance->actorStatus;
	ACTOR_PHYSICS			*actorPhysics = &actorInstance->actorBehaviour->physics;
	TBVector				tvec = { 0.0f,1.0f,0.0f,1.0f };//,tempVector;

	if (RespondToStateExit(actorInstance)) 
	{
		actorInstance->characterInfo->rumbleHandle = FlushRumbleQueueMember(controller, actorInstance->characterInfo->rumbleHandle);
		RespondToStateExitPost(actorInstance,actorStatus);
		return;
	}

	// PP: back to normal mood as soon as we start to spin down - just a bit neater (to play) & more consistent that way

	// PP: messy: if tiptoe button is being held, go to tiptoe music instead
	if(controller->l2Channel->value)
	{
		musicSystem.RequestMood(0,STREAMMOOD_SLOW);
	}
	else
	{
		musicSystem.RequestMood(0,STREAMMOOD_NORMAL);
	}

	if(!CheckIfPlayingAnimation(actorInstance, "spindown"))
	{
		actorInstance->characterInfo->rumbleHandle = FlushRumbleQueueMember(controller, actorInstance->characterInfo->rumbleHandle);
		ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
		//-----------------STATE CHANGE-----------------
		return;
	}

	// TP: continue to allow for jump
	// check for player pressing jump
	// TP: not if in caged move state though
	if ((controller->crossChannel->value)&&(actorInstance->actorStatus->currentMoveState!=STATE_CAGEDMOVE))
	{
		PlayAnimationByName(actorInstance,"jumprise",0.6f,0,0,0.1f,JUMP);	// If I could play jump then
		CalculateSpeedBeforeJump(actorInstance);							// fill in current speed before jump
		actorInstance->actorInstance.position[Y] += 1.0f;					// bodge to avoid sticking to ground

		// zero vertical speed before adding jump
		actorInstance->actorBehaviour->physics.velocity[Y] = 0.0f;
		SETVECTOR(tvec,0.0f,1.0f,0.0f,1.0f);
		
		if (player->actorInstance->actorStatus->multiplayer.effect == PLAYEREFFECTSLOMO)
			AddActorSpeed( actorInstance->actorBehaviour, tvec, METERTOUNIT(3.0f));//METERTOUNIT(6.324f)
		else
			AddActorSpeed( actorInstance->actorBehaviour, tvec, METERTOUNIT(7.0f));//METERTOUNIT(6.324f)

		// Queue fall animation
		PlayAnimationByName(actorInstance,"jumpfall",0.4f,1,1,0.4f,JUMP);
		actorPhysics->flags &= ~PHYSFLAG_WALLVELCORRECTION;
		actorInstance->actorAnimation->useMotionBone = FALSE;

		// keep track of position prior to jump move
		switch (bmRand()%2)
		{
		case 0:	PlaySample("tazjump_1.wav",255,actorInstance->actorInstance.position);	break;
		case 1:	PlaySample("tazjump_2.wav",255,actorInstance->actorInstance.position);	break;
		}

		actorInstance->characterInfo->rumbleHandle = FlushRumbleQueueMember(controller, actorInstance->characterInfo->rumbleHandle);
		ChangeState(actorInstance,STATE_JUMP);
		//-----------------STATE CHANGE-----------------
		return;
	}

	// TP: Allow player to go straight from spin into spit
	// TP: Not if in caged move state
	if ((controller->triangleChannel->value)&&(actorInstance->actorStatus->currentMoveState!=STATE_CAGEDMOVE))
	{
		actorInstance->characterInfo->rumbleHandle = FlushRumbleQueueMember(controller, actorInstance->characterInfo->rumbleHandle);
		DoSpitMove(actorInstance);
		//-----------------STATE CHANGE-----------------
		return;
	}
}


/* --------------------------------------------------------------------------------
   Function : UpdateCharacterState
   Purpose : Analize the characters current state and input to decide what character does
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : if changeStateOnly is set then return value tells if the requested state change was completed, 
				otherwise the return value tells if any state change was made
   Info : 
*/
bool UpdateCharacterState(PLAYER *player, bool changeStateOnly)
{
	PADCONTROLSTATUS		*controller = player->controller;
	CAMERAINFO				*cameraInfo = player->camera;
	ACTORINSTANCE			*actorInstance = player->actorInstance;
	TBActorInstance			*tbTempInstance;
	ACTOR_STATUS			*actorStatus = actorInstance->actorStatus;
	ACTOR_PHYSICS			*actorPhysics = &actorInstance->actorBehaviour->physics;
	TBVector				tvec = { 0.0f,1.0f,0.0f,1.0f };
	TBActorAnimSegment		*animationToPlay;
	float					tempFloat;
	TBVector				zeroVec = {0.0f, 0.0f, 0.0f, 1.0f};
	TBVector				yAxis = {0.0f, 1.0f, 0.0f, 1.0f};
	TBVector				starsOffset = {0.0f, METERTOUNIT(0.2f), 0.0f, 1.0f};
	TBVector				starsBounds = {METERTOUNIT(0.3f), METERTOUNIT(0.4f), 0.0f, 0.0f};
	TBMatrix				tempMatrix;
	TBVector				screenSpace, worldSpace, position, velocity;
	TBActorNodeInstance		*node;
	FADE					*fade = &player->camera->fade;
	TBVector				delta, clip, tempVector;
	TBQuaternion			tempQuat;
	float					diameter = METERTOUNIT(0.779f);
	float					length;
	int						i, stateIn = actorStatus->currentState;
	float					fOne, fTwo, fThree;

	if (changeStateOnly==false)
	{
		if(watchLook&&CheatActive(CHEAT_DIGITALWATCH))
		{
			// PP: check to see if the player has stopped looking at the watch
			CheckForWatchLook();

			return false;
		}

		if ((actorStatus->currentState!=STATE_DEBUGMOVE)&&(actorStatus->currentState!=STATE_DEAD)
			&&(actorStatus->stateRequest!=STATE_DEAD)&&(fade->GetFadeState()==FADESTATE_OFF))
		{
			if (actorInstance->actorInstance.position[Y] < (worldBoundingBox.ymin-METERTOUNIT(2.0f)))
			{
				if(~actorInstance->flags&ACTORFLAG_DONTCHECKBOUNDINGBOX)
				{
					if (map.sceneNumber > SCENE_TAZBOSS && gameStatus.multiplayer.versusCPU == false)
					{
						DoMultiplayerCharacterFall(actorInstance);
					}
					else
					{
						// CMD: repsawn power ups
						RespawnAllPowerUps();
						ProcessCharacterDeath(actorInstance);
					}
				}
			}
		}
		
	#ifndef NOHEALTH
		if((map.sceneNumber != SCENE_LANGUAGESELECT)&&(map.sceneNumber != SCENE_FRONTEND)&&(actorInstance->characterInfo))
		{
			if((controller->r3DebounceChannel->value)&&(!actorInstance->characterInfo->health.isHealthBubbleActive))
			{
				actorInstance->characterInfo->health.isHealthBubbleActive = TRUE;
				actorInstance->characterInfo->health.timeHealthBubbleActive = 0.0f;
			}
		}
	#endif

		actorInstance->actorStatus->timeInCurrentState += fTime;

#ifdef MOTIONBLUR_AT_HIGH_SPEED

#if(BPLATFORM != PC)// PP: IFDEFOUT: I'm hoping this will only be temporary.  PC render targets have to be pow-2 (I currently use a blocky 512*256 for motion blur), but I'll do a couple of tests and see if I can't get more res.

		if(actorPhysics->speed > actorPhysics->maxSpeed)
		{
			if((tempFloat = (actorPhysics->speed-actorPhysics->maxSpeed)*0.004f*MOTIONBLUR_SPIN_CONST) > 0.3f) tempFloat = 0.3f;
			VFX_motionBlur(tempFloat);
		}

#endif// PP: (BPLATFORM != PC)

#endif// PP: def MOTIONBLUR_AT_HIGH_SPEED

	}

	noExitState = true;
	stateChanged = false;


	switch(actorStatus->currentState)
	{
	case STATE_INIT: 
		noExitState = false;

		// TP: if entering scene using a x door then get door and go to correct state
		if (camera[0].fade.xDoorCRC)
		{
			if (EnterSceneUsingXDoor(actorInstance,camera[0].fade.xDoorCRC))
			{
				camera[0].fade.xDoorCRC = 0;
				ChangeState(actorInstance,STATE_ENTERINGXDOOR);
				break;
			}
			camera[0].fade.xDoorCRC = 0;
		}

		if ((IN_BOSS_GAME)||(map.sceneNumber > SCENE_TAZBOSS))
		{
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			break;
		}

		if (gameStatus.demoMode)
		{
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			break;
		}

		FlushAnimationQueue(actorInstance);
		PlayAnimationByName(actorInstance,"run",1.0f,0,1,0.0f,NONE);
		PlayAnimationByName(actorInstance,"runstop",1.0f,0,1,0.0f,NONE);
		PlayAnimationByName(actorInstance,"agitated",1.0f,0,1,0.0f,NONE);
		ChangeState(actorInstance,STATE_RUNON);

		fade->SetCircleFadeTo(0, 0);
		player->camera->fade.StartFadeUp(FADETYPE_CIRCLE, 0.5f);

		// TP: if entering level in police box
		if (IN_HUB)
		{
			switch(map.lastScene)
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
				if (EnterSceneUsingPoliceBox(actorInstance,map.lastScene))
				{
					ChangeState(actorInstance,STATE_ENTERINGPHONEBOX);
				}
				break;
			}
		}
		break;
	case STATE_RUNON:
		noExitState = false;

	// PP: dirty, inefficient, moveme
#define STATE_RUNON_DURATION	(ANIMTIME(baFindAnimSegment(&player->actorInstance->actorInstance, "run")) \
								+ANIMTIME(baFindAnimSegment(&player->actorInstance->actorInstance, "runstop")) \
								+ANIMTIME(baFindAnimSegment(&player->actorInstance->actorInstance, "agitated")))

		musicSystem.RequestMood(0, STREAMMOOD_NORMAL/*, STATE_RUNON_DURATION*/);// PP: fadetime temp test

		if (!baIsActorAnimPlaying(&actorInstance->actorInstance,actorInstance->actorInstance.rootNodeInstance,TRUE))
		{
			ClearStateChangeRequest(actorInstance);
			if(RequestMorronicMessage(MEVENT_ENTER_1))
			{
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance, "idle1", 1.0f, 1, 0, 0.1f, IDLE);
				ChangeState(actorInstance, STATE_WAITFORTEXT);
			}
			else
			{
				ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			}
		}

		if ((controller->mag1 > 0.3f)||(controller->crossDebounceChannel->value)||(controller->triangleDebounceChannel->value) 
			||(controller->squareDebounceChannel->value) ||(controller->circleDebounceChannel->value))
		{
			ClearStateChangeRequest(actorInstance);
			if(RequestMorronicMessage(MEVENT_ENTER_1))
			{
				ChangeState(actorInstance, STATE_WAITFORTEXT);
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance, "idle1", 1.0f, 1, 0, 0.1f, IDLE);
			}
			else
			{
				ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			}
		}

		break;
	case STATE_LOSECOSTUME:
//		if((actorInstance->characterInfo->costumeChangeInfo = UpdateLoseCostumeEffect(actorInstance, actorInstance->characterInfo->costumeChangeInfo)) == NULL)
//		{
//			FlushAnimationQueue(actorInstance);
//			PlayAnimationByName(actorInstance, "idle1", 1.0f, 0, 0, 0.1f, IDLE);
//			ChangeState(actorInstance, actorInstance->actorStatus->currentMoveState);
//		}

		FlushAnimationQueue(actorInstance);
		PlayAnimationByName(actorInstance, "idle1", 1.0f, 0, 0, 0.1f, IDLE);
		ChangeState(actorInstance, actorInstance->actorStatus->currentMoveState);
		RemoveCostume(actorInstance);
		actorInstance->characterInfo->loseCostume = false;
		break;
	case STATE_MOVE:	

		if (RespondToStateExit(actorInstance)) 
		{
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}
		
		if (actorInstance->characterInfo->loseCostume)
		{
//			EnterLoseCostumeState(actorInstance);
			EnterSpinState(actorInstance);
			break;
		}

		musicSystem.RequestMood(0,STREAMMOOD_NORMAL);

		// check for not on ground for more that x secs
		if ((actorStatus->timeWithoutGroundCollision > 0.15f)&&(!controller->l3Channel->value))
		{
			// must now choose a fall
			// am I colliding with a wall, if so go to slide state
			if((actorPhysics->flags&PHYSFLAG_WALLCOLL)&&(actorInstance->actorStatus->timeWithoutGroundCollision > 0.3f)
				&&((!actorPhysics->coll)||(actorPhysics->lastAverageNormal[Y]>0.4f)))
			{
				// flush anim ready for move state
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance,"slide",0.1f,1,0,0.1f,JUMP);
				actorInstance->actorAnimation->useMotionBone = FALSE;
				ZEROVECTOR(actorInstance->actorBehaviour->physics.velocity);
				if (actorStatus->soundHandle[0]==-1)
				{
					actorStatus->soundHandle[0] = PlaySample("taz_gen_08a.wav",255,actorInstance->actorInstance.position);
				}
				ChangeState(actorInstance,STATE_SLIDE);
				//-----------------STATE CHANGE-----------------
				break;
			}
			// if doing a medium height fall then do a fall animation
			if(actorInstance->characterInfo)
			{
				if(!actorInstance->characterInfo->powerUpInfo.hiccup)
				{
					if (GetHeightOfActor(actorInstance,0.0f) > METERTOUNIT(1.0f))
					{
						CalculateSpeedBeforeJump(actorInstance);	// fill in speed before jump
						PlayAnimationByName(actorInstance,"jumpfall",0.4f,1,0,0.1f,JUMP);
						actorInstance->actorAnimation->useMotionBone = FALSE;
						bmVectorCopy(actorStatus->positionBeforeJump,actorInstance->actorInstance.position);
						ChangeState(actorInstance,STATE_JUMP);
						//-----------------STATE CHANGE-----------------
						break;
					}
				}
			}
		}
		// check for bite attack
		if (controller->triangleDebounceChannel->value)
		{
			DoSpitMove(actorInstance);
			//-----------------STATE CHANGE-----------------
			break;
		}
		// check for scare move
		if (controller->squareDebounceChannel->value)
		{
			if (actorInstance->characterInfo->currentBurp == 1)
			{
				actorInstance->characterInfo->currentBurp = 0;
				SuperBurp(&gameStatus.player[actorInstance->playerNo]);
				CreateSuperBurpBlastRadius(actorInstance);
				cameraInfo->quakeOffsetLevel = 0.2f;
				cameraInfo->tiltOffsetLevel = RAD(10.0f);
				cameraInfo->flags |= CAMERA_QUAKEFADE;
				PlayAnimationByName(actorInstance,"superbelch",1.0,0,0,0.2f,ATTACK);
				PlaySample("burpcanuse.wav",255,actorInstance->actorInstance.position);
				RequestRumbleEffect(controller, 255, 0.0f, 1.0f, 2.0f, 0.0f, WAVEFORM_RANDOM);
			}
			else
			{
				if (actorInstance->characterInfo->costume!=-1)
				{
					switch(actorInstance->characterInfo->costume)
					{
					case COSTUME_RAPPA:
						//musicSystem.FadeDownAudio(0, 0, 0.5f, 0, 0);

						if (characterSounds.daffy==-1)
						{
							CallbackWhenSampleFinished((characterSounds.daffy=PlaySample("scratchattack.wav",255,actorInstance->actorInstance.position)),CharacterSoundsFinishedCallback,&characterSounds.daffy);	
						}
						PlayAnimationByName(actorInstance,"rapperghettoblast",1.0f,0,0,0.2f,ATTACK);
						// TP: use burp radius
						cameraInfo->quakeOffsetLevel = 0.1f;
						cameraInfo->tiltOffsetLevel = RAD(5.0f);
						cameraInfo->flags |= CAMERA_QUAKEFADE;

						actorInstance->characterInfo->timer = 0.0f;
						ChangeState(actorInstance,STATE_RAPPERATTACK);
//						CreateRapperBlast(actorInstance);
						break;
#ifndef CONSUMERDEMO
					case COSTUME_NINJA:

						// PP: (for CHEAT_TIMESLICENINJAKICK) unlatch the timeslice and reset its delay timer
						VFX_timesliceLatch=false;
						VFX_timesliceTimer=0.0f;
						
						// PP: initialise kick flags ready for the new kick
						actorInstance->characterInfo->costumeFlags=0;

						PlaySample("swipe.wav", 255);

						// PP: "hi-ya!"
						characterSounds.taz=PlayTrackingSample("tazninja_2.wav", 255, actorInstance->actorInstance.position);

						PlayAnimationByName(actorInstance,"ninjakick",1.0f,0,0,0.2f,ATTACK);
						ChangeState(actorInstance,STATE_NINJAKICK);

						break;
					case COSTUME_SKATER:
						PlaySample("skateboard.wav", 255);
						PlayAnimationByName(actorInstance,"skatecharge",1.0f,0,0,0.2f,ATTACK);
						ChangeState(actorInstance,STATE_SKATECHARGE);
						break;
					case COSTUME_SNOWBOARD:
						PlayAnimationByName(actorInstance,"throwsnowball",1.0f,0,0,0.2f,ATTACK);
						ChangeState(actorInstance,STATE_SNOWBOARDATTACK);
						break;
					case COSTUME_SURFER:
						PlayAnimationByName(actorInstance,"surfboardwallop",1.0f,0,0,0.2f,ATTACK);
						ChangeState(actorInstance,STATE_SURFBOARDATTACK);
						break;
					case COSTUME_SWAT:
						if (STINKYCHEESE::NumActive()) 
						{
							// TP: there is already cheese active so play different anim
							PlayAnimationByName(actorInstance,"idle6",1.0f,0,0,0.2f,ATTACK);
							ChangeState(actorInstance,STATE_CHEESYATTACK);
						}
						else
						{
							PlayAnimationByName(actorInstance,"cheesyattack",1.0f,0,0,0.2f,ATTACK);
							ChangeState(actorInstance,STATE_CHEESYATTACK);
						}
						break;
					case COSTUME_WERETAZ:
						PlayAnimationByName(actorInstance,"werewolfbiteclaw",1.0f,0,0,0.2f,ATTACK);
						PlaySample("swipe3.wav", 255, actorInstance->actorInstance.position, 0);
						ChangeState(actorInstance,STATE_WEREWOLFATTACK);
						break;
					case COSTUME_REINDEER:
						PlayAnimationByName(actorInstance,"reindeercharge",1.0f,0,0,0.2f,ATTACK);
						ChangeState(actorInstance,STATE_WEREWOLFATTACK);
						break;
					case COSTUME_COWBOY:
						PlayAnimationByName(actorInstance,"cowboyshootfinger",1.0f,0,0,0.2f,ATTACK);
						ChangeState(actorInstance,STATE_COWBOYSHOOT);
						break;
					case COSTUME_INDY:
						PlayAnimationByName(actorInstance,"throwhelmet",1.0f,0,0,0.2f,ATTACK);
						ChangeState(actorInstance,STATE_INDYROLL);
						break;
					case COSTUME_TARZAN:
						PlayAnimationByName(actorInstance,"tarzanyell",1.0f,0,0,0.2f,ATTACK);
						ChangeState(actorInstance,STATE_TAZANYELL);

						StopChannel(&actorStatus->soundHandle[0]);
						CallbackWhenSampleFinished(actorStatus->soundHandle[0] = PlaySample("taztarzan_1.wav",255), ClearSoundHandle, (void*)&actorStatus->soundHandle[0]);
						actorStatus->soundHandle[0] = PlaySample("taztarzan_1.wav",255);
						break;
#endif
					}
				}
				else
				{
//					if(characterSounds.taz == -1)
//					{
//						CallbackWhenSampleFinished((characterSounds.taz=PlaySample("tazrant1.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.taz);
//					}

					PlaySample("tazrant1.wav",255);
					cameraInfo->quakeOffsetLevel = 0.1f;
					cameraInfo->tiltOffsetLevel = RAD(5.0f);
					cameraInfo->flags |= CAMERA_QUAKEFADE;
					PlayAnimationByName(actorInstance,"rant2",1,0,0,0.2f,ATTACK);
					ChangeState(actorInstance,STATE_SCARE);
				}
			}
			//-----------------STATE CHANGE-----------------
			break;
		}
		// check for player pressing jump
		if(CheckForJump(player)) break;

		// check for spin mode
		if(CheckForSpin(player)) break;

		// check for tiptoe move
		if (controller->l2Channel->value)
		{
			PlayAnimationByName(actorInstance,"tiptoeidle1",1,1,0,0.1f,TIPTOE);
			ChangeState(actorInstance,STATE_TIPTOE);
			//-----------------STATE CHANGE-----------------
			break;
		}

		// TP: check for entering look around mode and that no camera case is playing
		if ((controller->r2Channel->value)&&(cameraInfo->cameraCase==NULL)&&(map.sceneNumber != SCENE_ZOOBOSS)&&(map.sceneNumber != SCENE_CITYBOSS)&&(map.sceneNumber != SCENE_WESTBOSS)&&(map.sceneNumber != SCENE_TAZPREBOSS)&&(map.sceneNumber != SCENE_TAZBOSS)&&(map.sceneNumber < SCENE_MUSEUMVEHICLERACE))
		{
			FlushAnimationQueue(actorInstance);
			PlayAnimationByName(actorInstance,"idle1",1.0f,1,0,0.1f,NONE);
			player->camera->flags |= CAMERA_LOOKAROUND;
			ChangeState(actorInstance,STATE_ENTERLOOKAROUND);
			break;
		}

		if(actorInstance->actorBehaviour != NULL)// PP: check we have behaviour first
		{
			//check for surface terrain
			if(actorInstance->actorBehaviour->physics.currentTerrain & TERRAIN_ICE)
			{
				ChangeState(actorInstance, STATE_ONICE);
				// TP: RequestStateChange(actorInstance, STATE_ONICE);
				SetActorFriction(actorInstance->actorBehaviour, METERTOUNIT(0.0f));
				break;
			}
		}

		// check for idle move// PP: if not in a boss game
		if(!IN_BOSS_GAME)
		{
			animationToPlay = ChooseIdleAnimationForTaz(controller, actorInstance, cameraInfo);
			if(animationToPlay)
			{
				FlushAnimationQueue(actorInstance);
				PlayAnimationBySegmentPointer(actorInstance, animationToPlay, 1.0f, 0, 0, 0.1f, IDLE);
				ChangeState(actorInstance,STATE_IDLE);
				//-----------------STATE CHANGE-----------------
				break;
			}
		}

		// check if player is trying to move
		if (controller->mag1 > 0.2f)
		{
			// process character movement
			if (GetPriorityOfCurrentAnimation(actorInstance) <= MOVE)
			{
				CharacterRespondToMovementInput(controller,cameraInfo,actorInstance);
			}
		}

		// find the appropriate movement animation to play
		animationToPlay = ChooseMovementAnimationForTaz(controller,actorInstance);
		if (actorInstance->actorStatus->timeInRunAnimation > 0.5f) // if running for more that 0.5 secs
		{
			// check to see if going from a run to a stand
			if (animationToPlay==baFindAnimSegment(&actorInstance->actorInstance, "idle1"))
			{
				if (CheckIfPlayingAnimation(actorInstance,"run"))
				{
					PlaySample("skidstop.wav",255,actorInstance->actorInstance.position);
					PlayAnimationByName(actorInstance,"runstop",1.5f,0,0,0.1f,SLIDESTOP);
					ChangeState(actorInstance, STATE_SKIDSTOP);
					// TP: RequestStateChange(actorInstance, STATE_SKIDSTOP);
					//-----------------STATE CHANGE-----------------
					break;
				}
			}
		}
		if (animationToPlay==baFindAnimSegment(&actorInstance->actorInstance, "idle1"))
		{
			if (actorStatus->timeSinceLastTiltingCollision < 0.15f)
			{
				// TP: touching tilting object so wobble
				animationToPlay = baFindAnimSegment(&actorInstance->actorInstance, "wobble");
			}
		}

		// play new animation
		if (CurrentAnimation(actorInstance)!=animationToPlay)	// am i already playing this anim
			PlayAnimationBySegmentPointer(actorInstance,animationToPlay,1.0f,1,0,0.1f,MOVE);


		if(gameStatus.multiplayer.numScreens == 1)// PP: don't want any complications yet - stick to camera[0] only
		{
			if(CheatActive(CHEAT_DIGITALWATCH))
			{
				// PP: can't do it while in a camera case
				if(camera[0].cameraCase == NULL)
				{
					// PP: Check for the magic button combo that lets you look at your watch
					// PP: "Taz:Wanted - a game and a clock in one!"™
					CheckForWatchLook();
				}
			}
		}

		break;
	case STATE_EAT:
		// TP: Eating
		if (RespondToStateExit(actorInstance))
		{
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}
		if(!CheckIfPlayingAnimation(actorInstance,"eatlargeobject")&&!CheckIfPlayingAnimation(actorInstance, "eatlikeananimal"))
		{
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			//-----------------STATE CHANGE-----------------
			break;
		}
		break;
	case STATE_SPIT:
		if (RespondToStateExit(actorInstance))
		{
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}
		if (!CheckIfPlayingAnimation(actorInstance,"attack1"))
		{
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			//-----------------STATE CHANGE-----------------
			break;
		}
		break;
	case STATE_ENTERLOOKAROUND:
		if (RespondToStateInit(actorInstance))
		{
			SetCameraSmooth(player->camera, 0.15f, 0.15f);
			SetCameraCollision(player->camera, false);
			player->camera->flags &= ~CAMERA_SMARTCAM;
		}

		ProcessCameraEnterLookAround(player);

		break;
	case STATE_LOOKAROUND:
		// TP: Respond to exit requests and exit on cameracase also
		if (RespondToStateExit(actorInstance)||cameraInfo->cameraCase) 
		{
			// TP: sort out flags
			player->actorInstance->flags &= ~ACTORFLAG_DONTDRAW;
			player->camera->flags &= ~(CAMERA_LOOKAROUND|CAMERA_BLINKEYES|CAMERA_NOCOLLISION);
			gameStatus.chrisCam = TRUE;

			// TP: take care of blinking eyes
			if (player->actorInstance->characterInfo->blinkEyes)
			{
//				delete player->actorInstance->characterInfo->blinkEyes;
				SAFE_DELETE(player->actorInstance->characterInfo->blinkEyes);
			}
			break;
		
		}

		ProcessCameraLookAround(player);


		break;
	case STATE_MOVESQUASHED:
		if (RespondToStateInit(actorInstance))
			PlaySample("thud2.wav", 255);

		if (RespondToStateExit(actorInstance)) 
		{
			actorInstance->actorStatus->timeInSquashedState = 0.0f;
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}

		// check if player is trying to move
		if (controller->mag1 > 0.2f)
		{
			// process character movement
			if(!CheckIfPlayingAnimation(actorInstance, "recover22a"))
			{
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance, "recover22a", 1.0f, 1, 0, 0.1f, MOVE);
			}
			CharacterRespondToMovementInput(controller,cameraInfo,actorInstance);
		}
		else
		{
			FlushAnimationQueue(actorInstance);
			PlayAnimationByName(actorInstance, "recover21a",1.0f, 1, 0, 0.1f, REACT);
		}
		//check for button press
		if(actorInstance->flags & ACTORFLAG_ALLOWUNSQUASHMOVE)
		{
			if((controller->crossDebounceChannel->value)||(controller->circleDebounceChannel->value)
				||(controller->triangleDebounceChannel->value)||(controller->squareDebounceChannel->value)
				||((actorInstance->actorStatus->timeInSquashedState+=fTime) > 5.0f))
			{
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance,"recover22c",1.0f,0,0,0.1f,REACT);
/*				if(actorInstance->characterInfo)
				{
					if(actorInstance->characterInfo->powerUpInfo.chilliPepper)
					{
						ChangeState(actorInstance,STATE_CHILLIPEPPER);
					}
					else
					{
						ChangeState(actorInstance,STATE_MOVE);
					}
					break;
				}
				else
				{
					ChangeState(actorInstance,STATE_MOVE);
				}*/
				actorInstance->actorStatus->timeInSquashedState = 0.0f;
				ChangeState(actorInstance,STATE_SQUASHED);
				//-----------------STATE CHANGE-----------------
				actorInstance->flags &= ~ACTORFLAG_ALLOWUNSQUASHMOVE;
			}
		}
		break;
	case STATE_DEBUGMOVE:
		// check for reset
		noExitState = false;
		if(controller->dPadYDebounceChannel->value > 0)
		{
			ResetCharacterAfterJump(actorInstance);
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_NOGRAVITY;
			actorPhysics->flags &= ~PHYSFLAG_WALLGROUNDCOLLFIX;
			actorPhysics->flags |= PHYSFLAG_WALLVELCORRECTION;
			break;
		}

		// check if player is trying to move
//		if (controller->mag1 > 0.2f)
		{
			// process character movement
			if (GetPriorityOfCurrentAnimation(actorInstance) <= MOVE)
			{
				CharacterRespondToDebugMovementInput(controller,cameraInfo,actorInstance);
			}
		}
		// find the appropriate movement animation to play
		animationToPlay = ChooseMovementAnimationForTaz(controller,actorInstance);
		if (actorInstance->actorStatus->timeInRunAnimation > 0.5f) // if running for more that 0.5 secs
		{
			// check to see if going from a run to a stand
			if (animationToPlay==baFindAnimSegment(&actorInstance->actorInstance, "idle1"))
			{
				if (CheckIfPlayingAnimation(actorInstance,"run"))
				{
					PlaySample("skidstop.wav",255,actorInstance->actorInstance.position);
					PlayAnimationByName(actorInstance,"runstop",1.5f,0,0,0.1f,SLIDESTOP);
					ChangeState(actorInstance, STATE_SKIDSTOP);
					break;
					// TP: RequestStateChange(actorInstance, STATE_SKIDSTOP);
				}
			}
		}
		// play new animation
		if (CurrentAnimation(actorInstance)!=animationToPlay)	// am i already playing this anim
			PlayAnimationBySegmentPointer(actorInstance,animationToPlay,1,1,0,0.2f,MOVE);
		break;

	case STATE_NET:
		ProcessCharacterInNet(actorInstance);
		break;

	case STATE_WATERSLIDE:
		if (RespondToStateInit(actorInstance)) 
		{
			switch (map.sceneNumber)
			{
			case SCENE_AQUA:	
								if (actorStatus->soundHandle[0]==-1)
								{
									actorStatus->soundHandle[0] = PlaySample("loop\\watershute.wav", 255, 0);
								}
								break;
			}
		}

		if (RespondToStateExit(actorInstance)) 
		{
//			ResetCharacterAfterJump(actorInstance);
//			SetWorldGravity(NORMAL_WORLD_GRAVITY);
//			SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
//			SetActorFriction(actorInstance->actorBehaviour,0.0f);
//			actorInstance->actorAnimation->useMotionBone = TRUE;
//			RespondToStateExitPost(actorInstance,actorStatus);

			worldGravity = NORMAL_WORLD_GRAVITY;
			// TP: out-of-date actorInstance->actorInstance.position[Y] -= METERTOUNIT(0.3895f);
			actorInstance->actorBehaviour->collSphere.offset[1] = 120.0f;
			// TP: out-of-date actorInstance->actorBehaviour->collSphere.radius = METERTOUNIT(0.5f);
			actorInstance->actorAnimation->useMotionBone = TRUE;
			SetActorMinFricAngle(actorInstance->actorBehaviour, 0.0f);
			actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLGROUNDCOLLFIX;
			actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_WALLVELCORRECTION;
//			SetActorOrientation(actorInstance, actorInstance->actorStatus->waterResetQuaternion);
			actorInstance->flags &= ~ACTORFLAG_DONTDRAW;
			actorInstance->flags &= ~ACTORFLAG_NOVELOCITY;

			StopChannel(&actorStatus->soundHandle[0]);
			gameStatus.player[actorInstance->playerNo].camera->flags |= CAMERA_PADCONTROL;
			SetCameraSmooth(gameStatus.player[actorInstance->playerNo].camera, defaultPosSmooth, defaultLookatSmooth);
			// TP: needed this for some reason to fix a problem with the sewer tube in museum
			ClearCurrentAnimationPriority(actorInstance);

			RespondToStateExitPost(actorInstance,actorStatus);

			break;
		}

		EnableCameraAngleAboveHorizontal(cameraInfo,0);

		// TP: need the extra time to smooth out the sewer in the museum
		if (((map.sceneNumber==SCENE_AQUA)&&(actorInstance->actorStatus->timeWithoutWaterSlideCollision > 0.3f))||
			((map.sceneNumber!=SCENE_AQUA)&&(actorInstance->actorStatus->timeWithoutWaterSlideCollision > 1.0f)))
		{
			TBVector offset;

			SETVECTOR(offset, 0.0f, 120.0f, 0.0f, 1.0f);

			// I have slid onto proper ground
			// flush anim ready for move state
//			ResetCharacterAfterJump(actorInstance);
//			SetWorldGravity(NORMAL_WORLD_GRAVITY);
//			SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
//			SetActorFriction(actorInstance->actorBehaviour,0.0f);
//			actorInstance->actorAnimation->useMotionBone = TRUE;
			worldGravity = NORMAL_WORLD_GRAVITY;
			// TP: out-of-date actorInstance->actorInstance.position[Y] -= METERTOUNIT(0.3895f);
			actorInstance->actorBehaviour->collSphere.offset[1] = 120.0f;
			// TP: out-of-date actorInstance->actorBehaviour->collSphere.radius = METERTOUNIT(0.5f);
			actorInstance->actorAnimation->useMotionBone = TRUE;
			SetActorMinFricAngle(actorInstance->actorBehaviour, 0.7f);
			actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLGROUNDCOLLFIX;
			actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_WALLVELCORRECTION;
//			SetActorOrientation(actorInstance, actorInstance->actorStatus->waterResetQuaternion);
			actorInstance->flags &= ~ACTORFLAG_DONTDRAW;
			actorInstance->flags &= ~ACTORFLAG_NOVELOCITY;
			ResetCharacterAfterJump(actorInstance);
			FlushAnimationQueue(actorInstance);
			PlayAnimationByName(actorInstance, "getup", 1.0f, 0, 0, 0.1f, NONE);
			PlaySample("taz getup from water slide",255,actorInstance->actorInstance.position);
			StopChannel(&actorStatus->soundHandle[0]);
			// TP: SetActorCollSphere(actorInstance->actorBehaviour, offset, METERTOUNIT(0.5f));
			gameStatus.player[actorInstance->playerNo].camera->flags |= CAMERA_PADCONTROL;
			SetCameraSmooth(gameStatus.player[actorInstance->playerNo].camera, defaultPosSmooth, defaultLookatSmooth);
			ChangeState(actorInstance,STATE_GETUPFROMSLIDE);
			//-----------------STATE CHANGE-----------------
			break;
		}

		// TP: Lets keep the speed sensible, needs to be a little higher for aqua to make the slide work ok.
		if (map.sceneNumber==SCENE_AQUA)
		{
			checkSpeedLimit(&actorInstance->actorBehaviour->physics, 4000);
		}
		else
		{
			checkSpeedLimit(&actorInstance->actorBehaviour->physics, 3000);
		}

		UpdateSlidePitch(actorInstance, RAD(-40.0f));
		CharacterRespondToWaterSlideMovementInput(controller,cameraInfo,actorInstance);
//		CharacterRespondToGloverBallMovementInput(controller,cameraInfo,actorInstance);

		break;
	case STATE_ONICE:
		if(RespondToStateExit(actorInstance))
		{
			SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
			actorInstance->actorAnimation->useMotionBone = TRUE;

			FlushAnimationQueue(actorInstance);
			ResetCharacterAfterJump(actorInstance);
			//-----------------STATE CHANGE-----------------
			break;
		}
		if(actorInstance->actorStatus->timeWithoutIceCollision > 0.3f)
		{
			SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
			actorInstance->actorAnimation->useMotionBone = TRUE;

			FlushAnimationQueue(actorInstance);
			ResetCharacterAfterJump(actorInstance);
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			//-----------------STATE CHANGE-----------------
			break;
		}

		// PP: Check if the player is trying to jump; start the jump if so
		if(CheckForJump(player))
		{
			break;
		}

		//check for player pressing spin
		if(controller->circleDebounceChannel->value)
		{
			PlaySample("spinslip.wav",255,actorInstance->actorInstance.position);
			FlushAnimationQueue(actorInstance);
			PlayAnimationByName(actorInstance, "spinslip", 1.0f, 0, 0, 0.1f, REACT);
			ChangeState(actorInstance,STATE_SPINONICE);
			//-----------------STATE CHANGE-----------------
			break;
		}
		// find the appropriate movement animation to play
		if(!CheckIfPlayingAnimation(actorInstance, "slipslide"))
		{
			animationToPlay = ChooseMovementAnimationForTaz(controller,actorInstance);
			if(animationToPlay == baFindAnimSegment(&actorInstance->actorInstance, "idle1"))
			{
				animationToPlay = baFindAnimSegment(&actorInstance->actorInstance, "surf");
			}
			// play new animation
			if (CurrentAnimation(actorInstance)!=animationToPlay)	// am i already playing this anim
			{
				FlushAnimationQueue(actorInstance);
				PlayAnimationBySegmentPointer(actorInstance,animationToPlay,1,1,0,0.1f,MOVE);
			}
		}

		CharacterRespondToMovementInputOnIce(controller, cameraInfo, actorInstance);
		MatchGroundOrientation(actorInstance);
		break;
	case STATE_SPINONICE:
		if(RespondToStateExit(actorInstance))
		{
			ResetCharacterAfterJump(actorInstance);
			actorPhysics->flags &= ~PHYSFLAG_WALLVELCORRECTION;
			actorInstance->actorAnimation->useMotionBone = TRUE;
			SetActorMinFricAngle(actorInstance->actorBehaviour, 0.7f);
			RespondToStateExitPost(actorInstance, actorStatus);
			break;
		}
		if(!CheckIfPlayingAnimation(actorInstance, "spinslip"))
		{
			FlushAnimationQueue(actorInstance);
			PlayAnimationByName(actorInstance, "getup", 1.0f, 0, 0, 0.1f, NONE);
			ResetCharacterAfterJump(actorInstance);
			actorPhysics->flags &= ~PHYSFLAG_WALLVELCORRECTION;
			actorInstance->actorAnimation->useMotionBone = TRUE;
			SetActorMinFricAngle(actorInstance->actorBehaviour, 0.7f);
			// TP: PlaySample("taz getup from slide",255,actorInstance->actorInstance.position);
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);// TP: STATE_GETUPFROMSLIDE
		}
		break;
	case STATE_SKIDSTOP:
		if (RespondToStateExit(actorInstance)) 
		{
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}
		if(CheckIfPlayingAnimation(actorInstance, "runstop"))
		{
			CreateImpactClouds(actorInstance->actorInstance.position, gameStatus.windVelocity, NULL, METERTOUNIT(0.15f), 2.0f, 0.0f, SMOKE_SKIDSTOP);
			cloudCounter += 1/(fps*fps);
		}
		if(!CheckIfPlayingAnimation(actorInstance, "runstop"))
		{
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			//-----------------STATE CHANGE-----------------
			cloudCounter = 0.0f;
		}
		break;
	case STATE_TIPTOE:
		if (RespondToStateExit(actorInstance)) 
		{
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}
		
		musicSystem.RequestMood(0,STREAMMOOD_SLOW);

		if (!controller->l2Channel->value)
		{
			FlushAnimationQueue(actorInstance);
			PlayAnimationByName(actorInstance,"idle1",1.0f,1,0,0.1f,MOVE);

			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			//-----------------STATE CHANGE-----------------
			break;
		}
		// check if player is trying to move
		if (controller->mag1 > 0.2f)
		{
			// process character movement
			if (GetPriorityOfCurrentAnimation(actorInstance) <= TIPTOE)
			{
				CharacterRespondToMovementInput(controller,cameraInfo,actorInstance);
			}
		}
		// find the appropriate movement animation to play
		if (controller->mag1 > 0.2f)
		{
			// tiptoe
			animationToPlay = baFindAnimSegment(&actorInstance->actorInstance, "tiptoe");
		}
		else
		{
			animationToPlay = baFindAnimSegment(&actorInstance->actorInstance, "tiptoeidle1");
		}
		// check for player pressing jump
		if (controller->crossDebounceChannel->value)
		{
			if (PlayAnimationByName(actorInstance,"jumprise",0.6f,0,0,0.1f,JUMP))	// If I could play jump then
			{
				CalculateSpeedBeforeJump(actorInstance);	// fill in current speed before jump
				actorInstance->actorInstance.position[Y] += 1.0f;	// bodge to avoid sticking to ground
				// zero vertical speed before adding jump
				actorInstance->actorBehaviour->physics.velocity[Y] = 0.0f;
				tvec[X] = 0.0f;
				tvec[Y] = 1.0f;
				tvec[Z] = 0.0f;
				tvec[W] = 1.0f;
			
				if (player->actorInstance->actorStatus->multiplayer.effect == PLAYEREFFECTSLOMO)
					AddActorSpeed( actorInstance->actorBehaviour, tvec, METERTOUNIT(3.0f));//METERTOUNIT(6.324f)
				else
					AddActorSpeed( actorInstance->actorBehaviour, tvec, METERTOUNIT(7.0f));//7

				bmVectorCopy(startVel, actorInstance->actorBehaviour->physics.velocity);

				PlayAnimationByName(actorInstance,"jumpfall",0.4f,1,1,0.2f,JUMP);
				actorPhysics->flags &= ~PHYSFLAG_WALLVELCORRECTION;
				actorInstance->actorAnimation->useMotionBone = FALSE;

				switch (bmRand()%2)
				{
				case 0:	PlaySample("tazjump_1.wav",255,actorInstance->actorInstance.position);	break;
				case 1:	PlaySample("tazjump_2.wav",255,actorInstance->actorInstance.position);	break;
				}
				
				bmVectorCopy(actorStatus->positionBeforeJump,actorInstance->actorInstance.position);
				
				ChangeState(actorInstance,STATE_JUMP);
				//-----------------STATE CHANGE-----------------
				break;
			}
		}
		// play new animation
		if (CurrentAnimation(actorInstance)!=animationToPlay)	// am i already playing this anim
			PlayAnimationBySegmentPointer(actorInstance,animationToPlay,1,1,0,0.1f,TIPTOE);
		break;
	case STATE_JUMP:
		HandleJumpState(player);
		break;
	case STATE_SLIDE:
		if (RespondToStateInit(actorInstance))
		{
			// TP: use positionBeforeJump temporarly to store position before slide
			bmVectorCopy(actorInstance->actorStatus->stuckCheck,actorInstance->actorInstance.position);
		}

		if (RespondToStateExit(actorInstance)) 
		{
			StopChannel(&actorStatus->soundHandle[0]);
			actorInstance->actorAnimation->useMotionBone = TRUE;
			SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}

		// Check if character has slid onto ground yet
		if (actorPhysics->coll)
		{
			if (actorPhysics->flags&PHYSFLAG_GROUNDCOLL)
			{
				SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
				//SetActorFriction(actorInstance->actorBehaviour, 0.0f);
				FlushAnimationQueue(actorInstance);
				actorInstance->actorAnimation->useMotionBone = TRUE;
				//UpdateSlidePitch(actorInstance, RAD(-40.0f));
				ResetCharacterAfterJump(actorInstance);
				StopChannel(&actorStatus->soundHandle[0]);

				ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
				//-----------------STATE CHANGE-----------------
				break;
			}
		}
		if (actorStatus->timeWithoutWallCollision > 0.3f)
		{
			// have slid off into fall
			SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
			SetActorFriction(actorInstance->actorBehaviour,0.0f);
			CalculateSpeedBeforeJump(actorInstance);	// fill in speed before jump
			PlayAnimationByName(actorInstance,"jumpfall",0.4f,1,0,0.1f,JUMP);
			ResetCharacterAfterJump(actorInstance);
			actorInstance->actorAnimation->useMotionBone = FALSE;
			StopChannel(&actorStatus->soundHandle[0]);
			ChangeState(actorInstance,STATE_JUMP);
			//-----------------STATE CHANGE-----------------
			break;
		}
		// Process slide control
		// get actor speed
		bmVectorCopy(tempVector, actorInstance->actorBehaviour->physics.lastAverageNormal);
		tempVector[Y] = 0.0f;
		tempFloat = bmVectorLen(tempVector);
		if (tempFloat)//>(METERTOUNIT(0.1)/60))
		{
			// rotate actor in direction of slide
			tempFloat = GetAngleAroundYFromVector(actorInstance->actorBehaviour->physics.lastAverageNormal);
			tempFloat = Aabs(-tempFloat);
			// rotate angle 180 degrees because characters face in -Z
			CharacterRotateToAngle(actorInstance,0,tempFloat,0,0.2f);
		}

		// TP: Better do check for infernal slide, for example if Taz falls in the crack between three trees you can't escape
		// TP: if (have not move significantly in last second) then try quick spin to solve the problem
		if (actorStatus->timeInCurrentState > 1.0f)
		{
			// TP: see if player has moved much
			if (bmVectorDistance(actorInstance->actorInstance.position,actorStatus->stuckCheck) < METERTOUNIT(0.25f))
			{
				bmVectorSet(actorInstance->actorBehaviour->physics.velocity, 0.0f, 0.0f, 0.0f, 1.0f);
				// TP: Haven't moved enough, must be stuck
				EnterSpinState(actorInstance);
				break;
			}
			bmVectorCopy(actorStatus->stuckCheck,actorInstance->actorInstance.position);
			actorStatus->timeInCurrentState = 0.0f;
		}

		UpdateSlidePitch(actorInstance, RAD(-40.0f));
		CharacterRespondToSlideMovementInput(controller,cameraInfo,actorInstance);
		break;
	case STATE_GETUPFROMSLIDE:
		if (RespondToStateExit(actorInstance)) 
		{
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}
//		if((!CheckIfPlayingAnimation(actorInstance, "getup"))&&(!CheckIfPlayingAnimation(actorInstance, "getup")))
		if (!CheckIfPlayingAnimation(actorInstance,NULL))
		{
			FlushAnimationQueue(actorInstance);
			PlayAnimationByName(actorInstance, "idle1", 1.0f, 1, 0, 0.1f, NONE);

			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			//-----------------STATE CHANGE-----------------
		}
		break;
	case STATE_GETUPFROMWATER:
		if (RespondToStateExit(actorInstance)) 
		{
			actorInstance->actorAnimation->flags &= ~ANIMFLAG_WATERSPOUT;
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}
		//check if shandkey want to move
		if(controller->mag1 > 0.2f)
		{
			FlushAnimationQueue(actorInstance);
			PlayAnimationByName(actorInstance, "idle1", 1.0f, 1, 0, 0.1f, NONE);
			DestroyInjuryStars(actorInstance);
			actorInstance->actorAnimation->useMotionBone = TRUE;
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			break;
		}
		if(actorInstance->actorAnimation->flags & ANIMFLAG_WATERSPOUT)
		{
			// CMD: calculate position, inc offset from node 'snoutstem'
			if((actorInstance->actorAnimation->waterspoutClock -= fTime) < 0.0f)
			{
				actorInstance->actorAnimation->waterspoutClock = 0.0f;
				actorInstance->actorAnimation->flags &= ~ANIMFLAG_WATERSPOUT;
			}
			else
			{
				if(node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "snoutstem"))
				{
					baGetNodesWorldPosition(&actorInstance->actorInstance, node, position);
	
					fOne = METERTOUNIT(0.02f);
					fTwo = METERTOUNIT(0.0f);
					fThree = 0.75f;
					for(i = 10;i > 0;--i)
					{
						// CMD: chose randomish velocity for spout
						bmVectorSet(velocity, METERTOUNIT(UtilGenerateRandomNumber(0.3f, -0.3f)), METERTOUNIT(UtilGenerateRandomNumber(2.5f, 2.0f)),
												METERTOUNIT(UtilGenerateRandomNumber(0.3f, -0.3f)), 1.0f);
	
						baCreateExtra(squishExtraSystem, position, velocity, &fOne, &fTwo, &fThree);
					}
				}
			}
		}
		if((!CheckIfPlayingAnimation(actorInstance, "shakedry"))
			&&(!CheckIfPlayingAnimation(actorInstance, "shakedry2"))
			&&(!CheckIfPlayingAnimation(actorInstance, "agitated")))
		{
			actorInstance->characterInfo->cartoonScale.stage = CARTOONSCALE_STAGEONE;
			actorInstance->characterInfo->cartoonScale.time = 0.0f;
			actorInstance->characterInfo->cartoonScale.maxScale = 2.0f;
			FlushAnimationQueue(actorInstance);
			PlayAnimationByName(actorInstance, "idle1", 1.0f, 1, 0, 0.1f, NONE);
			DestroyInjuryStars(actorInstance);
			actorInstance->actorAnimation->useMotionBone = TRUE;
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
		}
		//set fade details
		if(fade->GetFadeState() != FADESTATE_OFF)
		{
			node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "snoutstem");
			if(node) 
			{
				bdSetIdentityObjectMatrix();
				baGetNodesWorldPosition(&actorInstance->actorInstance, node, worldSpace);
				worldSpace[3] = 1.0f;
				bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
				bdSetIdentityObjectMatrix();
				bdProjectVertices(screenSpace, worldSpace, 1);
				fade->SetCircleFadeTo(screenSpace[0], screenSpace[1]);
			}
		}
		break;
	case STATE_BIGFALL:
		if (RespondToStateExit(actorInstance)) 
		{
			// TP: make sure we get rid of the hiccup mode
			actorInstance->characterInfo->powerUpInfo.hiccupJumpState = false;

			SetCameraSmooth(camera,defaultPosSmooth,defaultLookatSmooth);
			SetDefaultCameraAngleAboveHorizontal(camera,CAMERAANGLE);
			ResetCharacterAfterJump(actorInstance);
			actorPhysics->flags |= PHYSFLAG_WALLVELCORRECTION;
			actorInstance->actorAnimation->useMotionBone = TRUE;
			actorStatus->positionBeforeJump[Y] = METERTOUNIT(1000.0f);
			RespondToStateExitPost(actorInstance,actorStatus);

			// PP: (for timeslice cheats) unlatch the timeslice
			VFX_timesliceLatch=false;

			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}

		// keep falling until ground hit
		if (actorPhysics->coll)
		{
			if (actorPhysics->flags&PHYSFLAG_GROUNDCOLL)
			{
				RequestRumbleEffect(controller, 255, 0.0f, 0.1f, 0.5f, 0.0f, WAVEFORM_NORMAL, 0.0f, 0.0f, 0);
				CreateImpactClouds(actorInstance->actorInstance.position, zeroVec, NULL, METERTOUNIT(0.2f), 2.0f, 0.0f, SMOKE_BIGFALL);
				RequestRumbleEffect(gameStatus.player[actorInstance->playerNo].controller, 200, 0.0f, 0.5f, 0.5f, 0.0f, WAVEFORM_NORMAL);
				// Have hit ground
				PlayAnimationByName(actorInstance,"recover3a",1,0,0,0,REACT);
				PlaySample("thud1.wav",255,actorInstance->actorInstance.position);
				CreateInjuryStars(10, actorInstance, "spinetop", starsOffset, starsBounds);
				PlaySample("taz recover from big fall",255,actorInstance->actorInstance.position);

				// TP: make sure we get rid of the hiccup mode
				actorInstance->characterInfo->powerUpInfo.hiccupJumpState = false;

				SetCameraSmooth(camera,defaultPosSmooth,defaultLookatSmooth);
				SetDefaultCameraAngleAboveHorizontal(camera,CAMERAANGLE);
				ResetCharacterAfterJump(actorInstance);
				actorPhysics->flags |= PHYSFLAG_WALLVELCORRECTION;
				actorInstance->actorAnimation->useMotionBone = TRUE;
				actorStatus->positionBeforeJump[Y] = METERTOUNIT(1000.0f);
				RespondToStateExitPost(actorInstance,actorStatus);

				// PP: (for timeslice cheats) unlatch the timeslice
				VFX_timesliceLatch=false;

				ChangeState(actorInstance,STATE_RECOVER);
				//-----------------STATE CHANGE-----------------
				break;
			}
			// Maybe there should be a test for a wall collision
		}

		// TP: Better do check for infernal slide, for example if Taz falls in the crack between three trees you can't escape
		// TP: if (have not move significantly in last second) then try quick spin to solve the problem
		if (actorStatus->timeInCurrentState > 1.0f)
		{
			// TP: see if player has moved much
			if (bmVectorDistance(actorInstance->actorInstance.position,actorStatus->stuckCheck) < METERTOUNIT(0.25f))
			{
				bmVectorSet(actorInstance->actorBehaviour->physics.velocity, 0.0f, 0.0f, 0.0f, 1.0f);
				// TP: Haven't moved enough, must be stuck
				EnterSpinState(actorInstance);
				break;
			}
			bmVectorCopy(actorStatus->stuckCheck,actorInstance->actorInstance.position);
			actorStatus->timeInCurrentState = 0.0f;
		}

		CharacterRespondToMovementWhileJumping(controller,cameraInfo,actorInstance,false);

		break;
	case STATE_RECOVER:
		if (RespondToStateExit(actorInstance)) 
		{
			// TP: ReduceCharactersHealth(actorInstance,1.0f);
			DestroyInjuryStars(actorInstance);
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}
		if ((!CheckIfPlayingAnimation(actorInstance,"recover3a"))
			&& (!CheckIfPlayingAnimation(actorInstance, "recover22c"))
			&& (!CheckIfPlayingAnimation(actorInstance, "react2"))
			&& (!CheckIfPlayingAnimation(actorInstance, "recover21b"))
			&& (!CheckIfPlayingAnimation(actorInstance, "electrorecover"))
			)
		{
			// CPW: DisableCameraBigFall(cameraInfo);
			// TP: ReduceCharactersHealth(actorInstance,0.5f);
			DestroyInjuryStars(actorInstance);

			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			//-----------------STATE CHANGE-----------------
			break;
		}
		break;
	case STATE_SQUASHED:
		{
			HandleSquashedState(player);
		}
		break;
	case STATE_BITE:
		if (RespondToStateExit(actorInstance)) 
		{
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}

		TazBite(actorInstance);	// check for the effects of a taz bite move

		// return to move when biteattack is finished
		if (!CheckIfPlayingAnimation(actorInstance,"attack1"))
		{
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			//-----------------STATE CHANGE-----------------
			break;
		}
		break;
	case STATE_SCARE:
		if (RespondToStateExit(actorInstance)) 
		{
			actorInstance->characterInfo->rumbleHandle = FlushRumbleQueueMember(controller, actorInstance->characterInfo->rumbleHandle);
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}

		TazScare(actorInstance);	// check for the effects of a taz bite move

		// return to move when scare is finished
		if (!CheckIfPlayingAnimation(actorInstance,"rant2"))
		{
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			//-----------------STATE CHANGE-----------------
			break;
		}
		break;
	case STATE_NINJAKICK: // TP: Done 
		if (RespondToStateExit(actorInstance)) 
		{
			RespondToStateExitPost(actorInstance,actorStatus);

			// PP: (for CHEAT_TIMESLICENINJAKICK) unlatch the timeslice
			VFX_timesliceLatch=false;

			break;
		}

		if(TazAttack(actorInstance, "footright")||CheatActive(CHEAT_TIMESLICENINJAKICK))
		{
			// PP: kick has connected with an enemy and the enemy has reacted in a dramatic way
			actorInstance->characterInfo->costumeFlags |= NINJAFLAG_GOODIMPACT;
		}

#define NINJAKICK_TIMESLICE_WINDOW		0.1f	// PP: the time window in seconds, during the kick, during which a timeslice can be started.  See I don't want it starting right at the end of the kick anim.

		if((actorInstance->characterInfo->costumeFlags&NINJAFLAG_GOODIMPACT)
			&&(VFX_timesliceTimer>CHEAT_TIMESLICENINJAKICK_DELAY)
			&&(VFX_timesliceTimer<(CHEAT_TIMESLICENINJAKICK_DELAY+NINJAKICK_TIMESLICE_WINDOW)))// PP: don't want the timeslice to start till the kick is ready; don't want it to start too late either
		{
			// PP: ACME budget timeslice...
			// PP: Yes, in case you're wondering, my timeslice effect IS meant to look tacky!  It's a joke, see?

			if(!VFX_timesliceLatch)
			{
				// PP: stop normal kick sound
				StopChannel(&characterSounds.taz);

				// PP: (long) "hi-ya!"
				PlayTrackingSample("tazninja_1.wav", 255, actorInstance->actorInstance.position);

				// PP: impact noise
				PlayTrackingSample("whip.wav", 255, actorInstance->actorInstance.position);

				VFX_timeslice();
			}
		}

		if (!CheckIfPlayingAnimation(actorInstance,"ninjakick"))
		{
			// PP: Kick has finished

			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);

			// PP: (for CHEAT_TIMESLICENINJAKICK) unlatch the timeslice
			VFX_timesliceLatch=false;

			break;
		}
		break;
	case STATE_SKATECHARGE: // TP: Done, maybe allow longer ride times
		if (RespondToStateExit(actorInstance)) 
		{
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}

		TazAttack(actorInstance, "handright");
		if (!CheckIfPlayingAnimation(actorInstance,"skatecharge"))
		{
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			break;

		}
		CharacterRespondToSteeringControl(controller, cameraInfo, actorInstance);
		break;
	case STATE_SNOWBOARDATTACK: // TP: Done, maybe tilt in angle on snowball slightly
		if (RespondToStateExit(actorInstance)) 
		{
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}
		if (!CheckIfPlayingAnimation(actorInstance,"throwsnowball"))
		{
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			break;
		}
		break;
	case STATE_SURFBOARDATTACK: // TP: Done
		if (RespondToStateExit(actorInstance)) 
		{
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}

		if (!CheckIfPlayingAnimation(actorInstance,"surfboardwallop"))
		{
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			break;
		}
		break;
	case STATE_RAPPERATTACK: // TP: Needs some effect for the music. // CMD: don't worry mr.tony, i'm on the case
		if (RespondToStateExit(actorInstance)) 
		{
			if(actorInstance->characterInfo->rapperBlastInfo)
			{
				if(actorInstance->characterInfo->rapperBlastInfo->draw)
				{
					actorInstance->characterInfo->rapperBlastInfo->draw = 1;
				}
			}
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}

		if(actorInstance->characterInfo->rapperBlastInfo->draw)
		{
			actorInstance->characterInfo->timer += 3.0f*fTime;
			SmashStuff(&map.levelInstances,actorInstance,METERTOUNIT(actorInstance->characterInfo->timer),100,NULL,SMASHFLAG_RANGEDATTACK);
			SmashStuff(&map.animatedInstances,actorInstance,METERTOUNIT(actorInstance->characterInfo->timer),100,NULL,SMASHFLAG_RANGEDATTACK);
			SmashStuff(&map.invisibleInstances,actorInstance,METERTOUNIT(actorInstance->characterInfo->timer),100,NULL,SMASHFLAG_RANGEDATTACK);

			// TP: knock enemies over
			ACTORINSTANCE *ptr;
			for (ptr = map.characterInstances.head.next;ptr != &map.characterInstances.head;ptr = ptr->next)
			{
				if (bmVectorDistance(ptr->actorInstance.position,gameStatus.player1->actorInstance.position) < METERTOUNIT(actorInstance->characterInfo->timer))
				{
					AICharacterSpun(ptr,actorInstance,ATTACKTYPE_COSTUMEREAR,false);
				}
			}

			// CMD: create and update the 'special' effect
//			UpdateRapperBlast(actorInstance);
		}

		if (!CheckIfPlayingAnimation(actorInstance,"rapperghettoblast"))
		{
			if(actorInstance->characterInfo->rapperBlastInfo)
			{
				if(actorInstance->characterInfo->rapperBlastInfo->draw)
				{
					actorInstance->characterInfo->rapperBlastInfo->draw = 1;
				}
			}
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			//musicSystem.FadeUpAudio(0, 0, 0.5f, 1.0f);
			break;
		}
		break;
	case STATE_CHEESYATTACK: // TP: Needs effect on the enemies
		if (RespondToStateExit(actorInstance)) 
		{
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}
		if (!CheckIfPlayingAnimation(actorInstance,"cheesyattack")||!CheckIfPlayingAnimation(actorInstance,"idle6"))
		{
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			break;
		}
		break;
	case STATE_WEREWOLFATTACK: // TP: Done
		if (RespondToStateExit(actorInstance)) 
		{
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}
		TazAttack(actorInstance,"handright");
		if ((!CheckIfPlayingAnimation(actorInstance,"werewolfbiteclaw"))&&(!CheckIfPlayingAnimation(actorInstance,"reindeercharge")))
		{
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			break;
		}
		break;
	case STATE_COWBOYSHOOT: // TP: Needs an event marker, also collision problem
		if (RespondToStateExit(actorInstance)) 
		{
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}
		if (!CheckIfPlayingAnimation(actorInstance,"cowboyshootfinger"))
		{
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			break;
		}
		break;
	case STATE_INDYROLL: // TP: Done, needs testing
		if (RespondToStateExit(actorInstance)) 
		{
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}
		if (!CheckIfPlayingAnimation(actorInstance,"throwhelmet"))
		{
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			break;
		}
		break;
	case STATE_TAZANYELL:
		if (RespondToStateExit(actorInstance)) 
		{
			StopChannel(&actorStatus->soundHandle[0]);
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}
		if (!CheckIfPlayingAnimation(actorInstance,"tarzanyell"))
		{
			DoTazanCostumeMove(actorInstance);
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			break;
		}
		break;
	case STATE_SPINUP:
		HandleSpinUpState(player);
		break;
	case STATE_SPINDOWN:
		CreateImpactClouds(actorInstance->actorInstance.position, gameStatus.windVelocity, NULL, METERTOUNIT(0.15f), 2.0f, 0.0f, SMOKE_SKIDSTOP);
		HandleSpinDownState(player);
		break;
	case STATE_SPINDOWNONWATER:
		if(!CheckIfPlayingAnimation(actorInstance, "spindown"))
		{
			if(actorInstance->characterInfo)
			{
				actorInstance->characterInfo->powerUpInfo.clock = 0.0f;
			}
			ChangeState(actorInstance,STATE_KOIKFROMWATER);
		}
		break;
	case STATE_SPIN:
		if ((actorInstance->actorStatus->soundloopTimer += 1.0f/fps) > 0.2 && actorInstance->actorStatus->soundHandle[1] != -1)
		{
			StopChannel(&actorInstance->actorStatus->soundHandle[1]);
		}
		if (actorInstance->characterInfo->loseCostume)
		{
			// CMD: suspend taz
			SETVECTOR(actorInstance->actorBehaviour->physics.velocity, 0.0f, 0.0f, 0.0f, 1.0f);
			actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_NOGRAVITY;
			
			if(GetNaked(actorInstance, (EScene)map.sceneNumber))
			{
				actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_NOGRAVITY;
				RemoveCostume(actorInstance);
				actorInstance->characterInfo->loseCostume = false;
				break;
			}
			HandleLoseCostumeSpin(player);
			break;
		}
		HandleSpinState(player);
		break;
	case STATE_IDLE:
		if (RespondToStateExit(actorInstance)) 
		{
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}
		EnableCameraAngleAboveHorizontal(cameraInfo, RAD(-10.0f));
		SetFixedDistanceToLookAt(cameraInfo, METERTOUNIT(2.5f));
		if (controller->idleTime < 10.0f)
		{
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			//-----------------STATE CHANGE-----------------
			break;
		}
		if (GetPriorityOfCurrentAnimation(actorInstance)==NONE)
		{
			animationToPlay = ChooseIdleAnimationForTaz(controller, actorInstance, cameraInfo);
			if(animationToPlay)
			{
				// CMD: although in state idle, current state flag may be state dance, make sense?
				if(actorStatus->currentState == STATE_DANCE)
				{
					FlushAnimationQueue(actorInstance);
					PlayAnimationBySegmentPointer(actorInstance, animationToPlay, 1.0f, 1, 0, 0.1f, IDLE);
				}
				else
				{
					FlushAnimationQueue(actorInstance);
					PlayAnimationBySegmentPointer(actorInstance, animationToPlay, 1.0f, 0, 0, 0.1f, IDLE);
				}
			}
			else
			{
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance, "idle1", 1.0f, 0, 0, 0.1f, IDLE);
			}
			break;
		}
		break;
	case STATE_GOTOSLEEP:
		if (RespondToStateExit(actorInstance)) 
		{
			musicSystem.RequestMood(0, STREAMMOOD_NORMAL);

			// TP: probably wont get chance to play anims but have a go 
			FlushAnimationQueue(actorInstance);
			PlaySample("tazwakeup.wav",255);
			PlayAnimationByName(actorInstance, "wakeywakey", 1.0f, 0, 0, 0.1f, DEATH);
			TurnOffSleepyZedsForCharacter(actorInstance);
			StopChannel(&actorInstance->actorStatus->snoreHandle);

			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}

		// PP: duration of Taz's falling-asleep sequence
#define FALLASLEEP_DURATION		ANIMTIME(baFindAnimSegment(&player->actorInstance->actorInstance, "gotosleep"))

		// PP: start subdued music as Taz falls asleep
		musicSystem.RequestMood(0,STREAMMOOD_SLOW,FALLASLEEP_DURATION);

		EnableCameraAngleAboveHorizontal(cameraInfo, RAD(10.0f));
		if(controller->idleTime < 10.0f) 
		{
			// PP: duration of Taz's waking-up sequence
#define WAKEUP_DURATION			ANIMTIME(baFindAnimSegment(&player->actorInstance->actorInstance, "wakeywakey"))

			// PP: return to normal music mood as Taz wakes up
			musicSystem.RequestMood(0, STREAMMOOD_NORMAL, WAKEUP_DURATION);

			FlushAnimationQueue(actorInstance);
			PlaySample("tazwakeup.wav",255);
			PlayAnimationByName(actorInstance, "wakeywakey", 1.0f, 0, 0, 0.1f, DEATH);
			TurnOffSleepyZedsForCharacter(actorInstance);
			StopChannel(&actorInstance->actorStatus->snoreHandle);
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			//-----------------STATE CHANGE-----------------
			break;
		}
		if(!CheckIfPlayingAnimation(actorInstance, "gotosleep"))
		{
			PlaySample("thudlight.wav", 255);

			FlushAnimationQueue(actorInstance);
			PlayAnimationByName(actorInstance, "sleepcycle", 1.0f, 1, 0, 0.1f, DEATH);
			ActivateSleepyZedsForCharacter(actorInstance, "snoutstem", bkLoadTexture(NULL,"zed.tga",0), NULL, METERTOUNIT(0.01f));
			StopChannel(&characterSounds.taz);
			characterSounds.taz = PlaySample("tazsnore.wav", 255, actorInstance->actorInstance.position,BSPLAYFLAG_FORCELOOPING);
			ChangeState(actorInstance,STATE_SLEEP);
			//-----------------STATE CHANGE-----------------
			break;
		}
		break;
	case STATE_SLEEP:

		if (RespondToStateExit(actorInstance)) 
		{
			controller->idleTime = 0.0f;
			RespondToStateExitPost(actorInstance,actorStatus);
			TurnOffSleepyZedsForCharacter(actorInstance);
			break;
		}
		
		// PP: subdued music while asleep
		//musicSystem.RequestMood(0,STREAMMOOD_SLOW);

		if(controller->idleTime < 10.0f) 
		{
			// PP: return to normal music mood as Taz wakes up
			musicSystem.RequestMood(0, STREAMMOOD_NORMAL, WAKEUP_DURATION);

			FlushAnimationQueue(actorInstance);
			PlaySample("tazwakeup.wav",255);
			PlayAnimationByName(actorInstance, "wakeywakey", 1.0f, 0, 0, 0.1f, DEATH);
			TurnOffSleepyZedsForCharacter(actorInstance);
			StopChannel(&characterSounds.taz);
			if(characterSounds.taz == -1)
			{
				CallbackWhenSampleFinished((characterSounds.taz=PlaySample("taz_voc_52a.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.taz);
			}
			ChangeState(actorInstance,STATE_PLAYANIMATION);
			//-----------------STATE CHANGE-----------------
			break;
		}
		break;

	case STATE_DANCE:
		if (RespondToStateExit(actorInstance)) 
		{
			controller->idleTime = 0.0f;
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}

		EnableCameraAngleAboveHorizontal(cameraInfo, RAD(-10.0f));
		SetFixedDistanceToLookAt(cameraInfo, METERTOUNIT(2.5f));

		// PP: if CHEAT_INSOMNIA is active, Taz can't sleep.  Fair's fair I say!
		// PP: NOTE: this code is duplicated in characters.cpp, in ChooseIdleAnimationForTaz.
		if(!CheatActive(CHEAT_INSOMNIA))
		{
			if((controller->idleTime > 120.0f)&&(actorInstance->actorAnimation->timeSinceLastIdleMove > 5.0f))
			{
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance, "gotosleep", 1.0f, 0, 0, 0.1f, DEATH);
				if(characterSounds.taz == -1)
				{
					CallbackWhenSampleFinished((characterSounds.taz=PlaySample("tazfallasleep.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.taz);
				}
				ChangeState(actorInstance,STATE_GOTOSLEEP);
				//----------------STATE CHANGE---------------
			}
		}

		if(controller->idleTime < 5.0f) 
		{
			FlushAnimationQueue(actorInstance);
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			//-----------------STATE CHANGE-----------------
			break;
		}
		if((actorInstance->actorAnimation->timeInDanceState += fTime) > 5.0f)
		{
			actorInstance->actorAnimation->timeInDanceState = 0.0f;
			FlushAnimationQueue(actorInstance);
			PlayAnimationByName(actorInstance, "idle1", 1.0f, 0, 0, 0.1f, IDLE);
			ChangeState(actorInstance,STATE_IDLE);
			//-----------------STATE CHANGE-----------------
			break;
		}
		break;
/*	case STATE_COLLECTPOSTCARD:
		CollectPostcard(actorInstance,&actorInstance->characterInfo->postcard,cameraInfo,"postcard.obe","fingerr2a");
		FlushAnimationQueue(actorInstance);
		PlayAnimationByName(actorInstance,"postcardidle1",1.0f,TRUE,FALSE,0.2f,NONE);

		UpdatePostcard(&actorInstance->characterInfo->postcard);
		StartPostcardCamera(&actorInstance->characterInfo->postcard,"nasal");
		ChangeState(actorInstance,STATE_HOLDINGPOSTCARD);
		//-----------------STATE CHANGE-----------------
		break;
	case STATE_HOLDINGPOSTCARD:
		UpdatePostcard(&actorInstance->characterInfo->postcard);
		if (controller->crossDebounceChannel->value)
		{
			EndPostcardCamera(&actorInstance->characterInfo->postcard);
			ChangeState(actorInstance,STATE_DESTROYPOSTCARD);
			//-----------------STATE CHANGE-----------------
			break;
		}
		break;
	case STATE_DESTROYPOSTCARD:
		RemovePostcard(&actorInstance->characterInfo->postcard);
		if(actorInstance->characterInfo)
		{
			if(actorInstance->characterInfo->powerUpInfo.chilliPepper)
			{
				ChangeState(actorInstance,STATE_CHILLIPEPPER);
			}
			else
			{
				ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			}
			break;
		}
		else
		{
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
		}
		//-----------------STATE CHANGE-----------------
		break;
*/	
	case STATE_VEHICLEWATERKOIK:
		actorInstance->actorAnimation->useMotionBone = TRUE;
		ResetCharacterAfterJump(actorInstance);
		FlushAnimationQueue(actorInstance);

		actorInstance->actorBehaviour->physics.coll = 0;

		//set fade details
		bmVectorCopy(worldSpace, actorInstance->actorInstance.position);
		worldSpace[3] = 1.0f;
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
		bdSetIdentityObjectMatrix();
		bdProjectVertices(screenSpace, worldSpace, 1);
		fade->SetCircleFadeTo(screenSpace[0], screenSpace[1]);
		fade->StartFadeUp(FADETYPE_CIRCLE, 1.0f);
		
		ChangeState(actorInstance,STATE_VEHICLE);

		break;

	case STATE_KOIKFROMWATER:
		bkPrintf("Taz is in KOIK state\n");
		if(actorInstance->actorStatus->vehicleInfo)
		{
			ResetCharacterAfterJump(actorInstance);

			switch(actorInstance->actorStatus->vehicleInfo->type)
			{
			case VEHICLE_TROLLEY:
				//change field of view
				SetFixedDistanceToLookAt(gameStatus.player[actorInstance->playerNo].camera, METERTOUNIT(2.0f));
				SetCameraSmooth(gameStatus.player[actorInstance->playerNo].camera, 0.2f, 0.2f);
				EnableCameraRotateBehind(gameStatus.player[actorInstance->playerNo].camera, RAD(90.0f));

				//set actor velocity
				SETVECTOR(actorInstance->actorBehaviour->physics.velocity, 0.0f, 0.0f, 0.0f, 1.0f);

				tbTempInstance = ReturnAttachedObjectInstance(actorInstance, actorInstance->actorStatus->vehicleInfo->handle);
				ASSERTM(tbTempInstance, "could not find vehicle in state koik from water");

				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance, "drive", 1.0f, 1, 0, 0.1f, NONE);
				FlushAnimationQueue(tbTempInstance);
				PlayAnimationByName(tbTempInstance, "shopasusual", 1.0f, 1, 0, 0.1f, NONE);

				//turn motion bone off
				actorInstance->actorAnimation->useMotionBone = FALSE;
				SetActorMinFricAngle(actorInstance->actorBehaviour, 1.0f);
				actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_WALLGROUNDCOLLFIX;
				actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;

				// CMD: zero velocity
				SETVECTOR(actorInstance->actorBehaviour->physics.velocity, 0.0f, 0.0f, 0.0f, 1.0f);
				actorInstance->actorStatus->vehicleInfo->clock = 1.0f;
				actorInstance->actorStatus->vehicleInfo->rate = 0.0f;
				actorInstance->actorStatus->vehicleInfo->numSoFar = 0;

				break;
			case VEHICLE_POLISHER:
				// CMD: no water in museum
				break;
			case VEHICLE_HOVERCRAFT:
				// CMD: no hovercraft in game
				break;
			case VEHICLE_ROCKETBIKE:
				// CMD: no water in grand canyon
				break;
			}

			//set fade details
			node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "snoutstem");
			if(node)
			{
				bdSetIdentityObjectMatrix();
				baGetNodesWorldPosition(&actorInstance->actorInstance, node, worldSpace);
				worldSpace[3] = 1.0f;
				bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
				bdSetIdentityObjectMatrix();
				bdProjectVertices(screenSpace, worldSpace, 1);
				fade->SetCircleFadeTo(screenSpace[0], screenSpace[1]);
				fade->StartFadeUp(FADETYPE_CIRCLE, 1.0f);
				bkPrintf("Fade up has started\n");
			}
			else
			{
				// TP: just in case bug(412)
				fade->StartFadeUp(FADETYPE_NORMAL, 1.0f);
				bkPrintf("Fade up has started\n");
			}
			ChangeState(actorInstance,STATE_VEHICLE);
			bkPrintf("Exited Koik state into STATE_VEHICLE\n");
		}
		else
		{
			if (fade->GetSceneToLoad() != FADESCENE_NONE)
				break;

			actorInstance->actorAnimation->useMotionBone = TRUE;
			ResetCharacterAfterJump(actorInstance);
			FlushAnimationQueue(actorInstance);
			if(bmRand()%2)	PlayAnimationByName(actorInstance, "shakedry", 1.0f, 0, 0, 0.1f, NONE);
			else			PlayAnimationByName(actorInstance, "shakedry2", 1.0f, 0, 0, 0.1f, NONE);

			PlaySample("taz shake hair dry after water",255,actorInstance->actorInstance.position);
			ReduceCharactersHealth(actorInstance, 1.0f);
			//CreateInjuryStars(10, actorInstance, "spinetop", starsOffset, starsBounds);

			//set fade details
			node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "snoutstem");
			if(node)
			{
				bdSetIdentityObjectMatrix();
				baGetNodesWorldPosition(&actorInstance->actorInstance, node, worldSpace);
				worldSpace[3] = 1.0f;
				bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
				bdSetIdentityObjectMatrix();
				bdProjectVertices(screenSpace, worldSpace, 1);
				fade->SetCircleFadeTo(screenSpace[0], screenSpace[1]);
				fade->StartFadeUp(FADETYPE_CIRCLE, 1.0f);
				bkPrintf("Fade up has started\n");
			}
			else
			{
				// TP: just in case bug(412)
				fade->StartFadeUp(FADETYPE_NORMAL, 1.0f);
				bkPrintf("Fade up has started\n");
			}

			ChangeState(actorInstance,STATE_GETUPFROMWATER);
			bkPrintf("Exited Koik state into STATE_GETUPFROMWATER\n");
		}
		break;
	case STATE_KOIKFROMDEATHPLANE:
		if(actorInstance->actorStatus->vehicleInfo)
		{
			ResetCharacterAfterJump(actorInstance);

			switch(actorInstance->actorStatus->vehicleInfo->type)
			{
			case VEHICLE_TROLLEY:
				//change field of view
				SetFixedDistanceToLookAt(gameStatus.player[actorInstance->playerNo].camera, METERTOUNIT(2.0f));
				SetCameraSmooth(gameStatus.player[actorInstance->playerNo].camera, 0.2f, 0.2f);
				EnableCameraRotateBehind(gameStatus.player[actorInstance->playerNo].camera, RAD(90.0f));

				//set actor velocity
				SETVECTOR(actorInstance->actorBehaviour->physics.velocity, 0.0f, 0.0f, 0.0f, 1.0f);

				tbTempInstance = ReturnAttachedObjectInstance(actorInstance, actorInstance->actorStatus->vehicleInfo->handle);
				ASSERTM(tbTempInstance, "could not find attached instance on hitting death plane");

				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance, "drive", 1.0f, 1, 0, 0.1f, NONE);
				FlushAnimationQueue(tbTempInstance);
				PlayAnimationByName(tbTempInstance, "shopasusual", 1.0f, 1, 0, 0.1f, NONE);

				//turn motion bone off
				actorInstance->actorAnimation->useMotionBone = FALSE;
				SetActorMinFricAngle(actorInstance->actorBehaviour, 1.0f);
				actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_WALLGROUNDCOLLFIX;
				actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;

				// CMD: zero velocity
				SETVECTOR(actorInstance->actorBehaviour->physics.velocity, 0.0f, 0.0f, 0.0f, 1.0f);
				actorInstance->actorStatus->vehicleInfo->clock = 1.0f;
				actorInstance->actorStatus->vehicleInfo->rate = 0.0f;
				actorInstance->actorStatus->vehicleInfo->numSoFar = 0;
				break;
			case VEHICLE_POLISHER:
				// CMD: no death plane in museum
				break;
			case VEHICLE_HOVERCRAFT:
				// CMD: no hovercraft in game
				break;
			case VEHICLE_ROCKETBIKE:
				//change field of view
				SetFixedDistanceToLookAt(gameStatus.player[actorInstance->playerNo].camera, METERTOUNIT(0.75));
				SetCameraSmooth(gameStatus.player[actorInstance->playerNo].camera, 1.0f, 0.6f);
				EnableCameraRotateBehind(gameStatus.player[actorInstance->playerNo].camera, RAD(90.0f));

				//set actor velocity
				SETVECTOR(actorInstance->actorBehaviour->physics.velocity, 0.0f, 0.0f, 0.0f, 1.0f);

				tbTempInstance = ReturnAttachedObjectInstance(actorInstance, actorInstance->actorStatus->vehicleInfo->handle);
				ASSERTM(tbTempInstance, "could not find attached instance on hitting death plane");

				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance, "fly", 1.0f, 1, 0, 0.1f, NONE);
				FlushAnimationQueue(tbTempInstance);
				PlayAnimationByName(tbTempInstance, "rocketbikeride", 1.0f, 1, 0, 0.1f, NONE);

				//turn motion bone off
				actorInstance->actorAnimation->useMotionBone = FALSE;
				SetActorMinFricAngle(actorInstance->actorBehaviour, 1.0f);
				actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_WALLGROUNDCOLLFIX;
				actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;

//				if (!actorInstance->playerNo)		oneTime = 0.0f;
//				else								twoTime = 0.0f;
				break;
			}

			//set fade details
			node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "snoutstem");
			if(node)
			{
				bdSetIdentityObjectMatrix();
				baGetNodesWorldPosition(&actorInstance->actorInstance, node, worldSpace);
				worldSpace[3] = 1.0f;
				bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
				bdSetIdentityObjectMatrix();
				bdProjectVertices(screenSpace, worldSpace, 1);
				fade->SetCircleFadeTo(screenSpace[0], screenSpace[1]);
				fade->StartFadeUp(FADETYPE_CIRCLE, 1.0f);
			}
			else
			{
				// TP: must fade up even if somehow the bone could not be found (bug 412)
				fade->StartFadeUp(FADETYPE_NORMAL, 1.0f);
			}
			ChangeState(actorInstance,STATE_VEHICLE);
		}
		else
		{
			actorInstance->actorAnimation->useMotionBone = TRUE;
			ResetCharacterAfterJump(actorInstance);
			FlushAnimationQueue(actorInstance);
			PlayAnimationByName(actorInstance, "agitated", 1.0f, 0, 0, 0.1f, NONE);

			ReduceCharactersHealth(actorInstance, 1.0f);
			//CreateInjuryStars(10, actorInstance, "spinetop", starsOffset, starsBounds);

			//set fade details
			node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "snoutstem");
			if(node)
			{
				bdSetIdentityObjectMatrix();
				baGetNodesWorldPosition(&actorInstance->actorInstance, node, worldSpace);
				worldSpace[3] = 1.0f;
				bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
				bdSetIdentityObjectMatrix();
				bdProjectVertices(screenSpace, worldSpace, 1);
				fade->SetCircleFadeTo(screenSpace[0], screenSpace[1]);
				fade->StartFadeUp(FADETYPE_CIRCLE, 1.0f);
			}
			else
			{
				// TP: must fade up even if somehow the bone could not be found (bug 412)
				fade->StartFadeUp(FADETYPE_NORMAL, 1.0f);
			}
			ChangeState(actorInstance,STATE_GETUPFROMWATER);
		}
		break;
	case STATE_PROJECTILE:
		if (RespondToStateExit(actorInstance)) 
		{
			SetDefaultCameraAngleAboveHorizontal(camera,CAMERAANGLE);
			ResetCharacterAfterJump(actorInstance);
			actorInstance->actorAnimation->useMotionBone = TRUE;
			RespondToStateExitPost(actorInstance,actorStatus);
			actorInstance->characterInfo->projecting.fixedPathEnable = false;
			break;
		}
		SetDefaultCameraAngleAboveHorizontal(camera,RAD(30.0f));

		FollowFixedPath(actorInstance);
		
#if BPLATFORM==PC
		if ((actorPhysics->coll)&&(actorInstance->actorStatus->timeInCurrentState > 0.13f))
#else
		if ((actorPhysics->coll)&&(actorInstance->actorStatus->timeInCurrentState > 0.1f))
#endif
		{
			// PP: if a custom collision callback has been set...
			if(projectileStateCustomCollisionCallback != NULL)
			{
				// PP: ...call it now...
				projectileStateCustomCollisionCallback(actorInstance);
			}
			else// PP: ...otherwise...
			{
				// PP: ...this is the way that we rock when we're doin' our ting
				ResetCharacterAfterJump(actorInstance);
				actorInstance->actorAnimation->useMotionBone = TRUE;
				PlaySample("taz getup from slide",255,actorInstance->actorInstance.position);

				FlushAnimationQueue(actorInstance);

				if(CheckIfPlayingAnimation(actorInstance, "fly2"))
				{
					PlayAnimationByName(actorInstance, "cannonfail", 1.0f, 0, 0, 0.1f, NONE);
				}
				else
				{
					switch(bmRand()%2)
					{
					case 0:
						PlayAnimationByName(actorInstance, "getup", 1.0f, 0, 0, 0.1f, NONE);
						break;
					case 1:
						PlayAnimationByName(actorInstance, "idle9", 1.0f, 0, 0, 0.1f, NONE);
						break;
					}
				}

				StopChannel(&actorStatus->soundHandle[0]);
				actorInstance->characterInfo->projecting.fixedPathEnable = false;
				ChangeState(actorInstance,STATE_GETUPFROMSLIDE);
				SetDefaultCameraAngleAboveHorizontal(camera,CAMERAANGLE);
				//-----------------STATE CHANGE-----------------
			}
		}
		else
		{
			// PP: Let me explain this disgusting-looking bodge.
			// PP: If the scene is West Boss and a character is in projectile state,
			// PP: I assume that the character is Taz and he's being blown along by an
			// PP: explosion.  I want him to fly face-first, not feet-first.
			// PP: This is only place in the game where we need face-first projectile movement.
			if(map.sceneNumber == SCENE_WESTBOSS)
			{
				UpdateProjectilePitch(actorInstance, HALFPI*0.2f);// PP: x tilt
			}
			else
			{
				UpdateProjectilePitch(actorInstance);
			}
		}
		break;
	case STATE_PROJECTILESLIDE:
		if (RespondToStateExit(actorInstance))
		{
			SetDefaultCameraAngleAboveHorizontal(camera,CAMERAANGLE);
			StopChannel(&actorStatus->soundHandle[0]);

			ResetCharacterAfterJump(actorInstance);
			SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
			SetActorFriction(actorInstance->actorBehaviour,0.0f);
			actorInstance->actorAnimation->useMotionBone = TRUE;
			actorInstance->characterInfo->projecting.fixedPathEnable = false;

			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}
		SetDefaultCameraAngleAboveHorizontal(camera,RAD(30.0f));

		FollowFixedPath(actorInstance);
		
		if (((actorPhysics->coll)||(actorInstance->actorStatus->timeWithoutWallCollision < 0.3f))&&(actorInstance->actorStatus->timeInCurrentState > 0.1f))
		{
			SetActorMinFricAngle(actorInstance->actorBehaviour,1.0f);
			SetActorFriction(actorInstance->actorBehaviour,METERTOUNIT(-7.0f));
			if(!CheckIfPlayingAnimation(actorInstance, "slide"))
			{
				PlayAnimationByName(actorInstance,"slide",0.1f,1,0,0.1f,JUMP);
			}
			actorInstance->actorAnimation->useMotionBone = FALSE;
		
			if((actorPhysics->speed <= METERTOUNIT(2.0f))) // && actorPhysics->friction)
			{
				// I have slid onto proper ground
				// flush anim ready for move state
				ResetCharacterAfterJump(actorInstance);
				SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
				SetActorFriction(actorInstance->actorBehaviour,0.0f);
				actorInstance->actorAnimation->useMotionBone = TRUE;
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance, "getup", 1.0f, 0, 0, 0.1f, NONE);
				StopChannel(&actorStatus->soundHandle[0]);
				PlaySample("taz getup from slide",255,actorInstance->actorInstance.position);
				SetDefaultCameraAngleAboveHorizontal(camera,CAMERAANGLE);
				actorInstance->characterInfo->projecting.fixedPathEnable = false;
				ChangeState(actorInstance,STATE_GETUPFROMSLIDE);
				//-----------------STATE CHANGE-----------------
				break;
			}
			// Process slide control
			// get actor speed
			bmVectorCopy(tempVector, actorInstance->actorBehaviour->physics.velocity);
			tempVector[Y] = 0.0f;
			tempFloat = bmVectorLen(tempVector);
			if (tempFloat)//>(METERTOUNIT(0.1)/60))
			{
				// rotate actor in direction of slide
				tempFloat = GetAngleAroundYFromVector(actorInstance->actorBehaviour->physics.velocity);
				// rotate angle 180 degrees because characters face in -Z
				CharacterRotateToAngle(actorInstance,0,-tempFloat,0,0.2f);
			}
			CharacterRespondToSlideMovementInput(controller,cameraInfo,actorInstance);
		}
		else
		{
			UpdateProjectilePitch(actorInstance);
		}
		break;
	case STATE_SWINGING:
		if (RespondToStateExit(actorInstance)) 
		{
			StopChannel(&actorInstance->characterInfo->swingingInfo.swingingOnObject->swingingObjectInfo->soundHandle);
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}
		// check for player pressing jump
		if (controller->crossDebounceChannel->value)
		{
			if(actorInstance->characterInfo->swingingInfo.swingingOnObject)
			{
				actorInstance->characterInfo->swingingInfo.swingingOnObject->swingingObjectInfo->collision = FALSE;
				if(actorInstance->characterInfo->swingingInfo.swingingOnObject->swingingObjectInfo->modelPtr)
				{
					FlushAnimationQueue(actorInstance->characterInfo->swingingInfo.swingingOnObject->swingingObjectInfo->modelPtr);
				}
				StopChannel(&actorInstance->characterInfo->swingingInfo.swingingOnObject->swingingObjectInfo->soundHandle);
			}
			if (PlayAnimationByName(actorInstance,"jumprise",0.6f,0,0,0.1f,JUMP))	// If I could play jump then
			{
				CalculateSpeedBeforeJump(actorInstance);	// fill in current speed before jump
				actorInstance->actorInstance.position[Y] += 1.0f;	// bodge to avoid sticking to ground
				// zero vertical speed before adding jump
				actorInstance->actorBehaviour->physics.velocity[Y] = 0.0f;
				tvec[X] = 0.0f;
				tvec[Y] = 1.0f;
				tvec[Z] = 0.0f;
				tvec[W] = 1.0f;
				
				if (player->actorInstance->actorStatus->multiplayer.effect == PLAYEREFFECTSLOMO)
					AddActorSpeed( actorInstance->actorBehaviour, tvec, METERTOUNIT(3.0f));//METERTOUNIT(6.324f)
				else
					AddActorSpeed( actorInstance->actorBehaviour, tvec, METERTOUNIT(7.0f));//7

				bmVectorCopy(startVel, actorInstance->actorBehaviour->physics.velocity);

				PlayAnimationByName(actorInstance,"fly1",0.4f,1,1,0.2f,JUMP);
				actorPhysics->flags &= ~PHYSFLAG_WALLVELCORRECTION;
				actorInstance->actorAnimation->useMotionBone = FALSE;
				numFrames = 0;
				totalTime = 0;
				maxHeight = 0;
				actorInstance->characterInfo->swingingInfo.swingingOnObject->swingingObjectInfo->collision = FALSE;
				actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_NOGRAVITY;
				StopChannel(&actorStatus->soundHandle[0]);
				actorStatus->soundHandle[0] = PlaySample("taz_gen_08a.wav",255,actorInstance->actorInstance.position);
				ChangeState(actorInstance,STATE_PROJECTILESLIDE);
				//-----------------STATE CHANGE-----------------

				//reset camera to standard
				RestoreCameraSettings(cameraInfo, &actorInstance->characterInfo->swingingInfo.preSwingCamera);
				cameraInfo->flags &= ~CAMERA_ROPECONTROL;
				break;
			}
		}
		//if not jumping out of swing state, then calculate swing
		actorInstance->actorAnimation->useMotionBone = FALSE;
		CharacterRespondToSwingMovementInput(controller,cameraInfo,actorInstance);
		SwingActor(actorInstance);
		break;
	case STATE_BURNT:
		DrawSmokeAtNodeInstance(actorInstance,"tailbase", 0, SMOKE_SKIDSTOP);
	case STATE_SPRUNG:
		if (RespondToStateInit(actorInstance))
		{
			// TP: get the initial position of the character before jump
			bmVectorCopy(actorStatus->positionBeforeJump,actorInstance->actorInstance.position);
		}
		if (RespondToStateExit(actorInstance)) 
		{
			actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_WALLVELCORRECTION;
			actorInstance->actorAnimation->useMotionBone = true;

			RespondToStateExitPost(actorInstance,actorStatus);
			SetDefaultCameraAngleAboveHorizontal(camera,CAMERAANGLE);
			break;
		}
		SetDefaultCameraAngleAboveHorizontal(camera,RAD(30.0f));
		//check to see if on the way down
		if(actorInstance->actorBehaviour->physics.velocity[1] < 0.0f)
		{
			if (CheckIfPlayingAnimation(actorInstance,"bounceup"))
			{
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance, "bouncedown", 1.0f, 1, 0, 0.6f, JUMP);
			}
			else
			{
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance, "bouncedown2", 1.0f, 0, 0, 0.6f, JUMP);
			}
			ChangeState(actorInstance,STATE_JUMP);
			break;
		}
		CharacterRespondToMovementWhileJumping(controller,cameraInfo,actorInstance);
		break;
	case STATE_ZAPPEDINTOMOUSE:
		if(!CheckIfPlayingAnimation(actorInstance, NULL))
		{
			ChangeActorInstance(actorInstance,FindActorInActorList("extras\\mouse.obe"),5,NULL);
			ChangeState(actorInstance, STATE_MOUSE);
		}
		break;
	case STATE_ZAPPEDINTOBALL:
		if(!CheckIfPlayingAnimation(actorInstance, NULL))
		{
			ChangeActorInstance(actorInstance,FindActorInActorList("extras\\ball.obe"),5,NULL);
			ChangeState(actorInstance, STATE_BALL);
		}
		break;
	case STATE_ZAPPEDINTOTAZ:
		if(!CheckIfPlayingAnimation(actorInstance, NULL))
		{
			if(actorInstance->characterInfo->costume != -1)
			{
				ChangeActorInstance(actorInstance,FindActorInActorList("tazdevil.obe"),5,NULL);
				SetupCostume(actorInstance, actorInstance->characterInfo->costume);
			}
			else
			{
				ChangeActorInstance(actorInstance,FindActorInActorList("tazdevil.obe"),5,NULL);
			}
			ChangeState(actorInstance, STATE_MOVE);
			FlushAnimationQueue(actorInstance);
			PlayAnimationByName(actorInstance, "idle1", 1.0f, 0, 0, 0.1f, IDLE);
		}
		break;
	case STATE_VEHICLE:

		if(RespondToStateExit(actorInstance)) break;
		if(actorInstance->actorStatus->vehicleInfo == NULL) break;

		switch(actorInstance->actorStatus->vehicleInfo->type)
		{
		case VEHICLE_TROLLEY:
			//set camera chase mode
			//calculate direction vector fo Taz
			bmQuatToMatrix(tempMatrix, actorInstance->actorInstance.orientation);
			SETVECTOR(tempVector, 0.0f, 0.0f, -1.0f, 1.0f);
			bmMatMultiplyVector(tempMatrix, tempVector);
			bmVectorCopy(actorInstance->actorStatus->vehicleInfo->orientationVector, tempVector);

			if(cameraInfo->collisionActor.actorBehaviour->physics.coll)
			{
				actorInstance->actorStatus->vehicleInfo->timeSinceCameraColl = 0.0f;
			}
			else actorInstance->actorStatus->vehicleInfo->timeSinceCameraColl += fTime;

			if(actorInstance->actorStatus->vehicleInfo->timeSinceCameraColl > 0.5f)
			{
				EnableCameraChaseCam(cameraInfo, RAD(360.0f),actorInstance->actorStatus->vehicleInfo->orientationVector);
			}
			CharacterRespondToTrolleyControl(controller, cameraInfo, actorInstance);
			break;
#ifndef CONSUMERDEMO
		case VEHICLE_POLISHER:
			//calculate direction vector fo Taz
			bmQuatToMatrix(tempMatrix, actorInstance->actorInstance.orientation);
			SETVECTOR(tempVector, 0.0f, 0.0f, -1.0f, 1.0f);
			bmMatMultiplyVector(tempMatrix, tempVector);
			bmVectorCopy(actorInstance->actorStatus->vehicleInfo->orientationVector, tempVector);
			//set camera chase mode
			switch(map.sceneNumber)
			{
			case SCENE_MUSEUM:
				EnableCameraAngleAboveHorizontal(cameraInfo, RAD(40.0f));
				SetFixedDistanceToLookAt(cameraInfo, METERTOUNIT(2.0f));
				break;
			default:
				EnableCameraAngleAboveHorizontal(cameraInfo, RAD(20.0f));
				SetFixedDistanceToLookAt(cameraInfo, METERTOUNIT(2.0f));
				break;
			}
			if(cameraInfo->collisionActor.actorBehaviour->physics.coll)
			{
				actorInstance->actorStatus->vehicleInfo->timeSinceCameraColl = 0.0f;
			}
			else actorInstance->actorStatus->vehicleInfo->timeSinceCameraColl += fTime;

			if(actorInstance->actorStatus->vehicleInfo->timeSinceCameraColl > 1.0f)
			{
				EnableCameraChaseCam(cameraInfo, RAD(500.0f),actorInstance->actorStatus->vehicleInfo->orientationVector);
			}
		
			CharacterRespondToFloorPolisherControl(controller, cameraInfo, actorInstance);
			break;
		case VEHICLE_HOVERCRAFT:
			//calculate direction vector fo Taz
			bmQuatToMatrix(tempMatrix, actorInstance->actorInstance.orientation);
			SETVECTOR(tempVector, 0.0f, 0.0f, -1.0f, 1.0f);
			bmMatMultiplyVector(tempMatrix, tempVector);
			bmVectorCopy(actorInstance->actorStatus->vehicleInfo->orientationVector, tempVector);
			//set camera chase mode
			EnableCameraAngleAboveHorizontal(cameraInfo, RAD(0.0f));
			if(cameraInfo->collisionActor.actorBehaviour->physics.coll)
			{
				actorInstance->actorStatus->vehicleInfo->timeSinceCameraColl = 0.0f;
			}
			else actorInstance->actorStatus->vehicleInfo->timeSinceCameraColl += fTime;

			if(actorInstance->actorStatus->vehicleInfo->timeSinceCameraColl > 1.0f)
			{
				EnableCameraChaseCam(cameraInfo, RAD(360.0f),actorInstance->actorStatus->vehicleInfo->orientationVector);
			}
		
			CharacterRespondToHovercraftControl(controller, cameraInfo, actorInstance);
			break;
		case VEHICLE_ROCKETBIKE:
			//calculate direction vector fo Taz
			bmQuatToMatrix(tempMatrix, actorInstance->actorInstance.orientation);
			SETVECTOR(tempVector, 0.0f, 0.0f, -1.0f, 1.0f);
			bmMatMultiplyVector(tempMatrix, tempVector);
			bmVectorCopy(actorInstance->actorStatus->vehicleInfo->orientationVector, tempVector);
			//set camera chase mode
			EnableCameraAngleAboveHorizontal(cameraInfo, RAD(0.0f));
			if(cameraInfo->collisionActor.actorBehaviour->physics.coll)
			{
				actorInstance->actorStatus->vehicleInfo->timeSinceCameraColl = 0.0f;
			}
			else actorInstance->actorStatus->vehicleInfo->timeSinceCameraColl += fTime;

			if(actorInstance->actorStatus->vehicleInfo->timeSinceCameraColl > 0.25f)
			{
				EnableCameraChaseCam(cameraInfo, RAD(360.0f),actorInstance->actorStatus->vehicleInfo->orientationVector);
			}
		
			// PP: NOTE: I moved FadeFieldOfView to somewhere where it always gets called

			CharacterRespondToRocketbikeControl(controller, cameraInfo, actorInstance);

			// CMD: add 'anti-wall' velocity
			if(actorInstance->actorStatus->timeWithoutWallCollision < 0.3f)
			{
				bmVectorSet(tempVector, 0.0f, 1.0f, 0.0f, 1.0f);
				if(bmVectorDot(tempVector, actorInstance->actorBehaviour->physics.lastAverageNormal) < 0.7f)
				{
					if((length = 0.5f*bmVectorLen(actorInstance->actorBehaviour->physics.velocity)) > METERTOUNIT(3.0f)) length = METERTOUNIT(3.0f);
					bmVectorScaleToLength(tempVector, actorInstance->actorBehaviour->physics.lastAverageNormal, length);
					AddActorVelocity(actorInstance->actorBehaviour, tempVector);
				}
			}

			break;
#endif
		}

		// check for player pressing jump
		if((controller->crossDebounceChannel->value)&&(gameStatus.multiplayer.numScreens == 1))
		{
			switch(map.sceneNumber)
			{
			case SCENE_MUSEUMVEHICLERACE:
			case SCENE_GRANDCVEHICLERACE:
			case SCENE_DEPTSTRVEHICLERACE:
				break;
			default:
				if(actorInstance->actorStatus->timeWithoutWallCollision < 0.025f)// PP: on ground
				{
					// reset vehicle
					if(actorInstance->actorStatus->vehicleInfo->soundHandle != -1)
					{
						StopChannel(&actorInstance->actorStatus->vehicleInfo->soundHandle);
						switch(actorInstance->actorStatus->vehicleInfo->type)
						{
						case VEHICLE_TROLLEY:
							break;
						case VEHICLE_POLISHER:
							{
								// PP: tell the polisher wot it got no rider no more
								actorInstance->actorStatus->vehicleInfo->polisherInfo->rider=NULL;

								float spdPrcnt = (bmVectorLen(actorInstance->actorBehaviour->physics.velocity) / FLOORPOLISHER_MAX_SPEED);

								int frequency = (int) ((STANDARD_FREQUENCY - LOWEST_FREQUENCY) * spdPrcnt) + LOWEST_FREQUENCY;
	
								PlaySample("obj_mus_03c.wav", 255, 0, frequency, actorInstance->actorInstance.position, 0);
							}
							break;
						case VEHICLE_ROCKETBIKE:
							break;
						}
					}

					// CMD: switch off the vehicle
					actorInstance->actorStatus->vehicleInfo->state=VEHICLE_STATE_OFF;

					AddActorToCollisionCache(&collisionCache, actorInstance->actorStatus->vehicleInfo->vehicleInstance, CACHEFLAGS_LIVEUPDATE);
					actorInstance->actorStatus->vehicleInfo->vehicleInstance->flags &= ~ACTORFLAG_DONTDRAW;
					RemoveObjectFromCharacter(actorInstance, actorStatus->vehicleInfo->handle);
					StopCollidingWith(actorInstance, actorInstance->actorStatus->vehicleInfo->vehicleInstance, 0.5f);
					SetActorPosition(actorStatus->vehicleInfo->vehicleInstance, actorInstance->actorInstance.position);
					FlushAnimationQueue(actorStatus->vehicleInfo->vehicleInstance);
					PlayAnimationByName(actorStatus->vehicleInfo->vehicleInstance,"idle1",1.0f,1,0,0.1f,IDLE);
					actorStatus->vehicleInfo = NULL;


					/* PP: TODO: REMOUT: duplicated code; now done in StartJump()*/
					PlayAnimationByName(actorInstance,"jumprise",0.6f,0,0,0.1f,JUMP);	// If I could play jump then
					CalculateSpeedBeforeJump(actorInstance);							// fill in current speed before jump
					actorInstance->actorInstance.position[Y] += 1.0f;					// bodge to avoid sticking to ground

					//reset camera
					DisableCameraChaseCam(cameraInfo);
					SetFixedDistanceToLookAt(cameraInfo, cameraInfo->defaultFixedDistance);
					DisableCameraRotateBehind( cameraInfo);
					SetCameraSmooth(cameraInfo, defaultPosSmooth, defaultLookatSmooth);

					//change field of view
					// PP: *** TEMP BUG FIX! - was RAD(90) tut tut *** But NORMAL_INGAME_FOV ain't gonna be any prettier in splitscreen!
					// PP: TODO: solve this situation
					setFOV(NORMAL_INGAME_FOV);

					// zero vertical speed before adding jump
					actorInstance->actorBehaviour->physics.velocity[Y] = 0.0f;
					SETVECTOR(tvec,0.0f,1.0f,0.0f,1.0f);
					
					if (player->actorInstance->actorStatus->multiplayer.effect == PLAYEREFFECTSLOMO)
						AddActorSpeed( actorInstance->actorBehaviour, tvec, METERTOUNIT(3.0f));//METERTOUNIT(6.324f)
					else
						AddActorSpeed( actorInstance->actorBehaviour, tvec, METERTOUNIT(7.0f));//7
		
					actorInstance->actorAnimation->useMotionBone = TRUE;

					// Queue fall animation
					PlayAnimationByName(actorInstance,"jumpfall",0.4f,1,1,0.2f,JUMP);
					actorPhysics->flags &= ~PHYSFLAG_WALLVELCORRECTION;
					actorInstance->actorAnimation->useMotionBone = FALSE;
					// PP: TODO: */

					SetActorMinFricAngle(actorInstance->actorBehaviour, 0.7f);

					// keep track of position prior to jump move
					actorInstance->actorStatus->lastState = actorInstance->actorStatus->currentState;
					ChangeState(actorInstance, STATE_JUMP);
					//-----------------STATE CHANGE-----------------
					break;
				}
				break;
			}
			break;
		}
		break;
#ifndef CONSUMERDEMO
	case STATE_ONMINECART:
		if(RespondToStateExit(actorInstance))
		{
			actorInstance->actorStatus->actorAttachedTo = NULL;
			SetFixedDistanceToLookAt(gameStatus.player[actorInstance->playerNo].camera, METERTOUNIT(1.0f));
			EnableCameraAngleAboveHorizontal(gameStatus.player[actorInstance->playerNo].camera, RAD(0.0f));
			DisableCameraChaseCam(cameraInfo);
			gameStatus.chrisCam = TRUE;
			SetCameraCollision(gameStatus.player[actorInstance->playerNo].camera, TRUE);
			SetCameraSmooth(gameStatus.player[actorInstance->playerNo].camera,0.06f, 0.15f);
	
			RespondToStateExitPost(actorInstance, actorStatus);
			break;
		}
		if(actorInstance->actorStatus->actorAttachedTo)
		{
			MINECARTINFO	*minecartInfo = actorInstance->actorStatus->actorAttachedTo->mineCartInfo;
			TBMatrix		minecartRot;
			TBVector		minecartPos;
	

			// CMD: increment smooth so that there is not a snap to zero smooth
			if((minecartInfo->cameraPosSmooth += fTime) > 1.0f) minecartInfo->cameraPosSmooth = 1.0f;
			if((minecartInfo->cameraLookAtSmooth += fTime) > 1.0f) minecartInfo->cameraLookAtSmooth = 1.0f;
			SetCameraSmooth(cameraInfo, minecartInfo->cameraPosSmooth, minecartInfo->cameraLookAtSmooth);

			// CMD: set camera destination, and look at destination
			bmQuatToMatrix(minecartRot, actorInstance->actorInstance.orientation);

			// CMD: matrix for camera pos offset
			bmVectorCopy(minecartPos, minecartInfo->cameraPosOffset);
			bmMatMultiplyVector(minecartRot, minecartPos);
			bmVectorAdd(minecartPos, minecartPos, actorInstance->actorInstance.position);

			SetCameraPositionDestination(cameraInfo, minecartPos);

			// CMD: calculate look at position
			bmVectorCopy(minecartPos, minecartInfo->cameraLookAtOffset);
			bmMatMultiplyVector(minecartRot, minecartPos);
			bmVectorAdd(minecartPos, minecartPos, actorInstance->actorInstance.position);

			SetCameraLookAtDestination(cameraInfo, minecartPos);

			if(~minecartInfo->flags & MINECART_NOCONTROL)
			{
				switch(map.sceneNumber)
				{
				case SCENE_GOLDMINE:
					CharacterRespondToMineCartInput(controller, cameraInfo, actorInstance);
					break;
				case SCENE_GHOSTTOWN:
					CharacterRespondToGhostTownRide(controller, cameraInfo, actorInstance);
					break;
				case SCENE_WILDWESTHUB:
					CharacterRespondWestHubSevenMineCart(controller, cameraInfo, actorInstance);
					break;
				}
			}
		}
		break;
#endif
	case STATE_TRAPPED:
		if (RespondToStateExit(actorInstance)) 
		{
			// TP: reset scale just to make sure
			bmVectorSet(actorInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}
		break;
	case STATE_DEAD:
		if((!CheckIfPlayingAnimation(actorInstance, "tazdie1"))&&(!CheckIfPlayingAnimation(actorInstance, "tazdie2"))
			&&(!CheckIfPlayingAnimation(actorInstance, "tazdie3")))
		{
			ProcessCharacterDeath(actorInstance);
			if(actorInstance->characterInfo)
			{
				actorInstance->characterInfo->powerUpInfo.clock = 0.0f;
			}
			ChangeState(actorInstance,STATE_DONOTHING);
		}
		break;
	case STATE_WAITFORTEXT:
		if (RespondToStateExit(actorInstance)) 
		{
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}
		if(CheckIfDisplayingMorronicMessages(MEVENT_NONE))
		{
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
		}
		break;
	case STATE_DONOTHING:
		if (RespondToStateExit(actorInstance)) 
		{
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}
		//otherwise, do nothing
		break;
	case STATE_MOUSE:
		if (RespondToStateExit(actorInstance)) 
		{
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}
	
		musicSystem.RequestMood(0,STREAMMOOD_SLOW);

		if(controller->mag1 > 0.2f)
		{
			if(!CheckIfPlayingAnimation(actorInstance, "idle1"))
			{
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance, "idle1", 1.0f, 1, 0, 0.1f, MOVE);
			}
			CharacterRespondToMouseMovementInput(controller,cameraInfo,actorInstance);
		}
		else
		{
			if(!CheckIfPlayingAnimation(actorInstance, "idle2"))
			{
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance, "idle2", 1.0f, 1, 0, 0.1f, IDLE);
			}
		}
		break;
	case STATE_BALL:
		if (RespondToStateExit(actorInstance)) 
		{
			worldGravity = NORMAL_WORLD_GRAVITY;
			actorInstance->actorInstance.position[Y] -= METERTOUNIT(0.3895f);
			actorInstance->actorBehaviour->collSphere.offset[1] = 120.0f;
			actorInstance->actorBehaviour->collSphere.radius = METERTOUNIT(0.5f);
			actorInstance->actorAnimation->useMotionBone = TRUE;
			SetActorMinFricAngle(actorInstance->actorBehaviour, 0.0f);
			actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLGROUNDCOLLFIX;
			actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_WALLVELCORRECTION;
			SetActorOrientation(actorInstance, actorInstance->actorStatus->waterResetQuaternion);
			actorInstance->flags &= ~ACTORFLAG_DONTDRAW;
			actorInstance->flags &= ~ACTORFLAG_NOVELOCITY;
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}
	
		musicSystem.RequestMood(0,STREAMMOOD_FAST);

		if(actorInstance->actorBehaviour->physics.coll)
		{
			// CMD: play sound based on length of velocity before collision
			if(actorInstance->actorStatus->timeWithoutWallCollisionLastFrame > 0.25f)
			{
				if((length = actorInstance->actorStatus->timeWithoutWallCollisionLastFrame*255.0f) > 255.0f) length = 255.0f;
				PlaySample("ballbounce2.wav", bmFloatToInt(length), actorInstance->actorInstance.position, 0);
			}
			ClipVector(delta, actorInstance->actorBehaviour->physics.velocity, actorInstance->actorBehaviour->physics.lastAverageNormal);
			bmVectorCMul(clip, actorInstance->actorBehaviour->physics.velImpactFromLastColl, 0.5f);
			if(bmVectorLen(clip) < METERTOUNIT(0.5f)) bmVectorZero(clip);
			bmVectorAdd(clip, delta, clip);
			SetActorVelocity(actorInstance->actorBehaviour, clip);
			bmVectorCopy(actorInstance->actorStatus->waterResetPoint, actorInstance->actorBehaviour->physics.lastAverageNormal);
		}
		if(controller->crossDebounceChannel->value)
		{
			if(actorInstance->actorStatus->timeWithoutWallCollision < 0.25f)
			{
//				bmVectorSet(delta, 0.0f, METERTOUNIT(7.0f), 0.0f, 1.0f);
				bmVectorScaleToLength(delta, actorInstance->actorStatus->waterResetPoint, METERTOUNIT(7.0f));
				AddActorVelocity(actorInstance->actorBehaviour, delta);
			}
		}
		// CMD: update orientation, position etc here!
		// CMD: Update rotation based on velocity 

		// CMD: If we are on the ground compute base our orientation
		// on our velocity

		delta[X] = actorInstance->actorInstance.position[X] - actorStatus->positionLastFrame[X];
		delta[Y] = 0.0f;
		delta[Z] = actorInstance->actorInstance.position[Z] - actorStatus->positionLastFrame[Z];
		delta[W] = 1.0f;

		length = bmSqrt(delta[X]*delta[X] + delta[Z]*delta[Z]);

		if(length)
		{
			TBVector		v;
			TBQuaternion	q, targetQuat;

			v[X] = -(delta[Z]/length);						// CR: Fill in the correct axis
			v[Y] =  0.0f;
			v[Z] =  (delta[X]/length);
			v[W] =  ((length*HALFPI)/diameter);				// CR: And the amount of rotation

			// CMD: Contruct rotation quaternion
			bmRotationToQuat(q, v);

			// CMD: // CMD: Concat. quaternions
			bmQuatMultiply(targetQuat, actorInstance->actorInstance.orientation, q);

			// CMD: Normalise the result as they quickly go weird!
			utilNormaliseQuaternion(targetQuat);

			SetActorOrientation(actorInstance, targetQuat);
		}		

		// CMD: respond to control input
		if(~actorInstance->flags & ACTORFLAG_NOVELOCITY)
		{
			CharacterRespondToGloverBallMovementInput(controller,cameraInfo,actorInstance);
		}
		break;
	case STATE_CRATE:
		break;
	case STATE_CAGED:
		noExitState = false;
		if (RespondToStateInit(actorInstance))
		{
			actorInstance->actorStatus->currentMoveState = STATE_CAGEDMOVE;
		}
		SetupCameraForCage(actorInstance);

		if(!CheckIfPlayingAnimation(actorInstance, "cageidle1"))
		{
			SetupTazPostCageIdle(actorInstance);
			ChangeState(actorInstance,STATE_CAGEDMOVE);
		}
		else
		{
			if(characterSounds.taz == -1 && camera[0].fade.GetFadeValue() == 0)
				CallbackWhenSampleFinished((characterSounds.taz=PlaySample("rattlecage.wav",200)),CharacterSoundsFinishedCallback,&characterSounds.taz);
		}
		break;
	case STATE_CAGEDMOVE:
		noExitState = false;
		// check for spin mode
		if(CheckForSpin(player)) break;

		// check if player is trying to move
		if (controller->mag1 > 0.2f)
		{
			// process character movement
			if (GetPriorityOfCurrentAnimation(actorInstance) <= MOVE)
			{
				CharacterRespondToMovementInput(controller,cameraInfo,actorInstance);
			}
		}

		// find the appropriate movement animation to play
		animationToPlay = ChooseMovementAnimationForTaz(controller,actorInstance);
		if (actorInstance->actorStatus->timeInRunAnimation > 0.5f) // if running for more that 0.5 secs
		{
			// check to see if going from a run to a stand
			if (animationToPlay==baFindAnimSegment(&actorInstance->actorInstance, "idle1"))
			{
				if (CheckIfPlayingAnimation(actorInstance,"run"))
				{
					PlaySample("skidstop.wav",255,actorInstance->actorInstance.position);
					PlayAnimationByName(actorInstance,"runstop",1.5f,0,0,0.1f,SLIDESTOP);
					ChangeState(actorInstance, STATE_SKIDSTOP);
					break;
					// TP: RequestStateChange(actorInstance, STATE_SKIDSTOP);
				}
			}
		}
		if (animationToPlay==baFindAnimSegment(&actorInstance->actorInstance, "idle1"))
		{
			if (actorStatus->timeSinceLastTiltingCollision < 0.15f)
			{
				// TP: touching tilting object so wobble
				animationToPlay = baFindAnimSegment(&actorInstance->actorInstance, "wobble");
			}
		}

		// play new animation
		if (CurrentAnimation(actorInstance)!=animationToPlay)	// am i already playing this anim
			PlayAnimationBySegmentPointer(actorInstance,animationToPlay,1,1,0,0.1f,MOVE);

		if (!FindActorInstanceInMap("cratespawn_tp",NULL,&map))
		{
			actorInstance->actorStatus->currentMoveState = STATE_MOVE;
			ChangeState(actorInstance,STATE_MOVE);
		}
		break;
	case STATE_ELECTROCUTED:
		if (RespondToStateExit(actorInstance)) 
		{
			RespondToStateExitPost(actorInstance,actorStatus);
			if(actorInstance->characterInfo->powerUpInfo.invisible)
			{
				EnableActorLineOnlyRenderMode(&actorInstance->actorInstance, &invisLineParms);
				// NH: baSetActorRenderMode(&actorInstance->actorInstance, 0, BACTRENDERMODE_ZONLY, NULL);
				// NH: baSetActorRenderMode(&actorInstance->actorInstance, 1, BACTRENDERMODE_OUTLINED, &invisLineParms);
			}
			else
			{
				EnableActorCartoonRenderMode(&actorInstance->actorInstance);
			}
			break;
		}
		
		if(!CheckIfPlayingAnimation(actorInstance, "electrocute"))
		{
			FlushAnimationQueue(actorInstance);
			PlayAnimationByName(actorInstance, "electrorecover", 1.0f, 0, 0, 0.1f, DEATH);

			ChangeState(actorInstance,STATE_RECOVER);

			//-----------------STATE CHANGE-----------------
			if(actorInstance->characterInfo->powerUpInfo.invisible)
			{
				EnableActorLineOnlyRenderMode(&actorInstance->actorInstance, &invisLineParms);
				// NH: baSetActorRenderMode(&actorInstance->actorInstance, 0, BACTRENDERMODE_ZONLY, NULL);
				// NH: baSetActorRenderMode(&actorInstance->actorInstance, 1, BACTRENDERMODE_OUTLINED, &invisLineParms);
			}
			else
			{
				EnableActorCartoonRenderMode(&actorInstance->actorInstance);
			}
			break;
		}
		UpdateElectrocutionEffect(actorInstance);
		break;
	case STATE_SHOCKED:
		if (RespondToStateExit(actorInstance)) 
		{
			EnableActorCartoonRenderMode(&actorInstance->actorInstance);
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}
		
		if(!CheckIfPlayingAnimation(actorInstance, "electrocute"))
		{
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			//-----------------STATE CHANGE-----------------
			EnableActorCartoonRenderMode(&actorInstance->actorInstance);
			break;
		}
		UpdateElectrocutionEffect(actorInstance);
		break;
	case STATE_GROUNDELECTROCUTED:
		if (RespondToStateExit(actorInstance)) 
		{
			RespondToStateExitPost(actorInstance,actorStatus);
			actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_WALLVELCORRECTION;
			actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_NOMOVE;
			actorInstance->actorAnimation->useMotionBone = true;
			actorInstance->actorStatus->electrocutionTime = 0.0f;
			EnableActorCartoonRenderMode(&actorInstance->actorInstance);
			break;
		}

		if((actorInstance->actorStatus->electrocutionTime += 1.0f/fps) > 1.0f)
		{
			actorInstance->actorStatus->electrocutionTime = 0.0f;
			FlushAnimationQueue(actorInstance);
			PlayAnimationByName(actorInstance, "waterbounce", 1.0f, 1, 0, 0.1f, NONE);
			actorInstance->actorBehaviour->physics.velocity[1] = 0.0f;
			AddActorSpeed(actorInstance->actorBehaviour, yAxis, METERTOUNIT(10.0f));

			if (characterSounds.taz == -1)
				CallbackWhenSampleFinished(characterSounds.taz = PlaySample("tazburnt.wav", 255), CharacterSoundsFinishedCallback, &characterSounds.taz);

			actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_NOMOVE;
			actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;
			actorInstance->actorAnimation->useMotionBone = FALSE;

			ChangeState(actorInstance, STATE_SPRUNG);
			//-----------------STATE CHANGE-----------------
			EnableActorCartoonRenderMode(&actorInstance->actorInstance);
			break;
		}
		UpdateElectrocutionEffect(actorInstance);
		break;
	case STATE_BADFOOD:
		if (RespondToStateInit(actorInstance))
		{
			// TP: This doesn't work well, leave out for now
			/*
			SetLightRamp(RAMP_SUNSET);
			EnableActorCartoonRenderMode(&gameStatus.player1->actorInstance);
			*/
		}

		if (CheckIfPlayingAnimation(actorInstance, "tntinside", bkCRC32((const uchar *) "tntinside",9,0)) || 
			CheckIfPlayingAnimation(actorInstance, "badfood", bkCRC32((const uchar *) "badfood",7,0)))
		{
			CreateCharacterChilliPepperSmoke(actorInstance);
		}

		if (RespondToStateExit(actorInstance)) 
		{
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}

		if (CheckIfPlayingAnimation(actorInstance,NULL)==FALSE)
		{
			// JW: Change Taz back to normal after being exploded
			/*
			SetLightRamp(LIGHTS_DEFAULT_RAMP);
			EnableActorCartoonRenderMode(&gameStatus.player1->actorInstance);
			*/

			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			//-----------------STATE CHANGE-----------------
		}
		break;
	case STATE_PLAYANIMATION:
		// TP: Got stuck in here once
		if (RespondToStateExit(actorInstance)) 
		{
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}
		switch(actorInstance->actorAnimation->waitingToEnd)
		{
		case WAITING_CANNONFAIL:
			if(!CheckIfPlayingAnimation(actorInstance, "cannonfail"))
			{
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance,"cannonfail02",1.0f,0,0,0.1f,NONE);
				actorInstance->actorAnimation->useMotionBone = TRUE;
				actorInstance->actorAnimation->waitingToEnd = WAITING_NONE;
				break;
			}
			break;
		case WAITING_WATERBOUNCE:
			if(!CheckIfPlayingAnimation(actorInstance, "waterbounce"))
			{
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance, "getup", 1.0f, 0, 0, 0.1f, REACT);
				actorInstance->actorAnimation->useMotionBone = TRUE;
				actorInstance->actorAnimation->waitingToEnd = WAITING_NONE;
			}
			break;
		case WAITING_NONE:
		default:
			if (CheckIfPlayingAnimation(actorInstance,NULL)==FALSE)
			{
				ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
			//-----------------STATE CHANGE-----------------
			}
			break;
		}
		break;
	case STATE_PLAYANIMANDFREEZE:
		// CMD: don't want to do anything in here
		break;
	case STATE_ENTERINGPHONEBOX:
		if (RespondToStateExit(actorInstance)) 
		{
			TBVector	tempVector;

			SETVECTOR(tempVector, 0.0f, METERTOUNIT(1.0f), 0.0f, 1.0f);

			// JW: Send camera back to look at Taz
			SetCameraLookAtFollowActorInstance(&camera[0], gameStatus.player1, &gameStatus.player1->actorInstance,(TBActorNodeInstance *)NULL);
			SetCameraLookAtActorInstanceOffset(&camera[0],tempVector);
			
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}
		break;
	case STATE_MESMERISED:
		if(RespondToStateInit(actorInstance))
			musicSystem.RequestMood(0,STREAMMOOD_SLOW);

		if(RespondToStateExit(actorInstance))
		{
			MUSICBOXEXTRAINFO	*musicBoxInfo;

			// CMD: spin through extras list and fade notes around taz's head
			for(i = (staveExtraSystem->noofInstances-1); i >= 0;--i)
			{
				musicBoxInfo = (MUSICBOXEXTRAINFO*)staveExtraSystem->dataPtrs[i];
				if(musicBoxInfo)
				{
					if(musicBoxInfo->type == MUSICBOXEXTRA_HEAD) musicBoxInfo->type = MUSICBOXEXTRA_FADE;
				}
			}
			
			musicSystem.RequestMood(0,STREAMMOOD_NORMAL);
			RespondToStateExitPost(actorInstance, actorStatus);
			break;
		}
		CharacterRespondToMesmerisedMovementInput(controller,cameraInfo,actorInstance);
		break;
	case STATE_ONFOUNTAIN:
		if (RespondToStateExit(actorInstance)) 
		{
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}

		//check for player pressing jump
		if(controller1.crossDebounceChannel->value)
		{
			//reset fountain
//			if(actorStatus->actorAttachedTo->fountainInfo->idle)
			{
				actorStatus->actorAttachedTo->fountainInfo->fountainState = FOUNTAIN_MINIMUM;
				SetActorPosition(actorStatus->actorAttachedTo, actorStatus->actorAttachedTo->fountainInfo->initialPosition);
			}
			actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_NOGRAVITY;
			AddActorToCollisionCache(&collisionCache, actorStatus->actorAttachedTo, 0);
			actorStatus->actorAttachedTo->fountainInfo->characterOnFountain = NULL;
			actorStatus->actorAttachedTo = NULL;

			PlayAnimationByName(actorInstance,"jumprise",0.6f,0,0,0.1f,JUMP);	// If I could play jump then
			CalculateSpeedBeforeJump(actorInstance);							// fill in current speed before jump
			actorInstance->actorInstance.position[Y] += 1.0f;					// bodge to avoid sticking to ground

			// zero vertical speed before adding jump
			actorInstance->actorBehaviour->physics.velocity[Y] = 0.0f;
			SETVECTOR(tvec,0.0f,1.0f,0.0f,1.0f);
			
			if (player->actorInstance->actorStatus->multiplayer.effect == PLAYEREFFECTSLOMO)
				AddActorSpeed( actorInstance->actorBehaviour, tvec, METERTOUNIT(3.0f));//METERTOUNIT(6.324f)
			else
				AddActorSpeed( actorInstance->actorBehaviour, tvec, METERTOUNIT(7.0f));//7

			// Queue fall animation
			PlayAnimationByName(actorInstance,"jumpfall",0.4f,1,1,0.2f,JUMP);
			actorPhysics->flags &= ~PHYSFLAG_WALLVELCORRECTION;
			actorInstance->actorAnimation->useMotionBone = FALSE;

			// keep track of position prior to jump move
			ChangeState(actorInstance,STATE_JUMP);
			//-----------------STATE CHANGE-----------------
			break;
		}
		break;
	case STATE_FRONTENDUSE:// PP: using a front-end item, eg. setting a volume level: Taz himself can't be controlled in this state
		if (RespondToStateExit(actorInstance)) 
		{
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}
		break;
	case STATE_FRONTEND:
		noExitState = false;
		switch(actorStatus->frontEndSubState)
		{
		case STATE_IDLE:
			break;
		case STATE_MOVE:
			break;
		case STATE_SPINUP:
			if(!CheckIfPlayingAnimation(actorInstance, "spinup"))
			{
				ChangeActorInstance(actorInstance,FindActorInActorList("spindevil.obe"),5,NULL);

				ENABLE_LINE_OR_CARTOON_RENDER_MODE(&actorInstance->actorInstance);
				
				PlayAnimationByName(actorInstance,"spin1",6.0f,1,1,0.2f,JUMP);

				actorStatus->frontEndSubState = STATE_SPIN;
			}
			break;
		case STATE_SPIN:
			CreateImpactClouds(actorInstance->actorInstance.position, actorInstance->actorBehaviour->physics.velocity, NULL, METERTOUNIT(0.2f), 0.75f, 0.0f, SMOKE_TAZSPIN);
			break;
		case STATE_SPINDOWN:
			break;
		}
		break;
	case STATE_CATATONIC:
		if (RespondToStateExit(actorInstance)) 
		{
			SETVECTOR(actorInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_NOGRAVITY;
			RespondToStateExitPost(actorInstance,actorStatus);
			actorInstance->characterInfo->powerUpInfo.drawBubbles = FALSE;
			break;
		}
		//suspend taz
		SETVECTOR(actorInstance->actorBehaviour->physics.velocity, 0.0f, 0.0f, 0.0f, 1.0f);
		actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_NOGRAVITY;
		//set fade details
		node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "snoutstem");
		if(node)
		{
			bdSetIdentityObjectMatrix();
			baGetNodesWorldPosition(&actorInstance->actorInstance, node, worldSpace);
			worldSpace[3] = 1.0f;
			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
			bdSetIdentityObjectMatrix();
			bdProjectVertices(screenSpace, worldSpace, 1);
			fade->SetCircleFadeTo(screenSpace[0], screenSpace[1]);

			if(actorInstance->characterInfo->powerUpInfo.drawBubbles)
			{
				//create bubbles as Taz drowns
				CreateBubbles(actorInstance, METERTOUNIT(0.1f), 1.0f, WHITE_BUBBLE);
			}
		}
		break;
	case STATE_CATATONICPHYS:
		if(RespondToStateExit(actorInstance))
		{
			RespondToStateExitPost(actorInstance, actorStatus);
			break;
		}
		break;
	case STATE_CATATONICDELAY:
		if(RespondToStateExit(actorInstance))
		{
			SETVECTOR(actorInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_NOGRAVITY;
			RespondToStateExitPost(actorInstance, actorStatus);
			break;
		}
		//suspend taz
		SETVECTOR(actorInstance->actorBehaviour->physics.velocity, 0.0f, 0.0f, 0.0f, 1.0f);
		actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_NOGRAVITY;
		if((actorStatus->catatonicDelay -= fTime) < 0.0f)
		{
			SETVECTOR(actorInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_NOGRAVITY;
			ChangeState(actorInstance, STATE_JUMP);
			// TP: RequestStateChange(actorInstance, STATE_JUMP);
		}
		break;
#ifndef CONSUMERDEMO
	case STATE_INTRANSPORT:
		if(RespondToStateExit(actorInstance))
		{
			SETVECTOR(actorInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_NOGRAVITY;
			actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_NOMOVE;
			RespondToStateExitPost(actorInstance, actorStatus);
			if (actorStatus->postCatatonicTrigger)
			{
				SendTrigger(actorStatus->postCatatonicTrigger, actorInstance->actorInstance.position);
				DeleteTriggerInfo(actorStatus->postCatatonicTrigger);
				actorStatus->postCatatonicTrigger = NULL;
			}
			break;
		}
		//suspend taz
		SETVECTOR(actorInstance->actorBehaviour->physics.velocity, 0.0f, 0.0f, 0.0f, 1.0f);
		actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_NOGRAVITY;
		if(actorInstance->characterInfo&&(actorInstance->characterInfo->transportInfo == NULL))
		{
			if((actorStatus->catatonicDelay -= fTime) < 0.0f)
			{
				SETVECTOR(actorInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
				actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_NOGRAVITY;
				actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_NOMOVE;
				if (actorStatus->postCatatonicTrigger)
				{
					SendTrigger(actorStatus->postCatatonicTrigger, actorInstance->actorInstance.position);
					// TP: need to free this, the copytrigger function mallocs a new triggerinfo
					DeleteTriggerInfo(actorStatus->postCatatonicTrigger);
					actorStatus->postCatatonicTrigger = NULL;
				}
				ChangeState(actorInstance, STATE_JUMP);
				// TP: RequestStateChange(actorInstance, STATE_JUMP);
			}
		}
		break;
	case STATE_ATLASSPINUP:
		musicSystem.RequestMood(0,STREAMMOOD_FAST);

// CPW: 		EnableCameraAngleAboveHorizontal(cameraInfo,0);
		if(!CheckIfPlayingAnimation(actorInstance, "spinup"))
		{
			// set spin behaviour
			// CPW: SetActorMinFricAngle(actorInstance->actorBehaviour,1.0f);

			ChangeActorInstance(actorInstance,FindActorInActorList("spindevil.obe"),5,NULL);
			ENABLE_LINE_OR_CARTOON_RENDER_MODE(&actorInstance->actorInstance);
			PlayAnimationByName(actorInstance,"spin1",6.0f,1,1,0.0f,JUMP);

			// CPW: actorPhysics->flags |= PHYSFLAG_WALLGROUNDCOLLFIX;
			// CPW: actorPhysics->flags &= ~PHYSFLAG_WALLVELCORRECTION;
			ChangeState(actorInstance,STATE_ATLASSPIN);
			// CPW: SetActorVelocity(actorInstance->actorBehaviour,actorStatus->velBeforeJump);
		}
		CharacterRespondToSpinMovementInput(controller,cameraInfo,actorInstance);
		break;

	case STATE_ATLASSPIN:
		if(	(!controller->circleChannel->value) ||
			controller->crossDebounceChannel->value ||
			RespondToStateExit(actorInstance))
		{
			// reset behaviour
			if(actorInstance->characterInfo->spinInfo)
			{
				actorInstance->characterInfo->spinInfo->spinUpdated = FALSE;
			}

			actorStatus->targetSpinSpeed = 0.0f;

			FlushAnimationQueue(actorInstance);
			bmQuatCopy(tempQuat,actorInstance->actorInstance.orientation);
			ChangeActorInstance(actorInstance,FindActorInActorList(actorInstance->actorName),5,NULL);

			EnableActorCartoonRenderMode(&actorInstance->actorInstance);
			bmQuatCopy(actorInstance->actorInstance.orientation,tempQuat);
			// CPW: ResetCharacterAfterJump(actorInstance);
			// CPW: actorPhysics->flags &= ~PHYSFLAG_WALLGROUNDCOLLFIX;
			// CPW: actorPhysics->flags |= PHYSFLAG_WALLVELCORRECTION;
			FlushAnimationQueue(actorInstance);

			PlayAnimationByName(actorInstance, "spindown", 2.0f, 0, 0, 0.0f, MOVE);

			StopChannel(&actorStatus->soundHandle[0]);
			actorStatus->soundHandle[0] = PlaySample("taz_gen_24a.wav",255,actorInstance->actorInstance.position);

			StopChannel(&actorStatus->soundHandle[1]);
			
			// PP: Setup costume even if none is being worn - this sets up any costume accessories specified by cheats, eg. the DIGITALWATCH cheat
			SetupCostume(actorInstance,actorInstance->characterInfo->costume);
			ChangeState(actorInstance,STATE_ATLASSPINDOWN);
			//-----------------STATE CHANGE-----------------
			break;
		}

		CharacterRespondToSpinMovementInput(controller,cameraInfo,actorInstance);

		break;

	case STATE_ATLASSPINDOWN:
		if(!CheckIfPlayingAnimation(actorInstance, "spindown"))
		{
			ChangeState(actorInstance,STATE_ATLASSPHERES);

			// CPW: do some stuff that has to be done, apparently
			// CPW: actorInstance->actorAnimation->useMotionBone = FALSE;
			// CPW: SetActorMinFricAngle(actorInstance->actorBehaviour,1.0f);

			// CPW: actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_WALLGROUNDCOLLFIX;
			// CPW: actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;
		}

		break;


	case STATE_ATLASSPHERES:
		noExitState = false;

		//check for player pressing spin
		if(controller->circleDebounceChannel->value)
		{
			break;

			FlushAnimationQueue(actorInstance);
			PlayAnimationByName(actorInstance, "spinup", 2.0f, 0, 0, 0.0f, NONE);

			ChangeState(actorInstance,STATE_ATLASSPINUP);

			//-----------------STATE CHANGE-----------------
			break;
		}

		// find the appropriate movement animation to play
		animationToPlay = ChooseMovementAnimationForTazInSphere(controller, actorInstance);

		// play new animation
		if(CurrentAnimation(actorInstance)!=animationToPlay)	// am i already playing this anim
		{
			FlushAnimationQueue(actorInstance);
			PlayAnimationBySegmentPointer(actorInstance,animationToPlay,1,1,0,0.1f,MOVE);
		}

		if(controller->mag1)
			CharacterRespondToMovementInputInSphere(controller, cameraInfo, actorInstance);

		/*if (map.sceneNumber == SCENE_AQUA)
		{
			if (RespondToStateExit(actorInstance)) 
			{
				ResetCharacterAfterJump(actorInstance);
				SetWorldGravity(NORMAL_WORLD_GRAVITY);
				SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
				SetActorFriction(actorInstance->actorBehaviour,0.0f);
				actorInstance->actorAnimation->useMotionBone = TRUE;
				RespondToStateExitPost(actorInstance,actorStatus);

				break;
			}

			if((actorInstance->actorStatus->timeWithoutWaterSlideCollision > 1.0f))
			{
				// I have slid onto proper ground
				// flush anim ready for move state
				ResetCharacterAfterJump(actorInstance);
				SetWorldGravity(NORMAL_WORLD_GRAVITY);
				SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
				SetActorFriction(actorInstance->actorBehaviour,0.0f);
				actorInstance->actorAnimation->useMotionBone = TRUE;
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance, "getup", 1.0f, 0, 0, 0.1f, NONE);
				PlaySample("taz getup from water slide",255,actorInstance->actorInstance.position);
				StopChannel(&actorStatus->soundHandle[0]);
				ChangeState(actorInstance,STATE_GETUPFROMSLIDE);
				//-----------------STATE CHANGE-----------------
				break;
			}
		}*/

		break;
#endif

	case STATE_BUBBLEGUM:
		if (RespondToStateExit(actorInstance)) 
		{
			POWERUPINFO	*ptr = &actorInstance->characterInfo->powerUpInfo;
			TBVector	offset;
			PlaySample("gumpopballoon.wav",255,actorInstance->actorInstance.position);
			SETVECTOR(offset, 0.0f, 120.0f, 0.0f, 1.0f);
			SetActorCollSphere(actorInstance->actorBehaviour, offset, METERTOUNIT(0.5f));
			ptr->bubbleGum = FALSE;
			actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_NOGRAVITY;
			ResetCharacterAfterJump(actorInstance);
			ptr->clock = 0.0f;
			ptr->randomClock = 0.0f;
			ptr->state = -1;
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}

		musicSystem.RequestMood(0,STREAMMOOD_NORMAL);

		// check if player is trying to move
		if (controller->mag1 > 0.2f)
		{
			// process character movement
			CharacterRespondToMovementInput(controller,cameraInfo,actorInstance);
		}

		break;
	case STATE_SPLATTED:
		if(RespondToStateExit(actorInstance))
		{
			RespondToStateExitPost(actorInstance, actorStatus);
			break;
		}
		if(actorInstance->actorStatus->timeWithoutGroundCollision < 0.3f)
		{
			if(CheckIfPlayingAnimation(actorInstance,"recover11a")) FlushAnimationQueue(actorInstance, 1);
			else FlushAnimationQueue(actorInstance);

			PlayAnimationByName(actorInstance,"recover11c",1.0f,0,1,0.1f,DEATH);
			ChangeState(actorInstance, STATE_SPLATRECOVER);
			// TP: RequestStateChange(actorInstance, STATE_SPLATRECOVER);
		}
		break;
	case STATE_SPLATRECOVER:
		if(!CheckIfPlayingAnimation(actorInstance,"recover11c")&&!CheckIfPlayingAnimation(actorInstance,"recover11a"))
		{
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
		}
		break;
	case STATE_ENTERINGXDOOR:
		if (UpdateXDoorState(actorInstance))
		{
			ChangeState(actorInstance,actorInstance->actorStatus->currentMoveState);
		}
		break;
	case STATE_INPORTAL:
		if(RespondToStateExit(actorInstance))
		{
			RespondToStateExitPost(actorInstance, actorStatus);
			break;
		}

		CalculatePortalWorldGravity(actorInstance);

		musicSystem.RequestMood(0,STREAMMOOD_NORMAL);

		// check if player is trying to move
		if (controller->mag1 > 0.2f)
		{
			// process character movement
			if (GetPriorityOfCurrentAnimation(actorInstance) <= MOVE)
			{
				CharacterRespondToPortalMovementInput(controller,cameraInfo,actorInstance);
			}
		}

		// find the appropriate movement animation to play
		animationToPlay = ChooseMovementAnimationForTaz(controller,actorInstance);
		if (actorInstance->actorStatus->timeInRunAnimation > 0.5f) // if running for more that 0.5 secs
		{
			// check to see if going from a run to a stand
			if (animationToPlay==baFindAnimSegment(&actorInstance->actorInstance, "idle1"))
			{
				if (CheckIfPlayingAnimation(actorInstance,"run"))
				{
					PlaySample("skidstop.wav",255,actorInstance->actorInstance.position);
					PlayAnimationByName(actorInstance,"runstop",1.5f,0,0,0.1f,SLIDESTOP);
					ChangeState(actorInstance, STATE_SKIDSTOP);
					break;
					// TP: RequestStateChange(actorInstance, STATE_SKIDSTOP);
				}
			}
		}

		// play new animation
		if (CurrentAnimation(actorInstance)!=animationToPlay)	// am i already playing this anim
			PlayAnimationBySegmentPointer(actorInstance,animationToPlay,1,1,0,0.1f,MOVE);

		break;
	case STATE_CHILLIPEPPER:
		if (RespondToStateInit(actorInstance))
		{
			actorInstance->actorStatus->currentMoveState = STATE_CHILLIPEPPER;

			if (GetPriorityOfCurrentAnimation(actorInstance)==JUMP)
			{
				// TP: If we have come from the jump state then make sure the new animation is played
				ClearCurrentAnimationPriority(actorInstance);	
			}
		}
		if (RespondToStateExit(actorInstance)) 
		{
			RespondToStateExitPost(actorInstance,actorStatus);
			break;
		}
	
		musicSystem.RequestMood(0,STREAMMOOD_NORMAL);

		// check for not on ground for more that x secs
		if ((actorStatus->timeWithoutGroundCollision > 0.15f)&&(!controller->l3Channel->value))
		{
			// must now choose a fall

			// am I colliding with a wall, if so go to slide state
			if((actorPhysics->flags&PHYSFLAG_WALLCOLL)&&(actorInstance->actorStatus->timeWithoutGroundCollision > 0.3f)
				&&((!actorPhysics->coll)||(actorPhysics->lastAverageNormal[Y]>0.4f)))
			{
				// flush anim ready for move state
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance,"slide",0.1f,1,0,0.1f,JUMP);
				actorInstance->actorAnimation->useMotionBone = FALSE;
				ZEROVECTOR(actorInstance->actorBehaviour->physics.velocity);
		
				if (actorStatus->soundHandle[0]==-1)
				{
					actorStatus->soundHandle[0] = PlaySample("taz_gen_08a.wav",255,actorInstance->actorInstance.position);
				}
				ChangeState(actorInstance,STATE_SLIDE);
				//-----------------STATE CHANGE-----------------
				break;
			}
			// am I very high and not colliding with anything, if so do a big fall
/*			if ((GetHeightOfActor(actorInstance,0.0f) > METERTOUNIT(3.5f)))
			{
				PlayAnimationByName(actorInstance,"death3a2",1,0,0,0,REACT);
				PlayAnimationByName(actorInstance,"fall",1,1,1,0.1f,REACT);
				ChangeState(actorInstance,STATE_BIGFALL);
				EnableCameraBigFall(cameraInfo, actorInstance);

				if ((bmRand()%2)==0)
				{
					if(characterSounds.taz == -1)
					{
						CallbackWhenSampleFinished((characterSounds.taz=PlaySample("taz_voc_31b.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.taz);
					}
				}
				//-----------------STATE CHANGE-----------------
				break;
			}*/

			// if doing a medium height fall then do a fall animation
			if (GetHeightOfActor(actorInstance,0.0f) > METERTOUNIT(1.0f))
			{
				CalculateSpeedBeforeJump(actorInstance);	// fill in speed before jump
				PlayAnimationByName(actorInstance,"jumpfall",0.4f,1,0,0.1f,JUMP);
				actorInstance->actorAnimation->useMotionBone = FALSE;
				ChangeState(actorInstance,STATE_JUMP);
				//-----------------STATE CHANGE-----------------
				break;
			}
		}

		// check for bite attack
		if (controller->triangleDebounceChannel->value)
		{
			if (actorInstance->characterInfo->inMouth)
			{
				PlayAnimationByName(actorInstance,"attack1",1,0,0,0.2f,ATTACK);
				TazSpit(actorInstance);
				ChangeState(actorInstance,STATE_SPIT);
				//-----------------STATE CHANGE-----------------
				break;
			}
			else
			{
				PlaySample("tazbite.wav",255,actorInstance->actorInstance.position);
				PlayAnimationByName(actorInstance,"attack1",1,0,0,0.2f,ATTACK);
				ChangeState(actorInstance,STATE_BITE);
				//-----------------STATE CHANGE-----------------
				break;
			}
		}

		// check for scare move
		if (controller->squareChannel->value)
		{
			if(!CheckIfPlayingAnimation(actorInstance, "chillifire"))
			{
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance,"chillifire",1.0f,0,0,0.1f,ATTACK);
				RequestRumbleEffect(controller, 250, 0.0f, 0.0f, 3.0f, 0.5f, WAVEFORM_RANDOM);
			}
		}
		//chilli activity
		if(CheckIfPlayingAnimation(actorInstance, "chillifire"))
		{
			CreateChilliPepperFlameThrower(actorInstance);
		}

		// check for player pressing jump
		if(CheckForJump(player)) break;

		// check for spin mode
		if(CheckForSpin(player)) break;

		// check for tiptoe move
		if (controller->l2Channel->value)
		{
			PlayAnimationByName(actorInstance,"tiptoeidle1",1,1,0,0.1f,TIPTOE);
			ChangeState(actorInstance,STATE_TIPTOE);
			//-----------------STATE CHANGE-----------------
			break;
		}

		// TP: check for entering look around mode
		if ((controller->r2Channel->value)&&(map.sceneNumber != SCENE_ZOOBOSS)&&(map.sceneNumber != SCENE_CITYBOSS)&&(map.sceneNumber != SCENE_WESTBOSS)&&(map.sceneNumber != SCENE_TAZPREBOSS)&&(map.sceneNumber != SCENE_TAZBOSS)&&(map.sceneNumber < SCENE_MUSEUMVEHICLERACE))
		{
			FlushAnimationQueue(actorInstance);
			PlayAnimationByName(actorInstance,"idle1",1.0f,1,0,0.1f,NONE);
			player->camera->flags |= CAMERA_LOOKAROUND;
			ChangeState(actorInstance,STATE_ENTERLOOKAROUND);
			// CMD: check to see if fire pickup is active, if it is "fireend"message will never arrive from anim
			if(actorInstance->characterInfo)
			{
				if(actorInstance->characterInfo->powerUpInfo.state == POWERUPSTATE_CHILLION)
				{
					actorInstance->characterInfo->powerUpInfo.state = POWERUPSTATE_CHILLIOFF;
				}
			}
			break;
		}

		//check for surface terrain
		if(actorInstance->actorBehaviour->physics.currentTerrain & TERRAIN_ICE)
		{
			ChangeState(actorInstance, STATE_ONICE);
			// TP: RequestStateChange(actorInstance, STATE_ONICE);
			SetActorFriction(actorInstance->actorBehaviour, METERTOUNIT(0.0f));
			break;
		}

		// check for idle move
		animationToPlay = ChooseIdleAnimationForTaz(controller, actorInstance, cameraInfo);
		if(animationToPlay)
		{
			FlushAnimationQueue(actorInstance);
			PlayAnimationBySegmentPointer(actorInstance, animationToPlay, 1.0f, 0, 0, 0.1f, IDLE);
			ChangeState(actorInstance,STATE_IDLE);
			//-----------------STATE CHANGE-----------------
			break;
		}

		// check if player is trying to move
		if (controller->mag1 > 0.2f)
		{
			// process character movement
			if (GetPriorityOfCurrentAnimation(actorInstance) <= MOVE)
			{
				CharacterRespondToMovementInput(controller,cameraInfo,actorInstance);
			}
		}

		// find the appropriate movement animation to play
		animationToPlay = ChooseMovementAnimationForTaz(controller,actorInstance);
		if (actorInstance->actorStatus->timeInRunAnimation > 0.5f) // if running for more that 0.5 secs
		{
			// check to see if going from a run to a stand
			if (animationToPlay==baFindAnimSegment(&actorInstance->actorInstance, "idle1"))
			{
				if (CheckIfPlayingAnimation(actorInstance,"run"))
				{
					PlaySample("skidstop.wav",255,actorInstance->actorInstance.position);
					PlayAnimationByName(actorInstance,"runstop",1.5f,0,0,0.1f,SLIDESTOP);
					ChangeState(actorInstance, STATE_SKIDSTOP);
					break;
					
					// TP: RequestStateChange(actorInstance, STATE_SKIDSTOP);
				}
			}
		}
		// play new animation
		if (CurrentAnimation(actorInstance)!=animationToPlay)	// am i already playing this anim
			PlayAnimationBySegmentPointer(actorInstance,animationToPlay,1,1,0,0.1f,MOVE);

		break;
	}

	if (noExitState)
	{
		bkPrintf("*** WARNING *** No exit state has been written for state %s\n", CharState[actorStatus->currentState]);
	}

	if (changeStateOnly)
	{
		return stateChanged;
	}
	else
	{
		return (actorStatus->currentState!=stateIn);
	}
}

/* --------------------------------------------------------------------------------
   Function : RespondToStateInit
   Purpose : Returns true if it is the first time in a new state
   Parameters : ACTORINSTANCE
   Returns : true if new state else false
   Info : 
*/
bool RespondToStateInit(ACTORINSTANCE *actorInstance)
{
	if (actorInstance->actorStatus->firstTimeInState)
	{
		actorInstance->actorStatus->firstTimeInState = false;
		return true;
	}
	return false;
}

/* --------------------------------------------------------------------------------
   Function : RespondToStateExit
   Purpose : if a state change is requested then change
   Parameters : ACTORSTATUS
   Returns : 
   Info : 
*/
int RespondToStateExit(ACTORINSTANCE *actorInstance)
{
	noExitState = false;

	if (actorInstance->actorStatus->stateRequest!=-1)
	{
		switch(actorInstance->actorStatus->currentState)
		{
		case STATE_JUMP:
			if(actorInstance->actorStatus->stateRequest == STATE_BUBBLEGUM) return FALSE;
			break;
		case STATE_SPLATTED:
			if(actorInstance->actorStatus->stateRequest != STATE_SPLATRECOVER) return FALSE;
			break;
		case STATE_ONMINECART:
			if((actorInstance->actorStatus->stateRequest != STATE_PROJECTILE)&&(actorInstance->actorStatus->stateRequest != actorInstance->actorStatus->currentMoveState)) return FALSE;
			break;
		case STATE_MOVESQUASHED:
			if(actorInstance->actorStatus->stateRequest == STATE_JUMP) return FALSE;
			break;
		case STATE_INPORTAL:
			if(actorInstance->actorStatus->stateRequest != actorInstance->actorStatus->currentMoveState) return FALSE;
			break;
		case STATE_BALL:
		case STATE_MOUSE:
			switch(actorInstance->actorStatus->stateRequest)
			{
			case STATE_ZAPPEDINTOMOUSE:
			case STATE_ZAPPEDINTOBALL:
			case STATE_ZAPPEDINTOTAZ:
			case STATE_INTRANSPORT:
			case STATE_MOVE:
				break;
			default:
				return FALSE;
			}
			break;
		case STATE_SPINONICE:
			if(actorInstance->actorStatus->stateRequest != STATE_CATATONIC) return FALSE;
			break;
		}
		switch(actorInstance->actorStatus->stateRequest)
		{
		case STATE_BALL:
			bmQuatCopy(actorInstance->actorStatus->waterResetQuaternion, actorInstance->actorInstance.orientation);
			break;
		}

		ChangeState(actorInstance,actorInstance->actorStatus->stateRequest);
		actorInstance->actorStatus->timeInCurrentState = 0.0f;
		actorInstance->actorStatus->stateRequest = (ECharState)-1;
		stateChanged = true;
		return TRUE;
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : RespondToStateExitPost
   Purpose : if a state change is requested then change
   Parameters : ACTORSTATUS
   Returns : 
   Info : 
*/
void RespondToStateExitPost(ACTORINSTANCE *actorInstance,ACTOR_STATUS *actorStatus)
{
	if (actorStatus->stateRequestCallback) 
	{
		actorStatus->stateRequestCallback(actorInstance);
		actorStatus->stateRequestCallback = NULL;
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateSlidePitch
   Purpose : rotatates the projectile to point feet first in direction of arc
   Parameters : Projectile Actor Instance;
   Returns : 
   Info :
*/

void UpdateSlidePitch(ACTORINSTANCE *actor, float angle)
{
	TBVector			zeroVector = { 0.0f, 0.0f, 0.0f, 1.0f };
	TBVector			tempVec = { 1.0f, 0.0f, 0.0f, 0.0f };
	TBQuaternion		tempQuat, temp2Quat,tempQuat3;

	tempVec[3] = angle;
	bmRotationToQuat(temp2Quat, tempVec);			//feet first
	CreateQuatFromPoints(tempQuat, zeroVector, actor->actorBehaviour->physics.velocity, 0.0f);

	bmQuatMultiply(tempQuat3, temp2Quat, tempQuat);
	ActorRotate(actor, tempQuat3, 0.2f);
}

/* --------------------------------------------------------------------------------
   Function : MatchGroundOrientation
   Purpose : rotatates the projectile to point feet first in direction of arc
   Parameters : Projectile Actor Instance;
   Returns : 
   Info :
*/

void MatchGroundOrientation(ACTORINSTANCE *actor)
{
}

/* --------------------------------------------------------------------------------
   Function : UpdateController
   Purpose : updates the controllers internal structure
   Parameters : PADCONTROLSTATUS
   Returns : 
   Info : 
*/
void UpdateController(PADCONTROLSTATUS *controller)
{
	float tempFloat;

	// update analogues
	controller->x1 = (float)controller->x1Channel->value;
	controller->y1 = (float)controller->y1Channel->value;

	if (!debugDPad)
	{
		if (controller->dPadXChannel->value < 0)
			controller->x1 = -32760.0f;
		else if (controller->dPadXChannel->value > 0)
			controller->x1 = 32760.0f;

		if (controller->dPadYChannel->value < 0)
			controller->y1 = 32760.0f;
		else if (controller->dPadYChannel->value > 0)
			controller->y1 = -32760.0f;
	}

	controller->x2 = (float)controller->x2Channel->value;
	controller->y2 = (float)controller->y2Channel->value;

	controller->l1 = (float)controller->l1Channel->value;
	controller->r1 = (float)controller->r1Channel->value;
	controller->l2 = (float)controller->l2Channel->value;
	controller->r2 = (float)controller->r2Channel->value;

	// range -1 to 1
	controller->x1 = controller->x1 / (BIMAX);
	controller->x2 = controller->x2 / (BIMAX);
	controller->y1 = controller->y1 / (BIMAX);
	controller->y2 = controller->y2 / (BIMAX);

	controller->l1 = controller->l1 / (BIMAX);
	controller->r1 = controller->r1 / (BIMAX);
	controller->l2 = controller->l2 / (BIMAX);
	controller->r2 = controller->r2 / (BIMAX);

	// calculate magnitude of input
	controller->mag1 = bmSqrt(controller->x1*controller->x1 + controller->y1*controller->y1);
	controller->mag2 = bmSqrt(controller->x2*controller->x2 + controller->y2*controller->y2);
	
	// max mag to 1 and correct x,y's
	if (controller->mag1 > 1.0f)
	{
		tempFloat = 1.0f/controller->mag1;
		controller->x1 *= tempFloat;
		controller->y1 *= tempFloat;
		controller->mag1 = 1.0f;
	}
	if (controller->mag2 > 1.0f)
	{
		tempFloat = 1.0f/controller->mag2;
		controller->x2 *= tempFloat;
		controller->y2 *= tempFloat;
		controller->mag2 = 1.0f;
	}

	// fill in direction values if not in deadzone.
	if (controller->mag1)
	{
		// calculate direction for analogue 1
		controller->direction1 = Aabs((float)bmATan2(controller->x1, -controller->y1));
	}
	if (controller->mag2)
	{
		controller->direction2 = Aabs((float)bmATan2(controller->x2, -controller->y2));
	}

	// PP: the minimum stick magnitude required to break the idle time.
	// PP: Need this because I was getting a tiny magnitude without touching the pad, due to a slightly 'sticky' stick.
#define IDLETIME_MIN_MAG		0.1f			// CMD: 0.5f was waay to large a value and caused allsorts of problems going into idle when not wanted

	// check for any input at all and update idleTime
/*	if ((controller->mag1)||(controller->mag2)||(controller->dPadXChannel->value)||(controller->dPadYChannel->value)
		||(controller->crossChannel->value)||(controller->triangleChannel->value)||(controller->circleChannel->value)
		||(controller->squareChannel->value)||(controller->l1Channel->value)||(controller->l2Channel->value)
		||(controller->l3Channel->value)||(controller->r1Channel->value)||(controller->r2Channel->value)
		||(controller->r3Channel->value))
*/	if ((controller->mag1>IDLETIME_MIN_MAG)||(controller->mag2>IDLETIME_MIN_MAG)||(controller->dPadXChannel->value)||(controller->dPadYChannel->value)
		||(controller->crossChannel->value)||(controller->triangleChannel->value)||(controller->circleChannel->value)
		||(controller->squareChannel->value)||(controller->l2Channel->value)
		||(controller->l3Channel->value)||(controller->r1Channel->value)||(controller->r2Channel->value)
		||(controller->r3Channel->value)
// NH: Bloody hell, thats a big 'un!  (oo-err)
#if(BPLATFORM == PC)
		||(controller->menuBackChannel->value)||(controller->menuSelectChannel->value)||(controller->menuXChannel->value)||(controller->menuYChannel->value)
		||(controller->mb1Channel->value)||(controller->mb2Channel->value)||(controller->mxChannel->value)||(controller->myChannel->value)
#endif // NH: #if(BPLATFORM == PC)
		)
	{
		controller->idleTime = 0.0f;

		// PP: reset demo overall idle time
#ifdef CONUSMERDEMO
		demoIdleTime=0.0f;
#endif
	}
	else
	{
		if (fps)
		{
			controller->idleTime += 1.0f/fps;	// idle time in seconds independant of frame rate
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : CreateInputChannels
   Purpose : Create all input channels
   Parameters : PADCONTROLSTATUS
   Returns : 
   Info : Also creates channel map
*/
void CreateInputChannels(PADCONTROLSTATUS *controller, int padID)
{
	char string[16];

	// PP: the 'padID' parameter is effectively a player index
	controller->player=padID;

	// PP: The maximum number of channels that each input map can simultaneously support
	// PP: Ye'll have to increase this if you add more channels
#define MAX_INPUT_CHANNELS		81

	controller->idleTime = 0.0f;
	controller->inputMap = biCreateMap(MAX_INPUT_CHANNELS);

	sprintf(string, "%dx1", padID);
	controller->x1Channel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_BIANALOGUE, BICHANNELFLAG_RECORD, 0);
	sprintf(string, "%dx2", padID);
	controller->x2Channel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_BIANALOGUE, BICHANNELFLAG_RECORD, 0);
	sprintf(string, "%dy1", padID);
	controller->y1Channel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_BIANALOGUE, BICHANNELFLAG_RECORD, 0);
	sprintf(string, "%dy2", padID);
	controller->y2Channel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_BIANALOGUE, BICHANNELFLAG_RECORD, 0);

	sprintf(string, "%dx1DB", padID);
	controller->x1DebounceChannel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_BIANALOGUE, BICHANNELFLAG_DEBOUNCE | BICHANNELFLAG_RECORD, 0);
	sprintf(string, "%dx2DB", padID);
	controller->x2DebounceChannel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_BIANALOGUE, BICHANNELFLAG_DEBOUNCE | BICHANNELFLAG_RECORD, 0);
	sprintf(string, "%dy1DB", padID);
	controller->y1DebounceChannel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_BIANALOGUE, BICHANNELFLAG_DEBOUNCE | BICHANNELFLAG_RECORD, 0);
	sprintf(string, "%dy2DB", padID);
	controller->y2DebounceChannel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_BIANALOGUE, BICHANNELFLAG_DEBOUNCE | BICHANNELFLAG_RECORD, 0);

	sprintf(string, "%ddPadX", padID);
	controller->dPadXChannel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_BIANALOGUE, BICHANNELFLAG_RECORD, 0);
	sprintf(string, "%ddPadY", padID);
	controller->dPadYChannel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_BIANALOGUE, BICHANNELFLAG_RECORD, 0);

	sprintf(string, "%dcross", padID);
	controller->crossChannel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_ANALOGUE, BICHANNELFLAG_RECORD, 0);
	sprintf(string, "%dtriangle", padID);
	controller->triangleChannel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_ANALOGUE, BICHANNELFLAG_RECORD, 0);
	sprintf(string, "%dcircle", padID);
	controller->circleChannel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_ANALOGUE, BICHANNELFLAG_RECORD, 0);
	sprintf(string, "%dsquare", padID);
	controller->squareChannel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_ANALOGUE, BICHANNELFLAG_RECORD, 0);

	sprintf(string, "%dl1", padID);
	controller->l1Channel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_ANALOGUE, BICHANNELFLAG_RECORD, 0);
	sprintf(string, "%dl2", padID);
	controller->l2Channel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_ANALOGUE, BICHANNELFLAG_RECORD, 0);
	sprintf(string, "%dl3", padID);
	controller->l3Channel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_DIGITAL, BICHANNELFLAG_RECORD, 0);
	sprintf(string, "%dr1", padID);
	controller->r1Channel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_ANALOGUE, BICHANNELFLAG_RECORD, 0);
	sprintf(string, "%dr2", padID);
	controller->r2Channel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_ANALOGUE, BICHANNELFLAG_RECORD, 0);
	sprintf(string, "%dr3", padID);
	controller->r3Channel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_DIGITAL, BICHANNELFLAG_RECORD, 0);

	controller->startChannel = biCreateChannel(controller->inputMap, "start", BICHANNELTYPE_DIGITAL, 0, 1);
	controller->selectChannel = biCreateChannel(controller->inputMap, "select", BICHANNELTYPE_DIGITAL, 0, 1);
	controller->tabChannel = biCreateChannel(controller->inputMap, "tab", BICHANNELTYPE_DIGITAL, 0, 1);

	controller->lshiftChannel = biCreateChannel(controller->inputMap, "lshift", BICHANNELTYPE_DIGITAL, 0, 1);

	sprintf(string, "%ddPadXDB", padID);
	controller->dPadXDebounceChannel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_BIANALOGUE, BICHANNELFLAG_DEBOUNCE | BICHANNELFLAG_RECORD, 0);
	sprintf(string, "%ddPadYDB", padID);
	controller->dPadYDebounceChannel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_BIANALOGUE, BICHANNELFLAG_DEBOUNCE | BICHANNELFLAG_RECORD, 0);

	sprintf(string, "%dcrossDB", padID);
	controller->crossDebounceChannel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_ANALOGUE, BICHANNELFLAG_DEBOUNCE | BICHANNELFLAG_RECORD, 0);
	sprintf(string, "%dtriangleDB", padID);
	controller->triangleDebounceChannel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_ANALOGUE, BICHANNELFLAG_DEBOUNCE | BICHANNELFLAG_RECORD, 0);
	sprintf(string, "%dcircleDB", padID);
	controller->circleDebounceChannel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_ANALOGUE, BICHANNELFLAG_DEBOUNCE | BICHANNELFLAG_RECORD, 0);
	sprintf(string, "%dsquareDB", padID);
	controller->squareDebounceChannel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_ANALOGUE, BICHANNELFLAG_DEBOUNCE | BICHANNELFLAG_RECORD, 0);

	sprintf(string, "%dl1DB", padID);
	controller->l1DebounceChannel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_ANALOGUE, BICHANNELFLAG_DEBOUNCE | BICHANNELFLAG_RECORD, 0);
	sprintf(string, "%dl2DB", padID);
	controller->l2DebounceChannel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_ANALOGUE, BICHANNELFLAG_DEBOUNCE | BICHANNELFLAG_RECORD, 0);
	sprintf(string, "%dl3DB", padID);
	controller->l3DebounceChannel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE | BICHANNELFLAG_RECORD, 0);
	sprintf(string, "%dr1DB", padID);
	controller->r1DebounceChannel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_ANALOGUE, BICHANNELFLAG_DEBOUNCE | BICHANNELFLAG_RECORD, 0);
	sprintf(string, "%dr2DB", padID);
	controller->r2DebounceChannel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_ANALOGUE, BICHANNELFLAG_DEBOUNCE | BICHANNELFLAG_RECORD, 0);
	sprintf(string, "%dr3DB", padID);
	controller->r3DebounceChannel = biCreateChannel(controller->inputMap, string, BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE | BICHANNELFLAG_RECORD, 0);

	controller->startDebounceChannel = biCreateChannel(controller->inputMap, "startDB", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE, 0);
	controller->selectDebounceChannel = biCreateChannel(controller->inputMap, "selectDB", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE, 0);
	controller->tabDebounceChannel = biCreateChannel(controller->inputMap, "tabDB", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE, 0);
	controller->lshiftDebounceChannel = biCreateChannel(controller->inputMap, "lshiftDB", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE, 0);
	controller->equalsChannel = biCreateChannel(controller->inputMap, "equals", BICHANNELTYPE_DIGITAL, 0, 0);
	controller->minusChannel = biCreateChannel(controller->inputMap, "minus", BICHANNELTYPE_DIGITAL, 0, 0);
	controller->commaDebounceChannel = biCreateChannel(controller->inputMap, "comma", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE, 0);
	controller->periodDebounceChannel = biCreateChannel(controller->inputMap, "period", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE, 0);
	controller->lbracketDebounceChannel = biCreateChannel(controller->inputMap, "lbracket", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE, 0);
	controller->rbracketDebounceChannel = biCreateChannel(controller->inputMap, "rbracket", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE, 0);
	controller->f10DebounceChannel = biCreateChannel(controller->inputMap, "f10", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE, 0);
	controller->rDebounceChannel = biCreateChannel(controller->inputMap, "r", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE, 0);
	controller->gDebounceChannel = biCreateChannel(controller->inputMap, "g", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE, 0);
	controller->sDebounceChannel = biCreateChannel(controller->inputMap, "s", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE, 0);
	controller->aDebounceChannel = biCreateChannel(controller->inputMap, "a", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE, 0);
	controller->mDebounceChannel = biCreateChannel(controller->inputMap, "mDB", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE, 0);
	controller->iDebounceChannel = biCreateChannel(controller->inputMap, "iDB", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE, 0);
	controller->hDebounceChannel = biCreateChannel(controller->inputMap, "hDB", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE, 0);
	controller->f3DebounceChannel = biCreateChannel(controller->inputMap, "f3", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE, 0);
	controller->f5DebounceChannel = biCreateChannel(controller->inputMap, "f5", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE, 0);
	controller->f6DebounceChannel = biCreateChannel(controller->inputMap, "f6", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE, 0);
	controller->guiDebounceChannel = biCreateChannel(controller->inputMap, "guiDB", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE, 0);
	
	// NH: Mouse Control
	controller->mb1DebounceChannel = biCreateChannel(controller->inputMap, "mb1DB", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE, 0);
	controller->mb1Channel = biCreateChannel(controller->inputMap, "mb1", BICHANNELTYPE_DIGITAL, 0, 0);
	controller->mb2DebounceChannel = biCreateChannel(controller->inputMap, "mb2DB", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE, 0);
	controller->mb2Channel = biCreateChannel(controller->inputMap, "mb2", BICHANNELTYPE_DIGITAL, 0, 0);
	controller->mxDebounceChannel = biCreateChannel(controller->inputMap, "mxDB", BICHANNELTYPE_BIANALOGUE, BICHANNELFLAG_DEBOUNCE, 0);
	controller->mxChannel = biCreateChannel(controller->inputMap, "mx", BICHANNELTYPE_BIANALOGUE, 0, 0);
	controller->myDebounceChannel = biCreateChannel(controller->inputMap, "myDB", BICHANNELTYPE_BIANALOGUE, BICHANNELFLAG_DEBOUNCE, 0);
	controller->myChannel = biCreateChannel(controller->inputMap, "my", BICHANNELTYPE_BIANALOGUE, 0, 0);
	
	// NH: Menu Navigation Controls
	controller->menuSelectChannel = biCreateChannel(controller->inputMap, "menuselect", BICHANNELTYPE_BIANALOGUE, 0, 0);
	controller->menuBackChannel = biCreateChannel(controller->inputMap, "menuback", BICHANNELTYPE_BIANALOGUE, 0, 0);
	controller->menuYChannel = biCreateChannel(controller->inputMap, "menuup", BICHANNELTYPE_BIANALOGUE, 0, 0);
	controller->menuXChannel = biCreateChannel(controller->inputMap, "menuleft", BICHANNELTYPE_BIANALOGUE, 0, 0);
	controller->menuSelectDebounceChannel = biCreateChannel(controller->inputMap, "menuselectDB", BICHANNELTYPE_BIANALOGUE, BICHANNELFLAG_DEBOUNCE, 0);
	controller->menuBackDebounceChannel = biCreateChannel(controller->inputMap, "menubackDB", BICHANNELTYPE_BIANALOGUE, BICHANNELFLAG_DEBOUNCE, 0);
	controller->menuUpRepeatChannel = biCreateChannel(controller->inputMap, "menuUpAR", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_AUTOREPEAT, 0);
	controller->menuDownRepeatChannel = biCreateChannel(controller->inputMap, "menuDownAR", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_AUTOREPEAT, 0);
	controller->menuLeftRepeatChannel = biCreateChannel(controller->inputMap, "menuLeftAR", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_AUTOREPEAT, 0);
	controller->menuRightRepeatChannel = biCreateChannel(controller->inputMap, "menuRightAR", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_AUTOREPEAT, 0);
	controller->menuUpDebounceChannel = biCreateChannel(controller->inputMap, "menuUpDB", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE, 0);
	controller->menuDownDebounceChannel = biCreateChannel(controller->inputMap, "menuDownDB", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE, 0);
	controller->menuLeftDebounceChannel = biCreateChannel(controller->inputMap, "menuLeftDB", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE, 0);
	controller->menuRightDebounceChannel = biCreateChannel(controller->inputMap, "menuRightDB", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE, 0);

	controller->escDebounceChannel = biCreateChannel(controller->inputMap, "escapeDB", BICHANNELTYPE_DIGITAL, BICHANNELFLAG_DEBOUNCE, 0);

	//create rumble interface
	controller->rumbleController = CreateRumbleController(5);
	controller->padID = padID;
}

/* --------------------------------------------------------------------------------
   Function : DeleteInputChannels
   Purpose : deletes all control channels along with their bindings.
   Parameters : PADCONTROLSTATUS
   Returns : 
   Info : Also creates channel map
*/
void DeleteInputChannels(PADCONTROLSTATUS *controller)
{
	biDeleteMap(controller->inputMap);
	FreeRumbleController(controller);
}


/* --------------------------------------------------------------------------------
   Function : BindInputChannelsPC
   Purpose : Bind keys and buttons to input channels
   Parameters : PADCONTROLSTATUS
   Returns : TRUE/FALSE
   Info : Uses the BPLATFORM define to decide whether to bind keys or buttons
*/
#if (BPLATFORM==PC)
int BindInputChannelsPC(PADCONTROLSTATUS *controller,int padNumber)
{
	int	padId;
	int DIKCode1, DIKCode2;
	int enumeratedKey1, enumeratedKey2;
	
	padId = controller->port = biFindPad(padNumber);
	
#ifndef _MASTER
	biBindInput(controller->f10DebounceChannel, BIINPUT_KEY_F10, BIINPUT_NULL);
	biBindInput(controller->dPadYDebounceChannel, BIINPUT_KEY_HOME, BIINPUT_KEY_END);
	biBindInput(controller->guiDebounceChannel, BIINPUT_KEY_LCONTROL, BIINPUT_NULL);
#endif

	// NH: Mouse Control
	biBindInput(controller->mb1Channel, BIINPUT_MOUSE_B1, BIINPUT_NULL);		
	biBindInput(controller->mb1DebounceChannel, BIINPUT_MOUSE_B1, BIINPUT_NULL);		
	biBindInput(controller->mb2Channel, BIINPUT_MOUSE_B2, BIINPUT_NULL);		
	biBindInput(controller->mb2DebounceChannel, BIINPUT_MOUSE_B2, BIINPUT_NULL);		
	biBindInput(controller->mxChannel, BIINPUT_MOUSE_X, BIINPUT_NULL);		
	biBindInput(controller->mxDebounceChannel, BIINPUT_MOUSE_X, BIINPUT_NULL);		
	biBindInput(controller->myChannel, BIINPUT_MOUSE_Y, BIINPUT_NULL);		
	biBindInput(controller->myDebounceChannel, BIINPUT_MOUSE_Y, BIINPUT_NULL);		

	// NH: Menu Navigation Controls
	biBindInput(controller->menuSelectChannel, BIINPUT_KEY_SPACE, BIINPUT_NULL);		
	biBindInput(controller->menuSelectChannel, BIINPUT_KEY_RETURN, BIINPUT_NULL);		
	biBindInput(controller->menuSelectChannel, BIINPUT_KEY_NUMPADENTER, BIINPUT_NULL);
	biBindInput(controller->menuBackChannel, BIINPUT_KEY_ESCAPE, BIINPUT_NULL);		
	biBindInput(controller->menuBackChannel, BIINPUT_KEY_BACK, BIINPUT_NULL);	
	biBindInput(controller->menuYChannel, BIINPUT_KEY_DOWN, BIINPUT_KEY_UP);	
	biBindInput(controller->menuXChannel, BIINPUT_KEY_RIGHT, BIINPUT_KEY_LEFT);

	biBindInput(controller->menuSelectDebounceChannel, BIINPUT_KEY_SPACE, BIINPUT_NULL);		
	biBindInput(controller->menuSelectDebounceChannel, BIINPUT_KEY_RETURN, BIINPUT_NULL);		
	biBindInput(controller->menuSelectDebounceChannel, BIINPUT_KEY_NUMPADENTER, BIINPUT_NULL);
	biBindInput(controller->menuBackDebounceChannel, BIINPUT_KEY_ESCAPE, BIINPUT_NULL);		
	biBindInput(controller->menuBackDebounceChannel, BIINPUT_KEY_BACK, BIINPUT_NULL);		

	biBindInput(controller->menuUpRepeatChannel, BIINPUT_KEY_UP, BIINPUT_NULL);
	biBindInput(controller->menuDownRepeatChannel, BIINPUT_KEY_DOWN, BIINPUT_NULL);
	biBindInput(controller->menuLeftRepeatChannel, BIINPUT_KEY_LEFT, BIINPUT_NULL);
	biBindInput(controller->menuRightRepeatChannel, BIINPUT_KEY_RIGHT, BIINPUT_NULL);

	biBindInput(controller->menuUpDebounceChannel, BIINPUT_KEY_UP, BIINPUT_NULL);
	biBindInput(controller->menuDownDebounceChannel, BIINPUT_KEY_DOWN, BIINPUT_NULL);
	biBindInput(controller->menuLeftDebounceChannel, BIINPUT_KEY_LEFT, BIINPUT_NULL);
	biBindInput(controller->menuRightDebounceChannel, BIINPUT_KEY_RIGHT, BIINPUT_NULL);

	biBindInput(controller->escDebounceChannel, BIINPUT_KEY_ESCAPE, BIINPUT_NULL);

#ifdef _TAZDEBUG	
#ifndef NHIGGS
	biBindInput(controller->tabChannel, BIINPUT_KEY_TAB, BIINPUT_NULL);	
	biBindInput(controller->commaDebounceChannel, BIINPUT_KEY_COMMA, BIINPUT_NULL);
	biBindInput(controller->periodDebounceChannel, BIINPUT_KEY_PERIOD, BIINPUT_NULL);
	biBindInput(controller->lbracketDebounceChannel, BIINPUT_KEY_LBRACKET, BIINPUT_NULL);
	biBindInput(controller->rbracketDebounceChannel, BIINPUT_KEY_RBRACKET, BIINPUT_NULL);

	biBindInput(controller->equalsChannel, BIINPUT_KEY_EQUALS, BIINPUT_NULL);
	biBindInput(controller->minusChannel, BIINPUT_KEY_MINUS, BIINPUT_NULL);

	biBindInput(controller->l3Channel, BIINPUT_KEY_S, BIINPUT_NULL);
	
	biBindInput(controller->lshiftChannel, BIINPUT_KEY_LSHIFT , BIINPUT_NULL);

	biBindInput(controller->lshiftDebounceChannel, BIINPUT_KEY_LSHIFT , BIINPUT_NULL);

	biBindInput(controller->tabDebounceChannel, BIINPUT_KEY_TAB, BIINPUT_NULL);

	biBindInput(controller->l3DebounceChannel, BIINPUT_KEY_S, BIINPUT_NULL);
	
	biBindInput(controller->f10DebounceChannel, BIINPUT_KEY_F10, BIINPUT_NULL);
	biBindInput(controller->rDebounceChannel, BIINPUT_KEY_R, BIINPUT_NULL);
	biBindInput(controller->gDebounceChannel, BIINPUT_KEY_G, BIINPUT_NULL);
	biBindInput(controller->sDebounceChannel, BIINPUT_KEY_S, BIINPUT_NULL);
	biBindInput(controller->aDebounceChannel, BIINPUT_KEY_A, BIINPUT_NULL);
	biBindInput(controller->mDebounceChannel, BIINPUT_KEY_M, BIINPUT_NULL);
	biBindInput(controller->hDebounceChannel, BIINPUT_KEY_H, BIINPUT_NULL);
	biBindInput(controller->iDebounceChannel, BIINPUT_KEY_I, BIINPUT_NULL);
	biBindInput(controller->f3DebounceChannel, BIINPUT_KEY_F3, BIINPUT_NULL);
	biBindInput(controller->f5DebounceChannel, BIINPUT_KEY_F5, BIINPUT_NULL);
	biBindInput(controller->f6DebounceChannel, BIINPUT_KEY_F6, BIINPUT_NULL);

	biBindInput(controller->y2Channel, BIINPUT_KEY_NUMPAD2, BIINPUT_KEY_NUMPAD8);
	biBindInput(controller->y2DebounceChannel, BIINPUT_KEY_NUMPAD2|BIINPUT_INVERT, BIINPUT_KEY_NUMPAD8|BIINPUT_INVERT);		

	biBindInput(controller->tabChannel, BIINPUT_KEY_TAB, BIINPUT_NULL);
	biBindInput(controller->commaDebounceChannel, BIINPUT_KEY_COMMA, BIINPUT_NULL);
	biBindInput(controller->periodDebounceChannel, BIINPUT_KEY_PERIOD, BIINPUT_NULL);

	biBindInput(controller->lbracketDebounceChannel, BIINPUT_KEY_LBRACKET, BIINPUT_NULL);
	biBindInput(controller->rbracketDebounceChannel, BIINPUT_KEY_RBRACKET, BIINPUT_NULL);

	biBindInput(controller->equalsChannel, BIINPUT_KEY_EQUALS, BIINPUT_NULL);
	biBindInput(controller->minusChannel, BIINPUT_KEY_MINUS, BIINPUT_NULL);

	biBindInput(controller->dPadXChannel, BIINPUT_KEY_NEXT, BIINPUT_KEY_DELETE);
	biBindInput(controller->dPadYChannel, BIINPUT_KEY_HOME, BIINPUT_KEY_END);

	biBindInput(controller->l3Channel, BIINPUT_KEY_S, BIINPUT_NULL);

	biBindInput(controller->lshiftChannel, BIINPUT_KEY_LSHIFT , BIINPUT_NULL);

	biBindInput(controller->lshiftDebounceChannel, BIINPUT_KEY_LSHIFT , BIINPUT_NULL);

	biBindInput(controller->tabDebounceChannel, BIINPUT_KEY_TAB, BIINPUT_NULL);
	biBindInput(controller->dPadXDebounceChannel, BIINPUT_KEY_NEXT, BIINPUT_KEY_DELETE);
	biBindInput(controller->dPadYDebounceChannel, BIINPUT_KEY_HOME, BIINPUT_KEY_END);

	biBindInput(controller->l3DebounceChannel, BIINPUT_KEY_S, BIINPUT_NULL);

	biBindInput(controller->f10DebounceChannel, BIINPUT_KEY_F10, BIINPUT_NULL);
	biBindInput(controller->rDebounceChannel, BIINPUT_KEY_R, BIINPUT_NULL);
	biBindInput(controller->gDebounceChannel, BIINPUT_KEY_G, BIINPUT_NULL);
	biBindInput(controller->sDebounceChannel, BIINPUT_KEY_S, BIINPUT_NULL);
	biBindInput(controller->aDebounceChannel, BIINPUT_KEY_A, BIINPUT_NULL);
	biBindInput(controller->mDebounceChannel, BIINPUT_KEY_M, BIINPUT_NULL);
	biBindInput(controller->hDebounceChannel, BIINPUT_KEY_H, BIINPUT_NULL);

	biBindInput(controller->iDebounceChannel, BIINPUT_KEY_I, BIINPUT_NULL);
	biBindInput(controller->f3DebounceChannel, BIINPUT_KEY_F3, BIINPUT_NULL);
	biBindInput(controller->f5DebounceChannel, BIINPUT_KEY_F5, BIINPUT_NULL);
	biBindInput(controller->f6DebounceChannel, BIINPUT_KEY_F6, BIINPUT_NULL);

#endif // NH: ndef NHIGGS
#endif // NH: _TAZDEBUG

	// NH: If TazData not loaded then use default controls
	if (!tazDataLoaded)
	{
		// NH: Oh No! No setup file found!  Set up the keyboard, as default controller
		defaultKeyboardControls(controller, padNumber);		
	}
	else	
	// NH: Else (Taz Data Loaded) use Defined Controls
	{
		if (padNumber == 0)
		{
			// NH: Setup the control as a joystick control or keyboard if needed.  May modify later
			if (memcmp(&tazData.player1.controllerGUID, &voidGuid, sizeof(GUID)) == 0)
			//if (tazData.player1.controllerGUID.Data1 == -1 && tazData.player1.controllerGUID.Data2 == -1 && tazData.player1.controllerGUID.Data3 == -1)
			{
				defaultKeyboardControls(controller, padNumber);		
			}
			else if (memcmp(&tazData.player1.controllerGUID, &nullGuid, sizeof(GUID)) == 0)
			//if (tazData.player1.controllerGUID.Data1 == 0 && tazData.player1.controllerGUID.Data2 == 0 && tazData.player1.controllerGUID.Data3 == 0)
			{			
#ifdef _TAZDEBUG
#ifndef NHIGGS
				biBindInput(controller->y2Channel, BIINPUT_KEY_NUMPAD2, BIINPUT_KEY_NUMPAD8);
				biBindInput(controller->y2DebounceChannel, BIINPUT_KEY_NUMPAD2|BIINPUT_INVERT, BIINPUT_KEY_NUMPAD8|BIINPUT_INVERT);		

				biBindInput(controller->tabChannel, BIINPUT_KEY_TAB, BIINPUT_NULL);
				biBindInput(controller->commaDebounceChannel, BIINPUT_KEY_COMMA, BIINPUT_NULL);
				biBindInput(controller->periodDebounceChannel, BIINPUT_KEY_PERIOD, BIINPUT_NULL);
		
				biBindInput(controller->lbracketDebounceChannel, BIINPUT_KEY_LBRACKET, BIINPUT_NULL);
				biBindInput(controller->rbracketDebounceChannel, BIINPUT_KEY_RBRACKET, BIINPUT_NULL);

				biBindInput(controller->equalsChannel, BIINPUT_KEY_EQUALS, BIINPUT_NULL);
				biBindInput(controller->minusChannel, BIINPUT_KEY_MINUS, BIINPUT_NULL);

				biBindInput(controller->dPadXChannel, BIINPUT_KEY_NEXT, BIINPUT_KEY_DELETE);
				biBindInput(controller->dPadYChannel, BIINPUT_KEY_HOME, BIINPUT_KEY_END);
				
				biBindInput(controller->l3Channel, BIINPUT_KEY_S, BIINPUT_NULL);

				biBindInput(controller->lshiftChannel, BIINPUT_KEY_LSHIFT , BIINPUT_NULL);

				biBindInput(controller->lshiftDebounceChannel, BIINPUT_KEY_LSHIFT , BIINPUT_NULL);

				biBindInput(controller->tabDebounceChannel, BIINPUT_KEY_TAB, BIINPUT_NULL);
				biBindInput(controller->dPadXDebounceChannel, BIINPUT_KEY_NEXT, BIINPUT_KEY_DELETE);
				biBindInput(controller->dPadYDebounceChannel, BIINPUT_KEY_HOME, BIINPUT_KEY_END);

				biBindInput(controller->l3DebounceChannel, BIINPUT_KEY_S, BIINPUT_NULL);

				biBindInput(controller->f10DebounceChannel, BIINPUT_KEY_F10, BIINPUT_NULL);
				biBindInput(controller->rDebounceChannel, BIINPUT_KEY_R, BIINPUT_NULL);
				biBindInput(controller->gDebounceChannel, BIINPUT_KEY_G, BIINPUT_NULL);
				biBindInput(controller->sDebounceChannel, BIINPUT_KEY_S, BIINPUT_NULL);
				biBindInput(controller->aDebounceChannel, BIINPUT_KEY_A, BIINPUT_NULL);
				biBindInput(controller->mDebounceChannel, BIINPUT_KEY_M, BIINPUT_NULL);
				biBindInput(controller->hDebounceChannel, BIINPUT_KEY_H, BIINPUT_NULL);

				biBindInput(controller->iDebounceChannel, BIINPUT_KEY_I, BIINPUT_NULL);
				biBindInput(controller->f3DebounceChannel, BIINPUT_KEY_F3, BIINPUT_NULL);
				biBindInput(controller->f5DebounceChannel, BIINPUT_KEY_F5, BIINPUT_NULL);
				biBindInput(controller->f6DebounceChannel, BIINPUT_KEY_F6, BIINPUT_NULL);
#endif // NH: ndef NHIGGS
#endif// PP: def _TAZDEBUG

				biBindInput(controller->x1Channel, DIKToBabel(tazData.player1.keyboardData.right - tazData.player1.keyboardData.keyboardOffset), DIKToBabel(tazData.player1.keyboardData.left - tazData.player1.keyboardData.keyboardOffset));	
				
				biBindInput(controller->x2Channel, DIKToBabel(tazData.player1.keyboardData.camRight - tazData.player1.keyboardData.keyboardOffset), DIKToBabel(tazData.player1.keyboardData.camLeft - tazData.player1.keyboardData.keyboardOffset));
				biBindInput(controller->y1Channel, DIKToBabel(tazData.player1.keyboardData.backward - tazData.player1.keyboardData.keyboardOffset), DIKToBabel(tazData.player1.keyboardData.forward - tazData.player1.keyboardData.keyboardOffset));

				biBindInput(controller->x1DebounceChannel, DIKToBabel(tazData.player1.keyboardData.right - tazData.player1.keyboardData.keyboardOffset), DIKToBabel(tazData.player1.keyboardData.left - tazData.player1.keyboardData.keyboardOffset));
				biBindInput(controller->x2DebounceChannel, DIKToBabel(tazData.player1.keyboardData.camRight - tazData.player1.keyboardData.keyboardOffset), DIKToBabel(tazData.player1.keyboardData.camLeft - tazData.player1.keyboardData.keyboardOffset));
				biBindInput(controller->y1DebounceChannel, DIKToBabel(tazData.player1.keyboardData.backward - tazData.player1.keyboardData.keyboardOffset)|BIINPUT_INVERT, DIKToBabel(tazData.player1.keyboardData.forward - tazData.player1.keyboardData.keyboardOffset)|BIINPUT_INVERT);

				biBindInput(controller->crossChannel, DIKToBabel(tazData.player1.keyboardData.jump - tazData.player1.keyboardData.keyboardOffset), BIINPUT_NULL);
				biBindInput(controller->triangleChannel, DIKToBabel(tazData.player1.keyboardData.bite - tazData.player1.keyboardData.keyboardOffset), BIINPUT_NULL);
				biBindInput(controller->circleChannel, DIKToBabel(tazData.player1.keyboardData.spin - tazData.player1.keyboardData.keyboardOffset), BIINPUT_NULL);
				biBindInput(controller->squareChannel, DIKToBabel(tazData.player1.keyboardData.rant - tazData.player1.keyboardData.keyboardOffset), BIINPUT_NULL);

				biBindInput(controller->l1Channel, DIKToBabel(tazData.player1.keyboardData.camLeft - tazData.player1.keyboardData.keyboardOffset), BIINPUT_NULL);
				biBindInput(controller->l2Channel, DIKToBabel(tazData.player1.keyboardData.tiptoe - tazData.player1.keyboardData.keyboardOffset), BIINPUT_NULL);
				
				biBindInput(controller->r1Channel, DIKToBabel(tazData.player1.keyboardData.camRight - tazData.player1.keyboardData.keyboardOffset), BIINPUT_NULL);
				biBindInput(controller->r2Channel, DIKToBabel(tazData.player1.keyboardData.look - tazData.player1.keyboardData.keyboardOffset), BIINPUT_NULL);

				biBindInput(controller->startChannel, DIKToBabel(tazData.player1.keyboardData.pause - tazData.player1.keyboardData.keyboardOffset), BIINPUT_NULL);

				biBindInput(controller->selectChannel, DIKToBabel(tazData.player1.keyboardData.select - tazData.player1.keyboardData.keyboardOffset), BIINPUT_NULL);
				biBindInput(controller->selectDebounceChannel, DIKToBabel(tazData.player1.keyboardData.select - tazData.player1.keyboardData.keyboardOffset), BIINPUT_NULL);

				biBindInput(controller->crossDebounceChannel, DIKToBabel(tazData.player1.keyboardData.jump - tazData.player1.keyboardData.keyboardOffset), BIINPUT_NULL);
				biBindInput(controller->triangleDebounceChannel, DIKToBabel(tazData.player1.keyboardData.bite- tazData.player1.keyboardData.keyboardOffset), BIINPUT_NULL);
				biBindInput(controller->circleDebounceChannel, DIKToBabel(tazData.player1.keyboardData.spin - tazData.player1.keyboardData.keyboardOffset), BIINPUT_NULL);
				biBindInput(controller->squareDebounceChannel, DIKToBabel(tazData.player1.keyboardData.rant - tazData.player1.keyboardData.keyboardOffset), BIINPUT_NULL);

				biBindInput(controller->l1DebounceChannel, DIKToBabel(tazData.player1.keyboardData.camLeft - tazData.player1.keyboardData.keyboardOffset), BIINPUT_NULL);
				biBindInput(controller->l2DebounceChannel, DIKToBabel(tazData.player1.keyboardData.tiptoe - tazData.player1.keyboardData.keyboardOffset), BIINPUT_NULL);
				
				biBindInput(controller->r1DebounceChannel, DIKToBabel(tazData.player1.keyboardData.camRight - tazData.player1.keyboardData.keyboardOffset), BIINPUT_NULL);
				biBindInput(controller->r2DebounceChannel, DIKToBabel(tazData.player1.keyboardData.look - tazData.player1.keyboardData.keyboardOffset), BIINPUT_NULL);

				biBindInput(controller->startDebounceChannel, DIKToBabel(tazData.player1.keyboardData.pause - tazData.player1.keyboardData.keyboardOffset), BIINPUT_NULL);
							}
			else
			{
				// NH: Setup the control as a joystick control or keyboard if needed.  May modify later
#ifdef _TAZDEBUG
#ifndef NHIGGS
				biBindInput(controller->lshiftDebounceChannel, BIINPUT_KEY_LSHIFT , BIINPUT_NULL);

				biBindInput(controller->tabDebounceChannel, BIINPUT_KEY_TAB, BIINPUT_NULL);
				biBindInput(controller->dPadXDebounceChannel, BIINPUT_KEY_NEXT, BIINPUT_KEY_DELETE);
				biBindInput(controller->dPadYDebounceChannel, BIINPUT_KEY_HOME, BIINPUT_KEY_END);

				biBindInput(controller->l3DebounceChannel, BIINPUT_KEY_S, BIINPUT_NULL);

				biBindInput(controller->f10DebounceChannel, BIINPUT_KEY_F10, BIINPUT_NULL);
				biBindInput(controller->rDebounceChannel, BIINPUT_KEY_R, BIINPUT_NULL);
				biBindInput(controller->gDebounceChannel, BIINPUT_KEY_G, BIINPUT_NULL);
				biBindInput(controller->sDebounceChannel, BIINPUT_KEY_S, BIINPUT_NULL);
				biBindInput(controller->aDebounceChannel, BIINPUT_KEY_A, BIINPUT_NULL);
				biBindInput(controller->mDebounceChannel, BIINPUT_KEY_M, BIINPUT_NULL);
				biBindInput(controller->hDebounceChannel, BIINPUT_KEY_H, BIINPUT_NULL);

				biBindInput(controller->iDebounceChannel, BIINPUT_KEY_I, BIINPUT_NULL);
				biBindInput(controller->f3DebounceChannel, BIINPUT_KEY_F3, BIINPUT_NULL);
				biBindInput(controller->f5DebounceChannel, BIINPUT_KEY_F5, BIINPUT_NULL);
				biBindInput(controller->f6DebounceChannel, BIINPUT_KEY_F6, BIINPUT_NULL);
#endif // NH: ndef NHIGGS
#endif// PP: def _TAZDEBUG
		
				
				if (tazData.player1.joystickData.left + BIINPUT_PCJOY_FIRSTID >= BIINPUT_PCJOY_X)
				{
					biBindInput(controller->x1Channel, tazData.player1.joystickData.left + BIINPUT_PCJOY_FIRSTID, NULL);
					biBindInput(controller->menuLeftRepeatChannel, tazData.player1.joystickData.left + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT, BIINPUT_NULL);
					biBindInput(controller->menuRightRepeatChannel, tazData.player1.joystickData.right + BIINPUT_PCJOY_FIRSTID, BIINPUT_NULL);									
					biBindInput(controller->menuLeftDebounceChannel, tazData.player1.joystickData.left + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT, BIINPUT_NULL);
					biBindInput(controller->menuRightDebounceChannel, tazData.player1.joystickData.right + BIINPUT_PCJOY_FIRSTID, BIINPUT_NULL);									
				}
				else
					biBindInput(controller->x1Channel, tazData.player1.joystickData.left + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT, tazData.player1.joystickData.right + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT);
				
				if (tazData.player1.joystickData.camLeft + BIINPUT_PCJOY_FIRSTID >= BIINPUT_PCJOY_X)
					biBindInput(controller->x2Channel, tazData.player1.joystickData.camLeft + BIINPUT_PCJOY_FIRSTID, NULL);
				else
					biBindInput(controller->x2Channel, tazData.player1.joystickData.camLeft + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT, tazData.player1.joystickData.camRight + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT);


				if (tazData.player1.joystickData.forward + BIINPUT_PCJOY_FIRSTID >= BIINPUT_PCJOY_X)
				{
					biBindInput(controller->y1Channel, tazData.player1.joystickData.forward + BIINPUT_PCJOY_FIRSTID, NULL);
					biBindInput(controller->menuUpRepeatChannel, tazData.player1.joystickData.forward + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT, BIINPUT_NULL);
					biBindInput(controller->menuDownRepeatChannel, tazData.player1.joystickData.backward + BIINPUT_PCJOY_FIRSTID, BIINPUT_NULL);
					biBindInput(controller->menuUpDebounceChannel, tazData.player1.joystickData.forward + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT, BIINPUT_NULL);
					biBindInput(controller->menuDownDebounceChannel, tazData.player1.joystickData.backward + BIINPUT_PCJOY_FIRSTID, BIINPUT_NULL);
				}
				else
					biBindInput(controller->y1Channel, tazData.player1.joystickData.forward + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT, tazData.player1.joystickData.backward + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT);
				
				if (tazData.player1.joystickData.left + BIINPUT_PCJOY_FIRSTID >= BIINPUT_PCJOY_X)
					biBindInput(controller->x1DebounceChannel, tazData.player1.joystickData.left + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT, NULL);
				else
					biBindInput(controller->x1DebounceChannel, tazData.player1.joystickData.left + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT, tazData.player1.joystickData.right + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT);
				
				if (tazData.player1.joystickData.camLeft + BIINPUT_PCJOY_FIRSTID >= BIINPUT_PCJOY_X)
					biBindInput(controller->x2DebounceChannel, tazData.player1.joystickData.camLeft + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT, NULL);
				else
					biBindInput(controller->x2DebounceChannel, tazData.player1.joystickData.camLeft + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT, tazData.player1.joystickData.camRight + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT);

				if (tazData.player1.joystickData.forward + BIINPUT_PCJOY_FIRSTID >= BIINPUT_PCJOY_X)
					biBindInput(controller->y1DebounceChannel, tazData.player1.joystickData.forward + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT, NULL);
				else
					biBindInput(controller->y1DebounceChannel, tazData.player1.joystickData.forward + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT, tazData.player1.joystickData.backward + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT);
				
				biBindInput(controller->crossChannel, tazData.player1.joystickData.jump + BIINPUT_PCJOY_FIRSTID, NULL);
				biBindInput(controller->triangleChannel, tazData.player1.joystickData.bite + BIINPUT_PCJOY_FIRSTID, NULL);
				biBindInput(controller->circleChannel, tazData.player1.joystickData.spin + BIINPUT_PCJOY_FIRSTID, NULL);
				biBindInput(controller->squareChannel, tazData.player1.joystickData.rant + BIINPUT_PCJOY_FIRSTID, NULL);
				biBindInput(controller->l1Channel, tazData.player1.joystickData.camLeft + BIINPUT_PCJOY_FIRSTID, NULL);
				biBindInput(controller->l2Channel, tazData.player1.joystickData.tiptoe + BIINPUT_PCJOY_FIRSTID, NULL);
				
				biBindInput(controller->r1Channel, tazData.player1.joystickData.camRight + BIINPUT_PCJOY_FIRSTID, NULL);
				biBindInput(controller->r2Channel, tazData.player1.joystickData.look + BIINPUT_PCJOY_FIRSTID, NULL);

				//biBindInput(controller->startChannel, tazData.player1.joystickData.pause + BIINPUT_PCJOY_FIRSTID, NULL);

				//biBindInput(controller->selectChannel, tazData.player1.joystickData.select + BIINPUT_PCJOY_FIRSTID, NULL);
				//biBindInput(controller->selectDebounceChannel, tazData.player1.joystickData.select + BIINPUT_PCJOY_FIRSTID, NULL);

				biBindInput(controller->crossDebounceChannel, tazData.player1.joystickData.jump + BIINPUT_PCJOY_FIRSTID, NULL);
				biBindInput(controller->triangleDebounceChannel, tazData.player1.joystickData.bite + BIINPUT_PCJOY_FIRSTID, NULL);
				biBindInput(controller->circleDebounceChannel, tazData.player1.joystickData.spin + BIINPUT_PCJOY_FIRSTID, NULL);
				biBindInput(controller->squareDebounceChannel, tazData.player1.joystickData.rant + BIINPUT_PCJOY_FIRSTID, NULL);
				biBindInput(controller->l1DebounceChannel, tazData.player1.joystickData.camLeft + BIINPUT_PCJOY_FIRSTID, NULL);
				biBindInput(controller->l2DebounceChannel, tazData.player1.joystickData.tiptoe + BIINPUT_PCJOY_FIRSTID, NULL);
				
				biBindInput(controller->r1DebounceChannel, tazData.player1.joystickData.camRight + BIINPUT_PCJOY_FIRSTID, NULL);
				biBindInput(controller->r2DebounceChannel, tazData.player1.joystickData.look + BIINPUT_PCJOY_FIRSTID, NULL);

				if (tazData.player1.joystickData.pause < tazData.player1.keyboardData.keyboardOffset)
				{
					biBindInput(controller->startDebounceChannel, tazData.player1.joystickData.pause + BIINPUT_PCJOY_FIRSTID, NULL);
					biBindInput(controller->startChannel, tazData.player1.joystickData.pause + BIINPUT_PCJOY_FIRSTID, NULL);
				}
				else
				{
					biBindInput(controller->startDebounceChannel, DIKToBabel(tazData.player1.joystickData.pause - tazData.player1.keyboardData.keyboardOffset), BIINPUT_NULL);
					biBindInput(controller->startChannel, DIKToBabel(tazData.player1.joystickData.pause - tazData.player1.keyboardData.keyboardOffset), BIINPUT_NULL);
				}
			
				if (tazData.player1.joystickData.select < tazData.player1.keyboardData.keyboardOffset)
				{
					biBindInput(controller->selectChannel, tazData.player1.joystickData.select + BIINPUT_PCJOY_FIRSTID, NULL);
					biBindInput(controller->selectDebounceChannel, tazData.player1.joystickData.select + BIINPUT_PCJOY_FIRSTID, NULL);
				}
				else
				{
					biBindInput(controller->selectDebounceChannel, DIKToBabel(tazData.player1.joystickData.select - tazData.player1.keyboardData.keyboardOffset), BIINPUT_NULL);
					biBindInput(controller->selectChannel, DIKToBabel(tazData.player1.joystickData.select - tazData.player1.keyboardData.keyboardOffset), BIINPUT_NULL);
				}
			}		
		}
		else if (padNumber == 1)
		{
			// NH: Player 2
			//if (tazData.player2.controllerGUID.Data1 == -1 && tazData.player2.controllerGUID.Data2 == -1 && tazData.player2.controllerGUID.Data3 == -1)
			if (memcmp(&tazData.player2.controllerGUID, &voidGuid, sizeof(GUID)) == 0)
			{
				defaultKeyboardControls(controller, padNumber);		
			}
			else if (memcmp(&tazData.player2.controllerGUID, &nullGuid, sizeof(GUID)) == 0)
			//if (tazData.player2.controllerGUID.Data1 == 0 && tazData.player2.controllerGUID.Data2 == 0 && tazData.player2.controllerGUID.Data3 == 0)
			{
#ifdef _TAZDEBUG
#ifndef NHIGGS
				biBindInput(controller->lshiftDebounceChannel, BIINPUT_KEY_LSHIFT , BIINPUT_NULL);

				biBindInput(controller->tabDebounceChannel, BIINPUT_KEY_TAB, BIINPUT_NULL);
				biBindInput(controller->dPadXDebounceChannel, BIINPUT_KEY_NEXT, BIINPUT_KEY_DELETE);
				biBindInput(controller->dPadYDebounceChannel, BIINPUT_KEY_HOME, BIINPUT_KEY_END);

				biBindInput(controller->l3DebounceChannel, BIINPUT_KEY_S, BIINPUT_NULL);

				biBindInput(controller->f10DebounceChannel, BIINPUT_KEY_F10, BIINPUT_NULL);
				biBindInput(controller->rDebounceChannel, BIINPUT_KEY_R, BIINPUT_NULL);
				biBindInput(controller->gDebounceChannel, BIINPUT_KEY_G, BIINPUT_NULL);
				biBindInput(controller->sDebounceChannel, BIINPUT_KEY_S, BIINPUT_NULL);
				biBindInput(controller->aDebounceChannel, BIINPUT_KEY_A, BIINPUT_NULL);
				biBindInput(controller->mDebounceChannel, BIINPUT_KEY_M, BIINPUT_NULL);
				biBindInput(controller->hDebounceChannel, BIINPUT_KEY_H, BIINPUT_NULL);

				biBindInput(controller->iDebounceChannel, BIINPUT_KEY_I, BIINPUT_NULL);
				biBindInput(controller->f3DebounceChannel, BIINPUT_KEY_F3, BIINPUT_NULL);
				biBindInput(controller->f5DebounceChannel, BIINPUT_KEY_F5, BIINPUT_NULL);
				biBindInput(controller->f6DebounceChannel, BIINPUT_KEY_F6, BIINPUT_NULL);
#endif // NH: ndef NHIGGS
#endif// PP: def _TAZDEBUG

				biBindInput(controller->x1Channel, DIKToBabel(tazData.player2.keyboardData.right - tazData.player2.keyboardData.keyboardOffset), DIKToBabel(tazData.player2.keyboardData.left - tazData.player2.keyboardData.keyboardOffset));
				biBindInput(controller->x2Channel, DIKToBabel(tazData.player2.keyboardData.camRight - tazData.player2.keyboardData.keyboardOffset), DIKToBabel(tazData.player2.keyboardData.camLeft - tazData.player2.keyboardData.keyboardOffset));
				biBindInput(controller->y1Channel, DIKToBabel(tazData.player2.keyboardData.backward - tazData.player2.keyboardData.keyboardOffset), DIKToBabel(tazData.player2.keyboardData.forward - tazData.player2.keyboardData.keyboardOffset));

				biBindInput(controller->x1DebounceChannel, DIKToBabel(tazData.player2.keyboardData.right - tazData.player2.keyboardData.keyboardOffset), DIKToBabel(tazData.player2.keyboardData.left - tazData.player2.keyboardData.keyboardOffset));
				biBindInput(controller->x2DebounceChannel, DIKToBabel(tazData.player2.keyboardData.camRight - tazData.player2.keyboardData.keyboardOffset), DIKToBabel(tazData.player2.keyboardData.camLeft - tazData.player2.keyboardData.keyboardOffset));
				biBindInput(controller->y1DebounceChannel, DIKToBabel(tazData.player2.keyboardData.backward - tazData.player2.keyboardData.keyboardOffset)|BIINPUT_INVERT, DIKToBabel(tazData.player2.keyboardData.forward - tazData.player2.keyboardData.keyboardOffset)|BIINPUT_INVERT);

				biBindInput(controller->crossChannel, DIKToBabel(tazData.player2.keyboardData.jump - tazData.player2.keyboardData.keyboardOffset), BIINPUT_NULL);
				biBindInput(controller->triangleChannel, DIKToBabel(tazData.player2.keyboardData.bite - tazData.player2.keyboardData.keyboardOffset), BIINPUT_NULL);
				biBindInput(controller->circleChannel, DIKToBabel(tazData.player2.keyboardData.spin - tazData.player2.keyboardData.keyboardOffset), BIINPUT_NULL);
				biBindInput(controller->squareChannel, DIKToBabel(tazData.player2.keyboardData.rant - tazData.player2.keyboardData.keyboardOffset), BIINPUT_NULL);

				biBindInput(controller->l1Channel, DIKToBabel(tazData.player2.keyboardData.camLeft - tazData.player2.keyboardData.keyboardOffset), BIINPUT_NULL);
				biBindInput(controller->l2Channel, DIKToBabel(tazData.player2.keyboardData.tiptoe - tazData.player2.keyboardData.keyboardOffset), BIINPUT_NULL);

				biBindInput(controller->r1Channel, DIKToBabel(tazData.player2.keyboardData.camRight - tazData.player2.keyboardData.keyboardOffset), BIINPUT_NULL);
				biBindInput(controller->r2Channel, DIKToBabel(tazData.player2.keyboardData.look - tazData.player2.keyboardData.keyboardOffset), BIINPUT_NULL);
			
				biBindInput(controller->startChannel, DIKToBabel(tazData.player2.keyboardData.pause - tazData.player2.keyboardData.keyboardOffset), BIINPUT_NULL);

				biBindInput(controller->selectChannel, DIKToBabel(tazData.player2.keyboardData.select - tazData.player2.keyboardData.keyboardOffset), BIINPUT_NULL);
				biBindInput(controller->selectDebounceChannel, DIKToBabel(tazData.player2.keyboardData.select - tazData.player2.keyboardData.keyboardOffset), BIINPUT_NULL);

				biBindInput(controller->crossDebounceChannel, DIKToBabel(tazData.player2.keyboardData.jump - tazData.player2.keyboardData.keyboardOffset), BIINPUT_NULL);
				biBindInput(controller->triangleDebounceChannel, DIKToBabel(tazData.player2.keyboardData.bite- tazData.player2.keyboardData.keyboardOffset), BIINPUT_NULL);
				biBindInput(controller->circleDebounceChannel, DIKToBabel(tazData.player2.keyboardData.spin - tazData.player2.keyboardData.keyboardOffset), BIINPUT_NULL);
				biBindInput(controller->squareDebounceChannel, DIKToBabel(tazData.player2.keyboardData.rant - tazData.player2.keyboardData.keyboardOffset), BIINPUT_NULL);

				biBindInput(controller->l1DebounceChannel, DIKToBabel(tazData.player2.keyboardData.camLeft - tazData.player2.keyboardData.keyboardOffset), BIINPUT_NULL);
				biBindInput(controller->l2DebounceChannel, DIKToBabel(tazData.player2.keyboardData.tiptoe - tazData.player2.keyboardData.keyboardOffset), BIINPUT_NULL);

				biBindInput(controller->r1DebounceChannel, DIKToBabel(tazData.player2.keyboardData.camRight - tazData.player2.keyboardData.keyboardOffset), BIINPUT_NULL);
				biBindInput(controller->r2DebounceChannel, DIKToBabel(tazData.player2.keyboardData.look - tazData.player2.keyboardData.keyboardOffset), BIINPUT_NULL);
			
				biBindInput(controller->startDebounceChannel, DIKToBabel(tazData.player2.keyboardData.pause - tazData.player2.keyboardData.keyboardOffset), BIINPUT_NULL);
			}
			else
			{
				// NH: Setup the control as a joystick control or keyboard if needed.  May modify later
#ifdef _TAZDEBUG
#ifndef NHIGGS
				biBindInput(controller->equalsChannel, BIINPUT_KEY_EQUALS, BIINPUT_NULL);
				biBindInput(controller->minusChannel, BIINPUT_KEY_MINUS, BIINPUT_NULL);

				biBindInput(controller->dPadXChannel, BIINPUT_KEY_NEXT, BIINPUT_KEY_DELETE);
				biBindInput(controller->dPadYChannel, BIINPUT_KEY_HOME, BIINPUT_KEY_END);
				biBindInput(controller->l3Channel, BIINPUT_KEY_S, BIINPUT_NULL);

				biBindInput(controller->lshiftChannel, BIINPUT_KEY_LSHIFT , BIINPUT_NULL);

				biBindInput(controller->lshiftDebounceChannel, BIINPUT_KEY_LSHIFT , BIINPUT_NULL);

				biBindInput(controller->tabDebounceChannel, BIINPUT_KEY_TAB, BIINPUT_NULL);
				biBindInput(controller->dPadXDebounceChannel, BIINPUT_KEY_NEXT, BIINPUT_KEY_DELETE);
				biBindInput(controller->dPadYDebounceChannel, BIINPUT_KEY_HOME, BIINPUT_KEY_END);
				biBindInput(controller->l3DebounceChannel, BIINPUT_KEY_S, BIINPUT_NULL);

				biBindInput(controller->f10DebounceChannel, BIINPUT_KEY_F10, BIINPUT_NULL);
				biBindInput(controller->rDebounceChannel, BIINPUT_KEY_R, BIINPUT_NULL);
				biBindInput(controller->gDebounceChannel, BIINPUT_KEY_G, BIINPUT_NULL);
				biBindInput(controller->sDebounceChannel, BIINPUT_KEY_S, BIINPUT_NULL);
				biBindInput(controller->aDebounceChannel, BIINPUT_KEY_A, BIINPUT_NULL);
				biBindInput(controller->mDebounceChannel, BIINPUT_KEY_M, BIINPUT_NULL);
				biBindInput(controller->hDebounceChannel, BIINPUT_KEY_H, BIINPUT_NULL);

				biBindInput(controller->iDebounceChannel, BIINPUT_KEY_I, BIINPUT_NULL);
				biBindInput(controller->f3DebounceChannel, BIINPUT_KEY_F3, BIINPUT_NULL);
				biBindInput(controller->f5DebounceChannel, BIINPUT_KEY_F5, BIINPUT_NULL);
				biBindInput(controller->f6DebounceChannel, BIINPUT_KEY_F6, BIINPUT_NULL);
#endif // NH: ndef NHIGGS
#endif// PP: def _TAZDEBUG
				if (tazData.player2.joystickData.left + BIINPUT_PCJOY_FIRSTID >= BIINPUT_PCJOY_X)
				{
					biBindInput(controller->x1Channel, tazData.player2.joystickData.left + BIINPUT_PCJOY_FIRSTID, NULL);
					biBindInput(controller->x1DebounceChannel, tazData.player2.joystickData.left + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT, NULL);
					biBindInput(controller->menuLeftRepeatChannel, tazData.player2.joystickData.left + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT, BIINPUT_NULL);
					biBindInput(controller->menuRightRepeatChannel, tazData.player2.joystickData.right + BIINPUT_PCJOY_FIRSTID, BIINPUT_NULL);
				}
				else
				{
					biBindInput(controller->x1Channel, tazData.player2.joystickData.left + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT, tazData.player2.joystickData.right + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT);
					biBindInput(controller->x1DebounceChannel, tazData.player2.joystickData.left + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT, tazData.player2.joystickData.right + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT);
				}
				
				if (tazData.player2.joystickData.camLeft + BIINPUT_PCJOY_FIRSTID >= BIINPUT_PCJOY_X)
					biBindInput(controller->x2Channel, tazData.player2.joystickData.camLeft + BIINPUT_PCJOY_FIRSTID, NULL);
				else
					biBindInput(controller->x2Channel, tazData.player2.joystickData.camLeft + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT, tazData.player2.joystickData.camRight + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT);

				if (tazData.player2.joystickData.forward + BIINPUT_PCJOY_FIRSTID >= BIINPUT_PCJOY_X)
				{
					biBindInput(controller->y1Channel, tazData.player2.joystickData.forward + BIINPUT_PCJOY_FIRSTID, NULL);
					biBindInput(controller->y1DebounceChannel, tazData.player2.joystickData.forward + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT, NULL);
					biBindInput(controller->menuUpRepeatChannel, tazData.player2.joystickData.forward + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT, BIINPUT_NULL);
					biBindInput(controller->menuDownRepeatChannel, tazData.player2.joystickData.backward + BIINPUT_PCJOY_FIRSTID, BIINPUT_NULL);
				}
				else
				{
					biBindInput(controller->y1Channel, tazData.player2.joystickData.forward + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT, tazData.player2.joystickData.backward + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT);
					biBindInput(controller->y1DebounceChannel, tazData.player2.joystickData.forward + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT, tazData.player2.joystickData.backward + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT);
				}
				
				if (tazData.player2.joystickData.camLeft + BIINPUT_PCJOY_FIRSTID >= BIINPUT_PCJOY_X)
					biBindInput(controller->x2DebounceChannel, tazData.player2.joystickData.camLeft + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT, NULL);
				else
					biBindInput(controller->x2DebounceChannel, tazData.player2.joystickData.camLeft + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT, tazData.player2.joystickData.camRight + BIINPUT_PCJOY_FIRSTID|BIINPUT_INVERT);

				biBindInput(controller->crossChannel, tazData.player2.joystickData.jump + BIINPUT_PCJOY_FIRSTID, NULL);
				biBindInput(controller->triangleChannel, tazData.player2.joystickData.bite + BIINPUT_PCJOY_FIRSTID, NULL);
				biBindInput(controller->circleChannel, tazData.player2.joystickData.spin + BIINPUT_PCJOY_FIRSTID, NULL);
				biBindInput(controller->squareChannel, tazData.player2.joystickData.rant + BIINPUT_PCJOY_FIRSTID, NULL);
				biBindInput(controller->l1Channel, tazData.player2.joystickData.camLeft + BIINPUT_PCJOY_FIRSTID, NULL);
				biBindInput(controller->l2Channel, tazData.player2.joystickData.tiptoe + BIINPUT_PCJOY_FIRSTID, NULL);
				
				biBindInput(controller->r1Channel, tazData.player2.joystickData.camRight + BIINPUT_PCJOY_FIRSTID, NULL);
				biBindInput(controller->r2Channel, tazData.player2.joystickData.look + BIINPUT_PCJOY_FIRSTID, NULL);
//				biBindInput(controller->startChannel, tazData.player2.joystickData.pause + BIINPUT_PCJOY_FIRSTID, NULL);
				
				biBindInput(controller->crossDebounceChannel, tazData.player2.joystickData.jump + BIINPUT_PCJOY_FIRSTID, NULL);
				biBindInput(controller->triangleDebounceChannel, tazData.player2.joystickData.bite + BIINPUT_PCJOY_FIRSTID, NULL);
				biBindInput(controller->circleDebounceChannel, tazData.player2.joystickData.spin + BIINPUT_PCJOY_FIRSTID, NULL);
				biBindInput(controller->squareDebounceChannel, tazData.player2.joystickData.rant + BIINPUT_PCJOY_FIRSTID, NULL);
				biBindInput(controller->l1DebounceChannel, tazData.player2.joystickData.camLeft + BIINPUT_PCJOY_FIRSTID, NULL);
				biBindInput(controller->l2DebounceChannel, tazData.player2.joystickData.tiptoe + BIINPUT_PCJOY_FIRSTID, NULL);

				biBindInput(controller->r1DebounceChannel, tazData.player2.joystickData.camRight + BIINPUT_PCJOY_FIRSTID, NULL);
				biBindInput(controller->r2DebounceChannel, tazData.player2.joystickData.look + BIINPUT_PCJOY_FIRSTID, NULL);

				if (tazData.player2.joystickData.pause < tazData.player2.keyboardData.keyboardOffset)
					biBindInput(controller->startDebounceChannel, tazData.player2.joystickData.pause + BIINPUT_PCJOY_FIRSTID, NULL);
				else
					biBindInput(controller->startDebounceChannel, DIKToBabel(tazData.player2.joystickData.pause - tazData.player2.keyboardData.keyboardOffset), BIINPUT_NULL);
			
				if (tazData.player2.joystickData.select < tazData.player2.keyboardData.keyboardOffset)
				{
					biBindInput(controller->selectChannel, tazData.player2.joystickData.select + BIINPUT_PCJOY_FIRSTID, NULL);
					biBindInput(controller->selectDebounceChannel, tazData.player2.joystickData.select + BIINPUT_PCJOY_FIRSTID, NULL);
				}
				else
				{
					biBindInput(controller->selectChannel, DIKToBabel(tazData.player2.keyboardData.select - tazData.player2.keyboardData.keyboardOffset), BIINPUT_NULL);
					biBindInput(controller->selectDebounceChannel, DIKToBabel(tazData.player2.keyboardData.select - tazData.player2.keyboardData.keyboardOffset), BIINPUT_NULL);
				}
			}		
		}
	}
	return TRUE;
}
#endif


/*  --------------------------------------------------------------------------------
	Function	: ClearInputBindings
	Purpose		: Removes all existing bindings on the input channels
	Parameters	: controller
	Returns		: 
	Info		: this allows the BindInputChannels function to be called again
*/

void ClearInputBindings(PADCONTROLSTATUS *controller)
{
	biResetChannel(controller->x1Channel);
	biResetChannel(controller->y1Channel);
	biResetChannel(controller->x2Channel);
	biResetChannel(controller->y2Channel);

	biResetChannel(controller->x1DebounceChannel);
	biResetChannel(controller->y1DebounceChannel);
	biResetChannel(controller->x2DebounceChannel);
	biResetChannel(controller->y2DebounceChannel);
	
	biResetChannel(controller->dPadXChannel);
	biResetChannel(controller->dPadYChannel);
	biResetChannel(controller->crossChannel);
	biResetChannel(controller->triangleChannel);
	biResetChannel(controller->circleChannel);
	biResetChannel(controller->squareChannel);
	biResetChannel(controller->l1Channel);
	biResetChannel(controller->l2Channel);
	biResetChannel(controller->l3Channel);
	biResetChannel(controller->r1Channel);
	biResetChannel(controller->r2Channel);
	biResetChannel(controller->r3Channel);
	biResetChannel(controller->startChannel);
	biResetChannel(controller->selectChannel);
	biResetChannel(controller->tabChannel);
	biResetChannel(controller->lshiftChannel);

	biResetChannel(controller->dPadXDebounceChannel);
	biResetChannel(controller->dPadYDebounceChannel);
	biResetChannel(controller->crossDebounceChannel);
	biResetChannel(controller->triangleDebounceChannel);
	biResetChannel(controller->circleDebounceChannel);
	biResetChannel(controller->squareDebounceChannel);
	biResetChannel(controller->l1DebounceChannel);
	biResetChannel(controller->l2DebounceChannel);
	biResetChannel(controller->l3DebounceChannel);
	biResetChannel(controller->r1DebounceChannel);
	biResetChannel(controller->r2DebounceChannel);
	biResetChannel(controller->r3DebounceChannel);
	biResetChannel(controller->startDebounceChannel);
	biResetChannel(controller->selectDebounceChannel);
	biResetChannel(controller->f10DebounceChannel);
	biResetChannel(controller->rDebounceChannel);
	biResetChannel(controller->gDebounceChannel);
	biResetChannel(controller->sDebounceChannel);
	biResetChannel(controller->aDebounceChannel);
	biResetChannel(controller->mDebounceChannel);
	biResetChannel(controller->hDebounceChannel);
	biResetChannel(controller->iDebounceChannel);
	biResetChannel(controller->lshiftDebounceChannel);
	biResetChannel(controller->commaDebounceChannel);
	biResetChannel(controller->equalsChannel);
	biResetChannel(controller->minusChannel);
	biResetChannel(controller->periodDebounceChannel);
	biResetChannel(controller->lbracketDebounceChannel);
	biResetChannel(controller->rbracketDebounceChannel);
	biResetChannel(controller->f5DebounceChannel);
	biResetChannel(controller->f6DebounceChannel);
	biResetChannel(controller->f3DebounceChannel);
	biResetChannel(controller->tabDebounceChannel);
	biResetChannel(controller->guiDebounceChannel);

	// NH: Mouse Control
	biResetChannel(controller->mb1Channel);
	biResetChannel(controller->mb1DebounceChannel);
	biResetChannel(controller->mb2Channel);
	biResetChannel(controller->mb2DebounceChannel);
	biResetChannel(controller->mxChannel);
	biResetChannel(controller->mxDebounceChannel);
	biResetChannel(controller->myChannel);
	biResetChannel(controller->myDebounceChannel);

	// NH: Menu Navigation Controls
	biResetChannel(controller->menuUpRepeatChannel);
	biResetChannel(controller->menuDownRepeatChannel);
	biResetChannel(controller->menuLeftRepeatChannel);
	biResetChannel(controller->menuRightRepeatChannel);
	biResetChannel(controller->menuUpDebounceChannel);
	biResetChannel(controller->menuDownDebounceChannel);
	biResetChannel(controller->menuLeftDebounceChannel);
	biResetChannel(controller->menuRightDebounceChannel);
	biResetChannel(controller->menuSelectDebounceChannel);
	biResetChannel(controller->menuBackDebounceChannel);
	biResetChannel(controller->menuYChannel);
	biResetChannel(controller->menuXChannel);
	biResetChannel(controller->menuSelectChannel);
	biResetChannel(controller->menuBackChannel);

	biResetChannel(controller->escDebounceChannel);
}


/* --------------------------------------------------------------------------------
   Function		: BindInputChannelsToPort
   Purpose		: Bind a pad structure's input channels to a specific port 
   Parameters	: PADCONTROLSTATUS*, physical 0-based port index
   Returns		: TRUE=success/FALSE=failure
   Shoe-horn	: // PP: complement of BindInputChannelsToStarboard
*/
BOOL BindInputChannelsToPort(PADCONTROLSTATUS* const controller, const uchar port)
{
	if(controller == NULL)
	{
		bkPrintf("BindInputChannelsToPort: controller is NULL!\n");
		return FALSE;
	}

	// PP: set the controller's port index
	controller->port = port;

#if BPLATFORM == PS2

	biBindInput(controller->x1Channel, BIINPUT_PS_X1|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->x2Channel, BIINPUT_PS_X2|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->y1Channel, BIINPUT_PS_Y1|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->y2Channel, BIINPUT_PS_Y2|BIINPUT_DEV(port), BIINPUT_NULL);
	
	biBindInput(controller->x1DebounceChannel, BIINPUT_PS_X1|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->x2DebounceChannel, BIINPUT_PS_X2|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->y1DebounceChannel, BIINPUT_PS_Y1|BIINPUT_DEV(port)|BIINPUT_INVERT, BIINPUT_NULL);
	biBindInput(controller->y2DebounceChannel, BIINPUT_PS_Y2|BIINPUT_DEV(port)|BIINPUT_INVERT, BIINPUT_NULL);

	biBindInput(controller->dPadXChannel, BIINPUT_PS_RIGHT|BIINPUT_DEV(port), BIINPUT_PS_LEFT|BIINPUT_DEV(port));
	biBindInput(controller->dPadYChannel, BIINPUT_PS_UP|BIINPUT_DEV(port), BIINPUT_PS_DOWN|BIINPUT_DEV(port));
	
	if (!debugDPad)
	{// JW: This'll never work...
		biBindInput(controller->x1Channel, BIINPUT_PS_RIGHT|BIINPUT_DEV(port), BIINPUT_PS_LEFT|BIINPUT_DEV(port));
		biBindInput(controller->y1Channel, BIINPUT_PS_UP|BIINPUT_DEV(port), BIINPUT_PS_DOWN|BIINPUT_DEV(port));
	}

	biBindInput(controller->crossChannel, BIINPUT_PS_CROSS|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->triangleChannel, BIINPUT_PS_TRIANGLE|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->circleChannel, BIINPUT_PS_CIRCLE|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->squareChannel, BIINPUT_PS_SQUARE|BIINPUT_DEV(port), BIINPUT_NULL);

	biBindInput(controller->l1Channel, BIINPUT_PS_L1|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->l2Channel, BIINPUT_PS_L2|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->l3Channel, BIINPUT_PS_L3|BIINPUT_DEV(port), BIINPUT_NULL);
	
	biBindInput(controller->r1Channel, BIINPUT_PS_R1|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->r2Channel, BIINPUT_PS_R2|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->r3Channel, BIINPUT_PS_R3|BIINPUT_DEV(port), BIINPUT_NULL);

	biBindInput(controller->startChannel, BIINPUT_PS_START|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->selectChannel, BIINPUT_PS_SELECT|BIINPUT_DEV(port), BIINPUT_NULL);

	biBindInput(controller->dPadXDebounceChannel, BIINPUT_PS_RIGHT|BIINPUT_DEV(port), BIINPUT_PS_LEFT|BIINPUT_DEV(port));
	biBindInput(controller->dPadYDebounceChannel, BIINPUT_PS_UP|BIINPUT_DEV(port), BIINPUT_PS_DOWN|BIINPUT_DEV(port));

	biBindInput(controller->crossDebounceChannel, BIINPUT_PS_CROSS|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->triangleDebounceChannel, BIINPUT_PS_TRIANGLE|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->circleDebounceChannel, BIINPUT_PS_CIRCLE|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->squareDebounceChannel, BIINPUT_PS_SQUARE|BIINPUT_DEV(port), BIINPUT_NULL);

	biBindInput(controller->l1DebounceChannel, BIINPUT_PS_L1|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->l2DebounceChannel, BIINPUT_PS_L2|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->l3DebounceChannel, BIINPUT_PS_L3|BIINPUT_DEV(port), BIINPUT_NULL);
	
	biBindInput(controller->r1DebounceChannel, BIINPUT_PS_R1|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->r2DebounceChannel, BIINPUT_PS_R2|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->r3DebounceChannel, BIINPUT_PS_R3|BIINPUT_DEV(port), BIINPUT_NULL);

	biBindInput(controller->startDebounceChannel, BIINPUT_PS_START|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->selectDebounceChannel, BIINPUT_PS_SELECT|BIINPUT_DEV(port), BIINPUT_NULL);
#ifndef _MASTER
	biBindInput(controller->guiDebounceChannel, BIINPUT_PS_SELECT|BIINPUT_DEV(port), BIINPUT_NULL);
#else
	#ifdef FORCEGUION
		biBindInput(controller->guiDebounceChannel, BIINPUT_PS_SELECT|BIINPUT_DEV(port), BIINPUT_NULL);
	#endif
#endif
	
	// PP: AUTO-REPEAT MENU NAVIGATION CHANNELS...

	biBindInput(controller->menuUpRepeatChannel, BIINPUT_PS_UP|BIINPUT_DEV(port), BIINPUT_XBOX_DOWN|BIINPUT_DEV(port));
	biBindInput(controller->menuUpRepeatChannel, BIINPUT_PS_Y1|BIINPUT_DEV(port)|BIINPUT_INVERT, BIINPUT_NULL);

	biBindInput(controller->menuDownRepeatChannel, BIINPUT_PS_DOWN|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->menuDownRepeatChannel, BIINPUT_PS_Y1|BIINPUT_DEV(port), BIINPUT_NULL);

	biBindInput(controller->menuLeftRepeatChannel, BIINPUT_PS_LEFT|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->menuLeftRepeatChannel, BIINPUT_PS_X1|BIINPUT_DEV(port)|BIINPUT_INVERT, BIINPUT_NULL);

	biBindInput(controller->menuRightRepeatChannel, BIINPUT_PS_RIGHT|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->menuRightRepeatChannel, BIINPUT_PS_X1|BIINPUT_DEV(port), BIINPUT_NULL);

	// PP: DEBOUNCED MENU NAVIGATION CHANNELS...

	biBindInput(controller->menuUpDebounceChannel, BIINPUT_PS_UP|BIINPUT_DEV(port), BIINPUT_XBOX_DOWN|BIINPUT_DEV(port));
	biBindInput(controller->menuUpDebounceChannel, BIINPUT_PS_Y1|BIINPUT_DEV(port)|BIINPUT_INVERT, BIINPUT_NULL);

	biBindInput(controller->menuDownDebounceChannel, BIINPUT_PS_DOWN|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->menuDownDebounceChannel, BIINPUT_PS_Y1|BIINPUT_DEV(port), BIINPUT_NULL);

	biBindInput(controller->menuLeftDebounceChannel, BIINPUT_PS_LEFT|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->menuLeftDebounceChannel, BIINPUT_PS_X1|BIINPUT_DEV(port)|BIINPUT_INVERT, BIINPUT_NULL);

	biBindInput(controller->menuRightDebounceChannel, BIINPUT_PS_RIGHT|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->menuRightDebounceChannel, BIINPUT_PS_X1|BIINPUT_DEV(port), BIINPUT_NULL);

#elif BPLATFORM == XBOX

	biBindInput(controller->x1Channel, BIINPUT_XBOX_X1|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->x2Channel, BIINPUT_XBOX_X2|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->y1Channel, BIINPUT_XBOX_Y1|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->y2Channel, BIINPUT_XBOX_Y2|BIINPUT_DEV(port), BIINPUT_NULL);
	
	biBindInput(controller->x1DebounceChannel, BIINPUT_XBOX_X1|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->x2DebounceChannel, BIINPUT_XBOX_X2|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->y1DebounceChannel, BIINPUT_XBOX_Y1|BIINPUT_DEV(port)|BIINPUT_INVERT, BIINPUT_NULL);
	biBindInput(controller->y2DebounceChannel, BIINPUT_XBOX_Y2|BIINPUT_DEV(port)|BIINPUT_INVERT, BIINPUT_NULL);

	biBindInput(controller->dPadXChannel,	BIINPUT_XBOX_RIGHT|BIINPUT_DEV(port),	BIINPUT_XBOX_LEFT|BIINPUT_DEV(port));
	biBindInput(controller->dPadYChannel,	BIINPUT_XBOX_UP|BIINPUT_DEV(port),		BIINPUT_XBOX_DOWN|BIINPUT_DEV(port));

	biBindInput(controller->crossChannel,		BIINPUT_XBOX_A|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->triangleChannel,	BIINPUT_XBOX_Y|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->circleChannel,		BIINPUT_XBOX_B|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->squareChannel,		BIINPUT_XBOX_X|BIINPUT_DEV(port), BIINPUT_NULL);

	biBindInput(controller->l1Channel, BIINPUT_XBOX_WHITE|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->l2Channel, BIINPUT_XBOX_LTRIGGER|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->l3Channel, BIINPUT_XBOX_STICK1|BIINPUT_DEV(port), BIINPUT_NULL);
	
	biBindInput(controller->r1Channel, BIINPUT_XBOX_BLACK|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->r2Channel, BIINPUT_XBOX_RTRIGGER|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->r3Channel, BIINPUT_XBOX_STICK2|BIINPUT_DEV(port), BIINPUT_NULL);

	biBindInput(controller->startChannel, BIINPUT_XBOX_START|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->selectChannel, BIINPUT_XBOX_BACK|BIINPUT_DEV(port), BIINPUT_NULL);

	biBindInput(controller->dPadXDebounceChannel, BIINPUT_XBOX_RIGHT|BIINPUT_DEV(port),	BIINPUT_XBOX_LEFT|BIINPUT_DEV(port));
	biBindInput(controller->dPadYDebounceChannel, BIINPUT_XBOX_UP|BIINPUT_DEV(port),		BIINPUT_XBOX_DOWN|BIINPUT_DEV(port));

	biBindInput(controller->crossDebounceChannel,		BIINPUT_XBOX_A|BIINPUT_DEV(port),	BIINPUT_NULL);
	biBindInput(controller->triangleDebounceChannel,	BIINPUT_XBOX_Y|BIINPUT_DEV(port),	BIINPUT_NULL);
	biBindInput(controller->circleDebounceChannel,		BIINPUT_XBOX_B|BIINPUT_DEV(port),	BIINPUT_NULL);
	biBindInput(controller->squareDebounceChannel,		BIINPUT_XBOX_X|BIINPUT_DEV(port),	BIINPUT_NULL);

	biBindInput(controller->l1DebounceChannel, BIINPUT_XBOX_WHITE|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->l2DebounceChannel, BIINPUT_XBOX_LTRIGGER|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->l3DebounceChannel, BIINPUT_XBOX_STICK1|BIINPUT_DEV(port), BIINPUT_NULL);
	
	biBindInput(controller->r1DebounceChannel, BIINPUT_XBOX_BLACK|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->r2DebounceChannel, BIINPUT_XBOX_RTRIGGER|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->r3DebounceChannel, BIINPUT_XBOX_STICK2|BIINPUT_DEV(port), BIINPUT_NULL);

	biBindInput(controller->startDebounceChannel, BIINPUT_XBOX_START|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->selectDebounceChannel, BIINPUT_XBOX_BACK|BIINPUT_DEV(port), BIINPUT_NULL);
#ifndef _MASTER
	biBindInput(controller->guiDebounceChannel, BIINPUT_XBOX_BACK|BIINPUT_DEV(port), BIINPUT_NULL);
#else
	#ifdef FORCEGUION
		biBindInput(controller->guiDebounceChannel, BIINPUT_XBOX_BACK|BIINPUT_DEV(port), BIINPUT_NULL);
	#endif
#endif

	// PP: AUTO-REPEAT MENU NAVIGATION CHANNELS...

	biBindInput(controller->menuUpRepeatChannel, BIINPUT_XBOX_UP|BIINPUT_DEV(port), BIINPUT_XBOX_DOWN|BIINPUT_DEV(port));
	biBindInput(controller->menuUpRepeatChannel, BIINPUT_XBOX_Y1|BIINPUT_DEV(port)|BIINPUT_INVERT, BIINPUT_NULL);

	biBindInput(controller->menuDownRepeatChannel, BIINPUT_XBOX_DOWN|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->menuDownRepeatChannel, BIINPUT_XBOX_Y1|BIINPUT_DEV(port), BIINPUT_NULL);

	biBindInput(controller->menuLeftRepeatChannel, BIINPUT_XBOX_LEFT|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->menuLeftRepeatChannel, BIINPUT_XBOX_X1|BIINPUT_DEV(port)|BIINPUT_INVERT, BIINPUT_NULL);

	biBindInput(controller->menuRightRepeatChannel, BIINPUT_XBOX_RIGHT|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->menuRightRepeatChannel, BIINPUT_XBOX_X1|BIINPUT_DEV(port), BIINPUT_NULL);

	// PP: DEBOUNCED MENU NAVIGATION CONTROLS...

	biBindInput(controller->menuUpDebounceChannel, BIINPUT_XBOX_UP|BIINPUT_DEV(port), BIINPUT_XBOX_DOWN|BIINPUT_DEV(port));
	biBindInput(controller->menuUpDebounceChannel, BIINPUT_XBOX_Y1|BIINPUT_DEV(port)|BIINPUT_INVERT, BIINPUT_NULL);

	biBindInput(controller->menuDownDebounceChannel, BIINPUT_XBOX_DOWN|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->menuDownDebounceChannel, BIINPUT_XBOX_Y1|BIINPUT_DEV(port), BIINPUT_NULL);

	biBindInput(controller->menuLeftDebounceChannel, BIINPUT_XBOX_LEFT|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->menuLeftDebounceChannel, BIINPUT_XBOX_X1|BIINPUT_DEV(port)|BIINPUT_INVERT, BIINPUT_NULL);

	biBindInput(controller->menuRightDebounceChannel, BIINPUT_XBOX_RIGHT|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->menuRightDebounceChannel, BIINPUT_XBOX_X1|BIINPUT_DEV(port), BIINPUT_NULL);

#elif BPLATFORM == GAMECUBE

	biBindInput(controller->x1Channel, BIINPUT_GAMECUBE_X1|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->x2Channel, BIINPUT_GAMECUBE_X2|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->y1Channel, BIINPUT_GAMECUBE_Y1|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->y2Channel, BIINPUT_GAMECUBE_Y2|BIINPUT_DEV(port), BIINPUT_NULL);
	
	biBindInput(controller->x1DebounceChannel, BIINPUT_GAMECUBE_X1|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->x2DebounceChannel, BIINPUT_GAMECUBE_X2|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->y1DebounceChannel, BIINPUT_GAMECUBE_Y1|BIINPUT_DEV(port)|BIINPUT_INVERT, BIINPUT_NULL);
	biBindInput(controller->y2DebounceChannel, BIINPUT_GAMECUBE_Y2|BIINPUT_DEV(port)|BIINPUT_INVERT, BIINPUT_NULL);

	biBindInput(controller->dPadXChannel,	BIINPUT_GAMECUBE_RIGHT|BIINPUT_DEV(port),	BIINPUT_GAMECUBE_LEFT|BIINPUT_DEV(port));
	biBindInput(controller->dPadYChannel,	BIINPUT_GAMECUBE_UP|BIINPUT_DEV(port),		BIINPUT_GAMECUBE_DOWN|BIINPUT_DEV(port));

	biBindInput(controller->crossChannel,		BIINPUT_GAMECUBE_A|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->triangleChannel,	BIINPUT_GAMECUBE_Y|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->circleChannel,		BIINPUT_GAMECUBE_B|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->squareChannel,		BIINPUT_GAMECUBE_X|BIINPUT_DEV(port), BIINPUT_NULL);

//		biBindInput(controller->l1Channel, BIINPUT_GAMECUBE_WHITE|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->l2Channel, BIINPUT_GAMECUBE_L|BIINPUT_DEV(port), BIINPUT_NULL);
//		biBindInput(controller->l3Channel, BIINPUT_GAMECUBE_STICK1|BIINPUT_DEV(port), BIINPUT_NULL);
	
//		biBindInput(controller->r1Channel, BIINPUT_GAMECUBE_BLACK|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->r2Channel, BIINPUT_GAMECUBE_R|BIINPUT_DEV(port), BIINPUT_NULL);
//		biBindInput(controller->r3Channel, BIINPUT_GAMECUBE_STICK2|BIINPUT_DEV(port), BIINPUT_NULL);

	biBindInput(controller->startChannel, BIINPUT_GAMECUBE_START|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->selectChannel, BIINPUT_GAMECUBE_Z|BIINPUT_DEV(port), BIINPUT_NULL);

	biBindInput(controller->dPadXDebounceChannel, BIINPUT_GAMECUBE_RIGHT|BIINPUT_DEV(port),	BIINPUT_GAMECUBE_LEFT|BIINPUT_DEV(port));
	biBindInput(controller->dPadYDebounceChannel, BIINPUT_GAMECUBE_UP|BIINPUT_DEV(port),		BIINPUT_GAMECUBE_DOWN|BIINPUT_DEV(port));

	biBindInput(controller->crossDebounceChannel,		BIINPUT_GAMECUBE_A|BIINPUT_DEV(port),	BIINPUT_NULL);
	biBindInput(controller->triangleDebounceChannel,	BIINPUT_GAMECUBE_Y|BIINPUT_DEV(port),	BIINPUT_NULL);
	biBindInput(controller->circleDebounceChannel,		BIINPUT_GAMECUBE_B|BIINPUT_DEV(port),	BIINPUT_NULL);
	biBindInput(controller->squareDebounceChannel,		BIINPUT_GAMECUBE_X|BIINPUT_DEV(port),	BIINPUT_NULL);

//		biBindInput(controller->l1DebounceChannel, BIINPUT_GAMECUBE_WHITE|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->l2DebounceChannel, BIINPUT_GAMECUBE_L|BIINPUT_DEV(port), BIINPUT_NULL);
//		biBindInput(controller->l3DebounceChannel, BIINPUT_GAMECUBE_STICK1|BIINPUT_DEV(port), BIINPUT_NULL);
	
//		biBindInput(controller->r1DebounceChannel, BIINPUT_GAMECUBE_BLACK|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->r2DebounceChannel, BIINPUT_GAMECUBE_R|BIINPUT_DEV(port), BIINPUT_NULL);
//		biBindInput(controller->r3DebounceChannel, BIINPUT_GAMECUBE_STICK2|BIINPUT_DEV(port), BIINPUT_NULL);

	biBindInput(controller->startDebounceChannel, BIINPUT_GAMECUBE_START|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->selectDebounceChannel, BIINPUT_GAMECUBE_Z|BIINPUT_DEV(port), BIINPUT_NULL);
#ifndef _MASTER
	biBindInput(controller->guiDebounceChannel, BIINPUT_GAMECUBE_Z|BIINPUT_DEV(port), BIINPUT_NULL);
#endif

	// PP: AUTO-REPEAT MENU NAVIGATION CONTROLS...

	biBindInput(controller->menuUpRepeatChannel, BIINPUT_GAMECUBE_UP|BIINPUT_DEV(port), BIINPUT_XBOX_DOWN|BIINPUT_DEV(port));
	biBindInput(controller->menuUpRepeatChannel, BIINPUT_GAMECUBE_Y1|BIINPUT_DEV(port)|BIINPUT_INVERT, BIINPUT_NULL);

	biBindInput(controller->menuDownRepeatChannel, BIINPUT_GAMECUBE_DOWN|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->menuDownRepeatChannel, BIINPUT_GAMECUBE_Y1|BIINPUT_DEV(port), BIINPUT_NULL);

	biBindInput(controller->menuLeftRepeatChannel, BIINPUT_GAMECUBE_LEFT|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->menuLeftRepeatChannel, BIINPUT_GAMECUBE_X1|BIINPUT_DEV(port)|BIINPUT_INVERT, BIINPUT_NULL);

	biBindInput(controller->menuRightRepeatChannel, BIINPUT_GAMECUBE_RIGHT|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->menuRightRepeatChannel, BIINPUT_GAMECUBE_X1|BIINPUT_DEV(port), BIINPUT_NULL);

	// PP: DEBOUNCED MENU NAVIGATION CONTROLS...

	biBindInput(controller->menuUpDebounceChannel, BIINPUT_GAMECUBE_UP|BIINPUT_DEV(port), BIINPUT_XBOX_DOWN|BIINPUT_DEV(port));
	biBindInput(controller->menuUpDebounceChannel, BIINPUT_GAMECUBE_Y1|BIINPUT_DEV(port)|BIINPUT_INVERT, BIINPUT_NULL);

	biBindInput(controller->menuDownDebounceChannel, BIINPUT_GAMECUBE_DOWN|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->menuDownDebounceChannel, BIINPUT_GAMECUBE_Y1|BIINPUT_DEV(port), BIINPUT_NULL);

	biBindInput(controller->menuLeftDebounceChannel, BIINPUT_GAMECUBE_LEFT|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->menuLeftDebounceChannel, BIINPUT_GAMECUBE_X1|BIINPUT_DEV(port)|BIINPUT_INVERT, BIINPUT_NULL);

	biBindInput(controller->menuRightDebounceChannel, BIINPUT_GAMECUBE_RIGHT|BIINPUT_DEV(port), BIINPUT_NULL);
	biBindInput(controller->menuRightDebounceChannel, BIINPUT_GAMECUBE_X1|BIINPUT_DEV(port), BIINPUT_NULL);

#elif BPLATFORM == PC
	return BindInputChannelsPC(controller, port);
#else// PP: platform
	#error "Must setup controls for this platform"
#endif // NH: BPLATFORM

	return TRUE;	
}


/* --------------------------------------------------------------------------------
   Function : BindInputChannels
   Purpose : Bind keys and buttons to input channels
   Parameters : PADCONTROLSTATUS*, logical 0-based pad index
   Returns : TRUE/FALSE
   Info : Uses the BPLATFORM define to decide whether to bind keys or buttons
*/

int BindInputChannels(PADCONTROLSTATUS *controller,int padNumber)
{
	char	logicalPortIndex;

	logicalPortIndex=biFindPad(padNumber);

	if(logicalPortIndex == -1)
	{
		bkPrintf("BindInputChannels: logical pad %d (0-based) not found\n", padNumber);
		return FALSE;
	}

#if(BPLATFORM==PC)
	return BindInputChannelsPC(controller, padNumber);
#else// PP: platform
	return BindInputChannelsToPort(controller, logicalPortIndex);
#endif// PP: platform
}


/*	--------------------------------------------------------------------------------
	Function 	: bindControllersToDefaultPorts
	Purpose 	: binds each PADCONTROLSTATUS structure to the port corresponding to its number;
					ie. (re)binds controller1..4 to ports 1..4
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: call this when you set padCheckMode to PADCHECK_ALL or PADCHECK_WAITING_FOR_PLAYER1
*/
void bindControllersToDefaultPorts(void)
{
	// PP: I should've just done this to start with
	static PADCONTROLSTATUS*	padArray[4]={&controller1, &controller2, &controller3, &controller4};
	uchar						padIndex=0;

	for(int port=0; port<NUM_CONTROLLER_PORTS; port++)
	{
		if((padCheckMode == PADCHECK_WAITING_FOR_PLAYER2) || (padCheckMode == PADCHECK_NORMAL))
		{
			if(gameStatus.pad1Present)
			{
				if(port == controller1.port)
				{
					// PP: skip this port; it's already being used
					continue;
				}

				if(padArray[padIndex]== &controller1)
				{
					// PP: can't use this controller struct; it's already bound
					padIndex++;

					if(padIndex == NUM_CONTROLLER_PORTS)
					{
						break;
					}
				}
			}
		}

		if(padCheckMode == PADCHECK_NORMAL)
		{
			if(gameStatus.pad2Present)
			{
				if(port == controller2.port)
				{
					// PP: skip this port; it's already being used
					continue;
				}

				if(padArray[padIndex]== &controller2)
				{
					// PP: can't use this controller struct; it's already bound
					padIndex++;

					if(padIndex == NUM_CONTROLLER_PORTS)
					{
						break;
					}
				}
			}
		}

		// PP: bind controller to port
		ClearInputBindings(padArray[padIndex]);
		BindInputChannelsToPort(padArray[padIndex],port);

		// PP: imagine forgetting this - silly fool
		padIndex++;

		if(padIndex == NUM_CONTROLLER_PORTS)
		{
			break;
		}
	}
}


/* --------------------------------------------------------------------------------
   Function : defaultKeyboardControls
   Purpose : Sets the default keyboard controls for a player
   Parameters : 
   Returns : 
   Info : 
*/
void defaultKeyboardControls(PADCONTROLSTATUS *controller, int  padNumber)
{
	switch (padNumber)
	{
	case 0:
		{
#ifdef _TAZDEBUG
/*				biBindInput(controller->tabChannel, BIINPUT_KEY_TAB, BIINPUT_NULL);	
				biBindInput(controller->commaDebounceChannel, BIINPUT_KEY_COMMA, BIINPUT_NULL);
				biBindInput(controller->periodDebounceChannel, BIINPUT_KEY_PERIOD, BIINPUT_NULL);
				biBindInput(controller->lbracketDebounceChannel, BIINPUT_KEY_LBRACKET, BIINPUT_NULL);
				biBindInput(controller->rbracketDebounceChannel, BIINPUT_KEY_RBRACKET, BIINPUT_NULL);

				biBindInput(controller->equalsChannel, BIINPUT_KEY_EQUALS, BIINPUT_NULL);
				biBindInput(controller->minusChannel, BIINPUT_KEY_MINUS, BIINPUT_NULL);

				biBindInput(controller->l1Channel, BIINPUT_KEY_Z, BIINPUT_NULL);
				biBindInput(controller->l2Channel, BIINPUT_KEY_Q, BIINPUT_NULL);
				biBindInput(controller->l3Channel, BIINPUT_KEY_S, BIINPUT_NULL);
				
				biBindInput(controller->lshiftChannel, BIINPUT_KEY_LSHIFT , BIINPUT_NULL);

				biBindInput(controller->lshiftDebounceChannel, BIINPUT_KEY_LSHIFT , BIINPUT_NULL);

				biBindInput(controller->tabDebounceChannel, BIINPUT_KEY_TAB, BIINPUT_NULL);

				biBindInput(controller->l1DebounceChannel, BIINPUT_KEY_Z, BIINPUT_NULL);
				biBindInput(controller->l2DebounceChannel, BIINPUT_KEY_Q, BIINPUT_NULL);
				biBindInput(controller->l3DebounceChannel, BIINPUT_KEY_S, BIINPUT_NULL);
				
				biBindInput(controller->f10DebounceChannel, BIINPUT_KEY_F10, BIINPUT_NULL);
				biBindInput(controller->rDebounceChannel, BIINPUT_KEY_R, BIINPUT_NULL);
				biBindInput(controller->gDebounceChannel, BIINPUT_KEY_G, BIINPUT_NULL);
				biBindInput(controller->sDebounceChannel, BIINPUT_KEY_S, BIINPUT_NULL);
				biBindInput(controller->aDebounceChannel, BIINPUT_KEY_A, BIINPUT_NULL);
				biBindInput(controller->mDebounceChannel, BIINPUT_KEY_M, BIINPUT_NULL);
				biBindInput(controller->hDebounceChannel, BIINPUT_KEY_H, BIINPUT_NULL);
				biBindInput(controller->iDebounceChannel, BIINPUT_KEY_I, BIINPUT_NULL);
				biBindInput(controller->f3DebounceChannel, BIINPUT_KEY_F3, BIINPUT_NULL);
				biBindInput(controller->f5DebounceChannel, BIINPUT_KEY_F5, BIINPUT_NULL);
				biBindInput(controller->f6DebounceChannel, BIINPUT_KEY_F6, BIINPUT_NULL);



				biBindInput(controller->y2Channel, BIINPUT_KEY_NUMPAD2, BIINPUT_KEY_NUMPAD8);
				biBindInput(controller->y2DebounceChannel, BIINPUT_KEY_NUMPAD2|BIINPUT_INVERT, BIINPUT_KEY_NUMPAD8|BIINPUT_INVERT);		

				biBindInput(controller->tabChannel, BIINPUT_KEY_TAB, BIINPUT_NULL);
				biBindInput(controller->commaDebounceChannel, BIINPUT_KEY_COMMA, BIINPUT_NULL);
				biBindInput(controller->periodDebounceChannel, BIINPUT_KEY_PERIOD, BIINPUT_NULL);
		
				biBindInput(controller->lbracketDebounceChannel, BIINPUT_KEY_LBRACKET, BIINPUT_NULL);
				biBindInput(controller->rbracketDebounceChannel, BIINPUT_KEY_RBRACKET, BIINPUT_NULL);

				biBindInput(controller->equalsChannel, BIINPUT_KEY_EQUALS, BIINPUT_NULL);
				biBindInput(controller->minusChannel, BIINPUT_KEY_MINUS, BIINPUT_NULL);

				biBindInput(controller->dPadXChannel, BIINPUT_KEY_NEXT, BIINPUT_KEY_DELETE);
				biBindInput(controller->dPadYChannel, BIINPUT_KEY_HOME, BIINPUT_KEY_END);

				biBindInput(controller->l3Channel, BIINPUT_KEY_S, BIINPUT_NULL);

				biBindInput(controller->lshiftChannel, BIINPUT_KEY_LSHIFT , BIINPUT_NULL);

				biBindInput(controller->lshiftDebounceChannel, BIINPUT_KEY_LSHIFT , BIINPUT_NULL);

				biBindInput(controller->tabDebounceChannel, BIINPUT_KEY_TAB, BIINPUT_NULL);
				biBindInput(controller->dPadXDebounceChannel, BIINPUT_KEY_NEXT, BIINPUT_KEY_DELETE);
				biBindInput(controller->dPadYDebounceChannel, BIINPUT_KEY_HOME, BIINPUT_KEY_END);

				biBindInput(controller->l3DebounceChannel, BIINPUT_KEY_S, BIINPUT_NULL);

				biBindInput(controller->f10DebounceChannel, BIINPUT_KEY_F10, BIINPUT_NULL);
				biBindInput(controller->rDebounceChannel, BIINPUT_KEY_R, BIINPUT_NULL);
				biBindInput(controller->gDebounceChannel, BIINPUT_KEY_G, BIINPUT_NULL);
				biBindInput(controller->sDebounceChannel, BIINPUT_KEY_S, BIINPUT_NULL);
				biBindInput(controller->aDebounceChannel, BIINPUT_KEY_A, BIINPUT_NULL);
				biBindInput(controller->mDebounceChannel, BIINPUT_KEY_M, BIINPUT_NULL);
				biBindInput(controller->hDebounceChannel, BIINPUT_KEY_H, BIINPUT_NULL);

				biBindInput(controller->iDebounceChannel, BIINPUT_KEY_I, BIINPUT_NULL);
				biBindInput(controller->f3DebounceChannel, BIINPUT_KEY_F3, BIINPUT_NULL);
				biBindInput(controller->f5DebounceChannel, BIINPUT_KEY_F5, BIINPUT_NULL);
				biBindInput(controller->f6DebounceChannel, BIINPUT_KEY_F6, BIINPUT_NULL);
*/
#endif// PP: def _TAZDEBUG

			biBindInput(controller->x1Channel, BIINPUT_KEY_RIGHT, BIINPUT_KEY_LEFT);
			//biBindInput(controller->x2Channel, BIINPUT_KEY_E, BIINPUT_KEY_T);
			biBindInput(controller->y1Channel, BIINPUT_KEY_DOWN, BIINPUT_KEY_UP);

			biBindInput(controller->x1DebounceChannel, BIINPUT_KEY_RIGHT | BIINPUT_INVERT, BIINPUT_KEY_LEFT | BIINPUT_INVERT);
			//biBindInput(controller->x2DebounceChannel, BIINPUT_KEY_T | BIINPUT_INVERT, BIINPUT_KEY_E | BIINPUT_INVERT);
			biBindInput(controller->y1DebounceChannel, BIINPUT_KEY_DOWN | BIINPUT_INVERT, BIINPUT_KEY_UP | BIINPUT_INVERT);
			
			biBindInput(controller->crossChannel, BIINPUT_KEY_C, BIINPUT_NULL);
			biBindInput(controller->triangleChannel, BIINPUT_KEY_R , BIINPUT_NULL);
			biBindInput(controller->circleChannel, BIINPUT_KEY_F , BIINPUT_NULL);
			biBindInput(controller->squareChannel, BIINPUT_KEY_D, BIINPUT_NULL);

			biBindInput(controller->l1Channel, BIINPUT_KEY_E, BIINPUT_NULL);
			biBindInput(controller->l2Channel, BIINPUT_KEY_X, BIINPUT_NULL);
				
			biBindInput(controller->r1Channel, BIINPUT_KEY_T, BIINPUT_NULL);
			biBindInput(controller->r2Channel, BIINPUT_KEY_V, BIINPUT_NULL);

			biBindInput(controller->startChannel, BIINPUT_KEY_P, BIINPUT_NULL);
			biBindInput(controller->selectChannel, BIINPUT_KEY_SPACE, BIINPUT_NULL);

			biBindInput(controller->crossDebounceChannel, BIINPUT_KEY_C, BIINPUT_NULL);
			biBindInput(controller->triangleDebounceChannel, BIINPUT_KEY_R, BIINPUT_NULL);
			biBindInput(controller->circleDebounceChannel, BIINPUT_KEY_F, BIINPUT_NULL);
			biBindInput(controller->squareDebounceChannel, BIINPUT_KEY_D, BIINPUT_NULL);

			biBindInput(controller->l1DebounceChannel, BIINPUT_KEY_E, BIINPUT_NULL);
			biBindInput(controller->l2DebounceChannel, BIINPUT_KEY_X, BIINPUT_NULL);
				
			biBindInput(controller->r1DebounceChannel, BIINPUT_KEY_T, BIINPUT_NULL);
			biBindInput(controller->r2DebounceChannel, BIINPUT_KEY_V, BIINPUT_NULL);

			biBindInput(controller->startDebounceChannel, BIINPUT_KEY_P, BIINPUT_NULL);
			biBindInput(controller->selectDebounceChannel, BIINPUT_KEY_SPACE, BIINPUT_NULL);				
		}
		break;
	case 1:
		{
			biBindInput(controller->x1Channel, BIINPUT_KEY_NUMPAD6, BIINPUT_KEY_NUMPAD4);
			//biBindInput(controller->x2Channel, BIINPUT_KEY_E, BIINPUT_KEY_T);
			biBindInput(controller->y1Channel, BIINPUT_KEY_NUMPAD2, BIINPUT_KEY_NUMPAD8);

			biBindInput(controller->x1DebounceChannel, BIINPUT_KEY_NUMPAD6 | BIINPUT_INVERT, BIINPUT_KEY_NUMPAD4 | BIINPUT_INVERT);
			//biBindInput(controller->x2DebounceChannel, BIINPUT_KEY_T | BIINPUT_INVERT, BIINPUT_KEY_E | BIINPUT_INVERT);
			biBindInput(controller->y1DebounceChannel, BIINPUT_KEY_NUMPAD2 | BIINPUT_INVERT, BIINPUT_KEY_NUMPAD8 | BIINPUT_INVERT);
			
			biBindInput(controller->crossChannel, BIINPUT_KEY_NUMPAD0, BIINPUT_NULL);
			biBindInput(controller->triangleChannel, BIINPUT_KEY_SUBTRACT , BIINPUT_NULL);
			biBindInput(controller->circleChannel, BIINPUT_KEY_NUMPADENTER , BIINPUT_NULL);
			biBindInput(controller->squareChannel, BIINPUT_KEY_NUMPAD5, BIINPUT_NULL);

			biBindInput(controller->l1Channel, BIINPUT_KEY_NUMPAD7, BIINPUT_NULL);
			biBindInput(controller->l2Channel, BIINPUT_KEY_NUMPAD9, BIINPUT_NULL);
				
			biBindInput(controller->r1Channel, BIINPUT_KEY_NUMPAD1, BIINPUT_NULL);
			biBindInput(controller->r2Channel, BIINPUT_KEY_NUMPAD3, BIINPUT_NULL);

			biBindInput(controller->startChannel, BIINPUT_KEY_P, BIINPUT_NULL);
			biBindInput(controller->selectChannel, BIINPUT_KEY_SPACE, BIINPUT_NULL);

			biBindInput(controller->crossDebounceChannel, BIINPUT_KEY_NUMPAD0, BIINPUT_NULL);
			biBindInput(controller->triangleDebounceChannel, BIINPUT_KEY_SUBTRACT, BIINPUT_NULL);
			biBindInput(controller->circleDebounceChannel, BIINPUT_KEY_NUMPADENTER, BIINPUT_NULL);
			biBindInput(controller->squareDebounceChannel, BIINPUT_KEY_NUMPAD5, BIINPUT_NULL);

			biBindInput(controller->l1DebounceChannel, BIINPUT_KEY_NUMPAD7, BIINPUT_NULL);
			biBindInput(controller->l2DebounceChannel, BIINPUT_KEY_NUMPAD9, BIINPUT_NULL);
				
			biBindInput(controller->r1DebounceChannel, BIINPUT_KEY_NUMPAD1, BIINPUT_NULL);
			biBindInput(controller->r2DebounceChannel, BIINPUT_KEY_NUMPAD3, BIINPUT_NULL);

			biBindInput(controller->startDebounceChannel, BIINPUT_KEY_P, BIINPUT_NULL);
			biBindInput(controller->selectDebounceChannel, BIINPUT_KEY_SPACE, BIINPUT_NULL);
		}
		break;

	}
}

/* --------------------------------------------------------------------------------
   Function : DIKToBabel
   Purpose : Converts a DIK (Direct Input Key) code to the Babel version
   Parameters : int (DIKCode)
   Returns : int (Bable code)
   Info : Scans through the LUT to find the correct Babel code
*/
int DIKToBabel(int DIKCode)
{
	int enumeratedKey = BIINPUT_NULL;

#if BPLATFORM==PC
	
	// Search for DIKCode in lookup table
	while((enumeratedKey<BDIRECTINPUTKEYMAPSIZE) && (bDirectInputKeyMap[enumeratedKey] != DIKCode))
		enumeratedKey++;

	if (bDirectInputKeyMap[enumeratedKey] != DIKCode)
		enumeratedKey = BIINPUT_NULL;
#endif				

	enumeratedKey += BIINPUT_KEY_FIRSTID + 1;
	return enumeratedKey;
}


// PP: Macro to re-bind a controller.  Pass in the controller, and a LOGICAL pad index!
#define REBIND(_controller, _logicalPadId) \
		ClearInputBindings(&(_controller)); \
		BindInputChannels(&(_controller), _logicalPadId);

//		FreeRumbleController(&(_controller));
//		CreateInputChannels(&(_controller), _logicalPadId);


/*	--------------------------------------------------------------------------------
	Function 	: checkForMissingControllers2_old
	Purpose 	: find and bind required controllers; if a required controller is missing,
					freeze the game and show the missing-controller message
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: This is the hassle-free no-nonsense version.  Not pretty,
					but very effective!
*/
void checkForMissingControllers2_old(bool display)
{
	extern BOOK		missingControllerBooks[MAX_PLAYERS];// PP: pause.cpp
	bool			padMissing				=false;
	int				pad1Port, pad2Port;
	static bool		foundPadLatch			=false;

	// PP: the delay in real seconds, after the game starts, before a missing controller will be indicated to the player
	// PP: (it seems to take a "whee while" for the pad to be found)
#define PADMISSING_INITIAL_DELAY			0.9f// PP: should probably be kept at less than a second, for Xbox TCR?

	// PP: Aw look I'm tired ok?!
#define madPissing			padMissing

	if(biGetPortStatus(controller1.port)!=EBPORTSTATUS_CONNECTED)
	{
		gameStatus.pad1Present=false;
		gameStatus.pad2Present=false;

		padMissing=true;
	}
	else
	{
		// PP: found pad 1

		gameStatus.pad1Present=true;

// TP: always bind second pad		if(gameStatus.multiplayer.numPlayers == 2)
		{
			pad2Port=biFindPad(1);

			if(pad2Port != controller2.port)
			{
				gameStatus.pad2Present=false;

				if (gameStatus.multiplayer.numPlayers==2)
				{
					padMissing=true;
				}
			}
			else
			{
				// PP: found pad 2 also

				gameStatus.pad2Present=true;

				if(pad2Port != controller2.port)
				{
					REBIND(controller2, 1);
				}
			}
		}
	}

	if(!foundPadLatch)
	{
		if(!padMissing)
		{
			foundPadLatch = true;

			// PP: just to be safe

		}
	}

#ifdef CONSUMERDEMO

	if(!CONSUMER_DEMO_PLAYABLE)
	{
		if(!foundPadLatch)
		{
			UpdateController(&controller1);// PP: needed for idleTime to update

			// PP: don't need any pads for attract mode
			padMissing=false;

		// PP: 	bkPrintf("checkForMissingControllers2: ATTRACT MODE - NO NEED FOR CONTROLLERS\n");
		}
	}

#endif// PP: def  CONSUMERDEMO

	if((padMissing)// PP: if a pad is missing
		&&(display)// PP: and if we want to indicate that it's missing
		&&(gameStatus.appTime>PADMISSING_INITIAL_DELAY))// PP: and if a certain amount of time has passed (pads don't seem to get picked up immediately)
	{
		int i;

		if (!gameStatus.pad1Present)
			i = 0;
		else if (!gameStatus.pad2Present)
			i = 1;
		
		missingControllerBooks[i].flags &= ~BKFLAG_BOOKLIST;// PP: bodge
		missingControllerBooks[i].open(NULL, true);
		missingControllerBooks[i].privateUpdate();

		SETPROJECTION(2D);
		ENABLEBLEND(TRUE);
		SETBLEND(BLEND);
		SETZWRITE(FALSE);
		SETZTEST(ALWAYS);

		bdSetIdentityObjectMatrix();

		bsPauseAllChannels(BSPAUSE);

		missingControllerBooks[i].privateDraw();

		Flip(0, 0, 0, 128);

		do
		{
			padMissing=false;

			biReadDevices();

			if(biFindPad(0) == -1)
			{
				padMissing=true;
			}
			else
			{
				// PP: got pad 1

				pad1Port=biFindPad(0);

				if(pad1Port != controller1.port)
				{
					controller1.port=pad1Port;

					REBIND(controller1, 0);
				}

				if (!gameStatus.multiplayer.versusCPU)	// JW: if(gameStatus.multiplayer.numPlayers == 2)
				{
					pad2Port=biFindPad(1);

					if(pad2Port == -1)
					{
						padMissing=true;
					}
					else
					{
						// PP: got both pads

						if(pad2Port != controller2.port)
						{
							controller2.port=pad2Port;

							REBIND(controller2, 1);
						}

						padMissing= (controller1.startDebounceChannel->value == 0) && (controller2.startDebounceChannel->value == 0);
					}
				}
				else
				{
					// PP: 1 player

					padMissing= (controller1.startDebounceChannel->value == 0);
				}
			}

			bsUpdate((int)(gameStatus.deltaTime));

#if(BPLATFORM == XBOX)// PP: TEMP MILESTONE FIX: NEED THIS TO KEEP THE STREAMING MUSIC TICKING OVER
			// PP: from bdFlip...
			DirectSoundDoWork();
			bUpdateSound();
			bDecodeWMA();
#endif// PP: xbox

			bkSleep(0);

		}while(padMissing);

		biForceDebounce(controller1.startDebounceChannel);
		biForceDebounce(controller2.startDebounceChannel);

		bsPauseAllChannels(BSUNPAUSE);

		// PP: start timing the frame
		startTimingFrame();
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: checkForPadRemoval
	Purpose 	: check for the removal of a controller
	Parameters 	: ref to controller, ref to bool indicating the presence of the controller
	Returns 	: true if the pad has been removed
	Info 		: 
*/
static bool checkForPadRemoval(PADCONTROLSTATUS& controller, bool& present)
{
	if(biGetPortStatus(controller.port) == EBPORTSTATUS_CONNECTED)
	{
		present=true;
		return false;
	}
	else
	{
		present=false;
		return true;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: checkForMissingControllers2
	Purpose 	: find and bind required controllers; if a required controller is missing,
					freeze the game and show the missing-controller message
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: This is the hassle-free no-nonsense version.  Not pretty,
					but very effective!
*/
void checkForMissingControllers2(bool display)
{
	extern BOOK					missingControllerBooks[MAX_PLAYERS];// PP: pause.cpp
	bool						padMissing				=false;
	int							pad1Port, pad2Port;
	static bool					foundPadLatch			=false;

	if (bInputInfo.noofConnectedDevices > numPadsConnected) numPadsConnected = bInputInfo.noofConnectedDevices;

	// PP: the delay in real seconds, after the game starts, before a missing controller will be indicated to the player
	// PP: (it seems to take a "whee while" for the pad to be found)
#define PADMISSING_INITIAL_DELAY			0.9f// PP: should probably be kept at less than a second, for Xbox TCR?

	// PP: update unused pads...

#define FIND_AND_BIND(_1basedPadNum)\
	if(controller##_1basedPadNum.port < 0)/* PP: not bound*/\
	{\
		if(biGetPortStatus(_1basedPadNum-1) == EBPORTSTATUS_CONNECTED)\
		{\
			/* PP: ooh look, a pad has been inserted - TODO: interrupt the attract cycle (on Xbox) somehow*/\
			BindInputChannelsToPort(&controller##_1basedPadNum, _1basedPadNum-1);\
		}\
	}

	switch(padCheckMode)
	{
	case PADCHECK_ALL:
	case PADCHECK_WAITING_FOR_PLAYER1:
		{
			// PP: update controller1
			FIND_AND_BIND(1);
		}// PP: nobreak
	case PADCHECK_WAITING_FOR_PLAYER2:
		{	
			// PP: controller1 is bound to player1's pad
			// PP: update controller2..4
			FIND_AND_BIND(2);
			FIND_AND_BIND(3);
			FIND_AND_BIND(4);
		}
		break;
	}

	// PP: update used pads...

#ifdef ALLOW_CONTROLLERS_IN_ANY_PORTS
	if(padCheckMode == PADCHECK_NORMAL)
#endif
	{
		// PP: check for removal of controller1 or controller2 as required

		padMissing=checkForPadRemoval(controller1, gameStatus.pad1Present);

		// PP: deal with only one missing controller at a time - this should work
		if(padMissing)
		{
			// PP: NEW: pause vibrations on controller1 and controller2
			// PP: NOTE: must continually poll bkUpdate for the vibrations to actually stop

			FlushRumbleQueue(&controller1);
			PauseRumble(&controller1);
			FlushRumbleQueue(&controller2);
			PauseRumble(&controller2);
#if(NUM_CONTROLLER_PORTS == 4)
			FlushRumbleQueue(&controller3);
			PauseRumble(&controller3);
			FlushRumbleQueue(&controller4);
			PauseRumble(&controller4);
#endif

			// PP: NEW
			// TP: Removed this to fix a bug where the controllers are removed during loading  - bindControllersToDefaultPorts();
		}
		else
		{
			// PP: IF WE'RE IN 2 PLAYER
			if (!gameStatus.multiplayer.versusCPU)	// JW: if(gameStatus.multiplayer.numPlayers == 2)
			{
				padMissing=checkForPadRemoval(controller2, gameStatus.pad2Present);

				if(padMissing)
				{
					// PP: NEW: pause vibrations on controller1 and controller2
					// PP: NOTE: must continually poll bkUpdate for the vibrations to actually stop

					FlushRumbleQueue(&controller1);
					PauseRumble(&controller1);
					FlushRumbleQueue(&controller2);
					PauseRumble(&controller2);
#if(NUM_CONTROLLER_PORTS == 4)
					FlushRumbleQueue(&controller3);
					PauseRumble(&controller3);
					FlushRumbleQueue(&controller4);
					PauseRumble(&controller4);
#endif

					// PP: NEW
					// TP: Removed this to fix a bug where the controllers are removed during loading  - bindControllersToDefaultPorts();
				}
			}
		}
	}
#ifdef ALLOW_CONTROLLERS_IN_ANY_PORTS
	else if(padCheckMode == PADCHECK_WAITING_FOR_PLAYER2)
	{
		// PP: while we're waiting for player 2, check for player 1 removing their pad
		padMissing=checkForPadRemoval(controller1, gameStatus.pad1Present);
	}
#endif

	if((padMissing)// PP: if a pad is missing
		&&(display)// PP: and if we want to indicate that it's missing
		&&(gameStatus.appTime>PADMISSING_INITIAL_DELAY))// PP: and if a certain amount of time has passed (pads don't seem to get picked up immediately)
	{
		int i;

		// PP: do this so's we can check which port the player wants to use
		bindControllersToDefaultPorts();

		if (!gameStatus.pad1Present)
		{
			i = 0;

			// PP: we're waiting for player 1 to choose a pad
		//	padCheckMode=PADCHECK_WAITING_FOR_PLAYER1;
		}
		else if (!gameStatus.pad2Present)
		{
			i = 1;

			// PP: we're waiting for player 2 to choose a pad
		//	padCheckMode=PADCHECK_WAITING_FOR_PLAYER2;
		}
		
		missingControllerBooks[i].flags &= ~BKFLAG_BOOKLIST;// PP: bodge
		missingControllerBooks[i].open(NULL, true);
		missingControllerBooks[i].privateUpdate();

		SETPROJECTION(2D);
		ENABLEBLEND(TRUE);
		SETBLEND(BLEND);
		SETZWRITE(FALSE);
		SETZTEST(ALWAYS);

		// PP: NEW (30.4.02): put all (r,g,b) framewrite on for this message (might be in stereoscopic mode)
		SETFRAMEWRITE(BDFRAMEWRITE_RED|BDFRAMEWRITE_GREEN|BDFRAMEWRITE_BLUE);

		bdSetIdentityObjectMatrix();

		bsPauseAllChannels(BSPAUSE);

		missingControllerBooks[i].privateDraw();

		Flip(0, 0, 0, 128);

		TBTimerValue	startTime, finishTime;

		float			localFTime;			// PP: was 'fTime', which is already a global
		float			elapsedTime=0.0f;	// PP: previously wasn't initialised before being used

		startTime = bkTimerRead();

		do
		{
#ifdef CONSUMERDEMO
			finishTime = bkTimerRead();

			localFTime = 1.0f / bkTimerToFPS(bkTimerDelta(startTime,finishTime));

			elapsedTime += localFTime;

			if (elapsedTime > to_inactive)
			{
				camera[0].fade.StartFadeDown(FADETYPE_NORMAL, 1.0f);
				camera[0].fade.SetFadeDownFinishedCallback(StopDemoGameFadeDownCallback);

				if (!gameStatus.multiplayer.versusCPU)
					camera[1].fade.StartFadeDown(FADETYPE_NORMAL, 1.0f);

				if (gameStatus.currentState == GAMESTATE_PAUSE)
				{
					if (!gameStatus.multiplayer.versusCPU)
						ExitMPPauseMode();
					else
						ExitPauseMode();
				}

				break;
			}

			startTime = bkTimerRead();
#else
			// TP: No you don't, timing out of the missing controller message is a TCR no no
			/*
			if (map.sceneNumber == SCENE_LANGUAGESELECT || map.sceneNumber == SCENE_FRONTEND)
			{
				finishTime = bkTimerRead();

				localFTime = 1.0f / bkTimerToFPS(bkTimerDelta(startTime,finishTime));

				elapsedTime += localFTime;

				if (elapsedTime > 30)
				{
					gameStatus.demoMode = 1;
					
					ChangeScene(&gameStatus, SCENE_SAFARI);

					break;
				}

				startTime = bkTimerRead();
			}*/
#endif
			
			padMissing=true;

			biReadDevices();

			FlushRumbleQueue(&controller1);
			PauseRumble(&controller1);
			FlushRumbleQueue(&controller2);
			PauseRumble(&controller2);
#if(NUM_CONTROLLER_PORTS == 4)
			FlushRumbleQueue(&controller3);
			PauseRumble(&controller3);
			FlushRumbleQueue(&controller4);
			PauseRumble(&controller4);
#endif

		
			// PP: ~~~~~~~~~~~~~

			if(!gameStatus.pad1Present)
			{
				// PP: check for the start button being pressed on any pad;
				// PP: the first pad to press start will become controller1

				PADCONTROLSTATUS*	padToIgnore=NULL;

				// PP: ignore the other pad if it's already bound
				if(!gameStatus.multiplayer.versusCPU)
				{
					// PP: 2 players

					if(padCheckMode == PADCHECK_NORMAL)
					{
						padToIgnore= &controller2;
					}
				}

				if(checkForStartAndBind(controller1, padToIgnore))
				{
					// PP: REMOUT: shouldn't need this	padCheckMode=PADCHECK_NORMAL;
					gameStatus.pad1Present=true;
					padMissing=false;

					// PP: new: bind up the other ports
					bindControllersToDefaultPorts();
				}
			}
			else// PP: assume !gameStatus.pad2Present
			{
				// PP: check for the start button being pressed on any pad;
				// PP: the first pad to press start will become controller2
				if(checkForStartAndBind(controller2, &controller1))// PP: 2nd param: controller1 already bound; ignore any START signal from it
				{
					padCheckMode=PADCHECK_NORMAL;
					gameStatus.pad2Present=true;
					padMissing=false;
				}
			}

			// PP: ~~~~~~~~~~~~~

			// PP: update sound
			bsUpdate((int)(gameStatus.deltaTime));

			/*
#if(BPLATFORM == XBOX)// PP: TEMP MILESTONE FIX: NEED THIS TO KEEP THE STREAMING MUSIC TICKING OVER
			// PP: from bdFlip...
			DirectSoundDoWork();
			bUpdateSound();
			bDecodeWMA();
#endif// PP: xbox
			/*/

			// PP: bkUpdate is the guts of bdFlip
			// PP: needed to keep streaming music ticking over, also to update the vibration (allowing it to stop)
			bkUpdate();

			//*/

			bkSleep(0);

		}while(padMissing);

		// PP: NEW (30.4.02): restore framewrite now we're finished showing the message (might be in stereoscopic mode)
		SETFRAMEWRITE(rgbFramewriteChannels);

		biForceDebounce(controller1.startDebounceChannel);
		biForceDebounce(controller2.startDebounceChannel);

		// PP: start timing the frame
		startTimingFrame();

		// PP: NEW: enter pause menu (in response to Pre-Cert Compliance Report 12.4.02)
		if(OkToEnterPause())
		{
			if(gameStatus.multiplayer.versusCPU)// PP: in 1 player
			{
				EnterPauseMode();
			}
			else// PP: in 2 player
			{
#if(BPLATFORM == PC)
				// PP: NOTE: P1 Will ALWAYS control pause on PC.  Pause button is the same for both players
				// PP: // NH: On PC use a Fullscreen Multiplayer Pause
				EnterMPPause(0, true);
#else// PP: !pc

				if(i==0)// PP: pad 1 was missing; open pause menu on player 1's screen
				{
					EnterMPPause(0);
				}
				else// PP: pad 2 was missing; open pause menu on player 2's screen
				{
					EnterMPPause(1);
				}

#endif// PP: !pc
			}
		}// PP: end if(OkToEnterPause())
		else
		{
			// PP: NEW: un-pause vibrations on controller1 and controller2
			UnPauseRumble(&controller1);
			UnPauseRumble(&controller2);

			bsPauseAllChannels(BSUNPAUSE);
		}

	}// PP: end if(padMissng && display, etc)
}


/*	--------------------------------------------------------------------------------
	Function 	: checkForStartAndBind
	Purpose 	: check for the START button on a particular pad; bind a PADCONTROLSTATUS
					struct to the port from which the START signal was detected
	Parameters 	: ref to PADCONTROLSTATUS to bind to port from which if any the START signal is detected,
					ptr to PADCONTROLSTATUS that has already been bound and is therefore to be ignored,
					ref to controller from which to detect START signal
	Returns 	: true if START was pressed, else false
	Info 		: 
*/
static bool checkForStartAndBind(PADCONTROLSTATUS& controller, PADCONTROLSTATUS* const controllerAlreadyBound, PADCONTROLSTATUS& controllerToCheck)
{
	if(controllerAlreadyBound != NULL)
	{
		if(controllerToCheck.port == controllerAlreadyBound->port)
		{
			// PP: port already used
			return false;
		}
	}

#if (BPLATFORM==XBOX)&&(defined CONSUMERDEMO)
	if((controllerToCheck.startDebounceChannel->value)||(controllerToCheck.crossDebounceChannel->value))
#else
	if(controllerToCheck.startDebounceChannel->value)
#endif
	{
		ClearInputBindings(&controller);

		BindInputChannelsToPort(&controller, controllerToCheck.port);/*bind the player to the pad on which they just pressed start*/

		// PP: NEW TEST: force-debounce the start signal
		biForceDebounce(controller.startDebounceChannel);

		return true;
	}

	return false;
}


/*	--------------------------------------------------------------------------------
	Function 	: checkForStartAndBind
	Purpose 	: check for the START button on any pad; bind a PADCONTROLSTATUS
					struct to the port from which the START signal was detected
	Parameters 	: ref to PADCONTROLSTATUS to bind to port from which if any the START signal is detected,
					(opt/NULL) ptr to PADCONTROLSTATUS that has already been bound and is therefore to be ignored
	Returns 	: true if START was pressed, else false
	Info 		: 
*/
bool checkForStartAndBind(PADCONTROLSTATUS& controller, PADCONTROLSTATUS* const controllerAlreadyBound)
{
	// PP: no, I don't like it either...

#ifdef ALLOW_CONTROLLERS_IN_ANY_PORTS

	if(checkForStartAndBind(controller, controllerAlreadyBound, controller1)) return true;
	if(checkForStartAndBind(controller, controllerAlreadyBound, controller2)) return true;

#if(NUM_CONTROLLER_PORTS == 4)

	if(checkForStartAndBind(controller, controllerAlreadyBound, controller3)) return true;
	if(checkForStartAndBind(controller, controllerAlreadyBound, controller4)) return true;

#endif// PP: 4 ports

#else// PP: ifndef ALLOW_CONTROLLERS_IN_ANY_PORTS

	// PP: ps2 does this one

	return (controller.startDebounceChannel->value != 0);

#endif// PP: ifndef ALLOW_CONTROLLERS_IN_ANY_PORTS

	return false;
}