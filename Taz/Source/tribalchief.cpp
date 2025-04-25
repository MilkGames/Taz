// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : anvilgang.cpp
//   Purpose : Anvil gang AI
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
#include "sfx.h"
#include "debris.h"
#include "projectile.h"
#include "fallingobjects.h"
#include "tribalchief.h"
#include "voodoocloud.h"
#include "destruct.h"
#include "lights.h"
#include "musicandsavagebeasts.h"


#define SHADOWMAP_RADIUS_TRIBALCHIEF			METERTOUNIT(5.5f)

static	char	cycle=0;

static SEQ idle[] = 
{
	{SEQ_RETURN,"walk1",0},
	{SEQ_PLAY,"idle1",0},
	{SEQ_WALK,"walk1",1},
	{SEQ_TURNLEFT,NULL,90},
	{SEQ_WALK,"walk2",1},
	{SEQ_TURNLEFT,NULL,90},
	{SEQ_WALK,"walk3",1},
	{SEQ_TURNLEFT,NULL,90},
	{SEQ_WALK,"walk3",1},
	{SEQ_TURNLEFT,NULL,90},
	{SEQ_END,NULL,0},
};

static SEQ aware[] =
{
	{SEQ_RETURN,"walk1",0},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAY,"dance1",0},
	{SEQ_PLAY,"dance2",0},
	{SEQ_PLAY,"dance3",0},
	{SEQ_PLAY,"dance4",0},
	{SEQ_PLAY,"dance5",0},
	{SEQ_MESSAGE,"voodooeffect",0},
	{SEQ_PLAYANDFACE,"spell7",0},
	{SEQ_MESSAGE,"voodoo",0},
	{SEQ_PLAY,"spell8",0},
	{SEQ_PLAY,"idle3",0},
	{SEQ_END,NULL,0},
};

static SEQ aware2[] =
{
	{SEQ_RETURN,"walk1",0},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAY,"dance7",0},
	{SEQ_PLAY,"dance8",0},
	{SEQ_PLAY,"dance9",0},
	{SEQ_MESSAGE,"voodooeffect",0},
	{SEQ_PLAYANDFACE,"spell7",0},
	{SEQ_MESSAGE,"voodoo",0},
	{SEQ_PLAY,"spell8",0},
	{SEQ_END,NULL,0},
};

static SEQ voodoocloud[] =
{
	// TP: {SEQ_RETURN,"walk3",0},
	{SEQ_PLAYANDFACE,"spell1",0},
	{SEQ_PLAYANDFACE,"spell2",0},
	{SEQ_PLAYANDFACE,"spell3",0},
	{SEQ_PLAYANDFACE,"spell4",0},
	{SEQ_PLAYANDFACE,"spell5",0},
	{SEQ_PLAYANDFACE,"spell6",0},
	{SEQ_MESSAGE,"voodooeffect",0},
	{SEQ_PLAYANDFACE,"spell7",0},
	{SEQ_MESSAGE,"voodoo",0},
	{SEQ_PLAY,"spell8",0},
	{SEQ_END,NULL,0},
};

static SEQ attack[] = 
{
	{SEQ_CHASE,"walk4",4},
	{SEQ_ATTACK,"attack6",0},
	{SEQ_PLAY,"dance2",1.0f},
	{SEQ_END,NULL,0},

	{SEQ_PLAY,"dance2",1.0f},
	{SEQ_END,NULL,0},
};

static SEQ spinFront[] =
{
	{SEQ_PLAY, "react1", 0},
	{SEQ_END,NULL,0},

	{SEQ_PLAYSAMPLE, "zoof.wav",FALSE},
	{SEQ_PLAY,"react2",0.0f},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_DISABLED,NULL,10.0f},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAY,"idle2",0.0f},
	{SEQ_PLAY,"idle3",0.0f},
	{SEQ_END,NULL,0},
};

static SEQ spinRear[] =
{
	{SEQ_PLAYSAMPLE, "zoof.wav",FALSE},
	{SEQ_PLAY,"react2",0.0f},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_DISABLED,NULL,10.0f},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAY,"idle2",0.0f},
	{SEQ_PLAY,"idle3",0.0f},
	{SEQ_END,NULL,0},
};

static SEQ biteFront[] =
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_ATTACK,"attack5",0},
	{SEQ_END,NULL,0},
};

static SEQ biteRear[] =
{
	{SEQ_PLAYSAMPLE, "tribal_2.wav", FALSE},
	{SEQ_PLAY,"react3",0},
	{SEQ_PLAY,"walk5",1.0f},
	{SEQ_END,NULL,0},
};

static SEQ scareFront[] =
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_ATTACK,"attack5",0},
	{SEQ_END,NULL,0},
};

static SEQ scareRear[] = 
{
	{SEQ_PLAYSAMPLE, "tribal_3.wav", FALSE},
	{SEQ_PLAY,"react4",0},
	{SEQ_ROTATELEFT,"walk4",2.5f},
	{SEQ_END,NULL,0},
};

static SEQ tooClose[] = 
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_ATTACK, "attack6", 0},
	{SEQ_END,NULL,0},
};

static SEQ costumeAttack[] = 
{
	{SEQ_PLAYSAMPLE, "zoof.wav",FALSE},
	{SEQ_PLAY,"react5",0.0f},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_DISABLED,NULL,10.0f},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAY,"react6",0.0f},
	{SEQ_PLAY,"idle3",0.0f},
	{SEQ_END,NULL,0},
};

static SEQ disguise[] =
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_STARTQUESTION,NULL,0},
	{SEQ_PLAY,"disguise",0},
	{SEQ_ENDQUESTION,NULL,0},
	{SEQ_COSTUMEDONE,NULL,true},
	{SEQ_END,NULL,0},
};

static SEQ outOfRange[] =
{
	{SEQ_RETURN,"walk1",0},
	{SEQ_END,NULL,0},
};

/* --------------------------------------------------------------------------------
   Function : TribalChiefIdleSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int TribalChiefIdleSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = idle;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : TribalChiefAwareSeq
   Purpose : Respond to requests for aware seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int TribalChiefAwareSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	switch(cycle)
	{
	case 0:
		actorInstance->genericAI->currentSeq = aware;
		break;
	case 1:
		actorInstance->genericAI->currentSeq = aware2;
		break;
	case 2:
		actorInstance->genericAI->currentSeq = voodoocloud;
		break;
	}

	if ((gameStatus.player1->actorStatus->multiplayer.effect == PLAYEREFFECTSLOMO)||(gameStatus.player1->characterInfo->voodooCloudInfo))
	{
		actorInstance->genericAI->currentSeq = aware;
	}
	else
	{
		if ((++cycle)>2) cycle=0;
	}
			
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

static void PerFrameCallback(ACTORINSTANCE *actorInstance)
{
	TRIBALCHIEFINFO *info = (TRIBALCHIEFINFO*)actorInstance->genericAI->customData;
	info->leftHandSpellInfo = UpdateVoodooSpellEffect(info->leftHandSpellInfo);
	info->rightHandSpellInfo = UpdateVoodooSpellEffect(info->rightHandSpellInfo);
}


/* --------------------------------------------------------------------------------
   Function : TribalChiefAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int TribalChiefAttackSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	// TP: PlaySample("tribal_4.wav", 255, actorInstance->actorInstance.position);

	// TP: Only play this sound if tribal guy is not already playing sound
	if (characterSounds.tribal==-1)
	{
		CallbackWhenSampleFinished((characterSounds.tribal=PlaySample("tribal_4.wav",255,actorInstance->actorInstance.position)),CharacterSoundsFinishedCallback,&characterSounds.tribal);	
	}

	actorInstance->genericAI->currentSeq = attack;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : TribalChiefAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int TribalChiefPursuitAttackSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = attack;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : TribalChiefTooCloseSeq
   Purpose : Respond to requests for defend seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int TribalChiefTooCloseSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = tooClose;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : TribalChiefDisguiseSeq
   Purpose : Respond to requests for disguise seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int TribalChiefDisguiseSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = disguise;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : TribalChiefAttackedSeq
   Purpose : Respond to requests for attacked seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int TribalChiefAttackedSeq(struct TAG_ACTORINSTANCE *actorInstance)
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
   Function : TribalChiefOutOfRangeSeq
   Purpose : Respond to requests for out of range seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int TribalChiefOutOfRangeSeq(struct TAG_ACTORINSTANCE *actorInstance)
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
	if((bmRand()%4) == 0)
	{
		PlayAnimationByName(actorInstance, "fly2", 1.0f, TRUE, 0, 0.1f, NONE);
	}
	else
	{
		PlayAnimationByName(actorInstance, "fly1", 1.0f, TRUE, 0, 0.1f, NONE);
	}
	actorInstance->actorAnimation->useMotionBone = FALSE;

	yangle = GetAngleAroundYFromQuat(gameStatus.player1->actorInstance.orientation);

	bmVectorSet(tempVector,0.0f,0.7f,1.0f,1.0f);

	bmVectorRotateY(tempVector,tempVector,Aabs(-yangle+PI));

	bmVectorCMul(gameStatus.player1->actorBehaviour->physics.velocity, tempVector, METERTOUNIT(8.0f));
}


/*  --------------------------------------------------------------------------------
	Function	: TribalChiefAttack
	Purpose		: tribal guy does attack move
	Parameters	: actorInstance for tribal guy, type of attack
	Returns		: 
	Info		: 
*/

void TribalChiefAttack(ACTORINSTANCE *actorInstance, int type)
{
	char	*bone;

	switch(type)
	{
	case 0:
		bone = "lowestorso";
		break;
	case 1:
		bone = "rhand";
		break;
	case 2:
		bone = "hat";
		break;
	}

	if ((type==0)||(type==1))
	{
		if (DistFromNodeToTaz(actorInstance,bone) < METERTOUNIT(1.25f))
		{
			if (EnterState(gameStatus.player1, STATE_PROJECTILESLIDE))
			{
				FacePoint(gameStatus.player1, actorInstance->actorInstance.position, 0.0f, RAD(180.0f));
				StopCollidingWith(gameStatus.player1,actorInstance,0.2f);
				myStateChangeCallback(gameStatus.player1);
			}
		}
	}
	else
	{
		if (DistFromNodeToTaz(actorInstance,bone) < METERTOUNIT(1.25f))
		{
			if (EnterState(gameStatus.player1,STATE_PLAYANIMATION))
			{
				FlushAnimationQueue(gameStatus.player1);
				PlayAnimationByName(gameStatus.player1,"react2",1.0f,0,0,0.1f,NONE);

				// TP: Turn taz to face character
				FacePoint(gameStatus.player1,actorInstance->actorInstance.position,0.0f,0.0f);
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : TribalChiefMessageHandler
   Purpose : Called when a message is sent in the sequence
   Parameters : ACTORINSTANCE *, char *message
   Returns : 
   Info : 
*/
int TribalChiefMessageHandler(ACTORINSTANCE *actorInstance, char *message)
{
	TRIBALCHIEFINFO *info = (TRIBALCHIEFINFO*)actorInstance->genericAI->customData;

	if (strcmpi(message, "startdance")==0)
	{
		info->dancing = true;
	}

	if (strcmpi(message,"tcbelly")==0)
	{
		TribalChiefAttack(actorInstance,0);
		return true;
	}

	if (strcmpi(message,"tcpush")==0)
	{
		TribalChiefAttack(actorInstance,1);
		return true;
	}

	if (strcmpi(message,"voodoo")==0)
	{
		CreateVoodooCloud(gameStatus.player1);
	}

	if (strcmpi(message,"mask")==0)
	{
		TribalChiefAttack(actorInstance,2);
	}

	if (strcmpi(message,"voodooeffect")==0)
	{
		if (!info->leftHandSpellInfo)
		{
			info->leftHandSpellInfo = CreateVoodooSpellEffect(actorInstance,"lwrist",1.5f);
		}
		if (!info->rightHandSpellInfo)
		{
			info->rightHandSpellInfo = CreateVoodooSpellEffect(actorInstance,"rwrist",1.5f);
		}
	}

	return false;
}

/*  --------------------------------------------------------------------------------
	Function	: Draw2
	Purpose		: Called to draw any alpha effects
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/

void Draw2(ACTORINSTANCE *actorInstance)
{
	TRIBALCHIEFINFO *info = (TRIBALCHIEFINFO*)actorInstance->genericAI->customData;

	DrawVoodooSpellEffect(info->leftHandSpellInfo);
	DrawVoodooSpellEffect(info->rightHandSpellInfo);
}

/*  --------------------------------------------------------------------------------
	Function	: Free
	Purpose		: Custom delete code
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/

static void Free(ACTORINSTANCE *actorInstance)
{
	TRIBALCHIEFINFO *info = (TRIBALCHIEFINFO*)actorInstance->genericAI->customData;

	if (!info->leftHandSpellInfo)
	{
		SAFE_DELETE(info->leftHandSpellInfo);
	}
	if (!info->rightHandSpellInfo)
	{
		SAFE_DELETE(info->rightHandSpellInfo);
	}
}


/*  --------------------------------------------------------------------------------
	Function	: TribalFrameCallback
	Purpose		: Called once per frame to keep track of dancing
	Parameters	: actorInstance for enemy
	Returns		: 
	Info		: 
*/

static void TribalFrameCallback(ACTORINSTANCE *actorInstance)
{
/*	TRIBALCHIEFINFO *info = (TRIBALCHIEFINFO*)actorInstance->genericAI->customData;
	
	if (info->dancing)
	{
		if (actorInstance->genericAI->state == AISTATE_AWARE)
		{
			switch(gameStatus.player1->actorStatus->currentState)
			{
			case STATE_MOVE:
			case STATE_SPIN:
			case STATE_SKIDSTOP:
			case STATE_TIPTOE:
			case STATE_CHILLIPEPPER:
				if (EnterState(gameStatus.player1, STATE_MESMERISEDBYTRIBAL))
				{
					MusicBoxStateChangeCallback(gameStatus.player1);
				}
			}
		}
		else
		{
			info->dancing = false;
			if(gameStatus.player1->actorStatus->currentState == STATE_MESMERISEDBYTRIBAL)
			{
				EnterState(gameStatus.player1, STATE_MOVE);
			}
		}
	}*/
}


/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceTribalChief
   Purpose : Setup the basic ai for a TribalChief
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceTribalChief(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList)
{
	GENERICAI	*genericAI;
	TBVector	tempVector;

	// create extra structures
	CreateActorInstanceBehaviour(actorInstance);
	CreateActorInstanceAnimation(actorInstance);
	CreateActorInstanceStatus(actorInstance);
	// TP: CreateActorInstanceCharacterInfo(actorInstance);
	SetActorInertia(actorInstance->actorBehaviour,0);
	SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
	SetActorMaxSpeed(actorInstance->actorBehaviour,METERTOUNIT(9)); // set max speed 8mps
	SETVECTOR(tempVector,0.0f,METERTOUNIT(1.0f),0.0f,1.0f);
	SetActorCylinder(0,actorInstance->actorBehaviour,tempVector,METERTOUNIT(0.7f),METERTOUNIT(2.0f));
	AddActorToCollisionCylinderCache(&collisionCache,actorInstance);
	
	// create ai structure
	if (CreateActorInstanceGenericAI(actorInstance,activeList,inactiveList)==FALSE) return FALSE;

	AISetShadow(actorInstance,METERTOUNIT(0.7f),METERTOUNIT(2.5f), SHADOWMAP_RADIUS_TRIBALCHIEF);

	// fill in custom parms
	genericAI = actorInstance->genericAI;
	bmVectorCopy(genericAI->center,actorInstance->actorInstance.position);

	genericAI->deactivationRange = METERTOUNIT(15.0f);
	genericAI->awarenessRange = METERTOUNIT(6.0f);
	genericAI->movementRange = METERTOUNIT(3.0f);
	genericAI->viewAngle = PI;
	genericAI->personalSpace = METERTOUNIT(1.2f);
	genericAI->standOffRange = METERTOUNIT(1.6f);
	genericAI->attackHoldDistance = METERTOUNIT(2.0f);
	genericAI->hearRange = METERTOUNIT(0.0f);

	genericAI->instantAttack = true;
	genericAI->instantAware = true;
	genericAI->alwaysClipPosition = true;

	genericAI->currentRotTime = RAD(90.0f)/60.0f;

	genericAI->headNode = "neck";
	SETVECTOR(genericAI->starsOffset,0.0f,METERTOUNIT(0.5f),0.0f,1.0f);
	SETVECTOR(genericAI->starVars,METERTOUNIT(0.3f),METERTOUNIT(0.4f),METERTOUNIT(0.0f),METERTOUNIT(0.0f));

	SetAISeqFunctions(actorInstance,TribalChiefIdleSeq,TribalChiefAwareSeq,TribalChiefAttackSeq,
		TribalChiefTooCloseSeq,TribalChiefAttackedSeq,TribalChiefOutOfRangeSeq,TribalChiefDisguiseSeq,TribalChiefPursuitAttackSeq);

	genericAI->bounceCRC = bkCRC32((uchar*)"bounce",6,0);


	genericAI->frameCallback = PerFrameCallback;
	genericAI->draw2 = Draw2;
	genericAI->freeCallback = Free;

	genericAI->customData = (void*)ZALLOC(sizeof(TRIBALCHIEFINFO));
	TRIBALCHIEFINFO *info = (TRIBALCHIEFINFO*)genericAI->customData;

	info->leftHandSpellInfo = NULL;
	info->rightHandSpellInfo = NULL;
	info->dancing = false;

	SetAIMessageHandler(actorInstance,TribalChiefMessageHandler);

	genericAI->runCRC = bkCRC32((uchar*)"run",3,0);

	return TRUE;
}
