// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Popup.cpp
//   Purpose : Popups windows used for things such as toggling options
// Component : Taz
//		Info : 
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#include "global.h"				// PP: global stuff
//#include "maths.h"				// PP: maths stuff
#include "main.h"				// PP: main stuff
#include "control.h"			// PP: provide basic input and control functions
#include "popup.h"
#include "slider.h"
#include "volume.h"
#include "frontendmain.h"
#include "icon.h"
#include "display.h"
#include "PageItemFX.h"			// PP: Special effects for page items
#include "XboxStrings.h"		// PP: Xbox versions of some strings

#define BACKGROUNDALPHA 64


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
											            POPUP methods
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*  --------------------------------------------------------------------------------
	Function	: POPUP::createVolumePage
	Purpose		: Create a page for controlling a volume level
	Parameters	: ptr to update callback for the volume slider
	Returns		: 
	Info		: // PP: 
*/
void POPUP::createVolumePage(EPopupTypes type, const SliderUpdateFunc updateFunc)
{
	PAGE*	subPage;		// PP: sub-page to contain the slider and whatever the PC version needs

	// PP: create a subpage containing the slider and whatever the PC version needs...

	subPage=new PAGE("slider sub-page");

#if(BPLATFORM == PC)
		subPage->insertItem(volSl = new SLIDER(updateFunc,NULL,COLOUR(0,0,255,255),COLOUR(255,255,255,255)));
#else
		subPage->insertItem(volSl = new SLIDER(updateFunc,NULL,COLOUR(0,0,128,255),COLOUR(128,128,128,255)));
#endif // NH: BPLATFORM
		volSl->setYPadding(false)
			->setXAlign(PIFLAG_XALIGN_RIGHT)// PP: REMOUT: 
			->setAlignScale(0.7f)// PP: 
			->setDrawScale(0.5f)// PP: 
			->setSelectable(true);
		
		if (type == POPUP_SFXVOLUME)
			volSl->value = gameStatus.globalSettings.sfxVolume/(float)255;
		else if (type == POPUP_MUSICVOLUME)
			volSl->value = gameStatus.globalSettings.musicVolume/(float)255;		

#if BPLATFORM == PC
{
	subPage->insertItem(okIcon = new ICON("yellowtick.bmp"),false);
	okIcon->setScale(0.36f)
		->setXAlign(PIFLAG_XALIGN_LEFT)
		->setSelectable(true,true);		
}
#endif

	// PP: squeeze the sub-page's contents together
	subPage->setSlack(false);

	// PP: add the subpage containing the slider and whatever the PC version needs
	page->insertItem(subPage);

/*	
	page->insertItem(STR_AU_CHANGE_VOL) 
		->setWrap(true)
		->setStyle(TEXTSTYLE_INFO) 
		->setAlignScale(1.0f)
		->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setSelectable(false);


	page->insertItem(STR_DIALOGUE_OKCANCEL) 
		->setStyle(TEXTSTYLE_CONTROL) 
		->setYPadding(false)
		->setSelectable(false);
*/
	page->insertItem(STR_DIALOGUE_OKCANCEL) 
		// PP: ->setWrap(true)// PP: REMOUT: There's always been a slight problem with text wrapping, and it's most pronounced when wrapping is enabled for a very narrow textbox like this.  The problem might get fixed at some point.
		->setStyle(TS_info) 
		->setAlignScale(1.0f)
		->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setSelectable(false);

	page->selectItem(volSl);

// PP: REMOUT: Not sure what's changed (if anything), but I'm getting a null frontEndMenu.audioPage at this point (I haven't been to the frontend yet).
// PP: I suggest there might be nicer place, in the frontend code, to prevent the first sfx-test explosion
/*
	// NH: Set this flag so that an explosion isn't done on first entry of SFX
	if (type == POPUP_SFXVOLUME)
		frontEndMenu.audioPage->explosionDone = true;
*/
}


/*  --------------------------------------------------------------------------------
	Function	: POPUP::TAG_POPUP
	Purpose		: Default constructor
	Parameters	: type (type of popup to create), selected (currently selected option),
					headerString (Popup header instruction), selectionItem1 (selection 1) ,
					selectionItem2 (selection 2), (PP:) (opt/true) flag indicating that the popup is
					to use the full screen rather than the screen indexed by 'player'
	Returns		: 
	Info		: // PP: this also opens the popup
*/
POPUP::TAG_POPUP(EPopupTypes type, int selected, EStringIndex headerString, EStringIndex selectionItem1,
				EStringIndex selectionItem2, EStringIndex controlString, char player, const bool fullscreen, float controlDelay)
{
	TEXTBOX	*text;
	MENU_SOUNDS	*popupSoundsPtr;

	// PP: set up initial response code
	this->response=(-1);// PP: no response

	this->type = type;

	this->controlDelay = controlDelay;

	//*



	/*/

	// TP: Set up rect
	if (headerString == STR_PRESS_Y_TO_FINISH_LEVEL)
		setRect(RECTANGLE(-0.49f,0.49f,-0.49f,-0.2f));
	else
	{
		if (gameStatus.multiplayer.numScreens == 2)
		{
			if (player == 0)
				setRect(RECTANGLE(-0.49f,0.49f,0.01f,0.2f));
			else
				setRect(RECTANGLE(-0.49f,0.49f,-0.49f,-0.3f));
		}
	}

  //*/

	this->setEffectChooser(&popupPIEChooser);

	// TP: Set up textboxes
	page = addPage("POPUPPAGE");
	
	// PP: fit the page tightly round its contents
	page->setVSlack(false);// PP: NOTE: disabling v-slack only - we don't wanna squish things together on the X axis

	// PP: add some margins but
#define POPUP_MARGIN	0.0f// PP: 0.025f
	page->setMargin(POPUP_MARGIN);

	// PP: specify that the page's outline box will be loose on the X but tight on the Y
	page->setBoxStyle(false, true);

	// PP: pull the page to the bottom of the screen
	page->setYAlign(PIFLAG_YALIGN_BOTTOM);

	// PP: set background and border colours for the page
	page->setBackgroundColour(COLOUR(0,0,0,BACKGROUNDALPHA));
	page->setBorderColour(COLOUR(0,0,0,255));
	
	page->setNavigation(true);

	page->setController(player);

	page->menuSounds = popupSoundsPtr = new MENU_SOUNDS;

	// NH: Setup the popups sounds

	strcpy(popupSoundsPtr->navigateSound.name, "moveselection.wav");
	strcpy(popupSoundsPtr->selectSound.name, "select.wav");
	strcpy(popupSoundsPtr->errorSound.name, "cancel.wav");
	strcpy(popupSoundsPtr->backupSound.name, "deselect.wav");
	popupSoundsPtr->navigateSound.volume = 255;
	popupSoundsPtr->selectSound.volume = 255;
	popupSoundsPtr->errorSound.volume = 255;
	popupSoundsPtr->backupSound.volume = 255;

	// PP: page->setEffect(&PIE_popupSpin);

	// PP: register menu action callback
	page->setMenuActionFunc(POPUP::menuActionFunc, this);

	switch (type)
	{
	case POPUP_CONFIRM:
		if (headerString)
			page->insertItem(text = new TEXTBOX(headerString));			
		else
			page->insertItem(text = new TEXTBOX(STR_ARE_YOU_SURE));
		
		text->setStyle(TS_info);
		text->setSelectable(false);
		text->setWrap(true);
		
		page->insertItem(selection1Tb = new TEXTBOX(selectionItem1), true);		
		page->insertItem(selection2Tb = new TEXTBOX(selectionItem2), false);
		
		page->insertItem(controlString) 
			->setStyle(TS_control) 
			->setSelectable(false);
	
		switch (selected)
		{
		case 1:
			page->selectItem(selection1Tb);
			break;
		case 0:
			page->selectItem(selection2Tb);
			break;
		}

		if (controlDelay)
		{
			page->setNavigation(false);
		}

		break;

	case POPUP_RETRYLEVEL:
		page->insertItem(text = new TEXTBOX(STR_RETRY_LEVEL));
		
		text->setStyle(TS_info);
		text->setSelectable(false);
		text->setWrap(true);
		
		page->insertItem(selection1Tb = new TEXTBOX(STR_DIALOGUE_YES), true);		
		page->insertItem(selection2Tb = new TEXTBOX(STR_DIALOGUE_NO), false);
		
		page->insertItem(STR_PRESS_X_TO_CONTINUE) 
			->setStyle(TS_control) 
			->setSelectable(false);
	
		switch (selected)
		{
		case 1:
			page->selectItem(selection1Tb);
			break;
		case 0:
			page->selectItem(selection2Tb);
			break;
		}

		page->setNavigation(false);

		break;
		
	case POPUP_DIFFICULTY:
		page->insertItem(selection1Tb = new TEXTBOX(STR_GO_DIFF_EASY), true);		
		page->insertItem(selection2Tb = new TEXTBOX(STR_GO_DIFF_MEDIUM), false);
		page->insertItem(selection3Tb = new TEXTBOX(STR_GO_DIFF_HARD), false);
		
		page->insertItem(STR_X_TO_SELECT_Y_TO_RETURN) 
			->setStyle(TS_control) 
			->setSelectable(false);
	
		switch (selected)
		{
		case 1:
			page->selectItem(selection1Tb);
			break;
		case 2:
			page->selectItem(selection2Tb);
			break;
		case 3:
			page->selectItem(selection3Tb);
			break;
		}		
		break;

	case POPUP_ON_OFF:
		page->insertItem(selection1Tb = new TEXTBOX(STR_FRONTEND_ON), true);		
		page->insertItem(selection2Tb = new TEXTBOX(STR_FRONTEND_OFF), false);
		
		page->insertItem(STR_X_TO_SELECT_Y_TO_RETURN) 
			->setStyle(TS_control) 
			->setSelectable(false);
		
		switch (selected)
		{
		case 1:
			page->selectItem(selection1Tb);
			break;
		case 0:
			page->selectItem(selection2Tb);
			break;
		}
		break;

	case POPUP_SFXVOLUME:
		{			
			// NH: Null the nav sound as this click as you slid up and down the slider bar as well which just seemed wrong to me
			strcpy(popupSoundsPtr->navigateSound.name,"");

			// PP: create a page for controlling the SFX volume level
			this->createVolumePage(type, FrontEnd_SFXSliderUpdateFunc);
		}
		break;

	case POPUP_MUSICVOLUME:
		{
			// NH: Null the nav sound as this click as you slid up and down the slider bar as well which just seemed wrong to me
			strcpy(popupSoundsPtr->navigateSound.name,"");

			// PP: create a page for controlling the music volume level
			this->createVolumePage(type, FrontEnd_musicSliderUpdateFunc);
		}
		break;
	}

	// PP: it's best not to set a book's screen after it's open (the code isn't there yet to update on the fly - hasn't been needed yet)
	if(!fullscreen)
	{
		this->setScreen(player);// PP: set screen to match 'player' index (ie player who can control the popup)
	}

	open();
}

/*  --------------------------------------------------------------------------------
	Function	: POPUP::~TAG_POPUP
	Purpose		: Default destructor
	Parameters	: 
	Returns		: 
	Info		: 
*/
POPUP::~TAG_POPUP()
{
#ifndef INFOGRAMES
	if(this->flags & BKFLAG_BOOKLIST)
	{
		if(~this->flags & BKFLAG_FREEWHENCLOSED)// PP: unless this is being deleted by the Book system
		{
			// PP: cause a crash if someone is trying to delete the popup
//			ASSERTM(false, "PP: Instead of deleting this popup, please pass it into BookList_closeAndFree, which will close it gracefully and then free it for you.\n");
		}
	}
#endif// PP: ndef INFOGRAMES

	SAFE_DELETE(page->menuSounds);
}


/*	--------------------------------------------------------------------------------
	Function 	: POPUP::menuActionFunc
	Purpose 	: callback that responds to menu actions on POPUPS
	Parameters 	: ref to a MENUACTION_INFO structure (TextFX.h)
	Returns 	: 
	Info 		: // PP: Must match the MenuActionFunc typedef (TextFX.h)
*/
void POPUP::menuActionFunc(MENUACTION_INFO& info)
{
	POPUP*	popup;


	// PP: get ptr to POPUP in question
	popup=(POPUP*)(info.context);

	// PP: code player's response...

	if(info.action == MENUACTION_BACK)
	{
		popup->response=0;
	}
	else if(info.action == MENUACTION_PRESS)
	{
		// NH: This or is needed (unless you can think of a different method) to stop the Volume Slider types shutting down when conttrolling with mouse
		if ((info.item == popup->selection1Tb) || (info.item == popup->okIcon) || (info.item == popup->volSl && !controller1.mb1DebounceChannel->value))
		{
			popup->response=1;
		}
		else if (info.item == popup->selection2Tb)
		{
			popup->response=2;
		}
		else if (info.item == popup->selection3Tb)
		{
			popup->response=3;
		}
	}
}


// PP: NOTE: I deleted the remmed-out POPUP::update; it's in sourcesafe's version 45 if you need it
