// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : endoflevelstats.cpp
//   Purpose : provides functions to display stats at end of level
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "actors.h"
#include "control.h"
#include "main.h"
#include "display.h"
#include "TextFX.h"
#include "icon.h"
#include "scene.h"
#include "playerstats.h"
#include "endoflevelstats.h"
#include "highscores.h"
#include "events.h"
#include "frontendmain.h"
#include "Table.h"						// PP: Table page-item type - lets you line up items into a grid arrangement
#include "util.h"
#include "collectibles.h"				// PP: functions for creating and updating collectibles
#include "PageItemFX.h"					// PP: Special effects for page items
#include "Cheats.h"						// PP: Cheats

BOOK	enterLevelBook;

// JW: Need a text file entry for end and delete!!!

char *enternameCharacters[34] = 
{
	{"A"},{"B"},{"C"},{"D"},{"E"},{"F"},{"G"},{"H"},
	{"I"},{"J"},{"K"},{"L"},{"M"},{"N"},{"O"},{"P"},
	{"Q"},{"R"},{"S"},{"T"},{"U"},{"V"},{"W"},{"X"},
	{"Y"},{"Z"},{"_"},{"!"},{"."},{"#"},{"*"},{"?"},

};

int	totalCash = 0;

/*  --------------------------------------------------------------------------------
	Function	: CreateEnterLevelStats
	Purpose		: setup book for enter level stats
	Parameters	: level
	Returns		: 
	Info		: 
*/
void CreateEnterLevelStats(int level)
{
	PAGE	*mainpage;
	PAGE	*page;
	ICON	*icon;
	TEXTBOX	*collectibleTextbox,*postersTextbox,*bonusTextbox,*destructTextbox, *spareText;

	COLOUR	cmplCol = COLOUR(0, 128, 0, 128), incmplCol = COLOUR(96, 0, 0, 128);

	enterLevelBook.shutdown();
	enterLevelBook.init();

	// PP: make this book a spinny book
	enterLevelBook.setEffectChooser(&spinBookPIEChooser);

	// NH: 0.48 aligns it properly with the pause.
	enterLevelBook.setBookRect(RECTANGLE(-0.48f, 0.45f, -0.45f, 0.48f));

	mainpage = enterLevelBook.addPage("stats");

	// TP: TODO:	Alternate stats for inaccesable level

	mainpage->insertFreeItem(page = new PAGE("title"), RECTANGLE(-0.4f, 0.4f, 0.35f, 0.5f));

	// NH: Add Title
	page->insertItem(spareText = new TEXTBOX((EStringIndex)levelNameStrings[level]), false);
	spareText->setStyle(TS_title)
		->setYAlign(PIFLAG_YALIGN_TOP)
		->setSelectable(false);

	if (level == SCENE_ZOOBOSS)
	{
		PlaySample("elephant.wav", 255);

		mainpage->insertFreeItem(page = new PAGE("boss info"), RECTANGLE(-0.3f, 0.3f, -0.3f, 0.2f));

		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[SCENE_ZOOBOSS].flags & LEVELFLAG_SEENINTRO)
		{}	// JW: page->insertItem(spareText = new TEXTBOX(STR_FILLER));
		else if (AllowedToEnterLevel(level))
			page->insertItem(spareText = new TEXTBOX(STR_ZOOHUB_BOSSGAMESTAT_COMPLETE));
		else if (IsLevelComplete(SCENE_SAFARI) && map.lastScene == SCENE_SAFARI)
			page->insertItem(spareText = new TEXTBOX(STR_ZOOHUB_BOSSGAMESTAT_SAFARI));
		else if (IsLevelComplete(SCENE_ICEDOME) && map.lastScene == SCENE_ICEDOME)
			page->insertItem(spareText = new TEXTBOX(STR_ZOOHUB_BOSSGAMESTAT_ICEKING));
		else if (IsLevelComplete(SCENE_AQUA) && map.lastScene == SCENE_AQUA)
			page->insertItem(spareText = new TEXTBOX(STR_ZOOHUB_BOSSGAMESTAT_AQUA));
		else 
			page->insertItem(spareText = new TEXTBOX(STR_ZOOHUB_BOSSGAMESTAT_EMPTY));
			
		spareText->setWrap(true);
	}
	else if (level == SCENE_CITYBOSS)
	{
		mainpage->insertFreeItem(page = new PAGE("boss info"), RECTANGLE(-0.3f, 0.3f, -0.3f, 0.2f));

		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[SCENE_CITYBOSS].flags & LEVELFLAG_SEENINTRO)
		{}	// JW: 	page->insertItem(spareText = new TEXTBOX(STR_FILLER));
		else if (AllowedToEnterLevel(level))
			page->insertItem(spareText = new TEXTBOX(STR_CITYHUB_BOSSGAMESTAT_COMPLETE));
		else if (IsLevelComplete(SCENE_CONSTRUCT) && map.lastScene == SCENE_CONSTRUCT)
			page->insertItem(spareText = new TEXTBOX(STR_CITYHUB_BOSSGAMESTAT_CONSTRUCT));
		else if (IsLevelComplete(SCENE_MUSEUM) && map.lastScene == SCENE_MUSEUM)
			page->insertItem(spareText = new TEXTBOX(STR_CITYHUB_BOSSGAMESTAT_MUSEUM));
		else if (IsLevelComplete(SCENE_DEPTSTR) && map.lastScene == SCENE_DEPTSTR)
			page->insertItem(spareText = new TEXTBOX(STR_CITYHUB_BOSSGAMESTAT_DEPTSTORE));
		else 
			page->insertItem(spareText = new TEXTBOX(STR_CITYHUB_BOSSGAMESTAT_EMPTY));
			
		spareText->setWrap(true);
	}
	else if (level == SCENE_WESTBOSS)
	{
		mainpage->insertFreeItem(page = new PAGE("boss info"), RECTANGLE(-0.3f, 0.3f, -0.3f, 0.2f));

		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[SCENE_WESTBOSS].flags & LEVELFLAG_SEENINTRO)
		{}// JW: 	page->insertItem(spareText = new TEXTBOX(STR_FILLER));
		else if (AllowedToEnterLevel(level))
			page->insertItem(spareText = new TEXTBOX(STR_WESTHUB_BOSSGAMESTAT_COMPLETE));
		else if (IsLevelComplete(SCENE_GRANDCANYON) && map.lastScene == SCENE_GRANDCANYON)
			page->insertItem(spareText = new TEXTBOX(STR_WESTHUB_BOSSGAMESTAT_CANYON));
		else if (IsLevelComplete(SCENE_GHOSTTOWN) && map.lastScene == SCENE_GHOSTTOWN)
			page->insertItem(spareText = new TEXTBOX(STR_WESTHUB_BOSSGAMESTAT_GHOST));
		else if (IsLevelComplete(SCENE_GOLDMINE) && map.lastScene == SCENE_GOLDMINE)
			page->insertItem(spareText = new TEXTBOX(STR_WESTHUB_BOSSGAMESTAT_GOLDMINE));
		else 
			page->insertItem(spareText = new TEXTBOX(STR_WESTHUB_BOSSGAMESTAT_EMPTY));
			
		spareText->setWrap(true);
	}
	else
	{
		mainpage->insertFreeItem(page = new PAGE("title"), RECTANGLE(-0.3f, 0.3f, 0.1f, 0.35f));

		// TP: complete or not complete message
		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[level].completed)
		{
			// TP: level has been completed
			page->insertItem(spareText = new TEXTBOX(STR_LEVELSTATE_COMPLETE));
			//spareText->setYAlign(PIFLAG_YALIGN_TOP);
		}

		page->insertItem(spareText = new TEXTBOX(STR_HUB_DIFFICULTY_LEVEL));
		
		switch(level)
		{
		case SCENE_AQUA:
		case SCENE_CONSTRUCT:
		case SCENE_GOLDMINE:	page->insertItem(icon = new ICON("starating.bmp"),false);
								icon->setScale(0.4f);
								icon->setXAlign(PIFLAG_XALIGN_RIGHT);
								icon->tilt = false;

		case SCENE_ICEDOME:
		case SCENE_DEPTSTR:
		case SCENE_GHOSTTOWN:	page->insertItem(icon = new ICON("starating.bmp"),false);
								icon->setScale(0.4f);
								icon->setXPadding(false);
								icon->tilt = false;

		case SCENE_SAFARI:	
		case SCENE_MUSEUM:
		case SCENE_GRANDCANYON:	page->insertItem(icon = new ICON("starating.bmp"),false);
								icon->setScale(0.4f);
								icon->setXAlign(PIFLAG_XALIGN_LEFT);
								icon->tilt = false;
		}

		mainpage->insertFreeItem(page = new PAGE("level stats"), RECTANGLE(-0.3f, 0.3f, -0.5f, 0.1f));
		
		// TP: poster
		page->insertItem(icon = new ICON("tazwanted.bmp"),true);
		icon->setAlignScale(ENDOFLEVELSTATS_ICONSCALE);
		icon->setDrawScale(ENDOFLEVELSTATS_ICONSCALE);
		icon->setXAlign(PIFLAG_XALIGN_LEFT);
		page->insertItem(postersTextbox = new TEXTBOX(),false);
		postersTextbox->sprintf8("%d/7",gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[level].postersCollected);
		postersTextbox->setXAlign(PIFLAG_XALIGN_RIGHT);

		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[level].postersCollected == 7)
			postersTextbox->setColour(cmplCol, cmplCol);
		else
			postersTextbox->setColour(incmplCol, incmplCol);

		// JW: destructibles
		page->insertItem(icon = new ICON("destructibles02.bmp"),true);
		icon->setAlignScale(ENDOFLEVELSTATS_ICONSCALE);
		icon->setDrawScale(ENDOFLEVELSTATS_ICONSCALE);
		icon->setXAlign(PIFLAG_XALIGN_LEFT);
		page->insertItem(destructTextbox = new TEXTBOX(),false);
		
		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].difficulty == DIFF_EASY)
		{
			if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[level].objectsDestroyed >= 50)
			{
				destructTextbox->setText(STR_LEVELSTATE_COMPLETE);

				destructTextbox->setColour(cmplCol, cmplCol);
			}
			else
			{
				destructTextbox->sprintf8("%d%%", gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[level].objectsDestroyed);

				destructTextbox->setColour(incmplCol, incmplCol);
			}
		}
		else if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].difficulty == DIFF_MEDIUM)
		{
			if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[level].objectsDestroyed >= 75)
			{
				destructTextbox->setText(STR_LEVELSTATE_COMPLETE);

				destructTextbox->setColour(cmplCol, cmplCol);
			}
			else
			{
				destructTextbox->sprintf8("%d%%", gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[level].objectsDestroyed);

				destructTextbox->setColour(incmplCol, incmplCol);
			}
		}
		else if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].difficulty == DIFF_HARD)
		{
			if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[level].objectsDestroyed >= 100)
			{
				destructTextbox->setText(STR_LEVELSTATE_COMPLETE);

				destructTextbox->setColour(cmplCol, cmplCol);
			}
			else
			{
				destructTextbox->sprintf8("%d%%", gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[level].objectsDestroyed);

				destructTextbox->setColour(incmplCol, incmplCol);
			}
		}
	
		destructTextbox->setXAlign(PIFLAG_XALIGN_RIGHT);

		// JW: bonus
		page->insertItem(icon = new ICON("samface.bmp"),true);
		icon->setAlignScale(ENDOFLEVELSTATS_ICONSCALE);
		icon->setDrawScale(ENDOFLEVELSTATS_ICONSCALE);
		icon->setXAlign(PIFLAG_XALIGN_LEFT);
		page->insertItem(bonusTextbox = new TEXTBOX(),false);
		bonusTextbox->setXAlign(PIFLAG_XALIGN_RIGHT);
		
		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[level].secretItemCollected)
		{
			bonusTextbox->setText(STR_FOUND);

			bonusTextbox->setColour(cmplCol, cmplCol);
		}
		else
		{
			bonusTextbox->setText(STR_NOT_FOUND);

			bonusTextbox->setColour(incmplCol, incmplCol);
		}

		// TP: sandwichs
		page->insertItem(icon = new ICON("sandwich.bmp"),true);
		icon->setAlignScale(ENDOFLEVELSTATS_ICONSCALE);
		icon->setDrawScale(ENDOFLEVELSTATS_ICONSCALE);
		icon->setXAlign(PIFLAG_XALIGN_LEFT);
		page->insertItem(collectibleTextbox = new TEXTBOX(),false);
		collectibleTextbox->setXAlign(PIFLAG_XALIGN_RIGHT);

		collectibleTextbox->sprintf8("%d/%d",gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[level].pickupsCollected, NUM_PICKUPS_EACH_LEVEL);

		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[level].pickupsCollected >= NUM_PICKUPS_EACH_LEVEL)
			collectibleTextbox->setColour(cmplCol, cmplCol);
		else
			collectibleTextbox->setColour(incmplCol, incmplCol);
	}

	enterLevelBook.open();
}

/* --------------------------------------------------------------------------------
   Function : CreateStatsAtEndOfLevel
   Purpose : creates structures used to draw end of level stats for single player game
   Parameters : options is true if this stats screen is for the options menu
   Returns : 
   Info : 
*/
int CreateStatsAtEndOfLevel(PLAYERSTATS *stats,int options,int fromFrontEnd)
{
	BOOK	*ptr;
	PAGE	*mainpagePtr, *pagePtr,*subpage;
	ICON	*iconPtr;
	TEXTBOX	*textPtr;

	char	time[16];

	totalCash = 0;

	// PP: NOTE: this'ere endOfLevelStatsBook gets closed-and-freed in ExitPauseMode
	if (gameStatus.player[0].stats->endOfLevelStatsBook)
	{
// TP: ASSERT(0);
		SAFE_DELETE(gameStatus.player[0].stats->endOfLevelStatsBook);
	}
	ptr = gameStatus.player[0].stats->endOfLevelStatsBook = new BOOK;
	
	///if (!frontEndMenu.frontEndMenuBook)
	//	ptr->setBackgroundColour(COLOUR(0, 0, 0, 64));
	// PP: REMOUT: books now default to filling the full 'safe' area of the screen		ptr->setBookRect(RECTANGLE(-0.5f, 0.5f, -0.5f, 0.5f));// PP: REMOUT: must now be expressed as fractions of the screen dimensions		RECTANGLE(-videoMode.xScreen/2.0f,videoMode.xScreen/2.0f,-videoMode.yScreen/2.0f,videoMode.yScreen/2.0f));

	stats->endOfLevelStatsPage = mainpagePtr = ptr->addPage("stats");

	if(options == 0 && map.sceneNumber > SCENE_PLAYPEN)
	{
		mainpagePtr->insertItem(stats->levelNameTextbox = new TEXTBOX((EStringIndex) levelNameStrings[map.sceneNumber]));
		
		stats->levelNameTextbox->setScale(ENDOFLEVELSTATS_TEXTSCALE);
		stats->levelNameTextbox->setYAlign(PIFLAG_YALIGN_TOP);
		stats->levelNameTextbox->setStyle(TS_title);// PP: set 'Title' style for the title
	}

	if(options == 0)
	{
// PP: REMOUT: not too sure what's going on here, but this isn't safe		subpage->setRect(RECTANGLE(-0.35f, 0.35f, -0.4f, 0.3f));
		mainpagePtr->insertFreeItem(subpage = new PAGE("stats grid"), RECTANGLE(-0.4f, 0.4f, -0.4f, 0.33f));

		//subpage->setDrawScale(1.1f);
	}
	else
	{
		mainpagePtr->insertFreeItem(subpage = new PAGE("stats grid"), RECTANGLE(-0.35f, 0.35f, -0.4f, 0.35f));

		subpage->setDrawScale(1.1f);
	}

	// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//->setXPadding(false);

	//->setAlignScale(ENDOFLEVELSTATS_ICONSCALE * 1.2)

#define ENDOFLEVELSTATS_ICON_ALIGN_SCALE	(ENDOFLEVELSTATS_ICONSCALE*1.45f)// PP: was 1.3	// PP: TODO: allow for separate X & Y PAGEITEM scaling

#define NORMAL_STATS_ROW(_icon, _info) \
	subpage->insertItem(iconPtr = new ICON(_icon)) \
		->setDrawScale(ENDOFLEVELSTATS_ICONSCALE) \
		->setAlignScale(ENDOFLEVELSTATS_ICON_ALIGN_SCALE) \
		->setXPadding(false) \
		->setXAlign(PIFLAG_XALIGN_LEFT); \
	\
	subpage->insertItem(stats->##_info##Scorebox = new TEXTBOX(),false) \
		->setXAlign(PIFLAG_XALIGN_LEFT) \
		->setScale(ENDOFLEVELSTATS_TEXTSCALE); \
	\
	subpage->insertItem(stats->##_info##Cashbox = new TEXTBOX(),false) \
		->setXAlign(PIFLAG_XALIGN_RIGHT) \
		->setScale(ENDOFLEVELSTATS_TEXTSCALE);

	NORMAL_STATS_ROW("tazwanted.bmp",		posters);
#ifndef CONSUMERDEMO
	NORMAL_STATS_ROW("captured.bmp",		captured);
#endif
	NORMAL_STATS_ROW("destructibles02.bmp",	destructible);

#ifndef CONSUMERDEMO
	NORMAL_STATS_ROW("samface.bmp",			hidden);
#endif

	NORMAL_STATS_ROW("dollar.bmp",			money);

#ifndef CONSUMERDEMO
	// PP: set text for money score box
	stats->capturedScorebox->setText(STR_STATS_CAPTURED);
#endif

	stats->moneyScorebox->setText(STR_MP_TOTAL);

	// PP: divide the last row into two equal halves, each containing a subpage

	PAGE	*lastRowSubPage, *leftSubPage, *rightSubPage;

	subpage->insertItem(lastRowSubPage = new PAGE());
	/*lastRowSubPage->setYPadding(false);*/

	// PP: NOTE: I fixed a bug that was stopping this last row's height
	// PP:			from being properly calculated; it should be fine now.

/*

	lastRowSubPage->insertItem(leftSubPage=new PAGE());
	lastRowSubPage->insertItem(rightSubPage=new PAGE(), false);

	/*/

	lastRowSubPage->insertFreeItem(leftSubPage=new PAGE(), UORECT.leftHalf());
	lastRowSubPage->insertFreeItem(rightSubPage=new PAGE(), UORECT.rightHalf());

  //*/

	// PP: left subpage: sandwich icon and collectible percentage

	leftSubPage->insertItem(iconPtr = new ICON("sandwich.bmp"))
		->setDrawScale(ENDOFLEVELSTATS_ICONSCALE)
		->setAlignScale(ENDOFLEVELSTATS_ICON_ALIGN_SCALE)
		->setXAlign(PIFLAG_XALIGN_LEFT)
		->setXPadding(false);

	leftSubPage->insertItem(stats->collectibleScorebox = new TEXTBOX(),false)
		->setXAlign(PIFLAG_XALIGN_LEFT)
		->setScale(ENDOFLEVELSTATS_TEXTSCALE);

	// PP: right subpage: clock icon and timer

	rightSubPage->insertItem(iconPtr = new ICON("clock.bmp"))
		->setDrawScale(ENDOFLEVELSTATS_ICONSCALE)
		->setAlignScale(ENDOFLEVELSTATS_ICON_ALIGN_SCALE)
		->setXAlign(PIFLAG_XALIGN_RIGHT);

	rightSubPage->insertItem(stats->gameTimeScorebox = new TEXTBOX(),false)
		->setXAlign(PIFLAG_XALIGN_RIGHT)
		->setXPadding(false)
		->setScale(ENDOFLEVELSTATS_TEXTSCALE);

	utilGetPrettyTime(time, gameStatus.playTime);

	stats->gameTimeScorebox->sprintf8("%s", time);

	//stats->gameTimeScorebox->sprintf8("0:00:00");

	// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	//pagePtr->insertItem(stats->rankTextbox = new TEXTBOX(stats->displayedRank), true);

	if (options == 0)
	{
		mainpagePtr->insertItem(textPtr = new TEXTBOX(STR_DIALOGUE_CROSSOK),true);

		textPtr->setYAlign(PIFLAG_YALIGN_BOTTOM);

		textPtr->setStyle(TS_control);// PP: set 'Control' style for the control footer

		if (!fromFrontEnd)
			ptr->open();
	}
	if(options == 2)
	{
		mainpagePtr->addRow();
//		pagePtr->insertItem(new TEXTBOX(STR_PRESS_X_TO_ENTER_TAZ), true);
//		pagePtr->insertItem(new TEXTBOX(STR_PRESS_Y_TO_RETURN_TO_HUB), true);
	}

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : CreateStatsAtEndOfLevel
   Purpose : creates structures used to draw end of level stats for single player game
   Parameters : 
   Returns : 
   Info : 
*/

void CreateNameEntryPageAtEndOfLevel(PLAYERSTATS *stats)
{
	BOOK	*bookPtr;
	PAGE	*pagePtr;
	PAGE	*subpagePtr;
	TABLE	*tablePtr;
	ICON	*iconPtr;
	TEXTBOX *textBox;
	ICON	*ptr;
	
	int	i;

//	bookPtr = stats->cheatsBook = new BOOK();
	
//	pagePtr = stats->cheatsPage = bookPtr->addPage("CheatsPage");
	
//	pagePtr->insertItem(stats->cheatsTextbox = new TEXTBOX("MONLKEY"));

	bookPtr = stats->endOfLevelStatsBook;

	pagePtr = stats->highscoreSelectIcon = bookPtr->addPage("SelectIcon");
	pagePtr->setNavigation(true);
	
	pagePtr->insertItem(textBox = new TEXTBOX(STR_CHOOSE_ICON));
	textBox->setStyle(TS_title)
		->setYAlign(PIFLAG_YALIGN_TOP);
	textBox->setSelectable(false);

	// PP: number of columns in the icon grid
#define ICONGRID_NUM_COLUMNS		5

	pagePtr->insertItem(tablePtr = new TABLE(ICONGRID_NUM_COLUMNS))
			->setName("Icon Grid");
	tablePtr->setNavigation(true);
	stats->iconGridPage = (PAGE*)tablePtr;

	for (i = 0 ; i < NUM_HSE_ICONS; i++)
	{
		tablePtr->insertItem(stats->chooseIcon[i] = new ICON(highScoreIconNames[i]));

		stats->chooseIcon[i]->tilt = false;
	}

	i = gameStatus.globalSettings.currentSaveSlot;
	
	// NH: Test subpagePtr->selectItem(stats->chooseIcon[gameStatus.globalSettings.saveInfo.saveSlot[i].lastIcon]);
	if(gameStatus.globalSettings.saveInfo.saveSlot[i].lastIcon != -1)
		pagePtr->selectItem(stats->chooseIcon[gameStatus.globalSettings.saveInfo.saveSlot[i].lastIcon]);
	else
		pagePtr->selectItem(stats->chooseIcon[0]);
	//stats->chooseIcon[0]->tilt = true;

	// NH: INFO
	// NH: Add Info Text Sub Book
	pagePtr->insertItem(stats->iconInfoSubBook=new BOOK("INFO SUB BOOK"));
	stats->iconInfoSubBook->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setSelectable(false);

	// NH: Add Info Text (Using Macro)
#define ADD_ICONINFOSUBBOOK_PAGE(name, infoText) \
	subpagePtr=stats->iconInfoSubBook->addPage(#name); \
	subpagePtr->insertItem(infoText) \
		->setStyle(TS_info) \
		->setYAlign(PIFLAG_YALIGN_BOTTOM) \
		->setAlignScale(1.2f) \
		->setSelectable(false);

	// NH: Add all info pages (using the macro).  Seperate pages allow for fading between the actions
	ADD_ICONINFOSUBBOOK_PAGE(NAME,STR_CHOOSE_ICON);

	// NH: CONTROL
	// NH: Add Control Text Sub Book
	pagePtr->insertItem(stats->iconControlSubBook=new BOOK("CONTROL SUB BOOK"));
	stats->iconControlSubBook->setYPadding(false)
		->setSelectable(false);

	// NH: Add Control Text (Using Macro)
#define ADD_ICONCONTROLSUBBOOK_PAGE(name, controlText) \
	subpagePtr=stats->iconControlSubBook->addPage(#name); \
	subpagePtr->insertItem(controlText) \
		->setStyle(TS_control) \
		->setYPadding(false) \
		->setSelectable(false);

	if (map.sceneNumber == SCENE_FRONTEND)
	{
		// NH: Add all control pages (using the macro).  Seperate pages allow for fading between the actions
		ADD_ICONCONTROLSUBBOOK_PAGE(SELECT_OR_BACK,STR_X_TO_SELECT_Y_TO_RETURN);
	}
	else
	{
		ADD_ICONCONTROLSUBBOOK_PAGE(SELECT,STR_PRESS_X_TO_CONTINUE);
	}

	stats->iconInfoSubBook->open();// PP: books default to closed
	stats->iconControlSubBook->open();

/*
	pagePtr->insertItem(textBox = new TEXTBOX(STR_PRESS_X_TO_CONTINUE));
	textBox->setYAlign(PIFLAG_YALIGN_BOTTOM);
	textBox->setSelectable(false);
*/

	pagePtr = stats->highscoreEnterName = bookPtr->addPage("EnterName");
	pagePtr->setNavigation(true);

	// PP: navigation Y-wrapping is allowed, but only in response to a debounced input, NOT an auto-repeat input
	pagePtr->setNavigationWrap(PAGEFLAG_NAVWRAP_YDEBOUNCE);
			
	pagePtr->insertItem(textBox = new TEXTBOX(STR_ENTER_INITIALS));
	textBox->setStyle(TS_title)
		->setYAlign(PIFLAG_YALIGN_TOP);
	textBox->setSelectable(false);

	pagePtr->insertItem(subpagePtr = new PAGE("Icon&Name"));

	subpagePtr->insertItem(iconPtr = new ICON())
		->setSelectable(false);

	stats->chosenIcon = iconPtr;

	subpagePtr->insertItem(textBox = new TEXTBOX(), false)
		->setSelectable(false);
	stats->enterName = textBox;

	// PP: put the current three characters into the textbox so's it knows how wide it is when the book gets aligned
	stats->enterName->sprintf8("    %c %c %c", stats->letters[0], stats->letters[1], stats->letters[2]);

	stats->enterName->setFontSize(2.0f);

	// PP: wrap the sub-page tightly round its contents; pack the contents together as tighly as possible.  The page now becomes an alignable item like any other.
	subpagePtr->setSlack(false);

	// PP: 
	subpagePtr->setSelectable(false);

	// PP: ~~~~~~
	// PP: NOTE: I'VE PUT THESE LETTERS IN A TABLE BECAUSE IT'S THE TOOL FOR THE JOB
	// PP: THIS LETS US USE THE FANCIER NAVIGATION WRAPPING MODES - 'NEWLINE' AND 'NEWCOLUMN'
	// PP: I HOPE YOU DON'T MIND
	// PP: ~~~~~~

	// PP: number of columns in the letter grid
#define LETTERGRID_NUM_COLUMNS		8

	pagePtr->insertItem(tablePtr = new TABLE(LETTERGRID_NUM_COLUMNS))->setName("Letter Grid");
	tablePtr->setNavigation(true);

	stats->letterGridPage = tablePtr;

	for (i = 0; i < 32; i++)
	{
		tablePtr->insertItem(stats->chooseLetter[i] = new TEXTBOX());

		stats->chooseLetter[i]->setFontSize(1.4f);
		stats->chooseLetter[i]->sprintf8(enternameCharacters[i]);

		// PP: also set name to match text - kinda handy
		stats->chooseLetter[i]->setName(enternameCharacters[i]);
	}

	// JW: Enter
	pagePtr->insertItem(stats->chooseLetter[32] = new TEXTBOX());
	stats->chooseLetter[32]->setFontSize(1.4f);
	stats->chooseLetter[32]->setText(STR_NAME_ENTRY_END);
	stats->chooseLetter[32]->setName("Enter");

	// JW: Delete
	pagePtr->insertItem(stats->chooseLetter[33] = new TEXTBOX(),false);
	stats->chooseLetter[33]->setFontSize(1.4f);
	stats->chooseLetter[33]->setText(STR_NAME_ENTRY_DELETE);
	stats->chooseLetter[33]->setName("Delete");

#ifndef DEFAULT_INITIALS// PP: only grey-out the 'del' button if default initials aren't to be used
	stats->chooseLetter[33]->setSelectable(false);
	stats->chooseLetter[33]->setColour(COLOUR(96, 96, 96, 128), COLOUR(32, 32, 32, 128));
#endif// PP: def DEFAULT_INITIALS

	pagePtr->selectItem(stats->chooseLetter[0]);

	// NH: INFO
	// NH: Add Info Text Sub Book
	pagePtr->insertItem(stats->nameInfoSubBook=new BOOK("INFO SUB BOOK"));
	stats->nameInfoSubBook->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setSelectable(false);

	// NH: Add Info Text (Using Macro)
#define ADD_NAMEINFOSUBBOOK_PAGE(name, infoText) \
	subpagePtr=stats->nameInfoSubBook->addPage(#name); \
	subpagePtr->insertItem(infoText) \
		->setStyle(TS_info) \
		->setYAlign(PIFLAG_YALIGN_BOTTOM) \
		->setAlignScale(1.2f) \
		->setSelectable(false);

	// NH: Add all info pages (using the macro).  Seperate pages allow for fading between the actions
	ADD_NAMEINFOSUBBOOK_PAGE(NAME,STR_ENTER_INITIALS);

	// NH: CONTROL
	// NH: Add Control Text Sub Book
	pagePtr->insertItem(stats->nameControlSubBook=new BOOK("CONTROL SUB BOOK"));
	stats->nameControlSubBook->setYPadding(false)
		->setSelectable(false);

	// NH: Add Control Text (Using Macro)
#define ADD_NAMECONTROLSUBBOOK_PAGE(name, controlText) \
	subpagePtr=stats->nameControlSubBook->addPage(#name); \
	subpagePtr->insertItem(controlText) \
		->setStyle(TS_control) \
		->setYPadding(false) \
		->setSelectable(false);

	// NH: Add all control pages (using the macro).  Seperate pages allow for fading between the actions
	ADD_NAMECONTROLSUBBOOK_PAGE(SELECT_OR_BACK,STR_X_TO_SELECT_Y_TO_RETURN);

	stats->nameInfoSubBook->open();// PP: books default to closed
	stats->nameControlSubBook->open();


/*
	pagePtr->insertItem(textBox = new TEXTBOX(STR_PRESS_X_TO_CONTINUE));
	textBox->setYAlign(PIFLAG_YALIGN_BOTTOM);
	textBox->setSelectable(false);

	pagePtr->insertItem(textBox = new TEXTBOX(STR_PRESS_Y_TO_CANCEL));
	textBox->setYAlign(PIFLAG_YALIGN_BOTTOM);
	textBox->setSelectable(false);
*/

	// PP: ======== PAGES HAVE BEEN ADDED TO THE BOOK WHILE IT IS OPEN ========
	// PP: NOTE: I don't recommend adding anything into a book while it's open!
	// PP: The book must be realigned now.
	bookPtr->realign();
}

/* --------------------------------------------------------------------------------
   Function : CreateHighScoreTablePage
   Purpose : creates a page containing a high scores table for a scene
   Parameters : 
   Returns : 
   Info : 
*/

void CreateHighScoreTablePage(PLAYERSTATS *stats, EScene currentScene)
{
	int				i;
	PAGE			*pagePtr;
	PAGE			*subpagePtr;
	TABLE			*tablePtr;
	ICON			*iconPtr;
	TEXTBOX			*textBox;
	ELevelScores	level;
	int				hours = 0;
	int				minutes = 1;
	int				seconds = 2;
	int				tempInt;
	float			tempFloat;
	char			tempString[16];

	level = ConvertSceneRefToLevelScoreRef(currentScene);

	pagePtr = stats->topTenPage = stats->endOfLevelStatsBook->addPage("highScores");

	pagePtr->insertItem(textBox = new TEXTBOX(STR_HIGH_SCORE_TITLE));
	textBox->setYAlign(PIFLAG_YALIGN_TOP);
	textBox->setStyle(TS_title);// PP: set 'Title' style for the title

	pagePtr->insertFreeItem(tablePtr = new TABLE("High Scores Grid"), RECTANGLE(-0.5f, 0.5f, -0.25f, 0.25));// PP: RECTANGLE(-0.4f, 0.4f, -0.25f, 0.25));

	for(i=0;i<5;i++)
	{
		//icon
		tablePtr->insertItem(iconPtr = new ICON(highScoreIconNames[gameStatus.globalSettings.saveInfo.highScores[level].iconRef[i]]),true);
		iconPtr->setXAlign(PIFLAG_XALIGN_RIGHT);// PP: 
		iconPtr->setScale(0.8f);// PP: I think this works better
		// PP: REMOUT: iconPtr->setDrawScale(0.5);
		
		//name
		tablePtr->insertItem(textBox = new TEXTBOX(),false);
		textBox->sprintf8("%c %c %c",gameStatus.globalSettings.saveInfo.highScores[level].names[i][0],gameStatus.globalSettings.saveInfo.highScores[level].names[i][1],gameStatus.globalSettings.saveInfo.highScores[level].names[i][2]);
		textBox->setScale(1.2f);
		
		//time
		tablePtr->insertItem(textBox = new TEXTBOX(),false);
		utilGetPrettyTime(tempString, gameStatus.globalSettings.saveInfo.highScores[level].time[i]);
		textBox->sprintf8("%s", tempString);
		textBox->setScale(1.2f);
		
		//score
		tablePtr->insertItem(textBox = new TEXTBOX(),false);
		utilFormatPrettyNumbers(gameStatus.globalSettings.saveInfo.highScores[level].money[i], tempString);
		textBox->sprintf8("$%s", tempString);
		textBox->setScale(1.2f);

		//rank
		//tablePtr->insertItem(textBox = new TEXTBOX(gameStatus.globalSettings.saveInfo.highScores[level].rank[i]),false);
		//textBox->setScale(0.8f);
	}

	pagePtr->insertItem(textBox = new TEXTBOX(STR_DIALOGUE_CROSSOK));
	textBox->setYAlign(PIFLAG_YALIGN_BOTTOM);
	textBox->setStyle(TS_control);// PP: set 'Control' style for the control footer

	//copy stats to save structure in global settings
	if((tempInt = map.mapStats.numPosters) > gameStatus.globalSettings.saveInfo.stats[level].totalPosters)
	{
		gameStatus.globalSettings.saveInfo.stats[level].totalPosters = tempInt;
	}
	if((tempInt = gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[currentScene].postersCollected) > gameStatus.globalSettings.saveInfo.stats[level].postersTriggered)
	{
		gameStatus.globalSettings.saveInfo.stats[level].postersTriggered = tempInt;
	}
	if((tempFloat = ((float)stats->numObjectsDestroyed/(float)map.mapStats.numDestructibles)*100.0f) > gameStatus.globalSettings.saveInfo.stats[level].destructibles)
	{
		gameStatus.globalSettings.saveInfo.stats[level].destructibles = tempFloat;
	}

	if((tempInt = gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[currentScene].pickupsCollected) > gameStatus.globalSettings.saveInfo.stats[level].collectibles)
	{
		gameStatus.globalSettings.saveInfo.stats[level].collectibles = tempInt;
	}

	if((tempInt = stats->bountyDisplay) > gameStatus.globalSettings.saveInfo.stats[level].money)
	{
		gameStatus.globalSettings.saveInfo.stats[level].money = tempInt;
	}
	if(gameStatus.globalSettings.saveInfo.stats[level].time == 0)
		gameStatus.globalSettings.saveInfo.stats[level].time = stats->time;
	else
	{
		if(stats->time < gameStatus.globalSettings.saveInfo.stats[level].time)
			gameStatus.globalSettings.saveInfo.stats[level].time = stats->time;
	}
	//mark level as being completed
	//globaly for rankings
	gameStatus.globalSettings.saveInfo.stats[level].levelCompleted = true;
	//and for the current save slot
	if(gameStatus.globalSettings.currentSaveSlot != -1)
	{
		gameStatus.globalSettings.saveInfo.saveSlot[(int)gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].completed = true;
	}

	// PP: NOTE: The book was open when the page was added to it.
	// PP: I recommend that we avoid adding contents into a book while it's open.
	// PP: The book must be manually realigned now.
	stats->endOfLevelStatsBook->realign();
}

void CreateInformPageAtEndOfLevel(PLAYERSTATS *stats)
{
	BOOK	*bookPtr;
	PAGE	*pagePtr;
	ICON	*iconPtr;
	TEXTBOX	*textBox;

	bookPtr = stats->endOfLevelStatsBook;

	pagePtr = stats->highscoreSelectIcon = bookPtr->addPage("GalleryInform");

	pagePtr->insertItem(textBox = new TEXTBOX(STR_GALLERY_UNLOCKED));
	textBox->setWrap(true);
	textBox->setScale(1.5);

	pagePtr->insertItem(iconPtr = new ICON("tazbiglogo.bmp"));
	iconPtr->setScale(2.5);

	pagePtr->insertItem(textBox = new TEXTBOX(STR_DIALOGUE_CROSSOK));
	textBox->setYAlign(PIFLAG_YALIGN_BOTTOM);
	textBox->setSelectable(false);

	stats->endOfLevelStatsBook->realign();

// PP: REMOUT: probably not needed now (PAGES now align their contents when they are opened for the first time)			bookPtr->realign();
}

/* --------------------------------------------------------------------------------
   Function : UpdateStatsAtEndOfLevel
   Purpose : updates structures used to draw end of level stats for single player game
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateStatsAtEndOfLevel(PLAYERSTATS *stats)
{
	int			destructibles,food;		// CMD: percentage complete
	int			posterbonus = posterBounties[map.sceneNumber], levelTotal;
	char		tempString[16];
	COLOUR		cmplCol = COLOUR(0, 127, 0, 127), incmplCol = COLOUR(127, 0, 0, 127);

	if((stats->endOfLevelStatsBook)&&(stats->endOfLevelStatsBook->getCurrentPage() == stats->endOfLevelStatsPage))
	{
		switch(stats->statsBookState)
		{
		case OVERLAYSTATE_ARRIVING:
			if((stats->statsBookClock -= fTime) < 0.0f)
			{
				stats->statsBookState = OVERLAYSTATE_ONSCREEN;
//				stats->endOfLevelStatsPage->setAlignScale(1.0f);
//				stats->endOfLevelStatsPage->setDrawScale(1.0f);
				break;
			}

//			stats->statsBookScale = 1.0f - (bmCos(RAD(stats->statsBookClock*720.0f))*bmCos(RAD(stats->statsBookClock*30.0f)));
//			stats->endOfLevelStatsPage->setDrawScale(stats->statsBookScale);
			break;
		case OVERLAYSTATE_ONSCREEN:
			if(stats->numPostersDisplayed < (float)gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].postersCollected)
			{
				if((stats->numPostersDisplayed += (ENDOFLEVELCOUNTUPRATE*fTime)) > (float)gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].postersCollected)
				{
					stats->numPostersDisplayed = (float)gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].postersCollected;
				}
			}
			else
			if(stats->numCashDisplayedEndOfLevel < stats->bountyDisplay)
			{
				if((stats->numCashDisplayedEndOfLevel += (int)(ENDOFLEVELCOUNTUPRATE*fTime*25000.0f)) > stats->bountyDisplay)
				{
					stats->numCashDisplayedEndOfLevel = stats->bountyDisplay;
				}
			}
			else
			if(stats->numCollectiblesDisplayed < (float)gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].pickupsCollected)
			{
				if((stats->numCollectiblesDisplayed += (ENDOFLEVELCOUNTUPRATE*fTime)) > (float)gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].pickupsCollected)
				{
					stats->numCollectiblesDisplayed = (float)gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].pickupsCollected;
				}
			}
			else
			if(stats->numDestructiblesDisplayed < (float)stats->numObjectsDestroyed)
			{
				if((stats->numDestructiblesDisplayed += (ENDOFLEVELCOUNTUPRATE*fTime)) > (float)stats->numObjectsDestroyed)
				{
					stats->numDestructiblesDisplayed = (float)stats->numObjectsDestroyed;
				}
			}
			else
			{
				if(stats->displayedRank < CalculateRankFromFloat(CalculateRankAtEndOfLevel(stats)))
				{
					if((stats->rankClock += fTime) > 0.5f)
					{
						stats->rankClock = 0.0f;
						stats->displayedRank = (EStringIndex)((int)(stats->displayedRank)+1);
					}
				}
			}
			break;
		}

//		if(map.mapStats.numPosters)
//		{
//			posters = (int)(stats->numPostersDisplayed / (float)map.mapStats.numPosters * 100.0f);
//		}
//		else posters = (int)100.0f;
//		if(map.mapStats.numCollectibles)
//		{
//			collectibles = (int)(stats->numCollectiblesDisplayed / (float)map.mapStats.numCollectibles * 100.0f);
//		}
//		else collectibles = (int)100.0f;
		/*if(!gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].destructBonusCollected && map.mapStats.numDestructibles)
		{
			destructibles = (int)(stats->numObjectsDestroyed / (float)map.mapStats.numDestructibles * 100.0f);
		}
		else destructibles = (int)100.0f;*/

		destructibles = gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].objectsDestroyed;

		if(map.mapStats.numFood)
		{
			food = (int)(stats->numFoodDisplayed / (float)map.mapStats.numFood * 100.0f);
		}
		else food = (int)100.0f;

		stats->postersScorebox->sprintf8("%d/%d",gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].postersCollected,map.mapStats.numPosters);
		stats->collectibleScorebox->sprintf8("%d/%d",gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].pickupsCollected, NUM_PICKUPS_EACH_LEVEL);
		
		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].difficulty == DIFF_EASY)
		{
			stats->destructibleScorebox->sprintf8("%d%% (50%%)",destructibles);
		}
		else if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].difficulty == DIFF_MEDIUM)
		{			
			stats->destructibleScorebox->sprintf8("%d%% (75%%)",destructibles);
		}
		else if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].difficulty == DIFF_HARD)
		{
			stats->destructibleScorebox->sprintf8("%d%% (100%%)",destructibles);
		}
		
		// JW: Show score for posters kaboomed

		utilFormatPrettyNumbers(posterbonus, tempString);
		stats->postersCashbox->sprintf8("%d x $%s", gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].postersCollected, tempString);

		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].postersCollected >= map.mapStats.numPosters)
			stats->postersScorebox->setColour(cmplCol, cmplCol);
		else
			stats->postersScorebox->setColour(incmplCol, incmplCol);

#ifndef CONSUMERDEMO
		// JW: Show score deduction for the number of times caught

		utilFormatPrettyNumbers(abs(gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].timesCaptured), tempString);

		stats->capturedCashbox->sprintf8("- $%s", tempString);
#endif

		// JW: Show cash score for destructibles

		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].difficulty == DIFF_EASY)
		{
			if (destructibles >= 50)
			{
				stats->destructibleScorebox->setText(STR_LEVELSTATE_COMPLETE);

				levelTotal = bmFloatToInt(posterbonus * 0.5f);

				stats->destructibleScorebox->setColour(cmplCol, cmplCol);
			}
			else
			{
				levelTotal = 0;

				stats->destructibleScorebox->setColour(incmplCol, incmplCol);
			}
		}
		else if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].difficulty == DIFF_MEDIUM)
		{
			if (destructibles >= 75)
			{
				stats->destructibleScorebox->setText(STR_LEVELSTATE_COMPLETE);

				levelTotal = bmFloatToInt(posterbonus * 0.75f);

				stats->destructibleScorebox->setColour(cmplCol, cmplCol);
			}
			else
			{
				levelTotal = 0;

				stats->destructibleScorebox->setColour(incmplCol, incmplCol);
			}
		}
		else if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].difficulty == DIFF_HARD)
		{
			if (destructibles >= 100)
			{
				stats->destructibleScorebox->setText(STR_LEVELSTATE_COMPLETE);
		
				levelTotal = posterbonus;

				stats->destructibleScorebox->setColour(cmplCol, cmplCol);
			}
			else
			{
				levelTotal = 0;

				stats->destructibleScorebox->setColour(incmplCol, incmplCol);
			}
		}

		utilFormatPrettyNumbers(levelTotal, tempString);
		stats->destructibleCashbox->sprintf8("$%s", tempString);

		// JW: Show score for collecting secret item

#ifndef CONSUMERDEMO
		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].secretItemCollected)
		{
			utilFormatPrettyNumbers(posterbonus * 2, tempString);
			stats->hiddenCashbox->sprintf8("$%s", tempString);
			stats->hiddenScorebox->setText(STR_FOUND);
			stats->hiddenScorebox->setColour(cmplCol, cmplCol);
			
			levelTotal += posterbonus * 2;
		}
		else
		{
			stats->hiddenCashbox->sprintf8("$%d", 0);
			stats->hiddenScorebox->setText(STR_NOT_FOUND);
			stats->hiddenScorebox->setColour(incmplCol, incmplCol);
		}
#endif

		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].pickupsCollected >= NUM_PICKUPS_EACH_LEVEL)
			stats->collectibleScorebox->setColour(cmplCol, cmplCol);
		else
			stats->collectibleScorebox->setColour(incmplCol, incmplCol);

		// JW: Calculate grand total and whirl the dollar score

		levelTotal += gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].postersCollected * posterbonus;

#ifndef CONSUMERDEMO
		levelTotal += gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].timesCaptured;
#endif

		utilFormatPrettyNumbers(levelTotal, tempString);

		stats->moneyCashbox->sprintf8("$%s",tempString);

		// JW: print time

		utilGetPrettyTime(tempString,gameStatus.playTime);

		stats->gameTimeScorebox->sprintf8(" %s",tempString);

		stats->bountyDisplay = levelTotal;
		
		//stats->rankTextbox->setText(stats->displayedRank);
	}
#ifndef CONSUMERDEMO
	else 
	if((stats->endOfLevelStatsBook)&&(!frontEndMenu.frontEndMenuBook)&&(stats->endOfLevelStatsBook->getCurrentPage() == stats->highscoreSelectIcon))
	{
		UpdateNameEntryPage(stats);
	}
	if((stats->endOfLevelStatsBook)&&(!frontEndMenu.frontEndMenuBook)&&(stats->endOfLevelStatsBook->getCurrentPage() == stats->highscoreEnterName))
	{
		UpdateNameEntryPage(stats);
	}
#endif

/* PP: REMOUT: Now handled by the Book List				if(stats->endOfLevelStatsBook)
	{
		stats->endOfLevelStatsBook->update();
	}
	enterLevelBook.update();*/
}

/*	--------------------------------------------------------------------------------
	Function 	: FreePerFreeStatsDisplaysmanentResources
	Purpose 	: Free all stats books
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void FreeStatsDisplays(void)
{
	int		i;

	for(i=0;i<gameStatus.multiplayer.numPlayers;i++)
	{
		if(gameStatus.player[i].stats->endOfLevelStatsBook)
		{
			SAFE_DELETE(gameStatus.player[i].stats->endOfLevelStatsBook);
		}
	}

	// PP: REMOUT: BookList_closeAndShutdown should do the trick	enterLevelBook.shutdown();

	// PP: start the book closing and have it shut-down once it finishes closing
	BookList_closeAndShutdown(enterLevelBook);
}

/* --------------------------------------------------------------------------------
   Function : UpdateNameEntryPage
   Purpose : updates structures used to draw end of level stats for single player game
   Parameters : 
   Returns : 
   Info : 
*/

#ifndef CONSUMERDEMO

void UpdateNameEntryPage(PLAYERSTATS *stats)
{
	static PAGEITEM		*lastLetterItem=NULL;		// PP: ptr to pageitem pressed to enter the last letter of the initials.  If this is pressed again (with all letters entered), it's like pressing the 'End' button.
	int					i, letSel;
	
	//if((stats->endOfLevelStatsBook)&&((stats->endOfLevelStatsBook->getCurrentPage() == stats->highscoreSelectIcon) ||
	//								  (stats->endOfLevelStatsBook->getCurrentPage() == stats->highscoreEnterName)))									  

	if (gameStatus.outroState == OUTRO_NAMEENTRY)
	{
		switch(stats->nameEntryStage)
		{
		case HSE_CHOOSEICON:
			if(OKBUTTON)
			{
				for (i = 0; i < NUM_HSE_ICONS; i++)
				{
					if (stats->chooseIcon[i]->selected)// PP: MILESTONE SHORTCUT
						break;
				}

				stats->iconNumber = i;
				
				stats->endOfLevelStatsBook->gotoPage("EnterName");

				stats->chosenIcon->setTexture(highScoreIconNames[stats->iconNumber]);

				// PP: Force-debounce ok button(s)
				FORCE_OKBUTTON_DEBOUNCE;
				
#ifdef DEFAULT_INITIALS

				stats->nameEntryStage=HSE_CONFIRM;

				// PP: select the 'End' textbox
				stats->highscoreEnterName->selectItem(stats->chooseLetter[32]);
		
				// PP: need a default name, for Xbox TCR 2-06
				stats->letters[0] = defaultInitials[0];// PP: T
				stats->letters[1] = defaultInitials[1];// PP: A
				stats->letters[2] = defaultInitials[2];// PP: Z

#else// PP: no default initials

				stats->nameEntryStage++;// PP: HSE_LETTERONE
				
				stats->highscoreEnterName->selectItem(stats->chooseLetter[0]);

				stats->letters[0] = HSE_DEFAULT_ASCII;
				stats->letters[1] = HSE_DEFAULT_ASCII;
				stats->letters[2] = HSE_DEFAULT_ASCII;
		
#endif// PP: no default initials

				i = gameStatus.globalSettings.currentSaveSlot;
			
				if (gameStatus.globalSettings.saveInfo.saveSlot[i].lastName[0])
				{
					stats->letters[0] = gameStatus.globalSettings.saveInfo.saveSlot[i].lastName[0];
					stats->nameEntryStage = HSE_LETTERTWO;
					stats->highscoreEnterName->selectItem(stats->chooseLetter[32]);

					stats->chooseLetter[33]->setSelectable(true);
					stats->chooseLetter[33]->setColour(TEXTFX_DEFAULT_COLOUR1, TEXTFX_DEFAULT_COLOUR2);
				}

				if (gameStatus.globalSettings.saveInfo.saveSlot[i].lastName[1])
				{
					stats->letters[1] = gameStatus.globalSettings.saveInfo.saveSlot[i].lastName[1];
					stats->nameEntryStage = HSE_LETTERTHREE;
					stats->highscoreEnterName->selectItem(stats->chooseLetter[32]);

					stats->chooseLetter[33]->setSelectable(true);
					stats->chooseLetter[33]->setColour(TEXTFX_DEFAULT_COLOUR1, TEXTFX_DEFAULT_COLOUR2);
				}

				if (gameStatus.globalSettings.saveInfo.saveSlot[i].lastName[2])
				{
					stats->letters[2] = gameStatus.globalSettings.saveInfo.saveSlot[i].lastName[2];
					stats->nameEntryStage = HSE_CONFIRM;
					stats->highscoreEnterName->selectItem(stats->chooseLetter[32]);

					stats->chooseLetter[33]->setSelectable(true);
					stats->chooseLetter[33]->setColour(TEXTFX_DEFAULT_COLOUR1, TEXTFX_DEFAULT_COLOUR2);

				}
			}
			else if (frontEndMenu.frontEndMenuBook && CANCELBUTTON)
			{
				stats->nameEntryStage = HSE_DROPOUT;
				BookList_closeAndFree(stats->endOfLevelStatsBook);// PP: 
				// PP: REMOUT: where possible, it's nicer to use BookList_closeAndFree		SAFE_DELETE(stats->endOfLevelStatsBook);
				return;
			}
			break;
		case HSE_LETTERONE:
			if((!stats->chooseLetter[33]->selected) && OKBUTTON)
			{
				if (stats->chooseLetter[32]->selected)// PP: MILESTONE SHORTCUT
				{
					stats->nameEntryStage = HSE_SENDENTRY;
				}
				else
				{
					for (i = 0; i < 32; i++)
					{
						if (stats->chooseLetter[i]->selected)// PP: MILESTONE SHORTCUT
						{
							letSel = i;
							break;
						}
					}

					stats->letters[0] = (char) enternameCharacters[letSel][0];
					stats->nameEntryStage++;
					stats->chooseLetter[33]->setSelectable(true);
					stats->chooseLetter[33]->setColour(TEXTFX_DEFAULT_COLOUR1, TEXTFX_DEFAULT_COLOUR2);
				}

				// PP: Force-debounce ok button(s)
				FORCE_OKBUTTON_DEBOUNCE;
			}
			if(CANCELBUTTON)
			{
				stats->endOfLevelStatsBook->gotoPage("SelectIcon");
				stats->nameEntryStage--;

				i = gameStatus.globalSettings.currentSaveSlot;
			}
			break;
		case HSE_LETTERTWO:
			if(OKBUTTON)
			{
				if (stats->chooseLetter[32]->selected)// PP: MILESTONE SHORTCUT
				{
					stats->nameEntryStage = HSE_SENDENTRY;
				}
				else if (stats->chooseLetter[33]->selected)// PP: MILESTONE SHORTCUT
				{
					stats->letters[0] = HSE_DEFAULT_ASCII;
					stats->chooseLetter[33]->setSelectable(false);
					stats->chooseLetter[33]->setColour(COLOUR(96, 96, 96, 128), COLOUR(32, 32, 32, 128));
					stats->highscoreEnterName->selectItem(stats->chooseLetter[0]);

					stats->nameEntryStage--;
				}
				else
				{
					for (i = 0; i < 32; i++)
					{
						if (stats->chooseLetter[i]->selected)// PP: MILESTONE SHORTCUT
						{
							letSel = i;
							break;
						}
					}

					stats->letters[1] = (char) enternameCharacters[letSel][0];
					stats->nameEntryStage++;
				}

				// PP: Force-debounce ok button(s)
				FORCE_OKBUTTON_DEBOUNCE;
			}
			if(CANCELBUTTON)
			{
#if (BPLATFORM != XBOX)
				stats->letters[0] = HSE_DEFAULT_ASCII;
				stats->chooseLetter[33]->setSelectable(false);
				stats->chooseLetter[33]->setColour(COLOUR(96, 96, 96, 128), COLOUR(32, 32, 32, 128));

				if (stats->chooseLetter[33]->selected)
					stats->highscoreEnterName->selectItem(stats->chooseLetter[0]);

				stats->nameEntryStage--;

#else
				stats->endOfLevelStatsBook->gotoPage("SelectIcon");
				stats->nameEntryStage = HSE_CHOOSEICON;

				i = gameStatus.globalSettings.currentSaveSlot;
#endif
			}
			break;
		case HSE_LETTERTHREE:
			if(OKBUTTON)
			{
				if (stats->chooseLetter[32]->selected)// PP: MILESTONE SHORTCUT
				{
					stats->nameEntryStage = HSE_SENDENTRY;
				}
				else if (stats->chooseLetter[33]->selected)// PP: MILESTONE SHORTCUT
				{
					stats->letters[1] = HSE_DEFAULT_ASCII;
					stats->nameEntryStage--;
				}
				else
				{
					for (i = 0; i < 32; i++)
					{
						if (stats->chooseLetter[i]->selected)// PP: MILESTONE SHORTCUT
						{
							// PP: keep a ptr to the item that's been pressed.
							// PP: If it's pressed agian (with all letters entered), it's like pressing the 'End' button.
							lastLetterItem=stats->chooseLetter[i];

							letSel = i;
							break;
						}
					}

					stats->letters[2] = (char) enternameCharacters[letSel][0];
					stats->highscoreEnterName->selectItem(stats->chooseLetter[32]);
					stats->nameEntryStage++;
				}

				// PP: Force-debounce ok button(s)
				FORCE_OKBUTTON_DEBOUNCE;
			}
			if(CANCELBUTTON)
			{
#if (BPLATFORM != XBOX)
				stats->letters[1] = HSE_DEFAULT_ASCII;
				stats->nameEntryStage--;
#else
				stats->endOfLevelStatsBook->gotoPage("SelectIcon");
				stats->nameEntryStage = HSE_CHOOSEICON;

				i = gameStatus.globalSettings.currentSaveSlot;
#endif
			}
			break;
		case HSE_CONFIRM:
			if(OKBUTTON)
			{
				if (stats->chooseLetter[32]->selected)// PP: MILESTONE SHORTCUT
				{
					stats->nameEntryStage++;

					// PP: Force-debounce ok button(s)
					FORCE_OKBUTTON_DEBOUNCE;
				}
				else if (stats->chooseLetter[33]->selected)// PP: MILESTONE SHORTCUT
				{
					stats->letters[2] = HSE_DEFAULT_ASCII;
					stats->nameEntryStage--;
				}
				else// if(lastLetterItem->selected)
				{
					// PP: the player has entered their three initials then just pressed x without changing letter - they're finished with this page.

					// JW: Change of plan - select end but don't send off the entry till they actually press on it

					// PP: select the 'End' button to confirm what's going on
					stats->highscoreEnterName->selectItem(stats->chooseLetter[32]);// PP: 33 = 'End' button

					// PP: move onto next page...
					//stats->nameEntryStage++;
					//controller1.crossDebounceChannel->value = 0;
				}
			}
			else if(CANCELBUTTON)
			{
#if (BPLATFORM != XBOX)
				stats->letters[2] = HSE_DEFAULT_ASCII;
				stats->nameEntryStage--;
#else
				stats->endOfLevelStatsBook->gotoPage("SelectIcon");
				stats->nameEntryStage = HSE_CHOOSEICON;

				i = gameStatus.globalSettings.currentSaveSlot;
#endif
			}
		/* PP: REMOUT: done elsewhere	else if((frontEndMenu.frontEndMenuBook) && (gameStatus.globalSettings.currentSaveSlot == 2) && (controller1.squareDebounceChannel->value))
			{
				checkCheats(stats);
			}*/
			break;
		case HSE_SENDENTRY:
			// NH: Don't show high score if front end name entry
			if (!frontEndMenu.frontEndMenuBook)
			{
				AddNewRecordToStats(stats,(EScene)map.sceneNumber);
				CreateHighScoreTablePage(stats,(EScene)map.sceneNumber);
				stats->endOfLevelStatsBook->gotoPage("highScores");
				stats->nameEntryStage = HSE_CHOOSEICON;
				//stats->advice->setText(STR_CHOOSE_ICON);
				gameStatus.outroState = OUTRO_HIGHSCORES;
			}
			else
			// NH: Exit if frontend name entry
			{
				gameStatus.outroState = OUTRO_EXIT;
				stats->nameEntryStage = HSE_EXIT;
			}

			// JW: DON'T save the new icon and name for the frontend

			// JW: just wait. they'll change their minds about this too...

			if (map.sceneNumber == SCENE_FRONTEND)
			{
				i = gameStatus.globalSettings.currentSaveSlot;

				gameStatus.globalSettings.saveInfo.saveSlot[i].lastIcon = stats->iconNumber;
				
				if (stats->letters[0] != HSE_DEFAULT_ASCII)
					gameStatus.globalSettings.saveInfo.saveSlot[i].lastName[0] = stats->letters[0];
				else
					gameStatus.globalSettings.saveInfo.saveSlot[i].lastName[0] = 0;
				
				if (stats->letters[1] != HSE_DEFAULT_ASCII)
					gameStatus.globalSettings.saveInfo.saveSlot[i].lastName[1] = stats->letters[1];
				else
					gameStatus.globalSettings.saveInfo.saveSlot[i].lastName[1] = 0;
				
				if (stats->letters[2] != HSE_DEFAULT_ASCII)
					gameStatus.globalSettings.saveInfo.saveSlot[i].lastName[2] = stats->letters[2];
				else
					gameStatus.globalSettings.saveInfo.saveSlot[i].lastName[2] = 0;
			}

			stats->statsBookClock = 0.0f;
			break;
		}
		
//		if (stats->cheatsScale < 2.0f)
//			stats->cheatsScale += 0.1f;
//		else
//		{
//			stats->cheatsBook->close();
//		}

		stats->enterName->sprintf8("    %c %c %c", stats->letters[0], stats->letters[1], stats->letters[2]);
	}
}

#endif

/* --------------------------------------------------------------------------------
   Function : DrawStatsAtEndOfLevel
   Purpose : draws end of level stats for single player game
   Parameters : 
   Returns : 
   Info : 
*/
/* PP: REMOUT: Now handled by the Book List			
void DrawStatsAtEndOfLevel(PLAYERSTATS *stats)
{
	if(stats->endOfLevelStatsBook)
	{
		stats->endOfLevelStatsBook->draw();
// PP: REMOUT: the alignment bug that was evident on this page is fixed now  		stats->endOfLevelStatsBook->drawDebug();
	}
	enterLevelBook.draw();
}
*/
/* --------------------------------------------------------------------------------
   Function : IncrementDecrementScoreCharacter
   Purpose : draws end of level stats for single player game
   Parameters : 
   Returns : 
   Info : 
*/

char IncrementDecrementScoreCharacter(char currentAscii, uint32 flag)
{
	if(flag & HSE_INCREMENT)
	{
		//check for end of capitals: goto numbers
		if(currentAscii == 90) return 48;
		//check for end of numbers: goto underscore
		if(currentAscii == 57) return HSE_DEFAULT_ASCII;
		//check for underscore: goto capitals
		if(currentAscii == HSE_DEFAULT_ASCII) return 65;
		//else
		return ++currentAscii;
	}
	if(flag & HSE_DECREMENT)
	{
		//check for end of capitals: goto underscore
		if(currentAscii == 65) return HSE_DEFAULT_ASCII;
		//check for underscore: goto numbers
		if(currentAscii == HSE_DEFAULT_ASCII) return 57;
		//check for end of numbers: goto capitals
		if(currentAscii == 48) return 90;
		//else
		return --currentAscii;
	}
	//huh? what exactly do you want to do?
	return currentAscii;
}
