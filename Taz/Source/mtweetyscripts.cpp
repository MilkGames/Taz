// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : mtweetyscripts.cpp
//   Purpose : contains the scipts used for the mecha tweety boss game
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "main.h"
#include "actors.h"
#include "control.h"
#include "characters.h"
#include "redshield.h"
#include "water.h"
#include "textfx.h"
#include "animation.h"
#include "mechatweety.h"
#include "status.h"
#include "mtweetyscripts.h"
#include "bossgamecontinue.h"

//#ifdef _TAZDEBUG
int32	allowDebugSkip = FALSE;
//#endif

void myMagnetsStateChange(ACTORINSTANCE *actorInstance)
{
	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance, "tazdie1", 1.0f, 0, 0, 0.1f, DEATH);
}

/* --------------------------------------------------------------------------------
   Function : UpdateAndChooseMechaTweetySequences
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

void UpdateAndChooseMechaTweetySequences(ACTORINSTANCELIST *actorInstanceList)
{
	TBQuaternion	tempQuat;
	ACTORINSTANCE	*ptr;
	MECHATWEETYINFO	*info;

	for(ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if(ptr->special == SPECIALTYPE_MECHATWEETY) break;
		//there can only ever be one MechaTweety
	}

	if(ptr->mechaTweetyInfo)
	{
		info = ptr->mechaTweetyInfo;

		if(~info->flags & MTWEETY_WAITFORCONTINUE)
		{
			switch(info->currentStage)
			{
			case MTWEETYAREA_CRATES:
//#ifndef	INFOGRAMES
//#ifdef	_TAZDEBUG
//				if(allowDebugSkip&&(controller1.l1Channel->value)&&(controller1.l2Channel->value)&&
//				(controller1.r1Channel->value)&&(controller1.r2Channel->value))
				if(allowDebugSkip)
				{
					//onto the next stage
					RequestRedShieldStateChange(info->shieldInfo, REDSHIELD_INACTIVE);
					RequestNewMechaTweetySequence(ptr, moveToMagnetRoom);
					info->currentStage = MTWEETYAREA_MAGNETS;
					if(info->attachHandle != -1) RemoveObjectFromCharacter(ptr, info->attachHandle);
					allowDebugSkip = FALSE;
				}
//#endif
//#endif
				switch(info->aiState)
				{
					//check for start of level
				case MTWEETYAI_START:
					info->attackPosition = MTWEETYATTACK_LEFT;
					RequestNewMechaTweetySequence(ptr, crateRoomSlowAttackLeft);
					break;
				case MTWEETYAI_END:
					//check number of hits left at end of sequence
					if(info->hitsToNextStage)
					{
						if(info->hitsToNextStage < 4)
						{
							switch(info->attackPosition)
							{
							case MTWEETYATTACK_LEFT:
								info->attackPosition = MTWEETYATTACK_RIGHT;
								RequestNewMechaTweetySequence(ptr, crateRoomQuickAttackRight);
								break;
							case MTWEETYATTACK_RIGHT:
								info->attackPosition = MTWEETYATTACK_LEFT;
								RequestNewMechaTweetySequence(ptr, crateRoomQuickAttackLeft);
								break;
							}
						}
						else
						{
							switch(info->attackPosition)
							{
							case MTWEETYATTACK_LEFT:
								info->attackPosition = MTWEETYATTACK_RIGHT;
								RequestNewMechaTweetySequence(ptr, crateRoomSlowAttackRight);
								break;
							case MTWEETYATTACK_RIGHT:
								info->attackPosition = MTWEETYATTACK_LEFT;
								RequestNewMechaTweetySequence(ptr, crateRoomSlowAttackLeft);
								break;
							}
						}
					}
					else
					{
						//onto the next stage
						RequestRedShieldStateChange(info->shieldInfo, REDSHIELD_INACTIVE);
						RequestNewMechaTweetySequence(ptr, moveToMagnetRoom);
						info->currentStage = MTWEETYAREA_MAGNETS;
					}
					break;
				}
				break;
			case MTWEETYAREA_MAGNETS:
//#ifndef	INFOGRAMES
//#ifdef	_TAZDEBUG
//				if((!controller1.l1Channel->value)&&(!controller1.l2Channel->value)&&
//				(!controller1.r1Channel->value)&&(!controller1.r2Channel->value))
//				{
//					allowDebugSkip = TRUE;
//				}
//				else
//				if(allowDebugSkip&&(controller1.l1Channel->value)&&(controller1.l2Channel->value)&&
//				(controller1.r1Channel->value)&&(controller1.r2Channel->value))
				if(allowDebugSkip)
				{
					// CMD: choose tweety disable and move to next area sequence
					info->flags |= MTWEETY_BADELECTRICITY;
					RequestNewMechaTweetySequence(ptr, moveToRockets);
					info->currentStage = MTWEETYAREA_ROCKETS;
					allowDebugSkip = FALSE;
				}
//#endif
//#endif
				switch(info->aiState)
				{
				case MTWEETYAI_END:
					// CMD: check that all magnet hit points have been disabled
					if(mTweetyGenericInfo->magnetsDestroyed[0]&&mTweetyGenericInfo->magnetsDestroyed[1]&&
						mTweetyGenericInfo->magnetsDestroyed[2]&&mTweetyGenericInfo->magnetsDestroyed[3])
					{
						// CMD: choose tweety disable and move to next area sequence
						info->flags |= MTWEETY_BADELECTRICITY;
						RequestNewMechaTweetySequence(ptr, moveToRockets);
						info->attackPosition = MTWEETYATTACK_LEFT;
						info->currentStage = MTWEETYAREA_ROCKETS;
					}
					else
					{
						RequestNewMechaTweetySequence(ptr, powerUp);
					}
					break;
				}
				if(~info->flags & MTWEETY_NOHURTTAZ)
				{
					if(UpdateElectricityAttackCollisionWithActor(info->leftElectricAttack, gameStatus.player[0].actorInstance)||
						UpdateElectricityAttackCollisionWithActor(info->rightElectricAttack, gameStatus.player[0].actorInstance))
					{
						RequestNewMechaTweetySequence(ptr, hurrahAndLowerMagnet);
						info->flags |= MTWEETY_NOHURTTAZ;

						if(gameStatus.player[0].actorInstance->characterInfo->tazShieldInfo)
						{
							if(!ReduceShieldHealthByOne(gameStatus.player[0].actorInstance))
							{
								// CMD: make sure Taz and Tweety are doing the right thing
								RequestNewMechaTweetySequence(globalMechaTweety, holdAtPosition);
								globalMechaTweety->mechaTweetyInfo->flags |= MTWEETY_TWEETYNOMOVE;
								bmQuatCopy(tempQuat,gameStatus.player[0].actorInstance->actorInstance.orientation);
								ChangeActorInstance(gameStatus.player[0].actorInstance,FindActorInActorList("tazdevil.obe"),5,NULL);
								bmQuatCopy(gameStatus.player[0].actorInstance->actorInstance.orientation,tempQuat);
								ResetCharacterAfterJump(gameStatus.player[0].actorInstance);
								if(RequestStateChangePlus(gameStatus.player[0].actorInstance, STATE_PLAYANIMANDFREEZE, myMagnetsStateChange))
								{
									if(!UpdateCharacterState(&gameStatus.player[gameStatus.player[0].actorInstance->playerNo], true))
									{
										ClearStateChangeRequest(gameStatus.player[0].actorInstance);
									}
								}
								mTweetyGenericInfo->flags |= MTWEETY_DONTUPDATECAMERA;
								ProcessTazDeathAtEndOfBossGame(ResetMechaTweetyCallback, (void*)globalMechaTweety->mechaTweetyInfo);
	//							bkGenerateEvent("scenechange","_tazboss2");
							}
						}
					}
				}
				break;
			case MTWEETYAREA_ROCKETS:
				switch(info->aiState)
				{
				case MTWEETYAI_END:
					mTweetyGenericInfo->flags &= ~MTWEETY_HITBYROCKET;
					switch(info->attackPosition)
					{
					case MTWEETYATTACK_MIDDLE:
						if(bmRand()%2)
						{
							info->attackPosition = MTWEETYATTACK_LEFT;
							RequestNewMechaTweetySequence(ptr, rocketAttackLeft);
						}
						else
						{
							info->attackPosition = MTWEETYATTACK_RIGHT;
							RequestNewMechaTweetySequence(ptr, rocketAttackRight);
						}
						break;
					default:
						info->attackPosition = MTWEETYATTACK_MIDDLE;
						RequestNewMechaTweetySequence(ptr, rocketAttackMiddle);
						break;
					}
				}
				// CMD: check for rocket collision with Taz, not done in similar manner to above
				// CMD: uglier, yes, but also quicker
				if(mTweetyGenericInfo->flags & MTWEETY_HITBYROCKET)
				{
					if(~info->flags & MTWEETY_NOHURTTAZ)
					{
						RequestNewMechaTweetySequence(ptr, hurrah);
						info->flags |= MTWEETY_NOHURTTAZ;
					}
				}
				break;
			}
		}
		else
		{
			// CMD: wait here for player to continue or quit to the front end
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateAndChooseMechaTweetySequences
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

void RequestNewMechaTweetySequence(ACTORINSTANCE *actorInstance, MTWEETY_EVENT *newScript)
{
	// CMD: checks for unwanted transitions here please
	if(actorInstance->mechaTweetyInfo->flags & MTWEETY_TWEETYNOMOVE) return;
	if((newScript == grumpyStompAndLowerMagnet)&&(actorInstance->mechaTweetyInfo->currentScript == hurrahAndLowerMagnet)) return;
	if((newScript == hurrahAndLowerMagnet)&&(actorInstance->mechaTweetyInfo->currentScript == grumpyStompAndLowerMagnet)) return;
	if((newScript == hurrahAndLowerMagnet)&&(actorInstance->mechaTweetyInfo->currentScript == hurrahAndLowerMagnet)) return;
//	if(actorInstance->mechaTweetyInfo->currentScript == defeated) return;
//	if(actorInstance->mechaTweetyInfo->currentScript == holdAtPosition) return;

	actorInstance->mechaTweetyInfo->flags &= ~MTWEETY_NOHURTTAZ;

	// CMD: get on with it
	actorInstance->mechaTweetyInfo->lastScript = actorInstance->mechaTweetyInfo->currentScript;
	actorInstance->mechaTweetyInfo->currentScript = newScript;
	actorInstance->mechaTweetyInfo->scriptIndex = 0;
	actorInstance->mechaTweetyInfo->scriptIndexLastFrame = -1;
}

/* --------------------------------------------------------------------------------
   Function : UpdateAndChooseMechaTweetySequences
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 ResetMechaTweetyCallback(void *context, EBossGameResetChoice choice)
{
	MECHATWEETYINFO		*info = (MECHATWEETYINFO*)context;
	ACTORINSTANCE		*tweety = info->actorInstance;
	ACTORINSTANCE		*tempInstance;
	TBVector			destination;
	TBVector			yAxis = {0.0f, 1.0f, 0.0f, RAD(180.0f)};
	TBQuaternion		tempQuat, rot;

	switch(choice)
	{
	case BOSSGAMERESET_CONTINUE:
		// CMD: this is more complex
		// CMD: need to reset Taz, Mechatweety, and all of the 'TazBossItems'
		gameStatus.chrisCam = FALSE;
		mTweetyGenericInfo->flags &= ~MTWEETY_DONTUPDATECAMERA;

		// TP: I moved this here, he he
		gameStatus.player[0].camera->fade.StartFadeUp(FADETYPE_CIRCLE, 1.5f);

		// CMD: re-initialise the taz boss items
		for(tempInstance = map.levelInstances.head.next;tempInstance != &map.levelInstances.head;tempInstance = tempInstance->next)
		{
			if(tempInstance->special == SPECIALTYPE_TAZBOSSITEM)
			{
				switch(tempInstance->tazBossItemInfo->type)
				{
				case TAZBOSSITEM_DOOR:
				case TAZBOSSITEM_MAGNET:
				case TAZBOSSITEM_MAGNETHITPOINT:
				case TAZBOSSITEM_MAGNETNODE:
				case TAZBOSSITEM_DOORBUTTON:
					InitialiseTazBossItemInstance(tempInstance);
					break;
				default:
					continue;
				}

			}
		}
		for(tempInstance = map.animatedInstances.head.next;tempInstance != &map.animatedInstances.head;tempInstance = tempInstance->next)
		{
			if(tempInstance->special == SPECIALTYPE_TAZBOSSITEM)
			{
				switch(tempInstance->tazBossItemInfo->type)
				{
				case TAZBOSSITEM_DOOR:
				case TAZBOSSITEM_MAGNET:
				case TAZBOSSITEM_MAGNETHITPOINT:
				case TAZBOSSITEM_MAGNETNODE:
				case TAZBOSSITEM_DOORBUTTON:
					InitialiseTazBossItemInstance(tempInstance);
					break;
				default:
					continue;
				}
			}
		}

		// CMD: reset Taz's helmet
		CreateTazShield(gameStatus.player[0].actorInstance);

		tweety->mechaTweetyInfo->flags &= ~MTWEETY_TWEETYNOMOVE;

		switch(info->currentStage)
			{
			case MTWEETYAREA_CRATES:
				// CMD: reset Tweety
				// CMD: set actor position
				SetActorPosition(tweety, info->homePos);
				SetActorOrientation(tweety, info->homeOrientation);
				// CMD: set other attributes
				tweety->mechaTweetyInfo->aiState = MTWEETYAI_START;
				tweety->mechaTweetyInfo->aiLastState = MTWEETYAI_IDLE; 
				PlayAnimationByName(tweety, "idle1", 1.0f, 1, 0, 0.1f, IDLE);
				tweety->mechaTweetyInfo->aiCallback = MechaTweetyIdleCallback;
				tweety->mechaTweetyInfo->turnAmount = 0.0f;
				tweety->mechaTweetyInfo->flags = 0;

				tweety->mechaTweetyInfo->currentScript = NULL;
				tweety->mechaTweetyInfo->lastScript = NULL;
				tweety->mechaTweetyInfo->scriptIndex = 0;
				tweety->mechaTweetyInfo->scriptIndexLastFrame = -1;
//				actorInstance->mechaTweetyInfo->scaleInfo.scale = MTWEETY_SCALE;
				tweety->mechaTweetyInfo->attackPosition = MTWEETYATTACK_MIDDLE;

				tweety->mechaTweetyInfo->currentStage = MTWEETYAREA_CRATES;
				tweety->mechaTweetyInfo->hitsToNextStage = 5;

				// CMD: reset Taz
				if(tempInstance = FindActorInstanceInMap(NULL, map.startPos, &map))
				{
					SetActorPosition(gameStatus.player[0].actorInstance, tempInstance->actorInstance.position);
					gameStatus.player[0].actorInstance->actorInstance.position[2] -= METERTOUNIT(2.0f);

					bmRotationToQuat(rot, yAxis);
					bmQuatMultiply(tempQuat, rot, tempInstance->actorInstance.orientation);
					SetActorOrientation(gameStatus.player[0].actorInstance, tempQuat);
				
					FlushAnimationQueue(gameStatus.player[0].actorInstance);
					PlayAnimationByName(gameStatus.player[0].actorInstance,"run",1.0f,0,1,0.0f,NONE);
					PlayAnimationByName(gameStatus.player[0].actorInstance,"runstop",1.0f,0,1,0.0f,NONE);
					PlayAnimationByName(gameStatus.player[0].actorInstance,"agitated",1.0f,0,1,0.0f,NONE);
//					RequestStateChange(gameStatus.player[0].actorInstance, STATE_RUNON);
					gameStatus.player[0].actorInstance->actorStatus->lastState = gameStatus.player[0].actorInstance->actorStatus->currentState;
					gameStatus.player[0].actorInstance->actorStatus->currentState = STATE_RUNON;
					gameStatus.player[0].actorInstance->actorStatus->timeInCurrentState = 0.0f;
					gameStatus.player[0].actorInstance->actorStatus->firstTimeInState = true;
				}
				break;
			case MTWEETYAREA_MAGNETS:
				// CMD: reset Tweety
				// CMD: set actor position
				SetActorPosition(tweety, info->homePos);
				SetActorOrientation(tweety, info->homeOrientation);

				RequestNewMechaTweetySequence(tweety, powerUp);

				// CMD: set other attributes
				mTweetyGenericInfo->clock = 0.0f;
				mTweetyGenericInfo->magnetClock = MTWEETYGENERIC_MAGNETTIME;
				mTweetyGenericInfo->magnetsDestroyed[0] = 0;
				mTweetyGenericInfo->magnetsDestroyed[1] = 0;
				mTweetyGenericInfo->magnetsDestroyed[2] = 0;
				mTweetyGenericInfo->magnetsDestroyed[3] = 0;
				mTweetyGenericInfo->flags = 0;

				// CMD: reset Taz
				for(tempInstance = map.levelInstances.head.next;tempInstance != &map.levelInstances.head;tempInstance = tempInstance->next)
				{
					if(tempInstance->special == SPECIALTYPE_TAZBOSSITEM)
					{
						if(tempInstance->tazBossItemInfo->type == TAZBOSSITEM_CAMERA)
						{
							if(tempInstance->tazBossItemInfo->number == mTweetyGenericInfo->currentCamera)
							{
								bmVectorCopy(destination, tempInstance->actorInstance.position);
								bmQuatCopy(tempQuat, tempInstance->actorInstance.orientation);
								bmRotationToQuat(rot, yAxis);
								bmQuatMultiply(tempQuat, rot, tempQuat);
							}
						}
					}
				}
		
				gameStatus.player[0].actorInstance->actorInstance.position[0] = destination[0];
				gameStatus.player[0].actorInstance->actorInstance.position[2] = destination[2];
				gameStatus.player[0].actorInstance->actorInstance.position[2] -= METERTOUNIT(1.0f);
				SetActorOrientation(gameStatus.player[0].actorInstance, tempQuat);
				// CMD: queue some anims
				FlushAnimationQueue(gameStatus.player[0].actorInstance);
				PlayAnimationByName(gameStatus.player[0].actorInstance,"run",1.0f,0,1,0.0f,NONE);
				PlayAnimationByName(gameStatus.player[0].actorInstance,"runstop",1.0f,0,1,0.0f,NONE);
				PlayAnimationByName(gameStatus.player[0].actorInstance,"agitated",1.0f,0,1,0.0f,NONE);
//				RequestStateChange(gameStatus.player[0].actorInstance, STATE_RUNON);
				gameStatus.player[0].actorInstance->actorStatus->lastState = gameStatus.player[0].actorInstance->actorStatus->currentState;
				gameStatus.player[0].actorInstance->actorStatus->currentState = STATE_RUNON;
				gameStatus.player[0].actorInstance->actorStatus->timeInCurrentState = 0.0f;
				gameStatus.player[0].actorInstance->actorStatus->firstTimeInState = true;
				break;
			case MTWEETYAREA_ROCKETS:
				// CMD: reset Tweety
				// CMD: set actor position
				SetActorPosition(tweety, info->homePos);
				SetActorOrientation(tweety, info->homeOrientation);

				RequestNewMechaTweetySequence(tweety, rocketAttackMiddle);

				// CMD: set other attributes
				// CMD: reset Taz
				for(tempInstance = map.levelInstances.head.next;tempInstance != &map.levelInstances.head;tempInstance = tempInstance->next)
				{
					if(tempInstance->special == SPECIALTYPE_TAZBOSSITEM)
					{
						if(tempInstance->tazBossItemInfo->type == TAZBOSSITEM_CAMERA)
						{
							if(tempInstance->tazBossItemInfo->number == mTweetyGenericInfo->currentCamera)
							{
								bmVectorCopy(destination, tempInstance->actorInstance.position);
								bmRotationToQuat(rot, yAxis);
								bmQuatMultiply(tempQuat, rot, tempInstance->actorInstance.orientation);
							}
						}
					}
				}
		
				gameStatus.player[0].actorInstance->actorInstance.position[0] = destination[0];
				gameStatus.player[0].actorInstance->actorInstance.position[2] = destination[2];
				gameStatus.player[0].actorInstance->actorInstance.position[2] -= METERTOUNIT(1.0f);
				SetActorOrientation(gameStatus.player[0].actorInstance, tempQuat);
				// CMD: queue some anims
				FlushAnimationQueue(gameStatus.player[0].actorInstance);
				PlayAnimationByName(gameStatus.player[0].actorInstance,"run",1.0f,0,1,0.0f,NONE);
				PlayAnimationByName(gameStatus.player[0].actorInstance,"runstop",1.0f,0,1,0.0f,NONE);
				PlayAnimationByName(gameStatus.player[0].actorInstance,"agitated",1.0f,0,1,0.0f,NONE);
//				RequestStateChange(gameStatus.player[0].actorInstance, STATE_RUNON);
				gameStatus.player[0].actorInstance->actorStatus->lastState = gameStatus.player[0].actorInstance->actorStatus->currentState;
				gameStatus.player[0].actorInstance->actorStatus->currentState = STATE_RUNON;
				gameStatus.player[0].actorInstance->actorStatus->timeInCurrentState = 0.0f;
				gameStatus.player[0].actorInstance->actorStatus->firstTimeInState = true;

				RandomiseButtonTimes();
				break;
		}
		break;
	case BOSSGAMERESET_QUIT:
		// CMD: all we need to do is reset back to the tazhub
		camera[0].fade.FadeAndChangeScene(&gameStatus,FADETYPE_NORMAL,SCENE_TAZHUB,1.0f,bkCRC32((uchar*)"tazboss2door", strlen("tazboss2door"), 0));
		SetGameState(&gameStatus,GAMESTATE_FADING,GAMESUBSTATE_NORMAL);
		SetHearingRange(METERTOUNIT(2.5f), METERTOUNIT(10.0f));
		break;
	}

	// PP: Must return a value
	return 0xB16B00B5;
}