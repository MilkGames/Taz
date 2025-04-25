// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : constructionbot.cpp
//   Purpose : ai functions for construction bot
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "genericai.h"
#include "maths.h"
#include "status.h"
#include "animation.h"
#include "collisioncache.h"
#include "scene.h"
#include "main.h"
#include "characters.h"
#include "physics.h"
#include "control.h"
#include "clouds.h"
#include "sfx.h"
#include "tazcatcher.h"
#include "snowball.h"
#include "bears.h"
#include "zzz.h"
#include "constructionworker.h"

// PP: TEMP?: radius of Construction Bot for shadow map
// PP: TODO? (how?): get bounds of actor instances on the fly so's to optimise shadow map resolution
#define SHADOWMAP_RADIUS_CONSTRUCTIONWORKER		METERTOUNIT(3.0f)

static SEQ idle[] = 
{
	{SEQ_SETROTSPEED,NULL,RAD(90.0f)},
	{SEQ_RETURN,"walk1",0},
	{SEQ_ROTATERIGHT,"walk1",3.0f},
	{SEQ_END,NULL,0},
};

static SEQ skipIdle[] = 
{
	{SEQ_SETROTSPEED,NULL,RAD(180.0f)},
	{SEQ_RETURN,"walk3",0},
	{SEQ_SETROTSPEED,NULL,RAD(90.0f)},
	{SEQ_ROTATERIGHT,"walk3",3.0f},
	{SEQ_END,NULL,0},
};

static SEQ snackIdle[] = 
{
	{SEQ_RETURN,"walk1",0},
	{SEQ_PLAY,"idle1",2.0f},
	{SEQ_PLAY,"idle2",0},
	{SEQ_PLAY,"idle3",2.0f},
	{SEQ_PLAYSAMPLE, "construction_8.wav", FALSE},
	{SEQ_PLAY,"idle4",0},
	{SEQ_PLAY,"idle3",2.0f},
	{SEQ_PLAY,"idle5",0},
	{SEQ_PLAY,"idle6",0},
	{SEQ_END,NULL,0},
};

static SEQ attack[] = 
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_CHASE,"walk3",0},
	{SEQ_ATTACK,"attack1",0},
	{SEQ_END,NULL,0},
};

static SEQ hatAttack[] = 
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_CHASE,"walk3",0},
	{SEQ_ATTACK,"attack5",0},
	{SEQ_ATTACK,"attack1",0},
	{SEQ_END,NULL,0},
};

static SEQ bellyFlop[] = 
{
	{SEQ_SETROTSPEED,NULL,RAD(180.0f)},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,true},
	{SEQ_CHASE,"walk2",0},

	{SEQ_LOCKSEQ,NULL,true},
	{SEQ_ATTACK,"attack3",0},
	{SEQ_LOCKSEQ,NULL,false},

	{SEQ_MESSAGE,"collision",0},
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},
	{SEQ_END,NULL,0},
};

static SEQ aware[] = 
{
	{SEQ_ATTACKANDFACE, "attack6",0},
	{SEQ_PLAY,"idle1",3.0f},
	{SEQ_PLAYANDFACE,"react2",0},
	{SEQ_PLAYANDFACE,"react3",0},
	{SEQ_PLAYANDFACE,"react4",0},
	{SEQ_RETURN,"walk1",0},
	{SEQ_END,NULL,0},
};

static SEQ defend[] =
{
	{SEQ_LOOP,NULL,0},
};

static SEQ smellCheese[] = 
{
	{SEQ_PLAY,"react10",0},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_DISABLED,NULL,10.0f},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAY,"idle14",0},
	{SEQ_END,NULL,0},
};

static SEQ spinFront[] = 
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAY,"react5",0},
	{SEQ_END,NULL,0},
};

static SEQ spinRear[] =
{
	{SEQ_PLAYSAMPLE, "zkoof.wav", FALSE},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAY,"react6",0},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_DISABLED,NULL,5.0f},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAY,"idle14",0},
	{SEQ_END,NULL,0},
};

static SEQ biteFront[] =
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_ATTACK,"attack5",0},
	{SEQ_ATTACK,"attack1",0},
	{SEQ_PLAY,"react3",0},
	{SEQ_PLAY,"react4",0},
	{SEQ_END,NULL,0},
};

static SEQ biteRear[] =
{
	{SEQ_PLAYSAMPLE,"construction_2.wav", FALSE},
	{SEQ_LOOKAWAYFROMTAZ,NULL,0},
	{SEQ_PLAY,"react7",0},
	{SEQ_ROTATELEFT,"react8",2.0f},
	{SEQ_RETURN,"walk3",0},
	{SEQ_END,NULL,0},
};

static SEQ scareFront[] =
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAY,"react1",0},
	{SEQ_PLAYSAMPLE, "construction_3.wav", FALSE},
	{SEQ_PLAY,"react2",0},
	{SEQ_PLAY,"react3",0},
	{SEQ_PLAY,"react4",0},
	{SEQ_END,NULL,0},
};

static SEQ scareRear[] = 
{
	{SEQ_PLAYSAMPLE, "construction_4.wav", FALSE},
	{SEQ_PLAY,"react9",0},
	{SEQ_PLAY,"idle10",1.0f},
	{SEQ_PLAY,"idle11",1.0f},
	{SEQ_PLAY,"idle15",0},
	{SEQ_PLAY,"idle12",0},
	{SEQ_END,NULL,0},
};

static SEQ tooClose[] = 
{
	{SEQ_PLAY,"walk5",0},
	{SEQ_END,NULL,0},
};

static SEQ outOfRange[] =
{
	{SEQ_END,NULL,0},
};

static SEQ puzzled[] = 
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAYSAMPLE,"construction_1.wav", FALSE},
	{SEQ_STARTQUESTION,NULL,0},
	{SEQ_PLAY,"disguise",0},
	{SEQ_COSTUMEDONE,NULL,true},
	{SEQ_PLAY,"react4",0},
	{SEQ_SETROTSPEED,NULL,RAD(180.0f)},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_LOCKSEQ,NULL,true},
	{SEQ_CHASE,"walk2",0},
	{SEQ_ATTACK,"attack3",0},
	{SEQ_MESSAGE,"collision",0},
	{SEQ_LOCKSEQ,NULL,false},
	{SEQ_RETURN,"walk1",0},
	{SEQ_END,NULL,0},
};


/* --------------------------------------------------------------------------------
   Function : ConstructionWorkerIdleSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ConstructionWorkerIdleSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	int			number;

	number = bmRand()%3;

	switch(number)
	{
	case 0:
		actorInstance->genericAI->currentSeq = idle;
		break;
	case 1:
		actorInstance->genericAI->currentSeq = skipIdle;
		break;
	case 2:
		actorInstance->genericAI->currentSeq = snackIdle;
		break;
	}
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ConstructionWorkerAwareSeq
   Purpose : Respond to requests for aware seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ConstructionWorkerAwareSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	int	number;

	number = bmRand()%3;

	actorInstance->genericAI->currentSeq = aware;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	if (bmRand()%2)
	{
		switch (bmRand()%3)
		{
		case 0:	PlaySample("construction_5.wav", 255, actorInstance->actorInstance.position);	break;
		case 1:	PlaySample("construction_6.wav", 255, actorInstance->actorInstance.position);	break;
		case 2:	PlaySample("construction_7.wav", 255, actorInstance->actorInstance.position);	break;
		}
	}

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ConstructionWorkerAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ConstructionWorkerAttackSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = bellyFlop;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ConstructionWorkerTooCloseSeq
   Purpose : Respond to requests for defend seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ConstructionWorkerTooCloseSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	if(bmRand()%2)
	{
		actorInstance->genericAI->currentSeq = attack;
	}
	else
	{
		actorInstance->genericAI->currentSeq = hatAttack;
	}
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ConstructionWorkerPuzzledSeq
   Purpose : Respond to requests for puzzled seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/

int ConstructionWorkerPuzzledSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = puzzled;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ConstructionWorkerAttackedSeq
   Purpose : Respond to requests for attacked seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ConstructionWorkerAttackedSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	int		random;

	if(characterSounds.sam == -1)
	{
		random = bmRand()%2;
		switch(random)
		{
		case 0:
			CallbackWhenSampleFinished((characterSounds.sam=PlaySample("sam_voc_15a.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.sam);
			break;
		case 1:
			CallbackWhenSampleFinished((characterSounds.sam=PlaySample("sam_voc_17b.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.sam);
			break;
		}
	}

	switch(actorInstance->genericAI->typeOfAttack)
	{
	case ATTACKTYPE_BITEFRONT:
		actorInstance->genericAI->currentSeq = biteFront;
		break;
	case ATTACKTYPE_BITEREAR:
		actorInstance->genericAI->currentSeq = biteRear;
		break;
	case ATTACKTYPE_SPINFRONT:
		actorInstance->genericAI->currentSeq = spinFront;
		break;
	case ATTACKTYPE_COSTUMEFRONT:
	case ATTACKTYPE_COSTUMEREAR:
		actorInstance->genericAI->currentSeq = smellCheese;
		break;
	case ATTACKTYPE_SPINREAR:
	case ATTACKTYPE_SPITFRONT:
	case ATTACKTYPE_SPITREAR:
		actorInstance->genericAI->currentSeq = spinRear;
		break;
	case ATTACKTYPE_SCAREFRONT:
		actorInstance->genericAI->currentSeq = scareFront;
		break;
	case ATTACKTYPE_SCAREREAR:
		actorInstance->genericAI->currentSeq = scareRear;
		break;
	}
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ConstructionWorkerOutOfRangeSeq
   Purpose : Respond to requests for out of range seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ConstructionWorkerOutOfRangeSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = outOfRange;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/*  --------------------------------------------------------------------------------
	Function	: myStateChangeCallback
	Purpose		: standard callback thingy
	Parameters	: 
	Returns		: 
	Info		: 
*/
static void myStateChangeCallback(ACTORINSTANCE *actorInstance)
{
	float		yangle;
	TBVector	tempVector;

	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance, "fly2", 1.0f, TRUE, 0, 0.3f, NONE);

	actorInstance->actorAnimation->useMotionBone = FALSE;

	yangle = GetAngleAroundYFromQuat(gameStatus.player1->actorInstance.orientation);

	bmVectorSet(tempVector,0.0f,0.7f,1.0f,1.0f);

	bmVectorRotateY(tempVector,tempVector,Aabs(-yangle+PI));

	bmVectorCMul(gameStatus.player1->actorBehaviour->physics.velocity, tempVector, METERTOUNIT(6.0f));
}

/*  --------------------------------------------------------------------------------
	Function	: ConstructionWorkerEvent
	Purpose		: Called with any event messages sent by the animation
	Parameters	: actorInstance, type
	Returns		: 
	Info		: 
*/
void ConstructionWorkerEvent(ACTORINSTANCE *actorInstance, int type)
{
	char	*bone;

	switch(type)
	{
	case 1:
		bone = "root";
		break;
	case 3:
		bone = "rhand";
		break;
	case 4:
		bone = "hattip";
		break;
	case 5:
		bone = "root";
		break;
	case 6:
		bone = "lhand";
		break;
	}

	switch(type)
	{
	case 1:
		if (DistFromNodeToTaz(actorInstance,bone) < METERTOUNIT(1.25f))
		{
			if (RequestStateChange(gameStatus.player1,STATE_SQUASHED))
			{
				if (UpdateCharacterState(&gameStatus.player[0], true))
				{
					FlushAnimationQueue(gameStatus.player1);
					PlayAnimationByName(gameStatus.player1,"recover21b",1.0f,0,0,0.1f,NONE);
					StopCollidingWith(gameStatus.player1, actorInstance);
					StopCollidingWith(actorInstance, gameStatus.player1);

					RequestRumbleEffect(gameStatus.player[0].controller, 180, 0.0f, 0.5f, 1.5f, 0.0f, WAVEFORM_SQUAREDSINUSOIDAL, 1.0f);
				}
				else
				{
					ClearStateChangeRequest(gameStatus.player1);
				}
			}
		}
		break;
	case 5:
		if (DistFromNodeToTaz(actorInstance,bone) < METERTOUNIT(1.25f))
		{
			RequestStateChangePlus(gameStatus.player1, STATE_PROJECTILESLIDE, myStateChangeCallback);
			FacePoint(gameStatus.player1, actorInstance->actorInstance.position, 0.0f, RAD(180.0f));
		}
		break;
	case 2:
		if (CheckIfPlayingAnimation(gameStatus.player1,"recover21b"))
		{
			
			RequestStateChangePlus(gameStatus.player1, STATE_PROJECTILESLIDE, myStateChangeCallback);
			FacePoint(gameStatus.player1, actorInstance->actorInstance.position, 0.0f, RAD(180.0f));
		}
		break;
	case 3:
	case 4:
		if (DistFromNodeToTaz(actorInstance,bone) < METERTOUNIT(1.25f))
		{
			if (RequestStateChange(gameStatus.player1,STATE_PLAYANIMATION))
			{
				if (UpdateCharacterState(&gameStatus.player[0], true))
				{
					FlushAnimationQueue(gameStatus.player1);
					PlayAnimationByName(gameStatus.player1,"react2",1.0f,0,0,0.1f,NONE);

					// TP: Turn taz to face character
					FacePoint(gameStatus.player1,actorInstance->actorInstance.position,0.0f,0.0f);

					RequestRumbleEffect(gameStatus.player[0].controller, 180, 0.0f, 0.5f, 0.75f, 0.0f, WAVEFORM_NORMAL);
				}
				else
				{
					ClearStateChangeRequest(gameStatus.player1);
				}
			}
		}
		break;
	case 6:
		if (DistFromNodeToTaz(actorInstance,bone) < METERTOUNIT(1.2f))
		{
			if (RequestStateChange(gameStatus.player1,STATE_PLAYANIMATION))
			{
				if (UpdateCharacterState(&gameStatus.player[0], true))
				{
					FlushAnimationQueue(gameStatus.player1);
					PlayAnimationByName(gameStatus.player1,"react1",1.0f,0,0,0.1f,NONE);

					// TP: Turn taz to face character
					FacePoint(gameStatus.player1,actorInstance->actorInstance.position,0.0f,0.0f);
					RequestRumbleEffect(gameStatus.player[0].controller, 180, 0.0f, 0.5f, 0.75f, 0.0f, WAVEFORM_NORMAL);
				}
				else
				{
					ClearStateChangeRequest(gameStatus.player1);
				}
			}
		}
		break;
	}
}

/* --------------------------------------------------------------------------------
   Function : ConstructionWorkerAttackCallback
   Purpose : Called each frame during an attack 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
int ConstructionWorkerAttackCallback(struct TAG_ACTORINSTANCE *actorInstance)
{
	return FALSE;	// continue processing pattern
}

/* --------------------------------------------------------------------------------
   Function : ConstructionWorkerStateChangeCallback
   Purpose : Called when state changed 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void ConstructionWorkerStateChangeCallback(ACTORINSTANCE *actorInstance, int state, int lastState)
{
	if(lastState == AISTATE_IDLE)
	{
		if((CheckIfPlayingAnimation(actorInstance,"idle2"))||CheckIfPlayingAnimation(actorInstance,"idle3")
			||CheckIfPlayingAnimation(actorInstance,"idle4")||CheckIfPlayingAnimation(actorInstance,"idle5")
			||CheckIfPlayingAnimation(actorInstance,"idle6"))
		{
			FlushAnimationQueue(actorInstance);
			PlayAnimationByName(actorInstance,"idle7",1.0f,0,0,0.1f,IDLE);
		}
		DestroyInjuryStars(actorInstance);
	}
	if(lastState == AISTATE_COSTUME)
	{
		DestroyQuestionMarkAboveActor(actorInstance);
	}
	if(lastState == AISTATE_AWARE)
	{
		DestroyQuestionMarkAboveActor(actorInstance);
	}
}

/* --------------------------------------------------------------------------------
   Function : ConstructionWorkerTazSquashedCallback
   Purpose : Called when state changed 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int ConstructionWorkerTazSquashedCallback(ACTORINSTANCE *actorInstance)
{
	TAZCATCHERINFO	*botInfo = (TAZCATCHERINFO*)actorInstance->genericAI->customData;

	if((botInfo->timeSinceLastAttack += fTime) > 2.0f)
	{
		SetAIAttackCallback(actorInstance,ConstructionWorkerAttackCallback);

		// sort out taz catcher
		FlushAnimationQueue(actorInstance);

		ResetCharacterAfterJump(actorInstance);

		actorInstance->genericAI->flags &= ~GENERICAI_DONTCHANGESTATE;
		return FALSE;
	}
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ConstructionWorkerMessageHandler
   Purpose : Called when a message is sent in the sequence
   Parameters : ACTORINSTANCE *, char *message
   Returns : 
   Info : 
*/
int ConstructionWorkerMessageHandler(ACTORINSTANCE *actorInstance, char *message)
{
	TBVector	tempVector;
	TBVector	pos;
	ACTORINSTANCE *tempInstance;
	TBActorNodeInstance	*node;

	// TP: Construction worker
	if (strcmpi(message,"cwsquash")==0)
	{
		ConstructionWorkerEvent(actorInstance,1);
		return true;
	}
	if (strcmpi(message,"cwkoick")==0)
	{
		ConstructionWorkerEvent(actorInstance,2);
		return true;
	}
	if (strcmpi(message,"cwpunch")==0)
	{
		ConstructionWorkerEvent(actorInstance,3);
		return true;
	}
	if (strcmpi(message,"cwhat")==0)
	{
		ConstructionWorkerEvent(actorInstance,4);
	}
	if (strcmpi(message,"cwbelly")==0)
	{
		ConstructionWorkerEvent(actorInstance,5);
		return true;
	}
	if (strcmpi(message,"cwflick")==0)
	{
		ConstructionWorkerEvent(actorInstance,6);
	}

	if (strcmpi(message,"collision")==0)
	{
		StopCollidingWith(gameStatus.player1,NULL);
	}

	if (strcmpi(message,"throw")==0)
	{
		bmVectorCopy(pos,gameStatus.player1->actorInstance.position);
		pos[Y] += METERTOUNIT(0.5f);
		node = baFindNode(actorInstance->actorInstance.rootNodeInstance,"bullet");
		if(node)
		{
			baGetNodesWorldPosition(&actorInstance->actorInstance, node, tempVector);
			tempInstance = CreateSnowball(actorInstance, tempVector, tempVector, METERTOUNIT(0.5f),pos,SNOWBALL_MEDIUM,"rivet.obe")->actorStructure;
		}
		if (tempInstance) SetCollisionCallback(tempInstance,SnowballCollision);
	}
	return false;
}

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceConstructionWorker
   Purpose : Setup the basic ai for a zookeeper
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceConstructionWorker(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList)
{
	GENERICAI	*genericAI;
	TBVector	tempVector;

	// create extra structures
	CreateActorInstanceBehaviour(actorInstance);
	CreateActorInstanceAnimation(actorInstance);
	CreateActorInstanceStatus(actorInstance);
	//CreateActorInstanceShadow(actorInstance,METERTOUNIT(1.5f),METERTOUNIT(2.5f), SHADOWMAP_RADIUS_CONSTRUCTIONWORKER);
	// TP: CreateActorInstanceCharacterInfo(actorInstance);
	SetActorInertia(actorInstance->actorBehaviour,0);
	SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
	SetActorMaxSpeed(actorInstance->actorBehaviour,METERTOUNIT(15)); // set max speed 8mps
	SETVECTOR(tempVector,0.0f,METERTOUNIT(1.0f),0.0f,1.0f);
	SetActorCylinder(0,actorInstance->actorBehaviour,tempVector,METERTOUNIT(0.5f),METERTOUNIT(2.0f));
	AddActorToCollisionCylinderCache(&collisionCache,actorInstance);
	
	// create ai structure
	if (CreateActorInstanceGenericAI(actorInstance,activeList,inactiveList)==FALSE) return FALSE;

	AISetShadow(actorInstance,METERTOUNIT(1.5f),METERTOUNIT(2.5f), SHADOWMAP_RADIUS_CONSTRUCTIONWORKER);

	// fill in custom parms
	genericAI = actorInstance->genericAI;
	bmVectorCopy(genericAI->center,actorInstance->actorInstance.position);
	genericAI->deactivationRange = METERTOUNIT(12.0f);
	genericAI->awarenessRange = METERTOUNIT(6.0f);
	genericAI->movementRange = METERTOUNIT(3.0f);
	genericAI->standOffRange = METERTOUNIT(1.9f);
	genericAI->personalSpace = METERTOUNIT(1.1f);
	genericAI->viewAngle = PI/2;

	genericAI->alwaysClipPosition = true;
	genericAI->instantAttack = true;
	genericAI->instantAware = true;
	genericAI->costumeInAware = true;
	genericAI->bounceCRC = bkCRC32((uchar*)"bounce",6,0);


	genericAI->headNode = "hattip";
	SETVECTOR(genericAI->starsOffset,0.0f,METERTOUNIT(0.2f),0.0f,1.0f);
	SETVECTOR(genericAI->starVars,METERTOUNIT(0.3f),METERTOUNIT(0.4f),METERTOUNIT(0.0f),METERTOUNIT(0.0f));

	SetAISeqFunctions(actorInstance,ConstructionWorkerIdleSeq,ConstructionWorkerAwareSeq,ConstructionWorkerAttackSeq,
		ConstructionWorkerTooCloseSeq,ConstructionWorkerAttackedSeq,ConstructionWorkerOutOfRangeSeq,ConstructionWorkerPuzzledSeq);

	SetAIAttackCallback(actorInstance,ConstructionWorkerAttackCallback);
	SetAIChangeStateCallback(actorInstance, ConstructionWorkerStateChangeCallback);
	SetAIMessageHandler(actorInstance,ConstructionWorkerMessageHandler);

	genericAI->customData = (TAZCATCHERINFO*)ZALLOC(sizeof(TAZCATCHERINFO));
	ASSERT(genericAI->customData);

	return TRUE;
}
