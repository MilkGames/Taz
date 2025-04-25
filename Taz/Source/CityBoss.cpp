
#include "global.h"
#include "main.h"

#include "actors.h"
#include "control.h"
#include "animation.h"
#include "physics.h"
#include "files.h"
#include "attach.h"
#include "util.h"
#include "status.h"
#include "bumper.h"
#include "collectibles.h"
#include "debris.h"
#include "icon.h"
#include "textures.h"
#include "display.h"
#include "player.h"
#include "genericai.h"
#include "gui.h"
#include "sfx.h"
#include "maths.h"
#include "multiplayer.h"
#include "atlassphere.h"
#include "bossgamecontinue.h"
#include "boss.h"
#include "cityboss.h"
#include "playerstats.h"

// CPW: #include "linkedlist.h"

#define MAGNETS
// CPW: #define MAGNET_DONTBLOW

// CPW: #define MAGNET_STRENGTH	(25000.0f)
#define MAGNET_PERIOD	(4.0f)

#define MAX_MUSHROOMS	(8)
#define MAX_TARGETS		(7)
#define MAX_MOUNDS		(4)

#define TIME_LIMIT		(120.0f)
// CPW: #define TIME_LIMIT		(10.0f)

#define DIST_TOUCHEDSTAR	(METERTOUNIT(0.5))

// CPW: Enemy skill 1.0 = perfect,  0.0 = complete vegetable
// CPW: Note that the scale here is far from linear, so 0.1 is still pretty shit-hot
// CPW: #define ENEMY_SKILL		(0.2f)
// CPW: #define ENEMY_SPEED		(METERTOUNIT(4.0f))

float enemySkill;// CPW:  = ENEMY_SKILL;
float enemySpeed;// CPW:  = ENEMY_SPEED;

static int zero = 0;
float	gameTimer;
float	timeWithoutAI;

// CPW: initialise our static counter
int ATLASSPHERE::instanceCtr = 0;
ATLASSPHERE **ATLASSPHERE::ppSphereList = NULL;

/*  --------------------------------------------------------------------------------
	Function	: cityBossForceEnd
	Purpose		: Force the end of a city boss game
	Parameters	: win- true/false
	Returns		: 
	Info		: 
*/

void CITYBOSSGAME::cityBossForceEnd(bool win)
{
	timer = 0.0f;
	gameTimer = TIME_LIMIT;
	if (win)
	{
		pcSphere[0]->pBoundToActor->actorStatus->multiplayer.numStars = 1;
		pcSphere[1]->pBoundToActor->actorStatus->multiplayer.numStars = 0;
	}
	else
	{
		pcSphere[0]->pBoundToActor->actorStatus->multiplayer.numStars = 0;
		pcSphere[1]->pBoundToActor->actorStatus->multiplayer.numStars = 1;
	}
}

#define FOUNTAIN_GLOW_RADIUS	(METERTOUNIT(0.25f))
#define FOUNTAIN_GLOW_ALPHA		(60)
#define FOUNTAIN_GLOW_LIFE		(3.0f)
#define FOUNTAIN_GLOW_SPEED		(METERTOUNIT(0.5f))
#define FOUNTAIN_GLOW_BALL_SIZE	(METERTOUNIT(0.5f))
#define FOUNTAIN_GLOW_BALL_VERT	(METERTOUNIT(2.0f))
#define FOUNTAIN_GLOW_BALL_TIME	(0.04f)

/* --------------------------------------------------------------------------------
   Function: CreateSpriteFountain
   Purpose : Creates bubbles which eminate from powerups
   Parameters : debris structure
   Returns : 
   Info : 
*/

void CreateSpriteFountain(TBVector pPosition, int *pGlowColour, float glowSpeed)
{
	TBVector		velocity,position,tempVelocity;
	TBVector		rotation = {0.0f, 0.0f, 0.0f, 1.0f};
	float			angle, life, size, zero, alpha;

	if(texturePowerUpGlow)
	{
		angle = RAD((float)(bmRand()%360));
		//set velocity to be a general radial drift
		velocity[0] = glowSpeed * bmCos(angle);
		velocity[1] = 0.0f;
		velocity[2] = glowSpeed * bmSin(angle);

		//find start position
		bmVectorCopy(position, pPosition);
		// TP: position[1] += METERTOUNIT(0.25f);
		// TP: position[1] += METERTOUNIT(1.0f);

		// TP: position[2] += METERTOUNIT(2.0f);

		//alter sprite position a little
		bmVectorCDiv(tempVelocity, velocity, 10.0f);
		bmVectorAdd(position, tempVelocity, position);

		//add y componant of velocity
		velocity[1] = FOUNTAIN_GLOW_BALL_VERT;

//		baCreateExtra(tem,NULL,texture,NULL,position,velocity,rotation, FOUNTAIN_GLOW_LIFE,
//						0.0f, FOUNTAIN_GLOW_BALL_SIZE, FOUNTAIN_GLOW_BALL_SIZE, pGlowColour[3],
//						DEBRISFLAG_BUBBLES|DEBRISFLAG_POWERUPGLOW, 0, 0, 0.0f,
//						pGlowColour[0], pGlowColour[1], pGlowColour[2]);
		life = FOUNTAIN_GLOW_LIFE;
		size = FOUNTAIN_GLOW_BALL_SIZE;
		zero = 0.0f;
		alpha = (float)pGlowColour[3];
		baCreateExtra(bubbleSystem,position,velocity,texturePowerUpGlow,&life,&size,&zero,
							pGlowColour[0],pGlowColour[1],pGlowColour[2],&alpha,DEBRISFLAG_BUBBLES|DEBRISFLAG_POWERUPGLOW, &zero);
	}
}

CITYBOSSGAME		CityBoss_game;

#define TB_TIMER			(0)
#define TB_COLLECTED		(1)
#define TB_TAZWIN			(2)
#define TB_DAFFYWIN			(3)
#define TB_STARTTOCOLLECT	(4)
#define TB_NEEDTOCOLLECT	(5)

void cityBossTouchedPad(int starNumber)
{
	int player;
	ACTOR_STATUS	*pStatus;
	ACTORINSTANCE *pActorInstance;// CPW:  = FindActorInstanceInMap(message, 0, &map);
	char sPadName[32];

	// PP: if we're in the boss-game continue sequence,
	// PP: we don't want anything to happen when a pad is touched
	if(CityBoss_game.inContinue)
	{
		return;
	}

	sprintf(sPadName, "star_panel%02d", starNumber);
	pActorInstance = FindActorInstanceInMap(sPadName, 0, &map);
	ASSERTM(pActorInstance, "Can't find actor");

	if(gameStatus.multiplayer.numPlayers == 1)
	{
		player = 0;
	}
	else
	{
		float d0 = bmVectorDistance(pActorInstance->actorInstance.position, CityBoss_game.pcSphere[0]->pBoundToActor->actorInstance.position);
		float d1 = bmVectorDistance(pActorInstance->actorInstance.position, CityBoss_game.pcSphere[1]->pBoundToActor->actorInstance.position);

		if(d0 < d1)
		{
			player = 0;
			bkPrintf("Player 0 has hit %d\n", starNumber);
		}
		else
			player = 1;
	}

	CityBoss_game.fountainProb = 1;
	CityBoss_game.fountainNum = starNumber;

	PlayTrackingSample("pickup.wav", 200, pActorInstance->actorInstance.position);

	pStatus = CityBoss_game.pcSphere[player]->pBoundToActor->actorStatus;

	// CPW: starNumber = utilBaseStringtoLong(message + strlen("star_panel_lightbeam"), BASE_DECIMAL, 2);
	pStatus->multiplayer.thisStar = starNumber;
	pStatus->multiplayer.thisPlayer = player;

	{
		int oldState = CityBoss_game.starTarget[pStatus->multiplayer.thisStar].state;
		int newState = player;

		// CPW: set state to say whether its been hit by player 1 or 2 (green or red)
		CityBoss_game.starTarget[pStatus->multiplayer.thisStar].state = newState;
		pStatus->multiplayer.numStars++;

		CityBoss_game.pcSphere[0]->pBoundToActor->actorStatus->multiplayer.numStars = 0;
		CityBoss_game.pcSphere[1]->pBoundToActor->actorStatus->multiplayer.numStars = 0;

		for(int i=0; i<MAX_TARGETS; i++)
		{
			int state = CityBoss_game.starTarget[i].state;

			if(state == 0)
				CityBoss_game.pcSphere[0]->pBoundToActor->actorStatus->multiplayer.numStars++;

			if(state == 1)
				CityBoss_game.pcSphere[1]->pBoundToActor->actorStatus->multiplayer.numStars++;
		}

		CityBoss_game.pcSphere[0]->pBoundToActor->actorStatus->multiplayer.pTextBox[TB_COLLECTED]->sprintf8("%d", CityBoss_game.pcSphere[0]->pBoundToActor->actorStatus->multiplayer.numStars);
		CityBoss_game.pcSphere[1]->pBoundToActor->actorStatus->multiplayer.pTextBox[TB_COLLECTED]->sprintf8("%d", CityBoss_game.pcSphere[1]->pBoundToActor->actorStatus->multiplayer.numStars);

		if(player == 0)
		{
			// PP: this probably wants to be quiet enough that we can hear what Daffy's saying
			PlaySample("applause.wav", 200);// PP: was "cheer.wav" (a stub I assume - doesn't exist)

			if (bmRand() % 2)
			{
				if (characterSounds.daffy == -1)
				{
					switch (bmRand()%3)
					{
					case 0:	CallbackWhenSampleFinished((characterSounds.daffy=PlaySample("daffygen_1.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.daffy);	break;
					case 1:	CallbackWhenSampleFinished((characterSounds.daffy=PlaySample("daffygen_4.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.daffy);	break;
					case 2:	CallbackWhenSampleFinished((characterSounds.daffy=PlaySample("daffygen_8.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.daffy);	break;
					}
				}
			}
		}
		else
		{
			PlaySample("booo.wav", 255);

			if (bmRand() % 2)
			{
				if (characterSounds.daffy == -1)
				{
					switch (bmRand()%3)
					{
					case 0:	CallbackWhenSampleFinished((characterSounds.daffy=PlaySample("daffygen_14.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.daffy);	break;
					case 1:	CallbackWhenSampleFinished((characterSounds.daffy=PlaySample("daffygen_13.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.daffy);	break;
					case 2:	CallbackWhenSampleFinished((characterSounds.daffy=PlaySample("daffygen_12.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.daffy);	break;
					}
				}
			}
		}

		// CPW: this bit used to be somewhere else

		if((player>0) && gameStatus.multiplayer.versusCPU)
		{
			{
				if (CityBoss_game.pcSphere[player]->homeInPoint==starNumber)
				{
					//CityBoss_game.pcSphere[player]->homeInPoint++;

					//if(CityBoss_game.pcSphere[player]->homeInPoint >= MAX_TARGETS)
						CityBoss_game.pcSphere[player]->homeInPoint = 0;
				}
			}
		}
	}

	pStatus->multiplayer.lastStar = pStatus->multiplayer.thisStar;
	pStatus->multiplayer.lastPlayer = pStatus->multiplayer.thisPlayer;

	RequestRumbleEffect(gameStatus.player[pStatus->multiplayer.thisPlayer].controller, 180, 0.0f, 0.5f, 0.75f, 0.0f, WAVEFORM_NORMAL);
}


/* --------------------------------------------------------------------------------
   Function		: CITYBOSSGAME::fadeDownCallback
   Purpose		: function called when the fade-down, before the continue sequence, finishes
   Parameters	: 
   Returns		: 
   Info			: // PP: must match the BossGameFadeDownCallback typedef (bossgamecontinue.h)
*/
void CITYBOSSGAME::fadeDownCallback(void)
{
	// PP: The fade-down has finished and we're about to start the continue sequence

	// PP: For all intents & purposes CITYBOSSGAME is an abstract class - there's never going to be more than one of them.
	// PP: However, it's more convenient to have a single CITYBOSSGAME instance than to declare EVERYTHING in the class as static.
	// PP: So there is nothing dirty about referring to the 'CityBoss_game' instance inside this static method!  It's the only instance there is and you know it!

	CityBoss_game.inContinue=true;// PP: we're now in the boss-game continue sequence
}


void CITYBOSSGAME::shoveItUp(ACTORINSTANCE *pMushroomInstance)
{
	PlayAnimationByName(&pMushroomInstance->actorInstance, "bumperup", 4.0f, 0, 1, 0.0f, NONE);
	pMushroomInstance->bumperInfo->isUp = 1;
}

void CITYBOSSGAME::pullItDown(ACTORINSTANCE *pMushroomInstance)
{
	PlayAnimationByName(&pMushroomInstance->actorInstance, "bumperdown", 4.0f, 0, 1, 0.0f, NONE);
	pMushroomInstance->bumperInfo->isUp = 0;
}

void CITYBOSSGAME::toggleIt(ACTORINSTANCE *pMushroomInstance)
{
	if(pMushroomInstance->bumperInfo->isUp)
		pullItDown(pMushroomInstance);
	else
		shoveItUp(pMushroomInstance);
}

/*	--------------------------------------------------------------------------------
	Function 	: CITYBOSSGAME::init
	Purpose 	: initialise the City Boss game
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides BOSSGAME::init
*/
void CITYBOSSGAME::init(void)
{
	char *sMushObeName = "mushroom_bumper";
	char *sTargetObeName = "star_panel_lightbeam";
	char buf[128];

	char *psMSNames[] = {
		"bumperdown",
		"bumperup"
	};

	int p;

	timeWithoutAI = -1.0f;

	gameTimer = 0;

	levelData.RaceState = Intro;

	BOSSGAME::init();

	mpCountdownSetup(SCENE_CITYBOSS);

	LoadActor("extras\\rollerball.obe", map.mapName);

	for(p=0; p<2; p++)
	{
		pcSphere[p] = new ATLASSPHERE(gameStatus.player[p].actorInstance, "extras\\rollerball.obe");
		
		ASSERTM(pcSphere[p], "Couldn't allocate new ATLASSPHERE");

		pcSphere[p]->ballColour = p;
		
		SetDefaultCameraAngleAboveHorizontal(gameStatus.player[p].camera, RAD(25.0f));
	}

	switch(gameStatus.globalSettings.difficulty)
	{
	case DIFF_EASY:
		enemySkill = 0.2f;
		enemySpeed = METERTOUNIT(4.0f);
		break;

	case DIFF_MEDIUM:
		enemySkill = 0.2f;
		enemySpeed = METERTOUNIT(4.0f);
		break;

	case DIFF_HARD:
		enemySkill = 0.3f;
		enemySpeed = METERTOUNIT(5.0f);
		break;
	}


	int m, n;

	char sModel[32];
	ACTORINSTANCE *pFlashPosActor;

	LoadActor("extras\\cam_flash_anim.obe", map.mapName);

	for(m=0; m < NUM_FLASHES; m++)
	{
		n=m+1;

		if(n<10)
			sprintf(sModel, "camera_flash_null0%d", n);// CPW: this is necessary becuse MAX is a bit strange
		else
			sprintf(sModel, "camera_flash_null%d", n);

		pFlashPosActor = FindActorInstanceInMap(sModel, 0, &map);

		ASSERTM(pFlashPosActor, "Can't find flash position boxy thing");

		this->ppCameraFlash[m] = CreateActorInstance(&map.animatedInstances, "extras\\cam_flash_anim.obe", "cameraflash");
		ASSERTM(this->ppCameraFlash[m], "Couldn't find cameraflash");

		SETVECTOR(this->ppCameraFlash[m]->actorInstance.scale,3.0f,3.0f,3.0f,1.0f);
		this->ppCameraFlash[m]->flags = ACTORFLAG_DONTFADE;

		bmVectorCopy(this->ppCameraFlash[m]->actorInstance.position, pFlashPosActor->actorInstance.position);
		PlayAnimationByName(&this->ppCameraFlash[m]->actorInstance, "idle", 1.0f, 0, 0, 0.0f, NONE);
	}

	// CPW: initialise mushroom bumpers! (0-8)
	for(m=0; m<8; m++)
	{
		char sInstName[32];
		ACTORINSTANCE *pMushroomInstance;

		sprintf(sInstName, "%s%02d", sMushObeName, m+1);
		pMushroomInstance = FindActorInstanceInMap(sInstName, 0, &map);
		ASSERTM(pMushroomInstance, "Couldn't find mushroom");

		ppMushroom[m] = pMushroomInstance;

		ASSERTM(pMushroomInstance->bumperInfo, "Mushroom is not a bumper!");

		for(int a=0; a<2; a++)
		{
			pMushroomInstance->bumperInfo->ppAnimSeg[a] = baFindAnimSegment(&pMushroomInstance->actorInstance, psMSNames[a]);
			ASSERT(pMushroomInstance->bumperInfo->ppAnimSeg[a]);
		}

		pullItDown(pMushroomInstance);
	}

	{
		char sInstName[32];
		ACTORINSTANCE *pMoundInstance;

		pMoundInstance = FindActorInstanceInMap("centre_mound00", 0, &map);
		ASSERT(pMoundInstance);

		this->ppMoundPos[0] = &pMoundInstance->actorInstance.position;

		for(m=0; m<MAX_MOUNDS; m++)
		{
			sprintf(sInstName, "floor_mounds%02d", m);
			pMoundInstance = FindActorInstanceInMap(sInstName, 0, &map);
			ASSERT(pMoundInstance);

			this->ppMoundPos[m+1] = &pMoundInstance->actorInstance.position;
		}
	}

	for(m=0; m<MAX_TARGETS; m++)
	{
		char sInstName[32];
		ACTORINSTANCE *pStarTargetInstance, *pDummyTargetInstance;
		
		sprintf(sInstName, "%s%02d", sTargetObeName, m);
		pStarTargetInstance = FindActorInstanceInMap(sInstName, 0, &map);
		ASSERT(pStarTargetInstance);

		pStarTargetInstance->flags|=ACTORFLAG_CUSTOMDRAW;
		starTarget[m].pStarTarget = pStarTargetInstance;
		starTarget[m].state = -1;
		starTarget[m].notTarget = 1;

		switch(m)
		{
		case 4:
			pDummyTargetInstance = FindActorInstanceInMap("dummy_target00", 0, &map);
			break;
		case 5:
			pDummyTargetInstance = FindActorInstanceInMap("dummy_target01", 0, &map);
			break;
		case 6:
			pDummyTargetInstance = FindActorInstanceInMap("dummy_target02", 0, &map);
			break;
		default:
			pDummyTargetInstance = pStarTargetInstance;
			starTarget[m].notTarget = 0;
		}


		ASSERTM(pDummyTargetInstance, "Can't find dummy_target");
		bmVectorCopy(starTarget[m].headFor, pDummyTargetInstance->actorInstance.position);
	}


	pLightTexture = bkLoadTexture(NULL, "textures\\trafficlights.gif", BDEFAULTGROUP);
	yelTexture = bkLoadTexture(NULL, "textures\\yellowlight.gif", BDEFAULTGROUP);
	greTexture = bkLoadTexture(NULL, "textures\\greenlight.gif", BDEFAULTGROUP);
	redTexture = bkLoadTexture(NULL, "textures\\redlight.gif", BDEFAULTGROUP);
	
	ASSERT(pLightTexture);

	this->startTime = gameStatus.gameTime;

	//Do Book stuff
	{
	ACTOR_STATUS	*pStatus = pcSphere[0]->pBoundToActor->actorStatus;
	TEXTBOX			*tb, **pTextBox = pStatus->multiplayer.pTextBox;
	PAGE			*pPage;
	ICON			*pIcon;

	pStatus->multiplayer.lastStar = -1;
	pStatus->multiplayer.numStars = 0;

	// CPW: open book
	pStatus->multiplayer.pStatsBook = new BOOK();	// PP: NOTE: PROBABLY LONG FIXED!// TP: Strange problems when this is BOOK(true)
	
	pPage = pStatus->multiplayer.pStatsBook->addPage(pIcon = new ICON("extras\\tazatlas.bmp", "TAZATLAS"));
	ASSERT(pPage);

	//pIcon->setVGrad(false);
	pIcon->setXAlign(PIFLAG_XALIGN_LEFT);
	pIcon->setYAlign(PIFLAG_YALIGN_TOP);
	pIcon->setXPadding(false);

	pPage->insertItem(pTextBox[TB_COLLECTED] = new TEXTBOX(),false);// PP: false = same row
	pTextBox[TB_COLLECTED]->setXAlign(PIFLAG_XALIGN_LEFT);
	pTextBox[TB_COLLECTED]->setYAlign(PIFLAG_YALIGN_TOP);
	pTextBox[TB_COLLECTED]->setXPadding(true);
	pTextBox[TB_COLLECTED]->setScale(2.5f);
	pTextBox[TB_COLLECTED]->setColour(COLOUR(0, 96, 0), COLOUR(0, 128, 0));
	pTextBox[TB_COLLECTED]->sprintf8("%d", pStatus->multiplayer.numStars);

	{
		ACTOR_STATUS	*pStatus = pcSphere[1]->pBoundToActor->actorStatus;
		
		pTextBox = pStatus->multiplayer.pTextBox;
		pStatus->multiplayer.lastStar = -1;
		pStatus->multiplayer.numStars = 0;
		
		pPage->insertItem(pTextBox[TB_COLLECTED] = new TEXTBOX(), false);
		pTextBox[TB_COLLECTED]->setXAlign(PIFLAG_XALIGN_RIGHT);
		
		if (!gameStatus.multiplayer.versusCPU)
			pTextBox[TB_COLLECTED]->setYAlign(PIFLAG_YALIGN_BOTTOM);
		else
			pTextBox[TB_COLLECTED]->setYAlign(PIFLAG_YALIGN_TOP);
		
		pTextBox[TB_COLLECTED]->setScale(2.5f);
		pTextBox[TB_COLLECTED]->setColour(COLOUR(96, 0, 0), COLOUR(128, 0, 0));
		pTextBox[TB_COLLECTED]->sprintf8("%d", pStatus->multiplayer.numStars);

		pPage->insertItem(pIcon = new ICON("extras\\daffyatlas.bmp", "DAFFYATLAS"), false);
		pIcon->setXAlign(PIFLAG_XALIGN_RIGHT);

		if (!gameStatus.multiplayer.versusCPU)
			pIcon->setYAlign(PIFLAG_YALIGN_BOTTOM);
		else
			pIcon->setYAlign(PIFLAG_YALIGN_TOP);
		
		pIcon->setXPadding(false);
	}

	// CPW: You (Taz) wins
	pPage = pStatus->multiplayer.pStatsBook->addPage("TazWin");
	ASSERTM(pPage, "Couldn't add page");
	
	pPage->insertItem(pIcon = new ICON("extras\\tazatlas.bmp", "TAZATLASL"));// PP: true = new row
	pIcon->setXAlign(PIFLAG_XALIGN_RIGHT);
	
	pPage->insertItem(pTextBox[TB_TAZWIN] = new TEXTBOX(STR_CITYBOSS_YOUWIN), false);
	pTextBox[TB_TAZWIN]->setScale(2.0f);
	pTextBox[TB_TAZWIN]->setXAlign(PIFLAG_XALIGN_CENTRE);
	pTextBox[TB_TAZWIN]->setXPadding(false);
	
	pPage->insertItem(pIcon = new ICON("extras\\tazatlas.bmp", "TAZATLASR"), false);// PP: true = new row
	pIcon->setXAlign(PIFLAG_XALIGN_LEFT);

	// CPW: Daffy Wins (You lose)
	pPage = pStatus->multiplayer.pStatsBook->addPage("DafWin");
	ASSERTM(pPage, "Couldn't add page");
	
	pPage->insertItem(pIcon = new ICON("extras\\daffyatlas.bmp", "DAFATLASL"));// PP: true = new row
	pIcon->setXAlign(PIFLAG_XALIGN_RIGHT);
	
	pPage->insertItem(pTextBox[TB_DAFFYWIN] = new TEXTBOX(STR_CITYBOSS_DAFFYWINS), false);
	pTextBox[TB_DAFFYWIN]->setScale(2.0f);
	pTextBox[TB_DAFFYWIN]->setXAlign(PIFLAG_XALIGN_CENTRE);
	pTextBox[TB_DAFFYWIN]->setXPadding(false);
	
	pPage->insertItem(pIcon = new ICON("extras\\daffyatlas.bmp", "DAFATLASR"), false);// PP: true = new row
	pIcon->setXAlign(PIFLAG_XALIGN_LEFT);
	}

	this->pElmer = LoadElmer;
	ASSERTM(this->pElmer, "Couldn't find Mr. Fudd");

	setBumperIgnoreCharacter(this->pElmer);

	// CPW: put Mr. Fudd on mound 1
	this->elmerMound = 2;
	bmVectorCopy(this->pElmer->actorInstance.position, *this->ppMoundPos[this->elmerMound]);

	this->extraTime = 0;
	this->whenToQuit = 0.0f;
	this->levelFinished = 0;

	this->fountainProb = 0;

	// CPW: find some anims
	this->animSegs.elmer.pSquashed = baFindAnimSegment(&this->pElmer->actorInstance, "squashed");
	ASSERTM(this->animSegs.elmer.pSquashed, "Can't find elmer's 'squashed' anim\n");

	this->animSegs.elmer.pUnSquash = baFindAnimSegment(&this->pElmer->actorInstance, "unsquash");
	ASSERTM(this->animSegs.elmer.pUnSquash, "Can't find elmer's 'unsquash' anim\n");

	this->animSegs.elmer.pStaySquashed = baFindAnimSegment(&this->pElmer->actorInstance, "staysquashed");
	ASSERTM(this->animSegs.elmer.pStaySquashed, "Can't find elmer's 'staysquashed' anim\n");

	this->animSegs.elmer.pWalk = baFindAnimSegment(&this->pElmer->actorInstance, "walk1");
	ASSERT(this->animSegs.elmer.pWalk);

	this->animSegs.elmer.pRun = baFindAnimSegment(&this->pElmer->actorInstance, "run");
	ASSERT(this->animSegs.elmer.pRun);

	this->animSegs.elmer.pIdle = baFindAnimSegment(&this->pElmer->actorInstance, "idle1");
	ASSERT(this->animSegs.elmer.pIdle);

	this->animSegs.sphere.pRunSphere = baFindAnimSegment(&pcSphere[0]->pBoundToActor->actorInstance, "runsphere");
	ASSERT(this->animSegs.sphere.pRunSphere);

	this->animSegs.sphere.pStaticSphere = baFindAnimSegment(&pcSphere[0]->pBoundToActor->actorInstance, "spherestatic");
	ASSERT(this->animSegs.sphere.pStaticSphere);

	this->animSegs.sphere.pRunSphereReverse = baFindAnimSegment(&pcSphere[0]->pBoundToActor->actorInstance, "runspherereverse");
	ASSERT(this->animSegs.sphere.pRunSphereReverse);

	PlayAnimationByName(pcSphere[1]->pBoundToActor,"spherestatic",1.0f,1,0,0.0f,NONE);

	levelData.RaceState = Intro;
	inContinue=false;									// PP: we're not in the boss-game continue sequence
	gameStatus.multiplayer.countingDown = FALSE;
	gameStatus.controlSuspend = TRUE;

}

/*	--------------------------------------------------------------------------------
	Function 	: ZOOBOSSGAME::draw
	Purpose 	: draw the zoo boss
	Returns 	:  game
	Parameters 	: 
	Info 		: this overrides BOSSGAME::draw
*/
void CITYBOSSGAME::draw2(void)
{	
	TBSavedRenderStates  saveState;

	if (!this->initialised) return;

	bdSaveRenderStates(&saveState);

	if(!CURRENTLY_DRAWING_FULL_SCREEN || gameStatus.multiplayer.numScreens == 1)
	{
		int i;

		for(i=0; i<gameStatus.multiplayer.numPlayers; i++)
			pcSphere[i]->draw();
		
		for(i=0; i<MAX_TARGETS; i++)
		{
			if(starTarget[i].pStarTarget->flags & ACTORFLAG_CUSTOMDRAW)
			{
				bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
				bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
				bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
				bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
				bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
				bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);

				switch(starTarget[i].state)
				{
				case -1:	SetTextureFrameManually(pLightTexture, 1);	break;
				case 0:		SetTextureFrameManually(pLightTexture, 0);	break;
				case 1:		SetTextureFrameManually(pLightTexture, 2);	break;
				}
				
				//bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);// PP: was false
				baDrawActorInstance(&starTarget[i].pStarTarget->actorInstance, BSETPRIMALPHATYPE_ALPHA);
			}
		}
	}

	if(CURRENTLY_DRAWING_FULL_SCREEN)
	{
		BOSSGAME::draw2();
		
		// CPW: draw Countdown			
		switch (levelData.RaceState)
		{
		case Countdown:	mpCountdownDraw();
						break;

		case Race:		{
							int		intaT = (int) actualTime;
							float	faT = actualTime - (float) intaT;

							if ((actualTime > 10.0) || (faT > 0.5 && faT < 0.75) || (faT > 0.0f && faT < 0.25) || (actualTime < 0.0f))
								mpDrawBook();
						}
						
/* PP: REMOUT: Now handled by the Book List				case TimeUp:	
						// PP: set an identity object matrix
						bdSetIdentityObjectMatrix();
						bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);

						bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
						bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
						bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);

						pcSphere[0]->pBoundToActor->actorStatus->multiplayer.pStatsBook->draw();
						//pcSphere[0]->pBoundToActor->actorStatus->multiplayer.pStatsBook->drawDebug();

						bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
						bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
						
						break;	*/
		}
	}

	bdRestoreRenderStates(&saveState);
}

/*	--------------------------------------------------------------------------------
	Function 	: ZOOBOSSGAME::update
	Purpose 	: update the Zoo Boss game
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides BOSSGAME::update
*/

void CITYBOSSGAME::update(void)
{
	BG_START_UPDATE;

	int p;
	TBActorAnimSegment *pRunSphere, *pStaticSphere, *pRunSphereReverse, *pCurrentAnim;
	TBVector velBackup[2];

	timer = TIME_LIMIT - gameTimer;

	actualTime = timer;

/* PP: REMOUT: Now handled by the Book List			raceBook->update();
	pcSphere[0]->pBoundToActor->actorStatus->multiplayer.pStatsBook->update();*/

	//Get animations for actors inside spheres
	pRunSphere = this->animSegs.sphere.pRunSphere;
	pStaticSphere = this->animSegs.sphere.pStaticSphere;
	pRunSphereReverse = this->animSegs.sphere.pRunSphereReverse;

	pcSphere[0]->beforeYouCallAnythingElseCallThis();
	pcSphere[1]->beforeYouCallAnythingElseCallThis();

	pcSphere[0]->calculateRoll();
	pcSphere[1]->calculateRoll();

	int m;

	for(m=0; m<NUM_FLASHES; m++)
	{
		if((bmRand() & 127) == 0)
		{
			PlayAnimationByName(&this->ppCameraFlash[m]->actorInstance, "flash", 1.0f, 0, 0, 0.0f, NONE);
			FacePoint(this->ppCameraFlash[m], gameStatus.player[0].camera->cameraState.cameraPos, 0.0f);




		}
	}

	// PP: (shifted some update to below the 'levelData.RaceState' switch)

	switch(levelData.RaceState)
	{
	case Finished:
		{
			// PP: are we in the boss-game continue sequence?
			if(this->inContinue)
			{
				// PP: We shouldn't be doing any update of the City Boss game during the continue sequence
				return;
			}
		}
		break;

	case Intro:		if (gameStatus.currentState == GAMESTATE_INGAME  && camera[0].fade.GetFadeState() == FADESTATE_OFF)
					{
						gameStatus.player2->flags &= ~ACTORFLAG_DONTDRAW;

						gameStatus.controlSuspend = TRUE;
						
						pcSphere[0]->pBoundToActor->actorBehaviour->physics.velocity[X] = 0;
						pcSphere[0]->pBoundToActor->actorBehaviour->physics.velocity[Z] = 0;
				
						pcSphere[1]->pBoundToActor->actorBehaviour->physics.velocity[X] = 0;
						pcSphere[1]->pBoundToActor->actorBehaviour->physics.velocity[Z] = 0;
				
						bkPrintf("Pressed Start\n");
								
						levelData.RaceState = Countdown;
						gameStatus.multiplayer.countingDown = TRUE;
						raceBook->gotoPage("5", true);
						raceBook->open();

						PlaySample("citybosscount_5.wav", 255);
					}
					
					break;

	case Countdown:	pcSphere[0]->pBoundToActor->actorBehaviour->physics.velocity[X] = 0;
					pcSphere[0]->pBoundToActor->actorBehaviour->physics.velocity[Z] = 0;
			
					pcSphere[1]->pBoundToActor->actorBehaviour->physics.velocity[X] = 0;
					pcSphere[1]->pBoundToActor->actorBehaviour->physics.velocity[Z] = 0;
					
					if (!mpCountdownUpdate())
					{
						gameStatus.controlSuspend = FALSE;
						levelData.RaceState = Race;

						startTime = gameStatus.gameTime;

						pcSphere[0]->pBoundToActor->actorStatus->multiplayer.pStatsBook->open();
						
						raceBook->gotoPage("Timer", true);
					}
				
					break;

	case Race:		gameTimer += fTime;
		
					for(p=0; p<gameStatus.multiplayer.numPlayers; p++)
					{
						//If player 2 is human, use your physics
						//if((p>0) && (!gameStatus.multiplayer.versusCPU))
						//	bmVectorCopy(velBackup[p], pcSphere[p]->pBoundToActor->actorBehaviour->physics.velocity);
						
						//If you're a computer player		
						if((p>0) && gameStatus.multiplayer.versusCPU && levelData.RaceState == Race)
						{			
							//pcSphere[p]->originSetNormal(pcSphere[p]->pBoundToActor);

							if (pcSphere[p]->homeInPoint == -1)
								bmVectorCopy(pcSphere[p]->homeInOn, pcSphere[0]->pBoundToActor->actorInstance.position);
							else
								bmVectorCopy(pcSphere[p]->homeInOn, starTarget[pcSphere[p]->homeInPoint].headFor);
								
							if((gameStatus.gameTime - pcSphere[p]->pBoundToActor->actorStatus->timeOfLastSpeedBoost) > 12.0f)
							{
								pcSphere[p]->calculateAI(enemySpeed, enemySkill);

								DoDaffyAI();

								// JW: Trying to make some better daffy AI code
							}
						}
							
						//If we've got more than one player work out the collisions
						if(gameStatus.multiplayer.numPlayers > 1)
							pcSphere[p]->calculateSphereCollision();
							
						//Find our distance to Elmer
						float refDist = bmVectorDistance(this->pElmer->actorInstance.position, pcSphere[p]->pBoundToActor->actorInstance.position);
						
						//Get Elmers animations
						TBActorAnimSegment *pCurrentElmerAnim = CurrentAnimation(this->pElmer);
						TBActorAnimSegment *pSquashed = this->animSegs.elmer.pSquashed;
						TBActorAnimSegment *pUnSquash = this->animSegs.elmer.pUnSquash;
						TBActorAnimSegment *pStaySquashed = this->animSegs.elmer.pStaySquashed;
						TBActorAnimSegment *pWalk = this->animSegs.elmer.pWalk;
							
						// CPW: handle any collision with the referee
						if(refDist < METERTOUNIT(3.0f))
						{	// CPW: we want to make elmer run away from the ball, so he gets run over "nicely"
							
							TBVector elmerFace;
							if((pCurrentElmerAnim != pSquashed) && (pCurrentElmerAnim != pStaySquashed) && (pCurrentElmerAnim != pUnSquash))
							{
								bmVectorSub(elmerFace, this->pElmer->actorInstance.position, pcSphere[p]->pBoundToActor->actorInstance.position);
								bmVectorAdd(elmerFace, elmerFace, this->pElmer->actorInstance.position);
								FacePoint(this->pElmer, elmerFace, 1.0f);// CPW: float offset);
								
								if(pCurrentElmerAnim != pWalk)	// am i already playing this anim?
								{
									PlayAnimationBySegmentPointer(&this->pElmer->actorInstance, pWalk, 1.0f, 0, 0, 0.0f, NONE);
								}
							}
						
							if(refDist < METERTOUNIT(1.0f))
							{
								if((pCurrentElmerAnim != pSquashed) && (pCurrentElmerAnim != pStaySquashed))	// am i already playing this anim?
								{
									PlayAnimationBySegmentPointer(&this->pElmer->actorInstance, pSquashed, 1.0f, 0, 0, 0.0f, NONE);
									PlayAnimationBySegmentPointer(&this->pElmer->actorInstance, pStaySquashed, 1.0f, 1, 1, 0.0f, NONE);// CPW: loop & queue
										
									PlaySample("poich!.wav",190);		// PP: "POICH!"
									// PP: REMOUT: sample doesn't exist (we have no Elmer sounds)	PlaySample("elmeryelp.wav", 255);
									
									bkPrintf("*** Ran over ref! RED CARD!!! ***\n");
									pcSphere[p]->justRanOverRef++;
								}
							}
						}
							
						if(refDist > METERTOUNIT(1.0f))
						{
							if((pCurrentElmerAnim == pStaySquashed) && pcSphere[p]->justRanOverRef) // am i already playing this anim?
							{
								PlayAnimationBySegmentPointer(&this->pElmer->actorInstance, pUnSquash, 1.0f, 0, 0, 0.0f, NONE);
								pcSphere[p]->justRanOverRef=0;
							}
						}
					}
		
					if(timer < 0.0f)
					{						
						tazScore	= pcSphere[0]->pBoundToActor->actorStatus->multiplayer.numStars;
						daffyScore	= pcSphere[1]->pBoundToActor->actorStatus->multiplayer.numStars;

						if(tazScore == daffyScore)
						{// CPW: it's a draw
							if(this->extraTime == 0)
							{
								this->extraTime = 1;
								
								raceBook->gotoPage("SuddenDeath", true);
								PlaySample("extratime.wav", 255);
							}
						}
						else
						{
							raceBook->close();

							if(this->levelFinished == 0)
							{
								this->levelFinished = 1;
								this->whenToQuit = timer - 7.0f;
							}

							levelData.RaceState = TimeUp;

							if(tazScore > daffyScore)
							{
								pcSphere[0]->pBoundToActor->actorStatus->multiplayer.pStatsBook->gotoPageNumber(1);

								if (characterSounds.sam == -1)
								{
									CallbackWhenSampleFinished((characterSounds.sam=PlaySample("elmertazwins_1.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.sam);
								}
							}			
							else
							{
								if (characterSounds.sam == -1)
								{
									CallbackWhenSampleFinished((characterSounds.sam=PlaySample("elmerdaffywins_1.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.sam);
								}

								pcSphere[0]->pBoundToActor->actorStatus->multiplayer.pStatsBook->gotoPageNumber(2);							
							}	
						}
					}

					break;

	case TimeUp:	gameTimer += fTime;

					if (!gameStatus.multiplayer.versusCPU)
					{
						pcSphere[0]->pBoundToActor->actorStatus->multiplayer.pStatsBook->close();
							
						int32	winner;

						if (tazScore > daffyScore)
						{
							winner=0;
							gameStatus.player1->actorStatus->multiplayer.position = 0;
							gameStatus.player2->actorStatus->multiplayer.position = 1;
						}
						else
						{
							winner=1;
							gameStatus.player1->actorStatus->multiplayer.position = 1;
							gameStatus.player2->actorStatus->multiplayer.position = 0;
						}
						
						mpEndGameMenuSetUp(&this->levelFinished, GAMEOVER_NOTFINISHED, winner);
						SetGameState(&gameStatus, GAMESTATE_CUSTOMPAUSE, GAMESUBSTATE_NORMAL, (int32)mpEndGameMenuHandler);

						if (!gameStatus.player1->actorStatus->multiplayer.position)
						{
							camera[1].fade.StartFadeDown(FADETYPE_NORMAL, 1.5f);
						}
						else
						{
							camera[0].fade.StartFadeDown(FADETYPE_NORMAL, 1.5f);
						}

						// TP: change to frontend
//						bkGenerateEvent("scenechange","_frontend");

						levelData.RaceState = Finished;
					}
					else 
					{
						if ((timer < this->whenToQuit) && (this->levelFinished == 1))
						{
							// TP: single player

							// TP: set this so the doesn't call the scene change more that once
							levelData.RaceState = Finished;

							this->levelFinished = 2;

							raceBook->close();

							gameStatus.player1->actorStatus->multiplayer.pStatsBook->close();
							
							setNumScreens(1);
							
							if (tazScore > daffyScore)
							{
								LevelCompleted(gameStatus.player1,LEVELCOMPLETE_NOSTATS);
							}
							else
							{
								DisableRumble(&controller1);

								ProcessTazDeathAtEndOfBossGame(ResetCityBossCallback, NULL, NULL, CITYBOSSGAME::fadeDownCallback);

								//bkGenerateEvent("scenechange","_cityhub");
							}
						}

						if (timer < (this->whenToQuit + 4) && characterSounds.daffy == -1)
						{// JW: Let Daffy whine (or gloat) a bit							
							if(tazScore > daffyScore)
							{
								switch(bmRand()%3)
								{
								case 0:	characterSounds.daffy = PlaySample("daffyloses_1.wav",255);	break;
								case 1:	characterSounds.daffy = PlaySample("daffyloses_1.wav",255);	break;
								case 2:	characterSounds.daffy = PlaySample("daffyloses_1.wav",255);	break;
								}
							}
							else
							{
								switch(bmRand()%3)
								{
								case 0: characterSounds.daffy = PlaySample("daffywins_1.wav",255);	break;
								case 1:	characterSounds.daffy = PlaySample("daffywins_1.wav",255);	break;
								case 2:	characterSounds.daffy = PlaySample("daffywins_1.wav",255);	break;
								}
							}
						}
					}

					break;
	}

	// PP: ~~~~~~~~~~~~ (shifted these bits of update here from before the 'levelData.RaceState' switch)

	//Get animations for actors inside spheres
	pRunSphere = this->animSegs.sphere.pRunSphere;
	pStaticSphere = this->animSegs.sphere.pStaticSphere;
	pRunSphereReverse = this->animSegs.sphere.pRunSphereReverse;

	pcSphere[0]->beforeYouCallAnythingElseCallThis();
	pcSphere[1]->beforeYouCallAnythingElseCallThis();

	pcSphere[0]->calculateRoll();
	pcSphere[1]->calculateRoll();

	for(m=0; m<NUM_FLASHES; m++)
	{
		if((bmRand() & 127) == 0)
		{
			PlayAnimationByName(&this->ppCameraFlash[m]->actorInstance, "flash", 1.0f, 0, 0, 0.0f, NONE);
			FacePoint(this->ppCameraFlash[m], gameStatus.player[0].camera->cameraState.cameraPos, 0.0f);
		}
	}

	// PP: ~~~~~~~~~~~~
    
	//If the actors aren't running then roll them in the sphere
	pCurrentAnim = CurrentAnimation(pcSphere[0]->pBoundToActor);

	if(pCurrentAnim && pCurrentAnim->crc == pStaticSphere->crc)
		pcSphere[0]->applySphereRotationToActor();

	pCurrentAnim = CurrentAnimation(pcSphere[1]->pBoundToActor);
						
	if(pCurrentAnim && pCurrentAnim->crc == pStaticSphere->crc)
		pcSphere[1]->applySphereRotationToActor();

	//Do pinball stuff (mushrooms, hammers and flippers)

	int pGlowColour[3][4] ={{127, 127, 0, 127},
							{0, 127, 0, 127},
							{127, 0, 0, 127}};
	
	if((bmRand() & 15) == 0)
	{
		for(m=0; m<MAX_TARGETS; m++)
			CreateSpriteFountain(starTarget[m].pStarTarget->actorInstance.position, pGlowColour[starTarget[m].state + 1], FOUNTAIN_GLOW_SPEED);
	}
	
	if(this->fountainProb == 1)
	{
		TBVector pos;
		
		this->fountainProb = 0;
		
		for(m=0; m<60; m++)
		{
			bmVectorCopy(pos, starTarget[this->fountainNum].pStarTarget->actorInstance.position);
			pos[Y] += 256 - (bmRand() & 1023);
			CreateSpriteFountain(pos, pGlowColour[starTarget[this->fountainNum].state + 1], FOUNTAIN_GLOW_SPEED * 4);
		}
	}
	
	for(m=0; m<8; m++)
	{
		float vd;
		
		if(ppMushroom[m]->bumperInfo->justKoiked)
		{
			PlayAnimationByName(&ppMushroom[m]->actorInstance, "koik", 1.0f, 0, 1, 0.0f, NONE);
			ppMushroom[m]->bumperInfo->justKoiked = 0;
			PlayTrackingSample("doink.wav", 255, ppMushroom[m]->actorInstance.position);
		}
		
		vd = bmVectorDistance(this->pElmer->actorInstance.position, ppMushroom[m]->actorInstance.position);
		
		if(vd < METERTOUNIT(2.0f))
		{
			if(ppMushroom[m]->bumperInfo->refereeNear == 0)
				pullItDown(ppMushroom[m]);
			
			ppMushroom[m]->bumperInfo->refereeNear++;
		}
		
		if(vd > METERTOUNIT(3.0f))
		{
			ppMushroom[m]->bumperInfo->refereeNear = 0;
		}
	}
	
	if((bmRand() & 127) == 0)
	{
		for(int m=0; m<8; m++)
		{
			ASSERT(ppMushroom[m]);
			
			if(!ppMushroom[m]->bumperInfo->refereeNear)
			{
				toggleIt(ppMushroom[m]);
			}
		}
	}					
	

	//if we have two human players
//	if(gameStatus.multiplayer.numPlayers == 2 && (!gameStatus.multiplayer.versusCPU))
//		bmVectorCopy(pcSphere[1]->pBoundToActor->actorBehaviour->physics.velocity, velBackup[1]);
//					}

	// CPW: now do elmer stuff
	// CPW: find the point that is exactly between the two players, (assuming, of course that there ARE two players)
	if (levelData.RaceState == Race)
	{
		TBVector diff, inBetween;
		float smallestDist;
		int smallestInd;
		
		float elmerToPlayer[2];
		TBVector elmerToLook;
		int i;
		
		TBActorAnimSegment *pAnim;
        
		TBActorAnimSegment *pWalkAnim = this->animSegs.elmer.pWalk;
		TBActorAnimSegment *pRunAnim = this->animSegs.elmer.pRun;
		TBActorAnimSegment *pIdleAnim = this->animSegs.elmer.pIdle;
        
		ASSERTM(pWalkAnim,  "Mr Fudd doesn't have a 'walk1' anim");
		ASSERTM(pRunAnim,   "Mr Fudd doesn't have a 'run' anim");
		ASSERTM(pIdleAnim,  "Mr Fudd doesn't have a 'idle1' anim");
        
		bmVectorSub(diff, gameStatus.player[0].actorInstance->actorInstance.position, gameStatus.player[1].actorInstance->actorInstance.position);
		bmVectorCMul(diff, diff, 0.5f);
		bmVectorAdd(inBetween, gameStatus.player[1].actorInstance->actorInstance.position, diff);

		// CPW: make it easy, temporarily
		bmVectorCopy(inBetween, gameStatus.player[0].actorInstance->actorInstance.position);

		smallestDist = bmVectorDistance(inBetween, *this->ppMoundPos[0]);
		smallestInd = 0;
                        
		for(i=1; i<(MAX_MOUNDS+1); i++)
		{
			float dist = bmVectorDistance(inBetween, *this->ppMoundPos[i]);
                            
			if(dist < smallestDist)
			{
				smallestDist = dist;
				smallestInd = i;
			}
        }
                        
        // CPW: plonk elmer at the centre of the nearest mound
		this->elmerMound = smallestInd;
		bmVectorCopy(this->elmerDesiredPos, *this->ppMoundPos[this->elmerMound]);

		//if Elmer is close enough to his desired position - make him stay there
		if(bmVectorDistance(this->elmerDesiredPos, this->pElmer->actorInstance.position) < METERTOUNIT(1.0f))
			pAnim = pIdleAnim;
		else
		{//else make him run there
			pAnim = pRunAnim;
			FacePoint(this->pElmer, this->elmerDesiredPos, 1.0f);// CPW: float offset)
		}

		pCurrentAnim = CurrentAnimation(&this->pElmer->actorInstance);

		//If elmer isn't doing something special, play the proper animation
		if( (pCurrentAnim != pAnim) &&
			(pCurrentAnim != this->animSegs.elmer.pSquashed) &&
			(pCurrentAnim != this->animSegs.elmer.pUnSquash) &&
			(pCurrentAnim != this->animSegs.elmer.pStaySquashed) &&
			(pCurrentAnim != this->animSegs.elmer.pWalk))
			PlayAnimationBySegmentPointer(&this->pElmer->actorInstance, pAnim, 1.0f, 1, 0, 0.0f, NONE);

		// CPW: make him look at the nearest player
		for(i=0; i<2; i++)
			elmerToPlayer[i] = bmVectorDistance(this->pElmer->actorInstance.position, gameStatus.player[i].actorInstance->actorInstance.position);

		if(elmerToPlayer[0] < elmerToPlayer[1])
			bmVectorSub(elmerToLook, gameStatus.player[0].actorInstance->actorInstance.position, this->pElmer->actorInstance.position);
		else
			bmVectorSub(elmerToLook, gameStatus.player[1].actorInstance->actorInstance.position, this->pElmer->actorInstance.position);
	}
					
	BOSSGAME::update();
}

/*	--------------------------------------------------------------------------------
	Function 	: ZOOBOSSGAME::shutdown
	Purpose 	: shut-down the Zoo Boss game
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides BOSSGAME::shutdown
*/
void CITYBOSSGAME::shutdown(void)
{
	// PP: do base-class shutdown
	BOSSGAME::shutdown();

	mpCountdownKill();

	ACTOR_STATUS	*pStatus;
	TEXTBOX			**ppTextBox;

	// CPW: bkDeleteEventClient(pEventClient);
	if (pcSphere[0])
	{
		pStatus = pcSphere[0]->pBoundToActor->actorStatus;
		ppTextBox = pStatus->multiplayer.pTextBox;

		if(pStatus->multiplayer.pStatsBook)
		{
			pStatus->multiplayer.pStatsBook->close();
			delete(pStatus->multiplayer.pStatsBook);
		}
	}

	for(int p=0; p<2; p++)
	{
		if(pcSphere[p])
		{
			delete(pcSphere[p]);
		}
	}

	gameStatus.multiplayer.numPlayers = 1;
	setNumScreens(1);
}

void CITYBOSSGAME::DoDaffyAI()
{
	if ((pcSphere[1]->homeInPoint < 1) || (timeWithoutAI < gameStatus.gameTime))
	{
		float	AIUpdateTime, dist = METERTOUNIT(100);	// JW: Something big enough to be too big
		
		int		i, close = -1;

		switch(gameStatus.globalSettings.difficulty)
		{
		case DIFF_EASY:		AIUpdateTime = 15.0f;
							
							if (pcSphere[1]->pBoundToActor->actorStatus->multiplayer.numStars > 5)
								goto assignPoint;

							break;

		case DIFF_MEDIUM:	AIUpdateTime = 10.0f;
							
							if (pcSphere[1]->pBoundToActor->actorStatus->multiplayer.numStars > 4)
								goto assignPoint;

							break;

		case DIFF_HARD:		AIUpdateTime = 5.0f;
							
							if (pcSphere[1]->pBoundToActor->actorStatus->multiplayer.numStars > 3)
								goto assignPoint;

							break;
		}
		
		for (i = 0; i < MAX_TARGETS; i++)
		{
			if (CityBoss_game.starTarget[i].state != 1)	// JW: I don't own this one
			{
				if (bmVectorDistance(pcSphere[1]->pBoundToActor->actorInstance.position, starTarget[i].headFor) < dist)
				{
					dist = bmVectorDistance(pcSphere[1]->pBoundToActor->actorInstance.position, starTarget[i].headFor);
					
					close = i;
				}
			}
		}
		
assignPoint:

		pcSphere[1]->homeInPoint = close;
		
		timeWithoutAI = gameStatus.gameTime + AIUpdateTime;
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateAndChooseMechaTweetySequences
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 ResetCityBossCallback(void *context, EBossGameResetChoice choice)
{
	switch(choice)
	{
	case BOSSGAMERESET_CONTINUE:
		bkGenerateEvent("scenechange", "_reset");
		break;
	case BOSSGAMERESET_QUIT:
		bkGenerateEvent("scenechange", "_cityhub");
		break;
	}

	EnableRumble(&controller1);

	// PP: must return a value
	return 0xB16B00B5;
}