// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : precipitation.cpp
//   Purpose : functions to create, and update precipitation
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "precipitation.h"
#include "debris.h"
#include "maths.h"
#include "textures.h"
#include "util.h"
#include "status.h"
#include "main.h"


PRECIPITATIONLIST		precipitationList;

/* --------------------------------------------------------------------------------
   Function : InitPrecipitationList
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void InitPrecipitationList(void)
{
	precipitationList.numEntries = 0;
	precipitationList.head.next = precipitationList.head.prev = &precipitationList.head;
}

/* --------------------------------------------------------------------------------
   Function : AddEntryToPrecipitationList
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void AddEntryToPrecipitationList(TBVector bounds, TBVector windVelocity, int density, float drawDistance, float xDim, float yDim, uint32 type, float rate)
{
	PRECIPITATIONLISTENTRY		*newEntry;
	PRECIPITATIONLISTENTRY		*tempPtr;

	tempPtr = precipitationList.head.next;

	if(rate)
	{
		//create new structure for incoming information
		if(newEntry = (PRECIPITATIONLISTENTRY *)ZALLOC(sizeof(PRECIPITATIONLISTENTRY)))
		{
			bmVectorCopy(newEntry->bounds, bounds);
			bmVectorCopy(newEntry->windVelocity, windVelocity);
			if((newEntry->density = density) == 0.0f)
			{
				bkPrintf("      *** WARNING *** precipitation density is 0.0, this wil result in a total lack of precipitation\n");
			}
			newEntry->drawDistance = drawDistance;
			newEntry->xDim = xDim;
			newEntry->yDim = yDim;
			newEntry->type = type;
			if((newEntry->rate = rate) == 0.0f)
			{
				bkPrintf("      *** WARNING *** precipitation period is 0.0, this wil result in a total lack of precipitation\n");
			}
			newEntry->currentTime = 0.0f;

			newEntry->next = tempPtr;
			newEntry->prev = tempPtr->prev;
			tempPtr->prev->next = newEntry;
			tempPtr->prev = newEntry;
			precipitationList.numEntries++;
		}

		ASSERT(newEntry);
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdatePrecipitationList
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdatePrecipitationList(void)
{
	PRECIPITATIONLISTENTRY	*ptr;
	
	if(precipitationList.head.next != NULL)
	{
		for (ptr = precipitationList.head.next;ptr != &precipitationList.head;ptr = ptr->next)
		{
			CreatePrecipitation(ptr->bounds, ptr->windVelocity, ptr->density, ptr->drawDistance, ptr->xDim, ptr->yDim, ptr->type, ptr->rate, ptr);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : CreateSnow
   Purpose : Creates tiled boxes of randomly generated snow sprites
   Parameters : bounds = {xMin, xMax, zMin, zMax};
   Returns : 
   Info : 
*/

int CreatePrecipitation(TBVector bounds, TBVector windVelocity, int density, float drawDistance, float xDim, float yDim, uint32 type, float rate, PRECIPITATIONLISTENTRY *listEntryPtr)
{
	TBVector		position, boxPosition, velocity;
	TBVector		rotation = {0.0f, 0.0f, 0.0f, 0.0f};
	int				counter;
	int				xNumBoxes, zNumBoxes;
	int				resolution;
	float			lifeTime;

	if((listEntryPtr->currentTime += fTime) < rate) return FALSE;
	else listEntryPtr->currentTime = 0.0f;

	//check bounds and determine number of blocks to draw
	if((bounds[0] >= bounds[1])||(bounds[2] >= bounds[3])) return FALSE;
	xNumBoxes = (int)((bounds[1] - bounds[0]) / PRECIPITATION_BOX);
	zNumBoxes = (int)((bounds[3] - bounds[2]) / PRECIPITATION_BOX);

	//create number of sprites as denoted by density
	for(counter = 0; counter < density; counter++)
	{
		if(type & DEBRISFLAG_SNOW)
		{
			//snow falls at a constant velocity, as it reaches its terminal velocity very quickly
			velocity[0] = velocity[2] = 0.0f;
			velocity[1] = SNOW_VELOCITY;
			velocity[3] = 1.0f;
			lifeTime = 10.0f;
		}
		if(type & DEBRISFLAG_RAIN)
		{
			SETVECTOR(velocity, 0.0f, RAIN_VELOCITY, 0.0f, 1.0f);
			lifeTime = 1.0f;
		}

		resolution = (int)PRECIPITATION_BOX;
		boxPosition[0] = (bmRand()%resolution) + bounds[0];
		boxPosition[2] = (bmRand()%resolution) + bounds[2];


		position[1] = METERTOUNIT(7.0f) + gameStatus.player1->actorInstance.position[1] + ((float)(bmRand()%(int)(rate*SNOW_VELOCITY))-(0.5f*rate*SNOW_VELOCITY));
		position[3] = 1.0f;
		position[0] = boxPosition[0]; //(xCounter*PRECIPITATION_BOX) + boxPosition[0];
		position[2] = boxPosition[2]; //(zCounter*PRECIPITATION_BOX) + boxPosition[2];

		rotation[1] = RAD(bmRand()%SNOW_ROTATION);						//angular velocity

		baCreateExtra(precipitationSystem, position, velocity, bounds, rotation, windVelocity, &lifeTime, density, &xDim, &yDim, xNumBoxes, zNumBoxes, type);
	}
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : PrecipitationExtraCreateCallback
   Purpose : This function is called when precipitation is created
   Parameters : 
   Returns : 
   Info : 
*/
void PrecipitationExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms)
{
	 PRECIPITATION	*info = (PRECIPITATION*)vData;
	 float			*position;
	 float			*velocity;
	 float			*bounds;
	 float			*rotationSpeed;
	 float			*windVelocity;

	if(parms)
	{
		position = va_arg(parms, float*);
		velocity = va_arg(parms, float*);
		bounds = va_arg(parms, float*);
		rotationSpeed = va_arg(parms, float*);
		windVelocity = va_arg(parms, float*);
		info->life = *va_arg(parms, float*);
		info->density = (uint32)va_arg(parms, int);
		info->xDim = *va_arg(parms, float*);
		info->yDim = *va_arg(parms, float*);
		info->xNumBoxes = va_arg(parms, int);
		info->zNumBoxes = va_arg(parms, int);
		info->flags = (uint32)va_arg(parms, int);

		info->alpha = 0.0f;
		bmVectorCopy(info->position, position);
		bmVectorCopy(info->velocity, velocity);
		bmVectorCopy(info->bounds, bounds);
		bmVectorCopy(info->rotationSpeed, rotationSpeed);
		bmVectorCopy(info->windVelocity, windVelocity);
		info->drawDistance = METERTOUNIT(10.0f);
		info->radius = METERTOUNIT(1.0f);
		info->rotSpeed = SNOW_ROTATION;

		if(info->flags&DEBRISFLAG_SNOW)
		{
			info->rotation = UtilGenerateRandomNumber(RAD(60.0f), 0.0f);
			info->sprite = textureSnow;
		}
		if(info->flags&DEBRISFLAG_RAIN)
		{
			info->rotation = 0.0f;
			info->sprite = textureRain;
			info->u = (float)(bmRand()%4)*0.25f;
			info->v = (float)(bmRand()%2)*0.5f;
		}

		bmVectorAdd(info->velocity, info->velocity, info->windVelocity);
	}
}

/* --------------------------------------------------------------------------------
   Function : PrecipitationExtraUpdateCallback
   Purpose : This function is called each frame to update the precipitation
   Parameters : 
   Returns : 
   Info : 
*/
void PrecipitationExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t)
{
	PRECIPITATION			*data;
	int						i;
	
	for (i=0;i<noofInstances;i++)
	{
		data = (PRECIPITATION*)dataPtrs[i];

		if((data->life -= fTime) > 0.0f)
		{
			if(data->flags & DEBRISFLAG_SNOW)
			{
				UpdateSnow(data);
			}
			if(data->flags & DEBRISFLAG_RAIN)
			{
				UpdateRain(data);
			}
		}
		else
		{
			baDeleteExtraInstance(eSystem, i);
			--noofInstances;
			continue;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateSnow
   Purpose : updates the position of snow
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateSnow(PRECIPITATION *snow)
{
	TBVector		tempVector;

	
	//adjust vertical position
	snow->position[1] += snow->velocity[1]*fTime;

	//move back to previus centre of rotation
	snow->position[0] -= snow->radius * bmSin(snow->rotationSpeed[0]);
	snow->position[2] -= snow->radius * bmCos(snow->rotationSpeed[0]);

	//increment angle, and calculate xz position
	snow->rotationSpeed[0] += snow->rotationSpeed[1]*fTime;
	bmVectorCMul(tempVector, snow->windVelocity, fTime);

	snow->position[0] += (snow->radius * bmSin(snow->rotationSpeed[0])) + tempVector[0];
		snow->position[2] += (snow->radius * bmCos(snow->rotationSpeed[0])) + tempVector[2];

	if(snow->alpha < SNOW_MAXALPHA)
	{
		if((snow->alpha += (150.0f*fTime)) > SNOW_MAXALPHA)
		{
			snow->alpha = SNOW_MAXALPHA;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateRain
   Purpose : updates the position of rain
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateRain(PRECIPITATION *rain)
{
	TBVector		tempVector;

	//adjust vertical position
	bmVectorCMul(tempVector, rain->velocity, fTime);
	bmVectorAdd(rain->position, rain->position, tempVector);
	if(rain->alpha < 90)
	{
		if((rain->alpha += (int)(512.0f*fTime)) > 90)
		{
			rain->alpha = 90;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : PrecipitationExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void PrecipitationExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context)
{
	PRECIPITATION	*data;
	int				i, j, k, bufIndex, total;
	float			tempFloat;
	TBVector		tempPos;
	
	//doesn't rain on planet x
	if(gameStatus.player[0].actorInstance->flags & ACTORFLAG_DONTCHECKBOUNDINGBOX) return;

	if((int32)context != EXTRA_ALPHA) return;

	bufIndex = 0;
	total = 0;
	for (i=0;i<noofInstances;i++)
	{
		data = (PRECIPITATION*)dataPtrs[i];
		data->position[3] = 1.0f;
		bmVectorCopy(tempPos, data->position);

		for(j=0;j<data->xNumBoxes;j++)
		{
			tempPos[0] += PRECIPITATION_BOX;
			tempPos[2] = data->position[2];
			for(k=0;k<data->zNumBoxes;k++)
			{
				tempPos[2] += PRECIPITATION_BOX;
				//check if within box
				tempFloat = tempPos[0]-gameStatus.player[0].camera->cameraState.cameraPos[0];
				if(tempFloat > data->drawDistance) continue;
				if(tempFloat < -data->drawDistance) continue;
				tempFloat = tempPos[2]-gameStatus.player[0].camera->cameraState.cameraPos[2];
				if(tempFloat > data->drawDistance) continue;
				if(tempFloat < -data->drawDistance) continue;

				//check limit has not been reached
				if((++total) > MAX_DRAWPRECIP) break;
	
				if(bdDrawFlatSprite(tempPos,data->xDim,data->yDim,data->rotation,data->sprite,0,0,255,255,255,
											bmFloatToInt(data->alpha)<<1,&global2DVertexBuffer[bufIndex],0.0f))
				{
					if(data->flags & DEBRISFLAG_RAIN)
					{
						BDVERTEX_SETUV(&global2DVertexBuffer[bufIndex], data->u, data->v);
						BDVERTEX_SETUV(&global2DVertexBuffer[bufIndex+1], data->u, data->v+0.5f);
						BDVERTEX_SETUV(&global2DVertexBuffer[bufIndex+2], data->u+0.25f, data->v);
						BDVERTEX_SETUV(&global2DVertexBuffer[bufIndex+3], data->u+0.25f, data->v+0.5f);
					}
					bufIndex += 4;
				}
			}
			//check limit has not been reached
			if((total) > MAX_DRAWPRECIP) break;
		}
		//check limit has not been reached
		if((total) > MAX_DRAWPRECIP) break;
	}

	bdSetRenderState(BDRENDERSTATE_ZWRITE,FALSE,FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);

	bdSetIdentityObjectMatrix();
	bdSetTexture(0, data->sprite);
	if(bufIndex) bdDrawPrimitive(BDPRIMTYPE_QUADLIST, global2DVertexBuffer, bufIndex,BVERTTYPE_SINGLE2D);

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE,TRUE,FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
}

/* --------------------------------------------------------------------------------
   Function : FreeAllPrecipitationFromList
   Purpose : Removes all actors from the list and free the memory allocated completely
   Parameters : 
   Returns : 
   Info : 
*/
void FreeAllPrecipitationFromList(void)
{
	PRECIPITATIONLISTENTRY		  *ptr,*temp;

	bkPrintf("FreeAllPrecipitationFromList\n");

	for (ptr = precipitationList.head.next;ptr != &precipitationList.head;)
	{
		ptr->prev->next = ptr->next;
		ptr->next->prev = ptr->prev;
		precipitationList.numEntries--;

		temp = ptr->next;
		FREE(ptr);
		ptr = temp;
	}
}

/* --------------------------------------------------------------------------------
   Function : PrecipitationExtraDeleteCallback
   Purpose : This function is called when precipitation extras are deleted
   Parameters : 
   Returns : 
   Info : 
*/
int PrecipitationExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata)
{
	PRECIPITATION *data = (PRECIPITATION*)vdata;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : PrecipitationExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/

void PrecipitationExtraSystemInitCallback(TBExtraSystem *eSystem, int reason)
{
}