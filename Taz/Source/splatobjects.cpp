// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : splatobjects.cpp
//   Purpose : objects that cause to to splat all over himself
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "actors.h"
#include "status.h"
#include "physics.h"
#include "main.h"
#include "animation.h"
#include "attach.h"
#include "sfx.h"
#include "characters.h"
#include "control.h"
#include "costumes.h"
#include "splatobjects.h"
#include "genericai.h"

/* --------------------------------------------------------------------------------
   Function : CreateSplatObject
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateSplatObject(ACTORINSTANCE *actorInstance)
{
	if(!actorInstance) return FALSE;

	actorInstance->special = SPECIALTYPE_SPLAT;
	AddActorToCollisionCache(&collisionCache, actorInstance, 0);
	SetCollisionCallback(actorInstance, RespondToSplatCollision);
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : RespondToSplatCollision
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToSplatCollision(ACTORINSTANCE *collider, ACTORINSTANCE *wall)
{
	TBVector			position;
	TBMatrix			matrix;

	if (collider->genericAI) 
	{
		AICollideWithSpecial(collider,wall);
		return FALSE;
	}

	if((collider->actorStatus->currentState == STATE_SPIN)||
		((collider->actorStatus->currentState == STATE_MOVE)&&(gameStatus.player[collider->playerNo].controller->mag1 > 0.8f)))
	{
		bmQuatCopy(collider->actorStatus->splatQuaternion, wall->actorInstance.orientation);
		bmVectorSet(position, 0.0f, 0.0f, collider->actorBehaviour->collSphere.radius, 1.0f);
		bmQuatToMatrix(matrix, wall->actorInstance.orientation);
		bmMatMultiplyVector(matrix, position);
		bmVectorAdd(collider->actorStatus->waterResetPoint, collider->actorInstance.position, position);

		StopCollidingWith(collider, wall, 4.0f);
		RequestStateChangePlus(collider, STATE_SPLATTED, SplatCallback);
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : SplatCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void SplatCallback(ACTORINSTANCE *actorInstance)
{
	TBVector	velocity;

	bmVectorSet(velocity, 0.0f, 0.0f, 0.0f, 1.0f);
    SetActorVelocity(actorInstance->actorBehaviour, velocity);

	//match orientation of object to that of Taz
	SetActorOrientation(actorInstance, actorInstance->actorStatus->splatQuaternion);
	actorInstance->actorInstance.position[0] = actorInstance->actorStatus->waterResetPoint[0];
	actorInstance->actorInstance.position[2] = actorInstance->actorStatus->waterResetPoint[2];

	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance, "recover11a",1.0f,0,0,0.1f,DEATH);
	PlayAnimationByName(actorInstance,"recover11b",1.0f,1,1,0.1f,DEATH);
}