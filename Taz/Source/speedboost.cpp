// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : springyobjects.cpp
//   Purpose : functions to calculate path of object sprung by spring
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "actors.h"
#include "status.h"
#include "collisioncache.h"
#include "physics.h"
#include "scene.h"
#include "camera.h"
#include "rumble.h"
#include "main.h"
#include "animation.h"
#include "speedboost.h"
#include "sfx.h"
#include "genericai.h"
#include "trigger.h"

/* --------------------------------------------------------------------------------
   Function : CreateSpeedBoost
   Purpose : creates an object which gives an actor a boost in speed if collided with
   Parameters : 
   Returns : 
   Info : 
*/

int CreateSpeedBoost(ACTORINSTANCE *speedboostInstance, TBVector position, TBQuaternion direction, float speed, uint32 target, bool active,bool collision)
{
	TBMatrix		tempMatrix, final;
	TBMatrix		rotationCorrection;

	speedboostInstance->special = SPECIALTYPE_SPEEDBOOST;
	speedboostInstance->speedboostInfo = CreateSpeedBoostInfo();

	if (!speedboostInstance->speedboostInfo) return false;

	speedboostInstance->speedboostInfo->onBoost = NULL;

	//calculate direction vector from direction quaternion
	bmQuatToMatrix(tempMatrix, direction);
	bmMatXYZRotation(rotationCorrection, RAD(0.0f), RAD(0.0f), RAD(0.0f));
	bmMatMultiply(final, rotationCorrection, tempMatrix);
	SETVECTOR(speedboostInstance->speedboostInfo->direction, 0.0f, 0.0f, 1.0f, 1.0f);
	bmMatMultiplyVector(final, speedboostInstance->speedboostInfo->direction);
	bmVectorScaleToLength(speedboostInstance->speedboostInfo->direction, speedboostInstance->speedboostInfo->direction, speed);

	speedboostInstance->speedboostInfo->active = active;


	AddActorToCollisionCache(&collisionCache, speedboostInstance, 0);
	SetCollisionCallback(speedboostInstance, RespondToSpeedboostCollision);
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : CreateSpeedBoostInfo
   Purpose : does the same as all the other functions of this type
   Parameters : 
   Returns : 
   Info : 
*/

SPEEDBOOSTINFO *CreateSpeedBoostInfo(void)
{
	SPEEDBOOSTINFO		*tempInfo;

	tempInfo = (SPEEDBOOSTINFO*)ZALLOC(sizeof(SPEEDBOOSTINFO));

	if(tempInfo)
	{
		return tempInfo;
	}
	else
	{
		bkPrintf("*** ERROR *** could not create info structure for one of those stupid speedboost things.\n");
		return NULL;
	}
}

/* --------------------------------------------------------------------------------
   Function : RespondToSpeedboostCollision
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToSpeedboostCollision(ACTORINSTANCE *collider, ACTORINSTANCE *speedboost)
{
	char	message[256];
	TBVector	pos;

	if (!speedboost->speedboostInfo->active) return FALSE;

	if (collider->genericAI) 
	{
		AICollideWithSpecial(collider,speedboost);
		return FALSE;
	}

	StopCollidingWith(collider, speedboost, 1.0f);
	// TP: PROBLEM
	if (collider->playerNo != -1)
	{
		StopCollidingWith(&gameStatus.player[collider->playerNo].camera->collisionActor, speedboost);
	}
	SetActorVelocity(collider->actorBehaviour, speedboost->speedboostInfo->direction);
	collider->actorStatus->timeOfLastSpeedBoost = gameStatus.gameTime;

	float mySpeed = bmVectorLen(collider->actorBehaviour->physics.velocity);

	// PP: I don't think it's defined anywhere, but -1 here means 'no player'
	if(collider->playerNo != -1)
	{
		if (gameStatus.player[collider->playerNo].controller->rumbleController)
			RequestRumbleEffect(gameStatus.player[collider->playerNo].controller, (int ) (mySpeed / 20.0f), 0.0f, 0.5f, 0.75f, 0.0f, WAVEFORM_NORMAL);
	}

	// PP: use position of speedboost object for the sound message
	bmVectorCopy(pos, speedboost->actorInstance.position);

	if (speedboost->speedboostInfo->onBoost)
	{
		SendTrigger(speedboost->speedboostInfo->onBoost, pos);
	}

	if((collider->actorAnimation)&&((collider->actorAnimation->useMotionBone == TRUE) || (collider->actorStatus->currentState == STATE_VEHICLE)))
	{
		switch(map.sceneNumber)
		{
		case SCENE_MUSEUM:
			PlaySample("obj_mus_22a.wav",255,speedboost->actorInstance.position);
			break;
		default:
			break;
		}
		RequestStateChange(collider, STATE_PROJECTILESLIDE);
		collider->actorAnimation->useMotionBone = FALSE;
	}

	return FALSE;
}
