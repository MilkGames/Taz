// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : CheckBox.h
//   Purpose : Check box control page-item - lets you toggle something on & off
// Component : Taz
//		Info : Named after the Windows controls that do the same thing
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_CHECKBOX
#define __H_CHECKBOX

#include "Textfx.h"						// PP: Text effects, eg. button glyphs, coloured text

enum ECheckBoxState// PP: NO NEGATIVES OR GAPS PLEASE - THESE VALUES GET USED AS ARRAY INDICES
{
	CHECKBOXSTATE_UNCHECKED,
	CHECKBOXSTATE_CHECKED,
	CHECKBOXSTATE_BULLET,

	CHECKBOX_NUM_STATES	
};

#define CHECKBOX_DEFAULT_STATE		CHECKBOXSTATE_UNCHECKED

#define CHECKBOX_CHECK_WIDTH		30.0f
#define CHECKBOX_CHECK_X_SPACE		8.0f

typedef struct TAG_CHECKBOX		:	public TEXTBOX
{
protected:

	ECheckBoxState		state;


	/*	--------------------------------------------------------------------------------
		Function 	: CHECKBOX::startDraw
		Purpose 	: set up conditions needed to draw the checkbox (eg. apply item's scale & position to object matrix)
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides TEXTBOX::startDraw to undo the reason that TEXTBOX::startDraw overrode PAGEITEM::startDraw in the first place!  It is the Great Circle of Life.
	*/
	virtual inline void startDraw(void)
	{
		PAGEITEM::startDraw();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: CHECKBOX::mainDraw
		Purpose 	: the core of the draw method - actually draw the checkbox
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides TEXTBOX::mainDraw
	*/
	virtual void mainDraw(void);


	/*	--------------------------------------------------------------------------------
		Function 	: CHECKBOX::getTextRect
		Purpose 	: get the rectangle into which the text must be fitted
		Parameters 	: 
		Returns 	: the rectangle into which the text must be fitted
		Info 		: this overrides TEXTBOX::getTextRect, to account for the part of this->optRect that is taken up by the check mark
	*/
	/*inline */RECTANGLE getTextRect(void) const;


public:

	static TBTexture*	checkTextures[CHECKBOX_NUM_STATES];


	/*	--------------------------------------------------------------------------------
		Function 	: CHECKBOX::initTextures
		Purpose 	: get pointers to all the check mark textures
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	static void initTextures(void)
	{
		TAG_CHECKBOX::checkTextures[CHECKBOXSTATE_UNCHECKED]	=bkLoadTexture(NULL, "checkboxcross.bmp", 0);
		TAG_CHECKBOX::checkTextures[CHECKBOXSTATE_CHECKED]		=bkLoadTexture(NULL, "checkboxtick.bmp", 0);
		TAG_CHECKBOX::checkTextures[CHECKBOXSTATE_BULLET]		=bkLoadTexture(NULL, "redbutton.bmp", 0);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: CHECKBOX::init
		Purpose 	: initialise the check box
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides TEXTBOX::init
	*/
	void init(void);


	/*	--------------------------------------------------------------------------------
		Function 	: CHECKBOX::TAG_CHECKBOX
		Purpose 	: constructor
		Parameters 	: (opt/none) index of string to hold
		Returns 	: new CHECKBOX instance 
		Info 		: this overrides TEXTBOX::TAG_TEXTBOX
	*/
	inline TAG_CHECKBOX(const EStringIndex string=NOSTRING)
	{
		this->init();

		this->setText(string);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: CHECKBOX::setCheck
		Purpose 	: set the check status of the the check box
		Parameters 	: new check status of the the check box
		Returns 	: this
		Info 		: 
	*/
	inline struct TAG_CHECKBOX* setCheck(const ECheckBoxState stateIn)
	{
		this->state=stateIn;

		return this;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: CHECKBOX::getCheck
		Purpose 	: get the check status of the the check box
		Parameters 	: 
		Returns 	: the check status of the the check box
		Info 		: 
	*/
	inline ECheckBoxState getCheck(void)
	{
		return this->state;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: CHECKBOX::getRequiredWidth
		Purpose 	: get the minimum required width of the the checkbox
		Parameters 	: 
		Returns 	: minimum required width of the the checkbox
		Info 		: this overrides TEXTBOX::getRequiredWidth
	*/
	inline float getRequiredWidth(void)
	{
		return TEXTBOX::getRequiredWidth()+((CHECKBOX_CHECK_WIDTH+CHECKBOX_CHECK_X_SPACE)*this->getAlignScale()*this->actualFontSize);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: CHECKBOX::getRequiredHeight
		Purpose 	: get the minimum required height of the the checkbox
		Parameters 	: 
		Returns 	: minimum required height of the the checkbox
		Info 		: this overrides TEXTBOX::getRequiredHeight
	*/
	inline float getRequiredHeight(void)
	{
		float	temp, temp2;
		
		temp=TEXTBOX::getRequiredHeight();

		temp2=CHECKBOX_CHECK_WIDTH*this->getAlignScale()*this->actualFontSize;

		return MAX(temp, temp2);// PP: assuming square check
	}


}CHECKBOX, _CHECKBOX BALIGN16;

#endif// PP: ndef __H_CHECKBOX