// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : trigger.cpp
//   Purpose : trigger functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "collisioncache.h"
#include "scene.h"
#include "transporter.h"
#include "status.h"
#include "maths.h"
#include "mapfile.h"
#include "trigger.h"
#include "characters.h"
#include "costumes.h"
#include "main.h"
#include "sfx.h"
#include "genericai.h"
#include "animation.h"
#include "transportereffect.h"

/* --------------------------------------------------------------------------------
   Function : CreateTransporter
   Purpose : Creates and initialises actorInstances that act as transporters
   Parameters : transportTo is CRC of object to transport to, name is for debug purposes only
   Returns : 
   Info : 
*/

int CreateTransporter(ACTORINSTANCE *transporter, uint32 transportTo, char *name, float xOffset, float yOffset, float zOffset, float delay, bool active, bool collision, uint32 fadeUp, uint32 fadeDown)
{
	if(!CreateTransporterInfo(transporter)) return FALSE;

	//set attributes
	transporter->special = SPECIALTYPE_TRANSPORTER;
	if(!(transporter->transporterInfo->transportTo = transportTo))
	{
		bkPrintf("      *** MOSES *** Transporter %s has no destination, pull your finger out!!!\n", name);
	}

	transporter->transporterInfo->xOffset = xOffset;
	transporter->transporterInfo->yOffset = yOffset;
	transporter->transporterInfo->zOffset = zOffset;
	transporter->transporterInfo->delay = delay;
	transporter->transporterInfo->active = active;
	transporter->transporterInfo->fadeUp = fadeUp;
	transporter->transporterInfo->fadeDown = fadeDown;
	transporter->transporterInfo->onComplete = NULL;
	transporter->transporterInfo->onContact = NULL;
	transporter->transporterInfo->onReachingDestination = NULL;

	if (collision)
	{
		AddActorToCollisionCache(&collisionCache,transporter, 0);
	}
	SetCollisionCallback(transporter,RespondToTransporterCollision);

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : CreateTransporterInfo
   Purpose : Creates and initialises transporter info part of object
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int CreateTransporterInfo(ACTORINSTANCE *actorInstance)
{
	actorInstance->transporterInfo = (TRANSPORTERINFO*)ZALLOC(sizeof(TRANSPORTERINFO));
	if(actorInstance->transporterInfo)
	{
		return TRUE;
	}
	else
	{
		bkPrintf("*** ERROR *** Could not create transporter object info structure\n");
		return FALSE;
	}
}

static void myStateChange(ACTORINSTANCE *actorInstance)
{
	if(actorInstance->actorStatus->currentState == STATE_BALL)
	{
		if(actorInstance->characterInfo->costume != -1)
		{
			SetupCostume(actorInstance, actorInstance->characterInfo->costume);
		}
		else
		{
			ChangeActorInstance(actorInstance,FindActorInActorList("tazdevil.obe"),5,NULL);
		}
	}
	SETVECTOR(actorInstance->actorInstance.scale, 0.0f, 0.0f, 0.0f,1.0f);
}

static void myOtherStateChange(ACTORINSTANCE *actorInstance)
{
	if(actorInstance->actorStatus->currentState == STATE_BALL)
	{
		if(actorInstance->characterInfo->costume != -1)
		{
			SetupCostume(actorInstance, actorInstance->characterInfo->costume);
		}
		else
		{
			ChangeActorInstance(actorInstance,FindActorInActorList("tazdevil.obe"),5,NULL);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : RespondToTransporterCollision
   Purpose : calculates response when an actor collides
   Parameters : ACTORINSTANCE *spring, ACTORINSTANCE *collider
   Returns : 
   Info : 
*/

int RespondToTransporterCollision(ACTORINSTANCE *collider, ACTORINSTANCE *transporter)
{
	ACTORINSTANCE	*tempInstance;
	TBVector		tempVector;
	char			message[256];
	TBVector		pos, vel;

	if (!transporter->transporterInfo->active) return FALSE;

	if (collider->genericAI) 
	{
		AICollideWithSpecial(collider,transporter);
		return TRUE;
	}

	if (collider->flags&ACTORFLAG_DEBRISACTOR) return FALSE;

	if(!collider->characterInfo) return FALSE;
	if(collider->characterInfo->transportInfo) return FALSE;

	if (!(bmRand()%3))
		PlaySample("tazteleport_1.wav", 255, collider->actorInstance.position);

	bmVectorCopy(pos,transporter->actorInstance.position);
	
	SetSoundPos(pos);
	if(characterSounds.taz == -1)
	{
		CallbackWhenSampleFinished((characterSounds.taz=PlaySample("taz_voc_09c.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.taz);
	}

	tempInstance = FindActorInstanceInMap(NULL,transporter->transporterInfo->transportTo, &map);
	if(tempInstance)
	{
		tempVector[0] = tempInstance->actorInstance.position[0] + transporter->transporterInfo->xOffset;
		tempVector[1] = tempInstance->actorInstance.position[1] + transporter->transporterInfo->yOffset;
		tempVector[2] = tempInstance->actorInstance.position[2] + transporter->transporterInfo->zOffset;

		// CMD: set velocity to zero
		bmVectorSet(vel, 0.0f, 0.0f, 0.0f, 1.0f);
		SetActorVelocity(collider->actorBehaviour, vel);

		// CMD: onContact trigger is ALWAYS sent when contact made
		if (transporter->transporterInfo->onContact)
				SendTrigger(transporter->transporterInfo->onContact);

		if(transporter->transporterInfo->fadeDown)
		{
			EnableCharacterTransporterMode(collider, METERTOUNIT(1.0f), METERTOUNIT(0.5f), transporter->transporterInfo->fadeUp);
			SetCharacterTransportToPosition(collider, tempVector, tempInstance->actorInstance.orientation);
			collider->characterInfo->transportInfo->delay = transporter->transporterInfo->delay;

			// CMD: copy onComplete trigger to be sent later
			if(transporter->transporterInfo->onComplete)
			{
				if(collider->characterInfo->transportInfo->onCompletion)
				{
					DeleteTriggerInfo(collider->characterInfo->transportInfo->onCompletion);
					collider->characterInfo->transportInfo->onCompletion = NULL;
				}
				collider->characterInfo->transportInfo->onCompletion = CopyTriggerInfo(transporter->transporterInfo->onComplete);
			}
			if(transporter->transporterInfo->onReachingDestination)
			{
				if(collider->characterInfo->transportInfo->onReachingDestination)
				{
					DeleteTriggerInfo(collider->characterInfo->transportInfo->onReachingDestination);
					collider->characterInfo->transportInfo->onReachingDestination = NULL;
				}
				collider->characterInfo->transportInfo->onReachingDestination = CopyTriggerInfo(transporter->transporterInfo->onReachingDestination);
			}
			FlushAnimationQueue(collider);
			PlayAnimationByName(collider, "idle1", 0.5f, 1, 0, 0.1f, IDLE);
			RequestStateChangePlus(collider, STATE_INTRANSPORT, myOtherStateChange);

			PlaySample("teleport.wav", 255, transporter->actorInstance.position);
			return FALSE;
		}
		else
		{
			SetActorPosition(collider, tempVector);

			if(transporter->transporterInfo->delay)
			{
				// TP: CLAUDE PLEASE DONT REMOVE THIS! Can't do this here in case player never enters transport and we then be zero scale for ever(bug 415) SETVECTOR(collider->actorInstance.scale, 0.0f, 0.0f, 0.0f,1.0f);
				collider->actorStatus->catatonicDelay = transporter->transporterInfo->delay;
				if (collider->actorStatus->postCatatonicTrigger)
				{
					// TP: just in case
					bkPrintf("*** WARNING *** Collider already had a postCatatonicTrigger\n");
					DeleteTriggerInfo(collider->actorStatus->postCatatonicTrigger);
					collider->actorStatus->postCatatonicTrigger = NULL;
				}
				collider->actorStatus->postCatatonicTrigger = CopyTriggerInfo(transporter->transporterInfo->onComplete);
				RequestStateChangePlus(collider, STATE_INTRANSPORT, myStateChange);
				return FALSE;
			}
		}

		RequestStateChange(collider, STATE_MOVE);
	}
	return FALSE;
}
