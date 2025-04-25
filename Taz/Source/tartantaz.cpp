// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : tartantaz.cpp
//   Purpose : functions to fill taz with a texture
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "main.h"
#include "display.h"
#include "characters.h"
#include "textures.h"
#include "tartantaz.h"

/* --------------------------------------------------------------------------------
   Function : EnableTartanTazRenderMode
   Purpose : 
   Parameters : 
   Returns : 
   Info : must have characterinfo structure
*/

int EnableTartanTazRenderMode(ACTORINSTANCE *actorInstance, float tileU, float tileV)
{
	TARTANTAZ		*info;

	if(actorInstance->characterInfo)
	{
		actorInstance->characterInfo->tartanInfo = (TARTANTAZ*)ZALLOC(sizeof(TARTANTAZ));
		ASSERTM(actorInstance->characterInfo->tartanInfo, "out of tartan memory");

		info = actorInstance->characterInfo->tartanInfo;
		if(!(info->target = bdCreateRenderTarget(512, 512, 16, 16, 0)))
		{
			SAFE_FREE(actorInstance->characterInfo->tartanInfo);
			return FALSE;
		}
		bdGetRenderTargetInfo(info->target, &info->width, &info->height, &info->rgbBits, &info->zBits);
		info->actorAlpha = 0.0f;
		info->textureAlpha = 127.0f;
		info->texture = bkLoadTexture(NULL, "tartan.bmp", 0);
		info->tileU = tileU;
		info->tileV = tileV;
		info->velU = 0.0f;
		info->velV = 0.0f;
		info->deltaU = 0.0f;
		info->deltaV = 0.0f;
		return TRUE;
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : DisableTartanTazRenderMode
   Purpose : 
   Parameters : 
   Returns : 
   Info : must have characterinfo structure
*/

void DisableTartanTazRenderMode(ACTORINSTANCE *actorInstance)
{
	if(actorInstance->characterInfo)
	{
		if(actorInstance->characterInfo->tartanInfo)
		{
			bdDeleteRenderTarget(actorInstance->characterInfo->tartanInfo->target);
			SAFE_FREE(actorInstance->characterInfo->tartanInfo);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : DisableTartanTazRenderMode
   Purpose : 
   Parameters : 
   Returns : 
   Info : must have characterinfo structure
*/

char UpdateTartanTazRenderTarget(ACTORINSTANCE *actorInstance)
{
	CAMERAINFO			*camera;
	TARTANTAZ			*info;
	float				x, y, z;
	TBPrimVertex		verts[4];
	TBQuaternion		tempQuat;
	TBVector			tempVector;
	TBMatrix			trans, rot, result;
	EBActRenderMode		renderModeCopies[2];
	void*				renderParmCopies[2];
	char				retVal;

	if(gameStatus.multiplayer.numScreens != 1) return 0;

	if(actorInstance->characterInfo)
	{
		if(info = actorInstance->characterInfo->tartanInfo)
		{
			// CMD: switch to render target
			// CMD: clear target to a = 0 and z = 0.0f

			// PP: REMOUT: it's safer to push and pop render targets		if(bdSetRenderTarget(info->target, 127,0,0,0, 0.0f, BSETRENDERTARGET_CLEAR))
			if(pushRenderTarget(info->target, 127,0,0,0, 0.0f, BSETRENDERTARGET_CLEAR))// PP: 
			{
				// CMD: calculate matrix to represent camera 1's rotation and translation
				camera = gameStatus.player[0].camera;
				bmMatTranslate(trans, camera->cameraState.cameraPos[0], camera->cameraState.cameraPos[1], camera->cameraState.cameraPos[2]);
				bmVectorSub(tempVector, camera->cameraState.cameraLookat, camera->cameraState.cameraPos);
				bmDirectionToQuat(tempQuat, tempVector, 0.0f);
				bmQuatToMatrix(rot, tempQuat);
				bmMatMultiply(result, trans, rot);

				// CMD: draw plane to set z buffer to min
				z = METERTOUNIT(-1.5f);
				x = z*bmTan(videoMode.screens[0].horzFov*0.5f);
				y = z*bmTan(videoMode.screens[0].vertFov*0.5f);
				// CMD: verts are ordered: lower left, lower right, upper left, upper right
				BDVERTEX_SETXYZW(&verts[0], -x, -y, z, 1.0f);
				BDVERTEX_SETNORMAL(&verts[0], 0.0f, 0.0f, 1.0f);
				BDVERTEX_SETRGBA(&verts[0], 127, 127, 127, 100);
				BDVERTEX_SETUV(&verts[0], info->tileU, info->tileV);

				BDVERTEX_SETXYZW(&verts[1], x, -y, z, 1.0f);
				BDVERTEX_SETNORMAL(&verts[1], 0.0f, 0.0f, 1.0f);
				BDVERTEX_SETRGBA(&verts[1], 127, 127, 127, 100);
				BDVERTEX_SETUV(&verts[1], 0.0f, info->tileV);

				BDVERTEX_SETXYZW(&verts[2], -x, y, z, 1.0f);
				BDVERTEX_SETNORMAL(&verts[2], 0.0f, 0.0f, 1.0f);
				BDVERTEX_SETRGBA(&verts[2], 127, 127, 127, 100);
				BDVERTEX_SETUV(&verts[2], info->tileU, 0.0f);

				BDVERTEX_SETXYZW(&verts[3], x, y, z, 1.0f);
				BDVERTEX_SETNORMAL(&verts[3], 0.0f, 0.0f, 1.0f);
				BDVERTEX_SETRGBA(&verts[3], 127, 127, 127, 100);
				BDVERTEX_SETUV(&verts[3], 0.0f, 0.0f);

				TBVector		willy;
				BDVERTEX_GETXYZW(&verts[0], willy[0], willy[1], willy[2], willy[3]);
				bmMatMultiplyVector(result, willy);

				bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
				bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, TRUE);
				bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
				bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, TRUE);
				bdSetObjectMatrix(result);
				bdSetTexture(0, textureCucumber);
				bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, verts, 4, BVERTTYPE_SINGLE);

				// CMD: second draw actorInstance naked with huge z bias, ztest = always
				renderModeCopies[0] = actorInstance->actorInstance.renderMode[0];
				renderModeCopies[1] = actorInstance->actorInstance.renderMode[1];
				renderParmCopies[0] = actorInstance->actorInstance.renderParms[0];
				renderParmCopies[1] = actorInstance->actorInstance.renderParms[1];

				baSetActorRenderMode(&actorInstance->actorInstance, 0, BACTRENDERMODE_ZONLY, NULL);
				baSetActorRenderMode(&actorInstance->actorInstance, 1, BACTRENDERMODE_NONE, NULL);
				bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
				bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, TRUE);
				bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
				bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, TRUE);

				bdSetZBias(10000.0f);
				baDrawActorInstance(&actorInstance->actorInstance,BSETPRIMALPHATYPE_SOLID);
				bdSetZBias(0.0f);

				// CMD: third draw tartan plane behind first plane

				z = METERTOUNIT(-1.0f);
				x = z*bmTan(videoMode.screens[0].horzFov*0.5f);
				y = z*bmTan(videoMode.screens[0].vertFov*0.5f);
				// CMD: verts are ordered: lower left, lower right, upper left, upper right
				BDVERTEX_SETXYZW(&verts[0], -x, -y, z, 1.0f);
				BDVERTEX_SETNORMAL(&verts[0], 0.0f, 0.0f, 1.0f);
				BDVERTEX_SETRGBA(&verts[0], 128, 128, 128, info->textureAlpha);
				BDVERTEX_SETUV(&verts[0], info->tileU, info->tileV);

				BDVERTEX_SETXYZW(&verts[1], x, -y, z, 1.0f);
				BDVERTEX_SETNORMAL(&verts[1], 0.0f, 0.0f, 1.0f);
				BDVERTEX_SETRGBA(&verts[1], 128, 128, 128, info->textureAlpha);
				BDVERTEX_SETUV(&verts[1], 0.0f, info->tileV);

				BDVERTEX_SETXYZW(&verts[2], -x, y, z, 1.0f);
				BDVERTEX_SETNORMAL(&verts[2], 0.0f, 0.0f, 1.0f);
				BDVERTEX_SETRGBA(&verts[2], 128, 128, 128, info->textureAlpha);
				BDVERTEX_SETUV(&verts[2], info->tileU, 0.0f);

				BDVERTEX_SETXYZW(&verts[3], x, y, z, 1.0f);
				BDVERTEX_SETNORMAL(&verts[3], 0.0f, 0.0f, 1.0f);
				BDVERTEX_SETRGBA(&verts[3], 128, 128, 128, info->textureAlpha);
				BDVERTEX_SETUV(&verts[3], 0.0f, 0.0f);

				bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
				bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, TRUE);
				bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
				bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, TRUE);
				bdSetObjectMatrix(result);
				bdSetTexture(0, info->texture);
				bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, verts, 4, BVERTTYPE_SINGLE);
				////////////////////////////

				// CMD: switch back to primary target

				// PP: REMOUT: it's safer to push and pop render targets		bdSetRenderTarget(NULL, 0,0,0,255, 1.0f, BSETRENDERTARGET_CLEARZ);
				popRenderTarget(0,0,0,255, 1.0f, BSETRENDERTARGET_CLEARZ);// PP: 


				// CMD: render wallpaper geometry here
				// CMD: calculate depth in z axis inwhich a quad of height videoMode.x will fit the screen
				z = -videoMode.xScreen/(2.0f*bmTan(videoMode.screens[0].vertFov*0.5f));
				
				// CMD: verts are ordered: lower left, lower right, upper left, upper right
				BDVERTEX_SETXYZW(&verts[0], -videoMode.xScreen*0.8f, -videoMode.yScreen*0.8f, z, 1.0f);
				BDVERTEX_SETNORMAL(&verts[0], 0.0f, 0.0f, 1.0f);
				BDVERTEX_SETRGBA(&verts[0], 128, 128, 128, 128);
				BDVERTEX_SETUV(&verts[0], 1.0f, 1.0f);

				BDVERTEX_SETXYZW(&verts[1], videoMode.xScreen*0.8f, -videoMode.yScreen*0.8f, z, 1.0f);
				BDVERTEX_SETNORMAL(&verts[1], 0.0f, 0.0f, 1.0f);
				BDVERTEX_SETRGBA(&verts[1], 128, 128, 128, 128);
				BDVERTEX_SETUV(&verts[1], 0.0f, 1.0f);

				BDVERTEX_SETXYZW(&verts[2], -videoMode.xScreen*0.8f, videoMode.yScreen*0.8f, z, 1.0f);
				BDVERTEX_SETNORMAL(&verts[2], 0.0f, 0.0f, 1.0f);
				BDVERTEX_SETRGBA(&verts[2], 128, 128, 128, 128);
				BDVERTEX_SETUV(&verts[2], 1.0f, 0.0f);

				BDVERTEX_SETXYZW(&verts[3], videoMode.xScreen*0.8f, videoMode.yScreen*0.8f, z, 1.0f);
				BDVERTEX_SETNORMAL(&verts[3], 0.0f, 0.0f, 1.0f);
				BDVERTEX_SETRGBA(&verts[3], 128, 128, 128, 128);
				BDVERTEX_SETUV(&verts[3], 0.0f, 0.0f);

//				bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, 0);
//				bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, 1);
//				bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, 0);
//				bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, 1);
				bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, 0);
				bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, 1);
				bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, 0);
				bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, 1);
				bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
				bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, TRUE);
				bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
				bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, TRUE);

				// CMD: set render target as current texture
				bdSetRenderTargetAsTexture(info->target, 0);
				bdSetObjectMatrix(result);
				bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, verts, 4, BVERTTYPE_SINGLE);

				// CMD: reset texture
				bdSetTexture(0, NULL);

				// CMD: render actor sans outline here
				if(info->actorAlpha > 0.0f)
				{
				}

				// CMD: render actor outline only here

				// CMD: reset render modes
				actorInstance->actorInstance.renderMode[0] = renderModeCopies[0];
				actorInstance->actorInstance.renderMode[1] = renderModeCopies[1];
				actorInstance->actorInstance.renderParms[0] = renderParmCopies[0];
				actorInstance->actorInstance.renderParms[1] = renderParmCopies[1];
				bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, 0);
				bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, 1);
				bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, 0);
				bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, 1);
				bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
				bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, TRUE);
			}
		}
	}
	retVal = 1;
	return retVal;
}