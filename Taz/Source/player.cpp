// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : player.cpp
//   Purpose : functions for setting up players
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#include "global.h"
#include "actors.h"
#include "main.h"
#include "player.h"
#include "mapfile.h"
#include "scene.h"
#include "camera.h"
#include "maths.h"
#include "physics.h"
#include "attach.h"
#include "trail.h"
#include "tweety.h"
#include "control.h"
#include "status.h"
#include "playerstats.h"
#include "animation.h"
#include "textures.h"
#include "textfx.h"
#include "display.h"
#include "files.h"
#include "multiplayer.h"

#include "ZooBoss.h"					// PP: Zoo Boss - Gossamer vs. Taz in an Elephant Pong showdown
#include "WestBoss.h"					// PP: Wile E West Boss - Yosemite Sam's ammo dump


/*	--------------------------------------------------------------------------------
	Function 	: InitialisePlayers
	Purpose 	: Initialse the player array
	Parameters 	: 
	Returns 	: 
	Info 		: Should be called prior to player setup
*/
void InitialisePlayers(void)
{
	int i;

	for (i=0;i<MAX_PLAYERS;i++)
	{
		gameStatus.player[i].actorInstance = NULL;
		gameStatus.player[i].camera = NULL;
		gameStatus.player[i].controller = NULL;
		InitialisePlayerStatsAtStartOfGame(gameStatus.player[i].stats);
	}
	gameStatus.player1 = NULL;
	gameStatus.player2 = NULL;
}

/*	--------------------------------------------------------------------------------
	Function 	: SetupPlayer
	Purpose 	: Setup a player
	Parameters 	: playerNumber, actorInstance, cameraInfo, controller
	Returns 	: PLAYER
	Info 		: Called to setup a player
*/
PLAYER *SetupPlayer(int playerNumber, ACTORINSTANCE *actorInstance, CAMERAINFO *cameraInfo, PADCONTROLSTATUS *controller)
{
	if (playerNumber >= MAX_PLAYERS) return NULL;

	gameStatus.player[playerNumber].actorInstance = actorInstance;
	gameStatus.player[playerNumber].camera = cameraInfo;
	gameStatus.player[playerNumber].controller = controller;

	// PP: set default flags to start with
	gameStatus.player[playerNumber].flags=PLAYER_DEFAULT_FLAGS;

	if (gameStatus.player[playerNumber].actorInstance)
	{
		gameStatus.player[playerNumber].actorInstance->playerNo = playerNumber;
	}

	if (playerNumber==0)
	{
		gameStatus.player1 = gameStatus.player[playerNumber].actorInstance;
	}
	else
	{
		if (playerNumber==1)
		{
			gameStatus.player2 = gameStatus.player[playerNumber].actorInstance;
		}
	}
	return &gameStatus.player[playerNumber];
}

ACTORINSTANCE *LoadSheDevil(void)
{
	TBVector	tempVector;

	// Setup Taz 2
	LoadWholePackageBkg("shedevil", "shedevil.obe",1);

	// PP: set up and open the books to be displayed during loading
	openLoadingBooks();

	while (GetCurrentBkgLoadingStatus()==TRUE)
	{
		DrawLoadLoop();
	}

	// PP: close the books that were displayed during loading
	closeLoadingBooks();

	ACTORINSTANCE *shedevilInstance = CreateActorInstance(&characterInstances,"shedevil.obe","shedevil");
	ASSERT(shedevilInstance);

	// CMD: need to know player nos. before 'SetUpPlayer' in order to create spin with correct colours
	shedevilInstance->playerNo = 1;

	LoadActor("shedevilbowtie.obe","shedevil");

	EnableActorCartoonRenderMode(&shedevilInstance->actorInstance);
	CreateActorInstanceBehaviour(shedevilInstance);
	CreateActorInstanceAnimation(shedevilInstance);
	CreateActorInstanceStatus(shedevilInstance);
	CreateActorInstanceShadow(shedevilInstance,METERTOUNIT(0.7f),METERTOUNIT(2.5));
	CreateActorInstanceCharacterInfo(shedevilInstance, CHARACTERINFO_CREATESPIN);
	SetActorInertia(shedevilInstance->actorBehaviour,0);
	SetActorMinFricAngle(shedevilInstance->actorBehaviour,0.7f);
	SetActorMaxSpeed(shedevilInstance->actorBehaviour,METERTOUNIT(9)); // set max speed 8mps
	PlayAnimationByName(shedevilInstance,"idle1",1,1,0,0,NONE);
	
	SETVECTOR(tempVector,0.0f,METERTOUNIT(1.0f),0.0f,1.0f);
	SetActorCylinder(0,shedevilInstance->actorBehaviour,tempVector,METERTOUNIT(0.5f),METERTOUNIT(1.2f));
	AddActorToCollisionCylinderCache(&collisionCache,shedevilInstance);
	
	AttachObjectToCharacter(shedevilInstance,"shedevilbowtie.obe","spinetop",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);
	trailSetUp(shedevilInstance);

	return shedevilInstance;
}

ACTORINSTANCE *LoadTaz(void)
{
	ACTORINSTANCE *tazInstance;
	TBVector tempVector;

	tazInstance = CreateActorInstance(&characterInstances,"tazdevil.obe","taz");
	ASSERT(tazInstance);

	// CMD: need to know player nos. before 'SetUpPlayer' in order to create spin with correct colours
	tazInstance->playerNo = 0;

	EnableActorCartoonRenderMode(&tazInstance->actorInstance);
	CreateActorInstanceBehaviour(tazInstance);
	CreateActorInstanceAnimation(tazInstance);
	CreateActorInstanceStatus(tazInstance);
	CreateActorInstanceShadow(tazInstance,METERTOUNIT(0.7f),METERTOUNIT(2.5),SHADOWMAP_RADIUS_TAZ);
	CreateActorInstanceCharacterInfo(tazInstance, CHARACTERINFO_CREATESPIN);
	SetActorInertia(tazInstance->actorBehaviour,0);
	SetActorMinFricAngle(tazInstance->actorBehaviour,0.7f);
	SetActorMaxSpeed(tazInstance->actorBehaviour,METERTOUNIT(9)); // set max speed 8mps
	SETVECTOR(tempVector,0.0f,METERTOUNIT(0.45f),0.0f,1.0f);
	SetActorCylinder(0,tazInstance->actorBehaviour,tempVector,METERTOUNIT(0.5f),METERTOUNIT(1.0f));
	AddActorToCollisionCylinderCache(&collisionCache,tazInstance);

	if (!gameStatus.multiplayer.versusCPU)
	{
		SETVECTOR(tempVector,0.0f,METERTOUNIT(1.0f),0.0f,1.0f);
		SetActorCylinder(0,tazInstance->actorBehaviour,tempVector,METERTOUNIT(0.5f),METERTOUNIT(1.2f));
		AddActorToCollisionCylinderCache(&collisionCache,tazInstance);
	}

	PlayAnimationByName(tazInstance,"idle1",1,1,0,0,NONE);
	trailSetUp(tazInstance);

	return tazInstance;
}

/*
ACTORINSTANCE *LoadSylvester(void)
{
	char *name = "sylvester.obe";
	char tempName[32];
	ACTORINSTANCE *sylvesterInstance;
	TBActor *pActor;

	pActor = FindActorInActorList(name);
	if(!pActor) 
	{
		char *temp;

		strcpy(tempName, name);

		temp = FindChar(tempName,'.');
		if(temp!=NULL)
			*temp = 0;

		LoadWholePackage(tempName, 1);
		LoadActor(name, tempName);
	}

	sylvesterInstance = CreateActorInstance(&characterInstances, name, "sylvester");
	ASSERT(sylvesterInstance);

	EnableActorCartoonRenderMode(&sylvesterInstance->actorInstance);
	CreateActorInstanceBehaviour(sylvesterInstance);
	CreateActorInstanceAnimation(sylvesterInstance);
	CreateActorInstanceStatus(sylvesterInstance);
	CreateActorInstanceShadow(sylvesterInstance,METERTOUNIT(0.7f),METERTOUNIT(2.5));
	CreateActorInstanceCharacterInfo(sylvesterInstance);
	SetActorInertia(sylvesterInstance->actorBehaviour,0);
	SetActorMinFricAngle(sylvesterInstance->actorBehaviour,0.7f);
	SetActorMaxSpeed(sylvesterInstance->actorBehaviour,METERTOUNIT(9)); // set max speed 8mps
	PlayAnimationByName(sylvesterInstance,"idle1",1,1,0,0,NONE);

	trailSetUp(sylvesterInstance);

	return sylvesterInstance;
}



ACTORINSTANCE *LoadDaffy(void)
{
	char *name = "daft.obe";
	char tempName[32];
	ACTORINSTANCE *daffyInstance;
	TBActor *pActor;

	pActor = FindActorInActorList(name);
	if(!pActor) 
	{
		char *temp;

		strcpy(tempName, name);

		temp = FindChar(tempName,'.');
		if(temp!=NULL)
			*temp = 0;

		LoadWholePackage(tempName, 1);
		LoadActor(name, tempName);
	}

	daffyInstance = CreateActorInstance(&characterInstances, name, "daffy");
	ASSERT(daffyInstance);

	EnableActorCartoonRenderMode(&daffyInstance->actorInstance);
	CreateActorInstanceBehaviour(daffyInstance);
	CreateActorInstanceAnimation(daffyInstance);
	CreateActorInstanceStatus(daffyInstance);
	CreateActorInstanceShadow(daffyInstance,METERTOUNIT(0.7f),METERTOUNIT(2.5));
	CreateActorInstanceCharacterInfo(daffyInstance);
	SetActorInertia(daffyInstance->actorBehaviour,0);
	SetActorMinFricAngle(daffyInstance->actorBehaviour,0.7f);
	SetActorMaxSpeed(daffyInstance->actorBehaviour,METERTOUNIT(9)); // set max speed 8mps
	PlayAnimationByName(daffyInstance,"idle1",1,1,0,0,NONE);

	trailSetUp(daffyInstance);

	return daffyInstance;
}
*/



ACTORINSTANCE *LoadGameCharacter(char *name, char *id)
{
	// CPW: char *name = "mrfudd.obe";
	char tempName[32];
	ACTORINSTANCE *pActorInstance;
	TBActor *pActor;

	pActor = FindActorInActorList(name);
	if(!pActor) 
	{
		char *temp;

		strcpy(tempName, name);

		temp = FindChar(tempName,'.');
		if(temp!=NULL)
			*temp = 0;

		LoadWholePackage(tempName, 1);
		LoadActor(name, tempName);
	}

// CPW: 	sylvesterInstance = CreateActorInstance(&characterInstances, name, "mrfudd");
	pActorInstance = CreateActorInstance(&characterInstances, name, id);
	ASSERT(pActorInstance);

	EnableActorCartoonRenderMode(&pActorInstance->actorInstance);
	CreateActorInstanceBehaviour(pActorInstance);
	CreateActorInstanceAnimation(pActorInstance);
	CreateActorInstanceStatus(pActorInstance);
	CreateActorInstanceShadow(pActorInstance,METERTOUNIT(0.7f),METERTOUNIT(2.5));
	CreateActorInstanceCharacterInfo(pActorInstance);
	SetActorInertia(pActorInstance->actorBehaviour,0);
	SetActorMinFricAngle(pActorInstance->actorBehaviour,0.7f);
	SetActorMaxSpeed(pActorInstance->actorBehaviour,METERTOUNIT(9)); // set max speed 8mps
	PlayAnimationByName(pActorInstance,"idle1",1,1,0,0,NONE);

	trailSetUp(pActorInstance);

	return pActorInstance;
}


/*	--------------------------------------------------------------------------------
	Function 	: CreatePlayers
	Purpose 	: Create the players and cameras needed for the current scene
	Parameters 	: scene
	Returns 	: 
	Info 		: 
*/
void CreatePlayers(int scene)
{
	TBVector			cameraEye, cameraGarib, tempVector;
	int					i;
	ACTORINSTANCE		*tazInstance,*secondPlayerInstance,*tempInstance, *magnet;
	CAMERAINFO			*cameraInfo;

	// Setup Taz
	if (scene == SCENE_LANGUAGESELECT)
	{
		SetupPlayer(0,NULL,NULL,NULL);
		SetupPlayer(1,NULL,NULL,NULL);
		return;
	}
	else
		tazInstance = LoadTaz();

//	InitMultiplayer(scene);

	if (gameStatus.multiplayer.numPlayers==2 || map.sceneNumber == SCENE_CITYBOSS)
	{
#ifndef CONSUMERDEMO
		switch(map.sceneNumber)
		{
		case SCENE_CITYBOSS:
			gameStatus.multiplayer.numPlayers = 2;

			secondPlayerInstance = LoadDaffy;
			// CMD: initialise magnets associations
			for(magnet = map.invisibleInstances.head.next;magnet != &map.invisibleInstances.head;magnet = magnet->next)
			{
				if(magnet->special == SPECIALTYPE_MAGNET)
				{
					magnet->magnetInfo->character[0].actorEffected = tazInstance;
					magnet->magnetInfo->character[1].actorEffected = secondPlayerInstance;
				}
			}
			break;

		case SCENE_ZOOBOSS:
			{
				TBVector	pos={0,0,0,0};
				float		yAng=0.0f;
				secondPlayerInstance = ZooBoss_game.createActorInstance("gossamer.obe", pos, yAng, BGACT_CARTOON|BGACT_BEHAVIOUR|BGACT_ANIMATION|BGACT_CHARINFO);

				CreateActorInstanceShadow(secondPlayerInstance,METERTOUNIT(1.4f),METERTOUNIT(10), SHADOWTYPE_ZOOBOSS);
			}
			break;

		case SCENE_WESTBOSS:
			{
				ACTORINSTANCE*	samPosInst;

				samPosInst=FindActorInstanceInMap("sam_inst", 0, &map);

				secondPlayerInstance=WestBoss_game.createActorInstance(WBSAM_MODEL, samPosInst->actorInstance.position, WBSAM_YANG, BGACT_LINE|BGACT_ANIMATION|BGACT_CHARINFO);
			}
			break;

		default:
			// Setup shedevil
#endif
			secondPlayerInstance = LoadSheDevil();
#ifndef CONSUMERDEMO
		}
#endif

		//AddActorToCollisionCylinderCache(&collisionCache, secondPlayerInstance);
	}
	// set start position for taz
	// use map
	if (map.startPos)
	{
		tempInstance = FindActorInstanceInMap(NULL,map.startPos,&map);
		
		if (tempInstance)
			SetStartPosition(tazInstance, tempInstance->actorInstance.position,tempInstance->actorInstance.orientation);
		else
		{
			tempVector[0] = 0.0f;
			tempVector[1] = METERTOUNIT(5.0f);
			tempVector[2] = 0.0f;
			tempVector[3] = 1.0f;

			SetStartPosition(tazInstance, tempVector);
		}
	}
	else
	{
		switch(map.sceneNumber)
		{
		case SCENE_FRONTEND:
			SETVECTOR(tempVector,0.0f,METERTOUNIT(0.0f),0.0f,1.0f);
			break;
		case SCENE_CITYBOSS:
			SETVECTOR(tempVector,METERTOUNIT(5.0f),METERTOUNIT(5.0f),0.0f,1.0f);
			break;
		default:
			SETVECTOR(tempVector,0.0f,METERTOUNIT(5.0f),0.0f,1.0f);
			break;
		}
		SetStartPosition(tazInstance, tempVector);
	}

	// JW: Assign second player a position
	if (gameStatus.multiplayer.numPlayers == 2)
	{
		if (map.startPos2)
		{
			tempInstance = FindActorInstanceInMap(NULL,map.startPos2,&map);
			
			if (tempInstance)
				SetStartPosition(secondPlayerInstance, tempInstance->actorInstance.position,tempInstance->actorInstance.orientation);
			else
			{
				tempVector[0] = METERTOUNIT(5.0f);
				tempVector[1] = METERTOUNIT(5.0f);
				tempVector[2] = 0.0f;
				tempVector[3] = 1.0f;
				
				SetStartPosition(secondPlayerInstance, tempVector);
			}
		}
		else
		{
			tempVector[0] = METERTOUNIT(5.0f);
			tempVector[1] = METERTOUNIT(5.0f);
			tempVector[2] = 0.0f;
			tempVector[3] = 1.0f;
			
			SetStartPosition(secondPlayerInstance, tempVector);
		}
	}

	for (i=0;i<gameStatus.multiplayer.numPlayers;i++)
	{
		cameraInfo = &camera[i];

		bmVectorSet(cameraEye,0.0f,0.0f,METERTOUNIT(2.0f),1.0f);

		if (i == 0)
		{
			VectorQuatMultiply(cameraEye,tazInstance->actorInstance.orientation);
			bmVectorAdd(cameraEye,cameraEye,tazInstance->actorInstance.position);
			bmVectorCopy(cameraGarib,tazInstance->actorInstance.position);
		}
		else
		{
			VectorQuatMultiply(cameraEye,secondPlayerInstance->actorInstance.orientation);
			bmVectorAdd(cameraEye,cameraEye,secondPlayerInstance->actorInstance.position);
			bmVectorCopy(cameraGarib,secondPlayerInstance->actorInstance.position);
		}

		InitCamera(cameraInfo,cameraEye,cameraGarib);
		
		ZEROVECTOR(tempVector);

		CreateCameraCollSphere(cameraInfo,tempVector,METERTOUNIT(0.5f),METERTOUNIT(9.0f));
		
		if (i == 0)
			SetCameraLookAtFollowActorInstance(cameraInfo,tazInstance, &tazInstance->actorInstance,(TBActorNodeInstance *)NULL);
		else
			SetCameraLookAtFollowActorInstance(cameraInfo,secondPlayerInstance, &secondPlayerInstance->actorInstance,(TBActorNodeInstance *)NULL);
			
		SetFixedDistanceToLookAt(cameraInfo,cameraInfo->defaultFixedDistance);
		
		EnableCameraAngleAboveHorizontal(cameraInfo,RAD(0.0f));
		
		SetCameraSmooth(cameraInfo, defaultPosSmooth, defaultLookatSmooth);
	}

	// TP: get pointers to all animated textures
	ListAnimatingTextures();

	// TP: Setup players
	InitialisePlayers();

	tempVector[X] = 0;
	tempVector[Y] = METERTOUNIT(1);
	tempVector[Z] = 0;
	SetCameraLookAtActorInstanceOffset(cameraInfo,tempVector);

	SetCameraLookAtFollowActorInstance(&camera[0], tazInstance, &tazInstance->actorInstance,(TBActorNodeInstance *)NULL);
	SetCameraLookAtActorInstanceOffset(&camera[0],tempVector);
	SetupPlayer(0,tazInstance,&camera[0],&controller1);

	if (gameStatus.multiplayer.numPlayers==2)
	{
		if(secondPlayerInstance != NULL)// PP: TEST
		{
			SetCameraLookAtFollowActorInstance(&camera[1], secondPlayerInstance, &secondPlayerInstance->actorInstance,(TBActorNodeInstance *)NULL);
			SetCameraLookAtActorInstanceOffset(&camera[1],tempVector);
			SetupPlayer(1,secondPlayerInstance,&camera[1],&controller2);
		}
	}
}

