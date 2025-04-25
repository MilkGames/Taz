// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : CheckBox.cpp
//   Purpose : Check box control page-item - lets you toggle something on & off
// Component : Taz
//		Info : Named after the Windows controls that do the same thing
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"				// PP: global stuff
#include "main.h"				// PP: main stuff
#include "control.h"			// PP: provide basic input and control functions
#include "CheckBox.h"			// PP: Check box control page-item - lets you toggle something on & off
#include "util.h"				// PP: general utility functions


// PP: initialise CHECKBOX static texture ptrs
TBTexture*	TAG_CHECKBOX::checkTextures[CHECKBOX_NUM_STATES]={NULL, NULL};


/*	--------------------------------------------------------------------------------
	Function 	: CHECKBOX::init
	Purpose 	: initialise the check box
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides TEXTBOX::init
*/
void CHECKBOX::init(void)
{
	TEXTBOX::init();

	this->state=CHECKBOX_DEFAULT_STATE;
}


/*	--------------------------------------------------------------------------------
	Function 	: CHECKBOX::mainDraw
	Purpose 	: the core of the draw method - actually draw the checkbox
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides TEXTBOX::mainDraw
*/
void CHECKBOX::mainDraw(void)
{
	if(~this->flags & TBFLAG_CONCEALED)// PP: concealed checkboxes don't need ticks/crosses
	{
		// PP: draw tick/cross/whtever...

		TBTexture*	texture;

		texture=CHECKBOX::checkTextures[this->state];

		if(texture != NULL)
		{
			utilDraw2DSprite(texture, VEC( (this->optRect.widthAbs()/2.0f)-(((CHECKBOX_CHECK_WIDTH/2.0f)-CHECKBOX_CHECK_X_SPACE)*this->actualFontSize) ), CHECKBOX_CHECK_WIDTH*this->actualFontSize);
		}
	}

	// PP: draw the text
	TEXTBOX::mainDraw();
}


/*	--------------------------------------------------------------------------------
	Function 	: CHECKBOX::getTextRect
	Purpose 	: get the rectangle into which the text must be fitted
	Parameters 	: 
	Returns 	: the rectangle into which the text must be fitted
	Info 		: this overrides TEXTBOX::getTextRect, to account for the part of this->optRect that is taken up by the check mark
*/
RECTANGLE CHECKBOX::getTextRect(void) const
{
	return RECTANGLE(this->optRect.left, this->optRect.right-((CHECKBOX_CHECK_WIDTH+CHECKBOX_CHECK_X_SPACE)*this->actualFontSize), this->optRect.bottom, this->optRect.top);
}