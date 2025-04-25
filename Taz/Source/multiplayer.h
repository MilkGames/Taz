// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : multiplayer.h
//   Purpose : multi-player stuff
// Component : Taz
//		Info : 
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_MULTIPLAYER
#define __H_MULTIPLAYER
// PP: ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯


#include "TextFX.h"			// PP: Text effects, eg. button glyphs, coloured text


#define MPMODE_NONE			(0)
#define MPMODE_NORMAL		(1)
#define MPMODE_RACE			(2)
#define MPMODE_DESTRUCT		(3)
#define MPMODE_TIMEATTACK	(4)
#define MPMODE_DINER		(5)
#define MPMODE_TOURNAMENT	(6)
#define MPMODE_BOSS			(7)
#define MPMODE_NUMBER		(8)

#define MPREASON_INITIALISE	(0)
#define MPREASON_PREP		(2)
#define MPREASON_POLL		(3)
#define MPREASON_DRAW		(4)
#define MPREASON_KILL		(5)

#define COUNTDOWN_SIZE		(6)

#define IN_MULTI_GAME		((map.sceneNumber==SCENE_MUSEUMVEHICLERACE)||(map.sceneNumber==SCENE_GRANDCVEHICLERACE)||(map.sceneNumber==SCENE_DEPTSTRVEHICLERACE)||(map.sceneNumber==SCENE_DESTRUCTIONAQUA)||(map.sceneNumber==SCENE_DESTRUCTIONCONSTRUCT)||(map.sceneNumber==SCENE_DESTRUCTIONTASMANIA)||(map.sceneNumber==SCENE_ICEDOMERACE)||(map.sceneNumber==SCENE_GOLDMINERACE)||(map.sceneNumber==SCENE_SAFARIRACE))

enum RACESTATE {Intro, Countdown, Race, TimeUp, Finished};

struct LEVELDATA
{
	int			gameType;
	float		timeLimit;
	float		startTime;
	float		timeCredit;				// CPW: 4L
	RACESTATE	RaceState;

	int			gameFinished;
	int			winner;
	int			needVehicle;

	float		oneChkTime, twoChkTime;
	BOOK		*oneBook, *twoBook;
};

extern LEVELDATA levelData;

extern struct TAG_ACTORINSTANCE *mpPlayer[];
extern int mpNumPlayers;
extern struct TAG_CAMERAINFO *mpDrawCam;
extern int mpSubMode;

extern BOOK		*raceBook;
extern float	actualTime;
extern int pCountDown[];
extern char *psCountDownSampleNames[];
extern char *psPositionNames[];

extern bool	mpTimerFlash;
extern BOOK	*pEndGameBook;
extern BOOK	*pEndMPGameStats;

int mpInit(int, int, struct TAG_ACTORINSTANCE *, struct TAG_ACTORINSTANCE *);
int mpPrep(void);
int mpPoll(void);
int mpDraw(struct TAG_CAMERAINFO *);
int mpKill(void);

void DoMultiplayerCharacterFall(ACTORINSTANCE *player);
void FinishedFadeFromMultiplayerFall(class TAG_FADE *fade);

void mpCountdownSetup(int);
void mpCountdownDraw(void);
int mpCountdownUpdate(void);
void mpCountdownKill(void);

int	mpDrawBook();

void InitMultiplayer(int);

void mpPageUpdateAndClose(PAGE * const pPage, const int32 context);
void mpPageUpdate(PAGE * const pPage, const int32 context);
void mpPageOpen(PAGE * const pPage, const int32 context);
void mpPageClose(PAGE * const pPage, const int32 context);

// CPW: move from here eventually
void raceAddCheckPoint(TBVector pos, TBQuaternion rot, int seq, float widdywidth);
void raceAddTarget(TBVector pos, TBQuaternion rot);

#define GAMEOVER_NULL			(0)
#define GAMEOVER_NOTFINISHED	(1)
#define GAMEOVER_COMPLETED		(2)
#define GAMEOVER_TIMEDOUT		(3)
#define GAMEOVER_BAILEDOUT		(4)

#define GAMETYPE_RACE			(0)
#define GAMETYPE_TIMEATTACK		(1)

void mpEndGameMenuSetUp(int *, int, const int32 winner);


/*	--------------------------------------------------------------------------------
	Function 	: mpEndGameMenuHandler
	Purpose 	: custom pause update function for the multiplayer end-of-level menus
	Parameters 	: context value, required but currently not used
	Returns 	: 
	Info 		: // PP: must match the PauseUpdateFunc typedef (main.h)
*/
void mpEndGameMenuHandler(const int32 context);


void mpEndGameMenuKill(void);

void InitEndMPGameStatsBook();

void mpEndBonusSetUp(int *, int, float result);
void mpEndBonusHandler(const int32 context);
void mpEndBonusKill(void);

void InsertRulebookChapterText(struct TAG_PAPERBOOK *Book, int sceneRules, int rulesHeading, char *title, char *picture, int extraRules = -1);
void InitRuleBook();
void StartRuleBook();
// PP: REMOUT: OODvoid DrawRuleBook();
void CloseRuleBook();
void KillRuleBook();


/*	--------------------------------------------------------------------------------
	Function 	: setNumPlayers
	Purpose 	: set the number of players
	Parameters 	: new number of players, (opt/true)allow players to choose their pads
	Returns 	: 
	Info 		: // PP: !!!!!!DON'T CALL THIS PER-FRAME!!!!!!
					// PP: prepares the pad-checking stuff for the new number of players
*/
void setNumPlayers(const uchar numPlayers, const bool choosePads=true);


// PP: __________________________
#endif// PP: ndef __H_MULTIPLAYER