// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : tumbleweed.cpp
//   Purpose : provides tumbleweed functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "main.h"
#include "animation.h"
#include "maths.h"
#include "physics.h"
#include "clouds.h"
#include "status.h"
#include "tumbleweed.h"

/* --------------------------------------------------------------------------------
   Function : CreateTumbleWeedInfo
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

TUMBLEWEEDINFO *CreateTumbleWeedInfo(ACTORINSTANCE *actorInstance, uint32 animCRC, uint32 nodeCRC)
{
	TUMBLEWEEDINFO		*info;

	info = (TUMBLEWEEDINFO*)ZALLOC(sizeof(TUMBLEWEEDINFO));
	ASSERTM(info, "no memory for tumbleweeds, poor tumbleweeds");

	actorInstance->special = SPECIALTYPE_TUMBLEWEED;

	info->animCRC = animCRC;

	info->node[0] = baFindNode(actorInstance->actorInstance.rootNodeInstance, "weed08");
	info->node[1] = baFindNode(actorInstance->actorInstance.rootNodeInstance, "weed09");
	info->node[2] = baFindNode(actorInstance->actorInstance.rootNodeInstance, "weed10");

	if(animCRC)
	{
		FlushAnimationQueue(actorInstance);
		PlayAnimationByCRCOfName(actorInstance, animCRC, 1.0f, 1, 0, 0.0f, IDLE);
	}
	actorInstance->tumbleweedInfo = info;

	return info;
}

/* --------------------------------------------------------------------------------
   Function : CreateTumbleWeedInfo
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateTumbleweed(ACTORINSTANCE *actorInstance)
{
	TBVector		nodePos;
	TBVector		disp;
	TBVector		starsBounds = {METERTOUNIT(0.3f), METERTOUNIT(0.4f), 0.0f, 0.0f};
	TBVector		starsOffset = {0.0f, METERTOUNIT(0.2f), 0.0f, 1.0f};

	
	// CMD: manual sphere on sphere collision probably the way to go
	for(int i = 0;i < gameStatus.multiplayer.numPlayers;i++)
	{
		for(int j = 0;j < 3;j++)
		{
			baGetNodesWorldPosition(&actorInstance->actorInstance, actorInstance->tumbleweedInfo->node[j], nodePos);

			bmVectorSub(disp, nodePos, gameStatus.player[i].actorInstance->actorInstance.position);
			if(bmVectorLen(disp) < (METERTOUNIT(0.5f)+gameStatus.player[i].actorInstance->actorBehaviour->collSphere.radius))
			{
				RequestStateChange(gameStatus.player[i].actorInstance, STATE_SQUASHED);
				CreateInjuryStars(10, gameStatus.player[i].actorInstance, "spinetop", starsOffset, starsBounds);
				FlushAnimationQueue(gameStatus.player[i].actorInstance);
				if(bmRand()%2)
				{
					PlayAnimationByName(gameStatus.player[i].actorInstance,"recover22c",1.0f,0,0,0.1f,NONE);
				}
				else
				{
					PlayAnimationByName(gameStatus.player[i].actorInstance,"recover21b",1.0f,0,0,0.1f,NONE);
				}
			}
		}
	}
}