// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : zookeeper.cpp
//   Purpose : ai functions for zookeeper
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "genericai.h"
#include "collectibles.h"
#include "util.h"
#include "maths.h"
#include "status.h"
#include "animation.h"
#include "collisioncache.h"
#include "scene.h"
#include "main.h"
#include "characters.h"
#include "physics.h"
#include "zookeeper.h"
#include "control.h"
#include "sfx.h"

// PP: TEMP?: radius of Zoo Keeper for shadow map
// PP: TODO (how?): get bounds of actor instances on the fly so's to optimise shadow map resolution
#define SHADOWMAP_RADIUS_ZOOKEEPER			METERTOUNIT(5.5)

/* ANIM LIST
Animation Name	Notes	Frame Start/End	Sound Effects
Run				10 - 70	Bouncing footsteps (20 & 50)
Attack1			71 - 142	"thwack" with net (90)
Dragonground1	Bounce crapply	143 - 183	Bounce thud (160)
Walk1				184 - 264	Soft prowling footsteps (224 & 264)
Sleepcycle		265 - 465	Gentle snore
Wakeup		465 - 543	Startled (469) & shake head (498-516)
Idle1	Breathe	544 - 644	
Idle2	Look around	644 - 738	
Netidle	Pull at taz	739 - 829	
React1	Bite from rear	830 - 880	"yeeoow!"
React1run		880 - 940	Bouncing footsteps (905 & 935)
React2	Bite from front	941 - 1025	"yeeoow!"
React2run	Run backwards	1026 - 1086	Bouncing footsteps (1051 & 1081)
React3	Scared jump	1087 - 1150	Shriek & landing thud (1130)
React3cycle	Cringe cycle	1150 - 1171	Soft crying
Death1	Spun off screen	1172 - 1192	Whirling wind & yell
Dragonground2	Standing skid	1193 - 1233	Intermittent screech (1228)
Death2	KO from front	1234 - 1270	"oof!" & landing thud (1245 & 1260)
Recover2	Ninja backflip to stand	1271 - 1366	Grunt of effort (1296)
Death3	KO from behind	1366 - 1402	"oof!" & landing thud (1378 & 1392)
Recover3	Get up	1403 - 1490	Groan with effort
Disguise	See taz in costume	1490 - 1623	Puzzled sound "huh?"
Sidestepleft	Sidle left	1624 - 1664	
Sidestepright	Sidle right	1665 - 1705	
Tazbounce1	Bounce for "React3cycle"	1706 - 1735	
Tazbounce2	Bounce for "death2"	1736 - 1766	
Tazbounce3	Bounce for "death3"	1767 - 1797	
Attack2	Lunge attack		
Fromdraggedtostanding	Go from "dragonground" anim to "netidle" anim		
Attack2fail	Miss taz with net		
Idle3	Air Rock!	
Edgeofterrain	

*/

// TP: ------------------------------------------------ SCOUT
static SEQ scoutidle[] = 
{
	{SEQ_FINDNEWCENTER,NULL,0},
	{SEQ_RETURN,"walk1",0},
	{SEQ_WALK,"walk1",1},
	{SEQ_PLAYSAMPLE,"keeper_18.wav",FALSE},
	{SEQ_PLAY,"idle2",0},
	{SEQ_PLAY,"idle1",4},
	{SEQ_TURNLEFT,NULL,180},
	{SEQ_WALK,"walk1",1},
	{SEQ_PLAY,"idle2",0},
	{SEQ_PLAY,"idle3",0},
	{SEQ_TURNLEFT,NULL,180},
	{SEQ_FINDNEWCENTER,NULL,0},
	{SEQ_RETURN,"walk1",0},
	{SEQ_WALK,"walk1",1},
	{SEQ_PLAYSAMPLE,"keeper_18.wav",FALSE},
	{SEQ_PLAY,"idle2",0},
	{SEQ_PLAY,"idle1",4},
	{SEQ_TURNLEFT,NULL,180},
	{SEQ_WALK,"walk1",1},
	{SEQ_PLAY,"idle2",0},
	{SEQ_PLAY,"idle1",4},
	{SEQ_TURNLEFT,NULL,180},
	{SEQ_END,NULL,0},
};

static SEQ scoutaware[] =
{
	{SEQ_FACETAZ, NULL, 0},
	{SEQ_PLAY,"idle2",0},
	{SEQ_PLAY,"idle1",3},
	{SEQ_FINDNEWCENTER,NULL,0},
	{SEQ_RETURN,"walk1",0},
	{SEQ_PLAYSAMPLE,"keeper_19.wav",FALSE},
	{SEQ_END,NULL,0},
};

static SEQ scoutattack[] = 
{
	// TP: pursue
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,true},
	{SEQ_WAIT,NULL,0},
	{SEQ_WAIT,NULL,0},
	{SEQ_CHASE,"run",9},
	{SEQ_PLAYSAMPLE,"zkthwack.wav",FALSE},

	{SEQ_LOCKSEQ, NULL, true},
	{SEQ_NODEACTIVATE, NULL, true},
	{SEQ_MESSAGE, "attack1", 0},		// TP: do net attack
	{SEQ_LOCKSEQ, NULL, false},
	{SEQ_NODEACTIVATE, NULL, false},
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},
	{SEQ_END,NULL,0},

	{SEQ_PLAY,"edgeofterrain",0},	// TP: give up after chase
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},
	{SEQ_END,NULL,0},
};

static SEQ spinFront[] =
{
	{SEQ_FACETAZ,NULL,0},
	{SEQ_SETCYLRADIUS,NULL,METERTOUNIT(1.1f)},
	{SEQ_PLAYSAMPLE,"keeper_13.wav",TRUE},
	{SEQ_PLAY,"death2",0},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_DISABLEDANIM, "tazbounce2", 0},
	{SEQ_DISABLED,NULL,10.0f},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAYSAMPLE,"keeper_14.wav",FALSE},
	{SEQ_PLAY,"recover2",0},
	{SEQ_STOPSAMPLE,NULL,0},
	{SEQ_END,NULL,0},
};

static SEQ spinRear[] =
{
	//{SEQ_LOOKAWAYFROMTAZ,NULL,0},
	{SEQ_PLAYSAMPLE,"keeper_15.wav",TRUE},
	{SEQ_SETCYLRADIUS,NULL,METERTOUNIT(1.1f)},
	{SEQ_PLAY,"death3",0},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_DISABLEDANIM, "tazbounce3", 0},
	{SEQ_DISABLED,NULL,10.0f},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_END,NULL,0},
};

static SEQ biteFront[] =
{
	{SEQ_PLAYSAMPLE,"keeper_9.wav",FALSE},
	{SEQ_PLAY,"react3",0},
	{SEQ_PLAYSAMPLE,"keeper_12.wav",TRUE},
	{SEQ_SETCYLRADIUS,NULL,METERTOUNIT(1.1f)},
	{SEQ_DISABLEDANIM, "tazbounce3", 0},
	{SEQ_DISABLED,"react3cycle",10.0f},
	{SEQ_STOPSAMPLE,NULL,0},
	{SEQ_PLAYSAMPLE,"keeper_16.wav",FALSE},
	{SEQ_PLAY,"recover3",0},
	{SEQ_END,NULL,0},
};

static SEQ biteRear[] =
{
	{SEQ_PLAYSAMPLE,"keeper_10.wav",FALSE},
	{SEQ_PLAY,"react3",0},
	{SEQ_DISABLEDANIM,"tazbounce1",0},
	{SEQ_DISABLED,"react3cycle",10.0f},
	{SEQ_PLAY,"recover3",0},
	{SEQ_END,NULL,0},
};

static SEQ scareFront[] =
{
	{SEQ_PLAYSAMPLE,"keeper_11.wav",FALSE},
	{SEQ_SETCYLRADIUS,NULL,METERTOUNIT(1.1f)},
	{SEQ_PLAY,"react3",0},
	{SEQ_PLAYSAMPLE,"keeper_12.wav",TRUE},
	{SEQ_DISABLEDANIM, "tazbounce1", 0},
	{SEQ_DISABLED,"react3cycle",10.0f},
	{SEQ_STOPSAMPLE,NULL,0},
	{SEQ_PLAYSAMPLE,"keeper_16.wav",FALSE},
	{SEQ_PLAY,"recover3",0},
	{SEQ_END,NULL,0},
};

static SEQ scareRear[] = 
{
	{SEQ_PLAYSAMPLE,"keeper_11.wav",FALSE},
	{SEQ_SETCYLRADIUS,NULL,METERTOUNIT(1.1f)},
	{SEQ_PLAY,"react3",0},
	{SEQ_PLAYSAMPLE,"keeper_12.wav",TRUE},
	{SEQ_DISABLEDANIM, "tazbounce1", 0},
	{SEQ_DISABLED,"react3cycle",10.0f},
	{SEQ_STOPSAMPLE,NULL,0},
	{SEQ_PLAYSAMPLE,"keeper_16.wav",FALSE},
	{SEQ_PLAY,"recover3",0},
	{SEQ_END,NULL,0},
};

static SEQ costumeAttack[] =
{
	{SEQ_LOOKAWAYFROMTAZ,NULL,0},
	{SEQ_PLAYSAMPLE,"keeper_13.wav",FALSE},
	{SEQ_SETCYLRADIUS,NULL,METERTOUNIT(1.1f)},
	{SEQ_PLAY,"death3",0},
	{SEQ_DEATH,NULL,0},
	{SEQ_END,NULL,0},
};

static SEQ tooClose[] = 
{
	{SEQ_FACETAZ,"walk1",0},
	{SEQ_PLAYSAMPLE,"zkthwack.wav",FALSE},
	//{SEQ_ATTACK,"attack1",0},
	{SEQ_END,NULL,0},
};

static SEQ outOfRange[] =
{
	{SEQ_RETURN,"walk1",0},
	{SEQ_END,NULL,0},
};

/*	--------------------------------------------------------------------------------
	Function 	: SetPositionForTazTrapped
	Purpose 	: Place Taz in correct position and orientation from zookeeper
	Parameters 	: player,zookeeper
	Returns 	: 
	Info 		: 
*/
void SetPositionForTazTrapped(ACTORINSTANCE *player,ACTORINSTANCE *zookeeper)
{
	TBVector tempVector,vec;
	TBMatrix matrix;
	float	y;

	// set correct position
	bmVectorCopy(tempVector,zookeeper->actorInstance.position);

	bmQuatToMatrix(matrix,zookeeper->actorInstance.orientation);
	SETVECTOR(vec,0.0f,0.0f,METERTOUNIT(-1.788),1.0f);
	bmMatMultiplyVector(matrix,vec);
	
	y = player->actorInstance.position[Y];
	bmVectorAdd(player->actorInstance.position,zookeeper->actorInstance.position,vec);
	player->actorInstance.position[Y] = y;

	// set orientation
	SetActorOrientation(player,zookeeper->actorInstance.orientation);

	ZEROVECTOR(player->actorBehaviour->physics.velocity);
}

/* --------------------------------------------------------------------------------
   Function : ZookeeperIdleSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ZookeeperIdleSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	ZOOKEEPERINFO *zooInfo = (ZOOKEEPERINFO*)actorInstance->genericAI->customData;

	actorInstance->genericAI->currentSeq = scoutidle;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ZookeeperAwareSeq
   Purpose : Respond to requests for aware seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ZookeeperAwareSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	ZOOKEEPERINFO *zooInfo = (ZOOKEEPERINFO*)actorInstance->genericAI->customData;

	if (CheckIfPlayingAnimation(actorInstance,"sleepcycle")) 
	{
		return FALSE;
	}

	actorInstance->genericAI->currentSeq = scoutaware;

	if (bmRand()%2)
	{
		switch(bmRand()%3)
		{
		case 0:	PlaySample("keeper_1.wav", 255, actorInstance->actorInstance.position);	break;
		case 1:	PlaySample("keeper_2.wav", 255, actorInstance->actorInstance.position);	break;
		case 2:	PlaySample("keeper_3.wav", 255, actorInstance->actorInstance.position);	break;
		}
	}

	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ZookeeperAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ZookeeperAttackSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	ZOOKEEPERINFO *zooInfo = (ZOOKEEPERINFO*)actorInstance->genericAI->customData;

	if (CheckIfPlayingAnimation(actorInstance,"sleepcycle")&&(gameStatus.player1->actorStatus->currentState==STATE_TIPTOE))
	{
		return FALSE;
	}
	
	actorInstance->genericAI->currentSeq = scoutattack;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ZookeeperAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ZookeeperPursuitAttackSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = scoutattack;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ZookeeperTooCloseSeq
   Purpose : Respond to requests for defend seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ZookeeperTooCloseSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = tooClose;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}


/* --------------------------------------------------------------------------------
   Function : ZookeeperCostumeSeq
   Purpose : Respond to requests for defend seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ZookeeperCostumeSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = scoutaware;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}


/* --------------------------------------------------------------------------------
   Function : ZookeeperAttackedSeq
   Purpose : Respond to requests for attacked seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ZookeeperAttackedSeq(struct TAG_ACTORINSTANCE *actorInstance)
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
		if (gameStatus.player1->characterInfo->powerUpInfo.invisible)
		{
			actorInstance->genericAI->currentSeq = biteFront;
		}
		else
		{
			actorInstance->genericAI->currentSeq = scoutattack;
		}
		break;
	case ATTACKTYPE_SPINFRONT:
		if (gameStatus.player1->characterInfo->powerUpInfo.invisible)
		{
			actorInstance->genericAI->currentSeq = spinFront;
		}
		else
		{
			actorInstance->genericAI->currentSeq = scoutattack;
		}
		break;
	case ATTACKTYPE_SCAREFRONT:
		if (gameStatus.player1->characterInfo->powerUpInfo.invisible)
		{
			actorInstance->genericAI->currentSeq = scareFront;
		}
		else
		{
			actorInstance->genericAI->currentSeq = scoutattack;
		}
		break;
	case ATTACKTYPE_BITEREAR:
		actorInstance->genericAI->currentSeq = biteRear;
		break;
	case ATTACKTYPE_COSTUMEFRONT:
	case ATTACKTYPE_COSTUMEREAR:
		actorInstance->genericAI->currentSeq = costumeAttack;
		if(!actorInstance->actorStatus->costumeChangeInfo) 
					actorInstance->actorStatus->costumeChangeInfo = CreateLoseCostumeEffect(actorInstance, "spinemid", 2.0f);
		gameStatus.player1->characterInfo->loseCostume = true;
		break;
	case ATTACKTYPE_SPINREAR:
	case ATTACKTYPE_SPITFRONT:
	case ATTACKTYPE_SPITREAR:
		actorInstance->genericAI->currentSeq = spinRear;
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
   Function : ZookeeperOutOfRangeSeq
   Purpose : Respond to requests for out of range seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ZookeeperOutOfRangeSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = outOfRange;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

enum 
{
	KEEPER_STARTATTACK,
	KEEPER_ATTACK,
	KEEPER_GOTHIM,
	KEEPER_HALT,
	KEEPER_TOJAIL,
	KEEPER_GIVEUP,
};

/* --------------------------------------------------------------------------------
   Function : NetControl
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/
void NetControl(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance)
{
	float			angleOfMovement;
	float			xRot,zRot;
	TBVector		tempVector,clipVector;
	float			vel;
	float			size;

	angleOfMovement = ConvertFromCameraToGlobalRotation(cameraInfo,0.0f);

	if (controller->mag1)
	{
		xRot = controller->y1 * (PI/6.0f);
		zRot = controller->x1 * (PI/6.0f);
	}
	else
	{
		xRot = 0.0f;
		zRot = 0.0f;
	}

	actorInstance->actorStatus->targetSpinSpeed = 0.0f;

	// calculate speed
	if (controller->mag1)
	{
		tempVector[X] = controller->x1;
		tempVector[Y] = 0.0f;
		tempVector[Z] = -controller->y1;
		tempVector[W] = 1.0f;

		// calculate direction vector
		bmVectorRotateY(tempVector,tempVector,angleOfMovement);

		// set acceleration speed controller mag * 3
		// only add speed if on ground
		if ((actorInstance->actorStatus->timeWithoutGroundCollision < 0.15f)||(actorInstance->actorStatus->timeWithoutWallCollision < 0.15f))
		{
			if(actorInstance->actorStatus->targetSpinSpeed)
			{
				vel = actorInstance->actorStatus->targetSpinSpeed;
			}
			
			if((!actorInstance->actorStatus->targetSpinSpeed) || (actorInstance->actorStatus->multiplayer.effect == PLAYEREFFECTSLOMO))
			{
				vel = controller->mag1*METERTOUNIT(0.2f)*speedRatio;
			}

			bmVectorNorm(tempVector,tempVector);
			bmVectorCMul(clipVector,tempVector,vel);


			bmVectorAdd(tempVector,clipVector,actorInstance->actorBehaviour->physics.velocity);
			vel = bmVectorLen(tempVector);
			if ((vel > actorInstance->actorBehaviour->physics.speed)&&(vel > actorInstance->actorBehaviour->physics.maxSpeed)&&(!actorInstance->actorStatus->targetSpinSpeed))
			{
				if (actorInstance->actorBehaviour->physics.speed > actorInstance->actorBehaviour->physics.maxSpeed)
				{
					bmVectorScaleToLength(actorInstance->actorBehaviour->physics.velocity,tempVector,actorInstance->actorBehaviour->physics.speed);
				}
				else
				{
					bmVectorScaleToLength(actorInstance->actorBehaviour->physics.velocity,tempVector,actorInstance->actorBehaviour->physics.maxSpeed);
				}
			}
			else
			{
				bmVectorCopy(actorInstance->actorBehaviour->physics.velocity,tempVector);
				actorInstance->actorBehaviour->physics.speed = vel;
			}
		}
	}
}

#define FIXED_ATTACK_RANGE (METERTOUNIT(1.788f))

/*  --------------------------------------------------------------------------------
	Function	: ProcessCharacterInNet
	Purpose		: Control for the Taz after he has been netted
	Parameters	: Taz
	Returns		: 
	Info		: 
*/

void ProcessCharacterInNet(ACTORINSTANCE *actorInstance)
{
	if(RespondToStateInit(actorInstance))
	{
		bkPrintf("Enter Net\n");
		SetActorMinFricAngle(actorInstance->actorBehaviour,1.0f);
		actorInstance->actorAnimation->useMotionBone = false;
	}
	if(RespondToStateExit(actorInstance))
	{
		bkPrintf("Exit Net\n");
		SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
		actorInstance->actorAnimation->useMotionBone = true;
		RespondToStateExitPost(actorInstance, actorInstance->actorStatus);
		return;
	}

	NetControl(gameStatus.player[0].controller, gameStatus.player[0].camera, gameStatus.player[0].actorInstance);
}

/*  --------------------------------------------------------------------------------
	Function : TrapTaz
	Purpose : Called when Taz gets hit with the Net, this routines decides if its safe to net taz
	Parameters : ACTORINSTANCE of character doing the netting
	Returns : true/false
	Info : 
*/

static bool TrapTaz(ACTORINSTANCE *actorInstance)
{
	TBVector	tempVector;
	GENERICAI	*genericAI = actorInstance->genericAI;
	ZOOKEEPERINFO	*zooInfo = (ZOOKEEPERINFO*)actorInstance->genericAI->customData;

	bmVectorSub(tempVector, actorInstance->actorInstance.position, gameStatus.player1->actorInstance.position);
	bmVectorScaleToLength(tempVector, tempVector, FIXED_ATTACK_RANGE);
	bmVectorAdd(actorInstance->actorInstance.position, gameStatus.player1->actorInstance.position, tempVector);
	if (actorInstance->genericAI->useMoveBox)
	{
		if (ClipPositionToBox(&actorInstance->genericAI->moveBox, actorInstance->actorInstance.position))
		{
			// TP: had to clip zookeepers position, this could mean trouble
			return false;
		}
	}
	else
	{
		if (genericAI->distToCenter > genericAI->movementRange)
		{
			TBVector	tempVector;
			bmVectorScaleToLength(tempVector,genericAI->toCenter,genericAI->movementRange);
			bmVectorSub(tempVector,genericAI->center,tempVector);
			actorInstance->actorInstance.position[X] = tempVector[X];
			actorInstance->actorInstance.position[Z] = tempVector[Z];

			// TP: had to clip zookeepers position, this could mean trouble
			return false;
		}
	}

	return true;
}


/*  --------------------------------------------------------------------------------
	Function	: AnotherZookeeperAttacking
	Purpose		: Test to see if any other zookeepers are currently in attack position
	Parameters	: zookeeper doing test
	Returns		: true/false
	Info		: 
*/

bool AnotherZookeeperAttacking(ACTORINSTANCE *actorInstance)
{
	ACTORINSTANCE *ptr;

	for (ptr = map.characterInstances.head.next; ptr != &map.characterInstances.head; ptr = ptr->next)
	{
		if (ptr==actorInstance) continue;

		// PP: I was getting a null pointer crash, where ptr was a parrot (?)
		if(ptr->genericAI != NULL)
		{
			// TP: test to see if character is a zookeeper
			if (ptr->genericAI->currentSeq == scoutattack)
			{
				if ((ptr->genericAI->currentSeq)&&(ptr->genericAI->currentSeq[ptr->genericAI->seqIdx].type == SEQ_MESSAGE))
				{
					if (strcmpi(ptr->genericAI->currentSeq[ptr->genericAI->seqIdx].anim, "attack1")==0) return true;
				}
			}
		}
	}

	return false;
}


/* --------------------------------------------------------------------------------
   Function : ZooKeeperMessageHandler
   Purpose : Called when a message is sent in the sequence
   Parameters : ACTORINSTANCE *, char *message
   Returns : 
   Info : 
*/

#define ORIEN_NET

int ZooKeeperMessageHandler(ACTORINSTANCE *actorInstance, char *message)
{
	ZOOKEEPERINFO		*zooInfo = (ZOOKEEPERINFO*)actorInstance->genericAI->customData;
	GENERICAI			*genericAI = actorInstance->genericAI;
	TBVector			tempVector, p1, p2;
	bool				outOfRange = false;
	float				tempFloat;
	TBActorNodeInstance	*node;

	if (strcmpi(message, "attack1")==0)
	{
		switch(zooInfo->state)
		{
		case KEEPER_STARTATTACK:
			// TP: start attack move
			bmVectorAdd(p1, gameStatus.player[0].actorInstance->actorInstance.position, gameStatus.player[0].actorInstance->actorBehaviour->collSphere.offset);
			bmVectorAdd(p2, actorInstance->actorInstance.position, actorInstance->actorBehaviour->collSphere.offset);

			if ((!SafeToAttackTaz(gameStatus.player1))||(AnotherZookeeperAttacking(actorInstance))||RayTest(&collisionCache, p1, p2))
			{
				// TP: no clear route
				return false;
			}

			FlushAnimationQueue(actorInstance);
			PlayAnimationByName(actorInstance, "attack2", 1.0f, 0, 0, 0.0f, NONE);
			zooInfo->state = KEEPER_ATTACK;
			break;

		case KEEPER_ATTACK:
			// TP: taz is in net and being dragged
			FacePoint(actorInstance,gameStatus.player1->actorInstance.position,0.0f);

			// TP: wait for zookeeper hit message
			if (!CheckIfPlayingAnimation(actorInstance, "attack2"))
			{
				PlayAnimationByName(actorInstance, "attack2fail", 1.0f, 0, 0, 0.1f, NONE);
				zooInfo->state = KEEPER_GIVEUP;
			}
			break;

		case KEEPER_GOTHIM:
			// TP: orientate node to point at taz
#ifdef ORIEN_NET
			node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "base");
			
			if (node)
			{
				baNodeLookAt(&actorInstance->actorInstance, node, gameStatus.player1->actorInstance.position, 0, PI, 
					HALFPI, -RAD(20.0f), RAD(20.0f), 0.0f, 1, BNODELOOKAT_PREPCHILDREN);
			}
#endif

			// TP: taz is in net and being dragged
			FacePoint(actorInstance,gameStatus.player1->actorInstance.position,0.0f);

			// TP: Increase drag factor
			zooInfo->dragFactor -= fTime/1.5f;
			if (zooInfo->dragFactor < 0.0f) zooInfo->dragFactor = 0.0f;

			// TP: Apply drag to character
			bmVectorCMul(gameStatus.player1->actorBehaviour->physics.velocity, gameStatus.player1->actorBehaviour->physics.velocity, zooInfo->dragFactor);
			
			// TP: help stop taz from falling of the sides of cliffs while trapped
			gameStatus.player1->actorBehaviour->physics.velocity[Y] = 0.0f;

			// TP: keep zookeeper a set distance from taz, in y direction only
			bmVectorSub(tempVector, actorInstance->actorInstance.position, gameStatus.player1->actorInstance.position);
			bmVectorScaleToLength(tempVector, tempVector, FIXED_ATTACK_RANGE);

			// TP: keep y coord the same, so keeper does not get pulled through ground
			tempFloat = actorInstance->actorInstance.position[Y];
			bmVectorAdd(actorInstance->actorInstance.position, gameStatus.player1->actorInstance.position, tempVector);
			actorInstance->actorInstance.position[Y] = tempFloat;

			// TP: now the zookeeper has moved re-clip the zookeepers position against his box
			if (actorInstance->genericAI->useMoveBox)
			{
				ClipPositionToBox(&actorInstance->genericAI->moveBox, actorInstance->actorInstance.position);
			}
			else
			{
				if (genericAI->distToCenter > genericAI->movementRange)
				{
					TBVector	tempVector;
					bmVectorScaleToLength(tempVector,genericAI->toCenter,genericAI->movementRange);
					bmVectorSub(tempVector,genericAI->center,tempVector);
					actorInstance->actorInstance.position[X] = tempVector[X];
					actorInstance->actorInstance.position[Z] = tempVector[Z];
				}
			}

			// TP: Just in case players position was clipped
			bmVectorSub(gameStatus.player1->actorInstance.position, actorInstance->actorInstance.position, tempVector);

			// TP: monitor speed and choose appropriate animations
			if (!CheckIfPlayingAnimation(actorInstance, "fromdraggedtostanding"))
			{
				if (gameStatus.player1->actorBehaviour->physics.speed > METERTOUNIT(0.5f))
				{
					if (!CheckIfPlayingAnimation(actorInstance, "dragonground2"))
					{
						PlayAnimationByName(actorInstance, "dragonground2", 1.0f, 1, 0, 0.5f, NONE);
					}
				}
				else
				{
					if (!CheckIfPlayingAnimation(actorInstance, "netidle"))
					{
						PlayAnimationByName(actorInstance, "netidle", 1.0f, 1, 0, 0.5f, NONE);
					}
				}
			}

			// TP: timeout
			zooInfo->timeWithTazTrapped += fTime;
			if (zooInfo->timeWithTazTrapped > 2.0f)
			{
				zooInfo->state = KEEPER_HALT;
			}
			break;

		case KEEPER_HALT:
			// TP: taz has slowed down to a halt
#ifdef ORIEN_NET
			node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "base");
			
			if (node)
			{
				baNodeLookAt(&actorInstance->actorInstance, node, gameStatus.player1->actorInstance.position, 0, PI, 
					HALFPI, -RAD(20.0f), RAD(20.0f), 0.0f, 1, BNODELOOKAT_PREPCHILDREN);
			}
#endif

			// TP: taz is in net and being dragged
			FacePoint(actorInstance,gameStatus.player1->actorInstance.position,0.0f);

			bmVectorZero(gameStatus.player1->actorBehaviour->physics.velocity);

			// TP: dragon
			// TP: keep set distance
			bmVectorSub(tempVector, actorInstance->actorInstance.position, gameStatus.player1->actorInstance.position);
			bmVectorScaleToLength(tempVector, tempVector, FIXED_ATTACK_RANGE);
			bmVectorAdd(actorInstance->actorInstance.position, gameStatus.player1->actorInstance.position, tempVector);
			if (actorInstance->genericAI->useMoveBox)
			{
				ClipPositionToBox(&actorInstance->genericAI->moveBox, actorInstance->actorInstance.position);
			}
			else
			{
				if (genericAI->distToCenter > genericAI->movementRange)
				{
					TBVector	tempVector;
					bmVectorScaleToLength(tempVector,genericAI->toCenter,genericAI->movementRange);
					bmVectorSub(tempVector,genericAI->center,tempVector);
					actorInstance->actorInstance.position[X] = tempVector[X];
					actorInstance->actorInstance.position[Z] = tempVector[Z];
				}
			}

			// TP: Just in case players position was clipped
			bmVectorSub(gameStatus.player1->actorInstance.position, actorInstance->actorInstance.position, tempVector);

			switch (camera[0].fade.GetFadeState())
			{
			case FADESTATE_ON:
				zooInfo->state = KEEPER_TOJAIL;

#ifdef ORIEN_NET
				TBActorNodeInstance	*node;
				node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "base");

				if (node)
				{
					node->flags |= BNODEINSTFLAG_ORIENTKEYS;
				}
#endif
				break;
				
			case FADESTATE_OFF:
				gameStatus.player[0].camera->fade.SetSwipeDirection(RAD(90), RAD(-90));
				camera[0].fade.StartFadeDown(FADETYPE_SWIPE, 1.0f);
				break;
			}
			break;
			
		case KEEPER_TOJAIL:
			// TP: Send Taz to jail

			CaptureTaz(actorInstance);
			RequestStateChange(gameStatus.player1, STATE_MOVE);

			zooInfo->state = KEEPER_STARTATTACK;
			return false;
			break;

		case KEEPER_GIVEUP:
			// TP: Taz escaped
			if (!CheckIfPlayingAnimation(actorInstance, "death2") && !CheckIfPlayingAnimation(actorInstance, "recover2")&& !CheckIfPlayingAnimation(actorInstance, "attack2fail"))
			{
				zooInfo->state = KEEPER_STARTATTACK;
				return false;
			}
			else
			{
				return true;
			}
			break;
		}

		return true;
	}

	if ((strcmpi(message,"keeperhit")==0)&&SafeToAttackTaz(gameStatus.player1))
	{
		if (DistFromNodeToTaz(actorInstance, "handle") < (gameStatus.player1->actorBehaviour->collSphere.radius + METERTOUNIT(0.45f)))
		{
			// TP: sort out Taz
			if (RequestStateChange(gameStatus.player[0].actorInstance, STATE_NET))
			{
				if (TrapTaz(actorInstance))
				{
					if (!UpdateCharacterState(&gameStatus.player[0], true))
					{
						// TP: its not safe to capture taz so cancel state request
						gameStatus.player[0].actorInstance->actorStatus->stateRequest = (ECharState)-1;
						gameStatus.player[0].actorInstance->actorStatus->stateRequestCallback = NULL;
					}
					else
					{
#ifndef CONSUMERDEMO
						if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].levelBounty > 0 && map.sceneNumber != SCENE_ZOOHUB)
						{
							char	tempString[16], cashString[16];

							switch(gameStatus.globalSettings.difficulty)
							{
							case DIFF_EASY:		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].levelBounty < 500)
													utilFormatPrettyNumbers(gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].levelBounty, tempString);	
												else
													utilFormatPrettyNumbers(500, tempString);
													
												break;

							case DIFF_MEDIUM:	if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].levelBounty < 700)
													utilFormatPrettyNumbers(gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].levelBounty, tempString);	
												else
													utilFormatPrettyNumbers(700, tempString);
													
												break;

							case DIFF_HARD:		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].levelBounty < 1000)
													utilFormatPrettyNumbers(gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].levelBounty, tempString);	
												else
													utilFormatPrettyNumbers(1000, tempString);
													
												break;
							}

							sprintf(cashString, "-$%s", tempString);
						
							CreateScreenScoreMember(gameStatus.player1->actorInstance.position, cashString, SCREENSCORE_PENALTY);
						}
#endif

						// TP: sort out keeper
						PlayAnimationByName(actorInstance, "fromdraggedtostanding", 1.0f, 0, 0, 0.1f, NONE);
						zooInfo->state = KEEPER_GOTHIM;
						zooInfo->timeWithTazTrapped = 0.0f;

#ifdef ORIEN_NET
						TBActorNodeInstance	*node;
						node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "base");

						if (node)
						{
							node->flags &= ~BNODEINSTFLAG_ORIENTKEYS;
						}
#endif

						// TP: taz
						FlushAnimationQueue(gameStatus.player1);
						PlayAnimationByName(gameStatus.player1,"trapped",1.0f,1,0,0.1f,NONE);
						zooInfo->dragFactor = 1.0f;
					}
				}
				else
				{
					// TP: handle not being able to enter state
					gameStatus.player[0].actorInstance->actorStatus->stateRequest = (ECharState)-1;
					gameStatus.player[0].actorInstance->actorStatus->stateRequestCallback = NULL;
				}
			}
			else
			{
				// TP: handle not being able to enter state
				gameStatus.player[0].actorInstance->actorStatus->stateRequest = (ECharState)-1;
				gameStatus.player[0].actorInstance->actorStatus->stateRequestCallback = NULL;
			}

		}
		return true;
	}
	return false;
}

/*  --------------------------------------------------------------------------------
	Function	: KeeperGoAware
	Purpose		: Test to see if the zookeeper should become aware
	Parameters	: zookeeper instance
	Returns		: true - go aware / false - dont
	Info		: 
*/

static bool KeeperGoAware(ACTORINSTANCE *actorInstance)
{
	GENERICAI	*genericAI = actorInstance->genericAI;

	if (gameStatus.player1->characterInfo->costume!=-1)
	{
		// TP: taz in costume
		if ((bmFAbs(genericAI->facingRelToTaz) > genericAI->awareAngle)&&(genericAI->distToTaz > genericAI->standOffRange)) return false;
	}

	if (IsPlayerQuiet(gameStatus.player1))
	{
		if (bmFAbs(genericAI->facingRelToTaz) > genericAI->awareAngle) return false;
	}

	if (!IsTazWithinHeightRange(actorInstance)) return false;

	if (genericAI->useMoveBox)
	{
		if (IsPointWithinBox(&genericAI->moveBox.attackBBox,genericAI->moveBox.invmatrix,gameStatus.player1->actorInstance.position,BOXFLAG_XTEST|BOXFLAG_ZTEST)||(genericAI->flags&GENERICAI_NOBOUNDARY))
		{
			return true;
		}
	}
	else
	{
		if ((genericAI->distTazToCenter < (genericAI->awarenessRange+genericAI->attackBoundaryRange))||(genericAI->flags&GENERICAI_NOBOUNDARY)) return true;
	}

	return false;
}

/*  --------------------------------------------------------------------------------
	Function	: KeeperGoAttack
	Purpose		: Test to see if the zookeeper should attack
	Parameters	: zookeeper instance
	Returns		: true - attack / false - dont
	Info		: 
*/

static bool KeeperGoAttack(ACTORINSTANCE *actorInstance)
{
	GENERICAI			*genericAI = actorInstance->genericAI;
	TBVector			p1, p2;
	bool				retVal = false;

	if (gameStatus.player1->characterInfo->costume!=-1)
	{
		// TP: taz in costume
		if ((bmFAbs(genericAI->facingRelToTaz) > genericAI->awareAngle)&&(genericAI->distToTaz > genericAI->movementRange)) return false;
	}

	if (IsPlayerQuiet(gameStatus.player1))
	{
		if (bmFAbs(genericAI->facingRelToTaz) > genericAI->awareAngle) return false;
	}

	if (bmFAbs(actorInstance->actorInstance.position[Y] - gameStatus.player1->actorInstance.position[Y]) > METERTOUNIT(0.5f)) return false;


	if (genericAI->useMoveBox)
	{
		if (IsPointWithinBox(&genericAI->moveBox.attackBBox,genericAI->moveBox.invmatrix,gameStatus.player1->actorInstance.position,BOXFLAG_XTEST|BOXFLAG_ZTEST)||(genericAI->flags&GENERICAI_NOBOUNDARY))
		{
			retVal = true;
		}
	}
	else
	{
		if ((genericAI->distTazToCenter < (genericAI->movementRange+genericAI->attackBoundaryRange))||(genericAI->flags&GENERICAI_NOBOUNDARY)) retVal = true;
	}

	// TP: maybe limit range of test
	bmVectorAdd(p1, gameStatus.player[0].actorInstance->actorInstance.position, gameStatus.player[0].actorInstance->actorBehaviour->collSphere.offset);
	bmVectorAdd(p2, actorInstance->actorInstance.position, actorInstance->actorBehaviour->collSphere.offset);

	if (RayTest(&collisionCache, p1, p2))
	{
		// TP: no clear route
		return false;
	}

	return retVal;
}

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceZookeeper
   Purpose : Setup the basic ai for a zookeeper
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceZookeeper(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList, EZookeeperType type, uint32 defend)
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
	SetActorCylinder(0,actorInstance->actorBehaviour,tempVector,METERTOUNIT(0.9f),METERTOUNIT(2.0f));
	AddActorToCollisionCylinderCache(&collisionCache,actorInstance);
	
	// create ai structure
	if (CreateActorInstanceGenericAI(actorInstance,activeList,inactiveList)==FALSE) return FALSE;

	AISetShadow(actorInstance,METERTOUNIT(0.7f),METERTOUNIT(2.5f), SHADOWMAP_RADIUS_ZOOKEEPER);

	// fill in custom parms
	genericAI = actorInstance->genericAI;
	bmVectorCopy(genericAI->center,actorInstance->actorInstance.position);

	genericAI->deactivationRange = METERTOUNIT(12.0f);
	genericAI->awarenessRange = METERTOUNIT(6.0f);
	genericAI->movementRange = METERTOUNIT(4.0f);
	genericAI->personalSpace = METERTOUNIT(1.0f);

	genericAI->standOffRange = METERTOUNIT(2.2f);
	genericAI->attackHoldDistance = METERTOUNIT(2.0f);
	genericAI->hearRange = METERTOUNIT(2.0f);

	genericAI->attackBoundaryRange = METERTOUNIT(1.7f);

	genericAI->viewAngle = PI/2;
	genericAI->awareAngle = PI/2;

	genericAI->alwaysClipPosition = true;
	genericAI->instantAware = true;
	genericAI->instantAttack = true;

	genericAI->goAttack = KeeperGoAttack;
	genericAI->goAware = KeeperGoAware;

	genericAI->currentRotTime = RAD(180.0f)/60.0f;

	genericAI->currentBlendTime = 0.3f;
	
	genericAI->headNode = "spinetop";
	SETVECTOR(genericAI->starsOffset,0.0f,METERTOUNIT(0.5f),0.0f,1.0f);
	SETVECTOR(genericAI->starVars,METERTOUNIT(0.3f),METERTOUNIT(0.4f),METERTOUNIT(0.0f),METERTOUNIT(0.0f));

	SetAISeqFunctions(actorInstance,ZookeeperIdleSeq,ZookeeperAwareSeq,ZookeeperAttackSeq,
		ZookeeperTooCloseSeq,ZookeeperAttackedSeq,ZookeeperOutOfRangeSeq,NULL,ZookeeperPursuitAttackSeq);

	SetAIMessageHandler(actorInstance, ZooKeeperMessageHandler);

	genericAI->customData = (ZOOKEEPERINFO*)ZALLOC(sizeof(ZOOKEEPERINFO));
	ASSERT(genericAI->customData);
	
	ZOOKEEPERINFO *zooInfo = (ZOOKEEPERINFO*)genericAI->customData;

	genericAI->runCRC = bkCRC32((uchar*)"run",3,0);

	zooInfo->type = type;
	zooInfo->state = KEEPER_STARTATTACK;
	zooInfo->defend = defend;

	return TRUE;
}
