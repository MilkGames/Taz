#ifndef __H_ENDOFLEVELSTATS
#define __H_ENDOFLEVELSTATS

#include "scene.h"

// PP: define DEFAULT_INITIALS to supply default save-game initials ("TAZ").
// PP: This is needed on Xbox to satisy TCR 2-06: 'Saved Game Default Name'
#if(BPLATFORM == XBOX)
#define DEFAULT_INITIALS
#endif// PP: platform

#ifdef DEFAULT_INITIALS
extern const uchar					defaultInitials[];		// PP: default saved-game initials	// PP: playerstats.cpp
#endif// PP: def DEFAULT_INITIALS

#define	ENDOFLEVELSTATS_ICONSCALE	(0.75f)
#define	ENDOFLEVELSTATS_TEXTSCALE	(1.2f)

extern BOOK	enterLevelBook;


/* --------------------------------------------------------------------------------
   Function : CreateStatsAtEndOfLevel
   Purpose : creates structures used to draw end of level stats for single player game
   Parameters : 
   Returns : 
   Info : 
*/

int CreateStatsAtEndOfLevel(struct TAG_PLAYERSTATS *stats,int options = 0,int fromFrontEnd = 0);

/* --------------------------------------------------------------------------------
   Function : CreateStatsAtEndOfLevel
   Purpose : creates structures used to draw end of level stats for single player game
   Parameters : 
   Returns : 
   Info : 
*/

void CreateNameEntryPageAtEndOfLevel(struct TAG_PLAYERSTATS *stats);

void CreateInformPageAtEndOfLevel(struct TAG_PLAYERSTATS *stats);

/* --------------------------------------------------------------------------------
   Function : CreateHighScoreTablePage
   Purpose : creates a page containing a high scores table for a scene
   Parameters : 
   Returns : 
   Info : 
*/

void CreateHighScoreTablePage(struct TAG_PLAYERSTATS *stats, EScene currentScene);

/* --------------------------------------------------------------------------------
   Function : UpdateStatsAtEndOfLevel
   Purpose : updates structures used to draw end of level stats for single player game
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateStatsAtEndOfLevel(struct TAG_PLAYERSTATS *stats);

/* --------------------------------------------------------------------------------
   Function : UpdateNameEntryPage
   Purpose : updates structures used to draw end of level stats for single player game
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateNameEntryPage(struct TAG_PLAYERSTATS *stats);

/* --------------------------------------------------------------------------------
   Function : DrawStatsAtEndOfLevel
   Purpose : draws end of level stats for single player game
   Parameters : 
   Returns : 
   Info : 
*/

// PP: REMOUT: Now handled by the Book List			void DrawStatsAtEndOfLevel(struct TAG_PLAYERSTATS *stats);

/* --------------------------------------------------------------------------------
   Function : DrawStatsAtEndOfLevel
   Purpose : draws end of level stats for single player game
   Parameters : 
   Returns : 
   Info : 
*/

char IncrementDecrementScoreCharacter(char currentAscii, uint32 flag);

/*	--------------------------------------------------------------------------------
	Function 	: FreePerFreeStatsDisplaysmanentResources
	Purpose 	: Free all stats books
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void FreeStatsDisplays(void);

/*  --------------------------------------------------------------------------------
	Function	: CreateEnterLevelStats
	Purpose		: setup book for enter level stats
	Parameters	: level
	Returns		: 
	Info		: 
*/
void CreateEnterLevelStats(int level);


#endif