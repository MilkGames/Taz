// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : waterfall.cpp
//   Purpose : waterfall functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "actors.h"
#include "physics.h"
#include "textures.h"
#include "display.h"
#include "waterfall.h"
#include "sfx.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : waterfall.cpp
//   Purpose : experimental waterfall functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"

#include "collisioncache.h"
#include "waterfall.h"

TBExtraSystem		*waterfallExtraSystem;
int32				numWfallPolys = 0;
/* --------------------------------------------------------------------------------
   Function : CreateWaterfallInstance
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateWaterfallInstance(ACTORINSTANCE *actorInstance, uint32 textureName, uint32 sprayStart, uint32 sprayEnd)
{
	TBVector		velocity;
	TBMatrix		orientationMat;

	actorInstance->special = SPECIALTYPE_WATERFALL;
	actorInstance->waterfallInfo = (WATERFALLINFO*)ZALLOC(sizeof(WATERFALLINFO));
	ASSERTM(actorInstance->waterfallInfo, "waterfallinfo could not be malloced");

	//create velocity allong orientation of object
	bmQuatToMatrix(orientationMat, actorInstance->actorInstance.orientation);
	SETVECTOR(velocity, 0.0f, 0.0f, 1.0f, 1.0f);
	bmMatMultiplyVector(orientationMat, velocity);
	bmVectorScaleToLength(actorInstance->waterfallInfo->velocity, velocity, METERTOUNIT(2.0f));

	//find texture
//	if(textureName)
	{
		actorInstance->waterfallInfo->texture = bkLoadTexture(NULL, "wfall.tga", 0);
		actorInstance->waterfallInfo->highlight = bkLoadTexture(NULL, "wfallhighlight.tga", 0);
	}

	//set clock
	actorInstance->waterfallInfo->clock = WATERFALL_PERIOD;

	//bottom spray
	actorInstance->waterfallInfo->spray = bkLoadTexture(NULL, "wfallspray.tga", 0);
	actorInstance->waterfallInfo->sprayStart = sprayStart;
	actorInstance->waterfallInfo->sprayEnd = sprayEnd;

	actorInstance->waterfallInfo->soundHandle = -1;

	//PlayTrackingSample("loop\\waterfall.wav", 255, actorInstance->actorInstance.position, 0, 0, METERTOUNIT(10.0f), METERTOUNIT(20.5f));

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : UpdateWaterfallInstance
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateWaterfallInstance(ACTORINSTANCE *actorInstance)
{
	TBVector	position, velocity, scale, dist;
	TBQuaternion result;
	float		length, len;
	int			num;
	ACTORINSTANCE	*sprayInstance;

	// CMD: only update if taz or she devil is near
	for(int j = gameStatus.multiplayer.numPlayers;j > 0;--j)
	{
		if(gameStatus.player[j-1].actorInstance)
		{
			bmVectorSub(dist, actorInstance->actorInstance.position, gameStatus.player[j-1].actorInstance->actorInstance.position);
			if(len = bmVectorLen(dist) < METERTOUNIT(30.0f))
			{
				if (actorInstance->waterfallInfo->soundHandle == -1)
					actorInstance->waterfallInfo->soundHandle = PlayTrackingSample("waterfall.wav", 128, actorInstance->actorInstance.position, 0, 0, METERTOUNIT(10.0f), METERTOUNIT(20.5f));

				num = 1;
				if(len < METERTOUNIT(15.0f))
				{
					num = 2;
					if(len < METERTOUNIT(10.0f))
					{
						num = 3;
					}
				}

				if((actorInstance->waterfallInfo->clock -= fTime) < 0.0f)
				{
					actorInstance->waterfallInfo->clock = WATERFALL_PERIOD;
		
					for(int i = num;i > 0;--i)
					{
						//scale
						bmVectorCopy(scale, actorInstance->actorInstance.scale);
						scale[0] = actorInstance->actorInstance.scale[0] + ((float)(bmRand()%5)*0.1f);

						//now create spray
						if(actorInstance->waterfallInfo->sprayStart)
						{
							sprayInstance = FindActorInstanceInInstanceList(NULL,actorInstance->waterfallInfo->sprayStart,&map.invisibleInstances);
							if (sprayInstance)
							{
								bmVectorCopy(actorInstance->waterfallInfo->sprayPosition, sprayInstance->actorInstance.position);
								actorInstance->waterfallInfo->sprayStart = 0;
							}
							else
							{
								bkPrintf("*** ERROR *** Could not find waterfall start instance %X on waterfall %s\n",actorInstance->waterfallInfo->sprayStart,actorInstance->instanceName);
							}
						}
						if(actorInstance->waterfallInfo->sprayEnd)
						{
							sprayInstance = FindActorInstanceInInstanceList(NULL,actorInstance->waterfallInfo->sprayEnd,&map.invisibleInstances);
							if (sprayInstance)
							{
								bmVectorSub(actorInstance->waterfallInfo->sprayEndVector, sprayInstance->actorInstance.position, actorInstance->waterfallInfo->sprayPosition);
								actorInstance->waterfallInfo->vectorLen = (int)bmVectorLen(actorInstance->waterfallInfo->sprayEndVector);
								actorInstance->waterfallInfo->sprayEnd = 0;
							}
							else
							{
								bkPrintf("*** ERROR *** Could not find waterfall end instance %X on waterfall %s\n",actorInstance->waterfallInfo->sprayStart,actorInstance->instanceName);
							}
						}
						length = (float)(bmRand()%actorInstance->waterfallInfo->vectorLen);
						bmVectorScaleToLength(position, actorInstance->waterfallInfo->sprayEndVector, length);
						bmVectorAdd(position, actorInstance->waterfallInfo->sprayPosition, position);
						SETVECTOR(velocity, 0.0f, WATERFALL_SPRAYVEL, 0.0f, 1.0f);

						baCreateExtra(waterfallExtraSystem, 1, position, scale, velocity, 
							actorInstance->waterfallInfo->spray, result);
					}
				}
				return;			// CMD: don't want to create extras for both players
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : CreateWaterfallVerts
   Purpose : creates verts for waterfall fragment
   Parameters : 
   Returns : 
   Info : 
*/

void CreateWaterfallVerts(WATERFALLEXTRAINFO *info, TBVector position, TBVector scale, TBQuaternion quat)
{
	TBVector	vertPos;
	TBMatrix	rotation, scaleMat, translation, result;

	bmQuatToMatrix(rotation, quat);
	bmMatScale(scaleMat, scale[0], scale[1], scale[2]);
	bmMatTranslate(translation, position[0], position[1], position[2]);

	bmMatMultiply(result, scaleMat, rotation);
	bmMatMultiply(scaleMat, translation, result);

	//first vert
	SETVECTOR(vertPos, METERTOUNIT(1.0f), METERTOUNIT(-0.3f),METERTOUNIT(-1.0f), 1.0f);
	bmMatMultiplyVector(scaleMat, vertPos);

	BDVERTEX_SETXYZW(&info->verts[0],vertPos[0], vertPos[1], vertPos[2], vertPos[3]);
	BDVERTEX_SETNORMAL(&info->verts[0], 0.0f, 1.0f, 0.0f);
	BDVERTEX_SETRGBA(&info->verts[0], 128.0f, 128.0f, 128.0f, 0.0f);
	BDVERTEX_SETUV(&info->verts[0], 1.0f, 1.0f);

	//second vert
	SETVECTOR(vertPos, METERTOUNIT(-1.0f), METERTOUNIT(-0.3f), METERTOUNIT(-1.0f), 1.0f);
	bmMatMultiplyVector(scaleMat, vertPos);

	BDVERTEX_SETXYZW(&info->verts[1],vertPos[0], vertPos[1], vertPos[2], vertPos[3]);
	BDVERTEX_SETNORMAL(&info->verts[1], 0.0f, 1.0f, 0.0f);
	BDVERTEX_SETRGBA(&info->verts[1], 128.0f, 128.0f, 128.0f, 0.0f);
	BDVERTEX_SETUV(&info->verts[1], 0.0f, 1.0f);

	//third vert
	SETVECTOR(vertPos, METERTOUNIT(1.0f), 0.0f, METERTOUNIT(0.0f), 1.0f);
	bmMatMultiplyVector(scaleMat, vertPos);

	BDVERTEX_SETXYZW(&info->verts[2], vertPos[0], vertPos[1], vertPos[2], vertPos[3]);
	BDVERTEX_SETNORMAL(&info->verts[2], 0.0f, 1.0f, 0.0f);
	BDVERTEX_SETRGBA(&info->verts[2], 128.0f, 128.0f, 128.0f, 0.0f);
	BDVERTEX_SETUV(&info->verts[2], 1.0f, 0.0f);

	//fourth vert
	SETVECTOR(vertPos, METERTOUNIT(-1.0f), 0.0f, METERTOUNIT(0.0f), 1.0f);
	bmMatMultiplyVector(scaleMat, vertPos);

	BDVERTEX_SETXYZW(&info->verts[3], vertPos[0], vertPos[1], vertPos[2], vertPos[3]);
	BDVERTEX_SETNORMAL(&info->verts[3], 0.0f, 1.0f, 0.0f);
	BDVERTEX_SETRGBA(&info->verts[3], 128.0f, 128.0f, 128.0f, 0.0f);
	BDVERTEX_SETUV(&info->verts[3], 0.0f, 0.0f);
}

/* --------------------------------------------------------------------------------
   Function : InitWaterfallExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitWaterfallExtraSystem(void)
{
	waterfallExtraSystem = baCreateExtraSystem("waterfall",WaterfallExtraCreateCallback,
		WaterfallExtraUpdateCallback,WaterfallExtraDeleteCallback,WaterfallExtraRenderCallback,
		WaterfallExtraSystemInitCallback,sizeof(WATERFALLEXTRAINFO),MAX_WATERFALLPARTS,0,0);
}

/* --------------------------------------------------------------------------------
   Function : WaterfallExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void WaterfallExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms)
{
    WATERFALLEXTRAINFO		*data = (WATERFALLEXTRAINFO*)vData;
	float		*position;
	float		*scale;
	float		*velocity;
	float		*rotation;
	TBQuaternion	rot;

    if(parms)
	{
		numWfallPolys++;
		data->type = (uint32)va_arg(parms, int);
		position = va_arg(parms, float*);
		scale = va_arg(parms, float*);
		velocity = va_arg(parms, float*);
		data->texture = (TBTexture*)va_arg(parms, TBTexture*);
		rotation = va_arg(parms, float*);

		switch(data->type)
		{
		case WATERFALL_SPLASH:
			bmVectorCopy(data->velocity, velocity);
			bmVectorCopy(data->trailVelocity, velocity);
			bmQuatCopy(rot, rotation);
			CreateWaterfallVerts(data, position, scale, rot);

			data->life = WATERFALL_LIFE;
			data->trailClock = WATERFALL_DELTA;
			data->updateTrail = FALSE;
			data->horizSpeedFactor = 0.5f + ((float)(bmRand()%25)/100.0f);
			data->alphaState = 0;
			break;
		case WATERFALL_SPRAY:
			bmVectorCopy(data->position, position);
			bmVectorCopy(data->velocity, velocity);
			data->angle = 0.0f;
			data->life = 2.0f;
			data->xDim = METERTOUNIT(0.3f)*scale[1];
			data->angVel = RAD((float)(WATERFALL_SPRAYANGVEL + (bmRand()%WATERFALL_SPRAYANGVEL)));
			data->alpha = 70.0f;
			break;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : WaterfallExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int WaterfallExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata)
{
	WATERFALLEXTRAINFO *data = (WATERFALLEXTRAINFO*)vdata;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : WaterfallExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void WaterfallExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t)
{
	WATERFALLEXTRAINFO		*data;
	TBVector				positionInc, position;
	float					a;
	int						j;
	
	for (int32 i=0;i<noofInstances;)
	{
		data = (WATERFALLEXTRAINFO*)dataPtrs[i];

		if((data->life -= fTime) > 0.0f)
		{
			switch(data->type)
			{
			case WATERFALL_SPLASH:
				//update first two verts
				bmVectorCMul(positionInc, data->velocity, fTime);
				for(j = 0;j < 2;j++)
				{
					BDVERTEX_GETXYZW(&data->verts[j], position[0], position[1], position[2], position[3]);
					bmVectorAdd(position, position, positionInc);
					BDVERTEX_SETXYZW(&data->verts[j], position[0], position[1], position[2], position[3]);
				}
				ApplyGravity(positionInc, data->velocity, METERTOUNIT(-20.0f));
				//update last two verts
				bmVectorCMul(positionInc, data->trailVelocity, fTime*data->horizSpeedFactor);
				for(j = 2;j < 4;j++)
				{
					BDVERTEX_GETXYZW(&data->verts[j], position[0], position[1], position[2], position[3]);
					bmVectorAdd(position, position, positionInc);
					BDVERTEX_SETXYZW(&data->verts[j], position[0], position[1], position[2], position[3]);
				}
				ApplyGravity(positionInc, data->trailVelocity, METERTOUNIT(-5.0f));

				//update alpha
				for(j = 0;j < 4;j++)
				{
					switch(data->alphaState)
					{
					case 0:
						BDVERTEX_GETA(&data->verts[j], a);
						if((a += 70.0f*fTime) > 60.0f)
						{
							a = 60.0f;
							data->alphaState = 1;
						}
						BDVERTEX_SETA(&data->verts[j], a);
						break;
					case 1:
						if(j<2)
						{
							BDVERTEX_GETA(&data->verts[j], a);
							if((a -= 128.0f*fTime) < 0.0f)
							{
								a = 0.0f;
							}
							BDVERTEX_SETA(&data->verts[j], a);
						}
						break;
					}
				}
				break;
			case WATERFALL_SPRAY:
				//update position
				bmVectorCMul(positionInc, data->velocity, fTime);
				bmVectorAdd(data->position, data->position, positionInc);

				//update scale
				data->xDim += WATERFALL_SPRAYSCALEVEL*fTime;
				//update angle
				data->angle += data->angVel*fTime;
				//update alpha
				if((data->alpha -= 30.0f*fTime) < 0.0f)
				{
					data->alpha = 0.0f;
				}
				break;
			}
			i++;
		}
		else
		{
			numWfallPolys--;
			baDeleteExtraInstance(eSystem, i);
			noofInstances--;
			continue;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : WaterfallExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void WaterfallExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context)
{
	WATERFALLEXTRAINFO *data;
	int	i;
	int32		bufIndex;
	TBTexture	*texture;

	if((int32)context != EXTRA_ALPHA) return;

	// PP: less-than z-test
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, 0);

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, TRUE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetIdentityObjectMatrix();

	bufIndex = 0;
	texture = NULL;

	for (i=0;i<noofInstances;i++)
	{
		data = (WATERFALLEXTRAINFO*)dataPtrs[i];

		switch(data->type)
		{
		case WATERFALL_SPLASH:
			bdSetTexture(0, data->texture);
			bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, data->verts, 4, BVERTTYPE_SINGLE);
			continue;
		case WATERFALL_SPRAY:
			data->position[W] = 1.0f;
			if(bdDrawFlatSprite(data->position, data->xDim, data->xDim,data->angle,data->texture,
								0,0,100,105,127,bmFloatToInt(data->alpha)<<1,&global2DVertexBuffer[bufIndex],0))
			{
				bufIndex += 4;
			}
			if(!texture) texture = data->texture;
			continue;
		}
	}
	
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);
	bdSetIdentityObjectMatrix();

	bdSetTexture(0, texture);
	if(bufIndex) bdDrawPrimitive(BDPRIMTYPE_QUADLIST, global2DVertexBuffer, bufIndex,BVERTTYPE_SINGLE2D);
	
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, TRUE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
}

/* --------------------------------------------------------------------------------
   Function : WaterfallExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/

void WaterfallExtraSystemInitCallback(TBExtraSystem *eSystem, int reason)
{
}