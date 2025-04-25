// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : startgamemenu.cpp
//   Purpose : functions to create and control the start game menu
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "TextFX.h"
#include "control.h"
#include "animation.h"
#include "memcard.h"
#include "frontendmain.h"
#include "frontendmenu.h"
#include "events.h"
#include "playerstats.h"
#include "icon.h"
#include "highscores.h"
#include "util.h"
#include "Cheats.h"				// PP: Cheats



uint32 oldSlot = 0;
char startGame;
uint32 moveTo = 0;

typedef struct TAG_SLOTDETAILS
{
	ICON	*icon;
	TEXTBOX *name;
	TEXTBOX *bounty;
	TEXTBOX *percent;
	TEXTBOX *creation;
	TEXTBOX *hub;
	TEXTBOX *diff;
} SLOTDETAILS;

SLOTDETAILS	slotInfo[3];

EStartGamePopups currentStartGamePopup;

/*  --------------------------------------------------------------------------------
	Function	: FillInSlotDetails
	Purpose		: Use info from the save data to create the slot pag
	Parameters	: 
	Returns		: 
	Info		: 
*/

void FillInSlotDetails(int slot)
{
	char	tempString[16];

	if (gameStatus.globalSettings.saveInfo.saveSlot[slot].lastIcon != -1)
		slotInfo[slot].icon->setTexture(highScoreIconNames[gameStatus.globalSettings.saveInfo.saveSlot[slot].lastIcon]);

	slotInfo[slot].name->sprintf8("%c %c %c",gameStatus.globalSettings.saveInfo.saveSlot[slot].lastName[0],gameStatus.globalSettings.saveInfo.saveSlot[slot].lastName[1],gameStatus.globalSettings.saveInfo.saveSlot[slot].lastName[2]);
	
	utilFormatPrettyNumbers(gameStatus.globalSettings.saveInfo.saveSlot[slot].bounty, tempString);
	slotInfo[slot].bounty->sprintf8("$%s", tempString);

	slotInfo[slot].percent->sprintf8("%d%%", calculatePercentageComplete(slot));

	PrintTimeAndDateIntoTextbox(slotInfo[slot].creation, &gameStatus.globalSettings.saveInfo.saveSlot[slot].creation);
}

/*	--------------------------------------------------------------------------------
	Function 	: CreateStartGameMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void CreateStartGameMenuPage(FRONTENDMENU *menu)
{
	STARTGAMEPAGE	*ptr;
	PAGE			*pagePtr, *subPage, *subSubPage, *subBookPagePtr;
	TEXTBOX			*dummyBox;
	char			tempString[16];
	ushort			diff[20];

	ptr = menu->startGamePage = (STARTGAMEPAGE*)ZALLOC(sizeof(STARTGAMEPAGE));
	if(!ptr)
	{
		bkPrintf("*** WARNING ***, could not malloc start game page/n");
		return;
	}
	// NH: Add Page (Temp? Blank as closing frontend book when doing icon selection and reopening if stepping back seems to take ages!)
	// PP: Alignment pauses shouldn't be a problem now; you might want to try it again.
	ptr->blankPage = menu->frontEndMenuBook->addPage(menuNames[MENU_BLANK]);
	
	// NH: Add Page
	pagePtr = ptr->page = menu->frontEndMenuBook->addPage(menuNames[MENU_STARTGAME]);
	pagePtr->setNavigation(true);

	// NH: Add Title
	pagePtr->insertItem(ptr->titleTb = new TEXTBOX(STR_START_GAME),false);
	ptr->titleTb->setStyle(TS_title)
		->setYAlign(PIFLAG_YALIGN_TOP)
		->setSelectable(false);
	
	// NH: Add Slot Book (Slots)
	pagePtr->insertItem(ptr->slotBook=new BOOK("SLOT BOOK"));
	ptr->slotBook->setSelectable(true);

	// NH: Add Slot Page (Slot Page)
	subPage=ptr->slotBook->addPage("SLOTS");

	// NH: Add Menu Item (Slot 1)
	subPage->insertItem(ptr->slotOneTb = new TEXTBOX(STR_SG_SLOT_ONE),true);
	ptr->slotOneTb->setAlignScale(1.4f)
		->select(false);
	
	// NH: Add Menu Item (Slot 2)
	subPage->insertItem(ptr->slotTwoTb = new TEXTBOX(STR_SG_SLOT_TWO),false);
	ptr->slotTwoTb->setAlignScale(1.4f)
		->select(false);
	
	// NH: Add Menu Item (Slot 3)
	subPage->insertItem(ptr->slotThreeTb = new TEXTBOX(STR_SG_SLOT_THREE),false);
	ptr->slotThreeTb->setAlignScale(1.4f)
		->select(false);



	// NH: Add Slot Details Book (Slot Details - Save Time/Date)
	pagePtr->insertItem(ptr->detailsSubBook=new BOOK("SLOT DETAILS SUB BOOK"));
	ptr->detailsSubBook->setYPadding(false)
		->setSelectable(false);

	// NH: Add Info (Empty Slot Details Page)
	subPage=ptr->detailsSubBook->addPage("EMPTY_SLOT_DETAILS");
	subPage->setNavigation(false)
		->setYPadding(false);
	
	subPage->insertItem(ptr->emptySlotDetailsTb = new TEXTBOX(),true);
	ptr->emptySlotDetailsTb->setText(STR_SG_EMPTY_SLOT);
	ptr->emptySlotDetailsTb->setAlignScale(1.2f)
		->setYPadding(true)
		->setSelectable(false);

	// PP: on xbox, gotter add a 'Damaged game' message for if the saves are corrupted
	// PP: (can't say 'Empty game' cos then MS think we've deleted the corrupted saves: TCR 3-53)
#ifdef XBOX_DAMAGED_SAVE_HANDLING// PP: MemCard.h

	// PP: Add Info (Damaged Game Details Page)...

	ptr->detailsSubBook->addPage("DAMAGED_GAME_DETAILS")
		->setNavigation(false)
		->insertItem(STR_XBOX_TCR_MU_CORRUPTEDSAVE);
		// PP: ->setYPadding(false);
		//->setAlignScale(1.2f)
		//->setYPadding(true)
		//->setSelectable(false);

#endif// PP: def XBOX_DAMAGED_SAVE_HANDLING

	// NH: Add Info (Slot 1 Details Page)
	subPage=ptr->detailsSubBook->addPage("SLOT_1_DETAILS");
	subPage->setNavigation(false)
		->setYPadding(false)
		->setSelectable(false);
	
	// NH: Icon
	if (gameStatus.globalSettings.saveInfo.saveSlot[0].lastIcon != -1)
		subPage->insertItem(slotInfo[0].icon = ptr->slot1Icon = new ICON(highScoreIconNames[gameStatus.globalSettings.saveInfo.saveSlot[0].lastIcon]), true);
	else
		subPage->insertItem(slotInfo[0].icon = ptr->slot1Icon = new ICON(), true);
	ptr->slot1Icon->setDrawScale(0.5)
		->setXAlign(PIFLAG_XALIGN_RIGHT)
		->setSelectable(false);

	// NH: Name
	subPage->insertItem(slotInfo[0].name = ptr->slot1NameTb = new TEXTBOX(),false);
	ptr->slot1NameTb->sprintf8("%c %c %c",gameStatus.globalSettings.saveInfo.saveSlot[0].lastName[0],gameStatus.globalSettings.saveInfo.saveSlot[0].lastName[1],gameStatus.globalSettings.saveInfo.saveSlot[0].lastName[2]);
	ptr->slot1NameTb->setAlignScale(1.2f)
		->setXAlign(PIFLAG_XALIGN_LEFT)
		->setSelectable(false)
		->setYPadding(false);

	// PP: Specify uniform X spacing for the last row we added to the page
	subPage->getLastRow()->flags |= ROWFLAG_UNIFORM_X_SPACING;
	
	// NH: Hub
	
	// NH: Must set save slot for AllowedToEnterLevel to work
	gameStatus.globalSettings.currentSaveSlot = 0;
	
	if (AllowedToEnterLevel(SCENE_TAZHUB))
		subPage->insertItem(slotInfo[0].hub = ptr->slot1HubTb = new TEXTBOX(STR_LEVELNAME_TASMANIA),true);	
	else if (AllowedToEnterLevel(SCENE_WILDWESTHUB))
		subPage->insertItem(slotInfo[0].hub = ptr->slot1HubTb = new TEXTBOX(STR_LEVELNAME_WESTWORLD),true);
	else if (AllowedToEnterLevel(SCENE_CITYHUB))
		subPage->insertItem(slotInfo[0].hub = ptr->slot1HubTb = new TEXTBOX(STR_LEVELNAME_CITYWORLD),true);
	else
		subPage->insertItem(slotInfo[0].hub = ptr->slot1HubTb = new TEXTBOX(STR_LEVELNAME_ZOOWORLD),true);
	
	ptr->slot1HubTb->setAlignScale(1.2f)
		->setXAlign(PIFLAG_XALIGN_RIGHT)
		->setSelectable(false)
		->setYPadding(false);

	// NH: Difficulty
	if (gameStatus.globalSettings.saveInfo.saveSlot[0].difficulty == 0)
		subPage->insertItem(slotInfo[0].diff = ptr->slot1DiffTb = new TEXTBOX(STR_SG_DIFF_EASY),false);	
	else if (gameStatus.globalSettings.saveInfo.saveSlot[0].difficulty == 1)
		subPage->insertItem(slotInfo[0].diff = ptr->slot1DiffTb = new TEXTBOX(STR_SG_DIFF_MEDIUM),false);	
	else
		subPage->insertItem(slotInfo[0].diff = ptr->slot1DiffTb = new TEXTBOX(STR_SG_DIFF_HARD),false);	
	
	ptr->slot1DiffTb->setAlignScale(1.2f)
		->setXAlign(PIFLAG_XALIGN_LEFT)
		->setSelectable(false)
		->setYPadding(false);

	
	// NH: Bounty (Mmmm.... Coconut.  Yummy)
	subPage->insertItem(slotInfo[0].bounty = ptr->slot1BountyTb = new TEXTBOX(),true);
	utilFormatPrettyNumbers(gameStatus.globalSettings.saveInfo.saveSlot[0].bounty, tempString);
	ptr->slot1BountyTb->sprintf8("$%s", tempString);
	ptr->slot1BountyTb->setAlignScale(1.2f)
		->setSelectable(false)
		->setYPadding(false);

	// NH: Percentage Complete
	subPage->insertItem(slotInfo[0].percent = ptr->slot1PercentageTb = new TEXTBOX(),true);
	ptr->slot1PercentageTb->sprintf8("%d%%", calculatePercentageComplete(0));
	ptr->slot1PercentageTb->setAlignScale(1.2f)
		->setSelectable(false)
		->setYPadding(false);

	// NH: Details
	subPage->insertItem(slotInfo[0].creation = ptr->slot1DetailsTb = new TEXTBOX(),true);
	PrintTimeAndDateIntoTextbox(ptr->slot1DetailsTb, &gameStatus.globalSettings.saveInfo.saveSlot[0].creation);
	ptr->slot1DetailsTb->setAlignScale(1.2f)
		->setSelectable(false)
		->setYPadding(false);

	
	// NH: Add Info (Slot 2 Details Page)
	subPage=ptr->detailsSubBook->addPage("SLOT_2_DETAILS");
	subPage->setNavigation(false)
		->setYPadding(false)
		->setSelectable(false);
	
	// NH: Icon
	if (gameStatus.globalSettings.saveInfo.saveSlot[1].lastIcon != -1)
		subPage->insertItem(slotInfo[1].icon = ptr->slot2Icon = new ICON(highScoreIconNames[gameStatus.globalSettings.saveInfo.saveSlot[1].lastIcon]), true);
	else
		subPage->insertItem(slotInfo[1].icon = ptr->slot2Icon = new ICON(), true);
	ptr->slot2Icon->setDrawScale(0.5)
		->setXAlign(PIFLAG_XALIGN_RIGHT)
		->setSelectable(false);

	// NH: Name
	subPage->insertItem(slotInfo[1].name = ptr->slot2NameTb = new TEXTBOX(),false);
	ptr->slot2NameTb->sprintf8("%c %c %c",gameStatus.globalSettings.saveInfo.saveSlot[1].lastName[0],gameStatus.globalSettings.saveInfo.saveSlot[1].lastName[1],gameStatus.globalSettings.saveInfo.saveSlot[1].lastName[2]);
	ptr->slot2NameTb->setAlignScale(1.2f)
		->setXAlign(PIFLAG_XALIGN_LEFT)
		->setSelectable(false)
		->setYPadding(false);

	// PP: Specify uniform X spacing for the last row we added to the page
	subPage->getLastRow()->flags |= ROWFLAG_UNIFORM_X_SPACING;
	
	// NH: Hub
	// NH: Must set save slot for AllowedToEnterLevel to work
	gameStatus.globalSettings.currentSaveSlot = 1;

	if (AllowedToEnterLevel(SCENE_TAZHUB))
		subPage->insertItem(slotInfo[1].hub = ptr->slot2HubTb = new TEXTBOX(STR_LEVELNAME_TASMANIA),true);	
	else if (AllowedToEnterLevel(SCENE_WILDWESTHUB))
		subPage->insertItem(slotInfo[1].hub = ptr->slot2HubTb = new TEXTBOX(STR_LEVELNAME_WESTWORLD),true);
	else if (AllowedToEnterLevel(SCENE_CITYHUB))
		subPage->insertItem(slotInfo[1].hub = ptr->slot2HubTb = new TEXTBOX(STR_LEVELNAME_CITYWORLD),true);
	else
		subPage->insertItem(slotInfo[1].hub = ptr->slot2HubTb = new TEXTBOX(STR_LEVELNAME_ZOOWORLD),true);

	ptr->slot2HubTb->setAlignScale(1.2f)
		->setXAlign(PIFLAG_XALIGN_RIGHT)
		->setSelectable(false)
		->setYPadding(false);

	// NH: Difficulty
	if (gameStatus.globalSettings.saveInfo.saveSlot[1].difficulty == 0)
		subPage->insertItem(slotInfo[1].diff = ptr->slot2DiffTb = new TEXTBOX(STR_SG_DIFF_EASY),false);	
	else if (gameStatus.globalSettings.saveInfo.saveSlot[1].difficulty == 1)
		subPage->insertItem(slotInfo[1].diff = ptr->slot2DiffTb = new TEXTBOX(STR_SG_DIFF_MEDIUM),false);	
	else
		subPage->insertItem(slotInfo[1].diff = ptr->slot2DiffTb = new TEXTBOX(STR_SG_DIFF_HARD),false);	
	
	ptr->slot2DiffTb->setAlignScale(1.2f)
		->setXAlign(PIFLAG_XALIGN_LEFT)
		->setSelectable(false)
		->setYPadding(false);

	
	// NH: Bounty (Two chocolate and coconut muncies in 1 pack!  Marvellous!)
	subPage->insertItem(slotInfo[1].bounty = ptr->slot2BountyTb = new TEXTBOX(),true);
	utilFormatPrettyNumbers(gameStatus.globalSettings.saveInfo.saveSlot[1].bounty, tempString);
	ptr->slot2BountyTb->sprintf8("$%s", tempString);
	ptr->slot2BountyTb->setAlignScale(1.2f)
		->setSelectable(false)
		->setYPadding(false);

	// NH: Percentage Complete
	subPage->insertItem(slotInfo[1].percent = ptr->slot2PercentageTb = new TEXTBOX(),true);
	ptr->slot2PercentageTb->sprintf8("%d%%", calculatePercentageComplete(1));
	ptr->slot2PercentageTb->setAlignScale(1.2f)
		->setSelectable(false)
		->setYPadding(false);

	// NH: Details
	subPage->insertItem(slotInfo[1].creation = ptr->slot2DetailsTb = new TEXTBOX(),true);
	PrintTimeAndDateIntoTextbox(ptr->slot2DetailsTb, &gameStatus.globalSettings.saveInfo.saveSlot[1].creation);
	ptr->slot2DetailsTb->setAlignScale(1.2f)
		->setSelectable(false)
		->setYPadding(false);

	
	
	
	// NH: Add Info (Slot 3 Details Page)
	subPage=ptr->detailsSubBook->addPage("SLOT_3_DETAILS");
	subPage->setNavigation(false)
		->setYPadding(false);
	
	// NH: Icon
	if (gameStatus.globalSettings.saveInfo.saveSlot[2].lastIcon != -1)
		subPage->insertItem(slotInfo[2].icon = ptr->slot3Icon = new ICON(highScoreIconNames[gameStatus.globalSettings.saveInfo.saveSlot[2].lastIcon]), true);
	else
		subPage->insertItem(slotInfo[2].icon = ptr->slot3Icon = new ICON(), true);
	ptr->slot3Icon->setDrawScale(0.5)
		->setXAlign(PIFLAG_XALIGN_RIGHT)
		->setSelectable(false);

	// NH: Name
	subPage->insertItem(slotInfo[2].name = ptr->slot3NameTb = new TEXTBOX(),false);
	ptr->slot3NameTb->sprintf8("%c %c %c",gameStatus.globalSettings.saveInfo.saveSlot[2].lastName[0],gameStatus.globalSettings.saveInfo.saveSlot[2].lastName[1],gameStatus.globalSettings.saveInfo.saveSlot[2].lastName[2]);
	ptr->slot3NameTb->setAlignScale(1.2f)
		->setXAlign(PIFLAG_XALIGN_LEFT)
		->setSelectable(false)
		->setYPadding(false);

	// PP: Specify uniform X spacing for the last row we added to the page
	subPage->getLastRow()->flags |= ROWFLAG_UNIFORM_X_SPACING;
	
	// NH: Hub
	// NH: Must set save slot for AllowedToEnterLevel to work
	gameStatus.globalSettings.currentSaveSlot = 2;
	
	if (AllowedToEnterLevel(SCENE_TAZHUB))
		subPage->insertItem(slotInfo[2].hub = ptr->slot3HubTb = new TEXTBOX(STR_LEVELNAME_TASMANIA),true);	
	else if (AllowedToEnterLevel(SCENE_WILDWESTHUB))
		subPage->insertItem(slotInfo[2].hub = ptr->slot3HubTb = new TEXTBOX(STR_LEVELNAME_WESTWORLD),true);
	else if (AllowedToEnterLevel(SCENE_CITYHUB))
		subPage->insertItem(slotInfo[2].hub = ptr->slot3HubTb = new TEXTBOX(STR_LEVELNAME_CITYWORLD),true);
	else
		subPage->insertItem(slotInfo[2].hub = ptr->slot3HubTb = new TEXTBOX(STR_LEVELNAME_ZOOWORLD),true);

	ptr->slot3HubTb->setAlignScale(1.2f)
		->setXAlign(PIFLAG_XALIGN_RIGHT)
		->setSelectable(false)
		->setYPadding(false);

	// NH: Difficulty
	if (gameStatus.globalSettings.saveInfo.saveSlot[2].difficulty == 0)
		subPage->insertItem(slotInfo[2].diff = ptr->slot3DiffTb = new TEXTBOX(STR_SG_DIFF_EASY),false);	
	else if (gameStatus.globalSettings.saveInfo.saveSlot[2].difficulty == 1)
		subPage->insertItem(slotInfo[2].diff = ptr->slot3DiffTb = new TEXTBOX(STR_SG_DIFF_MEDIUM),false);	
	else
		subPage->insertItem(slotInfo[2].diff = ptr->slot3DiffTb = new TEXTBOX(STR_SG_DIFF_HARD),false);	
	
	ptr->slot3DiffTb->setAlignScale(1.2f)
		->setXAlign(PIFLAG_XALIGN_LEFT)
		->setSelectable(false)
		->setYPadding(false);

	// NH: Bounty (I couldn't.  I'm getting fat enough as it is!)
	subPage->insertItem(slotInfo[2].bounty = ptr->slot3BountyTb = new TEXTBOX(),true);
	utilFormatPrettyNumbers(gameStatus.globalSettings.saveInfo.saveSlot[2].bounty, tempString);
	ptr->slot3BountyTb->sprintf8("$%s", tempString);
	ptr->slot3BountyTb->setAlignScale(1.2f)
		->setSelectable(false)
		->setYPadding(false);
	
	// NH: Percentage Complete
	subPage->insertItem(slotInfo[2].percent = ptr->slot3PercentageTb = new TEXTBOX(),true);
	ptr->slot3PercentageTb->sprintf8("%d%%", calculatePercentageComplete(2));
	ptr->slot3PercentageTb->setAlignScale(1.2f)
		->setSelectable(false)
		->setYPadding(false);

	// NH: Details
	subPage->insertItem(slotInfo[2].creation = ptr->slot3DetailsTb = new TEXTBOX(),true);
	PrintTimeAndDateIntoTextbox(ptr->slot3DetailsTb, &gameStatus.globalSettings.saveInfo.saveSlot[2].creation);
	ptr->slot3DetailsTb->setAlignScale(1.2f)
		->setSelectable(false)
		->setYPadding(false);

	// NH: Add Options (Slot handling options)
	pagePtr->insertItem(ptr->slotSubBook=new BOOK("SLOT SUB BOOK"));
	ptr->slotSubBook->setSelectable(true);

	// NH: Null page, for when no options are available (No navigation)
	subPage=ptr->slotSubBook->addPage("NULL");
	subPage->setNavigation(false)
		->setYPadding(false);
	
	// NH: Slot Control page (Show slot options - selectable)
	subPage=ptr->slotSubBook->addPage("SLOT CONTROL PAGE");
	subPage->setNavigation(true)
		->setYPadding(false);
	
	// NH: Add Option (Load Slot)
	subPage->insertItem(ptr->chooseLoadTb = new TEXTBOX(STR_SG_LOAD), true);
	ptr->chooseLoadTb->setYPadding(false);
		//->setSelectable(false);
		
	// NH: Add Option (Delete Slot)
	subPage->insertItem(ptr->chooseDeleteTb = new TEXTBOX(STR_SG_DELETE), true);
	ptr->chooseDeleteTb->setAlignScale(1.2f)
		->setYPadding(false);
		//->setSelectable(false);

	subPage->selectItem(ptr->chooseLoadTb);
	ptr->slotSubBook->open();	// PP: books default to closed
	
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
	ADD_INFOSUBBOOK_PAGE(SG_CHOOSE_SLOT,STR_SG_CHOOSE_SLOT);
	ADD_INFOSUBBOOK_PAGE(SG_NEW,STR_SG_NEW);
	ADD_INFOSUBBOOK_PAGE(SG_RU_SURE,STR_SG_RU_SURE);

	// NH: Add Null Info
	subPage=ptr->infoSubBook->addPage("NULL");
	subPage->insertItem(dummyBox = new TEXTBOX(), true)
		->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setSelectable(false);
	dummyBox->setText("");
	dummyBox->setStyle(TS_info);

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
	ADD_CONTROLSUBBOOK_PAGE(X_TO_SELECT_Y_TO_RETURN,STR_X_TO_SELECT_Y_TO_RETURN);
	ADD_CONTROLSUBBOOK_PAGE(CONTINUE_OR_CANCEL,STR_CONTINUE_OR_CANCEL);

	ptr->infoSubBook->open();// PP: books default to closed
	ptr->controlSubBook->open();
	
	ptr->slotBook->open();

#ifdef LOAD_AFTER_START_SCREEN// PP: MemCard.h

	// PP: Ah yeah, since this page now gets added while the frontend book is open,
	// PP: we've gotter realign the book at this point.  (Books normally tend to get aligned automatically when they open)
	// PP: We should normally try to avoid adding pages to a book while it's open, but there's no good alternative here.
	menu->frontEndMenuBook->realign();

#endif// PP: LOAD_AFTER_START_SCREEN

	// TP: Set the currentSaveSlot to the dummy save slot 3 (4th slot used for demo mode)
	// TP: this stops the bonus games from changing slot 2 - the currentSaveSlot setting left over from the above code
	gameStatus.globalSettings.currentSaveSlot = 3;
	InitialiseSaveGameSlot(3, DIFF_MEDIUM);
}

enum
{
	LOADSTATE_NONE,
	LOADSTATE_LOADING,
	LOADSTATE_LOADED,
};



void returnToSlotSelection(STARTGAMEPAGE *ptr)
{
	ptr->subState = SM_SLOTS;

	ptr->slotOneTb->highlight(false);
	ptr->slotTwoTb->highlight(false);
	ptr->slotThreeTb->highlight(false);

	// PP: on xbox, if the saves are corrupted, we'll ALWAYS show 'Damaged save' for each slot
#ifdef XBOX_DAMAGED_SAVE_HANDLING// PP: MemCard.h
	if(MemCard_corruptedSave)
	{
		ptr->detailsSubBook->gotoPage("DAMAGED_SAVE_DETAILS");
	}
	else
#endif// PP: def XBOX_DAMAGED_SAVE_HANDLING// PP: MemCard.h
	{
		ptr->detailsSubBook->gotoPage("EMPTY_SLOT_DETAILS");
	}

	ptr->infoSubBook->gotoPage("SG_CHOOSE_SLOT");
	ptr->controlSubBook->gotoPage("X_TO_SELECT_Y_TO_RETURN");

	ptr->slotBook->setSelectable(true);	
					
	switch(ptr->slotNum)
	{
	case 0:
		ptr->subState = SM_SLOTONE;
		//ptr->slotBook->getPage("SLOTS")->selectItem(ptr->slotOneTb);
		ptr->page->selectItem(ptr->slotOneTb);
		break;
	case 1:
		ptr->subState = SM_SLOTTWO;
		//ptr->slotBook->getPage("SLOTS")->selectItem(ptr->slotTwoTb);
		ptr->page->selectItem(ptr->slotTwoTb);
		break;
	case 2:
		ptr->subState = SM_SLOTTHREE;
		//ptr->slotBook->getPage("SLOTS")->selectItem(ptr->slotThreeTb);
		ptr->page->selectItem(ptr->slotThreeTb);
	}

	frontEndMenu.frontEndMenuBook->getPage(menuNames[MENU_STARTGAME])->setNavigation(true);
	frontEndMenu.startGamePage->infoSubBook->open();
	frontEndMenu.startGamePage->controlSubBook->open();
}

void clearGame(STARTGAMEPAGE *ptr)
{
	InitialiseSaveGameSlot(ptr->slotNum, gameStatus.globalSettings.difficulty);			
	gameStatus.globalSettings.saveInfo.saveSlot[ptr->slotNum].inUse = FALSE;		// CMD: save game effectively deleted
	gameStatus.globalSettings.saveInfo.saveSlot[ptr->slotNum].lastIcon = -1;
	gameStatus.globalSettings.saveInfo.saveSlot[ptr->slotNum].lastName[0] = 0;
	gameStatus.globalSettings.saveInfo.saveSlot[ptr->slotNum].lastName[1] = 0;
	gameStatus.globalSettings.saveInfo.saveSlot[ptr->slotNum].lastName[2] = 0;					
}

void openNameAndIconEntry(STARTGAMEPAGE *ptr)
{
	ptr->subState = SM_GETNAME;
	
	gameStatus.globalSettings.currentSaveSlot = ptr->slotNum;
	frontEndMenu.frontEndMenuBook->gotoPage(menuNames[MENU_BLANK]);
							
	CreateStatsAtEndOfLevel(gameStatus.player[0].stats, 0, 1);
	gameStatus.outroState = OUTRO_NAMEENTRY;
	gameStatus.player[0].stats->nameEntryStage = HSE_CHOOSEICON;
	CreateNameEntryPageAtEndOfLevel(gameStatus.player[0].stats);

	gameStatus.player[0].stats->endOfLevelStatsBook->open("SelectIcon");
	
//	gameStatus.player[0].stats->endOfLevelStatsBook->gotoPage("SelectIcon");	// PP: true: instantly, so's we don't see the transition
}

void moveTazBetweenSlots(STARTGAMEPAGE *ptr)
{
	// NH: If not still bringing Taz into screen
	if(!CheckIfPlayingAnimation(frontEndAnimInfo.paths[MENU_STARTGAME][FEPATH_THERE],"movetaz"))
	{
		if ((!CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_ONE_TO_TWO],"movetaz"))
			&& (!CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_TWO_TO_THREE],"movetaz"))
			&& (!CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_THREE_TO_TWO],"movetaz"))
			&& (!CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_TWO_TO_ONE],"movetaz"))		
			&& (!CheckIfPlayingAnimation(frontEndAnimInfo.paths[MENU_STARTGAME][FEPATH_THERE],"movetaz"))
			&& (!CheckIfPlayingAnimation(frontEndAnimInfo.paths[MENU_SLOTONE][FEPATH_THERE],"movetaz"))
			&& (!CheckIfPlayingAnimation(frontEndAnimInfo.paths[MENU_SLOTTWO][FEPATH_THERE],"movetaz"))
			&& (!CheckIfPlayingAnimation(frontEndAnimInfo.paths[MENU_SLOTTHREE][FEPATH_THERE],"movetaz")))
		{// NH: The following 3 blocks of code allow Taz to move between a slot to any of the others (even if it means stepping through the middle slot)
		// NH: Move allong specified path
		switch (ptr->pathToFollow)
		{
		case MENU_SLOTONE:
			ChooseFrontEndAnimationPath(MENU_SLOTONE);
			bkPrintf("Anim finished so.. Move To slot 1 - %d, %d\n", frontEndAnimInfo.lastMenu, frontEndAnimInfo.targetMenu);
			ptr->pathToFollow = -1;
			break;
		case MENU_SLOTTWO:
			ChooseFrontEndAnimationPath(MENU_SLOTTWO);
			bkPrintf("Anim finished so.. Move To slot 2 - %d, %d\n", frontEndAnimInfo.lastMenu, frontEndAnimInfo.targetMenu);
			ptr->pathToFollow = -1;
			break;
		case MENU_SLOTTHREE:
			ChooseFrontEndAnimationPath(MENU_SLOTTHREE);
			bkPrintf("Anim finished so.. Move To slot 3 - %d, %d\n", frontEndAnimInfo.lastMenu, frontEndAnimInfo.targetMenu);
			ptr->pathToFollow = -1;
			break;
		}		
		}

		// NH: When a slot is selected, set the slot that Taz should move to, grab the time/date from that slot and display the slots details
		if (ptr->page->selectedItem == ptr->slotOneTb)
		{
			ptr->slotNum = 0;		

			// PP: on xbox, if the saves are corrupted, we'll ALWAYS show 'Damaged save' for each slot
#ifdef XBOX_DAMAGED_SAVE_HANDLING// PP: MemCard.h
			if(MemCard_corruptedSave)
			{
				ptr->detailsSubBook->gotoPage("DAMAGED_GAME_DETAILS");
			}
			else
#endif// PP: def XBOX_DAMAGED_SAVE_HANDLING// PP: MemCard.h
			{
				// PP: on xbox, if the disk is full, we'll always show 'Empty game' for each slot
#ifdef XBOX_FULL_MEDIUM_HANDLING// PP: MemCard.h
				if(MemCard_fullMedium)
				{
					ptr->detailsSubBook->gotoPage("EMPTY_SLOT_DETAILS");
				}
				else
#endif// PP: def XBOX_FULL_MEDIUM_HANDLING
				{
					if (gameStatus.globalSettings.saveInfo.saveSlot[ptr->slotNum].inUse != FALSE)
					{ 	
						ptr->detailsSubBook->gotoPage("SLOT_1_DETAILS");				
					}
					else
					{	
						ptr->detailsSubBook->gotoPage("EMPTY_SLOT_DETAILS");
					}
				}
			}
			
			ptr->detailsSubBook->open();
		}
		else if (ptr->page->selectedItem == ptr->slotTwoTb)
		{
			ptr->slotNum = 1;
			
			// PP: on xbox, if the saves are corrupted, we'll ALWAYS show 'Damaged save' for each slot
#ifdef XBOX_DAMAGED_SAVE_HANDLING// PP: MemCard.h
			if(MemCard_corruptedSave)
			{
				ptr->detailsSubBook->gotoPage("DAMAGED_GAME_DETAILS");
			}
			else
#endif// PP: def XBOX_DAMAGED_SAVE_HANDLING// PP: MemCard.h
			{
				// PP: on xbox, if the disk is full, we'll always show 'Empty game' for each slot
#ifdef XBOX_FULL_MEDIUM_HANDLING// PP: MemCard.h
				if(MemCard_fullMedium)
				{
					ptr->detailsSubBook->gotoPage("EMPTY_SLOT_DETAILS");
				}
				else
#endif// PP: def XBOX_FULL_MEDIUM_HANDLING
				{
					if (gameStatus.globalSettings.saveInfo.saveSlot[ptr->slotNum].inUse != FALSE)
					{ 	
						ptr->detailsSubBook->gotoPage("SLOT_2_DETAILS");				
					}
					else
					{	
						ptr->detailsSubBook->gotoPage("EMPTY_SLOT_DETAILS");
					}
				}
			}
			
			ptr->detailsSubBook->open();
		}
		else if (ptr->page->selectedItem == ptr->slotThreeTb)
		{
			ptr->slotNum = 2;

			// PP: on xbox, if the saves are corrupted, we'll ALWAYS show 'Damaged save' for each slot
#ifdef XBOX_DAMAGED_SAVE_HANDLING// PP: MemCard.h
			if(MemCard_corruptedSave)
			{
				ptr->detailsSubBook->gotoPage("DAMAGED_GAME_DETAILS");
			}
			else
#endif// PP: def XBOX_DAMAGED_SAVE_HANDLING// PP: MemCard.h
			{
				// PP: on xbox, if the disk is full, we'll always show 'Empty game' for each slot
#ifdef XBOX_FULL_MEDIUM_HANDLING// PP: MemCard.h
				if(MemCard_fullMedium)
				{
					ptr->detailsSubBook->gotoPage("EMPTY_SLOT_DETAILS");
				}
				else
#endif// PP: def XBOX_FULL_MEDIUM_HANDLING
				{
					if (gameStatus.globalSettings.saveInfo.saveSlot[ptr->slotNum].inUse != FALSE)
					{ 	
						ptr->detailsSubBook->gotoPage("SLOT_3_DETAILS");				
					}
					else
					{	
						ptr->detailsSubBook->gotoPage("EMPTY_SLOT_DETAILS");
					}
				}
			}
			
			ptr->detailsSubBook->open();
		}
		
		// NH: CALCULATE SLOT TO MOVE TO
		// NH: If slot is 0...
		if (ptr->currentSlot == 0)	// NH: On screen shown as Slot 1 but stored as 0
		{
			// NH: ...and selected slot is 1 or 2, and all anims are finished move taz to slot 1
			if (ptr->currentSlot < ptr->slotNum)
			{
				if(!CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_ONE_TO_TWO],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_TWO_TO_THREE],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_THREE_TO_TWO],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_TWO_TO_ONE],"movetaz")				
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.paths[MENU_STARTGAME][FEPATH_THERE],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.paths[MENU_SLOTONE][FEPATH_THERE],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.paths[MENU_SLOTTWO][FEPATH_THERE],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.paths[MENU_SLOTTHREE][FEPATH_THERE],"movetaz"))
				{
					ptr->pathToFollow = MENU_SLOTTWO;
					ptr->currentSlot++;
				}
			}		
		}
		// NH: If slot is 1...
		else if (ptr->currentSlot == 1)	// NH: On screen shown as Slot 2 but stored as 1
		{
			// NH: ...and selected slot is 2, and all anims are finished move taz to slot 2
			if (ptr->currentSlot < ptr->slotNum)
			{
				if(!CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_ONE_TO_TWO],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_TWO_TO_THREE],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_THREE_TO_TWO],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_TWO_TO_ONE],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.paths[MENU_STARTGAME][FEPATH_THERE],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.paths[MENU_SLOTONE][FEPATH_THERE],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.paths[MENU_SLOTTWO][FEPATH_THERE],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.paths[MENU_SLOTTHREE][FEPATH_THERE],"movetaz"))
				{
					ptr->pathToFollow = MENU_SLOTTHREE;
					ptr->currentSlot++;
				}
			}
			// NH: (else)...and selected slot is 0, and all anims are finished move taz to slot 0
			else if (ptr->currentSlot > ptr->slotNum)
			{
				if(!CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_ONE_TO_TWO],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_TWO_TO_THREE],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_THREE_TO_TWO],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_TWO_TO_ONE],"movetaz")		
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.paths[MENU_STARTGAME][FEPATH_THERE],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.paths[MENU_SLOTONE][FEPATH_THERE],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.paths[MENU_SLOTTWO][FEPATH_THERE],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.paths[MENU_SLOTTHREE][FEPATH_THERE],"movetaz"))
				{
					ptr->pathToFollow = MENU_SLOTONE;
					ptr->currentSlot--;
				}
			}
		}
		// NH: If slot is 2...
		else if (ptr->currentSlot == 2)	// NH: On screen shown as Slot 3 but stored as 2
		{
			// NH: ...and selected slot is 0 or 1, and all anims are finished move taz to slot 1
			if (ptr->currentSlot > ptr->slotNum)
			{
				if(!CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_ONE_TO_TWO],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_TWO_TO_THREE],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_THREE_TO_TWO],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_TWO_TO_ONE],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.paths[MENU_STARTGAME][FEPATH_THERE],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.paths[MENU_SLOTONE][FEPATH_THERE],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.paths[MENU_SLOTTWO][FEPATH_THERE],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.paths[MENU_SLOTTHREE][FEPATH_THERE],"movetaz"))
				{
					ptr->pathToFollow = MENU_SLOTTWO;
					ptr->currentSlot--;
				}
			}
		}
	}
}

void startNewGame(STARTGAMEPAGE *ptr)
{
	if(!CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_ONE_TO_GAME],"movetaz")
		&& !CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_TWO_TO_GAME],"movetaz")
		&& !CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_THREE_TO_GAME],"movetaz"))
	{						
		if (ptr->subState != SM_SLOTS)
		{
			gameStatus.globalSettings.currentSaveSlot = ptr->slotNum;
			frontEndMenu.frontEndMenuBook->getPage(menuNames[MENU_STARTGAME])->setNavigation(true);		
			
			startGame = false;	
			
			// TP: Load and play start game cutscene
			// TP: bkGenerateEvent("scenechange","_zoohub");
			bkGenerateEvent("scenechange","_zoohub_startgame");// TP: play intro cutscene
			InitialiseSaveGameSlot(ptr->slotNum, gameStatus.globalSettings.difficulty);			

			// NH: Close and free book
			BookList_closeAndFree(frontEndMenu.frontEndMenuBook);
		}
		else
		{
			ptr->subState = SM_SLOTS;
			ChooseFrontEndAnimationPath(MENU_GAME);						
		}			
	}
}

void loadGame(STARTGAMEPAGE *ptr)
{
	if(!CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_ONE_TO_GAME],"movetaz")
		&& !CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_TWO_TO_GAME],"movetaz")
		&& !CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_THREE_TO_GAME],"movetaz"))
	{
		if (ptr->subState != SM_SLOTS)
		{
			gameStatus.globalSettings.difficulty = gameStatus.globalSettings.saveInfo.saveSlot[ptr->slotNum].difficulty;
			gameStatus.globalSettings.currentSaveSlot = ptr->slotNum;
			ChooseHubToStartIn(ptr->slotNum);
			frontEndMenu.frontEndMenuBook->getPage(menuNames[MENU_STARTGAME])->setNavigation(true);		
			
			startGame = false;	
			
			// NH: Close and free book
			BookList_closeAndFree(frontEndMenu.frontEndMenuBook);
		}
		else
		{
			ptr->subState = SM_SLOTS;
			ChooseFrontEndAnimationPath(MENU_GAME);						
		}
	}	
}

void handlePopups(STARTGAMEPAGE *ptr)
{
	int	retval = -1;
	// PP: get the player's response to the popup
	retval = frontEndMenu.popUpOptions->getResponse();

	if (retval != -1)
	{
		// NH: Close pop up
		BookList_closeAndFree(frontEndMenu.popUpOptions);

		// NH: NEW GAME
		if (currentStartGamePopup == START_POPUP_NEW_GAME)
		{
			switch (retval)
			{
			case 0:
			case 2:
				returnToSlotSelection(ptr);
				break;
			case 1: // NH: New Game Selected
				openNameAndIconEntry(ptr);
				break;
			}
		}
		// NH: CONTINUE / DELETE
		else if (currentStartGamePopup == START_POPUP_CONTINUE_OR_DELETE)
		{
			switch (retval)
			{
			case 0: // NH: Cancelled
				returnToSlotSelection(ptr);
				break;
			case 2: // NH: Cleared (TODO)
				ptr->subState = SM_DELETE_FINAL;
				break;
			case 1: // NH: Load Game
				startGame = TRUE;
				break;
			}
		}	
		// NH: CONFIRM DELETE
		else if (currentStartGamePopup == START_POPUP_DELETE_CONFIRM)
		{
			switch (retval)
			{
			case 0: // NH: Cancelled
				returnToSlotSelection(ptr);
				break;
			case 2: // NH: DON'T CLEAR
				//returnToSlotSelection(ptr);
				ptr->subState = SM_LOAD;
				break;
			case 1: // NH: CLEAR
				clearGame(ptr);					
				returnToSlotSelection(ptr);
				break;
			}
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: StartGameMenu_damagedSaveOKCallback
	Purpose 	: function called when the damaged-save or disk-full messages are OK'd
					(ie. player chooses to continue without saving) while in the Start Game menu
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: must match the DamagedSaveOKCallback typedef (MemCard.h)
*/
static void StartGameMenu_damagedSaveOKCallback(void)
{
	gameStatus.globalSettings.InitialiseSaveData();
	openNameAndIconEntry(frontEndMenu.startGamePage);
}


/*	--------------------------------------------------------------------------------
	Function 	: UpdateStartGameMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateStartGameMenuPage(FRONTENDMENU *menu, PADCONTROLSTATUS *controller)
{
	STARTGAMEPAGE	*ptr = menu->startGamePage;
	static	char	loadState;
	static	char	exit;

	if (ptr->subState == SM_LEAVING)
	{
		if(!CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_ONE_TO_TWO],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_TWO_TO_THREE],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_THREE_TO_TWO],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[SLOT_TWO_TO_ONE],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.paths[MENU_STARTGAME][FEPATH_THERE],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.paths[MENU_SLOTONE][FEPATH_THERE],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.paths[MENU_SLOTTWO][FEPATH_THERE],"movetaz")
					&&!CheckIfPlayingAnimation(frontEndAnimInfo.paths[MENU_SLOTTHREE][FEPATH_THERE],"movetaz"))
		//if(frontEndAnimInfo.lastMenu == frontEndAnimInfo.targetMenu)
		{
			bkPrintf("Leaving Now!!! %d, %d\n", frontEndAnimInfo.lastMenu, frontEndAnimInfo.targetMenu);
			ResetFrontEndAnimPath(frontEndAnimInfo.savePaths[SLOT_ONE_TO_TWO],"movetaz");
			ResetFrontEndAnimPath(frontEndAnimInfo.savePaths[SLOT_TWO_TO_THREE],"movetaz");
			ResetFrontEndAnimPath(frontEndAnimInfo.savePaths[SLOT_THREE_TO_TWO],"movetaz");
			ResetFrontEndAnimPath(frontEndAnimInfo.savePaths[SLOT_TWO_TO_ONE],"movetaz");
			frontEndAnimInfo.lastMenu = frontEndAnimInfo.targetMenu;
			bkPrintf("Leaving Now (Part Deux)!!! %d, %d\n", frontEndAnimInfo.lastMenu, frontEndAnimInfo.targetMenu);
			menu->currentPage = MENU_MAIN;
			ChooseFrontEndAnimationPath(MENU_MAIN);
			ptr->subState = SM_SLOTONE;
		}
		else
		{
			bkPrintf("Not quite yet!!! %d, %d\n", frontEndAnimInfo.lastMenu, frontEndAnimInfo.targetMenu);
		}
	}
	else
	{
		moveTazBetweenSlots(ptr);

		// PP: without this, you'd have to delete your game 3 in order to toggle a cheat!  a bit harsh, no?
		if((ptr->slotNum==2) && (controller1.squareDebounceChannel->value))
		{
			checkCheats(gameStatus.player[0].stats);
		}
		
		// NH: If popup open
		if (frontEndMenu.popUpOptions)
		{
			handlePopups(ptr);		
		}
		else if (!startGame)
		{
			// NH: Create POPUPS
			switch(ptr->subState)
			{
			case SM_LOAD:
				// NH: If no popup, then create one
				if (!frontEndMenu.popUpOptions)
				{
					currentStartGamePopup = START_POPUP_CONTINUE_OR_DELETE;
					menu->frontEndMenuBook->getPage(menuNames[MENU_STARTGAME])->setNavigation(false);
					ptr->infoSubBook->close();
					ptr->controlSubBook->close();

					// NH: Shouldn't really need to do this!!!
					// PP: Force-debounce ok button(s)
					FORCE_OKBUTTON_DEBOUNCE;

					frontEndMenu.popUpOptions = new POPUP(POPUP_CONFIRM, 1, STR_SG_LOAD, STR_SG_LOAD, STR_SG_DELETE, STR_X_TO_SELECT_Y_TO_RETURN);

					ptr->slotBook->setSelectable(false);	
				}
				break;
			case SM_NEW:

				// NH: If no popup, then create one
				if (!frontEndMenu.popUpOptions)
				{
					currentStartGamePopup = START_POPUP_NEW_GAME;
					menu->frontEndMenuBook->getPage(menuNames[MENU_STARTGAME])->setNavigation(false);
					ptr->infoSubBook->close();
					ptr->controlSubBook->close();

					// NH: Shouldn't really need to do this!!!
					// PP: Force-debounce ok button(s)
					FORCE_OKBUTTON_DEBOUNCE;

					frontEndMenu.popUpOptions = new POPUP(POPUP_CONFIRM, 1, STR_SG_NEW);

					ptr->slotBook->setSelectable(false);
				}
				break;
			}



			// NH: SELECT NAME AND ICON
			if (ptr->subState == SM_GETNAME)
			{
				if (gameStatus.player[0].stats->nameEntryStage != HSE_EXIT && gameStatus.player[0].stats->nameEntryStage != HSE_DROPOUT)
				{
					// PP: NAME ENTRY STUFF GETS COMPILED-OUT IN CONSUMER DEMO BUILDS
	#ifndef CONSUMERDEMO	
					UpdateNameEntryPage(gameStatus.player[0].stats);
	#endif// PP: ndef CONSUMERDEMO
				}
				else
				{
					if (gameStatus.player[0].stats->nameEntryStage != HSE_DROPOUT)
					{
						// NH: COMING BACK TO START GAME
						BookList_closeAndFree(gameStatus.player[0].stats->endOfLevelStatsBook);
						ChooseFrontEndAnimationPath(MENU_GAME);
						startGame = TRUE;
						ptr->subState = SM_NEW;
					}
					else
					{	
						// NH: COMING BACK TO STOP CREATING THAT NEW GAME
						menu->frontEndMenuBook->gotoPage(menuNames[MENU_STARTGAME]);
						returnToSlotSelection(ptr);

						// TP: Reset the current save slot to dummy 3
						gameStatus.globalSettings.currentSaveSlot = 3;
					}
				}
			}
			else if (ptr->subState == SM_DELETE_FINAL)
			{
				// NH: If no popup, then create one
				if (!frontEndMenu.popUpOptions)
				{
					currentStartGamePopup = START_POPUP_DELETE_CONFIRM;
					menu->frontEndMenuBook->getPage(menuNames[MENU_STARTGAME])->setNavigation(false);
					ptr->infoSubBook->close();
					ptr->controlSubBook->close();

					// NH: Shouldn't really need to do this!!!
					// PP: Force-debounce ok button(s)
					FORCE_OKBUTTON_DEBOUNCE;

					frontEndMenu.popUpOptions = new POPUP(POPUP_CONFIRM, 0, STR_SG_RU_SURE);

					ptr->slotBook->setSelectable(false);
				}
			}
		//	else
		//	{
		//		moveTazBetweenSlots(ptr);
		//	}


			// NH: If Selected...
			else if (OKBUTTON)
			{
				if (frontEndAnimInfo.targetMenu == MENU_SLOTONE || frontEndAnimInfo.targetMenu == MENU_SLOTTWO || frontEndAnimInfo.targetMenu == MENU_SLOTTHREE)
				{
					// NH: ELSE IF A SLOT IS SELECTED
					// NH: ...and a slot is selected...
					if ((ptr->page->selectedItem == ptr->slotOneTb) || (ptr->page->selectedItem == ptr->slotTwoTb) || (ptr->page->selectedItem == ptr->slotThreeTb))
					{							
						ptr->slotBook->setSelectable(false);

						switch(ptr->slotNum)
						{
						case 0:
							ptr->slotOneTb->highlight(true);
							break;
						case 1:
							ptr->slotTwoTb->highlight(true);
							break;
						case 2:
							ptr->slotThreeTb->highlight(true);
							break;
						}

#ifdef XBOX_DAMAGED_SAVE_HANDLING// PP: MemCard.h
						if(MemCard_corruptedSave)// PP:  MemCard_fullMedium & MemCard_corruptedSave are naturally mut-ex
						{
							// PP: NOTE: there's a bit of an oversight somewhere in the pad-checking code that means that
							// PP: if you're using a pad in the second port, the unused controller2 struct will be pointing at the second port as well.
							// PP: Debouncing just controller1's okbutton therefore isn't enough; the debounced signal will get picked up on controller2
							// PP: as it is read even in 1 player.
							// PP: This bodge hides the problem.
							FORCE_OKBUTTON_DEBOUNCE_ALL_PADS;

							MemCard_damagedSaveOKCallback=StartGameMenu_damagedSaveOKCallback;

							MemCard_book.open("DAMAGED_GAME");
						}
						else
#endif// PP: def XBOX_DAMAGED_SAVE_HANDLING

#ifdef XBOX_FULL_MEDIUM_HANDLING// PP: MemCard.h
						if(MemCard_fullMedium)// PP:  MemCard_fullMedium & MemCard_corruptedSave are naturally mut-ex
						{
							// PP: NOTE: there's a bit of an oversight somewhere in the pad-checking code that means that
							// PP: if you're using a pad in the second port, the unused controller2 struct will be pointing at the second port as well.
							// PP: Debouncing just controller1's okbutton therefore isn't enough; the debounced signal will get picked up on controller2
							// PP: as it is read even in 1 player.
							// PP: This bodge hides the problem.
							FORCE_OKBUTTON_DEBOUNCE_ALL_PADS;

							MemCard_fullDiskOKCallback=StartGameMenu_damagedSaveOKCallback;// PP: use StartGameMenu_damagedSaveOKCallback for disk-full as well as damaged-save

							MemCard_book.open("HARDDISK_FULL");
						}
						else
#endif// PP: define XBOX_FULL_MEDIUM_HANDLING

						{
							if(gameStatus.globalSettings.saveInfo.saveSlot[ptr->slotNum].inUse)
							{
								// NH: SM_LOAD means the options page (LOAD/SAVE), just can't be bothered changing the name at the mo.
								ptr->subState = SM_LOAD;									
							}
							// NH: ...else start new game
							else
							{
								ptr->subState = SM_NEW;										
							}
						}
					}
				}
			}	
			else if (((ptr->subState == SM_SLOTS) || (ptr->subState == SM_SLOTONE) || (ptr->subState == SM_SLOTTWO) || (ptr->subState == SM_SLOTTHREE)) && (CANCELBUTTON || (exit)))
			{
				if (frontEndAnimInfo.targetMenu != MENU_GAME)
				{
					// NH: Play bowhomb sound. Would be nice to be in PageFX, but not sure how to do it so this will work for now
					menu->startGamePage->page->playNavSound(menu->startGamePage->page->menuSounds, EMSFX_BACK);frontEndAnimInfo.requestState = GOING_HOME;
					ptr->pathToFollow = -1;
					menu->frontEndMenuBook->gotoPage(menuNames[MENU_MAIN]);
//					menu->currentPage = MENU_MAIN;
					SetFrontEndCameraDestination(FECAM_STARTGAME,FECAM_MAIN);
					bkPrintf("Back to start\n");
					ptr->subState = SM_LEAVING;
	//				ChooseFrontEndAnimationPath(MENU_MAIN);
	//				ptr->subState = SM_SLOTONE;
				}
			}
		}
		// NH: If new game or loaded game, wait till Taz's cave entrance anim is finished then load game
		else 
		{
			if (ptr->subState == SM_NEW)
			{		
				startNewGame(ptr);			
			}
			else if (ptr->subState == SM_LOAD)
			{
 				loadGame(ptr);			
			}
		}
	}
	
	/* PP: ===========================================================================================
	NOTE: I removed wads of remmed-out code from here - it's in sourcesafe's version 73 if you need it
	================================================================================================*/
}

/*	--------------------------------------------------------------------------------
	Function 	: ChooseHubToStartIn
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void ChooseHubToStartIn(uint32 slotNum)
{
	SAVESLOT	*saveSlot = &gameStatus.globalSettings.saveInfo.saveSlot[slotNum];

	if (AllowedToEnterLevel(SCENE_TAZHUB))
	{
		bkGenerateEvent("scenechange","_tazhub");
	}
	else
	{
		if (AllowedToEnterLevel(SCENE_WILDWESTHUB))
		{
			bkGenerateEvent("scenechange","_westhub");
		}
		else
		{
			if (AllowedToEnterLevel(SCENE_CITYHUB))
			{
				bkGenerateEvent("scenechange","_cityhub");
			}
			else
			{
				bkGenerateEvent("scenechange","_zoohub");
			}
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: ChooseHubToStartIn
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void PrintTimeAndDateIntoTextbox(TEXTBOX *textbox, TBClock	*clock)
{
	char	day[3];
	char	month[3];
	char	year[5];
	char	hour[3];
	char	minute[3];
	char	second[3];

	if(clock->day > 9) sprintf(day,"%d",(int)clock->day);
	else sprintf(day,"0%d",(int)clock->day);

	if(clock->month > 9) sprintf(month,"%d",(int)clock->month);
	else sprintf(month,"0%d",(int)clock->month);

	if(clock->year > 9) sprintf(year,"20%d",(int)clock->year);
	else sprintf(year,"200%d",(int)clock->year);

	if(clock->hour > 9) sprintf(hour,"%d",(int)clock->hour);
	else sprintf(hour,"0%d",(int)clock->hour);

	if(clock->minute > 9) sprintf(minute,"%d",(int)clock->minute);
	else sprintf(minute,"0%d",(int)clock->minute);

	if(clock->second > 9) sprintf(second,"%d",(int)clock->second);
	else sprintf(second,"0%d",(int)clock->second);

	if (videoMode.flags & BDISPLAYFLAG_NTSC)
	{
		// NH: Display Date in American Format
		textbox->sprintf8("%s/%s/%s ... %s:%s:%s",month,day,year,hour,minute,second);
	}
	else
	{
		// NH: Display Date in European Format
		textbox->sprintf8("%s/%s/%s ... %s:%s:%s",day,month,year,hour,minute,second);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: calculatePercentageComplete
	Purpose 	: Go on, have a guess!
	Parameters 	: int (slot number to calculate % for)
	Returns 	: int (% complete)
	Info 		:
*/

int calculatePercentageComplete_old(int slotNumber)
{
	int percentage	= 0;
	int level;

	// NH: 26%
	// NH: 7%
	level = SCENE_ICEDOME;
	percentage += ((gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].postersCollected >= 7) * 3);
	percentage += (gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].pickupsCollected >= 100);
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].secretItemCollected;
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].destructBonusCollected;
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].bonusgameOption;

	// NH: 7%
	level = SCENE_SAFARI;
	percentage += ((gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].postersCollected >= 7) * 3);
	percentage += (gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].pickupsCollected >= 100);
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].secretItemCollected;
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].destructBonusCollected;
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].bonusgameOption;

	// NH: 7%
	level = SCENE_AQUA;
	percentage += ((gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].postersCollected >= 7) * 3);
	percentage += (gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].pickupsCollected >= 100);
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].secretItemCollected;
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].destructBonusCollected;
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].bonusgameOption;

	// NH: 5%
	level = SCENE_ZOOBOSS;
	percentage += (gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].completed * 5);
	
	///

	// NH: 20% - 26%
	// NH: 5% - 7%
	level = SCENE_DEPTSTR;
	percentage += ((gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].postersCollected >= 7) * 3);
	percentage += (gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].pickupsCollected >= 100);
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].secretItemCollected;
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].destructBonusCollected;
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].bonusgameOption;

	// NH: 5% - 6%
	level = SCENE_MUSEUM;
	percentage += ((gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].postersCollected >= 7) * 3);
	percentage += (gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].pickupsCollected >= 100);
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].secretItemCollected;
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].destructBonusCollected;
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].bonusgameOption;

	// NH: 5% - 7%
	level = SCENE_CONSTRUCT;
	percentage += ((gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].postersCollected >= 7) * 3);
	percentage += (gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].pickupsCollected >= 100);
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].secretItemCollected;
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].destructBonusCollected;
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].bonusgameOption;

	// NH: 5%
	level = SCENE_CITYBOSS;
	percentage += (gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].completed * 5);

	///

	// NH: 20% - 26%
	// NH: 5% - 7%
	level = SCENE_GHOSTTOWN;
	percentage += ((gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].postersCollected >= 7) * 3);
	percentage += (gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].pickupsCollected >= 100);
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].secretItemCollected;
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].destructBonusCollected;
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].bonusgameOption;

	// NH: 5% - 7%
	level = SCENE_GOLDMINE;
	percentage += ((gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].postersCollected >= 7) * 3);
	percentage += (gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].pickupsCollected >= 100);
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].secretItemCollected;
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].destructBonusCollected;
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].bonusgameOption;

	// NH: 5% - 7%
	level = SCENE_GRANDCANYON;
	percentage += ((gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].postersCollected >= 7) * 3);
	percentage += (gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].pickupsCollected >= 100);
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].secretItemCollected;
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].destructBonusCollected;
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].bonusgameOption;

	// NH: 5%
	level = SCENE_WESTBOSS;
	percentage += (gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].completed * 5);

	///

	// NH: 20% - 22%
	// NH: 4% - 6%
	level = SCENE_TAZHUB;
	percentage += ((gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].postersCollected >= 7) * 3);
	percentage += (gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].pickupsCollected >= 100);
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].secretItemCollected;
	percentage += gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].destructBonusCollected;

	// NH: 5%
	level = SCENE_TAZPREBOSS;
	percentage += (gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].completed * 5);

	// NH: 11%
	level = SCENE_TAZBOSS;
	percentage += (gameStatus.globalSettings.saveInfo.saveSlot[slotNumber].level[level].completed * 11);

	// NH: All Done = 100%  Woohoo!!!!

	return percentage;
}

/*	--------------------------------------------------------------------------------
	Function 	: calculatePercentageComplete
	Purpose 	: Go on, have a guess!
	Parameters 	: int (slot number to calculate % for)
	Returns 	: int (% complete), -1 if slot not in use
	Info 		:
*/

int calculatePercentageComplete(int slotNumber)
{
	int		posters, pickups, destroyed, statues, mechatweety;
	uint32	scene;
	/************************************************************************************************************/
	/*	The idea is that the posters should be weighted such that they account for 70% of the completed game.	*/
	/*	Coincedentally the game contains a total of 70 posters, so each contributes 1% to the running total.	*/
	/*	Out of he remaining 30% the 10 secret statues contibute 10% and the pickups and destructibles			*/
	/*	contribute	19%. Destructibles contribute to completion dependent on whether the destruction bunus has	*/
	/*	been achieved. The last 1% is awarded when the final boss game is won and the game is trully finished.	*/
	/************************************************************************************************************/

	SAVESLOT	*slot = &gameStatus.globalSettings.saveInfo.saveSlot[slotNumber];

	if(!slot->inUse) return -1;

	// CMD: initialise counters
	posters = pickups = destroyed = statues = mechatweety = 0;
	for(scene = (uint32)SCENE_ICEDOME;scene <= (uint32)SCENE_TAZBOSS;++scene)
	{
		switch(scene)
		{
		case SCENE_ZOOBOSS:
		case SCENE_CITYHUB:
		case SCENE_CITYBOSS:
		case SCENE_WILDWESTHUB:
		case SCENE_WESTBOSS:
		case SCENE_TAZPREBOSS:
			continue;
		case SCENE_TAZBOSS:
			if(slot->level[scene].completed) mechatweety = 1;
			break;
		default:
			posters += slot->level[scene].postersCollected;
			pickups += slot->level[scene].pickupsCollected;
			if(slot->level[scene].destructBonusCollected) destroyed++;
			if(slot->level[scene].secretItemCollected) statues++;
			break;
		}
	}
	// CMD: now for the magic formula
	// CMD: check destroyed and pickups to make sure 100% isn't scuppered by casting errors
	if((destroyed == 10)&&(pickups==1000)) return posters + statues + mechatweety + 19;
	return posters + statues + mechatweety + bmFloatToInt( (((float)destroyed*0.1f)+((float)pickups*0.001f)) *19.0f*0.5f);
}
