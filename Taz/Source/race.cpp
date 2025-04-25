// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Blitz Games Ltd.
//
//      File : race.cpp
//   Purpose : multiplayer race stuff
// Component : Taz
//    Author : Chris W
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "main.h"

#include "display.h"
#include "maths.h"
#include "actors.h"
#include "status.h"
#include "physics.h"
#include "camera.h"
#include "control.h"
#include "trail.h"
#include "floorpolisher.h"
#include "scene.h"
#include "vehicles.h"
#include "sfx.h"
#include "text2.h"
#include "textfx.h"
#include "countdown.h"
#include "util.h"
#include "animation.h"
#include "pause.h"
#include "PageItemFX.h"						// PP: Special effects for page items

#include "multiplayer.h"
#include "tournament.h"

// PP: duration for which multiplayer "time up" message stays on screen
#define MP_TIMEUP_DURATION		2.5f		// PP: was 1.0f

#define RACE_MAXCHECKPOINTS	(100)

#define TB_GLOBAL	(0)
#define TB_POSITION	(1)
#define TB_LAP		(2)
#define TB_TIMER	(3)
#define TB_BEST		(4)

#define GAMETYPE_RACE			(0)
#define GAMETYPE_TIMEATTACK		(1)

#define CHECKPOINT_ISTARGET		(1)

#define	CHECKPOINT_TIMER_ONE	(0)
#define	CHECKPOINT_TIMER_TWO	(1)

typedef struct tagCHECKPOINT
{
	uint32			flags;
	int32			pad[3];			// CPW: 4L

	TBVector        actualPos;		// CPW: 4L

	TBVector		modelPos;		// CPW: 8L

	ACTORINSTANCE	*pModel;
	float			yAngle;
	float			timeCredit;		// CPW: 4L

	TBVector		testPoints[4];	// CPW: 16L
}CHECKPOINT;

int RACE_NUM_LAPS;
static CHECKPOINT pCheckPoint[RACE_MAXCHECKPOINTS];
static TBTexture *pTexture = NULL;
static float size = METERTOUNIT(0.05f);
static int white[4] = {127, 127, 127, 127};
static TBVector qDown, vDown = {0.0f, 0.0f, 0.0f, 1.0f};
int	cpoint = 0;	

float	lastRate1, lastRate2;

float	oneChkTime, twoChkTime, angle, raceTime;

// CPW: the one and only target (if this level has one)
CHECKPOINT *pTarget = NULL;

/*  --------------------------------------------------------------------------------
	Function	: raceForceEnd
	Purpose		: Force the end of a race game
	Parameters	: win- true/false
	Returns		: 
	Info		: 
*/

void raceForceEnd(bool win)
{
	if((mpNumPlayers == 2) && gameStatus.multiplayer.numCheckPoints)
	{
		raceBook->close();
		levelData.gameFinished = GAMEOVER_COMPLETED;
		if (win)
		{
			mpPlayer[0]->actorStatus->multiplayer.position = 0;
			mpPlayer[1]->actorStatus->multiplayer.position = 1;
			levelData.winner = 0;
		}
		else
		{
			mpPlayer[1]->actorStatus->multiplayer.position = 0;
			mpPlayer[0]->actorStatus->multiplayer.position = 1;
			levelData.winner = 1;
		}
	}
	else
	{
		levelData.gameFinished = GAMEOVER_COMPLETED;
		
		raceBook->close();

		mpPlayer[0]->actorStatus->multiplayer.position = 0;
		if (win)
		{
			cpoint = 20;// TP: this should cover it
			mpPlayer[0]->actorStatus->multiplayer.lapTime[0] = 0;
		}
		else
		{
			cpoint = 0;// TP: this should cover it
			mpPlayer[0]->actorStatus->multiplayer.lapTime[0] = -1;
		}
	}
}

void raceAddTarget(TBVector pos, TBQuaternion rot)
{
	TBVector scale = {1.0f, 1.0f, 1.0f, 1.0f};
	float a;
	TBVector	rotVec;

	ASSERT(pos);
	ASSERT(rot);

	pTarget = new(CHECKPOINT);
	ASSERT(pTarget);

	memset(pTarget, 0, sizeof(CHECKPOINT));

	pTarget->flags = CHECKPOINT_ISTARGET;

	// PP: what a thoroughly confusing function!  doesn't return a rotation vector in the sense I'm used to at all.
	// PP: needless to say this had me questioning my sanity for a good five hours!  Ah, all better now
	bmQuatToRotation(rotVec, rot);
	a = rotVec[W]*rotVec[Y];
	// CPW: I don't understand the above line, I just trust Phil!

	pTarget->yAngle = a;

	bmVectorCopy(pTarget->actualPos, pos);

	pTarget->pModel = CreateActorInstance(&map.levelInstances, "target.obe", "target");
	ASSERT(pTarget->pModel);

	pTarget->pModel->flags |= ACTORFLAG_DONTDRAW;

	bmVectorCopy(pTarget->modelPos, pos);
	bmVectorCopy(pTarget->pModel->actorInstance.position, pos);
	bmVectorCopy(pTarget->pModel->actorInstance.scale, scale);
}

void raceAddCheckPoint(TBVector pos, TBQuaternion rot, int seq, float widdywidth)
{
	int i;
	float sin, cos;
	TBVector scale = {1.0f, 1.0f, 1.0f, 1.0f};
	float a;
	TBVector	rotVec;

	ASSERT(pos);
	ASSERT(rot);
	ASSERT(widdywidth);
	ASSERT(seq >= 0);

	memset(pCheckPoint + seq, 0, sizeof(CHECKPOINT));

	// CPW:	pCheckPoint[seq].flags = CHECKPOINT_ISTARGET;

	// PP: what a thoroughly confusing function!  doesn't return a rotation vector in the sense I'm used to at all.
	// PP: needless to say this had me questioning my sanity for a good five hours!  Ah, all better now
	bmQuatToRotation(rotVec, rot);
	a = rotVec[W]*rotVec[Y];

// CPW: 	sin = bmSin(a) * METERTOUNIT(1.0f);
// CPW: 	cos = bmCos(a) * METERTOUNIT(1.0f);

	sin = bmSin(a) * widdywidth;
	cos = bmCos(a) * widdywidth;

	pCheckPoint[seq].yAngle = a;

	bmVectorCopy(pCheckPoint[seq].actualPos, pos);

	for(i=0; i<4; i++)
	{	
		bmVectorCopy(pCheckPoint[seq].testPoints[i], pos);
		pCheckPoint[seq].testPoints[i][W] = 1.0f;
	}

	pCheckPoint[seq].testPoints[0][X] += cos;
	pCheckPoint[seq].testPoints[0][Z] += sin;
	pCheckPoint[seq].testPoints[0][Y] += METERTOUNIT(2);

	pCheckPoint[seq].testPoints[1][X] += cos;
	pCheckPoint[seq].testPoints[1][Z] += sin;
	// CPW: pCheckPoint[numCheckPoints].testPoints[1][Y] += METERTOUNIT(-1);

	pCheckPoint[seq].testPoints[2][X] -= cos;
	pCheckPoint[seq].testPoints[2][Z] -= sin;
	pCheckPoint[seq].testPoints[2][Y] += METERTOUNIT(2);

	pCheckPoint[seq].testPoints[3][X] -= cos;
	pCheckPoint[seq].testPoints[3][Z] -= sin;
	// CPW: pCheckPoint[numCheckPoints].testPoints[3][Y] += METERTOUNIT(-1);

	if (seq)
	{
		pCheckPoint[seq].pModel = CreateActorInstance(&map.animatedInstances, "checkpoint.obe", "checkpoint");
		ASSERTM(pCheckPoint[seq].pModel, "Can't find checkpoint.obe");
	}
	else
	{
		pCheckPoint[seq].pModel = CreateActorInstance(&map.animatedInstances, "startpoint.obe", "checkpoint");
		ASSERTM(pCheckPoint[seq].pModel, "Can't find startpoint.obe");
	}

	PlayAnimationByName(pCheckPoint[seq].pModel, "checkpoint idle",  1.0f, 1, 0, 0.0f, NONE);
	
	bmVectorCopy(pCheckPoint[seq].pModel->actorInstance.position, pos);
	bmVectorCopy(pCheckPoint[seq].pModel->actorInstance.scale, scale);
	bmQuatCopy(pCheckPoint[seq].pModel->actorInstance.orientation, rot);

	if((seq + 1) >= gameStatus.multiplayer.numCheckPoints)
		gameStatus.multiplayer.numCheckPoints = (seq + 1);

	if (map.sceneNumber == SCENE_ICEDOMERACE)
		pCheckPoint[seq].timeCredit = 6.0f;
	else
		pCheckPoint[seq].timeCredit = 5.0f;
}

int raceHandler(int reason)
{
	int				player;
	ACTORINSTANCE	*pActorInstance;
	TEXTBOX			*pTextBox;				// PP: general textbox pointer - saves some typing
	PAGE			*pPage;
	ICON			*pIcon;
	TBSavedRenderStates savedState;

/*	if(levelData.gameFinished != GAMEOVER_NOTFINISHED)
		reason = MPREASON_MENUMODE:
*/

	switch(reason)
	{
	case MPREASON_INITIALISE:
	{	
		ACTORINSTANCE	*tempVehicle1 = 0, *tempVehicle2 = 0;


		// PP: TEMP TEST!!!!!!!!
		bmVectorZero(gameStatus.player1->actorBehaviour->physics.velocity);

		
		memset(&levelData, 0, sizeof(levelData));
	
		cpoint = 0;

		levelData.gameFinished = GAMEOVER_NOTFINISHED;
		levelData.winner = -1;
		levelData.timeCredit = 0;

		lastRate1 = lastRate2 = 1.0f;

		switch(mpSubMode)// CPW: what level is it?
		{
		case SCENE_MUSEUMVEHICLERACE:
		case SCENE_GRANDCVEHICLERACE:
		case SCENE_DEPTSTRVEHICLERACE:
			levelData.gameType = GAMETYPE_RACE;
			levelData.needVehicle = true;

			if (map.sceneNumber == SCENE_DEPTSTRVEHICLERACE)
			{
				tempVehicle1 = FindActorInstanceInMap("trolley01", 0, &map);
				tempVehicle2 = FindActorInstanceInMap("trolley02", 0, &map);
			}
			else if (map.sceneNumber == SCENE_MUSEUMVEHICLERACE)
			{
				tempVehicle1 = FindActorInstanceInMap("floorpolisher01", 0, &map);
				tempVehicle2 = FindActorInstanceInMap("floorpolisher02", 0, &map);
			}
			else
			{
				tempVehicle1 = FindActorInstanceInMap("motion01", 0, &map);
				tempVehicle2 = FindActorInstanceInMap("motion02", 0, &map);
			}

			break;

		case SCENE_ICEDOMERACE:
		case SCENE_GOLDMINERACE:
		case SCENE_SAFARIRACE:
			levelData.gameType = GAMETYPE_TIMEATTACK;
			levelData.timeLimit = 60.0f;
			levelData.needVehicle = false;
			break;

		default:
			ASSERT(0);
		}

		if (gameStatus.multiplayer.numPlayers == 1 && SCENE_DEPTSTRVEHICLERACE)
			PlayAnimationByName(gameStatus.player1,"rapperghettoblast",2.0f,0,0,0.2f,ATTACK);

		ASSERT(mpNumPlayers);

#ifdef CONSUMERDEMO
		levelData.timeLimit = 240.0f;
		RACE_NUM_LAPS = 3;
#else
		
		if (mpNumPlayers == 2)
		{
			if (map.sceneNumber == SCENE_SAFARIRACE)
				RACE_NUM_LAPS = 4;
			else
				RACE_NUM_LAPS = 3;
		}
		else
		{			
			TBVector	vect;

			if (levelData.gameType == GAMETYPE_RACE)
			{
				if (tempVehicle1)
				{
					SETVECTOR(vect, (tempVehicle1->actorInstance.position[X] + tempVehicle2->actorInstance.position[X]) / 2,
									(tempVehicle1->actorInstance.position[Y] + tempVehicle2->actorInstance.position[Y]) / 2,
									(tempVehicle1->actorInstance.position[Z] + tempVehicle2->actorInstance.position[Z]) / 2, 1.0f);

					FreeActorInstance(tempVehicle2);

					tempVehicle2 = 0;

					bmVectorCopy(tempVehicle1->actorInstance.position, vect);
				}
			}

			if (map.sceneNumber == SCENE_MUSEUMVEHICLERACE || map.sceneNumber == SCENE_DEPTSTRVEHICLERACE || map.sceneNumber == SCENE_SAFARIRACE || map.sceneNumber == SCENE_GRANDCVEHICLERACE)
				RACE_NUM_LAPS = 2;
			else
				RACE_NUM_LAPS = 1;
			
			if (map.sceneNumber == SCENE_SAFARIRACE)
				levelData.timeLimit = 10.0f;
			else if (map.sceneNumber == SCENE_ICEDOMERACE || map.sceneNumber == SCENE_GOLDMINERACE)
				levelData.timeLimit = 25.0f;
			else if (map.sceneNumber == SCENE_MUSEUMVEHICLERACE || map.sceneNumber == SCENE_DEPTSTRVEHICLERACE || map.sceneNumber == SCENE_GRANDCVEHICLERACE)
				levelData.timeLimit = 60.0f;
			else
				levelData.timeLimit = 30.0f;
		}
#endif

		actualTime = levelData.timeLimit;

		gameStatus.player1->actorStatus->multiplayer.lapBestTime = -1;

		// PP: I was getting a NULL player2 here
		if(gameStatus.player2 != NULL)
		{
			gameStatus.player2->actorStatus->multiplayer.lapBestTime = -1;
		}

		if (tempVehicle1)
		{
			bmVectorCopy(gameStatus.player1->actorInstance.position, tempVehicle1->actorInstance.position);

			// JW: gameStatus.player1->actorInstance.position[Y] += METERTOUNIT(1.0f);

			gameStatus.player1->actorStatus->currentState = STATE_JUMP;
			gameStatus.player1->actorStatus->timeWithoutVehicleCollision = 0.4f;

			//tempVehicle1->RespondToCollisionRoutine(gameStatus.player1, tempVehicle1);
		}

		if (tempVehicle2)
		{
			bmVectorCopy(gameStatus.player2->actorInstance.position, tempVehicle2->actorInstance.position);

			// JW: gameStatus.player2->actorInstance.position[Y] += METERTOUNIT(1.0f);

			gameStatus.player2->actorStatus->currentState = STATE_JUMP;
			gameStatus.player2->actorStatus->timeWithoutVehicleCollision = 0.4f;

			//tempVehicle2->RespondToCollisionRoutine(gameStatus.player2, tempVehicle2);
		}

		// PP: Colour for the text in the stats book
		COLOUR		statsBookColour=COLOUR(128, 128, 128, 128);// PP: gotter set alpha to 128 because COLOURs default to alpha 255 which isn't good for text

		for(player = 0; player<mpNumPlayers; player++)
		{
			int	i;

			TBActorAnimSegment *animSeg;
			BOOK *pStatsBook;

			pActorInstance = mpPlayer[player];
			ASSERT(pActorInstance);

			// CPW: MALLOC stuff, that kind of thing ...
			pActorInstance->actorStatus->multiplayer.checkPoint = 0;
			pActorInstance->actorStatus->multiplayer.lap = 0;
			pActorInstance->actorStatus->multiplayer.realLap = 0;
			
			for (i = 0; i < RACE_NUM_LAPS; i++)
				pActorInstance->actorStatus->multiplayer.lapTime[i] = 0;

			pActorInstance->actorStatus->multiplayer.lapBestTime = -1;
			pActorInstance->actorStatus->multiplayer.pVehicle = NULL;
			pActorInstance->actorStatus->multiplayer.position = 0;
			pActorInstance->actorStatus->multiplayer.lapStartTime = 0.0f;

			if (mpNumPlayers == 2)
			{
				char	temp[256];
				sprintf(temp,"race book %d",player);
				pActorInstance->actorStatus->multiplayer.pStatsBook = pStatsBook = new BOOK(temp);
				ASSERT(pStatsBook);

				// PP: Set no viewport correction
	// PP: REMOUT: hmm doesn't exist at the moment so hopefully it wasn't important			pStatsBook->pageItemFlags &= ~PIFLAG_VPADJUST;
				
				// PP: Set the size of the book			
				// PP: REMOUT: books now default to filling the full 'safe' area of the screen		pStatsBook->setBookRect(RECTANGLE(-0.5f, 0.5f, -0.25f, 0.25f));// PP: REMOUT: must now be expressed as fractions of the screen dimensions		RECTANGLE((int)(-videoMode.xScreen/2 + 20), videoMode.xScreen/2 - 20, -videoMode.yScreen/4 + 20, videoMode.yScreen/4 - 20));

				// PP: NEW: tell the book what screen it will draw on (for automatic on-the-fly rectangle adjustment)
				pStatsBook->setScreen(player);

				// PP: 'Position' textbox ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				pPage = pStatsBook->addPage(pActorInstance->actorStatus->multiplayer.pTextBox[TB_POSITION] = new TEXTBOX());
				ASSERT(pPage);

				// PP: Create and add the textbox
				// CPW: pActorInstance->actorStatus->multiplayer.pPage[0]->insertItem(pActorInstance->actorStatus->multiplayer.pTextBox[TB_POSITION] = new TEXTBOX());
				
				pTextBox=pActorInstance->actorStatus->multiplayer.pTextBox[TB_POSITION];
				ASSERT(pTextBox);

				// PP: Set the font size
				pTextBox->setScale(2.0f);

				// PP: Set the font colour and specify no vertical gradient
				pTextBox->setVGrad(false);
				pTextBox->setColour(statsBookColour);

				// JW: Align to top left if player one
				// JW: Align to bottom left if player two

				pTextBox->setXAlign(PIFLAG_XALIGN_LEFT);

				if (player == 1)
					pTextBox->setYAlign(PIFLAG_YALIGN_BOTTOM);
				else
					pTextBox->setYAlign(PIFLAG_YALIGN_TOP);

				// PP: Create a sub-page to hold the 'Lap' and 'Stats' textboxes...

				PAGE*		subPage;

				subPage=new PAGE;
				ASSERT(subPage);

				if (player == 1)
				{
					// PP: 'Lap' textbox ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

					// PP: Create and add the textbox
					subPage->insertItem(pActorInstance->actorStatus->multiplayer.pTextBox[TB_LAP] = new TEXTBOX);
					pTextBox=pActorInstance->actorStatus->multiplayer.pTextBox[TB_LAP];
					ASSERT(pTextBox);

					// PP: Set the font size
					pTextBox->setScale(1.0f);

					// PP: Set the font colour and specify no vertical gradient
					pTextBox->setVGrad(false);
					pTextBox->setColour(statsBookColour);

					// PP: Align to bottom-right
					pTextBox->setXAlign(PIFLAG_XALIGN_RIGHT);
					pTextBox->setYAlign(PIFLAG_YALIGN_BOTTOM);

					// PP: 'Timer' textbox ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

					// PP: Create and add the textbox
					subPage->insertItem(pActorInstance->actorStatus->multiplayer.pTextBox[TB_TIMER] = new TEXTBOX);
					pTextBox=pActorInstance->actorStatus->multiplayer.pTextBox[TB_TIMER];
					ASSERT(pTextBox);

					// PP: Set the font size
					pTextBox->setScale(0.75f);

					// PP: Set the font colour and specify no vertical gradient
					pTextBox->setVGrad(false);
					pTextBox->setColour(statsBookColour);

					// PP: Align to bottom-right
					pTextBox->setXAlign(PIFLAG_XALIGN_RIGHT);
					pTextBox->setYAlign(PIFLAG_YALIGN_BOTTOM);

					// PP: turn off wrapping
					pTextBox->setWrap(false);

					// PP: Turn off Y padding - this pulls the 'Lap' textbox neatly down on top of this 'Stats' textbox
					pTextBox->setYPadding(false);

					// PP: 'Best' textbox ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

					// PP: Create and add the textbox
					subPage->insertItem(pActorInstance->actorStatus->multiplayer.pTextBox[TB_BEST] = new TEXTBOX);
					pTextBox=pActorInstance->actorStatus->multiplayer.pTextBox[TB_BEST];
					ASSERT(pTextBox);

					// PP: Set the font size
					pTextBox->setScale(0.75f);

					// PP: Set the font colour and specify no vertical gradient
					pTextBox->setVGrad(false);
					pTextBox->setColour(statsBookColour);

					// PP: Align to bottom-right
					pTextBox->setXAlign(PIFLAG_XALIGN_RIGHT);
					pTextBox->setYAlign(PIFLAG_YALIGN_BOTTOM);

					// PP: turn off wrapping
					pTextBox->setWrap(false);

					// PP: Turn off Y padding - this pulls the 'Lap' textbox neatly down on top of this 'Stats' textbox
					pTextBox->setYPadding(false);
				}
				else
				{
					// PP: 'Best' textbox ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

					// PP: Create and add the textbox
					subPage->insertItem(pActorInstance->actorStatus->multiplayer.pTextBox[TB_BEST] = new TEXTBOX);
					pTextBox=pActorInstance->actorStatus->multiplayer.pTextBox[TB_BEST];
					ASSERT(pTextBox);

					// PP: Set the font size
					pTextBox->setScale(0.75f);

					// PP: Set the font colour and specify no vertical gradient
					pTextBox->setVGrad(false);
					pTextBox->setColour(statsBookColour);

					// PP: Align to top-right
					pTextBox->setXAlign(PIFLAG_XALIGN_RIGHT);
					pTextBox->setYAlign(PIFLAG_YALIGN_TOP);

					// PP: Turn off Y padding - this pulls the 'Lap' textbox neatly up below this 'Stats' textbox
					pTextBox->setYPadding(false);

					// PP: 'Timer' textbox ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

					// PP: Create and add the textbox
					subPage->insertItem(pActorInstance->actorStatus->multiplayer.pTextBox[TB_TIMER] = new TEXTBOX);
					pTextBox=pActorInstance->actorStatus->multiplayer.pTextBox[TB_TIMER];
					ASSERT(pTextBox);

					// PP: Set the font size
					pTextBox->setScale(0.75f);

					// PP: Set the font colour and specify no vertical gradient
					pTextBox->setVGrad(false);
					pTextBox->setColour(statsBookColour);

					// PP: Align to top-right
					pTextBox->setXAlign(PIFLAG_XALIGN_RIGHT);
					pTextBox->setYAlign(PIFLAG_YALIGN_TOP);

					// PP: Turn off Y padding - this pulls the 'Lap' textbox neatly up below this 'Stats' textbox
					pTextBox->setYPadding(false);

					// PP: 'Lap' textbox ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

					// PP: Create and add the textbox
					subPage->insertItem(pActorInstance->actorStatus->multiplayer.pTextBox[TB_LAP] = new TEXTBOX);
					pTextBox=pActorInstance->actorStatus->multiplayer.pTextBox[TB_LAP];
					ASSERT(pTextBox);

					// PP: Set the font size
					pTextBox->setScale(1.0f);

					// PP: Set the font colour and specify no vertical gradient
					pTextBox->setVGrad(false);
					pTextBox->setColour(statsBookColour);

					// PP: Align to bottom-right
					pTextBox->setXAlign(PIFLAG_XALIGN_RIGHT);
					pTextBox->setYAlign(PIFLAG_YALIGN_TOP);

					// PP: turn off wrapping
					pTextBox->setWrap(false);
				}

				// PP: Insert the sub-page, which now holds the 'Lap' and 'Stats' textboxes
				// CPW: pActorInstance->actorStatus->multiplayer.pPage[0]->insertItem(subPage, false);
				pPage->insertItem(subPage, false);
			}
			else
			{
				pActorInstance->actorStatus->multiplayer.pStatsBook = pStatsBook = new BOOK();
				ASSERT(pStatsBook);				

				pPage = pStatsBook->addPage("stats page");

				if (levelData.gameType == GAMETYPE_TIMEATTACK)
				{
					pPage->insertItem(pIcon = new ICON());

					pIcon->setTexture("checkpoint_icon.bmp");
					pIcon->setScale(0.5f);
					pIcon->setAlignScale(0.6f);
					pIcon->setXAlign(PIFLAG_XALIGN_RIGHT);
					pIcon->setYAlign(PIFLAG_YALIGN_TOP);
					

					pPage->insertItem(pTextBox = new TEXTBOX(),false);

					pTextBox->setXPadding(false);
				}
				else
				{
					pPage->insertItem(pTextBox = new TEXTBOX());
				}

				pActorInstance->actorStatus->multiplayer.pTextBox[0] = pTextBox;

				if (levelData.gameType == GAMETYPE_RACE)
					pTextBox->sprintf8("0 / %d", RACE_NUM_LAPS);
				else
					pTextBox->sprintf8("0 / %d", (RACE_NUM_LAPS * gameStatus.multiplayer.numCheckPoints) + 1);

				pTextBox->setVGrad(false);
				pTextBox->setColour(statsBookColour);
				pTextBox->setAlignScale(1.25f);
				pTextBox->setFontSize(1.25f);
				
				pTextBox->setXAlign(PIFLAG_XALIGN_RIGHT);
				pTextBox->setYAlign(PIFLAG_YALIGN_TOP);
			}
		}

		bmDirectionToQuat(qDown, vDown, 0);
		
		oneChkTime = twoChkTime = 0;

		// PP: re-malloc levelData.oneBook & levelData.twoBook (they get freed at the end of a race)
		levelData.oneBook=new BOOK();
		levelData.twoBook=new BOOK();

		// PP: tell levelData.oneBook & levelData.twoBook which screen they're for
		levelData.oneBook->setScreen(BKSCREEN_ONE)->setEffectChooser(&racePIEChooser);
		levelData.twoBook->setScreen(BKSCREEN_TWO)->setEffectChooser(&racePIEChooser);

		pPage = levelData.oneBook->addPage("checkpoint");
		pPage->insertItem(new TEXTBOX(STR_RACE_CHECKPOINT));
		pPage = levelData.twoBook->addPage("checkpoint");
		pPage->insertItem(new TEXTBOX(STR_RACE_CHECKPOINT));

		if (gameStatus.demoMode)
		{
			levelData.RaceState = Race;
			gameStatus.controlSuspend = FALSE;
		}
		else
		{
			levelData.RaceState = Intro;
		}
		
		gameStatus.multiplayer.countingDown = FALSE;
		
		mpCountdownSetup(mpSubMode);

		ZEROVECTOR(gameStatus.player1->actorBehaviour->physics.velocity);
		if (gameStatus.player2)
		{
			ZEROVECTOR(gameStatus.player2->actorBehaviour->physics.velocity);
		}
	}
	break;

	case MPREASON_PREP:
		
		if(levelData.gameFinished == GAMEOVER_NOTFINISHED)
		{
			for(player = 0; player<mpNumPlayers; player++)
			{
				int i;
				pActorInstance = mpPlayer[player];
				ASSERT(pActorInstance);

				i = pActorInstance->actorStatus->multiplayer.checkPoint;
		
				if(gameStatus.multiplayer.numCheckPoints)
				{
					// JW: Distance from player to next checkpoint
					pActorInstance->actorStatus->multiplayer.distToNext = bmVectorDistance(pActorInstance->actorInstance.position, pCheckPoint[i].actualPos);
				}
			}
		}		
		else
		{
			int gameState = GetCurrentGameState(&gameStatus);
		
			// PP: REMOUT: might be GAMESTATE_OUTRO!	if((gameState != GAMESTATE_CUSTOMPAUSE)&&(gameState != GAMESTATE_FADING))
			if(gameState == GAMESTATE_INGAME)// PP: perhaps this is a slightly more reliable way of doing it
			{
				// PP: char sResult[32];

				if (mpNumPlayers == 2)
				{
					// PP: REMOUT: WILSON!!!!!	sprintf(sResult, "Player %d is the winner!", levelData.winner + 1);
#ifndef CONSUMERDEMO
					if (gameStatus.multiplayer.Tournament)
					{
						mpEndRoundMenuSetUp(&levelData.gameFinished, GAMEOVER_NOTFINISHED, levelData.winner);
						SetGameState(&gameStatus, GAMESTATE_CUSTOMPAUSE, GAMESUBSTATE_NORMAL, (int32)mpEndRoundMenuHandler);
					}
					else
#endif
					{
						mpEndGameMenuSetUp(&levelData.gameFinished, GAMEOVER_NOTFINISHED, levelData.winner);
						SetGameState(&gameStatus, GAMESTATE_CUSTOMPAUSE, GAMESUBSTATE_NORMAL, (int32)mpEndGameMenuHandler);
					}

					if (!gameStatus.player1->actorStatus->multiplayer.position)
						camera[1].fade.StartFadeDown(FADETYPE_NORMAL, 1.5f);
					else
						camera[0].fade.StartFadeDown(FADETYPE_NORMAL, 1.5f);
				}
				else
				{
					//StartFadeDown(&camera[0].fade, FADETYPE_NORMAL, 1.5f);

					if (map.sceneNumber > SCENE_DESTRUCTIONTASMANIA)
						mpEndBonusSetUp(&levelData.gameFinished, GAMEOVER_NOTFINISHED, float(cpoint));
					else
					{
						if (levelData.gameFinished == GAMEOVER_COMPLETED)
							mpEndBonusSetUp(&levelData.gameFinished, GAMEOVER_NOTFINISHED, mpPlayer[0]->actorStatus->multiplayer.lapTime[0] + mpPlayer[0]->actorStatus->multiplayer.lapTime[1]);
						else
							mpEndBonusSetUp(&levelData.gameFinished, GAMEOVER_NOTFINISHED, -1.0f);
					}
					
					StopAllChannels();
					SetGameState(&gameStatus, GAMESTATE_CUSTOMPAUSE, GAMESUBSTATE_NORMAL, (int32)mpEndBonusHandler);
				}

				//if (!gameStatus.multiplayer.versusCPU)
				//{
					for (player = 0; player < gameStatus.multiplayer.numPlayers; player ++)
						gameStatus.player[player].actorInstance->actorStatus->multiplayer.pStatsBook->close();
				//}

				if (levelData.oneBook)	levelData.oneBook->close();
				if (levelData.twoBook)	levelData.twoBook->close();
			}
		}
		break;

	case MPREASON_POLL:

		char string8[64];

/* PP: REMOUT: Now handled by the Book List				raceBook->update();
		levelData.oneBook.update();
		levelData.twoBook.update();*/

		switch(levelData.RaceState)
		{
		case Intro:		
//#ifndef CONSUMERDEMO
						if (gameStatus.currentState == GAMESTATE_INGAME && camera[0].fade.GetFadeState() == FADESTATE_OFF)
//#endif
						{					
							bkPrintf("Pressed Start\n");
								
							levelData.RaceState = Countdown;
							gameStatus.multiplayer.countingDown = TRUE;
							raceBook->gotoPage("5", true);

							PlaySample("tweety_5.wav", 255);

							raceBook->open();
						}

						ZEROVECTOR(gameStatus.player1->actorBehaviour->physics.velocity);

						if (gameStatus.player2)
						{
							ZEROVECTOR(gameStatus.player2->actorBehaviour->physics.velocity);
						}
						
						break;
				
		case Countdown:	if (!mpCountdownUpdate())
						{
							gameStatus.controlSuspend = FALSE;
							gameStatus.gameTime = 0;
							levelData.RaceState = Race;

							raceTime = 0.0f;

							raceBook->gotoPage("Timer", true);

							for (player = 0; player < gameStatus.multiplayer.numPlayers; player ++)
								gameStatus.player[player].actorInstance->actorStatus->multiplayer.pStatsBook->open();
		
#ifndef CONSUMERDEMO
							if (levelData.gameType == GAMETYPE_RACE && gameStatus.multiplayer.numPlayers == 2)
								raceBook->close(0.0f, true);
#endif
						}

						ZEROVECTOR(gameStatus.player1->actorBehaviour->physics.velocity);
						if (gameStatus.player2)
						{
							ZEROVECTOR(gameStatus.player2->actorBehaviour->physics.velocity);
						}

						break;

		case Race:		raceTime += fTime;

						// JW: OKey-dokey...here we have a little finangle to try and vary the pitch of the vehicles 
						// JW: depending on their speed. I do not expect this to work. thats the kinda guy i am.
			
						/*if (levelData.gameType == GAMETYPE_RACE)
						{
							int	frequency;

							float	spdPrcnt1 = 0.0f, spdPrcnt2 = 0.0f;

							switch (map.sceneNumber)
							{
							case SCENE_MUSEUMVEHICLERACE:	

								spdPrcnt1 = (bmVectorLen(gameStatus.player1->actorBehaviour->physics.velocity) / FLOORPOLISHER_MAX_SPEED);
								spdPrcnt2 = (bmVectorLen(gameStatus.player2->actorBehaviour->physics.velocity) / FLOORPOLISHER_MAX_SPEED);

								break;

							default:	
								
								spdPrcnt1 = 1.0f;
								spdPrcnt2 = 1.0f;	
										
								break;
							}

							if (spdPrcnt1 != lastRate1 && bsGetChannelStatus(characterSounds.taz) == BSCHSTATUS_PLAYING)
							{
								frequency = (int) ((STANDARD_FREQUENCY - LOWEST_FREQUENCY) * spdPrcnt1) + LOWEST_FREQUENCY;

								bsSetChannelFrequency(characterSounds.taz, frequency);

								lastRate1 = spdPrcnt1;
							}

							if (spdPrcnt2 != lastRate2 && bsGetChannelStatus(characterSounds.sheDevil) == BSCHSTATUS_PLAYING)
							{
								frequency = (int) ((STANDARD_FREQUENCY - LOWEST_FREQUENCY) * spdPrcnt2) + LOWEST_FREQUENCY;

								bsSetChannelFrequency(characterSounds.sheDevil, frequency);

								lastRate2 = spdPrcnt2;
							}
						}*/

						if(levelData.gameFinished == GAMEOVER_NOTFINISHED)
						{	
							// CPW: "turn off" ALL the checkpoint objects
							if(gameStatus.multiplayer.numCheckPoints)
							{
								for(int j=0; j<gameStatus.multiplayer.numCheckPoints; j++)
								{
									pCheckPoint[j].pModel->flags |= ACTORFLAG_DONTDRAW;
								}
							}
							
							for(player = 0; player<mpNumPlayers; player++)
							{
								// CPW: game loop stuff
								int i;
								
								TBCollisionInfo2D info;
								TBCollisionLine2D trail, span;
								int crossed;
								
								pActorInstance = mpPlayer[player];
								ASSERT(pActorInstance);
								
								info.flags = 0;
								i = pActorInstance->actorStatus->multiplayer.checkPoint;
								
								if(gameStatus.multiplayer.numCheckPoints)
								{
									int j;
									
									TBVector realPos;
									TBVector2D vec2d;
									
									pCheckPoint[i].pModel->flags &= (~ACTORFLAG_DONTDRAW);	
									
									// CPW: I've changed this to show the next two checkpoints, rather than the previous and next
									// CPW: because it caused confusion in some of my 'playtesters'
									j = i + 1;
									if(j >= gameStatus.multiplayer.numCheckPoints)
										j = 0;
									
									pCheckPoint[j].pModel->flags &= (~ACTORFLAG_DONTDRAW);
									
									// CPW: line 1
									span.start[X] = pCheckPoint[i].testPoints[1][X];
									span.start[Y] = pCheckPoint[i].testPoints[1][Z];
									span.end[X] = pCheckPoint[i].testPoints[3][X];
									span.end[Y] = pCheckPoint[i].testPoints[3][Z];
									
									// CPW: line 2, the path
									bmVectorAdd(realPos, pActorInstance->actorBehaviour->collSphere.offset, pActorInstance->actorInstance.position);
									trail.start[X] = realPos[X];
									trail.start[Y] = realPos[Z];
									bmVectorAdd(realPos, pActorInstance->actorBehaviour->collSphere.offset, pActorInstance->actorStatus->positionLastFrame);
									trail.end[X] = realPos[X];
									trail.end[Y] = realPos[Z];
									
									vec2d[X] = trail.end[X] - trail.start[X];
									vec2d[Y] = trail.end[Y] - trail.start[Y];
									
									// CPW: TODO proximity test
									if(bmVector2DSquaredLen(vec2d) != 0.0f)
									{
										// CPW: if for some reason bmCollideLineLine2D() is passed a zero length 'line' then CRASH
										// CPW: hence the above test
										crossed = bmCollideLineLine2D(&trail, &span, &info);
										
										if(crossed)
										{
											if((pActorInstance->actorStatus->currentState == STATE_VEHICLE) || (levelData.needVehicle == false))							
											{
												cpoint++;
												
												// JW: If time attack, boost time limit
												if (levelData.gameType == GAMETYPE_TIMEATTACK && pActorInstance->actorStatus->multiplayer.position == 0)
													// JW: Get the time credit for the current checkpoint
													levelData.timeCredit += pCheckPoint[pActorInstance->actorStatus->multiplayer.checkPoint].timeCredit;
												
												//don't show 'Checkpoint!' when crossing final finishing line
												if (player == 0 && ((pActorInstance->actorStatus->multiplayer.realLap < RACE_NUM_LAPS - 1) || (pActorInstance->actorStatus->multiplayer.checkPoint) || (mpNumPlayers == 1)))
												{
													oneChkTime = gameStatus.gameTime + 1.0f;
													levelData.oneBook->open();
												}
												
												if (player == 1 && ((pActorInstance->actorStatus->multiplayer.realLap < RACE_NUM_LAPS - 1) || (pActorInstance->actorStatus->multiplayer.checkPoint)))
												{
													twoChkTime = gameStatus.gameTime + 1.0f;
													levelData.twoBook->open();
												}
												
												bkPrintf("Player %d has crossed checkpoint %d, lap %d\n", player, i, pActorInstance->actorStatus->multiplayer.lap);
												
												if (characterSounds.tweety == -1)
													CallbackWhenSampleFinished((characterSounds.tweety=PlaySample("tweetycheckpoint_1.wav",200)),CharacterSoundsFinishedCallback,&characterSounds.tweety);
												
												i++;
												
												if(i == gameStatus.multiplayer.numCheckPoints)
												{
													i=0;
													pActorInstance->actorStatus->multiplayer.lap++;
												}
												
												if((pActorInstance->actorStatus->multiplayer.lap == 0) && (i==1))
												{
													bkPrintf("Reset!\n");
												}
												
												if((pActorInstance->actorStatus->multiplayer.lap > 0) && (i==1))
												{
													float lt, at;
													int l;
													
													bkPrintf("*** NEW LAP %d ***\n", pActorInstance->actorStatus->multiplayer.lap);
													pActorInstance->actorStatus->multiplayer.realLap++;
													
													pActorInstance->actorStatus->multiplayer.lapTime[pActorInstance->actorStatus->multiplayer.lap - 1] = raceTime - pActorInstance->actorStatus->multiplayer.lapStartTime;
													pActorInstance->actorStatus->multiplayer.lapStartTime = raceTime;
													
													// CPW: calculate new average
													/*lt = pActorInstance->actorStatus->multiplayer.lapTime;
													at = pActorInstance->actorStatus->multiplayer.lapAvgTime;
													l = pActorInstance->actorStatus->multiplayer.realLap;
													
													pActorInstance->actorStatus->multiplayer.lapAvgTime = (at*(l-1)+lt)/l;*/
													
													if ((pActorInstance->actorStatus->multiplayer.lapTime[pActorInstance->actorStatus->multiplayer.lap - 1] < pActorInstance->actorStatus->multiplayer.lapTime[pActorInstance->actorStatus->multiplayer.lapBestTime]) ||
														(pActorInstance->actorStatus->multiplayer.lapBestTime == -1))
														pActorInstance->actorStatus->multiplayer.lapBestTime = pActorInstance->actorStatus->multiplayer.lap - 1;
												}
											}
										}
									}
							}
							
							pActorInstance->actorStatus->multiplayer.checkPoint = i;
							
							int dewFlaps = pActorInstance->actorStatus->multiplayer.realLap >= RACE_NUM_LAPS;
							
							if(dewFlaps && pTarget)
							{
								// CPW: if we've finished our laps AND there's a target, then draw it
								ASSERT(pTarget->pModel);
								pTarget->pModel->flags &= ~ACTORFLAG_DONTDRAW;
							}
							
							// CPW: bkPrintf("dist = %f\n", bmVectorDistance(pTarget->modelPos[0], pActorInstance->actorInstance.position));
							
							if((dewFlaps && (!pTarget)) || (dewFlaps && pTarget && (bmVectorDistance(pTarget->modelPos, pActorInstance->actorInstance.position) < METERTOUNIT(0.5f))))
							{
								levelData.gameFinished = GAMEOVER_COMPLETED;
								
								raceBook->close();
								
								if(pActorInstance->actorStatus->multiplayer.position == 0)
									levelData.winner = player;
							}
							else
							{
								if(mpNumPlayers == 2)
								{
									int slen;
									
									utilGetPrettyTime(string8,raceTime - pActorInstance->actorStatus->multiplayer.lapStartTime);
									
									pActorInstance->actorStatus->multiplayer.pTextBox[TB_TIMER]->sprintf8("%s", string8);
									
									/* PP: REMOUT: English	
									
									  slen = sprintf(string8, " best: ");
									  
										if (pActorInstance->actorStatus->multiplayer.lapBestTime == -1)
										sprintf(string8 + slen, "=:==.=");
										else
										utilGetPrettyTime(string8 + slen, pActorInstance->actorStatus->multiplayer.lapTime[pActorInstance->actorStatus->multiplayer.lapBestTime]);
										
										  pActorInstance->actorStatus->multiplayer.pTextBox[TB_BEST]->sprintf8("%s", string8);
									*/
									
									if (pActorInstance->actorStatus->multiplayer.lapBestTime == -1)
										sprintf(string8, "=:==.=");
									else
										utilGetPrettyTime(string8, pActorInstance->actorStatus->multiplayer.lapTime[pActorInstance->actorStatus->multiplayer.lapBestTime]);
									
									pActorInstance->actorStatus->multiplayer.pTextBox[TB_BEST]->setText(STR_RACE_BEST)->strcatf(" %s", string8);
									
									// CPW: print position
									pActorInstance->actorStatus->multiplayer.pTextBox[TB_POSITION]->setText(EStringIndex(STR_MP_1ST+pActorInstance->actorStatus->multiplayer.position));
									
									// CPW: print Lap
									pActorInstance->actorStatus->multiplayer.pTextBox[TB_LAP]->setText(STR_RACE_LAP, pActorInstance->actorStatus->multiplayer.realLap + 1, RACE_NUM_LAPS);
								}
								else
								{						
									if (levelData.gameType == GAMETYPE_RACE)
										pActorInstance->actorStatus->multiplayer.pTextBox[0]->setText(STR_RACE_LAP, pActorInstance->actorStatus->multiplayer.realLap + 1, RACE_NUM_LAPS);
									else
									{
										if (levelData.timeCredit && !pActorInstance->actorStatus->multiplayer.checkPoint)
											pActorInstance->actorStatus->multiplayer.pTextBox[0]->sprintf8("%d / %d", (pActorInstance->actorStatus->multiplayer.realLap * gameStatus.multiplayer.numCheckPoints) + gameStatus.multiplayer.numCheckPoints,  (RACE_NUM_LAPS * gameStatus.multiplayer.numCheckPoints) + 1);
										else
											pActorInstance->actorStatus->multiplayer.pTextBox[0]->sprintf8("%d / %d", (pActorInstance->actorStatus->multiplayer.realLap * gameStatus.multiplayer.numCheckPoints) + pActorInstance->actorStatus->multiplayer.checkPoint,  (RACE_NUM_LAPS * gameStatus.multiplayer.numCheckPoints) + 1);
									}
								}
							}
							
							// PP: re-align the book // PP: REMOUT: this was just for testing purposes - left here because I might want to use it again
							// PP: remember - BOOK::REALIGN IS VERY VERY COSTLY!!!!!!!!
							// PP: pActorInstance->actorStatus->multiplayer.pStatsBook->realign();
						}
						
						// CPW: do position rankings. NB currently, this will only work if there's 2 players
						if((mpNumPlayers == 2) && gameStatus.multiplayer.numCheckPoints) // CPW: breaks if more than 2 players, meaningless if there's 1
						{
							if(mpPlayer[0]->actorStatus->multiplayer.lap > mpPlayer[1]->actorStatus->multiplayer.lap)
							{	// CPW: player 0 ahead (on laps)
								mpPlayer[0]->actorStatus->multiplayer.position = 0;
								mpPlayer[1]->actorStatus->multiplayer.position = 1;
							}
							else
							{
								if(mpPlayer[0]->actorStatus->multiplayer.lap < mpPlayer[1]->actorStatus->multiplayer.lap)
								{	// CPW: player 1 ahead (on laps)
									mpPlayer[0]->actorStatus->multiplayer.position = 1;
									mpPlayer[1]->actorStatus->multiplayer.position = 0;
								}
								else
								{	// CPW: They are equal on laps, so let's check according to checkpoints instead
									if(mpPlayer[0]->actorStatus->multiplayer.checkPoint > mpPlayer[1]->actorStatus->multiplayer.checkPoint)
									{	// CPW: player 0 ahead (on checkpoints)
										mpPlayer[0]->actorStatus->multiplayer.position = 0;
										mpPlayer[1]->actorStatus->multiplayer.position = 1;
									}
									else
									{
										if(mpPlayer[0]->actorStatus->multiplayer.checkPoint < mpPlayer[1]->actorStatus->multiplayer.checkPoint)
										{	// CPW: player 1 ahead (on checkpoints)
											mpPlayer[0]->actorStatus->multiplayer.position = 1;
											mpPlayer[1]->actorStatus->multiplayer.position = 0;
										}
										else
										{	// CPW: they are equal on checkpoints too, so do it on dist to next checkpoint, (when I can be arsed)
											if(mpPlayer[0]->actorStatus->multiplayer.distToNext < mpPlayer[1]->actorStatus->multiplayer.distToNext)
											{// CPW: player 0 ahead (on distance)
												mpPlayer[0]->actorStatus->multiplayer.position = 0;
												mpPlayer[1]->actorStatus->multiplayer.position = 1;
											}
											else
											{
												if(mpPlayer[0]->actorStatus->multiplayer.distToNext > mpPlayer[1]->actorStatus->multiplayer.distToNext)
												{// CPW: player 1 ahead (on distance)
													mpPlayer[0]->actorStatus->multiplayer.position = 1;
													mpPlayer[1]->actorStatus->multiplayer.position = 0;
												}
												// CPW: don't bother with an else, 'cos if they are exactly neck and neck, that's too unlikely
												// CPW: to actually be bothered about.
											}
										}
									}
								}
							}
						}
					}
					else	//if(levelData.gameFinished != GAMEOVER_NOTFINISHED)
					{
						int	i;
						
						for (i = 0; i < gameStatus.multiplayer.numPlayers; i++)
						{
							if (gameStatus.player[i].actorInstance->actorStatus->currentState == STATE_SPIN)
							{
								if (i)
									controller2.circleDebounceChannel->value = 0;
								else
									controller1.circleDebounceChannel->value = 0;
								
								HandleSpinState(&gameStatus.player[i]);
							}
						}
						
						levelData.RaceState = Finished;
						
						raceBook->close();
						
						return 0;
					}

#ifndef CONSUMERDEMO
						// JW: Do Time Attack update
						if(levelData.gameType == GAMETYPE_TIMEATTACK || mpNumPlayers == 1)
						{				
#endif
							actualTime = levelData.timeLimit - raceTime + levelData.timeCredit;
	
							if (actualTime <= 0.0f)
							{
								int	i;

								for (i = 0; i < gameStatus.multiplayer.numPlayers; i++)
								{
									if (gameStatus.player[i].actorInstance->actorStatus->currentState == STATE_SPIN)
									{
										if (i)
											controller2.circleDebounceChannel->value = 0;
										else
											controller1.circleDebounceChannel->value = 0;
										
										HandleSpinState(&gameStatus.player[i]);
									}

									gameStatus.player[i].actorInstance->actorStatus->multiplayer.pStatsBook->close();
								}
								
								if (levelData.oneBook)	levelData.oneBook->close();
								if (levelData.twoBook)	levelData.twoBook->close();

								levelData.RaceState = TimeUp;

								raceBook->gotoPage("TimeUp");

								actualTime = gameStatus.gameTime + MP_TIMEUP_DURATION;

								if(pActorInstance->actorStatus->multiplayer.position == 0)
									levelData.winner = 1;
								else
									levelData.winner = 0;
							}
#ifndef CONSUMERDEMO
						}
#endif
												
						break;

		case TimeUp:	if (gameStatus.gameTime > actualTime)						
						{
							levelData.gameFinished = GAMEOVER_TIMEDOUT;
							levelData.RaceState = Finished;

							raceBook->close();
						}
			
						break;
		}

		break;

	case MPREASON_DRAW:
		if(CURRENTLY_DRAWING_FULL_SCREEN)
		{
			// CPW: draw Countdown			
			switch (levelData.RaceState)
			{
			case TimeUp:	// JW: Show Intro Screen
							mpDrawBook();
							break;

			case Countdown:	mpCountdownDraw();
							break;

			case Race:
						/* PP: REMOUT: Now handled by the Book List		
#ifdef CONSUMERDEMO							
						if (gameStatus.currentState == GAMESTATE_INGAME)
							{
								int		intaT = (int) actualTime;
								float	faT = actualTime - (float) intaT;

								if ((actualTime > 10.0f) || (faT > 0.5f && faT < 0.75f) || (faT > 0.0f && faT < 0.25f))
									mpDrawBook();
							}
							else if (gameStatus.currentState == GAMESTATE_PAUSE && gameStatus.multiplayer.numScreens == 2)
								DrawMPPausePage();
#else
							if ((levelData.gameType == GAMETYPE_TIMEATTACK || mpNumPlayers == 1) && gameStatus.currentState == GAMESTATE_INGAME)
							{
								int		intaT = (int) actualTime;
								float	faT = actualTime - (float) intaT;

								if ((actualTime > 10.0f) || (faT > 0.5f && faT < 0.75f) || (faT > 0.0f && faT < 0.25f))
									mpDrawBook();
							}
							else if (gameStatus.currentState == GAMESTATE_PAUSE && gameStatus.multiplayer.numScreens == 2)
								DrawMPPausePage();
#endif
							*/
							// JW: Show global timer
							break;
			}
		}
		
		if ((gameStatus.multiplayer.numScreens == 1 || !CURRENTLY_DRAWING_FULL_SCREEN) && gameStatus.currentState == GAMESTATE_INGAME)
		{
			if(levelData.gameFinished == GAMEOVER_NOTFINISHED)
			{	
				if(!mpDrawCam)
				{
					bkPrintf("mpDrawCam not set!\n");
					break;
				}
	
				pActorInstance = mpPlayer[gameStatus.multiplayer.currentScreen];
				ASSERT(pActorInstance);

				if (levelData.RaceState == Race)
				{	
					/* PP: REMOUT: Now handled by the Book List		
					bdSetIdentityObjectMatrix();
					
					SAVESTATES;
			
					// PP: draw navigation instructions...
					
					SETPROJECTION(2D);
					SETZTEST(ALWAYS);
					
					// PP: Draw the stats book
					
					if (mpNumPlayers == 2)
						pActorInstance->actorStatus->multiplayer.pStatsBook->draw();
					*/

					// PP: REMOUT: left in cos it might be useful again later		
					// PP: pActorInstance->actorStatus->multiplayer.pStatsBook->drawDebug();
					
					// JW: check which screen we're drawing in
					// JW: if (we're in screen one)
					
					if (pActorInstance->playerNo == 0)
					{
					// PP: REMOUT: Now handled by the Book List		levelData.oneBook.draw();
						
						if (oneChkTime && gameStatus.gameTime > oneChkTime)
						{
							levelData.oneBook->close();
							oneChkTime = 0;
						}
					}
					else // JW: if we're in screen two
					{
					// PP: REMOUT: Now handled by the Book List		levelData.twoBook.draw();
						
						if (twoChkTime && gameStatus.gameTime > twoChkTime)
						{
							levelData.twoBook->close();
							twoChkTime = 0;
						}
					}
					/* PP: REMOUT: Now handled by the Book List		
					SETPROJECTION(3D);
					
					RESTORESTATES;*/
				}
			}
		}
		break;

	case MPREASON_KILL:
		// CPW: FREE stuff
		// CPW: numCheckPoints = 0;
		{
			int i;

			for(i=0; i<mpNumPlayers; i++)
			{
				ACTORINSTANCE *pActorInstance = mpPlayer[i];
				ASSERT(pActorInstance);
				delete(pActorInstance->actorStatus->multiplayer.pStatsBook);
			}

			cdKillTimer(CHECKPOINT_TIMER_ONE);
			cdKillTimer(CHECKPOINT_TIMER_TWO);

			mpCountdownKill();
			mpEndBonusKill();

			BookList_closeAndFree(levelData.oneBook);	levelData.oneBook = NULL;
			BookList_closeAndFree(levelData.twoBook); levelData.twoBook = NULL;

			gameStatus.multiplayer.numCheckPoints = 0;

			if (!gameStatus.demoMode)
				setNumScreens(1);

			if(pTarget)
			{
				ASSERT(pTarget);
				delete(pTarget);
				pTarget = NULL;
			}
		}
		break;
	}

	return 0;
}
