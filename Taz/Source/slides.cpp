// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : slides.cpp
//   Purpose : provides functions to create slides
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "actors.h"
#include "collisioncache.h"
#include "scene.h"
#include "status.h"
#include "animation.h"
#include "physics.h"
#include "maths.h"
#include "slides.h"
#include "sfx.h"


/* --------------------------------------------------------------------------------
   Function : CreateSlide
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateSlide(ACTORINSTANCE *actorInstance)
{
	if(!actorInstance) return FALSE;

	actorInstance->special = SPECIALTYPE_SLIDE;

	AddActorToCollisionCache(&collisionCache, actorInstance, 0);
	SetCollisionCallback(actorInstance, RespondToSlideCollision);

	return TRUE;
}

static void myStateChange(ACTORINSTANCE *actorInstance)
{
	TBVector offset;

	SETVECTOR(offset, 0.0f, METERTOUNIT(0.1f), 0.0f, 1.0f);

	CalculateSpeedBeforeJump(actorInstance);	// fill in current speed before spin
	SetActorMinFricAngle(actorInstance->actorBehaviour,1.0f);
	// flush anim ready for move state
	FlushAnimationQueue(actorInstance);
	
	PlayAnimationByName(actorInstance,"slip",1.0f,0,0,0.1f,JUMP);
	PlayAnimationByName(actorInstance,"slidle2",1.0f,1,1,0.1f,JUMP);

	StopChannel(&actorInstance->actorStatus->soundHandle[0]);
	actorInstance->actorStatus->soundHandle[0] = PlaySample("loop\\taz_gen_08a.wav",255,actorInstance->actorInstance.position);

//	actorInstance->actorAnimation->useMotionBone = FALSE;
//	SetActorFriction(actorInstance->actorBehaviour, METERTOUNIT(0.0f));
//	SetActorVelocity(actorInstance->actorBehaviour,actorInstance->actorStatus->velBeforeJump);

//	actorInstance->actorBehaviour->collSphere.radius = METERTOUNIT(0.3895f);
	actorInstance->actorAnimation->useMotionBone = FALSE;
	// TP: Removed because you can fall through the museum sewer if you leave the ground for too long
	// TP: because the coll sphere offset is changed
	//SetActorCollSphere(actorInstance->actorBehaviour, offset, METERTOUNIT(0.25f));
	SetActorFriction(actorInstance->actorBehaviour, METERTOUNIT(0.9f));
	SetActorVelocity(actorInstance->actorBehaviour,actorInstance->actorStatus->velBeforeJump);
}

/* --------------------------------------------------------------------------------
   Function : CreateSlide
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/
int	RespondToSlideCollision(ACTORINSTANCE *collider, ACTORINSTANCE *slide)
{
	if (collider->flags&ACTORFLAG_DEBRISACTOR) return FALSE;

	collider->actorStatus->timeWithoutWaterSlideCollision = 0.0f;

	if (collider->actorStatus->currentState == STATE_SLIDE) return FALSE;
	if (collider->actorStatus->currentState == STATE_WATERSLIDE) return FALSE;

	if(collider->actorStatus->currentState != STATE_JUMP)
	{
		RequestStateChangePlus(collider, STATE_WATERSLIDE, myStateChange);
		//-----------------STATE CHANGE-----------------
	}
	return FALSE;				//yes please to physics
}