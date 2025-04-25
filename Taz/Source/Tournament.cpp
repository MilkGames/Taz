// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Blitz Games Ltd.
//
//      File : tournament.cpp
//   Purpose : multiplayer tournament routines
// Component : Taz
//    Author : John Weeks
//      Date : 12th September 2001
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "main.h"

#include "display.h"
#include "maths.h"
#include "actors.h"
#include "status.h"
#include "physics.h"
#include "camera.h"
#include "characters.h"
#include "control.h"
#include "trail.h"
#include "cutscene.h"
#include "scene.h"
#include "vehicles.h"
#include "splashscreen.h"
#include "sfx.h"
#include "text2.h"
#include "textfx.h"
#include "fireworks.h"
#include "timer.h"
#include "countdown.h"
#include "util.h"
#include "frontEndMain.h"
#include "animation.h"

#include "multiplayer.h"
#include "tournament.h"

char styleOrder[3], gameOrder[3];
bool Hold[3];
char Score1, Score2;

int	nextRound, state;

// PP: the zero-based index (0/1/2) of the tournament stage that we're currently on (if gameStatus.multiplayer.tournament is also true)
char	g_tournamentStage= -1;

//Spinner Class

/* PP: **** I RENAMED THIS CLASS FROM 'SPINNER' TO 'GAMBLESPINNER' ****

	I'm sorry about this.  By the time I realised that this 'SPINNER' class existed,
	I'd already added Spinner.cpp & Spinner.h, which define an unrelated struct also called SPINNER.
	Renaming this one seemed like the easiest solution, since renaming files that are already
	in the projects.
*/

//Declaration
/*
class GAMBLESPINNER
{
private:
	
	ACTORINSTANCE *SpinnerInstance;
	
	float TargetRot;
	float CurrentRot;

	TBQuaternion origQuat;

	char Holding;
	char Spinning;

public:

	GAMBLESPINNER();

	~GAMBLESPINNER();
		
	void InitSpinner();
		
	void UpdateSpinner();

	void SetActor(ACTORINSTANCE *actor);

	void SetPos(TBVector Pos);

	void SetTarget(float Target);

	char SetHold();

	ACTORINSTANCE *GetActor();

	float GetSpin();
};
*/
//Define

GAMBLESPINNER::GAMBLESPINNER()
{
	SpinnerInstance = 0;

	//SpinnerInstance->alpha = 0;

	Spinning = false;
	Holding = false;
	
	TargetRot = 0;
	CurrentRot = 0;

	SetFirstSpin(true);
}

GAMBLESPINNER::~GAMBLESPINNER()
{
	if (SpinnerInstance) 
		FreeActorInstance(SpinnerInstance);
}

void GAMBLESPINNER::UpdateSpinner()
{
	int i;

	float	RotDiff;

	TBQuaternion	quat;

	switch(state)
	{
	case DEAD:		break;
	
	case CREATE:	Holding = false;
					SetFirstSpin(true);//SpinnerInstance->alpha += 5;
					break;

	case NORMAL:	RotDiff = TargetRot - CurrentRot;
		
					if (RotDiff > 0.25f && !Holding)
					{
						if (RotDiff > 180.0f)
							CurrentRot += 20.0f;						// JW: Spin fast
						else
							CurrentRot += (RotDiff/360.0f) * 30.0f;		// JW: slow down

						bmQuatXRotation(quat, (CurrentRot/360.0f) * TWOPI);

						// NH: bmQuatCopy(SpinnerInstance->actorInstance.orientation, quat);

						bmQuatMultiply(SpinnerInstance->actorInstance.orientation, quat, origQuat);

						Spinning = 1;

						//SpinnerInstance->alpha = 128;
					}
					else
					{
						CurrentRot = TargetRot;
						SetFirstSpin(false);
						Spinning = false;		

						//SpinnerInstance->alpha = 255;
					}

					break;

	case DESTROY:	//SpinnerInstance->alpha -= 10;
					break;

	return;
	}
}

void GAMBLESPINNER::SetActor(ACTORINSTANCE *actor)
{
	if (!SpinnerInstance || !actor)
	{
		SpinnerInstance = actor;

		if (actor)
			bmQuatCopy(origQuat, actor->actorInstance.orientation);
	}
}

void GAMBLESPINNER::SetPos(TBVector Pos)
{
	SpinnerInstance->actorInstance.position[X] = Pos[X];
	SpinnerInstance->actorInstance.position[Y] = Pos[Y];
	SpinnerInstance->actorInstance.position[Z] = Pos[Z];
}

bool GAMBLESPINNER::SetHold()
{
	if (Spinning)
		return Holding;

	if (Holding)
	{
		Holding = 0;
		
		//SpinnerInstance->alpha = 255;
	}
	else
	{
		Holding = 1;
		
		//SpinnerInstance->alpha = 64;
	}

	return Holding;
}

void GAMBLESPINNER::SetTarget(float Target)
{
	if (Holding)
		return;

	TargetRot = Target;
	CurrentRot = 0;
}

void GAMBLESPINNER::SetFirstSpin(bool SetValue)
{
	FirstSpin = SetValue;
}

ACTORINSTANCE * GAMBLESPINNER::GetActor()
{
	return SpinnerInstance;
}

bool GAMBLESPINNER::GetFirstSpin()
{
	return FirstSpin;
}

bool GAMBLESPINNER::GetSpin()
{
	return Spinning;
}

bool GAMBLESPINNER::GetHold()
{
	return Holding;
}

//Tournament Page Functions

GAMBLESPINNER	*Spinner;

ACTORINSTANCE	*SlotMachine = 0;

void getTourneyRandom(char styleorder[], char Exclusive)
{
	int	i;

	char again = false;
	
	if (!Hold[0])
		styleorder[0] = -1;
	if (!Hold[1])
		styleorder[1] = -1;
	if (!Hold[2])
		styleorder[2] = -1;

	for (i = 0; i < 3; i++)
	{
		if (!Hold[i])
		{
			do 
			{
				styleorder[i] = (int) UtilGenerateRandomNumber(2, 5);
	
				if (Exclusive)
				{
					again = true;

					if (i == 0)
					{
						if (styleorder[i] != styleorder[1] && styleorder[i] != styleorder[2])
							again = false;
					}
					else if (i == 1)
					{
						if (styleorder[i] != styleorder[0] && styleorder[i] != styleorder[2])
							again = false;
					}
					else
					{
						if (styleorder[i] != styleorder[0] && styleorder[i] != styleorder[1])
							again = false;
					}
				}
			}
			while(again);
		}
	}

	//0 - Vehicle Race
	//1 - Time Attack
	//2 - Destruction
}

void ProcessNextRound()
{
	gameStatus.multiplayer.versusCPU = false;
	gameStatus.multiplayer.Tournament = true;

	// PP: update the tournament stage index so's we know what round we're on
	g_tournamentStage++;

	// PP: ==================================================================
	// PP: I was getting a crash trying to 'collect' the tournament,
	// PP: because gameStatus.multiplayer.numPlayers was never getting set to 2.
	// PP: Please let me know where this is meant to happen!
	setNumPlayers(2, g_tournamentStage==0);// PP: players can only choose their pads before stage 1
	// PP: ==================================================================

	switch(styleOrder[nextRound])
	{
	case MPMODE_RACE:		switch (gameOrder[nextRound])
							{
							case 2:	bkGenerateEvent("scenechange", "_vrmuseum");		break;	//num 9
							case 3: bkGenerateEvent("scenechange", "_vrgrandc");		break;	//num 1
							case 4: bkGenerateEvent("scenechange", "_vrdeptstr");	break;	//num 2
							}
							break;

	case MPMODE_DESTRUCT:	switch (gameOrder[nextRound])
							{
							case 2:	bkGenerateEvent("scenechange", "_deaqua");		break;	//num 3
							case 3: bkGenerateEvent("scenechange", "_deconstruct");	break;	//num 4
							case 4: bkGenerateEvent("scenechange", "_detasmania");	break;	//num 5
							}
							break;
	
	case MPMODE_TIMEATTACK:	switch (gameOrder[nextRound])
							{
							case 2:	bkGenerateEvent("scenechange", "_rcicedome");	break;	//num 6
							case 3: bkGenerateEvent("scenechange", "_rcgoldmine");	break;	//num 7
							case 4: bkGenerateEvent("scenechange", "_rcsafari");		break;	//num 8
							}
							break;
	}
}

#define ENDROUND_NUM_CATEGORIES (2)

int		currTrnCatergory = 0;
BOOK	*pEndRoundBook = NULL;
TEXTBOX	*pEndRoundTextBox[2];
int		*pTrnMode;
int		retTrnMode = 0;

static float	allowedToProceed;

bool	trnChoiceMade = false;

#define TB_NEXTROUND	(0)
#define TB_QUIT			(1)


void mpEndRoundMenuSetUp(int *pCurrentMode, int returnMode, const int32 winner)// PP: changed this to take a winner index (0/1/-1=draw) rather than a hard-coded un-localised byte string!
{	
	PauseRumble(&controller1);
	PauseRumble(&controller2);

	FlushRumbleQueue(&controller1);
	FlushRumbleQueue(&controller2);

	UpdateRumble(&controller1);
	UpdateRumble(&controller2);

	if (gameStatus.player1->actorStatus->multiplayer.position == 0)
		Score1++;
	else if (gameStatus.player2->actorStatus->multiplayer.position == 0)
		Score2++;
	
	COLOUR		statsBookColour=COLOUR(128, 128, 128);

	pTrnMode = pCurrentMode;

	pEndRoundBook = new BOOK;

	pEndRoundBook->setScreen(winner);

	// PP: Set the size of the book
	//pEndRoundBook->setBookRect(RECTANGLE(-0.4f, 0.4f, -0.2f, 0.2f));// PP: REMOUT: must now be expressed as fractions of the screen dimensions		RECTANGLE(-300, 300, -220, 220));

	// PP: 'result' textbox ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// PP: Create and add the textbox

	PAGE *pPage;
	TEXTBOX	*pTextBox;

	pPage = pEndRoundBook->addPage(pTextBox = new TEXTBOX());

	pPage->setController(gameStatus.player1->actorStatus->multiplayer.position);
	pPage->setNavigation(true);
	pPage->setNavigationWrap(PAGEFLAG_NAVWRAP_YDEBOUNCE);

	// PP: Set the font size
	pTextBox->setScale(1.2f);

	// PP: Set the font colour and specify no vertical gradient
	pTextBox->setVGrad(false);
	pTextBox->setColour(statsBookColour);

	// PP: Align to top-centre
	pTextBox->setXAlign(PIFLAG_XALIGN_CENTRE);
	pTextBox->setYAlign(PIFLAG_YALIGN_TOP);

	// PP: REMOUT: WILSON!!!!!!!	pTextBox->sprintf8("%s", sResult);

	if(winner == -1)
	{
		// PP: declare draw
		pTextBox->setText(STR_MP_DRAW);
	}
	else
	{
		// PP: state winner
		pTextBox->setText(STR_MP_WINNER, winner+1);
	}

	pTextBox->setSelectable(false);

	if (Score1 == 2 || Score2 == 2)
	{
		// PP: 'Next Round' textbox ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// PP: Create and add the textbox
		pPage->insertItem(pEndRoundTextBox[TB_NEXTROUND] = new TEXTBOX(STR_CONTINUE));
		pTextBox = pEndRoundTextBox[TB_NEXTROUND];

		// PP: Set the font size
		pTextBox->setFontSize(2.0f);

		// PP: Set the font colour and specify no vertical gradient
		pTextBox->setVGrad(false);
		pTextBox->setColour(statsBookColour);

		// PP: Align to bottom-left
		pTextBox->setXAlign(PIFLAG_XALIGN_CENTRE);
		pTextBox->setYAlign(PIFLAG_YALIGN_CENTRE);
	}
	else
	{
		// PP: 'Next Round' textbox ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// PP: Create and add the textbox
		pPage->insertItem(pEndRoundTextBox[TB_NEXTROUND] = new TEXTBOX(STR_NEXT_ROUND));
		pTextBox = pEndRoundTextBox[TB_NEXTROUND];

		// PP: Set the font size
		pTextBox->setFontSize(2.0f);

		// PP: Set the font colour and specify no vertical gradient
		pTextBox->setVGrad(false);
		pTextBox->setColour(statsBookColour);

		// PP: Align to bottom-left
		//pTextBox->setXAlign(PIFLAG_XALIGN_CENTRE);
		pTextBox->setYAlign(PIFLAG_YALIGN_CENTRE);

		// PP: 'Quit' textbox ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// PP: Create and add the textbox
		pPage->insertItem(pEndRoundTextBox[TB_QUIT] = new TEXTBOX(STR_EXIT_EXITGAME));
		pTextBox = pEndRoundTextBox[TB_QUIT];

		// PP: Set the font size
		pTextBox->setFontSize(2.0f);

		// PP: Set the font colour and specify no vertical gradient
		pTextBox->setVGrad(false);
		pTextBox->setColour(statsBookColour);

		// PP: Align to bottom-left
		//pTextBox->setXAlign(PIFLAG_XALIGN_CENTRE);
		//pTextBox->setYAlign(PIFLAG_YALIGN_BOTTOM);

		pTextBox->setYAlign(PIFLAG_YALIGN_CENTRE);
	}

	pPage->insertItem(pTextBox = new TEXTBOX(STR_PRESS_X_TO_CONTINUE));

	pTextBox->setSelectable(false);

	// PP: Set the font colour and specify no vertical gradient
	pTextBox->setVGrad(false);
	pTextBox->setColour(statsBookColour);

	// PP: Align to bottom-left
	//pTextBox->setXAlign(PIFLAG_XALIGN_CENTRE);
	//pTextBox->setYAlign(PIFLAG_YALIGN_BOTTOM);

	pTextBox->setYAlign(PIFLAG_YALIGN_BOTTOM);

	pPage->selectItem(pEndRoundTextBox[TB_NEXTROUND]);

	pEndRoundBook->open();
	
	InitEndMPGameStatsBook();

	trnChoiceMade = false;

	allowedToProceed = gameStatus.gameTime + 2.0f;
}

/*	--------------------------------------------------------------------------------
	Function 	: mpEndRoundMenuHandler
	Purpose 	: custom pause update function for the multiplayer end-of-level menus
	Parameters 	: context value, required but currently not used
	Returns 	: 
	Info 		: // PP: must match the PauseUpdateFunc typedef (main.h)
*/
void mpEndRoundMenuHandler(const int32 context)
{
	if (!pEndRoundBook)
		return;

	PADCONTROLSTATUS	controller;

	if (!gameStatus.player1->actorStatus->multiplayer.position)
	{// JW: player 1 won
		controller = controller1;
	}
	else
	{// JW: player 2 won
		controller = controller2;
	}
	
	// PP: player input follows....
// CPW: 	if(page->acceptInput)
	{
		if (Score1 == 2 || Score2 == 2)
		{
			if((Score2 == 2 ? OKBUTTON_PADNUM(2) : OKBUTTON_PADNUM(1)) && (allowedToProceed < gameStatus.gameTime))
			{
				// CPW: item has been selected
				// CPW: SetGameState(&gameStatus, GAMESTATE_INGAME, GAMESUBSTATE_NORMAL);
				
				// PP: Force-debounce ok button(s)
				FORCE_OKBUTTON_DEBOUNCE;

				mpEndRoundMenuKill();
				*pTrnMode = retTrnMode;
				//SetGameState(&gameStatus, GAMESTATE_INGAME, GAMESUBSTATE_NORMAL);
				
				if (Score1 == 2)
					showTournamentWinner(0);
				else
					showTournamentWinner(1);

				return;
			}
		}
		else
		{
			if (trnChoiceMade == false)
			{		
				if ((gameStatus.player1->actorStatus->multiplayer.position ? OKBUTTON_PADNUM(2) : OKBUTTON_PADNUM(1))  && (allowedToProceed < gameStatus.gameTime))
				{
					// CPW: item has been selected
					// CPW: SetGameState(&gameStatus, GAMESTATE_INGAME, GAMESUBSTATE_NORMAL);
					
					// PP: Force-debounce ok button(s)
					FORCE_OKBUTTON_DEBOUNCE;
					
					if (!gameStatus.player1->actorStatus->multiplayer.position)
						camera[0].fade.StartFadeDown(FADETYPE_NORMAL, 1.0f);
					else
						camera[1].fade.StartFadeDown(FADETYPE_NORMAL, 1.0f);
					
					trnChoiceMade = true;
				}
			}
			else
			{
				if ((!gameStatus.player1->actorStatus->multiplayer.position && gameStatus.player[0].camera->fade.GetFadeValue() == 127.0f) ||
					(!gameStatus.player2->actorStatus->multiplayer.position && gameStatus.player[1].camera->fade.GetFadeValue() == 127.0f))
				{
					mpEndRoundMenuKill();

					SetGameState(&gameStatus, GAMESTATE_INGAME, GAMESUBSTATE_NORMAL);
					
					if(currTrnCatergory == TB_NEXTROUND)
						ProcessNextRound();
					else
					{	//Quit to main menu
						ChangeScene(&gameStatus, SCENE_FRONTEND);
						
						gameStatus.multiplayer.numPlayers = 1;
					}
					
					return;
				}
			}
			
			/*if (gameStatus.player1->actorStatus->multiplayer.position ? OKBUTTON_PADNUM(2) : OKBUTTON_PADNUM(1))
			{
				// CPW: item has been selected
				// CPW: SetGameState(&gameStatus, GAMESTATE_INGAME, GAMESUBSTATE_NORMAL);
				
				// PP: Force-debounce ok button(s)
				FORCE_OKBUTTON_DEBOUNCE;

				mpEndRoundMenuKill();
				*pTrnMode = retTrnMode;
				SetGameState(&gameStatus, GAMESTATE_INGAME, GAMESUBSTATE_NORMAL);

				if(currTrnCatergory == TB_NEXTROUND)
				{
					gameStatus.player1->actorStatus->multiplayer.position = -1;
					gameStatus.player2->actorStatus->multiplayer.position = -1;

					// JW: Go to next game on list
					ProcessNextRound();
				}
				else
				{
					ChangeScene(&gameStatus, SCENE_FRONTEND);
					
					// JW: gameStatus.multiplayer.versusCPU = true;

					gameStatus.multiplayer.numPlayers = 1;
					//setNumScreens(1);
				}

				return;
			}*/
		}
		
		for(int i=0; i<ENDROUND_NUM_CATEGORIES; i++)
		{
			if (pEndRoundTextBox[i]->selected)
			{
				currTrnCatergory = i;
				
				pEndRoundTextBox[i]->setScale(1.25f);
			}
			else
				pEndRoundTextBox[i]->setScale(0.6f);
		}
	}

	// CPW: now draw 
/* PP: REMOUT: Now handled by the Book List			pEndRoundBook->update();
	pEndMPGameStats->update();
	*/
	// PP: set an identity object matrix
	bdSetIdentityObjectMatrix();
		
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);

	// PP: use a 2d world matrix on ps2
	#if BPLATFORM == PS2
		bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
	#endif

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);

/* PP: REMOUT: Now handled by the Book List					
	if (!gameStatus.player[gameStatus.multiplayer.currentScreen].actorInstance->actorStatus->multiplayer.position)
		pEndRoundBook->draw();	// JW: Draw the end mp game menu in the winners window
	else
		pEndMPGameStats->draw();
*/
	//pEndRoundBook->draw();
	// CPW: pEndRoundBook->drawDebug();


	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);

}

void mpEndRoundMenuKill(void)
{
	pEndRoundBook->close();
	delete(pEndRoundBook);
 	pEndRoundBook = NULL;

	pEndMPGameStats->close();
	delete(pEndMPGameStats);
 	pEndMPGameStats = NULL;

	gameStatus.multiplayer.Tournament = false;

	nextRound++;
}

void showTournamentWinner(char winner)
{
	camera[winner].fade.StartFadeDown(FADETYPE_NORMAL, 1.0f);
	camera[winner].fade.SetFadeDownFinishedCallback(DoTournamentWin);

	switch (winner)
	{
	case 0:	LoadSplashScreen(&gameStatus.splashScreen, "tazwins.bmp", "splshscr");	break;
	case 1:	LoadSplashScreen(&gameStatus.splashScreen, "shewins.bmp", "splshscr");	break;
	}
	
	//bkGenerateEvent("scenechange","_frontend");

	// JW: gameStatus.multiplayer.versusCPU = true;

	// JW: gameStatus.multiplayer.numPlayers = 1;
	// JW: setNumScreens(1);

	return;
}

void SetSlotState(int SlotState)
{
	state = SlotState;

	Spinner[0].SetFirstSpin(true);
	Spinner[1].SetFirstSpin(true);
	Spinner[2].SetFirstSpin(true);
}

void InitTournament()
{	
	TBVector Pos;

	Score1 = Score2 = 0;
	nextRound = 0;

	Hold[0] = Hold[1] = Hold[2] = 0;

	if (!SlotMachine)
	{
		SlotMachine = FindActorInstanceInMap("machine01", 0, &map);

		RemoveInstanceFromInstanceList(SlotMachine,&map.levelInstances);
		AddInstanceToInstanceList(SlotMachine,&map.animatedInstances);
	}

//	LoadActor("extras\\slotmachinespinner.obe", map.mapName);

	Spinner = new GAMBLESPINNER[3];

	Spinner[0].SetActor(FindActorInstanceInMap("reel_a01", 0, &map));
	Spinner[1].SetActor(FindActorInstanceInMap("reel_b01", 0, &map));
	Spinner[2].SetActor(FindActorInstanceInMap("reel_c01", 0, &map));

/*	Pos[X] = 2982.81f;
	Pos[Y] = 2682.21f;
	Pos[Z] = 1509.96f;
		
	Spinner[0].SetPos(Pos);
	
	Pos[X] = 3042.81f;
	Pos[Y] = 2682.21f;
	Pos[Z] = 1509.96f;
		
	Spinner[1].SetPos(Pos);
	
	Pos[X] = 3102.81f;
	Pos[Y] = 2682.21f;
	Pos[Z] = 1509.96f;
	
	Spinner[2].SetPos(Pos);*/

	//state = CREATE;

//	ProcessNextRound();
}

void ProcessTournamentButton(int choice)
{	
	if (state != NORMAL)
		return;

	switch(choice)
	{
	case TB_HANDLE_PULL:	
							//if (!Spinner[0].GetSpin()&&!Spinner[1].GetSpin()&&!Spinner[2].GetSpin())
							//{
								float	target;

								FlushAnimationQueue(&SlotMachine->actorInstance);

								PlayAnimationByName(SlotMachine, "handle_down", 1, 0, 0, 0.2f, NONE);
								PlayAnimationByName(SlotMachine, "handle_up", 1, 0, 1, 0.2f, NONE);

								getTourneyRandom(styleOrder, true);
								getTourneyRandom(gameOrder, false);

								target = (gameOrder[0] - 2)* 3.0f;
								target += (styleOrder[0] - 2);
								target *= (360.0f / 9.0f);
								target += 360.0f * 5.0f;
								//target += 360.0f / 18.0f;

								Spinner[0].SetTarget(target);
								
								target = (gameOrder[1] - 2) * 3.0f;
								target += (styleOrder[1] - 2);
								target *= (360.0f / 9.0f);
								target += 360.0f * 6.0f;
								//target += 360.0f / 18.0f;

								Spinner[1].SetTarget(target);

								target = (gameOrder[2] - 2) * 3.0f;
								target += (styleOrder[2] - 2);
								target *= (360.0f / 9.0f);
								target += 360.0f * 7.0f;
								//target += 360.0f / 18.0f;

								Spinner[2].SetTarget(target);	
							//}							
							break;

	case TB_HOLD_ONE:		if (!Spinner[0].GetSpin())
								Hold[0] = Spinner[0].SetHold();	break;
	
	case TB_HOLD_TWO:		if (!Spinner[1].GetSpin())
								Hold[1] = Spinner[1].SetHold();	break;
	
	case TB_HOLD_THREE:		if (!Spinner[2].GetSpin())
								Hold[2] = Spinner[2].SetHold();	break;
	
	case TB_COLLECT:		if (!Spinner[0].GetSpin()&&!Spinner[1].GetSpin()&&!Spinner[2].GetSpin())
							{
							//	CleanUpTournamentPage();

								// PP: initialise tournament stage index (-1 because ProcessNextRound will increment it before the first stage)
								g_tournamentStage= -1;

								ProcessNextRound();
							}
							break;
	}
}

void UpdateTournamentPage()
{
	int i;

	switch (state)
	{
	case DEAD:		break;
	
	case CREATE:	for (i = 0; i < 3; i++)
						Spinner[i].UpdateSpinner();

					if (Spinner[0].GetActor()->alpha >= 255)
					{
						state = NORMAL;
						ProcessTournamentButton(TB_HANDLE_PULL);
					}

					break;

	case NORMAL:	for (i = 0; i < 3; i++)
						Spinner[i].UpdateSpinner();

					break;

	case DESTROY:	//CleanUpTournamentPage();
					break;
	}

	return;
}

void KillTournamentPage()
{
	state = DESTROY;
}

void CleanUpTournamentPage()
{
	int	i;

	if (Spinner)
	{		
		Spinner[0].SetActor(NULL);
		Spinner[1].SetActor(NULL);
		Spinner[2].SetActor(NULL);

		delete[] Spinner;
	}

	SlotMachine = 0;

	/*
	for (i = 0; i < 3; i++)
	{
		if (&Spinner[i])
			Spinner[i].~SPINNER();
	}
	*/
	state = DEAD;
}

int GetGameNumber(int gameOrderNumber)
{
	return (((styleOrder[gameOrderNumber] - 2)*3) + (gameOrder[gameOrderNumber] - 2));
}

char * GetTournamentSplashScreen()
{
	char *splash;

	if (Score1 == 0 && Score2 == 0)
	{
		splash = "splashnildraw.bmp";
	}	// JW: We've only just begun
	else if (Score1 == 1 && Score2 == 0)
	{
		splash = "splashonenil.bmp";
	}	// JW: Taz is winning
	else if (Score1 == 0 && Score2 == 1)
	{
		splash = "splashnilone.bmp";
	}	// JW: She-Devil is winning
	else if (Score1 == 1 && Score2 == 1)
	{
		splash = "splashonedraw.bmp";
	}	// JW: It's a tie

	return splash;
}

float	fireworkTimer = 0.0f;

bool	tournWinLoop;

BOOK	*tourWinBook;

void DoTournamentWin(FADE *fade)
{
	TEXTBOX	*pTextbox;
	PAGE	*pPage;

	setNumScreens(1);

	// CMD: hide everything
	/*HideActorInstanceList(&map.animatedInstances);
	HideActorInstanceList(&map.levelInstances);
	HideActorInstanceList(&map.characterInstances);
	HideActorInstanceList(&characterInstances);*/

	TBVector	position;

	// CMD: place camera at the origin
	bmVectorSet(position, 0.0f, METERTOUNIT(0.25f), 0.0f, 1.0f);
	SetCameraPosition(&camera[0], position);
	// CMD: look down the z axis
	bmVectorSet(position, 0.0f, METERTOUNIT(0.25f), METERTOUNIT(-1.0f), 1.0f);
	SetCameraLookAtPosition(&camera[0], position);

	camera[0].fade.StartFadeUp(FADETYPE_NORMAL, 1.0f);

	fireworkTimer = -1.0f;

	tournWinLoop = true;

	tourWinBook = new BOOK();

	pPage = tourWinBook->addPage();

	pPage->insertItem(pTextbox = new TEXTBOX(STR_DIALOGUE_CROSSOK));

	pTextbox->setFontSize(1.5f);
	pTextbox->setYAlign(PIFLAG_YALIGN_BOTTOM);

	tourWinBook->open();

	SendCamera(&camera[0]);

	do
	{
		UpdateTournamentWin();
		DrawTournamentWin();

		// PP: check for missing controllers; display a message if any are missing
		checkForMissingControllers2(true);

		Flip(0, 0, 0, 128);

		biReadDevices();

		if (OKBUTTON && !camera[0].fade.GetFadeSpeed())
		{
			tourWinBook->close();

			camera[0].fade.StartFadeDown(FADETYPE_NORMAL, 1.0f);
			camera[0].fade.SetFadeDownFinishedCallback(EndTournamentWin);
		}
	}
	while (tournWinLoop);
}

void UpdateTournamentWin()
{
	TBVector		position;

	BookList_update();

	baUpdateExtraSystem(fireworksExtraSystem, fTime);

	if (fireworkTimer < 0.0)
	{
		if (bmRand()%2)
			bmVectorSet(position, METERTOUNIT(25.0), METERTOUNIT(15.0f), METERTOUNIT(-40.0f), 1.0f);
		else
			bmVectorSet(position, METERTOUNIT(-25.0f), METERTOUNIT(15.0f), METERTOUNIT(-40.0f), 1.0f);

		CreateFireworkBurst(position, FIREWORK_RANDOMTYPE, 0,0,0,FIREWORKS_RANDOMCOLOUR);

		fireworkTimer = UtilGenerateRandomNumber(0.5f, 1.5f);
	}

	fireworkTimer -= fTime;
}

void DrawTournamentWin()
{
	bdSetupView();
	bdBeginScene();

	applyFOV();
	bdSetViewClipPlanes(videoMode.nearPlane, videoMode.farPlane);
	bdSetViewport(int(videoMode.screens[0].viewport.left), int(videoMode.screens[0].viewport.top), int(videoMode.screens[0].viewport.widthAbs()), int(videoMode.screens[0].viewport.heightAbs()));

	DrawSplashScreen(&gameStatus.splashScreen);

	baRenderExtraSystem(fireworksExtraSystem, (void*)EXTRA_ALPHA);

	camera[0].fade.DrawFade();

	BookList_draw();

	bdEndScene();

	// PP: finish timing the frame; calculate frames per second, frame length, etc.
	finishTimingFrame();
	
	// null time, anything between here and end is not included in the timer
	camera[0].fade.UpdateFade();

	// PP: start timing the frame
	startTimingFrame();
}

void EndTournamentWin(FADE *fade)
{
	tournWinLoop = false;
	
	ChangeScene(&gameStatus, SCENE_FRONTEND);

	Score1 = Score2 = 0;
	nextRound = 0;

	Hold[0] = Hold[1] = Hold[2] = 0;
}