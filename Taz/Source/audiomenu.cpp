// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : startgamemenu.cpp
//   Purpose : functions to create and control the audio options menu
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "TextFX.h"
#include "control.h"
#include "pause.h"
#include "animation.h"
#include "audiomenu.h"
#include "frontendmain.h"
#include "music.h"

EPopupTypes currentAudioPopup;

int storedVolume;

/*	--------------------------------------------------------------------------------
	Function 	: CreateAudioOptionsMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void CreateAudioOptionsMenuPage(FRONTENDMENU *menu)
{
	AUDIOPAGE		*ptr;
	PAGE			*pagePtr, *subPage, *audioFreePage;
	TEXTBOX			*dummyBox;

	RECTANGLE freePos;
	
	freePos.left		= -0.5f;
	freePos.right		= 0.5f;
	freePos.top			= 0.12f;
	freePos.bottom		= -0.35f;

	ptr = menu->audioPage = (AUDIOPAGE*)ZALLOC(sizeof(AUDIOPAGE));
	if(!ptr)
	{
		bkPrintf("*** WARNING ***, could not malloc audio options page/n");
		return;
	}
	// NH: Add Page
	pagePtr = ptr->page = menu->frontEndMenuBook->addPage(menuNames[MENU_AUDIO]);
	pagePtr->setNavigation(true);

	// PP: navigation Y-wrapping is allowed, but only in response to a debounced input, NOT an auto-repeat input
	pagePtr->setNavigationWrap(PAGEFLAG_NAVWRAP_YDEBOUNCE);
	
	// NH: Add Title
	pagePtr->insertItem(ptr->titleTb = new TEXTBOX(STR_AUDIO_OPTIONS),false);
	ptr->titleTb->setStyle(TS_title)
		->setYAlign(PIFLAG_YALIGN_TOP)
		->setSelectable(false);
	
	pagePtr->insertFreeItem(audioFreePage = new PAGE(), freePos);

	// NH: Add Menu Item (Music Text)
	audioFreePage->insertItem(ptr->musicVolTb = new TEXTBOX(STR_AU_MUSIC_VOLUME),true);
	ptr->musicVolTb->setAlignScale(1.2f)
		->setYPadding(false);
	
	// NH: Add Menu Item (SFX Text)
	audioFreePage->insertItem(ptr->sfxVolTb = new TEXTBOX(STR_AU_SFX_VOLUME),true);
	ptr->sfxVolTb->setAlignScale(1.2f)
		->setYPadding(false);

	// NH: INFO
	// NH: Add Info Text Sub Book
	pagePtr->insertItem(ptr->infoSubBook=new BOOK("INFO SUB BOOK"));
	ptr->infoSubBook->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setSelectable(false);

	// NH: Add Info Text (Using Macro)
#define ADD_INFOSUBBOOK_PAGE(name, infoText) \
	subPage=ptr->infoSubBook->addPage(#name); \
	subPage->insertItem(infoText) \
		->setStyle(TS_info) \
		->setYAlign(PIFLAG_YALIGN_BOTTOM) \
		->setAlignScale(1.2f) \
		->setSelectable(false);

	// NH: Add all info pages (using the macro).  Seperate pages allow for fading between the actions
	ADD_INFOSUBBOOK_PAGE(CHANGE_MUSIC_VOL,STR_ADJUST_MUSIC_VOLUME);
	ADD_INFOSUBBOOK_PAGE(CHANGE_SFX_VOL,STR_ADJUST_SFX_VOLUME);

	// NH: CONTROL
	// NH: Add Control Text Sub Book
	pagePtr->insertItem(ptr->controlSubBook=new BOOK("CONTROL SUB BOOK"));
	ptr->controlSubBook->setYPadding(false)
		->setSelectable(false);

	// NH: Add Control Text (Using Macro)
#define ADD_CONTROLSUBBOOK_PAGE(name, controlText) \
	subPage=ptr->controlSubBook->addPage(#name); \
	subPage->insertItem(controlText) \
		->setStyle(TS_control) \
		->setYPadding(false) \
		->setSelectable(false);

	// NH: Add all control pages (using the macro).  Seperate pages allow for fading between the actions
	ADD_CONTROLSUBBOOK_PAGE(RETURN,STR_X_TO_SELECT_Y_TO_RETURN);

	ptr->infoSubBook->open();// PP: books default to closed
	ptr->controlSubBook->open();
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateAudioOptionsMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateAudioOptionsMenuPage(FRONTENDMENU *menu, PADCONTROLSTATUS *controller)
{
	static int storedVolume;
	static bool returning = false;
	int retval;
	
	AUDIOPAGE	*ptr = menu->audioPage;

	// NH: If popup open
	if (frontEndMenu.popUpOptions)
	{
		// PP: get the player's response to the popup
		retval = frontEndMenu.popUpOptions->getResponse();

		if (retval!=-1)
		{
			// NH: Close pop up
			// PP: REMOUT: nah		frontEndMenu.popUpOptions->close(0.0f,TRUE);
			BookList_closeAndFree(frontEndMenu.popUpOptions);
			menu->frontEndMenuBook->getPage(menuNames[MENU_AUDIO])->setNavigation(true);
			ptr->infoSubBook->open();
			ptr->controlSubBook->open();
			
			if (currentAudioPopup == POPUP_MUSICVOLUME)
			{
				// NH: Restore Old volume
				if (retval == 0)
				{
					// NH: Happens correctly, but an update is called that sets the volume to that of the slider again! Bugger! 8(
					gameStatus.globalSettings.musicVolume = storedVolume;
					bsSetGlobalAudioStreamVolume(gameStatus.globalSettings.musicVolume);
				}
			}
			else if (currentAudioPopup == POPUP_SFXVOLUME)
			{
				// NH: Restore Old volume
				if (retval == 0)
				{
					gameStatus.globalSettings.sfxVolume = storedVolume;
					// TP: bsSetGlobalAudioStreamVolume(gameStatus.globalSettings.sfxVolume);
					// TP: Did you mean this?
					bsSetGlobalSampleVolume(gameStatus.globalSettings.sfxVolume);
				}				
			}
		}
	}
	
	//triangle to return us back to main menu
	else if(CANCELBUTTON)
	{
		// NH: Play bowhomb sound. Would be nice to be in PageFX, but not sure how to do it so this will work for now
		menu->audioPage->page->playNavSound(menu->audioPage->page->menuSounds, EMSFX_BACK);
		frontEndAnimInfo.requestState = GOING_HOME;

		menu->currentPage = MENU_MAIN;
		//ChooseFrontEndAnimationPath(MENU_MAIN);

		menu->frontEndMenuBook->gotoPage(menuNames[MENU_MAIN]);
		
		SetFrontEndCameraDestination(FECAM_AUDIO,FECAM_MAIN);
	}
	// NH: Music Vol Selected
	else if (ptr->page->selectedItem == ptr->musicVolTb)
	{
		if (frontEndAnimInfo.currentState != AM_GRAMOPHONE)
		{
			frontEndAnimInfo.requestState = AM_GRAMOPHONE;
		}
		
		if ((mouse.active && controller1.mb1DebounceChannel->value) || (controller1.crossDebounceChannel->value) || (controller1.menuSelectDebounceChannel->value))
		{
			controller1.mb1DebounceChannel->value = controller1.mb1Channel->value = controller1.crossDebounceChannel->value = controller1.menuSelectDebounceChannel->value = false;
			
			if (ptr->subState != AU_MUSICVOL)
			{
				ptr->subState = AU_MUSICVOL;
			//	RequestTazFrontEndAnimation("grabgramophone");
			}

			if (!frontEndMenu.popUpOptions)
			{
				storedVolume = gameStatus.globalSettings.musicVolume;
				currentAudioPopup = POPUP_MUSICVOLUME;
				menu->frontEndMenuBook->getPage(menuNames[MENU_AUDIO])->setNavigation(false);
				ptr->infoSubBook->close();
				ptr->controlSubBook->close();

				frontEndMenu.popUpOptions = new POPUP(currentAudioPopup, 0);
			}
		}
		ptr->infoSubBook->gotoPage("CHANGE_MUSIC_VOL");
		ptr->controlSubBook->gotoPage("RETURN");
	}
	// NH: SFX Vol Selected
	else if (ptr->page->selectedItem == ptr->sfxVolTb)
	{
		if (frontEndAnimInfo.currentState != AM_PLUNGER)
		{
			frontEndAnimInfo.requestState = AM_PLUNGER;
		}
		
		if ((mouse.active && controller1.mb1DebounceChannel->value) || (controller1.crossDebounceChannel->value) || (controller1.menuSelectDebounceChannel->value))
		{
			controller1.mb1DebounceChannel->value = controller1.mb1Channel->value = controller1.crossDebounceChannel->value = controller1.menuSelectDebounceChannel->value = false;
		
			if (ptr->subState != AU_SFXVOL)
			{
				ptr->subState = AU_SFXVOL;
			//	RequestTazFrontEndAnimation("grabplunger");
			}
			
			if (!frontEndMenu.popUpOptions)
			{
				storedVolume = gameStatus.globalSettings.sfxVolume;
				currentAudioPopup = POPUP_SFXVOLUME;
				menu->frontEndMenuBook->getPage(menuNames[MENU_AUDIO])->setNavigation(false);
				ptr->infoSubBook->close();
				ptr->controlSubBook->close();
	
				frontEndMenu.popUpOptions = new POPUP(currentAudioPopup, 0);
			}
		}
		ptr->infoSubBook->gotoPage("CHANGE_SFX_VOL");
		ptr->controlSubBook->gotoPage("RETURN");
	}
}
