// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : watersplash.cpp
//   Purpose : functions to create splash effects
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "main.h"
#include "actors.h"
#include "physics.h"
#include "textures.h"
#include "util.h"
#include "waterrings.h"
#include "watersplash.h"
#include "display.h"			// PP: provide basic display setup and managment functions

TBExtraSystem		*splashExtraSystem;
TBExtraSystem		*squishExtraSystem;

/* --------------------------------------------------------------------------------
   Function : InitSplashExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/

void CreateWaterSplashes(TBVector position, float scale)
{
	TBVector		velocity;
	float			size, sizeInc, initial, angle;
	uint32			flag;
	int				i;

	// CMD: start with a large initial size, and decrease size by an increasing amount
	initial = size = METERTOUNIT(0.2f);
	sizeInc = size/(NUM_WATER_SPLASH_PARTS*2.0f);

	for(i = (NUM_WATER_SPLASH_PARTS-1);i >= 0;--i)
	{
		angle = UtilGenerateRandomNumber(RAD(360.0f), 0.0f);
		SETVECTOR(velocity, bmSin(angle), 0.0f, bmCos(angle), 1.0f);
		bmVectorScaleToLength(velocity, velocity, HORIZ_SPLASH_SPEED*initial/size);
		velocity[1] = VERT_SPLASH_SPEED*initial/size;
	
		if(i%4) flag = 0;
		else flag = WATERSPLASH_CREATERIPPLE;

		bmVectorCMul(velocity, velocity, scale);
		size *= scale;
		baCreateExtra(splashExtraSystem, position, velocity, &size, flag);

		// CMD: decrement size ready for next loop
		size -= sizeInc;
		// CMD: increment sizeInc
		sizeInc *= 1.1f;
	}
}

/* --------------------------------------------------------------------------------
   Function : InitSplashExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/

void InitSplashExtraSystem(void)
{
	splashExtraSystem = baCreateExtraSystem("splash",SplashExtraCreateCallback,
		SplashExtraUpdateCallback,SplashExtraDeleteCallback,SplashExtraRenderCallback,
		SplashExtraSystemInitCallback,sizeof(SPLASHINFO),MAX_SPLASHES,0,0);
}

/* --------------------------------------------------------------------------------
   Function : SplashExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/

void SplashExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms)
{
    SPLASHINFO		*data = (SPLASHINFO*)vData;
	float			*position;
	float			*velocity;

    if(parms)
	{
		position = va_arg(parms, float*);
		bmVectorCopy(data->position, position);
		velocity = va_arg(parms, float*);
		bmVectorCopy(data->velocity, velocity);
		data->size = *va_arg(parms, float*);
		data->flags = (uint32)va_arg(parms, int);

		data->colour.r = data->colour.g = data->colour.b = 127;
		data->colour.a = 40.0f;
		data->height = 0.0f;
	}
}

/* --------------------------------------------------------------------------------
   Function : SplashExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int SplashExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata)
{
	SPLASHINFO *data = (SPLASHINFO*)vdata;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : SplashExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void SplashExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t)
{
	SPLASHINFO		*data;
	TBVector		posInc;	
	int				i;
	float			gravity;
	
	gravity = worldGravity*0.5f;

	for (i=0;i<noofInstances;i++)
	{
		data = (SPLASHINFO*)dataPtrs[i];

		if(data->height < 0.0f)
		{
			if(data->flags & WATERSPLASH_CREATERIPPLE)
			{
				CreateWaterRing(data->position, NULL, 0.8f);
			}
			baDeleteExtraInstance(eSystem, i);
			noofInstances--;
			continue;
		}
		else
		{
			ApplyGravity(posInc, data->velocity, gravity);
			bmVectorAdd(data->position, data->position, posInc);

			data->height += posInc[1];
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : SplashExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void SplashExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context)
{
	SPLASHINFO			*data;
	TBMatrix			twoPlayerScale;
	int					i, bufIndex;

	if((int32)context != EXTRA_ALPHA) return;

	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	
	bufIndex = 0;
	for (i=0;i<noofInstances;i++)
	{
		data = (SPLASHINFO*)dataPtrs[i];
		
		data->position[3] = 1.0f;

		bdSetIdentityObjectMatrix();
		if(bdDrawFlatSprite(data->position,data->size,data->size,0.0f,textureWaterSplash,0,0,
							data->colour.r,data->colour.g,data->colour.b,bmFloatToInt(data->colour.a)<<1,&global2DVertexBuffer[bufIndex],0.0f))
		{
			bufIndex += 4;
		}
	}

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);

	// PP: set an object matrix to correct the appearance of the sprites
	setSpriteObjMat();

	bdSetTexture(0, textureWaterSplash);
	if(bufIndex) bdDrawPrimitive(BDPRIMTYPE_QUADLIST, global2DVertexBuffer, bufIndex, BVERTTYPE_SINGLE2D);

	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, TRUE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
}

/* --------------------------------------------------------------------------------
   Function : SplashExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/
void SplashExtraSystemInitCallback(TBExtraSystem *eSystem, int reason)
{
}

/* --------------------------------------------------------------------------------
   Function : IntVectorCopy
   Purpose : copies a vector made up from integers
   Parameters : 
   Returns : 
   Info : 
*/

void IntVectorCopy(int dest[4], int source[4])
{
	dest[0] = source[0];
	dest[1] = source[1];
	dest[2] = source[2];
	dest[3] = source[3];
}

/* --------------------------------------------------------------------------------
   Function : InitFountainExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/

void InitSquishExtraSystem(void)
{
	squishExtraSystem = baCreateExtraSystem("squish",SquishExtraCreateCallback,
		SquishExtraUpdateCallback,SquishExtraDeleteCallback,SquishExtraRenderCallback,
		SquishExtraSystemInitCallback,sizeof(SQUISHEXTRAINFO),100,0,0);
}

/* --------------------------------------------------------------------------------
   Function : SquishExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : leadingPosition, velocity, leadingDim, trailingDim, life
   Info : 
*/

void SquishExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms)
{
    SQUISHEXTRAINFO		*data = (SQUISHEXTRAINFO*)vData;
	float		*leadingPosition;
	float		*velocity;
	float		leadingDim, trailingDim, life;

    if(parms)
	{
		leadingPosition = va_arg(parms, float*);
		velocity = va_arg(parms, float*);
		leadingDim = *va_arg(parms, float*);
		trailingDim = *va_arg(parms, float*);
		life = *va_arg(parms, float*);

		bmVectorCopy(data->leadingPos, leadingPosition);
		bmVectorCopy(data->trailingPos, leadingPosition);
		bmVectorCopy(data->velocity, velocity);
		data->leadingDim = leadingDim;
		data->trailingDim = trailingDim;
		data->life = life;
		data->alpha = 100.0f;
	}
}

/* --------------------------------------------------------------------------------
   Function : SquishExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int SquishExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata)
{
	SQUISHEXTRAINFO *data = (SQUISHEXTRAINFO*)vdata;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : SquishExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void SquishExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t)
{
	SQUISHEXTRAINFO	*data;
	TBVector			posInc;
	int					i;

	for (i=0;i<noofInstances;i++)
	{
		data = (SQUISHEXTRAINFO*)dataPtrs[i];

		if((data->life -= 1.0f*fTime) < 0.0f)
		{
			baDeleteExtraInstance(eSystem, i);
			noofInstances--;
			continue;
		}
		else
		{
			ApplyGravity(posInc, data->velocity, worldGravity*0.25f);
			bmVectorCopy(data->trailingPos, data->leadingPos);
			bmVectorAdd(data->leadingPos, data->leadingPos, posInc);

			if((data->alpha -= 140.0f*fTime) < 0.0f) data->alpha = 0.0f;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : SquishExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void SquishExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context)
{
	SQUISHEXTRAINFO	*data;
	TBMatrix		twoPlayerScale;
	int				white[4] = {127, 127, 127, 80};
	int				i, bufIndex;

	if((int32)context != EXTRA_ALPHA) return;

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);

	bufIndex = 0;
	for (i=0;i<noofInstances;i++)
	{
		data = (SQUISHEXTRAINFO*)dataPtrs[i];
		
		data->leadingPos[3] = data->trailingPos[3] = 1.0f;
		white[3] = bmFloatToInt(data->alpha);

		if(bdDrawStreakClipped(data->leadingPos, data->leadingDim, data->leadingDim, white,
					data->trailingPos, data->trailingDim, data->trailingDim, white,
					NULL, &global2DVertexBuffer[bufIndex], 0.0f))
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
#if BPLATFORM == PS2
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
#else
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);
#endif
		
		// PP: set an object matrix to correct the appearance of the sprites
		setSpriteObjMat();

		bdSetTexture(0, textureWaterSplash);
		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, global2DVertexBuffer, (bufIndex-2),BVERTTYPE_SINGLE2D);

		bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	}
}

/* --------------------------------------------------------------------------------
   Function : SquishExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/
void SquishExtraSystemInitCallback(TBExtraSystem *eSystem, int reason)
{
}