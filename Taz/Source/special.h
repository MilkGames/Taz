#ifndef __H_SPECIAL
#define __H_SPECIAL

#include "actors.h"

/*	--------------------------------------------------------------------------------
	Function 	: InitialiseSpecial
	Purpose 	: Initialise the special items for a particular scene
	Parameters 	: scene number
	Returns 	: 
	Info 		: 
*/
void InitialiseSpecial(int scene);

/*	--------------------------------------------------------------------------------
	Function 	: ShutdownSpecial
	Purpose 	: Shutdown any special items on the level
	Parameters 	: scene number
	Returns 	: 
	Info 		: 
*/
void ShutdownSpecial(int scene);

/*	--------------------------------------------------------------------------------
	Function 	: PrepSpecial
	Purpose 	: Prepare any special items on the level
	Parameters 	: scene number
	Returns 	: 
	Info 		: 
*/
void PrepSpecial(int scene);

/*	--------------------------------------------------------------------------------
	Function 	: DrawSpecial
	Purpose 	: Draw any special items
	Parameters 	: scene number
	Returns 	: 
	Info 		: // PP: PLEASE NOTE: THIS GETS CALLED FOR SCREENS 1&2, THEN FOR SCREEN -1 (TO DRAW OVER BOTH SCREENS).
							BEFORE DRAWING, CHECK gameStatus.multiplayer.currentScreen
*/
void DrawSpecial(int scene, struct TAG_CAMERAINFO *cameraInfo);

/*	--------------------------------------------------------------------------------
	Function 	: DrawSpecial
	Purpose 	: Draw any special items
	Parameters 	: scene number
	Returns 	: 
	Info 		: 
*/
void DrawSpecial2(int scene, struct TAG_CAMERAINFO *cameraInfo, int drawBeforeExtras);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateSpecial
	Purpose 	: Update any special items on the level
	Parameters 	: scene number
	Returns 	: 
	Info 		: 
*/
void UpdateSpecial(int scene);

/*	--------------------------------------------------------------------------------
	Function 	: RocketProject
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void RocketProject(void);

void DrawStateChangeDebugInfo(ACTORINSTANCE *actorInstance);

/*  --------------------------------------------------------------------------------
	Function	: StartFrankenMachine
	Purpose		: Start the franken machine cutscene with SPECIAL! taz anim
	Parameters	: name of machine
	Returns		: 
	Info		: 
*/

void StartFrankenMachine(char *machine);

/*  --------------------------------------------------------------------------------
	Function	: UpdateFrankenMachine
	Purpose		: Hold taz in the correct position and wait for the animation to finish
	Parameters	: 
	Returns		: 
	Info		: 
*/

void UpdateFrankenMachine();

/*  --------------------------------------------------------------------------------
	Function	: EndFrankenMachine
	Purpose		: Resets taz and re-enables control
	Parameters	: 
	Returns		: 
	Info		: 
*/

void EndFrankenMachine();

extern bool frankenMachineActive;


#if BPLATFORM == PC

/*	--------------------------------------------------------------------------------
	Function 	: CStrikeSearchAndReplace
	Purpose 	: searces a cstrike file for a string and replaces the argument
	Parameters 	: scene number
	Returns 	: 
	Info 		: 
*/

int CStrikeSearchAndReplace(char *filename, char *searchString, char *replaceWith);

#endif

#endif