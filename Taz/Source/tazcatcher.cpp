// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : tazcatcher.cpp
//   Purpose : ai functions for tazcatcher
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

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
#include "control.h"
#include "clouds.h"
#include "sfx.h"
#include "tazcatcher.h"

// PP: TEMP?: radius of Taz Catcher for shadow map
// PP: TODO (how?): get bounds of actor instances on the fly so's to optimise shadow map resolution
#define SHADOWMAP_RADIUS_TAZCATCHER			METERTOUNIT(5.5f)

static SEQ idleMove[] = 
{
	{SEQ_RETURN,"walk1",0},
	{SEQ_WALK,"walk1",0.5f},
	{SEQ_TURNRIGHT,NULL,108.0f},
	{SEQ_WALK,"walk1",0.5f},
	{SEQ_RETURN,"walk1",0},
	{SEQ_END,NULL,0},
};

static SEQ idle1[] = 
{
	{SEQ_SETROTSPEED,NULL,RAD(90.0f)},
	{SEQ_RETURN,"walk1",0},
	{SEQ_PLAY,"idle1",5.0f},
	{SEQ_PLAY,"idle2",0},
	{SEQ_TURNLEFT,NULL,120.0f},
	{SEQ_PLAY,"idle1",1.5f},
	{SEQ_PLAY,"idle2",0},
	{SEQ_TURNLEFT,NULL,120.0f},
	{SEQ_PLAY,"idle1",1.5f},
	{SEQ_PLAY,"idle2",0},
	{SEQ_TURNLEFT,NULL,120.0f},
	{SEQ_END,NULL,0},
};

static SEQ idle2[] = 
{
	{SEQ_RETURN,"walk1",0},
	{SEQ_PLAY,"idle1",5.0f},
	{SEQ_PLAYSAMPLE,"hammerhit2.wav",FALSE},
	{SEQ_PLAY,"idle3",0},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_PLAY,"idle1",2.0f},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_END,NULL,0},
};

static SEQ idle3[] = 
{
	{SEQ_RETURN,"walk1",0},
	{SEQ_PLAY,"idle1",5},
	{SEQ_PLAYSAMPLE,"whirlhammer.wav",FALSE},
	{SEQ_PLAY,"idle4",0},
	{SEQ_TURNLEFT,"walk1",90.0f},
	{SEQ_END,NULL,0},
};

static SEQ aware[] =
{
	{SEQ_RETURN,"run",0},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAY,"idle2",0},
	{SEQ_PLAYSAMPLE,"blowwhistle2.wav",FALSE},
	{SEQ_PLAY,"blowwhistle2",0},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAY,"idle2",0},
	{SEQ_PLAYSAMPLE,"blowwhistle1.wav",FALSE},
	{SEQ_PLAY,"blowwhistle1",0},
	{SEQ_PLAY,"idle2",0},
	{SEQ_END,NULL,0},
};

static SEQ attack[] = 
{
	// TP: pursue and attack
	{SEQ_SETROTSPEED,NULL,RAD(180.0f)},
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,true},
	{SEQ_CHASE,"run",8},
	{SEQ_PLAYSAMPLE,"hammerhit.wav",FALSE},
	{SEQ_SETANIMSPEED,NULL,2.0f},
	{SEQ_ATTACK,"attack1",0},
	{SEQ_SETANIMSPEED,NULL,1.0f},
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},
	{SEQ_RETURN,"walk1",0},
	{SEQ_END,NULL,false},


	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAYSAMPLE,"blowwhistle1.wav",FALSE},
	{SEQ_PLAY,"blowwhistle1",0},
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},
	{SEQ_RETURN,"walk1",0},
	{SEQ_END,NULL,0},
};

static SEQ tooClose[] =
{
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,true},
	{SEQ_ATTACK,"attack1",0},
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},
	{SEQ_END,NULL,0},
};

static SEQ spinFrontSlow[] = 
{
	{SEQ_PLAYSAMPLE,"zkoof.wav",FALSE},
	{SEQ_PLAY,"death2",0},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_DISABLED,NULL,GENERICAI_KNOCKEDOUTTIME},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAYSAMPLE,"zkspringup",FALSE},
	{SEQ_PLAY,"recover2",0},
	{SEQ_END,NULL,0},
};

static SEQ spinFrontFast[] =
{
	{SEQ_PLAYSAMPLE,"zkyell.wav",FALSE},
	{SEQ_PLAY,"death1",0},
	{SEQ_PLAY,"death2",0},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_DISABLED,NULL,GENERICAI_KNOCKEDOUTTIME},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAYSAMPLE,"zkspringup.wav",FALSE},
	{SEQ_PLAY,"recover2",0},
	{SEQ_END,NULL,0},
};

static SEQ spinRear[] =
{
	{SEQ_PLAYSAMPLE,"zkoof.wav",FALSE},
	{SEQ_PLAY,"death3",0},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_DISABLED,NULL,GENERICAI_KNOCKEDOUTTIME},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAYSAMPLE,"zkspringup.wav",FALSE},
	{SEQ_PLAY,"recover3",0},
	{SEQ_END,NULL,0},
};

static SEQ biteFront[] =
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_CHASE,"run",0},
	{SEQ_ATTACK,"attack1",0},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_CHASE,"run",0},
	{SEQ_ATTACK,"attack2",0},
	{SEQ_END,NULL,0},
};

static SEQ biteRear[] =
{
	{SEQ_PLAYSAMPLE,"yeow.wav",FALSE},
	{SEQ_PLAY,"react1",0},
	{SEQ_FACECENTER,NULL,0},
	{SEQ_DISABLED,"react1run",1.0f},
	{SEQ_FACECENTER,NULL,0},
	{SEQ_DISABLED,"react1run",1.0f},
	{SEQ_END,NULL,0},
};

static SEQ scareFront[] =
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_CHASE,"run",0},
	{SEQ_ATTACK,"attack1",0},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_CHASE,"run",0},
	{SEQ_ATTACK,"attack2",0},
	{SEQ_END,NULL,0},
};

static SEQ scareRear[] = 
{
	{SEQ_PLAYSAMPLE,"zkshriek.wav",FALSE},
	{SEQ_PLAY,"react3",0},
	{SEQ_PLAYSAMPLE,"zkwhimper.wav",TRUE},
	{SEQ_DISABLED,"react3cycle",3.0f},
	{SEQ_STOPSAMPLE,NULL,0},
	{SEQ_PLAYSAMPLE,"zkgrunts.wav",FALSE},
	{SEQ_PLAY,"recover3",0},
	{SEQ_END,NULL,0},
};

static SEQ outOfRange[] =
{
	{SEQ_RETURN,"walk1",0},
	{SEQ_END,NULL,0},
};

static SEQ costumeAttack[] =
{
	{SEQ_LOOKAWAYFROMTAZ,NULL,0},
	{SEQ_PLAYSAMPLE,"zkoof.wav",TRUE},
	{SEQ_SETCYLRADIUS,NULL,METERTOUNIT(1.1f)},
	{SEQ_PLAY,"death3",0},
	{SEQ_DEATH,NULL,0},
	{SEQ_END,NULL,0},
};

/* --------------------------------------------------------------------------------
   Function : TazCatcherIdleSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int TazCatcherIdleSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	int			idleNum;

	if(actorInstance->genericAI->currentSeq == idleMove)
	{
		idleNum = bmRand()%3;

		switch(idleNum)
		{
		case 0:
			actorInstance->genericAI->currentSeq = idle1;
			break;
		case 1:
			actorInstance->genericAI->currentSeq = idle2;
			break;
		case 2:
			actorInstance->genericAI->currentSeq = idle3;
			break;
		default:
			actorInstance->genericAI->currentSeq = idle1;
			break;
		}
	}
	else
	{
		actorInstance->genericAI->currentSeq = idleMove;
	}
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : TazCatcherAwareSeq
   Purpose : Respond to requests for aware seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int TazCatcherAwareSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = aware;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	if (bmRand()%2)
	{
		switch(bmRand()%4)
		{
		case 0:	PlaySample("catcher_1.wav", 255, actorInstance->actorInstance.position);	break;
		case 1:	PlaySample("catcher_2.wav", 255, actorInstance->actorInstance.position);	break;
		case 2:	PlaySample("catcher_3.wav", 255, actorInstance->actorInstance.position);	break;
		case 3:	PlaySample("catcher_4.wav", 255, actorInstance->actorInstance.position);	break;
		}
	}

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : TazCatcherAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int TazCatcherAttackSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	if (CheckIfPlayingAnimation(actorInstance,"sleepcycle")) return FALSE;

	if (bmRand()%2)
	{
		switch(bmRand()%2)
		{
		case 0:	PlaySample("catcher_6.wav", 255, actorInstance->actorInstance.position);	break;
		case 1: PlaySample("catcher_7.wav", 255, actorInstance->actorInstance.position);	break;
		}
	}

	actorInstance->genericAI->currentSeq = attack;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : TazCatcherDefendSeq
   Purpose : Respond to requests for defend seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int TazCatcherTooCloseSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = tooClose;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : TazCatcherAttackedSeq
   Purpose : Respond to requests for attacked seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int TazCatcherAttackedSeq(struct TAG_ACTORINSTANCE *actorInstance)
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
		if(bmVectorLen(gameStatus.player1->actorBehaviour->physics.velocity) > METERTOUNIT(6.0f))
		{
			actorInstance->genericAI->currentSeq = spinFrontFast;
		}
		else
		{
			actorInstance->genericAI->currentSeq = spinFrontSlow;
		}
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
   Function : TazCatcherOutOfRangeSeq
   Purpose : Respond to requests for out of range seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int TazCatcherOutOfRangeSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = outOfRange;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : TazCatcherAttackCallback
   Purpose : Called each frame during an attack 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
int TazCatcherAttackCallback(struct TAG_ACTORINSTANCE *actorInstance)
{
	TBVector		starsBounds = {METERTOUNIT(0.3f), METERTOUNIT(0.4f), 0.0f, 0.0f};
	TBVector		starsOffset = {0.0f, METERTOUNIT(0.2f), 0.0f, 1.0f};
	TAZCATCHERINFO	*catcherInfo = (TAZCATCHERINFO*)actorInstance->genericAI->customData;
	
	if (DistFromNodeToTaz(actorInstance, "handle") < gameStatus.player1->actorBehaviour->collSphere.radius)
	{
		// hammer has hit taz
		FlushAnimationQueue(actorInstance);
		PlayAnimationByName(actorInstance, "blowwhistle2",1.0f,1,0,0.1f,NONE);
		PlaySample("catcher_14a.wav", 255, actorInstance->actorInstance.position);

		// TP: sort out taz
		ReduceCharactersHealth(gameStatus.player1,1.0f);
		RequestStateChange(gameStatus.player1,STATE_SQUASHED);
		CreateInjuryStars(10, gameStatus.player1, "spinetop", starsOffset, starsBounds);
		FlushAnimationQueue(gameStatus.player1);

		// TP: play recover anim on taz
		if(bmRand()%2)
		{
			PlayAnimationByName(gameStatus.player1,"recover22c",1.0f,0,0,0.1f,NONE);
			PlaySample("taz_gen_10a.wav",255);
		}
		else
		{
			PlayAnimationByName(gameStatus.player1,"recover21b",1.0f,0,0,0.1f,NONE);
		}
	
		CaptureTaz(actorInstance);

		catcherInfo->timeSinceLastAttack = 0.0f;
		return TRUE;	// don't continue pattern
	}
	return FALSE;	// continue processing pattern
}

/* --------------------------------------------------------------------------------
   Function : TazCatcherMessageHandler
   Purpose : Called when a message is sent in the sequence
   Parameters : ACTORINSTANCE *, char *message
   Returns : 
   Info : 
*/

int TazCatcherMessageHandler(ACTORINSTANCE *actorInstance, char *message)
{
	if (strcmpi(message,"catcherhit")==0)
	{
		TazCatcherAttackCallback(actorInstance);
		return true;
	}
	return false;
}

/* --------------------------------------------------------------------------------
   Function : TazCatcherStateChangeCallback
   Purpose : Called when state changed 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void TazCatcherStateChangeCallback(ACTORINSTANCE *actorInstance, int state, int lastState)
{
	if(lastState == AISTATE_IDLE)
	{
		DestroyInjuryStars(actorInstance);
	}
}

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceTazCatcher
   Purpose : Setup the basic ai for a zookeeper
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceTazCatcher(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList)
{
	GENERICAI	*genericAI;
	TBVector	tempVector;

	// create extra structures
	CreateActorInstanceBehaviour(actorInstance);
	CreateActorInstanceAnimation(actorInstance);
	CreateActorInstanceStatus(actorInstance);
	//CreateActorInstanceShadow(actorInstance,METERTOUNIT(1.5f),METERTOUNIT(2.5f),SHADOWMAP_RADIUS_TAZCATCHER);
	// TP: CreateActorInstanceCharacterInfo(actorInstance);
	SetActorInertia(actorInstance->actorBehaviour,0);
	SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
	SetActorMaxSpeed(actorInstance->actorBehaviour,METERTOUNIT(9)); // set max speed 8mps
	SETVECTOR(tempVector,0.0f,METERTOUNIT(1.0f),0.0f,1.0f);
	SetActorCylinder(0,actorInstance->actorBehaviour,tempVector,METERTOUNIT(1.0f),METERTOUNIT(2.0f));
	AddActorToCollisionCylinderCache(&collisionCache,actorInstance);
//	AddActorToCollisionSphereCache(&collisionCache,actorInstance);
	
	// create ai structure
	if (CreateActorInstanceGenericAI(actorInstance,activeList,inactiveList)==FALSE) return FALSE;

	AISetShadow(actorInstance,METERTOUNIT(1.5f),METERTOUNIT(2.5f),SHADOWMAP_RADIUS_TAZCATCHER);
	
	// fill in custom parms
	genericAI = actorInstance->genericAI;
	bmVectorCopy(genericAI->center,actorInstance->actorInstance.position);
	genericAI->deactivationRange = METERTOUNIT(12.0f);
	genericAI->awarenessRange = METERTOUNIT(8.0f);
	genericAI->movementRange = METERTOUNIT(4.0f);
	genericAI->standOffRange = METERTOUNIT(2.5f);
	genericAI->personalSpace = METERTOUNIT(2.0f);

	genericAI->viewAngle = PI/3;
	genericAI->awareAngle = PI*0.85f;

	genericAI->alwaysClipPosition = true;
	genericAI->instantAware = true;
	genericAI->instantAttack = true;

	genericAI->headNode = "spinetop";
	SETVECTOR(genericAI->starsOffset,0.0f,METERTOUNIT(0.5f),0.0f,1.0f);
	SETVECTOR(genericAI->starVars,METERTOUNIT(0.3f),METERTOUNIT(0.4f),METERTOUNIT(0.0f),METERTOUNIT(0.0f));

	SetAISeqFunctions(actorInstance,TazCatcherIdleSeq,TazCatcherAwareSeq,TazCatcherAttackSeq,
		TazCatcherTooCloseSeq,TazCatcherAttackedSeq,TazCatcherOutOfRangeSeq);

	SetAIAttackCallback(actorInstance,TazCatcherAttackCallback);
	SetAIChangeStateCallback(actorInstance, TazCatcherStateChangeCallback);
	SetAIMessageHandler(actorInstance, TazCatcherMessageHandler);

	genericAI->customData = (TAZCATCHERINFO*)ZALLOC(sizeof(TAZCATCHERINFO));
	ASSERT(genericAI->customData);

	return TRUE;
}
