// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : mechatweety.cpp
//   Purpose : mecha tweety boss game functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "physics.h"
#include "main.h"
#include "actors.h"
#include "control.h"
#include "animation.h"
#include "attach.h"
#include "status.h"
#include "breakables.h"
#include "util.h"
#include "projectile.h"
#include "display.h"
#include "chillipepper.h"
#include "debris.h"
#include "textures.h"
#include "characters.h"
#include "electricityattack.h"
#include "crateattack.h"
#include "tazbossitems.h"
#include "mtweetyscripts.h"
#include "tweetytransporter.h"
#include "mtweetymagnet.h"
#include "redshield.h"
#include "smashcamera.h"
#include "rocketscience.h"
#include "mechatweety.h"
#include "playerstats.h"
#include "transportereffect.h"

/******************************* Things To Do *******************************/
/*			--> Boxes on tweety's end in first room need to be fixed.		*/
/*			-->	Taz expresion of joy and fade to disbelief at end			*/
/*				of each stage.												*/
/*			--> Effect to make it apparent that the crate has hit			*/
/*				Tweety's shield												*/
/****************************************************************************/

ACTORINSTANCE				*globalMechaTweety;
MTWEETY_GENERICINFO			*mTweetyGenericInfo;


/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceMechaTweety
   Purpose : Create mecha tweety for final boss game
   Parameters : 
   Returns : 
   Info : Should be called after CreateActorInstance
*/

int CreateActorInstanceMechaTweety(ACTORINSTANCE *markerInstance)
{
	ACTORINSTANCE	*actorInstance;
	TBActorNodeInstance	*node;
	TBVector		tempVector;
	int32			handle;
	TBActorInstance	*light;

	SETVECTOR(tempVector, MTWEETY_SCALE, MTWEETY_SCALE, MTWEETY_SCALE, 1.0f);
	actorInstance = CreateActorInstance(&characterInstances, "mechatweety.obe", "mymechatweety", markerInstance->actorInstance.position,
											tempVector, markerInstance->actorInstance.orientation, 0);
	actorInstance->special = SPECIALTYPE_MECHATWEETY;

	// create extra structures
//	EnableActorCartoonRenderMode(&actorInstance->actorInstance);
	EnableActorLineRenderMode(&actorInstance->actorInstance);
	CreateActorInstanceBehaviour(actorInstance);
	CreateActorInstanceAnimation(actorInstance);
	CreateActorInstanceStatus(actorInstance);
	CreateActorInstanceShadow(actorInstance,METERTOUNIT(1.7f),METERTOUNIT(2.5f), METERTOUNIT(0.5f));
	CreateActorInstanceCharacterInfo(actorInstance);
	SetActorInertia(actorInstance->actorBehaviour,0);
	SetActorMinFricAngle(actorInstance->actorBehaviour,0.7f);
	SetActorMaxSpeed(actorInstance->actorBehaviour,METERTOUNIT(15)); // set max speed 8mps
	SETVECTOR(tempVector,0.0f,METERTOUNIT(0.705f),0.0f,1.0f);
	AddActorToCollisionCylinderCache(&collisionCache,actorInstance);
	SetActorCylinder(0,actorInstance->actorBehaviour,tempVector,METERTOUNIT(0.5f),METERTOUNIT(1.41f));

	//change current instance to that of mechatweety mesh
//	ChangeActorInstance(actorInstance,FindActorInActorList("mechatweety.obe"),5,NULL);
	handle = AttachObjectToCharacter(actorInstance,"mechatweetylight.obe","root",NULL,NULL,NULL,ATTACHFLAG_INHERITSCALE);
	light = ReturnAttachedObjectInstance(actorInstance, handle);
	FlushAnimationQueue(light);
	PlayAnimationByName(light, "rotate", 1.0f,1,0,0.1f,NONE);
	DisableActorCartoonRenderMode(light);

	actorInstance->mechaTweetyInfo = (MECHATWEETYINFO*)ZALLOC(sizeof(MECHATWEETYINFO));
	ASSERTM(actorInstance->mechaTweetyInfo, "mecha tweety info could not be created");

	actorInstance->special = SPECIALTYPE_MECHATWEETY;
	actorInstance->flags |= ACTORFLAG_DONTFADE;

	globalMechaTweety = actorInstance;

	//set other atributes
	actorInstance->mechaTweetyInfo->actorInstance = actorInstance;
	actorInstance->mechaTweetyInfo->aiState = MTWEETYAI_START;
	actorInstance->mechaTweetyInfo->aiLastState = MTWEETYAI_IDLE; 
	PlayAnimationByName(actorInstance, "idle1", 1.0f, 1, 0, 0.1f, IDLE);
	actorInstance->mechaTweetyInfo->aiCallback = MechaTweetyIdleCallback;
	actorInstance->mechaTweetyInfo->stateChangeCallback = MechaTweetyStateChangeCallback;
	actorInstance->mechaTweetyInfo->turnAmount = 0.0f;
	actorInstance->mechaTweetyInfo->flags = 0;
	actorInstance->mechaTweetyInfo->attachHandle = -1;
	actorInstance->mechaTweetyInfo->magnetSoundHandle = -1;
	actorInstance->mechaTweetyInfo->flySoundHandle = -1;

	actorInstance->mechaTweetyInfo->currentScript = NULL;
	actorInstance->mechaTweetyInfo->lastScript = NULL;
	actorInstance->mechaTweetyInfo->scriptIndex = 0;
	actorInstance->mechaTweetyInfo->scriptIndexLastFrame = -1;
	actorInstance->mechaTweetyInfo->scaleInfo.scale = MTWEETY_SCALE;
	actorInstance->mechaTweetyInfo->attackPosition = MTWEETYATTACK_MIDDLE;

	actorInstance->mechaTweetyInfo->currentStage = MTWEETYAREA_CRATES;
	actorInstance->mechaTweetyInfo->hitsToNextStage = 5;

	actorInstance->mechaTweetyInfo->leftElectricAttack = CreateElectricityAttack(MTWEETY_NUMPOINTS, MTWEETY_NUMSTREAKS);
	FillInAttributesForMTweetyGame(actorInstance->mechaTweetyInfo->leftElectricAttack);
	node = actorInstance->mechaTweetyInfo->leftElectricAttack->node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "arml_lower");
	actorInstance->mechaTweetyInfo->rightElectricAttack = CreateElectricityAttack(MTWEETY_NUMPOINTS, MTWEETY_NUMSTREAKS);
	FillInAttributesForMTweetyGame(actorInstance->mechaTweetyInfo->rightElectricAttack);
	node = actorInstance->mechaTweetyInfo->rightElectricAttack->node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "armr_lower");

	actorInstance->mechaTweetyInfo->headNode = baFindNode(actorInstance->actorInstance.rootNodeInstance, "tweetyeyes_flex");

	actorInstance->mechaTweetyInfo->shieldInfo = CreateRedShieldArroundActor(actorInstance, "root", METERTOUNIT(2.0f));

	InitialiseMagnetStreaks(actorInstance->mechaTweetyInfo);
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : MechaTweetyIdleCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyIdleCallback(MECHATWEETYINFO *mechaTweetyInfo)
{
	return FALSE;
}


/* --------------------------------------------------------------------------------
   Function : MechaTweetyCrateAttackCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyCrateAttackCallback(MECHATWEETYINFO *mechaTweetyInfo)
{
	ACTORINSTANCE	*tempInstance;
	TBVector		tempVector, pos, scale;

	//check for throw
	if(mechaTweetyInfo->flags & MTWEETY_THROWCRATE)
	{
		GetAttachedObjectsWorldPosition(mechaTweetyInfo->actorInstance, mechaTweetyInfo->attachHandle, pos);
		SETVECTOR(scale, 1.0f, 1.0f, 1.0f, 1.0f);

		tempInstance = CreateActorInstance(&map.characterInstances, "extras\\ninjathrowingcratetester.obe", "crate", 
						pos, mechaTweetyInfo->actorInstance->actorInstance.scale, bIdentityQuaternion, 0);
		if(tempInstance)
		{
			CreateTazBossItemInstance(tempInstance, TAZBOSSITEM_TWEETYCRATE);
			tempInstance->tazBossItemInfo->flags |= BOSSITEMFLAG_USECLOCK;
			//set timer for destrucion of crate
			tempInstance->tazBossItemInfo->clock = 1.5f;
			tempInstance->actorStatus = (ACTOR_STATUS*)ZALLOC(sizeof(ACTOR_STATUS));
			ASSERTM(tempInstance->actorStatus, "out of memory");
			InitActorStatus(tempInstance->actorStatus);
			tempInstance->actorStatus->currentState = STATE_SPIN;
			CreateActorInstanceBehaviour(tempInstance);
			SetCollisionCallback(tempInstance, RespondToMTweetyCrateCollision);
			// TP: this should work now.
			SetPreCollisionCallback(tempInstance, PreRespondToMTweetyCrateCollision);
			AddActorToCollisionSphereCache(&collisionCache, tempInstance);
			SETVECTOR(tempVector, 0.0f, METERTOUNIT(0.5f), 0.0f, 1.0f);
			SetActorCollSphere(tempInstance->actorBehaviour, tempVector, METERTOUNIT(0.5f));
			CalculateProjectileInitialVelocity(tempVector, pos, mechaTweetyInfo->targetPos, RAD(1.0f));
			SetActorVelocity(tempInstance->actorBehaviour, tempVector);
		}

		if(mechaTweetyInfo->attachHandle != -1)
		{
			RemoveObjectFromCharacter(mechaTweetyInfo->actorInstance, mechaTweetyInfo->attachHandle);
			mechaTweetyInfo->flags &= ~MTWEETY_GOTCRATE;
		}
		mechaTweetyInfo->flags &= ~MTWEETY_THROWCRATE;
	}
	if(!CheckIfPlayingAnimation(mechaTweetyInfo->actorInstance, "attack1"))
	{
		return TRUE;
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : MechaTweetyElectricityAttackCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :                                                  
*/

int32 MechaTweetyElectricityAttackCallback(MECHATWEETYINFO *mechaTweetyInfo)
{
	if(!CheckIfPlayingAnimation(mechaTweetyInfo->actorInstance, "attack2")&&!CheckIfPlayingAnimation(mechaTweetyInfo->actorInstance, "attack3"))
	{
		ResetElectricityAttack(mechaTweetyInfo->leftElectricAttack);
		ResetElectricityAttack(mechaTweetyInfo->rightElectricAttack);
		return TRUE;
	}

	if(mechaTweetyInfo->leftElectricAttack->state)
	{
		UpdateElectricityAttack(mechaTweetyInfo->actorInstance, mechaTweetyInfo->leftElectricAttack);
	}
	if(mechaTweetyInfo->rightElectricAttack->state)
	{
		UpdateElectricityAttack(mechaTweetyInfo->actorInstance, mechaTweetyInfo->rightElectricAttack);
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : MechaTweetyStageOneMoveCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyStageOneMoveCallback(MECHATWEETYINFO *mechaTweetyInfo)
{
	TBVector		distToTarget, tempVec;
	TBQuaternion	orientation;

	bmVectorSub(distToTarget, mechaTweetyInfo->actorInstance->actorInstance.position, mechaTweetyInfo->targetPos);
	distToTarget[1] = 0.0f;
	if(bmVectorLen(distToTarget) < (2.5f*fTime*bmVectorLen(mechaTweetyInfo->actorInstance->actorBehaviour->physics.velocity)))
	{
		mechaTweetyInfo->actorInstance->actorInstance.position[0] = mechaTweetyInfo->targetPos[0];
		mechaTweetyInfo->actorInstance->actorInstance.position[2] = mechaTweetyInfo->targetPos[2];
		return TRUE;
	}
	//calculate target orientation
	bmVectorSub(tempVec, mechaTweetyInfo->targetPos, mechaTweetyInfo->actorInstance->actorInstance.position);
	tempVec[1] = 0.0f;
	bmDirectionToQuat(mechaTweetyInfo->targetOrientation, tempVec, 0.0f);
	
	//turn tweety
	if((mechaTweetyInfo->turnAmount += 0.25f*fTime) > 1.0f) mechaTweetyInfo->turnAmount = 1.0f;
	bmQuatSlerp(orientation, mechaTweetyInfo->actorInstance->actorInstance.orientation, 
							mechaTweetyInfo->targetOrientation, mechaTweetyInfo->turnAmount);
	utilNormaliseQuaternion(orientation);
	SetActorOrientation(mechaTweetyInfo->actorInstance, orientation);
	
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : MechaTweetyDefendCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyDefendCallback(MECHATWEETYINFO *mechaTweetyInfo)
{
	if((mechaTweetyInfo->clock -= fTime) < 0.0f)
	{
		return TRUE;
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : MechaTweetyTurnCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyTurnCallback(MECHATWEETYINFO *mechaTweetyInfo)
{
	TBQuaternion	orientation;

	utilNormaliseQuaternion(mechaTweetyInfo->actorInstance->actorInstance.orientation);


	if(QuatSlerpAtFixedSpeed(orientation,mechaTweetyInfo->actorInstance->actorInstance.orientation,
								mechaTweetyInfo->targetOrientation, RAD(180.0f)*fTime))
	{
		SetActorOrientation(mechaTweetyInfo->actorInstance, mechaTweetyInfo->targetOrientation);
		return TRUE;
	}
	else
	{
		//not finished
		SetActorOrientation(mechaTweetyInfo->actorInstance, orientation);
		return FALSE;
	}
}

/* --------------------------------------------------------------------------------
   Function : MechaTweetyGetCrateCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyGetCrateCallback(MECHATWEETYINFO *mechaTweetyInfo)
{
	if(!CheckIfPlayingAnimation(mechaTweetyInfo->actorInstance, "grabcrate1")&&!CheckIfPlayingAnimation(mechaTweetyInfo->actorInstance, "grabcrate2"))
	{
		return TRUE;
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : MechaTweetyStunnedCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyStunnedCallback(MECHATWEETYINFO *mechaTweetyInfo)
{
	TBActorAnimSegment		*animSegment;

	animSegment = baFindAnimSegmentByCRC(&mechaTweetyInfo->actorInstance->actorInstance, mechaTweetyInfo->animCRC);

	if(!CheckIfPlayingAnimationPointedTo(&mechaTweetyInfo->actorInstance->actorInstance, animSegment))
	{
		mechaTweetyInfo->hitsToNextStage--;
		return TRUE;
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : MechaTweetySetHomePosCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetySetHomePosCallback(MECHATWEETYINFO *mechaTweetyInfo)
{
	bmVectorCopy(mechaTweetyInfo->homePos, mechaTweetyInfo->actorInstance->actorInstance.position);
	bmQuatCopy(mechaTweetyInfo->homeOrientation, mechaTweetyInfo->actorInstance->actorInstance.orientation);
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : MechaTweetyNextCameraCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyNextCameraCallback(MECHATWEETYINFO *mechaTweetyInfo)
{
	mTweetyGenericInfo->destinationCamera++;
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : MechaTweetyTransportCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyTransportCallback(MECHATWEETYINFO *mechaTweetyInfo)
{
//	switch(mechaTweetyInfo->aiSubState)
//	{
//	case TRANSPORTSTATE_APPEAR:
//		if((mechaTweetyInfo->actorInstance->alpha = UpdateTransientTransporterEffectForTweety(mechaTweetyInfo, 256, mechaTweetyInfo->actorInstance->alpha)) >=256)
//		{
//			mechaTweetyInfo->actorInstance->alpha = 256;
//			mechaTweetyInfo->aiSubState = TRANSPORTSTATE_FINISHED;
//			SETVECTOR(mechaTweetyInfo->actorInstance->actorInstance.scale, mechaTweetyInfo->scaleInfo.scale, mechaTweetyInfo->scaleInfo.scale, mechaTweetyInfo->scaleInfo.scale, 1.0f);
//			return TRUE;
//		}
//		return FALSE;
//	case TRANSPORTSTATE_DISAPPEAR:
//		if((mechaTweetyInfo->actorInstance->alpha = UpdateTransientTransporterEffectForTweety(mechaTweetyInfo, 0, mechaTweetyInfo->actorInstance->alpha)) <= 0)
//		{
//			bmVectorCopy(mechaTweetyInfo->tempVector, mechaTweetyInfo->targetPos);
//			//set position
//			mechaTweetyInfo->actorInstance->actorInstance.position[0] = mechaTweetyInfo->targetPos[0];
///			mechaTweetyInfo->actorInstance->actorInstance.position[2] = mechaTweetyInfo->targetPos[2];
//			mechaTweetyInfo->actorInstance->actorInstance.position[1] += METERTOUNIT(0.3f);
///			mechaTweetyInfo->actorInstance->alpha = 0;
//			SetActorOrientation(mechaTweetyInfo->actorInstance, mechaTweetyInfo->targetOrientation);
//			mechaTweetyInfo->aiSubState = TRANSPORTSTATE_APPEAR;
//		}
//		return FALSE;
//	}
//	return FALSE;
	if(mechaTweetyInfo->actorInstance->characterInfo->transportInfo) return FALSE;
	else return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : MechaTweetyPlayAnimCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyPlayAnimCallback(MECHATWEETYINFO *mechaTweetyInfo)
{
	if(mechaTweetyInfo->clock)
	{
		if((mechaTweetyInfo->clock -= fTime) < 0.0f)
		{
			return TRUE;
		}
	}
	else
	{
		if(!CheckIfPlayingAnimation(mechaTweetyInfo->actorInstance, NULL))
		{
			return TRUE;
		}
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : MechaTweetyPlayAnimCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyReturnTrueCallback(MECHATWEETYINFO *mechaTweetyInfo)
{
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : MechaTweetyPlayAnimCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyMagnetChargeCallback(MECHATWEETYINFO *mechaTweetyInfo)
{
	if(!CheckIfPlayingAnimation(mechaTweetyInfo->actorInstance, "magnetcharge"))
	{
		return TRUE;
	}
	
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : MechaTweetyPlayAnimCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyFlyCallback(MECHATWEETYINFO *mechaTweetyInfo)
{
	TBVector		distToTarget, tempVec, position, delta, offset;
	TBQuaternion	orientation, tempQuat;
	TBMatrix		matrix;
	float			size;
	int				i, colour[3];

	switch(mechaTweetyInfo->aiSubState)
	{
	case TWEETYFLY_CRUISE:
		bmVectorSub(distToTarget, mechaTweetyInfo->actorInstance->actorInstance.position, mechaTweetyInfo->targetPos);
		distToTarget[1] = 0.0f;
		if(bmVectorLen(distToTarget) < (2.5f*fTime*bmVectorLen(mechaTweetyInfo->actorInstance->actorBehaviour->physics.velocity)))
		{
			mechaTweetyInfo->actorInstance->actorInstance.position[0] = mechaTweetyInfo->targetPos[0];
			mechaTweetyInfo->actorInstance->actorInstance.position[2] = mechaTweetyInfo->targetPos[2];
			FlushAnimationQueue(mechaTweetyInfo->actorInstance);
			PlayAnimationByName(mechaTweetyInfo->actorInstance, "boostland", 1.0f, 0, 0, 0.1f, REACT);
			StopChannel(&mechaTweetyInfo->flySoundHandle);
			mechaTweetyInfo->aiSubState = TWEETYFLY_LAND;
			return FALSE;
		}
		//calculate target orientation
		bmVectorSub(tempVec, mechaTweetyInfo->targetPos, mechaTweetyInfo->actorInstance->actorInstance.position);
		tempVec[1] = 0.0f;
		bmDirectionToQuat(mechaTweetyInfo->targetOrientation, tempVec, 0.0f);
		
		//turn tweety
		if((mechaTweetyInfo->turnAmount += 0.25f*fTime) > 1.0f) mechaTweetyInfo->turnAmount = 1.0f;
		bmQuatSlerp(orientation, mechaTweetyInfo->actorInstance->actorInstance.orientation, 
								mechaTweetyInfo->targetOrientation, mechaTweetyInfo->turnAmount);
		utilNormaliseQuaternion(orientation);
		SetActorOrientation(mechaTweetyInfo->actorInstance, orientation);

		SETVECTOR(tempVec, UNITTOMETER((float)((bmRand()%(CHILLIPEPPER_FIRE_DISP*2))-CHILLIPEPPER_FIRE_DISP)), 
								UNITTOMETER((float)((bmRand()%(CHILLIPEPPER_FIRE_DISP)))), 
								(-1.0f-UNITTOMETER((float)((bmRand()%(CHILLIPEPPER_FIRE_DISP*2))-CHILLIPEPPER_FIRE_DISP))), 1.0f);

		// CMD: rotate velocity to the correct position
		SETVECTOR(position, 1.0f, 0.0f, 0.0f, RAD(60.0f));
		bmRotationToQuat(orientation, position);
		bmQuatMultiply(tempQuat, orientation, mechaTweetyInfo->actorInstance->actorInstance.orientation);
		bmQuatToMatrix(matrix, tempQuat);
		bmMatMultiplyVector(matrix, tempVec);
		bmVectorScaleToLength(tempVec, tempVec, METERTOUNIT(6.0f));

		bmVectorSet(delta, 0.0f, 0.0f, 0.0f, 1.0f);
		bmVectorSet(offset, 0.0f, 0.0f, -1.0f, 1.0f);
		bmMatMultiplyVector(matrix, offset);
		bmVectorScaleToLength(offset, offset, METERTOUNIT(6.0f)*0.25f*fTime);

		// CMD: create flames from mecha tweety's feet
		for(i=0;i<4;i++)
		{
			//need small displacement to seperate the sprites early in their life time
			SETVECTOR(distToTarget,0.0f,0.0f,-((float)(bmRand()%CHILLIPEPPER_FIRE_RANDDISP)),1.0f);

			if((size = CHILLIPEPPER_FIRE_MAXSIZE*distToTarget[2]/(int)CHILLIPEPPER_FIRE_RANDDISP) < CHILLIPEPPER_FIRE_MINSIZE)
			{
				size = CHILLIPEPPER_FIRE_MINSIZE;
			}

			bmMatMultiplyVector(matrix, distToTarget);

			if(i%2)
			{
				baGetNodesWorldPosition(&mechaTweetyInfo->actorInstance->actorInstance, baFindNode(mechaTweetyInfo->actorInstance->actorInstance.rootNodeInstance, "legl_calf"),position);
			}
			else
			{
				baGetNodesWorldPosition(&mechaTweetyInfo->actorInstance->actorInstance, baFindNode(mechaTweetyInfo->actorInstance->actorInstance.rootNodeInstance, "legr_calf"),position);
			}
			bmVectorAdd(position, position, distToTarget);

			if(bmRand()%2)
			{
				colour[0]=255;colour[1]=38;colour[2]=10;
			}
			else
			{
				colour[0]=255;colour[1]=186;colour[2]=10;
			}
			bmVectorAdd(position, position, delta);
			float	tempFloat = CHILLIPEPPER_FIRE_LIFE;
			float	rand = UtilGenerateRandomNumber(RAD(360.0f), 0.0f);
			baCreateExtra(chilliFireSystem,position,tempVec,textureChilliFire,&tempFloat,&rand,&size,colour[0],colour[1],colour[2],140,DEBRISFLAG_CHILLIFIRE|DEBRISFLAG_INCEDENTALFIRE);
			bmVectorAdd(delta, delta, offset);
		}
		return FALSE;
	case TWEETYFLY_LAND:
		if(!CheckIfPlayingAnimation(mechaTweetyInfo->actorInstance, "boostland"))
		{
			mechaTweetyInfo->genericRumbleHandle = FlushRumbleQueueMember(&controller1, mechaTweetyInfo->genericRumbleHandle);
			return TRUE;
		}
		return FALSE;
	default:
		return FALSE;
	}
}

/* --------------------------------------------------------------------------------
   Function : MechaTweetyPlayAnimCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyFireRocketsCallback(MECHATWEETYINFO *mechaTweetyInfo)
{
	TBVector	zeroVec;

	switch(mechaTweetyInfo->aiSubState)
	{
	case TWEETYROCKETS_PREPARE:
		if(!CheckIfPlayingAnimation(mechaTweetyInfo->actorInstance, "preparerockets"))
		{
			SETVECTOR(zeroVec, 0.0f, 0.0f, 0.0f, 1.0f);
			FireSeekingRocketFromActorNode(mechaTweetyInfo->actorInstance, gameStatus.player[0].actorInstance, "arml_lower", zeroVec);
			FireSeekingRocketFromActorNode(mechaTweetyInfo->actorInstance, gameStatus.player[0].actorInstance, "armr_lower", zeroVec);
			FlushAnimationQueue(mechaTweetyInfo->actorInstance);
			PlayAnimationByName(mechaTweetyInfo->actorInstance, "firerockets", 1.0f, 0, 0, 0.1f, REACT);
			mechaTweetyInfo->rocketsRumbleHandle = RequestRumbleEffect(&controller1, 200, 1.5f, 1.0f, 2.5f, 0.0f, WAVEFORM_RANDOM);
			mechaTweetyInfo->aiSubState = TWEETYROCKETS_FIRE;
		}
		return FALSE;
	case TWEETYROCKETS_FIRE:
		if(!CheckIfPlayingAnimation(mechaTweetyInfo->actorInstance, "firerockets"))
		{
			return TRUE;
		}
		return FALSE;
	default:
		return FALSE;
	}
}

/* --------------------------------------------------------------------------------
   Function : MechaTweetyOpenDoorCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyOpenDoorCallback(MECHATWEETYINFO *mechaTweetyInfo)
{
	if(!CheckIfPlayingAnimation(mechaTweetyInfo->actorInstance, "opendoor"))
	{
		return TRUE;
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : MechaTweetyScaleCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 MechaTweetyScaleCallback(MECHATWEETYINFO *mechaTweetyInfo)
{
	// CMD: Scale Tweety Up in stages
	// CMD: Scale just past the target scale for each stage, and quickly shrink

	if((mechaTweetyInfo->scaleInfo.scale -= MTWEETY_SCALETAPERRATE*fTime) < mechaTweetyInfo->scaleInfo.targetScale)
	{
		// CMD: have we reached the final scale
		if(mechaTweetyInfo->scaleInfo.scaleStage++ == MTWEETY_NUMSCALES)
		{
			mechaTweetyInfo->scaleInfo.scale = mechaTweetyInfo->scaleInfo.finalScale;
			SETVECTOR(mechaTweetyInfo->actorInstance->actorInstance.scale, mechaTweetyInfo->scaleInfo.scale, mechaTweetyInfo->scaleInfo.scale, mechaTweetyInfo->scaleInfo.scale, 1.0f);
			return TRUE;
		}
		else
		{
			// CMD: calculate new target scale
			mechaTweetyInfo->scaleInfo.targetScale = 
				(((mechaTweetyInfo->scaleInfo.finalScale - mechaTweetyInfo->scaleInfo.initialScale)/MTWEETY_NUMSCALES)*mechaTweetyInfo->scaleInfo.scaleStage)+mechaTweetyInfo->scaleInfo.initialScale;

			// CMD: increase scale beyond the target
			mechaTweetyInfo->scaleInfo.scale = mechaTweetyInfo->scaleInfo.targetScale + MTWEETY_SCALETAPER;
			SETVECTOR(mechaTweetyInfo->actorInstance->actorInstance.scale, mechaTweetyInfo->scaleInfo.scale, mechaTweetyInfo->scaleInfo.scale, mechaTweetyInfo->scaleInfo.scale, 1.0f);
			return FALSE;
		}
	}
	SETVECTOR(mechaTweetyInfo->actorInstance->actorInstance.scale, mechaTweetyInfo->scaleInfo.scale, mechaTweetyInfo->scaleInfo.scale, mechaTweetyInfo->scaleInfo.scale, 1.0f);
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : MechaTweetyStateChangeCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int MechaTweetyStateChangeCallback(MECHATWEETYINFO *mechaTweetyInfo, EMTweetyAIState state, EMTweetyAIState lastState)
{
	ACTORINSTANCE		*tweety = mechaTweetyInfo->actorInstance;
	ACTORINSTANCE		*actorInstance;

	switch(lastState)
	{
	case MTWEETYAI_ATTACKELECLEFT:
		mechaTweetyInfo->leftElectricAttack->state = FALSE;
		break;
	case MTWEETYAI_ATTACKELECRIGHT:
		mechaTweetyInfo->rightElectricAttack->state = FALSE;
		break;
	case MTWEETYAI_GETCRATE:
		mechaTweetyInfo->flags &= ~MTWEETY_HIGH;
		mechaTweetyInfo->flags &= ~MTWEETY_LOW;
		break;
	case MTWEETYAI_TRANSPORT:
//		EnableActorCartoonRenderMode(&tweety->actorInstance);
		break;
	case MTWEETYAI_MAGNETCHARGE:
		mechaTweetyInfo->flags &= ~MTWEETY_MAGNETCHARGE;
		StopChannel(&mechaTweetyInfo->magnetSoundHandle);
		break;
	case MTWEETYAI_FLYTOTARGET:
//		tweety->flags &= ~ACTORFLAG_NOCOLLISION;
//		tweety->actorBehaviour->physics.flags &= ~PHYSFLAG_NOGRAVITY;
		break;
	}
	switch(state)
	{
	case MTWEETYAI_END:
		FlushAnimationQueue(tweety);
		mechaTweetyInfo->aiCallback = NULL;
		break;
	case MTWEETYAI_SETHOMEPOS:
		mechaTweetyInfo->aiCallback = MechaTweetySetHomePosCallback;
		break;
	case MTWEETYAI_NEXTCAMERA:
		mechaTweetyInfo->aiCallback = MechaTweetyNextCameraCallback;
		break;
	case MTWEETYAI_WALKTOTARGET:
		FlushAnimationQueue(tweety);
		PlayAnimationByName(tweety, "walk1", 1.0f, 1, 0, 0.1f, MOVE);
		mechaTweetyInfo->aiCallback = MechaTweetyStageOneMoveCallback;
		break;
	case MTWEETYAI_RUNTOTARGET:
		FlushAnimationQueue(tweety);
		PlayAnimationByName(tweety, "run1", 1.0f, 1, 0, 0.1f, MOVE);
		mechaTweetyInfo->aiCallback = MechaTweetyStageOneMoveCallback;
		break;
	case MTWEETYAI_TURNLEFT:
		FlushAnimationQueue(tweety);
		if(mechaTweetyInfo->flags & MTWEETY_GOTCRATE) PlayAnimationByName(tweety, "turnleftcrate", 1.0f, 1, 0, 0.1f, MOVE);
		else PlayAnimationByName(tweety, "turnleft", 1.0f, 1, 0, 0.1f, MOVE);
		mechaTweetyInfo->aiCallback = MechaTweetyTurnCallback;
		break;
	case MTWEETYAI_TURNRIGHT:
		FlushAnimationQueue(tweety);
		if(mechaTweetyInfo->flags & MTWEETY_GOTCRATE) PlayAnimationByName(tweety, "turnrightcrate", 1.0f, 1, 0, 0.1f, MOVE);
		else PlayAnimationByName(tweety, "turnright", 1.0f, 1, 0, 0.1f, MOVE);
		mechaTweetyInfo->aiCallback = MechaTweetyTurnCallback;
		break;
	case MTWEETYAI_IDLE:
		FlushAnimationQueue(tweety);
		PlayAnimationByName(tweety, "idle1", 1.0f, 1, 0, 0.1f, IDLE);
		mechaTweetyInfo->aiCallback = MechaTweetyIdleCallback;
		break;
	case MTWEETYAI_DEFEND:
		FlushAnimationQueue(tweety);
		PlayAnimationByName(tweety, "defend", 1.0f, 1, 0, 0.1f, REACT);
		mechaTweetyInfo->aiCallback = MechaTweetyDefendCallback;
		break;
	case MTWEETYAI_STUNNED:
		FlushAnimationQueue(tweety);
		PlayAnimationByCRCOfName(tweety, mechaTweetyInfo->animCRC, 1.0f, 0, 0, 0.1f, REACT);
		mechaTweetyInfo->aiCallback = MechaTweetyStunnedCallback;
		break;
	case MTWEETYAI_ATTACKELECLEFT:
		FlushAnimationQueue(tweety);
		PlayAnimationByName(tweety, "attack3", 1.0f, 0, 0, 0.1f, REACT);
		mechaTweetyInfo->aiCallback = MechaTweetyElectricityAttackCallback;
		break;
	case MTWEETYAI_ATTACKELECRIGHT:
		FlushAnimationQueue(tweety);
		PlayAnimationByName(tweety, "attack2", 1.0f, 0, 0, 0.1f, REACT);
		mechaTweetyInfo->aiCallback = MechaTweetyElectricityAttackCallback;
		break;
	case MTWEETYAI_GETCRATE:
		if(mechaTweetyInfo->flags & MTWEETY_HIGH)
		{
			FlushAnimationQueue(tweety);
			PlayAnimationByName(tweety, "grabcrate1", 1.0f, 0, 0,0.1f, REACT);
		}
		if(mechaTweetyInfo->flags & MTWEETY_LOW)
		{
			FlushAnimationQueue(tweety);
			PlayAnimationByName(tweety, "grabcrate2", 1.0f, 0, 0,0.1f, REACT);
		}
		mechaTweetyInfo->aiCallback = MechaTweetyGetCrateCallback;
		break;
	case MTWEETYAI_ATTACKCRATE:
		FlushAnimationQueue(tweety);
		PlayAnimationByName(tweety, "attack1", 1.0f, 0, 0, 0.1f, REACT);
		mechaTweetyInfo->aiCallback = MechaTweetyCrateAttackCallback;
		break;
	case MTWEETYAI_TRANSPORT:
	case MTWEETYAI_TRANSPORTOPP:
		FlushAnimationQueue(tweety);
		PlayAnimationByName(tweety, "defend", 1.0f, 1, 0, 0.1f, REACT);
		mechaTweetyInfo->aiCallback = MechaTweetyTransportCallback;
//		DisableActorCartoonRenderMode(&tweety->actorInstance);
		break;
	case MTWEETYAI_ANIM:
		FlushAnimationQueue(tweety);
		if(mechaTweetyInfo->clock)
		{
			PlayAnimationByCRCOfName(tweety, mechaTweetyInfo->animCRC, 1.0f, 1, 0, 0.1f, REACT);
		}
		else
		{
			PlayAnimationByCRCOfName(tweety, mechaTweetyInfo->animCRC, 1.0f, 0, 0, 0.1f, REACT);
		}
		mechaTweetyInfo->aiCallback = MechaTweetyPlayAnimCallback;
		break;
	case MTWEETYAI_COLLISIONOFF:
	case MTWEETYAI_COLLISIONON:
	case MTWEETYAI_RAISEMAGNET:
	case MTWEETYAI_LOWERMAGNET:
	case MTWEETYAI_LOWERSHIELD:
	case MTWEETYAI_RAISESHIELD:
	case MTWEETYAI_DROPSHIELDFLICKER:
		mechaTweetyInfo->aiCallback = MechaTweetyReturnTrueCallback;
		break;
	case MTWEETYAI_MAGNETCHARGE:
		FlushAnimationQueue(tweety);
		PlayAnimationByName(tweety, "magnetcharge", 1.0f, 0, 0, 0.1f, REACT);
		mechaTweetyInfo->aiCallback = MechaTweetyMagnetChargeCallback;
		break;
	case MTWEETYAI_FLYTOTARGET:
		FlushAnimationQueue(tweety);
		PlayAnimationByName(tweety, "taketotheskies", 1.0f, 0, 0, 0.1f, REACT);
		PlayAnimationByName(tweety, "rocketboost", 1.0f, 1, 1, 0.1f, IDLE);
		mechaTweetyInfo->aiCallback = MechaTweetyFlyCallback;
//		tweety->actorBehaviour->physics.flags |= PHYSFLAG_NOGRAVITY;
//		tweety->flags |= ACTORFLAG_NOCOLLISION;
		break;
	case MTWEETYAI_ROCKETATTACK:
		FlushAnimationQueue(tweety);
		PlayAnimationByName(tweety, "preparerockets", 1.0f, 0, 0, 0.1f, REACT);
		mechaTweetyInfo->aiCallback = MechaTweetyFireRocketsCallback;
		break;
	case MTWEETYAI_OPENDOOR:
		FlushAnimationQueue(tweety);
		PlayAnimationByCRCOfName(tweety, mechaTweetyInfo->animCRC, 1.0f, 0, 0, 0.1f, REACT);
		// CMD: find door and open it
		for(actorInstance = map.animatedInstances.head.next;actorInstance != &map.animatedInstances.head;actorInstance = actorInstance->next)
		{
			if(actorInstance->special == SPECIALTYPE_TAZBOSSITEM)
			{
				if(actorInstance->tazBossItemInfo->type == TAZBOSSITEM_DOOR)
				{
					if(actorInstance->tazBossItemInfo->number == (uint32)bmFloatToInt(mechaTweetyInfo->clock))
					{
						FlushAnimationQueue(actorInstance);
						PlayAnimationByName(actorInstance, "open doors", 1.0f, 0, 0, 0.1f, REACT);
					}
				}
			}
		}
		mechaTweetyInfo->aiCallback = MechaTweetyOpenDoorCallback;
		break;
	case MTWEETYAI_SCALE:
		FlushAnimationQueue(tweety);
		PlayAnimationByCRCOfName(tweety, mechaTweetyInfo->animCRC, 1.0f, 1, 0, 0.1f, REACT);
		mechaTweetyInfo->aiCallback = MechaTweetyScaleCallback;
		break;
	}
	mechaTweetyInfo->turnAmount = 0.0f;
	mechaTweetyInfo->aiLastState = state;
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : UpdateMechaTweety
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

void UpdateMechaTweety(ACTORINSTANCE *actorInstance)
{
	MECHATWEETYINFO		*ptr = actorInstance->mechaTweetyInfo;

	if(ptr)
	{
		if(ptr->aiState != ptr->aiLastState)
		{
			ptr->stateChangeCallback(ptr, ptr->aiState, ptr->aiLastState);
		}
		else
		{
			if(ptr->aiCallback)
			{
				if(ptr->aiCallback(ptr))
				{
					ptr->scriptIndex++;
				}
			}
		}
		if(ptr->shieldInfo)
		{
			UpdateRedShieldArroundActor(actorInstance, ptr->shieldInfo);
		}
		if(ptr->magnetStreak)
		{
			UpdateMagnetStreaks(ptr);
		}
		// CMD: safety check for tweety falling below mesh
		if(actorInstance->actorInstance.position[1] < worldBoundingBox.ymin)
		{
			if(ptr->aiState != MTWEETYAI_TRANSPORT)
			{
				RequestNewMechaTweetySequence(actorInstance, resetToHome);
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateMechaTweety
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

void UpdateMechaTweetyBossGameLogic(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE	*instance;
	ACTORINSTANCE	*ptr;
	MECHATWEETYINFO	*info;
	MTWEETY_EVENT	*event;
	TBVector		left, right, tempVector;
	TBMatrix		tempMat;
	TBQuaternion	tempQuat, safetyQuat;
#ifdef _TAZDEBUG
	TAZBOSSITEMINFO		*bossItem;
#endif

	for(ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if(ptr->special != SPECIALTYPE_MECHATWEETY) continue;

		info = ptr->mechaTweetyInfo;

		if(info->currentScript)
		{
			if(info->scriptIndex != info->scriptIndexLastFrame)
			{
				info->scriptIndexLastFrame = info->scriptIndex;

				event = &info->currentScript[info->scriptIndex];

				switch(event->action)
				{
				case MTWEETYAI_LOOP:
					info->scriptIndex = 0;
					info->scriptIndexLastFrame = -1;
					info->aiState = info->currentScript[0].action;
					break;
				case MTWEETYAI_END:
					info->currentScript = NULL;
					info->scriptIndex = 0;
					info->scriptIndexLastFrame = -1;
					info->aiState = MTWEETYAI_END;
					break;
				case MTWEETYAI_PLAYSAMPLE:
					if (characterSounds.tweety == -1)
					{
						if (!strcmp(event->targetInstance, "damage"))
						{
							if (bmRand()%2)
								CallbackWhenSampleFinished((characterSounds.tweety=PlaySample("tweety_hurt_1.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.tweety);	
							else
								CallbackWhenSampleFinished((characterSounds.tweety=PlaySample("tweety_hurt_2.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.tweety);	
						}
						else if (!strcmp(event->targetInstance, "celebrate"))
						{
							switch(bmRand()%3)
							{
							case 0:	CallbackWhenSampleFinished((characterSounds.tweety=PlaySample("tweetydamage_1.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.tweety);	break;
							case 1:	CallbackWhenSampleFinished((characterSounds.tweety=PlaySample("tweetydamage_2.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.tweety);	break;
							case 2:	CallbackWhenSampleFinished((characterSounds.tweety=PlaySample("tweetydamage_3.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.tweety);	break;
							}
						}
						else
							CallbackWhenSampleFinished((characterSounds.tweety=PlaySample(event->targetInstance,255)),CharacterSoundsFinishedCallback,&characterSounds.tweety);	
					}
					break;
				case MTWEETYAI_STOPSAMPLE:
					StopChannel(&characterSounds.tweety);
					break;
				case MTWEETYAI_IDLE:
					info->aiState = MTWEETYAI_IDLE;
					break;
				case MTWEETYAI_RAISEMAGNET:
					for(instance = map.animatedInstances.head.next;instance != &map.animatedInstances.head;instance = instance->next)
					{
						if((instance->special == SPECIALTYPE_TAZBOSSITEM)&&(instance->tazBossItemInfo->type == TAZBOSSITEM_MAGNET))
						{
#ifdef _TAZDEBUG
	bossItem = instance->tazBossItemInfo;
#endif
							instance->tazBossItemInfo->flags |= BOSSITEMFLAG_RAISEMAGNET;
							instance->tazBossItemInfo->flags &= ~BOSSITEMFLAG_LOWERMAGNET;
						}
					}
					info->aiState = MTWEETYAI_RAISEMAGNET;
					break;
				case MTWEETYAI_LOWERMAGNET:
					for(instance = map.animatedInstances.head.next;instance != &map.animatedInstances.head;instance = instance->next)
					{
						if((instance->special == SPECIALTYPE_TAZBOSSITEM)&&(instance->tazBossItemInfo->type == TAZBOSSITEM_MAGNET))
						{
							instance->tazBossItemInfo->flags |= BOSSITEMFLAG_LOWERMAGNET;
							instance->tazBossItemInfo->flags &= ~BOSSITEMFLAG_RAISEMAGNET;
						}
					}
					info->aiState = MTWEETYAI_LOWERMAGNET;
					break;
				case MTWEETYAI_LOWERSHIELD:
					RequestRedShieldStateChange(info->shieldInfo, REDSHIELD_INACTIVE);
					info->aiState = MTWEETYAI_LOWERSHIELD;
					break;
				case MTWEETYAI_RAISESHIELD:
					RequestRedShieldStateChange(info->shieldInfo, REDSHIELD_ACTIVE);
					info->aiState = MTWEETYAI_RAISESHIELD;
					break;
				case MTWEETYAI_SETHOMEPOS:
					info->aiState = MTWEETYAI_SETHOMEPOS;
					break;
				case MTWEETYAI_NEXTCAMERA:
					mTweetyGenericInfo->clock = 0.0f;
					info->aiState = MTWEETYAI_NEXTCAMERA;
					break;
				case MTWEETYAI_WALKTOTARGET:
					if(strcmpi(event->targetInstance, "home") == 0)
					{
						bmVectorCopy(info->targetPos, info->homePos);
					}
					else
					{
						instance = FindActorInstanceInInstanceList(event->targetInstance, 0, &map.invisibleInstances);
						if(!instance) instance = FindActorInstanceInInstanceList(event->targetInstance, 0, &map.levelInstances);
						ASSERTM(instance, "could not find 'move to' instance");
						bmVectorCopy(info->targetPos, instance->actorInstance.position);
					}
					info->aiState = MTWEETYAI_WALKTOTARGET;
					break;
				case MTWEETYAI_RUNTOTARGET:
					if(strcmpi(event->targetInstance, "home") == 0)
					{
						bmVectorCopy(info->targetPos, info->homePos);
					}
					else
					{
						instance = FindActorInstanceInInstanceList(event->targetInstance, 0, &map.invisibleInstances);
						if(!instance) instance = FindActorInstanceInInstanceList(event->targetInstance, 0, &map.levelInstances);
						ASSERTM(instance, "could not find 'move to' instance");
						bmVectorCopy(info->targetPos, instance->actorInstance.position);
					}
					info->aiState = MTWEETYAI_RUNTOTARGET;
					break;
				case MTWEETYAI_TURN:
					if(strcmpi(event->targetInstance, "home") == 0)
					{
						bmQuatCopy(info->targetOrientation, info->homeOrientation);
					}
					else
					if(strcmpi(event->targetInstance, "facetaz") == 0)
					{
						bmVectorSub(right, gameStatus.player[0].actorInstance->actorInstance.position, ptr->actorInstance.position);
						bmDirectionToQuat(info->targetOrientation, right, 0.0f);
					}
					else
					{
						instance = FindActorInstanceInInstanceList(event->targetInstance, 0, &map.invisibleInstances);
						if(!instance) instance = FindActorInstanceInInstanceList(event->targetInstance, 0, &map.levelInstances);
						ASSERTM(instance, "could not find 'turn' instance");
						bmQuatCopy(info->targetOrientation, instance->actorInstance.orientation);
						//decide which way to turn
					}
					bmQuatToRotation(right, info->targetOrientation);
					bmQuatToRotation(left, ptr->actorInstance.orientation);
					if(left[3] > right[3]) info->aiState = MTWEETYAI_TURNLEFT;
					else info->aiState = MTWEETYAI_TURNRIGHT;
					break;
				case MTWEETYAI_TURNOPP:
					SETVECTOR(tempVector, 0.0f, 1.0f, 0.0f, RAD(180.0f));
					bmRotationToQuat(tempQuat, tempVector);
					if(strcmpi(event->targetInstance, "home") == 0)
					{
						bmQuatCopy(info->targetOrientation, info->homeOrientation);
					}
					else
					if(strcmpi(event->targetInstance, "facetaz") == 0)
					{
						bmVectorSub(right, gameStatus.player[0].actorInstance->actorInstance.position, ptr->actorInstance.position);
						bmDirectionToQuat(info->targetOrientation, right, 0.0f);
					}
					else
					{
						instance = FindActorInstanceInInstanceList(event->targetInstance, 0, &map.invisibleInstances);
						if(!instance) instance = FindActorInstanceInInstanceList(event->targetInstance, 0, &map.levelInstances);
						ASSERTM(instance, "could not find 'turn' instance");
						bmQuatCopy(info->targetOrientation, instance->actorInstance.orientation);
						//decide which way to turn
					}
					bmQuatMultiply(safetyQuat, tempQuat, info->targetOrientation);
					bmQuatCopy(info->targetOrientation, safetyQuat);

					bmQuatToRotation(right, info->targetOrientation);
					bmQuatToRotation(left, ptr->actorInstance.orientation);
					if(left[3] > right[3]) info->aiState = MTWEETYAI_TURNLEFT;
					else info->aiState = MTWEETYAI_TURNRIGHT;
					break;
				case MTWEETYAI_ATTACKCRATE:
					SETVECTOR(tempVector, 0.0f, 0.0f, METERTOUNIT(-1.0f), 1.0f);
					bmQuatToMatrix(tempMat, ptr->actorInstance.orientation);
					bmMatMultiplyVector(tempMat, tempVector);
					bmVectorSub(left, ptr->actorInstance.position, gameStatus.player[0].actorInstance->actorInstance.position);
					bmVectorScaleToLength(info->targetPos, tempVector, bmVectorLen(left));
					info->aiState = MTWEETYAI_ATTACKCRATE;
					break;
				case MTWEETYAI_ATTACKELECLEFT:
					info->clock = event->time;
					SETVECTOR(tempVector, 1.0f, 0.0f, 0.0f, RAD(5.0f));
					bmRotationToQuat(tempQuat, tempVector);
					bmQuatMultiply(safetyQuat, tempQuat, ptr->actorInstance.orientation);
					SETVECTOR(tempVector, 0.0f, 0.0f, METERTOUNIT(-1.0f), 1.0f);
					bmQuatToMatrix(tempMat, safetyQuat);
					bmMatMultiplyVector(tempMat, tempVector);
					bmVectorCopy(info->leftElectricAttack->offset, tempVector);
					info->aiState = MTWEETYAI_ATTACKELECLEFT;
					break;
				case MTWEETYAI_ATTACKELECRIGHT:
					info->clock = event->time;
					SETVECTOR(tempVector, 1.0f, 0.0f, 0.0f, RAD(5.0f));
					bmRotationToQuat(tempQuat, tempVector);
					bmQuatMultiply(safetyQuat, tempQuat, ptr->actorInstance.orientation);
					SETVECTOR(tempVector, 0.0f, 0.0f, METERTOUNIT(-1.0f), 1.0f);
					bmQuatToMatrix(tempMat, safetyQuat);
					bmMatMultiplyVector(tempMat, tempVector);
					bmVectorCopy(info->rightElectricAttack->offset, tempVector);
					info->aiState = MTWEETYAI_ATTACKELECRIGHT;
					break;
				case MTWEETYAI_GETCRATE:
					if(strcmpi(event->targetInstance, "high") == 0)
					{
						info->flags |= MTWEETY_HIGH;
					}
					else
					if(strcmpi(event->targetInstance, "low") == 0)
					{
						info->flags |= MTWEETY_LOW;
					}
					info->aiState = MTWEETYAI_GETCRATE;
					break;
				case MTWEETYAI_DEFEND:
					info->clock = event->time;
					info->aiState = MTWEETYAI_DEFEND;
					break;
				case MTWEETYAI_STUNNED:
					info->animCRC = bkCRC32((uchar*)event->targetInstance, strlen(event->targetInstance), 0);
					info->aiState = MTWEETYAI_STUNNED;
					break;
				case MTWEETYAI_TRANSPORT:
					if(strcmpi(event->targetInstance, "home") == 0)
					{
						bmVectorCopy(info->targetPos, info->homePos);
						bmQuatCopy(info->targetOrientation, info->homeOrientation);
					}
					else
					{
						instance = FindActorInstanceInInstanceList(event->targetInstance, 0, &map.invisibleInstances);
						if(!instance) instance = FindActorInstanceInInstanceList(event->targetInstance, 0, &map.levelInstances);
						ASSERTM(instance, "could not find 'transport to' instance");
						bmVectorCopy(info->targetPos, instance->actorInstance.position);
						bmQuatCopy(info->targetOrientation, instance->actorInstance.orientation);
					}
					info->targetPos[1] += METERTOUNIT(0.05f);
					bmVectorCopy(info->tempVector, ptr->actorInstance.position);
					info->clock = 0.0f;
					info->aiSubState = TRANSPORTSTATE_DISAPPEAR;
					info->aiState = MTWEETYAI_TRANSPORT;
					EnableCharacterTransporterMode(ptr, METERTOUNIT(3.25f), METERTOUNIT(2.0f), TRUE);
					SetCharacterTransportToPosition(ptr, info->targetPos, info->targetOrientation);
					PlaySample("teleport.wav", 255, ptr->actorInstance.position);
					ptr->characterInfo->transportInfo->xSize *= 2.0f;
					ptr->characterInfo->transportInfo->ySize *= 2.0f;
					break;
				case MTWEETYAI_TRANSPORTOPP:
					SETVECTOR(tempVector, 0.0f, 1.0f, 0.0f, RAD(180.0f));
					bmRotationToQuat(tempQuat, tempVector);  
					if(strcmpi(event->targetInstance, "home") == 0)
					{
						bmVectorCopy(info->targetPos, info->homePos);
						bmQuatCopy(info->targetOrientation, info->homeOrientation);
					}
					else
					{
						instance = FindActorInstanceInInstanceList(event->targetInstance, 0, &map.invisibleInstances);
						if(!instance) instance = FindActorInstanceInInstanceList(event->targetInstance, 0, &map.levelInstances);
						ASSERTM(instance, "could not find 'transport to' instance");
						bmVectorCopy(info->targetPos, instance->actorInstance.position);
						bmQuatCopy(info->targetOrientation, instance->actorInstance.orientation);
					}
					info->targetPos[1] += METERTOUNIT(0.05f);
					bmQuatMultiply(safetyQuat, tempQuat, info->targetOrientation);
					bmQuatCopy(info->targetOrientation, safetyQuat);
					bmVectorCopy(info->tempVector, ptr->actorInstance.position);
					info->clock = 0.0f;
					info->aiSubState = TRANSPORTSTATE_DISAPPEAR;
					info->aiState = MTWEETYAI_TRANSPORT;
					EnableCharacterTransporterMode(ptr, METERTOUNIT(3.25f), METERTOUNIT(2.0f), TRUE);
					SetCharacterTransportToPosition(ptr, info->targetPos, info->targetOrientation);
					PlaySample("teleport.wav", 255, ptr->actorInstance.position);
					ptr->characterInfo->transportInfo->xSize *= 2.0f;
					ptr->characterInfo->transportInfo->ySize *= 2.0f;
					break;
				case MTWEETYAI_FLYTOTARGET:
					if(strcmpi(event->targetInstance, "home") == 0)
					{
						bmVectorCopy(info->targetPos, info->homePos);
					}
					else
					{
						instance = FindActorInstanceInInstanceList(event->targetInstance, 0, &map.invisibleInstances);
						if(!instance) instance = FindActorInstanceInInstanceList(event->targetInstance, 0, &map.levelInstances);
						ASSERTM(instance, "could not find 'fly to' instance");
						bmVectorCopy(info->targetPos, instance->actorInstance.position);
					}
					info->aiSubState = TWEETYFLY_CRUISE;
					info->flySoundHandle = PlayTrackingSample("loop\\ftventloop.wav", 255, ptr->actorInstance.position);
					info->aiState = MTWEETYAI_FLYTOTARGET;
					break;
				case MTWEETYAI_ANIM:
					info->animCRC = bkCRC32((uchar*)event->targetInstance, strlen(event->targetInstance), 0);
					info->clock = event->time;
					info->aiState = MTWEETYAI_ANIM;
					break;
				case MTWEETYAI_COLLISIONOFF:
					ptr->flags |= ACTORFLAG_NOCOLLISION;
					ptr->actorBehaviour->physics.flags |= PHYSFLAG_NOGRAVITY;
					info->aiState = MTWEETYAI_COLLISIONOFF;
					break;
				case MTWEETYAI_COLLISIONON:
					ptr->flags &= ~ACTORFLAG_NOCOLLISION;
					ptr->actorBehaviour->physics.flags &= ~PHYSFLAG_NOGRAVITY;
					info->aiState = MTWEETYAI_COLLISIONON;
					break;
				case MTWEETYAI_MAGNETCHARGE:
					info->clock = event->time;
					info->aiState = MTWEETYAI_MAGNETCHARGE;
					break;
				case MTWEETYAI_ROCKETATTACK:
					info->aiState = MTWEETYAI_ROCKETATTACK;
					info->aiSubState = TWEETYROCKETS_PREPARE;
					break;
				case MTWEETYAI_OPENDOOR:
					info->animCRC = bkCRC32((uchar*)event->targetInstance, strlen(event->targetInstance), 0);
					info->clock = event->time;
					info->aiState = MTWEETYAI_OPENDOOR;
					break;
				case MTWEETYAI_SCALE:
					info->clock = event->time;
					info->scaleInfo.scaleStage = 0;
					info->scaleInfo.finalScale = info->scaleInfo.scale * event->time;
					info->scaleInfo.initialScale = info->scaleInfo.targetScale = info->scaleInfo.scale;
					info->animCRC = bkCRC32((uchar*)event->targetInstance, strlen(event->targetInstance), 0);
					info->aiState = MTWEETYAI_SCALE;
					break;
				case MTWEETYAI_DROPSHIELDFLICKER:
					if(info->shieldInfo) info->shieldInfo->flags |= REDSHIELD_FLICKER;
					info->aiState = MTWEETYAI_DROPSHIELDFLICKER;
					break;
				}

				// CMD: check to see if rumble requested
				if(event->rumble)
				{
					if(event->rumble->r_waveform != WAVEFORM_NONE)
					{
						info->genericRumbleHandle = RequestRumbleEffect(&controller1, event->rumble->r_amplitude, event->rumble->r_attack, 
													event->rumble->r_sustain, event->rumble->r_duration, event->rumble->r_delay, 
													event->rumble->r_waveform, event->rumble->r_frequency, event->rumble->r_freqRateOfChange, 0);
					}
					// CMD: do we want to use the buzzer?
					if(event->rumble->b_waveform != WAVEFORM_NONE)
					{
						info->genericRumbleHandle = RequestBuzzerEffect(&controller1, event->rumble->b_duration, event->rumble->b_delay, 
													event->rumble->b_waveform, event->rumble->b_frequency, event->rumble->b_freqRateOfChange, 0);
					}
				}

				if (event->sound)
				{
					if (strcmpi(event->sound, "damage") == 0)
					{
						if (bmRand()%2)
							CallbackWhenSampleFinished((characterSounds.tweety=PlaySample("tweety_hurt_1.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.tweety);	
						else
							CallbackWhenSampleFinished((characterSounds.tweety=PlaySample("tweety_hurt_2.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.tweety);	
					}
					else 
					if (strcmpi(event->sound, "celebrate") == 0)
					{
						switch(bmRand()%3)
						{
						case 0:
							CallbackWhenSampleFinished((characterSounds.tweety=PlaySample("tweetydamage_1.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.tweety);
							break;
						case 1:
							CallbackWhenSampleFinished((characterSounds.tweety=PlaySample("tweetydamage_2.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.tweety);
							break;
						case 2:
							CallbackWhenSampleFinished((characterSounds.tweety=PlaySample("tweetydamage_3.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.tweety);
							break;
						}
					}
					else
						CallbackWhenSampleFinished((characterSounds.tweety=PlaySample(event->sound,255)),CharacterSoundsFinishedCallback,&characterSounds.tweety);	
				}

				// CMD: little bug i have just discovered
				// CMD: two of the same ai events following one another can cause a problem
				// CMD: because the state change callback will not be called
				if(info->aiState == info->aiLastState) info->aiLastState = (EMTweetyAIState)-1;
			}
		}
	}
}


/* --------------------------------------------------------------------------------
   Function : DrawMechaTweetySpecial
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

void DrawMechaTweetySpecial(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE	*ptr;

	for(ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if(ptr->special != SPECIALTYPE_MECHATWEETY) continue;

		if(ptr->mechaTweetyInfo->leftElectricAttack->state)
		{
			DrawElectricityAttackInfo(ptr, ptr->mechaTweetyInfo->leftElectricAttack);
		}
		if(ptr->mechaTweetyInfo->rightElectricAttack->state)
		{
			DrawElectricityAttackInfo(ptr, ptr->mechaTweetyInfo->rightElectricAttack);
		}
		if(ptr->mechaTweetyInfo->magnetStreak)
		{
			DrawMagnetStreaks(ptr->mechaTweetyInfo);
		}
		if(ptr->mechaTweetyInfo->shieldInfo)
		{
			DrawRedShieldSpaghetti(ptr, ptr->mechaTweetyInfo->shieldInfo);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : FreeMechaTweetySpecial
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

void FreeMechaTweetySpecial(void)
{
	ACTORINSTANCE	*ptr;

	for(ptr = characterInstances.head.next;ptr != &characterInstances.head;ptr = ptr->next)
	{
		if(ptr->special != SPECIALTYPE_MECHATWEETY) continue;

		ptr->mechaTweetyInfo->leftElectricAttack = FreeElectricityAttackInfo(ptr->mechaTweetyInfo->leftElectricAttack);
		ptr->mechaTweetyInfo->rightElectricAttack = FreeElectricityAttackInfo(ptr->mechaTweetyInfo->rightElectricAttack);
		FreeMagnetStreaks(ptr->mechaTweetyInfo);
		DestroyRedShieldArroundActor(ptr->mechaTweetyInfo->shieldInfo);
	}
}

/* --------------------------------------------------------------------------------
   Function : FreeMechaTweetySpecial
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

void UpdateCrateCollisionWithMechaTweety(ACTORINSTANCE *actorInstance)
{
	MECHATWEETYINFO	*info = globalMechaTweety->mechaTweetyInfo;
	ACTORINSTANCE	*tempInstance;
	TBVector		headPosition, displacement, pos, scale, tempVector;

	switch(info->currentStage)
	{
	case MTWEETYAREA_CRATES:
		if(info->flags & MTWEETY_VULNERABLE)
		{
			baGetNodesWorldPosition(&globalMechaTweety->actorInstance, info->headNode, headPosition);
			bmVectorSub(displacement, headPosition, actorInstance->actorInstance.position);

			if(bmVectorLen(displacement) < METERTOUNIT(1.5f))
			{
				RequestNewMechaTweetySequence(globalMechaTweety, hitByCrate);
				RequestRedShieldStateChange(globalMechaTweety->mechaTweetyInfo->shieldInfo, REDSHIELD_ACTIVE);
				info->flags &= ~MTWEETY_VULNERABLE;
				actorInstance->flags |= ACTORFLAG_DESTROYNEXTUPDATE;

				if(globalMechaTweety->mechaTweetyInfo->flags & MTWEETY_GOTCRATE)
				{
					GetAttachedObjectsWorldPosition(globalMechaTweety->mechaTweetyInfo->actorInstance, globalMechaTweety->mechaTweetyInfo->attachHandle, pos);
					SETVECTOR(scale, 1.0f, 1.0f, 1.0f, 1.0f);

					tempInstance = CreateActorInstance(&map.characterInstances, "extras\\ninjathrowingcratetester.obe", "crate", 
									pos, info->actorInstance->actorInstance.scale, bIdentityQuaternion, 0);
					if(tempInstance)
					{
						CreateTazBossItemInstance(tempInstance, TAZBOSSITEM_TWEETYCRATE);
						tempInstance->tazBossItemInfo->flags |= BOSSITEMFLAG_USECLOCK;
						tempInstance->tazBossItemInfo->clock = 1.0f;
						tempInstance->actorStatus = (ACTOR_STATUS*)ZALLOC(sizeof(ACTOR_STATUS));
						ASSERTM(tempInstance->actorStatus, "out of memory");
						InitActorStatus(tempInstance->actorStatus);
						tempInstance->actorStatus->currentState = STATE_SPIN;
						CreateActorInstanceBehaviour(tempInstance);
						SetCollisionCallback(tempInstance, RespondToMTweetyCrateCollision);
						SetPreCollisionCallback(tempInstance, PreRespondToMTweetyCrateCollision);
						AddActorToCollisionSphereCache(&collisionCache, tempInstance);
						SETVECTOR(scale, 0.0f, METERTOUNIT(0.5f), 0.0f, 1.0f);
						SetActorCollSphere(tempInstance->actorBehaviour, scale, METERTOUNIT(0.5f));
						bmVectorCopy(scale, gameStatus.player[0].actorInstance->actorInstance.position);
						scale[1] += METERTOUNIT(2.5f);
						CalculateProjectileInitialVelocity(tempVector, pos, scale, RAD(15.0f));
						SetActorVelocity(tempInstance->actorBehaviour, tempVector);
					}

					if(globalMechaTweety->mechaTweetyInfo->attachHandle != -1)
					{
						RemoveObjectFromCharacter(globalMechaTweety->mechaTweetyInfo->actorInstance, globalMechaTweety->mechaTweetyInfo->attachHandle);
						globalMechaTweety->mechaTweetyInfo->flags &= ~MTWEETY_GOTCRATE;
					}
				}
			}
		}
		else
		{
			SETVECTOR(displacement, 0.0f, METERTOUNIT(0.705f), 0.0f, 1.0f);
			bmVectorAdd(headPosition, globalMechaTweety->actorInstance.position, displacement);
			bmVectorSub(displacement, headPosition, actorInstance->actorInstance.position);

			if(bmVectorLen(displacement) < METERTOUNIT(2.0f))
			{
				actorInstance->flags |= ACTORFLAG_DESTROYNEXTUPDATE;
			}
		}
		break;
	case MTWEETYAREA_MAGNETS:
		break;
	case MTWEETYAREA_ROCKETS:
		break;
	}
}

/* --------------------------------------------------------------------------------
   Function : InitialiseMTweetyGenericInfo
   Purpose : initialises all the gubbins that didn't seem to fit anywhere else
   Parameters : 
   Returns : 
   Info : 
*/

void InitialiseMTweetyGenericInfo(void)
{
	ACTORINSTANCE	*ptr;
	TBActorInstance	*instance;
	int				index;
	TBVector		velocity;
	TBVector		scale;

	mTweetyGenericInfo = (MTWEETY_GENERICINFO*)ZALLOC(sizeof(MTWEETY_GENERICINFO));
	ASSERTM(mTweetyGenericInfo, "out of memory");

	mTweetyGenericInfo->currentCamera = 1;
	mTweetyGenericInfo->destinationCamera = 1;
	mTweetyGenericInfo->cameraInfo = &camera[0];
//	SetCameraCollision(mTweetyGenericInfo->cameraInfo, FALSE);
	mTweetyGenericInfo->clock = 0.0f;
	mTweetyGenericInfo->magnetClock = MTWEETYGENERIC_MAGNETTIME;
	mTweetyGenericInfo->magnetsDestroyed[0] = 0;
	mTweetyGenericInfo->magnetsDestroyed[1] = 0;
	mTweetyGenericInfo->magnetsDestroyed[2] = 0;
	mTweetyGenericInfo->magnetsDestroyed[3] = 0;
	mTweetyGenericInfo->flags = 0;
	gameStatus.chrisCam = FALSE;

	//find magnet nodes
	index = 0;
	for(ptr = map.levelInstances.head.next;ptr != &map.levelInstances.head;ptr = ptr->next)
	{
		if((ptr->special == SPECIALTYPE_TAZBOSSITEM)&&(ptr->tazBossItemInfo->type == TAZBOSSITEM_MAGNETNODE))
		{
			if(index <2) bmVectorCopy(mTweetyGenericInfo->magnetNodes[index++], ptr->actorInstance.position);
			else break;
		}
	}
	bmVectorSub(mTweetyGenericInfo->magnetNodes[2], mTweetyGenericInfo->magnetNodes[1], mTweetyGenericInfo->magnetNodes[0]);
	mTweetyGenericInfo->magnetDistance = bmFAbs(mTweetyGenericInfo->magnetNodes[2][0]);
	bmVectorNorm(mTweetyGenericInfo->magnetNodes[2], mTweetyGenericInfo->magnetNodes[2]);

	//initialise magnetDrawBuffers
	mangaMagnetDrawBuffer = (TBPrimVertex2D*)ZALLOC(sizeof(TBPrimVertex2D)*4*MAX_MANGABLOBS);
	badMangaMagnetDrawBuffer = (TBPrimVertex2D*)ZALLOC(sizeof(TBPrimVertex2D)*4*MAX_MANGABLOBS);

	//create blobs at the magnet sources, and tweetys claws
	SETVECTOR(velocity, 0.0f, 0.0f, 0.0f, 1.0f);
	baCreateExtra(mangaMagnetExtraSystem, mTweetyGenericInfo->magnetNodes[0], velocity, &velocity[0], MAGNETFLAG_SOURCE|MAGNETFLAG_RIGHT|MAGNETFLAG_GOOD);
	baCreateExtra(mangaMagnetExtraSystem, mTweetyGenericInfo->magnetNodes[1], velocity, &velocity[0], MAGNETFLAG_SOURCE|MAGNETFLAG_LEFT|MAGNETFLAG_GOOD);
	//create tweetys at no specific position
	baCreateExtra(mangaMagnetExtraSystem, velocity, velocity, &velocity[0], MAGNETFLAG_TWEETY|MAGNETFLAG_LEFT|MAGNETFLAG_GOOD);
	baCreateExtra(mangaMagnetExtraSystem, velocity, velocity, &velocity[0], MAGNETFLAG_TWEETY|MAGNETFLAG_RIGHT|MAGNETFLAG_GOOD);
	//set field of view
	setFOV(1.0f);

	// CMD: attach shield to taz
	CreateTazShield(gameStatus.player[0].actorInstance);

	//temp
	mTweetyGenericInfo->openDoor = FALSE;
	//temp
	mTweetyGenericInfo->magnetSoundHandle = -1;
}

/* --------------------------------------------------------------------------------
   Function : UpdateMTweetyGenericInfo
   Purpose : updates the camera mostly
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateMTweetyGenericInfo(void)
{
	ACTORINSTANCE	*ptr;
	ACTORINSTANCE	*taz;
	ACTORINSTANCE	*button;
	TBVector		origin, destination, control1, control2, buttonPos;
	TBVector		yAxis = {0.0f, 1.0f, 0.0f, RAD(180.0f)};
	TBVector		zero = {0.0f, 0.0f,0.0f, 1.0f};
	TBQuaternion	tempQuat, rot;
	float			t, camDelta, buttonDist, tempFloat;
	int				zeroXVelocity, zeroZVelocity, number;
	TAZBOSSITEMINFO	*info;

	taz = gameStatus.player[0].actorInstance;

	//update camera
	if(mTweetyGenericInfo->destinationCamera != mTweetyGenericInfo->currentCamera)
	{
		//temp
		if(!mTweetyGenericInfo->openDoor) mTweetyGenericInfo->openDoor = 1;
		//temp

		//pan to destination camera
		for(ptr = map.levelInstances.head.next;ptr != &map.levelInstances.head;ptr = ptr->next)
		{
			if(ptr->special == SPECIALTYPE_TAZBOSSITEM)
			{
				if(ptr->tazBossItemInfo->type == TAZBOSSITEM_CAMERA)
				{
					if(ptr->tazBossItemInfo->number == mTweetyGenericInfo->currentCamera)
					{
						bmVectorCopy(origin, ptr->actorInstance.position);
					}
					if(ptr->tazBossItemInfo->number == mTweetyGenericInfo->destinationCamera)
					{
						bmVectorCopy(destination, ptr->actorInstance.position);
						bmRotationToQuat(rot, yAxis);
						bmQuatMultiply(tempQuat, rot, ptr->actorInstance.orientation);
					}
				}
				else
				if(ptr->tazBossItemInfo->type == TAZBOSSITEM_CAMERACONTROL)
				{
					if(ptr->tazBossItemInfo->destination == mTweetyGenericInfo->destinationCamera)
					{
						switch(ptr->tazBossItemInfo->number)
						{
						case 1:
							bmVectorCopy(control1, ptr->actorInstance.position);
							break;
						case 2:
							bmVectorCopy(control2, ptr->actorInstance.position);
							break;
						}
					}
				}
			}
		}
		
		if((t = ((mTweetyGenericInfo->clock += fTime)/MTWEETYGENERIC_PANTIME)) > 1.0f)
		{
			t = 1.0f;
			mTweetyGenericInfo->currentCamera = mTweetyGenericInfo->destinationCamera;
			// CMD: move taz
			taz->actorInstance.position[0] = destination[0];
			taz->actorInstance.position[2] = destination[2];
			taz->actorInstance.position[2] -= METERTOUNIT(1.0f);
			SetActorOrientation(taz, tempQuat);
			// CMD: reset his helmet
			ReInitialiseShieldHealth(taz, 9);
			// CMD: queue some anims
			FlushAnimationQueue(taz);
			PlayAnimationByName(taz,"run",1.0f,0,1,0.0f,NONE);
			PlayAnimationByName(taz,"runstop",1.0f,0,1,0.0f,NONE);
			PlayAnimationByName(taz,"agitated",1.0f,0,1,0.0f,NONE);
			RequestStateChange(taz, STATE_RUNON);
		}

		//pos
		FindPointOnCurve(mTweetyGenericInfo->cameraInfo->destinationPos, origin, control1, control2, destination, t);
		//look at
		bmVectorCopy(mTweetyGenericInfo->cameraInfo->destinationLookAt, globalMechaTweety->actorInstance.position);
	}
	else
	{
		if((taz->actorStatus->currentState != STATE_LOOKAROUND) &&
			(taz->actorStatus->currentState != STATE_ENTERLOOKAROUND)&&
			(~mTweetyGenericInfo->flags & MTWEETY_DONTUPDATECAMERA))
		{
			//find current camera position
			for(ptr = map.levelInstances.head.next;ptr != &map.levelInstances.head;ptr = ptr->next)
			{
				if((ptr->special == SPECIALTYPE_TAZBOSSITEM)&&(ptr->tazBossItemInfo->type == TAZBOSSITEM_CAMERA))
				{
					info = ptr->tazBossItemInfo;
					if(ptr->tazBossItemInfo->number == mTweetyGenericInfo->currentCamera)
					{
						//pos
						bmVectorCopy(mTweetyGenericInfo->cameraInfo->destinationPos, ptr->actorInstance.position);
						switch(ptr->tazBossItemInfo->number)
						{
						case 1:
						case 2:
							mTweetyGenericInfo->cameraInfo->destinationPos[0] = taz->actorInstance.position[0];
							break;
						case 3:
							mTweetyGenericInfo->cameraInfo->destinationPos[0] = taz->actorInstance.position[0];
							// CMD: also want to keep the camera reasonably close to taz
							if((mTweetyGenericInfo->cameraInfo->destinationPos[2] = (taz->actorInstance.position[2] - METERTOUNIT(4.0f))) < ptr->actorInstance.position[2])
							{
								mTweetyGenericInfo->cameraInfo->destinationPos[2] = ptr->actorInstance.position[2];
							}
							break;
						}
	
///						if(bmFAbs(camDelta = taz->actorInstance.position[0]) > METERTOUNIT(20.0f))
//						{
//							mTweetyGenericInfo->cameraInfo->destinationPos[0] = camDelta*1.5f;
//							mTweetyGenericInfo->cameraInfo->destinationPos[2] += 0.75f*(camDelta/bmFAbs(camDelta)*camDelta)-2.0f;
//						}
//						else
//						{
//							mTweetyGenericInfo->cameraInfo->destinationPos[0] = taz->actorInstance.position[0];
//						}

						//look at
						switch(ptr->tazBossItemInfo->number)
						{
						case 1:
						case 2:
							bmVectorCopy(mTweetyGenericInfo->cameraInfo->destinationLookAt, globalMechaTweety->actorInstance.position);
							break;
						case 3:
							// CMD: snap look at position o look at the nearest door button
							// CMD: should help make the aim easier, enabling me to increase the difficulty
							// CMD: in a fairer way

							// CMD: search actorlist for door buttons
							buttonDist = 0.0f;
							for(button = map.animatedInstances.head.next;button != &map.animatedInstances.head;button = button->next)
							{
								if(button->special != SPECIALTYPE_TAZBOSSITEM) continue;
								if(button->tazBossItemInfo->type != TAZBOSSITEM_DOORBUTTON) continue;

								if(buttonDist)
								{
									if((tempFloat = bmFAbs(button->actorInstance.position[0] - taz->actorInstance.position[0])) < buttonDist)
									{
										bmVectorCopy(buttonPos, button->actorInstance.position);
										buttonDist = bmFAbs(button->actorInstance.position[0] - taz->actorInstance.position[0]);
									}
								}
								else
								{
									bmVectorCopy(buttonPos, button->actorInstance.position);
									buttonDist = bmFAbs(button->actorInstance.position[0] - taz->actorInstance.position[0]);
								}
							}
							bmVectorCopy(mTweetyGenericInfo->cameraInfo->destinationLookAt, buttonPos);
							break;
						}
					}
				}
			}
		}
	}

	//update magnet effect
	if((mTweetyGenericInfo->destinationCamera == 2)||(mTweetyGenericInfo->currentCamera == 2))
	{
		//update magnet time
		if((mTweetyGenericInfo->magnetClock -= fTime) < 0.0f)
		{
			mTweetyGenericInfo->magnetClock = MTWEETYGENERIC_MINMAGTIME;
			for(int i = 4;i >= 0;i--)
			{
				//one way

				//find position
				bmVectorCopy(control1, mTweetyGenericInfo->magnetNodes[0]);
				//add randomness to y and z
				control1[1] += UtilGenerateRandomNumber(METERTOUNIT(0.5f), METERTOUNIT(-0.5f));
				control1[2] += UtilGenerateRandomNumber(METERTOUNIT(0.5f), METERTOUNIT(-0.5f));
				//find velocity
				bmVectorCopy(control2, mTweetyGenericInfo->magnetNodes[2]);
				bmVectorCMul(control2, control2, UtilGenerateRandomNumber(MTWEETYGENERIC_MAGNETMAXVEL, MTWEETYGENERIC_MAGNETMAXVEL/2.0f));
	
				// CMD: choose colour dependent on how many magnets have been destroyed
				number = 0;
				if(mTweetyGenericInfo->magnetsDestroyed[0]) number++;
				if(mTweetyGenericInfo->magnetsDestroyed[1]) number++;

				switch(number)
				{
				case 0:
					baCreateExtra(mangaMagnetExtraSystem, control1, control2, &mTweetyGenericInfo->magnetDistance, MAGNETFLAG_GOOD);
					break;
				case 1:
					if(bmRand()%2)
					{
						baCreateExtra(mangaMagnetExtraSystem, control1, control2, &mTweetyGenericInfo->magnetDistance, MAGNETFLAG_GOOD);
					}
					else
					{
						baCreateExtra(mangaMagnetExtraSystem, control1, control2, &mTweetyGenericInfo->magnetDistance, MAGNETFLAG_BAD);
					}
					break;
				case 2:
					baCreateExtra(mangaMagnetExtraSystem, control1, control2, &mTweetyGenericInfo->magnetDistance, MAGNETFLAG_BAD);
					break;
				}
				
				//the other

				//find position
				bmVectorCopy(control1, mTweetyGenericInfo->magnetNodes[1]);
				//add randomness to y and z
				control1[1] += UtilGenerateRandomNumber(METERTOUNIT(0.5f), METERTOUNIT(-0.5f));
				control1[2] += UtilGenerateRandomNumber(METERTOUNIT(0.5f), METERTOUNIT(-0.5f));
				//find velocity
				bmVectorCopy(control2, mTweetyGenericInfo->magnetNodes[2]);
				bmVectorCMul(control2, control2, -1.0f*UtilGenerateRandomNumber(MTWEETYGENERIC_MAGNETMAXVEL, MTWEETYGENERIC_MAGNETMAXVEL/2.0f));
	
				// CMD: choose colour dependent on how many magnets have been destroyed
				number = 0;
				if(mTweetyGenericInfo->magnetsDestroyed[2]) number++;
				if(mTweetyGenericInfo->magnetsDestroyed[3]) number++;

				switch(number)
				{
				case 0:
					baCreateExtra(mangaMagnetExtraSystem, control1, control2, &mTweetyGenericInfo->magnetDistance, MAGNETFLAG_GOOD);
					break;
				case 1:
					if(bmRand()%2)
					{
						baCreateExtra(mangaMagnetExtraSystem, control1, control2, &mTweetyGenericInfo->magnetDistance, MAGNETFLAG_GOOD);
					}
					else
					{
						baCreateExtra(mangaMagnetExtraSystem, control1, control2, &mTweetyGenericInfo->magnetDistance, MAGNETFLAG_BAD);
					}
					break;
				case 2:
					baCreateExtra(mangaMagnetExtraSystem, control1, control2, &mTweetyGenericInfo->magnetDistance, MAGNETFLAG_BAD);
					break;
				}
			}
		}
	}
	//update, and if necessary clamp Taz's position
	if((taz->actorStatus->currentState != STATE_RUNON)&&(taz->actorStatus->stateRequest != STATE_RUNON))
	{
		zeroXVelocity = FALSE;
		zeroZVelocity = FALSE;
		switch(mTweetyGenericInfo->currentCamera)
		{
		case 1:
			// CMD: this stage requires a limit to the x movement
			if(taz->actorInstance.position[0] < TAZBOSS_STAGE1_MINX)
			{
				taz->actorInstance.position[0] = TAZBOSS_STAGE1_MINX;
				zeroXVelocity = TRUE;
			}
			if(taz->actorInstance.position[0] > TAZBOSS_STAGE1_MAXX)
			{
				taz->actorInstance.position[0] = TAZBOSS_STAGE1_MAXX;
				zeroXVelocity = TRUE;
			}
			// CMD: now z
			if(taz->actorInstance.position[2] < TAZBOSS_STAGE1_MINZ)
			{
				taz->actorInstance.position[2] = TAZBOSS_STAGE1_MINZ;
				zeroZVelocity = TRUE;
			}
			if(taz->actorInstance.position[2] > TAZBOSS_STAGE1_MAXZ)
			{
				taz->actorInstance.position[2] = TAZBOSS_STAGE1_MAXZ;
				zeroZVelocity = TRUE;
			}
			break;
		case 2:
			// CMD: this stage requires a limit to the x movement
			if(taz->actorInstance.position[0] < TAZBOSS_STAGE2_MINX)
			{
				taz->actorInstance.position[0] = TAZBOSS_STAGE2_MINX;
				zeroXVelocity = TRUE;
			}
			if(taz->actorInstance.position[0] > TAZBOSS_STAGE2_MAXX)
			{
				taz->actorInstance.position[0] = TAZBOSS_STAGE2_MAXX;
				zeroXVelocity = TRUE;
			}
			// CMD: now z
			if(taz->actorInstance.position[2] < TAZBOSS_STAGE2_MINZ)
			{
				taz->actorInstance.position[2] = TAZBOSS_STAGE2_MINZ;
				zeroZVelocity = TRUE;
			}
			if(taz->actorInstance.position[2] > TAZBOSS_STAGE2_MAXZ)
			{
				taz->actorInstance.position[2] = TAZBOSS_STAGE2_MAXZ;
				zeroZVelocity = TRUE;
			}
			break;
		case 3:
			if(taz->actorInstance.position[2] < TAZBOSS_STAGE3_MINZ)
			{
				taz->actorInstance.position[2] = TAZBOSS_STAGE3_MINZ;
				zeroZVelocity = TRUE;
			}
			if(taz->actorInstance.position[2] > TAZBOSS_STAGE3_MAXZ)
			{
				taz->actorInstance.position[2] = TAZBOSS_STAGE3_MAXZ;
				zeroZVelocity = TRUE;
			}
			break;
		}
		if(taz->actorAnimation->useMotionBone == FALSE)
		{
			if(zeroXVelocity) taz->actorBehaviour->physics.velocity[0] = 0.0f;
			if(zeroZVelocity) taz->actorBehaviour->physics.velocity[2] = 0.0f;
		}
	}
	// CMD: if in last section check for end of game
	if((mTweetyGenericInfo->destinationCamera == 3)||(mTweetyGenericInfo->currentCamera == 3))
	{
		// CMD: can only complete game if taz hasn't been killed off
		if((~mTweetyGenericInfo->flags & MTWEETY_TWEETYBEATEN)&&(gameStatus.player[0].actorInstance->characterInfo->tazShieldInfo))
		{
			if(CheckForCompletionOfTazWantedGame())
			{
				globalMechaTweety->mechaTweetyInfo->currentScript = NULL;
				globalMechaTweety->mechaTweetyInfo->flags |= MTWEETY_TWEETYNOMOVE;
				mTweetyGenericInfo->flags |= MTWEETY_TWEETYBEATEN;
				LevelCompleted(gameStatus.player[0].actorInstance, LEVELCOMPLETE_NOSTATS);

				// CMD: just to make sure that all appropriate gallery pages have been opened
				processGalleryScore();
			}
			// CMD: debug cheat for QA, so that the hopeless sods can see the final cut scene
			if(allowDebugSkip)
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
							}
						}
					}
				}
				globalMechaTweety->mechaTweetyInfo->currentScript = NULL;
				globalMechaTweety->mechaTweetyInfo->flags |= MTWEETY_TWEETYNOMOVE;
				mTweetyGenericInfo->flags |= MTWEETY_TWEETYBEATEN;
				// CMD: MARK LEVEL AS HAVING BEEN COMPLETED
				gameStatus.globalSettings.saveInfo.stats[SCENE_TAZBOSS].levelCompleted = true;
				LevelCompleted(gameStatus.player[0].actorInstance, LEVELCOMPLETE_NOSTATS);
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : FreeMTweetyGenericInfo
   Purpose : frees all the gubbins that didn't seem to fit anywhere else
   Parameters : 
   Returns : 
   Info : 
*/

void FreeMTweetyGenericInfo(void)
{
//	SetCameraCollision(mTweetyGenericInfo->cameraInfo, TRUE);
	if(mangaMagnetDrawBuffer)
	{
		SAFE_FREE(mangaMagnetDrawBuffer);
	}
	if(badMangaMagnetDrawBuffer)
	{
		SAFE_FREE(badMangaMagnetDrawBuffer);
	}
	SAFE_FREE(mTweetyGenericInfo);
}