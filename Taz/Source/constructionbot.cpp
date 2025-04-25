// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : constructionbot.cpp
//   Purpose : ai functions for construction bot
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
#include "snowball.h"
#include "bears.h"
#include "constructionbot.h"

// PP: TEMP?: radius of Construction Bot for shadow map
// PP: TODO? (how?): get bounds of actor instances on the fly so's to optimise shadow map resolution
#define SHADOWMAP_RADIUS_CONSTRUCTIONBOT		METERTOUNIT(3.0f)

static SEQ idle1[] = 
{
	{SEQ_SETROTSPEED,NULL,RAD(90.0f)},
	{SEQ_PLAYSAMPLE, "loop\\cbotroll.wav", TRUE},
	{SEQ_RETURN,"walk1",0},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_PLAYSAMPLE, "cbspin.wav", FALSE},
	{SEQ_PLAY,"turn1",0},
	{SEQ_TURNLEFT,NULL,90.0f},
	{SEQ_PLAYSAMPLE, "loop\\cbotroll.wav", TRUE},
	{SEQ_RETURN,"walk1",0},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_PLAYSAMPLE, "cbspin.wav", FALSE},
	{SEQ_SETROTSPEED,NULL,RAD(180.0f)},
	{SEQ_PLAY,"turn2",0},
	{SEQ_TURNRIGHT,NULL,180.0f},
	{SEQ_PLAYSAMPLE, "loop\\cbotroll.wav", TRUE},
	{SEQ_RETURN,"walk1",0},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_PLAYSAMPLE, "cbspin.wav", FALSE},
	{SEQ_SETROTSPEED,NULL,RAD(90.0f)},
	{SEQ_PLAY,"turn1",0},
	{SEQ_TURNLEFT,NULL,90.0f},
	{SEQ_PLAYSAMPLE, "loop\\cbotroll.wav", TRUE},
	{SEQ_RETURN,"walk1",0},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_PLAYSAMPLE, "cbspin.wav", FALSE},
	{SEQ_SETROTSPEED,NULL,RAD(180.0f)},
	{SEQ_PLAY,"turn2",0},
	{SEQ_TURNRIGHT,NULL,180.0f},
	{SEQ_PLAY,"walk3",1.5f},
	{SEQ_PLAYSAMPLE, "cbspin.wav", FALSE},
	{SEQ_SETROTSPEED,NULL,RAD(90.0f)},
	{SEQ_PLAY,"turn1",0},
	{SEQ_TURNLEFT,NULL,90.0f},
	{SEQ_PLAY,"walk3",1.5f},
	{SEQ_PLAYSAMPLE, "cbspin.wav", FALSE},
	{SEQ_SETROTSPEED,NULL,RAD(180.0f)},
	{SEQ_PLAY,"turn2",0},
	{SEQ_TURNRIGHT,NULL,180.0f},
	{SEQ_PLAY,"walk3",1.5f},
	{SEQ_PLAYSAMPLE, "cbspin.wav", FALSE},
	{SEQ_PLAY,"turn1",0},
	{SEQ_TURNLEFT,NULL,90.0f},
	{SEQ_PLAY,"walk3",1.5f},
	{SEQ_PLAYSAMPLE, "cbspin.wav", FALSE},
	{SEQ_SETROTSPEED,NULL,RAD(180.0f)},
	{SEQ_PLAY,"turn2",0},
	{SEQ_TURNRIGHT,NULL,180.0f},
	{SEQ_END,NULL,0},
};

static SEQ idle2[] = 
{
	{SEQ_RETURN,"walk2",0},
	{SEQ_PLAY,"idle3",2.0f},
	{SEQ_PLAY,"idle4",0},
	{SEQ_STARTZZZ,NULL,1},
	{SEQ_PLAY,"idle5",10.0f},
	{SEQ_PLAY,"idle6",0},
	{SEQ_PLAY,"idle5",10.0f},
	{SEQ_ENDZZZ,NULL,0},
	{SEQ_PLAY,"idle7", 0},
	{SEQ_PLAY,"idle8",0},
	{SEQ_END,NULL,0},
};

static SEQ attack[] = 
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_CHASE,"walk4",0},
	{SEQ_PLAYSAMPLE, "cbslapping.wav", FALSE},
	{SEQ_ATTACKANDFACE,"attack4",0},
	{SEQ_PLAY,"idle14",0},
	{SEQ_END,NULL,0},
};

static SEQ aware[] = 
{
	{SEQ_FACETAZ,NULL,0},

	{SEQ_MESSAGE,"context1",0},
	{SEQ_PLAYANDFACE,"attack1",0},
	{SEQ_PLAYSAMPLE, "cbfirebolt.wav", FALSE},
	{SEQ_MESSAGE,"context2",0},
	{SEQ_PLAYANDFACE,"attack1",0},	
	{SEQ_PLAYSAMPLE, "cbfirebolt.wav", FALSE},
	{SEQ_MESSAGE,"context3",0},
	{SEQ_PLAYANDFACE,"attack1",0},
	{SEQ_PLAYSAMPLE, "cbfirebolt.wav", FALSE},
	{SEQ_PLAY,"idle20",5.0f},

	{SEQ_END,NULL,0},
};

static SEQ defend[] =
{
	{SEQ_LOOP,NULL,0},
};

static SEQ costume[] =
{
	{SEQ_PLAY,"react6",0},
	{SEQ_DISABLED,NULL,10.0f},
	{SEQ_PLAY,"react4",0},
	{SEQ_END,NULL,0},
};

static SEQ spinFront[] = 
{
	{SEQ_PLAYSAMPLE, "cbbump.wav", FALSE},
	{SEQ_PLAY,"react1",0},
	{SEQ_END,NULL,0},
};

static SEQ spinRear[] =
{
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_PLAYSAMPLE, "cbbump.wav", FALSE},
	{SEQ_PLAY,"react1",0},
	{SEQ_PLAYSAMPLE, "cbspun.wav", FALSE},
	{SEQ_PLAY,"react2",0},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAY,"react3",0},
	{SEQ_DISABLED,NULL,10.0f},
	{SEQ_PLAY,"react4",0},
	{SEQ_END,NULL,0},
};

static SEQ spinRearError[] = 
{
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_PLAYSAMPLE, "cbbump.wav", FALSE},
	{SEQ_PLAY,"react1",0},
	{SEQ_PLAYSAMPLE, "cbspun.wav", FALSE},
	{SEQ_PLAY,"react2",0},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAY,"react3",0},
	{SEQ_DISABLED,NULL,10.0f},
	{SEQ_PLAY,"react5",0},
	{SEQ_END,NULL,0},
};

static SEQ biteFront[] =
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAYSAMPLE, "cbhitwitharm.wav", FALSE},
	{SEQ_ATTACK,"attack2",0},
	{SEQ_END,NULL,0},
};

static SEQ biteRear[] =
{
	{SEQ_END,NULL,0},
};

static SEQ scareFront[] =
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_CHASE,"walk4",0},
	{SEQ_PLAYSAMPLE, "cbhitwitharm.wav", FALSE},
	{SEQ_ATTACK,"attack2",0},
	{SEQ_END,NULL,0},
};

static SEQ scareRear[] = 
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAYSAMPLE, "cbhitwitharm.wav", FALSE},
	{SEQ_ATTACK,"attack2",0},
	{SEQ_END,NULL,0},
};

static SEQ tooClose[] = 
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAY,"walk8",1.0f},
	{SEQ_END,NULL,0},
};

static SEQ outOfRange[] =
{
	{SEQ_RETURN,"walk4",0},
	{SEQ_END,NULL,0},
};

static SEQ puzzled[] = 
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_STARTQUESTION,NULL,0},
	{SEQ_PLAY,"disguise",0},
	{SEQ_ENDQUESTION,NULL,0},
	{SEQ_COSTUMEDONE,NULL,0},
	{SEQ_MESSAGE,"context1",0},
	{SEQ_PLAYANDFACE,"attack1",0},
	{SEQ_PLAYSAMPLE, "cbfirebolt.wav", FALSE},
	{SEQ_MESSAGE,"context2",0},
	{SEQ_PLAYANDFACE,"attack1",0},
	{SEQ_PLAYSAMPLE, "cbfirebolt.wav", FALSE},
	{SEQ_MESSAGE,"context3",0},
	{SEQ_PLAYANDFACE,"attack1",0},
	{SEQ_PLAYSAMPLE, "cbfirebolt.wav", FALSE},
	{SEQ_PLAY,"idle20",5.0f},
	{SEQ_END,NULL,0},
};

/* --------------------------------------------------------------------------------
   Function : ConstructionBotIdleSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ConstructionBotIdleSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	int		number;

	number = bmRand()%2;
	switch(number)
	{
	case 0:
		actorInstance->genericAI->currentSeq = idle1;
		break;
	case 1:
		actorInstance->genericAI->currentSeq = idle2;
		break;
	}
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ConstructionBotAwareSeq
   Purpose : Respond to requests for aware seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ConstructionBotAwareSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = aware;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ConstructionBotCostumeSeq
   Purpose : Respond to requests for costume seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ConstructionBotCostumeSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = puzzled;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ConstructionBotAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ConstructionBotAttackSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = attack;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ConstructionBotTooCloseSeq
   Purpose : Respond to requests for defend seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ConstructionBotTooCloseSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = tooClose;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ConstructionBotAttackedSeq
   Purpose : Respond to requests for attacked seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ConstructionBotAttackedSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	int		tempInt, random;

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
	case ATTACKTYPE_SPINFRONT:
		actorInstance->genericAI->currentSeq = spinFront;
		break;
	case ATTACKTYPE_COSTUMEFRONT:
	case ATTACKTYPE_COSTUMEREAR:
		actorInstance->genericAI->currentSeq = costume;
		break;
	case ATTACKTYPE_BITEREAR:
	case ATTACKTYPE_SPINREAR:
	case ATTACKTYPE_SPITFRONT:
	case ATTACKTYPE_SPITREAR:
		if(tempInt = bmRand()%4)
		{
			actorInstance->genericAI->currentSeq = spinRear;
		}
		else
		{
			actorInstance->genericAI->currentSeq = spinRearError;
		}
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
   Function : ConstructionBotOutOfRangeSeq
   Purpose : Respond to requests for out of range seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ConstructionBotOutOfRangeSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = outOfRange;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ConstructionBotAttackCallback
   Purpose : Called each frame during an attack 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
int ConstructionBotAttackCallback(struct TAG_ACTORINSTANCE *actorInstance)
{
	return FALSE;	// continue processing pattern
}

/* --------------------------------------------------------------------------------
   Function : ConstructionBotStateChangeCallback
   Purpose : Called when state changed 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void ConstructionBotStateChangeCallback(ACTORINSTANCE *actorInstance, int state, int lastState)
{
	if(lastState == AISTATE_IDLE)
	{
		DestroyInjuryStars(actorInstance);
	}
}

/* --------------------------------------------------------------------------------
   Function : ConstructionBotTazSquashedCallback
   Purpose : Called when state changed 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int ConstructionBotTazSquashedCallback(ACTORINSTANCE *actorInstance)
{
	TAZCATCHERINFO	*botInfo = (TAZCATCHERINFO*)actorInstance->genericAI->customData;

	if((botInfo->timeSinceLastAttack += fTime) > 2.0f)
	{
		SetAIAttackCallback(actorInstance,ConstructionBotAttackCallback);

		// sort out taz catcher
		FlushAnimationQueue(actorInstance);

		ResetCharacterAfterJump(actorInstance);

		actorInstance->genericAI->flags &= ~GENERICAI_DONTCHANGESTATE;
		return FALSE;
	}
	return TRUE;
}

static int PreRivetCollisionCallback(ACTORINSTANCE *rivet, ACTORINSTANCE *object, PRECOLLINFO *preCollInfo)
{
	return false;
}


/* --------------------------------------------------------------------------------
   Function : ConstructionBotMessageHandler
   Purpose : Called when a message is sent in the sequence
   Parameters : ACTORINSTANCE *, char *message
   Returns : 
   Info : 
*/
int ConstructionBotMessageHandler(ACTORINSTANCE *actorInstance, char *message)
{
	// TP: Construction bot
	if (strcmpi(message,"cbshoot")==0)
	{
		ConstructionBotEvent(actorInstance,1);
		return true;
	}
	if (strcmpi(message,"cwgunhit")==0)
	{
		ConstructionBotEvent(actorInstance,2);
		return true;
	}
	if (strcmpi(message,"cbpaint")==0)
	{
		ConstructionBotEvent(actorInstance,3);
		return true;
	}
	if (strcmpi(message,"cbpaint2")==0)
	{
		ConstructionBotEvent(actorInstance,4);
		return true;
	}

	if (strcmpi(message,"context1")==0)
	{
		actorInstance->genericAI->context = 1;
	}
	if (strcmpi(message,"context2")==0)
	{
		actorInstance->genericAI->context = 2;
	}
	if (strcmpi(message,"context3")==0)
	{
		actorInstance->genericAI->context = 3;
	}

	return false;
}

/* --------------------------------------------------------------------------------
   Function : RivetCollision
   Purpose : Called when snowball collides
   Parameters : ACTORINSTANCE *, 
   Returns : 
   Info : 
*/
int RivetCollision1(ACTORINSTANCE *collider,ACTORINSTANCE *objectHit)
{
	ACTORINSTANCE *rivet,*taz;

	if (collider == gameStatus.player1)
	{
		taz = collider;
		rivet = objectHit;
	}
	else
	{
		if (objectHit == gameStatus.player1)
		{
			taz = objectHit;
			rivet = collider;
		}
		else return FALSE;
	}

	// dont know which is which so try and destroy both
	rivet->fadeSpeed = -64.0f;
	rivet->flags |= ACTORFLAG_FADEAFTERANIM;
	if (rivet->actorBehaviour->physics.velocity[Y] < 0.0f)
	{
		rivet->actorBehaviour->physics.velocity[Y] = -rivet->actorBehaviour->physics.velocity[Y];
	}

	if (EnterState(gameStatus.player1, STATE_PLAYANIMATION))
	{
		FlushAnimationQueue(gameStatus.player1);
		PlayAnimationByName(gameStatus.player1,"idle6",1.5f,0,0,0.1f,NONE);
	}

	SetCollisionCallback(rivet, NULL);

	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : RivetCollision
   Purpose : Called when snowball collides
   Parameters : ACTORINSTANCE *, 
   Returns : 
   Info : 
*/
int RivetCollision2(ACTORINSTANCE *collider,ACTORINSTANCE *objectHit)
{
	ACTORINSTANCE *rivet,*taz;
	TBVector tempVector;

	if (collider == gameStatus.player1)
	{
		taz = collider;
		rivet = objectHit;
	}
	else
	{
		if (objectHit == gameStatus.player1)
		{
			taz = objectHit;
			rivet = collider;
		}
		else return FALSE;
	}

	// dont know which is which so try and destroy both
	rivet->fadeSpeed = -64.0f;
	rivet->flags |= ACTORFLAG_FADEAFTERANIM;
	if (rivet->actorBehaviour->physics.velocity[Y] < 0.0f)
	{
		rivet->actorBehaviour->physics.velocity[Y] = -rivet->actorBehaviour->physics.velocity[Y];
	}

	if (!EnterState(gameStatus.player1, STATE_PLAYANIMATION)) return TRUE;

	FlushAnimationQueue(gameStatus.player1);
	PlayAnimationByName(gameStatus.player1,"react1",1.0f,0,0,0.1f,NONE);
	FacePoint(gameStatus.player1, rivet->actorInstance.position, 0.0f, 0.0f);

	RequestRumbleEffect(gameStatus.player[0].controller, 180, 0.0f, 0.5f, 0.75f, 0.0f, WAVEFORM_NORMAL);

	SetCollisionCallback(rivet, NULL);

	return FALSE;
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

	PlayAnimationByName(actorInstance, "waterbounce", 1.0f, TRUE, 0, 0.1f, NONE);

	actorInstance->actorAnimation->useMotionBone = FALSE;

	yangle = GetAngleAroundYFromQuat(gameStatus.player1->actorInstance.orientation);

	bmVectorSet(tempVector,0.0f,0.7f,1.0f,1.0f);

	bmVectorRotateY(tempVector,tempVector,Aabs(-yangle+PI));

	bmVectorCMul(gameStatus.player1->actorBehaviour->physics.velocity, tempVector, METERTOUNIT(7.0f));
}

/* --------------------------------------------------------------------------------
   Function : RivetCollision
   Purpose : Called when snowball collides
   Parameters : ACTORINSTANCE *, 
   Returns : 
   Info : 
*/
int RivetCollision3(ACTORINSTANCE *collider,ACTORINSTANCE *objectHit)
{
	ACTORINSTANCE *rivet,*taz;
	TBVector tempVector;

	if (collider == gameStatus.player1)
	{
		taz = collider;
		rivet = objectHit;
	}
	else
	{
		if (objectHit == gameStatus.player1)
		{
			taz = objectHit;
			rivet = collider;
		}
		else return FALSE;
	}

	// dont know which is which so try and destroy both
	rivet->fadeSpeed = -64.0f;
	rivet->flags |= ACTORFLAG_FADEAFTERANIM;
	if (rivet->actorBehaviour->physics.velocity[Y] < 0.0f)
	{
		rivet->actorBehaviour->physics.velocity[Y] = -rivet->actorBehaviour->physics.velocity[Y];
	}

	if (EnterState(gameStatus.player1, STATE_PROJECTILESLIDE))
	{
		myStateChangeCallback(gameStatus.player1);
		bmVectorAdd(tempVector, rivet->actorBehaviour->physics.velocity, gameStatus.player1->actorInstance.position);
		FacePoint(gameStatus.player1, tempVector, 0.0f, 0.0f);

		RequestRumbleEffect(gameStatus.player[0].controller, 220, 0.0f, 0.5f, 0.75f, 0.0f, WAVEFORM_NORMAL);
	}

	SetCollisionCallback(rivet, NULL);

	return FALSE;
}

/*  --------------------------------------------------------------------------------
	Function	: ConstructionBotEvent
	Purpose		: Called in response to an anim message
	Parameters	: ACTORINSTANCE, type
	Returns		: 
	Info		: 
*/
void ConstructionBotEvent(ACTORINSTANCE *actorInstance, int type)
{
	TBVector	tempVector;
	TBVector	pos;
	ACTORINSTANCE *tempInstance;
	TBActorNodeInstance	*node;
	DEBRIS		*debris;

	switch(type)
	{
	case 1:
		bmVectorCopy(pos,gameStatus.player1->actorInstance.position);
		pos[Y] += METERTOUNIT(0.5f);
		node = baFindNode(actorInstance->actorInstance.rootNodeInstance,"bullet");
		if(node)
		{
			baGetNodesWorldPosition(&actorInstance->actorInstance, node, tempVector);
			debris = CreateSnowball(actorInstance, tempVector, tempVector, METERTOUNIT(0.4f),pos,0,"rivet.obe",RAD(35.0f), 1.2f);
			tempInstance = debris->actorStructure;
		}
		if (tempInstance) 
		{
			switch(actorInstance->genericAI->context)
			{
			case 1:
				SetCollisionCallback(tempInstance,RivetCollision1);
				break;
			case 2:
				SetCollisionCallback(tempInstance,RivetCollision2);
				break;
			case 3:
				SetCollisionCallback(tempInstance,RivetCollision3);
				break;
			}

			SetPreCollisionCallback(tempInstance,PreRivetCollisionCallback);
		}
		break;
	case 2:
		if (DistFromNodeToTaz(actorInstance,"bullet") < METERTOUNIT(1.2f))
		{
			RequestStateChange(gameStatus.player1,STATE_PLAYANIMATION);
			FlushAnimationQueue(gameStatus.player1);
			PlayAnimationByName(gameStatus.player1,"react2",1.0f,0,0,0.1f,NONE);

			// TP: Turn taz to face character
			FacePoint(gameStatus.player1,actorInstance->actorInstance.position,0.0f,0.0f);

			RequestRumbleEffect(gameStatus.player[0].controller, 180, 0.0f, 0.5f, 0.75f, 0.0f, WAVEFORM_NORMAL);
		}
		break;
	case 3:
		if (DistFromNodeToTaz(actorInstance,"m") < METERTOUNIT(1.2f))
		{
			RequestStateChange(gameStatus.player1,STATE_PLAYANIMATION);
			FlushAnimationQueue(gameStatus.player1);
			PlayAnimationByName(gameStatus.player1,"react1",1.0f,0,0,0.1f,NONE);

			// TP: Turn taz to face character
			FacePoint(gameStatus.player1,actorInstance->actorInstance.position,0.0f,0.0f);

			RequestRumbleEffect(gameStatus.player[0].controller, 180, 0.0f, 0.5f, 0.75f, 0.0f, WAVEFORM_NORMAL);
		}
		break;
	case 4:
		if (DistFromNodeToTaz(actorInstance,"m") < METERTOUNIT(1.2f))
		{
			RequestStateChange(gameStatus.player1,STATE_PLAYANIMATION);
			FlushAnimationQueue(gameStatus.player1);
			PlayAnimationByName(gameStatus.player1,"react2",1.0f,0,0,0.1f,NONE);

			// TP: Turn taz to face character
			FacePoint(gameStatus.player1,actorInstance->actorInstance.position,0.0f,0.0f);

			RequestRumbleEffect(gameStatus.player[0].controller, 180, 0.0f, 0.5f, 0.75f, 0.0f, WAVEFORM_NORMAL);
		}
		break;
	}
}

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceConstructionBot
   Purpose : Setup the basic ai for a zookeeper
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceConstructionBot(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList)
{
	GENERICAI	*genericAI;
	TBVector	tempVector;

	// create extra structures
	CreateActorInstanceBehaviour(actorInstance);
	CreateActorInstanceAnimation(actorInstance);
	CreateActorInstanceStatus(actorInstance);
	//CreateActorInstanceShadow(actorInstance,METERTOUNIT(1.7f),METERTOUNIT(2.5f), SHADOWMAP_RADIUS_CONSTRUCTIONBOT);
	// TP: CreateActorInstanceCharacterInfo(actorInstance);
	SetActorInertia(actorInstance->actorBehaviour,0);
	SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
	SetActorMaxSpeed(actorInstance->actorBehaviour,METERTOUNIT(15)); // set max speed 8mps
	SETVECTOR(tempVector,0.0f,METERTOUNIT(1.0f),0.0f,1.0f);
	SetActorCylinder(0,actorInstance->actorBehaviour,tempVector,METERTOUNIT(0.5f),METERTOUNIT(2.0f));
	AddActorToCollisionCylinderCache(&collisionCache,actorInstance);
//	AddActorToCollisionSphereCache(&collisionCache,actorInstance);
	
	// create ai structure
	if (CreateActorInstanceGenericAI(actorInstance,activeList,inactiveList)==FALSE) return FALSE;

	AISetShadow(actorInstance,METERTOUNIT(1.7f),METERTOUNIT(2.5f), SHADOWMAP_RADIUS_CONSTRUCTIONBOT);
	
	// fill in custom parms
	genericAI = actorInstance->genericAI;
	bmVectorCopy(genericAI->center,actorInstance->actorInstance.position);
	genericAI->deactivationRange = METERTOUNIT(12.0f);
	genericAI->awarenessRange = METERTOUNIT(8.0f);
	genericAI->movementRange = METERTOUNIT(2.0f);
	genericAI->standOffRange = METERTOUNIT(1.3f);
	genericAI->personalSpace = METERTOUNIT(1.0f);
	genericAI->viewAngle = PI/2;
	genericAI->awareAngle = PI/2;

	genericAI->costumeInAware = true;

	genericAI->alwaysClipPosition = true;
	genericAI->instantAttack = true;
	genericAI->instantAware = true;

	genericAI->headNode = "hat";
	SETVECTOR(genericAI->starsOffset,0.0f,METERTOUNIT(0.2f),0.0f,1.0f);
	SETVECTOR(genericAI->starVars,METERTOUNIT(0.3f),METERTOUNIT(0.4f),METERTOUNIT(0.0f),METERTOUNIT(0.0f));

	genericAI->currentRotTime = RAD(180.0f)/60.0f;

	// TP: create temp box
/*
	genericAI->useMoveBox = true;
	bmMatCopy(genericAI->moveBox.matrix,actorInstance->actorInstance.objectToWorld);
	bmMatInverse(genericAI->moveBox.invmatrix,actorInstance->actorInstance.objectToWorld);
	genericAI->moveBox.bbox.xmax = METERTOUNIT(2.0f);
	genericAI->moveBox.bbox.xmin = METERTOUNIT(-2.0f);
	genericAI->moveBox.bbox.ymax = METERTOUNIT(2.0f);
	genericAI->moveBox.bbox.ymin = METERTOUNIT(-2.0f);
	genericAI->moveBox.bbox.zmax = METERTOUNIT(5.0f);
	genericAI->moveBox.bbox.zmin = METERTOUNIT(-5.0f);

	memcpy(&genericAI->moveBox.attackBBox, &genericAI->moveBox.bbox, sizeof(BOUNDINGBOX));
	float tempFloat = 0.7f * actorInstance->genericAI->deactivationRange;
	memcpy(&actorInstance->genericAI->moveBox.idleBox, &actorInstance->genericAI->moveBox.bbox, sizeof(BOUNDINGBOX));
	EnlargeBBox(&actorInstance->genericAI->moveBox.idleBox, actorInstance->genericAI->moveBox.matrix, actorInstance->genericAI->moveBox.invmatrix, tempFloat);
	bmVectorCopy(actorInstance->genericAI->orgCenter, actorInstance->genericAI->center);
*/
	// TP: ---------

	SetAISeqFunctions(actorInstance,ConstructionBotIdleSeq,ConstructionBotAwareSeq,ConstructionBotAttackSeq,
		ConstructionBotTooCloseSeq,ConstructionBotAttackedSeq,ConstructionBotOutOfRangeSeq,ConstructionBotCostumeSeq);

	SetAIAttackCallback(actorInstance,ConstructionBotAttackCallback);
	SetAIChangeStateCallback(actorInstance, ConstructionBotStateChangeCallback);
	SetAIMessageHandler(actorInstance,ConstructionBotMessageHandler);

	return TRUE;
}
