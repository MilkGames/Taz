// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : WestBoss.h
//   Purpose : Wile E West Boss - Yosemite Sam's ammo dump
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_WESTBOSS
#define __H_WESTBOSS


#include "Boss.h"				// PP: General boss game stuff
#include "KOScam.h"				// PP: "Keep-On-Screen" camera
#include "LoopArray.h"			// PP: Looping array class - please use it!
#include "sfx.h"				// PP: sound effect rountines
#include "WestBossExp.h"		// PP: Bomber-Man-style corridor explosions, only for the West Boss game
#include "GolfBar.h"			// PP: Power-level indicator for the West Boss player 2's dynamite throwing
#include "bossgamecontinue.h"	// TP: continue scene

#define WB_HEARING_RANGE		METERTOUNIT(500.0f)// PP: ?

#define	WBGRID_GROUND			' '
//#define WBGRID_CRATE			'/'
#define WBGRID_WALL				'O'
#define WBGRID_DEPUTY			'+'

enum EWestBossCamera
{
//	WBCAM_INTROSAM,				// PP: Looking at sam in the intro ("I'll get ya ya varmint") (FULL SCREEN)
//	WBCAM_INTRODYNAMITE,		// PP: Following Sam's first stick of dynamite through the air in the intro (FULL SCREEN)
//	WBCAM_INTROTAZ,				// PP: not used.  Looking at taz in the intro ("uh-oh") (FULL SCREEN)
	WBCAM_PLAY,					// PP: playing the game (this includes 2-player split screen)
	WBCAM_EXPLOSIONDIVE,		// PP: taz is blasted towards the camera action-movie stylee (FULL SCREEN)
	WBCAM_SUPERCRATEEXPLODE,	// PP: a super crate explodes (FULL SCREEN)
	WBCAM_2PLAYER_OUTRO,		// PP: 2player outro yadda yadda i'm tired work it out for yourself
	WBCAM_TAZWIN,				// PP: used when taz wins
	WBCAM_TAZLOSE,				// PP: used when taz loses
	WBCAM_SAMWIN,				// PP: used when sam wins in 2 player
	WBCAM_NONE,
};

// PP: some game constants that are best kept here at the beginning of the header...

#define WB_WIDTH				METERTOUNIT(38.0f)
#define WB_HEIGHT				WB_WIDTH
								
#define WB_SQUARES_WIDE			19//17											// PP: RETHINKING REQUIRED IF THIS EXCEEDS 32!!!!
#define WB_SQUARES_HIGH			WB_SQUARES_WIDE									// PP: RETHINKING REQUIRED IF THIS EXCEEDS 32!!!!
								
#define WB_SQUARE_WIDTH			(WB_WIDTH/float(WB_SQUARES_WIDE))
#define WB_SQUARE_HEIGHT		(WB_HEIGHT/float(WB_SQUARES_HIGH))


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														WBSKATEBOMB

											Mobile bombs dipatched by Sam's deputies.
									(dynamite strapped to roller skates according to the doc)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


enum
{
	WBSBSTATE_ROLLING,
	WBSBSTATE_FINISHED
};

#define WBSB_MODEL					"food_dynamite.obe"

typedef struct TAG_WBSKATEBOMB:		public ACTORINSTANCEUSER
{
	TBVector2D		speed;
	float			distToTerminus;
	int				state;


	/*	--------------------------------------------------------------------------------
		Function 	: WBSKATEBOMB::init
		Purpose 	: init
		Parameters 	: x grid square, y grid square, x speed, z speed, distance to terminus
		Returns 	: 
		Info 		: 
	*/
	void init(const int gridXin, const int gridYin, const float xSpeed, const float zSpeed, const float range);


	/*	--------------------------------------------------------------------------------
		Function 	: WBSKATEBOMB::init
		Purpose 	: init
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void init(void)
	{
		ACTORINSTANCEUSER::init(WBSB_MODEL);

		// PP: switch off the skatebomb actorInstance till it gets dispatched
		this->actorInstance->flags |= (ACTORFLAG_DONTDRAW|ACTORFLAG_NOCOLLISION);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: WBSKATEBOMB::update
		Purpose 	: update
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WBSKATEBOMB::kaboom
		Purpose 	: explode
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void kaboom(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WBSKATEBOMB::TAG_WBSKATEBOMB
		Purpose 	: constructor: cals init
		Parameters 	: x grid square, y grid square, x speed, z speed, distance to terminus
		Returns 	: 
		Info 		: 
	*/
	TAG_WBSKATEBOMB(const int gridXin, const int gridYin, const float xSpeed, const float zSpeed, const float range)
	{
		this->init(gridXin, gridYin, xSpeed, zSpeed, range);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: WBSKATEBOMB::TAG_WBSKATEBOMB
		Purpose 	: default constructor: does bugger all
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	TAG_WBSKATEBOMB(void)
	{
	}


}WBSKATEBOMB, _WBSKATEBOMB BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														WBDEPUTY

								Sam's deputies, who hide in crates dispatching skatebombs.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: IDEA: it might be nice if you could destroy the deputies by spitting dynamite at them while they've got their flap open to 
// PP:			dispatch a bomb.

// PP: attack directions for deputies
enum
{
	WBDEPDIR_LEFT			=1,
	WBDEPDIR_RIGHT			=1<<1,
	WBDEPDIR_UP				=1<<2,
	WBDEPDIR_DOWN			=1<<3
};

enum
{
	WBDEPSTATE_BLOCKED,
	WBDEPSTATE_WAITING,
	WBDEPSTATE_OPENING,
	WBDEPSTATE_LAUNCHING,
	WBDEPSTATE_CLOSING
};

typedef struct TAG_WBDEPUTY
{
	int							gridX;
	int							gridY;
	int							attackDirections	:4;		// PP: directions in which the deputy can attack
	float						range[4];					// PP: distance to terminus in each direction
	float						timer;
	int							state;						// PP: state (WBDEPSTATE_)
	struct TAG_WBTNTCRATE*		blockingCrate;				// PP: single crate blocking this deputy

	/*	--------------------------------------------------------------------------------
		Function 	: WBDEPUTY::init
		Purpose 	: init
		Parameters 	: x grid square, y grid square
		Returns 	: 
		Info 		: 
	*/
	void init(const int gridXin, const int gridYin);


	/*	--------------------------------------------------------------------------------
		Function 	: WBDEPUTY::attack
		Purpose 	: attack
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void attack(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WBDEPUTY::update
		Purpose 	: udpate
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WBDEPUTY::setState
		Purpose 	: set state
		Parameters 	: new state to set
		Returns 	: 
		Info 		: 
	*/
	void setState(const int stateIn);


}WBDEPUTY, _WBDEPUTY BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														WBDYNAMITE

											Sticks of dynamite thrown by Sam.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define WBDYNAMITE_MODEL				"objects\\throwntnt.obe"

// PP: TWEAKME:
//#define WBDYNAMITE_FUSE_LENGTH_NORMAL		5.0f							// PP: length of dynamite fuse (normal)
//#define WBDYNAMITE_FUSE_LENGTH_FAST			1.0f							// PP: length of dynamite fuse (fast)


enum WBDynamiteState
{
	WBDYNAMITESTATE_LIT,						// PP: fizzle effects starts here, as soon as the dynamite is thrown
	WBDYNAMITESTATE_LANDED,						// PP: countdown actually starts here, once the dynamite has landed
//	WBDYNAMITESTATE_EXPLODING,
	WBDYNAMITESTATE_FINISHED
};

#define WBDYNAMITE_SHADOW_RADIUS		METERTOUNIT(1.0f)			// PP: radius of dynamite shadows
#define WBDYNAMITE_COLLISION_RADIUS		METERTOUNIT(0.4f)			// PP: radius of dynamite collision spheres

typedef struct TAG_WBDYNAMITE:	public ACTORINSTANCEUSER
{
	// PP: longest (initial) dynamite fuse lengths
	static float slowFuseLengths[NUM_DIFFICULTY_LEVELS];

	// PP: shortest (final) dynamite fuse lengths
	static float fastFuseLengths[NUM_DIFFICULTY_LEVELS];

	float						timer;			// PP: general-purpose timer, eg.time remaining in seconds before explosion
	WBDynamiteState				state;			// PP: state of the dynamite
	TBVector					targetPos;		// PP: position having landed
	int							gridX;			// PP: grid x pos
	int							gridY;			// PP: grid y pos
	TBVector2D					rotVec;			// PP: rotation vector
	float						yAng;			// PP: y ang of flight
	TBVector2D					rotSpeed;		// PP: rotation speed
	WESTBOSSEXPLOSION*			wbExplosion;	// PP: ptr to explosion to be caused by this dynamite


	/*	--------------------------------------------------------------------------------
		Function 	: WBDYNAMITE::TAG_WBDYNAMITE
		Purpose 	: default constructor; does bugger all
		Parameters 	: 
		Returns 	: new DYNAMITE instance
		Info 		: 
	*/
	TAG_WBDYNAMITE(void)
	{
	}


	/*	--------------------------------------------------------------------------------
		Function 	: WBDYNAMITE::init
		Purpose 	: initialisation
		Parameters 	: position, y angle
		Returns 	: 
		Info 		: 
	*/
	void init(TBVector posIn, const float yAngIn);


	/*	--------------------------------------------------------------------------------
		Function 	: WBDYNAMITE::init
		Purpose 	: initialisation
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void init(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WBDYNAMITE::TAG_WBDYNAMITE
		Purpose 	: constructor; calls init
		Parameters 	: position, y angle
		Returns 	: new DYNAMITE instance
		Info 		: 
	*/
	TAG_WBDYNAMITE(TBVector pos, const float yAngIn)
	{
		this->init(pos, yAngIn);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: WBDYNAMITE::plant
		Purpose 	: plant the dynamite on a specific grid square
		Parameters 	: grid x pos, grid y pos
		Returns 	: 
		Info 		: 
	*/
	void plant(const int gridXin, const int gridYin);


	/*	--------------------------------------------------------------------------------
		Function 	: WBDYNAMITE::kaboom
		Purpose 	: explode
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void kaboom(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WBDYNAMITE::setState
		Purpose 	: set state
		Parameters 	: new state for the dynamite
		Returns 	: 
		Info 		: 
	*/
	void setState(const WBDynamiteState stateIn);


	/*	--------------------------------------------------------------------------------
		Function 	: WBDYNAMITE::update
		Purpose 	: udpate
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WBDYNAMITE::draw
		Purpose 	: draw
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void draw(void);


}WBDYNAMITE, _WBDYNAMITE BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														WBSAM

										Yosemite Sam as the Wile E West boss
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: TWEAKME: from the doc...

// PP: #define WBSAM_MASSIVEATTACK_RADIUS		(5.5f*WB_SQUARE_WIDTH)			// PP: proximity to Sam that causes him to get mad

// PP: Sam's states - these may well change
enum WBSamState
{
	WBSAMSTATE_INTRO,										// PP: introducing the battle somehow
	WBSAMSTATE_INTROTHROW,									// PP: throwing first dynamite, waiting for it to hit ground
	WBSAMSTATE_INTROPOSTTHROW,								// PP: first dynamite has landed, just wait till taz has responded
	WBSAMSTATE_ATTACK,										// PP: attacking normally
//	WBSAMSTATE_MASSIVEATTACK,								// PP: attacking rapidly - Taz is close
	WBSAMSTATE_LOSING,										// PP: responding to a crate being destroyed
	WBSAMSTATE_WINNING,										// PP: celebrating have hurt Taz
	WBSAMSTATE_LOST,										// PP: lost the game
	WBSAMSTATE_WON,											// PP: won the game
	WBSAMSTATE_HUMANPLAY,									// PP: being played by a human

	NUM_WBSAM_STATES
};

enum WBSamFlags
{
	WBSAMFLAG_NONE								=0,
	WBSAMFLAG_PRODUCING							=1,
	WBSAMFLAG_CHUCKING							=1<<1,
	WBSAMFLAG_SULKING							=1<<2,
	WBSAMFLAG_CELEBRATING						=1<<3,
	WBSAMFLAG_FIRSTTHROWSINCELOSINGSUPERCRATE	=1<<4,		// PP: this is the first throw made by sam since he last lost a super crate - he's even more determined now, and makes a taunt
};

enum WBSamSpeech
{
	WBSAMSPEECH_THROW1,										// PP: There's no escape now, ha ha ha!
	WBSAMSPEECH_THROW2,										// PP: Catch that you critter!
	WBSAMSPEECH_THROW3,										// PP: You're not getting’ away this time, you hear me?
	WBSAMSPEECH_THROW4,										// PP: Eat this, you furball!

	WBSAMSPEECH_CRATEDESTROY1,								// PP: I don't believe it.
	WBSAMSPEECH_CRATEDESTROY2,								// PP: oooooooh!
	WBSAMSPEECH_CRATEDESTROY3,								// PP: oooooooh!
	WBSAMSPEECH_CRATEDESTROY4,								// PP: oooooooh!
	WBSAMSPEECH_CRATEDESTROY5,								// PP: what the?!
	WBSAMSPEECH_CRATEDESTROY6,								// PP: I don't believe it.
	WBSAMSPEECH_CRATEDESTROY7,								// PP: what the?!
	WBSAMSPEECH_CRATEDESTROY8,								// PP: This varmint's costin’ me a fortune!

	WBSAMSPEECH_SUPERCRATEDESTROY1,							// PP: Why ya consarned, skinny-legged, tongue-waggin' bird-chasin' idget mutt!
	
	WBSAMSPEECH_HURTTAZ1,									// PP: Bwa ha ha - How d'you like that!
	WBSAMSPEECH_HURTTAZ2,									// PP: Look at that devil do the rhumba!  Ha ha ha!
	WBSAMSPEECH_HURTTAZ3,									// PP: (laugh from WBSAMSPEECH_HURTTAZ1)
	WBSAMSPEECH_HURTTAZ4,									// PP: (laugh from WBSAMSPEECH_HURTTAZ2)

	WBSAM_NUM_PHRASES										// PP: total number of phrases for sam to say
};

// PP: TEMP - TODO: intro anim
//#define WBSAM_DURATION_INTRO			3.0f				// PP: duration of intro state before throwing first dynamite
															
#define WBSAM_DURATION_INTROPOSTTHROW	3.0f				// PP: duration of intro after throwing first dynamite

//#define WBSAM_NUM_CRATE_DESTRUCTION_ANIMS	3

#define WBSAM_MODEL						"yosemite.obe"
#define WBSAM_YANG						0.0f

#define WBSAM_ANIM_IDLE					"idle1"				// PP: sam's idle anim
															
typedef struct TAG_WBSAM:	public ACTORINSTANCEUSER		
{	
	// PP: longest (initial) intervals between sam's throws, for each difficulty level
	static float				slowThrowIntervals[NUM_DIFFICULTY_LEVELS];

	// PP: shortest (final) intervals between sam's throws, for each difficulty level
	static float				fastThrowIntervals[NUM_DIFFICULTY_LEVELS];

	PLAYER*						player;						// PP: player if any by whom Sam is being controlled
	float						timer;						// PP: general purpose timer, eg. time since last dynamite throw
	WBSamState					state;						// PP: Sam's state (WBSAMSTATE_)
	int							flags;						
	WBDYNAMITE*					introDynamite;				// PP: first stick of dynamite that Sam throws, in the intro
	TBActorAnimSegment*			produceAnim;				// PP: the anim segment that Sam will next use to produce dynamite
	TBActorAnimSegment*			idleAnim;					// PP: anim segment used to idle

	int							targetX;					// PP: target x grid coord
	int							targetY;					// PP: target y grid coord

	struct TAG_BRICKDISPLAY*	score;

	static char*				speechSamples[WBSAM_NUM_PHRASES];

	GOLFBAR*					golfBar;					// PP: human throw power-level guage
	float						humanThrowDistance;			// PP: distance of human throw
	float						humanTargetHeight;			// PP: height of human throw target


	// PP: number of different crate-destruction reactions
#define WBSAM_NUM_CRATE_DESTRUCTION_REACTIONS		3

	char						crateDestructionReactionList[WBSAM_NUM_CRATE_DESTRUCTION_REACTIONS];
	char						crateDestructionReaction;	// PP: index of crate-destruction reaction to use next


	/*	--------------------------------------------------------------------------------
		Function 	: WBSAM::loseSuperCrate
		Purpose 	: sam loses one of his super crates, thus losing a point
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void loseSuperCrate(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WBSAM::chooseRandomProduceAnim
		Purpose 	: randomly chooses a produce anim, but doesn't play it
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void chooseRandomProduceAnim(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WBSAM::init
		Purpose 	: initialisation
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void init(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WBSAM::produceDynamite
		Purpose 	: make Sam produce a stick of dynamite which he will then throw
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void produceDynamite(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WBSAM::startChuck
		Purpose 	: make Sam start to throw a stick of dynamite that he has produced
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void startChuck(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WBSAM::chuck
		Purpose 	: make sam throw a stick of dynamite to his target x&y grid coords
		Parameters 	: 
		Returns 	: ptr to the dynamite thrown
		Info 		: called 'chuck' because 'throw' is a keyword :o(
	*/
	WBDYNAMITE* chuck(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WBSAM::respondToMessage
		Purpose 	: respond to a message
		Parameters 	: message string
		Returns 	: 
		Info 		: 
	*/
	void respondToMessage(const char* const message);


	/*	--------------------------------------------------------------------------------
		Function 	: WBSAM::setState
		Purpose 	: set Sam's state
		Parameters 	: new state for Sam (WBSAMSTATE_)
		Returns 	: 
		Info 		: 
	*/
	void setState(const WBSamState stateIn);


	/*	--------------------------------------------------------------------------------
		Function 	: WBSAM::getThrowDistance
		Purpose 	: get the distance of a throw from a given power level
		Parameters 	: power level of throw
		Returns 	: distance
		Info 		: 
	*/
	static inline float getThrowDistance(const float level) 
	{
		// PP: the farthest we can throw is from the centre of the arena to the centre of a corner square
		return (level*SQRT2*(WB_WIDTH-(WB_SQUARE_WIDTH*3.0f))*0.5f);// PP: this assumes a square play area
	}


	/*	--------------------------------------------------------------------------------
		Function 	: WBSAM::getThrowPowerLevel
		Purpose 	: get the power level of a throw from a given distance
		Parameters 	: distance of throw
		Returns 	: distance
		Info 		: 
	*/
	static inline float getThrowPowerLevel(const float distance) 
	{
		// PP: the farthest we can throw is from the centre of the arena to the centre of a corner square
		return (distance/(SQRT2*(WB_WIDTH-(WB_SQUARE_WIDTH*3.0f))*0.5f));// PP: this assumes a square play area
	}


	/*	--------------------------------------------------------------------------------
		Function 	: WBSAM::humanThrowCallback
		Purpose 	: throw callback for player 2 Sam's golf bar
		Parameters 	: ptr to Sam, power level of throw
		Returns 	: 
		Info 		: must match the GolfBarHitCallback typedef
	*/
	static void humanThrowCallback(const long32 context, const float level);


	/*	--------------------------------------------------------------------------------
		Function 	: WBSAM::humanUpdate
		Purpose 	: human update
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void humanUpdate(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WBSAM::aiUpdate
		Purpose 	: ai update
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void aiUpdate(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WBSAM::update
		Purpose 	: update
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WBSAM::say
		Purpose 	: say a line of dialogue
		Parameters 	: 
		Returns 	: 
		Info 		: "Sam might saaayyy....."
	*/
	void say(const WBSamSpeech line)
	{
		this->playSound(TAG_WBSAM::speechSamples[line]);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: WBSAM::say
		Purpose 	: say or queue a line of dialogue
		Parameters 	: 
		Returns 	: 
		Info 		: "Sam might saaayyy....."
	*/
	void sayQueued(const WBSamSpeech line)
	{
		this->queueSound(TAG_WBSAM::speechSamples[line]);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: WBSAM::sulk
		Purpose 	: respond to a crate being destroyed
		Parameters 	: whether or not the crate is a super crate
		Returns 	: 
		Info 		: 
	*/
	void sulk(const bool super);


	/*	--------------------------------------------------------------------------------
		Function 	: WBSAM::celebrate
		Purpose 	: respond to hurting taz with dynamite
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void celebrate(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WBSAM::draw
		Purpose 	: draw effects for sam
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void draw(void);

						
}WBSAM, _WBSAM BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														WBTAZ

											Taz in the Wile E West Boss game
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

enum WBTazFlags
{
	WBTAZFLAG_NONE						=0,
	WBTAZFLAG_EXPLOSIONDIVE				=1,
	WBTAZFLAG_UPWARDSEXPLOSIONDIVE		=1<<1,
	WBTAZFLAG_INJURYSTARS				=1<<2,
};

#define WBTAZ_DURATION_INJURYSTARS		2.5f

typedef struct TAG_WBTAZ:	public ACTORINSTANCEUSER
{
	PLAYER*								player;
	int									gridX;
	int									gridY;
	int									flags;				// PP: flags (WBTAZFLAG_...)
	float								timer;				// PP: general-purpose timer
	float								arrowTimer;			// PP: timer used for the 2player marker arrow

	float								explosionHitTimer;	// PP: time since last being hurt by an explosion

	struct TAG_HEALTHBAR*				healthBar;			// PP: health bar display
										
	struct								
	{									
		float							targetPosition;		// PP: the target monaxial coordinate
		char							targetAxis;			// PP: X or Z - the axis Taz is getting blown along
		char							targetSide;			// PP: side of target pos that Taz isn't to end up at: -1 means stay above targetPos, 1 means stay below targetPos
		WBE_Direction					dir;				// PP: direction of Taz's dive
		TBVector						camPos;				// PP: camera position for Taz's dive

	}explosionDive;

	TBActorInstance*					markerInstance;		// PP: actor instance used for taz's 2player marker arrow that allows player 2 (sam) to see him better


	/*	--------------------------------------------------------------------------------
		Function 	: WBTAZ::init
		Purpose 	: initialisation
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void init(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WBTAZ::update
		Purpose 	: update
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WBTAZ::drawMarker
		Purpose 	: draw taz's 2player marker arrow for sam (player 2) to see him
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: hmm, not putting the arrow object on an instance list cos I don't
						know yet what sort of control I want
	*/
	void drawMarker(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WBTAZ::draw
		Purpose 	: draw
		Parameters 	: 
		Returns 	: 
		Info 		: draws taz's 2player marker arrow for sam to see him
	*/
	void draw(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WBTAZ::launchStateChangeCallback
		Purpose 	: function called when a character collides with something while in projectile state
		Parameters 	: ptr to actor instance
		Returns 	: 
		Info 		: must match the ProjectileStateCollisionCallback typedef
	*/
	static void projectileStateCollisionCallback(ACTORINSTANCE* const actorInstance);


	/*	--------------------------------------------------------------------------------
		Function 	: WBTAZ::launchStateChangeCallback
		Purpose 	: function called when Taz is changes state to be launched into the air by an explosion
		Parameters 	: ptr to actor instance
		Returns 	: 
		Info 		: 
	*/
	static void launchStateChangeCallback(ACTORINSTANCE *actorInstance);


	/*	--------------------------------------------------------------------------------
		Function 	: WBTAZ::getHitByExplosion
		Purpose 	: get hit, right...BY AN EXPLOSION!  *gasp!*
		Parameters 	: (opt/NULL=no branch, ie. centre of blast) ptr to branch info of fireball hitting taz
		Returns 	: 
		Info 		: 
	*/
	void getHitByExplosion(const WBE_FIREBALLBRANCHINFO* const branchInfo = NULL);


	/*	--------------------------------------------------------------------------------
		Function 	: WBTAZ::shutdown
		Purpose 	: shutdown
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void shutdown(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WBTAZ::loseHealthFromExplosion
		Purpose 	: lose health from being hit by an explosion
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void loseHealthFromExplosion(void);

						
}WBTAZ, _WBTAZ BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														WBTNTCRATE

											Exploding crates and Super-TNT crates
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: TEMP?  TODO: that bouncy-round-mesh thing
// PP: height that crate fragments fall to before finishing
#define WBTNTCRATE_FRAGMENT_FINISHED_HEIGHT		METERTOUNIT(-30)


enum WBTNTCrateType
{
	WBCRATETYPE_NORMAL,
	WBCRATETYPE_SUPER
};

typedef struct TAG_WBTNTCRATE:	public ACTORINSTANCEUSER
{
	bool						notUsed;		// PP: crate ain't used
	bool						exploded;		// PP: crate has exploded
	bool						finished;		// PP: crate has finished exploding - can be cleaned up completely
	float						timer;			// PP: la la general-purpose timer...PERVERT! =)		// PP: Oh God no - the change - IT'S...HAPPENING!	PERVERT! =)
	int							gridX;			// PP: closest grid square X index
	int							gridY;			// PP: closest grid square Y index
	WBTNTCrateType				type;
	BOUNDINGBOX					boundingBox;	// PP: absolute bounding box, taking into account fragment positions

	TBVector					blastDir;		// PP: direction of blast
	int							numFragments;

	

	struct WBTNTCRATEFRAGMENT
	{
		TBActorNodeInstance*				node;
		TBVector							vel;
	//	TBQuaternion						rotVelQuat;
		TBVector							rotVelVec;
		TBVector							rotVec;
	//	TBQuaternion						rotQuat;
		TBVector							pos;
		bool								finished;

	}							*fragments;


	/*	--------------------------------------------------------------------------------
		Function 	: WBTNTCRATE::initFragments
		Purpose 	: initialise a tnt crate's fragments
		Parameters 	: (opt/very root)ptr to root fragment, (opt/NULL=top level)fragment index (for recursion)
		Returns 	: 
		Info 		: 
	*/
	void initFragments(TBActorNodeInstance* rootNode=NULL, int* const fragmentIndexIn=NULL);


	/*	--------------------------------------------------------------------------------
		Function 	: WBTNTCRATE::launchFragments
		Purpose 	: launch a tnt crate's fragments as it explodes
		Parameters 	: y angle of blast causing crate to explode
		Returns 	: 
		Info 		: 
	*/
	void launchFragments(const float yAng);


	/*	--------------------------------------------------------------------------------
		Function 	: WBTNTCRATE::countFragments
		Purpose 	: count the number of chunks forming an actor
		Parameters 	: (opt/NULL=very root) ptr to root fragment
		Returns 	: number of chunks forming an actor
		Info 		: 
	*/
	int countFragments(TBActorNodeInstance* rootNode=NULL);


	/*	--------------------------------------------------------------------------------
		Function 	: WBTNTCRATE::drawFragments
		Purpose 	: draw fragments of an exploded crate
		Parameters 	: 
		Returns 	: 
		Info 		: forces clipping while drawing the fragments
	*/
	void drawFragments(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WBTNTCRATE::init
		Purpose 	: initialisation
		Parameters 	: name of crate instance, crate type (WBCRATETYPE_...)
		Returns 	: 
		Info 		: this overrides ACOTRINSTANCEUSER::init
	*/
	void init(char* const name, WBTNTCrateType typeIn);


	/*	--------------------------------------------------------------------------------
		Function 	: WBTNTCRATE::kaboom
		Purpose 	: explode
		Parameters 	: angle of blast causing crate to explode
		Returns 	: 
		Info 		: 
	*/
	void kaboom(const float yAng);


	/*	--------------------------------------------------------------------------------
		Function 	: WBTNTCRATE::kaboom
		Purpose 	: explode
		Parameters 	: direction of blast causing crate to explode(WBEDIRECTION_...)
		Returns 	: 
		Info 		: 
	*/
	inline void kaboom(const WBE_Direction dir)
	{
		this->kaboom(toYAng(dir));
	}


	/*	--------------------------------------------------------------------------------
		Function 	: WBTNTCRATE::updateFragments
		Purpose 	: update fragments of an exploded crate
		Parameters 	: void
		Returns 	: 
		Info 		: 
	*/
	void updateFragments(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WBTNTCRATE::update
		Purpose 	: update
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WBTNTCRATE::shutdown
		Purpose 	: shutdown
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void shutdown(void);


}WBTNTCRATE, _WBTNTCRATE BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														WESTBOSSGAME

						This is the BOSSGAME type for the Wile E West Boss - Yosemite Sam's ammo dump
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

						
#define WB_MAX_NUM_EXPLOSIVE_CRATES				19// PP: 23
#define WB_MAX_NUM_SUPER_TNT_CRATES				4
					
#define WB_NUM_EXPLOSIVE_CRATES					(WestBoss_game.twoPlayer?0:WB_MAX_NUM_EXPLOSIVE_CRATES)		// PP: TEMP?: no crates in 2player
#define WB_NUM_SUPER_TNT_CRATES					(WestBoss_game.twoPlayer?0:WB_MAX_NUM_SUPER_TNT_CRATES)		// PP: TEMP?: no stnt crates in 2player
												
#define WB_NUM_DEPUTIES							4
												
#define WB_MAX_DYNAMITES						15// PP: 4
												
#define WB_MAX_SKATEBOMBS						9

typedef struct TAG_WESTBOSSGAME		:			public BOSSGAME
{
	// PP: "I'll be there for yoooo...."

	friend										WBSAM;											// PP: Yosemite Sam as the Wile E West boss
	friend										WBTAZ;											// PP: Taz in the Wile E West Boss game
	friend										WBSKATEBOMB;									// PP: Mobile bombs dipatched by Sam's deputies
	friend										WBDYNAMITE;										// PP: Sticks of dynamite thrown by Sam
	friend										WBDEPUTY;										// PP: Sam's deputies, who hide in crates dispatching skatebombs
	friend										WBTNTCRATE;										// PP: Exploding crates and Super-TNT crates

	// PP: our friends in WestBossExp.h...

	friend										class TAG_WESTBOSSEXPLOSION;					// PP: west boss Bomber-Man explosions
	friend										class TAG_WESTBOSSEXPBRANCH;					// PP: The branches that make up a west boss explosion

private:

	// PP: YOSEMITE SAM
	WBSAM										sam;
												
	// PP: TAZ									
	WBTAZ										taz;
												
	LOOPARRAY<WBDYNAMITE, WB_MAX_DYNAMITES>		dynamiteList;									// PP: list of dynamite sticks (struct WBDYNAMITE instances)
	LOOPARRAY<WBSKATEBOMB, WB_MAX_SKATEBOMBS>	skateBombList;									// PP: list of skate bombs (struct WBSKATEBOMB instances)
																								
	WBTNTCRATE									crates[WB_MAX_NUM_EXPLOSIVE_CRATES];			// PP: array of ptrs to explosive crates
																								
	WBTNTCRATE									superTNT[WB_MAX_NUM_SUPER_TNT_CRATES];			// PP: array of ptrs to super TNT crates

	WBTNTCRATE*									explodingSuperCrate;							// PP: super TNT crate currently exploding
												
	WBDEPUTY									deputies[WB_NUM_DEPUTIES];
																								
	ACTORINSTANCE*								collisionMesh;									// PP: collision mesh that we don't want dynamite to collide with

	// PP: static so's it can be initialised at file scope; const because it IS initialised at file scope
	static const char							grid[WB_SQUARES_HIGH][WB_SQUARES_WIDE+1];		// PP: playable grid represented as a 2D character array
												
	TBVector									gridPoints[WB_SQUARES_HIGH][WB_SQUARES_WIDE];	// PP: a bit wasteful but who cares: 3D positions of the centres of grid squares // PP: could this be used for some ultra-cheap collision action?  hmm

	EWestBossCamera								camShot;										// PP: current camera shot (WBCAM_...)
	CAMERACASE*									cameraCase;										// PP: 'Keep-on-screen' camera case for the west boss game
	CAMERAINFO									dummyCam;										// PP: dummy camera that the KOS cameracase acts on - this is used as one of two reference points for camera[0]'s position, lookat, fov etc.


	int											numCratesDestroyed;								// PP: number of normal crates destroyed - handy to keep track of

	// PP: got a smell of TEMP about it		// PP: Mmmmmm, the smell of temp's...
	bool										movieMode;										// PP: in a non-controllable movie-style camera shot, eg. explosion dive

	VEC											outroCamPos[2];									// PP: positions for outro cameras, one per player
	VEC											outroCamLookat[2];									// PP: positions for outro cameras, one per player
							

	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::setupTazOutroCam
		Purpose 	: setup taz for an outro shot
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void setupTazOutroCam(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::setCamera
		Purpose 	: set the camera shot
		Parameters 	: shot to set
		Returns 	: 
		Info 		: 
	*/
	void setCamera(const EWestBossCamera cam);


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::initScoreBook
		Purpose 	: initialise the book for overlays such as score readouts
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOSSGAME::initScoreBook
	*/
	void initScoreBook(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::initMessageBook
		Purpose 	: initialise the book for messages such as "GET READY..." or "GOAL!"
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOSSGAME::initMessageBook
	*/
	void initMessageBook(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::initInstructionBook
		Purpose 	: initialise the book for instructions such as "press X to start"
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOSSGAME::initInstructionBook
	*/
	void initInstructionBook(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::addCameraTarget
		Purpose 	: add a target for the camera
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	inline void addCameraTarget(const ACTORINSTANCEUSER* const target)
	{
		this->cameraCase->KOScamData->addTarget(&(target->actorInstance->actorInstance));
	}


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::addSkateBomb
		Purpose 	: create a mobile bomb and set it in motion
		Parameters 	: grid x square, grid y square, x speed, z speed, distance to terminus
		Returns 	: 
		Info 		: 
	*/
	void addSkateBomb(const int gridX, const int gridY, const float xSpeed, const float zSpeed, const float range);


// PP: it seems that the level is a bit off-centre - this fixes it...

#define WB_GRID_X_OFFSET	-100.0f			// PP: distance by which the level is off-centre on the X axis
#define WB_GRID_Z_OFFSET	0.0f			// PP: distance by which the level is off-centre on the Z axis


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::evaluateGridX
		Purpose 	: convert an x grid position to a x world position
		Parameters 	: x grid position
		Returns 	: x world position
		Info 		: 
	*/
	inline float evaluateGridX(const float gridX) const
	{
		return WB_GRID_X_OFFSET+((gridX-(WB_SQUARES_WIDE/2.0f))*WB_SQUARE_WIDTH);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::getHeightOnGrid
		Purpose 	: get the height of an xz grid position
		Parameters 	: x & z grid coords
		Returns 	: height of grid at that pos
		Info 		: TODO: interpolate height from surrounding squares and exact pos with square
	*/
	inline float getHeightOnGrid(const float gridX, const float gridZ) const
	{
		return (TAG_WESTBOSSGAME::gridPoints[(int)gridZ][(int)gridX])[Y];
	}


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::evaluateGridZ
		Purpose 	: convert a z grid position to a z world position
		Parameters 	: z grid position
		Returns 	: z world position
		Info 		: 
	*/
	inline float evaluateGridZ(const float gridZ) const
	{
		return WB_GRID_Z_OFFSET+((gridZ-(WB_SQUARES_HIGH/2.0f))*WB_SQUARE_HEIGHT);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::evaluateGridPos
		Purpose 	: convert x&y grid coordinates to 3d position, by interpolating between grid points
		Parameters 	: x&y grid coordinates, 3d position to fill in
		Returns 	: 
		Info 		: 
	*/
	void evaluateGridPos(const TBVector2D gridPos, TBVector pos) const;


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::sitPointOnGrid
		Purpose 	: correct the height of a 3D position to sit it onto the grid
		Parameters 	: position to correct, (opt/0.0f)height to add onto grid height
		Returns 	: 
		Info 		: 
	*/
	void sitPointOnGrid(TBVector pos, float height=0.0f) const;


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::getCrate
		Purpose 	: get the crate that sits roughly on the specified square
		Parameters 	: x square index, y square index
		Returns 	: the crate that sits roughly on the specified square!
		Info 		: 
	*/
	WBTNTCRATE* getCrate(const int x, const int y);


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::getGridX
		Purpose 	: get the x index of the grid square on which the specified x position sits
		Parameters 	: x position in world space
		Returns 	: the x index of the grid square on which the specified x position sits!
		Info 		: 
	*/
	static inline int getGridX(const float xPos)
	{
		return ((int)((xPos+(WB_WIDTH/2.0f))/WB_SQUARE_WIDTH));
	}


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::getGridY
		Purpose 	: get the y index of the grid square on which the specified z position sits
		Parameters 	: z position in world space
		Returns 	: the y index of the grid square on which the specified z position sits!
		Info 		: 
	*/
	static inline int getGridY(const float zPos)
	{
		return ((int)((zPos+(WB_HEIGHT/2.0f))/WB_SQUARE_HEIGHT));
	}


public:


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::updateCamera
		Purpose 	: update the Zoo boss game's camera or cameras
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void updateCamera(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::setState
		Purpose 	: switch the game into a new state
		Parameters 	: the state into which to switch
		Returns 	: 
		Info 		: this overrides BOSSGAME::setState
	*/
	void setState(const int newState);
	// PP: public: TEMP? MILESTONE # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::TAG_WESTBOSSGAME
		Purpose 	: default constructor: does bugger all
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
/*	TAG_WESTBOSSGAME(void)
	{
	}*/


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::preInit
		Purpose 	: initialisation to be done before the scene is loaded
		Parameters 	: 
		Returns 	: 
		Info 		: puts the display into 32 bit
	*/
	void preInit(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::init
		Purpose 	: initialise the West Boss game
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOSSGAME::init
	*/
	void init(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::update
		Purpose 	: update the West Boss game
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOSSGAME::update
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::draw
		Purpose 	: draw the west boss game solid stuff
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOSSGAME::draw
	*/
	void draw(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::draw2
		Purpose 	: draw the West boss game alpha stuff
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOSSGAME::draw2
	*/
	void draw2(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::shutdown
		Purpose 	: shut-down the West Boss game
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOSSGAME::shutdown
	*/
	void shutdown(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::postShutdown
		Purpose 	: shutdown to be done after the scene is completely shut down
		Parameters 	: 
		Returns 	: 
		Info 		: puts the display back into 16 bit
	*/
	void postShutdown(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::respondToMessage
		Purpose 	: handle a message from the ball or a bat
		Parameters 	: pointer to actor instance, pointer to message string
		Returns 	: 
		Info 		: 
	*/
	void respondToMessage(const ACTORINSTANCE* const actorInstance, const char* const message);

	/* --------------------------------------------------------------------------------
	   Function		: WESTBOSSGAME::resetCallback
	   Purpose		: function called when the player selects the option to continue or to quit
	   Parameters	: context value passed through from ProcessTazDeathAtEndOfBossGame,
						the player's choice of action (BOSSGAMERESET_CONTINUE, BOSSGAMERESET_QUIT)
	   Returns		: big boobs
	   Info			:
	*/
	static int32 resetCallback(void *context, EBossGameResetChoice choice);


	/* --------------------------------------------------------------------------------
	   Function		: WESTBOSSGAME::getAdjacentGroundSquare
	   Purpose		: get an adjacent ground square to the square specified
	   Parameters	: x coord of square, y coord of square, [in/out]x coord of closest ground square,
						[in/out]y coord of closest ground square
	   Returns		: 
	   Info			:
	*/
	void getAdjacentGroundSquare(const uint32 gridX, const uint32 gridY, uint32 *const gridXOut, uint32 *const gridYOut);


	/* --------------------------------------------------------------------------------
	   Function		: WESTBOSSGAME::overrideInput
	   Purpose		: determine if player's input is to be ignored
	   Parameters	: 
	   Returns		: true if player's input is to be ignored, else false
	   Info			:
	*/
	inline bool overrideInput(void) const
	{
		// PP: can't control taz unless in 'play' state
		return (this->state != BGSTATE_PLAY);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::initCrates
		Purpose 	: initialise all tnt and super-tnt crates in the level
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void initCrates(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::removeDynamite
		Purpose 	: remove all dynamite sticks from the level
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void removeDynamite(void);


	/*	--------------------------------------------------------------------------------
		Function 	: WESTBOSSGAME::removeCrates
		Purpose 	: remove all tnt and super-tnt crates from the level
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void removeCrates(void);


}WESTBOSSGAME, _WESTBOSSGAME BALIGN16;

extern WESTBOSSGAME					WestBoss_game;				// PP: the West Boss game

// PP: Getting REALLY lazy here!  Even the shift key is too heavy now...
#define wbtaz						(WestBoss_game.taz)
#define wbsam						(WestBoss_game.sam)
#define wbcrate						(WestBoss_game.crates)
#define wbstnt						(WestBoss_game.superTNT)


#endif // PP: ndef __H_WESTBOSS