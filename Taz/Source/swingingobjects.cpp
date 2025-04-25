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
#include "collisioncache.h"
#include "physics.h"
#include "status.h"
#include "camera.h"
#include "scene.h"
#include "control.h"
#include "characters.h"
#include "maths.h"
#include "main.h"
#include "animation.h"
#include "events.h"
#include "sfx.h"
#include "swingingobjects.h"
#include "files.h"
#include "genericai.h"

/* --------------------------------------------------------------------------------
   Function : CreateSwingingObject
   Purpose : Initialises actors that act as swingers
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int CreateSwingingObject(ACTORINSTANCE *swinger, TBVector initialPosition, float length, int32 model, char *obe)
{
	TBVector		scale;
	char			tempName[256];
//	char			*temp;
	
	if(!CreateSwingingObjectActorInfo(swinger)) return FALSE;

	ENABLE_LINE_OR_CARTOON_RENDER_MODE(&swinger->actorInstance);

	//fill in attributes
	swinger->swingingObjectInfo->type = SPECIALTYPE_SWINGINGOBJECT;
	swinger->special = SPECIALTYPE_SWINGINGOBJECT;
	swinger->swingingObjectInfo->initialPosition[0] = initialPosition[0];
	swinger->swingingObjectInfo->initialPosition[1] = initialPosition[1];
	swinger->swingingObjectInfo->initialPosition[2] = initialPosition[2];
	swinger->swingingObjectInfo->initialPosition[3] = initialPosition[3];
	swinger->swingingObjectInfo->length = length;
	swinger->swingingObjectInfo->theta = 0.0f;
	swinger->swingingObjectInfo->phi = 0.0f;
	swinger->swingingObjectInfo->collision = FALSE;
	swinger->swingingObjectInfo->model = model;
	swinger->swingingObjectInfo->onContact = NULL;
	swinger->swingingObjectInfo->soundHandle = -1;

	if(model != -1)
	{
		switch(model)
		{
		case SWINGMODEL_MONKEY:
			LoadWholePackage("irrepresible",1);
			LoadActor("irrepresible.obe","irrepresible");
			swinger->swingingObjectInfo->modelPtr = CreateActorInstance(&map.animatedInstances, "irrepresible.obe", "monkey");
			bmVectorCopy(swinger->swingingObjectInfo->modelPtr->actorInstance.position,initialPosition);
			SETVECTOR(scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmVectorCopy(swinger->swingingObjectInfo->modelPtr->actorInstance.scale,scale);
			bmQuatCopy(swinger->swingingObjectInfo->modelPtr->actorInstance.orientation,bIdentityQuaternion);
			ENABLE_LINE_OR_CARTOON_RENDER_MODE(&swinger->swingingObjectInfo->modelPtr->actorInstance);
			break;
		case SWINGMODEL_CHANDELIER:
			swinger->swingingObjectInfo->modelPtr = CreateActorInstance(&map.animatedInstances, "chandelier.obe", "chandelier");
			bmVectorCopy(swinger->swingingObjectInfo->modelPtr->actorInstance.position,initialPosition);
			SETVECTOR(scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmVectorCopy(swinger->swingingObjectInfo->modelPtr->actorInstance.scale,scale);
			bmQuatCopy(swinger->swingingObjectInfo->modelPtr->actorInstance.orientation,bIdentityQuaternion);
			break;
		case SWINGMODEL_CHANDTRIGGER:
			swinger->swingingObjectInfo->modelPtr = CreateActorInstance(&map.animatedInstances, "chandtrigger.obe", "chandtrigger");
			bmVectorCopy(swinger->swingingObjectInfo->modelPtr->actorInstance.position,initialPosition);
			SETVECTOR(scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmVectorCopy(swinger->swingingObjectInfo->modelPtr->actorInstance.scale,scale);
			bmQuatCopy(swinger->swingingObjectInfo->modelPtr->actorInstance.orientation,bIdentityQuaternion);
			break;
		case SWINGMODEL_CRATEONROPE:
			swinger->swingingObjectInfo->modelPtr = CreateActorInstance(&map.animatedInstances, "crateonrope.obe", "crateonrope");
			bmVectorCopy(swinger->swingingObjectInfo->modelPtr->actorInstance.position,initialPosition);
			SETVECTOR(scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmVectorCopy(swinger->swingingObjectInfo->modelPtr->actorInstance.scale,scale);
			bmQuatCopy(swinger->swingingObjectInfo->modelPtr->actorInstance.orientation,bIdentityQuaternion);
			break;
		default:
			swinger->swingingObjectInfo->modelPtr = NULL;
			break;
		}
	}
	else
	{
		if(obe[0])
		{
			sprintf(tempName, "objects\\%s", obe);
			swinger->swingingObjectInfo->modelCRC = bkCRC32((uchar*)tempName, strlen(tempName), 0);
			swinger->swingingObjectInfo->modelPtr = NULL;
//			if(!(swinger->swingingObjectInfo->modelPtr = CreateActorInstance(&map.animatedInstances, obe, "monkey")))
//			{
//				if (!FindActorInActorList(obe))
//				{
//					strcpy(tempName,obe);
//
//					temp = FindChar(tempName,'.');
//					if (temp!=NULL) *temp = 0;
//
//					LoadWholePackage(tempName,1);
//					LoadActor(obe,tempName);
//				}
//				sprintf(tempName, "objects\\%s", obe);
//				swinger->swingingObjectInfo->modelPtr = CreateActorInstance(&map.animatedInstances, tempName, "monkey");
//			}
//			if(swinger->swingingObjectInfo->modelPtr)
//			{
//				bmVectorCopy(swinger->swingingObjectInfo->modelPtr->actorInstance.position,initialPosition);
//				SETVECTOR(scale, 1.0f, 1.0f, 1.0f, 1.0f);
//				bmVectorCopy(swinger->swingingObjectInfo->modelPtr->actorInstance.scale,scale);
//				bmQuatCopy(swinger->swingingObjectInfo->modelPtr->actorInstance.orientation,bIdentityQuaternion);
//				EnableActorLineRenderMode(&swinger->swingingObjectInfo->modelPtr->actorInstance);
//			}
		}
		else
		{
			swinger->swingingObjectInfo->modelPtr = NULL;
			swinger->swingingObjectInfo->modelCRC = 0;
		}
	}

	SetActorPosition(swinger, initialPosition);

	AddActorToCollisionCache(&collisionCache, swinger, CACHEFLAGS_LIVEUPDATE);
	SetCollisionCallback(swinger, RespondToSwingerCollision);
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : FindSwingingModelsFromMap
   Purpose : 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void FindSwingingModelsFromMap(ACTORINSTANCELIST *instanceList)
{
	ACTORINSTANCE	*ptr;
	TBActor			*actor;

	for(ptr = instanceList->head.next;ptr != &instanceList->head;ptr = ptr->next)
	{
		if(ptr->special == SPECIALTYPE_SWINGINGOBJECT)
		{
			if(ptr->swingingObjectInfo->model == -1)
			{
				if(ptr->swingingObjectInfo->modelCRC)
				{
					actor = FindActorInActorList(ptr->swingingObjectInfo->modelCRC);
					if(actor)
					{
						ptr->swingingObjectInfo->modelPtr = CreateActorInstanceFromActorPtr(&map.animatedInstances, actor, ptr->swingingObjectInfo->modelCRC, "monkey");
						if(ptr->swingingObjectInfo->modelPtr)
						{
							bmVectorCopy(ptr->swingingObjectInfo->modelPtr->actorInstance.position,ptr->swingingObjectInfo->initialPosition);
							SETVECTOR(ptr->swingingObjectInfo->modelPtr->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
							bmQuatCopy(ptr->swingingObjectInfo->modelPtr->actorInstance.orientation,bIdentityQuaternion);
							ENABLE_LINE_OR_CARTOON_RENDER_MODE(&ptr->swingingObjectInfo->modelPtr->actorInstance);
						}
					}
				}
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : CreateSwingingObjectActorInfo
   Purpose : Initialises actorInstances info structure
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int CreateSwingingObjectActorInfo(ACTORINSTANCE *swinger)
{
	swinger->swingingObjectInfo = (SWINGINGOBJECTINFO*)ZALLOC(sizeof(SWINGINGOBJECTINFO));
	if (swinger->swingingObjectInfo)
	{
		return TRUE;
	}
	else
	{
		bkPrintf("*** ERROR *** Could not create tilting object info structure\n");
		return FALSE;
	}
}

/* --------------------------------------------------------------------------------
   Function : RespondToSwingerCollision
   Purpose : collision callback
   Parameters : ACTORINSTANCE *collider, ACTORINSTANCE *swinger
   Returns : 
   Info : 
*/

int RespondToSwingerCollision(ACTORINSTANCE *collider, ACTORINSTANCE *swinger)
{
	TBVector	pos;
	TBVector	cameraOffset = {0.0f, 0.0f, 0.0f, 1.0f};

	if (collider->genericAI) 
	{
		AICollideWithSpecial(collider,swinger);
		return FALSE;
	}
	if (collider->flags&ACTORFLAG_DEBRISACTOR) return FALSE;

 	if(((collider->actorStatus->currentState == STATE_JUMP)&&(!collider->characterInfo->powerUpInfo.hiccup))||
		(collider->actorStatus->currentState == STATE_PROJECTILE)||
		(collider->actorStatus->currentState == STATE_PROJECTILESLIDE))
		//&&collider in appropriate position to swing
	{
		collider->characterInfo->swingingInfo.swingingOnObject = swinger;
		collider->actorBehaviour->physics.flags |= PHYSFLAG_NOGRAVITY;
		RequestStateChange(collider, STATE_SWINGING);
		FlushAnimationQueue(collider);
		PlayAnimationByName(collider, "mswingstatic", 1.0f, 1, 0, 0.1f, NONE);
		collider->actorInstance.position[0] = swinger->actorInstance.position[0];
		collider->actorInstance.position[1] = swinger->actorInstance.position[1]-swinger->swingingObjectInfo->length;
		collider->actorInstance.position[2] = swinger->actorInstance.position[2];
		swinger->swingingObjectInfo->collision = TRUE;
		collider->characterInfo->swingingInfo.firstFrame = TRUE;
		CalculateRotationAboutYAxis(collider);
		bmVectorCopy(pos, swinger->actorInstance.position);
		pos[1] -= swinger->swingingObjectInfo->length;
		pos[2] += METERTOUNIT(10.0f);

		//save camera state
		// TP: PROBLEM
		if (collider->playerNo != -1)
		{
			SaveCameraSettings(&collider->characterInfo->swingingInfo.preSwingCamera, gameStatus.player[collider->playerNo].camera);
			//set camera for swing state
			SetCameraLookAtFollowActorInstance(gameStatus.player[collider->playerNo].camera, swinger, &swinger->actorInstance, (TBActorNodeInstance *)NULL);
			cameraOffset[1] = -(swinger->swingingObjectInfo->length/2.0f);
			SetCameraLookAtActorInstanceOffset(gameStatus.player[collider->playerNo].camera, cameraOffset);
			SetFixedDistanceToLookAt(gameStatus.player[collider->playerNo].camera, (swinger->swingingObjectInfo->length*2.3f));
			gameStatus.player[collider->playerNo].camera->flags |= CAMERA_ROPECONTROL;
		}

		if(swinger->swingingObjectInfo->modelPtr)
		{
			//play correct sound
			switch(swinger->swingingObjectInfo->model)
			{
			case SWINGMODEL_MONKEY:
				//play correct monkey Animation
				if(!CheckIfPlayingAnimation(swinger->swingingObjectInfo->modelPtr, "swing"))
				{
					PlayAnimationByName(swinger->swingingObjectInfo->modelPtr, "swing", 1.0f, 1, 0, 0.1f, REACT);
				}
				swinger->swingingObjectInfo->soundHandle = PlaySample("loop\\obj_mus_13a.wav", 255, swinger->actorInstance.position);
				break;
			case SWINGMODEL_CHANDELIER:
				swinger->swingingObjectInfo->soundHandle = PlaySample("loop\\obj_mus_13a.wav", 255, swinger->actorInstance.position);
				break;
			case SWINGMODEL_CRATEONROPE:
				swinger->swingingObjectInfo->soundHandle = PlaySample("loop\\obj_mus_13a.wav", 255, swinger->actorInstance.position);
				break;
			}
		}
		if(swinger->swingingObjectInfo->onContact)
		{
			//send message
			SendTrigger(swinger->swingingObjectInfo->onContact);
		}
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : UpdateSwingingObject
   Purpose : updates swinging object
   Parameters : ACTORINSTANCE *swinger
   Returns : 
   Info : 
*/

void UpdateSwingingObject(ACTORINSTANCE *ptr)
{
	TBActorAnimSegment	*animationToPlay;

	if(ptr->swingingObjectInfo->collision == FALSE)
	{
		if(ptr->swingingObjectInfo->modelPtr)
		{
			ProcessSwing(ptr->swingingObjectInfo->modelPtr);
			if(!CheckIfPlayingAnimation(ptr->swingingObjectInfo->modelPtr, "idle1")&&
				!CheckIfPlayingAnimation(ptr->swingingObjectInfo->modelPtr, "idle2")&&
				!CheckIfPlayingAnimation(ptr->swingingObjectInfo->modelPtr, "idle3")&&
				!CheckIfPlayingAnimation(ptr->swingingObjectInfo->modelPtr, "idle4"))
			{
				animationToPlay = ChooseIdleAnimationForMonkey(ptr->swingingObjectInfo->modelPtr);
				if (animationToPlay)
				{
					PlayAnimationBySegmentPointer(ptr->swingingObjectInfo->modelPtr, animationToPlay, 1.0f, 0, 0, 0.1f, IDLE);
				}
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : ProcessSwing
   Purpose : calculates swing stuff
   Parameters : ACTORINSTANCE *swinger
   Returns : 
   Info : 
*/

void ProcessSwing(ACTORINSTANCE *swinger)
{
	TBVector			faceVector = {0.0f, -1.0f, 0.0f, 1.0f};
	TBVector			lookAtPoint;

	bmVectorCopy(lookAtPoint, swinger->actorInstance.position);
	lookAtPoint[3] = 1.0f;
	lookAtPoint[1] -= METERTOUNIT(30.0f);
	swinger->actorInstance.rootNodeInstance->flags &= ~BNODEINSTFLAG_ORIENTKEYS;
	baNodeLookAtUnconstrained(&swinger->actorInstance, swinger->actorInstance.rootNodeInstance, lookAtPoint, faceVector, 0.1f, 1.0f, BNODELOOKATUC_PREP|BNODELOOKATUC_PREPCHILDREN);
}

/* --------------------------------------------------------------------------------
   Function : SwingActor
   Purpose : calculates swing when taz is swinging
   Parameters : ACTORINSTANCE *tazInstance
   Returns : 
   Info : 
*/

void SwingActor(ACTORINSTANCE *actorInstance)
{
	TBVector			relativePosition;
	TBVector			xzPlanePosition;
	TBVector			velocity, nextPosition;
	ACTORINSTANCE		*swing;
	float				xzLength;
	float				speed, tempFloat;
	float				constant;					//springiness of swing
	float				damping = 0.975f;
	float				xTempSpeed, zTempSpeed;
	float				maxSpeed; //METERTOUNIT(3.0f);
	float				speedIncrementCutOff; // = METERTOUNIT(1.0f);

	maxSpeed = METERTOUNIT(bmSqrt(UNITTOMETER(actorInstance->characterInfo->swingingInfo.swingingOnObject->swingingObjectInfo->length)));
	//set "feel" of swinging object based on model type
	switch(actorInstance->characterInfo->swingingInfo.swingingOnObject->swingingObjectInfo->model)
	{
	case SWINGMODEL_MONKEY:
	case SWINGMODEL_CHANDTRIGGER:
		constant = METERTOUNIT(1.0f);
		speedIncrementCutOff = maxSpeed / 2.0f;
		break;
	case SWINGMODEL_CHANDELIER:
		constant = METERTOUNIT(0.3f);
		speedIncrementCutOff = maxSpeed / 7.0f;
		break;
	default:
		constant = METERTOUNIT(10.0f);
		speedIncrementCutOff = maxSpeed / 5.0f;
		break;
	}

	swing = actorInstance->characterInfo->swingingInfo.swingingOnObject;
	StopCollidingWith(actorInstance, swing, 1.0f);
	//calculate relative position vector
	bmVectorSub(relativePosition, actorInstance->actorInstance.position, swing->actorInstance.position);
	//calculate relative position on XZ plane
	bmVectorCopy(xzPlanePosition, relativePosition);
	xzPlanePosition[1] = 0.0f;

	if(!xzPlanePosition[0] && !xzPlanePosition[2])
	{
		xzLength = 0.0f;
		velocity[0] = 0.0f; velocity[1] = 0.0f; velocity[2] = 0.0f; velocity[3] = 1.0f;
	}
	else
	{
		//note: XZPlanePosition should point towards the centre of rotation
		if((xzLength = bmVectorLen(xzPlanePosition)) <= swing->swingingObjectInfo->length)
		{
			speed = -constant*(xzLength/swing->swingingObjectInfo->length);
		}
		else
		{
			//help, rope to long, haven't figured out what to do just yet
			xzLength = swing->swingingObjectInfo->length;
			bmVectorScaleToLength(xzPlanePosition, xzPlanePosition, xzLength);
			speed = -constant;
		}
		//apply speed along XZPlanePosition
		bmVectorScaleToLength(velocity, xzPlanePosition, speed);
	}
	//add to current velocity in XZ plane
	if(speed < maxSpeed)
	{
		//xTempSpeed = speedIncrementCutOff*bmCos(HALFPI*actorInstance->actorBehaviour->physics.velocity[0]/maxSpeed); 
		//zTempSpeed = speedIncrementCutOff*bmCos(HALFPI*actorInstance->actorBehaviour->physics.velocity[2]/maxSpeed);
		xTempSpeed = 0.1f + speedIncrementCutOff*bmSqrt(bmSin(PI*actorInstance->actorBehaviour->physics.velocity[0]/maxSpeed)*bmSin(PI*actorInstance->actorBehaviour->physics.velocity[0]/maxSpeed)); 
		zTempSpeed = 0.1f + speedIncrementCutOff*bmSqrt(bmSin(PI*actorInstance->actorBehaviour->physics.velocity[2]/maxSpeed)*bmSin(PI*actorInstance->actorBehaviour->physics.velocity[2]/maxSpeed));
	}
	else
	{
		xTempSpeed = 0.0f;
		zTempSpeed = 0.0f;
	}
	if(actorInstance->characterInfo->swingingInfo.firstFrame)
	{
		actorInstance->actorBehaviour->physics.velocity[0] = actorInstance->actorStatus->velBeforeJump[0];
		actorInstance->actorBehaviour->physics.velocity[2] = actorInstance->actorStatus->velBeforeJump[2];
		actorInstance->characterInfo->swingingInfo.firstFrame = FALSE;
	}
	if(bmVectorLen(actorInstance->actorBehaviour->physics.velocity) > METERTOUNIT(20.0f))
	{
		bmVectorScaleToLength(actorInstance->actorBehaviour->physics.velocity, actorInstance->actorBehaviour->physics.velocity, METERTOUNIT(20.0f));
	}
	velocity[0] += actorInstance->actorBehaviour->physics.velocity[0] + (actorInstance->characterInfo->swingingInfo.xySpeed * xTempSpeed);
	velocity[2] += actorInstance->actorBehaviour->physics.velocity[2] + (actorInstance->characterInfo->swingingInfo.zySpeed * zTempSpeed);

	//calculate new position for next frame
	//xzPlanePosition[0] += velocity[0]/fps;
	//xzPlanePosition[2] += velocity[2]/fps;
	if((tempFloat = (swing->swingingObjectInfo->length*swing->swingingObjectInfo->length)-(xzLength*xzLength)) < 0.0f)
	{
		//this should not be happening
		bkPrintf("*** ERROR *** swing buggered by -ve square root\n");
		//xzPlanePosition[1] = 0.0f;
		//bmVectorNorm(xzPlanePosition, xzPlanePosition);
		//bmVectorScaleToLength(xzPlanePosition, xzPlanePosition, swing->swingingObjectInfo->length);
		//bmVectorCopy(relativePosition, xzPlanePosition);
	}
	else
	{
		xzPlanePosition[1] = -bmSqrt(tempFloat);
		//calculate new position for next frame
		xzPlanePosition[0] += velocity[0]/fps;
		xzPlanePosition[2] += velocity[2]/fps;
	}

	//find position increment from last frame
	//remember, XZPlanePosition and relative position are in swing local coordinates

	bmVectorSub(velocity, xzPlanePosition, relativePosition);
	bmVectorCMul(velocity, velocity, damping);
	bmVectorCMul(actorInstance->actorBehaviour->physics.velocity, velocity, fps);

	bmVectorAdd(nextPosition, actorInstance->actorInstance.position, velocity);

	if(swing->swingingObjectInfo->modelPtr)
	{
		CalculateOrientationOfRope(/*actorInstance->actorInstance.position*/nextPosition, swing->swingingObjectInfo->modelPtr, actorInstance);
	}
}

/* --------------------------------------------------------------------------------
   Function : CalculateOrientationOfRope
   Purpose : name says it all
   Parameters : ACTORINSTANCE *actor, ACTORINSTANCE *rope
   Returns : 
   Info : 
*/

void CalculateOrientationOfRope(TBVector actorPosition, ACTORINSTANCE *rope, ACTORINSTANCE *actor)
{
	TBVector			tempQuat;
	TBVector			faceVector = {0.0f, -1.0f, 0.0f, 1.0f};
	
	actorPosition[3] = 1.0f;
	rope->actorInstance.rootNodeInstance->flags &= ~BNODEINSTFLAG_ORIENTKEYS;
	baNodeLookAtUnconstrained(&rope->actorInstance, rope->actorInstance.rootNodeInstance, actorPosition, faceVector, 1.0f, 1.0f, BNODELOOKATUC_PREP|BNODELOOKATUC_PREPCHILDREN);

//	bmQuatMultiply(actor->actorBehaviour->rotAim, actor->actorStatus->swingingInfo.previousOrientation, rope->actorInstance.rootNodeInstance->animState.orientation);
	bmQuatMultiply(tempQuat, actor->characterInfo->swingingInfo.previousOrientation, rope->actorInstance.rootNodeInstance->animState.orientation);
	SetActorOrientation(actor, tempQuat);
	baPrepActorInstance(&rope->actorInstance, TRUE);
}

/* --------------------------------------------------------------------------------
   Function : CalculateRotationAboutYAxis
   Purpose : calculates actors rotation about y axis, before collision with rope
   Parameters : ACTORINSTANCE *actor
   Returns : 
   Info : 
*/

void CalculateRotationAboutYAxis(ACTORINSTANCE *actor)
{
	TBVector			totalRotation;
	float				tempFloat;

	ResetCharacterAfterJump(actor);
	bmQuatToRotation(totalRotation, actor->actorInstance.orientation);
	tempFloat = totalRotation[3];
	bmVectorNorm(totalRotation, totalRotation);
	tempFloat *= totalRotation[1];
	totalRotation[0] = 0.0f;
	totalRotation[1] = 1.0f;
	totalRotation[2] = 0.0f;
	totalRotation[3] = -tempFloat;
	bmRotationToQuat(actor->characterInfo->swingingInfo.previousOrientation, totalRotation);
	//bmQuatCopy(actor->actorStatus->swingingInfo.previousOrientation, actor->actorInstance.orientation);
}

/* --------------------------------------------------------------------------------
   Function : ChooseIdleAnimationForMonkey
   Purpose : see above
   Parameters : ACTORINSTANCE *actorInstance
   Returns : TBActorAnimSegment *animSegment
   Info : 
*/

TBActorAnimSegment *ChooseIdleAnimationForMonkey(ACTORINSTANCE *monkey)
{
	int						animNumber;

	if (monkey->actorInstance.numAnimSegments == 0) return NULL;

	animNumber = bmRand()%3;

	if (bmRand()%2)
	{
		switch(animNumber)
		{
		case 0:		PlaySample("monkey1.wav", 255, monkey->actorInstance.position);	break;
		case 1:		PlaySample("monkey2.wav", 255, monkey->actorInstance.position);	break;
		case 2:		PlaySample("monkey3.wav", 255, monkey->actorInstance.position);	break;
		}
	}

	animNumber = bmRand()%monkey->actorInstance.numAnimSegments;	
	return &monkey->actorInstance.animSegments[animNumber];

	switch(animNumber)
	{
	case 0:
		return baFindAnimSegment(&monkey->actorInstance, "idle1");
	case 1:
		return baFindAnimSegment(&monkey->actorInstance, "idle2");
	case 2:
		return baFindAnimSegment(&monkey->actorInstance, "idle3");
	case 3:
		return baFindAnimSegment(&monkey->actorInstance, "idle4");
	}

	//just in case bmRand()%4 returns something stupid
	return NULL;
}