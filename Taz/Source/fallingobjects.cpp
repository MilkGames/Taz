// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : fallingobjects.cpp
//   Purpose : functions to calculate path of falling object
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"

#include "trigger.h"
#include "maths.h"
#include "animation.h"
#include "physics.h"
#include "collisioncache.h"
#include "scene.h"
#include "cartoonscaling.h"
#include "collectibles.h"
#include "status.h"
#include "fallingobjects.h"
#include "destructibles.h"
#include "breakables.h"
#include "timer.h"
#include "main.h"
#include "sfx.h"
#include "clouds.h"
#include "genericai.h"
#include "health.h"
#include "textures.h"
#include "shadow.h"

// PP: TEMP?: radius of Falling Object for shadow map
// PP: TODO? (how?): get bounds of actor instances on the fly so's to optimise shadow map resolution
#define SHADOWMAP_RADIUS_FALLINGOBJECT		METERTOUNIT(5.0f)// PP: ooh not too sure about this one

FALLINGOBJECTLIST		fallingObjectList;			//list of all objects waiting to fall

int RespondToActorCollision2(ACTORINSTANCE *actor, ACTORINSTANCE *collider, PRECOLLINFO *preCollInfo)
{
	return FALSE;
}
/* --------------------------------------------------------------------------------
   Function : InitFallingObjectList
   Purpose : guess what it does!
   Parameters : 
   Returns : 
   Info : 
*/

void InitFallingObjectList(void)
{
	fallingObjectList.numEntries = 0;
	fallingObjectList.head.next = fallingObjectList.head.prev = &fallingObjectList.head;
}

/* --------------------------------------------------------------------------------
   Function : CreateFallingObjectStructure
   Purpose : Function to store information about objects that are to fall at some point
   Parameters : 
   Returns : 
   Info : 
*/

void AddEntryToFallingObjectList(char *name, char *instanceName, TBVector startPos, TBVector scale, float areaRadius, int period, uint32 impactType, uint32 type, TRIGGERINFO *triggerInfo, DESTRUCTIBLEDEBRISINFO *debris, bool invisible, bool active)
{
	FALLINGOBJECTLISTENTRY		*newEntry;
	FALLINGOBJECTLISTENTRY		*tempPtr;

	tempPtr = fallingObjectList.head.next;

	//create new structure for incoming information
	if(newEntry = (FALLINGOBJECTLISTENTRY *)ZALLOC(sizeof(FALLINGOBJECTLISTENTRY)))
	{
		newEntry->nameCRC = bkCRC32((uchar*)name, strlen(name), 0);
		newEntry->instanceNameCRC = bkCRC32((uchar*)instanceName, strlen(instanceName), 0);
		bmVectorCopy(newEntry->startPos, startPos);
		
		if(type == FALLINGTYPE_WAIT) bmVectorCopy(newEntry->scale, scale);
		else bmVectorSet(newEntry->scale, 0.0f, 0.0f, 0.0f, 1.0f);

		newEntry->areaRadius = areaRadius;
		if((newEntry->period = period) == 0.0f)
		{
			bkPrintf("      *** WARNING *** falling object %s has a random time of zero, please note that this is not good\n", name);
		}
		newEntry->impactType = impactType;
		newEntry->type = type;
		newEntry->startTime = 0.0f;
		newEntry->next = NULL;
		newEntry->onDestruction = triggerInfo;
		newEntry->debris = debris;
		newEntry->initialHeight = GetHeightOfPoint(startPos,0,SPECIALTYPE_FALLINGOBJECT);
		newEntry->waitOver = FALSE;
		newEntry->invisible = invisible;
		newEntry->active = active;

		newEntry->next = tempPtr;
		newEntry->prev = tempPtr->prev;
		tempPtr->prev->next = newEntry;
		tempPtr->prev = newEntry;
		fallingObjectList.numEntries++;
	}
}

/*  --------------------------------------------------------------------------------
	Function	: SetFallingObjectState
	Purpose		: Sets the current state of a falling object generator
	Parameters	: crc of instance, true/false state
	Returns		: 
	Info		: 
*/
void SetFallingObjectState(uint32 crc, bool state)
{
	FALLINGOBJECTLISTENTRY  *ptr;

	for (ptr = fallingObjectList.head.next;ptr != &fallingObjectList.head;ptr=ptr->next)
	{
		if (ptr->instanceNameCRC==crc)
		{
			ptr->active = state;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : RemoveObjectFromFallingListByCRC
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/
int RemoveObjectFromFallingListByCRC(uint32 crc)
{
	FALLINGOBJECTLISTENTRY  *ptr;

	for (ptr = fallingObjectList.head.next;ptr != &fallingObjectList.head;ptr=ptr->next)
	{
		if (ptr->instanceNameCRC==crc)
		{
			RemoveObjectFromFallingList(ptr);
			return true;
		}
	}

	return false;
}

/* --------------------------------------------------------------------------------
   Function : RemoveObjectFromFallingList
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RemoveObjectFromFallingList(FALLINGOBJECTLISTENTRY *remPtr)
{
	FALLINGOBJECTLISTENTRY  *ptr, *temp;

	if(remPtr == NULL)
	{
		bkPrintf("**** ERROR **** pointer to be removed from falling list is NULL");
		return FALSE;
	}

	for (ptr = fallingObjectList.head.next;ptr != &fallingObjectList.head;)
	{
		if (ptr == remPtr)
		{
			if(ptr->onDestruction)
			{
				DeleteTriggerInfo(ptr->onDestruction);
				ptr->onDestruction = NULL;
			}
			if(ptr->debris)
			{
				FREE(ptr->debris);
				ptr->debris = NULL;
			}
			ptr->prev->next = ptr->next;
			ptr->next->prev = ptr->prev;
			fallingObjectList.numEntries--;
		
			temp = ptr->next;
			FREE(ptr);
			ptr = temp;
		}
		else
		{
			ptr = ptr->next;
		}
	}
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : UpdateFallingObjectList
   Purpose : updates falling objectlist and decides when to drop objects
   Parameters : FALLINGOBJECTLIST *list
   Returns : 
   Info : 
*/

void UpdateFallingObjectList(FALLINGOBJECTLIST *list)
{
	FALLINGOBJECTLISTENTRY	*ptr,*temp;
	//float					elapsedTime;			//measured in seconds
	float					xRadiusComp, zRadiusComp;
	int						radiusInUnits, xCompInUnits;
	TBVector				tempVector;
	ACTORINSTANCE			*fallingObjectInstance;
	
	if(fallingObjectList.head.next != NULL)
	{
		for (ptr = fallingObjectList.head.next;ptr != &fallingObjectList.head;)
		{
			temp = ptr->next;

			if (!ptr->active)
			{
				ptr = temp;
				continue;
			}

			switch(ptr->type)
			{
			case FALLINGTYPE_ONCE:
				break;
			case FALLINGTYPE_RANDOM:
				ptr->startTime += 1.0f/fps;
				//elapsedTime = (float)(bkTimerDelta(ptr->startTime, ptr->endTime)/bkTimerGetFrequency());
				
				if(ptr->startTime >= ptr->period)
				{
					ptr->startTime = 0.0f;
					if(ptr->areaRadius)
					{
						radiusInUnits = (int)METERTOUNIT(ptr->areaRadius);
						xCompInUnits = (bmRand()%(radiusInUnits*2))-radiusInUnits;
						//double check x componant
						xRadiusComp = (float)xCompInUnits;
						if(bmFAbs(xRadiusComp) > METERTOUNIT(ptr->areaRadius))
						{
							xRadiusComp = xRadiusComp*ptr->areaRadius/bmFAbs(xRadiusComp);
						}
						zRadiusComp = bmSqrt((METERTOUNIT(ptr->areaRadius)*METERTOUNIT(ptr->areaRadius)) - (xRadiusComp*xRadiusComp));
						if(bmRand()%2)
						{
							zRadiusComp = -zRadiusComp;
						}	
					}
					else
					{
						xRadiusComp = 0.0f;
						zRadiusComp = 0.0f;
					}
					tempVector[0] = ptr->startPos[0] + xRadiusComp;
					tempVector[1] = ptr->startPos[1];
					tempVector[2] = ptr->startPos[2] + zRadiusComp;
					tempVector[3] = ptr->startPos[3];
					fallingObjectInstance = DropObject(&map.fallingInstances, ptr->nameCRC, ptr->instanceNameCRC, 0, tempVector, ptr->type, ptr->impactType, ptr->onDestruction, ptr->debris, ptr->initialHeight, 1);
					if (fallingObjectInstance->actorShadow)
					{
						bmVectorCopy(fallingObjectInstance->actorInstance.scale, ptr->scale);
						fallingObjectInstance->actorShadow->flags &= ~SHADOWFLAG_COLLECTPOLYS;
					}

					//if (ptr->onDestruction) ptr->onDestruction = NULL;
				}
				break;
			case FALLINGTYPE_WAIT:
				fallingObjectInstance = DropObject(&map.fallingInstances, ptr->nameCRC, ptr->instanceNameCRC,0,ptr->startPos,ptr->type, ptr->impactType, ptr->onDestruction, ptr->debris, ptr->initialHeight, ptr->invisible);
				if (fallingObjectInstance->actorShadow)
				{
					bmVectorCopy(fallingObjectInstance->actorInstance.scale, ptr->scale);
					fallingObjectInstance->actorShadow->flags &= ~SHADOWFLAG_COLLECTPOLYS;
				}
				RemoveObjectFromFallingList(ptr);
				break;
			}

			ptr = temp;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : FreeAllFallingObjectsFromList
   Purpose : Removes all actors from the actor list and free the memory allocated completely
   Parameters : 
   Returns : 
   Info : 
*/
void FreeAllFallingObjectsFromList(void)
{
	FALLINGOBJECTLISTENTRY		  *ptr,*temp;

	bkPrintf("FreeAllFallingObjectsFromList\n");

	for (ptr = fallingObjectList.head.next;ptr != &fallingObjectList.head;)
	{
		if(ptr->onDestruction)
		{
			DeleteTriggerInfo(ptr->onDestruction);
			ptr->onDestruction = NULL;
		}
		if(ptr->debris)
		{
			SAFE_FREE(ptr->debris);
			ptr->debris = NULL;
		}
		ptr->prev->next = ptr->next;
		ptr->next->prev = ptr->prev;
		fallingObjectList.numEntries--;

		temp = ptr->next;
		SAFE_FREE(ptr);
		ptr = temp;
	}
}

/* --------------------------------------------------------------------------------
   Function : DropObject
   Purpose : creates instance to be dropped
   Parameters : name of object, start position
   Returns : 
   Info : 
*/

ACTORINSTANCE *DropObject(ACTORINSTANCELIST *instanceList, uint32 nameCRC, uint32 instanceNameCRC, char *name, TBVector startPos, uint32 type, uint32 impactType, TRIGGERINFO *onDestruction, DESTRUCTIBLEDEBRISINFO *debris, float initHeight,bool invisible, float timeBeforeFade, ACTORINSTANCE *owner, int flags)
{
	ACTORINSTANCE		*fallingObjectInstance;
	TBActor				*actor;
	int					maxAngle = 60;
	TBVector			xAxis = {1.0f, 0.0f, 0.0f, 0.0f};
	TBVector			yAxis = {0.0f, 1.0f, 0.0f, 0.0f};
	TBVector			zAxis = {0.0f, 0.0f, 1.0f, 0.0f};
	TBVector			zeroVec = {0.0f, 0.0f, 0.0f, 0.0f};
	TBVector			scale;
	TBQuaternion		quatOne, quatTwo, quatThree;

	if(nameCRC == 0)
	{
		fallingObjectInstance = CreateActorInstance(instanceList, name, "falling");
	}
	else
	{
		if(actor = FindActorInActorList(nameCRC))
		{
			fallingObjectInstance = CreateActorInstanceFromActorPtr(instanceList, actor, nameCRC, "falling");
			fallingObjectInstance->instanceCRC = instanceNameCRC;
		}
		else return NULL;
	}
	if(!fallingObjectInstance) return NULL;

	CreateActorInstanceBehaviour( fallingObjectInstance );
	CreateFallingObjectInstance( fallingObjectInstance, startPos );
	if(type == FALLINGTYPE_WAIT)
	{
		SETVECTOR(scale, 1.0f, 1.0f, 1.0f, 1.0f);
		bmVectorCopy(fallingObjectInstance->actorInstance.scale, scale);
		fallingObjectInstance->fallingObjectInfo->cartoonScaleInfo = NULL;
		if(invisible == true)
		{
			fallingObjectInstance->flags |= ACTORFLAG_DONTDRAW;
		}
	}
	else
	{
		fallingObjectInstance->fallingObjectInfo->cartoonScaleInfo = CreateCartoonScalingInfo();
		fallingObjectInstance->fallingObjectInfo->cartoonScaleInfo->type = SCALETYPE_UP;
		fallingObjectInstance->fallingObjectInfo->cartoonScaleInfo->stage = CARTOONSCALE_STAGEONE;
		fallingObjectInstance->fallingObjectInfo->cartoonScaleInfo->time = 0.0f;
		bmVectorCopy(fallingObjectInstance->actorInstance.scale, zeroVec);
	}
	CreateActorInstanceShadow(fallingObjectInstance, fallingObjectInstance->actorInstance.actor->maxRadius, initHeight*2.0f, SHADOWMAP_RADIUS_FALLINGOBJECT);
	
	fallingObjectInstance->fallingObjectInfo->flags = flags;
	fallingObjectInstance->fallingObjectInfo->owner = owner;
	
	fallingObjectInstance->fallingObjectInfo->onDestruction = CopyTriggerInfo(onDestruction);
	fallingObjectInstance->fallingObjectInfo->debris = CopyDestructibleDebrisInfo(debris);
	fallingObjectInstance->fallingObjectInfo->type = type;
	fallingObjectInstance->fallingObjectInfo->instanceNameCRC = instanceNameCRC;
	fallingObjectInstance->fallingObjectInfo->waitOver = FALSE;
	fallingObjectInstance->fallingObjectInfo->collision = FALSE;
	fallingObjectInstance->fallingObjectInfo->impactType = impactType;

	fallingObjectInstance->fallingObjectInfo->initialHeight = initHeight;
	switch(impactType)
	{
	case FALLINGTYPE_REMAIN:
	case FALLINGTYPE_REMAINANDFADE:
		fallingObjectInstance->fallingObjectInfo->destroy = FALSE;
		fallingObjectInstance->fallingObjectInfo->timeBeforeFade = timeBeforeFade;
		fallingObjectInstance->fallingObjectInfo->convertTo = (ESpecialType)-1;
		break;
	case FALLINGTYPE_REMAINTODESTRUCTIBLE:
		fallingObjectInstance->fallingObjectInfo->destroy = FALSE;
		fallingObjectInstance->fallingObjectInfo->convertTo = SPECIALTYPE_DESTRUCTIBLE;
		break;
	case FALLINGTYPE_REMAINTOSPARKY:
		fallingObjectInstance->fallingObjectInfo->destroy = FALSE;
		fallingObjectInstance->fallingObjectInfo->convertTo = SPECIALTYPE_ZOOKEEPER;			// CMD: sparky is also destructible, use this instead
		break;
	default:
		fallingObjectInstance->fallingObjectInfo->destroy = TRUE;
		fallingObjectInstance->fallingObjectInfo->convertTo = (ESpecialType)-1;
		break;
	}
//	if((impactType == FALLINGTYPE_REMAIN)||(impactType == FALLINGTYPE_REMAINANDFADE))
//	{
//		fallingObjectInstance->fallingObjectInfo->destroy = FALSE;
//		fallingObjectInstance->fallingObjectInfo->timeBeforeFade = timeBeforeFade;
//		fallingObjectInstance->fallingObjectInfo->convertTo = (ESpecialType)-1;
//	}
//	else
	if(/*(impactType == FALLINGTYPE_REMAINTODESTRUCTIBLE)||*/(gameStatus.multiplayer.numPlayers > 1))
	{
		fallingObjectInstance->fallingObjectInfo->destroy = FALSE;
		fallingObjectInstance->fallingObjectInfo->convertTo = SPECIALTYPE_DESTRUCTIBLE;
	}
//	else
//	{
//		fallingObjectInstance->fallingObjectInfo->destroy = TRUE;
//		fallingObjectInstance->fallingObjectInfo->convertTo = (ESpecialType)-1;
//	}

	//set orientation randomly
	xAxis[3] = RAD((float)((bmRand()%maxAngle)-(maxAngle/2)));
	yAxis[3] = RAD((float)(bmRand()%360));
	zAxis[3] = RAD((float)((bmRand()%maxAngle)-(maxAngle/2)));
	//rotate arround y, then x, then z
	bmRotationToQuat(quatOne, yAxis);
	bmRotationToQuat(quatTwo, xAxis);
	bmQuatMultiply(quatThree, quatOne, quatTwo);
	bmRotationToQuat(quatOne, zAxis);
	bmQuatMultiply(quatTwo, quatThree, quatOne);

	SetActorOrientation(fallingObjectInstance, quatTwo);

	SetActorPosition( fallingObjectInstance, fallingObjectInstance->fallingObjectInfo->startPos );
	AddActorToCollisionCache(&collisionCache,fallingObjectInstance, CACHEFLAGS_LIVEUPDATE);
	SetPreCollisionCallback(fallingObjectInstance,RespondToActorCollision2);
	SetCollisionCallback(fallingObjectInstance,RespondToActorCollision);

	if (fallingObjectInstance->actorShadow)
	{
		CreateShadow(fallingObjectInstance, &collisionCache, fallingObjectInstance->actorInstance.position);
	}

	return fallingObjectInstance;
}

/* --------------------------------------------------------------------------------
   Function : CreateActorInstancePathInfo
   Purpose : Creates and initialises the path info part of an actorInstance
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
int CreateFallingObjectInstance(ACTORINSTANCE *actorInstance, TBVector startPos)
{
	actorInstance->fallingObjectInfo = (FALLINGOBJECTINFO*)ZALLOC(sizeof(FALLINGOBJECTINFO));
	if (actorInstance->fallingObjectInfo)
	{
		actorInstance->special = SPECIALTYPE_FALLINGOBJECT;
		actorInstance->fallingObjectInfo->startPos[0] = startPos[0];
		actorInstance->fallingObjectInfo->startPos[1] = startPos[1];
		actorInstance->fallingObjectInfo->startPos[2] = startPos[2];
		actorInstance->fallingObjectInfo->startPos[3] = 1.0f;

		return TRUE;
	}
	else
	{
		bkPrintf("*** ERROR *** Could not create falling object info structure\n");
		return FALSE;
	}
}

/* --------------------------------------------------------------------------------
Function : UpdateFallingObject
Purpose : updates position and velocity of falling object
Parameters : actor instance list ptr
Returns : 
Info : 
*/

void UpdateFallingObject(ACTORINSTANCE *ptr)
{
	TBVector		pos;
	char			message[256];
	bool			shake = false;

	if(ptr->fallingObjectInfo)
	{
		if (ptr->fallingObjectInfo->impactType==FALLINGTYPE_WAITINGFORFADE)
		{
			if ((ptr->fallingObjectInfo->timeBeforeFade-=fTime) <= 0.0f)
			{
				// TP: start fade out
				SAFE_FREE(ptr->fallingObjectInfo);
				ptr->special = (ESpecialType)-1;

				ptr->fadeSpeed = -128.0f;
				ptr->flags |= ACTORFLAG_DELETEAFTERFADEOUT|ACTORFLAG_FADEAFTERANIM;
			}
		}
		else
		{
			// TP: Argggg my balls never drop, maybe some range check needed
			if( ptr->fallingObjectInfo->initialHeight > (ptr->fallingObjectInfo->startPos[1] - ptr->actorInstance.position[1]) )
			{
				if(ptr->fallingObjectInfo->type == FALLINGTYPE_WAIT)
				{
					if(ptr->fallingObjectInfo->waitOver)
					{
						UpdateFallingObjectInfo(ptr);
					}
				}
				else
				{
					UpdateCartoonScaling(ptr, ptr->fallingObjectInfo->cartoonScaleInfo);
				}
			}
			else
			{


				// TP: object has hit ground
				ptr->actorInstance.position[1] = ptr->fallingObjectInfo->startPos[1] - ptr->fallingObjectInfo->initialHeight;
				baPrepActorInstance(&ptr->actorInstance, TRUE);

				// TP: stop any more calcs on the shadows
				if (ptr->actorShadow)
				{
					ptr->actorShadow->flags |= SHADOWFLAG_DRAWONLY;
				}
				
				// CMD:  send messages whether or not the object is destroyed
				SetSoundPos(ptr->actorInstance.position);
				if (ptr->fallingObjectInfo->onDestruction)
				{
					SendTrigger(ptr->fallingObjectInfo->onDestruction);
					DeleteTriggerInfo(ptr->fallingObjectInfo->onDestruction);
					ptr->fallingObjectInfo->onDestruction = NULL;
				}

				if(ptr->fallingObjectInfo->destroy)
				{
					if(ptr->fallingObjectInfo->cartoonScaleInfo)
					{
						SAFE_FREE(ptr->fallingObjectInfo->cartoonScaleInfo);
						ptr->fallingObjectInfo->cartoonScaleInfo = NULL;
					}
					bmVectorCopy(pos,ptr->actorInstance.position);

					DestroyObject(ptr, ptr->fallingObjectInfo->debris, TRUE);
					shake = true;
				}
				else
				{
					if(ptr->fallingObjectInfo->collision == FALSE)
					{
						ResetRemainTypeFallingObject(NULL, ptr);
						shake = true;
					}
				}

				// TP: shake camera
				if (shake)
				{
					for (int i=0;i<gameStatus.multiplayer.numPlayers;i++)
					{
						TBVector	tempVector;
						float		tempFloat;

						if ((gameStatus.player[i].actorInstance)&&(gameStatus.player[i].camera))
						{
							// TP: check distance between player and falling object
							bmVectorSub(tempVector,gameStatus.player[i].actorInstance->actorInstance.position,ptr->actorInstance.position);
							tempFloat = bmVectorLen(tempVector);
							if (tempFloat < METERTOUNIT(10.0f))
							{
								gameStatus.player[i].camera->quakeOffsetLevel = 0.02f;
								gameStatus.player[i].camera->tiltOffsetLevel = RAD(4.0f);
								gameStatus.player[i].camera->flags |= CAMERA_QUAKEFADE;
							}
						}
					}
				}
				//PlaySample("hitground 2-1.wav",255,ptr->actorInstance.position);
			}
		}
	}
}

/* --------------------------------------------------------------------------------
Function : UpdateFallingObjectInfo
Purpose : updates position and velocity of falling object
Parameters : actor instance ptr
Returns : 
Info : 
*/

void UpdateFallingObjectInfo(ACTORINSTANCE *actorInstance)
{
	TBVector		positionInc;
	float			gravity, maxaccel;

	if (map.sceneNumber > SCENE_DEPTSTRVEHICLERACE)
	{
		gravity = METERTOUNIT(-5.0f);
		maxaccel = METERTOUNIT(2);
	}
	else
	{
		gravity = METERTOUNIT(-40.0f);
		maxaccel = METERTOUNIT(15);
	}

	if( ApplyGravity(positionInc, actorInstance->actorBehaviour->physics.velocity, gravity/*worldGravity*/ ) )
	{
		actorInstance->actorInstance.position[1] += positionInc[1];
		checkSpeedLimit( &actorInstance->actorBehaviour->physics, maxaccel);
		baPrepActorInstance(&actorInstance->actorInstance,TRUE);
	}
}

/* --------------------------------------------------------------------------------
   Function : StateChange
   Purpose : Function callback for 
   Parameters : ACTORINSTANCE - character causing callback
   Returns : 
   Info : 
*/
static void StateChange(ACTORINSTANCE *actorInstance)
{
	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance,"recover22c",1.0f,0,1,0.1f,REACT);
	PlaySample("taz_gen_10a.wav",255);
}

/* --------------------------------------------------------------------------------
   Function : RespondToActorCollision
   Purpose : Function to be called when a collision with an actor has occured
   Parameters : ACTORINSTANCE *actor in collision, ACTORINSTANCE *object hitting actor
   Returns : 
   Info : 
*/
int RespondToActorCollision(ACTORINSTANCE *actor, ACTORINSTANCE *collider)
{
	bool	objectKilled = false;

	if (actor->genericAI) 
	{
		AICollideWithSpecial(actor,collider);
		return FALSE;
	}

	if (actor->flags&ACTORFLAG_DEBRISACTOR) return FALSE;

	if((collider->fallingObjectInfo->type == FALLINGTYPE_WAIT)&&
		(collider->fallingObjectInfo->waitOver == FALSE)) return FALSE;

	PlaySample("taz_gen_09a.wav",255);
	PlaySample("poich!.wav",255);			// PP: poich!

	if(collider->fallingObjectInfo->destroy == FALSE)
	{
		collider->fallingObjectInfo->collision = TRUE;
		StopCollidingWith(actor, collider);
		SetOutsideBoundingBoxCallback(collider,ResetRemainTypeFallingObject);
		ReduceCharactersHealth(actor,1);
		if (EnterState(actor, STATE_MOVESQUASHED))
		{
			FlushAnimationQueue(actor);
			PlayAnimationByName(actor, "recover21a",1.0f, 1, 0, 0.1f, REACT);
		}

		if (collider->fallingObjectInfo->flags&FALLINGOBJECTFLAG_CAPTURETAZ)
		{
			if (collider->fallingObjectInfo->owner)
			{
				CaptureTaz(collider->fallingObjectInfo->owner);
			}
		}

		if (map.sceneNumber == SCENE_DESTRUCTIONAQUA ||
			map.sceneNumber == SCENE_DESTRUCTIONCONSTRUCT ||
			map.sceneNumber == SCENE_DESTRUCTIONTASMANIA)
		{
			ResetRemainTypeFallingObject(actor, collider);
			objectKilled = true;
		}
	}
	else
	{
		StopCollidingWith(actor, collider);
		ReduceCharactersHealth(actor,1.0f);
		RequestStateChangePlus(actor, STATE_RECOVER, StateChange);
	}
	for (int i=0;i<gameStatus.multiplayer.numPlayers;i++)
	{
		TBVector	tempVector;
		float		tempFloat;

		if ((gameStatus.player[i].actorInstance)&&(gameStatus.player[i].camera))
		{
			// TP: check distance between player and falling object
			bmVectorSub(tempVector,gameStatus.player[i].actorInstance->actorInstance.position,collider->actorInstance.position);
			tempFloat = bmVectorLen(tempVector);
			if (tempFloat < METERTOUNIT(10.0f))
			{
				gameStatus.player[i].camera->quakeOffsetLevel = 0.02f;
				gameStatus.player[i].camera->tiltOffsetLevel = RAD(4.0f);
				gameStatus.player[i].camera->flags |= CAMERA_QUAKEFADE;
			}
		}
	}
	return objectKilled;
}

/* --------------------------------------------------------------------------------
   Function : RespondToActorCollision
   Purpose : Function to be called when a collision with an actor has occured
   Parameters : ACTORINSTANCE *actor in collision, ACTORINSTANCE *object hitting actor
   Returns : 
   Info : 
*/

void FreeAllFallingObjectScaleInfo(ACTORINSTANCELIST *instanceList)
{
	ACTORINSTANCE *ptr;

	for (ptr = instanceList->head.next;ptr != &instanceList->head;ptr = ptr->next)
	{
		if(ptr->fallingObjectInfo)
		{
			if(ptr->special == SPECIALTYPE_FALLINGOBJECT)
			{
				if(ptr->fallingObjectInfo->cartoonScaleInfo)
				{
					SAFE_FREE(ptr->fallingObjectInfo->cartoonScaleInfo);
				}
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : ResetRemainTypeFallingObject
   Purpose : Function to be called when a collision with an actor has occured
   Parameters : ACTORINSTANCE *actor in collision, ACTORINSTANCE *object hitting actor
   Returns : 
   Info : 
*/

int ResetRemainTypeFallingObject(ACTORINSTANCE *actorInstance, ACTORINSTANCE *collider)
{
	COLLISIONLISTENTRY		*collEntry;
	DESTRUCTIBLEDEBRISINFO	debris;
	int					convertTo=-1;
	int					i;

	switch(collider->special)
	{
	case SPECIALTYPE_FALLINGOBJECT:

		//sort out Taz
		if(actorInstance)
		{
			actorInstance->flags |= ACTORFLAG_ALLOWUNSQUASHMOVE;
		}

		convertTo = collider->fallingObjectInfo->convertTo;
		if(collider->fallingObjectInfo->debris)
		{
			for(i = debris.numDebris = collider->fallingObjectInfo->debris->numDebris;i >= 0;i--)
			{
				debris.debrisCRC[i] = collider->fallingObjectInfo->debris->debrisCRC[i];
			}
		}
		else
		{
			debris.numDebris = 0;
		}

		//sort out falling object
		if(collider->fallingObjectInfo)
		{
			//undo live update
			collEntry = GetCollisionEntry(&collisionCache, collider);
			if (collEntry)
			{
				MakeEntryDormant(&collisionCache, collEntry);
				MakeEntryResident(&collisionCache, collEntry);
				collEntry->flags &= ~CACHEFLAGS_LIVEUPDATE;
			}

			// TP: remove cartoon scaling info
			if(collider->fallingObjectInfo->cartoonScaleInfo)
			{
				SAFE_FREE(collider->fallingObjectInfo->cartoonScaleInfo);
			}

			// TP: remove destruction triggers
			if(collider->fallingObjectInfo->onDestruction)
			{
				DeleteTriggerInfo(collider->fallingObjectInfo->onDestruction);
				collider->fallingObjectInfo->onDestruction = NULL;
			}

			// TP: remove debris
			if(collider->fallingObjectInfo->debris)
			{
				SAFE_FREE(collider->fallingObjectInfo->debris);
			}

			// TP: free falling object structure if not the remain and fade type
			if (collider->fallingObjectInfo->impactType!=FALLINGTYPE_REMAINANDFADE)
			{
				SAFE_FREE(collider->fallingObjectInfo);
			}
			else
			{
				collider->fallingObjectInfo->impactType = FALLINGTYPE_WAITINGFORFADE;
			}
		}

		collider->RespondToCollisionRoutine = NULL;
		collider->PreRespondToCollisionRoutine = NULL;

		// CMD: now need to be able to convert to many different types
		switch(convertTo)
		{
		case SPECIALTYPE_DESTRUCTIBLE:
			if (map.sceneNumber == SCENE_DESTRUCTIONAQUA || map.sceneNumber == SCENE_DESTRUCTIONCONSTRUCT || map.sceneNumber == SCENE_DESTRUCTIONTASMANIA)
			{
				if (!strcmp(collider->actorName, "speed clock.obe"))
					CreateCollectibleActor(collider->actorName, collider->actorInstance.position, 0, (int)(METERTOUNIT(0.5f)), COLLECTIBLETYPE_DE_EXTRATIME, false, 0);
				else if (!strcmp(collider->actorName, "mplayer box.obe"))
					CreateCollectibleActor(collider->actorName, collider->actorInstance.position, 0, (int)(METERTOUNIT(0.5f)), COLLECTIBLETYPE_DE_DOUBLECRATE, false, 0);
				else
					CreateCollectibleActor(collider->actorName, collider->actorInstance.position, 0, (int)(METERTOUNIT(0.5f)), COLLECTIBLETYPE_DE_SLOMO, false, 0);

				FreeActorInstance(collider);

				break;
			}
			else
			{
				CreateDestructibleObjectInstance(collider, DESTRUCTIBLETYPE_SPIN, NULL, 
											 0, 0, 0, 0, 0, 0, 
											 0.0f, 0.0f, 0.0f, 0, 100, 0);
			}
			
			if(collider->destructibleObjectInfo)
			{
				collider->destructibleObjectInfo->debris = NULL;
				collider->destructibleObjectInfo->sparkInfo = NULL;
				collider->destructibleObjectInfo->hitTriggers = NULL;
				collider->destructibleObjectInfo->explosion = NULL;

				if(debris.numDebris)
				{
					collider->destructibleObjectInfo->debris = (DESTRUCTIBLEDEBRISINFO*)ZALLOC(sizeof(DESTRUCTIBLEDEBRISINFO));
					ASSERTM(collider->destructibleObjectInfo->debris, "out of memory");
					for(i = collider->destructibleObjectInfo->debris->numDebris = debris.numDebris;i >= 0;i--)
					{
						collider->destructibleObjectInfo->debris->debrisCRC[i] = debris.debrisCRC[i];
					}
				}
			}
			if (gameStatus.multiplayer.numPlayers == 2)
			{
				collider->actorInstance.orientation[Z] = 0.0f;
				collider->actorInstance.orientation[X] = 0.0f;
			}
			break;
		case SPECIALTYPE_ZOOKEEPER:
			CreateDestructibleObjectInstance(collider, DESTRUCTIBLETYPE_SPARK, NULL, 0, 0, 0, 0, 0, NULL, 0.0f, 0.0f, 
												0.0f, 0, 0, 0, 0, 0.0f);
			if(collider->destructibleObjectInfo)
			{
				collider->destructibleObjectInfo->debris = NULL;
				collider->destructibleObjectInfo->hitTriggers = NULL;
				collider->destructibleObjectInfo->explosion = NULL;

				collider->destructibleObjectInfo->sparkInfo = (SPARKINFO*)ZALLOC(sizeof(SPARKINFO));
				ASSERTM(collider->destructibleObjectInfo->sparkInfo, "out of memory");
				collider->destructibleObjectInfo->sparkInfo->numTextures = -1;
				collider->destructibleObjectInfo->sparkInfo->sparkTexture = (TBTexture**)ZALLOC(sizeof(TBTexture*));
				ASSERTM(collider->destructibleObjectInfo->sparkInfo->sparkTexture, "out of memory");
				collider->destructibleObjectInfo->sparkInfo->sparkTexture[0] = textureSpark;
				collider->destructibleObjectInfo->sparkInfo->sparkTextureCRC = NULL;
			}
			break;
		default:
			// TP: if not waiting for fade then set type to normal otherwise keep object as falling
			if (!((collider->fallingObjectInfo)&&(collider->fallingObjectInfo->impactType==FALLINGTYPE_WAITINGFORFADE)))
			{
				collider->special = (ESpecialType)-1;
			}
			break;
		}

	default:
		return FALSE;
		break;
	}
}