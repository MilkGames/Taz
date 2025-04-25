// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : taskforce.cpp
//   Purpose : Task force AI
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "actors.h"
#include "genericai.h"
#include "maths.h"
#include "status.h"
#include "animation.h"
#include "collisioncache.h"
#include "scene.h"
#include "main.h"
#include "characters.h"
#include "physics.h"
#include "taskforce.h"
#include "control.h"
#include "sfx.h"

#define SHADOWMAP_RADIUS_TASKFORCE			METERTOUNIT(5.5)

/* ANIM LIST

	run	
	Attack1	
	Dragonground1	Bounce crapply
	Walk1	
	Sleepcycle	
	Wakeup	
	Idle1			Breathe
	Idle2			Look around
	Netidle			Pull at taz
	React1			Bite from rear
	React1run	
	React2			Bite from front
	React2run		Run backwards
	React3			scare
	React3cycle		Cringe cycle
	Death1			Spun off screen
	Dragonground2	Standing skid
	Death2			KO from front
	Recover2		Ninja backflip to stand
	Death3			KO from behind
	Recover3		Get up

*/

// TP: ------------------------------------------------ SCOUT
static SEQ idle[] = 
{
	{SEQ_RETURN,"walk1",0},
	{SEQ_PLAY,"idle1",0},
	{SEQ_PLAY,"idle2",0},
	{SEQ_WALK,"walk1",1},
	{SEQ_TURNLEFT,NULL,90},
	{SEQ_WALK,"walk1",1},
	{SEQ_TURNLEFT,NULL,90},
	{SEQ_WALK,"walk1",1},
	{SEQ_TURNLEFT,NULL,90},
	{SEQ_WALK,"walk1",1},
	{SEQ_TURNLEFT,NULL,90},
	{SEQ_END,NULL,0},
};

static SEQ aware[] =
{
	{SEQ_RETURN,"walk1",0},
	{SEQ_SETROTSPEED,NULL,RAD(180.0f)},
	{SEQ_FACETAZ,NULL,0},
	// TP: weak point
	{SEQ_PLAY,"idle4",0},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_SETANIMSPEED,NULL,0.7f},
	{SEQ_PLAY,"idle1",0},
	{SEQ_SETANIMSPEED,NULL,1.0f},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_ATTACK,"attack",0},

	{SEQ_END,NULL,0},
};

static SEQ attack[] = 
{
	{SEQ_SETROTSPEED,NULL,RAD(90.0f)},
	{SEQ_ATTACKANDFACE,"attack",0},
	{SEQ_END,NULL,0},
};

static SEQ spinFront[] =
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAY,"tazbounceoff",0},
	{SEQ_END,NULL,0},
};

static SEQ costumeAttack[] =
{
	{SEQ_PLAY,"death3",0},
	{SEQ_DEATH,NULL,0},
	{SEQ_END,NULL,0},
};

static SEQ spinRear[] =
{
	{SEQ_LOOKAWAYFROMTAZ,NULL,0},
	{SEQ_PLAYSAMPLE,"taskforce_10.wav",FALSE},
	{SEQ_PLAY,"death3",0},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_DISABLED,NULL,10.0f},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_END,NULL,0},
};

static SEQ biteFront[] =
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAY,"tazbounceoff",0},
	{SEQ_END,NULL,0},
};

static SEQ biteRear[] =
{
	{SEQ_LOOKAWAYFROMTAZ,NULL,0},
	{SEQ_PLAYSAMPLE,"taskforce_10.wav",FALSE},
	{SEQ_PLAY,"react2",0},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_DISABLED,"react2cycle",10.0f},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_END,NULL,0},
};

static SEQ scareFront[] =
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAY,"tazbounceoff",0},
	{SEQ_END,NULL,0},
};

static SEQ scareRear[] = 
{
	{SEQ_LOOKAWAYFROMTAZ,NULL,0},
	{SEQ_PLAYSAMPLE,"taskforce_10.wav",FALSE},
	{SEQ_PLAY,"react2",0},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_DISABLED,"react2cycle",10.0f},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
};

static SEQ tooClose[] = 
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_ATTACK,"attack",0},
	{SEQ_END,NULL,0},
};

static SEQ outOfRange[] =
{
	{SEQ_RETURN,"walk1",0},
	{SEQ_END,NULL,0},
};

/* --------------------------------------------------------------------------------
   Function : TaskForceIdleSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int TaskForceIdleSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = idle;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : TaskForceAwareSeq
   Purpose : Respond to requests for aware seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int TaskForceAwareSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = aware;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	if(bmRand()%2)
	{
		switch(bmRand()%5)
		{
		case 0:	PlaySample("taskforce_1.wav", 255, actorInstance->actorInstance.position);	break;
		case 1:	PlaySample("taskforce_2.wav", 255, actorInstance->actorInstance.position);	break;
		case 2:	PlaySample("taskforce_3.wav", 255, actorInstance->actorInstance.position);	break;
		case 3:	PlaySample("taskforce_4.wav", 255, actorInstance->actorInstance.position);	break;
		case 4:	PlaySample("taskforce_5.wav", 255, actorInstance->actorInstance.position);	break;
		}
	}

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : TaskForceAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int TaskForceAttackSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	if (bmRand()%2)
	{
		switch(bmRand()%3)
		{
		case 0:	PlaySample("taskforce_7.wav", 255, actorInstance->actorInstance.position);	break;
		case 1:	PlaySample("taskforce_8.wav", 255, actorInstance->actorInstance.position);	break;
		case 2:	PlaySample("taskforce_9.wav", 255, actorInstance->actorInstance.position);	break;
		}
	}

	actorInstance->genericAI->currentSeq = attack;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : TaskForceAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int TaskForcePursuitAttackSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = attack;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : TaskForceTooCloseSeq
   Purpose : Respond to requests for defend seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int TaskForceTooCloseSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = tooClose;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : TaskForceAttackedSeq
   Purpose : Respond to requests for attacked seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int TaskForceAttackedSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	int			random;

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
		actorInstance->genericAI->currentSeq = costumeAttack;
		gameStatus.player1->characterInfo->loseCostume = true;
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
   Function : TaskForceOutOfRangeSeq
   Purpose : Respond to requests for out of range seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int TaskForceOutOfRangeSeq(struct TAG_ACTORINSTANCE *actorInstance)
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
	FlushAnimationQueue(actorInstance);
	if((bmRand()%4) == 0)
	{
		PlayAnimationByName(actorInstance, "fly2", 1.0f, TRUE, 0, 0.1f, NONE);
	}
	else
	{
		PlayAnimationByName(actorInstance, "fly1", 1.0f, TRUE, 0, 0.1f, NONE);
	}
	actorInstance->actorAnimation->useMotionBone = FALSE;
}

/* --------------------------------------------------------------------------------
   Function : TaskForceAttackCallback
   Purpose : Called each frame during an attack 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
int TaskForceAttackCallback(struct TAG_ACTORINSTANCE *actorInstance)
{
	// TP: Check for distance between the glovestem and taz
	if ((DistFromNodeToTaz(actorInstance,"glovestem") < (gameStatus.player1->actorBehaviour->collSphere.radius+METERTOUNIT(0.5f)))&&(gameStatus.player1->actorStatus->currentState!=STATE_PROJECTILESLIDE))
	{
		// TP: has hit taz

		// TP: now catapult Taz
		TBVector	tempVector;
		float		tempFloat;

		bmVectorSub(tempVector, gameStatus.player1->actorInstance.position, actorInstance->actorInstance.position);
		tempFloat = bmVectorLen(tempVector);
		if (tempFloat)
		{
			bmVectorCDiv(tempVector,tempVector,tempFloat);
			tempVector[Y] = 0.5f;
			bmVectorCMul(tempVector,tempVector,METERTOUNIT(13.0f));
			bmVectorCopy(gameStatus.player1->actorBehaviour->physics.velocity, tempVector);
		}
		else
		{
			// TP: TODO throw taz in the direction the box is facing
		}

		PlaySample("whip.wav",255);

		// TP: UseStateChange callback
		RequestStateChangePlus(gameStatus.player1, STATE_PROJECTILESLIDE, myStateChangeCallback);

		// TP: Send taz to the cage
		CaptureTaz(actorInstance);
	}

	return FALSE;	// continue processing pattern
}

/*  --------------------------------------------------------------------------------
	Function	: ChangeState
	Purpose		: Called when the character changes state
	Parameters	: actorInstance, new state, last state
	Returns		: 
	Info		: 
*/
static void myChangeState(struct TAG_ACTORINSTANCE *actorInstance,int state,int lastState)
{
}

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceTaskForce
   Purpose : Setup the basic ai for a TaskForce
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceTaskForce(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList)
{
	GENERICAI	*genericAI;
	TBVector	tempVector;

	// create extra structures
	CreateActorInstanceBehaviour(actorInstance);
	CreateActorInstanceAnimation(actorInstance);
	CreateActorInstanceStatus(actorInstance);
	//CreateActorInstanceShadow(actorInstance,METERTOUNIT(0.7f),METERTOUNIT(2.5f), SHADOWMAP_RADIUS_TASKFORCE);
	// TP: CreateActorInstanceCharacterInfo(actorInstance);
	SetActorInertia(actorInstance->actorBehaviour,0);
	SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
	SetActorMaxSpeed(actorInstance->actorBehaviour,METERTOUNIT(9)); // set max speed 8mps
	SETVECTOR(tempVector,0.0f,METERTOUNIT(1.0f),0.0f,1.0f);
	SetActorCylinder(0,actorInstance->actorBehaviour,tempVector,METERTOUNIT(0.7f),METERTOUNIT(2.0f));
	AddActorToCollisionCylinderCache(&collisionCache,actorInstance);
	
	// create ai structure
	if (CreateActorInstanceGenericAI(actorInstance,activeList,inactiveList)==FALSE) return FALSE;
	
	AISetShadow(actorInstance,METERTOUNIT(0.7f),METERTOUNIT(2.5f), SHADOWMAP_RADIUS_TASKFORCE);

	// fill in custom parms
	genericAI = actorInstance->genericAI;
	bmVectorCopy(genericAI->center,actorInstance->actorInstance.position);

	genericAI->deactivationRange = METERTOUNIT(12.0f);
	genericAI->awarenessRange = METERTOUNIT(8.0f);
	genericAI->movementRange = METERTOUNIT(3.5f);
	genericAI->personalSpace = METERTOUNIT(0.5f);
	genericAI->standOffRange = METERTOUNIT(2.2f);
	genericAI->attackHoldDistance = METERTOUNIT(2.0f);

	genericAI->viewAngle = PI/3;
	genericAI->awareAngle = PI*0.85f;

	genericAI->instantAttack = true;
	genericAI->instantAware = true;
	genericAI->alwaysClipPosition = true;

	genericAI->currentRotTime = RAD(90.0f)/60.0f;

	genericAI->headNode = "spinetop";
	SETVECTOR(genericAI->starsOffset,0.0f,METERTOUNIT(0.5f),0.0f,1.0f);
	SETVECTOR(genericAI->starVars,METERTOUNIT(0.3f),METERTOUNIT(0.4f),METERTOUNIT(0.0f),METERTOUNIT(0.0f));

	SetAISeqFunctions(actorInstance,TaskForceIdleSeq,TaskForceAwareSeq,TaskForceAttackSeq,
		TaskForceTooCloseSeq,TaskForceAttackedSeq,TaskForceOutOfRangeSeq,0,TaskForcePursuitAttackSeq);

	SetAIAttackCallback(actorInstance,TaskForceAttackCallback);
	SetAIChangeStateCallback(actorInstance,myChangeState);

	genericAI->runCRC = bkCRC32((uchar*)"run",3,0);

	return TRUE;
}
