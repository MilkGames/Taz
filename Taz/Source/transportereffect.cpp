// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : tarbubble.cpp
//   Purpose : functions to bubbles for cement/lava...
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "characters.h"
#include "main.h"
#include "lights.h"
#include "status.h"
#include "costumes.h"
#include "util.h"
#include "trigger.h"
#include "lava.h"
#include "transportereffect.h"

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters :
   Returns : 
   Info : 
*/

int EnableCharacterTransporterMode(ACTORINSTANCE *actorInstance, float height, float radius, int fadeUp)
{
	float	tempFloat;

	if(actorInstance->characterInfo&&(!actorInstance->characterInfo->transportInfo))
	{
		actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_NOMOVE;

		actorInstance->characterInfo->transportInfo = (TRANSFXINFO*)ZALLOC(sizeof(TRANSFXINFO));
		ASSERTM(actorInstance->characterInfo->transportInfo, "unable to create transporter effect");

		actorInstance->characterInfo->transportInfo->actorAlpha = 256.0f;
		actorInstance->characterInfo->transportInfo->maxHeight = height;
		actorInstance->characterInfo->transportInfo->radius = radius;
		actorInstance->characterInfo->transportInfo->fadeUpOnCompletion = fadeUp;
		actorInstance->characterInfo->transportInfo->state = TRANSXFSTATE_FADEDOWN;
		actorInstance->characterInfo->transportInfo->delay = 0.0f;
		actorInstance->characterInfo->transportInfo->onReachingDestination = NULL;
		actorInstance->characterInfo->transportInfo->onCompletion = NULL;
		actorInstance->characterInfo->transportInfo->xSize = METERTOUNIT(0.1f);
		actorInstance->characterInfo->transportInfo->ySize = METERTOUNIT(0.025f);

		// CMD: create spangly data
		for(int i = (TRANSFX_NUMSPARKLES-1);i >= 0;--i)
		{
			// CMD: choose random start angle
			actorInstance->characterInfo->transportInfo->angle[i] = RAD(UtilGenerateRandomNumber(360.0f, 0.0f));
			// CMD: choose random velocity
			if(bmRand()%2) 	actorInstance->characterInfo->transportInfo->angVel[i] = RAD(90.0f);
			else			actorInstance->characterInfo->transportInfo->angVel[i] = RAD(-90.0f);
			// CMD: set clock
			tempFloat = (float)i/(float)TRANSFX_NUMSPARKLES;
			actorInstance->characterInfo->transportInfo->clock[i] = tempFloat;
			// CMD: set height
			actorInstance->characterInfo->transportInfo->height[i] = height*tempFloat;
			// CMD: set draw = FALSE
			actorInstance->characterInfo->transportInfo->draw[i] = FALSE;
		}
	}

	// PP: Must return a value...
	return 0xB16B00B5;
}

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters :
   Returns : 
   Info : 
*/

void DisableCharacterTransporterMode(ACTORINSTANCE *actorInstance)
{
	actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_NOMOVE;

	if(actorInstance->characterInfo&&actorInstance->characterInfo->transportInfo)
	{
		SAFE_FREE(actorInstance->characterInfo->transportInfo);
	}
}

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters :
   Returns : 
   Info : 
*/

void SetCharacterTransportToPosition(ACTORINSTANCE *actorInstance, TBVector position, TBQuaternion orientation)
{
	if(actorInstance->characterInfo->transportInfo)
	{
		bmVectorCopy(actorInstance->characterInfo->transportInfo->targetPosition, position);
		bmQuatCopy(actorInstance->characterInfo->transportInfo->targetOrientation, orientation);
	}
}

static void myStateChange(ACTORINSTANCE *actorInstance)
{
	if(actorInstance->actorStatus->currentState == STATE_BALL)
	{
		if(actorInstance->characterInfo->costume != -1)
		{
			SetupCostume(actorInstance, actorInstance->characterInfo->costume);
		}
		else
		{
			ChangeActorInstance(actorInstance,FindActorInActorList("tazdevil.obe"),5,NULL);
		}
	}
	SETVECTOR(actorInstance->actorInstance.scale, 0.0f, 0.0f, 0.0f,1.0f);
}

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters :
   Returns : 
   Info : 
*/

int UpdateCharacterInTransporter(ACTORINSTANCE *actorInstance)
{
	TRANSFXINFO		*info = actorInstance->characterInfo->transportInfo;
	TBVector		position;

	switch(info->state)
	{
	case TRANSXFSTATE_FADEDOWN:
		// CMD: update character alpha
		if((info->actorAlpha -= 127.0f*fTime) < 0.0f)
		{
			info->actorAlpha = 0.0f;
			// CMD: transport character here!
			SetActorPosition(actorInstance, info->targetPosition);
			SetActorOrientation(actorInstance, info->targetOrientation);

			if(info->fadeUpOnCompletion)
			{
				if((info->delay -= fTime) < 0.0f)
				{
					SendTrigger(info->onReachingDestination);
					DeleteTriggerInfo(info->onReachingDestination);
					info->onReachingDestination = NULL;
					info->state = TRANSXFSTATE_FADEUP;
				}
				break;
			}
			else 
			{
				if(info->delay)
				{
					SETVECTOR(actorInstance->actorInstance.scale, 0.0f, 0.0f, 0.0f,1.0f);
					actorInstance->actorStatus->catatonicDelay = info->delay;
					if(info->onCompletion)
					{
						actorInstance->actorStatus->postCatatonicTrigger = CopyTriggerInfo(info->onCompletion);
					}
					SendTrigger(info->onReachingDestination);
					DeleteTriggerInfo(info->onReachingDestination);
					info->onReachingDestination = NULL;
					RequestStateChangePlus(actorInstance, STATE_INTRANSPORT, myStateChange);
					return TRUE;
				}
				else
				{
					// CMD: no delay so send complete trigger now
					SendTrigger(info->onReachingDestination);
					DeleteTriggerInfo(info->onReachingDestination);
					info->onReachingDestination = NULL;
					SendTrigger(info->onCompletion);
					DeleteTriggerInfo(info->onCompletion);
					info->onCompletion = NULL;
				}
				DisableCharacterTransporterMode(actorInstance);
				return TRUE;
			}
		}
		break;
	case TRANSXFSTATE_FADEUP:
		if((info->actorAlpha += 127.0f*fTime) > 256.0f)
		{
			info->actorAlpha = 256.0f;
			// CMD: fade up complete so we should send the onComplete trigger
			SendTrigger(info->onCompletion);
			DeleteTriggerInfo(info->onCompletion);
			info->onCompletion = NULL;

			DisableCharacterTransporterMode(actorInstance);
			return TRUE;
		}
		break;
	}

	// CMD: update spangly effect
	for(int i = (TRANSFX_NUMSPARKLES-1);i >= 0;--i)
	{
		if(info->draw[i])
		{
			// CMD: update spawn points
			info->angle[i] += info->angVel[i]*fTime;
			position[0] = info->radius*bmSin(info->angle[i]);
			position[1] = info->height[i];
			position[2] = info->radius*bmCos(info->angle[i]);
			bmVectorAdd(position, position, actorInstance->actorInstance.position);

			// CMD: create extra at this point
			baCreateExtra(lavaSpitExtraSystem, position, NULL, &info->xSize, &info->ySize, LAVASPIT_TRAIL);
		}
		else
			if((info->clock[i] -= fTime) < 0.0f)
			{
				info->draw[i] = TRUE;
			}
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters :
   Returns : 
   Info : 
*/

int DrawCharacterInTransporter(ACTORINSTANCE *actorInstance)
{
	EBActRenderMode		renderModeCopies[2];
	void*				renderParmCopies[2];
	int					onScreen;

	// CMD: draw
	renderModeCopies[0] = actorInstance->actorInstance.renderMode[0];
	renderModeCopies[1] = actorInstance->actorInstance.renderMode[1];
	renderParmCopies[0] = actorInstance->actorInstance.renderParms[0];
	renderParmCopies[1] = actorInstance->actorInstance.renderParms[1];

	baSetGlobalAlpha((int)actorInstance->characterInfo->transportInfo->actorAlpha);
	baSetActorRenderMode(&actorInstance->actorInstance, 0, BACTRENDERMODE_NORMAL, NULL);
	baSetActorRenderMode(&actorInstance->actorInstance, 1, BACTRENDERMODE_NONE, NULL);

	baDrawActorInstance(&actorInstance->actorInstance, BSETPRIMALPHATYPE_SOLID);
	baSetGlobalAlpha(256);

	
	EnableActorLineOnlyRenderMode(&actorInstance->actorInstance);
	// NH: baSetActorRenderMode(&actorInstance->actorInstance, 0, BACTRENDERMODE_ZONLY, NULL);
	// NH: baSetActorRenderMode(&actorInstance->actorInstance, 1, BACTRENDERMODE_OUTLINED, &lineParms);

	onScreen = baDrawActorInstance(&actorInstance->actorInstance,BSETPRIMALPHATYPE_SOLID);

	actorInstance->actorInstance.renderMode[0] = renderModeCopies[0];
	actorInstance->actorInstance.renderMode[1] = renderModeCopies[1];
	actorInstance->actorInstance.renderParms[0] = renderParmCopies[0];
	actorInstance->actorInstance.renderParms[1] = renderParmCopies[1];

	// PP: return the character's visibility status
	return onScreen;
}