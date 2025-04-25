// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : genericai.cpp
//   Purpose : functions for basic ai
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "actors.h"
#include "maths.h"
#include "collisioncache.h"
#include "animation.h"
#include "mapfile.h"
#include "paths.h"
#include "genericai.h"
#include "characters.h"
#include "main.h"
#include "physics.h"
#include "status.h"
#include "clouds.h"
#include "sfx.h"
#include "zzz.h"
#include "playerstats.h"
#include "textures.h"
#include "trail.h"
#include "util.h"
#include "destructibles.h"
#include "trigger.h"
#include "costumes.h"
#include "cartoonscaling.h"
#include "superburp.h"
#include "invisibletaz.h"
#include "hiccup.h"
#include "chillipepper.h"
#include "bubblegum.h"
#include "thwap.h"
#include "Cheats.h"						// PP: Cheats
#include "prompt.h"

#ifndef INFOGRAMES
	//#define AIDEBUG
	#define AIDEBUGLINES
#endif

#define SPEEDMODIFIER	(1.0f)


char *SAIState[] = 
{
	"AISTATE_INACTIVE",
	"AISTATE_NEW",
	"AISTATE_IDLE",
	"AISTATE_AWARE",
	"AISTATE_ATTACK",
	"AISTATE_DEFEND",
	"AISTATE_ATTACKED",
	"AISTATE_DEATH",
	"AISTATE_OVERRIDE",
	"AISTATE_OUTOFRANGE",
	"AISTATE_TOOCLOSE",
	"AISTATE_COSTUME",
	"AISTATE_ATTACKEDOUTOFRANGE",
	"AISTATE_RESET",
	"AISTATE_POICKOUT",
	"AISTATE_POICKIN",
};

char *SAISeq[] = 
{
	"SEQ_PLAY",
	"SEQ_PLAYANDFACE",
	"SEQ_TURNLEFT",
	"SEQ_TURNRIGHT",
	"SEQ_CHASE",
	"SEQ_ATTACK",
	"SEQ_ATTACKANDFACE",
	"SEQ_RETURN",
	"SEQ_FACETAZ",
	"SEQ_LOOKAWAYFROMTAZ",
	"SEQ_END",
	"SEQ_WALK",
	"SEQ_LOOP",
	"SEQ_SAVEDIRECTION",
	"SEQ_RESTOREDIRECTION",
	"SEQ_SAVEPOSITION",
	"SEQ_WALKTOPOSITION",
	"SEQ_WAIT",
	"SEQ_STUNNED",
	"SEQ_SETBLENDTIME",
	"SEQ_SETROTSPEED",
	"SEQ_SETANIMSPEED",
	"SEQ_MESSAGE",
	"SEQ_FACECENTER",
	"SEQ_LOOKAWAYFROMCENTER",
	"SEQ_STARS",
	"SEQ_ENDSTARS",
	"SEQ_MOTIONBONE",
	"SEQ_LOCKSEQ",
	"SEQ_BOUNDARY",
	"SEQ_PLAYSAMPLE",
	"SEQ_STOPSAMPLE",
	"SEQ_STARTZZZ",
	"SEQ_ENDZZZ",
	"SEQ_STARTQUESTION",
	"SEQ_ENDQUESTION",
	"SEQ_STARTEXCLAMATION",
	"SEQ_ENDEXCLAMATION",
	"SEQ_SETCYLRADIUS",
	"SEQ_SETCYLHEIGHT",
	"SEQ_SETSPHRADIUS",
	"SEQ_SETVIEWANGLE",
	"SEQ_PURSUE",
	"SEQ_RESET",
	"SEQ_DESTROY",
	"SEQ_ATTACKHOLDDISTANDFACE",
	"SEQ_CHANGESTATEONATTACKONLY",
	"SEQ_SKIP",
	"SEQ_ROTATELEFT",
	"SEQ_ROTATERIGHT",
	"SEQ_FINDNEWCENTER",
	"SEQ_RETURNNEWCENTER",
	"SEQ_NODEACTIVATE",
	"SEQ_DEATH",
	"SEQ_CYLCOLLISION",
	"SEQ_SPHCOLLISION",
	"SEQ_SETSTANDOFF",
	"SEQ_DISABLED",
	"SEQ_COSTUMEDONE",
	"SEQ_FACECAMERA",
	"SEQ_DISABLEDANIM",
};


	// TP: character will spot taz if he is within 2 meters height difference
#define GENERICAI_HEIGHTRANGE (METERTOUNIT(2.0f))


/*	--------------------------------------------------------------------------------
	Function 	: InitActorGenericAI
	Purpose 	: Setup the initial state of a characters basic AI
	Parameters 	: ACTORINSTANCE
	Returns 	: 
	Info 		:
*/

void InitActorGenericAI(ACTORINSTANCE *actorInstance,ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList)
{
	GENERICAI	*genericAI = actorInstance->genericAI;

	// start character on correct list
	RemoveInstanceFromInstanceList(actorInstance,actorInstance->instanceList);
	AddInstanceToInstanceList(actorInstance,inactiveList);

	SetCollisionCallback(actorInstance,AICharacterCollision);
	SetPreCollisionCallback(actorInstance,AIPreCharacterCollision);
	
	genericAI->activeList = activeList;
	genericAI->inactiveList = inactiveList;

	genericAI->idle = NULL;
	genericAI->aware = NULL;
	genericAI->attack = NULL;
	genericAI->tooClose = NULL;
	genericAI->attacked = NULL;
	genericAI->outOfRange = NULL;
	genericAI->costume = NULL;
	genericAI->pursuitAttack = NULL;
	genericAI->moveCenter = NULL;

	genericAI->attackCallback = NULL;
	genericAI->messageHandler = NULL;
	genericAI->frameCallback = NULL;
	genericAI->draw2 = NULL;
	genericAI->freeCallback = NULL;
	genericAI->colliderSpecialCallback = NULL;

	genericAI->changeStateCallback = NULL;
	genericAI->flags = 0;

	genericAI->state = genericAI->lastState = AISTATE_INACTIVE;	
	genericAI->currentSeq = NULL;
	genericAI->seqIdx = 0;
	genericAI->currentBlendTime = 0.2f;
	genericAI->seqMode = SEQMODE_NORMAL;

	genericAI->currentMode = AIMODE_NORMAL;

	genericAI->customData = NULL;
	genericAI->currentRotTime = RAD(90.0f)/60.0f;
	genericAI->currentAnimSpeed = 1.0f;
	genericAI->soundHandle = -1;

	genericAI->standOffRange = 0.0f;
	genericAI->personalSpace = 0.0f;
	genericAI->viewAngle = PI/3;
	genericAI->awareAngle = PI;
	genericAI->hearRange = 0.0f;

	genericAI->attackSpeedMod = 1.0f;
	genericAI->attackBoundaryRange = 0.0f;

	genericAI->nodeIndex = -1;
	genericAI->waitingForReset = FALSE;
	genericAI->pursueAnimCRC = 0;
	genericAI->specialColl = -1;
	genericAI->instantAttack = false;
	genericAI->resetAfterAttackOutOfRange = true;

	genericAI->runCRC = 0;
	genericAI->patternState = PATTERNSTATE_FINISHED;
	genericAI->alwaysClipPosition = false;
	genericAI->useMoveBox = false;
	genericAI->instantAware = false;

	genericAI->doCollisionWithTaz = true;
	genericAI->spawnPoint = 0;
	genericAI->costumeInAware = false;
	genericAI->hasSeenCostume = false;
	genericAI->bounceCRC = bkCRC32((uchar*)"bounce",6,0);

	genericAI->characterEnabled = true;
	genericAI->centerValid = true;

	genericAI->goAttack = NULL;
	genericAI->goAware = NULL;
	genericAI->dying = false;
	genericAI->attackWithoutCollision = true;
	genericAI->knockoutOnTopCollision = true;

	genericAI->cartoonScale.stage = CARTOONSCALE_STAGEONE;
	genericAI->cartoonScale.time = 0.0f;
	genericAI->cartoonScale.maxScale = 2.0f;

	AISetShadow(actorInstance, 0.0f, 0.0f, 0.0f);
}

/*  --------------------------------------------------------------------------------
	Function	: CaptureTaz
	Purpose		: Respawns taz at the nearest spawn point or spawn point chosen by the enemy
	Parameters	: actorInstance of enemy
	Returns		: true/false 
	Info		: 
*/
bool CaptureTaz(const ACTORINSTANCE* const enemy)
{
	GENERICAI		*genericAI;
	ACTORINSTANCE	*spawnPoint=NULL,*ptr,*tempInstance,*tempInstance2;
	float			dist=METERTOUNIT(1000.0f),tempFloat;// TP: set out of range

	if (enemy) genericAI = enemy->genericAI;
	else genericAI = NULL;

	if (FindActorInstanceInMap("cratespawn_tp",NULL,&map))
	{
		bkPrintf("*** WARNING *** Attempt to capture taz twice\n");
		return false;
	}
	bkPrintf("Capture Taz\n");

	if ((enemy)&&(!genericAI)) return false;

	if ((genericAI)&&(genericAI->spawnPoint))
	{
		// TP: find the user specified cage
		spawnPoint = FindActorInstanceInMap(NULL, genericAI->spawnPoint, &map);
	}

	if (!spawnPoint)
	{
		// TP: find closest spawn point
		for (ptr=map.invisibleInstances.head.next;ptr!=&map.invisibleInstances.head;ptr=ptr->next)
		{
			if (ptr->special!=SPECIALTYPE_SPAWNCRATE) continue;

			if ((tempFloat = bmVectorDistance(gameStatus.player1->actorInstance.position,ptr->actorInstance.position)) < dist)
			{
				spawnPoint = ptr;
				dist = tempFloat;
			}
		}
	}

	if (!spawnPoint) return false;

	// TP: Create crate
	TBVector	scale;
	bmVectorSet(scale,1.0f,1.0f,1.0f,1.0f);
	tempInstance = CreateActorInstance(&map.animatedInstances,"capturecage_top.obe","cratespawn_tp",spawnPoint->actorInstance.position,scale,spawnPoint->actorInstance.orientation,0);
	tempInstance2 = CreateActorInstance(&map.animatedInstances,"capturecage_base.obe","cratespawn2_tp",spawnPoint->actorInstance.position,scale,spawnPoint->actorInstance.orientation,0);
	
	if(gameStatus.player1->characterInfo->powerUpInfo.invisible)
		FinishInvisibility(gameStatus.player1);

	if(gameStatus.player1->characterInfo->powerUpInfo.bubbleGum)
		FinishBubbleGum(gameStatus.player1);

	if(gameStatus.player1->characterInfo->powerUpInfo.hiccup)
		FinishHiccup(gameStatus.player1);

	if(gameStatus.player1->characterInfo->currentBurp)
		FinishBurp(gameStatus.player1);
	
	if(gameStatus.player1->characterInfo->powerUpInfo.chilliPepper)	
		FinishChilliPepper(gameStatus.player1);

	// CMD: repsawn power ups
	RespawnAllPowerUps();

	if (tempInstance)
	{
		AddActorToCollisionCache(&collisionCache,tempInstance,0);
		AddActorToCollisionCache(&collisionCache,tempInstance2,0);

		bmVectorCopy(gameStatus.player1->actorStatus->startPosition,tempInstance->actorInstance.position);
		gameStatus.player1->actorStatus->startPosition[Y] += METERTOUNIT(0.174f);
		bmVectorCopy(gameStatus.player1->actorStatus->startQuaternion,tempInstance->actorInstance.orientation);

		if (gameStatus.player[0].camera->fade.Lock())
		{
			gameStatus.player[0].camera->fade.FadeAndChangeScene(&gameStatus, FADETYPE_SWIPE, FADESCENE_CAGED, 1.0f, 0, true);
			gameStatus.player[0].camera->fade.SetPlayer(gameStatus.player[0].actorInstance);
			gameStatus.player[0].camera->fade.SetSwipeDirection(RAD(90), RAD(-90));
			gameStatus.player[0].camera->fade.Unlock();
		}
		else
		{
			// TP: destroy cage
			tempInstance = FindActorInstanceInMap("cratespawn_tp",0,&map);
			if (tempInstance)
			{
				FreeActorInstance(tempInstance);
			}
			tempInstance = FindActorInstanceInMap("cratespawn2_tp",0,&map);
			if (tempInstance)
			{
				FreeActorInstance(tempInstance);
			}
		}

		return true;
	}

	return false;
}

/*  --------------------------------------------------------------------------------
	Function	: SetupCameraForCage
	Purpose		: Maintain cage camera
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
void SetupCameraForCage(ACTORINSTANCE *actorInstance)
{
	TBVector		tempVector;
	TBMatrix		tempMatrix;
	CAMERAINFO		*cameraInfo = gameStatus.player[actorInstance->playerNo].camera;
	CHARACTERINFO	*characterInfo = actorInstance->characterInfo;

	KillAllPowerUps(actorInstance);

	if(!CheatActive(CHEAT_COSTUME))// PP: if the costume cheat isn't on...
	{
		// TP: If player dies then remove costume
		RemoveCostume(actorInstance);
	}

	if (characterInfo->inMouth)
	{
		FreeDestructibleInfo(characterInfo->inMouth);
		SAFE_FREE(characterInfo->inMouth);
		characterInfo->inMouth = NULL;
	}

	SETVECTOR(tempVector,0.0f,METERTOUNIT(1.0f),METERTOUNIT(-3.0f),1.0f);

	bmQuatToMatrix(tempMatrix,actorInstance->actorInstance.orientation);
	bmMatMultiplyVector(tempMatrix,tempVector);

	bmVectorAdd(tempVector,actorInstance->actorInstance.position,tempVector);

	SetCameraPosition(&camera[0], tempVector);
}


/*  --------------------------------------------------------------------------------
	Function	: SetupTazInCage
	Purpose		: Puts taz in cage and starts the animations playing
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
void SetupTazInCage(ACTORINSTANCE *actorInstance)
{
	ACTORINSTANCE *tempInstance;

	// TP: play the idle anim on taz
	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance, "cageidle1", 1.0f, 0, 0, 0.0f, MOVE);

	// TP: Find the cage that was created
	tempInstance = FindActorInstanceInMap("cratespawn_tp",0,&map);
	if (tempInstance)
	{
		// TP: Create the debris
		DESTRUCTIBLEDEBRISINFO *debris = (DESTRUCTIBLEDEBRISINFO*)ZALLOC(sizeof(DESTRUCTIBLEDEBRISINFO));
		debris->debrisCRC[0] = bkCRC32((uchar*)"capturecagedebris1.obe", strlen("capturecagedebris1.obe"), 0);
		debris->debrisCRC[1] = bkCRC32((uchar*)"capturecagedebris2.obe", strlen("capturecagedebris2.obe"), 0);
		debris->debrisCRC[2] = bkCRC32((uchar*)"capturecagedebris3.obe", strlen("capturecagedebris3.obe"), 0);
		debris->debrisCRC[3] = bkCRC32((uchar*)"capturecagedebris4.obe", strlen("capturecagedebris4.obe"), 0);
		debris->debrisCRC[4] = bkCRC32((uchar*)"capturecagedebris5.obe", strlen("capturecagedebris5.obe"), 0);
		debris->numDebris = 5;

		// TP: Setup cage as a spin destructible
		CreateDestructibleObjectInstance(tempInstance, DESTRUCTIBLETYPE_SPIN, debris,3, 0, 0, 0, 0, 0, 0.0f, 0.0f, 0.0f, TRUE, 0, 0, 0, METERTOUNIT(3.0f));

		// TP: Create a destruction message to remove the cage base when the top part is destroyed
		tempInstance->destructibleObjectInfo->hitTriggers = (TRIGGERINFO**)ZALLOC(sizeof(TRIGGERINFO*)*3);
		tempInstance->destructibleObjectInfo->hitTriggers[0] = CreateTriggerInfo(1);
		tempInstance->destructibleObjectInfo->hitTriggers[1] = NULL;
		tempInstance->destructibleObjectInfo->hitTriggers[2] = NULL;
		CreateTrigger(tempInstance->destructibleObjectInfo->hitTriggers[0],"trigger","remove=cratespawn2_tp");

		// TP: Start the taz idle animation on the cage also
		FlushAnimationQueue(tempInstance);
		PlayAnimationByName(tempInstance,"tazidle1",1.0f,0,0,0.0f,NONE);
	}

	// TP: choose a capture sound for Sam
	if(characterSounds.sam == -1)
	{
		switch(bmRand()%3)
		{
		case 0:	CallbackWhenSampleFinished((characterSounds.sam=PlaySample("samhappy_1.wav",200)),CharacterSoundsFinishedCallback,&characterSounds.sam);	break;
		case 1:	CallbackWhenSampleFinished((characterSounds.sam=PlaySample("samhappy_2.wav",200)),CharacterSoundsFinishedCallback,&characterSounds.sam);	break;
		case 2: CallbackWhenSampleFinished((characterSounds.sam=PlaySample("samhappy_3.wav",200)),CharacterSoundsFinishedCallback,&characterSounds.sam);	break;
		}
	}

	// TP: find the base of the cage
	tempInstance = FindActorInstanceInMap("cratespawn2_tp",0,&map);
	if (tempInstance)
	{
		// TP: start the idle on this object too
		FlushAnimationQueue(tempInstance);
		PlayAnimationByName(tempInstance,"tazidle1",1.0f,0,0,0.0f,NONE);
	}

}


/*  --------------------------------------------------------------------------------
	Function : IsTazWithinHeightRange
	Purpose : Test if taz is within height range of character
	Parameters : actorInstance of character
	Returns : true/false
	Info : 
*/

bool IsTazWithinHeightRange(ACTORINSTANCE *actorInstance)
{
	float	tazCylBottom = gameStatus.player1->actorInstance.position[Y] + gameStatus.player1->actorBehaviour->collCylinder[0].offset[Y]
		- (gameStatus.player1->actorBehaviour->collCylinder[0].height/2);
	float	tazCylTop = tazCylBottom + gameStatus.player1->actorBehaviour->collCylinder[0].height;
	float	charCylBottom = actorInstance->actorInstance.position[Y] + actorInstance->actorBehaviour->collCylinder[0].offset[Y]
		- (actorInstance->actorBehaviour->collCylinder[0].height/2);
	float	charCylTop = charCylBottom + actorInstance->actorBehaviour->collCylinder[0].height;

	if (tazCylBottom > (charCylTop + GENERICAI_HEIGHTRANGE)) return false;
	if (tazCylTop < (charCylBottom - GENERICAI_HEIGHTRANGE)) return false;

	return true;
}

/*	--------------------------------------------------------------------------------
	Function 	: SetSequenceOverride
	Purpose 	: Set the sequence override, will override until sequence finished.
	Parameters 	: ACTORINSTANCE, SEQ, state
	Returns 	: 
	Info 		: Set the current sequence to play, will override any playing sequence type
*/
void SetSequenceOverride(ACTORINSTANCE *actorInstance,SEQ *sequence,int state)
{
	GENERICAI	*genericAI = actorInstance->genericAI;

	genericAI->flags |= GENERICAI_DONTCHANGESTATE;
	genericAI->currentSeq = sequence;
	genericAI->state = (EAIState)state;
	genericAI->seqIdx = 0;
	genericAI->seqTime = 0.0f;
}

/*  --------------------------------------------------------------------------------
	Function	: SaveStandardSettings
	Purpose		: Save the current settings that should not be carried between sequences
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
static void SaveStandardSettings(ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->save.currentBlendTime = actorInstance->genericAI->currentBlendTime;
	actorInstance->genericAI->save.currentRotTime = actorInstance->genericAI->currentRotTime;
	actorInstance->genericAI->save.currentAnimSpeed = actorInstance->genericAI->currentAnimSpeed;
	actorInstance->genericAI->save.collCylinderHeight = actorInstance->actorBehaviour->collCylinder[0].height;
	actorInstance->genericAI->save.collCylinderRadius = actorInstance->actorBehaviour->collCylinder[0].radius;
	actorInstance->genericAI->save.collSphereRadius = actorInstance->actorBehaviour->collSphere.radius;
	actorInstance->genericAI->save.standOff = actorInstance->genericAI->standOffRange;
}

/*  --------------------------------------------------------------------------------
	Function	: RestoreStandardSettings
	Purpose		: Restore the current settings
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
static void RestoreStandardSettings(ACTORINSTANCE *actorInstance)
{
	actorInstance->genericAI->save.currentBlendTime = actorInstance->genericAI->currentBlendTime;
	actorInstance->genericAI->save.currentRotTime = actorInstance->genericAI->currentRotTime;
	actorInstance->genericAI->save.currentAnimSpeed = actorInstance->genericAI->currentAnimSpeed;
	actorInstance->actorBehaviour->collCylinder[0].height =	actorInstance->genericAI->save.collCylinderHeight;
	actorInstance->actorBehaviour->collCylinder[0].radius =	actorInstance->genericAI->save.collCylinderRadius;
	actorInstance->actorBehaviour->collSphere.radius = actorInstance->genericAI->save.collSphereRadius;
	actorInstance->genericAI->standOffRange = 	actorInstance->genericAI->save.standOff;
}

/*  --------------------------------------------------------------------------------
	Function	: GetNewSeq
	Purpose		: Request a new sequence from the specific ai
	Parameters	: actorInstance, current state
	Returns		: 
	Info		: 
*/
void GetNewSeq(ACTORINSTANCE *actorInstance, EAIState state)
{	
	RestoreStandardSettings(actorInstance);
	SaveStandardSettings(actorInstance);
	SetCollisionCallback(actorInstance, AICharacterCollision);
	RemoveAndFreeCollisionSphereListEntry(&collisionCache, actorInstance);
	actorInstance->genericAI->flags &= ~(GENERICAI_DONTCHANGESTATE|GENERICAI_CHANGEONATTACKEDONLY|GENERICAI_NOBOUNDARY|GENERICAI_NODEACTIVATE);
	actorInstance->actorAnimation->useMotionBone = true;

	switch(state)
	{
	case AISTATE_IDLE:
		actorInstance->genericAI->idle(actorInstance);
		break;
	case AISTATE_AWARE:
		actorInstance->genericAI->aware(actorInstance);
		break;
	case AISTATE_ATTACK:
		actorInstance->genericAI->attack(actorInstance);
		break;
	case AISTATE_COSTUME:
		actorInstance->genericAI->costume(actorInstance);
		break;
	case AISTATE_ATTACKED:
	case AISTATE_ATTACKEDOUTOFRANGE:
		actorInstance->genericAI->attacked(actorInstance);
		break;
	case AISTATE_OUTOFRANGE:
		actorInstance->genericAI->outOfRange(actorInstance);
		break;
	case AISTATE_TOOCLOSE:
		actorInstance->genericAI->tooClose(actorInstance);
		break;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: AICharacterCollisionDisabled
	Purpose 	: Called when a character collides with the AI character that has been disabled
	Parameters 	: npc, attacker
	Returns 	: 
	Info 		:
*/

int AICharacterCollisionDisabled(ACTORINSTANCE *collider, ACTORINSTANCE *npc)
{  
	TBVector		yDirection = { 0.0f, 1.0f, 0.0f, 1.0f };
	TBVector		pos;
	float			speed;
	char			message[256];
	int				val;

	if (!npc->genericAI) return false;

	if (~collider->flags&ACTORFLAG_DEBRISACTOR)
	{
		if (collider->actorStatus->currentState!=STATE_SPRUNG)
		{
			if ((bmRand()%10)>5)
			{
				val = PlayAnimationByName(collider,"bounceup",1.0f,1,0,0.1f,JUMP);	
			}
			else
			{
				val = PlayAnimationByName(collider,"bounceup2",1.0f,0,0,0.1f,JUMP);	
			}

			if (val)
			{	
				// TP: if the character has a bounce anim
				if ((npc->genericAI->bounceCRC)&&(npc->genericAI->collideWithTaz!=2))
				{
					FlushAnimationQueue(npc);
					PlayAnimationByCRCOfName(npc, npc->genericAI->bounceCRC, 1.0f, 0, 0, npc->genericAI->currentBlendTime, NONE);
				}

				if (!(bmRand() % 4))
					PlaySample("spring2.wav",255,0,0,npc->actorInstance.position,0);
				else
					PlaySample("spring.wav",255,0,0,npc->actorInstance.position,0);

				if (EnterState(collider, STATE_SPRUNG))
				{
					// TP: zero the players current y vel so we can be sure of the height of the bounce
					collider->actorBehaviour->physics.velocity[Y] = 0.0f;
					collider->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;
					collider->actorAnimation->useMotionBone = FALSE;

					// TP: give taz a 7 mps bounce
					AddActorSpeed(collider->actorBehaviour, yDirection, METERTOUNIT(7.0f));
				}
			}
		}
	}

	if (collider == gameStatus.player1)
	{
		npc->genericAI->collideWithTaz = TRUE;
	}

	return false;						//requre physics to be processed
}


/*  --------------------------------------------------------------------------------
	Function	: AIPreCharacterCollision
	Purpose		: Called when there is a collision
	Parameters	: collider, character collided with
	Returns		: 
	Info		: 
*/
int AIPreCharacterCollision(ACTORINSTANCE *collider, ACTORINSTANCE *npc, PRECOLLINFO *preCollInfo)
{
	// TP: handle case of falling on top of the character
	if (collider == gameStatus.player1) 
	{
		if ((preCollInfo)&&(npc->genericAI->knockoutOnTopCollision))
		{
			if (preCollInfo->type==PRECOLLTYPE_CYLINDER)
			{
				if (preCollInfo->specific==COLLTYPE_CYLINDER_TOP)
				{
					// TP: don't process collision in this case
					npc->genericAI->collideWithTaz = 2;
					return false;
				}
			}
		}
		npc->genericAI->collideWithTaz = TRUE;
	}
	return npc->genericAI->doCollisionWithTaz;
}


/*  --------------------------------------------------------------------------------
	Function	: AIPreCharacterCollisionDisabled
	Purpose		: Called when there is a collision and the character is disabled
	Parameters	: collider, character collided with
	Returns		: 
	Info		: 
*/

int AIPreCharacterCollisionDisabled(ACTORINSTANCE *collider, ACTORINSTANCE *npc, PRECOLLINFO *preCollInfo)
{
	return false;
}



/*	--------------------------------------------------------------------------------
	Function 	: AICharacterCollision
	Purpose 	: Called when a character collides with the AI character
	Parameters 	: npc, attacker
	Returns 	: 
	Info 		:
*/
int AICharacterCollision(ACTORINSTANCE *collider, ACTORINSTANCE *npc)
{
	if (!npc->genericAI) return false;

	if (npc->genericAI->collideWithTaz==2)
	{
		AICharacterCollisionDisabled(collider, npc);

		AICharacterSpun(npc,collider, ATTACKTYPE_SPINREAR);
		return false;
	}

	if (collider == gameStatus.player1)
	{
		npc->genericAI->collideWithTaz = TRUE;
	}

	if (collider->flags&ACTORFLAG_DEBRISACTOR)
	{
		if (npc->genericAI->currentMode==AIMODE_NORMAL)
		{
			npc->genericAI->currentMode = AIMODE_ATTACKED;
			if (bmFAbs(npc->genericAI->facingRelToTaz) < (PI/2))
			{
				if ((collider->actorStatus)&&(collider->actorStatus->costumeDebris))
				{
					npc->genericAI->typeOfAttack = ATTACKTYPE_COSTUMEFRONT;
				}
				else
				{
					npc->genericAI->typeOfAttack = ATTACKTYPE_SPITFRONT;
				}
			}
			else
			{
				if ((collider->actorStatus)&&(collider->actorStatus->costumeDebris))
				{
					npc->genericAI->typeOfAttack = ATTACKTYPE_COSTUMEREAR;
				}
				else
				{
					npc->genericAI->typeOfAttack = ATTACKTYPE_SPITREAR;
				}
			}
		}
	}
	else
	{
		if (collider->actorStatus)
		{
			// see if actor is taz and spinning
			if (collider->actorStatus->currentState==STATE_SPIN)
			{
				AICharacterSpun(npc,collider);
			}
		}
	}
	return FALSE;
}

/*	--------------------------------------------------------------------------------
	Function 	: AICharacterScared
	Purpose 	: Called when the character has been scared
	Parameters 	: npc, attacker
	Returns 	: 
	Info 		:
*/
void AICharacterScared(ACTORINSTANCE *npc,ACTORINSTANCE *attacker)
{
	if (!npc->genericAI) return;

	if ((npc->genericAI->attackWithoutCollision==false)&&(npc->actorBehaviour->collCylinder[0].radius==0.0f)) return;

	if (npc->genericAI->currentMode==AIMODE_NORMAL)
	{
		npc->genericAI->currentMode = AIMODE_ATTACKED;
		if (bmFAbs(npc->genericAI->facingRelToTaz) < (PI/2))
		{
			npc->genericAI->typeOfAttack = ATTACKTYPE_SCAREFRONT;
		}
		else
		{
			npc->genericAI->typeOfAttack = ATTACKTYPE_SCAREREAR;
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: AICharacterBitten
	Purpose 	: Called when the character has been bitten
	Parameters 	: npc, attacker
	Returns 	: 
	Info 		:
*/
void AICharacterBitten(ACTORINSTANCE *npc,ACTORINSTANCE *attacker)
{
	if (!npc->genericAI) return;

	if ((npc->genericAI->attackWithoutCollision==false)&&(npc->actorBehaviour->collCylinder[0].radius==0.0f)) return;

	// TP: turn attacker to face the npc
	if ((npc->genericAI->currentMode==AIMODE_NORMAL)||
		((npc->genericAI->currentMode==AIMODE_ATTACKED)&&
		((npc->genericAI->currentSeq)&&(npc->genericAI->currentSeq[npc->genericAI->seqIdx].type==SEQ_STUNNED))))
	{
		//reset last state so you can attack with a spin after a bite
		if ((npc->genericAI->currentMode==AIMODE_ATTACKED)&&(npc->genericAI->currentSeq[npc->genericAI->seqIdx].type==SEQ_STUNNED))
		{
			npc->genericAI->lastState = AISTATE_IDLE;
		}

		npc->genericAI->currentMode = AIMODE_ATTACKED;
		if (bmFAbs(npc->genericAI->facingRelToTaz) < (PI/2))
		{
			npc->genericAI->typeOfAttack = ATTACKTYPE_BITEFRONT;
		}
		else
		{
			npc->genericAI->typeOfAttack = ATTACKTYPE_BITEREAR;
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: AICharacterSpun
	Purpose 	: Called when the character has been spun into
	Parameters 	: npc, attacker
	Returns 	: // PP: true if the character was successfully made to spin, else false
	Info 		:
*/
bool AICharacterSpun(ACTORINSTANCE *npc,ACTORINSTANCE *attacker,int forceDir, bool affectStun)
{
	bool	rtn=false;		// PP: return value indicating that the character was successfully made to spin

	if (!npc->genericAI) return rtn;
	if (npc->genericAI->currentSeq)
	{
		if ((npc->genericAI->currentMode==AIMODE_NORMAL)||
			((affectStun)&&(npc->genericAI->currentMode==AIMODE_ATTACKED)&&((npc->genericAI->currentSeq)&&(npc->genericAI->currentSeq[npc->genericAI->seqIdx].type==SEQ_STUNNED))))
		{
			// PP: the character was successfully made to spin
			rtn=true;

			//reset last state so you can attack with a spin after a bite
			if ((npc->genericAI->currentMode==AIMODE_ATTACKED)&&(npc->genericAI->currentSeq[npc->genericAI->seqIdx].type==SEQ_STUNNED))
			{
				npc->genericAI->lastState = AISTATE_IDLE;
			}

			npc->genericAI->currentMode = AIMODE_ATTACKED;

			if (forceDir != -1)
			{
				npc->genericAI->typeOfAttack = forceDir;
				return rtn;
			}
			
			if (bmFAbs(npc->genericAI->facingRelToTaz) < (PI/2))
			{
				npc->genericAI->typeOfAttack = ATTACKTYPE_SPINFRONT;
			}
			else
			{
				npc->genericAI->typeOfAttack = ATTACKTYPE_SPINREAR;
			}
		}
	}

	return rtn;
}

/*  --------------------------------------------------------------------------------
	Function	: DrawDebugInfo
	Purpose		: Draws some usefull debug info on the character
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
void DrawDebugInfo(ACTORINSTANCE *actorInstance)
{
	GENERICAI	*genericai = actorInstance->genericAI;
	TBVector	box[4];

	if (!genericai) return;

	if (genericai->useMoveBox)
	{
		box[0][X] = genericai->moveBox.bbox.xmin;
		box[0][Y] = METERTOUNIT(0.2f);
		box[0][Z] = genericai->moveBox.bbox.zmax;
		box[0][W] = 1.0f;
		box[1][X] = genericai->moveBox.bbox.xmax;
		box[1][Y] = METERTOUNIT(0.2f);
		box[1][Z] = genericai->moveBox.bbox.zmax;
		box[1][W] = 1.0f;
		box[2][X] = genericai->moveBox.bbox.xmax;
		box[2][Y] = METERTOUNIT(0.2f);
		box[2][Z] = genericai->moveBox.bbox.zmin;
		box[2][W] = 1.0f;
		box[3][X] = genericai->moveBox.bbox.xmin;
		box[3][Y] = METERTOUNIT(0.2f);
		box[3][Z] = genericai->moveBox.bbox.zmin;
		box[3][W] = 1.0f;

		bmMatMultiplyVector(genericai->moveBox.matrix,box[0]);
		bmMatMultiplyVector(genericai->moveBox.matrix,box[1]);
		bmMatMultiplyVector(genericai->moveBox.matrix,box[2]);
		bmMatMultiplyVector(genericai->moveBox.matrix,box[3]);

		utilDrawLine(box[0], box[1], RED);
		utilDrawLine(box[1], box[2], RED);
		utilDrawLine(box[2], box[3], RED);
		utilDrawLine(box[3], box[0], RED);

		box[0][X] = genericai->moveBox.attackBBox.xmin;
		box[0][Y] = METERTOUNIT(0.2f);
		box[0][Z] = genericai->moveBox.attackBBox.zmax;
		box[0][W] = 1.0f;
		box[1][X] = genericai->moveBox.attackBBox.xmax;
		box[1][Y] = METERTOUNIT(0.2f);
		box[1][Z] = genericai->moveBox.attackBBox.zmax;
		box[1][W] = 1.0f;
		box[2][X] = genericai->moveBox.attackBBox.xmax;
		box[2][Y] = METERTOUNIT(0.2f);
		box[2][Z] = genericai->moveBox.attackBBox.zmin;
		box[2][W] = 1.0f;
		box[3][X] = genericai->moveBox.attackBBox.xmin;
		box[3][Y] = METERTOUNIT(0.2f);
		box[3][Z] = genericai->moveBox.attackBBox.zmin;
		box[3][W] = 1.0f;

		bmMatMultiplyVector(genericai->moveBox.matrix,box[0]);
		bmMatMultiplyVector(genericai->moveBox.matrix,box[1]);
		bmMatMultiplyVector(genericai->moveBox.matrix,box[2]);
		bmMatMultiplyVector(genericai->moveBox.matrix,box[3]);

		utilDrawLine(box[0], box[1], RED);
		utilDrawLine(box[1], box[2], RED);
		utilDrawLine(box[2], box[3], RED);
		utilDrawLine(box[3], box[0], RED);

	}

	TBVector	tempVector;

	bmVectorCopy(tempVector,genericai->center);
	tempVector[Y] += METERTOUNIT(0.5f);

	utilDrawCircle(tempVector,METERTOUNIT(0.4f),COLOUR(255,0,0));
	utilDrawCircle(tempVector,genericai->movementRange,COLOUR(255,0,0));
	utilDrawCircle(tempVector,genericai->awarenessRange,COLOUR(0,0,255));
	utilDrawCircle(tempVector,genericai->deactivationRange,COLOUR(0,255,0));

	bmVectorAdd(tempVector, actorInstance->actorInstance.position, actorInstance->actorBehaviour->collCylinder[0].offset);
	
	tempVector[Y] -= actorInstance->actorBehaviour->collCylinder[0].height/2;
	utilDrawCircle(tempVector,actorInstance->actorBehaviour->collCylinder[0].radius, COLOUR(255,0,0));
	tempVector[Y] += actorInstance->actorBehaviour->collCylinder[0].height;
	utilDrawCircle(tempVector,actorInstance->actorBehaviour->collCylinder[0].radius, COLOUR(255,0,0));

	bmVectorAdd(tempVector, actorInstance->actorInstance.position, actorInstance->actorBehaviour->collSphere.offset);
	VEC	vec = VEC(tempVector);
	utilDrawSphere(vec, actorInstance->actorBehaviour->collSphere.radius, COLOUR(255,0,0));
}

/*  --------------------------------------------------------------------------------
	Function	: DrawDebugInfo
	Purpose		: Draw debug info for all the ai characters on the given list
	Parameters	: actorInstanceList
	Returns		: 
	Info		: 
*/
void DrawDebugInfo(ACTORINSTANCELIST *actorInstanceList)
{
	if (!showDebugInfo) return;
#ifndef AIDEBUGLINES
	return;
#endif

	ACTORINSTANCE *actorInstance;

	for (actorInstance = actorInstanceList->head.next;actorInstance!=&actorInstanceList->head;actorInstance=actorInstance->next)
	{
		DrawDebugInfo(actorInstance);
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: SetAIPatternFunctions
	Purpose 	: Sets up call backs for requesting seq information
	Parameters 	: ACTORINSTANCE, idle, aware, attack, defend, attacked
	Returns 	: 
	Info 		:
*/
void SetAISeqFunctions(ACTORINSTANCE *actorInstance,RequestPattern idle,RequestPattern aware,RequestPattern attack,
					   RequestPattern tooClose,RequestPattern attacked,RequestPattern outOfRange,RequestPattern costume,
					   RequestPattern pursuitAttack, RequestPattern moveCenter)
{
	actorInstance->genericAI->idle = idle;
	actorInstance->genericAI->aware = aware;
	actorInstance->genericAI->attack = attack;
	actorInstance->genericAI->tooClose = tooClose;
	actorInstance->genericAI->attacked = attacked;
	actorInstance->genericAI->outOfRange = outOfRange;
	actorInstance->genericAI->costume = costume;
	actorInstance->genericAI->pursuitAttack = pursuitAttack;
	actorInstance->genericAI->moveCenter = moveCenter;
}

/*  --------------------------------------------------------------------------------
	Function	: EnlargeBBox
	Purpose		: Enlarge a bounding box by a real world value
	Parameters	: 
	Returns		: 
	Info		: 
*/

void EnlargeBBox(BOUNDINGBOX *box, TBMatrix matrix, TBMatrix invmatrix, float size)
{
	TBVector	tempVector, ref;
	TBMatrix	tempMatrix;

	bmVectorSet(ref,0.0f,0.0f,0.0f,1.0f);
	bmMatMultiplyVector(matrix,ref);

	bmVectorSet(tempVector, box->xmax, 0.0f, 0.0f, 1.0f);
	bmMatMultiplyVector(matrix,tempVector);
	bmVectorSub(tempVector,tempVector,ref);
	bmVectorScaleToLength(tempVector,tempVector,bmVectorLen(tempVector)+size);
	bmVectorAdd(tempVector,tempVector,ref);
	bmMatMultiplyVector(invmatrix,tempVector);
	box->xmax = tempVector[X];

	bmVectorSet(tempVector, box->xmin, 0.0f, 0.0f, 1.0f);
	bmMatMultiplyVector(matrix,tempVector);
	bmVectorSub(tempVector,tempVector,ref);
	bmVectorScaleToLength(tempVector,tempVector,bmVectorLen(tempVector)+size);
	bmVectorAdd(tempVector,tempVector,ref);
	bmMatMultiplyVector(invmatrix,tempVector);
	box->xmin = tempVector[X];
	
	bmVectorSet(tempVector, 0.0f, 0.0f, box->zmax, 1.0f);
	bmMatMultiplyVector(matrix,tempVector);
	bmVectorSub(tempVector,tempVector,ref);
	bmVectorScaleToLength(tempVector,tempVector,bmVectorLen(tempVector)+size);
	bmVectorAdd(tempVector,tempVector,ref);
	bmMatMultiplyVector(invmatrix,tempVector);
	box->zmax = tempVector[Z];

	bmVectorSet(tempVector, 0.0f, 0.0f, box->zmin, 1.0f);
	bmMatMultiplyVector(matrix,tempVector);
	bmVectorSub(tempVector,tempVector,ref);
	bmVectorScaleToLength(tempVector,tempVector,bmVectorLen(tempVector)+size);
	bmVectorAdd(tempVector,tempVector,ref);
	bmMatMultiplyVector(invmatrix,tempVector);
	box->zmin = tempVector[Z];
}

/*  --------------------------------------------------------------------------------
	Function	: ClipPositionToBox
	Purpose		: Clip the position vector to the moveBox
	Parameters	: moveBox, position, optional buffer
	Returns		: true if clipped false if not
	Info		: 
*/
bool ClipPositionToBox(MOVEMENTBOX *moveBox, TBVector	position, float buffer)
{
	bool		clipped = false;
	TBVector	tempVector,ref;
	MOVEMENTBOX	moveBoxBuf;

	if (buffer)
	{
		bmVectorSet(ref,0.0f,0.0f,0.0f,1.0f);
		bmMatMultiplyVector(moveBox->matrix,ref);

		bmVectorSet(tempVector, moveBox->bbox.xmax, 0.0f, 0.0f, 1.0f);
		bmMatMultiplyVector(moveBox->matrix,tempVector);
		bmVectorSub(tempVector,tempVector,ref);
		bmVectorScaleToLength(tempVector,tempVector,bmVectorLen(tempVector)-buffer);
		bmVectorAdd(tempVector,tempVector,ref);
		bmMatMultiplyVector(moveBox->invmatrix,tempVector);
		moveBoxBuf.bbox.xmax = tempVector[X];

		bmVectorSet(tempVector, moveBox->bbox.xmin, 0.0f, 0.0f, 1.0f);
		bmMatMultiplyVector(moveBox->matrix,tempVector);
		bmVectorSub(tempVector,tempVector,ref);
		bmVectorScaleToLength(tempVector,tempVector,bmVectorLen(tempVector)-buffer);
		bmVectorAdd(tempVector,tempVector,ref);
		bmMatMultiplyVector(moveBox->invmatrix,tempVector);
		moveBoxBuf.bbox.xmin = tempVector[X];
		
		bmVectorSet(tempVector, 0.0f, 0.0f, moveBox->bbox.zmax, 1.0f);
		bmMatMultiplyVector(moveBox->matrix,tempVector);
		bmVectorSub(tempVector,tempVector,ref);
		bmVectorScaleToLength(tempVector,tempVector,bmVectorLen(tempVector)-buffer);
		bmVectorAdd(tempVector,tempVector,ref);
		bmMatMultiplyVector(moveBox->invmatrix,tempVector);
		moveBoxBuf.bbox.zmax = tempVector[Z];

		bmVectorSet(tempVector, 0.0f, 0.0f, moveBox->bbox.zmin, 1.0f);
		bmMatMultiplyVector(moveBox->matrix,tempVector);
		bmVectorSub(tempVector,tempVector,ref);
		bmVectorScaleToLength(tempVector,tempVector,bmVectorLen(tempVector)-buffer);
		bmVectorAdd(tempVector,tempVector,ref);
		bmMatMultiplyVector(moveBox->invmatrix,tempVector);
		moveBoxBuf.bbox.zmin = tempVector[Z];

		bmMatCopy(moveBoxBuf.invmatrix, moveBox->invmatrix);
		bmMatCopy(moveBoxBuf.matrix, moveBox->matrix);

		moveBox=&moveBoxBuf;

		if (moveBox->bbox.xmax < moveBox->bbox.xmin) moveBox->bbox.xmax = moveBox->bbox.xmin = (moveBox->bbox.xmax+moveBox->bbox.xmin)/2;
		if (moveBox->bbox.zmax < moveBox->bbox.zmin) moveBox->bbox.zmax = moveBox->bbox.zmin = (moveBox->bbox.zmax+moveBox->bbox.zmin)/2;
	}

	position[W] = 1.0f;
	bmMatMultiplyVector2(tempVector, moveBox->invmatrix, position);

	if (tempVector[X] > moveBox->bbox.xmax)
	{
		tempVector[X] = moveBox->bbox.xmax;
		clipped = true;
	}
	if (tempVector[X] < moveBox->bbox.xmin)
	{
		tempVector[X] = moveBox->bbox.xmin;
		clipped = true;
	}
	if (tempVector[Z] > moveBox->bbox.zmax)
	{
		tempVector[Z] = moveBox->bbox.zmax;
		clipped = true;
	}
	if (tempVector[Z] < moveBox->bbox.zmin)
	{
		tempVector[Z] = moveBox->bbox.zmin;
		clipped = true;
	}

	if (clipped)
	{
		tempVector[W] = 1.0f;
		bmMatMultiplyVector2(position,moveBox->matrix,tempVector);
	}

	return clipped;
}

/*	--------------------------------------------------------------------------------
	Function 	: SetAIAttackCallback
	Purpose 	: Sets up call back for attacking moves
	Parameters 	: ACTORINSTANCE, attackCallback
	Returns 	: 
	Info 		:
*/
void SetAIAttackCallback(ACTORINSTANCE *actorInstance, AttackCallback attackCallback)
{
	actorInstance->genericAI->attackCallback = attackCallback;
}

/*	--------------------------------------------------------------------------------
	Function 	: SetAIChangeStateCallback
	Purpose 	: Sets up call back for when generic AI changes state.
	Parameters 	: ACTORINSTANCE, changeStateCallback
	Returns 	: 
	Info 		:
*/
void SetAIChangeStateCallback(ACTORINSTANCE *actorInstance, AIChangeState changeStateCallback)
{
	actorInstance->genericAI->changeStateCallback = changeStateCallback;
}
 
/*	--------------------------------------------------------------------------------
	Function 	: FreeActorGenericAI
	Purpose 	: Free generic AI
	Parameters 	: ACTORINSTANCE
	Returns 	: 
	Info 		:
*/
void FreeActorGenericAI(ACTORINSTANCE *actorInstance)
{
	GENERICAI	*genericAI = actorInstance->genericAI;

	StopChannel(&genericAI->soundHandle);

	if (genericAI->freeCallback) genericAI->freeCallback(actorInstance);

	SAFE_FREE(genericAI->customData);
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateCharacterAIStats
	Purpose 	: update the AI stats such as distance to taz etc.
	Parameters 	: ACTORINSTANCE
	Returns 	: 
	Info 		:
*/
void UpdateCharacterAIStats(ACTORINSTANCE *actorInstance)
{
	GENERICAI	*genericAI = actorInstance->genericAI;
	float		tempFloat,ang;
	TBVector	tempVector;

	bmVectorSub(genericAI->toTaz,gameStatus.player1->actorInstance.position,actorInstance->actorInstance.position);
	bmVectorSub(genericAI->toCenter,actorInstance->genericAI->center,actorInstance->actorInstance.position);
	bmVectorSub(genericAI->tazToCenter,actorInstance->genericAI->center,gameStatus.player1->actorInstance.position);

	genericAI->toTaz[Y] = 0.0f;
	genericAI->distToTaz = bmVectorLen(genericAI->toTaz);
	genericAI->toCenter[Y] = 0.0f;
	genericAI->distToCenter = bmVectorLen(genericAI->toCenter);
	genericAI->distTazToCenter = bmVectorLen(genericAI->tazToCenter);

	SETVECTOR(tempVector,0.0f,0.0f,1.0f,1.0f);
	bmVectorScaleToLength(tempVector,tempVector,649.51f);
	
	genericAI->facing = GetAngleAroundYFromQuat(actorInstance->actorInstance.orientation);

	SETVECTOR(genericAI->faceVector,0.0f,0.0f,1.0f,1.0f);
	bmVectorRotateY(genericAI->faceVector,genericAI->faceVector,genericAI->facing);

	tempFloat = GetAngleAroundYFromQuat(actorInstance->actorInstance.orientation);
	ang = Aabs(GetAngleAroundYFromVector(genericAI->toTaz)+PI);

	genericAI->facingRelToTaz = FindShortestAngleSigned(ang,tempFloat);
}

static void AIStateChange(ACTORINSTANCE *actorInstance, EAIState newState, EAIState lastState)
{
	GENERICAI	*genericAI = actorInstance->genericAI;

#ifdef AIDEBUG
	bkPrintf("Actor %s, %s -> %s\n",actorInstance->instanceName,SAIState[lastState],SAIState[newState]);
#endif

	// TP: Best kill any outstanding sound effects
	StopChannel(&genericAI->soundHandle);

	genericAI->state = newState;

	// TP: Make sure there are no stars active when shutting down
	DestroyInjuryStars(actorInstance);
	DestroyQuestionMarkAboveActor(actorInstance);
	TurnOffSleepyZedsForCharacter(actorInstance);

	// TP: If there is a callback the call it
	if (genericAI->changeStateCallback)
	{
		genericAI->changeStateCallback(actorInstance,(int)newState,(int)lastState);
	}
}

/*  --------------------------------------------------------------------------------
	Function	: UpdatePursue
	Purpose		: Follow path towards taz and attack unless hampered
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
static int UpdatePursue(ACTORINSTANCE *actorInstance)
{
	GENERICAI	*genericAI = actorInstance->genericAI;

	genericAI->pursuitTrailTimer += fTime;
	// TP: Find nearest node
	if ((genericAI->nodeIndex==-1)||(genericAI->pursuitTrailTimer>1.0f))
	{
		genericAI->pursuitTrailTimer = 0.0f;
		genericAI->nodeIndex = trailFindNearestPoint(gameStatus.player1, actorInstance->actorInstance.position, genericAI->nodePos);
	}

	if (genericAI->nodeIndex==-1)
	{
		// TP: if no node found then bail out of the chase and reset as soon as pos
		genericAI->waitingForReset = TRUE;
		return TRUE;
	}

	// TP: If not current node or current node is null then switch
	if (ReachedPoint(actorInstance->actorInstance.position, genericAI->nodePos, METERTOUNIT(1.0f)))
	{
		trailAdvanceToNext(gameStatus.player1, &genericAI->nodeIndex);

		if (genericAI->nodeIndex==-1)
		{
			// TP: no more points
			genericAI->waitingForReset = TRUE;
			return TRUE;
		}
		else
		{
			trailGetIndexedPoint(gameStatus.player1, genericAI->nodeIndex, genericAI->nodePos);
		}
	}

	// TP: check for getting stuck on something
	if (bmVectorLen(actorInstance->actorBehaviour->physics.velImpactFromLastColl) > (bmVectorLen(actorInstance->actorBehaviour->physics.velocity)*0.75f))
	{
		genericAI->timeStuck += fTime;
		if (genericAI->timeStuck > 1.0f)
		{
			genericAI->waitingForReset = TRUE;
			genericAI->timeStuck = 0.0f;
			return TRUE;
		}
	}

	// TP: check if there is a jump in the path that will cause a problem
	// TP: if move too far(50 meters) or jump problem then reset when appropriate

	// TP: if close enough to attack taz then attack
	if (ReachedPoint(actorInstance->actorInstance.position,gameStatus.player1->actorInstance.position,genericAI->standOffRange))
	{
		// TP: should attack
		return TRUE;
	}

	// TP: attempt to run towards point
	if (!CheckIfPlayingAnimation(actorInstance,NULL,genericAI->pursueAnimCRC))
	{
		PlayAnimationByCRCOfName(actorInstance,genericAI->pursueAnimCRC,genericAI->currentAnimSpeed*genericAI->attackSpeedMod,1,0,genericAI->currentBlendTime,MOVE);
	}

	FacePoint(actorInstance, genericAI->nodePos, genericAI->currentRotTime, 0.0f);

	return FALSE;
}

//#define GOACTIVE(_actorInstance)	((_actorInstance->genericAI->distTazToCenter < _actorInstance->genericAI->deactivationRange))

bool GoActive(ACTORINSTANCE *actorInstance)
{
	GENERICAI	*genericAI = actorInstance->genericAI;

	if (bmFAbs(gameStatus.player1->actorInstance.position[Y] - actorInstance->actorInstance.position[Y]) > actorInstance->genericAI->deactivationRange) return false;

	if (actorInstance->genericAI->useMoveBox)
	{
		if (IsPointWithinBox(&genericAI->moveBox.idleBox, genericAI->moveBox.invmatrix, gameStatus.player1->actorInstance.position, BOXFLAG_XTEST|BOXFLAG_ZTEST) || (genericAI->flags&GENERICAI_NOBOUNDARY))
		{
			return true;
		}
	}
	else
	{
		if (actorInstance->genericAI->distTazToCenter < actorInstance->genericAI->deactivationRange) return true;
	}

	return false;
}


/*  --------------------------------------------------------------------------------
	Function	: SafeToAttackTaz
	Purpose		: Test if it is ok to attack taz
	Parameters	: actorInstance of taz
	Returns		: true/false
	Info		: 
*/

bool SafeToAttackTaz(ACTORINSTANCE *actorInstance)
{
	switch(actorInstance->actorStatus->currentState)
	{
	case STATE_MOVE:
	case STATE_SKIDSTOP:
	case STATE_TIPTOE:
	case STATE_SLIDE:
	case STATE_GETUPFROMSLIDE:
	case STATE_GETUPFROMWATER:
	case STATE_JUMP:
	case STATE_FALL:
	case STATE_IDLE:
	case STATE_BITE:
	case STATE_SPINUP:
	case STATE_SPIN:
	case STATE_SPINDOWN:
	case STATE_SPINDOWNONWATER:
	case STATE_ONICE:
	case STATE_SPINONICE:
	case STATE_PLAYANIMATION:
	case STATE_SCARE:
	case STATE_GOTOSLEEP:			
	case STATE_SLEEP:
	case STATE_DANCE:
	case STATE_LOOKAROUND:
	case STATE_ENTERLOOKAROUND:
	case STATE_CHILLIPEPPER:
	case STATE_NINJAKICK:
	case STATE_BURNT:
	case STATE_SKATECHARGE:
	case STATE_SNOWBOARDATTACK:
	case STATE_SURFBOARDATTACK:
	case STATE_RAPPERATTACK:
	case STATE_WEREWOLFATTACK:
	case STATE_COWBOYSHOOT:
	case STATE_TAZANYELL:
	case STATE_INDYROLL:
	case STATE_CHEESYATTACK:
	case STATE_BADFOOD:
		return true;
	}
	return false;
}

/*  --------------------------------------------------------------------------------
	Function	: IsPlayerQuiet
	Purpose		: decide if the player has made a noise or not
	Parameters	: actorInstance
	Returns		: true if player is quite else false
	Info		: 
*/

bool IsPlayerQuiet(ACTORINSTANCE *actorInstance)
{
	if (actorInstance->actorStatus->currentState==STATE_TIPTOE) return true;
	if (actorInstance->actorStatus->currentState==STATE_MOVE)
	{
		if (CheckIfPlayingAnimation(actorInstance, "run")||CheckIfPlayingAnimation(actorInstance, "walk1")) return false;
		return true;
	}

	return false;
}


/*  --------------------------------------------------------------------------------
	Function	: GoAware
	Purpose		: Test if the character should become aware
	Parameters	: actorInstance of character
	Returns		: true/false
	Info		: 
*/

bool GoAware(ACTORINSTANCE *actorInstance)
{
	if (!actorInstance->genericAI->aware) return false;
	if (gameStatus.player[0].actorInstance->characterInfo->powerUpInfo.invisible) return false;
	if (gameStatus.player[0].actorInstance->characterInfo->powerUpInfo.bubbleGum) return false;
	if (gameStatus.player[0].actorInstance->characterInfo->powerUpInfo.hiccup) return false;
	if (gameStatus.controlSuspend==TRUE) return false;
	if (!SafeToAttackTaz(gameStatus.player1)) return false;

	if (actorInstance->genericAI->goAware) return actorInstance->genericAI->goAware(actorInstance);

	if (actorInstance->genericAI->distTazToCenter >= actorInstance->genericAI->awarenessRange) return false;

	if (!IsTazWithinHeightRange(actorInstance)) return false;
	
	if ((bmFAbs(actorInstance->genericAI->facingRelToTaz) < actorInstance->genericAI->awareAngle) ||
		((actorInstance->genericAI->distToTaz < actorInstance->genericAI->hearRange)&&(!IsPlayerQuiet(gameStatus.player[0].actorInstance))))
	{
		return true;
	}
	return false;
}


/*  --------------------------------------------------------------------------------
	Function	: GoAttack
	Purpose		: Test if it is ok to enter the attack mode
	Parameters	: actorInstance
	Returns		: true/false
	Info		: 
*/
bool GoAttack(ACTORINSTANCE *actorInstance)
{
	GENERICAI	*genericAI = actorInstance->genericAI;

	if (!SafeToAttackTaz(gameStatus.player1)) return false;

	if ((gameStatus.player1->actorStatus->currentState==STATE_DEAD)||(gameStatus.controlSuspend==TRUE)) return false;
	if ((gameStatus.player1->characterInfo->powerUpInfo.invisible==TRUE)||(gameStatus.player1->characterInfo->powerUpInfo.bubbleGum==TRUE)||(gameStatus.player[0].actorInstance->characterInfo->powerUpInfo.hiccup==TRUE)) return false;
	if (!genericAI->attack) return false;

	if (actorInstance->genericAI->goAttack) return actorInstance->genericAI->goAttack(actorInstance);

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
		if ((genericAI->distTazToCenter < (genericAI->movementRange+genericAI->attackBoundaryRange))||(genericAI->flags&GENERICAI_NOBOUNDARY)) return true;
	}

	return false;
}

#define GOATTACKED(_actorInstance)	((_actorInstance->genericAI->currentMode==AIMODE_ATTACKED))

/*  --------------------------------------------------------------------------------
	Function	: OUTOFRANGE
	Purpose		: Test if the character has move out of range
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
bool	OUTOFRANGE(ACTORINSTANCE *actorInstance)
{
	if (actorInstance->genericAI->flags&GENERICAI_NOBOUNDARY) return false;

	if (actorInstance->genericAI->useMoveBox)
	{
		// TP: Can't be out of range if always clip is on
		if (actorInstance->genericAI->alwaysClipPosition) return false;
		if (!IsPointWithinBox(&actorInstance->genericAI->moveBox.bbox,actorInstance->genericAI->moveBox.invmatrix,actorInstance->actorInstance.position)) return true;
	}
	else
	{
		if (actorInstance->genericAI->distToCenter > actorInstance->genericAI->movementRange) return true;
	}
	return false;
}

#define TOOCLOSE(_actorInstance) ((_actorInstance->genericAI->distToTaz < _actorInstance->genericAI->personalSpace)&&actorInstance->genericAI->tooClose)

#define WAITFORCHANGE(_actorInstance) ((_actorInstance->genericAI->patternState==PATTERNSTATE_FINISHED)||(_actorInstance->genericAI->patternState==PATTERNSTATE_COMMANDCOMPLETED)||(_actorInstance->genericAI->currentSeq[_actorInstance->genericAI->seqIdx].type==SEQ_CHASE))

#define CANICHANGESTATE(_actorInstance) ((!(_actorInstance->genericAI->flags&GENERICAI_DONTCHANGESTATE)) && (!(_actorInstance->genericAI->flags&GENERICAI_CHANGEONATTACKEDONLY)))


/*	--------------------------------------------------------------------------------
	Function 	: UpdateCharacterAI
	Purpose 	: Update the current AI for a character
	Parameters 	: ACTORINSTANCE
	Returns 	: true/false	if character is destroyed
	Info 		: Updates path and states
*/

bool UpdateCharacterAI(ACTORINSTANCE *actorInstance)
{
	GENERICAI	*genericAI = actorInstance->genericAI;
	TBQuaternion	tempQuat;

	if (genericAI->frameCallback) genericAI->frameCallback(actorInstance);

	// TP: Check for first time for character
	if (genericAI->currentSeq==NULL)
	{
		SaveStandardSettings(actorInstance);
	}

	// TP: update the standard stats for the character
	UpdateCharacterAIStats(actorInstance);

	// TP: check for falling out of landscape
	if (actorInstance->actorInstance.position[Y] < (worldBoundingBox.ymin-METERTOUNIT(2.0f)))
	{
		genericAI->waitingForReset = true;
	}

	// TP: Handle any reset requests
	if (genericAI->waitingForReset)
	{
		AIStateChange(actorInstance,AISTATE_RESET,genericAI->state);
		genericAI->currentMode = AIMODE_NORMAL;
	}

	if ((genericAI->alwaysClipPosition)&&(~genericAI->flags&GENERICAI_NOBOUNDARY))
	{
		if (genericAI->useMoveBox)
		{
			ClipPositionToBox(&genericAI->moveBox, actorInstance->actorInstance.position);
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
	}

	switch(genericAI->state)
	{
	
	case AISTATE_INACTIVE:
		if (GoActive(actorInstance)&&(genericAI->characterEnabled))
		{
			// activate character
			genericAI->hasSeenCostume = false;
			RemoveInstanceFromInstanceList(actorInstance,genericAI->inactiveList);
			AddInstanceToInstanceList(actorInstance,genericAI->activeList);
			bmVectorCopy(actorInstance->actorInstance.position,genericAI->center);

			genericAI->cartoonScale.stage = CARTOONSCALE_STAGEONE;
			genericAI->cartoonScale.time = 0.0f;
			genericAI->cartoonScale.type = SCALETYPE_UP;
			genericAI->cartoonScale.maxScale = 1.2f;
			AIStateChange(actorInstance,AISTATE_POICKIN,AISTATE_INACTIVE);

			// TP: get new sequence if first time
			if (!genericAI->currentSeq)	
			{
				GetNewSeq(actorInstance,AISTATE_IDLE);
				PlayAnimationByName(actorInstance, "idle1", 1.0f, true, false, 0.0f, NONE);
			}

			// TP: init shadow, if I can
			if (genericAI->shadowRadius!=0.0f)
			{
				CreateActorInstanceShadow(actorInstance,genericAI->shadowRadius, genericAI->shadowHeight, genericAI->shadowMapRadius);
			}
		}
		break;

	case AISTATE_IDLE:
		// TP: Check for going out of range
		if (OUTOFRANGE(actorInstance)&&WAITFORCHANGE(actorInstance))
		{
			AIStateChange(actorInstance,AISTATE_OUTOFRANGE,AISTATE_IDLE);
			break;
		}

		// TP: Check for having been attacked
		if (GOATTACKED(actorInstance))
		{
			if  (!(genericAI->flags&GENERICAI_DONTCHANGESTATE))
			{
				AIStateChange(actorInstance,AISTATE_ATTACKED,AISTATE_IDLE);
				break;
			}
			else
			{
				// TP: clear the attack mode
				genericAI->currentMode = AIMODE_NORMAL;
			}
		}

		// TP: Check for going aware
		if (GoAware(actorInstance) && (genericAI->instantAware||(WAITFORCHANGE(actorInstance))) && CANICHANGESTATE(actorInstance))
		{
			// TP: if player is in costume and in view // PP: or if player is wearing deceptive ACME goth mask...
/*			if (((genericAI->costume)&&(!actorInstance->genericAI->hasSeenCostume)&&(gameStatus.player[0].actorInstance->characterInfo->costume != -1)
				&&(bmFAbs(genericAI->facingRelToTaz) < genericAI->viewAngle))
				||(gameStatus.player[0].actorInstance->actorStatus->currentState==STATE_VEHICLE)||CheatActive(CHEAT_QWILSON))*/

			if ((genericAI->costume)&&(!actorInstance->genericAI->hasSeenCostume)&& ((gameStatus.player[0].actorInstance->characterInfo->costume != -1)||CheatActive(CHEAT_QWILSON))
				&&(bmFAbs(genericAI->facingRelToTaz) < genericAI->viewAngle))
			{
				AIStateChange(actorInstance,AISTATE_COSTUME,AISTATE_IDLE);
			}
			else
			{		
				AIStateChange(actorInstance,AISTATE_AWARE,AISTATE_IDLE);
			}
			break;
		}

		// TP: Check for going inactive
		if ((!GoActive(actorInstance))&&(~genericAI->flags&GENERICAI_NODEACTIVATE))
		{
			// TP: Cartoon poick out of existance
			genericAI->cartoonScale.stage = CARTOONSCALEDOWN_STAGEONE;
			genericAI->cartoonScale.time = 1.5f;
			genericAI->cartoonScale.type = SCALETYPE_DOWN;
			genericAI->cartoonScale.maxScale = 1.2f;
			AIStateChange(actorInstance,AISTATE_POICKOUT,AISTATE_IDLE);
			
			// TP: remove shadow
			SAFE_FREE(actorInstance->actorShadow);
			break;
		}

		// TP: Choose and play seq if not in process
		if ((genericAI->lastState!=AISTATE_IDLE)||(genericAI->patternState==PATTERNSTATE_FINISHED)||(!genericAI->currentSeq))
		{
			genericAI->lastState = genericAI->state;
			GetNewSeq(actorInstance,genericAI->state);
		}
		break;
		
	case AISTATE_AWARE:
		// TP: Check for going out of range
		if (OUTOFRANGE(actorInstance)&&WAITFORCHANGE(actorInstance))
		{
			AIStateChange(actorInstance,AISTATE_OUTOFRANGE,AISTATE_AWARE);
			break;
		}

		// TP: Check for costume
		if ((genericAI->costumeInAware)&&(!actorInstance->genericAI->hasSeenCostume)&&(genericAI->costume)&&
			(gameStatus.player[0].actorInstance->characterInfo->costume != -1)&&WAITFORCHANGE(actorInstance) && CANICHANGESTATE(actorInstance))
		{
			AIStateChange(actorInstance,AISTATE_COSTUME,AISTATE_AWARE);
			break;
		}

		// TP: Check for having been attacked
		if (GOATTACKED(actorInstance))
		{
			if (!(genericAI->flags&GENERICAI_DONTCHANGESTATE))
			{
				AIStateChange(actorInstance,AISTATE_ATTACKED,AISTATE_AWARE);
				break;
			}
			else
			{
				// TP: clear the attack mode
				genericAI->currentMode = AIMODE_NORMAL;
			}
		}

		// TP: Check for going idle
		if ((!GoAware(actorInstance))&&WAITFORCHANGE(actorInstance) && CANICHANGESTATE(actorInstance))
		{
			AIStateChange(actorInstance,AISTATE_IDLE,AISTATE_AWARE);
			break;
		}

		// TP: Check for going to attack
		if (GoAttack(actorInstance)&&(genericAI->instantAttack||(WAITFORCHANGE(actorInstance))) && CANICHANGESTATE(actorInstance))
		{
			// check if character can see taz
			if ((bmFAbs(genericAI->facingRelToTaz) < genericAI->viewAngle)||(genericAI->distToTaz < genericAI->hearRange))
			{
				// TP: if player is in costume // PP: or if player is wearing deceptive ACME goth mask...
				if ((genericAI->costume)&&(!actorInstance->genericAI->hasSeenCostume)&&
					((gameStatus.player[0].actorInstance->characterInfo->costume != -1)||CheatActive(CHEAT_QWILSON)))
				{
					AIStateChange(actorInstance,AISTATE_COSTUME,AISTATE_AWARE);
				}
				else
				{
					AIStateChange(actorInstance,AISTATE_ATTACK,AISTATE_AWARE);
				}
				break;
			}
		}

		// TP: Choose and play seq if not in process
		if ((genericAI->lastState!=AISTATE_AWARE)||(genericAI->patternState==PATTERNSTATE_FINISHED)||(!genericAI->currentSeq))
		{
			genericAI->lastState = genericAI->state;
			GetNewSeq(actorInstance,genericAI->state);
		}
		break;

	case AISTATE_COSTUME:
		// TP: Check for going out of range
		if (OUTOFRANGE(actorInstance)&&WAITFORCHANGE(actorInstance))
		{
			AIStateChange(actorInstance,AISTATE_OUTOFRANGE,AISTATE_COSTUME);
			break;
		}

		// TP: Check for having been attacked
		if (GOATTACKED(actorInstance))
		{
			if (!(genericAI->flags&GENERICAI_DONTCHANGESTATE))
			{
				AIStateChange(actorInstance,AISTATE_ATTACKED,AISTATE_COSTUME);
				break;
			}
			else
			{
				// TP: clear the attack mode
				genericAI->currentMode = AIMODE_NORMAL;
			}
		}

		if ((genericAI->hasSeenCostume==true)&&WAITFORCHANGE(actorInstance) && CANICHANGESTATE(actorInstance))
		{
			AIStateChange(actorInstance,AISTATE_AWARE,AISTATE_COSTUME);
			break;
		}
		
		if ((genericAI->costumeInAware==false)&&((gameStatus.player[0].actorInstance->actorStatus->currentState==STATE_VEHICLE)||(!GoAttack(actorInstance))) &&WAITFORCHANGE(actorInstance) && CANICHANGESTATE(actorInstance))
		{
			AIStateChange(actorInstance,AISTATE_AWARE,AISTATE_COSTUME);
			break;
		}

		// TP: Check for going idle
		if ((genericAI->costumeInAware)&&(!GoAware(actorInstance))&&WAITFORCHANGE(actorInstance) && CANICHANGESTATE(actorInstance))
		{
			AIStateChange(actorInstance,AISTATE_IDLE,AISTATE_COSTUME);
			break;
		}

		// TP: Choose and play seq if not in process
		if ((genericAI->lastState!=AISTATE_COSTUME)||(genericAI->patternState==PATTERNSTATE_FINISHED)||(!genericAI->currentSeq))
		{
			genericAI->lastState = genericAI->state;
			GetNewSeq(actorInstance,genericAI->state);
		}
		break;

	case AISTATE_ATTACK:

		// TP: Check for going out of range
		if (OUTOFRANGE(actorInstance)&&WAITFORCHANGE(actorInstance))
		{
			AIStateChange(actorInstance,AISTATE_OUTOFRANGE,AISTATE_ATTACK);
			break;
		}

		// TP: Check if the player has got too close to the enemy
		if (TOOCLOSE(actorInstance)&&CANICHANGESTATE(actorInstance))
		{
			AIStateChange(actorInstance,AISTATE_TOOCLOSE,AISTATE_ATTACK);
			break;
		}

		// TP: Check for having been attacked
		if (GOATTACKED(actorInstance))
		{
			if (!(genericAI->flags&GENERICAI_DONTCHANGESTATE))
			{
				if (genericAI->flags&GENERICAI_NOBOUNDARY)
				{
					AIStateChange(actorInstance,AISTATE_ATTACKEDOUTOFRANGE,AISTATE_ATTACK);
				}
				else
				{
					AIStateChange(actorInstance,AISTATE_ATTACKED,AISTATE_ATTACK);
				}
				break;
			}
			else
			{
				// TP: clear the attack mode
				genericAI->currentMode = AIMODE_NORMAL;
			}
		}

		// TP: Check for going aware
		if ( ((gameStatus.player[0].actorInstance->actorStatus->currentState==STATE_VEHICLE)||(!GoAttack(actorInstance))) &&WAITFORCHANGE(actorInstance) && CANICHANGESTATE(actorInstance))
		{
			AIStateChange(actorInstance,AISTATE_AWARE,AISTATE_ATTACK);
			break;
		}

		// TP: Check for costume
		if ((genericAI->costume)&&(!actorInstance->genericAI->hasSeenCostume)&&
			(gameStatus.player[0].actorInstance->characterInfo->costume != -1)&&WAITFORCHANGE(actorInstance) && CANICHANGESTATE(actorInstance))
		{
			AIStateChange(actorInstance,AISTATE_COSTUME,AISTATE_ATTACK);
			break;
		}

		// TP: Choose and play seq if not in process
		if ((genericAI->lastState!=AISTATE_ATTACK)||(genericAI->patternState==PATTERNSTATE_FINISHED)||(!genericAI->currentSeq))
		{
			genericAI->lastState = genericAI->state;
			GetNewSeq(actorInstance,genericAI->state);
		}
		break;

	case AISTATE_TOOCLOSE:
		// TP: Check if the player has got too close to the enemy
		if (!TOOCLOSE(actorInstance))
		{
			AIStateChange(actorInstance,AISTATE_ATTACK,AISTATE_TOOCLOSE);
			break;
		}

		// TP: Check for having been attacked
		if (GOATTACKED(actorInstance))
		{
			if (!(genericAI->flags&GENERICAI_DONTCHANGESTATE))
			{
				AIStateChange(actorInstance,AISTATE_ATTACKED,AISTATE_IDLE);
				break;
			}
			else
			{
				// TP: clear the attack mode
				genericAI->currentMode = AIMODE_NORMAL;
			}
		}

		// TP: Choose and play seq if not in process
		if ((genericAI->lastState!=AISTATE_TOOCLOSE)||(genericAI->patternState==PATTERNSTATE_FINISHED)||(!genericAI->currentSeq))
		{
			genericAI->lastState = genericAI->state;
			GetNewSeq(actorInstance,genericAI->state);
		}
		break;

	case AISTATE_ATTACKED:
		// TP: If not attacked in pursuit mode then crop players position to keep it inside the max range
		if (OUTOFRANGE(actorInstance))
		{
			// TP: crop position
			TBVector	tempVector;
			bmVectorScaleToLength(tempVector,genericAI->toCenter,genericAI->movementRange);
			bmVectorSub(tempVector,genericAI->center,tempVector);
			actorInstance->actorInstance.position[X] = tempVector[X];
			actorInstance->actorInstance.position[Z] = tempVector[Z];
		}

		// TP: Choose and play seq if not in process
		if (genericAI->lastState!=AISTATE_ATTACKED)
		{
			genericAI->lastState = genericAI->state;
			GetNewSeq(actorInstance,genericAI->state);
			genericAI->patternState = PATTERNSTATE_PROCESSINGCOMMAND;
		}

		// TP: kill character off
		if (genericAI->dying)
		{
			// TP: Cartoon poick out of existance
			genericAI->cartoonScale.stage = CARTOONSCALEDOWN_STAGEONE;
			genericAI->cartoonScale.time = 1.5f;
			genericAI->cartoonScale.type = SCALETYPE_DOWN;
			genericAI->cartoonScale.maxScale = 1.2f;
			AIStateChange(actorInstance,AISTATE_POICKOUT,AISTATE_ATTACKED);
			genericAI->characterEnabled = false;
			
			// TP: remove shadow
			SAFE_FREE(actorInstance->actorShadow);
		}

		// TP: Check for exiting attacked
		if ((genericAI->patternState==PATTERNSTATE_FINISHED)||(!genericAI->currentSeq))
		{
			AIStateChange(actorInstance,AISTATE_AWARE,AISTATE_ATTACKED);
			genericAI->currentMode = AIMODE_NORMAL;
			break;
		}
		break;
		
	case AISTATE_ATTACKEDOUTOFRANGE:
		// TP: Choose and play seq if not in process
		if (genericAI->lastState!=AISTATE_ATTACKEDOUTOFRANGE)
		{
			genericAI->lastState = genericAI->state;
			GetNewSeq(actorInstance,genericAI->state);
			genericAI->patternState = PATTERNSTATE_PROCESSINGCOMMAND;
		}

		// TP: Check for exiting attacked
		if ((genericAI->patternState==PATTERNSTATE_FINISHED)||(!genericAI->currentSeq))
		{
			// TP: Better reset character if they have been hit while out of range
			if (genericAI->resetAfterAttackOutOfRange)
			{
				AIStateChange(actorInstance,AISTATE_RESET,AISTATE_ATTACKEDOUTOFRANGE);
			}
			else
			{
				AIStateChange(actorInstance,AISTATE_AWARE,AISTATE_ATTACKEDOUTOFRANGE);
			}
			genericAI->currentMode = AIMODE_NORMAL;
			break;
		}
		break;

	case AISTATE_OUTOFRANGE:
		// TP: Choose and play seq if not in process
		if (genericAI->lastState!=AISTATE_OUTOFRANGE)
		{
			genericAI->lastState = genericAI->state;
			GetNewSeq(actorInstance,genericAI->state);
			genericAI->patternState = PATTERNSTATE_PROCESSINGCOMMAND;
		}

		// TP: Check for having been attacked
		if (GOATTACKED(actorInstance))
		{
			if (!(genericAI->flags&GENERICAI_DONTCHANGESTATE))
			{
				AIStateChange(actorInstance,AISTATE_ATTACKEDOUTOFRANGE,AISTATE_OUTOFRANGE);
				break;
			}
			else
			{
				// TP: clear the attack mode
				genericAI->currentMode = AIMODE_NORMAL;
			}
		}

		// TP: Choose and play seq if not in process
		if ((genericAI->patternState==PATTERNSTATE_FINISHED)||(!genericAI->currentSeq))
		{
			AIStateChange(actorInstance,AISTATE_IDLE,AISTATE_OUTOFRANGE);
			break;
		}
		break;

	case AISTATE_RESET:
		if (genericAI->distToCenter < genericAI->movementRange)
		{
			genericAI->lastState = genericAI->state;
			genericAI->nodeIndex=-1;
			genericAI->flags &= ~GENERICAI_DONTCHANGESTATE;
			genericAI->waitingForReset=false;
			genericAI->flags &= ~GENERICAI_NOBOUNDARY;
			AIStateChange(actorInstance,AISTATE_IDLE,AISTATE_RESET);
		}
		else
		{
			if (!actorInstance->onScreen)
			{
				// TP: perform reset and end sequence
				genericAI->lastState = genericAI->state;
				bmVectorCopy(actorInstance->actorInstance.position, genericAI->center);
				FlushAnimationQueue(actorInstance);
				genericAI->nodeIndex=-1;
				genericAI->flags &= ~GENERICAI_DONTCHANGESTATE;
				genericAI->waitingForReset=false;
				genericAI->flags &= ~GENERICAI_NOBOUNDARY;
				AIStateChange(actorInstance,AISTATE_IDLE,AISTATE_RESET);
			}
			else
			{
				// TP: Cartoon poick out of existance
				genericAI->lastState = genericAI->state;
				FlushAnimationQueue(actorInstance);
				genericAI->nodeIndex=-1;
				genericAI->flags &= ~GENERICAI_DONTCHANGESTATE;
				genericAI->waitingForReset=false;
				genericAI->flags &= ~GENERICAI_NOBOUNDARY;

				genericAI->cartoonScale.stage = CARTOONSCALEDOWN_STAGEONE;
				genericAI->cartoonScale.time = 1.5f;
				genericAI->cartoonScale.type = SCALETYPE_DOWN;
				genericAI->cartoonScale.maxScale = 1.2f;
				AIStateChange(actorInstance,AISTATE_POICKOUT,AISTATE_RESET);
			}
		}
		break;

	case AISTATE_POICKOUT:
		UpdateCartoonScaling(actorInstance,&genericAI->cartoonScale);
		if (actorInstance->actorInstance.scale[X]==0.0f)
		{
			// TP: Make sure there are no stars active when shutting down
			DestroyInjuryStars(actorInstance);
			DestroyQuestionMarkAboveActor(actorInstance);
			TurnOffSleepyZedsForCharacter(actorInstance);

			bmVectorCopy(actorInstance->actorInstance.position, genericAI->center);
			AIStateChange(actorInstance,AISTATE_INACTIVE,AISTATE_POICKOUT);
			genericAI->cartoonScale.stage = CARTOONSCALE_STAGEONE;
			genericAI->cartoonScale.time = 0.0f;
			genericAI->cartoonScale.type = SCALETYPE_UP;
			genericAI->cartoonScale.maxScale = 1.2f;
			RemoveInstanceFromInstanceList(actorInstance,genericAI->activeList);
			AddInstanceToInstanceList(actorInstance,genericAI->inactiveList);

			// TP: If center point was not validated then reset to origional center
			if (!genericAI->centerValid)
			{
				if (genericAI->useMoveBox)
				{
					bmVectorCopy(genericAI->center, genericAI->orgCenter);
				}
			}

			if (genericAI->dying) 
			{
				FreeActorInstance(actorInstance);
				return true;
			}
		}
		break;

	case AISTATE_POICKIN:
		UpdateCartoonScaling(actorInstance,&genericAI->cartoonScale);
		if (actorInstance->actorInstance.scale[X]==1.0f)
		{
			if(gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].helpFlags & MORRON_KEEPERVIRGIN)
			{
				if(actorInstance->onScreen)
				{
					CreateQuatFromPoints(tempQuat, gameStatus.player[0].actorInstance->actorInstance.position, actorInstance->actorInstance.position, 0.0f);
					if(bmQuatDot(gameStatus.player[0].actorInstance->actorInstance.orientation, tempQuat) > 0.9f)
					{
						RequestMorronicMessage(MEVENT_FIRSTKEEPER);
					}
				}
			}
			AIStateChange(actorInstance,AISTATE_IDLE,AISTATE_POICKIN);
		}
		break;
	}

	// TP: Perform standard processPattern update and get state
	if ((genericAI->state!=AISTATE_INACTIVE)&&(genericAI->lastState!=AISTATE_RESET)) genericAI->patternState = ProcessPattern(actorInstance);

	genericAI->collideWithTaz = FALSE;
	return false;
}

/*	--------------------------------------------------------------------------------
	Function 	: FacePoint
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void FacePoint(ACTORINSTANCE *actorInstance,TBVector dest,float rotTime, float offset)
{
	TBVector	tempVector;
	float		tempFloat;

	bmVectorSub(tempVector,dest,actorInstance->actorInstance.position);
	tempFloat = GetAngleAroundYFromVector(tempVector);

	tempFloat = Aabs(-(offset+tempFloat));

	CharacterRotateToAngle(actorInstance,0,tempFloat,0.0f,rotTime);
}

/*  --------------------------------------------------------------------------------
	Function	: FacingPoint
	Purpose		: Performs test to see is actorInstance is facing a point
	Parameters	: actorInstance, dest point, accuracy in radians either side
	Returns		: true if facing, false if not
	Info		: 
*/
bool FacingPoint(ACTORINSTANCE *actorInstance, TBVector dest, float offset, float accuracy)
{
	TBVector	tempVector;
	float		tempFloat,curAng;

	bmVectorSub(tempVector,dest,actorInstance->actorInstance.position);

	tempFloat = GetAngleAroundYFromVector(tempVector);

	tempFloat = Aabs(tempFloat+offset+PI);

	curAng = Aabs(GetAngleAroundYFromQuat(actorInstance->actorInstance.orientation));

	if (bmFAbs(FindShortestAngleSigned(tempFloat,curAng)) < accuracy) return true;
	else return false;
}

/*	--------------------------------------------------------------------------------
	Function 	: ReachedPoint
	Purpose 	: Test if an actor is within range of a point in the xz axis only
	Parameters 	: pos, point, range
	Returns 	: 
	Info 		: 
*/
int ReachedPoint(TBVector pos,TBVector point,float range)
{
	TBVector	tempVector;

	bmVectorSub(tempVector,pos,point);
	tempVector[Y] = 0.0f;		// only test range in XZ

	if (bmVectorLen(tempVector) < range)
		return TRUE;
	else
		return FALSE;
}

/*	--------------------------------------------------------------------------------
	Function 	: SetAISequenceMode
	Purpose 	: Sets the current sequence mode, SEQ_NORMAL, SEQ_PAUSE
	Parameters 	: ACTORINSTANCE , sequence mode
	Returns 	: 
	Info 		: 
*/
void SetAISequenceMode(ACTORINSTANCE *actorInstance, char seqMode)
{
	actorInstance->genericAI->seqMode = seqMode;
}

/*	--------------------------------------------------------------------------------
	Function 	: SetAIMessageHandler
	Purpose 	: Sets the current message handler
	Parameters 	: ACTORINSTANCE , message handler
	Returns 	: 
	Info 		: 
*/
void SetAIMessageHandler(ACTORINSTANCE *actorInstance, MessageHandler MessageCallback)
{
	actorInstance->genericAI->messageHandler = MessageCallback;
}


/*  --------------------------------------------------------------------------------
	Function	: AISoundCallback
	Purpose		: This function is called when a tracking sample finishes
	Parameters	: pointer to the handle in the ai structure
	Returns		: 
	Info		: 
*/

void AISoundCallback(void *voidPtr)
{
	ACTORINSTANCE *ptr;
	ACTORINSTANCE *actorInstance = (ACTORINSTANCE*)voidPtr;

	for (ptr = map.characterInstances.head.next;ptr != &map.characterInstances.head;ptr=ptr->next)
	{
		if (ptr == actorInstance)
		{
			// TP: to make sure the actorInstance is still valid search the character list
			if (ptr->genericAI)
			{
				ptr->genericAI->soundHandle = -1;
			}
		}
	}
	for (ptr = map.inactiveCharacterInstances.head.next;ptr != &map.inactiveCharacterInstances.head;ptr=ptr->next)
	{
		if (ptr == actorInstance)
		{
			// TP: to make sure the actorInstance is still valid search the character list
			if (ptr->genericAI)
			{
				ptr->genericAI->soundHandle = -1;
			}
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: ProcessPattern
	Purpose 	: Performs all the instructions layed down by the SEQ structure
	Parameters 	: ACTORINSTANCE 
	Returns 	: PATTERNSTATE_...
	Info 		: 
*/
EPatternState ProcessPattern(ACTORINSTANCE *actorInstance)
{
	SEQ				*seq = actorInstance->genericAI->currentSeq;
	GENERICAI		*genericAI = actorInstance->genericAI;
	int				idx,tempInt;
	char			done;
	TBVector		tempVector;
	int				advance = FALSE;

	idx = actorInstance->genericAI->seqIdx;

	if (genericAI->seqMode == SEQMODE_PAUSE)
	{
		// make sure we still call the callbacks even in PAUSE mode
		switch(seq[idx].type)
		{
		case SEQ_ATTACK:
			if (genericAI->attackCallback) genericAI->attackCallback(actorInstance);
			break;
		}
		return PATTERNSTATE_PROCESSINGCOMMAND;
	}

	if (seq==NULL) return PATTERNSTATE_FINISHED;

	done = FALSE;

	while (done==FALSE)
	{
		switch(seq[idx].type)
		{
		case SEQ_DISABLEDANIM:
			genericAI->bounceCRC = bkCRC32((uchar*)seq[idx].anim,strlen(seq[idx].anim),0);
			break;

		case SEQ_FINDNEWCENTER:
			if (genericAI->useMoveBox)
			{
				bmVectorCopy(genericAI->center,gameStatus.player1->actorInstance.position);
				ClipPositionToBox(&genericAI->moveBox,genericAI->center,genericAI->movementRange);
				genericAI->centerValid = false;
			}
			break;

		case SEQ_PLAYSAMPLE:
			if ((int)(seq[idx].time) == FALSE)
			{
				PlaySample(seq[idx].anim,255,actorInstance->actorInstance.position);
			}
			else
			{
				StopChannel(&genericAI->soundHandle);
				genericAI->soundHandle = PlayTrackingSample(seq[idx].anim,255,actorInstance->actorInstance.position,0);
				// TP: use a callback incase the sample finished before it is stopped
				CallbackWhenSampleFinished(genericAI->soundHandle, AISoundCallback, (void*)actorInstance);
			}
			break;
		case SEQ_STOPSAMPLE:
			StopChannel(&genericAI->soundHandle);
			break;
		case SEQ_BOUNDARY:
			if ((int)(seq[idx].time) == FALSE)
			{
				genericAI->flags |= GENERICAI_NOBOUNDARY;
			}
			else
			{
				genericAI->flags &= ~GENERICAI_NOBOUNDARY;
			}
			break;
		case SEQ_NODEACTIVATE:
			if ((int)(seq[idx].time) == TRUE)
			{
				genericAI->flags |= GENERICAI_NODEACTIVATE;
			}
			else
			{
				genericAI->flags &= ~GENERICAI_NODEACTIVATE;
			}
		case SEQ_SETBLENDTIME:
			genericAI->currentBlendTime = seq[idx].time;
			break;
		case SEQ_MOTIONBONE:
			actorInstance->actorAnimation->useMotionBone = (int)(seq[idx].time);
			break;
		case SEQ_MESSAGE:
			if (genericAI->messageHandler) tempInt = genericAI->messageHandler(actorInstance,seq[idx].anim);
			if (tempInt==TRUE) return PATTERNSTATE_PROCESSINGCOMMAND;
			break;
		case SEQ_SKIP:
			if (genericAI->messageHandler) tempInt = genericAI->messageHandler(actorInstance,seq[idx].anim);
			if (tempInt==TRUE) 
			{
				idx += (int)(seq[idx].time);
				actorInstance->genericAI->seqIdx = idx;
			}
			break;
		case SEQ_SETROTSPEED:
			genericAI->currentRotTime = seq[idx].time/60.0f;
			break;
		case SEQ_SETANIMSPEED:
			genericAI->currentAnimSpeed = seq[idx].time;
			break;
		case SEQ_LOCKSEQ:
			if ((int)(seq[idx].time) == TRUE)
			{
				actorInstance->genericAI->flags |= GENERICAI_DONTCHANGESTATE;
			}
			else
			{
				actorInstance->genericAI->flags &= ~GENERICAI_DONTCHANGESTATE;
			}
			break;
		case SEQ_CHANGESTATEONATTACKEDONLY:
			if ((int)(seq[idx].time) == TRUE)
			{
				actorInstance->genericAI->flags |= GENERICAI_CHANGEONATTACKEDONLY;
			}
			else
			{
				actorInstance->genericAI->flags &= ~GENERICAI_CHANGEONATTACKEDONLY;
			}
			break;
		case SEQ_STARTZZZ:
			if ((int)(seq[idx].time) == FALSE)
			{
				ActivateSleepyZedsForCharacter(actorInstance, genericAI->headNode, textureSleepyZeds, NULL, METERTOUNIT(0.03f));
			}
			else
			{
				ActivateSleepyZedsForCharacter(actorInstance, genericAI->headNode, bkLoadTexture(NULL,"one.tga",0), bkLoadTexture(NULL,"zero.tga",0), METERTOUNIT(0.03f));
			}
			break;
		case SEQ_ENDZZZ:
			TurnOffSleepyZedsForCharacter(actorInstance);
			break;
		case SEQ_STARTQUESTION:
			bmVectorCopy(tempVector, genericAI->starsOffset);
			tempVector[1] += METERTOUNIT(0.5f);
			CreateQuestionMarkAboveActor(actorInstance,genericAI->headNode,"questionmark.tga",METERTOUNIT(0.25f),tempVector);
			break;
		case SEQ_ENDQUESTION:
			DestroyQuestionMarkAboveActor(actorInstance);
			break;
		case SEQ_STARTEXCLAMATION:
			bmVectorCopy(tempVector, genericAI->starsOffset);
			tempVector[1] += METERTOUNIT(0.5f);
			CreateQuestionMarkAboveActor(actorInstance,genericAI->headNode,"exclamation.tga",METERTOUNIT(0.25f),tempVector);
			break;
		case SEQ_ENDEXCLAMATION:
			DestroyQuestionMarkAboveActor(actorInstance);
			break;
		case SEQ_SETCYLRADIUS:
			actorInstance->actorBehaviour->collCylinder[0].radius = seq[idx].time;
			break;
		case SEQ_SETCYLHEIGHT:
			actorInstance->actorBehaviour->collCylinder[0].height = seq[idx].time;
			break;
		case SEQ_SETSPHRADIUS:
			actorInstance->actorBehaviour->collSphere.radius = seq[idx].time;
			break;
		case SEQ_SETVIEWANGLE:
			actorInstance->genericAI->viewAngle = seq[idx].time;
			break;
		case SEQ_CYLCOLLISION:
			if ((int)(seq[idx].time) == FALSE)
			{
				actorInstance->actorBehaviour->collCylinder[0].radius = 0.0f;
			}
			else
			{
				actorInstance->actorBehaviour->collCylinder[0].radius = genericAI->save.collCylinderRadius;
			}
			break;
		case SEQ_SPHCOLLISION:
			if ((int)(seq[idx].time) == FALSE)
			{
				RemoveAndFreeCollisionSphereListEntry(&collisionCache, actorInstance);
			}
			else
			{
				AddActorToCollisionSphereCache(&collisionCache, actorInstance);
			}
			break;
		case SEQ_SETSTANDOFF:
			actorInstance->genericAI->standOffRange = seq[idx].time;
			break;
		case SEQ_COSTUMEDONE:
			actorInstance->genericAI->hasSeenCostume = true;
			break;
		default:
			done = TRUE;
			break;
		}
		if (done==FALSE)
		{
			if (seq[idx].type < SEQ_ENDOFSEQCOMMANDS)
			{
#ifdef AIDEBUG
				bkPrintf("	Actor %s, seq %s\n",actorInstance->instanceName,SAISeq[seq[idx].type]);
#endif
			}
			genericAI->seqTime = 0.0f;
			genericAI->seqIdx++;
			idx++;
		}
	}
	
	// process instructions
	switch(seq[idx].type)
	{
		case SEQ_DEATH:
			if(actorInstance->actorStatus->costumeChangeInfo)
			{
				if((actorInstance->actorStatus->costumeChangeInfo = UpdateLoseCostumeEffect(actorInstance, actorInstance->actorStatus->costumeChangeInfo)) == NULL)
				{
					// CMD: now done a little earlier within UpdateLoseCostumeEffect
//					actorInstance->fadeSpeed = -255.0f;
//					actorInstance->flags |= ACTORFLAG_DELETEAFTERFADEOUT;
					genericAI->dying = true;
				}
			}
			break;

		case SEQ_RESET:

			genericAI->waitingForReset = true;
			return PATTERNSTATE_FINISHED;

		case SEQ_PURSUE:
			if (!genericAI->pursueAnimCRC)
			{
				genericAI->pursueAnimCRC = bkCRC32((uchar*)seq[idx].anim, strlen(seq[idx].anim),0);
				if(gameStatus.player1->characterInfo->pTrail)
				{
					trailReset(gameStatus.player1, TRAIL_RESET_ALL);
					trailAddPosition(gameStatus.player1);
				}
				genericAI->pursuitTrailTimer = 0.0f;
				genericAI->timeStuck = 0.0f;
			}
			if (UpdatePursue(actorInstance)==TRUE)
			{
				genericAI->pursueAnimCRC = NULL;
				advance = true;
			}
			break;

		case SEQ_END:
			genericAI->flags &= ~GENERICAI_DONTCHANGESTATE;
			return PATTERNSTATE_FINISHED;
			break;

		case SEQ_STARS:
			CreateInjuryStars(10,actorInstance,genericAI->headNode,genericAI->starsOffset,genericAI->starVars);
			// TP: AddMoneyToBounty(gameStatus.player[0].actorInstance,NULL, ((bmRand()%9)+1)*100);
			advance = true;
			break;

		case SEQ_ENDSTARS:
			DestroyInjuryStars(actorInstance);
			advance = true;
			break;

		case SEQ_WAIT:
			// wait for limited time
			genericAI->seqTime += fTime;

			if (genericAI->seqTime > seq[idx].time)
			{
				advance = true;
			}
			break;

		case SEQ_DISABLED:

			StopCollidingWith(actorInstance, gameStatus.player1, 1.0f);

			if (seq[idx].anim)
			{
				if (seq[idx].time == 0.0f)
				{
					// play animation until finished
					if ((genericAI->seqTime==0.0f)&&(!CheckIfPlayingAnimation(actorInstance,seq[idx].anim)))
					{
						FlushAnimationQueue(actorInstance);
						PlayAnimationByName(actorInstance,seq[idx].anim,genericAI->currentAnimSpeed,0,0,genericAI->currentBlendTime,MOVE);
						AddActorToCollisionSphereCache(&collisionCache, actorInstance);
						actorInstance->actorBehaviour->collCylinder[0].radius = 0.0f;
						SetPreCollisionCallback(actorInstance, AIPreCharacterCollisionDisabled);
						SetCollisionCallback(actorInstance, AICharacterCollisionDisabled);
					}
				}
				else
				{
					// loop animation until time finished
					if ((!CheckIfPlayingAnimation(actorInstance,seq[idx].anim))&&(!CheckIfPlayingAnimation(actorInstance,NULL,genericAI->bounceCRC)))
					{
						FlushAnimationQueue(actorInstance);
						PlayAnimationByName(actorInstance,seq[idx].anim,genericAI->currentAnimSpeed,1,0,genericAI->currentBlendTime,MOVE);
						AddActorToCollisionSphereCache(&collisionCache, actorInstance);
						actorInstance->actorBehaviour->collCylinder[0].radius = 0.0f;
						SetPreCollisionCallback(actorInstance, AIPreCharacterCollisionDisabled);
						SetCollisionCallback(actorInstance, AICharacterCollisionDisabled);
					}
				}

				if (((seq[idx].time == 0.0f)&&(!CheckIfPlayingAnimation(actorInstance,seq[idx].anim))) ||
					(genericAI->seqTime > seq[idx].time))
				{
					// TP: wait until the characters are parted before adding the collision back on
					TBVector	tempDist;

					bmVectorSub(tempDist, actorInstance->actorInstance.position, gameStatus.player1->actorInstance.position);
					tempDist[Y] = 0.0f;

					if ((bmVectorLen(tempDist) > (genericAI->save.collCylinderRadius+gameStatus.player1->actorBehaviour->collCylinder[0].radius))
						&&(!CheckIfPlayingAnimation(actorInstance,NULL,genericAI->bounceCRC)))
					{
						RemoveAndFreeCollisionSphereListEntry(&collisionCache, actorInstance);
						actorInstance->actorBehaviour->collCylinder[0].radius = genericAI->save.collCylinderRadius;
						// TP: back to normal
						StopCollidingWith(actorInstance, NULL, 1.0f);

						SetCollisionCallback(actorInstance, AICharacterCollision);
						SetPreCollisionCallback(actorInstance, AIPreCharacterCollision);
						advance = true;
						break;
					}
				}
			}
			else
			{
				if (genericAI->seqTime==0.0f)
				{
					AddActorToCollisionSphereCache(&collisionCache, actorInstance);
					actorInstance->actorBehaviour->collCylinder[0].radius = 0.0f;
					SetPreCollisionCallback(actorInstance, AIPreCharacterCollisionDisabled);
					SetCollisionCallback(actorInstance, AICharacterCollisionDisabled);
					StopCollidingWith(actorInstance, gameStatus.player1, 1.0f);
				}
				if (genericAI->seqTime > seq[idx].time)
				{
					// TP: wait until the characters are parted before adding the collision back on
					TBVector	tempDist;

					bmVectorSub(tempDist, actorInstance->actorInstance.position, gameStatus.player1->actorInstance.position);
					tempDist[Y] = 0.0f;

					if ((bmVectorLen(tempDist) > (genericAI->save.collCylinderRadius+gameStatus.player1->actorBehaviour->collCylinder[0].radius))
						&&(!CheckIfPlayingAnimation(actorInstance,seq[idx].anim))&&(!CheckIfPlayingAnimation(actorInstance,NULL,genericAI->bounceCRC)))
					{
						RemoveAndFreeCollisionSphereListEntry(&collisionCache, actorInstance);
						actorInstance->actorBehaviour->collCylinder[0].radius = genericAI->save.collCylinderRadius;
						// TP: back to normal
						StopCollidingWith(actorInstance, NULL, 1.0f);

						SetCollisionCallback(actorInstance, AICharacterCollision);
						SetPreCollisionCallback(actorInstance, AIPreCharacterCollision);
						advance = true;
						break;
					}
				}
			}
			genericAI->seqTime += fTime;
			break;

		case SEQ_CHASE:
			// return to start position
			genericAI->seqTime += fTime;

			// if not playing animation
			if (!CheckIfPlayingAnimation(actorInstance,seq[idx].anim))
			{
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance,seq[idx].anim,genericAI->currentAnimSpeed*genericAI->attackSpeedMod,1,0,genericAI->currentBlendTime,MOVE);
			}
			FacePoint(actorInstance,gameStatus.player1->actorInstance.position,genericAI->currentRotTime);

			// TP: If reached destination the advance to the next seq
			if (ReachedPoint(actorInstance->actorInstance.position,gameStatus.player1->actorInstance.position,genericAI->standOffRange))
			{
				advance = true;
			}

			// TP: If the destination is not reachable then advance to the next sequence
			if (genericAI->useMoveBox)
			{
				if (!IsPointWithinBox(&genericAI->moveBox.attackBBox,genericAI->moveBox.invmatrix,gameStatus.player1->actorInstance.position,BOXFLAG_XTEST|BOXFLAG_ZTEST)&&(~genericAI->flags&GENERICAI_NOBOUNDARY))
				{
					// TP: Character is outside the players box
					if (seq[idx].time)
					{
						advance = (int)(seq[idx].time);
					}
					else
					{
						advance = true;
					}
				}
			}
			else
			{
				if ((genericAI->distTazToCenter > (genericAI->movementRange+genericAI->attackBoundaryRange))&&(~genericAI->flags&GENERICAI_NOBOUNDARY))
				{
					// TP: Character is outside the player movement range
					if (seq[idx].time)
					{
						advance = (int)(seq[idx].time);
					}
					else
					{
						advance = true;
					}
				}
			}
			break;

		case SEQ_ATTACKHOLDDISTANDFACE:
			TBVector	tempVector;
			float		tempFloat;

			tempFloat = bmVectorLen(genericAI->toTaz);
			if (bmFAbs(seq[idx].time - tempFloat) < METERTOUNIT(1.0f))
			{
				bmVectorScaleToLength(tempVector,genericAI->toTaz,genericAI->attackHoldDistance);
				bmVectorSub(actorInstance->actorInstance.position, gameStatus.player1->actorInstance.position, tempVector);
			}
		case SEQ_ATTACKANDFACE:
			FacePoint(actorInstance,gameStatus.player1->actorInstance.position,genericAI->currentRotTime,0.0f);
		case SEQ_ATTACK:
			if (genericAI->attackCallback) tempInt = genericAI->attackCallback(actorInstance);
			else tempInt = FALSE;

			if (!tempInt)
			{
				if (seq[idx].time == 0.0f)
				{
					// play animation until finished
					if (genericAI->seqTime==0.0f)
					{
						FlushAnimationQueue(actorInstance);
						PlayAnimationByName(actorInstance,seq[idx].anim,genericAI->currentAnimSpeed*genericAI->attackSpeedMod,0,0,genericAI->currentBlendTime,MOVE);
					}
				}
				else
				{
					// loop animation until time finished
					if (!CheckIfPlayingAnimation(actorInstance,seq[idx].anim))
					{
						FlushAnimationQueue(actorInstance);
						PlayAnimationByName(actorInstance,seq[idx].anim,genericAI->currentAnimSpeed*genericAI->attackSpeedMod,1,0,genericAI->currentBlendTime,MOVE);
					}
					if (genericAI->seqTime > seq[idx].time)
					{
						FlushAnimationQueue(actorInstance);
					}
				}

				if (!CheckIfPlayingAnimation(actorInstance,seq[idx].anim))
				{
					advance = true;
					break;
				}
			}
			genericAI->seqTime += fTime;
			break;

		case SEQ_SAVEDIRECTION:
			bmQuatCopy(genericAI->savedOrientation,actorInstance->actorInstance.orientation);			

			advance = true;
			break;

		case SEQ_RESTOREDIRECTION:
			ActorRotate( actorInstance, genericAI->savedOrientation, genericAI->currentRotTime);

			advance = true;
			break;

		case SEQ_SAVEPOSITION:
			bmVectorCopy(genericAI->savedPosition,actorInstance->actorInstance.position);

			advance = true;
			break;

		case SEQ_WALKTOPOSITION:
			// return to start position
			genericAI->seqTime += fTime;

			// if not playing animation
			if (!CheckIfPlayingAnimation(actorInstance,seq[idx].anim))
			{
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance,seq[idx].anim,genericAI->currentAnimSpeed,1,0,genericAI->currentBlendTime,MOVE);
			}

			FacePoint(actorInstance,genericAI->savedPosition,genericAI->currentRotTime);
			if (ReachedPoint(actorInstance->actorInstance.position,genericAI->savedPosition,METERTOUNIT(0.2f)))
			{
				advance = true;
			}
			break;

		case SEQ_RETURNNEWCENTER:
			if (genericAI->useMoveBox)
			{
				bmVectorCopy(genericAI->center,gameStatus.player1->actorInstance.position);
				ClipPositionToBox(&genericAI->moveBox,genericAI->center,genericAI->movementRange);
			}
			if (genericAI->distToCenter < genericAI->movementRange)
			{
				advance = true;
				break;
			}
		case SEQ_RETURN:
			// return to start position
			genericAI->seqTime += fTime;

			// if not playing animation
			if (!CheckIfPlayingAnimation(actorInstance,seq[idx].anim))
			{
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance,seq[idx].anim,genericAI->currentAnimSpeed,1,0,genericAI->currentBlendTime,MOVE);
			}

			if (ReachedPoint(actorInstance->actorInstance.position,genericAI->center,METERTOUNIT(0.4f)))
			{
				bmVectorCopy(genericAI->center, actorInstance->actorInstance.position);
				genericAI->centerValid = true;
				advance = true;
				break;
			}
			FacePoint(actorInstance,genericAI->center,genericAI->currentRotTime);
			break;

		case SEQ_WALK:
			// walk for given time
			genericAI->seqTime += fTime;

			// if not playing animation
			if (!CheckIfPlayingAnimation(actorInstance,seq[idx].anim))
			{
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance,seq[idx].anim,genericAI->currentAnimSpeed,1,0,genericAI->currentBlendTime,MOVE);
			}

			if (genericAI->seqTime > seq[idx].time)
			{
				advance = true;
			}
			break;

		case SEQ_TURNLEFT:
			// turn x degrees to left
			genericAI->seqTime += fTime;

			tempFloat = GetAngleAroundYFromQuat(actorInstance->actorInstance.orientation);
			tempFloat = Aabs((-(tempFloat+PI))-(RAD(seq[idx].time)));
			CharacterRotateToAngle(actorInstance,0.0f,tempFloat,0.0f,genericAI->currentRotTime);

			advance = true;
			break;

		case SEQ_ROTATELEFT:
			genericAI->seqTime += fTime;

			tempFloat = GetAngleAroundYFromQuat(actorInstance->actorInstance.orientation);
			tempFloat = Aabs((-(tempFloat+PI))-(RAD(45)));
			CharacterRotateToAngle(actorInstance,0.0f,tempFloat,0.0f,genericAI->currentRotTime);

			if (seq[idx].anim)
			{
				if (!CheckIfPlayingAnimation(actorInstance,seq[idx].anim))
				{
					FlushAnimationQueue(actorInstance);
					PlayAnimationByName(actorInstance,seq[idx].anim,genericAI->currentAnimSpeed,1,0,genericAI->currentBlendTime,MOVE);
				}
			}

			if (genericAI->seqTime > seq[idx].time)
			{
				advance = true;
			}
			break;

		case SEQ_ROTATERIGHT:
			genericAI->seqTime += fTime;

			tempFloat = GetAngleAroundYFromQuat(actorInstance->actorInstance.orientation);
			tempFloat = Aabs((-(tempFloat+PI))+(RAD(45)));
			CharacterRotateToAngle(actorInstance,0.0f,tempFloat,0.0f,genericAI->currentRotTime);

			if (seq[idx].anim)
			{
				if (!CheckIfPlayingAnimation(actorInstance,seq[idx].anim))
				{
					FlushAnimationQueue(actorInstance);
					PlayAnimationByName(actorInstance,seq[idx].anim,genericAI->currentAnimSpeed,1,0,genericAI->currentBlendTime,MOVE);
				}
			}

			if (genericAI->seqTime > seq[idx].time)
			{
				advance = true;
			}
			break;

		case SEQ_TURNRIGHT:
			// turn x degrees to right
			genericAI->seqTime += fTime;
	
			tempFloat = GetAngleAroundYFromQuat(actorInstance->actorInstance.orientation);
			tempFloat = Aabs((-(tempFloat+PI))+(RAD(seq[idx].time)));
			CharacterRotateToAngle(actorInstance,0.0f,tempFloat,0.0f,genericAI->currentRotTime);

			advance = true;
			break;

		case SEQ_PLAYANDFACE:
			FacePoint(actorInstance,gameStatus.player1->actorInstance.position,genericAI->currentRotTime,0.0f);
		case SEQ_PLAY:
		case SEQ_STUNNED:
			if (seq[idx].time == 0.0f)
			{
				// play animation until finished
				if (genericAI->seqTime==0.0f)
				{
					FlushAnimationQueue(actorInstance);
					PlayAnimationByName(actorInstance,seq[idx].anim,genericAI->currentAnimSpeed,0,0,genericAI->currentBlendTime,MOVE);
				}
			}
			else
			{
				if (seq[idx].time == -1.0f)
				{
					if (!CheckIfPlayingAnimation(actorInstance,seq[idx].anim))
					{
						FlushAnimationQueue(actorInstance);
						PlayAnimationByName(actorInstance,seq[idx].anim,genericAI->currentAnimSpeed,1,0,genericAI->currentBlendTime,MOVE);
					}
				}
				else
				{
					// loop animation until time finished
					if (!CheckIfPlayingAnimation(actorInstance,seq[idx].anim))
					{
						FlushAnimationQueue(actorInstance);
						PlayAnimationByName(actorInstance,seq[idx].anim,genericAI->currentAnimSpeed,1,0,genericAI->currentBlendTime,MOVE);
					}
					if (genericAI->seqTime > seq[idx].time)
					{
						FlushAnimationQueue(actorInstance);
					}
				}
			}
			if (!CheckIfPlayingAnimation(actorInstance,seq[idx].anim))
			{
				advance = true;
				break;
			}
			genericAI->seqTime += fTime;
			break;

		case SEQ_FACECAMERA:
			FacePoint(actorInstance,gameStatus.player[0].camera->pos,genericAI->currentRotTime,RAD(seq[idx].time));
			
			advance = true;
			break;

		case SEQ_FACETAZ:
			FacePoint(actorInstance,gameStatus.player1->actorInstance.position,genericAI->currentRotTime,RAD(seq[idx].time));
			
			// if not playing animation
			if (seq[idx].anim)
			{
				if (!CheckIfPlayingAnimation(actorInstance,seq[idx].anim))
				{
					FlushAnimationQueue(actorInstance);
					PlayAnimationByName(actorInstance,seq[idx].anim,genericAI->currentAnimSpeed,1,0,genericAI->currentBlendTime,MOVE);

				}
				if (FacingPoint(actorInstance, gameStatus.player1->actorInstance.position,RAD(seq[idx].time),RAD(5.0f)))
				{
					advance = true;
				}
			}
			else
			{
				advance = true;
			}
			break;

		case SEQ_LOOKAWAYFROMTAZ:
			FacePoint(actorInstance,gameStatus.player1->actorInstance.position,genericAI->currentRotTime,RAD(180.0f));

			advance = true;
			break;

		case SEQ_FACECENTER:
			FacePoint(actorInstance,genericAI->center,genericAI->currentRotTime);
			
			advance = true;
			break;

		case SEQ_LOOKAWAYFROMCENTER:
			FacePoint(actorInstance,genericAI->center,genericAI->currentRotTime);

			tempFloat = GetAngleAroundYFromQuat(actorInstance->actorBehaviour->rotAim);
			tempFloat = Aabs((-(tempFloat+PI))-(RAD(180)));
			CharacterRotateToAngle(actorInstance,0.0f,tempFloat,0.0f,genericAI->currentRotTime);

			advance = true;
			break;

		case SEQ_LOOP:
			genericAI->seqIdx = 0;
			genericAI->seqTime = 0.0f;
			return PATTERNSTATE_LOOPED;
	}

	if (advance)
	{
#ifdef AIDEBUG
		if (seq[idx].anim)
		{
			bkPrintf("	Actor %s, seq %s,%s,%d \n",actorInstance->instanceName,SAISeq[seq[idx].type],seq[idx].anim,seq[idx].time);
		}
		else
		{
			bkPrintf("	Actor %s, seq %s,NULL,%d \n",actorInstance->instanceName,SAISeq[seq[idx].type],seq[idx].time);
		}
#endif

		genericAI->seqTime = 0.0f;
		genericAI->seqIdx += advance;

		return PATTERNSTATE_COMMANDCOMPLETED;
	}
	return PATTERNSTATE_PROCESSINGCOMMAND;
}


/*	--------------------------------------------------------------------------------
	Function 	: UpdateCharacterAIInstances
	Purpose 	: Update the current AI for a character list
	Parameters 	: ACTORINSTANCELIST
	Returns 	: 
	Info 		: Updates path and states
*/
//#define ENEMYTESTCAM
void UpdateCharacterAIInstances(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE *ptr,*temp;
	bool	enemyAttack=false;

	ptr = actorInstanceList->head.next;

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;)
	{
		temp = ptr->next;
		if (ptr->genericAI)
		{
			if (!UpdateCharacterAI(ptr))
			{
				if (ptr->genericAI->useMoveBox)
				{
					if (IsPointWithinBox(&ptr->genericAI->moveBox.attackBBox,ptr->genericAI->moveBox.invmatrix,gameStatus.player1->actorInstance.position,BOXFLAG_XTEST|BOXFLAG_ZTEST))
					{
						enemyAttack = true;
					}
				}
				else
				{
					if ((ptr->genericAI->distTazToCenter < (ptr->genericAI->movementRange+ptr->genericAI->attackBoundaryRange))) enemyAttack = true;
				}
				if (!IsTazWithinHeightRange(ptr)) enemyAttack = false;
			}
		}
		ptr = temp;
	}

	// TP: only alter camera on active characters
	if (actorInstanceList==&map.characterInstances)
	{
		// TP: if the character is active
		if (enemyAttack)
		{
			if (camera[0].defaultAngleAboveHorizontal < RAD(30.0f))
			{
				camera[0].defaultAngleAboveHorizontal += RAD(15.0f)*fTime;
			}
			else
			{
				camera[0].defaultAngleAboveHorizontal = RAD(30.0f);
			}

			if (camera[0].defaultFixedDistance < 900.0f)
			{
				camera[0].defaultFixedDistance += 200.0f*fTime;
			}
			else
			{
				camera[0].defaultFixedDistance = 900.0f;
			}
		}
		else
		{
			if (camera[0].defaultAngleAboveHorizontal > CAMERAANGLE)
			{
				camera[0].defaultAngleAboveHorizontal -= RAD(15.0f)*fTime;
			}
			else
			{
				camera[0].defaultAngleAboveHorizontal = CAMERAANGLE;
			}

			if (camera[0].defaultFixedDistance > CAMERADIST)
			{
				camera[0].defaultFixedDistance -= 200.0f*fTime;
			}
			else
			{
				camera[0].defaultFixedDistance = CAMERADIST;
			}
		}
	}
}


/*  --------------------------------------------------------------------------------
	Function	: AICollideWithSpecial
	Purpose		: This function is called when the character collides with any special
	Parameters	: the character, the special object collided with
	Returns		: 
	Info		: 
*/

void AICollideWithSpecial(ACTORINSTANCE *collider, ACTORINSTANCE *specialObject)
{
	if (collider->genericAI->colliderSpecialCallback) collider->genericAI->colliderSpecialCallback(collider, specialObject);

	if (collider->genericAI->flags & GENERICAI_IGNORESPECIALCOLL) return;

	collider->genericAI->specialColl = specialObject->special;
	//collider->genericAI->waitingForReset = true;
}

/*  --------------------------------------------------------------------------------
	Function	: DistFromNodeToTaz
	Purpose		: Returns the distance between the node on an actor to Taz
	Parameters	: actorInstance, node on actor
	Returns		: distance
	Info		: 
*/
float	DistFromNodeToTaz(ACTORINSTANCE *actorInstance, char *nodeName)
{
	TBVector			tempVector,tempVector2;
	TBActorNodeInstance	*node;

	if(node = baFindNode(actorInstance->actorInstance.rootNodeInstance, nodeName))
	{
		baGetNodesWorldPosition(&actorInstance->actorInstance, node, tempVector);
	}
	else
	{
		bkPrintf("*** WARNING *** node not found\n");
		SETVECTOR(tempVector, 0.0f, 0.0f, 0.0f, 1.0f);
	}

	bmVectorAdd(tempVector2,gameStatus.player1->actorInstance.position,gameStatus.player1->actorBehaviour->collSphere.offset);
	bmVectorSub(tempVector,tempVector,tempVector2);

	return bmVectorLen(tempVector);
}

/*  --------------------------------------------------------------------------------
	Function	: FindClosestEnemyToActor
	Purpose		: This function will return the closest active or inactive character
	Parameters	: actorInstance, range of closest character, excluding
	Returns		: 
	Info		: 
*/
ACTORINSTANCE *FindClosestEnemyToActor(TBVector position, float &range, const ACTORINSTANCE* const exclude, const TBActor* const actorExclude)
{
	ACTORINSTANCE	*ptr,*ret=NULL;
	float			tempFloat;
	ACTORINSTANCELIST	*instanceList = &map.inactiveCharacterInstances;
	int				i;
	
	range = -1;

	for (i=0;i<2;i++)
	{
		if (i==1) instanceList = &map.characterInstances;

		for (ptr = instanceList->head.next;ptr != &instanceList->head;ptr = ptr->next)
		{
			if (ptr==exclude) continue;
			if (ptr->actorInstance.actor == actorExclude) continue;

			tempFloat = bmVectorDistance(position, ptr->actorInstance.position);

			if ((range==-1)||(tempFloat < range))
			{
				range = tempFloat;
				ret = ptr;
			}
		}
	}

	return ret;
}

/*  --------------------------------------------------------------------------------
	Function	: SetupCharacterBox
	Purpose		: Creates the movement box for a character
	Parameters	: actorInstance for character, name of bankobject, position, orientation and scale of the instance
	Returns		: 
	Info		: 
*/
void SetupCharacterBox(const ACTORINSTANCE* const actorInstance, char *bankObject, TBVector position, TBQuaternion orientation, TBVector scale)
{
	TBActor			*actor;
	ACTORINSTANCE	*tempInstance;
	float	tempFloat;

	if (!actorInstance->genericAI) return;

	bmVectorCopy(actorInstance->genericAI->orgCenter, actorInstance->genericAI->center);

	actor = FindActorInActorList(bkCRC32((uchar*)bankObject,strlen(bankObject),0));

	if (actor)
	{
		position[W] = 1.0f;
		scale[W] = 1.0f;
		tempInstance = CreateActorInstance(&map.levelInstances,bankObject,"temp",position,scale,orientation,0);
		if (tempInstance)
		{
			// TP: move box
			GetBoxBoundsAndMatrix(tempInstance,&actorInstance->genericAI->moveBox.bbox,actorInstance->genericAI->moveBox.invmatrix);
			bmMatCopy(actorInstance->genericAI->moveBox.matrix,tempInstance->actorInstance.objectToWorld);

			// TP: Create attack box, size up normal box by 0.7 of standoff range
			// TP: attack box
			tempFloat = 0.7f * actorInstance->genericAI->attackBoundaryRange;
			memcpy(&actorInstance->genericAI->moveBox.attackBBox, &actorInstance->genericAI->moveBox.bbox, sizeof(BOUNDINGBOX));
			EnlargeBBox(&actorInstance->genericAI->moveBox.attackBBox, actorInstance->genericAI->moveBox.matrix, actorInstance->genericAI->moveBox.invmatrix, tempFloat);

			// TP: idle box
			tempFloat = 0.7f * actorInstance->genericAI->deactivationRange;
			memcpy(&actorInstance->genericAI->moveBox.idleBox, &actorInstance->genericAI->moveBox.bbox, sizeof(BOUNDINGBOX));
			EnlargeBBox(&actorInstance->genericAI->moveBox.idleBox, actorInstance->genericAI->moveBox.matrix, actorInstance->genericAI->moveBox.invmatrix, tempFloat);

			FreeActorInstance(tempInstance);
			actorInstance->genericAI->useMoveBox = true;
			// TP: if there is a move box then never leave it
			actorInstance->genericAI->alwaysClipPosition = true;
		}
	}
}

/*  --------------------------------------------------------------------------------
	Function	: AISetShadow
	Purpose		: Define the shadow to be used by the character
	Parameters	: actorInstance, shadow height, shadow radius, shadow map radius
	Returns		: 
	Info		: 
*/

void AISetShadow(ACTORINSTANCE *actorInstance, float shadowRadius, float shadowHeight, float shadowMapRadius)
{
	if (!actorInstance->genericAI) return;

	actorInstance->genericAI->shadowHeight = shadowHeight;
	actorInstance->genericAI->shadowRadius = shadowRadius;
	actorInstance->genericAI->shadowMapRadius = shadowMapRadius;
}


/*  --------------------------------------------------------------------------------
	Function	: CheckForCharacterMessages
	Purpose		: See if the message is a character message
	Parameters	: actorInstance that sent the message, message
	Returns		: true if message used
	Info		: 
*/

bool CheckForCharacterMessages(ACTORINSTANCE *actorInstance, char *message)
{
	// TP: better check there is a message
	if (!message) return false;

	// NH: No genericAI was ocassionally found.  This would crash everything
	if (!actorInstance->genericAI) return false;

	if ((actorInstance->genericAI)&&(actorInstance->genericAI->messageHandler))
	{
		// TP: send message to characters message handler
		if (actorInstance->genericAI->messageHandler(actorInstance, message))
		{
			return true;
		}
	}
	else
	{
		//bkPrintf("*** WARNING *** Character %s sent message %s but has no message handler.\n", actorInstance->actorName, message);
	}
	
	return false;
}


/*  --------------------------------------------------------------------------------
	Function	: SetupTazPostCageIdle
	Purpose		: Called after taz has completed his in cage idle
	Parameters	: actorInstance of taz
	Returns		: 
	Info		: 
*/

void SetupTazPostCageIdle(ACTORINSTANCE *actorInstance)
{
#ifndef CONSUMERDEMO
	// TP: deduct some money for the attack depending on the difficulty

	if (map.sceneNumber != SCENE_ZOOHUB)
	{
		switch(gameStatus.globalSettings.difficulty)
		{
		case DIFF_EASY:
			startThwap(STR_FILLER, -500);
			break;
		case DIFF_MEDIUM:
			startThwap(STR_FILLER, -700);
			break;
		case DIFF_HARD:
			startThwap(STR_FILLER, -1000);
			break;
		}
	}

#endif
}
