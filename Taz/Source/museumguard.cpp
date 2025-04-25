// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : securitybot.cpp
//   Purpose : ai functions for security bot
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
#include "museumguard.h"
#include "zzz.h"

// PP: TEMP?: radius of Museum Guard for shadow map
// PP: TODO (how?): get bounds of actor instances on the fly so's to optimise shadow map resolution
#define SHADOWMAP_RADIUS_MUSEUMGUARD		METERTOUNIT(4.7f)

static SEQ idle[] = 
{
	{SEQ_SETROTSPEED,NULL,RAD(180.0f)},
	{SEQ_RETURN,"walk1",0},
	{SEQ_PLAYSAMPLE,"sgfeetcreak.wav",FALSE},
	{SEQ_PLAY,"idle1",1.0f},
	{SEQ_PLAY,"idle2",1.0f},
	{SEQ_PLAYSAMPLE,"guard_2.wav",FALSE},
	{SEQ_PLAY,"idle3",0},
	{SEQ_TURNLEFT,NULL,90.0f},
	{SEQ_PLAY,"walk1",2.0f},
	{SEQ_PLAYSAMPLE,"sgmunching.wav",FALSE},
	{SEQ_PLAY,"idle9",0},
	{SEQ_PLAYSAMPLE,"sgburp.wav",FALSE},
	{SEQ_PLAY,"idle10",0},
	{SEQ_PLAY,"idle12",0},
	{SEQ_TURNRIGHT,NULL,180.0f},
	{SEQ_PLAY,"idle13",0},
	{SEQ_RETURN,"walk3",0},
	{SEQ_PLAY,"idle17"},
	{SEQ_TURNLEFT,NULL,90.0f},
	{SEQ_PLAYSAMPLE,"guard_2.wav",FALSE},
	{SEQ_PLAY,"idle3",0},
	{SEQ_PLAYSAMPLE,"guard_3.wav",FALSE},
	{SEQ_PLAY,"idle4",0},
	{SEQ_TURNRIGHT,NULL,90.0f},
	{SEQ_PLAY,"idle13",0},
	{SEQ_PLAY,"idle15",0},
	{SEQ_PLAY,"walk3",2.0f},
	{SEQ_PLAY,"idle14",0},
	{SEQ_PLAY,"idle16",0},
	{SEQ_TURNLEFT,NULL,180.0f},
	{SEQ_PLAY,"idle17",0},
	{SEQ_RETURN,"walk1",0},
	{SEQ_TURNRIGHT,NULL,90.0f},
	{SEQ_END,NULL,0},
};

static SEQ aware[] =
{
	{SEQ_RETURN,"run1",0},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_STARTEXCLAMATION,NULL,0},
	{SEQ_PLAYSAMPLE,"sgblowwhistle.wav",FALSE},
	{SEQ_PLAY,"react1",0},
	{SEQ_FINDNEWCENTER,NULL,0},
	{SEQ_RETURN,"run1",0},
	{SEQ_END,NULL,0},
};

static SEQ attack1[] = 
{
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,true},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_CHASE,"run2",9},

	{SEQ_PLAYSAMPLE,"sgdive.wav",FALSE},
	{SEQ_ATTACK,"attack1",0},
	{SEQ_PLAYSAMPLE,"sgroll.wav",FALSE},
	{SEQ_STUNNED,"idle6",0},
	{SEQ_PLAYSAMPLE,"guard_4.wav",FALSE},
	{SEQ_STUNNED,"idle8",0},
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},
	{SEQ_END,NULL,0},

	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},
	{SEQ_PLAYSAMPLE,"guard_16.wav",FALSE},
	{SEQ_PLAY,"idle5",0},
	/*{SEQ_PLAYSAMPLE,"guard_14.wav",true},
	{SEQ_RETURN,"walk2",0},
	{SEQ_STOPSAMPLE,NULL,0},*/
	{SEQ_END,NULL,0},
};

static SEQ runaway[] = 
{
	{SEQ_LOOKAWAYFROMTAZ,NULL,0},
	{SEQ_PLAYSAMPLE,"guard_5.wav",FALSE},
	{SEQ_PLAY,"react3",2.0f},
	{SEQ_PLAY,"idle5",2.0f},
	{SEQ_END,NULL,0},
};

static SEQ attack2[] =
{
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,true},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_ATTACK,"attack4",0},
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},
	{SEQ_END,NULL,0},
};

static SEQ defend[] =
{
	{SEQ_END,NULL,0},
};

static SEQ spinRear[] =
{
	{SEQ_PLAYSAMPLE,"guard_6.wav", FALSE},// PP: added ".wav"
	{SEQ_PLAY,"react6",0},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_PLAYSAMPLE,"sgroll.wav",FALSE},
	{SEQ_PLAY,"idle6",0},
	{SEQ_PLAYSAMPLE,"sggetup.wav",FALSE},
	{SEQ_DISABLED,"idle7",8.0f},
	{SEQ_PLAYSAMPLE,"guard_4.wav",FALSE},
	{SEQ_PLAY,"idle8",0},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAY,"idle12",0},
	{SEQ_END,NULL,0},
};

static SEQ spinFront[] =
{
	{SEQ_ATTACK,"attack4",0},
	{SEQ_END,NULL,0},
};

static SEQ biteFront[] =
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAYSAMPLE,"sglook.wav",false},
	{SEQ_ATTACK,"attack2",0},
	{SEQ_END,NULL,0},
};

static SEQ biteRear[] =
{
	{SEQ_PLAYSAMPLE,"guard_12.wav",FALSE},
	{SEQ_PLAY,"react2",0},
	{SEQ_LOOKAWAYFROMTAZ,NULL,0},
	{SEQ_PLAYSAMPLE,"guard_5.wav",FALSE},
	{SEQ_PLAY,"react3",2.0f},
	{SEQ_END,NULL,0},
};

static SEQ scareFront[] =
{
	{SEQ_CHASE,"run2",4},
	{SEQ_PLAY,"attack2",0},
	{SEQ_PLAYSAMPLE,"sglook.wav",FALSE},
	{SEQ_END,NULL,0},
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},
	{SEQ_PLAYSAMPLE,"guard_16.wav",FALSE},
	{SEQ_PLAY,"idle5",0},
	{SEQ_PLAYSAMPLE,"guard_14.wav",true},
	{SEQ_RETURN,"walk2",0},
	{SEQ_STOPSAMPLE,NULL,0},
	{SEQ_END,NULL,0},
};

static SEQ scareRear[] = 
{
	{SEQ_PLAYSAMPLE,"guard_13.wav",FALSE},
	{SEQ_PLAY,"react4",0},
	{SEQ_PLAYSAMPLE,"guard_7.wav",FALSE},
	{SEQ_STUNNED,"react5",2.0f},
	{SEQ_END,NULL,0},
};

static SEQ tooClose[] = 
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAYSAMPLE,"sglook.wav",FALSE},
	{SEQ_ATTACK,"attack2",0},
	{SEQ_END,NULL,0},
};

static SEQ outOfRange[] =
{
	{SEQ_PLAYSAMPLE,"guard_16.wav",FALSE},
	{SEQ_PLAY,"idle5",0},
	{SEQ_RETURN,"walk2",0},
	{SEQ_END,NULL,0},
};

static SEQ puzzled[] = 
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAYSAMPLE,"guard_11.wav", FALSE},
	{SEQ_STARTQUESTION,NULL,0},
	{SEQ_PLAY,"disguise",0},
	{SEQ_COSTUMEDONE,NULL,0},
	{SEQ_RETURN,"walk1",0},
	{SEQ_END,NULL,0},
};

static SEQ puzzledDeptstr[] = 
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAYSAMPLE,"guard_11.wav", FALSE},
	{SEQ_STARTQUESTION,NULL,0},
	{SEQ_PLAY,"disguise2",0},
	{SEQ_ENDQUESTION,NULL,0},
	{SEQ_COSTUMEDONE,NULL,0},
	{SEQ_RETURN,"walk1",0},
	{SEQ_END,NULL,0},
};

static SEQ rap[] =
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_STUNNED,"react7",0},
	{SEQ_END,NULL,0},
};

/* --------------------------------------------------------------------------------
   Function : MuseumGuardIdleSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int MuseumGuardIdleSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = idle;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : MuseumGuardAwareSeq
   Purpose : Respond to requests for aware seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int MuseumGuardAwareSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = aware;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	if (bmRand()%2)
	{
		switch(bmRand()%4)
		{
		case 0:	PlaySample("guard_1.wav", 255, actorInstance->actorInstance.position);		break;
		case 1:	PlaySample("guard_8.wav", 255, actorInstance->actorInstance.position);	break;
		case 2:	PlaySample("guard_9.wav", 255, actorInstance->actorInstance.position);	break;
		case 3:	PlaySample("guard_10.wav", 255, actorInstance->actorInstance.position);	break;
		}
	}

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : MuseumGuardAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
static int curSeq = 0;

int MuseumGuardAttackSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	if (curSeq==0)
	{
		actorInstance->genericAI->currentSeq = attack1;
	}
	else
	{
		actorInstance->genericAI->currentSeq = runaway;
	}
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : MuseumGuardTooCloseSeq
   Purpose : Respond to requests for defend seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int MuseumGuardTooCloseSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = biteFront;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : MuseumGuardPuzzledSeq
   Purpose : Respond to requests for puzzled seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/

int MuseumGuardPuzzledSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	if (map.sceneNumber==SCENE_DEPTSTR)
	{
		actorInstance->genericAI->currentSeq = puzzledDeptstr;
	}
	else
	{
		actorInstance->genericAI->currentSeq = puzzled;
	}
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : MuseumGuardAttackedSeq
   Purpose : Respond to requests for attacked seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int MuseumGuardAttackedSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	int random;

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
		if (actorInstance->genericAI->currentSeq[actorInstance->genericAI->seqIdx].type==SEQ_STUNNED)
		{
			actorInstance->genericAI->currentSeq = biteRear;
		}
		else
		{
			actorInstance->genericAI->currentSeq = biteFront;
		}
		break;
	case ATTACKTYPE_BITEREAR:
		actorInstance->genericAI->currentSeq = biteRear;
		break;
	case ATTACKTYPE_SPINFRONT:
		if (actorInstance->genericAI->currentSeq[actorInstance->genericAI->seqIdx].type==SEQ_STUNNED)
		{
			//guard is already stunned
			actorInstance->genericAI->currentSeq = spinRear;
		}
		else
		{
			actorInstance->genericAI->currentSeq = spinFront;
		}
		break;
	case ATTACKTYPE_COSTUMEREAR:
		if (map.sceneNumber==SCENE_DEPTSTR)
		{
			actorInstance->genericAI->currentSeq = rap;
			break;
		}
	case ATTACKTYPE_SPINREAR:
	case ATTACKTYPE_SPITREAR:
		actorInstance->genericAI->currentSeq = spinRear;
		break;
	case ATTACKTYPE_COSTUMEFRONT:
		if (map.sceneNumber==SCENE_DEPTSTR)
		{
			actorInstance->genericAI->currentSeq = rap;
		}
		else
		{
			actorInstance->genericAI->currentSeq = spinRear;
		}
		break;
	case ATTACKTYPE_SPITFRONT:
		if (actorInstance->genericAI->currentSeq[actorInstance->genericAI->seqIdx].type==SEQ_STUNNED)
		{
			actorInstance->genericAI->currentSeq = scareRear;
		}
		else
		{
			actorInstance->genericAI->currentSeq = spinFront;
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
   Function : MuseumGuardOutOfRangeSeq
   Purpose : Respond to requests for out of range seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int MuseumGuardOutOfRangeSeq(struct TAG_ACTORINSTANCE *actorInstance)
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

	bmVectorSet(tempVector,0.0f,0.5f,1.0f,1.0f);

	bmVectorRotateY(tempVector,tempVector,Aabs(-yangle+PI));

	bmVectorCMul(gameStatus.player1->actorBehaviour->physics.velocity, tempVector, METERTOUNIT(7.0f));
}

/*  --------------------------------------------------------------------------------
	Function	: MuseumGuardAttack
	Purpose		: Called when the museum guard does an attack move
	Parameters	: actorInstance, type
	Returns		: 
	Info		: 
*/
void MuseumGuardAttack(ACTORINSTANCE *actorInstance,int type)
{
	char	*node;

	switch(type)
	{
	case 1:
		node = "lfinger";
		break;
	case 2:
		node = "truncheon";
		break;
	case 3:
		node = "root";
		break;
	case 4:
		PlaySample("sgfstepleft.wav",255,actorInstance->actorInstance.position);
		return;
	case 5:
		PlaySample("sgfstepright.wav",255,actorInstance->actorInstance.position);
		return;
	}

	if (DistFromNodeToTaz(actorInstance,node) < METERTOUNIT(1.5f))
	{
		switch(type)
		{
		case 1:
		case 2:
			if (EnterState(gameStatus.player1,STATE_PLAYANIMATION))
			{
				FlushAnimationQueue(gameStatus.player1);
				PlayAnimationByName(gameStatus.player1,"react2",1.0f,0,0,0.1f,NONE);

				// TP: Turn taz to face character
				FacePoint(gameStatus.player1,actorInstance->actorInstance.position,0.0f,0.0f);

				RequestRumbleEffect(gameStatus.player[0].controller, 180, 0.0f, 0.5f, 0.75f, 0.0f, WAVEFORM_NORMAL);
			}
			break;
		case 3:
			if (EnterState(gameStatus.player1, STATE_PROJECTILESLIDE))
			{
				FacePoint(gameStatus.player1, actorInstance->actorInstance.position, 0.0f, RAD(180.0f));
				StopCollidingWith(gameStatus.player1,actorInstance,0.2f);
				myStateChangeCallback(gameStatus.player1);

				RequestRumbleEffect(gameStatus.player[0].controller, 180, 0.0f, 0.5f, 0.75f, 0.0f, WAVEFORM_NORMAL);
			}
			break;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : MuseumGuardAttackCallback
   Purpose : Called each frame during an attack 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
int MuseumGuardAttackCallback(struct TAG_ACTORINSTANCE *actorInstance)
{
	return FALSE;	// continue processing pattern
}

/* --------------------------------------------------------------------------------
   Function : MuseumGuardStateChangeCallback
   Purpose : Called when state changed 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void MuseumGuardStateChangeCallback(ACTORINSTANCE *actorInstance, int state, int lastState)
{
	actorInstance->genericAI->flags &=~ GENERICAI_DONTCHANGESTATE;
	if(lastState == AISTATE_IDLE)
	{
		DestroyInjuryStars(actorInstance);
	}
	if(lastState == AISTATE_TOOCLOSE)
	{
		actorInstance->genericAI->currentAnimSpeed = 1.0f;
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
   Function : MuseumGuardTazSquashedCallback
   Purpose : Called when state changed 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int MuseumGuardTazSquashedCallback(ACTORINSTANCE *actorInstance)
{
	TAZCATCHERINFO	*guardInfo = (TAZCATCHERINFO*)actorInstance->genericAI->customData;

	if((guardInfo->timeSinceLastAttack += fTime) > 5.0f)
	{
		SetAIAttackCallback(actorInstance,MuseumGuardAttackCallback);

		// sort out taz catcher
		if(CheckIfPlayingAnimation(actorInstance, "idle7"))
		{
			FlushAnimationQueue(actorInstance);
			PlaySample("guard_4.wav",255,actorInstance->actorInstance.position);
			PlayAnimationByName(actorInstance, "idle8", 1.0f, 0, 0, 0.1f, NONE);
		}

		ResetCharacterAfterJump(actorInstance);

		return FALSE;
	}
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : MuseumGuardMessageHandler
   Purpose : Called when a message is sent in the sequence
   Parameters : ACTORINSTANCE *, char *message
   Returns : 
   Info : 
*/
int MuseumGuardMessageHandler(ACTORINSTANCE *actorInstance, char *message)
{
	GENERICAI	*genericAI = actorInstance->genericAI;

	// TP: Museum Guard
	if (strcmpi(message,"tackle")==0)
	{
		MuseumGuardAttack(actorInstance,1);
		return true;
	}
	if (strcmpi(message,"truncheon")==0)
	{
		MuseumGuardAttack(actorInstance,2);
		return true;
	}
	if (strcmpi(message,"mgbelly")==0)
	{
		MuseumGuardAttack(actorInstance,3);
		return true;
	}
	if (strcmpi(message,"sgfootl")==0)
	{
		MuseumGuardAttack(actorInstance,4);
		return true;
	}
	if (strcmpi(message,"sgfootr")==0)
	{
		MuseumGuardAttack(actorInstance,5);
		return true;
	}

	if (strcmpi(message,"sb_recovernow")==0)
	{
		if (CheckIfPlayingAnimation(gameStatus.player1,"recover22c"))
		{
			FlushAnimationQueue(gameStatus.player1);
			PlayAnimationByName(gameStatus.player1,"recover22c",1.0f,0,0,0.1f,NONE);
			PlaySample("taz_gen_10a.wav",255);
		}
		if (CheckIfPlayingAnimation(gameStatus.player1,"recover21b"))
		{
			FlushAnimationQueue(gameStatus.player1);
			PlayAnimationByName(gameStatus.player1,"recover21b",1.0f,0,0,0.1f,NONE);
		}
		return false;
	}
	return false;
}

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceMuseumGuard
   Purpose : Setup the basic ai for a zookeeper
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceMuseumGuard(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList)
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
	SetActorMaxSpeed(actorInstance->actorBehaviour,METERTOUNIT(15)); // set max speed 8mps
	SETVECTOR(tempVector,0.0f,METERTOUNIT(1.0f),0.0f,1.0f);
	SetActorCylinder(0,actorInstance->actorBehaviour,tempVector,METERTOUNIT(0.8f),METERTOUNIT(2.0f));
	AddActorToCollisionCylinderCache(&collisionCache,actorInstance);

	// create ai structure
	if (CreateActorInstanceGenericAI(actorInstance,activeList,inactiveList)==FALSE) return FALSE;

	AISetShadow(actorInstance,METERTOUNIT(1.6f),METERTOUNIT(2.5f), SHADOWMAP_RADIUS_MUSEUMGUARD);

	// fill in custom parms
	genericAI = actorInstance->genericAI;
	bmVectorCopy(genericAI->center,actorInstance->actorInstance.position);
	genericAI->deactivationRange = METERTOUNIT(15.0f);
	genericAI->awarenessRange = METERTOUNIT(6.0f);
	genericAI->movementRange = METERTOUNIT(4.0f);
	genericAI->standOffRange = METERTOUNIT(1.9f);
	genericAI->personalSpace = METERTOUNIT(1.3f);
	genericAI->viewAngle = PI/3;
	genericAI->awareAngle = PI*0.85f;

	genericAI->alwaysClipPosition = true;
	genericAI->instantAware = true;
	genericAI->instantAttack = true;
	genericAI->bounceCRC = bkCRC32((uchar*)"bounce",6,0);

	genericAI->headNode = "hat";
	SETVECTOR(genericAI->starsOffset,0.0f,METERTOUNIT(0.2f),0.0f,1.0f);
	SETVECTOR(genericAI->starVars,METERTOUNIT(0.3f),METERTOUNIT(0.4f),METERTOUNIT(0.0f),METERTOUNIT(0.0f));

	SetAISeqFunctions(actorInstance,MuseumGuardIdleSeq,MuseumGuardAwareSeq,MuseumGuardAttackSeq,
		MuseumGuardTooCloseSeq,MuseumGuardAttackedSeq,MuseumGuardOutOfRangeSeq,MuseumGuardPuzzledSeq);

	SetAIAttackCallback(actorInstance,MuseumGuardAttackCallback);
	SetAIChangeStateCallback(actorInstance, MuseumGuardStateChangeCallback);
	SetAIMessageHandler(actorInstance,MuseumGuardMessageHandler);

/*	genericAI->useMoveBox = true;

	bmMatCopy(genericAI->moveBox.matrix,actorInstance->actorInstance.objectToWorld);
	bmMatInverse(genericAI->moveBox.invmatrix,actorInstance->actorInstance.objectToWorld);
	genericAI->moveBox.bbox.xmax = METERTOUNIT(2.0f);
	genericAI->moveBox.bbox.xmin = METERTOUNIT(-2.0f);
	genericAI->moveBox.bbox.ymax = METERTOUNIT(2.0f);
	genericAI->moveBox.bbox.ymin = METERTOUNIT(-2.0f);
	genericAI->moveBox.bbox.zmax = METERTOUNIT(5.0f);
	genericAI->moveBox.bbox.zmin = METERTOUNIT(-5.0f);
*/
	genericAI->customData = (TAZCATCHERINFO*)ZALLOC(sizeof(TAZCATCHERINFO));
	ASSERT(genericAI->customData);

	return TRUE;
}
