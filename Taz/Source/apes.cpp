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
#include "apes.h"
#include "debris.h"
#include "linkedlist.h"
#include "ice.h"
#include "util.h"


// PP: TEMP?: radius of Ape for shadow map
// PP: TODO? (how?): get bounds of actor instances on the fly so's to optimise shadow map resolution
#define SHADOWMAP_RADIUS_APE		METERTOUNIT(3.0f)

LINKEDLIST<BANANASKIN>		BANANASKIN::list("bananaskin");

static SEQ idle[] = 
{
	{SEQ_SETROTSPEED,NULL,RAD(180.0f)},
	{SEQ_RETURN,"walk2",0},
	
	// TP: Throw banana
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,true},
	{SEQ_ATTACK,"idle6",0},
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},

	{SEQ_SETROTSPEED,NULL,RAD(45.0f)},
	{SEQ_TURNLEFT,NULL,90},
	{SEQ_WALK,"walk2",0.5},
	{SEQ_PLAY,"idle1",0},
	{SEQ_TURNLEFT,NULL,90},
	{SEQ_WALK,"walk2",1},
	{SEQ_PLAY,"idle5",0},
	{SEQ_TURNLEFT,NULL,90},
	{SEQ_WALK,"walk2",1},
	{SEQ_PLAY,"idle2",0},
	{SEQ_TURNLEFT,NULL,90},
	{SEQ_WALK,"walk3",1},
	{SEQ_PLAY,"idle4",0},

	// TP: {SEQ_CHANGESTATEONATTACKEDONLY,NULL,true},
	{SEQ_FINDNEWCENTER,NULL,0},
	{SEQ_RETURN,"walk3",0},
	// TP: {SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},

	// TP: Throw banana
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,true},
	{SEQ_PLAYSAMPLE,"paeatbanana.wav",false},
	{SEQ_ATTACK,"idle6",0},
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},

	{SEQ_END,NULL,0},
};

static SEQ aware[] =
{
	{SEQ_SETROTSPEED,NULL,RAD(360.0f)},
	// TP: {SEQ_CHANGESTATEONATTACKEDONLY,NULL,true},
	{SEQ_FINDNEWCENTER,NULL,0},
	{SEQ_RETURN,"walk3",0},
	// TP: {SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},

	{SEQ_SETROTSPEED,NULL,RAD(180.0f)},

	{SEQ_MOTIONBONE,NULL,0},
	{SEQ_FACETAZ,"walk2",0},
	{SEQ_MOTIONBONE,NULL,1},

	{SEQ_PLAY,"idle10",0},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAY,"faceoff",0},
	{SEQ_PLAY,"faceoff",0},

	{SEQ_MOTIONBONE,NULL,0},
	{SEQ_FACETAZ,"walk2",0},
	{SEQ_MOTIONBONE,NULL,1},
	
	{SEQ_PLAY,"idle4",0},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAY,"faceoff",0},

	{SEQ_MOTIONBONE,NULL,0},
	{SEQ_FACETAZ,"walk2",0},
	{SEQ_MOTIONBONE,NULL,1},

	{SEQ_PLAY,"idle1",0},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAY,"faceoff",0},
	{SEQ_PLAY,"faceoff",0},

	// TP: {SEQ_CHANGESTATEONATTACKEDONLY,NULL,true},
	{SEQ_FINDNEWCENTER,NULL,0},
	{SEQ_RETURN,"walk3",0},
	// TP: {SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},

	{SEQ_MOTIONBONE,NULL,0},
	{SEQ_FACETAZ,"walk2",0},
	{SEQ_MOTIONBONE,NULL,1},

	{SEQ_PLAY,"idle2",0},
	{SEQ_PLAYANDFACE,"faceoff",0},

	{SEQ_MOTIONBONE,NULL,0},
	{SEQ_FACETAZ,"walk2",0},
	{SEQ_MOTIONBONE,NULL,1},

	{SEQ_PLAY,"idle5",0},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAY,"faceoff",0},

	{SEQ_END,NULL,0},
};

static SEQ attack1[] = 
{
//	{SEQ_FACETAZ,NULL,0},
//	{SEQ_CHASE,"walk2",0},
	{SEQ_SETROTSPEED,NULL,RAD(180.0f)},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAYSAMPLE,"papunch1.wav",0},
	{SEQ_ATTACK,"attack2",0},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAYSAMPLE,"papunch2.wav",0},
	{SEQ_ATTACK,"attack1",0},
	{SEQ_END,NULL,0},
};

static SEQ attack2[] = 
{
//	{SEQ_CHASE,"walk2",0},
//	{SEQ_FACETAZ,NULL,-10.0f},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,true},
	{SEQ_PLAY,"react8",0},
	{SEQ_ATTACK,"attack4",0},
	{SEQ_PLAY,"attack5",0},
	{SEQ_STUNNED,"react9",3.0f},
	{SEQ_CHANGESTATEONATTACKEDONLY,NULL,false},
	{SEQ_END,NULL,0},
};

static SEQ attack3[] = 
{
	{SEQ_CHASE,"walk1",0},
	{SEQ_FACETAZ,NULL,-10.0f},
	{SEQ_PLAY,"idle2",0.25f},
	{SEQ_FACETAZ,NULL,-10.0f},
	{SEQ_PLAYSAMPLE,"papunch1.wav",0},
	{SEQ_ATTACK,"attack3",0},
	{SEQ_END,NULL,0},
};

static SEQ defend[] =
{
	{SEQ_LOOP,NULL,0},
};

static SEQ spinFront[] = 
{
	{SEQ_SETROTSPEED,NULL,RAD(360.0f)},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAYSAMPLE,"papunch1.wav",0},
	{SEQ_ATTACK,"attack2",0},
	{SEQ_PLAYSAMPLE,"papunch2.wav",0},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_ATTACK,"attack1",0},
	{SEQ_END,NULL,0},
};

static SEQ stunnedAttack[] =
{
	{SEQ_PLAY,"react5",0},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_PLAY,"react6",5.0f},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_PLAY,"react7",0},
	{SEQ_END,NULL,0},
};

static SEQ stunnedBiteAttack[] =
{
	{SEQ_PLAY,"react3",0},
	{SEQ_PLAYSAMPLE, "dazed.wav", TRUE},
	{SEQ_STARS,NULL,0},
	{SEQ_PLAY,"react6",5.0f},
	{SEQ_STOPSAMPLE, NULL, 0},
	{SEQ_ENDSTARS,NULL,0},
	{SEQ_END,NULL,0},
};

static SEQ spinRear[] =
{
	{SEQ_SETROTSPEED,NULL,RAD(360.0f)},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAYSAMPLE,"papunch1.wav",0},
	{SEQ_ATTACK,"attack2",0},
	{SEQ_FACETAZ,NULL,0},
	{SEQ_PLAYSAMPLE,"papunch2.wav",0},
	{SEQ_ATTACK,"attack1",0},
	{SEQ_END,NULL,0},
};

static SEQ biteFront[] =
{
	{SEQ_FACETAZ,"walk2",0},
	{SEQ_CHASE,"walk1",0},
	{SEQ_PLAYSAMPLE,"papunch2.wav",0},
	{SEQ_ATTACK,"attack1",0},
	{SEQ_END,NULL,0},
};

static SEQ biteRear[] =
{
	{SEQ_FACETAZ,"walk2",0},
	{SEQ_CHASE,"walk1",0},
	{SEQ_PLAYSAMPLE,"papunch2.wav",0},
	{SEQ_ATTACK,"attack1",0},
	{SEQ_END,NULL,0},
};

static SEQ scareFront[] =
{
	{SEQ_PLAY,"react8",0},
	{SEQ_ATTACK,"attack4",0},
	{SEQ_PLAY,"attack5",0},
	{SEQ_STUNNED,"react9",3.0f},
	{SEQ_END,NULL,0},
};

static SEQ scareRear[] = 
{
	{SEQ_PLAY,"react1",0},
	{SEQ_STUNNED,"react2",3.0f},
	{SEQ_END,NULL,0},
};

static SEQ spitFront[] =
{
	{SEQ_PLAY,"idle8",0},
	{SEQ_END,NULL,0},
};

static SEQ outOfRange[] =
{
	{SEQ_RETURN,"walk2",0},
	{SEQ_END,NULL,0},
};

static SEQ costumeAttack[] =
{
	{SEQ_PLAY,"react10",0},
	{SEQ_TURNLEFT,NULL,90.0f},
	{SEQ_SETROTSPEED,NULL,RAD(70.0f)},
	{SEQ_DISABLED,"walk4",4.0f},
	{SEQ_SETROTSPEED,NULL,RAD(90.0f)},
	{SEQ_PLAY,"idle11",0},
	{SEQ_END,NULL,0},
};

static SEQ puzzled[] = 
{
	{SEQ_MOTIONBONE,NULL,0},
	{SEQ_FACETAZ,"walk1",0},
	{SEQ_MOTIONBONE,NULL,1},
	{SEQ_STARTQUESTION,NULL,0},
	{SEQ_PLAY,"disguise",0},
	{SEQ_ENDQUESTION,NULL,0},
	{SEQ_COSTUMEDONE,NULL,0},
	{SEQ_PLAY,"idle10",0},
	{SEQ_PLAY,"idle8",0},
	{SEQ_END,NULL,0},
};

/* --------------------------------------------------------------------------------
   Function : ApeIdleSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ApeIdleSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = idle;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ApeAwareSeq
   Purpose : Respond to requests for aware seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ApeAwareSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = aware;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/*  --------------------------------------------------------------------------------
	Function	: BearPuzzledSeq
	Purpose		: Respond to requests for costume seq
	Parameters	: actorInstance
	Returns		: seq to follow
	Info		: 
*/
int ApePuzzledSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = puzzled;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ApeAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ApeAttackSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = attack3;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	if(bmRand()%2)
	{
		actorInstance->genericAI->currentSeq = attack1;
	}
	else
	{
		actorInstance->genericAI->currentSeq = attack2;
	}

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ApeTooCloseSeq
   Purpose : Respond to requests for defend seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ApeTooCloseSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = attack3;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ApeAttackedSeq
   Purpose : Respond to requests for attacked seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ApeAttackedSeq(struct TAG_ACTORINSTANCE *actorInstance)
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
		if (actorInstance->genericAI->currentSeq[actorInstance->genericAI->seqIdx].type==SEQ_STUNNED)
		{
			actorInstance->genericAI->currentSeq = stunnedBiteAttack;
		}
		else
		{
			actorInstance->genericAI->currentSeq = biteFront;
		}
		break;
	case ATTACKTYPE_BITEREAR:
		actorInstance->genericAI->currentSeq = stunnedBiteAttack;
		break;
	case ATTACKTYPE_SPINFRONT:
		if (actorInstance->genericAI->currentSeq[actorInstance->genericAI->seqIdx].type==SEQ_STUNNED)
		{
			actorInstance->genericAI->currentSeq = stunnedAttack;
		}
		else
		{
			actorInstance->genericAI->currentSeq = spinFront;
		}
		break;
	case ATTACKTYPE_SPINREAR:
		actorInstance->genericAI->currentSeq = stunnedBiteAttack;
		break;
	case ATTACKTYPE_SCAREFRONT:
		actorInstance->genericAI->currentSeq = scareFront;
		break;
	case ATTACKTYPE_SCAREREAR:
		actorInstance->genericAI->currentSeq = scareRear;
		break;
	case ATTACKTYPE_COSTUMEFRONT:
	case ATTACKTYPE_COSTUMEREAR:
		actorInstance->genericAI->currentSeq = costumeAttack;
		break;
	case ATTACKTYPE_SPITFRONT:
		actorInstance->genericAI->currentSeq = spitFront;
		break;
	case ATTACKTYPE_SPITREAR:
		actorInstance->genericAI->currentSeq = stunnedBiteAttack;
		break;
	}
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ApeOutOfRangeSeq
   Purpose : Respond to requests for out of range seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ApeOutOfRangeSeq(struct TAG_ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->currentSeq = outOfRange;
	actorInstance->genericAI->seqIdx = 0;
	actorInstance->genericAI->seqTime = 0.0f;

	return TRUE;
}

/*  --------------------------------------------------------------------------------
	Function	: BananaHitCallback
	Purpose		: Gets called when the flying Banana hits something
	Parameters	: DEBRIS, COLLINFO from collision
	Returns		: FALSE to kill debris, TRUE to allow debris to continue
	Info		: 
*/
static int BananaHitCallback(DEBRIS *debris, COLL_INFO *collInfo)
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

	// TP: Create Banana
	new	BANANASKIN(debris->actorInstance->position,debris->actorInstance->orientation,NULL,15.0f);
	
	return FALSE;
}

/*  --------------------------------------------------------------------------------
	Function	: BananaCollisionCallback
	Purpose		: Called when a character slips on a banana skin
	Parameters	: collider, banana
	Returns		: FALSE to avoid actual collision
	Info		: 
*/
int BananaCollisionCallback(ACTORINSTANCE *collider, ACTORINSTANCE *banana, struct TAG_PRECOLLINFO *preCollInfo)
{
	if (collider->genericAI) return false;

	PlaySample("spin.wav", 255);

	if (collider->actorStatus->currentState == STATE_SPINONICE) return FALSE;
	RequestStateChangePlus(collider,STATE_SPINONICE,IceStateChangeCallback);
	banana->flags |= ACTORFLAG_DELETEAFTERFADEOUT;
	// TP: Return false 'cause we dont want actual collision
	return FALSE;
}

/*  --------------------------------------------------------------------------------
	Function	: BANANASKIN::TAG_BANANASKIN
	Purpose		: Create a banana skin with a limited life
	Parameters	: position, orientation, owner of banana, life of banana skin
	Returns		: 
	Info		: 
*/
BANANASKIN::TAG_BANANASKIN(TBVector pos, TBQuaternion orientation, ACTORINSTANCE *owner, float life, char *actorName)
{
	actorInstance = CreateActorInstance(&map.levelInstances, actorName,"stickydebris");

	if (actorInstance)
	{
		ENABLE_LINE_OR_CARTOON_RENDER_MODE(&actorInstance->actorInstance);

		bmVectorCopy(actorInstance->actorInstance.position,pos);
		bmQuatCopy(actorInstance->actorInstance.orientation,orientation);
		AddActorToCollisionCache(&collisionCache,actorInstance,0);
		SetPreCollisionCallback(actorInstance, BananaCollisionCallback);
	}

	this->life = life;
	this->owner = owner;
	list.append(this);
}

/*  --------------------------------------------------------------------------------
	Function	: BANANASKIN::~TAG_BANANASKIN
	Purpose		: Destructor
	Parameters	: 
	Returns		: 
	Info		: 
*/
BANANASKIN::~TAG_BANANASKIN()
{
	list.remove(this);
}

/*  --------------------------------------------------------------------------------
	Function	: Update	
	Purpose		: Perform update on banana skin
	Parameters	: 
	Returns		: TRUE if life is over, FALSE if life goes on
	Info		: 
*/
bool BANANASKIN::Update(void)
{
	life -= fTime;

	if (actorInstance->flags&ACTORFLAG_DELETEAFTERFADEOUT)
	{
		// TP: If characters has slipped on banana then the coll callback will set this flag
		life = 0.0f;
	}

	if (life <= 0.0f)
	{
		// TP: life over
		actorInstance->flags |= ACTORFLAG_DELETEAFTERFADEOUT;
		actorInstance->fadeSpeed = -64.0f;
		return true;
	}

	return false;
}

/*  --------------------------------------------------------------------------------
	Function	: UpdateAllBananaSkins
	Purpose		: Perform a standard update on all the banana skins
	Parameters	: 
	Returns		: 
	Info		: 
*/
void UpdateAllBananaSkins(void)
{
	BANANASKIN	*bananaSkin;
	int			i;

	for (i=BANANASKIN::list.getSize()-1;i>=0;i--)
	{
		bananaSkin = BANANASKIN::list.get(i);

		if (bananaSkin->Update())
		{
			SAFE_DELETE(bananaSkin);
		}
	}
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
	Function	: PirateApeAttack
	Purpose		: Pirate ape attack
	Parameters	: actorInstance, type
	Returns		: 
	Info		: 
*/
void PirateApeAttack(ACTORINSTANCE *actorInstance, int type)
{
	char	*node;

	switch(type)
	{
	case 1:
		node = "lfoot";
		break;
	case 2:
		node = "rfinger";
		break;
	case 3:
		node = "lfinger";
		break;
	case 4:
		PlaySample("pabangchest1.wav",255,actorInstance->actorInstance.position);
		return;
	case 5:
		PlaySample("pabangchest2.wav",255,actorInstance->actorInstance.position);
		return;
	}

	if (DistFromNodeToTaz(actorInstance,node) < METERTOUNIT(1.25f))
	{
		RequestRumbleEffect(gameStatus.player[0].controller, 220, 0.0f, 0.5f, 0.75f, 0.0f, WAVEFORM_NORMAL);
		switch(type)
		{
		case 1:
			// TP: UseStateChange callback
			PlaySample("pasmackhit.wav",255);
			RequestStateChangePlus(gameStatus.player1, STATE_PROJECTILESLIDE, myStateChangeCallback);
			FacePoint(gameStatus.player1, actorInstance->actorInstance.position, 0.0f, RAD(180.0f));
			StopCollidingWith(gameStatus.player1,actorInstance,0.2f);
			break;
		case 2:
			PlaySample("pasmackhit.wav",255);
			RequestStateChange(gameStatus.player1,STATE_PLAYANIMATION);
			FlushAnimationQueue(gameStatus.player1);
			PlayAnimationByName(gameStatus.player1,"react1",1.0f,0,0,0.1f,NONE);
			break;
		case 3:
			PlaySample("pasmackhit.wav",255);
			RequestStateChange(gameStatus.player1,STATE_PLAYANIMATION);
			FlushAnimationQueue(gameStatus.player1);
			PlayAnimationByName(gameStatus.player1,"react2",1.0f,0,0,0.1f,NONE);
			break;
		}
	}
}

/*  --------------------------------------------------------------------------------
	Function	: PirateApeThrowBanana
	Purpose		: Throw a banana skin
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
void PirateApeThrowBanana(ACTORINSTANCE *actorInstance)
{
	TBVector				startPos,vel;
	TBActorNodeInstance		*node;
	TBMatrix				matrix;
	DEBRIS					*debris;

	// TP: find start position
	if(node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "rfinger"))
	{
		baGetNodesWorldPosition(&actorInstance->actorInstance, node, startPos);
	}
	else
	{
		return;
	}

	startPos[Y] += 250;

	// TP: find correct velocity
	bmVectorSet(vel,0.0f,1.0f,1.0f,1.0f);
	bmQuatToMatrix(matrix,actorInstance->actorInstance.orientation);
	bmMatMultiplyVector(matrix,vel);
	bmVectorScaleToLength(vel,vel,UtilGenerateRandomNumber(METERTOUNIT(7.0f),METERTOUNIT(5.0f)));

	// TP: create banana and throw using debris maybe with sticky property
	debris = TazSpit(actorInstance,bkCRC32((uchar*)"banana.obe",strlen("banana.obe"),0),startPos,vel,0.0f,METERTOUNIT(0.25f));
	if (debris)
	{
		debris->flags |= DEBRISFLAG_STICKY;
		debris->flags &=~DEBRISFLAG_AFFECTOBJECTS;
		SETVECTOR(debris->actorInstance->scale,1.0f,1.0f,1.0f,1.0f);
		EnableActorCartoonRenderMode(debris->actorInstance);
		debris->impactCallback = BananaHitCallback;
	}
}

/* --------------------------------------------------------------------------------
   Function : ApeAttackCallback
   Purpose : Called each frame during an attack 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
int ApeAttackCallback(struct TAG_ACTORINSTANCE *actorInstance)
{
	if (CheckIfPlayingAnimation(actorInstance,"attack4")&&(gameStatus.player1->actorStatus->currentState!=STATE_PROJECTILESLIDE))
	{
		// TP: if contact with Taz then send him flying
		if (actorInstance->genericAI->collideWithTaz)
		{
			// TP: UseStateChange callback
			RequestStateChangePlus(gameStatus.player1, STATE_PROJECTILESLIDE, myStateChangeCallback);
			StopCollidingWith(gameStatus.player1,actorInstance,0.2f);
			FacePoint(gameStatus.player1, actorInstance->actorInstance.position, 0.0f, RAD(180.0f));
		}
	}
	
	return FALSE;	// continue processing pattern
}

/* --------------------------------------------------------------------------------
   Function : ApeStateChangeCallback
   Purpose : Called when state changed 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void ApeStateChangeCallback(ACTORINSTANCE *actorInstance, int state, int lastState)
{
	if(lastState == AISTATE_IDLE)
	{
		DestroyInjuryStars(actorInstance);
	}
}

/* --------------------------------------------------------------------------------
   Function : ApeTazSquashedCallback
   Purpose : Called when state changed 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int ApeTazHitCallback(ACTORINSTANCE *actorInstance)
{
	TAZCATCHERINFO	*apeInfo = (TAZCATCHERINFO*)actorInstance->genericAI->customData;

	if((apeInfo->timeSinceLastAttack += fTime) > 1.0f)
	{
		SetAIAttackCallback(actorInstance,ApeAttackCallback);

		// sort out taz catcher
		FlushAnimationQueue(actorInstance);

		ResetCharacterAfterJump(actorInstance);

		actorInstance->genericAI->flags &= ~GENERICAI_DONTCHANGESTATE;
		return FALSE;
	}
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ApeMessageHandler
   Purpose : Called when a message is sent in the sequence
   Parameters : ACTORINSTANCE *, char *message
   Returns : 
   Info : 
*/
int ApeMessageHandler(ACTORINSTANCE *actorInstance, char *message)
{
	GENERICAI	*genericAI = actorInstance->genericAI;
	char		string[256];

	// TP: Pirate ape
	if (strcmpi(message,"throwbanana")==0)
	{
		PirateApeThrowBanana(actorInstance);
		return true;
	}
	if (strcmpi(message,"papeattack1")==0)
	{
		PirateApeAttack(actorInstance,1);
		return true;
	}
	if (strcmpi(message,"papeattack1")==0)
	{
		PirateApeAttack(actorInstance,1);
		return true;
	}
	if (strcmpi(message,"papeattack2")==0)
	{
		PirateApeAttack(actorInstance,2);
		return true;
	}
	if (strcmpi(message,"papeattack3")==0)
	{
		PirateApeAttack(actorInstance,3);
		return true;
	}
	if (strcmpi(message,"papebeatl")==0)
	{
		PirateApeAttack(actorInstance,4);
		return true;
	}
	if (strcmpi(message,"papebeatr")==0)
	{
		PirateApeAttack(actorInstance,5);
		return true;
	}

	if (strcmpi(message,"throwbanana")==0)
	{
		sprintf(string,"%X %X throwbanana",actorInstance);
		bkGenerateEvent("special",string);
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
   Function : CreateActorInstanceSecurityBot
   Purpose : Setup the basic ai for a zookeeper
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceApe(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList)
{
	GENERICAI	*genericAI;
	TBVector	tempVector;

	if (!FindActorInActorList("banana.obe"))
	{
		LoadActor("banana.obe","pirateape",0);
	}
	// create extra structures
	CreateActorInstanceBehaviour(actorInstance);
	CreateActorInstanceAnimation(actorInstance);
	CreateActorInstanceStatus(actorInstance);
	//CreateActorInstanceShadow(actorInstance,METERTOUNIT(1.5f),METERTOUNIT(2.5f), SHADOWMAP_RADIUS_APE);
	// TP: CreateActorInstanceCharacterInfo(actorInstance);
	SetActorInertia(actorInstance->actorBehaviour,0);
	SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
	SetActorMaxSpeed(actorInstance->actorBehaviour,METERTOUNIT(15)); // set max speed 8mps
	SETVECTOR(tempVector,0.0f,METERTOUNIT(1.0f),0.0f,1.0f);
	SetActorCylinder(0,actorInstance->actorBehaviour,tempVector,METERTOUNIT(1.0f),METERTOUNIT(2.0f));
	AddActorToCollisionCylinderCache(&collisionCache,actorInstance);
	
	// create ai structure
	if (CreateActorInstanceGenericAI(actorInstance,activeList,inactiveList)==FALSE) return FALSE;
	AISetShadow(actorInstance,METERTOUNIT(1.5f),METERTOUNIT(2.5f), SHADOWMAP_RADIUS_APE);

	// fill in custom parms
	genericAI = actorInstance->genericAI;
	bmVectorCopy(genericAI->center,actorInstance->actorInstance.position);
	genericAI->deactivationRange = METERTOUNIT(12.0f);
	genericAI->awarenessRange = METERTOUNIT(5.0f);
	genericAI->movementRange = METERTOUNIT(3.0f);
	genericAI->standOffRange = METERTOUNIT(2.0f);
	genericAI->personalSpace = METERTOUNIT(1.2f);
	genericAI->viewAngle = PI/2;

	genericAI->headNode = "brow";
	SETVECTOR(genericAI->starsOffset,0.0f,METERTOUNIT(0.2f),0.0f,1.0f);
	SETVECTOR(genericAI->starVars,METERTOUNIT(0.3f),METERTOUNIT(0.4f),METERTOUNIT(0.0f),METERTOUNIT(0.0f));

	SetAISeqFunctions(actorInstance,ApeIdleSeq,ApeAwareSeq,ApeAttackSeq,
		ApeTooCloseSeq,ApeAttackedSeq,ApeOutOfRangeSeq,ApePuzzledSeq);

	SetAIAttackCallback(actorInstance,ApeAttackCallback);
	SetAIChangeStateCallback(actorInstance, ApeStateChangeCallback);
	SetAIMessageHandler(actorInstance,ApeMessageHandler);

	genericAI->instantAttack = true;
	genericAI->alwaysClipPosition = true;
	genericAI->costumeInAware = true;
	genericAI->instantAware = true;

	genericAI->bounceCRC = bkCRC32((uchar*)"bounce", 6, 0);

	genericAI->customData = (TAZCATCHERINFO*)ZALLOC(sizeof(TAZCATCHERINFO));
	ASSERT(genericAI->customData);

	return TRUE;
}
