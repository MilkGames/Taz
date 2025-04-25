#ifndef __H_GUI
#define __H_GUI

#include "Cheats.h"						// PP: Cheats

extern TBGuiElement	*mainMenu;
extern char		lightingOn;
extern char		heapshow;
extern bool		guiDebugDestruct;
extern bool		guiDebugFood;


/* --------------------------------------------------------------------------------
   Function : GuiCallback
   Purpose : called when gui is active and a selection has been made
   Parameters : TBGuiEventInfo *cbInfo
   Returns : 
   Info : 
*/
int GuiCallback(TBGuiEventInfo *cbInfo);

/* --------------------------------------------------------------------------------
   Function : CreateMenuBar
   Purpose : create and setup the gui menu bar
   Parameters : 
   Returns : 
   Info : 
*/
void CreateMenuBar(void);

/* --------------------------------------------------------------------------------
   Function : CreateSceneMenu
   Purpose : create and setup the scene menu bar
   Parameters : 
   Returns : 
   Info : 
*/
void CreateSceneMenu(void);

/* --------------------------------------------------------------------------------
   Function : guiChangeScene
   Purpose : change scene
   Parameters : index of scene to change to
   Returns : 
   Info : // PP: this is needed by the front-end, which has to save global game settings before exiting
*/
void guiChangeScene(const int sceneIndex);

/* --------------------------------------------------------------------------------
   Function : guiCreateEnemy
   Purpose : create and setup an enemy for testing
   Parameters : name of enemy obe
   Returns : PP: ptr to newly-created enemy ACTORINSTANCE
   Info : 
*/
struct TAG_ACTORINSTANCE* guiCreateEnemy(char *instanceName, char *bankObject,char *name,TBVector position, TBQuaternion orientation, TBVector scale, uint32 attackPointCRC = 0,int type = 0,bool box=false, bool active=true, uint32 cageCRC=0);

/* --------------------------------------------------------------------------------
   Function		: guiSetCheatTick
   Purpose		: add a tick to the cheats menu, to indicate that a particular cheat is active
   Parameters	: cheat whose tick to add (CHEAT_)
   Returns		: 
   Info			: // PP: 
*/
void guiSetCheatTick(const ECheat cheat);

/* --------------------------------------------------------------------------------
   Function		: guiClearCheatTick
   Purpose		: clear a tick from the cheats menu, to indicate that the cheat is not active
   Parameters	: cheat whose tick to clear (CHEAT_)
   Returns		: 
   Info			: // PP: 
*/
void guiClearCheatTick(const ECheat cheat);

/* --------------------------------------------------------------------------------
   Function : UpdateGuiItems
   Purpose : updates the state of any gui items before gui is entered
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateGuiItems(void);

/*  --------------------------------------------------------------------------------
	Function	: DisplayGuiInfo
	Purpose		: Print any gui information to the screen
	Parameters	: 
	Returns		: 
	Info		: 
*/
void DisplayGuiInfo(void);

/*  --------------------------------------------------------------------------------
	Function	: SetupConsole
	Purpose		: Setup the onscreen debug print messages
	Parameters	: 
	Returns		: 
	Info		: 
*/

void SetupConsole(void);

/*  --------------------------------------------------------------------------------
	Function	: DrawConsole
	Purpose		: Draw the console to the screen
	Parameters	: 
	Returns		: 
	Info		: 
*/

void DrawConsole(void);

extern char showInvisibleObjects;

/*  --------------------------------------------------------------------------------
	Function	: FinishLevel
	Purpose		: Complete the level, mark all posters complete and exit level with cutscene
	Parameters	: scene, true to play cutscene immediately
	Returns		: 
	Info		: 
*/

void FinishLevel(int scene, bool complete=true);

/*  --------------------------------------------------------------------------------
	Function	: ForceWin
	Purpose		: Complete current scene with a win scenario
	Parameters	: scene
	Returns		: 
	Info		: 
*/

void ForceWin(int scene);

/*  --------------------------------------------------------------------------------
	Function	: ForceLoss
	Purpose		: Complete the current scene with a loss scenario
	Parameters	: scene
	Returns		: 
	Info		: 
*/

void ForceLoss(int scene);


#endif