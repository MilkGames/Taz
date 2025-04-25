
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : fire.cpp
//   Purpose : functions for fire
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "fire.h"
#include "actors.h"
#include "maths.h"
#include "main.h"

TBExtraSystem		*fireExtraSystem;
TBPrimVertex		red[MAX_FIRE*8];

/* --------------------------------------------------------------------------------
   Function : CreateFireInstance
   Purpose : creates structure to hold fire instance information
   Parameters :
   Returns : true or false: succesfull or not
   Info : 
*/

int CreateFireInstance(ACTORINSTANCE *fireInstance, float size)
{
	if(!fireInstance) return FALSE;
	if(!(fireInstance->fireInfo = CreateFireInfo())) return FALSE;

	//fill in attributes
	fireInstance->special = SPECIALTYPE_FIRE;
	fireInstance->fireInfo->maxRadius = size*0.5f;
	fireInstance->fireInfo->clock = 0.0f;
	// CMD: fill in bounding box info for culling
	fireInstance->fireInfo->bb.xmin = fireInstance->actorInstance.position[0] - fireInstance->fireInfo->maxRadius;
	fireInstance->fireInfo->bb.xmax = fireInstance->actorInstance.position[0] + fireInstance->fireInfo->maxRadius;
	fireInstance->fireInfo->bb.ymin = fireInstance->actorInstance.position[1];
	fireInstance->fireInfo->bb.ymax = fireInstance->actorInstance.position[1] + fireInstance->fireInfo->maxRadius*5.0f;
	fireInstance->fireInfo->bb.zmin = fireInstance->actorInstance.position[2] - fireInstance->fireInfo->maxRadius;
	fireInstance->fireInfo->bb.zmax = fireInstance->actorInstance.position[2] + fireInstance->fireInfo->maxRadius;

	if(!(fireInstance->fireInfo->texture = bkLoadTexture(NULL, "fire.bmp", 0))) return FALSE;
	
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : CreateFireInstance
   Purpose : creates structure to hold fire instance information
   Parameters :
   Returns : true or false: succesfull or not
   Info : 
*/

FIREINFO *CreateFireInfo(void)
{
	FIREINFO	*tempFireInfo;

	tempFireInfo = (FIREINFO*)ZALLOC(sizeof(FIREINFO));
	if(tempFireInfo)
	{
		return tempFireInfo;
	}
	else
	{
		bkPrintf("*** WARNING ***, could not create fire info structure\n");
		return NULL;
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateFireInstances
   Purpose : updates all the fire instances on a map
   Parameters :
   Returns : 
   Info : 
*/

void UpdateFireInstances(ACTORINSTANCE *ptr)
{
	if(ptr->fireInfo)
	{
	//	if((ptr->fireInfo->clock +=fTime) > FIRE_PARTICLE_PERIOD)
		{
			CreateFireParticleEffect(ptr);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : CreateFireParticleEffect
   Purpose : creates the actual fire effect
   Parameters :
   Returns : 
   Info : 
*/

void CreateFireParticleEffect(ACTORINSTANCE *fireInstance)
{
	FIREINFO	*info = fireInstance->fireInfo;
	TBVector	position, tempPos;
	int			counter;
	float		halfSize;
	int			red[4] = {255, 49, 0, 127};
	int			yellow[4] = {255, 255, 0, 127};		// CMD: this is not yellow, the 3rd componant is used as a marker

	bmVectorSub(tempPos,gameStatus.viewPoint,fireInstance->actorInstance.position);
	if (bmVectorLen(tempPos) > METERTOUNIT(20.0f)) return;

//	fireInstance->fireInfo->clock = 0.0f;

	if(bdTestBBVisibility(bIdentityMatrix, info->bb.xmin, info->bb.xmax, info->bb.ymin, info->bb.ymax, info->bb.zmin, info->bb.zmax))
	{
		for(counter = 0; counter < 1; counter++)
		{
			tempPos[0] = UtilGenerateRandomNumber(fireInstance->fireInfo->maxRadius, -fireInstance->fireInfo->maxRadius);
			tempPos[1] = 0.0f;
			tempPos[2] = UtilGenerateRandomNumber(fireInstance->fireInfo->maxRadius, -fireInstance->fireInfo->maxRadius);
			tempPos[3] = 1.0f;
			bmVectorAdd(position, fireInstance->actorInstance.position, tempPos);
	
			halfSize = bmFAbs(fireInstance->fireInfo->maxRadius*0.67f);
			if((bmFAbs(tempPos[0]) > halfSize)||(bmFAbs(tempPos[2]) > halfSize))
			{
				baCreateExtra(fireExtraSystem, position, fireInstance->fireInfo->texture, red, FIRETYPE_RED);
			}
			else
			{
				baCreateExtra(fireExtraSystem, position, fireInstance->fireInfo->texture, yellow, FIRETYPE_YELLOW);
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : CreateFireExtrasVertexList
   Purpose : creates the actual fire effect
   Parameters :
   Returns : 
   Info : 
*/

void CreateFireExtrasVertexList(FIREEXTRAS *fireInfo)
{
	float		temp, half;

	temp = METERTOUNIT(0.25f);
	half = METERTOUNIT(0.25f)*0.5f;

	BDVERTEX_SETXYZW(&fireInfo->vertexList[0], -temp, (2.0f*temp), 0.0f, 1.0f);
	BDVERTEX_SETNORMAL(&fireInfo->vertexList[0], 0.0f, 0.0f, 1.0f);
//	BDVERTEX_SETA(&fireInfo->vertexList[0], 128);
//	BDVERTEX_SETUV(&fireInfo->vertexList[0], 0.5f, 1.0f);

	BDVERTEX_SETXYZW(&fireInfo->vertexList[1], (temp+half), temp, 0.0f, 1.0f);
	BDVERTEX_SETNORMAL(&fireInfo->vertexList[1], 0.0f, 0.0f, 1.0f);
//	BDVERTEX_SETA(&fireInfo->vertexList[1], 128);
//	BDVERTEX_SETUV(&fireInfo->vertexList[1], 1.0f, 0.75f);

	BDVERTEX_SETXYZW(&fireInfo->vertexList[2], -half, temp, 0.0f, 1.0f);
	BDVERTEX_SETNORMAL(&fireInfo->vertexList[2], 0.0f, 0.0f, 1.0f);
//	BDVERTEX_SETA(&fireInfo->vertexList[2], 128);
//	BDVERTEX_SETUV(&fireInfo->vertexList[2], 0.0f, 0.75f);

	BDVERTEX_SETXYZW(&fireInfo->vertexList[3], half, -temp, 0.0f, 1.0f);
	BDVERTEX_SETNORMAL(&fireInfo->vertexList[3], 0.0f, 0.0f, 1.0f);
//	BDVERTEX_SETA(&fireInfo->vertexList[3], 128);
//	BDVERTEX_SETUV(&fireInfo->vertexList[3], 1.0f, 0.25f);

	BDVERTEX_SETXYZW(&fireInfo->vertexList[4], -(temp+half), -temp, 0.0f, 1.0f);
	BDVERTEX_SETNORMAL(&fireInfo->vertexList[4], 0.0f, 0.0f, 1.0f);
//	BDVERTEX_SETA(&fireInfo->vertexList[4], 128);
//	BDVERTEX_SETUV(&fireInfo->vertexList[4], 0.0f, 0.25f);

	BDVERTEX_SETXYZW(&fireInfo->vertexList[5], temp, (-2.0f*temp), 0.0f, 1.0f);
	BDVERTEX_SETNORMAL(&fireInfo->vertexList[5], 0.0f, 0.0f, 1.0f);
//	BDVERTEX_SETA(&fireInfo->vertexList[5], 128);
//	BDVERTEX_SETUV(&fireInfo->vertexList[5], 0.5f, 0.0f);
}

/////////////////// fire extra system functions ///////////////////

/* --------------------------------------------------------------------------------
   Function : InitFireExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitFireExtraSystem(void)
{
	fireExtraSystem = baCreateExtraSystem("fire",FireExtraCreateCallback,
		FireExtraUpdateCallback,FireExtraDeleteCallback,FireExtraRenderCallback,
		FireExtraSystemInitCallback,sizeof(FIREEXTRAS),MAX_FIRE,0,0);
}

/* --------------------------------------------------------------------------------
   Function : FireExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void FireExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms)
{
    FIREEXTRAS		*data = (FIREEXTRAS*)vData;
	float			*position;
	TBTexture		*texture;
	int				*colour;
	int				type;
	float			tempScale,tempRot;
	TBMatrix		result,translation,scale,rotation;


    if(parms)
	{
		position = va_arg(parms, float*);
		texture = (TBTexture*)va_arg(parms, TBTexture*);
		colour = va_arg(parms, int*); 
		type = va_arg(parms, int);

		data->texture = texture;
		data->life = 2.0f;

		bmVectorCopy(data->position, position);
		bmVectorCopy(data->initPos, position);
		CreateFireExtrasVertexList(data);
		data->velocity[0] = 0.0f; //(float)(bmRand()%200)-100.0f;
		data->velocity[1] = METERTOUNIT(1.0f);
		data->velocity[2] = 0.0f; //(float)(bmRand()%200)-100.0f;
		data->velocity[3] = 1.0f;
		data->scale = 0.4f + UtilGenerateRandomNumber(0.2f, 0.0f);
		data->rotation = RAD(UtilGenerateRandomNumber(360.0f, 0.0f));
		data->colour[0] = colour[0];
		data->colour[1] = colour[1];
		data->colour[2] = colour[2];
		data->colour[3] = colour[3];
		data->type = type;
		//calculate local to world matrix
		if(type == FIRETYPE_YELLOW)
		{
			tempScale = 0.5f*data->scale;
			tempRot = data->rotation + RAD(UtilGenerateRandomNumber(360.0f, 180.0f))*fTime;
		}
		else
		{
			tempScale = data->scale;
			tempRot = data->rotation;
		}
		bmMatTranslate(translation, data->position[0], data->position[1], data->position[2]);
		bmMatScale(scale, tempScale, tempScale, tempScale);
		bmMatYRotation(rotation, tempRot);
		bmMatMultiply(result, rotation, scale);
		bmMatMultiply(data->localToWorld, translation, result);
	}
}

/* --------------------------------------------------------------------------------
   Function : FireExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int FireExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata)
{
	FIREEXTRAS *data = (FIREEXTRAS*)vdata;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : FireExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void FireExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t)
{
	FIREEXTRAS		*data;
	TBMatrix		result,translation,scale,rotation;
	TBVector		tempVector;
	int				i, counter;
	float			tempScale, tempRot;
	
	for (i=0;i<noofInstances;i++)
	{
		data = (FIREEXTRAS*)dataPtrs[i];

		if((data->life -= fTime) > 0.0f)
		{
			//update position
			bmVectorCMul(tempVector, data->velocity, fTime);
			bmVectorAdd(data->position, data->position, tempVector);
			if((data->scale -= 0.3f*fTime) < 0.0f)
			{
				data->scale = 0.0f;
				baDeleteExtraInstance(eSystem, i);
				noofInstances--;
				continue;
			}
			data->rotation += RAD(UtilGenerateRandomNumber(360.0f, 180.0f))*fTime;
			if(data->type == FIRETYPE_YELLOW)
			{
				if((data->colour[3] -= (uchar)(100.0f*fTime)) < 0)
				{
					data->colour[3] = 0;
					baDeleteExtraInstance(eSystem, i);
					noofInstances--;
					continue;
				}
				data->colour[0] -= (int)(64.0f*fTime);
				data->colour[1] -= (int)(128.0f*fTime);
//				data->colour[2] = 255;
			}
			else
			{
				if((data->colour[3] -= (uchar)(100.0f*fTime)) < 0)
				{
					data->colour[3] = 0;
					baDeleteExtraInstance(eSystem, i);
					noofInstances--;
					continue;
				}
				data->colour[0] -= (int)(128.0f*fTime);
				data->colour[1] -= (int)(25.0f*fTime);
//				data->colour[2] = 0;
			}
			for(counter=0;counter<6;counter++)
			{
				BDVERTEX_SETRGBA(&data->vertexList[counter], data->colour[0],data->colour[1], data->colour[2], data->colour[3]);
			}
			//calculate local to world matrix
			if(data->type == FIRETYPE_YELLOW)
			{
				tempScale = 0.5f*data->scale;
				tempRot = data->rotation + RAD(UtilGenerateRandomNumber(360.0f, 180.0f))*fTime;
			}
			else
			{
				tempScale = data->scale;
				tempRot = data->rotation;
			}
			bmMatTranslate(translation, data->position[0], data->position[1], data->position[2]);
			bmMatScale(scale, tempScale, tempScale, tempScale);
			bmMatYRotation(rotation, tempRot);
			bmMatMultiply(result, rotation, scale);
			bmMatMultiply(data->localToWorld, translation, result);

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
   Function : FireExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void FireExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context)
{
	FIREEXTRAS				*data;
	TBVector				tempPos;

	TBPrimVertex			*verts;
	int						i,counter;
	int						yellowVertNum, redVertNum;

	if((int32)context != EXTRA_ALPHA) return;

	//init counters
	yellowVertNum = 0;
	redVertNum = 0;

	for (i=0;i<noofInstances;i++)
	{
		data = (FIREEXTRAS*)dataPtrs[i];

//		if(data->type == FIRETYPE_YELLOW)
//		{
//			verts = &yellow[yellowVertNum];
//		}
//		else
//		{
			verts = &red[redVertNum];
//		}

		for(counter=0;counter<8;counter++)
		{
			if(counter == 0)
			{
				TBPrimVertexCopy(&verts[0], &data->vertexList[0]);
			}
			else
			if(counter == 7)
			{
				TBPrimVertexCopy(&verts[counter], &data->vertexList[5]);
			}
			else
			{
				TBPrimVertexCopy(&verts[counter], &data->vertexList[counter-1]);
			}
			BDVERTEX_GETXYZW(&verts[counter], tempPos[0], tempPos[1], tempPos[2], tempPos[3]);
			bmMatMultiplyVector(data->localToWorld, tempPos);
			BDVERTEX_SETXYZW(&verts[counter], tempPos[0], tempPos[1], tempPos[2], tempPos[3]);
		}
//		if(data->type == FIRETYPE_YELLOW) yellowVertNum += 8;
//		else 	
		redVertNum += 8;
	}

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetTexture(0, NULL);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);

	// PP: set normal z-testing
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);

	//draw red

	bdSetIdentityObjectMatrix();
	bdSetTexture(0,data->texture);
	if(redVertNum)
	{
		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, red, redVertNum, BVERTTYPE_SINGLE);
	}

	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	//draw yellow
		
//	bdSetIdentityObjectMatrix();
//	bdSetTexture(0,data->texture);
//	if(yellowVertNum)
//	{
//		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, yellow, yellowVertNum, BVERTTYPE_SINGLE);
//	}

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
}

/* --------------------------------------------------------------------------------
   Function : FireExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/
void FireExtraSystemInitCallback(TBExtraSystem *eSystem, int reason)
{
}

/* --------------------------------------------------------------------------------
   Function : TBPrimVertexCopy
   Purpose : This function copies a TBPrimVertex structure
   Parameters : 
   Returns : 
   Info : 
*/

void TBPrimVertexCopy(TBPrimVertex *dest, TBPrimVertex *source)
{
	float			x,y,z,w,xn,yn,zn,u,v;
#if BPLATFORM == PS2
	float			rPs,gPs,bPs,aPs;
#endif
	int				r,g,b,a;
	BDVERTEX_GETXYZW(source, x, y, z, w);
	BDVERTEX_SETXYZW(dest, x, y, z, w);

	BDVERTEX_GETNORMAL(source, xn, yn, zn);
	BDVERTEX_SETNORMAL(dest, xn, yn, zn);

	BDVERTEX_GETUV(source, u, v);
	BDVERTEX_SETUV(dest, u, v);

#if BPLATFORM == PS2
	BDVERTEX_GETRGBA(source, rPs, gPs, bPs, aPs);
	BDVERTEX_SETRGBA(dest, rPs, gPs, bPs, aPs);
#else// PP: use PC settings on Xbox etc.
	BDVERTEX_GETRGBA(source, r, g, b, a);
	BDVERTEX_SETRGBA(dest, r, g, b, a);
#endif// PP: BPLATFORM == PS2 else
}
