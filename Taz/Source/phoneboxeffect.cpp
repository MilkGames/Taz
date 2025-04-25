// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : phoneboxeffects.cpp
//   Purpose : functions for effects surrounding phonebox
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "actors.h"
#include "maths.h"
#include "main.h"
#include "phonebox.h"
#include "textures.h"
#include "phoneboxeffect.h"

TBExtraSystem		*phoneBoxEffectExtraSystem;

/* --------------------------------------------------------------------------------
   Function : StartPhoneBoxEffect
   Purpose : call this function to start the effect
   Parameters : 
   Returns : 
   Info : 
*/

int StartPhoneBoxEffect(ACTORINSTANCE *actorInstance)
{
	PHONEBOXEFFECTINFO	*tempInfo;

	if((actorInstance->special != SPECIALTYPE_PHONEBOX)||(actorInstance->phoneBoxInfo == NULL)) return FALSE;

	tempInfo = (PHONEBOXEFFECTINFO*)ZALLOC(sizeof(PHONEBOXEFFECTINFO));
	if(!tempInfo) return FALSE;

	tempInfo->clock = 0.5f; //PHONEFX_ORANGETIME;
	tempInfo->orange = texturePhoneOrange;
	tempInfo->white = texturePhoneWhite;
	tempInfo->node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "receiver");
	actorInstance->phoneBoxInfo->effectInfo = tempInfo;
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : UpdatePhoneBoxEffect
   Purpose : call this function to update the effect
   Parameters : 
   Returns : 
   Info : 
*/

void UpdatePhoneBoxEffect(ACTORINSTANCE *actorInstance)
{
	TBVector		tempVector;
	TBTexture		*texture;
	float			speed, size, angVel;

	if(actorInstance->phoneBoxInfo->effectInfo)
	{
		if((actorInstance->phoneBoxInfo->effectInfo->clock -= fTime) < 0.0f)
		{
			speed = PHONEFX_MAXVERTSPEED;
			size = METERTOUNIT(1.2f);
			actorInstance->phoneBoxInfo->effectInfo->clock = 0.5f;
			if(bmRand()%2)
			{
				angVel = -PHONEFX_ANGVELOCITY;
//				bmVectorCopy(tempVector, actorInstance->phoneBoxInfo->actorInstance->actorInstance.position);
				bmVectorCopy(tempVector, actorInstance->actorInstance.position);
				texture = actorInstance->phoneBoxInfo->effectInfo->orange;
				baCreateExtra(phoneBoxEffectExtraSystem,tempVector,texture,&speed,&size,&angVel);
			}
			else
			{
				angVel = PHONEFX_ANGVELOCITY;
//				bmVectorCopy(tempVector, actorInstance->phoneBoxInfo->actorInstance->actorInstance.position);
				bmVectorCopy(tempVector, actorInstance->actorInstance.position);
				texture = actorInstance->phoneBoxInfo->effectInfo->white;
				baCreateExtra(phoneBoxEffectExtraSystem,tempVector,texture,&speed,&size,&angVel);
			}
		}
//		if(actorInstance->phoneBoxInfo->effectInfo->clock < 0.0f)
//		{
//			EndPhoneBoxEffect(actorInstance);
//		}
	}
}

/* --------------------------------------------------------------------------------
   Function : EndPhoneBoxEffect
   Purpose : call this function to end the effect
   Parameters : 
   Returns : 
   Info : 
*/

void EndPhoneBoxEffect(ACTORINSTANCE *actorInstance)
{
	if ((actorInstance->phoneBoxInfo)&&(actorInstance->phoneBoxInfo->effectInfo))
	{
		actorInstance->phoneBoxInfo->effectInfo->node = NULL;
		actorInstance->phoneBoxInfo->effectInfo->orange = NULL;
		actorInstance->phoneBoxInfo->effectInfo->white = NULL;
		FREE(actorInstance->phoneBoxInfo->effectInfo);
		actorInstance->phoneBoxInfo->effectInfo = NULL;
	}
}

/* --------------------------------------------------------------------------------
   Function : InitPhoneBoxExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitPhoneBoxExtraSystem(void)
{
	phoneBoxEffectExtraSystem = baCreateExtraSystem("phoneBox",PhoneBoxExtraCreateCallback,
		PhoneBoxExtraUpdateCallback,PhoneBoxExtraDeleteCallback,PhoneBoxExtraRenderCallback,
		PhoneBoxPhoneBoxExtraSystemInitCallback,sizeof(PHONEBOXEXTRAS),MAX_PHONEBOXEFFECTS,0,0);
}

/* --------------------------------------------------------------------------------
   Function : PhoneBoxExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void PhoneBoxExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms)
{
    PHONEBOXEXTRAS		*data = (PHONEBOXEXTRAS*)vData;
	float				*position;
	TBTexture			*texture;
	float				verticalSpeed, currentAngle, tempFloat;
	float				angularVelocity, initialRadius;
	int					i;

    if(parms)
	{
		position = va_arg(parms, float*);
		texture = (TBTexture*)va_arg(parms, TBTexture*);
		verticalSpeed = *va_arg(parms, float*);
		initialRadius = *va_arg(parms, float*);
		angularVelocity = *va_arg(parms, float*);

		data->initialPosition[0] = position[0];
		data->initialPosition[1] = position[1];
		data->initialPosition[2] = position[2];
		data->initialPosition[3] = 1.0f;

		data->angularVel = angularVelocity;
		data->verticalSpeed = verticalSpeed;
		data->radialVelocity = PHONEFX_RADIALVELOCITY;

		currentAngle = RAD((float)(bmRand()%360));

		for(i=0;i<PHONEFX_NUMNODES;i++)
		{
			bmVectorCopy(data->nodes[i], data->initialPosition);

			if(i)
			{
				data->colour[i][3] = 80;
			}
			tempFloat = ((float)i*PHONEFX_ANGULAROFFSET*(angularVelocity/bmFAbs(angularVelocity)));
			data->currentRot[i] = currentAngle - tempFloat;
			data->height[i] = verticalSpeed*-tempFloat/angularVelocity;
			data->radius[i] = initialRadius;

			data->colour[i][0] = 127;
			data->colour[i][1] = 127;
			data->colour[i][2] = 127;
			data->colour[0][3] = 127;

			data->size[i] = METERTOUNIT(0.5f) / (float)(i+1);
		}
		data->texture = texture;
		data->life = 2.0f;
	}
}

/* --------------------------------------------------------------------------------
   Function : PhoneBoxExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int PhoneBoxExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata)
{
	PHONEBOXEXTRAS *data = (PHONEBOXEXTRAS*)vdata;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : PhoneBoxExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void PhoneBoxExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t)
{
	PHONEBOXEXTRAS		*data;
	int					i, j;
	TBMatrix			rotation, worldTrans, result;
	float				angleInc, heightInc, radiusInc;
	
	for (i=0;i<noofInstances;i++)
	{
		data = (PHONEBOXEXTRAS*)dataPtrs[i];

//		if((data->colour[0][3] -= (int)(PHONEFX_FADERATE*128.0f*fTime)) > 0)
		if((data->life -= fTime) > 0.0f)
		{
			if(data->life < 1.0f)
			{
				data->colour[0][3] -= (int)(127.0f*fTime);
				for(j=1;j<PHONEFX_NUMNODES;j++)
				{
					data->colour[j][3] = data->colour[0][3];
				}
			}
			angleInc = data->angularVel*fTime;
			heightInc = data->verticalSpeed*fTime;
			radiusInc = data->radialVelocity*fTime;
			bmMatTranslate(worldTrans, data->initialPosition[0], data->initialPosition[1], data->initialPosition[2]);
			for(j=0;j<PHONEFX_NUMNODES;j++)
			{
				//increment values
				data->currentRot[j] += angleInc;
				data->height[j] += heightInc;
				data->radius[j] += radiusInc;

				//calculate new position
				SETVECTOR(data->nodes[j], 0.0f, data->height[j], data->radius[j], 1.0f);
				bmMatYRotation(rotation, data->currentRot[j]);

				bmMatMultiply(result, worldTrans, rotation);
				bmMatMultiplyVector(result, data->nodes[j]);
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
   Function : PhoneBoxExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void PhoneBoxExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context)
{
	PHONEBOXEXTRAS *data;
	int				i, j, noofVerts;
	float			screenSize[PHONEFX_NUMNODES];
	TBPrimVertex2D	destVerts[((PHONEFX_NUMNODES*4)+4)];

	if((int32)context != EXTRA_ALPHA) return;

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	for (i=0;i<noofInstances;i++)
	{
		data = (PHONEBOXEXTRAS*)dataPtrs[i];

		for(j=0;j<PHONEFX_NUMNODES;j++)
		{
			data->nodes[j][3] = 1.0f;
		}

		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
		noofVerts = bdDrawMultiStreakClipped(PHONEFX_NUMNODES,data->nodes[0],data->size,screenSize,&data->colour[0][0],destVerts, 0.0f);

		if(noofVerts)
		{
			bdSetTexture(0, data->texture);
			bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);
			bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, destVerts, noofVerts, BVERTTYPE_SINGLE2D);
		}
	}

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
}

/* -------------------------------------------------------------------------------
   Function : FireExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/
void PhoneBoxPhoneBoxExtraSystemInitCallback(TBExtraSystem *eSystem, int reason)
{
}