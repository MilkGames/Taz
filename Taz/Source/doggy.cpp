// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : doggy.cpp
//   Purpose : ai functions for dog
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
#include "control.h"
#include "clouds.h"
#include "sfx.h"
#include "doggy.h"

// PP: TEMP?: radius of Doggy for shadow map
// PP: TODO? (how?): get bounds of actor instances on the fly so's to optimise shadow map resolution
#define SHADOWMAP_RADIUS_DOGGY		METERTOUNIT(3.0f)

static SEQ idle[] = 
{
	{SEQ_RETURN,"walk3",0},
	{SEQ_PLAY,"idle10",0},
	{SEQ_PLAY,"idle2",0},
	{SEQ_PLAY,"idle3",0},
	{SEQ_PLAY,"idle4",5.0f},
	{SEQ_PLAY,"idle5",0},
	{SEQ_PLAY,"idle4",5.0f},
	{SEQ_PLAY,"idle6",0},
	{SEQ_PLAY,"idle15",0},
	{SEQ_PLAY,"idle16",0},
	{SEQ_PLAY,"idle7",0},
	// TP: {SEQ_CHANGESTATEONATTACKEDONLY,NULL,true},
	{SEQ_FINDNEWCENTER,NULL,0},
	{SEQ_RETURNNEWCENTER,"walk5",0},
	// TP: {SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},
	{SEQ_RETURN,"walk5",0},

	{SEQ_END,NULL,0},
};


static SEQ aware[] =
{
	{SEQ_SETROTSPEED,NULL,RAD(360.0f)},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAYSAMPLE, "dogpoint.wav", false},
	{SEQ_PLAY,"idle12",0},
	{SEQ_PLAY,"pointidle",1.0f},
	{SEQ_SETROTSPEED,NULL,RAD(180.0f)},
	{SEQ_PLAYSAMPLE, "dogbark3.wav", false},
	{SEQ_PLAY,"attack2",0},
	{SEQ_PLAY,"idle8",0},
	{SEQ_PLAYSAMPLE, "dogsniffs.wav", false},
	// TP: {SEQ_CHANGESTATEONATTACKEDONLY,NULL,true},
	{SEQ_FINDNEWCENTER,NULL,0},
	{SEQ_RETURNNEWCENTER,"walk1",0},
	// TP: {SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},
	{SEQ_RETURN,"walk1",0},
	{SEQ_SETROTSPEED,NULL,RAD(200.0f)},
	{SEQ_ROTATELEFT,"round",2.0f},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAYSAMPLE, "dogbark3.wav", false},
	{SEQ_PLAY,"attack2",0},
	
	{SEQ_END,NULL,0},
};

static SEQ attack[] = 
{
	{SEQ_SETROTSPEED,NULL,RAD(180.0f)},
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,true},
	{SEQ_CHASE,"walk4",5},
	{SEQ_PLAYSAMPLE, "dogbark2.wav", false},
	{SEQ_ATTACK,"attack1",0},
	{SEQ_PLAY,"idle15",0},
	{SEQ_END,NULL,0},
	{SEQ_PLAY,"idle15",0},
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},
	// TP: {SEQ_RETURN,"walk2",0},
	{SEQ_END,NULL,0},
};

static SEQ defend[] =
{
	{SEQ_END,NULL,0},
};

static SEQ attacked[] =
{
	{SEQ_PLAYSAMPLE, "dogyelping.wav", false},
	{SEQ_PLAY,"react1",0},
	{SEQ_END,NULL,0},
};

static SEQ outOfRange[] =
{
	// TP: throw snowball
	{SEQ_PLAY,"idle4",0},
	{SEQ_PLAYSAMPLE, "dogsniffs.wav", false},
	{SEQ_RETURN,"walk1",0},
	{SEQ_END,NULL,0},
};

static SEQ biteFront[] =
{
	{SEQ_PLAYSAMPLE, "dogbark3.wav", false},
	{SEQ_PLAY,"attack2",0},
	{SEQ_END,NULL,0},
};

static SEQ biteRear[] =
{
	{SEQ_PLAYSAMPLE, "dogyelping.wav", false},
	{SEQ_PLAY,"react1",0},
	{SEQ_PLAY,"idle14",0},
	{SEQ_END,NULL,0},
};

static SEQ spinFront[] =
{
	{SEQ_PLAYSAMPLE, "dogbark3.wav", false},
	{SEQ_PLAY,"attack2",0},
	{SEQ_END,NULL,0},
};

static SEQ howl[] =
{
	{SEQ_PLAYSAMPLE, "doghowl.wav", false},
	{SEQ_STUNNED,"react4",4.0f},
	{SEQ_END,NULL,0},
};

static SEQ spinRear[] =
{
	{SEQ_PLAYSAMPLE, "dogattack1.wav", false},
	{SEQ_PLAY,"react2",0},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_DISABLED,NULL,5.0f},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAY,"react3",0},
	{SEQ_END,NULL,0},
};

static SEQ scareFront[] = 
{
	{SEQ_PLAYSAMPLE, "dogbark3.wav", false},
	{SEQ_PLAY,"attack2",0},
	{SEQ_END,NULL,0},
};

static SEQ scareRear[] =
{
	{SEQ_FACECENTER,NULL,0},
	{SEQ_PLAYSAMPLE, "dogyelping.wav", false},
	{SEQ_PLAY,"walk6",1.0f},
	{SEQ_MOTIONBONE,NULL,false},
	{SEQ_PLAYSAMPLE, "dogbark3.wav", false},
	{SEQ_FACETAZ,"attack2",0},
	{SEQ_MOTIONBONE,NULL,true},
	{SEQ_END,NULL,0},
};

static SEQ puzzled[] = 
{
	{SEQ_MOTIONBONE,NULL,0},
	{SEQ_PLAYSAMPLE, "dogsniffs.wav", false},
	{SEQ_FACETAZ,"walk1",0},
	{SEQ_MOTIONBONE,NULL,1},
	{SEQ_STARTQUESTION,NULL,0},
	{SEQ_PLAYANDFACE,"disguise",0},
	{SEQ_ENDQUESTION,NULL,0},
	{SEQ_COSTUMEDONE,NULL,0},
	{SEQ_END,NULL,0},
};

/* --------------------------------------------------------------------------------
   Function : DoggyIdleSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int DoggyIdleSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = idle;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : DoggyAwareSeq
   Purpose : Respond to requests for aware seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int DoggyAwareSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = aware;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : DoggyAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int DoggyAttackSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = attack;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : DoggyDefendSeq
   Purpose : Respond to requests for defend seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int DoggyDefendSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = defend;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : DoggyAttackedSeq
   Purpose : Respond to requests for attacked seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int DoggyAttackedSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	int			random;
	GENERICAI *genericAI = actorInstance->genericAI;
	DOGGYINFO *doggyInfo = (DOGGYINFO *)genericAI->customData;

	switch(actorInstance->genericAI->typeOfAttack)
	{
	case ATTACKTYPE_BITEFRONT:
		if (actorInstance->genericAI->currentSeq[actorInstance->genericAI->seqIdx].type==SEQ_STUNNED)
		{
			actorInstance->genericAI->currentSeq = scareRear;
		}
		else
		{
			actorInstance->genericAI->currentSeq = biteFront;
		}
		break;
	case ATTACKTYPE_BITEREAR:
	case ATTACKTYPE_SPITREAR:
		actorInstance->genericAI->currentSeq = biteRear;
		break;
	case ATTACKTYPE_COSTUMEFRONT:
	case ATTACKTYPE_COSTUMEREAR:
		if (doggyInfo->type==DOGGY)
		{
			actorInstance->genericAI->currentSeq = howl;
		}
		else
		{
			actorInstance->genericAI->currentSeq = scareRear;
		}
		break;
	case ATTACKTYPE_SPINFRONT:
		if (actorInstance->genericAI->currentSeq[actorInstance->genericAI->seqIdx].type==SEQ_STUNNED)
		{
			actorInstance->genericAI->currentSeq = spinRear;
		}
		else
		{
			actorInstance->genericAI->currentSeq = spinFront;
		}
		break;
	case ATTACKTYPE_SPINREAR:
	case ATTACKTYPE_SPITFRONT:
		actorInstance->genericAI->currentSeq = spinRear;
		break;
	case ATTACKTYPE_SCAREFRONT:
		if (actorInstance->genericAI->currentSeq[actorInstance->genericAI->seqIdx].type==SEQ_STUNNED)
		{
			actorInstance->genericAI->currentSeq = scareRear;
		}
		else
		{
			actorInstance->genericAI->currentSeq = scareFront;
		}
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
   Function : DoggyOutOfRangeSeq
   Purpose : Respond to requests for out of range seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int DoggyOutOfRangeSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	GENERICAI *genericAI = actorInstance->genericAI;

	actorInstance->genericAI->currentSeq = outOfRange;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/*  --------------------------------------------------------------------------------
	Function	: DoggyPuzzledSeq
	Purpose		: Respond to requests for costume seq
	Parameters	: actorInstance
	Returns		: seq to follow
	Info		: 
*/
int DoggyPuzzledSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = puzzled;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/*  --------------------------------------------------------------------------------
	Function	: DoggyBiteChangeCallback
	Purpose		: standard callback thingy
	Parameters	: 
	Returns		: 
	Info		: 
*/
void DoggyBiteChangeCallback(ACTORINSTANCE *actorInstance)
{
	float		yangle;
	TBVector	tempVector;

	FlushAnimationQueue(actorInstance);

	PlayAnimationByName(actorInstance, "waterbounce", 1.0f, TRUE, 0, 0.0f, NONE);

	if (characterSounds.taz == -1)
		CallbackWhenSampleFinished(characterSounds.taz = PlaySample("tazburnt.wav", 255), CharacterSoundsFinishedCallback, &characterSounds.taz);

	actorInstance->actorAnimation->useMotionBone = false;

	yangle = GetAngleAroundYFromQuat(gameStatus.player1->actorInstance.orientation);

	bmVectorSet(tempVector,0.0f,0.7f,1.0f,1.0f);

	bmVectorRotateY(tempVector,tempVector,Aabs(-yangle+PI));

	bmVectorCMul(gameStatus.player1->actorBehaviour->physics.velocity, tempVector, METERTOUNIT(7.0f));
}

/*  --------------------------------------------------------------------------------
	Function	: DoggyBite
	Purpose		: Called when the doggy does a bite move
	Parameters	: actorInstance of doggy, type of hit
	Returns		: 
	Info		: 
*/
void DoggyBite(ACTORINSTANCE *actorInstance, int type)
{
	char	*bone;

	switch(type)
	{
	case 1:
		bone = "tongue";
		break;
	}

	if (DistFromNodeToTaz(actorInstance,bone) < METERTOUNIT(1.25f))
	{
		// TP: UseStateChange callback
		if (EnterState(gameStatus.player1, STATE_PROJECTILESLIDE))
		{
			FacePoint(gameStatus.player1, actorInstance->actorInstance.position, 0.0f, RAD(180.0f));
			DoggyBiteChangeCallback(gameStatus.player1);

			RequestRumbleEffect(gameStatus.player[0].controller, 180, 0.0f, 0.5f, 0.75f, 0.0f, WAVEFORM_NORMAL);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : DoggyAttackCallback
   Purpose : Called each frame during an attack 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
int DoggyAttackCallback(struct TAG_ACTORINSTANCE *actorInstance)
{
	return FALSE;	// continue processing pattern
}

/* --------------------------------------------------------------------------------
   Function : DoggyMessageHandler
   Purpose : Called when a message is sent in the sequence
   Parameters : ACTORINSTANCE *, char *message
   Returns : 
   Info : 
*/
int DoggyMessageHandler(ACTORINSTANCE *actorInstance, char *message)
{
	TBVector	tempVector,dist;
	TBVector	pos,scale;
	ACTORINSTANCE *tempInstance;

	// TP: Doggy
	if (strcmpi(message,"dogbite")==0)
	{
		DoggyBite(actorInstance,1);
		return true;
	}

	if (strcmpi(message,"rand")==0)
	{
		if ((bmRand()%3)==1) return true;
		else return false;
	}

	return false;
}

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceDoggy
   Purpose : Setup the basic ai for a doggy
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceDoggy(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList,int type)
{
	GENERICAI	*genericAI;
	TBVector	tempVector;

	// create extra structures
	CreateActorInstanceBehaviour(actorInstance);
	CreateActorInstanceAnimation(actorInstance);
	CreateActorInstanceStatus(actorInstance);
	//CreateActorInstanceShadow(actorInstance,METERTOUNIT(1.3f),METERTOUNIT(2.5f), SHADOWMAP_RADIUS_DOGGY);
	// TP: CreateActorInstanceCharacterInfo(actorInstance);
	SetActorInertia(actorInstance->actorBehaviour,0);
	SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
	SetActorMaxSpeed(actorInstance->actorBehaviour,METERTOUNIT(9)); // set max speed 8mps
	SETVECTOR(tempVector,0.0f,METERTOUNIT(1.0f),0.0f,1.0f);
	SetActorCylinder(0,actorInstance->actorBehaviour,tempVector,METERTOUNIT(0.8f),METERTOUNIT(1.0f));
	AddActorToCollisionCylinderCache(&collisionCache,actorInstance);

	// create ai structure
	if (CreateActorInstanceGenericAI(actorInstance,activeList,inactiveList)==FALSE) return FALSE;
	AISetShadow(actorInstance,METERTOUNIT(1.3f),METERTOUNIT(2.5f), SHADOWMAP_RADIUS_DOGGY);

	// fill in custom parms
	genericAI = actorInstance->genericAI;
	bmVectorCopy(genericAI->center,actorInstance->actorInstance.position);

	genericAI->deactivationRange = METERTOUNIT(12.0f);
	genericAI->awarenessRange = METERTOUNIT(7.0f);
	genericAI->movementRange = METERTOUNIT(2.0f);
	genericAI->standOffRange = METERTOUNIT(2.0f);

	genericAI->viewAngle = PI/3;

	genericAI->headNode = "forehead";
	SETVECTOR(genericAI->starsOffset,0.0f,METERTOUNIT(0.5f),0.0f,1.0f);
	SETVECTOR(genericAI->starVars,METERTOUNIT(0.3f),METERTOUNIT(0.4f),METERTOUNIT(0.0f),METERTOUNIT(0.0f));

	SetAISeqFunctions(actorInstance,DoggyIdleSeq,DoggyAwareSeq,DoggyAttackSeq,
		DoggyDefendSeq,DoggyAttackedSeq,DoggyOutOfRangeSeq,DoggyPuzzledSeq);

	SetAIAttackCallback(actorInstance,DoggyAttackCallback);
	SetAIMessageHandler(actorInstance,DoggyMessageHandler);

	genericAI->flags |= GENERICAI_IGNORESPECIALCOLL;

	genericAI->instantAttack = true;
	genericAI->instantAware = true;
	genericAI->costumeInAware = true;
	genericAI->alwaysClipPosition = true;
	genericAI->bounceCRC = bkCRC32((uchar*)"bounce",6,0);

	genericAI->customData = (DOGGYINFO*)ZALLOC(sizeof(DOGGYINFO));
	ASSERT(genericAI->customData);
	
	DOGGYINFO *doggyInfo = (DOGGYINFO*)genericAI->customData;

	doggyInfo->type = type;

	return TRUE;
}

