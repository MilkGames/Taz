// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : tonyscene.cpp
//   Purpose : custom scene information
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "actors.h"
#include "physics.h"
#include "animation.h"
#include "collisioncache.h"
#include "mapfile.h"
#include "status.h"
#include "paths.h"
#include "genericai.h"
#include "camera.h"
#include "control.h"
#include "maths.h"
#include "characters.h"
#include "shadow.h"
#include "scene.h"
#include "trigger.h"
#include "destructibles.h"
#include "water.h"
#include "projectile.h"
#include "springyobjects.h"
#include "mailbox.h"
#include "wantedposter.h"
#include "fallingobjects.h"
#include "main.h"
#include "lights.h"
#include "cutscene.h"
#include "envmap.h"
#include "sfx.h"
#include "textures.h"
#include "swingingobjects.h"
#include "playerstats.h"
#include "tonyscene.h"
#include "claudescene.h"
#include "precipitation.h"
#include "thoughtbubble.h"
#include "collectibles.h"
#include "debris.h"
#include "tiltingobjects.h"
#include "steppingstone.h"
#include "gui.h"
#include "tonyscene.h"
#include "zookeeper.h"
#include "bears.h"
#include "cameracase.h"
#include "alligator.h"
#include "phonebox.h"
#include "securitybox.h"
#include "files.h"
#include "map.h"
#include "memcard.h"
#include "philscene.h"				// PP: 
#include "display.h"
#include "collectibles.h"
#include "music.h"

//CUTSCENE	myCutScene("museum");


/*	--------------------------------------------------------------------------------
	Function 	: InitialiseTonyGame
	Purpose 	: Initialise Tony stuff at start of game
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void InitialiseTonyGame(void)
{
	//gameStatus.enableMusic = true;
#ifdef PHILSCENE
	InitialisePhilGame();
#endif// PP: def PHILSCENE
}


/*	--------------------------------------------------------------------------------
	Function 	: InitialiseTonyScene
	Purpose 	: Initialise a scene loading actors creating instances and setting up camera
	Parameters 	: scene number, camera to setup
	Returns 	: scene type - cutscene or ingame, -1 fail
	Info 		:
*/
void InitialiseTonyScene()
{
#ifdef PHILSCENE
 	InitialisePhilScene();
#endif// PP: def PHILSCENE
}

void PrepTonyScene(int scene) 
{
}

/*	--------------------------------------------------------------------------------
	Function 	: DrawTonyScene
	Purpose 	: Draws all actors in a scene
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
void DrawTonyScene(int scene,CAMERAINFO *cameraInfo)
{
#ifdef PHILSCENE
	DrawPhilScene(scene, cameraInfo);
#endif// PP: def PHILSCENE
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateTonyScene
	Purpose 	: Update scene animations and status etc.
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
void UpdateTonyScene(int scene)
{
#ifdef PHILSCENE
	UpdatePhilScene(scene);
#endif// PP: def PHILSCENE

	// PP: TEXT ALIGNMENT TEST - but I'm sure I've got it all fixed now
/*	if(controller1.circleDebounceChannel->value)
	{
		((TEXTBOX*)(MemCard_book.getPage("NOMEDIA")->getLastItem()))->strcatf("X ");
		MemCard_book.realign();
	}*/
}

/*	--------------------------------------------------------------------------------
	Function 	: FreeTonyScene
	Purpose 	: Free all scene data, instances, actors and textures
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
void FreeTonyScene(int scene)
{
}

