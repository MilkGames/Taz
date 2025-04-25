// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : springyobjects.cpp
//   Purpose : functions to calculate path of object sprung by spring
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "main.h"

#include "actors.h"
#include "collisioncache.h"
#include "physics.h"
#include "trigger.h"
#include "animation.h"
#include "status.h"
#include "scene.h"
#include "maths.h"
#include "springyobjects.h"
#include "sfx.h"
#include "genericai.h"
#include "rumble.h"

float radius;

/* --------------------------------------------------------------------------------
   Function : CreateSpringyObject
   Purpose : Creates and initialises actorInstances that act as springs
   Parameters : char *name, TBVector init position, float max height
   Returns : 
   Info : 
*/

int CreateSpringyObject(ACTORINSTANCE *springyObjectInstance, TBVector initPosition, float maxHeight, ESpecialType type, float radius, int alwaysBounce,int collFlags, bool state, bool collision)
{
	CreateActorInstanceBehaviour( springyObjectInstance );
	CreateSpringyActorInstanceInfo( springyObjectInstance );

	//EnableActorLineRenderMode( &springyObjectInstance->actorInstance );

	//set attributes
	springyObjectInstance->springyObjectInfo->type = type;
	springyObjectInstance->special = type;
	if (radius == 0)
	{
		bkPrintf("      *** WARNING *** Radius on springy object is 0.\n");
	}
	springyObjectInstance->springyObjectInfo->springRadius = radius;
	if (maxHeight==0)
	{
		bkPrintf("      *** WARNING *** Max height for springy object is 0.\n");
	}
	springyObjectInstance->springyObjectInfo->maxHeight = maxHeight;
	springyObjectInstance->springyObjectInfo->alwaysBounce = alwaysBounce;
	springyObjectInstance->springyObjectInfo->active = state;

	SetActorPosition( springyObjectInstance, initPosition );

	if (collision)
	{
		AddActorToCollisionCache(&collisionCache,springyObjectInstance, collFlags);
	}
	SetCollisionCallback(springyObjectInstance,RespondToSpringCollision);

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : CreateSpringyActorInstanceInfo
   Purpose : Creates and initialises springy info part of object
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int CreateSpringyActorInstanceInfo(ACTORINSTANCE *actorInstance)
{
	actorInstance->springyObjectInfo = (SPRINGYOBJECTINFO*)ZALLOC(sizeof(SPRINGYOBJECTINFO));
	if (actorInstance->springyObjectInfo)
	{
		//actorInstance->springyObjectInfo->type = SPECIALTYPE_SPRINGYOBJECT;
		return TRUE;
	}
	else
	{
		bkPrintf("*** ERROR *** Could not create springy object info structure\n");
		return FALSE;
	}
}


/*  --------------------------------------------------------------------------------
	Function	: 
	Purpose		: 
	Parameters	: 
	Returns		: 
	Info		: 
*/

static void myStateChange(ACTORINSTANCE *actorInstance)
{
	actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;
	actorInstance->actorAnimation->useMotionBone = FALSE;
}


/* --------------------------------------------------------------------------------
   Function : RespondToSpringCollision
   Purpose : calculates response when an actor collides
   Parameters : ACTORINSTANCE *spring, ACTORINSTANCE *collider
   Returns : 
   Info : 
*/

int RespondToSpringCollision(ACTORINSTANCE *collider, ACTORINSTANCE *spring)
{
	TBVector		yDirection = { 0.0f, 1.0f, 0.0f, 1.0f };
	TBVector		pos;
	float			speed;
	char			message[256];
	int				val;

	if (spring->springyObjectInfo->active==false) return true;

	if (collider->genericAI) 
	{
		AICollideWithSpecial(collider,spring);
		return FALSE;
	}
	if (collider->flags&ACTORFLAG_DEBRISACTOR) return FALSE;

	if( ((collider->actorStatus->currentState == STATE_JUMP) || (collider->actorStatus->currentState == STATE_PROJECTILE)||(spring->springyObjectInfo->alwaysBounce)) 
			&& (spring->actorInstance.position[1] < collider->actorInstance.position[1]) && (speed = CalculateInitialVelocity(spring, collider)))
	{
		if ((bmRand()%10)>5)
		{
			val = PlayAnimationByName(collider,"bounceup",1.0f,1,0,0.1f,JUMP);	
		}
		else
		{
			val = PlayAnimationByName(collider,"bounceup2",1.0f,0,0,0.1f,JUMP);	
		}

		// CMD: does anyone actually need this? the amplitude of 2 is unnoticable
//		if (gameStatus.currentState == GAMESTATE_INGAME)
//			RequestRumbleEffect(gameStatus.player[collider->playerNo].controller, 2, 0.05f, (0.5f-0.1f), 0.5f, 0.0f, WAVEFORM_NORMAL, 0.0f, 0.0f, 0);

		if ((map.sceneNumber==SCENE_CITYBOSS)||(val))
		{	
			bmVectorCopy(pos,spring->actorInstance.position);

			if (!(bmRand() % 4))
				PlaySample("spring2.wav",255,0,0,spring->actorInstance.position,0);
			else
				PlaySample("spring.wav",255,0,0,spring->actorInstance.position,0);

			// send messages
			if (spring->springyObjectInfo->onContact)
				SendTrigger(spring->springyObjectInfo->onContact, pos);

			// TP: play anim on platform if there is one
			if (spring->actorInstance.numAnimSegments)
			{
				PlayAnimationByName(spring,"bounce",1.0f,0,0,0.0f,NONE);
			}

			collider->actorBehaviour->physics.velocity[1] = 0.0f;
			AddActorSpeed( collider->actorBehaviour, yDirection, speed );
	
//			collider->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;
//			collider->actorAnimation->useMotionBone = FALSE;
			//StopCollidingWith(collider,spring);
			// added this ?
//			RequestStateChange(collider,STATE_SPRUNG);
			RequestStateChangePlus(collider,STATE_SPRUNG,myStateChange);
			if(collider->playerNo != -1)
			{
				RequestRumbleEffect(gameStatus.player[collider->playerNo].controller, 200, 0.0f, 0.0f, 0.5f, 0.0f, WAVEFORM_NORMAL);
			}
			//-----------------STATE CHANGE-----------------
		}
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : CalculateInitialVelocity
   Purpose : calculates initial velocity
   Parameters : ACTORINSTANCE *spring, ACTORINSTANCE *collider
   Returns : 
   Info : 
*/

float CalculateInitialVelocity( ACTORINSTANCE *spring, ACTORINSTANCE *collider )
{
	float			radius, height;
	float			offset = METERTOUNIT(0.5f);
	double			constant = 0.5;

	if(spring->special == SPECIALTYPE_TRAMAMPOLINE)
	{
		//calculate distance from actor to maximum springy point
		radius = bmSqrt(((collider->actorInstance.position[0] - spring->actorInstance.position[0])*(collider->actorInstance.position[0] - spring->actorInstance.position[0])) 
						+ ((collider->actorInstance.position[2] - spring->actorInstance.position[2])*(collider->actorInstance.position[2] - spring->actorInstance.position[2])));

		if(radius <	offset)
		{
			height = spring->springyObjectInfo->maxHeight;
			return( bmSqrt( 2.0f * (-worldGravity) * height ) );
		}
		else
			if(radius < spring->springyObjectInfo->springRadius )
			{
				//calculate height sprung
				height = spring->springyObjectInfo->maxHeight*bmCos(RAD(90.0f)*radius/spring->springyObjectInfo->springRadius);
//				height = spring->springyObjectInfo->maxHeight / ( (float)exp( constant*(double)UNITTOMETER((radius-offset))) );
				//calculate required initial speed in +ve y direction and return
				return( bmSqrt( 2.0f * (-worldGravity) * height ) );
			}
		//else return zero
		return 0.0f;
	}
	//else return zero
	return 0.0f;
}