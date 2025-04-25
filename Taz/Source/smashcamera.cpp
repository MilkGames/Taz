// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : smashcamera.cpp
//   Purpose : functions to smash the camera
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "main.h"
#include "util.h"
#include "display.h"
#include "textures.h"
#include "smashcamera.h"


/* --------------------------------------------------------------------------------
   Function : SmashCamera
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int SmashCamera(CAMERAINFO *camera, int xPosition, int yPosition, int width, int height, ESmashType type, float time, TBTexture *texture)
{
	if(camera->smashInfo) return FALSE;			// CMD: the camera is currently smashed, do we want to smash more than once?

	camera->smashInfo = (SCREENSMASHINFO*)ZALLOC(sizeof(SCREENSMASHINFO));
	ASSERTM(camera->smashInfo, "cannot malloc memory for screen smash info");

	camera->smashInfo->type = type;
	camera->smashInfo->state = CAMERASMASH_SPREAD;
	camera->smashInfo->clock = time;
	camera->smashInfo->tilt = UtilGenerateRandomNumber(RAD(360.0f), 0.0f);
	camera->smashInfo->wobble = 0.0f;
	camera->smashInfo->alpha = 70.0f;

	if(texture) camera->smashInfo->texture = texture;
	else camera->smashInfo->texture = bkLoadTexture(NULL, "glassmash.tga", 0);

	camera->smashInfo->screenCoords.x = (float)xPosition;
	camera->smashInfo->screenCoords.y = (float)yPosition;
	camera->smashInfo->screenCoords.deltaX = 0.0f;
	camera->smashInfo->screenCoords.deltaY = 0.0f;
	camera->smashInfo->screenCoords.maxDelta = METERTOUNIT(0.1f);
	camera->smashInfo->screenCoords.sizeX = (float)width;
	camera->smashInfo->screenCoords.sizeY = (float)height;
	camera->smashInfo->screenCoords.drawSizeFactor = 0.0f;

	camera->smashInfo->camera = camera;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : UpdateScreenSmashEffect
   Purpose : 
   Parameters : 
   Returns : TRUE while effect is still in effect
   Info : 
*/

int UpdateScreenSmashEffect(SCREENSMASHINFO *info)
{
	if(!info) return FALSE;				// CMD: there is no current smash

	// CMD: update position
	switch(info->state)
	{
	case CAMERASMASH_INITIALWOBBLE:
		// CMD: increment max delta
		if((info->screenCoords.maxDelta -= fTime*METERTOUNIT(0.2f)) < 0.0f)
		{
			info->screenCoords.maxDelta = 0.0f;
			info->state = CAMERASMASH_STATIC;
		}
		info->screenCoords.deltaX = UtilGenerateRandomNumber(info->screenCoords.maxDelta, -info->screenCoords.maxDelta);
		info->screenCoords.deltaY = UtilGenerateRandomNumber(info->screenCoords.maxDelta, -info->screenCoords.maxDelta);
		break;
	case CAMERASMASH_SPREAD:
		if((info->screenCoords.drawSizeFactor += 8.0f*fTime) > 1.0f)
		{
			info->screenCoords.drawSizeFactor = 1.0f;
			info->state = CAMERASMASH_STATIC;
		}
		break;
	case CAMERASMASH_STATIC:
		info->screenCoords.deltaX = 0.0f;
		info->screenCoords.deltaY = 0.0f;
		break;
	}
	// CMD: update alpha
	switch(info->type)
	{
	case CAMERASMASH_PERMANENT:
		// CMD: do nothing here
		break;
	case CAMERASMASH_FADE:
		if((info->clock -= fTime) < 0.0f)
		{
			if((info->alpha -= CAMERASMASH_FADERATE*fTime) < 0.0f)
			{
				info->alpha = 0.0f;
				FreeScreenSmashEffect(info->camera);
				return FALSE;
			}
		}
		break;
	}
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : DrawScreenSmashEffect
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void DrawScreenSmashEffect(SCREENSMASHINFO *info)
{
	float			depth, xPos, yPos, drawWidth, drawHeight;
	float			one, zero;
	TBPrimVertex	verts[4];
	TBQuaternion	quat;
	TBVector		direction;
	TBMatrix		translate, rotation, result;

	if(!info) return;								// CMD: there is no current smash


	// CMD: calculate depth in z axis inwhich a quad of height videoMode.x will fit the screen
	depth = -videoMode.xScreen/(2.0f*bmTan(videoMode.screens[0].vertFov*0.5f));
	
	xPos = info->screenCoords.x+info->screenCoords.deltaX;
	yPos = info->screenCoords.y+info->screenCoords.deltaY;
	drawWidth = info->screenCoords.sizeX*info->screenCoords.drawSizeFactor;
	drawHeight = info->screenCoords.sizeY*info->screenCoords.drawSizeFactor;

	// CMD: calculate what texture coords should be, such that as poly scales, texture does not
	one = 0.5f + (0.5f*info->screenCoords.drawSizeFactor);
	zero = 0.5f - (0.5f*info->screenCoords.drawSizeFactor);

	// CMD: verts are ordered: lower left, lower right, upper left, upper right
	BDVERTEX_SETXYZW(&verts[0], (xPos-drawWidth), (yPos-drawHeight), depth, 1.0f);
	BDVERTEX_SETNORMAL(&verts[0], 0.0f, 0.0f, 1.0f);
	BDVERTEX_SETRGBA(&verts[0], 128, 128, 128, (int)info->alpha);
	BDVERTEX_SETUV(&verts[0], one, one);

	BDVERTEX_SETXYZW(&verts[1], (xPos+drawWidth), (yPos-drawHeight), depth, 1.0f);
	BDVERTEX_SETNORMAL(&verts[1], 0.0f, 0.0f, 1.0f);
	BDVERTEX_SETRGBA(&verts[1], 128, 128, 128, (int)info->alpha);
	BDVERTEX_SETUV(&verts[1], zero, one);

	BDVERTEX_SETXYZW(&verts[2], (xPos-drawWidth), (yPos+drawHeight), depth, 1.0f);
	BDVERTEX_SETNORMAL(&verts[2], 0.0f, 0.0f, 1.0f);
	BDVERTEX_SETRGBA(&verts[2], 128, 128, 128, (int)info->alpha);
	BDVERTEX_SETUV(&verts[2], one, zero);

	BDVERTEX_SETXYZW(&verts[3], (xPos+drawWidth), (yPos+drawHeight), depth, 1.0f);
	BDVERTEX_SETNORMAL(&verts[3], 0.0f, 0.0f, 1.0f);
	BDVERTEX_SETRGBA(&verts[3], 128, 128, 128, (int)info->alpha);
	BDVERTEX_SETUV(&verts[3], zero, zero);

	// CMD: calculate matrix that will place polys in correct place
	bmMatTranslate(translate, info->camera->cameraState.cameraPos[0], info->camera->cameraState.cameraPos[1], info->camera->cameraState.cameraPos[2]);
	bmVectorSub(direction, info->camera->cameraState.cameraLookat, info->camera->cameraState.cameraPos);
	bmDirectionToQuat(quat, direction, 0.0f);
	bmQuatToMatrix(rotation, quat);
	bmMatMultiply(result, translate, rotation);

	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, 0);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);

	bdSetObjectMatrix(result);
	bdSetTexture(0, info->texture);
	bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, verts, 4, BVERTTYPE_SINGLE);

	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, 0);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, 0);
}

/* --------------------------------------------------------------------------------
   Function : DrawScreenSmashEffect
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void FreeScreenSmashEffect(CAMERAINFO *camera)
{
	if(camera->smashInfo)
	{
		SAFE_FREE(camera->smashInfo);
	}
}