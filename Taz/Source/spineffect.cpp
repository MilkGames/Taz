// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : sparks.cpp
//   Purpose : functions to produce sparks allong a path
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "status.h"
#include "main.h"
#include "maths.h"
#include "physics.h"
#include "control.h"
#include "characters.h"
#include "textures.h"
#include "holysmoke.h"
#include "ZooBoss.h"		// PP: for zoo boss spin dust special-case// PP: Zoo Boss - Gossamer vs. Taz in an Elephant Pong showdown
#include "endgamecredits.h"
#include "spineffect.h"

SPINEFFECTINFO		g_CutSceneSpinEffectInfo;

/* --------------------------------------------------------------------------------
   Function : CreateSpinEffectForActor
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

SPINEFFECTINFO *CreateSpinEffectForActor(uint32 playerNo)
{
	SPINEFFECTINFO	*spinInfo;
	int				i, colour[3];
	float			currentAngle;
	float			scaleFactor, randomTilt;
	TBMatrix		scale, localTrans;

	if(playerNo == -1) return NULL;

	spinInfo = (SPINEFFECTINFO*)ZALLOC(sizeof(SPINEFFECTINFO));
	ASSERTM(spinInfo, "no memory left to create spin effect");

	currentAngle = 0.0f;

	spinInfo->koikTimer = 0.0f;
	spinInfo->rumbleHandle = -1;
	spinInfo->spinUpdated = FALSE;

	for(i=0;i<SPINFX_NUMSTRIPS;i++)
	{
		spinInfo->polys[i].currentAngle = RAD(currentAngle);
		currentAngle += RAD(360.0f) / SPINFX_NUMSTRIPS;
		spinInfo->polys[i].angVelocity = RAD(SPINFX_MINANGVEL + (float)(bmRand()%(int)(SPINFX_MINANGVEL*2.0f)));

		spinInfo->polys[i].height = (float)(bmRand()%200);
		spinInfo->polys[i].radius = spinInfo->polys[i].height*(0.4f*(1.0f+(((float)(bmRand()%25)/100.0f)))) + METERTOUNIT(0.1f);

		randomTilt = RAD((float)((bmRand()%30)-15));
		spinInfo->polys[i].radiusIdle = (float)(bmRand()%30);

		spinInfo->polys[i].texture = textureSpin;
		spinInfo->polys[i].invisibleTexture = textureInvisSpin;

		scaleFactor = spinInfo->polys[i].height/200.0f*1.5f;
		bmMatScale(scale, 1.0f, SPINFX_YSCALE*(0.05f+((float)(bmRand()%4)/100.0f)), 1.0f);
		bmMatTranslate(localTrans, 0.0f, spinInfo->polys[i].height, 0.0f);
		bmMatMultiply(spinInfo->polys[i].scaleAndTrans, localTrans, scale);
		bmMatXRotation(spinInfo->polys[i].randomTilt, randomTilt);

		//set vert info
		switch(playerNo)
		{
		case 0:
			if(bmRand()%3)
			{
				colour[0] = colour[1] = colour[2] = bmRand()%128;
			}
			else
			{
				colour[0] = 43; colour[1] = 27; colour[2] = 7;						// CMD: poo brown
			}
			break;
		case 1:
			switch(bmRand()%4)
			{
			case 0:
			case 1:
				colour[0] = colour[1] = colour[2] = bmRand()%128;
				break;
			case 2:
				colour[0] = 43; colour[1] = 27; colour[2] = 7;						// CMD: poo brown
				break;
			case 3:
				colour[0] = 84; colour[1] = 15; colour[2] = 127;					// CMD: purple for she devil
				break;
			}
			break;
		}
		CreateSpinEffectStrips(&spinInfo->polys[i], colour, SPINFX_SEGMENTS, SPINFX_SEGMENTLEN);
	}
	return spinInfo;
}

/* --------------------------------------------------------------------------------
   Function : CreateSpinEffectForCutScene
   Purpose : initialises the spin effect used in the cut scenes
   Parameters : none, the spineffectiinfo is global, and there can only be one
   Returns : 
   Info : 
*/

void CreateSpinEffectForCutScene(void)
{
	int				i, colour[3];
	float			currentAngle;
	float			scaleFactor, randomTilt;
	TBMatrix		scale, localTrans;

	currentAngle = 0.0f;

	g_CutSceneSpinEffectInfo.koikTimer = 0.0f;
	g_CutSceneSpinEffectInfo.spinUpdated = FALSE;

	for(i=0;i<SPINFX_NUMSTRIPS;i++)
	{
		g_CutSceneSpinEffectInfo.polys[i].currentAngle = RAD(currentAngle);
		currentAngle += RAD(360.0f) / SPINFX_NUMSTRIPS;
		g_CutSceneSpinEffectInfo.polys[i].angVelocity = RAD(SPINFX_MINANGVEL + (float)(bmRand()%(int)(SPINFX_MINANGVEL*2.0f)));

		g_CutSceneSpinEffectInfo.polys[i].height = (float)(bmRand()%200);
		g_CutSceneSpinEffectInfo.polys[i].radius = g_CutSceneSpinEffectInfo.polys[i].height*(0.4f*(1.0f+(((float)(bmRand()%25)/100.0f)))) + METERTOUNIT(0.1f);

		randomTilt = RAD((float)((bmRand()%30)-15));
		g_CutSceneSpinEffectInfo.polys[i].radiusIdle = (float)(bmRand()%30);

		g_CutSceneSpinEffectInfo.polys[i].texture = textureSpin;
		g_CutSceneSpinEffectInfo.polys[i].invisibleTexture = textureInvisSpin;

		scaleFactor = g_CutSceneSpinEffectInfo.polys[i].height/200.0f*1.5f;
		bmMatScale(scale, 1.0f, (0.05f+((float)(bmRand()%4)/100.0f)), 1.0f);
		bmMatTranslate(localTrans, 0.0f, g_CutSceneSpinEffectInfo.polys[i].height, 0.0f);
		bmMatMultiply(g_CutSceneSpinEffectInfo.polys[i].scaleAndTrans, localTrans, scale);
		bmMatXRotation(g_CutSceneSpinEffectInfo.polys[i].randomTilt, randomTilt);

		//set vert info
		if(bmRand()%3)
		{
			colour[0] = colour[1] = colour[2] = bmRand()%128;
		}
		else
		{
			colour[0] = 43; colour[1] = 27; colour[2] = 7;						//poo brown
		}
		CreateSpinEffectStrips(&g_CutSceneSpinEffectInfo.polys[i], colour, SPINFX_SEGMENTS, SPINFX_SEGMENTLEN);
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateSpinEffectForActor
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateSpinEffectForActor(ACTORINSTANCE *actorInstance, float t)
{
	TBMatrix		objectRot, tilt, correction, result;
	TBMatrix		localTrans, worldTrans, temp, tempTwo;
	int				i, amp;
	float			currentAngle, angleIncrement;
	float			radInc, gameTime, heightDelta;
	SPINEFFECTINFO	*spinInfo;

	if(!(spinInfo = actorInstance->characterInfo->spinInfo)) return;

//	if((actorInstance->actorStatus->currentState != STATE_SPIN)
//		&& (actorInstance->actorStatus->currentState != STATE_ATLASSPIN)
//		&& (actorInstance->actorStatus->frontEndSubState != STATE_SPIN)) return;

	bmQuatToMatrix(temp, actorInstance->actorInstance.orientation);
//	bmMatYRotation(correction, RAD(90.0f));
	bmMatYRotation(correction, RAD(180.0f*gameStatus.gameTime));
	bmMatMultiply(tilt, temp, correction);
	bmMatTranslate(worldTrans, actorInstance->actorInstance.position[0],actorInstance->actorInstance.position[1],actorInstance->actorInstance.position[2]);

	//look for bounce
	if((actorInstance->actorStatus->timeWithoutWallCollisionLastFrame > 0.2f)&&(actorInstance->actorBehaviour->physics.coll))
	{
		spinInfo->koikTimer = SPINFX_KOIKTIME;
		if(actorInstance->actorStatus->currentState == STATE_SPIN)
		{
			if((amp = (int)((actorInstance->actorStatus->timeWithoutWallCollisionLastFrame-0.2f)*150.0f)+100) > 250) amp = 250;
			spinInfo->rumbleHandle = RequestRumbleEffect(gameStatus.player[actorInstance->playerNo].controller, amp, 0.05f, (SPINFX_KOIKTIME-0.1f), SPINFX_KOIKTIME, 0.0f, WAVEFORM_NORMAL, 0.0f, 0.0f, 0);
		}
	}
	if(spinInfo->koikTimer > 0.0f)
	{
		heightDelta = METERTOUNIT(0.15f)*bmSin(spinInfo->koikTimer/SPINFX_KOIKTIME*RAD(720.0f));
		// update polys
		for(i=0;i<SPINFX_NUMSTRIPS;i++)
		{
			//first increment angle
			spinInfo->polys[i].currentAngle += spinInfo->polys[i].angVelocity*t;
			currentAngle = spinInfo->polys[i].currentAngle;

			angleIncrement = spinInfo->polys[i].angVelocity*0.5f*t;
			bmMatMultiply(temp, tilt, spinInfo->polys[i].randomTilt);
			bmMatMultiply(result, worldTrans, temp);

			bmMatTranslate(temp,0.0f, (heightDelta*spinInfo->polys[i].height/200.0f), 0.0f);

			if((currentAngle -= angleIncrement) > RAD(360.0f))
			{
				currentAngle -= RAD(360.0f);
			}

			//calculate local to world transfom matrix
			bmMatYRotation(objectRot, currentAngle);

			//(dest, src1, src2); src2 takes effect first
			bmMatMultiply(tempTwo, temp, spinInfo->polys[i].scaleAndTrans);
			bmMatMultiply(temp, objectRot, tempTwo);
			bmMatMultiply(spinInfo->polys[i].localToWorld, result, temp);
		}
		spinInfo->koikTimer -= t;
	}
	else
		if(actorInstance->actorBehaviour->physics.speed > METERTOUNIT(0.3f))
		{
			// update polys
			for(i=0;i<SPINFX_NUMSTRIPS;i++)
			{
				//first increment angle
				spinInfo->polys[i].currentAngle += spinInfo->polys[i].angVelocity*t;
				currentAngle = spinInfo->polys[i].currentAngle;

				angleIncrement = spinInfo->polys[i].angVelocity*0.5f*t;
				bmMatMultiply(temp, tilt, spinInfo->polys[i].randomTilt);
				bmMatMultiply(result, worldTrans, temp);

				if((currentAngle -= angleIncrement) > RAD(360.0f))
				{
					currentAngle -= RAD(360.0f);
				}

				//calculate local to world transfom matrix
				bmMatYRotation(objectRot, currentAngle);

				//(dest, src1, src2); src2 takes effect first
				bmMatMultiply(temp, objectRot, spinInfo->polys[i].scaleAndTrans);
				bmMatMultiply(spinInfo->polys[i].localToWorld, result, temp);
			}
		}
		else
		{
			gameTime = bmSin(gameStatus.gameTime*3.0f);
			// update polys
			for(i=0;i<SPINFX_NUMSTRIPS;i++)
			{
				//first increment angle
				spinInfo->polys[i].currentAngle += spinInfo->polys[i].angVelocity*t;
				currentAngle = spinInfo->polys[i].currentAngle;

				//update radiusIdle
				radInc = spinInfo->polys[i].radiusIdle*gameTime;

				bmMatTranslate(tempTwo, 0.0f, 0.0f, radInc);
				bmMatMultiply(localTrans, tempTwo, spinInfo->polys[i].scaleAndTrans);

				angleIncrement = spinInfo->polys[i].angVelocity*t;
				bmMatMultiply(temp, tilt, spinInfo->polys[i].randomTilt);
				bmMatMultiply(result, worldTrans, temp);

				if((currentAngle -= angleIncrement) > RAD(360.0f))
				{
					currentAngle -= RAD(360.0f);
				}

				//calculate local to world transfom matrix
				bmMatYRotation(objectRot, currentAngle);
				
				//(dest, src1, src2); src2 takes effect first
				bmMatMultiply(temp, objectRot, localTrans);
				bmMatMultiply(spinInfo->polys[i].localToWorld, result, temp);
			}
		}
	spinInfo->spinUpdated = gameStatus.multiplayer.numScreens
							* videoMode.numDraws;				// PP: gotter draw the spin for each eye in stereoscopic mode
}

/* --------------------------------------------------------------------------------
   Function : UpdateSpinEffectForActor
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateCutSceneSpinEffect(TBActorInstance *actorInstance, TBActorNodeInstance *node)
{
	TBMatrix		objectRot, tilt, correction, result;
	TBMatrix		localTrans, worldTrans, temp, tempTwo;
	TBVector		nodePos, decPos, decScale;
	TBQuaternion	decOrientation;
	int				i;
	float			currentAngle, angleIncrement;
	float			radInc, gameTime, heightDelta;

	bmMatMultiply(tilt, actorInstance->objectToWorld, node->nodeToLocalWorld);
	bmMatDecompose(tilt, decPos, decScale, decOrientation);
	bmQuatToMatrix(temp, decOrientation);

	bmMatYRotation(correction, RAD(180.0f*gameStatus.gameTime));
	bmMatMultiply(tilt, temp, correction);
	baGetNodesWorldPosition(actorInstance, node, nodePos);
	bmMatTranslate(worldTrans, nodePos[0],nodePos[1],nodePos[2]);

	// update polys
	for(i=0;i<SPINFX_NUMSTRIPS;i++)
	{
		//first increment angle
		g_CutSceneSpinEffectInfo.polys[i].currentAngle += g_CutSceneSpinEffectInfo.polys[i].angVelocity*fTime;
		currentAngle = g_CutSceneSpinEffectInfo.polys[i].currentAngle;

		angleIncrement = g_CutSceneSpinEffectInfo.polys[i].angVelocity*0.5f*fTime;
		bmMatMultiply(temp, tilt, g_CutSceneSpinEffectInfo.polys[i].randomTilt);
		bmMatMultiply(result, worldTrans, temp);

		if((currentAngle -= angleIncrement) > RAD(360.0f))
		{
			currentAngle -= RAD(360.0f);
		}

		//calculate local to world transfom matrix
		bmMatYRotation(objectRot, currentAngle);

		//(dest, src1, src2); src2 takes effect first
		bmMatMultiply(temp, objectRot, g_CutSceneSpinEffectInfo.polys[i].scaleAndTrans);
		bmMatMultiply(g_CutSceneSpinEffectInfo.polys[i].localToWorld, result, temp);
	}

#define NORMAL_CUTSCENE_SPIN_SMOKE \
		CreateImpactClouds(nodePos, gameStatus.windVelocity, NULL, METERTOUNIT(0.2f), 0.75f, 0.0f, SMOKE_SPIN);

#ifndef CONSUMERDEMO
	// PP: Special case, 'scuse me
	if(map.sceneNumber == SCENE_ZOOBOSS)
	{
		// PP: on zoo boss, if taz is in the court, kick up pong dust
		if(!ZooBoss_createCutsceneSpinDust(nodePos))
		{
			NORMAL_CUTSCENE_SPIN_SMOKE;
		}
	}
	else
#endif
	{
		NORMAL_CUTSCENE_SPIN_SMOKE;
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawSpinEffectForActor
   Purpose :  SPINNING ... over and over ... SPINNING
   Parameters : 
   Returns : 
   Info : 
*/

void DrawSpinEffectForActor(ACTORINSTANCE *actorInstance)
{
	int				i;
	SPINEFFECTINFO	*spinInfo;

	if(!(spinInfo = actorInstance->characterInfo->spinInfo)) return;

	switch(GetCurrentGameState(&gameStatus))
	{
#ifndef CONSUMERDEMO
	case GAMESTATE_CREDITS:
		if((!creditsInfo)||(creditsInfo->tazState != CREDITS_TAZ_EXITSTAGELEFT)) return;
		break;
#endif
	case GAMESTATE_PAUSE:
	case GAMESTATE_CUSTOMPAUSE:
	case GAMESTATE_FADING:
		if((actorInstance->actorStatus->currentState != STATE_SPIN)
			&& (actorInstance->actorStatus->currentState != STATE_ATLASSPIN)
			&& (actorInstance->actorStatus->frontEndSubState != STATE_SPIN)) return;
		break;
	default:
		if((actorInstance->actorStatus->currentState != STATE_SPIN)
			&& (actorInstance->actorStatus->currentState != STATE_ATLASSPIN)
			&& (actorInstance->actorStatus->frontEndSubState != STATE_SPIN)) return;
		if(spinInfo->spinUpdated <= 0) return;
		break;
	}

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, 1);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, 1);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, TRUE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_WRAPU|BDTEXTUREWRAP_CLAMPV, 0);
	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_WRAPU|BDTEXTUREWRAP_CLAMPV, 1);

#if BPLATFORM != PS2 && BPLATFORM != GAMECUBE // IH: i.e. if on XBox or PC
	bdDisableTextureMatrix(0);
#endif

	switch(actorInstance->characterInfo->powerUpInfo.state)
	{
	case RENDERSTATE_LINEONLY:
		bdSetTexture(0, spinInfo->polys[0].invisibleTexture);
		break;
	default:
		bdSetTexture(0, spinInfo->polys[0].texture);
		break;
	}
	// draw spin polys
	for(i=0;i<SPINFX_NUMSTRIPS;i++)
	{
		bdSetObjectMatrix(spinInfo->polys[i].localToWorld);
		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,spinInfo->polys[i].verts, ((SPINFX_SEGMENTS*2)+2), BVERTTYPE_SINGLE);
	}

	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, TRUE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, 1);
	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_CLAMPU|BDTEXTUREWRAP_WRAPV, 0);
	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_CLAMPU|BDTEXTUREWRAP_WRAPV, 1);

	spinInfo->spinUpdated--;
}

/* --------------------------------------------------------------------------------
   Function : DrawSpinEffectForActor
   Purpose :  SPINNING ... over and over ... SPINNING
   Parameters : 
   Returns : 
   Info : 
*/

void DrawCutSceneSpinEffect(void)
{
	int				i;

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_WRAPU|BDTEXTUREWRAP_WRAPV, 0);
	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_WRAPU|BDTEXTUREWRAP_WRAPV, 0);

	bdSetTexture(0, g_CutSceneSpinEffectInfo.polys[0].texture);

	// draw spin polys
	for(i=0;i<SPINFX_NUMSTRIPS;i++)
	{
		bdSetObjectMatrix(g_CutSceneSpinEffectInfo.polys[i].localToWorld);
		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,g_CutSceneSpinEffectInfo.polys[i].verts, ((SPINFX_SEGMENTS*2)+2), BVERTTYPE_SINGLE);
	}

	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_CLAMPU|BDTEXTUREWRAP_CLAMPV, 0);
	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_CLAMPU|BDTEXTUREWRAP_CLAMPV, 1);
}

/* --------------------------------------------------------------------------------
   Function : CreateSpinEffectStrips
   Purpose : Creates strips of polys for use in spin effect
   Parameters : 
   Returns : 
   Info : 
*/

void CreateSpinEffectStrips(SPINPOLYS *polys, int colour[3], int noofSegments, float segmentLength)
{
	int			alpha, i;
	float		angle, u, v;
	TBVector	pos;

	angle = 0.0f;
	u = 0.0f;
	alpha = 0; //100;

	for(i=0;i<((SPINFX_SEGMENTS*2)+2);i++)
	{
		if(i%2)
		{
			v = 0.0f;
			pos[1] = 0.5f;
		}
		else
		{
			if(i)
			{
				alpha += (int)(200.0f/noofSegments);
				angle += segmentLength/noofSegments;
				u -= 1.0f/SPINFX_SEGMENTS;
			}
			v = 1.0f;
			pos[1] = -0.5f;
		}

		pos[0] = polys->radius*bmSin(angle);
		pos[2] = polys->radius*bmCos(angle);

		BDVERTEX_SETXYZW(&polys->verts[i], pos[0], METERTOUNIT(pos[1]), pos[2], 1.0f);
		bmVectorNorm(pos, pos);
		BDVERTEX_SETNORMAL(&polys->verts[i], pos[0], pos[1], pos[2]);
		BDVERTEX_SETUV(&polys->verts[i], u, v);
		BDVERTEX_SETRGBA(&polys->verts[i], colour[0], colour[1], colour[2], 0);
		if(alpha < 200)
		{
			BDVERTEX_SETA(&polys->verts[i], alpha);
		}
		else
		{
			BDVERTEX_SETA(&polys->verts[i], 0);
		}
	}
}