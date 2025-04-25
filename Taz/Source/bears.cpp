// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : bear.cpp
//   Purpose : provides functions to allow objects to be attached to characters
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
#include "bears.h"
#include "control.h"
#include "snowball.h"
#include "breakables.h"
#include "sfx.h"

// PP: TEMP?: radius of Bear for shadow map
// PP: TODO? (how?): get bounds of actor instances on the fly so's to optimise shadow map resolution
#define SHADOWMAP_RADIUS_BEAR		METERTOUNIT(3.0f)

/*
Idle1	Breathe
Idle2	Shiver
Walk1	Normal walk
React1	Push hat forward
Walk2	Lumber
Attack1	Pawswing
Stunned1	Stunned still
Walk3	Smell crawl
React2	Bitten bum
React3	Scared from behind
Death1	Fall forward
React4	Growl
Attack2	Throw snowball
React5	Stand + sniff
Death2	Spin die
*/

static SEQ idle[] = 
{
	// TP: {SEQ_CHANGESTATEONATTACKEDONLY,NULL,true},
	{SEQ_FINDNEWCENTER,NULL,0},
	{SEQ_RETURNNEWCENTER,"walk1",0},
	// TP: {SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},
	{SEQ_SETROTSPEED,NULL,RAD(45.0f)},
	{SEQ_TURNLEFT,NULL,90},
	{SEQ_WALK,"walk1",1},
	{SEQ_PLAY,"idle5",0},
	{SEQ_TURNLEFT,NULL,90},
	{SEQ_WALK,"walk3",1},
	{SEQ_PLAY,"idle6",0},
	{SEQ_PLAY,"idle2",0},
	{SEQ_TURNLEFT,NULL,90},
	{SEQ_WALK,"walk1",1},
	{SEQ_TURNLEFT,NULL,90},
	{SEQ_WALK,"walk1",1},
	{SEQ_PLAY,"idle4",0},
	{SEQ_TURNLEFT,NULL,90},
	{SEQ_END,NULL,0},
};

static SEQ brownIdle[] =
{
	{SEQ_RETURN,"walk1",0},
	{SEQ_WALK,"walk1",0.5f},
	{SEQ_TURNLEFT,NULL,90},

	{SEQ_PLAYSAMPLE,"byawn.wav",false},
	{SEQ_PLAY,"idle4",0},
	// TP: lie down and take a break
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,true},
	{SEQ_PLAY,"idle5",0},
	{SEQ_PLAYSAMPLE,"bsnore.wav",false},
	{SEQ_STUNNED,"idle6",5.0f},
	{SEQ_PLAY,"react8",0},
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},
	{SEQ_PLAYSAMPLE,"byawn.wav",false},
	{SEQ_PLAY,"idle4",0},

	{SEQ_PLAYSAMPLE,"bscratch.wav",false},
	{SEQ_PLAY,"idle3",0},
	{SEQ_PLAY,"idle1",0},
	{SEQ_TURNLEFT,NULL,90},
	{SEQ_WALK,"walk1",0.5f},
	{SEQ_TURNLEFT,NULL,90},
	{SEQ_WALK,"walk1",0.5f},
	{SEQ_PLAYSAMPLE,"byawn.wav",false},
	{SEQ_PLAY,"idle4",0},
	{SEQ_TURNLEFT,NULL,90},
	{SEQ_END,NULL,0},
};

static SEQ aware[] =
{
//	{SEQ_RETURN,"walk3",0},
	{SEQ_SETROTSPEED,NULL,RAD(90.0f)},
	{SEQ_PLAYSAMPLE,"pbsniffs.wav",false},
	{SEQ_PLAY,"idle3",0},
	{SEQ_MOTIONBONE,NULL,0},
	{SEQ_FACETAZ,"walk1",0},
	{SEQ_MOTIONBONE,NULL,1},

	{SEQ_PLAY,"react1",0},
	{SEQ_MOTIONBONE,NULL,0},
	{SEQ_FACETAZ,"walk1",0},
	{SEQ_MOTIONBONE,NULL,1},
	{SEQ_PLAY,"react6",0},
	{SEQ_MOTIONBONE,NULL,0},
	{SEQ_FACETAZ,"walk1",0},
	{SEQ_MOTIONBONE,NULL,1},

	// TP: throw snowball
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,true},
	{SEQ_SETBLENDTIME,NULL,0.0f},
	{SEQ_PLAYSAMPLE,"pbmakesnowball.wav",false},
	{SEQ_ATTACKANDFACE,"snowball1",0},
	{SEQ_MESSAGE,"pickup",0},
	{SEQ_ATTACKANDFACE,"snowball2",0},
	{SEQ_MESSAGE,"throw",0},
	{SEQ_PLAYSAMPLE,"pbthrowsb.wav",false},
	{SEQ_ATTACKANDFACE,"snowball3",0},
	{SEQ_SETBLENDTIME,NULL,0.2f},
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},

	{SEQ_SKIP,"rand",1},
	{SEQ_PLAY,"react1",0},
	{SEQ_MOTIONBONE,NULL,0},
	{SEQ_FACETAZ,"walk1",0},
	{SEQ_MOTIONBONE,NULL,1},

	{SEQ_END,NULL,0},
};

static SEQ brownAware[] =
{
	{SEQ_FACETAZ,"walk1",0},
	{SEQ_PLAYANDFACE,"idle1",3.0f},
	{SEQ_PLAYANDFACE,"idle2",0},
	{SEQ_PLAYANDFACE,"idle1",3.0f},
	{SEQ_PLAYANDFACE,"react1",0},
	{SEQ_PLAYANDFACE,"react6",0},
	{SEQ_END,NULL,0},
};

static SEQ attack[] = 
{
	{SEQ_SETROTSPEED,NULL,RAD(360.0f)},
	{SEQ_CHASE,"walk2",0},
	{SEQ_PLAYSAMPLE,"pbsmack.wav",false},
	{SEQ_ATTACK,"attack2",0},
	{SEQ_PLAY,"react1",0},
	{SEQ_PLAY,"react6",0},
	{SEQ_END,NULL,0},
};

static SEQ brownAttack[] = 
{
	{SEQ_SETROTSPEED,NULL,RAD(360.0f)},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_CHASE,"walk2",0},
	{SEQ_ATTACKANDFACE,"attack4",0},
	{SEQ_PLAYANDFACE,"idle2",0},
	// TP: need to take a break
	{SEQ_END,NULL,0},
};

static SEQ defend[] =
{
	{SEQ_END,NULL,0},
};

static SEQ attacked[] =
{
	{SEQ_PLAY,"react1",0},
	{SEQ_END,NULL,0},
};

static SEQ outOfRange[] =
{
	// TP: throw snowball
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,true},
	{SEQ_SETBLENDTIME,NULL,0.0f},
	{SEQ_ATTACKANDFACE,"snowball1",0},
	{SEQ_MESSAGE,"pickup",0},
	{SEQ_ATTACKANDFACE,"snowball2",0},
	{SEQ_MESSAGE,"throw",0},
	{SEQ_ATTACKANDFACE,"snowball3",0},
	{SEQ_SETBLENDTIME,NULL,0.2f},
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},

	{SEQ_PLAY,"idle4",0},
	{SEQ_RETURN,"walk1",0},
	{SEQ_END,NULL,0},
};

static SEQ brownOutOfRange[] =
{
	{SEQ_RETURN,"walk1",0},
	{SEQ_END,NULL,0},
};

static SEQ biteFront[] =
{
	{SEQ_PLAY,"react6",0},
	{SEQ_MOTIONBONE,NULL,0},
	{SEQ_FACETAZ,"walk1",0},
	{SEQ_MOTIONBONE,NULL,1},
	{SEQ_ATTACK,"attack1",0},
	{SEQ_END,NULL,0},
};

static SEQ brownBiteRear[] =
{
	{SEQ_PLAY,"react3",0},
	{SEQ_STUNNED,"react2",3.0f},
	{SEQ_END,NULL,0},
};

static SEQ brownBiteFront[] =
{
	{SEQ_ATTACK,"attack2",0},
	{SEQ_END,NULL,0},
};

static SEQ biteRear[] =
{
	{SEQ_PLAYSAMPLE,"pbbitten.wav",false},
	{SEQ_PLAY,"react3",0},
	{SEQ_STUNNED,"react2",3.0f},
	{SEQ_END,NULL,0},
};

static SEQ spinFrontAttack[] =
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAYSAMPLE,"spring.wav",false},
	{SEQ_ATTACK,"react9",0},
	{SEQ_END,NULL,0},
};

static SEQ spinFront[] =
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAYSAMPLE,"pbspininto.wav",false},
	{SEQ_PLAY,"react7",0},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_DISABLED,NULL,GENERICAI_KNOCKEDOUTTIME},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAY,"idle7",0},
	{SEQ_END,NULL,0},
};

static SEQ brownSpinFront[] =
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAYSAMPLE,"bsmack.wav",false},
	{SEQ_ATTACK,"attack5",0},
	{SEQ_END,NULL,0},
};

static SEQ spinRear[] =
{
	{SEQ_LOOKAWAYFROMTAZ,NULL,0},
	{SEQ_PLAYSAMPLE,"pbspininto.wav",false},
	{SEQ_PLAY,"react7",0},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_DISABLED,NULL,GENERICAI_KNOCKEDOUTTIME},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAY,"idle7",0},
	{SEQ_END,NULL,0},
};

static SEQ attackedOnGround[] = 
{
	{SEQ_PLAYSAMPLE,"bspininto.wav",false},
	{SEQ_PLAY,"react7",0},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_DISABLED,NULL,10.0f},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAY,"idle10",0},
	{SEQ_END,NULL,0},
};

static SEQ brownSpinRear[] = 
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAYSAMPLE,"bsmack.wav",false},
	{SEQ_ATTACK,"attack5",0},
	{SEQ_END,NULL,0},
};

static SEQ brownScareFront[] =
{
	{SEQ_ATTACK,"attack3",0},
	{SEQ_END,NULL,0},
};

static SEQ scareFront[] = 
{
	{SEQ_PLAY,"react6",0},
	{SEQ_MOTIONBONE,NULL,0},
	{SEQ_FACETAZ,"walk1",0},
	{SEQ_MOTIONBONE,NULL,1},
	{SEQ_ATTACK,"attack2",0},
	{SEQ_END,NULL,0},
};

static SEQ scareRear[] =
{
	{SEQ_PLAYSAMPLE,"pbscared.wav",false},
	{SEQ_PLAY,"react4",0},
	{SEQ_STUNNED,"react2",3.0f},
	{SEQ_END,NULL,0},
};

static SEQ puzzled[] = 
{
	{SEQ_MOTIONBONE,NULL,0},
	{SEQ_FACETAZ,"walk1",0},
	{SEQ_MOTIONBONE,NULL,1},
	{SEQ_STARTQUESTION,NULL,0},
	{SEQ_PLAY,"disguise",0},
	{SEQ_COSTUMEDONE,NULL,true},
	{SEQ_ENDQUESTION,NULL,0},
	{SEQ_MOTIONBONE,NULL,0},
	{SEQ_FACETAZ,"walk1",0},
	{SEQ_MOTIONBONE,NULL,1},
	{SEQ_PLAY,"react6",0},
//	{SEQ_CHASE,"walk2",0},
//	{SEQ_ATTACK,"attack2",0},
	{SEQ_END,NULL,0},
};

/* --------------------------------------------------------------------------------
   Function : BearIdleSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int BearIdleSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	BEARINFO	*bearInfo = (BEARINFO*)actorInstance->genericAI->customData;
	
	if (ReturnAttachedObjectInstance(actorInstance,0))
	{
		RemoveObjectFromCharacter(actorInstance,0);
	}

	if (bearInfo->type==POLARBEAR)
	{
		actorInstance->genericAI->currentSeq = idle;
	}
	else
	{
		actorInstance->genericAI->currentSeq = brownIdle;
	}

	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : BearAwareSeq
   Purpose : Respond to requests for aware seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int BearAwareSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	BEARINFO	*bearInfo = (BEARINFO*)actorInstance->genericAI->customData;

	if (ReturnAttachedObjectInstance(actorInstance,0))
	{
		RemoveObjectFromCharacter(actorInstance,0);
	}

	if (bearInfo->type==POLARBEAR)
	{
		actorInstance->genericAI->currentSeq = aware;
	}
	else
	{
		actorInstance->genericAI->currentSeq = brownAware;
	}
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : BearAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int BearAttackSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	BEARINFO	*bearInfo = (BEARINFO*)actorInstance->genericAI->customData;

	if (ReturnAttachedObjectInstance(actorInstance,0))
	{
		RemoveObjectFromCharacter(actorInstance,0);
	}
	if (bearInfo->type==POLARBEAR)
	{
		actorInstance->genericAI->currentSeq = attack;
	}
	else
	{
		actorInstance->genericAI->currentSeq = brownAttack;
	}
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : BearDefendSeq
   Purpose : Respond to requests for defend seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int BearDefendSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	if (ReturnAttachedObjectInstance(actorInstance,0))
	{
		RemoveObjectFromCharacter(actorInstance,0);
	}
	actorInstance->genericAI->currentSeq = defend;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : BearAttackedSeq
   Purpose : Respond to requests for attacked seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int BearAttackedSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	int			random;
	GENERICAI *genericAI = actorInstance->genericAI;
	BEARINFO	*bearInfo = (BEARINFO*)genericAI->customData;

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
	if (ReturnAttachedObjectInstance(actorInstance,0))
	{
		RemoveObjectFromCharacter(actorInstance,0);
	}
	switch(actorInstance->genericAI->typeOfAttack)
	{
	case ATTACKTYPE_BITEFRONT:
		if (bearInfo->type==POLARBEAR)
		{
			actorInstance->genericAI->currentSeq = biteFront;
		}
		else
		{
			if (actorInstance->genericAI->currentSeq[actorInstance->genericAI->seqIdx].type==SEQ_STUNNED)
			{
				actorInstance->genericAI->currentSeq = attackedOnGround;
			}
			else
			{
				actorInstance->genericAI->currentSeq = brownBiteFront;
			}
		}
		break;
	case ATTACKTYPE_BITEREAR:
		if (bearInfo->type==POLARBEAR)
		{
			actorInstance->genericAI->currentSeq = biteRear;
		}
		else
		{
			if (actorInstance->genericAI->currentSeq[actorInstance->genericAI->seqIdx].type==SEQ_STUNNED)
			{
				actorInstance->genericAI->currentSeq = attackedOnGround;
			}
			else
			{
				actorInstance->genericAI->currentSeq = brownBiteRear;
			}
		}
		break;
	case ATTACKTYPE_SPINFRONT:
	case ATTACKTYPE_SPITFRONT:
	case ATTACKTYPE_COSTUMEFRONT:
		if (genericAI->currentSeq[genericAI->seqIdx].type==SEQ_STUNNED)
		{
			// bear is already stunned
			if (bearInfo->type==POLARBEAR)
			{
				actorInstance->genericAI->currentSeq = spinRear;
			}
			else
			{
				if (actorInstance->genericAI->currentSeq[actorInstance->genericAI->seqIdx].type==SEQ_STUNNED)
				{
					actorInstance->genericAI->currentSeq = attackedOnGround;
				}
				else
				{
					actorInstance->genericAI->currentSeq = brownSpinRear;
				}
			}
		}
		else
		{
			if (bearInfo->type==POLARBEAR)
			{
				if (actorInstance->genericAI->typeOfAttack==ATTACKTYPE_SPINFRONT)
				{
					actorInstance->genericAI->currentSeq = spinFrontAttack;
				}
				else
				{
					actorInstance->genericAI->currentSeq = spinFront;
				}
			}
			else
			{
				actorInstance->genericAI->currentSeq = spinRear;//brownSpinFront;
			}
		}
		break;
	case ATTACKTYPE_SPINREAR:
	case ATTACKTYPE_SPITREAR:
	case ATTACKTYPE_COSTUMEREAR:
		if (bearInfo->type==POLARBEAR)
		{
			actorInstance->genericAI->currentSeq = spinRear;
		}
		else
		{
			if (actorInstance->genericAI->currentSeq[actorInstance->genericAI->seqIdx].type==SEQ_STUNNED)
			{
				actorInstance->genericAI->currentSeq = attackedOnGround;
			}
			else
			{
				actorInstance->genericAI->currentSeq = spinRear;
			}
		}
		break;
	case ATTACKTYPE_SCAREFRONT:
		if (bearInfo->type==POLARBEAR)
		{
			actorInstance->genericAI->currentSeq = scareFront;
		}
		else
		{
			actorInstance->genericAI->currentSeq = brownScareFront;
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
   Function : BearOutOfRangeSeq
   Purpose : Respond to requests for out of range seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int BearOutOfRangeSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	GENERICAI *genericAI = actorInstance->genericAI;
	BEARINFO	*bearInfo = (BEARINFO*)genericAI->customData;

	if (ReturnAttachedObjectInstance(actorInstance,0))
	{
		RemoveObjectFromCharacter(actorInstance,0);
	}
	if (bearInfo->type==POLARBEAR)
	{
		actorInstance->genericAI->currentSeq = outOfRange;
	}
	else
	{
		actorInstance->genericAI->currentSeq = brownOutOfRange;
	}
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
	Function	: BearPuzzledSeq
	Purpose		: Respond to requests for costume seq
	Parameters	: actorInstance
	Returns		: seq to follow
	Info		: 
*/
int BearPuzzledSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	if (ReturnAttachedObjectInstance(actorInstance,0))
	{
		RemoveObjectFromCharacter(actorInstance,0);
	}
	actorInstance->genericAI->currentSeq = puzzled;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/*  --------------------------------------------------------------------------------
	Function	: PolarBearSlap
	Purpose		: Called when the bear does a slap move
	Parameters	: actorInstance of bear.
	Returns		: 
	Info		: 
*/
void PolarBearHit(ACTORINSTANCE *actorInstance, int type)
{
	char	*bone;

	switch(type)
	{
	case 0:
		if (CheckIfPlayingAnimation(actorInstance,"attack1"))
		{
			bone = "armlfinger";
		}
		else
		{
			bone = "snowbone";
		}
		if (gameStatus.player1->actorStatus->currentState != STATE_PLAYANIMATION)
		{
			if (DistFromNodeToTaz(actorInstance,bone) < METERTOUNIT(1.75f))
			{
				if (EnterState(gameStatus.player1,STATE_PLAYANIMATION))
				{
					PlaySample("pbsmackhit.wav",255);
					FlushAnimationQueue(gameStatus.player1);
					PlayAnimationByName(gameStatus.player1,"react2",1.0f,0,0,0.1f,NONE);

					// TP: Turn taz to face character
					FacePoint(gameStatus.player1,actorInstance->actorInstance.position,0.0f,0.0f);

					RequestRumbleEffect(gameStatus.player[0].controller, 220, 0.0f, 0.5f, 0.75f, 0.0f, WAVEFORM_NORMAL);
				}
			}
		}
		break;
		
	case 1:
		bone = "root";
	
		if (DistFromNodeToTaz(actorInstance,bone) < METERTOUNIT(1.75f))
		{
			if (EnterState(gameStatus.player1, STATE_PROJECTILESLIDE))
			{
				FacePoint(gameStatus.player1, actorInstance->actorInstance.position, 0.0f, RAD(180.0f));
				StopCollidingWith(gameStatus.player1,actorInstance,0.2f);
				myStateChangeCallback(gameStatus.player1);
			}
		}
		break;
	}
	
}

/*  --------------------------------------------------------------------------------
	Function	: BrownBearHit
	Purpose		: Called when the bear does a hit move
	Parameters	: actorInstance of bear, type of hit
	Returns		: 
	Info		: 
*/
void BrownBearHit(ACTORINSTANCE *actorInstance, int type)
{
	char	*bone;

	switch(type)
	{
	case 1:
	case 3:
	case 4:
		bone = "armlfinger";
		break;
	case 2:
	case 5:
		bone = "snowbone";
		break;
	}


	if (DistFromNodeToTaz(actorInstance,bone) < METERTOUNIT(0.75f))
	{
		if ((type==3)||(type==4)||(type==5))
		{
			if (EnterState(gameStatus.player1,STATE_PLAYANIMATION))
			{
				PlaySample("bsmackhit.wav",255);
				FlushAnimationQueue(gameStatus.player1);
				PlayAnimationByName(gameStatus.player1,"react2",1.0f,0,0,0.1f,NONE);

				// TP: Turn taz to face character
				FacePoint(gameStatus.player1,actorInstance->actorInstance.position,0.0f,0.0f);

				RequestRumbleEffect(gameStatus.player[0].controller, 220, 0.0f, 0.5f, 0.75f, 0.0f, WAVEFORM_NORMAL);
			}
		}
		else
		{
			if (EnterState(gameStatus.player1,STATE_PLAYANIMATION))
			{
				PlaySample("bpunch1.wav",255);
				FlushAnimationQueue(gameStatus.player1);
				PlayAnimationByName(gameStatus.player1,"react1",1.0f,0,0,0.1f,NONE);

				// TP: Turn taz to face character
				FacePoint(gameStatus.player1,actorInstance->actorInstance.position,0.0f,0.0f);
				RequestRumbleEffect(gameStatus.player[0].controller, 200, 0.0f, 0.25f, 0.5f, 0.0f, WAVEFORM_NORMAL);
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : BearAttackCallback
   Purpose : Called each frame during an attack 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
int BearAttackCallback(struct TAG_ACTORINSTANCE *actorInstance)
{

	return FALSE;	// continue processing pattern
}

/* --------------------------------------------------------------------------------
   Function : SnowballCollision
   Purpose : Called when snowball collides
   Parameters : ACTORINSTANCE *, 
   Returns : 
   Info : 
*/
static int PreSnowballCollision(ACTORINSTANCE *collider,ACTORINSTANCE *objectHit, struct TAG_PRECOLLINFO *preCollInfo)
{
	if ((collider==gameStatus.player1)||(objectHit==gameStatus.player1))
	{
		return FALSE;
	}
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : SnowballCollision
   Purpose : Called when snowball collides
   Parameters : ACTORINSTANCE *, 
   Returns : 
   Info : 
*/
int SnowballCollision(ACTORINSTANCE *collider,ACTORINSTANCE *objectHit)
{
	// dont know which is which so try and destroy both
	collider->flags |= ACTORFLAG_DESTROYNEXTUPDATE;
	objectHit->flags |= ACTORFLAG_DESTROYNEXTUPDATE;

	if ((collider==gameStatus.player1)||(objectHit==gameStatus.player1))
	{
		if (EnterState(gameStatus.player1,STATE_PLAYANIMATION))
		{
			PlaySample("pbsbhittaz.wav",255);
			FlushAnimationQueue(gameStatus.player1);
			PlayAnimationByName(gameStatus.player1,"react1",1.0f,0,0,0.1f,NONE);
			ReduceCharactersHealth(gameStatus.player1,1.0f);

			RequestRumbleEffect(gameStatus.player[0].controller, 220, 0.0f, 0.5f, 0.75f, 0.0f, WAVEFORM_NORMAL);
		}
	}

	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : BearMessageHandler
   Purpose : Called when a message is sent in the sequence
   Parameters : ACTORINSTANCE *, char *message
   Returns : 
   Info : 
*/
int BearMessageHandler(ACTORINSTANCE *actorInstance, char *message)
{
	TBVector	tempVector,dist;
	TBVector	pos,scale;
	ACTORINSTANCE *tempInstance;

	// TP: Polar bear
	if (strcmpi(message,"polarbearslap")==0)
	{
		PolarBearHit(actorInstance,0);
		return true;
	}
	if (strcmpi(message,"eatbelly")==0)
	{
		PolarBearHit(actorInstance,1);
		return true;
	}

	// TP: Brown bear
	if (strcmpi(message,"bbbelly")==0)
	{
		PolarBearHit(actorInstance,1);
		return true;
	}
	if (strcmpi(message,"bbbox1")==0)
	{
		BrownBearHit(actorInstance,1);
		return true;
	}
	if (strcmpi(message,"bbbox2")==0)
	{
		BrownBearHit(actorInstance,2);
		return true;
	}
	if (strcmpi(message,"bbbox3")==0)
	{
		BrownBearHit(actorInstance,3);
		return true;
	}
	if (strcmpi(message,"bbslap2")==0)
	{
		BrownBearHit(actorInstance,4);
		return true;
	}
	if (strcmpi(message,"bbbonk")==0)
	{
		BrownBearHit(actorInstance,5);
		return true;
	}

	if (strcmpi(message,"rand")==0)
	{
		if ((bmRand()%3)==1) return true;
		else return false;
	}

	if (strcmpi(message,"pickup")==0)
	{
		SETVECTOR(pos,METERTOUNIT(-0.05f),METERTOUNIT(-0.2f),0.0f,1.0f);
		SETVECTOR(scale,1.0f,1.0f,1.0f,1.0f);
		AttachObjectToCharacter(actorInstance,"snowball.obe","snowbone",pos,scale,bIdentityQuaternion,0);
	} 
	if (strcmpi(message,"throw")==0)
	{
		// TP: get position to aim for
		bmVectorCopy(pos,gameStatus.player1->actorInstance.position);
		pos[Y] += METERTOUNIT(0.5f);

		// TP: get current position of snowball
		baGetNodesWorldPosition(&actorInstance->actorInstance, baFindNode(actorInstance->actorInstance.rootNodeInstance,"snowbone"), tempVector);

		// TP: remove snowball from character
		RemoveObjectFromCharacter(actorInstance,0);

		// TP: can only throw snowball a max of 10 meters
		bmVectorSub(dist,gameStatus.player1->actorInstance.position, actorInstance->actorInstance.position);
		if (bmVectorLen(dist) > METERTOUNIT(10.0f))
		{
			bmVectorScaleToLength(dist,dist,METERTOUNIT(10.0f));
			bmVectorAdd(pos,actorInstance->actorInstance.position,dist);
		}

		tempInstance = CreateSnowball(actorInstance, tempVector, tempVector, METERTOUNIT(0.5f), 
			pos,SNOWBALL_MEDIUM,"snowball.obe")->actorStructure;

		if (tempInstance) 
		{
			SetPreCollisionCallback(tempInstance,PreSnowballCollision);
			SetCollisionCallback(tempInstance,SnowballCollision);
			StopCollidingWith(tempInstance, actorInstance);
			StopCollidingWith(actorInstance, tempInstance);
		}
	}
	return false;
}

/*  --------------------------------------------------------------------------------
	Function	: BearChangeStateCallback
	Purpose		: Called when the bears change from one state to another
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
static void BearChangeStateCallback(ACTORINSTANCE *actorInstance, int state, int lastState)
{
	GENERICAI *genericAI = actorInstance->genericAI;
	
	genericAI->currentBlendTime = 0.2f;
}

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceBear
   Purpose : Setup the basic ai for a bear
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceBear(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList,int type)
{
	GENERICAI	*genericAI;
	TBVector	tempVector;
	BEARINFO	*bearInfo;

	// create extra structures
	CreateActorInstanceBehaviour(actorInstance);
	CreateActorInstanceAnimation(actorInstance);
	CreateActorInstanceStatus(actorInstance);
	//CreateActorInstanceShadow(actorInstance,METERTOUNIT(1.0f),METERTOUNIT(2.5f), SHADOWMAP_RADIUS_BEAR);
	CreateActorInstanceCharacterInfo(actorInstance);
	SetActorInertia(actorInstance->actorBehaviour,0);
	SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
	SetActorMaxSpeed(actorInstance->actorBehaviour,METERTOUNIT(9)); // set max speed 8mps
	SETVECTOR(tempVector,0.0f,METERTOUNIT(1.0f),0.0f,1.0f);
	
	SetActorCylinder(0,actorInstance->actorBehaviour,tempVector,METERTOUNIT(0.7f),METERTOUNIT(2.0f));
	AddActorToCollisionCylinderCache(&collisionCache,actorInstance);
	
	TBVector offset = {0.0f,METERTOUNIT(0.45f),0.0f,1.0f};
	SetActorCollSphere(actorInstance->actorBehaviour, offset, METERTOUNIT(0.5f));

	// create ai structure
	if (CreateActorInstanceGenericAI(actorInstance,activeList,inactiveList)==FALSE) return FALSE;

	AISetShadow(actorInstance,METERTOUNIT(1.0f),METERTOUNIT(2.5f), SHADOWMAP_RADIUS_BEAR);

	// fill in custom parms
	genericAI = actorInstance->genericAI;
	bmVectorCopy(genericAI->center,actorInstance->actorInstance.position);
	if (type==0)
	{
		genericAI->deactivationRange = METERTOUNIT(12.0f);
		genericAI->awarenessRange = METERTOUNIT(10.0f);
		genericAI->movementRange = METERTOUNIT(2.0f);
		genericAI->standOffRange = METERTOUNIT(1.3f);
	}
	else
	{
		genericAI->deactivationRange = METERTOUNIT(12.0f);
		genericAI->awarenessRange = METERTOUNIT(6.0f);
		genericAI->movementRange = METERTOUNIT(2.0f);
		genericAI->standOffRange = METERTOUNIT(1.3f);
	}
	genericAI->viewAngle = PI/3;

	genericAI->headNode = "brow";
	SETVECTOR(genericAI->starsOffset,0.0f,METERTOUNIT(0.5f),0.0f,1.0f);
	SETVECTOR(genericAI->starVars,METERTOUNIT(0.3f),METERTOUNIT(0.4f),METERTOUNIT(0.0f),METERTOUNIT(0.0f));

	SetAISeqFunctions(actorInstance,BearIdleSeq,BearAwareSeq,BearAttackSeq,
		BearDefendSeq,BearAttackedSeq,BearOutOfRangeSeq,BearPuzzledSeq);

	SetAIAttackCallback(actorInstance,BearAttackCallback);
	SetAIMessageHandler(actorInstance,BearMessageHandler);
	genericAI->changeStateCallback = BearChangeStateCallback;

	genericAI->flags |= GENERICAI_IGNORESPECIALCOLL;

	genericAI->customData = (void*)MALLOC(sizeof(BEARINFO));
	genericAI->instantAttack = true;
	genericAI->alwaysClipPosition = true;

	bearInfo = (BEARINFO*)genericAI->customData;
	bearInfo->type = type;

	return TRUE;
}

