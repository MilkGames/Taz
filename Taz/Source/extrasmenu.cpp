// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : startgamemenu.cpp
//   Purpose : functions to create and control the extras menu
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "textfx.h"
#include "control.h"
#include "frontendmenu.h"
#include "extrasmenu.h"
#include "credits.h"
#include "frontendcamera.h"
#include "frontendmain.h"
#include "animation.h"


/*	--------------------------------------------------------------------------------
	Function 	: CreateExtrasMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void CreateExtrasMenuPage(FRONTENDMENU *menu)
{
	EXTRASPAGE		*ptr;
	PAGE			*pagePtr;
	TEXTBOX			*dummyBox;
	PAGE			*subPage;

	ptr = menu->extrasPage = (EXTRASPAGE*)ZALLOC(sizeof(EXTRASPAGE));
	if(!ptr)
	{
		bkPrintf("*** WARNING ***, could not malloc game extras page/n");
		return;
	}
	// NH: Add Page (Extras)
	pagePtr = ptr->page = menu->frontEndMenuBook->addPage(menuNames[MENU_EXTRAS]);
	pagePtr->setNavigation(true);

	// PP: navigation Y-wrapping is allowed, but only in response to a debounced input, NOT an auto-repeat input
	pagePtr->setNavigationWrap(PAGEFLAG_NAVWRAP_YDEBOUNCE);
	
	// NH: Add Title
	pagePtr->insertItem(ptr->titleTb = new TEXTBOX(STR_EXTRAS),false);
	ptr->titleTb->setStyle(TS_title)
		->setYAlign(PIFLAG_YALIGN_TOP)
		->setSelectable(false);

	// NH: Add Menu Items (Credits)
	pagePtr->insertItem(ptr->creditsTb = new TEXTBOX(STR_EX_CREDITS),true);
	ptr->creditsTb->setYPadding(false);

	// NH: Add Menu Items (Gallery)
	pagePtr->insertItem(ptr->galleryTb = new TEXTBOX(STR_EX_GALLERY),true);
	ptr->galleryTb->setYPadding(false);

	// NH: Add Menu Items (Secrets)
	pagePtr->insertItem(ptr->secretsTb = new TEXTBOX(STR_EX_SECRETS),true);
	ptr->secretsTb->setYPadding(false);
	
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
	ADD_INFOSUBBOOK_PAGE(CREDITS,STR_EX_CREDITS_ACTION);
	ADD_INFOSUBBOOK_PAGE(GALLERY,STR_EX_GALLERY_ACTION);
	ADD_INFOSUBBOOK_PAGE(SECRETS,STR_EX_SECRETS_ACTION);

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
	ADD_CONTROLSUBBOOK_PAGE(SELECT,STR_X_TO_SELECT_Y_TO_RETURN);

	ptr->infoSubBook->open();// PP: books default to closed
	ptr->controlSubBook->open();
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateGameOptionsMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateExtrasMenuPage(FRONTENDMENU *menu, PADCONTROLSTATUS *controller)
{
	EXTRASPAGE	 *ptr = menu->extrasPage;
	static bool returning = false;
	
	// NH: Credits selected
	if (ptr->page->selectedItem == ptr->creditsTb)
	{
		if(OKBUTTON)
		{
			// NH: Play padoink sound. Would be nice to be in PageFX, but not sure how to do it so this will work for now
			menu->creditsPage->page->playNavSound(menu->creditsPage->page->menuSounds, EMSFX_SELECT);
			creditChangeTimer = gameStatus.gameTime + CREDITSDELAY;
			creditsPageNumber = 0;
			menu->creditsPage->creditsSubBook->gotoPageNumber(0, true);
			menu->frontEndMenuBook->gotoPage(menuNames[MENU_CREDITS]);
			menu->currentPage = MENU_CREDITS;		

			SetFrontEndCameraDestination(FECAM_EXTRAS,FECAM_EXTRAS_CREDITS);
		}
		ptr->infoSubBook->gotoPage("CREDITS");
		ptr->controlSubBook->gotoPage("SELECT");
	}
	
	// NH: Gallery selected
	else if (ptr->page->selectedItem == ptr->galleryTb)
	{
		if(OKBUTTON)
		{
			// NH: Play padoink sound. Would be nice to be in PageFX, but not sure how to do it so this will work for now
			menu->galleryPage->page->playNavSound(menu->galleryPage->page->menuSounds, EMSFX_SELECT);
			menu->galleryPage->subState = EX_GALLERYS;
			menu->galleryPage->galleryLoadState = EG_NONE;
			menu->frontEndMenuBook->gotoPage(menuNames[MENU_GALLERY]);
			menu->currentPage = MENU_GALLERY;
			ptr->subState = EX_GALLERY;
			//SetFrontEndCameraDestination(MENU_MAIN,MENU_GALLERY);
			//menu->twoPlayerPage->subState = TP_RACE;						

			SetFrontEndCameraDestination(FECAM_EXTRAS,FECAM_EXTRAS_GALLERY);
		}
		
		ptr->infoSubBook->gotoPage("GALLERY");
		ptr->controlSubBook->gotoPage("SELECT");
	}
	
	// NH: Secrets selected
	else if (ptr->page->selectedItem == ptr->secretsTb)
	{
		if(OKBUTTON)
		{
			// NH: Play padoink sound. Would be nice to be in PageFX, but not sure how to do it so this will work for now
			menu->secretsPage->page->playNavSound(menu->secretsPage->page->menuSounds, EMSFX_SELECT);
			menu->frontEndMenuBook->gotoPage(menuNames[MENU_SECRETS]);
			menu->currentPage = MENU_SECRETS;	

			menu->secretsPage->page->selectItem(menu->secretsPage->page->getFirstSelectableItem(true));

			SetFrontEndCameraDestination(FECAM_EXTRAS,FECAM_EXTRAS_SECRETS);						
		}
		ptr->infoSubBook->gotoPage("SECRETS");
		ptr->controlSubBook->gotoPage("SELECT");
	}

	//triangle to return us back to main menu
	if(CANCELBUTTON)
	{
		// NH: Play bowhomb sound. Would be nice to be in PageFX, but not sure how to do it so this will work for now
		menu->extrasPage->page->playNavSound(menu->extrasPage->page->menuSounds, EMSFX_BACK);
		frontEndAnimInfo.requestState = GOING_HOME;

		menu->currentPage = MENU_MAIN;
		ChooseFrontEndAnimationPath(MENU_MAIN);

		menu->frontEndMenuBook->gotoPage(menuNames[MENU_MAIN]);
		
		SetFrontEndCameraDestination(FECAM_EXTRAS,FECAM_MAIN);
	}
}