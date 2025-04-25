// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Blitz Games Ltd.
//
//      File : multiplayer.cpp
//   Purpose : multiplayer stuff
// Component : Taz
//    Author : Chris W
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "main.h"
#include "textfx.h"

#include "race.h"
#include "destruct.h"
#include "diner.h"
#include "tournament.h"
#include "util.h"
// CPW: #include "timeattack.h"

#include "actors.h"
#include "camera.h"
#include "linkedlist.h"
#include "string.h"

#include "control.h"
#include "multiplayer.h"
#include "display.h"
#include "status.h"

#include "characters.h"
#include "water.h"
#include "fade.h"
#include "playerstats.h"

#include "bubblegum.h"
#include "superburp.h"
#include "chillipepper.h"
#include "hiccup.h"
#include "demo.h"

#include "vehicles.h"
#include "paperbook.h"
#include "popup.h"
#include "XboxStrings.h"			// PP: Xbox versions of some strings
#include "tutorial.h"				// TP: get the text scale values
#include "PageItemFX.h"				// PP: Special effects for page items
#include "ZooBoss.h"				// PP: Zoo Boss - Gossamer vs. Taz in an Elephant Pong showdown

// PP: ==================

// PP: dimension of mpLevelPakIcons - see below
#define MP_NUM_LEVELPAK_ICONS		26// PP: no slack

// PP: BODGE: array of ptrs to icons in the paperbook that point to textures from the level paks
// PP:			(these icons need their OOD texture ptrs cleared at the end of the level)
static ICON*	mpLevelPakIcons[MP_NUM_LEVELPAK_ICONS];

// PP: ==================

// PP: info relating to the Rule Book overlay
static struct RULEBOOKOVERLAY_INFO
{
	BOOK*	book;							// PP: the Rule Book overlay book
	BOOK*	subBooks[MAX_PLAYERS];			// PP: the prompt sub-books containing 'press start' messages for players 1 and 2 (in 2-player)
	uchar	player1Ready	:1;				// PP: is player 1 ready?
	uchar	player2Ready	:1;				// PP: is player 2 ready?
	bool	choosePads;						// PP: are the players currently able to choose their pads?

	/*	--------------------------------------------------------------------------------
		Function 	: RULEBOOKOVERLAY_INFO::ruleBookOverlayMenuActionFunc
		Purpose 	: callback that responds to menu actions on the rule book overlay
		Parameters 	: ref to a MENUACTION_INFO structure (TextFX.h)
		Returns 	: 
		Info 		: // PP: Must match the MenuActionFunc typedef (TextFX.h)
	*/
	static void menuActionFunc(MENUACTION_INFO& info);


	/*	--------------------------------------------------------------------------------
		Function 	: RULEBOOKOVERLAY_INFO::twoPlayer_pageOpenFunc
		Purpose 	: page-open func for 2-player page of the rule book overlay
		Parameters 	: ptr to the page, context value
		Returns 	: 
		Info 		: // PP: Must match the PageFunc typedef (TextFX.h)
	*/
	static void twoPlayer_pageOpenFunc(struct TAG_PAGE* const page, const int32 context);


}ruleBookOverlayInfo;


static int mpMode = MPMODE_NONE;

BOOK		*pEndGameBook = NULL;
BOOK		*pEndMPGameStats = NULL;
BOOK		*pEndBonusBook = NULL;

LEVELDATA	levelData;

// CPW: static int (*mpHandler)(int)[MPMODE_NUMBER];

typedef int(MP_HANDLER)(int);
MP_HANDLER *mpHandler[MPMODE_NUMBER] =
#ifndef CONSUMERDEMO
	{NULL, NULL, raceHandler, destructHandler, raceHandler, dinerHandler, NULL, NULL};
#else
	{NULL, NULL, raceHandler, NULL, NULL, NULL, NULL, NULL};
#endif

ACTORINSTANCE *mpPlayer[2];
int mpNumPlayers = 1;
CAMERAINFO *mpDrawCam;
int mpSubMode;

bool	mpTimerFlash;

float	CountdownStartTime = -1;

float	CheckTimeon = 0, CheckStarttime = -1;

int mpInit(int mode, int subMode, ACTORINSTANCE *p1, ACTORINSTANCE *p2)
{
	if(mode == MPMODE_NONE)
		return 0;

	mpPlayer[0] = p1;
	mpPlayer[1] = p2;

	if(p2)
		mpNumPlayers = 2;
	else
		mpNumPlayers = 1;

	mpMode = mode;
	mpSubMode = subMode;
	return(mpHandler[mpMode](MPREASON_INITIALISE));
}

int mpPrep(void)
{
	if(mpMode == MPMODE_NONE)
		return 0;

	return(mpHandler[mpMode](MPREASON_PREP));
}

int mpPoll(void)
{
	if(mpMode == MPMODE_NONE)
		return 0;

// CPW: 	mpTextPoll();
	return(mpHandler[mpMode](MPREASON_POLL));
}

int mpDraw(CAMERAINFO *cameraInfo)
{
	if(mpMode == MPMODE_NONE)
		return 0;

	mpDrawCam = cameraInfo;
	return(mpHandler[mpMode](MPREASON_DRAW));
}

int mpKill(void)
{
	if(mpMode == MPMODE_NONE)
		return 0;

	BookList_closeAndFree(pEndGameBook);

	return(mpHandler[mpMode](MPREASON_KILL));
	mpMode = MPMODE_NONE;
}

// JW: Process a characters falling death in multiplayer mode

void DoMultiplayerCharacterFall(ACTORINSTANCE *player)
{
	TBVector		resetPoint, worldSpace, screenSpace;

	char			*tempName;

	ACTORINSTANCE	*tempInstance;

	// TP: Remove any power ups the character had
	KillAllPowerUps(player);
	
	tempName = new char[32];

	TBActorNodeInstance	*node;
		
	if (player->actorStatus->multiplayer.checkPoint)
		sprintf(tempName, "checkpoint0%dreset%d", player->actorStatus->multiplayer.checkPoint - 1, player->playerNo + 1);
	else
		sprintf(tempName, "checkpoint0%dreset%d", gameStatus.multiplayer.numCheckPoints - 1, player->playerNo + 1);
		
	tempInstance = FindActorInstanceInInstanceList(tempName, 0, &map.invisibleInstances);
	
	if(!tempInstance) tempInstance = FindActorInstanceInInstanceList(tempName, 0, &map.collisionInstances);
	if(!tempInstance) tempInstance = FindActorInstanceInInstanceList(tempName, 0, &map.levelInstances);

	if (tempInstance)
	{
		bmVectorCopy(resetPoint, tempInstance->actorInstance.position);
	}
	else
	{
		bmVectorCopy(resetPoint, player->actorStatus->startPosition);
	}

	node = baFindNode(player->actorInstance.rootNodeInstance, "snoutstem");
		
	if(node)
	{
		if (player->playerNo!=-1)
		{
// TP: no health anymore			if((player->characterInfo->health.currentHealth-1))
			{
				gameStatus.player[player->playerNo].camera->fade.StartFadeDown(FADETYPE_CIRCLE, 1.5f);
				gameStatus.player[player->playerNo].camera->fade.SetPlayer(player);
				gameStatus.player[player->playerNo].camera->fade.SetFadeDownFinishedCallback(FinishedFadeFromMultiplayerFall);
				gameStatus.player[player->playerNo].camera->fade.SetFadeUpFinishedCallback(ResetFadeToPoint);
				
				bdSetIdentityObjectMatrix();
				baGetNodesWorldPosition(&player->actorInstance, node, worldSpace);
				worldSpace[3] = 1.0f;
				bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
				bdSetIdentityObjectMatrix();
				bdProjectVertices(screenSpace, worldSpace, 1);
				
				gameStatus.player[player->playerNo].camera->fade.SetCircleFadeTo(screenSpace[0], screenSpace[1]);
				gameStatus.player[player->playerNo].camera->fade.FadeUpOnCompletion(true);
			
				player->characterInfo->powerUpInfo.clock = 0.0f;
			}
		}
	}

	bmVectorCopy(player->actorStatus->waterResetPoint, resetPoint);

	FREE(tempName);
}

void FinishedFadeFromMultiplayerFall(class TAG_FADE *fade)
{
	ACTORINSTANCE *pPlayer = fade->GetPlayer();
	
	bmVectorCopy(pPlayer->actorInstance.position, pPlayer->actorStatus->waterResetPoint);
	
	ResetCamera(gameStatus.player[pPlayer->playerNo].camera);
	
	fade->SetFadeDownFinishedCallback(NULL);

	fade->StartFadeUp(FADETYPE_CIRCLE, 1.0f);
}

// CPW: generic multiplayer routines and stuff

BOOK	*raceBook = 0;
float	actualTime;

#define COUNTDOWN_SIZE (6)

void mpPageUpdateAndClose(PAGE * const pPage, const int32 context)
{
	TEXTBOX*		textBox=((TEXTBOX *)pPage->getLastItem());

	if(textBox->getScale() != -1.0f)
	{
		textBox->setScale(textBox->getScale()+0.15f);
	}
}

void mpPageUpdate(PAGE * const pPage, const int32 context)
{
	char	string8[16];
	
	TEXTBOX*		textBox=((TEXTBOX *)pPage->getLastItem());

	if (mpCountdownUpdate())
	{
		textBox->setScale(textBox->getScale()+0.15f);
		textBox->setOpacity(textBox->getOpacity() - 0.01f);
	}
	else
	{
		if (strcmp(pPage->name, "Timer") == 0)
		{
			// JW: We're in the timer page
			if (actualTime < 10.0f && !mpTimerFlash)	
			{
				textBox->startEffect(PIE_flash.copy()->setItem(textBox)->setMode(PIEMODE_ON));

				mpTimerFlash = true;
			}

			if (actualTime > 10.0f && mpTimerFlash)	
			{
				textBox->startEffect(NULL);

				mpTimerFlash = false;
			}

			utilGetPrettyTime(string8, actualTime);
			textBox->sprintf8("%s", string8);
		}
		else if (strcmp(pPage->name, "ExtraTime") == 0)
			textBox->setScale(2.0f);
	}
}

void mpPageOpen(PAGE * const pPage, const int32 context)
{
	TEXTBOX*		textBox=((TEXTBOX *)pPage->getLastItem());

	textBox->setScale(1.0f);
}

void mpPageClose(PAGE * const pPage, const int32 context)
{// CPW: don't actually do anything (yet)
}

void mpCountdownSetup(int scene)
{
	TEXTBOX	*tb;
	char	buf[128];
	PAGE	*pPage;
	PAGE	*subPage;

	raceBook = new BOOK();

	// PP: This book gets shutdown and needs to be re-init'd before you use it again
	raceBook->init();

	raceBook->setName("raceBook");

	// PP: Use a custom PIE chooser for the race book
	raceBook->setEffectChooser(&racePIEChooser);

	raceBook->addPage(STR_COUNTDOWN_5, mpPageUpdate, "5", mpPageOpen, mpPageClose);
	bkPrintf("Added Page 5\n");
	raceBook->addPage(STR_COUNTDOWN_4, mpPageUpdate, "4", mpPageOpen, mpPageClose);
	bkPrintf("Added Page 4\n");
	raceBook->addPage(STR_COUNTDOWN_3, mpPageUpdate, "3", mpPageOpen, mpPageClose);
	bkPrintf("Added Page 3\n");
	raceBook->addPage(STR_COUNTDOWN_2, mpPageUpdate, "2", mpPageOpen, mpPageClose);
	bkPrintf("Added Page 2\n");
	raceBook->addPage(STR_COUNTDOWN_1, mpPageUpdate, "1", mpPageOpen, mpPageClose);
	bkPrintf("Added Page 1\n");
	raceBook->addPage(STR_COUNTDOWN_GO, mpPageUpdate, "GO", mpPageOpen, mpPageClose);
	bkPrintf("Added Page G\n");
	raceBook->addPage(tb = new TEXTBOX(), mpPageUpdate, "Timer", mpPageOpen, mpPageClose);
			
	tb->setFontSize(2.0f);
	tb->setEffectChooser(&mainPIEChooser);// PP: Don't use the race PIE chooser with the timer page
	
	if (gameStatus.multiplayer.versusCPU)
	{
		tb->setYAlign(PIFLAG_YALIGN_TOP);
		tb->setXAlign(PIFLAG_XALIGN_CENTRE);
	}

	pPage = raceBook->addPage(tb = new TEXTBOX(STR_TIMES_UP), mpPageUpdate, "TimeUp", mpPageOpen, mpPageClose);
	
	// PP: NEW: use proper page item effects on the timeup textbox
	tb->setFontSize(3.0f)
		->setWrap(true);

	pPage = raceBook->addPage(tb = new TEXTBOX(STR_SUDDEN_DEATH), mpPageUpdate, "SuddenDeath", mpPageOpen, mpPageClose);
	pPage = raceBook->addPage(tb = new TEXTBOX(STR_EXTRA_TIME), mpPageUpdate, "ExtraTime", mpPageOpen, mpPageClose);
	tb->setScale(2.0f);

	raceBook->gotoPage("Intro", true);
//	raceBook->open();

	CountdownStartTime = -1;

	mpTimerFlash = false;
}

char *tweetyCountSpeech[] = {"tweety_4.wav", "tweety_3.wav", "tweety_2.wav", "tweety_1.wav", "tweetycount_go.wav"};
char *elmerCountSpeech[] = {"citybosscount_4.wav", "citybosscount_3.wav", "citybosscount_2.wav", "citybosscount_1.wav", "elmercount_go.wav"};

void mpCountdownDraw(void)
{
	if (CountdownStartTime == -1)
		CountdownStartTime = gameStatus.appTime;// PP: NOTE: appTime (real seconds) rather than gameTime (could be speed-adjusted).  This is needed because the countdown speech is all one sample, and we're not allowed to speed up speech, and I can't be arsed chopping it up into seperate samples.

	float countDown = gameStatus.appTime - CountdownStartTime;// PP: NOTE: appTime (real seconds) rather than gameTime (could be speed-adjusted).  This is needed because the countdown speech is all one sample, and we're not allowed to speed up speech, and I can't be arsed chopping it up into seperate samples.
	static int oldCount = -1;// CPW: (int)countDown;

	if (countDown < (float) COUNTDOWN_SIZE)
	{
		TBMatrix mScaled;
		float remainder = (float)bmFMod(countDown, 1.0f);
		float fontScale = (remainder) * 4;
		
		int intCount = (int)countDown;
		
		bmMatCopy(mScaled, bIdentityMatrix);
		bmMatScale(mScaled, fontScale, fontScale, fontScale);
		bdSetObjectMatrix(mScaled);
		
		if(intCount != oldCount)
		{
			oldCount = intCount;
			
			if(intCount == 0)
			{
				raceBook->gotoPage("5", true);
				bkPrintf("Page 5\n");
			}
			else
			{
				if (map.sceneNumber == SCENE_CITYBOSS)
					PlaySample(elmerCountSpeech[intCount-1],255);
				else
					PlaySample(tweetyCountSpeech[intCount-1],255);

				/*if (intCount == 5)
				{
					if (map.sceneNumber == SCENE_CITYBOSS)
						PlaySample("elmercount_go.wav", 255);
					else
						PlaySample("tweetycount_go.wav", 255);
				}*/

				raceBook->nextPage();

				bkPrintf("Next Page!\n");
			}
		}
	}
	else
		gameStatus.multiplayer.countingDown = FALSE;

	mpDrawBook();
}

int	mpDrawBook()
{
	// PP: FUNCTION OOD
	return 0;
}

int mpCountdownUpdate(void)
{
	return(gameStatus.multiplayer.countingDown);
}

void mpCountdownKill(void)
{
	if (raceBook)
	{
		raceBook->close();
		// TP: raceBook->shutdown();
		SAFE_DELETE(raceBook);
	}
}

#define ENDGAME_NUM_CATEGORIES (2)

int		currentCatergory = 0;
TEXTBOX	*pEndGameTextBox[2];
int		*pMode;
int		retMode = 0;

static float	allowedToProceed;

#define TB_PLAYAGAIN	(0)
#define TB_QUIT			(1)

bool	choiceMade;

void mpEndGameMenuSetUp(int *pCurrentMode, int returnMode, const int32 winner)
{
	PauseRumble(&controller1);
	PauseRumble(&controller2);

	FlushRumbleQueue(&controller1);
	FlushRumbleQueue(&controller2);

	UpdateRumble(&controller1);
	UpdateRumble(&controller2);

	COLOUR		statsBookColour=COLOUR(128, 128, 128, 128);// PP: NOTE: COLOURs default to an alpha value of 255 which buggers text antialiasing somewhat

	pMode = pCurrentMode;

	pEndGameBook = new BOOK;
	// PP: Set the size of the book
	// PP: REMOUT: books now default to filling the full 'safe' area of the screen		pEndGameBook->setBookRect(RECTANGLE(-0.5f, 0.5f, -0.5f, 0.5f));// PP: REMOUT: must now be expressed as fractions of the screen dimensions		RECTANGLE(-300, 300, -220, 220));

	// PP: 'result' textbox ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// PP: Create and add the textbox

	PAGE *pPage;
	TEXTBOX	*pTextBox;

	pEndGameBook->setBookRect(RECTANGLE(-0.4f, 0.4f, -0.25f, 0.25f));

	pPage = pEndGameBook->addPage(pTextBox = new TEXTBOX());

	pPage->setController(gameStatus.player1->actorStatus->multiplayer.position);
	pPage->setNavigation(true);
	pPage->setNavigationWrap(PAGEFLAG_NAVWRAP_YDEBOUNCE);

	// PP: Set the font size
	pTextBox->setScale(1.0f);

	// PP: Set the font colour and specify no vertical gradient
	pTextBox->setVGrad(false);
	pTextBox->setColour(statsBookColour);

	// PP: Align to top-centre
	pTextBox->setXAlign(PIFLAG_XALIGN_CENTRE);
	pTextBox->setYAlign(PIFLAG_YALIGN_CENTRE);

// PP: REMOUT: OOD	pTextBox->sprintf8("%s", sResult);

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

#ifndef CONSUMERDEMO
	// PP: 'Timer' textbox ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// PP: Create and add the textbox
	pPage->insertItem(pEndGameTextBox[TB_PLAYAGAIN] = new TEXTBOX(STR_PLAY_AGAIN));
	pTextBox = pEndGameTextBox[TB_PLAYAGAIN];

	// PP: Set the font size
	pTextBox->setScale(2.0f);

	// PP: Set the font colour and specify no vertical gradient
	pTextBox->setVGrad(false);
	pTextBox->setColour(statsBookColour);

	// PP: Align to bottom-left
	pTextBox->setXAlign(PIFLAG_XALIGN_CENTRE);
	pTextBox->setYAlign(PIFLAG_YALIGN_BOTTOM);

	// PP: 'Position' textbox ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// PP: Create and add the textbox
	pPage->insertItem(pEndGameTextBox[TB_QUIT] = new TEXTBOX(STR_EXIT_EXITGAME));
	pTextBox = pEndGameTextBox[TB_QUIT];

	// PP: Set the font size
	pTextBox->setScale(2.0f);

	// PP: Set the font colour and specify no vertical gradient
	pTextBox->setVGrad(false);
	pTextBox->setColour(statsBookColour);

	// PP: Align to bottom-left
	pTextBox->setXAlign(PIFLAG_XALIGN_CENTRE);
	pTextBox->setYAlign(PIFLAG_YALIGN_TOP);

	pPage->insertItem(pTextBox = new TEXTBOX(STR_PRESS_X_TO_CONTINUE));

	pTextBox->setScale(1.0f);

	// PP: Set the font colour and specify no vertical gradient
	pTextBox->setVGrad(false);
	pTextBox->setColour(statsBookColour);

	// PP: Align to bottom-left
	pTextBox->setXAlign(PIFLAG_XALIGN_CENTRE);
	pTextBox->setYAlign(PIFLAG_YALIGN_BOTTOM);

	pTextBox->setSelectable(false);
#else
#if(BPLATFORM==PC)
	pPage->insertItem(pTextBox = new TEXTBOX(STR_PRESS_JUMP_TO_CONTINUE));
#else
	pPage->insertItem(pTextBox = new TEXTBOX(STR_PRESS_START_TO_CONTINUE));
#endif // NH: #if(BPLATFORM==PC)

	// PP: Set the font colour and specify no vertical gradient
	pTextBox->setVGrad(false);
	pTextBox->setColour(statsBookColour);
#endif

	pEndGameBook->open();

	InitEndMPGameStatsBook();

	choiceMade = false;

	gameStatus.player[0].camera->fade.SetFadeDownFinishedCallback(NULL);
	gameStatus.player[1].camera->fade.SetFadeDownFinishedCallback(NULL);

	if (!gameStatus.player1->actorStatus->multiplayer.position)
		pEndGameBook->setScreen(0);
	else
		pEndGameBook->setScreen(1);

	pPage->selectItem(pEndGameTextBox[TB_PLAYAGAIN]);

	allowedToProceed = gameStatus.gameTime + 2.0f;
}


/*	--------------------------------------------------------------------------------
	Function 	: mpEndGameMenuHandler
	Purpose 	: custom pause update function for the multiplayer end-of-level menus
	Parameters 	: context value, required but currently not used
	Returns 	: 
	Info 		: // PP: must match the PauseUpdateFunc typedef (main.h)
*/
void mpEndGameMenuHandler(const int32 context)
{
	if (!pEndGameBook)
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

#ifndef CONSUMERDEMO
	if (choiceMade == false)
	{
		if ((gameStatus.player1->actorStatus->multiplayer.position ? OKBUTTON_PADNUM(2) : OKBUTTON_PADNUM(1)) && allowedToProceed < gameStatus.gameTime)
		{
			// CPW: item has been selected
			// CPW: SetGameState(&gameStatus, GAMESTATE_INGAME, GAMESUBSTATE_NORMAL);
			
			// PP: Force-debounce ok button(s)
			FORCE_OKBUTTON_DEBOUNCE;
			
			*pMode = retMode;

			if (!gameStatus.player1->actorStatus->multiplayer.position)
				camera[0].fade.StartFadeDown(FADETYPE_NORMAL, 1.0f);
			else
				camera[1].fade.StartFadeDown(FADETYPE_NORMAL, 1.0f);

			choiceMade = true;

			pEndGameBook->close();
			pEndMPGameStats->close();
		}
		
		for(int i=0; i<ENDGAME_NUM_CATEGORIES; i++)
		{
			if (pEndGameTextBox[i]->selected)
			{
				currentCatergory = i;
				
				pEndGameTextBox[i]->setScale(2.0f);
			}
			else
				pEndGameTextBox[i]->setScale(1.0f);
		}
	}
	else
	{
		if ((!gameStatus.player1->actorStatus->multiplayer.position && gameStatus.player[0].camera->fade.GetFadeValue() == 127.0f) ||
			(!gameStatus.player2->actorStatus->multiplayer.position && gameStatus.player[1].camera->fade.GetFadeValue() == 127.0f))
		{
			SetGameState(&gameStatus, GAMESTATE_INGAME, GAMESUBSTATE_NORMAL);
		
			if(currentCatergory == TB_PLAYAGAIN)
				ChangeScene(&gameStatus, map.sceneNumber);
			else
			{	//Quit to main menu
				ChangeScene(&gameStatus, SCENE_FRONTEND);
				
				gameStatus.multiplayer.numPlayers = 1;
			}

			mpEndGameMenuKill();

			return;
		}
	}
#else
	if ((controller1.idleTime > to_inactive && controller2.idleTime > to_inactive) && (camera[gameStatus.player1->actorStatus->multiplayer.position].fade.GetFadeValue() == 0.0f))
	{
		BookList_closeAll();

		gameStatus.multiplayer.numPlayers = 1;

		gameStatus.multiplayer.versusCPU = true;

		camera[gameStatus.player1->actorStatus->multiplayer.position].fade.StartFadeDown(FADETYPE_NORMAL, 0.5f);

		camera[gameStatus.player1->actorStatus->multiplayer.position].fade.SetFadeDownFinishedCallback(StopDemoGameFadeDownCallback);
	}

	if(controller.startDebounceChannel->value || (gameStatus.player1->actorStatus->multiplayer.position ? OKBUTTON_PADNUM(2) : OKBUTTON_PADNUM(1)))
	{
		controller.startDebounceChannel->value = 0;

		// PP: Force-debounce ok button(s)
		FORCE_OKBUTTON_DEBOUNCE;

		pEndGameBook->close();
		pEndMPGameStats->close();

		if (!gameStatus.player1->actorStatus->multiplayer.position)
		{
			camera[0].fade.StartFadeDown(FADETYPE_NORMAL, 1.0f);
			camera[0].fade.SetFadeDownFinishedCallback(StopDemoGameFadeDownCallback);
		}
		else
		{
			camera[1].fade.StartFadeDown(FADETYPE_NORMAL, 1.0f);
			camera[1].fade.SetFadeDownFinishedCallback(StopDemoGameFadeDownCallback);
		}
	}
#endif
}

void mpEndGameMenuKill(void)
{
	if (pEndGameBook)
	{
		pEndGameBook->close();
		delete(pEndGameBook);
 		pEndGameBook = NULL;
	}

	if (pEndMPGameStats)
	{
		pEndMPGameStats->close();
		delete(pEndMPGameStats);
 		pEndMPGameStats = NULL;
	}
}

void InitMultiplayer(int scene)
{
	switch (scene)
	{
	case SCENE_CITYBOSS:	gameStatus.multiplayer.numPlayers = 2;
							break;
							
	case SCENE_MUSEUMVEHICLERACE:
	case SCENE_GRANDCVEHICLERACE:
	case SCENE_DEPTSTRVEHICLERACE:

	case SCENE_DESTRUCTIONAQUA:
	case SCENE_DESTRUCTIONCONSTRUCT:
	case SCENE_DESTRUCTIONTASMANIA:

	case SCENE_ICEDOMERACE:
	case SCENE_GOLDMINERACE:
	case SCENE_SAFARIRACE:
									gameStatus.multiplayer.versusCPU = false;
									//setNumScreens(gameStatus.multiplayer.numPlayers);
									break;
	}
}

void InitEndMPGameStatsBook()
{
	PAGE	*pPage;
	PAGE	*psubPage;
	TEXTBOX	*ptextBox;

	COLOUR	statsBookColour=COLOUR(128, 128, 128, 128);
	COLOUR	goldBookColour=COLOUR(128, 128, 0, 128);
	COLOUR	silverBookColour=COLOUR(64, 64, 64, 128);

	int	i;
	int	NUM_LAPS;

	char	string8[12];

	float	totalTime, centiTime;

	pEndMPGameStats	= new BOOK;

	pEndMPGameStats->setBookRect(RECTANGLE(-0.4f, 0.4f, -0.25f, 0.25f));

	if (map.sceneNumber > SCENE_TAZBOSS)
	{
		switch (mpMode)
		{
		case MPMODE_TIMEATTACK:
		case MPMODE_RACE:		if (map.sceneNumber == SCENE_SAFARIRACE)
									NUM_LAPS = 4;
								else
									NUM_LAPS = 3;
			
								pPage = pEndMPGameStats->addPage("Page");

								// JW: Taz's stats

								totalTime = 0.0f;
								centiTime = 0.0f;
								
								psubPage =  new PAGE;

								// PP: this tells the sub-page to remove all its slack so that it can be aligned like any other page item
								psubPage->setSlack(false);

								psubPage->insertItem(ptextBox = new TEXTBOX(STR_MP_TAZ));
								
								ptextBox->setYAlign(PIFLAG_YALIGN_TOP);
								ptextBox->setColour(statsBookColour);
								ptextBox->setVGrad(false);

								for (i = 0; i < NUM_LAPS; i++)
								{
									switch(i)
									{
									case 0:	psubPage->insertItem(ptextBox = new TEXTBOX(STR_MP_LAP1));	break;
									case 1:	psubPage->insertItem(ptextBox = new TEXTBOX(STR_MP_LAP2));	break;
									case 2:	psubPage->insertItem(ptextBox = new TEXTBOX(STR_MP_LAP3));	break;
									case 3:	psubPage->insertItem(ptextBox = new TEXTBOX(STR_MP_LAP4));	break;
									}
									
									ptextBox->setXAlign(PIFLAG_XALIGN_RIGHT);
									ptextBox->setScale(0.8f);
									ptextBox->setColour(statsBookColour);
									ptextBox->setVGrad(false);

									psubPage->insertItem(ptextBox = new TEXTBOX(), false);
								
									if (gameStatus.player1->actorStatus->multiplayer.lapTime[i])
									{
										utilGetPrettyTime(string8, gameStatus.player1->actorStatus->multiplayer.lapTime[i]);
										ptextBox->sprintf8(" - %s", string8);
									}
									else
										ptextBox->sprintf8(" - =:==.=");

									ptextBox->setXAlign(PIFLAG_XALIGN_LEFT);
									ptextBox->setScale(0.8f);
									
									if (i == gameStatus.player1->actorStatus->multiplayer.lapBestTime)
									{
										if ((gameStatus.player1->actorStatus->multiplayer.lapTime[i] < gameStatus.player2->actorStatus->multiplayer.lapTime[gameStatus.player2->actorStatus->multiplayer.lapBestTime]) || gameStatus.player2->actorStatus->multiplayer.lapBestTime == -1)
										{
											ptextBox->setColour(goldBookColour);
										}
										else
										{
											ptextBox->setColour(silverBookColour);
										}

	#ifdef TEXT_CHARACTER_MOVEMENT_SUPPORTED

										// PP: ideally I imagine we'd use a sparkling chrome effect on the text

										// PP: jings, we've got to do *something* to liven it up

										// PP: NOTE: This doesn't seem to be doing its job.
										// PP: Well how can you expect me to know about this kind of thing if I'm not the one writing the front end?!

										ptextBox->setEffect(&PIE_textLife, PIEMODE_ON);

	#endif// PP: TEXT_CHARACTER_MOVEMENT_SUPPORTED
									}
									else
										ptextBox->setColour(statsBookColour);

									ptextBox->setVGrad(false);
								}

								// JW: Show taz's total race time

								psubPage->insertItem(ptextBox = new TEXTBOX(STR_MP_TOTAL));

								ptextBox->setXAlign(PIFLAG_XALIGN_RIGHT);
								ptextBox->setScale(0.8f);
								ptextBox->setColour(statsBookColour);
								ptextBox->setVGrad(false);

								psubPage->insertItem(ptextBox = new TEXTBOX(), false);
								
								for (i = 0; i < NUM_LAPS; i++)
								{
									if (gameStatus.player1->actorStatus->multiplayer.lapTime[i])
									{
										totalTime += (float) ((int) gameStatus.player1->actorStatus->multiplayer.lapTime[i]);
										
										centiTime += (float) ((int) (10.0f * bmFMod(gameStatus.player1->actorStatus->multiplayer.lapTime[i], 1.0)) / 10.0f);
									}
									else
										goto TDNoTotal;
								}

								totalTime += (centiTime + 0.001f);

								utilGetPrettyTime(string8, totalTime);

								ptextBox->sprintf8(" - %s", string8);

								goto TDContinue;
								
	TDNoTotal:
								ptextBox->sprintf8(" - =:==.=");

	TDContinue:
								
								ptextBox->setXAlign(PIFLAG_XALIGN_LEFT);
								ptextBox->setScale(0.8f);
								ptextBox->setColour(statsBookColour);
								ptextBox->setVGrad(false);

								pPage->insertItem(psubPage);

								// JW: She Devils Stats

								totalTime = 0.0f;
								centiTime = 0.0f;

								psubPage =  new PAGE;

								// PP: this tells the sub-page to remove all its slack so that it can be aligned like any other page item
								psubPage->setSlack(false);

								psubPage->insertItem(ptextBox = new TEXTBOX(STR_MP_SHEDEVIL));
								
								ptextBox->setYAlign(PIFLAG_YALIGN_TOP);
								ptextBox->setColour(statsBookColour);
								ptextBox->setVGrad(false);

								for (i = 0; i < NUM_LAPS; i++)
								{
									switch(i)
									{
									case 0:	psubPage->insertItem(ptextBox = new TEXTBOX(STR_MP_LAP1));	break;
									case 1:	psubPage->insertItem(ptextBox = new TEXTBOX(STR_MP_LAP2));	break;
									case 2:	psubPage->insertItem(ptextBox = new TEXTBOX(STR_MP_LAP3));	break;
									case 3:	psubPage->insertItem(ptextBox = new TEXTBOX(STR_MP_LAP4));	break;
									}

									ptextBox->setXAlign(PIFLAG_XALIGN_RIGHT);
									ptextBox->setScale(0.8f);
									ptextBox->setColour(statsBookColour);
									ptextBox->setVGrad(false);

									psubPage->insertItem(ptextBox = new TEXTBOX(), false);
								
									if (gameStatus.player2->actorStatus->multiplayer.lapTime[i])
									{
										utilGetPrettyTime(string8, gameStatus.player2->actorStatus->multiplayer.lapTime[i]);
										ptextBox->sprintf8(" - %s", string8);
									}
									else
										ptextBox->sprintf8(" - =:==.=");

									ptextBox->setXAlign(PIFLAG_XALIGN_LEFT);
									ptextBox->setScale(0.8f);
									
									if (i == gameStatus.player2->actorStatus->multiplayer.lapBestTime)
									{
										if ((gameStatus.player2->actorStatus->multiplayer.lapTime[i] < gameStatus.player1->actorStatus->multiplayer.lapTime[gameStatus.player1->actorStatus->multiplayer.lapBestTime]) || gameStatus.player1->actorStatus->multiplayer.lapBestTime == -1)
										{
											ptextBox->setColour(goldBookColour);
										}
										else
										{
											ptextBox->setColour(silverBookColour);
										}

	#ifdef TEXT_CHARACTER_MOVEMENT_SUPPORTED

										// PP: ideally I imagine we'd use a sparkling chrome effect on the text

										// PP: jings, we've got to do *something* to liven it up

										// PP: NOTE: This doesn't seem to be doing its job.
										// PP: Well how can you expect me to know about this kind of thing if I'm not the one writing the front end?!
										ptextBox->setEffect(&PIE_textLife, PIEMODE_ON);

	#endif// PP: TEXT_CHARACTER_MOVEMENT_SUPPORTED
									}
									else
										ptextBox->setColour(statsBookColour);

									ptextBox->setVGrad(false);
								}

								// JW: Show taz's total race time

								psubPage->insertItem(ptextBox = new TEXTBOX(STR_MP_TOTAL));

								ptextBox->setXAlign(PIFLAG_XALIGN_RIGHT);
								ptextBox->setScale(0.8f);
								ptextBox->setColour(statsBookColour);
								ptextBox->setVGrad(false);

								psubPage->insertItem(ptextBox = new TEXTBOX(), false);

								for (i = 0; i < NUM_LAPS; i++)
								{
									if (gameStatus.player2->actorStatus->multiplayer.lapTime[i])
									{
										totalTime += (float) ((int) gameStatus.player2->actorStatus->multiplayer.lapTime[i]);
										
										centiTime += (float) ((int) (10.0f * bmFMod(gameStatus.player2->actorStatus->multiplayer.lapTime[i], 1.0)) / 10.0f);
									}
									else
										goto SDNoTotal;
								}

								utilGetPrettyTime(string8, totalTime + centiTime);

								ptextBox->sprintf8(" - %s", string8);

								goto SDContinue;
								
	SDNoTotal:
								ptextBox->sprintf8(" - =:==.=");

	SDContinue:
								ptextBox->setXAlign(PIFLAG_XALIGN_LEFT);
								ptextBox->setScale(0.8f);
								ptextBox->setColour(statsBookColour);
								ptextBox->setVGrad(false);

								// PP: put the second of the two stats sub-pages (She-Devil's page) on a new row if we're in vertical splitscreen mode
								// PP: otherwise it won't fit
								pPage->insertItem(psubPage, /* PP: false*/gameStatus.multiplayer.splitScreenMode==SPLITSCREEN_VERTICAL);
			
								break;

		case MPMODE_DESTRUCT:	pPage = pEndMPGameStats->addPage("Page");

								psubPage =  new PAGE;

								psubPage->insertItem(ptextBox = new TEXTBOX(STR_MP_TAZ));

								ptextBox->setColour(statsBookColour);
								ptextBox->setVGrad(false);

								pPage->insertItem(psubPage);

								psubPage->insertItem(ptextBox = new TEXTBOX(STR_MP_TOTAL));
								
								ptextBox->setYAlign(PIFLAG_XALIGN_RIGHT);
								ptextBox->setColour(statsBookColour);
								ptextBox->setVGrad(false);

								psubPage->insertItem(ptextBox = new TEXTBOX(), false);

								ptextBox->sprintf8(" - %d", gameStatus.player1->actorStatus->multiplayer.numPickups);
								ptextBox->setYAlign(PIFLAG_XALIGN_LEFT);
								ptextBox->setColour(statsBookColour);
								ptextBox->setVGrad(false);

								psubPage =  new PAGE;

								psubPage->insertItem(ptextBox = new TEXTBOX(STR_MP_SHEDEVIL));

								ptextBox->setColour(statsBookColour);
								ptextBox->setVGrad(false);

								pPage->insertItem(psubPage, false);

								psubPage->insertItem(ptextBox = new TEXTBOX(STR_MP_TOTAL));

								ptextBox->setXAlign(PIFLAG_XALIGN_RIGHT);
								ptextBox->setColour(statsBookColour);
								ptextBox->setVGrad(false);

								psubPage->insertItem(ptextBox = new TEXTBOX(), false);

								ptextBox->sprintf8(" - %d", gameStatus.player2->actorStatus->multiplayer.numPickups);
								ptextBox->setXAlign(PIFLAG_XALIGN_LEFT);
								ptextBox->setColour(statsBookColour);
								ptextBox->setVGrad(false);
			
								break;
		}
	}
	else
	{
		switch (map.sceneNumber)
		{
		case SCENE_CITYBOSS:	pPage = pEndMPGameStats->addPage("Page");

								psubPage =  new PAGE;

								psubPage->insertItem(ptextBox = new TEXTBOX(STR_MP_TAZ));

								ptextBox->setColour(statsBookColour);
								ptextBox->setVGrad(false);

								pPage->insertItem(psubPage);

								psubPage->insertItem(ptextBox = new TEXTBOX(STR_MP_TOTAL));
								
								ptextBox->setYAlign(PIFLAG_XALIGN_RIGHT);
								ptextBox->setColour(statsBookColour);
								ptextBox->setVGrad(false);

								psubPage->insertItem(ptextBox = new TEXTBOX(), false);

								ptextBox->sprintf8(" - %i", (int) gameStatus.player1->actorStatus->multiplayer.numStars);
								ptextBox->setYAlign(PIFLAG_XALIGN_LEFT);
								ptextBox->setColour(statsBookColour);
								ptextBox->setVGrad(false);

								psubPage =  new PAGE;

								psubPage->insertItem(ptextBox = new TEXTBOX(STR_ELMERDAFFY));

								ptextBox->setColour(statsBookColour);
								ptextBox->setVGrad(false);

								pPage->insertItem(psubPage, false);

								psubPage->insertItem(ptextBox = new TEXTBOX(STR_MP_TOTAL));

								ptextBox->setXAlign(PIFLAG_XALIGN_RIGHT);
								ptextBox->setColour(statsBookColour);
								ptextBox->setVGrad(false);

								psubPage->insertItem(ptextBox = new TEXTBOX(), false);

								ptextBox->sprintf8(" - %i", (int) gameStatus.player2->actorStatus->multiplayer.numStars);
								ptextBox->setXAlign(PIFLAG_XALIGN_LEFT);
								ptextBox->setColour(statsBookColour);
								ptextBox->setVGrad(false);
			
								break;
		}
	}

	StopChannel(&characterSounds.taz);
	StopChannel(&characterSounds.sheDevil);
	
	StopChannel(&gameStatus.player1->actorStatus->soundHandle[0]);

	if (gameStatus.player1->actorStatus->vehicleInfo)
		StopChannel(&gameStatus.player1->actorStatus->vehicleInfo->soundHandle);
	
	if (!gameStatus.multiplayer.versusCPU)
	{	
		StopChannel(&gameStatus.player2->actorStatus->soundHandle[0]);

		if (gameStatus.player2->actorStatus->vehicleInfo)
			StopChannel(&gameStatus.player2->actorStatus->vehicleInfo->soundHandle);
	}

	if (gameStatus.player1->actorStatus->multiplayer.position)
		pEndMPGameStats->setScreen(0);
	else
		pEndMPGameStats->setScreen(1);

	pEndMPGameStats->open();
}

bool	bonusGameWon;

POPUP	*bonusPopup;

void mpEndBonusSetUp(int *pCurrentMode, int returnMode, float result)
{
	PauseRumble(&controller1);
	PauseRumble(&controller2);

	FlushRumbleQueue(&controller1);
	FlushRumbleQueue(&controller2);

	UpdateRumble(&controller1);
	UpdateRumble(&controller2);

	COLOUR	statsBookColour=COLOUR(128, 128, 128);

	char	string[4], tempString[16];

	int		max, bonusResult, levelNum;

	switch (map.sceneNumber)
	{	
	case SCENE_MUSEUMVEHICLERACE:	levelNum = SCENE_MUSEUM;		break;
	case SCENE_GRANDCVEHICLERACE:	levelNum = SCENE_GRANDCANYON;	break;
	case SCENE_DEPTSTRVEHICLERACE:	levelNum = SCENE_DEPTSTR;		break;
	case SCENE_DESTRUCTIONAQUA:		levelNum = SCENE_AQUA;			break;
	case SCENE_DESTRUCTIONCONSTRUCT:levelNum = SCENE_CONSTRUCT;		break;
	case SCENE_DESTRUCTIONTASMANIA:	levelNum = SCENE_GHOSTTOWN;		break;
	case SCENE_ICEDOMERACE:			levelNum = SCENE_ICEDOME;		break;
	case SCENE_GOLDMINERACE:		levelNum = SCENE_GOLDMINE;		break;
	case SCENE_SAFARIRACE:			levelNum = SCENE_SAFARI;		break;
	}

	bonusResult = posterBounties[levelNum] * 2;

	pMode = pCurrentMode;

	pEndBonusBook = new BOOK;

	// PP: Set the size of the book
	// PP: REMOUT: books now default to filling the full 'safe' area of the screen		pEndGameBook->setBookRect(RECTANGLE(-0.5f, 0.5f, -0.5f, 0.5f));// PP: REMOUT: must now be expressed as fractions of the screen dimensions		RECTANGLE(-300, 300, -220, 220));

	// PP: 'result' textbox ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// PP: Create and add the textbox

	PAGE *pPage;
	TEXTBOX	*pTextBox;

	pEndBonusBook->setBookRect(RECTANGLE(-0.4f, 0.4f, -0.25f, 0.4f));

	pPage = pEndBonusBook->addPage("Result");

	switch(mpMode)
	{
	case MPMODE_RACE:		switch(map.sceneNumber)
							{
							case SCENE_MUSEUMVEHICLERACE:	max = 60;	break;
							case SCENE_GRANDCVEHICLERACE:	max = 60;	break;
							case SCENE_DEPTSTRVEHICLERACE:	max = 60;	break;
							}

							if (result == -1.0f)
							{
								pPage->insertItem(pTextBox = new TEXTBOX(STR_BAD_LUCK));
								bonusGameWon = false;
							}
							else
							{
								pPage->insertItem(pTextBox = new TEXTBOX(STR_WELL_DONE));
								bonusGameWon = true;
							}

							pTextBox->setScale(2.0f);

							pPage->insertItem(pTextBox = new TEXTBOX(STR_BONUS_TARGET));
							pPage->insertItem(pTextBox = new TEXTBOX(), false);
							pTextBox->sprintf8("%d.0s", max);

							pPage->insertItem(pTextBox = new TEXTBOX(STR_BONUS_TIME));
														
							if (result == -1.0f)
								pPage->insertItem(pTextBox = new TEXTBOX(STR_DNF), false);
							else
							{
								pPage->insertItem(pTextBox = new TEXTBOX(), false);
								pTextBox->sprintf8("%.1fs", result);
							}

							if (map.lastScene != SCENE_FRONTEND && !gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[levelNum].bonusgameOption && result != -1.0f)
							{
								pPage->insertItem(pTextBox = new TEXTBOX(STR_BONUS));
								pPage->insertItem(pTextBox = new TEXTBOX(), false);
							
								utilFormatPrettyNumbers(bonusResult, tempString);
								
								pTextBox->sprintf8("$%s", tempString);								
							}
		
							break;
	
	case MPMODE_DESTRUCT:	switch(map.sceneNumber)
							{
								case SCENE_DESTRUCTIONCONSTRUCT:	max = 40;	break;
								case SCENE_DESTRUCTIONAQUA:			max = 50;	break;
								case SCENE_DESTRUCTIONTASMANIA:		max = 45;	break;
							}

							if (result < max)
							{
								pPage->insertItem(pTextBox = new TEXTBOX(STR_BAD_LUCK));
								bonusGameWon = false;
							}
							else
							{
								pPage->insertItem(pTextBox = new TEXTBOX(STR_WELL_DONE));
								bonusGameWon = true;
							}

							pTextBox->setScale(2.0f);

							pPage->insertItem(pTextBox = new TEXTBOX(STR_BONUS_GOAL));
							pPage->insertItem(pTextBox = new TEXTBOX(), false);
							pTextBox->sprintf8("%d", max);

							pPage->insertItem(pTextBox = new TEXTBOX(STR_BONUS_SCORE));
							pPage->insertItem(pTextBox = new TEXTBOX(), false);
							pTextBox->sprintf8("%d", (int) result);

							if (map.lastScene != SCENE_FRONTEND && !gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[levelNum].bonusgameOption && result >= max)
							{
								pPage->insertItem(pTextBox = new TEXTBOX(STR_BONUS));
								pPage->insertItem(pTextBox = new TEXTBOX(), false);
							
								utilFormatPrettyNumbers(bonusResult, tempString);
								
								pTextBox->sprintf8("$%s", tempString);
							}
		
							break;

	case MPMODE_TIMEATTACK:	switch(map.sceneNumber)
							{
								case SCENE_ICEDOMERACE:		max = 7;	break;
								case SCENE_GOLDMINERACE:	max = 13;	break;
								case SCENE_SAFARIRACE:		max = 9;	break;
							}
		
		
							if (result < max)
							{
								pPage->insertItem(pTextBox = new TEXTBOX(STR_BAD_LUCK));
								bonusGameWon = false;
							}
							else
							{
								pPage->insertItem(pTextBox = new TEXTBOX(STR_WELL_DONE));
								bonusGameWon = true;
							}

							pTextBox->setScale(2.0f);

							sprintf(string, "%d ", max);
							pPage->insertItem(pTextBox = new TEXTBOX(STR_BONUS_GOAL));
							pPage->insertItem(pTextBox = new TEXTBOX(), false);							
							pTextBox->setText(string, STR_MP_CHECKPOINTS);

							sprintf(string, "%d ", (int) result);
							pPage->insertItem(pTextBox = new TEXTBOX(STR_BONUS_SCORE));
							pPage->insertItem(pTextBox = new TEXTBOX(), false);
							pTextBox->setText(string, STR_MP_CHECKPOINTS);

							if (map.lastScene != SCENE_FRONTEND && !gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[levelNum].bonusgameOption && result >= max)
							{
								pPage->insertItem(pTextBox = new TEXTBOX(STR_BONUS));
								pPage->insertItem(pTextBox = new TEXTBOX(), false);
							
								switch (map.sceneNumber)
								{
								case SCENE_ICEDOMERACE:		utilFormatPrettyNumbers(posterBounties[SCENE_ICEDOME] * 2, tempString);		break;
								case SCENE_GOLDMINERACE:	utilFormatPrettyNumbers(posterBounties[SCENE_GOLDMINE] * 2, tempString);	break;
								case SCENE_SAFARIRACE:		utilFormatPrettyNumbers(posterBounties[SCENE_SAFARI] * 2, tempString);		break;
								}

								utilFormatPrettyNumbers(bonusResult, tempString);
								
								pTextBox->sprintf8("$%s", tempString);							
							}

		
							break;
	}

	//pPage->insertItem(pTextBox = new TEXTBOX(STR_PRESS_X_OK_Y_RETRY));

	if (bonusGameWon && map.lastScene != SCENE_FRONTEND)
	{
		if (!gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[levelNum].bonusgameOption)
		{
			PlaySample("fanfare.wav", 255);

			gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[levelNum].bonusgameOption = true;

			gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty += bonusResult;

			if (processGalleryScore())
			{
				pPage->insertItem(pTextBox = new TEXTBOX(STR_GALLERY_UNLOCKED));

				pTextBox->setWrap(true);
				pTextBox->setScale(1.2f);
			}
		}
	}

	StopChannel(&characterSounds.taz);
	StopChannel(&characterSounds.sheDevil);

	StopChannel(&gameStatus.player1->actorStatus->soundHandle[0]);

	if (gameStatus.player1->actorStatus->vehicleInfo)
		StopChannel(&gameStatus.player1->actorStatus->vehicleInfo->soundHandle);
	
	if (!gameStatus.multiplayer.versusCPU)
	{	
		StopChannel(&gameStatus.player2->actorStatus->soundHandle[0]);

		if (gameStatus.player2->actorStatus->vehicleInfo)
			StopChannel(&gameStatus.player2->actorStatus->vehicleInfo->soundHandle);
	}

	bonusPopup = new POPUP(POPUP_RETRYLEVEL, (bonusGameWon ? 0 : 1));

	pEndBonusBook->open();

	allowedToProceed = gameStatus.gameTime + 2.0f;
}


/*	--------------------------------------------------------------------------------
	Function 	: mpEndGameMenuHandler
	Purpose 	: custom pause update function for the multiplayer end-of-level menus
	Parameters 	: context value, required but currently not used
	Returns 	: 
	Info 		: // PP: must match the PauseUpdateFunc typedef (main.h)
*/
void mpEndBonusHandler(const int32 context)
{
	if (!pEndBonusBook)
		return;

	if (allowedToProceed < gameStatus.gameTime)
	{
		bonusPopup->getPage("POPUPPAGE")->setNavigation(true);

		// PP: get the player's response to the popup
		int	retVal = bonusPopup->getResponse();

		if(retVal > 0)
		{
			// PP: Force-debounce ok button(s)
			FORCE_OKBUTTON_DEBOUNCE;
			
			BookList_closeAndFree(bonusPopup);
			
			*pMode = retMode;

			SetGameState(&gameStatus, GAMESTATE_INGAME, GAMESUBSTATE_NORMAL);
			
			switch(retVal)
			{
			case 2:	*pMode = retMode;
					SetGameState(&gameStatus, GAMESTATE_INGAME, GAMESUBSTATE_NORMAL);
					
					if (map.lastScene==SCENE_FRONTEND)
					{
						bkGenerateEvent("scenechange","_frontend");
					}
					else
					{
						switch(map.sceneNumber)
						{
						case SCENE_ICEDOMERACE:
						case SCENE_SAFARIRACE:
						case SCENE_DESTRUCTIONAQUA:
							if (bonusGameWon)
							{
								LevelCompleted(gameStatus.player1,LEVELCOMPLETE_NOSTATS);
							}
							else
							{
								bkGenerateEvent("scenechange","_zoohub");
							}
							break;
							
						case SCENE_MUSEUMVEHICLERACE:
						case SCENE_DEPTSTRVEHICLERACE:
						case SCENE_DESTRUCTIONCONSTRUCT:
							if (bonusGameWon)
							{
								LevelCompleted(gameStatus.player1,LEVELCOMPLETE_NOSTATS);
							}
							else
							{
								bkGenerateEvent("scenechange","_cityhub");
							}
							break;
							
						case SCENE_GRANDCVEHICLERACE:
						case SCENE_DESTRUCTIONTASMANIA:
						case SCENE_GOLDMINERACE:
							if (bonusGameWon)
							{
								LevelCompleted(gameStatus.player1,LEVELCOMPLETE_NOSTATS);
							}
							else
							{
								bkGenerateEvent("scenechange","_westhub");
							}
							break;
						}
					}
					
					break;
				
			case 1:	*pMode = retMode;
					SetGameState(&gameStatus, GAMESTATE_INGAME, GAMESUBSTATE_NORMAL);
					
					bkGenerateEvent("scenechange","_reset");
					
					break;
			}
			
			mpEndBonusKill();
			
			return;
		}
	}
}

void mpEndBonusKill(void)
{
	if (pEndBonusBook)
	{
		pEndBonusBook->close();
		delete(pEndBonusBook);
 		pEndBonusBook = NULL;
		
		camera[0].fade.StartFadeDown(FADETYPE_NORMAL, 1.5f);
	}
}

PAPERBOOK	*RuleBook;


/*	--------------------------------------------------------------------------------
	Function 	: RULEBOOKOVERLAY_INFO::menuActionFunc
	Purpose 	: callback that responds to menu actions on the rule book overlay
	Parameters 	: ref to a MENUACTION_INFO structure (TextFX.h)
	Returns 	: 
	Info 		: // PP: Must match the MenuActionFunc typedef (TextFX.h)
*/
void RULEBOOKOVERLAY_INFO::menuActionFunc(MENUACTION_INFO& info)
{
	bool	choosePads;// PP: are the players currently able to choose their pads?

#ifdef ALLOW_CONTROLLERS_IN_ANY_PORTS
	choosePads=(ruleBookOverlayInfo.choosePads);
#else// PP: ifndef ALLOW_CONTROLLERS_IN_ANY_PORTS
	choosePads=false;
#endif// PP: ndef ALLOW_CONTROLLERS_IN_ANY_PORTS

	if(info.action == MENUACTION_START)
	{
		if(!choosePads)
		{
			// PP: *** can't choose pads right now ***

			// PP: *** PS2 & PC ALWAYS GO IN HERE ***

			if(info.player == 0)// PP: player 1
			{
				ruleBookOverlayInfo.player1Ready=true;
					
				// PP: close player 1's prompt
				ruleBookOverlayInfo.subBooks[0]->close();

#ifdef ALLOW_CONTROLLERS_IN_ANY_PORTS
				// PP: prompts open one at a time
				// PP: open player 2's prompt
				ruleBookOverlayInfo.subBooks[1]->open();
#endif// PP: def ALLOW_CONTROLLERS_IN_ANY_PORTS
			}
			else// PP: player 2 (index 1)
			{
				ruleBookOverlayInfo.player2Ready=true;

				// PP: close player 1's prompt
				ruleBookOverlayInfo.subBooks[1]->close();
			}

			if((ruleBookOverlayInfo.player1Ready && gameStatus.multiplayer.versusCPU)		// PP: if player 1 is ready and there's only one player
				|| (ruleBookOverlayInfo.player1Ready && ruleBookOverlayInfo.player2Ready))	// PP: or if both players are ready
			{
				// PP: leave the rules state; close rule books
				CloseRuleBook();

				padCheckMode=PADCHECK_NORMAL;// PP: just in case
			}
		}
		else
		{
			// PP: *** XBOX SOMETIMES GOES IN HERE ***

			if(padCheckMode==PADCHECK_WAITING_FOR_PLAYER1)// PP: player 1
			{
				// PP: bind controller 1 to chosen port
				ClearInputBindings(&controller1);
				BindInputChannelsToPort(&controller1, info.port);

				if(!gameStatus.multiplayer.versusCPU)
				{
					padCheckMode=PADCHECK_WAITING_FOR_PLAYER2;

					// PP: player 1 has chosen
					// PP: close player 1's prompt
					ruleBookOverlayInfo.subBooks[0]->close();

					// PP: open player 2's prompt
					ruleBookOverlayInfo.subBooks[1]->open();

					// PP: must re-bind remaining controllers to remaining ports. otherwise player 2 would not be able to use the leftmost port
					bindControllersToDefaultPorts();
				}
				else
				{
					// PP: leave the rules state; close rule books
					CloseRuleBook();

					padCheckMode=PADCHECK_NORMAL;	
				}
			}
			else// PP: padCheckMode=PADCHECK_WAITING_FOR_PLAYER2
			{
				// PP: player 1 and player 2 can't use the same pad.
				// PP: That'd be 'Omni-Play™'!
				if(info.port != controller1.port)
				{
	#ifdef ALLOW_CONTROLLERS_IN_ANY_PORTS
					
					// PP: bind controller 2 to chosen port
					ClearInputBindings(&controller2);
					BindInputChannelsToPort(&controller2, info.port);

	#endif// PP: ndef ALLOW_CONTROLLERS_IN_ANY_PORTS
					padCheckMode=PADCHECK_NORMAL;

					// PP: player 1 has chosen, now player 2 has also chosen.
					// PP: IT BEGINS.™
					CloseRuleBook();
				}
			}


		}// PP: end if(choosePads)

#if(BPLATFORM==PC)
	biForceDebounce(controller1.crossDebounceChannel);
	biForceDebounce(controller2.crossDebounceChannel);
#else// PP: !pc
	biForceDebounce(controller1.startDebounceChannel);
	biForceDebounce(controller2.startDebounceChannel);
#endif

	}// PP: end if(info.action == MENUACTION_START)
}


/*	--------------------------------------------------------------------------------
	Function 	: RULEBOOKOVERLAY_INFO::twoPlayer_pageOpenFunc
	Purpose 	: page-open func for 2-player page of the rule book overlay
	Parameters 	: ptr to the page, context value
	Returns 	: 
	Info 		: // PP: Must match the PageFunc typedef (TextFX.h)
*/
void RULEBOOKOVERLAY_INFO::twoPlayer_pageOpenFunc(struct TAG_PAGE* const page, const int32 context)
{
	// PP: open the player 1 subbook
	ruleBookOverlayInfo.subBooks[0]->open();

	ruleBookOverlayInfo.subBooks[1]->book()\

#ifdef ALLOW_CONTROLLERS_IN_ANY_PORTS

	// PP: on Xbox, we only show one prompt at a time...
		->close(0.0f, true);

#else// PP: ifndef ALLOW_CONTROLLERS_IN_ANY_PORTS

	// PP: on PS2, we show both prompts at once...
		->open();

#endif// PP: ndef ALLOW_CONTROLLERS_IN_ANY_PORTS

	ruleBookOverlayInfo.player1Ready=false;
	ruleBookOverlayInfo.player2Ready=false;
}


/*	--------------------------------------------------------------------------------
	Function 	: ruleBookCloseCallback
	Purpose 	: receives notification that the paper rule-book has closed
	Parameters 	: ptr to paperbook that has closed
	Returns 	: 
	Info 		: // PP: must match the PaperBookCloseCallback typedef (PaperBook.h)
*/
void ruleBookCloseCallback(PAPERBOOK* const paperBook)
{
	// PP: return to in-game state
	SetGameState(&gameStatus, GAMESTATE_INTRO, GAMESUBSTATE_NORMAL);

// PP: ~~~~~~~~~~~~~~~~~

	gameStatus.introState = INTRO_LOADSFX;

	// NH: If palyer didn't exist (quiting near start), this would crash
	if (gameStatus.player1)
		gameStatus.player1->actorStatus->tutorialBookFlag = 0;

	if (!gameStatus.multiplayer.versusCPU)
	{
		setNumScreens(2);

		//InitCamera(
	}
	else
		setNumScreens(1);
}


/*	--------------------------------------------------------------------------------
	Function 	: MP_RuleBook_pageOpenFunc
	Purpose 	: page-open func for Rule Book multiplayer pages
	Parameters 	: ptr to the page, context value
	Returns 	: 
	Info 		: // PP: Must match the PageFunc typedef (TextFX.h)
*/
void MP_RuleBook_pageOpenFunc(struct TAG_PAGE* const page, const int32 context)
{
#ifndef CONSUMERDEMO
	// PP: the zero-based index (0/1/2) of the tournament stage that we're currently on (if gameStatus.multiplayer.tournament is also true)
	extern char		g_tournamentStage;// PP: tournament.cpp

	// PP: note: this value ain't used ifndef ALLOW_CONTROLLERS_IN_ANY_PORTS
	ruleBookOverlayInfo.choosePads= ((!gameStatus.multiplayer.Tournament)		// PP: if we're not doing a tournament or the tournament is on the first stage
									||(g_tournamentStage<=0));				
#endif

	if(ruleBookOverlayInfo.choosePads)
	{
		// PP: ...let the players choose their pads...

		padCheckMode=PADCHECK_WAITING_FOR_PLAYER1;

		// PP: if we didn't do this, the same start signal that brought us into the book would instantly bring us out again (if it were held for longer than a frame)
		FORCE_START_DEBOUNCE_ALL_PADS;

		// PP: bind controller1..4 to ports 1..4
		bindControllersToDefaultPorts();
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: MP_RuleBook_pageCloseFunc
	Purpose 	: page-close func for Rule Book multiplayer pages
	Parameters 	: ptr to the page, context value
	Returns 	: 
	Info 		: // PP: Must match the PageFunc typedef (TextFX.h)
*/
void MP_RuleBook_pageCloseFunc(struct TAG_PAGE* const page, const int32 context)
{
#ifndef CONSUMERDEMO
	// PP: BODGE: all the icons in the paperbook that point to textures in the level packages
	// PP:			must now have their OOD texture pointers cleared before the level can be re-initialised.
	for(int i=MP_NUM_LEVELPAK_ICONS-1; i>=0; i--)
	{
		mpLevelPakIcons[i]->tex=NULL;
	}
#endif
}


void InsertRulebookChapterText(struct TAG_PAPERBOOK *Book, int sceneRules, int rulesHeading, char *title, char *picture, int extraRules)
{
	PAGE			*page;
					
	TEXTBOX			*textPtr;
					
	ICON			*iconPtr;
					
	char			catStringTitle[256];// PP: was 40!  nowhere near big enough.

	static uchar	levelPakIconIndex=0;		// PP: BODGE: index used while loading the mpLevelPakIcons array

	strcpy(catStringTitle, "Rule Book - ");
	strcat(catStringTitle, title);
	
	page = Book->addPage(catStringTitle);

	// PP: set the page-open callback
	page->openFunc=MP_RuleBook_pageOpenFunc;

	// PP: set the page-close callback
	page->closeFunc=MP_RuleBook_pageCloseFunc;
	
	page->insertItem(textPtr = new TEXTBOX((EStringIndex)rulesHeading));
	textPtr->setWrap(true);// PP: needed for other languages
	textPtr->setStyle(TS_paperBookHeading);
	textPtr->setYPadding(false);
	textPtr->setXAlign(PIFLAG_XALIGN_CENTRE);
	textPtr->setYAlign(PIFLAG_YALIGN_TOP);
	
	page->insertItem(textPtr = new TEXTBOX((EStringIndex)sceneRules));
	textPtr->setColour(TUTORIALBOOK_TEXTCOLOUR_BODY);
	textPtr->setWrap(true);
	textPtr->setStyle(TS_paperBookBody);
	textPtr->setXAlign(PIFLAG_XALIGN_CENTRE);
	textPtr->setYAlign(PIFLAG_YALIGN_CENTRE);

	if (extraRules!=-1)
	{
		textPtr->setYAlign(PIFLAG_YALIGN_BOTTOM);

		page->insertItem(textPtr = new TEXTBOX((EStringIndex)extraRules));
		textPtr->setVGrad(false);
		textPtr->setWrap(true);
		textPtr->setStyle(TS_paperBookBody);
		textPtr->setXAlign(PIFLAG_XALIGN_CENTRE);

		textPtr->setYAlign(PIFLAG_YALIGN_TOP);
	}

/* PP: REMOUT: OOD, now in overlay book		page->insertItem(STR_PRESS_START_TO_CONTINUE)
		->setWrap(true)
		->setStyle(TS_paperBookBody)
		->setYPadding(false);*/
	
	page = Book->addPage();

	page->insertItem(iconPtr = new ICON());

	// PP: BODGE: add the icon to the array of level-pak icons (see mpLevelPakIcons)
	mpLevelPakIcons[levelPakIconIndex++]=iconPtr;

	strcpy(catStringTitle, "extras\\");
	strcat(catStringTitle, picture);

	iconPtr->setTexture(catStringTitle);
	iconPtr->setScale(3.5f);
	iconPtr->tilt = false;
	iconPtr->setXAlign(PIFLAG_XALIGN_CENTRE);
	iconPtr->setYAlign(PIFLAG_YALIGN_CENTRE);
}


void InitRuleBook()
{
#ifndef CONSUMERDEMO

	PAGE		*page;
	PAGE		*subPage;
	BOOK		*book;
	BOOK		*subBook;
	TEXTBOX		*textPtr;

	RuleBook = new PAPERBOOK("BOSS RULE BOOK");

	RuleBook->setBackground("bonus.bmp");

	/* PP: *** NOTE ****
	
	I reckon this paper book uses about 43kb just for its paper info.
	I'm going to change the paper books so they only store paper info for *visible* pages.

	*******************/

	// PP: specify the button(s) that can be used to close the paper book
	// PP: REMOUT: RuleBook->setCloseButton(PBCLOSEBUTTON_FORWARD);// PP: forward, because we're *starting* gameplay
	RuleBook->setCloseButton(PBCLOSEBUTTON_NONE);

	// PP: LAZY BODGE (but there are after all only 2 paperbooks in the whole game)
	// PP: Add 2 blank pages at the beginning of the book.
	// PP: Otherwise, you won't see a left-hand shadow when opening the first chapter.
	RuleBook->addPage("blank1");
	RuleBook->addPage("blank2");

	// PP: Macro to add a chapter's text to a tutorial
	// NH: Whoa! That was a big macro! Screws up the GameCube compiler as it's only got a small buffer allocated for each function, so I've had to change it to a function (InsertRulebookChapterText)
	// PP: Oops, yes, I didn't consider how many pages there were going to be!

	{
		RuleBook->startChapter("Zoo Boss");
		InsertRulebookChapterText(RuleBook, STR_TP_MP_SAFARIBOSS_INTRO_1, STR_LEVELNAME_ZOOBOSS, "Zoo Boss", "elephantpong.bmp", STR_TP_MP_SAFARIBOSS_INTRO_2);

		PAGE*	tempPage;

		tempPage=RuleBook->getLastPage(1);

		// PP: insert the correct max score into the rules text// PP: not a terribly clean way of doing it, calling setText again
		tempPage->getTextbox(STR_TP_MP_SAFARIBOSS_INTRO_2)->setText(STR_TP_MP_SAFARIBOSS_INTRO_2);

		// PP: set a callback on the zooboss page opening, that'll give us a chance to close the "Pachydermus Pongus" overlay
		tempPage->openFunc=ZOOBOSSGAME::ruleBookOpenFunc;
	}
	

	RuleBook->startChapter("City Boss");
//	InsertRulebookChapterText(RuleBook, STR_BOSSRULES_CITY_BOSS, STR_LEVELNAME_CITYBOSS, "City Boss");
	InsertRulebookChapterText(RuleBook, STR_BOSSRULES_CITY_BOSS_1, STR_LEVELNAME_CITYBOSS, "City Boss", "gladiatoons.bmp", STR_BOSSRULES_CITY_BOSS_2);

	RuleBook->startChapter("West Boss");
//	InsertRulebookChapterText(RuleBook, STR_BOSSRULES_WEST_BOSS, STR_LEVELNAME_WESTBOSS, "West Boss");
	InsertRulebookChapterText(RuleBook, STR_BOSSRULES_WEST_BOSS_1, STR_LEVELNAME_WESTBOSS, "West Boss", "dodgecity.bmp", STR_BOSSRULES_WEST_BOSS_2);

	RuleBook->startChapter("Taz Boss 1");
	InsertRulebookChapterText(RuleBook, STR_BOSSRULES_TAZ_PREBOSS, STR_LEVELNAME_TAZBOSS1, "Taz Boss 1", "discoinferno.bmp");
	
	RuleBook->startChapter("Taz Boss 2");
//	InsertRulebookChapterText(RuleBook, STR_BOSSRULES_TAZ_BOSS, STR_LEVELNAME_TAZBOSS2, "Taz Boss 2");
	InsertRulebookChapterText(RuleBook, STR_BOSSRULES_TAZ_BOSS_1, STR_LEVELNAME_TAZBOSS2, "Taz Boss 2", "tweetyboss.bmp", STR_BOSSRULES_TAZ_BOSS_2);

	RuleBook->startChapter("Multiplayer Museum Race");
#if BPLATFORM == PC
	InsertRulebookChapterText(RuleBook, STR_PC_TP_MP_MUSEUM_INTRO, STR_LEVELNAME_MUSEUM, "Multiplayer Museum Race", "vehicle - museum.bmp");
#elif BPLATFORM == XBOX
	InsertRulebookChapterText(RuleBook, STR_XBOX_TP_MP_MUSEUM_INTRO, STR_LEVELNAME_MUSEUM, "Multiplayer Museum Race", "vehicle - museum.bmp");
#else
	InsertRulebookChapterText(RuleBook, STR_TP_MP_MUSEUM_INTRO, STR_LEVELNAME_MUSEUM, "Multiplayer Museum Race", "vehicle - museum.bmp");
#endif
	
	// PP: set the page-open func that'll set up controller bindings for ALL pads
	RuleBook->getLastPage(1)->openFunc=MP_RuleBook_pageOpenFunc;

	RuleBook->startChapter("Singleplayer Museum Race");
	InsertRulebookChapterText(RuleBook, STR_TP_BG_MUSEUM_INTRO, STR_LEVELNAME_MUSEUM, "Singleplayer Museum Race", "vehicle - museum.bmp");

	RuleBook->startChapter("Multiplayer Mall Race");	
#if BPLATFORM == PC
	InsertRulebookChapterText(RuleBook, STR_PC_TP_MP_DEPTSTR_INTRO, STR_LEVELNAME_DEPTSTR, "Multiplayer Mall Race", "vehicle - deptstore.bmp");
#elif BPLATFORM == XBOX
	InsertRulebookChapterText(RuleBook, STR_XBOX_TP_MP_DEPTSTR_INTRO, STR_LEVELNAME_DEPTSTR, "Multiplayer Mall Race", "vehicle - deptstore.bmp");
#else
	InsertRulebookChapterText(RuleBook, STR_TP_MP_DEPTSTR_INTRO, STR_LEVELNAME_DEPTSTR, "Multiplayer Mall Race", "vehicle - deptstore.bmp");
#endif

	RuleBook->startChapter("Singleplayer Mall Race");
	InsertRulebookChapterText(RuleBook, STR_TP_BG_DEPTSTR_INTRO, STR_LEVELNAME_DEPTSTR, "Singleplayer Mall Race", "vehicle - deptstore.bmp");
	
	RuleBook->startChapter("Multiplayer Canyon Race");
#if BPLATFORM == PC
	InsertRulebookChapterText(RuleBook, STR_PC_TP_MP_CANYON_INTRO, STR_LEVELNAME_GRANDC, "Multiplayer Canyon Race", "vehicle - grandcanyon2.bmp");
#elif BPLATFORM == XBOX
	InsertRulebookChapterText(RuleBook, STR_XBOX_TP_MP_CANYON_INTRO, STR_LEVELNAME_GRANDC, "Multiplayer Canyon Race", "vehicle - grandcanyon2.bmp");
#else
	InsertRulebookChapterText(RuleBook, STR_TP_MP_CANYON_INTRO, STR_LEVELNAME_GRANDC, "Multiplayer Canyon Race", "vehicle - grandcanyon2.bmp");
#endif

	RuleBook->startChapter("Singleplayer Canyon Race");
	InsertRulebookChapterText(RuleBook, STR_TP_BG_CANYON_INTRO, STR_LEVELNAME_GRANDC, "Singleplayer Canyon Race", "vehicle - grandcanyon2.bmp");

	RuleBook->startChapter("Multiplayer Ice Dome Time Attack");
	InsertRulebookChapterText(RuleBook, STR_TP_MP_ICEDOME_INTRO, STR_LEVELNAME_ICEDOME, "Multiplayer Ice Dome Time Attack", "race - icedome.bmp");

	RuleBook->startChapter("Singleplayer Ice Dome Time Attack");
	InsertRulebookChapterText(RuleBook, STR_TP_BG_ICEDOME_INTRO, STR_LEVELNAME_ICEDOME, "Singleplayer Ice Dome Time Attack", "race - icedome.bmp");

	RuleBook->startChapter("Multiplayer Gold Mine Time Attack");
	InsertRulebookChapterText(RuleBook, STR_TP_MP_GOLDMINE_INTRO, STR_LEVELNAME_GOLDMINE, "Multiplayer Gold Mine Time Attack", "race - goldmine.bmp");

	RuleBook->startChapter("Singleplayer Gold Mine Time Attack");
	InsertRulebookChapterText(RuleBook, STR_TP_BG_GOLDMINE_INTRO, STR_LEVELNAME_GOLDMINE, "Singleplayer Gold Mine Time Attack", "race - goldmine.bmp");
	
	RuleBook->startChapter("Multiplayer Safari Time Attack");
	InsertRulebookChapterText(RuleBook, STR_TP_MP_SAFARI_INTRO, STR_LEVELNAME_SAFARI, "Multiplayer Safari Time Attack", "race - safari.bmp");

	RuleBook->startChapter("Singleplayer Safari Time Attack");
	InsertRulebookChapterText(RuleBook, STR_TP_BG_SAFARI_INTRO, STR_LEVELNAME_SAFARI, "Singleplayer Safari Time Attack", "race - safari.bmp");

	RuleBook->startChapter("Multiplayer Aqua Destruct");
	InsertRulebookChapterText(RuleBook, STR_TP_MP_AQUA_INTRO, STR_LEVELNAME_AQUA, "Multiplayer Aqua Destruct", "destruct - aqua.bmp");

	RuleBook->startChapter("Singleplayer Aqua Destruct");
	InsertRulebookChapterText(RuleBook, STR_TP_BG_AQUA_INTRO, STR_LEVELNAME_AQUA, "Singleplayer Aqua Destruct", "destruct - aqua.bmp");

	RuleBook->startChapter("Multiplayer Construction Destruct");
	InsertRulebookChapterText(RuleBook, STR_TP_MP_CONSTRUCT_INTRO, STR_LEVELNAME_CONSTRUCT, "Multiplayer Construction Destruct", "destruct - consite.bmp");

	RuleBook->startChapter("Singleplayer Construction Destruct");
	InsertRulebookChapterText(RuleBook, STR_TP_BG_CONSTRUCT_INTRO, STR_LEVELNAME_CONSTRUCT, "Singleplayer Construction Destruct", "destruct - consite.bmp");
	
	RuleBook->startChapter("Multiplayer Ghost Town Destruct");
	InsertRulebookChapterText(RuleBook, STR_TP_MP_GHOSTTOWN_INTRO, STR_LEVELNAME_GHOSTTOWN, "Multiplayer Ghost Town Destruct", "destruct - ghosttown.bmp");

	RuleBook->startChapter("Singleplayer Ghost Town Destruct");
	InsertRulebookChapterText(RuleBook, STR_TP_BG_GHOSTTOWN_INTRO, STR_LEVELNAME_GHOSTTOWN, "Singleplayer Ghost Town Destruct", "destruct - ghosttown.bmp");

	RuleBook->startChapter("Multiplayer Zoo Boss");
	InsertRulebookChapterText(RuleBook, STR_TP_MP_SAFARIBOSS_INTRO_1, STR_LEVELNAME_ZOOBOSS, "Multiplayer Zoo Boss", "elephantpong.bmp", STR_TP_MP_SAFARIBOSS_INTRO_2);
	// PP: insert the correct max score into the rules text// PP: not a terribly clean way of doing it, calling setText again
	RuleBook->getLastPage(1)->getTextbox(STR_TP_MP_SAFARIBOSS_INTRO_2)->setText(STR_XBOX_BOSSRULES_MP_ZOO_BOSS);

	RuleBook->startChapter("Multiplayer City Boss");
	InsertRulebookChapterText(RuleBook, STR_TP_MP_CITYBOSS_INTRO_1, STR_LEVELNAME_CITYBOSS, "Multiplayer City Boss", "gladiatoons.bmp", STR_TP_MP_CITYBOSS_INTRO_2);

	RuleBook->startChapter("Multiplayer West Boss");
	InsertRulebookChapterText(RuleBook, STR_TP_MP_WESTBOSS_INTRO, STR_LEVELNAME_WESTBOSS, "Multiplayer West Boss", "dodgecity.bmp");

	// PP: register our callback for when the paper-book closes
	RuleBook->setCloseCallback(ruleBookCloseCallback);

	// PP: CREATE RULE BOOK OVERLAY...	

	book=ruleBookOverlayInfo.book=new BOOK("RULE BOOK OVERLAY");

	// PP: 1-player page...

	page=book->addPage("1p");

	page->setMenuActionFunc(RULEBOOKOVERLAY_INFO::menuActionFunc);

#if(BPLATFORM==PC)
	page->insertItem(STR_PRESS_JUMP_TO_CONTINUE)
#elif(BPLATFORM==XBOX)
	page->insertItem(STR_XBOX_PRESS_START)
#else
	page->insertItem(STR_PRESS_START)
#endif
		->setStyle(TS_control)
		->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setEffect(&PIE_flash, PIEMODE_ON);

	// PP: 2-player page...

	page=book->addPage("2p");

	page->setMenuActionFunc(RULEBOOKOVERLAY_INFO::menuActionFunc);

	page->insertItem(ruleBookOverlayInfo.subBooks[0]=subBook=new BOOK)			// PP: left (player 1) subbook
		->setName("p1 subbook");

	subPage=subBook->addPage();

	subPage->setVSlack(false)									// PP: no Y slack
		->setYAlign(PIFLAG_YALIGN_BOTTOM);						// PP: bottom of screen

	subPage->insertItem(STR_PLAYER_1)
		->setStyle(TS_control)
		->setEffect(&PIE_flash, PIEMODE_ON);

#if(BPLATFORM==PC)
	subPage->insertItem(STR_PRESS_JUMP_TO_CONTINUE)
#elif(BPLATFORM==XBOX)
	subPage->insertItem(STR_XBOX_PRESS_START)
#else
	subPage->insertItem(STR_PRESS_START)
#endif
		->setStyle(TS_control)
		->setWrap(true)// PP: see french
		->setEffect(&PIE_flash, PIEMODE_ON);
		
	page->insertItem(ruleBookOverlayInfo.subBooks[1]=subBook=new BOOK, false)	// PP: right (player 2) subbook on same row
		->setName("p2 subbook");

	page->openFunc=RULEBOOKOVERLAY_INFO::twoPlayer_pageOpenFunc;

	subPage=subBook->addPage();

	subPage->setVSlack(false)									// PP: no Y slack
		->setYAlign(PIFLAG_YALIGN_BOTTOM);						// PP: bottom of screen

	subPage->insertItem(STR_PLAYER_2)							
		->setStyle(TS_control)
		->setEffect(&PIE_flash, PIEMODE_ON);

#if(BPLATFORM==PC)
		subPage->insertItem(STR_PRESS_JUMP_TO_CONTINUE)
#elif(BPLATFORM==XBOX)
		subPage->insertItem(STR_XBOX_PRESS_START)
#else
		subPage->insertItem(STR_PRESS_START)
#endif
		->setStyle(TS_control)
		->setWrap(true)// PP: see french
		->setEffect(&PIE_flash, PIEMODE_ON);

	// PP: ~~~~~~~~~

	//RuleBookHelp = new BOOK();

	//page = RuleBookHelp->addPage("help");

	//page->insertItem(textPtr = new TEXTBOX(STR_DIALOGUE_CROSSOK));

	//textPtr->setYAlign(PIFLAG_YALIGN_BOTTOM);
	//textPtr->setStyle(TEXTSTYLE_CONTROL);
#endif
}

void StartRuleBook()
{
#ifndef CONSUMERDEMO

	switch (map.sceneNumber)
	{
	case SCENE_ZOOBOSS:				if (gameStatus.multiplayer.versusCPU)
										RuleBook->restrictToChapter(0);	
									else
										RuleBook->restrictToChapter(23);	
									break;

	case SCENE_CITYBOSS:			if (gameStatus.multiplayer.versusCPU)
										RuleBook->restrictToChapter(1);	
									else
										RuleBook->restrictToChapter(24);	
									break;

	case SCENE_WESTBOSS:			if (gameStatus.multiplayer.versusCPU)
										RuleBook->restrictToChapter(2);	
									else
										RuleBook->restrictToChapter(25);	
									break;

	case SCENE_TAZPREBOSS:			RuleBook->restrictToChapter(3);	break;
	case SCENE_TAZBOSS:				RuleBook->restrictToChapter(4);	break;
	
	case SCENE_MUSEUMVEHICLERACE:	if (gameStatus.multiplayer.versusCPU)
										RuleBook->restrictToChapter(6);
									else
										RuleBook->restrictToChapter(5);

									break;

	case SCENE_DEPTSTRVEHICLERACE:	if (gameStatus.multiplayer.versusCPU)
										RuleBook->restrictToChapter(8);
									else
										RuleBook->restrictToChapter(7);

									break;

	case SCENE_GRANDCVEHICLERACE:	if (gameStatus.multiplayer.versusCPU)
										RuleBook->restrictToChapter(10);
									else
										RuleBook->restrictToChapter(9);

									break;

	case SCENE_ICEDOMERACE:			if (gameStatus.multiplayer.versusCPU)
										RuleBook->restrictToChapter(12);
									else
										RuleBook->restrictToChapter(11);

									break;

	case SCENE_GOLDMINERACE:		if (gameStatus.multiplayer.versusCPU)
										RuleBook->restrictToChapter(14);
									else
										RuleBook->restrictToChapter(13);

									break;

	case SCENE_SAFARIRACE:			if (gameStatus.multiplayer.versusCPU)
										RuleBook->restrictToChapter(16);
									else
										RuleBook->restrictToChapter(15);

									break;

	
	case SCENE_DESTRUCTIONAQUA:		if (gameStatus.multiplayer.versusCPU)
										RuleBook->restrictToChapter(18);
									else
										RuleBook->restrictToChapter(17);

									break;

	case SCENE_DESTRUCTIONCONSTRUCT:if (gameStatus.multiplayer.versusCPU)
										RuleBook->restrictToChapter(20);
									else
										RuleBook->restrictToChapter(19);

									break;

	case SCENE_DESTRUCTIONTASMANIA:	if (gameStatus.multiplayer.versusCPU)
										RuleBook->restrictToChapter(22);
									else
										RuleBook->restrictToChapter(21);

									break;
	}
	
	RuleBook->open();

	// PP: open the overlay book on the appropriate page
	if(gameStatus.multiplayer.versusCPU)
	{
		ruleBookOverlayInfo.book->open("1p");
	}
	else
	{
		ruleBookOverlayInfo.book->open("2p");
	}
#endif
}


void CloseRuleBook()
{
#ifdef CONSUMERDEMO
	return;
#endif

	RuleBook->close();
	ruleBookOverlayInfo.book->close();// PP: 

	// PP: MOVED IN FROM MAIN.CPP...

	extern float sfxTimer;	// PP: JW: Wait 1 second after change of introcam to load sfx

	sfxTimer = -1;

	gameStatus.introState = INTRO_LOADSFX;

	gameStatus.player1->actorStatus->tutorialBookFlag = 0;

	// PP: REMOUT: OOD		CloseRuleBook();

	if (!gameStatus.multiplayer.versusCPU)
		setNumScreens(2);
	else
		setNumScreens(1);
}

void KillRuleBook()
{
#ifdef CONSUMERDEMO
	return;
#endif
	
	SAFE_DELETE(RuleBook);
}


/*	--------------------------------------------------------------------------------
	Function 	: setNumPlayers
	Purpose 	: set the number of players
	Parameters 	: new number of players, (opt/true)allow players to choose their pads
	Returns 	: 
	Info 		: // PP: !!!!!!DON'T CALL THIS PER-FRAME!!!!!!
					// PP: prepares the pad-checking stuff for the new number of players
*/
void setNumPlayers(const uchar numPlayers, const bool choosePads)
{
	gameStatus.multiplayer.numPlayers = numPlayers;

	if(numPlayers == 2)
	{
		gameStatus.multiplayer.versusCPU = false;

		if(choosePads)
		{
			/* PP: must take pad-checking mode out of PADCHECK_NORMAL now, otherwise we might*/
			/* PP:	get a missing controller message having switched into 2 player*/
			padCheckMode=PADCHECK_ALL;
			bindControllersToDefaultPorts();
		}
	}
	else// PP: 1
	{
		gameStatus.multiplayer.versusCPU = true;

		// PP: shouldn't need to do any controller stuff; player 1 is already bound
	}
}