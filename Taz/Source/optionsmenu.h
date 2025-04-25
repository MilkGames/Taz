#ifndef __H_OPTIONSMENU
#define __H_OPTIONSMENU

enum EOptionsSubState
{
	GO_NONE,
	GO_DIFFICULTY,
	GO_VIBRATION,
	GO_SCREENPOS,
	GO_BACKTO_SCREENPOS,
	GO_SUBTITLES,
	GO_CONFIRM,
	GO_WAITFORRESET,

	GO_SUB_MOVESCREEN,
};

enum EOptionsPopups
{
	OPTIONS_POPUP_DIFFICULTY,
	OPTIONS_POPUP_VIBRATION,
	OPTIONS_POPUP_SUBTITLES,
	OPTIONS_POPUP_WIDESCREEN,				// PP: Popup for turning widescreen on & off
};

/*	--------------------------------------------------------------------------------
	Function 	: CreateGameOptionsMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void CreateGameOptionsMenuPage(struct TAG_FRONTENDMENU *menu);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateGameOptionsMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateGameOptionsMenuPage(struct TAG_FRONTENDMENU *menu, struct TAG_PADCONTROLSTATUS *controller);

/*	--------------------------------------------------------------------------------
	Function 	: ChangeScreenOffset
	Purpose 	: Changes Screen offset
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void ChangeScreenOffset(int x, int y);

/*	--------------------------------------------------------------------------------
	Function 	: FrontEndRumbleCallback
	Purpose 	: callback to rumble pad at correct time
	Parameters 	: 
	Returns 	: anim segment ptr to chosen anim
	Info 		:
*/

void FrontEndRumbleCallback(EBQueueCallbackReason reasonCode,TBActorInstance *actor,TBActorNodeInstance *node,
							  TBActorAnimSegment *segment,void *context);

#endif