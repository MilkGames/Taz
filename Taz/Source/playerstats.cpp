// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : playerstats.cpp
//   Purpose : functions to calculate projectile path of object
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "status.h"
#include "trigger.h"
#include "mailbox.h"
#include "wantedposter.h"
#include "main.h"
#include "playerstats.h"
#include "phonebox.h"
#include "display.h"
#include "scene.h"
#include "playerstats.h"
#include "sfx.h"
#include "icon.h"
#include "collectibles.h"
#include "control.h"
#include "endoflevelstats.h"
#include "cameracase.h"
#include "zzz.h"
#include "vehicles.h"
#include "characters.h"
#include "cutscene.h"
#include "files.h"
#include "map.h"
#include "music.h"
#include "boss.h"
#include "thwap.h"
#include "prompt.h"
#include "LanguageMain.h"					// PP: language select stuff

int		phoneTune[10] = 
{
	6000,
	8000,
	7500,
	7500,
	6500,
	6000,
	9000,
	8000,
	7000,
	7500,
};

#ifdef DEFAULT_INITIALS
const uchar				defaultInitials[]={'T','A','Z'};		// PP: default saved-game initials
#endif// PP: def DEFAULT_INITIALS

ACTORINSTANCE			*overlayTaztop;
BOUNTYLIST				bountyList;
int						posterBounties[19];

/* --------------------------------------------------------------------------------
   Function : InitialisePlayerStatsAtStartOfGame
   Purpose : initialises stats when a new game is started
   Parameters : 
   Returns : 
   Info : 
*/

void InitialisePlayerStatsAtStartOfGame(PLAYERSTATS *stats)
{
	int				counter, counterTwo;

	// PP: check for NULL stats ptr
	if(stats == NULL) return;

	stats->taztopCounter = 0;
	stats->bountyDisplay = 0;

	for(counter = 0; counter < 25; counter++)
	{
		for(counterTwo = 0; counterTwo < 10; counterTwo++)
		{
			stats->mailboxCounter[counter][counterTwo] = FALSE;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : InitialisePlayerStatsAtStartOfLevel
   Purpose : initialises stats when a new game is started
   Parameters : 
   Returns : 
   Info : 
*/

void InitialisePlayerStatsAtStartOfLevel(void)
{
	int		i;
	PLAYERSTATS		*stats;

	for(i=0;i<gameStatus.multiplayer.numPlayers;i++)
	{
		stats = gameStatus.player[i].stats;

		stats->numCollectiblesDisplayed = 0.0f;
		stats->numFoodCollected = 0;
		stats->numFoodDisplayed = 0.0f;
		stats->numObjectsDestroyed = 0;
		stats->numDestructiblesDisplayed = 0.0f;
		stats->numPostersDisplayed = 0.0f;
		stats->numBurpCansCollected = 0;
		stats->numBurpCansDisplayed = 0.0f;
		stats->numCashDisplayedEndOfLevel = 0;
		stats->statsBookScale = 0.0f;
		stats->statsBookState = OVERLAYSTATE_ARRIVING;
		stats->statsBookClock = 3.0f;
		stats->endOfLevelStatsBook = NULL;
		//stats->telephoneBountyCounter = 0;
		stats->rankClock = 0.0f;
		stats->displayedRank = STR_PLAYER_RANKING_DEFAULT;
		gameStatus.outroState = OUTRO_ENDCUTSCENE;
		stats->nameEntryStage = HSE_CHOOSEICON;
		stats->iconNumber = 0;

#ifdef DEFAULT_INITIALS
		
		// PP: need a default name, for Xbox TCR 2-06
		stats->letters[0] = defaultInitials[0];// PP: T
		stats->letters[1] = defaultInitials[1];// PP: A
		stats->letters[2] = defaultInitials[2];// PP: Z

#else// PP: no default initials

		stats->letters[0] = HSE_DEFAULT_ASCII;
		stats->letters[1] = HSE_DEFAULT_ASCII;
		stats->letters[2] = HSE_DEFAULT_ASCII;
		
#endif// PP: no default initials
		
	}
}

/* --------------------------------------------------------------------------------
   Function : AddMoneyToBounty
   Purpose : Add to the bounty on a players head
   Parameters : 
   Returns : 
   Info : 
*/
void AddMoneyToBounty(ACTORINSTANCE *actorInstance, ACTORINSTANCE *breakableInstance, uint32 amount)
{
	BOUNTYLISTENTRY		*tempPtr;
	BOUNTYLISTENTRY		*newEntry;
	COLLISIONLISTENTRY	*collEntry;
	char				buf[32];
	float				tempX, tempY;
	PLAYERSTATS		*playerStats = gameStatus.player[actorInstance->playerNo].stats;

	ActivateMoneyOverlay(&gameStatus.player[actorInstance->playerNo]);
	CallbackWhenSampleFinished((characterSounds.sam=PlaySample("samangry_4.wav",200)),CharacterSoundsFinishedCallback,&characterSounds.sam);

	if (amount>100)
	{
		// TP: play sound if lots of money
		PlaySample("taz_gen_18a.wav",255);
	}

	gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty += amount;

	//if((playerStats->telephoneBountyCounter += amount) > 10000)
	//{
	//	playerStats->telephoneBountyCounter = 0;
	//	ActivatePhoneBoxes(actorInstance);
	//}

	if(breakableInstance)
	{
		//add amount to bounty list for drawing bouty at object point
		tempPtr = bountyList.head.next;

		collEntry = GetCollisionEntry(&collisionCache, breakableInstance);
		if (!collEntry) return;

		//create new structure for incoming information
		if(newEntry = (BOUNTYLISTENTRY*)ZALLOC(sizeof(BOUNTYLISTENTRY)))
		{
			newEntry->alpha = 0;
			newEntry->falpha = 0.0f;
			newEntry->font = standardFont;
			newEntry->scale = 0.0f;
			newEntry->state = QUESTIONMARKSTATE_FADEUP;
			sprintf(buf,"$%d",amount);
			bkString8to16(newEntry->string, buf);
			//find position from collision bounding box
			newEntry->position[0] = collEntry->boundingBox.xmin + ((collEntry->boundingBox.xmax - collEntry->boundingBox.xmin)*0.5f);
			newEntry->position[1] = collEntry->boundingBox.ymin + ((collEntry->boundingBox.ymax - collEntry->boundingBox.ymin)*0.5f);
			newEntry->position[2] = collEntry->boundingBox.zmin + ((collEntry->boundingBox.zmax - collEntry->boundingBox.zmin)*0.5f);
			newEntry->position[3] = 1.0f;

			bdFontExtent(standardFont,newEntry->string,bkStringLength16(newEntry->string),&tempX,&tempY);
			newEntry->formatting.height = METERTOUNIT(1.0f);
			newEntry->formatting.width = METERTOUNIT(1.0f);
			newEntry->formatting.horzFormat = EBFONTFORMATTING_LEFT;
			newEntry->formatting.vertFormat = EBFONTFORMATTING_TOP;
			newEntry->formatting.lineSpace = 0.0f;
			newEntry->formatting.wrap = 0;
			newEntry->xOffset = (int)(0.5f*tempX);
			newEntry->yOffset = (int)(0.5f*tempY);

			newEntry->next = tempPtr;
			newEntry->prev = tempPtr->prev;
			tempPtr->prev->next = newEntry;
			tempPtr->prev = newEntry;
			bountyList.numEntries++;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : InitialiseCharacterResetPoints
   Purpose : initialises reset points for characters at the start of a scene
   Parameters : 
   Returns : 
   Info : 
*/

void InitialiseCharacterResetPoints(ACTORINSTANCE *actor, ACTORINSTANCELIST *actorInstanceList)
{
	TBVector			origin = {0.0f, 0.0f, 0.0f, 1.0f};
	ACTORINSTANCE		*ptr;

	ptr = actorInstanceList->head.next;

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if(ptr->special == SPECIALTYPE_RESETPOINT)
		{
			SetStartPosition(actor, ptr->actorInstance.position);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : InitialisePlayerStatsAtStartOfLevel
   Purpose : initialises stats when a new level is started
   Parameters : 
   Returns : 
   Info : 
*/

void InitialisePlayerStatsAtStartOfLevel(PLAYERSTATS *stats)
{
	stats->taztopCounter = 0;
	stats->swirlState = OVERLAYSTATE_OFFSCREEN;
	stats->swirlY = (float)((videoMode.yScreen/2)-30+150);
	stats->destructState = OVERLAYSTATE_OFFSCREEN;
	stats->destructY = (float)((videoMode.yScreen/2)-30+150);
	
	// JW: Set destruct value flags
	stats->destructA = 0;
	stats->destructB = 0;
	stats->destructC = 0;
	stats->destructD = 0;
	stats->destructE = 0;
	stats->destructF = 0;
	stats->destructG = 0;
	stats->destructH = 0;
	stats->destructI = 0;
	stats->destructJ = 0;
	stats->destructK = 0;
	stats->destructL = 0;
	stats->destructM = 0;
	stats->destructN = 0;
	stats->destructO = 0;
	stats->destructP = 0;
	stats->destructQ = 0;
	stats->destructR = 0;
	stats->destructS = 0;
	stats->destructT = 0;

	stats->moneyState = OVERLAYSTATE_OFFSCREEN;
	stats->moneyAlpha = 128;
	stats->moneyScale = 0.0f;
	stats->moneyX = (float)((-videoMode.xScreen/2)+30);
	stats->moneyY = (float)((videoMode.yScreen/2)-30);
	stats->posterState = OVERLAYSTATE_OFFSCREEN;
	stats->posterY = (float)((videoMode.yScreen/2)-30+150);
}

/*  --------------------------------------------------------------------------------
	Function	: LevelCompleted
	Purpose		: Called when the player completes a level
	Parameters	: ACTORINSTANCE of player that triggered the final poster
	Returns		: 
	Info		: 
*/
void LevelCompleted(ACTORINSTANCE *player, int	flags)
{
	// TP: Kill any camera cases
	if (camera[0].cameraCase) StopCameraCase(&camera[0],camera[0].cameraCase);
	if (camera[1].cameraCase) StopCameraCase(&camera[1],camera[1].cameraCase);

	// TP: quick double check there are no outstanding messages that might need the character instances to be present
	RespondToTriggerMessages();

	if (player)
	{
		if (player->characterInfo)
		{
			SAFE_DELETE(player->characterInfo->blinkEyes);
		}
		PlaceActorAtSafePoint(&gameStatus.player[player->playerNo]);

		gameStatus.player[player->playerNo].stats->time = gameStatus.playTime;

		//CalculateGameTimeFormatted(gameStatus.gameTime,&gameStatus.player[player->playerNo].stats->hours,&gameStatus.player[player->playerNo].stats->minutes,&gameStatus.player[player->playerNo].stats->seconds);
	}

	SetGameState(&gameStatus,GAMESTATE_OUTRO,GAMESUBSTATE_NORMAL);

	gameStatus.globalSettings.saveInfo.stats[ConvertSceneRefToLevelScoreRef((EScene)map.sceneNumber)].levelCompleted = true;
	if(gameStatus.globalSettings.currentSaveSlot != -1)
	{
		gameStatus.globalSettings.saveInfo.saveSlot[(int)gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].completed = true;
	}

	// PP: If you have a nicer alternative please let me know.
	// PP: Skip the intro cutscene if we've been to the language select from the front end.
	if((map.exitCutScene) && !((map.sceneNumber==SCENE_LANGUAGESELECT) && skipMainGameIntroCutscene))
	{
		// TP: if there is an exit cutscene available for this level then play it
		gameStatus.outroState = OUTRO_ENDCUTSCENE;

		if ((IN_HUB)||(IN_BOSS_GAME)||(map.sceneNumber==SCENE_LANGUAGESELECT))
		{
			map.exitCutScene->Setup();
			map.exitCutScene->Play(&camera[0]);
		}
		else
		{
			FreeAllActorInstances(&map.characterInstances);
			FreeCollisionCache(&collisionCache);
			if (!map.exitCutScene->Load())
			{
				// TP: delete cutscene as it is not runnable
				SAFE_DELETE(map.exitCutScene);
				if (flags==LEVELCOMPLETE_NOSTATS)
				{
					gameStatus.outroState = OUTRO_EXIT;
				}
				else
				{
					CreateStatsAtEndOfLevel(gameStatus.player[player->playerNo].stats);
					gameStatus.outroState = OUTRO_STATS;
					bkGenerateEvent("camera", "_case=stat_cam");
					RespondToCameraMessages();
				}
			}
			else
			{
				bkGenerateEvent("camera", "_case=outrocam1");
				RespondToCameraMessages();
			}
		}
	}
	else
	{
		// PP: no stats coming out of language select neivah
		if (IN_HUB || (map.sceneNumber == SCENE_LANGUAGESELECT))
		{
			gameStatus.outroState = OUTRO_EXIT;
		}
		else
		{
			if(flags==LEVELCOMPLETE_NOSTATS)
			{
				gameStatus.outroState = OUTRO_EXIT;
			}
			else
			{
				CreateStatsAtEndOfLevel(gameStatus.player[player->playerNo].stats);
				gameStatus.outroState = OUTRO_STATS;
				bkGenerateEvent("camera", "_case=stat_cam");
				RespondToCameraMessages();
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : InitPosterBounties
   Purpose : sets the value of the posters on each level
   Parameters : 
   Returns : 
   Info : 
*/

void InitPosterBounties()
{
	posterBounties[SCENE_ICEDOME] = 2000;
	posterBounties[SCENE_SAFARI] = 1000;
	posterBounties[SCENE_AQUA] = 3000;

	posterBounties[SCENE_DEPTSTR] = 5000;
	posterBounties[SCENE_MUSEUM] = 7500;
	posterBounties[SCENE_CONSTRUCT] = 10000;

	posterBounties[SCENE_GHOSTTOWN] = 75000;
	posterBounties[SCENE_GOLDMINE] = 50000;
	posterBounties[SCENE_GRANDCANYON] = 25000;

	posterBounties[SCENE_TAZHUB] = 100000;
}

/* --------------------------------------------------------------------------------
   Function : AddWantedPosterToStats
   Purpose : 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
int AddWantedPosterToStats(int number, ACTORINSTANCE *player, uint32 level)
{
	int			tempInt;
	PLAYERSTATS	*stats = gameStatus.player[player->playerNo].stats;
	char		message[256];
	TBVector	pos;

	tempInt = number;

	// TP: posters are numbered from 1
	if(tempInt > 0)
	{
		// TP: if the posteer has not already been collected
		if(!gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[level].posters[tempInt-1])
		{
			// TP: set the poster as collected and add money to bounty
			gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[level].posters[tempInt-1] = true;
			//AddMoneyToBounty(player,NULL,posterBounties[map.sceneNumber]);

			// TP: flag poster as complete on the map
			map.pauseMap->PosterCompleted(number);

			// TP: Activate poster stats display
			//ActivatePosterDisplay(&gameStatus.player[player->playerNo]);

			startThwap(STR_FILLER, posterBounties[map.sceneNumber]);

			RequestMorronicMessage(MEVENT_FIRSTPOSTER, 5.0f);

			// TP: Check if this means all the posters have been collected
			if((++gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[level].postersCollected) >= map.mapStats.numPosters)
			{
				gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[level].postersCollected = map.mapStats.numPosters;
				// TP: if there is an onlevelcomplete message then process that instead of doing a forced level complete
				if (map.onLevelComplete)
				{
					map.allPostersDestroyed = true;

					/*
					bmVectorCopy(pos,gameStatus.player[player->playerNo].actorInstance->actorInstance.position);
					sprintf(message,"_POS=%f %f %f",pos[X],pos[Y],pos[Z]);
					bkGenerateEvent("sound",message);
					SendTrigger(map.onLevelComplete);*/
				}
				else
				{
					LevelCompleted(player);
				}
			}
		}
		else
		{
			bkPrintf("***** ERROR ***** poster number duplication\n");
			return false;
		}
	}
	else
	{
		bkPrintf("***** ERROR ***** unidentified wanted poster");
		return false;
	}
	return true;
}

/* --------------------------------------------------------------------------------
   Function : AddMailboxToStats
   Purpose : 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int AddMailboxToStats(ACTORINSTANCE *mailbox, ACTORINSTANCE *player, uint32 level)
{
	uint32				tempInt;
	PLAYERSTATS			*stats = gameStatus.player[player->playerNo].stats;

	if(mailbox->mailboxInfo)
	{
		if(mailbox->special == SPECIALTYPE_MAILBOX)
		{
			tempInt = mailbox->mailboxInfo->mailboxNumber;
			if(tempInt > 0)
			{
				if(!stats->mailboxCounter[level][tempInt-1])
				{
					stats->mailboxCounter[level][tempInt-1] = TRUE;
				}
				else
				{
					bkPrintf("***** ERROR ***** mailbox number duplication\n");
					return FALSE;
				}
			}
			else
			{
				bkPrintf("***** ERROR ***** unidentified mailbox");
			}
		}
	}
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ZeroTazTopCounter
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/
void ZeroTazTopCounter(ACTORINSTANCE *actorInstance)
{/*
	if (actorInstance == gameStatus.player1)
	{
		gameStatus.player1Stats.taztopCounter = 0;
	}
	if (actorInstance == gameStatus.player2)
	{
		gameStatus.player2Stats.taztopCounter = 0;
	}*/
}

/* --------------------------------------------------------------------------------
   Function : IncrementTazTopCounter
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/
void IncrementTazTopCounter(ACTORINSTANCE *actorInstance, uint32 level)
{
	PLAYERSTATS		*playerStats;

	playerStats = gameStatus.player[actorInstance->playerNo].stats;

	ActivateSwirlOverlay(&gameStatus.player[actorInstance->playerNo]);

	playerStats->swirlIcon->setMaxTilt(1.0f);
	actorInstance->actorStatus->timeSinceLastTazTopCollected = 0.0f;
	playerStats->taztopCounter++;
}

/* --------------------------------------------------------------------------------
   Function : CalculateTotalPauseStats
   Purpose : 
   Parameters : char *
   Returns : 
   Info : 
*/

uint32 CalculateTotalPauseStats(char stats[25][10])
{
	int			levelCounter;
	int			counter;
	int			total = 0;

	for(levelCounter = 0; levelCounter < 25; levelCounter++)
	{
		for(counter = 0; counter < 10; counter++)
		{
			if(stats[levelCounter][counter] == TRUE)
			{
				total++;
			}
		}
	}
	return total;
}

/* --------------------------------------------------------------------------------
   Function : CalculatePauseStatsForLevel
   Purpose : 
   Parameters : char *, uint32 level
   Returns : 
   Info : 
*/

uint32 CalculatePauseStatsForLevel(char stats[25][10], uint32 level)
{
	int			counter;
	uint32		total = 0;

	for(counter = 0; counter < 10; counter++)
	{
		if(stats[level][counter] == TRUE)
		{
			total++;
		}
	}
	return total;
}

/* --------------------------------------------------------------------------------
   Function : InitOverlay
   Purpose : 
   Parameters :
   Returns : 
   Info : 
*/

void InitOverlay(void)
{
	TBVector		position = {0.0f, 0.0f, 0.1f, 1.0f};
	TBVector		scale = {1.0f, 1.0f, 0.001f, 1.0f};
	
	overlayTaztop = CreateActorInstance(&map.invisibleInstances, "restaztop.obe", "taztopCounter");
	if (overlayTaztop==NULL) return;
	bmVectorCopy(overlayTaztop->actorInstance.position, position);
	bmVectorCopy(overlayTaztop->actorInstance.scale, scale);
	baPrepActorInstance(&overlayTaztop->actorInstance,TRUE);
}

/* --------------------------------------------------------------------------------
   Function : DrawOverlay
   Purpose : 
   Parameters :
   Returns : 
   Info : 
*/

void DrawOverlay(PLAYERSTATS *player)
{
	char			buf[256];
	ushort			string16[256];

	if (overlayTaztop==NULL) return;

	bdSetIdentityObjectMatrix();
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	//bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	baDrawActorInstance(&overlayTaztop->actorInstance,BSETPRIMALPHATYPE_SOLID|BSETPRIMALPHATYPE_CKEY|BSETPRIMALPHATYPE_ALPHA);

	sprintf(buf, "qwertyuiop[]%d", player->taztopCounter);
	bkString8to16(string16, buf);
	bdPrintFontLite(standardFont, string16, strlen(buf), (float)((-videoMode.xScreen)/2+20),(float)((videoMode.yScreen)/2), 127,127,127, 127, NULL, NULL);

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
}

/* --------------------------------------------------------------------------------
   Function : UpdateOverlay
   Purpose : 
   Parameters :
   Returns : 
   Info : 
*/

void UpdateOverlay(void)
{
}

/*  --------------------------------------------------------------------------------
	Function	: DrawPlayerStats
	Purpose		: Draw the players stats and overlays
	Parameters	: PLAYER
	Returns		: 
	Info		: 
*/
void DrawPlayerStats(PLAYER *player)
{
	char			buf[256];
	char			tempString[16];
	ushort			string16[256];
	int				i,count;
	COLOUR			colour;
	
	bdSetIdentityObjectMatrix();
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	// PP: ----------------
	// PP: NOTE: AT PRESENT, TEXTBOX::sprintf8 WILL ALWAYS INCUR A BIT OF REALIGNMENT PROCESSING.
	// PP: I RECOMMEND AGAINST CALLING IT NEEDLESSLY (EG. WHEN ITS STRING WILL BE UNCHANGED OR ITS CONTAINER IS CLOSED)
	// PP: ----------------

	player->stats->swirlText->sprintf8("x %d", gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].pickupsCollected);
/* PP: REMOUT: Now Handled by the Book List			player->stats->swirlOverlay->update();
	player->stats->swirlOverlay->draw();*/

	player->stats->destructText->sprintf8("%d %%", (int) (gameStatus.player[0].stats->numObjectsDestroyed / (float) map.mapStats.numDestructibles * 100.0f));
/* PP: REMOUT: Now Handled by the Book List			player->stats->destructOverlay->update();
	player->stats->destructOverlay->draw();*/
	
	count = 0;
	for (i=0;i<10;i++)
	{
		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].posters[i] == TRUE)
		{
			count++;
		}
	}

	player->stats->posterText->sprintf8("%d/%d", count,map.mapStats.numPosters);
/* PP: REMOUT: Now Handled by the Book List				player->stats->posterOverlay->draw();
	player->stats->posterOverlay->update();*/
/*
	sprintf(buf, "$%d", player->stats->bountyDisplay);

	bkString8to16(string16, buf);
	TextFX_vGrad(128,128,0,110,40,0);
	bdPrintFontLite(standardFont, string16, strlen(buf), (float)((-videoMode.xScreen+60)/2),(float)((videoMode.yScreen)/2)-40, 128,128,128, 128, TextFX_escapeCallback, NULL);
	TextFX_resetColour();*/
	
	bdSetIdentityObjectMatrix();
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	utilFormatPrettyNumbers(player->stats->bountyDisplay,tempString);
	sprintf(buf,"$%s",tempString);
	bkString8to16(string16, buf);
	bdPrintFontLite(standardFont,string16,strlen(buf),player->stats->moneyX,player->stats->moneyY,128,128,128,player->stats->moneyAlpha,MoneyOverlayCallBack,player->stats);

// PP: REMOUT: Now Handled by the Book List				DrawStatsAtEndOfLevel(player->stats);

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
}

/*  --------------------------------------------------------------------------------
	Function	: UpdatePlayerStats
	Purpose		: Update the players stats and overlays
	Parameters	: PLAYER
	Returns		: 
	Info		: 
*/
void UpdatePlayerStats(PLAYER *player)
{
	uint32		tempInt;

	static RECTANGLE onScreen((float)((videoMode.xScreen/2)-200),(float)(videoMode.xScreen/2),(float)((videoMode.yScreen/2)-30),(float)((videoMode.yScreen/2)-70));
	static RECTANGLE offScreen((float)((videoMode.xScreen/2)-200),(float)(videoMode.xScreen/2),(float)((videoMode.yScreen/2)-30+150),(float)((videoMode.yScreen/2)-70+150));

	switch(player->stats->swirlState)
	{
	case OVERLAYSTATE_OFFSCREEN:
		player->stats->swirlScale = 0.0f;
		player->stats->swirlIcon->setDrawScale(player->stats->swirlScale);
		break;
	case OVERLAYSTATE_ARRIVING:
		player->stats->swirlScale = 0.9f *(1.0f - (bmCos(RAD(player->stats->swirlTime*360.0f))*bmCos(RAD(player->stats->swirlTime*(90.0f/OVERLAY_TIME)))));
		player->stats->swirlIcon->setDrawScale(player->stats->swirlScale);
		if(((player->stats->swirlTime += fTime) > OVERLAY_TIME))
		{
			player->stats->swirlState = OVERLAYSTATE_LEAVING;
			player->stats->swirlTime = 0.0f;
			player->stats->swirlIcon->setDrawScale(0.9f);
		}
		break;
	case OVERLAYSTATE_ONSCREEN:
		player->stats->swirlTime -= fTime;
		if (GetCurrentGameState(&gameStatus) != GAMESTATE_PAUSE)
		{
			if (player->stats->swirlTime < 0.0f) player->stats->swirlState = OVERLAYSTATE_LEAVING;
		}
		break;
	case OVERLAYSTATE_LEAVING:
		player->stats->swirlOverlay->close();
		player->stats->swirlState = OVERLAYSTATE_OFFSCREEN;
		break;
	}
	switch(player->stats->destructState)
	{
	case OVERLAYSTATE_OFFSCREEN:
		player->stats->destructScale = 0.0f;
		player->stats->destructIcon->setDrawScale(player->stats->destructScale);
		break;
	case OVERLAYSTATE_ARRIVING:
		player->stats->destructScale = 0.9f *(1.0f - (bmCos(RAD(player->stats->destructTime*360.0f))*bmCos(RAD(player->stats->destructTime*(90.0f/OVERLAY_TIME)))));
		player->stats->destructIcon->setDrawScale(player->stats->destructScale);
		if(((player->stats->destructTime += fTime) > OVERLAY_TIME))
		{
			player->stats->destructState = OVERLAYSTATE_LEAVING;
			player->stats->destructTime = 0.0f;
			player->stats->destructIcon->setDrawScale(0.9f);
		}
		break;
	case OVERLAYSTATE_ONSCREEN:
		player->stats->destructTime -= fTime;
		if (GetCurrentGameState(&gameStatus) != GAMESTATE_PAUSE)
		{
			if (player->stats->destructTime < 0.0f) player->stats->destructState = OVERLAYSTATE_LEAVING;
		}
		break;
	case OVERLAYSTATE_LEAVING:
		player->stats->destructOverlay->close();
		player->stats->destructState = OVERLAYSTATE_OFFSCREEN;
		break;
	}
	switch(player->stats->moneyState)
	{
	case OVERLAYSTATE_OFFSCREEN:
		break;
	case OVERLAYSTATE_ARRIVING:
		player->stats->moneyTime += fTime;
		player->stats->moneyAlpha = 128;
		if(player->stats->moneyTime > 1.0f+OVERLAYMONEY_TIME)
		{
			player->stats->moneyTime = 0.0f;
			player->stats->moneyState = OVERLAYSTATE_ONSCREEN;
		}
		break;
	case OVERLAYSTATE_ONSCREEN:
		player->stats->moneyTime += fTime;
		player->stats->moneyAlpha = 128;
/*		if((tempInt = (uint32)(OVERLAYMONEY_FADERATE*fTime*player->stats->moneyTime*player->stats->moneyTime)) > gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty)
		{
			tempInt = gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty;
		}
		gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty -= tempInt;*/

		player->stats->bountyDisplay = gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty;

		//if((gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty == 0))
		{
			player->stats->moneyState = OVERLAYSTATE_DELAY;
			player->stats->moneyTime = 1.0f;
		}		
		break;
	case OVERLAYSTATE_DELAY:
		player->stats->moneyTime -= fTime;
		player->stats->moneyAlpha = 128;
		if (player->stats->moneyTime <= 0.0f)
		{
			player->stats->moneyTime = OVERLAYMONEY_TIME;
			player->stats->moneyState = OVERLAYSTATE_LEAVING;
		}
		break;
	case OVERLAYSTATE_LEAVING:
		player->stats->moneyTime -= fTime;
		if((player->stats->moneyAlpha = (int)(128.0f*player->stats->moneyTime)) < 0)
		{
			player->stats->moneyAlpha = 0;
		}
		if(player->stats->moneyTime < 0.0f)
		{
			player->stats->moneyAlpha = 0;
			player->stats->moneyState = OVERLAYSTATE_OFFSCREEN;
		}
		break;
	}
	switch(player->stats->posterState)
	{
	case OVERLAYSTATE_OFFSCREEN:
		player->stats->posterScale = 0.0f;
		player->stats->posterIcon->setDrawScale(player->stats->posterScale);
		break;
	case OVERLAYSTATE_ARRIVING:
		player->stats->posterScale = 1.0f - (bmCos(RAD(player->stats->posterTime*360.0f))*bmCos(RAD(player->stats->posterTime*(90.0f/OVERLAY_TIME))));
		player->stats->posterIcon->setDrawScale(player->stats->posterScale);
		if(((player->stats->posterTime += fTime) > OVERLAY_TIME))
		{
			player->stats->posterState = OVERLAYSTATE_LEAVING;
			player->stats->posterTime = 0.0f;
			player->stats->posterIcon->setDrawScale(1.0f);
		}
		break;
	case OVERLAYSTATE_ONSCREEN:
		player->stats->posterTime -= fTime;
		if ((player->stats->posterTime < 0.0f))
		{
			player->stats->posterState = OVERLAYSTATE_LEAVING;
		}
		break;
	case OVERLAYSTATE_LEAVING:
		player->stats->posterOverlay->close();
		player->stats->posterState = OVERLAYSTATE_OFFSCREEN;
		break;
	}
	UpdateStatsAtEndOfLevel(player->stats);
}

/*  --------------------------------------------------------------------------------
	Function	: ClosePlayerStats
	Purpose		: Close any player stats
	Parameters	: PLAYER
	Returns		: 
	Info		: 
*/
void ClosePlayerStats(PLAYER *player)
{
	player->stats->swirlOverlay->close();
	player->stats->posterOverlay->close();
	player->stats->destructOverlay->close();
}

/*  --------------------------------------------------------------------------------
	Function	: DeletePlayerStats
	Purpose		: Delete any player stats
	Parameters	: PLAYER
	Returns		: 
	Info		: 
*/
void DeletePlayerStats(PLAYER *player)
{
	delete player->stats->swirlOverlay;
	delete player->stats->posterOverlay;
	delete player->stats->destructOverlay;
}

/*  --------------------------------------------------------------------------------
	Function	: InitialisePlayerStats
	Purpose		: Initialise the player stats and all overlays
	Parameters	: PLAYER*
	Returns		: 
	Info		: 
*/
void InitialisePlayerStats(PLAYER *player)
{
	PAGE	*mypage;

	player->stats->swirlOverlay = new BOOK();
	
	// PP: give the book a name, for debugging
	player->stats->swirlOverlay->setName("SWIRLOVERLAYBOOK");

	player->stats->swirlRect.init(0.3f, 0.625f, 0.433f, 0.34375f);// PP: REMOUT: must now be expressed as fractions of the screen dimensions	(float)((videoMode.xScreen/2)-200.0f),(float)(videoMode.xScreen/2),(float)((videoMode.yScreen/2)-30.0f),(float)((videoMode.yScreen/2)-70));
	player->stats->swirlOverlay->setBookRect(player->stats->swirlRect);
	mypage = player->stats->swirlOverlay->addPage(player->stats->swirlIcon = new ICON("sandwich.bmp"), NULL, "SWIRLOVERLAYPAGE");
	player->stats->swirlIcon->setAlignScale(1.0f);
	player->stats->swirlIcon->setDrawScale(0.0f);
	player->stats->swirlIcon->setMaxTiltDecay(1.5f);
	player->stats->swirlIcon->setLag(0.3f);
	
	mypage->insertItem(player->stats->swirlText = new TEXTBOX(), false);
	player->stats->swirlText->setXAlign(PIFLAG_XALIGN_RIGHT);
	player->stats->swirlText->setScale(1.5f);


	player->stats->swirlText->sprintf8("x %d", gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].pickupsCollected);
	
	player->stats->swirlScale = 0.0f;
	player->stats->swirlTime = 0.0f;

	player->stats->destructOverlay = new BOOK();
	
	// PP: give the book a name, for debugging
	player->stats->destructOverlay->setName("DESTRUCTOVERLAYBOOK");

	player->stats->destructRect.init(0.3f, 0.625f, 0.433f, 0.34375f);// PP: REMOUT: must now be expressed as fractions of the screen dimensions	(float)((videoMode.xScreen/2)-200.0f),(float)(videoMode.xScreen/2),(float)((videoMode.yScreen/2)-30.0f),(float)((videoMode.yScreen/2)-70));
	player->stats->destructOverlay->setBookRect(player->stats->destructRect);
	mypage = player->stats->destructOverlay->addPage(player->stats->destructIcon = new ICON("destructibles02.bmp"), NULL, "destructOVERLAYPAGE");
	player->stats->destructIcon->setAlignScale(1.0f);
	player->stats->destructIcon->setDrawScale(0.0f);
	player->stats->destructIcon->setMaxTiltDecay(2.0f);
	player->stats->destructIcon->setLag(0.3f);
	
	mypage->insertItem(player->stats->destructText = new TEXTBOX(), false);
	player->stats->destructText->setXAlign(PIFLAG_XALIGN_RIGHT);
	player->stats->destructText->setScale(1.5f);

	player->stats->destructText->sprintf8("x %d", gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].pickupsCollected);
	player->stats->destructScale = 0.0f;
	player->stats->destructTime = 0.0f;

	player->stats->moneyState = OVERLAYSTATE_OFFSCREEN;
	player->stats->moneyAlpha = 128;
	player->stats->moneyScale = 0.0f;
	player->stats->moneyX = 0;
	player->stats->moneyY = 0;

// PP: REMOUT: 	player->stats->moneyText->sprintf("10000");

	player->stats->posterOverlay = new BOOK();

	// PP: give the book a name, for debugging
	player->stats->posterOverlay->setName("POSTEROVERLAYBOOK");

	player->stats->posterRect.init(-0.15625f, 0.15625f, 0.433f, 0.34375f);// PP: REMOUT: must now be expressed as fractions of the screen dimensions	(-100.0f,100.0f,(float)((videoMode.yScreen/2)-30),(float)((videoMode.yScreen/2)-70));
	player->stats->posterOverlay->setBookRect(player->stats->posterRect);
	mypage = player->stats->posterOverlay->addPage(player->stats->posterIcon = new ICON("tazwanted.bmp"), NULL, "POSTEROVERLAYPAGE");
	player->stats->posterIcon->setAlignScale(2.0f);
	player->stats->posterIcon->setDrawScale(0.0f);
	player->stats->posterIcon->tilt=false;
	mypage->insertItem(player->stats->posterText = new TEXTBOX(), false);

// PP: REMOUT: 	player->stats->posterText->sprintf("10000");
	player->stats->posterScale = 0.0f;
	player->stats->posterTime = 0.0f;

}

/*  --------------------------------------------------------------------------------
	Function	: ActivateSwirlOverlay
	Purpose		: Active the pull down swirl overlay
	Parameters	: PLAYER
	Returns		: 
	Info		: 
*/
void ActivateSwirlOverlay(PLAYER *player)
{
	PLAYERSTATS *stats = player->stats;

	if ((stats->swirlState == OVERLAYSTATE_OFFSCREEN)||(stats->swirlState == OVERLAYSTATE_LEAVING))
	{
		if (stats->destructState != OVERLAYSTATE_OFFSCREEN && stats->destructState != OVERLAYSTATE_LEAVING)
			stats->destructState = OVERLAYSTATE_LEAVING;

		stats->swirlState = OVERLAYSTATE_ARRIVING;
		stats->swirlOverlay->open();
	}
}

/*  --------------------------------------------------------------------------------
	Function	: ActivateSwirlOverlay
	Purpose		: Active the pull down swirl overlay
	Parameters	: PLAYER
	Returns		: 
	Info		: 
*/
void ActivateDestructOverlay(PLAYER *player)
{
	if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level->destructBonusCollected)
		return;

	PLAYERSTATS *stats = player->stats;

	if ((stats->destructState == OVERLAYSTATE_OFFSCREEN)||(stats->destructState == OVERLAYSTATE_LEAVING))
	{
		if (stats->swirlState != OVERLAYSTATE_OFFSCREEN && stats->swirlState != OVERLAYSTATE_LEAVING)
			stats->swirlState = OVERLAYSTATE_LEAVING;
		
		stats->destructState = OVERLAYSTATE_ARRIVING;
		stats->destructOverlay->open();
	}
}

/*  --------------------------------------------------------------------------------
	Function	: ActivateMoneyOverlay
	Purpose		: Active the pull down swirl overlay
	Parameters	: PLAYER
	Returns		: 
	Info		: 
*/
void ActivateMoneyOverlay(PLAYER *player)
{
	PLAYERSTATS *stats = player->stats;

	if ((stats->moneyState == OVERLAYSTATE_OFFSCREEN)||(stats->moneyState == OVERLAYSTATE_LEAVING))
	{
		stats->moneyState = OVERLAYSTATE_ARRIVING;
		stats->moneyTime = 1.0f;
		stats->moneyScale = 0;
	}
}

/*  --------------------------------------------------------------------------------
	Function	: ActivatePosterDisplay
	Purpose		: Activate the poster status display
	Parameters	: PLAYER
	Returns		: 
	Info		: 
*/
void ActivatePosterDisplay(PLAYER *player)
{
	PLAYERSTATS *stats = player->stats;

	if ((stats->posterState == OVERLAYSTATE_OFFSCREEN)||(stats->posterState == OVERLAYSTATE_LEAVING))
	{
		stats->posterState = OVERLAYSTATE_ARRIVING;
		stats->posterOverlay->open();
	}
}

/* --------------------------------------------------------------------------------
   Function : InitBountyList
   Purpose : guess what it does!
   Parameters : 
   Returns : 
   Info : 
*/

void InitBountyList(void)
{
	bountyList.numEntries = 0;
	bountyList.head.next = bountyList.head.prev = &bountyList.head;
}

/* --------------------------------------------------------------------------------
   Function : UpdateBountyList
   Purpose : guess what it does!
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateBountyList(BOUNTYLIST *bountyList)
{
	BOUNTYLISTENTRY	*ptr,*temp;
	TBVector		destVec;
	int				or, and;

	for(ptr = bountyList->head.next;ptr != &bountyList->head;)
	{
//#if BPLATFORM == PC
		temp = ptr->next;
		bdProjectClipVertices(destVec, ptr->position, 1, &and, &or);
		if(or == 0)
		{
			ptr->formatting.x = destVec[0] - (float)ptr->xOffset;
			ptr->formatting.y = destVec[1] - (float)ptr->yOffset;
			ptr->scale += 1.0f*fTime;
		}
		else
		{
			RemoveBountyFromList(ptr);
			ptr = temp;
			continue;
		}
//#endif
/*#if BPLATFORM == PS2
		RemoveBountyFromList(ptr);
		continue;
#endif
*/
		switch(ptr->state)
		{
		case QUESTIONMARKSTATE_FADEUP:
			if((ptr->falpha += fTime*OVERLAYMONEY_FADERATE) > 128.0f)
			{
				ptr->falpha = 128.0f;
				ptr->state = QUESTIONMARKSTATE_FADEDOWN;
			}
			ptr->alpha = (int)ptr->falpha;
			break;
		case QUESTIONMARKSTATE_FADEDOWN:
			if((ptr->falpha -= fTime*OVERLAYMONEY_FADERATE) < 0.0f)
			{
				ptr->falpha = 0.0f;
				RemoveBountyFromList(ptr);
			}
			ptr->alpha = (int)ptr->falpha;
			break;
		}
		ptr = temp;
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateBountyList
   Purpose : guess what it does!
   Parameters : 
   Returns : 
   Info : 
*/

void RemoveBountyFromList(BOUNTYLISTENTRY *remPtr)
{
	BOUNTYLISTENTRY  *ptr, *temp;

	if(remPtr == NULL)
	{
		bkPrintf("**** ERROR **** pointer to be removed from falling list is NULL");
	}

	for (ptr = bountyList.head.next;ptr != &bountyList.head;)
	{
		if (ptr == remPtr)
		{
			ptr->prev->next = ptr->next;
			ptr->next->prev = ptr->prev;
			bountyList.numEntries--;
		
			temp = ptr->next;
			FREE(ptr);
			ptr = temp;
		}
		else
		{
			ptr = ptr->next;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawBountyList
   Purpose : guess what it does!
   Parameters : 
   Returns : 
   Info : 
*/

int DrawBountyList(BOUNTYLIST *bountyList)
{
	int					result = FALSE;
	BOUNTYLISTENTRY		*ptr;
	TBMatrix			scale, trans, resultMat;
	TBFontFormatting	tempFormat;

	bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
	bdSetRenderState(BDRENDERSTATE_TEXTUREFILTER, gameStatus.gfxSettings.filterMode, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST,BDZTESTMODE_ALWAYS, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);

	for(ptr = bountyList->head.next;ptr != &bountyList->head;ptr = ptr->next)
	{
		result = TRUE;
		bmMatTranslate(trans, (float)ptr->formatting.x, (float)ptr->formatting.y, 0.0f);
		bmMatScale(scale, ptr->scale, ptr->scale, ptr->scale);
		bmMatMultiply(resultMat, trans, scale);
		bdSetObjectMatrix(resultMat);
		//copy format
		tempFormat.height = ptr->formatting.height;
		tempFormat.horzFormat = ptr->formatting.horzFormat;
		tempFormat.lineSpace = ptr->formatting.lineSpace;
		tempFormat.vertFormat = ptr->formatting.vertFormat;
		tempFormat.width = ptr->formatting.width;
		tempFormat.wrap = ptr->formatting.wrap;
		tempFormat.x = 0;
		tempFormat.y = 0;
		bdPrintFont(ptr->font,ptr->string,bkStringLength16(ptr->string),&tempFormat,128,128,128,ptr->alpha,NULL,NULL);
	}

	bdSetIdentityObjectMatrix();
	bdSetRenderState(BDRENDERSTATE_ZTEST,BDZTESTMODE_LESSTHAN, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	return result;
}

/* --------------------------------------------------------------------------------
   Function : MoneyOverlayCallBack
   Purpose : callback for when bounty is drawn
   Parameters : 
   Returns : 
   Info : 
*/

int MoneyOverlayCallBack(EBFontCallbackReason reason, TBFontCallbackInfo *info)
{
	TBPrimVertex	*verts;
	PLAYERSTATS		*stats;
	TBVector		position;
	int				counter;//,r,g,b;
#if BPLATFORM == PS2
	float			r,g,b,a;
#else
	int				r,g,b,a;
#endif
	float			tempFloat;

	if(reason == EBFONTCALLBACKREASON_POLY)
	{
		verts = info->curVertex;
		stats = (PLAYERSTATS*)info->context;

		switch(stats->moneyState)
		{
		case OVERLAYSTATE_OFFSCREEN:
			stats->moneyAlpha = 0;
			break;
		case OVERLAYSTATE_ARRIVING:
			
			for(counter=0;counter<4;counter++)
			{
				//scale
				/*
				BDVERTEX_GETXYZW(&verts[counter],position[0],position[1],position[2],position[3]);

				position[0] -= stats->moneyX;
				position[0] *= OVERLAYMONEY_SCALE;
				position[0] += stats->moneyX;

				position[1] -= stats->moneyY;
				position[1] *= OVERLAYMONEY_SCALE;
				position[1] += stats->moneyY;

				BDVERTEX_SETXYZW(&verts[counter],position[0],position[1],position[2],position[3]);
				//position
				BDVERTEX_GETXYZW(&verts[counter],position[0],position[1],position[2],position[3]);

				tempFloat = 300.0f*bmCos((stats->moneyTime*RAD(720.0f))+(position[0]-stats->moneyX))/(30.0f*stats->moneyTime);
				switch(counter)
				{
				case 0:
				case 2:
					position[1] -= tempFloat;
					break;
				case 1:
				case 3:
					position[1] += tempFloat;
					break;
				}
				switch(counter)
				{
				case 0:
				case 1:
					position[0] -= 0.5f*tempFloat;
					break;
				case 2:
				case 3:
					position[0] += 0.5f*tempFloat;
					break;
				}

				BDVERTEX_SETXYZW(&verts[counter],position[0],position[1],position[2],position[3]);
				//colour
				BDVERTEX_GETRGBA(&verts[counter],r,g,b,a);
				if((counter == 1)||(counter == 3))
				{
					r = 128; g = 64; b = 8;
				}
				else
				{
					r = 128;
					g = 64 + (int)(42.0f*tempFloat/3.3f);
					b = 8 + (int)(2.0f*tempFloat/3.3f);
				}
				BDVERTEX_SETRGBA(&verts[counter],r,g,b,a);*/
				//scale
				BDVERTEX_GETXYZW(&verts[counter],position[0],position[1],position[2],position[3]);

				position[0] -= stats->moneyX;
				position[0] *= OVERLAYMONEY_SCALE;
				position[0] += stats->moneyX;

				position[1] -= stats->moneyY;
				position[1] *= OVERLAYMONEY_SCALE;
				position[1] += stats->moneyY;

				BDVERTEX_SETXYZW(&verts[counter],position[0],position[1],position[2],position[3]);
				//position
				BDVERTEX_GETXYZW(&verts[counter],position[0],position[1],position[2],position[3]);

				tempFloat = 300.0f*bmCos(((stats->moneyTime+1.0f)*RAD(720.0f))+(position[0]-stats->moneyX))/(30.0f*(stats->moneyTime+1.0f));
				switch(counter)
				{
				case 0:
				case 2:
					position[1] -= tempFloat;
					break;
				case 1:
				case 3:
					position[1] += tempFloat;
					break;
				}
				switch(counter)
				{
				case 0:
				case 1:
					position[0] -= 0.5f*tempFloat;
					break;
				case 2:
				case 3:
					position[0] += 0.5f*tempFloat;
					break;
				}

				BDVERTEX_SETXYZW(&verts[counter],position[0],position[1],position[2],position[3]);
				//colour
				BDVERTEX_GETRGBA(&verts[counter],r,g,b,a);
				if((counter == 1)||(counter == 3))
				{
					r = 128; g = 64; b = 8;
				}
				else
				{
					r = 128;
					g = 64 + (int)(42.0f*tempFloat/3.3f);
					b = 8 + (int)(2.0f*tempFloat/3.3f);
				}
				BDVERTEX_SETRGBA(&verts[counter],r,g,b,a);
			}
			break;
		case OVERLAYSTATE_ONSCREEN:
			for(counter=0;counter<4;counter++)
			{
				//scale
				BDVERTEX_GETXYZW(&verts[counter],position[0],position[1],position[2],position[3]);

				position[0] -= stats->moneyX;
				position[0] *= OVERLAYMONEY_SCALE;
				position[0] += stats->moneyX;

				position[1] -= stats->moneyY;
				position[1] *= OVERLAYMONEY_SCALE;
				position[1] += stats->moneyY;

				BDVERTEX_SETXYZW(&verts[counter],position[0],position[1],position[2],position[3]);
				//position
				BDVERTEX_GETXYZW(&verts[counter],position[0],position[1],position[2],position[3]);

				tempFloat = 300.0f*bmCos(((stats->moneyTime+1.0f)*RAD(720.0f))+(position[0]-stats->moneyX))/(30.0f*(stats->moneyTime+1.0f));
				switch(counter)
				{
				case 0:
				case 2:
					position[1] -= tempFloat;
					break;
				case 1:
				case 3:
					position[1] += tempFloat;
					break;
				}
				switch(counter)
				{
				case 0:
				case 1:
					position[0] -= 0.5f*tempFloat;
					break;
				case 2:
				case 3:
					position[0] += 0.5f*tempFloat;
					break;
				}

				BDVERTEX_SETXYZW(&verts[counter],position[0],position[1],position[2],position[3]);
				//colour
				BDVERTEX_GETRGBA(&verts[counter],r,g,b,a);
				if((counter == 1)||(counter == 3))
				{
					r = 128; g = 64; b = 8;
				}
				else
				{
					r = 128;
					g = 64 + (int)(42.0f*tempFloat/3.3f);
					b = 8 + (int)(2.0f*tempFloat/3.3f);
				}
				BDVERTEX_SETRGBA(&verts[counter],r,g,b,a);
			}
			break;
		case OVERLAYSTATE_DELAY:
			for(counter=0;counter<4;counter++)
			{
				//scale
				BDVERTEX_GETXYZW(&verts[counter],position[0],position[1],position[2],position[3]);

				position[0] -= stats->moneyX;
				position[0] *= OVERLAYMONEY_SCALE;
				position[0] += stats->moneyX;

				position[1] -= stats->moneyY;
				position[1] *= OVERLAYMONEY_SCALE;
				position[1] += stats->moneyY;

				BDVERTEX_SETXYZW(&verts[counter],position[0],position[1],position[2],position[3]);
				//colour
				BDVERTEX_GETRGBA(&verts[counter],r,g,b,a);
				if((counter == 1)||(counter == 3))
				{
					r = 128; g = 64; b = 8;
				}
				else
				{
					r = 128; g = 106; b = 10;
				}
				BDVERTEX_SETRGBA(&verts[counter],r,g,b,a);
			}
			break;
		case OVERLAYSTATE_LEAVING:
			for(counter=0;counter<4;counter++)
			{
				//scale
				BDVERTEX_GETXYZW(&verts[counter],position[0],position[1],position[2],position[3]);

				position[0] -= stats->moneyX;
				position[0] *= OVERLAYMONEY_SCALE;
				position[0] += stats->moneyX;

				position[1] -= stats->moneyY;
				position[1] *= OVERLAYMONEY_SCALE;
				position[1] += stats->moneyY;

				BDVERTEX_SETXYZW(&verts[counter],position[0],position[1],position[2],position[3]);
				//colour
				BDVERTEX_GETRGBA(&verts[counter],r,g,b,a);
				if((counter == 1)||(counter == 3))
				{
					r = 128; g = 64; b = 8;
				}
				else
				{
					r = 128; g = 106; b = 10;
				}
				BDVERTEX_SETRGBA(&verts[counter],r,g,b,a);
			}
			break;
		}
		return 1;
	}
	return 0;
}

/* --------------------------------------------------------------------------------
   Function : PlaceActorAtSafePoint
   Purpose : call in order to hide and suspend taz
   Parameters : 
   Returns : 
   Info : 
*/

int PlaceActorAtSafePoint(PLAYER *player)
{
	ACTORINSTANCE	*actorInstance = player->actorInstance;
	CAMERAINFO		*cameraInfo = player->camera;

	actorInstance->flags |= ACTORFLAG_DONTDRAW|ACTORFLAG_NOCOLLISION|ACTORFLAG_NOVELOCITY;
	actorInstance->actorStatus->lastState = actorInstance->actorStatus->currentState;
	actorInstance->actorStatus->currentState = STATE_MOVE;
	gameStatus.controlSuspend = true;

	if (cameraInfo)
	{
		// TP: If there is a camera then better stop it looking at taz
		DisableCameraLookAtFollowActorInstance(cameraInfo);
		SetCameraPosFollowActorInstance(cameraInfo, NULL, NULL);
	}

	if(actorInstance->actorStatus->vehicleInfo)
	{
		RemoveObjectFromCharacter(actorInstance, actorInstance->actorStatus->vehicleInfo->handle);
		//reset camera
		DisableCameraChaseCam(gameStatus.player[actorInstance->playerNo].camera);
		SetFixedDistanceToLookAt(gameStatus.player[actorInstance->playerNo].camera, cameraInfo->defaultFixedDistance);
		DisableCameraRotateBehind(gameStatus.player[actorInstance->playerNo].camera);
		SetCameraSmooth(gameStatus.player[actorInstance->playerNo].camera, defaultPosSmooth, defaultLookatSmooth);
		actorInstance->actorStatus->vehicleInfo = NULL;          
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : CalculateGameTimeFormatted
   Purpose : calculates gametime in hours minutes and seconds
   Parameters : 
   Returns : 
   Info : 
*/

void CalculateGameTimeFormatted(float numSeconds, int *hours, int *minutes, int *seconds)
{
	int		h,m,s;

	h = m = 0;
	s = (int)numSeconds;

	while(s >= 3600)
	{
		h++;
		s -= 3600;
	}
	while(s >= 60)
	{
		m++;
		s -= 60;
	}
	*hours = h;
	*minutes = m;
	*seconds = s;
}

char processGalleryScore()
{
	char	gotoInform = 0;
	int		targetBounty, i;

	for(i = 0;i < 10;++i)
	{
		switch(i)
		{
		case 0:
			targetBounty = 10000;
			break;
		case 1:
			targetBounty = 30000;
			break;
		case 2:
			targetBounty = 60000;
			break;
		case 3:
			targetBounty = 110000;
			break;
		case 4:
			targetBounty = 185000;
			break;
		case 5:
			targetBounty = 285000;
			break;
		case 6:
			targetBounty = 535000;
			break;
		case 7:
			targetBounty = 1035000;
			break;
		case 8:
			targetBounty = 1785000;
			break;
		case 9:
			targetBounty = 2785000;
			break;
		}
		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty >= targetBounty)
		{
			if (!gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlags[i])
			{
				gotoInform = 1;
				gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlags[i] = true;
			}
		}
	}

/*	if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty >= 2785000)
	{
		if (!gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagTen)
		{
			gotoInform = 1;
			gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagTen = 1;
		}
	}

	if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty >= 1785000)
	{
		if (!gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagNine)
		{
			gotoInform = 1;
			gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagNine = 1;
		}
	}
	
	if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty >= 1035000)
	{
		if (!gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagEight)
		{
			gotoInform = 1;
			gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagEight = 1;
		}
	}
	
	if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty >= 535000)
	{
		if (!gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagSeven)
		{
			gotoInform = 1;
			gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagSeven = 1;
		}
	}
	
	if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty >= 285000)
	{
		if (!gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagSix)
		{
			gotoInform = 1;
			gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagSix = 1;
		}
	}
	
	if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty >= 185000)
	{
		if (!gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagFive)
		{
			gotoInform = 1;
			gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagFive = 1;
		}
	}
	
	if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty >= 110000)
	{
		if (!gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagFour)
		{
			gotoInform = 1;
			gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagFour = 1;
		}
	}
	
	if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty >= 60000)
	{
		if (!gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagThree)
		{
			gotoInform = 1;
			gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagThree = 1;
		}
	}
	
	if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty >= 30000)
	{
		if (!gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagTwo)
		{
			gotoInform = 1;
			gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagTwo = 1;
		}
	}
	
	if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty >= 10000)
	{
		if (!gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagOne)
		{
			gotoInform = 1;
			gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlagOne = 1;
		}
	}*/

	return gotoInform;
}