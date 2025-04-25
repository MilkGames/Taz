// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : ice.cpp
//   Purpose : functions to calculate path of falling object
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "scene.h"
#include "status.h"
#include "collisioncache.h"
#include "animation.h"
#include "physics.h"
#include "characters.h"
#include "clouds.h"
#include "ice.h"
#include "sfx.h"
#include "genericai.h"

/*	--------------------------------------------------------------------------------
	Function 	: CreateIceInstance
	Purpose 	: 
	Parameters 	: iceInstance, liveupdate for collision(TRUE/FALSE)
	Returns 	: 
	Info 		:
*/
void CreateIceInstance(ACTORINSTANCE *iceInstance,char *animation,char liveUpdate)
{
	//set attributes
	AddActorToCollisionCache(&collisionCache,iceInstance, liveUpdate);
	SetCollisionCallback(iceInstance,RespondToIceCollision);

	if ((animation)&&(animation[0]!=0))
	{
		FlushAnimationQueue(iceInstance);
		PlayAnimationByName(iceInstance, animation, 1.0f, 1, 0, 0.0f, NONE);
	}
}

void IceStateChangeCallback(ACTORINSTANCE *actorInstance)
{
	PlaySample("spinslip.wav",255,actorInstance->actorInstance.position);
	actorInstance->actorStatus->timeWithoutIceCollision = 0.0f;
	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance, "spinslip", 1.0f, 0, 0, 0.1f, JUMP);
	actorInstance->actorAnimation->useMotionBone = FALSE;
	actorInstance->actorStatus->timeWithoutIceCollision = 0.0f;
	SetActorMinFricAngle(actorInstance->actorBehaviour,1.0f);
}

static void IceStateChangeCallbackNoSpin(ACTORINSTANCE *actorInstance)
{
	IceStateChangeCallback(actorInstance);
	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance, "slipslide", 1.0f, 0, 0, 0.1f, JUMP);
	actorInstance->actorStatus->timeWithoutIceCollision = 0.0f;
	actorInstance->actorAnimation->useMotionBone = FALSE;
	actorInstance->actorStatus->timeWithoutIceCollision = 0.0f;
	SetActorMinFricAngle(actorInstance->actorBehaviour,1.0f);
}

/* --------------------------------------------------------------------------------
   Function : RespondToIceCollision
   Purpose : calculates response when an actor collides
   Parameters : ACTORINSTANCE *spring, ACTORINSTANCE *collider
   Returns : 
   Info : 
*/
int RespondToIceCollision(ACTORINSTANCE *collider, ACTORINSTANCE *ice)
{
	if (collider->genericAI) 
	{
		AICollideWithSpecial(collider,ice);
		return FALSE;
	}
	if(collider->actorStatus->currentState == STATE_KOIKFROMWATER) return FALSE;

	if (collider->flags&ACTORFLAG_DEBRISACTOR) return FALSE;

	DestroyInjuryStars(collider);

	collider->actorStatus->timeWithoutIceCollision = 0.0f;

	if(collider->actorStatus->currentState == STATE_SPIN)
	{
		// TP: Must change this to a request it's breaking the spinmode
		RequestStateChangePlus(collider,STATE_SPINONICE,IceStateChangeCallback);
		return FALSE;
	}
	else
	{
		switch(collider->actorStatus->currentState)
		{
		case STATE_ONICE:
		case STATE_SPINONICE:
		case STATE_CATATONIC:
			return FALSE;
		default:
			RequestStateChangePlus(collider, STATE_ONICE, IceStateChangeCallbackNoSpin);
//			if(!RequestStateChange(collider, STATE_ONICE)) return TRUE;
//			FlushAnimationQueue(collider);
//			PlayAnimationByName(collider, "slipslide", 1.0f, 0, 0, 0.1f, JUMP);
			break;
		}
	}
//	collider->actorAnimation->useMotionBone = FALSE;
//	collider->actorStatus->timeWithoutIceCollision = 0.0f;
//	SetActorMinFricAngle(collider->actorBehaviour,1.0f);
	return FALSE;
}