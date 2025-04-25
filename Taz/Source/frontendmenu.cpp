// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : frontendmenu.cpp
//   Purpose : functions to create and control the front end menu system
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "TextFX.h"
#include "icon.h"
//#include "frontend.h"
#include "control.h"
#include "frontendmenu.h"
#include "frontendcamera.h"
#include "twoplayermenu.h"
#include "frontendanimation.h"
#include "animation.h"
#include "sfx.h"
#include "frontendmain.h"
#include "popup.h"
#include "secretsmenu.h"
#include "PaperBook.h"				// PP: 'Paper' book type, ie. one that looks and acts like a real physical book
#include "memcard.h"
#include "PageItemFX.h"				// PP: Special effects for page items
#include "languagemain.h"			// PP: main...language...stuff?
#include "XboxStrings.h"			// PP: Xbox versions of some strings


//int	retval = -1;
	
char *menuNames[] = 
{
	{"indexMain"},
	{"main"},
	{"startGame"},
	{"options"},
	{"audio"},
	{"twoPlayer"},
	{"extras"},
	{"records"},
	{"gallery"},
	{"credits"},
	{"secrets"},
	{"blank"},
	{"slotone"},
	{"slottwo"},
	{"slotthree"}
};

FRONTENDMENU	frontEndMenu;

/*	--------------------------------------------------------------------------------
	Function 	: CreateFrontEndMenuBook
	Purpose 	: doh, it creates the front end menu book
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void CreateFrontEndMenuBook(void)
{
	BOOK		*bookPtr;
	MENU_SOUNDS	*frontEndSoundsPtr;

	frontEndMenu.frontEndMenuBook = bookPtr = new BOOK;
	frontEndMenu.frontEndMenuBook->menuSounds = frontEndSoundsPtr = new MENU_SOUNDS;
	
	// NH: Setup the frontends sounds
	strcpy(frontEndSoundsPtr->navigateSound.name, "moveselection.wav");
	strcpy(frontEndSoundsPtr->selectSound.name, "select.wav");
	strcpy(frontEndSoundsPtr->errorSound.name, "cancel.wav");
	strcpy(frontEndSoundsPtr->backupSound.name, "deselect.wav");
	frontEndSoundsPtr->navigateSound.volume = 255;
	frontEndSoundsPtr->selectSound.volume = 255;
	frontEndSoundsPtr->errorSound.volume = 255;
	frontEndSoundsPtr->backupSound.volume = 255;

	// PP: REMOUT: must now be expressed in fractions of visible screen area	bookPtr->setRect(RECTANGLE(-300,300,-200,200));
	// PP: REMOUT: books now default to filling the full 'safe' area of the screen		bookPtr->setBookRect(RECTANGLE(-0.5f,0.5f,-0.5f,0.5f));

//	bookPtr->setBackgroundColour(COLOUR(0, 0, 0, 30));
//	bookPtr->setBorderColour(COLOUR(0,0,255,255));
	frontEndMenu.currentPage = MENU_INTROMAIN;

	CreateMainMenuPage(&frontEndMenu);

	// PP: on platforms on which loading is done after the PRESS START screen,
	// PP: we also have to delay the creation of the start-game menu page
	// PP: which displays the loaded data.
#ifndef LOAD_AFTER_START_SCREEN// PP: MemCard.h
	bkHeapCheck();
	CreateStartGameMenuPage(&frontEndMenu);
	bkHeapCheck();

	// PP: and records menu, oops
	CreateRecordsMenuPage(&frontEndMenu);
#endif// PP: ndef LOAD_AFTER_START_SCREEN// PP: MemCard.h

	CreateGameOptionsMenuPage(&frontEndMenu);
	CreateAudioOptionsMenuPage(&frontEndMenu);
	CreateTwoPlayerMenuPage(&frontEndMenu);
	// TP: CreateRecordsMenuPage(&frontEndMenu);
	CreateExtrasMenuPage(&frontEndMenu);
	CreateGalleryMenuPage(&frontEndMenu);
	CreateCreditsPage(&frontEndMenu);
	CreateSecretsMenuPage(&frontEndMenu);

//	bookPtr->open(menuNames[MENU_MAIN]);
// PP: REMOUT: probably not needed now (PAGES now align their contents when they are opened for the first time)			bookPtr->realign();
// PP: REMOUT: not needed - books default to closed		bookPtr->close(0.0f,TRUE);
}


/*	--------------------------------------------------------------------------------
	Function 	: mainMenuActionFunc
	Purpose 	: callback that responds to menu actions on the 'main' front-end menu
	Parameters 	: ref to a MENUACTION_INFO structure (TextFX.h)
	Returns 	: 
	Info 		: // PP: Must match the MenuActionFunc typedef (TextFX.h)
					// PP: I realise I'm being a bit unsociable here, splitting my little option off into its own callback,
					// PP: it's just that I think this way is nicer.
					// PP: I could change the whole main-menu over to use this system, but it'd be kinda pointless at this late stage.
*/
void mainMenuActionFunc(MENUACTION_INFO& info)
{
	char	message[256];

	if(info.item != NULL)
	{
		if(*(info.item) == "LANGUAGE")
		{
			switch(info.action)
			{
			case MENUACTION_SELECT:
				{
					// PP: Language Select option (Xbox only) has been selected.
					// PP: show the Language Select info text
					frontEndMenu.mainMenuPage->infoSubBook->gotoPage("LANGUAGE");
				}
				break;

			case MENUACTION_PRESS:
				{
					// PP: Language Select option (Xbox only) has been pressed.
					// PP: Go to the Language Select screen...

					// PP: Close all books
					BookList_closeAll();

					// PP: gotter specify no language select skipping
					skipLanguage=false;

					// PP: skip the main game intro cutscene
					skipMainGameIntroCutscene=true;
					
					// PP: generate the scene-change event to take us to the Language Select scene
					sprintf(message, "_%s", sceneFileName[SCENE_LANGUAGESELECT]);
					bkGenerateEvent("scenechange", message);
				}
				break;
			}
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: CreateMainMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void CreateMainMenuPage(FRONTENDMENU *menu)
{
	MAINMENUPAGE	*ptr;
	PAGE			*pagePtr, *subPage;// remout: not used,*subBookPagePtr;
	TEXTBOX			*dummyBox;

	ptr = menu->mainMenuPage = (MAINMENUPAGE*)ZALLOC(sizeof(MAINMENUPAGE));
	if(!ptr)
	{
		bkPrintf("*** WARNING ***, could not malloc main page/n");
		return;
	}

	// TP: Add blank page
	pagePtr = ptr->page = menu->frontEndMenuBook->addPage("BLANK");

	// NH: Add Page
	pagePtr = ptr->page = menu->frontEndMenuBook->addPage(menuNames[MENU_INTROMAIN]);
	pagePtr->setNavigation(true);
	
	// NH: Add Title Logo
	pagePtr->insertItem(ptr->titleLogo = new ICON("tazbiglogo.bmp","Intro Taz Logo"),false);
	ptr->titleLogo->setScale(5.0f)
		->setYAlign(PIFLAG_YALIGN_TOP)
		->setSelectable(false)
		->setEffect(&PIE_logoSpin, PIEMODE_ENTER)		// PP: have the logo spin up towards the screen (with sound)
		->setEffect(&PIE_iconShatter, PIEMODE_EXIT);	// PP: have the logo smash to pieces when you press start
	ptr->titleLogo->tilt=false;

	// NH: CONTROL
	// NH: Add Control Text Sub Book
	pagePtr->insertItem(ptr->introControlSubBook=new BOOK("INTRO CONTROL SUB BOOK"));
	ptr->introControlSubBook->setYPadding(false)
		->setSelectable(false);

	// NH: Add Control Text (Using Macro)
#define ADD_INTRO_CONTROLSUBBOOK_PAGE(name, controlText) \
	subPage=ptr->introControlSubBook->addPage(#name); \
	subPage->insertItem(controlText) \
		->setStyle(TS_largeControl) \
		->setYAlign(PIFLAG_YALIGN_BOTTOM) \
		->setSelectable(false);

	// NH: Add all control pages (using the macro).  Seperate pages allow for fading between the actions
#if(BPLATFORM==PC)
	ADD_INTRO_CONTROLSUBBOOK_PAGE(START,STR_PRESS_JUMP_TO_CONTINUE);
#else
	ADD_INTRO_CONTROLSUBBOOK_PAGE(START,STR_PRESS_START_TO_CONTINUE);
#endif

	ptr->introControlSubBook->open();

////////////////////
	
	// NH: Add Page
	pagePtr = ptr->page = menu->frontEndMenuBook->addPage(menuNames[MENU_MAIN]);
	pagePtr->setNavigation(true);

	// PP: navigation Y-wrapping is allowed, but only in response to a debounced input, NOT an auto-repeat input
	pagePtr->setNavigationWrap(PAGEFLAG_NAVWRAP_YDEBOUNCE);

	// PP: set the page's menu-action callback (this'll recieve notification when an item is pressed or that)
	pagePtr->setMenuActionFunc(mainMenuActionFunc);
	
	// NH: Add Title
	// NH: Can we have Taz:Wanted in the Text file?  Please!
	pagePtr->insertItem(ptr->introTitle = new TEXTBOX(STR_MFX_MARQUEE3),true)
		->setYAlign(PIFLAG_YALIGN_TOP)
		->setSelectable(false);//	dummyBox->setText("");		
	ptr->introTitle->setStyle(TS_title);

	// NH: Add Menu Items (Start Game)
	pagePtr->insertItem(ptr->startGameTb = new TEXTBOX(STR_START_GAME),true)
		->setYPadding(false);

	// NH: Add Menu Items (Game Options)
	pagePtr->insertItem(ptr->gameOptionsTb = new TEXTBOX(STR_GAME_OPTIONS),true)
		->setYPadding(false);

	// NH: Add Menu Items (Audio Options)
	pagePtr->insertItem(ptr->audioOptionsTb = new TEXTBOX(STR_AUDIO_OPTIONS),true)
		->setYPadding(false);

	// NH: Add Menu Items (Two Player)
#if BPLATFORM != XBOX
	pagePtr->insertItem(ptr->twoPlayerTb = new TEXTBOX(STR_TWO_PLAYER),true)
#else
	pagePtr->insertItem(ptr->twoPlayerTb = new TEXTBOX(STR_XBOX_MULTIPLAYER),true)
#endif
		->setYPadding(false);

	// NH: Add Menu Items (Records)
	pagePtr->insertItem(ptr->recordsTb = new TEXTBOX(STR_RECORDS),true)
		->setYPadding(false);

#if(BPLATFORM == XBOX || BPLATFORM==GAMECUBE)

	// PP: Language Select option
	pagePtr->insertItem(STR_LANGUAGE)
		->setName("LANGUAGE")
		->setYPadding(false);

#endif// PP: (BPLATFORM == XBOX || BPLATFORM==GAMECUBE)

	// NH: Add Menu Items (Extras)
	pagePtr->insertItem(ptr->extrasTb = new TEXTBOX(STR_EXTRAS),true)
		->setYPadding(false);

#if BPLATFORM==PC
	// NH: Add Menu Items (Exit)
	pagePtr->insertItem(ptr->exitTb = new TEXTBOX(STR_EXIT_EXITGAME),true)
		->setYPadding(false);
#endif

	// NH: INFO
	// NH: Add Info Text Sub Book
	pagePtr->insertItem(ptr->infoSubBook=new BOOK("INFO SUB BOOK"));
	ptr->infoSubBook->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setSelectable(false);

////////////

	// NH: Add Info Text (Using Macro)
#define ADD_INFOSUBBOOK_PAGE(name, infoText) \
	subPage=ptr->infoSubBook->addPage(#name); \
	subPage->insertItem(infoText) \
		->setStyle(TS_info) \
		->setYAlign(PIFLAG_YALIGN_BOTTOM) \
		->setAlignScale(1.2f) \
		->setSelectable(false)	

	// NH: Add all info pages (using the macro).  Seperate pages allow for fading between the actions
	ADD_INFOSUBBOOK_PAGE(START_GAME,STR_START_GAME_ACTION);
	ADD_INFOSUBBOOK_PAGE(GAME_OPTIONS,STR_GAME_OPTIONS_ACTION);
	ADD_INFOSUBBOOK_PAGE(AUDIO_OPTIONS,STR_AUDIO_OPTIONS_ACTION);
#if BPLATFORM != XBOX
	ADD_INFOSUBBOOK_PAGE(TWO_PLAYER,STR_TWO_PLAYER_ACTION);
#else
	ADD_INFOSUBBOOK_PAGE(TWO_PLAYER,STR_XBOX_MULTIPLAYER_ACTION);
#endif
	ADD_INFOSUBBOOK_PAGE(RECORDS,STR_RECORDS_ACTION);
	ADD_INFOSUBBOOK_PAGE(EXTRAS,STR_EXTRAS_ACTION);
#if(BPLATFORM == XBOX || BPLATFORM==GAMECUBE)// PP: Xbox isn't allowed to start on the language select screen, so we've put it in as an Xbox-only front-end option
	ADD_INFOSUBBOOK_PAGE(LANGUAGE, STR_LS_CHOOSE_LANGUAGE)
		->textbox()->setCase(TBCASE_SENTENCE);// PP: (just me being a fussy shit): This string was originally a title and so the English version is in title-case; convert to sentence case for the info box
#endif// PP: (BPLATFORM == XBOX || BPLATFORM==GAMECUBE)
	ADD_INFOSUBBOOK_PAGE(EXIT,STR_EXIT_EXITGAME);

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
	ADD_CONTROLSUBBOOK_PAGE(CONTINUE,STR_PRESS_X_TO_CONTINUE);

	ptr->infoSubBook->open();// PP: books default to closed
	ptr->controlSubBook->open();

	// NH: Set initial states
	ptr->subState = MM_INTRO;
	ptr->page->selectItem(ptr->startGameTb);	
}

/*	--------------------------------------------------------------------------------
	Function 	: FreeFrontEndMenuBook
	Purpose 	: guess what, it frees the front end memory book and any associated memory
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void FreeFrontEndMenuBook(void)
{
//	if(frontEndMenu)
	{
		SAFE_DELETE(frontEndMenu.galleryPage->textBook);

		// PP: get rid of the galleryBook's close callback because there will be null pointers
		frontEndMenu.galleryPage->galleryBook->setCloseCallback(NULL);

		SAFE_DELETE(frontEndMenu.galleryPage->galleryBook);

		SAFE_FREE(frontEndMenu.audioPage);
		SAFE_FREE(frontEndMenu.extrasPage);
		SAFE_FREE(frontEndMenu.gameOptionsPage);
		SAFE_FREE(frontEndMenu.mainMenuPage);
		SAFE_FREE(frontEndMenu.recordsPage);
		SAFE_FREE(frontEndMenu.startGamePage);
		SAFE_FREE(frontEndMenu.twoPlayerPage);
		SAFE_FREE(frontEndMenu.galleryPage);
		SAFE_FREE(frontEndMenu.creditsPage);
		SAFE_FREE(frontEndMenu.secretsPage);
	}
	if (frontEndMenu.frontEndMenuBook)
	{
		delete frontEndMenu.frontEndMenuBook;
		frontEndMenu.frontEndMenuBook = NULL;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateFrontEndMenuBook
	Purpose 	: i'm not going to even bother with this one, work it out yourself
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateFrontEndMenuBook(void)
{
	if(!frontEndMenu.frontEndMenuBook) return;

	switch(frontEndMenu.currentPage)
	{
	case MENU_INTROMAIN:
	case MENU_MAIN:
		UpdateMainMenuPage(&frontEndMenu, &controller1);
		break;
	case MENU_STARTGAME:
		UpdateStartGameMenuPage(&frontEndMenu, &controller1);
		break;
	case MENU_OPTIONS:
		UpdateGameOptionsMenuPage(&frontEndMenu, &controller1);
		break;
	case MENU_AUDIO:
		UpdateAudioOptionsMenuPage(&frontEndMenu, &controller1);
		break;
	case MENU_TWOPLAYER:
		UpdateTwoPlayerMenuPage(&frontEndMenu, &controller1);
		break;
	case MENU_RECORDS:
		UpdateRecordsMenuPage(&frontEndMenu, &controller1);
		break;
	case MENU_EXTRAS:
		UpdateExtrasMenuPage(&frontEndMenu, &controller1);
		break;
	case MENU_GALLERY:
		UpdateGalleryMenuPage(&frontEndMenu, &controller1);
		break;
	case MENU_CREDITS:
		UpdateCreditsPage(&frontEndMenu, &controller1);
		break;
	case MENU_SECRETS:
		UpdateSecretsMenuPage(&frontEndMenu, &controller1);
		break;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateMainMenuPage
	Purpose 	: i'm not going to even bother with this one, work it out yourself
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateMainMenuPage(FRONTENDMENU *menu, PADCONTROLSTATUS *controller)
{
	MAINMENUPAGE	*ptr = menu->mainMenuPage;
	int retval;

#if BPLATFORM!=PC
	// NH: Disable 2 player menu if only controller 1 plugged in
	if (biFindPad(1)==-1)
	{
		if (ptr->page->selectedItem == ptr->twoPlayerTb)
		{
			menu->frontEndMenuBook->getCurrentPage()->selectItem(ptr->startGameTb);
		}
		ptr->twoPlayerTb->enable(false);
		ptr->twoPlayerTb->setSelectable(false);
	}
	else
	{
		ptr->twoPlayerTb->enable(true);
		ptr->twoPlayerTb->setSelectable(true);		
	}
#else
	// NH: EXIT Popup
	// NH: If popup open
	if (frontEndMenu.popUpOptions)
	{
		// NH: Get Response from popup
		retval = frontEndMenu.popUpOptions->getResponse();

		if (CANCELBUTTON || OKBUTTON)
		{
			// NH: Close pop up
			// PP: REMOUT: nah		frontEndMenu.popUpOptions->close(0.0f,TRUE);
			BookList_closeAndFree(frontEndMenu.popUpOptions);
			menu->frontEndMenuBook->getPage(menuNames[MENU_MAIN])->setNavigation(true);
			menu->mainMenuPage->infoSubBook->open();
			menu->mainMenuPage->controlSubBook->open();

			switch (retval)
			{
			case 1:
				SaveGame(map.sceneNumber, true);
				gameStatus.quitNow = true;
				break;
			}
		}		
	}
	else
#endif
	if (ptr->subState == MM_INTRO)
	{
		menu->frontEndMenuBook->gotoPage(menuNames[MENU_INTROMAIN]);
		
		// PP: can't allow X button here because we're explicitly saying 'Press START' (bug 349)

		bool doStart=false;

#ifdef ALLOW_CONTROLLERS_IN_ANY_PORTS

		// PP: aight this is wonna dem 'press start' situations.
		// PP: when start is pressed, we'll know what pad player 1 is using.
		padCheckMode=PADCHECK_WAITING_FOR_PLAYER1;
		// PP: ~~~~~~~~~~~~~~~~~~~

		// PP: check for the start button being pressed on any pad;
		// PP: the first pad to press start will become controller1
		if(doStart=checkForStartAndBind(controller1))
		{
			padCheckMode=PADCHECK_NORMAL;

			// PP: If required, perform initial global load now
			CheckMemCardOnStart();
		}

#else// PP: ndef ALLOW_CONTROLLERS_IN_ANY_PORTS

#if(BPLATFORM == PC)
		doStart=OKBUTTON;
#else// PP: !pc
		doStart=(controller1.startDebounceChannel->value != 0);
#endif// PP: !pc

#endif// PP: ndef ALLOW_CONTROLLERS_IN_ANY_PORTS

		if(doStart)
		{
			frontEndAnimInfo.targetMenu = frontEndAnimInfo.lastMenu = frontEndMenu.currentPage = MENU_MAIN;
			menu->frontEndMenuBook->gotoPage(menuNames[MENU_MAIN]);
			// TP: need to wait for load to finish or the startGamePage might not be created in time, caused crash if pressing start multiple times
#ifdef LOAD_AFTER_START_SCREEN
			ptr->subState = MM_WAITFORLOAD;	
#else
			ptr->subState = MM_START;	
#endif
		}
	}
	else if (ptr->subState == MM_WAITFORLOAD)
	{
		if (menu->startGamePage)
		{
			ptr->subState = MM_START;	
		}
	}
	else
	{
		if (ptr->page->selectedItem == ptr->startGameTb)
		{	
			if(OKBUTTON)
			{
				menu->frontEndMenuBook->gotoPage(menuNames[MENU_STARTGAME]);
				menu->currentPage = MENU_STARTGAME;
				bkPrintf("START GAME\n");
				frontEndAnimInfo.requestState = SG_IDLE;
				SetFrontEndCameraDestination(FECAM_MAIN,FECAM_STARTGAME);
				ChooseFrontEndAnimationPath(MENU_STARTGAME);
				// NH: States that no slot is chosen
				menu->startGamePage->page->selectItem(menu->startGamePage->slotOneTb);
				menu->startGamePage->subState		= menu->startGamePage->lastSubState = SM_SLOTS;
				menu->startGamePage->slotNum		= 0;
				menu->startGamePage->currentSlot	= 0;
				menu->startGamePage->pathToFollow	= -1;
				FORCE_OKBUTTON_DEBOUNCE;
			}
			
			ptr->infoSubBook->gotoPage("START_GAME");
			ptr->controlSubBook->gotoPage("CONTINUE");
		}
		// NH: else if Game Options selected
		else if (ptr->page->selectedItem == ptr->gameOptionsTb)
		{
			if(OKBUTTON)
			{
				menu->frontEndMenuBook->gotoPage(menuNames[MENU_OPTIONS]);
				menu->currentPage = MENU_OPTIONS;
				SetFrontEndCameraDestination(FECAM_MAIN,FECAM_OPTIONS);
				frontEndAnimInfo.requestState = OM_PIZZA_IDLE;
				//ChooseFrontEndAnimationPath(MENU_OPTIONS);
				menu->gameOptionsPage->subState = GO_DIFFICULTY;
				menu->gameOptionsPage->lastSubState = GO_NONE;
				menu->gameOptionsPage->page->selectItem(menu->gameOptionsPage->difficultyTb);
				menu->gameOptionsPage->resetScreen = FALSE;
				menu->gameOptionsPage->rumbleHandle = -1;
			}
			
 			ptr->infoSubBook->gotoPage("GAME_OPTIONS");
			ptr->controlSubBook->gotoPage("CONTINUE");
		}
		// NH: Audio Options selected
		else if (ptr->page->selectedItem == ptr->audioOptionsTb)
		{
			if(OKBUTTON)
			{
				menu->frontEndMenuBook->gotoPage(menuNames[MENU_AUDIO]);
				menu->currentPage = MENU_AUDIO;
				SetFrontEndCameraDestination(FECAM_MAIN,FECAM_AUDIO);
				frontEndAnimInfo.requestState = AM_GRAMOPHONE;
				//ChooseFrontEndAnimationPath(MENU_AUDIO);
				menu->audioPage->subState = AU_MUSICVOL;
				menu->audioPage->page->selectItem(menu->audioPage->musicVolTb);
			}
			
			ptr->infoSubBook->gotoPage("AUDIO_OPTIONS");
			ptr->controlSubBook->gotoPage("CONTINUE");
		}
		// NH: 2 Player selected
		else if (ptr->page->selectedItem == ptr->twoPlayerTb)
		{
			if(OKBUTTON)
			{
				twoPlayerSubMenu = false;
				menu->frontEndMenuBook->gotoPage(menuNames[MENU_TWOPLAYER]);
				menu->currentPage = MENU_TWOPLAYER;
				SetFrontEndCameraDestination(FECAM_MAIN,FECAM_TWOPLAYER);
				frontEndAnimInfo.requestState = TP_IDLE;
				//ChooseFrontEndAnimationPath(MENU_TWOPLAYER);
				//menu->twoPlayerPage->subState = TP_DEST;	
				menu->twoPlayerPage->page->selectItem(menu->twoPlayerPage->destructionTb);
			}
			
			ptr->infoSubBook->gotoPage("TWO_PLAYER");
			ptr->controlSubBook->gotoPage("CONTINUE");
		}
		// NH: Records selected
		else if (ptr->page->selectedItem == ptr->recordsTb)
		{
			if(OKBUTTON)
			{
				menu->recordsPage->hubControlBook->gotoPage("overallHub",true);
				menu->recordsPage->levelControlBook->gotoPage("overallLevel",true);
				menu->recordsPage->overallBook->gotoPage("overall",true);
				menu->recordsPage->leftHubIconBook->gotoPage("off",true);
				menu->recordsPage->rightHubIconBook->gotoPage("on",true);	
				menu->recordsPage->leftLevelIconBook->gotoPage("off",true);
				menu->recordsPage->rightLevelIconBook->gotoPage("off",true);
				
				menu->frontEndMenuBook->gotoPage(menuNames[MENU_RECORDS]);
				menu->currentPage = MENU_RECORDS;
				SetFrontEndCameraDestination(FECAM_MAIN,FECAM_RECORDS);
				frontEndAnimInfo.requestState = RM_IDLE;
//				ChooseFrontEndAnimationPath(MENU_RECORDS);
				menu->recordsPage->subState = R_OVERALL;
				menu->recordsPage->subSubState = R_SUB_OVERALL1;	
				menu->recordsPage->page->selectItem(menu->recordsPage->hubControl[4]);

			}
			
			ptr->infoSubBook->gotoPage("RECORDS");
			ptr->controlSubBook->gotoPage("CONTINUE");
		}
		// NH: Extras selected
		else if (ptr->page->selectedItem == ptr->extrasTb)
		{
			if(OKBUTTON)
			{
 				menu->frontEndMenuBook->gotoPage(menuNames[MENU_EXTRAS]);
				menu->currentPage = MENU_EXTRAS;
				SetFrontEndCameraDestination(FECAM_MAIN,FECAM_EXTRAS);
				frontEndAnimInfo.requestState = EM_IDLE;
				//ChooseFrontEndAnimationPath(MENU_EXTRAS);
				menu->extrasPage->subState = EX_CREDITS;
				menu->extrasPage->page->selectItem(menu->extrasPage->creditsTb);
			}
			
			ptr->infoSubBook->gotoPage("EXTRAS");
			ptr->controlSubBook->gotoPage("CONTINUE");
		}
#if BPLATFORM == PC
		// NH: Exit selected
		else if (ptr->page->selectedItem == ptr->exitTb)
		{
			if(OKBUTTON)
			{
				// NH: If no popup, then create one
				if (!frontEndMenu.popUpOptions)
				{
					menu->frontEndMenuBook->getPage(menuNames[MENU_MAIN])->setNavigation(false);
					ptr->infoSubBook->close();
					ptr->controlSubBook->close();

					frontEndMenu.popUpOptions = new POPUP(POPUP_CONFIRM, false);
				}
			}
			
			ptr->infoSubBook->gotoPage("EXIT");
			ptr->controlSubBook->gotoPage("CONTINUE");
		}
#endif
	}
}
