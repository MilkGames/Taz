#ifndef __H_ENDGAMECREDITS
#define __H_ENDGAMECREDITS

#include "textfx.h"

// CMD: constants
#define CREDITS_FIREWORKTIME			(2.0f)
#define CREDITS_NAME_TIME				(3.0f)
#define CREDITS_LEGAL_TIME_MULTIPLIER	(4.0f)
#define CREDITS_STATS_TIME				(10.0f)
#define CREDITS_SCALE					(0.75f)
#define CREDITS_ICON_SCALE				(CREDITS_SCALE*0.75f)
#define CREDITS_IDLE_TIME				(10.0f)
#define CREDITS_NUM_COSTUMES			(5)

enum ECreditsSection
{
	CREDITS_BRUNO,
	CREDITS_INFOGRAMES_INT_INC,
	CREDITS_INFOGRAMES_INC,
	CREDITS_BLITZ,
	CREDITS_INFOGRAMES,
	CREDITS_WARNER,
	CREDITS_NIMROD,
	CREDITS_LEGAL,
};

enum EBookState
{
	CREDITS_STATSBOOK,
	CREDITS_BOOKOPEN,
	CREDITS_BOOKWAITING,
	CREDITS_BOOKCLOSED,
};

enum ETazCreditsState
{
	CREDITS_TAZ_INIT,
	CREDITS_TAZ_SPINUP,
	CREDITS_TAZ_MESMERISED,
	CREDITS_TAZ_EXITSTAGELEFT,
	CREDITS_TAZ_RAPPER,
	CREDITS_TAZ_BOARDER,
	CREDITS_TAZ_NINJA,
	CREDITS_TAZ_BANDITO,
	CREDITS_TAZ_WERETAZ,
	CREDITS_TAZ_BYEBYE,
};

enum ETotal
{
	TOTALS_BOUNTY,
	TOTALS_SANDWICHES,
	TOTALS_DESTRUCTIBLES,
	TOTALS_CAPTURED,
	TOTALS_TIME,
	TOTALS_STATUES,
	TOTALS_GALERIES,
	TOTALS_BONUSGAMES,
};

typedef struct TAG_CREDITSINFO
{
	// CMD: taz
	TBVector			initPos;
	TBVector			position;
	TBVector			target;
	TBVector			velocity;

	ETazCreditsState	tazState;
	ETazCreditsState	tazPrevState;	
	float				tazClock;
	uint32				currentCostume;

	// CMD: cartoon light
	float				theta, initTheta;
	float				phi, initPhi;

	// CMD: books
	BOOK				*statsBook;
	BOOK				*titleBook;
	BOOK				*namesSubBook;
	float				bookClock;
	ECreditsSection		currentSection;

	uint32				currentName;
	uint32				actualName;
	EBookState			bookState;

	// CMD: sound
	int					spinHandle;
	int32				iPad[3];
	
	// CMD: fireworks
	float				fireworkClock;

	// CMD: splashscreen
	SPLASHSCREEN		splash;
}CREDITSINFO, _CREDITESINFO BALIGN16;

extern CREDITSINFO		*creditsInfo;

/* --------------------------------------------------------------------------------
   Function : MainCredits
   Purpose : Main loop for credits sequence
   Parameters : 
   Returns : 
   Info : 
*/

void MainCredits(void);

/* --------------------------------------------------------------------------------
   Function : InitialiseEndGameCredits
   Purpose : Does any initialisation required to run the end game credits
   Parameters : 
   Returns : 
   Info : 
*/

void InitialiseEndGameCredits(void);

/* --------------------------------------------------------------------------------
   Function : CalculateStatsTotals
   Purpose : calculates all the totals required
   Parameters : 
   Returns : 
   Info : 
*/

int	CalculateStatsTotals(ETotal type, int index=0);

/* --------------------------------------------------------------------------------
   Function : TimeIntoHoursMinsAndSecs
   Purpose : converts a time in seconds to a time in hours minutes and seconds, and prints it into a string
   Parameters : ptr to stringBuff into which the result will be printed, time in seconds
   Returns : 
   Info : 
*/

void TimeIntoHoursMinsAndSecs(char *stringBuf, float time);

/* --------------------------------------------------------------------------------
   Function : CreateEndCreditsBooks
   Purpose : does what it says ... note: no tins involved
   Parameters : 
   Returns : 
   Info : 
*/

void CreateEndCreditsBooks(void);

/* --------------------------------------------------------------------------------
   Function : FreeEndGameCreditsAndChangeScene
   Purpose : does what it says ... note: no tins involved
   Parameters : 
   Returns : 
   Info : 
*/

void FreeEndGameCredits(void);

/* --------------------------------------------------------------------------------
   Function : PrepEndGameCredits
   Purpose : preps everything needed for the credits
   Parameters : 
   Returns : 
   Info : 
*/

void PrepEndGameCredits(void);

/* --------------------------------------------------------------------------------
   Function : UpdateEndGameCredits
   Purpose : Does any update required
   Parameters : 
   Returns : TRUE if sequence completed
   Info : 
*/

int UpdateEndGameCredits(void);

/* --------------------------------------------------------------------------------
   Function : UpdateTazInCredits
   Purpose : Does all taz related update
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateTazInCredits(void);

/* --------------------------------------------------------------------------------
   Function : UpdateCartoonLightDirection
   Purpose : updates the direction of the cartoon light, dependant on user input
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateCartoonLightDirection(PADCONTROLSTATUS *controller);

/* --------------------------------------------------------------------------------
   Function : DrawEndGameCredits
   Purpose : Does any drawing required
   Parameters : 
   Returns : 
   Info : 
*/

void DrawEndGameCredits(void);

/* --------------------------------------------------------------------------------
   Function : DrawEndGameCredits
   Purpose : Does any drawing required
   Parameters : 
   Returns : 
   Info : 
*/

void DrawEndGameCredits2(void);

#endif