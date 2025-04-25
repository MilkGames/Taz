// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : arrow.cpp
//   Purpose : boingy arrows for safari
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "main.h"
#include "util.h"
#include "collectibles.h"
#include "arrow.h"

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void CreateSafariArrow(ACTORINSTANCE *actorInstance)
{
	// CMD: only ever create these if in easy mode
	if(gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].difficulty == DIFF_EASY)
	{
		if(actorInstance)
		{
			actorInstance->arrowInfo = (SAFARIARROWINFO*)ZALLOC(sizeof(SAFARIARROWINFO));
			ASSERTM(actorInstance->arrowInfo, "could not malloc arrow info");
	
			actorInstance->special = SPECIALTYPE_ARROW;

			actorInstance->arrowInfo->rotSpeed = 0.0f;
			actorInstance->arrowInfo->rotSpeed = 0.0f;
			actorInstance->arrowInfo->rotAcceleration = 0.0f;
			actorInstance->arrowInfo->clock = 0.0f;
			actorInstance->arrowInfo->phaseAngle = 0.0f;
			actorInstance->arrowInfo->spinClock = UtilGenerateRandomNumber(10.0f, 5.0f);
			actorInstance->arrowInfo->spin = FALSE;
			actorInstance->arrowInfo->alpha = 256;
			actorInstance->arrowInfo->state = ARROW_NORMAL;
			bmQuatCopy(actorInstance->arrowInfo->initOrientation, actorInstance->actorInstance.orientation);
		}
	}
	else
	{
		FreeActorInstance(actorInstance);
		return;
	}
}

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateSafariArrow(ACTORINSTANCE *actorInstance)
{
	SAFARIARROWINFO		*info;
	TBVector			dist;
	TBQuaternion		quat;
	float				amp, len, excition, xzScale, yScale;

	if(actorInstance->special != SPECIALTYPE_ARROW) return;

	if(info = actorInstance->arrowInfo)
	{
		// CMD: do some bouncy scaling stuff
		// CMD: and vary it with the distance from taz
		// CMD: scale x, z in phase, y out of phase
		bmVectorSub(dist, actorInstance->actorInstance.position, gameStatus.player[0].actorInstance->actorInstance.position);
		if(len = bmVectorLen(dist))
		{
			if((excition = METERTOUNIT(2.0f)/len) > 0.8f) excition = 0.8f;
		}
		else excition = 0.0f;
		if((info->phaseAngle += COLLECTIBLES_SCALE_RATE*fTime*excition) > 360.0f) info->phaseAngle -= 360.0f;

		if((amp = COLLECTIBLES_AMP_CONST/len) > 2.0f) amp = 2.0f;
		xzScale = bmSin(RAD(info->phaseAngle));
		xzScale = 1.0f + (amp*xzScale*xzScale*0.5f);
		yScale = bmCos(RAD(info->phaseAngle));
		yScale = 1.0f + (amp*yScale*yScale);

		SETVECTOR(actorInstance->actorInstance.scale, xzScale, yScale, xzScale, 1.0f);

		switch(info->state)
		{
		case ARROW_NORMAL:
			// CMD: update alpha such that objects do not obscure camera
			bmVectorSub(dist, actorInstance->actorInstance.position, gameStatus.player[0].camera->cameraState.cameraPos);
	#define ARROW_THRESHOLD		(METERTOUNIT(5.0f))
	#define ARROW_MIN			(METERTOUNIT(2.0f))
			if((len = bmVectorLen(dist)) < ARROW_THRESHOLD)
			{
				if(len < ARROW_MIN)
				{
					info->alpha = 0;
				}
				else
				{
					// CMD: fade based on proximity
					info->alpha = (short)(256.0f*((len-ARROW_MIN)/(ARROW_THRESHOLD-ARROW_MIN)));
				}
			}
			else
			{
				info->alpha = 256;
			}
			actorInstance->alpha = info->alpha;
			break;
		case ARROW_FADE:
			if((info->alpha -= (short)(80.0f*fTime)) < 0.0f)
			{
				info->alpha = 0;
				info->state = ARROW_DEAD;
				actorInstance->flags |= ACTORFLAG_DONTDRAW;
			}
			actorInstance->alpha = info->alpha;
			break;
		case ARROW_DEAD:
			// CMD: do nothing
			break;
		}
	}
}                                                                                                                                                                                                                                                                                                                   