// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : steppingstone.cpp
//   Purpose : functions related to stepping stone objects
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "collisioncache.h"
#include "physics.h"
#include "status.h"
#include "scene.h"
#include "maths.h"
#include "steppingstone.h"
#include "genericai.h"


/* --------------------------------------------------------------------------------
   Function : CreateSteppingStoneObject
   Purpose : Initialises actorInstances that act as stepping stones
   Parameters : ACTORINSTANCE *, height of sink
   Returns : 
   Info : 
*/

void CreateSteppingStoneObject(ACTORINSTANCE *steppingStone, float heightOfSink, TBVector initPosition, float bobbleTime, float sinkSpeed, float bobbleHeight)
{
	CreateActorInstanceBehaviour( steppingStone );
	CreateSteppingStoneActorInstanceInfo( steppingStone );
	
	//fill in attributes
	steppingStone->steppingStoneObjectInfo->type = SPECIALTYPE_STEPPINGSTONE;
	steppingStone->special = SPECIALTYPE_STEPPINGSTONE;
	if((steppingStone->steppingStoneObjectInfo->heightOfSink = heightOfSink) < 0.0f)
	{
		bkPrintf("      *** WARNING *** negative sink height for stepping stone %s\n", steppingStone->actorName);
	}
	steppingStone->steppingStoneObjectInfo->collision = STONECOLL_FALSE;
	steppingStone->steppingStoneObjectInfo->collider = NULL;
	steppingStone->steppingStoneObjectInfo->elapsedTime = 0.0f;
	steppingStone->steppingStoneObjectInfo->idleBobbleInc = 1.0f;
	steppingStone->steppingStoneObjectInfo->bobbleTime = bobbleTime;
	if((steppingStone->steppingStoneObjectInfo->bobbleHeight = bobbleHeight) > METERTOUNIT(0.45f)) steppingStone->steppingStoneObjectInfo->bobbleHeight = METERTOUNIT(0.45f);
	steppingStone->steppingStoneObjectInfo->sinkSpeed = sinkSpeed;
	bmVectorCopy(steppingStone->steppingStoneObjectInfo->initialPosition, initPosition);
	steppingStone->steppingStoneObjectInfo->idleBobbleTime = 0.0f;
	steppingStone->steppingStoneObjectInfo->state = STEPPINGSTONE_STATEREST;

	SetActorPosition( steppingStone, initPosition );

	AddActorToCollisionCache(&collisionCache,steppingStone, CACHEFLAGS_LIVEUPDATE);
	SetCollisionCallback(steppingStone,RespondToSteppingStoneCollision);
}

/* --------------------------------------------------------------------------------
   Function : CreateSteppingStoneActorInstanceInfo
   Purpose : Creates and initialises springy info part of object
   Parameters : ACTORINSTANCE * 
   Returns : 
   Info : 
*/

int CreateSteppingStoneActorInstanceInfo( ACTORINSTANCE *actorInstance )
{
	actorInstance->steppingStoneObjectInfo = (STEPPINGSTONEOBJECTINFO*)ZALLOC(sizeof(STEPPINGSTONEOBJECTINFO));
	if (actorInstance->steppingStoneObjectInfo)
	{
		return TRUE;
	}
	else
	{
		bkPrintf("*** ERROR *** Could not create stepping stone object info structure\n");
		return FALSE;
	}
}

/* --------------------------------------------------------------------------------
   Function : RespondToSteppingStoneCollision
   Purpose : collision callback
   Parameters : ACTORINSTANCE * x2
   Returns : 
   Info : 
*/

int RespondToSteppingStoneCollision( ACTORINSTANCE *collider, ACTORINSTANCE *stone )
{
	if (collider->genericAI) 
	{
		AICollideWithSpecial(collider,stone);
		return FALSE;
	}
	if (collider->flags&ACTORFLAG_DEBRISACTOR) return FALSE;

	if((collider->actorStatus->currentState == STATE_JUMP) || (collider->actorStatus->currentState == STATE_PROJECTILE))
	{
		//set flag to say that collision has occured
		stone->steppingStoneObjectInfo->collision = STONECOLL_BOBBLE;
		stone->steppingStoneObjectInfo->elapsedTime = 0.0f;

		if(stone->steppingStoneObjectInfo->state == STEPPINGSTONE_STATEBOBBLE)
		{
			if(stone->steppingStoneObjectInfo->heightOfSink == 0.0f)
			{
				SetActorPosition(stone, stone->steppingStoneObjectInfo->initialPosition);
			}
			else
				if(stone->steppingStoneObjectInfo->heightOfSink > 0)
				{
					if(stone->steppingStoneObjectInfo->currentPosition[1] > stone->steppingStoneObjectInfo->initialPosition[1])
					{
						SetActorPosition(stone, stone->steppingStoneObjectInfo->initialPosition);
					}
				}
				else
				{
					if(stone->steppingStoneObjectInfo->currentPosition[1] < stone->steppingStoneObjectInfo->initialPosition[1])
					{
						SetActorPosition(stone, stone->steppingStoneObjectInfo->initialPosition);
					}
				}
		}

		stone->actorBehaviour->physics.velocity[1] = 0.0f;
		bmVectorCopy(stone->steppingStoneObjectInfo->currentPosition, stone->actorInstance.position);
	}
	else
	{
		switch(collider->actorStatus->currentState)
		{
		case STATE_MOVE:
		case STATE_TIPTOE:
		case STATE_SCARE:
		case STATE_BADFOOD:
			// CMD: note that collision has occurred
			stone->steppingStoneObjectInfo->collision = STONECOLL_TRUE;
			break;
		default:
			stone->steppingStoneObjectInfo->collision = STONECOLL_TRUE;
			stone->steppingStoneObjectInfo->collisionTime = 0.0f;
			return FALSE;
		}
	}

	stone->steppingStoneObjectInfo->collider = collider;
	stone->steppingStoneObjectInfo->collisionTime = 0.0f;

	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : UpdateSteppingStone
   Purpose : updates position and velocity of stepping stones in instance list
   Parameters : ACTORINSTANCELIST *
   Returns : 
   Info : 
*/

void UpdateSteppingStone(ACTORINSTANCE *ptr)
{
	if(ptr->steppingStoneObjectInfo)
	{
		if(ptr->steppingStoneObjectInfo->heightOfSink < 0.0f)
		{
			NegativeSteppingStonePart(ptr);
		}
		else
		{
			SteppingStonePart(ptr);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : SteppingStonePart
   Purpose : 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void SteppingStonePart( ACTORINSTANCE *stone )
{
	STEPPINGSTONEOBJECTINFO	*info = stone->steppingStoneObjectInfo;
	if((stone->steppingStoneObjectInfo->collisionTime += fTime) > 1.0f)
	{
		stone->steppingStoneObjectInfo->collider = NULL;
	}

//	bmVectorCopy(stone->steppingStoneObjectInfo->currentPosition, stone->actorInstance.position);

	switch(stone->steppingStoneObjectInfo->state)
	{
	case STEPPINGSTONE_STATEREST:
		if((stone->steppingStoneObjectInfo->idleBobbleTime += stone->steppingStoneObjectInfo->idleBobbleInc/fps*2) > 3.0f)
		{
			stone->steppingStoneObjectInfo->idleBobbleInc = -1.0f;
		}
		if((stone->steppingStoneObjectInfo->idleBobbleTime += stone->steppingStoneObjectInfo->idleBobbleInc/fps*2) < 0.0f)
		{
			stone->steppingStoneObjectInfo->idleBobbleInc = 1.0f;
		}
		//ApplyBobble(stone, METERTOUNIT(0.02f), 1.0f, stone->steppingStoneObjectInfo->idleBobbleTime);
		
		if(stone->steppingStoneObjectInfo->collision == STONECOLL_BOBBLE)
		{
			stone->steppingStoneObjectInfo->state = STEPPINGSTONE_STATEBOBBLE;
		}
		else
		{
			if(stone->steppingStoneObjectInfo->heightOfSink)
			{
				if(stone->steppingStoneObjectInfo->collision == STONECOLL_TRUE)
				{
					stone->steppingStoneObjectInfo->state = STEPPINGSTONE_STATESINK;
				}
				else
				if(stone->steppingStoneObjectInfo->heightOfSink > 0.0f)
				{
					stone->steppingStoneObjectInfo->state = STEPPINGSTONE_STATERISE;
				}
			}
		}
		break;
	case STEPPINGSTONE_STATESINK:
		SinkSteppingStone(stone);
		if(stone->steppingStoneObjectInfo->collisionTime >= 0.3f)
		{
			stone->steppingStoneObjectInfo->state = STEPPINGSTONE_STATERISE;
		}
		else
		{
			if((stone->steppingStoneObjectInfo->initialPosition[1] - stone->actorInstance.position[1]) < stone->steppingStoneObjectInfo->heightOfSink)
			{
				stone->steppingStoneObjectInfo->state = STEPPINGSTONE_STATESINK;
			}
		}
		break;
	case STEPPINGSTONE_STATEBOBBLE:
		ApplyBobble(stone, stone->steppingStoneObjectInfo->bobbleHeight, 4.0f, stone->steppingStoneObjectInfo->elapsedTime);
		if((stone->steppingStoneObjectInfo->elapsedTime += 1.0f/fps) > stone->steppingStoneObjectInfo->bobbleTime)
		{
			if(stone->steppingStoneObjectInfo->heightOfSink)
			{
				if((stone->steppingStoneObjectInfo->initialPosition[1] - stone->actorInstance.position[1]) < stone->steppingStoneObjectInfo->heightOfSink)
				{
					if(stone->steppingStoneObjectInfo->collisionTime >= 0.3f)
					{
						stone->steppingStoneObjectInfo->state = STEPPINGSTONE_STATERISE;
					}
					else
					{
						stone->steppingStoneObjectInfo->state = STEPPINGSTONE_STATESINK;
					}
				}
				else
				{
					stone->steppingStoneObjectInfo->state = STEPPINGSTONE_STATEREST;
				}
			}
			else
			{
				stone->steppingStoneObjectInfo->state = STEPPINGSTONE_STATEREST;
			}
		}
		break;
	case STEPPINGSTONE_STATERISE:
		RaiseTheSteppingStone(stone);				//sequel to 'titanic' but with a lower budget
		if(stone->steppingStoneObjectInfo->initialPosition[1] > stone->actorInstance.position[1])
		{
			if(stone->steppingStoneObjectInfo->collision == STONECOLL_BOBBLE)
			{
				stone->steppingStoneObjectInfo->state = STEPPINGSTONE_STATEBOBBLE;
			}
			else
			{
				if(stone->steppingStoneObjectInfo->collisionTime >= 0.3f)
				{
					stone->steppingStoneObjectInfo->state = STEPPINGSTONE_STATERISE;
				}
				else
				{
					stone->steppingStoneObjectInfo->state = STEPPINGSTONE_STATESINK;
				}
			}
		}
		else
		{
			stone->steppingStoneObjectInfo->state = STEPPINGSTONE_STATEREST;
			bmVectorCopy(stone->actorInstance.position, stone->steppingStoneObjectInfo->initialPosition);
		}
		break;
	}
	stone->steppingStoneObjectInfo->collision = STONECOLL_FALSE;
}

/* --------------------------------------------------------------------------------
   Function : NegativeSteppingStonePart
   Purpose : 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void NegativeSteppingStonePart(ACTORINSTANCE *stone)
{
	/*if((*/stone->steppingStoneObjectInfo->collisionTime += fTime; //) > 1.0f)
//	{
		stone->steppingStoneObjectInfo->collider = NULL;
//	}

//	bmVectorCopy(stone->steppingStoneObjectInfo->currentPosition, stone->actorInstance.position);

	switch(stone->steppingStoneObjectInfo->state)
	{
	case STEPPINGSTONE_STATEREST:
		if((stone->steppingStoneObjectInfo->idleBobbleTime += stone->steppingStoneObjectInfo->idleBobbleInc/fps*2) > 3.0f)
		{
			stone->steppingStoneObjectInfo->idleBobbleInc = -1.0f;
		}
		if((stone->steppingStoneObjectInfo->idleBobbleTime += stone->steppingStoneObjectInfo->idleBobbleInc/fps*2) < 0.0f)
		{
			stone->steppingStoneObjectInfo->idleBobbleInc = 1.0f;
		}
		
		if(stone->steppingStoneObjectInfo->collision == STONECOLL_BOBBLE)
		{
			stone->steppingStoneObjectInfo->state = STEPPINGSTONE_STATEBOBBLE;
		}
		else
		if(stone->steppingStoneObjectInfo->collision == STONECOLL_TRUE)
		{
			stone->steppingStoneObjectInfo->state = STEPPINGSTONE_STATERISE;
		}
		else
		if(stone->steppingStoneObjectInfo->heightOfSink < 0.0f)
		{
			stone->steppingStoneObjectInfo->state = STEPPINGSTONE_STATESINK;
		}
		break;
	case STEPPINGSTONE_STATESINK:
		SinkSteppingStone(stone);
		if(stone->actorInstance.position[1] < stone->steppingStoneObjectInfo->initialPosition[1])
		{
			stone->steppingStoneObjectInfo->state = STEPPINGSTONE_STATEREST;
			bmVectorCopy(stone->actorInstance.position, stone->steppingStoneObjectInfo->initialPosition);
			break;
		}
		if(stone->steppingStoneObjectInfo->collisionTime <= 0.3f)
		{
			stone->steppingStoneObjectInfo->state = STEPPINGSTONE_STATERISE;
			break;
		}
		else
		{
			stone->steppingStoneObjectInfo->collider = NULL;
		}
		break;
	case STEPPINGSTONE_STATEBOBBLE:
		ApplyBobble(stone, stone->steppingStoneObjectInfo->bobbleHeight, 4.0f, stone->steppingStoneObjectInfo->elapsedTime);
		if((stone->steppingStoneObjectInfo->elapsedTime += 1.0f/fps) > stone->steppingStoneObjectInfo->bobbleTime)
		{
			stone->steppingStoneObjectInfo->state = STEPPINGSTONE_STATERISE;
		}
		break;
	case STEPPINGSTONE_STATERISE:
		RaiseTheSteppingStone(stone);				//sequel to 'titanic' but with a lower budget
		if(stone->steppingStoneObjectInfo->collision == STONECOLL_BOBBLE)
		{
			stone->steppingStoneObjectInfo->state = STEPPINGSTONE_STATEBOBBLE;
			break;
		}
		if((stone->steppingStoneObjectInfo->initialPosition[1] - stone->actorInstance.position[1]) < stone->steppingStoneObjectInfo->heightOfSink)
		{
			if(stone->steppingStoneObjectInfo->collisionTime > 1.0f)
			{
				stone->steppingStoneObjectInfo->state = STEPPINGSTONE_STATESINK;
				break;
			}
			else
			{
				stone->steppingStoneObjectInfo->state = STEPPINGSTONE_STATEHOLD;
				break;
			}
		}
		if(stone->steppingStoneObjectInfo->collisionTime > 1.0f)
		{
			stone->steppingStoneObjectInfo->state = STEPPINGSTONE_STATESINK;
			stone->steppingStoneObjectInfo->collider = NULL;
			break;
		}
		break;
	case STEPPINGSTONE_STATEHOLD:
		if(stone->steppingStoneObjectInfo->collisionTime > 1.0f)
		{
			stone->steppingStoneObjectInfo->state = STEPPINGSTONE_STATESINK;
			stone->steppingStoneObjectInfo->collider = NULL;
		}
		break;
	}
	stone->steppingStoneObjectInfo->collision = STONECOLL_FALSE;
}

/* --------------------------------------------------------------------------------
   Function : ApplyBobble
   Purpose : calculates bobble
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void ApplyBobble( ACTORINSTANCE *stone, float maxBobble, float attenuation, float elapsedTime )
{
	TBVector		yDirection = { 0.0f, 1.0f, 0.0f, 1.0f };
	TBVector		tempPos;
	//float			attenuation = 2.0f;
	//float			maxBobble = METERTOUNIT(0.1f);
	float			bobbleRate = 4.0f*PI;

	tempPos[1] = (-maxBobble*bmCos((bobbleRate*elapsedTime))/((elapsedTime+1)*attenuation)); 

	tempPos[0] = stone->steppingStoneObjectInfo->currentPosition[0];
	tempPos[1] += stone->steppingStoneObjectInfo->currentPosition[1];
	tempPos[2] = stone->steppingStoneObjectInfo->currentPosition[2];
	tempPos[3] = 1.0f;

	SetActorPosition(stone, tempPos);
	baPrepActorInstance(&stone->actorInstance,TRUE);
}

/* --------------------------------------------------------------------------------
   Function : SinkSteppingStone
   Purpose : calculates sinking of stepping stone
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void SinkSteppingStone( ACTORINSTANCE *stone )
{
	TBVector			positionInc;

	if(ApplyGravity(positionInc, stone->actorBehaviour->physics.velocity, stone->steppingStoneObjectInfo->sinkSpeed))
	{
		stone->actorInstance.position[1] += positionInc[1];
		baPrepActorInstance(&stone->actorInstance,TRUE);
		if(stone->steppingStoneObjectInfo->collider)
		{
			stone->steppingStoneObjectInfo->collider->actorInstance.position[1] += positionInc[1];
			baPrepActorInstance(&stone->steppingStoneObjectInfo->collider->actorInstance,TRUE);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : RaiseTheSteppingStone
   Purpose : calculates raising (at a constnt rate) of stepping stone from murky depths of 'the pond'
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void RaiseTheSteppingStone( ACTORINSTANCE *stone )
{
	float			rate = METERTOUNIT(0.5f);					//measured in meters per second

//	if(stone->actorInstance.position[1] < stone->steppingStoneObjectInfo->initialPosition[1])
//	{
		stone->actorInstance.position[1] += rate*fTime;
		baPrepActorInstance(&stone->actorInstance,TRUE);
//	}
//	else
//	{
//		stone->actorInstance.position[1] = stone->steppingStoneObjectInfo->initialPosition[1];
//		//stone->steppingStoneObjectInfo->state = STEPPINGSTONE_STATEREST;
//		baPrepActorInstance(&stone->actorInstance,TRUE);
//	}
}