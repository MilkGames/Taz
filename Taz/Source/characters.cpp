// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : characters.cpp
//   Purpose : provides character response code
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"

#include "physics.h"

#include "camera.h"
#include "control.h"
#include "characters.h"

#include "maths.h"
#include "main.h"
#include "swingingobjects.h"
#include "status.h" 
#include "thoughtbubble.h"
#include "animation.h"
#include "scene.h"
#include "genericai.h"
#include "trail.h"
#include "sfx.h"
#include "holysmoke.h"
#include "genericai.h"
#include "superburp.h"
#include "eating.h"
#include "hiccup.h"
#include "invisibletaz.h"
#include "debris.h"
#include "destructibles.h"
#include "bubblegum.h"
#include "chillipepper.h"   
#include "VideoFX.h"				// PP: Video effects - motion blur, defocus, whatever.  Top-of-the-Pops-TASTIC, Mate!
#include "voodoocloud.h"
#include "music.h"					// PP: music stuff
#include "display.h"				// PP: provide basic display setup and managment functions
#include "Cheats.h"					// PP: Cheats
#include "transportereffect.h"
#include "cameracase.h"
#include "trigger.h"


/* --------------------------------------------------------------------------------
   Function : InitialiseCharacterInfo
   Purpose : initialise character info structure
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void InitialiseCharacterInfo(ACTORINSTANCE *actorInstance, uint32 createFlags)
{
	CHARACTERINFO *characterInfo = actorInstance->characterInfo;
	float		i;

	InitAttachedInfo(actorInstance);
	characterInfo->thoughtBubble = (THOUGHTBUBBLE*)ZALLOC(sizeof(THOUGHTBUBBLE));
	characterInfo->costume = -1;
	if (characterInfo->thoughtBubble)
	{
		InitThoughtBubble(characterInfo->thoughtBubble);
		if (map.sceneNumber <= SCENE_ZOOBOSS)
		{
			i = 3.0f;
		}
		else
		{
			if (map.sceneNumber <= SCENE_CITYBOSS)
			{
				i = 4.0f;
			}
			else
			{
				if (map.sceneNumber <= SCENE_WESTBOSS)
				{
					i = 5.0f;
				}
				else
				{
					i = 6.0f;
				}
			}
		}
		InitCharactersHealth(characterInfo,i);
		characterInfo->cartoonScale.stage = CARTOONSCALE_STAGEONE;
		characterInfo->cartoonScale.time = 0.0f;
		characterInfo->cartoonScale.maxScale = 2.0f;
	}
	else
	{
		bkPrintf("*** WARNING *** Could not allocate memory for thought bubble\n");
	}
	characterInfo->voodooCloudInfo = NULL;

	if(createFlags & CHARACTERINFO_CREATESPIN)
	{
		characterInfo->spinInfo = CreateSpinEffectForActor(actorInstance->playerNo);
	}
	else characterInfo->spinInfo = NULL;

	characterInfo->superBurp = NULL;
	characterInfo->superBurpEffectInfo.blastRadiusInfo = NULL;
	characterInfo->superBurpEffectInfo.bubblesInfo = NULL;
	characterInfo->superBurpEffectInfo.bubblesClock = 0.0f;
	switch(gameStatus.globalSettings.difficulty)
	{
	case DIFF_EASY:
		characterInfo->superBurpEffectInfo.clock = SUPERBURP_EASY;
		break;
	case DIFF_MEDIUM:
		characterInfo->superBurpEffectInfo.clock = SUPERBURP_MEDIUM;
		break;
	case DIFF_HARD:
		characterInfo->superBurpEffectInfo.clock = SUPERBURP_HARD;
		break;
	}
	characterInfo->superBurpEffectInfo.soundHandle = -1;

	characterInfo->powerUpInfo.clock = 0.0f;
	characterInfo->powerUpInfo.randomClock = 0.0f;
	characterInfo->powerUpInfo.thirdClock = 0.0f;
	characterInfo->powerUpInfo.invisible = FALSE;
	characterInfo->powerUpInfo.hiccup = FALSE;
	characterInfo->powerUpInfo.bubbleGum = FALSE;
	characterInfo->powerUpInfo.chilliPepper = FALSE;
	characterInfo->powerUpInfo.state = -1;
	characterInfo->powerUpInfo.gumReady = FALSE;
	characterInfo->powerUpInfo.gumBubbleHandle = -1;
	characterInfo->powerUpInfo.gumBubble = NULL;
	characterInfo->powerUpInfo.hiccupJumpState = false;
	characterInfo->powerUpInfo.drawBubbles = FALSE;
	characterInfo->loseCostume = false;
	characterInfo->rumbleHandle = -1;
	characterInfo->tartanInfo = NULL;
	characterInfo->transportInfo = NULL;
	characterInfo->projecting.fixedPathEnable = false;

	characterInfo->blinkEyes = NULL;
	characterInfo->tazShieldInfo = NULL;

	characterInfo->eating = new EATING;

	characterInfo->inMouth = NULL;

	characterInfo->pTrail = NULL;// CPW: (TRAIL *)MALLOC(sizeof(TRAIL));

	SETVECTOR(characterInfo->portalGravity, 0.0f, 1.0f, 0.0f, 1.0f);
	characterInfo->rapperBlastInfo = NULL;
	if(map.sceneNumber == SCENE_DEPTSTR || map.sceneNumber == SCENE_DEPTSTRVEHICLERACE)
	{
		characterInfo->rapperBlastInfo = (RAPPERBLASTINFO*)ZALLOC(sizeof(RAPPERBLASTINFO));
		ASSERTM(characterInfo->rapperBlastInfo, "no memory for rapper attack effect, you're probably running on gamecube!");

		characterInfo->rapperBlastInfo->clock = 0.0f;
		characterInfo->rapperBlastInfo->draw = FALSE;
		characterInfo->rapperBlastInfo->speakers[0] = NULL;
		characterInfo->rapperBlastInfo->speakers[1] = NULL;
		characterInfo->rapperBlastInfo->speakers[2] = NULL;
		characterInfo->rapperBlastInfo->speakers[3] = NULL;
	}
}

/* --------------------------------------------------------------------------------
   Function : DestroyCharacterInfo
   Purpose : destroy character info structure
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void DestroyCharacterInfo(ACTORINSTANCE *actorInstance)
{
	CHARACTERINFO *characterInfo = actorInstance->characterInfo;

	bkHeapCheck();
	trailFree(actorInstance);

	DestroyAttachedInfo(actorInstance);
	if (characterInfo->thoughtBubble)
	{
		DestroyThoughtBubble(characterInfo->thoughtBubble);
	}
	SAFE_FREE(characterInfo->thoughtBubble);
	characterInfo->thoughtBubble = NULL;

	if (characterInfo->eating)
	{
		delete characterInfo->eating;
		characterInfo->eating = NULL;
	}

/*	baFreeActorInstance(&characterInfo->health.info.drumstickFull);
	baFreeActorInstance(&characterInfo->health.info.drumstickHalf);
	baFreeActorInstance(&characterInfo->health.info.drumstickEmpty);*/

	if (characterInfo->blinkEyes)
	{
		SAFE_DELETE(characterInfo->blinkEyes);
	}

	if (characterInfo->superBurp)
	{
		SAFE_DELETE(characterInfo->superBurp);
	}

	if (characterInfo->voodooCloudInfo)
	{
		baFreeActorInstance(&actorInstance->characterInfo->voodooCloudInfo->actorInstance);
		SAFE_FREE(actorInstance->characterInfo->voodooCloudInfo);
	}

	StopChannel(&characterInfo->superBurpEffectInfo.soundHandle);

	if (characterInfo->inMouth)
	{
		FreeDestructibleInfo(characterInfo->inMouth);
		SAFE_FREE(characterInfo->inMouth);
		characterInfo->inMouth = NULL;
	}

#ifndef CONSUMERDEMO
	if(characterInfo->tazShieldInfo)
	{
		characterInfo->tazShieldInfo = FreeTazShield(characterInfo->tazShieldInfo);
	}
#endif
	if(characterInfo->spinInfo)
	{
		SAFE_FREE(characterInfo->spinInfo);
	}
	if(characterInfo->rapperBlastInfo)
	{
		SAFE_FREE(characterInfo->rapperBlastInfo);
	}
	if(characterInfo->tartanInfo)
	{
		SAFE_FREE(characterInfo->tartanInfo);
	}
	if(characterInfo->transportInfo)
	{
		DeleteTriggerInfo(characterInfo->transportInfo->onCompletion);
		DeleteTriggerInfo(characterInfo->transportInfo->onReachingDestination);
		SAFE_FREE(characterInfo->transportInfo);
	}
}

/* --------------------------------------------------------------------------------
   Function : PrepCharacterInfo
   Purpose : prep all character info
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void PrepCharacterInfo(ACTORINSTANCE *actorInstance)
{
	if (actorInstance->characterInfo)
	{
#ifdef CHRISCAM
		if(actorInstance->characterInfo->pTrail)
			trailAddPosition(actorInstance);
#endif

		if (actorInstance->characterInfo->eating)
		{
			actorInstance->characterInfo->eating->Update(actorInstance);
		}

		PrepAttachedObjects(actorInstance);
		PrepCharactersHealth(actorInstance);
		if (actorInstance->characterInfo->thoughtBubble)
		{
			// TP: POSSIBLE PROBLEM
			PrepThoughtBubble(actorInstance->characterInfo->thoughtBubble, gameStatus.player[actorInstance->playerNo].camera, actorInstance->actorInstance.position);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : PrepInstancesCharacterInfo
   Purpose : prepare all character info in a instance list
   Parameters : CHARACTERINFO
   Returns : 
   Info : prepare things like thought bubble etc.
*/
void PrepInstancesCharacterInfo(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE *ptr;

	ptr = actorInstanceList->head.next;

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if (ptr->characterInfo)
		{
			PrepCharacterInfo(ptr);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawCharacterInfo
   Purpose : draw all character info
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void DrawCharacterInfo(ACTORINSTANCE *actorInstance)
{
	if (actorInstance->characterInfo)
	{
		DrawAttachedObjects(actorInstance);
		DrawCharactersHealth(actorInstance);
		if (actorInstance->characterInfo->thoughtBubble)
		{
			DrawThoughtBubble(actorInstance->characterInfo->thoughtBubble,gameStatus.player[actorInstance->playerNo].camera);
		}

		if (actorInstance->characterInfo->eating)
		{
			actorInstance->characterInfo->eating->Draw();
		}

		if (actorInstance->characterInfo->superBurp)
		{
			actorInstance->characterInfo->superBurp->Draw();
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawCharacterInfo
   Purpose : draw all character info
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void DrawCharacterInfo2(ACTORINSTANCE *actorInstance)
{
	if (actorInstance->characterInfo)
	{
		DrawAttachedObjects2(actorInstance);
	}
	if (actorInstance->genericAI)
	{
		if (actorInstance->genericAI->draw2) actorInstance->genericAI->draw2(actorInstance);
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawInstancesCharacterInfo
   Purpose : draw all character info in a instance list
   Parameters : CHARACTERINFO
   Returns : 
   Info : draws things like thought bubbles etc.
*/
void DrawInstancesCharacterInfo(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE *ptr;

	ptr = actorInstanceList->head.next;

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if (ptr->characterInfo)
		{
			// PP: in Taz-Cam mode, don't draw Taz's stuff
			if(CheatActive(CHEAT_TAZCAM))
			{
				if(ptr == gameStatus.player1)
				{
					continue;
				}
			}

			DrawCharacterInfo(ptr);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawInstancesCharacterInfo
   Purpose : draw all character info in a instance list
   Parameters : CHARACTERINFO
   Returns : 
   Info : draws things like thought bubbles etc.
*/
void DrawInstancesCharacterInfo2(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE *ptr;

	ptr = actorInstanceList->head.next;

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if (ptr->characterInfo)
		{
			// PP: in Taz-Cam mode, don't draw Taz's stuff
			if(CheatActive(CHEAT_TAZCAM))
			{
				if(ptr == gameStatus.player1)
				{
					continue;
				}
			}

			DrawCharacterInfo2(ptr);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateCharacterInfo
   Purpose : Update all character info
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void UpdateCharacterInfo(ACTORINSTANCE *actorInstance)
{
	if (actorInstance->characterInfo)
	{
		UpdateAttachedObjects(actorInstance);
		UpdateCharactersHealthDisplayGeometry(actorInstance);
	}
	if (actorInstance->characterInfo->superBurp)
	{
		if (actorInstance->characterInfo->superBurp->Update()==FALSE)
		{
			delete actorInstance->characterInfo->superBurp;
			actorInstance->characterInfo->superBurp = NULL;
		}
	}
	if(actorInstance->characterInfo->currentBurp)
	{
		if (actorInstance->characterInfo->superBurpEffectInfo.soundHandle==-1)
		{
			actorInstance->characterInfo->superBurpEffectInfo.soundHandle = PlayTrackingSample("burpcanbubbles.wav",255,actorInstance->actorInstance.position,BSPLAYFLAG_FORCELOOPING,0);
		}
		if((actorInstance->characterInfo->superBurpEffectInfo.bubblesClock += fTime) > 0.1f)
		{
			actorInstance->characterInfo->superBurpEffectInfo.bubblesClock = 0.0f;
			CreateBubbles(actorInstance, METERTOUNIT(0.1f), BURPBUBBLE_LIFE, GREEN_BUBBLE);
		}
		if((actorInstance->characterInfo->superBurpEffectInfo.clock -= fTime) < 0.0f)
		{
			FinishBurp(actorInstance);
		}
	}
	else
	{
		// TP: was trying to track down a stopchannel call, but this was calling it every frame 
		if (actorInstance->characterInfo->superBurpEffectInfo.soundHandle!=-1)
		{
			StopChannel(&actorInstance->characterInfo->superBurpEffectInfo.soundHandle);
		}
	}
	if(actorInstance->characterInfo->powerUpInfo.invisible)
	{
		UpdateCharacterInvisiblity(actorInstance);
	}
	if(actorInstance->characterInfo->powerUpInfo.hiccup)
	{
		if((actorInstance->characterInfo->superBurpEffectInfo.bubblesClock += fTime) > 0.1f)
		{
			actorInstance->characterInfo->superBurpEffectInfo.bubblesClock = 0.0f;
			CreateBubbles(actorInstance, METERTOUNIT(0.1f), HICCUPBUBBLE_LIFE, WHITE_BUBBLE);
		}
		UpdateActorHiccup(actorInstance);
	}
	if(actorInstance->characterInfo->powerUpInfo.bubbleGum)
	{
		UpdateBubbleGumPowerUpForCharacter(actorInstance);
	}
	if(actorInstance->characterInfo->powerUpInfo.chilliPepper)
	{
		CreateCharacterChilliPepperSmoke(actorInstance);
		UpdateCharacterChilliPepperPowerUp(actorInstance);
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateInstancesCharacterInfo
   Purpose : Update all character info in a instance list
   Parameters : CHARACTERINFO
   Returns : 
   Info : Updates things like thought bubbles etc.
*/
void UpdateInstancesCharacterInfo(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE *ptr;

	ptr = actorInstanceList->head.next;

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if (ptr->characterInfo)
		{
			UpdateCharacterInfo(ptr);
		}
	}
}
/* --------------------------------------------------------------------------------
   Function : CharacterRespondToSpinMovementInput
   Purpose : controls the movement of a character while spinning based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR, (opt/false) whether or not to lock character's Y angle during the spin
   Returns : 
   Info : Will modify a characters direction and speed
*/
void CharacterRespondToSpinMovementInput(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance, const bool lockYAng)
{
	float			angleOfMovement;
	float			xRot,zRot;
	TBVector		tempVector,clipVector;
	float			vel;
	float			size;

	angleOfMovement = ConvertFromCameraToGlobalRotation(cameraInfo,0.0f);

	if (controller->mag1)
	{
		xRot = controller->y1 * (PI/6.0f);
		zRot = controller->x1 * (PI/6.0f);
	}
	else
	{
		xRot = 0.0f;
		zRot = 0.0f;
	}

	//calculate target spin speed
	if(actorInstance->actorBehaviour->physics.speed < METERTOUNIT(0.5f))
	{
		if((actorInstance->actorStatus->targetSpinSpeed += SPIN_BUILDUP_ACCELERATION*fTime) > MAX_SPIN_SPEED(actorInstance))
		{
			actorInstance->actorStatus->targetSpinSpeed = actorInstance->actorBehaviour->physics.maxSpeed*2.0f;
		}
		size = METERTOUNIT(0.2f)*(actorInstance->actorStatus->targetSpinSpeed/(actorInstance->actorBehaviour->physics.maxSpeed*2.0f));
		CreateImpactClouds(actorInstance->actorInstance.position, gameStatus.windVelocity, NULL, size, 0.75f, 0.0f, SMOKE_SPIN);
	}
	else
	{
		actorInstance->actorStatus->targetSpinSpeed = 0.0f;
	}
	// calculate speed
	if (controller->mag1)
	{
		tempVector[X] = controller->x1;
		tempVector[Y] = 0.0f;
		tempVector[Z] = -controller->y1;
		tempVector[W] = 1.0f;

		// calculate direction vector
		bmVectorRotateY(tempVector,tempVector,angleOfMovement);

		// set acceleration speed controller mag * 3
		// only add speed if on ground
		if ((actorInstance->actorStatus->timeWithoutGroundCollision < 0.15f)||(actorInstance->actorStatus->timeWithoutWallCollision < 0.15f))
		{
			if(actorInstance->actorStatus->targetSpinSpeed)
			{
				vel = actorInstance->actorStatus->targetSpinSpeed;
			}
			
			if((!actorInstance->actorStatus->targetSpinSpeed) || (actorInstance->actorStatus->multiplayer.effect == PLAYEREFFECTSLOMO))
			{
				vel = controller->mag1*METERTOUNIT(0.4f)*speedRatio;
			}

			bmVectorNorm(tempVector,tempVector);
			bmVectorCMul(clipVector,tempVector,vel);


			bmVectorAdd(tempVector,clipVector,actorInstance->actorBehaviour->physics.velocity);
			vel = bmVectorLen(tempVector);
			if ((vel > actorInstance->actorBehaviour->physics.speed)&&(vel > actorInstance->actorBehaviour->physics.maxSpeed)&&(!actorInstance->actorStatus->targetSpinSpeed))
			{
				if (actorInstance->actorBehaviour->physics.speed > actorInstance->actorBehaviour->physics.maxSpeed)
				{
					bmVectorScaleToLength(actorInstance->actorBehaviour->physics.velocity,tempVector,actorInstance->actorBehaviour->physics.speed);
				}
				else
				{
					bmVectorScaleToLength(actorInstance->actorBehaviour->physics.velocity,tempVector,actorInstance->actorBehaviour->physics.maxSpeed);
				}
			}
			else
			{
				bmVectorCopy(actorInstance->actorBehaviour->physics.velocity,tempVector);
				actorInstance->actorBehaviour->physics.speed = vel;
			}
		}
	}

	if(!lockYAng)
	{
		// lean Taz in correct direction
		CharacterRotateToAngle(actorInstance,xRot,angleOfMovement,zRot,0.03f);
	}
}

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToSteeringControl
   Purpose : simple steering control
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR, (opt/false) whether or not to lock character's Y angle during the spin
   Returns : 
   Info : Will modify a characters direction and speed
*/
void CharacterRespondToSteeringControl(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance)
{
	float	yRot;

	if (controller->mag1)
	{
		yRot = controller->x1 * (PI/2.0f)/fps;
	}
	else
	{
		return;
	}

	yRot+=Aabs(-GetAngleAroundYFromQuat(actorInstance->actorInstance.orientation)+PI);

	// lean Taz in correct direction
	CharacterRotateToAngle(actorInstance,0.0f,yRot,0.0f,0.03f);
}

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToSlideMovementInput
   Purpose : controls the movement of a character while sliding based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/
void CharacterRespondToSlideMovementInput(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance)
{
	float	angleOfMovement;
	float	xRot,zRot;
	TBVector	tempVector,clipVector;
	float	vel;

	angleOfMovement = ConvertFromCameraToGlobalRotation(cameraInfo,0.0f);


	if (controller->mag1)
	{
		xRot = controller->y1 * (PI/6.0f);
		zRot = controller->x1 * (PI/6.0f);
	}
	else
	{
		xRot = 0.0f;
		zRot = 0.0f;
	}

	// calculate speed
	if (controller->mag1)
	{
		tempVector[X] = controller->x1;
		tempVector[Y] = 0.0f;
		tempVector[Z] = -controller->y1;
		tempVector[W] = 1.0f;

		// calculate direction vector
		bmVectorRotateY(tempVector,tempVector,angleOfMovement);

		// set acceleration speed controller mag * 3
		// only add speed if on ground
		if ((actorInstance->actorStatus->timeWithoutGroundCollision < 0.15f)||(actorInstance->actorStatus->timeWithoutWallCollision < 0.15f))
		{
			vel = controller->mag1*METERTOUNIT(3.0f)/fps;
			bmVectorNorm(tempVector,tempVector);
			bmVectorCMul(clipVector,tempVector,vel);

			bmVectorCopy(tempVector,actorInstance->actorBehaviour->physics.velocity);
			if(bmVectorLen(tempVector))
			{
				bmVectorNorm(tempVector,tempVector);
				ClipVector(clipVector,clipVector,tempVector);
			}

			bmVectorAdd(tempVector,clipVector,actorInstance->actorBehaviour->physics.velocity);
			vel = bmVectorLen(tempVector);
			if ((vel > actorInstance->actorBehaviour->physics.speed)&&(vel > actorInstance->actorBehaviour->physics.maxSpeed))
			{
				if (actorInstance->actorBehaviour->physics.speed > actorInstance->actorBehaviour->physics.maxSpeed)
				{
					bmVectorScaleToLength(actorInstance->actorBehaviour->physics.velocity,tempVector,actorInstance->actorBehaviour->physics.speed);
				}
				else
				{
					bmVectorScaleToLength(actorInstance->actorBehaviour->physics.velocity,tempVector,actorInstance->actorBehaviour->physics.maxSpeed);
				}
			}
			else
			{
				bmVectorCopy(actorInstance->actorBehaviour->physics.velocity,tempVector);
				actorInstance->actorBehaviour->physics.speed = vel;
			}
			
//			ClipVector(clipVector,tempVector,actorInstance->actorBehaviour->physics.lastAverageNormal);
/*			bmVectorCopy(clipVector,tempVector);

			vel = controller->mag1*METERTOUNIT(0.4f)*speedRatio;

			AddActorSpeed(actorInstance->actorBehaviour,clipVector,vel);*/
		}
	}

	// lean Taz in correct direction
	//CharacterRotateToAngle(actorInstance,xRot,angleOfMovement,zRot,0.03f);
}

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToSwingMovementInput
   Purpose : controls the movement of a character while swinging based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/
void CharacterRespondToSwingMovementInput(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance)
{
	float			angleOfMovement;
	float			xRot,zRot;
	TBVector		tempVector;
	float			vel;
	float			radius;
	float			controllerThreshold = 0.1f;
	float			xTemp, zTemp;

	angleOfMovement = ConvertFromCameraToGlobalRotation(cameraInfo,0.0f);

	xTemp = actorInstance->actorInstance.position[0] - actorInstance->characterInfo->swingingInfo.swingingOnObject->actorInstance.position[0];
	zTemp = actorInstance->actorInstance.position[2] - actorInstance->characterInfo->swingingInfo.swingingOnObject->actorInstance.position[2];
	radius = bmSqrt((xTemp*xTemp) + (zTemp*zTemp));

	actorInstance->characterInfo->swingingInfo.xySpeed = 0.0f;
	actorInstance->characterInfo->swingingInfo.zySpeed = 0.0f;

	//only accept input if actor in centre of swing
	if(((actorInstance->actorBehaviour->physics.velocity[1] < 0.0f)&&(radius <= (0.5*actorInstance->characterInfo->swingingInfo.swingingOnObject->swingingObjectInfo->length))) || (radius <= (0.25*actorInstance->characterInfo->swingingInfo.swingingOnObject->swingingObjectInfo->length)))
	{
		if (controller->mag1)
		{
			xRot = controller->y1 * (PI/6.0f);
			zRot = controller->x1 * (PI/6.0f);
		}
		else
		{
			xRot = 0.0f;
			zRot = 0.0f;
		}


		// calculate speed
		if (controller->mag1)
		{
			tempVector[X] = controller->x1;
			tempVector[Y] = 0.0f;
			tempVector[Z] = -controller->y1;
			tempVector[W] = 1.0f;
	
			// calculate direction vector
			bmVectorRotateY(tempVector,tempVector,angleOfMovement);
	
			// set acceleration speed controller mag * 3
				vel = controller->mag1*METERTOUNIT(70.0f)/fps;
				bmVectorNorm(tempVector,tempVector);
				actorInstance->characterInfo->swingingInfo.xySpeed = tempVector[0];
				actorInstance->characterInfo->swingingInfo.zySpeed = tempVector[2];
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToMovementInput
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/

void CharacterRespondToPlungerMovementInput(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance)
{
	CharacterRespondToSpinMovementInput(controller, cameraInfo, actorInstance);
}

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToMovementInput
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/

void CharacterRespondToMovementInput(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance)
{
	float	angleOfMovement;

	angleOfMovement = ConvertFromCameraToGlobalRotation(cameraInfo,controller->direction1);

	// base speed of rotation on mag of pad 0.2 being fastest

	float	maxTurnSpeed;

	// PP: turn more slowly in Taz-Cam mode
	if(CheatActive(CHEAT_TAZCAM))
	{
		maxTurnSpeed=0.03f;
	}
	else
	{
		maxTurnSpeed=0.2f;
	}

	CharacterRotateToAngle(actorInstance,0,angleOfMovement,0.0f,maxTurnSpeed*controller->mag1);
}

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToMovementInput
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/

void CharacterRespondToMouseMovementInput(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance)
{
	float	angleOfMovement;

	angleOfMovement = ConvertFromCameraToGlobalRotation(cameraInfo,controller->direction1);

	// base speed of rotation on mag of pad 0.2 being fastest
	CharacterRotateToAngle(actorInstance,0,angleOfMovement,0.0f,0.2f*controller->mag1);
}

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToMovementInput
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/

void CharacterRespondToGloverBallMovementInput(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance)
{
	float			angleOfMovement;
	float			xRot,zRot;
	TBVector		tempVector,clipVector;
	float			vel, maxVel;
	float			size;

	angleOfMovement = ConvertFromCameraToGlobalRotation(cameraInfo,0.0f);

	if (controller->mag1)
	{
		xRot = controller->y1 * (PI/6.0f);
		zRot = controller->x1 * (PI/6.0f);
	}
	else
	{
		xRot = 0.0f;
		zRot = 0.0f;
	}

	// calculate speed
//	if (controller->mag1)
	{
		tempVector[X] = controller->x1;
		tempVector[Y] = 0.0f;
		tempVector[Z] = -controller->y1;
		tempVector[W] = 1.0f;

		// calculate direction vector
		bmVectorRotateY(tempVector,tempVector,angleOfMovement);

		// set acceleration speed controller mag * 3
		// only add speed if on ground
//		if ((actorInstance->actorStatus->timeWithoutGroundCollision < 0.15f)||(actorInstance->actorStatus->timeWithoutWallCollision < 0.15f))
		{
			vel = controller->mag1*METERTOUNIT(10.0f)*fTime;
			bmVectorNorm(tempVector,tempVector);
			bmVectorCMul(clipVector,tempVector,vel);


			maxVel = METERTOUNIT(10.0f);
			bmVectorAdd(tempVector,clipVector,actorInstance->actorBehaviour->physics.velocity);
			vel = bmVectorLen(tempVector);
			if((vel > actorInstance->actorBehaviour->physics.speed)&&(vel > maxVel))
			{
				if(actorInstance->actorBehaviour->physics.speed > maxVel)
				{
					bmVectorScaleToLength(actorInstance->actorBehaviour->physics.velocity,tempVector,actorInstance->actorBehaviour->physics.speed);
				}
				else
				{
					bmVectorScaleToLength(actorInstance->actorBehaviour->physics.velocity,tempVector,maxVel);
				}
			}
			else
			{
				bmVectorCopy(actorInstance->actorBehaviour->physics.velocity,tempVector);
				actorInstance->actorBehaviour->physics.speed = vel;
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToMovementInput
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/

void CharacterRespondToCrateMovementInput(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance)
{
	float	angleOfMovement;

	angleOfMovement = ConvertFromCameraToGlobalRotation(cameraInfo,controller->direction1);

	// base speed of rotation on mag of pad 0.2 being fastest
	CharacterRotateToAngle(actorInstance,0,angleOfMovement,0.0f,0.1f*controller->mag1);
}

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToMesmerisedMovementInput
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/

void CharacterRespondToMesmerisedMovementInput(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance)
{
	float	angleOfMovement;

	angleOfMovement = ConvertFromCameraToGlobalRotation(cameraInfo,controller->direction1);
	bkPrintf("direction: %f\n", controller->direction1);

	// CMD: speed of rotation is always SLOW
	CharacterRotateToAngle(actorInstance,0,angleOfMovement,0.0f,0.02f);
}

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToMovementInput
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/

void CharacterRespondToPortalMovementInput(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance)
{
	TBVector		rotation;
	TBQuaternion	inputQuat, planetQuat, tempQuat, result;

	SETVECTOR(rotation, 0.0f, 1.0f, 0.0f, controller->direction1);
	bmRotationToQuat(inputQuat, rotation);

	bmDirectionToQuat(planetQuat, actorInstance->characterInfo->portalGravity, 0.0f);
	bmQuatXRotation(tempQuat, RAD(-90.0f));

	bmQuatMultiply(result, inputQuat, tempQuat);
	bmQuatMultiply(inputQuat, planetQuat, result);

	ActorRotate(actorInstance, inputQuat, 0.2f*controller->mag1);
}

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToDebugMovementInput
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/

void CharacterRespondToDebugMovementInput(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance)
{
	TBVector	tempVector;
	float	angleOfMovement;

	angleOfMovement = ConvertFromCameraToGlobalRotation(cameraInfo,controller->direction1);

	// base speed of rotation on mag of pad 0.2 being fastest
	CharacterRotateToAngle(actorInstance,0,angleOfMovement,0.0f,0.2f*controller->mag1);

	if(controller->triangleChannel->value)
	{
		SETVECTOR(tempVector, 0.0f, METERTOUNIT(10.0f), 0.0f, 1.0f);
	}
	else
	if(controller->crossChannel->value)
	{
		SETVECTOR(tempVector, 0.0f, METERTOUNIT(-10.0f), 0.0f, 1.0f);
	}
	else
	{
		SETVECTOR(tempVector, 0.0f, 0.0f, 0.0f, 1.0f);
	}

	bmVectorCMul(tempVector, tempVector, fTime);
	bmVectorAdd(actorInstance->actorInstance.position, actorInstance->actorInstance.position, tempVector);
}

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToWaterSlideMovementInput
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/

void CharacterRespondToWaterSlideMovementInput(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance)
{
	if (camera[0].cameraCase)
		return;

	float	angleOfMovement;
	float	xRot,zRot;
	TBVector	tempVector,clipVector, rotVec;
	float	vel;
	float	accel;

	angleOfMovement = ConvertFromCameraToGlobalRotation(cameraInfo,0.0f);

	tempVector[X] = controller->x1;
	tempVector[Y] = 0.0f;
	tempVector[Z] = 1.0f;
	tempVector[W] = 1.0f;

	// calculate direction vector
	//bmVectorRotateY(tempVector,tempVector,angleOfMovement);
	
	// set acceleration speed controller mag * 3
	// only add speed if on ground
	if ((actorInstance->actorStatus->timeWithoutGroundCollision < 0.15f)||(actorInstance->actorStatus->timeWithoutWallCollision < 0.15f))
	{
		tempVector[Z] -= controller->y1;//*fTime;	//(METERTOUNIT(10.0f))*fTime;
		
		bmVectorNorm(tempVector,tempVector);
		bmVectorCMul(clipVector,tempVector,(METERTOUNIT(20.0f))*fTime);
		
		bmVectorRotateY(clipVector,clipVector,angleOfMovement);
		if(actorInstance->actorInstance.position[Y] > actorInstance->actorStatus->positionLastFrame[Y])
		{
			actorInstance->actorStatus->timeGoingUpWaterSlide += fTime;
		}
		else
		{
			gameStatus.player[actorInstance->playerNo].camera->flags |= CAMERA_PADCONTROL;
			SetCameraSmooth(gameStatus.player[actorInstance->playerNo].camera, defaultPosSmooth, defaultLookatSmooth);
			actorInstance->actorStatus->timeGoingUpWaterSlide = 0.0f;
		}

#define WATERSLIDE_UPSLOPE_LAGTIME	(1.0f)

		if(actorInstance->actorStatus->timeGoingUpWaterSlide > WATERSLIDE_UPSLOPE_LAGTIME)
		{
			// CMD: turn camera off, and rotate arround taz
			bmVectorAdd(rotVec, actorInstance->actorInstance.position, actorInstance->actorBehaviour->physics.velocity);
			rotVec[1] += METERTOUNIT(0.2f);
			SetCameraSmooth(gameStatus.player[actorInstance->playerNo].camera, 0.25f, defaultLookatSmooth);
			SetCameraPositionDestination(gameStatus.player[actorInstance->playerNo].camera, rotVec);
		}

		bmVectorAdd(tempVector,clipVector,actorInstance->actorBehaviour->physics.velocity);
	
		bmVectorRotateY(tempVector,tempVector,-angleOfMovement);

		if (tempVector[Z] < METERTOUNIT(2.0f))
			tempVector[Z] = METERTOUNIT(2.0f);

		bmVectorRotateY(tempVector,tempVector, angleOfMovement);

		vel = bmVectorLen(tempVector);
	
		if ((vel > actorInstance->actorBehaviour->physics.speed)&&(vel > actorInstance->actorBehaviour->physics.maxSpeed))
		{
			if (actorInstance->actorBehaviour->physics.speed > actorInstance->actorBehaviour->physics.maxSpeed)
			{
				bmVectorScaleToLength(actorInstance->actorBehaviour->physics.velocity,tempVector,actorInstance->actorBehaviour->physics.speed);
			}
			else
			{
				bmVectorScaleToLength(actorInstance->actorBehaviour->physics.velocity,tempVector,actorInstance->actorBehaviour->physics.maxSpeed);
			}
		}
		else
		{
			bmVectorCopy(actorInstance->actorBehaviour->physics.velocity,tempVector);
			actorInstance->actorBehaviour->physics.speed = vel;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToMovementInputOnIce
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/
void CharacterRespondToMovementInputOnIce(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance)
{
	float	angleOfMovement;
	float	xRot,zRot;
	TBVector	tempVector,inputVelocity;
	TBVector	newVelocity;

	angleOfMovement = ConvertFromCameraToGlobalRotation(cameraInfo,0);


	if (controller->mag1)
	{
		xRot = controller->y1 * (PI/6.0f);
		zRot = controller->x1 * (PI/6.0f);
	}
	else
	{
		xRot = 0.0f;
		zRot = 0.0f;
	}

	// calculate speed
	//if (controller->mag1)
	{
		tempVector[X] = controller->x1;
		tempVector[Y] = 0.0f;
		tempVector[Z] = -controller->y1;
		tempVector[W] = 1.0f;

		// calculate direction vector
		bmVectorRotateY(tempVector,tempVector,angleOfMovement);
		bmVectorCMul(inputVelocity, tempVector, (METERTOUNIT(6.0f)/fps));

		// set acceleration speed controller mag * 3
		// only add speed if on ground
		if ((actorInstance->actorStatus->timeWithoutGroundCollision < 0.15f)||(actorInstance->actorStatus->timeWithoutWallCollision < 0.15f))
		{
			bmVectorAdd(newVelocity, actorInstance->actorBehaviour->physics.velocity, inputVelocity);
			if(bmVectorLen(newVelocity) > METERTOUNIT(10.0f))
			{
				bmVectorScaleToLength(actorInstance->actorBehaviour->physics.velocity, newVelocity, METERTOUNIT(10.0f));
			}
			else
			{
				bmVectorCopy(actorInstance->actorBehaviour->physics.velocity, newVelocity);
			}
		}
	}

	angleOfMovement = ConvertFromCameraToGlobalRotation(cameraInfo,controller->direction1);
	// lean Taz in correct direction
	CharacterRotateToAngle(actorInstance,0,angleOfMovement,0.0f,0.2f*controller->mag1);
}

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToMovementInputInSphere
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/
void CharacterRespondToMovementInputInSphere(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance)
{
	float	angleOfMovement;
	float	xRot,zRot,vel;
	TBVector	tempVector,inputVelocity;
	TBVector	newVelocity;

	angleOfMovement = ConvertFromCameraToGlobalRotation(cameraInfo,0);


	if (controller->mag1)
	{
		xRot = controller->y1 * (PI/6.0f);
		zRot = controller->x1 * (PI/6.0f);
	}
	else
	{
		xRot = 0.0f;
		zRot = 0.0f;
	}

	// calculate speed
	//if (controller->mag1)
	{
		tempVector[X] = controller->x1;
		tempVector[Y] = 0.0f;
		tempVector[Z] = -controller->y1;
		tempVector[W] = 1.0f;

		// calculate direction vector
		bmVectorRotateY(tempVector,tempVector,angleOfMovement);
		bmVectorCMul(inputVelocity, tempVector, (METERTOUNIT(6.0f)/fps));

		// set acceleration speed controller mag * 3
		// only add speed if on ground
		if ((actorInstance->actorStatus->timeWithoutGroundCollision < 0.15f)||(actorInstance->actorStatus->timeWithoutWallCollision < 0.15f))
		{
			bmVectorAdd(newVelocity, actorInstance->actorBehaviour->physics.velocity, inputVelocity);
			if((vel = bmVectorLen(newVelocity)) > METERTOUNIT(25.0f))
			{
				bmVectorScaleToLength(actorInstance->actorBehaviour->physics.velocity, newVelocity, METERTOUNIT(25.0f));
				vel = METERTOUNIT(25.0f);
			}
			else
			{
				bmVectorCopy(actorInstance->actorBehaviour->physics.velocity, newVelocity);
			}
			actorInstance->actorBehaviour->physics.speed = vel;
		}
	}

	angleOfMovement = ConvertFromCameraToGlobalRotation(cameraInfo,controller->direction1);
	// lean Taz in correct direction
	CharacterRotateToAngle(actorInstance,0,angleOfMovement,0.0f,0.2f*controller->mag1);
}

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToMovementWhileJumping
   Purpose : controls the movement of a character while jumping based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTORINSTANCE, (opt/false) lock Y angle during jump
   Returns : 
   Info : Will modify a characters direction and speed
*/

void CharacterRespondToMovementWhileJumping(PADCONTROLSTATUS *controller,
											CAMERAINFO *cameraInfo,
											ACTORINSTANCE *actorInstance,
											const bool lockYAng)
{
	float	angleOfMovement;
	TBVector tvec;
	float	maxSpeed;
	float	xRot,tempFloat;

	// angle the player wants the character to move in
	angleOfMovement = ConvertFromCameraToGlobalRotation(cameraInfo,controller->direction1);

	// character should only move in direction currently facing
//	angleOfCharacter = Aabs(-GetAngleAroundYFromQuat(actorInstance->actorInstance.orientation)+PI);

	// create vector from angle of character
	tvec[0] = bmSin(angleOfMovement);
	tvec[1] = 0.0f;
	tvec[2] = bmCos(angleOfMovement);
	tvec[3] = 0.0f;

	// maxSpeed is the speed the character was moving at before the jump started
 	maxSpeed = actorInstance->actorStatus->speedBeforeJump;

	if (actorInstance->actorStatus->multiplayer.effect == PLAYEREFFECTSLOMO)
	{
		maxSpeed /= 2;

		if (maxSpeed<METERTOUNIT(1.8f)) maxSpeed = METERTOUNIT(1.8f);	// make sure player can move a bit even if they wern't moving when
									// they pressed jump
		if (maxSpeed>METERTOUNIT(3.0f)) maxSpeed = METERTOUNIT(3.0f);
	}
	else
	{
		if (maxSpeed<METERTOUNIT(3.6f)) maxSpeed = METERTOUNIT(3.6f);	// make sure player can move a bit even if they wern't moving when
									// they pressed jump
		if (maxSpeed>METERTOUNIT(6.0f)) maxSpeed = METERTOUNIT(6.0f);
	}

	if(!lockYAng)// PP: if the character's Y angle isn't locked for the duration of the jump
	{
		if (controller->mag1)
		{
			if (CheckIfPlayingAnimation(actorInstance,"jumpfall"))
			{
				// rotate character to face in correct direction
				// tilt character in x axis if doing a fall animation
				xRot = controller->mag1* (maxSpeed/(METERTOUNIT(6.0f))) *PI/8.0f;
				CharacterRotateToAngle(actorInstance,xRot,angleOfMovement,0.0f,0.2f);
			}
			else
			{
				// rotate character to face in correct direction
				CharacterRotateToAngle(actorInstance,0,angleOfMovement,0,0.2f);
			}
		}
	}

	tempFloat = actorInstance->actorBehaviour->physics.velocity[Y];
	bmVectorScaleToLength(tvec,tvec,controller->mag1*maxSpeed);
	tvec[Y] = tempFloat;
	SetActorVelocity( actorInstance->actorBehaviour, tvec);
	actorInstance->actorBehaviour->physics.velocity[Y] = tempFloat;

//	tempFloat = actorInstance->actorBehaviour->physics.velocity[Y];
//	SetActorSpeed( actorInstance->actorBehaviour, tvec, controller->mag1*maxSpeed);
//	actorInstance->actorBehaviour->physics.velocity[Y] = tempFloat;
}

/* --------------------------------------------------------------------------------
   Function : CharacterRotateToAngle
   Purpose : Rotate a character to a given angle taking into account turn speeds
   Parameters : ACTORINSTANCE *actorInstance, float angle in rads to face - clockwise (0 Rads is +Z)
   Returns : 
   Info : Will modify a characters direction
*/
void CharacterRotateToAngle(ACTORINSTANCE *actorInstance,float xAngle,float yAngle,float zAngle, float speed)
{
	TBVector	temp;
	TBQuaternion quat,xquat,zquat,tempQuat;

	// do y rot first and allways
	temp[X] = 0.0f;
	temp[Y] = 1.0f;
	temp[Z] = 0.0f;
	temp[A] = Aabs(-(yAngle+PI)); // convert to anitclockwise and add PI because characters face in -Z
	bmRotationToQuat(quat,temp);

	if (xAngle)
	{
		temp[X] = 1.0f;
		temp[Y] = 0.0f;
		temp[Z] = 0.0f;
		bmVectorRotateY(temp,temp,yAngle);
		temp[A] = xAngle;
		bmRotationToQuat(xquat,temp);
		bmQuatMultiply(tempQuat,quat,xquat);
		bmQuatCopy(quat,tempQuat);
	}

	if (zAngle)
	{
		temp[X] = 0.0f;
		temp[Y] = 0.0f;
		temp[Z] = 1.0f;
		bmVectorRotateY(temp,temp,yAngle);
		temp[A] = zAngle;
		bmRotationToQuat(zquat,temp);
		bmQuatMultiply(tempQuat,quat,zquat);
		bmQuatCopy(quat,tempQuat);
	}

	ActorRotate( actorInstance, quat,  speed);
}

/* --------------------------------------------------------------------------------
   Function : ChooseMovementAnimationForTaz
   Purpose : Selects the appropriate animation based on the current pad status
   Parameters : PADCONTROLSTATUS, ACTORINSTANCE
   Returns : the animation segment to play
   Info : 
*/

TBActorAnimSegment *ChooseMovementAnimationForTaz(PADCONTROLSTATUS *controller, ACTORINSTANCE *actorInstance)
{
	// PP: **** THERE HAS GOT TO BE A BETTER WAY OF DOING THIS! PLEASE TELL ME! ****
/*	if(CheatActive(CHEAT_DIGITALWATCH)&&watchLook)
	{
		return baFindAnimSegment(&actorInstance->actorInstance, "idle1");
	}*/

	if (controller->l2Channel->value)
	{
		if (controller->mag1 > 0.2f)
		{
			// tiptoe
			return baFindAnimSegment(&actorInstance->actorInstance, "tiptoe");
		}
		else
		{
			return baFindAnimSegment(&actorInstance->actorInstance, "idle1");
		}
	}
	if (controller->mag1 > 0.6f)//controller->dPadXChannel->value || controller->dPadYChannel->value || 
	{
		// run
		return baFindAnimSegment(&actorInstance->actorInstance, "run");
	}
	if (controller->mag1 > 0.2f)
	{
		// walk
		return baFindAnimSegment(&actorInstance->actorInstance, "walk1");
	}

	// no movement so do breathe
	return baFindAnimSegment(&actorInstance->actorInstance, "idle1");
//	return ChooseIdleAnimationForTaz(controller, actorInstance);
}

/* --------------------------------------------------------------------------------
   Function : ChooseMovementAnimationForTazInSphere
   Purpose : Selects the appropriate animation based on the current pad status
   Parameters : PADCONTROLSTATUS, ACTORINSTANCE
   Returns : the animation segment to play
   Info : 
*/

TBActorAnimSegment *ChooseMovementAnimationForTazInSphere(PADCONTROLSTATUS *controller, ACTORINSTANCE *actorInstance)
{
	if(controller->mag1 > 0.2f)
	{
		return baFindAnimSegment(&actorInstance->actorInstance, "runsphere");
	}

	return baFindAnimSegment(&actorInstance->actorInstance, "spherestatic");
}


/* --------------------------------------------------------------------------------
   Function : ChooseIdleAnimationForTaz
   Purpose : Selects the appropriate animation based on the current pad status
   Parameters : PADCONTROLSTATUS, ACTORINSTANCE
   Returns : the animation segment to play
   Info : 
*/

TBActorAnimSegment *ChooseIdleAnimationForTaz(PADCONTROLSTATUS *controller, ACTORINSTANCE *actorInstance, CAMERAINFO *cameraInfo)
{
	TBActorAnimSegment	*animation;
	int					nextIdle;
	
	animation = NULL;

	// PP: if CHEAT_INSOMNIA is active, Taz can't sleep.  Fair's fair I say!
	// PP: NOTE: this code is duplicated in control.cpp, for STATE_DANCE.
	if(!CheatActive(CHEAT_INSOMNIA))
	{
		if((controller->idleTime > 120.0f)&&(actorInstance->actorAnimation->timeSinceLastIdleMove > 5.0f))
		{
			animation = baFindAnimSegment(&actorInstance->actorInstance, "gotosleep");
	//		FlushAnimationQueue(actorInstance);
	//		PlayAnimationByName(actorInstance, "gotosleep", 1.0f, 0, 0, 0.1f, DEATH);
			if(characterSounds.taz == -1)
			{
				CallbackWhenSampleFinished((characterSounds.taz=PlaySample("taz_voc_50b.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.taz);
			}
			ChangeState(actorInstance, STATE_GOTOSLEEP);
			// TP: actorInstance->actorStatus->lastState = actorInstance->actorStatus->currentState;
			// TP: actorInstance->actorStatus->currentState = STATE_GOTOSLEEP;
			//----------------STATE CHANGE---------------
			return animation;
		}
	}

	if((controller->idleTime > 10.0f)&&(actorInstance->actorAnimation->timeSinceLastIdleMove > 5.0f))
	{
		//choose idle number
		do
		{
			nextIdle = bmRand()%12;
		} while(actorInstance->actorAnimation->lastIdle == nextIdle);
		//update last idle marker
		actorInstance->actorAnimation->lastIdle = nextIdle;

		actorInstance->actorAnimation->timeSinceLastIdleMove = 0.0f;

		// TP: Turn Taz to face the camera
		FacePoint(actorInstance,cameraInfo->pos,0.2f);

		// CMD: no need to play idle1, it plays by default
		switch(nextIdle)
		{
		case 0:
			animation = baFindAnimSegment(&actorInstance->actorInstance, "agitated");
			break;
		case 1:
		case 2:
		case 3:
		case 4:
			// TP: Animation idle3 missing for some reason
			animation = baFindAnimSegment(&actorInstance->actorInstance, "idle8");
			break;
		case 5:
			animation = baFindAnimSegment(&actorInstance->actorInstance, "idle4");
			break;
		case 6:
			animation = baFindAnimSegment(&actorInstance->actorInstance, "idle5");
			break;
		case 7:
			animation = baFindAnimSegment(&actorInstance->actorInstance, "idle6");
			break;
		case 8:
			animation = baFindAnimSegment(&actorInstance->actorInstance, "idle7");
			break;
		case 9:
			animation = baFindAnimSegment(&actorInstance->actorInstance, "angryidle1");
			break;
		case 10:
			animation = baFindAnimSegment(&actorInstance->actorInstance, "angryidle2");
			break;
		case 11:
			switch(bmRand()%8)
			{
			case 0:
				animation = baFindAnimSegment(&actorInstance->actorInstance, "boogie1");
				break;
			case 1:
				animation = baFindAnimSegment(&actorInstance->actorInstance, "boogie2");
				break;
			case 2:
				animation = baFindAnimSegment(&actorInstance->actorInstance, "boogie3");
				break;
			case 3:
				animation = baFindAnimSegment(&actorInstance->actorInstance, "boogie4");
				break;
			case 4:
				animation = baFindAnimSegment(&actorInstance->actorInstance, "boogie5");
				break;
			case 5:
				animation = baFindAnimSegment(&actorInstance->actorInstance, "boogie6");
				break;
			case 6:
				animation = baFindAnimSegment(&actorInstance->actorInstance, "boogie7");
				break;
			case 7:
				animation = baFindAnimSegment(&actorInstance->actorInstance, "boogie8");
				break;
			}
			if(animation)
			{
				ChangeState(actorInstance, STATE_DANCE);
				// TP: actorInstance->actorStatus->lastState = actorInstance->actorStatus->currentState;
				// TP: actorInstance->actorStatus->currentState = STATE_DANCE;
				//----------------STATE CHANGE---------------
			}
		}
		if (animation==NULL) bkPrintf("Could not find idle anim %d\n",nextIdle);
	}
	
	return animation;
}

/* --------------------------------------------------------------------------------
   Function : ChooseIdleAnimationForTaz
   Purpose : Selects the appropriate animation based on the current pad status
   Parameters : PADCONTROLSTATUS, ACTORINSTANCE
   Returns : the animation segment to play
   Info : 
*/

TBActorAnimSegment *ChooseIdleAnimationForCredits(ACTORINSTANCE *actorInstance)
{
	TBActorAnimSegment	*animation;
	int					nextIdle;
	
	animation = NULL;

	//choose idle number
	do
	{
		nextIdle = bmRand()%12;
	} while(actorInstance->actorAnimation->lastIdle == nextIdle);
	//update last idle marker
	actorInstance->actorAnimation->lastIdle = nextIdle;

	// CMD: no need to play idle1, it plays by default
	switch(nextIdle)
	{
	case 0:
		animation = baFindAnimSegment(&actorInstance->actorInstance, "agitated");
		break;
	case 1:
	case 2:
	case 3:
	case 4:
		// TP: Animation idle3 missing for some reason
		animation = baFindAnimSegment(&actorInstance->actorInstance, "idle8");
		break;
	case 5:
		animation = baFindAnimSegment(&actorInstance->actorInstance, "idle4");
		break;
	case 6:
		animation = baFindAnimSegment(&actorInstance->actorInstance, "idle5");
		break;
	case 7:
		animation = baFindAnimSegment(&actorInstance->actorInstance, "idle6");
		break;
	case 8:
		animation = baFindAnimSegment(&actorInstance->actorInstance, "idle7");
		break;
	case 9:
		animation = baFindAnimSegment(&actorInstance->actorInstance, "angryidle1");
		break;
	case 10:
		animation = baFindAnimSegment(&actorInstance->actorInstance, "angryidle2");
		break;
	case 11:
		switch(bmRand()%8)
		{
		case 0:
			animation = baFindAnimSegment(&actorInstance->actorInstance, "boogie1");
			break;
		case 1:
			animation = baFindAnimSegment(&actorInstance->actorInstance, "boogie2");
			break;
		case 2:
			animation = baFindAnimSegment(&actorInstance->actorInstance, "boogie3");
			break;
		case 3:
			animation = baFindAnimSegment(&actorInstance->actorInstance, "boogie4");
			break;
		case 4:
			animation = baFindAnimSegment(&actorInstance->actorInstance, "boogie5");
			break;
		case 5:
			animation = baFindAnimSegment(&actorInstance->actorInstance, "boogie6");
			break;
		case 6:
			animation = baFindAnimSegment(&actorInstance->actorInstance, "boogie7");
			break;
		case 7:
			animation = baFindAnimSegment(&actorInstance->actorInstance, "boogie8");
			break;
		}
	}
	
	return animation;
}

/* --------------------------------------------------------------------------------
   Function : ResetCharacterAfterJump
   Purpose : called when a character lands after a jump move, resets the character
   Parameters : ACTORINSTANCE *actorInstance
   Returns : 
   Info : 
*/
void ResetCharacterAfterJump(ACTORINSTANCE *actorInstance)
{
	TBVector	tempVector;
	TBQuaternion tempQuat;
	float	angle;

	angle = GetAngleAroundYFromQuat(actorInstance->actorInstance.orientation);
	tempVector[X] = 0;
	tempVector[Y] = 1;
	tempVector[Z] = 0;
	tempVector[A] = angle;
	bmRotationToQuat(tempQuat,tempVector);

	ActorRotate( actorInstance, tempQuat,  0.0f);
}

/* --------------------------------------------------------------------------------
   Function : TazBite
   Purpose : called each frame while Taz is performing a bite move.
   Parameters : ACTORINSTANCE *actorInstance
   Returns : 
   Info : 
*/
void TazBite(ACTORINSTANCE *actorInstance)
{
	TBVector	tempVector,noseVector;
	ACTORINSTANCE *ptr;
	TBActorNodeInstance *node;

	// test against character list
	ptr = map.characterInstances.head.next;

	baPrepActorInstance(&actorInstance->actorInstance,TRUE);
	node = baFindNode(actorInstance->actorInstance.rootNodeInstance,"hairstem");
	if (!node) return;
	baGetNodesWorldPosition(&actorInstance->actorInstance, node, noseVector);

	for (ptr = map.characterInstances.head.next;ptr != &map.characterInstances.head;ptr = ptr->next)
	{
		if(ptr->actorBehaviour)				// CMD: she devil in the boss game continue screen is on the characters list but has no actorBehaviour struct
		{
			bmVectorAdd(tempVector,ptr->actorInstance.position,ptr->actorBehaviour->collSphere.offset);
	
			bmVectorSub(tempVector,noseVector,tempVector);
			if (bmVectorLen(tempVector) < (ptr->actorBehaviour->collCylinder[0].radius+METERTOUNIT(0.5f)))
			{
				// taz has bitten character
				AICharacterBitten(ptr,actorInstance);
	
				// TP: rotate character to face
				FacePoint(actorInstance,ptr->actorInstance.position,0.1f,0.0f); 
			}
		}
	}
	// test against objects
}

/* --------------------------------------------------------------------------------
   Function		: TazAttack
   Purpose		: called each frame while Taz is performing an attack move
   Parameters	: ACTORINSTANCE* to Taz, attack node name, (opt/NULL=none) pos to use in the absence of a node name
   Returns		: // PP: true if the attack has connected with someone/something, else false
   Info			: 
*/
bool TazAttack(ACTORINSTANCE *actorInstance, char *nodeName, TBVector pos)
{
	TBVector				tempVector,noseVector;
	ACTORINSTANCE			*ptr;
	TBActorNodeInstance		*node;
	bool					rtn=false;					// PP: return value indicating contact with an enemy or that

	// test against character list
	ptr = map.characterInstances.head.next;

	if (nodeName)
	{
		baPrepActorInstance(&actorInstance->actorInstance,TRUE);
		node = baFindNode(actorInstance->actorInstance.rootNodeInstance,nodeName);
		if (!node) return false;
		baGetNodesWorldPosition(&actorInstance->actorInstance, node, noseVector);
	}
	else
	{
		bmVectorCopy(noseVector,pos);
	}

	for (ptr = map.characterInstances.head.next;ptr != &map.characterInstances.head;ptr = ptr->next)
	{
		bmVectorAdd(tempVector,ptr->actorInstance.position,ptr->actorBehaviour->collSphere.offset);

		bmVectorSub(tempVector,noseVector,tempVector);
		if (bmVectorLen(tempVector) < (ptr->actorBehaviour->collCylinder[0].radius+METERTOUNIT(0.5f)))
		{
			// PP: CONTACT!
			// PP: REMOUT: TEST		rtn=true;

			// taz has bitten character
			if (bmFAbs(ptr->genericAI->facingRelToTaz) < (PI/2))
			{
				rtn=(rtn || AICharacterSpun(ptr,actorInstance,ATTACKTYPE_COSTUMEFRONT));
			}
			else
			{
				rtn=(rtn || AICharacterSpun(ptr,actorInstance,ATTACKTYPE_COSTUMEREAR));
			}
		}
	}
	// test against objects

	// PP: return value indicates contact
	// PP: NOTE: actually, it now indicates "successful" contact.  But I don't fancy compiling for 20 minutes for the sake of that one little comment in the header, see?
	return rtn;
}


/*  --------------------------------------------------------------------------------
	Function	: TazSpit
	Purpose		: Get taz to spit out whats in mouth
	Parameters	: character, optional item, optional position, optional velocity
	Returns		: 
	Info		: 
*/
DEBRIS *TazSpit(ACTORINSTANCE *actorInstance, int32 item, TBVector iposition, TBVector ivelocity, float time, float size)
{
	TBVector				velocity,rotationSpeed,spriteVelocity,pos;
	TBMatrix				matrix;
	int						flag,number;
	TBTexture				*debrisTexture;
	TBActorNodeInstance		*node;
	CAMERAINFO				*camera;
	DESTRUCTIBLEOBJECTINFO	*destInfo;
	DESTRUCTIBLEDEBRISINFO	*debris;

	if (!item)
	{
		destInfo = actorInstance->characterInfo->inMouth;
		debris = actorInstance->characterInfo->inMouth->debris;
	}

	if (actorInstance->playerNo!=-1)
	{
		camera = gameStatus.player[actorInstance->playerNo].camera;
	}
	else
	{
		camera = NULL;
	}
	
	if ((!item)&&(!debris))
	{
		FreeDestructibleInfo(destInfo);
		SAFE_FREE(destInfo);
		actorInstance->characterInfo->inMouth = NULL;
		return NULL;
	}

	if ((camera)&&(camera->flags & CAMERA_BLINKEYES))
	{
		// TP: if in lookaround mode then aim in direction of camera
		bmMatYXZRotation(matrix,camera->cameraState.cameraAngles[X],camera->cameraState.cameraAngles[Y],camera->cameraState.cameraAngles[Z]);
		bmVectorCopy(pos,camera->cameraState.cameraPos);
		SETVECTOR(velocity, 0.0f,0.2f,1.0f,1.0f);
	}
	else
	{
		node = baFindNode(actorInstance->actorInstance.rootNodeInstance,"snoutstem");
		if (node)
		{
			baGetNodesWorldPosition(&actorInstance->actorInstance,node,pos);
			pos[Y] -= METERTOUNIT(0.15f);
		}
		bmQuatToMatrix(matrix,actorInstance->actorInstance.orientation);
		SETVECTOR(velocity, 0.0f,0.2f,-1.0f,1.0f);
	}

	if (iposition) bmVectorCopy(pos,iposition);

	rotationSpeed[X] = (bmRand()%3)*PI;
	rotationSpeed[Y] = (bmRand()%5)*PI;
	rotationSpeed[Z] = 0.0f;
	rotationSpeed[W] = 1.0f;

	if (ivelocity)
	{
		bmVectorCopy(velocity,ivelocity);
	}
	else
	{
		bmMatMultiplyVector(matrix,velocity);
		bmVectorCMul(velocity, velocity, METERTOUNIT(20.0f));
	}

	flag = DEBRISFLAG_GRAVITY;
	flag |= DEBRISFLAG_ROTATESPRITE|DEBRISFLAG_PHYSICS|DEBRISFLAG_AFFECTOBJECTS;

	if (!item)
	{
		if(destInfo->componentDebris)
		{
			number = 0;
		}
		else
		{
			number = bmRand()%debris->numDebris;
		}
	}

	TBVector	offset;
	SETVECTOR(offset,0.0f,0.0f,0.0f,1.0f);

	DEBRIS	*debrisActor;

	if((!item)&&((debrisTexture = bkLoadTextureByCRC(0, debris->debrisCRC[number], 0)) != NULL))
	{
		if (!time) time = 1.0f;
		bmVectorCopy(spriteVelocity, velocity);
		debrisActor = CreateDebrisActor(debrisTexture, pos, spriteVelocity, rotationSpeed,time, RAD((float)(bmRand()%360)), METERTOUNIT(0.3f), METERTOUNIT(0.3f), 128, flag,0,0,RAD((float)(bmRand()%720)-360.0f));
		if (debrisActor)
		{
			CreateActorInstanceStatus(debrisActor->actorStructure);
			debrisActor->actorStructure->actorStatus->lastState = debrisActor->actorStructure->actorStatus->currentState;
			debrisActor->actorStructure->actorStatus->currentState = STATE_SMASH;

			if (size)
			{
				SetActorCollSphere(debrisActor->actorStructure->actorBehaviour,offset,size);
				SetActorCylinder(0,debrisActor->actorStructure->actorBehaviour,offset,size,size*2);
			}
			else
			{
				SetActorCollSphere(debrisActor->actorStructure->actorBehaviour,offset,METERTOUNIT(0.5f));
				SetActorCylinder(0,debrisActor->actorStructure->actorBehaviour,offset,METERTOUNIT(0.5f),METERTOUNIT(1.0f));
			}
			
//			AddActorToCollisionCylinderCache(&collisionCache,debrisActor->actorStructure);

			StopCollidingWith(debrisActor->actorStructure,actorInstance);
			StopCollidingWith(actorInstance,debrisActor->actorStructure);
			debrisActor->actorStructure->playerNo = actorInstance->playerNo;
			debrisActor->actorStructure->flags |= ACTORFLAG_DEBRISACTOR;
		}
	}
	else
	{
		if (!time) time = 2.0f;
		debrisActor = CreateDebrisActor(item==NULL?debris->debrisCRC[number]:item, pos, velocity, rotationSpeed,time, 1.0f, 0, 0, 128, flag,0,0,0.0f);
		if (debrisActor)
		{
			if (!item)
			{
				float scale = METERTOUNIT(0.5f)/debrisActor->actorInstance->actor->maxRadius;
				SETVECTOR(debrisActor->actorInstance->scale,scale,scale,scale,1.0f);
			}

			CreateActorInstanceStatus(debrisActor->actorStructure);
			debrisActor->actorStructure->actorStatus->lastState = debrisActor->actorStructure->actorStatus->currentState;
			debrisActor->actorStructure->actorStatus->currentState = STATE_SMASH;

			if (size)
			{
				SetActorCollSphere(debrisActor->actorStructure->actorBehaviour,offset,size);
				SetActorCylinder(0,debrisActor->actorStructure->actorBehaviour,offset,size,size*2);
			}
			else
			{
				SetActorCollSphere(debrisActor->actorStructure->actorBehaviour,offset,METERTOUNIT(0.5f));
				SetActorCylinder(0,debrisActor->actorStructure->actorBehaviour,offset,METERTOUNIT(0.5f),METERTOUNIT(1.0f));
			}
//			AddActorToCollisionCylinderCache(&collisionCache,debrisActor->actorStructure);
			
			// TP: Stop taz and debris colliding with each other
			StopCollidingWith(debrisActor->actorStructure,actorInstance);
			StopCollidingWith(actorInstance,debrisActor->actorStructure);
			debrisActor->actorStructure->playerNo = actorInstance->playerNo;
			debrisActor->actorStructure->flags |= ACTORFLAG_DEBRISACTOR;
		}
	}
	if (!item)
	{
		FreeDestructibleInfo(destInfo);
		SAFE_FREE(destInfo);
		actorInstance->characterInfo->inMouth = NULL;
	}

	return debrisActor;
}

/* --------------------------------------------------------------------------------
   Function : TazScare
   Purpose : called each frame while Taz is performing a scare move.
   Parameters : ACTORINSTANCE *actorInstance
   Returns : 
   Info : 
*/
void TazScare(ACTORINSTANCE *actorInstance)
{
	TBVector	tempVector;
	ACTORINSTANCE *ptr;

	// test against character list
	ptr = map.characterInstances.head.next;

	for (ptr = map.characterInstances.head.next;ptr != &map.characterInstances.head;ptr = ptr->next)
	{
		bmVectorSub(tempVector,actorInstance->actorInstance.position,ptr->actorInstance.position);
		if (bmVectorLen(tempVector) < METERTOUNIT(3.0f))
		{
			AICharacterScared(ptr,actorInstance);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function		: GetYAngleBetweenLinesOfSight
   Purpose		: calculates the y angle between two actor instances' lines of sight /
					determines to what extent two characters are facing each other.
   Parameters	: ptr to TBActorInstance 1, ptr to TBActorInstance 2
   Returns		: the smallest angle between the two lines of sight, in the range 0..PI:
					PI means they are facing each other
   Info			: // PP: this takes into account the XZ positions of the two actor instances.
					That's how it's different to just getting the angle between
					the two actor instances' Y angles.
					For example, this function is used in the Zoo boss game
					to determine if Taz and the elephant are facing each other when you
					try to do the scare move.
*/
float GetYAngleBetweenLinesOfSight(TBActorInstance* const actorInstance1, TBActorInstance* const actorInstance2)
{
	// PP: get the maximum of the two characters' relative angles-from-line-of-sight-to-other-character
	return MAX(GetYAngleFromLineOfSight(actorInstance1, actorInstance2), GetYAngleFromLineOfSight(actorInstance2, actorInstance1));

	// PP: this function could be made a teensy bit faster at the expense of simplicity - it's always the way.  Just let it li ok?								// PP: And the nominees for Most Tasteless Comment are...
}

/* --------------------------------------------------------------------------------
   Function		: GetYAngleFromLineOfSight
   Purpose		: calculates the Y angle from a character's line of sight to another character
   Parameters	: ptr to TBActorInstance 1, ptr to TBActorInstance 2
   Returns		: the smallest number of radians that the first character would have to turn
					in order to face the second character
   Info			: // PP: For example in the Zoo boss game, this tells me how far around the
						elephant Taz is, so I know whether he's going to get prodded
						by its tusks or not
*/
float GetYAngleFromLineOfSight(TBActorInstance* const actorInstance1, TBActorInstance* const actorInstance2)
{
	TBVector2D		distVec;		// PP: the XZ difference vector betwen the two characters' positions
	float			ang;			// PP: the Y angle between the two characters
	float			ang1To2;		// PP: the Y angle from character 1's line of sight to character 2

	// PP: get the XZ difference vector betwen the two characters' positions
	distVec[X]=actorInstance2->position[X]-actorInstance1->position[X];
	distVec[Y]=actorInstance2->position[Z]-actorInstance1->position[Z];

	// PP: get the Y angle between the two characters
	ang=bmATan2(distVec[Y], distVec[X]);

	// PP: get the Y angle from character 1's line of sight to character 2
	ang1To2=Aabs(ang-GetAngleAroundYFromQuat(actorInstance1->orientation));
	ang1To2=Aabs(ang1To2+HALFPI);
	ang1To2=FORCE_SIGN(ang1To2);
	ang1To2=bmFAbs(ang1To2);

	return ang1To2;
}

/*  --------------------------------------------------------------------------------
	Function	: BLINKEYES::Update
	Purpose		: Update the animation of the eyes and update state
	Parameters	: 
	Returns		: 
	Info		: 
*/
void BLINKEYES::Update()
{
	TBActorAnimSegment *animation;

	if (instance)
	{
		baPrepActorInstance(instance,TRUE);
		baUpdateNodes(instance, instance->rootNodeInstance, speedRatio, NULL);

		switch(state)
		{
		case BLINKSTATE_INIT:
			animation = baFindAnimSegment(instance, "idle");
			baQueueNodeAnimNormal(instance, instance->rootNodeInstance, animation, 1024, -1, BANIMQUEUEFLAG_LOOPING, NULL, NULL);

			state = BLINKSTATE_IDLE;
			time = 0.0f;
			break;
		case BLINKSTATE_IDLE:
			time += fTime;

			if (time > 2.0f)
			{
				time = 0.0f;
				if ((bmRand()%5)==0)
				{
					baFlushNodeQueue(instance, instance->rootNodeInstance, TRUE);
					animation = baFindAnimSegment(instance, "blink1");
					baQueueNodeAnimNormal(instance, instance->rootNodeInstance, animation, 1024, -1, 0, NULL, NULL);

					animation = baFindAnimSegment(instance, "idle");
					baQueueNodeAnimNormal(instance, instance->rootNodeInstance, animation, 1024, -1, BANIMQUEUEFLAG_LOOPING, NULL, NULL);
				}
				if ((bmRand()%5)==1)
				{
					baFlushNodeQueue(instance, instance->rootNodeInstance, TRUE);
					animation = baFindAnimSegment(instance, "blink2");
					baQueueNodeAnimNormal(instance, instance->rootNodeInstance, animation, 1024, -1, 0, NULL, NULL);

					animation = baFindAnimSegment(instance, "idle");
					baQueueNodeAnimNormal(instance, instance->rootNodeInstance, animation, 1024, -1, BANIMQUEUEFLAG_LOOPING, NULL, NULL);
				}
			}
			break;
		}
	}
}

/*  --------------------------------------------------------------------------------
	Function	: BLINKEYES::TAG_BLINKEYES
	Purpose		: Constructor, creates actorInstance for eyes and initialises state
	Parameters	: 
	Returns		: 
	Info		: 
*/
BLINKEYES::TAG_BLINKEYES()
{
	instance = new TBActorInstance;
	if (instance)
	{
		baInitActorInstance(instance, FindActorInActorList("tazeyeblink.obe"),2);
		baPrepActorInstance(instance, TRUE);
		state = BLINKSTATE_INIT;
		time = 0.0f;
	}
}

/*  --------------------------------------------------------------------------------
	Function	: BLINKEYES::Draw
	Purpose		: Draw the eyes
	Parameters	: 
	Returns		: 
	Info		: Does not set the render state
*/
//float	blinkpos = 100.0f;
void BLINKEYES::Draw(CAMERAINFO *camera)
{
	TBMatrix	tempMatrix;
	TBVector	tempVector;

	if (instance)
	{
		overrideWidescreen();
		applyFOV();

		// TP: position and orient object to face camera
		bmMatYXZRotation(tempMatrix,camera->cameraState.cameraAngles[X],camera->cameraState.cameraAngles[Y],camera->cameraState.cameraAngles[Z]);
		bmMatrixToQuat(instance->orientation,tempMatrix);

		// TP: calc position
		bmVectorCopy(instance->position,camera->cameraState.cameraPos);

		bmVectorSub(tempVector, camera->cameraState.cameraLookat, camera->cameraState.cameraPos);
		bmVectorScaleToLength(tempVector,tempVector,270.0f);
		bmVectorAdd(instance->position,instance->position,tempVector);

		baPrepActorInstance(instance,FALSE);
		baDrawActorInstance(instance);

		overrideWidescreen(false);
		applyFOV();
	}
}

/*  --------------------------------------------------------------------------------
	Function	: DrawBlinkEyes
	Purpose		: Spin through characterInstances list to see if there are any blink eyes to draw
	Parameters	: camera currently in use
	Returns		: 
	Info		: 
*/
void DrawBlinkEyes(CAMERAINFO *camera)
{
	ACTORINSTANCE *ptr;

	if (!characterInstances.numEntries) return;

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	bdSetTexture(0, NULL);

	for (ptr = characterInstances.head.next;ptr != &characterInstances.head;ptr = ptr->next)
	{
		if (ptr->playerNo==-1) continue;
		if (gameStatus.player[ptr->playerNo].camera == camera)
		{
			if ((gameStatus.player[ptr->playerNo].actorInstance)&&(gameStatus.player[ptr->playerNo].actorInstance->characterInfo)&&(gameStatus.player[ptr->playerNo].actorInstance->characterInfo->blinkEyes))
			{
				gameStatus.player[ptr->playerNo].actorInstance->characterInfo->blinkEyes->Update();
				gameStatus.player[ptr->playerNo].actorInstance->characterInfo->blinkEyes->Draw(camera);
			}
		}
	}

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
}