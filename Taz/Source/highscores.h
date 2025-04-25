#ifndef __H_HIGHSCORES
#define __H_HIGHSCORES

#include "text.h"
#include "scene.h"

#define		NUM_HSE_ICONS		(15)
#define		HSE_DEFAULT_ASCII	(45)
#define		HSE_ICONSCALE		(0.5f)
#define		HSE_TEXTSCALE		(0.75f)
#define HSE_DECREMENT			(1<<0)
#define HSE_INCREMENT			(1<<1)


enum ELevelScores
{
	SCORES_ICEDOME,
	SCORES_SAFARI,
	SCORES_AQUA,
	SCORES_ZOOBOSS,
	
	SCORES_DEPTSTR,
	SCORES_MUSEUM,
	SCORES_CONSTRUCT,
	SCORES_CITYBOSS,
	
	SCORES_GHOSTTOWN,
	SCORES_GOLDMINE,
	SCORES_GRANDCANYON,
	SCORES_WESTBOSS,
	
	SCORES_TAZHUB,
	SCORES_TAZPREBOSS,
	SCORES_TAZBOSS,

	SCORES_TOTALNUM,
	SCORES_INVALIDSCENE,
}; 

enum 
{
	HSE_CHOOSEICON,
	HSE_LETTERONE,
	HSE_LETTERTWO,
	HSE_LETTERTHREE,
	HSE_CONFIRM,
	HSE_SENDENTRY,
	HSE_DROPOUT,
	HSE_EXIT,
};

typedef struct TAG_HIGHSCORES
{
	char			names[5][4];
	char			iconRef[5];
	float			time[5];
	float			exactRanking[5];
	EStringIndex	rank[5];
	uint32			money[5];
	bool			secretItemCollected[5];		// NH: Had the secret item been collected
	bool			destructBonusCollected[5];	// NH: Had the destruct bonus been won?	
}HIGHSCORES, _HIGHSCORES;

extern char *highScoreIconNames[];
//extern HIGHSCORES highScores[SCORES_TOTALNUM];

/*	--------------------------------------------------------------------------------
	Function 	: InitialiseHighScores
	Purpose 	: Temp function to initialise high score table in short term
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void InitialiseHighScores(void);

/*	--------------------------------------------------------------------------------
	Function 	: CalculateRankAtEndOfLevel
	Purpose 	: calculates rank of player based on level stats
	Parameters 	: playerstats structure
	Returns 	: index into string table
	Info 		:
*/

float CalculateRankAtEndOfLevel(struct TAG_PLAYERSTATS *playerStats);

/*	--------------------------------------------------------------------------------
	Function 	: CalculateRankAtEndOfLevel
	Purpose 	: calculates rank of player based on level stats
	Parameters 	: playerstats structure
	Returns 	: index into string table
	Info 		:
*/

EStringIndex CalculateRankFromFloat(float exactRank);

/* --------------------------------------------------------------------------------
   Function : ConvertSceneRefToLevelScoreRef
   Purpose : draws end of level stats for single player game
   Parameters : 
   Returns : 
   Info : 
*/

ELevelScores ConvertSceneRefToLevelScoreRef(EScene currentScene);

/* --------------------------------------------------------------------------------
   Function : CheckRankAgainstCurrentTopTen
   Purpose : draws end of level stats for single player game
   Parameters : 
   Returns : 
   Info : 
*/

int CheckRankAgainstCurrentTopTen(struct TAG_PLAYERSTATS *stats, EScene currentScene);

/* --------------------------------------------------------------------------------
   Function : IncrementDecrementScoreCharacter
   Purpose : draws end of level stats for single player game
   Parameters : 
   Returns : 
   Info : 
*/

void AddNewRecordToStats(struct TAG_PLAYERSTATS *stats, EScene	currentScene);

#endif