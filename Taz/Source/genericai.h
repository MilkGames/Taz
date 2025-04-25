#ifndef __H_GENERICAI
#define __H_GENERICAI

#include "actors.h"
#include "collisioncache.h"
#include "cartoonscaling.h"

#define GENERICAI_DONTCHANGESTATE	(1<<0)
#define GENERICAI_NOBOUNDARY		(1<<1)
#define GENERICAI_IGNORESPECIALCOLL (1<<2)
#define GENERICAI_CHANGEONATTACKEDONLY (1<<3)
#define GENERICAI_NODEACTIVATE (1<<4)

#define GENERICAI_KNOCKEDOUTTIME	(10.0f)

enum EPatternState
{
	PATTERNSTATE_FINISHED,
	PATTERNSTATE_LOOPED,
	PATTERNSTATE_PROCESSINGCOMMAND,
	PATTERNSTATE_COMMANDCOMPLETED,
};

enum EAIState
{
	AISTATE_INACTIVE,
	AISTATE_NEW,
	AISTATE_IDLE,
	AISTATE_AWARE,
	AISTATE_ATTACK,
	AISTATE_DEFEND,
	AISTATE_ATTACKED,
	AISTATE_DEATH,
	AISTATE_OVERRIDE,
	AISTATE_OUTOFRANGE,
	AISTATE_TOOCLOSE,
	AISTATE_COSTUME,
	AISTATE_ATTACKEDOUTOFRANGE,
	AISTATE_RESET,
	AISTATE_POICKOUT,
	AISTATE_POICKIN,
	// TP: Dont forget to update string table in genericai.cpp
};

enum 
{
	SEQ_PLAY,					// TP: SEQ_PLAY,"animname",seconds to loop for or 0 to play until finished, -1 to play forever
	SEQ_PLAYANDFACE,			// TP: SEQ_PLAY,"animname",seconds to loop for or 0 to play until finished, -1 to play forever
	SEQ_TURNLEFT,				// TP: SEQ_TURNLEFT,NULL,number of degrees to turn left
	SEQ_TURNRIGHT,				// TP: SEQ_TURNRIGHT,NULL,number of degrees to turn right
	SEQ_CHASE,					// TP: SEQ_CHASE,"animname",number of instructions to skip if the chase is abandoned
	SEQ_ATTACK,					// TP: SEQ_ATTACK,"animname",0 - perform attack move
	SEQ_ATTACKANDFACE,			// TP: SEQ_ATTACKANDFACE,"animname",0 - perform attack move and continualy faces taz
	SEQ_RETURN,					// TP: SEQ_RETURN,"animname",0 - move back to start position
	SEQ_FACETAZ,				// TP: SEQ_FACETAZ,"animname",offset in degrees + clockwise - turn character to face taz
	SEQ_LOOKAWAYFROMTAZ,		// TP: SEQ_LOOKAWAYFROMTAZ,NULL,0 - turn away from taz
	SEQ_END,					// TP: SEQ_END,NULL,0 - marks end of sequence
	SEQ_WALK,					// TP: SEQ_WALK,"animname",seconds to walk for
	SEQ_LOOP,					// TP: SEQ_LOOP,NULL,0 - loop back to start of sequence
	SEQ_SAVEDIRECTION,			// TP: SEQ_SAVEDIRECTION,NULL,0 - save the current direction of character see below
	SEQ_RESTOREDIRECTION,		// TP: SEQ_RESTOREDIRECTION,NULL,0 - restore the direction previously saved
	SEQ_SAVEPOSITION,			// TP: SEQ_SAVEPOSITION,NULL,0 - save the current position of the character
	SEQ_WALKTOPOSITION,			// TP: SEQ_WALKTOPOSITION,NULL,0 - walk to the position previously saved
	SEQ_WAIT,					// TP: SEQ_WAIT,NULL,number of seconds to pause sequence processing
	SEQ_STUNNED,				// TP: SEQ_STUNNED,"animname",seconds stunned for - puts character in stunned state
	SEQ_SETBLENDTIME,			// TP: SEQ_SETBLENDTIME,NULL,time in seconds for each animation blend
	SEQ_SETROTSPEED,			// TP: SEQ_SETROTSPEED,NULL,rads per second character can rotate at
	SEQ_SETANIMSPEED,			// TP: SEQ_SETANIMSPEED,NULL,current speed of animation default 1.0f
	SEQ_MESSAGE,				// TP: SEQ_MESSAGE,"text message",0 - send message to message callback routine
	SEQ_FACECENTER,				// TP: SEQ_FACECENTER,NULL,0 - turn character to face start position
	SEQ_LOOKAWAYFROMCENTER,		// TP: SEQ_LOOKAWAYFROMCENTER,NULL,0 - turn character to face away from start position
	SEQ_STARS,					// TP: SEQ_STARS,NULL,0 - put stars around characters head
	SEQ_ENDSTARS,				// TP: SEQ_ENDSTARS,NULL,0 - turn off stars
	SEQ_MOTIONBONE,				// TP: SEQ_MOTIONBONE,NULL,TRUE/FALSE - turn on or off motion bone
	SEQ_LOCKSEQ,				// TP: SEQ_LOCKSEQ,NULL,TRUE/FALSE - when TRUE this will stop the generic AI from changing state
	SEQ_BOUNDARY,				// TP: SEQ_BOUNDARY,NULL,TRUE/FALSE - turns on or off the boundary for the character
	SEQ_PLAYSAMPLE,				// TP: SEQ_PLAYSAMPLE,"sample name",loop TRUE/FALSE
	SEQ_STOPSAMPLE,				// TP: SEQ_STOPSAMPLE,NULL,0 - stop sample playing if looping type
	SEQ_STARTZZZ,				// TP: SEQ_STARTZZZ,NULL,0 - start zzz's
	SEQ_ENDZZZ,					// TP: SEQ_ENDZZZ,NULL,0 - end zzz's
	SEQ_STARTQUESTION,			// TP: SEQ_STARTQUESTION,NULL,0 - display a question mark above head
	SEQ_ENDQUESTION,			// TP: SEQ_ENDQUESTION,NULL,0 - stop displaying question marks above head
	SEQ_STARTEXCLAMATION,		// CMD: SEQ_STARTEXCLAMATION,NULL,0 - display an exclamation mark above head
	SEQ_ENDEXCLAMATION,			// CMD: SEQ_ENDEXCLAMATION,NULL,0 - stops displaying an exclamation mark above head
	SEQ_SETCYLRADIUS,			// TP: SEQ_SETCOLLRADIUS,NULL,METERTOUNIT(0.2f) - set the cylinder radius
	SEQ_SETCYLHEIGHT,			// TP: SEQ_SETCOLLHEIGHT,NULL,METERTOUNIT(1.0f) - set the cylinder height
	SEQ_SETSPHRADIUS,			// TP: SEQ_SETSPHRADIUS,NULL,METERTOUNIT(1.0f) - set the sphere radius
	SEQ_SETVIEWANGLE,			// TP: SEQ_SETVIEWANGLE,NULL,view angle (PI/3 for example would allow 60 degrees to the left and right)
	SEQ_PURSUE,					// TP: SEQ_PURSUE,NULL,0 - sets the character in pursuit mode
	SEQ_RESET,					// TP: SEQ_RESET,NULL,0 - returns the character to standard mode and resets the characters position to initial position
	SEQ_DESTROY,				// TP: SEQ_DESTROY,NULL,0 - removes the character completely from the level
	SEQ_ATTACKHOLDDISTANDFACE,	// TP: SEQ_ATTACKHOLDDISTANDFACE,NULL,dist - do attack move and continualy face taz maitaining the distance specified if actual distance is within 1 meter of that value
	SEQ_CHANGESTATEONATTACKEDONLY,	// TP: SEQ_CHANGESTATEONATTACKEDONLY,NULL,TRUE/FALSE - only change state if character is attacked
	SEQ_SKIP,					// TP: SEQ_SKIP,message name,1 - if the message handler returns true this will skip a number of entries in the sequence
	SEQ_ROTATELEFT,				// TP: SEQ_ROTATELEFT,"anim",time rotates to the left at current rot speed for given time doing anim
	SEQ_ROTATERIGHT,			// TP: SEQ_ROTATERIGHT,"anim",time rotates to the right at current rot speed for given time doing anim
	SEQ_FINDNEWCENTER,			// TP: SEQ_FINDNEWCENTER,NULL,0 - find a better center point for the character if there is one
	SEQ_RETURNNEWCENTER,		// TP: SEQ_RETURNNEWCENTER,"anim",0 - return to center while checking to see if a better center is available
	SEQ_NODEACTIVATE,			// TP: SEQ_ALLOWDEACTIVATE,NULL,true/false - allows the character to go inactive
	SEQ_DEATH,					// TP: SEQ_DEATH,NULL,0 - starts the character fading out and then free's it
	SEQ_CYLCOLLISION,			// TP: SEQ_CYLCOLLISION,NULL,true/false - turns the cylinder collision on the character on and off
	SEQ_SPHCOLLISION,			// TP: SEQ_SPHCOLLISION,NULL,true/false - turns the sphere collision on the character on and off
	SEQ_SETSTANDOFF,			// TP: SEQ_SETSTANDOFF,NULL,dist - sets the distance in meters for the standoff range
	SEQ_DISABLED,				// TP: SEQ_DISABLED,"anim",time - disables the character for a given time, also switches collision to sphere and makes character springy
	SEQ_COSTUMEDONE,			// TP: SEQ_COSTUMEDONE,NULL,0 - signifies that the character has completed the costume move
	SEQ_FACECAMERA,				// TP: SEQ_FACECAMERA,NULL,offset - instruct the character to face the camera
	SEQ_DISABLEDANIM,			// TP: SEQ_DISABLEANIM,anim,0 - set the anim to use for disabled bounce moves
	// TP: don't forget to update the string table in genericai.cpp

	SEQ_ENDOFSEQCOMMANDS,
};

enum
{
	ATTACKTYPE_BITEFRONT,
	ATTACKTYPE_BITEREAR,
	ATTACKTYPE_SPINFRONT,
	ATTACKTYPE_SPINREAR,
	ATTACKTYPE_SCAREFRONT,
	ATTACKTYPE_SCAREREAR,
	ATTACKTYPE_COSTUMEFRONT,
	ATTACKTYPE_COSTUMEREAR,
	ATTACKTYPE_SPITFRONT,
	ATTACKTYPE_SPITREAR,
	ATTACKTYPE_FLATTENED,
};

enum 
{
	AIMODE_NORMAL,
	AIMODE_ATTACKED,
	AIMODE_PURSUE,
	AIMODE_FLATTENED,
};

enum
{
	SEQMODE_NORMAL,
	SEQMODE_PAUSE,
};

typedef struct TAG_SEQ
{
	int		type;
	char	*anim;
	float	time;
} SEQ;

typedef int (*RequestPattern)(struct TAG_ACTORINSTANCE *actorInstance);

typedef int (*AttackCallback)(struct TAG_ACTORINSTANCE *actorInstance);

typedef int (*MessageHandler)(struct TAG_ACTORINSTANCE *actorInstance,char *message);

typedef void (*AIChangeState)(struct TAG_ACTORINSTANCE *actorInstance,int state,int lastState);

typedef void (*FrameCallback)(struct TAG_ACTORINSTANCE *actorInstance);

typedef void (*Draw)(struct TAG_ACTORINSTANCE *actorInstance);

typedef void (*FreeCallback)(struct TAG_ACTORINSTANCE *actorInstance);

typedef bool (*ChangeAIState)(struct TAG_ACTORINSTANCE *actorInstance);

typedef void (*SpecialCallback)(struct TAG_ACTORINSTANCE *actorInstance, struct TAG_ACTORINSTANCE *special);

typedef struct TAG_AISAVEINFO
{
	float		currentBlendTime;	// current blend time to use for animations
	float		currentRotTime;		// current time to perform rotate moves
	float		currentAnimSpeed;	// current speed for animations
	float		collCylinderHeight;	// TP: the height of the coll cylinder
	float		collCylinderRadius;	// TP: radius of the coll cylinder
	float		collSphereRadius;	// TP: the radius of the coll sphere
	float		standOff;			// TP: stand off range
} AISAVEINFO;

typedef struct TAG_MOVEMENTBOX
{
	BOUNDINGBOX		bbox;			// TP: axis aligned bounding box
	BOUNDINGBOX		attackBBox;		// TP: max box for attacking
	BOUNDINGBOX		idleBox;		// TP: bounding box for activating character
	TBMatrix		matrix;			// TP: matrix of the bounding box
	TBMatrix		invmatrix;		// TP: matrix to convert real world point into axis aligned space
} MOVEMENTBOX;

typedef struct TAG_GENERICAI
{
	// TP: save info
	AISAVEINFO	save;				// TP: structure to save information that should not be carried between sequences

	// user parms
	TBVector	orgCenter;			// TP: center as specified on the map
	TBVector	center;			// center of the characters perimeter
	float		deactivationRange;	// distance from center at which taz activates the enemy
	float		awarenessRange;		// maximum range from center that the character is aware of Taz
	float		movementRange;		// maximum range of characters movement from center
	int			context;

	// internal parms
	TBVector	toTaz;			// vector representing char->taz
	TBVector	toCenter;		// vector representing char->center
	TBVector	tazToCenter;	// vector representing taz->center
	TBVector	faceVector;		// vector representing the direction the character is facing (normalized)

	TBQuaternion	savedOrientation;	// for the SEQ_SAVEDIRECTION command
	TBVector		savedPosition;		// for the SEQ_SAVEPOSITION command
	EAIState	state;			// current state of character
	EAIState	lastState;		// state of character last AI round
	float		distToCenter;	// current distance to center of perimiter
	float		distToTaz;		// current distance to Taz
	float		distTazToCenter;// current distance from taz to the center of perimeter
	float		facing;			// current facing angle
	float		facingRelToTaz; // current facing relative to Taz ( 0 - facing Taz 90 -looking right)
	int			currentMode;	// current mode of character, NORMAL, ATTACKING, ATTACKED
	ACTORINSTANCELIST	*activeList,*inactiveList;
	SEQ			*currentSeq;	// current seq for character to follow
	int			seqIdx;			// current seq index
	float		seqTime;		// current time performing seq entry
	float		standOffRange;	// when chasing or attacking this is the distance between the character and Taz
	float		viewAngle;		// angle from center that character can see Taz
	float		currentBlendTime;	// current blend time to use for animations
	float		hearRange;			// TP: range in which enemy can hear Taz if not tip-toeing
	float		attackBoundaryRange;	// TP: attack beyond boundary range
	
	RequestPattern	idle;			// called when the character needs an idle seq
	RequestPattern	aware;			// called when the character needs an aware seq
	RequestPattern	attack;			// called when the character is able to attack taz
	RequestPattern	tooClose;		// called when taz is within the characters personal space
	RequestPattern	attacked;		// called when the character has been attacked by Taz
	RequestPattern	outOfRange;		// called when the character moves outside the maximum movement range
	RequestPattern	pursuitAttack;	// called when the character has been pursuing taz and is in attack range
	RequestPattern	moveCenter;		// TP: called when the character is able and trying to move the center point
	RequestPattern	flatten;		// TP: called when the character is flattened
	FrameCallback	frameCallback;	// TP: called once per frame
	Draw			draw2;			// TP: draw alpha
	FreeCallback	freeCallback;
	SpecialCallback	colliderSpecialCallback;	// TP: called when the character collides with a special object like water

	ChangeAIState	goAware;		// TP: custom function to test for going aware
	ChangeAIState	goAttack;		// TP: custom function to test for going to attack mode

	int				flags;				// ai flags
	char			*headNode;		// node to attach stars to
	TBVector		starsOffset;	// offset from the head for stars
	AttackCallback	attackCallback;	// function called each frame when character is doing attacking move
	MessageHandler	messageHandler;	// called when any messages are sent
	AIChangeState	changeStateCallback;	// called whenever the AI decides to change the state of a character
	int				soundHandle;			// keeps track of any looping samples playing
	float			starVars[4];		// describe the star format
	void			*customData;		// any custom data needed for the character
	float			currentRotTime;		// current time to perform rotate moves
	float			currentAnimSpeed;	// current speed for animations
	float			personalSpace;		// distance representing the characters personal space
	TBVector		toCam;				// TP: vector to cam
	float			distToCam;			// TP: dist to cam
	float			awareAngle;			// TP: view angle for entering the aware state
	RequestPattern	costume;			// TP: called when character has seen Taz in costume
	float			attackSpeedMod;		// TP: speed modifier for attack and chase moves
	char			typeOfAttack;		// type of attack, BITEFRONT, BITEREAR, SPINFRONT, SPINREAR
	char			seqMode;			// current sequence mode SEQMODE_PAUSE, SEQMODE_NORMAL
	char			collideWithTaz;		// TRUE/FALSE if collision with Taz
	float			attackHoldDistance;	// TP: distand to hold for SEQ_ATTACKHOLDDISTANDFACE move

	int				nodeIndex;			// TP: node index currently heading towards or -1 for no node
	TBVector		nodePos;			// TP: current position of the node heading towards	
	uint32			pursueAnimCRC;		// TP: anim to play while chasing taz around level.
	TBVector		posLastFrame;		// TP: like it sounds
	char			waitingForReset;	// TP: am waiting until off screen
	bool			resetAfterAttackOutOfRange;		// TP: Should character do reset after being attacked out of range

	float			pursuitTrailTimer;	// TP: timer for when to check for closest points on the trail when in pursuit
	float			timeStuck;			// TP: duration for which character has been stuck
	int				specialColl;		// TP: id of the specialtype that the character has collided with or -1
	uint32			runCRC;				// TP: crc of run animation used for reset
	EPatternState	patternState;		// TP: Current state of the pattern
	bool			instantAttack;		// TP: Should wait for attack move ?
	bool			instantAware;
	bool			alwaysClipPosition;	// TP: character has no concept of out of range and will always clip to the maximum movement range
	bool			costumeInAware;		// TP: determines if the character will do an costume move if taz is in the aware range,

	bool			useMoveBox;			// TP: option to use movement box or not
	MOVEMENTBOX		moveBox;			// TP: box that character can move inside
	bool			doCollisionWithTaz;	// TP: determines if actualy collision effect with Taz is ignored
	uint32			spawnPoint;			// TP: optional spawn point to use if taz captured
	bool			hasSeenCostume;		// TP: true if the character has done an attack move
	uint32			bounceCRC;			// TP: crc of bounce anim if there is one
	bool			centerValid;		// TP: is the current center validated

	float			shadowHeight;
	float			shadowRadius;
	float			shadowMapRadius;

	bool			characterEnabled;		// TP: is the character disabled, if so then stay inactive
	bool			dying;					// TP: flag is set to true to poick the character out and disable it
	bool			attackWithoutCollision;	// TP: flag is set true the character can be attacked if it does not have collision
	bool			knockoutOnTopCollision;	// TP: should the character be knocked out on a top cylinder collision
	CARTOONSCALEINFO	cartoonScale;
	
} GENERICAI, _GENERICAI BALIGN16;



/*  --------------------------------------------------------------------------------
	Function	: AIPreCharacterCollision
	Purpose		: Called when there is a collision
	Parameters	: collider, character collided with
	Returns		: 
	Info		: 
*/
int AIPreCharacterCollision(ACTORINSTANCE *collider, ACTORINSTANCE *npc, struct TAG_PRECOLLINFO *preCollInfo);

/*	--------------------------------------------------------------------------------
	Function 	: InitActorGenericAI
	Purpose 	: Setup the initial state of a characters basic AI
	Parameters 	: ACTORINSTANCE
	Returns 	: 
	Info 		:
*/
void InitActorGenericAI(ACTORINSTANCE *actorInstance,ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList);

/*	--------------------------------------------------------------------------------
	Function 	: SetAIPatternFunctions
	Purpose 	: Sets up call backs for requesting seq information
	Parameters 	: ACTORINSTANCE, idle, aware, attack, defend, attacked
	Returns 	: 
	Info 		:
*/
void SetAISeqFunctions(ACTORINSTANCE *actorInstance,RequestPattern idle,RequestPattern aware,RequestPattern attack,
					   RequestPattern tooClose,RequestPattern attacked,RequestPattern outOfRange,RequestPattern costume=NULL,
					   RequestPattern pursuitAttack=NULL, RequestPattern moveCenter=NULL);

/*	--------------------------------------------------------------------------------
	Function 	: AICharacterCollision
	Purpose 	: Called when a character collides with the AI character
	Parameters 	: npc, attacker
	Returns 	: 
	Info 		:
*/
int AICharacterCollision(ACTORINSTANCE *collider, ACTORINSTANCE *npc);

/*	--------------------------------------------------------------------------------
	Function 	: AICharacterScared
	Purpose 	: Called when the character has been scared
	Parameters 	: npc, attacker
	Returns 	: 
	Info 		:
*/
void AICharacterScared(ACTORINSTANCE *npc,ACTORINSTANCE *attacker);

/*  --------------------------------------------------------------------------------
	Function	: ClipPositionToBox
	Purpose		: Clip the position vector to the moveBox
	Parameters	: moveBox, position, optional buffer
	Returns		: true if clipped false if not
	Info		: 
*/
bool ClipPositionToBox(MOVEMENTBOX *moveBox, TBVector	position, float buffer=0.0f);

/*	--------------------------------------------------------------------------------
	Function 	: SetAISequenceMode
	Purpose 	: Sets the current sequence mode, SEQ_NORMAL, SEQ_PAUSE
	Parameters 	: ACTORINSTANCE , sequence mode
	Returns 	: 
	Info 		: 
*/
void SetAISequenceMode(ACTORINSTANCE *actorInstance, char seqMode);

/*	--------------------------------------------------------------------------------
	Function 	: SetAIMessageHandler
	Purpose 	: Sets the current message handler
	Parameters 	: ACTORINSTANCE , message handler
	Returns 	: 
	Info 		: 
*/
void SetAIMessageHandler(ACTORINSTANCE *actorInstance, MessageHandler MessageCallback);

/*	--------------------------------------------------------------------------------
	Function 	: SetAIChangeStateCallbac
	Purpose 	: Sets up call back for when generic AI changes state.
	Parameters 	: ACTORINSTANCE, changeStateCallback
	Returns 	: 
	Info 		:
*/
void SetAIChangeStateCallback(ACTORINSTANCE *actorInstance, AIChangeState changeStateCallback);

/*	--------------------------------------------------------------------------------
	Function 	: AICharacterBitten
	Purpose 	: Called when the character has been bitten
	Parameters 	: npc, attacker
	Returns 	: 
	Info 		:
*/
void AICharacterBitten(ACTORINSTANCE *npc,ACTORINSTANCE *attacker);

/*	--------------------------------------------------------------------------------
	Function 	: AICharacterSpun
	Purpose 	: Called when the character has been spun into
	Parameters 	: npc, attacker
	Returns 	: // PP: true if the character was successfully made to spin, else false
	Info 		:
*/
bool AICharacterSpun(ACTORINSTANCE *npc,ACTORINSTANCE *attacker,int forceDir = -1, bool affectStun = true);

/*	--------------------------------------------------------------------------------
	Function 	: SetAIAttackCallback
	Purpose 	: Sets up call back for attacking moves
	Parameters 	: ACTORINSTANCE, attackCallback
	Returns 	: 
	Info 		:
*/
void SetAIAttackCallback(ACTORINSTANCE *actorInstance, AttackCallback attackCallback);

/*	--------------------------------------------------------------------------------
	Function 	: ProcessPattern
	Purpose 	: Performs all the instructions layed down by the SEQ structure
	Parameters 	: ACTORINSTANCE 
	Returns 	: TRUE if pattern is finished, FALSE otherwise
	Info 		: 
*/
EPatternState ProcessPattern(ACTORINSTANCE *actorInstance);

/*	--------------------------------------------------------------------------------
	Function 	: FreeActorGenericAI
	Purpose 	: Free generic AI
	Parameters 	: ACTORINSTANCE
	Returns 	: 
	Info 		:
*/
void FreeActorGenericAI(ACTORINSTANCE *actorInstance);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateCharacterAI
	Purpose 	: Update the current AI for a character
	Parameters 	: ACTORINSTANCE
	Returns 	: true/false if actorInstance is free'd
	Info 		: Updates path and states
*/
bool UpdateCharacterAI(ACTORINSTANCE *actorInstance);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateCharacterAIInstances
	Purpose 	: Update the current AI for a character list
	Parameters 	: ACTORINSTANCELIST
	Returns 	: 
	Info 		: Updates path and states
*/
void UpdateCharacterAIInstances(ACTORINSTANCELIST *actorInstanceList);

/*	--------------------------------------------------------------------------------
	Function 	: SetSequenceOverride
	Purpose 	: Set the sequence override, will override until sequence finished.
	Parameters 	: ACTORINSTANCE, SEQ, STATE
	Returns 	: 
	Info 		: Set the current sequence to play, will override any playing sequence type
*/
void SetSequenceOverride(ACTORINSTANCE *actorInstance,SEQ *sequence,int state);

/*	--------------------------------------------------------------------------------
	Function 	: FacePoint
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void FacePoint(ACTORINSTANCE *actorInstance,TBVector dest,float rotTime,float offset = 0.0f);

/*	--------------------------------------------------------------------------------
	Function 	: ReachedPoint
	Purpose 	: Test if an actor is within range of a point in the xz axis only
	Parameters 	: pos, point, range
	Returns 	: 
	Info 		: 
*/
int ReachedPoint(TBVector pos,TBVector point,float range);

/*  --------------------------------------------------------------------------------
	Function	: AICollideWithSpecial
	Purpose		: This function is called when the character collides with any special
	Parameters	: the character, the special object collided with
	Returns		: 
	Info		: 
*/
void AICollideWithSpecial(ACTORINSTANCE *collider, ACTORINSTANCE *specialObject);

/*  --------------------------------------------------------------------------------
	Function	: DistFromNodeToTaz
	Purpose		: Returns the distance between the node on an actor to Taz
	Parameters	: actorInstance, node on actor
	Returns		: distance
	Info		: 
*/
float	DistFromNodeToTaz(ACTORINSTANCE *actorInstance, char *node);

/*  --------------------------------------------------------------------------------
	Function	: FindClosestEnemyToActor
	Purpose		: This function will return the closest active or inactive character
	Parameters	: actorInstance, range of closest character
	Returns		: 
	Info		: 
*/
ACTORINSTANCE *FindClosestEnemyToActor(TBVector position, float &range, const ACTORINSTANCE* const exclude = NULL, const TBActor* const actorExclude = NULL);

/*  --------------------------------------------------------------------------------
	Function	: DrawDebugInfo
	Purpose		: Draw debug info for all the ai characters on the given list
	Parameters	: actorInstanceList
	Returns		: 
	Info		: 
*/
void DrawDebugInfo(ACTORINSTANCELIST *actorInstanceList);

/*  --------------------------------------------------------------------------------
	Function	: SetupCharacterBox
	Purpose		: Creates the movement box for a character
	Parameters	: actorInstance for character, name of bankobject, position, orientation and scale of the instance
	Returns		: 
	Info		: 
*/
void SetupCharacterBox(const ACTORINSTANCE* const actorInstance, char *bankObject, TBVector position, TBQuaternion orientation, TBVector scale);

/*  --------------------------------------------------------------------------------
	Function	: CaptureTaz
	Purpose		: Respawns taz at the nearest spawn point or spawn point chosen by the enemy
	Parameters	: actorInstance of enemy
	Returns		: true/false 
	Info		: 
*/
bool CaptureTaz(const ACTORINSTANCE* const enemy);

/*  --------------------------------------------------------------------------------
	Function	: SetupTazInCage
	Purpose		: Puts taz in cage and starts the animations playing
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
void SetupTazInCage(ACTORINSTANCE *actorInstance);

/*  --------------------------------------------------------------------------------
	Function	: SetupCameraForCage
	Purpose		: Maintain cage camera
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
void SetupCameraForCage(ACTORINSTANCE *actorInstance);

/*  --------------------------------------------------------------------------------
	Function	: SafeToAttackTaz
	Purpose		: Test if it is ok to attack taz
	Parameters	: actorInstance of taz
	Returns		: true/false
	Info		: 
*/

bool SafeToAttackTaz(ACTORINSTANCE *actorInstance);

/*  --------------------------------------------------------------------------------
	Function	: AISetShadow
	Purpose		: Define the shadow to be used by the character
	Parameters	: actorInstance, shadow height, shadow radius, shadow map radius
	Returns		: 
	Info		: 
*/

void AISetShadow(ACTORINSTANCE *actorInstance, float shadowRadius, float shadowHeight, float shadowMapRadius);

/*  --------------------------------------------------------------------------------
	Function	: CheckForCharacterMessages
	Purpose		: See if the message is a character message
	Parameters	: actorInstance that sent the message, message
	Returns		: true if message used
	Info		: 
*/

bool CheckForCharacterMessages(ACTORINSTANCE *actorInstance, char *message);

/*  --------------------------------------------------------------------------------
	Function	: OUTOFRANGE
	Purpose		: Test if the character has move out of range
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
bool	OUTOFRANGE(ACTORINSTANCE *actorInstance);

/*  --------------------------------------------------------------------------------
	Function	: IsPlayerQuiet
	Purpose		: decide if the player has made a noise or not
	Parameters	: actorInstance
	Returns		: true if player is quite else false
	Info		: 
*/

bool IsPlayerQuiet(ACTORINSTANCE *actorInstance);

/*  --------------------------------------------------------------------------------
	Function : IsTazWithinHeightRange
	Purpose : Test if taz is within height range of character
	Parameters : actorInstance of character
	Returns : true/false
	Info : 
*/

bool IsTazWithinHeightRange(ACTORINSTANCE *actorInstance);


/*  --------------------------------------------------------------------------------
	Function	: FacingPoint
	Purpose		: Performs test to see is actorInstance is facing a point
	Parameters	: actorInstance, dest point, accuracy in radians either side
	Returns		: true if facing, false if not
	Info		: 
*/

bool FacingPoint(ACTORINSTANCE *actorInstance, TBVector dest, float offset, float accuracy);


/*  --------------------------------------------------------------------------------
	Function	: SetupTazPostCageIdle
	Purpose		: Called after taz has completed his in cage idle
	Parameters	: actorInstance of taz
	Returns		: 
	Info		: 
*/

void SetupTazPostCageIdle(ACTORINSTANCE *actorInstance);


/*  --------------------------------------------------------------------------------
	Function	: EnlargeBBox
	Purpose		: Enlarge a bounding box by a real world value
	Parameters	: 
	Returns		: 
	Info		: 
*/

void EnlargeBBox(BOUNDINGBOX *box, TBMatrix matrix, TBMatrix invmatrix, float size);

/*  --------------------------------------------------------------------------------
	Function	: AISoundCallback
	Purpose		: This function is called when a tracking sample finishes
	Parameters	: pointer to the handle in the ai structure
	Returns		: 
	Info		: 
*/

void AISoundCallback(void *voidPtr);


#endif