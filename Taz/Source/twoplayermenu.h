#ifndef __H_TWOPLAYERMENU
#define __H_TWOPLAYERMENU

enum ETwoPlayerSubState
{
	TP_RACE,
	TP_TT,
	TP_SUB_TROLLEY,
	TP_SUB_HOVERCRAFT,
	TP_SUB_POLISHER,
	TP_SUB_JETBIKE,
	TP_DESTRUCTION,
	TP_DEST_SAFARI,
	TP_DEST_MUSEUM,
	TP_DEST_TREEHOUSE,
	TP_TOURNAMENT,
	TP_SECRET,
	TP_BOSS_ZOO,
	TP_BOSS_CITY,
	TP_BOSS_WEST,
	TP_BOSS_TASMANIA,
	TP_RETURN,
};
 
extern bool twoPlayerSubMenu;

/*	--------------------------------------------------------------------------------
	Function 	: CreateGameOptionsMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void CreateTwoPlayerMenuPage(struct TAG_FRONTENDMENU *menu);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateTwoPlayerMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateTwoPlayerMenuPage(struct TAG_FRONTENDMENU *menu, struct TAG_PADCONTROLSTATUS *controller);

/*	--------------------------------------------------------------------------------
	Function 	: GetTournamentGameName
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
EStringIndex GetTournamentGameName(int gameNumber);

/*	--------------------------------------------------------------------------------
	Function 	: GetTournamentGameIcon
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
char *GetTournamentGameIcon(int gameNumber);

#endif