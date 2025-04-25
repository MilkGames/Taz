// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : gallerymenu.cpp
//   Purpose : functions to create and control the gallery menu
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "globalsettings.h"
#include "TextFX.h"
#include "control.h"
#include "frontendmain.h"
#include "VideoFX.h"				// PP: Video effects - motion blur, defocus, whatever.  Top-of-the-Pops-TASTIC, Mate!
#include "files.h"
#include "display.h"
#include "PaperBook.h"				// PP: 'Paper' book type, ie. one that looks and acts like a real physical book
#include "SplashScreen.h"
#include "Cheats.h"					// PP: Cheats

#define GALLERY_IMAGES 10

// PP: define GALLERY_PAGE_NUMBERS to put page numbers on the pages of the gallery paperbook
// PP: REMOUT: gallery paperbook is now ICON-ONLY; see PaperBook.h		#define GALLERY_PAGE_NUMBERS

SPLASHSCREEN galleryScreen;
RECTANGLE	 galleryRect;
TBPrimVertex	overlayVerts[4];
COLOUR			overlayColour=COLOUR(128,128,128,128);

TEXTBOX		*forwardGlyph;
TEXTBOX		*backGlyph;

// NH: Gallery Book Images (FORMAT: [GalleryPos][ImagePos][ImageName])  If you can't fill all 10 spaces don't worry, the code wont show any blanks
char	galleryImages[10][10][30] = {"docks generic01","docks generic02","generic01","generic5","hubobjects2","hubobjects3","skateboarder","surfer","tazcave","watertower",
									"consmixertruck","conssupplydepot","construction worker","conswreckin","hotdog01","museum and constructionsite","museumtoiletinterior","museumvangough","ninja","swat",
									"badguy guard","breakablescity01","cementmixer","citycomposite","dept and tv","depttnt","depttvplug","depttvstore","wireframe ghostcavern","vehicles",
									"acmetumbleweedgenerator","badguy devildog","buildingtaz","destructibleswest02","ghosthouse","ghosthousestuff","ghosttown","trainstation","waterwheelhouse","werewolf",
									"goldmine","indiana","mineacmemint","minecart","minecartsilo","miner","minestuff","minetrackstuff","rocketcart","taz walk",
									"acmebouldercreator","acmeelasticband","bandit","destructibleswest01","grandcanyon","highexstuff","ninja art","rocketbike","samsammodump","taz pose",
									"badguy tribal","desertcomposite","tarzan","treevilleairtcontroltower","treevilleappletree","treevillelifttree","treevillenest","treevillesawmill","treevillestatues","treevilletowerbloktree",
									"reindeer","shedevil","surfer art","tazhubtreevilleguardhut","tazhubtriangleportal","tazhubvolctemple","taztritanker","treevilleaircentre","treevilleairwing","volcwindpassage",
									"snowboard","stuntman","voodoo","conworker","docker","dynamite","guard","jack","keepers","keepers2",
									"bossgossamer","explorer","mechatweety","swat art","trippy","baditems","snax","taz look","tazchase","wireframe01"};

// NH: The names of the gallery paks (and the page name)
char	galleyName[10][11] =		{"gallery_1","gallery_2","gallery_3","gallery_4","gallery_5","gallery_6","gallery_7","gallery_8","gallery_9","gallery_10"};

// NH: The amount of credits required to open each gallery
uint32	galleryCost[10] =			{10000,30000,60000,110000,185000,285000,535000,1035000,1785000,2785000};

int		galleryNo = -1;

/*	--------------------------------------------------------------------------------
	Function 	: AddGalleryIcon
	Purpose 	: 
	Parameters 	: Adds Icons to the gallery page
	Returns 	: 
	Info 		:
*/

bool AddGalleryIcon(struct TAG_GALLERYPAGE *ptr ,int iconNo, bool openFlag1, bool openFlag2, bool openFlag3, bool newLine)
{
	PAGE *pagePtr = ptr->page;

	// NH: Add Gallery Icon
	if ((openFlag1) || (openFlag2) || (openFlag3)
		|| (CheatActive(CHEAT_OPEN_GALLERY)))
	{
		pagePtr->insertItem(ptr->galleryIcon[iconNo] = new ICON("galleryunlocked.bmp"), newLine)->setSelectable(true);
		return false;
	}	
	else 
	{
		pagePtr->insertItem(ptr->galleryIcon[iconNo] = new ICON("gallerylocked.bmp"), newLine)->setSelectable(true);
		return true;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: AddGalleryChapter
	Purpose 	: Add a gallery chapter to the Gallery Paperbook (Puts all graphics defined in galleryImages and text on the page)
	Parameters 	: ptr (Pointer to the Gallery Page), galleryTitle (Title of gallery), galleryNo (Number of gallery)
	Returns 	: 
	Info 		:
*/

void AddGalleryChapter(struct TAG_GALLERYPAGE *ptr, char* galleryTitle, int galleryNo)
{
	PAGE		*pagePtr = ptr->page;
	PAPERBOOK	*galleryBook = ptr->galleryBook;
	TEXTBOX		*textPtr;
	char		tempImageString[50];
	int			currentImage = 0;
	char		pageNoText[3];

	RECTANGLE	freePos;

	freePos.left	= -0.5f;
	freePos.right	= 0.5f;
	freePos.top		= 0.3f;
	freePos.bottom	= -0.5f;

	galleryBook->setFirstPageCallback(galleryFirstPageCallback);
	galleryBook->setCentralPageCallback(galleryCentralPageCallback);
	galleryBook->setLastPageCallback(galleryLastPageCallback);

#define GALLERYBOOK_SCALE_IMAGE			3.5f // NH: Shrunk it slightly as it makes them look higher quality and fits them on the page on PS2 without any major changes.  This way if you can't change anything, Pesti will kinda be happy!  8) //4.25f					// PP: scale factor for image icons

#ifdef GALLERY_PAGE_NUMBERS// PP: 
#define GALLERY_CHAPTER_TEXTBOX \
		pagePtr->insertItem(textPtr = new TEXTBOX()); \
		textPtr->setXAlign(PIFLAG_XALIGN_RIGHT); \
		textPtr->setYAlign(PIFLAG_YALIGN_BOTTOM);
#else// PP: ifndef GALLERY_PAGE_NUMBERS
#define GALLERY_CHAPTER_TEXTBOX
#endif// PP: GALLERY_PAGE_NUMBERS

	// NH: Macro to add a chapter to the gallery
#define GALLERY_CHAPTER(_page)/*, _picture)*/ \
	{ \
		pagePtr=ptr->galleryBook->addPage(); /* Adding a blank (underside) page */ \
		pagePtr=ptr->galleryBook->addPage(); \
		\
		pagePtr->insertFreeItem(ptr->galleryImage[_page] = new ICON(),freePos); \
		\
		/*ptr->galleryImage[_page]->setTexture("gallery\\surfer.bmp");*/ \
		ptr->galleryImage[_page]->setScale(GALLERYBOOK_SCALE_IMAGE); \
		ptr->galleryImage[_page]->tilt = false; \
		ptr->galleryImage[_page]->setXAlign(PIFLAG_XALIGN_CENTRE); \
		ptr->galleryImage[_page]->setYAlign(PIFLAG_YALIGN_CENTRE); \
		GALLERY_CHAPTER_TEXTBOX; \
	}

	
	ptr->galleryBook->startChapter("GALLERY");
//	ptr->galleryBook->startChapter(galleryTitle);
	while (currentImage < GALLERY_IMAGES)// && (strcmp(&galleryImages[galleryNo][currentImage][0], "") != 0))
	{
//		strcpy(tempImageString, "gallery\\");
//		strcat(tempImageString, &galleryImages[galleryNo][currentImage][0]);
//		strcat(tempImageString, ".bmp");
		GALLERY_CHAPTER(currentImage);//, tempImageString);

#ifdef GALLERY_PAGE_NUMBERS// PP: 

		// TP: ps2 libraries doesn't contain _itoa, this should do same thing // NH: Ah, Right! I'll try and remember that one!
		sprintf(pageNoText,"%d",currentImage+1);
		// TP: _itoa(currentImage+1, pageNoText, 10);
		
		textPtr->setText(pageNoText);
		textPtr->setStyle(TS_paperBookBody);

#endif// PP: def GALLERY_PAGE_NUMBERS

		currentImage++;
	}			
}

/*	--------------------------------------------------------------------------------
	Function 	: CreateGalleryMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void CreateGalleryMenuPage(FRONTENDMENU *menu)
{
	GALLERYPAGE		*ptr;
	PAGE			*pagePtr, *subPage;
	TEXTBOX			*dummyBox;
	TEXTBOX			*textPtr;

	SAVEINFO		*SaveInfo = &gameStatus.globalSettings.saveInfo;

	bool noneAvailable = true;

	ptr = menu->galleryPage = (GALLERYPAGE*)ZALLOC(sizeof(GALLERYPAGE));
	if(!ptr)
	{
		bkPrintf("*** WARNING ***, could not malloc gallery page/n");
		return;
	}
	// NH: Add Page
	pagePtr = ptr->page = menu->frontEndMenuBook->addPage(menuNames[MENU_GALLERY]);
	pagePtr->setNavigation(true);

	// NH: Add Title
	pagePtr->insertItem(ptr->titleTb = new TEXTBOX(STR_EX_GALLERY),false);
	ptr->titleTb->setStyle(TS_title)
		->setYAlign(PIFLAG_YALIGN_TOP)
		->setSelectable(false);
	
	// NH: Add Icons to page & determine if any are unlocked

	// PP: I had a bunch of short-to-bool warnings, and thought I'd remove the duplication while getting rid of them - hope you don't mind...
	// NH: Thats ok, was gonna come back and do it anyway, you just saved me the bother!  8) Had to add _newline though, you must have forgot.
#define ADD_GALLERY_ICON(_index, _num, _newline) \
	if (!AddGalleryIcon(ptr, \
						_index, \
						(SaveInfo->galleryFlags.galleryFlag##_num != 0), \
						(SaveInfo->galleryFlags.galleryFlag##_num != 0), \
						(SaveInfo->galleryFlags.galleryFlag##_num != 0), \
						_newline)) \
	{ \
		noneAvailable = false; \
	}

	AddGalleryIcon(ptr, 0, SaveInfo->galleryFlags.galleryFlags[0], SaveInfo->galleryFlags.galleryFlags[0], SaveInfo->galleryFlags.galleryFlags[0], true);
	AddGalleryIcon(ptr, 1, SaveInfo->galleryFlags.galleryFlags[1], SaveInfo->galleryFlags.galleryFlags[1], SaveInfo->galleryFlags.galleryFlags[1], false);
	AddGalleryIcon(ptr, 2, SaveInfo->galleryFlags.galleryFlags[2], SaveInfo->galleryFlags.galleryFlags[2], SaveInfo->galleryFlags.galleryFlags[2], false);
	AddGalleryIcon(ptr, 3, SaveInfo->galleryFlags.galleryFlags[3], SaveInfo->galleryFlags.galleryFlags[3], SaveInfo->galleryFlags.galleryFlags[3], false);
	AddGalleryIcon(ptr, 4, SaveInfo->galleryFlags.galleryFlags[4], SaveInfo->galleryFlags.galleryFlags[4], SaveInfo->galleryFlags.galleryFlags[4], false);
	AddGalleryIcon(ptr, 5, SaveInfo->galleryFlags.galleryFlags[5], SaveInfo->galleryFlags.galleryFlags[5], SaveInfo->galleryFlags.galleryFlags[5], true);
	AddGalleryIcon(ptr, 6, SaveInfo->galleryFlags.galleryFlags[6], SaveInfo->galleryFlags.galleryFlags[6], SaveInfo->galleryFlags.galleryFlags[6], false);
	AddGalleryIcon(ptr, 7, SaveInfo->galleryFlags.galleryFlags[7], SaveInfo->galleryFlags.galleryFlags[7], SaveInfo->galleryFlags.galleryFlags[7], false);
	AddGalleryIcon(ptr, 8, SaveInfo->galleryFlags.galleryFlags[8], SaveInfo->galleryFlags.galleryFlags[8], SaveInfo->galleryFlags.galleryFlags[8], false);
	AddGalleryIcon(ptr, 9, SaveInfo->galleryFlags.galleryFlags[9], SaveInfo->galleryFlags.galleryFlags[9], SaveInfo->galleryFlags.galleryFlags[9], false);
//	ADD_GALLERY_ICON(0, One, true);
//	ADD_GALLERY_ICON(1, Two, false);
//	ADD_GALLERY_ICON(2, Three, false);
//	ADD_GALLERY_ICON(3, Four, false);
//	ADD_GALLERY_ICON(4, Five, false);
//	ADD_GALLERY_ICON(5, Six, true);
//	ADD_GALLERY_ICON(6, Seven, false);
//	ADD_GALLERY_ICON(7, Eight, false);
//	ADD_GALLERY_ICON(8, Nine, false);
//	ADD_GALLERY_ICON(9, Ten, false);

	pagePtr->selectItem(pagePtr->getFirstSelectableItem(true));

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
//	ADD_INFOSUBBOOK_PAGE(NOT_AVAILABLE,STR_NOT_AVAILABLE);
	ADD_INFOSUBBOOK_PAGE(GALLERY1LOCKED,STR_EX_GALLERY1_LOCKED);
	ADD_INFOSUBBOOK_PAGE(GALLERY2LOCKED,STR_EX_GALLERY2_LOCKED);
	ADD_INFOSUBBOOK_PAGE(GALLERY3LOCKED,STR_EX_GALLERY3_LOCKED);
	ADD_INFOSUBBOOK_PAGE(GALLERY4LOCKED,STR_EX_GALLERY4_LOCKED);
	ADD_INFOSUBBOOK_PAGE(GALLERY5LOCKED,STR_EX_GALLERY5_LOCKED);
	ADD_INFOSUBBOOK_PAGE(GALLERY6LOCKED,STR_EX_GALLERY6_LOCKED);
	ADD_INFOSUBBOOK_PAGE(GALLERY7LOCKED,STR_EX_GALLERY7_LOCKED);
	ADD_INFOSUBBOOK_PAGE(GALLERY8LOCKED,STR_EX_GALLERY8_LOCKED);
	ADD_INFOSUBBOOK_PAGE(GALLERY9LOCKED,STR_EX_GALLERY9_LOCKED);
	ADD_INFOSUBBOOK_PAGE(GALLERY10LOCKED,STR_EX_GALLERY10_LOCKED);
	ADD_INFOSUBBOOK_PAGE(GALLERY1,STR_EX_GALLERY1);
	ADD_INFOSUBBOOK_PAGE(GALLERY2,STR_EX_GALLERY2);
	ADD_INFOSUBBOOK_PAGE(GALLERY3,STR_EX_GALLERY3);
	ADD_INFOSUBBOOK_PAGE(GALLERY4,STR_EX_GALLERY4);
	ADD_INFOSUBBOOK_PAGE(GALLERY5,STR_EX_GALLERY5);
	ADD_INFOSUBBOOK_PAGE(GALLERY6,STR_EX_GALLERY6);
	ADD_INFOSUBBOOK_PAGE(GALLERY7,STR_EX_GALLERY7);
	ADD_INFOSUBBOOK_PAGE(GALLERY8,STR_EX_GALLERY8);
	ADD_INFOSUBBOOK_PAGE(GALLERY9,STR_EX_GALLERY9);
	ADD_INFOSUBBOOK_PAGE(GALLERY10,STR_EX_GALLERY10);	
	
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

	ptr->subState = EX_GALLERYS;

	ptr->galleryBook=new PAPERBOOK("GALLERY PAPERBOOK");
	ptr->galleryBook->setCloseCallback(galleryBookCloseCallback);

	// PP: specify the button(s) that can be used to close the paper book
	ptr->galleryBook->setCloseButton(PBCLOSEBUTTON_BACK);// PP: back, because we're *returning* to the front end

	// NH: Set the book up as a sketch pad
	ptr->galleryBook->setOrientation(PBO_LANDSCAPE);		// PP: changed 'type' to 'orientation' because I was getting really confused with the old type enum
	ptr->galleryBook->paperBookFlags |= PBFLAG_ICONONLY;	// PP: this book will only hold pictures
	ptr->galleryBook->setBackground("gallery.bmp");
		
/*	AddGalleryChapter(ptr, "GALLERY 1", 0);
	AddGalleryChapter(ptr, "GALLERY 2", 1);
	AddGalleryChapter(ptr, "GALLERY 3", 2);
	AddGalleryChapter(ptr, "GALLERY 4", 3);
	AddGalleryChapter(ptr, "GALLERY 5", 4);
	AddGalleryChapter(ptr, "GALLERY 6", 5);
	AddGalleryChapter(ptr, "GALLERY 7", 6);
	AddGalleryChapter(ptr, "GALLERY 8", 7);
	AddGalleryChapter(ptr, "GALLERY 9", 8);
	AddGalleryChapter(ptr, "GALLERY 10", 9);
*/

	AddGalleryChapter(ptr, "GALLERY", 0);

	ptr->textBook=new BOOK("CONTROL INFO BOOK");
	pagePtr = ptr->textBook->addPage("Text Book Page");

	// NH: Add Title Book
	pagePtr->insertItem(ptr->titleSubBook=new BOOK("GALLERY TITLE SUB BOOK"));
	ptr->titleSubBook->setYAlign(PIFLAG_YALIGN_TOP)
		->setSelectable(false);

	// NH: Add Title Text (Using Macro)
#define ADD_GALLERYTITLESUBBOOK_PAGE(name, titleText) \
	subPage=ptr->titleSubBook->addPage(#name); \
	subPage->insertItem(titleText) \
		->setStyle(TS_title) \
		->setYAlign(PIFLAG_YALIGN_TOP) \
		->setSelectable(false);

	// NH: Add all info pages (using the macro).  Seperate pages allow for fading between the actions
	ADD_GALLERYTITLESUBBOOK_PAGE(gallery_1,STR_EX_GALLERY1);
	ADD_GALLERYTITLESUBBOOK_PAGE(gallery_2,STR_EX_GALLERY2);
	ADD_GALLERYTITLESUBBOOK_PAGE(gallery_3,STR_EX_GALLERY3);
	ADD_GALLERYTITLESUBBOOK_PAGE(gallery_4,STR_EX_GALLERY4);
	ADD_GALLERYTITLESUBBOOK_PAGE(gallery_5,STR_EX_GALLERY5);
	ADD_GALLERYTITLESUBBOOK_PAGE(gallery_6,STR_EX_GALLERY6);
	ADD_GALLERYTITLESUBBOOK_PAGE(gallery_7,STR_EX_GALLERY7);
	ADD_GALLERYTITLESUBBOOK_PAGE(gallery_8,STR_EX_GALLERY8);
	ADD_GALLERYTITLESUBBOOK_PAGE(gallery_9,STR_EX_GALLERY9);
	ADD_GALLERYTITLESUBBOOK_PAGE(gallery_10,STR_EX_GALLERY10);	

	// NH: INFO
	// NH: Add Info Text Sub Book
	pagePtr->insertItem(ptr->galleryInfoSubBook=new BOOK("GALLERY INFO SUB BOOK"));
	ptr->galleryInfoSubBook->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setSelectable(false);

	// NH: Add Info Text (Using Macro)
#define ADD_GALLERYINFOSUBBOOK_PAGE(name, infoText) \
	subPage=ptr->galleryInfoSubBook->addPage(#name); \
	subPage->insertItem(infoText) \
		->setStyle(TS_control) \
		->setYAlign(PIFLAG_YALIGN_BOTTOM) \
		->setAlignScale(1.2f) \
		->setSelectable(false);

	// NH: Add all info pages (using the macro).  Seperate pages allow for fading between the actions
	ADD_GALLERYINFOSUBBOOK_PAGE(INFO,STR_PRESS_Y_TO_GO_BACK);
	
	// NH: CONTROL
	// NH: Add Control Text Sub Book
	pagePtr->insertItem(ptr->galleryControlSubBook=new BOOK("GALLERY CONTROL SUB BOOK"));
	ptr->galleryControlSubBook->setYPadding(false)
		->setSelectable(false);
/*
	// NH: Add Control Text (Using Macro)
#define ADD_GALLERYCONTROLSUBBOOK_PAGE(name, controlText) \
	subPage=ptr->galleryControlSubBook->addPage(#name); \
	subPage->insertItem(controlText) \
		->setStyle(TS_control) \
		->setYPadding(false) \
		->setSelectable(false);

	// NH: Add all control pages (using the macro).  Seperate pages allow for fading between the actions
	ADD_GALLERYCONTROLSUBBOOK_PAGE(CONTROL_DOWN_ONLY,STR_BROWSE_PAGES_DOWN_ONLY);
	ADD_GALLERYCONTROLSUBBOOK_PAGE(CONTROL_ALL,STR_BROWSEGALLERY);
	ADD_GALLERYCONTROLSUBBOOK_PAGE(CONTROL_UP_ONLY,STR_BROWSE_PAGES_UP_ONLY);
*/
	subPage=ptr->galleryControlSubBook->addPage("CONTROL")
		->setSlack(false);
	subPage->insertItem(backGlyph = new TEXTBOX(STR_BROWSE_PAGES_UP_ONLY));
	backGlyph->setStyle(TS_control)
		->setYPadding(false)
		->setSelectable(false);

	subPage->insertItem(STR_BROWSE_PAGES_TEXT_ONLY, false)
		->setStyle(TS_control)
		->setYPadding(false)
		->setAlignScale(1.07f)
		->setSelectable(false);

	subPage->insertItem(forwardGlyph = new TEXTBOX(STR_BROWSE_PAGES_DOWN_ONLY), false);
	forwardGlyph->setStyle(TS_control)
		->setYPadding(false)
		->setSelectable(false);

	ptr->galleryInfoSubBook->open();// PP: books default to closed
	ptr->galleryControlSubBook->open();

	menu->galleryPage->galleryLastLoaded = 11;
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateGalleryMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateGalleryMenuPage(FRONTENDMENU *menu, PADCONTROLSTATUS *controller)
{
	GALLERYPAGE			*ptr = menu->galleryPage;
	static TBTexture	*overlayTexture[4];
	static int			currentPageOfGallery = 0;
	bool				noneAvailable;
	int					i;
	PAGE				*pagePtr = menu->frontEndMenuBook->getCurrentPage();

	char				imageString[50];

// PP: REMOUT: Now handled by the PaperBook system itself			ptr->galleryBook->privateUpdate();
	// PP: REMOUT: Now handled by the Book List			ptr->textBook->update();

	bdSetIdentityObjectMatrix();
	
	SAVESTATES;

	// NH: Draw pad
// PP: REMOUT: Now handled by the PaperBook system itself		ptr->galleryBook->privateDraw();// PP: PAPERBOOK
	//ptr->galleryBook->drawDebug();

/* PP: REMOUT: Now handled by the Book List		SETPROJECTION(2D);
	SETZTEST(ALWAYS);
	
	// NH: Draw Nav stuff
	ptr->textBook->draw();

	SETPROJECTION(3D);
	RESTORESTATES;
*/

	if (CheatActive(CHEAT_OPEN_GALLERY))
	{
		menu->galleryPage->galleryIcon[0]->setSelectable(true)->setTexture("galleryunlocked.bmp");
		menu->galleryPage->galleryIcon[1]->setSelectable(true)->setTexture("galleryunlocked.bmp");
		menu->galleryPage->galleryIcon[2]->setSelectable(true)->setTexture("galleryunlocked.bmp");
		menu->galleryPage->galleryIcon[3]->setSelectable(true)->setTexture("galleryunlocked.bmp");
		menu->galleryPage->galleryIcon[4]->setSelectable(true)->setTexture("galleryunlocked.bmp");
		menu->galleryPage->galleryIcon[5]->setSelectable(true)->setTexture("galleryunlocked.bmp");
		menu->galleryPage->galleryIcon[6]->setSelectable(true)->setTexture("galleryunlocked.bmp");
		menu->galleryPage->galleryIcon[7]->setSelectable(true)->setTexture("galleryunlocked.bmp");
		menu->galleryPage->galleryIcon[8]->setSelectable(true)->setTexture("galleryunlocked.bmp");
		menu->galleryPage->galleryIcon[9]->setSelectable(true)->setTexture("galleryunlocked.bmp");

		noneAvailable = false;
	}
	else
	{
		noneAvailable = true;

		// NH: Add Gallery Icons
		for(i = 9;i >= 0;--i)
		{
			if ((gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlags[i]) || (CheatActive(CHEAT_OPEN_GALLERY)))
			{
				ptr->galleryIcon[i]->setSelectable(true)->setTexture("galleryunlocked.bmp");
				noneAvailable = false;
			}
			else 
				ptr->galleryIcon[i]->setSelectable(true)->setTexture("gallerylocked.bmp");
		}
	}

/*		if ((gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagOne) || (CheatActive(CHEAT_OPEN_GALLERY)))
		{
			ptr->galleryIcon[0]->setSelectable(true)->setTexture("galleryunlocked.bmp");
			noneAvailable = false;
		}
		else 
			ptr->galleryIcon[0]->setSelectable(true)->setTexture("gallerylocked.bmp");
		
		if ((gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagTwo) || (CheatActive(CHEAT_OPEN_GALLERY)))
		{
			ptr->galleryIcon[1]->setSelectable(true)->setTexture("galleryunlocked.bmp");
			noneAvailable = false;
		}
		else 
			ptr->galleryIcon[1]->setSelectable(true)->setTexture("gallerylocked.bmp");
		
		if ((gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagThree) || (CheatActive(CHEAT_OPEN_GALLERY)))
		{
			ptr->galleryIcon[2]->setSelectable(true)->setTexture("galleryunlocked.bmp");
			noneAvailable = false;
		}
		else 
			ptr->galleryIcon[2]->setSelectable(true)->setTexture("gallerylocked.bmp");
		
		if ((gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagFour) || (CheatActive(CHEAT_OPEN_GALLERY)))
		{
			ptr->galleryIcon[3]->setSelectable(true)->setTexture("galleryunlocked.bmp");
			noneAvailable = false;
		}
		else 
			ptr->galleryIcon[3]->setSelectable(true)->setTexture("gallerylocked.bmp");
		
		if ((gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagFive) || (CheatActive(CHEAT_OPEN_GALLERY)))
		{
			ptr->galleryIcon[4]->setSelectable(true)->setTexture("galleryunlocked.bmp");
			noneAvailable = false;
		}
		else 
			ptr->galleryIcon[4]->setSelectable(true)->setTexture("gallerylocked.bmp");
		
		if ((gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagSix) || (CheatActive(CHEAT_OPEN_GALLERY)))
		{
			ptr->galleryIcon[5]->setSelectable(true)->setTexture("galleryunlocked.bmp");
			noneAvailable = false;
		}
		else 
			ptr->galleryIcon[5]->setSelectable(true)->setTexture("gallerylocked.bmp");
		
		if ((gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagSeven) || (CheatActive(CHEAT_OPEN_GALLERY)))
		{
			ptr->galleryIcon[6]->setSelectable(true)->setTexture("galleryunlocked.bmp");
			noneAvailable = false;
		}
		else 
			ptr->galleryIcon[6]->setSelectable(true)->setTexture("gallerylocked.bmp");
		
		if ((gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagEight) || (CheatActive(CHEAT_OPEN_GALLERY)))
		{
			ptr->galleryIcon[7]->setSelectable(true)->setTexture("galleryunlocked.bmp");
			noneAvailable = false;
		}
		else 
			ptr->galleryIcon[7]->setSelectable(true)->setTexture("gallerylocked.bmp");
		
		if ((gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagNine) || (CheatActive(CHEAT_OPEN_GALLERY)))
		{
			ptr->galleryIcon[8]->setSelectable(true)->setTexture("galleryunlocked.bmp");
			noneAvailable = false;
		}
		else 
			ptr->galleryIcon[8]->setSelectable(true)->setTexture("gallerylocked.bmp");
		
		if ((gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagTen)
			|| (CheatActive(CHEAT_OPEN_GALLERY)))
		{
			ptr->galleryIcon[9]->setSelectable(true)->setTexture("galleryunlocked.bmp");
			noneAvailable = false;
		}
		else 
			ptr->galleryIcon[9]->setSelectable(true)->setTexture("gallerylocked.bmp");	
	}*/

	if (menu->galleryPage->subState == EX_GALLERYS)
	{
		for(i = 9;i >= 0;--i)
		{
			if (ptr->page->selectedItem == ptr->galleryIcon[i])
			{
				if ((gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlags[i]) || (CheatActive(CHEAT_OPEN_GALLERY)))
				{
					if (OKBUTTON)
					{
						galleryNo = i;
						menu->frontEndMenuBook->gotoPage(menuNames[MENU_BLANK]);
						menu->galleryPage->galleryLoadState = EG_LOAD;
						LoadWholePackageBkg(galleyName[i]);
					}
					switch(i)
					{
					case 0:
						ptr->infoSubBook->gotoPage("GALLERY1");
						break;
					case 1:
						ptr->infoSubBook->gotoPage("GALLERY2");
						break;
					case 2:
						ptr->infoSubBook->gotoPage("GALLERY3");
						break;
					case 3:
						ptr->infoSubBook->gotoPage("GALLERY4");
						break;
					case 4:
						ptr->infoSubBook->gotoPage("GALLERY5");
						break;
					case 5:
						ptr->infoSubBook->gotoPage("GALLERY6");
						break;
					case 6:
						ptr->infoSubBook->gotoPage("GALLERY7");
						break;
					case 7:
						ptr->infoSubBook->gotoPage("GALLERY8");
						break;
					case 8:
						ptr->infoSubBook->gotoPage("GALLERY9");
						break;
					case 9:
						ptr->infoSubBook->gotoPage("GALLERY10");
						break;
					}
				}
				else
				{
					switch(i)
					{
					case 0:
						ptr->infoSubBook->gotoPage("GALLERY1LOCKED");
						break;
					case 1:
						ptr->infoSubBook->gotoPage("GALLERY2LOCKED");
						break;
					case 2:
						ptr->infoSubBook->gotoPage("GALLERY3LOCKED");
						break;
					case 3:
						ptr->infoSubBook->gotoPage("GALLERY4LOCKED");
						break;
					case 4:
						ptr->infoSubBook->gotoPage("GALLERY5LOCKED");
						break;
					case 5:
						ptr->infoSubBook->gotoPage("GALLERY6LOCKED");
						break;
					case 6:
						ptr->infoSubBook->gotoPage("GALLERY7LOCKED");
						break;
					case 7:
						ptr->infoSubBook->gotoPage("GALLERY8LOCKED");
						break;
					case 8:
						ptr->infoSubBook->gotoPage("GALLERY9LOCKED");
						break;
					case 9:
						ptr->infoSubBook->gotoPage("GALLERY10LOCKED");
						break;
					}
				}			
			}
		}

	/*	if (ptr->page->selectedItem == ptr->galleryIcon[0])
		{
			if ((gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagOne) || (CheatActive(CHEAT_OPEN_GALLERY)))
			{
				if (OKBUTTON)
				{
					galleryNo = 0;
					menu->frontEndMenuBook->gotoPage(menuNames[MENU_BLANK]);
					menu->galleryPage->galleryLoadState = EG_LOAD;
					LoadWholePackageBkg(galleyName[galleryNo]);
				}									
				ptr->infoSubBook->gotoPage("GALLERY1");
			}
			else
			{
				ptr->infoSubBook->gotoPage("GALLERY1LOCKED");
			}			
		}
		else if (ptr->page->selectedItem == ptr->galleryIcon[1])
		{
			if ((gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagTwo)
				|| (CheatActive(CHEAT_OPEN_GALLERY)))
			{
				if (OKBUTTON)
				{
					galleryNo = 1;
					menu->frontEndMenuBook->gotoPage(menuNames[MENU_BLANK]);
					menu->galleryPage->galleryLoadState = EG_LOAD;
					LoadWholePackageBkg(galleyName[galleryNo]);
				}		
				ptr->infoSubBook->gotoPage("GALLERY2");
			}
			else
			{
				ptr->infoSubBook->gotoPage("GALLERY2LOCKED");
			}
		}
		else if (ptr->page->selectedItem == ptr->galleryIcon[2])
		{
			if ((gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagThree)
				|| (CheatActive(CHEAT_OPEN_GALLERY)))
			{
				if (OKBUTTON)
				{
					galleryNo = 2;
					menu->frontEndMenuBook->gotoPage(menuNames[MENU_BLANK]);
					menu->galleryPage->galleryLoadState = EG_LOAD;
					LoadWholePackageBkg(galleyName[galleryNo]);
				}		
				ptr->infoSubBook->gotoPage("GALLERY3");
			}
			else
			{
				ptr->infoSubBook->gotoPage("GALLERY3LOCKED");
			}
		}
		else if (ptr->page->selectedItem == ptr->galleryIcon[3])
		{
			if ((gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagFour)
				|| (CheatActive(CHEAT_OPEN_GALLERY)))
			{
				if (OKBUTTON)
				{
					galleryNo = 3;
					menu->frontEndMenuBook->gotoPage(menuNames[MENU_BLANK]);
					menu->galleryPage->galleryLoadState = EG_LOAD;
					LoadWholePackageBkg(galleyName[galleryNo]);
				}		
				ptr->infoSubBook->gotoPage("GALLERY4");
			}
			else
			{
				ptr->infoSubBook->gotoPage("GALLERY4LOCKED");
			}
		}
		else if (ptr->page->selectedItem == ptr->galleryIcon[4])
		{
			if ((gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagFive)
				|| (CheatActive(CHEAT_OPEN_GALLERY)))
			{
				if (OKBUTTON)
				{
					galleryNo = 4;
					menu->frontEndMenuBook->gotoPage(menuNames[MENU_BLANK]);
					menu->galleryPage->galleryLoadState = EG_LOAD;
					LoadWholePackageBkg(galleyName[galleryNo]);
				}		
				ptr->infoSubBook->gotoPage("GALLERY5");
			}
			else
			{
				ptr->infoSubBook->gotoPage("GALLERY5LOCKED");
			}
		}
		else if (ptr->page->selectedItem == ptr->galleryIcon[5])
		{	
			if ((gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagSix)
				|| (CheatActive(CHEAT_OPEN_GALLERY)))
			{
				if (OKBUTTON)
				{
					galleryNo = 5;
					menu->frontEndMenuBook->gotoPage(menuNames[MENU_BLANK]);
					menu->galleryPage->galleryLoadState = EG_LOAD;
					LoadWholePackageBkg(galleyName[galleryNo]);
				}
				ptr->infoSubBook->gotoPage("GALLERY6");
			}
			else
			{
				ptr->infoSubBook->gotoPage("GALLERY6LOCKED");
			}
		}
		else if (ptr->page->selectedItem == ptr->galleryIcon[6])
		{
			if ((gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagSeven)
				|| (CheatActive(CHEAT_OPEN_GALLERY)))
			{
				if (OKBUTTON)
				{
					galleryNo = 6;
					menu->frontEndMenuBook->gotoPage(menuNames[MENU_BLANK]);
					menu->galleryPage->galleryLoadState = EG_LOAD;
					LoadWholePackageBkg(galleyName[galleryNo]);
				}						
				ptr->infoSubBook->gotoPage("GALLERY7");
			}
			else
			{
				ptr->infoSubBook->gotoPage("GALLERY7LOCKED");
			}
		}
		else if (ptr->page->selectedItem == ptr->galleryIcon[7])
		{
			if ((gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagEight)
				|| (CheatActive(CHEAT_OPEN_GALLERY)))
			{
				if (OKBUTTON)
				{
					galleryNo = 7;
					menu->frontEndMenuBook->gotoPage(menuNames[MENU_BLANK]);
					menu->galleryPage->galleryLoadState = EG_LOAD;
					LoadWholePackageBkg(galleyName[galleryNo]);
				}		
				ptr->infoSubBook->gotoPage("GALLERY8");
			}
			else
			{
				ptr->infoSubBook->gotoPage("GALLERY8LOCKED");
			}
		}
		else if (ptr->page->selectedItem == ptr->galleryIcon[8])
		{
			if ((gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagNine)
				|| (CheatActive(CHEAT_OPEN_GALLERY)))
			{
				if (OKBUTTON)
				{
					galleryNo = 8;
					menu->frontEndMenuBook->gotoPage(menuNames[MENU_BLANK]);
					menu->galleryPage->galleryLoadState = EG_LOAD;
					LoadWholePackageBkg(galleyName[galleryNo]);
				}		
				ptr->infoSubBook->gotoPage("GALLERY9");
			}
			else
			{
				ptr->infoSubBook->gotoPage("GALLERY9LOCKED");
			}
		}
		else if (ptr->page->selectedItem == ptr->galleryIcon[9])
		{
			if ((gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagTen)
				|| (CheatActive(CHEAT_OPEN_GALLERY)))
			{
				if (OKBUTTON)
				{
					galleryNo = 9;
					menu->frontEndMenuBook->gotoPage(menuNames[MENU_BLANK]);
					menu->galleryPage->galleryLoadState = EG_LOAD;
					LoadWholePackageBkg(galleyName[galleryNo]);
				}						
					ptr->infoSubBook->gotoPage("GALLERY10");
			}
			else
			{
				ptr->infoSubBook->gotoPage("GALLERY10LOCKED");
			}
		}*/
	}
	
	if (menu->galleryPage->galleryLoadState == EG_LOAD)
	{
		if (GetCurrentBkgLoadingStatus()==FALSE)
		{
			int i,k;

			// NH: menu->galleryPage->galleryLastLoaded == 11 means none opened
			if (menu->galleryPage->galleryLastLoaded != galleryNo && menu->galleryPage->galleryLastLoaded != 11)
				FreeWholePackage(galleyName[menu->galleryPage->galleryLastLoaded]);

			menu->galleryPage->galleryLoadState = EG_NONE;
			menu->galleryPage->galleryLastLoaded = galleryNo;
						
			for(i=0; i<10; i++)
			{
				sprintf(imageString, "%s.bmp", &galleryImages[galleryNo][i][0]);
				ptr->galleryImage[i]->setTexture(imageString);
			}
			
			ptr->galleryBook->gotoPageNumber(0,1);
			ptr->galleryBook->open();
			ptr->textBook->open();
			ptr->titleSubBook->open();

			menu->galleryPage->subState = EX_WORKTOP;
			ptr->titleSubBook->gotoPage(galleyName[galleryNo]);
//			menu->frontEndMenuBook->gotoPage(menuNames[MENU_BLANK]);
		}
	}

	//triangle to reset back to main menu
	if(CANCELBUTTON && menu->galleryPage->galleryLoadState == EG_NONE)
	{
		// NH: Play bowhomb sound. Would be nice to be in PageFX, but not sure how to do it so this will work for now
		menu->galleryPage->page->playNavSound(menu->galleryPage->page->menuSounds, EMSFX_BACK);
		ptr->textBook->close();
		ptr->titleSubBook->close();

	// NH: menu->galleryPage->galleryLastLoaded == 11 means none opened
		if (menu->galleryPage->galleryLastLoaded != 11)
			FreeWholePackage(galleyName[menu->galleryPage->galleryLastLoaded]);

		if (menu->galleryPage->subState == EX_GALLERYS)
		{
			SetFrontEndCameraDestination(FECAM_EXTRAS_GALLERY,FECAM_EXTRAS);
			menu->frontEndMenuBook->gotoPage(menuNames[MENU_EXTRAS]);
			menu->currentPage = MENU_EXTRAS;
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: galleryBookCloseCallback
	Purpose 	: receives notification that the gallery paper-book has closed
	Parameters 	: ptr to paperbook that has closed
	Returns 	: 
	Info 		: // PP: must match the PaperBookCloseCallback typedef (PaperBook.h)
*/
void galleryBookCloseCallback(PAPERBOOK* const paperBook)
{
	// TP: needed to reset these otherwise if you exit the book on the last page and then re-enter both arrows are disabled
	if (forwardGlyph) forwardGlyph->enable(true);
	if (backGlyph) backGlyph->enable(true);
	frontEndMenu.galleryPage->textBook->close();
	frontEndMenu.galleryPage->titleSubBook->close();
		
	frontEndMenu.frontEndMenuBook->gotoPage(menuNames[MENU_GALLERY]);
	frontEndMenu.galleryPage->subState = EX_GALLERYS;

	// NH: return to in-game state
	SetGameState(&gameStatus, GAMESTATE_INGAME, GAMESUBSTATE_NORMAL);
}


/*	--------------------------------------------------------------------------------
	Function 	: galleryFirstPageCallback
	Purpose 	: receives notification that the gallery paper-book has turned/opened on the first page
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void galleryFirstPageCallback(void)
{
	forwardGlyph->enable(false);
}

/*	--------------------------------------------------------------------------------
	Function 	: galleryCentralPageCallback
	Purpose 	: receives notification that the gallery paper-book has turned/opened on a central page
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void galleryCentralPageCallback(void)
{
	backGlyph->enable(true);
	forwardGlyph->enable(true);
}

/*	--------------------------------------------------------------------------------
	Function 	: galleryLastPageCallback
	Purpose 	: receives notification that the gallery paper-book has turned/opened on the last page
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void galleryLastPageCallback(void)
{
	backGlyph->enable(false);
}