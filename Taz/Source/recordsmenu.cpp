// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : startgamemenu.cpp
//   Purpose : functions to create and control the game records menu
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "textfx.h"
#include "control.h"
#include "icon.h"
#include "frontendmenu.h"
#include "recordsmenu.h"
#include "frontendcamera.h"
#include "frontendmain.h"
#include "animation.h"
#include "util.h"
#include "playerstats.h"
#include "memcard.h"

static int OVERALL_SCORE;
PLAYER_RECORDS playerRecords[4];
PLAYER_RECORDS bestPlayer;

int noOfPlayers = 0;

/*	--------------------------------------------------------------------------------
	Function 	: AddLevelRecordsPage
	Purpose 	: Adds a records page for a specified level
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void AddLevelRecordsPage(TAG_PAGE *levelPage, EScene sceneNumber, int hubNumber, int levelNumber)
{
	RECORDSPAGE		*ptr;
	PAGE			*subPage;
	TEXTBOX			*textboxPtr;
	ICON			*tickIconPtr;
	char			tempString[16];
	uint			highestScorer = 0;
	uint32			score = 0;

	ptr = frontEndMenu.recordsPage;

	// NH: Get highest Scorer
	memset(&playerRecords[0], 0 , sizeof(PLAYER_RECORDS));
	memset(&playerRecords[1], 0 , sizeof(PLAYER_RECORDS));
	memset(&playerRecords[2], 0 , sizeof(PLAYER_RECORDS));
	memset(&playerRecords[3], 0 , sizeof(PLAYER_RECORDS));
	highestScorer	= getHighestScorer(sceneNumber);

	// NH: Icon

	// NH: Player Icon
	levelPage->insertFreeItem(ptr->hiscoreIcon[hubNumber][levelNumber] = new ICON(highScoreIconNames[playerRecords[0].iconRef]), RECTANGLE(-0.15f, -0.05f, 0.45f, 0.35f));
	ptr->hiscoreIcon[hubNumber][levelNumber]
		->setDrawScale(0.75)
		->setSelectable(false);

	// NH: Player Name
	levelPage->insertFreeItem(ptr->hiscoreNameTb[hubNumber][levelNumber] = new TEXTBOX(), RECTANGLE(0.05f, 0.45f, 0.45f, 0.35f));
	ptr->hiscoreNameTb[hubNumber][levelNumber]->sprintf8("%s",playerRecords[0].name);
	ptr->hiscoreNameTb[hubNumber][levelNumber]
		->setXAlign(PIFLAG_XALIGN_LEFT)
		->setSelectable(false);
	
	levelPage->insertFreeItem(ptr->destructableIcon[hubNumber][levelNumber] = new ICON("destructibles02.bmp"), RECTANGLE(-0.2f, -0.15f,  0.1f, 0.0f));
	ptr->destructableIcon[hubNumber][levelNumber]
		->setDrawScale(0.5)
		->setSelectable(false);

	levelPage->insertFreeItem(tickIconPtr = new ICON(), RECTANGLE(-0.1f, -0.05f,  0.1f, 0.0f));
	if (playerRecords[0].destructsCollected)
		tickIconPtr->setTexture("yellowtick.bmp");
	else
		tickIconPtr->setTexture("yellowcross.bmp");
	tickIconPtr->tilt = false;
	tickIconPtr->setSelectable(false)
		->setDrawScale(0.3f);
	
	levelPage->insertFreeItem(ptr->secretIcon[hubNumber][levelNumber] = new ICON("samface.bmp"), RECTANGLE(0.05f, 0.1f,  0.1f, 0.0f));
	ptr->secretIcon[hubNumber][levelNumber]
		->setDrawScale(0.5f)
		->setSelectable(false);
	
	levelPage->insertFreeItem(tickIconPtr = new ICON(),RECTANGLE(0.15f, 0.2f,  0.1f, -0.0f));
	if (playerRecords[0].secretsCollected)
		tickIconPtr->setTexture("yellowtick.bmp");
	else
		tickIconPtr->setTexture("yellowcross.bmp");
	tickIconPtr->tilt = false;
	tickIconPtr->setSelectable(false)
		->setDrawScale(0.3f);
	
	levelPage->insertFreeItem(ptr->sandwichIcon[hubNumber][levelNumber] = new ICON("sandwich.bmp"), RECTANGLE(-0.2f, -0.15f,  -0.15f, -0.25f));
	ptr->sandwichIcon[hubNumber][levelNumber]
		->setDrawScale(0.5f)
		->setSelectable(false);
	
	levelPage->insertFreeItem(textboxPtr = new TEXTBOX(),RECTANGLE(-0.1f, -0.05f,  -0.15f, -0.25f));
	textboxPtr->sprintf8("%d/100", playerRecords[0].pickupsCollected);
	textboxPtr->setSelectable(false)
		->setScale(0.75f);
	
	levelPage->insertFreeItem(ptr->posterIcon[hubNumber][levelNumber] = new ICON("tazwanted.bmp"), RECTANGLE(0.05f, 0.1f,  -0.15f, -0.25f));
	ptr->posterIcon[hubNumber][levelNumber]
		->setDrawScale(0.5)
		->setSelectable(false);
	
	levelPage->insertFreeItem(ptr->posterTb[hubNumber][levelNumber] = new TEXTBOX(), RECTANGLE(0.15f, 0.2f, -0.15f, -0.25f));

	ptr->posterTb[hubNumber][levelNumber]->sprintf8("%d/7", playerRecords[0].postersDestroyed);
	ptr->posterTb[hubNumber][levelNumber]	
		->setSelectable(false)
		->setScale(0.75f);
	
	// NH: Bounty (Mmmm.... Coconut.  Yummy)
	levelPage->insertFreeItem(ptr->hiscoreBountyTb[hubNumber][levelNumber] = new TEXTBOX(), RECTANGLE(-0.45f, 0.45f, -0.45f, -0.5f));

	utilFormatPrettyNumbers(playerRecords[0].money, tempString);
	ptr->hiscoreBountyTb[hubNumber][levelNumber]->sprintf8("$%s", tempString);
	ptr->hiscoreBountyTb[hubNumber][levelNumber]//->setAlignScale(1.2f)
		->setSelectable(false);
	
	levelPage->setSlack(false);
}

/*	--------------------------------------------------------------------------------
	Function 	: CreateGameOptionsMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void CreateRecordsMenuPage(FRONTENDMENU *menu)
{
	RECORDSPAGE		*ptr;
	PAGE			*pagePtr,*subPage,*subSubPage,*levelPage,*row1Page,*row2Page,*detailsPage;
	TEXTBOX			*textboxPtr;
	BOOK			*subBook;
	char			tempString[16];
	RECTANGLE		hubRect, levelRect, hubTextRect, levelTextRect, detailsRect;
	int highestScorer;

	hubRect.left			= -0.5f;
	hubRect.right			= 0.5f;
	hubRect.top				= 0.38f;
	hubRect.bottom			= 0.3f;

	hubTextRect.left		= -0.5f;
	hubTextRect.right		= 0.5f;
	hubTextRect.top			= -0.5f;
	hubTextRect.bottom		= -0.5f;
	
	levelRect.left			= -0.5f;
	levelRect.right			= 0.5f;
	levelRect.top			= 0.31f;
	levelRect.bottom		= 0.23f;

	levelTextRect.left		= -0.5f;
	levelTextRect.right		= 0.5f;
	levelTextRect.top		= 0.35f;
	levelTextRect.bottom	= 0.35f;

	detailsRect.left		= -0.6f;				// CMD: need just a little more room
	detailsRect.right		= 0.6f;
	detailsRect.top			= 0.3f;
	detailsRect.bottom		= -0.35f;

	ptr = menu->recordsPage = (RECORDSPAGE*)ZALLOC(sizeof(RECORDSPAGE));
	if(!ptr)
	{
		bkPrintf("*** WARNING ***, could not malloc records page/n");
		return;
	}
	// NH: Add Page (Records)
	pagePtr = ptr->page = menu->frontEndMenuBook->addPage(menuNames[MENU_RECORDS]);
	pagePtr->setNavigation(true);

	// PP: navigation Y-wrapping is allowed, but only in response to a debounced input, NOT an auto-repeat input
	pagePtr->setNavigationWrap(PAGEFLAG_NAVWRAP_YDEBOUNCE);

	// NH: Add Title
	pagePtr->insertItem(ptr->titleTb = new TEXTBOX(STR_RECORDS),false);
	ptr->titleTb->setStyle(TS_title)
		->setYAlign(PIFLAG_YALIGN_TOP)
		->setYPadding(false)
		->setSelectable(false);

	// NH: Calculate Overall Top Scorer
	highestScorer = compileOverallRecord();	

	///////
	
	// NH: Create a page to put the HUB TOGGLE's icons and title in
	subPage = new PAGE("hubControlPage");

	// NH: Add the Left icon to the left of the page "hubControlPage"
	ptr->leftHubIconBook = new BOOK("leftHubIconBook");	
	subSubPage = ptr->leftHubIconBook->addPage("off");
	subSubPage = ptr->leftHubIconBook->addPage("on");
	subSubPage->insertItem(ptr->hubLeftIcon = new ICON("scrollerarrowleft.bmp"));
	ptr->hubLeftIcon->setName("hubArrowLeft");
	ptr->hubLeftIcon->setXAlign(PIFLAG_XALIGN_LEFT)
					->setDrawScale(0.5f)
					->setSelectable(true,true);
	ptr->hubLeftIcon->tilt = false;

	ptr->leftHubIconBook->open();
	
	subPage->insertItem(ptr->leftHubIconBook);

	// NH: Add the hub control book as a free page (Book so that multiple pages (for each hub title) can be added and faded through, and placed where we like)
	subPage->insertFreeItem(ptr->hubControlBook=new BOOK("HUB CONTROL BOOK"),hubTextRect);

	// NH: Add the pages (titles for each hub)
	// NH: Add The hub control page (overall)
	levelPage = ptr->hubControlBook->addPage("overallHub");
	ptr->hubControl[4] = levelPage->insertItem(STR_R_OVERALL,false);
	ptr->hubControl[4]->setYPadding(false);

	// NH: Add The hub control page (zoo)
	levelPage = ptr->hubControlBook->addPage("zooHub");
	ptr->hubControl[0] = levelPage->insertItem(STR_R_ZOO,false);
	ptr->hubControl[0]->setYPadding(false);
	
	// NH: Add The hub control page (zoo)
	levelPage = ptr->hubControlBook->addPage("cityHub");
	ptr->hubControl[1] = levelPage->insertItem(STR_R_SAM_FRAN,false);
	ptr->hubControl[1]->setYPadding(false);
	
	// NH: Add The hub control page (west)
	levelPage = ptr->hubControlBook->addPage("westHub");
	ptr->hubControl[2] = levelPage->insertItem(STR_R_WEST,false);
	ptr->hubControl[2]->setYPadding(false);
	
	// NH: Add The hub control page (taz)
	levelPage = ptr->hubControlBook->addPage("tazHub");
	ptr->hubControl[3] = levelPage->insertItem(STR_R_TASMANIA,false);
	ptr->hubControl[3]->setYPadding(false);

	// NH: Add right icon to the right of the page "hubControlPage"
	ptr->rightHubIconBook = new BOOK("rightHubIconBook");	
	subSubPage = ptr->rightHubIconBook->addPage("on");
	
	subSubPage->insertItem(ptr->hubRightIcon = new ICON("scrollerarrowright.bmp"),false);
	ptr->hubRightIcon->setName("hubArrowRight");
	ptr->hubRightIcon->setXAlign(PIFLAG_XALIGN_RIGHT)
					 ->setDrawScale(0.5f)
					 ->setSelectable(true,true);
	subSubPage = ptr->rightHubIconBook->addPage("off");
	ptr->hubRightIcon->tilt = false;

	ptr->rightHubIconBook->open();
	
	subPage->insertItem(ptr->rightHubIconBook,false);

	// NH: Insert the page "hubControlPage" as a free item on the main page
	pagePtr->insertFreeItem(subPage, hubRect);

	////////

	// NH: Create a page to put the LEVEL TOGGLE's icons and title in
	subPage = new PAGE("levelControlPage");

	// NH: Add the Left icon to the left of the page "levelControlPage"
	ptr->leftLevelIconBook = new BOOK("leftLevelIconBook");	
	subSubPage = ptr->leftLevelIconBook->addPage("off");
	subSubPage = ptr->leftLevelIconBook->addPage("on");
	subSubPage->insertItem(ptr->levelLeftIcon = new ICON("scrollerarrowleft.bmp"));
	ptr->levelLeftIcon->setName("levelArrowLeft");
	ptr->levelLeftIcon->setXAlign(PIFLAG_XALIGN_LEFT)
					  ->setDrawScale(0.5f)
					  ->setSelectable(true,true);
	ptr->levelLeftIcon->tilt = false;

	ptr->leftLevelIconBook->open();
	
	subPage->insertItem(ptr->leftLevelIconBook);

	// NH: Create a page to put the HUB TOGGLE's icons and title in
	subPage->insertFreeItem(ptr->levelControlBook=new BOOK("LEVEL CONTROL BOOK"),levelTextRect);
	
	// NH: Add the pages (titles for each level)
	// NH: Add The level control page (overall)
	levelPage = ptr->levelControlBook->addPage("overallLevel");
	
	// NH: Add The level control page (safari)
	levelPage = ptr->levelControlBook->addPage("safariLevel");
	ptr->levelControl[0][0] = levelPage->insertItem(STR_R_ZOO_SAFARI_ZONE);
	ptr->levelControl[0][0]->setYAlign(PIFLAG_YALIGN_TOP);

	// NH: Add The level control page (icekingdome)
	levelPage = ptr->levelControlBook->addPage("icekingdomeLevel");
	ptr->levelControl[0][1] = levelPage->insertItem(STR_R_ZOO_ICE_KING_DOME);
	ptr->levelControl[0][1]->setYAlign(PIFLAG_YALIGN_TOP);

	// NH: Add The level control page (aqua)
	levelPage = ptr->levelControlBook->addPage("aquaLevel");
	ptr->levelControl[0][2] = levelPage->insertItem(STR_R_ZOO_AQUA);
	ptr->levelControl[0][2]->setYAlign(PIFLAG_YALIGN_TOP);
	
	// NH: Add The level control page (deptstr)
	levelPage = ptr->levelControlBook->addPage("deptstrLevel");
	ptr->levelControl[1][0] = levelPage->insertItem(STR_R_SAM_FRAN_DEPT_STR);
	ptr->levelControl[1][0]->setYAlign(PIFLAG_YALIGN_TOP);

	// NH: Add The level control page (museum)
	levelPage = ptr->levelControlBook->addPage("museumLevel");
	ptr->levelControl[1][1] = levelPage->insertItem(STR_R_SAM_FRAN_MUSEUM);
	ptr->levelControl[1][1]->setYAlign(PIFLAG_YALIGN_TOP);

	// NH: Add The level control page (construct)
	levelPage = ptr->levelControlBook->addPage("constructLevel");
	ptr->levelControl[1][2] = levelPage->insertItem(STR_R_SAM_FRAN_CONSTRUCT);
	ptr->levelControl[1][2]->setYAlign(PIFLAG_YALIGN_TOP);

	// NH: Add The level control page (canyon)
	levelPage = ptr->levelControlBook->addPage("canyonLevel");
	ptr->levelControl[2][0] = levelPage->insertItem(STR_R_WEST_CANYON);
	ptr->levelControl[2][0]->setYAlign(PIFLAG_YALIGN_TOP);

	// NH: Add The level control page (mine)
	levelPage = ptr->levelControlBook->addPage("mineLevel");
	ptr->levelControl[2][1] = levelPage->insertItem(STR_R_WEST_MINE);
	ptr->levelControl[2][1]->setYAlign(PIFLAG_YALIGN_TOP);

	// NH: Add The level control page (ghost)
	levelPage = ptr->levelControlBook->addPage("ghostLevel");
	ptr->levelControl[2][2] = levelPage->insertItem(STR_R_WEST_GHOST);
	ptr->levelControl[2][2]->setYAlign(PIFLAG_YALIGN_TOP);


	// NH: Add The level control page (taz)
	levelPage = ptr->levelControlBook->addPage("tazLevel");
	ptr->levelControl[3][0] = levelPage->insertItem(STR_R_TASMANIA_TEMPLE);
	ptr->levelControl[3][0]->setYAlign(PIFLAG_YALIGN_TOP);

	// NH: Add right icon to the right of the page "levelControlPage"
	ptr->rightLevelIconBook = new BOOK("rightLevelIconBook");	
	subSubPage = ptr->rightLevelIconBook->addPage("off");
	subSubPage = ptr->rightLevelIconBook->addPage("on");
	subSubPage->insertItem(ptr->levelRightIcon = new ICON("scrollerarrowright.bmp"),false);
	ptr->levelRightIcon->setName("levelArrowRight");
	ptr->levelRightIcon->setXAlign(PIFLAG_XALIGN_RIGHT)
					   ->setDrawScale(0.5f)
					   ->setSelectable(true,true);
	ptr->levelRightIcon->tilt = false;

	ptr->rightLevelIconBook->open();
	
	subPage->insertItem(ptr->rightLevelIconBook,false);
	
	// NH: Insert the page "levelControlPage" as a free item on the main page
	pagePtr->insertFreeItem(subPage, levelRect);

	///////

	// NH: Add The overalls page
	subPage = new PAGE("detailsPage");
	subPage->insertFreeItem(ptr->overallBook=new BOOK("OVERALL BOOK"),detailsRect);
	
	// NH: Add The overalls page
	levelPage = ptr->overallBook->addPage("overall");
	
	// NH: Icon

	// NH: Player Icon
	levelPage->insertFreeItem(ptr->hiscoreIcon[3][1] = new ICON(highScoreIconNames[playerRecords[highestScorer].iconRef]), RECTANGLE(-0.15f, -0.05f, 0.45f, 0.35f));
	ptr->hiscoreIcon[3][1]
		->setDrawScale(0.75)
		->setSelectable(false);

	// NH: Player Name
	levelPage->insertFreeItem(ptr->hiscoreNameTb[3][1] = new TEXTBOX(),RECTANGLE(0.05f, 0.45f, 0.45f, 0.35f));
	ptr->hiscoreNameTb[3][1]->sprintf8("%s",playerRecords[highestScorer].name);
	ptr->hiscoreNameTb[3][1]
		->setXAlign(PIFLAG_XALIGN_LEFT)
		->setSelectable(false);
		
	levelPage->insertFreeItem(ptr->destructableIcon[3][1] = new ICON("destructibles02.bmp"), RECTANGLE(-0.2f, -0.15f,  0.1f, 0.0f));
	ptr->destructableIcon[3][1]
		->setDrawScale(0.5)
		->setSelectable(false);

	levelPage->insertFreeItem(textboxPtr = new TEXTBOX(), RECTANGLE(-0.1f, -0.05f, 0.1f, 0.0f));
	textboxPtr->sprintf8("%d/10",playerRecords[highestScorer].destructsCollected);
	textboxPtr->setSelectable(false)
		->setScale(0.75f);

	levelPage->insertFreeItem(ptr->secretIcon[3][1] = new ICON("samface.bmp"), RECTANGLE(0.05f, 0.1f, 0.1f, 0.0f));
	ptr->secretIcon[3][1]
		->setDrawScale(0.5)
		->setSelectable(false);

	levelPage->insertFreeItem(textboxPtr = new TEXTBOX(),RECTANGLE(0.15f, 0.2f, 0.1f, 0.0f));
	textboxPtr->sprintf8("%d/10",playerRecords[highestScorer].secretsCollected);
	textboxPtr->setSelectable(false)
		->setScale(0.75f);

	levelPage->insertFreeItem(ptr->sandwichIcon[3][1] = new ICON("sandwich.bmp"), RECTANGLE(-0.2f, -0.15f,  -0.15f, -0.25f));
	ptr->sandwichIcon[3][1]
		->setDrawScale(0.5)
		->setSelectable(false);

	levelPage->insertFreeItem(textboxPtr = new TEXTBOX(),RECTANGLE(-0.1f, -0.05f, -0.15f, -0.25f));
	textboxPtr->sprintf8("%d/1000",playerRecords[highestScorer].pickupsCollected);
	textboxPtr->setSelectable(false)
		->setScale(0.75f);

	levelPage->insertFreeItem(ptr->posterIcon[3][1] = new ICON("tazwanted.bmp"), RECTANGLE(0.05f, 0.1f, -0.15f, -0.25f));
	ptr->posterIcon[3][1]
		->setDrawScale(0.5)
		->setSelectable(false);

	levelPage->insertFreeItem(ptr->posterTb[3][1] = new TEXTBOX(), RECTANGLE(0.15f, 0.2f, -0.15f, -0.25f)); 
	ptr->posterTb[3][1]->sprintf8("%d/70",playerRecords[highestScorer].postersDestroyed);
	ptr->posterTb[3][1]->setSelectable(false)
		->setScale(0.75f);

	// NH: Bounty (Mmmm.... Coconut.  Yummy)
	levelPage->insertFreeItem(ptr->hiscoreBountyTb[3][1] = new TEXTBOX(),RECTANGLE(-0.45f, 0.45f, -0.45f, -0.5f));

	utilFormatPrettyNumbers(playerRecords[highestScorer].money, tempString);
	ptr->hiscoreBountyTb[3][1]->sprintf8("$%s", tempString);
	ptr->hiscoreBountyTb[3][1]
		->setSelectable(false);

	levelPage->setSlack(false);
	
	//////////////

	// NH: Add The safari page
	levelPage = ptr->overallBook->addPage("safari");
	AddLevelRecordsPage(levelPage, SCENE_SAFARI, 0, 0);
		
	// NH: Add The icekingdome page
	levelPage = ptr->overallBook->addPage("icekingdome");
	AddLevelRecordsPage(levelPage, SCENE_ICEDOME, 0, 1);
	
	// NH: Add The aqua page
	levelPage = ptr->overallBook->addPage("aqua");
	AddLevelRecordsPage(levelPage, SCENE_AQUA, 0, 2);
	
	// NH: Add The safari page
	levelPage = ptr->overallBook->addPage("deptstr");
	AddLevelRecordsPage(levelPage, SCENE_DEPTSTR, 1, 0);
	// NH: Add The museum page
	levelPage = ptr->overallBook->addPage("museum");
	AddLevelRecordsPage(levelPage, SCENE_MUSEUM, 1, 1);
	
	// NH: Add The construct page
	levelPage = ptr->overallBook->addPage("construct");
	AddLevelRecordsPage(levelPage, SCENE_CONSTRUCT, 1, 2);
	
	// NH: Add The canyon page
	levelPage = ptr->overallBook->addPage("canyon");
	AddLevelRecordsPage(levelPage, SCENE_GRANDCANYON, 2, 0);
	
	// NH: Add The museum page
	levelPage = ptr->overallBook->addPage("mine");
	AddLevelRecordsPage(levelPage, SCENE_GOLDMINE, 2, 1);
	
	// NH: Add The construct page
	levelPage = ptr->overallBook->addPage("ghost");
	AddLevelRecordsPage(levelPage, SCENE_GHOSTTOWN, 2, 2);
	
	// NH: Add The taz page
	levelPage = ptr->overallBook->addPage("taz");
	AddLevelRecordsPage(levelPage, SCENE_TAZHUB, 3, 0);

	// NH: Insert the page "detailsPage" as a free item on the main page
	pagePtr->insertFreeItem(subPage, detailsRect);

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
	ADD_INFOSUBBOOK_PAGE(INFO_OVERALL,STR_R_OVERALL);
	ADD_INFOSUBBOOK_PAGE(INFO_SAFARI,STR_R_ZOO_SAFARI_ZONE);
	ADD_INFOSUBBOOK_PAGE(INFO_ICE,STR_R_ZOO_ICE_KING_DOME);
	ADD_INFOSUBBOOK_PAGE(INFO_AQUA,STR_R_ZOO_AQUA);
	ADD_INFOSUBBOOK_PAGE(INFO_DEPT,STR_R_SAM_FRAN_DEPT_STR);
	ADD_INFOSUBBOOK_PAGE(INFO_MUSEUM,STR_R_SAM_FRAN_MUSEUM);
	ADD_INFOSUBBOOK_PAGE(INFO_CONSTRUCT,STR_R_SAM_FRAN_CONSTRUCT);
	ADD_INFOSUBBOOK_PAGE(INFO_CANYON,STR_R_WEST_CANYON);
	ADD_INFOSUBBOOK_PAGE(INFO_MINE,STR_R_WEST_MINE);
	ADD_INFOSUBBOOK_PAGE(INFO_GHOST,STR_R_WEST_GHOST);
	ADD_INFOSUBBOOK_PAGE(INFO_TASMANIA,STR_R_TASMANIA_TEMPLE);

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
	ADD_CONTROLSUBBOOK_PAGE(CONTROL,STR_PRESS_Y_TO_GO_BACK);

#ifdef LOAD_AFTER_START_SCREEN// PP: MemCard.h

	// PP: Ah yeah, since this page now gets added while the frontend book is open,
	// PP: we've gotter realign the book at this point.  (Books normally tend to get aligned automatically when they open)
	// PP: We should normally try to avoid adding pages to a book while it's open, but there's no good alternative here.
	menu->frontEndMenuBook->realign();

#endif// PP: LOAD_AFTER_START_SCREEN

	ptr->hubControlBook->open();
	ptr->levelControlBook->open();
	ptr->overallBook->open();

	ptr->infoSubBook->open();
	ptr->controlSubBook->open();
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateRecordsMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateRecordsMenuPage(FRONTENDMENU *menu, PADCONTROLSTATUS *controller)
{
	PAGEITEM	*currentHubPI, *currentLevelPI;
	RECORDSPAGE *ptr = menu->recordsPage;
	uint32		currentSubState		= ptr->subState;
	uint32		currentSubSubState	= ptr->subSubState;

	static bool returning = false;

	currentHubPI	= ptr->hubControlBook->getCurrentPage()->getFirstItem();
	currentLevelPI	= ptr->levelControlBook->getCurrentPage()->getFirstItem();

	// NH: If Push Right
	if (controller1.mb1DebounceChannel->value)
	{
		if(ptr->page->selectedItem == ptr->rightHubIconBook->getCurrentPage()->getFirstItem())
		{
			switch(ptr->subState)
			{
			case R_OVERALL:
				ptr->subState = R_ZOO;
				break;
			case R_ZOO:
				ptr->subState = R_SAMFRAN;
				break;
			case R_SAMFRAN:
				ptr->subState = R_WEST;
				break;
			case R_WEST:
				ptr->subState = R_TASMANIA;
				break;		
			}
		}

		if(ptr->page->selectedItem == ptr->rightLevelIconBook->getCurrentPage()->getFirstItem())
		{
			switch(ptr->subSubState)
			{
			case R_SUB_SAFARI:
				ptr->subSubState = R_SUB_ICEKINGDOME;
				break;
			case R_SUB_ICEKINGDOME:
				ptr->subSubState = R_SUB_AQUA;
				break;
			case R_SUB_AQUA:
				ptr->subSubState = R_SUB_SAFARI;
				break;

			case R_SUB_DEPTSTR:
				ptr->subSubState = R_SUB_MUSEUM;
				break;
			case R_SUB_MUSEUM:
				ptr->subSubState = R_SUB_CONSTRUCT;
				break;
			case R_SUB_CONSTRUCT:
				ptr->subSubState = R_SUB_DEPTSTR;
				break;

			case R_SUB_CANYON:
				ptr->subSubState = R_SUB_GOLDMINE;
				break;
			case R_SUB_GOLDMINE:
				ptr->subSubState = R_SUB_GHOST;
				break;
			case R_SUB_GHOST:
				ptr->subSubState = R_SUB_CANYON;
				break;
			}
		}

		if(ptr->page->selectedItem == ptr->leftHubIconBook->getCurrentPage()->getFirstItem())
		{
			switch(ptr->subState)
			{
			case R_ZOO:
				ptr->subState = R_OVERALL;
				break;
			case R_SAMFRAN:
				ptr->subState = R_ZOO;
				break;
			case R_WEST:
				ptr->subState = R_SAMFRAN;
				break;		
			case R_TASMANIA:
				ptr->subState = R_WEST;
				break;
			}
		}

		if(ptr->page->selectedItem == ptr->leftLevelIconBook->getCurrentPage()->getFirstItem())
		{
			switch(ptr->subSubState)
			{
			case R_SUB_SAFARI:
				ptr->subSubState = R_SUB_ICEKINGDOME;
				break;
			case R_SUB_ICEKINGDOME:
				ptr->subSubState = R_SUB_AQUA;
				break;
			case R_SUB_AQUA:
				ptr->subSubState = R_SUB_SAFARI;
				break;

			case R_SUB_DEPTSTR:
				ptr->subSubState = R_SUB_MUSEUM;
				break;
			case R_SUB_MUSEUM:
				ptr->subSubState = R_SUB_CONSTRUCT;
				break;
			case R_SUB_CONSTRUCT:
				ptr->subSubState = R_SUB_DEPTSTR;
				break;

			case R_SUB_CANYON:
				ptr->subSubState = R_SUB_GOLDMINE;
				break;
			case R_SUB_GOLDMINE:
				ptr->subSubState = R_SUB_GHOST;
				break;
			case R_SUB_GHOST:
				ptr->subSubState = R_SUB_CANYON;
				break;
			}
		}		
	}

	if((controller1.menuRightDebounceChannel->value)/*(controller1.menuXDebounceChannel->value>0)*/)// PP: REMOUT: using seperate up/down/left/right menu channels now
	{
		if(ptr->page->selectedItem == currentHubPI || ptr->page->selectedItem == ptr->hubRightIcon || ptr->page->selectedItem == ptr->hubLeftIcon)
		{
			switch(ptr->subState)
			{
			case R_OVERALL:
				ptr->subState = R_ZOO;
				break;
			case R_ZOO:
				ptr->subState = R_SAMFRAN;
				break;
			case R_SAMFRAN:
				ptr->subState = R_WEST;
				break;
			case R_WEST:
				ptr->subState = R_TASMANIA;
				break;		
			}
		}
		else if(ptr->page->selectedItem == currentLevelPI || ptr->page->selectedItem == ptr->levelRightIcon || ptr->page->selectedItem == ptr->levelLeftIcon)
		{
			switch(ptr->subSubState)
			{
			case R_SUB_SAFARI:
				ptr->subSubState = R_SUB_ICEKINGDOME;
				break;
			case R_SUB_ICEKINGDOME:
				ptr->subSubState = R_SUB_AQUA;
				break;
			case R_SUB_AQUA:
				ptr->subSubState = R_SUB_SAFARI;
				break;

			case R_SUB_DEPTSTR:
				ptr->subSubState = R_SUB_MUSEUM;
				break;
			case R_SUB_MUSEUM:
				ptr->subSubState = R_SUB_CONSTRUCT;
				break;
			case R_SUB_CONSTRUCT:
				ptr->subSubState = R_SUB_DEPTSTR;
				break;

			case R_SUB_CANYON:
				ptr->subSubState = R_SUB_GOLDMINE;
				break;
			case R_SUB_GOLDMINE:
				ptr->subSubState = R_SUB_GHOST;
				break;
			case R_SUB_GHOST:
				ptr->subSubState = R_SUB_CANYON;
				break;
			}

		}
	}

	// NH: If Push Left
	if((controller1.menuLeftDebounceChannel->value)/*(controller1.menuXDebounceChannel->value<0)*/)// PP: REMOUT: using seperate up/down/left/right menu channels now
	{
		if(ptr->page->selectedItem == currentHubPI || ptr->page->selectedItem == ptr->hubRightIcon || ptr->page->selectedItem == ptr->hubLeftIcon)
		{
			switch(ptr->subState)
			{
			case R_ZOO:
				ptr->subState = R_OVERALL;
				break;
			case R_SAMFRAN:
				ptr->subState = R_ZOO;
				break;
			case R_WEST:
				ptr->subState = R_SAMFRAN;
				break;		
			case R_TASMANIA:
				ptr->subState = R_WEST;
				break;
			}
		}
		else if(ptr->page->selectedItem == currentLevelPI || ptr->page->selectedItem == ptr->levelRightIcon || ptr->page->selectedItem == ptr->levelLeftIcon)
		{
			switch(ptr->subSubState)
			{
			case R_SUB_SAFARI:
				ptr->subSubState = R_SUB_AQUA;
				break;
			case R_SUB_ICEKINGDOME:
				ptr->subSubState = R_SUB_SAFARI;
				break;
			case R_SUB_AQUA:
				ptr->subSubState = R_SUB_ICEKINGDOME;
				break;

			case R_SUB_DEPTSTR:
				ptr->subSubState = R_SUB_CONSTRUCT;
				break;
			case R_SUB_MUSEUM:
				ptr->subSubState = R_SUB_DEPTSTR;
				break;
			case R_SUB_CONSTRUCT:
				ptr->subSubState = R_SUB_MUSEUM;
				break;

			case R_SUB_CANYON:
				ptr->subSubState = R_SUB_GHOST;
				break;
			case R_SUB_GOLDMINE:
				ptr->subSubState = R_SUB_CANYON;
				break;
			case R_SUB_GHOST:
				ptr->subSubState = R_SUB_GOLDMINE;
				break;
			}
		}
	}

	if (ptr->subSubState != currentSubSubState)
	{
		switch(ptr->subSubState)
		{
		case R_SUB_SAFARI:
			ptr->page->selectItem(ptr->levelControl[0][0]);
			ptr->levelControlBook->gotoPage("safariLevel");
			ptr->overallBook->gotoPage("safari");
			ptr->infoSubBook->gotoPage("INFO_SAFARI");
			break;
		case R_SUB_ICEKINGDOME:
			ptr->page->selectItem(ptr->levelControl[0][1]);
			ptr->levelControlBook->gotoPage("icekingdomeLevel");
			ptr->overallBook->gotoPage("icekingdome");
			ptr->infoSubBook->gotoPage("INFO_ICE");
			break;
		case R_SUB_AQUA:
			ptr->page->selectItem(ptr->levelControl[0][2]);
			ptr->levelControlBook->gotoPage("aquaLevel");
			ptr->overallBook->gotoPage("aqua");
			ptr->infoSubBook->gotoPage("INFO_AQUA");
			break;

		case R_SUB_DEPTSTR:
			ptr->page->selectItem(ptr->levelControl[1][0]);
			ptr->levelControlBook->gotoPage("deptstrLevel");
			ptr->overallBook->gotoPage("deptstr");			
			ptr->infoSubBook->gotoPage("INFO_DEPT");
			break;
		case R_SUB_MUSEUM:
			ptr->page->selectItem(ptr->levelControl[1][1]);
			ptr->levelControlBook->gotoPage("museumLevel");
			ptr->overallBook->gotoPage("museum");
			ptr->infoSubBook->gotoPage("INFO_MUSEUM");
			break;
		case R_SUB_CONSTRUCT:
			ptr->page->selectItem(ptr->levelControl[1][2]);
			ptr->levelControlBook->gotoPage("constructLevel");
			ptr->overallBook->gotoPage("construct");			
			ptr->infoSubBook->gotoPage("INFO_CONSTRUCT");
			break;

		case R_SUB_CANYON:
			ptr->page->selectItem(ptr->levelControl[2][0]);
			ptr->levelControlBook->gotoPage("canyonLevel");
			ptr->overallBook->gotoPage("canyon");
			ptr->infoSubBook->gotoPage("INFO_CANYON");
			break;
		case R_SUB_GOLDMINE:
			ptr->page->selectItem(ptr->levelControl[2][1]);
			ptr->levelControlBook->gotoPage("mineLevel");
			ptr->overallBook->gotoPage("mine");
			ptr->infoSubBook->gotoPage("INFO_MINE");
			break;
		case R_SUB_GHOST:
			ptr->page->selectItem(ptr->levelControl[2][2]);
			ptr->levelControlBook->gotoPage("ghostLevel");
			ptr->overallBook->gotoPage("ghost");
			ptr->infoSubBook->gotoPage("INFO_GHOST");
			break;
		}
	}

	if (ptr->subState != currentSubState)
	{
		switch(ptr->subState)
		{
		case R_OVERALL:
			ptr->page->selectItem(ptr->hubControl[4]);
			ptr->hubControlBook->gotoPage("overallHub");
			ptr->levelControlBook->gotoPage("overallLevel");
			ptr->infoSubBook->gotoPage("INFO_OVERALL");
			ptr->overallBook->gotoPage("overall");
			ptr->leftHubIconBook->gotoPage("off");
			ptr->rightHubIconBook->gotoPage("on");
			ptr->leftLevelIconBook->gotoPage("off");
			ptr->rightLevelIconBook->gotoPage("off");
			ptr->subSubState = R_SUB_OVERALL1;
			break;
		case R_ZOO:
			ptr->page->selectItem(ptr->hubControl[0]);
			ptr->hubControlBook->gotoPage("zooHub");
			ptr->levelControlBook->gotoPage("safariLevel");
			ptr->infoSubBook->gotoPage("INFO_SAFARI");
			ptr->overallBook->gotoPage("safari");
			ptr->leftHubIconBook->gotoPage("on");
			ptr->rightHubIconBook->gotoPage("on");			
			ptr->leftLevelIconBook->gotoPage("on");
			ptr->rightLevelIconBook->gotoPage("on");
			ptr->subSubState = R_SUB_SAFARI;
			break;
		case R_SAMFRAN:
			ptr->page->selectItem(ptr->hubControl[1]);
			ptr->hubControlBook->gotoPage("cityHub");
			ptr->levelControlBook->gotoPage("deptstrLevel");
			ptr->infoSubBook->gotoPage("INFO_DEPT");
			ptr->overallBook->gotoPage("deptstr");
			ptr->leftHubIconBook->gotoPage("on");
			ptr->rightHubIconBook->gotoPage("on");
			ptr->leftLevelIconBook->gotoPage("on");
			ptr->rightLevelIconBook->gotoPage("on");
			ptr->subSubState = R_SUB_DEPTSTR;				
			break;
		case R_WEST:
			ptr->page->selectItem(ptr->hubControl[2]);
			ptr->hubControlBook->gotoPage("westHub");
			ptr->levelControlBook->gotoPage("canyonLevel");
			ptr->infoSubBook->gotoPage("INFO_CANYON");
			ptr->overallBook->gotoPage("canyon");
			ptr->leftHubIconBook->gotoPage("on");
			ptr->rightHubIconBook->gotoPage("on");
			ptr->leftLevelIconBook->gotoPage("on");
			ptr->rightLevelIconBook->gotoPage("on");
			ptr->subSubState = R_SUB_CANYON;				
			break;
		case R_TASMANIA:
			ptr->page->selectItem(ptr->hubControl[3]);
			ptr->hubControlBook->gotoPage("tazHub");
			ptr->levelControlBook->gotoPage("tazLevel");						
			ptr->infoSubBook->gotoPage("INFO_TASMANIA");
			ptr->overallBook->gotoPage("taz");
			ptr->leftHubIconBook->gotoPage("on");
			ptr->rightHubIconBook->gotoPage("off");
			ptr->leftLevelIconBook->gotoPage("off");
			ptr->rightLevelIconBook->gotoPage("off");
			ptr->subSubState = R_SUB_TASMANIA1;			
			break;		
		}
	}
	
	// NH: Re-highlight the main text titles
	// NH: Hub icon selected
	if ((ptr->page->selectedItem == ptr->rightHubIconBook->getCurrentPage()->getFirstItem())||(ptr->page->selectedItem == ptr->leftHubIconBook->getCurrentPage()->getFirstItem())||(ptr->page->selectedItem == currentHubPI))
	{
		if (!currentHubPI->highlighted)
			currentHubPI->highlight(true);
		if (ptr->subState!=R_OVERALL)
		{
			if (currentLevelPI->highlighted)
				currentLevelPI->highlight(false);		
		}
	}

	if ((ptr->page->selectedItem == ptr->rightLevelIconBook->getCurrentPage()->getFirstItem())||(ptr->page->selectedItem == ptr->leftLevelIconBook->getCurrentPage()->getFirstItem())||(ptr->page->selectedItem == currentLevelPI))
	{
		if (ptr->subState!=R_OVERALL)
		{
			if (!currentLevelPI->highlighted)
				currentLevelPI->highlight(true);
		}
		if (currentHubPI->highlighted)
			currentHubPI->highlight(false);		
	}

	//triangle to return back to main menu
	if(CANCELBUTTON)
	{
		// NH: Play bowhomb sound. Would be nice to be in PageFX, but not sure how to do it so this will work for now
		menu->recordsPage->page->playNavSound(menu->recordsPage->page->menuSounds, EMSFX_BACK);
		menu->recordsPage->infoSubBook->gotoPage("INFO_OVERALL");
			
		frontEndAnimInfo.requestState = GOING_HOME;

		menu->currentPage = MENU_MAIN;
		ChooseFrontEndAnimationPath(MENU_MAIN);

		menu->frontEndMenuBook->gotoPage(menuNames[MENU_MAIN]);

		SetFrontEndCameraDestination(FECAM_RECORDS,FECAM_MAIN);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: checkDestructionAndSecretCollectionState
	Purpose 	: Checks to see if either the Destruction bonus has been got or the secret collect
					if so, then the icon is changed to represent this.
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void checkDestructionAndSecretCollectionState(ELevelScores scoreLevelName, int hubNo, int levelNo)
{
	RECORDSPAGE *ptr = frontEndMenu.recordsPage;
	
	if (gameStatus.globalSettings.saveInfo.highScores[scoreLevelName].destructBonusCollected[0])
		ptr->destructableIcon[hubNo][levelNo]->enable(true);
	else
		ptr->destructableIcon[hubNo][levelNo]->enable(false);

	if (gameStatus.globalSettings.saveInfo.highScores[scoreLevelName].secretItemCollected[0])
		ptr->secretIcon[hubNo][levelNo]->enable(true);
	else
		ptr->secretIcon[hubNo][levelNo]->enable(false);
}

/*	--------------------------------------------------------------------------------
	Function 	: compileOverallRecord
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

int compileOverallRecord()
{

	int highestScorer = 1;
	
	memset(&playerRecords[1], 0, sizeof(PLAYER_RECORDS));
	memset(&playerRecords[2], 0, sizeof(PLAYER_RECORDS));
	memset(&playerRecords[3], 0, sizeof(PLAYER_RECORDS));

	compileOverallForLevel(SCENE_SAFARI);
	compileOverallForLevel(SCENE_ICEDOME);
	compileOverallForLevel(SCENE_AQUA);
	compileOverallForLevel(SCENE_DEPTSTR);
	compileOverallForLevel(SCENE_MUSEUM);
	compileOverallForLevel(SCENE_CONSTRUCT);
	compileOverallForLevel(SCENE_GRANDCANYON);
	compileOverallForLevel(SCENE_GOLDMINE);
	compileOverallForLevel(SCENE_GHOSTTOWN);
	compileOverallForLevel(SCENE_TAZHUB);

	for (int player=0; player<3; player++)
	{
		if (gameStatus.globalSettings.saveInfo.saveSlot[player].lastIcon != -1)
		{
			sprintf(playerRecords[player+1].name, "%s", gameStatus.globalSettings.saveInfo.saveSlot[player].lastName);
			playerRecords[player+1].iconRef			= gameStatus.globalSettings.saveInfo.saveSlot[player].lastIcon;
		}
		else
		{
			sprintf(playerRecords[player+1].name, "TAZ");
			playerRecords[player+1].iconRef			= 1;
			playerRecords[player+1].destructsCollected	= 0;
			playerRecords[player+1].pickupsCollected	= 0;
			playerRecords[player+1].postersDestroyed	= 0;
			playerRecords[player+1].secretsCollected	= 0;
			playerRecords[player+1].money				= 0;
		}

		if(gameStatus.globalSettings.saveInfo.saveSlot[player].inUse)
		{
			playerRecords[player+1].money = (float)gameStatus.globalSettings.saveInfo.saveSlot[player].bounty;
		}
		else playerRecords[player+1].money = 0;

		if (playerRecords[player+1].money > playerRecords[highestScorer].money) highestScorer = player+1;
	}

	return highestScorer;
}

/*	--------------------------------------------------------------------------------
	Function 	: compileOverallForLevel
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void compileOverallForLevel(EScene levelToCheck)
{
	for(int player=0; player<3; player++)
	{
		if (gameStatus.globalSettings.saveInfo.saveSlot[player].inUse)
		{
			// NH: Calculate their score
			playerRecords[player+1].money	+= (gameStatus.globalSettings.saveInfo.saveSlot[player].level[levelToCheck].postersCollected * posterBounties[levelToCheck])
											+ (gameStatus.globalSettings.saveInfo.saveSlot[player].level[levelToCheck].secretItemCollected * (posterBounties[levelToCheck] * 2.0f))
											+ (gameStatus.globalSettings.saveInfo.saveSlot[player].level[levelToCheck].timesCaptured);

			if (gameStatus.globalSettings.saveInfo.saveSlot[player].difficulty == 0)
				playerRecords[player+1].money += gameStatus.globalSettings.saveInfo.saveSlot[player].level[levelToCheck].destructBonusCollected * (posterBounties[levelToCheck] * 0.5f);
			else if (gameStatus.globalSettings.saveInfo.saveSlot[player].difficulty == 1)
				playerRecords[player+1].money += gameStatus.globalSettings.saveInfo.saveSlot[player].level[levelToCheck].destructBonusCollected * (posterBounties[levelToCheck] * 0.75f);
			else
				playerRecords[player+1].money += gameStatus.globalSettings.saveInfo.saveSlot[player].level[levelToCheck].destructBonusCollected * (posterBounties[levelToCheck] * 1.0f);

			playerRecords[player+1].destructsCollected	+= gameStatus.globalSettings.saveInfo.saveSlot[player].level[levelToCheck].destructBonusCollected;
			playerRecords[player+1].pickupsCollected	+= gameStatus.globalSettings.saveInfo.saveSlot[player].level[levelToCheck].pickupsCollected;
			playerRecords[player+1].postersDestroyed	+= gameStatus.globalSettings.saveInfo.saveSlot[player].level[levelToCheck].postersCollected;
			playerRecords[player+1].secretsCollected	+= gameStatus.globalSettings.saveInfo.saveSlot[player].level[levelToCheck].secretItemCollected;
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: getHighestScorer
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
uint getHighestScorer(EScene levelToCheck)
{
	int currentScore;
	int highestScorer	= -1;
	int highestScore	= 0;

	// NH: Calculate their score
/*	int highestScore	= (gameStatus.globalSettings.saveInfo.saveSlot[highestScorer].level[levelToCheck].postersCollected * posterBounties[levelToCheck])
						+ (gameStatus.globalSettings.saveInfo.saveSlot[highestScorer].level[levelToCheck].secretItemCollected * (posterBounties[levelToCheck] * 2.0f))
						- (gameStatus.globalSettings.saveInfo.saveSlot[highestScorer].level[levelToCheck].timesCaptured);

	if (gameStatus.globalSettings.saveInfo.saveSlot[highestScorer].difficulty == 0)
		highestScore += gameStatus.globalSettings.saveInfo.saveSlot[highestScorer].level[levelToCheck].destructBonusCollected * (posterBounties[levelToCheck] * 0.5f);
	else if (gameStatus.globalSettings.saveInfo.saveSlot[highestScorer].difficulty == 1)
		highestScore += gameStatus.globalSettings.saveInfo.saveSlot[highestScorer].level[levelToCheck].destructBonusCollected * (posterBounties[levelToCheck] * 0.75f);
	else
		highestScore += gameStatus.globalSettings.saveInfo.saveSlot[highestScorer].level[levelToCheck].destructBonusCollected * (posterBounties[levelToCheck] * 1.0f);
*/	
	// NH: Check each person to see if their score is higher
	for (int i=0; i<3; i++)
	{
		if (gameStatus.globalSettings.saveInfo.saveSlot[i].inUse)
		{
			// NH: Calculate their score
			currentScore	= (gameStatus.globalSettings.saveInfo.saveSlot[i].level[levelToCheck].postersCollected * posterBounties[levelToCheck])
							+ ((int)(gameStatus.globalSettings.saveInfo.saveSlot[i].level[levelToCheck].secretItemCollected) * (posterBounties[levelToCheck] * 2))
							+ (gameStatus.globalSettings.saveInfo.saveSlot[i].level[levelToCheck].timesCaptured);

			if (gameStatus.globalSettings.saveInfo.saveSlot[i].difficulty == 0)
				currentScore += (int)(gameStatus.globalSettings.saveInfo.saveSlot[i].level[levelToCheck].destructBonusCollected) * (posterBounties[levelToCheck]>>1);
			else if (gameStatus.globalSettings.saveInfo.saveSlot[i].difficulty == 1)
				currentScore += (int)(gameStatus.globalSettings.saveInfo.saveSlot[i].level[levelToCheck].destructBonusCollected) * ( (posterBounties[levelToCheck]>>1)+(posterBounties[levelToCheck]>>1>>1) );
			else
				currentScore += (int)(gameStatus.globalSettings.saveInfo.saveSlot[i].level[levelToCheck].destructBonusCollected) * (posterBounties[levelToCheck]);

			if (currentScore > highestScore)
			{
				highestScorer	= i;
				highestScore	= currentScore;
			}
		}
	}

	// NH: If the highest scoring slot was used
	if (highestScorer != -1 && gameStatus.globalSettings.saveInfo.saveSlot[highestScorer].lastIcon != -1)
	{
		sprintf(playerRecords[0].name, "%s", gameStatus.globalSettings.saveInfo.saveSlot[highestScorer].lastName);
		playerRecords[0].iconRef			= gameStatus.globalSettings.saveInfo.saveSlot[highestScorer].lastIcon;
		playerRecords[0].destructsCollected	= gameStatus.globalSettings.saveInfo.saveSlot[highestScorer].level[levelToCheck].destructBonusCollected;
		playerRecords[0].pickupsCollected	= gameStatus.globalSettings.saveInfo.saveSlot[highestScorer].level[levelToCheck].pickupsCollected;
		playerRecords[0].postersDestroyed	= gameStatus.globalSettings.saveInfo.saveSlot[highestScorer].level[levelToCheck].postersCollected;
		playerRecords[0].secretsCollected	= gameStatus.globalSettings.saveInfo.saveSlot[highestScorer].level[levelToCheck].secretItemCollected;
		playerRecords[0].money				= highestScore;
	}
	else
	{
		sprintf(playerRecords[0].name, "TAZ");
		playerRecords[0].iconRef			= 1;
		playerRecords[0].destructsCollected	= 0;
		playerRecords[0].pickupsCollected	= 0;
		playerRecords[0].postersDestroyed	= 0;
		playerRecords[0].secretsCollected	= 0;
		playerRecords[0].money				= 0;
		return 0;
	}
	
	return highestScorer;
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateRecordsMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void SetFrontEndRecordsText(RECORDSPAGE *ptr, uint32 level)
{
/*	for(int i=0;i<5;i++)
	{
		ptr->recordsIcon[i]->setTexture(highScoreIconNames[gameStatus.globalSettings.saveInfo.highScores[level].iconRef[i]]);
		ptr->recordsTb[i][0]->setText(gameStatus.globalSettings.saveInfo.highScores[level].names[i]);
		ptr->recordsTb[i][1]->sprintf8("$ %d",gameStatus.globalSettings.saveInfo.highScores[level].money[i]);
 		if((gameStatus.globalSettings.saveInfo.highScores[level].time[i][1] < 10)&&(gameStatus.globalSettings.saveInfo.highScores[level].time[i][2] < 10))
		{
			ptr->recordsTb[i][2]->sprintf8("%d:0%d:0%d",gameStatus.globalSettings.saveInfo.highScores[level].time[i][0],
				gameStatus.globalSettings.saveInfo.highScores[level].time[i][1],gameStatus.globalSettings.saveInfo.highScores[level].time[i][2]);
		}
		else
		if(gameStatus.globalSettings.saveInfo.highScores[level].time[i][1] < 10)
		{
			ptr->recordsTb[i][2]->sprintf8("%d:0%d:%d",gameStatus.globalSettings.saveInfo.highScores[level].time[i][0],
				gameStatus.globalSettings.saveInfo.highScores[level].time[i][1],gameStatus.globalSettings.saveInfo.highScores[level].time[i][2]);
		}
		else
		if(gameStatus.globalSettings.saveInfo.highScores[level].time[i][2] < 10)
		{
			ptr->recordsTb[i][2]->sprintf8("%d:%d:0%d",gameStatus.globalSettings.saveInfo.highScores[level].time[i][0],
				gameStatus.globalSettings.saveInfo.highScores[level].time[i][1],gameStatus.globalSettings.saveInfo.highScores[level].time[i][2]);
		}
		else
		{
			ptr->recordsTb[i][2]->sprintf8("%d:%d:%d",gameStatus.globalSettings.saveInfo.highScores[level].time[i][0],
				gameStatus.globalSettings.saveInfo.highScores[level].time[i][1],gameStatus.globalSettings.saveInfo.highScores[level].time[i][2]);
		}
	}
	*/
}