// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : mailbox.cpp
//   Purpose : function associated with mailboxes
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "collisioncache.h"
#include "physics.h"
#include "scene.h"
#include "characters.h"
#include "main.h"
#include "animation.h"
#include "trigger.h"
#include "maths.h"
#include "status.h"
#include "playerstats.h"
#include "mailbox.h"
#include "sfx.h"

/* --------------------------------------------------------------------------------
   Function : CreateMailboxInstance
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateMailboxInstance(ACTORINSTANCE *mailbox, TBVector position, int32 mailboxNumber, uint32 state)
{
	CreateActorInstanceBehaviour(mailbox);
	CreateMailboxInstanceInfo(mailbox);
	EnableActorLineRenderMode(&mailbox->actorInstance);
	
	if (mailboxNumber==-1)
	{
		bkPrintf("      *** ERROR *** No mailbox number specified.\n");
	}
	//fill in attributes
	mailbox->special = SPECIALTYPE_MAILBOX;
	if(state) 
	{
		mailbox->mailboxInfo->currentState = MAILBOX_FLAGDOWN;
		PlayAnimationByName(mailbox, "staticflagdown", 1.0f, TRUE, 0, 0.1f, NONE);
	}
	else
	{
		mailbox->mailboxInfo->currentState = MAILBOX_FLAGUP;
		PlayAnimationByName(mailbox, "staticflagup", 1.0f, TRUE, 0, 0.1f, NONE);
	}
	mailbox->mailboxInfo->mailboxNumber = mailboxNumber;
	mailbox->mailboxInfo->triggeredPlayer1 = FALSE;
	mailbox->mailboxInfo->triggeredPlayer2 = FALSE;
	SetActorPosition(mailbox, position);

	AddActorToCollisionCache(&collisionCache,mailbox, 0);
	//SetCollisionCallback(mailbox,RespondToMailboxCollision);
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : CreateSpringyActorInstanceInfo
   Purpose : Creates and initialises springy info part of object
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int CreateMailboxInstanceInfo(ACTORINSTANCE *actorInstance)
{
	actorInstance->mailboxInfo = (MAILBOXINFO*)ZALLOC(sizeof(MAILBOXINFO));
	if (actorInstance->springyObjectInfo)
	{
		//actorInstance->springyObjectInfo->type = SPECIALTYPE_SPRINGYOBJECT;
		return TRUE;
	}
	else
	{
		bkPrintf("*** ERROR *** Could not create mailbox info structure\n");
		return FALSE;
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateMailboxes
   Purpose : updates mailboxes
   Parameters : ACTORINSTANCELIST *
   Returns : 
   Info : 
*/

void UpdateMailboxes(ACTORINSTANCE *ptr)
{
	if(ptr->mailboxInfo)
	{
		UpdateIndividualMailbox(ptr, gameStatus.player1);
		//UpdateIndividualMailbox(ptr, gameStatus.player2);
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateIndividualMailbox
   Purpose : updates individual mailboxes
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void UpdateIndividualMailbox(ACTORINSTANCE *mailbox, ACTORINSTANCE *actorInstance)
{
	TBVector		relativePosition, pos;
	float			distance;
	uint32			player;
	char			message[256];

	if(actorInstance == NULL) return;
	if(actorInstance == gameStatus.player1) player = 1;
	if(actorInstance == gameStatus.player2) player = 2;

	switch(mailbox->mailboxInfo->currentState)
	{
	case MAILBOX_FLAGDOWN:
		if(!CubeDistanceTest(mailbox->actorInstance.position, actorInstance->actorInstance.position, METERTOUNIT(5.0f)))
		{
			mailbox->mailboxInfo->currentState = MAILBOX_FLAGDOWN;
			break;
		}
		bmVectorSub(relativePosition, mailbox->actorInstance.position, actorInstance->actorInstance.position);
		distance = bmVectorLen(relativePosition);

		if(distance < METERTOUNIT(6.0f))
		{
			mailbox->mailboxInfo->currentState = MAILBOX_EXCITED;
			PlayAnimationByName(mailbox, "slightlyexcited", 1.0f, TRUE, 0, 0.1f, NONE);
		}
		if(distance < METERTOUNIT(3.0f))
		{
			mailbox->mailboxInfo->currentState = MAILBOX_VERYEXCITED;
			PlayAnimationByName(mailbox, "veryexcited", 1.0f, TRUE, 0, 0.1f, NONE);
		}
		if(distance < METERTOUNIT(1.0f))
		{
			FlushAnimationQueue(mailbox);
			PlayAnimationByName(mailbox, "opening", 1.0f, FALSE, 0, 0.1f, NONE);
			PlayAnimationByName(mailbox, "staticflagup", 1.0f, TRUE, TRUE, 0.1f, NONE);
			AddMailboxToStats(mailbox, actorInstance, map.sceneNumber);
			mailbox->mailboxInfo->currentState = MAILBOX_FLAGUP;

		}
		break;
	case MAILBOX_FLAGUP:
		if(mailbox->mailboxInfo->triggeredPlayer1 == FALSE)
		{
			mailbox->mailboxInfo->triggeredPlayer1 = TRUE;
			bmVectorCopy(actorInstance->actorStatus->startPosition, actorInstance->actorInstance.position);
			bmVectorCopy(actorInstance->actorStatus->startQuaternion, actorInstance->actorInstance.orientation);

			//reset camera position
			// TP: PROBLEM
			bmVectorCopy(camera[0].resetPoint, camera[0].pos);

			bmVectorCopy(pos,mailbox->actorInstance.position);

			// send messages
			PlaySample("find mailbox 01.wav",255);
			if (mailbox->mailboxInfo->onContact)
				SendTrigger(mailbox->mailboxInfo->onContact, pos);

		}
		break;
	case MAILBOX_EXCITED:
		if(!CubeDistanceTest(mailbox->actorInstance.position, actorInstance->actorInstance.position, METERTOUNIT(5.0f)))
		{
			mailbox->mailboxInfo->currentState = MAILBOX_FLAGDOWN;
			break;
		}
		bmVectorSub(relativePosition, mailbox->actorInstance.position, actorInstance->actorInstance.position);
		distance = bmVectorLen(relativePosition);

		if(distance > METERTOUNIT(6.0f))
		{
			mailbox->mailboxInfo->currentState = MAILBOX_FLAGDOWN;
			break;
		}
		if(distance < METERTOUNIT(3.0f))
		{
			mailbox->mailboxInfo->currentState = MAILBOX_VERYEXCITED;
			PlayAnimationByName(mailbox, "veryexcited", 1.0f, TRUE, 0, 0.1f, NONE);
		}
		if(distance < METERTOUNIT(1.0f))
		{
			FlushAnimationQueue(mailbox);
			PlayAnimationByName(mailbox, "opening", 1.0f, FALSE, 0, 0.1f, NONE);
			PlayAnimationByName(mailbox, "staticflagup", 1.0f, TRUE, TRUE, 0.1f, NONE);
			AddMailboxToStats(mailbox, actorInstance, map.sceneNumber);
			mailbox->mailboxInfo->currentState = MAILBOX_FLAGUP;
		}
		break;
	case MAILBOX_VERYEXCITED:
		if(!CubeDistanceTest(mailbox->actorInstance.position, actorInstance->actorInstance.position, METERTOUNIT(5.0f)))
		{
			mailbox->mailboxInfo->currentState = MAILBOX_FLAGDOWN;
			break;
		}
		bmVectorSub(relativePosition, mailbox->actorInstance.position, actorInstance->actorInstance.position);
		distance = bmVectorLen(relativePosition);

		if(distance > METERTOUNIT(6.0f))
		{
			mailbox->mailboxInfo->currentState = MAILBOX_FLAGDOWN;
			break;
		}
		if(distance > METERTOUNIT(3.0f))
		{
			mailbox->mailboxInfo->currentState = MAILBOX_EXCITED;
			PlayAnimationByName(mailbox, "slightlyexcited", 1.0f, TRUE, 0, 0.1f, NONE);
		}
		if(distance < METERTOUNIT(1.0f))
		{
			FlushAnimationQueue(mailbox);
			PlayAnimationByName(mailbox, "opening", 1.0f, FALSE, 0, 0.1f, NONE);
			PlayAnimationByName(mailbox, "staticflagup", 1.0f, TRUE, TRUE, 0.1f, NONE);
			AddMailboxToStats(mailbox, actorInstance, map.sceneNumber);
			mailbox->mailboxInfo->currentState = MAILBOX_FLAGUP;
			// point taz at postbox
			bmVectorSub(relativePosition, mailbox->actorInstance.position, actorInstance->actorInstance.position);
			CharacterRotateToAngle(actorInstance,0,-(GetAngleAroundYFromVector(relativePosition)),0.0f,0.2f);
			bmQuatCopy(actorInstance->actorInstance.orientation,actorInstance->actorBehaviour->rotAim);
			// collect postcard
			RequestStateChange(actorInstance,STATE_COLLECTPOSTCARD);
		}
		break;
	}
}

/* --------------------------------------------------------------------------------
   Function : CubeDistanceTest
   Purpose : checks to see iftwo objects occupy cube of side length
   Parameters : TBVector, TBVector, float
   Returns : true, false
   Info : 
*/

int CubeDistanceTest(TBVector position1, TBVector position2, float distance)
{
	TBVector			relativePosition;

	bmVectorSub(relativePosition, position1, position2);

	if(bmFAbs(relativePosition[0]) > distance) return FALSE;
	if(bmFAbs(relativePosition[1]) > distance) return FALSE;
	if(bmFAbs(relativePosition[2]) > distance) return FALSE;

	return TRUE;
}