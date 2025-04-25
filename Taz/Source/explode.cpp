// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : explode.cpp
//   Purpose : 
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "maths.h"
#include "actors.h"
#include "sfx.h"
#include "explode.h"
#include "util.h"			// PP: Utility functions
#include "camera.h"			// PP: camera stuff
#include "quakecam.h"		// PP: camera shake - vital for explosion effects!
#include "Fireball.h"		// PP: Particle emitters used for explosion effects, eg. by WestBossExp

TBExtraSystem	*explosionExtraSystem;

/* --------------------------------------------------------------------------------
   Function : StartExplosion
   Purpose : initiates an explosion, and add it to extra system
   Parameters : position, initial scale, rate of scale, sound sample name, type, (PP)[opt/EXPLODE_DEFAULT_FADE_RATE] rate of fade
   Returns : 
   Info : scale refers to the approximate initial size of the effect
*/

void StartExplosion(TBVector position, float scale, float rateOfScale, char *soundSample, uint32 type, const int rateOfFade)
{
	switch(type)
	{
	case EXPLOSIONTYPE_CARTOON:
		StartCartoonExplosion(position, scale, rateOfScale, rateOfFade);
		break;
	}
	if(soundSample)
	{
		PlaySample(soundSample, 255, position, 0);
	}
}

/* --------------------------------------------------------------------------------
   Function : StartCartoonExplosion
   Purpose : initiates a cartoon explosion, and add it to extra system
   Parameters : position, initial scale, rate of scale, (PP)[opt/EXPLODE_DEFAULT_FADE_RATE] rate of fade
   Returns : 
   Info : 
*/

void StartCartoonExplosion(TBVector position, float scale, float rateOfScale, const int rateOfFade)
{
	TBVector	scaleVector;
	TBActor		*yellow;
	TBActor		*red;

	//create big yellow explosion star, and smaller red explosion star
	yellow = FindActorInActorList("explosionyellow.obe");
	red = FindActorInActorList("explosionred.obe");

	SETVECTOR(scaleVector, scale, scale, scale, 1.0f);

	baCreateExtra(explosionExtraSystem, yellow, position, scaleVector, &rateOfScale, rateOfFade, 255);
	bmVectorCMul(scaleVector, scaleVector, 0.1f);
	rateOfScale *= 0.3f;
	baCreateExtra(explosionExtraSystem, red, position, scaleVector, &rateOfScale, rateOfFade, 255);
}

/////////////////// explosion extra system functions ///////////////////

/* --------------------------------------------------------------------------------
   Function : InitExplosionExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitExplosionExtraSystem(void)
{
	explosionExtraSystem = baCreateExtraSystem("explode",ExplosionExtraCreateCallback,
		ExplosionExtraUpdateCallback,ExplosionExtraDeleteCallback,ExplosionExtraRenderCallback,
		ExplosionExtraSystemInitCallback,sizeof(EXPLOSION),(2*MAX_EXPLOSION),0,0);
}

/* --------------------------------------------------------------------------------
   Function : ExplosionExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void ExplosionExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms)
{
	EXPLOSION		*data = (EXPLOSION*)vData;
	TBActor			*actor;
	float			*position;
	float			*scale;
	float			rateOfScale;
	int				rateOfFade;// PP: 
	int				alpha;

    if(parms)
	{
		actor = (TBActor*)va_arg(parms, TBActor*);
		position = va_arg(parms, float*);
		scale = va_arg(parms,float*);
		rateOfScale = *va_arg(parms,float*);		// CMD: alteration to avoid using a double
		rateOfFade = va_arg(parms,int);// PP: 
		alpha = va_arg(parms,int);


		bmVectorCopy(data->scale, scale);
		data->rateOfScale = rateOfScale;
		data->rateOfFade = rateOfFade;// PP: 
		data->alpha = alpha;
	}
	if(actor)
	{
		data->actorInstance = (TBActorInstance*)ZALLOC(sizeof(TBActorInstance));
		if(data->actorInstance==NULL) return;
		baInitActorInstance(data->actorInstance, actor,5);
		bmVectorCopy(data->actorInstance->position,position);
		bmVectorCopy(data->actorInstance->scale, scale);
		baPrepActorInstance(data->actorInstance,TRUE);
		
		baSetActorRenderMode(data->actorInstance, 0, BACTRENDERMODE_NORMAL, NULL);
		baSetActorRenderMode(data->actorInstance, 1, BACTRENDERMODE_NONE, NULL);
		baSetNodeFrame(data->actorInstance, data->actorInstance->rootNodeInstance, 0, TRUE);
		baFlushNodeQueue(data->actorInstance, data->actorInstance->rootNodeInstance, TRUE);
	}
}

/* --------------------------------------------------------------------------------
   Function : ExplosionExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int ExplosionExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata)
{
	EXPLOSION *data = (EXPLOSION*)vdata;

	if (data->actorInstance)
	{
		// if it has an actor then delete it
		baFreeActorInstance(data->actorInstance);
		FREE(data->actorInstance);
		data->actorInstance = NULL;
	}

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ExplosionExtraUpdateCallback
   Purpose : This function is called each frame to update the debris
   Parameters : 
   Returns : 
   Info : 
*/
void ExplosionExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t)
{
	EXPLOSION		*data;
	TBVector		scaleIncrement;
	int				i;
	
	for (i=0;i<noofInstances;i++)
	{
		data = (EXPLOSION*)dataPtrs[i];

		if(data->alpha > 0)
		{
			//update scale
			SETVECTOR(scaleIncrement, data->rateOfScale, data->rateOfScale, data->rateOfScale, 1.0f);
			bmVectorCDiv(scaleIncrement, scaleIncrement, fps);

			bmVectorAdd(data->actorInstance->scale, data->actorInstance->scale, scaleIncrement);
			
			//update alpha
			if((data->alpha -= (int)(data->rateOfFade)) < 0)// PP: 
			{
				data->alpha = 0;
			}

			baPrepActorInstance(data->actorInstance, FALSE);
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
   Function : ExplosionExtraRenderCallback
   Purpose : This function is called each frame to render the debris
   Parameters : 
   Returns : 
   Info : 
*/
void ExplosionExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context)
{
	EXPLOSION	*data;
	int			i;

	if((int32)context != EXTRA_ALPHA) return;

	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, 0);
	for (i=0;i<noofInstances;i++)
	{
		data = (EXPLOSION*)dataPtrs[i];

		if (data->actorInstance)
		{
			baSetGlobalAlpha(data->alpha);

			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

			bdSetTexture(0, NULL);
			baDrawActorInstance(data->actorInstance,BSETPRIMALPHATYPE_SOLID|BSETPRIMALPHATYPE_CKEY|BSETPRIMALPHATYPE_ALPHA);

			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
		}
	}
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, 0);
	baSetGlobalAlpha(256);
}

/* --------------------------------------------------------------------------------
   Function : ExplosionExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/
void ExplosionExtraSystemInitCallback(TBExtraSystem *eSystem, int reason)
{
}


// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// PP: NOTE: I've removed the poster explosion stuff - I'm using Fireball.cpp/.h now.
// PP: Just in case you maybe thought this was an out-of-date version of the file.
// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~