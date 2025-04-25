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
#include "watersplash.h"
#include "snowball.h"
#include "PhilScene.h"				// PP: Phil-specific stuff

// PP: define PHILSCENE to call Phil-specific stuff from ClaudeScene
//#define PHILSCENE

/*	--------------------------------------------------------------------------------
	Function 	: InitialiseClaudeScene
	Purpose 	: Initialise a scene loading actors creating instances and setting up camera
	Parameters 	: scene number, camera to setup
	Returns 	: scene type - cutscene or ingame, -1 fail
	Info 		:
*/
void InitialiseClaudeScene(int scene, CAMERAINFO *cameraInfo,CAMERAINFO *cameraInfo2)
{
	//TBVector			tempPos = {METERTOUNIT(-5.0f), METERTOUNIT(-1.0f), METERTOUNIT(1.0), 1.0f};				trailingColour[4] = {127, 0, 0, 127};
	//ACTORINSTANCE		*tempInstance;

	//LoadActor("sphere04.obe", "resobjs");

	//set up swinging chicken drumstick
	//tempInstance = CreateActorInstance(&map.levelInstances, "sphere04.obe", "swing");
	//CreateSwingingObject(tempInstance, tempPos, METERTOUNIT(3.0f));
}

/*	--------------------------------------------------------------------------------
	Function 	: PrepClaudeScene
	Purpose 	: Prepares all the actors in a scene
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
void PrepClaudeScene(int scene)
{
	/*if (!controller1.l2Channel->value)
	{
		ProcessCollisionForActor(&collisionCache, gameStatus.player1);
		if (gameStatus.player2)
		{
			ProcessCollisionForActor(&collisionCache, gameStatus.player2);
		}
	}
	else
	{
		bmVectorAdd(gameStatus.player1->actorInstance.position,gameStatus.player1->actorInstance.position,gameStatus.player1->actorBehaviour->physics.velocity);
	}

	// PrepInstancesPathInfo should be called early as it may move objects used by shadows etc.
	PrepInstancesPathInfo(&levelInstances);

	PrepActorInstances(&characterInstances);
	PrepInstancesCharacterInfo(&characterInstances);
	PrepActorInstances(&animatedInstances);
	CreateActorShadows(&characterInstances, &collisionCache);
	CreateActorShadows(&levelInstances, &collisionCache);

	PrepActorInstances(&levelInstances);
	PrepActorInstances(&rockInstances);
	// prepare the environment map
	PrepareEnvMap(&environmentMap,cameraInfo.pos);*/
}

/*	--------------------------------------------------------------------------------
	Function 	: DrawClaudeScene
	Purpose 	: Draws all actors in a scene
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
void DrawClaudeScene(int scene,CAMERAINFO *cameraInfo)
{
#ifdef PHILSCENE
	DrawPhilScene(scene, cameraInfo);
	return;
#endif// PP: def PHILSCENE

	//char					buf[256];
//	TBFont					*standardFont;
	//ushort					string16[256];
	TBSet					*set;

	// render environment must go first
	PrepareEnvMap(&environmentMap,cameraInfo->pos);
	RenderEnvMap(&environmentMap);

	bdSetIdentityObjectMatrix();
	set = (TBSet*)FindActorInActorList("testarena.obe");
	if (set) baDrawSet(set);

	DrawActorInstances(&characterInstances);
	
	bdSetRenderState(BDRENDERSTATE_TEXTUREFILTER, gameStatus.gfxSettings.filterMode, FALSE);
	DrawActorInstances(&levelInstances);
	DrawActorInstances(&rockInstances);
	DrawActorInstances(&animatedInstances);
	DrawActorShadows(&characterInstances);
	DrawActorShadows(&levelInstances);
	DrawInstancesCharacterInfo(&characterInstances);
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateClaudeScene
	Purpose 	: Update scene animations and status etc.
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
void UpdateClaudeScene(int scene)
{
#ifdef PHILSCENE
	UpdatePhilScene(scene);
	return;
#endif// PP: def PHILSCENE

	/*UpdatePathInfoInstances(&levelInstances,1.0f/fps*60.0f);
	AnimateActorInstances(&characterInstances);
	UpdateActorStatusInstances(&characterInstances);
	AnimateActorInstances(&animatedInstances);
	UpdateCollisionCache(&collisionCache);*/
}

/*	--------------------------------------------------------------------------------
	Function 	: FreeClaudeScene
	Purpose 	: Free all scene data, instances, actors and textures
	Parameters 	: scene number
	Returns 	: 
	Info 		:
*/
void FreeClaudeScene(int scene)
{
/*	FreeCollisionCache(&collisionCache);
	baDeleteExtraSystem(NULL);
	InitCollectibleSystem();
	InitDebrisSystem();
	DestroyCartoonLight();
	FreeAllActorInstances(&characterInstances);
	FreeAllActorInstances(&levelInstances);
	FreeAllActorInstances(&animatedInstances);
	FreeAllActorInstances(&rockInstances);
	FreeAllActorInstances(&tempInstances);
	FreeAllActors();
	FreeTextures("chars");
	FreeTextures("levels");
	FreeTextures("resobjs");*/
}

