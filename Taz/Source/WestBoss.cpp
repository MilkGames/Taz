// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : WestBoss.h
//   Purpose : Wile E West Boss - Yosemite Sam's ammo dump
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"					// PP: global stuff
#include "main.h"					// PP: main stuff
#include "maths.h"					// PP: maths stuff
#include "LinkedList.h"				// PP: linked list class
#include "actors.h"					// PP: actor stuff
#include "gui.h"					// PP: gui stuff
#include "camera.h"					// PP: camera stuff
#include "files.h"					// PP: provide support functions for actor and texture loading
#include "cameracase.h"				// PP: camera case stuff
#include "KOScam.h"					// PP: "Keep-On-Screen" camera - keeps its target actor instances on-screen at all times
#include "control.h"				// PP: control stuff
#include "parsemap.h"				// PP: map file parser
#include "TextFX.h"					// PP: for RECTANGLE
#include "animation.h"				// PP: provides functions and structures to manage animations
#include "physics.h"				// PP: physics functions
#include "collisioncache.h"			// PP: functions for handling the creation and storage of collision meshes
#include "scene.h"					// PP: scene initialise, setup and draw functions
#include "holysmoke.h"				// PP: smoke effects
#include "status.h"					// PP: pah, actor status?
#include "health.h"					// PP: handles anything to do with a character and its health
#include "sfx.h"					// PP: sound effect rountines
#include "characters.h"				// PP: provides character response code
#include "Icon.h"					// PP: Icon effects used by the front end
#include "lights.h"					// PP: lights
#include "util.h"					// PP: utility functions
#include "Explode.h"				// PP: explosions
#include "Shockwave.h"				// PP: Shockwave effect
#include "destructibles.h"			// PP: destructible objects
#include "Boss.h"					// PP: General boss game stuff
#include "MonitorFX.h"				// PP: for pretty colours
#include "display.h"				// PP: display stuff
#include "clouds.h"					// PP: cloud stuff (for injury stars)
#include "brickdisplay.h"			// PP: 'Brick Wall' score display for the Zoo boss game
#include "VideoFX.h"				// PP: Video effects - motion blur, defocus, whatever.  Top-of-the-Pops-TASTIC, Mate!
#include "WestBoss.h"				// PP: Wile E West Boss - Yosemite Sam's ammo dump
#include "pause.h"					// PP: pause menus
#include "timer.h"					// PP: timer stuff
#include "GolfBar.h"				// PP: Power-level indicator for the West Boss player 2's dynamite throwing
#include "playerstats.h"			// PP: player stats, what a pointless comment
#include "fireworks.h"				// PP: creates, updates, and draws firework effects
#include "HealthBar.h"				// PP: 'Health Bar' display for player characters, where it is needed
#include "bossgamecontinue.h"		// TP: continue scene
#include "PageItemFX.h"				// PP: Special effects for page items


WESTBOSSGAME		WestBoss_game;			// PP: the West Boss game
static TEXTBOX*		WestBoss_debugBox;		// PP: debug text box


// PP: TEMP FRIG
//#define WBDOF

// PP: TEMP for testing
bool overrideSamThrow=false;

// PP: TEMP TEST
bool g_WBstartedOutroAnim=false;

// PP: define WB_2P_CHARACTER_ICONS to get taz & sam character icons in 2player
// PP: #define WB_2P_CHARACTER_ICONS

// PP: define WB_TAZ_SCORE_DISPLAY to use a score display for taz in 1p & 2p
// PP: #define WB_TAZ_SCORE_DISPLAY

// PP: define WB_SPINOFF to get a spin-off effect when taz wins (disabled in stereoscopic modes, to save hassle)
// PP: NOTE: also disabled in PC compatibility mode (can't use offscreen render targets)
#define WB_SPINOFF

// PP: the global speed of the West Boss game
#define WB_GAMESPEED								1.0f

// PP: the split-screen mode to use in the West Boss game
#define WB_SPLITSCREEN_MODE							SPLITSCREEN_VERTICAL// PP: player 1 on left, player 2 on right

#define TAZ_HEIGHT									METERTOUNIT(0.6f)
#define WBTAZ_EXPLOSIONDIVE_CAMERA_HEIGHT			METERTOUNIT(1.5f)
#define WBTAZ_EXPLOSIONDIVE_TARGET_HEIGHT			(WBTAZ_EXPLOSIONDIVE_CAMERA_HEIGHT-TAZ_HEIGHT)

// PP: TEMP MS FRIG
VEC		exitPos=VEC(-1098, -1250, -15);//VEC(-3798, -1750, -515);

/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													WBSKATEBOMB methods

											Mobile bombs dipatched by Sam's deputies.
									(dynamite strapped to roller skates according to the doc)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: WBSKATEBOMB::init
	Purpose 	: init
	Parameters 	: x grid square, y grid square, x speed, z speed, distance to terminus
	Returns 	: 
	Info 		: 
*/
void WBSKATEBOMB::init(const int gridXin, const int gridYin, const float xSpeed, const float zSpeed, const float range)
{
	TBVector2D		gridPos;
	TBVector		position;

	gridPos[X]=gridXin+0.5f;					// PP: centre of square
	gridPos[Y]=gridYin+0.5f;					// PP: centre of square

	WestBoss_game.evaluateGridPos(gridPos, position);

	// PP: set position
	bmVectorCopy(this->pos(), position);

	this->speed[X]=xSpeed;
	this->speed[Y]=zSpeed;

	this->distToTerminus=range;

	this->state=WBSBSTATE_ROLLING;

	// PP: switch on the skatebomb while it's being used
	this->actorInstance->flags &= ~(ACTORFLAG_DONTDRAW|ACTORFLAG_NOCOLLISION);
}


/*	--------------------------------------------------------------------------------
	Function 	: WBSKATEBOMB::update
	Purpose 	: update
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
#define WBSKATEBOMB_DETONATION_RANGE		METERTOUNIT(0.5f)

void WBSKATEBOMB::update(void)
{
	float	xStep;
	float	yStep;

	xStep=(this->speed[X]/fps);
	yStep=(this->speed[Y]/fps);

	this->pos()[X]+=xStep;
	this->pos()[Z]+=yStep;

	// PP: direction is axis aligned so this is ok
	this->distToTerminus-=bmFAbs(xStep);
	this->distToTerminus-=bmFAbs(yStep);

	WestBoss_game.sitPointOnGrid(this->pos(), METERTOUNIT(0.3f));

	if(this->distToTerminus < 0.0f)
	{
		// PP: the bomb has reached its terminus - explode it
		this->kaboom();
		//this->state=WBSBSTATE_FINISHED
		return;
	}
	else
	{
		if(this->dist(wbtaz) < WBSKATEBOMB_DETONATION_RANGE)
		{
			// PP: HURT/PROPEL TAZ ON IMPACT...

	//#define BEBR_TAZHURT_RANGE		METERTOUNIT(1.75f)		// PP: max distance at which an explosion branch will explode dynamite


			// PP: MAKE A BIG BOOM NOISE
			PlaySample("explo_gen_01c.wav",255/*, this->pos(), 0*/);

			wbtaz.getHitByExplosion(NULL);


			// PP: bomb has hit Taz - explode it
			this->kaboom();
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: WBSKATEBOMB::kaboom
	Purpose 	: explode
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
#define WBSBSHOCK_NUMRINGS		3

// PP: Dynamite shockwave parameters
#define WBSBSHOCK_MAXRADIUS		METERTOUNIT(4.5f)
#define WBSBSHOCK_STARTSPEED		METERTOUNIT(1.1f)
#define WBSBSHOCK_ENDSPEED		METERTOUNIT(0.3f)
#define WBSBSHOCK_NUMRINGS		3
#define WBSBSHOCK_INTERVAL		0.07f
#define WBSBSHOCK_STARTWIDTH		METERTOUNIT(0.0f)
#define WBSBSHOCK_ENDWIDTH		METERTOUNIT(2.5f)

void WBSKATEBOMB::kaboom(void)
{
	COLOUR				colours[WBSBSHOCK_NUMRINGS];

	for(int c=0; c<WBSBSHOCK_NUMRINGS; ++c)
	{
		RANDOM_PRETTY_WINDOWS_COLOUR(colours[c]);
	}

	// PP: MAKE A SHOCKWAVE EFFECT
	// PP: REMOUT: TEMP to stop crash
/*	Shockwave_create(this->pos(),
						WBSBSHOCK_MAXRADIUS,
						WBSBSHOCK_STARTSPEED,
						WBSBSHOCK_ENDSPEED,
						WBSBSHOCK_NUMRINGS,
						WBSBSHOCK_INTERVAL,
						WBSBSHOCK_STARTWIDTH,
						WBSBSHOCK_ENDWIDTH,
						&colours[0]);*/

	this->state=WBSBSTATE_FINISHED;

	// PP: switch off the skatebomb actorInstance till it gets dispatched again
	this->actorInstance->flags |= (ACTORFLAG_DONTDRAW|ACTORFLAG_NOCOLLISION);
}



/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													WBDEPUTY methods

								Sam's deputies, who hide in crates dispatching skatebombs.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: WBDEPUTY::init
	Purpose 	: init
	Parameters 	: x grid square, y grid square
	Returns 	: 
	Info 		: 
*/
void WBDEPUTY::init(const int gridXin, const int gridYin)
{
	WBTNTCRATE*		crate;

	this->gridX=gridXin;
	this->gridY=gridYin;

	this->attackDirections=0;

	// PP: work out what directions we can attack in...
	
	int		maxNumSquares=0;		// PP: max num squares clear in a direction
	int		x,y;				// PP: loop counters

	y=this->gridY;

	// PP: left range
	for(x=this->gridX-1; x>=0; x--)
	{
		if(WESTBOSSGAME::grid[y][x] == WBGRID_GROUND)
		{
			// PP: see if there's a blocking crate on this square (y,x)
			if((crate=WestBoss_game.getCrate(x, y)) != NULL)
			{
				this->blockingCrate=crate;
			}

			maxNumSquares++;
		}
		else
		{
			// PP: oops, hit a wall
			break;
		}
	}

	if(maxNumSquares>1)
	{
		this->attackDirections |= WBDEPDIR_LEFT;
		this->range[WBDEPDIR_LEFT]=(maxNumSquares)*WB_SQUARE_WIDTH;
	}

	maxNumSquares=0;

	// PP: right range
	for(x=this->gridX+1; x<WB_SQUARES_WIDE; x++)
	{
		if(WESTBOSSGAME::grid[y][x] == WBGRID_GROUND)
		{
			// PP: see if there's a blocking crate on this square (y,x)
			if((crate=WestBoss_game.getCrate(x, y)) != NULL)
			{
				this->blockingCrate=crate;
			}

			maxNumSquares++;
		}
		else
		{
			// PP: oops, hit a wall
			break;
		}
	}

	if(maxNumSquares>1)
	{
		this->attackDirections |= WBDEPDIR_RIGHT;
		this->range[WBDEPDIR_RIGHT]=(maxNumSquares)*WB_SQUARE_WIDTH;
	}

	maxNumSquares=0;

	x=this->gridX;

	// PP: down range
	for(y=this->gridY-1; y>=0; y--)
	{
		if(WESTBOSSGAME::grid[y][x] == WBGRID_GROUND)
		{
			// PP: see if there's a blocking crate on this square (y,x)
			if((crate=WestBoss_game.getCrate(x, y)) != NULL)
			{
				this->blockingCrate=crate;
			}

			maxNumSquares++;
		}
		else
		{
			// PP: oops, hit a wall
			break;
		}
	}

	if(maxNumSquares>1)
	{
		this->attackDirections |= WBDEPDIR_DOWN;
		this->range[WBDEPDIR_DOWN]=(maxNumSquares)*WB_SQUARE_HEIGHT;
	}

	maxNumSquares=0;

	// PP: up range
	for(y=this->gridY+1; y<WB_SQUARES_HIGH; y++)
	{
		if(WESTBOSSGAME::grid[y][x] == WBGRID_GROUND)
		{
			// PP: see if there's a blocking crate on this square (y,x)
			if((crate=WestBoss_game.getCrate(x, y)) != NULL)
			{
				this->blockingCrate=crate;
			}

			maxNumSquares++;
		}
		else
		{
			// PP: oops, hit a wall
			break;
		}
	}

	if(maxNumSquares>1)
	{
		this->attackDirections |= WBDEPDIR_UP;
		this->range[WBDEPDIR_UP]=(maxNumSquares)*WB_SQUARE_HEIGHT;
	}

	if(this->blockingCrate == NULL)
	{
		this->setState(WBDEPSTATE_WAITING);
	}
	else
	{
		this->setState(WBDEPSTATE_BLOCKED);
	}

	this->timer=0.0f;
}


/*	--------------------------------------------------------------------------------
	Function 	: WBDEPUTY::attack
	Purpose 	: attack
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
#define WBDEP_SKATEBOMB_SPEED		METERTOUNIT(4.0f)

void WBDEPUTY::attack(void)
{
	if(this->attackDirections & WBDEPDIR_LEFT)
	{
		WestBoss_game.addSkateBomb(this->gridX, this->gridY, -WBDEP_SKATEBOMB_SPEED, 0.0f, this->range[WBDEPDIR_LEFT]);
	}

	if(this->attackDirections & WBDEPDIR_RIGHT)
	{
		WestBoss_game.addSkateBomb(this->gridX, this->gridY, WBDEP_SKATEBOMB_SPEED, 0.0f, this->range[WBDEPDIR_RIGHT]);
	}

	if(this->attackDirections & WBDEPDIR_DOWN)
	{
		WestBoss_game.addSkateBomb(this->gridX, this->gridY, 0.0f, -WBDEP_SKATEBOMB_SPEED, this->range[WBDEPDIR_DOWN]);
	}

	if(this->attackDirections & WBDEPDIR_UP)
	{
		WestBoss_game.addSkateBomb(this->gridX, this->gridY, 0.0f, WBDEP_SKATEBOMB_SPEED, this->range[WBDEPDIR_UP]);
	}

	this->timer=0.0f;
}


/*	--------------------------------------------------------------------------------
	Function 	: WBDEPUTY::setState
	Purpose 	: set state
	Parameters 	: new state to set
	Returns 	: 
	Info 		: // PP: looks kinda silly but it will change soon
*/
void WBDEPUTY::setState(const int stateIn)
{
	this->state=stateIn;
}


/*	--------------------------------------------------------------------------------
	Function 	: WBDEPUTY::update
	Purpose 	: udpate
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
#define WBDEP_ATTACK_INTERVAL		2.0f

void WBDEPUTY::update(void)
{
	// PP: check if the blockage has cleared.

	switch(this->state)
	{
	case WBDEPSTATE_BLOCKED:
		{
			if(this->blockingCrate != NULL)
			{
				if(this->blockingCrate->exploded)
				{
					this->setState(WBDEPSTATE_WAITING);
				}
			}
		}
		break;

	case WBDEPSTATE_WAITING:
		{
			// PP: TODO: only attack if taz is in path

			if(this->timer > WBDEP_ATTACK_INTERVAL)
			{
				this->attack();
			}
		}
		break;
	}

	this->timer+=fTime;
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													WBSAM methods

										Yosemite Sam as the Wile E West boss
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


// PP: longest (initial) intervals between sam's throws, for each difficulty level
float WBSAM::slowThrowIntervals[NUM_DIFFICULTY_LEVELS]=
{
	5.5f,		// PP: easy
	4.0f,		// PP: medium
	3.0f		// PP: hard
};

// PP: shortest (final) intervals between sam's throws, for each difficulty level
float WBSAM::fastThrowIntervals[NUM_DIFFICULTY_LEVELS]=
{
	1.5f,		// PP: easy
	1.25f,		// PP: medium
	0.9f		// PP: hard
};

char* WBSAM::speechSamples[WBSAM_NUM_PHRASES]=
{
	// PP: throwing TNT...
	"samwestbosstaunt_1.wav",								// PP: There's no escape now, ha ha ha!
	"samwestbosstnt_2.wav",									// PP: Catch that you critter!
	"samwestbosstnt_4.wav",									// PP: You're not getting’ away this time, you hear me?
	"samwestbosstnt_1.wav",									// PP: Eat this, you furball!

	// PP: Crate destroyed...
	"samwestbossmad_3.wav",									// PP: I don't believe it.
	"zoobossoutro_6.wav",									// PP: ooooooh!
	"zoobossoutro_6_alt1.wav",								// PP: ooooooh!
	"zoobossoutro_6_alt2.wav",								// PP: ooooooh!
	"samwestbossmad_2.wav",									// PP: what the?!
	"samwestbossmad_3_alt.wav",								// PP: I don't believe it.
	"samwestbossmad_2_alt_.wav",							// PP: what the?!	// PP: NOTE: THAT LAST UNDERSCORE IS MEANT TO BE THERE!
	"samwestbosstaunt_2.wav",								// PP: This varmint's costin’ me a fortune!*/

	// PP: Super crate destroyed...
	"samwestbossmad_1.wav",									// PP: Why ya consarned, skinny-legged, tongue-waggin' bird-chasin' idget mutt!

	// PP: Hurt Taz...
	"samwestbosshappy_1.wav",								// PP: Bwa ha ha - How d'you like that!
	"samwestbosshappy_2.wav",								// PP: Look at that devil do the rhumba!  Ha ha ha!
	"samwestbosslaugh1.wav",								// PP: (laugh from WBSAMSPEECH_HURTTAZ1)
	"samwestbosslaugh2.wav",								// PP: (laugh from WBSAMSPEECH_HURTTAZ2)
};

/*char* WBSAM::crateDestructionAnims[WBSAM_NUM_CRATE_DESTRUCTION_ANIMS]=
{
"destroybox1",
"destroybox2",
"destroybox3"
};*/


/*	--------------------------------------------------------------------------------
	Function 	: WBSAM::loseSuperCrate
	Purpose 	: sam loses one of his super crates, thus losing a point
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void WBSAM::loseSuperCrate(void)
{
	if(this->score->decreaseScore() <= 0)
	{
		WestBoss_game.winner=0;// PP: taz wins
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: WBSAM::chooseRandomProduceAnim
	Purpose 	: randomly chooses a produce anim, but doesn't play it
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void WBSAM::chooseRandomProduceAnim(void)
{
	// PP: play a produce anim
	if(bmRand()&1)
	{
		this->produceAnim=baFindAnimSegment(&(this->actorInstance->actorInstance), "producedynamite1");
	}
	else
	{
		this->produceAnim=baFindAnimSegment(&(this->actorInstance->actorInstance), "producedynamite2");
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: WBSAM::init
	Purpose 	: initialisation
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/

// PP: MOVE US!!!!!!!

// PP: shuffle _array; pseudo-randomize the positions of its elements
#define SHUFFLE(_array, _arraySize, _type) \
{ \
	int32	_indexA, _indexB; \
	\
	/* PP: No significance to _arraySize as the number of shuffles, at least not that I'm aware of. */ \
	/* PP: Just wanted to make sure that the number of shuffles is directly proportional to the number of elements. */ \
	/* PP: I don't know much about the maths of shuffling, and I don't think it matters much here anyway. */ \
	for(int _i=(_arraySize); _i>0; _i--) \
	{ \
		_indexA=bmRand()%(_arraySize); \
		_indexB=bmRand()%(_arraySize); \
		\
		SWAP((_array)[_indexA], (_array)[_indexB], _type); \
	} \
}

// PP: loads ints 0 upwards into corresponding elements of _array, then shuffles _array
#define LOAD_AND_SHUFFLE(_array, _arraySize, _type) \
{ \
	for(int _j=(_arraySize)-1; _j>=0; _j--) \
	{ \
		(_array)[_j]=_j; \
	} \
	\
	SHUFFLE(_array, _arraySize, _type); \
}

void WBSAM::init(void)
{
	ACTORINSTANCE*	samPosInst;

//	CreateActorInstanceBehaviour(actorInstance);

	// PP: set up Sam's player 2, or CPU...

	if(WestBoss_game.twoPlayer)
	{
		this->player= &gameStatus.player[1];

		// PP: clear the player's default control flag, so's we can use special westboss control
		this->player->flags &= ~PLAYERFLAG_DEFAULTCONTROL;
	}
	else
	{
		
		this->player=NULL;
	}

	samPosInst=FindActorInstanceInMap("sam_inst", 0, &map);

	if(this->player == NULL)
	{
		if(samPosInst == NULL)
		{
			ACTORINSTANCEUSER::init(WBSAM_MODEL, ZVEC, WBSAM_YANG, BGACT_LINE);
		}
		else
		{
			ACTORINSTANCEUSER::init(WBSAM_MODEL, samPosInst->actorInstance.position, WBSAM_YANG, BGACT_LINE);
		}
	}
	else
	{
		// PP: set the actor instance for the bat
		this->actorInstance=this->player->actorInstance;

		//TBVector	samPos;

		// PP: TEMP TESET
		//SETVECTOR(samPos, 7, 1023, 107, 0);

		if(samPosInst == NULL)
		{
		}
		else
		{
			// PP: TEST: position sam (again)
			bmVectorCopy(this->pos(), samPosInst->actorInstance.position);
		}

		//this->setPos(samPos);
	}

	this->pos()[Y]-=METERTOUNIT(0.6f);// PP: MILESTONE FRIG

	// PP: turn sam to face taz
	this->YrotateToFace(wbtaz);

	this->flags=WBSAMFLAG_NONE;

//	this->actorInstance->flags &= ~(ACTORFLAG_NOCOLLISION|ACTORFLAG_NOVELOCITY);// PP: TODO: MAKE THIS NICER

	// PP: choose a dynamite produce anim
	this->chooseRandomProduceAnim();

	// PP: keep ptr to idle anim
	this->idleAnim=baFindAnimSegment(&(this->actorInstance->actorInstance), WBSAM_ANIM_IDLE);

	// PP: set golfbar power level to target taz
	this->golfBar->level=getThrowPowerLevel(this->XZdist(wbtaz));

	// PP: randomise crate-destruction reaction list
	LOAD_AND_SHUFFLE(this->crateDestructionReactionList, WBSAM_NUM_CRATE_DESTRUCTION_REACTIONS, char);

	// PP: initialise crate destruction reaction index
	this->crateDestructionReaction=0;

	if(this->player == NULL)
	{
		this->setState(WBSAMSTATE_ATTACK);// PP: _INTRO
	}
	else
	{
		this->setState(WBSAMSTATE_HUMANPLAY);// PP: _INTRO
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: WBSAM::produceDynamite
	Purpose 	: make Sam produce a stick of dynamite which he will then throw
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void WBSAM::produceDynamite(void)
{
	// PP: TEMP for testing
	if(overrideSamThrow)
	{
		return;
	}

	// PP: play the dynamite produce anim
	this->playAnim(this->produceAnim);

	// PP: queue the idle anim
	this->queueAnim(this->idleAnim, true);

	this->flags |= WBSAMFLAG_PRODUCING;

	// PP: NEW TEST
	this->timer=0.0f;
}


/*	--------------------------------------------------------------------------------
	Function 	: WBSAM::startChuck
	Purpose 	: make Sam start to throw a stick of dynamite that he has produced
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void WBSAM::startChuck(void)
{
	this->flags |=  WBSAMFLAG_CHUCKING;

	// PP: play a random throw anim
	if(bmRand()&1)
	{
		this->playAnim("throwover");
	}
	else
	{
		this->playAnim("throwunder");
	}

	if(this->flags & WBSAMFLAG_FIRSTTHROWSINCELOSINGSUPERCRATE)// PP: only taunt if just lost a super crate
	{
		// PP: make a taunt

		switch(this->score->getScore())
		{
		case 3:
			{
				// PP: this->sayQueued(WBSAMSPEECH_THROW2);// PP: Catch that you critter!	

				this->sayQueued(WBSAMSPEECH_THROW4);// PP: Eat this, you furball!
			}
			break;

		case 2:
			{
				// PP: this->sayQueued(WBSAMSPEECH_THROW4);// PP: Eat this, you furball!

				this->sayQueued(WBSAMSPEECH_THROW3);// PP: You're not getting’ away this time, you hear me?
			}
			break;

		case 1:
			{
				// PP: this->sayQueued(WBSAMSPEECH_THROW3);// PP: You're not getting’ away this time, you hear me?

				this->sayQueued(WBSAMSPEECH_THROW1);// PP: There's no escape now, ha ha ha!
			}
			break;
		}

		this->flags &= ~WBSAMFLAG_FIRSTTHROWSINCELOSINGSUPERCRATE;
	}

	// PP: return to idle anim afterwards
	this->queueAnim(WBSAM_ANIM_IDLE, true);

	// PP: reset chuck timer
	this->timer = 0.0f;
}


/*	--------------------------------------------------------------------------------
	Function 	: WBSAM::chuck
	Purpose 	: make Sam throw a stick of dynamite to his target x&y grid coords
	Parameters 	: 
	Returns 	: ptr to the dynamite thrown
	Info 		: called 'chuck' because 'throw' is a keyword :o(
*/

#define WBSAM_MIN_THROW_ELEVATION		RAD(45)
#define WBSAM_MAX_THROW_ELEVATION		RAD(60)

WBDYNAMITE* WBSAM::chuck(void)
{
	TBVector		source;			// PP: source of throw
	TBVector		target;
	TBVector2D		targetGridPos;

	// PP: calculate target point from grid coords
	targetGridPos[X]=this->targetX+0.5f;
	targetGridPos[Y]=this->targetY+0.5f;

	WestBoss_game.evaluateGridPos(targetGridPos, target);

	target[Y]+=WBDYNAMITE_COLLISION_RADIUS;

	// PP: just so's we know we're getting here
//	bkPrintf("WBSAM::chuck\n");

	// PP: TEMP: throw dynamite from some height above Sam's position
	// PP: TODO: release from bone during throw animation
	bmVectorCopy(source, this->pos());
	source[Y]+=METERTOUNIT(1);

	float			theta;			// PP: angle of elevation for throw
	float			vx, vy;			// PP: X & Y components of throw velocity
	float			h;				// PP: vertical distance to target point
	float			d;				// PP: horizontal distance to target point
	float			a;				// PP: gravity
	float			sinTheta;		// PP: sine of theta
	float			cosTheta;		// PP: cosine of theta

	// PP: throw angle
	theta=WBSAM_MIN_THROW_ELEVATION+((bmRand()/RANDMAX)*(WBSAM_MAX_THROW_ELEVATION-WBSAM_MIN_THROW_ELEVATION));

	// PP: gravity
	a=worldGravity;

	// PP: get vertical throw distance
	h=target[Y]-source[Y];

	// PP: get horizontal throw distance...

	TBVector2D		distVec2D;

	distVec2D[X]=target[X]-source[X];
	distVec2D[Y]=target[Z]-source[Z];
	
	d=bmVector2DLen(distVec2D);

	// PP: store sine & cosine of theta, saves recalculation...

	sinTheta=bmSin(theta);
	cosTheta=bmCos(theta);

	// PP: calculate horizontal component of throw velocity
	vx= (bmSqrt(2.0f)*bmSqrt((a*cosTheta)/((cosTheta*h)-(d*sinTheta)))*d) / 2.0f;

	// PP: calculate vertical component of throw velocity
	vy=vx*(sinTheta/cosTheta);

	// PP: rotate horizontal speed to correct Y angle...

	float		yAng;
	TBVector2D	distVec;

	distVec[X]=target[X]-this->pos()[X];
	distVec[Y]=target[Z]-this->pos()[Z];

	float dist;

	dist=bmVector2DLen(distVec);

	if(dist > 0.0f)
	{
		yAng=bmATan2(distVec[Y], distVec[X]);
	}

	// PP: store velocity vector for the projectile...

	TBVector		velocity;

	velocity[X]=vx*bmCos(yAng);
	velocity[Y]=vy;
	velocity[Z]=vx*bmSin(yAng);

	// PP: create the dynamite...

	WBDYNAMITE*		dyn;

	dyn=WestBoss_game.dynamiteList.getNew();

	dyn->init(source, yAng);

	// PP: switch on the dynamite actorInstance
	dyn->actorInstance->flags &= ~(ACTORFLAG_DONTDRAW|ACTORFLAG_NOCOLLISION);

	bmVectorCopy(dyn->vel(), velocity);

	// PP: set dynamite's target height
	bmVectorCopy(dyn->targetPos, target);

	dyn->gridX=this->targetX;
	dyn->gridY=this->targetY;

// PP: TODO: ?	WestBoss_game.addCameraTarget(&dyn);

	return dyn;
}


/*	--------------------------------------------------------------------------------
	Function 	: WBSAM::respondToMessage
	Purpose 	: respond to a message
	Parameters 	: message string
	Returns 	: 
	Info 		: 
*/
void WBSAM::respondToMessage(const char* const message)
{
	static TBTimerValue		scaleTimer;

	if(stricmp(message, "throw")==0)
	{
//		bkPrintf("************** THROW!\n");

		WBDYNAMITE*		dynamite;

		dynamite=this->chuck();

/* PP: REMOUT: OOD		if(WestBoss_game.state == BGSTATE_INTRO)
		{
			this->introDynamite=dynamite;
			WestBoss_game.setCamera(WBCAM_INTRODYNAMITE);
		}*/

		// PP: 
	}
	else if(stricmp(message, "startscale")==0)
	{
		scaleTimer=bkTimerRead();
	}
	else if(stricmp(message, "endscale")==0)
	{
	//	bkPrintf("********* WBSAM dynamite scale duration: %5.2f ***********\n", bkTimerDelta(scaleTimer, bkTimerRead())/bkTimerGetFrequency());
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: WBSAM::sampleFinishCallback
	Purpose 	: general sample finish callback for sam
	Parameters 	: ptr to sample that is finishing

	Returns 	: 
	Info 		: 
*/


/*	--------------------------------------------------------------------------------
	Function 	: WBSAM::setState
	Purpose 	: set Sam's state
	Parameters 	: new state for Sam (WBSAMSTATE_)
	Returns 	: 
	Info 		: 
*/
#define WBSAM_ANIM_INTRO		"destroybox2"

void WBSAM::setState(const WBSamState stateIn)
{
	switch(stateIn)
	{
/* PP: REMOUT: OOD		case WBSAMSTATE_INTRO:
		{
			// PP: Turn to face Taz...
			this->YrotateToFace(wbtaz);

			this->playAnim(WBSAM_ANIM_INTRO);

			WestBoss_game.setCamera(WBCAM_INTROSAM);

// PP: REMOUT: OOD			this->say(WBSAMSPEECH_INTRO);
		}
		break;*/

/* PP: REMOUT: OOD		case WBSAMSTATE_INTROTHROW:
		{
			// PP: start the throw

			this->produceDynamite();
			//this->playAnim("throwover");
		}
		break;*/

/* PP: REMOUT: OOD		case WBSAMSTATE_INTROPOSTTHROW:
		{
			this->timer=0.0f;

// PP: no!			WestBoss_game.setCamera(WBCAM_INTROTAZ);
		}
		break;*/

	case WBSAMSTATE_ATTACK:
		{
			this->playAnim(WBSAM_ANIM_IDLE, true);// PP: loop

			this->timer=0.0f;
		}
		break;

	case WBSAMSTATE_HUMANPLAY:
		{
			this->playAnim(WBSAM_ANIM_IDLE, true);// PP: loop

			WestBoss_game.setState(BGSTATE_PLAY);
		}
		break;

	case WBSAMSTATE_WINNING:
		{
		}
		break;

	case WBSAMSTATE_LOSING:
		{
			// PP: hmm never called
		}
		break;
	}

	this->state=stateIn;
}


/*	--------------------------------------------------------------------------------
	Function 	: WBSAM::humanThrowCallback
	Purpose 	: throw callback for player 2 Sam's golf bar
	Parameters 	: ptr to Sam, power level of throw
	Returns 	: 
	Info 		: must match the GolfBarHitCallback typedef
*/
void WBSAM::humanThrowCallback(const long32 context, const float level)
{
	WBSAM*	sam;

	sam=(WBSAM*)context;

	if(!(sam->flags&(WBSAMFLAG_PRODUCING|WBSAMFLAG_CHUCKING)))
	{
		sam=(WBSAM*)context;

		// PP: get the horizontal distance of the throw...

		sam->startChuck();

		// PP: TEMP TEST - throw at taz
		//sam->chuck(wbtaz.gridX, wbtaz.gridY);
	}

}


/*	--------------------------------------------------------------------------------
	Function 	: WBSAM::humanUpdate
	Purpose 	: human update
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
	
// PP: angle through which Sam will turn in a second with stick at full tilt
#define WBSAM_HUMAN_TURN_SPEED		1.3f

void WBSAM::humanUpdate(void)
{
	switch(this->state)
	{
	case WBSAMSTATE_INTRO:
		{
			if(!this->playingAnim())
			{
				this->setState(WBSAMSTATE_HUMANPLAY);
			}
		}
		break;

	case WBSAMSTATE_HUMANPLAY:
		{
			if(this->animChanged)
			{
				if(this->flags & WBSAMFLAG_PRODUCING)
				{
					// PP: finished producing dynamite, return to idle

					this->flags &= ~WBSAMFLAG_PRODUCING;
				}

				if(this->flags & WBSAMFLAG_CHUCKING)
				{
					// PP: finished chucking, produce more dynamite
					this->flags &= ~WBSAMFLAG_CHUCKING;

					this->produceDynamite();
				}
			}

			if(this->currentAnim == this->idleAnim)// PP: finished playing an anim, now in idle
			{
				// PP: not doing any of these things any more
				this->flags &= ~(WBSAMFLAG_CHUCKING|WBSAMFLAG_SULKING|WBSAMFLAG_CELEBRATING);
			}

			// PP: find potential throw distance
			this->humanThrowDistance=this->getThrowDistance(this->golfBar->level);

			// PP: don't update if we're producing dynamite - we want to be able to aim, press the throw button, then immediately start re-aiming
			if(!(this->flags&(WBSAMFLAG_PRODUCING|WBSAMFLAG_CHUCKING)))
			{
				// PP: get the raw landing position of the throw...

				TBVector2D	landPos;

				landPos[X]=this->humanThrowDistance*bmCos(this->getYang()-HALFPI);
				landPos[Y]=this->humanThrowDistance*bmSin(this->getYang()-HALFPI);

				landPos[X]+=this->pos()[X];
				landPos[Y]+=this->pos()[Z];

				// PP: round raw landing position to nearest square...

				this->targetX=WESTBOSSGAME::getGridX(landPos[X]);
				this->targetY=WESTBOSSGAME::getGridY(landPos[Y]);
			}

			// PP: control Sam's Y angle with the left stick left&right
			if(this->player->controller->x1 != 0.0f)
			{
				this->setYang(this->getYang()-(this->player->controller->x1*fTime*WBSAM_HUMAN_TURN_SPEED));
			}
		}
		break;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: WBSAM::aiUpdate
	Purpose 	: ai update
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void WBSAM::aiUpdate(void)
{
	// PP: IMPROVE ME: Turn to face Taz...
	this->YrotateToFace(wbtaz);

	switch(this->state)
	{
	case WBSAMSTATE_INTRO:
		{
			if(!this->playingAnim())
			{
				this->setState(WBSAMSTATE_INTROTHROW);
			}
		}
		break;

	case WBSAMSTATE_INTROTHROW:
		{
			// PP: ok, so I'm throwing yeah?

			if(this->animChanged)
			{
				if(this->flags & WBSAMFLAG_PRODUCING)
				{
					// PP: finished producing dynamite
					this->flags &= ~WBSAMFLAG_PRODUCING;

					// PP: aim at taz
					this->targetX=wbtaz.gridX;
					this->targetY=wbtaz.gridY;

					// PP: and throw
					this->startChuck();
				}
				else
				{
					// PP: finished throwing - wait
					this->setState(WBSAMSTATE_INTROPOSTTHROW);
				}
			}
		}
		break;

	case WBSAMSTATE_INTROPOSTTHROW:
		{
			this->timer+=fTime;

			if(this->timer > WBSAM_DURATION_INTROPOSTTHROW)
			{
				this->setState(WBSAMSTATE_ATTACK);
			}
		}
		break;

	case WBSAMSTATE_ATTACK:
		{
			//if(/*!this->animChanged*/this->playingAnim())
			{
				if(this->timer > (WBSAM::fastThrowIntervals[CURRENT_DIFFICULTY]+(((this->score->getScore()-1)/4.0f)*(WBSAM::slowThrowIntervals[CURRENT_DIFFICULTY]-WBSAM::fastThrowIntervals[CURRENT_DIFFICULTY]))))
				{
					if(!(this->flags&(WBSAMFLAG_SULKING|WBSAMFLAG_CELEBRATING|WBSAMFLAG_PRODUCING|WBSAMFLAG_CHUCKING)))// PP: wait till the crate destruction response has finished before throwing dynamite
					{
						this->produceDynamite();
					}
				}
			}
			
			if(this->currentAnim == this->idleAnim)// PP: finished playing an anim, now in idle
			{
				// PP: not doing any of these things any more
				this->flags &= ~(WBSAMFLAG_CHUCKING|WBSAMFLAG_SULKING|WBSAMFLAG_CELEBRATING);
			}

			if(this->animChanged)
			{
				if(this->flags & WBSAMFLAG_PRODUCING)
				{
					// PP: finished producing dynamite - throw

					this->flags &= ~WBSAMFLAG_PRODUCING;

					// PP: aim at taz
					this->targetX=wbtaz.gridX;
					this->targetY=wbtaz.gridY;

					// PP: and throw
					this->startChuck();

					break;
				}

				// PP: back to idle
			// PP: 	this->playAnim("idle1", true);
			}

			this->timer+=fTime;

			// PP: If Taz gets too close, Sam gets reeel mad
			/*if(this->XZdist(wbtaz) < WBSAM_MASSIVEATTACK_RADIUS)
			{
				this->setState(WBSAMSTATE_MASSIVEATTACK);
			}*/
		}
		break;

/*	case WBSAMSTATE_MASSIVEATTACK:
		{
			if(CheckIfPlayingAnimation(this->actorInstance, NULL))
			{
				if(this->timer > WBSAM_THROW_INTERVAL_FAST)
				{
					if(!(this->flags&WBSAMFLAG_RESPONDING))// PP: wait till the crate destruction response has finished before throwing dynamite
					{
						this->produceDynamite();
					}
				}
			}
			else
			{
				if(this->flags & WBSAMFLAG_RESPONDING)
				{
					// PP: finished responding, return to idle

					this->flags &= ~WBSAMFLAG_RESPONDING;
					
					this->playAnim("idle1", true);


				}
				else if(this->flags & WBSAMFLAG_PRODUCING)
				{
					// PP: finished producing dynamite - throw

					this->flags &= ~WBSAMFLAG_PRODUCING;

					this->startChuck();
				}
				else if(this->flags & WBSAMFLAG_CHUCKING)
				{
					// PP: finished chucking, return to idle

					this->flags &= ~WBSAMFLAG_CHUCKING;
					
					this->playAnim("idle1", true);
				}
			}

			this->timer+=fTime;
			
			if(this->XZdist(wbtaz) > WBSAM_MASSIVEATTACK_RADIUS)
			{
				this->setState(WBSAMSTATE_ATTACK);
			}
		}
		break;*/
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: WBSAM::update
	Purpose 	: update
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void WBSAM::update(void)
{
	//this->timer-=fTime;

	ACTORINSTANCEUSER::update();	// PP: this updates animation-changed flag

	// PP: do nowt if not in 'play' state
	if(WestBoss_game.state != BGSTATE_PLAY)
	{
		return;
	}

	if(this->player == NULL)
	{
		this->aiUpdate();
	}
	else
	{
		this->humanUpdate();
	}

}


/*	--------------------------------------------------------------------------------
	Function 	: WBSAM::sulk
	Purpose 	: respond to a crate being destroyed
	Parameters 	: whether or not the crate is a super crate
	Returns 	: 
	Info 		: 
*/
void WBSAM::sulk(const bool super)
{
	if(super)
	{
		this->flags |= WBSAMFLAG_FIRSTTHROWSINCELOSINGSUPERCRATE;
	}

	if(this->flags & WBSAMFLAG_SULKING)
	{
		// PP: already sulking
		return;
	}

	if(!(this->flags&WBSAMFLAG_CELEBRATING))// PP: come out of a celbration to sulk
	{
		if(this->playingSound())
		{
			// PP: in the middle of saying something - don't respond to this small defeat
			return;
		}
	}

	this->flags |= WBSAMFLAG_SULKING;
	this->flags &= ~WBSAMFLAG_PRODUCING;
	this->flags &= ~WBSAMFLAG_CHUCKING;
	this->flags &= ~WBSAMFLAG_CELEBRATING;

	if(super)
	{
		// PP: why ya/jump on hat - VERY BAD MATCH - *** need about EIGHT stamps instead of 3 ***
		this->playAnim("destroysupercrate1");
		this->say(WBSAMSPEECH_SUPERCRATEDESTROY1);
	}
	else
	{
		// PP: switch(bmRand()%3)
		switch(this->crateDestructionReactionList[this->crateDestructionReaction])
		{
		case 0:
			{
				// PP: i don't believe it/slap face & shake head
				this->playAnim("destroybox1");

				if(bmRand()&1)
				{
					this->say(WBSAMSPEECH_CRATEDESTROY1);
				}
				else
				{
					this->say(WBSAMSPEECH_CRATEDESTROY6);
				}
			}
			break;

		case 1:
			{
				// PP: what the/jump in air
// PP: REMOUT: not a strong enough line for this anim				this->playAnim("destroybox2");

				if(bmRand()&1)
				{
					this->say(WBSAMSPEECH_CRATEDESTROY5);
				}
				else
				{
					this->say(WBSAMSPEECH_CRATEDESTROY7);
				}
			}
			break;

		case 2:
			{
				// PP: oooooh/twang moustache
				this->playAnim("destroybox3");

				this->say((WBSamSpeech)(WBSAMSPEECH_CRATEDESTROY2+(bmRand()%3)));
			}
			break;
		}

		// PP: on destruction of seventh normal crate
		if(WestBoss_game.numCratesDestroyed == 7)// PP: was 9, but now there are fewer crates to destroy
		{
			// PP: "this varmint's costin' me a fortune!"
			this->sayQueued(WBSAMSPEECH_CRATEDESTROY8);
		}
		
		// PP: increment reaction index
		(++this->crateDestructionReaction) %= WBSAM_NUM_CRATE_DESTRUCTION_REACTIONS;
	}

	// PP: return to idle anim afterwards
	this->queueAnim(WBSAM_ANIM_IDLE, true);
}


/*	--------------------------------------------------------------------------------
	Function 	: WBSAM::celebrate
	Purpose 	: respond to hurting taz with dynamite
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void WBSAM::celebrate(void)
{
	if(this->flags & WBSAMFLAG_CELEBRATING)
	{
		// PP: already celebrating
		return;
	}


	if(!(this->flags&WBSAMFLAG_SULKING))// PP: come out of a sulk to celebrate
	{
		if(this->playingSound())
		{
			// PP: in the middle of saying something - don't respond to this small victory
			return;
		}
	}

	this->flags |= WBSAMFLAG_CELEBRATING;
	this->flags &= ~WBSAMFLAG_PRODUCING;
	this->flags &= ~WBSAMFLAG_CHUCKING;
	this->flags &= ~WBSAMFLAG_SULKING;

	// PP: ~~~~~~~ TEMP...

	if((bmRand()%10)==0)
	{
		// PP: random laugh and taunt
		if(bmRand()&1)
		{
			wbsam.say(WBSAMSPEECH_HURTTAZ1);
		}
		else
		{
			wbsam.sayQueued(WBSAMSPEECH_HURTTAZ2);// PP: rhumba
		}
	}
	else
	{
		// PP: random laugh
		if(bmRand()&1)
		{
			wbsam.say(WBSAMSPEECH_HURTTAZ3);	
		}
		else
		{
			wbsam.say(WBSAMSPEECH_HURTTAZ4);
		}
	}

	// PP: play celebration anim
	this->playAnim("injuretaz1");// PP: belly laugh, // PP: TODO?: looping till sample finishes
	this->queueAnim(WBSAM_ANIM_IDLE, true);// PP: TEMP, TODO: blend into this only when sample finishes
}


/*	--------------------------------------------------------------------------------
	Function 	: WBSAM::draw
	Purpose 	: draw effects for sam
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void WBSAM::draw(void)
{
	if(WestBoss_game.twoPlayer)
	{
		if(gameStatus.multiplayer.currentScreen == 1)
		{
			if(!(this->flags&(WBSAMFLAG_PRODUCING|WBSAMFLAG_CHUCKING)))
			{
				bdSetIdentityObjectMatrix();// PP: taz's spin was spinning this effect round before
				bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, 0);

				if(WESTBOSSGAME::grid[this->targetY][this->targetX] == WBGRID_GROUND)
				{
					RECTANGLE	rect;

					rect.init(	WestBoss_game.gridPoints[this->targetY][this->targetX][X]-(WB_SQUARE_WIDTH*0.5f),
								WestBoss_game.gridPoints[this->targetY][this->targetX][X]+(WB_SQUARE_WIDTH*0.5f),
								WestBoss_game.gridPoints[this->targetY][this->targetX][Z]-(WB_SQUARE_HEIGHT*0.5f),
								WestBoss_game.gridPoints[this->targetY][this->targetX][Z]+(WB_SQUARE_HEIGHT*0.5f));
					//rect=rect.removeMargin(WB_SQUARE_WIDTH*0.05f);

#define FLASH_TIME		0.15f

					rect.draw(GREEN, (fmodf(gameStatus.gameTime, FLASH_TIME)>(FLASH_TIME/2.0f))?OUTLINE:ALPHAFILL, X, Z, WestBoss_game.gridPoints[this->targetY][this->targetX][Y]);

					this->humanTargetHeight=WestBoss_game.gridPoints[this->targetY][this->targetX][Y];
				}

				utilDrawCircle(VEC(this->pos()).setY(this->humanTargetHeight), this->humanThrowDistance, GREEN);

				utilDrawLine(this->pos(), (VEC(0,0,this->humanThrowDistance).rotateY(PI-this->getYang())+this->pos()).setY(this->humanTargetHeight), GREEN);

				bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, 0);
			}
		}
	}
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
															WBTAZ methods

													Taz in the Wile E West Boss game
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: minimum amount of time between being hurt by an explosion and being hurt by another explosion
#define WBTAZ_MIN_EXPLOSION_HIT_TIME		1.5f// PP: ?

// PP: min distance at which taz's 2p goal will trigger
#define WBTAZ_GOAL_PROXIMITY				METERTOUNIT(1.5f)


/*	--------------------------------------------------------------------------------
	Function 	: WBTAZ::init
	Purpose 	: initialisation
	Parameters 	: position, angle
	Returns 	: 
	Info 		: 
*/

#define WBTAZ_POS					{-1105.39f, -824.588f, -2253.82f, 1.0f}
#define WBTAZ_YANG					0.0f
#define WBTAZ_INITIAL_HEALTH		5											// PP: number of hits that Taz's crash helmet can withstand

void WBTAZ::init(void)
{
	TBVector	posVec=WBTAZ_POS;

	this->player= &(gameStatus.player[0]);
	this->actorInstance= gameStatus.player[0].actorInstance;

	this->flags=WBTAZFLAG_NONE;

	bmVectorCopy(this->pos(), posVec);
	this->setYang(WBTAZ_YANG);

	// PP: restore full health
	ResetCharactersHealth(this->actorInstance->characterInfo);

	// PP: init explosionHitTimer
	this->explosionHitTimer=0.0f;

	this->arrowTimer=0.0f;

	ACTORINSTANCE*	startInst=FindActorInstanceInMap("starting_inst", 0, &map);

	if(startInst == NULL)
	{
		bkPrintf("WBTAZ::init: no startInst\n");
	}
	else
	{
		bmVectorCopy(this->pos(), startInst->actorInstance.position);
	}

	// PP: get grid coords
	this->gridX=((int)((this->pos()[X]+(WB_WIDTH/2.0f))/WB_SQUARE_WIDTH));
	this->gridY=((int)((this->pos()[Z]+(WB_HEIGHT/2.0f))/WB_SQUARE_HEIGHT));

	// PP: create Taz's crash helmet
	CreateTazShield(this->actorInstance, WBTAZ_INITIAL_HEALTH);

	// PP: create 2player marker arrow
	{
		TBActor*		actor=NULL;

#define WBTAZ_MARKER_OBJECT		"extras\\bank_arrow.obe"

		LoadActor(WBTAZ_MARKER_OBJECT, map.mapName, 0);

		actor=FindActorInActorList(WBTAZ_MARKER_OBJECT);

		this->markerInstance=baInitActorInstance(NULL, actor, 0);

		ASSERTM(this->markerInstance, "wbtaz 2p marker arrow not found or that");

		bkPrintf("WBTAZ::init: this->markerInstance: %08x\n", this->markerInstance);

		// PP: must prep before first draw
		baPrepActorInstance(this->markerInstance,BACTORPREPFLAG_PREPNODES);
	}

	// PP: update 'score' display
	// PP: TEMP TEMP TEMP TEMP
	//((TEXTBOX*)(WestBoss_game.scoreBook.getItem("TAZSCORE")))->sprintf8("HEALTH %d", (int)(this->actorInstance->characterInfo->health.currentHealth));
//	((HEALTHBAR*)(WestBoss_game.scoreBook.getItem("HEALTHBAR")))->setHealth(int(this->actorInstance->characterInfo->health.currentHealth));

	// PP: TEMP TEMP TEMP TEMP
	//	((TEXTBOX*)(WestBoss_game.scoreBook.getItem("TAZSCORE2")))->sprintf8("HEALTH %d", (int)(this->actorInstance->characterInfo->health.currentHealth));
//	((HEALTHBAR*)(WestBoss_game.scoreBook.getItem("HEALTHBAR2")))->setHealth(int(this->actorInstance->characterInfo->health.currentHealth));
}


/*	--------------------------------------------------------------------------------
	Function 	: WBTAZ::update
	Purpose 	: update
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/

#define WBTAZ_EXPLOSIONDIVE_TERMINAL_DIST_TO_TARGETPOS		METERTOUNIT(0.5f)
#define WBTAZ_EXPLOSIONDIVE_GAMESPEED						0.5f

/* PP: REMOUT: NEW TEST - NO MOTION BLUR MAKES THE EXPLOSIONS SEEM CRISPER, HARSHER, MORE VIOLENT

#define WBTAZ_EXPLOSIONDIVE_MOTIONBLUR						0.0f

/*/
#if(BPLATFORM==PS2)
#define WBTAZ_EXPLOSIONDIVE_MOTIONBLUR						0.2f
#else// PP: if(BPLATFORM==XBOX) else
#define WBTAZ_EXPLOSIONDIVE_MOTIONBLUR						0.2f
#endif// PP: (BPLATFORM==XBOX) else
//*/

#define WBTAZ_EXPLOSIONDIVE_CRATEEXPLODE_RANGE				BEBR_CRATEEXPLODE_RANGE// PP: why not

void WBTAZ::update(void)
{
	ACTORINSTANCEUSER::update();	// PP: this updates animation-changed flag

	if(WestBoss_game.state != BGSTATE_PLAY)
	{
		return;
	}

	// PP: TEMP TEST!!!
	if(WESTBOSSGAME::grid[this->gridY][this->gridX] != WBGRID_GROUND)
	{
		bkPrintf("yar!\n");
	}

	// PP: TEMP MS FRIG
	// PP: I've got a checkpoint object now but for conwenience I'll keep using this bodgy check
	if(WestBoss_game.twoPlayer)
	{
		if(distance(exitPos, this->pos()) < WBTAZ_GOAL_PROXIMITY)
		{
			WestBoss_game.winner=0;
			WestBoss_game.loser=1;
			WestBoss_game.setState(BGSTATE_OUTRO);
			return;
		}
	}

	// PP: injury stars expire
	if(this->flags & WBTAZFLAG_INJURYSTARS)
	{
		this->timer-=fTime;

		if(this->timer < 0.0f)
		{
			this->flags &= ~WBTAZFLAG_INJURYSTARS;
			DestroyInjuryStars(this->actorInstance);
		}
	}

	if(this->flags & WBTAZFLAG_EXPLOSIONDIVE)
	{
		/*if(!this->playingAnim("fly1"))
		{
			this->flags &= ~WBTAZFLAG_EXPLOSIONFLIGHT;
		}*/

		//if(this->dist(this->explosionDive.targetPos) < WBTAZ_EXPLOSIONDIVE_TERMINAL_DIST_TO_TARGETPOS)
//		if((this->gridX==this->explosionDive.targetGridX)&&(this->gridY==this->explosionDive.targetGridY))


		// PP: TEST: to see what's going on
//		this->pos()[Y]=WestBoss_game.getHeightOnGrid(this->pos()[X], this->pos()[Z])+METERTOUNIT(2);

		// PP: TAZ SMASHES THROUGH CRATES WHILE IN EXPLOSION DIVE

		int i;
		TBVector	distVec;
		float		dist;

		for(i=0; i<WB_NUM_EXPLOSIVE_CRATES; i++)
		{
			if(wbcrate[i].exploded || wbcrate[i].finished) continue;

			bmVectorSub(distVec, wbcrate[i].pos(), this->pos());
			dist=bmVectorLen(distVec);

			if(dist < WBEB_CRATEEXPLODE_RANGE)
			{
				wbcrate[i].kaboom(this->explosionDive.dir);

				wbsam.sulk(false);
			}
		}

		// PP: AND SUPER CRATES, POURQUOI PAS
			
		for(i=0; i<WB_NUM_SUPER_TNT_CRATES-1; i++)
		{
			if(wbstnt[i].exploded || wbstnt[i].finished) continue;

			bmVectorSub(distVec, wbstnt[i].pos(), this->pos());
			dist=bmVectorLen(distVec);

			if(dist < WBEB_CRATEEXPLODE_RANGE)
			{
				wbstnt[i].kaboom(this->explosionDive.dir);

				wbsam.sulk(true);
			}
		}

		bool	reachedTarget;

		if(this->explosionDive.targetSide == 1)
		{
			reachedTarget=(this->pos()[this->explosionDive.targetAxis]<this->explosionDive.targetPosition);
		}
		else
		{
			reachedTarget=(this->pos()[this->explosionDive.targetAxis]>this->explosionDive.targetPosition);
		}

		if(reachedTarget)
		{
			// PP: if we're filming with the explosion-dive cam,
			if(WestBoss_game.camShot == WBCAM_EXPLOSIONDIVE)
			{
				// PP: make clunk noise as Taz hits camera
				PlaySample("clunk.wav", 190);

				// PP: stop taz saying 'nooo'
				//this->stopSound();
				
				// PP: "OOF!"
				this->playSound("oof.wav");

				// PP: end the shot
				WestBoss_game.setCamera(WBCAM_PLAY);

				// PP: make sam celebrate
				wbsam.celebrate();
			}

			// PP: end the dive
			wbtaz.actorInstance->actorAnimation->useMotionBone = TRUE;
			wbtaz.actorInstance->actorBehaviour->physics.flags &= (~PHYSFLAG_NOGRAVITY);
			this->flags &= ~WBTAZFLAG_EXPLOSIONDIVE;

			// PP: clear no-collide actor flag
			this->actorInstance->flags &= ~ACTORFLAG_NOCOLLISION;

			// PP: lose health
			this->loseHealthFromExplosion();

			//this->playAnim("slip");
		}
	}
	else if(this->flags & WBTAZFLAG_UPWARDSEXPLOSIONDIVE)
	{
		if(this->actorInstance->actorStatus->currentState != STATE_PROJECTILE)
		{
			// PP: end the dive
			this->flags &= ~WBTAZFLAG_UPWARDSEXPLOSIONDIVE;	

			// PP: lose health
			this->loseHealthFromExplosion();

			//this->playAnim("slip");
		}
	}

/*	FlushAnimationQueue(this->actorInstance);
	this->playAnim("recover22c");*/

	this->gridX=((int)((this->pos()[X]+(WB_WIDTH/2.0f))/WB_SQUARE_WIDTH));
	this->gridY=((int)((this->pos()[Z]+(WB_HEIGHT/2.0f))/WB_SQUARE_HEIGHT));

	// PP: update explosion-hit timer
	this->explosionHitTimer += fTime;

	// PP: update arrow timer
	this->arrowTimer += fTime;
}


/*	--------------------------------------------------------------------------------
	Function 	: WBTAZ::drawMarker
	Purpose 	: draw taz's 2player marker arrow for sam (player 2) to see him
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: hmm, not putting the arrow object on an instance list cos I don't
					know yet what sort of control I want
*/
void WBTAZ::drawMarker(void)
{
	bdSetIdentityObjectMatrix();

	// PP: don't z-test against the landscape
	SETZTEST(ALWAYS);

	bmVectorCopy(this->markerInstance->position, this->pos());

	static int yoff=600;
	this->markerInstance->position[Y]+=yoff;

	static float period=1.3f;
	bmEulerToQuatZXY(this->markerInstance->orientation, HALFPI, 0.0f, (this->arrowTimer/period)*TWOPI);

	static float scale=1.5f;
	SETVECTOR(this->markerInstance->scale, scale, scale, scale, 1.0f);

	baPrepActorInstance(this->markerInstance, 0);

	baDrawActorInstance(this->markerInstance);

	SETZTEST(LESSTHAN);
}


/*	--------------------------------------------------------------------------------
	Function 	: WBTAZ::draw
	Purpose 	: draw
	Parameters 	: 
	Returns 	: 
	Info 		: draws taz's 2player marker arrow for sam to see him
*/
void WBTAZ::draw(void)
{
	if(WestBoss_game.twoPlayer)// PP: if we're in 2player
	{
		if(gameStatus.multiplayer.currentScreen == 1)// PP: on sam's screen only
		{
			static float flash=0.2f;

			if(bmFMod(this->arrowTimer, flash)>(flash*0.5f))
			{
				this->drawMarker();// PP: draw taz's 2player marker arrow for sam (player 2) to see him
			}
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: WBTAZ::launchStateChangeCallback
	Purpose 	: function called when a character collides with something while in projectile state
	Parameters 	: ptr to actor instance
	Returns 	: 
	Info 		: must match the ProjectileStateCollisionCallback typedef
*/
void WBTAZ::projectileStateCollisionCallback(ACTORINSTANCE* const actorInstance)
{
	if(actorInstance != wbtaz.actorInstance)
	{
		return;
	}

	ACTOR_STATUS*			actorStatus;
	
	actorStatus=actorInstance->actorStatus;

	ResetCharacterAfterJump(actorInstance);
	actorInstance->actorAnimation->useMotionBone = TRUE;
	PlaySample("taz getup from slide",255,actorInstance->actorInstance.position);

	FlushAnimationQueue(actorInstance);

	if(CheckIfPlayingAnimation(actorInstance, "fly2"))
	{
		PlayAnimationByName(actorInstance, "cannonfail", 1.0f, 0, 0, 0.1f, NONE);

#ifdef PHIL// PP: TEMP TEST
		bkPrintf("WBTAZ::projectileStateCollisionCallback playing anim 'cannonfail'\n");
#endif// PP: def PHIL
	}
	else
	{
		FlushAnimationQueue(actorInstance);
		PlayAnimationByName(actorInstance, "getup", 1.0f, 0, 0, 0.1f, NONE);

#ifdef PHIL// PP: TEMP TEST
		bkPrintf("WBTAZ::projectileStateCollisionCallback playing anim 'getup'\n");
#endif// PP: def PHIL
	}

	StopChannel(&actorStatus->soundHandle[0]);
	actorStatus->lastState = actorStatus->currentState;
	actorStatus->currentState = STATE_GETUPFROMSLIDE;
	SetDefaultCameraAngleAboveHorizontal(camera,RAD(0.0f));
	//-----------------STATE CHANGE-----------------

	// PP: NOW UN-REGISTER THIS CALLBACK TO PUT PROJECTILE-STATE COLLISIONS BACK TO NORMAL
	projectileStateCustomCollisionCallback=NULL;
}


/*	--------------------------------------------------------------------------------
	Function 	: WBTAZ::launchStateChangeCallback
	Purpose 	: function called when Taz is changes state to be launched into the air by an explosion
	Parameters 	: ptr to actor instance
	Returns 	: 
	Info 		: 
*/
void WBTAZ::launchStateChangeCallback(ACTORINSTANCE *actorInstance)
{
	// PP: actorInstance param can be ignored - you know damn well it belongs to wbtaz so let's not get anal about this

	FlushAnimationQueue(wbtaz.actorInstance);

	wbtaz.playAnim("fly1", true);

	if(wbtaz.flags & WBTAZFLAG_EXPLOSIONDIVE)// PP: horizontal dive
	{
		wbtaz.actorInstance->actorAnimation->useMotionBone = FALSE;

		wbtaz.actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_NOGRAVITY;

		wbtaz.actorInstance->flags|=ACTORFLAG_NOCOLLISION;
	}

	projectileStateCustomCollisionCallback=WBTAZ::projectileStateCollisionCallback;
}


/*	--------------------------------------------------------------------------------
	Function 	: WBTAZ::getHitByExplosion
	Purpose 	: get hit, right...BY AN EXPLOSION!  *gasp!*
	Parameters 	: (opt/NULL=no branch, ie. centre of blast) ptr to branch info of fireball hitting taz
	Returns 	: 
	Info 		: 
*/
#define WBTAZ_EXPLOSIONDIVE_MIN_SHOT_DISTANCE		METERTOUNIT(5)	// PP: minimum distance from which it's worth cutting to the explosion dive cam

void WBTAZ::getHitByExplosion(const WBE_FIREBALLBRANCHINFO* const branchInfo)
{
	if(this->flags&(WBTAZFLAG_EXPLOSIONDIVE|WBTAZFLAG_UPWARDSEXPLOSIONDIVE))
	{
		return;
	}

	// PP: ignore if in outro state
	if(WestBoss_game.state == BGSTATE_OUTRO)
	{
		return;
	}

	// PP: clear collision flags
	this->actorInstance->actorBehaviour->physics.coll = 0;

	if(branchInfo == NULL)// PP: straight up - I think they did one like this in The Rock somewhere
	{
	//	bkPrintf("# # # # # VERTICAL ************\n");

		// PP: TEMP TEST FRIG THING
// PP: 		this->pos()[Y]+=METERTOUNIT(2);

		SETVECTOR(this->vel(), 0.0f, METERTOUNIT(11), 0.0f, 0.0f);

		this->actorInstance->actorAnimation->useMotionBone = FALSE;

		// PP: set upwardsexplosiondive flag - MUST BE DONE BEFORE STATE CHANGE
		this->flags |= WBTAZFLAG_UPWARDSEXPLOSIONDIVE;

		// PP: request state change
		RequestStateChangePlus(this->actorInstance,STATE_PROJECTILE,WBTAZ::launchStateChangeCallback);

		// PP: set Action-Movie-Dive-From-Explosion-in-Slow-Motion™ cam
//		WestBoss_game.setCamera(WBCAM_UPWARDSEXPLOSIONDIVE);

		// PP: raise taz a bit
		this->pos()[Y]+=WBTAZ_EXPLOSIONDIVE_TARGET_HEIGHT;
	}
	else// PP: horizontally - like in every action movie ever made
	{

		//bkPrintf("********** HORIZONTAL ************\n");

		float			flightSpeed;

		// PP: set up camera position for explosion dive shot...

		TBVector2D		gridPos;
	
		gridPos[X]=branchInfo->extendedTargetX+0.5f;
		gridPos[Y]=branchInfo->extendedTargetY+0.5f;

		WestBoss_game.evaluateGridPos(gridPos, this->explosionDive.camPos);

		// PP: record direction of dive
		this->explosionDive.dir=branchInfo->getDir();

		if((branchInfo->dir == WBEDIRECTION_UP)||(branchInfo->dir == WBEDIRECTION_DOWN))
		{
			this->explosionDive.targetAxis=Z;
		}
		else
		{
			this->explosionDive.targetAxis=X;
		}

		if((branchInfo->dir == WBEDIRECTION_LEFT)||(branchInfo->dir == WBEDIRECTION_DOWN))
		{
			this->explosionDive.targetSide=1;// PP: stay above
		}
		else
		{
			this->explosionDive.targetSide= -1;// PP: stay below
		}

		this->explosionDive.targetPosition=this->explosionDive.camPos[this->explosionDive.targetAxis];


		// PP: raise the camera from the ground a bitty
		this->explosionDive.camPos[Y]+=WBTAZ_EXPLOSIONDIVE_CAMERA_HEIGHT;

		// PP: set up target pos

		TBVector	targetPos;

		bmVectorCopy(targetPos, this->explosionDive.camPos);
		targetPos[Y]-=TAZ_HEIGHT;

		TBVector	distVec;

		bmVectorSub(distVec, targetPos, this->pos());
		

		// PP: set velocity (towards target pos)...

		flightSpeed=WBE_BRANCH_SPEED*1.0f;

		// PP: don't go into explosion dive shot if in supercrate explode shot
		if((WestBoss_game.camShot!=WBCAM_SUPERCRATEEXPLODE) && (bmVectorLen(distVec) > METERTOUNIT(1)))// PP: dist vec is big enough to safely scale into a velocity vector
		{
			bmVectorScaleToLength(this->vel(), distVec, flightSpeed);

			// PP: set explosiondive flag - MUST BE DONE BEFORE STATE CHANGE
			this->flags |= WBTAZFLAG_EXPLOSIONDIVE;

			// PP: request state change
			RequestStateChangePlus(this->actorInstance,STATE_PROJECTILE,WBTAZ::launchStateChangeCallback);

			// PP: set no-collide actor flag
			wbtaz.actorInstance->flags|=ACTORFLAG_NOCOLLISION;

			// PP: how far will Taz be from the explosion dive camera to start with - far enough to get a decent shot do you think? hmm?...
			if(bmVectorDistance(this->pos(), this->explosionDive.camPos) > WBTAZ_EXPLOSIONDIVE_MIN_SHOT_DISTANCE)
			{
				// PP: turn to face camera
				this->YrotateToFace(this->explosionDive.camPos);

				// PP: set Action-Movie-Dive-From-Explosion-in-Slow-Motion™ cam
				WestBoss_game.setCamera(WBCAM_EXPLOSIONDIVE);

				// PP: do the "NOOOOO" thing

				//PlayTrackingSample("sam_voc_03a.wav", 255, this->pos(), 0, 4500);//temp?
				//PlayTrackingSample("explo_gen_01c.wav",255, this->pos(), 0, 3000);// PP: temp

				this->playSound("explosiondive.wav");

				PlaySample("slowboom.wav",255);

				// PP: explosion-dive shot initiated - wait till taz hits the camera before starting sam's celebration
				return;
			}
		}
	}

	// PP: check to prevent the same fireball knocking off more than one health point
	if(this->explosionHitTimer > WBTAZ_MIN_EXPLOSION_HIT_TIME)
	{
		bkPrintf("WBTAZ::getHitByExplosion: ***** LOSING HEALTH ******\n");

		// PP: OW!
		switch(bmRand()%3)
		{
		case 0:
			{
				this->playSound("zoohubintro2_2.wav");
			}
			break;

		case 1:
			{
				this->playSound("zoohuboutro_2_alt_1.wav");
			}
			break;

		case 2:
			{
				this->playSound("zoohuboutro_2_alt_2.wav");
			}
			break;
		}

		// PP: NOT DOING AN EXPLOSIONDIVE SHOT
		// PP: make sam celebrate
		wbsam.celebrate();
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: WBTAZ::shutdown
	Purpose 	: shutdown
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void WBTAZ::shutdown(void)
{
	baFreeActorInstance(this->markerInstance);
	projectileStateCustomCollisionCallback=NULL;
}


/*	--------------------------------------------------------------------------------
	Function 	: WBTAZ::loseHealthFromExplosion
	Purpose 	: lose health from being hit by an explosion
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void WBTAZ::loseHealthFromExplosion(void)
{
	// PP: take some health off
	// PP: REMOUT: HEALTH DOESN'T REALLY EXIST AS SUCH NO MORE		ReduceCharactersHealth(this->actorInstance, 2);

	// PP: check to prevent the same fireball knocking off more than one health point
	if(this->explosionHitTimer <= WBTAZ_MIN_EXPLOSION_HIT_TIME)
	{
		return;
	}

	// PP: GOT THIS INSTEAD THOUGH
	// CMD: ReduceShieldHealth... now returns the remaing health ... so zero == dead
	if(!ReduceShieldHealthByOne(this->actorInstance))
	{
		// PP: milestone bodgery
		WestBoss_game.winner=1;
		WestBoss_game.loser=0;

		WestBoss_game.setState(BGSTATE_OUTRO);
	}

//	if(this->actorInstance->characterInfo

	// PP: add injury stars
// PP: REMOUT: TEMP TEST	CreateInjuryStars(this->actorInstance);

	this->timer=WBTAZ_DURATION_INJURYSTARS;
	this->flags |= WBTAZFLAG_INJURYSTARS;

	// PP: update 'score' display

	// PP: TEMPY TEMPY
	//((TEXTBOX*)(WestBoss_game.scoreBook.getItem("TAZSCORE")))->sprintf8("HEALTH %d", (int)(this->actorInstance->characterInfo->health.currentHealth));
	// PP: REMOUT: OOD((HEALTHBAR*)(WestBoss_game.scoreBook.getItem("HEALTHBAR")))->setHealth(int(this->actorInstance->characterInfo->health.currentHealth));

	// PP: TEMPY TEMPY
	//((TEXTBOX*)(WestBoss_game.scoreBook.getItem("TAZSCORE2")))->sprintf8("HEALTH %d", (int)(this->actorInstance->characterInfo->health.currentHealth));
	// PP: REMOUT: OOD((HEALTHBAR*)(WestBoss_game.scoreBook.getItem("HEALTHBAR2")))->setHealth(int(this->actorInstance->characterInfo->health.currentHealth));

	// PP: TEMP TEST
//	WestBoss_game.scoreBook.realign();

	// PP: switch to recovery state
//			RequestStateChange(this->actorInstance,STATE_RECOVER);
	
	// PP: reset explosion-hit timer
	this->explosionHitTimer=0.0f;
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														WBDYNAMITE methods

												Sticks of dynamite thrown by Sam.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


// PP: TEMP?: radius of Zoo Keeper for shadow map
#define SHADOWMAP_RADIUS_DYNAMITE			METERTOUNIT(0.9f)//METERTOUNIT(1.25f)
#define SHADOW_RADIUS_DYNAMITE				METERTOUNIT(0.3f)
#define SHADOW_HEIGHT_DYNAMITE				METERTOUNIT(10)

#define WBDYNAMITE_FUSELENGTH_TWOPLAYER		1.0f				// PP: length of all dynamite fuses in 2-player (seconds)

// PP: longest (initial) dynamite fuse lengths
float WBDYNAMITE::slowFuseLengths[NUM_DIFFICULTY_LEVELS]=
{
	5.0f,		// PP: easy
	4.0f,		// PP: medium
	3.0f,		// PP: hard
};

// PP: shortest (final) dynamite fuse lengths
float WBDYNAMITE::fastFuseLengths[NUM_DIFFICULTY_LEVELS]=
{
	1.5f,		// PP: easy
	1.25f,		// PP: medium
	0.9f,		// PP: hard
};


/*	--------------------------------------------------------------------------------
	Function 	: WBDYNAMITE::init
	Purpose 	: initialisation
	Parameters 	: position, y angle
	Returns 	: 
	Info 		: 
*/
#define WBDYNAMITE_SCALE				0.3f

#define WBDYNAMITE_X_ROT_SPEED			9.0f
#define WBDYNAMITE_MIN_Y_ROT_SPEED		4.0f
#define WBDYNAMITE_MAX_Y_ROT_SPEED		6.0f

void WBDYNAMITE::init(TBVector posIn, const float yAngIn)
{
	// PP: set pos & ang
	bmVectorCopy(this->pos(), posIn);
	this->setYang(yAngIn);

//	TBVector	offset={0,0,0,0};
//	SetActorCollSphere(this->actorInstance->actorBehaviour, offset, WBDYNAMITE_COLLISION_RADIUS);

// PP: REMOUT: TEMP			AddActorToCollisionSphereCache(&collisionCache, this->actorInstance);

	// PP: set up rotation speed
	this->rotSpeed[X]=WBDYNAMITE_X_ROT_SPEED;
	this->rotSpeed[Y]=WBDYNAMITE_MIN_Y_ROT_SPEED+((bmRand()/RANDMAX)*(WBDYNAMITE_MAX_Y_ROT_SPEED-WBDYNAMITE_MIN_Y_ROT_SPEED));

	// PP: set up rotation vector
	this->rotVec[X]=0.0f;
	this->rotVec[Y]=(bmRand()/RANDMAX)*TWOPI;

	// PP: set y angle
	this->yAng=yAngIn;

	// PP: NEW TeST: PREP TO AVOID FLASH OF OLD POSITION
	baPrepActorInstance(&this->actorInstance->actorInstance, 0);

	this->setState(WBDYNAMITESTATE_LIT);
}


/*	--------------------------------------------------------------------------------
	Function 	: WBDYNAMITE::init
	Purpose 	: initialisation
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void WBDYNAMITE::init(void)
{
	ACTORINSTANCEUSER::init(WBDYNAMITE_MODEL, BGACT_BEHAVIOUR|BGACT_CARTOON);

	// PP: apply scale adjustment
	this->setScale(WBDYNAMITE_SCALE);

	CreateActorInstanceShadow(this->actorInstance, SHADOW_RADIUS_DYNAMITE, SHADOW_HEIGHT_DYNAMITE, SHADOWMAP_RADIUS_DYNAMITE);

	//	StopCollidingWith(this->actorInstance, WestBoss_game.collisionMesh, 10.0f);
	this->actorInstance->flags|=ACTORFLAG_NOWORLDCOLLISION;

	SetActorFriction(this->actorInstance->actorBehaviour,METERTOUNIT(-20.0f));

	SetActorMinFricAngle(this->actorInstance->actorBehaviour, 0.7f);

	this->setState(WBDYNAMITESTATE_FINISHED);

	// PP: switch off the dynamite actorInstance till it gets chucked
	this->actorInstance->flags |= (ACTORFLAG_DONTDRAW|ACTORFLAG_NOCOLLISION);

	// PP: init explosion ptr
	this->wbExplosion=NULL;
}


/*	--------------------------------------------------------------------------------
	Function 	: WBDYNAMITE::plant
	Purpose 	: plant the dynamite on a specific grid square
	Parameters 	: grid x pos, grid y pos
	Returns 	: 
	Info 		: 
*/
void WBDYNAMITE::plant(const int gridXin, const int gridYin)
{
	this->gridX=gridXin;
	this->gridY=gridYin;

	// PP: calculate actual position from grid coords
	TBVector2D		gridPos;

	gridPos[X]=this->gridX+0.5f;
	gridPos[Y]=this->gridY+0.5f;

	WestBoss_game.evaluateGridPos(gridPos, this->pos());

	this->pos()[Y]+=WBDYNAMITE_COLLISION_RADIUS;

	// PP: update target pos, just in case
	bmVectorCopy(this->targetPos, this->pos());

	// PP: set y angle
	this->yAng=RANDOMANG;

	// PP: switch on the dynamite actorInstance
	this->actorInstance->flags &= ~(ACTORFLAG_DONTDRAW);

	this->setState(WBDYNAMITESTATE_LIT);
}


/*	--------------------------------------------------------------------------------
	Function 	: WBDYNAMITE::kaboom
	Purpose 	: explode
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void WBDYNAMITE::kaboom(void)
{
	this->setState(WBDYNAMITESTATE_FINISHED);

	// PP: stop fuse sound
	this->stopSound();

	// PP: CREATE A BOMBERMAN EXPLOSION!~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if(WESTBOSSGAME::grid[this->gridY][this->gridX] != WBGRID_GROUND)
	{
		// PP: if we couldn't create a bomberman explosion because of where we landed, use a normal explosion instead
		// PP: NOTE! this will not hurt player 1, it's just for effect.  Wicked.
		Fireball_normalExplosion(this->pos());
	}
	else
	{
		// PP: boom!
		this->wbExplosion->start();
	}

	// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// PP: destroy any links to this actorinstance
	if(this == wbsam.introDynamite)
	{
		wbsam.introDynamite=NULL;
	}
	
	// PP: switch off the dynamite actorInstance
	this->actorInstance->flags |= (ACTORFLAG_DONTDRAW|ACTORFLAG_NOCOLLISION);
}


/*	--------------------------------------------------------------------------------
	Function 	: WBDYNAMITE::setState
	Purpose 	: set state
	Parameters 	: new state for the dynamite
	Returns 	: 
	Info 		: 
*/
void WBDYNAMITE::setState(const WBDynamiteState stateIn)
{
	switch(stateIn)
	{
	case WBDYNAMITESTATE_LIT:
		{
			// PP: re-enable gravity
			this->actorInstance->actorBehaviour->physics.flags &= ~(PHYSFLAG_NOGRAVITY);
			this->actorInstance->flags &= ~ACTORFLAG_NOVELOCITY;

			// PP: in twoplayer, always use the WBDYNAMITE_FUSELENGTH_TWOPLAYER value
			if(WestBoss_game.twoPlayer)
			{
				this->timer=WBDYNAMITE_FUSELENGTH_TWOPLAYER;
			}
			else
			{
				this->timer=(WBDYNAMITE::fastFuseLengths[CURRENT_DIFFICULTY]+(((wbsam.score->getScore()-1)/4.0f)*(WBDYNAMITE::slowFuseLengths[CURRENT_DIFFICULTY]-WBDYNAMITE::fastFuseLengths[CURRENT_DIFFICULTY])));
			}

			// PP: play burn-out anim - TODO: adjust speed to match fuse length
			//this->playAnim("fuzeburning");

			// PP: play fuse noise
			//PlayTrackingSample("bombfuse.wav",255, this->pos(), 0);
			this->playSound("bombfuse.wav", AIU_PLAYSOUND_DEFAULT_FLAGS, 127);
		}
		break;

	case WBDYNAMITESTATE_LANDED:
		{
			this->actorInstance->actorBehaviour->physics.flags |= (PHYSFLAG_NOGRAVITY);
			this->actorInstance->flags |= ACTORFLAG_NOVELOCITY;

			if(WESTBOSSGAME::grid[this->gridY][this->gridX] == WBGRID_GROUND)
			{
				this->wbExplosion=WestBossExp_create(this->gridX, this->gridY, 3);
			}

			// PP: if this is the intro dynamite, switch to ingame cam now
			if(this == wbsam.introDynamite)
			{
		//		WestBoss_game.setCamera(WBCAM_PLAY);
			}
		}
		break;
	}

	this->state=stateIn;
}






/*	--------------------------------------------------------------------------------
	Function 	: WBDYNAMITE::update
	Purpose 	: udpate
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void WBDYNAMITE::update(void)
{
	switch(this->state)
	{
	case WBDYNAMITESTATE_LIT:
		{
			if(this->actorInstance != NULL)
			{
				if(this->pos()[Y]<(this->targetPos[Y]+WBDYNAMITE_COLLISION_RADIUS))
				{
					this->setState(WBDYNAMITESTATE_LANDED);
				}
			}

			// PP: update spin through air...

			this->rotVec[X]+=(this->rotSpeed[X]*fTime);
			this->rotVec[Y]+=(this->rotSpeed[Y]*fTime);

			// PP: apply rotation

			TBQuaternion	xRot;	// PP: spin
			TBQuaternion	yRot;	// PP: twist
			TBQuaternion	tempQuat;

			// PP: twist then spin
			bmQuatXRotation(xRot, this->rotVec[X]);
			bmQuatYRotation(yRot, this->rotVec[Y]);
			bmQuatMultiply(tempQuat, yRot, xRot);// PP: swap test

			// PP: then rotate to y ang of flight
			bmQuatYRotation(yRot, this->yAng+HALFPI);
			bmQuatMultiply(this->ang(), tempQuat, yRot);
		}
		break;

	case WBDYNAMITESTATE_LANDED:
		{
			this->timer-=fTime;

			if(this->actorInstance != NULL)
			{
				SETVECTOR(this->vel(), 0.0f, 0.0f, 0.0f, 0.0f);
				bmVectorCopy(this->pos(), this->targetPos);
			}

			if(this->timer < 0.0f)
			{
				this->kaboom();
			}
		}
		break;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: WBDYNAMITE::draw
	Purpose 	: draw
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void WBDYNAMITE::draw(void)
{
	if((!WestBoss_game.twoPlayer)		// PP: no fuse timer in 2 player - no point cos its so short; also projection is wrong
		&&(!WestBoss_game.movieMode))	// PP: no fuse timers in 'movie'-style shots
	{
		// PP: draw fuse timer above dynamite...

		TBVector	dest;
		TBVector	src;

		if(this->state == WBDYNAMITESTATE_LANDED)
		{
			if(this->timer > 0.0f)
			{
				// PP: DRAW THE COUNTDOWN **********************
				// PP: TODO: PRESENT THIS WITH SOME FLAIR FOR GOD'S SAKE!!!!!!!!

				// PP: NOTE: IT'S CRAZY TO BE SETTING THIS STUFF UP FOR EACH BIT OF DYNAMITE DRAWN - I KNOW
				// PP: I'M IN A HURRY NOW.  THIS WAS ONLY MEANT TO BE PLACEHOLDER BUT NOW IT STAYS

				bmVectorCopy(src, this->pos());

				src[W]=1.0f;

				// PP: IMPORTANT: SET 3D MODE BEFORE CALLING bdProjectVertices!!!!!!
				bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
				bdSetIdentityObjectMatrix();
				bdProjectVertices(dest, src, 1);
				bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);

				bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
				bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
				bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
				bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
				bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

				ushort		string16[3];
				char		string8[3];

				sprintf(string8, "%d", ROUNDUP(this->timer));

				bkString8to16(string16, string8);

#define WBDYNAMITE_COUNTDOWN_SCALE		2.0f

				// PP: adjust screen coords to compensate for scale
				dest[X]/=WBDYNAMITE_COUNTDOWN_SCALE;
				dest[Y]/=WBDYNAMITE_COUNTDOWN_SCALE;

				// PP: shift the number pos up a bit so's the number doesn't obscure the dynamite
				dest[Y]+=40.0f;

				// PP: and left a bit to centre number
				dest[X]-=10.0f;

				// PP: Apply scale to the number

				TBMatrix	scaleMat;

				bmMatScale(scaleMat, WBDYNAMITE_COUNTDOWN_SCALE, WBDYNAMITE_COUNTDOWN_SCALE, 1.0f);
				bdSetObjectMatrix(scaleMat);

				// PP: Draw the number

#define WBDYNAMITE_COUNTDOWN_ALPHA		80

				COLOUR	col;

				if(WestBoss_game.twoPlayer)
				{
					col=COLOUR(255, 0, 0, WBDYNAMITE_COUNTDOWN_ALPHA);
				}
				else
				{
					COLOUR	srcCol=COLOUR(0,255,0);
					COLOUR	destCol=COLOUR(255,0,0);

					float	interp=(WBDYNAMITE::slowFuseLengths[CURRENT_DIFFICULTY]-this->timer)/WBDYNAMITE::slowFuseLengths[CURRENT_DIFFICULTY];

					col=COLOUR(srcCol, destCol, interp, true);
					
					col.a=WBDYNAMITE_COUNTDOWN_ALPHA;
				}

				bdPrintFontLite(standardFont, string16, strlen(string8), dest[X], dest[Y], col.r, col.g, col.b, col.a, NULL, NULL);
				
				// PP: reset some stuff
				bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
				bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
				bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
				bdSetIdentityObjectMatrix();
			}
		}
	}
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													WBTNTCRATE methods

											Exploding crates and Super-TNT crates
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: WBTNTCRATE::initFragments
	Purpose 	: initialise a tnt crate's fragments
	Parameters 	: (opt/very root)ptr to root fragment, (opt/NULL=top level)fragment index (for recursion)
	Returns 	: 
	Info 		: 
*/
void WBTNTCRATE::initFragments(TBActorNodeInstance* rootNode, int* const fragmentIndexIn)
{
	TBActor					*actor = this->actorInstance->actorInstance.actor;
	TBActorNodeInstance		*instanceNode;

	int						fragmentIndex;

	WBTNTCRATEFRAGMENT*		fragment;

	if(rootNode == NULL)
	{
		rootNode=this->actorInstance->actorInstance.rootNodeInstance->children;
	}

	if(fragmentIndexIn == NULL)
	{
		fragmentIndex=0;
	}
	else
	{
		fragmentIndex = *fragmentIndexIn;
	}

	// traverse the list
	instanceNode = rootNode;
	do
	{
		// if this node isn't hidden
		if(!(instanceNode->flags & BNODEINSTFLAG_HIDDEN))
		{
			// what type of node is it?
			if(instanceNode->actorNode->type == TBACTORNODE_MESH)
			{
				fragment= &this->fragments[fragmentIndex++];

				fragment->finished=false;

				fragment->node=instanceNode;

				bmVectorCopy(fragment->pos, fragment->node->animState.position);

				instanceNode = instanceNode->next;
			}
		}

		// recurse if this node has any children
		if (instanceNode->children)
		{
			this->initFragments(instanceNode->children, &fragmentIndex);
		}

		if(fragmentIndexIn != NULL)
		{
			*fragmentIndexIn = fragmentIndex;
		}
	}
	while (instanceNode != rootNode);
}


/*	--------------------------------------------------------------------------------
	Function 	: WBTNTCRATE::launchFragments
	Purpose 	: launch a tnt crate's fragments as it explodes
	Parameters 	: y angle of blast causing crate to explode
	Returns 	: 
	Info 		: 
 */
#define LAUNCHFRAGS_MIN_SPEED		METERTOUNIT(5)
#define LAUNCHFRAGS_MAX_SPEED		METERTOUNIT(10)
#define LAUNCHFRAGS_MIN_Y_SPEED		METERTOUNIT(5)
#define LAUNCHFRAGS_MAX_Y_SPEED		METERTOUNIT(9)
#define LAUNCHFRAGS_ANGLE_RANGE		0.7f					// PP:  +/-
#define LAUNCHFRAGS_MAX_ROTSPEED	(TWOPI)

void WBTNTCRATE::launchFragments(const float yAng)
{
	WBTNTCRATEFRAGMENT*		fragment;
	float					fragSpeed;
	float					fragYang;
//	TBQuaternion			tempQuat;
//	TBQuaternion			xRot, yRot, zRot;

	for(int i=0;  i<this->numFragments; i++)
	{
		fragment= &this->fragments[i];
		
		fragSpeed=LAUNCHFRAGS_MIN_SPEED+(bmRandf()*(LAUNCHFRAGS_MAX_SPEED-LAUNCHFRAGS_MIN_SPEED));

		fragYang=yAng-LAUNCHFRAGS_ANGLE_RANGE+(2*bmRandf()*LAUNCHFRAGS_ANGLE_RANGE);

		fragment->vel[Y]=LAUNCHFRAGS_MIN_Y_SPEED+(bmRandf()*(LAUNCHFRAGS_MAX_Y_SPEED-LAUNCHFRAGS_MIN_Y_SPEED));
		fragment->vel[X]=fragSpeed*bmSin(fragYang);
		fragment->vel[Z]=fragSpeed*bmCos(fragYang);

		SETVECTOR(fragment->rotVelVec, LAUNCHFRAGS_MAX_ROTSPEED-(bmRandf()*2*LAUNCHFRAGS_MAX_ROTSPEED),
										LAUNCHFRAGS_MAX_ROTSPEED-(bmRandf()*2*LAUNCHFRAGS_MAX_ROTSPEED),
										0.0f,
										0.0f);

		SETVECTOR(fragment->rotVec, 0,0,0,0);

		bmVectorCopy(fragment->pos, fragment->node->animState.position);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: countFragments
	Purpose 	: count the number of chunks forming an actor
	Parameters 	: (opt/NULL=very root) ptr to root fragment
	Returns 	: number of chunks forming an actor
	Info 		: 
*/
int WBTNTCRATE::countFragments(TBActorNodeInstance* rootNode)
{
	TBActor					*actor = this->actorInstance->actorInstance.actor;
	TBActorNodeInstance		*instanceNode;
	int						numFragments=0;

	if(rootNode == NULL)
	{
		rootNode=this->actorInstance->actorInstance.rootNodeInstance->children;
	}

	// traverse the list
	instanceNode = rootNode;
	do
	{
		// if this node isn't hidden
		if(!(instanceNode->flags & BNODEINSTFLAG_HIDDEN))
		{
			// what type of node is it?
			if(instanceNode->actorNode->type == TBACTORNODE_MESH)
			{
				numFragments++;

				instanceNode = instanceNode->next;

				// recurse if this node has any children
				if (instanceNode->children)
				{
					numFragments += countFragments(instanceNode->children);
				}
			}
		}
	}
	while (instanceNode != rootNode);

	return numFragments;
}


/*	--------------------------------------------------------------------------------
	Function 	: WBTNTCRATE::updateFragments
	Purpose 	: update fragments of an exploded crate
	Parameters 	: void
	Returns 	: 
	Info 		: 
*/
void WBTNTCRATE::updateFragments(void)
{
	WBTNTCRATEFRAGMENT*		fragment;
	TBVector				tempVec;
	TBQuaternion			tempQuat;
//	TBQuaternion			tempQuat2;
	TBQuaternion			xRot, yRot, zRot;

	// PP: pretend finished==true until we find a fragment that isn't
	this->finished=true;

	this->boundingBox.init();

	for(int i=0; i< this->numFragments; i++)
	{
		fragment= &this->fragments[i];

		this->boundingBox.expandToContain(fragment->pos);

		if(fragment->finished) continue;

		if(fragment->pos[Y] < WBTNTCRATE_FRAGMENT_FINISHED_HEIGHT)
		{
			fragment->finished=true;
			continue;
		}

		// PP: we've found a fragment that isn't finished, so the crate isn't finished neivah
		this->finished=false;

		// PP: ROTATION ~~~~~~~~~~~~~~

		bmVectorCMul(tempVec, fragment->rotVelVec, fTime);

		fragment->rotVec[X]=Aabs(fragment->rotVec[X]+tempVec[X]);
		fragment->rotVec[Y]=Aabs(fragment->rotVec[Y]+tempVec[Y]);
		fragment->rotVec[Z]=Aabs(fragment->rotVec[Z]+tempVec[Z]);

		bmQuatXRotation(xRot, fragment->rotVec[X]);
		bmQuatYRotation(yRot, fragment->rotVec[Y]);
		bmQuatZRotation(zRot, fragment->rotVec[Z]);

		bmQuatMultiply(tempQuat, xRot, yRot);
		bmQuatMultiply(fragment->node->animState.orientation, tempQuat, zRot);

		// PP: TRANSLATION ~~~~~~~~~~~

		fragment->vel[Y] += NORMAL_WORLD_GRAVITY*fTime;

		bmVectorCMul(tempVec, fragment->vel, fTime);

		bmVectorAdd(fragment->pos, fragment->pos, tempVec);

		bmVectorCopy(fragment->node->animState.position, fragment->pos);
	}

	baPrepActorInstance(&this->actorInstance->actorInstance, TRUE);// PP: try true
}


/*	--------------------------------------------------------------------------------
	Function 	: WBTNTCRATE::drawFragments
	Purpose 	: draw fragments of an exploded crate
	Parameters 	: 
	Returns 	: 
	Info 		: forces clipping while drawing the fragments
*/
void WBTNTCRATE::drawFragments(void)
{
	bdSetRenderState(BDRENDERSTATE_FORCECLIP, TRUE, 0);
	bdSetRenderState(BDRENDERSTATE_FORCECLIP, TRUE, 1);

	/*
	baDrawActorInstance(&this->actorInstance->actorInstance, BACTPRIMALPHATYPE_SOLID);
	/*/

	// PP: test visiblity of fragment bounding box
	if(this->boundingBox.visible(this->actorInstance->actorInstance.objectToWorld))
	{
		/*
		// PP: this is the gist of baDrawActorInstance
		bSetupActorNodeList(this->actorInstance->actorInstance.rootNodeInstance, &(this->actorInstance->actorInstance), BACTPRIMALPHATYPE_ALL);
		/*/
		baDrawActorInstance(&(this->actorInstance->actorInstance), BACTPRIMALPHATYPE_ALL, 0);// PP: last parm (no flags): this overrides default bounding box test
		//*/
	}

	//*/

	bdSetObjectMatrix(this->actorInstance->actorInstance.objectToWorld);

	//this->boundingBox.draw();

	bdSetRenderState(BDRENDERSTATE_FORCECLIP, FALSE, 0);
	bdSetRenderState(BDRENDERSTATE_FORCECLIP, FALSE, 1);
}


/*	--------------------------------------------------------------------------------
	Function 	: WBTNTCRATE::init
	Purpose 	: initialisation
	Parameters 	: name of crate instance, crate type (WBCRATETYPE_...)
	Returns 	: 
	Info 		: this overrides ACOTRINSTANCEUSER::init
*/
void WBTNTCRATE::init(char* const name, WBTNTCrateType typeIn)
{
	//
	this->actorInstance=FindActorInstanceInMap(name, 0, &map);
	/*/
	ACTORINSTANCEUSER::init(FindActorInstanceInMap(name, 0, &map), BGACT_COLLCACHE|BGACT_CARTOON);
	//*/

	// PP: determine the number of fragments in this crate's exploding model...
	
	this->numFragments=0;
	this->fragments=NULL;

	// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if(this->actorInstance == NULL)
	{
		this->gridX=0;
		this->gridY=0;
	}
	else
	{
		// PP: get rounded grid coords
		this->gridX=WESTBOSSGAME::getGridX(this->pos()[X]);		
		this->gridY=WESTBOSSGAME::getGridY(this->pos()[Z]);
	}

	// PP: set crate type
	this->type=typeIn;

	this->notUsed=false;
	this->exploded=false;
	this->finished=false;
}


/*	--------------------------------------------------------------------------------
	Function 	: WBTNTCRATE::kaboom
	Purpose 	: explode
	Parameters 	: angle of blast causing crate to explode
	Returns 	: 
	Info 		: 
*/
#define WBCRATESHOCK_NUMRINGS		3

// PP: colours of westboss crate explosion shockwaves
static COLOUR			WestBoss_crateShockwaveColours[WBCRATESHOCK_NUMRINGS]={COLOUR(255,200,0), COLOUR(200,128,0), COLOUR(70,70,70)};

// PP: Dynamite shockwave parameters
#define WBCRATESHOCK_MAXRADIUS		METERTOUNIT(4.5f)
#define WBCRATESHOCK_STARTSPEED		METERTOUNIT(1.1f)
#define WBCRATESHOCK_ENDSPEED		METERTOUNIT(0.3f)
#define WBCRATESHOCK_NUMRINGS		3
#define WBCRATESHOCK_INTERVAL		0.07f
#define WBCRATESHOCK_STARTWIDTH		METERTOUNIT(0.0f)
#define WBCRATESHOCK_ENDWIDTH		METERTOUNIT(2.5f)

void WBTNTCRATE::kaboom(const float yAng)
{
	// PP: TEMP TEST
//	return;

	// PP: UNREMOUT: NEW TEST
	// PP: MAKE A SHOCKWAVE EFFECT
/*	Shockwave_create(this->pos(),
						WBCRATESHOCK_MAXRADIUS,
						WBCRATESHOCK_STARTSPEED,
						WBCRATESHOCK_ENDSPEED,
						WBCRATESHOCK_NUMRINGS,
						WBCRATESHOCK_INTERVAL,
						WBCRATESHOCK_STARTWIDTH,
						WBCRATESHOCK_ENDWIDTH,
						&WestBoss_crateShockwaveColours[0]);*/

	// PP: explode the actor instance
	RespondToDestructibleCollision(NULL, this->actorInstance);


	// PP: NEW TEST: trigger Calude's fireworks

	COLOUR	fireworkColour;
	RANDOM_PRETTY_WINDOWS_COLOUR(fireworkColour);
	CreateFireworkBurst(VEC(this->pos()).moveY(METERTOUNIT(1)), FIREWORK_RANDOMTYPE, fireworkColour.r, fireworkColour.g, fireworkColour.b, 0);
	//CreateFireworkBurst(VEC(this->pos()).moveY(METERTOUNIT(1)), FIREWORK_RANDOMTYPE, 0, 0, 0, FIREWORKS_RANDOMCOLOUR);

	// PP: set custom draw so's we can force clipping on the fragments
	this->actorInstance->flags |= ACTORFLAG_CUSTOMDRAW;

	// PP: now that we've switched model into the exploding stunt crate, set up flying fragments...

	// PP: count fragments
	this->numFragments=this->countFragments();

	// PP: malloc fragments
	this->fragments=new WBTNTCRATEFRAGMENT[this->numFragments];

	// PP: initialise fragments
	this->initFragments();

	// PP: launch fragments
	this->launchFragments(yAng);

	// PP: flag this crate as exploded
	this->exploded=true;

	if(this->type == WBCRATETYPE_SUPER)
	{
		// PP: BIG BARRA BOOM!
		PlaySample("slowboom.wav", 255);

		wbsam.loseSuperCrate();

		WestBoss_game.explodingSuperCrate=this;
		WestBoss_game.setCamera(WBCAM_SUPERCRATEEXPLODE);
	}
	else
	{
		// PP: TEMP?
		PlaySample("crash_gen_02c.wav", 255);

		// PP: another normal crate crate has been destroyed
		WestBoss_game.numCratesDestroyed++;
	}

 	this->timer=0.0f;
}


/*	--------------------------------------------------------------------------------
	Function 	: WBTNTCRATE::update
	Purpose 	: update
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/

#define WBTNTCRATE_FRAGMENT_LIFE	5.0f

void WBTNTCRATE::update(void)
{
	if(this->finished)
	{
		if(this->actorInstance != NULL)
		{
			FreeActorInstance(this->actorInstance);
			this->shutdown();// PP: nulls this->actorInstance
		}

		return;
	}

	if(this->exploded)
	{
		this->updateFragments();
		this->timer+=fTime;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: WBTNTCRATE::shutdown
	Purpose 	: shutdown
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void WBTNTCRATE::shutdown(void)
{
	// PP: null actorinstance ptr
	ACTORINSTANCEUSER::shutdown();

	// PP: hmm, this used to crash when I used the normal SAFE_DELETE.  interesting
	SAFE_DELETE_ARRAY(this->fragments);
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													WESTBOSSGAME methods

						This is the BOSSGAME type for the Wile E West Boss - Yosemite Sam's ammo dump
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: name of taz's goal/checkpoint object instance for 2 player
#define WB_2PLAYER_GOAL_INSTANCE		"checkpoint"// PP: the lom says "checkpoint01" I dunno what's going on

// PP: NOTE: this is a Y-flipped version of what you see in the docs
const char WESTBOSSGAME::grid[WB_SQUARES_HIGH][WB_SQUARES_WIDE+1]=
{
"OOOOOOOOOOOOOOOOOOO", // PP: Z
"O  O O O O OOOOOOOO", // PP: |
"O    O   O    O   O", // PP: |
"O  O O O O  O   O O", // PP: |
"O  O O O O  O O O O", // PP: |
"O  O   O O OO O O O", // PP: |
"O  O O O O O  OO  O", // PP: |
"OOOOOOOO   OOOO  OO", // PP: |
"O       OOO       O", // PP: |
"OOO OOO OOO  O OO O", // PP: |
"OO      OOO       O", // PP: |
"OO O O O   OOOOOO O", // PP: |
"OOOOOO O   O      O", // PP: |
"O      O O O OOOOOO", // PP: |
"OOOO OOO   O      O", // PP: |
"O    O   O OOOOOO O", // PP: |
"OO  OO O        O O", // PP: |
"OOO    O  O OOO   O", // PP: |
"OOOOOOOOOOOOOOOOOOO"  // PP: V
};// PP: X------------------->



/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSGAME::setupTazOutroCam
	Purpose 	: setup taz for an outro shot
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void WESTBOSSGAME::setupTazOutroCam(void)
{
	// PP: position the camera on the nearest ground square to the one taz is at...

	uint32	camSquareX, camSquareY;
	
	WESTBOSSGAME::getAdjacentGroundSquare(wbtaz.gridX, wbtaz.gridY, &camSquareX, &camSquareY);

	TBVector2D	temp2D={camSquareX+0.5f, camSquareY+0.5f};
	WESTBOSSGAME::evaluateGridPos(temp2D, this->outroCamPos[0]);

	static float camheight=400;

	this->outroCamPos[0][Y]+=camheight;

	// PP: put taz onto the middle of his square
	temp2D[X]=wbtaz.gridX+0.5f;
	temp2D[Y]=wbtaz.gridY+0.5f;
	WESTBOSSGAME::evaluateGridPos(temp2D, wbtaz.pos());

	// PP: bring it closer to taz
	static float lerpdist=0.4f;

	// PP: TODO: VEC::lerp2

	// TP: PS2 compiler will not compile this line, I get no error message it just crashes the assembler, nice!
	//this->outroCamPos[0]=this->outroCamPos[0].lerp(wbtaz.pos(), lerpdist);
	bmVectorLerp(this->outroCamPos[0], this->outroCamPos[0], wbtaz.pos(), lerpdist);

	// PP: take taz out of spin
	SpinDown(wbtaz.player);

	// PP: suspend taz control
	gameStatus.controlSuspend = TRUE;

	// PP: turn Taz to face the camera
	wbtaz.YrotateToFace(this->outroCamPos[0]);

	// PP: hold still and take it you whore
	SetActorVelocity(wbtaz.actorInstance->actorBehaviour, ZVEC);
	
	// PP: do I need this?
	FlushAnimationQueue(wbtaz.actorInstance);

	if(this->twoPlayer)
	{
		if(this->winner == 0)// PP: taz won
		{	
			// PP: have Taz play a celebration anim
			wbtaz.playAnim("happytaz", true);
		}
		else// PP: taz lost
		{
			// PP: have Taz play a losing anim
			wbtaz.playAnim("unhappytaz", true);
		}
	}
	else
	{
		if(this->winner == 0)// PP: taz won
		{	
			// PP: have Taz play a celebration anim
			wbtaz.playAnim("happytaz");
		}
		else// PP: taz lost
		{
			// PP: have Taz play a losing anim
			wbtaz.playAnim("tazdie1");
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSGAME::setCamera
	Purpose 	: set the camera shot
	Parameters 	: shot to set
	Returns 	: 
	Info 		: 
*/												
#define WBCAM_SAM_INTRO_POS_OFFSET_X			0.0f
#define WBCAM_SAM_INTRO_POS_OFFSET_Y			METERTOUNIT(4.0f)//1
#define WBCAM_SAM_INTRO_POS_OFFSET_Z			METERTOUNIT(4.0f)//2

#define WBCAM_INTRODYNAMITE_POS_OFFSET_X		METERTOUNIT(1)
#define WBCAM_INTRODYNAMITE_POS_OFFSET_Y		METERTOUNIT(0)
#define WBCAM_INTRODYNAMITE_POS_OFFSET_Z		METERTOUNIT(0)
#define WBCAM_INTRODYNAMITE_FOCUS_BLUR			4

#define WBCAM_EXPLOSIONDIVE_FOCUS_BLUR			1
#define WBCAM_EXPLOSIONDIVE_FOCUS_LENGTH		METERTOUNIT(15)
#define WBCAM_EXPLOSIONDIVE_MAX_TILT			0.7f

#define WBCAM_INTRODYNAMITE_GAMESPEED			0.25f
#define WBCAM_INTRODYNAMITE_TIMEJUMP1			2.0f
#define WBCAM_INTRODYNAMITE_DURATION			1.5f
#define WBCAM_INTRODYNAMITE_TIMEJUMP2			0.0f//1.9f

#define WBCAM_SUPERCRATEEXPLODE_POS_OFFSET_X	0.0f
#define WBCAM_SUPERCRATEEXPLODE_POS_OFFSET_Y	METERTOUNIT(4.0f)
#define WBCAM_SUPERCRATEEXPLODE_POS_OFFSET_Z	METERTOUNIT(4.0f)

#if(BPLATFORM==PS2)
#define WBCAM_SUPERCRATEEXPLODE_MOTIONBLUR		0.6f
#else// PP: if(BPLATFORM==XBOX) else
#define WBCAM_SUPERCRATEEXPLODE_MOTIONBLUR		0.8f
#endif// PP: (BPLATFORM==XBOX) else

#define WBCAM_SUPERCRATEEXPLODE_DURATION1		0.3f
#define WBCAM_SUPERCRATEEXPLODE_DURATION2		0.1f
#define WBCAM_SUPERCRATEEXPLODE_DURATION3		0.3f
#define WBCAM_SUPERCRATEEXPLODE_GAMESPEED1		0.75f
#define WBCAM_SUPERCRATEEXPLODE_GAMESPEED2		0.06f
#define WBCAM_SUPERCRATEEXPLODE_GAMESPEED3		WBCAM_SUPERCRATEEXPLODE_GAMESPEED1
#define WBCAM_DURATION_SUPERCRATEEXPLODE		((WBCAM_SUPERCRATEEXPLODE_DURATION1*WBCAM_SUPERCRATEEXPLODE_GAMESPEED1) \
												+(WBCAM_SUPERCRATEEXPLODE_DURATION2*WBCAM_SUPERCRATEEXPLODE_GAMESPEED2) \
												+(WBCAM_SUPERCRATEEXPLODE_DURATION3*WBCAM_SUPERCRATEEXPLODE_GAMESPEED3))

#define WBCAM_PLAY_FOCUS_BLUR					9
#define WBCAM_PLAY_FOCUS_LENGTH					METERTOUNIT(1.5f)


// PP: TEMP TEST
static VEC		gcamPos=ZVEC;

void WESTBOSSGAME::setCamera(const EWestBossCamera cam)
{
	// PP: no lookat actor instance
	// PP: DisableCameraLookAtFollowActorInstance(&camera[0]);

	// PP: All these shots are tied-in with game speeds
	TimeFX_cancelRates();

	// PP: take off any tilt; only one shot uses it
	camera[0].tilt=0.0f;

	// PP: back to normal game speed
	TimeFX_setGlobalRate(WB_GAMESPEED);
#ifdef WBDOF
	// PP: put characters' cel shading back on if it's been taken off for defocusing purposes
	if(this->camShot == WBCAM_INTRODYNAMITE)
	{
		// TP: EnableActorCartoonRenderMode(&wbsam.actorInstance->actorInstance);
		// TP: only using line render mode on Sam now
		EnableActorLineRenderMode(&wbsam.actorInstance->actorInstance);
	}
#endif

	// PP: sam can talk again
	wbsam.unmute();

	// PP: turn focus effects off, must be done after them EnableActorCartoonRenderMode calls
	//VFX_focusReset();

	switch(cam)
	{
	case WBCAM_NONE:
		break;

	case WBCAM_PLAY:
		{
			this->movieMode=false;
			setFilmBorders(false);

			// PP: set the right number of screens - TODO: that colour-separated twoplayer thingy
			if(this->twoPlayer)
			{
				setNumScreens(2, WB_SPLITSCREEN_MODE);

				// PP: specify that this book will always be drawn full-screen
				this->scoreBook.setScreen(BKSCREEN_FULL);

				//SetCameraLookAtFollowActorInstance(&camera[1], wbtaz.actorInstance, &(wbtaz.actorInstance->actorInstance), (TBActorNodeInstance *)NULL);
			}

			// PP: new test: open score book
		//	WestBoss_game.scoreBook.open();

			if(this->cameraCase != NULL)
			{
				// PP: REMOUT: StartCameraCase(this->cameraCase, &camera[0]);
				StartCameraCase(this->cameraCase, &this->dummyCam);
			}

#ifdef WBDOF
			// PP: a wee bit of far-focus so's objects very close to the camera blur
			VFX_focusSetBlurFactor(WBCAM_PLAY_FOCUS_BLUR);
			VFX_focusFar(WBCAM_PLAY_FOCUS_LENGTH);
#endif
			// PP: open the score book - I don't want it doing crazy defocus shit in the intro shots y'see

			if(this->twoPlayer)
			{
				this->scoreBook.open("2PSCOREPAGE");
			}
			else
			{
				this->scoreBook.open("1PSCOREPAGE");
			}
		}
		break;

	case WBCAM_EXPLOSIONDIVE:
		{
			// PP: Action-Movie-Dive-From-Explosion-in-Slow-Motion™ cam

			// PP: don't want sam talking during explosiondive
			wbsam.mute();

			this->movieMode=true;
			setFilmBorders(true);

			// PP: switch to one screen
			setNumScreens(1);

			// PP: Manditory motion blur...

			// PP: stop camera case
			StopCameraCase(&this->dummyCam, this->cameraCase);

			// PP: some 'extreme' camera tilt (Deeeuuuwd.)
			camera[0].tilt=bmRandfSigned()*WBCAM_EXPLOSIONDIVE_MAX_TILT;

			// PP: look at Taz
			SetCameraLookAtFollowActorInstance(&camera[0], wbtaz.actorInstance, &(wbtaz.actorInstance->actorInstance), (TBActorNodeInstance *)NULL);

			// PP: slow the action down
			TimeFX_setGlobalRate(WBTAZ_EXPLOSIONDIVE_GAMESPEED);

#ifdef WBDOF
			// PP: near-focus
			VFX_focusSetBlurFactor(WBCAM_EXPLOSIONDIVE_FOCUS_BLUR);
			VFX_focusNear(WBCAM_EXPLOSIONDIVE_FOCUS_LENGTH);
#endif
			TBVector	lookatOffset={0.0f, TAZ_HEIGHT, 0.0f, 0.0f};
			SetCameraLookAtActorInstanceOffset(&camera[0], lookatOffset);
/*
			// PP: no fixed distance to lookat
			SetFixedDistanceToLookAt(&camera[0], 0.0f);

			// PP: no fixed elevation above target
			DisableCameraAngleAboveHorizontal(&camera[0]);
*/
			// PP: new test: close score book
			WestBoss_game.scoreBook.close(0.0f, true);
		}
		break;

	case WBCAM_SUPERCRATEEXPLODE:
		{
			this->movieMode=true;
			setFilmBorders(true);

			// PP: switch to one screen
			setNumScreens(1);

			// PP: point the camera at the super-crate exploding...

			// PP: stop camera case
			StopCameraCase(&this->dummyCam,this->cameraCase);

			SimplifyCamera(&this->dummyCam);
			SimplifyCamera(&camera[0]);

			// PP: I don't EVER want to see camera collision
//			SetCameraCollision(&camera[0], false);

			// PP: look at super crate

			// PP: set camera pos...

			TBVector	tempVec, camPos;
			SETVECTORPOS(tempVec, WBCAM_SUPERCRATEEXPLODE_POS_OFFSET);

			bmVectorRotateY(camPos, tempVec, this->explodingSuperCrate->getYang(wbsam)+PI);

			bmVectorAdd(camPos, camPos, this->explodingSuperCrate->pos());

			SetCameraPosition(&camera[0], camPos);

			SetCameraLookAtPosition(&camera[0], this->explodingSuperCrate->pos());

			// PP: init timer
			this->cameraTimer=0.0f;

			// PP: queue game speeds
			TimeFX_cancelRates();// PP: cancel any currently queued rates
			TimeFX_queueRate(WBCAM_SUPERCRATEEXPLODE_GAMESPEED1, WBCAM_SUPERCRATEEXPLODE_DURATION1);

// PP: DEMO FRIG
#if(BPLATFORM == XBOX)
			if(VFX_effectActive(VFX_SHADOWMAP))
			{
				TimeFX_queueRate(WBCAM_SUPERCRATEEXPLODE_GAMESPEED2, WBCAM_SUPERCRATEEXPLODE_DURATION2);
			}
			else
#endif// PP: platform
			TimeFX_queueRate(WBCAM_SUPERCRATEEXPLODE_GAMESPEED2, WBCAM_SUPERCRATEEXPLODE_DURATION2, WBCAM_SUPERCRATEEXPLODE_MOTIONBLUR);

			

			//TimeFX_queueTimeslice();
			TimeFX_queueRate(WBCAM_SUPERCRATEEXPLODE_GAMESPEED3, WBCAM_SUPERCRATEEXPLODE_DURATION3);
		}
		break;

	case WBCAM_2PLAYER_OUTRO:
		{
			this->setupTazOutroCam();

			// PP: position sam's cam
			
			// TP: PS2 compiler will not compile this line, I get no error message it just crashes the assembler, nice!
			// PP: Oh wonderful
			//this->outroCamPos[1]=wbsam.pos()+(VECM(0,0,1.5f).rotateY(PI-wbsam.getYang())).moveY(200);
			VEC tempVec = VECM(0,0,1.5f);
			tempVec.rotateY2(PI-wbsam.getYang());
			tempVec[Y]+=200;
			tempVec =wbsam.pos() + tempVec;
			bmVectorCopy(&outroCamPos[1][0],tempVec);

			// PP: sam play anim
			if(this->winner == 1)// PP: sam win
			{
				wbsam.playAnim("injuretaz1", true);
			}
			else// PP: sam lose
			{
				wbsam.playAnim("destroysupercrate1", true);
			}
		}
		break;

	case WBCAM_TAZLOSE:
		{
			TimeFX_setGlobalRate(0.5f);
		}// PP: no break
	case WBCAM_TAZWIN:
		{
			this->setupTazOutroCam();

#ifdef PHIL// PP: TEMP TEST
			bkPrintf("STARTING OUTRO ANIM\n");
#endif// PP: def PHIL
		}
		break;
	}

	this->camShot=cam;
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSGAME::updateCamera
	Purpose 	: update the West boss game's camera or cameras
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides BOSSGAME::updateCamera
					// PP: NOTE: In stereoscopic mode, you'll see a lag between the two eyes on Westboss.
*/

#define WB_CAM_ANGLE_SLACK				0.35f			

void WESTBOSSGAME::updateCamera(void)
{
	if (this->camShot!=WBCAM_NONE) camera[0].fade.SetFadeStatus(FADESTATE_OFF);

	switch(this->camShot)
	{
	case WBCAM_NONE:
		break;

	case WBCAM_PLAY:
		{
			// PP: KINDA IMPORTANT!!
			UpdateCamera(&this->dummyCam);

			// PP: update player 1 camera to swing round Sam with Taz~~~~~~~~~~~~~~~~~~~~~~~~~~~

			// PP: get y angle between Taz & Sam...

			float		yAng;

			yAng= -wbtaz.getYang(wbsam);

			// PP: get the difference between that angle and the kos cam's angle
			float		angDiff;
			angDiff=FindShortestAngleSigned(this->cameraCase->KOScamData->angle[Y], yAng);

			// PP: if the difference is greater than the max allowed difference swing the camera
			if(bmFAbs(angDiff) > WB_CAM_ANGLE_SLACK)
			{
				this->cameraCase->KOScamData->angle[Y]=yAng+(angDiff*(WB_CAM_ANGLE_SLACK/bmFAbs(angDiff)));
			}

			// PP: ==================================

			// PP: TEMP TEST!!!!!
			// PP: this->cameraCase->camera->pos[Y]=bmRandf()*METERTOUNIT(50);

			float	distAcrossGrid;
			float	temp;
			
#define BLEH_HEIGHT		(WB_HEIGHT*0.5f)
#define BLEH_WIDTH		(WB_WIDTH*0.5f)

			distAcrossGrid=bmFAbs(wbtaz.pos()[X]/BLEH_WIDTH);

			temp=bmFAbs(wbtaz.pos()[Z]/BLEH_HEIGHT);

			if(temp>distAcrossGrid)
			{
				distAcrossGrid=temp;
			}

			if(distAcrossGrid>1.0f)
			{
				distAcrossGrid=1.0f;
			}

			// PP: TEMP TEST
			distAcrossGrid=distAcrossGrid*distAcrossGrid*distAcrossGrid;

			SetCameraPosition(&camera[0], VEC(this->cameraCase->camera->pos).lerp(wbtaz.pos()+(VECM(0, 7.5f, 2).rotateY(PI-this->cameraCase->KOScamData->angle[Y])), distAcrossGrid));
			SetCameraLookAtPosition(&camera[0], VEC(this->cameraCase->camera->lookAt).lerp(wbtaz.pos(), distAcrossGrid));

		/*	{
				camera[0].flags=this->dummyCam.flags;

			}*/

			//memcpy(&camera[0], &this->dummyCam, sizeof(CAMERAINFO));

			// PP: ==================================

			// PP: update player 2 camera ~~~~~~~~~~~~~~~~~~~~~~~~~~~

			if(this->twoPlayer)
			{
				// PP: 
				TBVector	camPos;
				TBVector	tempVec;

				SETVECTOR(tempVec, wbsam.pos()[X], wbsam.pos()[Y]+METERTOUNIT(8), wbsam.pos()[Z]-METERTOUNIT(2.5f), 0.0f);

				bmVectorRotateY(camPos, tempVec, PI-wbsam.getYang());

				SetCameraPosition(&camera[1], camPos);

				TBVector	lookat;

				SETVECTOR(tempVec, wbsam.pos()[X], wbsam.pos()[Y], wbsam.pos()[Z]+METERTOUNIT(4.5f), 0.0f);

				bmVectorRotateY(lookat, tempVec, PI-wbsam.getYang());

				SetCameraLookAtPosition(&camera[1], lookat);
			}
		}
		break;

	case WBCAM_EXPLOSIONDIVE:
		{
			// PP: TEMP?: moblo leaves a mess in 2-player D'OH!  (It never used to happen.)  Probably don't have time to fix it
			if(!this->twoPlayer)
			{
				// PP: TODO: should really change TimeFX_setGlobalRate to take a motion blur param
				VFX_motionBlur(WBTAZ_EXPLOSIONDIVE_MOTIONBLUR);
			}

			// PP: tst
			SetCameraPosition(&camera[0], wbtaz.explosionDive.camPos);// PP: NEW MILESTONE BODGE
			SetCameraLookAtPosition(&camera[0], camera[0].destinationLookAt);
		}
		break;

	case WBCAM_SUPERCRATEEXPLODE:
		{
			// PP: increment timer for this shot
			this->cameraTimer+=fTime;

			if(TimeFX_noQueue)
			{
				if(wbsam.score->getScore()<1)
				{
					// PP: milestone bodgery
					WestBoss_game.winner=0;
					WestBoss_game.loser=1;
				
					WestBoss_game.setState(BGSTATE_OUTRO);// PP: game over
				}
				else
				{
					// PP: end the shot
					this->setCamera(WBCAM_PLAY);
					break;
				}
			}
		}
		break;

		// PP: y offset of lookat point for taz's outro cam
#define OUTRO_CAM_LOOKAT_Y_OFFSET_TAZ		200.0f

		// PP: y offset of lookat point for sam's outro cam
#define OUTRO_CAM_LOOKAT_Y_OFFSET_SAM		200.0f

	case WBCAM_2PLAYER_OUTRO:
		{
			// PP: tazzy cam
			SetCameraPosition(&camera[0], this->outroCamPos[0]);
			SetCameraLookAtPosition(&camera[0], VEC(wbtaz.pos()).moveY(OUTRO_CAM_LOOKAT_Y_OFFSET_TAZ));

			// PP: sammy cam
			SetCameraPosition(&camera[1], this->outroCamPos[1]);
			SetCameraLookAtPosition(&camera[1], VEC(wbsam.pos()).moveY(OUTRO_CAM_LOOKAT_Y_OFFSET_SAM));
		}
		break;
	case WBCAM_TAZLOSE:
		{
			VFX_motionBlur(0.3f);
		}// PP: no break
	case WBCAM_TAZWIN:
		{
			SetCameraPosition(&camera[0], this->outroCamPos[0]);
			SetCameraLookAtPosition(&camera[0], VEC(wbtaz.pos()).moveY(OUTRO_CAM_LOOKAT_Y_OFFSET_TAZ));

			// PP: check for end of anim
			if(wbtaz.animChanged)
			{
				TimeFX_setGlobalRate(1.0f);

#ifdef WB_SPINOFF
				if((this->camShot == WBCAM_TAZWIN)// PP: could be WBCAM_TAZLOSE
	#if(BPLATFORM == PC)
					&&(!VOODOO)// PP: no ORTs, hence no spinoffs, in PC compatibility mode
	#endif// PP: pc
					&&(videoMode.numDraws == 1))// PP: no spinoff in stereoscopic mode; don't have time to get it working
				{		
					// PP: prepare a spinoff effect to start on next frame
					VFX_spinoffPrepare();
				}
				else
#endif// PP: def WB_SPINOFF
				{
					this->setState(BGSTATE_FINISHED);
				}
			}
			else
			{
#ifdef WB_SPINOFF
				if(	(videoMode.numDraws==1)// PP: no spinoff in stereoscopic mode; don't have time to get it working
	#if(BPLATFORM == PC)
					&&(!VOODOO)// PP: no ORTs, hence no spinoffs, in PC compatibility mode
	#endif// PP: pc
					&&VFX_spinoffNewlyPrepared)
#endif// PP: def WB_SPINOFF	
				{
					VFX_spinoffNewlyPrepared=false;

					// PP: spinoff frame has been written to the VFX_renderTarget, so we're ready now to change camera shot
					this->setState(BGSTATE_FINISHED);

#ifdef WB_SPINOFF

					if(	(videoMode.numDraws==1)
	#if(BPLATFORM == PC)
						&&(!VOODOO)// PP: no ORTs, hence no spinoffs, in PC compatibility mode
	#endif// PP: pc
						)
					{
						// PP: VFX_spinoffStart has to be called from outside VideoFX because there might be a time when
						// PP:		you want to spin off a frame that was prepared some time ago
						VFX_spinoffStart();
					}

#endif// PP: def WB_SPINOFF	

					// PP: VFX_spinoffStarted is now true
				}
			}
		}
		break;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSGAME::setState
	Purpose 	: switch the game into a new state
	Parameters 	: the state into which to switch
	Returns 	: 
	Info 		: this overrides BOSSGAME::setState
*/
void WESTBOSSGAME::setState(const int newState)
{
	switch(newState)
	{
	case BGSTATE_PLAY:
		{
			this->setCamera(WBCAM_PLAY);
		}
		break;

	case BGSTATE_OUTRO:
		{
			this->movieMode=false;
			setFilmBorders(false);

			this->instructionBook.close();
			this->scoreBook.close();

			// PP: remove all crates (they might get in the way)
			this->removeCrates();

			// PP: remove all dynamite
			this->removeDynamite();

			// PP: leave explosions and smoke though, cos they're cool

			if(this->twoPlayer)
			{
				// PP: update winner number
				this->messageBook.getPage("2P RESULT")->getTextbox(STR_MP_WINNER)->setText(STR_MP_WINNER, this->winner+1);

				// PP: open two-player result screen
				this->messageBook.open("2P RESULT");

				this->setCamera(WBCAM_2PLAYER_OUTRO);
			}
			else
			{
				if(this->winner==0)// PP: PLAYER 1, TAZ
				{
					// PP: open "YOU WIN" message
					this->messageBook.open("WIN");

					this->setCamera(WBCAM_TAZWIN);
				}
				else
				{
					// PP: open "YOU LOSE" message
					this->messageBook.open("LOSE");

					this->setCamera(WBCAM_TAZLOSE);
				}
			}
		}
		break;

	case BGSTATE_FINISHED:
		{
			// PP: turn off film borders
			setFilmBorders(false);

			// PP: restore normal time rate
			TimeFX_setGlobalRate(1.0f);

			// PP: close books
			this->instructionBook.close();
			this->messageBook.close();
			this->scoreBook.close();

			if(this->twoPlayer)
			{
				bkGenerateEvent("scenechange","_frontend");	
			}
			else
			{
				if(this->winner == 0)
				{
					// PP: REMOUT:	MS	this->messageBook.open("WIN");
					LevelCompleted(gameStatus.player1,LEVELCOMPLETE_NOSTATS);
				}
				else
				{
					// PP: REMOUT:	MS	this->messageBook.open("LOSE");
					// TP: bkGenerateEvent("scenechange","_westhub");
					setCamera(WBCAM_NONE);
					StopCameraCase(&this->dummyCam,this->cameraCase, false);
					gameStatus.chrisCam=true;
					ProcessTazDeathAtEndOfBossGame(WESTBOSSGAME::resetCallback, NULL);
				}
			}
		}
		break;
	}

	BOSSGAME::setState(newState);
}


/* --------------------------------------------------------------------------------
   Function		: WESTBOSSGAME::resetCallback
   Purpose		: function called when the player selects the option to continue or to quit
   Parameters	: context value passed through from ProcessTazDeathAtEndOfBossGame,
					the player's choice of action (BOSSGAMERESET_CONTINUE, BOSSGAMERESET_QUIT)
   Returns		: big boobs
   Info			:
*/
int32 WESTBOSSGAME::resetCallback(void *context, EBossGameResetChoice choice)
{
	switch(choice)
	{
	case BOSSGAMERESET_CONTINUE:
		bkGenerateEvent("scenechange", "_reset");
		break;
	case BOSSGAMERESET_QUIT:
		// PP: // CMD: all we need to do is reset back to the frontend
		bkGenerateEvent("scenechange", "_westhub");
		break;
	}

	// PP: must return a value
	return 0xB16B00B5;
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSGAME::respondToMessage
	Purpose 	: handle a message
	Parameters 	: pointer to actor instance, pointer to message string
	Returns 	: 
	Info 		: 
*/
void WESTBOSSGAME::respondToMessage(const ACTORINSTANCE* const actorInstance, const char* const message)
{
	if(actorInstance == wbsam.actorInstance)
	{
		wbsam.respondToMessage(message);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSGAME::initScoreBook
	Purpose 	: initialise the book for overlays such as score readouts
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides BOSSGAME::initScoreBook
*/
void WESTBOSSGAME::initScoreBook(void)
{
	PAGE*			page;
	ICON*			icon;
	BRICKDISPLAY*	brickDisplay;	
//	TEXTBOX*		textBox;

	BOSSGAME::initScoreBook();

	// PP: set the rectangle for the score book (whole screen, but only the top is used)
	// PP: REMOUT: books now default to filling the full 'safe' area of the screen		this->scoreBook.setBookRect(RECTANGLE(-0.5f, 0.5f, -0.5f, 0.5f));// PP: REMOUT: must now be expressed as fractions of the screen dimensions		RECTANGLE(-310, 310, -220, 220));

	// PP: specify screen index 2, meaning this book will always be drawn full-screen
//	this->scoreBook.setRect(RECTANGLE(-0.5f, 0.5f, -0.5f, 0.5f), 2);

	// PP: ~~~~~~ 1-PLAYER PAGE

	// PP: Taz's icon
	page=this->scoreBook.addPage("1PSCOREPAGE");
	page->insertItem(icon=new ICON("extras\\tazatlas.bmp", "TAZICON"));
	icon->setXPadding(false);
	icon->tilt=false;
	icon->setYAlign(PIFLAG_YALIGN_TOP);

#ifdef WB_TAZ_SCORE_DISPLAY// PP: WestBoss.cpp

	// PP: todo: Taz's score display
	// PP: TEMP: textbox to print taz's health value into
	page->insertItem(textBox=new TEXTBOX(), false);
	textBox->setXAlign(PIFLAG_XALIGN_LEFT);
	textBox->setYAlign(PIFLAG_YALIGN_TOP);
	textBox->setColour(COLOUR(0,255,0), COLOUR(0,40,0));
	textBox->setName("TAZSCORE");
	textBox->clear();

#endif// PP: def WB_TAZ_SCORE_DISPLAY

/*
	page->insertItem(this->taz.healthBar=new HEALTHBAR(int(this->taz.actorInstance->characterInfo->health.maxHealth), "HEALTHBAR"), false)
		->setXAlign(PIFLAG_XALIGN_LEFT)
		->setYAlign(PIFLAG_YALIGN_TOP);
*/
	// PP: debug text box
/*	page->insertItem(WestBoss_debugBox=new TEXTBOX((EStringIndex)(-1), COLOUR(255, 0, 0), COLOUR(0, 0, 255), TBFLAG_DEFAULTS), false);
	WestBoss_debugBox->setDrawScale(0.6f);
	WestBoss_debugBox->setAlignScale(0.0f);
	WestBoss_debugBox->setYAlign(PIFLAG_YALIGN_TOP);
	WestBoss_debugBox->setWrap(false);
	WestBoss_debugBox->clear();*/

	// PP: Sam's's score display...

	brickDisplay=new BRICKDISPLAY("SAMSCORE");

	page->insertItem(brickDisplay, false);

	brickDisplay->setXAlign(PIFLAG_XALIGN_RIGHT);

/*	if(gameStatus.multiplayer.numScreens == 1)
	{
		brickDisplay->setXPadding(false);
	}
	else*/
	{
		brickDisplay->setXPadding(true);
	}

	brickDisplay->setYAlign(PIFLAG_YALIGN_TOP);

	this->sam.score=brickDisplay;

	// PP: Sam's icon
	page->insertItem(icon=new ICON("extras\\sambomberman.bmp", "SAMICON"), false);
	icon->setXAlign(PIFLAG_XALIGN_RIGHT);
	icon->setXPadding(false);
	icon->tilt=false;
	icon->setYAlign(PIFLAG_YALIGN_TOP);

	//page->setScale(ZooBoss_bookScale);

//	WestBoss_debugBox->clear();

	// PP: ~~~~~~ 2-PLAYER PAGE

	page=this->scoreBook.addPage("2PSCOREPAGE");

	// PP: left & right sub-pages...

	PAGE*		subPage;

	// PP: TAZ'S PAGE...

	page->insertFreeItem(subPage=new PAGE("TAZPAGE"), UORECT.leftHalf());

	{
#ifdef WB_2P_CHARACTER_ICONS// PP: WestBoss.cpp

		// PP: Taz's icon
		subPage->insertItem(icon=new ICON("extras\\tazatlas.bmp", "TAZICON"));
		icon->setXAlign(PIFLAG_XALIGN_LEFT);
		icon->setYAlign(PIFLAG_YALIGN_TOP);
		icon->setXPadding(false);
		icon->tilt=false;

#endif// PP: def WB_2P_CHARACTER_ICONS

#ifdef WB_TAZ_SCORE_DISPLAY// PP: WestBoss.cpp

		// PP: todo: Taz's score display
		// PP: TEMP: textbox to print taz's health value into
		subPage->insertItem(textBox=new TEXTBOX(), false);
		textBox->setXAlign(PIFLAG_XALIGN_LEFT);
		textBox->setYAlign(PIFLAG_YALIGN_TOP);
		textBox->setColour(COLOUR(0,255,0), COLOUR(0,40,0));
		textBox->setName("TAZSCORE2");
		textBox->clear();
		/* PP: REMOUT: OOD:		page->insertItem(this->taz.healthBar=new HEALTHBAR(int(this->taz.actorInstance->characterInfo->health.maxHealth), "HEALTHBAR2"), false)
		->setXAlign(PIFLAG_XALIGN_LEFT)
		->setYAlign(PIFLAG_YALIGN_TOP);*/

#endif// PP: def WB_TAZ_SCORE_DISPLAY

	}

	// PP: SAM'S PAGE
	page->insertFreeItem(subPage=new PAGE("SAMPAGE"), UORECT.rightHalf());

	{
		// PP: Sam's golf bar
		subPage->insertItem(this->sam.golfBar=new GOLFBAR(&controller2, WBSAM::humanThrowCallback, long32(&wbsam), "GOLFBAR"))
#ifdef WB_2P_CHARACTER_ICONS// PP: WestBoss.cpp
			->setYAlign(PIFLAG_YALIGN_TOP)// PP: to avoid popup
			->setXAlign(PIFLAG_XALIGN_LEFT);
#else// PP: ifndef WB_2P_CHARACTER_ICONS
			// PP: to avoid popup
			->setYAlign(PIFLAG_YALIGN_BOTTOM)
			->setXAlign(PIFLAG_XALIGN_RIGHT);
#endif// PP: ndef WB_2P_CHARACTER_ICONS

#ifdef WB_2P_CHARACTER_ICONS// PP: WestBoss.cpp

		// PP: Sam's icon
		subPage->insertItem(icon=new ICON("extras\\sambomberman.bmp", "SAMICON"), false);
		icon->setXAlign(PIFLAG_XALIGN_RIGHT);
		icon->setYAlign(PIFLAG_YALIGN_TOP);
		icon->tilt=false;

#endif// PP: def WB_2P_CHARACTER_ICONS
	}

	// PP: we want uniform X spacing on the last row added (the only row, the row containing the 2 sub-pages, remember?)
//	page->getLastRow()->flags |= ROWFLAG_UNIFORM_X_SPACING;
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSGAME::initMessageBook
	Purpose 	: initialise the book for messages such as "GET READY..." or "GOAL!"
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides BOSSGAME::initMessageBook
*/
#define WB_MESSAGE_FONT_SIZE			3.0f

void WESTBOSSGAME::initMessageBook(void)
{
	PAGE*			page;

	BOSSGAME::initMessageBook();

	// PP: 1-player...

	// PP: win page
	page=this->messageBook.addPage("WIN");
	page->insertItem(STR_ZOOBOSS_WIN)
		->setFontSize(WB_MESSAGE_FONT_SIZE)
		->setYAlign(PIFLAG_YALIGN_BOTTOM);

	// PP: lose page
	page=this->messageBook.addPage("LOSE");
	page->insertItem(STR_ZOOBOSS_LOSE)
		->setFontSize(WB_MESSAGE_FONT_SIZE)
		->setYAlign(PIFLAG_YALIGN_BOTTOM);

	// PP: 2-player...

	page=this->messageBook.addPage("2P RESULT");
	page->insertItem(STR_MP_WINNER)
		->setFontSize(2.75f)
		->setWrap(true)
		->setYAlign(PIFLAG_YALIGN_TOP);

	// PP: for consistency, use the pie chooser used in the races and bonus games
	this->messageBook.setEffectChooser(&racePIEChooser);
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSGAME::initInstructionBook
	Purpose 	: initialise the book for instructions such as "press X to start"
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides BOSSGAME::initInstructionBook
*/
void WESTBOSSGAME::initInstructionBook(void)
{
	BOSSGAME::initInstructionBook();// PP: OOD?
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSGAME::draw
	Purpose 	: draw the west boss game solid stuff
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides BOSSGAME::draw
*/
void WESTBOSSGAME::draw(void)
{
	int		i;		// PP: loop counter

	if(!this->initialised)
	{
		return;
	}

	// PP: do base class solid draw
	BOSSGAME::draw();

	bdSetIdentityObjectMatrix();

	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);

	for(i=0; i<WB_NUM_EXPLOSIVE_CRATES; i++)
	{
		if(wbcrate[i].finished) continue;
		if(wbcrate[i].notUsed) continue;// PP: NEW

		if(wbcrate[i].exploded)
		{
			wbcrate[i].drawFragments();
		}
	}

	for(i=0; i<WB_NUM_SUPER_TNT_CRATES; i++)
	{
		if(wbstnt[i].finished) continue;
		if(wbstnt[i].notUsed) continue;// PP: NEW

		if(wbstnt[i].exploded)
		{
			wbstnt[i].drawFragments();
		}
	}

	this->taz.draw();

	// PP: TEMP TEST
	//utilDrawSphere(gcamPos);

	// PP: temp test
//	utilDrawSphere(this->taz.explosionDive.camPos);
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSGAME::initCrates
	Purpose 	: initialise all tnt and super-tnt crates in the level
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void WESTBOSSGAME::initCrates(void)
{
	int				i;						// PP: loop counter
	char			crateName[256]={""};	// PP: crate name
	ACTORINSTANCE*	crateInstance;			// PP: ptr to crate actorinstance

	for(i=0; i<WB_MAX_NUM_EXPLOSIVE_CRATES; i++)
	{
		sprintf(crateName, "xbox_inst%02d", i+1);

		this->crates[i].init(crateName, WBCRATETYPE_NORMAL);
	}

	for(i=0; i<WB_MAX_NUM_SUPER_TNT_CRATES; i++)
	{
		sprintf(crateName, "supertntcrate%02d", i+1);

		this->superTNT[i].init(crateName, WBCRATETYPE_SUPER);
	}
}



/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSGAME::removeDynamite
	Purpose 	: remove all dynamite sticks from the level
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void WESTBOSSGAME::removeDynamite(void)
{
	LA_FOREACHDO(this->dynamiteList, actorInstance->flags |= (ACTORFLAG_DONTDRAW|ACTORFLAG_NOCOLLISION));
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSGAME::removeCrates
	Purpose 	: remove all tnt and super-tnt crates from the level
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void WESTBOSSGAME::removeCrates(void)
{
	int				i;						// PP: loop counter
	char			crateName[256]={""};	// PP: crate name
	ACTORINSTANCE*	crateInstance;			// PP: ptr to crate actorinstance

	// PP: INITIALISE EXPLOSIVE CRATES AND SUPER-TNT CRATES...

	for(i=0; i<WB_MAX_NUM_EXPLOSIVE_CRATES; i++)
	{
		sprintf(crateName, "xbox_inst%02d", i+1);

		// PP: remove the crate
		crateInstance=FindActorInstanceInMap(crateName, 0, &map);

		if(crateInstance != NULL)
		{
			FreeActorInstance(crateInstance);
		}

		this->crates[i].notUsed=true;
	}

	for(i=0; i<WB_MAX_NUM_SUPER_TNT_CRATES; i++)
	{
		sprintf(crateName, "supertntcrate%02d", i+1);

		// PP: remove the crate

		crateInstance=FindActorInstanceInMap(crateName, 0, &map);

		if(crateInstance != NULL)
		{
			FreeActorInstance(crateInstance);
		}

		this->superTNT[i].notUsed=true;
	}

	if(this->twoPlayer)
	{
		// PP: remove invisible blockers for crates...

		FreeActorInstances("objects\\destructible_block_box.obe");
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSGAME::draw2
	Purpose 	: draw the west boss game alpha stuff
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides BOSSGAME::draw2
*/
void WESTBOSSGAME::draw2(void)
{
	if(!this->initialised)
	{
		return;
	}

	BOSSGAME::draw2();

	if(this->state != BGSTATE_PLAY)
	{
		return;
	}

	// PP: draw books and break out if this is not a player-specific screen
	if(gameStatus.multiplayer.currentScreen == -1)
	{
		return;
	}

	// PP: WHY?: if I don't keep setting the clip planes they go wacko
// PP: REMOUT: TEST			bdSetViewClipPlanes(videoMode.nearPlane, videoMode.farPlane);

	// PP: draw dynamite...
	LA_FOREACHDO(this->dynamiteList, draw());

	// PP: draw bomber-man explosions
	WestBossExp_draw();

	/* PP: REMOUT: got checkpoint object now

	if(this->twoPlayer)
	{
		//SETVECTOR(exitPos, -1504, 461, 16, 0);

		//util3DText("EXIT", exitPos.v);

		utilDrawSphere(exitPos.v, METERTOUNIT(0.75f), GREEN);
	}*/

	// PP: draw effects for sam
	this->sam.draw();


/*	BOSSGAME::draw2();

	// PP: draw books and break out if this is not a player-specific screen
	if(gameStatus.multiplayer.currentScreen == -1)
	{
		return;
	}

	// PP: draw dynamite...
	LA_FOREACHDO(this->dynamiteList, draw());

	// PP: draw bomber-man explosions
	BallExp_draw();*/

	// PP: temp: draw taz's explosion dive camera pos
//	utilDrawCircle(wbtaz.explosionDiveCamPos);
/*
	// PP: TEMP - display grid squares...

	TBMatrix	transMat;

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);

	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);

	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	for(int i=0; i<WB_SQUARES_HIGH; i++)
	{
		for(int j=0; j<WB_SQUARES_WIDE; j++)
		{
			ushort		string16[3];
			char		string8[3];

			sprintf(string8, "%c", this->grid[i][j]);
#if 0
			bkString8to16(string16, string8);

			bmMatTranslate(transMat, this->gridPoints[i][j][X], this->gridPoints[i][j][Y], this->gridPoints[i][j][Z]);

			bdSetObjectMatrix(transMat);

			bdPrintFontLite(standardFont, string16, 3, 0.0f, 0.0f, 128, 128, 128, 128, NULL, NULL);
#endif
			TBVector	pos;
			SETVECTOR(pos, this->gridPoints[i][j][X], this->gridPoints[i][j][Y], this->gridPoints[i][j][Z], 1.0f);

			RECTANGLE	rect;

			rect.init(pos[X]-(WB_SQUARE_WIDTH*0.5f), pos[X]+(WB_SQUARE_WIDTH*0.5f), pos[Z]-(WB_SQUARE_HEIGHT*0.5f), pos[Z]+(WB_SQUARE_HEIGHT*0.5f));
			//rect=rect.removeMargin(WB_SQUARE_WIDTH*0.05f);

			COLOUR		col;

			if((i==wbtaz.gridY) && (j==wbtaz.gridX))
			{
				col=COLOUR(0,255,255,100);
			}
			else
			{
				if(this->grid[i][j]==' ')
				{
					col=COLOUR(0,255,0,80);
				}
				else if(this->grid[i][j]=='O')
				{
					col=COLOUR(255,0,0,80);
				}
				else if(this->grid[i][j]=='H')
				{
					col=COLOUR(255,255,0,80);
				}
			}

			rect.draw(col, RECTANGLE_DEFAULT_DRAW_FLAGS, X, Z, pos[Y]);
		}
	}
*/
	// PP: temp: draw taz's explosiondive cam pos
//	utilDrawSphere(wbtaz.explosionDive.camPos, METERTOUNIT(1), COLOUR(255, 0, 0));
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSGAME::preInit
	Purpose 	: initialisation to be done before the scene is loaded
	Parameters 	: 
	Returns 	: 
	Info 		: puts the display into 32 bit
*/
void WESTBOSSGAME::preInit(void)
{
	// PP: test - no DOF in 2 player
	//if(gameStatus.multiplayer.numPlayers==1)
	{
#ifdef WBDOF
		// PP: Prepare for focus effects - putting the display into 32 bit will improve motion blur too
		VFX_focusPrepare();
#endif
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSGAME::init
	Purpose 	: initialise the West Boss game
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides BOSSGAME::init
*/

#define WB_HEIGHTOFPOINT_CEILING	400.0f		// PP: y position from which downward height-of-point tests are performed on the map

void WESTBOSSGAME::init(void)
{
	int		i, j;			// PP: loop counters

	// PP: TEMP TEST
	g_WBstartedOutroAnim=false;

	// PP: MILESTONE FRIG
	DeactivateCheat(CHEAT_NEARFOCUS);
	DeactivateCheat(CHEAT_FARFOCUS);
	DeactivateCheat(CHEAT_DEFOCUS);

	// PP: camera case not set up yet so null it
	this->cameraCase=NULL;

	// PP: SET UP TAZ (player 1)
	this->taz.init();

	// PP: base-class initialisation
	BOSSGAME::init();

	// PP: SET UP SAM (cpu/player 2)
	this->sam.init();	

#ifdef VFX_MOTIONBLUR_USE_RENDER_TARGET
	// PP: render target will be used a lot, so make it permanent to prevent fragmentation (more reliable solution might be needed in the future)
	VFX_bookPermanentRenderTarget();
#endif // PP: def VFX_MOTIONBLUR_USE_RENDER_TARGET

	// PP: set the clip planes back to where they should be...hmmm
// PP: REMOUT: test		bdSetViewClipPlanes(videoMode.nearPlane, videoMode.farPlane);

	// PP: initialise west boss explosions
	WestBossExp_init();

	// PP: chris cam off
	gameStatus.chrisCam=false;

	// PP: SET UP SOUND
	SetHearingRange(WB_HEARING_RANGE);

	// PP: lots of fire and smoke, and it has to look really cool, so lets get the draw order sorted before each draw
	Fireball_applyDrawOrder=true;

	// PP: SET UP SKATEBOMB LIST
	LA_FORARRAYDO(this->skateBombList, init());

	// PP: FIND HEIGHT OF EACH GRID SQUARE...
	for(i=0; i<WB_SQUARES_HIGH; i++)// PP: for each row
	{
		for(int j=0; j<WB_SQUARES_WIDE; j++)// PP: for each column
		{
			this->gridPoints[i][j][X]=WB_GRID_X_OFFSET+((float(j)-(WB_SQUARES_WIDE/2.0f)+0.5f)*float(WB_SQUARE_WIDTH));
			this->gridPoints[i][j][Z]=WB_GRID_Z_OFFSET+((float(i)-(WB_SQUARES_HIGH/2.0f)+0.5f)*float(WB_SQUARE_HEIGHT));

			this->gridPoints[i][j][Y]=WB_HEIGHTOFPOINT_CEILING;

			float	yValue;

			ProcessHeightOfPoint(&collisionCache, this->gridPoints[i][j], &yValue, PHOP_EXCLUDEOBJECTS);

			this->gridPoints[i][j][Y]=yValue;

			//bkPrintf("%d,%d: x %5.2f, y %5.2f, z %5.2f\n", i, j, this->gridPoints[i][j][X], this->gridPoints[i][j][Y], this->gridPoints[i][j][Z]);
		}
	}

	// PP: Load dynamite actor
	LoadActor(WBDYNAMITE_MODEL, map.mapName);

	// PP: SET UP DYNAMITE LIST
	LA_FORARRAYDO(this->dynamiteList, init());

	if(this->twoPlayer)
	{
		this->removeCrates();
	}
	else
	{
		// PP: remove the twoplayer checkpoint goal thingy
		ACTORINSTANCE*	goalInstance;

		goalInstance=FindActorInstanceInMap(WB_2PLAYER_GOAL_INSTANCE, NULL, &map);

		if(goalInstance == NULL)
		{
			bkPrintf("WESTBOSSGAME::init: DIDN'T FIND GOAL INSTANCE\n");
		}
		else
		{
			goalInstance->flags |= (ACTORFLAG_DONTDRAW|ACTORFLAG_NOCOLLISION);
		}

		this->initCrates();

		// PP: NEW: plant the dynamite from the cutscene at taz's feet
		this->dynamiteList.getNew()->plant(this->taz.gridX, this->taz.gridY);// PP: TODO: LINKEDLIST::get
	}

	// PP: OOD completely
	// PP: FIND AND INITIALISE DEPUTIES...
	
	int		numDeputies=0;

	for(i=0; i<WB_SQUARES_HIGH; i++)
	{
		for(j=0; j<WB_SQUARES_WIDE; j++)
		{
			if(this->grid[i][j] == WBGRID_DEPUTY)
			{
				this->deputies[numDeputies].init(j, i);
				numDeputies++;
			}
		}
	}

	// PP: TEMP: check number of deputies is ok
	if(numDeputies != WB_NUM_DEPUTIES)
	{
		bkPrintf("WESTBOSSGAME::init: found %d deputies in grid - should have found %d!!!\n", numDeputies, WB_NUM_DEPUTIES);
	}

	// PP: SET UP KEEP-ON-SCREEN CAMERA
	
	TBVector	cameraAngle;
	SETVECTOR(cameraAngle, 5.89f, 5.70f, 0.0f, 1.0f);

	this->cameraCase = CreateCameraCase("WestBoss cameraCase", CAMERACASETYPE_KEEPONSCREEN);	// PP: create the camera case
	this->cameraCase->flags &= ~CAMERACASE_TRANSITIONCONTROL;
	CameraCaseKeepOnScreen(this->cameraCase, cameraAngle);
	this->cameraCase->KOScamData->addTarget(&(this->taz.actorInstance->actorInstance), 0.0f, METERTOUNIT(1.25f));
	this->cameraCase->KOScamData->addTarget(&(this->sam.actorInstance->actorInstance), METERTOUNIT(0.6f), 0.0f);

	this->collisionMesh=FindActorInstanceInMap("coll10", 0, &map);

	// PP: turn off camera collision
	SetCameraCollision(&camera[0], false);

	// PP: Start off in the 'playing' state
	this->setState(BGSTATE_PLAY);

	// TP: Set default winner to be taz, this is for the levelcomplete cheat
	this->winner = 0;
	// PP: SET UP STUFF

	// PP: no crates destroyed yet
	this->numCratesDestroyed=0;

	// PP: load explosion actor
//	LoadActor(BMESEGMENT_MODEL, map.mapName);
//	LoadActor(BALLEXPBALL_MODEL, map.mapName);
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSGAME::update
	Purpose 	: update the West Boss game
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides BOSSGAME::update
*/
void WESTBOSSGAME::update(void)
{
	BG_START_UPDATE;

	int		i;			// PP: loop counters

	// PP: do base-class update
	BOSSGAME::update();

	// PP: NOTE: There's a noticable lag between the two eyes in this level, in stereoscopic mode.
	// PP: I don't have time to fix it though, unfortnately.
	// PP: On the next game I'll be more careful about where camera update gets done.
	// PP: You live and learn!
	this->updateCamera();

	// PP: UPDATE TAZ (player 1)
	this->taz.update();

	// PP: TEMP? MILESTONE
/*	if(this->state == BGSTATE_OUTRO)
	{
		return;
	}*/

	// PP: UPDATE SAM (cpu/player 2)
 	this->sam.update();

	// PP: UPDATE ALL DYNAMITE...

	LA_FOREACH(this->dynamiteList, WBDYNAMITE, dynamite)

 		dynamite->update();

		// PP: if the dynamite is finished remove it from the list
		if(dynamite->state == WBDYNAMITESTATE_FINISHED)
		{
			this->dynamiteList.discardOldest();
		}

	LA_ENDFOREACH(this->dynamiteList)

	// PP: UPDATE EXPLODING CRATES AND SUPER-TNT CRATES...
	// PP: REMOUT: TEMP TEST
	for(i=0; i<WB_NUM_EXPLOSIVE_CRATES; i++)
	{
		if(wbcrate[i].finished) continue;
		if(wbcrate[i].notUsed) continue;// PP: NEW

		this->crates[i].update();
	}

	for(i=0; i<WB_NUM_SUPER_TNT_CRATES; i++)
	{
		if(wbstnt[i].finished) continue;
		if(wbstnt[i].notUsed) continue;// PP: NEW

		this->superTNT[i].update();
	}

	// PP: UPDATE DEPUTIES...

	for(i=0; i<WB_NUM_DEPUTIES; i++)
	{
		this->deputies[i].update();
	}

	// PP: UPDATE SKATEBOMBS...

	LA_FOREACH(this->skateBombList, WBSKATEBOMB, bomb)

		if(bomb->state != WBSBSTATE_FINISHED)
		{
 			bomb->update();
		}

		// PP: if the dynamite is finished remove it from the list
	/*	if(bomb->state == WBSBSTATE_FINISHED)
		{
			this->skateBombList.discardOldest();
		}*/

	LA_ENDFOREACH(this->skateBombList)

	// PP: UPDATE BOMBER-MAN EXPLOSIONS
	WestBossExp_update();

	// PP: SOME DEBUG STUFF...

/*	if(this->cameraCase->KOScamData != NULL)
	{
		this->cameraCase->KOScamData->angle[Y]=Aabs(this->cameraCase->KOScamData->angle[Y]+(controller1.x2*0.1f));
		this->cameraCase->KOScamData->angle[X]=Aabs(this->cameraCase->KOScamData->angle[X]+(controller1.y2*0.1f));
	}*/

//	bkPrintf("cam angle x%5.2f, y%5.2f  player pos: (x%5.2f) x%5.2f, %5.2f, z%5.2f\n", this->cameraCase->KOScamData->angle[X], this->cameraCase->KOScamData->angle[Y], this->taz.actorInstance->actorInstance.position[X], UNITTOMETER(this->taz.actorInstance->actorInstance.position[X]), UNITTOMETER(this->taz.actorInstance->actorInstance.position[Y]), UNITTOMETER(this->taz.actorInstance->actorInstance.position[Z]));



	// PP: state-changey stuff...

	switch(this->state)
	{
	/* PP: REMOUT: OOD	case BGSTATE_INTRO:
		{
			if(this->sam.state == WBSAMSTATE_ATTACK)
			{
				this->setState(BGSTATE_PLAY);
			}
		}
		break;*/

	case BGSTATE_OUTRO:
		{
			BOSSGAME::handle2PlayerOutroState();
		}
		break;
	}
/*
	if(wbtaz.actorInstance->flags & ACTORFLAG_NOCOLLISION)
	{
		WestBoss_debugBox->setColour(COLOUR(255, 0, 0));
		WestBoss_debugBox->sprintf("*** NO COLLIDE ***");
	}
	else
	{
		WestBoss_debugBox->setColour(COLOUR(0, 255, 0));
		WestBoss_debugBox->sprintf("collide");
	}
*/
/*	int i,j;

	i=int((this->taz.pos()[Z]/WB_SQUARE_WIDTH)+(WB_SQUARES_WIDE/2.0f));
	j=int((this->taz.pos()[X]/WB_SQUARE_HEIGHT)+(WB_SQUARES_HIGH/2.0f));

	WestBoss_debugBox->strcatf("\\n%d,%d : %c", i, j, this->grid[j][i]);*/

	// PP: TEMP TEST
/*	{
		TBVector	pos;
		float yValue;
		bmVectorCopy(pos, this->taz.pos());
		pos[Y]+=METERTOUNIT(1);
		ProcessHeightOfPoint(&collisionCache, pos, &yValue, PHOP_EXCLUDEOBJECTS);

		WestBoss_debugBox->strcatf("\\nHEIGHT %5.2f", yValue);
	}*/
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSGAME::shutdown
	Purpose 	: shut-down the West Boss game
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides BOSSGAME::shutdown
*/
void WESTBOSSGAME::shutdown(void)
{
	int		i;						// PP: loop counter - y'know, like you do

	// PP: do base-class shutdown
	BOSSGAME::shutdown();
#ifdef WBDOF
	// PP: stop any focus effects
	VFX_focusReset();
#endif

#ifdef WB_SPINOFF

	// PP: stop any spin-off in progress
	VFX_deactivateEffect(VFX_SPINOFF);

#endif// PP: def #ifdef WB_SPINOFF

#ifdef VFX_MOTIONBLUR_USE_RENDER_TARGET
	// PP: release permanent render target // PP: TODO: find out if this is needed on Xbox/PC
	VFX_releasePermanentRenderTarget();
#endif // PP: def VFX_MOTIONBLUR_USE_RENDER_TARGET

	// PP: put the firebal draw order flag back to what it is normally
	Fireball_applyDrawOrder=FB_NORMALLY_SORT_DRAW_ORDER;

	// PP: SHUT DOWN TAZ (player 1)
	this->taz.shutdown();

	// PP: SHUT DOWN SAM (cpu/player 2)
	this->sam.shutdown();

	// PP: SHUT DOWN ALL EXPLOSIVE CRATES
	for(i=0; i<WB_NUM_EXPLOSIVE_CRATES; i++)
	{
		this->crates[i].shutdown();
	}

	// PP: SHUT DOWN ALL SUPER TNT CRATE
	for(i=0; i<WB_NUM_SUPER_TNT_CRATES; i++)
	{
		this->superTNT[i].shutdown();
	}

	// PP: SHUT DOWN ALL DYNAMITE...
	LA_FORARRAYDO(this->dynamiteList, shutdown());

	// PP: SHUT DOWN ALL SKATEBOMBS...
	LA_FORARRAYDO(this->skateBombList, shutdown());

	// PP: SHUT-DOWN BOMBER-MAN EXPLOSIONS
	WestBossExp_shutdown();

	// PP: chris cam on
	gameStatus.chrisCam=true;

	// PP: clean up camera case
	StopCameraCase(&this->dummyCam,this->cameraCase, false);
//	DeleteCameraCase(this->cameraCase);

	// PP: NEW TEST  MS FRIG
/* PP: REMOUT: NEW TEST		if(this->twoPlayer)
	{
		gameStatus.multiplayer.numPlayers=1;

		// PP: is this bad?: set split screen mode back to default horizontal split// PP: I want to sleep
		setNumScreens(1, DEFAULT_SPLIT_SCREEN_MODE);
	}*/

	// PP: restore normal splitscreen mode
	gameStatus.multiplayer.splitScreenMode=SPLITSCREEN_HORIZONTAL;

	// PP: turn off film borders cos they might be on
	setFilmBorders(false);

	// PP: just so's we know the game is shut-down
	bkPrintf("WESTBOSSGAME::shutdown() complete\n");
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSGAME::postShutdown
	Purpose 	: shutdown to be done after the scene is completely shut down
	Parameters 	: 
	Returns 	: 
	Info 		: puts the display back into 16 bit
*/
void WESTBOSSGAME::postShutdown(void)
{
#ifdef WBDOF
	// PP: back to crappy old 16 bit display
	VFX_focusUnprepare();
#endif
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSGAME::addSkateBomb
	Purpose 	: create a mobile bomb and set it in motion
	Parameters 	: grid x square, grid y square, x speed, z speed, distance to terminus
	Returns 	: 
	Info 		: 
*/
void WESTBOSSGAME::addSkateBomb(const int gridX, const int gridY, const float xSpeed, const float zSpeed, const float range)
{
	this->skateBombList.getNew()->init(gridX, gridY, xSpeed, zSpeed, range);
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSGAME::evaluateGridPos
	Purpose 	: convert x&y grid coordinates to 3d position, by interpolating between grid points
	Parameters 	: x&y grid coordinates, 3d position to fill in
	Returns 	: 
	Info 		: 
*/
void WESTBOSSGAME::evaluateGridPos(const TBVector2D gridPos, TBVector pos) const
{
	// PP: TEMP: just snap to this grid point...
	// PP: TODO: interpolate

	SETVECTOR(pos, this->evaluateGridX(gridPos[X]), this->getHeightOnGrid(gridPos[X], gridPos[Y]), this->evaluateGridZ(gridPos[Y]), 1.0f);
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSGAME::sitPointOnGrid
	Purpose 	: correct the height of a 3D position to sit it onto the grid
	Parameters 	: position to correct, (opt/0.0f)height to add onto grid height
	Returns 	: 
	Info 		: 
*/
void WESTBOSSGAME::sitPointOnGrid(TBVector pos, float height) const
{
	// PP: TEMP: just snap to this grid point...
	// PP: TODO: interpolate

	TBVector	gridPos;

	gridPos[X]=((pos[X]+(WB_WIDTH/2.0f))/WB_SQUARE_WIDTH);
	gridPos[Y]=((pos[Z]+(WB_HEIGHT/2.0f))/WB_SQUARE_HEIGHT);

	pos[Y]=(WESTBOSSGAME::gridPoints[(int)gridPos[Y]][(int)gridPos[X]])[Y];
	pos[Y]+=height;
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSGAME::getCrate
	Purpose 	: get the crate that sits roughly on the specified square
	Parameters 	: x square index, y square index
	Returns 	: the crate that sits roughly on the specified square!
	Info 		: 
*/
WBTNTCRATE* WESTBOSSGAME::getCrate(const int x, const int y)
{
	WBTNTCRATE*		crate;
	int				i;				// PP: loop counter

	for(i=0; i<WB_NUM_EXPLOSIVE_CRATES; i++)
	{
		crate= &this->crates[i];

		if(crate->gridX == x)
		{
			if(crate->gridY == y)
			{
				return crate;
			}
		}
	}

	for(i=0; i<WB_NUM_SUPER_TNT_CRATES; i++)
	{
		crate= &this->crates[i];

		if(crate->gridX == x)
		{
			if(crate->gridY == y)
			{
				return crate;
			}
		}
	}

	return NULL;
}


/* --------------------------------------------------------------------------------
   Function		: WESTBOSSGAME::getAdjacentGroundSquare
   Purpose		: get an adjacent ground square to the square specified
   Parameters	: x coord of square, y coord of square, [in/out]x coord of closest ground square,
					[in/out]y coord of closest ground square
   Returns		: 
   Info			:
*/
void WESTBOSSGAME::getAdjacentGroundSquare(const uint32 gridX, const uint32 gridY, uint32 *const gridXOut, uint32 *const gridYOut)
{

#define TEST_ADJACENT(_x, _y)\
	{\
		if(WESTBOSSGAME::grid[_y][_x] == WBGRID_GROUND)\
		{\
			(*gridXOut)=(_x);\
			(*gridYOut)=(_y);\
			return;\
		}\
	}

	if(gridY > 0)
	{
		TEST_ADJACENT(gridX, gridY-1);

		if(gridY < (WB_SQUARES_HIGH-1))
		{
			TEST_ADJACENT(gridX, gridY+1);
		}
	}

	if(gridX > 0)
	{
		TEST_ADJACENT(gridX-1, gridY);

		if(gridX < (WB_SQUARES_WIDE-1))
		{
			TEST_ADJACENT(gridX+1, gridY);
		}
	}

	ASSERTM(0, "SHITEAHONTAS\n");
}