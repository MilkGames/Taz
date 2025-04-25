// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : nastycan.cpp
//   Purpose : Nasty can AI
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
#include "nastycan.h"
#include "lava.h"
#include "health.h"
#include "clouds.h"
#include "doggy.h"

#define SHADOWMAP_RADIUS_NASTYCAN			METERTOUNIT(5.5)
/*
Idle1		Breathe
Walk1	
Run1	
Idle2		Lookaround
Gunready	Level gun at Taz
Attack1		Catlin-gun rapid fire loop
Attack2		Cactus mine shot
React1		Taz scare from behind
Stunned1	Hide in hat
Recover1	Exit hat
React2		Taz bite from behind
Stunned2	Cringe on ground
Recover2	Stand up
React3		Taz spin from behind
React4		Taz bite from front, flip & twat with gun
Disguise	See taz in costume
*/

static SEQ idle[] = 
{
	{SEQ_RETURN,"walk1",0},
	{SEQ_PLAY,"idle1",0},
	{SEQ_PLAY,"idle2",0},
	{SEQ_WALK,"walk1",1},
	{SEQ_TURNLEFT,NULL,90},
	{SEQ_WALK,"walk1",1},
	{SEQ_TURNLEFT,NULL,90},
	{SEQ_WALK,"walk1",1},
	{SEQ_TURNLEFT,NULL,90},
	{SEQ_WALK,"walk1",1},
	{SEQ_TURNLEFT,NULL,90},
	{SEQ_END,NULL,0},
};

static SEQ aware[] =
{
	{SEQ_RETURN,"walk1",0},
	{SEQ_SETROTSPEED,NULL,RAD(90)},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_SKIP,"test1",1},
	{SEQ_PLAY,"gunready",0},
	{SEQ_ATTACK,"attack2",0},
	{SEQ_PLAY,"idle1",0},
	{SEQ_PLAY,"idle2",0},
	{SEQ_PLAY,"gunready",0},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_ATTACK,"attack1",0},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_ATTACK,"attack1",0},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_ATTACK,"attack1",0},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_ATTACK,"attack1",0},

	{SEQ_END,NULL,0},
};

static SEQ attack[] = 
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_ATTACK,"react4",0},
	{SEQ_PLAY,"idle1",0},
	{SEQ_PLAY,"idle1",0},
	{SEQ_END,NULL,0},
};

static SEQ spinFront[] =
{
	{SEQ_PLAY,"react3",0.0f},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_DISABLED,NULL,15.0f},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAY,"recover2",0.0f},
	{SEQ_END,NULL,0},
};

static SEQ spinRear[] =
{
	{SEQ_PLAYSAMPLE, "zoof.wav",FALSE},
	{SEQ_PLAY,"react3",0.0f},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_DISABLED,NULL,15.0f},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAY,"recover2",0.0f},
	{SEQ_PLAYSAMPLE, "cactus_6.wav",FALSE},
	{SEQ_END,NULL,0},
};

static SEQ biteFront[] =
{
	{SEQ_ATTACK,"react4",0},
	{SEQ_END,NULL,0},
};

static SEQ biteRear[] =
{
	{SEQ_PLAYSAMPLE, "cactus_2.wav",FALSE},
	{SEQ_PLAY,"react2",0},
	{SEQ_END,NULL,0},
};

static SEQ scareFront[] =
{
	{SEQ_PLAY,"react4",0},
	{SEQ_END,NULL,0},
};

static SEQ scareRear[] = 
{
	{SEQ_PLAYSAMPLE, "cactus_3.wav",FALSE},
	{SEQ_PLAY,"react1",0},
	{SEQ_END,NULL,0},
};

static SEQ tooClose[] = 
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_LOCKSEQ,NULL,TRUE},
	{SEQ_WALK,"walk1",1},
	{SEQ_LOCKSEQ,NULL,FALSE},
	{SEQ_END,NULL,0},
};

static SEQ outOfRange[] =
{
	{SEQ_RETURN,"walk1",0},
	{SEQ_END,NULL,0},
};

/* --------------------------------------------------------------------------------
   Function : NastyCanIdleSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int NastyCanIdleSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = idle;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : NastyCanAwareSeq
   Purpose : Respond to requests for aware seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int NastyCanAwareSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = aware;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	if (bmRand()%2)
	{
		switch(bmRand()%2)
		{
		case 0:	PlaySample("cactus_4.wav", 255, actorInstance->actorInstance.position);	break;
		case 1:	PlaySample("cactus_5.wav", 255, actorInstance->actorInstance.position);	break;
		}
	}

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : NastyCanAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int NastyCanAttackSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = attack;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : NastyCanAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int NastyCanPursuitAttackSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = attack;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : NastyCanTooCloseSeq
   Purpose : Respond to requests for defend seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int NastyCanTooCloseSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = tooClose;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : NastyCanAttackedSeq
   Purpose : Respond to requests for attacked seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int NastyCanAttackedSeq(struct TAG_ACTORINSTANCE *actorInstance)
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
	case ATTACKTYPE_SPINREAR:
	case ATTACKTYPE_COSTUMEFRONT:
	case ATTACKTYPE_COSTUMEREAR:
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
   Function : NastyCanOutOfRangeSeq
   Purpose : Respond to requests for out of range seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int NastyCanOutOfRangeSeq(struct TAG_ACTORINSTANCE *actorInstance)
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

/*  --------------------------------------------------------------------------------
	Function	: NastyCanHit
	Purpose		: Called when Nasty can has done a gun thump move
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
void NastyCanHit(ACTORINSTANCE *actorInstance)
{
	TBVector		starsBounds = {METERTOUNIT(0.3f), METERTOUNIT(0.4f), 0.0f, 0.0f};
	TBVector		starsOffset = {0.0f, METERTOUNIT(0.2f), 0.0f, 1.0f};
	
	if (DistFromNodeToTaz(actorInstance,"gun_stem") < METERTOUNIT(1.0f))
	{
		// TP: has hit taz
		ReduceCharactersHealth(gameStatus.player1,1.0f);
		RequestStateChange(gameStatus.player1,STATE_SQUASHED);
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
}

/* --------------------------------------------------------------------------------
   Function : NastyCanAttackCallback
   Purpose : Called each frame during an attack 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
int NastyCanAttackCallback(struct TAG_ACTORINSTANCE *actorInstance)
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

/*  --------------------------------------------------------------------------------
	Function	: CactusReachedDestCallback
	Purpose		: Called when the anvil has reached a position above the players head
	Parameters	: debris
	Returns		: 
	Info		: 
*/
static void CactusReachedDestCallback(DEBRIS *debris)
{
}

/* --------------------------------------------------------------------------------
   Function : NastyCanMessageHandler
   Purpose : Called when a message is sent in the sequence
   Parameters : ACTORINSTANCE *, char *message
   Returns : 
   Info : 
*/
int NastyCanMessageHandler(ACTORINSTANCE *actorInstance, char *message)
{
	float	range;

	// TP: Nasty Canasta
	if (strcmpi(message,"cactusfire")==0)
	{
		NastyCanFire(actorInstance);
		return true;
	}
	if (strcmpi(message,"cactusthump")==0)
	{
		NastyCanHit(actorInstance);
		return true;
	}

	if (!((gameStatus.player1->actorStatus->currentState==STATE_MOVE)||(gameStatus.player1->actorStatus->currentState==STATE_SPIN))) return false;
	
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
	Function	: CactusCollisionCallback
	Purpose		: Callback for when cactus hits something like Taz
	Parameters	: collider, actorInstance
	Returns		: FALSE
	Info		: 
*/

int CactusCollisionCallback(ACTORINSTANCE *collider, ACTORINSTANCE *actorInstance)
{
	ACTORINSTANCE	*taz = NULL;
	ACTORINSTANCE	*object = NULL;

	if (collider==gameStatus.player1)
	{
		taz = collider;
		object = actorInstance;
	}
	else
	{
		if (actorInstance==gameStatus.player1)
		{
			taz = actorInstance;
			object = collider;
		}
	}

	if (taz)
	{
		// TP: poick taz, might as well use the doggy callback it does the same thing
		RequestStateChangePlus(taz, STATE_PROJECTILESLIDE, DoggyBiteChangeCallback);
		FacePoint(taz, object->actorInstance.position, 0.0f, RAD(180.0f));

		RequestRumbleEffect(gameStatus.player[0].controller, 180, 0.0f, 0.5f, 0.75f, 0.0f, WAVEFORM_NORMAL);

		StopCollidingWith(taz, object);
	}

	return FALSE;
}


/*  --------------------------------------------------------------------------------
	Function	: CactusHitCallback
	Purpose		: Gets called when the flying anvil hits something
	Parameters	: DEBRIS, COLLINFO from collision
	Returns		: FALSE to kill debris, TRUE to allow debris to continue
	Info		: 
*/
static int CactusHitCallback(DEBRIS *debris, COLL_INFO *collInfo)
{
	// TP: check it is a collision with a normal mesh
	for (int i=0;i<MAX_COLL_MESHES;i++)
	{
		if ((collInfo->mesh[i].Index!=-1)&&(!(collInfo->mesh[i].flags&RETFLAG_NORESPONCE)))
		{
			// TP: if valid object to stick to then break
			break;
		}
		else
		{
			return TRUE;
		}
	}

	// TP: Create new cactus
	new CACTUS(3.0f,debris->actorInstance->position,debris->actorInstance->scale,debris->actorInstance->orientation);

	return FALSE;
}

/*  --------------------------------------------------------------------------------
	Function	: CreateCactus
	Purpose		: Creates a debris actor for an anvil and sets it ready to go
	Parameters	: actorInstance,initial position, initial velocity, time-life of debris
	Returns		: 
	Info		: 
*/
void CreateCactus(ACTORINSTANCE *actorInstance, TBVector pos, TBVector velocity, float time)
{
	TBQuaternion	rotation;
	DEBRIS			*debris;
	int				flag;
	TBVector		offset;

	flag = DEBRISFLAG_GRAVITY;
	flag |= DEBRISFLAG_ROTATESPRITE|DEBRISFLAG_PHYSICS|DEBRISFLAG_STICKY|DEBRISFLAG_POINTINDIRECTION;

	bmVectorSet(rotation,RAD(bmRand()%360),0.0f,RAD(180),1.0f);

	debris = CreateDebrisActor("cactus.obe", pos, velocity, rotation,time, RAD((float)(bmRand()%360)), METERTOUNIT(0.3f), METERTOUNIT(0.3f), 128, flag,0,0,RAD((float)(bmRand()%720)-360.0f));

	bmVectorSet(offset,0.0f,0.0f,0.0f,1.0f);

	if (debris)
	{
		CreateActorInstanceStatus(debris->actorStructure);
		debris->actorStructure->actorStatus->currentState = STATE_SPIN;

		SetActorCollSphere(debris->actorStructure->actorBehaviour,offset,METERTOUNIT(0.2f));
		SetActorCylinder(0,debris->actorStructure->actorBehaviour,offset,METERTOUNIT(0.2f),METERTOUNIT(1.0f));

		AddActorToCollisionCylinderCache(&collisionCache,debris->actorStructure);

		// TP: Stop taz and debris colliding with each other
		StopCollidingWith(debris->actorStructure,actorInstance);
		StopCollidingWith(actorInstance,debris->actorStructure);
		debris->actorStructure->playerNo = -1;
		debris->actorStructure->flags |= ACTORFLAG_DEBRISACTOR;

		SetCollisionCallback(debris->actorStructure, CactusCollisionCallback);

		SETVECTOR(debris->actorInstance->scale,0.35f,0.35f,0.35f,1.0f);
		ENABLE_LINE_OR_CARTOON_RENDER_MODE(debris->actorInstance);
		debris->impactCallback = CactusHitCallback;
		debris->lifeOverCallback = CactusReachedDestCallback;
	}
}

/* --------------------------------------------------------------------------------
   Function : NastyCanFire
   Purpose : Called when the anvil guy sends an fireanvil command
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
void NastyCanFire(ACTORINSTANCE *actorInstance)
{
	TBVector	tempVector,dest,velocity;
	TBVector	pos;
	TBMatrix	matrix;
	float		time,dist;

	// TP: get current position of snowball
	baGetNodesWorldPosition(&actorInstance->actorInstance, baFindNode(actorInstance->actorInstance.rootNodeInstance,"gun_handle"), pos);

	// TP: get vector to player
	bmVectorSub(tempVector,gameStatus.player1->actorInstance.position,actorInstance->actorInstance.position);

	bmQuatToMatrix(matrix,actorInstance->actorInstance.orientation);
	bmVectorSet(dest,0.0f,0.0f,-1.0f,1.0f);
	bmMatMultiplyVector(matrix,dest);

	dist = bmVectorLen(tempVector);

	if (!dist) return;

	if (dist<METERTOUNIT(2.0f)) dist = METERTOUNIT(2.0f);

	bmVectorScaleToLength(dest,dest,dist);

	dest[Y] += METERTOUNIT(0.5f);

	bmVectorAdd(dest,dest,actorInstance->actorInstance.position);

	time = CalculateProjectileInitialVelocity(velocity, pos, dest, RAD(45.0f));

	time += 0.5f;	// TP: Add half a second to life

	CreateCactus(actorInstance,pos,velocity,time);
}

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceNastyCan
   Purpose : Setup the basic ai for a NastyCan
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceNastyCan(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList)
{
	GENERICAI	*genericAI;
	TBVector	tempVector;

	// create extra structures
	CreateActorInstanceBehaviour(actorInstance);
	CreateActorInstanceAnimation(actorInstance);
	CreateActorInstanceStatus(actorInstance);
	//CreateActorInstanceShadow(actorInstance,METERTOUNIT(0.7f),METERTOUNIT(2.5f), SHADOWMAP_RADIUS_NASTYCAN);
	// TP: CreateActorInstanceCharacterInfo(actorInstance);
	SetActorInertia(actorInstance->actorBehaviour,0);
	SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
	SetActorMaxSpeed(actorInstance->actorBehaviour,METERTOUNIT(9)); // set max speed 8mps
	SETVECTOR(tempVector,0.0f,METERTOUNIT(1.0f),0.0f,1.0f);
	SetActorCylinder(0,actorInstance->actorBehaviour,tempVector,METERTOUNIT(1.5f),METERTOUNIT(2.0f));
	AddActorToCollisionCylinderCache(&collisionCache,actorInstance);
	
	// create ai structure
	if (CreateActorInstanceGenericAI(actorInstance,activeList,inactiveList)==FALSE) return FALSE;

	AISetShadow(actorInstance,METERTOUNIT(0.7f),METERTOUNIT(2.5f), SHADOWMAP_RADIUS_NASTYCAN);

	// fill in custom parms
	genericAI = actorInstance->genericAI;
	bmVectorCopy(genericAI->center,actorInstance->actorInstance.position);

	genericAI->deactivationRange = METERTOUNIT(15.0f);
	genericAI->awarenessRange = METERTOUNIT(10.0f);
	genericAI->movementRange = METERTOUNIT(2.0f);
	genericAI->viewAngle = PI;
	genericAI->personalSpace = METERTOUNIT(1.5f);
	genericAI->standOffRange = METERTOUNIT(2.2f);
	genericAI->attackHoldDistance = METERTOUNIT(2.0f);
	genericAI->hearRange = METERTOUNIT(0.0f);

	genericAI->instantAttack = true;

	genericAI->currentRotTime = RAD(90.0f)/60.0f;

	genericAI->headNode = "mouth_stem";
	SETVECTOR(genericAI->starsOffset,0.0f,METERTOUNIT(0.5f),0.0f,1.0f);
	SETVECTOR(genericAI->starVars,METERTOUNIT(0.3f),METERTOUNIT(0.4f),METERTOUNIT(0.0f),METERTOUNIT(0.0f));

	SetAISeqFunctions(actorInstance,NastyCanIdleSeq,NastyCanAwareSeq,NastyCanAttackSeq,
		NastyCanTooCloseSeq,NastyCanAttackedSeq,NastyCanOutOfRangeSeq,0,NastyCanPursuitAttackSeq);

	SetAIAttackCallback(actorInstance,NastyCanAttackCallback);
	SetAIChangeStateCallback(actorInstance,myChangeState);
	SetAIMessageHandler(actorInstance,NastyCanMessageHandler);

	genericAI->runCRC = bkCRC32((uchar*)"run",3,0);

	return TRUE;
}


LINKEDLIST<CACTUS>	CACTUS::list;


/*  --------------------------------------------------------------------------------
	Function	: CACTUS::Init
	Purpose		: Called to create a cactus
	Parameters	: life, position, scale, orientation
	Returns		: 
	Info		: 
*/

void CACTUS::Init(float life, TBVector position, TBVector scale, TBQuaternion orientation)
{
	actorInstance = CreateActorInstance(&map.levelInstances,"cactus.obe","cactus",position,scale,orientation,0);
	ENABLE_LINE_OR_CARTOON_RENDER_MODE(&actorInstance->actorInstance);
	AddActorToCollisionCache(&collisionCache,actorInstance,0);
	CreateLavaObject(actorInstance,false);
	list.append(this);
	this->life = life;
}


/*  --------------------------------------------------------------------------------
	Function	: CACTUS::Update
	Purpose		: Called to update a cactus
	Parameters	: 
	Returns		: 
	Info		: 
*/

void CACTUS::Update()
{
	if (life)
	{
		life -= fTime;
		if (life <= 0.0f)
		{
			actorInstance->fadeSpeed = -128.0f;
			actorInstance->flags |= ACTORFLAG_DELETEAFTERFADEOUT;
			life = 0.0f;
		}
	}
}


/*  --------------------------------------------------------------------------------
	Function	: UpdateAllCactie
	Purpose		: General cactus update
	Parameters	: 
	Returns		: 
	Info		: 
*/

void UpdateAllCactie(void)
{
	CACTUS	*cactus;

	for (int i=TAG_CACTUS::list.getSize()-1;i>=0;i--)
	{
		cactus = TAG_CACTUS::list.get(i);

		cactus->Update();

		if (cactus->life == 0.0f) 
		{
			TAG_CACTUS::list.remove(cactus);
			delete cactus;
		}
	}
}


/*  --------------------------------------------------------------------------------
	Function	: IsActorInstanceACactus
	Purpose		: Returns true if the actor is a cactus
	Parameters	: actorInstance to test
	Returns		: 
	Info		: 
*/

bool	IsActorInstanceACactus(ACTORINSTANCE *actorInstance)
{
	CACTUS	*cactus;

	for (int i=TAG_CACTUS::list.getSize()-1;i>=0;i--)
	{
		cactus = TAG_CACTUS::list.get(i);

		if (cactus->actorInstance==actorInstance) return true;
	}

	return false;
}