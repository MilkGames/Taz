// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : GolfBar.h
//   Purpose : Power-level indicator for the West Boss player 2's dynamite throwing
// Component : Taz
// Info		 : The name relates to the power-level indicators found in golf games
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"				// PP: global stuff
#include "maths.h"				// PP: maths stuff
#include "main.h"				// PP: main stuff

#include "GolfBar.h"			// PP: Power-level indicator for the West Boss player 2's dynamite throwing


/*	--------------------------------------------------------------------------------
	Function 	: GOLFBAR::init
	Purpose 	: initialise the golf bar
	Parameters 	: ref to controller to control power level,
					(opt/NULL)hit callback, (opt/0)hit callback context, (opt/NULL)name for the golf bar
	Returns 	: 
	Info 		: 
*/
void GOLFBAR::init(const PADCONTROLSTATUS* const controllerIn, GolfBarHitCallback callback, const long32 callbackContext, const char* const nameIn)
{
	PAGEITEM::init(nameIn);

	this->controller=controllerIn;
	this->hitCallback=callback;
	this->hitCallbackContext=callbackContext;

	this->barRect=RECTANGLE(0.0f,0.045f,0.0f,0.335f).toOrigin();

	this->level=0.0f;
	this->timer=0.0f;
}


/*	--------------------------------------------------------------------------------
	Function 	: GOLFBAR::mainDraw
	Purpose 	: the core of the draw method - actually draw the golf bar
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEM::mainDraw
*/
void GOLFBAR::mainDraw(void)
{
	COLOUR	barCol;

	barCol=COLOUR(0, 255, 0, 255);

	this->barRect.draw(COLOUR(barCol.r, barCol.g, barCol.b, 64), SOLIDFILL);
	this->barRect.bottomPortion(this->level).draw(barCol, SOLIDFILL);
	this->barRect.draw(barCol, OUTLINE);
}


/*	--------------------------------------------------------------------------------
	Function 	: GOLFBAR::udpate
	Purpose 	: update the golf bar
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEM::update
*/

#define GOLFDISPLAY_TWANG_CHECK_TIME		0.15f		// PP: time between twang checks
#define GOLFBAR_MIN_TWANG_LEVEL				0.1f		// PP: minimum level from which dropping to 0 within GOLFDISPLAY_TWANG_CHECK_TIME will cause a twang
#define GOLFBAR_MOVE_SPEED					-0.5f

void GOLFBAR::update(void)
{
	PAGEITEM::update();

	// PP: check we have a controller to read
	if(this->controller == NULL) return;

	// PP: update level...

	this->level += this->controller->y1*fTime*GOLFBAR_MOVE_SPEED;
	// PP: REMOUT: hassle		this->level += this->controller->y2*fTime*GOLFBAR_MOVE_SPEED;

	this->level=CONSTRAIN(this->level, 0.0f, 1.0f);

	// PP: check for a twang...

	// PP: twang can be triggered with X// PP: NEW: X(A) ONLY!
	if((this->controller->crossDebounceChannel->value != 0)
	/* PP: REMOUT: came back as a bug, since only A is specified on the instructions
	||(this->controller->r1DebounceChannel->value)
	||(this->controller->r2DebounceChannel->value)*/)
	{
		if(this->level > GOLFBAR_MIN_TWANG_LEVEL)
		{
			if(this->hitCallback != NULL)
			{
				this->hitCallback(this->hitCallbackContext, this->level);
			}
		}
	}

	// PP: update twang timer
	this->timer+=fTime;
}