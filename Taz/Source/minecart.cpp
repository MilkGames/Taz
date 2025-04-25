// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : minecart.cpp
//   Purpose : minecart functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "actors.h"
#include "animation.h"
#include "status.h"
#include "physics.h"
#include "sfx.h"
#include "control.h"
#include "collisioncache.h"
#include "videofx.h"
#include "projectile.h"
#include "display.h"
#include "attach.h"
#include "vehicles.h"
#include "genericai.h"
#include "textures.h"
#include "sparks.h"
#include "util.h"
#include "glowaroundplinth.h"
#include "animation.h"
#include "prompt.h"

/* --------------------------------------------------------------------------------
   Function : CreateMineCart
   Purpose : Crate mine cart specific stuff
   Parameters : 
   Returns : 
   Info : 
*/

void CreateMineCart(ACTORINSTANCE *actorInstance, uint32 resetPointCRC)
{
	ACTORINSTANCE		*tempInstance;
	TBQuaternion		tempQuat;
	TBQuaternion		tempQuat2;
	TBVector			tempVec;
	TBActorAnimSegment	*segment;
	int					i, j;

	// CMD: note: the mine cart is the path object and NOT the actual mine cart
	actorInstance->mineCartInfo = (MINECARTINFO*)MALLOC(sizeof(MINECARTINFO));
	ASSERTM(actorInstance->mineCartInfo, "minecart specific info malloc failed");

	actorInstance->special = SPECIALTYPE_MINECART;

	actorInstance->mineCartInfo->flags = 0;
	actorInstance->mineCartInfo->landpointCRC = 0;
	actorInstance->mineCartInfo->resetPointCRC = resetPointCRC;

	actorInstance->mineCartInfo->speedFactor = 1.0f;
	actorInstance->mineCartInfo->tilt = 0.0f;
	actorInstance->mineCartInfo->animBlendInfo.currentBlend = 0.0f;
	actorInstance->mineCartInfo->animBlendInfo.progress = 0.0f;
	actorInstance->mineCartInfo->animBlendInfo.smooth = 1.0f;
	actorInstance->mineCartInfo->slowingDown = FALSE; 
	actorInstance->mineCartInfo->characterOnCart = NULL;
	actorInstance->flags |= ACTORFLAG_CUSTOMDRAW;

	actorInstance->mineCartInfo->soundNormal = -1;
	actorInstance->mineCartInfo->soundBoost = -1;
	actorInstance->mineCartInfo->soundSpark = -1;

	actorInstance->mineCartInfo->tiltClock = 0.0f;

	switch(map.sceneNumber)
	{
	case SCENE_GOLDMINE:
		actorInstance->mineCartInfo->wheelSparks = (MINECARTSPARKS*)ZALLOC(sizeof(MINECARTSPARKS)*4);
		ASSERTM(actorInstance->mineCartInfo->wheelSparks, "could not malloc minecart sparks");

		for(i = 3; i >= 0;--i)
		{
			for(j = (MINECART_NUMSPARKVERTS-1);j >= 0;--j)
			{
				BDVERTEX_SETRGBA(&actorInstance->mineCartInfo->wheelSparks[i].verts[j], 127, 50, 1, 70);
//				actorInstance->mineCartInfo->wheelSparks[i].colour[j][0] = 127;
//				actorInstance->mineCartInfo->wheelSparks[i].colour[j][1] = 50;
//				actorInstance->mineCartInfo->wheelSparks[i].colour[j][2] = 1;
//				actorInstance->mineCartInfo->wheelSparks[i].colour[j][3] = 70;

//				actorInstance->mineCartInfo->wheelSparks[i].size[j] = METERTOUNIT(0.05f);
			}
			actorInstance->mineCartInfo->wheelSparks[i].texture = bkLoadTexture(NULL, "extras\\voodoospell.tga", 0);
			actorInstance->mineCartInfo->wheelSparks[i].draw = FALSE;
		}
		bmVectorSet(actorInstance->mineCartInfo->wheelSparks[0].offset, METERTOUNIT(0.512f), 0.0f, METERTOUNIT(-0.8f), 1.0f);
		bmVectorSet(actorInstance->mineCartInfo->wheelSparks[1].offset, METERTOUNIT(0.512f), 0.0f, METERTOUNIT(-0.4f), 1.0f);
		bmVectorSet(actorInstance->mineCartInfo->wheelSparks[2].offset, METERTOUNIT(-0.712f), 0.0f, METERTOUNIT(-0.8f), 1.0f);
		bmVectorSet(actorInstance->mineCartInfo->wheelSparks[3].offset, METERTOUNIT(-0.712f), 0.0f, METERTOUNIT(-0.4f), 1.0f);
		break;
	default:
		actorInstance->mineCartInfo->wheelSparks = NULL;
		break;
	}

	// CMD: create dummy mine cart
/*	bmQuatCopy(tempQuat2, tempQuat);
	bmQuatMultiply(tempQuat, tempQuat2, actorInstance->actorInstance.orientation);*/


	CreateMineCartDummy(tempInstance = CreateActorInstance(&map.animatedInstances, "extras\\minecart.obe", "minecartdummy", 
		actorInstance->actorInstance.position,actorInstance->actorInstance.scale, actorInstance->actorInstance.orientation, 0));
	tempInstance->mineCartDummyInfo->minecartInstance = actorInstance;
	actorInstance->mineCartInfo->dummyInstance = tempInstance;

	bmVectorCopy(tempInstance->mineCartDummyInfo->posLastFrame, tempInstance->actorInstance.position);
	bmVectorSet(tempInstance->mineCartDummyInfo->velocity, 0.0f, 0.0f, 0.0f, 1.0f);
	
	FlushAnimationQueue(actorInstance->mineCartInfo->dummyInstance);
	PlayAnimationByName(actorInstance->mineCartInfo->dummyInstance, "drive", 1.0f,0,0,0.1f,IDLE);

	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance, "idle", 1.0f, 0, 0, 0.0f, IDLE);
}

/* --------------------------------------------------------------------------------
   Function : RespondToMineCartCollision
   Purpose : Crate mine cart specific stuff
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToMineCartCollision(ACTORINSTANCE *collider, ACTORINSTANCE *minecart)
{
	if (collider->genericAI) 
	{
		AICollideWithSpecial(collider,minecart);
		return TRUE;
	}

	if (collider->flags&ACTORFLAG_DEBRISACTOR) return TRUE;

	RequestStateChange(collider, STATE_ONMINECART);
	if(collider->actorStatus)
	{
		collider->actorStatus->actorAttachedTo = minecart;
	}

	//set orientation of collider to match trolley
	SetActorOrientation(collider, minecart->actorInstance.orientation);
	bmVectorCopy(collider->actorInstance.position, minecart->actorInstance.position);

	RemoveActorFromCollisionCache(&collisionCache, minecart);
	minecart->mineCartInfo->flags |= MINECART_ACTIVE;
	// CMD: zero colliders velocity
	SETVECTOR(collider->actorBehaviour->physics.velocity, 0.0f, 0.0f, 0.0f, 1.0f);
	// CMD: camera stuff
	gameStatus.chrisCam = FALSE;
	SetCameraCollision(gameStatus.player[collider->playerNo].camera, FALSE);
	SetCameraSmooth(gameStatus.player[collider->playerNo].camera, 0.0f, 0.0f);

	collider->actorBehaviour->physics.flags |= PHYSFLAG_NOGRAVITY;

	minecart->mineCartInfo->tiltRebound = 0.0f;
	minecart->mineCartInfo->tilt = 0.0f;
	minecart->mineCartInfo->tiltTarget = 0.0f;
	minecart->mineCartInfo->strength = 1.0f;
	minecart->mineCartInfo->balancePoint = 0.0f;
	minecart->mineCartInfo->strengthClock = MINECART_STRENGTHTIME;
	minecart->mineCartInfo->boostRemaining = 1.0f;
	minecart->mineCartInfo->boostAnimState = MINECART_NOBOOST;
	minecart->mineCartInfo->cameraPosSmooth = gameStatus.player[collider->playerNo].camera->posSmooth;
	minecart->mineCartInfo->cameraLookAtSmooth = gameStatus.player[collider->playerNo].camera->lookAtSmooth;
	SETVECTOR(minecart->mineCartInfo->cameraPosOffset,  0.0f, METERTOUNIT(2.5f), METERTOUNIT(3.5f), 1.0f);
	SETVECTOR(minecart->mineCartInfo->cameraLookAtOffset, 0.0f, 0.0f, METERTOUNIT(-10.0f), 1.0f);

	minecart->mineCartInfo->soundNormal = PlaySample("loop\\rcoasterlp2.wav", 255);

	FlushAnimationQueue(minecart);
	switch(map.sceneNumber)
	{
	case SCENE_GOLDMINE:
		PlayAnimationByName(minecart, "seg1", 1.0f,0,0,0.1f,NONE);
		break;
	case SCENE_GHOSTTOWN:
		PlayAnimationByName(minecart, "move", 1.0f, 0, 0, 0.1f, MOVE);
		break;
	case SCENE_WILDWESTHUB:
		PlayAnimationByName(minecart, "seg1", 1.0f, 0, 0, 0.1f, MOVE);
		break;
	}

	FlushAnimationQueue(minecart->mineCartInfo->dummyInstance);
	PlayAnimationByName(minecart->mineCartInfo->dummyInstance, "drive", 1.0f, 0, 0, 0.1f,MOVE);

	return TRUE;						//requre physics to be processed
}

/* --------------------------------------------------------------------------------
   Function : RespondToMineCartCollision
   Purpose : Crate mine cart specific stuff
   Parameters : 
   Returns : 
   Info : 
*/

static void LaunchAtEndOfMineCartCallback(ACTORINSTANCE *actorInstance)
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
   Function : RespondToMineCartCollision
   Purpose : Crate mine cart specific stuff
   Parameters : 
   Returns : 
   Info : 
*/
//#define NO_MINECART_OBSTACLES

void UpdateMineCart(ACTORINSTANCE *actorInstance)
{
	MINECARTINFO		*ptr = actorInstance->mineCartInfo;
	MINECARTDUMMYINFO	*dummyInfo = ptr->dummyInstance->mineCartDummyInfo;
	ACTORINSTANCE		*landingPoint;
	TBActorNodeInstance	*node;
	TBVector			position, velocity;
	TBQuaternion		quat, tempQuat;
	TBMatrix			matrix;
	TBActorAnimSegment	*path;

	node = actorInstance->actorInstance.rootNodeInstance;

	if(ptr->flags & MINECART_ACTIVE)
	{
		switch(map.sceneNumber)
		{
		case SCENE_GOLDMINE:
			// CMD: calculate minecart dummy velocity
			bmVectorSub(dummyInfo->velocity, ptr->dummyInstance->actorInstance.position, dummyInfo->posLastFrame);
			bmVectorCMul(dummyInfo->velocity, dummyInfo->velocity, fps);
			bmVectorCopy(dummyInfo->posLastFrame, ptr->dummyInstance->actorInstance.position);

			UpdateMinecartSparks(actorInstance);

#ifndef NO_MINECART_OBSTACLES
			if(ptr->flags & MINECART_TUMBLE)
			{
				if(~ptr->flags & MINECART_TRAGICSOUND)
				{
					TragicMinecartEventSounds(ptr, "minecartcrash.wav");
				}
				if(TumbleMineCart(actorInstance))
				{
					// CMD: tumble finished, set fade and return taz to start of level
					ResetMinecartAfterResetEvent(actorInstance);
				}
				return;
			}
			if(ptr->flags & MINECART_DROP)
			{
				if(~ptr->flags & MINECART_TRAGICSOUND)
				{
					TragicMinecartEventSounds(ptr, "bigfall.wav");
				}
				if(DropMineCart(actorInstance))
				{
					// CMD: drop finished, set fade and return
					ResetMinecartAfterResetEvent(actorInstance);
				}
				return;
			}
			if(ptr->flags & MINECART_DROPLEFT)
			{
				if(~ptr->flags & MINECART_TRAGICSOUND)
				{
					TragicMinecartEventSounds(ptr, "bigfall.wav");
				}
				if(DoMissingRailStuff(actorInstance))
				{
					// CMD: drop finished, set fade and return
					ResetMinecartAfterResetEvent(actorInstance);
				}
				return;
			}
			if(ptr->flags & MINECART_DROPRIGHT)
			{
				if(~ptr->flags & MINECART_TRAGICSOUND)
				{
					TragicMinecartEventSounds(ptr, "bigfall.wav");
				}
				if(DoMissingRailStuff(actorInstance))
				{
					// CMD: drop finished, set fade and return
					ResetMinecartAfterResetEvent(actorInstance);
				}
				return;
			}
			if(ptr->flags & MINECART_LEFTMISSING)
			{
				if(ptr->tilt < 0.3f)
				{
					StartMineCartMissingRailAction(actorInstance, MINECART_PATH_LEFT);
					ptr->flags |= MINECART_DROPLEFT;
					ptr->flags &= ~MINECART_LEFTMISSING;
					return;
				}
			}
			if(ptr->flags & MINECART_RIGHTMISSING)
			{
				if(ptr->tilt > -0.3f)
				{
					StartMineCartMissingRailAction(actorInstance, MINECART_PATH_RIGHT);
					ptr->flags |= MINECART_DROPRIGHT;
					ptr->flags &= ~MINECART_RIGHTMISSING;
					return;
				}
			}
			if(ptr->flags & MINECART_LEFTOBSTACLE)
			{
				if(ptr->tilt < 0.3f)
				{
					StartMineCartTumble(actorInstance);
					ptr->flags |= MINECART_TUMBLE;
					ptr->flags &= ~MINECART_LEFTOBSTACLE;
					return;
				}
			}
			if(ptr->flags & MINECART_RIGHTOBSTACLE)
			{
				if(ptr->tilt > -0.3f)
				{
					StartMineCartTumble(actorInstance);
					ptr->flags |= MINECART_TUMBLE;
					ptr->flags &= ~MINECART_RIGHTOBSTACLE;
					return;
				}
			}
			if(ptr->flags & MINECART_ONFIRE)
			{
				if(~ptr->flags & MINECART_TRAGICSOUND)
				{
					TragicMinecartEventSounds(ptr, NULL);
				}
				if(MineCartOnFire(actorInstance))
				{
					// CMD: fire finished, set fade and return taz to start of level
					ResetMinecartAfterResetEvent(actorInstance);
				}
				return;
			}
#endif

			//check to see if segment has finished
			if(!CheckIfPlayingAnimation(actorInstance, NULL))
			{
				if(~ptr->flags & MINECART_CESTTOUT)
				{
					// CMD: check for tilt and play appropriate animation dependant on the default path
					switch(ptr->defaultPath)
					{
					case MINECART_PATH_LEFT:
						if(ptr->tilt > 0.5f)
						{
							if(ptr->middle) path = ptr->middle;
							else path = ptr->right;
						}
						else path = ptr->left;
						break;
					case MINECART_PATH_MIDDLE:
						if(ptr->tilt < -0.5f)
						{
							// CMD: want to go left
							if(ptr->left) path = ptr->left;
							else path = ptr->middle;
						}
						else
							if(ptr->tilt > 0.5f)
							{
								// CMD: want to go right
								if(ptr->right) path = ptr->right;
								else path = ptr->middle;
							}
							else path = ptr->middle;
						break;
					case MINECART_PATH_RIGHT:
						if(ptr->tilt < -0.5f)
						{
							if(ptr->middle) path = ptr->middle;
							else path = ptr->left;
						}
						else path = ptr->right;
						break;
					}
					FlushAnimationQueue(actorInstance);
					PlayAnimationBySegmentPointer(actorInstance, path, ptr->speedFactor, 0, 0, 0.1f, REACT);
				}
				else
				{
					ptr->flags &= ~MINECART_ACTIVE;

//					ptr->flags &= ~MINECART_CESTTOUT;

					//throw taz from cart, and calculate velocity to do so
					landingPoint = FindActorInstanceInInstanceList(0,actorInstance->mineCartInfo->landpointCRC,&map.invisibleInstances);
					if(0) //landingPoint)
					{
						CalculateProjectileInitialVelocity(velocity, ptr->characterOnCart->actorInstance.position, landingPoint->actorInstance.position, RAD(45.0f));
					}
					else
					{
						SETVECTOR(position, 0.0f, 0.0f, METERTOUNIT(-5.0f), 1.0f);
						bmQuatToMatrix(matrix, ptr->dummyInstance->actorInstance.orientation);
						bmMatMultiplyVector(matrix, position);
						bmVectorAdd(position, position, ptr->dummyInstance->actorInstance.position);

						CalculateProjectileInitialVelocity(velocity, ptr->characterOnCart->actorInstance.position, position, RAD(45.0f));
					}

					RequestStateChangePlus(ptr->characterOnCart,STATE_PROJECTILE,LaunchAtEndOfMineCartCallback);
					SetActorVelocity(ptr->characterOnCart->actorBehaviour, velocity);
					ptr->characterOnCart->actorBehaviour->physics.flags &= ~PHYSFLAG_NOGRAVITY;
					ptr->characterOnCart->actorStatus->actorAttachedTo = NULL;
					ptr->speedFactor = 1.0f;
					VFX_motionBlur(0.0f);

					SetCameraCollision(gameStatus.player[ptr->characterOnCart->playerNo].camera, TRUE);
					SetCameraSmooth(gameStatus.player[ptr->characterOnCart->playerNo].camera,0.06f, 0.15f);

					AddActorToCollisionCache(&collisionCache, ptr->dummyInstance, 0);
					StopCollidingWith(ptr->characterOnCart, ptr->dummyInstance, 1.0f);

					StopChannel(&ptr->soundBoost);
					StopChannel(&ptr->soundNormal);
					StopChannel(&ptr->soundSpark);

					// CMD: check what state the minecart is in and play appropriate anims
					switch(ptr->boostAnimState)
					{
					case MINECART_READYBOOST:
						FlushAnimationQueue(ptr->dummyInstance, 1);
						PlayAnimationByName(ptr->dummyInstance, "drivefast_rockets_in", 1.0f, 0, 1, 0.1f, REACT);
						break;
					case MINECART_BOOST:
						FlushAnimationQueue(ptr->dummyInstance);
						PlayAnimationByName(ptr->dummyInstance, "drivefast_rockets_in", 1.0f, 0, 0, 0.1f, REACT);
						break;
					case MINECART_EXITBOOST:
						break;
					case MINECART_NOBOOST:
						break;
					case MINECART_JUMP:
						break;
					case MINECART_BRAKE:
						break;
					}
					return;
				}
			}
			break;
		case SCENE_GHOSTTOWN:
			if(ptr->flags & MINECART_TUMBLE)
			{
				if(~ptr->flags & MINECART_TRAGICSOUND)
				{
					TragicMinecartEventSounds(ptr, "minecartcrash.wav");
				}
				if(TumbleMineCart(actorInstance))
				{
					// CMD: tumble finished, set fade and return taz to start of level
					ResetMinecartAfterResetEvent(actorInstance);
				}
				return;
			}
			if(ptr->flags & MINECART_LEFTOBSTACLE)
			{
				if(ptr->tilt < 0.3f)
				{
					StartMineCartTumble(actorInstance);
					ptr->flags |= MINECART_TUMBLE;
					ptr->flags &= ~MINECART_LEFTOBSTACLE;
					return;
				}
			}
			if(ptr->flags & MINECART_RIGHTOBSTACLE)
			{
				if(ptr->tilt > -0.3f)
				{
					StartMineCartTumble(actorInstance);
					ptr->flags |= MINECART_TUMBLE;
					ptr->flags &= ~MINECART_RIGHTOBSTACLE;
					return;
				}
			}
			if(ptr->flags & MINECART_ONFIRE)
			{
				if(~ptr->flags & MINECART_TRAGICSOUND)
				{
					TragicMinecartEventSounds(ptr, NULL);
				}
				if(MineCartOnFire(actorInstance))
				{
					// CMD: fire finished, set fade and return taz to start of level
					ResetMinecartAfterResetEvent(actorInstance);
				}
				return;
			}
			if(!CheckIfPlayingAnimation(actorInstance, NULL))
			{
				ptr->flags &= ~MINECART_ACTIVE;

				SETVECTOR(position, 0.0f, 0.0f, METERTOUNIT(-5.0f), 1.0f);
				bmQuatToMatrix(matrix, ptr->dummyInstance->actorInstance.orientation);
				bmMatMultiplyVector(matrix, position);
				bmVectorAdd(position, position, ptr->dummyInstance->actorInstance.position);

				CalculateProjectileInitialVelocity(velocity, ptr->characterOnCart->actorInstance.position, position, RAD(45.0f));

				RequestStateChangePlus(ptr->characterOnCart,STATE_PROJECTILE,LaunchAtEndOfMineCartCallback);
				SetActorVelocity(ptr->characterOnCart->actorBehaviour, velocity);
				ptr->characterOnCart->actorBehaviour->physics.flags &= ~PHYSFLAG_NOGRAVITY;
				ptr->characterOnCart->actorStatus->actorAttachedTo = NULL;

				StopChannel(&ptr->soundBoost);
				StopChannel(&ptr->soundNormal);
				StopChannel(&ptr->soundSpark);

				AddActorToCollisionCache(&collisionCache, ptr->dummyInstance, 0);
				StopCollidingWith(ptr->characterOnCart, ptr->dummyInstance);
				ptr->speedFactor = 1.0f;
				VFX_motionBlur(0.0f);
			}
			break;
		case SCENE_WILDWESTHUB:
			if(!CheckIfPlayingAnimation(actorInstance, NULL))
			{
				ptr->flags &= ~MINECART_ACTIVE;

				//throw taz from cart, and calculate velocity to do so
				landingPoint = FindActorInstanceInInstanceList(0,actorInstance->mineCartInfo->landpointCRC,&map.invisibleInstances);
				if(landingPoint)
				{
					CalculateProjectileInitialVelocity(velocity, ptr->characterOnCart->actorInstance.position, landingPoint->actorInstance.position, RAD(45.0f));
				}
				else
				{
					SETVECTOR(position, 0.0f, 0.0f, METERTOUNIT(5.0f), 1.0f);
					bmQuatToMatrix(matrix, ptr->dummyInstance->actorInstance.orientation);
					bmMatMultiplyVector(matrix, position);
					bmVectorAdd(position, position, ptr->dummyInstance->actorInstance.position);

					CalculateProjectileInitialVelocity(velocity, ptr->characterOnCart->actorInstance.position, position, RAD(45.0f));
				}

				StopChannel(&ptr->soundBoost);
				StopChannel(&ptr->soundNormal);
				StopChannel(&ptr->soundSpark);

				RequestStateChangePlus(ptr->characterOnCart,STATE_PROJECTILE,LaunchAtEndOfMineCartCallback);
				SetActorVelocity(ptr->characterOnCart->actorBehaviour, velocity);
				ptr->characterOnCart->actorBehaviour->physics.flags &= ~PHYSFLAG_NOGRAVITY;
				ptr->characterOnCart->actorStatus->actorAttachedTo = NULL;

				AddActorToCollisionCache(&collisionCache, ptr->dummyInstance, 0);
				StopCollidingWith(ptr->characterOnCart, ptr->dummyInstance, 1.0f);
				ptr->speedFactor = 1.0f;
				VFX_motionBlur(0.0f);
			}
			break;
		}
		//update taz'z position
		if(node)
		{
			switch(map.sceneNumber)
			{
			case SCENE_GOLDMINE:
				//update Taz
				baGetNodesWorldPosition(&actorInstance->actorInstance, actorInstance->actorInstance.rootNodeInstance, position);
				bmVectorCopy(ptr->characterOnCart->actorInstance.position, position);
	
				bmQuatYRotation(tempQuat, RAD(180.0f));
				bmQuatMultiply(quat, tempQuat, node->animState.orientation);
				SetActorOrientation(ptr->characterOnCart, quat);
				//update minecart object
	
				bmVectorCopy(actorInstance->mineCartInfo->dummyInstance->actorInstance.position, position);
				SetActorOrientation(actorInstance->mineCartInfo->dummyInstance, quat);
				break;
			case SCENE_GHOSTTOWN:
				//update Taz
				baGetNodesWorldPosition(&actorInstance->actorInstance, actorInstance->actorInstance.rootNodeInstance, position);
				bmVectorCopy(ptr->characterOnCart->actorInstance.position, position);
	
				SetActorOrientation(ptr->characterOnCart, node->animState.orientation);
				//update minecart object
	
				bmVectorCopy(actorInstance->mineCartInfo->dummyInstance->actorInstance.position, position);
				SetActorOrientation(actorInstance->mineCartInfo->dummyInstance, node->animState.orientation);
				break;
			case SCENE_WILDWESTHUB:
				//update Taz
				baGetNodesWorldPosition(&actorInstance->actorInstance, actorInstance->actorInstance.rootNodeInstance, position);
				bmVectorCopy(ptr->characterOnCart->actorInstance.position, position);
	
				SetActorOrientation(ptr->characterOnCart, node->animState.orientation);
				//update minecart object
	
				bmVectorCopy(actorInstance->mineCartInfo->dummyInstance->actorInstance.position, position);
				SetActorOrientation(actorInstance->mineCartInfo->dummyInstance, node->animState.orientation);
				break;
			}
		}
		AnimateMineCart(actorInstance);
//		UpdateMineCartAnimation(actorInstance);
	}
	else
	if(ptr->flags & MINECART_RETURN)
	{
		//update Taz
		baGetNodesWorldPosition(&actorInstance->actorInstance, actorInstance->actorInstance.rootNodeInstance, position);

		bmQuatYRotation(tempQuat, RAD(90.0f));
		bmQuatMultiply(quat, tempQuat, node->animState.orientation);

		bmVectorCopy(actorInstance->mineCartInfo->dummyInstance->actorInstance.position, position);
		SetActorOrientation(actorInstance->mineCartInfo->dummyInstance, quat);
		AnimateMineCart(actorInstance);
		
		//check to see if return complete
		if(!CheckIfPlayingAnimation(actorInstance, "return"))
		{
			ptr->flags &= ~MINECART_RETURN;
			AddActorToCollisionCache(&collisionCache, actorInstance->mineCartInfo->dummyInstance, 0);
			return;
		}
	}
	else
	if(ptr->flags & MINECART_MOVETOPOSITION)
	{
		baGetNodesWorldPosition(&actorInstance->actorInstance, node, position);

		SetActorPosition(ptr->dummyInstance, position);
		SetActorOrientation(ptr->dummyInstance, node->animState.orientation);

		ptr->speedFactor = 1.0f;
		AnimateMineCart(actorInstance);
		if(!CheckIfPlayingAnimation(actorInstance, "pull_up"))
		{
			ptr->flags &= ~MINECART_MOVETOPOSITION;
			// CMD: maybe play some idle anim here
			RemoveActorFromCollisionCache(&collisionCache, ptr->dummyInstance);
			AddActorToCollisionCache(&collisionCache, ptr->dummyInstance, 0);
		}
	}
	else
	if(~ptr->flags & MINECART_DONOTHING)
	{
		// CMD: idle
		switch(map.sceneNumber)
		{
		case SCENE_GOLDMINE:
			bmQuatYRotation(tempQuat, RAD(180.0f));
			bmQuatMultiply(quat, tempQuat, node->animState.orientation);
			break;
		default:
			bmQuatCopy(quat, node->animState.orientation);
			break;
		}
		baGetNodesWorldPosition(&actorInstance->actorInstance, node, position);

		SetActorPosition(ptr->dummyInstance, position);
		SetActorOrientation(ptr->dummyInstance, quat);

		ptr->speedFactor = 1.0f;
		AnimateMineCart(actorInstance);
	}
}

/*  --------------------------------------------------------------------------------
	Function	: AnimateMineCart
	Purpose		: Update the animation for an actorInstance
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
void AnimateMineCart(ACTORINSTANCE *actorInstance)
{
	TBVector		motion;
	int				nodesUpdated;

	if(actorInstance->mineCartInfo->flags & MINECART_ACTIVE)
	{
		VFX_motionBlur(0.25f*(actorInstance->mineCartInfo->speedFactor-1.0f)/(MINECART_MEXSPEEDFACTOR-1.0f));
	}
	nodesUpdated = baUpdateNodes(&actorInstance->actorInstance, actorInstance->actorInstance.rootNodeInstance, 1.25f*speedRatio*actorInstance->mineCartInfo->speedFactor, motion);

	if (nodesUpdated) actorInstance->flags|=ACTORFLAG_HASANIMATED;
	else actorInstance->flags&=~ACTORFLAG_HASANIMATED;
}

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToMineCartInput
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/

void CharacterRespondToMineCartInput(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *tazInstance)
{
	ACTORINSTANCE	*path = tazInstance->actorStatus->actorAttachedTo;
	MINECARTINFO	*miney = path->mineCartInfo;
	ACTORINSTANCE	*dummyCart;
	TBVector		input;
	float			tempFloat;
	int				boostAllowed, transition;

	if(path)
	{
		dummyCart = path->mineCartInfo->dummyInstance;

		input[0] = controller->x1;
		input[1] = -controller->y1;

		//check for left/right input on analogue stick
		//check to see if max tilt time has been reached

		// CMD: separate input and treat left/right seperately, will be a lot easier
		// CMD: two forces act on the mine cart, bound force is force applied by player which decays linearly
		// CMD: from an initial high
		// CMD: rebound force is lightly damped force which returns cart to central position

		// CMD: calculate bound force
		path->mineCartInfo->tiltBound = input[0]*path->mineCartInfo->strength;

		// CMD: calculate rebound force

		// CMD: decrement strength
		if(bmFAbs(input[0]) > 0.2f)
		{
			if((path->mineCartInfo->strengthClock -= fTime) < 0.0f)
			{
				path->mineCartInfo->strengthClock = MINECART_STRENGTHTIME;
				path->mineCartInfo->strength = 0.0f;
			}
			else
			{
				if((path->mineCartInfo->strength -= MINECART_LOSESTRENGTH*fTime*(bmFAbs(input[0])-0.2f)) < 0.0f)
				{
					path->mineCartInfo->strength = 0.0f;
				}
			}
		}
		else
		{
			path->mineCartInfo->strengthClock = MINECART_STRENGTHTIME;
			if((path->mineCartInfo->strength += MINECART_GAINSTRENGTH*fTime) > 1.0f)
			{
				path->mineCartInfo->strength = 1.0f;
			}
		}

		// CMD: calculate rebound force

		// CMD: calculate tilt
		path->mineCartInfo->tiltTarget = path->mineCartInfo->tiltBound + 
						(path->mineCartInfo->tiltRebound*(0.5f+(miney->speedFactor/(MINECART_MEXSPEEDFACTOR*2.0f))));
		// CMD: apply some smoothing
		if(path->mineCartInfo->tilt > path->mineCartInfo->tiltTarget)
		{
			if((path->mineCartInfo->tilt -= MINECART_SMOOTHRATE*fTime) < path->mineCartInfo->tiltTarget)
			{
				path->mineCartInfo->tilt = path->mineCartInfo->tiltTarget;
			}
		}
		else
		if(path->mineCartInfo->tilt < path->mineCartInfo->tiltTarget)
		{
			if((path->mineCartInfo->tilt += MINECART_SMOOTHRATE*fTime) > path->mineCartInfo->tiltTarget)
			{
				path->mineCartInfo->tilt = path->mineCartInfo->tiltTarget;
			}
		}
		else
		{
			path->mineCartInfo->tilt = path->mineCartInfo->tiltTarget;
		}
		//update animation
		//is the cart tilting?
		// CMD: how about some sound for tilting
		if((path->mineCartInfo->tilt > 0.5f)||(path->mineCartInfo->tilt < -0.5f))
		{
			if(path->mineCartInfo->soundSpark == -1)
			{
				path->mineCartInfo->soundSpark = PlaySample("loop\\spark.wav", 200);
			}
		}
		else
		{
			if(path->mineCartInfo->boostAnimState != MINECART_BRAKE)
			{
				StopChannel(&path->mineCartInfo->soundSpark);
			}
		}

		switch(path->mineCartInfo->boostAnimState)
		{
		case MINECART_READYBOOST:
			if(path->mineCartInfo->tilt > 0.0f)
			{
				transition = PlayFixedBlendOfTwoAnimations(&dummyCart->actorInstance, "turboboost", "drivefastrighttilt", &path->mineCartInfo->animBlendInfo, path->mineCartInfo->tilt, ANIMFLAG_UPDATE, 4.0f);
				PlayFixedBlendOfTwoAnimations(&tazInstance->actorInstance, "minecartboosterstart", "minecartrightboosterstart", &path->mineCartInfo->animBlendInfo, path->mineCartInfo->tilt, 0, 4.0f);
			}
			else
			{
				transition = PlayFixedBlendOfTwoAnimations(&dummyCart->actorInstance, "turboboost", "drivefastlefttilt", &path->mineCartInfo->animBlendInfo, bmFAbs(path->mineCartInfo->tilt), ANIMFLAG_UPDATE, 4.0f);
				PlayFixedBlendOfTwoAnimations(&tazInstance->actorInstance, "minecartboosterstart", "minecartleftboosterstart", &path->mineCartInfo->animBlendInfo, bmFAbs(path->mineCartInfo->tilt), 0, 4.0f);
			}

			if(transition)
			{
				if(controller->circleChannel->value) 
				{
					path->mineCartInfo->boostAnimState = MINECART_BOOST;
				}
				else path->mineCartInfo->boostAnimState = MINECART_EXITBOOST;
			}
			break;
		case MINECART_BOOST:
			if(controller->circleChannel->value)
			{
				if((path->mineCartInfo->boostRemaining -= fTime*0.5f) < 0.0f)
				{
					path->mineCartInfo->boostRemaining = 0.0f;
					miney->boostAnimState = MINECART_EXITBOOST;
					break;
				}
				else
				{
					if((path->mineCartInfo->speedFactor += 6.0f*fTime) > MINECART_MEXSPEEDFACTOR)
					{
						path->mineCartInfo->speedFactor = MINECART_MEXSPEEDFACTOR;
					}
				}
			}
			else
			{
				miney->boostAnimState = MINECART_EXITBOOST;
				break;
			}

			if(path->mineCartInfo->tilt > 0.0f)
			{
				PlayFixedBlendOfTwoAnimations(&dummyCart->actorInstance, "drivefast_idle", "right_tilt_idle", &path->mineCartInfo->animBlendInfo, path->mineCartInfo->tilt, ANIMFLAG_UPDATE|ANIMFLAG_LOOP, path->mineCartInfo->speedFactor);
				PlayFixedBlendOfTwoAnimations(&tazInstance->actorInstance, "minecartdrivefast", "minecartfasttiltright", &path->mineCartInfo->animBlendInfo, path->mineCartInfo->tilt, ANIMFLAG_LOOP, path->mineCartInfo->speedFactor);
			}
			else
			{
				PlayFixedBlendOfTwoAnimations(&dummyCart->actorInstance, "drivefast_idle", "left_tilt_idle", &path->mineCartInfo->animBlendInfo, bmFAbs(path->mineCartInfo->tilt), ANIMFLAG_UPDATE|ANIMFLAG_LOOP, path->mineCartInfo->speedFactor);
				PlayFixedBlendOfTwoAnimations(&tazInstance->actorInstance, "minecartdrivefast", "minecartfasttiltleft", &path->mineCartInfo->animBlendInfo, bmFAbs(path->mineCartInfo->tilt), ANIMFLAG_LOOP, path->mineCartInfo->speedFactor);
			}
			break;
		case MINECART_EXITBOOST:
			if(path->mineCartInfo->tilt > 0.0f)
			{
				transition = PlayFixedBlendOfTwoAnimations(&dummyCart->actorInstance, "drivefast_rockets_in", "right_tilt_rockets_in", &path->mineCartInfo->animBlendInfo, path->mineCartInfo->tilt, ANIMFLAG_UPDATE, path->mineCartInfo->speedFactor);
				PlayFixedBlendOfTwoAnimations(&tazInstance->actorInstance, "minecartboosterstop", "minecartrighttiltloserockets", &path->mineCartInfo->animBlendInfo, path->mineCartInfo->tilt, 0, path->mineCartInfo->speedFactor);
			}
			else
			{
				transition = PlayFixedBlendOfTwoAnimations(&dummyCart->actorInstance, "drivefast_rockets_in", "left_tilt_rockets_in", &path->mineCartInfo->animBlendInfo, bmFAbs(path->mineCartInfo->tilt), ANIMFLAG_UPDATE, path->mineCartInfo->speedFactor);
				PlayFixedBlendOfTwoAnimations(&tazInstance->actorInstance, "minecartboosterstop", "minecartlefttiltloserockets", &path->mineCartInfo->animBlendInfo, bmFAbs(path->mineCartInfo->tilt), 0, path->mineCartInfo->speedFactor);
			}

			if(transition)
			{
				if(controller->circleChannel->value)
				{
					if(miney->boostRemaining >= 1.0f) path->mineCartInfo->boostAnimState = MINECART_READYBOOST;
					else
					{
						StopChannel(&path->mineCartInfo->soundBoost);
						path->mineCartInfo->soundNormal = PlaySample("loop\\rcoasterlp2.wav", 255);
						path->mineCartInfo->boostAnimState = MINECART_NOBOOST;
					}
				}
				else 
				{
					StopChannel(&path->mineCartInfo->soundBoost);
					path->mineCartInfo->soundNormal = PlaySample("loop\\rcoasterlp2.wav", 255);
					path->mineCartInfo->boostAnimState = MINECART_NOBOOST;
				}
			}
			break;
		case MINECART_NOBOOST:
			// CMD: refill boost fuel tanks
			if((path->mineCartInfo->boostRemaining += fTime/MINECART_MAXBOOSTTIME) > 1.0f)
			{
				path->mineCartInfo->boostRemaining = 1.0f;
			}
			if((path->mineCartInfo->speedFactor -= 0.5f*fTime) < 1.0f)
			{
				path->mineCartInfo->speedFactor = 1.0f;
			}

			// CMD: look for jump, which is only allowed from this state;
			if(controller->crossDebounceChannel->value)
			{
				if(bmFAbs(miney->tilt) < 0.2f)
				{
					FlushAnimationQueue(dummyCart);
					PlayAnimationByName(dummyCart, "jumprise", 1.0f, 0, 0, 0.1f, REACT);
					PlayAnimationByName(dummyCart, "jumpland", 1.0f, 0, 1, 0.1f, REACT);

					FlushAnimationQueue(tazInstance);
					PlayAnimationByName(tazInstance, "minecartjumprise", 1.0f, 0, 0, 0.1f, REACT);
					PlayAnimationByName(tazInstance, "minecartland", 1.0f, 0, 1, 0.1f, REACT);

					StopChannel(&path->mineCartInfo->soundNormal);
					path->mineCartInfo->soundNormal = PlaySample("loop\\rcoasterlp2.wav", 50);
					PlaySample("tazjump.wav", 255);

					miney->boostAnimState = MINECART_JUMP;
					break;
				}
			}
			// CMD: can only brake from this state also
			if(controller->y1 > 0.8f)
			{
				// CMD: can only brake if not tilting
				if(bmFAbs(miney->tilt) < 0.2f)
				{
					FlushAnimationQueue(dummyCart);
					PlayAnimationByName(dummyCart, "slowdown", 1.0f, 1, 0, 0.1f, REACT);

					FlushAnimationQueue(tazInstance);
					PlayAnimationByName(tazInstance, "minecartslowdown", 1.0f, 1, 0, 0.1f, REACT);

					StopChannel(&path->mineCartInfo->soundNormal);
					StopChannel(&path->mineCartInfo->soundBoost);
					path->mineCartInfo->soundSpark = PlaySample("loop\\spark.wav", 200);

					miney->boostAnimState = MINECART_BRAKE;
					break;
				}
			}
			if(controller->circleChannel->value)
			{
				// CMD: can only boost if boost remaing has fully reset
				// CMD: avoids problems of burst of boost at high speed
				if(miney->boostRemaining == 1.0f)
				{
					StopChannel(&path->mineCartInfo->soundNormal);
					path->mineCartInfo->soundBoost = PlaySample("loop\\rcoasterlp.wav", 255);
					path->mineCartInfo->boostAnimState = MINECART_READYBOOST;
					break;
				}
			}
			
			if(path->mineCartInfo->tilt > 0.0f)
			{
				PlayFixedBlendOfTwoAnimations(&dummyCart->actorInstance, "drive", "tiltright", &path->mineCartInfo->animBlendInfo, path->mineCartInfo->tilt, ANIMFLAG_UPDATE|ANIMFLAG_LOOP, path->mineCartInfo->speedFactor);
				PlayFixedBlendOfTwoAnimations(&tazInstance->actorInstance, "minecartdrive", "minecarttiltright", &path->mineCartInfo->animBlendInfo, path->mineCartInfo->tilt, ANIMFLAG_LOOP, path->mineCartInfo->speedFactor);
			}
			else
			{
				PlayFixedBlendOfTwoAnimations(&dummyCart->actorInstance, "drive", "tiltleft", &path->mineCartInfo->animBlendInfo, bmFAbs(path->mineCartInfo->tilt), ANIMFLAG_UPDATE|ANIMFLAG_LOOP, path->mineCartInfo->speedFactor);
				PlayFixedBlendOfTwoAnimations(&tazInstance->actorInstance, "minecartdrive", "minecarttiltleft", &path->mineCartInfo->animBlendInfo, bmFAbs(path->mineCartInfo->tilt), ANIMFLAG_LOOP, path->mineCartInfo->speedFactor);
			}
			break;
		case MINECART_JUMP:
			if(!CheckIfPlayingAnimation(dummyCart, NULL))
			{
				StopChannel(&path->mineCartInfo->soundNormal);
				path->mineCartInfo->soundNormal = PlaySample("loop\\rcoasterlp2.wav", 255);
				miney->boostAnimState = MINECART_NOBOOST;
			}
			break;
		case MINECART_BRAKE:
			// CMD: refill boost fuel tanks
			if((path->mineCartInfo->boostRemaining += fTime/MINECART_MAXBOOSTTIME) > 1.0f)
			{
				path->mineCartInfo->boostRemaining = 1.0f;
			}

			// CMD: reduce speed
			if((path->mineCartInfo->speedFactor -= 6.0f*fTime) < 0.5f)
			{
				path->mineCartInfo->speedFactor = 0.5f;
			}
			if(controller->y1 < 0.8f)
			{
				StopChannel(&path->mineCartInfo->soundSpark);
				path->mineCartInfo->soundNormal = PlaySample("loop\\rcoasterlp2.wav", 255);
				miney->boostAnimState = MINECART_NOBOOST;
				break;
			}
			break;
		}
	}	
}

/* -------------------------------------------------------------------------------
   Function : CharacterRespondToGhostTownRide
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/

void CharacterRespondToGhostTownRide(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *tazInstance)
{
	ACTORINSTANCE	*path = tazInstance->actorStatus->actorAttachedTo;
	MINECARTINFO	*miney = path->mineCartInfo;
	ACTORINSTANCE	*dummyCart;
	TBVector		input;
	float			tempFloat;
	int				boostAllowed;

	if(path)
	{
		dummyCart = path->mineCartInfo->dummyInstance;

		input[0] = controller->x1;
		input[1] = -controller->y1;

		// CMD: look for boost
		boostAllowed = FALSE;
		if(controller->circleChannel->value)
		{
			if((path->mineCartInfo->boostRemaining -= fTime/MINECART_MAXBOOSTTIME) < 0.0f)
			{
				path->mineCartInfo->boostRemaining = 0.0f;
			}
			else
			{
				boostAllowed = TRUE;

				if((path->mineCartInfo->speedFactor += 6.0f*fTime) > MINECART_MEXSPEEDFACTOR)
				{
					path->mineCartInfo->speedFactor = MINECART_MEXSPEEDFACTOR;
				}
			}
		}
		else
		{
			if((path->mineCartInfo->boostRemaining += fTime/MINECART_MAXBOOSTTIME) > 1.0f)
			{
				path->mineCartInfo->boostRemaining = 1.0f;
			}
			if((path->mineCartInfo->speedFactor -= 1.0f*fTime) < 1.0f)
			{
				path->mineCartInfo->speedFactor = 1.0f;
			}
		}

		//check for left/right input on analogue stick
		//check to see if max tilt time has been reached

		// CMD: separate input and treat left/right seperately, will be a lot easier
		// CMD: two forces act on the mine cart, bound force is force applied by player which decays linearly
		// CMD: from an initial high
		// CMD: rebound force is lightly damped force which returns cart to central position

		// CMD: calculate bound force
		path->mineCartInfo->tiltBound = input[0]*path->mineCartInfo->strength;

		// CMD: calculate rebound force

		// CMD: decrement strength
		if(bmFAbs(input[0]) > 0.2f)
		{
			if((path->mineCartInfo->strengthClock -= fTime) < 0.0f)
			{
				path->mineCartInfo->strengthClock = MINECART_STRENGTHTIME;
				path->mineCartInfo->strength = 0.0f;
			}
			else
			{
				if((path->mineCartInfo->strength -= MINECART_LOSESTRENGTH*fTime*(bmFAbs(input[0])-0.2f)) < 0.0f)
				{
					path->mineCartInfo->strength = 0.0f;
				}
			}
		}
		else
		{
			path->mineCartInfo->strengthClock = MINECART_STRENGTHTIME;
			if((path->mineCartInfo->strength += MINECART_GAINSTRENGTH*fTime) > 1.0f)
			{
				path->mineCartInfo->strength = 1.0f;
			}
		}

		// CMD: calculate rebound force

		// CMD: calculate tilt
		path->mineCartInfo->tiltTarget = path->mineCartInfo->tiltBound + 
						(path->mineCartInfo->tiltRebound*(0.5f+(miney->speedFactor/(MINECART_MEXSPEEDFACTOR*2.0f))));		// CMD: apply some smoothing
		if(path->mineCartInfo->tilt > path->mineCartInfo->tiltTarget)
		{
			if((path->mineCartInfo->tilt -= MINECART_SMOOTHRATE*fTime) < path->mineCartInfo->tiltTarget)
			{
				path->mineCartInfo->tilt = path->mineCartInfo->tiltTarget;
			}
		}
		else
		if(path->mineCartInfo->tilt < path->mineCartInfo->tiltTarget)
		{
			if((path->mineCartInfo->tilt += MINECART_SMOOTHRATE*fTime) > path->mineCartInfo->tiltTarget)
			{
				path->mineCartInfo->tilt = path->mineCartInfo->tiltTarget;
			}
		}
		else
		{
			path->mineCartInfo->tilt = path->mineCartInfo->tiltTarget;
		}
		//update animation
		//is the cart tilting?
		switch(path->mineCartInfo->boostAnimState)
		{
		case MINECART_BOOST:
			if(!boostAllowed)
			{
				StopChannel(&path->mineCartInfo->soundBoost);
				path->mineCartInfo->soundNormal = PlaySample("loop\\rcoasterlp2.wav", 255);
				path->mineCartInfo->boostAnimState = MINECART_NOBOOST;
			}
			if(path->mineCartInfo->tilt > 0.0f)
			{
				PlayFixedBlendOfTwoAnimations(&dummyCart->actorInstance, "boost", "boost_tilt_right", &path->mineCartInfo->animBlendInfo, path->mineCartInfo->tilt, ANIMFLAG_UPDATE|ANIMFLAG_LOOP, path->mineCartInfo->speedFactor);
				PlayFixedBlendOfTwoAnimations(&tazInstance->actorInstance, "minecartdrivefast", "minecartfasttiltright", &path->mineCartInfo->animBlendInfo, path->mineCartInfo->tilt, ANIMFLAG_LOOP, path->mineCartInfo->speedFactor);
			}
			else
			{
				PlayFixedBlendOfTwoAnimations(&dummyCart->actorInstance, "boost", "boost_tilt_left", &path->mineCartInfo->animBlendInfo, bmFAbs(path->mineCartInfo->tilt), ANIMFLAG_UPDATE|ANIMFLAG_LOOP, path->mineCartInfo->speedFactor);
				PlayFixedBlendOfTwoAnimations(&tazInstance->actorInstance, "minecartdrivefast", "minecartfasttiltleft", &path->mineCartInfo->animBlendInfo, bmFAbs(path->mineCartInfo->tilt), ANIMFLAG_LOOP, path->mineCartInfo->speedFactor);
			}
			break;
		case MINECART_NOBOOST:
			if(boostAllowed)
			{
				StopChannel(&path->mineCartInfo->soundNormal);
				path->mineCartInfo->soundBoost = PlaySample("loop\\rcoasterlp.wav", 255);
				path->mineCartInfo->boostAnimState = MINECART_BOOST;
			}

			if(path->mineCartInfo->tilt > 0.0f)
			{
				PlayFixedBlendOfTwoAnimations(&dummyCart->actorInstance, "drive", "drive_tilt_right", &path->mineCartInfo->animBlendInfo, path->mineCartInfo->tilt, ANIMFLAG_UPDATE|ANIMFLAG_LOOP, path->mineCartInfo->speedFactor);
				PlayFixedBlendOfTwoAnimations(&tazInstance->actorInstance, "minecartdrive", "minecarttiltright", &path->mineCartInfo->animBlendInfo, path->mineCartInfo->tilt, ANIMFLAG_LOOP, path->mineCartInfo->speedFactor);
			}
			else
			{
				PlayFixedBlendOfTwoAnimations(&dummyCart->actorInstance, "drive", "drive_tilt_left", &path->mineCartInfo->animBlendInfo, bmFAbs(path->mineCartInfo->tilt), ANIMFLAG_UPDATE|ANIMFLAG_LOOP, path->mineCartInfo->speedFactor);
				PlayFixedBlendOfTwoAnimations(&tazInstance->actorInstance, "minecartdrive", "minecarttiltleft", &path->mineCartInfo->animBlendInfo, bmFAbs(path->mineCartInfo->tilt), ANIMFLAG_LOOP, path->mineCartInfo->speedFactor);
			}
			break;
		}
	}	
}

/* -------------------------------------------------------------------------------
   Function : CharacterRespondWestHubSevenMineCart
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/

void CharacterRespondWestHubSevenMineCart(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *tazInstance)
{
	ACTORINSTANCE	*path = tazInstance->actorStatus->actorAttachedTo;

	if(path->mineCartInfo->flags & MINECART_BOING)
	{
		if(!CheckIfPlayingAnimation(path->mineCartInfo->dummyInstance, "boing"))
		{
			FlushAnimationQueue(tazInstance);
			PlayAnimationByName(tazInstance, "minecartdrive", 1.0f, 0, 0, 0.1f, REACT);

			FlushAnimationQueue(path->mineCartInfo->dummyInstance);
			PlayAnimationByName(path->mineCartInfo->dummyInstance, "drive", 1.0f, 0, 0, 0.1f, REACT);

			path->flags &= ~MINECART_BOING;
		}
	}
	else
	{
		if(!CheckIfPlayingAnimation(tazInstance, "minecartdrive"))
		{
			FlushAnimationQueue(tazInstance);
			PlayAnimationByName(tazInstance, "minecartdrive", 1.0f, 0, 0, 0.1f, REACT);
		}
		if(!CheckIfPlayingAnimation(tazInstance, "drive"))
		{
			FlushAnimationQueue(path->mineCartInfo->dummyInstance);
			PlayAnimationByName(path->mineCartInfo->dummyInstance, "drive", 1.0f, 0, 0, 0.1f, REACT);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : CreateMineCartDummy
   Purpose : creates instance which is dummy layer for specialtype_minecart
   Parameters : 
   Returns : 
   Info : 
*/

int CreateMineCartDummy(ACTORINSTANCE *actorInstance)
{
	actorInstance->mineCartDummyInfo = (MINECARTDUMMYINFO*)ZALLOC(sizeof(MINECARTINFO));
	ASSERTM(actorInstance->mineCartDummyInfo, "could not allocate memory for dummy mine cart");

	actorInstance->special = SPECIALTYPE_MINECARTDUMMY;

	EnableActorCartoonRenderMode(&actorInstance->actorInstance);
	AddActorToCollisionCache(&collisionCache, actorInstance, 0);
	SetCollisionCallback(actorInstance, RespondToMineCartDummyCollision);

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : RespondToMineCartDummyCollision
   Purpose : hides mine cart dummy, and calls respond to coll for minecart
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToMineCartDummyCollision(ACTORINSTANCE *collider, ACTORINSTANCE *minecartDummy)
{
	ACTORINSTANCE	*mineCart = minecartDummy->mineCartDummyInfo->minecartInstance;

	if((collider->actorStatus->currentState == STATE_JUMP)&&(~mineCart->mineCartInfo->flags & MINECART_CESTTOUT))
	{
		//hide instance, and stop collision
		RemoveActorFromCollisionCache(&collisionCache, minecartDummy);
	
		//attach mine cart actor to Taz

		//fill in the various actorInstances use
		mineCart->mineCartInfo->characterOnCart = collider;
	
		//call respond to collision for minecart specialtype
		RespondToMineCartCollision(collider, mineCart);

		RequestMorronicMessage(MEVENT_MINECART);
	}

	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : UpdateMineCartAnimation
   Purpose : updates dummy minecart's, and Taz's animation
   Parameters : path actorInstance
   Returns : 
   Info : 
*/

void UpdateMineCartAnimation(ACTORINSTANCE *actorInstance)
{
	ACTORINSTANCE	*dummyCart = actorInstance->mineCartInfo->dummyInstance;
	ACTORINSTANCE	*taz = actorInstance->mineCartInfo->characterOnCart;

	//are the brakes being applied?
//	if(actorInstance->mineCartInfo->slowingDown)
//	{
//		if(!CheckIfPlayingAnimation(dummyCart, "slowdown"))
//		{
//			FlushAnimationQueue(dummyCart);
//			PlayAnimationByName(dummyCart, "slowdown", 1.0f, 0, 0, 0.1f, REACT);
//			return;
//		}
//	}
//	//is the cart tilting?
//	if(actorInstance->mineCartInfo->tilt > 0.0f)
//	{
//		PlayFixedBlendOfTwoAnimations(&dummyCart->actorInstance, "drive", "tiltright", &actorInstance->mineCartInfo->animProgress, actorInstance->mineCartInfo->tilt, ANIMFLAG_UPDATE|ANIMFLAG_LOOP, actorInstance->mineCartInfo->speedFactor);
//	}
//	else
//	{
//		PlayFixedBlendOfTwoAnimations(&dummyCart->actorInstance, "drive", "tiltleft", &actorInstance->mineCartInfo->animProgress, bmFAbs(actorInstance->mineCartInfo->tilt), ANIMFLAG_UPDATE|ANIMFLAG_LOOP, actorInstance->mineCartInfo->speedFactor);
//	}
}

/* --------------------------------------------------------------------------------
   Function : TumbleMineCart
   Purpose : Controls the update of tumbling minecarts
   Parameters : 
   Returns : TRUE when tumble has finished
   Info : 
*/

void StartMineCartTumble(ACTORINSTANCE *minecart)
{
#ifndef NO_MINECART_OBSTACLES
	minecart->mineCartInfo->flags |= MINECART_TUMBLE;
	minecart->mineCartInfo->flags |= MINECART_NOCONTROL;
		
	FlushAnimationQueue(minecart->mineCartInfo->dummyInstance);
	if(minecart->mineCartInfo->boostAnimState == MINECART_BOOST)
	{
		PlayAnimationByName(minecart->mineCartInfo->dummyInstance, "tumble_with_rockets", 1.0f, 0, 0, 0.1f, DEATH);
	}
	else
	{
		PlayAnimationByName(minecart->mineCartInfo->dummyInstance, "tumble", 1.0f, 0, 0, 0.1f, DEATH);
	}

	FlushAnimationQueue(minecart->mineCartInfo->characterOnCart);
	PlayAnimationByName(minecart->mineCartInfo->characterOnCart, "minecarttumble", 1.0f, 0, 0, 0.1f, DEATH);

#endif
}

/* --------------------------------------------------------------------------------
   Function : TumbleMineCart
   Purpose : Controls the update of tumbling minecarts
   Parameters : 
   Returns : TRUE when tumble has finished
   Info : 
*/

int TumbleMineCart(ACTORINSTANCE *minecart)
{
	AnimateActorInstance(minecart->mineCartInfo->dummyInstance);
	baPrepActorInstance(&minecart->mineCartInfo->dummyInstance->actorInstance, TRUE);
	if((!CheckIfPlayingAnimation(minecart->mineCartInfo->dummyInstance, "tumble"))&&(!CheckIfPlayingAnimation(minecart->mineCartInfo->dummyInstance, "tumble_with_rockets")))
	{
		return TRUE;
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : StartMineCartFire
   Purpose : Controls the update of tumbling minecarts
   Parameters : 
   Returns : TRUE when tumble has finished
   Info : 
*/

void StartMineCartFire(ACTORINSTANCE *minecart)
{
#ifndef NO_MINECART_OBSTACLES
	int		queue = 0;

	minecart->mineCartInfo->flags |= MINECART_ONFIRE;
	minecart->mineCartInfo->flags |= MINECART_NOCONTROL;
		
	FlushAnimationQueue(minecart->mineCartInfo->dummyInstance);
	FlushAnimationQueue(minecart->mineCartInfo->characterOnCart);

	if(minecart->mineCartInfo->boostAnimState == MINECART_BOOST)
	{
		PlayAnimationByName(minecart->mineCartInfo->dummyInstance, "drivefast_rockets_in", 1.0f, 0, 0, 0.5f, DEATH);
		PlayAnimationByName(minecart->mineCartInfo->characterOnCart, "minecartboosterstop", 1.0f, 0, 0, 0.5f, DEATH);
		queue = 1;
	}

	PlayAnimationByName(minecart->mineCartInfo->dummyInstance, "minecartflamedeath", 1.0f, 0, queue, 0.5f, DEATH);
	PlayAnimationByName(minecart->mineCartInfo->characterOnCart, "minecartflamedeath", 1.0f, 0, queue, 0.5f, DEATH);

#endif
}
/* --------------------------------------------------------------------------------
   Function : MineCartOnFire
   Purpose : Controls the update of burning minecarts
   Parameters : 
   Returns : TRUE when tumble has finished
   Info : 
*/

int MineCartOnFire(ACTORINSTANCE *minecart)
{
	AnimateActorInstance(minecart->mineCartInfo->dummyInstance);
	baPrepActorInstance(&minecart->mineCartInfo->dummyInstance->actorInstance, TRUE);
	if((!CheckIfPlayingAnimation(minecart->mineCartInfo->characterOnCart, "minecartboosterstop"))
					&&(!CheckIfPlayingAnimation(minecart->mineCartInfo->characterOnCart, "minecartflamedeath")))
	{
		return TRUE;
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : StartMineCartDrop
   Purpose : Controls the update of tumbling minecarts
   Parameters : 
   Returns : TRUE when tumble has finished
   Info : 
*/

void StartMineCartDrop(ACTORINSTANCE *minecart)
{
#ifndef NO_MINECART_OBSTACLES
	if(minecart->mineCartInfo->speedFactor < 1.25f)
	{
		minecart->mineCartInfo->flags |= MINECART_DROP;
		minecart->mineCartInfo->flags |= MINECART_NOCONTROL;
		
		FlushAnimationQueue(minecart->mineCartInfo->dummyInstance);
		if(minecart->mineCartInfo->boostAnimState == MINECART_BOOST)
		{
			PlayAnimationByName(minecart->mineCartInfo->dummyInstance, "drop_with_rockets", 1.0f, 0, 0, 0.1f, DEATH);
		}
		else
		{
			PlayAnimationByName(minecart->mineCartInfo->dummyInstance, "drop", 1.0f, 0, 0, 0.1f, DEATH);
		}

		FlushAnimationQueue(minecart->mineCartInfo->characterOnCart);
		PlayAnimationByName(minecart->mineCartInfo->characterOnCart, "minecartdrop", 1.0f, 0, 0, 0.1f, DEATH);
	}

#endif
}

/* --------------------------------------------------------------------------------
   Function : DropMineCart
   Purpose : Controls the update of minecarts which are falling into deep chasms
   Parameters : 
   Returns : TRUE when drop has finished
   Info : 
*/

int DropMineCart(ACTORINSTANCE *minecart)
{
	minecart->mineCartInfo->cameraLookAtOffset[1] -= METERTOUNIT(2.0f)*fTime;
	minecart->mineCartInfo->cameraPosOffset[2] -= METERTOUNIT(0.5f)*fTime;
	if((!CheckIfPlayingAnimation(minecart->mineCartInfo->dummyInstance, "drop"))&&(!CheckIfPlayingAnimation(minecart->mineCartInfo->dummyInstance, "drop_with_rockets")))
	{
		return TRUE;
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : StartMineCartDrop
   Purpose : Controls the update of tumbling minecarts
   Parameters : 
   Returns : TRUE when tumble has finished
   Info : 
*/

void StartMineCartMissingRailAction(ACTORINSTANCE *minecart, EDefaultPath direction)
{
#ifndef NO_MINECART_OBSTACLES

	minecart->mineCartInfo->flags |= MINECART_NOCONTROL;
	FlushAnimationQueue(minecart->mineCartInfo->dummyInstance);

	switch(direction)
	{
	case MINECART_PATH_LEFT:
		minecart->mineCartInfo->flags |= MINECART_DROPLEFT;

		if(minecart->mineCartInfo->boostAnimState == MINECART_BOOST)
		{
			PlayAnimationByName(minecart->mineCartInfo->dummyInstance, "fallleftwithrockets", 1.0f, 0, 0, 0.1f, DEATH);
		}
		else
		{
			PlayAnimationByName(minecart->mineCartInfo->dummyInstance, "fallleft", 1.0f, 0, 0, 0.1f, DEATH);
		}

		FlushAnimationQueue(minecart->mineCartInfo->characterOnCart);
		PlayAnimationByName(minecart->mineCartInfo->characterOnCart, "minecartfallleft", 1.0f, 0, 0, 0.1f, DEATH);
		break;
	case MINECART_PATH_RIGHT:
		minecart->mineCartInfo->flags |= MINECART_DROPRIGHT;

		if(minecart->mineCartInfo->boostAnimState == MINECART_BOOST)
		{
			PlayAnimationByName(minecart->mineCartInfo->dummyInstance, "fallrightwithrockets", 1.0f, 0, 0, 0.1f, DEATH);
		}
		else
		{
			PlayAnimationByName(minecart->mineCartInfo->dummyInstance, "fallright", 1.0f, 0, 0, 0.1f, DEATH);
		}

		FlushAnimationQueue(minecart->mineCartInfo->characterOnCart);
		PlayAnimationByName(minecart->mineCartInfo->characterOnCart, "minecartfallright", 1.0f, 0, 0, 0.1f, DEATH);
		break;
	}

#endif
}

/* --------------------------------------------------------------------------------
   Function : DropMineCart
   Purpose : Controls the update of minecarts which are falling into deep chasms
   Parameters : 
   Returns : TRUE when drop has finished
   Info : 
*/

int DoMissingRailStuff(ACTORINSTANCE *minecart)
{
	if((!CheckIfPlayingAnimation(minecart->mineCartInfo->dummyInstance, "fallright"))
		&&(!CheckIfPlayingAnimation(minecart->mineCartInfo->dummyInstance, "fallleft"))
		&&(!CheckIfPlayingAnimation(minecart->mineCartInfo->dummyInstance, "fallleftwithrockets"))
		&&(!CheckIfPlayingAnimation(minecart->mineCartInfo->dummyInstance, "fallrightwithrockets")))
	{
		return TRUE;
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : ResetMinecartAfterResetEvent
   Purpose : resets the minecart and places taz at he reset position
   Parameters : 
   Returns : 
   Info : 
*/

void ResetMinecartAfterResetEvent(ACTORINSTANCE *minecart)
{
	MINECARTINFO	*info = minecart->mineCartInfo;

	if(info)
	{
		// CMD: reset properties
		info->flags = MINECART_DONOTHING|MINECART_NOCONTROL;
		info->landpointCRC = 0;
		info->speedFactor = 1.0f;
		info->tilt = 0.0f;
		info->animBlendInfo.currentBlend = 0.0f;
		info->animBlendInfo.progress = 0.0f;
		info->slowingDown = FALSE;
		info->tiltClock = 0.0f;
		VFX_motionBlur(0.0f);

		// CMD: camera properties
		SetCameraCollision(gameStatus.player[info->characterOnCart->playerNo].camera, TRUE);
		SetCameraSmooth(gameStatus.player[info->characterOnCart->playerNo].camera,0.06f, 0.15f);

		// CMD: set fade down callback, and start fade
		gameStatus.player[info->characterOnCart->playerNo].camera->fade.StartFadeDown(FADETYPE_CIRCLE, 1.5f);
		gameStatus.player[info->characterOnCart->playerNo].camera->fade.SetPlayer(info->characterOnCart);
		gameStatus.player[info->characterOnCart->playerNo].camera->fade.SetFadeDownFinishedCallback(ResetMinecartFadeDownCallback);
		gameStatus.player[info->characterOnCart->playerNo].camera->fade.FadeUpOnCompletion(true);
	}
}

/* --------------------------------------------------------------------------------
   Function : ResetMinecartAfterResetEvent
   Purpose : resets the minecart and places taz at he reset position
   Parameters : 
   Returns : 
   Info : 
*/

void ResetMinecartFadeDownCallback(FADE *fade)
{
	ACTORINSTANCE	*minecart = fade->GetPlayer()->actorStatus->actorAttachedTo;
	MINECARTINFO	*info = fade->GetPlayer()->actorStatus->actorAttachedTo->mineCartInfo;
	ACTORINSTANCE	*resetPoint;
	TBVector		cameraPos;

	if((minecart)&&(info))
	{
		switch(map.sceneNumber)
		{
		case SCENE_GHOSTTOWN:
			info->flags = MINECART_MOVETOPOSITION;
			FlushAnimationQueue(minecart);
			PlayAnimationByName(minecart, "pull_up", 10.0f, 0, 0, 0.0f, IDLE);

			FlushAnimationQueue(info->dummyInstance);
			PlayAnimationByName(info->dummyInstance, "drive", 1000.0f, 0, 0, 0.0f, IDLE);
			
			break;
		default:
			info->flags = 0;
			FlushAnimationQueue(minecart);
			PlayAnimationByName(minecart, "idle", 1.0f, 0, 0, 0.0, IDLE);

			FlushAnimationQueue(info->dummyInstance);
			PlayAnimationByName(info->dummyInstance, "drive", 1000.0f, 0, 0, 0.0f, IDLE);
			break;
		}

		// CMD: clean up
		AddActorToCollisionCache(&collisionCache, info->dummyInstance, CACHEFLAGS_LIVEUPDATE);
		StopCollidingWith(info->characterOnCart, info->dummyInstance, 1.0f);
		info->characterOnCart = NULL;
		if(RequestStateChange(fade->GetPlayer(), STATE_MOVE))
		{
			FlushAnimationQueue(fade->GetPlayer());
			PlayAnimationByName(fade->GetPlayer(), "idle1", 1.0f, 0, 0, 0.1f, IDLE);
		}

		// CMD: move taz to reset point
		resetPoint = FindActorInstanceInInstanceList(0, info->resetPointCRC, &map.invisibleInstances);
		if(resetPoint)
		{
			bmVectorCopy(fade->GetPlayer()->actorInstance.position, resetPoint->actorInstance.position);
			SetActorOrientation(fade->GetPlayer(), resetPoint->actorInstance.orientation);
			// CMD: reset camera
			SETVECTOR(cameraPos, 0.0f, METERTOUNIT(2.0f), METERTOUNIT(4.0f), 1.0f);
			bmVectorAdd(cameraPos, cameraPos, resetPoint->actorInstance.position);
			SetCameraPosition(gameStatus.player[fade->GetPlayer()->playerNo].camera, cameraPos);
		}
		else bkGenerateEvent("scenechange", "_death");
		fade->GetPlayer()->actorBehaviour->physics.flags &= ~PHYSFLAG_NOGRAVITY;

		fade->SetFadeDownFinishedCallback(NULL);
	}
}

/* --------------------------------------------------------------------------------
   Function : ResetMinecartAfterResetEvent
   Purpose : resets the minecart and places taz at he reset position
   Parameters : 
   Returns : 
   Info : 
*/

void ResetMinecartAfterCompletion(ACTORINSTANCE *minecart)
{
	MINECARTINFO	*info = minecart->mineCartInfo;

	if(info)
	{
		// CMD: reset properties
		info->flags = 0;
		info->landpointCRC = 0;
		info->speedFactor = 1.0f;
		info->tilt = 0.0f;
		info->animBlendInfo.currentBlend = 0.0f;
		info->animBlendInfo.progress = 0.0f;
		info->slowingDown = FALSE;
		info->tiltClock = 0.0f;
		VFX_motionBlur(0.0f);


		FlushAnimationQueue(minecart);
		PlayAnimationByName(minecart, "idle", 1.0f, 0, 0, 0.0, IDLE);

		FlushAnimationQueue(info->dummyInstance);
		PlayAnimationByName(info->dummyInstance, "drive", 10.0f, 0, 0, 0.0f, IDLE);

		// CMD: clean up
		RemoveActorFromCollisionCache(&collisionCache, info->dummyInstance);
		AddActorToCollisionCache(&collisionCache, info->dummyInstance, CACHEFLAGS_LIVEUPDATE);
	}
}

/* --------------------------------------------------------------------------------
   Function : CreateMineCart
   Purpose : Crate mine cart specific stuff
   Parameters : 
   Returns : 
   Info : 
*/

void DrawMinecartDebugInfo(void)
{
	MINECARTINFO	*info;
	char			stringBuf[64];
	ushort			string16Buf[64];
	TBMatrix		matrix;

	if(gameStatus.player[0].actorInstance->actorStatus->actorAttachedTo)
	{
		if(gameStatus.player[0].actorInstance->actorStatus->actorAttachedTo->special == SPECIALTYPE_MINECART)
		{
			info = gameStatus.player[0].actorInstance->actorStatus->actorAttachedTo->mineCartInfo;
			bmMatScale(matrix, 0.5f, 0.5f, 0.5f);

			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
			bdSetObjectMatrix(matrix);
			bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
			bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

			sprintf(stringBuf, "Speed Factor = %f", info->speedFactor);
			bkString8to16(string16Buf, stringBuf);
			bdPrintFontLite(standardFont, string16Buf, strlen(stringBuf), -500.0f, 150.0f, 127,127,127, 127, NULL, NULL);

			sprintf(stringBuf, "        Tilt = %f", info->tilt);
			bkString8to16(string16Buf, stringBuf);
			bdPrintFontLite(standardFont, string16Buf, strlen(stringBuf), -445.0f, 100.0f, 127,127,127, 127, NULL, NULL);

			switch(info->boostAnimState)
			{
			case MINECART_READYBOOST:
				strcpy(stringBuf, "Ready Boost");
				break;
			case MINECART_BOOST:
				strcpy(stringBuf, "Boost");
				break;
			case MINECART_EXITBOOST:
				strcpy(stringBuf, "Exit Boost");
				break;
			case MINECART_JUMP:
				strcpy(stringBuf, "Jump");
				break;
			case MINECART_BRAKE:
				strcpy(stringBuf, "Brake");
				break;
			default:
				strcpy(stringBuf, "");
				break;
			}
			bkString8to16(string16Buf, stringBuf);
			bdPrintFontLite(standardFont, string16Buf, strlen(stringBuf), -425.0f, 250.0f, 127,127,127, 127, NULL, NULL);

			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
			bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
			bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : CreateMinecartSparks
   Purpose : Crate mine cart specific stuff
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateMinecartSparks(ACTORINSTANCE *actorInstance)
{
	MINECARTSPARKS		*info;
	MINECARTDUMMYINFO	*dummyInfo = actorInstance->mineCartInfo->dummyInstance->mineCartDummyInfo;
	TBVector			position, velocity, yAxis;
	TBMatrix			matrix;
	int					i, j;
	float				xSpeed, size, pointFive;

	// CMD: choose random position within box for each vert
	// CMD: then translate that position into world coords

#define		MINECARTSPARKS_MAX_X		(METERTOUNIT(0.05f))
#define		MINECARTSPARKS_MAX_Z		(METERTOUNIT(0.5f))

	if((actorInstance->mineCartInfo->flags & MINECART_TUMBLE)||
		(actorInstance->mineCartInfo->flags & MINECART_DROP)||
		(actorInstance->mineCartInfo->flags & MINECART_BOING)||
		(actorInstance->mineCartInfo->flags & MINECART_DONOTHING)||
		(actorInstance->mineCartInfo->flags & MINECART_GONNADROP)||
		(actorInstance->mineCartInfo->flags & MINECART_LEFTMISSING)||
		(actorInstance->mineCartInfo->flags & MINECART_RIGHTMISSING)||
		(actorInstance->mineCartInfo->flags & MINECART_LEFTOBSTACLE)||
		(actorInstance->mineCartInfo->flags & MINECART_RIGHTOBSTACLE)||
		(actorInstance->mineCartInfo->flags & MINECART_DROPLEFT)||
		(actorInstance->mineCartInfo->flags & MINECART_DROPRIGHT)||
		(actorInstance->mineCartInfo->flags & MINECART_ONFIRE))
	{
		return;
	}

	baPrepActorInstance(&actorInstance->actorInstance, TRUE);
	baPrepActorInstance(&actorInstance->mineCartInfo->dummyInstance->actorInstance, TRUE);

	size = METERTOUNIT(0.05f);
	pointFive = 0.5f;
	for(i = 3;i >= 0;--i)
	{
		if(info = &actorInstance->mineCartInfo->wheelSparks[i])
		{
			switch(actorInstance->mineCartInfo->boostAnimState)
			{
			case MINECART_READYBOOST:
			case MINECART_BOOST:
			case MINECART_EXITBOOST:
			case MINECART_NOBOOST:
				switch(i)
				{
				case 0:
				case 1:
					if(actorInstance->mineCartInfo->tilt > -0.3f) continue;
					xSpeed = METERTOUNIT(UtilGenerateRandomNumber(-4.0f, -2.0f));
					break;
				case 2:
				case 3:
					if(actorInstance->mineCartInfo->tilt < 0.3f) continue;
					xSpeed = METERTOUNIT(UtilGenerateRandomNumber(4.0f, 2.0f));
					break;
				}
				break;
			case MINECART_BRAKE:
				switch(i)
				{
				case 0:
				case 1:
					xSpeed = METERTOUNIT(UtilGenerateRandomNumber(-4.0f, -2.0f));
					break;
				case 2:
				case 3:
					xSpeed = METERTOUNIT(UtilGenerateRandomNumber(4.0f, 2.0f));
					break;
				}
				break;
			default:
				info++;
				continue;
			}
		
			bmVectorCopy(position, info->offset);
			position[0] += UtilGenerateRandomNumber(MINECARTSPARKS_MAX_X, -MINECARTSPARKS_MAX_X);
			bmQuatToMatrix(matrix, actorInstance->mineCartInfo->dummyInstance->actorInstance.orientation);
			bmMatMultiplyVector(matrix, position);
			bmVectorAdd(position, position, actorInstance->mineCartInfo->dummyInstance->actorInstance.position);
			bmVectorCopy(info->position, position);
			info->position[3] = 1.0f;
			info->draw = TRUE;

			bmVectorSet(yAxis, 0.0f, 1.0f, 0.0, 1.0f);
			bmVectorCross(velocity, yAxis, dummyInfo->velocity);
			bmVectorScaleToLength(velocity, velocity, xSpeed);
			bmVectorCMul(yAxis, yAxis, UtilGenerateRandomNumber(METERTOUNIT(5.0f), 0.0f));
			bmVectorAdd(velocity, velocity, yAxis);

			baCreateExtra(crazySparksExtrasSystem,position,velocity,&size,&pointFive, textureSpark, dummyInfo->velocity);
			info++;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawMinecartSparks
   Purpose : Crate mine cart specific stuff
   Parameters : 
   Returns : 
   Info : 
*/

void DrawMinecartSparks(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE		*minecart;
	MINECARTSPARKS		*info;
	int				i;

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_ADDITIVE, 1);

	for(minecart = actorInstanceList->head.next;minecart != &actorInstanceList->head;minecart = minecart->next)
	{
		if(minecart->special != SPECIALTYPE_MINECART) continue;

		if(info = minecart->mineCartInfo->wheelSparks)
		{
			bdSetIdentityObjectMatrix();
			for(i = 4;i > 0;--i)
			{
				if(info->draw == TRUE)
				{

					bdDrawFlatSprite(info->position, METERTOUNIT(UtilGenerateRandomNumber(0.5f, 0.2f)), METERTOUNIT(UtilGenerateRandomNumber(0.5f, 0.2f)),
										UtilGenerateRandomNumber(RAD(360.0f), 0.0f), info->texture, 0, 0, 127, 127, 127, 80, NULL, 0.0f);

					bdDrawFlatSprite(info->position, METERTOUNIT(UtilGenerateRandomNumber(0.4f, 0.15f)), METERTOUNIT(UtilGenerateRandomNumber(0.4f, 0.15f)),
										UtilGenerateRandomNumber(RAD(360.0f), 0.0f), info->texture, 0, 0, 127, 127, 127, 40, NULL, 0.0f);

					info->draw = FALSE;
				}

				info++;
			}
		}
	}

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
}

/* --------------------------------------------------------------------------------
   Function : TragicMinecartEventSounds
   Purpose : Clears any looping sounds, and plays an event sound
   Parameters : 
   Returns : -1 if could not find a specified sound
   Info : 
*/

int TragicMinecartEventSounds(MINECARTINFO *info, char *sampleName)
{
	StopChannel(&info->soundBoost);
	StopChannel(&info->soundNormal);
	StopChannel(&info->soundSpark);

	info->flags |= MINECART_TRAGICSOUND;

	if(sampleName)
	{
		return PlaySample(sampleName, 255);
	}
	return TRUE;
}