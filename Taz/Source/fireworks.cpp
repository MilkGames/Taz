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
#include "textures.h"
#include "maths.h"
#include "physics.h"
#include "fireworks.h"

float	fireworkColours[][3] = 
{
	{128.0f,128.0f,128.0f},
	{128.0f,0.0f,0.0f},
	{0.0f,128.0f,0.0f},
	{0.0f,0.0f,128.0f},
	{0.0f,128.0f,128.0f},
	{128.0f,0.0f,128.0f},
	{128.0f,128.0f,0.0f}
};

#define FIREWORKS_NUMCOLOURS	(7)

TBExtraSystem		*fireworksExtraSystem;

/* --------------------------------------------------------------------------------
   Function : CreateFireworkBurst
   Purpose : Creates a firework burst of a given type at the specified origin
   Parameters : scale (defaults to 1.0f)
   Returns : 
   Info : flags include random colours, particle colours, and types see header for details
*/

void CreateFireworkBurst(TBVector position, EFirework type, int32 r, int32 g, int32 b, uint32 flags, float scale)
{
	EFirework		style;
	float			red,green,blue;
	int32			index, number;

	if(type == FIREWORK_RANDOMTYPE) style = (EFirework)(bmRand()%(int32)FIREWORK_NUM_STYLES);
	else style = type;

	if(flags&FIREWORKS_RANDOMCOLOUR)
	{
		index = bmRand()%FIREWORKS_NUMCOLOURS;
		red = fireworkColours[index][0];
		green = fireworkColours[index][1];
		blue = fireworkColours[index][2];
	}
	else
	{
		red = (float)r;
		green = (float)g;
		blue = (float)b;
	}

	switch(style)
	{
	case FIREWORK_BANGER:
		number = 3;
		break;
	case FIREWORK_SUBBURST:
		number = PARTICLES_PER_FIREWORK/3;
		break;
	case FIREWORK_TWISTER:
		number = 6;
		break;
	case FIREWORK_TWISTERTRAIL:
		number = 1;
		break;
	case FIREWORK_SUBTWISTER:
		number = 10;
		break;
	default:
		number = PARTICLES_PER_FIREWORK;
		break;
	}

	for(int32 i = number;i > 0;--i)
	{
		if(flags&FIREWORKS_RANDOMPARTICLE)
		{
			index = bmRand()%FIREWORKS_NUMCOLOURS;
			red = fireworkColours[index][0];
			green = fireworkColours[index][1];
			blue = fireworkColours[index][2];
		}

		baCreateExtra(fireworksExtraSystem,position,style,&red,&green,&blue,&scale);
	}
}

/* --------------------------------------------------------------------------------
   Function : InitFireworksExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitFireworksExtraSystem(uint32 numExtras)
{
	fireworksExtraSystem = baCreateExtraSystem("fireworks",FireworksExtraCreateCallback,
		FireworksExtraUpdateCallback,FireworksExtraDeleteCallback,FireworksExtraRenderCallback,
		FireworksExtraSystemInitCallback,sizeof(FIREWORKINFO),numExtras,0,0);
}

/* --------------------------------------------------------------------------------
   Function : FireExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void FireworksExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms)
{
    FIREWORKINFO		*data = (FIREWORKINFO*)vData;
	float				*position;

    if(parms)
	{
		position = va_arg(parms, float*);
		data->type = va_arg(parms, EFirework);
		data->colour[0].r = data->colour[1].r = *va_arg(parms, float*);
		data->colour[0].g = data->colour[1].g = *va_arg(parms, float*);
		data->colour[0].b = data->colour[1].b = *va_arg(parms, float*);
		data->colour[0].a = 128.0f;
		data->colour[1].a = 0.0f;
		data->scale = *va_arg(parms, float*);

		bmVectorCopy(data->position[0], position);

		switch(data->type)
		{
		case FIREWORK_TWISTER:
			data->angle = 0.0f;
			data->twisterClock = FIREWORKS_TWISTERPERIOD;
			//calculate initial velocity
			do
			{
				data->velocity[0] = UtilGenerateRandomNumber(50.0f, -50.0f);	//(float)((bmRand()%100)-50);
				data->velocity[1] = 0.0f;
				data->velocity[2] = UtilGenerateRandomNumber(50.0f, -50.0f);	//(float)((bmRand()%100)-50);
				data->velocity[3] = 1.0f;
			}while(bmVectorSquaredLen(data->velocity) == 0.0f);
			bmVectorScaleToLength(data->velocity, data->velocity, (float)((bmRand()%125) + FIREWORKS_TWISTERVELOCITY)*data->scale);
			data->velocity[1] = (float)(1000 + bmRand()%2000)*data->scale;
			//positions
			bmVectorCopy(data->position[1], data->position[0]);
			//other stuff
			data->streakClock = FIREWORKS_STREAKDELAY;
			data->size[0] = FIREWORKS_STREAKSIZE*data->scale;
			data->size[1] = FIREWORKS_STREAKTRAILSIZE*data->scale;
			data->life = FIREWORKS_TWISTERLIFE;
			data->texture = texturePowerUpGlow;
			data->colour[0].a = 70.0f;
			break;
		case FIREWORK_SUBTWISTER:
		case FIREWORK_STREAK:
			//calculate initial velocity
			do
			{
				data->velocity[0] = UtilGenerateRandomNumber(50.0f, -50.0f);	//(float)((bmRand()%100)-50);
				data->velocity[1] = 0.0f;
				data->velocity[2] = UtilGenerateRandomNumber(50.0f, -50.0f);	//(float)((bmRand()%100)-50);
				data->velocity[3] = 1.0f;
			}while(bmVectorSquaredLen(data->velocity) == 0.0f);
			bmVectorScaleToLength(data->velocity, data->velocity, (float)((bmRand()%125) + FIREWORKS_STREAKVELOCITY)*data->scale);
			data->velocity[1] = (float)(1000 + bmRand()%2000)*data->scale;
			//positions
			bmVectorCopy(data->position[1], data->position[0]);
			//other stuff
			data->streakClock = FIREWORKS_STREAKDELAY;
			data->size[0] = FIREWORKS_STREAKSIZE*data->scale;
			data->size[1] = FIREWORKS_STREAKTRAILSIZE*data->scale;
			data->life = FIREWORKS_STREAKLIFE;
			data->texture = texturePowerUpGlow;
			data->colour[0].a = 70.0f;
			break;
		case FIREWORK_SPARKLE:
		case FIREWORK_SUBBURST:
			//calculate radial velocity
			do
			{
				data->velocity[0] = UtilGenerateRandomNumber(50.0f, -50.0f);	//(float)((bmRand()%100)-50);
				data->velocity[1] = UtilGenerateRandomNumber(50.0f, -50.0f);	//(float)((bmRand()%100)-50);
				data->velocity[2] = UtilGenerateRandomNumber(50.0f, -50.0f);	//(float)((bmRand()%100)-50);
				data->velocity[3] = 1.0f;
			}while(bmVectorSquaredLen(data->velocity) == 0.0f);
			bmVectorScaleToLength(data->velocity, data->velocity, (float)((bmRand()%125) + FIREWORKS_SPARKLEVELOCITY)*data->scale);

			data->life = FIREWORKS_SPARKLELIFE;
			data->texture = texturePowerUpGlow;
			data->colour[0].sparkleAlpha = 128;

			data->size[0] = FIREWORKS_SPARKLESIZE*data->scale;
			break;
		case FIREWORK_BANGER:
			do
			{
				data->velocity[0] = UtilGenerateRandomNumber(50.0f, -50.0f);	//(float)((bmRand()%100)-50);
				data->velocity[1] = UtilGenerateRandomNumber(50.0f, 0.0f);		//(float)(bmRand()%50);
				data->velocity[2] = UtilGenerateRandomNumber(50.0f, -50.0f);	//(float)((bmRand()%100)-50);
				data->velocity[3] = 1.0f;
			}while(bmVectorSquaredLen(data->velocity) == 0.0f);
			bmVectorScaleToLength(data->velocity, data->velocity, ((float)(bmRand()%125) + FIREWORKS_BANGERVELOCITY)*data->scale);

			data->life = FIREWORKS_BANGERLIFE;
			data->texture = texturePowerUpGlow;
			data->size[0] = FIREWORKS_BANGERSIZE*data->scale;
			data->colour[0].r = data->colour[0].g = data->colour[0].b = data->colour[0].a = 128.0f;
			data->colour[0].sparkleAlpha = 128;
			break;
		case FIREWORK_TWISTERTRAIL:
			data->life = FIREWORKS_TWISTERTRAILLIFE;
			data->texture = textureRing;
			data->size[0] = FIREWORKS_TWISTERTRAILSIZE*data->scale;
			data->colour[0].a = 100.0f;
			data->angle = 0.0f;
			SETVECTOR(data->velocity, 0.0f, 0.0f, 0.0f, 1.0f);
			break;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : FireworksExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int FireworksExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata)
{
	FIREWORKINFO *data = (FIREWORKINFO*)vdata;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : FireworksExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void FireworksExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t)
{
	FIREWORKINFO		*data;
	TBVector			positionInc, velocityInc;
	TBVector			velocity = {0.0f, METERTOUNIT(3.0f), 0.0f, 1.0f};
	
	for (int32 i=0;i<noofInstances;i++)
	{
		data = (FIREWORKINFO*)dataPtrs[i];

		if((data->life -= fTime) > 0.0f)
		{
			//non type specific update
			//update position
			bmVectorCMul(positionInc, data->velocity, fTime);
			bmVectorAdd(data->position[0], data->position[0], positionInc);

			switch(data->type)
			{
			case FIREWORK_TWISTER:
				if((data->twisterClock -= fTime) < 0.0f)
				{
					data->twisterClock += FIREWORKS_TWISTERPERIOD;
					CreateFireworkBurst(data->position[0],FIREWORK_TWISTERTRAIL,
						(int32)data->colour[0].r,(int32)data->colour[0].g,(int32)data->colour[0].b,0, data->scale);
				}
			case FIREWORK_SUBTWISTER:
			case FIREWORK_STREAK:
				//update alpha
				if((data->colour[0].a -= FIREWORKS_SPARKLEDECAY*fTime) < 0)
				{
					data->colour[0].a = 0.0f;
					baDeleteExtraInstance(eSystem, i);
					noofInstances--;
					continue;
				}
				//update velocity
				bmVectorScaleToLength(velocityInc, data->velocity, FIREWORKS_STREAKDECEL*fTime);
				bmVectorAdd(data->velocity, data->velocity, velocityInc);
				ApplyGravity(positionInc, data->velocity,worldGravity*0.75f);
				//don't want to update position, it is enough to update the velocity
				//calculate trail position
				if((data->streakClock -= fTime) < 0.0f)
				{
					bmVectorCMul(positionInc, data->velocity, -FIREWORKS_STREAKDELAY*data->scale);
					bmVectorAdd(data->position[1], data->position[0], positionInc);
				}
				break;
			case FIREWORK_SPARKLE:
			case FIREWORK_SUBBURST:
				//update alpha
				if((data->colour[0].a -= FIREWORKS_SPARKLEDECAY*fTime) < 0)
				{
					data->colour[0].a = 0.0f;
					baDeleteExtraInstance(eSystem, i);
					noofInstances--;
					continue;
				}
				//calculate sparkle alpha
				data->colour[0].sparkleAlpha = bmRand()%((int32)data->colour[0].a+1);

				//update velocity
				bmVectorScaleToLength(velocityInc, data->velocity, FIREWORKS_SPARKLEDECEL*fTime);
				bmVectorAdd(data->velocity, data->velocity, velocityInc);
				ApplyGravity(positionInc, data->velocity,worldGravity*0.15f);
				//don't want to update position, it is enough to update the velocity
				break;
			case FIREWORK_BANGER:
				//update velocity
				bmVectorScaleToLength(velocityInc, data->velocity, FIREWORKS_STREAKDECEL*fTime);
				bmVectorAdd(data->velocity, data->velocity, velocityInc);
				ApplyGravity(positionInc, data->velocity,worldGravity*0.75f);
				//don't want to update position, it is enough to update the velocity
				break;
			case FIREWORK_TWISTERTRAIL:
				//update alpha
				if((data->colour[0].a -= 200.0f*fTime) < 0.0f)
				{
					data->colour[0].a = 0.0f;
					baDeleteExtraInstance(eSystem, i);
					noofInstances--;
					continue;
				}
				//update size
				data->size[0] += FIREWORK_TWISTERSIZEINCREM*fTime*data->scale;
				//update angle
				data->angle += RAD(360.0f*fTime);
				break;
			}
		}
		else
		{
			switch(data->type)
			{
			case FIREWORK_BANGER:
				CreateFireworkBurst(data->position[0], FIREWORK_SUBBURST,0,0,0,FIREWORKS_RANDOMCOLOUR,data->scale);
				break;
			case FIREWORK_TWISTER:
				CreateFireworkBurst(data->position[0], FIREWORK_SUBTWISTER,0,0,0,FIREWORKS_RANDOMCOLOUR,data->scale);
				break;
			}
			
			baDeleteExtraInstance(eSystem, i);
			noofInstances--;
			continue;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : FireworksExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void FireworksExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context)
{
	FIREWORKINFO *data;
	int32			i, bufIndexA,bufIndexB,bufIndexC, bufBStart, bufCStart;
	int				colourOne[4], colourTwo[4];

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);

	if((int32)context != EXTRA_ALPHA) return;

	bufIndexA = 0;
	bufIndexB = 0;
	bufBStart = 0;
	bufIndexC = 0;
	bufCStart = 0;
	TBTexture	*streak = NULL;
	TBTexture	*sparkle = NULL;
	TBTexture	*ring = NULL;

	// CMD: spin through list to draw streaks first
	for (i=0;i<noofInstances;i++)
	{
		data = (FIREWORKINFO*)dataPtrs[i];
		data->position[0][3] = 1.0f;
		data->position[1][3] = 1.0f;

		switch(data->type)
		{
		case FIREWORK_TWISTER:
		case FIREWORK_STREAK:
		case FIREWORK_SUBTWISTER:
			colourOne[0] = bmFloatToInt(data->colour[0].r); colourOne[1] = bmFloatToInt(data->colour[0].g);
			colourOne[2] = bmFloatToInt(data->colour[0].b); colourOne[3] = bmFloatToInt(data->colour[0].a)<<1;

			colourTwo[0] = bmFloatToInt(data->colour[1].r); colourTwo[1] = bmFloatToInt(data->colour[1].g);
			colourTwo[2] = bmFloatToInt(data->colour[1].b); colourTwo[3] = bmFloatToInt(data->colour[1].a)<<1;

			if(bdDrawStreakClipped(data->position[0],data->size[0],data->size[0],colourOne,
				data->position[1],data->size[1],data->size[1],colourTwo,data->texture,&global2DVertexBuffer[bufIndexA],0.0f))
			{
				if(bufIndexA)
				{
					//fill in extra vertices between streaks
					fmemcpy(&global2DVertexBuffer[bufIndexA-2], &global2DVertexBuffer[bufIndexA-3], sizeof(TBPrimVertex2D));
					fmemcpy(&global2DVertexBuffer[bufIndexA-1], &global2DVertexBuffer[bufIndexA], sizeof(TBPrimVertex2D));
				}
				bufIndexA += 10;
			}
			if(!streak) streak = data->texture;
			break;
		}
	}
	// CMD: mark start of next type in buffer
	bufBStart = bufIndexA;
	// CMD: spin through list to draw next type
	for (i=0;i<noofInstances;i++)
	{
		data = (FIREWORKINFO*)dataPtrs[i];
		data->position[0][3] = 1.0f;
		data->position[1][3] = 1.0f;

		switch(data->type)
		{
		case FIREWORK_SPARKLE:
		case FIREWORK_BANGER:
		case FIREWORK_SUBBURST:
			if(bdDrawFlatSprite(data->position[0],data->size[0],data->size[0],0.0f,data->texture,0,0,
				bmFloatToInt(data->colour[0].r), bmFloatToInt(data->colour[0].g), bmFloatToInt(data->colour[0].b), 
				data->colour[0].sparkleAlpha<<1, &global2DVertexBuffer[bufIndexB+bufBStart],0))
			{
				bufIndexB += 4;
			}
			if(!sparkle) sparkle = data->texture;
			break;
		}
	}
	// CMD: mark start of next type in buffer
	bufCStart = bufIndexA + bufIndexB;
	// CMD: spin through list to draw final type
	for (i=0;i<noofInstances;i++)
	{
		data = (FIREWORKINFO*)dataPtrs[i];
		data->position[0][3] = 1.0f;
		data->position[1][3] = 1.0f;

		switch(data->type)
		{
		case FIREWORK_TWISTERTRAIL:
			if(bdDrawFlatSprite(data->position[0],data->size[0],data->size[0],data->angle,data->texture,0,0,
				bmFloatToInt(data->colour[0].r), bmFloatToInt(data->colour[0].g), bmFloatToInt(data->colour[0].b), 
				bmFloatToInt(data->colour[0].a)<<1, &global2DVertexBuffer[bufIndexC+bufCStart],0))
			{
				bufIndexC += 4;
			}
			if(!ring) ring = data->texture;
			break;
		}
	}

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
//	bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdSetIdentityObjectMatrix();

	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);

	bdSetTexture(0, streak);
	//no need to check if bufIndexA is < 2, it is either 0 or a multiple of 10
	if(bufIndexA) bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, global2DVertexBuffer, bufIndexA-2,BVERTTYPE_SINGLE2D);

	bdSetTexture(0, sparkle);
	if(bufIndexB) bdDrawPrimitive(BDPRIMTYPE_QUADLIST, &global2DVertexBuffer[bufBStart], bufIndexB,BVERTTYPE_SINGLE2D);

	bdSetTexture(0, ring);
	if(bufIndexC) bdDrawPrimitive(BDPRIMTYPE_QUADLIST, &global2DVertexBuffer[bufCStart], bufIndexC,BVERTTYPE_SINGLE2D);

	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, TRUE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
}

/* --------------------------------------------------------------------------------
   Function : FireworksExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/

void FireworksExtraSystemInitCallback(TBExtraSystem *eSystem, int reason)
{
}
