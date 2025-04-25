// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : rocketscience.cpp
//   Purpose : it actually is rocket science, functions to fire rockets at a target
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "main.h"
#include "fireball.h"
#include "physics.h"
#include "rumble.h"
#include "fireworks.h"
#include "collectibles.h"
#include "characters.h"
#include "sfx.h"
#include "status.h"
#include "animation.h"
#include "health.h"
#include "textures.h"
#include "util.h"
#include "chillipepper.h"
#include "mtweetyscripts.h"
#include "mechatweety.h"
#include "rocketscience.h"
#include "bossgamecontinue.h"

/* --------------------------------------------------------------------------------
   Function : FireSeekingRocketFromActorNode
   Purpose : fires a rocket from a node on an actor, which then seeks another actor
   Parameters : offset represents the offset from the node to fire from
   Returns : 
   Info : 
*/

ACTORINSTANCE *FireSeekingRocketFromActorNode(ACTORINSTANCE *instance, ACTORINSTANCE *target, char *nodeName, TBVector offset)
{
	ACTORINSTANCE			*rocket;
	TBActorNodeInstance		*node;
	TBVector				velocity, offsetCopy, tempVec;
	TBQuaternion			orientation, modQuat, resultQuat;
	TBMatrix				matrix;
	int						rocketType;

	if(instance && target)
	{
		if(node = baFindNode(instance->actorInstance.rootNodeInstance, nodeName))
		{
			// CMD: copy offset to prevent shenanigans
			bmVectorCopy(offsetCopy, offset);
			// CMD: find position and orientation of node
			bmMatMultiply(matrix, instance->actorInstance.objectToWorld, node->nodeToLocalWorld);
			bmMatMultiplyVector(matrix, offsetCopy);

			bmMatDecompose(matrix, tempVec, velocity, orientation);
			// CMD: dirty bodge
			if(strcmpi(nodeName, "arml_lower") == 0)
			{
				SETVECTOR(tempVec, 0.0f, 1.0f, 0.0f, RAD(15.0f));
				bmRotationToQuat(modQuat, tempVec);
				bmQuatMultiply(resultQuat, modQuat, orientation);
			}
			else
			if(strcmpi(nodeName, "armr_lower") == 0)
			{
				SETVECTOR(tempVec, 0.0f, 1.0f, 0.0f, RAD(-15.0f));
				bmRotationToQuat(modQuat, tempVec);
				bmQuatMultiply(resultQuat, modQuat, orientation);
			}

			switch(rocketType = (bmRand()%7))
			{
			case 0:
				if(rocket = CreateActorInstance(&map.animatedInstances, "extras\\afootball_projectile.obe", "rocket", offsetCopy, instance->actorInstance.scale, resultQuat, 0))
				{
					FlushAnimationQueue(rocket);
					PlayAnimationByName(rocket, "rotate", 3.0f, 1, 0, 0.1f, IDLE);
				}
				break;
			case 1:
				if(rocket = CreateActorInstance(&map.animatedInstances, "extras\\boxingglove_projectile.obe", "rocket", offsetCopy, instance->actorInstance.scale, resultQuat, 0))
				{
					FlushAnimationQueue(rocket);
					PlayAnimationByName(rocket, "rotate", 3.0f, 1, 0, 0.1f, IDLE);
				}
				break;
			case 2:
				if(rocket = CreateActorInstance(&map.animatedInstances, "extras\\dart_projectile.obe", "rocket", offsetCopy, instance->actorInstance.scale, resultQuat, 0))
				{
					FlushAnimationQueue(rocket);
					PlayAnimationByName(rocket, "rotate", 3.0f, 1, 0, 0.1f, IDLE);
				}
				break;
			case 3:
				rocket = CreateActorInstance(&map.levelInstances, "extras\\plunger_projectile.obe", "rocket", offsetCopy, instance->actorInstance.scale, resultQuat, 0);
				break;
			case 4:
				if(rocket = CreateActorInstance(&map.animatedInstances, "extras\\rubberchicken_projectile.obe", "rocket", offsetCopy, instance->actorInstance.scale, resultQuat, 0))
				{
					FlushAnimationQueue(rocket);
					PlayAnimationByName(rocket, "rotate", 3.0f, 1, 0, 0.1f, IDLE);
				}
				break;
			case 5:
				if(rocket = CreateActorInstance(&map.animatedInstances, "extras\\shuttlecock_projectile.obe", "rocket", offsetCopy, instance->actorInstance.scale, resultQuat, 0))
				{
					FlushAnimationQueue(rocket);
					PlayAnimationByName(rocket, "rotate", 3.0f, 1, 0, 0.1f, IDLE);
				}
				break;
			case 6:
				if(rocket = CreateActorInstance(&map.animatedInstances, "extras\\teeth_projectile.obe", "rocket", offsetCopy, instance->actorInstance.scale, resultQuat, 0))
				{
					FlushAnimationQueue(rocket);
					PlayAnimationByName(rocket, "rotate", 3.0f, 1, 0, 0.1f, IDLE);
				}				
				break;
			}
			if(rocket)
			{
				EnableActorCartoonRenderMode(&rocket->actorInstance);

				rocket->special = SPECIALTYPE_ROCKET;
				rocket->flags |= ACTORFLAG_DONTFADE;
				rocket->rocketInfo = (ROCKETINFO*)ZALLOC(sizeof(ROCKETINFO));
				ASSERTM(rocket->rocketInfo, "out of memory");

				// CMD: attributes ahoy
				rocket->rocketInfo->speed = ROCKETPROPS_SPEED;			// CMD: the velocity is set by the update
				rocket->rocketInfo->target = target;					// CMD: actor at which the rocket is aimed
				rocket->rocketInfo->clock = ROCKETPROPS_LIFE;
				PlaySample("tweetyfire.wav", 255, rocket->actorInstance.position);
				switch(rocketType)
				{
				case 6:
					rocket->rocketInfo->soundHandle = PlayTrackingSample("loop\\teethchatterloop.wav", 255, rocket->actorInstance.position);
					break;
				default:
					rocket->rocketInfo->soundHandle = PlayTrackingSample("loop\\jetloop2.wav", 255, rocket->actorInstance.position);
					break;
				}

				return rocket;
			}
		}
	}

	return NULL;
}

void myRocketsStateChange(ACTORINSTANCE *actorInstance)
{
	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance, "tazdie1", 1.0f, 0, 0, 0.1f, DEATH);
}

/* --------------------------------------------------------------------------------
   Function : UpdateSeekingRockets
   Purpose : Updates rocket that seek their target
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateSeekingRockets(ACTORINSTANCE *rocket)
{
	ACTORINSTANCE	*target = rocket->rocketInfo->target;
	TBVector		direction, aimPos, velocity, smallVel, position;
	TBMatrix		matrix;
	TBQuaternion	orientation, targetQuat, tempQuat;
	float			size;
	int				i, colour[3];

	if(rocket->rocketInfo)
	{
		// CMD: calculate velocity
		SETVECTOR(direction, 0.0f, 0.0f, -1.0f, 1.0f);
		bmQuatToMatrix(matrix, rocket->actorInstance.orientation);
		bmMatMultiplyVector(matrix, direction);

		bmVectorScaleToLength(rocket->rocketInfo->velocity, direction, rocket->rocketInfo->speed);

		// CMD: update position
		bmVectorCMul(direction, rocket->rocketInfo->velocity, fTime);
		bmVectorAdd(rocket->actorInstance.position, rocket->actorInstance.position, direction);

		// CMD: update orientation, by moving towards pointing at Taz
		if((rocket->rocketInfo->clock -= fTime) < (ROCKETPROPS_LIFE-0.5f))
		{
			bmVectorAdd(aimPos, rocket->rocketInfo->target->actorInstance.position, rocket->rocketInfo->target->actorBehaviour->collSphere.offset);
			bmVectorSub(direction, aimPos, rocket->actorInstance.position);
			bmDirectionToQuat(orientation, direction, 0.0f);

			bmQuatSlerpAtFixedSpeed(targetQuat, rocket->actorInstance.orientation, orientation, RAD(90.0f)*fTime);
			SetActorOrientation(rocket, targetQuat);

					// CMD: create flames from rear end of rockets
					for(i=0;i<2;i++)
					{
						SETVECTOR(velocity, UNITTOMETER((float)((bmRand()%(CHILLIPEPPER_FIRE_DISP*2))-CHILLIPEPPER_FIRE_DISP)), 
											UNITTOMETER((float)((bmRand()%(CHILLIPEPPER_FIRE_DISP)))), 
											(-1.0f-UNITTOMETER((float)((bmRand()%(CHILLIPEPPER_FIRE_DISP*2))-CHILLIPEPPER_FIRE_DISP))), 1.0f);
						//need small displacement to seperate the sprites early in their life time
						SETVECTOR(smallVel,0.0f,0.0f,-((float)(bmRand()%CHILLIPEPPER_FIRE_RANDDISP)),1.0f);

						if((size = CHILLIPEPPER_FIRE_MAXSIZE*velocity[2]/(int)CHILLIPEPPER_FIRE_RANDDISP) < CHILLIPEPPER_FIRE_MINSIZE)
						{
							size = CHILLIPEPPER_FIRE_MINSIZE;
						}
						// CMD: rotate velocity to the correct position
//						SETVECTOR(position, 1.0f, 0.0f, 0.0f, RAD(60.0f));
//						bmRotationToQuat(orientation, position);
//						bmQuatMultiply(tempQuat, orientation, mechaTweetyInfo->actorInstance->actorInstance.orientation);
						bmQuatToMatrix(matrix, rocket->actorInstance.orientation);
						bmMatMultiplyVector(matrix, velocity);
						bmVectorScaleToLength(velocity, velocity, METERTOUNIT(-3.0f));

						bmMatMultiplyVector(matrix, smallVel);

						bmVectorAdd(position, rocket->actorInstance.position, smallVel);

						if(bmRand()%2)
						{
							colour[0]=255;colour[1]=18;colour[2]=5;
						}
						else
						{
							colour[0]=255;colour[1]=186;colour[2]=10;
//							colour[0] = 127;colour[1] = 93;colour[2] = 5;
						}
						float	tempFloat = CHILLIPEPPER_FIRE_LIFE;
						float	rand = UtilGenerateRandomNumber(RAD(360.0f), 0.0f);
#if BPLATFORM == PS2
						baCreateExtra(chilliFireSystem,position,velocity,textureChilliFire,&tempFloat,&rand,&size,colour[0],colour[1],colour[2],127,DEBRISFLAG_CHILLIFIRE|DEBRISFLAG_DONTDOCOLLISION);
#else
						baCreateExtra(chilliFireSystem,position,velocity,textureChilliFire,&tempFloat,&rand,&size,colour[0],colour[1],colour[2],127,DEBRISFLAG_CHILLIFIRE|DEBRISFLAG_INCEDENTALFIRE|DEBRISFLAG_DONTDOCOLLISION);
#endif
					}

			// CMD: destroy conditions
			if(bmVectorLen(direction) < METERTOUNIT(0.5f))
			{
				mTweetyGenericInfo->flags |= MTWEETY_HITBYROCKET;

				if(target->actorStatus->currentState == STATE_BITE)
				{
					EatRocket(rocket);
				}
				else
				if(globalMechaTweety->mechaTweetyInfo->flags & MTWEETY_TWEETYNOMOVE)
				{
#if BPLATFORM == PS2
					return DestroySeekingRocketQuietly(rocket);				// CMD: rocket no longer exists
#else
					DestroySeekingRocketQuietly(rocket);
					return;
#endif
				}
				else
				{
					if(rocket->rocketInfo->target->characterInfo->tazShieldInfo)
					{
						if(!CheckIfPlayingAnimation(rocket->rocketInfo->target, "react1"))
						{
							if(!ReduceShieldHealthByOne(rocket->rocketInfo->target))
							{
								// CMD: make sure Taz and Tweety are doing the right thing
								RequestNewMechaTweetySequence(globalMechaTweety, holdAtPosition);
								globalMechaTweety->mechaTweetyInfo->flags |= MTWEETY_TWEETYNOMOVE;
								bmQuatCopy(tempQuat,target->actorInstance.orientation);
								ChangeActorInstance(target,FindActorInActorList("tazdevil.obe"),5,NULL);
								bmQuatCopy(target->actorInstance.orientation,tempQuat);
								ResetCharacterAfterJump(target);
//								RequestStateChangePlus(target, STATE_PLAYANIMANDFREEZE, myRocketsStateChange);
								if(RequestStateChange(target, STATE_PLAYANIMANDFREEZE))
								{
									if(UpdateCharacterState(&gameStatus.player[target->playerNo], true))
									{
										FlushAnimationQueue(target);
										PlayAnimationByName(target, "tazdie1", 1.0f, 0, 0, 0.1f, DEATH);
									}
									else
									{
										ClearStateChangeRequest(target);
									}
								}
								mTweetyGenericInfo->flags |= MTWEETY_DONTUPDATECAMERA;
								ProcessTazDeathAtEndOfBossGame(ResetMechaTweetyCallback, (void*)globalMechaTweety->mechaTweetyInfo);
//								bkGenerateEvent("scenechange","_tazboss2");
#if	BPLATFORM == PS2
								return DestroySeekingRocketQuietly(rocket);						// CMD: rocket no longer exists
#else	
								DestroySeekingRocketQuietly(rocket);
								return;
#endif
							}
						}
					}
#if BPLATFORM == PS2
					return DestroySeekingRocketWithLoudBang(rocket);				// CMD: rocket no longer exists
#else
					DestroySeekingRocketWithLoudBang(rocket);
					return;
#endif
				}

			}
			else
			if(rocket->actorInstance.position[1] < -500.0f)
			{
#if	BPLATFORM == PS2
				return DestroySeekingRocketQuietly(rocket);						// CMD: rocket no longer exists
#else
				DestroySeekingRocketQuietly(rocket);
				return;
#endif
			}
			else
			if(rocket->rocketInfo->clock < 0.0f)
			{
#if	BPLATFORM == PS2
				return DestroySeekingRocketQuietly(rocket);						// CMD: rocket no longer exists
#else
				DestroySeekingRocketQuietly(rocket);
				return;
#endif
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : DestroySeekingRocketWithLoudBang
   Purpose : destroys and frees the rocket when an explosion is required
   Parameters : 
   Returns : 
   Info : 
*/

void DestroySeekingRocketWithLoudBang(ACTORINSTANCE *rocket)
{
	// CMD: explosion
//	COLOUR	expCols[FB_NUM_STAGE_COLOURS]={FBP_COLOUR_HOT, FBP_COLOUR_COLD, FBP_COLOUR_SMOKE, FBP_COLOUR_FINAL};
//	Fireball_normalExplosion(rocket->actorInstance.position, METERTOUNIT(0.5f), expCols[0], expCols[1], expCols[2], expCols[3], FIREBALLFLAG_NOCAMERASHAKE); 

//	RequestRumbleEffect(&controller1, 255, 0.1f, 0.5f, 0.5f, 0.0f, WAVEFORM_NORMAL, 10.0f, 0.0f, 0);
//	CreateFireworkBurst(rocket->actorInstance.position, FIREWORK_RANDOMTYPE, 0,0,0,FIREWORKS_RANDOMCOLOUR, 0.5f);
#define TWEETYROCKETS_NUMHITOPTIONS		(1)

	switch(bmRand()%TWEETYROCKETS_NUMHITOPTIONS)
	{
	case 0:
		RequestStateChange(rocket->rocketInfo->target,STATE_PLAYANIMATION);
		FlushAnimationQueue(rocket->rocketInfo->target);
		PlayAnimationByName(rocket->rocketInfo->target,"react1",1.0f,0,0,0.1f,NONE);
		RequestRumbleEffect(&controller1, 255, 0.2f, 0.5f, 1.25f, 0.0f, WAVEFORM_NORMAL);
		PlaySample("boing.wav", 255);
		ReduceCharactersHealth(rocket->rocketInfo->target,0.5f);
		break;
	}

	globalMechaTweety->mechaTweetyInfo->rocketsRumbleHandle = FlushRumbleQueueMember(&controller1, globalMechaTweety->mechaTweetyInfo->rocketsRumbleHandle);
	StopChannel(&rocket->rocketInfo->soundHandle);
	FreeActorInstance(rocket, 0);
}

/* --------------------------------------------------------------------------------
   Function : DestroySeekingRocketQuietly
   Purpose : destroys, and frees the rocket instance when an explosion is not required
   Parameters : 
   Returns : 
   Info : 
*/

void DestroySeekingRocketQuietly(ACTORINSTANCE *rocket)
{
	StopChannel(&rocket->rocketInfo->soundHandle);
	FreeActorInstance(rocket, 0);
}

/* --------------------------------------------------------------------------------
   Function : EatRocket
   Purpose : for when taz eats a rocket
   Parameters : 
   Returns : 
   Info : 
*/

void EatRocket(ACTORINSTANCE *rocket)
{
	ReduceCharactersHealth(rocket->rocketInfo->target, 0.5f);
	RequestStateChangePlus(rocket->rocketInfo->target,STATE_BADFOOD,EatBadFoodCallback);

	if(characterSounds.taz == -1)
	{
		switch(bmRand()%5)
		{
		case 0:
			CallbackWhenSampleFinished((characterSounds.taz=PlaySample("pickupbadfood1.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.taz);
			break;
		case 1:
			CallbackWhenSampleFinished((characterSounds.taz=PlaySample("pickupbadfood2.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.taz);
			break;
		case 2:
			CallbackWhenSampleFinished((characterSounds.taz=PlaySample("pickupbadfood3.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.taz);
			break;
		}
	}

	DestroySeekingRocketQuietly(rocket);
}