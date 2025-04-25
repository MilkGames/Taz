// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : electricityattack.cpp
//   Purpose : attack move for mecha tweety boss game
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "main.h"
#include "breakables.h"
#include "status.h"
#include "animation.h"
#include "characters.h"
#include "health.h"
#include "mtweetyscripts.h"
#include "mechatweety.h"
#include "crateattack.h"
#include "bossgamecontinue.h"


/* --------------------------------------------------------------------------------
   Function : PreRespondToMTweetyCrateCollision
   Purpose : called when mecha tweety hits taz with a crate but before response has been processed
   Parameters : 
   Returns : 
   Info : 
*/
int PreRespondToMTweetyCrateCollision(ACTORINSTANCE *collider, ACTORINSTANCE *crate, PRECOLLINFO *info)
{
	return false;	// TP: never do proper collision
}

void myStateChange(ACTORINSTANCE *actorInstance)
{
	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance, "tazdie1", 1.0f, 0, 0, 0.1f, DEATH);
}

/* --------------------------------------------------------------------------------
   Function : RespondToMTweetyCrateCollision
   Purpose : called when mecha tweety hits taz with a crate
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToMTweetyCrateCollision(ACTORINSTANCE *collider, ACTORINSTANCE *crate)
{
	TBQuaternion	tempQuat;

	if(collider == gameStatus.player[0].actorInstance)
	{
		if(~crate->tazBossItemInfo->flags & BOSSITEMFLAG_DESTROYASAP)
		{
			RequestStateChange(collider,STATE_PLAYANIMATION);
			FlushAnimationQueue(collider);
			PlayAnimationByName(collider,"react1",1.0f,0,0,0.1f,NONE);
			RequestRumbleEffect(&controller1, 255, 0.0f, 0.2f, 0.5f, 0.0f, WAVEFORM_NORMAL);
			PlaySample("boing.wav", 255);
	
			crate->tazBossItemInfo->flags |= BOSSITEMFLAG_DESTROYASAP;
			if(collider->characterInfo->tazShieldInfo)
			{
				bkPrintf("***\n\tReduce Taz shield health by one:\n\t\tgame time = %f\n\t\tCollider = %s\n\t\tCrate = %s\n***\n", 
							gameStatus.gameTime, collider->instanceName, crate->instanceName);
				if(!ReduceShieldHealthByOne(collider))
				{
					// CMD: make sure Taz and Tweety are doing the right thing	
					RequestNewMechaTweetySequence(globalMechaTweety, holdAtPosition);
					globalMechaTweety->mechaTweetyInfo->flags |= MTWEETY_TWEETYNOMOVE;
					bmQuatCopy(tempQuat,collider->actorInstance.orientation);
					ChangeActorInstance(collider,FindActorInActorList("tazdevil.obe"),5,NULL);
					bmQuatCopy(collider->actorInstance.orientation,tempQuat);
					ResetCharacterAfterJump(collider);
//					RequestStateChangePlus(collider, STATE_PLAYANIMANDFREEZE, myStateChange);
					if(RequestStateChange(collider, STATE_PLAYANIMANDFREEZE))
					{
						if(UpdateCharacterState(&gameStatus.player[collider->playerNo], true))
						{
							FlushAnimationQueue(collider);
							PlayAnimationByName(collider, "tazdie1", 1.0f, 0, 0, 0.1f, DEATH);
						}
						else
						{
							ClearStateChangeRequest(collider);
						}
					}
					
					mTweetyGenericInfo->flags |= MTWEETY_DONTUPDATECAMERA;
					ProcessTazDeathAtEndOfBossGame(ResetMechaTweetyCallback, (void*)globalMechaTweety->mechaTweetyInfo);
//					bkGenerateEvent("scenechange","_tazboss2");
				}
			}
		}
	}
	return FALSE;

	if(collider->actorStatus)
	{
		if(collider->actorStatus->currentState == STATE_SPIN)
		{
			collider->flags |= ACTORFLAG_DESTROYNEXTUPDATE;
			crate->tazBossItemInfo->flags |= BOSSITEMFLAG_DESTROYASAP;
			return FALSE;
		}
	}

	return FALSE;
}