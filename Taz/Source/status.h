#ifndef __H_STATUS
#define __H_STATUS

#include "swingingobjects.h"
#include "water.h"
#include "camera.h"
#include "actors.h"
#include "costumes.h"

#define WATER_SLIDE_MIN_VELOCITY	(METERTOUNIT(10.0f))

// JW: defines for destruction game effects on players
#define PLAYEREFFECTNORMAL	(0)
#define PLAYEREFFECTDOUBLE	(1)
#define PLAYEREFFECTSLOMO	(2)

enum ECharState
{
	STATE_MOVE,
	STATE_SKIDSTOP,
	STATE_TIPTOE,
	STATE_SLIDE,
	STATE_GETUPFROMSLIDE,
	STATE_GETUPFROMWATER,
	STATE_BIGFALL,
	STATE_SPLAT,
	STATE_JUMP,
	STATE_FALL,
	STATE_IDLE,
	STATE_BITE,
	STATE_SPINUP,
	STATE_SPIN,
	STATE_SPINDOWN,
	STATE_SPINDOWNONWATER,
	STATE_RECOVER,
	STATE_COLLECTPOSTCARD,
	STATE_HOLDINGPOSTCARD,
	STATE_DESTROYPOSTCARD,
	STATE_KOIKFROMWATER,
	STATE_PROJECTILE,
	STATE_PROJECTILESLIDE,
	STATE_SWINGING,
	STATE_SPRUNG,
	STATE_VEHICLE,
	STATE_TRAPPED,
	STATE_DEAD,
	STATE_DONOTHING,
	STATE_ELECTROCUTED,
	STATE_GROUNDELECTROCUTED,
	STATE_ONICE,
	STATE_SPINONICE,
	STATE_WATERSLIDE,
	STATE_PLAYANIMATION,
	STATE_SCARE,
	STATE_ENTERINGPHONEBOX,
	STATE_ONFOUNTAIN,
	STATE_FRONTENDUSE,			/* PP: using a front-end item, eg. setting a volume level: Taz himself can't be controlled in this state*/
	STATE_GOTOSLEEP,			
	STATE_SLEEP,
	STATE_DANCE,
	STATE_DEBUGMOVE,
	STATE_SQUASHED,
	STATE_CATATONIC,
	STATE_CATATONICPHYS,		/* CMD state catatonic but with physics */
	STATE_MOVESQUASHED,
	STATE_SHOCKED,
	STATE_CATATONICDELAY,
	STATE_INTRANSPORT,
	STATE_ATLASSPINUP,
	STATE_ATLASSPIN,
	STATE_ATLASSPINDOWN,
	STATE_ATLASSPHERES,
	STATE_LOOKAROUND,
	STATE_ENTERLOOKAROUND,
	STATE_EAT,
	STATE_SPIT,
	STATE_BUBBLEGUM,
	STATE_CHILLIPEPPER,
	STATE_FRONTEND,
	STATE_RUNON,
	STATE_INIT,
	STATE_NINJAKICK,
	STATE_BURNT,
	STATE_SKATECHARGE,
	STATE_SPLATTED,
	STATE_SPLATRECOVER,
	STATE_SNOWBOARDATTACK,
	STATE_SURFBOARDATTACK,
	STATE_RAPPERATTACK,
	STATE_WEREWOLFATTACK,
	STATE_COWBOYSHOOT,
	STATE_TAZANYELL,
	STATE_INDYROLL,
	STATE_CHEESYATTACK,
	STATE_MESMERISED,
	STATE_ONMINECART,
	STATE_INPORTAL,
	STATE_BADFOOD,
	STATE_ENTERINGXDOOR,
	STATE_MOUSE,
	STATE_BALL,
	STATE_CRATE,
	STATE_CAGED,
	STATE_CAGEDMOVE,
	STATE_SMASH,
	STATE_VEHICLEWATERKOIK,
	STATE_KOIKFROMDEATHPLANE,
	STATE_NET,
	STATE_PLAYANIMANDFREEZE,
	STATE_LOSECOSTUME,
	STATE_WAITFORTEXT,
	STATE_ZAPPEDINTOMOUSE,
	STATE_ZAPPEDINTOBALL,
	STATE_ZAPPEDINTOTAZ,
	// TP: Please update the text version of this in status,cpp
}ENUMOPS(ECharState);

extern char *CharState[];


typedef void (*StateChangeCallback)(struct TAG_ACTORINSTANCE *actorInstance);

typedef struct TAG_ACTOR_STATUS
{
	TBVector		startPosition;				// initial position of character
	TBQuaternion	startQuaternion;			// TP: initial orientation of character
	TBVector		groundBelow;				// pos of ground directly below character
	TBVector		velBeforeJump;
	TBVector		positionBeforeJump;			// position of character before jump
	TBVector		waterResetPoint;
	TBQuaternion	waterResetQuaternion;
	TBQuaternion	splatQuaternion;

	TBVector		stuckCheck;						// TP: used to check the player has not got stuck somewhere

	float			timeWithoutGroundCollision;	// time in seconds since last ground collision occured
	float			timeWithoutGroundCollisionLastFrame;	// time in seconds since last ground collision occured
	float			timeWithoutWallCollision;	// time in seconds since last wall collision occured
	float			timeWithoutWallCollisionLastFrame;	// time in seconds since last wall collision occured

	float			timeWithoutIceCollision;	//time in seconds since last ice collision
	float			timeWithoutWaterSlideCollision;		//time in seconds since last water slide collision
	float			electrocutionTime;			//time in electrocution state
	float			soundloopTimer;				// keep track of looping sound

	//int32			currentState;				// current state of character
	ECharState		currentState;				// current state of character
	ECharState		currentMoveState;			// TP: current state to switch to for movement
	ECharState		lastState;					// CMD: last state before current state
	EWaterType		lastWaterType;

	int32			frontEndSubState;			// CMD: subState within STATE_FRONTEND
	float			speedBeforeJump;			// the current speed of the character when the jump button is pushed
	float			timeInRunAnimation;			// length of time doing run animation
	float			heightAboveGround;			// current height above ground

	float			ageOfHeightTest;			// time in seconds since the last height test
	struct TAG_ACTORINSTANCE		*pathInfoCollidingWith;		// contains the actorInstance of object in contact with
	float			timeSinceCollidingWithPath;
	float			catatonicDelay;					// time left in catatonic state

	struct TAG_TRIGGERINFO	*postCatatonicTrigger;	// TP: message to send after catatonic state is complete
	int				soundHandle[2];					// sound handle for character
	struct TAG_ESCALATORINFO	*escalatorInfoCollWith;	// contains info of last escalator

	float					timeSinceCollidingWithEscalator;
	float					timeGoingUpWaterSlide;
	struct TAG_VEHICLEINFO	*vehicleInfo;			// cantains info for current vehicle
	float			timeWithoutVehicleCollision;	//time in seconds since last vehicle

	float			targetSpinSpeed;				//speedto start spinning
	float			timeInSquashedState;			//time in which taz is in squashed state
	StateChangeCallback		stateRequestCallback;	// function to be called when a state change occurs
	ECharState		stateRequest;					// state to change to

	float			timeSinceLastTazTopCollected;	// like it sounds
	ACTORINSTANCE	*actorAttachedTo;				// actorInstance to which this actorInstance is attached in some way
	float			timeSinceLastTiltingCollision;	// TP: time in seconds since last tilting collision
	float			timeInCurrentState;				// TP: time in seconds the player has been in this state

	bool			firstTimeInState;				// TP: is this the first time in a new state

	union
	{
		ACTORINSTANCE	*owner;						// TP: owner of debris if this is an status structure attached to a debris item
		ACTORINSTANCE	*xDoorInUse;					// TP: pointer to current x door if being used
	};


	struct	TAG_ZZZINFO		*zzzInfo;			// info for characters sleep info
	union{
		bool			costumeDebris;				// TP: used by the debris system to tell if debris came from a costume attack
		int				snoreHandle;
	};
	struct TAG_QUESTIONMARKINFO	*questionMarkInfo;
	float			distanceToClosestMusicbox;
	LOSECOSTUMEINFO				*costumeChangeInfo;

	TBVector		positionLastFrame;			// CPW: there the character was last frame

	float			timeOfLastSpeedBoost;		// CPW: so I can fix stuff

	float			slowDownTime;				// TP: countdown timer for slow down

	char			tutorialBookFlag;			// JW: Can Taz read a tutorial book right now?

	struct										// CPW: multiplayer specific variables
	{
		int					checkPoint;
		int					lap;
		int					realLap;
		float				lapStartTime;

		float				lapTime[4];
		int					lapBestTime;
		ACTORINSTANCE		*pArrow;
		ACTORINSTANCE		*pVehicle;

		int					numPickups;

		TBActorAnimSegment	*pArrowSegment;
		int					oldState;
		int					position;						// CPW: pos in race, (1st 2nd etc)
		float				distToNext;						// CPW: distance to next checkpoint

		float				timeCredit;

		struct TAG_BOOK			*pStatsBook;
		struct TAG_TEXTBOX		*pTextBox[6];

		char					numStars;
		char					thisStar;
		char					lastStar;
		char					thisPlayer;
		char					lastPlayer;
		int					effect;

	}multiplayer;								

} ACTOR_STATUS,_ACTOR_STATUS BALIGN16;

/*	--------------------------------------------------------------------------------
	Function 	: InitActorStatus
	Purpose 	: Initialise an actor status structure
	Parameters 	: ACTOR_STATUS
	Returns 	: 
	Info 		:
*/
void InitActorStatus(ACTOR_STATUS *actorStatus);

/*	--------------------------------------------------------------------------------
	Function 	: RequestStateChange
	Purpose 	: Request a character to change state
	Parameters 	: ACTORINSTANCE, newstate
	Returns 	: 
	Info 		:
*/
int RequestStateChange(ACTORINSTANCE *actorInstance,ECharState newState);

/*	--------------------------------------------------------------------------------
	Function 	: RequestStateChangePlus
	Purpose 	: Request a character to change state and then call function supplied
	Parameters 	: ACTORINSTANCE, newstate, change state function
	Returns 	: 
	Info 		:
*/
int RequestStateChangePlus(ACTORINSTANCE *actorInstance,ECharState newState,StateChangeCallback stateRequestCallback);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateActorStatusInstances
	Purpose 	: Updates the actor status structure
	Parameters 	: ACTORINSTANCELIST
	Returns 	: 
	Info 		:
*/
void UpdateActorStatusInstances(ACTORINSTANCELIST *actorInstanceList);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateActorStatus
	Purpose 	: Updates the actor status
	Parameters 	: ACTORINSTANCE
	Returns 	: 
	Info 		:
*/
void UpdateActorStatus(ACTORINSTANCE *actorInstance);

/*	--------------------------------------------------------------------------------
	Function 	: CalculateSpeedBeforeJump
	Purpose 	: calculates to 2D speed of the actor prior to jump and stores in ACTOR_STATUS
	Parameters 	: ACTORINSTANCE
	Returns 	: 
	Info 		:
*/
void CalculateSpeedBeforeJump(ACTORINSTANCE *actorInstance);

/*	--------------------------------------------------------------------------------
	Function 	: StopCollidingWith
	Purpose 	: stop collision between two actors until they are parted
	Parameters 	: ACTORINSTANCE actor, ACTORINSTANCE object not to collide with
	Returns 	: 
	Info 		:
*/
void StopCollidingWith(ACTORINSTANCE *actorInstance, ACTORINSTANCE *noCollide);

/*	--------------------------------------------------------------------------------
	Function 	: StopCollidingWith
	Purpose 	: sets stop colliding with time
	Parameters 	: ACTORINSTANCE actor, ACTORINSTANCE object not to collide with
	Returns 	: 
	Info 		:
*/
void StopCollidingWith(ACTORINSTANCE *actorInstance, ACTORINSTANCE *noCollide, float time);

/*	--------------------------------------------------------------------------------
	Function 	: SetStartPosition
	Purpose 	: sets the start position for the character
	Parameters 	: ACTORINSTANCE, TBVector position
	Returns 	: 
	Info 		:
*/
void SetStartPosition(ACTORINSTANCE *actorInstance, TBVector startPos, TBQuaternion quat = NULL);

/*	--------------------------------------------------------------------------------
	Function 	: GetHeightOfActor
	Purpose 	: calculates or just returns the current Height of point
	Parameters 	: ACTORINSTANCE, maxAge(how forgiving in seconds over the height information,0 to force new check )
	Returns 	: 
	Info 		: 
*/
float GetHeightOfActor(ACTORINSTANCE *actorInstance, float maxAge);

/*	--------------------------------------------------------------------------------
	Function 	: GetHeightOfPoint
	Purpose 	: calculates or just returns the current Height of point
	Parameters 	: TBVector, flags (PHOP_...)
	Returns 	: 
	Info 		: 
*/
float GetHeightOfPoint(TBVector point, int flags = 0, ESpecialType specialType=(ESpecialType)-1);

/* --------------------------------------------------------------------------------
   Function		: QuickHeightOfPoint
   Purpose		: get the height the ground beneath a point, using a SHADOW struct for the geometry
   Parameters	: point at which to test ground height, shadow whose geometry to use,
					ptr to a float to receive the height of the ground beneath the point
   Returns		: true if some ground was found beneath the point, else false
   Info			: // PP: 
*/
bool QuickHeightOfPoint(const TBVector point, struct TAG_SHADOW* const shadow, float* const groundHeight);

/*  --------------------------------------------------------------------------------
	Function	: ClearStateChangeRequest
	Purpose		: Removes any state request that has been queued
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/

void ClearStateChangeRequest(ACTORINSTANCE *actorInstance);


/*  --------------------------------------------------------------------------------
	Function	: ChangeState
	Purpose		: Change Taz's state
	Parameters	: actorInstance, newState
	Returns		: 
	Info		: 
*/

void ChangeState(ACTORINSTANCE *actorInstance, ECharState newState);

/*  --------------------------------------------------------------------------------
	Function	: ClearStateChangeRequest
	Purpose		: Removes any state request that has been queued
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/

void ClearStateChangeRequest(ACTORINSTANCE *actorInstance);


/*  --------------------------------------------------------------------------------
	Function	: ChangeState
	Purpose		: Change Taz's state
	Parameters	: actorInstance, newState
	Returns		: 
	Info		: 
*/

void ChangeState(ACTORINSTANCE *actorInstance, ECharState newState);


/*  --------------------------------------------------------------------------------
	Function	: EnterState
	Purpose		: Try and get Taz to enter state
	Parameters	: taz instance, state to enter
	Returns		: true if ok, false if not
	Info		: 
*/

bool EnterState(ACTORINSTANCE *actorInstance, ECharState state);

#endif

