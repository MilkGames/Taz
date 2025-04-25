#ifndef __H_RECORDSMENU
#define __H_RECORDSMENU

#include "textfx.h"
#include "highscores.h"

enum ERecordsSubState
{
	R_ZOO,
	R_SUB_SAFARI,
	R_SUB_ICEKINGDOME,
	R_SUB_AQUA,

	R_SAMFRAN,
	R_SUB_MUSEUM,
	R_SUB_DEPTSTR,
	R_SUB_CONSTRUCT,

	R_WEST,
	R_SUB_CANYON,
	R_SUB_GHOST,
	R_SUB_GOLDMINE,

	R_TASMANIA,
	R_SUB_TASMANIA1,
	R_SUB_TASMANIA2,
	R_SUB_TASMANIA3,

	R_OVERALL,
	R_SUB_OVERALL1,
	R_SUB_OVERALL2,
	R_SUB_OVERALL3,
};

typedef struct TAG_PLAYER_RECORDS
{
	char			name[4];
	char			iconRef;
	float			money;
	uint32			secretsCollected;		// NH: No. secret items been collected
	uint32			destructsCollected;		// NH: No. destruct bonuses been won	
	uint32			pickupsCollected;		// NH: No. pickup items been collected
	uint32			postersDestroyed;		// NH: No. posters destroyed
}PLAYER_RECORDS, _PLAYER_RECORDS;

//extern PLAYER_RECORDS playerRecords[4];

/*	--------------------------------------------------------------------------------
	Function 	: AddLevelRecordsPage
	Purpose 	: Adds a records page for a specified level
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void AddLevelRecordsPage(struct TAG_PAGE *levelPage, EScene sceneNumber, int hubNumber, int levelNumber);

/*	--------------------------------------------------------------------------------
	Function 	: CreateGameOptionsMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void CreateRecordsMenuPage(struct TAG_FRONTENDMENU *menu);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateRecordsMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateRecordsMenuPage(struct TAG_FRONTENDMENU *menu, struct TAG_PADCONTROLSTATUS *controller);

/*	--------------------------------------------------------------------------------
	Function 	: checkDestructionAndSecretCollectionState
	Purpose 	: Checks to see if either the Destruction bonus has been got or the secret collect
					if so, then the icon is changed to represent this.
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void checkDestructionAndSecretCollectionState(ELevelScores scoreLevelName, int hubNo, int levelNo);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateRecordsMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void SetFrontEndRecordsText(struct TAG_RECORDSPAGE *ptr, uint32 level);

/*	--------------------------------------------------------------------------------
	Function 	: compileOverallForLevel
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void compileOverallForLevel(EScene levelToCheck);

/*	--------------------------------------------------------------------------------
	Function 	: compileOverallRecord
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

int compileOverallRecord();

/*	--------------------------------------------------------------------------------
	Function 	: getHighestScorer
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
uint getHighestScorer(EScene levelToCheck);

void getHiscores(ELevelScores levelToCheck);

#endif