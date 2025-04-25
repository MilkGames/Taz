// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : ZooBoss.h
//   Purpose : Zoo Boss - Gossamer vs. Taz in an Elephant Pong showdown
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_ZOOBOSS
#define __H_ZOOBOSS


#include "Boss.h"					// PP: hmm
#include "BrickDisplay.h"			// PP: 'Brick Wall' score display for the Zoo boss game
#include "bossgamecontinue.h"		// PP: boss game continue stuff


#define ZB_COURT_LENGTH				METERTOUNIT(32.0f)
#define ZB_COURT_WIDTH				METERTOUNIT(20.0f)

#define ZB_COURT_TOP				(ZB_COURT_WIDTH*0.5f)

#define ZB_MAX_SCORE_1PLAYER		3		// PP: max score in 1-player
#define ZB_MAX_SCORE_2PLAYER		5		// PP: max score in 2-player

enum EZooBossCamera
{
	ZBCAM_INTRODROP,
	ZBCAM_INTROHOOT,
	ZBCAM_PLAY,
	ZBCAM_GOALWINNER,
	ZBCAM_GOALLOSER,
	ZBCAM_2PLAYER_OUTRO,
};	


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
															ZBBAT

								These are the 'bats' (Taz & Gossamer) used in the Zoo Boss game

															@zbbat
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: types of bat
enum ZBBatType
{
	ZBBATTYPE_TAZ,
	ZBBATTYPE_GOSSAMER,

	NUM_ZBBAT_TYPES
};

// PP: the bats are different to the ball in that they use all the ACTORINSTANCE states (STATE_MOVE, STATE_SKIDSTOP, etc)
// PP: so the only states here are those which apply to bats but not to actorinstances, see?
// PP: it just seems like the neatest way of doing it right now - let me know if you disagree
enum ZBBatState
{
	ZBBATSTATE_NONE,								// PP: not sure what this is really for yet

	ZBBATSTATE_PLAYING,								// PP: playing the game
	ZBBATSTATE_WINNING,								// PP: celebrating having scored a goal, gloating
	ZBBATSTATE_LOSING,								// PP: being pissed-off at having lost a point
	ZBBATSTATE_WON,									// PP: won the boss game
	ZBBATSTATE_LOST,								// PP: lost the boss game

	NUM_ZBBAT_STATES								// PP: Numero ståtos di bãta
};

// PP: Bat message strings...
#define ZBBATMESSAGE_STOMP		"stomp"				// PP: Gossamer's stomp effect

// PP: Bat state durations
//#define ZBBAT_DURATION_WAITFORSTOMP		0.618083f	// PP: TEMP: amount of time into scare move before stomp actually happens

#define ZBBAT_INITIAL_STATE		ZBBATSTATE_NONE

// PP: Gossamer's shockwave parameters
#define ZBSHOCKWAVE_MAXRADIUS		METERTOUNIT(4.5f)
#define ZBSHOCKWAVE_STARTSPEED		METERTOUNIT(0.73f)
#define ZBSHOCKWAVE_ENDSPEED		METERTOUNIT(0.2f)
#define ZBSHOCKWAVE_NUMRINGS		3
#define ZBSHOCKWAVE_INTERVAL		0.07f
#define ZBSHOCKWAVE_STARTWIDTH		METERTOUNIT(0.0f)
#define ZBSHOCKWAVE_ENDWIDTH		METERTOUNIT(2.5f)


// PP: scare types - so's the elephant knows how to respond
enum ZBBatScareType
{
	SCARE_SHOUT,
	SCARE_STOMP,
	SCARE_SUPERSPIN,
};


// PP: Bat AI info
struct ZBBATAI										
{												
	// PP: AI run-speed factors for each of the different difficulty levels.  By slowing Gossamer down, we we can make the game easier.
	static float			runSpeedFactors[NUM_DIFFICULTY_LEVELS];

	bool					doneScare;
	TBCollisionLine2D		interceptLine;				// PP: line we'll use to see if there's a good point to intercept this ball path line segment
	TBVector2D				destPos;			
	TBVector2D				ballPosOnScare;				// PP: where the ball will be when we scare it (a wee bit in front of the bat)
	float					eta;
	float					timeToScare;				// PP: TODO: comment me
	float					interceptLineOffset;		// PP: -1..1 random offset of lateral line used to intercept ball path
	float					runSpeedFactor;				// PP: maximum speed factor for the bat's run (used as a difficulty regulator). 1.0f=play run anim at full speed max; 0.5f=play run anim at half speed max
};


typedef struct TAG_ZBBAT:		public ACTORINSTANCEUSER
{

private:

													
	// PP: animation/movement stuff...					
														
	float						runPhase;					// PP: point in the run cycle (0..1)
	float						idlePhase;					// PP: point in the idle cycle (0..1)
														
	float						overallSpeedFraction;		// PP: speed level 0..1 (min..max)
	TBVector2D					speedFractions;				// PP: X&Z speed fractions aligned to character -1..1
														
	float						speed;						// PP: actual speed (distance units/second)
														
	float						timer;						// PP: general purpose timer
														
	struct												
	{													
		TBActorAnimSegment*		run;					
		TBActorAnimSegment*		runSkid;					// PP: hmm do I need this?
															
		TBActorAnimSegment*		runBack;					
		TBActorAnimSegment*		runBackSkid;				// PP: hmm do I need this?
															
		TBActorAnimSegment*		strafeRight;				
		TBActorAnimSegment*		strafeRightSkid;			// PP: hmm do I need this?
															
		TBActorAnimSegment*		strafeLeft;					
		TBActorAnimSegment*		strafeLeftSkid;				// PP: hmm do I need this?
								
		TBActorAnimSegment*		knocked;
								
		TBActorAnimSegment*		prodded;
		TBActorAnimSegment*		getup;
								
		TBActorAnimSegment*		idle;
								
	}anims;						
								
	bool						frameBlendLatch;			// PP: latching flag to say we're blending frame at a time - 
															// PP: used to prevent animation hoy
															// PP: caused by the normal looping animations interfering with the
															// PP: frame-by-frame blended animations
								
								
	float						normalYang;					// PP: y angle bat should normally have
								
	float						moveYangRelative;			// PP: difference between angle bat's facing and angle it's moving in
	
	ZBBATAI						ai;							// PP: AI info
														
	//struct TAG_BRICKDISPLAY*	score;						// PP: brick-display page item actually holds the score


	// PP: TEMP?
	char						score;						// PP: record of score
	TEXTBOX*					scoreBox;					// PP: text box to display score

	bool						allowMove;					// PP: whether or not to allow movement of the bat

	float						runSpeed;					// PP: speed of run m/s

	TBActorNodeInstance*		waistBone;					// PP: waist bone used to twist character towards what it's looking at

	const ACTORINSTANCEUSER*	characterToLookAt;			// PP: character that the bat looks at

public:		

	RECTANGLE					court;						// PP: legal area for the bat to be in (the relevant half of the game court).  Ahem that's *LEGAL* area?
														
	ZBBatType					type;					
	ZBBatState					state;					
	PLAYER*						player;					
	float						radius;					
	const float*				wall;						// PP: pointer to one of the coords in the ball's court rectangle, representing the wall this bat has to protect - yes, THERE CAN ONLY BE ONE BALL!!  To hell with this pointless flexibility nonsense.  The ZOOBOSSGAME still keeps a court rect just so it can divide it up for the bats and copy it for the ball.
														
	float						noiseLevel;					// PP: level of noise used for scaring the ball (0..1)

	ZBBatScareType				scareType;

	CAMERAINFO*					cam;						// PP: ptr to camera used to view this bat

	BOOK						book;						// PP: book, eg. used for scare prompts


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBAT::sulk
		Purpose 	: sulk (other player has scored)
		Parameters 	: ref to opponent who has scored
		Returns 	: 
		Info 		: 
	*/
	void sulk(struct TAG_ZBBAT& winner);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBAT::getScore
		Purpose 	: get the bat's score
		Parameters 	: 
		Returns 	: the bat's score
		Info 		: 
	*/
	inline int getScore(void) const
	{
		return (int)(this->score);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBAT::updateScoreDisplay
		Purpose 	: display updated score
		Parameters 	: 
		Returns 	: 
		Info 		: not as silly as it looks
	*/
	void updateScoreDisplay(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBAT::increaseScore
		Purpose 	: increase the bat's score
		Parameters 	: 
		Returns 	: 
		Info 		: not as silly as it looks
	*/
	void increaseScore(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBAT::celebrate
		Purpose 	: celebrate scoring a goal
		Parameters 	: ref to opponent against whom a goal was scored
		Returns 	: 
		Info 		: 
	*/
	void celebrate(struct TAG_ZBBAT& loser);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBAT::AIupdate
		Purpose 	: update the bat according to its AI
		Parameters 	: ref to the ball
		Returns 	: 
		Info 		: 
	*/
	void AIupdate(struct TAG_ZBBALL& ball);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBAT::humanUpdate
		Purpose 	: update the bat according to player input
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void humanUpdate(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBAT::init
		Purpose 	: initialise the bat
		Parameters 	: y angle, court rectangle, ptr to wall in ball's court to protect, type for bat,
						ptr to text box for score, (opt/NULL=cpu)player struct
		Returns 	: 
		Info 		: 
	*/
	void init(const float yAng, const RECTANGLE& courtIn, float* const wallIn, const ZBBatType typeIn,
				struct TAG_TEXTBOX* const scoreBoxIn, PLAYER* const playerIn=NULL);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBAT::setNormalYang
		Purpose 	: define the normal y angle for the bat
		Parameters 	: the normal y angle for the bat
		Returns 	: 
		Info 		: 
	*/
	inline void setNormalYang(const float yAng)
	{
		this->normalYang=yAng;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBAT::getNormalYang
		Purpose 	: get the normal y angle for the bat
		Parameters 	: 
		Returns 	: the normal y angle for the bat
		Info 		: 
	*/
	inline float getNormalYang(void) const
	{
		return this->normalYang;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBAT::resetYang
		Purpose 	: set the y ang back to normal (eg after a koik)
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	inline void resetYang(void)
	{
		this->setYang(this->normalYang);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBAT::draw
		Purpose 	: draw special stuff the bat
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void draw(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBAT::setXZVelocity
		Purpose 	: set the bat's velocity on the x-z plane
		Parameters 	: x component of velocity, z component of velocity
		Returns 	: 
		Info 		: 
	*/
	void setXZVelocity(const float vx, const float vz);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBAT::respondToMessage
		Purpose 	: respond to a message
		Parameters 	: message string to which to respond
		Returns 	: true if the message was recognised and handled, else false
		Info 		: 
	*/
	bool respondToMessage(const char* const message);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBAT::update
		Purpose 	: do the thing that scares (deflects) the ball (elephant)
		Parameters 	: 
		Returns 	: 
		Info 		: Taz shouts at the ball, Gossamer either shouts or causes an earthquake
	*/
	void scare(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBAT::setState
		Purpose 	: set the bat's state
		Parameters 	: state to set (ZBBATSTATE_...)
		Returns 	: 
		Info 		: NOTE TO SELF: always use setState methods! You know it makes sense!
	*/	
	void setState(const ZBBatState stateIn);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBAT::update
		Purpose 	: update the bat
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBAT::isDeflecting
		Purpose 	: find out if the bat is ready to deflect a ball
		Parameters 	: the ball against which to test for deflection
		Returns 	: 
		Info 		: 
	*/
	inline bool isDeflecting(const struct TAG_ZBBALL& ball);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBAT::getHit
		Purpose 	: get hit by a ball (when not prepared for it)
		Parameters 	: ball by which to get hit
		Returns 	: 
		Info 		: 
	*/
	void getHit(const struct TAG_ZBBALL& ball);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBAT::getKnocked
		Purpose 	: get knocked away by the elephant's side-belly when skidding to a halt,
						or by its rear hooves as it starts  running away
		Parameters 	: (opt/NULL) ball by which to get hit
		Returns 	: 
		Info 		: 
	*/
	void getKnocked(const struct TAG_ZBBALL* const ball=NULL);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBAT::getProdded
		Purpose 	: get prodded by the elephant's tusks as it skids to a halt (its head is in a lowered position then)
		Parameters 	: ball by which to get hit
		Returns 	: 
		Info 		: 
	*/
	void getProdded(const struct TAG_ZBBALL& ball);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBAT::lookAt
		Purpose 	: tell the bat what to look at
		Parameters 	: ptr to ACTORINSTANCEUSER to look at
		Returns 	: 
		Info 		: 
	*/
	void lookAt(ACTORINSTANCEUSER* const target)
	{
		this->characterToLookAt=target;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBAT::shutdown
		Purpose 	: shutdown
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void shutdown(void)
	{
		// PP: SHUTDOWN BOOK
		BookList_closeAndShutdown(this->book);
	}



}ZBBAT, _ZBBAT BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
															ZBBALL

									This is the 'ball' (Elephant) used in the Zoo Boss game

															@ZBBALL
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: types of ball, cos if we have time it could be funny to try different ones as cheats
enum ZBballType
{
	ZBBALLTYPE_ELEPHANT,

	NUM_ZBBALL_TYPES
};

// PP: states for the ball
enum ZBballState
{
	ZBBALLSTATE_NONE,					// PP: no state
	ZBBALLSTATE_INTRODROP1,				// PP: elephant is dropping
	ZBBALLSTATE_INTROID,				// PP: freeze-frame latin id thingy	
	ZBBALLSTATE_INTRODROP2,				// PP: elephant is dropping
	ZBBALLSTATE_INTROLAND,				// PP: elephant lands after dropping
	ZBBALLSTATE_INTROSHAKE,				// PP: elephant is shaking itself in recovery from the fall
	ZBBALLSTATE_INTROHOOT,				// PP: elephant is hooting - you know, like they do.  "Hoot-hoot!" you know.  Little cats hoot as well.  "Hoot-hoot!"  // PP: this acts as an additional warning that the game is about to start
	ZBBALLSTATE_RUNNING,				// PP: duh...run-ning?
	ZBBALLSTATE_SKIDDING,				// PP: duh...skid-ding
	ZBBALLSTATE_JUMPING,				// PP: jumpin' jumpin'
	ZBBALLSTATE_AWAITINGSTOMP,			// PP: waiting for gossamer to hit the ground in a stomp attack
	ZBBALLSTATE_QUAKING,				// PP: duh...quak-ing
	ZBBALLSTATE_WALLBOUNCING,			// PP: koiking off one of the bats' protected walls
	ZBBALLSTATE_GOAL,					// PP: waiting in the air after a goal has been scored
	ZBBALLSTATE_GOALDROP,				// PP: dropping into the court again after a goal has been scored

	NUM_ZBBALL_STATES					// PP: number of states for the ball
};

#define ZBBALL_INITIAL_STATE		ZBBALLSTATE_INTROSHAKE// PP: ZBBALLSTATE_RUNNING

enum
{
	ZBBALLFLAG_NONE								=0,
	ZBBALLFLAG_CRAZY							=1,				// PP: not used
	ZBBALLFLAG_TURNING							=1<<1,			// PP: turning towards dest Y angle
	ZBBALLFLAG_NEWDIRECTION						=1<<2,			// PP: direction has changed - good time for AI to recalculate intercept point
	ZBBALLFLAG_NEWSTATE							=1<<3,			// PP: ball's state has newly changed - game should handle this and then set stateChanged to false
	ZBBALLFLAG_NEWLYENTEREDCONTACTCOURT			=1<<4,			// PP: newly entered contact court
};

// PP: elephant's feet - corresponds with stepping order (apart from _ALL of course)
enum
{
	ZBBALLFOOT_REARLEFT,
	ZBBALLFOOT_FRONTLEFT,
	ZBBALLFOOT_REARRIGHT,
	ZBBALLFOOT_FRONTRIGHT,
	ZBBALLFOOT_ALL
};

// PP: actions that can be performed by the elephant's eyes
enum ZBBallEyeAction
{
	ZBBALLEYES_OPEN,
	ZBBALLEYES_CLOSE,
	ZBBALLEYES_BLINK,
};

// PP: animated texture for the cutscene elephant's eyes
#define ZBBALL_CUTSCENE_EYE_TEXTURE			"elephant_eyeblink.gif"

// PP: animated texture for the in-game elephant's eyes
#define ZBBALL_GAME_EYE_TEXTURE				"textures\\elephant_eyeblink.gif"

#define ZBBALL_INITIAL_FLAGS				(ZBBALLFLAG_NEWDIRECTION|ZBBALLFLAG_NEWSTATE)

#define ZBBALL_INNERCOLLRADIUS_ELEPHANT		METERTOUNIT(2.0f)		// PP: the one that squashes you
#define ZBBALL_OUTERCOLLRADIUS_ELEPHANT		METERTOUNIT(2.75f)		// PP: the one that knocks you back
#define ZBBALL_TUSKCOLLRADIUS_ELEPHANT		METERTOUNIT(4.5f)		// PP: the one that prods you

#define ZBBALL_FAST							(this->speedFraction > 0.5f)
#define ZBBALL_SLOW							(!ZBBALL_FAST)

#define ZBBALL_INTROID_HEIGHT				(ZB_COURT_HEIGHT+METERTOUNIT(1.5f))

#define ZBBALL_MAX_TURN_SPEED				6.0f
#define ZBBALL_MIN_TURN_SPEED				1.5f

typedef struct TAG_ZBBALL:	public ACTORINSTANCEUSER
{

private:

	float						srcYang;										// PP: angle before turn starts
	float						destYang;										// PP: destination Y angle
																				
	// PP: wall contact stuff...												
	bool						TBwallContactLatch;								
	bool						RLwallContactLatch;								
																				
	// PP: animation/movement stuff...											
	float						runPhase;										// PP: point in the run cycle (0..1)
																				
	float						timer;											// PP: general purpose timer
	float						duration;										// PP: general purpose duration member - ye can't get away from it
																				
	//float						markedTime;										// PP: general purpose record of a timer value at a certain time
																				
	static TBTexture*			footprintTexture;								// PP: decal texture for footprints

	static float				initialSpeedFractions[NUM_DIFFICULTY_LEVELS];	// PP: speed fractions at which the ball starts in each of the different difficulty levels

	
	struct
	{
		TBActorAnimSegment*		walk;
		TBActorAnimSegment*		run;
								
		TBActorAnimSegment*		jump;					// PP: response to a shout scare, while ball is at low speed
		TBActorAnimSegment*		skidJump;				// PP: response to a shout scare, while ball is at high speed
		TBActorAnimSegment*		quake;					// PP: response to a stomp scare
								
		TBActorAnimSegment*		wallKoik;				// PP: hitting and breaking a protected wall
								
	}anims;						
								
	TBVector2D					hearingOffset;			// PP: offset from centre of ball to centre of hearing circle
				
	TBActorNodeInstance*		headBone;				// PP: Heheh, you said "head" AND "bone", heheh


	ZBBatScareType				scareType;				// PP: type of scare being done to elephant

	// PP: eye control stuff...

	float						blinkTimer;				// PP: downwards timer till end of blink
	float						interBlinkTimer;		// PP: downwards timer till next blink
	
public:

	TBCollisionAABB				towerBox;				// PP: the collision box representing the watch tower in which Sam stands. This is used to block the sun from the elephant.
	TBCollisionRay				towerRay;				// PP: the ray used to check if the elephant is shaded by the watch tower
	
	TBTexture*					eyeTexture;				// PP: blinking eye texture used on the ball
	
	// PP: was private
	TBVector					headPos;				// PP: faur 'Is Gudjie's heed is like

	TBActorNodeInstance*		cutsceneNode;			// PP: motion bone of cutscene elephant

	RECTANGLE					court;					// PP: legal area for the ball to be in (the entire game court)
	RECTANGLE					contactCourt;			// PP: rectangle marking the wall contact margins of the court
	bool						inContactCourt;			// PP: was the ball inside its contact court last update?

	struct
	{
		bool					done;					// PP: has this step been done this cycle?
		TBActorNodeInstance*	node;					// PP: what foot node corresponds to this step?
		TBVector				pos;					// PP: position of the bone in world space - gets updated every time it's used
								
	}steps[4];
								
	float						speedFraction;			// PP: speed level 0..1 (min..max)
								
	const float*				koikWall;				// PP: pointer to one of the coords in the ball's court rect, representing the wall that the ball is koiking off of
								
	float						speed;					// PP: actual speed (distance units/second)
														
	ZBballState					state;					// PP: state of the ball
	ZBballState					oldState;				// PP: previous state of the ball
								
	int							flags;					// PP: flags for the ball
								
	float						innerCollRadius;		// PP: inner collision radius of the ball (the one that squashes you)
	float						outerCollRadius;		// PP: outer collision radius of the ball (the one that knocks you back)
	float						tuskCollRadius;			// PP: tusk collision radius of the ball (the one that prods you)

	// PP: bat contact stuff...
	bool						batInnerCollLatch[BOSS_NUM_PLAYERS];
	bool						batOuterCollLatch[BOSS_NUM_PLAYERS];
	bool						batTuskCollLatch[BOSS_NUM_PLAYERS];
	bool						batHearLatch[BOSS_NUM_PLAYERS];
	bool						batScareLatch[BOSS_NUM_PLAYERS];


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBALL::init
		Purpose 	: initialise the ball
		Parameters 	: position, angle, court, (opt/ZBBALLTYPE_ELEPHANT) type for ball
		Returns 	: 
		Info 		: 
	*/
	void init(TBVector pos, const float ang, const RECTANGLE& courtIn, const ZBballType type=ZBBALLTYPE_ELEPHANT);



	/*	--------------------------------------------------------------------------------
		Function 	: ZBBALL::initPreCutscene
		Purpose 	: initialisation before the intro cutscene
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void initPreCutscene(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBALL::setState
		Purpose 	: set the state of the ball
		Parameters 	: (opt/ZBBALLTYPE_ELEPHANT) type for ball
		Returns 	: 
		Info 		: 
	*/
	void setState(const ZBballState newState);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBALL::draw
		Purpose 	: draw special stuff the ball
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void draw(void) const;


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBALL::shutdown
		Purpose 	: shutdown the ball
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void shutdown(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBALL::respondToMessage
		Purpose 	: respond to a message
		Parameters 	: ptr to ACTORINSTANCE generating the message, message string to which to respond
		Returns 	: true if the message was recognised and handled, else false
		Info 		: 
	*/
	bool respondToMessage(const ACTORINSTANCE* const messageActorInstance, const char* const message);
		

	/*	--------------------------------------------------------------------------------
		Function 	: ZBBALL::quake
		Purpose 	: quake in response to Gossamer's stomp hitting the ground
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void quake(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBALL::beDeflected
		Purpose 	: be deflected by a bat
		Parameters 	: bat by which to be deflected
		Returns 	: 
		Info 		: 
	*/
	void beDeflected(const struct TAG_ZBBAT& bat);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBALL::innerCollide
		Purpose 	: check for inner collision with a bat (the type that'd squash you if you weren't scaring)
		Parameters 	: bat with which to check for collision
		Returns 	: 
		Info 		: 
	*/
	bool innerCollide(const ZBBAT& bat) const;


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBALL::outerCollide
		Purpose 	: check for outer collision with a bat (the type that'd knock you back while you're scaring)
		Parameters 	: bat with which to check for collision
		Returns 	: 
		Info 		: 
	*/
	bool outerCollide(const ZBBAT& bat) const;


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBALL::tuskCollide
		Purpose 	: check for tusk collision with a bat (the type that'd prod you)
		Parameters 	: bat with which to check for collision
		Returns 	: 
		Info 		: 
	*/
	bool tuskCollide(const ZBBAT& bat) const;


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBALL::hear
		Purpose 	: check if the ball can hear a bat (assuming the bat is scaring)
		Parameters 	: bat with which to check for hearing
		Returns 	: 
		Info 		: 
	*/
	bool hear(const ZBBAT& bat);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBALL::applyNormalMovement
		Purpose 	: move the ball in the way that it moves while running or rebounding from a wall
		Parameters 	: angle of movement
		Returns 	: 
		Info 		: 
	*/
	void applyNormalMovement(const float yAng);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBALL::testWallCollsions
		Purpose 	: check for, and respond to, collisions of the ball with the court walls
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void testWallCollisions(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBALL::footstep
		Purpose 	: elephant footstep, bleh, i'm tired
		Parameters 	: ptr to ACTORINSTANCE doing the footstep (could be cutscene elephant),
						which foot/feet (ZBBALLFOOT_), , (opt/false)is this footstep part of a ZBBALLFOOT_ALL
		Returns 	: 
		Info 		: 
	*/
	void footstep(const ACTORINSTANCE* const footstepActorInstance, const int foot, const bool all=false);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBALL::update
		Purpose 	: update the ball
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBALL::finishedIntro
		Purpose 	: find out if the ball has finished its intro sequence
		Parameters 	: whether or not the ball has finished its intro sequence
		Returns 	: 
		Info 		: 
	*/
	inline bool finishedIntro(void)
	{
		return (this->state >ZBBALLSTATE_INTROHOOT);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBALL::startMove
		Purpose 	: set the ball moving
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	inline void startMove(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBALL::getTurnSpeed
		Purpose 	: get the speed at which the elephant will rotate when it brushes a side wall,
						given its current forward speed
		Parameters 	: 
		Returns 	: the speed at which the elephant will rotate when it brushes a side wall,
						given its current forward speed
		Info 		: 
	*/
	inline float getTurnSpeed(void) const
	{
		return ZBBALL_MIN_TURN_SPEED+(this->speedFraction*(ZBBALL_MAX_TURN_SPEED-ZBBALL_MIN_TURN_SPEED));
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ZBBALL::controlEyes
		Purpose 	: control the ball's eyes
		Parameters 	: eye action to perform (ZBBALLEYES_)
		Returns 	: 
		Info 		: 
	*/
	void controlEyes(const ZBBallEyeAction action);
	

}ZBBALL, _ZBBALL BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
															ZBSAM

				Yosemite Sam, observing the match from his lookout tower and no doubt shouting a lot and getting angry
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: sam's phrases
enum ZBSamSpeech
{
	ZBSAMSPEECH_ANGRY1,				// PP: I said SCARE the elephant, not invite him to dinner!
	ZBSAMSPEECH_ANGRY2,				// PP: Come on! Get that critter!

#define ZBSAM_NUM_ANGRY_PHRASES		2

	ZBSAMSPEECH_TAUNT1,				// PP: You're a-going DOWN, devil!
	ZBSAMSPEECH_TAUNT2,				// PP: You're as bad as that pesky duck...

#define ZBSAM_NUM_TAUNT_PHRASES		2

	ZBSAMSPEECH_FASTBALL,			// PP: That elephant sure can pong!

	ZBSAM_NUM_PHRASES
};

// PP: sam's animations
enum ZBSamAnim
{
	ZBSAMANIM_IDLE,					// PP: breathe// PP: worth head-tracking or something?// PP: maybe just turn him

	ZBSAMANIM_LOSE1,				// PP: slap face, shake head
	ZBSAMANIM_LOSE2,				// PP: jump in air kicking & punching
	ZBSAMANIM_LOSE3,				// PP: twang moustache
	ZBSAMANIM_LOSE4,				// PP: jump on hat

#define ZBSAM_NUM_LOSE_ANIMS		4

	ZBSAMANIM_WIN1,					// PP: belly laugh
	ZBSAMANIM_WIN2,					// PP: throw hat

#define ZBSAM_NUM_WIN_ANIMS			2

	ZBSAM_NUM_ANIMS
};


typedef struct TAG_ZBSAM			:		public ACTORINSTANCEUSER
{
	// PP: sample names for sam's phrases
	static char*					speechSamples[ZBSAM_NUM_PHRASES];

	// PP: anim names for sam's anims
	static char*					anims[ZBSAM_NUM_ANIMS];

	// PP: cutscene sam
	ACTORINSTANCE*					cutsceneActorInstance;
	
	// PP: motion bone of cutscene sam
	TBActorNodeInstance*			cutsceneNode;


	/*	--------------------------------------------------------------------------------
		Function 	: ZBSAM::playAnim
		Purpose 	: play an animation
		Parameters 	: ZBSAMANIM_ anim enum, (opt/false) whether or not to loop, (opt/1.0f)speed
		Returns 	: 
		Info 		: this overloads ACTORINSTANCEAUSER::playAnim, to take an anim enum
	*/
	void playAnim(const ZBSamAnim anim,  const bool loop=false, const float speed=1.0f)
	{
		ACTORINSTANCEUSER::playAnim(this->anims[anim], loop, speed);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ZBSAM::say
		Purpose 	: say a line of dialogue
		Parameters 	: 
		Returns 	: 
		Info 		: "Sam might saaayyy....."
	*/
	void say(const ZBSamSpeech line)
	{
		this->playSound(TAG_ZBSAM::speechSamples[line]);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ZBSAM::init
		Purpose 	: initialisation at beginning of boss game
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void init(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBSAM::initPreCutscene
		Purpose 	: initialisation before the intro cutscene
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void initPreCutscene(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBSAM::update
		Purpose 	: per-frame update
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBSAM::win
		Purpose 	: react to gossamer scoring a goal
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void win(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ZBSAM::lose
		Purpose 	: react to gossamer losing a goal
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void lose(void);


}ZBSAM, _ZBSAM BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														ZOOBOSSGAME

					This is the BOSSGAME type for the Zoo Boss - Gossamer vs. Taz in an Elephant Pong showdown
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: states for the zoo boss game
enum
{
	ZBSTATE_NONE,			// PP: no state
	ZBSTATE_GOAL,			// PP: sequence started when a goal is scored

	NUM_ZBSTATES
};

#define ZB_HEARING_RANGE					METERTOUNIT(50.0f)// PP: ?

// PP: point awards for one-player mode
enum ZBPointAward
{
	ZBPOINTS_SCARE		=10,		// PP: number of points earned by a successful scare
	ZBPOINTS_GOAL		=50			// PP: number of points earned by a winning a goal
};

typedef struct TAG_ZOOBOSSGAME		:		public BOSSGAME
{

	// PP: "I'll be there for yoooo...."

	friend						ZBBAT;					// PP: These are the 'bats' (Taz & Gossamer) used in the Zoo Boss game
	friend						ZBBALL;					// PP: This is the 'ball' (Elephant) used in the Zoo Boss game
	friend						ZBSAM;					// PP: Yosemite Sam, observing the match from his lookout tower and no doubt shouting a lot and getting angry

	// PP: create pong dust for taz's spin in the cutscene
	friend bool ZooBoss_createCutsceneSpinDust(const VEC& pos);
								
private:						
								
	int							ZBstate;				// PP: the state of the zoo boss game		
	RECTANGLE					court;					// PP: the playing area of the zoo boss game	// PP: NOTE!!!: this is stored in such a way that the bats are on the LEFT & RIGHT sides of the court, not TOP & BOTTOM!
	ZBBAT						bats[BOSS_NUM_PLAYERS];	// PP: the 'bats' (Taz & Gossamer) for the zoo boss game
	ZBBALL						ball;					// PP: the 'ball' (elephant) for the zoo boss game
	EZooBossCamera				camShot;				// PP: PPOOIICH! heheh
	CAMERACASE*					debugCameraCase;		// PP: debug camera case for the zoo boss game
	int							score;					// PP: score (in one-player)
	TEXTBOX*					scoreTextbox;			// PP: textbox used to display score (in one-player)
	static TBActorInstance*		shadowZMask;
	float						delay;					// TP: delay used for the end of game
	ZBSAM						sam;					// PP: Yosemite Sam, observing the match from his lookout tower and no doubt shouting a lot and getting angry
	
	VEC							outroCamPos[2];			// PP: positions for outro cameras, one per player
	VEC							outroCamLookat[2];		// PP: positions for outro cameras, one per player


	/*	--------------------------------------------------------------------------------
		Function 	: ZOOBOSSGAME::getMaxScore
		Purpose 	: get the maximum score for the game with it's current number of players
		Parameters 	: 
		Returns 	: the maximum score for the game with it's current number of players
		Info 		: 
	*/
	inline int getMaxScore(void) const
	{
		return (this->twoPlayer?ZB_MAX_SCORE_2PLAYER:ZB_MAX_SCORE_1PLAYER);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ZOOBOSSGAME::initScoreBook
		Purpose 	: initialise the book for overlays such as score readouts
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOSSGAME::initScoreBook
	*/
	void initScoreBook(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ZOOBOSSGAME::initMessageBook
		Purpose 	: initialise the book for messages such as "GET READY..." or "GOAL!"
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOSSGAME::initMessageBook
	*/
	void initMessageBook(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ZOOBOSSGAME::initInstructionBook
		Purpose 	: initialise the book for instructions such as "press X to start"
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOSSGAME::initInstructionBook
	*/
	void initInstructionBook(void);

	/*	--------------------------------------------------------------------------------
		Function 	: ZOOBOSSGAME::cameraLookAtBat
		Purpose 	: point the camera at a bat
		Parameters 	: index of bat at which to look
		Returns 	: 
		Info 		: 
	*/
	void cameraLookAtBat(const int index);


	/*	--------------------------------------------------------------------------------
		Function 	: ZOOBOSSGAME::setCamera
		Purpose 	: set the camera shot
		Parameters 	: shot to set
		Returns 	: 
		Info 		: 
	*/
	void setCamera(const EZooBossCamera cam);

	
	
	/*	--------------------------------------------------------------------------------
		Function 	: ZOOBOSSGAME::groundPoundCameraShake
		Purpose 	: shake the camera in response to something pounding the ground
		Parameters 	: position of impact, strength of pound, (opt/0.0f)maximum random deviation (absolute)
		Returns 	: 
		Info 		: FOR GOD'S SAKE, STRAP YOURSELVES DOWN!!
	*/
	void groundPoundCameraShake(TBVector pos, const float strength, const float maxDeviation=0.0f);


public:


	/*	--------------------------------------------------------------------------------
		Function 	: ZOOBOSSGAME::updateCamera
		Purpose 	: update the Zoo boss game's camera or cameras
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void updateCamera(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ZOOBOSSGAME::win
		Purpose 	: win the boss game
		Parameters 	: (opt/0)player to win
		Returns 	: 
		Info 		: this overrides BOSSGAME::win
	*/
	void win(const int weiner=0);


	/*	--------------------------------------------------------------------------------
		Function 	: ZOOBOSSGAME::lose
		Purpose 	: lose the boss game
		Parameters 	: (opt/0)player to lose
		Returns 	: 
		Info 		: this overrides BOSSGAME::lose
	*/
	void lose(const int luzer=0);


	/*	--------------------------------------------------------------------------------
		Function 	: ZOOBOSSGAME::getShadowZMask
		Purpose 	: get a ptr to the actor instance to use to z-mask character shadows in the Zoo Bawss game
		Parameters 	: PARAMETERS?!!		*I'LL* GIVE YOU PARAMETERS!!  I'LL *PARAMETER* YOU SUNSHINE!!
		Returns 	: a ptr to the actor instance to use to z-mask character shadows in the Zoo Bawss game!
		Info 		: 
	*/
	static inline TBActorInstance* getShadowZMask(void)
	{
		return TAG_ZOOBOSSGAME::shadowZMask;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ZOOBOSSGAME::setState
		Purpose 	: switch the game into a new state
		Parameters 	: the state into which to switch
		Returns 	: 
		Info 		: this overrides BOSSGAME::setState
	*/
	void setState(const int newState);


	/*	--------------------------------------------------------------------------------
		Function 	: ZOOBOSSGAME::initPreCutscene
		Purpose 	: initialisation for the Zoo Boss game, before the intro cutscene has run
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOSSGAME::initPreCutscene
	*/
	void initPreCutscene(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ZOOBOSSGAME::init
		Purpose 	: initialise the Zoo Boss game
		Parameters 	: rectangle for playing area
		Returns 	: 
		Info 		: this overrides BOSSGAME::init
	*/
	void init(const RECTANGLE &courtIn=RECTANGLE(-ZB_COURT_LENGTH*0.5f, ZB_COURT_LENGTH*0.5f, -ZB_COURT_WIDTH*0.5f, ZB_COURT_WIDTH*0.5f));


	/*	--------------------------------------------------------------------------------
		Function 	: ZOOBOSSGAME::update
		Purpose 	: update the Zoo Boss game
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOSSGAME::update
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ZOOBOSSGAME::draw2
		Purpose 	: draw the zoo boss game alpha effects
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOSSGAME::draw2
	*/
	void draw2(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ZOOBOSSGAME::shutdown
		Purpose 	: shut-down the Zoo Boss game
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOSSGAME::shutdown
	*/
	void shutdown(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ZOOBOSSGAME::respondToMessage
		Purpose 	: handle a message from the ball or a bat
		Parameters 	: pointer to actor instance, pointer to message string
		Returns 	: true if the message was recognised and handled, else false
		Info 		: 
	*/
	bool respondToMessage(const ACTORINSTANCE* const actorInstance, const char* const message);


	/*	--------------------------------------------------------------------------------
		Function 	: ZOOBOSSGAME::spitCollide
		Purpose 	: respond to collision with a spat object
		Parameters 	: projectile, victim
		Returns 	: 
		Info 		: 
	*/
	static int spitCollide(ACTORINSTANCE *collider, ACTORINSTANCE *object);


	/* --------------------------------------------------------------------------------
	   Function		: ZOOBOSSGAME::fadeDownCallback
	   Purpose		: function called when the fade-down, before the continue sequence, finishes
	   Parameters	: 
	   Returns		: 
	   Info			: // PP: must match the BossGameFadeDownCallback typedef (bossgamecontinue.h)
	*/
	static void fadeDownCallback(void);


	/* --------------------------------------------------------------------------------
	   Function		: ZOOBOSSGAME::resetCallback
	   Purpose		: function called when the player selects the option to continue or to quit
	   Parameters	: context value passed through from ProcessTazDeathAtEndOfBossGame,
						the player's choice of action (BOSSGAMERESET_CONTINUE, BOSSGAMERESET_QUIT)
	   Returns		: big boobs
	   Info			: // PP: must match the BossGameResetCallback typedef (bossgamecontinue.h)
	*/
	static int32 resetCallback(void *context, EBossGameResetChoice choice);


	/*	--------------------------------------------------------------------------------
		Function 	: ZOOBOSSGAME::ruleBookOpenFunc
		Purpose 	: page-open func for zooboss page of the rules paperbook
		Parameters 	: ptr to the page, context value
		Returns 	: 
		Info 		: // PP: Must match the PageFunc typedef (TextFX.h)
	*/
	static void ruleBookOpenFunc(struct TAG_PAGE* const page, const int32 context);


	/*	--------------------------------------------------------------------------------
		Function 	: ZOOBOSSGAME::prepareForPause
		Purpose 	: make preparations for entering pause mode while in the Zoo Boss game
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void prepareForPause(void);


}ZOOBOSSGAME, _ZOOBOSSGAME BALIGN16;

extern ZOOBOSSGAME					ZooBoss_game;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													PUBLIC MAIN FUNCTIONS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: ZooBoss_introIDpauseUpdateFunc
	Purpose 	: update function for the custom pause state 
	Parameters 	: long
	Returns 	: 
	Info 		: must match the PauseUpdateFunc typedef (main.h).
*/
void ZooBoss_introIDpauseUpdateFunc(const int32 parameter);


/*	--------------------------------------------------------------------------------
	Function 	: ZooBoss_createCutsceneSpinDust
	Purpose 	: create pong dust for taz's spin in the cutscene
	Parameters 	: position at which to create dust
	Returns 	: true if pong dust was appropriate, else false (use normal spin dust)
	Info 		: // PP: only call this on Zoo Boss cutscene
*/
bool ZooBoss_createCutsceneSpinDust(const VEC& pos);


#endif // PP: ndef __H_ZOOBOSS