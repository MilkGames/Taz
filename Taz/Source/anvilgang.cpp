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
#include "anvilgang.h"

/*
Walk1		20 - 100	Slow clumping boots
Idle1		Breathe	101 - 261	
Idle2		Look around	261 - 351	
Attack1		396 - 484	Gun firing bang/pop
run			485 - 545	Heavy run
React1		Bitten from rear	546 - 596	"yeoow!"
React1run	Run holding ass	596 - 656	
React2		Scared from rear	657 - 720	"yipes!"
React2cycle	Fire gun into air	720 - 758	Gun fire sample
React2getup	758 - 836	Grunt
Spinout		Spun by Taz & fire gun randomly	837 - 949	Windy whirl & "wooaah!"
Spinrecover	949 - 1215	Thump on land
Attack2arm	Kneel down for repeat fire	1216 - 1279	Arm gun "ka-chik"
Attack2		Rapid fire loop	1280 - 1332	Gun fire anim
Idle3		Check gun	1333 - 1550	Arm gun with lots of "Ka-chik"s
Crateland	1551 - 1640	
Intransit	Loop for traveling in van	1641 - 1681	
leavevan	Jump out of van	1681 - 1733	Grunt on jump
Disguise	See Taz in costume	1734 - 1867	
*/

#define SHADOWMAP_RADIUS_ANVILGANG			METERTOUNIT(5.5)

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
	{SEQ_ATTACK,"attack1",0},
	{SEQ_PLAY,"idle3",0},
	{SEQ_PLAY,"crateland",0},
	{SEQ_PLAY,"idle2",0},
	{SEQ_SKIP,"test2",7},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_ATTACKANDFACE,"attack2arm",0},
	{SEQ_ATTACK,"attack2",0},
	{SEQ_FACETAZ,"attack2arm",-30.0f},
	{SEQ_ATTACK,"attack2",0},
	{SEQ_FACETAZ,"attack2arm",30.0f},
	{SEQ_ATTACK,"attack2",0},
	{SEQ_PLAY,"idle3",0},
	{SEQ_PLAY,"crateland",0},
	{SEQ_PLAY,"idle2",0},

	{SEQ_END,NULL,0},
};

static SEQ attack[] = 
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_ATTACK,"attack1",0},
	{SEQ_END,NULL,0},
};

static SEQ spinFront[] =
{
	{SEQ_PLAYSAMPLE, "zkoof.wav", FALSE},
	{SEQ_PLAY,"spinout",0.0f},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_DISABLED,NULL,20.0f},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAY,"spinrecover",0.0f},
	{SEQ_END,NULL,0},
};

static SEQ spinRear[] =
{
	{SEQ_PLAYSAMPLE, "zkoof.wav", FALSE},
	{SEQ_PLAY,"spinout",0.0f},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_DISABLED,NULL,20.0f},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAY,"spinrecover",0.0f},
	{SEQ_END,NULL,0},
};

static SEQ biteFront[] =
{
	{SEQ_PLAYSAMPLE, "anvilgang_11.wav", FALSE},
	{SEQ_PLAY,"react2",0},
	{SEQ_PLAY,"react2cycle",0},
	{SEQ_PLAY,"react2getup",0},
	{SEQ_END,NULL,0},
};

static SEQ biteRear[] =
{
	{SEQ_PLAYSAMPLE, "anvilgang_10.wav", FALSE},
	{SEQ_PLAY,"react2",0},
	{SEQ_PLAY,"react2cycle",0},
	{SEQ_PLAY,"react2getup",0},
	{SEQ_END,NULL,0},
};

static SEQ scareFront[] =
{
	{SEQ_PLAYSAMPLE, "anvilgang_12.wav", FALSE},
	{SEQ_PLAY,"react2",0},
	{SEQ_PLAY,"react2cycle",0},
	{SEQ_PLAY,"react2getup",0},
	{SEQ_END,NULL,0},
};

static SEQ scareRear[] = 
{
	{SEQ_PLAYSAMPLE, "anvilgang_12.wav", FALSE},
	{SEQ_PLAY,"react2",0},
	{SEQ_PLAY,"react2cycle",0},
	{SEQ_PLAY,"react2getup",0},
	{SEQ_END,NULL,0},
};

static SEQ tooClose[] = 
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_END,NULL,0},
};

static SEQ outOfRange[] =
{
	{SEQ_RETURN,"walk1",0},
	{SEQ_END,NULL,0},
};

/* --------------------------------------------------------------------------------
   Function : AnvilGangIdleSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int AnvilGangIdleSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = idle;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : AnvilGangAwareSeq
   Purpose : Respond to requests for aware seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int AnvilGangAwareSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = aware;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	if (bmRand()%2)
	{
		switch(bmRand()%8)
		{
		case 0:	PlaySample("anvilgang_1.wav", 255, actorInstance->actorInstance.position);	break;
		case 1:	PlaySample("anvilgang_2.wav", 255, actorInstance->actorInstance.position);	break;
		case 2:	PlaySample("anvilgang_3.wav", 255, actorInstance->actorInstance.position);	break;
		case 3:	PlaySample("anvilgang_4.wav", 255, actorInstance->actorInstance.position);	break;
		case 4:	PlaySample("anvilgang_5.wav", 255, actorInstance->actorInstance.position);	break;
		case 5:	PlaySample("anvilgang_6.wav", 255, actorInstance->actorInstance.position);	break;
		case 6:	PlaySample("anvilgang_7.wav", 255, actorInstance->actorInstance.position);	break;
		case 7:	PlaySample("anvilgang_8.wav", 255, actorInstance->actorInstance.position);	break;
		}
	}

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : AnvilGangAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int AnvilGangAttackSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	if (bmRand()%2)
	{
		switch(bmRand()%3)
		{
		case 0:	PlaySample("anvilgang_13.wav", 255, actorInstance->actorInstance.position);	break;
		case 1:	PlaySample("anvilgang_14.wav", 255, actorInstance->actorInstance.position);	break;
		case 2:	PlaySample("anvilgang_15.wav", 255, actorInstance->actorInstance.position);	break;
		}
	}

	actorInstance->genericAI->currentSeq = attack;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : AnvilGangAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int AnvilGangPursuitAttackSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = attack;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : AnvilGangTooCloseSeq
   Purpose : Respond to requests for defend seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int AnvilGangTooCloseSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = tooClose;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : AnvilGangAttackedSeq
   Purpose : Respond to requests for attacked seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int AnvilGangAttackedSeq(struct TAG_ACTORINSTANCE *actorInstance)
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
		PlaySample("anvilgang_16.wav", 255, actorInstance->actorInstance.position);
		
		gameStatus.player1->characterInfo->loseCostume = true;
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
   Function : AnvilGangOutOfRangeSeq
   Purpose : Respond to requests for out of range seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int AnvilGangOutOfRangeSeq(struct TAG_ACTORINSTANCE *actorInstance)
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
   Function : AnvilGangAttackCallback
   Purpose : Called each frame during an attack 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
int AnvilGangAttackCallback(struct TAG_ACTORINSTANCE *actorInstance)
{
	if (DistFromNodeToTaz(actorInstance,"glovestem") < gameStatus.player1->actorBehaviour->collSphere.radius)
	{
		// TP: has hit taz

		// TP: now catapult Taz
		TBVector	tempVector;
		float		tempFloat;

		bmVectorSub(tempVector, gameStatus.player1->actorInstance.position, actorInstance->actorInstance.position);
		tempFloat = bmVectorLen(tempVector);
		if (tempFloat)
		{
			bmVectorCDiv(tempVector,tempVector,tempFloat);
			tempVector[Y] = 0.5f;
			bmVectorCMul(tempVector,tempVector,METERTOUNIT(13.0f));
			bmVectorCopy(gameStatus.player1->actorBehaviour->physics.velocity, tempVector);
		}
		else
		{
			// TP: TODO throw taz in the direction the box is facing
		}

		PlaySample("whip.wav",255);

		// TP: UseStateChange callback
		RequestStateChangePlus(gameStatus.player1, STATE_PROJECTILESLIDE, myStateChangeCallback);
	}

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
	Function	: AnvilReachedDestCallback
	Purpose		: Called when the anvil has reached a position above the players head
	Parameters	: debris
	Returns		: 
	Info		: 
*/
static void AnvilReachedDestCallback(DEBRIS *debris)
{
	ACTORINSTANCE *instance;

	instance = DropObject(&map.fallingInstances,NULL,NULL,"anvil.obe",debris->position,
		FALLINGTYPE_ONCE,FALLINGTYPE_REMAINANDFADE,NULL,NULL,GetHeightOfPoint(debris->position,0),FALSE,1.0f,debris->actorStructure->actorStatus->owner,FALLINGOBJECTFLAG_CAPTURETAZ);
}

/* --------------------------------------------------------------------------------
   Function : AnvilGangMessageHandler
   Purpose : Called when a message is sent in the sequence
   Parameters : ACTORINSTANCE *, char *message
   Returns : 
   Info : 
*/
int AnvilGangMessageHandler(ACTORINSTANCE *actorInstance, char *message)
{
	float	range;

	if (strcmpi(message,"fire")==0)
	{
		AnvilGangFire(actorInstance);
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
	Function	: AnvilHitCallback
	Purpose		: Gets called when the flying anvil hits something
	Parameters	: DEBRIS, COLLINFO from collision
	Returns		: FALSE to kill debris, TRUE to allow debris to continue
	Info		: 
*/
static int AnvilHitCallback(DEBRIS *debris, COLL_INFO *collInfo)
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

	// TP: Create 

	return FALSE;
}

/*  --------------------------------------------------------------------------------
	Function	: CreateAnvil
	Purpose		: Creates a debris actor for an anvil and sets it ready to go
	Parameters	: actorInstance,initial position, initial velocity, time-life of debris
	Returns		: 
	Info		: 
*/
void CreateAnvil(ACTORINSTANCE *actorInstance, TBVector pos, TBVector velocity, float time)
{
	TBQuaternion	rotation;
	DEBRIS			*debris;
	int				flag;
	TBVector		offset;

	flag = DEBRISFLAG_GRAVITY;
	flag |= DEBRISFLAG_ROTATESPRITE|DEBRISFLAG_PHYSICS|DEBRISFLAG_STICKY;

	bmQuatCopy(rotation,bIdentityQuaternion);

	debris = CreateDebrisActor("anvil.obe", pos, velocity, rotation,time, RAD((float)(bmRand()%360)), METERTOUNIT(0.3f), METERTOUNIT(0.3f), 128, flag,0,0,RAD((float)(bmRand()%720)-360.0f));

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
		debris->actorStructure->actorStatus->owner = actorInstance;

		SETVECTOR(debris->actorInstance->scale,0.25f,0.25f,0.25f,1.0f);
		EnableActorCartoonRenderMode(debris->actorInstance);
		debris->impactCallback = AnvilHitCallback;
		debris->lifeOverCallback = AnvilReachedDestCallback;
	}
}

/* --------------------------------------------------------------------------------
   Function : AnvilGangFire
   Purpose : Called when the anvil guy sends an fireanvil command
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
void AnvilGangFire(ACTORINSTANCE *actorInstance)
{
	TBVector	tempVector,dest,velocity;
	TBVector	pos;
	TBMatrix	matrix;
	float		time,dist;

	// TP: get current position of snowball
	baGetNodesWorldPosition(&actorInstance->actorInstance, baFindNode(actorInstance->actorInstance.rootNodeInstance,"gunflex"), pos);

	// TP: get vector to player
	bmVectorSub(tempVector,gameStatus.player1->actorInstance.position,actorInstance->actorInstance.position);

	// TP: get vector(dest) in direction the character is facing
	bmQuatToMatrix(matrix,actorInstance->actorInstance.orientation);
	bmVectorSet(dest,0.0f,0.0f,-1.0f,1.0f);
	bmMatMultiplyVector(matrix,dest);

	// TP: get distance between character and taz
	dist = bmVectorLen(tempVector);

	if (!dist) return;

	if (dist<METERTOUNIT(2.0f)) dist = METERTOUNIT(2.0f);

	// TP: scale to vector (dest=character facing) to distance between characters so character shoots as far as Taz in the direction they are facing
	bmVectorScaleToLength(dest,dest,dist);

	// TP: anvil should stop 4 meters above taz's head
	dest[Y] += METERTOUNIT(4.0f);
	bmVectorAdd(dest,dest,actorInstance->actorInstance.position);

	// TP: calc velocity to get anvil there
	time = CalculateProjectileInitialVelocity(velocity, pos, dest, RAD(60.0f));

	time-=0.1f;

	CreateAnvil(actorInstance,pos,velocity,time);
}

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceAnvilGang
   Purpose : Setup the basic ai for a AnvilGang
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceAnvilGang(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList)
{
	GENERICAI	*genericAI;
	TBVector	tempVector;

	// create extra structures
	CreateActorInstanceBehaviour(actorInstance);
	CreateActorInstanceAnimation(actorInstance);
	CreateActorInstanceStatus(actorInstance);
	//CreateActorInstanceShadow(actorInstance,METERTOUNIT(0.7f),METERTOUNIT(2.5f), SHADOWMAP_RADIUS_ANVILGANG);
	// TP: CreateActorInstanceCharacterInfo(actorInstance);
	SetActorInertia(actorInstance->actorBehaviour,0);
	SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
	SetActorMaxSpeed(actorInstance->actorBehaviour,METERTOUNIT(9)); // set max speed 8mps
	SETVECTOR(tempVector,0.0f,METERTOUNIT(1.0f),0.0f,1.0f);
	SetActorCylinder(0,actorInstance->actorBehaviour,tempVector,METERTOUNIT(0.7f),METERTOUNIT(2.0f));
	AddActorToCollisionCylinderCache(&collisionCache,actorInstance);
	
	// create ai structure
	if (CreateActorInstanceGenericAI(actorInstance,activeList,inactiveList)==FALSE) return FALSE;
	
	AISetShadow(actorInstance,METERTOUNIT(0.7f),METERTOUNIT(2.5f), SHADOWMAP_RADIUS_ANVILGANG);
	// fill in custom parms
	genericAI = actorInstance->genericAI;
	bmVectorCopy(genericAI->center,actorInstance->actorInstance.position);

	genericAI->deactivationRange = METERTOUNIT(20.0f);
	genericAI->awarenessRange = METERTOUNIT(20.0f);
	genericAI->movementRange = METERTOUNIT(2.0f);
	genericAI->viewAngle = PI;
	genericAI->personalSpace = METERTOUNIT(0.5f);
	genericAI->standOffRange = METERTOUNIT(2.2f);
	genericAI->attackHoldDistance = METERTOUNIT(2.0f);
	genericAI->hearRange = METERTOUNIT(0.0f);

	genericAI->instantAttack = true;

	genericAI->currentRotTime = RAD(90.0f)/60.0f;

	genericAI->headNode = "spinetop";
	SETVECTOR(genericAI->starsOffset,0.0f,METERTOUNIT(0.5f),0.0f,1.0f);
	SETVECTOR(genericAI->starVars,METERTOUNIT(0.3f),METERTOUNIT(0.4f),METERTOUNIT(0.0f),METERTOUNIT(0.0f));

	SetAISeqFunctions(actorInstance,AnvilGangIdleSeq,AnvilGangAwareSeq,AnvilGangAttackSeq,
		AnvilGangTooCloseSeq,AnvilGangAttackedSeq,AnvilGangOutOfRangeSeq,0,AnvilGangPursuitAttackSeq);

	SetAIAttackCallback(actorInstance,AnvilGangAttackCallback);
	SetAIChangeStateCallback(actorInstance,myChangeState);
	SetAIMessageHandler(actorInstance,AnvilGangMessageHandler);

	genericAI->runCRC = bkCRC32((uchar*)"run",3,0);

	return TRUE;
}
