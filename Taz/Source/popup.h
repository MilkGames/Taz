// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Popup.cpp
//   Purpose : Popups windows used for things such as toggling options
// Component : Taz
//		Info : 
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __POPUP_H__
#define __POPUP_H__

#include "Textfx.h"						// NH: Text effects, eg. button glyphs, coloured text
#include "icon.h"

/* NH: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
											              POPUP
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

enum EPopupTypes
{
	POPUP_CONFIRM,
	POPUP_RETRYLEVEL,
	POPUP_ON_OFF,
	POPUP_DIFFICULTY,
	POPUP_SFXVOLUME,
	POPUP_MUSICVOLUME,
};

typedef class TAG_POPUP : public BOOK
{
private:

	PAGE		*page;
	int32		response;
	EPopupTypes	type;


	/*  --------------------------------------------------------------------------------
		Function	: POPUP::createVolumePage
		Purpose		: Create a page for controlling a volume level
		Parameters	: ptr to update callback for the volume slider
		Returns		: 
		Info		: // PP: meet Kimmi - she fell out of my breakfast!
	*/
	void createVolumePage(EPopupTypes type, const SliderUpdateFunc updateFunc);


	/*	--------------------------------------------------------------------------------
		Function 	: POPUP::menuActionFunc
		Purpose 	: callback that responds to menu actions on POPUPS
		Parameters 	: ref to a MENUACTION_INFO structure (TextFX.h)
		Returns 	: 
		Info 		: // PP: Must match the MenuActionFunc typedef (TextFX.h)
	*/
	static void menuActionFunc(MENUACTION_INFO& info);


public:

	struct TAG_TEXTBOX	*titleTb;
	struct TAG_TEXTBOX	*selection1Tb;
	struct TAG_TEXTBOX	*selection2Tb;
	struct TAG_TEXTBOX	*selection3Tb;
	struct TAG_ICON		*okIcon;
	struct TAG_SLIDER	*volSl;
	struct TAG_BOOK		*infoSubBook;
	struct TAG_BOOK		*controlSubBook;
	float		controlDelay;			// TP: time before control input is recognized

	/*  --------------------------------------------------------------------------------
		Function	: POPUP::TAG_POPUP
		Purpose		: Default constructor
		Parameters	: type (type of popup to create), selected (currently selected option),
						headerString (Popup header instruction), selectionItem1 (selection 1) ,
						selectionItem2 (selection 2), (PP:) (opt/true) flag indicating that the popup is
						to use the full screen rather than the screen indexed by 'player'
						controlDelay - time before control input is recognized
		Returns		: 
		Info		: // PP: this also opens the popup
	*/
	TAG_POPUP(EPopupTypes type, int selected = 1, EStringIndex headerString = STR_DIALOGUE_YES, EStringIndex selectionItem1 = STR_DIALOGUE_YES,
				EStringIndex selectionItem2 = STR_DIALOGUE_NO, EStringIndex controlString = STR_X_TO_SELECT_Y_TO_RETURN, char player = 0, const bool fullscreen=true, float controlDelay=0.0f);

	/*  --------------------------------------------------------------------------------
		Function	: POPUP::~TAG_POPUP
		Purpose		: Default destructor
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	~TAG_POPUP();

	/*  --------------------------------------------------------------------------------
		Function	: POPUP::getResponse
		Purpose		: get the player's response to the popup
		Parameters	: 
		Returns		: 1-based index of chosen option, or 0 for cancel, or -1 for no response
		Info		: // PP: replaces POPUP::Update
	*/
	inline int32 getResponse(void) const
	{
		return this->response;
	}


} POPUP, _POPUP BALIGN16;

#endif // NH: ndef __POPUP_H__