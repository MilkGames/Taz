// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : debris.cpp
//   Purpose : provides functions to draw and move small incidental objects and textures
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "status.h"

#include "fountainextrasystem.h"
#include "precipitation.h"
#include "holysmoke.h"
#include "debris.h"
#include "main.h"
#include "display.h"
#include "physics.h"
#include "maths.h"
#include "collisioncache.h"
#include "scene.h"
#include "superburpeffect.h"
#include "chillipepper.h"
#include "flocking.h"
#include "collectibles.h"
#include "mechatweety.h"
#include "chillipepper.h"
#include "util.h"
#include "textures.h"
#include "characters.h"

TBExtraSystem	*debrisSystem;
TBExtraSystem	*smokeSystem;
TBExtraSystem	*precipitationSystem;
TBExtraSystem	*bubbleSystem;
TBExtraSystem	*chilliFireSystem;

int	numDebrisUsed = 0;

/* --------------------------------------------------------------------------------
   Function : InitDebrisSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitDebrisSystem(void)
{
	debrisSystem = baCreateExtraSystem("debris",DebrisExtraCreateCallback,
		DebrisExtraUpdateCallback,DebrisExtraDeleteCallback,DebrisExtraRenderCallback,
		DebrisExtraSystemInitCallback,sizeof(DEBRIS),MAX_DEBRIS,0,0);

	smokeSystem = baCreateExtraSystem("smoke",SmokeExtraCreateCallback, 
		SmokeExtraUpdateCallback,SmokeExtraDeleteCallback,SmokeExtraRenderCallback,
		SmokeExtraSystemInitCallback,sizeof(SMOKEEXTRAINFO),MAX_SMOKE,0,0);

	precipitationSystem = baCreateExtraSystem("precipitation",PrecipitationExtraCreateCallback, 
		PrecipitationExtraUpdateCallback,PrecipitationExtraDeleteCallback,PrecipitationExtraRenderCallback,
		PrecipitationExtraSystemInitCallback,sizeof(PRECIPITATION),MAX_PRECIPITATION,0,0);

	bubbleSystem = baCreateExtraSystem("bubbles",BubblesExtraCreateCallback,
		BubblesExtraUpdateCallback,BubblesExtraDeleteCallback,BubblesExtraSystemRenderCallback,
		BubblesExtraSystemInitCallback,sizeof(BUBBLESEXTRA),MAX_BUBBLES,0,0);
	
	chilliFireSystem = baCreateExtraSystem("badbreath",ChilliPepperExtraCreateCallback,
		UpdateChilliPepperFire,ChilliPepperExtraDeleteCallback,ChilliPepperExtraRenderCallback,
		ChilliPepperExtraSystemInitCallback,sizeof(CHILLIPEPPEREXTRAS),MAX_CHILLI_FIRE,0,0);
}


/* --------------------------------------------------------------------------------
   Function : CreateDebrisActor
   Purpose : Call this function to create a collectilble actor
   Parameters : actor to create debris from,pos,life,radius,type
   Returns : TRUE /FALSE
   Info : 
*/

DEBRIS *CreateDebrisActor(char *actorName, TBVector position, TBVector velocity, TBVector rotationSpeed, float life, float scale, float xDim, float yDim, int alpha, uint32 flags, int xNumBoxes, int zNumBoxes,float spriteRotVel)
{
	TBActor			*actor;
	TBTexture		*texture = NULL;
	PRECIPITATION	*precipitation = NULL;
	DEBRIS			*debrisInstance;

	actor = FindActorInActorList(actorName);
	if (actor)
	{
		debrisInstance = (DEBRIS*)baCreateExtra(debrisSystem,actor,texture,precipitation,position,velocity,rotationSpeed,&life,&scale,&xDim,&yDim, alpha,flags,xNumBoxes,zNumBoxes,&spriteRotVel);
		if(flags & DEBRISFLAG_PHYSICS)
		{
			debrisInstance->actorStructure = AddActorInstanceStructureToDebris(debrisInstance, actorName);
		}
	}
	else
	{
		return NULL;
	}
	return debrisInstance;
}

/* --------------------------------------------------------------------------------
   Function : CreateDebrisActor
   Purpose : Call this function to create a collectilble actor
   Parameters : actor to create debris from,pos,life,radius,type
   Returns : TRUE /FALSE
   Info : 
*/

DEBRIS *CreateDebrisActor(uint32 actorName, TBVector position, TBVector velocity, TBVector rotationSpeed, float life, float scale, float xDim, float yDim, int alpha, uint32 flags, int xNumBoxes, int zNumBoxes,float spriteRotVel)
{
	TBActor		*actor;
	TBTexture	*texture = NULL;
	PRECIPITATION	*precipitation = NULL;
	DEBRIS		*debrisInstance;

	actor = FindActorInActorList(actorName);
	if (actor)
	{
		debrisInstance = (DEBRIS*)baCreateExtra(debrisSystem,actor,texture,precipitation,position,velocity,rotationSpeed,&life,&scale,&xDim,&yDim, alpha,flags,xNumBoxes,zNumBoxes,&spriteRotVel);
		if(flags & DEBRISFLAG_PHYSICS)
		{
			debrisInstance->actorStructure = AddActorInstanceStructureToDebris(debrisInstance, "debrisactor");
			// TP: AddActorToCollisionSphereCache(&collisionCache, debrisInstance->actorStructure);
		}
	}
	else
	{
		return NULL;
	}

	return debrisInstance;
}

/* --------------------------------------------------------------------------------
   Function : CreateDebrisActor
   Purpose : Call this function to create a collectilble actor
   Parameters : actor to create debris from,pos,life,radius,type
   Returns : TRUE /FALSE
   Info : 
*/

DEBRIS *CreateDebrisActor(TBTexture *texture, TBVector position, TBVector velocity, TBVector rotationSpeed, float life, float scale, float xDim, float yDim, int alpha, uint32 flags, int xNumBoxes, int zNumBoxes,float spriteRotVel)
{
	TBActor		*actor = NULL;
	PRECIPITATION	*precipitation = NULL;
	DEBRIS		*debrisInstance;

	if (texture)
	{
		debrisInstance = (DEBRIS*)baCreateExtra(debrisSystem,actor,texture,precipitation,position,velocity,rotationSpeed,&life,&scale,&xDim,&yDim, alpha,flags,xNumBoxes,zNumBoxes,&spriteRotVel);
		if(flags & DEBRISFLAG_PHYSICS)
		{
			debrisInstance->actorStructure = AddActorInstanceStructureToDebris(debrisInstance, "debristexture");
			// TP: AddActorToCollisionSphereCache(&collisionCache, debrisInstance->actorStructure);
		}
	}
	else
	{
		return NULL;
	}

	return debrisInstance;
}


/* --------------------------------------------------------------------------------
   Function : CreateDebrisActor
   Purpose : Call this function to create a collectilble actor
   Parameters : actor to create debris from,pos,life,radius,type
   Returns : TRUE /FALSE
   Info : 
*/
/*
DEBRIS *CreateDebrisActor(PRECIPITATION *precipitation, TBVector position, TBVector velocity, TBVector rotationSpeed, float life, float scale, float xDim, float yDim, int alpha, uint32 flags, int xNumBoxes, int zNumBoxes,float spriteRotVel)
{
	TBActor		*actor = NULL;
	TBTexture	*texture = NULL;
	DEBRIS		*debrisInstance;

	if (precipitation)
	{
		debrisInstance = (DEBRIS*)baCreateExtra(precipitationSystem,actor,texture,precipitation,position,velocity,rotationSpeed,&life,&scale,&xDim,&yDim, alpha,flags,xNumBoxes,zNumBoxes,&spriteRotVel);
		if(flags & DEBRISFLAG_PHYSICS)
		{
			debrisInstance->actorStructure = AddActorInstanceStructureToDebris(debrisInstance, "precipdebris");
			// TP: AddActorToCollisionSphereCache(&collisionCache, debrisInstance->actorStructure);
		}
	}
	else
	{
		return NULL;
	}

	return debrisInstance;
}
*/
/* --------------------------------------------------------------------------------
   Function : DebrisCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void DebrisExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms)
{
    DEBRIS			*data = (DEBRIS*)vData;
	TBActor			*actor;
	TBTexture		*texture;
	PRECIPITATION	*precipitation;
	float			*position;
	float			*velocity;
	float			*rotationSpeed;
	float			life;
	float			xDim;
	float			yDim;
	float			spriteRotVel;
	int				alpha,zNumBoxes,xNumBoxes;
	int				colour[3];
	int				playerNo;
	float			ground;
	
    if (parms)
	{
		actor = (TBActor*)va_arg(parms, TBActor*);
		texture = (TBTexture*)va_arg(parms, TBTexture*);
		precipitation = (PRECIPITATION*)va_arg(parms, PRECIPITATION*);
		position = va_arg(parms, float*);
		velocity = va_arg(parms, float*);
		rotationSpeed = va_arg(parms, float*);
		life = *va_arg(parms,float*);
		data->scale = *va_arg(parms,float*);
		xDim = *va_arg(parms,float*);
		yDim = *va_arg(parms,float*);
		alpha = va_arg(parms,int);
		data->flags = va_arg(parms,int);
		xNumBoxes = va_arg(parms,int);
		zNumBoxes = va_arg(parms,int);
		spriteRotVel = *va_arg(parms,float*);
/*
		// TP: these dont seem to be used
		colour[0] = va_arg(parms,int);
		colour[1] = va_arg(parms,int);
		colour[2] = va_arg(parms,int);
		playerNo = va_arg(parms,int);
		ground = *va_arg(parms,float*);
		*/
		colour[0] = 0;
		colour[1] = 0;
		colour[2] = 0;
		playerNo = 0;
		ground = 0;


		data->impactCallback = NULL;
		data->lifeOverCallback = NULL;
		data->position[X] = position[X];
		data->position[Y] = position[Y];
		data->position[Z] = position[Z];
		data->position[W] = 1.0f;
		data->velocity[X] = velocity[X];
		data->velocity[Y] = velocity[Y];
		data->velocity[Z] = velocity[Z];
		data->velocity[W] = 1.0f;
		data->rotationSpeed[X] = rotationSpeed[X];
		data->rotationSpeed[Y] = rotationSpeed[Y];
		data->rotationSpeed[Z] = rotationSpeed[Z];
		data->rotationSpeed[W] = rotationSpeed[W];
		data->life = life;
		data->actorStructure = NULL;
		if(actor)
		{
			data->actorInstance = (TBActorInstance*)MALLOC(sizeof(TBActorInstance));
			if(data->actorInstance==NULL) return;
			baInitActorInstance(data->actorInstance, actor,5);
			bmVectorCopy(data->actorInstance->position,data->position);
			data->actorInstance->scale[X] = data->actorInstance->scale[Y] = 
			data->actorInstance->scale[Z] = data->scale;
			
			baPrepActorInstance(data->actorInstance,TRUE);
			
			baSetActorRenderMode(data->actorInstance, 0, BACTRENDERMODE_NORMAL, NULL);
			baSetActorRenderMode(data->actorInstance, 1, BACTRENDERMODE_NONE, NULL);
			baSetNodeFrame(data->actorInstance, data->actorInstance->rootNodeInstance, 0, TRUE);
			baFlushNodeQueue(data->actorInstance, data->actorInstance->rootNodeInstance, TRUE);
		}
		if(texture)
		{
			data->flags |= DEBRISFLAG_SPRITE;
			data->sprite.texture = texture;
			data->sprite.xDim = xDim;
			data->sprite.yDim = yDim;
			data->sprite.alpha = alpha;
			data->sprite.rotSpeed = spriteRotVel;
			data->sprite.colour[0] = colour[0];
			data->sprite.colour[1] = colour[1];
			data->sprite.colour[2] = colour[2];
			data->playerNo = playerNo;
			data->sprite.ground = ground;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : SmokeExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void SmokeExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms)
{
    SMOKEEXTRAINFO	*data = (SMOKEEXTRAINFO*)vData;
	float			*position;
	float			*velocity;
	
    if (parms)
	{
		position = va_arg(parms, float*);
		bmVectorCopy(data->position, position);
		velocity = va_arg(parms, float*);
		bmVectorCopy(data->velocity, velocity);
		data->texture = (TBTexture*)va_arg(parms, TBTexture*);
		data->size = *va_arg(parms, float*);
		data->rotAngle = RAD(UtilGenerateRandomNumber(360.0f, 0.0f));
		data->life = * va_arg(parms, float*);
		data->alpha = *va_arg(parms, float*);
		data->flags = (uint32)va_arg(parms, uint32);

		data->clock = 0.0f;		
		if(data->flags & DEBRISFLAG_FINGERSMOKE) 
		{
			data->resistence = METERTOUNIT(UtilGenerateRandomNumber(20.0f, 3.0f));
			bmVectorCopy(data->result, data->velocity);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : BubblesExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/

void BubblesExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms)
{
    BUBBLESEXTRA	*data = (BUBBLESEXTRA*)vData;
	float			*position;
	float			*velocity;

	if (parms)
	{
		position = va_arg(parms, float*);
		bmVectorCopy(data->position, position);
		velocity = va_arg(parms, float*);
		bmVectorCopy(data->velocity, velocity);
		data->texture = (TBTexture*)va_arg(parms, TBTexture*);
		data->life = *va_arg(parms,float*);
		data->size = *va_arg(parms, float*);
		data->rotation = *va_arg(parms, float*);
		
		data->colour.r = va_arg(parms, int);
		data->colour.g = va_arg(parms, int);
		data->colour.b = va_arg(parms, int);
		data->colour.a = *va_arg(parms, float*);

		data->flags = va_arg(parms, int);
		data->ground = *va_arg(parms, float*);

		data->xScale = 1.0f;
		data->yScale = 0.0f;
		data->scaleClock = 0.0f;
	}
}

/* --------------------------------------------------------------------------------
   Function : DebrisExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int DebrisExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata)
{
	DEBRIS *data = (DEBRIS*)vdata;

	if (data->lifeOverCallback) 
	{
		data->lifeOverCallback(data);
		data->lifeOverCallback = NULL;
	} 
	
	if (data->actorInstance)
	{
		// if it has an actor then delete it
		baFreeActorInstance(data->actorInstance);
		FREE(data->actorInstance);
		data->actorInstance = NULL;
	}
	// what's that thing i want to delete, you know, flat textured deelei
	data->sprite.texture = NULL;

	//get rid of that nasty precipitation
	data->precipitation.sprite = NULL;

	if(data->actorStructure)
	{
		RemoveActorFromCollisionCache(&collisionCache,data->actorStructure);
		if(data->actorStructure->actorBehaviour)
		{
			FREE(data->actorStructure->actorBehaviour);
			data->actorStructure->actorBehaviour = NULL;
		}
		if (data->actorStructure->actorStatus)
		{
			FREE(data->actorStructure->actorStatus);
			data->actorStructure->actorStatus = NULL;
		}
		FREE(data->actorStructure);
		data->actorStructure = NULL;
	}
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : SmokeExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int SmokeExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata)
{
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : BubblesExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int BubblesExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata)
{
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : DebrisExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void DebrisExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t)
{
	DEBRIS			*data;
	int				i,coll;
	TBVector		tempVector;
	TBQuaternion	tempQuat,resultQuat;
	TBMatrix		tempMatrix;
	
	for (i=0;i<noofInstances;)
	{
		data = (DEBRIS*)dataPtrs[i];

		if (data->actorStructure) 
		{
#ifndef CONSUMERDEMO
			// TP: Added the scene number check because this code seems to crash the spit and special items like cheese and snowballs
			if((map.sceneNumber==SCENE_TAZBOSS)&&(data->actorStructure->actorStatus))
			{
				if((data->actorStructure->actorStatus->currentState == STATE_SPIN)||(data->actorStructure->actorStatus->currentState == STATE_SMASH))
				{
					bmVectorCopy(data->actorStructure->actorInstance.position, data->position);
					UpdateCrateCollisionWithMechaTweety(data->actorStructure);
				}
			}
#endif
			if (data->actorStructure->flags&ACTORFLAG_DESTROYNEXTUPDATE)
			{
				baDeleteExtraInstance(eSystem, i);
				noofInstances--;
				continue;
			}
		}

		if (data->life)
		{
			data->life -= t;
			if ((data->life) <= 0.0f)
			{
				baDeleteExtraInstance(eSystem, i);
				noofInstances--;
				continue;
			}
		}
		if (data->actorInstance)
		{
			// update actor instance
			if(!data->actorStructure)
			{
				bmVectorCopy(data->actorInstance->position,data->position);
				bmVectorCMul(tempVector,data->velocity,fTime);
				bmVectorAdd(data->position,data->position,tempVector);

				// apply gravity if needed
				if (data->flags&DEBRISFLAG_GRAVITY)
				{
					data->velocity[1] -= METERTOUNIT(20.0f)/fps;
				}
				if (data->flags&DEBRISFLAG_LOWGRAVITY)
				{
					data->velocity[1] -= METERTOUNIT(10.0f)/fps;
				}
			}
//			if(data->flags & DEBRISFLAG_CHILLIFIRE)
//			{
//				UpdateChilliPepperFire(data);
//			}


			if (data->flags&DEBRISFLAG_POINTINDIRECTION)
			{
				TBQuaternion	tempQuat;
				TBVector		zeroVector;

				bmVectorSet(zeroVector,0.0f,0.0f,0.0f,1.0f);

				data->velocity[W] = 1.0f;

				// TP: temp use of rotationSpeed[X] to keep track of current z rotation of POINTINDIRECTION type debris objects
				data->rotationSpeed[X] += data->rotationSpeed[Z]/fps;
				data->rotationSpeed[X] = Aabs(data->rotationSpeed[X]);

				CreateQuatFromPoints(tempQuat, zeroVector, data->velocity, data->rotationSpeed[X]);

				bmQuatCopy( data->actorInstance->orientation, tempQuat);
			}
			else
			{
				// apply rotation speed
				bmMatXYZRotation(tempMatrix,data->rotationSpeed[X]/fps,data->rotationSpeed[Y]/fps,data->rotationSpeed[Z]/fps);
				bmMatrixToQuat(tempQuat,tempMatrix);
				bmQuatCopy(resultQuat,data->actorInstance->orientation);
				bmQuatMultiply(data->actorInstance->orientation,resultQuat,tempQuat);
			}
			
			baUpdateNodes(data->actorInstance, data->actorInstance->rootNodeInstance, speedRatio, NULL);
			baPrepActorInstance(data->actorInstance,TRUE);
		}
		else
		{
			if(!data->actorStructure)
			{
				bmVectorCDiv(tempVector, data->velocity, fps);
				bmVectorAdd(data->position, data->position, tempVector);
				// apply gravity
				if(data->flags & DEBRISFLAG_GRAVITY)
				{
					//update position
					data->velocity[1] -= METERTOUNIT(20.0f)/fps;
				}
				if(data->flags & DEBRISFLAG_LOWGRAVITY)
				{
					//update position
					data->velocity[1] -= METERTOUNIT(10.0f)/fps;
				}
				if(data->flags & DEBRISFLAG_SPRITE)
				{
					if((data->sprite.xDim <= 0.0f)||(data->sprite.yDim <= 0.0f))
					{
						baDeleteExtraInstance(eSystem, i);
						noofInstances--;
						continue;
					}
				}
			}
			if(data->flags&DEBRISFLAG_ROTATESPRITE)
			{
				data->scale += DEG(data->sprite.rotSpeed)*fTime;
			}
		}

		if(data->actorStructure)
		{
#ifndef CONSUMERDEMO
			if((data->actorStructure->flags&ACTORFLAG_DEBRISACTOR)&&(data->actorStructure->actorStatus->currentState == STATE_SMASH))
			{
				CheckForFlockerCollidingWithTazSpitObject(data);
			}
#endif
			//fill attributes into dummy actorInstance structure
			if(data->actorInstance)
			{
				bmVectorCopy(data->actorStructure->actorInstance.position, data->position);
				data->actorStructure->actorInstance.scale[0] = data->scale;
				data->actorStructure->actorInstance.scale[1] = data->scale;
				data->actorStructure->actorInstance.scale[2] = data->scale;
				bmQuatCopy(data->actorStructure->actorInstance.orientation, data->actorInstance->orientation);
				bmVectorCopy(data->actorStructure->actorBehaviour->physics.velocity, data->velocity);
			}
			else
			{
				bmVectorCopy(data->actorStructure->actorInstance.position, data->position);
				data->actorStructure->actorInstance.scale[0] = 1.0f;
				data->actorStructure->actorInstance.scale[1] = 1.0f;
				data->actorStructure->actorInstance.scale[2] = 1.0f;
				bmQuatCopy(data->actorStructure->actorInstance.orientation, bIdentityQuaternion);
				bmVectorCopy(data->actorStructure->actorBehaviour->physics.velocity, data->velocity);
			}
			if(data->flags & DEBRISFLAG_PRECIPITATION)
			{
				bmVectorCopy(data->actorStructure->actorInstance.position, data->position);
				data->actorStructure->actorInstance.scale[0] = 1.0f;
				data->actorStructure->actorInstance.scale[1] = 1.0f;
				data->actorStructure->actorInstance.scale[2] = 1.0f;
				bmQuatCopy(data->actorStructure->actorInstance.orientation, bIdentityQuaternion);
				bmVectorCopy(data->actorStructure->actorBehaviour->physics.velocity, data->velocity);
			}

			//process physics
			COLL_INFO *collInfo;
			if (data->flags&DEBRISFLAG_AFFECTOBJECTS)
			{
				coll = ProcessCollisionForActor(&collisionCache, data->actorStructure, 0, &collInfo);
			}
			else
			{
				coll = ProcessCollisionForActor(&collisionCache, data->actorStructure, PROCESSCOLLFLAG_DONTAFFECTOBJECTS, &collInfo);
			}

			//copy attributes back into debris structure
			bmVectorCopy(data->position, data->actorStructure->actorInstance.position);
			if (data->actorInstance)
			{
				bmVectorCopy(data->actorInstance->position, data->actorStructure->actorInstance.position);
			}
			bmVectorCopy(data->velocity, data->actorStructure->actorBehaviour->physics.velocity);

			if (data->impactCallback)		
			{
				if (data->impactCallback(data,collInfo))
				{
				}
				else
				{
					baDeleteExtraInstance(eSystem, i);
					noofInstances--;
					continue;
				}
			}

			if ((data->flags&DEBRISFLAG_STICKY)&&(coll==1))
			{
				// TP: Create new version of object
				ACTORINSTANCE *tempInstance;
				tempInstance = CreateActorInstanceFromActorPtr(&map.levelInstances,data->actorInstance->actor,0,"stickydebris");
				bmVectorCopy(tempInstance->actorInstance.position,data->actorInstance->position);
				bmVectorCopy(tempInstance->actorInstance.scale,data->actorInstance->scale);
				bmQuatCopy(tempInstance->actorInstance.orientation,data->actorInstance->orientation);
				baDeleteExtraInstance(eSystem, i);
				noofInstances--;
				continue;
			}
		}

		if (data->flags&DEBRISFLAG_SCALEOUT)
		{
			// TP: if last 0.5 second of life then scale out
			if (data->life < 0.5f)
			{
				data->scale = data->scale*(data->life*2.0f);
				if (data->actorInstance)
				{
					bmVectorSet(data->actorInstance->scale, data->scale, data->scale, data->scale, 1.0f);
				}
			}
		}

		if (data->flags&DEBRISFLAG_SCALEUP)
		{
			if (data->actorInstance->scale[X] < data->scale)
			{
				data->actorInstance->scale[X] = data->actorInstance->scale[Y] = data->actorInstance->scale[Z] = data->actorInstance->scale[X] + fTime*data->scale*2.0f;
				if (data->actorInstance->scale[X] > data->scale)
				{
					bmVectorSet(data->actorInstance->scale, data->scale, data->scale, data->scale, 1.0f);
					data->flags &= ~DEBRISFLAG_SCALEUP;
				}
			}
		}

		i++;
	}
}

/* --------------------------------------------------------------------------------
   Function : SmokeExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void SmokeExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t)
{
	TBVector		posInc;
	SMOKEEXTRAINFO	*data;
	int				i;

	if(noofInstances > numDebrisUsed) numDebrisUsed = noofInstances;

	for(i=0;i<noofInstances;i++)
	{
		data = (SMOKEEXTRAINFO*)dataPtrs[i];

		if((data->life -= fTime) < 0.0f)
		{
			baDeleteExtraInstance(eSystem, i);
			noofInstances--;
			continue;
		}
		// CMD: update position according to velocity
		bmVectorCMul(posInc, data->velocity, fTime);
		bmVectorAdd(data->position, data->position, posInc);

		if(data->flags & DEBRISFLAG_IMPACTCLOUD)
		{
			if(UpdateImpactClouds(data))
			{
				baDeleteExtraInstance(eSystem, i);
				noofInstances--;
				continue;
			}
		}
		if(data->flags & DEBRISFLAG_EXPLOSIONSMOKE)
		{
			if(UpdateExplosionSmoke(data))
			{
				baDeleteExtraInstance(eSystem, i);
				noofInstances--;
				continue;
			}
		}
		if(data->flags & DEBRISFLAG_FINGERSMOKE)
		{
			if(UpdateFingerSmoke(data))
			{
				baDeleteExtraInstance(eSystem, i);
				--noofInstances;
				continue;
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : BubblesExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void BubblesExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t)
{
	BUBBLESEXTRA			*data;
	int				i;
	
	for (i=0;i<noofInstances;++i)
	{
		data = (BUBBLESEXTRA*)dataPtrs[i];


		if((data->life -= fTime) > 0.0f)
		{
			if(data->flags & DEBRISFLAG_BUBBLES)
			{
				if(UpdateBubbles(data))
				{
					baDeleteExtraInstance(eSystem, i);
					noofInstances--;
					continue;
				}
			}
			if((data->flags & DEBRISFLAG_TWINKLE)||(data->flags & DEBRISFLAG_TWINKLEGLOW))
			{
				if(UpdateTwinkles(data))
				{
					baDeleteExtraInstance(eSystem, i);
					noofInstances--;
					continue;
				}
			}
			if((data->flags & DEBRISFLAG_SHOWER)||(data->flags & DEBRISFLAG_FOODSHOWER))
			{
				if(UpdateCollectiblesShower(data))
				{
					baDeleteExtraInstance(eSystem, i);
					noofInstances--;
					continue;
				}
			}
		}
		else
		{
			baDeleteExtraInstance(eSystem, i);
			noofInstances--;
			continue;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : DebrisExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void DebrisExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context)
{
	TBMatrix	twoPlayerScale;
	DEBRIS *data;
	int			i;

	if((int32)context != EXTRA_SOLID) return;

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	SETZWRITE(TRUE);
	SETZTEST(LESSTHAN);
	ENABLEBLEND(TRUE);
	SETBLEND(BLEND);
	baSetGlobalAlpha(256);

	for (i=0;i<noofInstances;)
	{
		data = (DEBRIS*)dataPtrs[i];
		data->position[3] = 1.0f;

		if (data->actorInstance)
		{
			if (!(data->actorInstance->flags&ACTORFLAG_DONTDRAW))
			{
				bdSetIdentityObjectMatrix();
				baDrawActorInstance(data->actorInstance,BSETPRIMALPHATYPE_SOLID|BSETPRIMALPHATYPE_CKEY|BSETPRIMALPHATYPE_ALPHA);
			}
		}
		else
		{
			bdSetIdentityObjectMatrix();
			bdDrawFlatSprite(data->position, data->sprite.xDim, data->sprite.yDim,
				RAD(data->scale), data->sprite.texture, 0, 0, 127, 127, 127, data->sprite.alpha, NULL,0);
		}
		i++;
	}
}

/* --------------------------------------------------------------------------------
   Function : DebrisExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/

void SmokeExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context)
{
	SMOKEEXTRAINFO	*data;
	int				i, bufIndex, outBufIndex;
	TBVector		tempPos;
	TBMatrix		twoPlayerScale;
	bool			disabledFramewrite=false;			// PP: becomes true as we run DISABLE_FRAMEWRITE
	
	if((int32)context != EXTRA_ALPHA) return;

	bufIndex = 0;
	outBufIndex = 0;
	for(i = 0;i < noofInstances;i++)
//	for(i = (noofInstances-1);i >= 0;--i)
	{
		data = (SMOKEEXTRAINFO*)dataPtrs[i];
		data->position[3] = 1.0f;
		bmVectorCopy(tempPos, data->position);
		tempPos[1]+=METERTOUNIT(0.02f);

			if(bdDrawFlatSprite(data->position, data->size*1.1f, data->size*1.1f,data->rotAngle, data->texture, 0, 0, 
				0, 0, 0, 25/*bmFloatToInt(data->alpha)*/, &global2DVertexBuffer[outBufIndex],0))
			{
				outBufIndex += 4;
			}
	}

//	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
//	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
//	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
//	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);

//	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
//	bdSetIdentityObjectMatrix();
//	bdSetTexture(0, data->texture);
//	if(bufIndex) bdDrawPrimitive(BDPRIMTYPE_QUADLIST, global2DVertexBuffer, bufIndex,BVERTTYPE_SINGLE2D);
//	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
//	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);


//	bufIndex = 0;
	for(i = 0;i < noofInstances;i++)
//	for(i = (noofInstances-1);i >= 0;--i)
	{
		data = (SMOKEEXTRAINFO*)dataPtrs[i];
		data->position[3] = 1.0f;

			if(bdDrawFlatSprite(data->position, data->size, data->size,data->rotAngle, data->texture, 0, 0, 
				255, 255, 255, 30 /*bmFloatToInt(data->alpha)*/, &global2DVertexBuffer[bufIndex+1000],0))
			{
				bufIndex += 4;
			}
	}

	// PP: set an object matrix to correct the appearance of the sprites
	setSpriteObjMat();

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);


	if(gameStatus.player[0].actorInstance)
	{
		if((gameStatus.player[0].actorInstance->characterInfo->powerUpInfo.invisible)&&
				(gameStatus.player[0].actorInstance->characterInfo->powerUpInfo.state == RENDERSTATE_LINEONLY))
		{
			// PP: REMOUT: safer to use DISABLE_FRAMEWRITE (main.h)		bdSetRenderState(BDRENDERSTATE_FRAMEWRITE, 0, 0);
			disabledFramewrite=true;// PP: so's we know to run REENABLE_FRAMEWRITE at the end
			DISABLE_FRAMEWRITE;
		}
	}
	bdSetTexture(0, data->texture);
	if(bufIndex) bdDrawPrimitive(BDPRIMTYPE_QUADLIST, &global2DVertexBuffer[1000], bufIndex,BVERTTYPE_SINGLE2D);

	// PP: DISABLE_FRAMEWRITE and REENABLE_FRAMEWRITE must be paired
	if(disabledFramewrite)
	{
		// PP: REMOUT: safer to use REENABLE_FRAMEWRITE (main.h)	bdSetRenderState(BDRENDERSTATE_FRAMEWRITE, BDFRAMEWRITE_ALL, 0);
		REENABLE_FRAMEWRITE;
	}

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetTexture(0, data->texture);
	bdSetZBias(30000);
	if(outBufIndex) bdDrawPrimitive(BDPRIMTYPE_QUADLIST, global2DVertexBuffer, outBufIndex,BVERTTYPE_SINGLE2D);
	bdSetZBias(0);
	
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
}

/* --------------------------------------------------------------------------------
   Function : BubblesExtraSystemRenderCallback
   Purpose : This function is called each frame to render the bubbles
   Parameters : 
   Returns : 
   Info : 
*/

void BubblesExtraSystemRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context)
{
	BUBBLESEXTRA	*data;
	TBMatrix		twoPlayerScale;
	int				i, bufIndexA,bufIndexB,bufIndexC,bufIndexD,bufIndexE;
	
	if((int32)context != EXTRA_ALPHA) return;

	bufIndexA = 0;
	bufIndexB = 0;
	bufIndexC = 0;
	bufIndexD = 0;
	bufIndexE = 0;
	TBTexture	*twinkle = NULL;

	for (i=0;i<noofInstances;i++)
	{
		data = (BUBBLESEXTRA*)dataPtrs[i];
		data->position[3] = 1.0f;

		if(data->flags&DEBRISFLAG_WHITEBUBBLE)
		{
			if(bdDrawFlatSprite(data->position,data->size,data->size,data->rotation,data->texture,0,0,data->colour.r, 
								data->colour.g, data->colour.b, bmFloatToInt(data->colour.a)<<1,&global2DVertexBuffer[bufIndexA],0))
			{
				bufIndexA += 4;
			}
		}
		if(data->flags&DEBRISFLAG_GREENBUBBLE)
		{
			if(bdDrawFlatSprite(data->position,data->size,data->size,data->rotation,data->texture,0,0,data->colour.r,
								data->colour.g,data->colour.b,bmFloatToInt(data->colour.a)<<1,&global2DVertexBuffer[bufIndexB+1000],0))
			{
				bufIndexB += 4;
			}
		}
		if((data->flags&DEBRISFLAG_POWERUPGLOW)||(data->flags&DEBRISFLAG_TWINKLEGLOW))
		{
			if(bdDrawFlatSprite(data->position,data->size,data->size,data->rotation,data->texture,0,0,data->colour.r,
								data->colour.g,data->colour.b,bmFloatToInt(data->colour.a)<<1,&global2DVertexBuffer[bufIndexC+2000],0))
			{
				bufIndexC += 4;
			}
		}
		if(data->flags&DEBRISFLAG_TWINKLE)
		{
			// PP: set an object matrix to correct the appearance of the sprites
			setSpriteObjMat();

			bdDrawFlatSprite(data->position,data->size*data->xScale,data->size*data->yScale,data->rotation,data->texture,0,0,data->colour.r,
								data->colour.g,data->colour.b,bmFloatToInt(data->colour.a)<<1,NULL/*&global2DVertexBuffer[bufIndexD+3000]*/,0);
//			{
//				bufIndexD += 4;
//			}
//			if(!twinkle) twinkle = data->texture;
		}
		if(data->flags&DEBRISFLAG_SHOWER)
		{
			if(bdDrawFlatSprite(data->position,data->size,data->size,data->rotation,data->texture,0,0,data->colour.r,
								data->colour.g,data->colour.b,bmFloatToInt(data->colour.a)<<1,&global2DVertexBuffer[bufIndexE+3000],0))
			{
				bufIndexE += 4;
			}
		}
		if(data->flags&DEBRISFLAG_FOODSHOWER)
		{
			bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
			
			// PP: set an object matrix to correct the appearance of the sprites
			setSpriteObjMat();

			bdDrawFlatSprite(data->position,data->size,data->size,data->rotation,data->texture,0,0,data->colour.r,
								data->colour.g,data->colour.b,bmFloatToInt(data->colour.a)<<1,NULL,0);
			
			bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, TRUE);
			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
		}
	}

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);

	// PP: set an object matrix to correct the appearance of the sprites
	setSpriteObjMat();

	bdSetTexture(0, textureWhiteBubble);
	if(bufIndexA) bdDrawPrimitive(BDPRIMTYPE_QUADLIST, global2DVertexBuffer, bufIndexA,BVERTTYPE_SINGLE2D);

	bdSetTexture(0, textureGreenBubble);
	if(bufIndexB) bdDrawPrimitive(BDPRIMTYPE_QUADLIST, &global2DVertexBuffer[1000], bufIndexB,BVERTTYPE_SINGLE2D);

	bdSetTexture(0, texturePowerUpGlow);
	if(bufIndexC) bdDrawPrimitive(BDPRIMTYPE_QUADLIST, &global2DVertexBuffer[2000], bufIndexC,BVERTTYPE_SINGLE2D);

//	bdSetTexture(0, twinkle);
//	if(bufIndexD) bdDrawPrimitive(BDPRIMTYPE_QUADLIST, &global2DVertexBuffer[2999], bufIndexD,BVERTTYPE_SINGLE2D);

	bdSetTexture(0, texturePowerUpGlow);
	if(bufIndexE) bdDrawPrimitive(BDPRIMTYPE_QUADLIST, &global2DVertexBuffer[3000], bufIndexE,BVERTTYPE_SINGLE2D);

	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, TRUE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
}

/* --------------------------------------------------------------------------------
   Function : DebrisExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/

void ChimneyPuffExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context)
{
	DEBRIS *data;
	int			i, bufIndex;
	
	if((int32)context != EXTRA_ALPHA) return;

	bufIndex = 0;
	for (i=0;i<noofInstances;i++)
	{
		data = (DEBRIS*)dataPtrs[i];
		data->position[3] = 1.0f;

			if(bdDrawFlatSprite(data->position, data->sprite.xDim, data->sprite.yDim,
				RAD(data->scale), data->sprite.texture, 0, 0, 255, 255, 255, data->sprite.alpha, &global2DVertexBuffer[bufIndex],0))
			{
				bufIndex += 4;
			}
	}

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);
	bdSetIdentityObjectMatrix();
	bdSetTexture(0, data->sprite.texture);
	if(bufIndex) bdDrawPrimitive(BDPRIMTYPE_QUADLIST, global2DVertexBuffer, bufIndex,BVERTTYPE_SINGLE2D);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
}

/* --------------------------------------------------------------------------------
   Function : DebrisExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/

void DebrisExtraSystemInitCallback(TBExtraSystem *eSystem, int reason)
{
}

/* --------------------------------------------------------------------------------
   Function : DebrisExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/

void BubblesExtraSystemInitCallback(TBExtraSystem *eSystem, int reason)
{
}

/* --------------------------------------------------------------------------------
   Function : SmokeExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/

void SmokeExtraSystemInitCallback(TBExtraSystem *eSystem, int reason)
{
}

/* --------------------------------------------------------------------------------
   Function : AddActorInstanceStructureToDebris
   Purpose : Adds ACTORINSTANCE structure to debris extra actor
   Parameters : 
   Returns : 
   Info : 
*/

ACTORINSTANCE *AddActorInstanceStructureToDebris(DEBRIS *debrisInstance, char *name)
{
	ACTORINSTANCE	*tempInstance;

	tempInstance = (ACTORINSTANCE*)MALLOC(sizeof(ACTORINSTANCE));
	if(!tempInstance)
	{
		bkPrintf("*** WARNING *** could not allocate memory for actor instance within debris structure.\n");
		return NULL;
	}

	InitActorStructures(tempInstance);
	AddActorBehaviourToDebris(debrisInstance, tempInstance);

	if (strlen(name)<32)
	{
		strcpy(tempInstance->actorName,name);
	}
	else
	{
		STRNCPY(tempInstance->actorName,name,32);
	}
	if (strlen(name)<32)
	{
		strcpy(tempInstance->instanceName,name);
	}
	else
	{
		STRNCPY(tempInstance->instanceName,name,32);
	}
	tempInstance->actorCRC = bkCRC32((uchar*)name,strlen(name),0);
	tempInstance->instanceCRC = bkCRC32((uchar*)name,strlen(name),0);

	return tempInstance;
}

/* --------------------------------------------------------------------------------
   Function : AddActorBehaviourToDebris
   Purpose : Adds actorBehaviour structure to debris extra actor
   Parameters : 
   Returns : 
   Info : 
*/

int AddActorBehaviourToDebris(DEBRIS *debrisInstance, ACTORINSTANCE *actorInstance)
{
	actorInstance->actorBehaviour = (ACTOR_BEHAVIOUR*)MALLOC(sizeof(ACTOR_BEHAVIOUR));
	if (actorInstance->actorBehaviour)
	{
		if(debrisInstance->actorInstance)
		{
			InitActorBehaviour(actorInstance->actorBehaviour,(METERTOUNIT(0.5f)*debrisInstance->scale));
			actorInstance->actorBehaviour->collSphere.offset[Y] = METERTOUNIT(0.45f)*debrisInstance->scale;
		}
		else
		if(debrisInstance->flags & DEBRISFLAG_SPRITE)
		{
			InitActorBehaviour(actorInstance->actorBehaviour,(debrisInstance->sprite.xDim / 2.0f));
			actorInstance->actorBehaviour->collSphere.offset[Y] = debrisInstance->sprite.xDim / 2.0f;
		}
		else
		if(debrisInstance->flags & DEBRISFLAG_PRECIPITATION)
		{
			InitActorBehaviour(actorInstance->actorBehaviour,(debrisInstance->precipitation.xDim / 2.0f));
			actorInstance->actorBehaviour->collSphere.offset[Y] = debrisInstance->precipitation.xDim / 2.0f;
		}
		actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_WALLVELCORRECTION;
		return TRUE;
	}
	else
	{
		bkPrintf("*** ERROR *** Could not create debris instance physics\n");
		return FALSE;
	}
}
