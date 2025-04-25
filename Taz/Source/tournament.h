#ifndef __H_TOURNAMENT
#define __H_TOURNAMENT

#define	TB_HANDLE_PULL	(0)
#define	TB_HOLD_ONE		(1)
#define	TB_HOLD_TWO		(2)
#define	TB_HOLD_THREE	(3)
#define	TB_COLLECT		(4)

#define	DEAD	(0)
#define	CREATE	(1)
#define	NORMAL	(2)
#define	DESTROY	(3)

//Spinner Class

/* PP: **** I RENAMED THIS CLASS FROM 'SPINNER' TO 'GAMBLESPINNER' ****

	I'm sorry about this.  By the time I realised that this 'SPINNER' class existed,
	I'd already added Spinner.cpp & Spinner.h, which define an unrelated struct also called SPINNER.
	Renaming this one seemed like the easiest solution, since renaming files that are already
	in the projects.
*/

//Declaration

class GAMBLESPINNER
{
private:
	
	ACTORINSTANCE *SpinnerInstance;
	
	float TargetRot;
	float CurrentRot;

	TBQuaternion origQuat;

	bool Holding;
	bool Spinning;

	bool FirstSpin;

public:

	GAMBLESPINNER();

	~GAMBLESPINNER();
		
	void InitSpinner();
		
	void UpdateSpinner();

	void SetActor(ACTORINSTANCE *actor);

	void SetPos(TBVector Pos);

	void SetTarget(float Target);

	bool SetHold();

	void SetFirstSpin(bool SetValue);

	ACTORINSTANCE *GetActor();

	bool GetSpin();

	bool GetHold();

	bool GetFirstSpin();	
};

void InitTournament();
void ProcessTournamentButton(int);
void UpdateTournamentPage();
void KillTournamentPage();
void CleanUpTournamentPage();
void SetSlotState(int SlotState);

void getTourneyRandom(char styleorder[], char Exclusive);
void ProcessNextRound();
void showTournamentWinner(char winner);

void mpEndRoundMenuSetUp(int *, int, const int32 winner);// PP: changed this to take a winner index (0/1/-1=draw) rather than a hard-coded un-localised byte string!
void mpEndRoundMenuHandler(const int32 context);
void mpEndRoundMenuKill(void);

char * GetTournamentSplashScreen();
	
int GetGameNumber(int gameOrderNumber);

extern GAMBLESPINNER	*Spinner;

void DoTournamentWin(FADE *fade);
void UpdateTournamentWin();
void DrawTournamentWin();
void EndTournamentWin(FADE *fade);

#endif