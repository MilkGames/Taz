// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : startgamemenu.cpp
//   Purpose : functions to create and control the two player game menu
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "textfx.h"
#include "control.h"
#include "frontendmenu.h"
#include "twoplayermenu.h"
#include "frontendcamera.h"
#include "animation.h"
#include "tournament.h"
#include "display.h"				// PP: provide basic display setup and managment functions
#include "sfx.h"
#include "frontendmain.h"
#include "tournament.h"
#include "Cheats.h"					// PP: Cheats
#include "multiplayer.h"			// PP: multi-player stuff

bool twoPlayerSubMenu	= false;	// NH: Flag to track if the user is in a submenu (If so only step back to higher level menu)
bool onTournament		= false;
/*	--------------------------------------------------------------------------------
	Function 	: CreateGameOptionsMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void CreateTwoPlayerMenuPage(FRONTENDMENU *menu)
{
	TWOPLAYERPAGE	*ptr;
	PAGE			*pagePtr, *subPage;
	TEXTBOX			*dummyBox, *tempText;
	ICON			*dummyIcon;
	BOOK			*machineControlsBook;

	ptr = menu->twoPlayerPage = (TWOPLAYERPAGE*)ZALLOC(sizeof(TWOPLAYERPAGE));
	if(!ptr)
	{
		bkPrintf("*** WARNING ***, could not malloc two plyer select page/n");
		return;
	}
	// NH: Add Page (Options)
	pagePtr = ptr->page = menu->frontEndMenuBook->addPage(menuNames[MENU_TWOPLAYER]);
	pagePtr->setNavigation(true);

	// PP: navigation Y-wrapping is allowed, but only in response to a debounced input, NOT an auto-repeat input
	pagePtr->setNavigationWrap(PAGEFLAG_NAVWRAP_YDEBOUNCE);
	
	// NH: Add Title
#if BPLATFORM != XBOX
	pagePtr->insertItem(ptr->titleTb = new TEXTBOX(STR_TWO_PLAYER),false);
#else
	pagePtr->insertItem(ptr->titleTb = new TEXTBOX(STR_XBOX_MULTIPLAYER),false);
#endif
	ptr->titleTb->setStyle(TS_title)
		->setYAlign(PIFLAG_YALIGN_TOP)
		->setSelectable(false);

	// NH: Add Menu Items
	pagePtr->insertItem(ptr->destructionTb = new TEXTBOX(STR_TP_DESTRUCTION),true);
	ptr->destructionTb->setYPadding(false);

	pagePtr->insertItem(ptr->raceModeTb = new TEXTBOX(STR_TP_VEHICLE_RACE),true);
	ptr->raceModeTb->setYPadding(false);

	pagePtr->insertItem(ptr->timeTrialTb = new TEXTBOX(STR_TP_RACE_MODE),true);
	ptr->timeTrialTb->setYPadding(false);
	
	pagePtr->insertItem(ptr->tournamentTb = new TEXTBOX(STR_TP_TOURNAMENT),true);
	ptr->tournamentTb->setYPadding(false);

	pagePtr->insertItem(ptr->secretGamesTb = new TEXTBOX(STR_TP_SECRETS),true);
	ptr->secretGamesTb->setYPadding(false);

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
	ADD_INFOSUBBOOK_PAGE(VEHICLE_RACE,STR_TP_VEHICLE_RACE_ACTION);
	ADD_INFOSUBBOOK_PAGE(RACE_MODE,STR_TP_RACE_MODE_ACTION);
	ADD_INFOSUBBOOK_PAGE(DESTRUCTION,STR_TP_DESTRUCTION_ACTION);
	ADD_INFOSUBBOOK_PAGE(TOURNAMENT,STR_TP_TOURNAMENT_ACTION);
	ADD_INFOSUBBOOK_PAGE(SECRETS,STR_TP_SECRETS_ACTION);

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

	ptr->infoSubBook->open();
	ptr->controlSubBook->open();

	////

	// Add new page for two player race games
	pagePtr = menu->frontEndMenuBook->addPage("vrraces");
	pagePtr->setNavigation(true);

	// PP: navigation Y-wrapping is allowed, but only in response to a debounced input, NOT an auto-repeat input
	pagePtr->setNavigationWrap(PAGEFLAG_NAVWRAP_YDEBOUNCE);

	// NH: Add Title
	pagePtr->insertItem(ptr->titleTb = new TEXTBOX(STR_TP_VEHICLE_RACE),false);
	ptr->titleTb->setStyle(TS_title)
		->setYAlign(PIFLAG_YALIGN_TOP)
		->setSelectable(false);
	
	pagePtr->insertItem(ptr->vrracesTb[0] = new TEXTBOX(STR_TP_RACE_DEPTSTR), true);
		ptr->vrracesTb[0]->setYPadding(false);

	pagePtr->insertItem(ptr->vrracesTb[1] = new TEXTBOX(STR_TP_RACE_MUSEUM), true);
		ptr->vrracesTb[1]->setYPadding(false);

	pagePtr->insertItem(ptr->vrracesTb[2] = new TEXTBOX(STR_TP_RACE_CANYON), true);
		ptr->vrracesTb[2]->setYPadding(false);

	// NH: INFO
	// NH: Add Info Text Sub Book
	pagePtr->insertItem(ptr->gameInfoSubBook[0]=new BOOK("GAME INFO 0 SUB BOOK"));
	ptr->gameInfoSubBook[0]->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setSelectable(false);

	// NH: Add Info Text (Using Macro)
#define ADD_GAME0INFOSUBBOOK_PAGE(name, infoText) \
	subPage=ptr->gameInfoSubBook[0]->addPage(#name); \
	subPage->setNavigation(true); \
	subPage->insertItem(infoText) \
		->setStyle(TS_info) \
		->setYAlign(PIFLAG_YALIGN_BOTTOM) \
		->setAlignScale(1.2f) \
		->setSelectable(false);	

	// NH: Add all info pages (using the macro).  Seperate pages allow for fading between the actions
	ADD_GAME0INFOSUBBOOK_PAGE(VR_DEPTSTR,STR_TP_TROLLEY_ACTION);
	ADD_GAME0INFOSUBBOOK_PAGE(VR_MUSEUM,STR_TP_POLISHER_ACTION);
	ADD_GAME0INFOSUBBOOK_PAGE(VR_CANYON,STR_TP_JETBIKE_ACTION);

	// NH: CONTROL
	// NH: Add Control Text Sub Book
	pagePtr->insertItem(ptr->gameControlSubBook[0]=new BOOK("GAME CONTROL 0 SUB BOOK"));
	ptr->gameControlSubBook[0]->setYPadding(false)
		->setSelectable(false);

	subPage=ptr->gameControlSubBook[0]->addPage("SELECT");
	subPage->insertItem(STR_X_TO_SELECT_Y_TO_RETURN) 
		->setStyle(TS_control) 
		->setYPadding(false) 
		->setSelectable(false);

	ptr->gameInfoSubBook[0]->open();
	ptr->gameControlSubBook[0]->open();

	////

	// Add new page for two player Time Trial games
	pagePtr = menu->frontEndMenuBook->addPage("races");
	pagePtr->setNavigation(true);

	// PP: navigation Y-wrapping is allowed, but only in response to a debounced input, NOT an auto-repeat input
	pagePtr->setNavigationWrap(PAGEFLAG_NAVWRAP_YDEBOUNCE);

	// NH: Add Title
	pagePtr->insertItem(ptr->titleTb = new TEXTBOX(STR_TP_RACE_MODE),false);
	ptr->titleTb->setStyle(TS_title)
		->setYAlign(PIFLAG_YALIGN_TOP)
		->setSelectable(false);
	
	pagePtr->insertItem(ptr->racesTb[0] = new TEXTBOX(STR_TP_RACE_ICEDOME), true);
		ptr->racesTb[0]->setYPadding(false);

	pagePtr->insertItem(ptr->racesTb[1] = new TEXTBOX(STR_TP_RACE_GOLDMINE), true);
		ptr->racesTb[1]->setYPadding(false);

	pagePtr->insertItem(ptr->racesTb[2] = new TEXTBOX(STR_TP_RACE_SAFARI), true);
		ptr->racesTb[2]->setYPadding(false);

	// NH: INFO
	// NH: Add Info Text Sub Book
	pagePtr->insertItem(ptr->gameInfoSubBook[1]=new BOOK("GAME INFO 1 SUB BOOK"));
	ptr->gameInfoSubBook[1]->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setSelectable(false);

	// NH: Add Info Text (Using Macro)
#define ADD_GAME1INFOSUBBOOK_PAGE(name, infoText) \
	subPage=ptr->gameInfoSubBook[1]->addPage(#name); \
	subPage->insertItem(infoText) \
		->setStyle(TS_info) \
		->setYAlign(PIFLAG_YALIGN_BOTTOM) \
		->setAlignScale(1.2f) \
		->setSelectable(false);	

	// NH: Add all info pages (using the macro).  Seperate pages allow for fading between the actions
	ADD_GAME1INFOSUBBOOK_PAGE(TA_ICEDOME,STR_TP_ICEDOME_ACTION);
	ADD_GAME1INFOSUBBOOK_PAGE(TA_GOLDMINE,STR_TP_GOLDMINE_ACTION);
	ADD_GAME1INFOSUBBOOK_PAGE(TA_SAFARI,STR_TP_SAFARI_ACTION);

	// NH: CONTROL
	// NH: Add Control Text Sub Book
	pagePtr->insertItem(ptr->gameControlSubBook[1]=new BOOK("GAME CONTROL 1 SUB BOOK"));
	ptr->gameControlSubBook[1]->setYPadding(false)
		->setSelectable(false);

	subPage=ptr->gameControlSubBook[1]->addPage("SELECT");
	subPage->insertItem(STR_X_TO_SELECT_Y_TO_RETURN) 
		->setStyle(TS_control) 
		->setYPadding(false) 
		->setSelectable(false);

	ptr->gameInfoSubBook[1]->open();
	ptr->gameControlSubBook[1]->open();

	////

	// Add new page for two player Destruction games
	pagePtr = menu->frontEndMenuBook->addPage("destruction");
	pagePtr->setNavigation(true);

	// PP: navigation Y-wrapping is allowed, but only in response to a debounced input, NOT an auto-repeat input
	pagePtr->setNavigationWrap(PAGEFLAG_NAVWRAP_YDEBOUNCE);

	// NH: Add Title
	pagePtr->insertItem(ptr->titleTb = new TEXTBOX(STR_TP_DESTRUCTION),false);
	ptr->titleTb->setStyle(TS_title)
		->setYAlign(PIFLAG_YALIGN_TOP)
		->setSelectable(false);
	
	pagePtr->insertItem(ptr->destGamesTb[0] = new TEXTBOX(STR_TP_DEST_AQUA), true);
		ptr->destGamesTb[0]->setYPadding(false);

	pagePtr->insertItem(ptr->destGamesTb[1] = new TEXTBOX(STR_TP_DEST_CONSTRUCTION), true);
		ptr->destGamesTb[1]->setYPadding(false);

	pagePtr->insertItem(ptr->destGamesTb[2] = new TEXTBOX(STR_TP_DEST_GHOSTTOWN), true);
		ptr->destGamesTb[2]->setYPadding(false);

	// NH: INFO
	// NH: Add Info Text Sub Book
	pagePtr->insertItem(ptr->gameInfoSubBook[2]=new BOOK("GAME INFO 2 SUB BOOK"));
	ptr->gameInfoSubBook[2]->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setSelectable(false);

	// NH: Add Info Text (Using Macro)
#define ADD_GAME2INFOSUBBOOK_PAGE(name, infoText) \
	subPage=ptr->gameInfoSubBook[2]->addPage(#name); \
	subPage->insertItem(infoText) \
		->setStyle(TS_info) \
		->setYAlign(PIFLAG_YALIGN_BOTTOM) \
		->setAlignScale(1.2f) \
		->setSelectable(false);	

	// NH: Add all info pages (using the macro).  Seperate pages allow for fading between the actions
	ADD_GAME2INFOSUBBOOK_PAGE(DE_AQUA,STR_TP_AQUA_ACTION);
	ADD_GAME2INFOSUBBOOK_PAGE(DE_CONSTRUCT,STR_TP_CONSTRUCT_ACTION);
	ADD_GAME2INFOSUBBOOK_PAGE(DE_TASMANIA,STR_TP_GHOSTTOWN_ACTION);

	// NH: CONTROL
	// NH: Add Control Text Sub Book
	pagePtr->insertItem(ptr->gameControlSubBook[2]=new BOOK("GAME CONTROL 2 SUB BOOK"));
	ptr->gameControlSubBook[2]->setYPadding(false)
		->setSelectable(false);

	subPage=ptr->gameControlSubBook[2]->addPage("SELECT");
	subPage->insertItem(STR_X_TO_SELECT_Y_TO_RETURN) 
		->setStyle(TS_control) 
		->setYPadding(false) 
		->setSelectable(false);

	ptr->gameInfoSubBook[2]->open();
	ptr->gameControlSubBook[2]->open();

	////
#define SMALL_FONTSIZE 0.85f

	// Add new page for two player Tournament games
	pagePtr = menu->frontEndMenuBook->addPage("tournament");
	pagePtr->setNavigation(true);

	// PP: navigation X-wrapping is allowed, but only in response to a debounced input, NOT an auto-repeat input
	pagePtr->setNavigationWrap(PAGEFLAG_NAVWRAP_XDEBOUNCE);

	// NH: Add Title
	pagePtr->insertItem(ptr->titleTb = new TEXTBOX(STR_TP_TOURNAMENT));
	ptr->titleTb->setStyle(TS_title)
		->setYAlign(PIFLAG_YALIGN_TOP)
		->setSelectable(false);

	////
	// NH: Insert Free Items for the tournament icon and names to appear in when spinning has finished

	pagePtr->insertFreeItem(ptr->iconBook[0] = new BOOK("icon1book"), RECTANGLE(-0.45f, -0.17f, 0.1f, 0.35f));
	ptr->iconBook[0]->setSelectable(false);

	// NH: No icon
	subPage = ptr->iconBook[0]->addPage("off");

	// NH: Icon
	subPage = ptr->iconBook[0]->addPage("on");
	subPage->insertItem(ptr->tourneyIcon[0] = new ICON());
	ptr->tourneyIcon[0]->setSelectable(false)
		->setYPadding(false);
	ptr->tourneyIcon[0]->tilt = false;

	subPage->insertItem(ptr->tourneyText[0] = new TEXTBOX(), true);
	ptr->tourneyText[0]->setWrap(true)
		->setStyle(TS_info) 
		->setScale(SMALL_FONTSIZE)
		->setYAlign(PIFLAG_YALIGN_TOP);

	ptr->iconBook[0]->open();

	////

	pagePtr->insertFreeItem(ptr->iconBook[1] = new BOOK("icon2book"), RECTANGLE(-0.14f, 0.14f, 0.1f, 0.35f));
	ptr->iconBook[1]->setSelectable(false);

	// NH: No icon
	subPage = ptr->iconBook[1]->addPage("off");

	// NH: Icon
	subPage = ptr->iconBook[1]->addPage("on");
	subPage->insertItem(ptr->tourneyIcon[1] = new ICON());
	ptr->tourneyIcon[1]->setSelectable(false)
		->setYPadding(false);
	ptr->tourneyIcon[1]->tilt = false;

	subPage->insertItem(ptr->tourneyText[1] = new TEXTBOX(), true);
	ptr->tourneyText[1]->setWrap(true)
		->setStyle(TS_info) 
		->setScale(SMALL_FONTSIZE)
		->setYAlign(PIFLAG_YALIGN_TOP);

	ptr->iconBook[1]->open();

	////

	pagePtr->insertFreeItem(ptr->iconBook[2] = new BOOK("icon3book"), RECTANGLE(0.17f, 0.45f, 0.1f, 0.35f));
	ptr->iconBook[2]->setSelectable(false);

	// NH: No icon
	subPage = ptr->iconBook[2]->addPage("off");

	// NH: Icon
	subPage = ptr->iconBook[2]->addPage("on");
	subPage->insertItem(ptr->tourneyIcon[2] = new ICON());
	ptr->tourneyIcon[2]->setSelectable(false)
		->setYPadding(false);
	ptr->tourneyIcon[2]->tilt = false;

	subPage->insertItem(ptr->tourneyText[2] = new TEXTBOX(), true);
	ptr->tourneyText[2]->setWrap(true)
		->setStyle(TS_info)
		->setScale(SMALL_FONTSIZE)
		->setYAlign(PIFLAG_YALIGN_TOP);

	ptr->iconBook[2]->open();

	////

	// NH: Add Free Item for the Slot Machine controls
	
	pagePtr->insertFreeItem(machineControlsBook = new BOOK(), RECTANGLE(-0.475f, 0.475f, -0.24f, -0.34f));
	subPage = machineControlsBook->addPage();

	subPage->insertItem(ptr->tourneyTb[0] = new TEXTBOX(STR_PULL), true);
	ptr->tourneyTb[0]->setScale(SMALL_FONTSIZE);
	subPage->insertItem(ptr->tourneyTb[1] = new TEXTBOX(STR_HOLD), false);
	ptr->tourneyTb[1]->setScale(SMALL_FONTSIZE);
	subPage->insertItem(ptr->tourneyTb[2] = new TEXTBOX(STR_HOLD), false);
	ptr->tourneyTb[2]->setScale(SMALL_FONTSIZE);
	subPage->insertItem(ptr->tourneyTb[3] = new TEXTBOX(STR_HOLD), false);
	ptr->tourneyTb[3]->setScale(SMALL_FONTSIZE);
	subPage->insertItem(ptr->tourneyTb[4] = new TEXTBOX(STR_COLLECT), false);
	ptr->tourneyTb[4]->setScale(SMALL_FONTSIZE);
	
	machineControlsBook->open();

	// NH: INFO
	// NH: Add Info Text Sub Book
	pagePtr->insertItem(ptr->gameInfoSubBook[3]=new BOOK("GAME INFO 3 SUB BOOK"));
	ptr->gameInfoSubBook[3]->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setYPadding(false) 
		->setSelectable(false);

	// NH: Add Info Text (Using Macro)
#define ADD_GAME3INFOSUBBOOK_PAGE(name, infoText) \
	subPage=ptr->gameInfoSubBook[3]->addPage(#name); \
	subPage->insertItem(infoText) \
		->setStyle(TS_info) \
		->setYAlign(PIFLAG_YALIGN_BOTTOM) \
		->setAlignScale(1.2f) \
		->setSelectable(false);	

	// NH: Add all info pages (using the macro).  Seperate pages allow for fading between the actions
	ADD_GAME3INFOSUBBOOK_PAGE(T_SELECT,STR_FILLER);
	ADD_GAME3INFOSUBBOOK_PAGE(T_GETGAMES,STR_TP_GET_GAMES);
	ADD_GAME3INFOSUBBOOK_PAGE(T_REEL1,STR_TP_HOLD_REEL_1);
	ADD_GAME3INFOSUBBOOK_PAGE(T_REEL2,STR_TP_HOLD_REEL_2);
	ADD_GAME3INFOSUBBOOK_PAGE(T_REEL3,STR_TP_HOLD_REEL_3);
	ADD_GAME3INFOSUBBOOK_PAGE(T_PLAY,STR_TP_PLAY_TOURNAMENT);

	// NH: CONTROL
	// NH: Add Control Text Sub Book
	pagePtr->insertItem(ptr->gameControlSubBook[3]=new BOOK("GAME CONTROL 3 SUB BOOK"));
	ptr->gameControlSubBook[3]->setYPadding(false)
		->setSelectable(false);

	subPage=ptr->gameControlSubBook[3]->addPage("SELECT");
	subPage->insertItem(STR_X_TO_SELECT_Y_TO_RETURN) 
		->setStyle(TS_control) 
		->setYPadding(false) 
		->setSelectable(false);

	ptr->gameInfoSubBook[3]->open();
	ptr->gameControlSubBook[3]->open();

	//////////////

	// Add new page for two player Secret games
	pagePtr = menu->frontEndMenuBook->addPage("bosses");
	pagePtr->setNavigation(true);

	// PP: navigation Y-wrapping is allowed, but only in response to a debounced input, NOT an auto-repeat input
	pagePtr->setNavigationWrap(PAGEFLAG_NAVWRAP_YDEBOUNCE);

	// NH: Add Title
	pagePtr->insertItem(ptr->titleTb = new TEXTBOX(STR_TP_SECRETS),false);
	ptr->titleTb->setStyle(TS_title)
		->setYAlign(PIFLAG_YALIGN_TOP)
		->setSelectable(false);

	pagePtr->insertItem(ptr->bossesTb[0] = new TEXTBOX(STR_TP_BOSS_ZOO), true);
	ptr->bossesTb[0]->setYPadding(false);
	
	pagePtr->insertItem(ptr->bossesTb[1] = new TEXTBOX(STR_TP_BOSS_CITY), true);
	ptr->bossesTb[1]->setYPadding(false);
	
	pagePtr->insertItem(ptr->bossesTb[2] = new TEXTBOX(STR_TP_BOSS_WEST), true);
	ptr->bossesTb[2]->setYPadding(false);
	
	pagePtr->selectItem(pagePtr->getFirstSelectableItem(true));

	// NH: INFO
	// NH: Add Info Text Sub Book
	pagePtr->insertItem(ptr->gameInfoSubBook[4]=new BOOK("GAME INFO 4 SUB BOOK"));
	ptr->gameInfoSubBook[4]->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setSelectable(false);

	// NH: Add Info Text (Using Macro)
#define ADD_GAME4INFOSUBBOOK_PAGE(name, infoText) \
	subPage=ptr->gameInfoSubBook[4]->addPage(#name); \
	subPage->insertItem(infoText) \
		->setStyle(TS_info) \
		->setYAlign(PIFLAG_YALIGN_BOTTOM) \
		->setAlignScale(1.2f) \
		->setSelectable(false);	

	// NH: Add all info pages (using the macro).  Seperate pages allow for fading between the actions
	ADD_GAME4INFOSUBBOOK_PAGE(S_NONE_AVAILABLE,STR_NONE_AVAILABLE);
	ADD_GAME4INFOSUBBOOK_PAGE(S_SELECT,STR_SELECT_GAME);

	// NH: CONTROL
	// NH: Add Control Text Sub Book
	pagePtr->insertItem(ptr->gameControlSubBook[4]=new BOOK("GAME CONTROL 4 SUB BOOK"));
	ptr->gameControlSubBook[4]->setYPadding(false)
		->setSelectable(false);

	subPage=ptr->gameControlSubBook[4]->addPage("SELECT");
	subPage->insertItem(STR_X_TO_SELECT_Y_TO_RETURN) 
		->setStyle(TS_control) 
		->setYPadding(false) 
		->setSelectable(false);

	ptr->gameInfoSubBook[4]->open();
	ptr->gameControlSubBook[4]->open();

	InitTournament();
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateTwoPlayerMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateTwoPlayerMenuPage(FRONTENDMENU *menu, PADCONTROLSTATUS *controller)
{
	TWOPLAYERPAGE	*ptr = menu->twoPlayerPage;
	bool noneAvailable = true;
	static bool spinnerPageSet[3] = {false, false, false};

	if (onTournament)
	{
		if ((!spinnerPageSet[0]) && (!Spinner[0].GetFirstSpin()) && (!Spinner[0].GetSpin()))
		{
			ptr->tourneyIcon[0]->setTexture(GetTournamentGameIcon(GetGameNumber(0)));
			ptr->tourneyText[0]->setText(GetTournamentGameName(GetGameNumber(0)));
			ptr->iconBook[0]->gotoPage("on");
			spinnerPageSet[0] = true;
		}
		else if ((spinnerPageSet[0]) && (Spinner[0].GetSpin()))
		{
			ptr->iconBook[0]->gotoPage("off");
			spinnerPageSet[0] = false;
		}
		
		if ((!spinnerPageSet[1]) && (!Spinner[1].GetFirstSpin()) && (!Spinner[1].GetSpin()))
		{
			ptr->tourneyIcon[1]->setTexture(GetTournamentGameIcon(GetGameNumber(1)));
			ptr->tourneyText[1]->setText(GetTournamentGameName(GetGameNumber(1)));
			ptr->iconBook[1]->gotoPage("on");
			spinnerPageSet[1] = true;
		}
		else if ((spinnerPageSet[1]) && (Spinner[1].GetSpin()))
		{
			ptr->iconBook[1]->gotoPage("off");
			spinnerPageSet[1] = false;
		}
		
		if ((!spinnerPageSet[2]) && (!Spinner[2].GetFirstSpin()) && (!Spinner[2].GetSpin()))
		{
			ptr->tourneyIcon[2]->setTexture(GetTournamentGameIcon(GetGameNumber(2)));
			ptr->tourneyText[2]->setText(GetTournamentGameName(GetGameNumber(2)));
			ptr->iconBook[2]->gotoPage("on");
			spinnerPageSet[2] = true;
		}
		else if ((spinnerPageSet[2]) && (Spinner[2].GetSpin()))
		{
			ptr->iconBook[2]->gotoPage("off");
			spinnerPageSet[2] = false;
		}

		if ((!Spinner[0].GetSpin() && !Spinner[1].GetSpin() && !Spinner[2].GetSpin()))
		{
			ptr->tourneyTb[0]->setSelectable(true);
			ptr->tourneyTb[0]->enable(true);

			ptr->tourneyTb[1]->setSelectable(true);
			ptr->tourneyTb[1]->enable(true);
		
			ptr->tourneyTb[2]->setSelectable(true);
			ptr->tourneyTb[2]->enable(true);

			ptr->tourneyTb[3]->setSelectable(true);
			ptr->tourneyTb[3]->enable(true);

			ptr->tourneyTb[4]->setSelectable(true);
			ptr->tourneyTb[4]->enable(true);			
		}
		else
		{
			ptr->tourneyTb[0]->setSelectable(false);
			ptr->tourneyTb[0]->enable(false);

			ptr->tourneyTb[1]->setSelectable(false);
			ptr->tourneyTb[1]->enable(false);
		
			ptr->tourneyTb[2]->setSelectable(false);
			ptr->tourneyTb[2]->enable(false);

			ptr->tourneyTb[3]->setSelectable(false);
			ptr->tourneyTb[3]->enable(false);

			ptr->tourneyTb[4]->setSelectable(false);
			ptr->tourneyTb[4]->enable(false);			
		}
	}

	// NH: Race selected
	if ((ptr->page->selectedItem == ptr->raceModeTb)&&(!twoPlayerSubMenu))
	{
		// NH: If clicked...
		if(OKBUTTON)
		{		
			menu->frontEndMenuBook->gotoPage("vrraces");
			menu->frontEndMenuBook->getCurrentPage()->selectItem(ptr->vrracesTb[0]);
			twoPlayerSubMenu = TRUE;

			SetFrontEndCameraDestination(FECAM_TWOPLAYER,FECAM_TWOPLAYER_RACE);

			return;
		}
	
		ptr->infoSubBook->gotoPage("VEHICLE_RACE");
		ptr->controlSubBook->gotoPage("RETURN");
	}

	// NH: Time Trial Selected
	else if ((ptr->page->selectedItem == ptr->timeTrialTb)&&(!twoPlayerSubMenu))
	{
		// NH: If clicked...
		if(OKBUTTON)
		{
			menu->frontEndMenuBook->gotoPage("races");
			menu->frontEndMenuBook->getCurrentPage()->selectItem(ptr->racesTb[0]);
			twoPlayerSubMenu = TRUE;

			SetFrontEndCameraDestination(FECAM_TWOPLAYER,FECAM_TWOPLAYER_TIME);

			return;
		}
		
		ptr->infoSubBook->gotoPage("RACE_MODE");
		ptr->controlSubBook->gotoPage("RETURN");
	}
	
	// NH: Destruction selected
	else if ((ptr->page->selectedItem == ptr->destructionTb)&&(!twoPlayerSubMenu))
	{
		// NH: If clicked...
		if(OKBUTTON)
		{
			menu->frontEndMenuBook->gotoPage("destruction");
			menu->frontEndMenuBook->getCurrentPage()->selectItem(ptr->destGamesTb[0]);
			twoPlayerSubMenu = TRUE;

			SetFrontEndCameraDestination(FECAM_TWOPLAYER,FECAM_TWOPLAYER_DESTRUCTION);

			return;
		}

		ptr->infoSubBook->gotoPage("DESTRUCTION");
		ptr->controlSubBook->gotoPage("RETURN");
	}
	
	// NH: Tournament selected
	else if ((ptr->page->selectedItem == ptr->tournamentTb)&&(!twoPlayerSubMenu))
	{
		// NH: If clicked...
		if(OKBUTTON)
		{
			ptr->iconBook[0]->gotoPage("off",true);
			ptr->iconBook[1]->gotoPage("off",true);
			ptr->iconBook[2]->gotoPage("off",true);
			menu->frontEndMenuBook->gotoPage("tournament");
			menu->frontEndMenuBook->getCurrentPage()->selectItem(ptr->tourneyTb[0]);
			twoPlayerSubMenu = true;			
			
//			InitTournament();
			if (!Spinner[0].GetSpin()&&!Spinner[1].GetSpin()&&!Spinner[2].GetSpin())
			{
				SetSlotState(CREATE);	
			}
			onTournament = true;

			SetFrontEndCameraDestination(FECAM_TWOPLAYER,FECAM_TWOPLAYER_TOURNAMENT);

			return;
		}

		ptr->infoSubBook->gotoPage("TOURNAMENT");
		ptr->controlSubBook->gotoPage("RETURN");
	}

	// NH: Secrets selected
	else if ((ptr->page->selectedItem == ptr->secretGamesTb)&&(!twoPlayerSubMenu))
	{		
		// NH: If clicked...
		if(OKBUTTON)
		{	
			menu->frontEndMenuBook->gotoPage("bosses");
						
			twoPlayerSubMenu = TRUE;

			return;			
		}

		ptr->infoSubBook->gotoPage("SECRETS");
		ptr->controlSubBook->gotoPage("RETURN");
	}

	if (menu->frontEndMenuBook->getCurrentPage() == menu->frontEndMenuBook->getPage("bosses"))
	{
		if ((gameStatus.globalSettings.saveInfo.stats[ConvertSceneRefToLevelScoreRef(SCENE_ZOOBOSS)].levelCompleted/* PP: REMOUT: not necessary nor terribly safe		== true*/) || (CheatActive(CHEAT_OPEN_BOSS_GAME)))
		{
			ptr->bossesTb[0]->setConcealed(false);
			noneAvailable = false;
		}
		else
		{
			ptr->bossesTb[0]->setConcealed(true);
		}
		
		if ((gameStatus.globalSettings.saveInfo.stats[ConvertSceneRefToLevelScoreRef(SCENE_CITYBOSS)].levelCompleted/* PP: REMOUT: not necessary nor terribly safe		== true*/) || (CheatActive(CHEAT_OPEN_BOSS_GAME)))
		{
			ptr->bossesTb[1]->setConcealed(false);
			noneAvailable = false;
		}
		else
		{
			ptr->bossesTb[1]->setConcealed(true);
		}

		if ((gameStatus.globalSettings.saveInfo.stats[ConvertSceneRefToLevelScoreRef(SCENE_WESTBOSS)].levelCompleted/* PP: REMOUT: not necessary nor terribly safe		== true*/) || (CheatActive(CHEAT_OPEN_BOSS_GAME)))
		{
			ptr->bossesTb[2]->setConcealed(false);
			noneAvailable = false;
		}
		else
		{
			ptr->bossesTb[2]->setConcealed(true);
		}

		if (noneAvailable)
		{
			ptr->infoSubBook->gotoPage("S_NONE_AVAILABLE");
			menu->frontEndMenuBook->getCurrentPage()->selectNone();
		}
		else if (menu->frontEndMenuBook->getCurrentPage()->selectedItem == NULL)
			menu->frontEndMenuBook->getCurrentPage()->selectItem(menu->frontEndMenuBook->getCurrentPage()->getFirstSelectableItem(true));
	}

	// NH: A twoPlayerSubMenu selected
	if (twoPlayerSubMenu)
	{
		// NH: Vehicle Race
		if (menu->frontEndMenuBook->getCurrentPage()->selectedItem == ptr->vrracesTb[0])	
		{
			if(OKBUTTON)
			{
				// PP: set number of players to 2, etc.
				setNumPlayers(2);

				BookList_closeAndFree(menu->frontEndMenuBook);
				bkGenerateEvent("scenechange","_vrdeptstr");
				return;
			}
			ptr->gameInfoSubBook[0]->gotoPage("VR_DEPTSTR");
		}
		else if (menu->frontEndMenuBook->getCurrentPage()->selectedItem == ptr->vrracesTb[1])
		{
			if(OKBUTTON)
			{
				// PP: set number of players to 2, etc.
				setNumPlayers(2);

				BookList_closeAndFree(menu->frontEndMenuBook);
				bkGenerateEvent("scenechange","_vrmuseum");													// NH: Hover Craft Race selected
				return;
			}
			ptr->gameInfoSubBook[0]->gotoPage("VR_MUSEUM");
		}
		else if (menu->frontEndMenuBook->getCurrentPage()->selectedItem == ptr->vrracesTb[2])
		{
			if(OKBUTTON)
			{
				// PP: set number of players to 2, etc.
				setNumPlayers(2);

				BookList_closeAndFree(menu->frontEndMenuBook);
				bkGenerateEvent("scenechange","_vrgrandc");
				return;
			}
			ptr->gameInfoSubBook[0]->gotoPage("VR_CANYON");
		}
		// NH: Race
		else if (menu->frontEndMenuBook->getCurrentPage()->selectedItem == ptr->racesTb[0])
		{
			if(OKBUTTON)
			{
				// PP: set number of players to 2, etc.
				setNumPlayers(2);

				BookList_closeAndFree(menu->frontEndMenuBook);
				bkGenerateEvent("scenechange","_rcicedome");
				return;
			}
			ptr->gameInfoSubBook[1]->gotoPage("TA_ICEDOME");
		}
		else if (menu->frontEndMenuBook->getCurrentPage()->selectedItem == ptr->racesTb[1])
		{
			if(OKBUTTON)
			{
				// PP: set number of players to 2, etc.
				setNumPlayers(2);

				BookList_closeAndFree(menu->frontEndMenuBook);
				bkGenerateEvent("scenechange","_rcgoldmine");
				return;
			}
			ptr->gameInfoSubBook[1]->gotoPage("TA_GOLDMINE");
		}	
		else if (menu->frontEndMenuBook->getCurrentPage()->selectedItem == ptr->racesTb[2])
		{
			if(OKBUTTON)
			{
				// PP: set number of players to 2, etc.
				setNumPlayers(2);

				BookList_closeAndFree(menu->frontEndMenuBook);
				bkGenerateEvent("scenechange","_rcsafari");
				return;
			}
			ptr->gameInfoSubBook[1]->gotoPage("TA_SAFARI");
		}	
		// TP: Destruction
		else if (menu->frontEndMenuBook->getCurrentPage()->selectedItem == ptr->destGamesTb[0])
		{
			if(OKBUTTON)
			{
				// PP: set number of players to 2, etc.
				setNumPlayers(2);

				BookList_closeAndFree(menu->frontEndMenuBook);
				bkGenerateEvent("scenechange","_deaqua");
				return;
			}
			ptr->gameInfoSubBook[2]->gotoPage("DE_AQUA");
		}	
		else if (menu->frontEndMenuBook->getCurrentPage()->selectedItem == ptr->destGamesTb[1])
		{
			if(OKBUTTON)
			{
				// PP: set number of players to 2, etc.
				setNumPlayers(2);

				BookList_closeAndFree(menu->frontEndMenuBook);
				bkGenerateEvent("scenechange","_deconstruct");
				return;
			}
			ptr->gameInfoSubBook[2]->gotoPage("DE_CONSTRUCT");
		}
		else if (menu->frontEndMenuBook->getCurrentPage()->selectedItem == ptr->destGamesTb[2])
		{
			if(OKBUTTON)
			{
				// PP: set number of players to 2, etc.
				setNumPlayers(2);

				BookList_closeAndFree(menu->frontEndMenuBook);
				bkGenerateEvent("scenechange","_detasmania");
				return;
			}
			ptr->gameInfoSubBook[2]->gotoPage("DE_TASMANIA");
		}
		// JW: Tournament
		else if (menu->frontEndMenuBook->getCurrentPage()->selectedItem == ptr->tourneyTb[0])
		{
			if((OKBUTTON) && (!Spinner[0].GetSpin()&&!Spinner[1].GetSpin()&&!Spinner[2].GetSpin()))
			{
				ProcessTournamentButton(TB_HANDLE_PULL);
				return;
			}	// JW: Handle Pull
			ptr->gameInfoSubBook[3]->gotoPage("T_GETGAMES");
		}
		else if (menu->frontEndMenuBook->getCurrentPage()->selectedItem == ptr->tourneyTb[1])
		{
			if(OKBUTTON)
			{
				ProcessTournamentButton(TB_HOLD_ONE);

				ptr->tourneyIcon[0]->selected = Spinner[0].GetHold();		
				return;
			}
			ptr->gameInfoSubBook[3]->gotoPage("T_REEL1");	// JW: Hold 1
		}	
		else if (menu->frontEndMenuBook->getCurrentPage()->selectedItem == ptr->tourneyTb[2])
		{
			if(OKBUTTON)
			{
				ProcessTournamentButton(TB_HOLD_TWO);

				ptr->tourneyIcon[1]->selected = Spinner[1].GetHold();
				return;
			}
			ptr->gameInfoSubBook[3]->gotoPage("T_REEL2");// JW: Hold 2
		}
		else if (menu->frontEndMenuBook->getCurrentPage()->selectedItem == ptr->tourneyTb[3])
		{
			if(OKBUTTON)
			{
				ProcessTournamentButton(TB_HOLD_THREE);
				
				ptr->tourneyIcon[2]->selected = Spinner[2].GetHold();
				return;
			}
			ptr->gameInfoSubBook[3]->gotoPage("T_REEL3");// JW: Hold 3
		}	
		else if (menu->frontEndMenuBook->getCurrentPage()->selectedItem == ptr->tourneyTb[4])
		{
			if(OKBUTTON)
			{
				ProcessTournamentButton(TB_COLLECT);
				return;
			}
			ptr->gameInfoSubBook[3]->gotoPage("T_PLAY");// JW: Collect
		}
		// NH: Secrets / Bosses
		else if (menu->frontEndMenuBook->getCurrentPage()->selectedItem == ptr->bossesTb[0])
		{
			if(OKBUTTON)
			{
				// PP: set number of players to 2, etc.
				setNumPlayers(2);

				BookList_closeAndFree(menu->frontEndMenuBook);
				bkGenerateEvent("scenechange","_zooboss");												// NH: Elephanmt Pong
				return;
			}
			ptr->gameInfoSubBook[4]->gotoPage("S_SELECT");
		}
		else if (menu->frontEndMenuBook->getCurrentPage()->selectedItem == ptr->bossesTb[1])
		{
			if(OKBUTTON)
			{
				// PP: set number of players to 2, etc.
				setNumPlayers(2);

				BookList_closeAndFree(menu->frontEndMenuBook);
				bkGenerateEvent("scenechange","_cityboss");												// NH: Atlas-spheres
				return;
			}			
			ptr->gameInfoSubBook[4]->gotoPage("S_SELECT");
		}
		else if (menu->frontEndMenuBook->getCurrentPage()->selectedItem == ptr->bossesTb[2])
		{
			if(OKBUTTON)
			{
				gameStatus.multiplayer.numPlayers = 2;
				gameStatus.multiplayer.versusCPU = false;

				// PP: set number of players to 2, etc.
				setNumPlayers(2);

				BookList_closeAndFree(menu->frontEndMenuBook);
				bkGenerateEvent("scenechange","_westboss");	// NH: Bummer Mann (?!?)
				return;
			}	
			ptr->gameInfoSubBook[4]->gotoPage("S_SELECT");
		}

		if(
#if BPLATFORM!=PC
		(biFindPad(1)==-1)||
#endif
		CANCELBUTTON)
		{
			if (strcmp(menu->frontEndMenuBook->getCurrentPage()->name, "tournament") == 0)
			{
//				KillTournamentPage();
				SetFrontEndCameraDestination(FECAM_TWOPLAYER_TOURNAMENT, FECAM_TWOPLAYER);
				onTournament = FALSE;
			}
			else if (strcmp(menu->frontEndMenuBook->getCurrentPage()->name, "destruction") == 0)
			{
				SetFrontEndCameraDestination(FECAM_TWOPLAYER_DESTRUCTION, FECAM_TWOPLAYER);
			}
			else if (strcmp(menu->frontEndMenuBook->getCurrentPage()->name, "races") == 0)
			{
				SetFrontEndCameraDestination(FECAM_TWOPLAYER_TIME, FECAM_TWOPLAYER);
			}
			else if (strcmp(menu->frontEndMenuBook->getCurrentPage()->name, "vrraces") == 0)
			{
				SetFrontEndCameraDestination(FECAM_TWOPLAYER_RACE, FECAM_TWOPLAYER);
			}

			menu->frontEndMenuBook->gotoPage(menuNames[MENU_TWOPLAYER]);
			twoPlayerSubMenu = FALSE;


//			PlaySample("deselect.wav", 255);

			return;
		}
	}

	UpdateTournamentPage();

	//triangle to return to main menu
	if(
#if BPLATFORM!=PC
		(biFindPad(1)==-1)||
#endif
		CANCELBUTTON)
	{
		
		// NH: Play bowhomb sound. Would be nice to be in PageFX, but not sure how to do it so this will work for now
		menu->twoPlayerPage->page->playNavSound(menu->twoPlayerPage->page->menuSounds, EMSFX_BACK);
		frontEndAnimInfo.requestState = GOING_HOME;

		menu->currentPage = MENU_MAIN;
		ChooseFrontEndAnimationPath(MENU_MAIN);

		menu->frontEndMenuBook->gotoPage(menuNames[MENU_MAIN]);
		
		SetFrontEndCameraDestination(FECAM_TWOPLAYER,FECAM_MAIN);
	}

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);			// PP: this should anti-alias the text in the book
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);// PP: false
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);

//	frontEndMenu.frontEndMenuBook->drawDebug();

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);


}

EStringIndex GetTournamentGameName(int gameNumber)
{
	EStringIndex gameName;

	switch (gameNumber)
	{
	case 0:
		gameName = STR_TP_RACE_MUSEUM;
		break;
	case 1:
		gameName = STR_TP_RACE_CANYON;
		break;
	case 2:
		gameName = STR_TP_RACE_DEPTSTR;
		break;
	case 3:
		gameName = STR_TP_DEST_AQUA;
		break;
	case 4:
		gameName = STR_TP_DEST_CONSTRUCTION;
		break;
	case 5:
		gameName = STR_TP_DEST_GHOSTTOWN;
		break;
	case 6:
		gameName = STR_TP_RACE_ICEDOME;
		break;
	case 7:
		gameName = STR_TP_RACE_GOLDMINE;
		break;
	case 8:
		gameName = STR_TP_RACE_SAFARI;
		break;
	}

	return gameName;
}

char *GetTournamentGameIcon(int gameNumber)
{
	char *iconName;

	switch (gameNumber)
	{
	case 0:
		iconName = "extras\\museum.bmp";
		break;
	case 1:
		iconName = "extras\\grandcanyon.bmp";
		break;
	case 2:
		iconName = "extras\\deptstore.bmp";
		break;
	case 3:
		iconName = "extras\\aqua.bmp";
		break;
	case 4:
		iconName = "extras\\consite.bmp";
		break;
	case 5:
		iconName = "extras\\ghostown.bmp";
		break;
	case 6:
		iconName = "extras\\icedome.bmp";
		break;
	case 7:
		iconName = "extras\\goldmine.bmp";
		break;
	case 8:
		iconName = "extras\\safari.bmp";
		break;
	}
	
	return iconName;
}