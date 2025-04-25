// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : portal.cpp
//   Purpose : controls functionality of hub portals
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "main.h"
#include "actors.h"
#include "animation.h"
#include "physics.h"
#include "status.h"
#include "characters.h"
#include "camera.h"
#include "portal.h"
#include "genericai.h"

/* --------------------------------------------------------------------------------
   Function : CreatePortalInstance
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreatePortalWorldInstance(ACTORINSTANCE *actorInstance)
{
	actorInstance->special = SPECIALTYPE_PORTALWORLD;

	AddActorToCollisionCache(&collisionCache, actorInstance, 0);
//	SetCollisionCallback(actorInstance, RespondToPortalCollision);
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : CreatePortalInstance
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToPortalCollision(ACTORINSTANCE *collider, ACTORINSTANCE *portal)
{
	if (collider->genericAI) 
	{
		AICollideWithSpecial(collider,portal);
		return TRUE;
	}
	//stuff to do every frame
	SetFixedDistanceToLookAt(gameStatus.player[collider->playerNo].camera, METERTOUNIT(8.0f));

	if(collider->actorStatus->currentState == STATE_INPORTAL) return TRUE;

	//stuff to do once
//	collider->actorBehaviour->physics.flags |= PHYSFLAG_DIRGRAVITY;
	gameStatus.chrisCam = FALSE;
//	FlushAnimationQueue(collider);

//	RequestStateChange(collider, STATE_INPORTAL);

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : CreatePortalInstance
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void CalculatePortalWorldGravity(ACTORINSTANCE *actorInstance)
{
	ACTORINSTANCELIST	*list = &map.levelInstances;
	ACTORINSTANCE		*ptr;
//	TBQuaternion		quat, tempQuat;

	for(ptr = list->head.next;ptr != &list->head;ptr = ptr->next)
	{
		if(ptr->special != SPECIALTYPE_PORTALWORLD) continue;

		//there should only be one instance in list
		bmVectorSub(actorInstance->characterInfo->portalGravity, actorInstance->actorInstance.position, ptr->actorInstance.position);
		bmVectorNorm(actorInstance->characterInfo->portalGravity, actorInstance->characterInfo->portalGravity);

		//also update orientation of actorInstance to match gravity
//		bmDirectionToQuat(quat, actorInstance->characterInfo->portalGravity, 0.0f);
//		bmQuatXRotation(tempQuat, RAD(90.0f));
//		bmQuatMultiply(quat, tempQuat, quat);
//		SetActorOrienation(actorInstance, quat);
	}
}