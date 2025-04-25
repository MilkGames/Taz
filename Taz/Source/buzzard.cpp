// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : buzzard.cpp
//   Purpose : ai functions for buzzard
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "main.h"
#include "maths.h"
#include "physics.h"
#include "actors.h"
#include "util.h"
#include "chillipepper.h"
#include "debris.h"
#include "textures.h"
#include "characters.h"
#include "apes.h"
#include "animation.h"
#include "camera.h"
#include "lights.h"
#include "polyEffects.h"
#include "buzzard.h"

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceBuzzard
   Purpose : Create buzzard or parrot
   Parameters : 
   Returns : 
   Info : 
*/

int CreateActorInstanceBuzzard(ACTORINSTANCE *actorInstance)
{
	actorInstance->special = SPECIALTYPE_BUZZARD;

	// create extra structures
	ENABLE_LINE_OR_CARTOON_RENDER_MODE(&actorInstance->actorInstance);

	CreateActorInstanceBehaviour(actorInstance);
	CreateActorInstanceAnimation(actorInstance);
	CreateActorInstanceShadow(actorInstance,METERTOUNIT(1.7f),METERTOUNIT(2.5f), METERTOUNIT(0.5f));
	SetActorInertia(actorInstance->actorBehaviour,0);
	SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
	SetActorMaxSpeed(actorInstance->actorBehaviour,METERTOUNIT(15));

	actorInstance->buzzardInfo = (BUZZARDINFO*)ZALLOC(sizeof(BUZZARDINFO));
	ASSERTM(actorInstance->buzzardInfo, "buzzard info could not be created");

	actorInstance->flags |= (ACTORFLAG_DONTFADE|ACTORFLAG_NOCOLLISION);

	//set other atributes
	actorInstance->buzzardInfo->aiState = BUZZARDAI_NONE;
	actorInstance->buzzardInfo->aiLastState = BUZZARDAI_NONE;
	actorInstance->buzzardInfo->aiMetaState = BUZZARDAI_PERCH;
	actorInstance->buzzardInfo->stateChangeCallback = BuzzardAIStateChangeCallback;
	actorInstance->buzzardInfo->clock = 0.0f;
	actorInstance->buzzardInfo->flags = 0;
	actorInstance->buzzardInfo->currentScript = NULL;
	actorInstance->buzzardInfo->lastScript = NULL;
	actorInstance->buzzardInfo->scriptIndex = 0;
	actorInstance->buzzardInfo->scriptIndexLastFrame = -1;
	bmVectorCopy(actorInstance->buzzardInfo->homePos, actorInstance->actorInstance.position);
	bmQuatCopy(actorInstance->buzzardInfo->homeOrientation, actorInstance->actorInstance.orientation);

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : BuzzardReturnTrueCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 BuzzardReturnTrueCallback(ACTORINSTANCE *actorInstance, BUZZARDINFO *info)
{
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : BuzzardPlayAnimCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 BuzzardPlayAnimCallback(ACTORINSTANCE *actorInstance, BUZZARDINFO *info)
{
	if(info->clock)
	{
		if((info->clock -= fTime) < 0.0f)
		{
			return TRUE;
		}
	}
	else
	{
		if(!CheckIfPlayingAnimation(actorInstance, NULL))
		{
			return TRUE;
		}
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : BuzzardTakeOffCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 BuzzardTakeOffCallback(ACTORINSTANCE *actorInstance, BUZZARDINFO *info)
{
	TBVector		tempVector;
	TBQuaternion	destQuat;

	switch(info->aiSubState)
	{
	case BUZZARDAI_ASCENT:
		bmQuatSlerpAtFixedSpeed(destQuat, actorInstance->actorInstance.orientation, info->targetOrientation, RAD(20.0f)*fTime);

		if(actorInstance->actorInstance.position[1] > (info->homePos[1] + info->clock))
		{
			bmQuatCopy(info->targetOrientation, info->homeOrientation);
			info->aiSubState = BUZZARDAI_LEVELOFF;
		}
		break;
	case BUZZARDAI_LEVELOFF:
		if(QuatSlerpAtFixedSpeed(destQuat, actorInstance->actorInstance.orientation, info->targetOrientation, RAD(10.0f)*fTime))
		{
			SetActorOrientation(actorInstance, info->targetOrientation);
			FlushAnimationQueue(actorInstance);
			return TRUE;
		}
		break;
	}
	SetActorOrientation(actorInstance, destQuat);
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : BuzzardFlyHomeCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 BuzzardFlyHomeCallback(ACTORINSTANCE *actorInstance, BUZZARDINFO *info)
{
	TBQuaternion	tempQuat;
	TBVector		tempVector, smallVel, velocity, position, delta, offset;
	float			dist, size;
	int				colour[3];
	TBMatrix		matrix;
	int				i;

	// CMD: check if beaky has arrived home
	bmVectorSub(tempVector, info->targetPos, actorInstance->actorInstance.position);
	if(bmVectorLen(tempVector) < 2.0f*fTime*bmVectorLen(actorInstance->actorBehaviour->physics.velocity))
	{
		bmVectorCopy(actorInstance->actorInstance.position, info->targetPos);
		return TRUE;
	}
	switch(info->aiSubState)
	{
	case BUZZARDAI_TURN:
		SETVECTOR(velocity, UNITTOMETER((float)((bmRand()%(CHILLIPEPPER_FIRE_DISP*2))-CHILLIPEPPER_FIRE_DISP)), 
							UNITTOMETER((float)((bmRand()%(CHILLIPEPPER_FIRE_DISP)))), 
							(-1.0f-UNITTOMETER((float)((bmRand()%(CHILLIPEPPER_FIRE_DISP*2))-CHILLIPEPPER_FIRE_DISP))), 1.0f);

		bmQuatToMatrix(matrix, actorInstance->actorInstance.orientation);
		bmMatMultiplyVector(matrix, velocity);
		bmVectorScaleToLength(velocity, velocity, METERTOUNIT(-3.0f));

		bmVectorSet(delta, 0.0f, 0.0f, 0.0f, 1.0f);
		bmVectorSet(offset, 0.0f, 0.0f, -1.0f, 1.0f);
		bmMatMultiplyVector(matrix, offset);
		bmVectorScaleToLength(offset, offset, METERTOUNIT(3.0f)*0.5f*fTime);

		// CMD: create flames from rear end of rockets
		for(i=0;i<2;i++)
		{
			//need small displacement to seperate the sprites early in their life time
			SETVECTOR(smallVel,0.0f,0.0f,-((float)(bmRand()%CHILLIPEPPER_FIRE_RANDDISP)),1.0f);

			if((size = CHILLIPEPPER_FIRE_MAXSIZE*0.5f*velocity[2]/(int)CHILLIPEPPER_FIRE_RANDDISP) < 0.5f*CHILLIPEPPER_FIRE_MINSIZE)
			{
				size = 0.5f*CHILLIPEPPER_FIRE_MINSIZE;
			}

			bmMatMultiplyVector(matrix, smallVel);

			bmVectorAdd(position, actorInstance->actorInstance.position, smallVel);

			if(bmRand()%2)
			{
				colour[0]=255;colour[1]=38;colour[2]=10;
			}
			else
			{
				colour[0]=255;colour[1]=186;colour[2]=10;
			}
			bmVectorAdd(position, position, delta);

			// TP: done so we can pass the address to avoid double 
			float	tempFloat = CHILLIPEPPER_FIRE_LIFE;
			float	rand = UtilGenerateRandomNumber(RAD(360.0f), 0.0f);
			baCreateExtra(chilliFireSystem,position,velocity,textureChilliFire,&tempFloat,&rand,&size,colour[0],colour[1],colour[2],180,DEBRISFLAG_CHILLIFIRE|DEBRISFLAG_DONTDOCOLLISION|DEBRISFLAG_INCEDENTALFIRE);
			bmVectorAdd(delta, delta, offset);
		}
		// CMD: look to see if turn has completed
		bmDirectionToQuat(info->targetOrientation, tempVector, 0.0f);
		if(QuatSlerpAtFixedSpeed(tempQuat, actorInstance->actorInstance.orientation, info->targetOrientation, fTime*RAD(info->clock)))
		{
			FlushAnimationQueue(actorInstance);
			PlayAnimationByName(actorInstance, "fly1", 1.0f, 1, 0, 0.1f, REACT);
			// CMD: calculate target quaternion again just in case bmQuatSlerp... has gone funny again
			bmVectorSub(tempVector, info->targetPos, actorInstance->actorInstance.position);
			bmVectorNorm(tempVector, tempVector);
			bmDirectionToQuat(info->targetOrientation, tempVector, 0.0f);
			SetActorOrientation(actorInstance, info->targetOrientation);
			info->aiSubState = BUZZARDAI_STRAIGHT;
		}
		utilNormaliseQuaternion(tempQuat);
		SetActorOrientation(actorInstance, tempQuat);
		break;
	case BUZZARDAI_STRAIGHT:
		break;
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : BuzzardTurnCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 BuzzardTurnCallback(ACTORINSTANCE *actorInstance, BUZZARDINFO *info)
{
	TBQuaternion	destQuat;

	if(QuatSlerpAtFixedSpeed(destQuat, actorInstance->actorInstance.orientation, info->targetOrientation, fTime*RAD(90.0f)))
	{
		SetActorOrientation(actorInstance, info->targetOrientation);
		return TRUE;
	}
	SetActorOrientation(actorInstance, destQuat);
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : BuzzardLandCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 BuzzardLandCallback(ACTORINSTANCE *actorInstance, BUZZARDINFO *info)
{
	TBVector		tempVector;
	TBQuaternion	tempQuat;

	// CMD: check if beaky has arrived home
	bmVectorSub(tempVector, info->targetPos, actorInstance->actorInstance.position);
	if(bmVectorLen(tempVector) < 2.0f*fTime*bmVectorLen(actorInstance->actorBehaviour->physics.velocity))
	{
		SetActorOrientation(actorInstance, info->homeOrientation);
		bmVectorCopy(actorInstance->actorInstance.position, info->targetPos);
		FlushAnimationQueue(actorInstance);
		PlayAnimationByName(actorInstance, "land", 1.0f, 0, 0, 0.1f, REACT);
		info->aiSubState = BUZZARDAI_LAND;
		return FALSE;
	}
	switch(info->aiSubState)
	{
	case BUZZARDAI_BEGINAPPROACH:
		bmDirectionToQuat(info->targetOrientation, tempVector, 0.0f);
		bmQuatSlerpAtFixedSpeed(tempQuat, actorInstance->actorInstance.orientation, info->targetOrientation, fTime*RAD(180.0f));
		SetActorOrientation(actorInstance, tempQuat);
		return FALSE;
		break;
	case BUZZARDAI_LAND:
		if(!CheckIfPlayingAnimation(actorInstance, "land"))
		{
			SetActorOrientation(actorInstance, info->homeOrientation);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

/*  --------------------------------------------------------------------------------
	Function	: FriedEggHitCallback
	Purpose		: Gets called when the flying Banana hits something
	Parameters	: DEBRIS, COLLINFO from collision
	Returns		: FALSE to kill debris, TRUE to allow debris to continue
	Info		: 
*/
static int FriedEggHitCallback(DEBRIS *debris, COLL_INFO *collInfo)
{
	BANANASKIN		*friedEgg;
	TBVector		offset;
	ACTORINSTANCE	*tempInstance;
	float			tazDist;
	int				amp;

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

	if(debris->actorStructure)
	{
		// CMD: create shatter egg
		bmVectorSet(offset, 1.0f, 1.0f, 1.0f, 1.0f);
		tempInstance = CreateActorInstance(&map.levelInstances, "egg.obe", "beakyegg", debris->position, offset, bIdentityQuaternion, 0);
		AddActorToCollisionCache(&collisionCache, tempInstance, 0);
		bmVectorSet(offset, 0.0f, 0.0f, 0.0f, 1.0f);
		CreatePolyLevelCopyOfActorInstance(&collisionCache, &polyEffectsList, tempInstance, offset, TRUE, NULL, POLYFX_EXPLODE);
		// CMD: create slippy egg
		friedEgg = new	BANANASKIN(debris->actorInstance->position,debris->actorInstance->orientation,NULL,60.0f, "friedegg.obe");
		bmVectorSet(friedEgg->actorInstance->actorInstance.scale, 2.0f, 2.0f, 2.0f, 1.0f);
		// CMD: rumble
		if((tazDist = bmVectorDistance(gameStatus.player[0].actorInstance->actorInstance.position, debris->position)) < METERTOUNIT(10.0f))
		{
			amp = 100 + (120 - bmFloatToInt(120.0f*tazDist/METERTOUNIT(10.0f)));
			RequestRumbleEffect(gameStatus.player[0].controller, amp, 0.1f, 0.5f, 1.0f, 0.0, WAVEFORM_NORMAL);
		}
	}
	
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : BuzzardEggAttackCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 BuzzardEggAttackCallback(ACTORINSTANCE *actorInstance, BUZZARDINFO *info)
{
	TBVector				startPos, rotSpeed;
	TBActorNodeInstance		*node;
	DEBRIS					*debris;
	TBVector				tempVector;
	TBQuaternion			tempQuat;

	switch(info->aiSubState)
	{
	case BUZZARDAI_BEGINAPPROACH:
		// CMD: just play that anim for the desired time
		// CMD: beacky gets tired of the chase
		if((info->clock -= fTime) < 0.0f)
		{
			// CMD: beaky needs to relieve himself of his egg
			FlushAnimationQueue(actorInstance);
			PlayAnimationByName(actorInstance, "attack1", 1.0f, 0, 0, 0.1f, REACT);
			info->aiSubState = BUZZARDAI_LAND;
			return FALSE;
		}
		return FALSE;
	case BUZZARDAI_LAND:
		if(!CheckIfPlayingAnimation(actorInstance, "attack1"))
		{
			bkPrintf("**********************\neggs away\n**********************\n");
			// CMD: find start position
			node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "arse");
			ASSERTM(node, "could not find node to drop beaky's egg from");
			baGetNodesWorldPosition(&actorInstance->actorInstance, node, startPos);

			bmVectorSet(rotSpeed, 0.0f, 0.0f, 0.0f, 1.0f);
			if(debris = CreateDebrisActor("egg.obe", startPos, actorInstance->actorBehaviour->physics.velocity, rotSpeed, 3.0f,
										1.0f,0.0f, 0.0f, 256, DEBRISFLAG_PHYSICS, 0, 0, 0.0f))
			{
				debris->flags |= DEBRISFLAG_STICKY;
				debris->flags &=~DEBRISFLAG_AFFECTOBJECTS;
				SETVECTOR(debris->actorInstance->scale,1.0f,1.0f,1.0f,1.0f);
				EnableActorCartoonRenderMode(debris->actorInstance);
				debris->impactCallback = FriedEggHitCallback;
			}
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : BuzzardStateChangeCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 BuzzardAIStateChangeCallback(ACTORINSTANCE *actorInstance, EBuzzardAIState state, EBuzzardAIState lastState)
{
	BUZZARDINFO		*info = actorInstance->buzzardInfo;

	// PP: IFOUT: sorry, removes a warning
#if 0

	switch(lastState)
	{
	}

#endif// PP: 0

	switch(state)
	{
	case BUZZARDAI_END:
	case BUZZARDAI_LOOP:
		info->aiCallback = BuzzardReturnTrueCallback;
		break;
	case BUZZARDAI_PLAYANIM:
		info->aiCallback = BuzzardPlayAnimCallback;
		break;
	case BUZZARDAI_TAKEOFF:
		info->aiCallback = BuzzardTakeOffCallback;
		break;
	case BUZZARDAI_BEAKYFLYHOME:
		info->aiCallback = BuzzardFlyHomeCallback;
		break;
	case BUZZARDAI_TURNHOME:
		info->aiCallback = BuzzardTurnCallback;
		break;
	case BUZZARDAI_LAND:
		info->aiCallback = BuzzardLandCallback;
		break;
	case BUZZARDAI_EGGATTACK:
		info->aiCallback = BuzzardEggAttackCallback;
		break;
	}
	info->aiLastState = state;
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : UpdateBuzzard
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

void UpdateBuzzard(ACTORINSTANCE *actorInstance)
{
	BUZZARDINFO		*info = actorInstance->buzzardInfo;

	if(info)
	{
		if(info->aiState != info->aiLastState)
		{
			info->stateChangeCallback(actorInstance, info->aiState, info->aiLastState);
		}
		else
		{
			if(info->aiCallback)
			{
				if(info->aiCallback(actorInstance, info))
				{
					info->scriptIndex++;
				}
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateBuzzardLogic
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

void UpdateBuzzardLogic(ACTORINSTANCE *buzzard)
{
	ACTORINSTANCE	*instance;
	BUZZARDINFO		*info;
	BUZZARD_EVENT	*event;
	TBVector		tempVector;
	TBMatrix		tempMat;
	TBQuaternion	tempQuat;


	info = buzzard->buzzardInfo;

	if(info->currentScript)
	{
		if(info->scriptIndex != info->scriptIndexLastFrame)
		{
			info->scriptIndexLastFrame = info->scriptIndex;

			event = &info->currentScript[info->scriptIndex];

			switch(event->action)
			{
			case BUZZARDAI_END:
				info->currentScript = NULL;
				info->scriptIndex = 0;
				info->scriptIndexLastFrame = -1;
				info->aiState = BUZZARDAI_END;
				break;
			case BUZZARDAI_LOOP:
				info->scriptIndex = 0;
				info->scriptIndexLastFrame = -1;
				info->aiState = info->currentScript[0].action;
				break;
			case BUZZARDAI_PLAYANIM:
				info->animCRC = bkCRC32((uchar*)event->targetInstance, strlen(event->targetInstance), 0);
				info->clock = event->time;
				FlushAnimationQueue(buzzard);
				PlayAnimationByName(buzzard, event->targetInstance, 1.0f, (char)event->time, 0, 0.5f, REACT);
				info->aiState = BUZZARDAI_PLAYANIM;
				break;
			case BUZZARDAI_TAKEOFF:
				info->clock = event->time;					// CMD: in this case height
				// CMD: calculate target orientation (45 deg)
				SETVECTOR(tempVector, 1.0f, 0.0f, 0.0f, RAD(-45.0f));
				bmRotationToQuat(tempQuat, tempVector);
				bmQuatMultiply(info->targetOrientation, tempQuat, buzzard->actorInstance.orientation);

				FlushAnimationQueue(buzzard);
				PlayAnimationByName(buzzard, "fly3", 1.0f, 0, 0, 0.1f, REACT);
				PlayAnimationByName(buzzard, "fly1", 1.0f, 1, 1, 0.1f, REACT);
				info->aiState = BUZZARDAI_TAKEOFF;
				info->aiSubState = BUZZARDAI_ASCENT;
				break;
			case BUZZARDAI_BEAKYFLYHOME:
				info->clock = event->time;
				bmVectorCopy(info->targetPos, info->homePos);
				info->targetPos[1] = buzzard->actorInstance.position[1];

				FlushAnimationQueue(buzzard);
				PlayAnimationByName(buzzard, "fly2", 1.0f, 1, 0, 0.1f, REACT);
				info->aiState = BUZZARDAI_BEAKYFLYHOME;
				info->aiSubState = BUZZARDAI_TURN;
				break;
			case BUZZARDAI_BEGINAPPROACH:
				info->clock = event->time;
				// CMD: find target position
				SETVECTOR(tempVector, 0.0f, 0.0f, METERTOUNIT(15.0f), 1.0f);
				bmQuatToMatrix(tempMat, info->homeOrientation);
				bmMatMultiplyVector(tempMat, tempVector);
				bmVectorAdd(info->targetPos, tempVector, info->homePos);
				info->targetPos[1] += ((buzzard->actorInstance.position[1]-info->targetPos[1])*0.5f);

				FlushAnimationQueue(buzzard);
				PlayAnimationByName(buzzard, event->targetInstance, 1.0f, 1, 0, 0.1f, REACT);
				info->aiState = BUZZARDAI_BEAKYFLYHOME;
				info->aiSubState = BUZZARDAI_TURN;
				break;
			case BUZZARDAI_TURNHOME:
				bmQuatCopy(info->targetOrientation, info->homeOrientation);

				FlushAnimationQueue(buzzard);
				PlayAnimationByName(buzzard, "fly8", 1.0f, 0, 0, 0.1f, REACT);
				PlayAnimationByName(buzzard, "turn1", 1.0f, 1, 1, 0.1f, REACT);
				info->aiState = BUZZARDAI_TURNHOME;
				break;
			case BUZZARDAI_LAND:
				bmVectorCopy(info->targetPos, info->homePos);

				FlushAnimationQueue(buzzard);
				PlayAnimationByName(buzzard, "fly2", 1.0f, 1, 0, 0.1f, REACT);
				info->aiState = BUZZARDAI_LAND;
				info->aiSubState = BUZZARDAI_BEGINAPPROACH;
				break;
			case BUZZARDAI_EGGATTACK:
				info->clock = UtilGenerateRandomNumber(event->time, 0.0f);

				FlushAnimationQueue(buzzard);
				PlayAnimationByName(buzzard, event->targetInstance, 1.0f, 1, 0, 0.1f, REACT);
				info->aiState = BUZZARDAI_EGGATTACK;
				info->aiSubState = BUZZARDAI_BEGINAPPROACH;
				break;
			}
		}
	}
}
