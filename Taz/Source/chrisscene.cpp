// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : chrisscene.cpp
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
#include "chrisscene.h"
#include "zookeeper.h"
#include "bears.h"
#include "cameracase.h"
#include "alligator.h"
#include "trail.h"

// CPW: ok, I admit it.
#define GOTH			TRUE

/*
ACTORINSTANCE			*jeepInstance;

ACTORINSTANCELIST		inactiveCharacterInstances;

// test jeep code
TBVector				start = {-1894,-1000,2191};
TBVector				mid = {-1894,-500,2191};
TBVector				end = {-1894,-500,-2191};
TBQuaternion			startq = {0,1,0,RAD(0)};
TBQuaternion			endq = {0,1,0,RAD(180)};
TBVector				starts = {1,1,1};
TBVector				ends = {1,1,1};
PATH					testPath;
*/

/*	--------------------------------------------------------------------------------
	Function 	: InitialiseChrisScene
	Purpose 	: Initialise a scene loading actors creating instances and setting up camera
	Parameters 	: scene number, camera to setup
	Returns 	: scene type - cutscene or ingame, -1 fail
	Info 		:
*/
void InitialiseChrisScene(int scene, CAMERAINFO *cameraInfo, CAMERAINFO *cameraInfo2)
{
	// CPW: EnableCameraRotateBehind(cameraInfo, RAD(45));
//	trailSetUp(gameStatus.player1);
// CPW: 	EnableCameraFollowBehind(cameraInfo, gameStatus.player1);
}

void PrepChrisScene(int scene)
{
	;
}

/*	--------------------------------------------------------------------------------
	Function 	: DrawChrisScene
	Purpose 	: Draws all actors in a scene
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
void DrawChrisScene(int scene,CAMERAINFO *cameraInfo)
{
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateChrisScene
	Purpose 	: Update scene animations and status etc.
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
void UpdateChrisScene(int scene)
{
}

/*	--------------------------------------------------------------------------------
	Function 	: FreeChrisScene
	Purpose 	: Free all scene data, instances, actors and textures
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
void FreeChrisScene(int scene)
{

}







