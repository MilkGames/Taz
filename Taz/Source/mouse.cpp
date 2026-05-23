// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : mouse.cpp
//   Purpose : mouse control
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "actors.h"
#include "collision.h"
#include "collisioncache.h"
#include "physics.h"
#include "camera.h"
#include "objectviewer.h"
#include "maths.h"
#include "control.h"
#include "display.h"
#include "animation.h"
#include "characters.h"
#include "lights.h"
#include "main.h"
#include "costumes.h"
#include "textFX.h"
#include "frontendmenu.h"
#include "files.h"

#include "rectangle.h"
#include "display.h"
#include "colour.h"
#include "util.h"

#include "mouse.h"

MOUSE mouse;
float i = 0.0f;
float j = 0.0f;
TBVector spritePos;
float mouseSize;
float mouseMovement;

/*	--------------------------------------------------------------------------------
	Function 	: UpdateMousePointer
	Purpose 	: Update the mouse pointer
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void UpdateMousePointer(MOUSE *mousePointer)
{
	PAGEITEM*	item;

	if (mouse.draw)
	{
		mouseSize = (videoMode.xScreen/640.0f) * 25.0f;

		// MG: XBOX SOURCE <-> PC RELEASE DIFFERENCES
		//mouseMovement = (videoMode.xScreen/640.0f) * 250.0f; // MG: OLD
		mouseMovement = (videoMode.xScreen/640.0f) / 250.0f; // MG: NEW

		// NH: X Movement
		//mousePointer->mousePos[X] += (controller1.mxChannel->value / mouseMovement); // MG: OLD
		mousePointer->mousePos[X] += (controller1.mxChannel->value * mouseMovement); // MG: NEW

		if (mousePointer->mousePos[X] > videoMode.xScreen/2.0f)
			mousePointer->mousePos[X] = videoMode.xScreen/2.0f;
		else if (mousePointer->mousePos[X] < -(videoMode.xScreen/2.0f))
			mousePointer->mousePos[X] = -(videoMode.xScreen/2.0f);
		
		// NH: Y Movement
		//mousePointer->mousePos[Y] -= (controller1.myChannel->value / mouseMovement); // MG: OLD
		mousePointer->mousePos[Y] -= (controller1.myChannel->value * mouseMovement); // MG: NEW

		if (mousePointer->mousePos[Y] > videoMode.yScreen/2.0f)
			mousePointer->mousePos[Y] = videoMode.yScreen/2.0f;
		else if (mousePointer->mousePos[Y] < -(videoMode.yScreen/2.0f))
			mousePointer->mousePos[Y] = -(videoMode.yScreen/2.0f);
	}

	// MG: NEW CODE
	if (controller1.mxChannel->value || controller1.myChannel->value)
	{
		mouse.moved = TRUE;
	}

	// NH: Button 1 down
	if (controller1.mb1Channel->value != 0 || controller1.mb1Channel->value != 0)
	{
		mousePointer->mouseB1ClickPos[X] = mousePointer->mousePos[X];
		mousePointer->mouseB1ClickPos[Y] = mousePointer->mousePos[Y];
	}	
}


/*	--------------------------------------------------------------------------------
	Function 	: DrawMousePointer
	Purpose 	: Draw the mouse pointer
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void DrawMousePointer(MOUSE *mousePointer)
{
	TBTexture*	tex=bkLoadTexture(NULL, "mousepointer.bmp", 0);
	//TBTexture*	tex=bkLoadTexture(NULL, "textures\\ramps.bmp", 0);

	// MG: NEW CODE
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D, FALSE);
	bdSetObjectMatrix(bIdentityMatrix);

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, TRUE);
	// MG: ---------

	// MG: PC Release uses scaled mouseSize for both offset and draw size
	float size = mouseSize * 0.6f;

	spritePos[0] = mouse.mousePos[X]+15.0f;
	spritePos[1] = mouse.mousePos[Y]-15.0f;
	spritePos[2] = 0;
	spritePos[3] = 0;

	utilDraw2DSprite(tex, spritePos, size, size);
	//utilDraw2DSprite(tex, spritePos, 100.0f, 100.0f);

	// MG: NEW CODE
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D, FALSE);
}



/*	--------------------------------------------------------------------------------
	Function 	: InitMouse
	Purpose 	: Initialise The Mouse
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void InitMouse(MOUSE *mousePointer)
{
	mouse.mousePos[X] = (videoMode.xScreen/2.0f)-10.0f;
	mouse.mousePos[Y] = (videoMode.yScreen/2.0f)+10.0f;
	mouse.mouseB1ClickPos[X] = (videoMode.xScreen/2.0f)-10.0f;
	mouse.mouseB1ClickPos[Y] = (videoMode.yScreen/2.0f)+10.0f;

	mouse.active	= false;	// NH: Update the mouse?
	mouse.draw		= true;		// NH: If updating mouse, draw the mouse?
	mouse.moved		= true;     // MG: Mouse moved?

	spritePos[0] = 0.0f;
	spritePos[1] = 0.0f;
	spritePos[2] = 0.0f;
	spritePos[3] = 0.0f;
}