// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : trolley.cpp
//   Purpose : provides character responce code
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "physics.h"
#include "scene.h"
#include "status.h"
#include "vehicles.h"
#include "attach.h"
#include "animation.h"
#include "maths.h"
#include "main.h"
#include "control.h"
#include "holysmoke.h"
#include "vehicles.h"
#include "prompt.h"
#include "genericai.h"

/* --------------------------------------------------------------------------------
   Function : CreateTrolley
   Purpose : Creates and initialises actorInstances that act as trolleys
   Parameters : 
   Returns : 
   Info : 
*/

char	GoingFast;

void CreateTrolley(ACTORINSTANCE *trolley)
{
	AddActorToCollisionCache(&collisionCache, trolley, CACHEFLAGS_LIVEUPDATE);
	SetCollisionCallback(trolley, RespondToTrolleyCollision);
}

/* --------------------------------------------------------------------------------
   Function : RespondToTrolleyCollision
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

int RespondToTrolleyCollision(ACTORINSTANCE *collider, ACTORINSTANCE *trolley)
{
	TBVector		relativePosition = {0.0f, 0.0f, 0.0f, 1.0f};
	TBVector		relativeScale = {1.0f, 1.0f, 1.0f, 1.0f};
	TBActorInstance	*actorInstance;

	if (collider->genericAI) 
	{
		AICollideWithSpecial(collider,trolley);
		return FALSE;
	}

	if((~collider->flags&ACTORFLAG_DEBRISACTOR)&&
		(collider->actorStatus->currentState == STATE_JUMP)&&
		(collider->actorStatus->timeWithoutVehicleCollision > 0.3f))
	{
		// JW: **************************
		if (RequestStateChange(collider, STATE_VEHICLE))
		{
			if (UpdateCharacterState(&gameStatus.player[collider->playerNo], true))
			{
				trolley->vehicleInfo->state = VEHICLE_STATE_ON;
				collider->actorStatus->vehicleInfo = trolley->vehicleInfo;
				collider->actorStatus->lastState = collider->actorStatus->currentState;
				collider->actorStatus->currentState = STATE_VEHICLE;

				//change field of view
				if (collider->playerNo != -1)
				{
					SetFixedDistanceToLookAt(gameStatus.player[collider->playerNo].camera, METERTOUNIT(2.0f));
					SetCameraSmooth(gameStatus.player[collider->playerNo].camera, 0.2f, 0.2f);
				}

				//set orientation of collider to match rocketbike
				SetActorOrientation(collider, trolley->actorInstance.orientation);
				// CMD: also need to match Taz's position to that of the floor polisher
				SetActorPosition(collider, trolley->actorInstance.position);
				//set actor velocity
				SETVECTOR(collider->actorBehaviour->physics.velocity, 0.0f, 0.0f, 0.0f, 1.0f);

				trolley->vehicleInfo->handle = AttachObjectToCharacter(collider, trolley->actorName, "motion", relativePosition, relativeScale, bIdentityQuaternion, 0);
				actorInstance = ReturnAttachedObjectInstance(collider, trolley->vehicleInfo->handle);
				if(!actorInstance)
				{
					bkPrintf("*** ERROR ***, could not find trolley.obe\n");
					return FALSE;
				}
				DisableActorCartoonRenderMode(actorInstance);
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance, "drive", 1.0f, 1, 0, 0.1f, NONE);
				FlushAnimationQueue(collider);
				PlayAnimationByName(collider, "shopasusual", 1.0f, 1, 0, 0.1f, NONE);

		//		CallbackWhenSampleFinished(PlaySample("", 255, collider->actorInstance.position, 0),RocketbikeSoundCallback,collider);

				RemoveActorFromCollisionCache(&collisionCache, trolley);
				trolley->flags |= ACTORFLAG_DONTDRAW;

//				if (collider->playerNo!=-1)
//				{
//					EnableCameraRotateBehind(gameStatus.player[collider->playerNo].camera, RAD(90.0f));
//				}

				//turn motion bone off
				collider->actorAnimation->useMotionBone = FALSE;
				SetActorMinFricAngle(collider->actorBehaviour, 1.0f);
				collider->actorBehaviour->physics.flags |= PHYSFLAG_WALLGROUNDCOLLFIX;
				collider->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;

				// CMD: zero velocity
				SETVECTOR(collider->actorBehaviour->physics.velocity, 0.0f, 0.0f, 0.0f, 1.0f);
				trolley->vehicleInfo->clock = 1.0f;
				trolley->vehicleInfo->rate = 0.0f;
				trolley->vehicleInfo->numSoFar = 0;

				RequestMorronicMessage(MEVENT_TROLLEY);
			}
			else
			{
				ClearStateChangeRequest(collider);
			}
		}
	}

	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToTrolleyControl
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

void CharacterRespondToTrolleyControl(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance)
{
	VEHICLEINFO		*info = actorInstance->actorStatus->vehicleInfo;
	TBQuaternion	inputQuat,tempQuat;
	float			turnInput, length;
	float			scaleFactor = 0.95f;
	TBVector		tempVelocity;
	TBVector		rotatedVelocity, scaledVelocity;
	TBVector		accelComponant;
	TBVector		yAxis;
	TBVector		directionVector;
	TBMatrix		tempMatrix;

	//turn motion bone off just in case
	actorInstance->actorAnimation->useMotionBone = FALSE;
	actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_WALLGROUNDCOLLFIX;
	actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;
	//zero time without vehicle collision
	actorInstance->actorStatus->timeWithoutVehicleCollision = 0.0f;

	turnInput = -controller->x1 * MAX_TROLLEY_TURN * fTime;

	SETVECTOR(yAxis, 0.0f, 1.0f, 0.0f, turnInput);
	bmRotationToQuat(inputQuat, yAxis);
	bmQuatMultiply(tempQuat, inputQuat, actorInstance->actorInstance.orientation);

	// CMD: this line removed because we now need to be able to turn on the spot
//	if(bmVectorLen(actorInstance->actorBehaviour->physics.velocity) > METERTOUNIT(0.25f))
	{
		SetActorOrientation(actorInstance, tempQuat);
	}
	//calculate vector for direction
	bmQuatToMatrix(tempMatrix, tempQuat);
	SETVECTOR(directionVector, 0.0f, 0.0f, -1.0f, 1.0f);
	bmMatMultiplyVector(tempMatrix, directionVector);
	bmVectorNorm(directionVector, directionVector);

	// CMD: first things first, calculate rate at which circle is being pressed
	if(controller->circleDebounceChannel->value) info->numSoFar++;
	if((info->clock -= fTime) < 0.0f)
	{
		info->clock = 1.0f;
		info->rate = (float)info->numSoFar;
		info->numSoFar = 0;
	}

	// only add speed if on ground
	if ((actorInstance->actorStatus->timeWithoutGroundCollision < 0.15f)||(actorInstance->actorStatus->timeWithoutWallCollision < 0.15f))
	{
		//copy current velocity to prevent shennanigans
		bmVectorCopy(tempVelocity, actorInstance->actorBehaviour->physics.velocity);
		//check for acceleration/brake input
		if(info->rate)
		{
			//accelerate
			if(bmVectorLen(tempVelocity))
			{
				bmVectorScaleToLength(accelComponant, directionVector, (TROLLEY_ACCELERATION*info->rate*fTime));
			}
			else
			{
				bmVectorScaleToLength(accelComponant, directionVector, (TROLLEY_ACCELERATION*info->rate*fTime));
			}
			bmVectorAdd(tempVelocity, tempVelocity, accelComponant);
			//check for max velocity
			if(bmVectorLen(tempVelocity) > MAX_TROLLEY_SPEED)
			{
				bmVectorScaleToLength(tempVelocity, tempVelocity, MAX_TROLLEY_SPEED);
			}
			//PlayAnimationByName(actorInstance, "trolleyaccelerate", 2.0f, 0, 0, 0.1f, JUMP);
		}
		//always decelerate
		{
			//brake
/*			if(bmVectorLen(tempVelocity))
			{
				bmVectorScaleToLength(accelComponant, tempVelocity, (TROLLEY_DECELERATION/fps));
			}
			else
			{
				bmVectorScaleToLength(accelComponant, directionVector, (TROLLEY_DECELERATION/fps));
			}
			//check to rule out reversing
			if(bmVectorLen(accelComponant) > bmVectorLen(tempVelocity))
			{
				SETVECTOR(tempVelocity, 0.0f, 0.0f, 0.0f, -1.0f);
			}
			else
			{
				bmVectorSub(tempVelocity, tempVelocity, accelComponant);
			}*/
			// CMD: allways subtract a little velocity
			length = bmVectorLen(tempVelocity);
			if((length -= TROLLEY_DECELERATION*fTime) > 0.0f)
			{
				bmVectorScaleToLength(tempVelocity, tempVelocity, length);
			}
			else
			{
				SETVECTOR(tempVelocity, 0.0f, 0.0f, 0.0f, 1.0f);
			}
		}
	
		if(bmVectorLen(tempVelocity))
		{
			if((!controller->circleDebounceChannel->value)||(bmFAbs(controller->x1) > 0.7f))
			{
				CreateImpactClouds(actorInstance->actorInstance.position, gameStatus.windVelocity, NULL, METERTOUNIT(0.1f), 0.75f, 0.0f, SMOKE_TAZSPIN);
			}
		}

		//play correct animation
		ChooseCorrectTazOnTrolleyAnimations(actorInstance, controller);

		//update velocity to point in correct direction
		//split updated velocity into two componants, point one in direction of trolley, and add the velocities
		bmVectorCMul(scaledVelocity, tempVelocity, scaleFactor);
		bmVectorScaleToLength(rotatedVelocity, directionVector, ((1.0f-scaleFactor)*bmVectorLen(tempVelocity)));
		bmVectorAdd(tempVelocity, scaledVelocity, rotatedVelocity);

		SetActorVelocity(actorInstance->actorBehaviour, tempVelocity);
	}
}

/* --------------------------------------------------------------------------------
   Function : ChooseCorrectTazOnTrolleyAnimations
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

void ChooseCorrectTazOnTrolleyAnimations(ACTORINSTANCE *actorInstance, PADCONTROLSTATUS *controller)
{
	TBActorInstance		*tbActorInstance;
	int					random;

	VEHICLEINFO			*vInfo = actorInstance->actorStatus->vehicleInfo;

	tbActorInstance = ReturnAttachedObjectInstance(actorInstance, actorInstance->actorStatus->vehicleInfo->handle);

	if (!tbActorInstance) return;
		
	if ((actorInstance->actorStatus->timeWithoutWallCollisionLastFrame > 0.2f)&&(actorInstance->actorBehaviour->physics.coll))
	{
		//Taz
		if((!CheckIfPlayingAnimation(actorInstance, "trolleybounce1"))&&(!CheckIfPlayingAnimation(actorInstance, "trolleybounce2")))
		{
			StopChannel(&vInfo->soundHandle);
			
			// PP: previously, the rumble was being requested with an amplitude of 2 (too small to feel)
			// PP: I imagine the amplitude scale must have changed at some point?
#define TROLLEYBOUNCE_RUMBLE_AMPLITUDE		200		// PP: was 2
#define TROLLEYBOUNCE_RUMBLE_DURATION		0.25f	// PP: was 0.5f, which I thought was a bit long for a ground impact
													// CMD: the solenoids used in the pad don't seem to be responsive enough to differentate such small times

			random = bmRand()%2;
			switch(random)
			{
			case 0:
				PlayAnimationByName(actorInstance, "trolleybounce1", 1.0f, 0, 0, 0.1f, REACT);
				PlaySample("trolleycrash1.wav", 255, 0);
				RequestRumbleEffect(controller, TROLLEYBOUNCE_RUMBLE_AMPLITUDE, 0.05f, (0.5f-0.1f), TROLLEYBOUNCE_RUMBLE_DURATION, 0.0f, WAVEFORM_NORMAL, 0.0f, 0.0f, 0);
				break;
			case 1:
				PlayAnimationByName(actorInstance, "trolleybounce2", 1.0f, 0, 0, 0.1f, REACT);
				PlaySample("trolleycrash2.wav", 255, 0);
				RequestRumbleEffect(controller, TROLLEYBOUNCE_RUMBLE_AMPLITUDE, 0.05f, (0.5f-0.1f), TROLLEYBOUNCE_RUMBLE_DURATION, 0.0f, WAVEFORM_NORMAL, 0.0f, 0.0f, 0);
				break;
			}
		}
		//Trolley
		if((!CheckIfPlayingAnimation(tbActorInstance, "bounceonground"))&&(!CheckIfPlayingAnimation(tbActorInstance, "bounceoffwall")))
		{
			PlayAnimationByName(tbActorInstance, "bounceonground", 1.0f, 0, 0, 0.1f, REACT);
		}

		actorInstance->actorBehaviour->physics.coll = false;
	}
	if((controller->circleChannel->value) && (actorInstance->actorStatus->timeWithoutWallCollisionLastFrame < 0.1f) && (!gameStatus.multiplayer.countingDown))
	{
		//Taz
		if(!CheckIfPlayingAnimation(actorInstance, "trolleyaccelerate"))
		{
			if(bmVectorLen(actorInstance->actorBehaviour->physics.velocity) > METERTOUNIT(10.0f))
			{
				PlayAnimationByName(actorInstance, "trolleyaccelerate", 2.0f, 0, 0, 0.1f, JUMP);

				if (!vInfo->soundHandle)
					vInfo->soundHandle = PlaySample("loop\\trolleyfast.wav", 255, actorInstance->actorInstance.position);
				else if (!GoingFast)
				{
					StopChannel(&vInfo->soundHandle);
					vInfo->soundHandle = PlaySample("loop\\trolleyfast.wav", 255, actorInstance->actorInstance.position);
				}

				GoingFast = 1;
			}
			else
			{
				PlayAnimationByName(actorInstance, "trolleyaccelerate", 1.0f, 0, 0, 0.1f, JUMP);

				if (!vInfo->soundHandle)
					vInfo->soundHandle = PlaySample("loop\\trolleyslow.wav", 255, actorInstance->actorInstance.position);
				else if (GoingFast)
				{
					StopChannel(&vInfo->soundHandle);
					vInfo->soundHandle = PlaySample("loop\\trolleyslow.wav", 255, actorInstance->actorInstance.position);
				}

				GoingFast = 0;
			}
		}
	}
	if(controller->idleTime > 5.0f)
	{
		//Taz
		if((!CheckIfPlayingAnimation(actorInstance, "trolleygotoidle"))&&(!CheckIfPlayingAnimation(actorInstance, "trolleyidle")))
		{
			PlayAnimationByName(actorInstance, "trolleygotoidle", 1.0f, 0, 0, 0.1f, IDLE);
			PlayAnimationByName(actorInstance, "trolleyidle", 1.0f, 1, 1, 0.1f, IDLE);
		}
		//Trolley
		if(!CheckIfPlayingAnimation(tbActorInstance, "static"))
		{
			PlayAnimationByName(tbActorInstance, "static", 1.0f, 1, 0, 0.1f, IDLE);
		}

		StopChannel(&vInfo->soundHandle);
	}
	else
	{
		//Taz
		if(!CheckIfPlayingAnimation(actorInstance, "shopasusual"))
		{
			PlayAnimationByName(actorInstance, "shopasusual", 1.0f, 0, 0, 0.1f, MOVE);
		}
		//Trolley
		if(!CheckIfPlayingAnimation(tbActorInstance, "drive"))
		{
			PlayAnimationByName(tbActorInstance, "drive", 1.0f, 0, 0, 0.1f, MOVE);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : TrolleySoundCallback
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

void TrolleySoundCallback(void *context)
{
	ACTORINSTANCE	*actorInstance;

	actorInstance = (ACTORINSTANCE*)context;

//	actorInstance->actorStatus->vehicleInfo->soundHandle = PlaySample("", 255, actorInstance->actorInstance.position, BSPLAYFLAG_LOOPING);
}