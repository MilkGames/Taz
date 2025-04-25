// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : actors.cpp
//   Purpose : provides functions to aid in the use of actors
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"


#include "global.h"
#include "actors.h"
#include "main.h"
#include "collisioncache.h"
#include "physics.h"
#include "animation.h"
#include "debris.h"
#include "status.h"
#include "fallingobjects.h"
#include "flocking.h"

/* --------------------------------------------------------------------------------
   Function : CreateFlockers
   Purpose : creates flocking objects
   Parameters : bounding box is in world space
   Returns : true/false dependant on success
   Info : 
*/

int CreateFlockers(ACTORINSTANCE *actorInstance, BOUNDINGBOX *boundingBox, char *name, float safetyRange, int number, uint32 deadFlockerCRC, uint32 sprayCRC)
{
	TBActor			*actor;
	TBActorInstance	*actPtr;
	TBVector		tempVector;
	BOUNDINGBOX		box;
	int				i;

	safetyRange = METERTOUNIT(100.0f);			// CMD: force flockers to ALWAYS interact
	if(number <= 0) return FALSE;
	if(number > 10) number = 10;
	if((actorInstance->flockingInfo = CreateFlockStructure(number)) == NULL) return FALSE;

	actorInstance->special = SPECIALTYPE_FLOCKING;

	box.xmax = actorInstance->flockingInfo->boundingBox.xmax = actorInstance->actorInstance.position[0] + (actorInstance->actorInstance.actor->xMax*actorInstance->actorInstance.scale[0]);
	box.xmin = actorInstance->flockingInfo->boundingBox.xmin = actorInstance->actorInstance.position[0] + (actorInstance->actorInstance.actor->xMin*actorInstance->actorInstance.scale[0]);
	box.ymax = actorInstance->flockingInfo->boundingBox.ymax = actorInstance->actorInstance.position[1] + (actorInstance->actorInstance.actor->yMax*actorInstance->actorInstance.scale[1]);
	box.ymin = actorInstance->flockingInfo->boundingBox.ymin = actorInstance->actorInstance.position[1] + (actorInstance->actorInstance.actor->yMin*actorInstance->actorInstance.scale[1]);
	box.zmax = actorInstance->flockingInfo->boundingBox.zmax = actorInstance->actorInstance.position[2] + (actorInstance->actorInstance.actor->zMax*actorInstance->actorInstance.scale[2]);
	box.zmin = actorInstance->flockingInfo->boundingBox.zmin = actorInstance->actorInstance.position[2] + (actorInstance->actorInstance.actor->zMin*actorInstance->actorInstance.scale[2]);
	actorInstance->flockingInfo->commonCentre[0] = box.xmin + ((box.xmax - box.xmin)*0.5f);
	actorInstance->flockingInfo->commonCentre[1] = box.ymax + ((box.ymax - box.ymin)*0.5f);
	actorInstance->flockingInfo->commonCentre[2] = box.zmax + ((box.zmax - box.zmin)*0.5f);
	actorInstance->flockingInfo->commonCentre[3] = 1.0f;

	actorInstance->flockingInfo->numFlockers = number;
	actorInstance->flockingInfo->safetyRange = safetyRange;
	actorInstance->flockingInfo->deadFlockerCRC = deadFlockerCRC;

	LoadActor(name,map.mapName);
	actor = FindActorInActorList(name);
	if(actor)
	{
		for(i=0;i<number;i++)
		{
			actPtr = &actorInstance->flockingInfo->flockers[i]->actorInstance;

			baInitActorInstance(actPtr, actor,1);

			SETVECTOR(actPtr->scale, 1.0f, 1.0f, 1.0f, 1.0f);
			//set random orientation
			SETVECTOR(tempVector, 0.0f, 1.0f, 0.0f, (RAD((float)(bmRand()%360))));
			bmRotationToQuat(actPtr->orientation, tempVector);
			//choose random start position from within bounding box
			actPtr->position[0] = boundingBox->xmin + (float)(bmRand()%(1+(int)(boundingBox->xmax - boundingBox->xmin)));
			actPtr->position[1] = boundingBox->ymin + (float)(bmRand()%(1+(int)(boundingBox->ymax - boundingBox->ymin)));
			actPtr->position[2] = boundingBox->zmin + (float)(bmRand()%(1+(int)(boundingBox->zmax - boundingBox->zmin)));
			actPtr->position[3] = 1.0f;

			baPrepActorInstance(actPtr,TRUE);
			baSetActorRenderMode(actPtr, 0, BACTRENDERMODE_NORMAL, NULL);
			baSetActorRenderMode(actPtr, 1, BACTRENDERMODE_NONE, NULL);
			baSetNodeFrame(actPtr, actPtr->rootNodeInstance, 0, TRUE);
			baFlushNodeQueue(actPtr, actPtr->rootNodeInstance,TRUE);

			actorInstance->flockingInfo->flockers[i]->onScreen = TRUE;
			actorInstance->flockingInfo->flockers[i]->alpha = 256.0f;
			actorInstance->flockingInfo->flockers[i]->state = FLOCKER_FLOCKING;
			actorInstance->flockingInfo->flockers[i]->deadFlockerCRC = deadFlockerCRC;
			actorInstance->flockingInfo->flockers[i]->sprayTextureCRC = sprayCRC;
			RandomlyPlayAnimationByName(actPtr,"fly",1.0f,1,0,0.1f,IDLE);
		}
	}
	else
	{
		for(i=0;i<actorInstance->flockingInfo->numFlockers;i++)
		{
			FREE(actorInstance->flockingInfo->flockers[i]);
		}
		FREE(actorInstance->flockingInfo->flockers);
		FREE(actorInstance->flockingInfo);
		actorInstance->special = (ESpecialType)-1;
	}
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : CreateFlockStructure
   Purpose : creates structures to contain info on flocking objects
   Parameters : 
   Returns : 
   Info : 
*/

FLOCKINGINFO *CreateFlockStructure(int numFlockers)
{
	FLOCKINGINFO	*ptr;
	int				i;

	ptr = (FLOCKINGINFO*)ZALLOC(sizeof(FLOCKINGINFO));
	if(!ptr)
	{
		bkPrintf("*** WARNING *** flocking info structure could not be created./n");
		return NULL;
	}

	ptr->flockers = (FLOCKERS**)ZALLOC(sizeof(FLOCKERS*)*numFlockers);
	if(!ptr->flockers)
	{
		bkPrintf("*** WARNING *** flockers ptr array malloc failed/n");
		FREE(ptr);
		return NULL;
	}
	for(i=0;i<numFlockers;i++)
	{
		ptr->flockers[i] = (FLOCKERS*)ZALLOC(sizeof(FLOCKERS));
		if(!ptr->flockers[i])
		{
			bkPrintf("*** WARNING *** flocker creation failed/n");
			FREE(ptr->flockers);
			FREE(ptr);
			return NULL;
		}
	}
	return ptr;
}

/* --------------------------------------------------------------------------------
   Function : PrepFlockers
   Purpose : preps flocking objects within a flocking specialtype
   Parameters : 
   Returns : 
*/

void PrepFlockers(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE	*ptr;
	int		i;

	for(ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if(ptr->special == SPECIALTYPE_FLOCKING)
		{
			if(ptr->flockingInfo)
			{
				for(i=0;i<ptr->flockingInfo->numFlockers;i++)
				{
					baPrepActorInstance(&ptr->flockingInfo->flockers[i]->actorInstance,TRUE);
					baUpdateNodes(&ptr->flockingInfo->flockers[i]->actorInstance,ptr->flockingInfo->flockers[i]->actorInstance.rootNodeInstance,speedRatio,NULL);
				}
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateFlockers
   Purpose : updates flocking objects within a flocking specialtype
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateFlockers(ACTORINSTANCE *actorInstance)
{
	FLOCKINGINFO	*ptr = actorInstance->flockingInfo;
	TBVector		distance,averageVec,zeroVec,tempPosition;
	TBQuaternion	separationQuat, alignment, cohesion, tempQuat;
	int				i,j,quatCounter, numLocal,wallColl;
	TBVector		velocity;
	TBMatrix		orientationMat;
	float			dot,time;

	SETVECTOR(zeroVec,0.0f, 0.0f, 0.0f, 1.0f);

	for(i=0;i<ptr->numFlockers;i++)
	{
		switch(ptr->flockers[i]->state)
		{
		case FLOCKER_DEAD:
			if((ptr->flockers[i]->deathClock += fTime) > 20.0f)
			{
				if((ptr->flockers[i]->alpha += (fTime*256.0f)) > 256.0f)
				{
					ptr->flockers[i]->alpha = 256.0f;
					ptr->flockers[i]->state = FLOCKER_FLOCKING;
				}
			}
		case FLOCKER_FLOCKING:
			//first check for collisions, including with edge of box
			//check other flockers
			SETVECTOR(averageVec, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempPosition, ptr->flockers[i]->actorInstance.position[0],ptr->flockers[i]->actorInstance.position[1],ptr->flockers[i]->actorInstance.position[2],1.0f);
			quatCounter = 0;
			bmQuatCopy(alignment,bIdentityQuaternion);
			numLocal = 0;
			wallColl = FALSE;
			for(j=0;j<ptr->numFlockers;j++)
			{
				if(&ptr->flockers[i] == &ptr->flockers[j]) continue;
				//check only for local flockers
				bmVectorSub(distance,ptr->flockers[i]->actorInstance.position,ptr->flockers[j]->actorInstance.position);
				if(bmVectorLen(distance) > ptr->safetyRange) continue;
				numLocal++;

				//while in loop calculate average alignment of other flockers
				//try linear interpolation between quats, speed rather than accuracy is key
				if(!wallColl)
				{
					if(quatCounter == 0) continue;
					else
					{
						alignment[0] = (alignment[0] + ptr->flockers[j]->actorInstance.orientation[0])*0.5f;
						alignment[1] = (alignment[1] + ptr->flockers[j]->actorInstance.orientation[1])*0.5f;
						alignment[2] = (alignment[2] + ptr->flockers[j]->actorInstance.orientation[2])*0.5f;
						alignment[3] = (alignment[3] + ptr->flockers[j]->actorInstance.orientation[3])*0.5f;
						quatCounter++;
					}
					/////////////////////////////////////////////////////////////
					//calculate average position of flock
					bmVectorAdd(tempPosition,tempPosition,ptr->flockers[j]->actorInstance.position);
					/////////////////////////////////////
				}
				if(bmVectorLen(distance) > FLOCK_SPEED*8.0f) continue;
				wallColl = TRUE;
				
				bmVectorAdd(averageVec,averageVec,distance);
			}
			//average position again
			if(numLocal)
			{
				bmVectorCDiv(tempPosition,tempPosition,(float)numLocal);
				bmVectorSub(tempPosition,tempPosition,ptr->flockers[i]->actorInstance.position);
				CreateQuatFromPoints(cohesion,zeroVec,tempPosition,0.0f);
			}
			else bmQuatCopy(cohesion,bIdentityQuaternion);
			//check bounding box
			if(ptr->flockers[i]->actorInstance.position[0] > ptr->boundingBox.xmax) 
			{
				averageVec[0] += ptr->boundingBox.xmax - ptr->flockers[i]->actorInstance.position[0];
				wallColl = TRUE;
			}
			if(ptr->flockers[i]->actorInstance.position[0] < ptr->boundingBox.xmin) 
			{
				averageVec[0] += ptr->boundingBox.xmin - ptr->flockers[i]->actorInstance.position[0];
				wallColl = TRUE;
			}

			if(ptr->flockers[i]->actorInstance.position[1] > ptr->boundingBox.ymax) 
			{
				averageVec[1] += ptr->boundingBox.ymax - ptr->flockers[i]->actorInstance.position[1];
				wallColl = TRUE;
			}
			if(ptr->flockers[i]->actorInstance.position[1] < ptr->boundingBox.ymin) 
			{
				averageVec[1] += ptr->boundingBox.ymin - ptr->flockers[i]->actorInstance.position[1];
				wallColl = TRUE;
			}

			if(ptr->flockers[i]->actorInstance.position[2] > ptr->boundingBox.zmax) 
			{
				averageVec[2] += ptr->boundingBox.zmax - ptr->flockers[i]->actorInstance.position[2];
				wallColl = TRUE;
			}
			if(ptr->flockers[i]->actorInstance.position[2] < ptr->boundingBox.zmin) 
			{
				averageVec[2] += ptr->boundingBox.zmin - ptr->flockers[i]->actorInstance.position[2];
				wallColl = TRUE;
			}
			if(wallColl) CreateQuatFromPoints(separationQuat,zeroVec,averageVec,0.0f);
			
			//slerp between three orientaions with relevant priorities given
			TBQuaternion tempQuat;

			time = fTime*0.25f;
			if(wallColl)
			{
				//slerp between current and proposed at rate of full turn in one second
				// TP: quat copy to fix assert on dest == src, poick
				bmQuatCopy(tempQuat, ptr->flockers[i]->actorInstance.orientation);
				bmQuatSlerp(ptr->flockers[i]->actorInstance.orientation,tempQuat,separationQuat,time*10.0f);
			}
			else
			if(numLocal)
			{
				bmVectorNorm4(tempQuat,ptr->flockers[i]->actorInstance.orientation);
				bmVectorNorm4(alignment, alignment);
				dot = bmACos(bmQuatDot(tempQuat,alignment));

				if(dot > RAD(10.0f))
				{
					//match alignment
					// TP: quat copy to fix assert on dest == src, poick
					bmQuatCopy(tempQuat, ptr->flockers[i]->actorInstance.orientation);
					bmQuatSlerp(ptr->flockers[i]->actorInstance.orientation,tempQuat,alignment,time);
				}
				else
				{
					//cohesion
					// TP: quat copy to fix assert on dest == src, poick
					bmQuatCopy(tempQuat, ptr->flockers[i]->actorInstance.orientation);
					bmQuatSlerp(ptr->flockers[i]->actorInstance.orientation,tempQuat,cohesion,time);
				}
			}

			//calculate resultant velocity and hence new position
			bmQuatToMatrix(orientationMat,ptr->flockers[i]->actorInstance.orientation);
			SETVECTOR(zeroVec,0.0f,0.0f,-1.0f,1.0f);
			bmMatMultiplyVector(orientationMat,zeroVec);
			bmVectorScaleToLength(velocity,zeroVec,(FLOCK_SPEED*fTime));
			bmVectorAdd(ptr->flockers[i]->actorInstance.position,ptr->flockers[i]->actorInstance.position,velocity);
			break;
		case FLOCKER_HIT:
			if(!CheckIfPlayingAnimation(&ptr->flockers[i]->actorInstance,"flop"))
			{
				ptr->flockers[i]->state = FLOCKER_DEAD;
				ptr->flockers[i]->alpha = 0;
				ptr->flockers[i]->deathClock = 0.0f;
				FlushAnimationQueue(&ptr->flockers[i]->actorInstance);
				RandomlyPlayAnimationByName(&ptr->flockers[i]->actorInstance,"fly",1.0f,1,0,0.1f,IDLE);
				DropObject(&map.levelInstances,ptr->flockers[i]->deadFlockerCRC,0,NULL,ptr->flockers[i]->actorInstance.position,
					FALLINGTYPE_ONCE,FALLINGTYPE_DESTROY,NULL,NULL,GetHeightOfPoint(ptr->flockers[i]->actorInstance.position,0),FALSE);
			}
			break;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawFlockers
   Purpose : draws flocking objects within a flocking specialtype
   Parameters : 
   Returns : 
   Info : 
*/

void DrawFlockers(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE	*ptr;
	int				i;

	for(ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if(ptr->special == SPECIALTYPE_FLOCKING)
		{
			if(ptr->flockingInfo)
			{
				bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
				bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
				bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
				baSetGlobalAlpha(ptr->currentAlpha);
				// TP: Only set onScreen don't remove
#if BPLATFORM == PC
				bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
#endif
				for(i=ptr->flockingInfo->numFlockers-1;i>=0;i--)
				{
					// TP: Better do some culling i think! (on safari the flockers were taking a sixth of a frame just to draw!)
/*					if(ptr->flockingInfo->flockers[i]->state == FLOCKER_DEAD)
					{
						baSetGlobalAlpha((int)ptr->flockingInfo->flockers[i]->alpha);
					}*/
					if(bmFloatToInt(ptr->flockingInfo->flockers[i]->alpha))
					{
						baSetGlobalAlpha((int)ptr->flockingInfo->flockers[i]->alpha);
						ptr->flockingInfo->flockers[i]->onScreen = baDrawActorInstance(&ptr->flockingInfo->flockers[i]->actorInstance,BSETPRIMALPHATYPE_SOLID);
					}
				}
				bdSetRenderState(BDRENDERSTATE_TEXTUREFILTER, gameStatus.gfxSettings.filterMode, FALSE);
				bdSetRenderState(BDRENDERSTATE_BACKFACECULL, FALSE, FALSE);
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawFlockers
   Purpose : draws flocking objects within a flocking specialtype
   Parameters : 
   Returns : 
   Info : 
*/

void DrawFlockers2(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE	*ptr;
	int				i;

	for(ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if(ptr->special == SPECIALTYPE_FLOCKING)
		{
			if(ptr->flockingInfo)
			{
				bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
				bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
				bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
				baSetGlobalAlpha(ptr->currentAlpha);
				// TP: Only set onScreen don't remove
#if BPLATFORM == PC
				bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
#endif
				for(i=0;i<ptr->flockingInfo->numFlockers;i++)
				{
					bdSetRenderState(BDRENDERSTATE_ZWRITE,TRUE,0);
					baDrawActorInstance(&ptr->flockingInfo->flockers[i]->actorInstance,BSETPRIMALPHATYPE_CKEY);
					bdSetRenderState(BDRENDERSTATE_ZWRITE,FALSE,0);
					baDrawActorInstance(&ptr->flockingInfo->flockers[i]->actorInstance,BSETPRIMALPHATYPE_ALPHA);
				}
				bdSetRenderState(BDRENDERSTATE_TEXTUREFILTER, gameStatus.gfxSettings.filterMode, FALSE);
				bdSetRenderState(BDRENDERSTATE_BACKFACECULL, FALSE, FALSE);
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : CheckForFlockerCollidingWithTazSpitObject
   Purpose : checks to see if an object that taz has spat, is close enough to count as a collision
   Parameters : 
   Returns : TRUE if hit, FALSE if not.
   Info : 
*/

void CheckForFlockerCollidingWithTazSpitObject(DEBRIS *debris)
{
	ACTORINSTANCE	*root;
	TBVector		separation, velocity,rotationSpeed;
	TBTexture		*feather;

	//flocking root objects exists on the invisible instances list
	for(root = map.invisibleInstances.head.next;root != &map.invisibleInstances.head;)
	{
		if(root->special == SPECIALTYPE_FLOCKING)
		{
			if(root->flockingInfo)
			{
				if(root->flockingInfo->deadFlockerCRC)
				{
					for(int i = root->flockingInfo->numFlockers-1;i>=0;i--)
					{
						bmVectorSub(separation,debris->position,root->flockingInfo->flockers[i]->actorInstance.position);
						if(bmVectorLen(separation) < METERTOUNIT(1.0f))
						{
							//only want to create feathers once
							if(root->flockingInfo->flockers[i]->state == FLOCKER_FLOCKING)
							{
								//create feathers
								velocity[X] = (float)bmRand();
								velocity[Y] = (float)bmRand();
								velocity[Z] = (float)bmRand();
								velocity[X] -= 32768.0f/2.0f;
								velocity[Y] *= 2.0f;
								velocity[Z] -= 32768.0f/2.0f;
								rotationSpeed[X] = (bmRand()%3)*PI;
								rotationSpeed[Y] = (bmRand()%5)*PI;
								rotationSpeed[Z] = 0.0f;
								rotationSpeed[W] = 1.0f;
								bmVectorScaleToLength(velocity,velocity,METERTOUNIT(4.8f));
								if((feather = bkLoadTextureByCRC(0, root->flockingInfo->flockers[i]->sprayTextureCRC, 0)) != NULL)
								{
									for(int j = 3;j > 0;j--)
									{
										CreateDebrisActor(feather, root->flockingInfo->flockers[i]->actorInstance.position, 
											velocity, rotationSpeed,1.0f, RAD((float)(bmRand()%360)), 
											METERTOUNIT(0.3f), METERTOUNIT(0.3f), 128, DEBRISFLAG_ROTATESPRITE,
											0,0,RAD((float)(bmRand()%720)-360.0f));
									}
								}
							}

							root->flockingInfo->flockers[i]->state = FLOCKER_HIT;
							FlushAnimationQueue(&root->flockingInfo->flockers[i]->actorInstance);
							PlayAnimationByName(&root->flockingInfo->flockers[i]->actorInstance,"flop",1.0f,0,0,0.1f,NONE);
						}
					}
				}
			}
		}
		
		root = root->next;
	}
}