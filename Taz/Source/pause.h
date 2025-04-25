#ifndef __H_PAUSE
#define __H_PAUSE

#include "textfx.h"
#include "popup.h"
#include "Cheats.h"								// PP: Cheats

#define	PAUSE_MENU_TEXT_SCALE			(1.0f)

enum EPauseMenuPage
{
	PAUSE_MENU,
	PAUSE_MAP,
	PAUSE_STATS,
	PAUSE_OPTIONS,
#ifdef INGAME_CHEATS_MENU
	PAUSE_CHEATS,								// PP: the cheats menu
#endif// PP: def INGAME_CHEATS_MENU
	
	PAUSE_NUMSUBMENUS,
};

enum EPausePopups
{
	PAUSE_POPUP_VIBRATION,
	PAUSE_POPUP_SUBTITLES,
	PAUSE_POPUP_EXIT,
	PAUSE_POPUP_MUSICVOLUME,
	PAUSE_POPUP_SFXVOLUME,
};

typedef struct TAG_PAUSEOPTIONSPAGE
{
	struct TAG_TEXTBOX	*titleTb;
	struct TAG_TEXTBOX	*musicVolTb;
	struct TAG_TEXTBOX	*sfxVolTb;
	struct TAG_TEXTBOX	*vibrationTb;
	struct TAG_TEXTBOX	*subtitlesTb;
	struct TAG_TEXTBOX	*exitTb;

	struct TAG_SLIDER	*musicVolSl;
	struct TAG_SLIDER	*sfxVolSl;

	struct TAG_BOOK		*infoSubBook;
	struct TAG_BOOK		*controlSubBook;

	struct TAG_PAGE		*page;
	
}PAUSEOPTIONSPAGE, _PAUSEOPTIONSPAGE BALIGN16;

typedef struct TAG_PAUSEMAPPAGE
{
	struct TAG_TEXTBOX	*titleTb;

	struct TAG_BOOK		*infoSubBook;
	struct TAG_BOOK		*controlSubBook;

	struct TAG_PAGE		*page;
	
}PAUSEMAPPAGE, _PAUSEMAPPAGE BALIGN16;

typedef struct TAG_PAUSESTATSPAGE
{
	struct TAG_TEXTBOX	*titleTb;

	struct TAG_BOOK		*infoSubBook;
	struct TAG_BOOK		*controlSubBook;

	struct TAG_TEXTBOX	*mapTb;
	struct TAG_TEXTBOX	*optionsTb;

	struct TAG_PAGE		*page;
	
}PAUSESTATSPAGE, _PAUSESTATSPAGE BALIGN16;

typedef struct TAG_PAUSEMENUPAGE
{
	struct TAG_TEXTBOX	*titleTb;
	struct TAG_TEXTBOX	*mapTb;
	struct TAG_TEXTBOX	*statsTb;
	struct TAG_TEXTBOX	*optionsTb;
	
	struct TAG_BOOK		*infoSubBook;
	struct TAG_BOOK		*controlSubBook;
	
	struct TAG_PAGE		*page;
	
}PAUSEMENUPAGE, _PAUSEMENUPAGE BALIGN16;

typedef struct TAG_PAUSEMENU
{
	PAUSEMENUPAGE			*pauseMenuPage;
	PAUSEMAPPAGE			*pauseMapPage;
	PAUSESTATSPAGE			*pauseStatsPage;
	PAUSEOPTIONSPAGE		*pauseOptionsPage;

	POPUP					*pausePopUp;

	struct TAG_BOOK			*pauseMenuBook;
	struct TAG_TABLE		*pauseCheatsTable;				// PP: table of cheats

	EPauseMenuPage			currentPage;
	bool					enteredMapUsingShortCut;		// TP: set to true if the player used the short cut to enter the map
}PAUSEMENU, _PAUSEMENU BALIGN16;

typedef struct TAG_PAUSEMULTIPLAYERPAGE
{
	struct TAG_TEXTBOX	*titleTb;

	struct TAG_TEXTBOX	*resumeTb;
	struct TAG_TEXTBOX	*quitTb;

	struct TAG_BOOK		*infoSubBook;
	struct TAG_BOOK		*controlSubBook;

	struct TAG_PAGE		*page;

	POPUP				*pausePopUp;
	
}PAUSEMULTIPLAYERPAGE, _PAUSEMULTIPLAYERPAGE BALIGN16;

extern PAUSEMENU	pauseMenu;
extern char *pauseMenuNames[];
extern bool pauseMenuActive;

typedef class TAG_CONFIRM : public BOOK
{
private:

	PAGE		*page;

public:

	/*  --------------------------------------------------------------------------------
		Function	: CONFIRM::TAG_CONFIRM
		Purpose		: Default constructor
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	TAG_CONFIRM(int header = -1);

	/*  --------------------------------------------------------------------------------
		Function	: CONFIRM::~TAG_CONFIRM
		Purpose		: Default destructor
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	~TAG_CONFIRM();

	/*  --------------------------------------------------------------------------------
		Function	: int CONFIRM::update
		Purpose		: respond to control input
		Parameters	: 
		Returns		:  -1 no selection, TRUE/FALSE (select/cancel)
		Info		: 
	*/
	int Update();

} CONFIRM, _CONFIRM BALIGN16;

/*	--------------------------------------------------------------------------------
	Function 	: Pause_init
	Purpose 	: Initialise stuff for the pause mode, eg. set up the pause menu book
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: 
*/
void Pause_init(void);

/*	--------------------------------------------------------------------------------
	Function 	: Pause_shutdown
	Purpose 	: shutdown stuff for the pause mode, eg. delete the pause menu book
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: 
*/
void Pause_shutdown(void);

/* --------------------------------------------------------------------------------
   Function		: DefaultPauseUpdateFunc
   Purpose		: update function used for default pause
   Parameters	: context value, required but currently not used
   Returns		: 
   Info			: // PP: this is the one that gets called as a result of setting GAMESTATE_PAUSE
					// PP: Must match the PauseUpdateFunc typedef (main.h)
*/
void DefaultPauseUpdateFunc(const int32 context);

void DefaultMPPauseUpdateFunc(const int32 context);

void DrawMPPausePage();


/*  --------------------------------------------------------------------------------
	Function	: EnterPauseMode
	Purpose		: Called when the player enters the pause mode, init pause book etc.
	Parameters	: 
	Returns		: 
	Info		: 
*/

#define	PAUSEPAGE_NONE		(-1)	// PP: don't use any page in the Pause Menu book (eg. using controller-missing book instead)

void EnterPauseMode(int num = PAUSE_STATS);

// PP: playerIndex is the zero-based index of the player who initiated the pause
void EnterMPPause(const uchar playerIndex, bool fullScreen = false);

/*  --------------------------------------------------------------------------------
	Function	: ExitPauseMode
	Purpose		: Called when the player exits the pause mode, shutdown the pause book.
	Parameters	: 
	Returns		: 
	Info		: 
*/
void ExitPauseMode(char openBooks=TRUE);

void ExitMPPauseMode(void);

/*  --------------------------------------------------------------------------------
	Function	: ReturnToHub
	Purpose		: Send the appropriate scenechange message
	Parameters	: 
	Returns		: 
	Info		: 
*/
void ReturnToHub(void);

/*	--------------------------------------------------------------------------------
	Function 	: musicSliderUpdateFunc
	Purpose 	: callback function for the music volume slider
	Parameters 	: the value represented by the slider (0..1), ptr to slider
	Returns 	: 
	Info 		: called on every update of the slider, whether the value has changed or not
*/
void musicSliderUpdateFunc(const float sliderValue, struct TAG_SLIDER* const slider);

// TP: temp just so I don't have to get any new stuff yet, am doing disk.
void musicSliderUpdateFunc(const float sliderValue);

/*	--------------------------------------------------------------------------------
	Function 	: sfxSliderUpdateFunc
	Purpose 	: callback function for the music volume slider
	Parameters 	: the value represented by the slider (0..1)
	Returns 	: 
	Info 		: called on every update of the slider, whether the value has changed or not
*/
void sfxSliderUpdateFunc(const float sliderValue, struct TAG_SLIDER* const slider);
// TP: temp just so I don't have to get any new stuff yet, am doing disk.
void sfxSliderUpdateFunc(const float sliderValue);

/* --------------------------------------------------------------------------------
   Function		: PauseToExitMenu
   Purpose		: enter pause mode and go straight to the exit options screen
   Parameters	: 
   Returns		: 
   Info			: // PP: probably a temp thang
*/
void PauseToExitMenu(void);

/*	--------------------------------------------------------------------------------
	Function 	: UpdatePauseMenuPage
	Purpose 	: i'm not going to even bother with this one, work it out yourself
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void UpdatePauseMenuPage(PAUSEMENU *menu, struct TAG_PADCONTROLSTATUS *controller);

/*	--------------------------------------------------------------------------------
	Function 	: UpdatePauseMapPage
	Purpose 	: i'm not going to even bother with this one, work it out yourself
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void UpdatePauseMapPage(PAUSEMENU *menu, struct TAG_PADCONTROLSTATUS *controller);

/*	--------------------------------------------------------------------------------
	Function 	: UpdatePauseStatsPage
	Purpose 	: i'm not going to even bother with this one, work it out yourself
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void UpdatePauseStatsPage(PAUSEMENU *menu, struct TAG_PADCONTROLSTATUS *controller);

/*	--------------------------------------------------------------------------------
	Function 	: UpdatePauseOptionsPage
	Purpose 	: i'm not going to even bother with this one, work it out yourself
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void UpdatePauseOptionsPage(PAUSEMENU *menu, struct TAG_PADCONTROLSTATUS *controller);

/*	--------------------------------------------------------------------------------
	Function 	: CreatePauseMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void CreatePauseMenuPage(PAUSEMENU *menu);

/*	--------------------------------------------------------------------------------
	Function 	: CreatePauseMapPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void CreatePauseMapPage(PAUSEMENU *menu);

/*	--------------------------------------------------------------------------------
	Function 	: CreatePauseStatsPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void CreatePauseStatsPage(PAUSEMENU *menu);

/*	--------------------------------------------------------------------------------
	Function 	: CreatePauseOptionsPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void CreatePauseOptionsPage(PAUSEMENU *menu);

/*	--------------------------------------------------------------------------------
	Function 	: CreatePauseExitPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void CreatePauseExitPage(PAUSEMENU *menu);

void CreateMPPausePage(BOOK* book);

/*	--------------------------------------------------------------------------------
	Function 	: StartMapHint
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void StartMapHint(int);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateMapHint
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateMapHint();

/*	--------------------------------------------------------------------------------
	Function 	: DrawMapHint
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void DrawMapHint();

/*	--------------------------------------------------------------------------------
	Function 	: StopMapHint
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void StopMapHint();

void InitMapControls();
void UpdateMapControls();
void DrawMapControls();
void KillMapControls();


/*  --------------------------------------------------------------------------------
	Function	: GenericPauseModePrepare
	Purpose		: generic preparations for entering any sort of pause mode
	Parameters	: 
	Returns		: 
	Info		: // PP: pauses sound and rumbles, subdues music, etc.
					// PP: Separated this off from ExitPauseMode, to allow for other pause modes
							such as when a controller is removed.
*/
void GenericPauseModePrepare(void);


/*  --------------------------------------------------------------------------------
	Function	: GenericPauseModeEnd
	Purpose		: generic stuff that has to be done when exiting any sort of pause mode
	Parameters	: 
	Returns		: 
	Info		: // PP: resumes sound and rumbles, restores music mood, etc.
					// PP: Separated this off from ExitPauseMode, to allow for other pause modes
							such as when a controller is removed.
*/
void GenericPauseModeEnd(void);


/*  --------------------------------------------------------------------------------
	Function	: OkToEnterPause
	Purpose		: Determine if it is ok to enter the pause or map mode
	Parameters	: 
	Returns		: true/false
	Info		: 
*/

bool OkToEnterPause();


/*  --------------------------------------------------------------------------------
	Function	: OkToEnterMap
	Purpose		: check if its ok to enter the map
	Parameters	: 
	Returns		: 
	Info		: 
*/

bool OkToEnterMap();



#endif