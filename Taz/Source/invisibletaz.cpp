// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : superburpeffect.cpp
//   Purpose : functions to update and draw superburp effects
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "characters.h"
#include "lights.h"
#include "invisibletaz.h"

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateCharacterInvisiblity(ACTORINSTANCE *actorInstance)
{
	if(actorInstance->characterInfo->powerUpInfo.invisible)
	{
		actorInstance->characterInfo->powerUpInfo.clock += fTime;

		if(actorInstance->characterInfo->powerUpInfo.clock > (INVISIBLE_EFFECT_TIME-5.0f))
		{
			actorInstance->characterInfo->powerUpInfo.randomClock += fTime;
			
			switch(actorInstance->characterInfo->powerUpInfo.state)
			{
			case RENDERSTATE_CARTOON:
				if(actorInstance->characterInfo->powerUpInfo.randomClock > INVISIBLE_NOT_TIME)
				{
					actorInstance->characterInfo->powerUpInfo.randomClock = 0.0f;
					actorInstance->characterInfo->powerUpInfo.state = RENDERSTATE_LINEONLY;
					EnableActorLineOnlyRenderMode(&actorInstance->actorInstance, &invisLineParms);
					// NH: baSetActorRenderMode(&actorInstance->actorInstance, 0, BACTRENDERMODE_ZONLY, NULL);
					// NH: baSetActorRenderMode(&actorInstance->actorInstance, 1, BACTRENDERMODE_OUTLINED, &invisLineParms);
				}
				break;
			case RENDERSTATE_LINEONLY:
				if(actorInstance->characterInfo->powerUpInfo.randomClock > actorInstance->characterInfo->powerUpInfo.thirdClock)
				{
					if((actorInstance->characterInfo->powerUpInfo.thirdClock *= 0.75f) < 0.25f)
					{
						actorInstance->characterInfo->powerUpInfo.thirdClock = 0.25f;
					}
					actorInstance->characterInfo->powerUpInfo.randomClock = 0.0f;
					actorInstance->characterInfo->powerUpInfo.state = RENDERSTATE_CARTOON;
					EnableActorCartoonRenderMode(&actorInstance->actorInstance, NULL, &invisLineParms);
					// NH: baSetActorRenderMode(&actorInstance->actorInstance, 0, BACTRENDERMODE_CARTOONLIT, &cartoonParms);
					// NH: baSetActorRenderMode(&actorInstance->actorInstance, 1, BACTRENDERMODE_OUTLINED, &invisLineParms);
				}
				break;
			}
		}
//		else
//		{
//			baSetActorRenderMode(&actorInstance->actorInstance, 0, BACTRENDERMODE_ZONLY, NULL);
//			baSetActorRenderMode(&actorInstance->actorInstance, 1, BACTRENDERMODE_OUTLINED, &invisLineParms);
//		}
		if(actorInstance->characterInfo->powerUpInfo.clock > INVISIBLE_EFFECT_TIME)
		{
			FinishInvisibility(actorInstance);
		}
	}
}

/*  --------------------------------------------------------------------------------
	Function	: FinishInvisibility
	Purpose		: Turns off invisibility on an actorinstance
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
void FinishInvisibility(ACTORINSTANCE *actorInstance)
{
	actorInstance->characterInfo->powerUpInfo.clock = 0.0f;
	EnableActorCartoonRenderMode(&actorInstance->actorInstance);
	actorInstance->characterInfo->powerUpInfo.state = RENDERSTATE_CARTOON;
	actorInstance->characterInfo->powerUpInfo.invisible = FALSE;
}