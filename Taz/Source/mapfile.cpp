// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : mapfile.cpp
//   Purpose : functions to load and create levels from map files
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//include "taz.h"

#include "global.h"
#include "actors.h"
#include "physics.h"
#include "animation.h"
#include "collisioncache.h"
#include "destructibles.h"
#include "sparks.h"
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
#include "water.h"
#include "projectile.h"
#include "springyobjects.h"
#include "mailbox.h"
#include "wantedposter.h"
#include "fallingobjects.h"
#include "main.h"
#include "lights.h"
#include "steppingstone.h"
#include "tiltingobjects.h"
#include "swingingobjects.h"
#include "electrocution.h"
#include "collectibles.h"
#include "files.h"
#include "effects.h"
#include "cameracase.h"
#include "playerstats.h"
#include "clouds.h"
#include "debris.h"
#include "precipitation.h"
#include "zookeeper.h"
#include "fire.h"
#include "transporter.h"
#include "ice.h"
#include "speedboost.h"
#include "timer.h"
#include "slides.h"
#include "gui.h"
#include "foodrandomiser.h"
#include "escalator.h"
#include "vehicles.h"
#include "doors.h"
#include "scenicobjects.h"
#include "phonebox.h"
#include "waterfall.h"
#include "laser.h"
#include "lightning.h"
#include "fountain.h"
#include "zzz.h"
#include "polyeffects.h"
#include "shatter.h"
#include "envmap.h"
#include "voodoocloud.h"
#include "multiplayer.h"
#include "phoneboxeffect.h"
#include "bumper.h"
#include "water.h"
#include "MonitorFX.h"							// PP: computer monitor effects
#include "Cheats.h"								// PP: Cheats
#include "flocking.h"
#include "buzzard.h"
#include "mapfile.h"
#include "securitybox.h"
#include "textures.h"
#include "cutscene.h"
#include "frontendmain.h"
#include "map.h"
#include "lava.h"
#include "VideoFX.h"							// PP: Video effects - motion blur, defocus, whatever.  Top-of-the-Pops-TASTIC, Mate!
#include "splatobjects.h"
#include "death.h"
#include "haar.h"
#include "musicandsavagebeasts.h"
#include "minecart.h"
#include "portal.h"
#include "famethrowa.h"
#include "mechatweety.h"
#include "smashcamera.h"
#include "display.h"
#include "util.h"								// PP: Utility functions
#include "rocketscience.h"
#include "tarbubble.h"
#include "nastycan.h"
#include "halo.h"
#include "zapper.h"
#include "aircon.h"
#include "tumbleweed.h"
#include "philscene.h"							// PP: TEMP: for timer macros
#include "arrow.h"
#include "transportereffect.h"
#include "footalign.h"
#include "waterrings.h"
#include "mirrorball.h"
static char		currentObjectName[256];
static char		currentObject[256];
static char		lomName[256];
static char		pakName[256];
static char		pathName[256];
static MAPINFO		*currentMap;
static int			numPolysInObject;
static CPMNode		*rootNode;
static int			instanceCounter;
/*
char	instanceTestName[] = "rubber_ring_trigger01";
static bool instanceTest = true;
*/

// PP: ~~~~~ OBJECT FADE-OUT DISTANCES ~~~~~

#define FADEOUT_MIN_DIST_CHARACTER		METERTOUNIT(40.0f)				// PP: distance from viewpoint at which character instances start to fade out
#define FADEOUT_MAX_DIST_CHARACTER		METERTOUNIT(50.f)				// PP: distance from viewpoint at which character instances fade out completely
													
#if(BPLATFORM == XBOX)// PP: TEMP TEST!!!!!!!
						
float levmin=30;
float levmax=40;

#define FADEOUT_MIN_DIST_LEVEL			METERTOUNIT(levmin)				// PP: distance from viewpoint at which level instances start to fade out
#define FADEOUT_MAX_DIST_LEVEL			METERTOUNIT(levmax)				// PP: distance from viewpoint at which level instances fade out completely
					
#else// PP: !xbox
			
#define FADEOUT_MIN_DIST_LEVEL			METERTOUNIT(15.0f)				// PP: distance from viewpoint at which level instances start to fade out
#define FADEOUT_MAX_DIST_LEVEL			METERTOUNIT(20.f)				// PP: distance from viewpoint at which level instances fade out completely

#endif// PP: !xbox

#define FADEOUT_MIN_DIST_ANIMATED		FADEOUT_MIN_DIST_LEVEL			// PP: distance from viewpoint at which animated instances start to fade out
#define FADEOUT_MAX_DIST_ANIMATED		FADEOUT_MAX_DIST_LEVEL			// PP: distance from viewpoint at which animated instances fade out completely
																		
#define FADEOUT_MIN_DIST_FALLING		FADEOUT_MIN_DIST_LEVEL			// PP: distance from viewpoint at which falling instances start to fade out
#define FADEOUT_MAX_DIST_FALLING		FADEOUT_MAX_DIST_LEVEL			// PP: distance from viewpoint at which falling instances fade out completely
																		
#define FADEOUT_MIN_DIST_EFFECT			FADEOUT_MIN_DIST_LEVEL			// PP: distance from viewpoint at which falling instances start to fade out
#define FADEOUT_MAX_DIST_EFFECT			FADEOUT_MAX_DIST_LEVEL			// PP: distance from viewpoint at which falling instances fade out completely

#define FADEOUT_MIN_DIST_PHONEBOX		FADEOUT_MIN_DIST_CHARACTER		// PP: distance from viewpoint at which phonebox instances start to fade out
#define FADEOUT_MAX_DIST_PHONEBOX		FADEOUT_MAX_DIST_CHARACTER		// PP: distance from viewpoint at which phonebox instances fade out completely

// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// object attributes
static MAPOBJECTATTRIBS		objectAttribs;

// instance attributes
static MAPOBJECTATTRIBS		instanceAttribs;

//#define bkPrintf //

short	stCounters[NUM_SPECIAL_TYPES];

// IH: lomFilePak is the pointer to the lom file pak, make it global so we can check for NULL later
#if BPLATFORM==GAMECUBE
TBPackageIndex *lomFilePak;
#endif

/* --------------------------------------------------------------------------------
   Function : InitialiseMap
   Purpose : Load in a map file and create the scene
   Parameters : 
   Returns : 
   Info : 
*/
int	InitialiseMap(MAPINFO *map)
{
	TBVector	bounds = {METERTOUNIT(1.0f), METERTOUNIT(1.5f), METERTOUNIT(1.0f), METERTOUNIT(1.5f)};

	// NH: On PC (Dunno about other platforms), variables can not be defined more than
	// NH: once so please define them here rather than in multiple for loops.  Cheers
	int i;

	InitActorInstanceList(&map->characterInstances);

	InitActorInstanceList(&map->inactiveCharacterInstances);
	SetInstanceListParms(&map->inactiveCharacterInstances,0);
	
	InitActorInstanceList(&map->levelInstances);
	SetInstanceListParms(&map->levelInstances,0);
	
	InitActorInstanceList(&map->animatedInstances);
	
	InitActorInstanceList(&map->fallingInstances);
	SetInstanceListParms(&map->fallingInstances,0);
	
	InitActorInstanceList(&map->invisibleInstances);
	SetInstanceListParms(&map->invisibleInstances,0);
	
	InitActorInstanceList(&map->collisionInstances);
	SetInstanceListParms(&map->collisionInstances,0);
	
	InitActorInstanceList(&map->effectsInstances);
	SetInstanceListParms(&map->effectsInstances,0);
	
	InitActorInstanceList(&map->triggerInstances);
	SetInstanceListParms(&map->triggerInstances,0);
	
	InitActorInstanceList(&map->phoneBoxInstances);

	InitActorInstanceList(&map->statsInstances);

	InitialiseCommonlyUsedTextures();

	for (i=0;i<NUM_SPECIAL_TYPES;i++) stCounters[i] = 0;

	for (i=0;i<10;i++)
	{
		if (map->posterComplete[i])
		{
			map->posterComplete[i] = NULL;
		}
	}

	//InitOverlay();
	currentMap = map;
	map->meshPolys = 0;
	map->drawnPolys = 0;
	map->numberOfCollectibles = 0;
	map->numberOfDestructibles = 0;
	map->landscape = NULL;
	//initialise map stats
	map->mapStats.numCollectibles = 0;
	map->mapStats.numDestructibles = 0;
	map->mapStats.numFood = 0;
	map->mapStats.numPosters = 0;
	map->mapStats.numBurpCans = 0;

	map->fogMin = map->fogMinOrig = METERTOUNIT(20.0f);
	map->fogMax = map->fogMaxOrig = METERTOUNIT(300.0f);
	map->fogColour = COLOUR(80,80,200);
	map->fogState = true;
	map->allPostersDestroyed = false;

	//load everpresent actors which need to be loaded before map
	LoadActor("objects\\cloud.obe", map->mapName);
	
	map->pauseMap = new MAP;

	LoadLOMFile(map->mapName);

#if BPLATFORM == PC
	// NH: If farplane nearer than on PS2 (300) then have a look at scraping the envbox so it doesn't show through the walls.  This occasionally also needs fogging
	if (videoMode.farPlane < METERTOUNIT(300))
	{
		// NH: If in indoor based levels return as we don't want to show the envmap through walls that are clipped
		switch(map->sceneNumber)
		{
		case SCENE_CITYBOSS:	// NH: Needs fog enabled
		case SCENE_WESTBOSS:	// NH: Needs fog enabled
		case SCENE_TAZBOSS:		// NH: Needs fog enabled
			map->fogState = 1;
			SetFog(map->fogMin,map->fogMax,map->fogColour);
			break;
		}
	}

	bdSetViewClipPlanes(videoMode.nearPlane, METERTOUNIT(600));
	bdSetupView();
#endif

	map->pauseMap->LoadMapSplash();

	if (map->fogState)
	{
		SetFog(map->fogMin,map->fogMax,map->fogColour);
	}
	else
	{
		bdSetRenderState(BDRENDERSTATE_DISTANCEFOG, FALSE, FALSE);
	}

	// TP: flag the level as having been visited
	if (gameStatus.globalSettings.currentSaveSlot != -1)
	{
		if (map->sceneNumber < SCENE_NUMSCENES)
		{
			gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map->sceneNumber].flags |= LEVELFLAG_HASVISITEDLEVEL;
		}
	}

	bkHeapCheck();

	return TRUE;
}


/*  --------------------------------------------------------------------------------
	Function	: IsActorInstanceValid
	Purpose		: Can be used to test if this pointer points to a valid actorInstance structure
	Parameters	: actorInstance
	Returns		: true/false
	Info		: 
*/

bool IsActorInstanceValid(ACTORINSTANCE *actorInstance)
{
	ACTORINSTANCELIST	*list;
	ACTORINSTANCE		*ptr;

	list = actorInstance->instanceList;

	// TP: find if the instanceList pointer is valid
	if ((list == &characterInstances)||
		(list == &map.characterInstances)||
		(list == &map.inactiveCharacterInstances) ||
		(list == &map.levelInstances) ||
		(list == &map.animatedInstances) ||
		(list == &map.fallingInstances) ||
		(list == &map.invisibleInstances) ||
		(list == &map.collisionInstances) ||
		(list == &map.effectsInstances) ||
		(list == &map.triggerInstances) ||
		(list == &map.statsInstances) ||
		(list == &map.phoneBoxInstances))
	{
		if (list->numEntries==0) return false;

		// TP: now check that this actorInstance is still on the list
		for (ptr = list->head.next;ptr != &list->head;ptr = ptr->next)
		{
			if (ptr==actorInstance) return true;
		}
	}

	return false;
}


/* --------------------------------------------------------------------------------
   Function : FindActorInstanceInMap
   Purpose : Locates an actor instance in map
   Parameters : instance name, or crc, instance list
   Returns : ACTORINSTANCE or NULL
   Info : 
*/
ACTORINSTANCE *FindActorInstanceInMap(char *name, uint32 crc, MAPINFO *map)
{
	uint32 crcl;
	ACTORINSTANCE *actor;

	if (name) crcl = bkCRC32((uchar*)name,strlen(name),0);
	else	crcl = crc;

	if (actor = FindActorInstanceInInstanceList(NULL,crcl,&map->levelInstances)) return actor;
	if (actor = FindActorInstanceInInstanceList(NULL,crcl,&map->invisibleInstances)) return actor;
	if (actor = FindActorInstanceInInstanceList(NULL,crcl,&map->fallingInstances)) return actor;
	if (actor = FindActorInstanceInInstanceList(NULL,crcl,&map->collisionInstances)) return actor;
	if (actor = FindActorInstanceInInstanceList(NULL,crcl,&map->effectsInstances)) return actor;
	if (actor = FindActorInstanceInInstanceList(NULL,crcl,&map->animatedInstances)) return actor;
	if (actor = FindActorInstanceInInstanceList(NULL,crcl,&map->inactiveCharacterInstances)) return actor;
	if (actor = FindActorInstanceInInstanceList(NULL,crcl,&map->characterInstances)) return actor;
	if (actor = FindActorInstanceInInstanceList(NULL,crcl,&map->triggerInstances)) return actor;
	if (actor = FindActorInstanceInInstanceList(NULL,crcl,&map->phoneBoxInstances)) return actor;
	if (actor = FindActorInstanceInInstanceList(NULL,crcl,&characterInstances)) return actor;

	return NULL;
}


/* --------------------------------------------------------------------------------
   Function		: FreeActorInstances
   Purpose		: Free all instances of an actor
   Parameters	: (opt/NULL=all lists) ptr to instance list on which to find instances
   Returns		: 
   Info			: // PP: not too sure where this should go
*/
void FreeActorInstances(const char* const actorName, ACTORINSTANCELIST* const instanceList)
{
	if(instanceList == NULL)
	{
		// PP: check all instance lists...

		FreeActorInstances(actorName, &map.invisibleInstances);
		FreeActorInstances(actorName, &map.collisionInstances);
		FreeActorInstances(actorName, &map.effectsInstances);
		FreeActorInstances(actorName, &map.fallingInstances);
		FreeActorInstances(actorName, &map.animatedInstances);
		FreeActorInstances(actorName, &map.levelInstances);
		FreeActorInstances(actorName, &map.inactiveCharacterInstances);
		FreeActorInstances(actorName, &map.characterInstances);
		FreeActorInstances(actorName, &map.triggerInstances);
		FreeActorInstances(actorName, &map.phoneBoxInstances);
	}
	else
	{
		ACTORINSTANCE*		ptr;
		ACTORINSTANCE*		nextPtr;
		uint32				actorCRC;

		actorCRC = bkCRC32((uchar*)(actorName),strlen(actorName),0);

		// PP: for each actor instance in the instance list...
		for (ptr = instanceList->head.next;ptr != &instanceList->head;ptr = nextPtr)
		{
			nextPtr=ptr->next;

			// PP: if the actor instance is an instance of our specified actor...
			if (ptr->actorCRC == actorCRC)
			{
				// PP: free the actor instance
				FreeActorInstance(ptr);
			}
		}
	}
}


/* --------------------------------------------------------------------------------
   Function : PrepareMap
   Purpose : Prepare a map for drawing
   Parameters : 
   Returns : 
   Info : 
*/
int	PrepareMap(MAPINFO *map)
{
	int i;
	
	StartProfileTimer(&tonyProfile,0,"PrepMap");

	for (i=0;i<gameStatus.multiplayer.numPlayers;i++)
	{
		if (gameStatus.player[i].actorInstance)
		{
			bmVectorCopy(gameStatus.player[i].actorInstance->actorStatus->positionLastFrame, gameStatus.player[i].actorInstance->actorInstance.position);
			PrepSuperBurpBlastRadius(gameStatus.player[i].actorInstance);
		}
	}
	
	PrepInstancesPathInfo(&map->levelInstances);
	PrepInstancesPathInfo(&map->animatedInstances);

	if (gameStatus.currentState != GAMESTATE_CAMERACASE)
	{	
		StartProfileTimer(&tonyProfile,0,"PrepMap");
		ProcessCollisionForActors(&collisionCache,&characterInstances);
		ProcessCollisionForActors(&collisionCache,&map->characterInstances);
		StartProfileTimer(&tonyProfile,0,"PrepMap");

		AlignFeet(gameStatus.player1);

		PrepActorInstances(&characterInstances);
		PrepInstancesCharacterInfo(&characterInstances);
		
		PrepActorInstances(&map->characterInstances);
		PrepInstancesCharacterInfo(&map->characterInstances);
		CreateActorShadows(&map->characterInstances, &collisionCache);
	}
	
	PrepActorInstances(&map->fallingInstances);
	CreateActorShadows(&map->fallingInstances, &collisionCache);

	PrepActorInstances(&map->animatedInstances);
	PrepActorInstances(&map->invisibleInstances);
	PrepActorInstances(&map->levelInstances);
	PrepActorInstances(&map->effectsInstances);
	PrepActorInstances(&map->triggerInstances);
	PrepActorInstances(&map->phoneBoxInstances);
	PrepCloudBank();

#ifndef CONSUMERDEMO
	PrepFlockers(&map->invisibleInstances);

	for (i=0;i<gameStatus.multiplayer.numPlayers;i++)
	{
		if (gameStatus.player[i].actorInstance)
		{
			PrepVoodooCloud(gameStatus.player[i].actorInstance);
		}
	}
#endif

	EndProfileTimer(&tonyProfile,0);

	return TRUE;
}

void DrawMapSet(MAPINFO *map,CAMERAINFO *cameraInfo)
{
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	if (cameraInfo->cameraCase)
	{
		PrepareEnvMap(&environmentMap,cameraInfo->cameraCase->camera->pos);
	}
	else
	{
		PrepareEnvMap(&environmentMap,cameraInfo->pos);
	}
	RenderEnvMap(&environmentMap);

	SETZWRITE(TRUE);
	ENABLEBLEND(true);
	SETBLEND(BLEND);
	SETZTEST(LESSTHAN);
	baSetGlobalAlpha(256);
	
	bdSetIdentityObjectMatrix();
// PP: REMOUT: TEMP	StartProfileTimer(&profile,6,"Level Aprx");

// PP: LIGHTSOURCE TEST - TODO: UM, STUFF, yKNOW
#ifdef PHIL
	//bdSetRenderState(BDRENDERSTATE_LIGHTING, TRUE, 0);
	//bdSetRenderState(BDRENDERSTATE_LIGHTING, TRUE, 1);
#endif// PP: def PHIL

	if (map->landscape) baDrawSet(map->landscape,BSETPRIMALPHATYPE_SOLID);
}

void DrawMapSetAlpha(MAPINFO *map,CAMERAINFO *cameraInfo)
{
	TBSavedRenderStates save;

	bdSaveRenderStates(&save);

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);

	SETBLEND(BLEND);
	SETZTEST(LESSTHAN);

	baSetGlobalAlpha(256);
	bdSetIdentityObjectMatrix();

	SETZWRITE(TRUE);
	if (map->landscape) baDrawSet(map->landscape,BSETPRIMALPHATYPE_CKEY);
	SETZWRITE(FALSE);
	if (map->landscape) baDrawSet(map->landscape,BSETPRIMALPHATYPE_ALPHA);
	SETZWRITE(TRUE);

	bdRestoreRenderStates(&save);
}


/* --------------------------------------------------------------------------------
   Function : DrawMap
   Purpose : Draw a map
   Parameters : 
   Returns : 
   Info : 
*/
int	DrawMap(MAPINFO *map)
{
	CalculateCurrentAlpha(&characterInstances,		FADEOUT_MIN_DIST_CHARACTER,	FADEOUT_MAX_DIST_CHARACTER,		METERTOUNIT(0.0f),METERTOUNIT(0.0f));
	CalculateCurrentAlpha(&map->levelInstances,		FADEOUT_MIN_DIST_LEVEL,		FADEOUT_MAX_DIST_LEVEL,			METERTOUNIT(0.0f),METERTOUNIT(0.0f),2.0f);
	CalculateCurrentAlpha(&map->animatedInstances,	FADEOUT_MIN_DIST_ANIMATED,	FADEOUT_MAX_DIST_ANIMATED,		METERTOUNIT(0.0f),METERTOUNIT(0.0f),2.0f);
	CalculateCurrentAlpha(&map->fallingInstances,	FADEOUT_MIN_DIST_FALLING,	FADEOUT_MAX_DIST_FALLING,		METERTOUNIT(0.0f),METERTOUNIT(0.0f),2.0f);
	CalculateCurrentAlpha(&map->effectsInstances,	FADEOUT_MIN_DIST_EFFECT,	FADEOUT_MAX_DIST_EFFECT,		METERTOUNIT(0.0f),METERTOUNIT(0.0f),2.0f);
	CalculateCurrentAlpha(&map->phoneBoxInstances,	FADEOUT_MIN_DIST_PHONEBOX,	FADEOUT_MAX_DIST_PHONEBOX,		METERTOUNIT(0.0f),METERTOUNIT(0.0f),2.0f);

	StartProfileTimer(&profile,10,"opaque");

//	STARTTIMER;
	DrawActorInstances(&map->characterInstances);
//	ENDTIMER("characterInstances");
//
//	STARTTIMER;
	DrawActorInstances(&map->levelInstances);
//	ENDTIMER("levelInstances");

//	STARTTIMER;
	DrawActorInstances(&map->animatedInstances);
//	ENDTIMER("animatedInstances");

//	STARTTIMER;
	DrawActorInstances(&map->fallingInstances);
//	ENDTIMER("fallingInstances");

//	STARTTIMER;
	DrawActorInstances(&map->effectsInstances);
//	ENDTIMER("effectsInstances");

//	STARTTIMER;
	DrawActorInstances(&map->phoneBoxInstances);
//	ENDTIMER("phoneBoxInstances");

	EndProfileTimer(&profile,10);

	if (showInvisibleObjects) 
	{
		DrawActorInstances(&map->invisibleInstances);
		DrawActorInstances(&map->triggerInstances);
	}
	DrawCloudBankForeground();
	DrawActorShadows(&map->characterInstances);
	DrawActorShadows(&map->fallingInstances);
	DrawInstancesCharacterInfo(&map->characterInstances);
	StartProfileTimer(&profile,6,"flockers");

#ifndef CONSUMERDEMO
	DrawFlockers(&map->invisibleInstances);
	DrawVoodooCloud(gameStatus.player1);
#endif

	EndProfileTimer(&profile,6);
	DrawLaser(&map->invisibleInstances);
	DrawLaserBeams();
	DrawLightning(&map->invisibleInstances);
	DrawLightning(&map->levelInstances);
	DrawQuestionMarkAboveActor(&characterInstances);
	DrawQuestionMarkAboveActor(&map->characterInstances);
	MonitorFX_drawInstanceList(&map->levelInstances);			// PP: draw all monitors in the level instance list
	DrawDebugInfo(&map->characterInstances);
	//DrawVehicles(&map->levelInstances);							// PP: draw any special stuff to do with vehicles, eg. floor polisher trail			
	//DrawOverlay(&gameStatus.player1Stats);
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : DrawMap2
   Purpose : Draw alpha and ckey components of a map
   Parameters : 
   Returns : 
   Info : 
*/

bool	enterLevelFlag = false;

int	DrawMap2(MAPINFO *map)
{
	int		i;

	StartProfileTimer(&profile,11,"alpha");
	DrawActorInstances2(&map->characterInstances);
	DrawActorInstances2(&map->levelInstances);
	DrawActorInstances2(&map->animatedInstances);
	DrawActorInstances2(&map->fallingInstances);
	DrawActorInstances2(&map->effectsInstances);
	DrawActorInstances2(&map->phoneBoxInstances);

	DrawBountyList(&bountyList);
	EndProfileTimer(&profile,11);
	if (showInvisibleObjects) 
	{
		DrawActorInstances2(&map->invisibleInstances);
		DrawActorInstances2(&map->triggerInstances);
	}
	for(i=0;i<gameStatus.multiplayer.numPlayers;i++)
	{
		if(gameStatus.player[i].actorInstance)
		{
			DrawSuperBurpBlastRadius(gameStatus.player[i].actorInstance);
		}
	}
	DrawInstancesCharacterInfo2(&map->characterInstances);
	DrawPolyEffects(&polyEffectsList);

#ifndef CONSUMERDEMO
	DrawFlockers2(&map->invisibleInstances);
#endif

/*	if(showHubStats)
	{
		if ((gameStatus.currentState  == GAMESTATE_PAUSE || gameStatus.player[0].camera->flags&CAMERA_LOOKAROUND)&& !enterLevelFlag)
		{
			enterLevelBook.close();
			enterLevelFlag = true;
		}
		else if ((gameStatus.currentState != GAMESTATE_PAUSE && !(gameStatus.player[0].camera->flags&CAMERA_LOOKAROUND))  && enterLevelFlag)
		{
			enterLevelBook.open();
			enterLevelFlag = false;
		}
 PP: REMOUT: Now handled by the Book List				if(showHubStats)
		EnterHubUpdate(0);
	}*/
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : UpdateInstanceList
   Purpose : Process a list of special type objects and perform updates
   Parameters : ACTORINSTANCELIST
   Returns : 
   Info : 
*/
void UpdateInstanceList(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE	*ptr,*temp;

	ptr = actorInstanceList->head.next;

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;)
	{
		temp = ptr->next;
		if (ptr->pathInfo)
		{
			UpdatePathInfo(ptr,fTime);
		}
#ifndef CONSUMERDEMO
		if(ptr->characterInfo&&ptr->characterInfo->transportInfo)
		{
			UpdateCharacterInTransporter(ptr);
		}
#endif

		if (ptr->special != -1)
		{
			// TP: It's a special object
			switch(ptr->special)
			{
			case SPECIALTYPE_FALLINGOBJECT:
				UpdateFallingObject(ptr);
				break;
			case SPECIALTYPE_WATERFALL:
				UpdateWaterfallInstance(ptr);
				break;
			case SPECIALTYPE_STEPPINGSTONE:
				UpdateSteppingStone(ptr);
				break;
			case SPECIALTYPE_MAILBOX:
				UpdateMailboxes(ptr);
				break;
			case SPECIALTYPE_WANTEDPOSTER:
				UpdateSpecialCaseWantedPoster(ptr);
				break;
			case SPECIALTYPE_TILTINGOBJECT:
			case SPECIALTYPE_SEESAWOBJECT:
				UpdateTiltingObject(ptr);
				break;
			case SPECIALTYPE_DOOR:
				UpdateDoors(ptr);
				break;
			case SPECIALTYPE_SWINGINGOBJECT:
				UpdateSwingingObject(ptr);
				break;
			case SPECIALTYPE_PROJECTILE:
			case SPECIALTYPE_PROJECTILESLIDE:
				UpdateCannonState(ptr);
				break;
			case SPECIALTYPE_ELECTRICITY:
				UpdateElectricityObject(ptr);
				break;
			case SPECIALTYPE_PHONEBOX:
				UpdatePhoneBox(ptr);
				UpdatePhoneBoxEffect(ptr);
				break;
			case SPECIALTYPE_LIGHTNING:
				UpdateLightning(ptr, gameStatus.player1);
				break;
			case SPECIALTYPE_FOUNTAIN:
				UpdateFountains(ptr);
				break;
			case SPECIALTYPE_FIRE:
				UpdateFireInstances(ptr);
				break;
			case SPECIALTYPE_SHATTER:
				UpdateShatterObject(ptr);
				break;
			case SPECIALTYPE_DESTRUCTIBLE:
				UpdateSpiningDestructibleObjects(ptr);
				break;
			case SPECIALTYPE_MONITOR:
				MonitorFX_update(ptr);									// PP: update the monitor's screen effect
				break;
			case SPECIALTYPE_WATERYOBJECT:
				UpdateWater(ptr);
				break;
			case SPECIALTYPE_MAGNET:
				UpdateMagnetInstance(ptr);
				break;
#ifndef CONSUMERDEMO
			case SPECIALTYPE_FLOCKING:
				UpdateFlockers(ptr);
				break;
			case SPECIALTYPE_BUZZARD:
				UpdateBuzzard(ptr);
				break;
			case SPECIALTYPE_MINECART:
				UpdateMineCart(ptr);
				break;
			case SPECIALTYPE_FLAMETHROWA:
				UpdateFlameThrowa(ptr);
				break;
			case SPECIALTYPE_MECHATWEETY:
				UpdateMechaTweety(ptr);
				break;
			case SPECIALTYPE_TAZBOSSITEM:
				UpdateTazBossItemInstance(ptr);
				break;
			case SPECIALTYPE_ROCKET:
				UpdateSeekingRockets(ptr);
				break;
#endif
			case SPECIALTYPE_TARBUBBLE:
				UpdateTarBubbleEmmiter(ptr);
				break;
			case SPECIALTYPE_HALO:
				UpdateHaloObject(ptr);
				break;
			case SPECIALTYPE_ZAPPER:
				UpdateZapper(ptr);
				break;
			case SPECIALTYPE_HIGHVOLTAGE:
				UpdateHighVoltageObject(ptr);
				break;
			case SPECIALTYPE_SCENIC:
				UpdateScenicObject(ptr);
				break;
#ifndef CONSUMERDEMO
			case SPECIALTYPE_TUMBLEWEED:
				UpdateTumbleweed(ptr);
				break;
#endif
			case SPECIALTYPE_AIRCON:
				UpdateAirconUnit(ptr);
				break;
			case SPECIALTYPE_ARROW:
				UpdateSafariArrow(ptr);
				break;
			case SPECIALTYPE_LAVASPIT:
				UpdateLavaSpitSource(ptr);
				break;
#ifndef CONSUMERDEMO
			case SPECIALTYPE_MIRRORBALL:
				UpdateMirrorball(ptr);
				break;
#endif
			case SPECIALTYPE_WATERRIPPLE:
				UpdateWaterRingObject(ptr);
				break;
			case SPECIALTYPE_XDOOR:
				UpdateXDoorSparkles(ptr);
				break;
			}
		}
		ptr = temp;
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateMap
   Purpose : Update a map
   Parameters : 
   Returns : 
   Info : 
*/
int	UpdateMap(MAPINFO *map)
{
	int			i;

	// TP: test
	UpdateDestroyEverything(NULL);
	// TP: red
	StartProfileTimer(&tonyProfile,3,"UpMap");
	
	if (gameStatus.currentState != GAMESTATE_CAMERACASE)
	{	
		UpdatePhoneBoxes(&map->phoneBoxInstances);
		
		UpdateInstanceList(&map->levelInstances);
		UpdateInstanceList(&map->animatedInstances);
		UpdateInstanceList(&map->fallingInstances);
		UpdateInstanceList(&map->invisibleInstances);
		UpdateInstanceList(&map->phoneBoxInstances);
		UpdateInstanceList(&map->triggerInstances);

		// TP: old
		UpdateInstanceList(&map->characterInstances);
		UpdateInstanceList(&characterInstances);
		StartProfileTimer(&profile,3,"Update");
		UpdateLastEscalator(gameStatus.player1);

		AnimateActorInstances(&map->characterInstances);
		UpdateActorStatusInstances(&map->characterInstances);
		UpdateCharacterAIInstances(&map->characterInstances);
		UpdateCharacterAIInstances(&map->inactiveCharacterInstances);
		UpdateInstancesCharacterInfo(&characterInstances);
		UpdateInstancesCharacterInfo(&map->characterInstances);

		// TP: new
		UpdateFallingObjectList(&fallingObjectList);
		
		AnimateActorInstances(&map->animatedInstances);
		UpdateEffectsList(&effectsList);
	
	#ifndef CONSUMERDEMO	
		UpdateAllCactie();
		UpdateVoodooCloud(gameStatus.player1);
	#endif
		
		CheckForAreaTriggers(gameStatus.player1, &map->triggerInstances);
		
		UpdateSleepyZedsForCharacter(&characterInstances);
		UpdateSleepyZedsForCharacter(&map->characterInstances);
		UpdateQuestionMarkAboveActor(&characterInstances);
		UpdateQuestionMarkAboveActor(&map->characterInstances);
		
		UpdatePolyEffects(&polyEffectsList);
		UpdateLaserBeams();
		UpdateBountyList(&bountyList);
		UpdateMusicBoxInstances(&map->animatedInstances);
		
		for(i=0;i<gameStatus.multiplayer.numPlayers;i++)
		{
			if(gameStatus.player[i].actorInstance)
			{
				UpdateSuperBurpBlastRadius(gameStatus.player[i].actorInstance);
			}
		}
	}
	
	EndProfileTimer(&tonyProfile,3);

	return TRUE;
}


// PP: TEMP TEST
#include "nastycan.h"

/* --------------------------------------------------------------------------------
   Function : FreeMap
   Purpose : free a map
   Parameters : 
   Returns : 
   Info : 
*/
int	FreeMap(MAPINFO *map)
{
	FreeAllMirrors();

#ifndef CONSUMERDEMO
	FreeAllCactie();
#endif

	SAFE_DELETE(map->exitCutScene);
	SAFE_DELETE(map->exitCutScenePt2);
	SAFE_DELETE(map->introCutScene);
	SAFE_DELETE(map->introCutScenePt2);
	SAFE_DELETE(map->pauseMap);
	if (map->onLevelComplete) 
	{
		DeleteTriggerInfo(map->onLevelComplete);
		map->onLevelComplete = NULL;
	}
	for (int i=0;i<10;i++)
	{
		if (map->posterComplete[i])
		{
			DeleteTriggerInfo(map->posterComplete[i]);
			map->posterComplete[i] = NULL;
		}
	}
	FreeAllActorInstances(&map->characterInstances);
	FreeAllActorInstances(&map->inactiveCharacterInstances);
	FreeAllActorInstances(&map->levelInstances);
	FreeAllActorInstances(&map->animatedInstances);
	FreeAllActorInstances(&map->fallingInstances);
	FreeAllActorInstances(&map->collisionInstances);
	FreeAllActorInstances(&map->invisibleInstances);
	FreeAllActorInstances(&map->effectsInstances);
	FreeAllActorInstances(&map->triggerInstances);
	FreeAllActorInstances(&map->phoneBoxInstances);
	FreeAllPolyEffectsFromList(&polyEffectsList);
	DeleteLaserBeams();
	FreeWholePackageGroup(1);
	FreeWholePackageGroup(2);
	return TRUE;
}


/* --------------------------------------------------------------------------------
   Function : collateObjectInstances
   Purpose : node enumeration callback. collate all "Instance" nodes in this branch
   Parameters : node class pointer, context [float *]
   Returns : NENUM_CONTINUE - continue search, NENUM_END - terminate search, NENUM_ENDPATH - don't search below this node
   Info : 
*/
int collateObjectInstances(CPMNode *node, void *context)
{
	float			*transVecs;
	TBVector		position,scale;
	TBQuaternion	quat;
	ACTORINSTANCE	*tempInstance,*tempInstance2;
	PATH			*path;
	TRIGGERINFO		*tempTriggerInfo;
	DESTRUCTIBLEDEBRISINFO	*tempDebris;
	CAMERACASE		*cameraCase;
	int				tempInt;
	BOUNDINGBOX		boundingBox;
	BOUNDINGBOX		localBoundingBox;
	TBVector		tempVector;
	char			*tempString;
	int				flag = CAMERACASEFLAG_SMOOTH;

#ifdef TONY
//	PrintDebugVal();
#endif

	transVecs = (float *)context;

	// get position vector from this node
	if (!node->GetAttrib("PositionX", transVecs[0]))
	{
		if (!node->GetAttrib("PosX", transVecs[0]))
			transVecs[0] = 0.f;
	}

	if (!node->GetAttrib("PositionY", transVecs[1]))
	{
		if (!node->GetAttrib("PosY", transVecs[1]))
			transVecs[1] = 0.f;
	}

	if (!node->GetAttrib("PositionZ", transVecs[2]))
	{
		if (!node->GetAttrib("PosZ", transVecs[2]))
			transVecs[2] = 0.f;
	}

	position[X] = transVecs[0];
	position[Y] = transVecs[1];
	position[Z] = transVecs[2];

	// get rotation vector from this node
	if (!node->GetAttrib("RotationX", transVecs[4]))
	{
		if (!node->GetAttrib("RotX", transVecs[4]))
			transVecs[4] = 0.f;
	}

	if (!node->GetAttrib("RotationY", transVecs[5]))
	{
		if (!node->GetAttrib("RotY", transVecs[5]))
			transVecs[5] = 0.f;
	}

	if (!node->GetAttrib("RotationZ", transVecs[6]))
	{
		if (!node->GetAttrib("RotZ", transVecs[6]))
			transVecs[6] = 0.f;
	}

	if (!node->GetAttrib("RotationW", transVecs[7]))
	{
		if (!node->GetAttrib("RotW", transVecs[7]))
			transVecs[7] = 0.f;
	}

	quat[X] = transVecs[4];
	quat[Y] = transVecs[5];
	quat[Z] = transVecs[6];
	quat[W] = transVecs[7];

	// get scale vector from this node
	if (!node->GetAttrib("ScaleX", transVecs[8]))
	{
		if (!node->GetAttrib("SclX", transVecs[8]))
			transVecs[8] = 0.f;
	}

	if (!node->GetAttrib("ScaleY", transVecs[9]))
	{
		if (!node->GetAttrib("SclY", transVecs[9]))
			transVecs[9] = 0.f;
	}

	if (!node->GetAttrib("ScaleZ", transVecs[10]))
	{
		if (!node->GetAttrib("SclZ", transVecs[10]))
			transVecs[10] = 0.f;
	}

	scale[X] = transVecs[8];
	scale[Y] = transVecs[9];
	scale[Z] = transVecs[10];

	// Create instance with relevent information
	// use attributes to set object properly
	memcpy(&instanceAttribs,&objectAttribs,sizeof(MAPOBJECTATTRIBS));
	ChangeAttributes(&instanceAttribs,node);

/*	if (instanceTest && (strcmpi(instanceTestName, instanceAttribs.name)==0))
	{
		while(instanceTest)
		{
		}
	}*/

	instanceCounter++;

	tempInstance = NULL;

	short actorFlags = 0;

	if (instanceAttribs.liveUpdate)
	{
		bkStreamPrintf(&map.optimizeStream,"Instance \"%s\"[%s] \t\t has LIVE update.\n",instanceAttribs.name,currentObjectName);
	}
	if (!instanceAttribs.fade)
	{
		bkStreamPrintf(&map.optimizeStream,"Instance \"%s\"[%s] \t\t has FADE OUT turned off.\n",instanceAttribs.name,currentObjectName);
	}
	if (strcmpi(instanceAttribs.name,currentObject)==0)
	{
		bkStreamPrintf(&map.optimizeStream,"*** ERROR *** Object \"%s\" \t\t has Instances with _BANK_OBJECT properties.\n",currentObjectName);
	}

	if (!instanceAttribs.fade)
	{
		actorFlags |= ACTORFLAG_DONTFADE;
	}
	if (!instanceAttribs.fog)
	{
		actorFlags |= ACTORFLAG_DONTFOG;
	}
	if (instanceAttribs.invisible)
	{
		actorFlags |= ACTORFLAG_DONTDRAW;
	}

	if (instanceAttribs.specialType != -1)
	{
		// specialtype
		stCounters[instanceAttribs.specialType]++;
		switch(instanceAttribs.specialType)
		{
		case SPECIALTYPE_SPAWNCRATE:
			tempInstance = CreateActorInstance(&map.invisibleInstances, currentObjectName, instanceAttribs.name, position, scale, quat, actorFlags);
			tempInstance->special = SPECIALTYPE_SPAWNCRATE;
			return PMENUM_CONTINUE;
			
		case SPECIALTYPE_XDOOR:
			CreateActorInstanceXDoor(currentObjectName, instanceAttribs.name, instanceAttribs.triggerCRC, position, quat, scale, rootNode, node);
			return PMENUM_CONTINUE;

		case SPECIALTYPE_MIRROR:
			tempInstance = CreateActorInstance(&map.invisibleInstances, currentObjectName, instanceAttribs.name, position, scale, quat, actorFlags);
			baPrepActorInstance(&tempInstance->actorInstance,TRUE);

			new MIRROR(tempInstance);
			return PMENUM_CONTINUE;

		case SPECIALTYPE_MAPPOINT:
			int	mapX,mapY;

			// TP: tempInstance = CreateActorInstance(&map.levelInstances, currentObjectName, instanceAttribs.name, position, scale, quat, actorFlags);
			// TP: baPrepActorInstance(&tempInstance->actorInstance,TRUE);

			node->GetAttrib("mx", mapX);
			node->GetAttrib("my", mapY);

			if (map.pauseMap)
			{
				map.pauseMap->AddPoint(position,(short)mapX,(short)mapY);
			}

			// TP: FreeActorInstance(tempInstance);
			return PMENUM_CONTINUE;

		case SPECIALTYPE_CHECKPOINT:
			bkPrintf("   SPECIALTYPE_CHECKPOINT:%s\n",instanceAttribs.name);

			TBActor *actor;
			actor = FindActorInActorList(currentObjectName);
			if(actor)
			{
				if(instanceAttribs.width == 0)
					raceAddCheckPoint(position, quat, instanceAttribs.sequence, actor->maxRadius*scale[Y]);
				else
					raceAddCheckPoint(position, quat, instanceAttribs.sequence, instanceAttribs.width);
			}
			return PMENUM_CONTINUE;

		case SPECIALTYPE_TARGET:
			bkPrintf("   SPECIALTYPE_TARGET:%s\n",instanceAttribs.name);
			raceAddTarget(position, quat);
			return PMENUM_CONTINUE;

		case SPECIALTYPE_ENEMY:
			guiCreateEnemy(instanceAttribs.name, currentObjectName, instanceAttribs.modelName, position, quat, scale, instanceAttribs.triggerCRC, instanceAttribs.flags, instanceAttribs.useBox, instanceAttribs.state, instanceAttribs.cageCRC);
			return PMENUM_CONTINUE;

		case SPECIALTYPE_PHONEBOX:
			bkPrintf("   SPECIALTYPE_PHONEBOX:%s\n",instanceAttribs.name);
			tempInstance = CreateActorInstance(&map.phoneBoxInstances,"phonebox.obe","phonebox",position,NULL,quat, actorFlags);
			tempInstance2 = CreateActorInstance(&map.invisibleInstances,"phonebox_collision.obe","phoneboxcoll",position,NULL,quat, actorFlags);
			CreateActorInstancePhoneBox(tempInstance,tempInstance2,BOXTYPE_PHONE);
			return PMENUM_CONTINUE;

		case SPECIALTYPE_POLICEBOX:
			bkPrintf("   SPECIALTYPE_POLICEBOX:%s\n",instanceAttribs.name);
			if (IsPoliceBoxAccessible(instanceAttribs.modelName))
			{
				tempInstance = CreateActorInstance(&map.phoneBoxInstances,"extras\\policebox.obe","policebox",position,NULL,quat, actorFlags);
				tempInstance2 = CreateActorInstance(&map.invisibleInstances,"phonebox_collision.obe","phoneboxcoll",position,NULL,quat, actorFlags);
				CreateActorInstancePhoneBox(tempInstance,tempInstance2,BOXTYPE_POLICE,instanceAttribs.modelName);
			}
			return PMENUM_CONTINUE;

		case SPECIALTYPE_CAMERACASE:
			bkPrintf("   SPECIALTYPE_CAMERACASE:%s\n",instanceAttribs.name);
			tempInstance = CreateActorInstance(&currentMap->invisibleInstances, currentObjectName, instanceAttribs.name,position,scale,quat, actorFlags);
			cameraCase = CreateCameraCase(instanceAttribs.name,instanceAttribs.caseType);
			switch(instanceAttribs.caseType)
			{
			case CAMERACASETYPE_FIXEDPOINT:
				if (instanceAttribs.cameraFollow==NULL)
				{
					bkPrintf("      *** ERROR *** Fixed point camera has now camera follow attribute.\n");
				}
				CameraCaseFixedPoint(cameraCase,instanceAttribs.cameraFollow,instanceAttribs.cameraLookAt,instanceAttribs.caseTime,instanceAttribs.cameraLookAtNode,instanceAttribs.cameraFollowNode);
				break;
			case CAMERACASETYPE_ROTATEAROUND:
				CameraCaseRotateAround(cameraCase,instanceAttribs.cameraLookAt,METERTOUNIT(instanceAttribs.radius),RAD(instanceAttribs.angle), 0);
				break;
			case CAMERACASETYPE_PAN:
				CameraCasePan(cameraCase,instanceAttribs.cameraLookAt,instanceAttribs.cameraLookAtNode,instanceAttribs.cameraFollow,instanceAttribs.cameraDest,instanceAttribs.caseTime);
				break;
			default:
				bkPrintf("      *** ERROR *** Unknown camera case type.\n");
				break;
			}

			if (instanceAttribs.possnap!=-1)
			{
				if (instanceAttribs.possnap)
				{
					flag |= CAMERACASE_CAMERAPOSSNAP;
				}
				else
				{
					flag |= CAMERACASE_CAMERAPOSSMOOTH;
				}
			}
			if (instanceAttribs.lookatsnap!=-1)
			{
				if (instanceAttribs.lookatsnap)
				{
					flag |= CAMERACASE_CAMERALOOKATSNAP;
				}
				else
				{
					flag |= CAMERACASE_CAMERALOOKATSMOOTH;
				}
			}
			if (instanceAttribs.possnapback!=-1)
			{
				if (instanceAttribs.possnapback)
				{
					flag |= CAMERACASE_CAMERAPOSSNAPBACK;
				}
				else
				{
					flag |= CAMERACASE_CAMERAPOSSMOOTHBACK;
				}
			}
			if (instanceAttribs.lookatsnapback!=-1)
			{
				if (instanceAttribs.lookatsnapback)
				{
					flag |= CAMERACASE_CAMERALOOKATSNAPBACK;
				}
				else
				{
					flag |= CAMERACASE_CAMERALOOKATSMOOTHBACK;
				}
			}
			
			if (instanceAttribs.smooth==FALSE)
			{
				flag &=~ CAMERACASEFLAG_SMOOTH;
			}

			if (instanceAttribs.transitionControl==true)
			{
				flag |= CAMERACASE_TRANSITIONCONTROL;
			}
			else
			{
				flag &= ~CAMERACASE_TRANSITIONCONTROL;
			}

			if (instanceAttribs.triggerCRC)
			{
				// TP: if taz needs to be locked in position
				cameraCase->lockTazCRC = instanceAttribs.triggerCRC;
			}

			if (instanceAttribs.cameraStartDelay)
			{
				CameraCaseStartDelay(cameraCase,instanceAttribs.cameraStartDelay);
			}

			if (map.exitCutScene==NULL)
			{
				TRIGGERINFO *tempTrigger;
				char		tempChar[256];

				sprintf(tempChar, "%s_ecs", map.mapName);

				tempTrigger = GetTrigger("REMOVEOBJECT",node);
				if (tempTrigger)
				{
					// TP: create exit cutscene
					if(!map.exitCutScene) 
					{
						flag |= CAMERACASE_EXITCUTSCENE;
						map.exitCutScene = new CUTSCENE(tempChar);
						ChooseMusicForOutroCutscene(map.exitCutScene);
					}

					for (int i=0;i < tempTrigger->numMessages;i++)
					{
						// CMD: add one to move past underscore
						map.exitCutScene->AddEntry(REMOVEOBJECT, (tempTrigger->messages[i]+1));
					}
					DeleteTriggerInfo(tempTrigger);
				}
				tempTrigger = GetTrigger("ADDNORMALOBJECT",node);
				if (tempTrigger)
				{
					// TP: create exit cutscene
					if(!map.exitCutScene) 
					{
						flag |= CAMERACASE_EXITCUTSCENE;
						map.exitCutScene = new CUTSCENE(tempChar);
						ChooseMusicForOutroCutscene(map.exitCutScene);
					}

					for (int i=0;i < tempTrigger->numMessages;i++)
					{
						// CMD: add one to move past underscore
						map.exitCutScene->AddEntry(ADDNORMALOBJECT, (tempTrigger->messages[i]+1));
					}
					DeleteTriggerInfo(tempTrigger);
				}
				tempTrigger = GetTrigger("ADDCARTOONOBJECT",node);
				if (tempTrigger)
				{
					// TP: create exit cutscene
					if(!map.exitCutScene) 
					{
						flag |= CAMERACASE_EXITCUTSCENE;
						map.exitCutScene = new CUTSCENE(tempChar);
						ChooseMusicForOutroCutscene(map.exitCutScene);
					}

					for (int i=0;i < tempTrigger->numMessages;i++)
					{
						// CMD: add one to move past underscore
						map.exitCutScene->AddEntry(ADDCARTOONOBJECT, (tempTrigger->messages[i]+1));
					}
					DeleteTriggerInfo(tempTrigger);
				}
			}

			CameraCaseAttribs(cameraCase,0,flag);

			cameraCase->onStart = GetTrigger("ONSTART",node);
			if (!cameraCase->onStart)
				cameraCase->onStart = GetTrigger("ONSTART",rootNode);

			cameraCase->onEnd = GetTrigger("ONEND",node);
			if (!cameraCase->onEnd)
				cameraCase->onEnd = GetTrigger("ONEND",rootNode);
			return PMENUM_CONTINUE;
		case SPECIALTYPE_PATH:
			bkPrintf("   SPECIALTYPE_PATH:%s\n",instanceAttribs.name);
			CreateSpecialTypePath(node,&instanceAttribs,position,quat,scale);
			return PMENUM_CONTINUE;
		case SPECIALTYPE_PLATFORM:
			bkPrintf("   SPECIALTYPE_PLATFORM:%s\n",instanceAttribs.name);
			CreateSpecialTypePath(node,&instanceAttribs,position,quat,scale);
			tempInstance = CreateActorInstance(&currentMap->levelInstances,currentObjectName,instanceAttribs.name,position,scale,quat, actorFlags);
			currentMap->drawnPolys += numPolysInObject;

			if (instanceAttribs.invisible)
			{
				tempInstance->flags |= ACTORFLAG_DONTDRAW;
			}
			
			if (instanceAttribs.collision)
			{
				AddActorToCollisionCache(currentMap->collisionCache,tempInstance,CACHEFLAGS_LIVEUPDATE|instanceAttribs.liveUpdate);
			}

			if (tempInstance->actorInstance.numAnimSegments)
			{
				RemoveInstanceFromInstanceList(tempInstance,&currentMap->levelInstances);
				AddInstanceToInstanceList(tempInstance,&currentMap->animatedInstances);
				baQueueNodeAnimNormal(&tempInstance->actorInstance, tempInstance->actorInstance.rootNodeInstance, 
							&tempInstance->actorInstance.animSegments[0], 512+(bmRand()%200), -1, BANIMQUEUEFLAG_LOOPING, NULL,NULL);
			}

			tempString = node->GetAttrib("path");

			CreateActorInstancePathInfo(tempInstance);
			tempInstance->pathInfo->start = GetTrigger("ONSTART",node);
			if (!tempInstance->pathInfo->start)
				tempInstance->pathInfo->start = GetTrigger("ONSTART",rootNode);
			tempInstance->pathInfo->end = GetTrigger("ONEND",node);
			if (!tempInstance->pathInfo->end)
				tempInstance->pathInfo->end = GetTrigger("ONEND",rootNode);

			if (instanceAttribs.platformCameraControl==true)
			{
				tempInstance->pathInfo->flags |= PATHFLAG_CAMERACONTROL;
				tempInstance->pathInfo->cameraDist = instanceAttribs.radius;
			}

			if(instanceAttribs.returnSpeed > 1.0f) instanceAttribs.returnSpeed = 1.0f;
			if(instanceAttribs.returnSpeed < 0.0f) instanceAttribs.returnSpeed = 0.0f;
			tempInstance->pathInfo->returnSpeed = instanceAttribs.returnSpeed;

			if (tempString)
			{
				if (path = FindPathInList(tempString,0))
				{
					ResetPathInfo(tempInstance->pathInfo,path);
					GetPathPosition(tempInstance->pathInfo,tempInstance->actorInstance.position);
					GetPathOrientation(tempInstance->pathInfo,tempInstance->actorInstance.orientation);
					GetPathScale(tempInstance->pathInfo,tempInstance->actorInstance.scale);
					SetPathType(tempInstance->pathInfo,PATHTYPE_NORMAL);
					SetPathLoop(tempInstance->pathInfo,(EPathLoopMode)instanceAttribs.platformType);
					if (instanceAttribs.platformType==PATHLOOP_LOOP)
					{
						FollowPathClockwise(tempInstance->pathInfo);
					}
					else
					{
						if (instanceAttribs.platformType==PATHLOOP_YOYO)
						{
							FollowPathClockwise(tempInstance->pathInfo);
						}
						else
						{
							FollowPathClockwise(tempInstance->pathInfo);
							ChangePathState(tempInstance->pathInfo,PATHSTATE_WAITING);
						}
					}
					SetPathType(tempInstance->pathInfo,(EPathType)instanceAttribs.platformMode);
				}
				else
				{
					// must fix up later
					strcpy(tempInstance->pathInfo->pathNameFixup,tempString);
					tempInstance->pathInfo->loopMode = instanceAttribs.platformType;
					tempInstance->pathInfo->type = instanceAttribs.platformMode;
				}
			}
			else
			{
				if (path = FindPathInList(instanceAttribs.name,0))
				{
					ResetPathInfo(tempInstance->pathInfo,path);
					GetPathPosition(tempInstance->pathInfo,tempInstance->actorInstance.position);
					GetPathOrientation(tempInstance->pathInfo,tempInstance->actorInstance.orientation);
					GetPathScale(tempInstance->pathInfo,tempInstance->actorInstance.scale);
					SetPathLoop(tempInstance->pathInfo,(EPathLoopMode)instanceAttribs.platformType);
					if (instanceAttribs.platformType==PATHLOOP_LOOP)
					{
						FollowPathClockwise(tempInstance->pathInfo);
						SetPathType(tempInstance->pathInfo,PATHTYPE_NORMAL);
					}
					else
					{
						if (instanceAttribs.platformType==PATHLOOP_YOYO)
						{
							FollowPathClockwise(tempInstance->pathInfo);
							SetPathType(tempInstance->pathInfo,PATHTYPE_NORMAL);
						}
						else
						{
							FollowPathClockwise(tempInstance->pathInfo);
							ChangePathState(tempInstance->pathInfo,PATHSTATE_WAITING);
							SetPathType(tempInstance->pathInfo,PATHTYPE_NORMAL);
						}
					}
					SetPathType(tempInstance->pathInfo,(EPathType)instanceAttribs.platformMode);
				}
				else
				{
					DeleteTriggerInfo(tempInstance->pathInfo->start);
					DeleteTriggerInfo(tempInstance->pathInfo->end);
					SAFE_FREE(tempInstance->pathInfo);
					tempInstance->special = (ESpecialType)-1;

					bkPrintf("      *** ERROR *** Could not find path specified, path has now been demoted to standard object.\n");
				}
			}
		
			return PMENUM_CONTINUE;
		case SPECIALTYPE_PROJECTILE:
			bkPrintf("   SPECIALTYPE_PROJECTILE:%s\n",instanceAttribs.name);
			if(instanceAttribs.invisible)
			{
				tempInstance = CreateActorInstance(&currentMap->invisibleInstances,currentObjectName,instanceAttribs.name,position,scale,quat, actorFlags);
			}
			else
			{
				tempInstance = CreateActorInstance(&currentMap->animatedInstances,currentObjectName,instanceAttribs.name,position,scale,quat, actorFlags);
			}
			currentMap->drawnPolys += numPolysInObject;
			
			if(CreateActorInstanceProjectile(tempInstance, instanceAttribs.projectTo, instanceAttribs.projectAngle, SPECIALTYPE_PROJECTILE, instanceAttribs.contact, instanceAttribs.collision, instanceAttribs.liveUpdate, instanceAttribs.cannonEffect,instanceAttribs.flags))
			{
				tempInstance->projectileInfo->onContact = GetTrigger("ONCONTACT",node);
				if (!tempInstance->projectileInfo->onContact)
					tempInstance->projectileInfo->onContact = GetTrigger("ONCONTACT",rootNode);

				tempInstance->projectileInfo->onAim = GetTrigger("ONAIM",node);
				if (!tempInstance->projectileInfo->onAim)
					tempInstance->projectileInfo->onAim = GetTrigger("ONAIM",rootNode);

				tempInstance->projectileInfo->onFire = GetTrigger("ONFIRE",node);
				if (!tempInstance->projectileInfo->onFire)
					tempInstance->projectileInfo->onFire = GetTrigger("ONFIRE",rootNode);

				tempInstance->projectileInfo->onRecoil = GetTrigger("ONRECOIL",node);
				if (!tempInstance->projectileInfo->onRecoil)
					tempInstance->projectileInfo->onRecoil = GetTrigger("ONRECOIL",rootNode);
			}

			if (!instanceAttribs.state)
			{
				tempInstance->projectileInfo->flags |= CANNONFLAG_INACTIVE;	// TP: initial state for the trigger active or not
			}

			return PMENUM_CONTINUE;

		case SPECIALTYPE_PROJECTILESLIDE:
			bkPrintf("   SPECIALTYPE_PROJECTILESLIDE:%s\n",instanceAttribs.name);
			if(instanceAttribs.invisible)
			{
				tempInstance = CreateActorInstance(&currentMap->invisibleInstances,currentObjectName,instanceAttribs.name,position,scale,quat, actorFlags);
			}
			else
			{
				tempInstance = CreateActorInstance(&currentMap->animatedInstances,currentObjectName,instanceAttribs.name,position,scale,quat, actorFlags);
			}
			currentMap->drawnPolys += numPolysInObject;
			
			if(CreateActorInstanceProjectile(tempInstance, instanceAttribs.projectTo, instanceAttribs.projectAngle, SPECIALTYPE_PROJECTILESLIDE, instanceAttribs.contact,true,DEFAULT_CANNON_EFFECT,instanceAttribs.flags))
			{
				tempInstance->projectileInfo->onContact = GetTrigger("ONCONTACT",node);
				if (!tempInstance->projectileInfo->onContact)
					tempInstance->projectileInfo->onContact = GetTrigger("ONCONTACT",rootNode);

				tempInstance->projectileInfo->onAim = GetTrigger("ONAIM",node);
				if (!tempInstance->projectileInfo->onAim)
					tempInstance->projectileInfo->onAim = GetTrigger("ONAIM",rootNode);

				tempInstance->projectileInfo->onFire = GetTrigger("ONFIRE",node);
				if (!tempInstance->projectileInfo->onFire)
					tempInstance->projectileInfo->onFire = GetTrigger("ONFIRE",rootNode);

				tempInstance->projectileInfo->onRecoil = GetTrigger("ONRECOIL",node);
				if (!tempInstance->projectileInfo->onRecoil)
					tempInstance->projectileInfo->onRecoil = GetTrigger("ONRECOIL",rootNode);
			}

			if (!instanceAttribs.state)
			{
				tempInstance->projectileInfo->flags |= CANNONFLAG_INACTIVE;	// TP: initial state for the trigger active or not
			}

			return PMENUM_CONTINUE;
		case SPECIALTYPE_TRAMAMPOLINE:
			bkPrintf("   SPECIALTYPE_TRAMAMPOLINE:%s\n",instanceAttribs.name);

			tempInstance = CreateActorInstance(&currentMap->levelInstances,currentObjectName,instanceAttribs.name,position,scale,quat, actorFlags);
			if (tempInstance->actorInstance.numAnimSegments)
			{
				RemoveInstanceFromInstanceList(tempInstance,&currentMap->levelInstances);
				AddInstanceToInstanceList(tempInstance,&currentMap->animatedInstances);
			}
			currentMap->drawnPolys += numPolysInObject;

			if(CreateSpringyObject(tempInstance, position, instanceAttribs.springyHeight, SPECIALTYPE_TRAMAMPOLINE, instanceAttribs.springyRadius, instanceAttribs.alwaysBounce,instanceAttribs.liveUpdate,instanceAttribs.state!=0,instanceAttribs.collision))	// TP: !=0 forces char to bool value
			{
				tempInstance->springyObjectInfo->onContact = GetTrigger("ONCONTACT",node);
				if (!tempInstance->springyObjectInfo->onContact)
					tempInstance->springyObjectInfo->onContact = GetTrigger("ONCONTACT",rootNode);

			}
			return PMENUM_CONTINUE;
		case SPECIALTYPE_STEPPINGSTONE:
			bkPrintf("   SPECIALTYPE_STEPPINGSTONE:%s\n",instanceAttribs.name);
			tempInstance = CreateActorInstance(&currentMap->levelInstances,currentObjectName,instanceAttribs.name,position,scale,quat, actorFlags);
			if (!tempInstance) return PMENUM_CONTINUE;
			currentMap->drawnPolys += numPolysInObject;

			CreateSteppingStoneObject(tempInstance,instanceAttribs.steppingStoneHeight,position,instanceAttribs.bobbleTime, instanceAttribs.sinkAcceleration, instanceAttribs.bobbleHeight);
			return PMENUM_CONTINUE;
		case SPECIALTYPE_TILTINGOBJECT:
			bkPrintf("   SPECIALTYPE_TILTINGOBJECT:%s\n",instanceAttribs.name);
			tempInstance = CreateActorInstance(&currentMap->levelInstances,currentObjectName,instanceAttribs.name,position,scale,quat, actorFlags);
			currentMap->drawnPolys += numPolysInObject;

			CreateTiltingObject(tempInstance, instanceAttribs.xAngleMax, instanceAttribs.zAngleMax, instanceAttribs.maxRadius, position, instanceAttribs.maxAcceleration, instanceAttribs.retardationForce);
			return PMENUM_CONTINUE;
		case SPECIALTYPE_DOOR:
			bkPrintf("   SPECIALTYPE_DOOR:%s\n", instanceAttribs.name);
			tempInstance = CreateActorInstance(&currentMap->levelInstances, currentObjectName, instanceAttribs.name,position,scale,quat, actorFlags);
			currentMap->drawnPolys += numPolysInObject;

			CreateDoor(tempInstance, instanceAttribs.doorSpringyness, instanceAttribs.doorStiffness, instanceAttribs.doorType);
			return PMENUM_CONTINUE;
		case SPECIALTYPE_SWINGINGOBJECT:
			bkPrintf("   SPECIALTYPE_SWINGINGOBJECT:%s\n",instanceAttribs.name);
			tempInstance = CreateActorInstance(&currentMap->invisibleInstances,currentObjectName,instanceAttribs.name,position,scale,quat, actorFlags);
			currentMap->drawnPolys += numPolysInObject;
			
			if(CreateSwingingObject(tempInstance, position, instanceAttribs.length, instanceAttribs.model, instanceAttribs.modelName))
			{
				tempInstance->swingingObjectInfo->onContact = GetTrigger("ONCONTACT", node);
				if(!tempInstance->swingingObjectInfo->onContact)
					tempInstance->swingingObjectInfo->onContact = GetTrigger("ONCONTACT", rootNode);
			}
			return PMENUM_CONTINUE;
		case SPECIALTYPE_WATERYOBJECT:
			bkPrintf("   SPECIALTYPE_WATERYOBJECT:%s\n",instanceAttribs.name);
			tempInstance = CreateActorInstance(&currentMap->levelInstances,currentObjectName,instanceAttribs.name,position,scale,quat, actorFlags);
			currentMap->drawnPolys += numPolysInObject;
#if 0
			tempInt = NumberOfTrisInActor(&tempInstance->actorInstance)*3;
#else
			tempInt = NumberOfVertsInActor(&tempInstance->actorInstance);
#endif
			
			CalculateBoundingBox(&boundingBox, &localBoundingBox, tempInstance, NULL, tempInt);

			if(CreateWateryObject(tempInstance, &boundingBox, instanceAttribs.numWaterResetPoints, position, instanceAttribs.waterType))
			{
				tempInstance->wateryObjectInfo->resetInfo = GetWaterResetObjectsFromMap("resetObject", node);
				if(!tempInstance->wateryObjectInfo->resetInfo)
				{
					tempInstance->wateryObjectInfo->resetInfo = GetWaterResetObjectsFromMap("resetObject", rootNode);
				}
			}
			return PMENUM_CONTINUE;
		case SPECIALTYPE_WATERRIPPLE:
			bkPrintf("   SPECIALTYPE_WATERRIPPLE:%s\n", instanceAttribs.name);
			tempInstance = CreateActorInstance(&currentMap->invisibleInstances, currentObjectName, instanceAttribs.name, position, scale, quat, actorFlags);
			if(tempInstance)
			{
				CreateWaterRingObject(tempInstance, instanceAttribs.speedboost, instanceAttribs.projectAngle, instanceAttribs.deadFlockerCRC, instanceAttribs.delay);
			}
			return PMENUM_CONTINUE;
		case SPECIALTYPE_ELECTRICITY:
			bkPrintf("   SPECIALTYPE_ELECTRICITY:%s\n",instanceAttribs.name);
			//tempInstance = CreateActorInstance(&currentMap->levelInstances, currentObjectName, instanceAttribs.name);
			//currentMap->drawnPolys += numPolysInObject;

			//bmVectorCopy(tempInstance->actorInstance.scale,scale);
			//bmQuatCopy(tempInstance->actorInstance.orientation,quat);

			CreateElectricityObject(instanceAttribs.electricityAccuracy, currentObjectName, instanceAttribs.name, scale, instanceAttribs.state, node);
			return PMENUM_CONTINUE;
		case SPECIALTYPE_LASER:
			bkPrintf("   SPECIALTYPE_LASER: %s\n", instanceAttribs.name);

			CreateLaser(instanceAttribs.state,currentObjectName,instanceAttribs.name,instanceAttribs.laserTexture,instanceAttribs.laserSize,node,rootNode);
			return PMENUM_CONTINUE;
		case SPECIALTYPE_LIGHTNING:
			bkPrintf("   SPECIALTYPE_LIGHTNING: %s\n", instanceAttribs.name);

			CreateLightning(currentObjectName, instanceAttribs.name, node, rootNode);
			return PMENUM_CONTINUE;
		case SPECIALTYPE_MAILBOX:
			bkPrintf("   SPECIALTYPE_MAILBOX:%s, number: %d\n",instanceAttribs.name, instanceAttribs.mailboxNumber);
			tempInstance = CreateActorInstance(&currentMap->animatedInstances,currentObjectName,instanceAttribs.name,position,scale,quat, actorFlags);
			currentMap->drawnPolys += numPolysInObject;

			if(CreateMailboxInstance(tempInstance, position, instanceAttribs.mailboxNumber, FALSE))
			{
				tempInstance->mailboxInfo->onContact = GetTrigger("ONCONTACT",node);
				if (!tempInstance->mailboxInfo->onContact)
					tempInstance->mailboxInfo->onContact = GetTrigger("ONCONTACT",rootNode);

				tempInstance->mailboxInfo->onExcited = GetTrigger("ONEXCITED",node);
				if (!tempInstance->mailboxInfo->onExcited)
					tempInstance->mailboxInfo->onExcited = GetTrigger("ONEXCITED",rootNode);

				tempInstance->mailboxInfo->onVeryExcited = GetTrigger("ONVERYEXCITED",node);
				if (!tempInstance->mailboxInfo->onVeryExcited)
					tempInstance->mailboxInfo->onVeryExcited = GetTrigger("ONVERYEXCITED",rootNode);
			}
			return PMENUM_CONTINUE;
		case SPECIALTYPE_WANTEDPOSTER:
			bkPrintf("   SPECIALTYPE_WANTEDPOSTER:%s, number: %d\n",instanceAttribs.name, instanceAttribs.posterNumber);
			tempInstance = CreateActorInstance(&currentMap->animatedInstances,currentObjectName,instanceAttribs.name,position,scale,quat, actorFlags);
			currentMap->drawnPolys += numPolysInObject;
			currentMap->numberOfDestructibles += 1;

			if(CreateWantedPosterInstance(tempInstance, instanceAttribs.posterNumber, instanceAttribs.posterSpecialCase))
			{
				tempInstance->wantedPosterInfo->onDestruction = GetTrigger("ONDESTRUCTION", node);
				if(!tempInstance->wantedPosterInfo->onDestruction)
					tempInstance->wantedPosterInfo->onDestruction= GetTrigger("ONDESTRUCTION", rootNode);
			}
			return PMENUM_CONTINUE;
		case SPECIALTYPE_DESTRUCTIBLE:
			bkPrintf("   SPECIALTYPE_DESTRUCTIBLE:%s\n",instanceAttribs.name);
			if(instanceAttribs.invisible)
			{
				tempInstance = CreateActorInstance(&currentMap->invisibleInstances,currentObjectName,instanceAttribs.name,position,scale,quat, actorFlags);
			}
			else
			{
				tempInstance = CreateActorInstance(&currentMap->levelInstances,currentObjectName,instanceAttribs.name,position,scale,quat, actorFlags);
			}

			if (tempInstance->actorInstance.numAnimSegments)
			{
				// move instance to animated list
				RemoveInstanceFromInstanceList(tempInstance,&currentMap->levelInstances);
				AddInstanceToInstanceList(tempInstance,&currentMap->animatedInstances);
			}
			currentMap->drawnPolys += numPolysInObject;
			currentMap->numberOfDestructibles += 1;

			SetupDestructible(currentMap,node,rootNode,&instanceAttribs,tempInstance);

			return PMENUM_CONTINUE;
		case SPECIALTYPE_FALLINGOBJECT:
			bkPrintf("   SPECIALTYPE_FALLINGOBJECT:%s\n",instanceAttribs.name);
			tempTriggerInfo = GetTrigger("ONDESTRUCTION",node);
			if (!tempTriggerInfo)
				tempTriggerInfo = GetTrigger("ONDESTRUCTION",rootNode);
			tempDebris = GetDebrisObjectsFromMap("debrisObject", node);
			if(!tempDebris) tempDebris = GetDebrisObjectsFromMap("debrisObject", rootNode);

			AddEntryToFallingObjectList(currentObjectName, instanceAttribs.name, position, scale, instanceAttribs.fallingRadius, instanceAttribs.fallingPeriod, instanceAttribs.impactType, instanceAttribs.fallingType, tempTriggerInfo, tempDebris,instanceAttribs.invisible,instanceAttribs.state);
			return PMENUM_CONTINUE;
		case SPECIALTYPE_LAVA:
			bkPrintf("   SPECIALTYPE_LAVA:%s\n",instanceAttribs.name);
			if(instanceAttribs.invisible)
			{
				tempInstance = CreateActorInstance(&currentMap->invisibleInstances,currentObjectName,instanceAttribs.name,position,scale,quat, actorFlags);
			}
			else
			{
				tempInstance = CreateActorInstance(&currentMap->levelInstances,currentObjectName,instanceAttribs.name,position,scale,quat, actorFlags);
			}
			currentMap->drawnPolys += numPolysInObject;

			if (instanceAttribs.collision)
			{
				AddActorToCollisionCache(currentMap->collisionCache,tempInstance,instanceAttribs.liveUpdate);
			}

			if(CreateLavaObject(tempInstance,instanceAttribs.flags!=0))
			{
/*				tempInstance->lavaObjectInfo->onContact = GetTrigger("ONCONTACT",node);
				if (!tempInstance->lavaObjectInfo->onContact)
					tempInstance->lavaObjectInfo->onContact = GetTrigger("ONCONTACT",rootNode);
*/			}
			return PMENUM_CONTINUE;
		case SPECIALTYPE_LAVASPIT:
			bkPrintf("   SPECIALTYPE_LAVASPIT: %s\n", instanceAttribs.name);
			CreateLavaSpitSource(CreateActorInstance(&currentMap->invisibleInstances, currentObjectName, instanceAttribs.name, position, scale, quat, actorFlags));
			return PMENUM_CONTINUE;
		case SPECIALTYPE_HIGHVOLTAGE:
			bkPrintf("   SPECIALTYPE_HIGHVOLTAGE:%s\n",instanceAttribs.name);
			if(instanceAttribs.invisible)
			{
				tempInstance = CreateActorInstance(&currentMap->invisibleInstances,currentObjectName,instanceAttribs.name,position,scale,quat, actorFlags);
			}
			else
			{
				tempInstance = CreateActorInstance(&currentMap->animatedInstances,currentObjectName,instanceAttribs.name,position,scale,quat, actorFlags);
			}
			currentMap->drawnPolys += numPolysInObject;

			if(CreateHighVoltageObject(tempInstance, instanceAttribs.voltageState, instanceAttribs.initialVoltageState))
			{
				tempInstance->highVoltageObjectInfo->onContact = GetTrigger("ONCONTACT",node);
				if (!tempInstance->highVoltageObjectInfo->onContact)
					tempInstance->highVoltageObjectInfo->onContact = GetTrigger("ONCONTACT",rootNode);
			}
			return PMENUM_CONTINUE;
		case SPECIALTYPE_PRECIPITATION:
			bkPrintf("   SPECIALTYPE_PRECIPITATION:%s\n",instanceAttribs.name);
			tempInstance = CreateActorInstance(&currentMap->invisibleInstances, currentObjectName, instanceAttribs.name);
#if 0
			tempInt = NumberOfTrisInActor(&tempInstance->actorInstance)*3;
#else
			tempInt = NumberOfVertsInActor(&tempInstance->actorInstance);
#endif
			CalculateBoundingBox(&boundingBox, &localBoundingBox, tempInstance, NULL, tempInt);
			tempVector[0] = localBoundingBox.xmin + position[0];
			tempVector[1] = localBoundingBox.xmax + position[0];
			tempVector[2] = localBoundingBox.zmin + position[2];
			tempVector[3] = localBoundingBox.zmax + position[2];
			AddEntryToPrecipitationList(tempVector, gameStatus.windVelocity, instanceAttribs.snowDensity, METERTOUNIT(30.0f), instanceAttribs.snowDim, instanceAttribs.snowDim, instanceAttribs.snowType, instanceAttribs.snowRate);
			return PMENUM_CONTINUE;
		case SPECIALTYPE_FIRE:
			bkPrintf("   SPECIALTYPE_FIRE:%s\n",instanceAttribs.name);
			tempInstance = CreateActorInstance(&currentMap->invisibleInstances, currentObjectName, instanceAttribs.name,position,scale,quat, actorFlags);

			CreateFireInstance(tempInstance, instanceAttribs.fireSize);
			return PMENUM_CONTINUE;
		case SPECIALTYPE_SPEEDBOOST:
			bkPrintf("   SPECIALTYPE_SPEEDBOOST:%s\n",instanceAttribs.name);
			if(instanceAttribs.invisible)
			{
				tempInstance = CreateActorInstance(&currentMap->invisibleInstances, currentObjectName, instanceAttribs.name,position,scale,quat, actorFlags);
			}
			else
			{
				tempInstance = CreateActorInstance(&currentMap->levelInstances, currentObjectName, instanceAttribs.name,position,scale,quat, actorFlags);
			}

			if (CreateSpeedBoost(tempInstance, position, quat, instanceAttribs.speedboost, instanceAttribs.speedboostTarget, instanceAttribs.state, instanceAttribs.collision))
			{
				tempInstance->speedboostInfo->onBoost = GetTrigger("ONBOOST",node);
				if (!tempInstance->speedboostInfo->onBoost)
					tempInstance->speedboostInfo->onBoost = GetTrigger("ONBOOST",rootNode);
			}

			return PMENUM_CONTINUE;
		case SPECIALTYPE_BUMPER:
			bkPrintf("   SPECIALTYPE_BUMPER:%s\n",instanceAttribs.name);
			if(instanceAttribs.invisible)
			{
				tempInstance = CreateActorInstance(&currentMap->invisibleInstances, currentObjectName, instanceAttribs.name, position,scale,quat, actorFlags);
			}
			else
			{
				tempInstance = CreateActorInstance(&currentMap->animatedInstances, currentObjectName, instanceAttribs.name,position,scale,quat, actorFlags);
			}

			CreateBumper(tempInstance, instanceAttribs.speedboost);
			return PMENUM_CONTINUE;
		case SPECIALTYPE_TRIGGER:
			bkPrintf("   SPECIALTYPE_TRIGGER:%s\n",instanceAttribs.name);
			if (instanceAttribs.invisible)
			{
				tempInstance = CreateActorInstance(&currentMap->triggerInstances,currentObjectName,instanceAttribs.name,position,scale,quat, actorFlags);
			}
			else
			{
				tempInstance = CreateActorInstance(&currentMap->levelInstances,currentObjectName,instanceAttribs.name,position,scale,quat, actorFlags);
			}
			tempInstance->flags |= ACTORFLAG_ALWAYSCALLPRECOLLCALLBACK;

			AddActorToCollisionCache(currentMap->collisionCache,tempInstance,instanceAttribs.liveUpdate);
			SetPreCollisionCallback(tempInstance,TriggerCallback);

			if (CreateTriggerObjectInstance(tempInstance))
			{
				tempInstance->triggerObjectInfo->radius = METERTOUNIT(instanceAttribs.radius);

				tempInstance->triggerObjectInfo->onContact = GetTrigger("ONCONTACT",node);
				if (!tempInstance->triggerObjectInfo->onContact)
					tempInstance->triggerObjectInfo->onContact = GetTrigger("ONCONTACT",rootNode);

				tempInstance->triggerObjectInfo->enterRadius = GetTrigger("ENTERRADIUS",node);
				if (!tempInstance->triggerObjectInfo->enterRadius)
					tempInstance->triggerObjectInfo->enterRadius = GetTrigger("ENTERRADIUS",rootNode);

				tempInstance->triggerObjectInfo->exitRadius = GetTrigger("EXITRADIUS",node);
				if (!tempInstance->triggerObjectInfo->exitRadius)
					tempInstance->triggerObjectInfo->exitRadius = GetTrigger("EXITRADIUS",rootNode);
			}

			tempInstance->triggerObjectInfo->disabled = 1-instanceAttribs.state;	// TP: initial state for the trigger active or not

			return PMENUM_CONTINUE;
		case SPECIALTYPE_EFFECT:
			bkPrintf("   SPECIALTYPE_EFFECT:%s\n",instanceAttribs.name);
			SETVECTOR(tempVector, instanceAttribs.effectXSpeed, instanceAttribs.effectYSpeed, instanceAttribs.effectZSpeed, 1.0f);

			AddEntryToEffectsList(position, tempVector, instanceAttribs.effectPeriod, instanceAttribs.effect);
			return PMENUM_CONTINUE;
		case SPECIALTYPE_ICE:
			bkPrintf("   SPECIALTYPE_ICE: %s\n", instanceAttribs.effect);

			tempInstance = CreateActorInstance(&currentMap->levelInstances, currentObjectName, instanceAttribs.name,position,scale,quat, actorFlags);
			
			// TP: if it is an animated object then add to animated list
			if (tempInstance->actorInstance.numAnimSegments)
			{
				RemoveInstanceFromInstanceList(tempInstance,&currentMap->levelInstances);
				AddInstanceToInstanceList(tempInstance,&currentMap->animatedInstances);
			}

			if (instanceAttribs.invisible)
			{
				tempInstance->flags |= ACTORFLAG_DONTDRAW;
			}

			CreateIceInstance(tempInstance,instanceAttribs.modelName,instanceAttribs.liveUpdate);
			return PMENUM_CONTINUE;
		case SPECIALTYPE_RANDOMFOOD:
			bkPrintf("   SPECIALTYPE_RANDOMFOOD: %s\n", instanceAttribs.name);
			CreateRandomFood(position, instanceAttribs.foodType);				
			return PMENUM_CONTINUE;
		case SPECIALTYPE_VEHICLE:
			bkPrintf("   SPECIALTYPE_VEHICLE: %s\n", instanceAttribs.name);
			tempInstance = CreateActorInstance(&currentMap->levelInstances, currentObjectName, instanceAttribs.name,position,scale,quat, actorFlags);

			CreateVehicle(tempInstance, instanceAttribs.vehicleType);
			return PMENUM_CONTINUE;
		case SPECIALTYPE_SCENIC:
			bkPrintf("   SPECIALTYPE_SCENIC: %s\n", instanceAttribs.name);
			tempInstance = CreateActorInstance(&currentMap->animatedInstances, currentObjectName, instanceAttribs.name,position,scale,quat, actorFlags);

			CreateScenic(tempInstance, instanceAttribs.modelName, instanceAttribs.collision, instanceAttribs.deadFlockerCRC, instanceAttribs.laserSize, instanceAttribs.scenicType, instanceAttribs.liveUpdate,instanceAttribs.alwaysBounce);
			return PMENUM_CONTINUE;
		case SPECIALTYPE_ESCALATOR:
			bkPrintf("   SPECIALTYPE_ESCALATOR: %s\n", instanceAttribs.name);
			tempInstance = CreateActorInstance(&currentMap->levelInstances, currentObjectName, instanceAttribs.name,position,scale,quat, actorFlags);

			CreateEscalator(tempInstance, quat, instanceAttribs.speedboost);
			return PMENUM_CONTINUE;
#ifndef CONSUMERDEMO
		case SPECIALTYPE_SLIDE:
			bkPrintf("   SPECIALTYPE_SLIDE:%s\n", instanceAttribs.name);
			tempInstance = CreateActorInstance(&currentMap->levelInstances, currentObjectName, instanceAttribs.name,position,scale,quat, actorFlags);

			CreateSlide(tempInstance);
			return PMENUM_CONTINUE;
		case SPECIALTYPE_TRANSPORTER:
			bkPrintf("   SPECIALTYPE_TRANSPORTER: %s\n", instanceAttribs.name);
			if(instanceAttribs.invisible)
			{
				tempInstance = CreateActorInstance(&currentMap->invisibleInstances, currentObjectName, instanceAttribs.name, position,scale,quat, actorFlags);
			}
			else
			{
				tempInstance = CreateActorInstance(&currentMap->levelInstances, currentObjectName, instanceAttribs.name,position,scale,quat, actorFlags);
			}
			CreateTransporter(tempInstance, instanceAttribs.transportTo, instanceAttribs.name, instanceAttribs.transX, instanceAttribs.transY, instanceAttribs.transZ, 
				instanceAttribs.delay, instanceAttribs.state, instanceAttribs.collision, instanceAttribs.fadeUp, instanceAttribs.fadeDown);

			tempInstance->transporterInfo->onContact = GetTrigger("ONCONTACT",node);
			if (!tempInstance->transporterInfo->onContact)
				tempInstance->transporterInfo->onContact = GetTrigger("ONCONTACT",rootNode);

			tempInstance->transporterInfo->onReachingDestination = GetTrigger("ONREACHINGDEST", node);
			if(!tempInstance->transporterInfo->onReachingDestination)
				tempInstance->transporterInfo->onReachingDestination = GetTrigger("ONREACHINGDEST", rootNode);

			tempInstance->transporterInfo->onComplete = GetTrigger("ONCOMPLETE",node);
			if (!tempInstance->transporterInfo->onComplete)
				tempInstance->transporterInfo->onComplete = GetTrigger("ONCOMPLETE",rootNode);
			return PMENUM_CONTINUE;
		case SPECIALTYPE_MINECART:
			bkPrintf("   SPECIALTYPE_MINECART: %s\n", instanceAttribs.name);
			tempInstance = CreateActorInstance(&map.animatedInstances, currentObjectName, instanceAttribs.name, position, scale, quat, actorFlags);
			CreateMineCart(tempInstance, instanceAttribs.musicBoxInstanceCRC);
			return PMENUM_CONTINUE;
		case SPECIALTYPE_TUMBLEWEED:
			bkPrintf("   SPECIALTYPE_TUMBLEWEED: %s\n", instanceAttribs.name);
			tempInstance = CreateActorInstance(&map.animatedInstances, currentObjectName, instanceAttribs.name, position, scale, quat, actorFlags);
			CreateTumbleWeedInfo(tempInstance, instanceAttribs.animCRC, instanceAttribs.deadFlockerCRC);
			return PMENUM_CONTINUE;
#endif
		case SPECIALTYPE_WATERFALL:
			bkPrintf("   SPECIALTYPE_WATERFALL: %s\n", instanceAttribs.name);
			tempInstance = CreateActorInstance(&map.invisibleInstances, currentObjectName, instanceAttribs.name, position, scale, quat, actorFlags);
			CreateWaterfallInstance(tempInstance, instanceAttribs.deadFlockerCRC, instanceAttribs.flockerSprayCRC, instanceAttribs.triggerCRC);
			return PMENUM_CONTINUE;
		case SPECIALTYPE_FOUNTAIN:
			bkPrintf("   SPECIALTYPE_FOUNTAIN: %s\n", instanceAttribs.name);
			tempInstance = CreateActorInstance(&currentMap->invisibleInstances, currentObjectName, instanceAttribs.name, position,scale,quat, actorFlags);

			CreateFountain(tempInstance, instanceAttribs.fountainMaxHeight, instanceAttribs.fountainPeriod, instanceAttribs.fountainIdle);
			return PMENUM_CONTINUE;
		case SPECIALTYPE_SHATTER:
			bkPrintf("   SPECIALTYPE_SHATTER: %s\n", instanceAttribs.name);
			tempInstance = CreateActorInstance(&currentMap->animatedInstances, currentObjectName, instanceAttribs.name,position,scale,quat, actorFlags);

			CreateShatterObject(tempInstance, instanceAttribs.shatterType,instanceAttribs.destroy,instanceAttribs.triggerType,instanceAttribs.delay,instanceAttribs.idleAnimCRC,instanceAttribs.destroyAnimCRC, instanceAttribs.replacementCRC);

			tempInstance->shatterInfo->onShatter = GetTrigger("ONSHATTER",node);
			if (!tempInstance->shatterInfo->onShatter)
				tempInstance->shatterInfo->onShatter = GetTrigger("ONSHATTER",rootNode);

			return PMENUM_CONTINUE;

		case SPECIALTYPE_MONITOR:// PP: special type for computer monitors
			{
				bkPrintf("   SPECIALTYPE_MONITOR: %s\n", instanceAttribs.name);

				tempInstance = CreateActorInstance(&currentMap->levelInstances, currentObjectName, instanceAttribs.name, position,scale,quat, actorFlags);

				// PP: TEMP TEST
				/*tempInstance->actorInstance.orientation[X]=0.0f;
				tempInstance->actorInstance.orientation[Y]=0.0f;
				tempInstance->actorInstance.orientation[Z]=0.0f;
				tempInstance->actorInstance.orientation[W]=-1.0f;*/

				CreateMonitorObject(tempInstance);
			}
			return PMENUM_CONTINUE;
		case SPECIALTYPE_SAFEPOINT:
			bkPrintf("   SPECIALTYPE_SAFEPOINT: %s\n", instanceAttribs.name);
			tempInstance = CreateActorInstance(&currentMap->invisibleInstances, currentObjectName, instanceAttribs.name,position,scale,quat, actorFlags);
			tempInstance->special = SPECIALTYPE_SAFEPOINT;
			return PMENUM_CONTINUE;
		case SPECIALTYPE_MAGNET:
			bkPrintf("   SPECIALTYPE_SAFEPOINT: %s\n", instanceAttribs.name);
			tempInstance = CreateActorInstance(&currentMap->invisibleInstances, currentObjectName, instanceAttribs.name,position,scale,quat, actorFlags);

			CreateMagnetInstance(tempInstance, instanceAttribs.magnetMin, instanceAttribs.magnetMax);
			return PMENUM_CONTINUE;
#ifndef CONSUMERDEMO
		case SPECIALTYPE_FLOCKING:
			bkPrintf("   SPECIALTYPE_FLOCKING: %s\n", instanceAttribs.name);
			tempInstance = CreateActorInstance(&currentMap->invisibleInstances, currentObjectName, instanceAttribs.name,position,scale,quat, actorFlags);

#if 0
			tempInt = NumberOfTrisInActor(&tempInstance->actorInstance)*3;
#else
			tempInt = NumberOfVertsInActor(&tempInstance->actorInstance);
#endif
			CalculateBoundingBox(&boundingBox,&localBoundingBox,tempInstance,NULL,tempInt);
			CreateFlockers(tempInstance,&boundingBox,instanceAttribs.modelName,instanceAttribs.safetyRange,instanceAttribs.numFlockers,instanceAttribs.deadFlockerCRC,instanceAttribs.flockerSprayCRC);
			return PMENUM_CONTINUE;
		case SPECIALTYPE_BUZZARD:
			if (strcmpi(instanceAttribs.modelName,"beaky.obe"))
			{
				bkPrintf("   SPECIALTYPE_BUZZARD: %s\n", instanceAttribs.name);
				CreateActorInstanceBuzzard(CreateActorInstance(&currentMap->characterInstances, instanceAttribs.modelName, instanceAttribs.name,position,scale,quat, actorFlags));
			}
			return PMENUM_CONTINUE;
		case SPECIALTYPE_FECAMERA:
			bkPrintf("   SPECIALTYPE_FECAMERA: %s\n", instanceAttribs.name);
			tempInstance = CreateActorInstance(&currentMap->invisibleInstances, currentObjectName, instanceAttribs.name,position,scale,quat, actorFlags);

			StoreFrontEndCameraInfoFromMap(tempInstance, instanceAttribs.frontEndCamera, instanceAttribs.cameraLookAt);
			return PMENUM_CONTINUE;
#endif
		case SPECIALTYPE_SPLAT:
			bkPrintf("   SPECIALTYPE_SPLAT: %s\n", instanceAttribs.name);
			tempInstance = CreateActorInstance(&currentMap->invisibleInstances, currentObjectName, instanceAttribs.name,position,scale,quat, actorFlags);
			CreateSplatObject(tempInstance);
			return PMENUM_CONTINUE;
		case SPECIALTYPE_DEATH:
			bkPrintf("   SPECIALTYPE_DEATH: %s\n", instanceAttribs.name);
			if(CreateDeathObject(tempInstance = CreateActorInstance(&currentMap->invisibleInstances, currentObjectName, instanceAttribs.name,position,scale,quat,actorFlags)))
			{
				tempInstance->deathObjectInfo->resetInfo = GetWaterResetObjectsFromMap("resetObject", node);
				if(!tempInstance->deathObjectInfo->resetInfo)
				{
					tempInstance->deathObjectInfo->resetInfo = GetWaterResetObjectsFromMap("resetObject", rootNode);
				}
			}
			return PMENUM_CONTINUE;
		case SPECIALTYPE_HAAR:
			bkPrintf("   SPECIALTYPE_HAAR: %s\n", instanceAttribs.name);
			tempInstance = CreateActorInstance(&currentMap->invisibleInstances, currentObjectName, instanceAttribs.name, position,scale,quat,actorFlags);
			CreateHaarObject(tempInstance, instanceAttribs.density, (float)instanceAttribs.collectibleColour[0], (float)instanceAttribs.collectibleColour[1], (float)instanceAttribs.collectibleColour[2]);
			return PMENUM_CONTINUE;
		case SPECIALTYPE_PORTALWORLD:
			bkPrintf("   SPECIALTYPE_PORTAL(WORLD): %s\n", instanceAttribs.name);
			CreatePortalWorldInstance(tempInstance = CreateActorInstance(&map.levelInstances, currentObjectName, instanceAttribs.name, position, scale, quat, actorFlags));
			return PMENUM_CONTINUE;
		case SPECIALTYPE_MUSICBOX:
			bkPrintf("   SPECIALTYPE_MUSICBOX: %s\n", instanceAttribs.name);
			tempInstance = CreateActorInstance(&currentMap->animatedInstances, currentObjectName, instanceAttribs.name, position, scale, quat, actorFlags);
			CreateMusicBoxInstance(tempInstance, instanceAttribs.musicBoxRadius, instanceAttribs.musicBoxInstanceCRC, instanceAttribs.musicBoxState);
			return PMENUM_CONTINUE;
		case SPECIALTYPE_FLAMETHROWA:
			bkPrintf("   SPECIALTYPE_FLAMETHROWA: %s\n", instanceAttribs.name);
			tempInstance = CreateActorInstance(&currentMap->levelInstances, currentObjectName, instanceAttribs.name, position, scale, quat, actorFlags);
			CreateFlameThrowa(tempInstance, instanceAttribs.length, instanceAttribs.delay);
			return PMENUM_CONTINUE;
#ifndef CONSUMERDEMO
		case SPECIALTYPE_MECHATWEETY:
			bkPrintf("   SPECAILTYPE_MECHATWEETY: %s\n", instanceAttribs.name);
			tempInstance = CreateActorInstance(&map.invisibleInstances, currentObjectName, instanceAttribs.name, position, scale, quat, actorFlags);
			tempInstance->special = SPECIALTYPE_MECHATWEETY;
			return PMENUM_CONTINUE;
		case SPECIALTYPE_TAZBOSSITEM:
			bkPrintf("   SPECIALTYPE_TAZBOSSITEM: %s\n", instanceAttribs.name);
			tempInstance = CreateActorInstance(&map.levelInstances, currentObjectName, instanceAttribs.name, position, scale, quat, actorFlags);
			if(tempInstance->actorInstance.numAnimSegments)
			{
				RemoveInstanceFromInstanceList(tempInstance, &map.levelInstances);
				AddInstanceToInstanceList(tempInstance, &map.animatedInstances);
			}
			if(CreateTazBossItemInstance(tempInstance, instanceAttribs.itemType))
			{
				tempInstance->tazBossItemInfo->replacementCRC = instanceAttribs.replacementCRC;
				tempInstance->tazBossItemInfo->animCRC = instanceAttribs.animCRC;
				tempInstance->tazBossItemInfo->number = instanceAttribs.camNumber;
				InitialiseTazBossItemInstance(tempInstance);
				tempInstance->tazBossItemInfo->debris = GetDebrisObjectsFromMap("debrisObject", node);
				if(!tempInstance->tazBossItemInfo->debris)
					tempInstance->tazBossItemInfo->debris = GetDebrisObjectsFromMap("debrisObject", rootNode);
			}
			return PMENUM_CONTINUE;
#endif
		case SPECIALTYPE_TARBUBBLE:
			bkPrintf("   SPECIALTYPE_TARBUBBLE: %s\n", instanceAttribs.name);
			tempInstance = CreateActorInstance(&map.invisibleInstances, currentObjectName, instanceAttribs.name, position, scale, quat, actorFlags);
			if(tempInstance) CreateTarBubbleEmmiter(tempInstance, instanceAttribs.modelName, instanceAttribs.springyHeight, instanceAttribs.springyRadius, instanceAttribs.initState);
			return PMENUM_CONTINUE;
		case SPECIALTYPE_HALO:
			bkPrintf("   SPECIALTYPE_HALO: %s\n", instanceAttribs.name);
			tempInstance = CreateActorInstance(&map.levelInstances, currentObjectName, instanceAttribs.name, position, scale, quat, actorFlags);
			if(tempInstance) CreateHaloObject(tempInstance, instanceAttribs.maxRadius, instanceAttribs.snowType, instanceAttribs.deadFlockerCRC,
												instanceAttribs.haloColour[0], instanceAttribs.haloColour[1], instanceAttribs.haloColour[2]);
			return PMENUM_CONTINUE;
		case SPECIALTYPE_ZAPPER:
			bkPrintf("   SPECIALTYPE_ZAPPER: %s\n", instanceAttribs.name);
			tempInstance = CreateActorInstance(&map.levelInstances, currentObjectName, instanceAttribs.name, position, scale, quat, actorFlags);
			if(tempInstance) CreateZapper(tempInstance, instanceAttribs.zapperType);
			return PMENUM_CONTINUE;
		case SPECIALTYPE_AIRCON:
			bkPrintf("   SPECIALTYPE_AIRCON: %s\n", instanceAttribs.name);
			CreateAirconUnit(CreateActorInstance(&map.invisibleInstances, currentObjectName, instanceAttribs.name, position, scale, quat, actorFlags));
			return PMENUM_CONTINUE;
#if BPLATFORM==GAMECUBE
		case SPECIALTYPE_REVERB:
			bkPrintf("   SPECIALTYPE_REVERB: %s\n", instanceAttribs.name);
			tempInstance = CreateActorInstance(&map.invisibleInstances, currentObjectName, instanceAttribs.name, position, scale, quat, actorFlags);
			if(tempInstance)
			{
				tempInstance->soundEffectType = instanceAttribs.reverbType;
				tempInstance->special = SPECIALTYPE_REVERB;
			}
			return PMENUM_CONTINUE;
#endif
#ifndef CONSUMERDEMO
		case SPECIALTYPE_ARROW:
			bkPrintf("   SPECIALTYPE_ARROW: %s\n", instanceAttribs.name);
			CreateSafariArrow(CreateActorInstance(&map.levelInstances, currentObjectName, instanceAttribs.name, position, scale, quat, actorFlags));
			return PMENUM_CONTINUE;
		case SPECIALTYPE_MIRRORBALL:
			bkPrintf("   SPECIALTYPE_MIRRORBALL: %s\n", instanceAttribs.name);
			if(tempInstance = CreateActorInstance(&map.animatedInstances, currentObjectName, instanceAttribs.name, position, scale, quat, actorFlags))
			{
				tempInstance->special = SPECIALTYPE_MIRRORBALL;
			}
			return PMENUM_CONTINUE;
#endif
		}
	}

	if (instanceAttribs.collectible!=COLLECTIBLETYPE_NONE)
	{
		// create collectible
		if (instanceAttribs.collectible<ENDOFCOLLECTIBLETYPES)
		{
			bkPrintf("   COLLECTIBLE:%s\n",instanceAttribs.name);
			currentMap->drawnPolys += numPolysInObject;
//			currentMap->numberOfCollectibles+=1;

			switch(instanceAttribs.collectible)
			{
			case COLLECTIBLETYPE_SECURITYBOX:
				new SECURITYBOX(position,quat,instanceAttribs.triggerCRC,rootNode,node);
				break;

			case COLLECTIBLETYPE_SPINPAD:
				new SPINPAD(position,quat,actorFlags);
				break;

			case COLLECTIBLETYPE_SECRETITEM:
				if (!instanceAttribs.state)
				{
					DeactivateSecretItem();
				}
				else
				{
					ActivateSecretItem();
				}
				if (!gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].secretItemCollected)
					CreateCollectibleActor("yosamiteskull.obe",position,0,(int)(METERTOUNIT(0.5f)),instanceAttribs.collectible);
				break;

			case COLLECTIBLETYPE_BURPCAN:
				CreateCollectibleActor("burpcan.obe",position,0,(int)(METERTOUNIT(0.5f)),instanceAttribs.collectible);
				break;
				
			case COLLECTIBLETYPE_TESTTUBE:
				CreateCollectibleActor("testtube.obe",position,0,(int)(METERTOUNIT(0.5f)),instanceAttribs.collectible);
				break;

			case COLLECTIBLETYPE_HICCUP:
				CreateCollectibleActor("hiccupcan.obe",position,0,(int)(METERTOUNIT(0.5f)),instanceAttribs.collectible);
				break;

			case COLLECTIBLETYPE_BUBBLEGUM:
				CreateCollectibleActor("bubblegum.obe",position,0,(int)(METERTOUNIT(0.5f)),instanceAttribs.collectible);
				break;

			case COLLECTIBLETYPE_CHILLIPEPPER:
				CreateCollectibleActor("chillipepper.obe",position,0,(int)(METERTOUNIT(0.5f)),instanceAttribs.collectible);
				break;

			case COLLECTIBLETYPE_PICKUP:
				CreateCollectibleActor("clubsandwich.obe", position, 0, (int)(METERTOUNIT(0.7f)),instanceAttribs.collectible, false, map.mapStats.numCollectibles);
				break;

			default:
				bkPrintf("   *** WARNING *** Default collectible type: %s\n", instanceAttribs.name);
				// TP: no such thing as a default collectible anymore CreateCollectibleActor(currentObjectName,position,0,(int)(METERTOUNIT(0.5f)),instanceAttribs.collectible);
				break;
			}
		}
		else
		{
			bkPrintf("   *** WARNING *** Unknown collectible type: %s\n", instanceAttribs.name);
		}
		return PMENUM_CONTINUE;
	}

	if((instanceAttribs.invisible)||(instanceAttribs.visible==FALSE))
	{
		bkPrintf("   INVISIBLE:%s\n",instanceAttribs.name);
		tempInstance = CreateActorInstance(&currentMap->invisibleInstances,currentObjectName,instanceAttribs.name,position,scale,quat,actorFlags);
		if (instanceAttribs.collision)
		{
			bkPrintf("   %s\n",instanceAttribs.name);
			AddActorToCollisionCache(currentMap->collisionCache,tempInstance,instanceAttribs.liveUpdate);
		}
		else
		{
			bkPrintf("   NOCOLLISION:%s\n",instanceAttribs.name);
		}
		return PMENUM_CONTINUE;
	}

	// create standard object with collision
	currentMap->drawnPolys += numPolysInObject;
	tempInstance = CreateActorInstance(&currentMap->levelInstances,currentObjectName,instanceAttribs.name,position,scale,quat,actorFlags);
	
	if (instanceAttribs.collision)
	{
		bkStreamPrintf(&map.optimizeStream,"Instance \"%s\"[%s] \t\t could be MERGED with landscape.\n",instanceAttribs.name,currentObjectName);
		bkPrintf("   %s\n",instanceAttribs.name);
		AddActorToCollisionCache(currentMap->collisionCache,tempInstance,instanceAttribs.liveUpdate);
	}
	else
	{
		bkPrintf("   NOCOLLISION:%s\n",instanceAttribs.name);
	}

	// check all nodes on this branch
	return PMENUM_CONTINUE;
}

char	*searchType;

int ReceiveAttrib(CPMAttrib *attrib,void *context)
{
	if (strcmpi(attrib->ident,searchType)==0)
	{
		((int *)context)[0]++;
	}
	return PMENUM_CONTINUE;
}

int ReceiveAttribAndCreateTrigger(CPMAttrib *attrib,void *context)
{
	TRIGGERINFO *trigger;
	char	event[256],message[256];
	char	*temp;
	int		numChars;

	trigger = (TRIGGERINFO*)context;

	if (strcmpi(attrib->ident,searchType)==0)
	{
		temp = FindChar(attrib->string,':');
		if (temp==0) return PMENUM_CONTINUE;
		numChars = (temp - attrib->string);

		if (numChars==0) return PMENUM_CONTINUE;

		memcpy(event,attrib->string,numChars);
		event[numChars] = 0;

		temp++;
		if (strlen(temp) == 0)
		{
			//FREE(event);
			return PMENUM_CONTINUE;
		}
		//message = (char*)ZALLOC((strlen(temp))+1);
		/*if (message==NULL)
		{
			//FREE(event);
			return PMENUM_CONTINUE;
		}*/
		strcpy(message,temp);

		strlwr(event);
		strlwr(message);

		CreateTrigger(trigger, event, message);
		//AddTrigger(trigger,event,message);
	}
	return PMENUM_CONTINUE;
}

TRIGGERINFO *GetTrigger(char *type, CPMNode *node)
{
	int numberAttribs=0;
	TRIGGERINFO *triggerInfo;

	searchType = type;

	node->EnumAttribs(ReceiveAttrib,&numberAttribs);

	if (numberAttribs)
	{
		triggerInfo = CreateTriggerInfo(numberAttribs);

		node->EnumAttribs(ReceiveAttribAndCreateTrigger,triggerInfo);

		return triggerInfo;
	}
	return NULL;
}

/* --------------------------------------------------------------------------------
   Function : ReceiveWaterAttribAndStoreObject
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int ReceiveWaterAttribAndStoreObject(CPMAttrib *attrib,void *context)
{
	WATERRESETINFO *waterResetObject;
	char	*objectName;

	waterResetObject = (WATERRESETINFO*)context;

	if (strcmpi(attrib->ident,searchType)==0)
	{
		objectName = (char*)ZALLOC(strlen(attrib->string)+1);
		ASSERT(objectName);
		strcpy(objectName, attrib->string);

		AddWaterResetObject(waterResetObject, objectName);
	}
	return PMENUM_CONTINUE;
}

/* --------------------------------------------------------------------------------
   Function : GetWaterResetObjectsFromMap
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

WATERRESETINFO *GetWaterResetObjectsFromMap(char *type, CPMNode *node)
{
	WATERRESETINFO	*waterResetInfo;
	int				numberAttribs = 0;
	searchType = type;

	node->EnumAttribs(ReceiveAttrib, &numberAttribs);

	if(numberAttribs)
	{
		waterResetInfo = CreateWaterResetInfo(numberAttribs);

		node->EnumAttribs(ReceiveWaterAttribAndStoreObject, waterResetInfo);
		return waterResetInfo;
	}
	else
	{
		bkPrintf("      *** ERROR *** No reset points for water.\n");
	}
	return NULL;
}

/* --------------------------------------------------------------------------------
   Function : ReceiveElectricityPathAttribAndStoreObject
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int ReceiveElectricityPathAttribAndStoreObject(CPMAttrib *attrib,void *context)
{
	ELECTRICITYPATHINFO *pathNodeObject;
	uint32				objectName;

	pathNodeObject = (ELECTRICITYPATHINFO*)context;

	if (strcmpi(attrib->ident,searchType)==0)
	{
		objectName = bkCRC32((uchar*)attrib->string, strlen(attrib->string),0);

		AddElectricityPathObject(pathNodeObject, objectName);
	}
	return PMENUM_CONTINUE;
}

/* --------------------------------------------------------------------------------
   Function : GetElectricityPathNodesFromMap
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

ELECTRICITYPATHINFO *GetElectricityPathNodesFromMap(char *type, CPMNode *node)
{
	ELECTRICITYPATHINFO	*pathInfo;
	int					numberAttribs = 0;
	searchType = type;

	node->EnumAttribs(ReceiveAttrib, &numberAttribs);

	if(numberAttribs)
	{
		pathInfo = CreateElectricityPathInfo(numberAttribs);

		node->EnumAttribs(ReceiveElectricityPathAttribAndStoreObject, pathInfo);
		return pathInfo;
	}
	else
	{
		bkPrintf("      *** WARNING *** No paths nodes for electricity.\n");
	}
	return NULL;
}

/* --------------------------------------------------------------------------------
   Function : ReceiveSparkAttribAndStoreTexture
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int ReceiveSparkAttribAndStoreTexture(CPMAttrib *attrib,void *context)
{
	SPARKINFO	*sparkInfo;
	char		*objectName;

	sparkInfo = (SPARKINFO*)context;

	if (strcmpi(attrib->ident,searchType)==0)
	{
		if((int)sparkInfo->sparkClock < sparkInfo->numTextures)
		{
			sparkInfo->sparkTextureCRC[(int)sparkInfo->sparkClock] = bkCRC32((uchar*)attrib->string, strlen(attrib->string), 0);
			sparkInfo->sparkClock += 1.0f;
		}
	}
	return PMENUM_CONTINUE;
}

/* --------------------------------------------------------------------------------
   Function : GetSparkTexturesFromMap
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

SPARKINFO *GetSparkTexturesFromMap(char *type, CPMNode *node)
{
	SPARKINFO	*sparkInfo;
	int			numberAttribs = 0;
	searchType = type;

	node->EnumAttribs(ReceiveAttrib, &numberAttribs);

	if(numberAttribs)
	{
		sparkInfo = (SPARKINFO*)ZALLOC(sizeof(SPARKINFO));
		ASSERTM(sparkInfo, "could not create sparkInfo");
		if(sparkInfo)
		{
			sparkInfo->numTextures = numberAttribs;
			sparkInfo->sparkTextureCRC = (uint32*)ZALLOC(sizeof(uint32)*numberAttribs);
			ASSERTM(sparkInfo->sparkTextureCRC, "out of memory");
			sparkInfo->sparkTexture = (TBTexture**)ZALLOC(sizeof(TBTexture*)*numberAttribs);
			ASSERTM(sparkInfo->sparkTexture, "out of memory");
		}

		sparkInfo->sparkClock = 0.0f;
		node->EnumAttribs(ReceiveSparkAttribAndStoreTexture, sparkInfo);
		return sparkInfo;
	}
	return NULL;
}

/* --------------------------------------------------------------------------------
   Function : GetDebrisObjectsFromMap
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

DESTRUCTIBLEDEBRISINFO *GetDebrisObjectsFromMap(char *type, CPMNode *node)
{
	DESTRUCTIBLEDEBRISINFO	*debris;
	int				numberAttribs = 0;
	int				counter;
	searchType = type;

	node->EnumAttribs(ReceiveAttrib, &numberAttribs);

	if(numberAttribs)
	{
		debris = (DESTRUCTIBLEDEBRISINFO*)ZALLOC(sizeof(DESTRUCTIBLEDEBRISINFO));
		ASSERT(debris);
		debris->numDebris = 0;
		for(counter = 0; counter < MAX_BREAKABLE_DEBRIS; counter++)
		{
			debris->debrisCRC[counter] = 0;
		}

		node->EnumAttribs(ReceiveDebrisAttribAndStore, debris);
		return debris;
	}
	return NULL;
}

/* --------------------------------------------------------------------------------
   Function : ReceiveDebrisAttribAndStore
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int ReceiveDebrisAttribAndStore(CPMAttrib *attrib,void *context)
{
	DESTRUCTIBLEDEBRISINFO	*debris;
	int						counter;
	char					debrisName[256];

	debris = (DESTRUCTIBLEDEBRISINFO*)context;

	if (strcmpi(attrib->ident,searchType)==0)
	{
		for(counter = 0; counter < MAX_BREAKABLE_DEBRIS; counter++)
		{
			if(debris->debrisCRC[counter] == 0)
			{
				if(FindChar(attrib->string, '.'))
				{
					strlwr(attrib->string);
					debris->debrisCRC[counter] = bkCRC32((uchar*)attrib->string,strlen(attrib->string),0);
					debris->numDebris++;
					return PMENUM_CONTINUE;
				}
				else
				{
					sprintf(debrisName, "objects\\%s.obe", attrib->string);
					strlwr(debrisName);
					debris->debrisCRC[counter] = bkCRC32((uchar*)debrisName, strlen(debrisName),0);
					debris->numDebris++;
					return PMENUM_CONTINUE;
				}
			}
			else continue;
		}

	}
	return PMENUM_CONTINUE;
}

/* --------------------------------------------------------------------------------
   Function : collateObjectsNodes
   Purpose : node enumeration callback. collate all "Object" nodes in the level tree
   Parameters : node class pointer, context [int *]
   Returns : NENUM_CONTINUE - continue search, NENUM_END - terminate search, NENUM_ENDPATH - don't search below this node
   Info : 
*/

int collateObjectsNodes(CPMNode *node, void *context)
{
	char	*string;
	float	transVecs[3+4+4];
	TBActor	*currentActor;
	int		memory;

#if(BPLATFORM == XBOX)// PP: TEMP MILESTONE FIX: NEED THIS TO KEEP THE STREAMING MUSIC TICKING OVER
	// PP: from bdFlip...
	DirectSoundDoWork();
	bUpdateSound();
	bDecodeWMA();
#endif// PP: xbox

	// count this object node
	((int *)context)[0]++;

	bkPrintf("--------------------------------------\n");
	// get "Name" attribute from this string
	string = node->GetAttrib("NaMe");				// case insensitive
	strcpy(currentObject,string);
	strlwr(currentObject);
	sprintf(currentObjectName,"%s%s.obe",pathName,string);
	strlwr(currentObjectName);

	memory = bkHeapFreeSpace();
	// read attributes of bank object
	// load actor
	FillInAttributes(&objectAttribs,node);

	if (objectAttribs.specialType!=SPECIALTYPE_MAPPOINT)
	{
		if (!LoadActor(currentObjectName,pakName,currentMap->mapPackage)) return PMENUM_END_PATH;
		currentActor = FindActorInActorList(currentObjectName);
		numPolysInObject = 0;//NumberOfVertsInMesh(currentActor) /3;
		currentMap->meshPolys += numPolysInObject;
	}
	instanceCounter = 0;

	// ** We may now want to check for instances of this object, which
	// ** we know are children below this node.

	rootNode = node;

	// collate all instances of this object node
	node->EnumNodes(collateObjectInstances, transVecs, "Instance");

	memory = memory - bkHeapFreeSpace();

	if (currentObjectName)
	{
		bkStreamPrintf(&map.memoryStream,"Item %s \n %d kb\n\n",currentObjectName,memory/1024);
	}

	if (instanceCounter==0)
	{
		bkPrintf("   *** WARNING *** There are no instances of this object Bucky.\n",instanceCounter);
//		FreeActor(currentObjectName);
	}
	else
	{
		bkPrintf("   Number of instances:%d\n",instanceCounter);
	}

	// ** Since we know that no other "Object" nodes are below this one, we
	// ** can instruct the enumeration to end this hierachy path.

	// terminate this enumeration path
	return PMENUM_END_PATH;
}


/* --------------------------------------------------------------------------------
   Function : collateWorldNodes
   Purpose : node enumeration callback. collate all "Object" nodes in the level tree
   Parameters : node class pointer, context [int *]
   Returns : NENUM_CONTINUE - continue search, NENUM_END - terminate search, NENUM_ENDPATH - don't search below this node
   Info : 
*/
int collateWorldNodes(CPMNode *node, void *context)
{
	char			*string;
	char			temp[256];
	float			transVecs[3+4+3];
	TBVector		position,scale;
	TBVector		cloudVector;
	float			cloudSpeed,tempFloat;
	uint32			cloudType;
	int				cloudNumber, minAlpha, maxAlpha;
	TBQuaternion	quat;
	ACTORINSTANCE	*tempInstance;
	int				tempInt, i;

	// count this object node
	((int *)context)[0]++;

	// get "Name" attribute from this string
	string = node->GetAttrib("NaMe");				// case insensitive
	sprintf(currentObjectName,"%s.obe",string);
	strlwr(currentObjectName);
	// string == NULL - error!
	// get position vector from this node
	if (!node->GetAttrib("PositionX", transVecs[0]))
	{
		if (!node->GetAttrib("PosX", transVecs[0]))
			transVecs[0] = 0.f;
	}

	if (!node->GetAttrib("PositionY", transVecs[1]))
	{
		if (!node->GetAttrib("PosY", transVecs[1]))
			transVecs[1] = 0.f;
	}

	if (!node->GetAttrib("PositionZ", transVecs[2]))
	{
		if (!node->GetAttrib("PosZ", transVecs[2]))
			transVecs[2] = 0.f;
	}

	position[X] = transVecs[0];
	position[Y] = transVecs[1];
	position[Z] = transVecs[2];

	// get rotation vector from this node
	if (!node->GetAttrib("RotationX", transVecs[4]))
	{
		if (!node->GetAttrib("RotX", transVecs[4]))
			transVecs[4] = 0.f;
	}

	if (!node->GetAttrib("RotationY", transVecs[5]))
	{
		if (!node->GetAttrib("RotY", transVecs[5]))
			transVecs[5] = 0.f;
	}

	if (!node->GetAttrib("RotationZ", transVecs[6]))
	{
		if (!node->GetAttrib("RotZ", transVecs[6]))
			transVecs[6] = 0.f;
	}

	if (!node->GetAttrib("RotationW", transVecs[7]))
	{
		if (!node->GetAttrib("RotW", transVecs[7]))
			transVecs[7] = 0.f;
	}

	quat[X] = transVecs[4];
	quat[Y] = transVecs[5];
	quat[Z] = transVecs[6];
	quat[W] = transVecs[7];

	// get scale vector from this node
	if (!node->GetAttrib("ScaleX", transVecs[8]))
	{
		if (!node->GetAttrib("SclX", transVecs[8]))
			transVecs[8] = 0.f;
	}

	if (!node->GetAttrib("ScaleY", transVecs[9]))
	{
		if (!node->GetAttrib("SclY", transVecs[9]))
			transVecs[9] = 0.f;
	}

	if (!node->GetAttrib("ScaleZ", transVecs[10]))
	{
		if (!node->GetAttrib("SclZ", transVecs[10]))
			transVecs[10] = 0.f;
	}

	scale[X] = transVecs[8];
	scale[Y] = transVecs[9];
	scale[Z] = transVecs[10];

	// TP: find any map poster markers
	TRIGGERINFO	*tempTrigger;

	tempTrigger = GetTrigger("POSTER",node);
	
	if (tempTrigger)
	{
		for (i=0;i < tempTrigger->numMessages;i++)
		{
			int  number,x,y;

			number = atoi(tempTrigger->events[i]);
			// CMD: add one to move past underscore
			sscanf((tempTrigger->messages[i]+1),"%i %i",&x,&y);
			map.pauseMap->AddPoint(NULL,x,y,MAPFLAG_POSTER,number);
		}

		DeleteTriggerInfo(tempTrigger);
	}

	//ALLPOSTERSCOMPLETE = "1:293 223"
	tempTrigger = GetTrigger("ALLPOSTERSCOMPLETE",node);
	
	if (tempTrigger)
	{
		for (i=0;i < tempTrigger->numMessages;i++)
		{
			int  number,x,y;

			number = atoi(tempTrigger->events[i]);
			// CMD: add one to move past underscore
			sscanf((tempTrigger->messages[i]+1),"%i %i",&x,&y);
			map.pauseMap->AddPoint(NULL,x,y,MAPFLAG_LEVELCOMPLETE,number);
		}

		DeleteTriggerInfo(tempTrigger);
	}

	map.onLevelComplete = GetTrigger("ONLEVELCOMPLETION",node);

	if (node->GetAttrib("wantedposters",tempInt))
	{
		map.mapStats.numPosters = tempInt;
	}
	else
	{
		// TP:  default number of posters
		map.mapStats.numPosters = 5;
	}

	// TP: read all messages to be sent if posters have already been completed
	for (i=0;i<10;i++)
	{
		sprintf(temp, "poster%dcomplete",i+1);
		map.posterComplete[i] = GetTrigger(temp,node);
	}

	string = node->GetAttrib("background1");
	if (string)
	{
		sprintf(map.background1,"textures\\%s",string);
	}
	else
	{
		map.background1[0] = 0;
	}
	string = node->GetAttrib("background2");
	if (string)
	{
		sprintf(map.background2,"textures\\%s",string);
	}
	else
	{
		map.background2[0] = 0;
	}
	string = node->GetAttrib("background3");
	if (string)
	{
		sprintf(map.background3,"textures\\%s",string);
	}
	else
	{
		map.background3[0] = 0;
	}
	map.backgroundAngle = 0.0f;
	map.backgroundYOffset = 0.0f;
	if (node->GetAttrib("backgroundangle",tempFloat))
	{
		map.backgroundAngle = RAD(tempFloat);
	}
	else
	{
		if (node->GetAttrib("backgroundangle",tempInt))
		{
			map.backgroundAngle = RAD((float)tempInt);
		}
	}
	if (node->GetAttrib("backgroundyoffset",tempFloat))
	{
		map.backgroundYOffset = METERTOUNIT(tempFloat);
	}
	else
	{
		if (node->GetAttrib("backgroundyoffset",tempInt))
		{
			map.backgroundYOffset = METERTOUNIT(tempInt);
		}
	}
	string = node->GetAttrib("startposition");
	if (string)
	{
		// get start position
		if (map.startPos==0)
		{
			map.startPos = bkCRC32((uchar*)string,strlen(string),0);
		}
	}
	string = node->GetAttrib("startposition2");
	if (string)
	{
		// get start position
		if (map.startPos2==0)
		{
			map.startPos2 = bkCRC32((uchar*)string,strlen(string),0);
		}
	}
	if (node->GetAttrib("FOGMIN",tempFloat))
	{
		map.fogState = true;
		map.fogMinOrig = map.fogMin = METERTOUNIT(tempFloat);
	}
	else
	{
		if (node->GetAttrib("FOGMIN",tempInt))
		{
			map.fogState = true;
			map.fogMinOrig = map.fogMin = METERTOUNIT((float)tempInt);
		}
	}
	if (node->GetAttrib("FOGMAX",tempFloat))
	{
		map.fogState = true;
		map.fogMaxOrig = map.fogMax = METERTOUNIT(tempFloat);
	}
	else
	{
		if (node->GetAttrib("FOGMAX",tempInt))
		{
			map.fogState = true;
			map.fogMaxOrig = map.fogMax = METERTOUNIT((float)tempInt);
		}
	}
	string = node->GetAttrib("FOGRGB");
	if (string)
	{
		int r,g,b;
		sscanf(string,"%d %d %d",&r,&g,&b);
		map.fogColour.r = r;
		map.fogColour.g = g;
		map.fogColour.b = b;
		map.fogState = true;
	}
	// CMD: use fogMax == 0.0f to indicate that fog is not desired
	if(map.fogMax == 0.0f)
	{
		map.fogState = false;
		map.fogMinOrig = map.fogMin = METERTOUNIT(300.0f);
		map.fogMaxOrig = map.fogMax = METERTOUNIT(400.0f);
	}

	node->GetAttrib("cloudMinRadius", cloudVector[0]);
	cloudVector[0] = METERTOUNIT(cloudVector[0]);
	node->GetAttrib("cloudMaxRadius", cloudVector[1]);
	cloudVector[1] = METERTOUNIT(cloudVector[1]);
	node->GetAttrib("cloudMinHeight", cloudVector[2]);
	cloudVector[2] = METERTOUNIT(cloudVector[2]);
	node->GetAttrib("cloudMaxHeight", cloudVector[3]);
	cloudVector[3] = METERTOUNIT(cloudVector[3]);
	node->GetAttrib("cloudNumber", cloudNumber);
	node->GetAttrib("cloudSpeed", cloudSpeed);
	node->GetAttrib("minAlpha", minAlpha);
	node->GetAttrib("maxAlpha", maxAlpha);
	string = node->GetAttrib("cloudType");
	if(string)
	{
		string = strlwr(string);
		if(strcmp("wacky", string) == 0)
		{
			cloudType = SKYCLOUD_WACKY;
		}
		if(strcmp("fluffy", string) == 0)
		{
			cloudType = SKYCLOUD_FLUFFY;	
		} 
		CreateCloudsInSky(cloudVector, cloudNumber, RAD(cloudSpeed), maxAlpha, minAlpha, cloudType, gameStatus.player1, NULL, NULL);
	}

	// read attributes of bank object
	// load actor
	// check file tag
	bkPrintf("--------------------------------------\n");

	if (LoadSet(currentObjectName,pakName))
	{
		currentMap->landscape = (TBSet*)FindActorInActorList(currentObjectName);

		strcpy(temp,currentObjectName);
		string = FindChar(temp,'.');

		sprintf(string,"_coll.obe");
		if (LoadActor(temp,pakName,currentMap->mapPackage))
		{
			tempInstance = CreateActorInstance(&currentMap->collisionInstances,temp,"level");
			tempInt = NumberOfVertsInMesh(&tempInstance->actorInstance) /3;
			currentMap->meshPolys += tempInt;
			currentMap->drawnPolys += tempInt;
			bmVectorCopy(tempInstance->actorInstance.position,position);
			bmVectorCopy(tempInstance->actorInstance.scale,scale);
			bmQuatCopy(tempInstance->actorInstance.orientation,quat);
			baPrepActorInstance(&tempInstance->actorInstance,TRUE);
			AddActorToCollisionCache(currentMap->collisionCache,tempInstance,CACHEFLAGS_STAYRESIDENT|CACHEFLAGS_SUB);
			if (tempInstance->actorInstance.rootNodeInstance->children)
			{
				bkPrintf("   *** WARNING *** The World has NON-instance objects linked to it.\n");
			}
		}
		else
		{
			// TP: No separate actor for collision so collision info must be extracted from the set
			AddActorToCollisionCache(currentMap->collisionCache,currentMap->landscape,CACHEFLAGS_STAYRESIDENT|CACHEFLAGS_SUB);
		}

	}


	// terminate this enumeration path
	return PMENUM_END_PATH;
}

/*  --------------------------------------------------------------------------------
	Function	: SendPosterCompleteMessages
	Purpose		: Based on the posters complete this send messages to replicate the effect of them being destroyed etc.
	Parameters	: 
	Returns		: 
	Info		: 
*/

void SendPosterCompleteMessages()
{
	int i;
	TBVector pos;
	char message[256];

	for (i=0;i<10;i++)
	{
		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].posters[i])
		{
			// TP: poster i is complete
			if (map.posterComplete[i])
			{
				SendTrigger(map.posterComplete[i]);
			}
		}
		RespondToSoundMessages();
		RespondToSceneChangeMessages();
		RespondToTriggerMessages();
	}

	if((map.mapStats.numPosters)&&((gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].postersCollected) == map.mapStats.numPosters))
	{
		// TP: if there is a onlevelcomplete message then process that instead of doing level complete
		map.allPostersDestroyed = 1;

/*		if (map.onLevelComplete)
		{
			bmVectorCopy(pos,gameStatus.player[0].actorInstance->actorInstance.position);
			// send messages after final destruction anim
			sprintf(message,"_POS=%f %f %f",pos[X],pos[Y],pos[Z]);
			bkGenerateEvent("sound",message);
			SendTrigger(map.onLevelComplete);

			RespondToSoundMessages();
			RespondToSceneChangeMessages();
			RespondToTriggerMessages();
		}*/
	}
}

/* --------------------------------------------------------------------------------
   Function : LoadLOMFile
   Purpose : example of how load and use a .lom file
   Parameters : pointer to .lom file name
   Returns : 1 - okay, 0 - error
   Info : 
*/
int LoadLOMFile(char *filename)
{
	CPMLevel		*levelInst;
	int				noofObjs,noofWorlds;
	uchar			*lomFile;
	int				lomFileSize;
	ACTORINSTANCE	*ptr;
	PATH			*path;
	int				tempInt,tempInt2;

	sprintf(pakName,"%s",filename);
	sprintf(lomName,"%s.lom",filename);
	sprintf(pathName,"objects\\");

	bkStreamPrintf(&map.optimizeStream,"%s\n",pakName);
	bkStreamPrintf(&map.optimizeStream,"-------------------------------------\n");

	// IH: Gamecube only - load the lom file from a separate pak, so it can be deleted 
#if BPLATFORM==GAMECUBE
	// IH: TEMP: Test on safari_lom
	lomFilePak = OpenPackage("lomfiles");
	// IH: Check to see if the lom file package exists
	if( lomFilePak )
	{
		bkPrintf( "Loading Lom File from Lom file pak\n" );
		lomFile = bkLoadFile(lomFilePak, lomName, NULL, &lomFileSize, NULL, 0);
	}
	else
	{
		bkPrintf( "Loading Lom File from Level pak\n" );
		lomFile = bkLoadFile(currentMap->mapPackage,lomName,NULL,&lomFileSize,NULL,0);
	}
#else
	// ** Load our Level Object Map file. If succesful an pointer to an instance
	// ** of a level class is returned. This class essentially holds our map file
	// ** in a tree structure. Methods are supplied to enumerate the tree.
	// load our Level Object Map file. A pointer to the 
	lomFile = bkLoadFile(currentMap->mapPackage,lomName,NULL,&lomFileSize,NULL,0);
#endif

	if (lomFile==0)
	{
		bkPrintf("Could not load lom file\n");
		return FALSE;
	}
	if ((levelInst = pmLoadLevel((char*)lomFile,PMLOAD_MEMORY,lomFileSize)) == NULL)
	{
		bkPrintf("Could not perform pmLoadLevel\n");
		return FALSE;
	}

	// IH: The lom file has been parsed, so we can now free up its memory
#if BPLATFORM==GAMECUBE
	if(lomFilePak)
		SAFE_FREE(lomFile);
#endif

	// ** All access to the tree is done through enumerated callbacks. (If you prefer
	// ** to access the tree through loops, let me know and i'll add the required
	// ** methods for you.)
	levelInst->EnumNodes(collateWorldNodes, &noofWorlds, "World");

	// this methods is used to enumerate every "Object" node in our level tree
	// (if the last parameter is omitted, every node is visited)
	levelInst->EnumNodes(collateObjectsNodes, &noofObjs, "Object");

	// TP: Fixup paths
	for (ptr = map.levelInstances.head.next;ptr!=&map.levelInstances.head;ptr = ptr->next)
	{
		if ((ptr->pathInfo)&&(ptr->pathInfo->path==NULL))
		{
			path = FindPathInList(ptr->pathInfo->pathNameFixup,NULL);
			if (path)
			{
				tempInt = ptr->pathInfo->loopMode;
				tempInt2 = ptr->pathInfo->type;
				ResetPathInfo(ptr->pathInfo,path);
				GetPathPosition(ptr->pathInfo,ptr->actorInstance.position);
				GetPathOrientation(ptr->pathInfo,ptr->actorInstance.orientation);
				GetPathScale(ptr->pathInfo,ptr->actorInstance.scale);
				SetPathType(ptr->pathInfo,(EPathType)tempInt2);
				SetPathLoop(ptr->pathInfo,(EPathLoopMode)tempInt);
				if (tempInt==PATHLOOP_LOOP)
				{
					FollowPathClockwise(ptr->pathInfo);
				}
				else
				{
					if (tempInt==PATHLOOP_YOYO)
					{
						FollowPathClockwise(ptr->pathInfo);
					}
					else
					{
						FollowPathClockwise(ptr->pathInfo);
						ChangePathState(ptr->pathInfo,PATHSTATE_WAITING);
					}
				}
				if (tempInt2==PATHTYPE_SPIN)
				{
					SetPathType(ptr->pathInfo,PATHTYPE_SPIN);
				}
			}
			else
			{
				DeleteTriggerInfo(ptr->pathInfo->start);
				DeleteTriggerInfo(ptr->pathInfo->end);
				SAFE_FREE(ptr->pathInfo);
				ptr->special = (ESpecialType)-1;
				bkPrintf("*** ERROR *** Could not fixup PLATFORM %s\n",ptr->instanceName);
			}
		}
	}
	FindTexturesFromMapUsedBySparkingObjects();
	FindTexturesFromMapUsedByTarBubbleObjects();
	FindSwingingModelsFromMap(&map.invisibleInstances);
	PreCacheMusicBoxBoundaryInfo(&map.animatedInstances);
	FixUpWaterRingTextures(&map.invisibleInstances);

	// delete our level instance
	pmDeleteLevel(levelInst);

	bkStreamPrintf(&map.optimizeStream,"---------------------------- STATS -----------------------\n");
	for (int i=0;i<NUM_SPECIAL_TYPES;i++)
	{
		bkStreamPrintf(&map.optimizeStream,"SPECIALTYPE_%s\t\t	 no:%d\n", specialTypeNames[i], stCounters[i]);
	}
	bkStreamPrintf(&map.optimizeStream,"\n \n \n");
#if BPLATFORM==PS2
	bkFlushDebugStreamToFile(&map.optimizeStream);
	bkFlushDebugStreamToFile(&map.memoryStream);
#endif
	return 1;
}

/* --------------------------------------------------------------------------------
   Function : FillInAttributes
   Purpose : read known attributes from node and alter in attribute structure
   Parameters : pointer to node
   Returns : 
   Info : 
*/
void FillInAttributes(MAPOBJECTATTRIBS *attribs,CPMNode *node)
{
	char	*string;

	// default values
	string = node->GetAttrib("name");
	sprintf(attribs->name,string);
	attribs->specialType = -1;
	attribs->collectible = COLLECTIBLETYPE_NONE;
	attribs->collectibleColour[0] = 127;
	attribs->collectibleColour[0] = 64;
	attribs->collectibleColour[0] = 0;
	attribs->haloColour[0] = 127;
	attribs->haloColour[1] = 127;
	attribs->haloColour[2] = 127;
	attribs->respawn = TRUE;
	attribs->destructible = DESTRUCTIBLETYPE_SPIN;
	attribs->invisible = FALSE;
	attribs->steppingStoneHeight = 0.0f;
	attribs->bobbleTime = 1.0f;
	attribs->bobbleHeight = METERTOUNIT(0.1f);
	attribs->sinkAcceleration = METERTOUNIT(-0.5f);
	attribs->projectAngle = 0.0f;
	attribs->projectTo = NULL;
	attribs->contact = FALSE;
	attribs->cannonEffect = DEFAULT_CANNON_EFFECT;			// PP: initialis projectile cannon effect to default
	attribs->springyHeight = 0;
	attribs->springyRadius = 0;
	attribs->alwaysBounce = FALSE;							// TP: also used by scenics
	attribs->xAngleMax = RAD(60.0f);
	attribs->zAngleMax = RAD(60.0f);
	attribs->maxAcceleration = RAD(30.0f);
	attribs->maxRadius = METERTOUNIT(2.0f);
	attribs->retardationForce = 0.0f;
	attribs->length = 3.0f;
	attribs->model = -1;
	attribs->modelName[0] = 0;
	attribs->fallingType = FALLINGTYPE_ONCE;
	attribs->impactType = FALLINGTYPE_DESTROY;
	attribs->fallingRadius = 0.0f;
	attribs->fallingPeriod = 1;
	attribs->numWaterResetPoints = 1;
	attribs->waterType = WATERTYPE_WATER;
	attribs->visible = TRUE;
	attribs->collision = TRUE;
	attribs->liveUpdate = FALSE;
	attribs->fog = true;
	attribs->posterNumber = -1;
	attribs->posterSpecialCase = -1;
	attribs->mailboxNumber = -1;
	attribs->voltageState = HIGHVOLTAGE_OFF;
	attribs->initialVoltageState = 0;
	attribs->hits = 1;
	attribs->idle = 0;
	attribs->firstHit = 0;
	attribs->secondHit = 0;
	attribs->destroyHit = 0;
	attribs->effect = NULL;
	attribs->effectPeriod = 0.0f;
	attribs->effectXSpeed = 0.0f;
	attribs->effectYSpeed = 0.0f;
	attribs->effectZSpeed = 0.0f;
	attribs->radius = 0.0f;
	attribs->caseTime = 0.0f;
	attribs->snowDensity = 0;
	attribs->snowRate = 0.0f;
	attribs->snowType = DEBRISFLAG_SNOW;
	attribs->snowDim = 0.0f;
	attribs->electricityAccuracy = METERTOUNIT(0.5f);
	attribs->state = 1;// TP: note, I use this for the triggers and enemies also
	attribs->fireSize = METERTOUNIT(1.0f);
	attribs->transportTo = 0;
	attribs->transX = 0.0f;
	attribs->transY = 0.0f;
	attribs->transZ = 0.0f;
	attribs->delay = 0.0f;
	attribs->speedboost = 0.0f;
	attribs->speedboostTarget = 0;
	attribs->foodType = -1;
	attribs->vehicleType = -1;
	attribs->cameraFollow = NULL;
	attribs->cameraLookAt = NULL;
	attribs->cameraLookAtNode = NULL;
	attribs->cameraFollowNode = NULL;
	attribs->cameraStartDelay = 0.0f;
	attribs->transitionControl = false;
	attribs->doorSpringyness = 0.0f;
	attribs->doorStiffness = 0.0f;
	attribs->doorType = -1;
	attribs->laserSize = METERTOUNIT(0.1f);
	attribs->laserTexture[0] = 0;
	attribs->fountainMaxHeight = 0.0f;
	attribs->fountainPeriod = 0.0f;
	attribs->fountainIdle = TRUE;
	attribs->platformMode = PATHTYPE_NORMAL;
	attribs->platformCameraControl = false;
	attribs->triggerType = SHATTERTYPE_DEFAULT;
	attribs->shatterType = POLYFX_EXPLODE;
	attribs->destroy = TRUE;
	attribs->money = -1;
	attribs->idleAnimCRC = 0;
	attribs->destroyAnimCRC = 0;
	attribs->footstep = 0;
	attribs->componentDebris = FALSE;
	attribs->platformCRC = 0;
	attribs->magnetMax = 0.0f;
	attribs->magnetMin = 0.0f;
	attribs->safetyRange = METERTOUNIT(10.0f);
	attribs->numFlockers = 0;
	attribs->flockerSprayCRC = 0;
	attribs->deadFlockerCRC = 0;
	attribs->frontEndCamera = -1;
	attribs->flags = 0;
	attribs->fade = TRUE;
	attribs->triggerCRC = 0;
	attribs->density = 1.0f;
	attribs->possnap = -1;
	attribs->lookatsnap = -1;
	attribs->possnapback = -1;
	attribs->lookatsnapback = -1;
	attribs->smooth = false;
	attribs->itemType = (ETazBossItem)-1;
	attribs->sparkTex = 0;
	attribs->musicBoxRadius = 0.0f;
	attribs->musicBoxInstanceCRC = 0;
	attribs->musicBoxState = TRUE;
	attribs->useBox = false;
	attribs->zapperType = ZAPPER_NONE;
	attribs->fadeUp = FALSE;
	attribs->fadeDown = FALSE;
	attribs->cageCRC = 0;
	attribs->initState = 1;
	attribs->returnSpeed = 1.0f;
	attribs->scenicType = SCENICTYPE_NONE;
#if BPLATFORM==GAMECUBE
	attribs->reverbType = ESFX_NO_EFFECTS;
#endif

	ChangeAttributes(attribs,node);
}

/* --------------------------------------------------------------------------------
   Function : ChangeAttributes
   Purpose : alter existing attributes based on new node
   Parameters : pointer to node
   Returns : 
   Info : 
*/
void ChangeAttributes(MAPOBJECTATTRIBS *attribs, CPMNode *node)
{
	char	*string;
	char	*tempString;
	char	anotherString[256];
	bool	value;
	float	tempFloat;
	int		tempInt;
	char	tempChar[1];
	
	tempChar[0] = NULL;

	string = node->GetAttrib("name");
	string = strlwr(string);
	sprintf(attribs->name,string);

	if (node->GetAttrib("FADE", value))
	{
		attribs->fade = value;
	}
	if (node->GetAttrib("FOG", value))
	{
		attribs->fog = value;
	}
	// SPECIAL TYPE
	string = node->GetAttrib("specialtype");
	if ((!string)&&(attribs->specialType !=-1)) string = tempChar;

	if ((string)||(attribs->specialType !=-1))
	{
		if(strcmpi(string,"NONE")==0)
		{
			attribs->specialType = -1;
		}

		if(strcmpi(string,"SECRETITEM")==0)
		{
			attribs->collectible = COLLECTIBLETYPE_SECRETITEM;

			if (node->GetAttrib("active",value))
			{
				attribs->state = value;
			}
		}

		if (((strcmpi(string, "XDOOR")==0) || (attribs->specialType == SPECIALTYPE_XDOOR)))
		{
			attribs->specialType = SPECIALTYPE_XDOOR;

			if (tempString = node->GetAttrib("DESTDOOR"))
			{
				attribs->triggerCRC = bkCRC32((uchar*)tempString,strlen(tempString),0);
			}
		}

		if(((strcmpi(string, "LAVA")==0) || (attribs->specialType == SPECIALTYPE_LAVA)))
		{
			attribs->specialType = SPECIALTYPE_LAVA;
		
			if (node->GetAttrib("SMOKE",value))
			{
				attribs->flags = value;
			}
		}
		if((strcmpi(string, "LAVASPIT") == 0)||(attribs->specialType == SPECIALTYPE_LAVASPIT))
		{
			attribs->specialType = SPECIALTYPE_LAVASPIT;
		}

		if ((strcmpi(string,"SECURITYBOX")==0)||(attribs->collectible == COLLECTIBLETYPE_SECURITYBOX))
		{
			attribs->collectible = COLLECTIBLETYPE_SECURITYBOX;
			attribs->specialType = SPECIALTYPE_SECURITYBOX;
			if (tempString = node->GetAttrib("triggerbox"))
			{
				attribs->triggerCRC = bkCRC32((uchar*)tempString,strlen(tempString),0);
			}
		}

		if (strcmpi(string,"SPINPAD")==0)
		{
			attribs->collectible = COLLECTIBLETYPE_SPINPAD;
		}

//#ifdef CHRIS
		if(((strcmpi(string, "CHECKPOINT")==0) || (attribs->specialType == SPECIALTYPE_CHECKPOINT)))
//#else
//		if(((strcmpi(string, "CHECKPOINT")==0) || (attribs->specialType == SPECIALTYPE_CHECKPOINT)) && (gameStatus.multiplayer.numPlayers>1))
//#endif
		{
			attribs->specialType = SPECIALTYPE_CHECKPOINT;

			if(node->GetAttrib("sequence",tempInt))
			{
				attribs->sequence = tempInt;
			}

			if(node->GetAttrib("width",tempInt))
			{
				attribs->width = METERTOUNIT((float)tempInt);
			}
			else
			{
				attribs->width = METERTOUNIT(2.0f);
			}

		}

#ifdef CHRIS
		if(((strcmpi(string, "TARGET")==0) || (attribs->specialType == SPECIALTYPE_TARGET)))
#else
		if(((strcmpi(string, "TARGET")==0) || (attribs->specialType == SPECIALTYPE_TARGET)) && (gameStatus.multiplayer.numPlayers>1))
#endif
		{
			attribs->specialType = SPECIALTYPE_TARGET;
			if(node->GetAttrib("sequence",tempInt))
			{
				attribs->sequence = tempInt;
			}
		}

		if((strcmpi(string, "ZOOKEEPER")==0)||(attribs->specialType == SPECIALTYPE_ZOOKEEPER))
		{
			attribs->specialType = SPECIALTYPE_ZOOKEEPER;
			if (node->GetAttrib("radius",tempFloat))
			{
				attribs->radius = tempFloat;
			}
			else
			{
				if (node->GetAttrib("radius",tempInt))
				{
					attribs->radius = (float)tempInt;
				}
			}
		}
		if ((strcmpi(string, "ENEMY")==0)||(attribs->specialType == SPECIALTYPE_ENEMY))
		{
			attribs->specialType = SPECIALTYPE_ENEMY;
			if (tempString = node->GetAttrib("type"))
			{
				strcpy(attribs->modelName,tempString);

				if(strcmpi(tempString, "beaky.obe") == 0)
				{
					attribs->specialType = SPECIALTYPE_BUZZARD;
					return;
				}
				else
				if(strcmpi(tempString, "parrot.obe") == 0)
				{
					attribs->specialType = SPECIALTYPE_BUZZARD;
					return;
				}
			}
			else
			{
				attribs->modelName[0] = 0;
			}

			if (node->GetAttrib("BOX",value))
			{
				attribs->useBox = value;
			}

			if (node->GetAttrib("ACTIVE",value))
			{
				attribs->state = value;
			}

			if (tempString = node->GetAttrib("cage"))
			{
				attribs->cageCRC = bkCRC32((uchar*)tempString,strlen(tempString),0);
			}

			if (tempString = node->GetAttrib("attackpoint"))
			{
				attribs->triggerCRC = bkCRC32((uchar*)tempString,strlen(tempString),0);
			}

			if (node->GetAttrib("mode",tempInt))
			{
				attribs->flags = tempInt;
			}
		}
		if ((strcmpi(string, "PHONEBOX")==0)||(attribs->specialType == SPECIALTYPE_PHONEBOX))
		{	
			attribs->specialType = SPECIALTYPE_PHONEBOX;
		}
		if ((strcmpi(string, "POLICEBOX")==0)||(attribs->specialType == SPECIALTYPE_POLICEBOX))
		{
			attribs->specialType = SPECIALTYPE_POLICEBOX;

			tempString = node->GetAttrib("type");
			if (tempString)
			{
				strcpy(attribs->modelName,tempString);
			}
		}
		if ((strcmpi(string, "MONITOR")==0)||(attribs->specialType == SPECIALTYPE_MONITOR))
		{	
			attribs->specialType = SPECIALTYPE_MONITOR;

			// PP: read in any other info here
		}
		if ((strcmpi(string, "CAMERACASE")==0)||(attribs->specialType == SPECIALTYPE_CAMERACASE))
		{
			attribs->specialType = SPECIALTYPE_CAMERACASE;
			tempString = node->GetAttrib("type");

			if (tempString)
			{
				if (strcmpi(tempString,"FIXEDPOINT")==0)
				{
					attribs->caseType = CAMERACASETYPE_FIXEDPOINT;
				}
				if (strcmpi(tempString,"ROTATEAROUND")==0)
				{
					attribs->caseType = CAMERACASETYPE_ROTATEAROUND;
				}
				if (strcmpi(tempString,"PAN")==0)
				{
					attribs->caseType = CAMERACASETYPE_PAN;
				}

				if (node->GetAttrib("smooth",value))
				{
					attribs->smooth = value;
				}

				if (node->GetAttrib("possnap",value))
				{
					attribs->possnap = value;
				}
				if (node->GetAttrib("lookatsnap",value))
				{
					attribs->lookatsnap = value;
				}
				if (node->GetAttrib("lookatsnapback",value))
				{
					attribs->lookatsnapback = value;
				}
				if (node->GetAttrib("possnapback",value))
				{
					attribs->possnapback = value;
				}
				if (node->GetAttrib("transitioncontrol"),value)
				{
					attribs->transitionControl = value;
				}
				if (node->GetAttrib("transitioncontrol"),value)
				{
					attribs->transitionControl = value;
				}
				if (node->GetAttrib("startdelay",tempFloat))
				{
					attribs->cameraStartDelay = tempFloat;
				}
				else
				{
					if (node->GetAttrib("startdelay",tempInt))
					{
						attribs->cameraStartDelay = (float)tempInt;
					}
				}
					
				tempString = node->GetAttrib("cameradest");
				if (tempString)
				{
					strlwr(tempString);
					attribs->cameraDest = bkCRC32((uchar*)tempString,strlen(tempString),0);
				}
				tempString = node->GetAttrib("camerafollow");
				if (tempString)
				{
					strlwr(tempString);
					attribs->cameraFollow = bkCRC32((uchar*)tempString,strlen(tempString),0);
				}
				tempString = node->GetAttrib("cameralookat");
				if (tempString)
				{
					strlwr(tempString);
					attribs->cameraLookAt = bkCRC32((uchar*)tempString,strlen(tempString),0);
				}
				tempString = node->GetAttrib("cameralookatnode");
				if (tempString)
				{
					strlwr(tempString);
					attribs->cameraLookAtNode = bkCRC32((uchar*)tempString,strlen(tempString),0);
				}
				tempString = node->GetAttrib("camerafollownode");
				if (tempString)
				{
					strlwr(tempString);
					attribs->cameraFollowNode = bkCRC32((uchar*)tempString,strlen(tempString),0);
				}
				if (node->GetAttrib("time",tempFloat))
				{
					attribs->caseTime = tempFloat;
				}
				else
				{
					if (node->GetAttrib("time",tempInt))
					{
						attribs->caseTime = (float)tempInt;
					}
				}
				if (node->GetAttrib("radius",tempFloat))
				{
					attribs->radius = tempFloat;
				}
				else
				{
					if (node->GetAttrib("radius",tempInt))
					{
						attribs->radius = (float)tempInt;
					}
				}
				if (node->GetAttrib("angle",tempFloat))
				{
					attribs->angle = tempFloat;
				}
				else
				{
					if (node->GetAttrib("angle",tempInt))
					{
						attribs->angle = (float)tempInt;
					}
				}
				tempString = node->GetAttrib("holdtazat");
				if (tempString)
				{
					attribs->triggerCRC = bkCRC32((uchar*)tempString,strlen(tempString),0);
				}
			}
		}
		if ((strcmpi(string, "TRIGGER")==0)||(attribs->specialType==SPECIALTYPE_TRIGGER))
		{
			attribs->invisible = TRUE;
			attribs->collision = FALSE;
			attribs->specialType = SPECIALTYPE_TRIGGER;
			if (node->GetAttrib("radius",tempFloat))
			{
				attribs->radius = tempFloat;
			}
			else
			{
				if (node->GetAttrib("radius",tempInt))
				{
					attribs->radius = (float)tempInt;
				}
			}
			if (node->GetAttrib("active",value))
			{
				attribs->state = value;
			}
		}
		if ((strcmpi(string, "MAPPOINT")==0)||(attribs->specialType == SPECIALTYPE_MAPPOINT))
		{
			attribs->specialType = SPECIALTYPE_MAPPOINT;
		}
		if ((strcmpi(string, "MIRROR")==0)||(attribs->specialType == SPECIALTYPE_MIRROR))
		{
			attribs->specialType = SPECIALTYPE_MIRROR;
		}
		if((strcmpi(string, "SPEEDBOOST") == 0)||(attribs->specialType == SPECIALTYPE_SPEEDBOOST))
		{
			attribs->specialType = SPECIALTYPE_SPEEDBOOST;
			if(node->GetAttrib("speedboost", tempFloat))
			{
				attribs->speedboost = METERTOUNIT(tempFloat);
			}
			else
			{
				if(node->GetAttrib("speedboost", tempInt))
				{
					attribs->speedboost = METERTOUNIT((float)tempInt);
				}
			}

			if (node->GetAttrib("active",value))
			{
				attribs->state = value;
			}
		}
		if((strcmpi(string, "BUMPER") == 0)||(attribs->specialType == SPECIALTYPE_BUMPER))
		{
			attribs->specialType = SPECIALTYPE_BUMPER;
			if(node->GetAttrib("speedboost", tempFloat))
			{
				attribs->speedboost = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("speedboost", tempInt))
				{
					attribs->speedboost = METERTOUNIT((float)tempInt);
				}
		}
		if((strcmpi(string, "ESCALATOR") == 0)||(attribs->specialType == SPECIALTYPE_ESCALATOR))
		{
			attribs->specialType = SPECIALTYPE_ESCALATOR;
			if(node->GetAttrib("speed", tempFloat))
			{
				attribs->speedboost = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("speed", tempInt))
				{
					attribs->speedboost = (float)tempInt;
				}
		}
		if((strcmpi(string, "WATERRING") == 0)||(attribs->specialType == SPECIALTYPE_WATERRIPPLE))
		{
			attribs->specialType = SPECIALTYPE_WATERRIPPLE;
			if(node->GetAttrib("maxsize", tempFloat))
			{
				attribs->speedboost = tempFloat;		// CMD: no METERTOUNIT conversion, this is a scale!
			}
			else
				if(node->GetAttrib("maxsize", tempInt))
				{
					attribs->speedboost = (float)tempInt;
				}
			if(node->GetAttrib("minsize", tempFloat))
			{
				attribs->projectAngle = tempFloat;		// CMD: no METERTOUNIT conversion, this is a scale!
			}
			else
				if(node->GetAttrib("minsize", tempInt))
				{
					attribs->projectAngle = (float)tempInt;
				}
			if(tempString = node->GetAttrib("texture"))
			{
				attribs->deadFlockerCRC = bkCRC32((uchar*)tempString, strlen(tempString), 0);
			}
			if(node->GetAttrib("rotation", tempFloat))
			{
				attribs->delay = RAD(tempFloat);
			}
			else
				if(node->GetAttrib("rotation", tempInt))
				{
					attribs->delay = RAD((float)tempInt);
				}
		}

		if ((strcmpi(string, "PATH")==0)||(attribs->specialType == SPECIALTYPE_PATH))
		{
			// found path
			attribs->specialType = SPECIALTYPE_PATH;
		}
		if((strcmpi(string, "PORTAL") == 0)||(attribs->specialType == SPECIALTYPE_PORTALWORLD))
		{
			attribs->specialType = SPECIALTYPE_PORTALWORLD;
		}
		if ((strcmpi(string, "PLATFORM")==0)||(attribs->specialType == SPECIALTYPE_PLATFORM))
		{
			// found path
			attribs->specialType = SPECIALTYPE_PLATFORM;
			if ((tempString = node->GetAttrib("type"))!=NULL)
			{
				if (strcmpi(tempString,"LOOP")==0)
				{
					attribs->platformType = PATHLOOP_LOOP;
				}
				else
				{
					if (strcmpi(tempString,"NONE")==0)
					{
						attribs->platformType = PATHLOOP_NONE;
					}
					else
					{
						if (strcmpi(tempString,"YOYO")==0)
						{
							attribs->platformType = PATHLOOP_YOYO;
						}
						else
						{
							if (strcmpi(tempString,"SPIN")==0)
							{
								attribs->platformType = PATHTYPE_SPIN;
							}
							else
							{
								if (strcmpi(tempString,"RESET")==0)
								{
									attribs->platformType = PATHLOOP_RESET;
								}
							}
						}
					}
				}
			}
			if ((tempString = node->GetAttrib("mode"))!=NULL)
			{
				if (strcmpi(tempString,"SPIN")==0)
				{
					attribs->platformMode = PATHTYPE_SPIN;
				}
				else
				{
					if (strcmpi(tempString,"SQUASH")==0)
					{
						attribs->platformMode = PATHTYPE_SQUASH;
					}
					else
					{
						attribs->platformMode = PATHTYPE_NORMAL;
					}
				}
			}
			if (node->GetAttrib("CAMERACONTROL", value))
			{
				attribs->platformCameraControl = value;
			}
			if (node->GetAttrib("CAMERADIST", tempFloat))
			{
				attribs->radius = METERTOUNIT(tempFloat);
			}
			else
			{
				if (node->GetAttrib("CAMERADIST", tempInt))
				{
					attribs->radius = METERTOUNIT((float)tempInt);
				}
			}
			if(node->GetAttrib("RETURNSPEED", tempFloat))
			{
				attribs->returnSpeed = tempFloat;
			}
		}

		if(((strcmpi(string, "PROJECTILE") ==0)||(attribs->specialType == SPECIALTYPE_PROJECTILE))
			||(strcmpi(string, "PROJECTILESLIDE") ==0)||(attribs->specialType == SPECIALTYPE_PROJECTILESLIDE))
		{
			if ((strcmpi(string, "PROJECTILE") ==0)||(attribs->specialType == SPECIALTYPE_PROJECTILE))
			{
				attribs->specialType = SPECIALTYPE_PROJECTILE;
			}
			else
			{
				attribs->specialType = SPECIALTYPE_PROJECTILESLIDE;
			}
			if(node->GetAttrib("angle", tempFloat))
			{
				attribs->projectAngle = RAD(tempFloat);
			}
			else
				if(node->GetAttrib("angle", tempInt))
				{
					attribs->projectAngle = RAD((float)tempInt);
				}
			if((tempString = node->GetAttrib("landingPoint")) != NULL) 
			{
				attribs->projectTo = bkCRC32((uchar*)tempString, strlen(tempString), 0);
			}
			tempString = node->GetAttrib("type");
			if(tempString)
			{
				if(strcmpi(tempString,"CONTACT") == 0)
				{
					attribs->contact = TRUE;
				}
			}
			if (node->GetAttrib("active",value))
			{
				attribs->state = value;
			}
			if (node->GetAttrib("DONTMATCHORIGINS",value))
			{
				if (value)
				{
					attribs->flags |= CANNONFLAG_NOSNAPTOORIGIN;
				}
				else
				{
					attribs->flags &= ~CANNONFLAG_NOSNAPTOORIGIN;
				}
			}
			if (node->GetAttrib("CENTERORIGIN",value))
			{
				// TP: on by default
				if (value==false)
				{
					attribs->flags |= CANNONFLAG_NOCENTERORIGIN;
				}
				else
				{
					attribs->flags &= ~CANNONFLAG_NOCENTERORIGIN;
				}
			}
			if (node->GetAttrib("FIXEDPATH", value))
			{
				if (value==true)
				{
					attribs->flags |= CANNONFLAG_FIXEDPATH;
				}
				else
				{
					attribs->flags &= ~CANNONFLAG_FIXEDPATH;
				}
			}

			// PP: read-in effect type...

			if((tempString=node->GetAttrib("effect")) != NULL)
			{
				// PP: get the index of the effect.  If not found, -1 will be returned, which conweniently equals CANNONEFFECT_NONE
				attribs->cannonEffect=utilFindStringInArray(cannonEffectNames, NUM_CANNON_EFFECTS, tempString);

				// PP: if the type name wasn't recognised, print out a warning...

				if(attribs->cannonEffect == CANNONEFFECT_NONE)
				{
					bkPrintf("**** CANNON EFFECT TYPE \"%S\" NOT RECOGNISED! ****\n", tempString);
				}
			}
		}
		if((strcmpi(string, "SPRING") == 0)||(attribs->specialType == SPECIALTYPE_TRAMAMPOLINE))
		{
			attribs->specialType = SPECIALTYPE_TRAMAMPOLINE;
			if(node->GetAttrib("maxHeight", tempFloat))
			{
				attribs->springyHeight = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("maxHeight", tempInt))
				{
					attribs->springyHeight = METERTOUNIT((float)tempInt);
				}
			if(node->GetAttrib("radius", tempFloat))
			{
				attribs->springyRadius = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("radius", tempInt))
				{
					attribs->springyRadius = METERTOUNIT((float)tempInt);
				}
			if(node->GetAttrib("alwaysBounce", value))
			{
				attribs->alwaysBounce = TRUE;
			}
			if (node->GetAttrib("active",value))
			{
				if (value) attribs->state = true;
				else attribs->state = false;
			}
		}
		if((strcmpi(string, "STEPPINGSTONE")==0)||(attribs->specialType == SPECIALTYPE_STEPPINGSTONE))
		{
			attribs->specialType = SPECIALTYPE_STEPPINGSTONE;
			if(node->GetAttrib("sinkHeight",tempFloat))
			{
				attribs->steppingStoneHeight = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("sinkHeight",tempInt))
				{
					attribs->steppingStoneHeight = METERTOUNIT((float)tempInt);
				}
			if(node->GetAttrib("bobbleTime",tempFloat))
			{
				attribs->bobbleTime = tempFloat;
			}
			else
				if(node->GetAttrib("bobbleTime",tempInt))
				{
					attribs->bobbleTime = (float)tempInt;
				}
			if(node->GetAttrib("sinkAcceleration",tempFloat))
			{
				attribs->sinkAcceleration = -METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("sinkAcceleration",tempInt))
				{
					attribs->sinkAcceleration = -METERTOUNIT((float)tempInt);
				}
			if(node->GetAttrib("bobbleHeight", tempFloat))
			{
				attribs->bobbleHeight = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("bobbleHeight", tempInt))
				{
					attribs->bobbleHeight = METERTOUNIT((float)tempInt);
				}
		}
		if((strcmpi(string, "TILTING") == 0)||(attribs->specialType == SPECIALTYPE_TILTINGOBJECT))
		{
			attribs->specialType = SPECIALTYPE_TILTINGOBJECT;
			if(node->GetAttrib("xAngleMax", tempFloat))
			{
				attribs->xAngleMax = RAD(tempFloat);
			}
			else
				if(node->GetAttrib("xAngleMax", tempInt))
				{
					attribs->xAngleMax = RAD((float)tempInt);
				}
			if(node->GetAttrib("zAngleMax", tempFloat))
			{
				attribs->zAngleMax = RAD(tempFloat);
			}
			else
				if(node->GetAttrib("zAngleMax", tempInt))
				{
					attribs->zAngleMax = RAD((float)tempInt);
				}
			if(node->GetAttrib("maxAcceleration", tempFloat))
			{
				attribs->maxAcceleration = RAD(tempFloat);
			}
			else
				if(node->GetAttrib("maxAcceleration", tempInt))
				{
					attribs->maxAcceleration = RAD((float)tempInt);
				}
			if(node->GetAttrib("maxRadius", tempFloat))
			{
				attribs->maxRadius = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("maxRadius", tempInt))
				{
					attribs->maxRadius = METERTOUNIT((float)tempInt);
				}
			if(node->GetAttrib("springyness", tempFloat))
			{
				attribs->retardationForce = RAD(tempFloat);
			}
			else
				if(node->GetAttrib("springyness", tempInt))
				{
					attribs->retardationForce = RAD((float)tempInt);
				}
		}
		if((strcmpi(string, "MUSICBOX") == 0)||(attribs->specialType == SPECIALTYPE_MUSICBOX))
		{
			attribs->specialType = SPECIALTYPE_MUSICBOX;
			if(node->GetAttrib("radius", tempFloat))
			{
				attribs->musicBoxRadius = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("radius", tempInt))
				{
					attribs->musicBoxRadius = METERTOUNIT((float)tempInt);
				}
			tempString = node->GetAttrib("boundary");
			if(tempString)
			{
				attribs->musicBoxInstanceCRC = bkCRC32((uchar*)tempString, strlen(tempString), 0);
			}
			if(node->GetAttrib("active", value))
			{
				if(value) attribs->musicBoxState = TRUE;
				else attribs->musicBoxState = FALSE;
			}
		}
		if((strcmpi(string, "DOOR") == 0)||(attribs->specialType == SPECIALTYPE_DOOR))
		{
			attribs->specialType = SPECIALTYPE_DOOR;
			
			tempString = node->GetAttrib("type");
			if(tempString)
			{
				if(strcmpi("revolving", tempString) == 0)
				{
					attribs->doorType = DOORTYPE_REVOLVING;
				}
				if(strcmpi("springy", tempString) == 0)
				{
					attribs->doorType = DOORTYPE_SPRINGY;
				}
			}

			if(node->GetAttrib("springyness", tempFloat))
			{
				attribs->doorSpringyness = RAD(tempFloat);
			}
			else
				if(node->GetAttrib("springyness", tempInt))
				{
					attribs->doorSpringyness = RAD((float)tempInt);
				}
			if(node->GetAttrib("stiffness", tempFloat))
			{
				attribs->doorStiffness = RAD(tempFloat);
			}
			else
				if(node->GetAttrib("stiffness", tempInt))
				{
					attribs->doorStiffness = RAD((float)tempInt);
				}
		}
		if((strcmpi(string, "SWINGING") == 0)||(attribs->specialType == SPECIALTYPE_SWINGINGOBJECT))
		{
			attribs->specialType = SPECIALTYPE_SWINGINGOBJECT;
			if(node->GetAttrib("length", tempFloat))
			{
				attribs->length = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("length", tempInt))
				{
					attribs->length = METERTOUNIT((float)tempInt);
				}
			if(tempString = node->GetAttrib("model"))
			{
				if(strcmpi(tempString, "MONKEY") == 0)
				{
					attribs->model = SWINGMODEL_MONKEY;
				}
				else
				if(strcmpi(tempString, "FULLCHAND") == 0)
				{
					attribs->model = SWINGMODEL_CHANDELIER;
				}
				else
				if(strcmpi(tempString, "CHAND") == 0)
				{
					attribs->model = SWINGMODEL_CHANDTRIGGER;
				}
				else
				if(strcmpi(tempString, "CRATE") == 0)
				{
					attribs->model = SWINGMODEL_CRATEONROPE;
				}
				else
				{
					strcpy(attribs->modelName, tempString);
				}
			}
		}
		if((strcmpi(string, "HIGHVOLTAGE") == 0)||(attribs->specialType == SPECIALTYPE_HIGHVOLTAGE))
		{
			attribs->specialType = SPECIALTYPE_HIGHVOLTAGE;
			if(tempString = node->GetAttrib("type"))
			{
				if(strcmpi(tempString, "WALL") == 0)
				{
					attribs->voltageState = HIGHVOLTAGE_WALL;
				}
				else
					if(strcmpi(tempString, "GROUND") == 0)
					{
						attribs->voltageState = HIGHVOLTAGE_GROUND;
					}
			}
			if(node->GetAttrib("initialState", tempInt))
			{
				attribs->initialVoltageState = tempInt;
			}
			else
				if(node->GetAttrib("initialState", tempFloat))
				{
					attribs->initialVoltageState = (int)tempFloat;
				}
		}
		if((strcmpi(string, "FALLING") == 0)||(attribs->specialType == SPECIALTYPE_FALLINGOBJECT))
		{
			attribs->specialType = SPECIALTYPE_FALLINGOBJECT;
			if (node->GetAttrib("active",value))
			{
				attribs->state = value;
			}
			if((tempString = node->GetAttrib("fallingType")) != NULL)
			{
				if(strcmpi(tempString, "ONCE") ==0)
				{
					attribs->fallingType = FALLINGTYPE_ONCE;
				}
				else
					if(strcmpi(tempString, "RANDOM") ==0)
					{
						attribs->fallingType = FALLINGTYPE_RANDOM;
					}
					else
						if(strcmpi(tempString, "WAIT") == 0)
						{
							attribs->fallingType = FALLINGTYPE_WAIT;
						}
			}
			tempString = node->GetAttrib("groundImpact");
			if(!tempString) tempString = node->GetAttrib("impactType");
			if(tempString)
			{
				if(strcmpi(tempString, "DESTROY") ==0)
				{
					attribs->impactType = FALLINGTYPE_DESTROY;
				}
				else
					if(strcmpi(tempString, "REMAIN") ==0)
					{
						attribs->impactType = FALLINGTYPE_REMAIN;
					}
					else
						if(strcmpi(tempString, "CONVERTTODESTRUCTIBLE") == 0)
						{
							attribs->impactType = FALLINGTYPE_REMAINTODESTRUCTIBLE;
						}
						else
							if(strcmpi(tempString, "CONVERTTOSPARKY") == 0)
							{
								attribs->impactType = FALLINGTYPE_REMAINTOSPARKY;
							}
			}
			if(node->GetAttrib("radius", tempFloat))
			{
				attribs->fallingRadius = tempFloat;
			}
			else
				if(node->GetAttrib("radius", tempInt))
				{
					attribs->fallingRadius = (float)tempInt;
				}
			if(node->GetAttrib("randomTime", tempInt))
			{
				attribs->fallingPeriod = tempInt;
			}
			else
				if(node->GetAttrib("randomTime", tempFloat))
				{
					attribs->fallingPeriod = (int)tempFloat;
				}
		}
		if((strcmpi(string, "WANTEDPOSTER") == 0)||(attribs->specialType == SPECIALTYPE_WANTEDPOSTER))
		{
			attribs->specialType = SPECIALTYPE_WANTEDPOSTER;
			if(node->GetAttrib("number", tempInt))
			{
				attribs->posterNumber = tempInt;
			}
			else
				if(node->GetAttrib("number", tempFloat))
				{
					attribs->posterNumber = (int)tempFloat;
				}
			tempString = node->GetAttrib("SPECIALCASE");
			if(tempString)
			{
				if(strcmpi(tempString, "CLOCK") == 0)
				{
					attribs->posterSpecialCase = POSTERSPECIALCASE_CLOCK;
				}
			}
		}
		if((strcmpi(string, "MAILBOX") == 0)||(attribs->specialType == SPECIALTYPE_MAILBOX))
		{
			attribs->specialType = SPECIALTYPE_MAILBOX;
			if(node->GetAttrib("number", tempInt))
			{
				attribs->mailboxNumber = tempInt;
			}
			else
				if(node->GetAttrib("number", tempFloat))
				{
					attribs->mailboxNumber = (int)tempFloat;
				}
		}
		if((strcmpi(string, "SLIDE") == 0)||(attribs->specialType == SPECIALTYPE_SLIDE))
		{
			attribs->specialType = SPECIALTYPE_SLIDE;
		}
		if((strcmpi(string,"WATER") == 0)||(attribs->specialType == SPECIALTYPE_WATERYOBJECT))
		{
			attribs->specialType = SPECIALTYPE_WATERYOBJECT;
			if(node->GetAttrib("numResetPoints", tempInt))
			{
				attribs->numWaterResetPoints = tempInt;
			}
			else
				if(node->GetAttrib("numResetPoints", tempFloat))
				{
					attribs->numWaterResetPoints = (int)tempFloat;
				}
			if(tempString = node->GetAttrib("type"))
			{
				if(strcmpi(tempString, "CONCRETE") == 0)
				{
					attribs->waterType = WATERTYPE_CONCRETE;
				}
			}
		}
		if((strcmpi(string, "EFFECT") == 0)||(attribs->specialType == SPECIALTYPE_EFFECT))
		{
			attribs->specialType = SPECIALTYPE_EFFECT;
			if((tempString = node->GetAttrib("type")) != NULL)
			{
				if (strcmpi(tempString, "CHIMNEYPUFF")==0)
				{
					attribs->effect = EFFECT_CHIMNEYPUFF;
				}
			}
			if(node->GetAttrib("period", tempFloat))
			{
				attribs->effectPeriod = tempFloat;
			}
			else
				if(node->GetAttrib("period", tempInt))
				{
					attribs->effectPeriod = (float)tempInt;
				}
			if(node->GetAttrib("xspeed", tempFloat))
			{
				attribs->effectXSpeed = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("xspeed", tempInt))
				{
					attribs->effectXSpeed = METERTOUNIT((float)tempInt);
				}
			if(node->GetAttrib("yspeed", tempFloat))
			{
				attribs->effectYSpeed = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("yspeed", tempInt))
				{
					attribs->effectYSpeed = METERTOUNIT((float)tempInt);
				}
			if(node->GetAttrib("zspeed", tempFloat))
			{
				attribs->effectZSpeed = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("zspeed", tempInt))
				{
					attribs->effectZSpeed = METERTOUNIT((float)tempInt);
				}
		}
		if((strcmpi(string, "PRECIPITATION") == 0)||(attribs->specialType == SPECIALTYPE_PRECIPITATION))
		{
			attribs->specialType = SPECIALTYPE_PRECIPITATION;
			if(node->GetAttrib("density", tempInt))
			{
				attribs->snowDensity = tempInt;
			}
			else
				if(node->GetAttrib("density", tempFloat))
				{
					attribs->snowDensity = (int)tempFloat;
				}
			if(node->GetAttrib("size", tempFloat))
			{
				attribs->snowDim = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("size", tempInt))
				{
					attribs->snowDim = (float)METERTOUNIT(tempInt);
				}
			if(node->GetAttrib("period", tempFloat))
			{
				attribs->snowRate = tempFloat;
			}
			else
				if(node->GetAttrib("period", tempInt))
				{
					attribs->snowRate = (float)tempInt;
				}
			if((tempString = node->GetAttrib("type")) != NULL)
			{
				if (strcmpi(tempString, "SNOW")==0)
				{
					attribs->snowType = DEBRISFLAG_SNOW;
				}
				if(strcmpi(tempString, "RAIN") == 0)
				{
					attribs->snowType = DEBRISFLAG_RAIN;
				}
			}
		}
		if((strcmpi(string, "DESTRUCTIBLE") == 0)||(attribs->specialType == SPECIALTYPE_DESTRUCTIBLE))
		{
			attribs->specialType = SPECIALTYPE_DESTRUCTIBLE;
			tempString = node->GetAttrib("type");
			if (tempString)
			{
				if (strcmpi(tempString, "SPIN")==0)
				{
					attribs->destructible = DESTRUCTIBLETYPE_SPIN;
				}
				if (strcmpi(tempString, "CONTACT")==0)
				{
					attribs->destructible = DESTRUCTIBLETYPE_CONTACT;
				}
				if (strcmpi(tempString, "RESPONDTO")==0)
				{
					attribs->destructible = DESTRUCTIBLETYPE_RESPONDTO;
				}
				if(strcmpi(tempString, "TRIGGER") == 0)
				{
					attribs->destructible = DESTRUCTIBLETYPE_TRIGGER;
				}
				if(strcmpi(tempString, "ANIMATE") == 0)
				{
					attribs->destructible = DESTRUCTIBLETYPE_ANIMATE;
				}
				if(strcmpi(tempString, "ANIMATECONTACT") == 0)
				{
					attribs->destructible = DESTRUCTIBLETYPE_ANIMATECONTACT;
				}
				if(strcmpi(tempString, "ANIMATELOOP") ==0)
				{
					attribs->destructible = DESTRUCTIBLETYPE_ANIMATELOOP;
				}
				if(strcmpi(tempString, "OBJECTSPIN") == 0)
				{
					attribs->destructible = DESTRUCTIBLETYPE_OBJECTSPIN;
				}
				if(strcmpi(tempString, "WBTNTCRATE") == 0)
				{
					attribs->destructible = DESTRUCTIBLETYPE_WBTNTCRATE;
				}
				if(strcmpi(tempString, "SPARK") == 0)
				{
					attribs->destructible = DESTRUCTIBLETYPE_SPARK;
				}
				if (strcmpi(tempString, "SPIKEY") == 0)
				{
					attribs->destructible = DESTRUCTIBLETYPE_SPIKEY;
				}
			}
			tempString = node->GetAttrib("platform");
			if (tempString)
			{
				attribs->platformCRC = bkCRC32((uchar*)tempString,strlen(tempString),0);
			}
			if(node->GetAttrib("hits", tempInt))
			{
				attribs->hits = tempInt;
			}
			else
			{
				if(node->GetAttrib("hits", tempFloat))
				{
					attribs->hits = (int)tempFloat;
				}
			}
			tempString = node->GetAttrib("idle");
			if(tempString)
			{
				attribs->idle = bkCRC32((uchar*)tempString, strlen(tempString),0);
			}
			tempString = node->GetAttrib("first");
			if(tempString)
			{
				attribs->firstHit = bkCRC32((uchar*)tempString, strlen(tempString),0);
			}
			tempString = node->GetAttrib("penultimate");
			if(tempString)
			{
				attribs->secondHit = bkCRC32((uchar*)tempString, strlen(tempString),0);
			}
			tempString = node->GetAttrib("destroy");
			if(tempString)
			{
				attribs->destroyHit = bkCRC32((uchar*)tempString, strlen(tempString),0);
			}
			tempString = node->GetAttrib("destversion");
			if (tempString)
			{
				attribs->model = TRUE;
				strcpy(attribs->modelName,tempString);
			}
//			tempString = node->GetAttrib("texture");
//			if(tempString)
//			{
//				attribs->sparkTex = bkCRC32((uchar*)tempString, strlen(tempString), 0);
//			}
			if (node->GetAttrib("money", tempInt))
			{
				attribs->money = tempInt;
			}
			else
			{
				if (node->GetAttrib("money", tempFloat))
				{
					attribs->money = (int)tempFloat;
				}
			}
			if(node->GetAttrib("xoffset", tempFloat))
			{
				attribs->transX = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("xoffset", tempInt))
				{
					attribs->transX = METERTOUNIT((float)tempInt);
				}
			if(node->GetAttrib("yoffset", tempFloat))
			{
				attribs->transY = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("yoffset", tempInt))
				{
					attribs->transY = METERTOUNIT((float)tempInt);
				}
			if(node->GetAttrib("zoffset", tempFloat))
			{
				attribs->transZ = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("zoffset", tempInt))
				{
					
					attribs->transZ = METERTOUNIT((float)tempInt);
				}
			if(node->GetAttrib("componentDebris", value))
			{
				attribs->componentDebris = TRUE;
			}
			if (node->GetAttrib("indestructible", value))
			{
				if (value)
				{
					attribs->flags |= DESTRUCTIBLEFLAG_INDESTRUCTIBLE;
				}
				else
				{
					attribs->flags &= ~DESTRUCTIBLEFLAG_INDESTRUCTIBLE;
				}
			}
			if (node->GetAttrib("dontfadeout", value))
			{
				if (value)
				{
					attribs->flags |= DESTRUCTIBLEFLAG_DONTFADEOUT;
				}
				else
				{
					attribs->flags &= ~DESTRUCTIBLEFLAG_DONTFADEOUT;
				}
			}
			if (node->GetAttrib("updatecollafteranim", value))
			{
				if (value)
				{
					attribs->flags |= DESTRUCTIBLEFLAG_UPDATECOLLAFTERANIM;
				}
				else
				{
					attribs->flags &= ~DESTRUCTIBLEFLAG_UPDATECOLLAFTERANIM;
				}
			}
		}
		if((strcmpi(string, "ELECTRICITY") == 0)||(attribs->specialType == SPECIALTYPE_ELECTRICITY))
		{
			attribs->specialType = SPECIALTYPE_ELECTRICITY;
			if(node->GetAttrib("accuracy", tempFloat))
			{
				attribs->electricityAccuracy = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("accuracy", tempInt))
				{
					attribs->electricityAccuracy = METERTOUNIT((float)tempInt);
				}
			if(node->GetAttrib("initialState", tempInt))
			{
				attribs->state = (char)tempInt;
			}
		}
		if((strcmpi(string, "LASER") == 0)||(attribs->specialType == SPECIALTYPE_LASER))
		{
			attribs->specialType = SPECIALTYPE_LASER;
			if(node->GetAttrib("initialstate", tempInt))
			{
				attribs->state = (char)tempInt;
			}
			if(node->GetAttrib("size", tempFloat))
			{
				attribs->laserSize = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("size", tempInt))
				{
					attribs->laserSize = METERTOUNIT((float)tempInt);
				}
			tempString = node->GetAttrib("texture");
			if(tempString)
			{
				tempString = strlwr(tempString);
				strcpy(attribs->laserTexture, tempString);
			}
		}
		if((strcmpi(string, "LIGHTNING") == 0)||(attribs->specialType == SPECIALTYPE_LIGHTNING))
		{
			attribs->specialType = SPECIALTYPE_LIGHTNING;
		}
		if((strcmpi(string, "FIRE") == 0)||(attribs->specialType == SPECIALTYPE_FIRE))
		{
			attribs->specialType = SPECIALTYPE_FIRE;
			if(node->GetAttrib("SIZE", tempFloat))
			{
				attribs->fireSize = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("SIZE", tempInt))
				{
					attribs->fireSize = METERTOUNIT((float)tempInt);
				}
		}
		if((strcmpi(string, "TRANSPORTER") == 0)||(attribs->specialType == SPECIALTYPE_TRANSPORTER))
		{
			attribs->specialType = SPECIALTYPE_TRANSPORTER;
			tempString = node->GetAttrib("transportto");
			if(tempString)
			{
				attribs->transportTo = bkCRC32((uchar*)tempString, strlen(tempString), 0);
			}
			if(node->GetAttrib("xoffset", tempFloat))
			{
				attribs->transX = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("xoffset", tempInt))
				{
					attribs->transX = METERTOUNIT((float)tempInt);
				}
			if(node->GetAttrib("yoffset", tempFloat))
			{
				attribs->transY = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("yoffset", tempInt))
				{
					attribs->transY = METERTOUNIT((float)tempInt);
				}
			if(node->GetAttrib("zoffset", tempFloat))
			{
				attribs->transZ = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("zoffset", tempInt))
				{
					attribs->transZ = METERTOUNIT((float)tempInt);
				}
			if(node->GetAttrib("delay", tempFloat))
			{
				attribs->delay = tempFloat;
			}
			else
				if(node->GetAttrib("delay", tempInt))
				{
					attribs->delay = (float)tempInt;
				}
			if (node->GetAttrib("active",value))
			{
				attribs->state = value;
			}
			if(node->GetAttrib("fadeUp", value))
			{
				attribs->fadeUp = TRUE;
			}
			if(node->GetAttrib("fadeDown", value))
			{
				attribs->fadeDown = TRUE;
			}
		}
		if((strcmpi(string, "ICE") == 0)||(attribs->specialType == SPECIALTYPE_ICE))
		{
			attribs->specialType = SPECIALTYPE_ICE;
			tempString = node->GetAttrib("colour");
			if (tempString)
			{
				attribs->footstep = TRUE;
				sscanf(tempString,"%i %i %i %i",attribs->r,attribs->g,attribs->b,attribs->a);
			}
			tempString = node->GetAttrib("animation");
			if(tempString)
			{
				strcpy(attribs->modelName, tempString);
			}
		}
		if((strcmpi(string, "FOOD") == 0)||(attribs->specialType == SPECIALTYPE_RANDOMFOOD))
		{
			attribs->specialType = SPECIALTYPE_RANDOMFOOD;
			tempString = node->GetAttrib("type");
			if(tempString)
			{
				if(strcmpi(tempString, "BIG") == 0)
				{
					attribs->foodType = COLLECTIBLETYPE_BIGFOOD;
				}
				if((strcmpi(tempString, "SMALL") == 0)||(strcmpi(tempString, "GOOD") == 0))
				{
					attribs->foodType = COLLECTIBLETYPE_FOOD;
				}
				if((strcmpi(tempString, "STRANGE") == 0)||(strcmpi(tempString, "BAD") == 0))
				{
					attribs->foodType = COLLECTIBLETYPE_STRANGEFOOD;
				}
			}
		}
		if((strcmpi(string, "VEHICLE") == 0)||(attribs->specialType == SPECIALTYPE_VEHICLE))
		{
			attribs->specialType = SPECIALTYPE_VEHICLE;
			tempString = node->GetAttrib("type");
			if(tempString)
			{
				if(strcmpi(tempString, "TROLLEY") == 0)
				{
					attribs->vehicleType = VEHICLE_TROLLEY;
				}
				if(strcmpi(tempString, "FLOORPOLISHER") == 0)
				{
					attribs->vehicleType = VEHICLE_POLISHER;
				}
				if(strcmpi(tempString, "HOVERCRAFT") == 0)
				{
					attribs->vehicleType = VEHICLE_HOVERCRAFT;
				}
				if(strcmpi(tempString, "ROCKETBIKE") == 0)
				{
					attribs->vehicleType = VEHICLE_ROCKETBIKE;
				}
			}
		}
		if((strcmpi(string, "MINECART") == 0)||(attribs->specialType == SPECIALTYPE_MINECART))
		{
			attribs->specialType = SPECIALTYPE_MINECART;
			tempString = node->GetAttrib("RESETPOINT");
			if(tempString)
			{
				attribs->musicBoxInstanceCRC = bkCRC32((uchar*)tempString, strlen(tempString), 0);
			}
		}
		if((strcmpi(string, "SCENIC") == 0)||(attribs->specialType == SPECIALTYPE_SCENIC))
		{
			attribs->specialType = SPECIALTYPE_SCENIC;
			tempString = node->GetAttrib("animation");
			if(tempString)
			{
				strcpy(attribs->modelName, tempString);
			}
			tempString = node->GetAttrib("smoke");
			if(tempString)
			{
				attribs->deadFlockerCRC = bkCRC32((uchar*)tempString, strlen(tempString), 0);
			}
			if(node->GetAttrib("smokesize", tempFloat))
			{
				attribs->laserSize = METERTOUNIT(tempFloat);
			}
			else
			{
				if(node->GetAttrib("smokesize", tempInt))
				{
					attribs->laserSize = METERTOUNIT((float)tempInt);
				}
			}
			if (node->GetAttrib("bounce", value))
			{
				if (value) attribs->alwaysBounce = true;
				else attribs->alwaysBounce = false;
			}

			if(tempString = node->GetAttrib("type"))
			{
				if(strcmpi(tempString, "FIREWORKS")==0)
				{
					attribs->scenicType = SCENICTYPE_FIREWORKS;
				}
			}

		}
		if((strcmpi(string, "TUMBLEWEED") == 0)||(attribs->specialType == SPECIALTYPE_TUMBLEWEED))
		{
			attribs->specialType = SPECIALTYPE_TUMBLEWEED;
			tempString = node->GetAttrib("animation");
			if(tempString)
			{
				attribs->animCRC = bkCRC32((uchar*)tempString, strlen(tempString), 0);
			}
			tempString = node->GetAttrib("node");
			if(tempString)
			{
				attribs->deadFlockerCRC = bkCRC32((uchar*)tempString, strlen(tempString), 0);
			}
		}
		if((strcmpi(string, "WATERFALL") == 0)||(attribs->specialType == SPECIALTYPE_WATERFALL))
		{
			attribs->specialType = SPECIALTYPE_WATERFALL;
			tempString = node->GetAttrib("texture");
			if(tempString)
			{
				attribs->deadFlockerCRC = bkCRC32((uchar*)tempString, strlen(tempString), 0);
			}
			tempString = node->GetAttrib("start");
			if(tempString)
			{
				attribs->flockerSprayCRC = bkCRC32((uchar*)tempString, strlen(tempString), 0);
			}
			tempString = node->GetAttrib("end");
			if(tempString)
			{
				attribs->triggerCRC = bkCRC32((uchar*)tempString, strlen(tempString), 0);
			}
		}
		if((strcmpi(string, "FOUNTAIN") == 0)||(attribs->specialType == SPECIALTYPE_FOUNTAIN))
		{
			attribs->specialType = SPECIALTYPE_FOUNTAIN;
			if(node->GetAttrib("maxheight", tempFloat))
			{
				attribs->fountainMaxHeight = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("maxheight", tempInt))
				{
					attribs->fountainMaxHeight = METERTOUNIT((float)tempInt);
				}
			if(node->GetAttrib("period", tempFloat))
			{
				attribs->fountainPeriod = tempFloat;
			}
			else
				if(node->GetAttrib("period", tempInt))
				{
					attribs->fountainPeriod = (float)tempInt;
				}
			tempString = node->GetAttrib("type");
			if(tempString)
			{
				if(strcmpi(tempString, "IDLE") == 0)
				{
					attribs->fountainIdle = TRUE;
				}
				if(strcmpi(tempString, "ACTIVE") == 0)
				{
					attribs->fountainIdle = FALSE;
				}
				if(strcmpi(tempString, "TRIGGERED") == 0)
				{
					attribs->fountainIdle = -1;
				}
				if(strcmpi(tempString, "SCENIC") == 0)
				{
					attribs->fountainIdle = 2;
				}
				if(strcmpi(tempString, "INACTIVESCENIC") == 0)
				{
					attribs->fountainIdle = 3;
				}
			}
		}
		if((strcmpi(string, "SHATTER") == 0)||(attribs->specialType == SPECIALTYPE_SHATTER))
		{
			attribs->specialType = SPECIALTYPE_SHATTER;
			tempString = node->GetAttrib("shattertype");
			if(tempString)
			{
				if(strcmpi(tempString, "DROP") == 0)
				{
					attribs->shatterType = POLYFX_DROP;
				}
				if(strcmpi(tempString, "EXPLODE") == 0)
				{ 
					attribs->shatterType = POLYFX_EXPLODE;
				}
			}
			tempString = node->GetAttrib("triggertype");
			if(!tempString) tempString = node->GetAttrib("type");
			if(tempString)
			{
				if(strcmpi(tempString, "SPIN") == 0)
				{
					attribs->triggerType = SHATTERTYPE_SPIN;
				}
				if(strcmpi(tempString, "TRIGGER") == 0)
				{
					attribs->triggerType = SHATTERTYPE_TRIGGER;
				}
				if(strcmpi(tempString, "MARIO") == 0)
				{
					attribs->triggerType = SHATTERTYPE_MARIOPAINTING;
				}
			}
			if(node->GetAttrib("destroy", value))
			{
				attribs->destroy = TRUE;
			}
			if(node->GetAttrib("delay", tempFloat))
			{
				attribs->delay = tempFloat;
			}
			else
				if(node->GetAttrib("delay", tempInt))
				{
					attribs->delay = (float)tempInt;
				}
			tempString = node->GetAttrib("idle");
			if(tempString)
			{
				attribs->idleAnimCRC = bkCRC32((uchar*)tempString, strlen(tempString),0);
			}
			tempString = node->GetAttrib("destroy");
			if(tempString)
			{
				attribs->destroyAnimCRC = bkCRC32((uchar*)tempString,strlen(tempString),0);
			}
			tempString = node->GetAttrib("texture");
			if(tempString)
			{
				attribs->replacementCRC = bkCRC32((uchar*)tempString, strlen(tempString), 0);
			}
		}
		if((strcmpi(string,"SAFEPOINT") == 0)||(attribs->specialType == SPECIALTYPE_SAFEPOINT))
		{
			attribs->specialType = SPECIALTYPE_SAFEPOINT;
		}
		if((strcmpi(string,"HAAR") == 0)||(attribs->specialType == SPECIALTYPE_HAAR))
		{
			attribs->specialType = SPECIALTYPE_HAAR;
			if(node->GetAttrib("DENSITY", tempFloat))
			{
				attribs->density = tempFloat;
			}
			else
				if(node->GetAttrib("DENSITY", tempInt))
				{
					attribs->density = (float)tempInt;
				}
			tempString = node->GetAttrib("COLOUR");
			if(tempString)
			{
				// TP: Had to change this because in max, properties cannot include commas, useful that!
				sscanf(tempString,"%d %d %d",&attribs->collectibleColour[0],&attribs->collectibleColour[1],&attribs->collectibleColour[2]);
#if BPLATFORM == PS2
				attribs->collectibleColour[0] /= 2;							// CMD: you know what artists are like with numbers
				attribs->collectibleColour[1] /= 2;
				attribs->collectibleColour[2] /= 2;
#endif
			}
		}
		if((strcmpi(string,"MAGNET") == 0)||(attribs->specialType == SPECIALTYPE_MAGNET))
		{
			attribs->specialType = SPECIALTYPE_MAGNET;
			if(node->GetAttrib("MINRADIUS", tempFloat))
			{
				attribs->magnetMin = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("MINRADIUS", tempInt))
				{
					attribs->magnetMin = METERTOUNIT(((int)tempInt));
				}
			if(node->GetAttrib("MAXRADIUS", tempFloat))
			{
				attribs->magnetMax = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("MAXRADIUS", tempInt))
				{
					attribs->magnetMax = METERTOUNIT(((int)tempInt));
				}

		}
		if((strcmpi(string,"FLOCKING") == 0)||(attribs->specialType == SPECIALTYPE_FLOCKING))
		{
			attribs->specialType = SPECIALTYPE_FLOCKING;
			tempString = node->GetAttrib("MODEL");
			if(tempString)
			{
				strcpy(attribs->modelName, tempString);
			}
			if(node->GetAttrib("RANGE",tempFloat))
			{
				attribs->safetyRange = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("RANGE",tempInt))
				{
					attribs->safetyRange = METERTOUNIT((float)tempInt);
				}
			if(node->GetAttrib("NUMBER",tempInt))
			{
				attribs->numFlockers = tempInt;
			}
			else
				if(node->GetAttrib("NUMBER",tempFloat))
				{
					attribs->numFlockers = (int)tempFloat;
				}
			tempString = node->GetAttrib("DEADFLOCKER");
			if(tempString)
			{
				attribs->deadFlockerCRC = bkCRC32((uchar*)tempString,strlen(tempString),0);
			}
			tempString = node->GetAttrib("SPRAY");
			if(tempString)
			{
				attribs->flockerSprayCRC = bkCRC32((uchar*)tempString,strlen(tempString),0);
			}
		}
		if((strcmpi(string,"SPLAT") == 0)||(attribs->specialType == SPECIALTYPE_SPLAT))
		{
			attribs->specialType = SPECIALTYPE_SPLAT;
		}
		if((strcmpi(string,"DEATH") == 0)||(attribs->specialType == SPECIALTYPE_DEATH))
		{
			attribs->specialType = SPECIALTYPE_DEATH;
		}
		if((strcmpi(string, "FLAMETHROWA") == 0)||(attribs->specialType == SPECIALTYPE_FLAMETHROWA))
		{
			attribs->specialType = SPECIALTYPE_FLAMETHROWA;
			if(node->GetAttrib("length", tempFloat))
			{
				attribs->length = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("length", tempInt))
				{
					attribs->length = METERTOUNIT((float)tempInt);
				}
			if(node->GetAttrib("period", tempFloat))
			{
				attribs->delay = tempFloat;
			}
			else
				if(node->GetAttrib("period", tempInt))
				{
					attribs->delay = (float)tempInt;
				}
		}
		if((strcmpi(string, "MECHATWEETY") == 0)||(attribs->specialType == SPECIALTYPE_MECHATWEETY))
		{
			attribs->specialType = SPECIALTYPE_MECHATWEETY;
		}
		if((strcmpi(string, "TAZBOSSITEM") == 0)||(attribs->specialType == SPECIALTYPE_TAZBOSSITEM))
		{
			attribs->specialType = SPECIALTYPE_TAZBOSSITEM;
			tempString = node->GetAttrib("TYPE");
			if(tempString)
			{
				if(strcmpi(tempString, "TAZCRATE") == 0)
				{
					attribs->itemType = TAZBOSSITEM_TAZCRATE;
				}
				if(strcmpi(tempString, "CAMERA") == 0)
				{
					attribs->itemType = TAZBOSSITEM_CAMERA;
				}
				if(strcmpi(tempString, "CONTROL POINT") == 0)
				{
					attribs->itemType = TAZBOSSITEM_CAMERACONTROL;
				}
				if(strcmpi(tempString, "DOOR") == 0)
				{
					attribs->itemType = TAZBOSSITEM_DOOR;
				}
				if(strcmpi(tempString, "MAGNET") == 0)
				{
					attribs->itemType = TAZBOSSITEM_MAGNET;
				}
				if(strcmpi(tempString, "MAGNETHITPOINT") == 0)
				{
					attribs->itemType = TAZBOSSITEM_MAGNETHITPOINT;
				}
				if(strcmpi(tempString, "MAGNETNODE") == 0)
				{
					attribs->itemType = TAZBOSSITEM_MAGNETNODE;
				}
				if(strcmpi(tempString, "DOORBUTTON") == 0)
				{
					attribs->itemType = TAZBOSSITEM_DOORBUTTON;
				}
				if(strcmpi(tempString, "COLLPLANE") == 0)
				{
					attribs->itemType = TAZBOSSITEM_COLLPLANE;
				}
				if(strcmpi(tempString, "SHUTTER") == 0)
				{
					attribs->itemType = TAZBOSSITEM_SHUTTER;
				}
			}
			tempString = node->GetAttrib("REPLACEMENT");
			if(tempString)
			{
				sprintf(anotherString, "objects\\%s.obe", tempString);
				attribs->replacementCRC = bkCRC32((uchar*)anotherString, strlen(anotherString), 0);
			}
			tempString = node->GetAttrib("ANIMATION");
			if(tempString)
			{
				attribs->animCRC = bkCRC32((uchar*)tempString, strlen(tempString), 0);
			}
			if(node->GetAttrib("NUMBER", tempInt))
			{
				attribs->camNumber = tempInt;
			}
			else
				if(node->GetAttrib("NUMBER", tempFloat))
				{
					attribs->camNumber = bmFloatToInt(tempFloat);
				}
		}
		if((strcmpi(string, "AIRCON") == 0)||(attribs->specialType == SPECIALTYPE_AIRCON))
		{
			attribs->specialType = SPECIALTYPE_AIRCON;
		}
		if((strcmpi(string, "TARBUBBLE") == 0)||(attribs->specialType == SPECIALTYPE_TARBUBBLE))
		{
			attribs->specialType = SPECIALTYPE_TARBUBBLE;

			tempString = node->GetAttrib("texture");
			if(tempString)
			{
				strcpy(attribs->modelName, tempString);
			}
			if(node->GetAttrib("height", tempFloat))
			{
				attribs->springyHeight = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("height", tempInt))
				{
					attribs->springyHeight = METERTOUNIT((float)tempInt);
				}
			if(node->GetAttrib("width", tempFloat))
			{
				attribs->springyRadius = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("width", tempInt))
				{
					attribs->springyRadius = METERTOUNIT((float)tempInt);
				}
			if(node->GetAttrib("initialstate", tempInt))
			{
				attribs->initState = tempInt;
			}
		}
		if ((strcmpi(string,"CAGE")==0)||(attribs->specialType == SPECIALTYPE_SPAWNCRATE))
		{
			attribs->specialType = SPECIALTYPE_SPAWNCRATE;
		}
		if((strcmpi(string, "HALO") == 0)||(attribs->specialType == SPECIALTYPE_HALO))
		{
			attribs->specialType = SPECIALTYPE_HALO;
			if(node->GetAttrib("size", tempFloat))
			{
				attribs->maxRadius = METERTOUNIT(tempFloat);
			}
			else
				if(node->GetAttrib("size", tempInt))
				{
					attribs->maxRadius = METERTOUNIT((float)tempInt);
				}
			tempString = node->GetAttrib("texture");
			if(tempString)
			{
				attribs->snowType = bkCRC32((uchar*)tempString, strlen(tempString), 0);
			}
			tempString = node->GetAttrib("node");
			if(tempString)
			{
				attribs->deadFlockerCRC = bkCRC32((uchar*)tempString, strlen(tempString), 0);
			}
			tempString = node->GetAttrib("COLOUR");
			if(tempString)
			{
				sscanf(tempString,"%d %d %d",&attribs->haloColour[0],&attribs->haloColour[1],&attribs->haloColour[2]);
			}
		}
		if((strcmpi(string, "MIRRORBALL") == 0)||(attribs->specialType == SPECIALTYPE_MIRRORBALL))
		{
			attribs->specialType = SPECIALTYPE_MIRRORBALL;
		}
		if((strcmpi(string, "ZAPPER") == 0)||(attribs->specialType == SPECIALTYPE_ZAPPER))
		{
			attribs->specialType = SPECIALTYPE_ZAPPER;
			tempString = node->GetAttrib("type");
			if(tempString)
			{
				if(strcmpi(tempString, "MOUSE") == 0)
				{
					attribs->zapperType = ZAPPER_MOUSE;
				}
				if(strcmpi(tempString, "BALL") == 0)
				{
					attribs->zapperType = ZAPPER_BALL;
				}
				if(strcmpi(tempString, "CRATE") == 0)
				{
					attribs->zapperType = ZAPPER_CRATE;
				}
				if(strcmpi(tempString, "TAZ") == 0)
				{
					attribs->zapperType = ZAPPER_TAZ;
				}
			}
		}
		if((strcmpi(string, "ARROW") == 0)||(attribs->specialType == SPECIALTYPE_ARROW))
		{
			attribs->specialType = SPECIALTYPE_ARROW;
		}
		if((strcmpi(string,"FRONTENDCAMERA") == 0)||(attribs->specialType == SPECIALTYPE_FECAMERA))
		{
			attribs->specialType = SPECIALTYPE_FECAMERA;
			tempString = node->GetAttrib("MENU");
			if(tempString)
			{
				if(strcmpi(tempString,"MAIN") == 0)
				{
					attribs->frontEndCamera = FECAM_MAIN;
				}
				if(strcmpi(tempString,"START") == 0)
				{
					attribs->frontEndCamera = FECAM_STARTGAME;
				}
				if(strcmpi(tempString,"OPTIONS") == 0)
				{
					attribs->frontEndCamera = FECAM_OPTIONS;
				}
				if(strcmpi(tempString,"AUDIO") == 0)
				{
					attribs->frontEndCamera = FECAM_AUDIO;
				}
				if(strcmpi(tempString,"TWOPLAYER") == 0)
				{
					attribs->frontEndCamera = FECAM_TWOPLAYER;
				}
				if(strcmpi(tempString,"TWOPLAYER_TOURNAMENT") == 0)
				{
					attribs->frontEndCamera = FECAM_TWOPLAYER_TOURNAMENT;
				}
				if(strcmpi(tempString,"TWOPLAYER_TIME") == 0)
				{
					attribs->frontEndCamera = FECAM_TWOPLAYER_TIME;
				}
				if(strcmpi(tempString,"TWOPLAYER_DESTRUCTION") == 0)
				{
					attribs->frontEndCamera = FECAM_TWOPLAYER_DESTRUCTION;
				}
				if(strcmpi(tempString,"TWOPLAYER_RACE") == 0)
				{
					attribs->frontEndCamera = FECAM_TWOPLAYER_RACE;
				}
				if(strcmpi(tempString,"RECORDS") == 0)
				{
					attribs->frontEndCamera = FECAM_RECORDS;
				}
				if(strcmpi(tempString,"EXTRAS") == 0)
				{
					attribs->frontEndCamera = FECAM_EXTRAS;
				}
				if(strcmpi(tempString,"EXTRAS_CREDITS") == 0)
				{
					attribs->frontEndCamera = FECAM_EXTRAS_CREDITS;
				}
				if(strcmpi(tempString,"EXTRAS_GALLERY") == 0)
				{
					attribs->frontEndCamera = FECAM_EXTRAS_GALLERY;
				}
				if(strcmpi(tempString,"EXTRAS_SECRETS") == 0)
				{
					attribs->frontEndCamera = FECAM_EXTRAS_SECRETS;
				}
				/*if(strcmpi(tempString,"SLOTONE") == 0)
				{
					attribs->frontEndCamera = MENU_SLOTONE;
				}
				if(strcmpi(tempString,"SLOTTWO") == 0)
				{
					attribs->frontEndCamera = MENU_SLOTTWO;
				}
				if(strcmpi(tempString,"SLOTTHREE") == 0)
				{
					attribs->frontEndCamera = MENU_SLOTTHREE;
				}
				*/
			}
			tempString = node->GetAttrib("CAMERALOOKAT");
			if(tempString)
			{
				attribs->cameraLookAt = bkCRC32((uchar*)tempString,strlen(tempString),0);
			}
		}
	#if BPLATFORM==GAMECUBE
		if ((strcmpi(string,"REVERB")==0)||(attribs->specialType == SPECIALTYPE_REVERB))
		{
			attribs->specialType = SPECIALTYPE_REVERB;
			tempString = node->GetAttrib("type");
			if(tempString)
			{
				if(strcmpi(tempString, "SMALLROOM") == 0)
				{
					attribs->reverbType = ESFX_SMALL_ROOM_REVERB;
				}
				if(strcmpi(tempString, "LARGEROOM") == 0)
				{
					attribs->reverbType = ESFX_LARGE_ROOM_REVERB;
				}
				if(strcmpi(tempString, "CAVERN") == 0)
				{
					attribs->reverbType = ESFX_BIG_CAVERN_ECHO;
				}
			}
		}

	#endif
	}

	// GENERAL INFO
	if (node->GetAttrib("visable",value))
	{
		attribs->invisible = 1-value;
		attribs->visible = value;
		if (attribs->invisible==TRUE)
		{
			attribs->collision = FALSE;
		}
	}
	//check for the power of invisibility
	if(node->GetAttrib("invisible", value))
	{
		attribs->invisible = value;
		if ((attribs->invisible==TRUE)&&(attribs->specialType!=SPECIALTYPE_TRANSPORTER)&&(attribs->specialType!=SPECIALTYPE_SPEEDBOOST))
		{
			attribs->collision = FALSE;
		}
	}
	if (node->GetAttrib("collision",value))
	{
		attribs->collision = value;
	}
	if (node->GetAttrib("liveupdate",value))
	{
		if (value==TRUE) attribs->liveUpdate = CACHEFLAGS_LIVEUPDATE;
		else attribs->liveUpdate = FALSE;
	}
	if (node->GetAttrib("livescaleupdate",value))
	{
		if (value) attribs->liveUpdate|=CACHEFLAGS_LIVESCALEUPDATE;
		else attribs->liveUpdate &=~CACHEFLAGS_LIVESCALEUPDATE;
	}

	// check for collectibles
	string = node->GetAttrib("collectible");
	if (string)
	{
		// there is collectible information
		if (strcmpi(string,"TAZCAM")==0)
		{
			attribs->collectible = COLLECTIBLETYPE_SECURITYBOX;
		}
		if (strcmpi(string,"PICKUP")==0) attribs->collectible = COLLECTIBLETYPE_SPINPAD;

		if (strcmpi(string,"TAZTOP")==0) attribs->collectible = COLLECTIBLETYPE_PICKUP;

		if ((strcmpi(string,"GENERICPICKUP")==0)||(strcmpi(string,"GENERICPICKUPEASY")==0)||(strcmpi(string,"GENERICPICKUPMEDIUM")==0))
		{
			attribs->collectible = COLLECTIBLETYPE_PICKUP;

			if ((strcmpi(string,"GENERICPICKUPEASY")==0)&&(gameStatus.globalSettings.difficulty!=DIFF_EASY))
			{
				attribs->collectible = ENDOFCOLLECTIBLETYPES;
			}
			if ((strcmpi(string,"GENERICPICKUPMEDIUM")==0)&&(gameStatus.globalSettings.difficulty==DIFF_HARD))
			{
				attribs->collectible = ENDOFCOLLECTIBLETYPES;
			}

			tempString = node->GetAttrib("COLOUR");
			if(tempString)
			{
				// TP: Had to change this because in max, properties cannot include commas, useful that!
				sscanf(tempString,"%d %d %d",&attribs->collectibleColour[0],&attribs->collectibleColour[1],&attribs->collectibleColour[2]);
				attribs->collectibleColour[0] /= 2;							// CMD: you know what artists are like with numbers
				attribs->collectibleColour[1] /= 2;
				attribs->collectibleColour[2] /= 2;
			}
			else
			{
				attribs->collectibleColour[0] = 127;
				attribs->collectibleColour[1] = 64;
				attribs->collectibleColour[2] = 0;
			}
		}
		if (strcmpi(string,"SMALLFOOD")==0) attribs->collectible = COLLECTIBLETYPE_FOOD;
		if (strcmpi(string,"LARGEFOOD")==0) attribs->collectible = COLLECTIBLETYPE_BIGFOOD;
		if (strcmpi(string,"BURPCAN") == 0) attribs->collectible = COLLECTIBLETYPE_BURPCAN;
		if (strcmpi(string,"TESTTUBE") == 0) attribs->collectible = COLLECTIBLETYPE_TESTTUBE;
		if (strcmpi(string,"HICCUP") == 0) attribs->collectible = COLLECTIBLETYPE_HICCUP;
		if (strcmpi(string,"BUBBLEGUM") == 0) attribs->collectible = COLLECTIBLETYPE_BUBBLEGUM;
		if (strcmpi(string,"CHILLIPEPPER") == 0) attribs->collectible = COLLECTIBLETYPE_CHILLIPEPPER;

		if(node->GetAttrib("RESPAWN",value))
		{
			if(value) attribs->respawn = TRUE;
			else attribs->respawn = FALSE;
		}
	}
	else
	{
		if (node->GetAttrib("collectible",value))
		{
			// no collectible
			if (value==0) attribs->collectible = COLLECTIBLETYPE_NONE;
		}
	}

	// check for destructibles
	string = node->GetAttrib("destructible");
	if (string)
	{
		// there is destructible information
		attribs->specialType = SPECIALTYPE_DESTRUCTIBLE;
		attribs->destructible = DESTRUCTIBLETYPE_SPIN;
	}
	else
	{
		if (node->GetAttrib("destructible",value))
		{
			// no destructible
			if (value==0) attribs->destructible = DESTRUCTIBLETYPE_NONE;
		}
	}
}