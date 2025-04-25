// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : buzzardscripts.cpp
//   Purpose : ai scripts for buzzard
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "main.h"

#include "buzzard.h"
#include "buzzardscripts.h"

/* --------------------------------------------------------------------------------
   Function : UpdateBuzzardAIScripts
   Purpose : checks to see what any buzzards in the scene should be doing
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateBuzzardAIScripts(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE	*taz;
	ACTORINSTANCE	*buzzard;
	BUZZARDINFO		*info;
	TBVector		tazDist;

	taz = gameStatus.player[0].actorInstance;

	for(buzzard = actorInstanceList->head.next;buzzard != &actorInstanceList->head;buzzard = buzzard->next)
	{
		if(buzzard->special != SPECIALTYPE_BUZZARD) continue;
		if(!(info = buzzard->buzzardInfo)) continue;

		switch(info->aiMetaState)
		{
		case BUZZARDAI_PERCH:
			bmVectorSub(tazDist, taz->actorInstance.position, buzzard->actorInstance.position);
			tazDist[1] = 0.0f;
			if(bmVectorLen(tazDist) < METERTOUNIT(3.0f))
			{
				RequestNewBuzzardScript(buzzard, beakyTakeOff);
				info->aiMetaState = BUZZARDAI_FLY;
			}
			else
			{
				if(!info->currentScript)
				{
					// CMD: nothing else going on, so choose an idle at random
					switch(bmRand()%10)
					{
					case 0:
					case 1:
					case 2:
						RequestNewBuzzardScript(buzzard, beakyIdle1);
						break;
					case 3:
					case 4:
					case 5:
						RequestNewBuzzardScript(buzzard, beakyIdle2);
						break;
					case 6:
					case 7:
					case 8:
						RequestNewBuzzardScript(buzzard, beakyIdle3);
						break;
					case 9:
						RequestNewBuzzardScript(buzzard, beakyTakeOff);
						info->aiMetaState = BUZZARDAI_FLY;
						break;
					}
				}
			}
			break;
		case BUZZARDAI_ATTACK:
			if(!info->currentScript)
			{
				switch(bmRand()%4)
				{
				case 0:
					RequestNewBuzzardScript(buzzard, standardFly);
					info->aiMetaState = BUZZARDAI_FLY;
					break;
				case 1:
					RequestNewBuzzardScript(buzzard, breaststrokeFly);
					info->aiMetaState = BUZZARDAI_FLY;
					break;
				case 2:
					RequestNewBuzzardScript(buzzard, frontcrawlFly);
					info->aiMetaState = BUZZARDAI_FLY;
					break;
				case 3:
					RequestNewBuzzardScript(buzzard, backstrokeFly);
					info->aiMetaState = BUZZARDAI_FLY;
					break;
				}
			}
			break;
		case BUZZARDAI_FLY:
			bmVectorSub(tazDist, taz->actorInstance.position, buzzard->actorInstance.position);
			tazDist[1] = 0.0f;
			if(bmVectorLen(tazDist) < METERTOUNIT(10.0f))
			{
				if(!info->currentScript)
				{
					RequestNewBuzzardScript(buzzard, beakyEggAttack);
					info->aiMetaState = BUZZARDAI_ATTACK;
				}
			}
			else
			{
				if(!info->currentScript)
				{
					switch(bmRand()%5)
					{
					case 0:
						RequestNewBuzzardScript(buzzard, standardFly);
						info->aiMetaState = BUZZARDAI_FLY;
						break;
					case 1:
						RequestNewBuzzardScript(buzzard, breaststrokeFly);
						info->aiMetaState = BUZZARDAI_FLY;
						break;
					case 2:
						RequestNewBuzzardScript(buzzard, frontcrawlFly);
						info->aiMetaState = BUZZARDAI_FLY;
						break;
					case 3:
						RequestNewBuzzardScript(buzzard, backstrokeFly);
						info->aiMetaState = BUZZARDAI_FLY;
						break;
					case 4:
						RequestNewBuzzardScript(buzzard, beakyLand);
						info->aiMetaState = BUZZARDAI_PERCH;
						break;
					}
				}
			}
			break;
		}

		UpdateBuzzardLogic(buzzard);
	}
}

/* --------------------------------------------------------------------------------
   Function : RequestNewBuzzardScript
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/


int32 RequestNewBuzzardScript(ACTORINSTANCE *actorInstance, BUZZARD_EVENT *newScript)
{
	actorInstance->buzzardInfo->lastScript = actorInstance->buzzardInfo->currentScript;
	actorInstance->buzzardInfo->currentScript = newScript;
	actorInstance->buzzardInfo->scriptIndex = 0;
	actorInstance->buzzardInfo->scriptIndexLastFrame = -1;

	return TRUE;
}