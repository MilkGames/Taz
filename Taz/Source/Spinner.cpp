// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Spinner.cpp
//   Purpose : Spinner control page-item (lets you specify a numerical value)
// Component : Taz
//		Info : Named after the Windows controls that do the same thing
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"				// PP: global stuff
#include "main.h"				// PP: main stuff
#include "control.h"			// PP: provide basic input and control functions
#include "Spinner.h"			// PP: Spinner control page-item (lets you specify a numerical value)


/*	--------------------------------------------------------------------------------
	Function 	: SPINNER::init
	Purpose 	: initialise the spinner
	Parameters 	: (opt/default)type, (opt/NULL=none) pointer to static value, (opt/default)adjustment speed, (opt/NULL=default)name
	Returns 	: 
	Info 		: this overrides TEXTBOX::init
*/
void SPINNER::init(const ESpinnerType typeIn, void* const valPtr, const float speedIn, const char* const nameIn)
{
	TEXTBOX::init();

	this->type=typeIn;

	this->voidVal=valPtr;

	this->speed=speedIn;

	this->setName(nameIn);
}


/*	--------------------------------------------------------------------------------
	Function 	: SPINNER::mainDraw
	Purpose 	: the core of the draw method - actually draw the spinner
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides TEXTBOX::mainDraw
*/
void SPINNER::mainDraw(void)
{
	// PP: more todo

	TEXTBOX::mainDraw();
}


/*	--------------------------------------------------------------------------------
	Function 	: SPINNER::udpate
	Purpose 	: update the spinner
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides TEXTBOX::update
*/
void SPINNER::update(void)
{
	// PP: IMPORTANT: TEXTBOX base class update, including effects update
	// PP: NOTE: At the time of writing, TEXTBOX::update is simply a wrapper for PAGEITEM::update.
	// PP:			However, having all TEXTBOX subclasses call TEXTBOX::update rather than PAGEITEM::update is reassuringly future-proof.
	TEXTBOX::update();

	if(this->type == SPINNERTYPE_INT)
	{
		this->sprintf8("%d", *this->intVal);
	}
	else if(this->type == SPINNERTYPE_UINT)
	{
		this->sprintf8("%u", *this->uintVal);
	}
	else
	{
		this->sprintf8("%5.2f", *this->floatVal);
		
#define SPINNER_MAX_STEP_FLOAT		3.0f

		if(this->selected)// PP: only respond to input if selected eh?
		{
			if(this->floatVal != NULL)
			{
				if(this->type == SPINNERTYPE_FLOAT)
				{
					/* PP: control volume by pushing stick left and right (analogue control) */
					*this->floatVal = *this->floatVal+(controller1.x1*this->speed);
				}
				else// PP: unsigned
				{
					/* PP: control volume by pushing stick left and right (analogue control) */
					*this->floatVal = *this->floatVal+(controller1.x1*this->speed);

					if(*this->floatVal < 0.0f)
					{
						*this->floatVal=0.0f;
					}
				}
			}
		}
	}


}