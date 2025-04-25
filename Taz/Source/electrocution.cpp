// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : electrocution.cpp
//   Purpose : functions that fry taz
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"

#include "collisioncache.h"
#include "physics.h"
#include "scene.h"
#include "animation.h"
#include "electrocution.h"
#include "maths.h"
#include "status.h"
#include "health.h"
#include "fade.h"
#include "polyeffects.h"
#include "main.h"
#include "characters.h"
#include "costumes.h"
#include "genericai.h"
#include "health.h"
#include "fade.h"
#include "cameracase.h"
#include "sfx.h"
#include "water.h"

TBActRenderMode_Outlined	electrocutionLineParms BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateHighVoltageObject
   Purpose : creates an object which can, if turned on, electrocute a character
   Parameters : ACTORINSTANCE *
   Returns :
   Info :
*/

int CreateHighVoltageObject(ACTORINSTANCE *actor, int type, char state)
{
	if(!CreateHighVoltageInfo(actor)) return FALSE;

	//set attributes
	actor->special = SPECIALTYPE_HIGHVOLTAGE;
	if(state) 
	{
		actor->highVoltageObjectInfo->currentState = type;
	}
	else
	{
		actor->highVoltageObjectInfo->currentState = HIGHVOLTAGE_OFF;
	}
	actor->highVoltageObjectInfo->possibleState = type;
	actor->highVoltageObjectInfo->resetTime = 0.0f;

	AddActorToCollisionCache(&collisionCache,actor, 0);
	SetCollisionCallback(actor, RespondToHighVoltageCollision);
	if (actor->actorInstance.numAnimSegments)
	{
		baQueueNodeAnimNormal(&actor->actorInstance, ROOTNODE(actor->actorInstance.rootNodeInstance), 
					&actor->actorInstance.animSegments[0], 512+(bmRand()%200), -1, BANIMQUEUEFLAG_LOOPING, NULL,NULL);
	}

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : CreateHighVoltageInfo
   Purpose : creates info structure
   Parameters : ACTORINSTANCE *
   Returns :
   Info :
*/

int CreateHighVoltageInfo(ACTORINSTANCE *actor)
{
	actor->highVoltageObjectInfo = (HIGHVOLTAGEOBJECTINFO*)ZALLOC(sizeof(HIGHVOLTAGEOBJECTINFO));
	if (actor->highVoltageObjectInfo)
	{
		return TRUE;
	}
	else
	{
		bkPrintf("*** ERROR *** Could not create springy object info structure\n");
		return FALSE;
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateHighVoltageObject
   Purpose : updates high voltage objects
   Parameters : ACTORINSTANCE *
   Returns :
   Info :
*/

void UpdateHighVoltageObject(ACTORINSTANCE *actor)
{
	// CMD: the following two checks DO make sense, only want to decrement the clock
	// CMD: belonging to objects whose clocks are greater than zero
	// CMD: There is the possibility that objects may be inactive, but not timed to
	// CMD: switch themselves back on again
	if(actor->highVoltageObjectInfo->resetTime > 0.0f)
	{
		if((actor->highVoltageObjectInfo->resetTime -= fTime) < 0.0f)
		{
			actor->highVoltageObjectInfo->currentState = actor->highVoltageObjectInfo->possibleState;
			actor->highVoltageObjectInfo->resetTime = 0.0f;
		}
	}
}
/*
static void ElectrocuteStateChangeCallback(ACTORINSTANCE *actorInstance)
{
	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance, "electrocute", 1.0f, 0, 0, 0.1f, NONE);
	PlayAnimationByName(actorInstance, "electrocute", 1.0f, 0, 1, 0.1f, NONE);
	PlayAnimationByName(actorInstance, "electrocute", 1.0f, 0, 1, 0.1f, NONE);
	PlayAnimationByName(actorInstance, "electrocute", 1.0f, 0, 1, 0.1f, NONE);
}
*/

/*static void ElectrocuteGroundStateChangeCallback(ACTORINSTANCE *actorInstance)
{
	PlayAnimationByName(actorInstance, "electrofly", 1.0f, 1, 0, 0.1f, DEATH);
}*/

/* --------------------------------------------------------------------------------
   Function : FinishedFadeFromBallPuzzle
   Purpose : 
   Parameters : FADE *
   Returns : 
   Info : 
*/

void FinishedFadeFromBallPuzzle(FADE *fade)
{ 	
	ACTORINSTANCE	*resetPt;
	TBMatrix		matrix;
	TBQuaternion	quat;
	TBVector		tempVector;
	ACTORINSTANCE	*pPlayer = fade->GetPlayer();

	resetPt = FindActorInstanceInMap("ball_challenge_inst", 0, &map);
	if(resetPt)
	{
		if(pPlayer->characterInfo->costume != -1)
		{
			SetupCostume(pPlayer, pPlayer->characterInfo->costume);
		}
		else
		{
			ChangeActorInstance(pPlayer,FindActorInActorList("tazdevil.obe"),5,NULL);
		}
		RequestStateChange(pPlayer, STATE_MOVE);

		bmVectorCopy(pPlayer->actorInstance.position, resetPt->actorInstance.position);
		bmVectorSet(tempVector, 0.0f, 1.0f, 0.0f, RAD(180.0f));
		bmRotationToQuat(quat, tempVector);
		bmQuatMultiply(pPlayer->actorStatus->waterResetQuaternion, quat, resetPt->actorInstance.orientation);

		if(gameStatus.player[pPlayer->playerNo].camera->cameraCase)
		{
			StopCameraCase(gameStatus.player[pPlayer->playerNo].camera, gameStatus.player[pPlayer->playerNo].camera->cameraCase);
		}
		ResetCamera(gameStatus.player[pPlayer->playerNo].camera);
		bmVectorSet(tempVector, 0.0f, METERTOUNIT(2.0f), METERTOUNIT(2.0f), 1.0f);
		bmQuatToMatrix(matrix, pPlayer->actorStatus->waterResetQuaternion);
		bmMatMultiplyVector(matrix, tempVector);
		bmVectorAdd(gameStatus.player[pPlayer->playerNo].camera->pos, tempVector, resetPt->actorInstance.position);
		bmVectorCopy(gameStatus.player[pPlayer->playerNo].camera->destinationPos, gameStatus.player[pPlayer->playerNo].camera->pos);
	}
#ifdef _TAZDEBUG
	else
	{
		bkPrintf("could not find ball puzzle reset point\n");
	}
#endif

	fade->SetFadeDownFinishedCallback(NULL);
}

/* --------------------------------------------------------------------------------
   Function : RespondToHighVoltageCollision
   Purpose : 
   Parameters : ACTORINSTANCE *
   Returns :
   Info :
*/

int RespondToHighVoltageCollision(ACTORINSTANCE *collider, ACTORINSTANCE *object)
{
	TBVector			yDirection = {0.0f, 1.0f, 0.0f, 1.0f};
	TBVector			offset, screenSpace;
	int					random;

	if (collider->genericAI) 
	{
		AICollideWithSpecial(collider,object);
		return FALSE;
	}
	if(collider->flags&ACTORFLAG_DEBRISACTOR) return FALSE;
	if(collider->actorStatus->currentState==STATE_VEHICLE) return FALSE;
	if(collider->playerNo == -1) return FALSE;

	if((object->highVoltageObjectInfo)&&(object->highVoltageObjectInfo->currentState != HIGHVOLTAGE_OFF))
	{
		switch(collider->actorStatus->currentState)
		{
		case STATE_VEHICLE:
			return FALSE;
		case STATE_BALL:
			collider->flags |= ACTORFLAG_DONTDRAW;
			collider->flags |= ACTORFLAG_NOVELOCITY;
			bmVectorSet(collider->actorBehaviour->physics.velocity, 0.0f, 0.0f, 0.0f, 1.0f);
			AddActorToCollisionCache(&collisionCache, collider, 0);
			bmVectorSet(offset, 0.0f, 0.0f, 0.0f, 1.0f);
			CreatePolyLevelCopyOfActorInstance(&collisionCache, &polyEffectsList, collider, offset, FALSE, bkLoadTexture(NULL, "extras\\ball.bmp", 0), POLYFX_EXPLODE);
			RemoveActorFromCollisionCache(&collisionCache, collider);

			gameStatus.player[collider->playerNo].camera->fade.StartFadeDown(FADETYPE_CIRCLE, 1.5f);
			gameStatus.player[collider->playerNo].camera->fade.SetPlayer(collider);
			gameStatus.player[collider->playerNo].camera->fade.SetFadeDownFinishedCallback(FinishedFadeFromBallPuzzle);
			gameStatus.player[collider->playerNo].camera->fade.SetFadeUpFinishedCallback(ResetFadeToPoint);
			bdSetIdentityObjectMatrix();
			bmVectorSet(offset, 0.0f, 0.0f, 0.0f, 1.0f);
			bmVectorAdd(offset, offset, collider->actorInstance.position);
			offset[3] = 1.0f;
			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
			bdSetIdentityObjectMatrix();
			bdProjectVertices(screenSpace, offset, 1);
			gameStatus.player[collider->playerNo].camera->fade.SetCircleFadeTo(screenSpace[0], screenSpace[1]);
			gameStatus.player[collider->playerNo].camera->fade.FadeUpOnCompletion(true);

			PlaySample("crashchina.wav", 255);

			break;
		default:
			if(object->highVoltageObjectInfo->currentState == HIGHVOLTAGE_WALL)
			{
				ReduceCharactersHealth(collider,1.0f);
				if (RequestStateChange(collider, STATE_ELECTROCUTED))
				{
					if (UpdateCharacterState(&gameStatus.player[collider->playerNo], true))
					{
						PlaySample("electricspark.wav",255,collider->actorInstance.position);
						if(characterSounds.sam == -1)
						{
							random = bmRand()%2;
							switch(random)
							{
							case 0:
								CallbackWhenSampleFinished((characterSounds.sam=PlaySample("sam_voc_10b.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.sam);
								break;
							case 1:
								CallbackWhenSampleFinished((characterSounds.sam=PlaySample("sam_voc_11a.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.sam);
								break;
							}
						}

						RequestRumbleEffect(gameStatus.player[collider->playerNo].controller, 255, 0.0f, 0.5f, 1.0f, 0.0, WAVEFORM_RANDOM);
						RequestBuzzerEffect(gameStatus.player[collider->playerNo].controller, 1.0f, 0.0, WAVEFORM_RANDOM);
						FlushAnimationQueue(collider);
						PlayAnimationByName(collider, "electrocute", 1.0f, 0, 0, 0.1f, NONE);
						PlayAnimationByName(collider, "electrocute", 1.0f, 0, 1, 0.1f, NONE);
						PlayAnimationByName(collider, "electrocute", 1.0f, 0, 1, 0.1f, NONE);
						PlayAnimationByName(collider, "electrocute", 1.0f, 0, 1, 0.1f, NONE);
					}
					else
					{
						ClearStateChangeRequest(collider);
					}
				}
			}
			else
			{
				if(object->highVoltageObjectInfo->currentState == HIGHVOLTAGE_GROUND)
				{	
					if(!CheckIfPlayingAnimation(collider, "electrofly"))
					{
						if (RequestStateChange(collider, STATE_GROUNDELECTROCUTED))
						{
							if (UpdateCharacterState(&gameStatus.player[collider->playerNo], true))
							{
								PlaySample("electricspark.wav",255,collider->actorInstance.position);
								if(characterSounds.sam == -1)
								{
									random = bmRand()%2;
									switch(random)
									{
									case 0:
										CallbackWhenSampleFinished((characterSounds.sam=PlaySample("sam_voc_10b.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.sam);
										break;
									case 1:
										CallbackWhenSampleFinished((characterSounds.sam=PlaySample("sam_voc_11a.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.sam);
										break;
									}
								}

								ReduceCharactersHealth(collider, 1.0f);
								PlayAnimationByName(collider, "electrofly", 1.0f, 1, 0, 0.1f, DEATH);
								RequestRumbleEffect(gameStatus.player[collider->playerNo].controller, 255, 0.0f, 0.5f, 1.0f, 0.0, WAVEFORM_RANDOM);
								RequestBuzzerEffect(gameStatus.player[collider->playerNo].controller, 1.0f, 0.0, WAVEFORM_RANDOM);
								StopCollidingWith(collider, object);
								collider->actorBehaviour->physics.flags |= PHYSFLAG_NOMOVE;
								collider->actorStatus->electrocutionTime = 0.0f;
							}
							else
							{
								ClearStateChangeRequest(collider);
							}
						}
					}
				}
			}
			break;
		}
		object->highVoltageObjectInfo->currentState = HIGHVOLTAGE_OFF;
		object->highVoltageObjectInfo->resetTime = ELECTRIC_RESET_TIME;
		KillAllPowerUps(collider);
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : UpdateElectrocutionEffect
   Purpose : 
   Parameters :
   Returns :
   Info :
*/

void UpdateElectrocutionEffect(ACTORINSTANCE *actorInstance)
{
	int				colour, alpha;
	float			thickness;

	//update cartoon outline
	colour = bmRand()%255;
	alpha = bmRand()%255;
	thickness = (float)(bmRand()%70)*0.1f;

	electrocutionLineParms.r = colour;
	electrocutionLineParms.g = colour;
	electrocutionLineParms.b = colour;
	electrocutionLineParms.a = colour;
	electrocutionLineParms.thickness = thickness + 3.5f;

	if ((!(gameStatus.customOptions&CUSTOMOPTION_DISABLEOUTLINES)))
		baSetActorRenderMode(&actorInstance->actorInstance, 1, BACTRENDERMODE_OUTLINED, &electrocutionLineParms);
}
