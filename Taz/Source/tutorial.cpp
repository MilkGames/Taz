// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : tutorial.cpp
//   Purpose : handle tutorial processing
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "main.h"

#include "actors.h"
#include "status.h"
#include "control.h"
#include "subtitle.h"

#include "text.h"
#include "tutorial.h"
#include "collectibles.h"
#include "animation.h"
#include "XboxStrings.h"			// PP: Xbox versions of some strings

EStringIndex tutorialBookChapters[4][7] =
{
	{STR_TUTORIALBOOK_CHAPTER1_1,
	 STR_TUTORIALBOOK_CHAPTER1_2,
	 STR_TUTORIALBOOK_CHAPTER1_3,
	 STR_TUTORIALBOOK_CHAPTER1_4,
	 STR_TUTORIALBOOK_CHAPTER1_5},
	
	{STR_TUTORIALBOOK_CHAPTER2_1,
	 STR_TUTORIALBOOK_CHAPTER2_2,
	 STR_TUTORIALBOOK_CHAPTER2_3,
	 STR_TUTORIALBOOK_CHAPTER2_4,
	 STR_TUTORIALBOOK_CHAPTER2_5,
	 STR_TUTORIALBOOK_CHAPTER2_6,
	 STR_TUTORIALBOOK_CHAPTER2_7},

	{STR_TUTORIALBOOK_CHAPTER3_1,
	 STR_TUTORIALBOOK_CHAPTER3_2,
	 STR_TUTORIALBOOK_CHAPTER3_3,
	 STR_TUTORIALBOOK_CHAPTER3_4},

	{STR_TUTORIALBOOK_CHAPTER4_1,
	 STR_TUTORIALBOOK_CHAPTER4_2,
	 STR_TUTORIALBOOK_CHAPTER4_3,
	 STR_TUTORIALBOOK_CHAPTER4_4,
	 STR_TUTORIALBOOK_CHAPTER4_5,
	 STR_TUTORIALBOOK_CHAPTER4_6},
};

EStringIndex tutorialBookText[4][7] =
{
	{STR_TUTORIALBOOK1_1,
	 STR_TUTORIALBOOK1_2,
	 STR_TUTORIALBOOK1_3,
	 STR_TUTORIALBOOK1_4,
	 STR_TUTORIALBOOK1_5},
	
	{STR_TUTORIALBOOK2_1,
	 STR_TUTORIALBOOK2_2,
	 STR_TUTORIALBOOK2_3,
	 STR_TUTORIALBOOK2_4,
	 STR_TUTORIALBOOK2_5,
	 STR_TUTORIALBOOK2_6,
	 STR_TUTORIALBOOK2_7},

	{STR_TUTORIALBOOK3_1,
	 STR_TUTORIALBOOK3_2,
	 STR_TUTORIALBOOK3_3,
	 STR_TUTORIALBOOK3_4},

	{STR_TUTORIALBOOK4_1,
	 STR_TUTORIALBOOK4_2,
	 STR_TUTORIALBOOK4_3,
	 STR_TUTORIALBOOK4_4,
	 STR_TUTORIALBOOK4_5,
	 STR_TUTORIALBOOK4_6},
};

struct	TUTORIALINFO
{
	char	tutStage;
	
	char	tutReadOne:1;
	char	tutReadTwo:1;
	char	tutReadThr:1;
	char	tutReadFor:1;
	char	tutReadFve:1;
	char	tutReadSix:1;
	char	tutReadSvn:1;
	char	tutReadEgh:1;

	char	tutFood;
	char	tutObjects;
}	tutInfo[4];
	
float	subTimer;

PAPERBOOK	*tutorialBook;					// PP: paper book containing tutorials - each tutorial is a different chapter
BOOK		*tutorialInstructionBook;		// PP: 2D book showing navigation instructions for the tutorial paper-book
BOOK		*tutorialControlBook;			// NH: 2D book showing control instructions for the tutorial paper-book
TEXTBOX		*tutorialBackGlyph;
TEXTBOX		*tutorialForwardGlyph;

/*	--------------------------------------------------------------------------------
	Function 	: Tutorial_paperBookCloseCallback
	Purpose 	: receives notification that the tutorial paper-book has closed
	Parameters 	: ptr to paperbook that has closed
	Returns 	: 
	Info 		: // PP: must match the PaperBookCloseCallback typedef (PaperBook.h)
*/
void Tutorial_paperBookCloseCallback(PAPERBOOK* const paperBook)
{
	// TP: needed to reset these otherwise if you exit the book on the last page and then re-enter both arrows are disabled
	if (tutorialForwardGlyph) tutorialForwardGlyph->enable(true);
	if (tutorialBackGlyph) tutorialBackGlyph->enable(true);

	gameStatus.controlSuspend = false;
	
	// PP: close the 2D instruction overlay
	tutorialInstructionBook->close();// PP: REMOUT: just for fun		0.0f, true);
	tutorialControlBook->close();

	// PP: JW: player just looked at book and can't read it now
	gameStatus.player1->actorStatus->tutorialBookFlag = -1;

	// PP: return to in-game state
	SetGameState(&gameStatus, GAMESTATE_INGAME, GAMESUBSTATE_NORMAL);

	OpenTutInfBook();
}


void initTutorial()
{
	int	i;

	for (i = 0; i < 4; i++)
	{
		tutInfo[i].tutStage		= 0;
		
		tutInfo[i].tutReadOne	= 0;
		tutInfo[i].tutReadTwo	= 0;
		tutInfo[i].tutReadThr	= 0;
		tutInfo[i].tutReadFor	= 0;
		tutInfo[i].tutReadFve	= 0;
		tutInfo[i].tutReadSix	= 0;
		tutInfo[i].tutReadSvn	= 0;
		tutInfo[i].tutReadEgh	= 0;

		tutInfo[i].tutFood		= 0;
		tutInfo[i].tutObjects	= 0;
	}

	GetTutorialAreaInfo();

	//LoadSplashScreen(&tutorialScreenTwo, "extras\\tazhelp.bmp", map.mapName);

	tutorialBook = new PAPERBOOK("TUTORIAL PAPERBOOK");
	tutorialInstructionBook = new BOOK("TUTORIAL INSTRUCTION BOOK");
	tutorialControlBook = new BOOK("TUTORIAL CONTROL BOOK");

	createTutorialBookPages();

	// PP: register our callback for when the tutorial paper-book closes
	tutorialBook->setCloseCallback(Tutorial_paperBookCloseCallback);

	//LoadSplashScreen(&tutorialScreenThr, "extras\\tazhelp.bmp", map.mapName);
	//LoadSplashScreen(&tutorialScreenFor, "extras\\tazhelp.bmp", map.mapName);
}

int doTutorial(int tutNumber)
{
	ACTORINSTANCE	*pTutPoint;

	TBVector		position;

	float			distToTut;

	if (gameStatus.player1->actorStatus->tutorialBookFlag == 1 && tutNumber > -1)
	{	// JW: Player is reading the book		
		if (tutInfo[tutNumber].tutStage == 1  && (subTimer < gameStatus.gameTime || OKBUTTON))
		{
			CloseCurrentSubtitle();
			InitTutorialBook(tutNumber);
		}
/* PP: REMOUT: OOD
		if (tutInfo[0].tutStage == 2)
			DrawTutorialBookOne();*/
	}

	if (gameStatus.player1->actorStatus->tutorialBookFlag == -1)
	{// JW: player just looked at book and can't read it now
	}
		
	if (gameStatus.player1->actorStatus->tutorialBookFlag == 0)
	{	// JW: Normal
		if (subTimer > gameStatus.gameTime && OKBUTTON)
			CloseCurrentSubtitle();
	}

	return tutNumber;
}


/*	--------------------------------------------------------------------------------
	Function 	: drawTutorial
	Purpose 	: draw tutorial stuff, eg. tutorial paperbook
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: separated this from doTutorial because it was getting drawn at update stage -
							problematic in a number of ways, most importantly that the level was drawing after it & therefore on top of it
*/
/* PP: REMOUT: Now handled by the Book List	
void drawTutorial(void)
{
	bdSetIdentityObjectMatrix();
	
//	SAVESTATES;

// PP: REMOUT: Now handled by the PaperBook system itself			tutorialBook->privateDraw();// PP: PAPERBOOK

	// PP: draw navigation instructions...

	SETPROJECTION(2D);
	SETZTEST(ALWAYS);

//	tutorialInstructionBook->privateDraw();

	SETPROJECTION(3D);

	RESTORESTATES;
}*/


void InitTutorialBook(int current)
{
/*	switch(current)
	{
	case 0:		tutorialBook->gotoPage("tutorial1page1");
				break;

	case 1:		tutorialBook->gotoPage("tutorial2page1");
				break;

	case 2:		tutorialBook->gotoPage("tutorial3page1");
				break;

	case 3:		tutorialBook->gotoPage("tutorial4page1");
				break;
	}*/

	tutorialBook->restrictToChapter(current);

	tutorialBook->open();
	tutorialInstructionBook->open();// PP: open instructions book too
	tutorialControlBook->open();

	tutorialBook->setFirstPageCallback(tutorialFirstPageCallback);
	tutorialBook->setCentralPageCallback(tutorialCentralPageCallback);
	tutorialBook->setLastPageCallback(tutorialLastPageCallback);

	tutInfo[current].tutStage = 2;
}

void EnterTutorialArea(int current)
{
	CloseCurrentSubtitle();

	// TP: flush any rumbles present on opening book
	FlushRumbleQueue(&controller1);
	FlushRumbleQueue(&controller2);

	gameStatus.controlSuspend = true;

	if (!tutInfo[current].tutStage)
	{
		switch(current)
		{
		case 0:		Subtitle(STR_TUTORIAL1_1, 4.3f);
					subTimer = gameStatus.gameTime + 4.9f;
					break;

		case 1:		Subtitle(STR_TUTORIAL2_1, 3.2f);
					subTimer = gameStatus.gameTime + 3.7f;
					break;

		case 2:		Subtitle(STR_TUTORIAL3_1, 4.6f);
					subTimer = gameStatus.gameTime + 5.2f;
					break;

		case 3:		Subtitle(STR_TUTORIAL4_1, 4.9f);
					subTimer = gameStatus.gameTime + 5.5f;
					break;
		}

		tutInfo[current].tutStage = 1;
	}
	else
	{
		InitTutorialBook(current);
	}
}

void ExitTutorialArea(int current)
{
	CloseCurrentSubtitle();

	if (tutInfo[current].tutStage)
	{
		switch(current)
		{
		case 0:		ResetTutorialOne();
					Subtitle(STR_TUTORIAL1_2, 4.4f);
					subTimer = gameStatus.gameTime + 5.0f;
					break;

		case 1:		Subtitle(STR_TUTORIAL2_2, 6.5f);
					subTimer = gameStatus.gameTime + 7.1f;
					break;

		case 2:		ResetTutorialThree();
					Subtitle(STR_TUTORIAL3_2, 2.7f);
					subTimer = gameStatus.gameTime + 3.3f;
					break;

		case 3:		Subtitle(STR_TUTORIAL4_2, 3.9f);
					subTimer = gameStatus.gameTime + 4.5f;
					break;
		}

		tutInfo[current].tutStage = 0;
	}
}

/* PP: REMOUT: duplicated code

int doTutorialOne()
{
	ACTORINSTANCE *pTutPoint;

	TBVector	position;

	float		distToTut;

	if (gameStatus.player1->actorStatus->tutorialBookFlag == 1)
	{	// JW: Player is reading the book		
		if (tutInfo[0].tutStage == 1  && subTimer < gameStatus.gameTime)
			InitTutorialBook(0);

		if (tutInfo[0].tutStage == 2)
			DrawTutorialBookOne();
	}

	if (gameStatus.player1->actorStatus->tutorialBookFlag == -1)
	{// JW: player just looked at book and can't read it now
	}
		
	if (gameStatus.player1->actorStatus->tutorialBookFlag == 0)
	{	// JW: Normal
	}

	return 0;
}

int doTutorialTwo()
{
	ACTORINSTANCE *pTutPoint;

	TBVector	tempVector;

	float		distToTut;

	if (gameStatus.player1->actorStatus->tutorialBookFlag == 1)
	{	// JW: Player is reading the book		
		if (tutInfo[1].tutStage == 1  && subTimer < gameStatus.gameTime)
			InitTutorialBook(1);

		if (tutInfo[1].tutStage == 2)
			DrawTutorialBookTwo();
	}

	if (gameStatus.player1->actorStatus->tutorialBookFlag == -1)
	{// JW: player just looked at book and can't read it now
	}
		
	if (gameStatus.player1->actorStatus->tutorialBookFlag == 0)
	{	// JW: Normal
	}

	return 0;
}

int doTutorialThree()
{
	ACTORINSTANCE *pTutPoint;

	TBVector	tempVector;

	float		distToTut;

	if (gameStatus.player1->actorStatus->tutorialBookFlag == 1)
	{	// JW: Player is reading the book		
		if (tutInfo[2].tutStage == 1  && subTimer < gameStatus.gameTime)
			InitTutorialBook(2);

		if (tutInfo[2].tutStage == 2)
			DrawTutorialBookThree();
	}

	if (gameStatus.player1->actorStatus->tutorialBookFlag == -1)
	{// JW: player just looked at book and can't read it now
	}
		
	if (gameStatus.player1->actorStatus->tutorialBookFlag == 0)
	{	// JW: Normal
	}

	return 0;
}

int doTutorialFour()
{
	ACTORINSTANCE *pTutPoint;

	TBVector	position;

	float		distToTut;

	if (gameStatus.player1->actorStatus->tutorialBookFlag == 1)
	{	// JW: Player is reading the book		
		if (tutInfo[3].tutStage == 1  && subTimer < gameStatus.gameTime)
			InitTutorialBook(3);

		if (tutInfo[3].tutStage == 2)
			DrawTutorialBookFour();
	}

	if (gameStatus.player1->actorStatus->tutorialBookFlag == -1)
	{// JW: player just looked at book and can't read it now
	}
		
	if (gameStatus.player1->actorStatus->tutorialBookFlag == 0)
	{	// JW: Normal
	}

	return 0;
}
*/

/* PP: REMOUT: OOD and duplicated code

// PP: Macro to close the tutorial books (this is done in more than one place)
#define CLOSE_TUTORIAL_BOOKS			tutorialBook->close(); \
										tutorialInstructionBook->close(0.0f, true);



void DrawTutorialBookOne()
{
	if (CANCELBUTTON)
	{
		gameStatus.player1->actorStatus->tutorialBookFlag = -1;
		gameStatus.controlSuspend = false;

		CLOSE_TUTORIAL_BOOKS;
	}
}

void DrawTutorialBookTwo()
{
	if (CANCELBUTTON)
	{
		gameStatus.player1->actorStatus->tutorialBookFlag = -1;
		gameStatus.controlSuspend = false;

		CLOSE_TUTORIAL_BOOKS;
	}
}

void DrawTutorialBookThree()
{
	if (CANCELBUTTON)
	{
		gameStatus.player1->actorStatus->tutorialBookFlag = -1;
		gameStatus.controlSuspend = false;

		CLOSE_TUTORIAL_BOOKS;
	}
}

void DrawTutorialBookFour()
{
	if (CANCELBUTTON)
	{
		gameStatus.player1->actorStatus->tutorialBookFlag = -1;
		gameStatus.controlSuspend = false;

		CLOSE_TUTORIAL_BOOKS;
	}
}
*/


float TUTORIALBOOK_SCALE_PHOTO=				(3.5f);						// PP: scale factor for photo icons


/*	--------------------------------------------------------------------------------
	Function 	: AddTutorialChapterText
	Purpose 	: add a page of text covering a topic in a tutorial book
	Parameters 	: ptr to page to fill in, header string index, body text index
	Returns 	: 
	Info 		: // PP: moved this here from the header (more convenient here)
*/
void AddTutorialChapterText(PAGE *page, int string, int string2)
{
	TEXTBOX	*textPtr;

	/* PP: Insert chapter heading, eg. STR_TUTORIALBOOK_CHAPTER1_1 */ 
	// PP: page->insertFreeItem(textPtr = new TEXTBOX(), UORECT);
	page->insertItem(textPtr = new TEXTBOX());
	
	textPtr->setText((EStringIndex) string);
	textPtr->setWrap(true);// PP: needed for those silly foreigners with their big long silly words
	textPtr->setYPadding(false);
	textPtr->setStyle(TS_paperBookHeading);
	textPtr->setXAlign(PIFLAG_XALIGN_CENTRE);

	/* PP: Insert chapter body text, eg. STR_TUTORIALBOOK1_1 */
	page->insertItem(textPtr = new TEXTBOX());

	textPtr->setText((EStringIndex) string2);
	textPtr->setWrap(true);
	textPtr->setStyle(TS_paperBookBody);
	textPtr->setXAlign(PIFLAG_XALIGN_CENTRE);// PP: font is too big for left alignment
	textPtr->setYAlign(PIFLAG_YALIGN_CENTRE);
}


void createTutorialBookPages()
{
	PAGE		*page;
	PAGE		*subpagePtr;
				
	TEXTBOX		*textPtr;
				
	ICON		*iconPtr;
				
	CHECKBOX	*chkPtr;

		// PP: Macro to add a chapter's text to a tutorial
#define TUTORIAL_CHAPTER_TEXT(_tutorialNumber, _chapterNumber) \
		{ \
			page = tutorialBook->addPage("tutorial"#_tutorialNumber##"page"#_chapterNumber); \
			\
			AddTutorialChapterText(page,tutorialBookChapters[(int) _tutorialNumber][(int) _chapterNumber],tutorialBookText[(int) _tutorialNumber][(int) _chapterNumber]); \
		}

		// PP: Macro to add a chapter to a tutorial
#define TUTORIAL_CHAPTER(_tutorialNumber, _chapterNumber, _picture) \
		{ \
			/* PP: Add the text on the left hand page */ \
			TUTORIAL_CHAPTER_TEXT(_tutorialNumber, _chapterNumber); \
			\
			page=tutorialBook->addPage(); \
			/* PP: Add the picture on the right hand page... */ \
			\
			page->insertItem(iconPtr = new ICON()); \
			\
			iconPtr->setTexture("extras\\"#_picture##".bmp"); \
			iconPtr->setScale(TUTORIALBOOK_SCALE_PHOTO); \
			iconPtr->tilt = false; \
			iconPtr->setXAlign(PIFLAG_XALIGN_CENTRE); \
			iconPtr->setYAlign(PIFLAG_YALIGN_CENTRE); \
		}

	/* PP: *** NOTE ****
	
	I reckon this paper book uses about 40kb just for its paper info.
	I'm going to change the paper books so they only store paper info for *visible* pages.

	*******************/

	// PP: specify the button(s) that can be used to close the paper book
	tutorialBook->setCloseButton(PBCLOSEBUTTON_BACK);// PP: back, because we're *returning* to gameplay

	// PP: LAZY BODGE (but there are after all only 2 paperbooks in the whole game)
	// PP: Add 2 blank pages at the beginning of the book.
	// PP: Otherwise, you won't see a left-hand shadow when opening the first chapter.
	tutorialBook->addPage("blank1");
	tutorialBook->addPage("blank2");

	// PP: OOPS: I've started using the term 'chapter' to mean different things at different times - let me know if you're confused and I'll explain it

	// PP: Tutorial 1
	tutorialBook->startChapter("Tutorial 1");
	TUTORIAL_CHAPTER(0, 0, zoom);
	TUTORIAL_CHAPTER(0, 1, chomp);
	TUTORIAL_CHAPTER(0, 2, spit);
	TUTORIAL_CHAPTER(0, 3, 3hit);
	TUTORIAL_CHAPTER(0, 4, clang);

	// PP: Tutorial 2
	tutorialBook->startChapter("Tutorial 2");
	TUTORIAL_CHAPTER(1, 0, cant swim);
	TUTORIAL_CHAPTER(1, 1, jump);
	TUTORIAL_CHAPTER(1, 2, bouncy rings);
	TUTORIAL_CHAPTER(1, 3, sinking lilys);
	TUTORIAL_CHAPTER(1, 4, tilting logs);
	TUTORIAL_CHAPTER(1, 5, tiptoe);
	TUTORIAL_CHAPTER(1, 6, floaty logs);

	// PP: Tutorial 3
	tutorialBook->startChapter("Tutorial 3");
	TUTORIAL_CHAPTER(2, 0, tazwantedphoto);
	TUTORIAL_CHAPTER(2, 1, whacked);
	TUTORIAL_CHAPTER(2, 2, spinpad);
	TUTORIAL_CHAPTER(2, 3, destroywanted);

	// PP: Tutorial 4
	tutorialBook->startChapter("Tutorial 4");
	TUTORIAL_CHAPTER(3, 0, enemies);
	TUTORIAL_CHAPTER(3, 1, rant);
	TUTORIAL_CHAPTER(3, 2, guardians);
	TUTORIAL_CHAPTER(3, 3, caged);
	TUTORIAL_CHAPTER(3, 4, phonebox);
	TUTORIAL_CHAPTER(3, 5, costume attack);

	// PP: Also set up the instruction book here...

	page=tutorialInstructionBook->addPage();

	tutorialInstructionBook->setBookRect(RECTANGLE(-0.5f, 0.5f, -0.45f, 0.45f));

	page->insertItem(textPtr = new TEXTBOX(STR_DIALOGUE_TRIANGLECLOSE));
	textPtr->setYAlign(PIFLAG_YALIGN_BOTTOM);
	textPtr->setStyle(TS_control);



	
	page=tutorialControlBook->addPage("CONTROL")
		->setHSlack(false);

	tutorialControlBook->setBookRect(RECTANGLE(-0.5f, 0.5f, -0.5f, 0.45f));

	page->insertItem(tutorialBackGlyph = new TEXTBOX(STR_BROWSE_PAGES_LEFT_ONLY));
	tutorialBackGlyph->setStyle(TS_control)
		->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->enable(false)
	//->setYPadding(false)
		->setSelectable(false);

	page->insertItem(STR_BROWSE_PAGES_TEXT_ONLY, false)
		->setStyle(TS_control)
		->setYAlign(PIFLAG_YALIGN_BOTTOM)
	//->setYPadding(false)
		->setAlignScale(1.07f)
		->setSelectable(false);

	page->insertItem(tutorialForwardGlyph = new TEXTBOX(STR_BROWSE_PAGES_RIGHT_ONLY), false);
	tutorialForwardGlyph->setStyle(TS_control)
		->setYAlign(PIFLAG_YALIGN_BOTTOM)
	//->setYPadding(false)
		->setSelectable(false);


/*




	page=tutorialControlBook->addPage("CONTROL_RIGHT_ONLY");

	tutorialControlBook->setBookRect(RECTANGLE(-0.5f, 0.5f, -0.5f, 0.45f));

	page->insertItem(textPtr = new TEXTBOX(STR_BROWSE_PAGES_RIGHT_ONLY));
	textPtr->setYAlign(PIFLAG_YALIGN_BOTTOM);
	//textPtr->setYPadding(false);
	textPtr->setStyle(TS_control);

	page=tutorialControlBook->addPage("CONTROL_ALL");

	tutorialControlBook->setBookRect(RECTANGLE(-0.5f, 0.5f, -0.5f, 0.45f));

	page->insertItem(textPtr = new TEXTBOX(STR_BROWSETUTORIAL));
	textPtr->setYAlign(PIFLAG_YALIGN_BOTTOM);
	//textPtr->setYPadding(false);
	textPtr->setStyle(TS_control);

	page=tutorialControlBook->addPage("CONTROL_LEFT_ONLY");

	tutorialControlBook->setBookRect(RECTANGLE(-0.5f, 0.5f, -0.5f, 0.45f));

	page->insertItem(textPtr = new TEXTBOX(STR_BROWSE_PAGES_LEFT_ONLY));
	textPtr->setYAlign(PIFLAG_YALIGN_BOTTOM);
	//textPtr->setYPadding(false);
	textPtr->setStyle(TS_control);
	*/
}


/*	--------------------------------------------------------------------------------
	Function 	: tutorialFirstPageCallback
	Purpose 	: receives notification that the tutorial has turned/opened on the first page
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void tutorialFirstPageCallback(void)
{
	tutorialBackGlyph->enable(false);
}

/*	--------------------------------------------------------------------------------
	Function 	: tutorialCentralPageCallback
	Purpose 	: receives notification that the tutorial has turned/opened on a central page
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void tutorialCentralPageCallback(void)
{
	tutorialForwardGlyph->enable(true);
	tutorialBackGlyph->enable(true);
}

/*	--------------------------------------------------------------------------------
	Function 	: tutorialLastPageCallback
	Purpose 	: receives notification that the tutorial has turned/opened on the last page
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void tutorialLastPageCallback(void)
{
	tutorialForwardGlyph->enable(false);
}

void KillTutorial()
{
	delete(tutorialBook);
	delete(tutorialInstructionBook);
	delete(tutorialControlBook);
}

