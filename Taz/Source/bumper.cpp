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
#include "main.h"
#include "animation.h"
#include "bumper.h"
#include "sfx.h"
#include "genericai.h"
#include "rumble.h"

static ACTORINSTANCE *pBumperIgnoreChar = NULL;


/* --------------------------------------------------------------------------------
   Function : CreateSpeedBoost
   Purpose : creates an object which gives an actor a boost in speed if collided with
   Parameters : 
   Returns : 
   Info : 
*/

int CreateBumper(ACTORINSTANCE *actorInstance, float speedBoost)
{
	actorInstance->special = SPECIALTYPE_BUMPER;
	actorInstance->bumperInfo = CreateBumperInfo();

	actorInstance->bumperInfo->speedBoost = speedBoost;
	actorInstance->bumperInfo->isUp = 0;

	AddActorToCollisionCache(&collisionCache, actorInstance, CACHEFLAGS_LIVEUPDATE);
	SetCollisionCallback(actorInstance, RespondToBumperCollision);
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : CreateBumperInfo
   Purpose : does the same as all the other functions of this type
   Parameters : 
   Returns : 
   Info : 
*/

BUMPERINFO *CreateBumperInfo(void)
{
	BUMPERINFO		*tempInfo;

	tempInfo = (BUMPERINFO*)ZALLOC(sizeof(BUMPERINFO));

	if(tempInfo)
	{
		return tempInfo;
	}
	else
	{
		bkPrintf("*** ERROR *** could not create info structure for one of those stupid bumper things.\n");
		return NULL;
	}
}

/* --------------------------------------------------------------------------------
   Function : RespondToBumperCollision
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToBumperCollision(ACTORINSTANCE *collider, ACTORINSTANCE *bumper)
{
	TBVector		velocity;

	float			mySpeed;

	if (collider->genericAI) 
	{
		AICollideWithSpecial(collider,bumper);
		return FALSE;
	}
	ASSERT(bumper->bumperInfo);
	if(bumper->bumperInfo->isUp && (collider != pBumperIgnoreChar))
	{
		StopCollidingWith(collider, bumper);
		// TP: PROBLEM
		if (collider->playerNo != -1)
		{
			StopCollidingWith(&gameStatus.player[collider->playerNo].camera->collisionActor, bumper);
		}

		bmVectorSub(velocity,collider->actorInstance.position,bumper->actorInstance.position);
		bmVectorScaleToLength(velocity, velocity, bumper->bumperInfo->speedBoost);

		AddActorVelocity(collider->actorBehaviour, velocity);
		if(collider->actorAnimation->useMotionBone == TRUE)
		{
			RequestStateChange(collider, STATE_PROJECTILESLIDE);
			collider->actorAnimation->useMotionBone = FALSE;
		}

		bumper->bumperInfo->justKoiked = 1;

		mySpeed = bmVectorLen(collider->actorBehaviour->physics.velocity);

		RequestRumbleEffect(gameStatus.player[collider->playerNo].controller, (int ) (mySpeed / 20.0f), 0.0f, 0.5f, 0.75f, 0.0f, WAVEFORM_NORMAL);
	}

	return FALSE;
}


void setBumperIgnoreCharacter(ACTORINSTANCE *p)
{
	pBumperIgnoreChar = p;
}
