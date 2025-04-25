// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : minerbot.cpp
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
#include "minerbot.h"
#include "zzz.h"
#include "bjshellac.h"

// PP: TEMP?: radius of Security Bot for shadow map
// PP: TODO? (how?): get bounds of actor instances on the fly so's to optimise shadow map resolution
#define SHADOWMAP_RADIUS_MINERBOT		METERTOUNIT(4.0f)

#define PATROL_TIME		(0.5f)

static SEQ idle1[] = 
{
	{SEQ_RETURN,"walk1",0},
	{SEQ_PLAY,"idle8",0},
	{SEQ_SETROTSPEED,NULL,RAD(90.0f)},
	{SEQ_ROTATERIGHT,"idle9",2.0f},
	{SEQ_PLAY,"idle10",0},
	{SEQ_TURNLEFT,NULL,90},
	{SEQ_END,NULL,0},
};

static SEQ aware[] =
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAYSAMPLE,"mbdrillintoground.wav",FALSE},
	{SEQ_PLAY,"idle2",0},
	{SEQ_SETROTSPEED,NULL,RAD(360.0f)},
	{SEQ_CYLCOLLISION,NULL,false},
	{SEQ_SETSTANDOFF,NULL,METERTOUNIT(0.2f)},
	{SEQ_SETROTSPEED,NULL,RAD(180.0f)},
	{SEQ_ROTATELEFT,"walk9",10.0f},
	{SEQ_SETROTSPEED,NULL,RAD(360.0f)},
	{SEQ_PLAYSAMPLE, "mbburstthrough.wav", FALSE},
	{SEQ_PLAY,"idle3",0},
	{SEQ_END,NULL,0},

	
/*
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAY,"react5",0},
	{SEQ_SETROTSPEED,NULL,RAD(90.0f)},
	{SEQ_ROTATELEFT,"walk4",2.0f},
	{SEQ_SETROTSPEED,NULL,RAD(360.0f)},
	{SEQ_RETURN,"walk4",0},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAY,"idle1",0},
	{SEQ_PLAY,"react1",0},
	{SEQ_PLAY,"idle8",0},
	{SEQ_SETROTSPEED,NULL,RAD(90.0f)},
	{SEQ_ROTATELEFT,"idle9",2.0f},
	{SEQ_END,NULL,0},
	*/
};

static SEQ attack1[] = 
{
// TP: 	{SEQ_PLAYSAMPLE,"mbdrillintoground.wav",FALSE},
// TP: 	{SEQ_PLAY,"idle2",0},
	{SEQ_SETROTSPEED,NULL,RAD(360.0f)},
	{SEQ_CYLCOLLISION,NULL,false},
	{SEQ_SETSTANDOFF,NULL,METERTOUNIT(0.2f)},
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,true},
	{SEQ_CHASE,"walk8",5},
	{SEQ_PLAYSAMPLE,"mbburstthrough.wav",FALSE},
	{SEQ_ATTACK,"attack4",0},
	{SEQ_SETSTANDOFF,NULL,METERTOUNIT(2.0f)},
	{SEQ_END,NULL,0},

	{SEQ_CYLCOLLISION,NULL,true},
	{SEQ_PLAY,"idle6",0},
	{SEQ_PLAYSAMPLE, "mbburstthrough.wav", FALSE},
	{SEQ_PLAY,"idle3",0},
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},
	{SEQ_RETURN,"walk1",0},
	{SEQ_PLAYSAMPLE,"mbdrillintoground.wav",FALSE},
	{SEQ_PLAY,"idle2",0},
	{SEQ_END,NULL,0},
/*	{SEQ_CHASE,"walk8",0},
	{SEQ_PLAYSAMPLE,"mbburstthrough.wav",FALSE},
	{SEQ_ATTACK,"attack4",0},
	{SEQ_FACECENTER,NULL,0},
	{SEQ_RETURN,"walk8",0},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_CYLCOLLISION,NULL,true},
	{SEQ_SETSTANDOFF,NULL,METERTOUNIT(2.0f)},
	{SEQ_PLAYSAMPLE, "mbburstthrough.wav", FALSE},
	{SEQ_PLAY,"idle3",0},
	{SEQ_PLAY,"idle4",0},
	{SEQ_CHASE,"walk5",0},
	{SEQ_ATTACK,"attack2",0},
	{SEQ_RETURN,"walk4",0},
	{SEQ_PLAYSAMPLE,"mbdrillintoground.wav",FALSE},
	{SEQ_PLAY,"idle2",0},
	{SEQ_END,NULL,0},*/
};

static SEQ defend[] =
{
	{SEQ_END,NULL,0},
};

static SEQ spinFront[] = 
{
	{SEQ_PLAY,"react2",0},
	{SEQ_ATTACK,"attack1",0},
	{SEQ_END,NULL,0},
};

static SEQ spinRear[] =
{
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_PLAY,"react3",0},
	{SEQ_DISABLED,NULL,10.0f},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAY,"idle5",0},
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
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_PLAY,"react3",0},
	{SEQ_DISABLED,NULL,10.0f},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAY,"idle5",0},
	{SEQ_END,NULL,0},
};

static SEQ scareFront[] =
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_ATTACK,"attack5",0},
	{SEQ_END,NULL,0},
};

static SEQ costume[] =
{
	{SEQ_LOOKAWAYFROMTAZ,NULL,0},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_PLAY,"react3",0},
	{SEQ_DISABLED,NULL,10.0f},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAY,"idle5",0},
	{SEQ_END,NULL,0},
};

static SEQ scareRear[] = 
{
	{SEQ_ATTACK,"attack1",0},
	{SEQ_END,NULL,0},
};

static SEQ tooClose[] = 
{
	{SEQ_PLAYSAMPLE,"mbburstthrough.wav",FALSE},
	{SEQ_ATTACK,"attack4",0},
	{SEQ_END,NULL,0},
};

static SEQ outOfRange[] =
{
	{SEQ_PLAYSAMPLE,"sbotroll.wav",FALSE},
	{SEQ_RETURN,"walk1",0},
	{SEQ_STOPSAMPLE,NULL,0},
	{SEQ_END,NULL,0},
};

static SEQ puzzled[] = 
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_STARTQUESTION,NULL,0},
	{SEQ_PLAY,"disguise",0.0f},
	{SEQ_ENDQUESTION,NULL,0},
	{SEQ_COSTUMEDONE,NULL,0},
	{SEQ_END,NULL,0},
};

/* --------------------------------------------------------------------------------
   Function : MinerBotIdleSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int MinerBotIdleSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = idle1;

	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : MinerBotAwareSeq
   Purpose : Respond to requests for aware seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int MinerBotAwareSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	if((CheckIfPlayingAnimation(actorInstance,"idle5"))||(CheckIfPlayingAnimation(actorInstance,"idle6"))) return FALSE;

	actorInstance->genericAI->currentSeq = aware;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : MinerBotAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int MinerBotAttackSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	int			tempInt;
	
	if((CheckIfPlayingAnimation(actorInstance,"idle5"))||(CheckIfPlayingAnimation(actorInstance,"idle6"))) return FALSE;

	actorInstance->genericAI->currentSeq = attack1;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : MinerBotTooCloseSeq
   Purpose : Respond to requests for defend seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int MinerBotTooCloseSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = tooClose;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : MinerBotGuardPuzzledSeq
   Purpose : Respond to requests for puzzled seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/

int MinerBotPuzzledSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = puzzled;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : MinerBotAttackedSeq
   Purpose : Respond to requests for attacked seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int MinerBotAttackedSeq(struct TAG_ACTORINSTANCE *actorInstance)
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
	case ATTACKTYPE_BITEREAR:
		actorInstance->genericAI->currentSeq = biteRear;
		break;
	case ATTACKTYPE_SPINFRONT:
		actorInstance->genericAI->currentSeq = spinFront;
		break;
	case ATTACKTYPE_COSTUMEFRONT:
	case ATTACKTYPE_COSTUMEREAR:
	case ATTACKTYPE_SPITFRONT:
		actorInstance->genericAI->currentSeq = costume;
		break;
	case ATTACKTYPE_SPINREAR:
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
   Function : MinerBotOutOfRangeSeq
   Purpose : Respond to requests for out of range seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int MinerBotOutOfRangeSeq(struct TAG_ACTORINSTANCE *actorInstance)
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

	PlayAnimationByName(actorInstance, "waterbounce", 1.0f, TRUE, 0, 0.1f, NONE);

	actorInstance->actorAnimation->useMotionBone = FALSE;

	yangle = GetAngleAroundYFromQuat(gameStatus.player1->actorInstance.orientation);

	bmVectorSet(tempVector,0.0f,1.0f,0.3f,1.0f);

	bmVectorRotateY(tempVector,tempVector,Aabs(-yangle+PI));

	bmVectorCMul(gameStatus.player1->actorBehaviour->physics.velocity, tempVector, METERTOUNIT(10.0f));
}

/*  --------------------------------------------------------------------------------
	Function	: MinerBotAttack
	Purpose		: Called when the security bot does an attack move
	Parameters	: actorInstance, type of attack
	Returns		: 
	Info		: 
*/
void MinerBotAttack(ACTORINSTANCE *actorInstance, int type)
{
	char			*node,*node2=NULL;

	switch(type)
	{
	case 1:
		node = "drillcontrol";
		break;
	case 2:
		node = "rusnap";
		node2 = "lusnap";
		break;
	case 3:
		node = "dynamite";
		break;
	}

	if ((DistFromNodeToTaz(actorInstance,node) < METERTOUNIT(2.0f))||((node2)&&(DistFromNodeToTaz(actorInstance,node2) < METERTOUNIT(1.5f))))
	{
		switch(type)
		{
		case 1:
			RequestStateChangePlus(gameStatus.player1, STATE_PROJECTILESLIDE, myStateChangeCallback);
			RequestRumbleEffect(gameStatus.player[0].controller, 180, 0.0f, 0.5f, 0.75f, 0.0f, WAVEFORM_NORMAL);
			break;
		case 2:
			RequestStateChange(gameStatus.player1,STATE_PLAYANIMATION);
			FlushAnimationQueue(gameStatus.player1);
			PlayAnimationByName(gameStatus.player1,"react1",1.0f,0,0,0.1f,NONE);

			// TP: Turn taz to face character
			FacePoint(gameStatus.player1,actorInstance->actorInstance.position,0.0f,0.0f);

			RequestRumbleEffect(gameStatus.player[0].controller, 180, 0.0f, 0.5f, 0.75f, 0.0f, WAVEFORM_NORMAL);
			break;
		case 3:
			TBVector				pos,vel;
			TBActorNodeInstance		*nodePtr;

			bmVectorSet(vel,0.0f,0.0f,0.0f,1.0f);

			nodePtr = baFindNode(actorInstance->actorInstance.rootNodeInstance,node);
			if (nodePtr)
			{
				baGetNodesWorldPosition(&actorInstance->actorInstance,nodePtr,pos);
				CreateDynamite(actorInstance, pos, vel, 1.0f);
			}

			break;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : MinerBotAttackCallback
   Purpose : Called each frame during an attack 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
int MinerBotAttackCallback(struct TAG_ACTORINSTANCE *actorInstance)
{
	TBMatrix		tempMatrix;
	TBVector		tempVector,tempVector2;
	TBVector		starsBounds = {METERTOUNIT(0.3f), METERTOUNIT(0.4f), 0.0f, 0.0f};
	TBVector		starsOffset = {0.0f, METERTOUNIT(0.2f), 0.0f, 1.0f};
	TBQuaternion	tempQuat, tempQuat2;
	TBActorNodeInstance *node;
	TAZCATCHERINFO	*botInfo = (TAZCATCHERINFO*)actorInstance->genericAI->customData;
	
/*	if(CheckIfPlayingAnimation(actorInstance, "attack4"))
	{
		node = baFindNode(actorInstance->actorInstance.rootNodeInstance,"truncheon");
		if (node)
		{
			baGetNodesWorldPosition(&actorInstance->actorInstance, node, tempVector);
		}
		else
		{
			SETVECTOR(tempVector,0.0f,0.0f,0.0f,1.0f);
		}

		bmVectorAdd(tempVector2,gameStatus.player1->actorInstance.position,gameStatus.player1->actorBehaviour->collSphere.offset);
		bmVectorSub(tempVector,tempVector,tempVector2);

		if (bmVectorLen(tempVector) < (gameStatus.player1->actorBehaviour->collSphere.radius+METERTOUNIT(0.5f)))
		{
			FlushAnimationQueue(gameStatus.player1);
			PlayAnimationByName(gameStatus.player1,"react2",1.0f,0,0,0.1f,NONE);
			ReduceCharactersHealth(gameStatus.player1,1.0f);
			RequestStateChange(gameStatus.player1,STATE_RECOVER);

			// Turn Taz to face 90 degrees to right of Bear
			bmQuatCopy(gameStatus.player1->actorBehaviour->rotAim,actorInstance->actorInstance.orientation);

			SETVECTOR(tempVector,0.0f,-1.0f,0.0f,RAD(90));
			bmRotationToQuat(tempQuat,tempVector);
			bmQuatCopy(tempQuat2,gameStatus.player1->actorBehaviour->rotAim);
			bmQuatMultiply(gameStatus.player1->actorBehaviour->rotAim,tempQuat2,tempQuat);
			bmQuatCopy(gameStatus.player1->actorInstance.orientation,gameStatus.player1->actorBehaviour->rotAim);

			SetAIAttackCallback(actorInstance,MinerBotTazSquashedCallback);
			botInfo->timeSinceLastAttack = 0.0f;
			return TRUE;	// don't continue pattern
		}
	}*/
	return FALSE;	// continue processing pattern
}

/* --------------------------------------------------------------------------------
   Function : MinerBotStateChangeCallback
   Purpose : Called when state changed 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void MinerBotStateChangeCallback(ACTORINSTANCE *actorInstance, int state, int lastState)
{
	if(lastState == AISTATE_IDLE)
	{
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
   Function : MinerBotTazSquashedCallback
   Purpose : Called when state changed 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int MinerBotTazSquashedCallback(ACTORINSTANCE *actorInstance)
{
	TAZCATCHERINFO	*botInfo = (TAZCATCHERINFO*)actorInstance->genericAI->customData;

	if((botInfo->timeSinceLastAttack += fTime) > 2.0f)
	{
		SetAIAttackCallback(actorInstance,MinerBotAttackCallback);

		// sort out taz catcher
		FlushAnimationQueue(actorInstance);

		ResetCharacterAfterJump(actorInstance);

		return FALSE;
	}
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : MinerBotMessageHandler
   Purpose : Called when a message is sent in the sequence
   Parameters : ACTORINSTANCE *, char *message
   Returns : 
   Info : 
*/
int MinerBotMessageHandler(ACTORINSTANCE *actorInstance, char *message)
{
	GENERICAI	*genericAI = actorInstance->genericAI;

	// TP: Minerbot
	if (strcmpi(message,"mbdrill")==0)
	{
		MinerBotAttack(actorInstance,1);
		return true;
	}
	if (strcmpi(message,"mbspin")==0)
	{
		MinerBotAttack(actorInstance,2);
		return true;
	}
	if (strcmpi(message,"mbdrop")==0)
	{
		MinerBotAttack(actorInstance,3);
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
   Function : CreateActorInstanceMinerBot
   Purpose : Setup the basic ai for a zookeeper
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceMinerBot(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList)
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
	SetActorCylinder(0,actorInstance->actorBehaviour,tempVector,METERTOUNIT(0.7f),METERTOUNIT(2.0f));
	AddActorToCollisionCylinderCache(&collisionCache,actorInstance);
	
	// create ai structure
	if (CreateActorInstanceGenericAI(actorInstance,activeList,inactiveList)==FALSE) return FALSE;

	// fill in custom parms
	genericAI = actorInstance->genericAI;
	bmVectorCopy(genericAI->center,actorInstance->actorInstance.position);
	genericAI->deactivationRange = METERTOUNIT(12.0f);
	genericAI->awarenessRange = METERTOUNIT(6.0f);
	genericAI->movementRange = METERTOUNIT(3.0f);
	genericAI->standOffRange = METERTOUNIT(2.0f);
	genericAI->personalSpace = METERTOUNIT(0.0f);
	genericAI->viewAngle = PI/2;

	genericAI->headNode = "hat";
	SETVECTOR(genericAI->starsOffset,0.0f,METERTOUNIT(0.2f),0.0f,1.0f);
	SETVECTOR(genericAI->starVars,METERTOUNIT(0.3f),METERTOUNIT(0.4f),METERTOUNIT(0.0f),METERTOUNIT(0.0f));

/*
	genericAI->useMoveBox = true;
	bmMatCopy(genericAI->moveBox.matrix,actorInstance->actorInstance.objectToWorld);
	bmMatInverse(genericAI->moveBox.invmatrix,actorInstance->actorInstance.objectToWorld);
	genericAI->moveBox.bbox.xmax = METERTOUNIT(2.0f);
	genericAI->moveBox.bbox.xmin = METERTOUNIT(-2.0f);
	genericAI->moveBox.bbox.ymax = METERTOUNIT(2.0f);
	genericAI->moveBox.bbox.ymin = METERTOUNIT(-2.0f);
	genericAI->moveBox.bbox.zmax = METERTOUNIT(2.0f);
	genericAI->moveBox.bbox.zmin = METERTOUNIT(-2.0f);

	memcpy(&genericAI->moveBox.attackBBox, &genericAI->moveBox.bbox, sizeof(BOUNDINGBOX));
	float tempFloat = 0.7f * actorInstance->genericAI->deactivationRange;
	memcpy(&actorInstance->genericAI->moveBox.idleBox, &actorInstance->genericAI->moveBox.bbox, sizeof(BOUNDINGBOX));
	EnlargeBBox(&actorInstance->genericAI->moveBox.idleBox, actorInstance->genericAI->moveBox.matrix, actorInstance->genericAI->moveBox.invmatrix, tempFloat);
	bmVectorCopy(actorInstance->genericAI->orgCenter, actorInstance->genericAI->center);
*/

	SetAISeqFunctions(actorInstance,MinerBotIdleSeq,MinerBotAwareSeq,MinerBotAttackSeq,
		MinerBotTooCloseSeq,MinerBotAttackedSeq,MinerBotOutOfRangeSeq,MinerBotPuzzledSeq);

	SetAIAttackCallback(actorInstance,MinerBotAttackCallback);
	SetAIChangeStateCallback(actorInstance, MinerBotStateChangeCallback);
	SetAIMessageHandler(actorInstance,MinerBotMessageHandler);

	genericAI->alwaysClipPosition = true;
	genericAI->instantAttack = true;
	genericAI->instantAware = true;
	genericAI->attackWithoutCollision = false;

	return TRUE;
}
