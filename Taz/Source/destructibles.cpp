// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : destructibles.cpp
//   Purpose : functions to handle destructible objects
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"

#include "trigger.h"
#include "status.h"
#include "main.h"
#include "scene.h"
#include "physics.h"
#include "destruct.h"
#include "maths.h"
#include "collisioncache.h"
#include "scene.h" 
#include "precipitation.h"
#include "debris.h"
#include "animation.h"
#include "destructibles.h"
#include "breakables.h"
#include "playerstats.h"
#include "sfx.h"
#include "Boss.h"							// PP: General boss game stuff
#include "characters.h"
#include "eating.h"
#include "textures.h"
#include "sparks.h"
#include "fireball.h"
#include "tutorial.h"
#include "lava.h"
#include "subtitle.h"
#include "thwap.h"
#include "paths.h"
#include "gui.h"

/*  --------------------------------------------------------------------------------
	Function	: SetupDestructible
	Purpose		: Create a destructible object from the map attribs
	Parameters	: currentmap, node, instanceAttribs, instance
	Returns		: 
	Info		: 
*/
void SetupDestructible(MAPINFO *currentMap, CPMNode *node, CPMNode *rootNode, MAPOBJECTATTRIBS *instanceAttribs, ACTORINSTANCE *tempInstance)
{
	char	*tempString;
	DESTRUCTIBLEOBJECTINFO	*destInfo;
	int		myint[3],bounceInt,i;
	float	bounce;
	bool	value;

	if (instanceAttribs->collision)
	{
		if (instanceAttribs->platformCRC)
		{
			AddActorToCollisionCache(currentMap->collisionCache,tempInstance,CACHEFLAGS_LIVEUPDATE);
		}
		else
		{
			AddActorToCollisionCache(currentMap->collisionCache,tempInstance,instanceAttribs->liveUpdate);
		}
	}

	if (instanceAttribs->model == TRUE)
	{
		tempString = instanceAttribs->modelName;
	}
	else
	{
		tempString = NULL;
	}

	if (!node->GetAttrib("bounce",bounce))
	{
		if (!node->GetAttrib("bounce",bounceInt))
		{
			if (!rootNode->GetAttrib("bounce",bounce))
			{
				if (!rootNode->GetAttrib("bounce",bounceInt))
				{
					bounce = METERTOUNIT(10.0f);
				}
				else
				{
					bounce = (float)METERTOUNIT(bounceInt);
				}
			}
		}
		else
		{
			bounce = (float)METERTOUNIT(bounceInt);
		}
	}

	if (instanceAttribs->destructible == DESTRUCTIBLETYPE_SPIKEY)
	{
		instanceAttribs->flags |= DESTRUCTIBLEFLAG_INDESTRUCTIBLE;
	}

	if (instanceAttribs->money==-1)
	{
		if (!CreateDestructibleObjectInstance(tempInstance,instanceAttribs->destructible,NULL, instanceAttribs->hits,instanceAttribs->idle,
			instanceAttribs->firstHit,instanceAttribs->secondHit,instanceAttribs->destroyHit,tempString,instanceAttribs->transX,instanceAttribs->transY,instanceAttribs->transZ,instanceAttribs->componentDebris,10,instanceAttribs->platformCRC,(short)instanceAttribs->flags,bounce))
		{
			bkPrintf("*** ERROR *** Could not create destructible\n");
			return;
		}
	}
	else
	{
		if (!CreateDestructibleObjectInstance(tempInstance,instanceAttribs->destructible,NULL, instanceAttribs->hits,instanceAttribs->idle,
			instanceAttribs->firstHit,instanceAttribs->secondHit,instanceAttribs->destroyHit,tempString,instanceAttribs->transX,instanceAttribs->transY,instanceAttribs->transZ,instanceAttribs->componentDebris,instanceAttribs->money,instanceAttribs->platformCRC,instanceAttribs->flags,bounce))
		{
			bkPrintf("*** ERROR *** Could not create destructible\n");
			return;
		}
	}

	destInfo = tempInstance->destructibleObjectInfo;
	if (destInfo->maxHits)
	{
		destInfo->hitTriggers = (TRIGGERINFO**)ZALLOC(sizeof(TRIGGERINFO*)*destInfo->maxHits);
	}
	else
	{
		destInfo->hitTriggers = NULL;
	}
	
	if (destInfo->hitTriggers)
	{
		char	*string,tempChar[256];
		int		j=1;
		int		i;

		for (i=destInfo->maxHits-1;i>=0;i--)
		{
			sprintf(tempChar,"ONHIT%d",j);

			destInfo->hitTriggers[i] = GetTrigger(tempChar,node);
			if (!destInfo->hitTriggers[i])
				destInfo->hitTriggers[i] = GetTrigger(tempChar,rootNode);

			sprintf(tempChar,"HITANIM%d",j++);
			string = node->GetAttrib(tempChar);
			if (!string) string = rootNode->GetAttrib(tempChar);

			if (string)
			{
				destInfo->animation[i] = bkCRC32((uchar*)string,strlen(string),0);
			}
			else
			{
				destInfo->animation[i];
			}
		}
		// TP: Maintain compatibility with the old animated destructible system
		if ((!destInfo->hitTriggers[0])&&(destInfo->maxHits>0))
		{
			destInfo->hitTriggers[0] = GetTrigger("ONDESTRUCTION",node);
			if (!destInfo->hitTriggers[0])
				destInfo->hitTriggers[0] = GetTrigger("ONDESTRUCTION",rootNode);
		}
		if ((!destInfo->hitTriggers[1])&&(destInfo->maxHits>1))
		{
			destInfo->hitTriggers[1] = GetTrigger("ONPENULTIMATE",node);
			if (!destInfo->hitTriggers[1])
				destInfo->hitTriggers[1] = GetTrigger("ONPENULTIMATE",rootNode);
		}
		if ((!destInfo->hitTriggers[2])&&(destInfo->maxHits>2))
		{
			destInfo->hitTriggers[2] = GetTrigger("ONFIRST",node);
			if (!destInfo->hitTriggers[2])
				destInfo->hitTriggers[2] = GetTrigger("ONFIRST",rootNode);
		}
	}

	// TP: Add to destructible count if suitable
	if((instanceAttribs->destructible != DESTRUCTIBLETYPE_OBJECTSPIN)&&
		(instanceAttribs->destructible != DESTRUCTIBLETYPE_ANIMATE)&&
		(instanceAttribs->destructible != DESTRUCTIBLETYPE_ANIMATECONTACT)&&
		(instanceAttribs->destructible != DESTRUCTIBLETYPE_ANIMATELOOP)&&
		(instanceAttribs->destructible != DESTRUCTIBLETYPE_SPARK)&&
		(instanceAttribs->destructible != DESTRUCTIBLETYPE_SPIKEY))
	{
		if (strcmp("secbox.obe", tempInstance->actorName))	// JW: Don't add security boxes to destructible count
		{
			// TP: this object should be counted
			destInfo->flags |= DESTRUCTIBLEFLAG_COUNT;
		}
	}

	if (node->GetAttrib("destroyafteranim", value))
	{
		if (value)
		{
			destInfo->flags |= DESTRUCTIBLEFLAG_DESTROYAFTERANIM;
		}
		else
		{
			destInfo->flags &= ~DESTRUCTIBLEFLAG_DESTROYAFTERANIM;
		}
	}
	else
	{
		if (rootNode->GetAttrib("destroyafteranim",value))
		{
			if (value)
			{
				destInfo->flags |= DESTRUCTIBLEFLAG_DESTROYAFTERANIM;
			}
			else
			{
				destInfo->flags &= ~DESTRUCTIBLEFLAG_DESTROYAFTERANIM;
			}
		}
	}

	// TP: do we allow ranged attacks
	if (node->GetAttrib("norangedattacks", value))
	{
		if (value)
		{
			destInfo->flags |= DESTRUCTIBLEFLAG_NORANGEDATTACKS;
		}
		else
		{
			destInfo->flags &= ~DESTRUCTIBLEFLAG_NORANGEDATTACKS;
		}
	}
	else
	{
		if (rootNode->GetAttrib("norangedattacks",value))
		{
			if (value)
			{
				destInfo->flags |= DESTRUCTIBLEFLAG_NORANGEDATTACKS;
			}
			else
			{
				destInfo->flags &= ~DESTRUCTIBLEFLAG_NORANGEDATTACKS;
			}
		}
	}

	if (node->GetAttrib("nosound",value))
	{
		if (value)
		{
			destInfo->flags |= DESTRUCTIBLEFLAG_NOSOUND;
		}
		else
		{
			destInfo->flags &= ~DESTRUCTIBLEFLAG_NOSOUND;
		}
	}
	else
	{
		if (rootNode->GetAttrib("nosound",value))
		{
			if (value)
			{
				destInfo->flags |= DESTRUCTIBLEFLAG_NOSOUND;
			}
			else
			{
				destInfo->flags &= ~DESTRUCTIBLEFLAG_NOSOUND;
			}
		}
	}

	if (destInfo->hitTriggers)
	{
		for (i=0;i<destInfo->maxHits;i++)
		{
			if ((destInfo->hitTriggers[i])&&(destInfo->hitTriggers[i]->flags&TRIGGERFLAG_CONTAINSWANTED))
			{
				destInfo->flags |= DESTRUCTIBLEFLAG_CONTAINSWANTED;
				destInfo->flags &= ~DESTRUCTIBLEFLAG_COUNT;
			}
		}
	}

	if (node->GetAttrib("nopoint",value))
	{
		if (value)
		{
			destInfo->flags &= ~DESTRUCTIBLEFLAG_COUNT;
		}
		else
		{
			destInfo->flags |= DESTRUCTIBLEFLAG_COUNT;
		}
	}
	else
	{
		if (rootNode->GetAttrib("nopoint",value))
		{
			if (value)
			{
				destInfo->flags &= ~DESTRUCTIBLEFLAG_COUNT;
			}
			else
			{
				destInfo->flags |= DESTRUCTIBLEFLAG_COUNT;
			}
		}
	}

	if (destInfo->flags & DESTRUCTIBLEFLAG_COUNT)
	{
		map.mapStats.numDestructibles += 1;
		gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].totalDestructiblesInMap++;
	}

	tempString = node->GetAttrib("explosion");
	if (!tempString) tempString = rootNode->GetAttrib("explosion");
	
	if (tempString)
	{

		int		i=0;

		destInfo->explosion = (EXPLOSIONINFO*)ZALLOC(sizeof(EXPLOSIONINFO));

		if (destInfo->explosion)
		{
			destInfo->explosion->expCols[0]=FBP_COLOUR_HOT;
			destInfo->explosion->expCols[1]=FBP_COLOUR_COLD;
			destInfo->explosion->expCols[2]=FBP_COLOUR_SMOKE;
			destInfo->explosion->expCols[3]=FBP_COLOUR_FINAL;

			destInfo->explosion->type=atoi(tempString);

			tempString++;

			while((*tempString) != '\0')
			{
				if ((*tempString)=='/') tempString++;
				if ((*tempString)==' ') tempString++;
				sscanf(tempString, "%d:%d:%d",&myint[0],&myint[1],&myint[2]);
				destInfo->explosion->expCols[i].r = (uchar)myint[0];
				destInfo->explosion->expCols[i].g = (uchar)myint[1];
				destInfo->explosion->expCols[i++].b = (uchar)myint[2];
				while(((*tempString)!='/')&&((*tempString)!='\0')) tempString++;
			}
		}
	}

	//get debris
	destInfo->debris = GetDebrisObjectsFromMap("debrisObject", node);
	if(!destInfo->debris)
		destInfo->debris = GetDebrisObjectsFromMap("debrisObject", rootNode);
	//get sparking textures
	if(instanceAttribs->destructible == DESTRUCTIBLETYPE_SPARK)
	{
		if(!(destInfo->sparkInfo = GetSparkTexturesFromMap("texture", node)))
		{
			if(!(destInfo->sparkInfo = GetSparkTexturesFromMap("texture", rootNode)))
			{
				destInfo->sparkInfo = (SPARKINFO*)ZALLOC(sizeof(SPARKINFO));
				ASSERTM(destInfo->sparkInfo, "could not create sparky type spark info");

				destInfo->sparkInfo->numTextures = -1;
				
				destInfo->sparkInfo->sparkTextureCRC = (uint32*)ZALLOC(sizeof(uint32));
				ASSERTM(destInfo->sparkInfo->sparkTextureCRC, "out of memory");
				*destInfo->sparkInfo->sparkTextureCRC = 0;
				
				destInfo->sparkInfo->sparkTexture = (TBTexture**)ZALLOC(sizeof(TBTexture*));
				ASSERTM(destInfo->sparkInfo->sparkTexture, "out of memory");
				destInfo->sparkInfo->sparkTexture[0] = textureSpark;
			}
		}
		destInfo->sparkInfo->sparkClock = DESTRUCTIBLETYPE_SPARKTIME;
	}
}

/* --------------------------------------------------------------------------------
   Function : CreateDestructibleObjectInstance
   Purpose : Create a destructibleObjectInstance
   Parameters : ACTORINSTANCE, type (DESTRUCTIBLETYPE_...), destructible verion or 0
   Returns : 
   Info : 
*/
int CreateDestructibleObjectInstance(ACTORINSTANCE *actorInstance, int type, DESTRUCTIBLEDEBRISINFO *debris, 
									 int hits, uint32 idle, uint32 firstHit, uint32 secondHit, uint32 destroy,char *destructibleVersion, 
									 float xOffset, float yOffset, float zOffset,int componentDebris,uint32 money,uint32 platformCRC,short flags,float bounce)
{
	char					destVersion[256];
	DESTRUCTIBLEOBJECTINFO	*destInfo;

	actorInstance->special = SPECIALTYPE_DESTRUCTIBLE;
	actorInstance->destructibleObjectInfo = (DESTRUCTIBLEOBJECTINFO*)ZALLOC(sizeof(DESTRUCTIBLEOBJECTINFO));
	if (!actorInstance->destructibleObjectInfo) 
	{
		bkPrintf("*** WARNING *** Could not create destructibleObjectInfo.\n");
		return FALSE;
	}
	destInfo = actorInstance->destructibleObjectInfo;
	SetCollisionCallback(actorInstance,RespondToDestructibleCollision);
	if ((type == DESTRUCTIBLETYPE_CONTACT)||(type == DESTRUCTIBLETYPE_SPIN))
	{
		SetPreCollisionCallback(actorInstance,PreRespondToDestructibleCollision);
	}
	if (destructibleVersion)
	{
		sprintf(destVersion,"objects\\%s",destructibleVersion);
		destInfo->destructibleVersion = bkCRC32((uchar*)destVersion,strlen(destVersion),0);
	}
	else
	{
		destInfo->destructibleVersion = 0;
	}
	destInfo->flags = flags;
	destInfo->platformCRC = platformCRC;
	destInfo->type = type;
	destInfo->hits = destInfo->maxHits = hits;
	destInfo->time = 0.0f;
	destInfo->rotation = 0.0f;
	destInfo->xOffset = bmFAbs(xOffset);
	destInfo->yOffset = bmFAbs(yOffset);
	destInfo->zOffset = bmFAbs(zOffset);
	destInfo->componentDebris = componentDebris;
	destInfo->hitTriggers = NULL;
	destInfo->sparkInfo = NULL;
	destInfo->explosion = NULL;
	destInfo->bounce = bounce;

	destInfo->debris = debris;

	destInfo->idle = idle;
	if (destInfo->idle)
	{
		PlayAnimationByCRCOfName(actorInstance, destInfo->idle, 1.0f, 1, 0, 0.1f, IDLE);
	}
	destInfo->money = money;
	if((type == DESTRUCTIBLETYPE_ANIMATE)||(type == DESTRUCTIBLETYPE_ANIMATECONTACT)||(type == DESTRUCTIBLETYPE_ANIMATELOOP))
	{
		destInfo->animation = (uint32*)ZALLOC(sizeof(uint32)*(destInfo->maxHits<4?4:destInfo->maxHits));
		if(destInfo->animation)
		{
			destInfo->animation[0] = destroy;
			destInfo->animation[1] = secondHit;
			destInfo->animation[2] = firstHit;
			if(type == DESTRUCTIBLETYPE_ANIMATELOOP)
			{
				destInfo->hits = 0;
			}
		}
	}
	else 
	{
		destInfo->animation = (uint32*)ZALLOC(sizeof(uint32)*(destInfo->maxHits<4?4:destInfo->maxHits));
		destInfo->animation[0] = destroy;
		destInfo->animation[1] = secondHit;
		destInfo->animation[2] = firstHit;
		if (actorInstance->actorInstance.numAnimSegments>1)
		{
			baQueueNodeAnimNormal(&actorInstance->actorInstance, actorInstance->actorInstance.rootNodeInstance, 
						&actorInstance->actorInstance.animSegments[1], 512+(bmRand()%200), -1, BANIMQUEUEFLAG_LOOPING, NULL,NULL);
		}
	}
	return TRUE;
}

/* -------------------------------------------------------------------------------- 
   Function : PreResponToDestructibleCollision
   Purpose : Function to be called when a collision with a destructible has occured
   Parameters : ACTORINSTANCE object hitting rock, ACTORINSTANCE *destructible
   Returns : 
   Info : 
*/
int PreRespondToDestructibleCollision(ACTORINSTANCE *collider, ACTORINSTANCE *destructible, PRECOLLINFO *info)
{
	DESTRUCTIBLEOBJECTINFO	*destInfo = destructible->destructibleObjectInfo;
	int	i;

	if (destructible->destructibleObjectInfo->flags&DESTRUCTIBLEFLAG_INDESTRUCTIBLE) return TRUE;

	if (collider->flags&ACTORFLAG_DEBRISACTOR)
	{
		if (destInfo->flags&DESTRUCTIBLEFLAG_NORANGEDATTACKS)
		{
			return TRUE;
		}

		if (destInfo->hitTriggers)
		{
			for (i=0;i<destInfo->maxHits;i++)
			{
				if ((destInfo->hitTriggers[i])&&(destInfo->hitTriggers[i]->flags&TRIGGERFLAG_CONTAINSWANTED))
				{
					return TRUE;
				}
			}
		}
	}

	if (destructible->destructibleObjectInfo->type == DESTRUCTIBLETYPE_SPIN)
	{
		if (collider->actorStatus)
		{
			if ((
				((collider->actorStatus->currentState==STATE_JUMP)&&(collider->characterInfo)&&(collider->characterInfo->powerUpInfo.hiccupJumpState))||
				(collider->actorStatus->currentState==STATE_SPIN)||
				(collider->actorStatus->currentState==STATE_SPINUP)||
				(collider->actorStatus->currentState==STATE_ATLASSPIN)||
				(collider->actorStatus->currentState==STATE_ATLASSPINUP)||
				(collider->actorStatus->currentState==STATE_VEHICLE)||
				(collider->actorStatus->currentState==STATE_NINJAKICK)||
				(collider->actorStatus->currentState==STATE_SKATECHARGE)||
				(collider->actorStatus->currentState==STATE_WEREWOLFATTACK)||
				(collider->actorStatus->currentState==STATE_INDYROLL)||
				(collider->actorStatus->currentState==STATE_SMASH))&&((bmRand()%5)<4)) return FALSE;
			else return TRUE;
		}
	}

	if((destructible->destructibleObjectInfo->type == DESTRUCTIBLETYPE_OBJECTSPIN)||
		(destructible->destructibleObjectInfo->type == DESTRUCTIBLETYPE_ANIMATE)||
		(destructible->destructibleObjectInfo->type == DESTRUCTIBLETYPE_ANIMATECONTACT)||
		(destructible->destructibleObjectInfo->type == DESTRUCTIBLETYPE_ANIMATELOOP)||
		(destructible->destructibleObjectInfo->type == DESTRUCTIBLETYPE_SPARK)||
		(destructible->destructibleObjectInfo->type == DESTRUCTIBLETYPE_SPIKEY))return TRUE;

	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : eatCallback
   Purpose : Callback for when state is changed 
   Parameters : ACTORINSTANCE object hitting rock, ACTORINSTANCE *destructible
   Returns : 
   Info : 
*/
void eatCallback(ACTORINSTANCE *actorInstance)
{
	FlushAnimationQueue(actorInstance);
	PlaySample("tazbiteeat.wav",255,actorInstance->actorInstance.position);
	PlayAnimationByName(actorInstance,"eatlikeananimal",1.0f,0,0,0.1f,NONE);
}

/* --------------------------------------------------------------------------------
   Function : ResponToDestructibleCollision
   Purpose : Function to be called when a collision with a destructible has occured
   Parameters : ACTORINSTANCE object hitting rock, ACTORINSTANCE *destructible
   Returns : TRUE if destructible is freed, FALSE if not
   Info : 
*/
int RespondToDestructibleCollision(ACTORINSTANCE *collider, ACTORINSTANCE *destructible)
{
	float			tempFloat;
	TBVector		pos;
	TBVector		direction, relPos;
	char			flag = FALSE;
	char			message[256];
	TBVector		tempPos,tempScale;
	TBQuaternion	tempQuat;
	int				i;
	TBTexture		*texture;
	DESTRUCTIBLEOBJECTINFO	*destInfo = destructible->destructibleObjectInfo;

	if (!destInfo)
	{
		bkPrintf("*** WARNING *** Tried to destroy a non destructible object.\n");
		return FALSE;
	}

	if (destInfo->flags&DESTRUCTIBLEFLAG_INDESTRUCTIBLE) 
	{
		if (destInfo->type==DESTRUCTIBLETYPE_SPIKEY)
		{
			RespondToLavaCollision(collider,destructible);
		}
		return TRUE;
	}

	if ((collider)&&(collider->flags&ACTORFLAG_DEBRISACTOR))
	{
		if (destInfo->flags&DESTRUCTIBLEFLAG_NORANGEDATTACKS)
		{
			// TP: a debris actor is usually the result of a ranged attack like a snowball or spat object
			return FALSE;
		}

		if (destInfo->hitTriggers)
		{
			for (i=0;i<destInfo->maxHits;i++)
			{
				if ((destInfo->hitTriggers[i])&&(destInfo->hitTriggers[i]->flags&TRIGGERFLAG_CONTAINSWANTED))
				{
					return FALSE;
				}
			}
		}
	}

	if(collider != NULL)// PP: allow null collider, it's kinda handy
	{
		// TP: If the collider is not taz then we should bail out.
		if (collider->playerNo==-1)
		{
			return false;
		}
	}

	if(collider == NULL)
	{
		switch(destInfo->type)
		{
		case DESTRUCTIBLETYPE_WBTNTCRATE:
			{
				flag = TRUE;
			}
			break;
		}
	}
	else
	{
		if(collider->actorStatus)
		{
			switch(destInfo->type)
			{
			case DESTRUCTIBLETYPE_SPIKEY:
			case DESTRUCTIBLETYPE_SPIN:
			case DESTRUCTIBLETYPE_SPINNER:
				if (
					((collider->actorStatus->currentState==STATE_JUMP)&&(collider->characterInfo)&&(collider->characterInfo->powerUpInfo.hiccupJumpState))||
					(collider->actorStatus->currentState==STATE_SPIN)||
					(collider->actorStatus->currentState==STATE_SPINUP)||
					(collider->actorStatus->currentState==STATE_PROJECTILESLIDE)||
					(collider->actorStatus->currentState==STATE_PROJECTILE)||
					(collider->actorStatus->currentState==STATE_ATLASSPIN)||
					(collider->actorStatus->currentState==STATE_ATLASSPINUP)||
					(collider->actorStatus->currentState==STATE_NINJAKICK)||
					(collider->actorStatus->currentState==STATE_SKATECHARGE)||
					(collider->actorStatus->currentState==STATE_WEREWOLFATTACK)||
					(collider->actorStatus->currentState==STATE_INDYROLL)||
					(collider->actorStatus->currentState==STATE_SMASH) ||
					(collider->actorStatus->currentState == STATE_BALL)) flag = TRUE;
					
				// TP: If doing bite move then change Taz into STATE_EAT
				if (collider->actorStatus->currentState==STATE_BITE)
				{
					flag = 1;
					if (collider->characterInfo->eating)
					{
						TBActorInstance	*tempInstance;

						// TP: Back up existing pos,rot,scale
						bmVectorCopy(tempPos,destructible->actorInstance.position);
						bmVectorCopy(tempScale,destructible->actorInstance.scale);
						bmQuatCopy(tempQuat,destructible->actorInstance.orientation);

						// TP: Create new instance to use for the eating item
						tempInstance = (TBActorInstance*)ZALLOC(sizeof(TBActorInstance));
						if (tempInstance==NULL)
						{
							bkPrintf("*** ERROR *** Could not allocate memory to eat destructible item.\n");
							FreeActorInstance(destructible);
						}
						else
						{
							// TP: Setup new instance
							baInitActorInstance(tempInstance, destructible->actorInstance.actor,3);
							bmVectorCopy(tempInstance->position, tempPos);
							bmVectorCopy(tempInstance->scale, tempScale);
							bmQuatCopy(tempInstance->orientation, tempQuat);
							baPrepActorInstance(tempInstance, TRUE);

							// TP: Add item to eat list
							collider->characterInfo->eating->AddItem(tempInstance);

							// TP: Save destructibleobjectinfo for spitting
							if (collider->characterInfo->inMouth==NULL)
							{
								collider->characterInfo->inMouth = destInfo;
								flag = 2;

								if (!destInfo->debris)
								{
									destInfo->debris = (DESTRUCTIBLEDEBRISINFO*)ZALLOC(sizeof(DESTRUCTIBLEDEBRISINFO));
									if (destInfo->debris)
									{
										destInfo->debris->debrisCRC[0] = destructible->actorCRC;
										destInfo->debris->numDebris = 1;
									}
								}
							}
							RequestStateChangePlus(collider, STATE_EAT, eatCallback);
						}
					}
					break;
				}

				if(collider->actorStatus->currentState==STATE_VEHICLE) flag = TRUE;
				break;
			case DESTRUCTIBLETYPE_CONTACT:
				flag = TRUE;
				break;
			case DESTRUCTIBLETYPE_RESPONDTO:
				break;
			case DESTRUCTIBLETYPE_TRIGGER:
				if((collider->actorStatus->currentState==STATE_SPIN || collider->actorStatus->currentState==STATE_SMASH 
													|| collider->actorStatus->currentState == STATE_BALL) &&(destInfo->hits))
				{
					if((gameStatus.gameTime - destInfo->time) > 0.5f)
					{
						destInfo->hits--;
					}
					destInfo->time = gameStatus.gameTime;
				}
				else
					if(collider->actorStatus->currentState==STATE_SPIN || collider->actorStatus->currentState==STATE_SMASH)	flag = TRUE;
				break;
			case DESTRUCTIBLETYPE_ANIMATE:
			case DESTRUCTIBLETYPE_ANIMATECONTACT:
				if(((collider->actorStatus->currentState==STATE_SPIN)||(collider->actorStatus->currentState==STATE_SMASH)
					||(destInfo->type==DESTRUCTIBLETYPE_ANIMATECONTACT)||(collider->actorStatus->currentState == STATE_BALL))&&(destInfo->hits))
				{
					if( (((gameStatus.gameTime - destInfo->time) > 0.5f)||(destInfo->flags&DESTRUCTIBLEFLAG_NODELAY)) &&(destInfo->hits))
					{
						destInfo->time = gameStatus.gameTime;
						if (destInfo->hits>=0) destInfo->hits--;

						if ((destInfo->hits)&&(!(destInfo->flags&DESTRUCTIBLEFLAG_NOBOUNCE)))
						{
//							BounceCharacter(collider,collider->actorBehaviour->physics.lastAverageNormal,1.0f+((destInfo->maxHits-destInfo->hits)/destInfo->maxHits));
							BounceCharacter(collider,destructible,destInfo->bounce, destInfo->hits);
						}

						bkPrintf("%s, hit %d\n",destructible->instanceName, destInfo->hits);

						switch(destInfo->hits)
						{
						case 0:
							FlushAnimationQueue(destructible);
							if (PlayAnimationByCRCOfName(destructible, destInfo->animation[destInfo->hits], 1.0f, 0, 0, 0.1f, DEATH))
							{
								if (destInfo->animation[destInfo->hits] == bkCRC32((const uchar *) "fall", 4, 0))
									PlaySample("ropebridgeplanksnap.wav", 255);

								if (!(destInfo->flags & DESTRUCTIBLEFLAG_DONTFADEOUT))
								{
									destructible->flags |= ACTORFLAG_FADEAFTERANIM;
								}
							}
							if (destInfo->flags & DESTRUCTIBLEFLAG_UPDATECOLLAFTERANIM)
							{
								destructible->flags |= ACTORFLAG_UPDATECOLLAFTERANIM;
							}
							bmVectorCopy(pos,destructible->actorInstance.position);

							if (!(destInfo->flags & DESTRUCTIBLEFLAG_DONTFADEOUT))
							{
								destructible->fadeSpeed = -64.0f;
								destructible->flags |= ACTORFLAG_DELETEAFTERFADEOUT;
							}

							// send messages after final destruction anim
							if ((destInfo->hitTriggers)&&(destInfo->maxHits>destInfo->hits)&&(destInfo->hitTriggers[destInfo->hits]))
							{
								SendTrigger(destInfo->hitTriggers[destInfo->hits], pos);
							}

							if(destInfo->debris)
							{
								// TP: if the DESTROYAFTERANIM flag is set then remove the object
								DestroyObject(destructible, destInfo->debris, destInfo->flags&DESTRUCTIBLEFLAG_DESTROYAFTERANIM);
								if (destInfo->flags&DESTRUCTIBLEFLAG_DESTROYAFTERANIM) return TRUE;
							}
							break;
						default:
							FlushAnimationQueue(destructible);
							PlayAnimationByCRCOfName(destructible, destInfo->animation[destInfo->hits], 1.0f, 0, 0, 0.1f, DEATH);
							if(destInfo->debris)
							{
								DestroyObject(destructible, destInfo->debris, FALSE);
							}

							// send messages after final destruction anim
							if ((destInfo->hitTriggers)&&(destInfo->maxHits>destInfo->hits)&&(destInfo->hitTriggers[destInfo->hits]))
							{
								bmVectorCopy(pos,destructible->actorInstance.position);
								SendTrigger(destInfo->hitTriggers[destInfo->hits], pos);
							}
							break;
						}
					}
				}
				break;
			case DESTRUCTIBLETYPE_ANIMATELOOP:
				if(collider->actorStatus->currentState == STATE_SPIN ||
				   collider->actorStatus->currentState==STATE_SMASH)
				{
					if((gameStatus.gameTime - destInfo->time) > 0.5f)
					{
						destInfo->time = gameStatus.gameTime;

						if (!(destInfo->flags&DESTRUCTIBLEFLAG_NOBOUNCE))
						{
							BounceCharacter(collider,collider->actorBehaviour->physics.lastAverageNormal,1.0f);
						}
						
						//idle=first idle; first=first hit; penultimate=2nd idle;destroy = 2nd hit
						switch(destInfo->hits)
						{
						case 0:
							FlushAnimationQueue(destructible);
							PlayAnimationByCRCOfName(destructible,destInfo->animation[2],1.0f,0,0,0.1f,REACT);
							PlayAnimationByCRCOfName(destructible,destInfo->animation[1],1.0f,1,1,0.1f,REACT);
							destInfo->hits = 1;
							if(destInfo->debris)
							{
								DestroyObject(destructible, destInfo->debris, FALSE);
							}
							// send messages after final destruction anim
							if ((destInfo->hitTriggers)&&(destInfo->maxHits>2)&&(destInfo->hitTriggers[2]))
							{
								SendTrigger(destInfo->hitTriggers[2], destructible->actorInstance.position);
							}
							break;
						case 1:
							FlushAnimationQueue(destructible);
							PlayAnimationByCRCOfName(destructible,destInfo->animation[0],1.0f,0,0,0.1f,REACT);
							PlayAnimationByCRCOfName(destructible,destInfo->idle,1.0f,1,1,0.1f,REACT);
							destInfo->hits = 0;
							if(destInfo->debris)
							{
								DestroyObject(destructible, destInfo->debris, FALSE);
							}
							// send messages after final destruction anim
							if ((destInfo->hitTriggers)&&(destInfo->maxHits>0)&&(destInfo->hitTriggers[0]))
							{
								SendTrigger(destInfo->hitTriggers[0], destructible->actorInstance.position);
							}
							break;
						}
					}
				}
				break;
			case DESTRUCTIBLETYPE_OBJECTSPIN:
				if((collider->actorStatus->currentState == STATE_SPIN || collider->actorStatus->currentState==STATE_SMASH)&&(destInfo->rotation==0.0f))
				{
					PlaySample("spinsign.wav",255,destructible->actorInstance.position);
					if(bmVectorLen(collider->actorBehaviour->physics.velocity))
					{
						bmVectorNorm(direction, collider->actorBehaviour->physics.velocity);
						bmVectorSub(relPos, collider->actorInstance.position, destructible->actorInstance.position);
						bmVectorNorm(relPos, relPos);
						tempFloat = bmVectorDot(direction, relPos);
						destInfo->rotation = DESTRUCTIBLETYPE_INITIALSPINRATE*(tempFloat/bmFAbs(tempFloat));
					}
					else
					{
						destInfo->rotation = RAD(90.0f);
					}
				}
				break;
			case DESTRUCTIBLETYPE_SPARK:
				if(collider->actorStatus->currentState == STATE_SPIN)
				{				
					if((destInfo->sparkInfo->numTextures == -1)||((destInfo->sparkInfo->sparkClock -= fTime) < 0.0f))
					{
						if (collider->actorStatus->currentState == STATE_SPIN)
						{
							collider->actorStatus->soundloopTimer = 0.0f;

							if ((collider->actorStatus->soundHandle[1] == -1)&&(~destInfo->flags&DESTRUCTIBLEFLAG_NOSOUND))
								collider->actorStatus->soundHandle[1] = PlaySample("spark.wav", 255);
						}

						destInfo->sparkInfo->sparkClock = DESTRUCTIBLETYPE_SPARKTIME;
						bmVectorAdd(pos, collider->actorInstance.position, collider->actorBehaviour->collSphere.offset);
						if(destInfo->sparkInfo->numTextures == -1)
						{
							texture = destInfo->sparkInfo->sparkTexture[0];
						}
						else
						{
							texture = destInfo->sparkInfo->sparkTexture[bmRand()%destInfo->sparkInfo->numTextures];
						}
						WereGonnaHaveSomeCrazySparks(pos, destructible->actorInstance.position,collider->actorBehaviour->collSphere.radius, texture);
					}
				}
				return FALSE;
			}
		}
	}

	if (flag)
	{
		// TP: send explosion message if need be
		if (destInfo->explosion)
		{
			switch(destInfo->explosion->type)
			{
			case 1:
				{
					Fireball_normalExplosion(destructible, destInfo->explosion->expCols[0], destInfo->explosion->expCols[1], destInfo->explosion->expCols[2], destInfo->explosion->expCols[3]);
				}
				break;

			case 2:
				{
					Fireball_flatExplosion(destructible, destInfo->explosion->expCols[0], destInfo->explosion->expCols[1], destInfo->explosion->expCols[2], destInfo->explosion->expCols[3]);
				}
				break;
			}
			
		}

		//add object to stats
		switch(destInfo->type)
		{
		case DESTRUCTIBLETYPE_SPIN:
		case DESTRUCTIBLETYPE_CONTACT:
		case DESTRUCTIBLETYPE_RESPONDTO:
		case DESTRUCTIBLETYPE_TRIGGER:
			if(collider->playerNo != -1)
			{
				switch(map.sceneNumber)
				{
					case SCENE_AQUA:
					case SCENE_CONSTRUCT:
					case SCENE_GHOSTTOWN:
					case SCENE_ICEDOME:
					case SCENE_MUSEUM:
					case SCENE_GOLDMINE:
					case SCENE_SAFARI:	
					case SCENE_DEPTSTR:
					case SCENE_GRANDCANYON:
					case SCENE_TAZHUB:

						// TP: Double check its ok to count
					if(destInfo->flags&DESTRUCTIBLEFLAG_COUNT)
					{
						gameStatus.player[collider->playerNo].stats->numObjectsDestroyed += 1;
					}

					if ((int) (gameStatus.player[collider->playerNo].stats->numObjectsDestroyed / (float) map.mapStats.numDestructibles * 100.0f) > gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].objectsDestroyed)
						gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].objectsDestroyed = (int) (gameStatus.player[collider->playerNo].stats->numObjectsDestroyed / (float) map.mapStats.numDestructibles * 100.0f);

					// JW: God, why do there have to be so many checks for things...

					if (!gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].destructBonusCollected)
					{
						if ((int) (gameStatus.player[collider->playerNo].stats->numObjectsDestroyed / (float) map.mapStats.numDestructibles * 100.0f) == 10 && !gameStatus.player[collider->playerNo].stats->destructA)
						{
							ActivateDestructOverlay(&gameStatus.player[0]);
							gameStatus.player[collider->playerNo].stats->destructA = 1;
						}
						if ((int) (gameStatus.player[collider->playerNo].stats->numObjectsDestroyed / (float) map.mapStats.numDestructibles * 100.0f) == 20 && !gameStatus.player[collider->playerNo].stats->destructB)
						{
							ActivateDestructOverlay(&gameStatus.player[0]);
							gameStatus.player[collider->playerNo].stats->destructB = 1;
						}
						if ((int) (gameStatus.player[collider->playerNo].stats->numObjectsDestroyed / (float) map.mapStats.numDestructibles * 100.0f) == 30 && !gameStatus.player[collider->playerNo].stats->destructC)
						{
							ActivateDestructOverlay(&gameStatus.player[0]);
							gameStatus.player[collider->playerNo].stats->destructC = 1;
						}
						if ((int) (gameStatus.player[collider->playerNo].stats->numObjectsDestroyed / (float) map.mapStats.numDestructibles * 100.0f) == 40 && !gameStatus.player[collider->playerNo].stats->destructD)
						{
							ActivateDestructOverlay(&gameStatus.player[0]);
							gameStatus.player[collider->playerNo].stats->destructD = 1;
						}
						if ((int) (gameStatus.player[collider->playerNo].stats->numObjectsDestroyed / (float) map.mapStats.numDestructibles * 100.0f) == 50 && !gameStatus.player[collider->playerNo].stats->destructE)
						{
							ActivateDestructOverlay(&gameStatus.player[0]);
							gameStatus.player[collider->playerNo].stats->destructE = 1;
						}
						if ((int) (gameStatus.player[collider->playerNo].stats->numObjectsDestroyed / (float) map.mapStats.numDestructibles * 100.0f) == 60 && !gameStatus.player[collider->playerNo].stats->destructF)
						{
							ActivateDestructOverlay(&gameStatus.player[0]);
							gameStatus.player[collider->playerNo].stats->destructF = 1;
						}
						if ((int) (gameStatus.player[collider->playerNo].stats->numObjectsDestroyed / (float) map.mapStats.numDestructibles * 100.0f) == 70 && !gameStatus.player[collider->playerNo].stats->destructG)
						{
							ActivateDestructOverlay(&gameStatus.player[0]);
							gameStatus.player[collider->playerNo].stats->destructG = 1;
						}
						if ((int) (gameStatus.player[collider->playerNo].stats->numObjectsDestroyed / (float) map.mapStats.numDestructibles * 100.0f) == 80 && !gameStatus.player[collider->playerNo].stats->destructH)
						{
							ActivateDestructOverlay(&gameStatus.player[0]);
							gameStatus.player[collider->playerNo].stats->destructH = 1;
						}
						if ((int) (gameStatus.player[collider->playerNo].stats->numObjectsDestroyed / (float) map.mapStats.numDestructibles * 100.0f) == 90 && !gameStatus.player[collider->playerNo].stats->destructI)
						{
							ActivateDestructOverlay(&gameStatus.player[0]);
							gameStatus.player[collider->playerNo].stats->destructI = 1;
						}
						if ((int) (gameStatus.player[collider->playerNo].stats->numObjectsDestroyed / (float) map.mapStats.numDestructibles * 100.0f) == 100 && !gameStatus.player[collider->playerNo].stats->destructJ)
						{
							ActivateDestructOverlay(&gameStatus.player[0]);
							gameStatus.player[collider->playerNo].stats->destructJ = 1;
						}
						if ((int) (gameStatus.player[collider->playerNo].stats->numObjectsDestroyed / (float) map.mapStats.numDestructibles * 100.0f) == 75 && !gameStatus.player[collider->playerNo].stats->destructK)
						{
							ActivateDestructOverlay(&gameStatus.player[0]);
							gameStatus.player[collider->playerNo].stats->destructK = 1;
						}
						if ((int) (gameStatus.player[collider->playerNo].stats->numObjectsDestroyed / (float) map.mapStats.numDestructibles * 100.0f) == 5 && !gameStatus.player[collider->playerNo].stats->destructL)
						{
							ActivateDestructOverlay(&gameStatus.player[0]);
							gameStatus.player[collider->playerNo].stats->destructL = 1;
						}
						if ((int) (gameStatus.player[collider->playerNo].stats->numObjectsDestroyed / (float) map.mapStats.numDestructibles * 100.0f) == 15 && !gameStatus.player[collider->playerNo].stats->destructM)
						{
							ActivateDestructOverlay(&gameStatus.player[0]);
							gameStatus.player[collider->playerNo].stats->destructM = 1;
						}
						if ((int) (gameStatus.player[collider->playerNo].stats->numObjectsDestroyed / (float) map.mapStats.numDestructibles * 100.0f) == 25 && !gameStatus.player[collider->playerNo].stats->destructN)
						{
							ActivateDestructOverlay(&gameStatus.player[0]);
							gameStatus.player[collider->playerNo].stats->destructN = 1;
						}
						if ((int) (gameStatus.player[collider->playerNo].stats->numObjectsDestroyed / (float) map.mapStats.numDestructibles * 100.0f) == 35 && !gameStatus.player[collider->playerNo].stats->destructO)
						{
							ActivateDestructOverlay(&gameStatus.player[0]);
							gameStatus.player[collider->playerNo].stats->destructO = 1;
						}
						if ((int) (gameStatus.player[collider->playerNo].stats->numObjectsDestroyed / (float) map.mapStats.numDestructibles * 100.0f) == 45 && !gameStatus.player[collider->playerNo].stats->destructP)
						{
							ActivateDestructOverlay(&gameStatus.player[0]);
							gameStatus.player[collider->playerNo].stats->destructP = 1;
						}
						if ((int) (gameStatus.player[collider->playerNo].stats->numObjectsDestroyed / (float) map.mapStats.numDestructibles * 100.0f) == 55 && !gameStatus.player[collider->playerNo].stats->destructQ)
						{
							ActivateDestructOverlay(&gameStatus.player[0]);
							gameStatus.player[collider->playerNo].stats->destructQ = 1;
						}
						if ((int) (gameStatus.player[collider->playerNo].stats->numObjectsDestroyed / (float) map.mapStats.numDestructibles * 100.0f) == 65 && !gameStatus.player[collider->playerNo].stats->destructR)
						{
							ActivateDestructOverlay(&gameStatus.player[0]);
							gameStatus.player[collider->playerNo].stats->destructR = 1;
						}
						if ((int) (gameStatus.player[collider->playerNo].stats->numObjectsDestroyed / (float) map.mapStats.numDestructibles * 100.0f) == 85 && !gameStatus.player[collider->playerNo].stats->destructS)
						{
							ActivateDestructOverlay(&gameStatus.player[0]);
							gameStatus.player[collider->playerNo].stats->destructS = 1;
						}
						if ((int) (gameStatus.player[collider->playerNo].stats->numObjectsDestroyed / (float) map.mapStats.numDestructibles * 100.0f) == 95 && !gameStatus.player[collider->playerNo].stats->destructT)
						{
							ActivateDestructOverlay(&gameStatus.player[0]);
							gameStatus.player[collider->playerNo].stats->destructT = 1;
						}
					}

#ifndef STANDALONE
#ifdef CONSUMERDEMO
					if (demoGameTimer > 0.0f)
#endif
#endif
					{
						if (!gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].destructBonusCollected)
						{
							int destructibles;

							destructibles = (int)(gameStatus.player[collider->playerNo].stats->numObjectsDestroyed / (float)map.mapStats.numDestructibles * 100.0f);
						
							if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].difficulty == DIFF_EASY && destructibles >= 50)
								gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].destructBonusCollected = true;
							else if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].difficulty == DIFF_MEDIUM && destructibles >= 75)
								gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].destructBonusCollected = true;
							else if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].difficulty == DIFF_HARD && destructibles >= 100)
								gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].destructBonusCollected = true;

							if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].destructBonusCollected)
							{
								if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].difficulty == DIFF_EASY && destructibles >= 50)
									startThwap(STR_DESTRUCTION_BONUS, (int) (posterBounties[map.sceneNumber] * 0.5));
								else if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].difficulty == DIFF_MEDIUM && destructibles >= 75)
									startThwap(STR_DESTRUCTION_BONUS, (int) (posterBounties[map.sceneNumber] * 0.75));
								else if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].difficulty == DIFF_HARD && destructibles >= 100)
									startThwap(STR_DESTRUCTION_BONUS, (int) (posterBounties[map.sceneNumber]));
							}
						}
					}
				}
			}
			break;
		}
		
		// PP: don't raise bounty on destruction of objects if we're in a boss game (eg. TNT crates in West Boss)  // PP: hmm
		if(!IN_BOSS_GAME)
		{
			// TP: Add money to bounty
			//AddMoneyToBounty(collider,destructible,destInfo->money);
		}

		// TP: Destroy object
		bmVectorCopy(pos,destructible->actorInstance.position);

#ifndef CONSUMERDEMO
		if ((map.sceneNumber==SCENE_DESTRUCTIONAQUA)||(map.sceneNumber==SCENE_DESTRUCTIONCONSTRUCT)||(map.sceneNumber==SCENE_DESTRUCTIONTASMANIA))
		{
			// TP: only send this message if we are in a destruction game
			//sprintf(message,"_D %08x %08x", collider, destructible->actorCRC);
			//bkGenerateEvent("destroycollect", message);

			DestructibleGameDestroy(collider, destructible);
		}
#endif

		// send messages
		SetSoundPos(pos);

		// PP: no sound messages handled yet
		bool	soundMessage=false;

		if ((destInfo->hitTriggers)&&(destInfo->hitTriggers[0]))
		{
			// PP: SendTrigger returns true if a sound message was found
			soundMessage=SendTrigger(destInfo->hitTriggers[0]);
		}

		// PP: don't play a default destruction sound if a sound message has already been handled
		if(!soundMessage)
		{
			PlaySample("crashwood 1-1.wav",255,pos);
		}

		if (destInfo->destructibleVersion)
		{
			if (!ChangeIntoDestructibleVersion(destructible, flag==2?FALSE:TRUE))
			{
				if (flag == 2)
				{
					FreeActorInstance(destructible,DONTFREE_DESTRUCTIBLEINFO);
					return TRUE;
				}
				else
				{
					FreeActorInstance(destructible);
					return TRUE;
				}
			}
			else
			{
				return TRUE;
			}
		}
		else
		{
			if (flag ==2)
			{
				FreeActorInstance(destructible,DONTFREE_DESTRUCTIBLEINFO);
				return TRUE;
			}
			else
			{
				DestroyObject(destructible, destInfo->debris, TRUE);
				return TRUE;
			}
		}
	}
	return FALSE;
}


/* --------------------------------------------------------------------------------
   Function : ChangeIntoDestructibleVersion
   Purpose : If the destructible has a special version for when its destroyed then change
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
int ChangeIntoDestructibleVersion(ACTORINSTANCE *destructible, BOOL destroyOldObject)
{
	TBActor		*tempActor;
	int			i;

	tempActor = FindActorInActorList(destructible->destructibleObjectInfo->destructibleVersion);

	if (tempActor)
	{
		ChangeActorInstance(destructible,tempActor,3,NULL);
		DisableActorCartoonRenderMode(&destructible->actorInstance);
		// choose random animation for new destructible
		RemoveActorFromCollisionCache(&collisionCache,destructible);
#if BPLATFORM==GAMECUBE
		destructible->myActor = FindActorInList(destructible->destructibleObjectInfo->destructibleVersion);
#endif

		if (destructible->actorInstance.numAnimSegments)
		{
			i = bmRand()%destructible->actorInstance.numAnimSegments;
			baQueueNodeAnimNormal(&destructible->actorInstance, destructible->actorInstance.rootNodeInstance, 
						&destructible->actorInstance.animSegments[i], 1024, -1, 0, NULL,NULL);

			// TP: null destructible info if this object was eaten
			if (!destroyOldObject)
			{
				destructible->destructibleObjectInfo = NULL;
			}
			destructible->fadeSpeed = -128.0f;
			destructible->flags |= ACTORFLAG_DELETEAFTERFADEOUT;
		}
		else
		{
			if(destructible->destructibleObjectInfo->type != DESTRUCTIBLETYPE_WBTNTCRATE)
			{
				AddActorToCollisionCache(&collisionCache,destructible,0);
			}

			if (destroyOldObject)
			{
				// PP: call function to 'destroy' the destructible
				DestroyObject(destructible, destructible->destructibleObjectInfo->debris, FALSE);
			}

			// CMD: remove specialtype attributes from actorInstance
			if(destructible->destructibleObjectInfo->type != DESTRUCTIBLETYPE_WBTNTCRATE)
			{
				if(destructible->destructibleObjectInfo)
				{
					if (destroyOldObject)
					{
						FreeDestructibleInfo(destructible->destructibleObjectInfo);
						SAFE_FREE(destructible->destructibleObjectInfo);
					}
					destructible->destructibleObjectInfo = NULL;
					destructible->RespondToCollisionRoutine = NULL;
					destructible->PreRespondToCollisionRoutine = NULL;
					destructible->special = ESpecialType(-1);
				}
			}
		}
		return TRUE;
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : CheckForFurtherDestructions
   Purpose : Look for destructible object touching destroyed object
   Parameters : ACTORINSTANCE, ACTORINSTANCELIST
   Returns : 
   Info : 
*/
void CheckForFurtherDestructions(ACTORINSTANCE *actorInstance,ACTORINSTANCE *destructible, ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE	*ptr,*temp;
	float			dist;
	TBVector		tempVector;
	ACTORINSTANCE	*tempInstance;
	TBActor			*tempActor;
	int				i;
	TBVector		pos;
	char			flag = FALSE;
	char			message[256];

	// TP: spin through level instances
	ptr = actorInstanceList->head.next;

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;)
	{
		temp = ptr->next;
		if (ptr == destructible) 
		{
			ptr = temp;
			continue;
		}

		if (ptr->special == SPECIALTYPE_DESTRUCTIBLE)
		{
			if (ptr->destructibleObjectInfo->type == DESTRUCTIBLETYPE_SPIN)
			{
				// TP: Check the 2D radius is in range
				bmVectorSub(tempVector,destructible->actorInstance.position,ptr->actorInstance.position);
				dist = bmVectorLen(tempVector);

				if (dist < (destructible->actorInstance.actor->maxRadius + ptr->actorInstance.actor->maxRadius))
				{
					// TP: Destroy object
					bmVectorCopy(pos,ptr->actorInstance.position);

					// send messages
					sprintf(message,"_D %08x %08x", actorInstance, destructible);
					bkGenerateEvent("destroycollect", message);

					if ((ptr->destructibleObjectInfo->hitTriggers)&&(ptr->destructibleObjectInfo->maxHits>0)&&(ptr->destructibleObjectInfo->hitTriggers[0]))
					{
						SendTrigger(ptr->destructibleObjectInfo->hitTriggers[0], pos);
					}

					if (ptr->destructibleObjectInfo->destructibleVersion)
					{
						tempActor = FindActorInActorList(ptr->destructibleObjectInfo->destructibleVersion);
						if (tempActor)
						{
							tempInstance = CreateActorInstanceFromActorPtr(&map.animatedInstances,tempActor,ptr->destructibleObjectInfo->destructibleVersion,"destruct");
							if (tempInstance)
							{
								bmVectorCopy(tempInstance->actorInstance.position,ptr->actorInstance.position);
								bmVectorCopy(tempInstance->actorInstance.scale,ptr->actorInstance.scale);
								bmQuatCopy(tempInstance->actorInstance.orientation,ptr->actorInstance.orientation);
							}
						}
						FreeActorInstance(ptr);

						// choose random animation for new ptr
						if (tempInstance->actorInstance.numAnimSegments)
						{
							i = bmRand()%tempInstance->actorInstance.numAnimSegments;
							baQueueNodeAnimNormal(&tempInstance->actorInstance, tempInstance->actorInstance.rootNodeInstance, 
										&tempInstance->actorInstance.animSegments[i], 1024, -1, 0, NULL,NULL);
						}
						tempInstance->fadeSpeed = -128.0f;
						tempInstance->flags |= ACTORFLAG_DELETEAFTERFADEOUT;
					}
					else
					{
						DestroyObject(ptr, ptr->destructibleObjectInfo->debris, TRUE);
					}
				}
			}
		}
		ptr = temp;
	}
}

/* --------------------------------------------------------------------------------
   Function : CopyDestructibleDebrisInfo
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

DESTRUCTIBLEDEBRISINFO *CopyDestructibleDebrisInfo(DESTRUCTIBLEDEBRISINFO *debris)
{
	DESTRUCTIBLEDEBRISINFO	*copy;
	int						counter;

	if(debris == NULL) return NULL;

	copy = (DESTRUCTIBLEDEBRISINFO *)ZALLOC(sizeof(DESTRUCTIBLEDEBRISINFO));
	ASSERT(copy);

	for(counter = 0; counter < MAX_BREAKABLE_DEBRIS; counter++)
	{
		copy->debrisCRC[counter] = debris->debrisCRC[counter];
	}
	copy->numDebris = debris->numDebris;

	return copy;
}

/* --------------------------------------------------------------------------------
   Function : UpdateSpinningDestructibleObjects
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateSpinningDestructibleObjects(ACTORINSTANCE *ptr)
{
	TBQuaternion	rotInc, tempQuat;
	TBMatrix		tempMatrix;
	TBVector		rotAxis;
	ACTORINSTANCE	*actorInstance;

	if(ptr->destructibleObjectInfo)
	{
		if (ptr->destructibleObjectInfo->platformCRC)
		{
			// TP: update object
			actorInstance = FindActorInstanceInInstanceList(NULL,ptr->destructibleObjectInfo->platformCRC,&map.levelInstances);
			if (actorInstance)
			{
				// TP: copy platform information
				bmVectorCopy(ptr->actorInstance.position,actorInstance->pathInfo->position);
				bmVectorCopy(ptr->actorInstance.scale,actorInstance->pathInfo->scale);
				bmQuatCopy(ptr->actorInstance.orientation,actorInstance->pathInfo->orientation);
			}
		}

		if((ptr->destructibleObjectInfo->type == DESTRUCTIBLETYPE_OBJECTSPIN)&&(ptr->destructibleObjectInfo->rotation))
		{
			// update the spin
			SETVECTOR(rotAxis, 0.0f, 1.0f, 0.0f, (ptr->destructibleObjectInfo->rotation*fTime));
			bmRotationToQuat(rotInc, rotAxis);
			bmQuatMultiply(tempQuat, rotInc, ptr->actorInstance.orientation);

			bmQuatToMatrix(tempMatrix, tempQuat);
			bmMatrixToQuat(ptr->actorInstance.orientation, tempMatrix);
			// decay the spin
			if(ptr->destructibleObjectInfo->rotation > 0.0f)
			{
				if((ptr->destructibleObjectInfo->rotation -= DESTRUCTIBLETYPE_SPINDOWNRATE*fTime) <0.0f)
				{
					ptr->destructibleObjectInfo->rotation = 0.0f;
				}
			}
			else
			{
				if((ptr->destructibleObjectInfo->rotation += DESTRUCTIBLETYPE_SPINDOWNRATE*fTime) >0.0f)
				{
					ptr->destructibleObjectInfo->rotation = 0.0f;
				}
			}
		}

		if (ptr->destructibleObjectInfo->type == DESTRUCTIBLETYPE_SPINNER)
		{
			SETVECTOR(rotAxis, 0.0f, 1.0f, 0.0f, 0.0f);

			//rotate food, whoo hoo: a dumb waiter sim
			rotAxis[3] = RAD(120.0f)/fps;
			bmRotationToQuat(tempQuat, rotAxis);
			bmQuatMultiply(rotInc, ptr->actorInstance.orientation, tempQuat);
			bmQuatCopy(ptr->actorInstance.orientation, rotInc);
		}

		if (ptr) 
		{
			UpdateDestroyEverything(ptr);
			if (guiDebugDestruct)
			{
				if (ptr->destructibleObjectInfo->flags&DESTRUCTIBLEFLAG_COUNT)
				{
					if(GetCollisionEntry(&collisionCache, ptr))
					{
						utilDrawLine(gameStatus.player1->actorInstance.position, ptr->actorInstance.position, COLOUR(255,0,0));
					}
					else
					{
						utilDrawLine(gameStatus.player1->actorInstance.position, ptr->actorInstance.position, COLOUR(0,0,255));
					}
				}
				else
				{
					utilDrawLine(gameStatus.player1->actorInstance.position, ptr->actorInstance.position, COLOUR(0,255,0));
				}
			}
		}
	}
}

/*  --------------------------------------------------------------------------------
	Function	: DestroyEverything
	Purpose		: Starts destroying everything on the level
	Parameters	: 
	Returns		: 
	Info		: 
*/

static	bool destroyEverything = false;

void DestroyEverything(void)
{
	destroyEverything = true;
}

/*  --------------------------------------------------------------------------------
	Function	: UpdateDestroyEverything
	Purpose		: Called each frame to destroy level stuff
	Parameters	: ptr to destructible object
	Returns		: 
	Info		: 
*/
#define MAX_DESTROYED_THIS_FRAME (5)

void UpdateDestroyEverything(ACTORINSTANCE *actorInstance)
{
	static int destroyedThisFrame = 0;
	char	tempString[256];
	ACTORINSTANCE *destructor;
	int	tempState;

	if (!actorInstance) 
	{
		destroyedThisFrame = 0;
		return;
	}
	if (!destroyEverything) return;

	if((actorInstance->destructibleObjectInfo->type != DESTRUCTIBLETYPE_OBJECTSPIN)&&
		(actorInstance->destructibleObjectInfo->type != DESTRUCTIBLETYPE_ANIMATE)&&
		(actorInstance->destructibleObjectInfo->type != DESTRUCTIBLETYPE_ANIMATECONTACT)&&
		(actorInstance->destructibleObjectInfo->type != DESTRUCTIBLETYPE_ANIMATELOOP)&&
		(actorInstance->destructibleObjectInfo->type != DESTRUCTIBLETYPE_SPARK)&&
		(actorInstance->destructibleObjectInfo->type != DESTRUCTIBLETYPE_SPIKEY))
	{
		if (destroyedThisFrame < MAX_DESTROYED_THIS_FRAME)
		{
			destroyedThisFrame++;
			// TP: better credit player 0 with destruction
			destructor = gameStatus.player[0].actorInstance;
			// TP: dirty bodge to simulate the effect of Taz spinning into the destructibles
			tempState = destructor->actorStatus->currentState;
			destructor->actorStatus->currentState = STATE_SMASH;
			if (actorInstance->special == SPECIALTYPE_DESTRUCTIBLE)
			{
				if (actorInstance->destructibleObjectInfo)
				{
					// TP: clear indestructible flag
					if (actorInstance->destructibleObjectInfo->flags&DESTRUCTIBLEFLAG_INDESTRUCTIBLE)
					{
						actorInstance->destructibleObjectInfo->flags &=~DESTRUCTIBLEFLAG_INDESTRUCTIBLE;
						actorInstance->destructibleObjectInfo->flags |= DESTRUCTIBLEFLAG_NODELAY;
						if (!RespondToDestructibleCollision(destructor,actorInstance))
						{
							actorInstance->destructibleObjectInfo->flags &= ~DESTRUCTIBLEFLAG_NODELAY;
							actorInstance->destructibleObjectInfo->flags |=DESTRUCTIBLEFLAG_INDESTRUCTIBLE;
						}
					}
					else
					{
						actorInstance->destructibleObjectInfo->flags |= DESTRUCTIBLEFLAG_NOBOUNCE|DESTRUCTIBLEFLAG_NODELAY;
						if (!RespondToDestructibleCollision(destructor,actorInstance))
						{
							actorInstance->destructibleObjectInfo->flags &= ~DESTRUCTIBLEFLAG_NOBOUNCE|DESTRUCTIBLEFLAG_NODELAY;
						}
					}
				}
			}
			destructor->actorStatus->lastState = destructor->actorStatus->currentState;
			destructor->actorStatus->currentState = (ECharState)tempState;
		}
	}
}

/*  --------------------------------------------------------------------------------
	Function	: BounceCharacter
	Purpose		: Reflect a characters velocity against a normal with an elasticity factor
	Parameters	: actorInstance, normal of plane to reflect against. 0-1 elasticity factor (1 - perfect reflection, 0 - stop dead
	Returns		: 
	Info		: 
*/
void BounceCharacter(ACTORINSTANCE *actorInstance, TBVector normal, float speed)
{
	TBVector	vel,tempVector;
	float		value;

	if (!(bmRand() % 4))
		PlaySample("spring2.wav",255,0,0,actorInstance->actorInstance.position,0);
	else
		PlaySample("spring.wav",255,0,0,actorInstance->actorInstance.position,0);

	normal[Y] = 0.0f;
	bmVectorNorm(normal,normal);

	// TP: vel contains current velocity
	bmVectorCopy(vel, actorInstance->actorBehaviour->physics.velocity);

	// TP: reflect vector against plane
	value = bmVectorDot(vel, normal);
	bmVectorCMul(tempVector,normal,value*3.0f);
	bmVectorSub(actorInstance->actorBehaviour->physics.velocity,actorInstance->actorBehaviour->physics.velocity,tempVector);
}

/*  --------------------------------------------------------------------------------
	Function	: BounceCharacter
	Purpose		: Reflect a characters velocity against a normal with an elasticity factor
	Parameters	: actorInstance, bounceActor. 0-1 elasticity factor (1 - perfect reflection, 0 - stop dead), hitsLeft - number of hits left until object breaks
	Returns		: 
	Info		: 
*/
void BounceCharacter(ACTORINSTANCE *actorInstance, ACTORINSTANCE *bounceActor, float speed, int hitsLeft)
{
	TBVector	vel,tempVector,normal;
	float		value;
	int			pitch=8000;

	if (hitsLeft) pitch -= 1000 * (hitsLeft>3?3:hitsLeft);

	bkPrintf("pitch %d\n", pitch);
	PlaySample("spring.wav",255,0,pitch,NULL,0);

	bmVectorSub(tempVector,actorInstance->actorInstance.position,bounceActor->actorInstance.position);
	bmVectorNorm(normal,tempVector);

	bmVectorCMul(tempVector,normal,speed);
	bmVectorAdd(actorInstance->actorBehaviour->physics.velocity,actorInstance->actorBehaviour->physics.velocity,tempVector);
}

/*  --------------------------------------------------------------------------------
	Function	: FindTexturesFromMapUsedBySparkingObjects
	Purpose		: Once all of the map has been loaded we can look for the textures used by sparking objects
	Parameters	: none
	Returns		: nothing
	Info		: check out the long function name
*/

void FindTexturesFromMapUsedBySparkingObjects(void)
{
	ACTORINSTANCE	*ptr;
	SPARKINFO		*info;
	int				i;

	//should only need to search the level instances
	for(ptr = map.levelInstances.head.next;ptr != &map.levelInstances.head;ptr = ptr->next)
	{
		if((ptr->special == SPECIALTYPE_DESTRUCTIBLE)&&(ptr->destructibleObjectInfo))
		{
			if(ptr->destructibleObjectInfo->type == DESTRUCTIBLETYPE_SPARK)
			{
				if(info = ptr->destructibleObjectInfo->sparkInfo)
				{
					if(info->numTextures != -1)
					{
						for(i = (info->numTextures-1);i >= 0;--i)
						{
							info->sparkTexture[i] = bkLoadTextureByCRC(NULL, info->sparkTextureCRC[i], 0);
						}
					}
					else
					{
						info->sparkTexture[0] = textureSpark;
					}
				}
			}
		}
	}
}