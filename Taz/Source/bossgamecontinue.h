#ifndef __H_BOSSGAMECONTINUE
#define __H_BOSSGAMECONTINUE

#include "popup.h"
#include "fade.h"

enum EBossGameResetChoice
{
	BOSSGAMERESET_NONE,
	BOSSGAMERESET_CONTINUE,
	BOSSGAMERESET_QUIT,
};

enum EFieldOfViewState
{
	FOV_STATIC,
	FOV_DECREASE,
	FOV_INCREASE,
};

typedef int32 (*BossGameResetCallback)(void *context, EBossGameResetChoice choice);
typedef void (*BossGameFadeDownCallback)(void);

typedef struct TAG_BOSSGAMECONTINUEINFO
{
	BossGameResetCallback		callback;
	BossGameFadeDownCallback	fadeDownCallback;
	void					*context;
	EBossGameResetChoice	choice;

	EFieldOfViewState		fovState;
	float					fov;					// CMD: in radians
	float					fovInit;				// CMD: guess
	float					fovTarget;				// CMD: in radians
	float					fovRateOfChange;		// CMD: in radians per second

	ACTORINSTANCE			*sharkPool;
	ACTORINSTANCE			*sheDevil;
	ACTORINSTANCE			*sharks[3];
	POPUP					*popUp;
}BOSSGAMECONTINUEINFO, _BOSSGAMECONTINUEINFO;

extern BOSSGAMECONTINUEINFO	*bossGameContinueInfo;

/* --------------------------------------------------------------------------------
   Function : LoadBossGameContinue
   Purpose : Loads all the obe's needed for the bossgame continue screen
   Parameters : 
   Returns : TRUE if all obe's loaded succesfully
   Info :
*/

int LoadBossGameContinue(void);

/* --------------------------------------------------------------------------------
   Function : ProcessTazDeathAtHandsOfMechatweety
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

void ProcessTazDeathAtEndOfBossGame(BossGameResetCallback callback, void *context, TBVector lookAt=NULL, BossGameFadeDownCallback fadeDownCallback=NULL);

/* --------------------------------------------------------------------------------
   Function : FinishedFadeFromDeathInBlimp
   Purpose : 
   Parameters : FADE *
   Returns : 
   Info : 
*/

void InitialiseBossGameContinue(void);

/* --------------------------------------------------------------------------------
   Function : FinishedFadeFromDeathInBlimp
   Purpose : 
   Parameters : FADE *
   Returns : 
   Info : 
*/

void UpdateBossGameContinue(void);

/* --------------------------------------------------------------------------------
   Function : FinishedFadeFromDeathInBlimp
   Purpose : 
   Parameters : FADE *
   Returns : 
   Info : 
*/

void FreeBossGameContinue(void);

/* --------------------------------------------------------------------------------
   Function : FinishedFadeFromDeathInBlimp
   Purpose : 
   Parameters : FADE *
   Returns : 
   Info : 
*/

void FinishedFadeFromDeathInBoss(FADE *fade);

/* --------------------------------------------------------------------------------
   Function : FinishedFadeFromDeathInBlimp
   Purpose : 
   Parameters : FADE *
   Returns : 
   Info : 
*/

void OpenContinueBookCallback(FADE *fade);

/* --------------------------------------------------------------------------------
   Function : FinishedFadeFromDeathInBlimp
   Purpose : 
   Parameters : FADE *
   Returns : 
   Info : 
*/

void MadeContinueChoiceCallback(FADE *fade);

/* --------------------------------------------------------------------------------
   Function : MainBossContinue
   Purpose : Main loop for boss continue screen
   Parameters : 
   Returns : 
   Info : 
*/

void MainBossContinue(void);

/* --------------------------------------------------------------------------------
   Function : MainBossContinueUpdate
   Purpose : Main update for boss continue screen
   Parameters : 
   Returns : 
   Info : 
*/

void MainBossContinueUpdate(void);

/* --------------------------------------------------------------------------------
   Function : MainBossContinuePrep
   Purpose : Main prep for boss continue screen
   Parameters : 
   Returns : 
   Info : 
*/

void MainBossContinuePrep(void);

/* --------------------------------------------------------------------------------
   Function : MainBossContinueDraw
   Purpose : Main draw for boss continue screen
   Parameters : 
   Returns : 
   Info : 
*/

void MainBossContinueDraw(void);

/* --------------------------------------------------------------------------------
   Function : MainBossContinueDraw2
   Purpose : Main draw for boss continue screen
   Parameters : 
   Returns : 
   Info : 
*/

void MainBossContinueDraw2(void);

#endif