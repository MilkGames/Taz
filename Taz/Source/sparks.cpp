// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : sparks.cpp
//   Purpose : functions to produce sparks allong a path
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "main.h"
#include "actors.h"
#include "maths.h"
#include "scene.h"
#include "physics.h"
#include "control.h"
#include "sparks.h"
#include "util.h"
#include "animation.h"
#include "textures.h"

TBExtraSystem	*crazySparksExtrasSystem;

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateElectricityObject(float nodeInterceptAccuracy, char *objectName, char *name, TBVector scale, char state, CPMNode *node)
{
	ACTORINSTANCE	*actor;
	int				counter;

	for(counter = 0; counter < 5; counter++)
	{
		actor = CreateActorInstance(&map.animatedInstances, objectName, name);
		bmVectorCDiv(scale, scale, 1.5f);
		bmVectorCopy(actor->actorInstance.scale, scale);
		if(!CreateElectricityInfoStructure(actor)) return FALSE;

		actor->special = SPECIALTYPE_ELECTRICITY;
		actor->electricityInfo->nodeInterceptAccuracy = nodeInterceptAccuracy;
		actor->electricityInfo->hasStartPosition = FALSE;
		actor->electricityInfo->state = state;
		actor->electricityInfo->speed = METERTOUNIT(15.0f);
		actor->electricityInfo->pathNodes = GetElectricityPathNodesFromMap("pathNode", node);
		FlushAnimationQueue(actor);
		PlayAnimationByName(actor, "sparks", 1.0f, 1, 0, 0.1f, NONE);
	}
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateElectricityInfoStructure(ACTORINSTANCE *actor)
{
	if((actor->electricityInfo = (ELECTRICITYINFO*)ZALLOC(sizeof(ELECTRICITYINFO))) == NULL)
	{
		bkPrintf("**** ERROR **** electricityInfo malloc failed\n");
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateElectricityObject(ACTORINSTANCE *ptr)
{
	TBVector		tempVector;
	float			distance;

	if(ptr->electricityInfo)
	{
		if(ptr->electricityInfo->hasStartPosition)
		{
			//check if spark is near to target node
			bmVectorSub(tempVector, ptr->electricityInfo->currentTarget, ptr->actorInstance.position);
			if((distance = bmVectorLen(tempVector)) <= ptr->electricityInfo->nodeInterceptAccuracy)
			{
				CalculateNewSparkTarget(ptr);
			}
			
			MoveSparkTowardsTarget(ptr);
		}
		else
		{
			CalculateSparkStartPosition(ptr);
			ptr->electricityInfo->hasStartPosition = TRUE;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : CalculateSparkStartPosition
   Purpose : 
   Parameters :
   Returns :
   Info : 
*/

void CalculateSparkStartPosition(ACTORINSTANCE *actor)
{
	ACTORINSTANCE	*nodeInstance;

	//choose node to start at
	if(actor->electricityInfo->pathNodes)
	{
		actor->electricityInfo->pathNodes->currentObject = bmRand()%actor->electricityInfo->pathNodes->numObjects;
		nodeInstance = FindActorInstanceInInstanceList(NULL, actor->electricityInfo->pathNodes->objectNames[actor->electricityInfo->pathNodes->currentObject], &map.invisibleInstances);
		if(nodeInstance)
		{
			SetActorPosition(actor, nodeInstance->actorInstance.position);
			bmVectorCopy(actor->electricityInfo->currentTarget, nodeInstance->actorInstance.position);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : CalculateNewSparkTarget
   Purpose : 
   Parameters :
   Returns :
   Info : 
*/

void CalculateNewSparkTarget(ACTORINSTANCE *actor)
{
	ACTORINSTANCE	*nodeInstance;

	if(actor->electricityInfo->pathNodes == NULL) return;

	if(actor->electricityInfo->pathNodes->currentObject == 0)
	{
		actor->electricityInfo->pathNodes->currentObject = 1;
	}
	else
		if(actor->electricityInfo->pathNodes->currentObject == actor->electricityInfo->pathNodes->numObjects)
		{
			actor->electricityInfo->pathNodes->currentObject = actor->electricityInfo->pathNodes->numObjects - 1;
		}
		else
		{
			if((bmRand()%2) == 0)
			{
				actor->electricityInfo->pathNodes->currentObject += 1;
			}
			else
			{
				actor->electricityInfo->pathNodes->currentObject -= 1;
			}
		}
	nodeInstance = FindActorInstanceInInstanceList(NULL, actor->electricityInfo->pathNodes->objectNames[actor->electricityInfo->pathNodes->currentObject], &map.invisibleInstances);
	if(nodeInstance)
	{
		bmVectorCopy(actor->electricityInfo->currentTarget, nodeInstance->actorInstance.position);
	}
}

/* --------------------------------------------------------------------------------
   Function : MoveSparkTowardsTarget
   Purpose : 
   Parameters :
   Returns :
   Info : 
*/

void MoveSparkTowardsTarget(ACTORINSTANCE *actor)
{
	TBVector		tempVector;

	if(actor->electricityInfo->pathNodes == NULL) return;

	bmVectorSub(tempVector, actor->electricityInfo->currentTarget, actor->actorInstance.position);
	if(bmVectorLen(tempVector) == 0.0f) return;

	bmVectorScaleToLength(tempVector, tempVector, (actor->electricityInfo->speed/fps));

	bmVectorAdd(actor->actorInstance.position, actor->actorInstance.position,tempVector);
	UpdateSparkOrientation(actor, tempVector);
}

/* --------------------------------------------------------------------------------
   Function : UpdateSparkOrientation
   Purpose : 
   Parameters :
   Returns :
   Info : 
*/

void UpdateSparkOrientation(ACTORINSTANCE *actor, TBVector velocity)
{
	TBVector			zeroVector = { 0.0f, 0.0f, 0.0f, 1.0f };
	TBVector			tempVec = { 1.0f, 0.0f, 0.0f, 0.0f };
	TBQuaternion		tempQuat, temp2Quat;

	//tempVec[3] = angle;
	tempVec[3] = RAD(0.0f);
	bmRotationToQuat(temp2Quat, tempVec);			//feet first
	CreateQuatFromPoints(tempQuat, zeroVector, velocity, 0.0f);

	bmQuatMultiply(actor->actorInstance.orientation, temp2Quat, tempQuat);
}
/* --------------------------------------------------------------------------------
   Function : CreateElectricityPathInfo
   Purpose : 
   Parameters : ELECTRICITYPATHINFO *
   Returns : ELECTRICITYPATHINFO *
   Info : 
*/

ELECTRICITYPATHINFO *CreateElectricityPathInfo(int numObjects)
{
	ELECTRICITYPATHINFO *pathInfo;

	pathInfo = (ELECTRICITYPATHINFO*)ZALLOC(sizeof(ELECTRICITYPATHINFO));
	if(pathInfo)
	{
		pathInfo->objectNames = (uint32*)ZALLOC(numObjects*sizeof(uint32));
		pathInfo->numObjects = numObjects;
		pathInfo->currentObject = 0;
		return pathInfo;
	}
	return NULL;
}

/* --------------------------------------------------------------------------------
   Function : AddElectricityPathObject
   Purpose : 
   Parameters : ELECTRICITYPATHINFO *, char *
   Returns : 
   Info : 
*/

int AddElectricityPathObject(ELECTRICITYPATHINFO *pathInfo, uint32 objectName)
{
	if(pathInfo->currentObject < pathInfo->numObjects)
	{
		pathInfo->objectNames[pathInfo->currentObject] = objectName;
		pathInfo->currentObject++;
		return TRUE;
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void FreeAllElectricityPathNodes(ACTORINSTANCE *ptr)
{	
	FREE(ptr->electricityInfo->pathNodes->objectNames);
	FREE(ptr->electricityInfo->pathNodes);
	ptr->electricityInfo->pathNodes = NULL;
}

/* --------------------------------------------------------------------------------
   Function : WereGonnaHaveSomeCraziSparks
   Purpose : Creates sparks in a Crazi Taxi Stylee, which are used for indestructibles
   Parameters : 
   Returns : 
   Info : 
*/

void WereGonnaHaveSomeCrazySparks(TBVector position1, TBVector position2, float distFromPosition1, TBTexture *texture)
{
	TBVector	velocity;
	TBVector	position, posNorm;
	TBVector	up = {0.0f, 1.0f,0.0f, 1.0f};
	float		pointFive = 0.5f;
	float		size;

	//calculate initial position along vector from pos1 to pos2
	bmVectorSub(position,position2,position1);
	bmVectorScaleToLength(position,position,distFromPosition1);
	bmVectorNorm(posNorm, position);
	bmVectorAdd(position, position1, position);

	//calculate velocity from cross product of up vector and position
	//randomly choose between posiible directions
	if(bmRand()%2)
	{
		bmVectorCross(velocity,posNorm,up);
	}
	else
	{
		bmVectorCross(velocity,up,posNorm);
	}
	bmVectorScaleToLength(velocity, velocity, (float)(CRAZYSPARKS_SPEED+(bmRand()%CRAZYSPARKS_SPEED)));
	velocity[0] += (float)((bmRand()%250)-125);
	velocity[1] = (float)(750 +(bmRand()%500));
	velocity[2] += (float)((bmRand()%250)-125);

	size = CRAZYSPARKS_SIZE;
	baCreateExtra(crazySparksExtrasSystem,position,velocity,&size,&pointFive, texture, NULL);
}

/* --------------------------------------------------------------------------------
   Function : InitCraziSparksExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitCrazySparksExtraSystem(void)
{
	crazySparksExtrasSystem = baCreateExtraSystem("crazySparks",CrazySparksExtraCreateCallback,
		CrazySparksExtraUpdateCallback,CrazySparksExtraDeleteCallback,CrazySparksExtraRenderCallback,
		CrazySparksPhoneBoxExtraSystemInitCallback,sizeof(CRAZYSPARKSINFO),MAX_CRAZYSPARKS,0,0);
}

/* --------------------------------------------------------------------------------
   Function : CrazySparksExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void CrazySparksExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms)
{
    CRAZYSPARKSINFO		*data = (CRAZYSPARKSINFO*)vData;
	float				*position;
	float				*velocity;
	float				size;
	float				life;

    if(parms)
	{
		position = va_arg(parms, float*);
		velocity = va_arg(parms, float*);
		size = *va_arg(parms, float*);
		life = *va_arg(parms, float*);
		data->texture = (TBTexture*)va_arg(parms, TBTexture*);

		bmVectorCopy(data->leadPosition,position);
		bmVectorCopy(data->trailPosition,position);

		bmVectorCopy(data->velocity,velocity);

		velocity = va_arg(parms, float*);
		if(velocity)
		{
			bmVectorCopy(data->trailVelocity, velocity);
			data->flags = CRAZYSPARKS_TRAILVEL;
		}
		else data->flags = 0;

		if(data->texture == textureSpark)
		{
			data->trailClock = CRAZYSPARKS_DELTA*bmVectorLen(data->velocity)/250.0f;
			data->life = life;
			data->size = size;
			data->alpha = 120;
		}
		else
		{
			data->life = life;
			data->size = METERTOUNIT(0.2f);
			data->alpha = 120;
			data->angle = UtilGenerateRandomNumber(RAD(360.0f), RAD(0.0f));
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : CrazySparksExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int CrazySparksExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata)
{
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : CrazySparksExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void CrazySparksExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t)
{
	CRAZYSPARKSINFO		*data;
	TBVector			positionInc, gravityInc;
	int					i;
	
	for (i=0;i<noofInstances;i++)
	{
		data = (CRAZYSPARKSINFO*)dataPtrs[i];

		if((data->life -= t) > 0.0f)
		{
			if(data->flags & CRAZYSPARKS_TRAILVEL)
			{
				ApplyGravity(positionInc, data->velocity, worldGravity, fTime*0.25f);

				bmVectorCopy(data->trailPosition, data->leadPosition);
				bmVectorAdd(data->leadPosition, data->leadPosition, positionInc);

				bmVectorCMul(positionInc, data->trailVelocity, fTime);
				bmVectorAdd(data->leadPosition, data->leadPosition, positionInc);
				bmVectorAdd(data->trailPosition, data->trailPosition, positionInc);
			}
			else
			{
				ApplyGravity(positionInc,data->velocity,worldGravity);
				//update leadPosition
				bmVectorCopy(data->trailPosition, data->leadPosition);
				bmVectorAdd(data->leadPosition,data->leadPosition,positionInc);
			}

			if(data->life < 0.25f)
			{
				if((data->alpha -= 128.0f*4.0f*t) < 0.0f)
				{
					data->alpha = 0.0f;
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
   Function : CrazySparksExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void CrazySparksExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context)
{
	CRAZYSPARKSINFO *data;
	int				i, bufIndex;
	int				colour[4] = {255,255,255,255};

	if((int32)context != EXTRA_ALPHA) return;

	bufIndex = 0;

	for (i=0;i<noofInstances;i++)
	{
		data = (CRAZYSPARKSINFO*)dataPtrs[i];

		colour[3] = (int)data->alpha;
		data->leadPosition[3] = data->trailPosition[3] = 1.0f;

		if(data->texture == textureSpark)
		{
			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
			if(bdDrawStreakClipped(data->leadPosition,data->size,data->size,colour,data->trailPosition,data->size,data->size,colour,textureSpark,&global2DVertexBuffer[bufIndex],0.0f))
			{
				if(bufIndex)
				{
					//fill in extra vertices between streaks
					fmemcpy(&global2DVertexBuffer[bufIndex-2], &global2DVertexBuffer[bufIndex-3], sizeof(TBPrimVertex2D));
					fmemcpy(&global2DVertexBuffer[bufIndex-1], &global2DVertexBuffer[bufIndex], sizeof(TBPrimVertex2D));
				}
				bufIndex += 10;
			}
		}
		else
		{
			bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);

			bdDrawFlatSprite(data->leadPosition, data->size, data->size, data->angle, data->texture, 0, 0, 128, 128, 128, colour[3]<<1, NULL,0);

			bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
		}
	}

	if(bufIndex)
	{
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);

		bdSetIdentityObjectMatrix();
		bdSetTexture(0,textureSpark);
		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, global2DVertexBuffer, (bufIndex-2),BVERTTYPE_SINGLE2D);

		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
		bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	}
}

/* -------------------------------------------------------------------------------
   Function : CrazySparksPhoneBoxExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/
void CrazySparksPhoneBoxExtraSystemInitCallback(TBExtraSystem *eSystem, int reason)
{
}