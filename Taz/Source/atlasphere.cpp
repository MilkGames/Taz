// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2001 Interactive Studios Ltd.
//
//      File : Atlasphere.cpp
//   Purpose : Atlas-Sphere code
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "main.h"

#include "physics.h"
#include "status.h"
#include "rumble.h"
#include "animation.h"
#include "atlassphere.h"
#include "player.h"
#include "util.h"
#include "sfx.h"
#include "textures.h"

#define MAX_SPHERES		(4)
#define SPHERE_RADIUS	(160.0f)

ATLASSPHERE::ATLASSPHERE(ACTORINSTANCE *pActorInstance, char *sSphereName)
{
	TBVector scale =	{1.0f, 1.0f, 1.0f, 1.0f};
	TBVector zero =		{0.0f, 0.0f, 0.0f, 1.0f};
	char	instanceName[64];

	ZEROCLASS;

	this->pBoundToActor = pActorInstance;

	if (pActorInstance)
	{
		this->pBoundToActor->actorInstance.position[Y] = -1598.0f;
		this->pBoundToActor->actorStatus->timeOfLastSpeedBoost = -12.0f;
	}

	sprintf(instanceName,"atlassphere%d",instanceCtr);
	pSphereActor = CreateActorInstance(gameStatus.player1->instanceList, sSphereName, instanceName);

	ASSERTM(pSphereActor, "Couldn't find sphere");

	CreateActorInstanceShadow(pSphereActor, METERTOUNIT(0.7f),METERTOUNIT(1.5),SHADOWMAP_RADIUS_TAZ);

	pSphereActor->flags|=ACTORFLAG_CUSTOMDRAW;

	homeInPoint = 0;
	collidedLastTime = false;

	bmVectorCopy(pSphereActor->actorInstance.scale, scale);

	ySphereOffset = SPHERE_RADIUS;
	sphereActorRadius = SPHERE_RADIUS;

	if (pActorInstance)
	{
		pActorInstance->actorStatus->lastState = pActorInstance->actorStatus->currentState;
		pActorInstance->actorStatus->currentState = STATE_ATLASSPHERES;
		pActorInstance->actorAnimation->useMotionBone = FALSE;
		
		SetActorMinFricAngle(pActorInstance->actorBehaviour,1.0f);

		pActorInstance->actorBehaviour->physics.flags |= PHYSFLAG_WALLGROUNDCOLLFIX;
		pActorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;

		originSetAtlas(pActorInstance);
	}

	pAnimTexture = bkLoadTexture(NULL, "extras\\ballcolour2.gif", BDEFAULTGROUP);
	ASSERT(pAnimTexture);

	int	ctr = instanceCtr;
	
	// CPW: add to our internal linked list
	if(instanceCtr == 0)
	{
		ppSphereList = (ATLASSPHERE **)ZALLOC(sizeof(ATLASSPHERE *) * MAX_SPHERES);
		ASSERTM(ppSphereList, "Couldn't MALLOC ppSphereList :(");
	}

	ppSphereList[instanceCtr] = this;
	instanceCtr++;

	ctr = instanceCtr;
}

ATLASSPHERE::~ATLASSPHERE(void)
{
	// CPW: pRootNode->remove(this);

	int	ctr = instanceCtr;

	instanceCtr--;

	ctr = instanceCtr;

	if(instanceCtr == 0)
	{
		SAFE_FREE(ppSphereList);
	}
}

void ATLASSPHERE::setBoundToActor(ACTORINSTANCE *pActorInstance)
{
	this->pBoundToActor = pActorInstance;

	pActorInstance->actorStatus->lastState = pActorInstance->actorStatus->currentState;
	pActorInstance->actorStatus->currentState = STATE_ATLASSPHERES;
	pActorInstance->actorAnimation->useMotionBone = FALSE;
		
	SetActorMinFricAngle(pActorInstance->actorBehaviour,1.0f);

	pActorInstance->actorBehaviour->physics.flags |= PHYSFLAG_WALLGROUNDCOLLFIX;
	pActorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;

	originSetAtlas(pActorInstance);
}

void ATLASSPHERE::calculateAI(float enemySpeed, float enemySkill)
{
	TBVector dirHomeInOn;
	float dist;
	float dp;

	bmVectorSub(dirHomeInOn, homeInOn, pBoundToActor->actorInstance.position);

	dist = bmSqrt((dirHomeInOn[X]*dirHomeInOn[X]) + (dirHomeInOn[Z]*dirHomeInOn[Z]));
	dirHomeInOn[Y] = 0.0f;

	dp =	dirHomeInOn[X] * pBoundToActor->actorBehaviour->physics.velocity[X] +
			dirHomeInOn[Z] * pBoundToActor->actorBehaviour->physics.velocity[Z];

	backwards = (dp<0.0f)?1:0;

	bmVectorScaleToLength(dirHomeInOn, dirHomeInOn, enemySpeed);

	if((pBoundToActor->actorStatus->timeWithoutGroundCollision < 0.1f)
		|| (pBoundToActor->actorStatus->timeWithoutWallCollision < 0.1f))// CPW: stop it turning in mid-air
	{

		bmVectorLerp(pBoundToActor->actorBehaviour->physics.velocity, pBoundToActor->actorBehaviour->physics.velocity, dirHomeInOn, enemySkill);
	}

	// JW: Play the computer players animations

	TBVector vel;
	
	float speed, animSpeed;
	
	TBActorAnimSegment *pAnim, *pCurrentAnim;
	
	/*speed = bmVectorLen(pBoundToActor->actorBehaviour->physics.velocity);

	// find the appropriate movement animation to play
		animationToPlay = ChooseMovementAnimationForTazInSphere(controller, actorInstance);

	// play new animation
	if(CurrentAnimation(actorInstance)!=animationToPlay)	// am i already playing this anim
	{
			FlushAnimationQueue(actorInstance);
			PlayAnimationBySegmentPointer(actorInstance,animationToPlay,1,1,0,0.1f,MOVE);
	}
							
	if(speed > 50.0f)
	{
		vel[Y] = 0.0f;
		vel[W] = 0.0f;
		bmRotationToQuat(pBoundToActor->actorInstance.orientation, vel);
								
		animSpeed = speed;

		if(backwards)
		{
			if (!CheckIfPlayingAnimation(&pBoundToActor->actorInstance, "runspherereverse", 0))
			{
				FlushAnimationQueue(&pBoundToActor->actorInstance);
			
				PlayAnimationByName(&pBoundToActor->actorInstance, "runspherereverse", speed, 1, 0, 0.2f, MOVE);
			}
		}
		else
		{
			if (!CheckIfPlayingAnimation(&pBoundToActor->actorInstance, "runsphere", 0))
			{
				FlushAnimationQueue(&pBoundToActor->actorInstance);
				
				PlayAnimationByName(&pBoundToActor->actorInstance, "runsphere", speed, 1, 0, 0.2f, MOVE);
			}
		}
			
		//pAnim = baFindAnimSegment(&pBoundToActor->actorInstance, "runsphere");
	}
	else
	{
		if (!CheckIfPlayingAnimation(&pBoundToActor->actorInstance, "spherestatic", 0))
		{
			FlushAnimationQueue(&pBoundToActor->actorInstance);
				
			PlayAnimationByName(&pBoundToActor->actorInstance, "spherestatic", 0.0f, 1, 0, 0.2f, MOVE);
		}

		//pAnim = baFindAnimSegment(&pBoundToActor->actorInstance, "spherestatic");
		//animSpeed = 0;
	}
								
	/*pCurrentAnim = CurrentAnimation(pBoundToActor);

	if(pCurrentAnim && pCurrentAnim->crc != pAnim->crc)	// am i already playing this anim?
	{
		FlushAnimationQueue(pBoundToActor);
		PlayAnimationBySegmentPointer(pBoundToActor, pAnim, 1,1,0,0.1f,MOVE);
	}
								
	baSetAnimSpeed(pBoundToActor->actorInstance.rootNodeInstance, 1, (int)animSpeed, 0,0);*/

	//if(pRunSphere && pStaticSphere && pRunSphereReverse)
	//{
		vel[X] = pBoundToActor->actorInstance.position[X] - pBoundToActor->actorStatus->positionLastFrame[X];
		vel[Z] = pBoundToActor->actorInstance.position[Z] - pBoundToActor->actorStatus->positionLastFrame[Z];
	
		speed = bmSqrt(vel[X]*vel[X] + vel[Z]*vel[Z]);

		bmDirectionToQuat(pBoundToActor->actorInstance.orientation, pBoundToActor->actorBehaviour->physics.velocity, 0.0f);
		
		if(speed > 30.0f)
		{
			if(backwards)
				pAnim = baFindAnimSegment(&pBoundToActor->actorInstance, "runspherereverse");
			else
				pAnim = baFindAnimSegment(&pBoundToActor->actorInstance, "runsphere");
			
			vel[Y] = 0.0f;
			vel[W] = 0.0f;
			bmDirectionToQuat(pBoundToActor->actorInstance.orientation, vel, 0.0f);
			
			animSpeed = speed;
		}
		else
		{
			pAnim = baFindAnimSegment(&pBoundToActor->actorInstance, "spherestatic");
			animSpeed = 0;
		}
		
		if(CurrentAnimation(pBoundToActor) != pAnim)	// am i already playing this anim?
		{
			FlushAnimationQueue(pBoundToActor);
			PlayAnimationBySegmentPointer(pBoundToActor, pAnim, 1.0f,1,0,0.1f,MOVE);
		}
		
		//baSetAnimSpeed(pBoundToActor->actorInstance.rootNodeInstance, 1, (int)animSpeed, 0,0);
	//}
}

void ATLASSPHERE::applySphereRotationToActor(void)
{
	ActorRotate(this->pBoundToActor, this->pSphereActor->actorInstance.orientation, 0.1f);
}

void ATLASSPHERE::beforeYouCallAnythingElseCallThis(void)
{
	//if the player is taz-spinning move their origin to the right place

	if ((this->pBoundToActor->actorStatus->currentState == STATE_SPINUP) ||
		(this->pBoundToActor->actorStatus->currentState == STATE_SPIN) ||
		(this->pBoundToActor->actorStatus->currentState == STATE_SPINDOWN) ||
		(this->pBoundToActor->actorStatus->currentState == STATE_ATLASSPINUP) ||
		(this->pBoundToActor->actorStatus->currentState == STATE_ATLASSPIN) ||
		(this->pBoundToActor->actorStatus->currentState == STATE_ATLASSPINDOWN))
	{
		originSetNormal(this->pBoundToActor);

		if ((this->pBoundToActor->actorStatus->currentState == STATE_ATLASSPINDOWN) ||
			(this->pBoundToActor->actorStatus->currentState == STATE_SPINDOWN))
		{
			TBVector upright = {0.0f, 0.0f, 0.0f, 1.0f};
			ActorRotate(pBoundToActor, upright, 0.0f);
		}
	}
	else
	{
		originSetAtlas(this->pBoundToActor);
	}
}

void ATLASSPHERE::calculateRoll(void)
{
	// CR: Update rotation based on velocity 
	TBVector					delta;
	float						diameter = SPHERE_RADIUS * 2;
	float						length;

	// CR: If we are on the ground compute base our orientation
	// on our velocity

	delta[X] = pBoundToActor->actorInstance.position[X] - pBoundToActor->actorStatus->positionLastFrame[X];
	delta[Z] = pBoundToActor->actorInstance.position[Z] - pBoundToActor->actorStatus->positionLastFrame[Z];

	length = bmSqrt(delta[X]*delta[X] + delta[Z]*delta[Z]);

	ASSERTM(length >= 0.0f, "bmSqrt() has gone tits up");

	if(length)
	{
		TBVector		v;
		TBQuaternion	q, tempQuat;

		v[X] = -(delta[Z]/length);						// CR: Fill in the correct axis
		v[Y] =  0.0f;
		v[Z] =  (delta[X]/length);
		v[W] =  ((length*HALFPI)/diameter);				// CR: And the amount of rotation

		// CR: Contruct rotation quaternion
		bmRotationToQuat(q, v);

		// CR: Concat. quaternions
		bmQuatMultiply(tempQuat, pSphereActor->actorInstance.orientation, q);

		bmQuatCopy(pSphereActor->actorInstance.orientation, tempQuat);

		// CR: Normalise the result as they quickly go weird!
		// CPW: we can actually get away with only doing this every few seconds
		// CPW: look into this ...
		utilNormaliseQuaternion(pSphereActor->actorInstance.orientation);
	}

	length = bmVectorLen(pBoundToActor->actorBehaviour->physics.velocity);

	if(length < 1.0f)
		bmVectorZero(pBoundToActor->actorBehaviour->physics.velocity);
	else
		bmVectorScaleToLength(pBoundToActor->actorBehaviour->physics.velocity, pBoundToActor->actorBehaviour->physics.velocity, length - 0.05f);
	
	bmVectorCopy(pSphereActor->actorInstance.position, pBoundToActor->actorInstance.position);
	pSphereActor->actorInstance.position[Y] += ySphereOffset;

	bmVectorCopy(this->oldVel, pBoundToActor->actorBehaviour->physics.velocity);
}

void ATLASSPHERE::calculateSphereCollision(void)
{
	int i;
	ATLASSPHERE *ptr;

	for(i=0; i<instanceCtr; i++)
	{
		float distToNeighbour, minDistToNeighbour;
		TBVector vecToNeighbour, dirToNeighbour;

		ptr = ppSphereList[i];
		ASSERT(ptr);

		if(ptr == this)
			continue;
#ifdef	REPLICATEWILSONSMADCOLLISION
#ifndef _MASTER
		if(controller1.l3Channel->value)
		{
			TBVector		tempVel;

			bmVectorSub(tempVel, ptr->pSphereActor->actorInstance.position, this->pSphereActor->actorInstance.position);
			bmVectorScaleToLength(tempVel, tempVel, METERTOUNIT(30.0f));
			SetActorVelocity(this->pBoundToActor->actorBehaviour, tempVel);
			continue;
		}
#endif
#endif
		// CPW: work out distance between us and another sphere

		bmVectorSub(vecToNeighbour, this->pSphereActor->actorInstance.position, ptr->pSphereActor->actorInstance.position);
		distToNeighbour = bmVectorLen(vecToNeighbour);
		minDistToNeighbour = this->sphereActorRadius + ptr->sphereActorRadius;

		if(1) //collidedLastTime == false)
		{
			if(distToNeighbour < minDistToNeighbour)
			{
				float mySpeed;
				TBVector myVel, myDir;
				float dp;

				bmVectorCopy(myVel, pBoundToActor->actorBehaviour->physics.velocity);
				mySpeed = bmVectorLen(myVel);

				// CPW: take a copy of old velocity, so that the other ball can use it
				bmVectorCopy(this->oldVel, myVel);

				// CPW: normalise vectors
				bmVectorNorm(myDir, myVel);
				bmVectorNorm(dirToNeighbour, vecToNeighbour);

				dp = bmVectorDot(myDir, dirToNeighbour);
				
				(dp > 0) ? dp += 1.0f : dp -= 1.0f;
				
				dp /= 2.0f;

				bmVectorLerp(pBoundToActor->actorBehaviour->physics.velocity, ptr->oldVel, pBoundToActor->actorBehaviour->physics.velocity, dp);

				if((mySpeed = bmVectorLen(pBoundToActor->actorBehaviour->physics.velocity)) > worldMaxGravSpeed)
				{
					bmVectorScaleToLength(pBoundToActor->actorBehaviour->physics.velocity, pBoundToActor->actorBehaviour->physics.velocity, worldMaxGravSpeed);
				}
				else
				if(mySpeed < METERTOUNIT(2.0f))
				{
					if(mySpeed != 0.0f)
					{
						bmVectorScaleToLength(pBoundToActor->actorBehaviour->physics.velocity, pBoundToActor->actorBehaviour->physics.velocity, METERTOUNIT(3.0f));
					}
					else
					{
						// CMD: in the unlikely event that the velocity is exactly zero
						// CMD: we should perhaps provide some velocity in the opposite direction from the collidee
						bmVectorSub(pBoundToActor->actorBehaviour->physics.velocity, this->pSphereActor->actorInstance.position, ptr->pSphereActor->actorInstance.position);
						SafeScaleToLength(pBoundToActor->actorBehaviour->physics.velocity, pBoundToActor->actorBehaviour->physics.velocity, METERTOUNIT(3.0f));
					}
				}

				collidedLastTime = false;

				PlaySample("cagecrash.wav", 255);

				RequestRumbleEffect(gameStatus.player[i].controller, (int ) (mySpeed / 20.0f), 0.0f, 0.5f, 0.75f, 0.0f, WAVEFORM_NORMAL);
			}
			else
			{
				collidedLastTime = false;
			}
		}
		else
		{
			// CPW: We collided last time, so do an extra check here to ensure that the spheres have actually been pushed
			// CPW: apart. If they haven't then we could end up with the two spheres stuck together :(

			if(distToNeighbour < minDistToNeighbour)
			{
				// CPW: Evasive maneuvres, Mr Data
				// CPW: Try pushing the spheres apart the way they came

				float overBy = minDistToNeighbour - distToNeighbour;
				ASSERTM(overBy > 0, "Impossible!");

				bkPrintf(" *** Stuck together!!! ***\n");
				// CMD: WILSOOOON, what on earth were you thinking!
//				bmVectorNegate(pBoundToActor->actorBehaviour->physics.velocity, pBoundToActor->actorBehaviour->physics.velocity);
//				bmVectorCMul(pBoundToActor->actorBehaviour->physics.velocity, pBoundToActor->actorBehaviour->physics.velocity, 2.0f);
				collidedLastTime = false;
			}
			else
			{
				// CPW: It was OK, so all clear
				collidedLastTime = false;
			}
		}
	}
}

void ATLASSPHERE::calculateMagnets(ACTORINSTANCE **ppActorMagnets, int numMagnets)
{
	ACTORINSTANCE *pActorMagnet;
	int m;

	for(m=0; m<numMagnets; m++)
	{
		TBVector vectToMagnet;
		float distToMagnet;
		float force;

		pActorMagnet = ppActorMagnets[m];

		bmVectorSub(vectToMagnet, pActorMagnet->actorInstance.position, pSphereActor->actorInstance.position);
		distToMagnet = bmVectorLen(vectToMagnet);
		bmVectorNorm(vectToMagnet, vectToMagnet);

		force = 100000.0f / distToMagnet;
		bmVectorCMul(vectToMagnet, vectToMagnet, force);

		// CPW: bkPrintf("force = %f\n", force);
		bmVectorAdd(pBoundToActor->actorBehaviour->physics.velocity, pBoundToActor->actorBehaviour->physics.velocity, vectToMagnet);
	}
}

// JW: For normal Taz anims the origin is at the characters feet
// Move the origin to the centre of their bodies
// move the character back down to the ground if he's too high
void ATLASSPHERE::originSetNormal(ACTORINSTANCE *pActorInstance)
{
	//if the character has previously been moved up we need to move him back down
	//check the current offset
	
	if(!ySphereOffset)
	{
		TBVector	offset;
		TBVector	upright = {0.0f, 0.0f, 0.0f, 1.0f};

		SETVECTOR(offset,0.0f, SPHERE_RADIUS, 0.0f, 1.0f);
		SetActorCollSphere(pActorInstance->actorBehaviour, offset, SPHERE_RADIUS);
		ySphereOffset = SPHERE_RADIUS;

		pActorInstance->actorInstance.position[Y] -=SPHERE_RADIUS;
		gameStatus.player[pActorInstance->playerNo].camera->actorInstanceLookAtOffset[Y] = 250.0f;

		ActorRotate(pActorInstance, upright, 0.0f);
	}
}

// JW: For Atlas Taz anims the origin is at the characters centre
// move character up to compensate
// coll sphere doesn't need to change
void ATLASSPHERE::originSetAtlas(ACTORINSTANCE *pActorInstance)
{
	//if we've already called this we don't want to call it again or the character will just move up and up
	//check to see if the character is far up enough already

	if(ySphereOffset)
	{
		TBVector	offset;
		TBVector	upright = {0.0f, 0.0f, 0.0f, 1.0f};

		SETVECTOR(offset,0.0f, 0.0f, 0.0f, 1.0f);
		SetActorCollSphere(pActorInstance->actorBehaviour, offset, SPHERE_RADIUS);
		ySphereOffset = 0;

		pActorInstance->actorInstance.position[Y] +=SPHERE_RADIUS;

		gameStatus.player[pActorInstance->playerNo].camera->actorInstanceLookAtOffset[Y] = 250.0f - SPHERE_RADIUS;

		ActorRotate(pActorInstance, upright, 0.0f);
	}
}


void ATLASSPHERE::draw(void)
{
	if(!this)
		return;

	if((this->pSphereActor->flags & ACTORFLAG_CUSTOMDRAW)&&(~pSphereActor->flags&ACTORFLAG_DONTDRAW))
	{
		SetTextureFrameManually(pAnimTexture, ballColour);
		baDrawActorInstance(&this->pSphereActor->actorInstance, BSETPRIMALPHATYPE_SOLID|BSETPRIMALPHATYPE_CKEY|BSETPRIMALPHATYPE_ALPHA);
	}
}