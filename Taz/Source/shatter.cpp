// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : shatter.cpp
//   Purpose : functions for dealing with shattner specialtypes
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "actors.h"
#include "polyeffects.h"
#include "collisioncache.h"
#include "scene.h"
#include "main.h"
#include "maths.h"
#include "status.h"
#include "animation.h"
#include "shatter.h"
#include "trigger.h"
#include "genericai.h"
#include "physics.h"

/* --------------------------------------------------------------------------------
   Function : CreateShatterObject
   Purpose : creates an object which waits to be shattered
   Parameters : 
   Returns : 
   Info : 
*/

int CreateShatterObject(ACTORINSTANCE *actorInstance, uint32 shatterType, int destroy, uint32 triggerType, float delay,uint32 idleAnimCRC,uint32 destroyAnimCRC, uint32 textureCRC)
{
	actorInstance->special = SPECIALTYPE_SHATTER;
	if((actorInstance->shatterInfo = CreateShatterInfo()) == NULL)
	{
		return FALSE;
	}

	actorInstance->shatterInfo->shatterType = shatterType;
	if((actorInstance->shatterInfo->triggerType = triggerType) == SHATTERTYPE_MARIOPAINTING)
	{
		actorInstance->shatterInfo->shatterType = POLYFX_RIPPLE;
	}
	actorInstance->shatterInfo->destroy = destroy;
	actorInstance->shatterInfo->collision = FALSE;
	actorInstance->shatterInfo->delay = delay;
	actorInstance->shatterInfo->clock = 0.0f;
	actorInstance->shatterInfo->destroyAnimCRC = destroyAnimCRC;
	actorInstance->shatterInfo->textureCRC = textureCRC;
	if(actorInstance->shatterInfo->idleAnimCRC = idleAnimCRC)
	{
		FlushAnimationQueue(actorInstance);
		PlayAnimationByCRCOfName(actorInstance,idleAnimCRC,1.0f,1,0,0.1f,IDLE);
	}
	actorInstance->shatterInfo->state = SHATTERSTATE_IDLE;

	AddActorToCollisionCache(&collisionCache, actorInstance, 0);
	SetPreCollisionCallback(actorInstance, PreRespondToShatterCollision);
	SetCollisionCallback(actorInstance, RespondToShatterCollision);
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : CreateShatterObject
   Purpose : creates an object which waits to be shattered
   Parameters : 
   Returns : 
   Info : 
*/

SHATTERINFO *CreateShatterInfo(void)
{
	SHATTERINFO		*tempInfo;

	if(tempInfo = (SHATTERINFO*)ZALLOC(sizeof(SHATTERINFO)))
	{
		return tempInfo;
	}
	else
	{
		bkPrintf("   *** WARNING ***, could not create shattnerinfo structure\n");
		return NULL;
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateShatterObject
   Purpose : updates an object which is waiting to be shattered
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateShatterObject(ACTORINSTANCE *ptr)
{
	TBVector		offset,pos;

	if(ptr->shatterInfo)
	{
		if(ptr->shatterInfo->collision)
		{
			if((ptr->shatterInfo->clock -= fTime) < 0.0f)
			{
				switch(ptr->shatterInfo->state)
				{
				case SHATTERSTATE_IDLE:
					if(ptr->shatterInfo->destroyAnimCRC)
					{
						FlushAnimationQueue(ptr);
						PlayAnimationByCRCOfName(ptr,ptr->shatterInfo->destroyAnimCRC,1.0f,0,0,0.1f,DEATH);
						ptr->shatterInfo->state = SHATTERSTATE_DESTROYANIM;
					}
					else
					{
						ptr->shatterInfo->state = SHATTERSTATE_SHATTER;
					}
					break;
				case SHATTERSTATE_DESTROYANIM:
					if(!CheckIfPlayingAnimation(ptr,NULL,ptr->shatterInfo->destroyAnimCRC))
					{
						ptr->shatterInfo->state = SHATTERSTATE_SHATTER;
					}
					break;
				case SHATTERSTATE_SHATTER:
					if (ptr->shatterInfo->onShatter)
					{
						bmVectorCopy(pos,ptr->actorInstance.position);
						if (ptr->shatterInfo->onShatter)
							SendTrigger(ptr->shatterInfo->onShatter, pos);
					}
					SETVECTOR(offset, 0.0f, 0.0f, 0.0f, 1.0f);
					switch(ptr->shatterInfo->triggerType)
					{
					case SHATTERTYPE_SPIN:
	//							if((gameStatus.player[0].actorInstance->actorStatus->currentState == STATE_SPIN)||(gameStatus.player[0].actorInstance->actorStatus->currentState == STATE_PROJECTILE))
	//							{
							ptr = CreatePolyLevelCopyOfActorInstance(&collisionCache,&polyEffectsList,ptr,offset,ptr->shatterInfo->destroy,
																bkLoadTextureByCRC(NULL, ptr->shatterInfo->textureCRC, 0),ptr->shatterInfo->shatterType);
	//							}
						break;
					case SHATTERTYPE_MARIOPAINTING:
						ptr = CreatePolyLevelCopyOfActorInstance(&collisionCache,&polyEffectsList,ptr,offset,FALSE,
																bkLoadTextureByCRC(NULL, ptr->shatterInfo->textureCRC, 0),ptr->shatterInfo->shatterType);
						ptr->flags |= ACTORFLAG_DONTDRAW;
						ptr->shatterInfo->clock = 5.0f;
						ptr->shatterInfo->triggerType = SHATTERTYPE_MARIOWAITING;
						break;
					default:
						ptr = CreatePolyLevelCopyOfActorInstance(&collisionCache,&polyEffectsList,ptr,offset,ptr->shatterInfo->destroy,
																bkLoadTextureByCRC(NULL, ptr->shatterInfo->textureCRC, 0),ptr->shatterInfo->shatterType);
						break;
					}
					if(ptr)
					{
						RemoveActorFromCollisionCache(&collisionCache, ptr);
						ptr->shatterInfo->collision = FALSE;
					}
					break;
				}
			}
		}
		else
		{
			switch(ptr->shatterInfo->triggerType)
			{
			case SHATTERTYPE_MARIOWAITING:
				if((ptr->shatterInfo->clock -= fTime) < 0.0f)
				{
					FreeAllPolyEffectsFromList(&polyEffectsList, POLYFX_RIPPLE);
					ptr->flags &= ~ACTORFLAG_DONTDRAW;
					AddActorToCollisionCache(&collisionCache, ptr, 0);
					ptr->shatterInfo->triggerType = SHATTERTYPE_MARIOPAINTING;
				}
				break;
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : RespondToShattnerCollision
   Purpose : guess
   Parameters : 
   Returns : 
   Info : 
*/

int PreRespondToShatterCollision(ACTORINSTANCE *collider, ACTORINSTANCE *shatter, PRECOLLINFO *info)
{
	switch(shatter->shatterInfo->triggerType)
	{
	case SHATTERTYPE_MARIOPAINTING:
		return FALSE;
	default:
		return TRUE;
	}
}

/* --------------------------------------------------------------------------------
   Function : RespondToShattnerCollision
   Purpose : guess
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToShatterCollision(ACTORINSTANCE *collider, ACTORINSTANCE *shatter)
{
	if (collider->genericAI) 
	{
		AICollideWithSpecial(collider,shatter);
		return FALSE;
	}
	switch(shatter->shatterInfo->triggerType)
	{
	case SHATTERTYPE_SPIN:
		switch(collider->actorStatus->currentState)
		{
		case STATE_SPIN:
		case STATE_SMASH:
		case STATE_PROJECTILE:
		case STATE_PROJECTILESLIDE:
		case STATE_ATLASSPIN:
		case STATE_ATLASSPINUP:
			shatter->shatterInfo->collision = TRUE;
			shatter->shatterInfo->clock = shatter->shatterInfo->delay;
			return TRUE;
		default:
			return FALSE;
		}
	case SHATTERTYPE_TRIGGER:
		shatter->shatterInfo->collision = FALSE;
		shatter->shatterInfo->clock = shatter->shatterInfo->delay;
		return FALSE;
	case SHATTERTYPE_MARIOPAINTING:
		shatter->shatterInfo->collision = TRUE;
		return FALSE;
	case SHATTERTYPE_DEFAULT:
		shatter->shatterInfo->collision = TRUE;
		shatter->shatterInfo->clock = shatter->shatterInfo->delay;
		return TRUE;
	}
	return FALSE;
}
