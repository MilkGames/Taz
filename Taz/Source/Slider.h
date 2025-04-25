// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Slider.cpp
//   Purpose : Horizontal slider page-item used for controlling values, eg. music/sound fx volume.
// Component : Taz
//		Info : Named after the Windows controls that do the same thing
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __SLIDER_H__
#define __SLIDER_H__

#include "Textfx.h"						// PP: Text effects, eg. button glyphs, coloured text

// PP: type for pointer to function called when a slider is updated
typedef void (*SliderUpdateFunc)(const float sliderValue, struct TAG_SLIDER* const slider);


#define SLIDER_DEFAULT_COLOUR1			COLOUR(128, 128, 255)
#define SLIDER_DEFAULT_COLOUR2			COLOUR(255, 255, 255)
#define SLIDER_DEFAULT_NUM_BLOCKS		7
#define SLIDER_HEIGHT					0.12f// PP: REMOUT: must now be expressed as a fraction of the screen height		50.0f
#define SLIDER_WIDTH					0.45f// PP: REMOUT: must now be expressed as a fraction of the screen height		250.0f

typedef struct TAG_SLIDER		:		public PAGEITEM
{

protected:


	/*	--------------------------------------------------------------------------------
		Function 	: SLIDER::mainDraw
		Purpose 	: the core of the draw method - actually draw the slider
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::mainDraw
	*/
	virtual void mainDraw(void);


public:

	// PP: why can't I make these static?
	COLOUR				colour1;		// PP: left-hand colour of slider
	COLOUR				colour2;		// PP: right-hand colour of slider
	int					numBlocks;		// PP: number of blocks on slider
	SliderUpdateFunc	updateFunc;		// PP: function called when the slider is updated - this is used to set the value controlled by the slider

	float				value;			// PP: fraction (0..1) being represented by the slider bar


	/*	--------------------------------------------------------------------------------
		Function 	: SLIDER::update
		Purpose 	: update the slider
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::update
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: SLIDER::TAG_SLIDER
		Purpose 	: [default] constructor
		Parameters 	: (opt/NULL) callback function, (opt/default) colour1, (opt/default) colour2, (opt/default)number of blocks,
		Returns 	: new SLIDER instance
		Info 		: this overrides PAGEITEM::TAG_PAGEITEM
	*/
	TAG_SLIDER(const SliderUpdateFunc updateFuncIn=NULL, const char* const nameIn=NULL, const COLOUR colour1In=SLIDER_DEFAULT_COLOUR1,
				const COLOUR colour2In=SLIDER_DEFAULT_COLOUR2, const int numBlocksIn=SLIDER_DEFAULT_NUM_BLOCKS)
	{
		PAGEITEM::init(nameIn);

		this->updateFunc=updateFuncIn;

		this->colour1=colour1In;
		this->colour2=colour2In;
		this->numBlocks=numBlocksIn;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::getRequiredHeight
		Purpose 	: get the minimum required height for the slider
		Parameters 	: 
		Returns 	: the minimum required height for the slider
		Info 		: this overrides PAGEITEM::getRequiredHeight
	*/
	float getRequiredHeight(void)
	{
		return this->getAlignScale()*SLIDER_HEIGHT;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::getRequiredWidth
		Purpose 	: get the minimum required width for the slider
		Parameters 	: 
		Returns 	: the minimum required width for the slider
		Info 		: this overrides PAGEITEM::getRequiredWidth
	*/
	float getRequiredWidth(void)
	{
		return this->getAlignScale()*SLIDER_WIDTH;
	}

	/*	--------------------------------------------------------------------------------
		Function 	: SLIDER::getRect
		Purpose 	: get the rect of the slider
		Parameters 	: 
		Returns 	: The rect used by the slider
		Info 		: 
	*/
	RECTANGLE* getRect(void);

	/*	--------------------------------------------------------------------------------
		Function 	: SLIDER::setNavigation
		Purpose 	: enable/disable navigation for individual items
		Parameters 	: true to enable / false to disable
		Returns 	: this
		Info 		: 
	*/
//	void setNavigation(const bool on, const bool mouseOnly = false);


}SLIDER, _SLIDER BALIGN16;


#endif// PP: ndef __SLIDER_H__
