#ifndef __H_FRONTENDMAIN
#define __H_FRONTENDMAIN

#include "frontendcamera.h"
#include "frontendmenu.h"
#include "frontendanimation.h"
#include "Slider.h"						// PP: Horizontal slider page-item used for controlling values, eg. music/sound fx volume.

extern struct TAG_FRONTENDCAMERAINFO	frontEndCameraInfo;

// PP: moved from frontend.h
#define FRONTEND_SFX_VOLUME_SAMPLE	"explosion.wav"			// PP: sample used for testing sound effects
#define PAUSE_SFX_VOLUME_SAMPLE		"burp1.wav"	// NH: sample used for testing sound effects during pause 

// PP: some defaults (moved from GlobalSettings.cpp)...

#define DEFAULT_MUSIC_VOLUME			128					// PP: default music volume
#define DEFAULT_SFX_VOLUME				255					// PP: default sfx volume

#define DEFAULT_VIBRATION				true				// PP: default vibration on/off


/*	--------------------------------------------------------------------------------
	Function 	: InitialiseFrontEnd
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

int InitialiseFrontEnd(void);

/*	--------------------------------------------------------------------------------
	Function 	: StartFrontEndFunctionality
	Purpose 	: Starts the front end after the initial anim has played
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void StartFrontEndFunctionality(void);

/*	--------------------------------------------------------------------------------
	Function 	: PrepFrontEnd
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void PrepFrontEnd(void);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateFrontEndDelay
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

int UpdateFrontEndDelay(void);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateFrontEnd
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateFrontEnd(void);

/*	--------------------------------------------------------------------------------
	Function 	: DrawFrontEnd
	Purpose 	: Draws front end specific stuff
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void DrawFrontEnd(void);

/*	--------------------------------------------------------------------------------
	Function 	: DrawFrontEnd2
	Purpose 	: Draws front end specific stuff
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void DrawFrontEnd2(void);

/*	--------------------------------------------------------------------------------
	Function 	: DrawFrontEnd
	Purpose 	: Draws front end specific stuff
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void FreeFrontEnd(void);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateFrontEndInitialAnim
	Purpose 	: updates the anim which is played on entering the front end
	Parameters 	: 
	Returns 	: true if anim is stil playing
	Info 		:
*/

int UpdateFrontEndInitialAnim(void);


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
											CALLBACK FUNCTIONS for CONTROL PAGEITEMS

													moved from frontend.h
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: FrontEnd_vibrationICSetFunc
	Purpose 	: callback function for the vibration toggle-control
	Parameters 	: the state shown by the toggle-control (0/1)
	Returns 	: 
	Info 		: called only when the control toggles
*/
void FrontEnd_vibrationICSetFunc(const int state);

#endif