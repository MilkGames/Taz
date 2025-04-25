// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : physics.cpp
//   Purpose : physics functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "physics.h"
#include "status.h"
#include "maths.h"
#include "collisioncache.h"
#include "main.h"
#include "timer.h"
#include "characters.h"

// collision info array size for processActorBehaviour
#define		MAX_COLL_FACES		100
#define		VectorDot(src1,src2) (src1[0]*src2[0] + src1[1]*src2[1] + src1[2]*src2[2])
#define		MAX_SPEEDRATIO			(60.0f/40.0f)
#define		MAX_SPEEDRATIO_STEPS	4.0f

float	worldGravity = NORMAL_WORLD_GRAVITY;//20f
float	worldMaxGravSpeed = METERTOUNIT(50.0f);//9;

#define SLIDE_FIX

/* --------------------------------------------------------------------------------
   Function : ProcessActorBehaviour
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

COLL_INFO *ProcessActorBehaviour( ACTORINSTANCE *actorInstance, COLLISIONLISTENTRY **meshLists, int noofVertLists,
									ACTORINSTANCE **cylInstances, COLL_CYLINDER *cylList, int noofCylLists, 
									ACTORINSTANCE **sphereInstances, COLL_SPHERE *sphereList, int noofSpheres, uint32 flags, COLL_INFO *retcollinfo)
{
	TBActorInstance		*babelActorInstance = &actorInstance->actorInstance;
	ACTOR_BEHAVIOUR		*actorBehaviour = actorInstance->actorBehaviour;
	ACTOR_SPHERE		*actorSphere = &actorInstance->actorBehaviour->collSphere;
	ACTOR_PHYSICS		*physics = &actorBehaviour->physics;
	TBVector			stepvec,tempVel;
	float				speed;
	COLL_INFO			collinfo;
	float				invPolyFric;
	TBVector			pos;
	int					cidx, sphereIndex;
	COLL_FACEINFO		faceinfo[MAX_COLL_FACES];
	int					numfaces;
	int					midx,rmidx,rsidx,rcidx;
	int					i;
	int					nf;
	TBVector			posInc;
	int					f;
	TBVector			fullStep,corVel;
	float				fullSpeed,tempFloat;
	char				haveCollided = 0;
	ACTORINSTANCE		*instance;

	actorInstance->actorInstance.position[W] = 1.0f;

	
	// if speedRatio has gone bezerk lets not perform any move/collision to prevent
	// system from locking up. this shouldn't really happen on final version
	// remove at your own risk
	if (fps < 2) 
	{
		if (retcollinfo)
		{
			retcollinfo->cylinder[0].Index = -1;
			retcollinfo->sphere[0].Index = -1;
			retcollinfo->mesh[0].Index = -1;
		}
		return NULL;
	}

	ZEROVECTOR(physics->velImpactFromLastColl);

	if(actorBehaviour->rotSpeed == 0.0f)
	{
		bmQuatCopy(actorBehaviour->rotAim, babelActorInstance->orientation);
	}
	else
	{
		// face requested direction
		TBQuaternion tempQuat;
		bmQuatCopy(tempQuat, babelActorInstance->orientation);
		bmQuatSlerpAtFixedSpeed(babelActorInstance->orientation, tempQuat, actorBehaviour->rotAim, actorBehaviour->rotSpeed*speedRatio*2);
	}

	// PP: set the W to 1 if each & every element of the quat equals zero
	if((babelActorInstance->orientation[0] == 0.0f)
		&&(babelActorInstance->orientation[1] == 0.0f)
		&&(babelActorInstance->orientation[2] == 0.0f)
		&&(babelActorInstance->orientation[3] == 0.0f))
	{
		// PP: IFDEFOUT: I hope this hasn't caused any confusion.  Mmm, with hindsight I should've just write-enabled it.
#ifdef CHRIS
		bkPrintf("ERK!\n");
#endif// PP: def CHRIS

		babelActorInstance->orientation[3] = 1.0f;
	}


	// do not allow to go over speed limit
	checkSpeedLimit(physics, worldMaxGravSpeed);
	physics->speedIn = physics->speed;
	bmVectorCopy(tempVel,physics->velocity);
	bmVectorSet(posInc, 0.0f, 0.0f, 0.0f, 1.0f);

	if(~physics->flags&PHYSFLAG_NOMOVE)
	{
		if (physics->flags&PHYSFLAG_NOGRAVITY)
		{
			if (physics->speed==0.0f)
			{
				physics->velocity[W] = 1.0f;
			}
			posInc[0] = physics->velocity[X]/fps;
			posInc[1] = physics->velocity[Y]/fps;
			posInc[2] = physics->velocity[Z]/fps;
			posInc[3] = 1.0f;
		}
		else
		{
			if(physics->flags&PHYSFLAG_DIRGRAVITY)
			{
				if(physics->coll)
				{
					ApplyFriction(physics);
				}
				ApplyGravityInDirection(posInc, physics->velocity, actorInstance->characterInfo->portalGravity, worldGravity);
			}
			else
			{
				if (physics->coll)
				{
					ApplyFriction(physics);
				}
				if ((actorInstance->actorStatus)&&(actorInstance->actorStatus->currentState == STATE_JUMP))
				{
					ApplyGravity(posInc, physics->velocity, worldGravity);
				}
				else
				{
#ifdef SLIDE_FIX
					// TP: we dont want to apply gravity if the character is in a stable collision with the ground and is not trying to move
					// TP: otherwise we get that nasty sliding shenanigans
					if ((!physics->coll)||(physics->lastAverageNormal[Y]<=physics->minFricAngle)||(physics->speed >= physics->minVel))
#endif
					{
						ApplyGravity(posInc, physics->velocity, worldGravity*physics->lastInvFriction);
					}
				}
			}
		}
	}

	physics->speed = bmVectorLen(physics->velocity);
	
	bmVectorCopy(stepvec, posInc);
	bmVectorCopy(fullStep,stepvec);

	stepvec[W] = 1.0f;
	fullStep[W] = 1.0f;
	physics->velocity[W] = 1.0f;

//	bmVectorCopy(corVel,fullStep);
	fullSpeed = bmVectorLen(fullStep);
	speed = fullSpeed;

	if (!retcollinfo)
	{
		retcollinfo = (COLL_INFO *)ZALLOC(sizeof(COLL_INFO));
		if (retcollinfo==NULL)
		{
			bkPrintf("*** ERROR *** could not allocate enough memory to perform collision test.\n");
			return NULL;
		}
	}
	retcollinfo->cylinder[0].Index = -1;
	retcollinfo->sphere[0].Index = -1;
	retcollinfo->mesh[0].Index = -1;

	rmidx = 0;
	rsidx = 0;
	rcidx = 0;

	physics->flags &= ~(PHYSFLAG_WALLCOLL|PHYSFLAG_GROUNDCOLL);

	do
	{
		if (fullSpeed > (actorSphere->radius*0.5f))
		{
			SafeScaleToLength(stepvec,fullStep,actorSphere->radius*0.5f);
			speed = actorSphere->radius*0.5f;
			fullSpeed -= speed;
		}
		else
		{
			if (fullSpeed==0.0f)
			{
				bmVectorCopy(stepvec,fullStep);
			}
			else
			{
				SafeScaleToLength(stepvec,fullStep,fullSpeed);
				speed = fullSpeed;
				fullSpeed = 0.0f;
			}
		}
		// if multiple collision is on and speed is too huge to handle in one go

		// now that we have the stepvec we can apply inertia
		//applyInertia(physics);

		invPolyFric = 0.0f;

		//--------- cylinder collision response
		// move to virtual new position
		bmVectorCopy(pos,babelActorInstance->position);
		pos[W] = 1.0f;
		// if this is the last step rescale stepvec to remaining speed
		bmVectorAdd(pos,pos,stepvec);

		//--------- cylinder collision response
		cidx = ProcessCylinderCollision(pos,actorBehaviour, cylInstances, cylList, noofCylLists, actorInstance, stepvec, &collinfo, retcollinfo, &rcidx);

		//--------- sphere collision response
		//sensible variable name alert
		sphereIndex = ProcessSphereCollision(pos, actorBehaviour, sphereInstances, sphereList, noofSpheres, actorInstance, stepvec, &collinfo, retcollinfo, &rsidx);

		if ((cidx)||(sphereIndex))	haveCollided = TRUE;

		bmVectorAdd(pos,pos,actorSphere->offset);

		if ( cidx )
			physics->flags |= PHYSFLAG_CYLINDERCOLL;
		else
			physics->flags &= ~PHYSFLAG_CYLINDERCOLL;

		//--------- sphere-mesh collision response

		// check for collisions
		numfaces = 0;
		midx = 0;
		
		// loop thru all meshes and store return info
		for(f=0;f<noofVertLists;f++)
		{
			nf = MAX_COLL_FACES - numfaces;

			StartProfileTimer(&tonyProfile,6,"spcoll");

			if(actorInstance->flags&ACTORFLAG_NOWORLDCOLLISION)
			{
				nf=FALSE;
			}
			else
			{
				SphereFaceCollision(meshLists[f],pos,actorSphere->radius,&faceinfo[numfaces],&nf);
			}
			
			EndProfileTimer(&tonyProfile,6);

			// did we collide with any faces on this mesh?
			if ( nf )
			{
				//if actorinstance[f] callback then callback
				// if routine returns 1 then do physics else 
				instance = meshLists[f]->actorInstance;

				if ((instance)&&((~flags&PROCESSCOLLFLAG_DONTAFFECTOBJECTS)||(instance->flags&ACTORFLAG_ALWAYSCALLPRECOLLCALLBACK))
					&& (instance->PreRespondToCollisionRoutine) &&
					(!instance->PreRespondToCollisionRoutine(actorInstance, instance, NULL)))
				{
					if (rmidx>=MAX_COLL_MESHES)
					{
						// too many meshes in collision
						continue;
					}
					else
					{
						retcollinfo->mesh[rmidx].Index = collinfo.mesh[midx].Index = f;
						for(i=0;i<nf;i++)
						{
							retcollinfo->mesh[rmidx].polyIndexList[i] = faceinfo[i].index;
						}
						if ( nf < MAX_COLL_POLYS )
						{
							retcollinfo->mesh[rmidx].polyIndexList[nf] = -1;	// end of list marker
						}
						// TP: set flag to indicate that the collision was detected but no responce was made
						retcollinfo->mesh[rmidx].flags = RETFLAG_NORESPONCE;
						rmidx++;
						continue;
					}
				}
				else
				{
					// store them in return structure
					if((rmidx<MAX_COLL_MESHES)&&(midx<MAX_COLL_MESHES))
					{
						retcollinfo->mesh[rmidx].Index = collinfo.mesh[midx].Index = f;
						for(i=0;i<nf;i++)
						{
							collinfo.mesh[midx].polyIndexList[i] = faceinfo[i].index;
							retcollinfo->mesh[rmidx].polyIndexList[i] = faceinfo[i].index;
						}
						if ( nf < MAX_COLL_POLYS )
						{
							collinfo.mesh[midx].polyIndexList[nf] = -1;	// end of list marker
							retcollinfo->mesh[rmidx].polyIndexList[nf] = -1;	// end of list marker
						}
						retcollinfo->mesh[rmidx].flags = 0;
						midx++;
						rmidx++;
						numfaces += nf;
						if ( numfaces == MAX_COLL_FACES )
							break;
					}
					else
					{
						// too many meshes in collision
						continue;
					}
				}
			}
		}
		if ( midx < MAX_COLL_MESHES )
			collinfo.mesh[midx].Index = -1;	// end of list marker

		if ((numfaces/*&&speed*/)||(cidx)||(sphereIndex))
		{
			// yes there was a poly collision
			if((actorInstance->actorStatus)&&((actorInstance->actorStatus->currentState == STATE_SPIN)||(actorInstance->actorStatus->currentState == STATE_VEHICLE)))
			{
				bmVectorCopy(corVel,fullStep);
			}
			else
			{
				bmVectorCopy(corVel,physics->velocity);
			}

			tempFloat = corVel[Y];

			if (numfaces/*&&speed*/)
			{
				invPolyFric = PerformCollisionFaceResponse(actorInstance,numfaces,faceinfo,pos,invPolyFric,&collinfo,stepvec,speed,corVel);
				if ((actorInstance->actorStatus)&&(actorInstance->actorStatus->currentState == STATE_JUMP))
				{
					// TP: Dont allow the landscape to affect the height of the jump etc.
					corVel[Y] = tempFloat;
				}
				haveCollided = TRUE;
			}
			if (sphereIndex)
			{
				PerformSphereResponse(actorInstance,sphereIndex,sphereList,pos,&collinfo,stepvec,speed,corVel);

				invPolyFric = 1.0f;
			}
			if (cidx)
			{
				PerformCylinderResponse(actorInstance,cidx,cylList,pos,&collinfo,stepvec,speed,corVel);

				// TP: If it was a cylinder then make sure it slides
				invPolyFric = 1.0f;
			}
		}
		else 
		{
			// no there was NO collision
			if ( physics->coll )
			{
				// stopped colliding this frame
				// reset collision vars
				bmVectorZero(physics->lastAverageNormal);
				//memset(physics->lastAverageNormal,0,sizeof(physics->lastAverageNormal));
				if ( cidx == 0 )	// remove this line if you want coll flag to exclude cylinder collision
					physics->coll = false;
				//physics->flags &= ~(PHYSFLAG_WALLCOLL|PHYSFLAG_GROUNDCOLL);
				invPolyFric = 0.0f;
			}
			else
			{
				// no collision apply full gravity
				invPolyFric = 1.0f;
			}
		}

		bmVectorSub(babelActorInstance->position,pos,actorSphere->offset);

		physics->lastInvFriction = invPolyFric;

	} while (fullSpeed);

// TP: 	if ( rmidx < MAX_COLL_MESHES )
		retcollinfo->mesh[rmidx].Index = -1;	// end of list marker
// TP: 	if ( rsidx < MAX_COLL_MESHES )
		retcollinfo->sphere[rsidx].Index = -1;	// end of list marker
// TP: 	if ( rcidx < MAX_COLL_MESHES )
		retcollinfo->cylinder[rcidx].Index = -1;	// end of list marker

	if (haveCollided)
	{
		if((actorInstance->actorStatus)&&((actorInstance->actorStatus->currentState == STATE_SPIN)||(actorInstance->actorStatus->currentState == STATE_VEHICLE)))
		{
			bmVectorCDiv(physics->velocity, corVel, fTime);
		}
		else
		{
			if (physics->bounce)
			{
				bmVectorCMul(physics->velocity, physics->lastAverageNormal, physics->speed*physics->bounce);
			}
			else
			{
				bmVectorCopy(physics->velocity, corVel);
			}
		}
	}
	checkSpeedLimit(physics, worldMaxGravSpeed);
	//physics->speed = bmVectorLen(physics->velocity);

	bmVectorSub(physics->velImpactFromLastColl, physics->velocity, tempVel);

	return retcollinfo;
}

/*	--------------------------------------------------------------------------------
	Function 	: ApplyGravity
	Purpose 	: updates position and velocity as a result of gravity
	Parameters 	: pointer to component of stepvec, pointer to component of velocity, gravity
					NOTE: remember that gravity is -ve, ie in the real world = -9.81 m/s^2
	Returns 	: int
	Info 		:
*/
int ApplyGravity(TBVector positionInc, TBVector currentVelocity, float gravity, float t)
{
	if( (positionInc == NULL) || (currentVelocity == NULL) )
	{
		bkPrintf("****** ERROR ****** null vector passed to ApplyGravity\n");
		return FALSE;
	}

	//calculate position increment
	positionInc[1] = ((currentVelocity[1])*t) + (0.5f*gravity*(t*t));

	//update current velocity as a result of gravity
	currentVelocity[1] = currentVelocity[1] + (gravity*t);

	positionInc[0] = currentVelocity[0]*t;
	positionInc[2] = currentVelocity[2]*t;
	positionInc[3] = 1.0f;
	return TRUE;
}

/*	--------------------------------------------------------------------------------
	Function 	: ApplyGravityInDirection
	Purpose 	: applies gravity in a given direction
	Parameters 	: 
					NOTE: remember that gravity is -ve, ie in the real world = -9.81 m/s^2
	Returns 	: int
	Info 		:
*/
int ApplyGravityInDirection(TBVector positionInc, TBVector currentVelocity, TBVector direction, float gravity)
{
	int				counter;
	TBVector		acceleration;

	if( (positionInc == NULL) || (currentVelocity == NULL)  || (direction == NULL))
	{
		bkPrintf("****** ERROR ****** null vector passed to ApplyGravityInDirection\n");
		return FALSE;
	}
	if(bmVectorLen(direction) == 0.0f)
	{
		bkPrintf("****** ERROR ****** request for gravity to be directed along zero vector\n");
		return FALSE;
	}
	//calculate componant regular orthogonal componants of acceleration along direction 
	//vector and calculate new position and speed for each
	//make any sense?
	bmVectorNorm(acceleration, direction);
	acceleration[W] = 1.0f;
	bmVectorCMul(acceleration, acceleration, gravity);
	for(counter = 0; counter < 3; counter++)
	{
		//calculate position increment
		positionInc[counter] = ((currentVelocity[counter])/fps) + (0.5f*acceleration[counter]/(fps*fps));

		//update current velocity as a result of gravity
		currentVelocity[counter] = currentVelocity[counter] + (acceleration[counter]/fps);
	}
	positionInc[3] = 1.0f;
	return TRUE;
}

/*	--------------------------------------------------------------------------------
	Function 	: SetWorldGravity
	Purpose 	: 
	Parameters 	: float
	Returns 	: 
	Info 		:
*/

void SetWorldGravity(float newGravity)
{
	worldGravity = newGravity;
}

/*	--------------------------------------------------------------------------------
	Function 	: ApplyFriction
	Purpose 	: updates velocity as a result of friction
	Parameters 	: float *, float *
	Returns 	: int
	Info 		:
*/

int ApplyFriction( ACTOR_PHYSICS *physics )
{
	TBVector			tempVector;
	float				speed;

	if(physics->velocity == NULL)
	{
		return FALSE;
	}

	if (physics->friction==0.0f) return FALSE;

	if(physics->friction > 0.0f)
	{
		//+ve friction, go back to school
		physics->friction = -physics->friction;
	}

	//calculate length of current velocity vector
	speed = bmVectorLen(physics->velocity);
	if(speed)
	{
		//apply friction to speed
		speed += physics->friction/fps;
		//scale velocity vector
		SafeScaleToLength(tempVector, physics->velocity, speed);
		//copy modified vector to velocity
		bmVectorCopy(physics->velocity, tempVector);
	}
	//mission succesfully accomplished, jim
	return TRUE;
}


/*	--------------------------------------------------------------------------------
	Function 	: PerformSphereResponse
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PerformSphereResponse(ACTORINSTANCE *actorInstance,int numSpheres,COLL_SPHERE *sphereList,TBVector pos, COLL_INFO *collinfo,TBVector stepvec,float speed,TBVector corVel)
{
	TBVector	result,normal,tempVector,averageNormal;

	corVel[W] = 1.0f;

	for (int i=0;i<numSpheres;i++)
	{
		if (collinfo->sphere[i].Index == -1) break;

		sphereList[collinfo->sphere[i].Index].pos[W] = 1.0f;
		bmVectorSub(tempVector, pos, sphereList[collinfo->sphere[i].Index].pos);
		bmVectorNorm(averageNormal,tempVector);
		averageNormal[W] = 1.0f;

		ClipVector(result, corVel, averageNormal);

		bmVectorCopy(corVel, result);
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: PerformCollisionFaceResponse
	Purpose 	: 
	Parameters 	: ?
	Returns 	: invPolyFriction
	Info 		: 
*/
TBVector vectab[MAX_COLL_FACES];		// how much are we into vector
float	lentab[MAX_COLL_FACES];		// "" vector length
int		ontab[MAX_COLL_FACES];			// poly type
float fudge = 0.001f;
float PerformCollisionFaceResponse(ACTORINSTANCE *actorInstance,int numfaces,COLL_FACEINFO *faceinfo,TBVector pos, float invPolyFric,COLL_INFO *collinfo,
								   TBVector stepvec,float speed,TBVector corVel)
{
	TBActorInstance *babelActorInstance = &actorInstance->actorInstance;
	ACTOR_BEHAVIOUR *actorBehaviour = actorInstance->actorBehaviour;
	ACTOR_PHYSICS	*physics = &actorBehaviour->physics;
	TBVector		averageNormal;
	TBVector		averageNPoint;
	TBVector		groundNormal;
	float			vlen, gvlen;
	int				gnf, wnf;
	TBVector		vec;
	float			dot,tempFloat;
	float			upCos;
	int				j,i,nf;
	int				f;

	// collision occured
	bmVectorZero(averageNormal);
	bmVectorZero(averageNPoint);
	bmVectorZero(groundNormal);
	bmVectorZero(vec);

	// fill in tables
	for(f=0;f<numfaces;f++)
	{
		bmVectorSub(vec,faceinfo[f].nearestPoint,pos);
		
		// if poly nearest point is below pos point and the the poly face normal
		// hasn't too	much inclination then consider it ground
		if ( vec[1] < 0.0f && faceinfo[f].faceNormal[1] > physics->minFricAngle )
			ontab[f] = 2;	// ground
		else
			ontab[f] = 1;	// wall

		// find distance vector of sphere radius to poly nearest point
		tempFloat = bmVectorLen(vec);

		// TP: Make sure there is no divide by 0
		if (tempFloat) 
		{
			bmVectorCMul(vec,vec,actorBehaviour->collSphere.radius/tempFloat);
		}

		bmVectorAdd(vec,vec,pos);
		bmVectorSub(vectab[f],faceinfo[f].nearestPoint,vec);
		lentab[f] = bmVectorLen(vectab[f]);
	}

	// discard polys whos normal is almost identical
	for(f=0;f<numfaces;f++)
	{
		if ( ontab[f] == 0 )
			continue;

		i = f;
		for(j=0;j<numfaces;j++)
		{
			if ( ontab[j] == 0 || j == i )
				continue;

			// if less than 11 degrees (aprox.) apart discard poly that's furthest away
			if ( bmVectorDot(faceinfo[i].faceNormal,faceinfo[j].faceNormal) >= 0.98f )
			{
				if ( lentab[j] > lentab[i] )
				{
					ontab[i] = 0;
					i = j;
				}
				else
					ontab[j] = 0;
			}
		}
	}

	vlen = 0.0f;
	gvlen = 0.0f;
	upCos = 0.0f;
	nf = 0;
	gnf = 0;
	wnf = 0;

	// find average normals
	for(f=0;f<numfaces;f++)
	{
		if ( ontab[f] == 0 )
			continue;

		// longest into-vector
		if ( lentab[f] > vlen )
		{
			vlen = lentab[f];	// get the pop-out amount
		}

		// if wall add to average wall normal
		if ( ontab[f] == 1 )
		{
			bmVectorAdd(averageNormal,averageNormal,vectab[f]);
			wnf++;
		}
		
		// if ground add to average ground normal
		else
		{
			bmVectorAdd(groundNormal,groundNormal,vectab[f]);
			gnf++;
		}

		// average up angle for friction
		upCos += faceinfo[f].faceNormal[1];
		
		// get poly friciton
		invPolyFric += faceinfo[f].polyFriction;
		nf++;
	}

	bmVectorCopy(collinfo->wallNormal,averageNormal);
	bmVectorCopy(collinfo->groundNormal,groundNormal);

	// calculate average collision pop vector
	if ( vlen > 0.0f)
	{
		upCos /= nf;
		
		// final inverted friction to apply to gravity
		invPolyFric = (1.0f - (invPolyFric / nf)) * (1.0f/*-physics->friction*/);
		
		// treat walls as flat if there is ground collision as well
		// prevents popping into ground plane
		if ( gnf )
			averageNormal[1] = 0.0f;
		bmVectorAdd(averageNormal,averageNormal,groundNormal);
		bmVectorNorm(averageNormal,averageNormal);
		averageNormal[W] = 1.0f;
		
		// smooth pop out dir using average pop between this and last av. normal
		if ( VectorDot(physics->lastAverageNormal,averageNormal) != -1.0f )
		{
			bmVectorAdd(averageNormal,averageNormal,physics->lastAverageNormal);
			bmVectorNorm(averageNormal,averageNormal);
			averageNormal[W] = 1.0f;
		}

#ifdef SLIDE_FIX
	// TP: reduce length a little so as to maintain collision with the ground and avoid the collision/no collision cycle
	vlen-=fudge;
	if (vlen<0.0f) vlen = 0.0f;
#endif

		bmVectorCMul(averageNPoint,averageNormal,vlen);
		bmVectorCopy(physics->lastAverageNormal,averageNormal);
	}
	else
		invPolyFric = 1.0f;// - physics->friction;

	bmVectorAdd(pos,pos,averageNPoint);

	if ( !physics->coll )
	{
		// new collision this frame
		// no bounce, so remove velocity from impact
		if ( upCos <= physics->minFricAngle )
		{
			ClipVector(corVel,corVel,averageNormal);
		}
		else
			memset(corVel,0,sizeof(stepvec));
		invPolyFric = 0.0f;
	}
	else
	{
		// still colliding
		// divert speed along polygon surface
		dot = VectorDot(corVel,averageNormal);
		bmVectorCMul(vec,averageNormal,dot);
		bmVectorSub(corVel,corVel,vec);

		// this is to prevent sphere to gain upwards speed when colliding with
		// a wall at high speed. not very nice but works
		if ( (physics->flags & PHYSFLAG_WALLVELCORRECTION)/* && gnf*/ && (corVel[1] > 0.0f) )
		{
			corVel[1] = 0.0f;
		}

		if ((physics->flags & PHYSFLAG_WALLGROUNDCOLLFIX)&&gnf&&wnf&&(corVel[1]>0.0f))
		{
			corVel[1] = 0.0f;
		}

		// stick sphere to ground if angle not steep enough
		if ( upCos > physics->minFricAngle )
		{
			invPolyFric = 0.0f;
		}

#ifdef	DEBUGGING
		pos[1] += 150;
		addline(pos,averageNormal,200, 0,150,150, 0,255,255 );
		pos[1] -= 150;
#endif
	}

	physics->coll = true;

	// set return flags
	if ( wnf )
		physics->flags |= PHYSFLAG_WALLCOLL;
	if ( gnf )
		physics->flags |= PHYSFLAG_GROUNDCOLL;

	return invPolyFric;
}

/*	--------------------------------------------------------------------------------
	Function 	: ProcessCylinderCollision
	Purpose 	: process cylinder to cylinder collision
	Parameters 	: ?
	Returns 	: number of cylinder collisions occured
	Info 		: 
*/
int ProcessCylinderCollision(TBVector pos,ACTOR_BEHAVIOUR *actorBehaviour, ACTORINSTANCE **cylInstances, COLL_CYLINDER *cylList,int noofCylLists, 
									 ACTORINSTANCE *actorInstance, TBVector stepvec, COLL_INFO *collinfo, COLL_INFO *retcollinfo, int *rcidx)
{
	COLL_CYLINDER	cyl;
	int				cidx,f;
	float			y,horzInt,vertInt;
	TBVector		tempVector;
	PRECOLLINFO		preCollInfo;

	if (actorBehaviour->collCylinder[0].radius == 0.0f) 
	{
		collinfo->cylinder[0].Index = -1;	// end of list marker

		return 0;
	}

	bmVectorAdd(cyl.pos, pos, actorBehaviour->collCylinder[0].offset);
	cyl.height = actorBehaviour->collCylinder[0].height;
	cyl.radius = actorBehaviour->collCylinder[0].radius;
	cyl.type = actorBehaviour->collCylinder[0].type;
	cyl.pos[W] = 1.0f;
	cidx = 0;

	for(f=0;f<noofCylLists;f++)
	{
		cylList[f].pos[W] = 1.0f;
		if (CylinderCylinderCollision(&cyl,&cylList[f],&horzInt,&vertInt))
		{
			preCollInfo.type = PRECOLLTYPE_CYLINDER;
			if ((horzInt>vertInt)||(horzInt>cyl.radius))
			{
				preCollInfo.specific = COLLTYPE_CYLINDER_TOP;
			}
			else
			{
				preCollInfo.specific = COLLTYPE_CYLINDER_SIDE;
			}

			if ((cylInstances[f]->PreRespondToCollisionRoutine && (!(cylInstances[f]->PreRespondToCollisionRoutine(actorInstance, cylInstances[f], &preCollInfo))))
				||(actorInstance->PreRespondToCollisionRoutine && (!(actorInstance->PreRespondToCollisionRoutine(cylInstances[f],actorInstance, &preCollInfo)))))
			{
				// TP: Don't do collision responce
				if (horzInt>vertInt)
				{
					if ((*rcidx)<MAX_COLL_CYLINDERS)
					{
						retcollinfo->cylinder[*rcidx].Index = f;
						retcollinfo->cylinder[*rcidx].collType = COLLTYPE_CYLINDER_TOP;
						(*rcidx)++;
					}
				}
				else
				{
					if ((*rcidx)<MAX_COLL_CYLINDERS)
					{
						retcollinfo->cylinder[*rcidx].Index = f;
						retcollinfo->cylinder[*rcidx].collType = COLLTYPE_CYLINDER_SIDE;
						(*rcidx)++;
					}
				}
			}
			else
			{
				actorBehaviour->physics.coll = TRUE;
				// TP: if the collision looks like a top/bottom coll or the horzIntersection is dangerously large
				if ((horzInt>vertInt)||(horzInt>cyl.radius))
				{
					// top / bottom collision
					float	p1 = (cyl.pos[Y]-(cyl.height/2));
					float	p2 = (cylList[f].pos[Y] - (cylList[f].height/2));

					if (p1 > p2)
					{
						pos[Y] += (cylList[f].pos[Y] + (cylList[f].height/2))-(cyl.pos[Y]-(cyl.height/2));
					}

					collinfo->cylinder[cidx].Index = f;
					collinfo->cylinder[cidx].collType = COLLTYPE_CYLINDER_TOP;
					cidx++;
					if ((*rcidx)<MAX_COLL_CYLINDERS)
					{
						retcollinfo->cylinder[*rcidx].Index = f;
						retcollinfo->cylinder[*rcidx].collType = COLLTYPE_CYLINDER_TOP;
						(*rcidx)++;
					}

					actorBehaviour->physics.flags |= PHYSFLAG_GROUNDCOLL;
				}
				else
				{
					// side collision
					bmVectorSub(tempVector, pos, cylList[f].pos);
					tempVector[Y] = 0.0f;

					// TP: check for soft top
					SafeScaleToLength(tempVector,tempVector,cyl.radius+cylList[f].radius);

					y = pos[Y];
					bmVectorAdd(pos,cylList[f].pos,tempVector);
					pos[Y] = y;

					collinfo->cylinder[cidx].Index = f;
					collinfo->cylinder[cidx].collType = COLLTYPE_CYLINDER_SIDE;
					cidx++;
					if ((*rcidx)<MAX_COLL_CYLINDERS)
					{
						retcollinfo->cylinder[*rcidx].Index = f;
						retcollinfo->cylinder[*rcidx].collType = COLLTYPE_CYLINDER_SIDE;
						(*rcidx)++;
					}
					actorBehaviour->physics.flags |= PHYSFLAG_WALLCOLL;

				}
			}
		}
	}
	if ( cidx < MAX_COLL_CYLINDERS )
		collinfo->cylinder[cidx].Index = -1;	// end of list marker

	return cidx;
}


/*	--------------------------------------------------------------------------------
	Function 	: PerformCollisionCylinderResponse
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PerformCylinderResponse(ACTORINSTANCE *actorInstance,int numCyl,COLL_CYLINDER *cylList,TBVector pos, COLL_INFO *collinfo,TBVector stepvec,float speed,TBVector corVel)
{
	TBVector	averageNormal,tempVector;
	int			i;

	corVel[W] = 1.0f;
	pos[W] = 1.0f;


	for (i=0;i<numCyl;i++)
	{
		if (collinfo->cylinder[i].Index == -1) break;

		if (collinfo->cylinder[i].collType==COLLTYPE_CYLINDER_TOP)
		{
			bmVectorSet(averageNormal, 0.0f, 1.0f, 0.0f, 1.0f);
		}
		else
		{
			// TP: correct velocity
			cylList[collinfo->cylinder[i].Index].pos[W] = 1.0f;
			bmVectorSub(tempVector,pos,cylList[collinfo->cylinder[i].Index].pos);
			tempVector[Y] = 0.0f;
			bmVectorNorm(averageNormal,tempVector);
			averageNormal[W] = 1.0f;
		}

		ClipVector(corVel,corVel,averageNormal);

	}
}


/*	--------------------------------------------------------------------------------
	Function 	: ProcessSphereCollision
	Purpose 	: process sphere to sphere collision
	Parameters 	: ?
	Returns 	: number of sphere collisions occured
	Info 		: 
*/
int ProcessSphereCollision(TBVector pos,ACTOR_BEHAVIOUR *actorBehaviour, ACTORINSTANCE **sphereInstances, COLL_SPHERE *sphereList,int noofSphereLists, 
									 ACTORINSTANCE *actorInstance, TBVector stepvec, COLL_INFO *collinfo, COLL_INFO *retcollinfo, int *rsidx)
{
	COLL_SPHERE		sphere;
	int				i, sphereIndex;
	float			into;

	if (actorBehaviour->collSphere.radius == 0.0f) 
	{
		collinfo->sphere[0].Index = -1;	// end of list marker

		return 0;
	}
	
	bmVectorAdd(sphere.pos,pos,actorBehaviour->collSphere.offset);
	sphere.radius = actorBehaviour->collSphere.radius;
	sphere.pos[W] = 1.0f;
	sphereIndex = 0;

	for(i = 0; i < noofSphereLists; i++)
	{
		sphereList[i].pos[W] = 1.0f;
		// cylinder vs cylinder collision response
		into = SphereSphereCollision(&sphere,&sphereList[i]);
		if ( into > 0.0f )
		{
			if (
				(sphereInstances[i]->PreRespondToCollisionRoutine &&(!(sphereInstances[i]->PreRespondToCollisionRoutine(actorInstance, sphereInstances[i], NULL))))
				||(actorInstance->PreRespondToCollisionRoutine && (!(actorInstance->PreRespondToCollisionRoutine(sphereInstances[i],actorInstance, NULL))))
				)
			{
				if ((*rsidx)<MAX_COLL_SPHERES)
				{
					retcollinfo->sphere[*rsidx].Index = i;
					(*rsidx)++;
				}
			}
			else
			{
				TBVector	tempVector;

				bmVectorSub(tempVector, pos, sphereList[i].pos);
				SafeScaleToLength(tempVector, tempVector, into);
				bmVectorAdd(pos,pos,tempVector);

				collinfo->sphere[sphereIndex].Index = i;
				sphereIndex++;
				if ((*rsidx)<MAX_COLL_SPHERES)
				{
					retcollinfo->sphere[*rsidx].Index = i;
					(*rsidx)++;
				}
			}
		}
	}
	if ( sphereIndex < MAX_COLL_SPHERES )
		collinfo->sphere[sphereIndex].Index = -1;	// end of list marker

	return sphereIndex;
}

/* --------------------------------------------------------------------------------
   Function : InitActorBehaviour
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void InitActorBehaviour( ACTOR_BEHAVIOUR *actorBehaviour, float collRadius )
{
	int i;

	memset(actorBehaviour,0,sizeof(ACTOR_BEHAVIOUR));
	actorBehaviour->collSphere.radius = collRadius;
	for (i=0;i<MAX_ACTORCYLINDERS;i++)
	{
		actorBehaviour->collCylinder[i].radius = 0.0f;
		actorBehaviour->collCylinder[i].height = 0.0f;
		actorBehaviour->collCylinder[i].node = NULL;
	}
	actorBehaviour->stopCollidingWith = NULL;
	actorBehaviour->stopCollidingWithClock = 0.0f;
	actorBehaviour->stopCollidingWithTime = 0.0f; 
	actorBehaviour->physics.maxSpeed = actorBehaviour->collSphere.radius*60;
	// TP: very small cut of speed needed to stop nasty sliding
	actorBehaviour->physics.minVel = 0.001f;
	actorBehaviour->physics.minFricAngle = 1.0f;
	actorBehaviour->physics.inertia = 0;
	actorBehaviour->physics.flags = PHYSFLAG_FORCEMULTCOLL;
	actorBehaviour->physics.bounce = 0.0f;
	actorBehaviour->physics.friction = 0.0f;
	actorBehaviour->physics.currentTerrain |= TERRAIN_GROUND;
}

/* --------------------------------------------------------------------------------
   Function : SetActorMaxSpeed
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void SetActorMaxSpeed( ACTOR_BEHAVIOUR *actorBehaviour, float maxSpeed )
{
	actorBehaviour->physics.maxSpeed = maxSpeed;
/*	if ( maxSpeed > (actorBehaviour->collSphereRadius*60) )
		actorBehaviour->physics.flags |= PHYSFLAG_FORCEMULTCOLL;
	else
		actorBehaviour->physics.flags &= ~PHYSFLAG_FORCEMULTCOLL;*/
}

/* --------------------------------------------------------------------------------
   Function : SetActorMinVel
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void SetActorMinVel( ACTOR_BEHAVIOUR *actorBehaviour, float minVel )
{
	actorBehaviour->physics.minVel = minVel;
}

/* --------------------------------------------------------------------------------
   Function : SetActorHelium
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void SetActorHelium( ACTOR_BEHAVIOUR *actorBehaviour, float helium )
{
	actorBehaviour->physics.helium = helium;
}

/* --------------------------------------------------------------------------------
   Function : SetActorBounce
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void SetActorBounce( ACTOR_BEHAVIOUR *actorBehaviour, float bounce )
{
	actorBehaviour->physics.bounce = bounce;
}

/* --------------------------------------------------------------------------------
   Function : SetActorFriction
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void SetActorFriction( ACTOR_BEHAVIOUR *actorBehaviour, float friction )
{
	actorBehaviour->physics.friction = friction;
}

/* --------------------------------------------------------------------------------
   Function : SetActorMinFricAngle
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void SetActorMinFricAngle( ACTOR_BEHAVIOUR *actorBehaviour, float minFricAngle )
{
	actorBehaviour->physics.minFricAngle = minFricAngle;
}

/* --------------------------------------------------------------------------------
   Function : SetActorInertia
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void SetActorInertia( ACTOR_BEHAVIOUR *actorBehaviour, float inertia )
{
	actorBehaviour->physics.inertia = inertia;
}

/* --------------------------------------------------------------------------------
   Function : SetActorFlags
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void SetActorFlags( ACTOR_BEHAVIOUR *actorBehaviour, int flags )
{
	actorBehaviour->physics.flags = flags;
}

/* --------------------------------------------------------------------------------
   Function : ActorLookAt
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void ActorLookAt( ACTORINSTANCE *actorInstance, ACTORINSTANCE *destActorInstance,
				  float rotAngle, float rotSpeed )
{
	TBVector vec;

	bmVectorSub(vec,destActorInstance->actorInstance.position,actorInstance->actorInstance.position);
	bmVectorNorm(vec,vec);
	vec[3] = rotAngle;
	bmRotationToQuat(actorInstance->actorBehaviour->rotAim,vec);
	actorInstance->actorBehaviour->rotSpeed = rotSpeed;
}

/* --------------------------------------------------------------------------------
   Function : ActorRotate
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void ActorRotate(ACTORINSTANCE *actorInstance, TBQuaternion destdir, float rotSpeed )
{

	if(actorInstance->actorBehaviour)
	{
		bmVectorCopy(actorInstance->actorBehaviour->rotAim,destdir);
		actorInstance->actorBehaviour->rotSpeed = rotSpeed;
		if(rotSpeed == 0.0f)
		{
			bmQuatCopy(actorInstance->actorInstance.orientation,actorInstance->actorBehaviour->rotAim);
		}
	}
	else
	{
		bmQuatCopy(actorInstance->actorInstance.orientation, destdir);
	}
}

/* --------------------------------------------------------------------------------
   Function : SetActorVelocity
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void SetActorVelocity( ACTOR_BEHAVIOUR *actorBehaviour, TBVector newVel )
{
	bmVectorCopy(actorBehaviour->physics.velocity,newVel);
	actorBehaviour->physics.speed = bmVectorLen(actorBehaviour->physics.velocity);
}

/* --------------------------------------------------------------------------------
   Function : SetActorSpeed
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void SetActorSpeed( ACTOR_BEHAVIOUR *actorBehaviour, TBVector velDir, float newSpeed )
{
	bmVectorNorm(velDir,velDir);
	velDir[W] = 1.0f;

	bmVectorCMul(actorBehaviour->physics.velocity,velDir,newSpeed);
	actorBehaviour->physics.speed = bmVectorLen(actorBehaviour->physics.velocity);
}

/* --------------------------------------------------------------------------------
   Function : SetActorCollSphere
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void SetActorCollSphere(ACTOR_BEHAVIOUR *actorBehaviour, TBVector offset, float radius)
{
	bmVectorCopy(actorBehaviour->collSphere.offset,offset);
	actorBehaviour->collSphere.radius = radius;
}

/* --------------------------------------------------------------------------------
   Function : SetActorCylinder
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/
void SetActorCylinder(int cylinderNumber,ACTOR_BEHAVIOUR *actorBehaviour, TBVector offset, float radius,float height,int type)
{
	if (cylinderNumber>=MAX_ACTORCYLINDERS) return;
	bmVectorCopy(actorBehaviour->collCylinder[cylinderNumber].offset,offset);
	actorBehaviour->collCylinder[cylinderNumber].radius = radius;
	actorBehaviour->collCylinder[cylinderNumber].height = height;
	actorBehaviour->collCylinder[cylinderNumber].node = NULL;
	actorBehaviour->collCylinder[cylinderNumber].type = type;
}

/* --------------------------------------------------------------------------------
   Function : SetActorCylinder
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/
void SetActorCylinder(int cylinderNumber,TBActorNodeInstance *node,ACTOR_BEHAVIOUR *actorBehaviour, TBVector offset, float radius,float height, int type)
{
	if (cylinderNumber>=MAX_ACTORCYLINDERS) return;
	bmVectorCopy(actorBehaviour->collCylinder[cylinderNumber].offset,offset);
	actorBehaviour->collCylinder[cylinderNumber].radius = radius;
	actorBehaviour->collCylinder[cylinderNumber].height = height;
	actorBehaviour->collCylinder[cylinderNumber].node = node;
	actorBehaviour->collCylinder[cylinderNumber].type = type;
}

/* --------------------------------------------------------------------------------
   Function : AddActorVelocity
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void AddActorVelocity( ACTOR_BEHAVIOUR *actorBehaviour, TBVector vel )
{
	bmVectorAdd(actorBehaviour->physics.velocity,actorBehaviour->physics.velocity,vel);
	actorBehaviour->physics.speed = bmVectorLen(actorBehaviour->physics.velocity);
}

/* --------------------------------------------------------------------------------
   Function : AddActorSpeed
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void AddActorSpeed( ACTOR_BEHAVIOUR *actorBehaviour, TBVector velDir, float spd )
{
	TBVector vec;

	bmVectorNorm(velDir,velDir);
	velDir[W] = 1.0f;
	bmVectorCMul(vec,velDir,spd);
	bmVectorAdd(actorBehaviour->physics.velocity,actorBehaviour->physics.velocity,vec);
	actorBehaviour->physics.speed = bmVectorLen(actorBehaviour->physics.velocity);
}

/* --------------------------------------------------------------------------------
   Function : checkSpeedLimit
   Purpose : Check the speed against the max and min speed
   Parameters : 
   Returns : 
   Info : 
*/

void checkSpeedLimit( ACTOR_PHYSICS *physics, float worldMaxSpeed )
{
	float tvy = 0.0f;

	physics->speed = bmVectorLen(physics->velocity);

	if (physics->speed > worldMaxSpeed)
	{
		SafeScaleToLength(physics->velocity, physics->velocity, worldMaxSpeed);
		physics->speed = worldMaxSpeed;
	}
#ifdef SLIDE_FIX
	if (physics->speed < physics->minVel)
	{
		bmVectorZero(physics->velocity);
		physics->speed = 0.0f;
	}
#endif
}

/* --------------------------------------------------------------------------------
   Function : applyInertia
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void applyInertia( ACTOR_PHYSICS *physics )
{
	if ( physics->velocity[0] > 0.0f )			// x
	{
		if ( physics->velocity[0] < physics->minVel )
			physics->velocity[0] = 0.0f;
		else
		{
			physics->velocity[0] -= physics->inertia * physics->maxSpeed * speedRatio;
			if ( physics->velocity[0] < physics->minVel )
				physics->velocity[0] = 0.0f;
		}
	}
	else if ( physics->velocity[0] < 0.0f )
	{
		if ( physics->velocity[0] > -physics->minVel )
			physics->velocity[0] = 0.0f;
		else
		{
			physics->velocity[0] += physics->inertia * physics->maxSpeed * speedRatio;
			if ( physics->velocity[0] > -physics->minVel )
				physics->velocity[0] = 0.0f;
		}
	}
	if ( physics->velocity[2] > 0.0f )			// z
	{
		if ( physics->velocity[2] < physics->minVel )
			physics->velocity[2] = 0.0f;
		else
		{
			physics->velocity[2] -= physics->inertia * physics->maxSpeed * speedRatio;
			if ( physics->velocity[2] < physics->minVel )
				physics->velocity[2] = 0.0f;
		}
	}
	else if ( physics->velocity[2] < 0.0f )
	{
		if ( physics->velocity[2] > -physics->minVel )
			physics->velocity[2] = 0.0f;
		else
		{
			physics->velocity[2] += physics->inertia * physics->maxSpeed * speedRatio;
			if ( physics->velocity[2] > -physics->minVel )
				physics->velocity[2] = 0.0f;
		}
	}
}
