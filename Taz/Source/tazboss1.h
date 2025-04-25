#ifndef __H_TAZBOSS1
#define __H_TAZBOSS1

#include "boss.h"
#include "debris.h"
#include "paths.h"
#include "chillipepper.h"
#include "fireball.h"
#include "popup.h"
#include "bossgamecontinue.h"



/* SO: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												TBSPARK
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
typedef struct TAG_TBSPARK : public ACTORINSTANCEUSER
{
	/*	--------------------------------------------------------------------------------
		Function 	: TBSPARK::init
		Purpose 	: initialisation
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void init(void);

}TBSPARK, _TBSPARK BALIGN16;


/* SO: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
											TBFUSE_PATH
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
typedef struct TAG_TBFUSE_PATH
{
	TBSPARK		spark;				// SO: Spark actor
	TBVector	explosionPoint;		// SO: Point of explosion
	PATH		*fusePath;			// SO: Path of spark

	int		index;					// SO: Index number of path
	float	time;					// SO: time < 0.0 is inactive, else time is 0..1
	float	maxTime;				// SO: Total amount of time to take (in seconds)
	float	pathLength;				// SO: Length of path
	int		col[4];					// SO: Colour of fuse
	int		lastSparkPoint;			// SO: Last point the spark passed (for drawing)

	/*	--------------------------------------------------------------------------------
		Function 	: TBFUSE_PATH::init
		Purpose 	: initialisation
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void init(int iPlunger, int iSide, int iPath);

	/*	--------------------------------------------------------------------------------
		Function 	: TBFUSE_PATH::startFuse
		Purpose 	: Starts the fuse going
		Parameters 	: secs - number of seconds to take (default = 1.0)
		Returns 	: 
		Info 		: 
	*/
	void startFuse(float secs=1.0f);

	/*	--------------------------------------------------------------------------------
		Function 	: TBFUSE_PATH::addExplosion
		Purpose 	: Adds an explosion
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void addExplosion(void);

	/*	--------------------------------------------------------------------------------
		Function 	: TBFUSE_PATH::addFireball
		Purpose 	: Adds a fireball
		Parameters 	: angle = direction of fireball, range = distance
		Returns 	: 
		Info 		: 
	*/
	void addFireball(float angle, float range, float xOff=0.0f, float yOff=0.0f, float zOff=0.0f);

	/*	--------------------------------------------------------------------------------
		Function 	: TBFUSE_PATH::fireballCallback
		Purpose 	: Callback during fireball update
		Parameters 	: Fireball and context
		Returns 	: 
		Info 		: 
	*/
	static void fireballCallback(FIREBALL* const fireball, const long32 context);

	/*	--------------------------------------------------------------------------------
		Function 	: TBFUSE_PATH::update
		Purpose 	: Updates the fuse
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void update(void);

	/*	--------------------------------------------------------------------------------
		Function 	: TBFUSE_PATH::draw
		Purpose 	: Draws the fuse
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void draw(void);

}TBFUSE_PATH, _TBFUSE_PATH BALIGN16;


/* SO: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
											  TBPLUNGER
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

enum TBExplosionPattern
{
	TBPATTERN_MIDDLE,		// SO: Only the 2 middle fuses
	TBPATTERN_SIDEWAVE,		// SO: Timed so one side will explode in a wave
	TBPATTERN_TBM,			// SO: Top/Bottom/Middle .. First the top set goes, then the bottom, finally the middle.
	TBPATTERN_FULLON,		// SO: All explosives go off
};

#define NUM_PLUNGERS	2
#define NUM_FUSE_SIDES	2
#define NUM_FUSE_PATHS	3

typedef struct TAG_TBPLUNGER : public ACTORINSTANCEUSER
{
	TBVector	launchVec;			// SO: Vector of launch position
	TBVector	landVec;			// SO: Vector of land (and operate) position

	TBFUSE_PATH	fusePaths[NUM_FUSE_SIDES][NUM_FUSE_PATHS];	// SO: Fuse paths running from the plunger to some TNT

	int		index;					// SO: Index number of plunger

	/*	--------------------------------------------------------------------------------
		Function 	: TBPLUNGER::init
		Purpose 	: initialisation
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void init(int iPlunger, TBVector actualPos, TBVector launchPos, TBVector landPos, float yRotation);

	/*	--------------------------------------------------------------------------------
		Function 	: TBPLUNGER::update
		Purpose 	: update routine
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void update(void);

	/*	--------------------------------------------------------------------------------
		Function 	: TBPLUNGER::draw
		Purpose 	: draw routine
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void draw(void);

	/*	--------------------------------------------------------------------------------
		Function 	: TBPLUNGER::triggerExplosivePattern
		Purpose 	: Triggers an explosion of unimaginable destruction
		Parameters 	: pattern - which fuses to trigger
		Returns 	: 
		Info 		: 
	*/
	void triggerExplosivePattern(TBExplosionPattern pattern);

	/*	--------------------------------------------------------------------------------
		Function 	: TBPLUNGER::isActive
		Purpose 	: Test if this plunger has active fuses
		Parameters 	: 
		Returns 	: If fuses are active
		Info 		: 
	*/
	int isActive(void);

}TBPLUNGER, _TBPLUNGER BALIGN16;




/* SO: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												TBSAM
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
enum TBSamState
{
	TBSAMSTATE_NOSTATE,
	TBSAMSTATE_RUNABOUT,			// SO: Standard state .. Sam is mirroring Taz
	TBSAMSTATE_STANDOFF,			// SO: Standoff - Sam needs to shoot summit!
	TBSAMSTATE_ATTACKTAZ,			// SO: Taz is close
	TBSAMSTATE_LUNGEFORPLUNGE,		// SO: Make a dive for the plunger
	TBSAMSTATE_USEPLUNGER,			// SO: Get on and use plunger
	TBSAMSTATE_RESPAWN,				// SO: Respawn Sam (called after plunger activated and after chilli burns)
	TBSAMSTATE_DEATH,				// SO: Occurs when Sam has had too much of Taz's firey breath
	TBSAMSTATE_WAITFORNEXTROUND,	// TP: Wait for the player to choose retry or not
};

enum TBSamSubState
{
	TBSAMSUBSTATE_NOSUBSTATE,

	TBSAMSUBSTATE_RUNABOUT_WAIT,			// SO: Waiting
	TBSAMSUBSTATE_RUNABOUT_LEFT,			// SO: Running left
	TBSAMSUBSTATE_RUNABOUT_RIGHT,			// SO: Running right

	TBSAMSUBSTATE_STANDOFF_SHOOTWALL,		// SO: Shoot the wall behind Taz

	TBSAMSUBSTATE_ATTACKTAZ_DEFEND,			// SO: Cane spin to warn Taz of impending attack
	TBSAMSUBSTATE_ATTACKTAZ_ATTACK,			// SO: Attack! Attack!

	TBSAMSUBSTATE_LUNGEFORPLUNGE_1,			// SO: Lunge for plunger 1
	TBSAMSUBSTATE_LUNGEFORPLUNGE_2,			// SO: Lunge for plunger 2
	TBSAMSUBSTATE_LUNGEFORPLUNGE_GOLAUNCH,	// SO: Heading for launch point
	TBSAMSUBSTATE_LUNGEFORPLUNGE_FLYING,	// SO: I'm flying!
	TBSAMSUBSTATE_LUNGEFORPLUNGE_FALLING,	// SO: Falling now
	TBSAMSUBSTATE_LUNGEFORPLUNGE_LANDING,	// SO: And to complete the jump - the land!

	TBSAMSUBSTATE_USEPLUNGER_GETON,			// SO: Get on plunger
	TBSAMSUBSTATE_USEPLUNGER_POSE,			// SO: Pose before using plunger (Note: occurs after closeup only)
	TBSAMSUBSTATE_USEPLUNGER_JUMPLOOP,		// SO: Work that plunger!
	TBSAMSUBSTATE_USEPLUNGER_ACTIVATION,	// SO: Plunger activated
	TBSAMSUBSTATE_USEPLUNGER_ASSBURN,		// SO: Ass burnt using the plunger

	TBSAMSUBSTATE_RESPAWN_WAIT,				// SO: Wait for Taz to get into a good position
	TBSAMSUBSTATE_RESPAWN_FALL_LEFT,		// SO: Fall on left side
	TBSAMSUBSTATE_RESPAWN_FALL_RIGHT,		// SO: Fall on right side
	TBSAMSUBSTATE_RESPAWN_FALLING,			// SO: I'm falling!
	TBSAMSUBSTATE_RESPAWN_LANDING,			// SO: Land then onto run around
	TBSAMSUBSTATE_RESPAWN_POSE,				// SO: Pose after landing

	TBSAMSUBSTATE_DEATH_DYING,				// SO: Fall off plunger, saved by claw
	TBSAMSUBSTATE_DEATH_DEAD,				// SO: After death .. we are dead
};

typedef struct TAG_TBSAM : public ACTORINSTANCEUSER
{
	TBSamState		state;
	TBSamSubState	subState;

	TBActorNodeInstance	*bodyNode;			// SO: Bone for camera focus
	TBActorNodeInstance	*caneNode;			// SO: Bone in Sam's cane

	TBPLUNGER		*lungePlunger;			// SO: Plunger we're lunging for
	float			timer;					// SO: Internal state timer
	int				jumps;					// SO: Number of times jumped on plunger
	int				wasAssBurnt;			// SO: Was my ass just burnt?
	int				totalAssBurns;			// SO: How many times has my ass been burnt?
	int				validAction;			// SO: Is this action valid yet (cane attacks)
	int				seenCloseup;			// SO: Have we seen a closeup of Sam?
	bool			samWinsPoint;			// TP: set to true to if Sam presses plunger, ready for processing after the explosions have

	/*	--------------------------------------------------------------------------------
		Function 	: TBSAM::init
		Purpose 	: initialisation
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void init(void);

	/*	--------------------------------------------------------------------------------
		Function 	: TBSAM::update
		Purpose 	: update routine
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void update(void);

	/*	--------------------------------------------------------------------------------
		Function 	: TBSAM::setState
		Purpose 	: set Sam's state
		Parameters 	: new state for Sam
		Returns 	: 
		Info 		: 
	*/
	void setState(const TBSamState stateIn, const TBSamSubState subStateIn=TBSAMSUBSTATE_NOSUBSTATE);

	/*	--------------------------------------------------------------------------------
		Function 	: TBSAM::setSubState
		Purpose 	: set Sam's sub-state
		Parameters 	: subStateIn - new sub-state for Sam
		              forceState - force this state (false means changes to the same state are ignored)
		Returns 	: 
		Info 		: 
	*/
	void setSubState(const TBSamSubState subStateIn, const char forceState=true);

	/*	--------------------------------------------------------------------------------
		Function 	: TBSAM::burnMyAss
		Purpose 	: Burn Sam's ass
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void burnMyAss();

}TBSAM, _TBSAM BALIGN16;



/* SO: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												TBTAZ
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
typedef struct TAG_TBTAZ : public ACTORINSTANCEUSER
{
	PLAYER*			player;
	TBVector		velocityBeforeSuspend;	// SO: Taz's velocity before suspending

	char			suspended;				// SO: Has Taz been suspended

	/*	--------------------------------------------------------------------------------
		Function 	: TBTAZ::init
		Purpose 	: initialisation
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void init(void);

	/*	--------------------------------------------------------------------------------
		Function 	: TBTAZ::stopChilliBreath
		Purpose 	: Cancels the chilli breath powerup
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void stopChilliBreath(void);

	/*	--------------------------------------------------------------------------------
		Function 	: TBTAZ::setSuspendedState
		Purpose 	: Set whether Taz is suspended or not
		Parameters 	: suspend yes/no
		Returns 	: 
		Info 		: 
	*/
	void setSuspendedState(char suspend);

}TBTAZ, _TBTAZ BALIGN16;



/* SO: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
											TAZBOSS1GAME
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
enum TBArenaArea
{
	TBARENA_AREA_UNKNOWN = 1<<0,	// SO: Unimportant areas
	TBARENA_AREA_TOP     = 1<<1,	// SO: Plunger 1
	TBARENA_AREA_LEFT    = 1<<2,	// SO: Standoff zone 1
	TBARENA_AREA_RIGHT   = 1<<3,	// SO: Standoff zone 2
	TBARENA_AREA_BOTTOM  = 1<<4,	// SO: Plunger 2
	TBARENA_AREA_CENTRE  = 1<<5,	// SO: Centre platform
	TBARENA_AREA_LAVA    = 1<<6,	// SO: Somewhere in the lava
};

#define TBARENA_AREAS_PLATFORM_CENTRE	(TBARENA_AREA_LEFT|TBARENA_AREA_RIGHT)
#define TBARENA_AREAS_BY_A_PLUNGER		(TBARENA_AREA_TOP|TBARENA_AREA_BOTTOM)
#define TBARENA_AREAS_IN_LAVA			(TBARENA_AREA_LAVA|TBARENA_AREA_UNKNOWN)

enum TBEventType
{
	TBEVENT_LIGHTNING,		// SO: Sam is shooting lightning with his cane
	TBEVENT_THUMP,			// SO: Sam is attacking with his cane
	TBEVENT_BIGBANG,		// SO: Sam has activated the plunger!
};

enum TBCamType
{
	TBCAM_DEFAULT,			// SO: Normal default Taz camera
	TBCAM_RUNABOUT,			// SO: Run about mode - keep Sam in focus
	TBCAM_SECURITY_SAM,		// SO: Security camera focusing on sam
	TBCAM_FIXED,			// SO: Fixed camera
};

typedef class TAG_TAZBOSS1GAME : public BOSSGAME
{
	friend	TBSAM;
	friend	TBTAZ;
	friend	TBPLUNGER;
	friend	void TazBoss1Reset(void);

private:

	TBSAM		sam;					// SO: Tycoon Sam structure
	TBTAZ		taz;					// SO: Taz structure

	TBPLUNGER	plunger1, plunger2;		// SO: Our 2 plungers

	TBCamType	camType;				// SO: Current camera type

	TBVector	camPos;					// SO: Position for Security/Fixed camera
	TBVector	camRef;					// SO: Reference for Fixed camera

	int			numExplosionsTriggered;	// TP: number of explosions Sam has successfully triggered

	BOOK		*scoreBook;				// TP: score book
	TEXTBOX		*tazScore,*samScore;	// TP: text boxes containing the score

public:

	EBossGameResetChoice continueChoice;

	char		readyForChilli;			// SO: Flag stating if we're "Ready For Chilli!"
	bool		chilliBurnEnabled;		// TP: do we currently allow chilli burns

	/*	--------------------------------------------------------------------------------
		Function 	: TAZBOSS1GAME::init
		Purpose 	: initialise the Tycoon Sam Boss game
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOSSGAME::init
	*/
	void init(void);


	/*	--------------------------------------------------------------------------------
		Function 	: TAZBOSS1GAME::shutdown
		Purpose 	: shut-down the Tycoon Sam Boss game
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOSSGAME::shutdown
	*/
	void shutdown(void);


	/*	--------------------------------------------------------------------------------
		Function 	: TAZBOSS1GAME::update
		Purpose 	: update the Tycoon Sam Boss game
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOSSGAME::update
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: TAZBOSS1GAME::draw
		Purpose 	: draw the Tycoon Sam Boss game solid stuff
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOSSGAME::draw
	*/
	void draw(void);


	/*	--------------------------------------------------------------------------------
		Function 	: TAZBOSS1GAME::draw2
		Purpose 	: draw the Tycoon Sam Boss game alpha stuff
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOSSGAME::draw2
	*/
	void draw2(void);


	/*	--------------------------------------------------------------------------------
		Function 	: TAZBOSS1GAME::setCamera
		Purpose 	: set the camera type
		Parameters 	: camera type, pos (security/fixed only), ref (fixed only)
		Returns 	: 
		Info 		: 
	*/
	void setCamType(const TBCamType camTypeIn, TBVector pos=NULL, TBVector ref=NULL);


	/*	--------------------------------------------------------------------------------
		Function 	: TAZBOSS1GAME::updateCamera
		Purpose 	: update the camera
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void updateCamera(void);


	/*	--------------------------------------------------------------------------------
		Function 	: TAZBOSS1GAME::doChilliPepperFireCollision
		Purpose 	: handles chilli pepper collisions
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void doChilliPepperFireCollision(CHILLIPEPPEREXTRAS *data);


	/*	--------------------------------------------------------------------------------
		Function 	: TAZBOSS1GAME::readyForChilliRespawn
		Purpose 	: Returns if we're ready for a new chillipepper
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	int readyForChilliRespawn(void);


	/*	--------------------------------------------------------------------------------
		Function 	: TAZBOSS1GAME::respondToEvent
		Purpose 	: respond to an animation event
		Parameters 	: event type
		Returns 	: 
		Info 		: 
	*/
	void respondToEvent(TBEventType eventType);


	/*	--------------------------------------------------------------------------------
		Function 	: TAZBOSS1GAME::getArenaAngle
		Purpose 	: Gets angle around the area (signed)
		Parameters 	: Who
		Returns 	: Angle
		Info 		: 
	*/
	float getArenaAngle(ACTORINSTANCEUSER *user);


	/*	--------------------------------------------------------------------------------
		Function 	: TAZBOSS1GAME::getArenaArea
		Purpose 	: Returns where Taz currently is
		Parameters 	: Who
		Returns 	: TBArenaArea
		Info 		: 
	*/
	TBArenaArea getArenaArea(ACTORINSTANCEUSER *user, bool sideOnly = false);

	/*  --------------------------------------------------------------------------------
		Function	: SamWinsAPoint
		Purpose		: Called when Sam pushes the plunger and triggers an explosion
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	void SamWinsAPoint()
	{
		numExplosionsTriggered ++;
	}

	/*  --------------------------------------------------------------------------------
		Function	: SamsScore
		Purpose		: Return the number of successfull explosions Sam has caused
		Parameters	: 
		Returns		: score
		Info		: 
	*/
	int SamsScore()
	{
		return numExplosionsTriggered;
	}

	/*  --------------------------------------------------------------------------------
		Function	: TAZBOSS1GAME::CreateScoreBook
		Purpose		: Setup up the score book for the game
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	void CreateScoreBook();

	/*  --------------------------------------------------------------------------------
		Function	: TAZBOSS1GAME::DeleteScoreBook
		Purpose		: Close and remove the score book
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	void DeleteScoreBook(void);

	void lose(void)
	{
		numExplosionsTriggered = 5;
		sam.totalAssBurns = 0;
	}

	void win(void)
	{
		numExplosionsTriggered = 0;
		sam.totalAssBurns = 5;
	}

}TAZBOSS1GAME BALIGN16;

extern TAZBOSS1GAME	tazBoss1Game;

void TazBoss1Reset(void);


#endif