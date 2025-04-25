// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : shark.cpp
//   Purpose : ai functions for shark
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "main.h"
#include "actors.h"
#include "genericai.h"
#include "maths.h"
#include "physics.h"
#include "status.h"
#include "clouds.h"
#include "characters.h"
#include "scene.h"
#include "collisioncache.h"
#include "animation.h"
#include "tazcatcher.h"
#include "shark.h"
#include "sfx.h"
#include "timer.h"
#include "projectile.h"
#include "videofx.h"

static SEQ patrol[] =
{
	// TP: head to point 0
	{SEQ_LOCKSEQ, NULL, TRUE},
	{SEQ_MESSAGE, "facepoint1", 0},
	{SEQ_PLAY, "turn", 0},
	{SEQ_LOCKSEQ, NULL, FALSE},
	{SEQ_MESSAGE, "headpoint1", 0},
	// TP: turn
	{SEQ_LOCKSEQ, NULL, TRUE},
	{SEQ_MESSAGE, "facepoint2", 0},
	{SEQ_PLAY, "turn", 0},
	{SEQ_LOCKSEQ, NULL, FALSE},
	{SEQ_MESSAGE, "headpoint2", 0},
	// TP: turn
	{SEQ_END, NULL, 0},
};

static SEQ attack[] =
{
	{SEQ_NODEACTIVATE, NULL, TRUE},
	{SEQ_LOCKSEQ, NULL, TRUE},
	{SEQ_PLAYSAMPLE, "sharkattack.wav", FALSE},
	{SEQ_MESSAGE, "attack1", 0},
	{SEQ_LOCKSEQ, NULL, FALSE},
	{SEQ_NODEACTIVATE, NULL, FALSE},
	{SEQ_END, NULL, 0},
};

static SEQ idle[] =
{
	{SEQ_END, NULL, 0},
};

static SEQ costume[] =
{
	// TP: head to point 0
	//{SEQ_LOCKSEQ, NULL, TRUE},
	//{SEQ_MESSAGE, "facepoint1", 0},
	//{SEQ_LOCKSEQ, NULL, FALSE},
	{SEQ_MESSAGE, "headpoint1", 0},
	
	// TP: turn
	{SEQ_LOCKSEQ, NULL, TRUE},
	{SEQ_PLAY, "idle3", 0},
	{SEQ_MESSAGE, "headpoint2cruise", 0},
	{SEQ_PLAY, "idle4", 0},
	{SEQ_LOCKSEQ, NULL, FALSE},
	// TP: turn
	{SEQ_END, NULL, 0},
};



/* --------------------------------------------------------------------------------
   Function : SharkIdleSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/

int SharkIdleSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = patrol;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;
	return TRUE;
}


/* --------------------------------------------------------------------------------
   Function : SharkAwareSeq
   Purpose : Respond to requests for aware seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/

int SharkAwareSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = costume;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;
	return TRUE;
}


/* --------------------------------------------------------------------------------
   Function : SharkAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/

int SharkAttackSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = attack;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;
	return TRUE;
}


/* --------------------------------------------------------------------------------
   Function : SharkTooCloseSeq
   Purpose : Respond to requests for defend seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/

int SharkTooCloseSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = attack;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}


/* --------------------------------------------------------------------------------
   Function : SharkCostumeSeq
   Purpose : Respond to requests for defend seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/

int SharkCostumeSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = costume;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}



/* --------------------------------------------------------------------------------
   Function : SharkAttackedSeq
   Purpose : Respond to requests for attacked seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int SharkAttackedSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = idle;
	switch(actorInstance->genericAI->typeOfAttack)
	{
	case ATTACKTYPE_BITEFRONT:
//		actorInstance->genericAI->currentSeq = scareFront;
		break;
	case ATTACKTYPE_BITEREAR:
//		actorInstance->genericAI->currentSeq = swimIdle;
		break;
	case ATTACKTYPE_SPINFRONT:
//		actorInstance->genericAI->currentSeq = swimIdle;
		break;
	case ATTACKTYPE_SPINREAR:
//		actorInstance->genericAI->currentSeq = scareFront;
		break;
	case ATTACKTYPE_SCAREFRONT:
//		actorInstance->genericAI->currentSeq = swimIdle;
		break;
	case ATTACKTYPE_SCAREREAR:
//		actorInstance->genericAI->currentSeq = swimIdle;
		break;
	}
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : SharkOutOfRangeSeq
   Purpose : Respond to requests for out of range seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int SharkOutOfRangeSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
//	actorInstance->genericAI->currentSeq = outOfRange;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

static void TrappedStateChange(ACTORINSTANCE *actorInstance)
{
	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance,"trapped",1.0f,1,0,0.1f,MOVE);
}

static void EnterBounceCallback(ACTORINSTANCE *actorInstance)
{
	TBVector				yAxis = {0.0f, 1.0f, 0.0f, 1.0f};

	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance, "waterbounce", 1.0f, 1, 0, 0.1f, MOVE);
	actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_NOMOVE;
	actorInstance->actorBehaviour->physics.velocity[1] = 0.0f;
	AddActorSpeed(actorInstance->actorBehaviour, yAxis, METERTOUNIT(11.0f));

	actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;
	actorInstance->actorAnimation->useMotionBone = FALSE;
}


/* --------------------------------------------------------------------------------
   Function : SharkSpitTaz
   Purpose : Called to spit taz out of shark
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void SharkSpitTaz(ACTORINSTANCE *actorInstance, TBVector resetPoint = NULL)
{
	TBVector	tempVector;

	//SetAIAttackCallback(actorInstance,SharkAttackCallback);

	if (RequestStateChange(gameStatus.player1, STATE_PROJECTILE))
	{
		if (UpdateCharacterState(&gameStatus.player[0], true))
		{
			TBVector				yAxis = {0.0f, 1.0f, 0.0f, 1.0f};

			if (!resetPoint)
			{
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance, "waterbounce", 1.0f, 1, 0, 0.1f, MOVE);
				actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_NOMOVE;
				actorInstance->actorBehaviour->physics.velocity[1] = 0.0f;
				AddActorSpeed(actorInstance->actorBehaviour, yAxis, METERTOUNIT(11.0f));

				actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;
				actorInstance->actorAnimation->useMotionBone = FALSE;
			}
			else
			{
				// TP: project towards the reset point
				FlushAnimationQueue(gameStatus.player1);
				if((bmRand()%4) == 0)
				{
					PlayAnimationByName(gameStatus.player1, "fly2", 1.0f, TRUE, 0, 0.1f, NONE);
				}
				else
				{
					PlayAnimationByName(gameStatus.player1, "fly1", 1.0f, TRUE, 0, 0.1f, NONE);
				}
				gameStatus.player1->actorAnimation->useMotionBone = FALSE;

				CalculateProjectileInitialVelocity(tempVector, gameStatus.player1->actorInstance.position, resetPoint, RAD(60.0f));

				SetActorVelocity(gameStatus.player1->actorBehaviour, tempVector);

				RequestRumbleEffect(gameStatus.player[0].controller, 180, 0.0f, 0.5f, 0.75f, 0.0f, WAVEFORM_NORMAL);

				StopCollidingWith(gameStatus.player1, actorInstance);
				StopCollidingWith(actorInstance, gameStatus.player1);
			}
		}
		else
		{
			gameStatus.player1->actorStatus->stateRequest = (ECharState)-1;
			gameStatus.player1->actorStatus->stateRequestCallback = NULL;

			// TP: snap taz to reset point
			bmVectorCopy(gameStatus.player1->actorInstance.position, resetPoint);
		}
	}
	bmVectorSet(gameStatus.player1->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
}


/* --------------------------------------------------------------------------------
   Function : SharkTazTrappedCallback
   Purpose : Called each frame while taz trapped
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int SharkTazTrappedCallback(struct TAG_ACTORINSTANCE *actorInstance)
{
	TBActorNodeInstance *node;
	TBVector	tempVector;

	// TP: Check if player has escaped the trapped state somehow
	if (gameStatus.player1->actorStatus->currentState!=STATE_TRAPPED)
	{
		bmVectorSet(gameStatus.player1->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
		//SetAIAttackCallback(actorInstance,SharkAttackCallback);
	}

	// TP: Check for collision between shark jaw and taz
	if(node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "jaw"))
	{
		baGetNodesWorldPosition(&actorInstance->actorInstance, node, tempVector);
	}
	else
	{
		return TRUE;
	}

	// TP: Hold taz in position
	bmVectorCopy(gameStatus.player1->actorInstance.position,tempVector);

	return TRUE;
}

/*  --------------------------------------------------------------------------------
	Function	: SharkBite
	Purpose		: Called when the shark sends a bite message
	Parameters	: actorInstance for shark
	Returns		: 
	Info		: 
*/
void SharkBite(ACTORINSTANCE *actorInstance)
{
	if (DistFromNodeToTaz(actorInstance,"jaw") < METERTOUNIT(1.0f))
	{
		if (!RequestStateChange(gameStatus.player1,STATE_TRAPPED)) 
		{
			FlushAnimationQueue(actorInstance);
			PlayAnimationByName(actorInstance, "missed",1.0f,0,0,0.0f,MOVE);
			return;
		}
		if (!UpdateCharacterState(&gameStatus.player[0], true)) 
		{
			FlushAnimationQueue(actorInstance);
			PlayAnimationByName(actorInstance, "missed",1.0f,0,0,0.0f,MOVE);
			gameStatus.player1->actorStatus->stateRequest = (ECharState)-1;
			gameStatus.player1->actorStatus->stateRequestCallback = NULL;
			return;
		}

		FlushAnimationQueue(actorInstance);
		PlayAnimationByName(actorInstance, "swallow",1.0f,0,0,0.0f,MOVE);

		// sort out taz
		bmVectorSet(gameStatus.player1->actorInstance.scale, 0.0f, 0.0f, 0.0f, 1.0f);
		//SetAIAttackCallback(actorInstance,SharkTazTrappedCallback);
	}
	else
	{
		FlushAnimationQueue(actorInstance);
		PlayAnimationByName(actorInstance, "missed",1.0f,0,0,0.0f,MOVE);
	}
}

/* --------------------------------------------------------------------------------
   Function : SharkAttackCallback
   Purpose : Called each frame during an attack 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
int SharkAttackCallback(struct TAG_ACTORINSTANCE *actorInstance)
{
	TBVector		tempVector,tempVector2;
	TBVector		starsBounds = {METERTOUNIT(0.3f), METERTOUNIT(0.4f), 0.0f, 1.0f};
	TBVector		starsOffset = {0.0f, METERTOUNIT(0.2f), 0.0f, 1.0f};
	TBActorNodeInstance *node;
	TAZCATCHERINFO	*sharkInfo = (TAZCATCHERINFO*)actorInstance->genericAI->customData;
	
	// TP: Check for collision between shark jaw and taz
	if(node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "jaw"))
	{
		baGetNodesWorldPosition(&actorInstance->actorInstance, node, tempVector);
	}
	else
	{
		return TRUE;
	}

	bmVectorAdd(tempVector2,gameStatus.player1->actorInstance.position,gameStatus.player1->actorBehaviour->collSphere.offset);
	bmVectorSub(tempVector,tempVector,tempVector2);

	if (bmVectorLen(tempVector) < (gameStatus.player1->actorBehaviour->collSphere.radius+METERTOUNIT(1.0f)))
	{
		// TP: have collided
/*		if (!CheckIfPlayingAnimation(actorInstance,"attack5"))
		{
			ThrowTazOff(actorInstance);

			SetAIAttackCallback(actorInstance,SharkTazSquashedCallback);
			sharkInfo->timeSinceLastAttack = 0.0f;
		}
		return TRUE;*/	// don't continue pattern
	}
	
	return FALSE;	// continue processing pattern
}

/* --------------------------------------------------------------------------------
   Function : SharkStateChangeCallback
   Purpose : Called when state changed 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void SharkStateChangeCallback(ACTORINSTANCE *actorInstance, int state, int lastState)
{
	switch(lastState)
	{
	case AISTATE_IDLE:
		DestroyInjuryStars(actorInstance);
		break;
	
	case AISTATE_AWARE:
		PlayAnimationByName(actorInstance, "swim5", 1.0f, 0, 1, 0.1f, IDLE);
		break;
	
	case AISTATE_ATTACK:
	case AISTATE_TOOCLOSE:

		actorInstance->genericAI->currentAnimSpeed = 1.0f;
		break;
	}

	switch(state)
	{
	case AISTATE_AWARE:
		FlushAnimationQueue(actorInstance);
		PlayAnimationByName(actorInstance, "swim3", 1.0f, 0, 0, 0.1f, IDLE);
		break;
	}
}

/* --------------------------------------------------------------------------------
   Function : SharkTazSquashedCallback
   Purpose : Called when state changed 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int SharkTazSquashedCallback(ACTORINSTANCE *actorInstance)
{
	TAZCATCHERINFO	*sharkInfo = (TAZCATCHERINFO*)actorInstance->genericAI->customData;

	if((sharkInfo->timeSinceLastAttack += fTime) > 2.0f)
	{
		//SetAIAttackCallback(actorInstance,SharkAttackCallback);

		// sort out taz catcher
		FlushAnimationQueue(actorInstance);

		ResetCharacterAfterJump(actorInstance);

		actorInstance->genericAI->flags &= ~GENERICAI_DONTCHANGESTATE;
		return FALSE;
	}
	return TRUE;
}


enum ESharkState
{
	SHARKSTATE_STARTATTACK,
	SHARKSTATE_FLY,
	SHARKSTATE_LAND,
	SHARKSTATE_GOTTAZ,
	SHARKSTATE_MISSEDTAZ,
};


/*  --------------------------------------------------------------------------------
	Function	: CollideWithSpecial
	Purpose		: Called if the shark colliders with a special object
	Parameters	: ACTORINSTANCE of special object
	Returns		: 
	Info		: 
*/

static void CollideWithSpecial(ACTORINSTANCE *shark, ACTORINSTANCE *special)
{
	GENERICAI	*genericAI = shark->genericAI;
	SHARKINFO	*sharkInfo = (SHARKINFO*)genericAI->customData;

	if (special->special == SPECIALTYPE_WATERYOBJECT)
	{
		sharkInfo->myWater = special;		
	}
}


/*  --------------------------------------------------------------------------------
	Function	: HoldTazInShark
	Purpose		: Called each frame to keep taz hidden and inside the shark
	Parameters	: actorInstance of shark
	Returns		: 
	Info		: 
*/

void HoldTazInShark(ACTORINSTANCE *actorInstance)
{
	TBActorNodeInstance	*node;
	TBVector	tempVector;

	// TP: make sure taz is invisible

	// TP: hold taz in correct position
	if(node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "jaw"))
	{
		baGetNodesWorldPosition(&actorInstance->actorInstance, node, tempVector);
	}
	else
	{
		return;
	}

	// TP: Hold taz in position
	bmVectorCopy(gameStatus.player1->actorInstance.position, tempVector);
	bmVectorZero(gameStatus.player1->actorBehaviour->physics.velocity);
}


static float	stime = 0.8f;
/*  --------------------------------------------------------------------------------
	Function	: SharkFlyingUpdate
	Purpose		: Called during the fly move to follow path
	Parameters	: ACTORINSTANCE 
	Returns		: 
	Info		: 
*/

bool SharkFlyingUpdate(ACTORINSTANCE *actorInstance)
{
	GENERICAI	*genericAI = actorInstance->genericAI;
	SHARKINFO	*sharkInfo = (SHARKINFO*)genericAI->customData;
	TBActorNodeInstance	*node;
	TBVector	tempVector;

	sharkInfo->jumpT += fTime/(stime);

	if (sharkInfo->jumpT > (1.0f - (0.1f/stime)))
	{
		if (!CheckIfPlayingAnimation(actorInstance, "newattack3")) 
		{
			PlayAnimationByName(actorInstance, "newattack3", 1.0f, 0, 0, 0.1f, MOVE);
			TimeFX_setGlobalRate(1.0f);
		}
	}

	if ((!sharkInfo->gotTaz)&&(gameStatus.player1->actorInstance.position[Y] > (genericAI->center[Y] + METERTOUNIT(0.5f))))
	{
		if (gameStatus.player1->actorInstance.position[Y] < (genericAI->center[Y] + METERTOUNIT(3.0f)))
		{
			sharkInfo->p1[Y] = gameStatus.player1->actorInstance.position[Y];
			sharkInfo->p2[Y] = gameStatus.player1->actorInstance.position[Y];
		}
	}

	// TP: test for collision between shark and taz
	if (!sharkInfo->gotTaz)
	{
		if(node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "jaw"))
		{
			baGetNodesWorldPosition(&actorInstance->actorInstance, node, tempVector);
			if (bmVectorDistance(tempVector, gameStatus.player1->actorInstance.position) < METERTOUNIT(1.0f))
			{
				// TP: get taz into correct state
				if (RequestStateChange(gameStatus.player1, STATE_TRAPPED))
				{
					if (UpdateCharacterState(&gameStatus.player[0], true))
					{
						// TP: eat it you mutha
						sharkInfo->gotTaz = true;
						bmVectorSet(gameStatus.player1->actorInstance.scale, 0.0f, 0.0f, 0.0f, 1.0f);
						VFX_timesliceLatch=false;
						VFX_timesliceTimer=0.0f;

//						TimeFX_setGlobalRate(0.25f);
					}
					else
					{
						gameStatus.player1->actorStatus->stateRequest = (ECharState)-1;
						gameStatus.player1->actorStatus->stateRequestCallback = NULL;
					}
				}
			}
		}
	}

	if (sharkInfo->jumpT <= 1.0f)
	{
		FindPointOnCurve(actorInstance->actorInstance.position, sharkInfo->start, sharkInfo->p1, sharkInfo->p2, sharkInfo->end, sharkInfo->jumpT);
		return false;
	}
	else
	{
		return true;
	}
}


/* --------------------------------------------------------------------------------
   Function : SharkMessageHandler
   Purpose : Called when a message is sent in the sequence
   Parameters : ACTORINSTANCE *, char *message
   Returns : 
   Info : 
*/

int SharkMessageHandler(ACTORINSTANCE *actorInstance, char *message)
{
	GENERICAI	*genericAI = actorInstance->genericAI;
	SHARKINFO	*sharkInfo = (SHARKINFO*)genericAI->customData;
	char		string[256];
	TBVector		tempVector;
	TBQuaternion	tempQuat;
	char			*anim;

	if (strcmpi(message, "facepoint1")==0)
	{
		sharkInfo->costumeOk = false;	
		// TP: make sure the rotation direction is anti-clockwise
		FacePoint(actorInstance, sharkInfo->patrolPoint[0], 0.0f, RAD(-179.0f));

		// TP: now face
		FacePoint(actorInstance, sharkInfo->patrolPoint[0], RAD(180.0f) / 250.0f, 0.0f);
		return false;
	}

	if (strcmpi(message, "facepoint2")==0)
	{
		sharkInfo->costumeOk = false;	
		// TP: make sure the rotation direction is anti-clockwise
		FacePoint(actorInstance, sharkInfo->patrolPoint[1], 0.0f, RAD(-179.0f));

		FacePoint(actorInstance, sharkInfo->patrolPoint[1], RAD(180.0f) / 250.0f, 0.0f);
		return false;
	}

	if (strcmpi(message, "headpoint1")==0)
	{
		sharkInfo->costumeOk = true;	

		FacePoint(actorInstance, sharkInfo->patrolPoint[0], RAD(180.0f) / 120.0f, 0.0f);
		anim = "swim1";

		if (!CheckIfPlayingAnimation(actorInstance, anim))
		{
			PlayAnimationByName(actorInstance, anim, 1.0f, 1, 0, 0.2f, MOVE);
		}
		
		if (ReachedPoint(actorInstance->actorInstance.position, sharkInfo->patrolPoint[0], METERTOUNIT(0.2f)))
		{
			// TP: got there
			return false;
		}
		else
		{
			return true;
		}
	}

	if (strcmpi(message, "headpoint2")==0)
	{
		sharkInfo->costumeOk = false;	

		FacePoint(actorInstance, sharkInfo->patrolPoint[1], RAD(180.0f) / 120.0f, 0.0f);
		anim = "swim1";

		if (!CheckIfPlayingAnimation(actorInstance, anim))
		{
			PlayAnimationByName(actorInstance, anim, 1.0f, 1, 0, 0.2f, MOVE);
		}

		if (ReachedPoint(actorInstance->actorInstance.position, sharkInfo->patrolPoint[1], METERTOUNIT(0.2f)))
		{
			// TP: got there
			return false;
		}
		else
		{
			return true;
		}
	}

	if (strcmpi(message, "headpoint2cruise")==0)
	{
		sharkInfo->costumeOk = true;	

		FacePoint(actorInstance, sharkInfo->patrolPoint[1], RAD(180.0f) / 120.0f, RAD(180.0f));
		anim = "swim7";

		if (!CheckIfPlayingAnimation(actorInstance, anim))
		{
			PlayAnimationByName(actorInstance, anim, 1.0f, 1, 0, 0.2f, MOVE);
		}

		if (ReachedPoint(actorInstance->actorInstance.position, sharkInfo->patrolPoint[1], METERTOUNIT(0.2f)))
		{
			// TP: got there
			return false;
		}
		else
		{
			return true;
		}
	}

	if (strcmpi(message, "attack1")==0)
	{
		switch(sharkInfo->state)
		{
		case SHARKSTATE_STARTATTACK:
			actorInstance->genericAI->doCollisionWithTaz = false;
			actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_NOGRAVITY;
			actorInstance->actorAnimation->useMotionBone = FALSE;
			sharkInfo->jumpT = 0.0f;

			bmVectorCopy(sharkInfo->start, actorInstance->actorInstance.position);

			// TP: which point am I facing
			if (FacingPoint(actorInstance, sharkInfo->patrolPoint[0], 0.0f, RAD(45.0f)))
			{
				bmVectorCopy(sharkInfo->end, sharkInfo->patrolPoint[0]);
			}
			else
			{
				bmVectorCopy(sharkInfo->end, sharkInfo->patrolPoint[1]);
			}

			bmVectorLerp(sharkInfo->p1, sharkInfo->start, sharkInfo->end, 0.3f);
			bmVectorLerp(sharkInfo->p2, sharkInfo->start, sharkInfo->end, 0.6f);

			sharkInfo->p1[Y] = gameStatus.player1->actorInstance.position[Y];
			sharkInfo->p2[Y] = gameStatus.player1->actorInstance.position[Y];

			sharkInfo->state = SHARKSTATE_FLY;

			PlayAnimationByName(actorInstance, "newattack2", (0.5f / stime), 0, 0, 0.2f, MOVE);// TP: 0.6
			return true;
		
		case SHARKSTATE_FLY:

			if (sharkInfo->gotTaz)
			{
				HoldTazInShark(actorInstance);
				if((!VFX_timesliceLatch)&&(VFX_timesliceTimer<1.0f))
				{
					VFX_timeslice();
				}
			}

			if (SharkFlyingUpdate(actorInstance))
			{
				actorInstance->genericAI->doCollisionWithTaz = true;
				actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_NOGRAVITY;
				sharkInfo->state = SHARKSTATE_LAND;
			}

			return true;
			break;
		
		case SHARKSTATE_LAND:

			if (sharkInfo->gotTaz)
			{
				HoldTazInShark(actorInstance);
			}

			if (CheckIfPlayingAnimation(actorInstance, "newattack3")) return true;

			actorInstance->actorAnimation->useMotionBone = TRUE;
			TimeFX_setGlobalRate(1.0f);

			if (sharkInfo->gotTaz)
			{
				if (sharkInfo->myWater)
				{
					FindClosestResetPoint(sharkInfo->myWater, gameStatus.player1->actorInstance.position, sharkInfo->resetPoint, tempQuat);
					FacePoint(actorInstance, sharkInfo->resetPoint,  RAD(180.0f) / 120.0f, 0.0f);
					sharkInfo->gotResetPoint = true;
				}
				else
				{
					sharkInfo->gotResetPoint = false;
				}

				PlaySample("sharkeat.wav", 255);
				PlayAnimationByName(actorInstance, "newattack4", 1.0f, 0, 0, 0.1f, MOVE);
				sharkInfo->state = SHARKSTATE_GOTTAZ;
			}
			else
			{
				sharkInfo->state = SHARKSTATE_MISSEDTAZ;
			}
			return true;
			break;
		
		case SHARKSTATE_GOTTAZ:
			if (sharkInfo->gotTaz)
			{
				HoldTazInShark(actorInstance);
			}

			VFX_timesliceLatch=false;
			VFX_timesliceTimer=0.0f;

			if (CheckIfPlayingAnimation(actorInstance, "newattack4")) return true;

			sharkInfo->state = SHARKSTATE_STARTATTACK;
			return false;
			break;
		
		case SHARKSTATE_MISSEDTAZ:
			sharkInfo->state = SHARKSTATE_STARTATTACK;
			return false;
			break;
		}
	}

	if (strcmpi(message,"spittaz")==0)
	{
		PlaySample("sharkburp.wav", 255);

		if (sharkInfo->gotResetPoint)
		{
			SharkSpitTaz(actorInstance, sharkInfo->resetPoint);
		}
		else
		{
			SharkSpitTaz(actorInstance, NULL);
		}
		sharkInfo->gotTaz = false;
		return true;
	}

	/*if (strcmpi(message,"sharkbite")==0)
	{
		SharkBite(actorInstance);
	}*/
	return false;
}


/*  --------------------------------------------------------------------------------
	Function	: SharkGoAware
	Purpose		: Used to determine if the shark should enter the aware state
	Parameters	: shark actor
	Returns		: true/false
	Info		: 
*/

static bool  SharkGoAware(ACTORINSTANCE *actorInstance)
{
	GENERICAI	*genericAI = actorInstance->genericAI;
	SHARKINFO	*sharkInfo = (SHARKINFO*)actorInstance->genericAI->customData;

	if (gameStatus.player1->characterInfo->costume!=-1)
	{
		// TP: check its ok to switch to the costume seq
		if (sharkInfo->costumeOk)
		{
			if ((genericAI->distTazToCenter < genericAI->awarenessRange)||(genericAI->flags&GENERICAI_NOBOUNDARY)) return true;
		}
		return false;
	}

	if (!IsTazWithinHeightRange(actorInstance)) return false;

	if (bmFAbs(genericAI->facingRelToTaz) > RAD(45.0f)) return false;

	if (genericAI->useMoveBox)
	{
		if (IsPointWithinBox(&genericAI->moveBox.attackBBox,genericAI->moveBox.invmatrix,gameStatus.player1->actorInstance.position,BOXFLAG_XTEST|BOXFLAG_ZTEST)||(genericAI->flags&GENERICAI_NOBOUNDARY))
		{
			return true;
		}
	}
	else
	{
		if ((genericAI->distTazToCenter < genericAI->awarenessRange)||(genericAI->flags&GENERICAI_NOBOUNDARY)) return true;
	}

	return false;
}


/*  --------------------------------------------------------------------------------
	Function	: SharkGoAttack
	Purpose		: Used to determine if the shark should enter the attack state
	Parameters	: shark actor
	Returns		: true/false
	Info		: 
*/

static bool SharkGoAttack(ACTORINSTANCE *actorInstance)
{
	GENERICAI	*genericAI = actorInstance->genericAI;

	if (gameStatus.player1->characterInfo->costume!=-1)
	{
		// TP: taz in costume
		return false;
	}

	if (!IsTazWithinHeightRange(actorInstance)) return false;

	if (bmFAbs(genericAI->facingRelToTaz) > RAD(45.0f)) return false;

	if (genericAI->useMoveBox)
	{
		if (IsPointWithinBox(&genericAI->moveBox.attackBBox,genericAI->moveBox.invmatrix,gameStatus.player1->actorInstance.position,BOXFLAG_XTEST|BOXFLAG_ZTEST)||(genericAI->flags&GENERICAI_NOBOUNDARY))
		{
			return true;
		}
	}
	else
	{
		if ((genericAI->distTazToCenter < genericAI->awarenessRange)||(genericAI->flags&GENERICAI_NOBOUNDARY)) return true;
	}

	return false;
}


/*  --------------------------------------------------------------------------------
	Function	: FindPatrolPoints
	Purpose		: Find two patrol points inside the box
	Parameters	: ACTORINSTANCE of shark
	Returns		: 
	Info		: 
*/

void FindPatrolPoints(ACTORINSTANCE *actorInstance)
{
	GENERICAI	*genericAI = actorInstance->genericAI;
	SHARKINFO	*sharkInfo = (SHARKINFO*)genericAI->customData;
	TBVector	x1, x2, z1, z2;

	if (!sharkInfo) return;

	if (!genericAI->useMoveBox)
	{
		// TP: default patrol points for non box enemy
		bmVectorCopy(sharkInfo->patrolPoint[0], genericAI->center);
		bmVectorCopy(sharkInfo->patrolPoint[1], genericAI->center);
		sharkInfo->patrolPoint[0][X] += METERTOUNIT(2.0f);
		sharkInfo->patrolPoint[0][X] -= METERTOUNIT(2.0f);
	}

	float	z = (genericAI->moveBox.bbox.zmax + genericAI->moveBox.bbox.zmin) / 2.0f;
	bmVectorSet(x1, genericAI->moveBox.bbox.xmin, genericAI->center[Y], z, 1.0f);
	bmVectorSet(x2, genericAI->moveBox.bbox.xmax, genericAI->center[Y], z, 1.0f);

	bmMatMultiplyVector(genericAI->moveBox.matrix, x1);
	bmMatMultiplyVector(genericAI->moveBox.matrix, x2);

	float	x = (genericAI->moveBox.bbox.xmax + genericAI->moveBox.bbox.xmin) / 2.0f;
	bmVectorSet(z1, x, genericAI->center[Y], genericAI->moveBox.bbox.zmin, 1.0f);
	bmVectorSet(z2, x, genericAI->center[Y], genericAI->moveBox.bbox.zmax, 1.0f);

	bmMatMultiplyVector(genericAI->moveBox.matrix, z1);
	bmMatMultiplyVector(genericAI->moveBox.matrix, z2);

	if (bmVectorDistance(x1, x2) > bmVectorDistance(z1, z2))
	{
		// TP: x axis is longer
		float	z = (genericAI->moveBox.bbox.zmax + genericAI->moveBox.bbox.zmin) / 2.0f;
		bmVectorSet(sharkInfo->patrolPoint[0], genericAI->moveBox.bbox.xmin, genericAI->center[Y], z, 1.0f);
		bmVectorSet(sharkInfo->patrolPoint[1], genericAI->moveBox.bbox.xmax, genericAI->center[Y], z, 1.0f);
	}
	else
	{
		float	x = (genericAI->moveBox.bbox.xmax + genericAI->moveBox.bbox.xmin) / 2.0f;
		bmVectorSet(sharkInfo->patrolPoint[0], x, genericAI->center[Y], genericAI->moveBox.bbox.zmin, 1.0f);
		bmVectorSet(sharkInfo->patrolPoint[1], x, genericAI->center[Y], genericAI->moveBox.bbox.zmax, 1.0f);
	}

	bmMatMultiplyVector(genericAI->moveBox.matrix, sharkInfo->patrolPoint[0]);
	sharkInfo->patrolPoint[0][Y] = genericAI->center[Y];
	bmMatMultiplyVector(genericAI->moveBox.matrix, sharkInfo->patrolPoint[1]);
	sharkInfo->patrolPoint[1][Y] = genericAI->center[Y];
}


/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceShark
   Purpose : Setup the basic ai for a zookeeper
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceShark(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList)
{
	GENERICAI	*genericAI;
	TBVector	tempVector;
	SHARKINFO	*sharkInfo;

	// create extra structures
	CreateActorInstanceBehaviour(actorInstance);
	CreateActorInstanceAnimation(actorInstance);
	CreateActorInstanceStatus(actorInstance);
	// TP: CreateActorInstanceCharacterInfo(actorInstance);
	SetActorInertia(actorInstance->actorBehaviour,0);
	SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
	SetActorMaxSpeed(actorInstance->actorBehaviour,METERTOUNIT(15)); // set max speed 8mps
	SETVECTOR(tempVector,0.0f,METERTOUNIT(1.0f),0.0f,1.0f);
	
	// create ai structure
	if (CreateActorInstanceGenericAI(actorInstance,activeList,inactiveList)==FALSE) return FALSE;

	// fill in custom parms
	genericAI = actorInstance->genericAI;
	bmVectorCopy(genericAI->center,actorInstance->actorInstance.position);
	genericAI->deactivationRange = METERTOUNIT(12.0f);
	genericAI->awarenessRange = METERTOUNIT(6.0f);
	genericAI->movementRange = METERTOUNIT(6.0f);
	genericAI->standOffRange = METERTOUNIT(0.7f);
	genericAI->personalSpace = METERTOUNIT(0.0f);
	genericAI->viewAngle = PI;
	genericAI->instantAttack = true;
	genericAI->instantAware = true;
	genericAI->flags |= GENERICAI_IGNORESPECIALCOLL;

	genericAI->headNode = "brow";
	SETVECTOR(genericAI->starsOffset,0.0f,METERTOUNIT(0.2f),0.0f,1.0f);
	SETVECTOR(genericAI->starVars,METERTOUNIT(0.3f),METERTOUNIT(0.4f),METERTOUNIT(0.0f),METERTOUNIT(0.0f));

	SetAISeqFunctions(actorInstance, SharkIdleSeq, SharkAwareSeq, SharkAttackSeq, SharkTooCloseSeq, SharkAttackedSeq, SharkOutOfRangeSeq, SharkCostumeSeq);

	AISetShadow(actorInstance,0.0f,0.0f,0.0f);

	//SetAIAttackCallback(actorInstance,SharkAttackCallback);
	SetAIChangeStateCallback(actorInstance, SharkStateChangeCallback);
	SetAIMessageHandler(actorInstance,SharkMessageHandler);

	genericAI->goAttack = SharkGoAttack;
	genericAI->goAware = SharkGoAware;
	genericAI->colliderSpecialCallback = CollideWithSpecial;

	genericAI->alwaysClipPosition = true;

	genericAI->customData = (SHARKINFO*)ZALLOC(sizeof(SHARKINFO));
	sharkInfo = (SHARKINFO*)genericAI->customData;
	ASSERT(genericAI->customData);

	sharkInfo->state = SHARKSTATE_STARTATTACK;
	sharkInfo->gotTaz = false;
	sharkInfo->myWater = NULL;
	sharkInfo->gotResetPoint = false;
	sharkInfo->costumeOk = false;

	return TRUE;
}
