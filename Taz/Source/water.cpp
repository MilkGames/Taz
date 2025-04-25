// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : water.cpp
//   Purpose : functions to calculate the tilt of objects when collided with
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "actors.h"
#include "maths.h"
#include "status.h"
#include "animation.h"
#include "physics.h"
#include "projectile.h"
#include "scene.h"
#include "collisioncache.h"
#include "collectibles.h"
#include "characters.h"
#include "water.h"
#include "watersplash.h"
#include "sfx.h"
#include "fade.h"
#include "main.h"
#include "textures.h"
#include "waterrings.h"
#include "trail.h"
#include "genericai.h"
#include "bubblegum.h"
#include "chillipepper.h"
#include "hiccup.h"
#include "superburp.h"
#include "playerstats.h"

/* --------------------------------------------------------------------------------
   Function : CreateWateryObject
   Purpose : Initialises actorInstances that act as water
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int CreateWateryObject(ACTORINSTANCE *water, struct TAG_BOUNDINGBOX *boundingBox, uint32 numResetPoints, TBVector initialPosition, EWaterType type)
{
	COLLISIONLISTENTRY	*tempCollInfo;

	if(!CreateWateryObjectInfo(water))
	{
		return FALSE;
	}

	SetActorPosition(water, initialPosition);
	tempCollInfo = AddActorToCollisionCache(&collisionCache,water, CACHEFLAGS_DOUBLESIDED);
	if (!tempCollInfo) return FALSE;

	//fill in attributes
	water->wateryObjectInfo->type = type;
	water->special = SPECIALTYPE_WATERYOBJECT;
	water->wateryObjectInfo->numResetPoints = numResetPoints;
	water->wateryObjectInfo->initialCollHeight = tempCollInfo->boundingBox.ymin;
	water->wateryObjectInfo->collision = FALSE;
	water->wateryObjectInfo->time = 0.0f;
	water->wateryObjectInfo->rippleClock = 0.0f;

	SetPreCollisionCallback(water, PreRespondToWaterCollision);
	SetCollisionCallback(water,RespondToWaterCollision);

	//ripple stuff
	water->wateryObjectInfo->boundingBox.xmin = boundingBox->xmin;
	water->wateryObjectInfo->boundingBox.xmax = boundingBox->xmax;
	water->wateryObjectInfo->boundingBox.ymin = boundingBox->ymin;
	water->wateryObjectInfo->boundingBox.ymax = boundingBox->ymax;
	water->wateryObjectInfo->boundingBox.zmin = boundingBox->zmin;
	water->wateryObjectInfo->boundingBox.zmax = boundingBox->zmax;

	water->wateryObjectInfo->rippleCreationTime = 0.1f;
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : CreateWateryObjectInfo
   Purpose : Initialises actorInstanceInfo part
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int CreateWateryObjectInfo(ACTORINSTANCE *water)
{
	water->wateryObjectInfo = (WATERYOBJECTINFO*)ZALLOC(sizeof(WATERYOBJECTINFO));
	if (water->wateryObjectInfo)
	{
		return TRUE;
	}
	else
	{
		bkPrintf("*** ERROR *** Could not create watery object info structure\n");
		return FALSE;
	}
}

/* --------------------------------------------------------------------------------
   Function : RespondToWaterCollision
   Purpose : handles collision with water
   Parameters : ACTORINSTANCE *collider, ACTORINSTANCE *water
   Returns : 
   Info : 
*/

int PreRespondToWaterCollision(ACTORINSTANCE *collider, ACTORINSTANCE *water, PRECOLLINFO *info)
{
	TBVector		spinVelocity;
	TBQuaternion	tempQuat;
	int					leadingColour[4] = {127, 127, 127, 127};
	int					trailingColour[4] = {127, 127, 127, 70};
	float				length;

	if (collider->genericAI) 
	{
		return TRUE;
	}

	// TP: if taz has been captured by shark then ignore
	if ((collider->actorStatus)&&(collider->actorStatus->currentState==STATE_TRAPPED)) return FALSE;

	if(collider->actorStatus->currentState == STATE_SPIN)
	{
		bmVectorCopy(spinVelocity, collider->actorBehaviour->physics.velocity);

		if((spinVelocity[0]*spinVelocity[0] + spinVelocity[2]*spinVelocity[2]) > METERTOUNIT(0.5f))
		{
			spinVelocity[1] = -spinVelocity[1];
			if((length = bmVectorLen(spinVelocity)*0.75f) > METERTOUNIT(10.0f)) length = METERTOUNIT(10.0f);
			bmVectorScaleToLength(spinVelocity, spinVelocity, length);
			bmVectorCopy(collider->actorBehaviour->physics.velocity, spinVelocity);
			if(water->wateryObjectInfo->type == WATERTYPE_WATER)
			{
				if((water->wateryObjectInfo->time += 1/fps) > TIME_BETWEEN_WATER_RINGS)
				{
					water->wateryObjectInfo->time = 0.0f;
					CreateWaterRing(collider->actorInstance.position, NULL, 1.0f);
					PlaySample("splash.wav",255,0,0,collider->actorInstance.position,BSPLAYFLAG_IGNORE);
				}
			}
		}
	}

	if((collider->actorInstance.position[1]+(0.9f*collider->actorBehaviour->collSphere.radius)) <= water->wateryObjectInfo->initialCollHeight)
	{
		SetActorMinFricAngle(collider->actorBehaviour,0.7f);
		if(collider->actorAnimation)
		{
			collider->actorAnimation->useMotionBone = TRUE;
			collider->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLGROUNDCOLLFIX;
			collider->actorBehaviour->physics.flags |= PHYSFLAG_WALLVELCORRECTION;
		}
		switch(collider->actorStatus->currentState)
		{
		case STATE_SPIN:
			// TP: Can't assume there is animation on the collider, might be debris spat
			if (collider->actorAnimation)
			{
				FlushAnimationQueue(collider);
				bmQuatCopy(tempQuat,collider->actorInstance.orientation);
				ChangeActorInstance(collider,FindActorInActorList("tazdevil.obe"),5,NULL);
				bmQuatCopy(collider->actorInstance.orientation,tempQuat);
				ResetCharacterAfterJump(collider);
				FlushAnimationQueue(collider);
				PlayAnimationByName(collider, "spindown", 2.0f, 0, 0, 0.1f, NONE);
				StopChannel(&collider->actorStatus->soundHandle[0]);
				collider->actorStatus->soundHandle[0] = PlaySample("stop.wav",255);
				StopChannel(&collider->actorStatus->soundHandle[1]);
			}
			break;
		case STATE_VEHICLE:
			break;
		}		
		water->wateryObjectInfo->collision = TRUE;
	}
	else
	{
		if(water->wateryObjectInfo->type == WATERTYPE_WATER)
		{
			if((water->wateryObjectInfo->time += 1/fps) > TIME_BETWEEN_WATER_RINGS)
			{
				water->wateryObjectInfo->time = 0.0f;
				CreateWaterRing(collider->actorInstance.position, NULL, 1.0f);
				PlaySample("splash.wav",255,0,0,collider->actorInstance.position,BSPLAYFLAG_IGNORE);
			}
		}
	}
	if(water->wateryObjectInfo->type == WATERTYPE_WATER)
	{
		if(collider->actorBehaviour->physics.velocity[1] < METERTOUNIT(-2.5f))
		{
			CreateWaterSplashes(collider->actorInstance.position);
			PlaySample("splash.wav",255,0,0,collider->actorInstance.position,BSPLAYFLAG_IGNORE);
		}
	}
	return FALSE;
}

static void myStateChange(ACTORINSTANCE *actorInstance)
{
	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance,"drown",1.0f,0,0,0.1f,MOVE);
	actorInstance->characterInfo->powerUpInfo.drawBubbles = TRUE;

	/*if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].pickupsCollected < 100 && gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].difficulty == DIFF_HARD)
	{
		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].pickupsCollected)
		{
			if (gameStatus.currentScene < SCENE_MUSEUMVEHICLERACE)
			{
//				numPickupsRegen += 5;	// JW: Increase Number of pickups to respawn
				
				PlaySample("tweetydamage 2.wav", 255);
				
				ActivateSwirlOverlay(&gameStatus.player[0]);
			}
		}
	}*/
}

/* --------------------------------------------------------------------------------
   Function : RespondToWaterCollision
   Purpose : handles collision with water
   Parameters : ACTORINSTANCE *collider, ACTORINSTANCE *water
   Returns : 
   Info : 
*/
int RespondToWaterCollision(ACTORINSTANCE *collider, ACTORINSTANCE *water)
{
	if (collider->genericAI) 
	{
		AICollideWithSpecial(collider,water);
		return FALSE;
	}

	// TP: if taz has been captured by shark then ignore
	if ((collider->actorStatus)&&(collider->actorStatus->currentState==STATE_TRAPPED)) return FALSE;

	if(water->wateryObjectInfo->collision)
	{
		if(water->wateryObjectInfo->resetInfo)
		{
			if((!(collider->flags&ACTORFLAG_DEBRISACTOR))&&(collider->actorStatus->currentState != STATE_CATATONIC))
			{
				CalculateResetPoint(collider, water);
			}
		}
		water->wateryObjectInfo->collision = FALSE;
	}
	return FALSE;
}


/*  --------------------------------------------------------------------------------
	Function	: FindClosestResetPoint
	Purpose		: returns the position of the closest reset point
	Parameters	: water actor, point to test, vector to receive result, quat to receive result
	Returns		: index to reset objects, position, quaternion 
	Info		: 
*/

float FindClosestResetPoint(ACTORINSTANCE *water, TBVector point, TBVector closestPos, TBQuaternion closestQuat)
{
	float				previousBest = -1;
	int					counter;
	float				tempLen;
	int					retVal = 0;
	char				*tempName;
	ACTORINSTANCE		*tempInstance;
	TBVector			tempVector;

	for(counter = 0; counter < water->wateryObjectInfo->resetInfo->numObjects; counter++)
	{
		tempName = water->wateryObjectInfo->resetInfo->objectNames[counter];
		tempInstance = FindActorInstanceInInstanceList(tempName, 0, &map.invisibleInstances);
		if(!tempInstance) tempInstance = FindActorInstanceInInstanceList(tempName, 0, &map.collisionInstances);
		if(!tempInstance) tempInstance = FindActorInstanceInInstanceList(tempName, 0, &map.levelInstances);

		if(tempInstance)
		{
			bmVectorSub(tempVector, tempInstance->actorInstance.position, point);
			tempLen = bmVectorLen(tempVector);

			if((previousBest == -1) || (tempLen < previousBest))
			{
				previousBest = tempLen;
				retVal = counter;
				bmVectorCopy(closestPos, tempInstance->actorInstance.position);
				bmQuatCopy(closestQuat, tempInstance->actorInstance.orientation);
			}
		}
	}

	return previousBest;
}


/* --------------------------------------------------------------------------------
   Function : CalculateResetPoint
   Purpose : calculates reset point
   Parameters : ACTORINSTANCE *drowner, ACTORINSTANCE *water
   Returns : 
   Info : 
*/

void CalculateResetPoint(ACTORINSTANCE *drowner, ACTORINSTANCE *water)
{
	TBVector			resetPoint;
	TBVector			tempVector;
	TBVector			worldSpace, screenSpace;
	TBQuaternion		resetQuat;
	ACTORINSTANCE		*tempInstance;
	char				*tempName;
	float				tempLen;
	float				previousBest = -1;
	int					counter;
	TBActorNodeInstance	*node;
	
	//drowner->playerNo == 1	drowner->actorStatus->multiplayer.checkPoint = 1
	//checkpoint00reset1 = Reset point at checkpoint 0 for player 2

	//drowner->playerNo == 0	drowner->actorStatus->multiplayer.checkPoint = 5
	//checkpoint04reset0 = Reset point at checkpoint 4 for player 1
	tempInstance = NULL;
	if ((map.sceneNumber > SCENE_TAZBOSS) && gameStatus.multiplayer.numCheckPoints)
	{
		// JW: Find multiplayer reset points

		tempName = new char[32];
		
		if (drowner->actorStatus->multiplayer.checkPoint)
			sprintf(tempName, "checkpoint0%dreset%d", drowner->actorStatus->multiplayer.checkPoint - 1, drowner->playerNo + 1);
		else
			sprintf(tempName, "checkpoint0%dreset%d", gameStatus.multiplayer.numCheckPoints - 1, drowner->playerNo + 1);
		
		tempInstance = FindActorInstanceInInstanceList(tempName, 0, &map.invisibleInstances);
		if(!tempInstance) tempInstance = FindActorInstanceInInstanceList(tempName, 0, &map.collisionInstances);
		if(!tempInstance) tempInstance = FindActorInstanceInInstanceList(tempName, 0, &map.levelInstances);

		delete[] tempName;

		if (tempInstance)
		{
			bmVectorCopy(resetPoint, tempInstance->actorInstance.position);
			bmQuatCopy(resetQuat, tempInstance->actorInstance.orientation);
		}
	}
	
	// TP: Changed this to try and fix bug (412)
	if (RequestStateChange(drowner, STATE_CATATONIC))
	{
		if (UpdateCharacterState(&gameStatus.player[drowner->playerNo], true))
		{
			FlushAnimationQueue(drowner);
			PlayAnimationByName(drowner,"drown",1.0f,0,0,0.1f,MOVE);
			drowner->characterInfo->powerUpInfo.drawBubbles = TRUE;
		}
		else
		{
			ClearStateChangeRequest(drowner);
			// TP: must handle this better
		}
	}
	// CMD: whether we manage to go into state_catatonic or not we should always reset Taz

	if (!gameStatus.multiplayer.numCheckPoints || !tempInstance)
	{
		// TP: Needed this little bit somewhere else
		previousBest = FindClosestResetPoint(water, drowner->actorInstance.position, resetPoint, resetQuat);
	}
	
	if(previousBest > 0.0f || map.sceneNumber > SCENE_TAZBOSS)
	{
		//ResetActorAtSideOfWater(drowner, resetPoint);
		node = baFindNode(drowner->actorInstance.rootNodeInstance, "snoutstem");
		if(node)
		{
			if (drowner->playerNo!=-1)
			{
				gameStatus.player[drowner->playerNo].camera->fade.StartFadeDown(FADETYPE_CIRCLE, 1.5f);

				gameStatus.player[drowner->playerNo].camera->fade.SetPlayer(drowner);
				gameStatus.player[drowner->playerNo].camera->fade.SetFadeDownFinishedCallback(FinishedFadeFromWater);
				gameStatus.player[drowner->playerNo].camera->fade.SetFadeUpFinishedCallback(ResetFadeToPoint);
				bdSetIdentityObjectMatrix();
				baGetNodesWorldPosition(&drowner->actorInstance, node, worldSpace);
				worldSpace[3] = 1.0f;
				bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
				bdSetIdentityObjectMatrix();
				bdProjectVertices(screenSpace, worldSpace, 1);
				gameStatus.player[drowner->playerNo].camera->fade.SetCircleFadeTo(screenSpace[0], screenSpace[1]);
//					gameStatus.player[drowner->playerNo].camera->fade.fadeUpOnCompletion = TRUE;
				drowner->characterInfo->powerUpInfo.clock = 0.0f;
				// TP: Remove any power ups the character had
				KillAllPowerUps(drowner);
				bkPrintf("Taz drowning, fade down started\n");
			}
		}
	}

	
	if(characterSounds.taz == -1)
	{
		if (!(bmRand()%3))
			CallbackWhenSampleFinished((characterSounds.taz=PlaySample("tazdrown_1.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.taz);
	}
	
	bmVectorCopy(drowner->actorStatus->waterResetPoint, resetPoint);
	bmQuatCopy(drowner->actorStatus->waterResetQuaternion, resetQuat);
	//		ResetActorAtSideOfWaterWithCartoonScaling(drowner, resetPoint);
}

/* --------------------------------------------------------------------------------
   Function : ResetActorAtSideOfWater
   Purpose : resets actorInstance at designated point, nearest to water entry point
   Parameters : ACTORINSTANCE *drowner, ACTORINSTANCE *water
   Returns : 
   Info : 
*/

void ResetActorAtSideOfWater(ACTORINSTANCE *drowner, TBVector resetPoint)
{
	TBVector			initialVelocity;
	int					leadingColour[4] = {127, 127, 127, 127};
	int					trailingColour[4] = {127, 127, 127, 70};

	drowner->actorInstance.position[1] += METERTOUNIT(1.0f);

	CalculateProjectileInitialVelocity(initialVelocity, drowner->actorInstance.position, resetPoint, RAD(70.0f));

	FlushAnimationQueue(drowner);
	if(1)
	{
		PlayAnimationByName(drowner, "drown", 1.0f, TRUE, 0, 0.1f, NONE);
	}
	else
	{
		PlayAnimationByName(drowner, "fly1", 1.0f, TRUE, 0, 0.1f, NONE);
	}
	if(drowner->characterInfo)
	{
		drowner->characterInfo->powerUpInfo.clock = 0.0f;
	}

	RequestStateChange(drowner, STATE_KOIKFROMWATER);
	drowner->actorAnimation->useMotionBone = FALSE;

	drowner->actorBehaviour->physics.coll = 0;
	SetActorVelocity(drowner->actorBehaviour, initialVelocity);
//	CreateWaterRing(drowner->actorInstance.position, NULL, 2.0f);
	if(drowner->actorBehaviour->physics.velocity[1] < METERTOUNIT(-2.5f))
	{
		CreateWaterSplashes(drowner->actorInstance.position);
	}
	ResetCamera(gameStatus.player[drowner->playerNo].camera);
}

/* --------------------------------------------------------------------------------
   Function : ResetActorAtSideOfWaterWithCartoonScaling
   Purpose : resets actorInstance at designated point, nearest to water entry point
   Parameters : ACTORINSTANCE *drowner, ACTORINSTANCE *water
   Returns : 
   Info : 
*/

void ResetActorAtSideOfWaterWithCartoonScaling(ACTORINSTANCE *drowner, TBVector resetPoint, TBQuaternion resetQuat, EWaterType waterType)
{
	int					leadingColour[4] = {127, 127, 127, 127};
	int					trailingColour[4] = {127, 127, 127, 70};

	bkPrintf("Trying to reset taz at side\n");

	if (RequestStateChange(drowner, STATE_KOIKFROMWATER))
	{
		if (UpdateCharacterState(&gameStatus.player[drowner->playerNo], true))
		{
			if(drowner->characterInfo)
			{
				drowner->characterInfo->powerUpInfo.numFire = 0;
			}

			drowner->actorAnimation->useMotionBone = TRUE;
			if(waterType == WATERTYPE_WATER)
			{
				if(drowner->actorBehaviour->physics.velocity[1] < METERTOUNIT(-2.5f))
				{
					CreateWaterSplashes(drowner->actorInstance.position);
				}
			}

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
			bkPrintf("Taz entered KOIK state ok\n");
		}
		else
		{
			ClearStateChangeRequest(drowner);
			// TP: if the state change had failed the screen would never fade up (bug 412)
			gameStatus.player[drowner->playerNo].camera->fade.StartFadeUp(FADETYPE_NORMAL, 1.0f);
			bkPrintf("Taz failed to enter KOIK state ok\n");
		}
	}
	else
	{
		// TP: if the state change had failed the screen would never fade up (bug 412)
		gameStatus.player[drowner->playerNo].camera->fade.StartFadeUp(FADETYPE_NORMAL, 1.0f);
		bkPrintf("Taz failed to enter KOIK state ok\n");
	}
}

/* --------------------------------------------------------------------------------
   Function : FreeWaterResetPoints
   Purpose : frees water reset points
   Parameters : ACTORINSTANCELIST *actorInstanceList
   Returns : 
   Info : 
*/

//void FreeWaterResetPoints(ACTORINSTANCE *ptr)
void FreeWaterResetPoints(WATERRESETINFO *ptr)
{
	int				counter;
			
	for(counter = 0; counter < ptr->numObjects; counter++)
	{
		SAFE_FREE(ptr->objectNames[counter]);
	}
	SAFE_FREE(ptr->objectNames);
	SAFE_FREE(ptr);
}

/* --------------------------------------------------------------------------------
   Function : CreateWaterResetInfo
   Purpose : 
   Parameters : WATERRESETINFO *
   Returns : WATERRESETINFO *
   Info : 
*/

WATERRESETINFO *CreateWaterResetInfo(int numObjects)
{
	WATERRESETINFO *waterInfo;

	waterInfo = (WATERRESETINFO*)ZALLOC(sizeof(WATERRESETINFO));
	if(waterInfo)
	{
		waterInfo->objectNames = (char**)ZALLOC(numObjects*sizeof(char*));
		waterInfo->numObjects = numObjects;
		waterInfo->currentObject = 0;
		return waterInfo;
	}
	return NULL;
}

/* --------------------------------------------------------------------------------
   Function : AddWaterResetObject
   Purpose : 
   Parameters : WATERRESETINFO *, char *
   Returns : 
   Info : 
*/

int AddWaterResetObject(WATERRESETINFO *waterInfo, char*objectName)
{
	if (waterInfo->currentObject < waterInfo->numObjects)
	{
		waterInfo->objectNames[waterInfo->currentObject] = objectName;
		waterInfo->currentObject++;
		return TRUE;
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : FinishedFadeFromWater
   Purpose : 
   Parameters : FADE *
   Returns : 
   Info : 
*/

void FinishedFadeFromWater(class TAG_FADE *fade)
{ 	
	ACTORINSTANCE *pPlayer = fade->GetPlayer();

	ResetActorAtSideOfWaterWithCartoonScaling(pPlayer, pPlayer->actorStatus->waterResetPoint, pPlayer->actorStatus->waterResetQuaternion, 
										pPlayer->actorStatus->lastWaterType);
	
	// CMD: repsawn power ups
	RespawnAllPowerUps();

	fade->SetFadeDownFinishedCallback(NULL);
}

/* --------------------------------------------------------------------------------
   Function : ResetFadeToPoint
   Purpose : 
   Parameters : FADE *
   Returns : 
   Info : 
*/

void ResetFadeToPoint(class TAG_FADE *fade)
{
	fade->SetCircleFadeTo(0.0f, 0.0f);
}

/* --------------------------------------------------------------------------------
   Function : UpdateWater
   Purpose : 
   Parameters : FADE *
   Returns : 
   Info : 
*/

void UpdateWater(ACTORINSTANCE *actorInstance)
{
	if(actorInstance->special == SPECIALTYPE_WATERYOBJECT)
	{
		if((actorInstance->wateryObjectInfo->rippleClock += fTime) > actorInstance->wateryObjectInfo->rippleCreationTime)
		{
			actorInstance->wateryObjectInfo->rippleClock = 0.0f;
			if(waterRingList.numEntries < 50)
			{
				//temp
				if(gameStatus.currentScene == SCENE_AQUA)
				{
					CreateWaterRipple(actorInstance,waterWibble,METERTOUNIT(0.5f));
				}
			}
		}
	}
}