// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : GolfBar.h
//   Purpose : Power-level indicator for the West Boss player 2's dynamite throwing
// Component : Taz
// Info		 : The name relates to the power-level indicators found in golf games
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __GOLFBAR_H__
#define __GOLFBAR_H__

#include "TextFX.h"							// PP: Text effects, eg. button glyphs, coloured text
#include "control.h"						// PP: controller stuff


// PP: type for the function called when a golf bar 'hits'
typedef void(*GolfBarHitCallback)(const long32 context, const float level);

struct GOLFBAR		:		public PAGEITEM
{

private:

//	float					level2;					// PP: the power level of the golf bar (0..1) according to the twang stick
//	float					lastLevel2;				// PP: the power level of the golf bar (0..1) according to the twang stick ON LAST TWANG TEST 
	const PADCONTROLSTATUS*	controller;				// PP: ptr to the controller used to control the power level
	GolfBarHitCallback		hitCallback;			// PP: function that gets called when the golf bar 'hits'
	long32					hitCallbackContext;		// PP: context value passed through to hit callback
	float					timer;					// PP: general-purpose timer
	RECTANGLE				barRect;				// PP: rectangle of bar
	
protected:


	/*	--------------------------------------------------------------------------------
		Function 	: GOLFBAR::mainDraw
		Purpose 	: the core of the draw method - actually draw the golf bar
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::mainDraw
	*/
	virtual void mainDraw(void);


public:

	float					level;					// PP: the power level of the golf bar (0..1)

	/*	--------------------------------------------------------------------------------
		Function 	: GOLFBAR::GOLFBAR
		Purpose 	: constructor; creates a new golf bar
		Parameters 	: ptr to controller to control control power level,
						(opt/NULL)hit callback, (opt/0)hit callback context, (opt/NULL)name for the golf bar
		Returns 	: new GOLFBAR instance
		Info 		: 
	*/
	GOLFBAR(const PADCONTROLSTATUS* const controllerIn, const GolfBarHitCallback callback=NULL, const long32 callbackContext=0, const char* const nameIn=NULL)
	{
		GOLFBAR::init(controllerIn, callback, callbackContext, nameIn);
	}

	
	/*	--------------------------------------------------------------------------------
		Function 	: GOLFBAR::init
		Purpose 	: initialise the golf bar
		Parameters 	: ptr to controller to control control power level,
						(opt/NULL)hit callback, (opt/0)hit callback context, (opt/NULL)name for the golf bar
		Returns 	: 
		Info 		: 
	*/
	void init(const PADCONTROLSTATUS* const controllerIn, GolfBarHitCallback callback=NULL, const long32 callbackContext=0, const char* const nameIn=NULL);


	/*	--------------------------------------------------------------------------------
		Function 	: GOLFBAR::udpate
		Purpose 	: update the golf bar
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::update
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: GOLFBAR::getRequiredHeight
		Purpose 	: get the minimum required height for a GOLFBAR
		Parameters 	: 
		Returns 	: minimum required height for the item
		Info 		: this overrides PAGEITEM::getRequiredHeight
	*/
	inline float getRequiredHeight(void)
	{
		return this->getAlignScale()*this->barRect.heightAbs();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: GOLFBAR::getRequiredWidth
		Purpose 	: get the minimum required width for a GOLFBAR
		Parameters 	: 
		Returns 	: minimum required width for the item
		Info 		: this overrides PAGEITEM::getRequiredWidth
	*/
	inline float getRequiredWidth(void)
	{
		return this->getAlignScale()*this->barRect.widthAbs();
	}


}BALIGN16;


#endif // PP: __GOLFBAR_H__