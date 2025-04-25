// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : projectile.cpp
//   Purpose : functions to calculate projectile path of object
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "actors.h"
#include "physics.h"
#include "trigger.h"
#include "scene.h"
#include "status.h"
#include "animation.h"
#include "maths.h"
#include "main.h"
#include "projectile.h"
#include "mapfile.h"
#include "sfx.h"
#include "genericai.h"
#include "Fireball.h"			// PP: Particle emitters used for explosion effects, eg. by WestBossExp
#include "control.h"
#include "characters.h"


// PP: ~~~~~~~~~~~~~~~~~~~~~~

// PP: names used to specify cannon effects (in Max)
const char* const				cannonEffectNames[NUM_CANNON_EFFECTS]=		{
																				"cannon",
																				"cementmixer",
																			};

// PP: array of cannon effect functions
const CannonEffectFunction		cannonEffectFunctions[NUM_CANNON_EFFECTS]=	{
																				Fireball_cannonExplosion,
																				Fireball_cementMixerExplosion,
																			};

// PP: ~~~~~~~~~~~~~~~~~~~~~~


/* --------------------------------------------------------------------------------
   Function : CreateActorInstancePathInfo
   Purpose : Creates and initialises the path info part of an actorInstance
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/

int CreateActorInstanceProjectile(ACTORINSTANCE *actorInstance, uint32 landPointCRC, float angle, ESpecialType type, int projectOnContact,bool collision, char liveUpdate, const int cannonEffect,int flags)
{
	actorInstance->projectileInfo = (PROJECTILEINFO*)ZALLOC(sizeof(PROJECTILEINFO));
	if (actorInstance->projectileInfo)
	{
		actorInstance->projectileInfo->type = type;
		actorInstance->special = type;
		if (landPointCRC==0)
		{
			bkPrintf("      *** ERROR *** No landing point found.\n");
		}
		actorInstance->projectileInfo->landPointCRC = landPointCRC;
		if (angle==0)
		{
			bkPrintf("      *** ERROR *** No angle specified.\n");
		}
		actorInstance->projectileInfo->angle = angle;
		actorInstance->projectileInfo->collision = FALSE;
		actorInstance->projectileInfo->fireFlag = FALSE;
		actorInstance->projectileInfo->collider = NULL;
		actorInstance->projectileInfo->projectOnContact = projectOnContact;
		actorInstance->projectileInfo->cannonEffect = cannonEffect;				// PP: set cannon effect type
		actorInstance->projectileInfo->soundHandle = -1;
		actorInstance->projectileInfo->flags = flags;
		if (collision)
		{
			if(liveUpdate)
			{
				AddActorToCollisionCache(&collisionCache, actorInstance, CACHEFLAGS_LIVEUPDATE);
			}
			else
			{
				AddActorToCollisionCache(&collisionCache, actorInstance, 0); 
			}
		}
		SetCollisionCallback(actorInstance,RespondToCannonCollision);
		FlushAnimationQueue(actorInstance);
		PlayAnimationByName(actorInstance, "ready", 1.0f, TRUE, 0, 0.1f, NONE);

		return TRUE;
	}
	else
	{
		bkPrintf("      *** ERROR *** Could not create projectile info structure\n");
		return FALSE;
	}
}

/* --------------------------------------------------------------------------------
   Function : RespondToCannonCollision
   Purpose : Function to be called when a collision with a cannon has occured
   Parameters : ACTORINSTANCE object hitting cannon, ACTORINSTANCE *cannon
   Returns : 
   Info : 
*/
int RespondToCannonCollision(ACTORINSTANCE *collider, ACTORINSTANCE *cannon)
{
	TBVector		pos;
	char			message[256];

	if (collider->genericAI) 
	{
		AICollideWithSpecial(collider,cannon);
		return FALSE;
	}

	if (collider->playerNo==-1) return FALSE;

	if((collider->actorStatus->currentState == STATE_JUMP)
		|| (collider->actorStatus->currentState == STATE_SPRUNG)
		|| (collider->actorStatus->currentState == STATE_PROJECTILE)
		|| (collider->actorStatus->currentState  == STATE_PROJECTILESLIDE)
		|| (cannon->projectileInfo->projectOnContact))
	{
		if(cannon->projectileInfo->flags & CANNONFLAG_INACTIVE) return FALSE;

		if(cannon->projectileInfo->collision == FALSE)
		{
			if (!RequestStateChange(collider, STATE_DONOTHING)) return FALSE;
			// TP: If can't change state then exit
			if (!UpdateCharacterState(&gameStatus.player[collider->playerNo], true)) 
			{
				ClearStateChangeRequest(collider);
				return FALSE;
			}

			RemoveInstanceFromInstanceList(collider, collider->instanceList);
			collider->flags |= ACTORFLAG_NOCOLLISION;
			// TP: PROBLEM
			if (collider->playerNo != -1)
			{
				SetCameraLookAtFollowActorInstance(gameStatus.player[collider->playerNo].camera, cannon, &cannon->actorInstance, (TBActorNodeInstance *)NULL);
			}
			FlushAnimationQueue(cannon);
			PlayAnimationByName(cannon, "aim", 2.0f, 0, 0 , 0.1f, NONE);

			if (!(cannon->projectileInfo->flags&CANNONFLAG_NOSNAPTOORIGIN))
			{
				bmVectorCopy(pos,cannon->actorInstance.position);
			}

			cannon->projectileInfo->soundHandle = PlaySample("lightfuse.wav",255,cannon->actorInstance.position);

			// send messages
			if (cannon->projectileInfo->onContact)
				SendTrigger(cannon->projectileInfo->onContact, pos);
		}
		cannon->projectileInfo->collision = TRUE;
		cannon->projectileInfo->collider = collider;
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : UpdateCannonState
   Purpose : Updates cannons on an instance list
   Parameters : ACTORINSTANCELIST *
   Returns : 
   Info :
*/

void UpdateCannonState(ACTORINSTANCE *ptr)
{
	TBVector		cameraOffset = {0.0f, METERTOUNIT(1.0f), 0.0f, 1.0f};
	TBVector		pos;
	char			message[256];

	if(ptr->projectileInfo)
	{
		if((ptr->projectileInfo->collision)&& (!CheckIfPlayingAnimation(ptr, "aim")))
		{
			if(ptr->projectileInfo->fireFlag == FALSE)
			{
				FlushAnimationQueue(ptr);
				PlayAnimationByName(ptr, "fire", 2.0f, 0, 0, 0.1f, NONE);
				bmVectorCopy(pos,ptr->actorInstance.position);

				// send messages
				if (ptr->projectileInfo->onFire)
					SendTrigger(ptr->projectileInfo->onFire, pos);

				ptr->projectileInfo->fireFlag = TRUE;
				return;
			}
			if(!CheckIfPlayingAnimation(ptr, "fire"))
			{
				// TP: PROBLEM
				if (ptr->projectileInfo->collider->playerNo != -1)
				{
					SetCameraLookAtFollowActorInstance(gameStatus.player[ptr->projectileInfo->collider->playerNo].camera, ptr->projectileInfo->collider, &ptr->projectileInfo->collider->actorInstance, (TBActorNodeInstance *)NULL);
					SetCameraLookAtActorInstanceOffset(gameStatus.player[ptr->projectileInfo->collider->playerNo].camera, cameraOffset);
				}
				ptr->projectileInfo->collider->flags &= ~ACTORFLAG_NOCOLLISION;
				AddInstanceToInstanceList(ptr->projectileInfo->collider, ptr->projectileInfo->collider->instanceList);
				PlayAnimationByName(ptr, "recoil", 1.0f, 0, 0, 0.1f, NONE);
				PlayAnimationByName(ptr, "ready", 1.0f, 1, 1, 0.1f, NONE);
				bmVectorCopy(pos,ptr->actorInstance.position);

				StopChannel(&ptr->projectileInfo->soundHandle);

				PlaySample("cannonfire.wav",255,ptr->actorInstance.position);

				if(ptr->projectileInfo->cannonEffect != CANNONEFFECT_NONE)// PP: if the cannon has an explosion effect to trigger...
				{
					// PP: ...trigger the appropriate pre-defined explosion effect for this cannon type
					cannonEffectFunctions[ptr->projectileInfo->cannonEffect](ptr);
				}

				// send messages
				if (ptr->projectileInfo->onRecoil)
					SendTrigger(ptr->projectileInfo->onRecoil, pos);

				ptr->projectileInfo->collision = FALSE;
				ptr->projectileInfo->fireFlag = FALSE;
				LaunchActor(ptr->projectileInfo->collider, ptr);
			}
		}
	}
}

void	LaunchStateChangeCallback(ACTORINSTANCE *actorInstance)
{
	FlushAnimationQueue(actorInstance);
	if((bmRand()%4) == 0)
	{
		PlayAnimationByName(actorInstance, "fly2", 1.0f, TRUE, 0, 0.1f, NONE);
	}
	else
	{
		PlayAnimationByName(actorInstance, "fly1", 1.0f, TRUE, 0, 0.1f, NONE);
	}
	actorInstance->actorAnimation->useMotionBone = FALSE;
}

/* --------------------------------------------------------------------------------
   Function : LaunchActor
   Purpose : entry point for proj motion calculations. launches actor from actor at position A, 
				to another actor at position B.
   Parameters : Actor Instance x2;
   Returns : 
   Info :
*/

void LaunchActor(ACTORINSTANCE *projectile, ACTORINSTANCE *cannon)
{
	float				theta;
	TBVector			initialVelocity;		//[3]=total,[0]=xcomponent,[1]=ycomponent,[2]=zcomponent
	ACTORINSTANCE		*landingPad;
	float				retTime;


	//get info from structures
	if (!cannon->projectileInfo) return;

	theta = cannon->projectileInfo->angle;

	if((landingPad = FindActorInstanceInMap( 0, cannon->projectileInfo->landPointCRC, &map)) == NULL )
	{
		bkPrintf("****ERROR**** Projector cannot find landingPad instance.\n");
		// TP: Should this really change state here?
//		RequestStateChange(projectile, STATE_PROJECTILE);
//		projectile->actorStatus->currentState = STATE_PROJECTILE;
		return;
	}

	landingPad->actorInstance.position[W] = 1.0f;

	if (cannon->projectileInfo->flags&CANNONFLAG_NOSNAPTOORIGIN)
	{
		retTime = CalculateProjectileInitialVelocity(initialVelocity, projectile->actorInstance.position, landingPad->actorInstance.position, theta);
	}
	else
	{
		if (cannon->projectileInfo->flags&CANNONFLAG_NOCENTERORIGIN)
		{
			TBVector	tempVector;
			bmVectorSub(tempVector, landingPad->actorInstance.position, projectile->actorBehaviour->collSphere.offset);
			retTime = CalculateProjectileInitialVelocity(initialVelocity, cannon->actorInstance.position, tempVector, theta);
		}
		else
		{
			retTime = CalculateProjectileInitialVelocity(initialVelocity, cannon->actorInstance.position, landingPad->actorInstance.position, theta);
		}
	}
	
	//change projectile actor instance status and turn off motion bone
	if(cannon->special == SPECIALTYPE_PROJECTILE)
	{
		RequestStateChangePlus(projectile,STATE_PROJECTILE,LaunchStateChangeCallback);
	}
	else
	{
		if(cannon->special == SPECIALTYPE_PROJECTILESLIDE)
		{
			RequestStateChangePlus(projectile,STATE_PROJECTILESLIDE,LaunchStateChangeCallback);
		}
	}

	//centre projectile on cannon object
	if (!(cannon->projectileInfo->flags&CANNONFLAG_NOSNAPTOORIGIN))
	{
		if (cannon->projectileInfo->flags&CANNONFLAG_NOCENTERORIGIN)
		{
			bmVectorCopy(projectile->actorInstance.position, cannon->actorInstance.position);
		}
		else
		{
			bmVectorSub(projectile->actorInstance.position,cannon->actorInstance.position,projectile->actorBehaviour->collSphere.offset);
		}
	}
	projectile->actorInstance.position[3] = 1.0f;

	projectile->actorBehaviour->physics.coll = 0;

	if (cannon->projectileInfo->flags&CANNONFLAG_FIXEDPATH)
	{
		// TP: setup the fixed curve
		projectile->characterInfo->projecting.fixedPathEnable = true;

		// TP: setup start and end of path
		if (cannon->projectileInfo->flags&CANNONFLAG_NOSNAPTOORIGIN)
		{
			bmVectorCopy(projectile->characterInfo->projecting.points[0], projectile->actorInstance.position);
		}
		else
		{
			bmVectorCopy(projectile->characterInfo->projecting.points[0], cannon->actorInstance.position);
		}
		bmVectorCopy(projectile->characterInfo->projecting.points[3], landingPad->actorInstance.position);
		if ((~cannon->projectileInfo->flags&CANNONFLAG_NOSNAPTOORIGIN)&&(cannon->projectileInfo->flags&CANNONFLAG_NOCENTERORIGIN))
		{
			TBVector	tempVector;
			bmVectorSub(tempVector, landingPad->actorInstance.position, projectile->actorBehaviour->collSphere.offset);
			bmVectorCopy(projectile->characterInfo->projecting.points[3], tempVector);
		}

		// TP: find intermediate points
		bmVectorLerp(projectile->characterInfo->projecting.points[1], projectile->characterInfo->projecting.points[0], projectile->characterInfo->projecting.points[3], 0.333f);
		bmVectorLerp(projectile->characterInfo->projecting.points[2], projectile->characterInfo->projecting.points[0], projectile->characterInfo->projecting.points[3], 0.666f);

		projectile->characterInfo->projecting.points[1][W] = 1.0f;
		projectile->characterInfo->projecting.points[2][W] = 1.0f;

		TBVector posInc, vel;

		bmVectorCopy(vel, initialVelocity);
		ApplyGravity(posInc, vel, worldGravity, retTime*0.333f);
		projectile->characterInfo->projecting.points[1][Y] = projectile->characterInfo->projecting.points[0][Y] + posInc[Y];

		bmVectorCopy(vel, initialVelocity);
		ApplyGravity(posInc, vel, worldGravity, retTime*0.666f);
		projectile->characterInfo->projecting.points[2][Y] = projectile->characterInfo->projecting.points[0][Y] + posInc[Y];

		projectile->characterInfo->projecting.t = 0.0f;
		projectile->characterInfo->projecting.time = retTime;
	}
	else
	{
		SetActorVelocity(projectile->actorBehaviour, initialVelocity);
	}

	//stop collision
	if (cannon->projectileInfo->flags&CANNONFLAG_NOSNAPTOORIGIN)
	{
		// TP: stop colliding for half a second, this was needed for the wrecking balls on the construction site
		StopCollidingWith(projectile, cannon, 0.5f);
	}
	else
	{
		StopCollidingWith(projectile, cannon);
	}

	if(projectile->playerNo != -1)
	{
		RequestRumbleEffect(gameStatus.player[projectile->playerNo].controller, 200, 0.0, 0.5f, 1.0f, 0.0f, WAVEFORM_NORMAL);
	}
}

/* --------------------------------------------------------------------------------
   Function : CalculateProjectileInitialVelocity
   Purpose : calculates the initial velocity required given two points, and an angle
   Parameters :
   Returns : 
   Info :
*/

float CalculateProjectileInitialVelocity(TBVector initialVelocity, TBVector currentPos, TBVector landingPos, float theta)
{
	float				relativeXZDistance;
	float				velocityXZLength;
	TBVector			relativeDisplacement;

	//first must calculate displacement between cannon and landingPad
	bmVectorSub( relativeDisplacement, landingPos, currentPos );
	
	//now need distance in XZ plane
	relativeXZDistance = bmSqrt((relativeDisplacement[0]*relativeDisplacement[0]) + (relativeDisplacement[2]*relativeDisplacement[2]) );

	//check to see if angle is allowed
	if((theta <= bmATan2(relativeDisplacement[1], relativeXZDistance)) || theta == RAD(90.0f))
	{
		bkPrintf("****ERROR**** Projector projection value is impossible and/or silly.\n");
		return 0;
	}
	//now can calculate required initial velocity for given angle of elevation
	if(relativeDisplacement[1])
	{
		initialVelocity[3] = bmSqrt((-worldGravity)*(relativeXZDistance*relativeXZDistance)/
								(2.0f*((relativeXZDistance*bmTan(theta))-relativeDisplacement[1])*(bmCos(theta)*bmCos(theta))));
	}
	else
	{
		if( !relativeDisplacement[1] )
		{
			initialVelocity[3] = bmSqrt((relativeXZDistance*(-worldGravity))/bmSin(2.0f*theta));
		}
	}

	//calculate y componant of velocity
	initialVelocity[1] = initialVelocity[3]*bmSin(theta);

	//calculate remaining component of velocity
	velocityXZLength = initialVelocity[3]*bmCos(theta);

	//calculate x and z components
	initialVelocity[0] = relativeDisplacement[0]*velocityXZLength/relativeXZDistance;
	initialVelocity[2] = relativeDisplacement[2]*velocityXZLength/relativeXZDistance;

	//reset w component of initial vel vector to 1, so that we can use it to set actor vel
	initialVelocity[3] = 1.0f;

	return (2 * bmVectorLen(initialVelocity) * bmSin(theta) / (-worldGravity));
}

/* --------------------------------------------------------------------------------
   Function : UpdateProjectilePitch
   Purpose : rotatates the projectile to point feet first in direction of arc
   Parameters : Projectile Actor Instance;
   Returns : 
   Info :
*/

void UpdateProjectilePitch(ACTORINSTANCE *projectile)
{
	TBVector			zeroVector = { 0.0f, 0.0f, 0.0f, 1.0f };
	TBVector			tempVec = { 1.0f, 0.0f, 0.0f, RAD(-50.0f) };
	TBQuaternion		tempQuat, temp2Quat, another;

	//check what animation is playing, "fly1" is default
	if(CheckIfPlayingAnimation(projectile, "fly2"))
	{
		tempVec[3] = RAD(40.0f);
	}
	bmRotationToQuat(temp2Quat, tempVec);			//feet first
	CreateQuatFromPoints(tempQuat, zeroVector, projectile->actorBehaviour->physics.velocity, 0.0f);

	bmQuatMultiply(another, temp2Quat, tempQuat);
	SetActorOrientation(projectile, another);
}

/* --------------------------------------------------------------------------------
   Function : UpdateProjectilePitch
   Purpose : rotatates the projectile to point feet first in direction of arc
   Parameters : Projectile Actor Instance;
   Returns : 
   Info :
*/
void UpdateProjectilePitch(ACTORINSTANCE *projectile,float xAngle)
{
	TBVector			zeroVector = { 0.0f, 0.0f, 0.0f, 1.0f };
	TBVector			tempVec = { 1.0f, 0.0f, 0.0f, xAngle };
	TBQuaternion		tempQuat, temp2Quat, another;

	//check what animation is playing, "fly1" is default
	bmRotationToQuat(temp2Quat, tempVec);			//feet first
	projectile->actorBehaviour->physics.velocity[W] = 1.0f;
	CreateQuatFromPoints(tempQuat, zeroVector, projectile->actorBehaviour->physics.velocity, 0.0f);

	bmQuatMultiply(another, temp2Quat, tempQuat);
	SetActorOrientation(projectile, another);
}


/*  --------------------------------------------------------------------------------
	Function	: FollowFixedPath
	Purpose		: Get a projectile to follow the curve
	Parameters	: ACTORINSTANCE for projectile
	Returns		: 
	Info		: 
*/

void FollowFixedPath(ACTORINSTANCE *actorInstance)
{
	if (actorInstance->characterInfo->projecting.fixedPathEnable)
	{
		TBVector	direction;
		float		tempFloat;

		if (actorInstance->characterInfo->projecting.t > 1.0f) actorInstance->characterInfo->projecting.t = 1.0f;

		FindPointOnCurve(actorInstance->actorInstance.position, actorInstance->characterInfo->projecting.points[0],
			actorInstance->characterInfo->projecting.points[1],
			actorInstance->characterInfo->projecting.points[2],
			actorInstance->characterInfo->projecting.points[3],
			actorInstance->characterInfo->projecting.t);

		if (actorInstance->characterInfo->projecting.t == 1.0f)
		{
			actorInstance->characterInfo->projecting.fixedPathEnable = false;
		}
		else
		{
			actorInstance->characterInfo->projecting.t += fTime/actorInstance->characterInfo->projecting.time;

			FindPointOnCurve(direction, actorInstance->characterInfo->projecting.points[0],
				actorInstance->characterInfo->projecting.points[1],
				actorInstance->characterInfo->projecting.points[2],
				actorInstance->characterInfo->projecting.points[3],
				actorInstance->characterInfo->projecting.t);

			bmVectorSub(actorInstance->actorBehaviour->physics.velocity, direction, actorInstance->actorInstance.position);
			bmVectorCDiv(actorInstance->actorBehaviour->physics.velocity, actorInstance->actorBehaviour->physics.velocity, 60.0f);
		}
	}
}