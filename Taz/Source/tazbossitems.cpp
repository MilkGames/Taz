// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : tazbossitems.cpp
//   Purpose : functions to do with the items used in the mecha tweety boss game
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "main.h"
#include "animation.h"
#include "status.h"
#include "characters.h"
#include "eating.h"
#include "control.h"
#include "destructibles.h"
#include "breakables.h"
#include "mechatweety.h"
#include "textures.h"
#include "electricityattack.h"
#include "mtweetyscripts.h"
#include "tazbossitems.h"

float		buttonTimes[4];

/* --------------------------------------------------------------------------------
   Function : RandomiseButtonTimes
   Purpose : randomises the times which the buttons take to reset
   Parameters : 
   Returns : 
   Info : 
*/

void RandomiseButtonTimes(void)
{
	int		i, num;
	float	values[4] = {30.0f, 20.0f, 10.0f, 2.5f};

	for(i = 3;i >= 0;--i)
	{
		// CMD: choose random member
		buttonTimes[i] = values[num = bmRand()%(i+1)];
		// CMD: now shuffle the array
		values[num] = values[i];
	}
}

/* --------------------------------------------------------------------------------
   Function : CreateTazBossItemInstance
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateTazBossItemInstance(ACTORINSTANCE *instance, ETazBossItem type)
{
	TAZBOSSITEMINFO	*info;

	instance->special = SPECIALTYPE_TAZBOSSITEM;
	instance->tazBossItemInfo = (TAZBOSSITEMINFO*)ZALLOC(sizeof(TAZBOSSITEMINFO));
	ASSERTM(instance->tazBossItemInfo, "out of memory");

	info = instance->tazBossItemInfo;
	info->type = type;
	info->flags = 0;
	info->clock = 0.0f;
	info->debris = NULL;
	info->soundHandle = -1;
//	EnableActorCartoonRenderMode(&instance->actorInstance);

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : CreateTazBossItemInstance
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int InitialiseTazBossItemInstance(ACTORINSTANCE *instance)
{
	TAZBOSSITEMINFO	*info = instance->tazBossItemInfo;
	int32			tempInt;

	RemoveActorFromCollisionCache(&collisionCache, instance);

	switch(info->type)
	{
	case TAZBOSSITEM_TAZCRATE:
		FlushAnimationQueue(instance);
		PlayAnimationByCRCOfName(instance, info->animCRC, 1.0f, 0, 0, 0.1f, NONE);
		AddActorToCollisionCache(&collisionCache, instance, CACHEFLAGS_LIVEUPDATE);
		instance->flags |= ACTORFLAG_DONTFADE;
		info->state = BOSSITEMSTATE_ANIMATING;
		break;
	case TAZBOSSITEM_SHUTTER:
		FlushAnimationQueue(instance);
		PlayAnimationByCRCOfName(instance, info->animCRC, 1.0f, 0, 0, 0.1f, NONE);
		break;
	case TAZBOSSITEM_TWEETYCRATE:
		break;
	case TAZBOSSITEM_DESTRUCTIBLE:
#if BPLATFORM==GAMECUBE
		AddActorToCollisionCache(&collisionCache, instance, CACHEFLAGS_LIVEUPDATE); // IH: Need live update - although this might push you through mesh
#else
		AddActorToCollisionCache(&collisionCache, instance, 0);
#endif
		SetCollisionCallback(instance, RespondToTazBossBreakableCollision);
		break;
	case TAZBOSSITEM_CAMERA:
		// CMD: temp move until camera can be moved on map
		switch(info->number)
		{
		case 1:
			instance->actorInstance.position[2] += METERTOUNIT(2.0f);
			break;
		case 2:
			instance->actorInstance.position[1] += METERTOUNIT(1.25f);
			break;
		case 3:
//			instance->actorInstance.position[2] += METERTOUNIT(2.0f);
			break;
		}
		// CMD: end temp
		instance->flags |= ACTORFLAG_DONTDRAW;
		break;
	case TAZBOSSITEM_CAMERACONTROL:
		tempInt = info->number;
		info->number = tempInt%10;
		tempInt -= info->number;
		tempInt /= 10;
		info->destination = tempInt%10;
		instance->flags |= ACTORFLAG_DONTDRAW;
		break;
	case TAZBOSSITEM_DOOR:
		FlushAnimationQueue(instance);
		RemoveActorFromCollisionCache(&collisionCache, instance);
		break;
	case TAZBOSSITEM_MAGNET:
		FlushAnimationQueue(instance);
		PlayAnimationByName(instance, "idle", 1.0f, 1, 0, 0.1f, NONE);
		AddActorToCollisionCache(&collisionCache, instance, CACHEFLAGS_LIVEUPDATE);
		info->state = BOSSITEMSTATE_LOWERED;
		break;
	case TAZBOSSITEM_MAGNETHITPOINT:
		FlushAnimationQueue(instance);
		PlayAnimationByName(instance, "idle", 1.0f, 1, 0, 0.1f, NONE);
		DisableActorCartoonRenderMode(&instance->actorInstance);
		AddActorToCollisionCache(&collisionCache, instance, 0);
		SetCollisionCallback(instance, RespondToMagnetHitPointCollision);
		// CMD: find gifs that will be used to signify state change
		switch(info->number)
		{
		case 1:
			info->texture = SpecifyTexturesToAnimateManually("textures\\greenred01.gif");
			break;
		case 2:
			info->texture = SpecifyTexturesToAnimateManually("textures\\greenred02.gif");
			break;
		case 3:
			info->texture = SpecifyTexturesToAnimateManually("textures\\greenred03.gif");
			break;
		case 4:
			info->texture = SpecifyTexturesToAnimateManually("textures\\greenred04.gif");
			break;
		}
		if(info->texture) bdSetTextureFrame(info->texture, (info->textureFrame = 0));
		info->state = BOSSITEMSTATE_LOWERED;
		// CMD: initialise colour to green
		info->colour.r = 0;
		info->colour.g = 119;
		info->colour.b = 0;
		info->colour.f_alpha = BUTTON_GLOW_MAX_A;
		break;
	case TAZBOSSITEM_MAGNETNODE:
		info->state = BOSSITEMSTATE_LOWERED;
		instance->flags |= ACTORFLAG_DONTDRAW;
		break;
	case TAZBOSSITEM_DOORBUTTON:
		FlushAnimationQueue(instance);
		PlayAnimationByName(instance, "idle", 1.0f, 1, 0, 0.1f, IDLE);
		AddActorToCollisionCache(&collisionCache, instance, 0);
		SetCollisionCallback(instance, RespondToDoorButtonCollision);
		info->state = BOSSITEMSTATE_RESET;
		info->clock = 0.0f;
		instance->flags |= ACTORFLAG_DONTFADE;
		info->texture = textureGenericGlow;
		info->node = baFindNode(instance->actorInstance.rootNodeInstance, "bank_button");
		// CMD: initialise colour to red
		info->colour.r = 119;
		info->colour.g = 0;
		info->colour.b = 0;
		info->colour.f_alpha = BUTTON_GLOW_MAX_A;
		break;
	case TAZBOSSITEM_COLLPLANE:
		RemoveInstanceFromInstanceList(instance, &map.animatedInstances);
		AddInstanceToInstanceList(instance, &map.invisibleInstances);
		AddActorToCollisionCache(&collisionCache, instance, 0);
		break;
	default:
		instance->flags |= ACTORFLAG_DONTFADE;
		break;
	}
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : UpdateTazBossItemInstance
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateTazBossItemInstance(ACTORINSTANCE *instance)
{
	TAZBOSSITEMINFO		*info = instance->tazBossItemInfo;
	ACTORINSTANCE		*newInstance;
	TBActorAnimSegment	*animation;
	TBActorNodeInstance	*node;
	TBActor				*actor;
	TBVector			position, scale;
	float				tempFloat;
	int					i;

	switch(info->type)
	{
	case TAZBOSSITEM_TAZCRATE:
		if(~info->flags & BOSSITEMFLAG_CHECKTAZCRATE)
		{
			info->flags |= BOSSITEMFLAG_CHECKTAZCRATE;
			break;
		}
		switch(info->state)
		{
		case BOSSITEMSTATE_ANIMATING:
			animation = baFindAnimSegmentByCRC(&instance->actorInstance, info->animCRC); 
			if(!CheckIfPlayingAnimationPointedTo(&instance->actorInstance, animation))
			{
				bkPrintf("anim finished for instance : %s\n", instance->actorName);
				node = baFindNode(instance->actorInstance.rootNodeInstance, "box");
				if(node)
				{
					//activate invisibility on animated crate
					instance->flags |= ACTORFLAG_CUSTOMDRAW;
					RemoveActorFromCollisionCache(&collisionCache, instance);
					//place new 'destructible' version in scene
					baGetNodesWorldPosition(&instance->actorInstance, node, position);
					SETVECTOR(scale, 1.0f, 1.0f, 1.0f, 1.0f);
					actor = FindActorInActorList(info->replacementCRC);
					newInstance = CreateActorInstanceFromActorPtr(&map.levelInstances, actor, info->replacementCRC, "replacement");
					bmVectorCopy(newInstance->actorInstance.position, position);
					bmVectorCopy(newInstance->actorInstance.scale, scale);
					bmQuatCopy(newInstance->actorInstance.orientation, instance->actorInstance.orientation);
					if(CreateTazBossItemInstance(newInstance, TAZBOSSITEM_DESTRUCTIBLE))
					{
						newInstance->tazBossItemInfo->original = instance;
						InitialiseTazBossItemInstance(newInstance);
						// CMD: copy over debris info
						if(instance->tazBossItemInfo->debris)
						{
							newInstance->tazBossItemInfo->debris = (DESTRUCTIBLEDEBRISINFO*)ZALLOC(sizeof(DESTRUCTIBLEDEBRISINFO));
							for(i = ((newInstance->tazBossItemInfo->debris->numDebris = instance->tazBossItemInfo->debris->numDebris)-1);i >= 0;--i)
							{
								newInstance->tazBossItemInfo->debris->debrisCRC[i] = instance->tazBossItemInfo->debris->debrisCRC[i];
							}
						}
					}
					info->state = BOSSITEMSTATE_WAITING;
				}
			}
			break;
		case BOSSITEMSTATE_WAITING:
			break;
		}
		break;
		// CMD: update magnets
	case TAZBOSSITEM_MAGNET:
		switch(info->state)
		{
		case BOSSITEMSTATE_LOWERED:
			// CMD: check for end of lowering anim
			if(!(mTweetyGenericInfo->flags & MTWEETY_DRAWMAGNETFX))
			{
				if(!CheckIfPlayingAnimation(instance, "mag drop idle"))
				{
					mTweetyGenericInfo->flags |= MTWEETY_DRAWMAGNETFX;
				}
			}
			if(info->flags & BOSSITEMFLAG_RAISEMAGNET)
			{
				FlushAnimationQueue(instance);
				PlayAnimationByName(instance, "mag lift", 1.0f, 0, 0, 0.1f, REACT);
				PlayAnimationByName(instance, "mag lift idle", 1.0f, 1, 1, 0.1f, IDLE);
				info->flags &= ~BOSSITEMFLAG_RAISEMAGNET;
				info->state = BOSSITEMSTATE_RAISED;
				if(info->flags & BOSSITEMFLAG_LOWERMAGNET)
				{
					bkPrintf("");
				}

				mTweetyGenericInfo->flags &= ~MTWEETY_DRAWMAGNETFX;
			}
			break;
		case BOSSITEMSTATE_RAISED:
			if(info->flags & BOSSITEMFLAG_LOWERMAGNET)
			{
				FlushAnimationQueue(instance);
				PlayAnimationByName(instance, "mag drop idle", 1.0f, 0, 0, 0.1f, REACT);
				PlayAnimationByName(instance, "idle", 1.0f, 1, 1, 0.1f, IDLE);
				info->flags &= ~BOSSITEMFLAG_LOWERMAGNET;
				info->state = BOSSITEMSTATE_LOWERED;
			}
			break;
		}
		break;
	case TAZBOSSITEM_MAGNETHITPOINT:
		switch(info->state)
		{
		case BOSSITEMSTATE_LOWERED:
			break;
		case BOSSITEMSTATE_RAISED:
			break;
		}
		break;
	case TAZBOSSITEM_DOORBUTTON:
		switch(info->state)
		{
		case BOSSITEMSTATE_RESET:
			break;
		case BOSSITEMSTATE_LOWERED:
			if((info->clock -= fTime) < 3.0f)
			{
				// CMD: button about to reset, start flashing
				tempFloat = bmSin(RAD(720.0f)*info->clock);
				info->colour.f_alpha = BUTTON_GLOW_MAX_A*tempFloat*tempFloat;
			}
			if(info->clock < 0.0f)
			{
				// CMD: time is up, button should now reset
				info->state = BOSSITEMSTATE_RESET;
				info->clock = 0.0f;

				FlushAnimationQueue(instance);
//				PlayAnimationByName(instance, "resetAnim", 1.0f, 0, 0, 0.1f, REACT);
				PlayAnimationByName(instance, "idle", 1.0f, 1, 0, 0.1f, IDLE);
				// CMD: reset colour
				info->colour.r = 119;
				info->colour.g = 0;
				info->colour.b = 0;
				info->colour.f_alpha = BUTTON_GLOW_MAX_A;
			}
			break;
		}
		break;
	case TAZBOSSITEM_TWEETYCRATE:
		// CMD: decrement scale in an attempt to counteract funny effects caused by fov
		tempFloat = instance->actorInstance.scale[0];
		if((tempFloat -= 1.5f*fTime) < 0.5f) tempFloat = 0.5f;
		bmVectorSet(instance->actorInstance.scale, tempFloat, tempFloat, tempFloat, 1.0f);
		break;
	case TAZBOSSITEM_SHUTTER:
		if(info->soundHandle == -1)
		{
			switch(info->number)
			{
			case 0:
			case 1:
			case 2:
			case 3:
				info->soundHandle = PlayTrackingSample("loop\\conveyor.wav", 255, instance->actorInstance.position);
				break;
			}
		}
	}

	if(info->flags & BOSSITEMFLAG_DESTROYASAP)
	{
		DestroyObject(instance);
	}
	if(info->flags & BOSSITEMFLAG_USECLOCK)
	{
		if((info->clock -= fTime) < 0.0f)
		{
			DestroyObject(instance);
		}
	}
	//temp
	if(info->type == TAZBOSSITEM_DOOR)
	{
		if(mTweetyGenericInfo->openDoor == 1)
		{
			mTweetyGenericInfo->openDoor = 2;
//			PlayAnimationByName(instance, "open doors", 1.0f, 0, 0, 0.1f, DEATH);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawTazBossItemStuff
   Purpose : Draw any taz boss item stuff that doesn't get drawn elsewhere 
   Parameters : 
   Returns : 
   Info : 
*/

void DrawTazBossItemStuff(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE		*item;
	TAZBOSSITEMINFO		*info;
	TBVector			tempVector;

	// PP: Z-BIAS VALUES
	// PP: NOTE: the zbias scale used by bdDrawFlatSprite is different to the one used by bdSetZBias.
	// PP: I expect this is the 2D zbias scale; that'd make sense.

#if(BPLATFORM == PS2)
#define ZBIAS_MAGNETHITPOINT		METERTOUNIT(1.0f)
#define ZBIAS_DOORBUTTON			METERTOUNIT(-2.0f)
#else// PP: !ps2
#define ZBIAS_MAGNETHITPOINT		0.001f					// PP: put the glow just behind the pillar
#define ZBIAS_DOORBUTTON			(-0.0005f)				// PP: put the glow in front of the button but behind the mech - is this right Claude?
#endif// PP: !ps2

	// PP: no z-write - we don't want to risk blocking any other effects
	SETZWRITE(FALSE);

	for(item = actorInstanceList->head.next;item != &actorInstanceList->head;item = item->next)
	{
		if(item->special == SPECIALTYPE_TAZBOSSITEM)
		{
			info = item->tazBossItemInfo;

			switch(item->tazBossItemInfo->type)
			{
			case TAZBOSSITEM_DOORBUTTON:
				baGetNodesWorldPosition(&item->actorInstance, info->node, tempVector);
				tempVector[3] = 1.0f;

				bdDrawFlatSprite(tempVector, BUTTON_GLOW_SIZE, BUTTON_GLOW_SIZE, 0.0f, info->texture, 0, 0,
									info->colour.r, info->colour.g, info->colour.b, bmFloatToInt(info->colour.f_alpha)<<1, NULL, ZBIAS_DOORBUTTON);
				break;
			case TAZBOSSITEM_MAGNETHITPOINT:
				if(~mTweetyGenericInfo->flags & MTWEETY_DRAWMAGNETFX)
				{
					item->actorInstance.position[3] = 1.0f;

					bdDrawFlatSprite(item->actorInstance.position, BUTTON_GLOW_SIZE*0.85f, BUTTON_GLOW_SIZE*0.85f, 0.0f, textureGenericGlow, 0, 0,
										info->colour.r, info->colour.g, info->colour.b, bmFloatToInt(info->colour.f_alpha)<<1, NULL, ZBIAS_MAGNETHITPOINT);

				}
				break;
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : eatCallback
   Purpose : Callback for when state is changed 
   Parameters : ACTORINSTANCE object hitting rock, ACTORINSTANCE *destructible
   Returns : 
   Info : 
*/
void myEatCallback(ACTORINSTANCE *actorInstance)
{
	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance,"eatlikeananimal",1.0f,0,0,0.1f,NONE);
}

/* --------------------------------------------------------------------------------
   Function : RespondToTazBossBreakableCollision
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToTazBossBreakableCollision(ACTORINSTANCE *collider, ACTORINSTANCE *destructible)
{
	ACTORINSTANCE			*original, *shutter;
	TBActorInstance			*tempInstance;
	DESTRUCTIBLEOBJECTINFO	*destInfo;
	TBVector				tempPos, tempScale;
	TBQuaternion			tempQuat;
	int32					destroyed;

	original = destructible->tazBossItemInfo->original;

	if(collider->actorStatus)
	{
		switch(collider->actorStatus->currentState)
		{
		case STATE_SPIN:
		case STATE_ELECTROCUTED:
			destroyed = TRUE;
			PlaySample("crashwood 1-1.wav", 255, destructible->actorInstance.position);
			DestroyObject(destructible, destructible->tazBossItemInfo->debris, TRUE);
			break;
		case STATE_BITE:
			destroyed = TRUE;
			if(collider->characterInfo)
			{
				if (collider->characterInfo->eating)
				{
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
						if(!collider->characterInfo->inMouth)
						{
							collider->characterInfo->inMouth = destInfo = (DESTRUCTIBLEOBJECTINFO*)ZALLOC(sizeof(DESTRUCTIBLEOBJECTINFO));
							destInfo->debris = (DESTRUCTIBLEDEBRISINFO*)ZALLOC(sizeof(DESTRUCTIBLEDEBRISINFO));
							if (destInfo->debris)
							{
								destInfo->debris->debrisCRC[0] = destructible->actorCRC;
								destInfo->debris->numDebris = 1;
							}
							destInfo->hitTriggers = NULL;
							destInfo->animation = NULL;
							destInfo->explosion = NULL;
							destInfo->sparkInfo = NULL;
							RequestStateChangePlus(collider, STATE_EAT, myEatCallback);
						}
					}
				}
				FreeActorInstance(destructible, 0);
			}
			break;
		default:
			destroyed = FALSE;
			break;
		}
	}
	else
	{
		if(collider->special == SPECIALTYPE_TAZBOSSITEM)
		{
			switch(collider->tazBossItemInfo->type)
			{
			case TAZBOSSITEM_TWEETYCRATE:
			case TAZBOSSITEM_DESTRUCTIBLE:
				collider->tazBossItemInfo->flags |= BOSSITEMFLAG_DESTROYASAP;
				destructible->tazBossItemInfo->flags |= BOSSITEMFLAG_DESTROYASAP;
				destroyed = TRUE;
				break;
			}
		}
	}

	if(destroyed)
	{
		original->tazBossItemInfo->state = BOSSITEMSTATE_ANIMATING;
		original->flags &= ~ACTORFLAG_CUSTOMDRAW;
		AddActorToCollisionCache(&collisionCache, original, CACHEFLAGS_LIVEUPDATE);
		FlushAnimationQueue(original);
		PlayAnimationByCRCOfName(original, original->tazBossItemInfo->animCRC, 1.0f, 0, 0, 0.0f, NONE);
		// CMD: find shutter that matches box and play its anim too
		for(shutter = map.animatedInstances.head.next;shutter != &map.animatedInstances.head;shutter = shutter->next)
		{
			if(shutter->special == SPECIALTYPE_TAZBOSSITEM)
			{
				if(shutter->tazBossItemInfo)
				{
					if(shutter->tazBossItemInfo->type == TAZBOSSITEM_SHUTTER)
					{
						if(shutter->tazBossItemInfo->number == original->tazBossItemInfo->number)
						{
							FlushAnimationQueue(shutter);
							PlayAnimationByCRCOfName(shutter, shutter->tazBossItemInfo->animCRC, 1.0f, 0, 0, 0.1f, NONE);
							break;
						}
					}
				}
			}
		}
	}
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : RespondToMagnetHitPointCollision
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToMagnetHitPointCollision(ACTORINSTANCE *collider, ACTORINSTANCE *hitPoint)
{
	if(collider->actorStatus)
	{
		if((collider->actorStatus->currentState == STATE_SPIN)||(collider->actorStatus->currentState == STATE_SMASH))
		{
			// CMD: for the moment, and until the anims are done this is all we can do
			if(hitPoint->tazBossItemInfo)
			{
				switch(hitPoint->tazBossItemInfo->state)
				{
				case BOSSITEMSTATE_DESTROYED:
					// CMD: hit point now supplying "bad" electricity to tweety
					break;
				default:
					if(++hitPoint->tazBossItemInfo->textureFrame > 1)
					{
						hitPoint->tazBossItemInfo->textureFrame = 2;
						hitPoint->tazBossItemInfo->state = BOSSITEMSTATE_DESTROYED;
						mTweetyGenericInfo->magnetsDestroyed[(hitPoint->tazBossItemInfo->number)-1] = TRUE;
					}
					switch(hitPoint->tazBossItemInfo->textureFrame)
					{
					case 1:
						// CMD: set glow to amber
						hitPoint->tazBossItemInfo->colour.r = 127;
						hitPoint->tazBossItemInfo->colour.g = 57;
						hitPoint->tazBossItemInfo->colour.b = 11;
						break;
					case 2:
						// CMD: set glow to red
						hitPoint->tazBossItemInfo->colour.r = 119;
						hitPoint->tazBossItemInfo->colour.g = 0;
						hitPoint->tazBossItemInfo->colour.b = 0;
						break;
					}
					bdSetTextureFrame(hitPoint->tazBossItemInfo->texture, hitPoint->tazBossItemInfo->textureFrame);
					FlushAnimationQueue(hitPoint);
					PlayAnimationByName(hitPoint, "hit mag", 1.0f, 0, 0, 0.1f, REACT);
					PlayAnimationByName(hitPoint, "hit mag", 1.0f, 0, 1, 0.1f, REACT);
					PlayAnimationByName(hitPoint, "hit mag", 1.0f, 0, 1, 0.1f, REACT);
					PlayAnimationByName(hitPoint, "hit mag", 1.0f, 0, 1, 0.1f, REACT);
					PlayAnimationByName(hitPoint, "idle", 1.0f, 1, 1, 0.1f, IDLE);
					// CMD: explosion
					COLOUR	expCols[FB_NUM_STAGE_COLOURS]={FBP_COLOUR_HOT, FBP_COLOUR_COLD, FBP_COLOUR_SMOKE, FBP_COLOUR_FINAL};
					Fireball_flatExplosion(hitPoint->actorInstance.position, METERTOUNIT(2.0f), METERTOUNIT(1.0f),expCols[0], expCols[1], expCols[2], expCols[3]); 

					// CMD: rumble
					RequestRumbleEffect(&controller1, 255, 0.1f, 0.3f, 1.5f, 0.0f, WAVEFORM_NORMAL);
					RequestNewMechaTweetySequence(globalMechaTweety, grumpyStompAndLowerMagnet);

					collider->flags |= ACTORFLAG_DESTROYNEXTUPDATE;
					break;
				}
			}
		}
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : RespondToDoorButtonCollision
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToDoorButtonCollision(ACTORINSTANCE *collider, ACTORINSTANCE *piston)
{
	TBActorNodeInstance		*node;
	TBVector				nodePos, sep;
	TAZBOSSITEMINFO			*info = piston->tazBossItemInfo;

	if(collider->actorStatus)
	{
		if((collider->actorStatus->currentState == STATE_SPIN)||(collider->actorStatus->currentState == STATE_SMASH))
		{
			// CMD: if collided with button node move to destoyed state
			// CMD: and start reset clock
			if(piston->tazBossItemInfo->state == BOSSITEMSTATE_RESET)
			{
				node = baFindNode(piston->actorInstance.rootNodeInstance, "bank_button");
				if(node)
				{
					baGetNodesWorldPosition(&piston->actorInstance, node, nodePos);
					bmVectorSub(sep, nodePos, collider->actorInstance.position);
					if(bmVectorLen(sep) < METERTOUNIT(2.0f))
					{
						// CMD: we have a winner
						// CMD: now we need to start the reset clock
						info->clock = buttonTimes[info->number - 1];
						info->state = BOSSITEMSTATE_LOWERED;

						FlushAnimationQueue(piston);
						PlayAnimationByName(piston, "destroy", 1.0f, 0, 0, 0.1f, REACT);
						RequestRumbleEffect(&controller1, 200, 0.5f, 0.3f, 1.0f, 0.0f, WAVEFORM_SQUARE, 4.0f);

						// CMD: set colour to green ... for open
						info->colour.r = 0;
						info->colour.g = 119;
						info->colour.b = 0;
					}
				}
			}
			collider->flags |= ACTORFLAG_DESTROYNEXTUPDATE;
		}
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : RespondToDoorButtonCollision
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CheckForCompletionOfTazWantedGame(void)
{
	ACTORINSTANCE		*ptr;

	for(ptr = map.animatedInstances.head.next;ptr != &map.animatedInstances.head;ptr = ptr->next)
	{
		if(ptr->special == SPECIALTYPE_TAZBOSSITEM)
		{
			if(ptr->tazBossItemInfo->type == TAZBOSSITEM_DOORBUTTON)
			{
				if(ptr->tazBossItemInfo->state == BOSSITEMSTATE_RESET) return FALSE;
			}
		}
	}
	// CMD: congratulations you have completed the game and me very happy
	// CMD: spin though list and play door open anim on all pistons and the door
	if(~globalMechaTweety->mechaTweetyInfo->flags & MTWEETY_WAITINGFORBOMBAYDOOR)
	{
		for(ptr = map.animatedInstances.head.next;ptr != &map.animatedInstances.head;ptr = ptr->next)
		{
			if(ptr->special == SPECIALTYPE_TAZBOSSITEM)
			{
				if(ptr->tazBossItemInfo->type == TAZBOSSITEM_DOORBUTTON)
				{
					if(ptr->tazBossItemInfo->state != BOSSITEMSTATE_DESTROYED)
					{
						FlushAnimationQueue(ptr);
						PlayAnimationByName(ptr, "gen_open", 1.0f, 0, 0, 0.1f, DEATH);
						ptr->tazBossItemInfo->state = BOSSITEMSTATE_DESTROYED;
					}
				}
				if(ptr->tazBossItemInfo->type == TAZBOSSITEM_DOOR)
				{
					if(ptr->tazBossItemInfo->number == 3)
					{
						FlushAnimationQueue(ptr);
						PlayAnimationByName(ptr, "open doors", 1.0f, 0, 0, 0.1f, DEATH);
						globalMechaTweety->mechaTweetyInfo->flags |= MTWEETY_WAITINGFORBOMBAYDOOR;
					}
				}
			}
		}
	}
	else
	{
		for(ptr = map.animatedInstances.head.next;ptr != &map.animatedInstances.head;ptr = ptr->next)
		{
			if(ptr->tazBossItemInfo->type == TAZBOSSITEM_DOOR)
			{
				if(ptr->tazBossItemInfo->number == 3)
				{
					if(!CheckIfPlayingAnimation(ptr, "open doors"))
					{
						globalMechaTweety->mechaTweetyInfo->flags &= ~MTWEETY_WAITINGFORBOMBAYDOOR;
						RequestNewMechaTweetySequence(globalMechaTweety, defeated);
						RequestRumbleEffect(&controller1, 200, 0.1f, 0.25f, 2.0f, 0.0f, WAVEFORM_RANDOM);
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}