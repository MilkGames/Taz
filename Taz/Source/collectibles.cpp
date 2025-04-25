// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : collectibles.cpp
//   Purpose : functions for creating and updating collectibles
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"

#include "collectibles.h"
#include "main.h"

#include "maths.h"
#include "collisioncache.h"
#include "shadow.h"
#include "physics.h"
#include "camera.h"
#include "scene.h"

#include "control.h"
#include "characters.h"
#include "sfx.h" 
#include "playerstats.h"
#include "status.h"
#include "debris.h"
#include "tutorial.h"
#include "destruct.h"
#include "subtitle.h"

#include "lights.h"
#include "animation.h"
#include "laser.h"
#include "breakables.h"
#include "status.h"
#include "eating.h"
#include "bubblegum.h"
#include "textures.h"
#include "chillipepper.h"
#include "util.h"
#include "display.h"				// PP: display stuff

#include "Cheats.h"					// PP: Cheats (for CHEAT_QWILSON)

#include "tazboss1.h"				// SO: Taz vs. Tycoon Sam boss game
#include "minecart.h"
#include "thwap.h"
#include "glowaroundplinth.h"
#include "gui.h"
#include "superburp.h"


#define COLLECTIBLE_SHADOW_RANGE	(METERTOUNIT(10.0f))
#define MAX_COLLECTIBLE_RANGE		(METERTOUNIT(13.0f))

// PP: GRRRRRR Z-BIAS
#if(BPLATFORM == PS2)
#define COLLECTIBLE_GLOW_ZBIAS		METERTOUNIT(0.2f)
#else// PP: not ps2
#define COLLECTIBLE_GLOW_ZBIAS		0.0f
#endif// PP: not ps2

TBExtraSystem	*collectibleSystem;

static ACTORINSTANCE	tempInstance;
static PICKUPINFO		pickupInfo;

char	clearCollectables = FALSE;		// SO: Update callback monitors this flag to clear all collectables.  Note: Used on TazBoss1 to control chilli appearance
static	bool	secretItemState = true;	// TP: initial state of the secret item

char	numPickupsRegen;

SCREENSCORELIST			screenScoreList;

/* --------------------------------------------------------------------------------
   Function : InitCollectibleSystem
   Purpose : call this function to create the collectible extra system
   Parameters : 
   Returns : `
   Info : 
*/
void InitCollectibleSystem(void)
{
	collectibleSystem = baCreateExtraSystem("collectible",CollectibleExtraCreateCallback,
		CollectibleExtraUpdateCallback,CollectibleExtraDeleteCallback,CollectibleExtraRenderCallback,
		CollectibleExtraSystemInitCallback,sizeof(COLLECTIBLE),MAX_COLLECTIBLES,0,0);
	InitActorStructures(&tempInstance);

	InitScreenScoreList();
}

/*  --------------------------------------------------------------------------------
	Function	: ActivateSecretItem
	Purpose		: Used to make the secret item visable and active
	Parameters	: 
	Returns		: 
	Info		: 
*/

void ActivateSecretItem(void)
{
	secretItemState = true;
}

/*  --------------------------------------------------------------------------------
	Function	: DeactivateSecretItem
	Purpose		: Used to deactivate and hide the secret item
	Parameters	: 
	Returns		: 
	Info		: 
*/

void DeactivateSecretItem(void)
{
	secretItemState = false;
}

/* --------------------------------------------------------------------------------
   Function : CreateCollectibleActor
   Purpose : Call this function to create a collectilble actor
   Parameters : actor to create collectible from,pos,life,radius,type
   Returns : TRUE /FALSE
   Info : 
*/
int CreateCollectibleActor(char *actorName, TBVector position, float life, int radius, char type, int respawn, int ident)
{
	TBActor	*actor;
	uint32	crc;
	int		colour[4];

	uchar			*temp;
	COLLECTIBLE		*data;
	float			tempFloat;

//	bkPrintf("CreateCollectibleActor(%s,,%d,%d,%d)\n",actorName,life,radius,type);
	actor = FindActorInActorList(actorName);
	if (actor)
	{
		switch(type)
		{
		case COLLECTIBLETYPE_BURPCAN:
			colour[0] = 189;colour[1] = 0;colour[2] = 255;colour[3] = POWERUP_GLOW_ALPHA;
			break;
		case COLLECTIBLETYPE_TESTTUBE:
			colour[0] = 255;colour[1] = 255;colour[2] = 255;colour[3] = POWERUP_GLOW_ALPHA;
			break;
		case COLLECTIBLETYPE_HICCUP:
			colour[0] = 255;colour[1] = 54;colour[2] = 0;colour[3] = POWERUP_GLOW_ALPHA;
			break;
		case COLLECTIBLETYPE_BUBBLEGUM:
			colour[0] = 0;colour[1] = 231;colour[2] = 255;colour[3] = POWERUP_GLOW_ALPHA;
			break;
		case COLLECTIBLETYPE_CHILLIPEPPER:
			colour[0] = 255;colour[1] = 255;colour[2] = 0;colour[3] = POWERUP_GLOW_ALPHA;
			break;
		case COLLECTIBLETYPE_DE_EXTRATIME:
			colour[0] = 255;colour[1] = 255;colour[2] = 0;colour[3] = POWERUP_GLOW_ALPHA;
			break;
		case COLLECTIBLETYPE_DE_DOUBLECRATE:
			colour[0] = 255;colour[1] = 255;colour[2] = 0;colour[3] = POWERUP_GLOW_ALPHA;
			break;
		case COLLECTIBLETYPE_DE_SLOMO:
			colour[0] = 255;colour[1] = 255;colour[2] = 0;colour[3] = POWERUP_GLOW_ALPHA;
			break;
		case COLLECTIBLETYPE_SECRETITEM:
			colour[0] = 128;colour[1] = 128;colour[2] = 0;colour[3] = POWERUP_GLOW_ALPHA;
			break;
		}
		crc = bkCRC32((uchar*)actorName,strlen(actorName),0);

		if(type == COLLECTIBLETYPE_DE_EXTRATIME || type == COLLECTIBLETYPE_DE_DOUBLECRATE || type == COLLECTIBLETYPE_DE_SLOMO)
		{
			tempFloat = DE_POWERUP_GLOW_RADIUS;
			temp = baCreateExtra(collectibleSystem,actor,NULL,position,&life,radius,type,crc,colour[0],colour[1],colour[2],colour[3],&tempFloat,respawn,ident);
		}
		else
		{
			tempFloat = POWERUP_GLOW_RADIUS;
			temp = baCreateExtra(collectibleSystem,actor,NULL,position,&life,radius,type,crc,colour[0],colour[1],colour[2],colour[3],&tempFloat,respawn,ident);
		}
	}
	switch(type)
	{
	case COLLECTIBLETYPE_DE_EXTRATIME:
	case COLLECTIBLETYPE_DE_DOUBLECRATE:
	case COLLECTIBLETYPE_DE_SLOMO:
		map.mapStats.numCollectibles += 1;
		break;
	case COLLECTIBLETYPE_PICKUP:
		map.mapStats.numCollectibles += 1;

		data = (COLLECTIBLE *) temp;

		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].swirls[data->ident])
			data->flags |= COLLECTIBLEFLAG_DISABLED;

		break;
	case COLLECTIBLETYPE_FOOD:
	case COLLECTIBLETYPE_BIGFOOD:
		map.mapStats.numFood += 1;
		break;
	case COLLECTIBLETYPE_BURPCAN:
	case COLLECTIBLETYPE_TESTTUBE:
	case COLLECTIBLETYPE_HICCUP:
	case COLLECTIBLETYPE_BUBBLEGUM:
	case COLLECTIBLETYPE_CHILLIPEPPER:	
		map.mapStats.numBurpCans += 1;
		break;
	}
	return FALSE;
}

/*  --------------------------------------------------------------------------------
	Function	: HasPickupBeenCollected
	Purpose		: Like it sounds
	Parameters	: number of pickup
	Returns		: true/false
	Info		: 
*/
bool HasPickupBeenCollected(int	number)
{
	// TP: if no save slot then assume not collected
	if (gameStatus.globalSettings.currentSaveSlot==-1) return false;

	if (number >= 120)
	{
		bkPrintf("*** WARNING *** More that 120 collectible pickups have been found.\n");
		return false;
	}

	if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].swirls[number]) return true;
	
	return false;
}

/* --------------------------------------------------------------------------------
   Function : CreateCollectibleTexture
   Purpose : Call this function to create a collectilble texture
   Parameters : actor to create collectible from,pos,life,radius,type
   Returns : TRUE /FALSE
   Info : 
*/
int CreateCollectibleTexture(char *textureName, TBVector position, float life, int radius, char type, int colour[3],int ident)
{
	TBTexture *texture;
	uint32	crc;
	float	zero = 0.0f;

	texture = bkLoadTexture(NULL, textureName, 0);
	crc = bkCRC32((uchar*)textureName,strlen(textureName),0);
	if (texture)
	{
		baCreateExtra(collectibleSystem,NULL,texture,position,&life,radius,type,crc,colour[0],colour[1],colour[2],128,&zero,0,ident);
	}
	switch(type)
	{
	case COLLECTIBLETYPE_PICKUP:
		map.mapStats.numCollectibles += 1; 
		break;
	}
	return FALSE;
}


/* --------------------------------------------------------------------------------
   Function : CollectibleCreateCallback
   Purpose : This function is called when a collectible is created
   Parameters : 
   Returns : 
   Info : 
*/
void CollectibleExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms)
{
    COLLECTIBLE *data = (COLLECTIBLE*)vData;
	TBActor	*actor;
	TBTexture *texture;
	float	*position;
	float	life;
	int		colour[4];
	float	size;
	int		respawn;

    if (parms)
	{
		actor = (TBActor*)va_arg(parms, TBActor*);
		texture = (TBTexture*)va_arg(parms, TBTexture*);
		position = va_arg(parms, float*);
		life = *va_arg(parms,float*);
		data->radius = va_arg(parms,int);
		data->type = (char)va_arg(parms,int);
		data->crc = (uint32)va_arg(parms,int);
		colour[0] = va_arg(parms,int);
		colour[1] = va_arg(parms,int);
		colour[2] = va_arg(parms,int);
		colour[3] = va_arg(parms,int);
		size = *va_arg(parms,float*);
		respawn = va_arg(parms,int);
		data->ident = va_arg(parms,int);

		data->textureRot = 0.0f;
		data->phaseAngle = UtilGenerateRandomNumber(360.0f, 0.0f);
		data->rotAngle = UtilGenerateRandomNumber(RAD(360.0f), 0.0f);

		data->counter = 0.0f;
		data->glowAlpha = 0.0f;
		if (data->type == COLLECTIBLETYPE_PICKUP)
		{
			data->colour = COLOUR(colour[0],colour[1],colour[2],127);
		}
		else
		{
			data->colour = COLOUR(127,127,127,127);
		}
		data->flags = 0;
		if ((data->type == COLLECTIBLETYPE_TAZCAM)||(data->type == COLLECTIBLETYPE_PICKUP))
		{
			data->state = COLLECTIBLE_OFF;
		}
		if((data->type == COLLECTIBLETYPE_BURPCAN)||(data->type == COLLECTIBLETYPE_TESTTUBE)||
			(data->type == COLLECTIBLETYPE_BUBBLEGUM)||(data->type == COLLECTIBLETYPE_HICCUP)||
			(data->type == COLLECTIBLETYPE_CHILLIPEPPER)||(data->type == COLLECTIBLETYPE_DE_EXTRATIME)||
			(data->type == COLLECTIBLETYPE_DE_DOUBLECRATE)||(data->type == COLLECTIBLETYPE_DE_SLOMO)
			||(data->type == COLLECTIBLETYPE_SECRETITEM))
		{
			data->powerUpInfo = (COLLECTIBLEPOWERUP*)ZALLOC(sizeof(COLLECTIBLEPOWERUP));
			if(data->powerUpInfo)
			{
				data->powerUpInfo->glowColour[0] = colour[0];
				data->powerUpInfo->glowColour[1] = colour[1];
				data->powerUpInfo->glowColour[2] = colour[2];
				data->powerUpInfo->glowColour[3] = colour[3];
				data->powerUpInfo->glowSize = size;
				data->powerUpInfo->glowTilt = RAD(0.0f);
				data->powerUpInfo->clock = 0.0f;
				data->powerUpInfo->respawn = respawn;
				data->powerUpInfo->respawnClock = 0.0f;
			}
			data->flags |= COLLECTIBLEFLAG_POWERUP;
		}
		else
		{
			data->powerUpInfo = NULL;
		}

		data->position[X] = position[X];
		data->position[Y] = position[Y];
		data->position[Z] = position[Z];

		data->life = life;
		data->shadow = NULL;
		data->flags |= COLLECTIBLEFLAG_NEW;
		if ((data->type == COLLECTIBLETYPE_TAZCAM)||(data->type == COLLECTIBLETYPE_PICKUP))
		{
			data->flags |= COLLECTIBLEFLAG_DONTDRAW;
		}
	
		if (actor)
		{
			data->actorInstance = (TBActorInstance*)ZALLOC(sizeof(TBActorInstance));
			if (!data->actorInstance) return;

			baInitActorInstance(data->actorInstance, actor,5);
			bmVectorCopy(data->actorInstance->position,data->position);
			
			if((data->type == COLLECTIBLETYPE_PICKUP)||(data->type == COLLECTIBLETYPE_SECRETITEM))
			{
				SETVECTOR(data->actorInstance->scale, 1.0f, 1.0f, 1.0f, 1.0f);
			}
			else
			{
				float scale = data->radius/actor->maxRadius;
				SETVECTOR(data->actorInstance->scale,scale,scale,scale,1.0f);
			}

			baPrepActorInstance(data->actorInstance,TRUE);
			
			baSetActorRenderMode(data->actorInstance, 0, BACTRENDERMODE_NORMAL, NULL);
			baSetActorRenderMode(data->actorInstance, 1, BACTRENDERMODE_NONE, NULL);
			baSetNodeFrame(data->actorInstance, data->actorInstance->rootNodeInstance, 0, TRUE);
			baFlushNodeQueue(data->actorInstance, data->actorInstance->rootNodeInstance, TRUE);
			if ((data->type!=COLLECTIBLETYPE_SECRETITEM)&&((MORE_CARTOON_RENDERING)||(data->type!=COLLECTIBLETYPE_PICKUP)))
			{
				EnableActorLineRenderMode(data->actorInstance);
			}
			if (data->actorInstance->numAnimSegments)
			{
				baQueueNodeAnimNormal(data->actorInstance, data->actorInstance->rootNodeInstance, 
							&data->actorInstance->animSegments[0], 512+(bmRand()%200), -1, BANIMQUEUEFLAG_LOOPING, NULL,NULL);
			}
		}
		else
		{
			// TP: must be a texture
			data->texture = texture;
			data->pulse = (float)(bmRand()%10)/10.0f;
			data->twinkleClock = 0.0f;
			data->twinkleDeadTime = 0.0f;
		}
		if((data->type == COLLECTIBLETYPE_SECRETITEM)&&(secretItemState))
		{
			CreateGlowAroundSecretItem((void*)data);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : CollectibleExtraDeleteCallback
   Purpose : This function is called when a collectible is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int CollectibleExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata)
{
	COLLECTIBLE *data = (COLLECTIBLE*)vdata;

	if (data->actorInstance)
	{
		// if it has an actor then delete it
		if (data->shadow)
		{
			FREE(data->shadow);
			data->shadow = NULL;
		}
		if (data->actorInstance) 
		{
			baFreeActorInstance(data->actorInstance);
			FREE(data->actorInstance);
		}
		data->actorInstance = NULL;
	}
	if (data->powerUpInfo) FREE(data->powerUpInfo);
	return TRUE;
}

void CollectibleStateChange(ACTORINSTANCE *actorInstance)
{
	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance, "electrocute", 1.0f, 0, 0, 0.1f, NONE);
	PlayAnimationByName(actorInstance, "electrocute", 1.0f, 0, 1, 0.1f, NONE);
	PlayAnimationByName(actorInstance, "electrocute", 1.0f, 0, 1, 0.1f, NONE);
	PlaySample("electricspark.wav",255,actorInstance->actorInstance.position);
}

/* --------------------------------------------------------------------------------
   Function : CollectibleExtraUpdateCallback
   Purpose : This function is called each frame to update the collectibles
   Parameters : 
   Returns : 
   Info : 
*/
int ReadyForCollectableRespawn(COLLECTIBLE *data)
{
#ifndef CONSUMERDEMO
	if(map.sceneNumber == SCENE_TAZPREBOSS)
	{
		return tazBoss1Game.readyForChilliRespawn();
	}
#endif

	return ((data->powerUpInfo->respawnClock += fTime) > POWERUP_RESPAWN_TIME);
}

void CollectibleExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t)
{
	COLLECTIBLE		*data;
	int				i = 0;
	TBVector		tempVector;
	float			distance=0;
	char			reactivatePickups = FALSE;
	float			ground;

	/*while (numPickupsRegen && gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].pickupsCollected)	// JW: we've got pickups to regenerate
	{
		data = (COLLECTIBLE*)dataPtrs[i];		
		
		if (data->type == COLLECTIBLETYPE_PICKUP)
		{
			if (data->flags&COLLECTIBLEFLAG_DISABLED)
			{
				if (!(bmRand()%3))	// JW: If a pickup has previously been disabled, it has a 1 in 3 chance of being the one to regen if you lose it
				{
					data = (COLLECTIBLE*)dataPtrs[i];		
					
					data->flags	&= ~COLLECTIBLEFLAG_DISABLED;
					
					gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].swirls[data->ident] = false;
					
					gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].pickupsCollected --;
					
					numPickupsRegen--;
				}
			}
		}
		
		i ++;

		if (i == noofInstances)
			i = 0;
	}

	numPickupsRegen = 0;*/

	for (i=0;i<noofInstances;i++)
	{
		data = (COLLECTIBLE*)dataPtrs[i];

		if (reactivatePickups)
		{
			data->flags &= ~COLLECTIBLEFLAG_DISABLED;
			data->state = COLLECTIBLE_OFF;
		}

		if (data->type == COLLECTIBLETYPE_SECRETITEM)
		{
			if (secretItemState)
			{
				if (data->flags&COLLECTIBLEFLAG_INACTIVE)
				{
					data->flags &=~ COLLECTIBLEFLAG_INACTIVE;
					CreateGlowAroundSecretItem((void*)data);
				}
			}
			else
			{
				if (~data->flags&COLLECTIBLEFLAG_INACTIVE)
				{
					data->flags |= COLLECTIBLEFLAG_INACTIVE;
					data->flags |= COLLECTIBLEFLAG_DONTDRAW;
				}
			}

			if (data->flags & COLLECTIBLEFLAG_INACTIVE) continue;
		}

		if( clearCollectables )
		{
			data->flags |= COLLECTIBLEFLAG_RESPAWN;
			continue;
		}

		if (data->flags&COLLECTIBLEFLAG_NEW)
		{
			// new collectible, must do height of point
			data->flags &= ~COLLECTIBLEFLAG_NEW;

			bmVectorCopy(tempVector,data->position);

			distance = GetHeightOfPoint(data->position);
			if (distance)
			{
				if (distance < METERTOUNIT(1.0f))
				{
					data->position[Y] -= distance;
					data->position[Y] += METERTOUNIT(0.3f);
					if (data->actorInstance)
					{
						data->position[Y] += (bmFAbs(data->actorInstance->actor->yMin)*data->actorInstance->scale[Y]);
					}
					else
					{
						data->position[Y] += data->radius;
					}
				}
			}
			else
			{
				// no ground found so move test point up half a meter
				tempVector[Y] += METERTOUNIT(0.5f);
				distance = GetHeightOfPoint(tempVector);
				if (distance)
				{
					if (distance < METERTOUNIT(1.0f))
					{
						data->position[Y] = tempVector[Y] - distance;
						data->position[Y] += METERTOUNIT(0.3f);
						if (data->actorInstance)
						{
							data->position[Y] += (bmFAbs(data->actorInstance->actor->yMin)*data->actorInstance->scale[Y]);
						}
						else
						{
							data->position[Y] += data->radius;
						}
					}
				}
				else
				{
					bkPrintf("Couldn't set colletible height.\n");
				}
			}
		}
		if(data->flags&COLLECTIBLEFLAG_POWERUP)
		{
			if(data->flags & COLLECTIBLEFLAG_RESPAWN)
			{
				if(ReadyForCollectableRespawn(data))
				{
					data->powerUpInfo->respawnClock = 0.0f;
					data->flags &= ~COLLECTIBLEFLAG_RESPAWN;
					ground = GetHeightOfPoint(data->position,PHOP_EXCLUDEOBJECTS);
					ground = data->position[1] - ground + TWINKLE_MIN_RADIUS;
					for(int j=0;j<20;j++)
					{
						CreateCollectibleTwinkles(data, DEBRISFLAG_TWINKLEGLOW);
					}
				}
			}
			else
			{
				if(data->powerUpInfo)
				{
					if(data->type != COLLECTIBLETYPE_SECRETITEM)
					{
	//					data->powerUpInfo->glowSize = (POWERUP_GLOW_RADIUS)+(POWERUP_GLOW_RADIUS*bmCos(RAD(gameStatus.gameTime*90.0f)));
						data->powerUpInfo->glowColour[3] = 50+(int)(30*bmCos(RAD(gameStatus.gameTime*90.0f)));
						data->powerUpInfo->glowTilt -= RAD(90.0f*fTime);
						if((data->powerUpInfo->clock += fTime) > POWERUP_GLOW_BALL_TIME)
						{
							data->powerUpInfo->clock = 0.0f;
							CreatePowerUpBubbles(data);
						}
					}
				}
			}
		}

		if (data->life)
		{
			data->life -= t;
			if ((data->life) <= 0.0f)
			{
				baDeleteExtraInstance(eSystem, i);
				noofInstances--;
				continue;
			}
		}

		if (UpdateCollectible(data))
		{
			switch(data->type)
			{
			case COLLECTIBLETYPE_BURPCAN:
			case COLLECTIBLETYPE_TESTTUBE:
			case COLLECTIBLETYPE_HICCUP:
			case COLLECTIBLETYPE_BUBBLEGUM:
			case COLLECTIBLETYPE_CHILLIPEPPER:
				if(data->powerUpInfo->respawn)
				{
					data->flags |= COLLECTIBLEFLAG_RESPAWN;
				}
				else
				{
					baDeleteExtraInstance(eSystem,i);
					noofInstances--;
				}
				continue;
			case COLLECTIBLETYPE_PICKUP:
				data->flags |= COLLECTIBLEFLAG_DISABLED;
				continue;
			default:
				baDeleteExtraInstance(eSystem,i);
				noofInstances--;
				continue;
			}
		}
//		i++;
	}

	reactivatePickups = FALSE;
	clearCollectables = FALSE;
}

/* --------------------------------------------------------------------------------
   Function : CollectibleExtraUpdateCallback
   Purpose : This function is called each frame to update the collectibles
   Parameters : 
   Returns : 
   Info : 
*/

int UpdateCollectible(COLLECTIBLE *data)
{
	TBVector				tempVector, disp, nodePos;
	float					distance=0;
	float					distance2=0;
	ACTORINSTANCE			*player;
	float					ground, len, tazCollRadius, tempFloat, tempFloat1, tempFloat2;
	int						i,j;
	char					message[32];
	char					tempBuff[16];
	float					xzScale, yScale, amp, excition, offset;
	TBVector				yAxis;
	TBActorNodeInstance		*node;

	data->distanceFromCam = bmVectorDistanceApprox(data->position, gameStatus.viewPoint);

	if (data->distanceFromCam < MAX_COLLECTIBLE_RANGE)
	{
		data->flags&=~COLLECTIBLEFLAG_DONTDRAW;
	}
	else
	{
		tempFloat1 = bmVectorDistanceApprox(data->position, gameStatus.player1->actorInstance.position);

		if ((gameStatus.multiplayer.versusCPU == false)&&(gameStatus.player[1].actorInstance))
			tempFloat2 = bmVectorDistanceApprox(data->position, gameStatus.player[1].actorInstance->actorInstance.position);
		else
			tempFloat2 = tempFloat1 + 1;

		tempFloat1 < tempFloat2 ? tempFloat = tempFloat1 : tempFloat = tempFloat2;

		// TP: check if distance to taz is closes
		if (tempFloat < data->distanceFromCam)
		{
			data->distanceFromCam = tempFloat;
		}
		if (data->distanceFromCam < MAX_COLLECTIBLE_RANGE)
		{
			data->flags&=~COLLECTIBLEFLAG_DONTDRAW;
		}
		else
		{
			data->flags|=COLLECTIBLEFLAG_DONTDRAW;
		}
	}

	// TP: Check for collectible being collected
	for (i=0;i < gameStatus.multiplayer.numPlayers; i++)
	{
		if (player = gameStatus.player[i].actorInstance)
		{
			// CMD: need to do this whether drawn or not
			// CMD: but before any possiblity of being collected
			if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].pickupsCollected == NUM_PICKUPS_EACH_LEVEL &&
					data->type == COLLECTIBLETYPE_PICKUP && (~data->flags & COLLECTIBLEFLAG_DONTREMOVE))
			{
				sprintf(message,"_C %08x %08x", player, data->crc);
				bkGenerateEvent("destroycollect", message);

				data->flags |= COLLECTIBLEFLAG_DONTREMOVE;

//				ground = GetHeightOfPoint(data->position,PHOP_EXCLUDEOBJECTS);
//				ground = data->position[1] - ground + TWINKLE_MIN_RADIUS;

//				for(j = 0;j < 20;j++)
//				{
//					CreateCollectibleShower(data,ground);
//				}
				return TRUE;
			}

			if (!(data->flags&COLLECTIBLEFLAG_DONTDRAW) && !(data->flags&COLLECTIBLEFLAG_DISABLED))
			{
				// CMD: unfortunately this might get a bit ugly
				// CMD: when on minecart, need to have 'collectible collision'
				// CMD: based around one of taz's hands
				switch(player->actorStatus->currentState)
				{
				case STATE_ONMINECART:
					// CMD: seeing as we are in state_minecart, I am going to assume that the actor
					// CMD: attached to taz is indeed a minecart, seems over the top doing this check 
					// CMD: for every collectible, when it in fact should never fail, hence an
					// CMD: assert should be sufficient
					ASSERTM(player->actorStatus->actorAttachedTo, "Taz is in state_onminecart, but with no minecart attached!");

					if(player->actorStatus->actorAttachedTo->mineCartInfo->tilt > 0.0f)
					{
						// CMD: tilting right
						node = baFindNode(player->actorInstance.rootNodeInstance, "fingerr2a");
					}
					else
					{
						// CMD: tilting left
						node = baFindNode(player->actorInstance.rootNodeInstance, "fingerl2a");
					}
					// CMD: likewise only taz should be collecting the collectibles
					// CMD: and Taz should always have these bones
					ASSERTM(node, "could not find one of taz's bones, when trying to check for collectibles while in state_onminecart");
					baGetNodesWorldPosition(&player->actorInstance, node, nodePos);
					bmVectorSub(tempVector, data->position, nodePos);
					distance = bmVectorLenApprox(tempVector);
					tazCollRadius = TAZ_ON_MINECART_COLL_RADIUS;
					break;
				default:
					bmVectorAdd(tempVector, player->actorInstance.position, player->actorBehaviour->collSphere.offset);
					bmVectorSub(tempVector, data->position, tempVector);
					distance = bmVectorLenApprox(tempVector);
					tazCollRadius = player->actorBehaviour->collSphere.radius;
					break;
				}				

				if(data->type == COLLECTIBLETYPE_PICKUP)
				{
					// CMD: do some bouncy scaling stuff
					// CMD: and vary it with the distance from taz
					// CMD: scale x, z in phase, y out of phase
					if(distance)
					{
						if((excition = METERTOUNIT(2.0f)/distance) > 1.0f) excition = 1.0f;
					}
					else excition = 0.0f;
					if((data->phaseAngle += COLLECTIBLES_SCALE_RATE*fTime*excition) > 360.0f) data->phaseAngle -= 360.0f;
	
					if((amp = COLLECTIBLES_AMP_CONST/distance) > 2.0f) amp = 2.0f;
					xzScale = bmSin(RAD(data->phaseAngle));
					xzScale = 1.0f + (amp*xzScale*xzScale*0.5f);
					yScale = bmCos(RAD(data->phaseAngle));
					yScale = 1.0f + (amp*yScale*yScale);
	
					SETVECTOR(data->actorInstance->scale, xzScale, yScale, xzScale, 1.0f);
				
					// CMD: make them rotate
					if((data->rotAngle += COLLECTIBLES_ROT_SPEED*fTime) > RAD(360.0f)) data->rotAngle -= RAD(360.0f);
					SETVECTOR(yAxis, 0.0f, 1.0f, 0.0f, data->rotAngle);
					bmRotationToQuat(data->actorInstance->orientation, yAxis);
				}

				// TP: if being projected then be a little more generous with the collection range
				if ((player->actorStatus->currentState==STATE_PROJECTILE)||(player->actorStatus->currentState==STATE_PROJECTILESLIDE))
				{
					// TP: check for collection along path of projectile
					TBCollisionLine	line;
					TBVector closestPoint;

					bmVectorAdd(line.start, player->actorStatus->positionLastFrame, player->actorBehaviour->collSphere.offset);
					bmVectorAdd(line.end, player->actorInstance.position, player->actorBehaviour->collSphere.offset);
					distance = bmGeomDistanceFromLine(&line, data->position, closestPoint);
				}

				if (distance <= (tazCollRadius+data->radius))
				{
					// has collected
					switch(data->type)
					{
					case COLLECTIBLETYPE_DE_EXTRATIME:
					case COLLECTIBLETYPE_DE_DOUBLECRATE:
					case COLLECTIBLETYPE_DE_SLOMO:
						ProcessItemCollected(player, data);
						return TRUE;
						break;
					case COLLECTIBLETYPE_SECRETITEM:
						ProcessItemCollected(player, data);
						return TRUE;
						break;
					case COLLECTIBLETYPE_PICKUP:
						ProcessItemCollected(player, data);
						ground = GetHeightOfPoint(data->position,PHOP_EXCLUDEOBJECTS);
						ground = data->position[1] - ground + TWINKLE_MIN_RADIUS;
						for(j = 0;j < 20;j++)
						{
							CreateCollectibleShower(data,ground);
						}
						sprintf(tempBuff, "%d", gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].pickupsCollected);
						CreateScreenScoreMember(data->position, tempBuff, SCREENSCORE_SANDWICHES);
						return TRUE;
						break;
					case COLLECTIBLETYPE_FOOD:
					case COLLECTIBLETYPE_BIGFOOD:
					case COLLECTIBLETYPE_STRANGEFOOD:
						ProcessItemCollected(player, data);
						return TRUE;
						break;
					case COLLECTIBLETYPE_BURPCAN:
					case COLLECTIBLETYPE_TESTTUBE:
					case COLLECTIBLETYPE_HICCUP:
					case COLLECTIBLETYPE_BUBBLEGUM:
					case COLLECTIBLETYPE_CHILLIPEPPER:
						if((player->characterInfo->currentBurp==0)&&
							(player->characterInfo->powerUpInfo.invisible == 0)&&
							(player->characterInfo->powerUpInfo.hiccup == 0)&&
							(player->characterInfo->powerUpInfo.bubbleGum == 0)&&
							(player->characterInfo->powerUpInfo.chilliPepper == 0))
						{
							if(!(data->flags & COLLECTIBLEFLAG_RESPAWN))
							{
								ProcessItemCollected(player, data);
								return TRUE;
							}
							else return FALSE;
						}
						else
						{
							return FALSE;
						}
						break;
					}
				}
			}
			// CMD: need to do this even if pickups are not being updated
			if((data->type == COLLECTIBLETYPE_PICKUP)||(data->type == COLLECTIBLETYPE_STRANGEFOOD))
			{
				// CMD: update glow alpha
				if((~data->flags & COLLECTIBLEFLAG_DISABLED)&&(data->distanceFromCam < METERTOUNIT(20.0f)))
				{
					data->glowAlpha = 50.0f*bmSin(RAD(180.0f)*data->distanceFromCam/METERTOUNIT(20.0f));
				}
				else
				{
					data->glowAlpha = 0.0f;
				}
			}
		}
	}

	
	if ((data->type == COLLECTIBLETYPE_PICKUP)&&(!(data->flags&COLLECTIBLEFLAG_DONTDRAW))&&(!(data->flags&COLLECTIBLEFLAG_DISABLED)))
	{
		// TP: update the colour and scale of the generic pickup
		data->textureRot -= RAD(90.0f*fTime);
		if (data->textureRot < 0) data->textureRot += TWOPI;
		// CMD: lets create some twinkles
		if((data->twinkleClock -= fTime) < 0.0f)
		{
			bmVectorSub(disp,gameStatus.player[0].actorInstance->actorInstance.position, data->position);
			len = bmVectorLenApprox(disp);
			if(len < METERTOUNIT(5.0f))
			{
				data->twinkleClock = (float)(bmRand()%20)/100.0f;
				CreateCollectibleTwinkles(data);
			}
			else
				if(len < METERTOUNIT(10.0f))
				{
					data->twinkleClock = (float)(bmRand()%30)/100.0f;
					CreateCollectibleTwinkles(data);
				}
				else
					if(len < METERTOUNIT(15.0f))
					{
						data->twinkleClock = (float)(bmRand()%50)/100.0f;
						CreateCollectibleTwinkles(data);
					}
		}
	}

	// TP: Perform update on collectibles position and rotation etc.
	if ((data->actorInstance)&&(!(data->flags&COLLECTIBLEFLAG_DONTDRAW))&&(!(data->flags&COLLECTIBLEFLAG_DISABLED)))
	{
		// update actor instance
		if(distance <= MAX_COLLECTIBLE_RANGE)
		{
			if ((data->type != COLLECTIBLETYPE_PICKUP))
			{
				UpdateCollectibleFoodScaleAndOrientation(data, RAD(120.0f));
			}
		}

		bmVectorCopy(data->actorInstance->position,data->position);
		baUpdateNodes(data->actorInstance, ROOTNODE(data->actorInstance->rootNodeInstance), speedRatio, NULL);
		baPrepActorInstance(data->actorInstance,TRUE);
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : CollectibleExtraRenderCallback
   Purpose : This function is called each frame to render the collectibles
   Parameters : 
   Returns : 
   Info : 
*/
void CollectibleExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context)
{
	COLLECTIBLE				*data;
	int						i;
	float					min=METERTOUNIT(10.0f),max=METERTOUNIT(13.0f),dist;
	float					radius,falpha;
	static int						alpha;
	TBVector				position;
	TBMatrix				twoPlayerScale;
	TBActorNodeInstance		*node;
	int32					drawStage=(int32)context;			// PP: the current draw stage for the extra
	float					opacity;
	// PP: REMOUT: OOD		if((int32)context != EXTRA_ALPHA) return;

	// PP: set 3D projection mode
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);

	// PP: set less-than z-test - moved up here from further down the function
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, 0);
// PP: 	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, 1);

	for (i=0;i<noofInstances;)
	{
		data = (COLLECTIBLE*)dataPtrs[i];

		if (guiDebugFood)
		{
			if(drawStage == EXTRA_ALPHA)// PP: the lines'll get drawn later that way
			{
				if ((data->type == COLLECTIBLETYPE_PICKUP)&&(~data->flags&COLLECTIBLEFLAG_DISABLED))
				{
					// TP: draw line
					if (gameStatus.player[0].actorInstance)
					{
						utilDrawLine(gameStatus.player[0].actorInstance->actorInstance.position, data->position, COLOUR(0,255,0));
					}
				}
			}
		}

		// PP: DRAW GLOW...

		if(drawStage == EXTRA_SOLID)// PP: draw glow before solid pass; solid pass is the first of the the two
		{
			if((data->glowAlpha)&&(~data->flags&COLLECTIBLEFLAG_INACTIVE))	// TP: the inactive flag allows the secret item to be hidden (construction site)
			{
				// PP: zbias glow towards viewer to reduce interception problems with ground - not too far though or it'll shine through cupboards etc.
#if(BPLATFORM == PS2)
				bdSetZBias(-50000.0f);// PP: ?
#else// PP: !ps2
				bdSetZBias(-1.5f);
#endif// PP: !ps2

				bdSetIdentityObjectMatrix();
				bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
				bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);

				data->position[3] = 1.0f;
				switch(data->type)
				{
				case COLLECTIBLETYPE_STRANGEFOOD:
					bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
					bdDrawFlatSprite(data->position, METERTOUNIT(0.5f), METERTOUNIT(0.5f), 0.0, textureSandwichGlow ,0, 0, 119, 0, 0, bmFloatToInt(data->glowAlpha)<<1, NULL, COLLECTIBLE_GLOW_ZBIAS);
					break;
				default:		// CMD: sandwiches
					bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_ADDITIVE, FALSE);

					bdDrawFlatSprite(data->position, METERTOUNIT(0.5f), METERTOUNIT(0.5f), 0.0, textureSandwichGlow ,0, 0, 110, 95, 85, bmFloatToInt(data->glowAlpha)<<1, NULL, COLLECTIBLE_GLOW_ZBIAS);
					break;
				}

				bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);

				// PP: reset z-bias
				bdSetZBias(0.0f);
			}
		}

		if (!(data->flags&COLLECTIBLEFLAG_DONTDRAW)&&!(data->flags&COLLECTIBLEFLAG_DISABLED))
		{
			dist = data->distanceFromCam = bmVectorDistanceApprox(data->position, gameStatus.viewPoint);

			if (data->actorInstance)
			{
				if (dist > max) 
				{
					// PP: too far away, don't draw
					
					i++;
					continue;
				}

				if (dist > min)
				{
					opacity=1.0f-((dist-min)/(max-min));

					if(opacity > 1.0f)
					{
						opacity=1.0f;
					}

					alpha=bmFloatToInt(opacity*256.0f);// PP: TEMP TEST

					if (alpha!=256)
					{
	//						lineParms.a = 0;
						bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
						bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
						bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
						baSetGlobalAlpha(alpha);
					}
				}
				else
				{
					// PP: reet close

	//					lineParms.a = 255;
					opacity=1.0f;
					alpha=256;
					baSetGlobalAlpha(alpha);
					bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
				}

				//always draw power up collectibles
				if(data->flags&COLLECTIBLEFLAG_POWERUP)
				{
					if(data->flags & COLLECTIBLEFLAG_RESPAWN)
					{
						i++;
						continue;
					}

					if(drawStage == EXTRA_SOLID)// PP: draw swirl before solid pass; solid pass is the first of the the two
					{
						//draw power up sprite
						if(textureSwirl)
						{
							bdSetIdentityObjectMatrix();
							bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
							bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
							bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);

							node = baFindNode(data->actorInstance->rootNodeInstance,"root");
							if(node)
							{
								if(data->powerUpInfo)
								{
									baGetNodesWorldPosition(data->actorInstance,node,position);	
									position[3] = 1.0f;

									// PP: set an object matrix to correct the appearance of the sprites
									setSpriteObjMat();

									bdDrawFlatSprite(position,data->powerUpInfo->glowSize,data->powerUpInfo->glowSize,data->powerUpInfo->glowTilt,textureSwirl,0,0,
										data->powerUpInfo->glowColour[0],data->powerUpInfo->glowColour[1],data->powerUpInfo->glowColour[2],bmFloatToInt((data->powerUpInfo->glowColour[3]<<1)*opacity),NULL,0);

								}
							}

							bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
							// PP: REMOUT: MOVING TO TOP OF FUNC	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
							bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
						}
					}

					//draw power up actor
					bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
					bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
					bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
					
					if (data->type == COLLECTIBLETYPE_DE_SLOMO)
					{
						baSetActorRenderMode(data->actorInstance, 0, BACTRENDERMODE_NORMAL, &cartoonParms);	//BACTRENDERMODE_CARTOONLIT, &cartoonParms);
						baSetActorRenderMode(data->actorInstance, 1, BACTRENDERMODE_NONE, &lineParms);	//BACTRENDERMODE_OUTLINED, &lineParms);
					}
					else
					{
						//baSetActorRenderMode(data->actorInstance, 0, BACTRENDERMODE_CARTOONLIT, &cartoonParms);
						//baSetActorRenderMode(data->actorInstance, 1, BACTRENDERMODE_OUTLINED, &lineParms);
						EnableActorCartoonRenderMode(data->actorInstance);
					}

					bdSetIdentityObjectMatrix();

					if(drawStage == EXTRA_SOLID)// PP: EXTRA_SOLID is the first draw stage to be performed; only prep before fist stage
					{
// PP: REMOUT: don't think we need this						baPrepActorInstance(data->actorInstance, TRUE);

						// PP: draw colour key stuff along with solid stuff
						baDrawActorInstance(data->actorInstance,BACTPRIMALPHATYPE_SOLID|BACTPRIMALPHATYPE_CKEY);
					}
					else// PP: EXTRA_ALPHA
					{
						// PP: draw alpha parts
						baDrawActorInstance(data->actorInstance,BACTPRIMALPHATYPE_ALPHA);

						// PP: draw shadow
						if (data->shadow)
						{
							DrawShadow(data->shadow);
						}
					}

					i++;
	//					bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	//					bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	//					bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
					continue;
				}// PP: end if powerup

				// PP: must z-write to draw an outlined actor
				bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);

				bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
				bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
				bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

				// PP: TEMP TEST!!1!
				//util3DText(data->position, "%d", bmFloatToInt(data->distanceFromCam));

				if(drawStage == EXTRA_SOLID)// PP: EXTRA_SOLID is the first draw stage to be performed; only prep before fist stage
				{
// PP: REMOUT: wasn't previously being prepped - dun't it need to be?					baPrepActorInstance(data->actorInstance, TRUE);

					// PP: draw colour key stuff along with solid stuff
					baDrawActorInstance(data->actorInstance,BACTPRIMALPHATYPE_SOLID|BACTPRIMALPHATYPE_CKEY);
				}
				else// PP: EXTRA_ALPHA
				{
					// PP: draw alpha parts
					baDrawActorInstance(data->actorInstance,BACTPRIMALPHATYPE_ALPHA);

					// PP: draw shadow
					if (data->shadow)
					{
						DrawShadow(data->shadow);
					}
				}

				// PP: REMOUT: OOD	baDrawActorInstance(data->actorInstance,BSETPRIMALPHATYPE_SOLID|BSETPRIMALPHATYPE_CKEY|BSETPRIMALPHATYPE_ALPHA);
			}
			else// PP: if(!data->actorInstance)
			{
				// PP: NO ACTORINSTANCE, JUST A SPRITE... (OOD?)...

				// PP: only draw these in alpha stage
				if(drawStage == EXTRA_ALPHA)
				{
					if (dist > max) 
					{
						i++;
						continue;
					}

					if (dist > min)
					{
						falpha = (max-dist) / (max-min);
					}
					else
					{
						falpha=1.0f;
					}

					// TP: render sprite
					bdSetIdentityObjectMatrix();
					bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
					bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
					bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
					bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
					
					data->position[W] = 1.0f;

					// PP: Collect the Qwilsons
					if(CheatActive(CHEAT_QWILSON))
					{
						bdDrawFlatSprite(data->position,
										(data->radius*2.0f),
										(data->radius*1.5f),
										0.0f,
										QwilsonTexture,
										0,
										0,
										128,100,90,128,// PP: FRIG: flesh tone.  I tried (saturation=1-goth) but it came out too white(?)
										NULL,
										0);
					}
					else
					{
						radius = data->radius*1.5f*bmFAbs(bmSin(RAD(90.0f*(data->pulse = gameStatus.gameTime))));
						bdDrawFlatSprite(data->position,radius,radius,0.0f,texturePowerUpGlow,0,0,data->colour.r,data->colour.g,data->colour.b,bmFloatToInt(140.0f*falpha)<<1,NULL,0.0f);
						bdDrawFlatSprite(data->position,data->radius,data->radius,data->textureRot,data->texture,0,0,data->colour.r,data->colour.g,data->colour.b,bmFloatToInt(data->colour.a*falpha)<<1,NULL,0);
					}

					bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
					bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
				}
			}
		}

		i++;
	}

//	lineParms.a = 255;
	if (alpha!=256)
	{
		baSetGlobalAlpha(256);
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	}

	// PP: TEMP TEST!!!!!!
	lineParms.thickness=1.8f;
}

/* --------------------------------------------------------------------------------
   Function : CollectibleExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/
void CollectibleExtraSystemInitCallback(TBExtraSystem *eSystem, int reason)
{
}

static void ChewGumSoundCallback(void *context)
{
	PlaySample("gumblowballoon.wav",255);
}


/*  --------------------------------------------------------------------------------
	Function	: myStateChangeBubble
	Purpose		: Callback used when changing into the bubblegum state
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/

void myStateChangeBubble(ACTORINSTANCE *actorInstance)
{
	TBVector		offset;

	SETVECTOR(offset, 0.0f, METERTOUNIT(0.9f), 0.0f, 1.0f);
	SetActorCollSphere(actorInstance->actorBehaviour, offset, METERTOUNIT(0.9f));
	actorInstance->characterInfo->powerUpInfo.bubbleGum = TRUE;
	SETVECTOR(actorInstance->characterInfo->powerUpInfo.tempVector, 0.0f, 0.0f, 0.0f, 1.0f);
	actorInstance->characterInfo->powerUpInfo.gumReady = FALSE;
	actorInstance->characterInfo->powerUpInfo.clock = 0.0f;
	actorInstance->characterInfo->powerUpInfo.randomClock = 0.0f;
	actorInstance->characterInfo->powerUpInfo.thirdClock = 0.0f;
	actorInstance->characterInfo->powerUpInfo.state = BUBBLEGUM_RISING;
	AttachGumBubbleToActor(actorInstance);
	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance,"blowgumbubble",1.0f,0,0,0.1f,MOVE);
	CallbackWhenSampleFinished(PlaySample("gumchew.wav",255),ChewGumSoundCallback,NULL);
//	actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_NOGRAVITY;
}

void EatBadFoodCallback(ACTORINSTANCE *actorInstance)
{
	FlushAnimationQueue(actorInstance); 
	PlayAnimationByName(actorInstance, "runeat2", 1.0f, 0, 0, 0.1f, REACT);
	if (UtilGenerateRandomNumber(1.0f,0.0f)>0.5f)
	{
		PlayAnimationByName(actorInstance, "tntinside", 1.0f, 0, 1, 0.1f, NONE);
		if(actorInstance->playerNo != -1)
		{
			PlaySample("muffledexplode.wav", 255);
			RequestRumbleEffect(gameStatus.player[actorInstance->playerNo].controller, 175, 0.0f, 0.5f, 1.0f, 0.0f, WAVEFORM_NORMAL);
		}
	}
	else
	{
		PlayAnimationByName(actorInstance, "badfood", 1.0f, 0, 1, 0.1f, NONE);
		if(actorInstance->playerNo != -1)
		{
			RequestRumbleEffect(gameStatus.player[actorInstance->playerNo].controller, 175, 0.5f, 0.5f, 1.5f, 0.0f, WAVEFORM_SQUAREDSINUSOIDAL, 2.0f);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : ProcessItemCollected
   Purpose : This function is called when an item is collected
   Parameters : actor collecting the item, item type
   Returns : 
   Info : 
*/
void ProcessItemCollected(ACTORINSTANCE *actorInstance, COLLECTIBLE *pCollectible)
{
	TBActorNodeInstance	*node;
	char message[32];
	CHARACTERINFO *charInfo = actorInstance->characterInfo;
	int type = pCollectible->type;

	sprintf(message,"_C %08x %08x", actorInstance, pCollectible->crc);
	bkGenerateEvent("destroycollect", message);

	switch(type)
	{
	case COLLECTIBLETYPE_BIGFOOD:
	/*	switch(bmRand()%3)
		{
		case 0:
			PlaySample("taz_gen_07a.wav",255);
			break;
		case 1:
			PlaySample("taz_gen_07b.wav",255);
			break;
		case 2:
			PlaySample("taz_gen_07c.wav",255);
			break;
		}
		if (charInfo)
		{
			IncreaseCharactersSubHealth(charInfo,5);
		}
		break;*/
	case COLLECTIBLETYPE_FOOD:
		if ((actorInstance->actorStatus->currentState != STATE_SPIN)
			&&(actorInstance->actorStatus->currentState != STATE_MOVESQUASHED)
			&&(actorInstance->actorStatus->currentState != STATE_BALL)
			&&(actorInstance->actorStatus->currentState != STATE_MOUSE))
		{
			node = baFindNode(actorInstance->actorInstance.rootNodeInstance,"spinelower");
			PlayAnimationByNamePart(actorInstance,"runeat1",0.5f,0,0,0.1f,REACT,node);
			PlayAnimationByNamePart(actorInstance,"runeat1",0.5f,0,1,0.1f,REACT,node);
			PlayAnimationByNamePart(actorInstance,"runeat1",0.5f,0,1,0.1f,REACT,node);
			baQueueNodeAnimBlendToNode(&actorInstance->actorInstance,node,actorInstance->actorInstance.rootNodeInstance,0.1f);
		}

		// JW: Yummy food sounds

		if(characterSounds.taz == -1)
		{
			if (!(bmRand()%3))
			{
			switch(bmRand()%3)
				{
				case 0:
					CallbackWhenSampleFinished((characterSounds.taz=PlaySample("tazeat_2 alt.wav",255,actorInstance->actorInstance.position)),CharacterSoundsFinishedCallback,&characterSounds.taz);
					break;
				case 1:
					CallbackWhenSampleFinished((characterSounds.taz=PlaySample("tazeat_3 alt.wav",255,actorInstance->actorInstance.position)),CharacterSoundsFinishedCallback,&characterSounds.taz);
					break;
				case 2:
					CallbackWhenSampleFinished((characterSounds.taz=PlaySample("tazeat_5.wav",255,actorInstance->actorInstance.position)),CharacterSoundsFinishedCallback,&characterSounds.taz);
					break;
	/*			case 3:
					CallbackWhenSampleFinished((characterSounds.taz=PlaySample("tazeat_4.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.taz);
					break;
				case 4:
					CallbackWhenSampleFinished((characterSounds.taz=PlaySample("tazeat_5.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.taz);
					break;
				case 5:
					CallbackWhenSampleFinished((characterSounds.taz=PlaySample("tazeat_6.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.taz);
					break;
				case 6:
					CallbackWhenSampleFinished((characterSounds.taz=PlaySample("tazeat_7.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.taz);
					break;
				case 7:
					CallbackWhenSampleFinished((characterSounds.taz=PlaySample("tazeat_8.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.taz);
					break;*/
				}
			}
		}
		if (charInfo)
		{
			IncreaseCharactersHealth(charInfo,0.5f);
		}
		//add food to stats
		gameStatus.player[actorInstance->playerNo].stats->numFoodCollected += 1;

		if (charInfo->eating) 
		{
			charInfo->eating->AddItem(pCollectible->actorInstance);
			pCollectible->actorInstance = NULL;
		}
		break;
	case COLLECTIBLETYPE_STRANGEFOOD:

		KillAllPowerUps(actorInstance);
		RequestStateChangePlus(actorInstance,STATE_BADFOOD,EatBadFoodCallback);

		if(characterSounds.taz == -1)
		{
			switch(bmRand()%3)
			{
			case 0:
				CallbackWhenSampleFinished((characterSounds.taz=PlaySample("pickupbadfood1.wav",255,actorInstance->actorInstance.position)),CharacterSoundsFinishedCallback,&characterSounds.taz);
				break;
			case 1:
				CallbackWhenSampleFinished((characterSounds.taz=PlaySample("pickupbadfood2.wav",255,actorInstance->actorInstance.position)),CharacterSoundsFinishedCallback,&characterSounds.taz);
				break;
			case 2:
				CallbackWhenSampleFinished((characterSounds.taz=PlaySample("pickupbadfood3.wav",255,actorInstance->actorInstance.position)),CharacterSoundsFinishedCallback,&characterSounds.taz);
				break;
			}
		}
		break;
	case COLLECTIBLETYPE_BURPCAN:
		if((actorInstance->actorStatus->currentState != STATE_SPIN)&&
			(actorInstance->actorStatus->currentState != STATE_BALL))
		{
			node = baFindNode(actorInstance->actorInstance.rootNodeInstance,"spinelower");
			PlayAnimationByNamePart(actorInstance,"runeat2",1.0f,0,0,0.1f,REACT,node);
			baQueueNodeAnimBlendToNode(&actorInstance->actorInstance,node,actorInstance->actorInstance.rootNodeInstance,0.1f);
		}
		PlaySample("burpcanpickup.wav",255);
		if (charInfo)
		{
			IncreaseCharactersBurp(charInfo,1);
		}
		switch(gameStatus.globalSettings.difficulty)
		{
		case DIFF_EASY:
			charInfo->superBurpEffectInfo.clock = SUPERBURP_EASY;
			break;
		case DIFF_MEDIUM:
			charInfo->superBurpEffectInfo.clock = SUPERBURP_MEDIUM;
			break;
		case DIFF_HARD:
			charInfo->superBurpEffectInfo.clock = SUPERBURP_HARD;
			break;
		}
		gameStatus.player[actorInstance->playerNo].stats->numBurpCansCollected += 1;
		break;
	case COLLECTIBLETYPE_TESTTUBE:
		if((actorInstance->actorStatus->currentState != STATE_SPIN)&&
			(actorInstance->actorStatus->currentState != STATE_BALL))
		{
			node = baFindNode(actorInstance->actorInstance.rootNodeInstance,"spinelower");
			PlayAnimationByNamePart(actorInstance,"runeat2",1.0f,0,0,0.1f,REACT,node);
			baQueueNodeAnimBlendToNode(&actorInstance->actorInstance,node,actorInstance->actorInstance.rootNodeInstance,0.1f);
		}
		PlaySample("pickup.wav",255);
		PlaySample("invisible.wav",255);
		if(charInfo)
		{
			actorInstance->characterInfo->powerUpInfo.invisible= TRUE;
			actorInstance->characterInfo->powerUpInfo.clock = 0.0f;
			actorInstance->characterInfo->powerUpInfo.randomClock = 0.0f;
			actorInstance->characterInfo->powerUpInfo.thirdClock = INVISIBLE_INITIAL_TIME_DIFF;
			EnableActorLineOnlyRenderMode(&actorInstance->actorInstance, &invisLineParms);
			// NH: baSetActorRenderMode(&actorInstance->actorInstance, 0, BACTRENDERMODE_ZONLY, NULL);
			// NH: baSetActorRenderMode(&actorInstance->actorInstance, 1, BACTRENDERMODE_OUTLINED, &invisLineParms);
			actorInstance->characterInfo->powerUpInfo.state = RENDERSTATE_LINEONLY;
			gameStatus.player[actorInstance->playerNo].stats->numBurpCansCollected += 1;
		}
		break;
	case COLLECTIBLETYPE_HICCUP:
		if((actorInstance->actorStatus->currentState != STATE_SPIN)&&
			(actorInstance->actorStatus->currentState != STATE_BALL))
		{
			node = baFindNode(actorInstance->actorInstance.rootNodeInstance,"spinelower");
			PlayAnimationByNamePart(actorInstance,"runeat2",1.0f,0,0,0.1f,REACT,node);
			baQueueNodeAnimBlendToNode(&actorInstance->actorInstance,node,actorInstance->actorInstance.rootNodeInstance,0.1f);
		}
		PlaySample("pickup.wav",255);
		if(charInfo)
		{
			actorInstance->characterInfo->powerUpInfo.hiccup = TRUE;
			gameStatus.player[actorInstance->playerNo].stats->numBurpCansCollected++;
		}
		break;

	case COLLECTIBLETYPE_BUBBLEGUM:
		if((actorInstance->actorStatus->currentState != STATE_SPIN)&&
			(actorInstance->actorStatus->currentState != STATE_BALL))
		{
			node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "spinelower");
			PlayAnimationByNamePart(actorInstance,"runeat2",1.0f,0,0,0.1f,REACT,node);
			baQueueNodeAnimBlendToNode(&actorInstance->actorInstance,node,actorInstance->actorInstance.rootNodeInstance,0.1f);
		}
		PlaySample("gumpickup.wav",255);
		if(charInfo)
		{
			RequestStateChangePlus(actorInstance, STATE_BUBBLEGUM, myStateChangeBubble);
			gameStatus.player[actorInstance->playerNo].stats->numBurpCansCollected++;
		}
		break;
	
	case COLLECTIBLETYPE_CHILLIPEPPER:
		if((actorInstance->actorStatus->currentState != STATE_SPIN)&&
			(actorInstance->actorStatus->currentState != STATE_BALL))
		{
			node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "spinelower");
			PlayAnimationByNamePart(actorInstance,"runeat2",1.0f,0,0,0.1f,REACT,node);
			baQueueNodeAnimBlendToNode(&actorInstance->actorInstance,node,actorInstance->actorInstance.rootNodeInstance,0.1f);
		}
		PlaySample("pickup.wav",255);
		if(charInfo)
		{
			switch(gameStatus.globalSettings.difficulty)
			{
			case DIFF_EASY:
				charInfo->powerUpInfo.clock = CHILLIPEPPER_EASY;
				break;
			case DIFF_MEDIUM:
				charInfo->powerUpInfo.clock = CHILLIPEPPER_MEDIUM;
				break;
			case DIFF_HARD:
				charInfo->powerUpInfo.clock = CHILLIPEPPER_HARD;
				break;
			}
			charInfo->powerUpInfo.state = POWERUPSTATE_CHILLIOFF;
			charInfo->powerUpInfo.chilliPepper = TRUE;
			charInfo->powerUpInfo.numFire = CHILLIPEPPER_FIRE_NUM;
			RequestStateChange(actorInstance, STATE_CHILLIPEPPER);
			gameStatus.player[actorInstance->playerNo].stats->numBurpCansCollected++;
		}
		break;
#ifndef CONSUMERDEMO
	case COLLECTIBLETYPE_DE_EXTRATIME:
	case COLLECTIBLETYPE_DE_DOUBLECRATE:
	case COLLECTIBLETYPE_DE_SLOMO:
		PlaySample("pickup.wav",255);

		if ((actorInstance->actorStatus->currentState != STATE_SPIN)&&(actorInstance->actorStatus->currentState != STATE_MOVESQUASHED))
		{
			node = baFindNode(actorInstance->actorInstance.rootNodeInstance,"spinelower");
			PlayAnimationByNamePart(actorInstance,"runeat1",0.5f,0,0,0.1f,MOVE,node);
			baQueueNodeAnimBlendToNode(&actorInstance->actorInstance,node,actorInstance->actorInstance.rootNodeInstance,0.1f);
		}
	
		doDestructionGamePickup(actorInstance, type);

		break;
#endif
	case COLLECTIBLETYPE_PICKUP:
		if((actorInstance->actorStatus->currentState != STATE_SPIN)&&
			(actorInstance->actorStatus->currentState != STATE_MOVESQUASHED)&&
			(actorInstance->actorStatus->currentState != STATE_BALL))
		{
			node = baFindNode(actorInstance->actorInstance.rootNodeInstance,"spinelower");
			PlayAnimationByNamePart(actorInstance,"runeat1",0.5f,0,0,0.1f,REACT,node);
			baQueueNodeAnimBlendToNode(&actorInstance->actorInstance,node,actorInstance->actorInstance.rootNodeInstance,0.1f);
		}

		if(characterSounds.taz == -1)
		{
			if (!(bmRand()%3))
			{
				switch(bmRand()%3)
				{
				case 0:
					CallbackWhenSampleFinished((characterSounds.taz=PlaySample("tazeat_2 alt.wav",255,actorInstance->actorInstance.position)),CharacterSoundsFinishedCallback,&characterSounds.taz);
					break;
				case 1:
					CallbackWhenSampleFinished((characterSounds.taz=PlaySample("tazeat_3 alt.wav",255,actorInstance->actorInstance.position)),CharacterSoundsFinishedCallback,&characterSounds.taz);
					break;
				case 2:
					CallbackWhenSampleFinished((characterSounds.taz=PlaySample("tazeat_5.wav",255,actorInstance->actorInstance.position)),CharacterSoundsFinishedCallback,&characterSounds.taz);
					break;
				/*case 3:
					CallbackWhenSampleFinished((characterSounds.taz=PlaySample("tazeat_4.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.taz);
					break;
				case 4:
					CallbackWhenSampleFinished((characterSounds.taz=PlaySample("tazeat_5.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.taz);
					break;
				case 5:
					CallbackWhenSampleFinished((characterSounds.taz=PlaySample("tazeat_6.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.taz);
					break;
				case 6:
					CallbackWhenSampleFinished((characterSounds.taz=PlaySample("tazeat_7.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.taz);
					break;
				case 7:
					CallbackWhenSampleFinished((characterSounds.taz=PlaySample("tazeat_8.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.taz);
					break;*/
				}
			}
		}

		IncrementTazTopCounter(actorInstance, map.sceneNumber);
		PlaySample("pickup.wav",255);

		// TP: the number of collectibles is now stored immediately
		gameStatus.globalSettings.saveInfo.stats[ConvertSceneRefToLevelScoreRef((EScene)(map.sceneNumber))].collectibles = gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].pickupsCollected;

		if (gameStatus.globalSettings.currentSaveSlot!=-1)
		{
			// TP: store the ident of the swirl collected
			if (pCollectible->ident>=120)
			{
				bkPrintf("*** WARNING *** More that 120 collectible pickups have been found.\n");
			}
			else
			{
				gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].swirls[pCollectible->ident] = true;
				gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].pickupsCollected ++;

#ifndef CONSUMERDEMO
				if (map.sceneNumber < SCENE_TAZHUB)
				{
					if (!gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].bonusgameOption)
					{
						if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].pickupsCollected == NUM_PICKUPS_EACH_LEVEL)
						{
							Subtitle(STR_HUNDRED_COLLECTIBLES,5.0f, SUBTITLE_ALWAYS);

							PlaySample("fanfare.wav", 255);
						}
					}
				}
#endif
			}
		}
		break;

	case COLLECTIBLETYPE_SECRETITEM:
		if((actorInstance->actorStatus->currentState != STATE_SPIN)&&
			(actorInstance->actorStatus->currentState != STATE_MOVESQUASHED)&&
			(actorInstance->actorStatus->currentState != STATE_BALL))
		{
			node = baFindNode(actorInstance->actorInstance.rootNodeInstance,"spinelower");
			PlayAnimationByNamePart(actorInstance,"runeat1",0.5f,0,0,0.1f,REACT,node);
			baQueueNodeAnimBlendToNode(&actorInstance->actorInstance,node,actorInstance->actorInstance.rootNodeInstance,0.1f);
		}
		if (!gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].secretItemCollected)
		{
			startThwap(STR_SECRET_ITEM_FOUND,posterBounties[map.sceneNumber] * 2);

			if (gameStatus.globalSettings.currentSaveSlot!=-1)
			{
				gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].secretItemCollected = true;
			}
		}
		break;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateCollectibleFoodScaleAndOrientation
	Purpose 	: see above
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateCollectibleFoodScaleAndOrientation(COLLECTIBLE *food, float angularVelocity)
{
	TBVector		yAxis = {0.0f, 1.0f, 0.0f, 0.0f};
	TBQuaternion	tempQuat, orientation;

	//rotate food, whoo hoo: a dumb waiter sim
	yAxis[3] = angularVelocity/fps;
	bmRotationToQuat(tempQuat, yAxis);
	bmQuatMultiply(orientation, food->actorInstance->orientation, tempQuat);
	bmQuatCopy(food->actorInstance->orientation, orientation);
}

/*  --------------------------------------------------------------------------------
	Function	: ShutdownBarriers
	Purpose		: Used when the player has destroyed enough pickups
	Parameters	: ACTORINSTANCE
	Returns		: 
	Info		: Starts the 20 second countdown before taz cams re-activate
*/
void ShutdownTazCameras(ACTORINSTANCE *actorInstance)
{
}

/* --------------------------------------------------------------------------------
   Function : InitBarrierSystem
   Purpose : Call this function to init the counters and siren sound for the taz cam shutdown thingys
   Parameters : none
   Returns : 
   Info : 
*/
void InitBarrierSystem(void)
{
}

/* --------------------------------------------------------------------------------
   Function: ShutdownBarrierSystem
   Purpose : This function clears anything left behind by the taz cam shutdown stuff
   Parameters : none
   Returns : 
   Info : 
*/
void ShutdownBarrierSystem(void)
{
}

/* --------------------------------------------------------------------------------
   Function: UpdateBarrierSystem
   Purpose : Controls the barriers, update the timers and the screen tint
   Parameters : none
   Returns : 
   Info : 
*/
int UpdateBarrierSystem(void)
{
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function: CreatePowerUpBubbles
   Purpose : Creates bubbles which eminate from powerups
   Parameters : debris structure
   Returns : 
   Info : 
*/

void CreatePowerUpBubbles(COLLECTIBLE *data)
{
	TBVector		velocity,position,tempVelocity;
	TBVector		rotation = {0.0f, 0.0f, 0.0f, 1.0f};
	float			angle, life, size, alpha, zero;;


	if(texturePowerUpGlow)
	{
		angle = RAD((float)(bmRand()%360));
		//set velocity to be a general radial drift
		velocity[0] = POWERUP_GLOW_SPEED * bmCos(angle);
		velocity[1] = 0.0f;
		velocity[2] = POWERUP_GLOW_SPEED * bmSin(angle);

		//find start position
		bmVectorCopy(position, data->position);
		position[1] += METERTOUNIT(0.25f);

		//alter sprite position a little
		bmVectorCDiv(tempVelocity, velocity, 10.0f);
		bmVectorAdd(position, tempVelocity, position);

		//add y componant of velocity
		velocity[1] = POWERUP_GLOW_BALL_VERT;

		life = POWERUP_GLOW_LIFE;
		size = POWERUP_GLOW_BALL_SIZE;
		zero = 0.0f;
		alpha = (float)data->powerUpInfo->glowColour[3];
//		baCreateExtra(bubbleSystem,NULL,texturePowerUpGlow,NULL,position,velocity,rotation,POWERUP_GLOW_LIFE,0.0f,POWERUP_GLOW_BALL_SIZE,POWERUP_GLOW_BALL_SIZE, data->powerUpInfo->glowColour[3],DEBRISFLAG_BUBBLES|DEBRISFLAG_POWERUPGLOW,0,0,0.0f,data->powerUpInfo->glowColour[0],data->powerUpInfo->glowColour[1],data->powerUpInfo->glowColour[2]);
		baCreateExtra(bubbleSystem,position,velocity,texturePowerUpGlow,&life,&size,&zero,
						data->powerUpInfo->glowColour[0],data->powerUpInfo->glowColour[1],data->powerUpInfo->glowColour[2],
						&alpha,DEBRISFLAG_BUBBLES|DEBRISFLAG_POWERUPGLOW, &zero);
	}
}

/* --------------------------------------------------------------------------------
   Function: CreateCollectibleTwinkles
   Purpose : Creates twinkles around collectibles
   Parameters : debris structure
   Returns : 
   Info : 
*/

void CreateCollectibleTwinkles(COLLECTIBLE *data, uint32 flags)
{
	TBVector		position;
	TBTexture		*texture;
	TBVector		velocity = {0.0f, 0.0f, 0.0f, 1.0f};
	TBVector		rotation = {0.0f, 0.0f, 0.0f, 1.0f};
	int				yellow[3] = {125,122,0};
	int				cyan[3] = {48,125,125};
	int				purple[3] = {118,110,125};
	int				green[3] = {141,123,142};
	int				colour[3];
	float			size, life, zero;

	if(texturePowerUpGlow)
	{
		//find start position
/*
		position[0] = (float)(bmRand()%100)-50.0f;
		position[1] = (float)(bmRand()%100)-50.0f;
		position[2] = (float)(bmRand()%100)-50.0f;
*/
		utilGenerateRandomNonZeroVector(position, 50.0f, TRUE);
		bmVectorScaleToLength(position,position,TWINKLE_RAD_FROM_POINT);

		bmVectorScaleToLength(velocity, position, UtilGenerateRandomNumber(METERTOUNIT(1.0f), METERTOUNIT(0.5f)));
		bmVectorAdd(position, position, data->position);

		size = TWINKLE_MIN_RADIUS + (float)(bmRand()%((int)TWINKLE_MIN_RADIUS));

		switch(bmRand()%2)
		{
		case 0:
			texture = textureTomato;
//			colour[0] = yellow[0]; colour[1] = yellow[1], colour[2] = yellow[2];
			break;
		case 1:
			texture = textureCucumber;
//			colour[0] = cyan[0]; colour[1] = cyan[1], colour[2] = cyan[2];
			break;
		case 2:
//			colour[0] = purple[0]; colour[1] = purple[1], colour[2] = purple[2];
			break;
		case 3:
//			colour[0] = green[0]; colour[1] = green[1], colour[2] = green[2];
			break;
		}
		colour[0] = colour[1] = colour[2] = 127;
		life = TWINKLE_HALF_LIFE*2.0f;
		zero = 0.0f;
		if(flags&DEBRISFLAG_TWINKLEGLOW)
		{
			switch(bmRand()%3)
			{
			case 0:
				colour[0] = yellow[0]; colour[1] = yellow[1]; colour[2] = yellow[2];
				break;
			case 1:
				colour[0] = cyan[0]; colour[1] = cyan[1]; colour[2] = cyan[2];
				break;
			case 2:
				colour[0] = purple[0]; colour[1] = purple[1]; colour[2] = purple[2];
				break;
			case 3:
				colour[0] = green[0]; colour[1] = green[1]; colour[2] = green[2];
				break;
			}
			baCreateExtra(bubbleSystem,position,velocity,texture,&life,&size,&zero,colour[0],colour[1],colour[2],&zero,flags, &zero);
		}
		else
		{
			baCreateExtra(bubbleSystem,position,velocity,texture,&life,&size,&zero,colour[0],colour[1],colour[2],&zero,DEBRISFLAG_TWINKLE, &zero);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function: UpdateTwinkles
   Purpose : Creates twinkles around collectibles
   Parameters : debris structure
   Returns : 
   Info : 
*/

int32 UpdateTwinkles(BUBBLESEXTRA *data)
{
	TBVector		temp;

	// CMD: update position
	bmVectorCMul(temp, data->velocity, fTime);
	bmVectorAdd(data->position, data->position, temp);

	// CMD: update scale
	data->xScale = bmCos(RAD(180.0f*(data->scaleClock += fTime)));
	data->yScale = bmSin(RAD(180.0f*data->scaleClock));

	if(data->life > TWINKLE_HALF_LIFE)
	{
		// CMD: fadein
		data->colour.a += 128.0f/TWINKLE_HALF_LIFE*fTime;
	}
	else
	{
		// CMD: fade out
		if((data->colour.a -= 128.0f/TWINKLE_HALF_LIFE*fTime) < 0.0f)
		{
			data->colour.a = 0.0f;
			return TRUE;
		}
	}

	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function: CreateCollectibleShower
   Purpose : Creates shower for when collectibles are collected
   Parameters : debris structure
   Returns : 
   Info : 
*/

void CreateCollectibleShower(COLLECTIBLE *data, float groundCoord)
{
	TBVector		velocity = {0.0f, 0.0f, 0.0f, 1.0f};
	TBVector		rotation = {0.0f, 0.0f, 0.0f, 1.0f};
	int				yellow[3] = {125,122,0};
	int				cyan[3] = {48,125,125};
	int				purple[3] = {118,110,125};
	int				green[3] = {141,123,142};
	int				colour[3];
	float			size, life, alpha, other;

//	if(texturePowerUpGlow)
//	{
		//find start position
		velocity[0] = UtilGenerateRandomNumber(50.0f, -50.0f);
		velocity[1] = METERTOUNIT(0.25f);
		velocity[2] = UtilGenerateRandomNumber(50.0f, -50.0f);
		bmVectorScaleToLength(velocity,velocity,UtilGenerateRandomNumber(4.0f*SHOWER_MIN_VEL, SHOWER_MIN_VEL));

/*		switch(bmRand()%4)
		{
		case 0:
			colour[0] = yellow[0]; colour[1] = yellow[1], colour[2] = yellow[2];
			break;
		case 1:
			colour[0] = cyan[0]; colour[1] = cyan[1], colour[2] = cyan[2];
			break;
		case 2:
			colour[0] = purple[0]; colour[1] = purple[1], colour[2] = purple[2];
			break;
		case 3:
			colour[0] = green[0]; colour[1] = green[1], colour[2] = green[2];
			break;
		}*/
		colour[0] = colour[1] = colour[2] = 127;

		life = TWINKLE_HALF_LIFE*4.0f;
		other = UtilGenerateRandomNumber(RAD(360.0f), 0.0f);
		alpha = SHOWER_INIT_ALPHA;
		switch(bmRand()%8)
		{
		case 0:
			size = UtilGenerateRandomNumber(2.0f*TWINKLE_MIN_RADIUS, TWINKLE_MIN_RADIUS);
			baCreateExtra(bubbleSystem,data->position,velocity,textureTomato,&life,&size,&other,colour[0],colour[1],colour[2],&alpha,DEBRISFLAG_FOODSHOWER,&groundCoord);
			break;
		case 1:
			size = UtilGenerateRandomNumber(2.0f*TWINKLE_MIN_RADIUS, TWINKLE_MIN_RADIUS);
			baCreateExtra(bubbleSystem,data->position,velocity,textureCucumber,&life,&size,&other,colour[0],colour[1],colour[2],&alpha,DEBRISFLAG_FOODSHOWER,&groundCoord);
			break;
		default:
			size = UtilGenerateRandomNumber(0.25f*TWINKLE_MIN_RADIUS, TWINKLE_MIN_RADIUS);
			baCreateExtra(bubbleSystem,data->position,velocity,textureCrumb,&life,&size,&other,colour[0],colour[1],colour[2],&alpha,DEBRISFLAG_FOODSHOWER,&groundCoord);
			break;
		}
//	}
}

void CreateCollectibleShowerAtPosition(TBVector position, float groundCoord)
{
	TBVector		velocity = {0.0f, 0.0f, 0.0f, 1.0f};
	TBVector		rotation = {0.0f, 0.0f, 0.0f, 1.0f};
	int				yellow[3] = {125,122,0};
	int				cyan[3] = {48,125,125};
	int				purple[3] = {118,110,125};
	int				green[3] = {141,123,142};
	int				colour[3];
	float			size, life, alpha, zero;;

	if(texturePowerUpGlow)
	{
		//find start position
		velocity[0] = UtilGenerateRandomNumber(50.0f, -50.0f);
		velocity[1] = METERTOUNIT(0.25f);
		velocity[2] = UtilGenerateRandomNumber(50.0f, -50.0f);
		bmVectorScaleToLength(velocity,velocity,UtilGenerateRandomNumber(2.0f*SHOWER_MIN_VEL, SHOWER_MIN_VEL));

		size = UtilGenerateRandomNumber(2.0f*TWINKLE_MIN_RADIUS, TWINKLE_MIN_RADIUS);

		switch(bmRand()%4)
		{
		case 0:
			colour[0] = yellow[0]; colour[1] = yellow[1], colour[2] = yellow[2];
			break;
		case 1:
			colour[0] = cyan[0]; colour[1] = cyan[1], colour[2] = cyan[2];
			break;
		case 2:
			colour[0] = purple[0]; colour[1] = purple[1], colour[2] = purple[2];
			break;
		case 3:
			colour[0] = green[0]; colour[1] = green[1], colour[2] = green[2];
			break;
		}
		life = TWINKLE_HALF_LIFE*4.0f;
		zero = 0.0f;
		alpha = SHOWER_INIT_ALPHA;
		baCreateExtra(bubbleSystem,position,velocity,texturePowerUpGlow,&life,&size,&zero,colour[0],colour[1],colour[2],&alpha,DEBRISFLAG_SHOWER,&groundCoord);
	}
}

/* --------------------------------------------------------------------------------
   Function: UpdateCollectiblesShower
   Purpose : updates shower produced when collectibles are collected
   Parameters : debris structure
   Returns : 
   Info : 
*/

int32 UpdateCollectiblesShower(BUBBLESEXTRA *data)
{
	TBVector	temp;

	//update position
	bmVectorCMul(temp,data->velocity,fTime);
	bmVectorAdd(data->position, data->position, temp);

	if(data->life < 0.5f)
	{
		if((data->colour.a -= 2.0f*SHOWER_INIT_ALPHA*fTime) < 0.0f)
		{
			data->colour.a = 0.0f;
			return TRUE;
		}
	}
	//check for ground collision
	if(data->position[1] < data->ground)
	{
		data->position[1] = data->ground;
		data->velocity[1] *= -0.8f;
	}
	//sudo-gravity
	if(data->flags&DEBRISFLAG_FOODSHOWER)
	{
		data->velocity[1] -= METERTOUNIT(6.0f)*fTime;
	}
	else
	{
		data->velocity[1] -= METERTOUNIT(3.0f)*fTime;
	}
	return FALSE;
}


/* --------------------------------------------------------------------------------
   Function: ClearCollectibles
   Purpose : Clears all collectables to respawn
   Parameters : 
   Returns : 
   Info : 
*/
void ClearCollectibles(void)
{
	clearCollectables = TRUE;
}

/* --------------------------------------------------------------------------------
   Function : InitScreenScoreList
   Purpose : guess what it does!
   Parameters : 
   Returns : 
   Info : 
*/

void InitScreenScoreList(void)
{
	screenScoreList.numEntries = 0;
	screenScoreList.head.next = screenScoreList.head.prev = &screenScoreList.head;
}

/* --------------------------------------------------------------------------------
   Function : CreateScreenScoreMember
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void CreateScreenScoreMember(TBVector position, char *string, EScreenScoreType type)
{
	SCREENSCOREMEMBER		*newEntry;
	SCREENSCOREMEMBER		*tempPtr;

	tempPtr = screenScoreList.head.next;

	// CMD: create new structure for incoming information
	if(newEntry = (SCREENSCOREMEMBER*)ZALLOC(sizeof(SCREENSCOREMEMBER)))
	{
		// CMD: get screen coords from world coords
		// CMD: its not that i'm paranoid, i just want to make sure
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
		bdSetIdentityObjectMatrix();
		bdProjectVertices(newEntry->position, position, 1);
		newEntry->position[2] = 0.0f;

		strcpy(newEntry->string, string);

		switch(newEntry->type = type)
		{
		case SCREENSCORE_REWARD:
			// CMD: want to reach target in SCREENSCORE_TIME seconds
			bmVectorSet(newEntry->velocity, 0.0f, 400.0f, 0.0f, 0.0f);
//			bmVectorSub(newEntry->velocity, newEntry->targetPos, newEntry->position);
			newEntry->velocity[2] = 0.0f;

			newEntry->r = 0;
			newEntry->g = 255;
			newEntry->b = 0;
			newEntry->a = 127<<8;
			newEntry->alphaRate = (float)((int)(80.0f/SCREENSCORE_TIME)<<8);

			newEntry->angle = 0.0f;
			newEntry->scale = 1.0f;

			newEntry->callback = &ScreenScoreRotateToTargetCallback;
			break;
		case SCREENSCORE_PENALTY:
			// CMD: want to reach target in SCREENSCORE_TIME seconds
			bmVectorSet(newEntry->velocity, 0.0f, 200.0f, 0.0f, 0.0f);
//			bmVectorSub(newEntry->velocity, newEntry->targetPos, newEntry->position);
			newEntry->velocity[2] = 0.0f;

			newEntry->r = 255;
			newEntry->g = 0;
			newEntry->b = 0;
			newEntry->a = 127<<8;
			newEntry->alphaRate = (float)((int)(80.0f/SCREENSCORE_TIME)<<8);

			newEntry->angle = 0.0f;
			newEntry->scale = 1.0f;

			newEntry->callback = &ScreenScoreRotateToTargetCallback;
			break;
		case SCREENSCORE_SANDWICHES:
			// CMD: want to reach target in SCREENSCORE_TIME seconds
			bmVectorSet(newEntry->velocity, 0.0f, 400.0f, 0.0f, 0.0f);
//			bmVectorSub(newEntry->velocity, newEntry->targetPos, newEntry->position);
			newEntry->velocity[2] = 0.0f;

			newEntry->r = 127;
			newEntry->g = 50;
			newEntry->b = 50;
			newEntry->a = 127<<8;
			newEntry->alphaRate = (float)((int)(80.0f/SCREENSCORE_TIME)<<8);

			newEntry->angle = 0.0f;
			newEntry->scale = 1.0f;

			newEntry->callback = &ScreenScoreRotateToTargetCallback;
			break;
		}

		newEntry->next = tempPtr;
		newEntry->prev = tempPtr->prev;
		tempPtr->prev->next = newEntry;
		tempPtr->prev = newEntry;
		screenScoreList.numEntries++;
	}
}

/* --------------------------------------------------------------------------------
   Function : RemoveScreenScoreMember
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void RemoveScreenScoreMember(SCREENSCOREMEMBER *remPtr)
{
	SCREENSCOREMEMBER  *ptr, *temp;

	for(ptr = screenScoreList.head.next;ptr != &screenScoreList.head;)
	{
		if(ptr == remPtr)
		{
			ptr->prev->next = ptr->next;
			ptr->next->prev = ptr->prev;
			screenScoreList.numEntries--;
		
			temp = ptr->next;
			SAFE_FREE(ptr);
			ptr = temp;
		}
		else
		{
			ptr = ptr->next;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateScreenScoreList
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/
void UpdateScreenScoreList(void)
{
	SCREENSCOREMEMBER	*info, *temp;
	ushort				string16Buf[64];
	TBVector			posInc;

	for(info = screenScoreList.head.prev;info != &screenScoreList.head;)
	{
		temp = info->prev;

		// CMD: calculate offset such that string is centred on its desired position
		bkString8to16(string16Buf, info->string);
		bdFontExtent(standardFont, string16Buf, strlen(info->string), &info->offset[0], &info->offset[1], NULL, NULL);
		info->offset[0] *= 0.5f;
		info->offset[1] *= 0.5f;

		// CMD: update velocity
		bmVectorCMul(posInc, info->velocity, fTime);
		bmVectorAdd(info->position, info->position, posInc);

		switch(info->type)
		{
		case SCREENSCORE_REWARD:
		case SCREENSCORE_PENALTY:
		case SCREENSCORE_SANDWICHES:
			// CMD: update scale
			info->scale += 3.0f*fTime;
			// CMD: update angle
			if((info->angle += RAD(720.0f)*fTime) > 360.0f) info->angle -= RAD(360.0f);
			// CMD: update alpha
			if((info->a -= (int)(info->alphaRate*fTime)) < 0)
			{
				RemoveScreenScoreMember(info);
				break;
			}
			break;
		}

		info = temp;
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawScreenScoreList
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/
void DrawScreenScoreList(void)
{
	SCREENSCOREMEMBER	*info;
	ushort			string16Buf[64];
	TBMatrix		scale, there, back, tempMat;
	TBVector		tempVector;
	float			xPlusOne, xMinusOne, yPlusOne, yMinusOne;

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_ADDITIVE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_ADDITIVE, 1);

	for(info = screenScoreList.head.prev;info != &screenScoreList.head;info = info->prev)
	{
//		bdSetIdentityObjectMatrix();
		bmVectorCopy(tempVector, info->position);
		tempVector[0] -= (info->scale*info->offset[0]);
		tempVector[1] -= (info->scale*info->offset[1]);
		bmMatTranslate(there, tempVector[0], tempVector[1], tempVector[2]);
		bmMatScale(scale, info->scale, info->scale, info->scale);
		bmMatTranslate(back, -tempVector[0], -tempVector[1], -tempVector[2]);

		bmMatMultiply(tempMat, scale, back);
		bmMatMultiply(back, there, tempMat);

		bdSetObjectMatrix(back);

		bkString8to16(string16Buf, info->string);

		info->flags |= SCREENSCORE_OUTLINE;

//		xPlusOne = info->position[0] + (1.0f/info->scale);
//		xMinusOne = info->position[0] - (1.0f/info->scale);
//		yPlusOne = info->position[1] + (1.0f/info->scale);
//		yMinusOne = info->position[1] - (1.0f/info->scale);

//		bdPrintFontLite(standardFont, string16Buf, strlen(info->string), xPlusOne, yPlusOne, 
//							info->r ,info->g ,info->b, info->a>>8, info->callback, info);

//		bdPrintFontLite(standardFont, string16Buf, strlen(info->string), xPlusOne, yMinusOne, 
//							info->r ,info->g ,info->b, info->a>>8, info->callback, info);

//		bdPrintFontLite(standardFont, string16Buf, strlen(info->string), xMinusOne, yPlusOne, 
//							info->r ,info->g ,info->b, info->a>>8, info->callback, info);
		
//		bdPrintFontLite(standardFont, string16Buf, strlen(info->string), xMinusOne, yMinusOne, 
//							info->r ,info->g ,info->b, info->a>>8, info->callback, info);

		info->flags &= ~SCREENSCORE_OUTLINE;
		bdPrintFontLite(standardFont, string16Buf, strlen(info->string), tempVector[0], tempVector[1], 
									info->r ,info->g ,info->b, info->a>>8, info->callback, info);
	}

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
}

/* --------------------------------------------------------------------------------
   Function : FreeScreenScoreList
   Purpose : Removes all actors from the actor list and free the memory allocated completely
   Parameters : 
   Returns : 
   Info : 
*/

void FreeScreenScoreList(void)
{
	SCREENSCOREMEMBER		  *ptr,*temp;

	for(ptr = screenScoreList.head.next;ptr != &screenScoreList.head;)
	{
		ptr->prev->next = ptr->next;
		ptr->next->prev = ptr->prev;
		screenScoreList.numEntries--;

		temp = ptr->next;
		SAFE_FREE(ptr);
		ptr = temp;
	}
}

/* --------------------------------------------------------------------------------
   Function : ScreenScoreRotateToTargetCallback
   Purpose : spins the font towards a target coordinate
   Parameters : 
   Returns : 
   Info : 
*/

int ScreenScoreRotateToTargetCallback(EBFontCallbackReason reason, TBFontCallbackInfo *info)
{
	SCREENSCOREMEMBER		*data;
	float					r, g, b, a;
	TBVector				centre, diff;
	TBVector				verts[4];
	int						i;

	switch(reason)
	{
	case EBFONTCALLBACKREASON_POLY:
		data = (SCREENSCOREMEMBER*)info->context;

		if(data->flags & SCREENSCORE_OUTLINE)
		{
			for(i = 3;i >= 0;--i)
			{
				BDVERTEX_SETRGBA(&info->curVertex[i], 15.0f, 15.0f, 15.0f, 127.0f);
			}
		}
		else
		{
			for(i = 3;i >= 0;--i)
			{
				switch(i)
				{
				case 0:
					BDVERTEX_SETRGBA(&info->curVertex[i], 127.0f, 96.0f, 15.0f, data->a>>8);
					break;
				case 1:
					BDVERTEX_SETRGBA(&info->curVertex[i], 127.0f, 118.0f, 15.0f, data->a>>8);
					break;
				case 2:
					BDVERTEX_SETRGBA(&info->curVertex[i], 127.0f, 96.0f, 15.0f, data->a>>8);
					break;
				case 3:
					BDVERTEX_SETRGBA(&info->curVertex[i], 127.0f, 118.0f, 15.0f, data->a>>8);
					break;
				}
			}
		}
		break;
	}

	// PP: TEMP: Must return a value; might as well return a rude one
	return 0x81680085;// PP: teehee!
}
