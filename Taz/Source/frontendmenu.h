#ifndef __H_FRONTENDMENU
#define __H_FRONTENDMENU

#include "audiomenu.h"
#include "extrasmenu.h"
#include "optionsmenu.h"
#include "recordsmenu.h"
#include "startgamemenu.h"
#include "twoplayermenu.h"
#include "gallerymenu.h"
#include "secretsmenu.h"
#include "credits.h"
#include "popup.h"
#include "icon.h"

#define	FRONTEND_TEXT_SCALE			(0.8f)
#define MENU_NUMINCEDENTALS			6

enum EMenuPage
{
	MENU_INTROMAIN,
	MENU_MAIN,
	MENU_STARTGAME,
	MENU_OPTIONS,
	MENU_AUDIO,
	MENU_TWOPLAYER,
	MENU_EXTRAS,
	MENU_RECORDS,
	MENU_GALLERY,
	MENU_CREDITS,
	MENU_SECRETS,
	MENU_BLANK,
	MENU_SLOTONE,
	MENU_SLOTTWO,
	MENU_SLOTTHREE,

	MENU_NUMSUBMENUS,

	MENU_GAME,
};

enum EMainMenuSubState
{
	MM_INTRO,
	MM_WAITFORLOAD,
	MM_START,
	MM_OPTIONS,
	MM_AUDIO,
	MM_TWOPLAYER,
	MM_RECORDS,
	MM_EXTRAS,
};

enum EMainAnimState
{
	NULL_STATE,
	MM_IDLE,
	SM_SLOT_IDLE,
	SM_SLOT_ONE_TO_TWO,
	SM_SLOT_TWO_TO_THREE,
	SM_SLOT_THREE_TO_TWO,
	SM_SLOT_TWO_TO_ONE,
	OM_PIZZA_IDLE,
	OM_CLOCK_IDLE,
	OM_TV_IDLE,
	OM_TV_ADJUST_IDLE,
	OM_TV_HIT,
	OM_TV_TWIDDLE,
	OM_TV_TWIDDLE2,
	OM_TV_TWIDDLE3,
	OM_EASY_TO_MEDIUM,
	OM_MEDIUM_TO_HARD,
	OM_HARD_TO_MEDIUM,
	OM_MEDIUM_TO_EASY,
	OM_VIB_TO_ON,
	OM_VIB_TO_OFF,
	OM_NOWHERE,
	AM_GRAMOPHONE,
	AM_PLUNGER,
	TP_IDLE,
	RM_IDLE,
	EM_IDLE,
	AM_IDLE,
	SG_IDLE,
	GOING_HOME,
};

typedef struct TAG_MAINMENUPAGE
{
	struct TAG_TEXTBOX	*startGameTb;
	struct TAG_TEXTBOX	*gameOptionsTb;
	struct TAG_TEXTBOX	*audioOptionsTb;
	struct TAG_TEXTBOX	*twoPlayerTb;

	struct TAG_ICON		*titleLogo;
	struct TAG_TEXTBOX	*introTitle;
	struct TAG_TEXTBOX	*recordsTb;
	struct TAG_TEXTBOX	*extrasTb;
	struct TAG_TEXTBOX	*exitTb;
	struct TAG_BOOK		*infoSubBook;
	struct TAG_BOOK		*controlSubBook;
	struct TAG_BOOK		*introControlSubBook;
	struct TAG_BOOK		*actionSubBook;
	struct TAG_TEXTBOX	*instructionTb;
	struct TAG_BOOK		*instructionSubBook;
	struct TAG_BOOK		*subBook;
	struct TAG_PAGE		*page;

	EMainMenuSubState	subState;
	int					iPad[3];
}MAINMENUPAGE, _MAINMENUPAGE BALIGN16;

typedef struct TAG_STARTGAMEPAGE
{
	int					pathToFollow;
	uint32				slotNum;
	int					currentSlot;
	EStartMenuSubState	subState;
	EStartMenuSubState	lastSubState;
	struct TAG_TEXTBOX	*slotOneTb;
	struct TAG_TEXTBOX	*titleTb;
	struct TAG_TEXTBOX	*slotTwoTb;
	struct TAG_TEXTBOX	*slotThreeTb;
	struct TAG_BOOK		*subBook;
	struct TAG_BOOK		*detailsSubBook;
	struct TAG_PAGE		*page;
	struct TAG_PAGE		*blankPage;
	struct TAG_BOOK		*infoSubBook;
	struct TAG_BOOK		*controlSubBook;
	struct TAG_BOOK		*actionSubBook;
	struct TAG_BOOK		*slotBook;
	struct TAG_BOOK		*slotSubBook;
	struct TAG_BOOK		*instructionSubBook;

	struct TAG_TEXTBOX	*loadTb;
	struct TAG_TEXTBOX	*slot1BountyTb;
	struct TAG_TEXTBOX	*slot2BountyTb;
	struct TAG_TEXTBOX	*slot3BountyTb;
	struct TAG_TEXTBOX	*slot1PercentageTb;
	struct TAG_TEXTBOX	*slot2PercentageTb;
	struct TAG_TEXTBOX	*slot3PercentageTb;
	struct TAG_TEXTBOX	*slot1DetailsTb;
	struct TAG_TEXTBOX	*slot2DetailsTb;
	struct TAG_TEXTBOX	*slot3DetailsTb;
	struct TAG_TEXTBOX	*slot1NameTb;
	struct TAG_TEXTBOX	*slot2NameTb;
	struct TAG_TEXTBOX	*slot3NameTb;
	struct TAG_TEXTBOX	*slot1DiffTb;
	struct TAG_TEXTBOX	*slot2DiffTb;
	struct TAG_TEXTBOX	*slot3DiffTb;
	struct TAG_TEXTBOX	*slot1HubTb;
	struct TAG_TEXTBOX	*slot2HubTb;
	struct TAG_TEXTBOX	*slot3HubTb;
	struct TAG_ICON		*slot1Icon;
	struct TAG_ICON		*slot2Icon;
	struct TAG_ICON		*slot3Icon;
	struct TAG_TEXTBOX	*emptySlotDetailsTb;
	struct TAG_TEXTBOX	*deleteTb;
	struct TAG_TEXTBOX	*chooseLoadTb;
	struct TAG_TEXTBOX	*chooseDeleteTb;
	int					iPad;
}STARTGAMEPAGE, _STARTGAMEPAGE BALIGN16;

typedef struct TAG_GAMEOPTIONSPAGE
{
	EOptionsSubState	subState;
	EOptionsSubState	lastSubState;
	struct TAG_TEXTBOX	*difficultyTb;
	struct TAG_TEXTBOX	*vibrationTb;

	struct TAG_TEXTBOX	*titleTb;
	struct TAG_TEXTBOX	*screenPosTb;
	struct TAG_TEXTBOX	*subtitlesTb;

	struct TAG_TEXTBOX	*diffActionTb;
	struct TAG_TEXTBOX	*vibActionTb;
	struct TAG_TEXTBOX	*posActionTb;
	struct TAG_TEXTBOX	*subtitlesActionTb;
	struct TAG_TEXTBOX	*instructionTb;

	struct TAG_BOOK		*infoSubBook;
	struct TAG_BOOK		*controlSubBook;
	struct TAG_BOOK		*subBook;
	struct TAG_BOOK		*actionSubBook;
	struct TAG_BOOK		*instructionSubBook;
	struct TAG_PAGE		*page;
	struct TAG_PAGE		*adjustScreenPage;

	int					rumbleHandle;
	uint32				resetScreen;
	TBTexture			*corner;
	int					iPad;
}GAMEOPTIONSPAGE, _GAMEOPTIONSPAGE BALIGN16;

typedef struct TAG_AUDIOPAGE
{
	uint32				subState;
	bool				ShovePlungerDown;
	bool				explosionDone;
	struct TAG_TEXTBOX	*musicVolTb;
	struct TAG_SLIDER	*musicVolSl;
	int					iPadDV;

	struct TAG_TEXTBOX	*titleTb;
	struct TAG_TEXTBOX	*sfxVolTb;
	struct TAG_SLIDER	*sfxVolSl;
	struct TAG_TEXTBOX	*actionTb;
	struct TAG_TEXTBOX	*instructionTb;
	struct TAG_BOOK		*infoSubBook;
	struct TAG_BOOK		*controlSubBook;
	struct TAG_BOOK		*actionSubBook;
	struct TAG_BOOK		*instructionSubBook;

	struct TAG_TEXTBOX	*returnTb;
	struct TAG_PAGE		*page;
	int					iPad[2];
}AUDIOPAGE, _AUDIOPAGE BALIGN16;

typedef struct TAG_TWOPLAYERPAGE
{
	uint32				subState;
	struct TAG_TEXTBOX	*raceModeTb;
	struct TAG_TEXTBOX	*timeTrialTb;
	struct TAG_TEXTBOX	*destructionTb;
	
	struct TAG_TEXTBOX	*titleTb;
	struct TAG_TEXTBOX	*tournamentTb;
	struct TAG_TEXTBOX	*secretGamesTb;
	struct TAG_BOOK		*infoSubBook;
	struct TAG_BOOK		*controlSubBook;
	struct TAG_BOOK		*actionSubBook;
	struct TAG_BOOK		*secretsSubBook[3];
	struct TAG_BOOK		*gameInfoSubBook[5];
	struct TAG_BOOK		*gameControlSubBook[5];	
	struct TAG_BOOK		*instructionSubBook;
	struct TAG_TEXTBOX	*instructionTb;

	struct TAG_TEXTBOX	*returnTb;
	struct TAG_PAGE		*page;
	struct TAG_BOOK		*iconsBook;
	struct TAG_BOOK		*iconBook[3];
	struct TAG_ICON		*tourneyIcon[3];
	struct TAG_TEXTBOX	*tourneyText[3];
	struct TAG_TEXTBOX	*racesTb[4];
	struct TAG_TEXTBOX	*vrracesTb[4];
	struct TAG_TEXTBOX	*destGamesTb[4];
	struct TAG_TEXTBOX	*bossesTb[4];
	struct TAG_TEXTBOX	*tourneyTb[5];
}TWOPLAYERPAGE, _TWOPLAYERPAGE BALIGN16;

typedef struct TAG_RECORDSPAGE
{
	uint32				subState;
	uint32				subSubState;
	struct TAG_PAGE		*page;
	struct TAG_BOOK		*worldBook;
	
	struct TAG_BOOK		*overallBook;
	struct TAG_BOOK		*zooBook;
	struct TAG_BOOK		*cityBook;
	struct TAG_BOOK		*westBook;
	struct TAG_BOOK		*tasmaniaBook;
	struct TAG_BOOK		*infoSubBook;
	struct TAG_BOOK		*controlSubBook;

	struct TAG_BOOK		*actionBook;
	struct TAG_PAGE		*records;
	uint32				preRecordsState;

	struct TAG_TEXTBOX	*titleTb;

	struct TAG_TEXTBOX	*hiscoreBountyTb[4][3];
	struct TAG_TEXTBOX	*hiscoreNameTb[4][3];

	struct TAG_BOOK		*leftHubIconBook;
	struct TAG_BOOK		*rightHubIconBook;
	struct TAG_BOOK		*leftLevelIconBook;
	struct TAG_BOOK		*rightLevelIconBook;
	struct TAG_BOOK		*hubControlBook;
	struct TAG_BOOK		*levelControlBook;
	struct TAG_TEXTBOX	*hubControl[5];
	struct TAG_TEXTBOX	*levelControl[4][3];
	struct TAG_ICON		*hubLeftIcon;
	struct TAG_ICON		*hubRightIcon;
	struct TAG_ICON		*levelLeftIcon;
	struct TAG_ICON		*levelRightIcon;
	
	struct TAG_ICON		*hiscoreIcon[4][3];
	struct TAG_ICON		*destructableIcon[4][3];
	struct TAG_ICON		*destructableTick[4][3];
	struct TAG_ICON		*secretIcon[4][3];
	struct TAG_ICON		*secretTick[4][3];
	struct TAG_ICON		*sandwichIcon[4][3];
	struct TAG_ICON		*sandwichTick[4][3];
	struct TAG_ICON		*posterIcon[4][3];
	struct TAG_TEXTBOX	*posterTb[4][3];

}RECORDSPAGE, _RECORDSPAGE BALIGN16;

typedef struct TAG_EXTRASPAGE
{
	uint32				subState;
	struct TAG_TEXTBOX	*titleTb;
	struct TAG_TEXTBOX	*creditsTb;
	struct TAG_TEXTBOX	*galleryTb;
	struct TAG_TEXTBOX	*secretsTb;

	struct TAG_BOOK		*infoSubBook;
	struct TAG_BOOK		*controlSubBook;
	struct TAG_BOOK		*actionSubBook;
	struct TAG_BOOK		*instructionSubBook;
	struct TAG_TEXTBOX	*instructionTb;
	struct TAG_TEXTBOX	*returnTb;
	struct TAG_PAGE		*page;

}EXTRASPAGE, _EXTRASPAGE BALIGN16;

typedef struct TAG_GALLERYPAGE
{
	uint32					subState;
	uint32					galleryLoadState;
	uint32					galleryLastLoaded;
	struct TAG_TEXTBOX		*titleTb;
	struct TAG_TEXTBOX		*Tb;
	struct TAG_ICON			*galleryIcon[10];
	struct TAG_ICON			*galleryImage[10];
	struct TAG_BOOK			*titleSubBook;
	struct TAG_BOOK			*infoSubBook;
	struct TAG_BOOK			*controlSubBook;
	struct TAG_BOOK			*galleryInfoSubBook;
	struct TAG_BOOK			*galleryControlSubBook;
	struct TAG_PAGE			*page;
	struct TAG_BOOK	*textBook;
	struct TAG_PAPERBOOK	*galleryBook;

}GALLERYPAGE, _GALLERYPAGE BALIGN16;

typedef struct TAG_CREDITSPAGE
{
	uint32				subState;
	struct TAG_TEXTBOX	*titleTb;
	struct TAG_BOOK		*creditsTitleSubBook;
	struct TAG_BOOK		*creditsSubBook;
	struct TAG_BOOK		*infoSubBook;
	struct TAG_BOOK		*controlSubBook;
	struct TAG_PAGE		*page;

}CREDITSPAGE, _CREDITSPAGE BALIGN16;

typedef struct TAG_SECRETSPAGE
{
	uint32				subState;
	struct TAG_TEXTBOX	*titleTb;
	struct TAG_TEXTBOX	*levelTb[9];
	struct TAG_BOOK		*infoSubBook;
	struct TAG_BOOK		*controlSubBook;
	struct TAG_PAGE		*page;

}SECRETSPAGE, _SECRETSPAGE BALIGN16;

typedef struct TAG_FRONTENDMENU
{
	TBMatrix				location;

	MAINMENUPAGE			*mainMenuPage;
	STARTGAMEPAGE			*startGamePage;
	GAMEOPTIONSPAGE			*gameOptionsPage;
	AUDIOPAGE				*audioPage;

	TWOPLAYERPAGE			*twoPlayerPage;
	RECORDSPAGE				*recordsPage;
	EXTRASPAGE				*extrasPage;
	GALLERYPAGE				*galleryPage;
	CREDITSPAGE				*creditsPage;
	SECRETSPAGE				*secretsPage;
	struct TAG_BOOK			*frontEndMenuBook;
	
	POPUP					*popUpOptions;

	EMenuPage				currentPage;
	int						iPad[3];

}FRONTENDMENU, _FRONTENDMENU BALIGN16;

extern FRONTENDMENU	frontEndMenu;
extern char *menuNames[];

/*	--------------------------------------------------------------------------------
	Function 	: CreateFrontEndMenuBook
	Purpose 	: doh, it creates the front end menu book
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void CreateFrontEndMenuBook(void);

/*	--------------------------------------------------------------------------------
	Function 	: CreateMainMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void CreateMainMenuPage(FRONTENDMENU *menu);

/*	--------------------------------------------------------------------------------
	Function 	: FreeFrontEndMenuBook
	Purpose 	: guess what, it frees the front end memory book and any associated memory
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void FreeFrontEndMenuBook(void);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateFrontEndMenuBook
	Purpose 	: i'm not going to even bother with this one, work it out yourself
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateFrontEndMenuBook(void);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateMainMenuPage
	Purpose 	: i'm not going to even bother with this one, work it out yourself
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateMainMenuPage(FRONTENDMENU *menu, struct TAG_PADCONTROLSTATUS *controller);

/*	--------------------------------------------------------------------------------
	Function 	: DrawFrontEndMenuBook
	Purpose 	: i'm not going to even bother with this one, work it out yourself
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

#endif