// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : bubblegum.cpp
//   Purpose : functions to update and draw bubblegum effects
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "characters.h"
#include "maths.h"
#include "physics.h"
#include "status.h"
#include "animation.h"
#include "invisibletaz.h"
#include "bubblegum.h"
#include "control.h"
#include "main.h"
#include "sfx.h"

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateBubbleGumPowerUpForCharacter(ACTORINSTANCE *actorInstance)
{
	POWERUPINFO	*ptr = &actorInstance->characterInfo->powerUpInfo;
	bool	exit = false;

	if(ptr->bubbleGum)
	{
		if (actorInstance->actorStatus->currentState==STATE_BUBBLEGUM)
		{
			if((!ptr->gumReady)&&(!CheckIfPlayingAnimation(actorInstance,"blowgumbubble")))
			{
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance,"floatgumbubble",1.0f,1,0,0.1f,MOVE);
				actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_NOGRAVITY;
				ptr->gumReady = TRUE;
			}
			else
				if(!ptr->gumReady)
				{
					UpdateGumBubble(actorInstance);
				}

			if(ptr->gumReady)
			{
				if((ptr->clock += fTime) < BUBBLEGUM_TIME)
				{
					switch(ptr->state)
					{
					case BUBBLEGUM_RISING:
						//apply constant vertical velocity for a set time
						SETVECTOR(ptr->tempVector, 0.0f, METERTOUNIT(1.0f), 0.0f, 1.0f);
						if(ptr->clock > 2.0f)
						{
							ptr->state = BUBBLEGUM_STABLE;
							break;
						}
						break;
					case BUBBLEGUM_STABLE:
						//apply rotating velocity vector in vertical direction only
						SETVECTOR(ptr->tempVector, 0.0f, BUBBLEGUM_ROTATING_VECTOR_LENGTH*bmSin(RAD((ptr->randomClock+=fTime)*180.0f)), 0.0f, 1.0f);
						break;
					}
				}
				else
				{
					//end of time, what to do?
					exit = true;
				}
				switch(actorInstance->playerNo)
				{
				case 0:
					if(controller1.squareDebounceChannel->value)
					{
						exit = true;
					}
					break;
				case 1:
					if(controller2.squareDebounceChannel->value)
					{
						exit = true;
					}
					break;
				}

				if (exit)
				{
					if (!RequestStateChange(actorInstance, STATE_MOVE))
					{
						// TP: bugger can't change state, this should'nt really happen changing from bubble->move
					}
				}
			}
		}
		else
		{
			// TP: I'm not in this state anymore
			ptr->bubbleGum = false;
		}
	}
}

/*  --------------------------------------------------------------------------------
	Function	: FinishBubbleGum
	Purpose		: Sort out Taz after bubble gum move
	Parameters	: ACTORINSTANCE
	Returns		: 
	Info		: Made this seperate function so it can be called if the player dies
*/
void FinishBubbleGum(ACTORINSTANCE *actorInstance)
{
	POWERUPINFO	*ptr = &actorInstance->characterInfo->powerUpInfo;

	ptr->clock = BUBBLEGUM_TIME;
}

/* --------------------------------------------------------------------------------
   Function : AttachGumBubbleToActor
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int	AttachGumBubbleToActor(ACTORINSTANCE *actorInstance)
{
	POWERUPINFO		*ptr;

	if(!actorInstance->characterInfo) return FALSE;
	if(!(ptr = &actorInstance->characterInfo->powerUpInfo)) return FALSE;

	if((ptr->gumBubbleHandle = AttachObjectToCharacter(actorInstance,"gumbubble.obe","snoutstem",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR)) != -1)
	{
//		ptr->node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "toungemid");
//		ptr->gumBubble = ReturnAttachedObjectInstance(actorInstance,ptr->gumBubbleHandle);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateGumBubble
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateGumBubble(ACTORINSTANCE *actorInstance)
{
	POWERUPINFO		*ptr = &actorInstance->characterInfo->powerUpInfo;

	//do a linear scale from min to max
//	scale = (ptr->thirdClock += fTime) * BUBBLEGUM_SCALE_CONST;

//	SETVECTOR(ptr->gumBubble->scale, scale, scale, scale, 1.0f);

	//update orientation
//	if(ptr->node)
//	{
//		bmQuatMultiply(ptr->gumBubble->orientation,actorInstance->actorInstance.orientation,ptr->node->animState.orientation);
//	}
}