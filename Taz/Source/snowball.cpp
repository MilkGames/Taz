// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : snowball.cpp
//   Purpose : provides functions to create and throw snowballs
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "actors.h"
#include "maths.h"
#include "debris.h"
#include "projectile.h"
#include "snowball.h"
#include "status.h"
#include "collisioncache.h"
#include "scene.h"


/* --------------------------------------------------------------------------------
   Function : CreateSnowball
   Purpose : Creates a snowball, which takes the form of an extra actor
   Parameters : 
   Returns : 
   Info : 
*/

DEBRIS *CreateSnowball(ACTORINSTANCE *actorInstance,TBVector initialPosition, TBVector direction2, float	size, TBVector target, uint32 strength, char *actorName, float angOff, float scale)
{
	float			speed;
	TBVector		velocity, tempVelocity,tempVector;
	TBVector		direction;
	float			angle;
	DEBRIS			*debris;

	bmVectorCopy(direction,direction2);

	if(target == NULL)
	{
		//always throw snowball at angle of 30 degrees (reasonable?)
		if(strength == SNOWBALL_WEAK) speed = WEAK_SNOWBALL_SPEED;
		else
			if(strength == SNOWBALL_MEDIUM) speed = MEDIUM_SNOWBALL_SPEED;
			else
				if(strength == SNOWBALL_STRONG) speed = STRONG_SNOWBALL_SPEED;

		velocity[1] = speed*bmSin(RAD(30.0f));
		//direction should not have any vertical componant
		direction[1] = 0.0f;
		bmVectorScaleToLength(tempVelocity, direction, bmSqrt((speed*speed)-(velocity[1]*velocity[1])));
		velocity[0] = tempVelocity[0];
		velocity[2] = tempVelocity[2];
		velocity[3] = 1.0f;
	}
	else
	{
		bmVectorSub(tempVector,target,initialPosition);
		angle = tempVector[Y];
		tempVector[Y] = 0.0f;
		angle = bmATan2(angle,bmVectorLen(tempVector));
		CalculateProjectileInitialVelocity(velocity, initialPosition, target, angle + angOff);
	}
//	bmVectorCMul(velocity, velocity, 60.0f);
	bmVectorSet(tempVector,RAD(360.0f),RAD(360.0f),RAD(360.0f),1.0f);
	debris = CreateDebrisActor(actorName, initialPosition, velocity, tempVector, 1.0f, scale, size, size, 127, DEBRISFLAG_PHYSICS,0,0,0.0f);
	if (!debris) return NULL;
	else
	{
		TBVector	offset;
		bmVectorSet(offset,0.0f,0.0f,0.0f,1.0f);
		SetActorCylinder(0,debris->actorStructure->actorBehaviour,offset,size,size*2);
		AddActorToCollisionCylinderCache(&collisionCache,debris->actorStructure);

		debris->flags |= DEBRISFLAG_POINTINDIRECTION;
	}
	CreateActorInstanceStatus(debris->actorStructure);
	StopCollidingWith(debris->actorStructure,actorInstance);
	EnableActorCartoonRenderMode(debris->actorInstance);
	debris->actorStructure->actorCRC = bkCRC32((uchar*)actorName,12,0);
	debris->actorStructure->flags|=ACTORFLAG_DEBRISACTOR;
	return debris;
}
