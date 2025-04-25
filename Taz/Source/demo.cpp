// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Blitz Games Ltd.
//
//      File : demo.cpp
//   Purpose : consumer demo specifc stuff
// Component : Taz
//    Author : John Weeks
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// PP: This file isn't used unless it's a consumer demo build

#include "global.h"

#include "main.h"
#include "demo.h"
#include "control.h"
#include "icon.h"
#include "files.h"
#include "languagemain.h"
#include "playerstats.h"
#include "textfx.h"
#include "music.h"
#include "checkbox.h"
#include "util.h"
#include "thwap.h"
#include "pause.h"
#include "memCard.h"
#include "timer.h"				// PP: timer functions
#include "Table.h"				// PP: Table page-item type - lets you line up items into a grid arrangement
#include "PageItemFX.h"// PP: TEMP TEST
#include "scene.h"
#include "events.h"				// PP: TEMP TEST
#include "XboxStrings.h"		// PP: Xbox versions of some strings

#if(BPLATFORM == PS2)
#include <libsceedemo.h>		// PP: SCEE Demo Disk SDK Version 1.0 Pre-release 1
#else// PP: not ps2
#include "libsceedemo.h"		// PP: SCEE Demo Disk SDK Version 1.0 Pre-release 1
#endif// PP: not ps2

// PP: on Xbox, we can interrupt the kiosk mode rolling demo (or 2nd page of outro blurb) and it'll take us back to the demo frontend
#if(BPLATFORM == XBOX)
#define INTERRUPTIBLE_ATTRACT
#endif// PP: platform

#ifdef CONSUMERDEMO

enum	DEMOMENUSTATES
{
	TITLE_SCREEN,
	SINGLE_PLAYER,
	MULTI_PLAYER,
	CONTROLLER_INFO,
	PITCH_ONE,
	PITCH_TWO,
	ATTRACT,
	EXIT,
};

// PP: time since last pad input
// PP: We can't use controller1.idleTime.
float			demoIdleTime;

//bool		attractInterrupt;	// PP: flag to interrupt the attract cycle, eg. on controller insertion (control.cpp)

DEMOMENUSTATES DemoMenuState, DemoLastState;

BOOK	*demoMenuBook;
BOOK	*demoMenuSubBook;		// PP: sub-book within demoMenuBook (holds the single-player & multi-player icons)
BOOK	*demoMenuSubBook2;		// PP: sub-book within demoMenuBook (holds the different outro blurb pages)
BOOK	*demoInfoBook;

TEXTBOX	*sngText, *mltText;

#if(BPLATFORM == PC)
TEXTBOX *exitText;
#endif // NH: #if(BPLATFORM == PC)

SPLASHSCREEN	splashToDraw, titleSplash, menuSplash, contSplash, splash;

TBPackageIndex	*demoPak;

void DoDemoMenu()
{
	UpdateDemoMenu();
	DrawDemoMenu();
}

char DoDemoQuit()
{
	if (UpdateDemoQuit())	return 0;

	DrawDemoQuit();
	return 1;	// JW: Keep The Game Running Till I Tell it to quit
}


void CreateDemoMenu()
{
	PAGE	*page, *subPage;
	BOOK	*subBook;
	TABLE	*table;
	ICON	*icon;

	TEXTBOX	*text;

#if(BPLATFORM == XBOX)
#define DEMO_FOOTER_LEFT		-0.47f		// PP: left position of demo footer text (to line up with the bar on the splashes)
#define DEMO_FOOTER_BOTTOM		-0.47f		// PP: bottom position of demo footer text (to line up with the bar on the splashes)
#else// PP: ps2
#define DEMO_FOOTER_LEFT		-0.44f		// PP: left position of demo footer text (to line up with the bar on the splashes)
#define DEMO_FOOTER_BOTTOM		-0.44f		// PP: bottom position of demo footer text (to line up with the bar on the splashes)
#endif// PP: ps2

#define PS2_OLD_SAFE_AREA		0.95f		// PP: the safe area that was in effect on PS2 when these books were being designed pre-BKFLAG_SAFEAREA
#define XBOX_OLD_SAFE_AREA		0.85f		// PP: the safe area that was in effect on Xbox when these books were being designed pre-BKFLAG_SAFEAREA

#if(BPLATFORM == PC)
#define DEMO_SELECTED_ICON_SCALE	2.0f	// NH: Scale factor for the selected icon on the PC
#else// NH: PC
#define DEMO_SELECTED_ICON_SCALE	2.15f	// NH: Scale factor for the selected icon on platforms other than PC
#endif// NH: PC

	// PP: ********************************************************************************
	// PP: TODO: MAKE A NICE IN-GAME GRAPHICAL BOOK-EDITOR FOR OCCASIONS SUCH AS THESE!!!!!
	// PP: ********************************************************************************

	demoMenuBook = new BOOK();

	// PP: NEW: as with any book that has to line up with a splash screen, we've got to ignore the 'safe area' otherwise things'll shift about between platforms
	levelBook->flags &= ~BKFLAG_SAFEAREA;

	page = demoMenuBook->addPage("first page");

#if(BPLATFORM==PC)
	page->insertFreeItem(text = new TEXTBOX(STR_PRESS_JUMP_TO_CONTINUE), RECTANGLE(DEMO_FOOTER_LEFT, 0.5f, DEMO_FOOTER_BOTTOM, 0.5f))
		->setEffect(&PIE_flip);
#else
	page->insertFreeItem(text = new TEXTBOX(STR_PRESS_START_TO_CONTINUE), RECTANGLE(DEMO_FOOTER_LEFT, 0.5f, DEMO_FOOTER_BOTTOM, 0.5f))
		->setEffect(&PIE_flip);
#endif // NH: #if(BPLATFORM!=PC)

	text->setXAlign(PIFLAG_XALIGN_LEFT);
	text->setYAlign(PIFLAG_YALIGN_BOTTOM);

	page = demoMenuBook->addPage("single menu");

	page->insertItem(demoMenuSubBook=subBook=new BOOK(true));

	subPage = subBook->addPage("single menu");

#if(BPLATFORM == PC)
	subPage->insertFreeItem(sngText = new TEXTBOX(STR_DEMO_SINGLE_PLAYER), RECTANGLE(0.15f, 0.45f, 0.32f, 0.45f));
	subPage->insertFreeItem(mltText = new TEXTBOX(STR_DEMO_TWO_PLAYER), RECTANGLE(0.18f, 0.42f, -0.01f, 0.115f));
	subPage->insertFreeItem(exitText = new TEXTBOX(STR_EXIT_EXITGAME), RECTANGLE(0.18f, 0.42f, -0.42f, -0.17f));

	exitText->setColour(COLOUR(72,75,102,128),COLOUR(54,58,91,128));
	exitText->setWrap(true);
	exitText->setFontSize(0.7f);
#else
	subPage->insertFreeItem(sngText = new TEXTBOX(STR_DEMO_SINGLE_PLAYER), RECTANGLE(0.15f, 0.45f, 0.32f, 0.45f));
	subPage->insertFreeItem(mltText = new TEXTBOX(STR_DEMO_TWO_PLAYER), RECTANGLE(0.18f, 0.42f, -0.14f, 0.015f));
#endif

	sngText->setWrap(true);

	mltText->setColour(COLOUR(72,75,102,128),COLOUR(54,58,91,128));
	mltText->setWrap(true);
	mltText->setFontSize(0.7f);

#if(BPLATFORM == PC)
	subPage->insertFreeItem(icon = new ICON("icons\\multiplayergrey.bmp"), RECTANGLE(0.15f, 0.45f, -0.04f, -0.02f));
#else
	subPage->insertFreeItem(icon = new ICON("icons\\multiplayergrey.bmp"), RECTANGLE(0.15f, 0.45f, -0.16f, -0.14f));
#endif

	icon->setScale(1.5f);
	icon->tilt = false;

	subPage->insertFreeItem(icon = new ICON("icons\\singleplayercolor.bmp"), RECTANGLE(0.15f, 0.45f, 0.26f, 0.28f));
	icon->setScale(DEMO_SELECTED_ICON_SCALE);
	icon->tilt = false;

	///////

	subPage = subBook->addPage("multi menu");

#if(BPLATFORM == PC)
	subPage->insertFreeItem(sngText = new TEXTBOX(STR_DEMO_SINGLE_PLAYER), RECTANGLE(0.15f, 0.45f, 0.28f, 0.415f));
	//subPage->insertFreeItem(mltText = new TEXTBOX(STR_DEMO_TWO_PLAYER), RECTANGLE(0.12f, 0.48f, -0.24f, 0.115f));
	subPage->insertFreeItem(mltText = new TEXTBOX(STR_DEMO_TWO_PLAYER), RECTANGLE(0.12f, 0.48f, -0.09f, 0.235f));
	subPage->insertFreeItem(exitText = new TEXTBOX(STR_EXIT_EXITGAME), RECTANGLE(0.18f, 0.42f, -0.42f, -0.17f));

	exitText->setColour(COLOUR(72,75,102,128),COLOUR(54,58,91,128));
	exitText->setWrap(true);
	exitText->setFontSize(0.7f);
#else
	subPage->insertFreeItem(sngText = new TEXTBOX(STR_DEMO_SINGLE_PLAYER), RECTANGLE(0.15f, 0.45f, 0.28f, 0.415f));
	subPage->insertFreeItem(mltText = new TEXTBOX(STR_DEMO_TWO_PLAYER), RECTANGLE(0.12f, 0.48f, -0.1f, 0.05f));
#endif // NH: #if(BPLATFORM == PC)

	sngText->setColour(COLOUR(72,75,102,128),COLOUR(54,58,91,128));
	sngText->setWrap(true);
	sngText->setFontSize(0.7f);

	mltText->setWrap(true);

#if(BPLATFORM == PC)
	subPage->insertFreeItem(icon = new ICON("icons\\multiplayercolor.bmp"), RECTANGLE(0.15f, 0.45f, -0.04f, -0.02f));
#else
	subPage->insertFreeItem(icon = new ICON("icons\\multiplayercolor.bmp"), RECTANGLE(0.15f, 0.45f, -0.16f, -0.14f));
#endif // NH: #if(BPLATFORM == PC)

	icon->setScale(DEMO_SELECTED_ICON_SCALE);
	icon->tilt = false;

	subPage->insertFreeItem(icon = new ICON("icons\\singleplayergrey.bmp"), RECTANGLE(0.15f, 0.45f, 0.26f, 0.28f));
	icon->setScale(1.5f);
	icon->tilt = false;

	subBook->setEffect(&PIE_leftSlide, PIEMODE_ENTER);
	subBook->setEffect(&PIE_rightSlide, PIEMODE_EXIT);

	///////

#if(BPLATFORM == PC)
	subPage = subBook->addPage("exit menu");

	subPage->insertFreeItem(sngText = new TEXTBOX(STR_DEMO_SINGLE_PLAYER), RECTANGLE(0.15f, 0.45f, 0.28f, 0.415f));
	//subPage->insertFreeItem(mltText = new TEXTBOX(STR_DEMO_TWO_PLAYER), RECTANGLE(0.18f, 0.42f, -0.14f, 0.015f));
	subPage->insertFreeItem(mltText = new TEXTBOX(STR_DEMO_TWO_PLAYER), RECTANGLE(0.18f, 0.42f, -0.01f, 0.115f));
	subPage->insertFreeItem(exitText = new TEXTBOX(STR_EXIT_EXITGAME), RECTANGLE(0.18f, 0.42f, -0.42f, -0.17f));

	sngText->setColour(COLOUR(72,75,102,128),COLOUR(54,58,91,128));
	sngText->setWrap(true);
	sngText->setFontSize(0.7f);

	mltText->setColour(COLOUR(72,75,102,128),COLOUR(54,58,91,128));
	mltText->setWrap(true);
	mltText->setFontSize(0.7f);
	
	exitText->setWrap(true);

	//subPage->insertFreeItem(icon = new ICON("icons\\multiplayergrey.bmp"), RECTANGLE(0.15f, 0.45f, -0.16f, -0.14f));
	subPage->insertFreeItem(icon = new ICON("icons\\multiplayergrey.bmp"), RECTANGLE(0.15f, 0.45f, -0.04f, -0.02f));
	icon->setScale(1.5f);
	icon->tilt = false;

	subPage->insertFreeItem(icon = new ICON("icons\\singleplayergrey.bmp"), RECTANGLE(0.15f, 0.45f, 0.26f, 0.28f));
	icon->setScale(1.5f);
	icon->tilt = false;

	subBook->setEffect(&PIE_leftSlide, PIEMODE_ENTER);
	subBook->setEffect(&PIE_rightSlide, PIEMODE_EXIT);
#endif // NH: #if(BPLATFORM == PC)

	/////////////

	page = demoMenuBook->addPage("controller");
	
	// PP: I think the safest way to get this looking right on Xbox is to create the whole page differently between the 2 platforms
	// PP: I don't feel guilty about it either - the positions of the button lines on the splash are different...

#if(BPLATFORM == PS2)

	// PP: DIRTY FIX BUT THE BEST THING FOR IT AT THE MOMENT: specify the safe area that was in effect when the book was being designed pre-BKFLAG_SAFEAREA
	demoMenuBook->setOldSafeArea(PS2_OLD_SAFE_AREA, PS2_OLD_SAFE_AREA);

	page->insertFreeItem(subPage = new PAGE("forward-back"), RECTANGLE(DEMO_FOOTER_LEFT, 0.5f, DEMO_FOOTER_BOTTOM, 0.5f));
		
	subPage->insertItem(text = new TEXTBOX(STR_PRESS_START_TO_CONTINUE))->setEffect(&PIE_flip);
	text->setXPadding(false);
	text->setAlignScale(1.05f);
	text->setXAlign(PIFLAG_XALIGN_LEFT);
	text->setYAlign(PIFLAG_YALIGN_BOTTOM);
	
	subPage->insertItem(text = new TEXTBOX(STR_PRESS_Y_TO_GO_BACK), false)->setEffect(&PIE_flip);
	text->setXAlign(PIFLAG_XALIGN_LEFT);
	text->setYAlign(PIFLAG_YALIGN_BOTTOM);

	page->insertFreeItem(text = new TEXTBOX(STR_DEMO_CONTROL_L1), RECTANGLE(-0.375f, 0.4f, 0.33f, 0.38f));
	text->setXAlign(PIFLAG_XALIGN_LEFT);
	text->setScale(0.65f);

	page->insertFreeItem(text = new TEXTBOX(STR_DEMO_CONTROL_L2), RECTANGLE(-0.375f, 0.4f, 0.43f, 0.48f));
	text->setXAlign(PIFLAG_XALIGN_LEFT);
	text->setScale(0.65f);

	page->insertFreeItem(text = new TEXTBOX(STR_DEMO_CONTROL_R1), RECTANGLE(-0.095f, 0.4f, 0.28f, 0.33f));
	text->setXAlign(PIFLAG_XALIGN_LEFT);
	text->setScale(0.65f);

	page->insertFreeItem(text = new TEXTBOX(STR_DEMO_CONTROL_R2), RECTANGLE(-0.095f, 0.4f, 0.38f, 0.43f));
	text->setXAlign(PIFLAG_XALIGN_LEFT);
	text->setScale(0.65f);

	page->insertFreeItem(text = new TEXTBOX(STR_DEMO_CONTROL_LEFT_ANALOG), RECTANGLE(-0.5f, -0.3f, -0.118f, -0.118f));
	text->setXAlign(PIFLAG_XALIGN_RIGHT);
	text->setScale(0.65f);

	page->insertFreeItem(text = new TEXTBOX(STR_DEMO_CONTROL_RIGHT_ANALOG), RECTANGLE(-0.13f, -0.13f, -0.11f, -0.11f));
	text->setXAlign(PIFLAG_XALIGN_LEFT);
	text->setScale(0.65f);

	page->insertFreeItem(text = new TEXTBOX(STR_DEMO_CONTROL_SELECT), RECTANGLE(-0.5f, -0.23f, -0.16f, -0.16f));
	text->setXAlign(PIFLAG_XALIGN_RIGHT);
	text->setScale(0.65f);

	page->insertFreeItem(text = new TEXTBOX(STR_DEMO_CONTROL_START), RECTANGLE(-0.2f, 0.5f, -0.16f, -0.16f));
	text->setXAlign(PIFLAG_XALIGN_LEFT);
	text->setScale(0.65f);

	page->insertFreeItem(text = new TEXTBOX(STR_DEMO_CONTROL_SQUARE), RECTANGLE(0.025f, 0.075f, 0.205f, 0.205f));
	text->setScale(0.6f);
	text->setXAlign(PIFLAG_XALIGN_LEFT);

	page->insertFreeItem(text = new TEXTBOX(STR_DEMO_CONTROL_TRIANGLE), RECTANGLE(0.025f, 0.075f, 0.153f, 0.153f));
	text->setScale(0.6f);
	text->setXAlign(PIFLAG_XALIGN_LEFT);

	page->insertFreeItem(text = new TEXTBOX(STR_DEMO_CONTROL_CIRCLE), RECTANGLE(0.025f, 0.075f, 0.101f, 0.101f));
	text->setScale(0.6f);
	text->setXAlign(PIFLAG_XALIGN_LEFT);
	
	page->insertFreeItem(text = new TEXTBOX(STR_DEMO_CONTROL_CROSS), RECTANGLE(0.025f, 0.075f, 0.049f, 0.049f));
	text->setScale(0.6f);
	text->setXAlign(PIFLAG_XALIGN_LEFT);

	page->insertFreeItem(subPage = new PAGE("control text"), RECTANGLE(0.1f, 0.45f, -0.45f, 0.45f));

#elif(BPLATFORM == XBOX)

	// PP: DIRTY FIX BUT THE BEST THING FOR IT AT THE MOMENT: specify the safe area that was in effect when the book was being designed pre-BKFLAG_SAFEAREA
	demoMenuBook->setOldSafeArea(XBOX_OLD_SAFE_AREA, XBOX_OLD_SAFE_AREA);

#define CONTROLLER_SCREEN_FONT_SIZE		0.565f

	page->insertFreeItem(subPage = new PAGE("forward-back"), RECTANGLE(DEMO_FOOTER_LEFT, 0.5f, DEMO_FOOTER_BOTTOM, 0.5f));
		
	subPage->insertItem(text = new TEXTBOX(STR_PRESS_START_TO_CONTINUE))->setEffect(&PIE_flip);
	text->setXPadding(false);
	text->setAlignScale(1.05f);
	text->setXAlign(PIFLAG_XALIGN_LEFT);
	text->setYAlign(PIFLAG_YALIGN_BOTTOM);
	
	subPage->insertItem(text = new TEXTBOX(STR_PRESS_Y_TO_GO_BACK), false)->setEffect(&PIE_flip);
	text->setXAlign(PIFLAG_XALIGN_LEFT);
	text->setYAlign(PIFLAG_YALIGN_BOTTOM);

/*	page->insertFreeItem(table = new TABLE(), RECTANGLE(-0.45f, 0.3f, 0.34f, 0.47f));

	table->insertItem(text = new TEXTBOX(STR_DEMO_CONTROL_L1));
	text->setXAlign(PIFLAG_XALIGN_LEFT);
	text->setFontSize(CONTROLLER_SCREEN_FONT_SIZE);

	table->insertItem(text = new TEXTBOX(STR_DEMO_CONTROL_R1));
	text->setXAlign(PIFLAG_XALIGN_LEFT);
	text->setFontSize(CONTROLLER_SCREEN_FONT_SIZE);

	table->insertItem(text = new TEXTBOX(STR_DEMO_CONTROL_L2), false);
	text->setYAlign(PIFLAG_YALIGN_BOTTOM);
	text->setXAlign(PIFLAG_XALIGN_LEFT);
	text->setFontSize(CONTROLLER_SCREEN_FONT_SIZE);

	table->insertItem(text = new TEXTBOX(STR_DEMO_CONTROL_R2), false);
	text->setYAlign(PIFLAG_YALIGN_BOTTOM);
	text->setXAlign(PIFLAG_XALIGN_LEFT);
	text->setFontSize(CONTROLLER_SCREEN_FONT_SIZE);*/

/*
	page->insertFreeItem(text = new TEXTBOX(STR_DEMO_CONTROL_L2), RECTANGLE(-0.395f, 0.3f, 0.33f, 0.455f));
	text->setYAlign(PIFLAG_YALIGN_TOP);
	text->setXAlign(PIFLAG_XALIGN_LEFT);
	text->setFontSize(CONTROLLER_SCREEN_FONT_SIZE);

	page->insertFreeItem(text = new TEXTBOX(STR_DEMO_CONTROL_R2), RECTANGLE(-0.095f, 0.3f, 0.33f, 0.4f));
	text->setYAlign(PIFLAG_YALIGN_TOP);
	text->setXAlign(PIFLAG_XALIGN_LEFT);
	text->setFontSize(CONTROLLER_SCREEN_FONT_SIZE);
	/*/

	page->insertFreeItem(text = new TEXTBOX(STR_DEMO_CONTROL_L2), RECTANGLE(-0.4f, -0.15f, 0.5f, 0.42f));
//	text->setBackgroundColour(RED);
	text->setWrap(true);
	text->setYAlign(PIFLAG_YALIGN_TOP);
	text->setXAlign(PIFLAG_XALIGN_LEFT);
	text->setFontSize(CONTROLLER_SCREEN_FONT_SIZE);

	page->insertFreeItem(text = new TEXTBOX(STR_DEMO_CONTROL_R2), RECTANGLE(-0.095f, 0.5f, 0.5f, 0.42f));
//	text->setBackgroundColour(BLUE);
	text->setYAlign(PIFLAG_YALIGN_TOP);
	text->setXAlign(PIFLAG_XALIGN_LEFT);
	text->setFontSize(CONTROLLER_SCREEN_FONT_SIZE);

	//*/


// PP: ~~~~~~~~~~~~~

	page->insertFreeItem(text = new TEXTBOX(STR_DEMO_CONTROL_LEFT_ANALOG), RECTANGLE(-0.47f, 0.5f, -0.11f, -0.11f));
	text->setXAlign(PIFLAG_XALIGN_LEFT);
	text->setFontSize(CONTROLLER_SCREEN_FONT_SIZE);

	page->insertFreeItem(text = new TEXTBOX(STR_DEMO_CONTROL_RIGHT_ANALOG), RECTANGLE(-0.045f, -0.045f, -0.09f, -0.09f));
	text->setXAlign(PIFLAG_XALIGN_LEFT);
	text->setFontSize(CONTROLLER_SCREEN_FONT_SIZE);

#define STARTSELECT_Y	-0.185f

#define SELECT_X		-0.26f

#define START_X			-0.21f

	page->insertFreeItem(text = new TEXTBOX(STR_DEMO_CONTROL_SELECT), RECTANGLE(SELECT_X, SELECT_X, STARTSELECT_Y, STARTSELECT_Y));
	text->setXAlign(PIFLAG_XALIGN_RIGHT);
	text->setFontSize(CONTROLLER_SCREEN_FONT_SIZE);

	page->insertFreeItem(text = new TEXTBOX(STR_DEMO_CONTROL_START), RECTANGLE(START_X, START_X, STARTSELECT_Y, STARTSELECT_Y));
	text->setXAlign(PIFLAG_XALIGN_LEFT);
	text->setFontSize(CONTROLLER_SCREEN_FONT_SIZE);

// PP: ~~~~~~~~~~~~~


	page->insertFreeItem(text = new TEXTBOX(STR_DEMO_CONTROL_L1), RECTANGLE(-0.05f, 0.5f, -0.5f, 0.345f))// PP: white
		->textbox()->setFontSize(CONTROLLER_SCREEN_FONT_SIZE)
		->setXAlign(PIFLAG_XALIGN_LEFT)
		->setYAlign(PIFLAG_YALIGN_TOP);
	
	page->insertFreeItem(text = new TEXTBOX(STR_DEMO_CONTROL_R1), RECTANGLE(-0.02f, 0.5f, -0.5f, 0.29f))// PP: black
		->textbox()->setFontSize(CONTROLLER_SCREEN_FONT_SIZE)
		->setXAlign(PIFLAG_XALIGN_LEFT)
		->setYAlign(PIFLAG_YALIGN_TOP);

// PP: ~~~~~~~~~~~~~

	// PP: page->insertFreeItem(subPage=new PAGE(), RECTANGLE(0.075f, 0.5f, -0.5f, 0.22475f));
	page->insertFreeItem(subPage=new PAGE(), RECTANGLE(0.025f, 0.5f, -0.5f, 0.22475f));// PP: You awkward German pig-dogs!

	subPage->insertItem(text = new TEXTBOX(STR_DEMO_CONTROL_TRIANGLE));
	text->setYPadding(false);
	text->setFontSize(CONTROLLER_SCREEN_FONT_SIZE);
	text->setXAlign(PIFLAG_XALIGN_LEFT);

	subPage->insertItem(text = new TEXTBOX(STR_DEMO_CONTROL_CIRCLE));
	text->setYPadding(false);
	text->setFontSize(CONTROLLER_SCREEN_FONT_SIZE);
	text->setXAlign(PIFLAG_XALIGN_LEFT);

	subPage->insertItem(text = new TEXTBOX(STR_DEMO_CONTROL_SQUARE));
	text->setYPadding(false);
	text->setFontSize(CONTROLLER_SCREEN_FONT_SIZE);
	text->setXAlign(PIFLAG_XALIGN_LEFT);
	
	subPage->insertItem(text = new TEXTBOX(STR_DEMO_CONTROL_CROSS));
	text->setFontSize(CONTROLLER_SCREEN_FONT_SIZE);
	text->setXAlign(PIFLAG_XALIGN_LEFT);
	text->setYAlign(PIFLAG_YALIGN_TOP);

// PP: ~~~~~~~~~~~~

	page->insertFreeItem(subPage = new PAGE("control text"), RECTANGLE(0.1f, 0.45f, -0.45f, 0.45f));

#endif// PP: platform

	demoMenuBook->open();

	
	demoInfoBook = new BOOK();

	// PP: NEW: as with any book that has to line up with a splash screen, we've got to ignore the 'safe area' otherwise things'll shift about between platforms
	demoInfoBook->flags &= ~BKFLAG_SAFEAREA;

#if(BPLATFORM == XBOX)
	// PP: DIRTY FIX BUT THE BEST THING FOR IT AT THE MOMENT: specify the safe area that was in effect when the book was being designed pre-BKFLAG_SAFEAREA
	demoInfoBook->setOldSafeArea(XBOX_OLD_SAFE_AREA, XBOX_OLD_SAFE_AREA);
#else// PP: ps2
		// PP: DIRTY FIX BUT THE BEST THING FOR IT AT THE MOMENT: specify the safe area that was in effect when the book was being designed pre-BKFLAG_SAFEAREA
	demoInfoBook->setOldSafeArea(PS2_OLD_SAFE_AREA, PS2_OLD_SAFE_AREA);
#endif// PP: ps2

	demoInfoBook->setBookRect(RECTANGLE(DEMO_FOOTER_LEFT, 0.5f, DEMO_FOOTER_BOTTOM, 0.5f));

	page = demoInfoBook->addPage("single player");
	
	page->insertItem(text = new TEXTBOX());

	text->sprintf8("%s ", "@(cross)");
	text->setXPadding(false);
	text->setXAlign(PIFLAG_XALIGN_LEFT);
	text->setYAlign(PIFLAG_YALIGN_BOTTOM);
	text->setEffect(&PIE_flip);

	page->insertItem(STR_DEMO_DEPTSTORE_POSTER_HEADING, false)
		->setXAlign(PIFLAG_XALIGN_LEFT) 
		->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setEffect(&PIE_flip);

	page = demoInfoBook->addPage("multiplayer");

	page->insertItem(text = new TEXTBOX());

	text->sprintf8("%s ", "@(cross)");
	text->setXPadding(false);
	text->setXAlign(PIFLAG_XALIGN_LEFT);
	text->setYAlign(PIFLAG_YALIGN_BOTTOM);
	text->setEffect(&PIE_flip);

	page->insertItem(STR_DEMO_DEPTSTORE_TROLLEY_HEADING, false)
		->setXAlign(PIFLAG_XALIGN_LEFT)
		->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setEffect(&PIE_flip);

#if(BPLATFORM == PC)
	page = demoInfoBook->addPage("exit");
	page->insertItem(STR_EXIT_EXITGAME)
		->setXAlign(PIFLAG_XALIGN_LEFT)
		->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setEffect(&PIE_flip);
#endif // NH: #if(BPLATFORM == PC)

	page = demoInfoBook->addPage("multiplayer unavailable");
	page->insertItem(text = new TEXTBOX(STR_DEMO_TWOPLAYER_CONTROLLER_MESSAGE))
		->setXAlign(PIFLAG_XALIGN_LEFT)
		->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setEffect(&PIE_flip);
		text->setWrap(true);

	// PP: "WARNING! WARNING! DANGER! DANGER!"
	{
		text->setVGrad(false);
		text->setColour(COLOUR(128,0,0,128));
		text->setEffect(&PIE_flash, PIEMODE_ON);// PP: TODO: incorporate colour info into the PIE_FLASH effect type?
	}

	demoInfoBook->gotoPage("single player");

	camera[0].fade.StartFadeUp(FADETYPE_NORMAL,0.5f);

	LoadSplashScreen(&titleSplash, "demo00.bmp", "splshscr");
	LoadSplashScreen(&menuSplash, "demo01.bmp", "splshscr");
	LoadSplashScreen(&contSplash, "demo02.bmp", "splshscr");

	splashToDraw = titleSplash;

	// PP: REMOUT: can't use this	controller1.idleTime = 0.0f;
	demoIdleTime=0.0f;

	// PP: stop all sound effects
	StopAllChannels();

	// PP: stop all pad vibration
	FlushRumbleQueue(&controller1);
	FlushRumbleQueue(&controller2);

	musicSystem.ChooseMusic(SCENE_FRONTEND,MUSICTYPE_MAIN,MUSICSTATE_START);

#if BPLATFORM == PC
    
	ShowCursor(false);	// NH: Just to make sure.  This little rascal kept on popping it's furry little head up. 
	InitMouse(&mouse);
	mouse.active = true;
#endif
}

void CreateDemoQuit()
{
	PAGE		*page;
	PAGE		*subBookPage;		// PP: ptr for sub-book pages
	PAGE		*featuresSubPage;	// PP: sub-page within sub-book, containing a list of features
	PAGE		*rowSubPage;		// PP: sub-page used to group each line of text with its bullet point
	ICON		*icon;
	TEXTBOX		*text;

	// PP: bit of a bodge; should really do this when the demo ends
	// PP: TEMP TEST
	BookList_closeAll();
	
	demoMenuBook = new BOOK();

	demoMenuBook->setBookRect(RECTANGLE(-0.07f, 0.5f, -0.4f, 0.5f));// PP: 

	page = demoMenuBook->addPage();

#define	DEMOSALESPITCH(_page, _string,_bullet) \
{ \
		(_page)->insertItem(rowSubPage = new PAGE()); \
		\
		if (_bullet) \
		{ \
			rowSubPage->insertItem(icon = new ICON("redbutton.bmp")); \
			\
			icon->setXPadding(false); \
			icon->setYAlign(PIFLAG_YALIGN_TOP); \
			icon->setAlignScale(0.35f); \
			icon->setDrawScale(0.25f); \
			icon->tilt=false;/* PP: */ \
		} \
		\
		rowSubPage->insertItem(text = new TEXTBOX(_string), false); \
		text->setFontSize(0.8f); \
		text->setWrap(true); \
		text->setYAlign(PIFLAG_YALIGN_TOP);/* PP: */ \
		text->setXAlign(PIFLAG_XALIGN_LEFT); \
} 

	// PP: header
	page->insertItem(STR_DEMO_MARKETING_HEADER)
		->textbox()
		->setStyle(TS_smallTitle)
		->setFontSize(0.8f)
		->setWrap(true);

	page->insertItem(demoMenuSubBook2 = new BOOK(true));// PP: demoMenuSubBook should have been created in CreateDemoMenu

	demoMenuSubBook2->setName("demoMenuSubBook2");

	{
		subBookPage = demoMenuSubBook2->addPage("first page");

		subBookPage->insertItem(featuresSubPage=new PAGE);
		featuresSubPage->setVSlack(false);// PP: remove vertical slack from the page
	
		if (videoMode.flags & BDISPLAYFLAG_NTSC)
		{
			DEMOSALESPITCH(featuresSubPage, STR_NTSC_DEMO_MARKETING_1,1);
			DEMOSALESPITCH(featuresSubPage, STR_NTSC_DEMO_MARKETING_2,1);
			DEMOSALESPITCH(featuresSubPage, STR_NTSC_DEMO_MARKETING_3,1);
			DEMOSALESPITCH(featuresSubPage, STR_NTSC_DEMO_MARKETING_4,1);
			DEMOSALESPITCH(featuresSubPage, STR_NTSC_DEMO_MARKETING_5,1);
		}
		else
		{
			DEMOSALESPITCH(featuresSubPage, STR_DEMO_MARKETING_1,1);
			DEMOSALESPITCH(featuresSubPage, STR_DEMO_MARKETING_2,1);
			DEMOSALESPITCH(featuresSubPage, STR_DEMO_MARKETING_3,1);
			DEMOSALESPITCH(featuresSubPage, STR_DEMO_MARKETING_4,1);
			DEMOSALESPITCH(featuresSubPage, STR_DEMO_MARKETING_5,1);
		}

		subBookPage = demoMenuSubBook2->addPage("second page");
		
		subBookPage->insertItem(featuresSubPage=new PAGE);
		featuresSubPage->setVSlack(false);// PP: remove vertical slack from the page

		if (videoMode.flags & BDISPLAYFLAG_NTSC)
		{
			DEMOSALESPITCH(featuresSubPage, STR_NTSC_DEMO_MARKETING_6,1);
			DEMOSALESPITCH(featuresSubPage, STR_NTSC_DEMO_MARKETING_7,1);
			DEMOSALESPITCH(featuresSubPage, STR_NTSC_DEMO_MARKETING_8,1);
			DEMOSALESPITCH(featuresSubPage, STR_NTSC_DEMO_MARKETING_9,1);
		}
		else
		{
			DEMOSALESPITCH(featuresSubPage, STR_DEMO_MARKETING_6,1);
			DEMOSALESPITCH(featuresSubPage, STR_DEMO_MARKETING_7,1);
			DEMOSALESPITCH(featuresSubPage, STR_DEMO_MARKETING_8,1);
			DEMOSALESPITCH(featuresSubPage, STR_DEMO_MARKETING_9,1);
			// PP: REMOUT: mmm, I disagree		DEMOSALESPITCH(featuresSubPage, STR_DEMO_MARKETING_10,1);
		}

		// PP: release date
		if (videoMode.flags & BDISPLAYFLAG_NTSC)
			subBookPage->insertItem(text = new TEXTBOX(STR_NTSC_DEMO_MARKETING_10));
		else
			subBookPage->insertItem(text = new TEXTBOX(STR_DEMO_MARKETING_10));

		// JW: text->textbox();
		text->setStyle(TS_smallTitle);
		text->setFontSize(0.8f);
		text->setWrap(true);
	}

	// PP: NO CONTROL PROMPT IN KIOSK MODE
	if(CONSUMER_DEMO_PLAYABLE)
	{
		// PP: control prompt
		page->insertItem(STR_DIALOGUE_CROSSOK)
			->textbox()
			->setStyle(TS_control);
	}
	else
	{
		// PP: OOF! I hate doing this
		// PP: But, yknow...
		page->addRow();
	}

	demoMenuBook->open();

	camera[0].fade.StartFadeUp(FADETYPE_NORMAL,0.5f);

	LoadSplashScreen(&splash, "demo05.bmp", "splshscr");

	// PP: REMOUT: can't use this	controller1.idleTime = 0.0f;
	demoIdleTime=0.0f;

	TimeFX_setGlobalRate(1.0f);
	VFX_motionBlur(0.0f);

	ClosePlayerStats(&gameStatus.player[0]);

	// PP: stop all sound effects
	StopAllChannels();

	// PP: stop all pad vibration
	FlushRumbleQueue(&controller1);
	FlushRumbleQueue(&controller2);

	// PP: hmm, looks like we have to stop the current music first - is that right?
	// TP: Choose music will automaticaly fade out and stop the old track musicSystem.ChooseMusic(SCENE_DEPTSTR,MUSICTYPE_ALL,MUSICSTATE_STOP);

	// PP: play frontend music
	musicSystem.ChooseMusic(SCENE_FRONTEND,MUSICTYPE_MAIN,MUSICSTATE_START);
}

void UpdateDemoMenu()
{
	if (gameStatus.currentState != GAMESTATE_DEMOMENU)
		return;

// PP: REMOUT: Now handled by the Book List				demoMenuBook->update();

// PP: REMOUT: OOD	checkForMissingControllers(true);

	if (DemoMenuState == ATTRACT)
		return;

	// PP: REMOUT: can't use this	if (controller1.idleTime > to_inactive)
	if(demoIdleTime > to_inactive)
	{
		bkPrintf("demoMode set to Disk Play in Demo Menu timeout");

		// PP: reset controller bindings
		padCheckMode=PADCHECK_ALL;
		bindControllersToDefaultPorts();
		
		gameStatus.demoMode = 1;

		gameStatus.multiplayer.versusCPU = true;

		/*if (bmRand()%2)
		{
			gameStatus.multiplayer.numPlayers = 2;
			gameStatus.multiplayer.versusCPU = false;
		
			bkGenerateEvent("scenechange","_vrdeptstr");
		}
		else*/
 			bkGenerateEvent("scenechange","_deptstr");
		
		demoMenuBook->close();

		camera[0].fade.StartFadeDown(FADETYPE_NORMAL, 0.5f);
		camera[0].fade.SetFadeDownFinishedCallback(StartDemoGameFadeDownCallback);

		DemoMenuState = ATTRACT;

		return;
	}

	switch (DemoMenuState)
	{
	case TITLE_SCREEN:	
		{
			bool	start;

#if(BPLATFORM==PC)
			start=(controller1.crossDebounceChannel->value || controller1.menuSelectDebounceChannel->value || controller1.mb1DebounceChannel->value);
			
#else// PP: !pc

#ifdef ALLOW_CONTROLLERS_IN_ANY_PORTS

			// PP: check for the start button being pressed on any pad;
			// PP: the first pad to press start will become controller1
			if(start=checkForStartAndBind(controller1))
			{
				padCheckMode=PADCHECK_NORMAL;
			}

#else// PP: ndef ALLOW_CONTROLLERS_IN_ANY_PORTS

			start=(controller1.startDebounceChannel->value);

#endif// PP: ndef ALLOW_CONTROLLERS_IN_ANY_PORTS

#endif// PP: !pc

			if(start)
			{
				PlaySample("select.wav", 255);

				// PP: REMOUT: can't use this	controller1.idleTime = 0.0f;
				demoIdleTime=0.0f;

#if(BPLATFORM!=PC)
				biForceDebounce(controller1.startDebounceChannel);// = 0;
#else
				biForceDebounce(controller1.crossDebounceChannel);
#endif
				
				biForceDebounce(controller1.menuSelectDebounceChannel);
				biForceDebounce(controller1.mb1DebounceChannel);

				splashToDraw = menuSplash;

				demoMenuBook->nextPage();

				demoInfoBook->open();

				DemoMenuState = SINGLE_PLAYER;
			}
		}
		break;

	case SINGLE_PLAYER:		

		if (OKBUTTON)
		{
			// PP: REMOUT: can't use this	controller1.idleTime = 0.0f;	
			demoIdleTime=0.0f;

			biForceDebounce(controller1.startDebounceChannel);// = 0;
			biForceDebounce(controller1.crossDebounceChannel);// = 0;
			biForceDebounce(controller1.menuSelectDebounceChannel);
			biForceDebounce(controller1.mb1DebounceChannel);

			PlaySample("select.wav", 255);
				
			DemoMenuState = CONTROLLER_INFO;
			DemoLastState = SINGLE_PLAYER;

// NH: No controller splash for PC
#if BPLATFORM != PC
			splashToDraw = contSplash;
#endif						
			demoMenuBook->gotoPage("controller");
			demoInfoBook->close();
		}

		if ((controller1.menuDownDebounceChannel->value != 0)/* PP: REMOUT: can't use this	(controller1.dPadYDebounceChannel->value < 0 || controller1.y1DebounceChannel->value < 0 || controller1.menuDownRepeatChannel->value)*/
			|| (mouse.mousePos[Y]/videoMode.yScreen > -0.142f && mouse.mousePos[Y]/videoMode.yScreen < 0.096f && mouse.mousePos[X]/videoMode.xScreen > 0.19f && mouse.mousePos[X]/videoMode.xScreen < 0.37f))
		{
			biForceDebounce(controller1.dPadYDebounceChannel);
			biForceDebounce(controller1.y1DebounceChannel);
			biForceDebounce(controller1.menuUpRepeatChannel);
			
			PlaySample("moveselection.wav", 255);

			DemoMenuState = MULTI_PLAYER;

			demoMenuSubBook->nextPage();

			demoInfoBook->gotoPage("multiplayer");
		}

#if(BPLATFORM==PC)
		else if ((mouse.mousePos[Y]/videoMode.yScreen > -0.297f && mouse.mousePos[Y]/videoMode.yScreen < -0.259f && mouse.mousePos[X]/videoMode.xScreen > 0.212f && mouse.mousePos[X]/videoMode.xScreen < 0.353f))
		{
			PlaySample("moveselection.wav", 255);

			DemoMenuState = EXIT;

			demoInfoBook->gotoPage("exit");
		
			demoMenuSubBook->gotoPage("exit menu");
		}
			
#endif // NH: PC
		
		break;

	case MULTI_PLAYER:	
		
		if (OKBUTTON
#if(BPLATFORM != PC) // NH: No need to check pad on PC
			&& (biFindPad(1) != -1)
#endif // NH: #if(BPLATFORM != PC)
			)			
		{
			PlaySample("select.wav", 255);

			// PP: REMOUT: can't use this	controller1.idleTime = 0.0f;
			demoIdleTime=0.0f;

			biForceDebounce(controller1.startDebounceChannel);// = 0;
			biForceDebounce(controller1.crossDebounceChannel);// = 0;
			biForceDebounce(controller1.menuSelectDebounceChannel);
			biForceDebounce(controller1.mb1DebounceChannel);

			//gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].difficulty = DIFF_EASY;

			demoMenuBook->gotoPage("controller");

			// PP: WHAT DOES THIS CODE DO?  WHY IS IT DIFFERENT IN NTSC?, TP: good point
			if (1)	// TP: videoMode.flags & BDISPLAYFLAG_NTSC)
			{
				bkGenerateEvent("scenechange", "_vrdeptstr");

				gameStatus.multiplayer.numPlayers = 2;
				gameStatus.multiplayer.versusCPU = false;

				// PP: haven't bound player2 yet
				padCheckMode=PADCHECK_WAITING_FOR_PLAYER2;

				// PP: REMOUT: can't use this	controller1.idleTime = 0.0f;
				demoIdleTime=0.0f;

				demoMenuBook->close();

				camera[0].fade.StartFadeDown(FADETYPE_NORMAL, 0.5f);

				camera[0].fade.SetFadeDownFinishedCallback(StartDemoGameFadeDownCallback);
			}
			else
			{

// NH: No controller splash for PC
#if BPLATFORM != PC				
				splashToDraw = contSplash;
#endif // NH: BPLATFORM != PC
				

				DemoMenuState = CONTROLLER_INFO;
				DemoLastState = MULTI_PLAYER;
			}

			demoInfoBook->close();
		}
		
		if ((controller1.menuUpDebounceChannel->value!=0)// PP: REMOUT: can't use this	/*(controller1.dPadYDebounceChannel->value > 0 || controller1.y1DebounceChannel->value > 0 || controller1.menuUpRepeatChannel->value)*/
		|| (mouse.mousePos[Y]/videoMode.yScreen > 0.14f && mouse.mousePos[Y]/videoMode.yScreen < 0.37f && mouse.mousePos[X]/videoMode.xScreen > 0.19f && mouse.mousePos[X]/videoMode.xScreen < 0.37f))
		{
			biForceDebounce(controller1.dPadYDebounceChannel);
			biForceDebounce(controller1.y1DebounceChannel);
			biForceDebounce(controller1.menuUpRepeatChannel);

			PlaySample("moveselection.wav", 255);

			DemoMenuState = SINGLE_PLAYER;

			demoInfoBook->gotoPage("single player");
		
			demoMenuSubBook->prevPage();
		}

#if(BPLATFORM==PC)
		else if ((controller1.dPadYDebounceChannel->value < 0 || controller1.y1DebounceChannel->value < 0 || controller1.menuDownRepeatChannel->value)
				|| (mouse.mousePos[Y]/videoMode.yScreen > -0.297f && mouse.mousePos[Y]/videoMode.yScreen < -0.259f && mouse.mousePos[X]/videoMode.xScreen > 0.212f && mouse.mousePos[X]/videoMode.xScreen < 0.353f))
		{
			PlaySample("moveselection.wav", 255);

			DemoMenuState = EXIT;

			demoInfoBook->gotoPage("exit");
		
			demoMenuSubBook->gotoPage("exit menu");
		}
			
#endif // NH: PC
		
#if (BPLATFORM != PC)
		if ((demoInfoBook->getCurrentPage() == demoInfoBook->getPage("multiplayer")) && (biFindPad(1) == -1))
			demoInfoBook->gotoPage("multiplayer unavailable");
		else if ((demoInfoBook->getCurrentPage() == demoInfoBook->getPage("multiplayer unavailable")) && (biFindPad(1) != -1))
#else
		if (demoInfoBook->getCurrentPage() == demoInfoBook->getPage("multiplayer"))
#endif // NH: #if (BPLATFORM != PC)
			demoInfoBook->gotoPage("multiplayer");

		break;

	case EXIT:		

		if (OKBUTTON)
		{
			// PP: REMOUT: can't use this	controller1.idleTime = 0.0f;
			demoIdleTime=0.0f;

			biForceDebounce(controller1.startDebounceChannel);// = 0;
			biForceDebounce(controller1.crossDebounceChannel);// = 0;
			biForceDebounce(controller1.menuSelectDebounceChannel);
			biForceDebounce(controller1.mb1DebounceChannel);

			PlaySample("select.wav", 255);

			demoInfoBook->close();
			demoMenuBook->close();

			mouse.active = false;

			camera[0].fade.StartFadeDown(FADETYPE_NORMAL, 0.5f);

			camera[0].fade.SetFadeDownFinishedCallback(StopDemoGameFadeDownCallback);
		}

		if ((controller1.dPadYDebounceChannel->value > 0 || controller1.y1DebounceChannel->value > 0 || controller1.menuUpRepeatChannel->value)
			|| (mouse.mousePos[Y]/videoMode.yScreen > -0.142f && mouse.mousePos[Y]/videoMode.yScreen < 0.096f && mouse.mousePos[X]/videoMode.xScreen > 0.19f && mouse.mousePos[X]/videoMode.xScreen < 0.37f))
		{
			PlaySample("moveselection.wav", 255);

			DemoMenuState = MULTI_PLAYER;

			demoMenuSubBook->prevPage();

			demoInfoBook->gotoPage("multiplayer");
		}
		else if (mouse.mousePos[Y]/videoMode.yScreen > 0.14f && mouse.mousePos[Y]/videoMode.yScreen < 0.37f && mouse.mousePos[X]/videoMode.xScreen > 0.19f && mouse.mousePos[X]/videoMode.xScreen < 0.37f)
		{
			PlaySample("moveselection.wav", 255);

			DemoMenuState = SINGLE_PLAYER;

			demoInfoBook->gotoPage("single player");
		
			demoMenuSubBook->gotoPage("single menu");
		}	

		break;

	case CONTROLLER_INFO:
		if ((camera[0].fade.GetFadeState()==FADESTATE_OFF)
#if (BPLATFORM!=PC)
#if (BPLATFORM==XBOX)
			&& (controller1.startDebounceChannel->value||controller1.crossDebounceChannel->value)
#else
			&& controller1.startDebounceChannel->value
#endif
#endif
			)
		{
			PlaySample("select.wav", 255);
			
			demoIdleTime=0.0f;

			demoMenuBook->close();

			KillDemoMenu();

			if (DemoLastState == SINGLE_PLAYER)
			{
				InitialiseSaveGameSlot(0, DIFF_EASY);

				gameStatus.globalSettings.currentSaveSlot = 0;
				
				gameStatus.multiplayer.numPlayers = 1;
				gameStatus.multiplayer.versusCPU = true;

				/*if (map.sceneNumber == SCENE_DEPTSTR)
				{
					demoIdleTime=0.0f;

					demoMenuBook->close();*/

					camera[0].fade.FadeAndChangeScene(&gameStatus, FADETYPE_NORMAL, SCENE_DEPTSTR, 1.0f);

					//SetFadeDownFinishedCallback(&camera[0].fade, StartDemoGameFadeDownCallback);
				/*}//bkGenerateEvent("scenechange", "_reset");
				else
					bkGenerateEvent("scenechange", "_deptstr");*/
			}
			else
			{				
				gameStatus.multiplayer.numPlayers = 2;
				gameStatus.multiplayer.versusCPU = false;

				//bkGenerateEvent("scenechange", "_vrdeptstr");

				camera[0].fade.FadeAndChangeScene(&gameStatus, FADETYPE_NORMAL, SCENE_DEPTSTRVEHICLERACE, 1.0f);

				//SetFadeDownFinishedCallback(&camera[0].fade, StartDemoGameFadeDownCallback);

				// PP: we haven't bound player2 yet
				padCheckMode=PADCHECK_WAITING_FOR_PLAYER2;
			}


			/*PlaySample("select.wav", 255);

			// PP: REMOUT: can't use this	controller1.idleTime = 0.0f;
			demoIdleTime=0.0f;

			demoMenuBook->close();

			//StartFadeDown(&camera[0].fade, FADETYPE_NORMAL, 0.5f);

			//SetFadeDownFinishedCallback(&camera[0].fade, StartDemoGameFadeDownCallback);*/
		}
		else if (camera[0].fade.GetFadeState()==FADESTATE_OFF && CANCELBUTTON)
		{
			if (DemoLastState == SINGLE_PLAYER)
			{
				demoInfoBook->gotoPage("single player");

				demoMenuSubBook->gotoPage("single menu");

				DemoMenuState = SINGLE_PLAYER;
			}
			else
			if (DemoLastState == MULTI_PLAYER)
			{
				demoInfoBook->gotoPage("multiplayer");

				demoMenuSubBook->gotoPage("multi menu");

				DemoMenuState = MULTI_PLAYER;
			}

			PlaySample("deselect.wav", 255);

			demoMenuBook->gotoPage("single menu");
			
			// PP: REMOUT: can't use this	controller1.idleTime = 0.0f;
			demoIdleTime=0.0f;

			demoInfoBook->open();

			splashToDraw = menuSplash;
		}
#if(BPLATFORM != PC)
		else if((DemoLastState == MULTI_PLAYER) && (biFindPad(1) == -1))// PP: 2nd pad removed while in 2player controller screen - go back to prev screen
		{
			// PP: *** DUPLICATED CODE *** I'm in a hurry

			demoInfoBook->gotoPage("multiplayer");

			demoMenuSubBook->gotoPage("multi menu");

			DemoMenuState = MULTI_PLAYER;

			PlaySample("deselect.wav", 255);

			demoMenuBook->gotoPage("single menu");
			
			// PP: REMOUT: can't use this	controller1.idleTime = 0.0f;
			demoIdleTime=0.0f;

			demoInfoBook->open();

			splashToDraw = menuSplash;
		}
#endif // NH: #if(BPLATFORM != PC)
		
		break;
	}
}

// PP: moveme
void QuitBackToXDemos();


/* --------------------------------------------------------------------------------
   Function		: interruptBlurb
   Purpose		: check for interruption of outro blurb
   Parameters	: 
   Returns		: true if blurb was interrupted, else false
   Info			: // PP: only call this ifdef INTERRUPTIBLE_ATTRACT
*/
static bool interruptBlurb(void)
{
	// PP: on Xbox, we can interrupt the kiosk mode outro blurb and it'll take us back to the demo frontend

	if((!CONSUMER_DEMO_PLAYABLE) &&	(attractInterrupt || AttractInterruptButton()))
	{
		// PP: kiosk mode broken out of by player

		// PP: no longer in kiosk mode
		playmode=SCEE_DEMO_PLAYMODE_PLAYABLE;

		// PP: 0=not demo
		gameStatus.demoMode=0;

		camera[0].fade.StartFadeDown(FADETYPE_NORMAL, 0.5f);

		// PP: start the demo frontend again
		camera[0].fade.SetFadeDownFinishedCallback(StartDemoMenuFadeDownCallback);

		// PP: Force-debounce ok button(s)
		FORCE_OKBUTTON_DEBOUNCE;

		demoMenuBook->close();

		return true;
	}

	return false;
}


char UpdateDemoQuit()
{
	if (gameStatus.currentState != GAMESTATE_DEMOQUIT)
		return 0;

	if (camera[0].fade.GetFadeValue())
		return 0;

	switch (DemoMenuState)
	{
	case PITCH_ONE:	
		{
#ifdef INTERRUPTIBLE_ATTRACT
			if(!interruptBlurb())
#endif// PP: def INTERRUPTIBLE_ATTRACT
			{
			//	bkPrintf("pitch1: idle %5.2f\n", controller1.idleTime);
				if(OKBUTTON_PADNUM(1)	
#ifdef ALLOW_CONTROLLERS_IN_ANY_PORTS// PP: any pad must be able to turn the page
				||OKBUTTON_PADNUM(2)
#if(NUM_CONTROLLER_PORTS == 4)
				||OKBUTTON_PADNUM(3)
				||OKBUTTON_PADNUM(4)
#endif// PP: 4 ports
#endif// PP: def (ALLOW_CONTROLLERS_IN_ANY_PORTS)
				||(demoIdleTime>30.0f))// PP: REMOUT: can't use this	(controller1.idleTime > 30.0f)
				{
					// PP: REMOUT: can't use this	controller1.idleTime = 0.0f;
					demoIdleTime=0.0f;

					biForceDebounce(controller1.startDebounceChannel);// = 0;
					biForceDebounce(controller1.crossDebounceChannel);// = 0;
					biForceDebounce(controller1.menuSelectDebounceChannel);
					biForceDebounce(controller1.mb1DebounceChannel);

					// PP: turn the sub-book to the next page of blurb
					demoMenuSubBook2->nextPage();

					DemoMenuState = PITCH_TWO;

					FreeSplashScreen(&splash);
					LoadSplashScreen(&splash, "demo06.bmp", "splshscr");
				}
			}
		}
		break;

	case PITCH_TWO:
		{
		//	bkPrintf("pitch2: idle %5.2f\n", controller1.idleTime);

			// PP: did the player stop the outro blurb?
			bool	playerStopped;

#ifdef INTERRUPTIBLE_ATTRACT
			if(!interruptBlurb())
#endif// PP: def INTERRUPTIBLE_ATTRACT
			{
				if(OKBUTTON_PADNUM(1)	
#ifdef ALLOW_CONTROLLERS_IN_ANY_PORTS// PP: any pad must be able to turn the page
				||OKBUTTON_PADNUM(2)
#if(NUM_CONTROLLER_PORTS == 4)
				||OKBUTTON_PADNUM(3)
				||OKBUTTON_PADNUM(4)
#endif// PP: 4 ports
#endif// PP: def (ALLOW_CONTROLLERS_IN_ANY_PORTS)
				||(demoIdleTime>30.0f))// PP: REMOUT: can't use this	(controller1.idleTime > 30.0f)
				{
#ifndef STANDALONE
				//gameStatus.demoMode = 1;
#endif// PP: def STANDALONE

					// PP: Force-debounce ok button(s)
					FORCE_OKBUTTON_DEBOUNCE;

					demoMenuBook->close();

					camera[0].fade.StartFadeDown(FADETYPE_NORMAL, 0.5f);
					camera[0].fade.SetFadeDownFinishedCallback(DemoQuitFadeDownCallback);

					return 0;
				}
			}
		}
		break;
	}

	return 0;
}

void DrawDemoMenu()
{
	char	mystring[256];

	if (gameStatus.currentState != GAMESTATE_DEMOMENU)
		return;

	// prepare scene
	bdSetupView();
	bdBeginScene();

	applyFOV();
	bdSetViewClipPlanes(videoMode.nearPlane, videoMode.farPlane);
	bdSetViewport(int(videoMode.screens[0].viewport.left), int(videoMode.screens[0].viewport.top), int(videoMode.screens[0].viewport.widthAbs()), int(videoMode.screens[0].viewport.heightAbs()));


	DrawSplashScreen(&splashToDraw);


	bdSetIdentityObjectMatrix();
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);

	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	if (demoMenuBook)
		demoMenuBook->drawDebug();

	/*
	if (DemoMenuState == CONTROLLER_INFO)
	{
		TBVector	tempVec, tempVec2;
		COLOUR		col = COLOUR(0,0,0,128);

		// JW: Select Button
		SETVECTOR(tempVec, -156.0f, -72.0f, 0.0f, 0.0f);
		SETVECTOR(tempVec2, -156.0f, 37.0f, 0.0f, 0.0f);
		utilDrawThickLine(tempVec, tempVec2, 3, col);

		// JW: Start Button
		SETVECTOR(tempVec, -106.0f, -72.0f, 0.0f, 0.0f);
		SETVECTOR(tempVec2, -106.0f, 37.0f, 0.0f, 0.0f);
		utilDrawThickLine(tempVec, tempVec2, 3, col);

		// JW: Movement Stick
		SETVECTOR(tempVec, -248.0f, -33.0f, 0.0f, 0.0f);
		SETVECTOR(tempVec2, -174.0f, 0.0f, 0.0f, 0.0f);
		utilDrawThickLine(tempVec, tempVec2, 3, col);

		// JW: Camera Stick
		SETVECTOR(tempVec, -15.0f, -33.0f, 0.0f, 0.0f);
		SETVECTOR(tempVec2, -89.0f, 0.0f, 0.0f, 0.0f);
		utilDrawThickLine(tempVec, tempVec2, 3, col);

		// JW: Cross Button
		SETVECTOR(tempVec, -47.0f, 23.0f, 0.0f, 0.0f);
		SETVECTOR(tempVec2, 37.0f, 23.0f, 0.0f, 0.0f);
		utilDrawThickLine(tempVec, tempVec2, 3, col);

		// JW: Circle Button
		SETVECTOR(tempVec, -25.0f, 45.0f, 0.0f, 0.0f);
		SETVECTOR(tempVec2, 37.0f, 45.0f, 0.0f, 0.0f);
		utilDrawThickLine(tempVec, tempVec2, 3, col);
		
		// JW: Triangle Button
		SETVECTOR(tempVec, -47.0f, 67.0f, 0.0f, 0.0f);
		SETVECTOR(tempVec2, 37.0f, 67.0f, 0.0f, 0.0f);
		utilDrawThickLine(tempVec, tempVec2, 3, col);

		// JW: Square Button
		SETVECTOR(tempVec, -70.0f, 89.0f, 0.0f, 0.0f);
		SETVECTOR(tempVec2, 37.0f, 89.0f, 0.0f, 0.0f);;
		utilDrawThickLine(tempVec, tempVec2, 3, col);
		SETVECTOR(tempVec, -70.0f, 90.0f, 0.0f, 0.0f);
		SETVECTOR(tempVec2, -70.0f, 45.0f, 0.0f, 0.0f);
		utilDrawThickLine(tempVec, tempVec2, 3, col);
	}
*/
// PP: REMOUT: Now handled by the Book List				demoMenuBook->draw();



	camera[0].fade.DrawFade();

	bdEndScene();

	// PP: finish timing the frame; calculate frames per second, frame length, etc.
	finishTimingFrame();
	
	// null time, anything between here and end is not included in the timer
	camera[0].fade.UpdateFade();

	// PP: start timing the frame
	startTimingFrame();

//	BookList_draw();
}

void DrawDemoQuit()
{
	if (gameStatus.currentState != GAMESTATE_DEMOQUIT)
		return;

	// prepare scene
	bdSetupView();
	bdBeginScene();

	applyFOV();
	bdSetViewClipPlanes(videoMode.nearPlane, videoMode.farPlane);
	bdSetViewport(int(videoMode.screens[0].viewport.left), int(videoMode.screens[0].viewport.top), int(videoMode.screens[0].viewport.widthAbs()), int(videoMode.screens[0].viewport.heightAbs()));

	DrawSplashScreen(&splash);

	bdSetIdentityObjectMatrix();
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);

	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

/* PP: REMOUT: Now handled by the Book List		
	demoMenuBook->draw();
*/
	camera[0].fade.DrawFade();

	bdEndScene();

	// PP: finish timing the frame; calculate frames per second, frame length, etc.
	finishTimingFrame();
	
	// null time, anything between here and end is not included in the timer
	camera[0].fade.UpdateFade();

	// PP: start timing the frame
	startTimingFrame();
}

void KillDemoMenu()
{
	//FreeSplashScreen(&splashToDraw);
	FreeSplashScreen(&titleSplash);
	FreeSplashScreen(&menuSplash);
	FreeSplashScreen(&contSplash);
	FreeSplashScreen(&splash);

	FreeWholePackage("demo");

	if (demoMenuBook)
	{
		delete(demoMenuBook);
		demoMenuBook = 0;
	}

	if (demoInfoBook)
	{
		delete(demoInfoBook);
		demoInfoBook = 0;
	}
}

void StartDemoAttract()
{
	bkPrintf("demoMode set to Disk Play in Start Demo Attract");

	gameStatus.demoMode = 1;

	gameStatus.multiplayer.numPlayers = 1;
	gameStatus.multiplayer.versusCPU = true;

	attractInterrupt = false;

	// PP: reset controller bindings
	padCheckMode=PADCHECK_ALL;
	bindControllersToDefaultPorts();
		
#if BPLATFORM == PC
	mouse.active = false;
#endif;

	bkGenerateEvent("scenechange","_deptstr");
}

void DemoForceQuit()
{	
#ifdef STANDALONE
	camera[0].fade.StartFadeDown(FADETYPE_NORMAL, 0.25f);

	//#ifdef STANDALONE
	//	SetFadeDownFinishedCallback(&camera[0].fade, DemoQuitFadeDownCallback);
	//#else
		camera[0].fade.SetFadeDownFinishedCallback(DemoForceQuitFadeDownCallback);
	//#endif

	if (gameStatus.currentState == GAMESTATE_PAUSE)
	{
		if (gameStatus.multiplayer.numPlayers == 2)
			ExitMPPauseMode();
		else
			ExitPauseMode();
	}

	if (gameStatus.multiplayer.numPlayers == 2)
		camera[1].fade.StartFadeDown(FADETYPE_NORMAL, 0.25f);
#else
	gameStatus.quitNow = true;
#endif

	StopAllChannels();
			
	musicSystem.ChooseMusic(SCENE_DEPTSTR,MUSICTYPE_ALL,MUSICSTATE_STOP);

	// PP: close all the books on the Book List instantly (true=instant)
	BookList_closeAll(true);
}

void DemoForceQuitFadeDownCallback(class TAG_FADE *fade)
{
#ifdef STANDALONE
	KillDemoMenu();
	
	bsStopAllChannels();

	musicSystem.ChooseMusic(map.sceneNumber,MUSICTYPE_ALL,MUSICSTATE_STOP);

	bkDeleteAllSamples();
	
	bdCloseDisplay();

	doCorporateStuff();

	SetupDisplay(&videoMode);

	#if(BPLATFORM!=PC)
		#ifdef ALLOW_CONTROLLERS_IN_ANY_PORTS
			if (biFindPad(0) != -1)
		#else // NH: ALLOW_CONTROLLERS_IN_ANY_PORTS
			if (biGetPortStatus(0) == EBPORTSTATUS_CONNECTED)
		#endif // NH: ALLOW_CONTROLLERS_IN_ANY_PORTS
				bkGenerateEvent("scenechange", "_language");
			else
			{
				gameStatus.demoMode = 1;

				gameStatus.multiplayer.numPlayers = 1;
				gameStatus.multiplayer.versusCPU = 1;

				bkGenerateEvent("scenechange", "_deptstr");
			}

		SetGameState(&gameStatus, GAMESTATE_INGAME, GAMESUBSTATE_NORMAL);

	#else // NH: #if(BPLATFORM!=PC)
		StartDemoMenuFadeDownCallback(&camera[0].fade);
	#endif // NH: #if(BPLATFORM!=PC)
#else
	gameStatus.quitNow = true;	
#endif
}

// JW: Callback to quit the demo completely

void DemoQuitFadeDownCallback(class TAG_FADE *fade)
{
	KillDemoMenu();

#ifndef STANDALONE

	#if(BPLATFORM == XBOX)
		// PP: SEE YA IN HELL!  HAAAHAAHAAAAAA!!!!!!!
		QuitBackToXDemos();
	#endif// PP: (BPLATFORM == XBOX)
		
	gameStatus.quitNow = true;	
#else
	
	bsStopAllChannels();

	musicSystem.ChooseMusic(map.sceneNumber,MUSICTYPE_ALL,MUSICSTATE_STOP);

	bkDeleteAllSamples();
	
	bdCloseDisplay();

	doCorporateStuff();

	SetupDisplay(&videoMode);

	//bkGenerateEvent("scenechange", "_language");

#if(BPLATFORM!=PC)
#ifdef ALLOW_CONTROLLERS_IN_ANY_PORTS
	if (biFindPad(0) != -1)
#else // NH: ALLOW_CONTROLLERS_IN_ANY_PORTS
	if (biGetPortStatus(0) == EBPORTSTATUS_CONNECTED)
#endif // NH: ALLOW_CONTROLLERS_IN_ANY_PORTS
		StartDemoMenuFadeDownCallback(&camera[0].fade);
	else
	{
		gameStatus.demoMode = 1;

		gameStatus.multiplayer.numPlayers = 1;
		gameStatus.multiplayer.versusCPU = 1;

		bkGenerateEvent("scenechange", "_deptstr");

		SetGameState(&gameStatus, GAMESTATE_INGAME, GAMESUBSTATE_NORMAL);
	}
#else // NH: #if(BPLATFORM!=PC)
	StartDemoMenuFadeDownCallback(&camera[0].fade);
#endif // NH: #if(BPLATFORM!=PC)
#endif
}

void DemoQuitFadeDownCallbackNoExit(class TAG_FADE *fade)
{
	KillDemoMenu();

#ifndef STANDALONE

	#if(BPLATFORM == XBOX)
		// PP: SEE YA IN HELL!  HAAAHAAHAAAAAA!!!!!!!
		QuitBackToXDemos();
	#endif// PP: (BPLATFORM == XBOX)

	//gameStatus.quitNow = true;	
#else
	
	bsStopAllChannels();

	musicSystem.ChooseMusic(map.sceneNumber,MUSICTYPE_ALL,MUSICSTATE_STOP);

	bkDeleteAllSamples();
	
	bdCloseDisplay();

	doCorporateStuff();

	SetupDisplay(&videoMode);

	//bkGenerateEvent("scenechange", "_language");

#ifdef ALLOW_CONTROLLERS_IN_ANY_PORTS
	if (biFindPad(0) != -1)
#else
	if (biGetPortStatus(0) == EBPORTSTATUS_CONNECTED)
#endif
		StartDemoMenuFadeDownCallback(&camera[0].fade);
	else
	{
		gameStatus.demoMode = 1;

		gameStatus.multiplayer.numPlayers = 1;
		gameStatus.multiplayer.versusCPU = 1;

		bkGenerateEvent("scenechange", "_deptstr");

		SetGameState(&gameStatus, GAMESTATE_INGAME, GAMESUBSTATE_NORMAL);
	}
	
#endif
}

void StartDemoMenuFadeDownCallback(class TAG_FADE *fade)
{
	gameStatus.controlSuspend = false;

	// PP: TEMP TEST/BODGE to prevent checkpoint message coming up as I time out from the loaded press start screen
	BookList_closeAll(false);
	/*if(gameStatus.player2 && gameStatus.player2->actorStatus->multiplayer.pStatsBook)
	{
		gameStatus.player1->actorStatus->multiplayer.pStatsBook->close(0.0f, true);
		gameStatus.player2->actorStatus->multiplayer.pStatsBook->close(0.0f, true);
	}*/

	setNumScreens(1);

	SetGameState(&gameStatus, GAMESTATE_DEMOMENU, GAMESUBSTATE_NORMAL);

	fade->SetFadeDownFinishedCallback(NULL);

#ifdef ALLOW_CONTROLLERS_IN_ANY_PORTS
	// PP: reset controller bindings
	padCheckMode=PADCHECK_ALL;
#else// PP: ifndef ALLOW_CONTROLLERS_IN_ANY_PORTS
	padCheckMode=PADCHECK_NORMAL;
#endif// PP: ndef ALLOW_CONTROLLERS_IN_ANY_PORTS
	bindControllersToDefaultPorts();

	// PP: can't / don't need to do this stuff if we've skipped the language select (gone straight to the game intro cutscene as on Xbox/PC)
	if ((map.sceneNumber == SCENE_LANGUAGESELECT) && (!skipLanguage))
	{
		SwitchLanguage();

		StopChannel(&characterSounds.tweety);

		FreeLanguageSelect();
	}

	demoPak = LoadWholePackage("demo", 1);

	bkLoadPackageSamples(demoPak);

	CreateDemoMenu();

	DemoMenuState = TITLE_SCREEN;
}

void StartDemoAttractFadeDownCallback(class TAG_FADE *fade)
{
	SetGameState(&gameStatus, GAMESTATE_INGAME, GAMESUBSTATE_NORMAL);

	KillDemoMenu();

	fade->SetFadeDownFinishedCallback(NULL);
}

void StartDemoGameFadeDownCallback(class TAG_FADE *fade)
{
#if(BPLATFORM == XBOX || BPLATFORM == PC)
	// PP: respond to scene change messages (this is needed because I skip the language select loadup on Xbox consumer demo)
	RespondToSceneChangeMessages();
#endif// PP: xbox

	SetGameState(&gameStatus, GAMESTATE_INGAME, GAMESUBSTATE_NORMAL);

	KillDemoMenu();

	fade->SetFadeDownFinishedCallback(NULL);
}

void StopDemoGameFadeDownCallback(class TAG_FADE *fade)
{
	gameStatus.controlSuspend = false;

	setNumScreens(1);

	gameStatus.multiplayer.numPlayers = 1;

	gameStatus.multiplayer.versusCPU = 1;

	SetGameState(&gameStatus, GAMESTATE_DEMOQUIT, GAMESUBSTATE_NORMAL);
	
	fade->SetFadeDownFinishedCallback(NULL);

	StopAllChannels();

	CreateDemoQuit();

	DemoMenuState = PITCH_ONE;

	// PP: reset controller bindings
	padCheckMode=PADCHECK_ALL;
	bindControllersToDefaultPorts();

}


void StopDemoBlurbFadeDownCallback(class TAG_FADE *fade)
{
	KillDemoMenu();

	fade->SetFadeDownFinishedCallback(NULL);
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												XBOX-SPECIFIC INTERACTIVE DEMO STUFF
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#if(BPLATFORM == XBOX)


DEMO_LAUNCH_INFO		*demoInfo;


/*	--------------------------------------------------------------------------------
	Function 	: CheckLaunchInfo
	Purpose 	: Check the launch info passed into the Xbox interactive demo
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void CheckLaunchInfo(void)
{
	bkPrintf("\n\nLAUNCH TYPE: %d\n\n",bXBLaunchType);

	bkPrintf("\n\n");

	demoInfo = (DEMO_LAUNCH_INFO *)&bXBLaunchData;

	if(bXBLaunchType == LDT_TITLE)
	{
		char infoCopy[128];

		// PP: REMOUT: IMPORTANT!!!!: DO NOT TRY TO USE szDemoInfo IN ANY WAY - THAT INCLUDES STRNCPYING IT!!!!STRNCPY(infoCopy, demoInfo->szDemoInfo, 128);

		bkPrintf("====== DEMO INFO ======\n");
		bkPrintf("dwID:           %d\n", demoInfo->dwID);
		bkPrintf("dwRunmode:      %d\n", demoInfo->dwRunmode);
		bkPrintf("dwTimeout:      %d\n", demoInfo->dwTimeout);
		bkPrintf("szLauncherXBE:  \"%s\"\n", demoInfo->szLauncherXBE);
		bkPrintf("szLaunchedXBE:  \"%s\"\n", demoInfo->szLaunchedXBE);
		// PP: REMOUT: IMPORTANT!!!!: DO NOT TRY TO USE szDemoInfo IN ANY WAY - THAT INCLUDES STRNCPYING IT!!!!bkPrintf("szDemoInfo:     \"%s\"\n", infoCopy);
		bkPrintf("=======================\n");

#ifdef CONSUMERDEMO

	// PP: convert those xbox launch details to the ps2 launch details which are the ones that actually get used...
	// PP: The ones that we need, that is...

	// PP: ~~~~~~~~~~~~

	// PP: kiosk mode/not kiosk mode...

	if(demoInfo->dwRunmode == KIOSK_MODE)
	{
		playmode=SCEE_DEMO_PLAYMODE_ATTRACT;
	}
	else
	{
		playmode=SCEE_DEMO_PLAYMODE_PLAYABLE;
	}

	// PP: aspect ratio: only 4:3 is allowed in a demo (TCR)

	aspect=SCEE_DEMO_ASPECT_4_3;

	// PP: time-out

	to_inactive=(ushort)(demoInfo->dwTimeout/1000);// PP: dwTimeout is in milliseconds!  You have to wonder why...

	// PP: ~~~~~~~~~~~~
	}
	else// PP: launched from debugger or launcher
	{
		// PP: TEMP TEST

		//*
		demoInfo->dwRunmode=NON_KIOSK_MODE;
		playmode=SCEE_DEMO_PLAYMODE_PLAYABLE;
		/*/
		demoInfo->dwRunmode=KIOSK_MODE;
		playmode=SCEE_DEMO_PLAYMODE_ATTRACT;
		//*/
	}

	if(playmode==SCEE_DEMO_PLAYMODE_ATTRACT)
	{
		// PP: hmm, well this is SUPPOSED to get set in MainSplash.
		// PP: However, the Xbox is never getting into that function.
		// PP: God knows how the PS2 is, if indeed it is.
		// PP: Probably just a timing fluke.
		gameStatus.demoMode = 1;// PP: 1=demo playing
	}

	bkPrintf("\nQUIT TIME: %f\n",(float)(demoInfo->dwTimeout*60.0f)/1000.0f);

	char *backslash = strrchr(demoInfo->szLaunchedXBE,'\\');

	if(backslash)
	{
		strcpy(bHomeDirectory,demoInfo->szLaunchedXBE);

		char *backslash = strrchr(bHomeDirectory,'\\');

		if(backslash)
			*(backslash + 1) = 0;
	}

	bkPrintf("\n\n%s\n\n",bHomeDirectory);

#else
//#ifndef TEST_LAUNCH
//	if(strcmp("XDEMOS",((char *)&bXBLaunchData)) == 0)
//#endif
//		launchedFromDemo = TRUE;
#endif

}


/*	--------------------------------------------------------------------------------
	Function 	: PrepForLaunch
	Purpose 	: Make preparations for returning to the application that launched the Xbox interactive demo
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PrepForLaunch(void)
{
	bkPrintf("\nPrep start\n");

	bkCacheEnable(FALSE);
//	StopMusic(-1000);
	StopAllChannels();
#if BPLATFORM == XBOX
	bDisplayInfo.d3dDevice->BlockUntilIdle();
#endif


	while (!bPauseBackgroundFileAccess(TRUE))
		Sleep(0);

	bkPrintf("\nPrep end\n");
}

/*	--------------------------------------------------------------------------------
	Function 	: QuitBackToXDemos
	Purpose 	: Quit from the Xbox interactive demo back to whatever launched the demo
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void QuitBackToXDemos(void)
{
	DWORD res;
	
	PrepForLaunch();

	D3DDevice_PersistDisplay();

	bkCacheFlush(TRUE);	

	if(bXBLaunchType == LDT_TITLE)
		res = XLaunchNewImage(((DEMO_LAUNCH_INFO *)&bXBLaunchData)->szLauncherXBE,&bXBLaunchData);// PP: pass the launch data back to the launcher, unchanged (this is recommended, if not required, by Microsoft)
	else
		res = XLaunchNewImage("d:\\XDemos\\XDemos.xbe",&bXBLaunchData);

	bkPrintf("\n\nXLaunchNewImage returned %d\n\n",res);
}


#endif// PP: (BPLATFORM == XBOX)


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
											END XBOX-SPECIFIC INTERACTIVE DEMO STUFF
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#endif// PP: def CONSUMERDEMO


/*  --------------------------------------------------------------------------------
	Function	: debounceAttractInterruptButton
	Purpose		: force-debounce all attract-interrupt buttons on a specified controller
	Parameters	: ref to controller to force-debounce
	Returns		: 
	Info		: // PP: 
*/
static void debounceAttractInterruptButton(PADCONTROLSTATUS &controller)
{
#if(BPLATFORM == XBOX)

	// PP: TCR 3-40 states that START, BACK, A, B, X, Y, Black, White, thumbstick clicks, and triggers should all interrupt the attract cycle
	
	biForceDebounce(controller.startDebounceChannel);// PP: start
	biForceDebounce(controller.selectDebounceChannel);// PP: back
	biForceDebounce(controller.crossDebounceChannel);// PP: a
	biForceDebounce(controller.circleDebounceChannel);// PP: b
	biForceDebounce(controller.squareDebounceChannel);// PP: x
	biForceDebounce(controller.triangleDebounceChannel);// PP: y
	biForceDebounce(controller.r1DebounceChannel);// PP: black
	biForceDebounce(controller.l1DebounceChannel);// PP: white
	biForceDebounce(controller.l3DebounceChannel);// PP: left stick click
	biForceDebounce(controller.r3DebounceChannel);// PP: right stick click
	biForceDebounce(controller.l2DebounceChannel);// PP: left trigger
	biForceDebounce(controller.r2DebounceChannel);// PP: right trigger

#elif(BPLATFORM == PC)

	biForceDebounce(controller1.crossDebounceChannel);
	biForceDebounce(controller1.menuSelectDebounceChannel);
	biForceDebounce(controller1.mb1DebounceChannel);

#else// PP: ps2/gc

	biForceDebounce(controller.startDebounceChannel);

#endif// PP: platform

}


/*  --------------------------------------------------------------------------------
	Function	: DebounceAttractInterruptButton
	Purpose		: force-debounce all attract-interrupt buttons on all appropriate controllers
	Parameters	: 
	Returns		: 
	Info		: // PP: theres a reason for this being non-static and starting with a capital - the ps2 compiler...
*/
void DebounceAttractInterruptButton(void)
{
	debounceAttractInterruptButton(controller1);

#ifdef ALLOW_CONTROLLERS_IN_ANY_PORTS	
	
	debounceAttractInterruptButton(controller2);

#if(NUM_CONTROLLER_PORTS == 4)

	debounceAttractInterruptButton(controller3);
	debounceAttractInterruptButton(controller4);

#endif// PP: 4 ports

#endif// PP: pads in any port
}


int				frameLength;

/*  --------------------------------------------------------------------------------
	Function	: RecordingDemo
	Purpose		: Handles pad input recording when producing a rolling demo
	Parameters	: 
	Returns		: 
	Info		: 
*/

void RecordingDemo(void)
{
	if (dmData)
	{
		if (dmData->session->noofSamples == dmData->session->maxSamples)
		{
			// JW: Run out of space for fps - reallocate memory
			float *newBuffer;

			// PP: allocate the new buffer for fps
			newBuffer = (float *) ZALLOC(sizeof(float) * (dmData->session->noofSamples + 1800));

			// PP: copy the existing fps's into the new buffer
			memcpy(newBuffer, dmData->fps, sizeof(float) * dmData->session->noofSamples);

			// PP: free the old buffer
			FREE(dmData->fps);

			// PP: use the new buffer
			dmData->fps = newBuffer;

			fpsStore = &dmData->fps[dmData->session->noofSamples];
		}
		
		// PP: update the session ptr; the thing could get realloc'd if it finds it needs more space
		dmData->session = biRecordInputs(dmData->session, &frameLength);

		//fps = 60;

		*fpsStore++ = fps;

		//*pos1Store = spare;

//		pos1Store++;

		//bmVectorCopy(spare, gameStatus.player1->actorInstance.position);
		//bmVectorCopy(*pos2Store, gameStatus.player2->actorInstance.position);

		//pos1Store++;

		fTime = 1.0f/fps;
		speedRatio = 60/fps;
	}

	if (controller1.startDebounceChannel->value)
	{
		controller1.startDebounceChannel->value = 0;

		bkGenerateEvent("scenechange","_reset");
	}
}


/*  --------------------------------------------------------------------------------
	Function	: FinishPlayingDemo
	Purpose		: finish playing the rolling demo
	Parameters	: was the demo playback stopped by the player?
	Returns		: 
	Info		: // PP: 
*/
static void FinishPlayingDemo(const bool playerStopped)
{
	// TP: make sure no other secure fades will stop this fade from happening, this is only needed for the attact mode
	camera[0].fade.RemoveSecureFade();

	camera[0].fade.Lock();

#ifndef CONSUMERDEMO

	// TP: change the generate events to immediate fade and change scenes so the fade could be locked
	if (gameStatus.demoMode == 1)
		#if (BPLATFORM == XBOX)
		{
			if (map.lastScene == SCENE_LANGUAGESELECT)
			{
				skipLanguage = false;

				camera[0].fade.FadeAndChangeScene(&gameStatus, FADETYPE_NORMAL, SCENE_LANGUAGESELECT, 1.0f);
				SetGameState(&gameStatus,GAMESTATE_FADING,GAMESUBSTATE_NORMAL);

				//bkGenerateEvent("scenechange", "_language");
			}
			else
			{
				camera[0].fade.FadeAndChangeScene(&gameStatus, FADETYPE_NORMAL, SCENE_FRONTEND, 1.0f);
				SetGameState(&gameStatus,GAMESTATE_FADING,GAMESUBSTATE_NORMAL);
			}
				//bkGenerateEvent("scenechange", "_frontend");
		}
		#else
		{
			camera[0].fade.FadeAndChangeScene(&gameStatus, FADETYPE_NORMAL, SCENE_FRONTEND, 1.0f);
			SetGameState(&gameStatus,GAMESTATE_FADING,GAMESUBSTATE_NORMAL);
			//bkGenerateEvent("scenechange", "_frontend");
		}
		#endif
	else
	{
		camera[0].fade.FadeAndChangeScene(&gameStatus,FADETYPE_CIRCLE,gameStatus.currentScene,1.0f);
		SetGameState(&gameStatus,GAMESTATE_FADING,GAMESUBSTATE_NORMAL);
	}
		//bkGenerateEvent("scenechange", "_reset");
#else
	camera[0].fade.StartFadeDown(FADETYPE_NORMAL, 1.0f, true);
		
	if (gameStatus.multiplayer.numScreens == 2)
		camera[1].fade.StartFadeDown(FADETYPE_NORMAL, 1.0f);
	
	if(CONSUMER_DEMO_PLAYABLE)
	{
		// PP: if the demo is playable, interrupting the attract mode will always take us to the demo front end
		// TP: Nope, never go from playable demo back to front end always go back to launcher
		camera[0].fade.SetFadeDownFinishedCallback(StopDemoGameFadeDownCallback);  // TP: StartDemoMenuFadeDownCallback
		gameStatus.controlSuspend = true;
	}
#ifdef INTERRUPTIBLE_ATTRACT
	else// PP: kiosk mode
	{
		if(playerStopped)// PP: if demo playback was stopped by the player
		{
			// PP: on Xbox, we can interrupt the kiosk mode rolling demo and it'll take us back to the demo frontend to let us play...

			camera[0].fade.SetFadeDownFinishedCallback(StartDemoMenuFadeDownCallback);// PP: this used to say StopDemoGameFadeDownCallback, which I don't think made any sense, certainly didn't match the comments

			// PP: not going to roll demo now, not yet at least
			gameStatus.demoMode=0;

			playmode=SCEE_DEMO_PLAYMODE_PLAYABLE;
		}
		else
		{
			// PP: demo finished naturally; to go outro blurb
			camera[0].fade.SetFadeDownFinishedCallback(StopDemoGameFadeDownCallback);
		}
	}
#else// PP: ifndef INTERRUPTABLE_ATTRACT
	{
		camera[0].fade.SetFadeDownFinishedCallback(StopDemoGameFadeDownCallback);
		gameStatus.controlSuspend = true;
	}
#endif// PP: ndef INTERRUPTABLE_ATTRACT
	
	demoBook->close();

	ClosePlayerStats(&gameStatus.player[0]);
	
	//sfxTimer = 1.0f;
	
	//gameStatus.demoMode = 0;
	
	gameStatus.multiplayer.numPlayers = 1;
	gameStatus.multiplayer.versusCPU = true;
	
	demoBook->shutdown();

	gameStatus.demoMode = 0;
	
	SAFE_DELETE(demoBook);
	SAFE_FREE(dmData->fps);
	SAFE_FREE(dmData->session);
	SAFE_FREE(dmData);

	TBClock		clock;
	bkReadClock(&clock);
	bmSRand(clock.second);

	EnableRumble(&controller1);
	EnableRumble(&controller2);

	// PP: finish overriding stereoscopy
	overrideStereoscopy(false);

#endif// PP: ?

	camera[0].fade.Unlock();
	//FadeAndChangeScene(&camera[0].fade, &gameStatus, FADETYPE_NORMAL, SCENE_FRONTEND, 1.0f);
}


/*  --------------------------------------------------------------------------------
	Function	: CheckForDemoInterrupt
	Purpose		: check for, and respond to, interruption of the rolling demo
	Parameters	: 
	Returns		: 
	Info		: // PP: this needs to be called after ProcessInput
*/
void CheckForDemoInterrupt(void)
{
	if(attractInterrupt)
	{
		// PP: reset the attract-interrupt flag
		attractInterrupt=false;

		FinishPlayingDemo(true);// PP: true=player stopped the demo

		// PP: force-debounce all attract-interrupt buttons
		DebounceAttractInterruptButton();
	}
}


/*  --------------------------------------------------------------------------------
	Function	: PlayingDemo
	Purpose		: Read key inputs from file to playback rolling demo
	Parameters	: 
	Returns		: 
	Info		: 
*/
void PlayingDemo(void)
{
	// PP: override stereoscopy - it would affect framewrite, which is not good in a rolling demo
	overrideStereoscopy(true);

	// PP: check at this early stage in the frame if the player is trying to interrupt the demo with pad input.
	// PP: It's only nearer the end of the frame that we'll be able to interrupt the demo in response to any such pad input,
	// PP: by which time that input wil have been overwritten by the recorded input, see?
	if(AttractInterruptButton())
	{
		attractInterrupt=true;
	}

	if(biPlaybackInputs(dmData->session, &frameLength))// PP: if the rolling demo has run to the end
	{
		// PP: finish playing the rolling demo
		FinishPlayingDemo(false);// PP: false=demo ended naturally
	}
	else
	{
		// PP: rolling demo still running

		fps = *fpsStore++;

		//fps = 60;	

		fTime = 1.0f/fps;
		speedRatio = 60/fps;

		// PP: also update rawFPS, rawFTime
		rawFPS=fps;
		rawFTime=fTime;
	}
}

/*  --------------------------------------------------------------------------------
	Function	: attractInterruptButton
	Purpose		: detect button presses that can interrupt the attract cycle 
	Parameters	: controller to check
	Returns		: 
	Info		: // PP: 
*/
static bool attractInterruptButton(PADCONTROLSTATUS &controller)
{
#if(BPLATFORM == XBOX)

	// PP: TCR 3-40 states that START, BACK, A, B, X, Y, Black, White, thumbstick clicks, and triggers should all interrupt the attract cycle
	
	if(controller.startDebounceChannel->value != 0) goto return_true;// PP: start
	if(controller.selectDebounceChannel->value != 0) goto return_true;// PP: back
	if(controller.crossDebounceChannel->value != 0) goto return_true;// PP: a
	if(controller.circleDebounceChannel->value != 0) goto return_true;// PP: b
	if(controller.squareDebounceChannel->value != 0) goto return_true;// PP: x
	if(controller.triangleDebounceChannel->value != 0) goto return_true;// PP: y
	if(controller.r1DebounceChannel->value != 0) goto return_true;// PP: black
	if(controller.l1DebounceChannel->value != 0) goto return_true;// PP: white
	if(controller.l3DebounceChannel->value != 0) goto return_true;// PP: left stick click
	if(controller.r3DebounceChannel->value != 0) goto return_true;// PP: right stick click
	if(controller.l2DebounceChannel->value != 0) goto return_true;// PP: left trigger
	if(controller.r2DebounceChannel->value != 0) goto return_true;// PP: right trigger

#elif(BPLATFORM == PC)

	if(controller1.crossDebounceChannel->value != 0) return true;
	if(controller1.menuSelectDebounceChannel->value != 0) return true;
	if(controller1.mb1DebounceChannel->value != 0) return true;

#else// PP: ps2/gc

	if(controller.startDebounceChannel->value != 0) return true;

#endif// PP: platform

	return false;

return_true:// PP: this label is so's I can breakpoint the return true

	return true;
}


/*  --------------------------------------------------------------------------------
	Function	: attractInterruptButton
	Purpose		: detect button presses (on all appropriate controllers) that can interrupt the attract cycle 
	Parameters	: 
	Returns		: 
	Info		: // PP: 
*/
bool AttractInterruptButton(void)
{
	if(attractInterruptButton(controller1)) return true;

#ifdef ALLOW_CONTROLLERS_IN_ANY_PORTS	
	
	if(attractInterruptButton(controller2)) return true;

#if(NUM_CONTROLLER_PORTS == 4)

	if(attractInterruptButton(controller3)) return true;
	if(attractInterruptButton(controller4)) return true;

#endif// PP: 4 ports

#endif// PP: pads in any port

	return false;
}


/*  --------------------------------------------------------------------------------
	Function	: DrawLegalSplashScreen
	Purpose		: 
	Parameters	: 
	Returns		: 
	Info		: 
*/

void DrawLegalSplashScreen()
{
	if (videoMode.flags & BDISPLAYFLAG_NTSC)
	{
		camera[0].fade.StartFadeUp(FADETYPE_NORMAL,1);
		
#ifdef STANDALONE
		LoadSplashScreen(&gameStatus.splashScreen,"legal.bmp","splshscr");
#else
		LoadSplashScreen(&gameStatus.splashScreen,"legal2.bmp","splshscr");
#endif
		
		TBTimerValue	startTime, finishTime;
		
		float	elapsedTime = 0.0f;
		
		startTime = bkTimerRead();
		
		do
		{
			biReadDevices();

			// prepare scene
			bdSetupView();
			bdBeginScene();
			
			applyFOV();
			bdSetViewClipPlanes(videoMode.nearPlane, videoMode.farPlane);
			bdSetViewport(int(videoMode.screens[0].viewport.left), int(videoMode.screens[0].viewport.top), int(videoMode.screens[0].viewport.widthAbs()), int(videoMode.screens[0].viewport.heightAbs()));
			
			DrawSplashScreen(&gameStatus.splashScreen);
			
			camera[0].fade.DrawFade();
			
			bdEndScene();
			
			finishTime = bkTimerRead();
			
			fps = bkTimerToFPS(bkTimerDelta(startTime,finishTime));
			
			fTime = 1.0f / fps;
			
			speedRatio = 60/fps;
			
			elapsedTime += fTime;
			
			camera[0].fade.UpdateFade();
			
			startTime = bkTimerRead();
			
			Flip(0, 0, 0, 128);
		}
		while (elapsedTime < 5.0f);
	}
}