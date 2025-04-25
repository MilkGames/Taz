// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : highscores.cpp
//   Purpose : functions to calculate high score table
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "scene.h"
#include "playerstats.h"
#include "highscores.h"

//array of icon texture names

char *highScoreIconNames[NUM_HSE_ICONS] = 
{
	{"tazshout.bmp"},
	{"tazscared.bmp"},
	{"taztongue.bmp"},
	{"tazgrowl.bmp"},
	{"tazfast.bmp"},
	{"marvin.bmp"},
	{"porky.bmp"},
	{"sylvest.bmp"},
	{"daffymad.bmp"},
	{"daffymean.bmp"},
	{"gossamer.bmp"},
	{"pepelepew.bmp"},
	{"tweety.bmp"},
	{"speedy.bmp"},
	{"granny.bmp"},
};

//HIGHSCORES		highScores[SCORES_TOTALNUM];

/*	--------------------------------------------------------------------------------
	Function 	: InitialiseHighScores
	Purpose 	: Temp function to initialise high score table in short term
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void InitialiseHighScores(void)
{
	int		i,j;

	for(j=0;j<(int)SCORES_TOTALNUM;j++)
	{
		for(i=0;i<5;i++)
		{
			gameStatus.globalSettings.saveInfo.highScores[j].iconRef[i] = 0;
			gameStatus.globalSettings.saveInfo.highScores[j].rank[i] = STR_PLAYER_RANKING_0;
			gameStatus.globalSettings.saveInfo.highScores[j].time[i] = 0.0f;
			gameStatus.globalSettings.saveInfo.highScores[j].names[i][0] = 'T';
			gameStatus.globalSettings.saveInfo.highScores[j].names[i][1] = 'A';
			gameStatus.globalSettings.saveInfo.highScores[j].names[i][2] = 'Z';
			gameStatus.globalSettings.saveInfo.highScores[j].names[i][3] = 0;
			gameStatus.globalSettings.saveInfo.highScores[j].exactRanking[i] = 0.0f;
			gameStatus.globalSettings.saveInfo.highScores[j].money[i] = 0;
			gameStatus.globalSettings.saveInfo.highScores[j].destructBonusCollected[i] = false;
			gameStatus.globalSettings.saveInfo.highScores[j].secretItemCollected[i] = false;
		}
		gameStatus.globalSettings.saveInfo.stats[j].totalPosters = 0;
		gameStatus.globalSettings.saveInfo.stats[j].postersTriggered = 0;
		gameStatus.globalSettings.saveInfo.stats[j].destructibles = 0.0f;
		gameStatus.globalSettings.saveInfo.stats[j].collectibles = 0;
		gameStatus.globalSettings.saveInfo.stats[j].money = 0;
		gameStatus.globalSettings.saveInfo.stats[j].time = 0.0f;
		gameStatus.globalSettings.saveInfo.stats[j].levelCompleted = FALSE;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: CalculateRankAtEndOfLevel
	Purpose 	: calculates rank of player based on level stats
	Parameters 	: playerstats structure
	Returns 	: index into string table
	Info 		:
*/

float CalculateRankAtEndOfLevel(PLAYERSTATS *playerStats)
{
	float		rankCoeff;

	rankCoeff = 5.0f*bmSqrt(((float)playerStats->numObjectsDestroyed/(float)map.mapStats.numDestructibles)*
		((float)gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].postersCollected/(float)map.mapStats.numPosters));

	//add for time
	if(playerStats->time/60 < 15)
	{
		rankCoeff += 1.5f;
	}
	else
	if(playerStats->time/60 < 25)
	{
		rankCoeff += 0.5f;
	}
	else
	if(playerStats->time/60 > 35)
	{
		rankCoeff -= 0.5f;
	}
	else
	if(playerStats->time/60 > 45)
	{
		rankCoeff -= 1.5;
	}

	return rankCoeff;
}

/*	--------------------------------------------------------------------------------
	Function 	: CalculateRankAtEndOfLevel
	Purpose 	: calculates rank of player based on level stats
	Parameters 	: playerstats structure
	Returns 	: index into string table
	Info 		:
*/

EStringIndex CalculateRankFromFloat(float exactRank)
{
	//decide on rank
	if(exactRank > 4.0f)
	{
		return STR_PLAYER_RANKING_4;
	}
	else
	if(exactRank > 3.0f)
	{
		return STR_PLAYER_RANKING_3;
	}
	else
	if(exactRank > 2.0f)
	{
		return STR_PLAYER_RANKING_2;
	}
	else
	if(exactRank > 1.0f)
	{
		return STR_PLAYER_RANKING_1;
	}
	else
	{
		return STR_PLAYER_RANKING_0;
	}
}

/* --------------------------------------------------------------------------------
   Function : ConvertSceneRefToLevelScoreRef
   Purpose : draws end of level stats for single player game
   Parameters : 
   Returns : 
   Info : 
*/

ELevelScores ConvertSceneRefToLevelScoreRef(EScene currentScene)
{
	switch(currentScene)
	{
	case SCENE_ICEDOME:
		return SCORES_ICEDOME;
		break;
	case SCENE_SAFARI:
		return SCORES_SAFARI;
		break;
	case SCENE_AQUA:
		return SCORES_AQUA;
		break;
	case SCENE_ZOOBOSS:
		return SCORES_ZOOBOSS;
		break;
	case SCENE_DEPTSTR:
		return SCORES_DEPTSTR;
		break;
	case SCENE_MUSEUM:
		return SCORES_MUSEUM;
		break;
	case SCENE_CONSTRUCT:
		return SCORES_CONSTRUCT;
		break;
	case SCENE_CITYBOSS:
		return SCORES_CITYBOSS;
		break;
	case SCENE_GHOSTTOWN:
		return SCORES_GHOSTTOWN;
		break;
	case SCENE_GOLDMINE:
		return SCORES_GOLDMINE;
		break;
	case SCENE_GRANDCANYON:
		return SCORES_GRANDCANYON;
		break;
	case SCENE_WESTBOSS:
		return SCORES_WESTBOSS;
		break;
	case SCENE_TAZHUB:
		return SCORES_TAZHUB;
		break;
	case SCENE_TAZPREBOSS:
		return SCORES_TAZPREBOSS;
		break;
	case SCENE_TAZBOSS:
		return SCORES_TAZBOSS;
		break;
	default:
		return SCORES_INVALIDSCENE;
		break;
	}
}

/* --------------------------------------------------------------------------------
   Function : CheckRankAgainstCurrentTopTen
   Purpose : draws end of level stats for single player game
   Parameters : 
   Returns : 
   Info : 
*/

int CheckRankAgainstCurrentTopTen(PLAYERSTATS *stats, EScene currentScene)
{
	//if(CalculateRankAtEndOfLevel(stats) > gameStatus.globalSettings.saveInfo.highScores[ConvertSceneRefToLevelScoreRef(currentScene)].exactRanking[5])
	if(stats->bountyDisplay > gameStatus.globalSettings.saveInfo.highScores[ConvertSceneRefToLevelScoreRef(currentScene)].money[4])
	{
		return TRUE;
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : IncrementDecrementScoreCharacter
   Purpose : draws end of level stats for single player game
   Parameters : 
   Returns : 
   Info : 
*/

void AddNewRecordToStats(PLAYERSTATS *stats, EScene	currentScene)
{
	int				i,position;
	float			exactRank;
	ELevelScores	currentEntry;
	HIGHSCORES		tempTable;

	exactRank = CalculateRankAtEndOfLevel(stats);
	currentEntry = ConvertSceneRefToLevelScoreRef(currentScene);

	//find correct position in table
	i = 0;
	position = 0;
	for(i=0;i<5;i++)
	{
		//if(gameStatus.globalSettings.saveInfo.highScores[currentEntry].exactRanking[i] > exactRank) position++;
		if (gameStatus.globalSettings.saveInfo.highScores[currentEntry].money[i] > stats->bountyDisplay) 
			position++;
		else if (gameStatus.globalSettings.saveInfo.highScores[currentEntry].money[i] == stats->bountyDisplay)
		{
			if (stats->time > gameStatus.globalSettings.saveInfo.highScores[currentEntry].time[i])
				position ++;
		}
		
		//copy table into temp structure
		tempTable.exactRanking[i] = gameStatus.globalSettings.saveInfo.highScores[currentEntry].exactRanking[i];
		tempTable.iconRef[i] = gameStatus.globalSettings.saveInfo.highScores[currentEntry].iconRef[i];
		tempTable.rank[i] = gameStatus.globalSettings.saveInfo.highScores[currentEntry].rank[i];
		tempTable.time[i] = gameStatus.globalSettings.saveInfo.highScores[currentEntry].time[i];
		tempTable.names[i][0] = gameStatus.globalSettings.saveInfo.highScores[currentEntry].names[i][0];
		tempTable.names[i][1] = gameStatus.globalSettings.saveInfo.highScores[currentEntry].names[i][1];
		tempTable.names[i][2] = gameStatus.globalSettings.saveInfo.highScores[currentEntry].names[i][2];
		tempTable.money[i] = gameStatus.globalSettings.saveInfo.highScores[currentEntry].money[i];
	}

	if (position==5) return;

	//fill in new entry
	gameStatus.globalSettings.saveInfo.highScores[currentEntry].exactRanking[position] = exactRank;
	gameStatus.globalSettings.saveInfo.highScores[currentEntry].iconRef[position] = stats->iconNumber;
	gameStatus.globalSettings.saveInfo.highScores[currentEntry].rank[position] = CalculateRankFromFloat(exactRank);
	gameStatus.globalSettings.saveInfo.highScores[currentEntry].time[position] = stats->time;
	gameStatus.globalSettings.saveInfo.highScores[currentEntry].names[position][0] = stats->letters[0];
	gameStatus.globalSettings.saveInfo.highScores[currentEntry].names[position][1] = stats->letters[1];
	gameStatus.globalSettings.saveInfo.highScores[currentEntry].names[position][2] = stats->letters[2];
	gameStatus.globalSettings.saveInfo.highScores[currentEntry].money[position] = stats->bountyDisplay;
	gameStatus.globalSettings.saveInfo.highScores[currentEntry].destructBonusCollected[position] = gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].destructBonusCollected;
	gameStatus.globalSettings.saveInfo.highScores[currentEntry].secretItemCollected[position] = gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].secretItemCollected;

	position++;
	while(position<5)
	{
		//rearrange table
		gameStatus.globalSettings.saveInfo.highScores[currentEntry].exactRanking[position] = tempTable.exactRanking[position-1];
		gameStatus.globalSettings.saveInfo.highScores[currentEntry].iconRef[position] = tempTable.iconRef[position-1];
		gameStatus.globalSettings.saveInfo.highScores[currentEntry].rank[position] = tempTable.rank[position-1];
		gameStatus.globalSettings.saveInfo.highScores[currentEntry].time[position] = tempTable.time[position-1];
		gameStatus.globalSettings.saveInfo.highScores[currentEntry].names[position][0] = tempTable.names[position-1][0];
		gameStatus.globalSettings.saveInfo.highScores[currentEntry].names[position][1] = tempTable.names[position-1][1];
		gameStatus.globalSettings.saveInfo.highScores[currentEntry].names[position][2] = tempTable.names[position-1][2];
		gameStatus.globalSettings.saveInfo.highScores[currentEntry].money[position] = tempTable.money[position-1];
		position++;
	}
}