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
#include "main.h"
#include "physics.h"
#include "animation.h"
#include "characters.h"
#include "water.h"
#include "fade.h"
#include "status.h"
#include "death.h"
#include "genericai.h"
#include "trail.h"


/* --------------------------------------------------------------------------------
   Function : PreRespondToDeathCollision
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int PreRespondToDeathCollision(ACTORINSTANCE *collider, ACTORINSTANCE *deathObject, PRECOLLINFO *info)
{
	return FALSE;
}


/* --------------------------------------------------------------------------------
   Function : CreateDeathObject
   Purpose : Creates an object which will kill taz immediately on contact
   Parameters : 
   Returns : 
   Info : 
*/

int CreateDeathObject(ACTORINSTANCE *actorInstance)
{
	if(!actorInstance) return FALSE;

	actorInstance->deathObjectInfo = (DEATHOBJECTINFO*)ZALLOC(sizeof(DEATHOBJECTINFO));
	ASSERTM(actorInstance->deathObjectInfo, "death object could not be malloced");
	if(!actorInstance->deathObjectInfo) return FALSE;

	actorInstance->special = SPECIALTYPE_DEATH;
	AddActorToCollisionCache(&collisionCache, actorInstance, 0);

	// TP: Added normal coll and pre coll callbacks 'cause the stopcollidingwith function only works on non-pre callbacks
	// TP: so I was getting multiple calls to RespondToDeathCollision
	SetPreCollisionCallback(actorInstance, PreRespondToDeathCollision);
	SetCollisionCallback(actorInstance, RespondToDeathCollision);
	return TRUE;
}


/* --------------------------------------------------------------------------------
   Function : RespondToDeathCollision
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToDeathCollision(ACTORINSTANCE *collider, ACTORINSTANCE *deathObject)
{
	if (collider->genericAI) 
	{
		AICollideWithSpecial(collider,deathObject);
		return FALSE;
	}
	
	if(deathObject->deathObjectInfo->resetInfo)
	{
		if(!(collider->flags&ACTORFLAG_DEBRISACTOR))
		{
			if((collider->actorStatus)&&(collider->actorStatus->currentState != STATE_CATATONICPHYS))
			{
				CalculateDeathObjectResetPoint(collider, deathObject);
				StopCollidingWith(collider, deathObject, 5.0f);
			}
		}
	}

	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : CalculateDeathObjectResetPoint
   Purpose : calculates reset point
   Parameters : ACTORINSTANCE *drowner, ACTORINSTANCE *water
   Returns : 
   Info : 
*/

void CalculateDeathObjectResetPoint(ACTORINSTANCE *collider, ACTORINSTANCE *deathObject)
{
	TBVector			resetPoint;
	TBQuaternion		resetQuat;
	TBVector			tempVector;
	TBVector			worldSpace, screenSpace;
	ACTORINSTANCE		*tempInstance;
	char				*tempName;
	float				tempLen;
	float				previousBest = -1;
	int					counter;
	TBActorNodeInstance	*node;
	
	KillAllPowerUps(collider);

	for(counter = 0; counter < deathObject->deathObjectInfo->resetInfo->numObjects; counter++)
	{
		tempName = deathObject->deathObjectInfo->resetInfo->objectNames[counter];
		tempInstance = FindActorInstanceInInstanceList(tempName, 0, &map.invisibleInstances);
		if(!tempInstance) tempInstance = FindActorInstanceInInstanceList(tempName, 0, &map.collisionInstances);
		if(!tempInstance) tempInstance = FindActorInstanceInInstanceList(tempName, 0, &map.levelInstances);

		if(tempInstance)
		{
			bmVectorSub(tempVector, tempInstance->actorInstance.position, collider->actorInstance.position);
			tempLen = bmVectorLen(tempVector);

			if((counter == 0) || (tempLen < previousBest))
			{
				previousBest = tempLen;
				bmVectorCopy(resetPoint, tempInstance->actorInstance.position);
				bmQuatCopy(resetQuat, tempInstance->actorInstance.orientation);
			}
		}
	}
	if(previousBest > 0.0f)
	{
		if (collider->playerNo!=-1)
		{
			node = baFindNode(collider->actorInstance.rootNodeInstance, "snoutstem");
			if(node)
			{
				baGetNodesWorldPosition(&collider->actorInstance, node, worldSpace);
			}
			else
			{
				bmVectorCopy(worldSpace, collider->actorInstance.position);
				if(collider->actorBehaviour)
				{
					bmVectorAdd(worldSpace, worldSpace, collider->actorBehaviour->collSphere.offset);
				}
			}

			gameStatus.player[collider->playerNo].camera->fade.SetPlayer(collider);

			if (EnterState(collider, STATE_CATATONICPHYS))
			{
				if (gameStatus.player[collider->playerNo].camera->fade.Lock() || gameStatus.player[collider->playerNo].actorInstance->actorStatus->currentState == STATE_VEHICLE)
				{
					gameStatus.player[collider->playerNo].camera->fade.StartFadeDown(FADETYPE_CIRCLE, 1.0f, true);
					gameStatus.player[collider->playerNo].camera->fade.SetPlayer(collider);
					gameStatus.player[collider->playerNo].camera->fade.SetFadeDownFinishedCallback(FinishedFadeFromDeathPlane);
					gameStatus.player[collider->playerNo].camera->fade.SetFadeUpFinishedCallback(ResetFadeToPoint);
					worldSpace[3] = 1.0f;
					bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
					bdSetIdentityObjectMatrix();
					bdProjectVertices(screenSpace, worldSpace, 1);
					gameStatus.player[collider->playerNo].camera->fade.SetCircleFadeTo(screenSpace[0], screenSpace[1]);
					
					bmVectorCopy(collider->actorStatus->waterResetPoint, resetPoint);
					bmQuatCopy(collider->actorStatus->waterResetQuaternion, resetQuat);
					gameStatus.player[collider->playerNo].camera->fade.Unlock();
					return;
				}
			}

			// TP: must do immediate reset if fade request fails or could not enter state
			bmVectorCopy(collider->actorStatus->waterResetPoint, resetPoint);
			bmQuatCopy(collider->actorStatus->waterResetQuaternion, resetQuat);
			FinishedFadeFromDeathPlane(&gameStatus.player[collider->playerNo].camera->fade);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : DeathStateChangeCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void DeathStateChangeCallback(ACTORINSTANCE *actorInstance)
{
	if(!CheckIfPlayingAnimation(actorInstance, "jumpfall"))
	{
		FlushAnimationQueue(actorInstance);
		PlayAnimationByName(actorInstance, "jumpfall", 1.0f, 1, 0, 0.6f,REACT);
		actorInstance->characterInfo->powerUpInfo.drawBubbles = FALSE;
	}
}

/* --------------------------------------------------------------------------------
   Function : FinishedFadeFromWater
   Purpose : 
   Parameters : FADE *
   Returns : 
   Info : 
*/

void FinishedFadeFromDeathPlane(class TAG_FADE *fade)
{ 	
	ACTORINSTANCE *player = fade->GetPlayer();

	ResetActorAtSideOfDeathPlane(player, player->actorStatus->waterResetPoint, player->actorStatus->waterResetQuaternion);
	
	// CMD: repsawn power ups
	RespawnAllPowerUps();
}

/* --------------------------------------------------------------------------------
   Function : ResetActorAtSideOfDeathPlane
   Purpose : resets actorInstance at designated point, nearest to water entry point
   Parameters : ACTORINSTANCE *drowner, ACTORINSTANCE *water
   Returns : 
   Info : 
*/

void ResetActorAtSideOfDeathPlane(ACTORINSTANCE *drowner, TBVector resetPoint, TBQuaternion resetQuat)
{
	if(drowner->characterInfo)
	{
		drowner->characterInfo->powerUpInfo.numFire = 0;
	}
	RequestStateChange(drowner, STATE_KOIKFROMDEATHPLANE);
	drowner->actorAnimation->useMotionBone = TRUE;

	//reset actor to reset position
	bmVectorCopy(drowner->actorInstance.position, resetPoint);

	// CMD: set target orientation
	TBVector	tempVector,tempQuat,tempQuat2;

	SETVECTOR(tempVector,0.0f,1.0f,0.0f,RAD(180.0f));
	bmRotationToQuat(tempQuat,tempVector);
	bmQuatMultiply(tempQuat2,resetQuat,tempQuat);

	SetActorOrientation(drowner, tempQuat2);
	ResetCamera(gameStatus.player[drowner->playerNo].camera);
	trailReset(drowner, TRAIL_RESET_ALL);
}