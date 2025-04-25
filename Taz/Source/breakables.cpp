// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : breakables.cpp
//   Purpose : functions for creating and updating breakable items.
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "actors.h"
#include "status.h"
#include "main.h"
#include "sfx.h"
#include "debris.h"
#include "maths.h"
#include "scene.h"
#include "destructibles.h"
#include "boss.h"							// PP: General boss game stuff
#include "tutorial.h"

/* --------------------------------------------------------------------------------
   Function : DestroyObject
   Purpose : Function to be called when a object is to be destroyed
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void DestroyObject(ACTORINSTANCE *objectInstance)
{
	TBVector pos,velocity,rotationSpeed;
	int		i, random;
	DEBRIS	*debris;

	if (objectInstance)
	{
		// PP: don't have Sam say anything if we're in a boss game
		if(!IN_BOSS_GAME)
		{
			//play sound
			if(objectInstance->special == SPECIALTYPE_DESTRUCTIBLE)
			{
				if((bmRand()%5) == 0)
				{
					if(characterSounds.sam == -1)
					{
						random = bmRand()%3;
						switch(random)
						{
						case 0:
							CallbackWhenSampleFinished((characterSounds.sam=PlaySample("samangry_1.wav",200)),CharacterSoundsFinishedCallback,&characterSounds.sam);
							break;
						case 1:
							CallbackWhenSampleFinished((characterSounds.sam=PlaySample("samangry_2.wav",200)),CharacterSoundsFinishedCallback,&characterSounds.sam);
							break;
						case 2:
//							CallbackWhenSampleFinished((characterSounds.sam=PlaySample("samangry_4.wav",200)),CharacterSoundsFinishedCallback,&characterSounds.sam);
//							break;
//						case 3:
							CallbackWhenSampleFinished((characterSounds.sam=PlaySample("samangry_5.wav",200)),CharacterSoundsFinishedCallback,&characterSounds.sam);
							break;
						}
					}
				}
			}
		}

		// start tree explosion
		bmVectorCopy(pos,objectInstance->actorInstance.position);
		for (i=0;i<5;i++)
		{
			velocity[X] = (float)bmRand();
			velocity[Y] = (float)bmRand();
			velocity[Z] = (float)bmRand();
			velocity[X] -= 32768.0f/2.0f;
			velocity[Y] *= 2.0f;
			velocity[Z] -= 32768.0f/2.0f;
			rotationSpeed[X] = (bmRand()%3)*PI;
			rotationSpeed[Y] = (bmRand()%5)*PI;
			rotationSpeed[Z] = 0.0f;
			rotationSpeed[W] = 1.0f;
			bmVectorScaleToLength(velocity,velocity,METERTOUNIT(4.8f));
			
			debris = CreateDebrisActor(objectInstance->actorCRC, pos, velocity, rotationSpeed,0.5f+((float)(bmRand()%10)/10), 0.25f*objectInstance->actorInstance.scale[0], 0, 0, 128, DEBRISFLAG_LOWGRAVITY,0,0,0.0f);
			if ((debris)&&(objectInstance->flags & ACTORFLAG_DONTDRAW)||(objectInstance->instanceList==&map.invisibleInstances))
			{
				debris->actorInstance->flags|=ACTORFLAG_DONTDRAW;
			}
		}
		// destroy tree
		RemoveActorFromCollisionCache(&collisionCache, objectInstance);
		FreeActorInstance(objectInstance);
	}
}

/* --------------------------------------------------------------------------------
   Function : BreakObject
   Purpose : Function to be called when a object is to be destroyed
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void BreakObject(ACTORINSTANCE *objectInstance)
{
	TBVector pos,velocity,rotationSpeed;
	int i;

	if (objectInstance)
	{
		// start tree explosion
		bmVectorCopy(pos,objectInstance->actorInstance.position);
		for (i=0;i<5;i++)
		{
			velocity[X] = (float)bmRand();
			velocity[Y] = (float)bmRand();
			velocity[Z] = (float)bmRand();
			velocity[X] -= 32768.0f/2.0f;
			velocity[Y] *= 2.0f;
			velocity[Z] -= 32768.0f/2.0f;
			rotationSpeed[X] = (bmRand()%3)*PI;
			rotationSpeed[Y] = (bmRand()%5)*PI;
			rotationSpeed[Z] = 0.0f;
			rotationSpeed[W] = 1.0f;
			bmVectorScaleToLength(velocity,velocity,METERTOUNIT(2.4f));
			CreateDebrisActor(objectInstance->actorCRC, pos, velocity, rotationSpeed,1.0f, 0.25f*objectInstance->actorInstance.scale[0], 0, 0, 128, DEBRISFLAG_GRAVITY,0,0,0.0f);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : BreakTazPickup
   Purpose : Function to be called when a pickup is to be destroyed
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void BreakTazPickup(TBVector pos,char *name)
{
	TBVector velocity,rotationSpeed;
	int i;

	for (i=0;i<10;i++)
	{
		velocity[X] = (float)bmRand();
		velocity[Y] = (float)bmRand();
		velocity[Z] = (float)bmRand();
		velocity[X] -= 32768.0f/2.0f;
		velocity[Y] -= 32768.0f/2.0f;
		velocity[Z] -= 32768.0f/2.0f;
		rotationSpeed[X] = (bmRand()%3)*PI;
		rotationSpeed[Y] = (bmRand()%5)*PI;
		rotationSpeed[Z] = 0.0f;
		rotationSpeed[W] = 1.0f;
		bmVectorScaleToLength(velocity,velocity,(bmRand()%300)+720.0f);
		CreateDebrisActor(name, pos, velocity, rotationSpeed,0.5f, 0.25f, 0, 0, 128, 0,0,0,0.0f);
	}
}

/* --------------------------------------------------------------------------------
   Function : DestroyObject
   Purpose : Function to be called when a object is to be destroyed
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void DestroyObject(ACTORINSTANCE *objectInstance, DESTRUCTIBLEDEBRISINFO *debris, int destroyOriginal)
{
	TBTexture		*debrisTexture;
	int number, i, random;
	TBVector	offset;
	TBVector	pos,velocity,rotationSpeed;

	if (objectInstance)
	{
		// PP: don't have Sam say anything if we're in a boss game
		if(!IN_BOSS_GAME)
		{
			//play sound
			if(objectInstance->special == SPECIALTYPE_DESTRUCTIBLE)
			{
				if((bmRand()%3) == 0)
				{
					if(characterSounds.sam == -1)
					{
						random = bmRand()%3;
						switch(random)
						{
						case 0:
							CallbackWhenSampleFinished((characterSounds.sam=PlaySample("samangry_1.wav",200)),CharacterSoundsFinishedCallback,&characterSounds.sam);
							break;
						case 1:
							CallbackWhenSampleFinished((characterSounds.sam=PlaySample("samangry_2.wav",200)),CharacterSoundsFinishedCallback,&characterSounds.sam);
							break;
						case 2:
//							CallbackWhenSampleFinished((characterSounds.sam=PlaySample("samangry_4.wav",200)),CharacterSoundsFinishedCallback,&characterSounds.sam);
//							break;
//						case 3:
							CallbackWhenSampleFinished((characterSounds.sam=PlaySample("samangry_5.wav",200)),CharacterSoundsFinishedCallback,&characterSounds.sam);
							break;
						}
					}
				}
			}
		}

		// start tree explosion
		bmVectorCopy(pos,objectInstance->actorInstance.position);
		
		for (i=0;i<7;i++)
		{
			SETVECTOR(offset, 0.0f, 0.0f, 0.0f, 1.0f);
			if((objectInstance->special == SPECIALTYPE_DESTRUCTIBLE)&&(objectInstance->destructibleObjectInfo))
			{
				if((int)objectInstance->destructibleObjectInfo->xOffset)
				{
					if(bmRand()%2)
					{
						offset[0] = (float)(bmRand()%(int)objectInstance->destructibleObjectInfo->xOffset);
					}
					else
					{
						offset[0] = -(float)(bmRand()%(int)objectInstance->destructibleObjectInfo->xOffset);
					}
				}
				if((int)objectInstance->destructibleObjectInfo->yOffset)
				{
					offset[1] = objectInstance->destructibleObjectInfo->yOffset;
				}
				else
				{
					// CMD: request from pesty, may well need changing
	//				offset[1] = METERTOUNIT(0.5f);
				}
				if((int)objectInstance->destructibleObjectInfo->zOffset)
				{
					if(bmRand()%2)
					{
						offset[2] = (float)(bmRand()%(int)objectInstance->destructibleObjectInfo->zOffset);
					}
					else
					{
						offset[2] = -(float)(bmRand()%(int)objectInstance->destructibleObjectInfo->zOffset);
					}
				}
			}
			bmVectorAdd(offset, pos, offset);


			velocity[X] = -0.4f + ((float)(bmRand()%800) / 1000);
			velocity[Y] = 0.5f + ((float)(bmRand()%1000) / 2000);
			velocity[Z] = -0.4f + ((float)(bmRand()%800) / 1000);

			bmVectorScaleToLength(velocity,velocity,METERTOUNIT(5.0f)+METERTOUNIT((float)(bmRand()%5)));

			rotationSpeed[X] = (bmRand()%3)*PI;
			rotationSpeed[Y] = (bmRand()%5)*PI;
			rotationSpeed[Z] = 0.0f;
			rotationSpeed[W] = 1.0f;

			if(debris)
			{
				if(objectInstance->destructibleObjectInfo->componentDebris)
				{
					number = i;
				}
				else
				{
					number = bmRand()%debris->numDebris;
				}
				if((debrisTexture = bkLoadTextureByCRC(0, debris->debrisCRC[number], 0)) != NULL)
				{
					CreateDebrisActor(debrisTexture, pos, velocity, rotationSpeed,UtilGenerateRandomNumber(1.5f, 0.5f), RAD(UtilGenerateRandomNumber(360.0f, 0.0f)), METERTOUNIT(0.3f), METERTOUNIT(0.3f), 128, DEBRISFLAG_GRAVITY|DEBRISFLAG_ROTATESPRITE,0,0,RAD(UtilGenerateRandomNumber(360.0f, -360.0f)));
				}
				else
				{
					CreateDebrisActor(debris->debrisCRC[number], offset, velocity, rotationSpeed,UtilGenerateRandomNumber(1.5f, 0.5f), objectInstance->actorInstance.scale[0], 0, 0, 128, DEBRISFLAG_GRAVITY|DEBRISFLAG_ROTATESPRITE,0,0,0.0f);
				}
			}
			else
			{
				CreateDebrisActor(objectInstance->actorCRC, offset, velocity, rotationSpeed,UtilGenerateRandomNumber(1.5f, 0.5f), 0.25f*objectInstance->actorInstance.scale[0], 0, 0, 128, DEBRISFLAG_GRAVITY|DEBRISFLAG_ROTATESPRITE,0,0,0.0f);
			}
		}
		if(destroyOriginal)
		{
			// destroy tree
			RemoveActorFromCollisionCache(&collisionCache, objectInstance);
			FreeActorInstance(objectInstance);
		}
	}
}