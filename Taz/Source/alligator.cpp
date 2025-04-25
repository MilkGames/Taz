// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : alligator.cpp
//   Purpose : Alligator AI
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
#include "alligator.h"
#include "control.h"
#include "breakables.h"
#include "projectile.h"
#include "sfx.h"
#include "watersplash.h"

static SEQ idle[] = 
{
	{SEQ_SETROTSPEED,NULL,RAD(30.0f)},
	{SEQ_SETANIMSPEED,NULL,1.0f},
	{SEQ_MOTIONBONE,NULL,TRUE},
	{SEQ_RETURN,"swim2",0},
	{SEQ_ROTATELEFT,"swim2",4.0f},
	{SEQ_PLAY,"idle3",0},
	{SEQ_RETURN,"swim2",0},
	{SEQ_ROTATERIGHT,"swim1",8.0f},
	{SEQ_PLAY,"idle4",0},
	{SEQ_RETURN,"swim2",0},
	{SEQ_END,NULL,0},
};

static SEQ attack[] = 
{
	{SEQ_LOCKSEQ,NULL,true},
	{SEQ_CYLCOLLISION,NULL,false},
	{SEQ_ATTACK,"attack1",0},
	{SEQ_PLAY,"return",0},
	{SEQ_LOCKSEQ,NULL,false},
	{SEQ_END,NULL,0},
	/*{SEQ_SETANIMSPEED,NULL,1.0f},
	{SEQ_MOTIONBONE,NULL,TRUE},
	{SEQ_PLAYSAMPLE,"crocklookup.wav",FALSE},
	{SEQ_PLAY,"react1",0},
	{SEQ_MESSAGE,"jump",0},
	{SEQ_PLAY,"attack2",0},
	{SEQ_ATTACK,"attack3",0},
	{SEQ_END,NULL,0},*/
};

static SEQ outOfRange[] =
{
	{SEQ_SETANIMSPEED,NULL,1.0f},
	{SEQ_MOTIONBONE,NULL,TRUE},
	{SEQ_RETURN,"swim2",0},
	{SEQ_END,NULL,0},
};

static SEQ rangedAttack[] = 
{
	{SEQ_LOCKSEQ,NULL,true},
	{SEQ_NODEACTIVATE,NULL,true},
	{SEQ_SETROTSPEED,NULL,RAD(180.0f)},
	{SEQ_BOUNDARY,NULL,FALSE},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAYSAMPLE, "crockattack.wav", 0},
	{SEQ_SETBLENDTIME,NULL,0.0f},
	{SEQ_ATTACK,"jump1",0},
	{SEQ_SETBLENDTIME,NULL,0.2f},
	{SEQ_MOTIONBONE,NULL,false},

	{SEQ_SETCYLRADIUS, NULL, METERTOUNIT(0.6f)},
	{SEQ_SETCYLHEIGHT, NULL, METERTOUNIT(2.7f)},

	{SEQ_MESSAGE, "ra_jump", 0},
	{SEQ_LOCKSEQ,NULL,false},

	{SEQ_STUNNED,"attack6",0},
	{SEQ_STUNNED,"attack7",0},
	{SEQ_STUNNED,"react2",0},

	{SEQ_SKIP, "test1", 2},
	{SEQ_FACECAMERA, NULL, 0},
	{SEQ_PLAY, "lookatcamera", 0},

	{SEQ_SETROTSPEED,NULL,RAD(90.0f)},
	{SEQ_FACECENTER,NULL,0},
	{SEQ_SETROTSPEED,NULL,RAD(360.0f)},
	{SEQ_STUNNED,"turn2",0},
	{SEQ_SETBLENDTIME,NULL,0.0f},
	{SEQ_LOCKSEQ,NULL,true},
	{SEQ_PLAY,"dive1",0},
	{SEQ_SETBLENDTIME,NULL,0.2f},

	{SEQ_PLAYSAMPLE,"crockdive.wav",FALSE},
	{SEQ_MESSAGE,"ra_jumpback",0},

	//{SEQ_SETCYLRADIUS, NULL, METERTOUNIT(1.5f)},
	//{SEQ_SETCYLHEIGHT, NULL, METERTOUNIT(0.4f)},

	{SEQ_PLAY,"dive3",0},
	{SEQ_WAIT,NULL,0.5f},
	{SEQ_NODEACTIVATE,NULL,false},
	{SEQ_BOUNDARY,NULL,TRUE},
	{SEQ_END,NULL,0},
};

static SEQ attacked[] =
{
	{SEQ_BOUNDARY,NULL,FALSE},
	{SEQ_SETANIMSPEED,NULL,1.0f},
	{SEQ_MOTIONBONE,NULL,TRUE},
	{SEQ_FACECENTER,NULL,0},
	{SEQ_PLAY,"react3",0},
	{SEQ_DISABLED,NULL,5.0f},
	{SEQ_PLAY,"react4",0},
	{SEQ_PLAY,"dive1",0},
	{SEQ_PLAYSAMPLE,"crockdive.wav",FALSE},
	{SEQ_MESSAGE,"ra_jumpback",0},
	//{SEQ_ATTACK,"dive2",0},
	{SEQ_SETANIMSPEED,NULL,1.0f},
	{SEQ_MESSAGE,"ra_landwater",0},

	//{SEQ_SETCYLRADIUS, NULL, METERTOUNIT(1.5f)},
	//{SEQ_SETCYLHEIGHT, NULL, METERTOUNIT(0.4f)},
	
	{SEQ_PLAY,"dive3",0},
	{SEQ_BOUNDARY,NULL,true},
	{SEQ_END,NULL,0},
};

static SEQ tazInAttackRange[] =
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAY,"idle1",0},
	{SEQ_END,NULL,0},
};

static SEQ attackedInWater[] =
{
	{SEQ_LOCKSEQ,NULL,true},
	{SEQ_ATTACK,"attack1",0},
	{SEQ_PLAY,"return",0},
	{SEQ_LOCKSEQ,NULL,false},
	{SEQ_END,NULL,0},
};

static float	h1=METERTOUNIT(3.0f),h2=METERTOUNIT(6.0f);

static float alliTime = 0.8f;

/* --------------------------------------------------------------------------------
   Function : AlligatorIdleSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int AlligatorIdleSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = idle;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : AlligatorAwareSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int AlligatorAwareSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = idle;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : AlligatorAttackSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int AlligatorAttackSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	TBVector		pos;
	GENERICAI		*genericAI = actorInstance->genericAI;
	ALLIGATORINFO	*alligatorInfo = (ALLIGATORINFO*)genericAI->customData;
	ACTORINSTANCE	*attackInstance;

	attackInstance = FindActorInstanceInMap(NULL,alligatorInfo->attackObjectCRC,&map);

	if (attackInstance)
	{
		if (IsPointWithinBox(attackInstance,gameStatus.player1->actorInstance.position))
		{
			actorInstance->genericAI->currentSeq = rangedAttack;
			actorInstance->genericAI->seqIdx = 0;
			actorInstance->genericAI->seqTime = 0.0f;

			// TP: keep copy of attack point
			bmVectorCopy(alligatorInfo->endPos, gameStatus.player1->actorInstance.position);

			return TRUE;
		}
	}

	//if (actorInstance->genericAI->collideWithTaz==TRUE)
	{
		// TP: Taz is touching alligator so bite
		actorInstance->genericAI->currentSeq = attack;
	}
	/*else
	{
		actorInstance->genericAI->currentSeq = tazInAttackRange;
	}*/

	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : AlligatorDefendSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int AlligatorDefendSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = idle;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : AlligatorAttackedSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int AlligatorAttackedSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	GENERICAI *genericAI = actorInstance->genericAI;

	if (genericAI->currentSeq[genericAI->seqIdx].type==SEQ_STUNNED)
	{
		actorInstance->genericAI->currentSeq = attacked;
		actorInstance->genericAI->seqIdx = 0;
		actorInstance->genericAI->seqTime = 0.0f;
	}
	else
	{
		actorInstance->genericAI->currentSeq = attackedInWater;
		actorInstance->genericAI->seqIdx = 0;
		actorInstance->genericAI->seqTime = 0.0f;
	}

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : AlligatorOutOfRangeSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int AlligatorOutOfRangeSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = outOfRange;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

enum
{
	ALLISTATE_JUMP,
	ALLISTATE_FLY,
	ALLISTATE_LAND,
};

/*  --------------------------------------------------------------------------------
	Function	: AlligatorJumpUpdate
	Purpose		: Called during the jump move to follow path
	Parameters	: ACTORINSTANCE 
	Returns		: 
	Info		: 
*/
bool AlligatorJumpUpdate(ACTORINSTANCE *actorInstance, bool home = true)
{
	GENERICAI	*genericAI = actorInstance->genericAI;
	ALLIGATORINFO	*alligatorInfo = (ALLIGATORINFO*)genericAI->customData;

	alligatorInfo->jumpT += fTime/(alliTime);	// TP: 5 secs

	if (alligatorInfo->jumpT <= 1.0f)
	{
		if (home && IsPointWithinBox(alligatorInfo->attackInstance, gameStatus.player1->actorInstance.position))
		{
			// TP: If taz is still in box then home in on
			bmVectorCopy(alligatorInfo->endPos, gameStatus.player1->actorInstance.position);

			bmVectorLerp(alligatorInfo->p1, alligatorInfo->startPos, alligatorInfo->endPos, 0.3f);
			bmVectorLerp(alligatorInfo->p2, alligatorInfo->startPos, alligatorInfo->endPos, 0.6f);

			alligatorInfo->p1[Y] += h1;
			alligatorInfo->p2[Y] += h2;

		}

		FindPointOnCurve(actorInstance->actorInstance.position, alligatorInfo->startPos, alligatorInfo->p1, alligatorInfo->p2, alligatorInfo->endPos, alligatorInfo->jumpT);
		return false;
	}
	else
	{
		return true;
	}
}


/* --------------------------------------------------------------------------------
   Function : AlligatorMessageHandler
   Purpose : Called when a message is sent in the sequence
   Parameters : ACTORINSTANCE *, char *message
   Returns : 
   Info : 
*/
int AlligatorMessageHandler(ACTORINSTANCE *actorInstance, char *message)
{
	float		speed;
	TBVector	pos,initialVelocity;
	GENERICAI	*genericAI = actorInstance->genericAI;
	ALLIGATORINFO	*alligatorInfo = (ALLIGATORINFO*)genericAI->customData;
	float		theta,time;

	if (strcmpi(message, "test1")==0)
	{
		if (gameStatus.player1->actorStatus->currentState == STATE_RECOVER) return false;
		else return true;
	}

	if (strcmpi(message,"ra_jump")==0)
	{
		switch(alligatorInfo->alliState)
		{
		case ALLISTATE_JUMP:
			actorInstance->genericAI->doCollisionWithTaz = false;
			actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_NOGRAVITY;
			alligatorInfo->jumpT = 0.0f;

			bmVectorCopy(alligatorInfo->startPos, actorInstance->actorInstance.position);

			// TP: This will have been filled in when attack started
			//bmVectorCopy(alligatorInfo->endPos, gameStatus.player1->actorInstance.position);

			bmVectorLerp(alligatorInfo->p1, alligatorInfo->startPos, alligatorInfo->endPos, 0.3f);
			bmVectorLerp(alligatorInfo->p2, alligatorInfo->startPos, alligatorInfo->endPos, 0.6f);

			alligatorInfo->p1[Y] += h1;
			alligatorInfo->p2[Y] += h2;

			alligatorInfo->alliState = ALLISTATE_FLY;

			PlayAnimationByName(actorInstance, "jump2", (1.12f / alliTime), 0, 0, 0.1f, NONE);
			return true;
			break;

		case ALLISTATE_FLY:
			if (AlligatorJumpUpdate(actorInstance))
			{
				alligatorInfo->alliState = ALLISTATE_LAND;
			}
			return true;
			break;

		case ALLISTATE_LAND:
			actorInstance->genericAI->doCollisionWithTaz = true;
			actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_NOGRAVITY;

			camera[0].quakeOffsetLevel = 0.02f;
			camera[0].tiltOffsetLevel = RAD(1.0f);
			camera[0].flags |= CAMERA_QUAKEFADE;
			actorInstance->actorAnimation->useMotionBone = TRUE;

			alligatorInfo->alliState = ALLISTATE_JUMP;

			bmVectorZero(actorInstance->actorBehaviour->physics.velocity);
			bmVectorCopy(actorInstance->actorInstance.position, alligatorInfo->endPos);

			if (genericAI->collideWithTaz)
			{
				// TP: sort out Taz
				if (RequestStateChange(gameStatus.player1,STATE_RECOVER))
				{
					if (UpdateCharacterState(&gameStatus.player[0], true))
					{
						FlushAnimationQueue(gameStatus.player1);
						PlayAnimationByName(gameStatus.player1,"recover22c",0.001f,0,0,0.0f,NONE);
						PlaySample("poich!.wav",255);
						ReduceCharactersHealth(gameStatus.player1,1.0f);
						RequestRumbleEffect(gameStatus.player[0].controller, 255, 0.0f, 0.5f, 0.5f, 0.0f, WAVEFORM_NORMAL);
					}
					else
					{
						ClearStateChangeRequest(gameStatus.player1);
					}
					// TP: Must use time based stop colliding with, other doesn't work between to characters
					StopCollidingWith(gameStatus.player1,actorInstance, 7.0f);
					StopCollidingWith(actorInstance,gameStatus.player1, 7.0f);
				}
			}
			
			return false;
			break;
		}


		return false;
	}

	if (strcmpi(message,"ra_jumpback")==0)
	{
		switch(alligatorInfo->alliState)
		{
		case ALLISTATE_JUMP:
			actorInstance->genericAI->doCollisionWithTaz = false;

			actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_NOGRAVITY;
			actorInstance->actorAnimation->useMotionBone = FALSE;

			alligatorInfo->jumpT = 0.0f;

			bmVectorCopy(alligatorInfo->startPos, actorInstance->actorInstance.position);
			bmVectorCopy(alligatorInfo->endPos, genericAI->center);

			bmVectorLerp(alligatorInfo->p1, alligatorInfo->startPos, alligatorInfo->endPos, 0.3f);
			bmVectorLerp(alligatorInfo->p2, alligatorInfo->startPos, alligatorInfo->endPos, 0.6f);

			alligatorInfo->p1[Y] += h1;
			alligatorInfo->p2[Y] += h2;

			alligatorInfo->alliState = ALLISTATE_FLY;

			PlayAnimationByName(actorInstance, "dive2", (1.12f / alliTime), 0, 0, 0.1f, NONE);
			return true;
			break;

		case ALLISTATE_FLY:
			if (AlligatorJumpUpdate(actorInstance, false))
			{
				alligatorInfo->alliState = ALLISTATE_LAND;
			}
			return true;
			break;

		case ALLISTATE_LAND:
			actorInstance->genericAI->doCollisionWithTaz = true;
			actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_NOGRAVITY;

			camera[0].quakeOffsetLevel = 0.02f;
			camera[0].tiltOffsetLevel = RAD(1.0f);
			camera[0].flags |= CAMERA_QUAKEFADE;
			actorInstance->actorAnimation->useMotionBone = TRUE;

			alligatorInfo->alliState = ALLISTATE_JUMP;

			bmVectorZero(actorInstance->actorBehaviour->physics.velocity);
			bmVectorCopy(actorInstance->actorInstance.position, alligatorInfo->endPos);

			if (CheckIfPlayingAnimation(gameStatus.player1,"recover22c"))
			{
				FlushAnimationQueue(gameStatus.player1);
				PlayAnimationByName(gameStatus.player1,"recover22c",1.0f,0,0,0.1f,NONE);
				PlaySample("taz_gen_10a.wav",255);
			}

			CreateWaterSplashes(actorInstance->actorInstance.position);
			return false;
			break;
		}


		return false;
	}

	if (strcmpi(message,"ra_landwater")==0)
	{
		actorInstance->genericAI->doCollisionWithTaz = true;
		if (CheckIfPlayingAnimation(gameStatus.player1,"recover22c"))
		{
			FlushAnimationQueue(gameStatus.player1);
			PlayAnimationByName(gameStatus.player1,"recover22c",1.0f,0,0,0.1f,NONE);
			PlaySample("taz_gen_10a.wav",255);
		}
		actorInstance->actorAnimation->useMotionBone = TRUE;
		return false;
	}

	if (strcmpi(message,"ra_land")==0)
	{
		//actorInstance->genericAI->doCollisionWithTaz = true;
		actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_NOGRAVITY;

		// TP: PROBLEM
		camera[0].quakeOffsetLevel = 0.02f;
		camera[0].tiltOffsetLevel = RAD(1.0f);
		camera[0].flags |= CAMERA_QUAKEFADE;
		//actorInstance->actorAnimation->useMotionBone = TRUE;
		return false;
	}
	return false;
}

/*  --------------------------------------------------------------------------------
	Function	: AlligatorUpdate
	Purpose		: Called once per frame to do updates
	Parameters	: actorInstance of alligator
	Returns		: 
	Info		: 
*/
void AlligatorUpdate(ACTORINSTANCE *actorInstance)
{
	ACTORINSTANCE	*attackInstance;
	GENERICAI		*genericAI = actorInstance->genericAI;
	ALLIGATORINFO	*alligatorInfo = (ALLIGATORINFO*)actorInstance->genericAI->customData;

	alligatorInfo->attackInstance = FindActorInstanceInMap(NULL,alligatorInfo->attackObjectCRC,&map);
		
	if (alligatorInfo->attackInstance)
	{
		if (IsPointWithinBox(alligatorInfo->attackInstance, gameStatus.player1->actorInstance.position))
		{
			if (genericAI->currentSeq==idle)
			{
				genericAI->currentSeq = rangedAttack;
				genericAI->seqIdx = 0;
				genericAI->seqTime = 0.0f;

				// TP: keep copy of attack point
				bmVectorCopy(alligatorInfo->endPos, gameStatus.player1->actorInstance.position);
			}
		}
	}

	// TP: Allways stop alligators collision response to the player, prevents Taz being able to push the alligator
	StopCollidingWith(actorInstance, gameStatus.player1, 5.0f);

	//AlligatorJumpUpdate(actorInstance);
}


/* --------------------------------------------------------------------------------
   Function : AlligatorAttackCallback
   Purpose : Called each frame during an attack 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
int AlligatorAttackCallback(struct TAG_ACTORINSTANCE *actorInstance)
{
	TBVector		tempVector,tempVector2;
	float			speed;
	ACTOR_PHYSICS	*actorPhysics = &actorInstance->actorBehaviour->physics;
	GENERICAI		*genericAI = actorInstance->genericAI;
	ALLIGATORINFO	*alligatorInfo = (ALLIGATORINFO*)genericAI->customData;

	alligatorInfo->contactWithPlayer = FALSE;
	
	if (CheckIfPlayingAnimation(actorInstance,"jump2")||CheckIfPlayingAnimation(actorInstance,"attack5"))
	{
		/*if (actorPhysics->coll)
		{
			actorInstance->actorAnimation->useMotionBone = TRUE;
			actorInstance->genericAI->doCollisionWithTaz = true;
		}*/
		return FALSE;
	}
	if (CheckIfPlayingAnimation(actorInstance,"dive2"))
	{
		if (actorPhysics->coll)
		{
			actorInstance->actorAnimation->useMotionBone = TRUE;
		}
		return FALSE;
	}
	if (CheckIfPlayingAnimation(actorInstance,"jump2"))
	{

		return FALSE;
	}
	return FALSE;	// continue processing pattern

}

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceAlligator
   Purpose : Setup the basic ai for an alligator
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceAlligator(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList,uint32 attackPointCRC)
{
	GENERICAI	*genericAI;
	TBVector	tempVector;
	ALLIGATORINFO	*alligatorInfo;

	// create extra structures
	CreateActorInstanceBehaviour(actorInstance);
	CreateActorInstanceAnimation(actorInstance);
	CreateActorInstanceStatus(actorInstance);
	// TP: CreateActorInstanceCharacterInfo(actorInstance);
	SetActorInertia(actorInstance->actorBehaviour,0);
	SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
	SetActorMaxSpeed(actorInstance->actorBehaviour,METERTOUNIT(9)); // set max speed 8mps

	SETVECTOR(tempVector,0.0f,METERTOUNIT(0.0f),0.0f,1.0f);

	SetActorCylinder(0,actorInstance->actorBehaviour,tempVector,METERTOUNIT(1.0f),METERTOUNIT(0.5f));

	AddActorToCollisionCylinderCache(&collisionCache,actorInstance);

	// create ai structure
	if (CreateActorInstanceGenericAI(actorInstance,activeList,inactiveList)==FALSE) return FALSE;

	AISetShadow(actorInstance,0.0f,0.0f,0.0f);

	// fill in custom parms
	genericAI = actorInstance->genericAI;
	bmVectorCopy(genericAI->center,actorInstance->actorInstance.position);
	genericAI->deactivationRange = METERTOUNIT(12.0f);
	genericAI->awarenessRange = METERTOUNIT(3.5f);
	genericAI->movementRange = METERTOUNIT(3.5f);
	genericAI->standOffRange = METERTOUNIT(1.0f);
	genericAI->viewAngle = PI;
	genericAI->attackSpeedMod = 1.0f;
	genericAI->resetAfterAttackOutOfRange = false;

	SetAISeqFunctions(actorInstance,AlligatorIdleSeq,AlligatorAwareSeq,AlligatorAttackSeq,
		AlligatorDefendSeq,AlligatorAttackedSeq,AlligatorOutOfRangeSeq);

	SetAIAttackCallback(actorInstance,AlligatorAttackCallback);
	SetAIMessageHandler(actorInstance,AlligatorMessageHandler);

	genericAI->customData = (ALLIGATORINFO*)ZALLOC(sizeof(ALLIGATORINFO));
	ASSERT(genericAI->customData);

	alligatorInfo = (ALLIGATORINFO*)genericAI->customData;

	genericAI->alwaysClipPosition = true;
	genericAI->instantAttack = true;
	genericAI->instantAware = true;
	genericAI->knockoutOnTopCollision = false;

	genericAI->frameCallback = AlligatorUpdate;
	alligatorInfo->alliState = ALLISTATE_JUMP;

	alligatorInfo->attackObjectCRC = attackPointCRC;
	genericAI->flags |= GENERICAI_IGNORESPECIALCOLL;

	return TRUE;
}
