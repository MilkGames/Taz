#ifndef __H_STARTGAMEMENU
#define __H_STARTGAMEMENU

enum EStartMenuSubState
{
	SM_SLOTS=-1,
	SM_SLOTONE=0,
	SM_SLOTTWO=1,
	SM_SLOTTHREE=2,	
	SM_NEW,
	SM_LOAD,
	SM_DELETE,
	SM_DELETE_FINAL,
	SM_CONFIRM,
	SM_GETNAME,
	SM_LOADMEMCARD,
	SM_INIT,
	SM_LEAVING,
};

enum EStartGamePopups
{
	START_POPUP_CONTINUE_OR_DELETE,
	START_POPUP_DELETE_CONFIRM,
	START_POPUP_NEW_GAME,
};

/*	--------------------------------------------------------------------------------
	Function 	: CreateStartGameMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void CreateStartGameMenuPage(struct TAG_FRONTENDMENU *menu);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateStartGameMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateStartGameMenuPage(struct TAG_FRONTENDMENU *menu, struct TAG_PADCONTROLSTATUS *controller);

/*	--------------------------------------------------------------------------------
	Function 	: ChooseHubToStartIn
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void ChooseHubToStartIn(uint32 slotNum);

/*	--------------------------------------------------------------------------------
	Function 	: ChooseHubToStartIn
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void PrintTimeAndDateIntoTextbox(struct TAG_TEXTBOX *textbox, TBClock	*clock);

/*	--------------------------------------------------------------------------------
	Function 	: calculatePercentageComplete
	Purpose 	: Go on, have a guess!
	Parameters 	: int (slot number to calculate % for)
	Returns 	: int (% complete)
	Info 		:
*/

int calculatePercentageComplete(int slotNumber);

#endif