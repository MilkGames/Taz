// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : escalator.cpp
//   Purpose : provides escalator functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "maths.h"
#include "collisioncache.h"
#include "scene.h"
#include "status.h"
#include "physics.h"
#include "animation.h"
#include "escalator.h"

/* --------------------------------------------------------------------------------
   Function : CreateEscalator
   Purpose : Creates escalator object
   Parameters : oreintation of object relative to world
   Returns : true/false
   Info : 
*/

int CreateEscalator(ACTORINSTANCE *actorInstance, TBQuaternion orientation, float speed)
{
	TBMatrix		tempMatrix, final;
	TBMatrix		rotationCorrection;

	actorInstance->special = SPECIALTYPE_ESCALATOR;
	if((actorInstance->escalatorInfo = CreateEscalatorInfo()) == NULL)
	{
		return FALSE;
	}

	//calculate direction vector from direction quaternion
	bmQuatToMatrix(tempMatrix, orientation);
	bmMatXYZRotation(rotationCorrection, RAD(0.0f), RAD(0.0f), RAD(0.0f));
	bmMatMultiply(final, rotationCorrection, tempMatrix);
	SETVECTOR(actorInstance->escalatorInfo->direction, 0.0f, 0.0f, 1.0f, 1.0f);
	bmMatMultiplyVector(final, actorInstance->escalatorInfo->direction);
	bmVectorScaleToLength(actorInstance->escalatorInfo->direction, actorInstance->escalatorInfo->direction, speed);

	actorInstance->escalatorInfo->active = true;
	
	AddActorToCollisionCache(&collisionCache, actorInstance, 0);
	SetCollisionCallback(actorInstance, RespondToEscalatorCollision);
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : CreateEscalatorInfo
   Purpose : creates escalator info structure
   Parameters : 
   Returns : pointer to structure
   Info : 
*/

ESCALATORINFO *CreateEscalatorInfo(void)
{
	ESCALATORINFO	*tempInfo;

	tempInfo = (ESCALATORINFO*)ZALLOC(sizeof(ESCALATORINFO));

	if(tempInfo)
	{
		return tempInfo;
	}
	else
	{
		bkPrintf("*** LEAMINGTON, WE HAVE A PROBLEM ***, could not create escalator info structure\n");
		return NULL;
	}
}

/* --------------------------------------------------------------------------------
   Function : RespondToEscalatorCollision
   Purpose : guess
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToEscalatorCollision(ACTORINSTANCE *collider, ACTORINSTANCE *escalator)
{
	if (!escalator->escalatorInfo->active) return FALSE;
	if(collider->actorStatus)
	{
		collider->actorStatus->escalatorInfoCollWith = escalator->escalatorInfo;
		collider->actorStatus->timeSinceCollidingWithEscalator = 0.0f;
	}
	return FALSE;							//yum, yum I will have physics 
}

/* --------------------------------------------------------------------------------
   Function : UpdateLastEscalator
   Purpose : updates taz as aresult of the last escalator collided with
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateLastEscalator(ACTORINSTANCE *actorInstance)
{
	TBVector		posInc, rotation;

	// PP: check for NULL actorInstance (what the hell's going on here?  Oh, packages - maybe that's it)
	if(actorInstance == NULL) return;

	// PP: check for NULL actorStatus
	if(actorInstance->actorStatus == NULL) return;

	if(actorInstance->actorStatus->escalatorInfoCollWith == NULL) return;

	if((actorInstance->actorStatus->timeSinceCollidingWithEscalator += 1.0f*fTime) < 0.1f)
	{
		bmVectorCMul(posInc, actorInstance->actorStatus->escalatorInfoCollWith->direction, fTime);

		bmQuatToRotation(rotation, actorInstance->actorInstance.orientation);
//		posInc[1] += (10.0f*bmSin(rotation[3]));

		if(actorInstance->actorAnimation)
		{
			if(actorInstance->actorAnimation->useMotionBone)
			{
				bmVectorCMul(posInc, posInc, fps);
			}
		}
		AddActorVelocity(actorInstance->actorBehaviour, posInc);

//		bmVectorAdd(actorInstance->actorInstance.position, actorInstance->actorInstance.position, posInc);
	}
}