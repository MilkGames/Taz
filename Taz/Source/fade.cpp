// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : fade.cpp
//   Purpose : provide functions for fading in and out, as well as different wipes
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "actors.h"
#include "main.h"
#include "display.h"
#include "fade.h"
#include "status.h"
#include "camera.h"
#include "maths.h"
#include "MemCard.h"			// PP: memory card operations & dialogues
#include "tweety.h"
#include "cameracase.h"
#include "characters.h"
#include "music.h"
#include "timer.h"				// PP: timer / time fx stuff
#include "animation.h"
#include "genericai.h"
#include "playerstats.h"
#include "multiplayer.h"

#define CTEXC1	(0.0f)
#define CTEXC2	(1.9f)
#define CTEXC3	(2.0f)

// PP: define FADE_DEBUG to get debug messages about fades
#ifdef PHIL
#define FADE_DEBUG
#endif// PP: def PHIL

/*	--------------------------------------------------------------------------------
	Function 	: FADE::InitFade
	Purpose 	: Initialise the fade structure
	Parameters 	: FADE
	Returns 	: 
	Info 		:
*/

void FADE::InitFade()
{
	int i;
	TBVector tempVector;

	access = false;
	secureFade = false;
	fadeSpeed = 0;
	fadeType = FADETYPE_NORMAL;
 	fadeUpFinishedCallback = NULL;
	fadeDownFinishedCallback = NULL;
	fadeValue = 0;
	status = FADESTATE_OFF;
	sceneToLoad = FADESCENE_NONE;
	swipeAngleDown = 0.0f;
	swipeAngleUp = 0.0f;
	circleFadeTo[X] = 0.0f;
	circleFadeTo[Y] = 0.0f;
	circleFadeTo[Z] = 0.0f;
	circleFadeTo[W] = 0.0f;
	xDoorCRC = 0;
	levelCompleteOnFadeDown = false;

	// TP: Set default colour
	r = g = b = a = 0;

	// setup standard screen fade
	BDVERTEX_SETXYZW(&screenPolyVerts[0], (float)(-videoMode.xScreen/2),(float)(videoMode.yScreen/2),0.0f, 1.0f);
    BDVERTEX_SETRGBA(&screenPolyVerts[0], 0,0,0, 0);
	BDVERTEX_SETXYZW(&screenPolyVerts[1], (float)(videoMode.xScreen/2),(float)(videoMode.yScreen/2),0.0f, 1.0f);
    BDVERTEX_SETRGBA(&screenPolyVerts[1], 0,0,0, 0);
	BDVERTEX_SETXYZW(&screenPolyVerts[3], (float)(videoMode.xScreen/2),(float)(-videoMode.yScreen/2),0.0f, 1.0f);
    BDVERTEX_SETRGBA(&screenPolyVerts[3], 0,0,0, 0);
	BDVERTEX_SETXYZW(&screenPolyVerts[2], (float)(-videoMode.xScreen/2),(float)(-videoMode.yScreen/2),0.0f, 1.0f);
    BDVERTEX_SETRGBA(&screenPolyVerts[2], 0,0,0, 0);

	// setup standard tint fade
	BDVERTEX_SETXYZW(&tintPolyVerts[0], (float)(-videoMode.xScreen/2),(float)(videoMode.yScreen/2),0.0f, 1.0f);
    BDVERTEX_SETRGBA(&tintPolyVerts[0], 0,0,0, 0);
	BDVERTEX_SETXYZW(&tintPolyVerts[1], (float)(videoMode.xScreen/2),(float)(videoMode.yScreen/2),0.0f, 1.0f);
    BDVERTEX_SETRGBA(&tintPolyVerts[1], 0,0,0, 0);
	BDVERTEX_SETXYZW(&tintPolyVerts[3], (float)(videoMode.xScreen/2),(float)(-videoMode.yScreen/2),0.0f, 1.0f);
    BDVERTEX_SETRGBA(&tintPolyVerts[3], 0,0,0, 0);
	BDVERTEX_SETXYZW(&tintPolyVerts[2], (float)(-videoMode.xScreen/2),(float)(-videoMode.yScreen/2),0.0f, 1.0f);
    BDVERTEX_SETRGBA(&tintPolyVerts[2], 0,0,0, 0);

	// calc distance too corner of screen
	tempVector[X] = (float)(videoMode.xScreen/2);
	tempVector[Y] = 0.0f;
	tempVector[Z] = (float)(videoMode.yScreen/2);
	tempVector[W] = 1.0f;

	// PP: TODO?: adjust for widescreen somehow?

	distanceToScreenCorner = bmVectorLen(tempVector);

	// setup swipe
	for (i=0;i<6;i++)
	{
		swipeVerts[i][Y] = 0.0f;
		swipeVerts[i][W] = 1.0f;
	}
	swipeVerts[0][X] = distanceToScreenCorner;
	swipeVerts[0][Z] = -distanceToScreenCorner;
	swipeVerts[1][X] = -distanceToScreenCorner;
	swipeVerts[1][Z] = -distanceToScreenCorner;
	swipeVerts[2][X] = distanceToScreenCorner;
	swipeVerts[2][Z] = distanceToScreenCorner;
	swipeVerts[3][X] = -distanceToScreenCorner;
	swipeVerts[3][Z] = distanceToScreenCorner;
	swipeVerts[4][X] = distanceToScreenCorner;
	swipeVerts[4][Z] = distanceToScreenCorner+40.0f;
	swipeVerts[5][X] = -distanceToScreenCorner;
	swipeVerts[5][Z] = distanceToScreenCorner+40.0f;
	for(i=0;i<6;i++)
	{
		BDVERTEX_SETXYZW(&swipePolyVerts[i],swipeVerts[i][X],swipeVerts[i][Z],0.0f,1.0f);
		BDVERTEX_SETRGBA(&swipePolyVerts[i],0,0,0,127);
	}
	BDVERTEX_SETRGBA(&swipePolyVerts[4], 0,0,0, 0);
	BDVERTEX_SETRGBA(&swipePolyVerts[5], 0,0,0, 0);

	// calculate inner circle
	tempVector[X] = 0.0f;
	tempVector[Y] = 0.0f;
	tempVector[Z] = distanceToScreenCorner;
	tempVector[W] = 1.0f;
	bmVectorCopy(innerCircle[90],tempVector);
	BDVERTEX_SETRGBA(&circlePolyVerts[90*2], 0,0,0, 127);
	BDVERTEX_SETUV(&circlePolyVerts[90*2], 0.0f,CTEXC2);
	for (i=0;i<90;i++)
	{
		bmVectorCopy(innerCircle[i],tempVector);
		bmVectorRotateY(tempVector,tempVector,PI/45.0f);
		BDVERTEX_SETXYZW(&circlePolyVerts[i*2],tempVector[X],tempVector[Z],0.0f,1.0f);
		BDVERTEX_SETRGBA(&circlePolyVerts[i*2], 0,0,0, 127);
		BDVERTEX_SETUV(&circlePolyVerts[i*2], 0.0f,CTEXC2);
	}

	// calculate outer circle
	tempVector[X] = 0.0f;
	tempVector[Y] = 0.0f;
	tempVector[Z] = distanceToScreenCorner + 0.0f;
	tempVector[W] = 1.0f;
	for (i=0;i<90;i++)
	{
		BDVERTEX_SETXYZW(&circlePolyVerts[(i*2)+1],tempVector[X],tempVector[Z],0.0f,1.0f);
	    BDVERTEX_SETRGBA(&circlePolyVerts[(i*2)+1], 0,0,0, 127);
	    BDVERTEX_SETUV(&circlePolyVerts[(i*2)+1], 0.0f,CTEXC1);
		bmVectorRotateY(tempVector,tempVector,PI/45.0f);
	}
	BDVERTEX_SETXYZW(&circlePolyVerts[(90*2)+1],0,distanceToScreenCorner + 0.0f,0.0f,1.0f);
    BDVERTEX_SETRGBA(&circlePolyVerts[(90*2)+1], 0,0,0, 127);
    BDVERTEX_SETUV(&circlePolyVerts[(90*2)+1], 0.0f,CTEXC1);

	SetFadeState();
}


/*	--------------------------------------------------------------------------------
	Function 	: FADE::DrawFade
	Purpose 	: Draw the fade in its current state
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void FADE::DrawFade()
{
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdSetIdentityObjectMatrix();
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	bdSetTexture(0, NULL);

	if (a != 0)
	{
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_SUBTRACTIVE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_SUBTRACTIVE, 1);
		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, tintPolyVerts, 4, BVERTTYPE_SINGLE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	}

	if (status==FADESTATE_OFF) 
	{
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
		bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);

		return;
	}

	switch(fadeType)
	{
	case FADETYPE_NORMAL:
		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, screenPolyVerts, 4, BVERTTYPE_SINGLE);
		break;
	case FADETYPE_CIRCLEWB:
		bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_WRAPU|BDTEXTUREWRAP_WRAPV, FALSE);
		bdSetTexture(0, NULL);

	case FADETYPE_CIRCLE:
		// bizaar
		bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_WRAPU|BDTEXTUREWRAP_WRAPV, FALSE);
		bdSetTexture(0, NULL);

		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, circlePolyVerts, 182, BVERTTYPE_SINGLE);
		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, circleEdgePolyVerts, 182, BVERTTYPE_SINGLE);
		break;
	case FADETYPE_SWIPE:
		bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_WRAPU|BDTEXTUREWRAP_WRAPV, FALSE);
		bdSetTexture(0, NULL);
		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, swipePolyVerts, 6, BVERTTYPE_SINGLE);
		break;
	}

	// PP: draw memory card display
// PP: REMOUT: Now handled by the Book List				MemCard_draw();
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
}


/*  --------------------------------------------------------------------------------
	Function	: FADE::OkToChangeFade
	Purpose		: Check if it is ok to change the current fade settings
	Parameters	: fade structure
	Returns		: true/false
	Info		: 
*/

bool FADE::OkToChangeFade()
{
	if (secureFade) return false;

	return true;
}


/*	--------------------------------------------------------------------------------
	Function 	: FADE::UpdateFade
	Purpose 	: Updates the fade
	Parameters 	: FADE
	Returns 	: 
	Info 		: uses speedRatio to determine how much to advance the fade
*/

void FADE::UpdateFade()
{
	int i;
	TBVector tempVector;
	float	scaleFactor;
	int		colour=0;
	int		tempSceneToLoad;
	ACTORINSTANCE *tempInstance;

	// TP: clear access attrib
	if (access) access=false;

	switch(status)
	{
	case FADESTATE_OFF:
		secureFade = false;
		return;

	case FADESTATE_ON:
		tempSceneToLoad = sceneToLoad;
		secureFade = false;
		sceneToLoad = FADESCENE_NONE;
		if((tempSceneToLoad == FADESCENE_DEATH)||(tempSceneToLoad==FADESCENE_CAGED))
		{
			if (pPlayer && pPlayer->actorStatus)
			{
				// TP: Reset character's position etc.
				ResetCharacterAfterDeath(pPlayer);

				if (tempSceneToLoad==FADESCENE_CAGED)
				{
					if (EnterState(gameStatus.player[0].actorInstance,STATE_CAGED))
					{
						SetupTazInCage(gameStatus.player[0].actorInstance);
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
				}
			}
		}
		if (tempSceneToLoad >= 0)
		{
			ChangeScene(gameStatusFade,tempSceneToLoad);
		}
		if (fadeUpOnCompletion)
		{
			StartFadeUp(fadeType,fadeSpeed);
			if(fadeDownFinishedCallback)
			{
				fadeDownFinishedCallback(this);
				fadeDownFinishedCallback = NULL;
			}
		}
		else
		{			
			if(fadeDownFinishedCallback)
			{
				if(fadeSpeed != 0.0f)
				{
					fadeSpeed = 0.0f;
					fadeDownFinishedCallback(this);
					fadeDownFinishedCallback = NULL;
				}
			}
			else
			{
				fadeSpeed = 0.0f;
			}
		}
		if (levelCompleteOnFadeDown)
		{
			LevelCompleted(gameStatus.player1);
			levelCompleteOnFadeDown = false;
		}
		return;

	case FADESTATE_UP:
	case FADESTATE_DOWN:
		// update fade level
		if (fadeSpeed)
		{
			fadeValue += ((128.0f/fadeSpeed)/60.0f)*speedRatio;
			if (fadeValue < 0.0f)
			{
				if(fadeSpeed != 0.0f)
				{
					if(fadeUpFinishedCallback)
					{
						fadeUpFinishedCallback(this);
						fadeUpFinishedCallback = NULL;
					}
				}
				fadeValue = 0.0f;
				fadeSpeed = 0.0f;
				status = FADESTATE_OFF;
			}
			if (fadeValue > 127.0f)
			{
				fadeValue = 127.0f;
				status = FADESTATE_ON;
			}
		}
		break;
	}

	SetFadeState();
}


/*	--------------------------------------------------------------------------------
	Function 	: FADE::StartFadeDown
	Purpose 	: Start a fade down of the specified type
	Parameters 	: FADE, type, speed in seconds to make complete fade, should the fade be interruptable
	Returns 	: true/false
	Info 		: 
*/

bool FADE::StartFadeDown(EFadeTypes type, float speed, bool secure)
{
	if (!Access()) return false;

#ifdef FADE_DEBUG
	bkPrintf("FADE::StartFadeDown\n");
#endif// PP: def FADE_DEBUG

	fadeSpeed = speed;
	// TP: maybe shouldn't do this fadeValue = 0.0f;
	fadeType = type;
	status = FADESTATE_DOWN;
	fadeUpOnCompletion = FALSE;
	sceneToLoad = FADESCENE_NONE;
	fadeUpFinishedCallback = NULL;
	fadeDownFinishedCallback = NULL;
	secureFade = secure;
	SetFadeState();
	return true;
}


/*  --------------------------------------------------------------------------------
	Function	: FADE::SetFadeUp
	Purpose		: Sets the fade in the off state immediately so there is no fade present
	Parameters	: fade
	Returns		: true/false
	Info		: 
*/

bool FADE::SetFadeUp()
{
	if (!Access()) return false;

	fadeSpeed = 0.0f;
	fadeValue = 0.0f;
	status = FADESTATE_OFF;
	sceneToLoad = FADESCENE_NONE;
	fadeUpOnCompletion = FALSE;
	levelCompleteOnFadeDown = false;
	fadeUpFinishedCallback = NULL;
	fadeDownFinishedCallback = NULL;
	SetFadeState();
	return true;
}


/*  --------------------------------------------------------------------------------
	Function	: FADE::SetFadeDown
	Purpose		: Sets the fade in the on state immediately so the screen will go black
	Parameters	: fade
	Returns		: true/false
	Info		: 
*/

bool FADE::SetFadeDown()
{
	if (!Access()) return false;

	fadeSpeed = 0.0f;
	fadeValue = 127.0f;
	status = FADESTATE_ON;
	sceneToLoad = FADESCENE_NONE;
	fadeUpOnCompletion = FALSE;
	levelCompleteOnFadeDown = false;
	SetFadeState();
	return true;
}


/*	--------------------------------------------------------------------------------
	Function 	: FADE::StartFadeUp
	Purpose 	: Start a fade up of the specified type
	Parameters 	: FADE, type, speed in seconds, is fade interruptable
	Returns 	: true/false
	Info 		: 
*/

bool FADE::StartFadeUp(EFadeTypes type, float speed, bool secure)
{
	if (!Access()) return false;

#ifdef FADE_DEBUG
	bkPrintf("FADE::StartFadeUp\n");
#endif// PP: def FADE_DEBUG

	fadeSpeed = -speed;
	// TP: maybe shouldn't do this fadeValue = 127.0f;
	fadeType = type;
	status = FADESTATE_UP;
	fadeUpOnCompletion = FALSE;
	secureFade = secure;
	SetFadeState();
	return true;
}


/*	--------------------------------------------------------------------------------
	Function 	: FADE::FadeAndChangeScene
	Purpose 	: Fades down then frees the current scene and load the new one, then fade up
	Parameters 	: FADE, type, gameStatus structure, type of fade, scene number or special code, speed, xDoor to use, fadeUpOnCompletion
	Returns 	: true if ok/ false if problem
	Info 		: calls FreeScene and InitialiseScene when fade on
*/

bool FADE::FadeAndChangeScene(GAMESTATUS *gameStatus, EFadeTypes type, int scene, float speed, uint32 xDoorCRC, bool fadeUpOnCompletion)
{
	if (!Access()) return false;

	// PP: cancel time effects and restore normal game speed - otherwise the fade could happen at the wrong speed
	TimeFX_setGlobalRate(1.0f);

#ifndef CONSUMERDEMO
	if (scene > FADESCENE_DEATH)
	{
		if (scene<0)
		{
	 		musicSystem.ChooseMusic(map.sceneNumber,MUSICTYPE_ALL,MUSICSTATE_STOP);
		}
	}
#endif

	ClosePlayerStats(&gameStatus->player[0]);
	
	if (gameStatus->player1 && gameStatus->player1->actorStatus->currentState != STATE_NET && (scene != -2 && map.sceneNumber < SCENE_MUSEUMVEHICLERACE))
		BookList_closeAll();

	if ((scene>=0)||(scene==FADESCENE_CAGED))
	{
		// TP: do a secure fade that can't be interrupted
		StartFadeDown(type, speed, true);
	}
	else
	{
		StartFadeDown(type,speed);
	}
	sceneToLoad = scene;
	gameStatusFade = gameStatus;
	this->fadeUpOnCompletion = fadeUpOnCompletion;
	this->xDoorCRC = xDoorCRC;
	fadeUpFinishedCallback = NULL;
	fadeDownFinishedCallback = NULL;

	if (gameStatus->multiplayer.numScreens == 2)
	{
		camera[1].fade.StartFadeDown(type, speed);
		camera[1].fade.sceneToLoad = FADESCENE_NONE;
		camera[1].fade.gameStatusFade = gameStatus;
	}

	return true;
}


/*	--------------------------------------------------------------------------------
	Function 	: FADE::FinishedFade
	Purpose 	: has the fade finished
	Parameters 	: FADE
	Returns 	: TRUE / FALSE
	Info 		: 
*/

int FADE::FinishedFade()
{
	if (fadeUpOnCompletion)
	{
		if (status==FADESTATE_OFF) return TRUE;
	}
	else
	{
		if ((status==FADESTATE_OFF)||(status==FADESTATE_ON)) return TRUE;
	}
	return FALSE;
}


/*	--------------------------------------------------------------------------------
	Function 	: FADE::SetFadeState
	Purpose 	: update the fade structure with the current state
	Parameters 	: FADE
	Returns 	: 
	Info 		: 
*/

bool FADE::SetFadeState()
{
	int i;
	TBVector tempVector;
	float	scaleFactor;
	int		colour=0;

	switch(fadeType)
	{
	case FADETYPE_NORMAL:
		BDVERTEX_SETRGBA(&screenPolyVerts[0], r,g,b, (int)(fadeValue));
		BDVERTEX_SETRGBA(&screenPolyVerts[1], r,g,b, (int)(fadeValue));
		BDVERTEX_SETRGBA(&screenPolyVerts[2], r,g,b, (int)(fadeValue));
		BDVERTEX_SETRGBA(&screenPolyVerts[3], r,g,b, (int)(fadeValue));
		break;
	case FADETYPE_CIRCLEWB:
		colour = 127;
	case FADETYPE_CIRCLE:
		for (i=0;i<91;i++)
		{
			// update outer circle colour
			BDVERTEX_SETRGBA(&circlePolyVerts[(i*2)+1], colour,colour,colour, 127);
			tempVector[X] = innerCircle[i][X];
			tempVector[Y] = 0.0f;
			tempVector[Z] = innerCircle[i][Z];
			tempVector[W] = 1.0f;
			scaleFactor = (127.0f-fadeValue) / 127.0f;
			bmVectorScaleToLength(tempVector,tempVector,distanceToScreenCorner*scaleFactor);
			// add circle offset to tempVector
			tempVector[X] += circleFadeTo[X]*(1.0f-scaleFactor);
			tempVector[Z] += circleFadeTo[Z]*(1.0f-scaleFactor);
			tempVector[W] = 1.0f;
			BDVERTEX_SETXYZW(&circlePolyVerts[i*2],tempVector[X],tempVector[Z],0.0f,1.0f);
			BDVERTEX_SETRGBA(&circlePolyVerts[i*2], colour,colour,colour, 127);
			BDVERTEX_SETXYZW(&circleEdgePolyVerts[(i*2)+1],tempVector[X],tempVector[Z],0.0f,1.0f);
			BDVERTEX_SETRGBA(&circleEdgePolyVerts[(i*2)+1], colour,colour,colour, 127);
			BDVERTEX_SETUV(&circleEdgePolyVerts[(i*2)+1], 0.0f,CTEXC2);
			tempVector[X] = innerCircle[i][X];
			tempVector[Y] = 0.0f;
			tempVector[Z] = innerCircle[i][Z];
			tempVector[W] = 1.0f;
			scaleFactor = (124.0f-fadeValue) / 127.0f;
			bmVectorScaleToLength(tempVector,tempVector,distanceToScreenCorner*scaleFactor);
			// add circle offset to tempVector
			tempVector[X] += circleFadeTo[X]*(1.0f-scaleFactor);
			tempVector[Z] += circleFadeTo[Z]*(1.0f-scaleFactor);
			tempVector[W] = 1.0f;
			BDVERTEX_SETXYZW(&circleEdgePolyVerts[i*2],tempVector[X],tempVector[Z],0.0f,1.0f);
			BDVERTEX_SETRGBA(&circleEdgePolyVerts[i*2], colour,colour,colour, 0);
		    BDVERTEX_SETUV(&circleEdgePolyVerts[i*2], 0.0f,CTEXC3);
		}
		break;
	case FADETYPE_SWIPE:
		for (i=0;i<6;i++)
		{
			tempVector[X] = 0.0f;
			tempVector[Y] = 0.0f;
			tempVector[Z] = 1.0f;
			tempVector[W] = 1.0f;
			scaleFactor = (127.0f-fadeValue) / 127.0f;
			bmVectorScaleToLength(tempVector,tempVector,(-((distanceToScreenCorner*2.0f)+40.0f))*scaleFactor);
			tempVector[X] += swipeVerts[i][X];
			tempVector[Z] += swipeVerts[i][Z];
			tempVector[W] = 1.0f;
			if (status==FADESTATE_DOWN)
			{
				bmVectorRotateY(tempVector,tempVector,swipeAngleDown);
			}
			else
			{
				bmVectorRotateY(tempVector,tempVector,swipeAngleUp);
			}
			BDVERTEX_SETXYZW(&swipePolyVerts[i],tempVector[X],tempVector[Z],0.0f,1.0f);
		}
		break;
	}

	return true;

}


/*	--------------------------------------------------------------------------------
	Function 	: FADE::SetSwipeDirection
	Purpose 	: set the current direction for the swipe.
	Parameters 	: FADE, angle in rads 0 - vertical for fade down, angle for fade up
	Returns 	: nothing
	Info 		: 
*/

bool FADE::SetSwipeDirection(float fadeDownAngle,float fadeUpAngle)
{
	if (!Access()) return false;

	swipeAngleUp = fadeUpAngle;
	swipeAngleDown = fadeDownAngle;
	return true;
}


/*	--------------------------------------------------------------------------------
	Function 	: FADE::SetCircleFadeTo
	Purpose 	: sets the screen coordinate to fade the circle onto
	Parameters 	: FADE, x and y coords
	Returns 	: nothing
	Info 		: 
*/

bool FADE::SetCircleFadeTo(float x,float y)
{
	if (!Access()) return false;

	circleFadeTo[X] = x;
	circleFadeTo[Y] = 0.0f;
	circleFadeTo[Z] = y;
	circleFadeTo[W] = 1.0f;

	return true;
}


/*	--------------------------------------------------------------------------------
	Function 	: FADE::GetFadeState
	Purpose 	: returns the current state of the fade
	Parameters 	: FADE
	Returns 	: 
	Info 		: 
*/

int FADE::GetFadeState()
{
	return status;
}


/*	--------------------------------------------------------------------------------
	Function 	: FADE::SetFadeFinishedCallback
	Purpose 	: sets the function to be called when a fade is finished
	Parameters 	: FADE
	Returns 	: true/false
	Info 		: 
*/

bool FADE::SetFadeUpFinishedCallback(FadeFinishedCallback callback)
{
	if (!Access()) return false;

	fadeUpFinishedCallback = callback;
	return true;
}


/*	--------------------------------------------------------------------------------
	Function 	: SetFadeFinishedCallback
	Purpose 	: sets the function to be called when a fade is finished
	Parameters 	: FADE
	Returns 	: true/false
	Info 		: 
*/

bool FADE::SetFadeDownFinishedCallback(FadeFinishedCallback callback)
{
	if (!Access()) return false;

	fadeDownFinishedCallback = callback;
	return true;
}


/*	--------------------------------------------------------------------------------
	Function 	: FADE::SetFadeTint
	Purpose 	: Sets the current colour and alpha for the camera
	Parameters 	: FADE, r, g, b, a
	Returns 	: 
	Info 		: 
*/

bool FADE::SetFadeTint(uchar r, uchar g, uchar b, uchar a)
{
	if (!Access()) return false;

	r = r;
	g = g;
	b = b;
	a = a;

    BDVERTEX_SETRGBA(&tintPolyVerts[0], r,g,b, a);
    BDVERTEX_SETRGBA(&tintPolyVerts[1], r,g,b, a);
    BDVERTEX_SETRGBA(&tintPolyVerts[3], r,g,b, a);
    BDVERTEX_SETRGBA(&tintPolyVerts[2], r,g,b, a);

	return true;
}

/*  --------------------------------------------------------------------------------
	Function	: FADE::SetPlayer
	Purpose		: Set the player that this fade is associated with
	Parameters	: actorInstance of player
	Returns		: 
	Info		: 
*/

bool FADE::SetPlayer(ACTORINSTANCE *player)
{
	if (!Access()) return false;

	pPlayer = player;
	return true;
}

/*  --------------------------------------------------------------------------------
	Function	: FADE::SetSceneToLoad
	Purpose		: Set the scene to be loaded when the fade down is complete
	Parameters	: scene number
	Returns		: true/false
	Info		: 
*/
bool FADE::SetSceneToLoad(int scene)
{
	if (!Access()) return false;

	sceneToLoad = scene;
	return true;
}

/*  --------------------------------------------------------------------------------
	Function	: FADE::SetGameStatus
	Purpose		: Set the current game status structure the camera should refer to 
	Parameters	: gameStatus
	Returns		: true/false
	Info		: 
*/
bool FADE::SetGameStatus(TAG_GAMESTATUS *gameStatus)
{
	if (!Access()) return false;

	gameStatusFade = gameStatus;
	return true;
}

/*  --------------------------------------------------------------------------------
	Function	: FADE::LevelCompleteOnFadeDown
	Purpose		: Time a level complete call with the completion of the fade down
	Parameters	: state = true/false
	Returns		: true/false
	Info		: 
*/
bool FADE::LevelCompleteOnFadeDown(bool state)
{
	if (!Access()) return false;

	levelCompleteOnFadeDown = state;
	return true;
}

/*  --------------------------------------------------------------------------------
	Function	: FADE::GetPlayer
	Purpose		: Return the player that this fade is associated with
	Parameters	: 
	Returns		: ACTORINSTANCE of player
	Info		: 
*/

ACTORINSTANCE *FADE::GetPlayer()
{
	return pPlayer;
}


/*  --------------------------------------------------------------------------------
	Function	: FADE::FadeUpOnCompletion
	Purpose		: Fade up after fade down is complete
	Parameters	: true/false
	Returns		: true/false
	Info		: 
*/
bool FADE::FadeUpOnCompletion(bool state)
{
	if (!Access()) return false;

	fadeUpOnCompletion = state;
	return true;
}


/*  --------------------------------------------------------------------------------
	Function	: FADE::SetFadeStatus
	Purpose		: Set the current status of the fade
	Parameters	: state
	Returns		: true/false
	Info		: 
*/
bool FADE::SetFadeStatus(EFadeStates state)
{
	if (!Access()) return false;

	status = state;
	return true;
}


/*  --------------------------------------------------------------------------------
	Function	: FADE::GetFadeValue
	Purpose		: Return the current fade value
	Parameters	: 
	Returns		: fadeValue
	Info		: 
*/

float FADE::GetFadeValue()
{
	return fadeValue;
}


/*  --------------------------------------------------------------------------------
	Function	: FADE::Access
	Purpose		: Are changes to the fade info currently allowed
	Parameters	: 
	Returns		: true/false
	Info		: 
*/
bool	FADE::Access()
{
	if (!access)
	{
		// TP: if we could lock it just for this command then do so
		if (Lock()) 
		{
			// TP: dont change global attrib, its just for this command only
			access = false;
			return true;
		}
	}
	
	if (!access) bkPrintf("*** WARNING *** Access to unlocked fade.\n");
	
	return access;
}


/*  --------------------------------------------------------------------------------
	Function	: GetFadeSpeed
	Purpose		: Return the current fade speed
	Parameters	: 
	Returns		: fadeSpeed
	Info		: 
*/

float FADE::GetFadeSpeed()
{
	return fadeSpeed;
}


/*  --------------------------------------------------------------------------------
	Function	: GetSceneToLoad
	Purpose		: Return the scene to be loaded
	Parameters	: 
	Returns		: scene number
	Info		: 
*/

int FADE::GetSceneToLoad()
{
	return sceneToLoad;
}


/*  --------------------------------------------------------------------------------
	Function	: GetFadeType
	Purpose		: return the current fade type
	Parameters	: 
	Returns		: EFadeTypes
	Info		: 
*/

EFadeTypes FADE::GetFadeType()
{
	return fadeType;
}


/*  --------------------------------------------------------------------------------
	Function	: FADE::Lock
	Purpose		: Must be called before using any of the fade functions
	Parameters	: 
	Returns		: true if fade is locked and available / false if fade is not available for use
	Info		: 
*/

bool FADE::Lock()
{
	if (OkToChangeFade())
	{
		access = true;
	}
	return access;
}


/*  --------------------------------------------------------------------------------
	Function	: FADE::Unlock
	Purpose		: Called to unlock a fade, fade functions 
	Parameters	: 
	Returns		: 
	Info		: 
*/

void FADE::Unlock()
{
	access = false;
}


/*  --------------------------------------------------------------------------------
	Function	: FADE::SecureFade
	Purpose		: Tells the fade handler that no other fade instructions should be processed during this fade
				:	NOTE: this is set automatically if the fadeandchangescene function is used to change level or cage taz
	Parameters	: 
	Returns		: 
	Info		: 
*/

void FADE::SecureFade()
{
	if (!Access()) return;

	secureFade = true;
}


/*  --------------------------------------------------------------------------------
	Function	: FADE::RemoveSecureFade
	Purpose		: Takes control of the fade regardless of the secure setting
	Parameters	: 
	Returns		: 
	Info		: ***WARNING*** This function should only be used by the demo modes to exit on a start press
*/

bool FADE::RemoveSecureFade()
{
	secureFade = false;
	return true;
}
