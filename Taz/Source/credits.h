#ifndef __H_CREDITS
#define __H_CREDITS

#include "TextFX.h"		// PP: for BOOK base class
#include "text.h"

#define CREDITSDELAY			3.0f
#define CREDITSLEGALMULTIPLYER	4.0f
#define NUMBER_OF_SECTIONS		8 // NH: (Using the larger US version)

enum ECreditsSubStates
{
	EX_G,
	EX_W
};

enum ECreditType
{
	CREDITS_JOB_TITLE,
	CREDITS_NAME,
	CREDITS_BOTTOM_NAME
};

extern float	creditChangeTimer;
extern int		creditsPageNumber;

/*	--------------------------------------------------------------------------------
	Function 	: CreateCreditsPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void CreateCreditsPage(struct TAG_FRONTENDMENU *menu);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateCreditsPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateCreditsPage(struct TAG_FRONTENDMENU *menu, struct TAG_PADCONTROLSTATUS *controller);

/*	--------------------------------------------------------------------------------
	Function 	: InsertCredit
	Purpose 	: Inserts a credit line to the specified credits page
	Parameters 	: (PAGE *) creditsPage - the page to add credits too, (EStringIndex) stringIndex - the string to add, (int) type - the type of credit to add
	Returns 	: 
	Info 		: 
*/

void InsertCredit(struct TAG_PAGE *creditPage, EStringIndex stringIndex, ECreditType type, float scale=1.0f);

/*	--------------------------------------------------------------------------------
	Function 	: AddCreditsSection
	Purpose 	: Adds a skippable section to the credits (stores position so that it may be skipped to)
	Parameters 	: (bool)first - Indicates the first page (DEFAULT = FALSE)
	Returns 	: successfull - true/false
	Info 		: Could have used a linked list but just for this it seemed a bit like overkill
*/

bool AddCreditsSection(bool first = false);

/*	--------------------------------------------------------------------------------
	Function 	: CreateNimrodCredits
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/

void CreateIntroCredits(struct TAG_BOOK* creditsBook, char skipable=TRUE, float scale=1.0f);

/*	--------------------------------------------------------------------------------
	Function 	: CreateUSInfogramesIntCredits
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 9 Items
*/
void CreateUSInfogramesIntCredits(struct TAG_BOOK* creditsBook, char skipable=TRUE, float scale=1.0f);

/*	--------------------------------------------------------------------------------
	Function 	: CreateUSInfogramesCredits
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 7 Items (10 on PC)
*/
void CreateUSInfogramesCredits(struct TAG_BOOK* creditsBook, char skipable=TRUE, float scale=1.0f);

/*	--------------------------------------------------------------------------------
	Function 	: CreateNimrodCredits
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/

void CreateBlitzCredits(struct TAG_BOOK* creditsBook, char skipable=TRUE, float scale=1.0f);

/*	--------------------------------------------------------------------------------
	Function 	: CreateNimrodCredits
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/

void CreateInfogramesCredits(struct TAG_BOOK* creditsBook, char skipable=TRUE, float scale=1.0f);

/*	--------------------------------------------------------------------------------
	Function 	: CreateNimrodCredits
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/

void CreateWarnerCredits(struct TAG_BOOK* creditsBook, char skipable=TRUE, float scale=1.0f);

/*	--------------------------------------------------------------------------------
	Function 	: CreateNimrodCredits
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/

void CreateNimrodCredits(struct TAG_BOOK* creditsBook, char skipable=TRUE, float scale=1.0f);

/*	--------------------------------------------------------------------------------
	Function 	: CreateLegalCredits
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/

void CreateLegalCredits(struct TAG_BOOK* creditsBook, char skipable=TRUE, float scale=1.0f);

#endif