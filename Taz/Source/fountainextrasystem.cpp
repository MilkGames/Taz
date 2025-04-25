// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : fountainextrasystem.cpp
//   Purpose : 
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "main.h"
#include "maths.h"
#include "physics.h"
#include "actors.h"
#include "util.h"
#include "textures.h"
#include "fountainextrasystem.h"

TBExtraSystem		*fountainExtraSystem;

/* --------------------------------------------------------------------------------
   Function : InitFountainExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/

void InitFountainExtraSystem(void)
{
	fountainExtraSystem = baCreateExtraSystem("fountain",FountainExtraCreateCallback,
		FountainExtraUpdateCallback,FountainExtraDeleteCallback,FountainExtraRenderCallback,
		FountainExtraSystemInitCallback,sizeof(FOUNTAINEXTRAINFO),MAX_FOUNTAINSPLASHES,0,0);
}

/* --------------------------------------------------------------------------------
   Function : FountainExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : leadingPosition, leadingDim, trailingDim, height, FOUNTAINEXTRA_WAYDOWN);
   Info : 
*/

void FountainExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms)
{
    FOUNTAINEXTRAINFO		*data = (FOUNTAINEXTRAINFO*)vData;
	ACTORINSTANCE	*actorInstance;
	float		*leadingPosition;
	TBVector	velocity;
	float		leadingDim, trailingDim;
	float		height, life;
	uint32		state;

    if(parms)
	{
		leadingPosition = va_arg(parms, float*);
		leadingDim = *va_arg(parms, float*);
		trailingDim = *va_arg(parms, float*);
		height = *va_arg(parms, float*);
		life = *va_arg(parms, float*);
		actorInstance = (ACTORINSTANCE*)va_arg(parms, void*);
		state = (uint32)va_arg(parms, int);

		bmVectorCopy(data->leadingPos, leadingPosition);
		leadingPosition[1] -=METERTOUNIT(0.2f);
		bmVectorCopy(data->trailingPos, leadingPosition);
		data->height = leadingPosition[1] + height;
		data->leadingDim = leadingDim;
		data->trailingDim = trailingDim;
		data->state = state;
		data->life = life;
		data->texture = textureWaterSplash;
		data->instanceOnFountain = actorInstance;
		if(state == FOUNTAINEXTRA_WAYUP)
		{
			SETVECTOR(velocity, 0.0f, UtilGenerateRandomNumber(FOUNTAINEXTRA_WAYUPSPEED, FOUNTAINEXTRA_WAYUPSPEED*0.75f), 0.0f, 1.0f);
			bmVectorCopy(data->velocity, velocity);
		}
		if(state == FOUNTAINEXTRA_WAYDOWN)
		{
			SETVECTOR(velocity, (float)((bmRand()%FOUNTAINEXTRA_HORIZSPEED)-(FOUNTAINEXTRA_HORIZSPEED/2)), 
				FOUNTAINEXTRA_UPSPEED, (float)((bmRand()%FOUNTAINEXTRA_HORIZSPEED)-(FOUNTAINEXTRA_HORIZSPEED/2)), 1.0f);
			bmVectorCopy(data->velocity, velocity);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : FountainExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int FountainExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata)
{
	FOUNTAINEXTRAINFO *data = (FOUNTAINEXTRAINFO*)vdata;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : FountainExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void FountainExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t)
{
	FOUNTAINEXTRAINFO	*data;
	TBVector			tempVelocity;
	int					i;

	for (i=0;i<noofInstances;i++)
	{
		data = (FOUNTAINEXTRAINFO*)dataPtrs[i];

		if(data->state == FOUNTAINEXTRA_WAYUP)
		{
			if(data->instanceOnFountain)
			{
				if(data->leadingPos[1] < data->instanceOnFountain->actorInstance.position[1])
				{
					bmVectorCDiv(tempVelocity, data->velocity, fps);
					bmVectorAdd(data->leadingPos, data->leadingPos, tempVelocity);
					bmVectorAdd(data->trailingPos, data->trailingPos, tempVelocity);
				}
				else
				{
					SETVECTOR(tempVelocity, (float)((bmRand()%FOUNTAINEXTRA_HORIZSPEED)-(FOUNTAINEXTRA_HORIZSPEED/2)), 
							FOUNTAINEXTRA_UPSPEED, (float)((bmRand()%FOUNTAINEXTRA_HORIZSPEED)-(FOUNTAINEXTRA_HORIZSPEED/2)), 1.0f);
					bmVectorCopy(data->velocity, tempVelocity);
					data->state = FOUNTAINEXTRA_WAYDOWN;
				}
			}
			else
			{
				if(data->leadingPos[1] < data->height)
				{
					bmVectorCMul(tempVelocity, data->velocity, fTime);
					bmVectorAdd(data->leadingPos, data->leadingPos, tempVelocity);
					bmVectorAdd(data->trailingPos, data->trailingPos, tempVelocity);
				}
				else
				{
					SETVECTOR(tempVelocity, (float)((bmRand()%FOUNTAINEXTRA_HORIZSPEED)-(FOUNTAINEXTRA_HORIZSPEED/2)), 
							FOUNTAINEXTRA_UPSPEED, (float)((bmRand()%FOUNTAINEXTRA_HORIZSPEED)-(FOUNTAINEXTRA_HORIZSPEED/2)), 1.0f);
					bmVectorCopy(data->velocity, tempVelocity);
					data->state = FOUNTAINEXTRA_WAYDOWN;
				}
			}
		}
		if(data->state == FOUNTAINEXTRA_WAYDOWN)
		{
			if((data->life -= 1.0f/fps) > 0.0f)
			{
				data->velocity[1] += worldGravity/fps;
				bmVectorCDiv(tempVelocity, data->velocity, fps);
				bmVectorCopy(data->trailingPos, data->leadingPos);
				bmVectorAdd(data->leadingPos, data->leadingPos, tempVelocity);
			}
			else
			{
				baDeleteExtraInstance(eSystem, i);
				noofInstances--;
				continue;
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : FountainExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void FountainExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context)
{
	FOUNTAINEXTRAINFO	*data;
	int			white[4] = {127, 127, 127, 80};
	int			i, bufIndex;

	if((int32)context != EXTRA_ALPHA) return;

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);

	bufIndex = 0;
	for (i=0;i<noofInstances;i++)
	{
		data = (FOUNTAINEXTRAINFO*)dataPtrs[i];
		
		data->leadingPos[3] = data->trailingPos[3] = 1.0f;

		if(bdDrawStreakClipped(data->leadingPos, data->leadingDim, data->leadingDim, white,
					data->trailingPos, data->trailingDim, data->trailingDim, white,
					data->texture, &global2DVertexBuffer[bufIndex], 0.0f))
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
	if(bufIndex)
	{
		bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, 0);
		bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);
		
		bdSetIdentityObjectMatrix();
		bdSetTexture(0, data->texture);
		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, global2DVertexBuffer, (bufIndex-2),BVERTTYPE_SINGLE2D);

		bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	}
}

/* --------------------------------------------------------------------------------
   Function : FountainExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/
void FountainExtraSystemInitCallback(TBExtraSystem *eSystem, int reason)
{
}