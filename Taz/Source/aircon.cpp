// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : fireworks.cpp
//   Purpose : creates, updates, and draws firework effects
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"
#include "global.h"

#include "main.h"
#include "util.h"
#include "textures.h"
#include "aircon.h"

TBExtraSystem		*airconExtraSystem;

/* --------------------------------------------------------------------------------
   Function : CreateAirconUnit
   Purpose : Creates an air conditioning unit at the specified point, in the specified orientation
   Parameters : 
   Returns : 
   Info : 
*/

void CreateAirconUnit(ACTORINSTANCE *actorInstance)
{
	TBMatrix		matrix;

	actorInstance->airconunitInfo = (AIRCONUNITINFO*)ZALLOC(sizeof(AIRCONUNITINFO));
	ASSERTM(actorInstance->airconunitInfo, "out of memory");

	actorInstance->special = SPECIALTYPE_AIRCON;

	bmQuatToMatrix(matrix, actorInstance->actorInstance.orientation);
	bmVectorSet(actorInstance->airconunitInfo->direction, 0.0f, 0.0f, 1.0f, 1.0f);
	bmMatMultiplyVector(matrix, actorInstance->airconunitInfo->direction);

	actorInstance->airconunitInfo->clock = AIRCON_PERIOD;
}

/* --------------------------------------------------------------------------------
   Function : UpdateAirconUnit
   Purpose : Creates an air conditioning unit at the specified point, in the specified orientation
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateAirconUnit(ACTORINSTANCE *actorInstance)
{
	TBVector		velocity;
	TBVector		tazDist;
	float			length;
	AIRCONUNITINFO	*info = actorInstance->airconunitInfo;

	if(actorInstance->airconunitInfo)
	{
		bmVectorSub(tazDist, actorInstance->actorInstance.position, gameStatus.player[0].camera->cameraState.cameraPos);
		// CMD: cull completely if camera too far away
		if((length = bmVectorLen(tazDist)) < AIRCON_THRESHOLD)
		{
			if((actorInstance->airconunitInfo->clock -= fTime) < 0.0f)
			{
				// CMD: base frequency of creation upon distance betweet camera and air con unit
				actorInstance->airconunitInfo->clock = -(AIRCON_PERIOD*length/AIRCON_THRESHOLD) + AIRCON_PERIOD;

				// CMD: calculate velocity
				bmVectorScaleToLength(velocity, actorInstance->airconunitInfo->direction, UtilGenerateRandomNumber(AIRCON_MAX_SPEED, AIRCON_MAX_SPEED*0.5f));

				baCreateExtra(airconExtraSystem, actorInstance->actorInstance.position, velocity);
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : InitAirconExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitAirconExtraSystem(void)
{
	airconExtraSystem = baCreateExtraSystem("aircon",AirconExtraCreateCallback,
		AirconExtraUpdateCallback,AirconExtraDeleteCallback,AirconExtraRenderCallback,
		AirconExtraSystemInitCallback,sizeof(AIRCONINFO),MAX_AIRCON,0,0);
}

/* --------------------------------------------------------------------------------
   Function : AirconExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void AirconExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms)
{
    AIRCONINFO		*data = (AIRCONINFO*)vData;
	float			*position;
	float			*velocity;

    if(parms)
	{
		position = va_arg(parms, float*);
		bmVectorCopy(data->position, position);
		velocity = va_arg(parms, float*);
		bmVectorCopy(data->velocity, velocity);

		data->alpha = 0.0f;
		data->alphaUp = TRUE;
		data->angle = UtilGenerateRandomNumber(RAD(360.0f), 0.0f);
		data->rotSpeed = UtilGenerateRandomNumber(RAD(45.0f), RAD(-45.0f));
		data->size = METERTOUNIT(0.5);
	}
}

/* --------------------------------------------------------------------------------
   Function : AirconExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int AirconExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata)
{
	AIRCONINFO *data = (AIRCONINFO*)vdata;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : AirconExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void AirconExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t)
{
	AIRCONINFO		*data;
	TBVector		posInc;
	
	for (int32 i=0;i<noofInstances;i++)
	{
		data = (AIRCONINFO*)dataPtrs[i];

		// CMD: update position
		bmVectorCMul(posInc, data->velocity, fTime);
		bmVectorAdd(data->position, data->position, posInc);

		// CMD: update angle
		data->angle += data->rotSpeed*fTime;

		// CMD: do we increment or decrement the alpha?
		switch(data->alphaUp)
		{
		case TRUE:
			if((data->alpha += 256.0f*fTime) > 70.0f)
			{
				data->alpha = 70.0f;
				data->alphaUp = FALSE;
			}
			break;
		case FALSE:
			if((data->alpha -= 60.0f*fTime) < 0.0f)
			{
				data->alpha = 0.0f;
				baDeleteExtraInstance(eSystem, i);
				noofInstances--;
				continue;
			}
			break;
		}
		// CMD: always decrement the size
		if((data->size -= UtilGenerateRandomNumber(METERTOUNIT(0.4f), 0.0f)*fTime) < 0.0f)
		{
			data->size = 0.0f;
			baDeleteExtraInstance(eSystem, i);
			noofInstances--;
			continue;
		}
		// CMD: decrement the velocity ready for next frame
		bmVectorScaleToLength(posInc, data->velocity, -(AIRCON_MAX_SPEED*0.5f*fTime));
		bmVectorAdd(data->velocity, data->velocity, posInc);
	}
}

/* --------------------------------------------------------------------------------
   Function : AirconExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void AirconExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context)
{
	AIRCONINFO		*data;
	int32			i, bufIndex;


	if((int32)context != EXTRA_ALPHA) return;

	bufIndex = 0;
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);

	for (i=0;i<noofInstances;i++)
	{
		data = (AIRCONINFO*)dataPtrs[i];
		data->position[3] = 1.0f;

		if(bdDrawFlatSprite(data->position,data->size,data->size,data->angle,NULL,0,0,127,127,127,bmFloatToInt(data->alpha)<<1,&global2DVertexBuffer[bufIndex],0))
		{
			bufIndex += 4;
		}
	}

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);

	bdSetIdentityObjectMatrix();

	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);

	bdSetTexture(0, textureWaterfallSpray);
	if(bufIndex) bdDrawPrimitive(BDPRIMTYPE_QUADLIST, global2DVertexBuffer, bufIndex, BVERTTYPE_SINGLE2D);


	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, TRUE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
}

/* --------------------------------------------------------------------------------
   Function : AirconExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/

void AirconExtraSystemInitCallback(TBExtraSystem *eSystem, int reason)
{
}
