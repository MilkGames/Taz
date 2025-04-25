// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : secretsmenu.cpp
//   Purpose : functions to create and control the secrets menu
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "globalsettings.h"
#include "TextFX.h"
#include "control.h"
#include "frontendmain.h"
#include "files.h"
#include "display.h"
#include "Cheats.h"						// PP: Cheats

/*	--------------------------------------------------------------------------------
	Function 	: CreateSecretsMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void CreateSecretsMenuPage(FRONTENDMENU *menu)
{
	SECRETSPAGE		*ptr;
	PAGE			*pagePtr, *subPage;
	TEXTBOX			*dummyBox;

	ptr = menu->secretsPage = (SECRETSPAGE*)ZALLOC(sizeof(SECRETSPAGE));
	if(!ptr)
	{
		bkPrintf("*** WARNING ***, could not malloc secrets page/n");
		return;
	}
	// NH: Add Page
	pagePtr = ptr->page = menu->frontEndMenuBook->addPage(menuNames[MENU_SECRETS]);
	pagePtr->setNavigation(true);

	// PP: navigation Y-wrapping is allowed, but only in response to a debounced input, NOT an auto-repeat input
	pagePtr->setNavigationWrap(PAGEFLAG_NAVWRAP_YDEBOUNCE);

	// NH: Add Title
	pagePtr->insertItem(ptr->titleTb = new TEXTBOX(STR_EX_SECRETS),false);
	ptr->titleTb->setStyle(TS_title)
		->setYAlign(PIFLAG_YALIGN_TOP)
		->setSelectable(false);

	pagePtr->insertItem(ptr->levelTb[0] = new TEXTBOX(STR_LEVELNAME_SAFARI), true);
	ptr->levelTb[0]->setYPadding(false);

	pagePtr->insertItem(ptr->levelTb[1] = new TEXTBOX(STR_LEVELNAME_ICEDOME), true);
	ptr->levelTb[1]->setYPadding(false);
	
	pagePtr->insertItem(ptr->levelTb[2] = new TEXTBOX(STR_LEVELNAME_AQUA), true);
	ptr->levelTb[2]->setYPadding(false);
	
	pagePtr->insertItem(ptr->levelTb[3] = new TEXTBOX(STR_LEVELNAME_DEPTSTR), true);
	ptr->levelTb[3]->setYPadding(false);
	
	pagePtr->insertItem(ptr->levelTb[4] = new TEXTBOX(STR_LEVELNAME_MUSEUM), true);
	ptr->levelTb[4]->setYPadding(false);
	
	pagePtr->insertItem(ptr->levelTb[5] = new TEXTBOX(STR_LEVELNAME_CONSTRUCT), true);
	ptr->levelTb[5]->setYPadding(false);
	
	pagePtr->insertItem(ptr->levelTb[6] = new TEXTBOX(STR_LEVELNAME_GRANDC), true);
	ptr->levelTb[6]->setYPadding(false);
	
	pagePtr->insertItem(ptr->levelTb[7] = new TEXTBOX(STR_LEVELNAME_GOLDMINE), true);
	ptr->levelTb[7]->setYPadding(false);
	
	pagePtr->insertItem(ptr->levelTb[8] = new TEXTBOX(STR_LEVELNAME_GHOSTTOWN), true);
	ptr->levelTb[8]->setYPadding(false);
	
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
	ADD_INFOSUBBOOK_PAGE(NONE_AVAILABLE,STR_NONE_AVAILABLE);
	ADD_INFOSUBBOOK_PAGE(SELECT,STR_SELECT_GAME);		// NH: Should drop TP (and not Tony!)
	
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
	Function 	: UpdateSecretsMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateSecretsMenuPage(FRONTENDMENU *menu, PADCONTROLSTATUS *controller)
{
	SECRETSPAGE			*ptr = menu->secretsPage;
	PAGE				*pagePtr = menu->frontEndMenuBook->getCurrentPage();
	bool				noneAvailable = true;

	if ((gameStatus.globalSettings.saveInfo.saveSlot[0].level[SCENE_SAFARI].pickupsCollected >= 100) || (gameStatus.globalSettings.saveInfo.saveSlot[1].level[SCENE_SAFARI].pickupsCollected >= 100) || (gameStatus.globalSettings.saveInfo.saveSlot[2].level[SCENE_SAFARI].pickupsCollected >= 100) || (CheatActive(CHEAT_OPEN_BONUS_GAMES)))
	{
		ptr->levelTb[0]->setConcealed(false);
		noneAvailable = false;
	}
	else
	{
		ptr->levelTb[0]->setConcealed(true);
	}

	if ((gameStatus.globalSettings.saveInfo.saveSlot[0].level[SCENE_ICEDOME].pickupsCollected >= 100) || (gameStatus.globalSettings.saveInfo.saveSlot[1].level[SCENE_ICEDOME].pickupsCollected >= 100) || (gameStatus.globalSettings.saveInfo.saveSlot[2].level[SCENE_ICEDOME].pickupsCollected >= 100) || (CheatActive(CHEAT_OPEN_BONUS_GAMES)))
	{
		ptr->levelTb[1]->setConcealed(false);
		noneAvailable = false;
	}
	else
	{
		ptr->levelTb[1]->setConcealed(true);
	}
	
	if ((gameStatus.globalSettings.saveInfo.saveSlot[0].level[SCENE_AQUA].pickupsCollected >= 100) || (gameStatus.globalSettings.saveInfo.saveSlot[1].level[SCENE_AQUA].pickupsCollected >= 100) || (gameStatus.globalSettings.saveInfo.saveSlot[2].level[SCENE_AQUA].pickupsCollected >= 100) || (CheatActive(CHEAT_OPEN_BONUS_GAMES)))
	{
		ptr->levelTb[2]->setConcealed(false);
		noneAvailable = false;
	}
	else
	{
		ptr->levelTb[2]->setConcealed(true);
	}
	
	if ((gameStatus.globalSettings.saveInfo.saveSlot[0].level[SCENE_DEPTSTR].pickupsCollected >= 100) || (gameStatus.globalSettings.saveInfo.saveSlot[1].level[SCENE_DEPTSTR].pickupsCollected >= 100) || (gameStatus.globalSettings.saveInfo.saveSlot[2].level[SCENE_DEPTSTR].pickupsCollected >= 100) || (CheatActive(CHEAT_OPEN_BONUS_GAMES)))
	{
		ptr->levelTb[3]->setConcealed(false);
		noneAvailable = false;
	}
	else
	{
		ptr->levelTb[3]->setConcealed(true);
	}
	
	if ((gameStatus.globalSettings.saveInfo.saveSlot[0].level[SCENE_MUSEUM].pickupsCollected >= 100) || (gameStatus.globalSettings.saveInfo.saveSlot[1].level[SCENE_MUSEUM].pickupsCollected >= 100) || (gameStatus.globalSettings.saveInfo.saveSlot[2].level[SCENE_MUSEUM].pickupsCollected >= 100) || (CheatActive(CHEAT_OPEN_BONUS_GAMES)))
	{
		ptr->levelTb[4]->setConcealed(false);
		noneAvailable = false;
	}
	else
	{
		ptr->levelTb[4]->setConcealed(true);
	}
	
	if ((gameStatus.globalSettings.saveInfo.saveSlot[0].level[SCENE_CONSTRUCT].pickupsCollected >= 100) || (gameStatus.globalSettings.saveInfo.saveSlot[1].level[SCENE_CONSTRUCT].pickupsCollected >= 100) || (gameStatus.globalSettings.saveInfo.saveSlot[2].level[SCENE_CONSTRUCT].pickupsCollected >= 100) || (CheatActive(CHEAT_OPEN_BONUS_GAMES)))
	{
		ptr->levelTb[5]->setConcealed(false);
		noneAvailable = false;
	}
	else
	{
		ptr->levelTb[5]->setConcealed(true);
	}
	
	if ((gameStatus.globalSettings.saveInfo.saveSlot[0].level[SCENE_GRANDCANYON].pickupsCollected >= 100) || (gameStatus.globalSettings.saveInfo.saveSlot[1].level[SCENE_GRANDCANYON].pickupsCollected >= 100) || (gameStatus.globalSettings.saveInfo.saveSlot[2].level[SCENE_GRANDCANYON].pickupsCollected >= 100) || (CheatActive(CHEAT_OPEN_BONUS_GAMES)))
	{
		ptr->levelTb[6]->setConcealed(false);
		noneAvailable = false;
	}
	else
	{
		ptr->levelTb[6]->setConcealed(true);
	}
	
	if ((gameStatus.globalSettings.saveInfo.saveSlot[0].level[SCENE_GOLDMINE].pickupsCollected >= 100) || (gameStatus.globalSettings.saveInfo.saveSlot[1].level[SCENE_GOLDMINE].pickupsCollected >= 100) || (gameStatus.globalSettings.saveInfo.saveSlot[2].level[SCENE_GOLDMINE].pickupsCollected >= 100) || (CheatActive(CHEAT_OPEN_BONUS_GAMES)))
	{
		ptr->levelTb[7]->setConcealed(false);
		noneAvailable = false;
	}
	else
	{
		ptr->levelTb[7]->setConcealed(true);
	}
	
	// NH: SCENE_GHOSTTOWN -> Taz level.  YES THIS IS RIGHT!!!!!
	if ((gameStatus.globalSettings.saveInfo.saveSlot[0].level[SCENE_GHOSTTOWN].pickupsCollected >= 100) || (gameStatus.globalSettings.saveInfo.saveSlot[1].level[SCENE_GHOSTTOWN].pickupsCollected >= 100) || (gameStatus.globalSettings.saveInfo.saveSlot[2].level[SCENE_GHOSTTOWN].pickupsCollected >= 100) || (CheatActive(CHEAT_OPEN_BONUS_GAMES))) 
	{
		ptr->levelTb[8]->setConcealed(false);
		noneAvailable = false;
	}
	else
	{
		ptr->levelTb[8]->setConcealed(true);
	}

	if (noneAvailable)
		pagePtr->selectNone();
	else if (pagePtr->selectedItem == NULL)
		pagePtr->selectItem(pagePtr->getFirstSelectableItem(true));


	if (ptr->page->selectedItem == ptr->levelTb[0])
	{
		if(OKBUTTON)
		{
			gameStatus.multiplayer.numPlayers = 1;
			bkGenerateEvent("scenechange","_rcsafari");
		}

		ptr->infoSubBook->gotoPage("SELECT");
	}
	else if (ptr->page->selectedItem == ptr->levelTb[1])
	{
		if(OKBUTTON)
		{
			gameStatus.multiplayer.numPlayers = 1;
			bkGenerateEvent("scenechange","_rcicedome");
		}

		ptr->infoSubBook->gotoPage("SELECT");
	}
	else if (ptr->page->selectedItem == ptr->levelTb[2])
	{
		if(OKBUTTON)
		{
			gameStatus.multiplayer.numPlayers = 1;
			bkGenerateEvent("scenechange","_deaqua");
		}

		ptr->infoSubBook->gotoPage("SELECT");
	}
	else if (ptr->page->selectedItem == ptr->levelTb[3])
	{
		if(OKBUTTON)
		{
			gameStatus.multiplayer.numPlayers = 1;
			bkGenerateEvent("scenechange","_vrdeptstr");
		}

		ptr->infoSubBook->gotoPage("SELECT");
	}
	else if (ptr->page->selectedItem == ptr->levelTb[4])
	{
		if(OKBUTTON)
		{
			gameStatus.multiplayer.numPlayers = 1;
			bkGenerateEvent("scenechange","_vrmuseum");
		}

		ptr->infoSubBook->gotoPage("SELECT");
	}
	else if (ptr->page->selectedItem == ptr->levelTb[5])
	{
		if(OKBUTTON)
		{
			gameStatus.multiplayer.numPlayers = 1;
			bkGenerateEvent("scenechange","_deconstruct");
		}

		ptr->infoSubBook->gotoPage("SELECT");
	}
	else if (ptr->page->selectedItem == ptr->levelTb[6])
	{
		if(OKBUTTON)
		{
			gameStatus.multiplayer.numPlayers = 1;
			bkGenerateEvent("scenechange","_vrgrandc");
		}

		ptr->infoSubBook->gotoPage("SELECT");
	}
	else if (ptr->page->selectedItem == ptr->levelTb[7])
	{
		if(OKBUTTON)
		{
			gameStatus.multiplayer.numPlayers = 1;
			bkGenerateEvent("scenechange","_rcgoldmine");
		}

		ptr->infoSubBook->gotoPage("SELECT");
	}
	else if (ptr->page->selectedItem == ptr->levelTb[8])
	{
		if(OKBUTTON)
		{
			gameStatus.multiplayer.numPlayers = 1;
			bkGenerateEvent("scenechange","_detasmania");
		}

		ptr->infoSubBook->gotoPage("SELECT");
	}

	//triangle to reset back to main menu
	if(CANCELBUTTON)
	{
		// NH: Play bowhomb sound. Would be nice to be in PageFX, but not sure how to do it so this will work for now
		menu->secretsPage->page->playNavSound(menu->secretsPage->page->menuSounds, EMSFX_BACK);
		SetFrontEndCameraDestination(FECAM_EXTRAS_SECRETS,FECAM_EXTRAS);
		menu->frontEndMenuBook->gotoPage(menuNames[MENU_EXTRAS]);
		menu->currentPage = MENU_EXTRAS;
	}
}
