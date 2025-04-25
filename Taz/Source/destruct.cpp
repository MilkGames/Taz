// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Blitz Games Ltd.
//
//      File : destruct.cpp
//   Purpose : multiplayer race stuff
// Component : Taz
//    Author : Chris W
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "main.h"

#include "display.h"
#include "actors.h"
#include "animation.h"
#include "status.h"
#include "electrocution.h"
#include "events.h"
#include "util.h"
#include "textfx.h"
#include "icon.h"
#include "scene.h"
#include "collision.h"
#include "control.h"
#include "fallingobjects.h"
#include "videofx.h"
#include "collectibles.h"
#include "pause.h"
#include "cheesegun.h"

#include "multiplayer.h"
#include "tournament.h"
#include "destruct.h"

#define ACTION_NULL			(0)
#define ACTION_DESTROYED	(1)
#define ACTION_COLLECTED	(2)

#define TB_COLLECTED		(0)
#define TB_TIMER			(1)
#define TB_POSITION			(2)

#define GAMETYPE_TARGET		(0)
#define GAMETYPE_TIMELIMIT	(1)


extern TBEventClient	*destroycollectEventClient;

char	lastDestroyer;

static struct
{
	char			*sObjLookFor;
	char			*sObjName;
	char			*sObjLeafName;
	uint32			crcObjLookFor;
	
	char			*sObjExtraTime;
	char			*sObjSloMo;
	char			*sObjDoubleCrate;

	char			*sObjLookForContact;
	uint32			crcObjLookForContact;
} destructData;

	/*ACTORINSTANCE	*pShowEm;
	float			globalScale;
	uint32			gameType;
	int				theWinner;

	RACESTATE		RaceState;
	int				timesUp;
	int				winner;
	int				gameFinished;
	float			timeCredit;

	float			oneChkTime, twoChkTime;
	BOOK			*oneBook, *twoBook;			// PP: these hold destruction-specific pickup messages

	union{
		struct{
			float timeLimit;
		}timeLimit;

		struct{
			int total;
			int count;
		}target;
	};
}levelData;*/

float	TimeUpTimer = 0.0f, nextDropTime = 0.0f, PlayerEffectTimer[2], extraTimer = 0.0f, destructTime = 0.0f;

int		numBonusItemsOnScreen;

TBActRenderMode_Outlined	PlayerOutline[2];

/*  --------------------------------------------------------------------------------
	Function	: destructForceEnd
	Purpose		: Force the end of a destruction game
	Parameters	: win- true/false
	Returns		: 
	Info		: 
*/

void destructForceEnd(bool win)
{
	levelData.RaceState = Finished;

	if(mpNumPlayers == 2)
	{
		raceBook->close();

		if (win)
		{
			mpPlayer[0]->actorStatus->multiplayer.numPickups = mpPlayer[1]->actorStatus->multiplayer.numPickups+1;
		}
		else
		{
			mpPlayer[1]->actorStatus->multiplayer.numPickups = mpPlayer[0]->actorStatus->multiplayer.numPickups+1;
		}
	}
	else
	{
		raceBook->close();

		if (win)
		{
			mpPlayer[0]->actorStatus->multiplayer.numPickups = 100;	// JW: A little excessive perhaps?
		}
		else
		{
			mpPlayer[0]->actorStatus->multiplayer.numPickups = 0;
		}
	}
}

int destructHandler(int reason)
{
	char parmStringBuf[256];
	int i;

	switch(reason)
	{
	case MPREASON_INITIALISE:
	{
		// CPW: called once
		int player;

		numBonusItemsOnScreen = 0;

		memset(&levelData, 0, sizeof(levelData));
		levelData.gameType = GAMETYPE_TIMELIMIT;

		actualTime = 60.0f;

		destructData.sObjLookForContact = 0;

		switch(mpSubMode)
		{
		case SCENE_DESTRUCTIONCONSTRUCT:
			destructData.sObjLookFor = "objects\\bank - destructible crates.obe";
			destructData.sObjLeafName = "objects\\bank - destructible crates.obe";
			destructData.sObjName = "Crates";

			destructData.sObjLookForContact = "objects\\bank - contact dest crates.obe";

			break;

		case SCENE_DESTRUCTIONAQUA:
			destructData.sObjLookFor = "objects\\bank - destructible crates.obe";
			destructData.sObjLeafName = "objects\\bank - destructible crates.obe";
			destructData.sObjName = "Crates";

			destructData.sObjLookForContact = "objects\\bank - dest crates cont.obe";

			break;

		case SCENE_DESTRUCTIONTASMANIA:
			destructData.sObjLookFor = "objects\\bank - destructible crates.obe";
			destructData.sObjLeafName = "objects\\bank - destructible crates.obe";
			destructData.sObjName = "Crates";

			destructData.sObjLookForContact = "objects\\bank - dest crates cont.obe";
			break;

		default:
			destructData.sObjLookFor = NULL;
			destructData.sObjLeafName = NULL;
			destructData.crcObjLookFor = 0;
		}
		
		destructData.sObjExtraTime = "speed clock.obe";
		destructData.sObjDoubleCrate = "mplayer box.obe";
		destructData.sObjSloMo = "ice clock.obe";

		if(destructData.sObjLookFor)
			destructData.crcObjLookFor = bkCRC32((uchar*)destructData.sObjLookFor,strlen(destructData.sObjLookFor),0);

		if (destructData.sObjLookForContact)
			destructData.crcObjLookForContact = bkCRC32((uchar*)destructData.sObjLookForContact,strlen(destructData.sObjLookForContact),0);

		ASSERT(mpNumPlayers);

		BOOK		*pStatsBook;
		PAGE		*pPage;
		PAGE		*pSubPage;
		ICON		*pIcon;
		TEXTBOX		*pTextBox;

		for(player=0; player<mpNumPlayers; player++)
		{
			ACTORINSTANCE *pActorInstance = mpPlayer[player];
			pActorInstance->actorStatus->multiplayer.numPickups = 0;
			pActorInstance->actorStatus->multiplayer.position = 0;
			pActorInstance->actorStatus->multiplayer.effect = PLAYEREFFECTNORMAL;

			// PP: Colour for the text in the stats book
			COLOUR		statsBookColour=COLOUR(128, 128, 128);
				
			pStatsBook = pActorInstance->actorStatus->multiplayer.pStatsBook = new BOOK;

			pStatsBook->setBookRect(RECTANGLE(-0.45f, 0.45f, -0.45f, 0.45f));

			// PP: REMOUT: books now default to filling the full 'safe' area of the screen		pStatsBook->setBookRect(RECTANGLE(-0.5f, 0.5f, -0.5f, 0.5f));
			pStatsBook->setScreen(player);

			// PP: REMOUT: books now default to filling the full 'safe' area of the screen		pStatsBook->setBookRect(RECTANGLE(-0.5f, 0.5f, -0.5f, 0.5f));// PP: REMOUT: must now be expressed as fractions of the screen dimensions		RECTANGLE((int)(-videoMode.xScreen/2 + 20), videoMode.xScreen/2 - 20, -videoMode.yScreen/2 + 20, videoMode.yScreen/2 - 20));

			// JW: Collected Text Box
			
			// TP: this was causing memory leaks and didn't seem to be used:	pSubPage = new PAGE;
			// TP: ASSERT(pSubPage);
			
			pPage = pStatsBook->addPage();
			
			if (mpNumPlayers == 2)
			{				
				pPage->insertItem(pActorInstance->actorStatus->multiplayer.pTextBox[TB_POSITION] = new TEXTBOX);
				pTextBox = pActorInstance->actorStatus->multiplayer.pTextBox[TB_POSITION];
				ASSERT(pTextBox);

				pTextBox->setVGrad(false);
				pTextBox->setScale(2.0f);
				pTextBox->setColour(statsBookColour);
				pTextBox->setXAlign(PIFLAG_XALIGN_LEFT);
					
				if (player == 0)
					pTextBox->setYAlign(PIFLAG_YALIGN_TOP);
				else
					pTextBox->setYAlign(PIFLAG_YALIGN_BOTTOM);
			}
	
			pPage->insertItem(pIcon = new ICON("destructibles02.bmp"), false);
			ASSERT(pIcon);

			pIcon->tilt = false;
			pIcon->setScale(0.8f);
			pIcon->setXAlign(PIFLAG_XALIGN_RIGHT);
				
			if (player == 0)
				pIcon->setYAlign(PIFLAG_YALIGN_TOP);
			else
				pIcon->setYAlign(PIFLAG_YALIGN_BOTTOM);
			
			pPage->insertItem(pTextBox = pActorInstance->actorStatus->multiplayer.pTextBox[TB_COLLECTED] = new TEXTBOX(), false);
			ASSERT(pTextBox);

			pTextBox->setVGrad(false);
			pTextBox->setScale(2.0f);
			pTextBox->setXPadding(false);
			pTextBox->setColour(statsBookColour);
			pTextBox->setXAlign(PIFLAG_XALIGN_RIGHT);
				
			if (player == 0)
				pTextBox->setYAlign(PIFLAG_YALIGN_TOP);
			else
				pTextBox->setYAlign(PIFLAG_YALIGN_BOTTOM);

			pTextBox->sprintf8("   %d", 0);
		}

		levelData.RaceState = Intro;
		levelData.timeCredit = 0.0f;

		gameStatus.multiplayer.countingDown = FALSE;
		gameStatus.controlSuspend = TRUE;

		TimeUpTimer = 0.0f;
		nextDropTime = 0.0f;
		PlayerEffectTimer[0] = PlayerEffectTimer[1] = -1.0f; 

		// PP: Create the two books, remembering to TELL THEM WHICH SCREEN THEY'RE FOR!!!!!!!! ...

		levelData.oneBook = new BOOK();
		levelData.oneBook->setScreen(BKSCREEN_ONE);

		levelData.twoBook = new BOOK();
		levelData.twoBook->setScreen(BKSCREEN_TWO);

		pPage = levelData.oneBook->addPage("extra time");
		pPage->insertItem(new TEXTBOX(STR_EXTRA_TIME));
		pPage = levelData.twoBook->addPage("extra time");
		pPage->insertItem(new TEXTBOX(STR_EXTRA_TIME));

		pPage = levelData.oneBook->addPage("slo mo");
		pPage->insertItem(new TEXTBOX(STR_SLO_MO));
		pPage = levelData.twoBook->addPage("slo mo");
		pPage->insertItem(new TEXTBOX(STR_SLO_MO));

		pPage = levelData.oneBook->addPage("double crates");
		pPage->insertItem(new TEXTBOX(STR_DOUBLE_CRATE));
		pPage = levelData.twoBook->addPage("double crates");
		pPage->insertItem(new TEXTBOX(STR_DOUBLE_CRATE));

		mpCountdownSetup(mpSubMode);
	}
	break;

	case MPREASON_PREP:
		// CPW: called once
		break;

	case MPREASON_POLL:
	{
/* PP: REMOUT: Now handled by the Book List					raceBook->update();
		levelData.oneBook->update();
		levelData.twoBook->update();
*/
		int player;

		switch(levelData.RaceState)
		{
		case Intro:		if (gameStatus.currentState == GAMESTATE_INGAME && camera[0].fade.GetFadeState() == FADESTATE_OFF)
						{				
							bkPrintf("Pressed Start\n");
							
							levelData.RaceState = Countdown;
							gameStatus.multiplayer.countingDown = TRUE;
							raceBook->gotoPage("5", true);

							raceBook->open();

							PlaySample("tweety_5.wav", 255);
						}

						break;
				
		case Countdown:	if (!mpCountdownUpdate())
						{
							gameStatus.controlSuspend = FALSE;
							gameStatus.gameTime = 0;
							levelData.RaceState = Race;
					
							raceBook->gotoPage("Timer", true);
							raceBook->getCurrentPage()->getLastItem()->setScale(1.5);

							for (i = 0; i < gameStatus.multiplayer.numPlayers;i++)
								gameStatus.player[i].actorInstance->actorStatus->multiplayer.pStatsBook->open();

							destructTime = 0.0f;
						}

						break;
				
		case Race:		destructTime += fTime;
						actualTime = 60.0f - destructTime;

						if (extraTimer && extraTimer <= gameStatus.gameTime)
						{
							raceBook->gotoPage("Timer");
							extraTimer = 0;
						}
						
						if (actualTime <= 0.0f)
						{
							if (gameStatus.multiplayer.Tournament && 
								(mpPlayer[0]->actorStatus->multiplayer.numPickups == mpPlayer[1]->actorStatus->multiplayer.numPickups))
							{
								destructTime -= 10.0f;

								raceBook->gotoPage("ExtraTime");

								extraTimer = gameStatus.gameTime + 1.5f;
							}
							else
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

								raceBook->gotoPage("TimeUp", true);
		
								levelData.RaceState = TimeUp;

								for (i = 0; i < gameStatus.multiplayer.numPlayers;i++)
									gameStatus.player[i].actorInstance->actorStatus->multiplayer.pStatsBook->close();
								
								TimeUpTimer = gameStatus.gameTime + 1.0f;

								break;
							}
						}

						if (mpNumPlayers == 2)
						{
							if (gameStatus.gameTime >= nextDropTime)
							{
								ACTORINSTANCE	*fallingObject;

								// JW: Drop Object
								// JW: Get new drop time
								COLLVERTEX * vertices;

								TBVector DropPos;

								float	randvalue;

								int		j, numTriggers = map.triggerInstances.numEntries, randObject;

								ACTORINSTANCE *Trigger = map.triggerInstances.head.next;

								float	dirx, dirz;

								if (numBonusItemsOnScreen <= 10)
								{
									numTriggers = bmRand() % numTriggers;
									
									for (j = 0; j < numTriggers; j++)
										Trigger = Trigger->next;
#if BPLATFORM==GAMECUBE
									vertices = (COLLVERTEX*)GetActorGeometryFromARAM(Trigger, 0, NULL, NULL, BGETACTGEOMETRY_WORLDSPACE, 0);
#else
									vertices = (COLLVERTEX*)baGetActorGeometry(&Trigger->actorInstance, Trigger->actorInstance.rootNodeInstance, 
													   0, NULL, NULL, NULL, NULL, BGETACTGEOMETRY_WORLDSPACE);
#endif
									
									dirx = vertices[0].x - vertices[1].x;
									dirz = vertices[0].z - vertices[1].z;
									
									randvalue = bmRandf();
									
									DropPos[X] = vertices[1].x + (dirx * randvalue);
									DropPos[Z] = vertices[1].z + (dirz * randvalue);
									
									dirx = vertices[2].x - vertices[1].x;
									dirz = vertices[2].z - vertices[1].z;
									
									randvalue = bmRandf();
									
									DropPos[X] += (dirx * randvalue);
									DropPos[Z] += (dirz * randvalue);
									
									DropPos[X] -= bmFMod(DropPos[X], METERTOUNIT(2));
									DropPos[Y] = vertices[0].y;
									DropPos[Z] -= bmFMod(DropPos[Z], METERTOUNIT(2));
									
									randObject = (int) UtilGenerateRandomNumber(0, 10);
									
									switch(randObject)
									{
									case 0:
									case 1:
									case 2:
									case 3:
									case 4:								
										fallingObject = DropObject(&map.levelInstances, 0, NULL, destructData.sObjExtraTime, DropPos, NULL, FALLINGTYPE_REMAIN, NULL, NULL, GetHeightOfPoint(DropPos, NULL) - METERTOUNIT(0.25f), 0);	break;
										
									case 5:
									case 6:
									case 7:
										fallingObject = DropObject(&map.levelInstances, 0, NULL, destructData.sObjSloMo, DropPos, NULL, FALLINGTYPE_REMAIN, NULL, NULL, GetHeightOfPoint(DropPos, NULL) - METERTOUNIT(0.25f), 0);		break;
										
									case 8:
									case 9:
										fallingObject = DropObject(&map.levelInstances, 0, NULL, destructData.sObjDoubleCrate, DropPos, NULL, FALLINGTYPE_REMAIN, NULL, NULL, GetHeightOfPoint(DropPos, NULL) - METERTOUNIT(0.25f), 0);	break;
									}
									
									RemoveInstanceFromInstanceList(fallingObject,&map.levelInstances);
									AddInstanceToInstanceList(fallingObject,&map.animatedInstances);
									
									FlushAnimationQueue(fallingObject, 0);
									PlayAnimationByName(fallingObject, "idle", 1, 1, 0, 0, NONE);
									
									numBonusItemsOnScreen ++;
									SAFE_FREE(vertices);
								}


								nextDropTime = gameStatus.gameTime + (bmRandf() * 3.0f) + 2.0f;
							}
						}
				
						for(player=0; player<mpNumPlayers; player++)
						{
							ACTORINSTANCE *pActorInstance = mpPlayer[player];
							pActorInstance->actorStatus->multiplayer.pTextBox[TB_COLLECTED]->sprintf8("    %d", pActorInstance->actorStatus->multiplayer.numPickups);
																	
							// CPW: print position
							//pActorInstance->actorStatus->multiplayer.pTextBox[TB_POSITION]->sprintf8("%s", psPositionNames[pActorInstance->actorStatus->multiplayer.position]);
// PP: REMOUT: Now Handled by the Book List										pActorInstance->actorStatus->multiplayer.pStatsBook->update();

							if (pActorInstance->actorStatus->multiplayer.effect == PLAYEREFFECTDOUBLE)
								SetPlayerOutlineColor(pActorInstance, &PlayerOutline[pActorInstance->playerNo], YELLOW_EFFECT);

							if (pActorInstance->actorStatus->multiplayer.effect == PLAYEREFFECTSLOMO)
								SetPlayerOutlineColor(pActorInstance, &PlayerOutline[pActorInstance->playerNo], BLUE_EFFECT);
							
							if (gameStatus.gameTime >= PlayerEffectTimer[pActorInstance->playerNo] && PlayerEffectTimer[pActorInstance->playerNo] != -1)
							{
								SetPlayerOutlineColor(pActorInstance, &PlayerOutline[pActorInstance->playerNo], NORMAL_EFFECT);
								
								PlayerEffectTimer[pActorInstance->playerNo] = -1;
							
								pActorInstance->actorStatus->multiplayer.effect = PLAYEREFFECTNORMAL;
							}

							if (mpNumPlayers == 2)
								pActorInstance->actorStatus->multiplayer.pTextBox[TB_POSITION]->setText(EStringIndex(STR_MP_1ST+pActorInstance->actorStatus->multiplayer.position));
						}
				
						if (mpNumPlayers == 2)
						{
							if (mpPlayer[0]->actorStatus->multiplayer.numPickups != mpPlayer[1]->actorStatus->multiplayer.numPickups)
							{
								if (mpPlayer[0]->actorStatus->multiplayer.numPickups > mpPlayer[1]->actorStatus->multiplayer.numPickups)
								{
									mpPlayer[0]->actorStatus->multiplayer.position = 0;
									mpPlayer[1]->actorStatus->multiplayer.position = 1;
								}
								else
								{
									mpPlayer[0]->actorStatus->multiplayer.position = 1;
									mpPlayer[1]->actorStatus->multiplayer.position = 0;
								}
							}
							else
							{
								mpPlayer[0]->actorStatus->multiplayer.position = mpPlayer[1]->actorStatus->multiplayer.position = 0;
							}
						}
			
						break;
			
		case TimeUp:	if (gameStatus.gameTime > TimeUpTimer)
						{
							levelData.RaceState = Finished;

							raceBook->close();
						}
						
						break;
			
		case Finished:	{
							int gameState = GetCurrentGameState(&gameStatus);
								
							if(mpNumPlayers == 2)
							{
								if(mpPlayer[0]->actorStatus->multiplayer.numPickups != mpPlayer[1]->actorStatus->multiplayer.numPickups)
								{
									if(mpPlayer[0]->actorStatus->multiplayer.numPickups > mpPlayer[1]->actorStatus->multiplayer.numPickups)
										levelData.winner = 0;
									else
										levelData.winner = 1;
								}
								else
								{
									levelData.winner = -1;
									bkPrintf("It's a dead heat!\n");
								}
							}
														
							if((gameState != GAMESTATE_CUSTOMPAUSE) && (gameState != GAMESTATE_FADING))
							{
								if (mpNumPlayers == 2)
								{
									// PP: char	sResult[32];

									/* PP: REMOUT: WILSON!!!!!!
									if(levelData.winner == -1)
									{
										// PP: REMOUT: WILSON!!!!!!		sprintf(sResult, "It's a dead heat!");
									}
									else
									{
										// PP: REMOUT: WILSON!!!!!!!!	sprintf(sResult, "Player %d is the winner!", levelData.winner + 1);
									}*/
									
									if (gameStatus.multiplayer.Tournament)
									{
										mpEndRoundMenuSetUp(&levelData.gameFinished, GAMEOVER_NOTFINISHED, levelData.winner);
										SetGameState(&gameStatus, GAMESTATE_CUSTOMPAUSE, GAMESUBSTATE_NORMAL, (int32)mpEndRoundMenuHandler);
									}
									else
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

									mpEndBonusSetUp(&levelData.gameFinished, GAMEOVER_NOTFINISHED, float(mpPlayer[0]->actorStatus->multiplayer.numPickups));
									
									SetGameState(&gameStatus, GAMESTATE_CUSTOMPAUSE, GAMESUBSTATE_NORMAL, (int32)mpEndBonusHandler);
								}
							}
						}
						break;
			}
		}
		break;

	case MPREASON_DRAW:
		// CPW: called (num players) times
		if (CURRENTLY_DRAWING_FULL_SCREEN)
		{
			// CPW: draw Countdown			
			switch (levelData.RaceState)
			{
			/*case Intro:		// JW: Show Intro Screen
							{
							float yRot = gameStatus.gameTime * 2.0f;
							float scale = 0.2f;

							TBVector zero = {0.0f, 0.0f, 0.0f, 0.0f};
							TBVector dir = {0.0f, 0.0f, 0.0f, 0.0f};

							bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);

							bdSetViewPosition(0.0f, 0.0f, 0.0f);
							bdSetViewOrientation(0,0,0,BVPROTATE_XYZ);
							bdSetupView();
							
							if (levelData.pShowEm)
							{
								levelData.pShowEm->actorInstance.position[X] = 0.0f;
								levelData.pShowEm->actorInstance.position[Y] = METERTOUNIT(-0.25f);
								levelData.pShowEm->actorInstance.position[Z] = METERTOUNIT(0.8f);
								levelData.pShowEm->actorInstance.position[W] = 1.0f;
					
								levelData.pShowEm->actorInstance.scale[X] = scale * levelData.globalScale;
								levelData.pShowEm->actorInstance.scale[Y] = scale * levelData.globalScale / mpNumPlayers;
								levelData.pShowEm->actorInstance.scale[Z] = scale * levelData.globalScale;
								levelData.pShowEm->actorInstance.scale[W] = 1.0f;
								
								dir[X] = bmSin(yRot);
								dir[Y] = 0.0f;
								dir[Z] = bmCos(yRot);

								CreateQuatFromPoints(levelData.pShowEm->actorInstance.orientation, zero, dir, 0);
								baPrepActorInstance(&levelData.pShowEm->actorInstance, FALSE);
								baDrawActorInstance(&levelData.pShowEm->actorInstance, BACTPRIMALPHATYPE_ALL);

							}
				
							// CPW: put it back!
							// CPW: put it back!
							SendCamera();// PP: by default, SendCamera now sends a copy of the last camera sent - this prevents lagging
							bdSetupView();

							mpDrawBook();

							}
							break;*/

			case Countdown:	mpCountdownDraw();
							break;
/* PP: REMOUT: Now Handled by the Book List
			case Race:		if (gameStatus.currentState == GAMESTATE_INGAME)
							{
								int		intaT = (int) actualTime;
								float	faT = actualTime - (float) intaT;

								if ((actualTime > 10.0f) || (faT > 0.5f && faT < 0.75f) || (faT > 0.0f && faT < 0.25f))
									mpDrawBook();	
							}
							else if (gameStatus.currentState == GAMESTATE_PAUSE && gameStatus.multiplayer.numScreens == 2)
								DrawMPPausePage();

							break;
*/
			case TimeUp:	mpDrawBook();
							break;
			}
		}

		if ((gameStatus.multiplayer.numScreens == 1 || !CURRENTLY_DRAWING_FULL_SCREEN) && gameStatus.currentState == GAMESTATE_INGAME)
		{
			if (levelData.RaceState == Race)
			{
				ACTORINSTANCE *pActorInstance = mpPlayer[gameStatus.multiplayer.currentScreen];
			
				bdSetIdentityObjectMatrix();
					
				SAVESTATES;
				
				SETPROJECTION(2D);
				SETZTEST(ALWAYS);

// PP: REMOUT: Now Handled by the Book List							pActorInstance->actorStatus->multiplayer.pStatsBook->draw();
				//pActorInstance->actorStatus->multiplayer.pStatsBook->drawDebug();

				// JW: check which screen we're drawing in
				// JW: if (we're in screen one)
				
				if (pActorInstance->playerNo == 0)
				{
// PP: REMOUT: Now Handled by the Book List								levelData.oneBook->draw();
					
					if (levelData.oneChkTime && gameStatus.gameTime > levelData.oneChkTime)
					{
						levelData.oneBook->close();
						levelData.oneChkTime = 0;
					}
				}
				else // JW: if we're in screen two
				{
// PP: REMOUT: Now Handled by the Book List								levelData.twoBook->draw();
					
					if (levelData.twoChkTime && gameStatus.gameTime > levelData.twoChkTime)
					{
						levelData.twoBook->close();
						levelData.twoChkTime = 0;
					}
				}

				SETPROJECTION(3D);	
				RESTORESTATES;
			}
		}
		break;

	case MPREASON_KILL:
		// CPW: called once

		mpCountdownKill();

		BookList_closeAndFree(levelData.oneBook);
		BookList_closeAndFree(levelData.twoBook);

		// TP: Thought you might need this.
		for(int i=0; i<mpNumPlayers; i++)
		{
			ACTORINSTANCE *pActorInstance = mpPlayer[i];
			ASSERT(pActorInstance);
			BookList_closeAndFree(pActorInstance->actorStatus->multiplayer.pStatsBook);
		}
		
		FreeActor("objects\\speed clock.obe");
		FreeActor("objects\\mplayer box.obe");
		FreeActor("objects\\ice clock.obe");

		if (!gameStatus.demoMode)
			setNumScreens(1);
		
		break;
	}

	return 0;
}

void doDestructionGamePickup(ACTORINSTANCE *actorInstance, int type)
{
	if (levelData.RaceState != Race)
		return;

	BOOK	*puBook;
	float	*puTimer;

	int	playerNum = actorInstance->playerNo;
	int	otherPlyNum = 1 - actorInstance->playerNo;

	if (!playerNum)
	{
		puBook = levelData.oneBook;
		puTimer = &levelData.oneChkTime;
	}
	else
	{
		puBook = levelData.twoBook;
		puTimer = &levelData.twoChkTime;
	}

	switch (type)
	{
	case COLLECTIBLETYPE_DE_EXTRATIME:		destructTime -= 5.0f;

											puBook->gotoPage("extra time");
											break;
	
	case COLLECTIBLETYPE_DE_DOUBLECRATE:	SetPlayerOutlineColor(actorInstance, &PlayerOutline[playerNum], YELLOW_EFFECT);
											PlayerEffectTimer[playerNum] = gameStatus.gameTime + 7.0f;
											actorInstance->actorStatus->multiplayer.effect = PLAYEREFFECTDOUBLE;

											puBook->gotoPage("double crates");
											break;

	case COLLECTIBLETYPE_DE_SLOMO:			SetPlayerOutlineColor(mpPlayer[otherPlyNum], &PlayerOutline[otherPlyNum], BLUE_EFFECT);
											PlayerEffectTimer[otherPlyNum] = gameStatus.gameTime + 5.0f;
											mpPlayer[otherPlyNum]->actorStatus->multiplayer.effect = PLAYEREFFECTSLOMO;

											puBook->gotoPage("slo mo");
											break;
	}

	*puTimer = gameStatus.gameTime + 2.0f;
	
	puBook->open();

	numBonusItemsOnScreen --;
}

void SetPlayerOutlineColor(ACTORINSTANCE *actorInstance, TBActRenderMode_Outlined *Outline, int color)
{
	switch (color)
	{
	case YELLOW_EFFECT:	Outline->r = 255;
						Outline->g = 255;
						Outline->b = 0;

						Outline->thickness = 7.0f;
						break;

	case RED_EFFECT:	Outline->r = 255;
						Outline->g = 0;
						Outline->b = 0;

						Outline->thickness = 7.0f;
						break;

	case BLUE_EFFECT:	Outline->r = 0;
						Outline->g = 0;
						Outline->b = 255;

						Outline->thickness = 7.0f;
						break;

	case GREEN_EFFECT:	Outline->r = 0;
						Outline->g = 255;
						Outline->b = 0;

						Outline->thickness = 7.0f;
						break;

	case NORMAL_EFFECT: Outline->r = 0;
						Outline->g = 0;
						Outline->b = 0;

						Outline->thickness = 3.5f;
						break;
	}

	Outline->a = 128;

	if ((!(gameStatus.customOptions&CUSTOMOPTION_DISABLEOUTLINES)))
		baSetActorRenderMode(&actorInstance->actorInstance, 1, BACTRENDERMODE_OUTLINED, Outline);
	// NH: baSetActorRenderMode(&actorInstance->actorInstance, 1, BACTRENDERMODE_OUTLINED, Outline);
}

void DestructibleGameDestroy(ACTORINSTANCE *collider, ACTORINSTANCE *destructible)
{
	if (levelData.RaceState == Race)
	{
		uint32 crcItem = destructible->actorCRC;
			
		if (crcItem == destructData.crcObjLookFor ||
			crcItem == destructData.crcObjLookForContact)	
		{
			if (mpPlayer[collider->playerNo]->actorStatus->multiplayer.effect == PLAYEREFFECTDOUBLE)
				mpPlayer[collider->playerNo]->actorStatus->multiplayer.numPickups += 2;
			else
				mpPlayer[collider->playerNo]->actorStatus->multiplayer.numPickups++;

			lastDestroyer = collider->playerNo;
		}
	}	
}