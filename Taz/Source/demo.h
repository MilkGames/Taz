// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Blitz Games Ltd.
//
//      File : demo.cpp
//   Purpose : consumer demo specifc stuff
// Component : Taz
//    Author : John Weeks
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// PP: This file isn't used unless it's a consumer demo build
// PP: REMOUT: TEMP TEST	#ifdef CONSUMERDEMO

#ifndef __DEMO_H
#define __DEMO_H

#include "main.h"
#include "global.h"

#include "control.h"

// PP: Macro to determine if the consumer demo is in interactive mode (ie. not in purely-attract mode)  Does that make sense?
#if(BPLATFORM == PS2)
#define CONSUMER_DEMO_PLAYABLE			(playmode == SCEE_DEMO_PLAYMODE_PLAYABLE)
#elif(BPLATFORM == XBOX)
										// PP: I now convert demoInfo->dwRunmode into 'playmode', because I need to be able to change the playability status of the demo from kiosk to normal without damaging my launch info
#define CONSUMER_DEMO_PLAYABLE			(playmode == SCEE_DEMO_PLAYMODE_PLAYABLE)// PP: (demoInfo->dwRunmode != KIOSK_MODE)
#elif(BPLATFORM == PC)
#ifdef CONSUMERDEMO
#define CONSUMER_DEMO_PLAYABLE			TRUE
#endif
#endif// PP: platform

// PP: evaluates true if the rolling demo is actively playing right now
#define ROLLING_DEMO_ACTIVELY_PLAYING	((gameStatus.demoMode > 0) && (gameStatus.currentState == GAMESTATE_INGAME))

// PP: define this to record a demo for the consumer demo
// JW:
//#define CONSUMERDEMO_RECORD_DEMO// PP: TEMP TEST

// PP: time since last pad input
// PP: We can't use controller1.idleTime.
extern float	demoIdleTime;
extern bool		attractInterrupt;

static bool attractInterruptButton(PADCONTROLSTATUS &controller);
bool AttractInterruptButton(void);

void DoDemoMenu();
char DoDemoQuit();

void CreateDemoMenu();
void UpdateDemoMenu();
void DrawDemoMenu();
void KillDemoMenu();

void CreateDemoQuit();
char UpdateDemoQuit();
void DrawDemoQuit();
void KillDemoQuit();

void StartDemoAttract();
void DemoForceQuit();

void DemoForceQuitFadeDownCallback(class TAG_FADE *fade);
void StartDemoMenuFadeDownCallback(class TAG_FADE *fade);
void StartDemoAttractFadeDownCallback(class TAG_FADE *fade);
void StartDemoGameFadeDownCallback(class TAG_FADE *fade);
void StopDemoGameFadeDownCallback(class TAG_FADE *fade);
void DemoQuitFadeDownCallback(class TAG_FADE *fade);

/*  --------------------------------------------------------------------------------
	Function	: RecordingDemo
	Purpose		: Handles pad input recording when producing a rolling demo
	Parameters	: 
	Returns		: 
	Info		: 
*/

void RecordingDemo(void);

/*  --------------------------------------------------------------------------------
	Function	: CheckForDemoInterrupt
	Purpose		: check for, and respond to, interruption of the rolling demo
	Parameters	: 
	Returns		: 
	Info		: // PP: this needs to be called after ProcessInput
*/

void CheckForDemoInterrupt(void);

/*  --------------------------------------------------------------------------------
	Function	: PlayingDemo
	Purpose		: Read key inputs from file to playback rolling demo
	Parameters	: 
	Returns		: 
	Info		: 
*/

void PlayingDemo(void);

/*  --------------------------------------------------------------------------------
	Function	: DrawLegalSplashScreen
	Purpose		: 
	Parameters	: 
	Returns		: 
	Info		: 
*/

void DrawLegalSplashScreen();



/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												XBOX-SPECIFIC INTERACTIVE DEMO STUFF
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#if(BPLATFORM == XBOX)


typedef struct 
{
	DWORD	dwID;
	DWORD	dwRunmode;
	DWORD	dwTimeout;
	char	szLauncherXBE[64];
	char	szLaunchedXBE[64];
	char	*szDemoInfo;				// PP: IMPORTANT!!!!: DO NOT TRY TO USE THIS IN ANY WAY - THAT INCLUDES STRNCPYING IT!!!!
}DEMO_LAUNCH_INFO;

#define KIOSK_MODE						1
#define NON_KIOSK_MODE					0// PP: ?

extern DEMO_LAUNCH_INFO					*demoInfo;


/*	--------------------------------------------------------------------------------
	Function 	: CheckLaunchInfo
	Purpose 	: Check the launch info passed into the Xbox interactive demo
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void CheckLaunchInfo(void);


/*	--------------------------------------------------------------------------------
	Function 	: PrepForLaunch
	Purpose 	: Make preparations for returning to the application that launched the Xbox interactive demo
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PrepForLaunch(void);


/*	--------------------------------------------------------------------------------
	Function 	: QuitBackToXDemos
	Purpose 	: Quit from the Xbox interactive demo back to whatever launched the demo
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void QuitBackToXDemos(void);


#endif// PP: (BPLATFORM == XBOX)

#endif// PP: ndef __DEMO_H

// PP: REMOUT: TEMP TEST	#endif// PP: def CONSUMERDEMO	// PP: This file isn't used unless it's a consumer demo build