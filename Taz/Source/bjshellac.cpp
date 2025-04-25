// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : bjshellac.cpp
//   Purpose : ai functions for bjshellac
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
#include "bjshellac.h"
#include "control.h"
#include "sfx.h"
#include "debris.h"
#include "fireball.h"
#include "projectile.h"


#define SHADOWMAP_RADIUS_BJSHELLAC			METERTOUNIT(5.5)


#define SHADOWMAP_RADIUS_ANVILGANG			METERTOUNIT(5.5)

static SEQ idle[] = 
{
	{SEQ_RETURN,"walk1",0},
	{SEQ_PLAY,"idle9",0},
	{SEQ_PLAY,"idle10",0},
	{SEQ_WALK,"walk1",1.0f},
	{SEQ_PLAY,"idle2",0},
	{SEQ_PLAY,"idle1",0},
	{SEQ_END,NULL,0},
};

static SEQ aware[] =
{
	{SEQ_RETURN,"walk1",0},
	{SEQ_SETROTSPEED,NULL,RAD(90)},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_WALK,"walk1",0.5f},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAY,"idle9",0},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,true},
	{SEQ_ATTACK,"attack1",0},
	{SEQ_TURNLEFT,NULL,45},
	{SEQ_PLAY,"dive1",0},
	{SEQ_PLAY,"idle3",2.0f},
	{SEQ_PLAY,"idle4",0},
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},
	{SEQ_PLAY,"idle5",0},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAY,"idle10",0},
	{SEQ_SETROTSPEED,NULL,RAD(120)},
	{SEQ_LOOKAWAYFROMTAZ,NULL,0},
	{SEQ_ATTACK,"attack4",1.0f},
	{SEQ_PLAY,"idle7",0},
	{SEQ_PLAY,"idle6",0},
	{SEQ_PLAY,"idle8",0},

	{SEQ_END,NULL,0},
};

static SEQ attack[] = 
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_CHASE,"walk3",0},
	{SEQ_PLAYSAMPLE, "kneeimpact.wav", FALSE},
	{SEQ_ATTACK,"attack6",0},
	{SEQ_END,NULL,0},
};

static SEQ spinFront[] =
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAY,"attack5",0.0f},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAY,"attack6",0.0f},
	{SEQ_END,NULL,0},
};

static SEQ spinRear[] =
{
	{SEQ_PLAYSAMPLE, "zkoof.wav", FALSE},
	{SEQ_PLAY,"react2",0.0f},
	{SEQ_PLAYSAMPLE, "thud1.wav", FALSE},
	{SEQ_PLAY,"react3",0.0f},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_DISABLED,NULL,8.0f},
	{SEQ_PLAY,"idle6",0.0f},
	{SEQ_PLAY,"walk6",0},
	{SEQ_TURNLEFT,NULL,25},
	{SEQ_PLAY,"walk8",0},
	{SEQ_TURNRIGHT,NULL,25},
	{SEQ_PLAY,"walk5",0},
	{SEQ_TURNLEFT,NULL,25},
	{SEQ_PLAY,"walk7",0},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAY,"idle8",0.0f},
	{SEQ_END,NULL,0},
};

static SEQ biteFront[] =
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAY,"attack6",0.0f},
	{SEQ_END,NULL,0},
};

static SEQ scareRear[] = 
{
	{SEQ_PLAYSAMPLE,"cowboy_2.wav",FALSE},
	{SEQ_PLAY,"react1",0},
	{SEQ_WALK,"walk2",0.5f},
	{SEQ_PLAY,"idle7",0},
	{SEQ_PLAY,"idle4",0},
	{SEQ_PLAY,"idle5",0},
	{SEQ_PLAY,"idle10",0},
	{SEQ_END,NULL,0},
};

static SEQ scareFront[] =
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_CHASE,"walk3",0},
	{SEQ_PLAYSAMPLE, "headbut.wav", FALSE},
	{SEQ_PLAY,"attack3",0},

	{SEQ_END,NULL,0},
};

static SEQ biteRear[] =
{
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,true},
	{SEQ_PLAYSAMPLE,"cowboy_3.wav",FALSE},
	{SEQ_PLAY,"react4",0},
	{SEQ_FACECENTER,NULL,0},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_PLAY,"walk5",0},
	{SEQ_TURNLEFT,NULL,25},
	{SEQ_PLAY,"walk6",0},
	{SEQ_TURNRIGHT,NULL,25},
	{SEQ_PLAY,"walk7",0},
	{SEQ_TURNLEFT,NULL,25},
	{SEQ_PLAY,"walk8",0},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAY,"idle8",0},
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},
	{SEQ_END,NULL,0},
};

static SEQ tooClose[] = 
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_WALK,"walk4",1},
	{SEQ_END,NULL,0},
};

static SEQ costumeAttack[] = 
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_STUNNED,"react5",0},
	{SEQ_END,NULL,0},
};

static SEQ costumeAttackScargil[] = 
{
	{SEQ_PLAYSAMPLE, "zkoof.wav", FALSE},
	{SEQ_PLAY,"react2",0.0f},
	{SEQ_PLAYSAMPLE, "thud1.wav", FALSE},
	{SEQ_PLAY,"react3",0.0f},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_DISABLED,NULL,8.0f},
	{SEQ_PLAY,"idle6",0.0f},
	{SEQ_PLAY,"walk6",0},
	{SEQ_TURNLEFT,NULL,25},
	{SEQ_PLAY,"walk8",0},
	{SEQ_TURNRIGHT,NULL,25},
	{SEQ_PLAY,"walk5",0},
	{SEQ_TURNLEFT,NULL,25},
	{SEQ_PLAY,"walk7",0},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAY,"idle8",0.0f},
	{SEQ_END,NULL,0},
};

static SEQ outOfRange[] =
{
	{SEQ_RETURN,"walk1",0},
	{SEQ_END,NULL,0},
};

static SEQ disguise[] =
{
	{SEQ_PLAYSAMPLE, "cowboy_1.wav", FALSE},
	{SEQ_STARTQUESTION,NULL,0},
	{SEQ_PLAY,"disguise",0},
	{SEQ_ENDQUESTION,NULL,0},
	{SEQ_COSTUMEDONE,NULL,0},
	{SEQ_END,NULL,0},
};


/* --------------------------------------------------------------------------------
   Function : BJShellacIdleSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int BJShellacIdleSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = idle;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : BJShellacAwareSeq
   Purpose : Respond to requests for aware seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int BJShellacAwareSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = aware;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	if (bmRand()%2)
	{
		switch(bmRand()%2)
		{
		case 0:	PlaySample("cowboy_4.wav", 255, actorInstance->actorInstance.position);	break;
		case 1:	PlaySample("cowboy_6.wav", 255, actorInstance->actorInstance.position);	break;
		}
	}

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : BJShellacAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int BJShellacAttackSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	if(bmRand()%2)
	{
		PlaySample("cowboy_5.wav", 255, actorInstance->actorInstance.position);
	}

	actorInstance->genericAI->currentSeq = attack;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : BJShellacAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int BJShellacPursuitAttackSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = attack;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : BJShellacTooCloseSeq
   Purpose : Respond to requests for defend seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int BJShellacTooCloseSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = tooClose;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : BJShellacCostumeSeq
   Purpose : Respond to requests for costume seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int BJShellacCostumeSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = disguise;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : BJShellacAttackedSeq
   Purpose : Respond to requests for attacked seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int BJShellacAttackedSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	int			random;
	BJSHELLACINFO *bjInfo = (BJSHELLACINFO*)actorInstance->genericAI->customData;

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
		if (bjInfo->type==0)
		{
			actorInstance->genericAI->currentSeq = costumeAttack;
		}
		else
		{
			actorInstance->genericAI->currentSeq = costumeAttackScargil;
		}
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
   Function : BJShellacOutOfRangeSeq
   Purpose : Respond to requests for out of range seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int BJShellacOutOfRangeSeq(struct TAG_ACTORINSTANCE *actorInstance)
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
}

/* --------------------------------------------------------------------------------
   Function : BJShellacAttackCallback
   Purpose : Called each frame during an attack 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
int BJShellacAttackCallback(struct TAG_ACTORINSTANCE *actorInstance)
{
	return FALSE;	// continue processing pattern
}


/*  --------------------------------------------------------------------------------
	Function	: ChangeState
	Purpose		: Called when the character changes state
	Parameters	: actorInstance, new state, last state
	Returns		: 
	Info		: 
*/
static void myChangeState(struct TAG_ACTORINSTANCE *actorInstance,int state,int lastState)
{
}

static void EnterBounceCallback(ACTORINSTANCE *actorInstance)
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

	SetActorSpeed(gameStatus.player1->actorBehaviour, tempVector, METERTOUNIT(6.0f));
}

static void DynamiteAffectOnTaz(TBVector position, float radius)
{
	float		dist;
	TBVector	tempVector;

	bmVectorSub(tempVector, gameStatus.player1->actorInstance.position, position);

	dist = bmVectorLen(tempVector);

	if ((dist)&&(dist < radius))
	{
		// TP: impact is a value between 0 and 1 to show how close the character was to the explosion
		if (EnterState(gameStatus.player1, STATE_PROJECTILESLIDE))
		{
			ReduceCharactersHealth(gameStatus.player1,1);
			FacePoint(gameStatus.player1, position, 0.0f, RAD(180.0f));
			EnterBounceCallback(gameStatus.player1);
		}
	}
	else
	{
		return;
	}
}

/*  --------------------------------------------------------------------------------
	Function	: DynamiteReachedDestCallback
	Purpose		: Called when the dynamite has reached a position above the players head
	Parameters	: debris
	Returns		: 
	Info		: 
*/
static void DynamiteReachedDestCallback(DEBRIS *debris)
{
	COLOUR	expCols[FB_NUM_STAGE_COLOURS]={FBP_COLOUR_HOT, FBP_COLOUR_COLD, FBP_COLOUR_SMOKE, FBP_COLOUR_FINAL};// PP: default explosion colours

	Fireball_normalExplosion(debris->position, METERTOUNIT(1.5f), expCols[0], expCols[1], expCols[2], expCols[3]);

	DynamiteAffectOnTaz(debris->position, METERTOUNIT(4.0f));
}

/* --------------------------------------------------------------------------------
   Function : BJShellacMessageHandler
   Purpose : Called when a message is sent in the sequence
   Parameters : ACTORINSTANCE *, char *message
   Returns : 
   Info : 
*/
int BJShellacMessageHandler(ACTORINSTANCE *actorInstance, char *message)
{
	float	range;

	// TP: Black jack shellac
	if (strcmpi(message,"bjdynamite")==0)
	{
		BJShellacFire(actorInstance,0);
		return true;
	}
	if (strcmpi(message,"bjdynamite2")==0)
	{
		BJShellacFire(actorInstance,1);
		return true;
	}
	if (strcmpi(message,"headbutt")==0)
	{
		BJShellacFire(actorInstance,2);
		return true;
	}
	if (strcmpi(message,"bjkick")==0)
	{
		BJShellacFire(actorInstance,3);
		return true;
	}
	if (strcmpi(message,"bjstop")==0)
	{
		BJShellacFire(actorInstance,4);
		return true;
	}

	if (!((gameStatus.player1->actorStatus->currentState==STATE_MOVE)||(gameStatus.player1->actorStatus->currentState==STATE_SPIN))) return true;
	
	if (!FindClosestEnemyToActor(gameStatus.player1->actorInstance.position,range,actorInstance)) return true;
	
	if (strcmpi(message,"test1")==0)
	{
		if (range < METERTOUNIT(5.0f)) return false;
		else return true;
	}
	if (strcmpi(message,"test2")==0)
	{
		if (range < METERTOUNIT(10.0f)) return false;
		else return true;
	}
	return true;
}

/*  --------------------------------------------------------------------------------
	Function	: DynamiteHitCallback
	Purpose		: Gets called when the flying dynamite hits something
	Parameters	: DEBRIS, COLLINFO from collision
	Returns		: FALSE to kill debris, TRUE to allow debris to continue
	Info		: 
*/
static int DynamiteHitCallback(DEBRIS *debris, COLL_INFO *collInfo)
{
	// TP: check it is a collision with a normal mesh
	for (int i=0;i<MAX_COLL_MESHES;i++)
	{
		if ((collInfo->mesh[i].Index!=-1)&&(!(collInfo->mesh[i].flags&RETFLAG_NORESPONCE)))
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
/*
	COLOUR	expCols[FB_NUM_STAGE_COLOURS]={FBP_COLOUR_HOT, FBP_COLOUR_COLD, FBP_COLOUR_SMOKE, FBP_COLOUR_FINAL};// PP: default explosion colours

	Fireball_normalExplosion(debris->position, METERTOUNIT(2.0f), expCols[0], expCols[1], expCols[2], expCols[3]);

	DynamiteAffectOnTaz(debris->position, METERTOUNIT(3.0f));
*/
	return FALSE;
}

/*  --------------------------------------------------------------------------------
	Function	: CreateDynamite
	Purpose		: Creates a debris actor for dynamite and sets it ready to go
	Parameters	: actorInstance,initial position, initial velocity, time-life of debris
	Returns		: 
	Info		: 
*/
void CreateDynamite(ACTORINSTANCE *actorInstance, TBVector pos, TBVector velocity, float time)
{
	TBQuaternion	rotation;
	DEBRIS			*debris;
	int				flag;
	TBVector		offset;

	flag = DEBRISFLAG_GRAVITY;
	flag |= DEBRISFLAG_ROTATESPRITE|DEBRISFLAG_PHYSICS;

	rotation[X] = (bmRand()%3)*PI;
	rotation[Y] = (bmRand()%5)*PI;
	rotation[Z] = 0.0f;
	rotation[W] = 1.0f;

	debris = CreateDebrisActor("bjdynamite.obe", pos, velocity, rotation,time, RAD((float)(bmRand()%360)), METERTOUNIT(0.3f), METERTOUNIT(0.3f), 128, flag,0,0,RAD((float)(bmRand()%720)-360.0f));

	bmVectorSet(offset,0.0f,0.0f,0.0f,1.0f);

	if (debris)
	{
		CreateActorInstanceStatus(debris->actorStructure);
		debris->actorStructure->actorStatus->lastState = debris->actorStructure->actorStatus->currentState;
		debris->actorStructure->actorStatus->currentState = STATE_SPIN;

		SetActorCollSphere(debris->actorStructure->actorBehaviour,offset,METERTOUNIT(0.5f));
		SetActorCylinder(0,debris->actorStructure->actorBehaviour,offset,METERTOUNIT(0.5f),METERTOUNIT(1.0f));

		AddActorToCollisionCylinderCache(&collisionCache,debris->actorStructure);
		
		// TP: Stop taz and debris colliding with each other
		StopCollidingWith(debris->actorStructure,actorInstance);
		StopCollidingWith(actorInstance,debris->actorStructure);
		debris->actorStructure->playerNo = -1;
		debris->actorStructure->flags |= ACTORFLAG_DEBRISACTOR;

		SETVECTOR(debris->actorInstance->scale,0.2f,0.2f,0.2f,1.0f);

		ENABLE_LINE_OR_CARTOON_RENDER_MODE(debris->actorInstance);

		debris->impactCallback = DynamiteHitCallback;
		debris->lifeOverCallback = DynamiteReachedDestCallback;
	}
}

/*  --------------------------------------------------------------------------------
	Function	: myKickStateChangeCallback
	Purpose		: standard callback thingy
	Parameters	: 
	Returns		: 
	Info		: 
*/
static void myKickStateChangeCallback(ACTORINSTANCE *actorInstance)
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

static void myPlayAnimStateChangeCallback(ACTORINSTANCE *actorInstance)
{
	FlushAnimationQueue(gameStatus.player1);
	PlayAnimationByName(gameStatus.player1,"react1",1.0f,0,0,0.1f,NONE);
}


/* --------------------------------------------------------------------------------
   Function : BJShellacFire
   Purpose : Called when the anvil guy sends an fireanvil command
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
void BJShellacFire(ACTORINSTANCE *actorInstance, int type)
{
	TBVector	tempVector,dest,velocity;
	TBVector	pos;
	TBMatrix	matrix;
	float		time,dist;

	switch(type)
	{
	case 2:
		if (DistFromNodeToTaz(actorInstance,"eye") < (gameStatus.player1->actorBehaviour->collSphere.radius+METERTOUNIT(0.25f)))
		{
			if (EnterState(gameStatus.player1,STATE_PLAYANIMATION))
			{
				FlushAnimationQueue(gameStatus.player1);
				PlayAnimationByName(gameStatus.player1,"react2",1.0f,0,0,0.1f,NONE);
		
				FacePoint(gameStatus.player1, actorInstance->actorInstance.position, 0.0f, 0.0f);

				ReduceCharactersHealth(gameStatus.player1,1.0f);
			}
		}
		break;

	case 0:
	case 1:
		// TP: get current position of snowball
		baGetNodesWorldPosition(&actorInstance->actorInstance, baFindNode(actorInstance->actorInstance.rootNodeInstance,"rhand"), pos);

		// TP: get vector to player
		bmVectorSub(tempVector,gameStatus.player1->actorInstance.position,actorInstance->actorInstance.position);

		bmQuatToMatrix(matrix,actorInstance->actorInstance.orientation);
		if (type==0)
		{
			bmVectorSet(dest,0.0f,0.0f,-1.0f,1.0f);
		}
		else
		{
			bmVectorSet(dest,0.0f,0.0f,1.0f,1.0f);
		}
		bmMatMultiplyVector(matrix,dest);

		dist = bmVectorLen(tempVector);

		if (!dist) return;

		if (dist<METERTOUNIT(2.0f)) dist = METERTOUNIT(2.0f);

		bmVectorScaleToLength(dest,dest,dist);

		bmVectorAdd(dest,dest,actorInstance->actorInstance.position);

		time = CalculateProjectileInitialVelocity(velocity, pos, dest, RAD(45.0f));

		time += 0.5f;	// TP: Add half a second to life

		CreateDynamite(actorInstance,pos,velocity,time);
		break;

	case 3:
		if (DistFromNodeToTaz(actorInstance,"rleg") < METERTOUNIT(1.25f))
		{
			// TP: UseStateChange callback
			if (EnterState(gameStatus.player1, STATE_PROJECTILESLIDE))
			{
				FacePoint(gameStatus.player1, actorInstance->actorInstance.position, 0.0f, RAD(180.0f));
				myKickStateChangeCallback(gameStatus.player1);
			}
		}
		break;

	case 4:
		if (DistFromNodeToTaz(actorInstance,"rhand") < METERTOUNIT(1.25f))
		{
			if (EnterState(gameStatus.player1,STATE_PLAYANIMATION))
			{
				// TP: Turn taz to face character
				FacePoint(gameStatus.player1,actorInstance->actorInstance.position,0.0f,0.0f);
				myPlayAnimStateChangeCallback(gameStatus.player1);
			}
		}
		break;

	}
}

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceBJShellac
   Purpose : Setup the basic ai for a BJShellac
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceBJShellac(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList,int type)
{
	GENERICAI	*genericAI;
	TBVector	tempVector;

	// create extra structures
	CreateActorInstanceBehaviour(actorInstance);
	CreateActorInstanceAnimation(actorInstance);
	CreateActorInstanceStatus(actorInstance);
	//CreateActorInstanceShadow(actorInstance,METERTOUNIT(0.7f),METERTOUNIT(2.5f), SHADOWMAP_RADIUS_BJSHELLAC);
	// TP: CreateActorInstanceCharacterInfo(actorInstance);
	SetActorInertia(actorInstance->actorBehaviour,0);
	SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
	SetActorMaxSpeed(actorInstance->actorBehaviour,METERTOUNIT(9)); // set max speed 8mps
	SETVECTOR(tempVector,0.0f,METERTOUNIT(1.0f),0.0f,1.0f);
	SetActorCylinder(0,actorInstance->actorBehaviour,tempVector,METERTOUNIT(0.7f),METERTOUNIT(2.0f));
	AddActorToCollisionCylinderCache(&collisionCache,actorInstance);
	
	// create ai structure
	if (CreateActorInstanceGenericAI(actorInstance,activeList,inactiveList)==FALSE) return FALSE;

	AISetShadow(actorInstance,METERTOUNIT(0.7f),METERTOUNIT(2.5f), SHADOWMAP_RADIUS_BJSHELLAC);

	// fill in custom parms
	genericAI = actorInstance->genericAI;
	bmVectorCopy(genericAI->center,actorInstance->actorInstance.position);

	genericAI->deactivationRange = METERTOUNIT(15.0f);
	genericAI->awarenessRange = METERTOUNIT(10.0f);
	genericAI->movementRange = METERTOUNIT(4.0f);
	genericAI->viewAngle = PI/2;
	genericAI->personalSpace = METERTOUNIT(0.5f);
	genericAI->standOffRange = METERTOUNIT(1.5f);
	genericAI->attackHoldDistance = METERTOUNIT(2.0f);
	genericAI->hearRange = METERTOUNIT(0.0f);

	genericAI->instantAttack = true;
	genericAI->instantAware = true;
	genericAI->alwaysClipPosition = true;
	genericAI->costumeInAware = true;

	genericAI->currentRotTime = RAD(90.0f)/60.0f;

	genericAI->bounceCRC = bkCRC32((uchar*)"bounce",6,0);

	genericAI->headNode = "head";
	SETVECTOR(genericAI->starsOffset,0.0f,METERTOUNIT(0.5f),0.0f,1.0f);
	SETVECTOR(genericAI->starVars,METERTOUNIT(0.3f),METERTOUNIT(0.4f),METERTOUNIT(0.0f),METERTOUNIT(0.0f));

	SetAISeqFunctions(actorInstance,BJShellacIdleSeq,BJShellacAwareSeq,BJShellacAttackSeq,
		BJShellacTooCloseSeq,BJShellacAttackedSeq,BJShellacOutOfRangeSeq,BJShellacCostumeSeq,BJShellacPursuitAttackSeq);

	SetAIAttackCallback(actorInstance,BJShellacAttackCallback);
	SetAIChangeStateCallback(actorInstance,myChangeState);
	SetAIMessageHandler(actorInstance,BJShellacMessageHandler);

	genericAI->customData = (BJSHELLACINFO*)ZALLOC(sizeof(BJSHELLACINFO));
	ASSERT(genericAI->customData);
	
	BJSHELLACINFO *bjInfo = (BJSHELLACINFO*)genericAI->customData;

	bjInfo->type = type;

	genericAI->runCRC = bkCRC32((uchar*)"run",3,0);

	return TRUE;
}
