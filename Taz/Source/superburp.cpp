// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : superburp.cpp
//   Purpose : functions to create the super burp
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "actors.h"
#include "characters.h"
#include "player.h"
#include "superburp.h"
#include "main.h"
#include "destructibles.h"
#include "status.h"
#include "scene.h"
#include "maths.h"
#include "genericai.h"
#include "trigger.h"

/*  --------------------------------------------------------------------------------
	Function	: SuperBurp
	Purpose		: Called when the player chooses to do the super burp move
	Parameters	: PLAYER
	Returns		: 
	Info		: if the player does not have enough pop this will not have any function
*/
void SuperBurp(PLAYER *player)
{
	if (player->actorInstance->characterInfo->superBurp == NULL)
	{
		player->actorInstance->characterInfo->superBurp = new SUPERBURP(player);
	}
}

/*  --------------------------------------------------------------------------------
	Function	: FinishBurp
	Purpose		: End the burpcan power up
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
void FinishBurp(ACTORINSTANCE *actorInstance)
{
	actorInstance->characterInfo->currentBurp = 0;
	if(actorInstance->characterInfo->superBurp)
	{
		delete actorInstance->characterInfo->superBurp;
		actorInstance->characterInfo->superBurp = NULL;
	}
	switch(gameStatus.globalSettings.difficulty)
	{
	case DIFF_EASY:
		actorInstance->characterInfo->superBurpEffectInfo.clock = SUPERBURP_EASY;
		break;
	case DIFF_MEDIUM:
		actorInstance->characterInfo->superBurpEffectInfo.clock = SUPERBURP_MEDIUM;
		break;
	case DIFF_HARD:
		actorInstance->characterInfo->superBurpEffectInfo.clock = SUPERBURP_HARD;
		break;
	}
}
		

/*  --------------------------------------------------------------------------------
	Function	: SUPERBURP::TAG_SUPERBURP
	Purpose		: Called when player chooses to do the superburp move
	Parameters	: none
	Returns		: 
	Info		: 
*/
SUPERBURP::TAG_SUPERBURP(PLAYER *player)
{
	ACTORINSTANCE *ptr;

	this->player = player;
	this->activeTime = 0.0f;


	// TP: knock enemies over
	for (ptr = map.characterInstances.head.next;ptr != &map.characterInstances.head;ptr = ptr->next)
	{
		AICharacterSpun(ptr,this->player->actorInstance,ATTACKTYPE_SPINREAR);
	}
}

/*  --------------------------------------------------------------------------------
	Function	: SUPERBURP::TAG_SUPERBURP
	Purpose		: destructor called when burp affect has finished
	Parameters	: none
	Returns		: 
	Info		: 
*/
SUPERBURP::~TAG_SUPERBURP()
{
//	this->player->camera->flags &= ~CAMERA_QUAKEFADE;
}

/*  --------------------------------------------------------------------------------
	Function	: SUPERBURP::Update
	Purpose		: called to do a general update of the superburp
	Parameters	: none
	Returns		: TRUE, FALSE ( FALSE if burp effect has finished)
	Info		: 
*/
int SUPERBURP::Update()
{
	this->activeTime += fTime;

	// TP: start breaking objects
	if(this->player->actorInstance->characterInfo->superBurpEffectInfo.blastRadiusInfo)
	{
		if(METERTOUNIT(this->player->actorInstance->characterInfo->superBurpEffectInfo.blastRadiusInfo->radius) < SUPERBURP_RANGE)
		{
			SmashStuff(&map.levelInstances,this->player->actorInstance,METERTOUNIT(this->player->actorInstance->characterInfo->superBurpEffectInfo.blastRadiusInfo->radius),100,NULL,SMASHFLAG_RANGEDATTACK);
			SmashStuff(&map.animatedInstances,this->player->actorInstance,METERTOUNIT(this->player->actorInstance->characterInfo->superBurpEffectInfo.blastRadiusInfo->radius),100,NULL,SMASHFLAG_RANGEDATTACK);
			SmashStuff(&map.invisibleInstances,this->player->actorInstance,METERTOUNIT(this->player->actorInstance->characterInfo->superBurpEffectInfo.blastRadiusInfo->radius),100,NULL,SMASHFLAG_RANGEDATTACK);
		}
	}

	if (this->activeTime > 2.0f)
	{
		return FALSE;
	}
	return TRUE;
}

/*  --------------------------------------------------------------------------------
	Function	: SUPERBURP::SmashStuff
	Purpose		: Call this to destroy a number of destructible items on the list given in range of position
	Parameters	: actorInstanceList, destructor, range, number, optional center point for test
	Returns		: 
	Info		: 
*/
void SmashStuff(const ACTORINSTANCELIST* const actorInstanceList, ACTORINSTANCE *destructor, const float range, int number, TBVector center, int flags)
{
	ACTORINSTANCE	*ptr,*temp;
	ECharState		tempState;
	TBVector		tempVector;
	TBVector		position;

	if (center) bmVectorCopy(position,center);
	else bmVectorCopy(position,destructor->actorInstance.position);

	// TP: spin through level instances
	ptr = actorInstanceList->head.next;

	// TP: dirty bodge to simulate the effect of Taz spinning into the destructibles
	tempState = destructor->actorStatus->currentState;
	destructor->actorStatus->currentState = STATE_SMASH;

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;)
	{
		temp = ptr->next;

		if (ptr->special == SPECIALTYPE_DESTRUCTIBLE)
		{
			if ((~flags&SMASHFLAG_RANGEDATTACK)||(~ptr->destructibleObjectInfo->flags&DESTRUCTIBLEFLAG_NORANGEDATTACKS))
			{
				bmVectorSub(tempVector,position,ptr->actorInstance.position);
				
				// TP: if destructible is in range the simulate destroy
				if (bmVectorLen(tempVector) < range)
				{
					if (ptr->destructibleObjectInfo->type==DESTRUCTIBLETYPE_SPIKEY)
					{
						if (flags&SMASHFLAG_CHILLI)
						{
							ptr->destructibleObjectInfo->flags &=~ DESTRUCTIBLEFLAG_INDESTRUCTIBLE;
							// TP: only decrement number if object is actualy destroyed
							if (ptr->RespondToCollisionRoutine) ptr->RespondToCollisionRoutine(destructor,ptr);
						}
					}
					else
					{
						// TP: only decrement number if object is actualy destroyed
						if (ptr->RespondToCollisionRoutine) ptr->RespondToCollisionRoutine(destructor,ptr);
					}
				}
			}
		}

		ptr = temp;
	}
	destructor->actorStatus->currentState = tempState;
}

/*  --------------------------------------------------------------------------------
	Function	: SUPERBURP::Draw
	Purpose		: called to do any visual effects needed
	Parameters	: none
	Returns		: 
	Info		: 
*/
void SUPERBURP::Draw()
{
}

