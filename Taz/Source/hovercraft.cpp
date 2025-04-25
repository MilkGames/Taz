// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : hovercraft.cpp
//   Purpose : hovercraft functions
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
#include "characters.h"
#include "vehicles.h"
#include "genericai.h"

/* --------------------------------------------------------------------------------
   Function : CreateHovercraft
   Purpose : Creates and initialises actorInstances that act as hovercrafts
   Parameters : 
   Returns : 
   Info : 
*/

void CreateHovercraft(ACTORINSTANCE *hovercraft)
{
	AddActorToCollisionCache(&collisionCache, hovercraft, CACHEFLAGS_LIVEUPDATE);
	SetCollisionCallback(hovercraft, RespondToHovercraftCollision);
}

/* --------------------------------------------------------------------------------
   Function : RespondToHovercraftCollision
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

int RespondToHovercraftCollision(ACTORINSTANCE *collider, ACTORINSTANCE *hovercraft)
{
	TBVector		relativePosition = {0.0f, 0.0f, 0.0f, 1.0f};
	TBVector		relativeScale = {1.0f, 1.0f, 1.0f, 1.0f};
	TBActorInstance	*actorInstance;

	if (collider->genericAI) 
	{
		AICollideWithSpecial(collider,hovercraft);
		return FALSE;
	}

	if((~collider->flags&ACTORFLAG_DEBRISACTOR)&&
		(collider->actorStatus->currentState == STATE_JUMP)&&
		(collider->actorStatus->timeWithoutVehicleCollision > 0.3f))
	{
		hovercraft->vehicleInfo->state = VEHICLE_STATE_ON;
		hovercraft->vehicleInfo->currentRotation = 0.0f;
		hovercraft->vehicleInfo->currentRotationalVelocity = 0.0f;
		collider->actorStatus->vehicleInfo = hovercraft->vehicleInfo;
		RequestStateChange(collider, STATE_VEHICLE);

		//set orientation of collider to match trolley
		SetActorOrientation(collider, hovercraft->actorInstance.orientation);
		// CMD: also need to match Taz's position to that of the floor polisher
		SetActorPosition(collider, hovercraft->actorInstance.position);

		hovercraft->vehicleInfo->handle = AttachObjectToCharacter(collider, "objects\\hovercraft.obe", "motion", relativePosition, relativeScale, bIdentityQuaternion, 0);
		actorInstance = ReturnAttachedObjectInstance(collider, hovercraft->vehicleInfo->handle);
		if(!actorInstance)
		{
			bkPrintf("*** ERROR ***, could not find hovercraft.obe\n");
			return FALSE;
		}
		DisableActorCartoonRenderMode(actorInstance);
		FlushAnimationQueue(actorInstance);
		PlayAnimationByName(actorInstance, "polish", 1.0f, 1, 0, 0.1f, NONE);
		FlushAnimationQueue(collider);
		PlayAnimationByName(collider, "polish", 1.0f, 1, 0, 0.1f, NONE);

//		StopCollidingWith(collider, polisher);
		RemoveActorFromCollisionCache(&collisionCache, hovercraft);
		hovercraft->flags |= ACTORFLAG_DONTDRAW;

		if (collider->playerNo != -1)
		{
			SetFixedDistanceToLookAt(gameStatus.player[collider->playerNo].camera, METERTOUNIT(3.0f));
			SetCameraSmooth(gameStatus.player[collider->playerNo].camera, 0.7f, 0.9f);
		}

		//turn motion bone off
		collider->actorAnimation->useMotionBone = FALSE;
		SetActorMinFricAngle(collider->actorBehaviour, 1.0f);
		collider->actorBehaviour->physics.flags |= PHYSFLAG_WALLGROUNDCOLLFIX;
		collider->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;
	}

	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToHovercraftControl
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

void CharacterRespondToHovercraftControl(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance)
{
	TBMatrix	tempMatrix;
	TBVector	orientationVector;
	TBVector	tempVelocity;
	float		leftStick, rightStick;
	float		netRotationInput;
	float		netThrustInput;

	//turn motion bone off just in case
	actorInstance->actorAnimation->useMotionBone = FALSE;
	actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_WALLGROUNDCOLLFIX;
	actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;
	//zero time without vehicle collision
	actorInstance->actorStatus->timeWithoutVehicleCollision = 0.0f;

	leftStick = controller1.y1;
	rightStick = controller1.y2;

	// only add speed if on ground
	if ((actorInstance->actorStatus->timeWithoutGroundCollision < 0.15f)||(actorInstance->actorStatus->timeWithoutWallCollision < 0.15f))
	{
		//calculate vector to apply velocity along
		bmQuatToMatrix(tempMatrix, actorInstance->actorInstance.orientation);
		SETVECTOR(orientationVector, 0.0f, 0.0f, 1.0f, 1.0f);
		bmMatMultiplyVector(tempMatrix, orientationVector);
		bmVectorNorm(orientationVector, orientationVector);

		netThrustInput = (leftStick + rightStick)/2.0f;
		bmVectorScaleToLength(tempVelocity, orientationVector, (HOVERCRAFT_THRUST*netThrustInput/fps));
		bmVectorAdd(tempVelocity, tempVelocity, actorInstance->actorBehaviour->physics.velocity);
		//check for maximum velocity
		if(bmVectorLen(tempVelocity) > HOVERCRAFT_MAX_VELOCITY)
		{
			bmVectorScaleToLength(tempVelocity, tempVelocity, HOVERCRAFT_MAX_VELOCITY);
		}
		SetActorVelocity(actorInstance->actorBehaviour, tempVelocity);

		//set current rotational velocity
		netRotationInput = (leftStick - rightStick)/2.0f;
		if(bmFAbs(actorInstance->actorStatus->vehicleInfo->currentRotationalVelocity -= (netRotationInput*HOVERCRAFT_MAX_ROT_INPUT/fps)) > HOVERCRAFT_MAX_ROT_ACCEL)
		{
			actorInstance->actorStatus->vehicleInfo->currentRotationalVelocity = actorInstance->actorStatus->vehicleInfo->currentRotationalVelocity*HOVERCRAFT_MAX_ROT_ACCEL/bmFAbs(actorInstance->actorStatus->vehicleInfo->currentRotationalVelocity);
		}
		//reduce rotation when on ground
		if(actorInstance->actorStatus->vehicleInfo->currentRotationalVelocity)
		{
			actorInstance->actorStatus->vehicleInfo->currentRotationalVelocity = actorInstance->actorStatus->vehicleInfo->currentRotationalVelocity*(bmFAbs(actorInstance->actorStatus->vehicleInfo->currentRotationalVelocity)-(HOVERCRAFT_ROT_FRIC/fps))/bmFAbs(actorInstance->actorStatus->vehicleInfo->currentRotationalVelocity);
		}
	}
	//set rotation whether, or not, hovercraft is on ground
	if((actorInstance->actorStatus->vehicleInfo->currentRotation += (actorInstance->actorStatus->vehicleInfo->currentRotationalVelocity/fps)) > RAD(360.0f))
	{
		actorInstance->actorStatus->vehicleInfo->currentRotation -= RAD(360.0f);
	}
	if(actorInstance->actorStatus->vehicleInfo->currentRotation < RAD(360.0f))
	{
		actorInstance->actorStatus->vehicleInfo->currentRotation += RAD(360.0f);
	}
	CharacterRotateToAngle(actorInstance, 0.0f, actorInstance->actorStatus->vehicleInfo->currentRotation, 0.0f, 3.0f);
}

/* --------------------------------------------------------------------------------
   Function : ChooseCorrectTazOnHovercraftAnimations
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

void ChooseCorrectTazOnHovercraftAnimations(ACTORINSTANCE *actorInstance, PADCONTROLSTATUS *controller)
{
/*	TBActorInstance		*tbActorInstance;
	int					random;

	tbActorInstance = ReturnAttachedObjectInstance(actorInstance, actorInstance->actorStatus->vehicleInfo->handle);
		
	if ((actorInstance->actorStatus->timeWithoutWallCollisionLastFrame > 0.2f)&&(actorInstance->actorBehaviour->physics.coll))
	{
		//Taz
		if((!CheckIfPlayingAnimation(actorInstance, "trolleybounce1"))&&(!CheckIfPlayingAnimation(actorInstance, "trolleybounce2")))
		{
			random = bmRand()%2;
			switch(random)
			{
			case 0:
				PlayAnimationByName(actorInstance, "trolleybounce1", 1.0f, 0, 0, 0.1f, REACT);
				break;
			case 1:
				PlayAnimationByName(actorInstance, "trolleybounce2", 1.0f, 0, 0, 0.1f, REACT);
				break;
			}
		}
		//Trolley
		if((!CheckIfPlayingAnimation(tbActorInstance, "bounceonground"))&&(!CheckIfPlayingAnimation(tbActorInstance, "bounceoffwall")))
		{
			PlayAnimationByName(tbActorInstance, "bounceonground", 1.0f, 0, 0, 0.1f, REACT);
		}
	}
	if((controller->circleChannel->value)&&(actorInstance->actorStatus->timeWithoutWallCollisionLastFrame < 0.1f))
	{
		//Taz
		if(!CheckIfPlayingAnimation(actorInstance, "trolleyaccelerate"))
		{
			if(bmVectorLen(actorInstance->actorBehaviour->physics.velocity) > METERTOUNIT(10.0f))
			{
				PlayAnimationByName(actorInstance, "trolleyaccelerate", 2.0f, 0, 0, 0.1f, JUMP);
			}
			else
			{
				PlayAnimationByName(actorInstance, "trolleyaccelerate", 1.0f, 0, 0, 0.1f, JUMP);
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
	}*/
}