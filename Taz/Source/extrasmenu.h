#ifndef __H_EXTRASMENU
#define __H_EXTRASMENU

enum EExtrasSubStates
{
	EX_CREDITS,
	EX_GALLERY,
	EX_SECRETS,
	EX_RETURN,
};

/*	--------------------------------------------------------------------------------
	Function 	: CreateExtrasMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void CreateExtrasMenuPage(struct TAG_FRONTENDMENU *menu);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateGameOptionsMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateExtrasMenuPage(struct TAG_FRONTENDMENU *menu, struct TAG_PADCONTROLSTATUS *controller);

#endif