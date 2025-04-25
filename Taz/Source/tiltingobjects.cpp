// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : tiltingobjects.cpp
//   Purpose : functions to calculate the tilt of objects when collided with
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "main.h"

#include "actors.h"
#include "sfx.h"
#include "scene.h"
#include "steppingstone.h"
#include "tiltingobjects.h"
#include "status.h"
#include "prompt.h"
#include "genericai.h"

/* --------------------------------------------------------------------------------
   Function : CreateTiltingObject
   Purpose : Initialises actorInstances that act as tilting objects
   Parameters : ACTORINSTANCE *, max tilt theta, max tile phi
   Returns : 
   Info : 
*/

void CreateTiltingObject(ACTORINSTANCE *actorInstance, float maxTheta, float maxPhi, float maxRadius, TBVector initPos, float maxAcceleration, float retardationForce)
{
	//CreateActorInstanceBehaviour( actorInstance );
	CreateSteppingStoneActorInstanceInfo( actorInstance );

	//EnableActorLineRenderMode( &actorInstance->actorInstance );

	//fill in attributes
	actorInstance->tiltingObjectInfo->type = SPECIALTYPE_TILTINGOBJECT;
	actorInstance->special = SPECIALTYPE_TILTINGOBJECT;
	bmVectorCopy(actorInstance->tiltingObjectInfo->initialPosition, initPos);
	bmQuatCopy(actorInstance->tiltingObjectInfo->initialOrientation, actorInstance->actorInstance.orientation);
	actorInstance->tiltingObjectInfo->maxTheta = maxTheta;
	actorInstance->tiltingObjectInfo->maxPhi = maxPhi;
	actorInstance->tiltingObjectInfo->soundHandle = -1;
	if((actorInstance->tiltingObjectInfo->maxRadius = maxRadius) == 0.0f)
	{
		bkPrintf("      *** WARNING *** tilting object %s has zero max radius\n", actorInstance->actorName);
	}
	actorInstance->tiltingObjectInfo->theta = 0.0f;
	actorInstance->tiltingObjectInfo->phi = 0.0f;
	actorInstance->tiltingObjectInfo->maxAcceleration = maxAcceleration;
	actorInstance->tiltingObjectInfo->retardation = retardationForce;
#ifndef INFOGRAMES
	if((maxAcceleration - retardationForce) <= 0.0f)
	{
		bkPrintf("      *** WARNING *** tilting object %s; acceleration should be greater than springyness\n", actorInstance->actorName);
	}
#endif
	actorInstance->tiltingObjectInfo->timeSinceLastCollision = 5.0f;
	actorInstance->tiltingObjectInfo->collider = NULL;
	
	SetActorPosition( actorInstance, initPos );

	AddActorToCollisionCache(&collisionCache,actorInstance, CACHEFLAGS_LIVEUPDATE);
	SetCollisionCallback(actorInstance,RespondToTiltingObjectCollision);
}

/* --------------------------------------------------------------------------------
   Function : CreateTiltingObjectActorInfo
   Purpose : creates actorInfo structure
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int CreateTiltingObjectActorInfo(ACTORINSTANCE *actorInstance)
{
	actorInstance->tiltingObjectInfo = (TILTINGOBJECTINFO*)ZALLOC(sizeof(TILTINGOBJECTINFO));
	if (actorInstance->tiltingObjectInfo)
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
   Function : RespondToTiltingObjectActorInfo
   Purpose : collision callback
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int RespondToTiltingObjectCollision(ACTORINSTANCE *collider, ACTORINSTANCE *tilter)
{
	if (tilter->tiltingObjectInfo->soundHandle == -1)
	{
		if (strstr(tilter->instanceName, "chand"))
		{
			tilter->tiltingObjectInfo->soundHandle = PlayTrackingSample("loop\\obj_mus_13a.wav", 255, tilter->actorInstance.position, 0, 0, METERTOUNIT(2.5f), METERTOUNIT(10.0f));
			
			PlayTrackingSample("chandelier.wav", 255, tilter->actorInstance.position, 0, 0, METERTOUNIT(2.5f), METERTOUNIT(10.0f));
		}
	}

	if (collider->genericAI) 
	{
		AICollideWithSpecial(collider,tilter);
		return FALSE;
	}
	if (collider->flags&ACTORFLAG_DEBRISACTOR) return FALSE;
	if(collider->actorStatus)
	{
		if(collider->actorStatus->currentState == STATE_TIPTOE) return FALSE;
	}

	RequestMorronicMessage(MEVENT_TIPTOEHINT);
	tilter->tiltingObjectInfo->collider = collider;
	tilter->tiltingObjectInfo->timeSinceLastCollision = 0.0f;

	if (collider->actorStatus) collider->actorStatus->timeSinceLastTiltingCollision = 0.0f;

	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : UpdateTiltingObject
   Purpose : updates tilting objects
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void UpdateTiltingObject(ACTORINSTANCE *ptr)
{
	if (ptr->tiltingObjectInfo->soundHandle != -1)
	{
		if (gameStatus.player1->actorStatus->timeSinceLastTiltingCollision > 0.15)
		{
			StopChannel(&ptr->tiltingObjectInfo->soundHandle);
		}
	}

	if(ptr->steppingStoneObjectInfo)
	{
		ProcessTilting(ptr);
	}
}

/* --------------------------------------------------------------------------------
   Function : ProcessTilting
   Purpose : prosecces the tilt of object dependant on possition of collider
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void ProcessTilting(ACTORINSTANCE *tilter)
{
	TBVector			relativePosition;
	TBVector			XZPlanePosition;
	TBVector			xAxis = {1.0f, 0.0f, 0.0f, 0.0f};
	TBVector			zAxis= {0.0f, 0.0f, 1.0f, 0.0f};
	TBQuaternion		totalRotQuat, xAxisQuat, zAxisQuat;
	float				xAcceleration, zAcceleration;				//in radians per second per second
	float				xRetardation, zRetardation;

	//variables from map:	maxRadius, maxAcceleration, retardationForce, maxTheta, maxPhi
	//increment collision counter
	if((tilter->tiltingObjectInfo->timeSinceLastCollision += 1/fps) <= 0.3f)
	{
		//calculate acceleration due to taz
		if (tilter->tiltingObjectInfo->maxRadius!=0.0f)
		{
			bmVectorSub(relativePosition, tilter->actorInstance.position, tilter->tiltingObjectInfo->collider->actorInstance.position);
			bmVectorCopy(XZPlanePosition, relativePosition);
			XZPlanePosition[1] = 0.0f;

			xAcceleration = -XZPlanePosition[0]*tilter->tiltingObjectInfo->maxAcceleration/tilter->tiltingObjectInfo->maxRadius;
			zAcceleration = XZPlanePosition[2]*tilter->tiltingObjectInfo->maxAcceleration/tilter->tiltingObjectInfo->maxRadius;
		}
		else
		{
			xAcceleration = 0.0f;
			zAcceleration = 0.0f;
		}
	}
	else
	{
		xAcceleration = 0.0f;
		zAcceleration = 0.0f;
	}
	//calculate acceleration due to retardation force
	xRetardation = -tilter->tiltingObjectInfo->retardation*bmSin(tilter->tiltingObjectInfo->theta);
	zRetardation = -tilter->tiltingObjectInfo->retardation*bmSin(tilter->tiltingObjectInfo->phi);
	
	//total acceleration
	xAcceleration += xRetardation;
	zAcceleration += zRetardation;

	//calculate new angles
	if((tilter->tiltingObjectInfo->theta += xAcceleration/fps) > tilter->tiltingObjectInfo->maxTheta)
	{
		tilter->tiltingObjectInfo->theta = tilter->tiltingObjectInfo->maxTheta;
	}
	if(tilter->tiltingObjectInfo->theta < -tilter->tiltingObjectInfo->maxTheta)
	{
		tilter->tiltingObjectInfo->theta = -tilter->tiltingObjectInfo->maxTheta;
	}
	if((tilter->tiltingObjectInfo->phi += zAcceleration/fps) > tilter->tiltingObjectInfo->maxPhi)
	{
		tilter->tiltingObjectInfo->phi = tilter->tiltingObjectInfo->maxPhi;
	}
	if(tilter->tiltingObjectInfo->phi < -tilter->tiltingObjectInfo->maxPhi)
	{
		tilter->tiltingObjectInfo->phi = -tilter->tiltingObjectInfo->maxPhi;
	}
	zAxis[3] = tilter->tiltingObjectInfo->theta;
	xAxis[3] = tilter->tiltingObjectInfo->phi;
	bmRotationToQuat(zAxisQuat, zAxis);
	bmRotationToQuat(xAxisQuat, xAxis);
	bmQuatMultiply(totalRotQuat, xAxisQuat, zAxisQuat);

	bmQuatMultiply(tilter->actorInstance.orientation, tilter->tiltingObjectInfo->initialOrientation, totalRotQuat);
}
