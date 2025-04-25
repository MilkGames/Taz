// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : lava.cpp
//   Purpose : lava functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "main.h"
#include "textures.h"
#include "actors.h"
#include "characters.h"
#include "chillipepper.h"
#include "sfx.h"
#include "lava.h"
#include "physics.h"
#include "status.h"
#include "animation.h"
#include "health.h"
#include "util.h"
#include "genericai.h"
#include "rumble.h"

TBExtraSystem	*lavaSpitExtraSystem;

static void myStateChange(ACTORINSTANCE *actorInstance)
{
	TBVector				yAxis = {0.0f, 1.0f, 0.0f, 1.0f};

	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance, "waterbounce", 1.0f, 1, 0, 0.1f, NONE);
	
	if (characterSounds.taz == -1)
		CallbackWhenSampleFinished(characterSounds.taz = PlaySample("tazburnt.wav", 255), CharacterSoundsFinishedCallback, &characterSounds.taz);
	
	actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_NOMOVE;
	actorInstance->actorBehaviour->physics.velocity[1] = 0.0f;
	AddActorSpeed(actorInstance->actorBehaviour, yAxis, METERTOUNIT(10.0f));

	actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;
	actorInstance->actorAnimation->useMotionBone = FALSE;
	ReduceCharactersHealth(actorInstance, 0.5f);
}

/* --------------------------------------------------------------------------------
   Function : RespondToLavaCollision
   Purpose : 
   Parameters : ACTORINSTANCE *
   Returns :
   Info :
*/
int RespondToLavaCollision(ACTORINSTANCE *collider, ACTORINSTANCE *object)
{
	if (collider->genericAI) 
	{
		AICollideWithSpecial(collider,object);
		// TP: returning true and false on a collision callback rather that a pre collision callback
		// TP: tells the collision routine that the object has been destroyed by the callback
		return FALSE;
	}
	if (collider->flags&ACTORFLAG_DEBRISACTOR) return FALSE;
	if (collider->actorStatus->currentState==STATE_VEHICLE) return FALSE;

//	StopCollidingWith(collider,object);
	// TP: burn ass and make Taz fly

	PlaySample("off.wav", 255);

	if ((object->special==SPECIALTYPE_LAVA)&&(object->lavaInfo->smoke))
	{
		RequestStateChangePlus(collider,STATE_BURNT,myStateChange);

		if(gameStatus.player[collider->playerNo].actorInstance->characterInfo->powerUpInfo.chilliPepper)	
			FinishChilliPepper(gameStatus.player[collider->playerNo].actorInstance);

		RequestRumbleEffect(gameStatus.player[collider->playerNo].controller, 200, 0.0f, 0.5f, 0.5f, 0.0f, WAVEFORM_RANDOM);
		RequestBuzzerEffect(gameStatus.player[collider->playerNo].controller, 1.0f, 0.0f, WAVEFORM_SQUARE, 5.0f);
	}
	else
	{
		RequestStateChangePlus(collider,STATE_SPRUNG,myStateChange);
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : CreateLavaObject
   Purpose : Creates a lava object
   Parameters : ACTORINSTANCE *
   Returns :
   Info :
*/
int CreateLavaObject(ACTORINSTANCE *actorInstance,bool smoke)
{
	actorInstance->lavaInfo = (LAVAOBJECTINFO*)ZALLOC(sizeof(LAVAOBJECTINFO));
	if (!actorInstance->lavaInfo) return FALSE;
	actorInstance->special = SPECIALTYPE_LAVA;
	actorInstance->lavaInfo->smoke = smoke;
	SetCollisionCallback(actorInstance, RespondToLavaCollision);
	return TRUE;

}

/* --------------------------------------------------------------------------------
   Function : CreateLavaObject
   Purpose : Creates a lava object
   Parameters : ACTORINSTANCE *
   Returns :
   Info :
*/

void CreateLavaSpitSource(ACTORINSTANCE *actorInstance)
{
	actorInstance->special = SPECIALTYPE_LAVASPIT;
	actorInstance->lavaSpitInfo = (LAVASPITINFO*)ZALLOC(sizeof(LAVASPITINFO));
	ASSERTM(actorInstance->lavaSpitInfo, "out of memory apparently");

	actorInstance->lavaSpitInfo->clock = UtilGenerateRandomNumber(8.0f, 6.0f);
	actorInstance->lavaSpitInfo->state = LAVASPIT_IDLE;
}

/* --------------------------------------------------------------------------------
   Function : CreateLavaObject
   Purpose : Creates a lava object
   Parameters : ACTORINSTANCE *
   Returns :
   Info :
*/

void UpdateLavaSpitSource(ACTORINSTANCE *actorInstance)
{
	LAVASPITINFO	*info = actorInstance->lavaSpitInfo;
	TBVector		posInc, tempPos;
	float			size = METERTOUNIT(0.45f);

	switch(info->state)
	{
	case LAVASPIT_IDLE:
		if((info->clock -= fTime) < 0.0f)
		{
			info->clock = 3.0f;
			if(bmRand()%2)	info->state = LAVASPIT_TRAIL;
			else			
			{
				bmVectorSet(info->offset, METERTOUNIT(UtilGenerateRandomNumber(0.5f, -0.5f)), 0.0f, METERTOUNIT(UtilGenerateRandomNumber(0.5f, -0.5f)), 1.0f);
				info->state = LAVASPIT_FOUNTAIN;
			}
			bmVectorCopy(info->position, actorInstance->actorInstance.position);
			bmVectorSet(info->velocity, UtilGenerateRandomNumber(LAVASPIT_HORIZVEL, -LAVASPIT_HORIZVEL), 
										UtilGenerateRandomNumber(LAVASPIT_VERTVEL, LAVASPIT_VERTVEL*0.5f), 
										UtilGenerateRandomNumber(LAVASPIT_HORIZVEL, -LAVASPIT_HORIZVEL), 1.0f);
		}
		break;
	case LAVASPIT_TRAIL:
		ApplyGravity(posInc, info->velocity, worldGravity*0.25f);
		bmVectorAdd(info->position, info->position, posInc);

		if(info->position[1] < actorInstance->actorInstance.position[1])
		{
			info->clock = UtilGenerateRandomNumber(8.0f, 5.0f);
			info->state = LAVASPIT_IDLE;
			break;
		}
		baCreateExtra(lavaSpitExtraSystem, info->position, NULL, &size, &size, LAVASPIT_TRAIL);
		break;
	case LAVASPIT_FOUNTAIN:
		if((info->clock -= fTime) < 0.0f)
		{
			info->clock = UtilGenerateRandomNumber(8.0f, 5.0f);
			info->state = LAVASPIT_IDLE;
		}

		// CMD: choose random velocity
		bmVectorSet(posInc, METERTOUNIT(UtilGenerateRandomNumber(1.0f, -1.0f)), 
							METERTOUNIT(UtilGenerateRandomNumber(4.0f, 3.5f)), 
							METERTOUNIT(UtilGenerateRandomNumber(1.0f, -1.0f)), 1.0f);
		bmVectorAdd(tempPos, info->position, info->offset);
		baCreateExtra(lavaSpitExtraSystem, tempPos, posInc, &size, &size, LAVASPIT_FOUNTAIN);
		break;
	}
}

/* --------------------------------------------------------------------------------
   Function : InitLavaSpitExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitLavaSpitExtraSystem(void)
{
	lavaSpitExtraSystem = baCreateExtraSystem("lavaSpit",LavaSpitExtraCreateCallback,
		LavaSpitExtraUpdateCallback,LavaSpitExtraDeleteCallback,LavaSpitExtraRenderCallback,
		LavaSpitExtraSystemInitCallback,sizeof(LAVASPITEXTRA),MAX_LAVASPITTLE,0,0);
}

/* --------------------------------------------------------------------------------
   Function : LavaSpitExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void LavaSpitExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms)
{
    LAVASPITEXTRA		*data = (LAVASPITEXTRA*)vData;
	float				*position;

    if(parms)
	{
		position = va_arg(parms, float*);
		bmVectorCopy(data->position, position);
		position = va_arg(parms, float*);
		if(position)	bmVectorCopy(data->velocity, position);
		else			bmVectorSet(data->velocity, 0.0f, 0.0f, 0.0f, 1.0f);
		data->xSize = *va_arg(parms, float*);
		data->ySize = *va_arg(parms, float*);
		data->type = (ELavaState)va_arg(parms, ELavaState);

		data->alpha = 100.0f;
		data->alphaFadeRate = 100.0f; //per second
		data->r = LAVASPIT_INITR;
		data->g = LAVASPIT_INITG;
		data->b = LAVASPIT_INITB;
	}
}

/* --------------------------------------------------------------------------------
   Function : LavaSpitExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int LavaSpitExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata)
{
	LAVASPITEXTRA *data = (LAVASPITEXTRA*)vdata;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : LavaSpitExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void LavaSpitExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t)
{
	LAVASPITEXTRA		*data;
	TBVector		posInc;
	
	for (int i=0;i<noofInstances;i++)
	{
		data = (LAVASPITEXTRA*)dataPtrs[i];

		switch(data->type)
		{
		case LAVASPIT_FOUNTAIN:
			// CMD: update position
			ApplyGravity(posInc, data->velocity, worldGravity*0.5f);
			bmVectorAdd(data->position, data->position, posInc);
			break;
		}

		// CMD: update colour
		data->r += (LAVASPIT_TARGETR-LAVASPIT_INITR)*fTime;
		data->g += (LAVASPIT_TARGETG-LAVASPIT_INITG)*fTime;
		data->b += (LAVASPIT_TARGETB-LAVASPIT_INITB)*fTime;

		// CMD: update alpha
		if((data->alpha -= data->alphaFadeRate*fTime) < 0.0f)
		{
			baDeleteExtraInstance(eSystem, i);
			--noofInstances;
			continue;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : LavaSpitExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void LavaSpitExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context)
{
	LAVASPITEXTRA		*data;
	int32			i, bufIndex;


	if((int32)context != EXTRA_ALPHA) return;

	bufIndex = 0;
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);

	for (i=0;i<noofInstances;i++)
	{
		data = (LAVASPITEXTRA*)dataPtrs[i];
		data->position[3] = 1.0f;

		if(bdDrawFlatSprite(data->position,data->xSize,data->ySize,0.0f,NULL,0,0,127,60,30,bmFloatToInt(data->alpha)<<1,&global2DVertexBuffer[bufIndex],0))
		{
			bufIndex += 4;
		}
	}

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_ADDITIVE, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);
	bdSetIdentityObjectMatrix();

	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);

	bdSetTexture(0, texturePowerUpGlow);
	if(bufIndex) bdDrawPrimitive(BDPRIMTYPE_QUADLIST, global2DVertexBuffer, bufIndex, BVERTTYPE_SINGLE2D);


	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, TRUE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
}

/* --------------------------------------------------------------------------------
   Function : LavaSpitExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/

void LavaSpitExtraSystemInitCallback(TBExtraSystem *eSystem, int reason)
{
}