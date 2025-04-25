// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : health.cpp
//   Purpose : handles anything to do with a character and its health
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"


#include "global.h"
#include "main.h"

#include "actors.h"
#include "status.h"
#include "thoughtbubble.h"
#include "animation.h"
#include "characters.h"
#include "maths.h"
#include "clouds.h"
#include "costumes.h"
#include "main.h"
#include "sfx.h"
#include "Boss.h"						// PP: General boss game stuff
#include "WestBoss.h"					// PP: TEMP? MILESTONE
#include "bubblegum.h"
#include "hiccup.h"
#include "collectibles.h"
#include "chillipepper.h"
#include "superburp.h"
#include "Cheats.h"						// PP: Cheats

#define DRUM1X (METERTOUNIT(-0.4f))
#define DRUM1Y (METERTOUNIT(-0.03))
#define DRUM1SCALE (0.6f)

#define DRUM2X (METERTOUNIT(0.0f))
#define DRUM2Y (METERTOUNIT(-0.03))
#define DRUM2SCALE (0.6f)

#define DRUM3X (METERTOUNIT(0.4f))
#define DRUM3Y (METERTOUNIT(-0.03))
#define DRUM3SCALE (0.6f)

/* --------------------------------------------------------------------------------
   Function : InitCharactersHealth
   Purpose : initialise the characters health structure
   Parameters : charinfo, max health
   Returns : 
   Info : 
*/
void InitCharactersHealth(CHARACTERINFO *charInfo, float health)
{
	charInfo->currentBurp = 0;
/*
	charInfo->health.maxHealth = charInfo->health.currentHealth = charInfo->health.oldHealth = health;
	charInfo->health.currentBurp = 0;
	charInfo->health.isHealthBubbleActive = FALSE;
	charInfo->health.timeHealthBubbleActive = 0.0f;
	InitHealthInfo(&charInfo->health);*/
}

/* --------------------------------------------------------------------------------
   Function : InitHealthInfo
   Purpose : initialise the characters health structure
   Parameters : charinfo, max health
   Returns : 
   Info : 
*/

void InitHealthInfo(HEALTH *health)
{
/*	TBActor		*actor;
	int			i;

	for(i=0;i<TAZHEALTH_MAX;i++)
	{
		health->info.drumsticks[i] = HEALTHSTATE_FULL;
	}
	health->info.theta = health->info.phi = 0.0f;
	health->info.angVelTheta = TAZHEALTH_THETARATE;
	health->info.angVelPhi = TAZHEALTH_PHIRATE;
	health->info.scale = TAZHEALTH_SCALE;
	health->flags = 0;

	actor = FindActorInActorList("drumstickfull.obe");
	if(actor)
	{
		baInitActorInstance(&health->info.drumstickFull, actor,0);
		baPrepActorInstance(&health->info.drumstickFull,TRUE);
		EnableActorCartoonRenderMode(&health->info.drumstickFull);
		SETVECTOR(health->info.drumstickFull.position, 0.0f, 0.0f, 0.0f, 1.0f);
		SETVECTOR(health->info.drumstickFull.scale, 1.0f, 1.0f, 1.0f, 1.0f);
		bmQuatCopy(health->info.drumstickFull.orientation, bIdentityQuaternion);
	}

	actor = FindActorInActorList("drumstickhalf.obe");
	if(actor)
	{
		baInitActorInstance(&health->info.drumstickHalf, actor,0);
		baPrepActorInstance(&health->info.drumstickHalf,TRUE);
		EnableActorCartoonRenderMode(&health->info.drumstickHalf);
		SETVECTOR(health->info.drumstickHalf.position, 0.0f, 0.0f, 0.0f, 1.0f);
		SETVECTOR(health->info.drumstickHalf.scale, 1.0f, 1.0f, 1.0f, 1.0f);
		bmQuatCopy(health->info.drumstickHalf.orientation, bIdentityQuaternion);
	}
	actor = FindActorInActorList("drumstickempty.obe");
	if(actor)
	{
		baInitActorInstance(&health->info.drumstickEmpty, actor,0);
		baPrepActorInstance(&health->info.drumstickEmpty,TRUE);
		EnableActorCartoonRenderMode(&health->info.drumstickEmpty);
		SETVECTOR(health->info.drumstickEmpty.position, 0.0f, 0.0f, 0.0f, 1.0f);
		SETVECTOR(health->info.drumstickEmpty.scale, 1.0f, 1.0f, 1.0f, 1.0f);
		bmQuatCopy(health->info.drumstickEmpty.orientation, bIdentityQuaternion);
	}*/
}
/* --------------------------------------------------------------------------------
   Function : ResetCharactersHealth
   Purpose : 
   Parameters : charinfo
   Returns : 
   Info : 
*/
void ResetCharactersHealth(CHARACTERINFO *charInfo)
{
	/*
	charInfo->health.currentHealth = charInfo->health.maxHealth;
	charInfo->health.flags &= ~HEALTHFLAG_NOPREP;*/
}

/* --------------------------------------------------------------------------------
   Function : ReduceCharactersHealth
   Purpose : Reduce the characters health by a given amount
   Parameters : charinfo, amount
   Returns : 
   Info : 
*/
void ReduceCharactersHealth(ACTORINSTANCE *actorInstance, float amount)
{
#ifdef NOHEALTH
	// TP: not any more
	return;
#endif
/*
	// PP: if we're using the INVINCIBLE cheat, we can laugh in the face of pain
	if(CheatActive(CHEAT_INVINCIBLE))
	{
		// PP: TODO: will this function only ever be used for players???
		return;
	}

	if (gameStatus.multiplayer.numPlayers == 2)
	{
		if (gameStatus.currentScene != SCENE_WESTBOSS)
			return;
	}

	actorInstance->characterInfo->health.currentHealth-=1; // 1 by default amount;
	if(actorInstance->characterInfo->health.currentHealth<0.0f) actorInstance->characterInfo->health.currentHealth = 0.0f;
	*/
}

/* --------------------------------------------------------------------------------
   Function : IncreaseCharactersHealth
   Purpose : Increase the characters health
   Parameters : charinfo, amount
   Returns : 
   Info : used when collecting food
*/
void IncreaseCharactersHealth(CHARACTERINFO *charInfo, float amount)
{
#ifdef NOHEALTH
	// TP: not any more
	return;
#endif
/*
	charInfo->health.currentHealth+=1;		//amount;
	if(charInfo->health.currentHealth>charInfo->health.maxHealth)
	{
		charInfo->health.currentHealth = charInfo->health.maxHealth;
	}*/
}

/* --------------------------------------------------------------------------------
   Function : IncreaseCharactersBurp
   Purpose : Increase the characters Burp
   Parameters : charinfo, amount
   Returns : 
   Info : used when collecting burp cans
*/
void IncreaseCharactersBurp(CHARACTERINFO *charInfo, int amount)
{
	charInfo->currentBurp+=amount;
	if(charInfo->currentBurp>1)
	{
		charInfo->currentBurp = 1;
	}
}

/*  --------------------------------------------------------------------------------
	Function	: KillAllPowerUps
	Purpose		: Like it sounds
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
void KillAllPowerUps(ACTORINSTANCE *actorInstance)
{
	if ((!actorInstance)||(!actorInstance->characterInfo)) return;

	// TP: Remove any power ups the character had
	if(actorInstance->characterInfo->powerUpInfo.bubbleGum)
	{
		FinishBubbleGum(actorInstance);
	}
	if(actorInstance->characterInfo->powerUpInfo.chilliPepper)
	{
		FinishChilliPepper(actorInstance);
	}
	if(actorInstance->characterInfo->powerUpInfo.hiccup)
	{
		FinishHiccup(actorInstance);
	}
	if(actorInstance->characterInfo->powerUpInfo.invisible)
	{
		FinishInvisibility(actorInstance);
	}
	if(actorInstance->characterInfo->currentBurp)
	{
		FinishBurp(actorInstance);
	}
}

/*  --------------------------------------------------------------------------------
	Function	: respawns all power ups
	Purpose		: spins through collectibles extra instance list
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
void RespawnAllPowerUps(void)
{
	COLLECTIBLE		*data;
	int				i;

	for(i = (collectibleSystem->noofInstances-1);i >= 0;--i)
	{
		data = (COLLECTIBLE*)collectibleSystem->dataPtrs[i];

		if(data)
		{
			if(data->powerUpInfo)
			{
				data->flags |= COLLECTIBLEFLAG_RESPAWN;
				data->powerUpInfo->respawnClock = POWERUP_RESPAWN_TIME;
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : ProcessCharacterDeath
   Purpose : Respond to a character running out of health
   Parameters : ACTORINSTANCE
   Returns : 
   Info : Internal
*/
void ProcessCharacterDeath(ACTORINSTANCE *actorInstance)
{
	DestroyInjuryStars(actorInstance);

	RemoveCostume(actorInstance);

	// TP: Remove any power ups the character had
	KillAllPowerUps(actorInstance);

	// PP: I don't want tweety to say anything when the player dies in a boss game
	// PP: I'll think up something more appropriate to each game
/*	if(!IN_BOSS_GAME)
	{
		if(characterSounds.tweety == -1)
		{
			CallbackWhenSampleFinished((characterSounds.tweety=PlaySample("tweety_voc_08c.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.tweety);
		}
	}*/

	// PP: TEMP?, MILESTONE
#ifndef CONSUMERDEMO
	if(map.sceneNumber == SCENE_WESTBOSS)
	{
		WestBoss_game.winner=1;
		WestBoss_game.loser=0;
		WestBoss_game.setState(BGSTATE_OUTRO);

		return;
	}
#endif
	
	if(!CheatActive(CHEAT_COSTUME))// PP: if the costume cheat isn't on...
	{
		// TP: If player dies then remove costume
		RemoveCostume(actorInstance);
	}

	if(actorInstance == gameStatus.player1)
	{
		// CMD: just for now reload the scene when taz dies at the hands of MechaTweety
		// CMD: curly brackets make everything just that bit more readable, don't you think?
		if(map.sceneNumber == SCENE_TAZBOSS)
		{
			bkGenerateEvent("scenechange", "_tazboss2");
		}
		else
		{
			bkGenerateEvent("scenechange","_death");
		}
	}
	else
	{
		bkGenerateEvent("scenechange","_death2");
	}
}

static void myStateChange(ACTORINSTANCE *actorInstance)
{
	int i;

	i = bmRand()%3;
	FlushAnimationQueue(actorInstance);
	switch(i)
	{
	case 0:
		PlayAnimationByName(actorInstance, "tazdie1", 1.0f, 0, 0, 0.1f, DEATH);
		break;
	case 1:
		PlayAnimationByName(actorInstance, "tazdie2", 1.0f, 0, 0, 0.1f, DEATH);
		break;
	case 2:
		PlayAnimationByName(actorInstance, "tazdie3", 1.0f, 0, 0, 0.1f, DEATH);
		break;
	}
}

/* --------------------------------------------------------------------------------
   Function : PrepCharactersHealth
   Purpose : Update the thought bubble with current health if needed and looks for characters death
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void PrepCharactersHealth(ACTORINSTANCE *actorInstance)
{
	CHARACTERINFO	*charInfo = actorInstance->characterInfo;

#ifdef NOHEALTH
	// TP: not any more
	return;
#endif

/*
	if (!charInfo) return;
	if(actorInstance->characterInfo->health.flags&HEALTHFLAG_NOPREP) return;

	if (charInfo->health.currentHealth != charInfo->health.oldHealth)
	{
		// health has changed in last frame
		// check for death
		if (charInfo->health.currentHealth == 0.0f)
		{
			RequestStateChangePlus(actorInstance, STATE_DEAD, myStateChange);
			// TP: Set coutdown timer for after death
			charInfo->timer = 0.5f;
		}

		charInfo->health.isHealthBubbleActive = TRUE;
		charInfo->health.timeHealthBubbleActive = 0.0f;
			
		charInfo->health.oldHealth = charInfo->health.currentHealth;
	}

	// do general update
	if (charInfo->health.isHealthBubbleActive)
	{
		charInfo->health.timeHealthBubbleActive += fTime;
		if (charInfo->health.timeHealthBubbleActive > TAZHEALTH_DISPLAYTIME)
		{
			charInfo->health.isHealthBubbleActive = FALSE;
		}
	}
	*/
}

/* --------------------------------------------------------------------------------
   Function : CreateHealthThoughtBubble
   Purpose : Sets up a health thought bubble
   Parameters : CHARACTERINFO
   Returns : 
   Info : 
*/
int CreateHealthThoughtBubble(CHARACTERINFO *charInfo)
{
/*	TBVector tempVector;
	BOOL	retVal;
	THOUGHTBUBBLE *thoughtBubble = charInfo->thoughtBubble;

	tempVector[X] = 0;
	tempVector[Y] = METERTOUNIT(1.0f);
	tempVector[Z] = 0;
	retVal = CreateThoughtBubble(thoughtBubble,tempVector,METERTOUNIT(1.5f),METERTOUNIT(0.75),5,NULL);
	if (retVal)
	{
		if (charInfo->health.currentHealth > 2)
		{
			AddObjectToThoughtBubble(thoughtBubble,"drumstickfull.obe",DRUM1X,DRUM1Y,DRUM1SCALE);
			if (charInfo->health.currentHealth > 4)
			{
				AddObjectToThoughtBubble(thoughtBubble,"drumstickfull.obe",DRUM2X,DRUM2Y,DRUM2SCALE);
				if (charInfo->health.currentHealth == 5)
				{
					AddObjectToThoughtBubble(thoughtBubble,"drumstickhalf.obe",DRUM3X,DRUM3Y,DRUM3SCALE);
				}
				else
				{
					AddObjectToThoughtBubble(thoughtBubble,"drumstickfull.obe",DRUM3X,DRUM3Y,DRUM3SCALE);
				}
			}
			else
			{
				if (charInfo->health.currentHealth == 3)
				{
					AddObjectToThoughtBubble(thoughtBubble,"drumstickhalf.obe",DRUM2X,DRUM2Y,DRUM2SCALE);
				}
				else
				{
					AddObjectToThoughtBubble(thoughtBubble,"drumstickfull.obe",DRUM2X,DRUM2Y,DRUM2SCALE);
				}

				AddObjectToThoughtBubble(thoughtBubble,"drumstickempty.obe",DRUM3X,DRUM3Y,DRUM3SCALE);
			}
		}
		else
		{
			switch(charInfo->health.currentHealth)
			{
			case 0:
				AddObjectToThoughtBubble(thoughtBubble,"drumstickempty.obe",DRUM1X,DRUM1Y,DRUM1SCALE);
				break;
			case 1:
				AddObjectToThoughtBubble(thoughtBubble,"drumstickhalf.obe",DRUM1X,DRUM1Y,DRUM1SCALE);
				break;
			case 2:
				AddObjectToThoughtBubble(thoughtBubble,"drumstickfull.obe",DRUM1X,DRUM1Y,DRUM1SCALE);
				break;
			}
			AddObjectToThoughtBubble(thoughtBubble,"drumstickempty.obe",DRUM2X,DRUM2Y,DRUM2SCALE);
			AddObjectToThoughtBubble(thoughtBubble,"drumstickempty.obe",DRUM3X,DRUM3Y,DRUM3SCALE);
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}*/
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : DrawCharactersHealth
   Purpose : Draws any items needed in relation to health and thought bubble
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void DrawCharactersHealth(ACTORINSTANCE *actorInstance)
{
	TBMatrix	worldTrans, localTrans;
	TBMatrix	scale, result, tempMat, final;
	TBMatrix	rotTheta, rotPhi, extraRot, resultRot;
	TBActorInstance	*drawActor;
	int			i;
	float		angle;

#ifdef NOHEALTH
	// TP: not any more
	return;
#endif

/*	if(actorInstance->characterInfo->health.isHealthBubbleActive)
	{
		bmMatTranslate(worldTrans, actorInstance->actorInstance.position[0], actorInstance->actorInstance.position[1], actorInstance->actorInstance.position[2]);
		bmMatTranslate(localTrans, 0.0f, METERTOUNIT(1.2f), METERTOUNIT(0.5f));
		bmMatScale(scale, actorInstance->characterInfo->health.info.scale,actorInstance->characterInfo->health.info.scale,actorInstance->characterInfo->health.info.scale);
		bmMatYRotation(rotPhi, actorInstance->characterInfo->health.info.phi);
		bmMatXRotation(extraRot, TAZHEALTH_AXISANGLE);

		angle = actorInstance->characterInfo->health.info.theta;
		bmMatMultiply(tempMat, localTrans, scale);

		bmMatMultiply(result, rotPhi, extraRot);
		bmMatMultiply(resultRot, worldTrans, result);

		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
		baSetGlobalAlpha(256);

		for(i=0;i<actorInstance->characterInfo->health.maxHealth;i++)
		{
			switch(actorInstance->characterInfo->health.info.drumsticks[i])
			{
			case HEALTHSTATE_FULL:
				drawActor = &actorInstance->characterInfo->health.info.drumstickFull;
				break;
			case HEALTHSTATE_HALF:
				drawActor = &actorInstance->characterInfo->health.info.drumstickHalf;
				break;
			case HEALTHSTATE_NONE:
				drawActor = &actorInstance->characterInfo->health.info.drumstickEmpty;
				break;
			default:
				drawActor = NULL;
				continue;
			}
			EnableActorCartoonRenderMode(drawActor);
#if BPLATFORM == PC
	bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
#endif
			if(i)
			{
				angle += RAD(360.0f/actorInstance->characterInfo->health.maxHealth);
			}
			bmMatYRotation(rotTheta, angle);

			bmMatMultiply(result, rotTheta, tempMat);
			bmMatMultiply(final, resultRot, result);

			bmMatCopy(drawActor->objectToWorld, final);
			baDrawActorInstance(drawActor,BACTPRIMALPHATYPE_ALL);
		}

		bdSetRenderState(BDRENDERSTATE_TEXTUREFILTER, gameStatus.gfxSettings.filterMode, FALSE);
		bdSetRenderState(BDRENDERSTATE_BACKFACECULL, FALSE, FALSE);
	}*/
}

/* --------------------------------------------------------------------------------
   Function : ResetCharacterPropertiesAfterDeath
   Purpose : 
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/

void ResetCharacterPropertiesAfterDeath(ACTORINSTANCE *actorInstance)
{
	// TP: moved this into ResetCharactersAfterDeath ResetCharactersHealth(actorInstance->characterInfo);
	// TP: Reset physics
	SetActorInertia(actorInstance->actorBehaviour,0);
	SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
	SetActorMaxSpeed(actorInstance->actorBehaviour,METERTOUNIT(9)); // set max speed 8mps
}

/*  --------------------------------------------------------------------------------
	Function	: ResetCharacterAfterDeath
	Purpose		: Reset the characters position, orientation, camera and health
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
void ResetCharacterAfterDeath(ACTORINSTANCE *actorInstance)
{
	CAMERAINFO	*cameraInfo;
	TBVector	tempVector;

	if (actorInstance->playerNo==-1) return;

	cameraInfo = gameStatus.player[actorInstance->playerNo].camera;

	if (!cameraInfo) return;

	ResetCharactersHealth(actorInstance->characterInfo);

	StopCameraCase(cameraInfo,cameraInfo->cameraCase);
	RequestStateChange(cameraInfo->ourActorInstanceLookAt,STATE_INIT);
	SetActorPosition(cameraInfo->ourActorInstanceLookAt, cameraInfo->ourActorInstanceLookAt->actorStatus->startPosition);
	SetActorOrientation(cameraInfo->ourActorInstanceLookAt, cameraInfo->ourActorInstanceLookAt->actorStatus->startQuaternion);

	bmVectorSet(tempVector,0.0f,0.0f,METERTOUNIT(2.0f),1.0f);
	VectorQuatMultiply(tempVector,cameraInfo->ourActorInstanceLookAt->actorStatus->startQuaternion);
	bmVectorAdd(tempVector,tempVector,cameraInfo->ourActorInstanceLookAt->actorInstance.position);

	SetCameraPosition(cameraInfo, tempVector);//cameraInfo.resetPoint
	SetCameraLookAtPosition(cameraInfo,cameraInfo->ourActorInstanceLookAt->actorInstance.position);
}

/* --------------------------------------------------------------------------------
   Function : UpdateCharactersHealthDisplay
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateCharactersHealthDisplay(HEALTH *health)
{
	int			i;
	float		currentHealth;

	//update health level
	if(health->currentHealth != health->oldHealth) return;

	currentHealth = health->currentHealth;
	for(i=0;i<(int)TAZHEALTH_MAX;i++)
	{
		if(currentHealth >= 1.0f)
		{
			health->info.drumsticks[i] = HEALTHSTATE_FULL;
		}
		else
		if(currentHealth > 0.0f)
		{
			health->info.drumsticks[i] = HEALTHSTATE_HALF;
		}
		else
		{
			health->info.drumsticks[i] = HEALTHSTATE_NONE;
		}
		currentHealth--;
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateCharactersHealthDisplayGeometry
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateCharactersHealthDisplayGeometry(ACTORINSTANCE *actorInstance)
{
	HEALTH		*health;
	float		scaleDelta;
	float		tempAngle;
	TBVector	tempVector;
	int			i;

#ifdef NOHEALTH
	// TP: not any more
	return;
#endif

/*	if(!actorInstance->characterInfo) return;

	health = &actorInstance->characterInfo->health;
	tempAngle = RAD(360.0f)/actorInstance->characterInfo->health.maxHealth*(actorInstance->characterInfo->health.maxHealth-1.0f);

	//update orientations of health display
	if(health->isHealthBubbleActive)
	{
		if(health->timeHealthBubbleActive < 1.0f)
		{
			// TP: Update the health
			UpdateCharactersHealthDisplay(&actorInstance->characterInfo->health);

			//find last drumstick
			for(i=0;i<(int)actorInstance->characterInfo->health.maxHealth;i++)
			{
				if(health->info.drumsticks[i] == HEALTHSTATE_NONE)
				{
					if(health->currentHealth > (float)(i))
					{
						tempAngle = RAD(360.0f)/actorInstance->characterInfo->health.maxHealth*(float)(i);
						break;
					}
					else
					{
						tempAngle = RAD(360.0f)/actorInstance->characterInfo->health.maxHealth*(float)(i-1);
						break;
					}
				}
			}
			bmVectorSub(tempVector,gameStatus.player[actorInstance->playerNo].camera->pos,actorInstance->actorInstance.position);
			health->info.theta = -tempAngle - GetAngleAroundYFromVector(tempVector);
			health->info.phi = 0.0f;
		}
		else
		{
			if((health->info.theta += (health->info.angVelTheta*fTime)) > RAD(360.0f))
			{
				health->info.theta -= RAD(360.0f);
			}
			if((health->info.phi += (health->info.angVelPhi*fTime)) > RAD(360.0f))
			{
				health->info.phi -= RAD(360.0f);
			}
		}
		//koik the scale
		if(health->timeHealthBubbleActive < 1.0f)
		{
			scaleDelta = TAZHEALTH_SCALE*bmCos(RAD(1080.0f)*health->timeHealthBubbleActive)*bmCos(RAD(90.0f)*health->timeHealthBubbleActive);
		}
		else
		if(health->timeHealthBubbleActive > (TAZHEALTH_DISPLAYTIME-1.0f))
		{
			scaleDelta = -TAZHEALTH_SCALE*0.5f*bmCos(RAD(1080.0f)*health->timeHealthBubbleActive)*bmSin(RAD(90.0f)*(health->timeHealthBubbleActive-(TAZHEALTH_DISPLAYTIME-1.0f)));
		}
		else
		{
			scaleDelta = 0.0f;
		}
		health->info.scale = TAZHEALTH_SCALE + scaleDelta;
	}*/
}