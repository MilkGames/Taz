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
#include "securitybot.h"
#include "zzz.h"

// PP: TEMP?: radius of Security Bot for shadow map
// PP: TODO? (how?): get bounds of actor instances on the fly so's to optimise shadow map resolution
#define SHADOWMAP_RADIUS_SECURITYBOT		METERTOUNIT(4.0f)

#define PATROL_TIME		(1.0f)

static SEQ idle1[] = 
{
	{SEQ_RETURN,"walk2",0},
	{SEQ_SETROTSPEED,NULL,RAD(90)},
	{SEQ_PLAYSAMPLE,"sbotroll.wav",TRUE},
	{SEQ_PLAY,"walk1",PATROL_TIME/2.0f},
	{SEQ_STOPSAMPLE,NULL,0},
	{SEQ_PLAYSAMPLE,"sbotspin90.wav",FALSE},
	{SEQ_TURNRIGHT,NULL,90.0f},
	{SEQ_PLAY,"turn2",0.5f},
	{SEQ_PLAYSAMPLE,"sbotroll.wav",TRUE},
	{SEQ_PLAY,"walk1",PATROL_TIME/2.0f},
	{SEQ_STOPSAMPLE,NULL,0},
	{SEQ_PLAYSAMPLE,"sbotspin90.wav",FALSE},
	{SEQ_TURNRIGHT,NULL,90.0f},
	{SEQ_PLAY,"turn2",0.5f},
	{SEQ_PLAYSAMPLE,"sbotroll.wav",TRUE},
	{SEQ_PLAY,"walk1",PATROL_TIME},
	{SEQ_STOPSAMPLE,NULL,0},
	{SEQ_PLAYSAMPLE,"sbotspin90.wav",FALSE},
	{SEQ_TURNRIGHT,NULL,90.0f},
	{SEQ_PLAY,"turn2",0.5f},
	{SEQ_PLAYSAMPLE,"sbotroll.wav",TRUE},
	{SEQ_PLAY,"walk1",PATROL_TIME},
	{SEQ_STOPSAMPLE,NULL,0},
	{SEQ_PLAYSAMPLE,"sbotspin90.wav",FALSE},
	{SEQ_TURNRIGHT,NULL,90.0f},
	{SEQ_PLAY,"turn2",0.5f},
	{SEQ_PLAYSAMPLE,"sbotroll.wav",TRUE},
	{SEQ_PLAY,"walk1",PATROL_TIME},
	{SEQ_STOPSAMPLE,NULL,0},
	{SEQ_PLAYSAMPLE,"sbotspin90.wav",FALSE},
	{SEQ_TURNRIGHT,NULL,90.0f},
	{SEQ_PLAY,"turn2",0.5f},
	{SEQ_PLAYSAMPLE,"sbotroll.wav",TRUE},
	{SEQ_PLAY,"walk1",PATROL_TIME/2.0f},
	{SEQ_STOPSAMPLE,NULL,0},
	{SEQ_PLAYSAMPLE,"sbotspin180.wav",FALSE},
	{SEQ_TURNRIGHT,NULL,180.0f},
	{SEQ_PLAY,"turn2",1.0f},
	{SEQ_PLAY,"walk3",PATROL_TIME/2.0f},
	{SEQ_PLAYSAMPLE,"sbotspin90.wav",FALSE},
	{SEQ_TURNLEFT,NULL,90.0f},
	{SEQ_PLAY,"turn1",0.5f},
	{SEQ_PLAY,"walk3",PATROL_TIME},
	{SEQ_PLAYSAMPLE,"sbotspin90.wav",FALSE},
	{SEQ_TURNLEFT,NULL,90.0f},
	{SEQ_PLAY,"turn1",0.5f},
	{SEQ_PLAY,"walk3",PATROL_TIME},
	{SEQ_PLAYSAMPLE,"sbotspin90.wav",FALSE},
	{SEQ_TURNLEFT,NULL,90.0f},
	{SEQ_PLAY,"turn1",0.5f},
	{SEQ_PLAY,"walk3",PATROL_TIME},
	{SEQ_PLAYSAMPLE,"sbotspin90.wav",FALSE},
	{SEQ_TURNLEFT,NULL,90.0f},
	{SEQ_PLAY,"turn1",0.5f},
	{SEQ_PLAY,"walk3",PATROL_TIME/2.0f},
	{SEQ_PLAYSAMPLE,"sbotspin90.wav",FALSE},
	{SEQ_TURNLEFT,NULL,90.0f},
	{SEQ_PLAY,"turn1",0.5f},
	{SEQ_PLAY,"walk3",PATROL_TIME/2.0f},
	{SEQ_PLAYSAMPLE,"sbotspin180.wav",FALSE},
	{SEQ_TURNLEFT,NULL,180.0f},
	{SEQ_FINDNEWCENTER,NULL,0},
	{SEQ_RETURNNEWCENTER,"walk1",0},
	{SEQ_END,NULL,0},
};

static SEQ idle2[] = 
{
	{SEQ_PLAY,"idle3",0},
	{SEQ_PLAY,"idle4",0},
	{SEQ_STARTZZZ,NULL,1},
	{SEQ_PLAY,"idle5",5.0f},
	{SEQ_PLAY,"idle6",0},
	{SEQ_PLAY,"idle5",5.0f},
	{SEQ_ENDZZZ,NULL,0},
	{SEQ_PLAY,"idle7",0},
	{SEQ_PLAY,"idle8",0},
	{SEQ_END,NULL,0},
};

static SEQ aware[] =
{
	{SEQ_PLAYSAMPLE,"sbotroll.wav",TRUE},
	{SEQ_STARTEXCLAMATION,NULL,0},
	{SEQ_FINDNEWCENTER,NULL,0},
	{SEQ_RETURN,"walk1",0},
	{SEQ_STOPSAMPLE,NULL,0},
	{SEQ_END,NULL,0},
};

static SEQ attack1[] = 
{
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,true},
	{SEQ_PLAYSAMPLE,"sbotroll.wav",true},
	{SEQ_CHASE,"walk4",7},
	{SEQ_STOPSAMPLE,NULL,0},
	{SEQ_PLAYSAMPLE,"sbotswipeslow.wav",false},
	{SEQ_ATTACK,"attack1",0},
	{SEQ_WAIT,NULL,0.45f},
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},
	{SEQ_END,NULL,0},

	{SEQ_PLAYSAMPLE,"sbotloset.wav",false},
	{SEQ_PLAY,"idle1",0},
	{SEQ_PLAYSAMPLE,"sbotremovet.wav",false},
	{SEQ_PLAY,"idle2",0},
	{SEQ_PLAYSAMPLE,"sbotroll.wav",true},
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},
	/*{SEQ_RETURN,"walk2",0},
	{SEQ_STOPSAMPLE,NULL,0},*/
	{SEQ_END,NULL,0},
};

//	{SEQ_PLAYSAMPLE,"sbotmalletswipe.wav",FALSE},

static SEQ defend[] =
{
	{SEQ_END,NULL,0},
};

static SEQ spinFront[] = 
{
	{SEQ_ATTACK,"attack4",0},
	{SEQ_END,NULL,0},
};

static SEQ spinRear[] =
{
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_PLAYSAMPLE,"sbotbump.wav",FALSE},	
	{SEQ_PLAY,"react1",0},
	{SEQ_PLAYSAMPLE,"sbotspun.wav",FALSE},
	{SEQ_PLAY,"react2",0},
	{SEQ_PLAYSAMPLE,"sbotcollapse.wav",FALSE},
	{SEQ_PLAY,"react3",0},
	{SEQ_STARS,NULL,0},
	{SEQ_DISABLED,NULL,10.0f},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_PLAYSAMPLE,"sbotgetup.wav",FALSE},
	{SEQ_PLAY,"react4",0},
	{SEQ_END,NULL,0},
};

static SEQ spinRearError[] =
{
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_PLAYSAMPLE,"sbotbump.wav",FALSE},
	{SEQ_PLAY,"react1",0},
	{SEQ_PLAYSAMPLE,"sbotspun.wav",FALSE},
	{SEQ_PLAY,"react2",0},
	{SEQ_PLAYSAMPLE,"sbotcollapse.wav",FALSE},
	{SEQ_PLAY,"react3",0},
	{SEQ_STARS,NULL,0},
	{SEQ_DISABLED,NULL,10.0f},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAYSAMPLE,"sbotgetup.wav",FALSE},
	{SEQ_PLAY,"react5",0},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_END,NULL,0},
};

static SEQ biteFront[] =
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAYSAMPLE,"sbotmalletswipe.wav",FALSE},
	{SEQ_ATTACK,"attack5",0},
	{SEQ_END,NULL,0},
};

static SEQ biteRear[] =
{
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_PLAYSAMPLE,"sbotbump.wav",FALSE},
	{SEQ_PLAY,"react1",0},
	{SEQ_PLAYSAMPLE,"sbotspun.wav",FALSE},
	{SEQ_PLAY,"react2",0},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAYSAMPLE,"sbotcollapse.wav",FALSE},
	{SEQ_PLAY,"react3",0},
	{SEQ_STARS,NULL,0},
	{SEQ_DISABLED,NULL,10.0f},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAYSAMPLE,"sbotgetup.wav",FALSE},
	{SEQ_PLAY,"react4",0},
	{SEQ_END,NULL,0},
};

static SEQ scareFront[] =
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAYSAMPLE,"sbotmalletswipe.wav",FALSE},
	{SEQ_ATTACK,"attack5",0},
	{SEQ_END,NULL,0},
};

static SEQ costume[] =
{
	{SEQ_PLAYSAMPLE,"sbotspun.wav",FALSE},
	{SEQ_PLAY,"react6",0},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_DISABLED,NULL,10.0f},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAYSAMPLE,"sbotgetup.wav",FALSE},
	{SEQ_PLAY,"react5",0},
	{SEQ_END,NULL,0},
};

static SEQ scareRear[] = 
{
	{SEQ_PLAYSAMPLE,"sbotrotswipe.wav",FALSE},
	{SEQ_ATTACK,"attack4",0},
	{SEQ_END,NULL,0},
};

static SEQ tooClose[] = 
{
	{SEQ_PLAYSAMPLE,"sbotrotswipe.wav",FALSE},
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
	{SEQ_PLAYANDFACE,"disguise",0.0f},
	{SEQ_ENDQUESTION,NULL,0},
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,true},
	{SEQ_COSTUMEDONE, NULL,0},
	{SEQ_PLAYSAMPLE,"sbotcollapse.wav",FALSE},
	{SEQ_PLAY,"react3",0},
	{SEQ_STARS,NULL,0},
	{SEQ_DISABLED,NULL,3.0f},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAYSAMPLE,"sbotgetup.wav",FALSE},
	{SEQ_PLAY,"react5",0},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},
	{SEQ_END,NULL,0},
};

/* --------------------------------------------------------------------------------
   Function : SecurityBotIdleSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int SecurityBotIdleSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	if(bmRand()%2)
	{
		actorInstance->genericAI->currentSeq = idle1;
	}
	else
	{
		actorInstance->genericAI->currentSeq = idle2;
	}
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : SecurityBotAwareSeq
   Purpose : Respond to requests for aware seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int SecurityBotAwareSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	if((CheckIfPlayingAnimation(actorInstance,"idle5"))||(CheckIfPlayingAnimation(actorInstance,"idle6"))) return FALSE;

	actorInstance->genericAI->currentSeq = aware;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : SecurityBotAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int SecurityBotAttackSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	int			tempInt;
	
	if((CheckIfPlayingAnimation(actorInstance,"idle5"))||(CheckIfPlayingAnimation(actorInstance,"idle6"))) return FALSE;

	actorInstance->genericAI->currentSeq = attack1;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : SecurityBotTooCloseSeq
   Purpose : Respond to requests for defend seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int SecurityBotTooCloseSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = tooClose;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : SecurityBotGuardPuzzledSeq
   Purpose : Respond to requests for puzzled seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/

int SecurityBotPuzzledSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = puzzled;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : SecurityBotAttackedSeq
   Purpose : Respond to requests for attacked seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int SecurityBotAttackedSeq(struct TAG_ACTORINSTANCE *actorInstance)
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
   Function : SecurityBotOutOfRangeSeq
   Purpose : Respond to requests for out of range seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int SecurityBotOutOfRangeSeq(struct TAG_ACTORINSTANCE *actorInstance)
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
	Function	: SecurityBotAttack
	Purpose		: Called when the security bot does an attack move
	Parameters	: actorInstance, type of attack
	Returns		: 
	Info		: 
*/
void SecurityBotAttack(ACTORINSTANCE *actorInstance, int type)
{
	char			*node;
	TBVector		starsBounds = {METERTOUNIT(0.3f), METERTOUNIT(0.4f), 0.0f, 0.0f};
	TBVector		starsOffset = {0.0f, METERTOUNIT(0.2f), 0.0f, 1.0f};

	// TP: not if you've already been hit
	if (gameStatus.player1->actorStatus->currentState==STATE_PLAYANIMATION)
	{
		return;
	}

	switch(type)
	{
	case 1:
	case 3:
	case 4:
	case 6:
		node = "truncheon";
		break;
	case 5:
		node = "mallet2";
		break;
	}

	if (DistFromNodeToTaz(actorInstance,node) < METERTOUNIT(1.5f))
	{
		switch(type)
		{
		case 1:
		case 3:
			if (EnterState(gameStatus.player1,STATE_PLAYANIMATION))
			{
				FlushAnimationQueue(gameStatus.player1);
				PlayAnimationByName(gameStatus.player1,"react1",1.0f,0,0,0.1f,NONE);

				// TP: Turn taz to face character
				FacePoint(gameStatus.player1,actorInstance->actorInstance.position,0.0f,0.0f);
				RequestRumbleEffect(gameStatus.player[0].controller, 180, 0.0f, 0.5f, 0.75f, 0.0f, WAVEFORM_NORMAL);
			}
			break;
		case 4:
			if (EnterState(gameStatus.player1,STATE_PLAYANIMATION))
			{
				FlushAnimationQueue(gameStatus.player1);
				PlayAnimationByName(gameStatus.player1,"react2",1.0f,0,0,0.1f,NONE);

				// TP: Turn taz to face character
				FacePoint(gameStatus.player1,actorInstance->actorInstance.position,0.0f,RAD(90.0f));
				RequestRumbleEffect(gameStatus.player[0].controller, 180, 0.0f, 0.5f, 0.75f, 0.0f, WAVEFORM_NORMAL);
			}
			break;
		case 5:
			if (EnterState(gameStatus.player1,STATE_SQUASHED))
			{
				CreateInjuryStars(10, gameStatus.player1, "spinetop", starsOffset, starsBounds);
				FlushAnimationQueue(gameStatus.player1);
				if(bmRand()%2)
				{
					PlayAnimationByName(gameStatus.player1,"recover22c",1.0f,0,0,0.1f,NONE);
					PlaySample("taz_gen_10a.wav",255);
				}
				else
				{
					PlayAnimationByName(gameStatus.player1,"recover21b",1.0f,0,0,0.1f,NONE);
				}
				RequestRumbleEffect(gameStatus.player[0].controller, 180, 0.0f, 0.5f, 0.75f, 0.0f, WAVEFORM_NORMAL);
			}
			break;
		case 6:
			if (EnterState(gameStatus.player1,STATE_PLAYANIMATION))
			{
				FlushAnimationQueue(gameStatus.player1);
				PlayAnimationByName(gameStatus.player1,"react2",1.0f,0,0,0.1f,NONE);

				// TP: Turn taz to face character
				FacePoint(gameStatus.player1,actorInstance->actorInstance.position,0.0f,-RAD(90.0f));
				RequestRumbleEffect(gameStatus.player[0].controller, 180, 0.0f, 0.5f, 0.75f, 0.0f, WAVEFORM_NORMAL);
			}
			break;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : SecurityBotAttackCallback
   Purpose : Called each frame during an attack 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
int SecurityBotAttackCallback(struct TAG_ACTORINSTANCE *actorInstance)
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

			SetAIAttackCallback(actorInstance,SecurityBotTazSquashedCallback);
			botInfo->timeSinceLastAttack = 0.0f;
			return TRUE;	// don't continue pattern
		}
	}*/
	return FALSE;	// continue processing pattern
}

/* --------------------------------------------------------------------------------
   Function : SecurityBotStateChangeCallback
   Purpose : Called when state changed 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void SecurityBotStateChangeCallback(ACTORINSTANCE *actorInstance, int state, int lastState)
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
   Function : SecurityBotTazSquashedCallback
   Purpose : Called when state changed 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int SecurityBotTazSquashedCallback(ACTORINSTANCE *actorInstance)
{
	TAZCATCHERINFO	*botInfo = (TAZCATCHERINFO*)actorInstance->genericAI->customData;

	if((botInfo->timeSinceLastAttack += fTime) > 2.0f)
	{
		SetAIAttackCallback(actorInstance,SecurityBotAttackCallback);

		// sort out taz catcher
		FlushAnimationQueue(actorInstance);

		ResetCharacterAfterJump(actorInstance);

		return FALSE;
	}
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : SecurityBotMessageHandler
   Purpose : Called when a message is sent in the sequence
   Parameters : ACTORINSTANCE *, char *message
   Returns : 
   Info : 
*/
int SecurityBotMessageHandler(ACTORINSTANCE *actorInstance, char *message)
{
	GENERICAI	*genericAI = actorInstance->genericAI;

	// TP: Security bot
	if (strcmpi(message,"rtruncheon")==0)
	{
		SecurityBotAttack(actorInstance,1);
		return true;
	}
	if (strcmpi(message,"rtruncheon2")==0)
	{
		SecurityBotAttack(actorInstance,3);
		return true;
	}
	if (strcmpi(message,"eat")==0)
	{
		SecurityBotAttack(actorInstance,4);
		return true;
	}
	if (strcmpi(message,"rmallet")==0)
	{
		SecurityBotAttack(actorInstance,5);
		return true;
	}
	if (strcmpi(message,"eat2")==0)
	{
		SecurityBotAttack(actorInstance,6);
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
	}
	return false;
}

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceSecurityBot
   Purpose : Setup the basic ai for a zookeeper
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceSecurityBot(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList)
{
	GENERICAI	*genericAI;
	TBVector	tempVector;

	// create extra structures
	CreateActorInstanceBehaviour(actorInstance);
	CreateActorInstanceAnimation(actorInstance);
	CreateActorInstanceStatus(actorInstance);
	//CreateActorInstanceShadow(actorInstance,METERTOUNIT(1.0f),METERTOUNIT(2.5f),SHADOWMAP_RADIUS_SECURITYBOT);
	// TP: CreateActorInstanceCharacterInfo(actorInstance);
	SetActorInertia(actorInstance->actorBehaviour,0);
	SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
	SetActorMaxSpeed(actorInstance->actorBehaviour,METERTOUNIT(15)); // set max speed 8mps
	SETVECTOR(tempVector,0.0f,METERTOUNIT(1.0f),0.0f,1.0f);
	SetActorCylinder(0,actorInstance->actorBehaviour,tempVector,METERTOUNIT(0.7f),METERTOUNIT(2.0f));
	AddActorToCollisionCylinderCache(&collisionCache,actorInstance);
	
	// create ai structure
	if (CreateActorInstanceGenericAI(actorInstance,activeList,inactiveList)==FALSE) return FALSE;

	AISetShadow(actorInstance,METERTOUNIT(1.0f),METERTOUNIT(2.5f),SHADOWMAP_RADIUS_SECURITYBOT);

	// fill in custom parms
	genericAI = actorInstance->genericAI;
	bmVectorCopy(genericAI->center,actorInstance->actorInstance.position);
	genericAI->deactivationRange = METERTOUNIT(12.0f);
	genericAI->awarenessRange = METERTOUNIT(4.5f);
	genericAI->movementRange = METERTOUNIT(3.0f);
	genericAI->standOffRange = METERTOUNIT(2.0f);
	genericAI->personalSpace = METERTOUNIT(1.5f);
	genericAI->viewAngle = PI/2;

	genericAI->headNode = "hat";
	SETVECTOR(genericAI->starsOffset,0.0f,METERTOUNIT(0.2f),0.0f,1.0f);
	SETVECTOR(genericAI->starVars,METERTOUNIT(0.3f),METERTOUNIT(0.4f),METERTOUNIT(0.0f),METERTOUNIT(0.0f));

	SetAISeqFunctions(actorInstance,SecurityBotIdleSeq,SecurityBotAwareSeq,SecurityBotAttackSeq,
		SecurityBotTooCloseSeq,SecurityBotAttackedSeq,SecurityBotOutOfRangeSeq,SecurityBotPuzzledSeq);

	SetAIAttackCallback(actorInstance,SecurityBotAttackCallback);
	SetAIChangeStateCallback(actorInstance, SecurityBotStateChangeCallback);
	SetAIMessageHandler(actorInstance,SecurityBotMessageHandler);

	genericAI->alwaysClipPosition = true;
	genericAI->instantAttack = true;
	genericAI->instantAware = true;
	genericAI->costumeInAware = true;

	genericAI->customData = (TAZCATCHERINFO*)ZALLOC(sizeof(TAZCATCHERINFO));
	ASSERT(genericAI->customData);

	return TRUE;
}
