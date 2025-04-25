// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : fountain.cpp
//   Purpose : functions to create fountains
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "maths.h"
#include "actors.h"
#include "collisioncache.h"
#include "scene.h"
#include "status.h"
#include "physics.h"
#include "main.h"
#include "animation.h"
#include "fountainextrasystem.h"
#include "fountain.h"
#include "genericai.h"
#include "sfx.h"


/* --------------------------------------------------------------------------------
   Function : CreateFountain
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateFountain(ACTORINSTANCE *actorInstance, float maxHeight, float period, int idle)
{
	actorInstance->special = SPECIALTYPE_FOUNTAIN;
	if(!(actorInstance->fountainInfo = CreateFountainInfo())) return FALSE;

	//fill in attributes
	actorInstance->fountainInfo->characterOnFountain = NULL;
	actorInstance->fountainInfo->fountainState = FOUNTAIN_MINIMUM;
	actorInstance->fountainInfo->clock = 0.0f;
	bmVectorCopy(actorInstance->fountainInfo->initialPosition, actorInstance->actorInstance.position);
	actorInstance->fountainInfo->maxHeight = maxHeight;
	actorInstance->fountainInfo->period = 0.1f;
	actorInstance->fountainInfo->scenic = FALSE;
	if(idle == 2)
	{
		actorInstance->fountainInfo->scenic = TRUE;
		idle = TRUE;
	}
	if(idle == 3)
	{
		actorInstance->fountainInfo->scenic = 2;
		idle = TRUE;
	}
	if((actorInstance->fountainInfo->idle = idle) == -1) actorInstance->fountainInfo->active = FALSE;
	else actorInstance->fountainInfo->active = TRUE;
	SETVECTOR(actorInstance->fountainInfo->velocity, 0.0f, 0.0f, 0.0f, 1.0f);

	if(!actorInstance->fountainInfo->scenic)
	{
		AddActorToCollisionCache(&collisionCache, actorInstance, 0);
		SetCollisionCallback(actorInstance, RespondToFountainCollision);
	}

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : CreateFountainInfo
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

FOUNTAININFO *CreateFountainInfo(void)
{
	FOUNTAININFO	*tempStruct;

	tempStruct = (FOUNTAININFO*)ZALLOC(sizeof(FOUNTAININFO));
	if(tempStruct)
	{
		return tempStruct;
	}
	else
	{
		bkPrintf("*** WARNING ***, could not create fountain info structure\n");
		return NULL;
	}
}

/* --------------------------------------------------------------------------------
   Function : RespondToFountainCollision
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToFountainCollision(ACTORINSTANCE *collider, ACTORINSTANCE *fountain)
{
	TBVector	tempVector;

	if (collider->genericAI) 
	{
		AICollideWithSpecial(collider,fountain);
		return FALSE;
	}

	if(!fountain->fountainInfo->active) return FALSE;
	if(fountain->fountainInfo->scenic) return FALSE;
	if (collider->flags&ACTORFLAG_DEBRISACTOR) return FALSE;

	if(!RequestStateChange(collider, STATE_ONFOUNTAIN)) return FALSE;
	fountain->fountainInfo->characterOnFountain = collider;
	collider->actorStatus->actorAttachedTo = fountain;

	collider->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;
	collider->actorAnimation->useMotionBone = FALSE;
//	RequestStateChange(collider, STATE_ONFOUNTAIN);
	RemoveActorFromCollisionCache(&collisionCache, fountain);
//	RemoveActorFromCollisionCache(&collisionCache, gameStatus.player1);

	// TP: PROBLEM
	if (collider->playerNo != -1)
	{
		SetFixedDistanceToLookAt(gameStatus.player[collider->playerNo].camera, METERTOUNIT(5.0f));
	}
	//zero velocity
	ZEROVECTOR(tempVector);
	SetActorVelocity(collider->actorBehaviour, tempVector);
	
	//set colliders position in xz plane to be that of fountain
	bmVectorCopy(collider->actorInstance.position, fountain->actorInstance.position);
	PlayAnimationByName(collider, "idle4", 1.0f, 1, 0, 0.1f, DEATH);
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : ProcessFountainPhysics
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void ProcessFountainPhysics(ACTORINSTANCE *fountain)
{
	TBVector		velocity, tempPosition;
	TBVector		tempVector;
	int				counter, fade;
	FOUNTAININFO	*info = fountain->fountainInfo;
	float			fOne, fTwo, fThree, fFour;

	if(fountain->fountainInfo->characterOnFountain)
	{
		fountain->fountainInfo->clock += 1.0f/fps;
		switch(fountain->fountainInfo->fountainState)
		{
		case FOUNTAIN_MINIMUM:
			if(fountain->fountainInfo->characterOnFountain->actorInstance.position[1] <= fountain->fountainInfo->initialPosition[1])
			{
				SetActorPosition(fountain->fountainInfo->characterOnFountain, fountain->fountainInfo->initialPosition);
				SETVECTOR(velocity, 0.0f, METERTOUNIT(5.0f), 0.0f, 1.0f);
				SetActorVelocity(fountain->fountainInfo->characterOnFountain->actorBehaviour, velocity);
			}
			if(fountain->fountainInfo->clock > fountain->fountainInfo->period)
			{
				fountain->fountainInfo->clock = 0.0f;
				fountain->fountainInfo->fountainState = FOUNTAIN_RISE;
				fountain->fountainInfo->characterOnFountain->actorBehaviour->physics.flags |= PHYSFLAG_NOGRAVITY;
				ZEROVECTOR(velocity);
				SetActorVelocity(fountain->fountainInfo->characterOnFountain->actorBehaviour, velocity);
			}
			break;
		case FOUNTAIN_MAXIMUM:
			if(fountain->fountainInfo->characterOnFountain->actorInstance.position[1]
												<= (fountain->fountainInfo->initialPosition[1]+fountain->fountainInfo->maxHeight))
			{
				SETVECTOR(tempVector, 0.0f, fountain->fountainInfo->maxHeight, 0.0f, 1.0f);
				bmVectorAdd(tempVector, fountain->fountainInfo->initialPosition, tempVector);
				SetActorPosition(fountain->fountainInfo->characterOnFountain, tempVector);
				SETVECTOR(velocity, 0.0f, METERTOUNIT(5.0f), 0.0f, 1.0f);
				SetActorVelocity(fountain->fountainInfo->characterOnFountain->actorBehaviour, velocity);
			}
			if(fountain->fountainInfo->clock > fountain->fountainInfo->period)
			{
				fountain->fountainInfo->clock = 0.0f;
				fountain->fountainInfo->fountainState = FOUNTAIN_STALL;
				SetActorPosition(fountain, fountain->fountainInfo->initialPosition);
				fountain->fountainInfo->characterOnFountain->actorBehaviour->physics.flags |= PHYSFLAG_NOGRAVITY;
				ZEROVECTOR(velocity);
				SetActorVelocity(fountain->fountainInfo->characterOnFountain->actorBehaviour, velocity);
			}
			break;
		case FOUNTAIN_RISE:
			if((fountain->actorInstance.position[1] += (FOUNTAIN_VELOCITY/fps)) > (fountain->fountainInfo->initialPosition[1]+fountain->fountainInfo->maxHeight))
			{
				fountain->fountainInfo->fountainState = FOUNTAIN_MAXIMUM;
				fountain->fountainInfo->characterOnFountain->actorBehaviour->physics.flags &= ~PHYSFLAG_NOGRAVITY;
				fountain->fountainInfo->clock = 0.0f;
			}
			SetActorPosition(fountain->fountainInfo->characterOnFountain, fountain->actorInstance.position);
			break;
		case FOUNTAIN_FALL:
			if(fountain->fountainInfo->characterOnFountain->actorInstance.position[1] < fountain->fountainInfo->initialPosition[1])
			{
				fountain->fountainInfo->fountainState = FOUNTAIN_MINIMUM;
				fountain->fountainInfo->characterOnFountain->actorBehaviour->physics.flags &= ~PHYSFLAG_NOGRAVITY;
				fountain->fountainInfo->clock = 0.0f;
			}
			break;
		case FOUNTAIN_STALL:
			if(fountain->fountainInfo->clock > FOUNTAIN_STALLTIME)
			{
				fountain->fountainInfo->clock = 0.0f;
				fountain->fountainInfo->fountainState = FOUNTAIN_FALL;
				fountain->fountainInfo->characterOnFountain->actorBehaviour->physics.flags &= ~PHYSFLAG_NOGRAVITY;
				ZEROVECTOR(velocity);
				SetActorVelocity(fountain->fountainInfo->characterOnFountain->actorBehaviour, velocity);
			}
			break;
		}
	}
	if(fountain->onScreen)
	{
		bmVectorSub(tempVector, gameStatus.viewPoint, fountain->actorInstance.position);
		fade = 0;
		if(bmVectorLen(tempVector) < METERTOUNIT(30.0f)) fade = 1;
		if(bmVectorLen(tempVector) < METERTOUNIT(20.0f)) fade = 2;
		if(bmVectorLen(tempVector) < METERTOUNIT(10.0f)) fade = 3;

		fOne = METERTOUNIT(0.1f);
		fTwo = METERTOUNIT(0.02f);
		fFour = 2.0f;
		if(fade)
		{
			//update fountain particle effect
			if(fountain->fountainInfo->scenic)
			{
				if(fountain->fountainInfo->scenic == 1)
				{
					for(counter = fade; counter >= 0; --counter)
					{
						tempPosition[0] = fountain->fountainInfo->initialPosition[0] + (float)((bmRand()%100)-50);
						tempPosition[1] = fountain->fountainInfo->initialPosition[1];
						tempPosition[2] = fountain->fountainInfo->initialPosition[2] + (float)((bmRand()%100)-50);
						tempPosition[3] = 1.0f;
						baCreateExtra(fountainExtraSystem, tempPosition, &fOne, &fTwo, &fountain->fountainInfo->maxHeight, &fFour, NULL, FOUNTAINEXTRA_WAYUP);
					}
				}
			}
			else
			{
				switch(fountain->fountainInfo->fountainState)
				{
				case FOUNTAIN_MINIMUM:
					for(counter = fade; counter >= 0; --counter)
					{
						tempPosition[0] = fountain->fountainInfo->initialPosition[0] + (float)((bmRand()%100)-50);
						tempPosition[1] = fountain->fountainInfo->initialPosition[1];
						tempPosition[2] = fountain->fountainInfo->initialPosition[2] + (float)((bmRand()%100)-50);
						tempPosition[3] = 1.0f;
						baCreateExtra(fountainExtraSystem, tempPosition, &fOne, &fTwo, &fountain->fountainInfo->maxHeight, &fFour, fountain, FOUNTAINEXTRA_WAYDOWN);
					}
					break;
				case FOUNTAIN_MAXIMUM:
					for(counter = fade; counter >= 0; --counter)
					{
						tempPosition[0] = fountain->fountainInfo->initialPosition[0] + (float)((bmRand()%100)-50);
						tempPosition[1] = fountain->fountainInfo->initialPosition[1];
						tempPosition[2] = fountain->fountainInfo->initialPosition[2] + (float)((bmRand()%100)-50);
						tempPosition[3] = 1.0f;
						fThree = fountain->actorInstance.position[1]-fountain->fountainInfo->initialPosition[1];
						baCreateExtra(fountainExtraSystem, tempPosition, &fOne, &fTwo, &fThree, &fFour, fountain, FOUNTAINEXTRA_WAYUP);
					}
					SetActorPosition(fountain, fountain->fountainInfo->initialPosition);
					break;
				case FOUNTAIN_RISE:
					for(counter = fade; counter >= 0; --counter)
					{
						tempPosition[0] = fountain->fountainInfo->initialPosition[0] + (float)((bmRand()%100)-50);
						tempPosition[1] = fountain->fountainInfo->initialPosition[1];
						tempPosition[2] = fountain->fountainInfo->initialPosition[2] + (float)((bmRand()%100)-50);
						tempPosition[3] = 1.0f;
						fThree = fountain->actorInstance.position[1]-fountain->fountainInfo->initialPosition[1];
						baCreateExtra(fountainExtraSystem, tempPosition, &fOne, &fTwo, &fThree, &fFour, fountain, FOUNTAINEXTRA_WAYUP);
					}
					break;
				case FOUNTAIN_FALL:
					for(counter = fade; counter >= 0; --counter)
					{
						tempPosition[0] = fountain->fountainInfo->initialPosition[0] + (float)((bmRand()%100)-50);
						tempPosition[1] = fountain->fountainInfo->initialPosition[1];
						tempPosition[2] = fountain->fountainInfo->initialPosition[2] + (float)((bmRand()%100)-50);
						tempPosition[3] = 1.0f;
						baCreateExtra(fountainExtraSystem, tempPosition, &fOne, &fTwo, &fountain->fountainInfo->maxHeight, &fFour,fountain, FOUNTAINEXTRA_WAYDOWN);
					}
					break;
				}
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateFountains
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateFountains(ACTORINSTANCE *ptr)
{
	if(ptr->fountainInfo)
	{
		if(ptr->fountainInfo->active)
		{
			ProcessFountainPhysics(ptr);
		}
	}
}
