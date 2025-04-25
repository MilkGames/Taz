// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Spinner.h
//   Purpose : Spinner control page-item (lets you specify a numerical value)
// Component : Taz
//		Info : Named after the Windows controls that do the same thing
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __SPINNER_H__
#define __SPINNER_H__

#include "Textfx.h"						// PP: Text effects, eg. button glyphs, coloured text

// PP: spinner types
enum ESpinnerType
{
	SPINNERTYPE_INT,
	SPINNERTYPE_UINT,
	SPINNERTYPE_FLOAT,
	SPINNERTYPE_UFLOAT,

	NUM_SPINNER_TYPES
};

// PP: default type for a spinner
#define SPINNER_DEFAULT_TYPE	SPINNERTYPE_UINT

#define SPINNER_TEXT_LENGTH		32

#define SPINNER_DEFAULT_SPEED	5.0f

struct SPINNER		:		public TEXTBOX
{
	ESpinnerType	type;

	float			speed;

	union
	{
		void*			voidVal;

		int32*			intVal;
		uint32*			uintVal;
		float*			floatVal;
	};

protected:


	/*	--------------------------------------------------------------------------------
		Function 	: SPINNER::mainDraw
		Purpose 	: the core of the draw method - actually draw the spinner
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides TEXTBOX::mainDraw
	*/
	virtual void mainDraw(void);


public:


	/*	--------------------------------------------------------------------------------
		Function 	: SPINNER::SPINNER
		Purpose 	: constructor; creates a new spinner
		Parameters 	: (opt/default)type, (opt/NULL=none) pointer to static value, (opt/default)adjustment speed, (opt/NULL=default)name
		Returns 	: new GOLFBAR instance
		Info 		: 
	*/
	SPINNER(const ESpinnerType typeIn=SPINNER_DEFAULT_TYPE, void* const valPtr=NULL, const float speedIn=SPINNER_DEFAULT_SPEED, const char* const nameIn=NULL)
	{
		SPINNER::init(typeIn, valPtr, speedIn, name);
	}

	
	/*	--------------------------------------------------------------------------------
		Function 	: SPINNER::init
		Purpose 	: initialise the spinner
		Parameters 	: (opt/default)type, (opt/NULL=none) pointer to static value, (opt/default)adjustment speed, (opt/NULL=default)name
		Returns 	: 
		Info 		: this overrides TEXTBOX::init
	*/
	void init(const ESpinnerType typeIn=SPINNER_DEFAULT_TYPE, void* const valPtr=NULL, const float speedIn=SPINNER_DEFAULT_SPEED, const char* const nameIn=NULL);


	/*	--------------------------------------------------------------------------------
		Function 	: SPINNER::udpate
		Purpose 	: update the spinner
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides TEXTBOX::update
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: SPINNER::getRequiredWidth
		Purpose 	: get the minimum required width of the the spinner
		Parameters 	: 
		Returns 	: minimum required width of the the spinner
		Info 		: this overrides TEXTBOX::getRequiredWidth
	*/

#define SPINNER_WIDTH	0.15f

	float SPINNER::getRequiredWidth(void)
	{
		return SPINNER_WIDTH*this->getAlignScale();
	}


}BALIGN16;


#endif// PP: ndef __SPINNER_H__