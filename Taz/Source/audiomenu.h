#ifndef __H_AUDIOMENU
#define __H_AUDIOMENU

enum EAudioSubState
{
	AU_MUSICVOL,
	AU_SFXVOL,
	AU_RETURN,
};
	
/*	--------------------------------------------------------------------------------
	Function 	: CreateAudioOptionsMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void CreateAudioOptionsMenuPage(struct TAG_FRONTENDMENU *menu);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateAudioOptionsMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateAudioOptionsMenuPage(struct TAG_FRONTENDMENU *menu, struct TAG_PADCONTROLSTATUS *controller);

#endif