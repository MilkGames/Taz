#ifndef __H_TUTORIAL
#define __H_TUTORIAL

#define	TUT1_FRIDGE	0
#define TUT1_CRATE	1

#include "text2.h"
#include "icon.h"
#include "textfx.h"
#include "checkbox.h"
#include "PaperBook.h"				// PP: 'Paper' book type, ie. one that looks and acts like a real physical book

void initTutorial();

int doTutorial(int stage);
/*
#define TUTORIALBOOK_GLOBAL_SCALE			0.8f// PP: I don't know what has changed but everything has got bigger// PP: I'm far too tired to work it out right now so I'll just bodge
#define TUTORIALBOOK_FONTSIZE_HEADING		(1.3f*TUTORIALBOOK_GLOBAL_SCALE)// PP: was1.5				// PP: font size for heading text
#define TUTORIALBOOK_FONTSIZE_BODY			(1.1f*TUTORIALBOOK_GLOBAL_SCALE)						// PP: font size for body text
#define TUTORIALBOOK_SCALE_PHOTO			(4.25f*TUTORIALBOOK_GLOBAL_SCALE)						// PP: scale factor for photo icons
*/

#define TUTORIALBOOK_TEXTCOLOUR_HEADING		COLOUR(90, 0, 0, 128)				// PP: colour for heading text
#define TUTORIALBOOK_TEXTCOLOUR_BODY		COLOUR(0, 0, 0, 128)			// PP: colour for body text

extern float TUTORIALBOOK_GLOBAL_SCALE;
extern float TUTORIALBOOK_FONTSIZE_HEADING;
extern float TUTORIALBOOK_FONTSIZE_BODY;
extern float TUTORIALBOOK_SCALE_PHOTO;

/*	--------------------------------------------------------------------------------
	Function 	: drawTutorial
	Purpose 	: draw tutorial stuff, eg. tutorial paperbook
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: separated this from doTutorial because it was getting drawn at update stage -
							problematic in a number of ways, most importantly that the level was drawing after it & therefore on top of it
*/
// PP: REMOUT: OOD	void drawTutorial(void);

/* PP: REMOUT: OOD & duplicated code

int doTutorialOne();
int doTutorialTwo();
int doTutorialThree();
int doTutorialFour();

void DrawTutorialBookOne();
void DrawTutorialBookTwo();
void DrawTutorialBookThree();
void DrawTutorialBookFour();
*/

void createTutorialBookPages();

void GetTutorialAreaInfo();
void GetTutOneInfo();
void ResetTutorialOne();
void GetTutThreeInfo();
void ResetTutorialThree();

void InitTutorialBook(int);
void EnterTutorialArea(int);
void ExitTutorialArea(int);
void KillTutorial();

// JW: Though the functions are actually defined in special.cpp, i've put these here so it affects less files.

void CloseTutInfBook();
void OpenTutInfBook();

/*	--------------------------------------------------------------------------------
	Function 	: tutorialFirstPageCallback
	Purpose 	: receives notification that the tutorial has turned/opened on the first page
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void tutorialFirstPageCallback(void);

/*	--------------------------------------------------------------------------------
	Function 	: tutorialCentralPageCallback
	Purpose 	: receives notification that the tutorial has turned/opened on a central page
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void tutorialCentralPageCallback(void);

/*	--------------------------------------------------------------------------------
	Function 	: tutorialLastPageCallback
	Purpose 	: receives notification that the tutorial has turned/opened on the last page
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void tutorialLastPageCallback(void);

/* PP: REMOUT: OOD
inline void AddTutorialChapter()
{
}*/

#endif
