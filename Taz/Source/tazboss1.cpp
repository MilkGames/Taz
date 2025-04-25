// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Tazboss1.cpp
//   Purpose : First taz boss code
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// SO: TODO:
//	Add electrical effect
//	Make explosions bigger and cover more area
//	Stop respawning until explosions finished
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// SO: Level changes required:
//	Make plunger platform completely inaccessable to Taz (electrify it?  make it
//		higher?)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// SO: This game also demonstraights some lovely hard-coding techniques .. you will
//	find examples of these in the following places:
//
//	File: chillipepper.cpp		Func: UpdateChilliPepperFireCollision()
//	Reason: Chilli pepper response code is in here for when Taz burns ole' Sam
//
//	File: collectibles.cpp		Func: CollectibleExtraUpdateCallback()
//	Reason: To disable chilli pepper
//
//	File: collectibles.cpp		Func: CollectibleExtraUpdateCallback()
//	Reason: Need to control when the chilli pepper appears
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// SO: Special search strings...
//	"GOTO NEXT LEVEL" - Here you must either call a cut-scene or goto the next level
//	"ADD ELECTRICAL EFFECT" - Here an electrical effect must be added to go from
//		Sam's cane to the rocks above Taz's head.
//	"TAZ ON FIRE" - Here fireball response code must be added.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Class Notes:
//	TBSPARK:		This is for the spark actor.
//	TBFUSE_PATH:	This is the fuse path (for drawing, running spark, explosion).
//	TBPLUNGER:		Contains all fuse paths and starts detonation sequences
//	TBSAM:			Controls Sam - where he is and what he's doing.  It could
//					be considered to be the main game controller as the state of
//					the game is dictated by what Sam is doing.
//	TBTAZ:			Contains some functions to control Taz.
//	TAZBOSS1GAME:	Main game.. camera control and pickup control.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



#include "global.h"				// SO: Standard include files
#include "actors.h"
#include "status.h"
#include "display.h"
#include "physics.h"

#include "fallingobjects.h"		// SO: For the rocks
#include "camera.h"				// SO: For camera control
#include "collectibles.h"		// SO: For chilli pepper control
#include "chillipepper.h"		// SO: For chilli pepper response
#include "explode.h"			// SO: For explosion fx
#include "fireball.h"			// SO: For fireballs
#include "paths.h"				// SO: For the fuse paths
#include "holysmoke.h"			// SO: For smoke fx
#include "playerstats.h"

#include "tazboss1.h"


TAZBOSS1GAME	tazBoss1Game;


// SO: Rather handy macros to get sam n' taz
#define tbtaz		(tazBoss1Game.taz)
#define tbsam		(tazBoss1Game.sam)
#define tbplunger1	(tazBoss1Game.plunger1)
#define tbplunger2	(tazBoss1Game.plunger2)


// SO: Model names defined
#define TBROCK_MODEL	"extras\\falling_rock.obe"
#define TBPLUNGER_MODEL	"extras\\tycoontntplunger.obe"
#define TBSPARK_MODEL	"extras\\staticsparks.obe"
#define TBSAM_MODEL		"tycoon.obe"


// SO: World awarness constants
#define TBARENA_CENTRE_RADIUS	 METERTOUNIT(3.0f)											// SO: Radius of centre platform
#define TBARENA_PLATFORM_MAX	 METERTOUNIT(13.5f)											// SO: Min radius of platform (including a little overlap)
#define TBARENA_PLATFORM_MIN	 METERTOUNIT(10.0f)											// SO: Max radius of platform
#define TBARENA_PLATFORM_MIDDLE	 METERTOUNIT(11.2f)											// SO: Distance from centre Sam should try to maintain
#define TBARENA_PLATFORM_YPOS	-METERTOUNIT(5.6f)											// SO: Y position/height of platform

#define TBARENA_PLUNGER1_POS	 METERTOUNIT(19.2f), -METERTOUNIT(5.6f), 0.0f, 1.0f 		// SO: Position of Plunger 1
#define TBARENA_PLUNGER2_POS	-METERTOUNIT(19.2f), -METERTOUNIT(5.6f), 0.0f, 1.0f			// SO: Position of Plunger 2

#define TBARENA_LAUNCH1_SAMPOS	 METERTOUNIT(12.0f), -METERTOUNIT(5.6f), 0.0f, 1.0f 		// SO: Launch position of Sam to get to Plunger 1
#define TBARENA_LAUNCH2_SAMPOS	-METERTOUNIT(12.0f), -METERTOUNIT(5.6f), 0.0f, 1.0f 		// SO: Launch position of Sam to get to Plunger 1

#define TBARENA_PLUNGER1_SAMPOS	 METERTOUNIT(18.0f), -METERTOUNIT(5.6f), 0.0f, 1.0f 		// SO: Position of Sam to activate Plunger 1
#define TBARENA_PLUNGER2_SAMPOS	-METERTOUNIT(18.0f), -METERTOUNIT(5.6f), 0.0f, 1.0f 		// SO: Position of Sam to activate Plunger 1

#define TBARENA_PLUNGER1_ANG	 HALFPI														// SO: Angle around centre of Plunger 1
#define TBARENA_PLUNGER2_ANG	-HALFPI														// SO: Angle around centre of Plunger 2

#define TBARENA_TNT_CORNER_POS_X  METERTOUNIT(12.0f)										// SO: TNT corner position (other 3 corners are reflected vectors)
#define TBARENA_TNT_CORNER_POS_Y -METERTOUNIT(4.5f)
#define TBARENA_TNT_CORNER_POS_Z  METERTOUNIT(12.0f)

#define TBARENA_TNT_SIDE_POS_X	  METERTOUNIT(0.0f)											// SO: TNT side position
#define TBARENA_TNT_SIDE_POS_Y	 -METERTOUNIT(4.5f)
#define TBARENA_TNT_SIDE_POS_Z	  METERTOUNIT(17.0)

#define TBSAM_RESPAWN_HEIGHT	 METERTOUNIT(3.0f)											// SO: Y Position Sam will respawn at
#define TBSAM_SPAWN_CAMPOS_X	 METERTOUNIT(0.5f)											// SO: Camera position for Sam spawn
#define TBSAM_SPAWN_CAMPOS_Y	-METERTOUNIT(5.0f)											// SO: Note: Negate X/Z for right side
#define TBSAM_SPAWN_CAMPOS_Z	 METERTOUNIT(9.15f)
#define TBSAM_PLUNGE_CAMPOS_X	 METERTOUNIT(14.5f)											// SO: Camera position for Sam using plunger
#define TBSAM_PLUNGE_CAMPOS_Y	-METERTOUNIT(3.5f)											// SO: Note: Negate X/Z for opposite plunger
#define TBSAM_PLUNGE_CAMPOS_Z	 METERTOUNIT(4.5f)
#define TBSAM_BURNS_CAMPOS_X	 METERTOUNIT(16.0f)											// SO: Camera position for Sam getting chilli burns
#define TBSAM_BURNS_CAMPOS_Y	-METERTOUNIT(3.5f)											// SO: Note: Negate X/Z for opposite plunger
#define TBSAM_BURNS_CAMPOS_Z	 METERTOUNIT(0.0f)

#define TBTAZ_SPAWN_POSITION	-METERTOUNIT(4.8f), -METERTOUNIT(5.5f), METERTOUNIT(10.4f), 1.0f 	// SO: Initial position of Taz


// SO: Some defines to make the fuse path code more readable
#define SIDE_LEFT	0
#define SIDE_RIGHT	1
#define PATH_TOP	0
#define PATH_MIDDLE	1
#define PATH_BOTTOM	2


// SO: Game difficulty setting stuff
#define GET_DIFFICULTY_VALUE(_a,_b,_c)	(gameStatus.globalSettings.difficulty==DIFF_EASY?_a:(gameStatus.globalSettings.difficulty==DIFF_MEDIUM?_b:_c))

#define NUM_ROCKS_EASY				 8						// SO: Number of rocks generated (easy mode)
#define NUM_ROCKS_MEDIUM			12						// SO: Number of rocks generated (medium mode)
#define NUM_ROCKS_HARD				16						// SO: Number of rocks generated (hard mode)

#define ASS_BURNS_B4_DEATH_EASY		6						// SO: Number of times Sam's ass has to be burnt to win (easy mode)
#define ASS_BURNS_B4_DEATH_MEDIUM	6						// SO: Number of times Sam's ass has to be burnt to win (medium mode)
#define ASS_BURNS_B4_DEATH_HARD		6						// SO: Number of times Sam's ass has to be burnt to win (hard mode)

// SO: A strangely useful vector
TBVector ArenaCentre = {0.0f,-METERTOUNIT(5.5f),0.0f,1.0f};


/* SO: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
											  TBSPARK methods
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*	--------------------------------------------------------------------------------
	Function 	: TBSPARK::init
	Purpose 	: initialisation
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TBSPARK::init(void)
{
	// SO: Init actor ..
	ACTORINSTANCEUSER::init(TBSPARK_MODEL, ArenaCentre, 0.0f, BGACT_ANIMATION );
	bmVectorSet(this->scale(),4.5f,4.5f,4.5f,1.0f);

	// SO: Play looping sparky animation
	this->playAnim("sparks",true);

	// SO: Turn off drawing (this is turned on when fuse is active)
	this->actorInstance->flags |= ACTORFLAG_DONTDRAW;

}



/* SO: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
											TBFUSE_PATH methods
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

char FuseNames[NUM_PLUNGERS*NUM_FUSE_SIDES*NUM_FUSE_PATHS][32] =
{
	"box-fuse b- right_01_spline01",		// plunger 1	// left
	"box-fuse b- right_02_spline01",
	"box-fuse b- right_03_spline01",
	"box-fuse b- left_01_spline01",							// right
	"box-fuse b- leftt_02_spline01",
	"box-fuse b- left_03_spline01",
	"box- fuse a- left_01_spline01",		// plunger 2	// left
	"box-fuse a- left_02_spline01",
	"box-fuse a- left_03_spline01",
	"box-fuse a- right_01_spline01",						// right
	"box-fuse a- right_02_spline01",
	"box-fuse a- right_03_spline01",
};


/*	--------------------------------------------------------------------------------
	Function 	: TBFUSE_PATH::init
	Purpose 	: initialisation
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TBFUSE_PATH::init(int iPlunger, int iSide, int iPath)
{
	this->time = -1.0f;
	this->maxTime = 1.0f;
	this->lastSparkPoint = -1;

	// SO: Locate fuse path
	char *pathname = FuseNames[ (iPlunger*NUM_FUSE_SIDES*NUM_FUSE_PATHS) + (iSide*NUM_FUSE_PATHS) + (iPath) ];

	if( !(this->fusePath = FindPathInList(pathname,0)) )
		bkPrintf("PATHSEARCH: ERROR: Path %s not found",pathname);
	else
	{
		bkPrintf("PATHSEARCH: SUCESS: Path %s found",pathname);

		// SO: Calc length of path
		this->pathLength = 0.0f;
		for( int i=1; i<this->fusePath->numNodes; i++ )
			this->pathLength += bmVectorDistance(this->fusePath->nodes[i-1].position,this->fusePath->nodes[i].position);
	}

	// SO: Set explosion point (Note: These are hard-coded values for maximum-control)
	int path;
	if( iPlunger )
		path = (NUM_FUSE_PATHS-1) - iPath;		// SO: Paths are reversed for second plunger
	else
		path = iPath;

	this->index = path;

	switch( path )
	{
	case PATH_TOP:
		// SO: Corner pos (0)
		bmVectorSet(this->explosionPoint,  TBARENA_TNT_CORNER_POS_X, TBARENA_TNT_CORNER_POS_Y, TBARENA_TNT_CORNER_POS_Z, 1.0f);
		col[0] = 32;	col[1] = 127;	col[2] = 32;	col[3] = 120;
		break;

	case PATH_MIDDLE:
		// SO: Side pos (1)
		bmVectorSet(this->explosionPoint,  TBARENA_TNT_SIDE_POS_X, TBARENA_TNT_SIDE_POS_Y, TBARENA_TNT_SIDE_POS_Z, 1.0f);
		col[0] = 127;	col[1] = 127;	col[2] = 32;	col[3] = 120;
		break;

	case PATH_BOTTOM:
		// SO: Opposite corner (2) (Note: reverse X)
		bmVectorSet(this->explosionPoint, -TBARENA_TNT_CORNER_POS_X, TBARENA_TNT_CORNER_POS_Y, TBARENA_TNT_CORNER_POS_Z, 1.0f);
		col[0] = 127;	col[1] = 32;	col[2] = 32;	col[3] = 120;
		break;
	}

	// SO: If other side reverse Z
	if( (!iPlunger && iSide) || (iPlunger && !iSide) )
		this->explosionPoint[Z] = -this->explosionPoint[Z];

	bkPrintf("  Pos [%.2f,%.2f,%.2f]\n",this->explosionPoint[X],this->explosionPoint[Y],this->explosionPoint[Z]);

	// SO: Initialise spark
	this->spark.init();
}


/*	--------------------------------------------------------------------------------
	Function 	: TBFUSE_PATH::startFuse
	Purpose 	: Starts the fuse going
	Parameters 	: secs - number of seconds to take (default = 1.0)
	Returns 	: 
	Info 		: 
*/
void TBFUSE_PATH::startFuse(float secs)
{
	// SO: Set maxTime and set time to zero (to make it active)
	this->maxTime = secs;
	this->time = 0.0f;
	this->lastSparkPoint = -1;

	// SO: Start drawing the spark
	this->spark.actorInstance->flags &= ~ACTORFLAG_DONTDRAW;
}

/*	--------------------------------------------------------------------------------
	Function 	: TBFUSE_PATH::addExplosion
	Purpose 	: Adds an explosion
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TBFUSE_PATH::addExplosion(void)
{
	TBVector pos;

	// SO: Add sparky explosion
	bmVectorSet(pos, this->explosionPoint[X] - METERTOUNIT(2.0f) + (METERTOUNIT(4.0f) * bmRandf()),
					 this->explosionPoint[Y] - METERTOUNIT(1.0f) + (METERTOUNIT(2.0f) * bmRandf()),
					 this->explosionPoint[Z] - METERTOUNIT(2.0f) + (METERTOUNIT(4.0f) * bmRandf()), 1.0f);
	StartExplosion(pos, 0.3f + (bmRandf()*0.4f), 30.0f + (bmRandf()*40.f), "explosion.wav", EXPLOSIONTYPE_CARTOON, 50 + (int)(bmRandf()*50.f));
}


/*	--------------------------------------------------------------------------------
	Function 	: TBFUSE_PATH::addFireball
	Purpose 	: Adds a fireball
	Parameters 	: angle = direction of fireball, range = distance
	Returns 	: 
	Info 		: 
*/
#define FIREBALL_SPEED		METERTOUNIT(30.0f)
#define FIREBALL_SCALE		FB_DEFAULT_SCALE		// PP: you can specify a thickness for explosions now - FB_DEFAULT_SCALE is currently 1.0f
#define FIREBALL_HEAT		FB_DEFAULT_HEAT			// PP: you can specity a heat for explosions now too - FB_DEFAULT_HEAT is currently 1.0f

void TBFUSE_PATH::addFireball(float angle, float range, float xOff, float yOff, float zOff)
{
	TBVector vel = {FIREBALL_SPEED*bmSin(angle), 0.0f, FIREBALL_SPEED*bmCos(angle), 1.0f};
	TBVector pos = {this->explosionPoint[X]+xOff, this->explosionPoint[Y]+yOff, this->explosionPoint[Z]+zOff, 1.0f};
	Fireball_create(pos, vel, range, FIREBALL_SCALE, FIREBALL_HEAT, TBFUSE_PATH::fireballCallback, (long32)this);
}


/*	--------------------------------------------------------------------------------
	Function 	: TBFUSE_PATH::update
	Purpose 	: Updates the fuse
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TBFUSE_PATH::update()
{
	// SO: A time above 0.0 means an active fuse
	if( this->time >= 0.0f )
	{
		// SO: Update time value
		this->time += (fTime/this->maxTime);

		// SO: Check if fuse finished
		if( this->time >= 1.0f )
		{
			// SO: Whooo!  Explosion time!
			this->time = -1.0f;
			this->lastSparkPoint = -1;
			this->spark.actorInstance->flags |= ACTORFLAG_DONTDRAW;

			// SO: Add fireballs
			float mainAngle = bmATan2(-this->explosionPoint[X],-this->explosionPoint[Z]);

			switch( this->index )
			{
			case PATH_TOP:
				this->addFireball(mainAngle-QUARTPI, METERTOUNIT(12.0f));
				this->addFireball(mainAngle        , METERTOUNIT(8.0f));
				break;

			case PATH_MIDDLE:
				this->addFireball(mainAngle, METERTOUNIT(11.5f), -METERTOUNIT(2.0f), 0.0f, 0.0f);
				this->addFireball(mainAngle, METERTOUNIT(11.5f),  METERTOUNIT(2.0f), 0.0f, 0.0f);
				break;

			case PATH_BOTTOM:
				this->addFireball(mainAngle        , METERTOUNIT(8.0f));
				this->addFireball(mainAngle+QUARTPI, METERTOUNIT(15.0f));
				break;
			}

			// SO: Add explosions
			this->addExplosion();
			this->addExplosion();
		}
		else
		{
			// SO: Update spark's position

			// SO: Calc dist to travel
			float reqDist = this->pathLength * this->time;
			float thisDist;
			float currDist = 0.0f;
			float tPos;
			int	  i, numPoints;

			numPoints = this->fusePath->numNodes;

			for( i=1; i<numPoints; i++ )
			{
				thisDist = bmVectorDistance(this->fusePath->nodes[i-1].position,this->fusePath->nodes[i].position);
				if( (currDist+thisDist) > reqDist )
				{
					// SO: Spark is between i-1 and i
					this->lastSparkPoint = i-1;

					// SO: Get percentage along line
					reqDist -= currDist;
					tPos = reqDist/thisDist;

					// SO: Set spark position
					bmVectorLerp(this->spark.pos(),this->fusePath->nodes[i-1].position,this->fusePath->nodes[i].position,tPos);

					break;
				}

				currDist += thisDist;
			}

			this->spark.update();
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: TBFUSE_PATH::fireballCallback
	Purpose 	: Callback during fireball update
	Parameters 	: Fireball and context
	Returns 	: 
	Info 		: 
*/
void TBFUSE_PATH::fireballCallback(FIREBALL* const fireball, const long32 context)
{
	// SO: Do collision checks with Taz

	// "TAZ ON FIRE"

	// SO: Add sparky explosion
	if( bmRandf() > 0.9f )
	{
		// SO: Make the sparky explosions somewhat infrequent
		// They are also difficult to see, so maybe remove the explosion itself, but
		// retain the sfx, etc.
		TBFUSE_PATH	*path = (TBFUSE_PATH*)context;
		path->addExplosion();
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: TBFUSE_PATH::draw
	Purpose 	: Draws the fuse
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
#define MAX_PATH_POINTS 40	// SO: Note: If path points increases, increase this value accordingly
#define FUSESIZE_BIG	30.0f
#define FUSESIZE_SMALL	10.0f

void TBFUSE_PATH::draw()
{
	// Draw fuse
	TBVector	posArray[MAX_PATH_POINTS];
	float		sizeArray[MAX_PATH_POINTS];
	int			colArray[MAX_PATH_POINTS][4];
	float	colMod, size;
	int		i, numPoints, numNodes;

	numNodes = this->fusePath->numNodes;

	if( numNodes > (MAX_PATH_POINTS-2) )
	{
		bkPrintf("ERROR!!  Increase MAX_PATH_POINTS to %d!\n",numNodes+2);
		return;
	}

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	bdSetIdentityObjectMatrix();

	if( this->time > 0.0f )
	{
		// SO: If fuse is active, make brighter and make start size big
		colMod = 2.0f;
		size = FUSESIZE_BIG;
	}
	else
	{
		// SO: Else use normal half-colour and small size
		colMod = 1.0f;
		size = FUSESIZE_SMALL;
	}

	numPoints = 0;
	for( i=0; i<numNodes; i++ )
	{
		// SO: Add path point
		bmVectorCopy(posArray[numPoints],this->fusePath->nodes[i].position);
		sizeArray[numPoints] = size;
		colArray[numPoints][0] = (int)(this->col[0] * colMod);
		colArray[numPoints][1] = (int)(this->col[1] * colMod);
		colArray[numPoints][2] = (int)(this->col[2] * colMod);
		colArray[numPoints][3] = this->col[3];
		numPoints++;

		if( i == this->lastSparkPoint )
		{
			// SO: Add spark point
			bmVectorCopy(posArray[numPoints],this->spark.pos());
			sizeArray[numPoints] = size;
			colArray[numPoints][0] = (int)(this->col[0] * colMod);
			colArray[numPoints][1] = (int)(this->col[1] * colMod);
			colArray[numPoints][2] = (int)(this->col[2] * colMod);
			colArray[numPoints][3] = this->col[3];
			numPoints++;

			size = FUSESIZE_SMALL;

			bmVectorCopy(posArray[numPoints],this->spark.pos());
			sizeArray[numPoints] = size;
			colArray[numPoints][0] = (int)(this->col[0] * colMod);
			colArray[numPoints][1] = (int)(this->col[1] * colMod);
			colArray[numPoints][2] = (int)(this->col[2] * colMod);
			colArray[numPoints][3] = this->col[3];
			numPoints++;
		}
	}

	TBPrimVertex2D	verts[(MAX_PATH_POINTS*4)+4];
	int noofVerts;
	noofVerts = bdDrawMultiStreakClipped(numPoints,posArray[0],sizeArray,sizeArray,&colArray[0][0],verts,0.0f);

	bdSetTexture(0, NULL);

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);
	if (noofVerts>=3)// TP: Put this check in to stop crash problem on pc
	{
		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, verts, noofVerts, BVERTTYPE_SINGLE2D);
	}

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
}




/* SO: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
											  TBPLUNGER methods
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*	--------------------------------------------------------------------------------
	Function 	: TBPLUNGER::init
	Purpose 	: initialisation
	Parameters 	: actualPos - position in world
	              launchPos - position on walkway for Sam to launch himself
				  landPos
	Returns 	: 
	Info 		: 
*/
void TBPLUNGER::init(int iPlunger, TBVector actualPos, TBVector launchPos, TBVector landPos, float yRotation)
{
	// SO: Init actor .. no need for collision as Taz musn't get close enough
	ACTORINSTANCEUSER::init(TBPLUNGER_MODEL, actualPos, yRotation, BGACT_COLLCACHE | BGACT_ANIMATION | BGACT_CARTOON );

	bmVectorCopy(this->launchVec,launchPos);
	bmVectorCopy(this->landVec,landPos);

	this->playAnim("static",true);

	this->index = iPlunger;

	// SO: Initialise fuse paths
	int i,j;

	for( i=0; i<NUM_FUSE_SIDES; i++ )
		for( j=0; j<NUM_FUSE_PATHS; j++ )
		{
			bkPrintf("Initialising Plunger %d .. side %d .. path %d ...   ",iPlunger,i,j);
			this->fusePaths[i][j].init(iPlunger,i,j);
		}
}



/*	--------------------------------------------------------------------------------
	Function 	: TBPLUNGER::update
	Purpose 	: update routine
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TBPLUNGER::update(void)
{
	ACTORINSTANCEUSER::update();

	int i,j;

	// SO: Update all fuses
	for( i=0; i<NUM_FUSE_SIDES; i++ )
		for( j=0; j<NUM_FUSE_PATHS; j++ )
			this->fusePaths[i][j].update();
}


/*	--------------------------------------------------------------------------------
	Function 	: TBPLUNGER::draw
	Purpose 	: draw routine
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TBPLUNGER::draw(void)
{
	int i,j;

	// SO: Draw all fuses
	for( i=0; i<NUM_FUSE_SIDES; i++ )
		for( j=0; j<NUM_FUSE_PATHS; j++ )
		{
			this->fusePaths[i][j].draw();
		}
}


/*	--------------------------------------------------------------------------------
	Function 	: TBPLUNGER::triggerExplosivePattern
	Purpose 	: Triggers an explosion of unimaginable destruction
	Parameters 	: pattern - which fuses to trigger
	Returns 	: 
	Info 		: 
*/
void TBPLUNGER::triggerExplosivePattern(TBExplosionPattern pattern)
{
	switch( pattern )
	{
	case TBPATTERN_MIDDLE:
		// SO: Set middle fuses going
		this->fusePaths[SIDE_LEFT ][PATH_MIDDLE].startFuse(5.0f);			// SO: Note parameter (5.0) is time in seconds for fuse to take
		this->fusePaths[SIDE_RIGHT][PATH_MIDDLE].startFuse(5.0f);
		break;

	case TBPATTERN_SIDEWAVE:
		// SO: Set all fuses along 1 side going
		this->fusePaths[SIDE_LEFT ][PATH_TOP   ].startFuse(4.25f);
		this->fusePaths[SIDE_LEFT ][PATH_MIDDLE].startFuse(5.00f);
		this->fusePaths[SIDE_LEFT ][PATH_BOTTOM].startFuse(5.75f);
		break;

	case TBPATTERN_TBM:
		// SO: Stagger all fuses so top goes first, then bottom, then middle
		this->fusePaths[SIDE_LEFT ][PATH_TOP   ].startFuse(4.0f);
		this->fusePaths[SIDE_RIGHT][PATH_TOP   ].startFuse(4.0f);
		this->fusePaths[SIDE_LEFT ][PATH_MIDDLE].startFuse(8.0f);
		this->fusePaths[SIDE_RIGHT][PATH_MIDDLE].startFuse(8.0f);
		this->fusePaths[SIDE_LEFT ][PATH_BOTTOM].startFuse(6.0f);
		this->fusePaths[SIDE_RIGHT][PATH_BOTTOM].startFuse(6.0f);
		break;

	case TBPATTERN_FULLON:
		// SO: Same as side wave but both sides
		this->fusePaths[SIDE_LEFT ][PATH_TOP   ].startFuse(4.0f);
		this->fusePaths[SIDE_RIGHT][PATH_TOP   ].startFuse(4.25f);
		this->fusePaths[SIDE_LEFT ][PATH_MIDDLE].startFuse(4.5f);
		this->fusePaths[SIDE_RIGHT][PATH_MIDDLE].startFuse(4.75f);
		this->fusePaths[SIDE_LEFT ][PATH_BOTTOM].startFuse(5.0f);
		this->fusePaths[SIDE_RIGHT][PATH_BOTTOM].startFuse(5.25f);
		break;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: TBPLUNGER::isActive
	Purpose 	: Test if this plunger has active fuses
	Parameters 	: 
	Returns 	: If fuses are active
	Info 		: 
*/
int TBPLUNGER::isActive(void)
{
	int i,j;

	for( i=0; i<NUM_FUSE_SIDES; i++ )
		for( j=0; j<NUM_FUSE_PATHS; j++ )
		{
			// SO: If any fuse has a time greater than 0 it is active
			if( this->fusePaths[i][j].time > 0.0f )
				return TRUE;
		}

	return FALSE;
}




/* SO: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												TBSAM methods
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*	--------------------------------------------------------------------------------
	Function 	: TBSAM::init
	Purpose 	: initialisation
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TBSAM::init(void)
{
	TBVector initPos;

	bmVectorSet(initPos, -tbtaz.pos()[X], tbtaz.pos()[Y], -tbtaz.pos()[Z], 1.0f);

	// SO: Init actor .. no need for collision as Taz musn't get close enough
	ACTORINSTANCEUSER::init(TBSAM_MODEL, initPos, 0.0f, BGACT_BEHAVIOUR | BGACT_ANIMATION | BGACT_LINE );

	// SO: Initialise data (see class in header file for descriptions)
	this->bodyNode = baFindNode(this->actorInstance->actorInstance.rootNodeInstance, "neck");
	this->caneNode = baFindNode(this->actorInstance->actorInstance.rootNodeInstance, "canestem");
	this->lungePlunger = NULL;

	this->wasAssBurnt = false;
	this->totalAssBurns = 0;

	this->validAction = false;
	this->seenCloseup = false;
	samWinsPoint = false;

	// SO: Initialise states
	this->state = TBSAMSTATE_NOSTATE;
	this->subState = TBSAMSUBSTATE_NOSUBSTATE;
	this->setState(TBSAMSTATE_RESPAWN);
}

// TP: temp
void TazBoss1Reset(void)
{
	TBVector	initPos;
	TBVector	tempVector;

	// TP: reset taz
	bmVectorSet(tazBoss1Game.taz.pos(), TBTAZ_SPAWN_POSITION );

	bmVectorZero(tazBoss1Game.taz.velocityBeforeSuspend);
	tazBoss1Game.taz.suspended = FALSE;

	// TP: reset sam
	bmVectorSet(initPos, -tbtaz.pos()[X], tbtaz.pos()[Y], -tbtaz.pos()[Z], 1.0f);
	bmVectorCopy(tazBoss1Game.sam.pos(), initPos);

	tazBoss1Game.sam.lungePlunger = NULL;

	tazBoss1Game.sam.wasAssBurnt = false;
	tazBoss1Game.sam.totalAssBurns = 0;

	tazBoss1Game.sam.validAction = false;
	tazBoss1Game.sam.seenCloseup = false;
	tazBoss1Game.sam.samWinsPoint = false;

	// SO: Initialise states
	tazBoss1Game.sam.state = TBSAMSTATE_NOSTATE;
	tazBoss1Game.sam.subState = TBSAMSUBSTATE_NOSUBSTATE;
	tazBoss1Game.sam.setState(TBSAMSTATE_RESPAWN);

	tazBoss1Game.readyForChilli = TRUE;
	tazBoss1Game.numExplosionsTriggered = 0;

	tempVector[X] = 0;
	tempVector[Y] = METERTOUNIT(1);
	tempVector[Z] = 0;
	SetCameraLookAtActorInstanceOffset(&camera[0],tempVector);
}


/*  --------------------------------------------------------------------------------
	Function	: ResetTazboss1Callback
	Purpose		: 
	Parameters	: 
	Returns		: 
	Info		: 
*/

static int32 ResetTazboss1Callback(void *context, EBossGameResetChoice choice)
{
	tazBoss1Game.continueChoice = choice;

	// PP: must return a value
	// PP: NOTE: the FadeFinishedCallback type, as which this is used, has a void return type
	return 0xB16B00B5;
}


/*  --------------------------------------------------------------------------------
	Function	: FadeDownCallback
	Purpose		: Called when the fade down on game loss has finished
	Parameters	: 
	Returns		: 
	Info		: 
*/

static void FadeDownCallback(void)
{
	tazBoss1Game.setCamType(TBCAM_DEFAULT);
}


/*	--------------------------------------------------------------------------------
	Function 	: TBSAM::update
	Purpose 	: update routine
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
#define TIME_B4_STAND_OFF		5.0f				// SO: RUNAROUND: Time in state before going to stand off
#define MAX_ANGLE_DIFF_B4_MOVE	(PI/16.0f)			// SO: RUNAROUND: Maximum angle difference between angle & desired before Sam will run left/right
#define MAX_ANGLE_DIFF_B4_LUNGE	(PI/16.0f)			// SO: RUNAROUND: Maximum angle difference between angle & plunger before Sam goes to lunge

#define ROCKS_YPOS			METERTOUNIT(2.0f)		// SO: STANDOFF: Minimum height rocks start at
#define ROCKS_YPOS_RAND		METERTOUNIT(10.0f)		// SO: STANDOFF: Randomness added to rocks height
#define ROCKS_RADIUS_RAND	METERTOUNIT(1.2f)		// SO: STANDOFF: Randomness applied to rocks radius
#define ROCKS_ANGLE_RAND	(PI/8.0f)				// SO: STANDOFF: Randomness applied to rocks angle

#define MAX_DISTANCE_CHECK	METERTOUNIT(0.2f)		// SO: General distance macro when checking if we've reached a vector

#define MIN_DIST_B4_ATTACK	METERTOUNIT(3.0f)		// SO: RUNAROUND: Distance between sam and taz before attacking
#define MAX_DIST_B4_ATTACK	METERTOUNIT(4.0f)		// SO: ATTACKTAZ: I need the two distances to stop Sam having a fit
#define DIST_B4_TWAT		METERTOUNIT(2.0f)		// SO: ATTACKTAZ: Distance before Sam actually swings at Taz
#define DIST_B4_TWAT_HIT	METERTOUNIT(1.5f)		// SO: ATTACKTAZ: Distance before swinging Sam actually hits Taz


void TBSAM::update(void)
{
	ACTORINSTANCEUSER::update();
	int retval;

	this->timer += fTime;

	// TP: Check for falling through the ground
	if (pos()[Y] < -1410.0f) pos()[Y] = -1393.72f;

	switch( this->state )
	{

	/////////////////////////////////////////////////////////////////////////////////////
	case TBSAMSTATE_RUNABOUT:
	/////////////////////////////////////////////////////////////////////////////////////

		if( (this->timer > TIME_B4_STAND_OFF) && (this->subState == TBSAMSUBSTATE_RUNABOUT_WAIT) && (tazBoss1Game.getArenaArea(this) & TBARENA_AREAS_PLATFORM_CENTRE) )
			this->setState(TBSAMSTATE_STANDOFF);
		else
		{
			// SO: Get Taz's current angle (around the arena)
			float tazAngle = tazBoss1Game.getArenaAngle(&tbtaz);

			// SO: Add PI - this is Sam's desired angle
			float desiredAngle;
			if( tazAngle > 0.0f )
				desiredAngle = tazAngle - PI;
			else
				desiredAngle = tazAngle + PI;

			// SO: Check for nearby Taz
			if( this->dist(tbtaz) <= MIN_DIST_B4_ATTACK )
			{
				this->setState(TBSAMSTATE_ATTACKTAZ);
				return;
			}	

			// SO: Check for nearby plungers
			float samAngle = tazBoss1Game.getArenaAngle(this);
			float diffAngle;

			diffAngle = FindShortestAngleSigned(samAngle,TBARENA_PLUNGER1_ANG);
			if( bmFAbs(diffAngle) <	MAX_ANGLE_DIFF_B4_LUNGE )
			{
				// SO: We're near to plunger 1 .. go get it!
				this->setState(TBSAMSTATE_LUNGEFORPLUNGE,TBSAMSUBSTATE_LUNGEFORPLUNGE_1);
			}
			else
			{
				diffAngle = FindShortestAngleSigned(samAngle,TBARENA_PLUNGER2_ANG);
				if( bmFAbs(diffAngle) <	MAX_ANGLE_DIFF_B4_LUNGE )
				{
					// SO: We're near to plunger 2 .. go get it!
					this->setState(TBSAMSTATE_LUNGEFORPLUNGE,TBSAMSUBSTATE_LUNGEFORPLUNGE_2);
				}
				else
				{
					// SO: No plungers nearby, so get Sam to face the centre of the arena
					this->YrotateToFace(ArenaCentre);

					// SO: And play either RunLeft or RunRight anims depending on Sam's angle
					diffAngle = FindShortestAngleSigned(samAngle,desiredAngle);

					if( bmFAbs(diffAngle) <	MAX_ANGLE_DIFF_B4_MOVE )
						this->setSubState(TBSAMSUBSTATE_RUNABOUT_WAIT,false);
					else if( diffAngle < 0.0f )
						this->setSubState(TBSAMSUBSTATE_RUNABOUT_LEFT,false);
					else
						this->setSubState(TBSAMSUBSTATE_RUNABOUT_RIGHT,false);
				}
			}

			if( this->subState != TBSAMSUBSTATE_RUNABOUT_WAIT )
			{
				// SO: Run-around adjustment code ...
				// SO: Slerp sam to desired position...

				TBVector desiredPos = {TBARENA_PLATFORM_MIDDLE*bmSin(samAngle), this->pos()[Y], TBARENA_PLATFORM_MIDDLE*bmCos(samAngle), 1.0f};
				bmVectorLerp(this->pos(),desiredPos,this->pos(),0.25f);
			}

		}
		break;


	/////////////////////////////////////////////////////////////////////////////////////
	case TBSAMSTATE_STANDOFF:
	/////////////////////////////////////////////////////////////////////////////////////

		switch( this->subState )
		{
		case TBSAMSUBSTATE_STANDOFF_SHOOTWALL:
			if( this->validAction )
			{
				this->validAction = false;

				// "ADD ELECTRICAL EFFECT"
				
				// SO: Blast dem rocks
				float radius, angle, scale;
				int max = GET_DIFFICULTY_VALUE(NUM_ROCKS_EASY,NUM_ROCKS_MEDIUM,NUM_ROCKS_HARD);
				TBVector rockPos;

				for( int i=0; i<max; i++ )
				{
					radius = TBARENA_PLATFORM_MIDDLE - ROCKS_RADIUS_RAND + (bmRandf() * (ROCKS_RADIUS_RAND*2.0f));
					angle = Aabs( bmATan2(-this->pos()[X], -this->pos()[Z]) - ROCKS_ANGLE_RAND + (bmRandf() * (ROCKS_ANGLE_RAND*2.0f)) );
					bmVectorSet(rockPos, radius*bmSin(angle), ROCKS_YPOS+(bmRandf()*ROCKS_YPOS_RAND), radius*bmCos(angle), 1.0f);

					ACTORINSTANCE *instance = DropObject(&map.levelInstances,NULL,NULL,TBROCK_MODEL,rockPos,
						FALLINGTYPE_ONCE,FALLINGTYPE_DESTROY,NULL,NULL,GetHeightOfPoint(rockPos,0),FALSE);

					if (instance)
					{
						EnableActorCartoonRenderMode(&instance->actorInstance);
						// SO: Set scale
						scale = 0.25f + bmRandf();
						bmVectorSet(instance->actorInstance.scale,scale,scale,scale,1.0f);
					}
				}

				this->timer = -100.0f;
			}

			if( !this->playingAnim() )
				this->setState(TBSAMSTATE_RUNABOUT);
			break;
		}

		break;


	/////////////////////////////////////////////////////////////////////////////////////
	case TBSAMSTATE_ATTACKTAZ:
	/////////////////////////////////////////////////////////////////////////////////////

		this->YrotateToFace(tbtaz);

		switch( this->subState )
		{
		case TBSAMSUBSTATE_ATTACKTAZ_DEFEND:
			{
				float dist = this->dist(tbtaz);
				if( dist > MAX_DIST_B4_ATTACK )
				{
					// SO: Taz has buggered off .. continue the quest for the mystical plunger
					this->setState(TBSAMSTATE_RUNABOUT);
					return;
				}

				if( dist <= DIST_B4_TWAT )
				{
					this->setSubState(TBSAMSUBSTATE_ATTACKTAZ_ATTACK);
					return;
				}
			}
			break;

		case TBSAMSUBSTATE_ATTACKTAZ_ATTACK:
			if( this->validAction )
			{
				this->validAction = false;

				//TBVector canePos;
				//baGetNodesWorldPosition(&this->actorInstance->actorInstance,this->caneNode,canePos);
				//float dist = tbtaz.dist(canePos);
				float dist = this->dist(tbtaz);
				if( dist <= DIST_B4_TWAT_HIT )
				{
					// SO: Taz hit!
					// SO: Code stolen (before being slightly altered) from bumper.cpp
					// SO: Note also I don't remove health .. chances are taz'll fall in the lava anyway
					TBVector velocity;
					float angle = bmATan2(tbtaz.pos()[X]-this->pos()[X], tbtaz.pos()[Z]-this->pos()[Z]);

					bmVectorSet(velocity,bmSin(angle), 0.5f, bmCos(angle), 1.0f);
					bmVectorScaleToLength(velocity, velocity, METERTOUNIT(10.0f));

					AddActorVelocity(tbtaz.actorInstance->actorBehaviour, velocity);
					if(tbtaz.actorInstance->actorAnimation->useMotionBone == TRUE)
					{
						RequestStateChange(tbtaz.actorInstance, STATE_PROJECTILESLIDE);
						tbtaz.actorInstance->actorAnimation->useMotionBone = FALSE;
					}

					// SO: This makes it crash?
					//tbtaz.actorInstance->bumperInfo->justKoiked = 1;
				}
			}

			if( !this->playingAnim() )
				this->setSubState(TBSAMSUBSTATE_ATTACKTAZ_DEFEND);
			break;
		}

		break;


	/////////////////////////////////////////////////////////////////////////////////////
	case TBSAMSTATE_LUNGEFORPLUNGE:
	/////////////////////////////////////////////////////////////////////////////////////

		switch( this->subState )
		{
		case TBSAMSUBSTATE_LUNGEFORPLUNGE_GOLAUNCH:
			{
				// TP: make sure we are still facing target
				this->YrotateToFace(this->lungePlunger->launchVec);
				
				// SO: Check dist to target...
				float dist = this->XZdist(this->lungePlunger->launchVec);
				if( dist <= MAX_DISTANCE_CHECK )
				{
					this->setSubState(TBSAMSUBSTATE_LUNGEFORPLUNGE_FLYING);
				}
			}
			break;

		case TBSAMSUBSTATE_LUNGEFORPLUNGE_FLYING:
			if( this->vel()[Y] < 0.0f )
			{
				this->setSubState(TBSAMSUBSTATE_LUNGEFORPLUNGE_FALLING);
			}
			break;

		case TBSAMSUBSTATE_LUNGEFORPLUNGE_FALLING:
			if( this->actorInstance->actorBehaviour->physics.coll )
			{
				this->setSubState(TBSAMSUBSTATE_LUNGEFORPLUNGE_LANDING);
			}
			break;

		case TBSAMSUBSTATE_LUNGEFORPLUNGE_LANDING:
			if( !this->playingAnim() )
				this->setState(TBSAMSTATE_USEPLUNGER,TBSAMSUBSTATE_USEPLUNGER_GETON);
			break;
		}

		break;


	/////////////////////////////////////////////////////////////////////////////////////
	case TBSAMSTATE_USEPLUNGER:
	/////////////////////////////////////////////////////////////////////////////////////

		switch( this->subState )
		{
		case TBSAMSUBSTATE_USEPLUNGER_GETON:
			if( !this->playingAnim() )
			{
				if( !this->seenCloseup )
				{
					this->setSubState(TBSAMSUBSTATE_USEPLUNGER_POSE);
					this->seenCloseup = true;
				}
				else
				{
					this->setSubState(TBSAMSUBSTATE_USEPLUNGER_JUMPLOOP);

					if (characterSounds.sam == -1)
					{
						if (bmRand()%2)
						{
							CallbackWhenSampleFinished((characterSounds.sam=PlaySample("samtazbossplunger_1.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.sam);	
						}
						else
						{
							CallbackWhenSampleFinished((characterSounds.sam=PlaySample("samtazbossplunger_1_alt_1.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.sam);	
						}
					}
				}

				tazBoss1Game.setCamType(TBCAM_DEFAULT);
			}
			break;

		case TBSAMSUBSTATE_USEPLUNGER_POSE:
			if( !this->playingAnim() )
			{
				this->setSubState(TBSAMSUBSTATE_USEPLUNGER_JUMPLOOP);

				if (characterSounds.sam == -1)
				{
					if (bmRand()%2)
					{
						CallbackWhenSampleFinished((characterSounds.sam=PlaySample("samtazbossplunger_1.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.sam);	
					}
					else
					{
						CallbackWhenSampleFinished((characterSounds.sam=PlaySample("samtazbossplunger_1_alt_1.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.sam);	
					}
				}
			}
			break;
		
		case TBSAMSUBSTATE_USEPLUNGER_JUMPLOOP:
			if( !this->playingAnim() )
			{
				if( this->jumps >= 3 )
					this->setSubState(TBSAMSUBSTATE_USEPLUNGER_ACTIVATION);
				else
					this->setSubState(TBSAMSUBSTATE_USEPLUNGER_JUMPLOOP);
			}
			break;

		case TBSAMSUBSTATE_USEPLUNGER_ACTIVATION:
			if( this->validAction )
			{
				this->validAction = false;

				samWinsPoint = true;
				tazBoss1Game.chilliBurnEnabled = false;// TP: no ass burn any more

				if (characterSounds.sam == -1)
					CallbackWhenSampleFinished((characterSounds.sam=PlaySample("samtazbossexplosion_1.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.sam);	

				// SO: Start explosions!
				this->lungePlunger->triggerExplosivePattern(TBExplosionPattern(bmRand()%4));
			}

			if( !this->playingAnim() )
				this->setState(TBSAMSTATE_RESPAWN);

			break;

		case TBSAMSUBSTATE_USEPLUNGER_ASSBURN:
			// SO: Add smoke to Sam
			DrawSmokeAtNodeInstance(this->actorInstance, "root", 0, SMOKE_SKIDSTOP);

			if( !this->playingAnim() )
				this->setState(TBSAMSTATE_RESPAWN);
			break;
		}

		break;


	/////////////////////////////////////////////////////////////////////////////////////
	case TBSAMSTATE_RESPAWN:
	/////////////////////////////////////////////////////////////////////////////////////

		switch( this->subState )
		{
		case TBSAMSUBSTATE_RESPAWN_WAIT:
			// SO: No respawning until plunger is inactive
			if( !this->lungePlunger || !this->lungePlunger->isActive() )
			{
				TBArenaArea area = tazBoss1Game.getArenaArea(&tbtaz, true);
				/*
				switch( area )
				{
					case TBARENA_AREA_UNKNOWN: bkPrintf("Unknown\n"); break;
					case TBARENA_AREA_TOP: bkPrintf("Top\n"); break;
					case TBARENA_AREA_LEFT: bkPrintf("Left\n"); break;
					case TBARENA_AREA_RIGHT: bkPrintf("Right\n"); break;
					case TBARENA_AREA_BOTTOM: bkPrintf("Bottom\n"); break;
					case TBARENA_AREA_CENTRE: bkPrintf("Centre\n"); break;
					case TBARENA_AREA_LAVA: bkPrintf("Lava\n"); break;
				}
				*/
				if( area == TBARENA_AREA_LEFT )
					this->setSubState(TBSAMSUBSTATE_RESPAWN_FALL_RIGHT);
				else if( area == TBARENA_AREA_RIGHT )
					this->setSubState(TBSAMSUBSTATE_RESPAWN_FALL_LEFT);
			}
			break;

		case TBSAMSUBSTATE_RESPAWN_FALLING:
			if( this->actorInstance->actorBehaviour->physics.coll )
				this->setSubState(TBSAMSUBSTATE_RESPAWN_LANDING);
			break;

		case TBSAMSUBSTATE_RESPAWN_LANDING:
			if( !this->playingAnim() )
				this->setSubState(TBSAMSUBSTATE_RESPAWN_POSE);
			break;

		case TBSAMSUBSTATE_RESPAWN_POSE:
			if( !this->playingAnim() )
			{
				if (tazBoss1Game.SamsScore()>=6)
				{
					ProcessTazDeathAtEndOfBossGame(ResetTazboss1Callback, NULL, camera[0].lookAt, FadeDownCallback);
					tazBoss1Game.continueChoice = BOSSGAMERESET_NONE;
					tazBoss1Game.scoreBook->close();

					this->setState(TBSAMSTATE_WAITFORNEXTROUND);
				}
				else
				{
					this->setState(TBSAMSTATE_RUNABOUT);
				}
			}
			break;
		}

		break;

	/////////////////////////////////////////////////////////////////////////////////////
	case TBSAMSTATE_WAITFORNEXTROUND:
	/////////////////////////////////////////////////////////////////////////////////////

		switch(tazBoss1Game.continueChoice)
		{
		case BOSSGAMERESET_NONE:
			break;

		case BOSSGAMERESET_CONTINUE:
			//TazBoss1Reset();
			//tazBoss1Game.scoreBook->open();
			bkGenerateEvent("scenechange","_reset");
			tazBoss1Game.continueChoice = BOSSGAMERESET_NONE;
			break;
	
		case BOSSGAMERESET_QUIT:
			camera[0].fade.FadeAndChangeScene(&gameStatus,FADETYPE_NORMAL,SCENE_TAZHUB,1.0f,bkCRC32((uchar*)"tazboss1door", strlen("tazboss1door"), 0));
			SetGameState(&gameStatus,GAMESTATE_FADING,GAMESUBSTATE_NORMAL);
			tazBoss1Game.continueChoice = BOSSGAMERESET_NONE;
			break;
		}
		break;

	/////////////////////////////////////////////////////////////////////////////////////
	case TBSAMSTATE_DEATH:
	/////////////////////////////////////////////////////////////////////////////////////

		switch( this->subState )
		{
		case TBSAMSUBSTATE_DEATH_DYING:
			if( !this->playingAnim() )
				this->setSubState(TBSAMSUBSTATE_DEATH_DEAD);
			break;
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: TBSAM::setState
	Purpose 	: set Sam's state
	Parameters 	: new state for Sam (TBSAMSTATE_)
	Returns 	: 
	Info 		: 
*/
void TBSAM::setState(const TBSamState stateIn, const TBSamSubState subStateIn)
{
	// SO: Set new state
	this->state = stateIn;

	switch( stateIn )
	{
	case TBSAMSTATE_RUNABOUT:
		tazBoss1Game.readyForChilli = TRUE;
		setSubState(TBSAMSUBSTATE_RUNABOUT_WAIT);
		this->timer = 0.0f;												// SO: Clear timer .. used to goto StandOff (if necessary)
		tazBoss1Game.setCamType(TBCAM_RUNABOUT);
		break;

	case TBSAMSTATE_STANDOFF:
		setSubState(TBSAMSUBSTATE_STANDOFF_SHOOTWALL);
		break;

	case TBSAMSTATE_ATTACKTAZ:
		setSubState(TBSAMSUBSTATE_ATTACKTAZ_DEFEND);
		break;

	case TBSAMSTATE_LUNGEFORPLUNGE:
		setSubState(subStateIn);
		break;

	case TBSAMSTATE_USEPLUNGER:
		setSubState(subStateIn);
		tazBoss1Game.chilliBurnEnabled = true;
		this->wasAssBurnt = FALSE;
		break;

	case TBSAMSTATE_RESPAWN:
		setSubState(TBSAMSUBSTATE_RESPAWN_WAIT);
		break;

	case TBSAMSTATE_DEATH:
		setSubState(TBSAMSUBSTATE_DEATH_DEAD);// TP: DYING
		break;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: TBSAM::setSubState
	Purpose 	: set Sam's sub-state
	Parameters 	: subStateIn - new sub-state for Sam
		          forceState - force this state (false means changes to the same state are ignored)
	Returns 	: 
	Info 		: 
*/
void TBSAM::setSubState(const TBSamSubState subStateIn, const char forceState)
{
	if( (this->subState == subStateIn) && !forceState )
		return;

	// SO: Set new sub state (Note: has to be called here due to recursive calls)
	this->subState = subStateIn;

	this->validAction = false;

	switch( subStateIn )
	{

	// TBSAMSTATE_RUNABOUT SubStates...

	case TBSAMSUBSTATE_RUNABOUT_WAIT:
		this->playAnim("idle1",true);
		break;

	case TBSAMSUBSTATE_RUNABOUT_LEFT:
		this->playAnim("runleft",true);
		break;

	case TBSAMSUBSTATE_RUNABOUT_RIGHT:
		this->playAnim("runright",true);
		break;


	// TBSAMSTATE_STANDOFF SubStates...

	case TBSAMSUBSTATE_STANDOFF_SHOOTWALL:
		this->playAnim("attack1",false);
		break;


	// TBSAMSTATE_ATTACKTAZ SubStates...

	case TBSAMSUBSTATE_ATTACKTAZ_DEFEND:
		this->playAnim("defend",true);
		break;

	case TBSAMSUBSTATE_ATTACKTAZ_ATTACK:
		this->playAnim("attack2",false);
		break;


	// TBSAMSTATE_LUNGEFORPLUNGE SubStates...

	case TBSAMSUBSTATE_LUNGEFORPLUNGE_1:
		this->lungePlunger = &tbplunger1;
		setSubState(TBSAMSUBSTATE_LUNGEFORPLUNGE_GOLAUNCH);

		if( !this->seenCloseup )
		{
			// SO: Set security camera
			TBVector pos = { TBSAM_PLUNGE_CAMPOS_X, TBSAM_PLUNGE_CAMPOS_Y,  TBSAM_PLUNGE_CAMPOS_Z, 1.0f };
			tazBoss1Game.setCamType(TBCAM_SECURITY_SAM,pos);
		}
		else
			tazBoss1Game.setCamType(TBCAM_DEFAULT);
		break;

	case TBSAMSUBSTATE_LUNGEFORPLUNGE_2:
		this->lungePlunger = &tbplunger2;
		setSubState(TBSAMSUBSTATE_LUNGEFORPLUNGE_GOLAUNCH);

		if( !this->seenCloseup )
		{
			// SO: Set security camera
			TBVector pos = {-TBSAM_PLUNGE_CAMPOS_X, TBSAM_PLUNGE_CAMPOS_Y, -TBSAM_PLUNGE_CAMPOS_Z, 1.0f };
			tazBoss1Game.setCamType(TBCAM_SECURITY_SAM,pos);
		}
		else
			tazBoss1Game.setCamType(TBCAM_DEFAULT);
		break;

	case TBSAMSUBSTATE_LUNGEFORPLUNGE_GOLAUNCH:
		// SO: Face launch vector and run there
		this->YrotateToFace(this->lungePlunger->launchVec);
		this->playAnim("run",true);
		break;

	case TBSAMSUBSTATE_LUNGEFORPLUNGE_FLYING:
		// TP: Make sure start position is exactly right
		bmVectorCopy(this->pos(), this->lungePlunger->launchVec);

		bkPrintf("Tried to launch!!!\n");

		// SO: Face land vector and jump there
		this->YrotateToFace(this->lungePlunger->landVec);
		this->playAnim("jumprise",false);

		// SO: Start jump
		this->actorInstance->actorAnimation->useMotionBone = FALSE;
		this->pos()[Y] += 1.0f;	// bodge to avoid sticking to ground

		{
			// SO: Calculate jump velocity
			float	dist, speed;
			float	sqrtInput;

			dist = this->dist(this->lungePlunger->landVec);
				
			// PP: make sure we don't pass a negative value into bmSqrt (fooks up on PC & Xbox)
			sqrtInput=dist * NORMAL_WORLD_GRAVITY;
			sqrtInput=bmFAbs(sqrtInput);

			speed = bmSqrt(sqrtInput);		// SO: This relies on a QUARTPI takeoff angle (2*sin:QUARTPI*cos:QUARTPI = 1)

			TBVector vel={0.f,0.f,1.f,1.f};
			bmVectorRotateXY(vel,vel,-QUARTPI,this->getYang());
			bmVectorNorm(vel,vel);
			bmVectorCMul(this->vel(),vel,speed);
			this->actorInstance->actorBehaviour->physics.speed = bmVectorLen(this->vel());
		}
		break;

	case TBSAMSUBSTATE_LUNGEFORPLUNGE_FALLING:
		this->playAnim("jumpfall",true);
		break;

	case TBSAMSUBSTATE_LUNGEFORPLUNGE_LANDING:
		this->actorInstance->actorAnimation->useMotionBone = TRUE;
		bmVectorCopy(this->pos(),this->lungePlunger->landVec);
		this->YrotateToFace(this->lungePlunger->pos());
		this->playAnim("jumpland",false);
		break;


	// TBSAMSTATE_USEPLUNGER SubStates...

	case TBSAMSUBSTATE_USEPLUNGER_GETON:
		// SO: Play anims for Sam
		this->playAnim("getonplunger",false);
		this->jumps = 0;
		break;

	case TBSAMSUBSTATE_USEPLUNGER_POSE:
		// SO: Play anims for Sam
		this->playAnim("idleonplunger",false);
		break;

	case TBSAMSUBSTATE_USEPLUNGER_JUMPLOOP:
		// SO: Play anims for Sam and the plunger
		this->playAnim("jumponplunger",false);
		this->lungePlunger->playAnim("jumponplunger",false);
		this->jumps++;
		break;

	case TBSAMSUBSTATE_USEPLUNGER_ACTIVATION:
		// SO: Play anims for Sam and the plunger
		this->playAnim("pushplunger",false);
		this->lungePlunger->playAnim("pushplunger",false);
		break;

	case TBSAMSUBSTATE_USEPLUNGER_ASSBURN:
		// SO: Play anims for Sam and the plunger
		FlushAnimationQueue(this->actorInstance);
		FlushAnimationQueue(this->lungePlunger->actorInstance);

		this->playAnim("chilliburns",false);
		this->lungePlunger->playAnim("chilliburns",false);

		// SO: Stop Taz's chilli mouth
		tbtaz.stopChilliBreath();

		if (bmRand()%2)
			CallbackWhenSampleFinished((characterSounds.sam=PlaySample("samtazbossburnt_1.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.sam);	
		else
			CallbackWhenSampleFinished((characterSounds.sam=PlaySample("samtazbossburnt_2.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.sam);	


		// SO: Finish off
		this->wasAssBurnt = true;

		{
			// SO: Set fixed camera
			TBVector pos;
			TBVector ref;
			if( this->lungePlunger == &tbplunger1 )
				bmVectorSet(pos, TBSAM_BURNS_CAMPOS_X, TBSAM_BURNS_CAMPOS_Y, TBSAM_BURNS_CAMPOS_Z, 1.0f);
			else
				bmVectorSet(pos, -TBSAM_BURNS_CAMPOS_X, TBSAM_BURNS_CAMPOS_Y, -TBSAM_BURNS_CAMPOS_Z, 1.0f);
 			baGetNodesWorldPosition(&this->actorInstance->actorInstance,this->bodyNode,ref);
			tazBoss1Game.setCamType(TBCAM_FIXED,pos,ref);
		}
		break;


	// TBSAMSTATE_RESPAWN SubStates...

	case TBSAMSUBSTATE_RESPAWN_WAIT:
		tazBoss1Game.setCamType(TBCAM_DEFAULT);
		// SO: While respawning, hide the chilli pepper and stop Taz's chilli breath
		ClearCollectibles();
		break;

	case TBSAMSUBSTATE_RESPAWN_FALL_LEFT:
		{
			// SO: Make sam fall on the left side of the arena
			float angle = 0.0f;
			bmVectorSet(this->pos(), TBARENA_PLATFORM_MIDDLE*bmSin(angle), TBSAM_RESPAWN_HEIGHT, TBARENA_PLATFORM_MIDDLE*bmCos(angle), 1.0f );
			this->setSubState(TBSAMSUBSTATE_RESPAWN_FALLING);

			// SO: Set fixed camera
			TBVector pos = {TBSAM_SPAWN_CAMPOS_X, TBSAM_SPAWN_CAMPOS_Y, TBSAM_SPAWN_CAMPOS_Z, 1.0f};
			TBVector ref = {this->pos()[X], TBARENA_PLATFORM_YPOS+METERTOUNIT(1.0f), this->pos()[Z], 1.0f};
			tazBoss1Game.setCamType(TBCAM_FIXED,pos,ref);
		}
		break;

	case TBSAMSUBSTATE_RESPAWN_FALL_RIGHT:
		{
			// SO: Make sam fall on the right side of the arena
			float angle = PI;
			bmVectorSet(this->pos(), TBARENA_PLATFORM_MIDDLE*bmSin(angle), TBSAM_RESPAWN_HEIGHT, TBARENA_PLATFORM_MIDDLE*bmCos(angle), 1.0f );
			this->setSubState(TBSAMSUBSTATE_RESPAWN_FALLING);

			// SO: Set fixed camera
			TBVector pos = {-TBSAM_SPAWN_CAMPOS_X, TBSAM_SPAWN_CAMPOS_Y, -TBSAM_SPAWN_CAMPOS_Z, 1.0f};
			TBVector ref = {this->pos()[X], TBARENA_PLATFORM_YPOS+METERTOUNIT(1.0f), this->pos()[Z], 1.0f};
			tazBoss1Game.setCamType(TBCAM_FIXED,pos,ref);
		}
		break;

	case TBSAMSUBSTATE_RESPAWN_FALLING:
		// SO: Respawning works by choosing a position to respawn and falling there
		this->playAnim("jumpfall",true);
		this->YrotateToFace(ArenaCentre);
		break;

	case TBSAMSUBSTATE_RESPAWN_LANDING:
		this->playAnim("jumpland",false);
		break;

	case TBSAMSUBSTATE_RESPAWN_POSE:

		// TP: if Sam caused an explosion then add point
		if (samWinsPoint)
		{
			tazBoss1Game.SamWinsAPoint();
			samWinsPoint = false;
		}

		if( this->wasAssBurnt )
		{
			this->totalAssBurns++;
			this->wasAssBurnt = false;
			// TP: Sam lost last go so play a defeat anim
			if( bmRandf() > 0.5f )
				this->playAnim("idle3",false);
			else
				this->playAnim("idle4",false);

			if (characterSounds.sam == -1)
			{
				if (bmRand()%2)
					CallbackWhenSampleFinished((characterSounds.sam=PlaySample("samtazbosshit_1.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.sam);	
				else
					CallbackWhenSampleFinished((characterSounds.sam=PlaySample("samtazbosshit_3.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.sam);	
			}
		}
		else
		{
			this->playAnim("idle2",false);

			if (bmRand()%2)
				CallbackWhenSampleFinished((characterSounds.sam=PlaySample("samtazbossgen_1.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.sam);	
			else
				CallbackWhenSampleFinished((characterSounds.sam=PlaySample("samtazbossgen_2.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.sam);	
		}

		// SO: While respawning, hide the chilli pepper and stop Taz's chilli breath
		tbtaz.stopChilliBreath();
		break;


	// TBSAMSTATE_DEATH SubStates...

	case TBSAMSUBSTATE_DEATH_DYING:

		// SO: Play anims for Sam and the plunger
		FlushAnimationQueue(this->actorInstance);
		FlushAnimationQueue(this->lungePlunger->actorInstance);

		this->playAnim("chillideath",false);
		this->lungePlunger->playAnim("chillideath",false);

		// SO: Stop Taz's chilli mouth
		tbtaz.stopChilliBreath();

		{
			// SO: Set fixed camera
			TBVector pos;
			if( this->lungePlunger == &tbplunger1 )
				bmVectorSet(pos, TBSAM_BURNS_CAMPOS_X, TBSAM_BURNS_CAMPOS_Y, TBSAM_BURNS_CAMPOS_Z, 1.0f);
			else
				bmVectorSet(pos, -TBSAM_BURNS_CAMPOS_X, TBSAM_BURNS_CAMPOS_Y, -TBSAM_BURNS_CAMPOS_Z, 1.0f);
			tazBoss1Game.setCamType(TBCAM_SECURITY_SAM,pos);
		}
		break;

	case TBSAMSUBSTATE_DEATH_DEAD:
		// SO: GOTO NEXT LEVEL
		tazBoss1Game.DeleteScoreBook();

		actorInstance->flags |= ACTORFLAG_DONTDRAW;

		LevelCompleted(gameStatus.player1,LEVELCOMPLETE_NOSTATS);
		break;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: TBSAM::burnMyAss
	Purpose 	: Burn Sam's ass
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TBSAM::burnMyAss()
{
	if( this->totalAssBurns >= (GET_DIFFICULTY_VALUE(ASS_BURNS_B4_DEATH_EASY,ASS_BURNS_B4_DEATH_MEDIUM,ASS_BURNS_B4_DEATH_HARD)-1) )
	{
		this->setState(TBSAMSTATE_DEATH);
	}
	else
	{
		this->setSubState(TBSAMSUBSTATE_USEPLUNGER_ASSBURN,false);
	}
}



/* SO: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												TBTAZ methods
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*	--------------------------------------------------------------------------------
	Function 	: TBTAZ::init
	Purpose 	: initialisation
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TBTAZ::init(void)
{
	this->player = &(gameStatus.player[0]);
	this->actorInstance = gameStatus.player[0].actorInstance;
	bmVectorSet(this->pos(), TBTAZ_SPAWN_POSITION );

	bmVectorZero(this->velocityBeforeSuspend);
	this->suspended = FALSE;
}


/*	--------------------------------------------------------------------------------
	Function 	: TBTAZ::stopChilliBreath
	Purpose 	: Cancels the chilli breath powerup
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TBTAZ::stopChilliBreath(void)
{
	if(this->actorInstance->characterInfo->powerUpInfo.chilliPepper)	FinishChilliPepper(this->actorInstance);
}


/*	--------------------------------------------------------------------------------
	Function 	: TBTAZ::setSuspendedState
	Purpose 	: Set whether Taz is suspended or not
	Parameters 	: suspend yes/no
	Returns 	: 
	Info 		: 
*/
void TBTAZ::setSuspendedState(char suspend)
{
	if( suspend == this->suspended )
		return;
	
	if( suspend )
	{
		// SO: Suspend controller
		gameStatus.controlSuspend = TRUE;

		// SO: We need to 1) Stop gravity 2) Stop velocity and 3) Clear velocity
		this->actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_NOGRAVITY;
		this->actorInstance->flags |= ACTORFLAG_NOVELOCITY;
		bmVectorCopy(this->velocityBeforeSuspend,this->actorInstance->actorBehaviour->physics.velocity);
		bmVectorZero(this->actorInstance->actorBehaviour->physics.velocity);
	}
	else
	{
		// SO: Un-suspend controller
		gameStatus.controlSuspend = FALSE;

		// SO: Re-enable gravity and velocity
		this->actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_NOGRAVITY;
		this->actorInstance->flags &= ~ACTORFLAG_NOVELOCITY;
		bmVectorCopy(this->actorInstance->actorBehaviour->physics.velocity,this->velocityBeforeSuspend);

		// TP: if(( this->actorInstance->actorStatus->currentState == STATE_BURNT )|| (this->actorInstance->actorStatus->currentState == STATE_BURNT == STATE_JUMP))
		{
			// SO: If Taz is jumping and not on either of the main platforms, he should be replaced to one of them
			TBArenaArea area = tazBoss1Game.getArenaArea(this);

			float radius = this->XZdist(ArenaCentre);

			if(radius > TBARENA_CENTRE_RADIUS)
			{
				// SO: Get current angle and set current radius
				float angle = tazBoss1Game.getArenaAngle(this);

				// SO: Reset radius to be on platform
				if( radius < TBARENA_PLATFORM_MIN )
					radius = TBARENA_PLATFORM_MIN;
				else
					radius = TBARENA_PLATFORM_MAX - METERTOUNIT(2.5f);

				// SO: Place Taz
				bmVectorSet(this->pos(), radius*bmSin(angle), TBARENA_PLATFORM_YPOS, radius*bmCos(angle), 1.0f);
			}
		}
	}

	this->suspended = suspend;
}




/* SO: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												TAZBOSS1GAME methods

											Tasmania Boss I - Tycoon Sam!
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*	--------------------------------------------------------------------------------
	Function 	: TAZBOSS1GAME::init
	Purpose 	: initialise the Boss game
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TAZBOSS1GAME::init(void)
{
	BOSSGAME::init();

	// PP: force draw-order-sorting on fireballs (sorts according to depth and activity, works a treat :)
	Fireball_applyDrawOrder=true;

	// SO: Initialise Sam n' Max .. err I mean Sam n' Taz
	this->taz.init();
	this->sam.init();

	// SO: Initialise boss game camera case
	setCamType(TBCAM_RUNABOUT);

	// SO: Load actor resources
	LoadActor(TBPLUNGER_MODEL,map.mapName);
	LoadActor(TBROCK_MODEL,map.mapName);
	LoadActor(TBSPARK_MODEL,map.mapName);

	// SO: Initialise plungers
	TBVector actualPos, launchPos, landPos;

	// SO: Setup plunger 1
	bmVectorSet(actualPos, TBARENA_PLUNGER1_POS);
	bmVectorSet(launchPos,TBARENA_LAUNCH1_SAMPOS);
	bmVectorSet(landPos,TBARENA_PLUNGER1_SAMPOS);
	this->plunger1.init(0,actualPos, launchPos, landPos, 0.0f);

	// SO: Setup plunger 2
	bmVectorSet(actualPos, TBARENA_PLUNGER2_POS);
	bmVectorSet(launchPos,TBARENA_LAUNCH2_SAMPOS);
	bmVectorSet(landPos,TBARENA_PLUNGER2_SAMPOS);
	this->plunger2.init(1,actualPos, launchPos, landPos, PI);

	// SO: Initialise all other variables
	this->readyForChilli = TRUE;
	numExplosionsTriggered = 0;
	continueChoice = BOSSGAMERESET_NONE;

	ClearCollectibles();
	CreateScoreBook();
}


/*	--------------------------------------------------------------------------------
	Function 	: TAZBOSS1GAME::shutdown
	Purpose 	: shut-down the Tycoon Sam Boss game
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides BOSSGAME::shutdown
*/
void TAZBOSS1GAME::shutdown(void)
{
	// SO: do base-class shutdown
	BOSSGAME::shutdown();

	DeleteScoreBook();

	// PP: set the fireball draw-order-sorting flag back to what it is normally
	Fireball_applyDrawOrder=FB_NORMALLY_SORT_DRAW_ORDER;

	this->taz.shutdown();
	this->sam.shutdown();

	this->plunger1.shutdown();
	this->plunger2.shutdown();

	FreeActor(TBPLUNGER_MODEL);
	FreeActor(TBROCK_MODEL);
	FreeActor(TBSPARK_MODEL);
}


/*	--------------------------------------------------------------------------------
	Function 	: TAZBOSS1GAME::update
	Purpose 	: update the Tycoon Sam Boss game
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides BOSSGAME::update
*/
//#define SCOTT_DEBUG 1
//#include "control.h"

void TAZBOSS1GAME::update(void)
{
	BG_START_UPDATE;

	// SO: Do base-class update
	BOSSGAME::update();

	// SO: Update Sam n' Taz
	this->sam.update();
	this->taz.update();

	if (scoreBook)
	{
		tazScore->sprintf8("%d/6", sam.totalAssBurns);
		samScore->sprintf8("%d/6", numExplosionsTriggered);
	}

	// SO: Update plungers
	this->plunger1.update();
	this->plunger2.update();

	// SO: Update camera
	this->updateCamera();

	if (!gameStatus.player1->characterInfo->powerUpInfo.chilliPepper && this->readyForChilli == FALSE)
		this->readyForChilli = TRUE;

#ifdef SCOTT_DEBUG
	/////////////////////////////////////////////////////////
	// SO: DEBUG CONTROL ... ALL TEMPORARY

	static int xxLookingAtSam=0;
	static int xxSamState=0;

	static int xxPlunger = 0;
	static int xxPattern = 0;

	// SO: R2 - Hold sam's where he is .. preserve AI state
	if(controller1.r2DebounceChannel->value != 0)
	{
		if( !xxSamState )
		{
			xxSamState = tbsam.state;
			tbsam.state = TBSAMSTATE_NOSTATE;
		}
		else
		{
			tbsam.state = (TBSamState)xxSamState;
			xxSamState = 0;
		}
	}

	// SO: R3 - Respawn Sam
	if(controller1.r3DebounceChannel->value != 0)
		this->sam.setState(TBSAMSTATE_RESPAWN);

	// SO: L3 - Switch focus to go between sam and taz
	if(controller1.l3DebounceChannel->value != 0)
	{
		if( xxLookingAtSam )
			SetCameraLookAtFollowActorInstance(&camera[0], this->taz.actorInstance, &(this->taz.actorInstance->actorInstance), (TBActorNodeInstance *)NULL);
		else
		{
			TBActorNodeInstance *node = baFindNode(this->sam.actorInstance->actorInstance.rootNodeInstance,"neck");
			SetCameraLookAtFollowActorInstance(&camera[0], this->sam.actorInstance, &(this->sam.actorInstance->actorInstance), node);
		}
	
		xxLookingAtSam = !xxLookingAtSam;
	}

	// SO: L2 - Give chilli breath
	if(controller1.l2DebounceChannel->value != 0)
	{
		CHARACTERINFO *charInfo = tbtaz.actorInstance->characterInfo;

		charInfo->powerUpInfo.clock = 2.0f; //CHILLIPEPPER_EASY*2.0f;
		charInfo->powerUpInfo.state = POWERUPSTATE_CHILLIOFF;
		charInfo->powerUpInfo.chilliPepper = TRUE;
		charInfo->powerUpInfo.numFire = CHILLIPEPPER_FIRE_NUM;
		RequestStateChange(tbtaz.actorInstance, STATE_CHILLIPEPPER);

		if( !xxPlunger )
			this->plunger1.triggerExplosivePattern(TBExplosionPattern(xxPattern));
		else
			this->plunger2.triggerExplosivePattern(TBExplosionPattern(xxPattern));
	}

	/////////////////////////////////////////////////////////
#endif

}


/*	--------------------------------------------------------------------------------
	Function 	: TAZBOSS1GAME::draw
	Purpose 	: draw the Tycoon Sam Boss game solid stuff
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides BOSSGAME::draw
*/
void TAZBOSS1GAME::draw(void)
{
	if(!this->initialised)// PP: 
	{
		return;
	}

	// SO: do base class solid draw
	BOSSGAME::draw();

	// SO: Draw plungers (for their fuses)
	this->plunger1.draw();
	this->plunger2.draw();
/* PP: REMOUT: Now handled by the Book List		
	bdSetIdentityObjectMatrix();
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);			// PP: this should anti-alias the text in the book
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	if (popup) popup->draw();
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);*/
}


/*	--------------------------------------------------------------------------------
	Function 	: TAZBOSS1GAME::draw2
	Purpose 	: draw the Tycoon Sam Boss game alpha stuff
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides BOSSGAME::draw
*/
void TAZBOSS1GAME::draw2(void)
{
	if(!this->initialised)// PP: 
	{
		return;
	}

	// SO: do base class alpha draw
	BOSSGAME::draw2();
}


/*	--------------------------------------------------------------------------------
	Function 	: TAZBOSS1GAME::setCamType
	Purpose 	: set the camera type
	Parameters 	: camera type, pos (security/fixed only), ref (fixed only)
	Returns 	: 
	Info 		: 
*/
void TAZBOSS1GAME::setCamType(const TBCamType camTypeIn, TBVector pos, TBVector ref)
{
	// SO: Initialise new camera...
	switch( camTypeIn )
	{
	case TBCAM_RUNABOUT:
		// SO: Normal runabout camera with lower FOV
		setFOV(0.9f);

		this->taz.setSuspendedState(FALSE);
		break;

	case TBCAM_SECURITY_SAM:
		// SO: Security cam (ie. fixed pos - watch sam)
		if( !pos )	{ bkPrintf("TBCAM_SECURITY_SAM - no pos\n"); return; }
		bmVectorCopy(this->camPos,pos);
		setFOV(NORMAL_INGAME_FOV);

		this->taz.setSuspendedState(TRUE);
		//this->camType = TBCAM_DEFAULT;	// SO: DEBUG ONLY
		break;

	case TBCAM_FIXED:
		// SO: Totally fixed camera
		if( !pos )	{ bkPrintf("TBCAM_FIXED - no pos\n"); return; }
		if( !ref )	{ bkPrintf("TBCAM_FIXED - no ref\n"); return; }
		bmVectorCopy(this->camPos,pos);
		bmVectorCopy(this->camRef,ref);
		setFOV(NORMAL_INGAME_FOV);

		this->taz.setSuspendedState(TRUE);
		break;

	default:
		// SO: Set default FOV
		setFOV(NORMAL_INGAME_FOV);

		this->taz.setSuspendedState(FALSE);
		break;
	}


	// SO: Old camera type clean-up...
	switch( this->camType )
	{
	case TBCAM_SECURITY_SAM:
	case TBCAM_FIXED:
		// SO: Bring camera back behind Taz, with focus on Sam.
		// SO: This is so when normal play resumes the player is not
		// SO: disorientated and doesn't need to move the camera.
		this->camType = TBCAM_RUNABOUT;
		this->updateCamera();
		bmVectorCopy(camera[0].lookAt,camera[0].destinationLookAt);
		bmVectorCopy(camera[0].pos,camera[0].destinationPos);
		break;
	}


	// SO: Set new camera type...
	this->camType = camTypeIn;
}


/*	--------------------------------------------------------------------------------
	Function 	: TAZBOSS1GAME::updateCamera
	Purpose 	: update the camera
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
#define RUNABOUT_CAM_MAXDIST	METERTOUNIT(24.0f)
void TAZBOSS1GAME::updateCamera(void)
{
	TBVector pos, tempVector;
	float dist, offset;

	switch( this->camType )
	{
	case TBCAM_RUNABOUT:
		bmVectorSub(tempVector, this->taz.pos(), ArenaCentre);
		if (bmVectorLen(tempVector) > (TBARENA_PLATFORM_MAX-METERTOUNIT(0.5f)))
		{
			// TP: special cam for taz in outer rim
		}
		else
		{
			// SO: Set position
			bmVectorSub(pos, this->taz.pos(), this->sam.pos());
			pos[Y] = 0.0f;

			dist = bmVectorLen(pos);
			offset = (dist/RUNABOUT_CAM_MAXDIST);

			bmVectorNorm(pos,pos);								// SO: Calculate distance between sam n' taz, and add a bit
			bmVectorCMul(pos,pos,dist+METERTOUNIT(3.25f));
			bmVectorAdd(pos,this->sam.pos(),pos);
			pos[Y] += METERTOUNIT(1.0f) + (METERTOUNIT(2.0f) * offset);
			SetCameraPositionDestination(&camera[0], pos);

			// SO: Set look at
			// SO: Note: [Y] pos lowered so player can see more of the lava'ry area
			bmVectorSet(pos, this->sam.pos()[X], this->sam.pos()[Y] - (METERTOUNIT(4.0f)*offset), this->sam.pos()[Z], 1.0f);
			SetCameraLookAtDestination(&camera[0], pos);
		}
		break;

	case TBCAM_SECURITY_SAM:
		// SO: Set look at
		baGetNodesWorldPosition(&tbsam.actorInstance->actorInstance,tbsam.bodyNode,pos);
		SetCameraLookAtDestination(&camera[0], pos);
		// SO: Set camera pos
		SetCameraPosition(&camera[0], this->camPos);
		break;

	case TBCAM_FIXED:
		// SO: Set camera pos/ref
		SetCameraPosition(&camera[0], this->camPos);
		SetCameraLookAtPosition(&camera[0], this->camRef);
		break;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: TAZBOSS1GAME::doChilliPepperFireCollision
	Purpose 	: handles chilli pepper collisions
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TAZBOSS1GAME::doChilliPepperFireCollision(CHILLIPEPPEREXTRAS *data)
{
	if( tbsam.state != TBSAMSTATE_USEPLUNGER )		// SO: If he ain't using the plunger, I ain't interested
		return;

	// SO: Chilli flame partical must be within .35 meters to burn Sam
	if( (bmVectorDistance(data->position,tbsam.pos()) < METERTOUNIT(0.5f))&& (chilliBurnEnabled))
	{
		tbsam.burnMyAss();
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: TAZBOSS1GAME::readyForChilliRespawn
	Purpose 	: Returns if we're ready for a new chillipepper
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
int TAZBOSS1GAME::readyForChilliRespawn(void)
{
	if( this->readyForChilli )
	{
		// SO: This ensures the chilli is only respawned when required.
		this->readyForChilli = FALSE;
		return 1;
	}

	return 0;
}


/*	--------------------------------------------------------------------------------
	Function 	: TAZBOSS1GAME::respondToEvent
	Purpose 	: respond to an animation event
	Parameters 	: event type
	Returns 	: 
	Info 		: 
*/
void TAZBOSS1GAME::respondToEvent(TBEventType eventType)
{
	// SO: Just set validaction .. I know what it refers to because of Sam's state
	sam.validAction = true;
}


/*	--------------------------------------------------------------------------------
	Function 	: TAZBOSS1GAME::getArenaAngle
	Purpose 	: Gets angle around the area (signed)
	Parameters 	: Who
	Returns 	: Angle
	Info 		: 
*/
float TAZBOSS1GAME::getArenaAngle(ACTORINSTANCEUSER *user)
{
	return bmATan2(user->pos()[X], user->pos()[Z]);
}


/*	--------------------------------------------------------------------------------
	Function 	: TAZBOSS1GAME::getArenaArea
	Purpose 	: Returns where Taz currently is
	Parameters 	: Who
	Returns 	: TBArenaArea
	Info 		: 
*/
#define ARENA_STANDOFF_ANGLE_SIZE (PI*0.3f)
#define ARENA_STANDOFF_ANGLE_SIZE_SIDE (PI*0.5f)
TBArenaArea TAZBOSS1GAME::getArenaArea(ACTORINSTANCEUSER *user, bool sideOnly)
{
	float radius = bmSqrt(user->pos()[X]*user->pos()[X] + user->pos()[Z]*user->pos()[Z]);

	if (sideOnly)
	{
		float angle = this->getArenaAngle(user);

		if( angle < 0.0f )
		{
			if( angle > -ARENA_STANDOFF_ANGLE_SIZE_SIDE )
			{
				return TBARENA_AREA_LEFT;
			}
			else 
			{
				return TBARENA_AREA_RIGHT;
			}
		}
		else
		{
			if( angle < ARENA_STANDOFF_ANGLE_SIZE_SIDE )
			{
				return TBARENA_AREA_LEFT;
			}
			else 
			{
				return TBARENA_AREA_RIGHT;
			}
		}
	}
	else
	{
		if( radius < TBARENA_CENTRE_RADIUS )
			return TBARENA_AREA_CENTRE;
		if( radius < TBARENA_PLATFORM_MIN )
			return TBARENA_AREA_LAVA;
		else if( radius > TBARENA_PLATFORM_MAX )
			return TBARENA_AREA_UNKNOWN;
		else
		{
			float angle = this->getArenaAngle(user);

			if( angle < 0.0f )
			{
				if( angle > -ARENA_STANDOFF_ANGLE_SIZE )
					return TBARENA_AREA_LEFT;
				else if( angle < -(PI-ARENA_STANDOFF_ANGLE_SIZE) )
					return TBARENA_AREA_RIGHT;
				else
					return TBARENA_AREA_BOTTOM;
			}
			else
			{
				if( angle < ARENA_STANDOFF_ANGLE_SIZE )
					return TBARENA_AREA_LEFT;
				else if( angle > (PI-ARENA_STANDOFF_ANGLE_SIZE) )
					return TBARENA_AREA_RIGHT;
				else
					return TBARENA_AREA_TOP;
			}
		}
	}
}

/*  --------------------------------------------------------------------------------
	Function	: TAZBOSS1GAME::CreateScoreBook
	Purpose		: Setup up the score book for the game
	Parameters	: 
	Returns		: 
	Info		: 
*/

void TAZBOSS1GAME::CreateScoreBook(void)
{
	PAGE	*pPage;
	ICON	*pIcon;

	scoreBook = new BOOK();	// TP: Strange problems when this is BOOK(true)
	
	pPage = scoreBook->addPage(pIcon = new ICON("extras\\tazatlas.bmp", "TAZ"));
	ASSERT(pPage);

	pIcon->setXAlign(PIFLAG_XALIGN_LEFT);
	pIcon->setYAlign(PIFLAG_YALIGN_TOP);
	pIcon->setXPadding(false);

	pPage->insertItem(tazScore = new TEXTBOX(),false);// PP: false = same row
	tazScore->setXAlign(PIFLAG_XALIGN_LEFT);
	tazScore->setYAlign(PIFLAG_YALIGN_TOP);
	tazScore->setXPadding(true);
	tazScore->setScale(2.5f);
	tazScore->setColour(COLOUR(0, 96, 0), COLOUR(0, 128, 0));
	tazScore->sprintf8("%d/6", sam.totalAssBurns);

	pPage->insertItem(samScore = new TEXTBOX(), false);
	samScore->setXAlign(PIFLAG_XALIGN_RIGHT);
	samScore->setYAlign(PIFLAG_YALIGN_TOP);
	samScore->setScale(2.5f);
	samScore->setColour(COLOUR(96, 0, 0), COLOUR(128, 0, 0));
	samScore->sprintf8("%d/6", numExplosionsTriggered);

	pPage->insertItem(pIcon = new ICON("extras\\sambomberman.bmp", "SAM"), false);
	pIcon->setXPadding(false);
	pIcon->setXAlign(PIFLAG_XALIGN_RIGHT);
	pIcon->setYAlign(PIFLAG_YALIGN_TOP);

	scoreBook->open();
}

/*  --------------------------------------------------------------------------------
	Function	: TAZBOSS1GAME::DeleteScoreBook
	Purpose		: Close and remove the score book
	Parameters	: 
	Returns		: 
	Info		: 
*/

void TAZBOSS1GAME::DeleteScoreBook(void)
{
	SAFE_DELETE(scoreBook);
}