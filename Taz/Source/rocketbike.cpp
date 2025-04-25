// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : rocketbike.cpp
//   Purpose : rocketbike functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "scene.h"
#include "status.h"
#include "display.h"
#include "main.h"
#include "maths.h"
#include "physics.h"
#include "collisioncache.h"
#include "control.h"
#include "holysmoke.h"
#include "animation.h"
#include "vehicles.h"
#include "attach.h"
#include "rocketbike.h"
#include "genericai.h"

float	*animTime, oneTime = 0.0f, twoTime = 0.0f;

/* --------------------------------------------------------------------------------
   Function : CreateRocketbike
   Purpose : Creates and initialises actorInstances that act as rocketbikes
   Parameters : 
   Returns : 
   Info : 
*/

void CreateRocketbike(ACTORINSTANCE *rocketbike)
{
	FlushAnimationQueue(rocketbike);
	PlayAnimationByName(rocketbike, "idle1", 1.0f, 1, 0, 0.1f, IDLE);
	AddActorToCollisionCache(&collisionCache, rocketbike, CACHEFLAGS_LIVEUPDATE);
	SetCollisionCallback(rocketbike, RespondToRocketbikeCollision);
}

/* --------------------------------------------------------------------------------
   Function : RespondToRocketbikeCollision
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

int RespondToRocketbikeCollision(ACTORINSTANCE *collider, ACTORINSTANCE *rocketbike)
{
	TBVector		relativePosition = {0.0f, 0.0f, 0.0f, 1.0f};
	TBVector		relativeScale = {1.0f, 1.0f, 1.0f, 1.0f};
	TBActorInstance	*actorInstance;

	if (collider->genericAI) 
	{
		AICollideWithSpecial(collider,rocketbike);
		return FALSE;
	}
	if((~collider->flags&ACTORFLAG_DEBRISACTOR)&&
		(collider->actorStatus->currentState == STATE_JUMP)&&
		(collider->actorStatus->timeWithoutVehicleCollision > 0.3f))
	{
		if (RequestStateChange(collider, STATE_VEHICLE))
		{
			if (UpdateCharacterState(&gameStatus.player[collider->playerNo], true))
			{
				rocketbike->vehicleInfo->state = VEHICLE_STATE_ON;
				collider->actorStatus->vehicleInfo = rocketbike->vehicleInfo;
				collider->actorStatus->lastState = collider->actorStatus->currentState;
				collider->actorStatus->currentState = STATE_VEHICLE;

				//change field of view
				if (collider->playerNo != -1)
				{
					SetFixedDistanceToLookAt(gameStatus.player[collider->playerNo].camera, METERTOUNIT(0.75));
					//SetCameraSmooth(gameStatus.player[collider->playerNo].camera, 1.0f, 0.6f);

					//SetFixedDistanceToLookAt(gameStatus.player[collider->playerNo].camera, METERTOUNIT(2.0f));
					SetCameraSmooth(gameStatus.player[collider->playerNo].camera, 0.15f, 0.3f);
				}

				//set orientation of collider to match rocketbike
				SetActorOrientation(collider, rocketbike->actorInstance.orientation);
				// CMD: also need to match Taz's position to that of the floor polisher
				SetActorPosition(collider, rocketbike->actorInstance.position);
				//set actor velocity
				SETVECTOR(collider->actorBehaviour->physics.velocity, 0.0f, 0.0f, 0.0f, 1.0f);

				rocketbike->vehicleInfo->handle = AttachObjectToCharacter(collider, rocketbike->actorName, "motion", relativePosition, relativeScale, bIdentityQuaternion, 0);
				actorInstance = ReturnAttachedObjectInstance(collider, rocketbike->vehicleInfo->handle);
				if(!actorInstance)
				{
					bkPrintf("*** ERROR ***, could not find rocketbike.obe\n");
					return FALSE;
				}

				DisableActorCartoonRenderMode(actorInstance);
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance, "fly", 1.0f, 1, 0, 0.1f, NONE);
				FlushAnimationQueue(collider);
				PlayAnimationByName(collider, "rocketbikeride", 1.0f, 1, 0, 0.1f, NONE);

		//		CallbackWhenSampleFinished(PlaySample("", 255, collider->actorInstance.position, 0),RocketbikeSoundCallback,collider);

				RemoveActorFromCollisionCache(&collisionCache, rocketbike);
				rocketbike->flags |= ACTORFLAG_DONTDRAW;

				if (collider->playerNo!=-1)
				{
//					EnableCameraRotateBehind(gameStatus.player[collider->playerNo].camera, RAD(90.0f));
				}

				//turn motion bone off
				collider->actorAnimation->useMotionBone = FALSE;
				SetActorMinFricAngle(collider->actorBehaviour, 1.0f);
				collider->actorBehaviour->physics.flags |= PHYSFLAG_WALLGROUNDCOLLFIX;
				collider->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;

				if (!collider->playerNo)
					oneTime = 0.0f;
				else
					twoTime = 0.0f;
			}
		}
		else
		{
			ClearStateChangeRequest(collider);
		}
	}

	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToRocketbikeControl
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

void CharacterRespondToRocketbikeControl(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance)
{
	TBQuaternion	inputQuat,tempQuat, anotherQuat, finalQuat;
	float			turnInput;
	float			scaleFactor = 0.95f;
	float			spdPrcnt;
	TBVector		tempVelocity;
	TBVector		rotatedVelocity, scaledVelocity;
	TBVector		accelComponant;
	TBVector		yAxis;
	TBVector		directionVector, smokePos;
	TBMatrix		tempMatrix,transMat,result;

	//turn motion bone off just in case
	actorInstance->actorAnimation->useMotionBone = FALSE;
	actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_WALLGROUNDCOLLFIX;
	actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;
	
	//zero time without vehicle collision
	actorInstance->actorStatus->timeWithoutVehicleCollision = 0.0f;

	// JW: turn speed is exponentially based on analogue input
	// JW: Maximum speed
	// JW: and damping value (1.25)
	turnInput = (controller->x1 * controller->x1) * MAX_ROCKETBIKE_TURN * fTime * 1.25f;

	// JW: Make sure we turn the right way
	if (controller->x1 > 0)
		turnInput *= -1;

	SETVECTOR(yAxis, 0.0f, 1.0f, 0.0f, turnInput);
	
	bmRotationToQuat(inputQuat, yAxis);
	bmQuatMultiply(tempQuat, inputQuat, actorInstance->actorInstance.orientation);
	if(bmVectorLen(actorInstance->actorBehaviour->physics.velocity))
	{
		bmDirectionToQuat(anotherQuat, actorInstance->actorBehaviour->physics.velocity, 0.0f);
		bmQuatSlerp(finalQuat, tempQuat, anotherQuat, 0.1f);
	}
	else bmQuatCopy(finalQuat, tempQuat);

	SetActorOrientation(actorInstance, finalQuat);
	
	//calculate vector for direction
	bmQuatToMatrix(tempMatrix, tempQuat);
	SETVECTOR(directionVector, 0.0f, 0.0f, -1.0f, 1.0f);
	bmMatMultiplyVector(tempMatrix, directionVector);
	bmVectorNorm(directionVector, directionVector);

	// only add speed if on ground
	if ((actorInstance->actorStatus->timeWithoutGroundCollision < 0.15f)||(actorInstance->actorStatus->timeWithoutWallCollision < 0.15f))
	{
		//copy current velocity to prevent shennanigans
		bmVectorCopy(tempVelocity, actorInstance->actorBehaviour->physics.velocity);
		
		//check for acceleration/brake input
		actorInstance->actorStatus->vehicleInfo->controllerInputTimer += fTime;
		
		if(controller->circleChannel->value)
		{
			if (actorInstance->actorStatus->soundHandle[0] == -1)
				actorInstance->actorStatus->soundHandle[0] = PlaySample("jetbikespeedup.wav", 255);
			
				//accelerate
			bmVectorScaleToLength(accelComponant, directionVector, (ROCKETBIKE_ACCELERATION*fTime));
		
			bmVectorAdd(tempVelocity, tempVelocity, accelComponant);
			
			//check for max velocity
			if(bmVectorLen(tempVelocity) > ROCKETBIKE_SPEED)
			{
				bmVectorScaleToLength(tempVelocity, tempVelocity, ROCKETBIKE_SPEED);
			}
		}
		else
		{
			if (actorInstance->actorStatus->soundHandle[0] != -1)
				StopChannel(&actorInstance->actorStatus->soundHandle[0]);

			//brake
			if(bmVectorLen(tempVelocity))
			{
				bmVectorScaleToLength(accelComponant, tempVelocity, (-ROCKETBIKE_ACCELERATION*fTime*0.5f));
			}
		}

		//play correct animation
		ChooseCorrectTazOnRocketbikeAnimations(actorInstance, controller);

		//update velocity to point in correct direction
		//split updated velocity into two componants, point one in direction of trolley, and add the velocities
		bmVectorCMul(scaledVelocity, tempVelocity, scaleFactor);
		bmVectorScaleToLength(rotatedVelocity, directionVector, ((1.0f-scaleFactor)*bmVectorLen(tempVelocity)));
		bmVectorAdd(tempVelocity, scaledVelocity, rotatedVelocity);

		SetActorVelocity(actorInstance->actorBehaviour, tempVelocity);
	}
	
	if((!controller->circleDebounceChannel->value)||(bmFAbs(controller->x1) > 0.7f))
	{
		SETVECTOR(smokePos,0.0f, METERTOUNIT(0.55f), METERTOUNIT(0.8f), 1.0f);
		bmMatTranslate(transMat,actorInstance->actorInstance.position[0],actorInstance->actorInstance.position[1],actorInstance->actorInstance.position[2]);
		bmMatMultiply(result,transMat,tempMatrix);
		bmMatMultiplyVector(result, smokePos);
		CreateImpactClouds(smokePos, gameStatus.windVelocity, NULL, METERTOUNIT(0.1f), 0.75f, 0.0f, SMOKE_TAZSPIN);
	}
	
// JW: Change the pitch of the rocket bike based on the speed

	if (bsGetChannelStatus(actorInstance->actorStatus->vehicleInfo->soundHandle) == BSCHSTATUS_PLAYING)
	{
		spdPrcnt = (bmVectorLen(actorInstance->actorBehaviour->physics.velocity) / ROCKETBIKE_SPEED);

		int frequency = (int) ((STANDARD_FREQUENCY - LOWEST_FREQUENCY) * spdPrcnt) + LOWEST_FREQUENCY;
		int	volume = (int) ((255.0f - 160.0f) * spdPrcnt) + 160;

		if (frequency > STANDARD_FREQUENCY)
			frequency = STANDARD_FREQUENCY;

		bsSetChannelFrequency(actorInstance->actorStatus->vehicleInfo->soundHandle, frequency);
		volume = CONSTRAIN(volume, 0, 255);
		bsSetChannelVolume(actorInstance->actorStatus->vehicleInfo->soundHandle, volume);
	}
}

/* --------------------------------------------------------------------------------
   Function : ChooseCorrectTazOnRocketbikeAnimations
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

void ChooseCorrectTazOnRocketbikeAnimations(ACTORINSTANCE *actorInstance, PADCONTROLSTATUS *controller)
{
	TBActorInstance		*tbActorInstance;
	int					flag;

	if (!actorInstance->playerNo)
		animTime = &oneTime;
	else
		animTime = &twoTime;

	tbActorInstance = ReturnAttachedObjectInstance(actorInstance, actorInstance->actorStatus->vehicleInfo->handle);
	flag = ANIMFLAG_UPDATE | ANIMFLAG_LOOP;
		
	if ((actorInstance->actorStatus->timeWithoutWallCollisionLastFrame > 0.2f)&&(actorInstance->actorBehaviour->physics.coll))
	{
		//Taz
		if((!CheckIfPlayingAnimation(actorInstance, "rocketbikereact1"))&&(!CheckIfPlayingAnimation(actorInstance, "rocketbikereact2")))
		{
			FlushAnimationQueue(actorInstance);
			PlayAnimationByName(actorInstance, "rocketbikereact2", 1.0f, 0, 0, 0.1f, REACT);
		}
		//rocketbike
		if((!CheckIfPlayingAnimation(tbActorInstance, "collidewall"))&&(!CheckIfPlayingAnimation(tbActorInstance, "collideground")))
		{
			FlushAnimationQueue(tbActorInstance);
			PlayAnimationByName(tbActorInstance, "collideground", 1.0f, 0, 0, 0.1f, REACT);
		}
	}
	else if(controller->x1 > 0.0f)
		*animTime += 0.1f;
	else if(controller->x1 < 0.0f)
		*animTime -= 0.1f;
	else
	{
		if (*animTime < 0.0f)
			*animTime += 0.1f;
		else if (*animTime > 0.0f)
			*animTime -= 0.1f;
	}

	if (*animTime < -1.0f)
		*animTime = -1.0f;
	else if (*animTime > 1.0f)
		*animTime = 1.0f;

	if (*animTime < 0)
	{
		PlayFixedBlendOfTwoAnimations(&actorInstance->actorInstance,"rocketbikeride","rocketbikeleftturn",&actorInstance->actorStatus->vehicleInfo->animBlendInfo,bmFAbs(controller->x1) * bmFAbs(*animTime),flag);
		PlayFixedBlendOfTwoAnimations(tbActorInstance,"fly","leftturn",&actorInstance->actorStatus->vehicleInfo->animBlendInfo,bmFAbs(controller->x1) * bmFAbs(*animTime),ANIMFLAG_LOOP);
	}
	else if (*animTime > 0)
	{
		PlayFixedBlendOfTwoAnimations(&actorInstance->actorInstance,"rocketbikeride","rocketbikerightturn",&actorInstance->actorStatus->vehicleInfo->animBlendInfo,controller->x1 * (*animTime),flag);
		PlayFixedBlendOfTwoAnimations(tbActorInstance,"fly","rightturn",&actorInstance->actorStatus->vehicleInfo->animBlendInfo,controller->x1 * (*animTime),ANIMFLAG_LOOP);
	}
}

/* --------------------------------------------------------------------------------
   Function : RocketbikeSoundCallback
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

void RocketbikeSoundCallback(void *context)
{
	ACTORINSTANCE	*actorInstance;

	actorInstance = (ACTORINSTANCE*)context;

//	actorInstance->actorStatus->vehicleInfo->soundHandle = PlaySample("", 255, actorInstance->actorInstance.position, BSPLAYFLAG_LOOPING);
}