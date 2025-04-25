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
#include "animation.h"
#include "fade.h"
#include "water.h"
#include "cameracase.h"
#include "files.h"
#include "characters.h"
#include "attach.h"
#include "VideoFX.h"
#include "timer.h"
#include "textures.h"
#include "bossgamecontinue.h"

BOSSGAMECONTINUEINFO	*bossGameContinueInfo;

/* --------------------------------------------------------------------------------
   Function : LoadBossGameContinue
   Purpose : Loads all the obe's needed for the bossgame continue screen
   Parameters : 
   Returns : TRUE if all obe's loaded succesfully
   Info :
*/

int LoadBossGameContinue(void)
{
	if(!LoadWholePackage("continue"))
	{
		bkPrintf("*** WARNING *** could not load package 'continue'");
		return FALSE;
	}
	if(!LoadActor("sharktank.obe", "continue", 0)) return FALSE;
	if(!LoadActor("continuedevil.obe", "continue", 0)) return FALSE;
	if(!LoadActor("shark01.obe", "continue", 0)) return FALSE;
	if(!LoadActor("shark02.obe", "continue", 0)) return FALSE;
	if(!LoadActor("shark03.obe", "continue", 0)) return FALSE;
	if(!LoadActor("shedevilbowtie.obe", "continue", 0)) return FALSE;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ProcessTazDeathAtHandsOfMechatweety
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

void ProcessTazDeathAtEndOfBossGame(BossGameResetCallback callback, void *context, TBVector	lookAt, BossGameFadeDownCallback fadeDownCallback)
{
	ACTORINSTANCE			*taz = gameStatus.player[0].actorInstance;
	TBActorNodeInstance		*node;
	TBVector				worldSpace, screenSpace, position, scale;

	bossGameContinueInfo = (BOSSGAMECONTINUEINFO*)ZALLOC(sizeof(BOSSGAMECONTINUEINFO));
	ASSERTM(bossGameContinueInfo, "we really are short of memory if this cannot be malloc'ed");

	bossGameContinueInfo->callback = callback;
	bossGameContinueInfo->context = context;
	bossGameContinueInfo->choice = BOSSGAMERESET_NONE;
	bossGameContinueInfo->fadeDownCallback = fadeDownCallback;

	bossGameContinueInfo->fovState = FOV_STATIC;
	bossGameContinueInfo->fovInit = videoMode.screens[0].fov;
	// TP: changed this because the zooboss changes the fov
	bossGameContinueInfo->fov = NORMAL_INGAME_FOV;

	bossGameContinueInfo->popUp = NULL;

	// CMD: if Taz has anything in his mouth get it out
	if (taz->characterInfo->inMouth)
	{
		FreeDestructibleInfo(taz->characterInfo->inMouth);
		SAFE_FREE(taz->characterInfo->inMouth);
		taz->characterInfo->inMouth = NULL;
	}

	// CMD: create actorInstance for continue 'scene'
	bmVectorSet(scale, 1.0f, 1.0f, 1.0f, 1.0f);
	bmVectorSet(position, METERTOUNIT(300.0f), METERTOUNIT(300.0f), METERTOUNIT(300.0f), 1.0f);
	if(bossGameContinueInfo->sharkPool = CreateActorInstance(&map.animatedInstances, "sharktank.obe", "continue", position, scale, bIdentityQuaternion, 0))
	{
		FlushAnimationQueue(bossGameContinueInfo->sharkPool);
		PlayAnimationByName(bossGameContinueInfo->sharkPool, "swingin", 1.0f, 1, 0, 0.1f, IDLE);
	}
	if(bossGameContinueInfo->sheDevil = CreateActorInstance(&map.characterInstances, "continuedevil.obe", "continuedevil", position, scale, bIdentityQuaternion, 0))
	{
		EnableActorCartoonRenderMode(&bossGameContinueInfo->sheDevil->actorInstance);
		FlushAnimationQueue(bossGameContinueInfo->sheDevil);
		PlayAnimationByName(bossGameContinueInfo->sheDevil, "swingin", 1.0f, 1, 0, 0.1f, IDLE);
		CreateActorInstanceCharacterInfo(bossGameContinueInfo->sheDevil, 0);
		// TP: This is done by CreateActorInstanceCharacterInfo - InitialiseCharacterInfo(bossGameContinueInfo->sheDevil, 0);
		AttachObjectToCharacter(bossGameContinueInfo->sheDevil,"shedevilbowtie.obe","spinetop",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);
	}
	if(bossGameContinueInfo->sharks[0] = CreateActorInstance(&map.animatedInstances, "shark01.obe", "continueshark01", position, scale, bIdentityQuaternion, 0))
	{
		EnableActorCartoonRenderMode(&bossGameContinueInfo->sharks[0]->actorInstance);
		FlushAnimationQueue(bossGameContinueInfo->sharks[0]);
		PlayAnimationByName(bossGameContinueInfo->sharks[0], "swingin", 1.0f, 1, 0, 0.1f, IDLE);
	}
	if(bossGameContinueInfo->sharks[1] = CreateActorInstance(&map.animatedInstances, "shark02.obe", "continueshark02", position, scale, bIdentityQuaternion, 0))
	{
		EnableActorCartoonRenderMode(&bossGameContinueInfo->sharks[1]->actorInstance);
		FlushAnimationQueue(bossGameContinueInfo->sharks[1]);
		PlayAnimationByName(bossGameContinueInfo->sharks[1], "swingin", 1.0f, 1, 0, 0.1f, IDLE);
	}
	if(bossGameContinueInfo->sharks[2] = CreateActorInstance(&map.animatedInstances, "shark03.obe", "continueshark03", position, scale, bIdentityQuaternion, 0))
	{
		EnableActorCartoonRenderMode(&bossGameContinueInfo->sharks[2]->actorInstance);
		FlushAnimationQueue(bossGameContinueInfo->sharks[2]);
		PlayAnimationByName(bossGameContinueInfo->sharks[2], "swingin", 1.0f, 1, 0, 0.1f, IDLE);
	}

//	gameStatus.controlSuspend = TRUE;

	// CMD: flush the rumble queue
	if(gameStatus.player[0].controller) FlushRumbleQueue(gameStatus.player[0].controller);
	if(gameStatus.player[1].controller) FlushRumbleQueue(gameStatus.player[1].controller);

	// CMD: fade to black and offer the player the option to continue
	if (!lookAt)
	{
		node = baFindNode(taz->actorInstance.rootNodeInstance, "snoutstem");
		if(node)
		{
			baGetNodesWorldPosition(&taz->actorInstance, node, worldSpace);
			worldSpace[3] = 1.0f;
		}
		else
		{
			bmVectorCopy(worldSpace, taz->actorInstance.position);
			worldSpace[1] += METERTOUNIT(0.5f);
			worldSpace[3] = 1.0f;
		}
	}
	else
	{
		bmVectorCopy(worldSpace, lookAt);
	}
	
	if (gameStatus.player[taz->playerNo].camera->fade.Lock())
	{
		gameStatus.player[taz->playerNo].camera->fade.StartFadeDown(FADETYPE_CIRCLE, 3.0f);
		gameStatus.player[taz->playerNo].camera->fade.SetPlayer(taz);
		gameStatus.player[taz->playerNo].camera->fade.SetFadeDownFinishedCallback(FinishedFadeFromDeathInBoss);
		bdSetIdentityObjectMatrix();
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
		bdSetIdentityObjectMatrix();
		bdProjectVertices(screenSpace, worldSpace, 1);
		gameStatus.player[taz->playerNo].camera->fade.SetCircleFadeTo(screenSpace[0], screenSpace[1]);
		gameStatus.player[taz->playerNo].camera->fade.Unlock();
	}
	else
	{
		// TP: TODO: handle access to fade problem
	}
}

/* --------------------------------------------------------------------------------
   Function : FinishedFadeFromDeathInBlimp
   Purpose : 
   Parameters : FADE *
   Returns : 
   Info : 
*/

void InitialiseBossGameContinue(void)
{
	bossGameContinueInfo = NULL;
}

/* --------------------------------------------------------------------------------
   Function : FinishedFadeFromDeathInBlimp
   Purpose : 
   Parameters : FADE *
   Returns : 
   Info : 
*/

void UpdateBossGameContinue(void)
{
	TBVector	position;
	int	retval = -1;
	int	i;


	switch(bossGameContinueInfo->fovState)
	{
	case FOV_STATIC:
		break;
	case FOV_DECREASE:
		if((bossGameContinueInfo->fov -= bossGameContinueInfo->fovRateOfChange*fTime) < bossGameContinueInfo->fovTarget)
		{
			bossGameContinueInfo->fov = bossGameContinueInfo->fovTarget;
			// CMD: set up to increase fov back to previous
			bossGameContinueInfo->fovTarget = bossGameContinueInfo->fovInit;
			bossGameContinueInfo->fovRateOfChange = RAD(15.0f);
			bossGameContinueInfo->fovState = FOV_INCREASE;
		}
		break;
	case FOV_INCREASE:
		if((bossGameContinueInfo->fov += bossGameContinueInfo->fovRateOfChange*fTime) > bossGameContinueInfo->fovTarget)
		{
			bossGameContinueInfo->fov = bossGameContinueInfo->fovTarget;
			bossGameContinueInfo->fovState = FOV_STATIC;
		}
		break;
	}
	setFOV(bossGameContinueInfo->fov);

	if(bossGameContinueInfo->popUp)
	{
		retval = bossGameContinueInfo->popUp->getResponse();

		if (retval > 0)
		{
			BookList_closeAndFree(bossGameContinueInfo->popUp);
	
			// CMD: set some camera attributes
			bmVectorSet(position, METERTOUNIT(299.36f), METERTOUNIT(303.76f), METERTOUNIT(297.22f), 1.0f);
			SetCameraPosition(gameStatus.player[0].camera, position);
			
			SetCameraSmooth(gameStatus.player[0].camera, 0.015f, 0.01f);
			bmVectorSub(position, gameStatus.player[0].camera->cameraState.cameraLookat, gameStatus.player[0].camera->cameraState.cameraPos);
			bmVectorCMul(position, position, 0.5f);
			position[1] += METERTOUNIT(3.0f);
			bmVectorAdd(position, position, gameStatus.player[0].camera->cameraState.cameraPos);
			SetCameraLookAtDestination(gameStatus.player[0].camera, position);
			bmVectorCopy(position, gameStatus.player[0].camera->cameraState.cameraPos);
			position[1] += METERTOUNIT(5.0f);
			SetCameraPositionDestination(gameStatus.player[0].camera, position);
			// CMD: start to with the field of view
//			bossGameContinueInfo->fovTarget = RAD(30.0f);
//			bossGameContinueInfo->fovRateOfChange = RAD(20.0f);
//			bossGameContinueInfo->fovState = FOV_DECREASE;

			// CMD: start fade up
			gameStatus.player[0].camera->fade.StartFadeDown(FADETYPE_CIRCLE, 3.0f);
			gameStatus.player[0].camera->fade.SetFadeDownFinishedCallback(MadeContinueChoiceCallback);
			
			FlushAnimationQueue(bossGameContinueInfo->sharkPool);
			FlushAnimationQueue(bossGameContinueInfo->sheDevil);
			for(i = 2;i >= 0;--i)
			{
				FlushAnimationQueue(bossGameContinueInfo->sharks[i]);
			}

			switch(retval)
			{
			case 1:
				// CMD: continue
				PlayAnimationByName(bossGameContinueInfo->sharkPool, "swingup", 1.0f, 0, 0, 0.1f, REACT);
				PlayAnimationByName(bossGameContinueInfo->sheDevil, "swingup", 1.0f, 0, 0, 0.1f, REACT);
				for(i = 2;i >= 0;--i)
				{
					PlayAnimationByName(bossGameContinueInfo->sharks[i], "swingup", 1.0f, 0, 0, 0.0f, REACT);
				}
				bossGameContinueInfo->choice = BOSSGAMERESET_CONTINUE;
				break;
			case 2:
				// CMD: quit
				PlayAnimationByName(bossGameContinueInfo->sharkPool, "swingdown", 1.0f, 0, 0, 0.1f, REACT);
				PlayAnimationByName(bossGameContinueInfo->sheDevil, "swingdown", 1.0f, 0, 0, 0.1f, REACT);
				for(i = 2;i >= 0;--i)
				{
					PlayAnimationByName(bossGameContinueInfo->sharks[i], "swingdown", 1.0f, 0, 0, 0.0f, REACT);
				}
				bossGameContinueInfo->choice = BOSSGAMERESET_QUIT;
				break;
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : FinishedFadeFromDeathInBlimp
   Purpose : 
   Parameters : FADE *
   Returns : 
   Info : 
*/

void FreeBossGameContinue(void)
{
	FreeWholePackage("continue");
	if(bossGameContinueInfo)
	{
		SAFE_FREE(bossGameContinueInfo);
	}
}

/* --------------------------------------------------------------------------------
   Function : FinishedFadeFromDeathInBlimp
   Purpose : 
   Parameters : FADE *
   Returns : 
   Info : 
*/

void FinishedFadeFromDeathInBoss(FADE *fade)
{ 	
	TBVector	lookAt, pos, add;

	if (bossGameContinueInfo->fadeDownCallback) bossGameContinueInfo->fadeDownCallback();

	// CMD: need to reset the camera and point it at the shark pool
	SimplifyCamera(gameStatus.player[0].camera);
	bmVectorCopy(lookAt, bossGameContinueInfo->sharkPool->actorInstance.position);
	bmVectorSet(add, METERTOUNIT(0.0f), METERTOUNIT(2.9f), METERTOUNIT(1.0f), 1.0f);
	bmVectorAdd(lookAt, lookAt, add);

	bmVectorCopy(pos, bossGameContinueInfo->sharkPool->actorInstance.position);
	bmVectorSet(add, METERTOUNIT(-0.4f), METERTOUNIT(3.5f), METERTOUNIT(-3.0f), 1.0f);
	bmVectorAdd(pos, pos, add);

	bmVectorCopy(gameStatus.player[0].camera->lookAt, lookAt);
	bmVectorCopy(gameStatus.player[0].camera->destinationLookAt, lookAt);
	bmVectorCopy(gameStatus.player[0].camera->pos, pos);
	bmVectorCopy(gameStatus.player[0].camera->destinationPos, pos);

	// CMD: set game state
	SetGameState(&gameStatus, GAMESTATE_BOSSCONTINUE, GAMESUBSTATE_NORMAL);

	// CMD: stop any sounds that might be playing
	StopAllChannels();

	// CMD: start fade up
	fade->SetCircleFadeTo(0.0f, 0.0f);
	gameStatus.player[0].camera->fade.StartFadeUp(FADETYPE_CIRCLE, 1.5f);
	gameStatus.player[0].camera->fade.SetFadeUpFinishedCallback(OpenContinueBookCallback);
}

/* --------------------------------------------------------------------------------
   Function : FinishedFadeFromDeathInBlimp
   Purpose : 
   Parameters : FADE *
   Returns : 
   Info : 
*/

void OpenContinueBookCallback(FADE *fade)
{
	bossGameContinueInfo->popUp = new POPUP(POPUP_CONFIRM, 1, STR_SG_LOAD, STR_DIALOGUE_YES, STR_DIALOGUE_NO, STR_DIALOGUE_CROSSOK);
}

/* --------------------------------------------------------------------------------
   Function : FinishedFadeFromDeathInBlimp
   Purpose : 
   Parameters : FADE *
   Returns : 
   Info : 
*/

void MadeContinueChoiceCallback(FADE *fade)
{
	TBVector			cameraEye, cameraGarib, tempVector;
	ACTORINSTANCE		*tazInstance = gameStatus.player[0].actorInstance;
	CAMERAINFO			*cameraInfo = gameStatus.player[0].camera;

	gameStatus.controlSuspend = FALSE;

	// CMD: reset the camera
	bmVectorSet(cameraEye,0.0f,0.0f,METERTOUNIT(2.0f),1.0f);

	VectorQuatMultiply(cameraEye,tazInstance->actorInstance.orientation);
	bmVectorAdd(cameraEye,cameraEye,tazInstance->actorInstance.position);
	bmVectorCopy(cameraGarib,tazInstance->actorInstance.position);

	DestroyCamera(cameraInfo);
	InitCamera(cameraInfo,cameraEye,cameraGarib);
	
	ZEROVECTOR(tempVector);

	CreateCameraCollSphere(cameraInfo,tempVector,METERTOUNIT(0.5f),METERTOUNIT(9.0f));
		
	SetCameraLookAtFollowActorInstance(cameraInfo,tazInstance, &tazInstance->actorInstance,(TBActorNodeInstance *)NULL);
			
	SetFixedDistanceToLookAt(cameraInfo,cameraInfo->defaultFixedDistance);
	
	EnableCameraAngleAboveHorizontal(cameraInfo,RAD(0.0f));
	
	SetCameraSmooth(cameraInfo, defaultPosSmooth, defaultLookatSmooth);

	SetGameState(&gameStatus,(EGameState)gameStatus.lastState,GAMESUBSTATE_NORMAL);

	// CMD: only thing to do now is call the user specified callback
	bossGameContinueInfo->callback(bossGameContinueInfo->context, bossGameContinueInfo->choice);

	// CMD: lastly reset the fov and free up everything
	setFOV(bossGameContinueInfo->fovInit);

	FreeActorInstance(bossGameContinueInfo->sharkPool);
	FreeActorInstance(bossGameContinueInfo->sheDevil);
	for(int i = 2;i >= 0;--i)
	{
		FreeActorInstance(bossGameContinueInfo->sharks[i]);
	}
	SAFE_FREE(bossGameContinueInfo);

/*	StartFadeUp(&gameStatus.player[0].camera->fade, FADETYPE_CIRCLE, 1.5f);
	fade->fadeDownFinishedCallback = NULL;
	fade->fadeUpFinishedCallback = NULL;*/
}

/* --------------------------------------------------------------------------------
   Function : MainBossContinue
   Purpose : Main loop for boss continue screen
   Parameters : 
   Returns : 
   Info : 
*/

void MainBossContinue(void)
{
	applyFOV();

	bdSetViewClipPlanes(videoMode.nearPlane, videoMode.farPlane);
	bdSetViewport(int(videoMode.screens[0].viewport.left), int(videoMode.screens[0].viewport.top), 
					int(videoMode.screens[0].viewport.widthAbs()), int(videoMode.screens[0].viewport.heightAbs()));

	// CMD: update
	MainBossContinueUpdate();

	// CMD: prep
	MainBossContinuePrep();

	// PP: in stereoscopic mode, draw everything once per eye per frame
	for(int i=0; i<videoMode.numDraws; i++)
	{
		// PP: start the stereoscopic draw pass
		if(VFX_effectActive(VFX_STEREO))
		{
			VFX_stereoPreDraw();
		}

		// PP: send the camera - must do this for each eye in stereoscopic mode
		SendCamera(&camera[0]);

		// prepare scene
		bdSetupView();
		bdBeginScene();

		// CMD: draw
		MainBossContinueDraw();
		MainBossContinueDraw2();

		//draw fade
		camera[0].fade.DrawFade();

		// PP: finalise the stereoscopic draw pass
		if(VFX_effectActive(VFX_STEREO))
		{
			VFX_stereoPostDraw();
		}

		bdEndScene();
	}

	if(gameStatus.player[0].controller->l3DebounceChannel->value)
	{
		TakeScreenShot();
	}

	// PP: finish timing the frame; calculate frames per second, frame length, etc.
	finishTimingFrame();
	
	// null time, anything between here and end is not included in the timer
	camera[0].fade.UpdateFade();

	// PP: start timing the frame
	startTimingFrame();
}

/* --------------------------------------------------------------------------------
   Function : MainBossContinueUpdate
   Purpose : Main update for boss continue screen
   Parameters : 
   Returns : 
   Info : 
*/

void MainBossContinueUpdate(void)
{
	int				i;

	if(bossGameContinueInfo)
	{
		AnimateActorInstance(bossGameContinueInfo->sheDevil);
		AnimateActorInstance(bossGameContinueInfo->sharkPool);
		for(i = 2;i >= 0;--i)
		{
			AnimateActorInstance(bossGameContinueInfo->sharks[i]);
		}

		UpdateAttachedObjects(bossGameContinueInfo->sheDevil);
	
		UpdateAnimatingTextures();

		UpdateBossGameContinue();

		UpdateCamera(&camera[0]);
	}
}

/* --------------------------------------------------------------------------------
   Function : MainBossContinuePrep
   Purpose : Main prep for boss continue screen
   Parameters : 
   Returns : 
   Info : 
*/

void MainBossContinuePrep(void)
{
	int			i;

	if(bossGameContinueInfo)
	{
		baPrepActorInstance(&bossGameContinueInfo->sheDevil->actorInstance, TRUE);
		baPrepActorInstance(&bossGameContinueInfo->sharkPool->actorInstance, TRUE);
		for(i = 2;i >= 0;--i)
		{
			baPrepActorInstance(&bossGameContinueInfo->sharks[i]->actorInstance, TRUE);
		}
		PrepAttachedObjects(bossGameContinueInfo->sheDevil);
	}
}

/* --------------------------------------------------------------------------------
   Function : MainBossContinueDraw
   Purpose : Main draw for boss continue screen
   Parameters : 
   Returns : 
   Info : 
*/

void MainBossContinueDraw(void)
{
	if(bossGameContinueInfo)
	{
		// CMD: just to be safe set characters alpha
		bossGameContinueInfo->sheDevil->currentAlpha = 256;
		bossGameContinueInfo->sharkPool->currentAlpha = 256;
		bossGameContinueInfo->sharks[0]->currentAlpha = 256;
		bossGameContinueInfo->sharks[1]->currentAlpha = 256;
		bossGameContinueInfo->sharks[2]->currentAlpha = 256;
		bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, TRUE);
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);

		DrawActorInstances(&map.characterInstances);
		DrawActorInstances(&map.animatedInstances);
		DrawAttachedObjects(bossGameContinueInfo->sheDevil);
	}
}

/* --------------------------------------------------------------------------------
   Function : MainBossContinueDraw2
   Purpose : Main draw for boss continue screen
   Parameters : 
   Returns : 
   Info : 
*/

void MainBossContinueDraw2(void)
{
	if(bossGameContinueInfo)
	{
		DrawActorInstances2(&map.characterInstances);
		DrawActorInstances2(&map.animatedInstances);
		DrawAttachedObjects2(bossGameContinueInfo->sheDevil);

		BookList_draw();
	}
}