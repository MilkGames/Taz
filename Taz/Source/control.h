#ifndef __H_CONTROL
#define __H_CONTROL

#include "rumble.h"						// CMD: may as well include here, whenever you need rumble you need control
#include "mouse.h"

// PP: Xbox TCR requires us to allow controllers to be used in any port.
// PP: I'm guessing that GameCube should work the same way, being a 4-port machine.
// PP: I'll assume the same for PC while I'm at it.
// PP: Sony on the other hand like player 1 in port 1, and player 2 in port 2
// NH: Not quite sure how ports work on PC, but this didn't so I'm taking it out for now.
#if((BPLATFORM == XBOX) || (BPLATFORM == GAMECUBE))//. || (BPLATFORM == PC))
#define ALLOW_CONTROLLERS_IN_ANY_PORTS
#endif

// PP: THE NUMBER OF CONTROLLER PORTS THAT CAN BE USED TO CONTROL THE GAME
#if((BPLATFORM == XBOX) || (BPLATFORM == GAMECUBE))
#define NUM_CONTROLLER_PORTS		4
#else// PP: 2 ports
#define NUM_CONTROLLER_PORTS		2
#endif// PP: 2 ports

// PP: =====================================
// PP: CANCELBUTTON replaces a ton of instances of the triangle button being checked to cancel an action or back out of a menu.
// PP: I'd recommend using PAGE::setMenuActionFunc in future
#if(BPLATFORM == PS2)

// PP: PS2 - Triangle cancels or goes back.
#define CANCELBUTTON_PADNUM(_pad)			(controller##_pad.triangleDebounceChannel->value)

#elif(BPLATFORM == PC)

// PP: PC - button 1 (triangle), or right mouse button, or default back key (backspace is it?) cancels or goes back.
#define CANCELBUTTON_PADNUM(_pad)			((controller##_pad.triangleDebounceChannel->value)||(mouse.active && controller##_pad.mb2DebounceChannel->value) || (controller##_pad.menuBackDebounceChannel->value))

#elif(BPLATFORM == XBOX)

// PP: XBOX - B (circle) or BACK (select) cancels or goes back.
#define CANCELBUTTON_PADNUM(_pad)			((controller##_pad.circleDebounceChannel->value)/*B*/ || (controller##_pad.selectDebounceChannel->value)/*BACK*/)

#elif(BPLATFORM == GAMECUBE)

// IH: GameCube uses B button for back/cancel
#define CANCELBUTTON_PADNUM(_pad)			(controller##_pad.circleDebounceChannel->value)/*B*/

#else// PP: other platform - just use triangle

#define CANCELBUTTON_PADNUM(_pad)			(controller##_pad.triangleDebounceChannel->value)

#endif// PP: other platform

// PP: check for the cancel button on pad 1
#define CANCELBUTTON						CANCELBUTTON_PADNUM(1)

// PP: check for the cancel button on pad 2
#define CANCELBUTTON2						CANCELBUTTON_PADNUM(2)

// PP: =====================================
// PP: OKBUTTON replaces a ton of instances of the cross button being checked to accept an action or enter into a menu.
// PP: I'd STRONGLY recommend using PAGE::setMenuActionFunc in future

// PP: this is true while we're in the pause menu
extern bool pauseMenuActive;

// PP: PS2 - cross button accepts an option
#if(BPLATFORM == PS2)

#define OKBUTTON_PADNUM(_pad)				(controller##_pad.crossDebounceChannel->value)

// PP: PC - cross button equivalent, left mouse button or 'menu select' button accepts an option
#elif(BPLATFORM == PC)

#define OKBUTTON_PADNUM(_pad)				((controller##_pad.crossDebounceChannel->value) || (mouse.active && controller##_pad.mb1DebounceChannel->value) || (controller##_pad.menuSelectDebounceChannel->value))

// PP: XBOX - A (cross), or START select an option
#elif(BPLATFORM == XBOX)

#define OKBUTTON_PADNUM(_pad)				((controller##_pad.crossDebounceChannel->value)/*A*/ || (controller##_pad.startDebounceChannel->value))

#else// PP: other platform - just use cross

#define OKBUTTON_PADNUM(_pad)				(controller##_pad.crossDebounceChannel->value)

#endif// PP: other platform

// PP: check for the accept button on pad 1
#define OKBUTTON							OKBUTTON_PADNUM(1)

// PP: check for the accept button on pad 2
#define OKBUTTON2							OKBUTTON_PADNUM(2)

// PP: La la la what a mess la la la
#if(BPLATFORM == XBOX)
// PP: OKBUTTON_NOTSTART is like OKBUTTON but doesn't include START.  This one is used in the pause menu because START enters and exits pause mode.
#define OKBUTTON_NOTSTART					((controller1.crossDebounceChannel->value)/*A*/)
#else// PP: !xbox
#define OKBUTTON_NOTSTART					OKBUTTON
#endif// PP: !xbox

// PP: La la la what a mess la la la
#if(BPLATFORM == XBOX)
// PP: OKBUTTON_NOTSTART is like OKBUTTON but doesn't include START.  This one is used in the pause menu because START enters and exits pause mode.
#define OKBUTTON2_NOTSTART					((controller2.crossDebounceChannel->value)/*A*/)
#else// PP: !xbox
#define OKBUTTON2_NOTSTART					OKBUTTON2
#endif// PP: !xbox

#if(NUM_CONTROLLER_PORTS == 4)

// PP: OKBUTTON_ANY_PAD evaluates to non-zero if an OK button on any pad is being pressed
#define OKBUTTON_ANY_PAD\
	OKBUTTON_PADNUM(1)\
	||OKBUTTON_PADNUM(2)\
	||OKBUTTON_PADNUM(3)\
	||OKBUTTON_PADNUM(4)

// PP: CANCELBUTTON_ANY_PAD evaluates to non-zero if a cancel button on any pad is being pressed
#define CANCELBUTTON_ANY_PAD\
	CANCELBUTTON_PADNUM(1)\
	||CANCELBUTTON_PADNUM(2)\
	||CANCELBUTTON_PADNUM(3)\
	||CANCELBUTTON_PADNUM(4)

#else// PP: !4 ports

// PP: OKBUTTON_ANY_PAD evaluates to non-zero if an OK button on any pad is being pressed
#define OKBUTTON_ANY_PAD\
	OKBUTTON_PADNUM(1)\
	||OKBUTTON_PADNUM(2)

// PP: CANCELBUTTON_ANY_PAD evaluates to non-zero if a cancel button on any pad is being pressed
#define CANCELBUTTON_ANY_PAD\
	CANCELBUTTON_PADNUM(1)\
	||CANCELBUTTON_PADNUM(2)

#endif// PP: !4 ports

// PP: =====================================

// PP: I saw a few places where the triangle button (for cancel or back) was being force-debounced.  Hopefully this macro should be a bit safer.
#define FORCE_CANCELBUTTON_DEBOUNCE			biForceDebounce(controller1.triangleDebounceChannel); \
											biForceDebounce(controller1.circleDebounceChannel);/*XBOX 'A' BUTTON!!!*/ \
											biForceDebounce(controller1.selectDebounceChannel);/*XBOX 'BACK' BUTTON!!!*/ \
											biForceDebounce(controller1.menuBackDebounceChannel); \
											if(mouse.active) biForceDebounce(controller1.mb2DebounceChannel);

// PP: I saw a few places where the cross button (for accept) was being force-debounced.  Hopefully this macro should be a bit safer.
#define FORCE_OKBUTTON_DEBOUNCE				biForceDebounce(controller1.crossDebounceChannel); \
											biForceDebounce(controller1.startDebounceChannel); \
											biForceDebounce(controller1.menuSelectDebounceChannel); \
											if(mouse.active) biForceDebounce(controller1.mb1DebounceChannel);

// PP: FORCE_OKBUTTON_DEBOUNCE_ALL_PADS force-debounces the 'ok' buttons on all pads
#if(NUM_CONTROLLER_PORTS == 4)
#define FORCE_OKBUTTON_DEBOUNCE_ALL_PADS		biForceDebounce(controller1.crossDebounceChannel); \
												biForceDebounce(controller1.startDebounceChannel); \
												biForceDebounce(controller2.crossDebounceChannel); \
												biForceDebounce(controller2.startDebounceChannel); \
												biForceDebounce(controller3.crossDebounceChannel); \
												biForceDebounce(controller3.startDebounceChannel); \
												biForceDebounce(controller4.crossDebounceChannel); \
												biForceDebounce(controller4.startDebounceChannel); \
												biForceDebounce(controller1.menuSelectDebounceChannel); \
												if(mouse.active) biForceDebounce(controller1.mb1DebounceChannel);
#else// PP: 2 ports
#define FORCE_OKBUTTON_DEBOUNCE_ALL_PADS		biForceDebounce(controller1.crossDebounceChannel); \
												biForceDebounce(controller1.startDebounceChannel); \
												biForceDebounce(controller2.crossDebounceChannel); \
												biForceDebounce(controller2.startDebounceChannel); \
												biForceDebounce(controller1.menuSelectDebounceChannel); \
												if(mouse.active) biForceDebounce(controller1.mb1DebounceChannel);
#endif// PP: 2 ports

// PP: FORCE_OKBUTTON_DEBOUNCE_ALL_PADS force-debounces the 'cancel' buttons on all pads
#if(NUM_CONTROLLER_PORTS == 4)
#define FORCE_CANCELBUTTON_DEBOUNCE_ALL_PADS	biForceDebounce(controller1.triangleDebounceChannel); \
												biForceDebounce(controller1.circleDebounceChannel);/*XBOX 'A' BUTTON!!!*/ \
												biForceDebounce(controller1.selectDebounceChannel);/*XBOX 'BACK' BUTTON!!!*/ \
												biForceDebounce(controller2.triangleDebounceChannel); \
												biForceDebounce(controller2.circleDebounceChannel);/*XBOX 'A' BUTTON!!!*/ \
												biForceDebounce(controller2.selectDebounceChannel);/*XBOX 'BACK' BUTTON!!!*/ \
												biForceDebounce(controller3.triangleDebounceChannel); \
												biForceDebounce(controller3.circleDebounceChannel);/*XBOX 'A' BUTTON!!!*/ \
												biForceDebounce(controller3.selectDebounceChannel);/*XBOX 'BACK' BUTTON!!!*/ \
												biForceDebounce(controller4.triangleDebounceChannel); \
												biForceDebounce(controller4.circleDebounceChannel);/*XBOX 'A' BUTTON!!!*/ \
												biForceDebounce(controller4.selectDebounceChannel);/*XBOX 'BACK' BUTTON!!!*/ \
												biForceDebounce(controller1.menuBackDebounceChannel); \
												if(mouse.active) biForceDebounce(controller1.mb2DebounceChannel);
#else// PP: 2 ports
#define FORCE_CANCELBUTTON_DEBOUNCE_ALL_PADS	biForceDebounce(controller1.triangleDebounceChannel); \
												biForceDebounce(controller1.circleDebounceChannel);/*XBOX 'A' BUTTON!!!*/ \
												biForceDebounce(controller1.selectDebounceChannel);/*XBOX 'BACK' BUTTON!!!*/ \
												biForceDebounce(controller2.triangleDebounceChannel); \
												biForceDebounce(controller2.circleDebounceChannel);/*XBOX 'A' BUTTON!!!*/ \
												biForceDebounce(controller2.selectDebounceChannel);/*XBOX 'BACK' BUTTON!!!*/ \
												biForceDebounce(controller1.menuBackDebounceChannel); \
												if(mouse.active) biForceDebounce(controller1.mb2DebounceChannel);
#endif// PP: 2 ports

// PP: FORCE_START_DEBOUNCE_ALL_PADS force-debounces the start buttons on all pads
#if(NUM_CONTROLLER_PORTS == 4)
#define FORCE_START_DEBOUNCE_ALL_PADS\
		biForceDebounce(controller1.startDebounceChannel);\
		biForceDebounce(controller2.startDebounceChannel);\
		biForceDebounce(controller3.startDebounceChannel);\
		biForceDebounce(controller4.startDebounceChannel);
#else// PP: 2 ports
#define FORCE_START_DEBOUNCE_ALL_PADS\
		biForceDebounce(controller1.startDebounceChannel);\
		biForceDebounce(controller2.startDebounceChannel);
#endif// PP: 2 ports

// PP: callback type for projectile-state collisions
typedef void(*ProjectileStateCollisionCallback)(ACTORINSTANCE* const acotrInstance);

// Globals
extern TBVector								startVel;
extern TBVector								landingVel;
extern int									numFrames;
extern float								totalTime;
extern float								maxHeight;

// PP: pad-checking modes
enum EPadCheckMode
{
	PADCHECK_ALL,						// PP: keep controller1..4 bound to ports 1..4
	PADCHECK_WAITING_FOR_PLAYER1,		// PP: the next port to receive a START signal will be used for player 1
	PADCHECK_WAITING_FOR_PLAYER2,		// PP: the next port to receive a START signal will be used for player 2
	PADCHECK_NORMAL,					// PP: check for removal of necessary controllers
};

// PP: current pad-checking mode
extern EPadCheckMode						padCheckMode;

#define MOTIONBLUR_SPIN_CONST				(0.06f)

// PP: custom callback function for projectile-state collisions
extern ProjectileStateCollisionCallback		projectileStateCustomCollisionCallback;


typedef struct TAG_PADCONTROLSTATUS
{
	float	direction1,direction2;		// directions in radians of the two analogues, check dead zone flag first
										// 0 is forwards and increase clockwise
	float	mag1,mag2;					// magnitude of analogue input 0 to 1 (***problem*** with diagonal 0 to 1.14)

	float	x1,y1,x2,y2,l1,r1,l2,r2;	// analogue values after dead zone is taken into account range -1 to +1
	float	idleTime;					// time in seconds which no control input has been received
	
	// Babel control mapping
	TBIChannelMap	*inputMap;

	TBIInputChannel	*x1Channel,*y1Channel;
	TBIInputChannel *x2Channel,*y2Channel;

	TBIInputChannel	*x1DebounceChannel,*y1DebounceChannel;
	TBIInputChannel	*x2DebounceChannel,*y2DebounceChannel;
	
	TBIInputChannel *dPadXChannel,*dPadYChannel;
	TBIInputChannel *crossChannel,*triangleChannel,*circleChannel,*squareChannel;
	TBIInputChannel *l1Channel,*l2Channel,*l3Channel;
	TBIInputChannel *r1Channel,*r2Channel,*r3Channel;
	TBIInputChannel *startChannel,*selectChannel;
	TBIInputChannel *tabChannel;
	TBIInputChannel *lshiftChannel;

	TBIInputChannel *dPadXDebounceChannel,*dPadYDebounceChannel;
	TBIInputChannel *crossDebounceChannel,*triangleDebounceChannel,*circleDebounceChannel,*squareDebounceChannel;
	TBIInputChannel *l1DebounceChannel,*l2DebounceChannel,*l3DebounceChannel;
	TBIInputChannel *r1DebounceChannel,*r2DebounceChannel,*r3DebounceChannel;
	TBIInputChannel *startDebounceChannel,*selectDebounceChannel;
	TBIInputChannel *f10DebounceChannel;
	TBIInputChannel *rDebounceChannel;
	TBIInputChannel *gDebounceChannel;
	TBIInputChannel *sDebounceChannel;
	TBIInputChannel *aDebounceChannel;
	TBIInputChannel *mDebounceChannel;
	TBIInputChannel *hDebounceChannel;
	TBIInputChannel *iDebounceChannel;
	TBIInputChannel *lshiftDebounceChannel;
	TBIInputChannel *commaDebounceChannel;
	TBIInputChannel *equalsChannel;
	TBIInputChannel *minusChannel;
	TBIInputChannel *periodDebounceChannel;
	TBIInputChannel *lbracketDebounceChannel;
	TBIInputChannel *rbracketDebounceChannel;
	TBIInputChannel *f5DebounceChannel;
	TBIInputChannel *f6DebounceChannel;
	TBIInputChannel *f3DebounceChannel;
	TBIInputChannel *tabDebounceChannel;
	TBIInputChannel *guiDebounceChannel;
	TBIInputChannel *escDebounceChannel;

	// NH: Mouse Control
	TBIInputChannel *mb1Channel;
	TBIInputChannel *mb1DebounceChannel;
	TBIInputChannel *mb2Channel;
	TBIInputChannel *mb2DebounceChannel;
	TBIInputChannel *mxChannel;
	TBIInputChannel *mxDebounceChannel;
	TBIInputChannel *myChannel;
	TBIInputChannel *myDebounceChannel;

	// NH: Menu Navigation Controls
//	TBIInputChannel *menuUpChannel;
//	TBIInputChannel *menuDownChannel;
//	TBIInputChannel *menuLeftChannel;
//	TBIInputChannel *menuRightChannel;
//	TBIInputChannel *menuSelectChannel;
//	TBIInputChannel *menuBackChannel;

	// PP: auto-repeat menu navigation controls...
	TBIInputChannel *menuUpRepeatChannel;				// PP: auto-repeating digital up channel, for menu navigation
	TBIInputChannel *menuDownRepeatChannel;				// PP: auto-repeating digital down channel, for menu navigation
	TBIInputChannel *menuLeftRepeatChannel;				// PP: auto-repeating digital left channel, for menu navigation
	TBIInputChannel *menuRightRepeatChannel;			// PP: auto-repeating digital right channel, for menu navigation

	// PP: debounced menu navigation controls...
	TBIInputChannel *menuUpDebounceChannel;				// PP: debounced digital up channel, for menu navigation
	TBIInputChannel *menuDownDebounceChannel;			// PP: debounced digital down channel, for menu navigation
	TBIInputChannel *menuLeftDebounceChannel;			// PP: debounced digital left channel, for menu navigation
	TBIInputChannel *menuRightDebounceChannel;			// PP: debounced digital right channel, for menu navigation

	TBIInputChannel *menuSelectDebounceChannel;
	TBIInputChannel *menuBackDebounceChannel;
	TBIInputChannel *menuYChannel;
	TBIInputChannel *menuXChannel;
	TBIInputChannel *menuSelectChannel;
	TBIInputChannel *menuBackChannel;

	// rumble control structure
	struct TAG_RUMBLECONTROLLER		*rumbleController;

	int				padID;			// PP: LOGICAL pad number (zero-based)
	int32			port;			// PP: zero-based index of port into which pad is plugged, or -1 if not plugged in	// PP: int32 because god knows how many 'ports' a PC or something has
	char			player;			// PP: zero-based index of player using the pad; -1 if none/unknown

} PADCONTROLSTATUS;

// PP: I added these - YES THEY *ARE* NEEDED
extern PADCONTROLSTATUS						controller3,controller4;// PP: the third and fourth controllers - only used while determining which pads the player(s) want to use; only controller1 and controller2 are used during gameplay

// PP: On PS2, assume only 2 ports, and each controller in the 'correct' port
// PP: I don't particularly like this macro.
#define PADSCONNECTED	((BPLATFORM==PC)||(BPLATFORM==XBOX)||(gameStatus.multiplayer.versusCPU&&biFindPad(0)==0)||(!gameStatus.multiplayer.versusCPU&&biFindPad(1)==1))


/* --------------------------------------------------------------------------------
   Function		: Control_init
   Purpose		: initialisation at the beginning of the game for the Control module
   Parameters	: 
   Returns		: 
   Info			: // PP: 
*/
void Control_init(void);


/* --------------------------------------------------------------------------------
   Function		: Control_shutdown
   Purpose		: shutdown at the end of the game for the Control module
   Parameters	: 
   Returns		: 
   Info			: // PP: 
*/
void Control_shutdown(void);


/*  --------------------------------------------------------------------------------
	Function	: ClearInputBindings
	Purpose		: Removes all existing bindings on the input channels
	Parameters	: controller
	Returns		: 
	Info		: this allows the BindInputChannels function to be called again
*/

void ClearInputBindings(PADCONTROLSTATUS *controller);

/* --------------------------------------------------------------------------------
   Function : ProcessInput
   Purpose : Responds to pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO or NULL, ACTOR or NULL
   Returns : 
   Info : will modify the camera and actor if supplied
*/
void ProcessInput(struct TAG_PLAYER *player);

/* --------------------------------------------------------------------------------

   Function : RespondToStateChange
   Purpose : if a state change is requested then change
   Parameters : ACTORSTATUS
   Returns : 
   Info : 
*/
int RespondToStateExit(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : RespondToStateChangePost
   Purpose : if a state change is requested then change
   Parameters : ACTORSTATUS
   Returns : 
   Info : 
*/
void RespondToStateExitPost(struct TAG_ACTORINSTANCE *actorInstance,struct TAG_ACTOR_STATUS *actorStatus);

/* --------------------------------------------------------------------------------
   Function : UpdateSlidePitch
   Purpose : rotatates the projectile to point feet first in direction of arc
   Parameters : Projectile Actor Instance;
   Returns : 
   Info :
*/

void UpdateSlidePitch(struct TAG_ACTORINSTANCE *actor, float angle);

/* --------------------------------------------------------------------------------
   Function : MatchGroundOrientation
   Purpose : rotatates the projectile to point feet first in direction of arc
   Parameters : Projectile Actor Instance;
   Returns : 
   Info :
*/

void MatchGroundOrientation(struct TAG_ACTORINSTANCE *actor);


/* --------------------------------------------------------------------------------
   Function		: HandleSquashedState
   Purpose		: Handle the squashed state in the normal way
   Parameters	: ptr to PLAYER who's squashed,
   Returns		: 
   Info			: // PP: this saves cutting & pasting for alternative (eg. boss game) UpdateCharacterState-type functions.
					It also cuts down on the size of UpdateCharacterState.
*/
void HandleSquashedState(struct TAG_PLAYER* const player);


/* --------------------------------------------------------------------------------
   Function		: CheckForJump
   Purpose		: Check if the player is trying to jump; start the jump if so
   Parameters	: ptr to PLAYER for which to check
   Returns		: true if the player is trying to jump
   Info			: // PP: this saves cutting & pasting for alternative (eg. boss game) UpdateCharacterState-type functions.
					It also cuts down on the size of UpdateCharacterState.
*/
bool CheckForJump(struct TAG_PLAYER* const player);


/* --------------------------------------------------------------------------------
   Function		: HandleJumpState
   Purpose		: Handle the jump state in the normal way
   Parameters	: ptr to PLAYER who's jumping,
					(opt/false) whether or not the character's Y angle is locked during the jump,
					(opt/true) whether or not to go into a move anim on landing
   Returns		: 
   Info			: // PP: this saves cutting & pasting for alternative (eg. boss game) UpdateCharacterState-type functions.
					It also cuts down on the size of UpdateCharacterState.
*/
void HandleJumpState(struct TAG_PLAYER* const player, const bool lockYAng=false, const bool intoMoveAnim=true);

/* --------------------------------------------------------------------------------
   Function		: CheckForSpin
   Purpose		: Check if the player is trying to spin; start the spin if so
   Parameters	: ptr to PLAYER for which to check
   Returns		: true if the player is trying to spin
   Info			: // PP: this saves cutting & pasting for alternative (eg. boss game) UpdateCharacterState-type functions.
					It also cuts down on the size of UpdateCharacterState.
*/
bool CheckForSpin(struct TAG_PLAYER* const player);

/* --------------------------------------------------------------------------------
   Function		: HandleSpinUpState
   Purpose		: Handle the spin-up state in the normal way
   Parameters	: ptr to PLAYER who's spinning-up, (opt/false) whether or not to lock Y angle during spinup
   Returns		: 
   Info			: // PP: this saves cutting & pasting for alternative (eg. boss game) UpdateCharacterState-type functions.
					It also cuts down on the size of UpdateCharacterState.
*/
void HandleSpinUpState(struct TAG_PLAYER* const player, const bool lockYAng=false);

/* --------------------------------------------------------------------------------
   Function		: HandleSpinState
   Purpose		: Handle the spin state in the normal way
   Parameters	: ptr to PLAYER who's spinning, (opt/false) lock Y angle during spin
   Returns		: 
   Info			: // PP: this saves cutting & pasting for alternative (eg. boss game) UpdateCharacterState-type functions.
					It also cuts down on the size of UpdateCharacterState.
*/
void HandleSpinState(struct TAG_PLAYER* const player, const bool lockYAng=false);

/* --------------------------------------------------------------------------------
   Function		: HandHandleLoseCostumeSpinleSpinState
   Purpose		: Handle the spin state without any control
   Parameters	: ptr to PLAYER who's spinning, (opt/false) lock Y angle during spin
   Returns		: 
   Info			: 
*/
void HandleLoseCostumeSpin(struct TAG_PLAYER* const player, const bool lockYAng=false);

/* --------------------------------------------------------------------------------
   Function		: SpinDown
   Purpose		: Break out of STATE_SPIN into STATE_SPINDOWN
   Parameters	: ptr to PLAYER who's gonna break out of a spin
   Returns		: 
   Info			: // PP: this saves cutting & pasting for alternative (eg. boss game) UpdateCharacterState-type functions.
					It also cuts down on the size of UpdateCharacterState.
*/
void SpinDown(struct TAG_PLAYER* const player);


/* --------------------------------------------------------------------------------
   Function		: HandleSpinDownState
   Purpose		: Handle the spin-down state in the normal way
   Parameters	: ptr to PLAYER who's spinning-down
   Returns		: 
   Info			: // PP: this saves cutting & pasting for alternative (eg. boss game) UpdateCharacterState-type functions.
					It also cuts down on the size of UpdateCharacterState.
*/
void HandleSpinDownState(struct TAG_PLAYER* const player);

/* --------------------------------------------------------------------------------
   Function : UpdateCharacterState
   Purpose : Analize the characters current state and input to decide what character does
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : if changeStateOnly is set then return value tells if the requested state change was completed, 
				otherwise the return value tells if any state change was made
   Info : 
*/
bool UpdateCharacterState(struct TAG_PLAYER *player, bool changeStateOnly = false);

/* --------------------------------------------------------------------------------
   Function : UpdateController
   Purpose : updates the controllers internal structure
   Parameters : PADCONTROLSTATUS
   Returns : 
   Info : 
*/
void UpdateController(struct TAG_PADCONTROLSTATUS *controller);

/* --------------------------------------------------------------------------------
   Function : CreateInputChannels
   Purpose : Create all input channels
   Parameters : PADCONTROLSTATUS
   Returns : 
   Info : Also creates channel map
*/
void CreateInputChannels(struct TAG_PADCONTROLSTATUS *controller, int padID);

/* --------------------------------------------------------------------------------
   Function : DeleteInputChannels
   Purpose : deletes all control channels along with their bindings.
   Parameters : PADCONTROLSTATUS
   Returns : 
   Info : Also creates channel map
*/
void DeleteInputChannels(struct TAG_PADCONTROLSTATUS *controller);

/* --------------------------------------------------------------------------------
   Function		: BindInputChannelsToPort
   Purpose		: Bind a pad structure's input channels to a specific port 
   Parameters	: PADCONTROLSTATUS*, physical 0-based port index
   Returns		: TRUE=success/FALSE=failure
   Info			: // PP: 
*/
BOOL BindInputChannelsToPort(PADCONTROLSTATUS* const controller, const uchar port);

/* --------------------------------------------------------------------------------
   Function : BindInputChannels
   Purpose : Bind keys and buttons to input channels
   Parameters : PADCONTROLSTATUS
   Returns : TRUE / FALSE
   Info : Uses the BPLATFORM define to decide whether to bind keys or buttons
*/
int BindInputChannels(struct TAG_PADCONTROLSTATUS *controller,int padNumber);

/*	--------------------------------------------------------------------------------
	Function 	: bindControllersToDefaultPorts
	Purpose 	: binds each PADCONTROLSTATUS structure to the port corresponding to its number;
					ie. (re)binds controller1..4 to ports 1..4
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: call this when you set padCheckMode to PADCHECK_ALL or PADCHECK_WAITING_FOR_PLAYER1
*/
void bindControllersToDefaultPorts(void);

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToSteeringControl
   Purpose : simple steering control
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR, (opt/false) whether or not to lock character's Y angle during the spin
   Returns : 
   Info : Will modify a characters direction and speed
*/
void CharacterRespondToSteeringControl(struct TAG_PADCONTROLSTATUS *controller, struct TAG_CAMERAINFO *cameraInfo, struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : RespondToStateInit
   Purpose : Returns true if it is the first time in a new state
   Parameters : ACTORINSTANCE
   Returns : true if new state else false
   Info : 
*/
bool RespondToStateInit(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : DIKToBabel
   Purpose : Converts a DIK (Direct Input Key) code to the Babel version
   Parameters : int (DIKCode)
   Returns : int (Bable code)
   Info : Scans through the LUT to find the correct Babel code
*/
int DIKToBabel(int DIKCode);

/* --------------------------------------------------------------------------------
   Function : defaultKeyboardControls
   Purpose : Sets the default keyboard controls for a player
   Parameters : 
   Returns : 
   Info : 
*/
void defaultKeyboardControls(PADCONTROLSTATUS *controller, int padNumber);



// PP: ***************************************** MISSING-CONTROLLER STUFF ***************************************
// PP: **********************************************************************************************************

/*	--------------------------------------------------------------------------------
	Function 	: checkForMissingControllers2
	Purpose 	: find and bind required controllers; if a required controller is missing,
					freeze the game and show the missing-controller message
	Parameters 	: true - display message and wait for controller insertion(default)/false - just check
	Returns 	: 
	Info 		: // PP: This is the hassle-free no-nonsense version.  Not pretty,
					but very effective!
*/
void checkForMissingControllers2(bool display=true);


#ifdef ALLOW_CONTROLLERS_IN_ANY_PORTS

/*	--------------------------------------------------------------------------------
	Function 	: checkForMissingControllers
	Purpose 	: check for a necessary controller being removed, and take appropriate action (pause game, show message)
	Parameters 	: whether or not to pause and display a message if a required controller is missing
	Returns 	: 
	Info 		: // PP: Bit of a misleading name now - when passed 'false', this function just finds and binds the controllers
					TODO: look into biSetMapPhysicalPort
*/
void checkForMissingControllers(const bool moanIfMissing);

#else// PP: ifdef ALLOW_CONTROLLERS_IN_ANY_PORTS else

// PP: controllers must be in their 'correct' ports

/*	--------------------------------------------------------------------------------
	Function 	: PS2_checkForMissingControllers
	Purpose 	: check for a necessary controller being removed (on PS2), and take appropriate action (pause game, show message)
	Parameters 	: whether or not to pause and display a message if a required controller is missing
	Returns 	: 
	Info 		: // PP: Bit of a misleading name now - when passed 'false', this function just finds and binds the controllers
					TODO: look into biSetMapPhysicalPort
*/
void PS2_checkForMissingControllers(const bool moanIfMissing);

#define checkForMissingControllers		PS2_checkForMissingControllers

#endif// PP: def ALLOW_CONTROLLERS_IN_ANY_PORTS else
// PP: **********************************************************************************************************
// PP: **********************************************************************************************************


/*	--------------------------------------------------------------------------------
	Function 	: checkForStartAndBind
	Purpose 	: check for the START button on any pad; bind a PADCONTROLSTATUS
					struct to the port from which the START signal was detected
	Parameters 	: ref to PADCONTROLSTATUS to bind to port from which if any the START signal is detected,
					(opt/NULL) ptr to PADCONTROLSTATUS that has already been bound and is therefore to be ignored
	Returns 	: true if START was pressed, else false
	Info 		: 
*/
bool checkForStartAndBind(PADCONTROLSTATUS& controller, PADCONTROLSTATUS* const controllerAlreadyBound=NULL);


#endif// PP: ndef __H_CONTROL