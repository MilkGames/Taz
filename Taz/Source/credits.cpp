// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : credits.cpp
//   Purpose : functions to create and control the credits
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "TextFX.h"
#include "control.h"
#include "frontendmain.h"

static int	creditsSections[NUMBER_OF_SECTIONS];
int			creditsPosition;
float		creditChangeTimer;
int			creditsPageNumber;
int			lastBeforeLegal;

#define LAST_SECTION_BEFORE_LEGAL_US 6
#define LAST_SECTION_BEFORE_LEGAL 4

/*	--------------------------------------------------------------------------------
	Function 	: InsertCredit
	Purpose 	: Inserts a credit line to the specified credits page
	Parameters 	: (PAGE *) creditsPage - the page to add credits too, (EStringIndex) stringIndex - the string to add, (int) type - the type of credit to add
	Returns 	: 
	Info 		: 
*/
void InsertCredit(PAGE *creditPage, EStringIndex stringIndex, ECreditType type, float scale)
{
	TEXTBOX		*temp;

	switch(type)
	{
	case CREDITS_JOB_TITLE:
		creditsSections[creditsPosition]++;
		temp = new TEXTBOX(stringIndex);
		temp->setFontSize(scale)->setStyle(TS_creditsTitle)->setYAlign(PIFLAG_YALIGN_BOTTOM)->setSelectable(false);
		creditPage->insertItem(temp);
		break;
	case CREDITS_NAME:
		temp = new TEXTBOX(stringIndex);
		temp->setFontSize(scale)->setYPadding(FALSE)->setSelectable(false);
		creditPage->insertItem(temp);
		break;
	case CREDITS_BOTTOM_NAME:
		temp = new TEXTBOX(stringIndex);
		temp->setFontSize(scale)->setYAlign(PIFLAG_YALIGN_TOP)->setSelectable(false);
		creditPage->insertItem(temp);
		break;
	}	
}

/*	--------------------------------------------------------------------------------
	Function 	: AddCreditsSection
	Purpose 	: Adds a skippable section to the credits (stores position so that it may be skipped to)
	Parameters 	: (bool)first - Indicates the first page 
	Returns 	: successfull - true/false
	Info 		: Could have used a linked list but just for this it seemed a bit like overkill
*/
bool AddCreditsSection(bool first)
{
	if (first)
	{
		creditsPosition = 0;
		creditsSections[creditsPosition] = 0;
		return true;
	}
	else if (creditsPosition < NUMBER_OF_SECTIONS-1)
	{
		creditsPosition++;
		creditsSections[creditsPosition] = creditsSections[creditsPosition-1];
		return true;
	}
	return false;
}

/*	--------------------------------------------------------------------------------
	Function 	: CreateCreditsPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void CreateCreditsPage(FRONTENDMENU *menu)
{
	CREDITSPAGE		*ptr;
	PAGE			*pagePtr, *subPage;
	TEXTBOX			*dummyBox;

	ptr = menu->creditsPage = (CREDITSPAGE*)ZALLOC(sizeof(CREDITSPAGE));
	if(!ptr)
	{
		bkPrintf("*** WARNING ***, could not malloc credits page/n");
		return;
	}

	// NH: NTSC (American) Version Only
	if (videoMode.flags & BDISPLAYFLAG_NTSC)
	{
		lastBeforeLegal = LAST_SECTION_BEFORE_LEGAL_US;
	}
	else
	{	
		lastBeforeLegal = LAST_SECTION_BEFORE_LEGAL;
	}

	// NH: Add Page
	pagePtr = ptr->page = menu->frontEndMenuBook->addPage(menuNames[MENU_CREDITS]);
	pagePtr->setNavigation(true);

	// NH: Add Title
//	pagePtr->insertItem(ptr->titleTb = new TEXTBOX(STR_EX_CREDITS),false);
//	ptr->titleTb->setStyle(TEXTSTYLE_TITLE)
//		->setYAlign(PIFLAG_YALIGN_TOP)
//		->setSelectable(false);

	// NH: Add Credits Title Sub Book
	pagePtr->insertItem(ptr->creditsTitleSubBook=new BOOK("CREDITS TITLE SUB BOOK"));
	ptr->creditsTitleSubBook
		->setYAlign(PIFLAG_YALIGN_TOP)
		->setSelectable(false);

	// NH: Bruno Presents
	subPage=ptr->creditsTitleSubBook->addPage("CREDITS_BRUNO");
	subPage->insertItem(STR_EX_CREDITS)->setStyle(TS_title)->setYAlign(PIFLAG_YALIGN_TOP)->setSelectable(false);

	// NH: NTSC (American) Version Only
	if (videoMode.flags & BDISPLAYFLAG_NTSC)
	{
		// NH: Infogrames Interactive US
		subPage=ptr->creditsTitleSubBook->addPage("US_CREDITS_I_INT");
		subPage->insertItem(STR_EX_CREDITS)->setStyle(TS_title)->setYPadding(FALSE)->setSelectable(false);
		subPage->insertItem(STR_US_CREDITS_I_INTERACTIVE_INC)->setStyle(TS_smallTitle)->setYAlign(PIFLAG_YALIGN_TOP)->setSelectable(false);

		// NH: Infogrames US
		subPage=ptr->creditsTitleSubBook->addPage("US_CREDITS_I");
		subPage->insertItem(STR_EX_CREDITS)->setStyle(TS_title)->setYPadding(FALSE)->setSelectable(false);
		subPage->insertItem(STR_US_CREDITS_I_INC)->setStyle(TS_smallTitle)->setYAlign(PIFLAG_YALIGN_TOP)->setSelectable(false);
	}

	// NH: Blitz
	subPage=ptr->creditsTitleSubBook->addPage("CREDITS_BLITZ");
	subPage->insertItem(STR_EX_CREDITS)->setStyle(TS_title)->setYPadding(FALSE)->setSelectable(false);
	subPage->insertItem(STR_CREDITS_BLITZ)->setStyle(TS_smallTitle)->setYAlign(PIFLAG_YALIGN_TOP)->setSelectable(false);

	// NH: Infogrames
	subPage=ptr->creditsTitleSubBook->addPage("CREDIT_I");
	subPage->insertItem(STR_EX_CREDITS)->setStyle(TS_title)->setYPadding(FALSE)->setSelectable(false);
	subPage->insertItem(STR_CREDIT_I)->setStyle(TS_smallTitle)->setYAlign(PIFLAG_YALIGN_TOP)->setSelectable(false);

	// NH: WB
	subPage=ptr->creditsTitleSubBook->addPage("CREDITS_WB");
	subPage->insertItem(STR_EX_CREDITS)->setStyle(TS_title)->setYPadding(FALSE)->setSelectable(false);
	subPage->insertItem(STR_CREDITS_WB)->setStyle(TS_smallTitle)->setYAlign(PIFLAG_YALIGN_TOP)->setSelectable(false);

	// NH: Nimrod
	subPage=ptr->creditsTitleSubBook->addPage("CREDITS_NIMROD");
	subPage->insertItem(STR_EX_CREDITS)->setStyle(TS_title)->setYPadding(FALSE)->setSelectable(false);
	subPage->insertItem(STR_CREDITS_NAMES_NIMROD)->setStyle(TS_smallTitle)->setYAlign(PIFLAG_YALIGN_TOP)->setSelectable(false);

	// NH: Copyright
	subPage=ptr->creditsTitleSubBook->addPage("CREDITS_WB");
	subPage->insertItem(STR_EX_CREDITS)->setStyle(TS_title)->setYAlign(PIFLAG_YALIGN_TOP)->setSelectable(false);

	// NH: CREDITS
	// NH: Add Credits Text Sub Book
	pagePtr->insertItem(ptr->creditsSubBook=new BOOK("CREDITS SUB BOOK"));
	ptr->creditsSubBook->setYPadding(false)//Align(PIFLAG_YALIGN_TOP)
		->setSelectable(false);

	// NH: Intro
	CreateIntroCredits(ptr->creditsSubBook);
	
	// NH: NTSC (American) Version Only
	if (videoMode.flags & BDISPLAYFLAG_NTSC)
	{
		// NH: US Infogrames Int
		CreateUSInfogramesIntCredits(ptr->creditsSubBook);
		// NH: US Infogrames
		CreateUSInfogramesCredits(ptr->creditsSubBook);
	}

	// NH: Blitz
	CreateBlitzCredits(ptr->creditsSubBook);
	// NH: Infogrames
	CreateInfogramesCredits(ptr->creditsSubBook);
	// NH: Warner Bros
	CreateWarnerCredits(ptr->creditsSubBook);
	// NH: NIMROD
	CreateNimrodCredits(ptr->creditsSubBook);
	// NH: Legal
	CreateLegalCredits(ptr->creditsSubBook);
	
	// NH: CONTROL
	// NH: Add Control Text Sub Book
	pagePtr->insertItem(ptr->controlSubBook=new BOOK("CONTROL SUB BOOK"));
	ptr->controlSubBook->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setSelectable(false);

	// NH: Add Control Text (Using Macro)
#define ADD_CONTROLSUBBOOK_PAGE(name, controlText) \
	subPage=ptr->controlSubBook->addPage(#name); \
	subPage->insertItem(controlText) \
		->setStyle(TS_control) \
		->setYAlign(PIFLAG_YALIGN_BOTTOM) \
		->setSelectable(false);	

	// NH: Add all control pages (using the macro).  Seperate pages allow for fading between the actions
	ADD_CONTROLSUBBOOK_PAGE(RETURN,STR_PRESS_Y_TO_GO_BACK);

	ptr->controlSubBook->open();
	ptr->creditsTitleSubBook->open();
	ptr->creditsSubBook->open();
}

/*	--------------------------------------------------------------------------------
	Function 	: CreateIntroCredits
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void CreateIntroCredits(struct TAG_BOOK* creditsBook, char skipable, float scale)
{
	TEXTBOX			*ptr;
	ICON			*tazWantedLogo;
	PAGE			*subPage;

	// NH: TAZ: WANTED
	if(skipable) AddCreditsSection(true);
	// NH: Special Case
	subPage=creditsBook->addPage("CREDITS_BRUNO");
	creditsSections[creditsPosition]++;
	ptr = new TEXTBOX(STR_CREDITS_BRUNO);
	ptr->setFontSize(scale)->setStyle(TS_creditsTitle)->setYAlign(PIFLAG_YALIGN_BOTTOM)->setSelectable(false);
	subPage->insertItem(ptr);
	ptr = new TEXTBOX(STR_CREDITS_PRESENTS);
	ptr->setFontSize(scale)->setStyle(TS_creditsTitle)->setYAlign(PIFLAG_YALIGN_TOP)->setSelectable(false);
	subPage->insertItem(ptr);
	
	// NH: Special Case
	subPage=creditsBook->addPage("CREDITS_LOGO");
	creditsSections[creditsPosition]++;
	subPage->insertItem(tazWantedLogo = new ICON("tazbiglogo.bmp","Taz Logo"))->setAlignScale(1.3f)->setYAlign(PIFLAG_YALIGN_BOTTOM)->setSelectable(false);
	InsertCredit(subPage, STR_CREDITS_BLITZ, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_TEAM_PHOENIX_PRODUCTION, CREDITS_BOTTOM_NAME, scale);
}

/*	--------------------------------------------------------------------------------
	Function 	: CreateUSInfogramesIntCredits
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 10 Items
*/
void CreateUSInfogramesIntCredits(struct TAG_BOOK* creditsBook, char skipable, float scale)
{
	PAGE			*subPage;

	if(skipable) AddCreditsSection();

	// NH: 
	subPage=creditsBook->addPage("US_CREDITS_I_PRODUCER");
	InsertCredit(subPage, STR_US_CREDITS_I_PRODUCER, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_JCHMURA, CREDITS_BOTTOM_NAME, scale);

	// NH: 
	subPage=creditsBook->addPage("US_CREDITS_I_EXEC_PRODUCER");
	InsertCredit(subPage, STR_US_CREDITS_I_EXEC_PRODUCER, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_BLEVAY, CREDITS_BOTTOM_NAME, scale);
	
	// NH: 
	subPage=creditsBook->addPage("US_CREDITS_I_BRANDMANAGER");
	InsertCredit(subPage, STR_US_CREDITS_I_BRANDMANAGER, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_MMACCONNELL, CREDITS_BOTTOM_NAME, scale);
		
	// NH: 
	subPage=creditsBook->addPage("US_CREDITS_I_DIRQA");
	InsertCredit(subPage, STR_US_CREDITS_I_DIRQA, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_MCRAIGHEAD, CREDITS_BOTTOM_NAME, scale);

	// NH: 
	subPage=creditsBook->addPage("US_CREDITS_I_DIRMARKETING");
	InsertCredit(subPage, STR_US_CREDITS_I_DIRMARKETING, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_SARTHUR, CREDITS_BOTTOM_NAME, scale);

	// NH: 
	subPage=creditsBook->addPage("US_CREDITS_I_DIRCREATIVE");
	InsertCredit(subPage, STR_US_CREDITS_I_DIRCREATIVE, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_SMARTIN, CREDITS_BOTTOM_NAME, scale);

	// NH: 
	subPage=creditsBook->addPage("US_CREDITS_I_DIREDIT");
	InsertCredit(subPage, STR_US_CREDITS_I_DIREDIT, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_EMACKNEY, CREDITS_BOTTOM_NAME, scale);

	// NH: 
	subPage=creditsBook->addPage("US_CREDITS_I_GRAPHIC_DESIGNER");
	InsertCredit(subPage, STR_US_CREDITS_I_GRAPHIC_DESIGNER, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_MTOMAIOLO, CREDITS_BOTTOM_NAME, scale);

	// NH: 
	subPage=creditsBook->addPage("US_CREDITS_I_DOCUMENTATION");
	InsertCredit(subPage, STR_US_CREDITS_I_DOCUMENTATION, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_RKRAVITZ, CREDITS_BOTTOM_NAME, scale);

	// NH: 
	subPage=creditsBook->addPage("US_CREDITS_I_COPYWRITER");
	InsertCredit(subPage, STR_US_CREDITS_I_COPYWRITER, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_PCOLLIN, CREDITS_BOTTOM_NAME, scale);
}

/*	--------------------------------------------------------------------------------
	Function 	: CreateUSInfogramesCredits
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 7 Items (10 on PC)
*/
void CreateUSInfogramesCredits(struct TAG_BOOK* creditsBook, char skipable, float scale)
{
	PAGE			*subPage;

	if(skipable) AddCreditsSection();

	// NH: 
	subPage=creditsBook->addPage("US_CREDITS_I_DIRPSS");
	InsertCredit(subPage, STR_US_CREDITS_I_DIRPSS, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_MGILMARTIN, CREDITS_BOTTOM_NAME, scale);
	
	// NH: 
	subPage=creditsBook->addPage("US_CREDITS_I_STRATEGIC");
	InsertCredit(subPage, STR_US_CREDITS_I_STRATEGIC, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_JSCHNEER, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_DCOSTELLO, CREDITS_BOTTOM_NAME, scale);
	
#if BPLATFORM == PS2
	// NH: 
	subPage=creditsBook->addPage("US_CREDITS_I_QA_SUPERVISOR");
	InsertCredit(subPage, STR_US_CREDITS_I_QA_SUPERVISOR, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_DCLAY, CREDITS_BOTTOM_NAME, scale);

	// NH: 
	subPage=creditsBook->addPage("US_CREDITS_I_PS2LEADTESTER");
	InsertCredit(subPage, STR_US_CREDITS_I_PS2LEADTESTER, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_DSTRANG, CREDITS_BOTTOM_NAME, scale);

	// NH: 
	subPage=creditsBook->addPage("US_CREDITS_I_PS2ASSISTANTLEADTESTER");
	InsertCredit(subPage, STR_US_CREDITS_I_PS2ASSISTANTLEADTESTER, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_ALEMAT, CREDITS_BOTTOM_NAME, scale);
#else // NH: !PS2
	// NH: 
	subPage=creditsBook->addPage("US_CREDITS_I_QA_SUPERVISOR");
	InsertCredit(subPage, STR_US_CREDITS_I_QA_SUPERVISOR, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_CNUNEZ, CREDITS_BOTTOM_NAME, scale);

	// NH: 
	subPage=creditsBook->addPage("US_CREDITS_I_LEADTESTER");
	InsertCredit(subPage, STR_US_CREDITS_I_LEADTESTER, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_RHIGBEE, CREDITS_BOTTOM_NAME, scale);

	// NH: 
	subPage=creditsBook->addPage("US_CREDITS_I_TESTERS");
	InsertCredit(subPage, STR_US_CREDITS_I_TESTERS, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_HHINCHCLIFFE, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_JFRIED, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_SGASPAR, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_RMACRAE, CREDITS_BOTTOM_NAME, scale);
#endif
	// NH: 
	subPage=creditsBook->addPage("US_CREDITS_I_SENIORMANAGER");
	InsertCredit(subPage, STR_US_CREDITS_I_SENIORMANAGER, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_LRIVAS, CREDITS_BOTTOM_NAME, scale);
	
	// NH: 
	subPage=creditsBook->addPage("US_CREDITS_I_ENGINEERING");
	InsertCredit(subPage, STR_US_CREDITS_I_ENGINEERING, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_KEDWARDS, CREDITS_BOTTOM_NAME, scale);
	
#if BPLATFORM == PC
	// NH: 
	subPage=creditsBook->addPage("US_CREDITS_I_COMPATIBILITY");
	InsertCredit(subPage, STR_US_CREDITS_I_COMPATIBILITY, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_DSTRANG, CREDITS_BOTTOM_NAME, scale);
	
	// NH: 
	subPage=creditsBook->addPage("US_CREDITS_I_COMPATIBILITY_LEAD");
	InsertCredit(subPage, STR_US_CREDITS_I_COMPATIBILITY_LEAD, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_JCORDERO, CREDITS_BOTTOM_NAME, scale);
	
	// NH: 
	subPage=creditsBook->addPage("US_CREDITS_I_COMPATIBILITY_ANALYSTS");
	InsertCredit(subPage, STR_US_CREDITS_I_COMPATIBILITY_ANALYSTS, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_RBUCHHOLZ, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_BMCQUINN, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_EORNSTEAD, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_US_CREDITS_I_GSMITH, CREDITS_BOTTOM_NAME, scale);
#endif // NH: #if BPLATFORM == PC
}

/*	--------------------------------------------------------------------------------
	Function 	: CreateBlitzCredits
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void CreateBlitzCredits(struct TAG_BOOK* creditsBook, char skipable, float scale)
{
	TEXTBOX			*ptr;
	PAGE			*subPage;

	if(skipable) AddCreditsSection();
	// NH: Production Manager
	subPage=creditsBook->addPage("CREDITS_PROD_MAN");
	InsertCredit(subPage, STR_CREDITS_PROD_MAN, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_JCARTWRIGHT, CREDITS_BOTTOM_NAME, scale);
	
	// NH: Technical Manager
	subPage=creditsBook->addPage("CREDITS_TECH_MAN");
	InsertCredit(subPage, STR_CREDITS_TECH_MAN, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_TPOVEY, CREDITS_BOTTOM_NAME, scale);
	
	// NH: Creative Manager
	subPage=creditsBook->addPage("CREDITS_CREATE_MAN");
	InsertCredit(subPage, STR_CREDITS_CREATE_MAN, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_IPESTRIDGE, CREDITS_BOTTOM_NAME, scale);
	
	// NH: Design Coordinator
	subPage=creditsBook->addPage("CREDITS_DESIGN_COORD");
	InsertCredit(subPage, STR_CREDITS_DESIGN_COORD, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_SWEST, CREDITS_BOTTOM_NAME, scale);
	
	// NH: Programmers
	subPage=creditsBook->addPage("CREDITS_PROG");
	InsertCredit(subPage, STR_CREDITS_PROG, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_CDAREAU, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_PPALMER, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_NHIGGS, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_JWEEKS, CREDITS_BOTTOM_NAME, scale);
	
	// NH: Character Animation
	subPage=creditsBook->addPage("CREDITS_CHAR_ANIM");
	InsertCredit(subPage, STR_CREDITS_CHAR_ANIM, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_EQUIGLEY, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_NPETTITT, CREDITS_BOTTOM_NAME, scale);
			
	// NH: Texture Artists
	subPage=creditsBook->addPage("CREDITS_TEXTURE_ART");
	InsertCredit(subPage, STR_CREDITS_TEXTURE_ART, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_CSOUTHALL, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_MBUCKINGHAM, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_DWEBB, CREDITS_BOTTOM_NAME, scale);
	
	// NH: 3D Modelers
	subPage=creditsBook->addPage("CREDITS_3D_MODEL");
	InsertCredit(subPage, STR_CREDITS_3D_MODEL, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_TADAMS, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_AHODGSON, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_DNOURISH, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_FTEDESCHI, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_CSANDELL, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_JCHILDS, CREDITS_BOTTOM_NAME, scale);
		
	// NH: Additional Programming
	subPage=creditsBook->addPage("CREDITS_ADD_PROGR");
	InsertCredit(subPage, STR_CREDITS_ADD_PROGR, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_SORCHARD, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_CWILSON, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_DSERAFIM, CREDITS_BOTTOM_NAME, scale);
	
	// NH: Additional Modeling
	subPage=creditsBook->addPage("CREDITS_ADD_MODEL");
	InsertCredit(subPage, STR_CREDITS_ADD_MODEL, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_DFLYNN, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_DWHITEHEAD, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_DPINKNEY, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_GAMEWORLD7, CREDITS_BOTTOM_NAME, scale);
	
	// NH: Additional Design
	subPage=creditsBook->addPage("CREDITS_ADD_DESIGN");
	InsertCredit(subPage, STR_CREDITS_ADD_DESIGN, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_SSMITH, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_DNIMMO, CREDITS_BOTTOM_NAME, scale);
	
	// NH: Associate Project anager
	subPage=creditsBook->addPage("CREDITS_ASSOC_PROJ_MAN");
	InsertCredit(subPage, STR_CREDITS_ASSOC_PROJ_MAN, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_CVIGGERS, CREDITS_BOTTOM_NAME, scale);
	
	// NH: Tools & Technology
	subPage=creditsBook->addPage("CREDITS_TOOLS_AND_TECH");
	InsertCredit(subPage, STR_CREDITS_TOOLS_AND_TECH, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_JWHIGHAM, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_ASIDWELL, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_ASLATER, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_TEWHITTOCK, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_LHOMEWOOD, CREDITS_BOTTOM_NAME, scale);
		
	// NH: Sound Engineer
	subPage=creditsBook->addPage("CREDITS_SOUND_ENGINE");
	InsertCredit(subPage, STR_CREDITS_SOUND_ENGINE, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_AMORRIS, CREDITS_BOTTOM_NAME, scale);
	
	// NH: QA
	subPage=creditsBook->addPage("CREDITS_QA");
	InsertCredit(subPage, STR_CREDITS_QA, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_JTAINSH, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_BFIELD, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_TCHEUNG, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_JHEALEY, CREDITS_BOTTOM_NAME, scale);

	// NH: Thanks - IT & Hardware
	subPage=creditsBook->addPage("CREDITS_IT");
	InsertCredit(subPage, STR_CREDITS_THANKS, CREDITS_JOB_TITLE, scale);
	// NH: Special Type
	ptr = new TEXTBOX(STR_CREDITS_IT);
	ptr->setFontSize(scale)->setStyle(TS_creditsTitle)->setYPadding(FALSE)->setSelectable(false);
	subPage->insertItem(ptr);
	InsertCredit(subPage, STR_CREDITS_NAMES_JCORRIGAN, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_ASTILL, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_DFELLOWS, CREDITS_BOTTOM_NAME, scale);
	
	// NH: PR & Media
	subPage=creditsBook->addPage("CREDITS_PR");
	InsertCredit(subPage, STR_CREDITS_PR, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_NGRIFFITH, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_JSCARCLIFFE, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_SLAWRENCE, CREDITS_BOTTOM_NAME, scale);
	
	// NH: Recruitment
	subPage=creditsBook->addPage("CREDITS_RECRUIT");
	InsertCredit(subPage, STR_CREDITS_RECRUIT, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_CHART, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_LOSBORN, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_JPINNOCK, CREDITS_BOTTOM_NAME, scale);
	
	// NH: Admin
	subPage=creditsBook->addPage("CREDITS_ADMIN");
	InsertCredit(subPage, STR_CREDITS_ADMIN, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_APARKER, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_CSTRINGER, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_DCARBERY, CREDITS_BOTTOM_NAME, scale);
	
	// NH: Representation
	subPage=creditsBook->addPage("CREDITS_REPS");
	InsertCredit(subPage, STR_CREDITS_REPS, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_JLYONS, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_GHERBERT, CREDITS_BOTTOM_NAME, scale);
	
	// NH: Accountant
	subPage=creditsBook->addPage("CREDITS_ACCOUNTANT");
	InsertCredit(subPage, STR_CREDITS_ACCOUNTANT, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_CTHORNICROFT, CREDITS_BOTTOM_NAME, scale);
	
	// NH: Human Resorces
	subPage=creditsBook->addPage("CREDITS_HUMAN_RESOURCE");
	InsertCredit(subPage, STR_CREDITS_HUMAN_RESOURCE, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_STHORBURN, CREDITS_BOTTOM_NAME, scale);
	
	// NH: Commercial Director
	subPage=creditsBook->addPage("CREDITS_COMMERCIAL_DIR");
	InsertCredit(subPage, STR_CREDITS_COMMERCIAL_DIR, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_NDAVIES, CREDITS_BOTTOM_NAME, scale);
	
	// NH: Development Director
	subPage=creditsBook->addPage("CREDITS_DEV_DIR");
	InsertCredit(subPage, STR_CREDITS_DEV_DIR, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_AOLIVER, CREDITS_BOTTOM_NAME, scale);
	
	// NH: CEO
	subPage=creditsBook->addPage("CREDITS_CEO");
	InsertCredit(subPage, STR_CREDITS_CEO, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_POLIVER, CREDITS_BOTTOM_NAME, scale);
	
	// NH: Designed & Developed
	subPage=creditsBook->addPage("CREDITS_DESIGNED_AND_DEVELOPED");
	InsertCredit(subPage, STR_CREDITS_DESIGNED_AND_DEVELOPED, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_BLITZ, CREDITS_BOTTOM_NAME, scale);	
}

/*	--------------------------------------------------------------------------------
	Function 	: CreateInfogramesCredits
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void CreateInfogramesCredits(struct TAG_BOOK* creditsBook, char skipable, float scale)
{
	PAGE			*subPage;

	if(skipable) AddCreditsSection();

/*	////START_OLD////
	// NH: 
	subPage=creditsBook->addPage("CREDIT_I_MARKETING_DIRECTOR");
	InsertCredit(subPage, STR_CREDIT_I_MARKETING_DIRECTOR, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDIT_I_K_ORDERS, CREDITS_BOTTOM_NAME, scale);
*/	////END_OLD/////

	// NH: 
	subPage=creditsBook->addPage("CREDIT_I_PRODUCTION_DIRECTOR");
	InsertCredit(subPage, STR_CREDIT_I_PRODUCTION_DIRECTOR, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDIT_I_EPROTIERE, CREDITS_BOTTOM_NAME, scale);
	
	// NH: 
	subPage=creditsBook->addPage("CREDIT_I_PRODUCER");
	InsertCredit(subPage, STR_CREDIT_I_PRODUCER, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDIT_I_HSTRANDBERG, CREDITS_BOTTOM_NAME, scale);
	
	////START_NEW/////
	
	// NH: 
	subPage=creditsBook->addPage("CREDITS_AD_DIRECTION");
	InsertCredit(subPage, STR_CREDITS_AD_DIRECTION, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_I_MDUTRIEZ, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_I_NPOTHIER, CREDITS_BOTTOM_NAME, scale);
	
	/////END_NEW//////
 
/*  ////START_OLD///
	// NH:
	subPage=creditsBook->addPage("CREDIT_I_BRAND_MANAGER");
	InsertCredit(subPage, STR_CREDIT_I_BRAND_MANAGER, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDIT_I_LARNAUD, CREDITS_BOTTOM_NAME, scale);
*/  /////END_OLD////
	
	// NH: 
	subPage=creditsBook->addPage("CREDIT_I_PRODUCT_MANAGER");
	InsertCredit(subPage, STR_CREDIT_I_PRODUCT_MANAGER, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDIT_I_BAUGUIN, CREDITS_BOTTOM_NAME, scale);
	
	// NH: 
	subPage=creditsBook->addPage("CREDIT_I_EDITION");
	InsertCredit(subPage, STR_CREDIT_I_EDITION, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDIT_I_PCHOUZENOUX, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDIT_I_MMEGOZ, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDIT_I_BVRDOLJAK, CREDITS_BOTTOM_NAME, scale);
	
	// NH: 
	subPage=creditsBook->addPage("CREDIT_I_ADMIN");
	InsertCredit(subPage, STR_CREDIT_I_ADMIN, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDIT_I_CCIAMPI, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDIT_I_NRIGOT, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDIT_I_YALONSO, CREDITS_BOTTOM_NAME, scale);
	
	// NH: 
	subPage=creditsBook->addPage("CREDIT_I_SFTM");
	InsertCredit(subPage, STR_CREDIT_I_SFTM, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDIT_I_OROBIN, CREDITS_BOTTOM_NAME, scale);
	
	// NH: 
	subPage=creditsBook->addPage("CREDIT_I_GESM");
	InsertCredit(subPage, STR_CREDIT_I_GESM, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDIT_I_DMOREL, CREDITS_BOTTOM_NAME, scale);
	
	// NH: 
	subPage=creditsBook->addPage("CREDIT_I_GESC");
	InsertCredit(subPage, STR_CREDIT_I_GESC, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDIT_I_ECHOLLEY, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDIT_I_JLAPASSET, CREDITS_BOTTOM_NAME, scale);
	
	// NH: 
	subPage=creditsBook->addPage("CREDIT_I_SFTS");
	InsertCredit(subPage, STR_CREDIT_I_SFTS, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDIT_I_VLALOY, CREDITS_BOTTOM_NAME, scale);
	
	// NH: 
	subPage=creditsBook->addPage("CREDIT_I_SFTC");
	InsertCredit(subPage, STR_CREDIT_I_SFTC, CREDITS_JOB_TITLE, scale);

#if BPLATFORM == PS2
	InsertCredit(subPage, STR_CREDIT_I_JBOEVE, CREDITS_BOTTOM_NAME, scale);
#else
	InsertCredit(subPage, STR_XBOX_CREDITS_I_SBOUZAC, CREDITS_BOTTOM_NAME, scale);
#endif

	// NH: 
	subPage=creditsBook->addPage("CREDIT_I_ESS");
	InsertCredit(subPage, STR_CREDIT_I_ESS, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDIT_I_PLOUVET, CREDITS_BOTTOM_NAME, scale);
	
	// NH: 
	subPage=creditsBook->addPage("CREDIT_I_PRE_MASTERING");
	InsertCredit(subPage, STR_CREDIT_I_PRE_MASTERING, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDIT_I_SENTERIC, CREDITS_BOTTOM_NAME, scale);
	
	// NH: 
	subPage=creditsBook->addPage("CREDIT_I_TESTERS");
	InsertCredit(subPage, STR_CREDIT_I_TESTERS, CREDITS_JOB_TITLE, scale);

#if BPLATFORM == PS2
	InsertCredit(subPage, STR_CREDIT_I_SBRAHIMI, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDIT_I_SBREUIL, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDIT_I_EALVAREZ, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDIT_I_SCARANZAN, CREDITS_BOTTOM_NAME, scale);
#else
	InsertCredit(subPage, STR_XBOX_CREDITS_I_ACRAWFORD, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_XBOX_CREDITS_I_SMBOULE, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_XBOX_CREDITS_I_SBREUIL, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_XBOX_CREDITS_I_ODESTJEAN, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_XBOX_CREDITS_I_ASOULIER, CREDITS_BOTTOM_NAME, scale);
#endif
	
	// NH: 
	subPage=creditsBook->addPage("LSGM");
	InsertCredit(subPage, STR_CREDIT_I_LOCALISATION, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDIT_I_SPIVOTCHOSSAT, CREDITS_BOTTOM_NAME, scale);

	// NH: 
	subPage=creditsBook->addPage("LPM");
	InsertCredit(subPage, STR_CREDIT_I_LOCALISATION_PM, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDIT_I_KVALLET, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDIT_I_SBENNETT, CREDITS_BOTTOM_NAME, scale);

	// NH: 
	subPage=creditsBook->addPage("TPM");
	InsertCredit(subPage, STR_CREDIT_I_TRANSLATION_PM, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDIT_I_CBARBONE, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDIT_I_BVREITER, CREDITS_BOTTOM_NAME, scale);

	// NH: 
	subPage=creditsBook->addPage("LTC");
	InsertCredit(subPage, STR_CREDIT_I_LOCALISATION_TECH, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDIT_I_FROSET, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDIT_I_BPIVANO, CREDITS_BOTTOM_NAME, scale);

	// NH: 
	subPage=creditsBook->addPage("T & Recordings");
	InsertCredit(subPage, STR_CREDIT_I_TRANSLATION_REC, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDIT_I_KBP, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDIT_I_SYNTHESIS, CREDITS_BOTTOM_NAME, scale);

	// NH: 
	subPage=creditsBook->addPage("L Testing Comp");
	InsertCredit(subPage, STR_CREDIT_I_LOCALISATION_TEST, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDIT_I_BABEL_MEDIA, CREDITS_BOTTOM_NAME, scale);

	// NH: 
	subPage=creditsBook->addPage("CREDIT_I_CPSG");
	InsertCredit(subPage, STR_CREDIT_I_CPSG, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDIT_I_RPERNERED, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDIT_I_EBALLIVY, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDIT_I_CFAUCHILLE, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDIT_I_JDITULLIO, CREDITS_BOTTOM_NAME, scale);
	
	// NH: 
	subPage=creditsBook->addPage("CREDIT_I_THANKS");
	InsertCredit(subPage, STR_CREDIT_I_THANKS, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDIT_I_RCOURTOIS, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDIT_I_DPAINE, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDIT_I_RGONNARD, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDIT_I_EBAPTIZAT, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDIT_I_PCROOKS, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDIT_I_JCLARK, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDIT_I_RMATHON, CREDITS_BOTTOM_NAME, scale);

	// NH: 
	subPage=creditsBook->addPage("CREDIT_I_THANKS");
	InsertCredit(subPage, STR_CREDIT_I_THANKS, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDIT_I_ABREAS, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDIT_I_EBAESA, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDIT_I_NBALTZER, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDIT_I_KGLOVER, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDIT_I_K_ORDERS, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDIT_I_LARNAUD, CREDITS_BOTTOM_NAME, scale);	
	
}

/*	--------------------------------------------------------------------------------
	Function 	: CreateWarnerCredits
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void CreateWarnerCredits(struct TAG_BOOK* creditsBook, char skipable, float scale)
{
	PAGE			*subPage;

	if(skipable) AddCreditsSection();
	
	// NH: 
	subPage=creditsBook->addPage("CREDITS_WB_PRODUCER");
	InsertCredit(subPage, STR_CREDITS_WB_PRODUCER, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_WB_LMCTIGHE, CREDITS_BOTTOM_NAME, scale);// NH: 
	
	// NH: 
	subPage=creditsBook->addPage("CREDITS_WB_EXEC_PRODUCER");
	InsertCredit(subPage, STR_CREDITS_WB_EXEC_PRODUCER, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_WB_BSKOGEN, CREDITS_BOTTOM_NAME, scale);
		
	// NH: 
	subPage=creditsBook->addPage("CREDITS_WB_CREATIVE_DIRECTOR");
	InsertCredit(subPage, STR_CREDITS_WB_CREATIVE_DIRECTOR, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_WB_CCARNEY, CREDITS_BOTTOM_NAME, scale);
	
	// NH: 
	subPage=creditsBook->addPage("CREDITS_WB_ART_DIRECTOR");
	InsertCredit(subPage, STR_CREDITS_WB_ART_DIRECTOR, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_WB_AHELBIG, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_WB_PTUMMINELLO, CREDITS_BOTTOM_NAME, scale);
	
	// NH: 
	subPage=creditsBook->addPage("CREDITS_WB_VO_DIRECTOR");
	InsertCredit(subPage, STR_CREDITS_WB_VO_DIRECTOR, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_WB_CSUNDERMAN, CREDITS_BOTTOM_NAME, scale);
	
	// NH: 
	subPage=creditsBook->addPage("CREDITS_WB_STUDIO_ENGINEER");
	InsertCredit(subPage, STR_CREDITS_WB_STUDIO_ENGINEER, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_WB_ECORNELL, CREDITS_BOTTOM_NAME, scale);
	
	// NH: 
	subPage=creditsBook->addPage("CREDITS_WB_VOICE_TALENT");
	InsertCredit(subPage, STR_CREDITS_WB_VOICE_TALENT, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_WB_JALASKEY, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_WB_JCUMMINGS, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_WB_MLAMARCHE, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_WB_BWEST, CREDITS_BOTTOM_NAME, scale);
	
	// NH: 
	subPage=creditsBook->addPage("CREDITS_WB_SPECIAL_THANKS");
	InsertCredit(subPage, STR_CREDITS_WB_SPECIAL_THANKS, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_WB_PERWIN, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_WB_MHARKAVY, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_WB_RSEBASTIAN, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_WB_SJOHNSON, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_WB_JMOLINARO, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_WB_LBOND, CREDITS_BOTTOM_NAME, scale);
	
	// NH: 
	subPage=creditsBook->addPage("CREDITS_WB_SPECIAL_THANKS");
	InsertCredit(subPage, STR_CREDITS_WB_SPECIAL_THANKS, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_WB_JMCNALLY, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_WB_AKIRSHEN, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_WB_DBINNIE, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_WB_HBEHRENDT, CREDITS_BOTTOM_NAME, scale);
	
	// NH: 
	subPage=creditsBook->addPage("CREDITS_WB_DUBBING");
	InsertCredit(subPage, STR_CREDITS_WB_DUBBING, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_WB_CFRIZAT, CREDITS_NAME, scale);
	InsertCredit(subPage, STR_CREDITS_WB_JBARREAU, CREDITS_BOTTOM_NAME, scale);
}

/*	--------------------------------------------------------------------------------
	Function 	: CreateNimrodCredits
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void CreateNimrodCredits(struct TAG_BOOK* creditsBook, char skipable, float scale)
{
	PAGE			*subPage;
	
	if(skipable) AddCreditsSection();

	subPage=creditsBook->addPage("CREDITS_NIMROD");
	InsertCredit(subPage, STR_CREDITS_MUSIC, CREDITS_JOB_TITLE, scale);
	InsertCredit(subPage, STR_CREDITS_NAMES_NIMROD, CREDITS_BOTTOM_NAME, scale);
}

/*	--------------------------------------------------------------------------------
	Function 	: CreateLegalCredits
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void CreateLegalCredits(struct TAG_BOOK* creditsBook, char skipable, float scale)
{
	PAGE			*subPage;
	
	if(skipable) AddCreditsSection();
	// NH: (57)
	subPage=creditsBook->addPage("LEGAL");
	
	subPage->insertItem(STR_CREDIT_LEGAL_1)->setStyle(TS_creditsTitle)->setYPadding(FALSE)->setYAlign(PIFLAG_YALIGN_BOTTOM)->setSelectable(false);
	subPage->insertItem(STR_CREDIT_LEGAL_2)->setStyle(TS_creditsTitle)->setYPadding(FALSE)->setSelectable(false);
	subPage->insertItem(STR_CREDIT_LEGAL_3)->setStyle(TS_creditsTitle)->setWrap(true)->setScale(0.75f)->setYPadding(FALSE)->setSelectable(false);
	subPage->insertItem(STR_CREDIT_LEGAL_4)->setStyle(TS_creditsTitle)->setWrap(true)->setScale(0.75f)->setYAlign(PIFLAG_YALIGN_TOP)->setSelectable(false);
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateCreditsPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateCreditsPage(FRONTENDMENU *menu, PADCONTROLSTATUS *controller)
{
	CREDITSPAGE	*ptr = menu->creditsPage;
	
	if (gameStatus.gameTime > creditChangeTimer)
	{	
		// NH: page > last page? yes = set page to 0
		if (creditsPageNumber>creditsSections[creditsPosition])
		{
			creditsPageNumber=0;
		}
		else
		{
			creditsPageNumber++;
		}

		ptr->creditsSubBook->gotoPageNumber(creditsPageNumber);
	
		if(creditsPageNumber == creditsSections[lastBeforeLegal])
		{
			// NH: Increase Time for legal bits
			creditChangeTimer = gameStatus.gameTime + CREDITSDELAY*CREDITSLEGALMULTIPLYER;
		}
		else
		{
			creditChangeTimer = gameStatus.gameTime + CREDITSDELAY;
		}
	}

	if(creditsPageNumber == 0) // NH: Bruno
		ptr->creditsTitleSubBook->gotoPageNumber(0);
	else if(creditsPageNumber == creditsSections[0]) // NH: Blitz EURO / InfogramesIntUS US 
		ptr->creditsTitleSubBook->gotoPageNumber(1);
	else if(creditsPageNumber == creditsSections[1]) // NH: Infogrames EURO / InfogramesUS US
		ptr->creditsTitleSubBook->gotoPageNumber(2);
	else if(creditsPageNumber == creditsSections[2]) // NH: WB EURO / Blitz US
		ptr->creditsTitleSubBook->gotoPageNumber(3);
	else if(creditsPageNumber == creditsSections[3]) // NH: Nimrod EURO / Infogrames US
		ptr->creditsTitleSubBook->gotoPageNumber(4);
	else if(creditsPageNumber == creditsSections[4]) // NH: Copyright EURO / Nimrod US
		ptr->creditsTitleSubBook->gotoPageNumber(5);
	// NH: NTSC (American) Version Only
	else if((videoMode.flags & BDISPLAYFLAG_NTSC) && (creditsPageNumber == creditsSections[5])) // NH: Nimrod US
		ptr->creditsTitleSubBook->gotoPageNumber(6);
	else if((videoMode.flags & BDISPLAYFLAG_NTSC) && (creditsPageNumber == creditsSections[6])) // NH: Copyright US
		ptr->creditsTitleSubBook->gotoPageNumber(7);

	//cross to step on
	if(OKBUTTON)
	{
		if (creditsPageNumber<creditsSections[0])
		{
			ptr->creditsSubBook->gotoPageNumber(creditsPageNumber=creditsSections[0]);
			ptr->creditsTitleSubBook->gotoPageNumber(1);
		}
		else if (creditsPageNumber<creditsSections[1])
		{
			ptr->creditsSubBook->gotoPageNumber(creditsPageNumber=creditsSections[1]);
			ptr->creditsTitleSubBook->gotoPageNumber(2);
		}
		else if (creditsPageNumber<creditsSections[2])
		{
			ptr->creditsSubBook->gotoPageNumber(creditsPageNumber=creditsSections[2]);
			ptr->creditsTitleSubBook->gotoPageNumber(3);
		}
		else if (creditsPageNumber<creditsSections[3])
		{
			ptr->creditsSubBook->gotoPageNumber(creditsPageNumber=creditsSections[3]);
			ptr->creditsTitleSubBook->gotoPageNumber(4);
		}
		else if (creditsPageNumber<creditsSections[4])
		{
			ptr->creditsSubBook->gotoPageNumber(creditsPageNumber=creditsSections[4]);
			ptr->creditsTitleSubBook->gotoPageNumber(5);
		}
		// NH: NTSC (American) Version Only
		else if ((videoMode.flags & BDISPLAYFLAG_NTSC) && (creditsPageNumber<creditsSections[5]))
		{
			ptr->creditsSubBook->gotoPageNumber(creditsPageNumber=creditsSections[5]);
			ptr->creditsTitleSubBook->gotoPageNumber(6);
		}
		else if ((videoMode.flags & BDISPLAYFLAG_NTSC) && (creditsPageNumber<creditsSections[6]))
		{
			ptr->creditsSubBook->gotoPageNumber(creditsPageNumber=creditsSections[6]);
			ptr->creditsTitleSubBook->gotoPageNumber(7);
		}
		else
		{
			ptr->creditsSubBook->gotoPageNumber(creditsPageNumber=0);
			ptr->creditsTitleSubBook->gotoPageNumber(0);
		}
		
		creditChangeTimer = gameStatus.gameTime + CREDITSDELAY;
	}	

	//triangle to reset back to main menu
	if(CANCELBUTTON)
	{
		// NH: Play bowhomb sound. Would be nice to be in PageFX, but not sure how to do it so this will work for now
		menu->creditsPage->page->playNavSound(menu->creditsPage->page->menuSounds, EMSFX_BACK);
		SetFrontEndCameraDestination(FECAM_EXTRAS_CREDITS,FECAM_EXTRAS);
		menu->frontEndMenuBook->gotoPage(menuNames[MENU_EXTRAS]);
		menu->currentPage = MENU_EXTRAS;
		creditChangeTimer = gameStatus.gameTime-1.0f;
	}
}