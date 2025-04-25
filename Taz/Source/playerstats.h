#ifndef __H_PLAYERSTATS
#define __H_PLAYERSTATS

#define	OVERLAYMONEY_FADERATE	(100.0f)
#define OVERLAYMONEY_TIME		(1.0f)
#define OVERLAYMONEY_SCALE		(1.5f)
#define ENDOFLEVELCOUNTUPRATE	(10.0f)					// CMD: counts at 50% per second
#define OVERLAY_TIME			(3.0f)

extern	struct TAG_ACTORINSTANCE	*overlayTaztop;

#include "textfx.h"
#include "endoflevelstats.h"
#include "highscores.h"

enum EOverlayState
{
	OVERLAYSTATE_OFFSCREEN,
	OVERLAYSTATE_ARRIVING,
	OVERLAYSTATE_ONSCREEN,
	OVERLAYSTATE_LEAVING,
	OVERLAYSTATE_DELAY,
	OVERLAYSTATE_PREDELAY,
};

#define		LEVELCOMPLETE_NOSTATS	(1<<0)

typedef struct TAG_PLAYERSTATS
{
	uint32				bountyDisplay;					// current displayed bounty value
	uint32				taztopCounter;
//	uint32				globalTaztopCounter[25];		//best scores from the 25 levels
//	char				posterCounter[25][10];			//25 levels, 10 posters in each
	char				mailboxCounter[25][10];			//TRUE or FALSE

	struct TAG_BOOK		*swirlOverlay;
	struct TAG_TEXTBOX	*swirlText;
	struct TAG_ICON		*swirlIcon;
	RECTANGLE			swirlRect;

	struct TAG_BOOK		*destructOverlay;
	struct TAG_TEXTBOX	*destructText;
	struct TAG_ICON		*destructIcon;
	RECTANGLE			destructRect;

//	struct TAG_BOOK		*moneyOverlay;
//	struct TAG_TEXTBOX	*moneyText;
//	struct TAG_ICON		*moneyIcon;
//	RECTANGLE			moneyRect;

	struct TAG_BOOK		*posterOverlay;
	struct TAG_TEXTBOX	*posterText;
	struct TAG_ICON		*posterIcon;
	RECTANGLE			posterRect;

	EOverlayState				swirlState;						// TP: current state of the swirl overlay
	float				swirlTime;						// TP: time counter for swirl
	float				swirlY;							// TP: y coord of swirl overlay
	float				swirlScale;

	EOverlayState				destructState;						// TP: current state of the swirl overlay
	float				destructTime;						// TP: time counter for swirl
	float				destructY;							// TP: y coord of swirl overlay
	float				destructScale;

	char				destructA:1;					// JW: Player hit 10%
	char				destructB:1;					// JW: Player hit 20%
	char				destructC:1;					// JW: Player hit 30%
	char				destructD:1;					// JW: Player hit 40%
	char				destructE:1;					// JW: Player hit 50%
	char				destructF:1;					// JW: Player hit 60%
	char				destructG:1;					// JW: Player hit 70%
	char				destructH:1;					// JW: Player hit 80%
	char				destructI:1;					// JW: Player hit 90%
	char				destructJ:1;					// JW: Player hit 100%
	char				destructK:1;					// JW: Player hit 75%
	char				destructL:1;					// JW: Player hit 5%
	char				destructM:1;					// JW: Player hit 15%
	char				destructN:1;					// JW: Player hit 25%
	char				destructO:1;					// JW: Player hit 35%
	char				destructP:1;					// JW: Player hit 45%
	char				destructQ:1;					// JW: Player hit 55%
	char				destructR:1;					// JW: Player hit 65%
	char				destructS:1;					// JW: Player hit 85%
	char				destructT:1;					// JW: Player hit 95%

	EOverlayState				burpState;						// TP: current state of the burp overlay
	float				burpTime;						// TP: time counter for burp
	float				burpY;							// TP: y coord of burp overlay

	EOverlayState				moneyState;						// TP: current state of the money overlay
	float				moneyTime;						// TP: time counter for money
	float				moneyX;							// CMD: x coord of money overlay
	float				moneyY;							// CMD: y coord of money overlay
	float				moneyScale;						// CMD: pulse componant of scale of money overlay
	int					moneyAlpha;						// CMD: alpha of money overlay

	EOverlayState				posterState;					// TP: current state of the poster overlay
	float				posterTime;						// TP: time counter for poster
	float				posterY;						// TP: y coord of poster overlay
	float				posterScale;

	// TP: int					numPostersCollected;			// CMD: number of poster actually collected
	float				numPostersDisplayed;			// CMD: number of posters displayed in stats
	// TP: int					numCollectiblesCollected;
	float				numCollectiblesDisplayed;
	int					numFoodCollected;
	float				numFoodDisplayed;
	int					numObjectsDestroyed;
	float				numDestructiblesDisplayed;
	int					numBurpCansCollected;
	float				numBurpCansDisplayed;
	uint32				numCashDisplayedEndOfLevel;
	EStringIndex		displayedRank;

	float				statsBookScale;
	EOverlayState		statsBookState;
	float				statsBookClock;
	float				rankClock;

	//int					hours,minutes,seconds;

	float				time;

	int					nameEntryStage;

	float				cheatsScale;

	struct TAG_BOOK				*cheatsBook;
	struct TAG_PAGE				*cheatsPage;
	struct TAG_TEXTBOX			*cheatsTextbox;
	
	struct TAG_BOOK				*iconInfoSubBook;
	struct TAG_BOOK				*iconControlSubBook;

	struct TAG_BOOK				*nameInfoSubBook;
	struct TAG_BOOK				*nameControlSubBook;

	struct TAG_BOOK				*endOfLevelStatsBook;
	struct TAG_PAGE				*endOfLevelStatsPage;

	struct TAG_TEXTBOX			*postersScorebox;
	struct TAG_TEXTBOX			*postersCashbox;

	struct TAG_TEXTBOX			*capturedScorebox;
	struct TAG_TEXTBOX			*capturedCashbox;
	
	struct TAG_TEXTBOX			*moneyScorebox;
	struct TAG_TEXTBOX			*moneyCashbox;
	
	struct TAG_TEXTBOX			*foodScorebox;
	struct TAG_TEXTBOX			*foodCashbox;
	
	struct TAG_TEXTBOX			*collectibleScorebox;
	struct TAG_TEXTBOX			*collectibleCashbox;
	
	struct TAG_TEXTBOX			*destructibleScorebox;
	struct TAG_TEXTBOX			*destructibleCashbox;

	struct TAG_TEXTBOX			*gameTimeScorebox;
	struct TAG_TEXTBOX			*gameTimeCashbox;

	struct TAG_TEXTBOX			*hiddenScorebox;
	struct TAG_TEXTBOX			*hiddenCashbox;
	
	struct TAG_TEXTBOX			*burpCansTextBox;
	struct TAG_TEXTBOX			*rankTextbox;
	struct TAG_TEXTBOX			*levelNameTextbox;

	struct TAG_PAGE				*highscoreSelectIcon;			//enter high score page
	struct TAG_PAGE				*iconGridPage;					//Icon Grid sub page
	struct TAG_ICON				*chooseIcon[NUM_HSE_ICONS];		//Icon grid
	int							iconNumber;						//Selected Icon
	
	struct TAG_PAGE				*highscoreEnterName;			//enter high score page
	struct TAG_PAGE				*letterGridPage;
	struct TAG_ICON				*chosenIcon;
	struct TAG_TEXTBOX			*enterName;
	struct TAG_TEXTBOX			*chooseLetter[34];				//Letter Grid

	struct TAG_PAGE				*galleryInformPage;
	struct TAG_TEXTBOX			*galleryInformText;
	
	struct TAG_TEXTBOX			*advice;
	struct TAG_TEXTBOX			*instruction;
	char						letters[3];

	struct TAG_PAGE				*topTenPage;					//high scores page
} PLAYERSTATS, _PLAYERSTATS;

typedef struct TAG_BOUNTYLISTENTRY
{
	ushort				string[32];

	TBVector			position;
	TBFontFormatting	formatting;

	int					alpha;
	float					falpha;
	int					state;
	int					xOffset;

	int					yOffset;
	int					iPad[3];

	float				scale;
	TBFont				*font;
	struct TAG_BOUNTYLISTENTRY		*next;
	struct TAG_BOUNTYLISTENTRY		*prev;
} BOUNTYLISTENTRY, _BOUNTYLISTENTRY BALIGN16;

typedef struct TAG_BOUNTYLIST
{
	BOUNTYLISTENTRY		head;

	uint32				numEntries;
	uint32				pad[3];
} BOUNTYLIST, _BOUNTYLIST BALIGN16;

extern BOUNTYLIST bountyList;

extern int posterBounties[19];

/* --------------------------------------------------------------------------------
   Function : InitialisePlayerStatsAtStartOfGame
   Purpose : initialises stats when a new game is started
   Parameters : 
   Returns : 
   Info : 
*/

void InitialisePlayerStatsAtStartOfGame(struct TAG_PLAYERSTATS *stats);

/* --------------------------------------------------------------------------------
   Function : InitialisePlayerStatsAtStartOfLevel
   Purpose : initialises stats when a new game is started
   Parameters : 
   Returns : 
   Info : 
*/

void InitialisePlayerStatsAtStartOfLevel(void);

/* --------------------------------------------------------------------------------
   Function : AddMoneyToBounty
   Purpose : Add to the bounty on a players head
   Parameters : 
   Returns : 
   Info : 
*/
void AddMoneyToBounty(struct TAG_ACTORINSTANCE *actorInstance,struct TAG_ACTORINSTANCE *breakableInstance,uint32 amount);

/* --------------------------------------------------------------------------------
   Function : InitialiseCharacterResetPoints
   Purpose : initialises reset points for characters at the start of a scene
   Parameters : 
   Returns : 
   Info : 
*/

void InitialiseCharacterResetPoints(struct TAG_ACTORINSTANCE *actor, struct TAG_ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : InitialisePlayerStatsAtStartOfLevel
   Purpose : initialises stats when a new level is started
   Parameters : 
   Returns : 
   Info : 
*/

void InitialisePlayerStatsAtStartOfLevel(struct TAG_PLAYERSTATS *stats);

/* --------------------------------------------------------------------------------
   Function : AddWantedPosterToStats
   Purpose : 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int AddWantedPosterToStats(int number, struct TAG_ACTORINSTANCE *player, uint32 level);

/*  --------------------------------------------------------------------------------
	Function	: LevelCompleted
	Purpose		: Called when the player completes a level
	Parameters	: ACTORINSTANCE of player that triggered the final poster
	Returns		: 
	Info		: 
*/
void LevelCompleted(struct TAG_ACTORINSTANCE *player, int flags = 0);

/* --------------------------------------------------------------------------------
   Function : AddMailboxToStats
   Purpose : 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int AddMailboxToStats(struct TAG_ACTORINSTANCE *mailbox, struct TAG_ACTORINSTANCE *player, uint32 level);

/* --------------------------------------------------------------------------------
   Function : ZeroTazTopCounter
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/
void ZeroTazTopCounter(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : IncrementTazTopCounter
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void IncrementTazTopCounter(struct TAG_ACTORINSTANCE *actorInstance, uint32 level);

/* --------------------------------------------------------------------------------
   Function : CalculateTotalPauseStats
   Purpose : 
   Parameters : char *
   Returns : 
   Info : 
*/

uint32 CalculateTotalPauseStats(char stats[25][10]);

/* --------------------------------------------------------------------------------
   Function : CalculatePauseStatsForLevel
   Purpose : 
   Parameters : char *, uint32 level
   Returns : 
   Info : 
*/

uint32 CalculatePauseStatsForLevel(char stats[25][10], uint32 level);

/* --------------------------------------------------------------------------------
   Function : InitOverlay
   Purpose : 
   Parameters :
   Returns : 
   Info : 
*/

void InitOverlay(void);

/* --------------------------------------------------------------------------------
   Function : DrawOverlay
   Purpose : 
   Parameters :
   Returns : 
   Info : 
*/

void DrawOverlay(struct TAG_PLAYERSTATS *player);

/* --------------------------------------------------------------------------------
   Function : UpdateOverlay
   Purpose : 
   Parameters :
   Returns : 
   Info : 
*/

void UpdateOverlay(void);

/*  --------------------------------------------------------------------------------
	Function	: ClosePlayerStats
	Purpose		: Close any player stats
	Parameters	: PLAYER
	Returns		: 
	Info		: 
*/
void ClosePlayerStats(PLAYER *player);

/*  --------------------------------------------------------------------------------
	Function	: DeletePlayerStats
	Purpose		: Delete any player stats
	Parameters	: PLAYER
	Returns		: 
	Info		: 
*/
void DeletePlayerStats(struct TAG_PLAYER *player);

/*  --------------------------------------------------------------------------------
	Function	: InitialisePlayerStats
	Purpose		: Initialise the player stats and all overlays
	Parameters	: PLAYER*
	Returns		: 
	Info		: 
*/
void InitialisePlayerStats(struct TAG_PLAYER *player);

/*  --------------------------------------------------------------------------------
	Function	: DrawPlayerStats
	Purpose		: Draw the players stats and overlays
	Parameters	: PLAYER
	Returns		: 
	Info		: 
*/
void DrawPlayerStats(struct TAG_PLAYER *player);

/*  --------------------------------------------------------------------------------
	Function	: UpdatePlayerStats
	Purpose		: Update the players stats and overlays
	Parameters	: PLAYER
	Returns		: 
	Info		: 
*/
void UpdatePlayerStats(struct TAG_PLAYER *player);

/*  --------------------------------------------------------------------------------
	Function	: ActivateSwirlOverlay
	Purpose		: Active the pull down radar overlay
	Parameters	: PLAYER
	Returns		: 
	Info		: 
*/
void ActivateSwirlOverlay(struct TAG_PLAYER *player);

void ActivateDestructOverlay(struct TAG_PLAYER *player);

/*  --------------------------------------------------------------------------------
	Function	: ActivateMoneyOverlay
	Purpose		: Active the pull down radar overlay
	Parameters	: PLAYER
	Returns		: 
	Info		: 
*/
void ActivateMoneyOverlay(struct TAG_PLAYER *player);

/*  --------------------------------------------------------------------------------
	Function	: ActivatePosterDisplay
	Purpose		: Activate the poster status display
	Parameters	: PLAYER
	Returns		: 
	Info		: 
*/
void ActivatePosterDisplay(struct TAG_PLAYER *player);

/* --------------------------------------------------------------------------------
   Function : InitBountyList
   Purpose : guess what it does!
   Parameters : 
   Returns : 
   Info : 
*/

void InitBountyList(void);

/* --------------------------------------------------------------------------------
   Function : InitPosterBounties
   Purpose : sets the value of the posters on each level
   Parameters : 
   Returns : 
   Info : 
*/

void InitPosterBounties(void);

/* --------------------------------------------------------------------------------
   Function : UpdateBountyList
   Purpose : guess what it does!
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateBountyList(BOUNTYLIST *bountyList);

/* --------------------------------------------------------------------------------
   Function : UpdateBountyList
   Purpose : guess what it does!
   Parameters : 
   Returns : 
   Info : 
*/

void RemoveBountyFromList(BOUNTYLISTENTRY *remPtr);

/* --------------------------------------------------------------------------------
   Function : DrawBountyList
   Purpose : guess what it does!
   Parameters : 
   Returns : 
   Info : 
*/

int DrawBountyList(BOUNTYLIST *bountyList);

/* --------------------------------------------------------------------------------
   Function : MoneyOverlayCallBack
   Purpose : callback for when bounty is drawn
   Parameters : 
   Returns : 
   Info : 
*/

int MoneyOverlayCallBack(EBFontCallbackReason reason, TBFontCallbackInfo *info);

/* --------------------------------------------------------------------------------
   Function : MoneyOverlayCallBack
   Purpose : callback for when bounty is drawn
   Parameters : 
   Returns : 
   Info : 
*/

int PlaceActorAtSafePoint(struct TAG_PLAYER *player);

/* --------------------------------------------------------------------------------
   Function : CalculateGameTimeFormatted
   Purpose : calculates gametime in hours minutes and seconds
   Parameters : 
   Returns : 
   Info : 
*/

void CalculateGameTimeFormatted(float numSeconds, int *hours, int *minutes, int *seconds);

char processGalleryScore();

#endif