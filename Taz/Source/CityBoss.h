#ifndef __H_CITYBOSS
#define __H_CITYBOSS

#include "multiplayer.h"
#include "bossgamecontinue.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														CITYBOSSGAME

					This is the BOSSGAME type for the Zoo Boss - Gossamer vs. Taz in an Elephant Pong showdown
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/



#define NUM_FLASHES	(292)


typedef struct TAG_CITYBOSSGAME		:		public BOSSGAME
{
	/*	--------------------------------------------------------------------------------
		Function 	: ZOOBOSSGAME::draw
		Purpose 	: draw the zoo boss game
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOSSGAME::draw
	*/
	void draw2(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ZOOBOSSGAME::init
		Purpose 	: initialise the Zoo Boss game
		Parameters 	: rectangle for playing area
		Returns 	: 
		Info 		: this overrides BOSSGAME::init
	*/
	void init(void);


	/*	--------------------------------------------------------------------------------
		Function 	: CITYBOSSGAME::updateCamera
		Purpose 	: update the City boss game's camera or cameras
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void updateCamera(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ZOOBOSSGAME::update
		Purpose 	: update the Zoo Boss game
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOSSGAME::update
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ZOOBOSSGAME::shutdown
		Purpose 	: shut-down the Zoo Boss game
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOSSGAME::shutdown
	*/
	void shutdown(void);

	/*	--------------------------------------------------------------------------------
		Function 	: ZOOBOSSGAME::DoDaffyAI
		Purpose 	: Makes daffys decisions based on difficulty level
		Parameters 	: 
		Returns 	: 
		Info 		: choose a target to go for or go for the play
	*/
	void DoDaffyAI();

	/*	--------------------------------------------------------------------------------
		Function 	: CITYBOSSGAME::forceComplete
		Purpose 	: forces the cityboss game into a taz win situation
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void forceComplete(void)
	{
		tazScore = daffyScore+1;
		levelData.RaceState = TimeUp;
		levelFinished = 1;
		whenToQuit = timer;
	}


	/* --------------------------------------------------------------------------------
	   Function		: CITYBOSSGAME::fadeDownCallback
	   Purpose		: function called when the fade-down, before the continue sequence, finishes
	   Parameters	: 
	   Returns		: 
	   Info			: // PP: must match the BossGameFadeDownCallback typedef (bossgamecontinue.h)
	*/
	static void fadeDownCallback(void);


	void shoveItUp(ACTORINSTANCE *);
	void pullItDown(ACTORINSTANCE *);
	void toggleIt(ACTORINSTANCE *);
	friend void cityBossTouchedPad(int);
	void cityBossForceEnd(bool win);

	class ATLASSPHERE	*pcSphere[2];

	ACTORINSTANCE		*ppMushroom[8];
	TBActorAnimSegment	**ppMushroomSeg[4];
	
	TBVector			*ppMoundPos[5];

	struct 
	{
		ACTORINSTANCE		*pStarTarget;
		ACTORINSTANCE		*pDummyTarget;
		TBVector			headFor;
		int					state;			// CPW: -1 = unset, 0 = taz (green), 1 = daffy (red)
		int					notTarget;
	}starTarget[10];

	struct
	{
		struct
		{
			TBActorAnimSegment *pSquashed;
			TBActorAnimSegment *pUnSquash;
			TBActorAnimSegment *pStaySquashed;

			TBActorAnimSegment *pWalk;
			TBActorAnimSegment *pRun;
			TBActorAnimSegment *pIdle;
		}elmer;

		struct
		{
			TBActorAnimSegment *pRunSphere;
			TBActorAnimSegment *pStaticSphere;
			TBActorAnimSegment *pRunSphereReverse;
		}sphere;

	}animSegs;

	TBTexture			*pLightTexture;
	TBTexture			*yelTexture;
	TBTexture			*greTexture;
	TBTexture			*redTexture;

	float				timer;
	float				startTime;

	ACTORINSTANCE		*pElmer;
	int					elmerMound;
	TBVector			elmerDesiredPos;

	TBTexture			*pClock;
	int					extraTime;
	int					levelFinished;
	float				whenToQuit;

	int					fountainProb;
	int					fountainNum;

	int					tazScore, daffyScore;

	bool				inContinue;								// PP: indicates that we're currently in the boss-game continue sequence (I didn't want to add this to the RACESTATE enum because it has nothing to do with races)

	ACTORINSTANCE		*ppCameraFlash[NUM_FLASHES];

}CITYBOSSGAME, _CITYBOSSGAME BALIGN16;

// TP: void originSetNormal(ACTORINSTANCE *);
// TP: void originSetSpecial(ACTORINSTANCE *);

int32 ResetCityBossCallback(void *context, EBossGameResetChoice choice);

extern CITYBOSSGAME		CityBoss_game;

#define MUSHROOMSEG_STATICDOWN	(0)
#define MUSHROOMSEG_STATICUP	(1)
#define MUSHROOMSEG_DOWN		(2)
#define MUSHROOMSEG_UP			(3)

#endif