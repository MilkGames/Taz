#ifndef __H_CHARACTERS
#define __H_CHARACTERS

#include "health.h"
#include "attach.h"
#include "cartoonscaling.h"
#include "postcard.h"
#include "spineffect.h"
#include "camera.h"
#include "control.h"
#include "redshield.h"
#include "invisibletaz.h"
#include "actors.h"
#include "superburpeffect.h"
#include "rapperblast.h"
#include "costumes.h"
#include "swingingobjects.h"

#define CHARACTERINFO_CREATESPIN		(1<<0)
#define CHARACTERINFO_TARTANTAZ			(1<<1)

// PP: the maximum speed to which a spin can be built-up
#define MAX_SPIN_SPEED(_actorInstance)		((_actorInstance)->actorBehaviour->physics.maxSpeed*2.0f)

// PP: rate at which target spin speed builds up while spinning on the spot
#define SPIN_BUILDUP_ACCELERATION			METERTOUNIT(10)

enum EBlinkState
{
	BLINKSTATE_INIT,
	BLINKSTATE_IDLE,
	BLINKSTATE_BLINK,
};

typedef class TAG_BLINKEYES
{
	TBActorInstance		*instance;		// TP: blink actor
	EBlinkState			state;			// TP: current state of the blink
	float				time;			// TP: general purpose timer

public:

	/*  --------------------------------------------------------------------------------
		Function	: BLINKEYES::TAG_BLINKEYES
		Purpose		: Constructor, creates actorInstance for eyes and initialises state
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	TAG_BLINKEYES();

	/*  --------------------------------------------------------------------------------
		Function	: BLINKEYES::~TAG_BLINKEYES
		Purpose		: Destructor, deletes the actorinstance
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	~TAG_BLINKEYES()
	{
		if (instance)
		{
			baFreeActorInstance(instance);
			delete instance;
		}
	}

	/*  --------------------------------------------------------------------------------
		Function	: BLINKEYES::Update
		Purpose		: Update the animation of the eyes and update state
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	void Update();

	/*  --------------------------------------------------------------------------------
		Function	: BLINKEYES::Draw
		Purpose		: Draw the eyes
		Parameters	: Current camera
		Returns		: 
		Info		: Does not set the render state
	*/
	void Draw(struct TAG_CAMERAINFO *camera);
} BLINKEYES, _BLINKEYES BALIGN16;


// PP: flags relating to the Ninja costume
enum
{
	NINJAFLAG_GOODIMPACT		=1<<0,				// PP: Indicates that the kick has connected with an enemy and caused the enemy to react in a dramatic way (these conditions are required for the timeslice shot)
};

typedef struct TAG_SWINGINGINFO
{
	TBQuaternion	previousOrientation;

	ACTORINSTANCE	*swingingOnObject;			// pointer to object which taz is currently swinging on, if none pointer is null
	float			xyMaxSpeed;					// maxiumum speed without other input when taz is swinging
	float			zyMaxSpeed;					// when taz is swinging he needs a lot of speed
	float			xySpeed;

	float			zySpeed;
	uint32			firstFrame;

	struct TAG_CAMERAINFO		preSwingCamera;
} SWINGINGINFO, _SWINGINGINFO BALIGN16;

typedef struct TAG_CHARACTERINFO		// all character specific info
{
	TBVector					portalGravity;		// CMD: direction of gravity when actor is in portal

//	struct TAG_HEALTH			health;				// characters health related info
	struct TAG_ATTACH			attachedObjects;	// attached objects
//	struct TAG_POSTCARD			postcard;			// postcard structure
	struct TAG_CARTOONSCALEINFO	cartoonScale;		// cartoon scale info structure

	struct TAG_TRAIL			*pTrail;			// CPW: record of where character has gone
	struct TAG_THOUGHTBUBBLE	*thoughtBubble;		// characters thought bubble
	struct TAG_VOODOOCLOUDINFO	*voodooCloudInfo;	// characters voodoo cloud
	char						costume;			// TP: current costume or -1
	bool						loseCostume;		// TP: set to true if the character should lose the costume at the next opportunity
	int							costumeHandle;		// CMD: handle identifier for costume actor
	uchar						costumeFlags;		// PP: flags relating to the current costume, eg the NINJAFLAG_ flags
	
	struct TAG_SUPERBURPEFFECT	superBurpEffectInfo;	// CMD: structure to hold info about superburp effect, can't be bothered with the public/private class thing above, must make a stand for a deterministic approach!!this is lovely tea
	struct TAG_POWERUPINFO		powerUpInfo;		// CMD: info for taz power ups

	struct TAG_SUPERBURP		*superBurp;			// TP: structure to hold info about the superburp
	struct TAG_SPINEFFECTINFO	*spinInfo;			// CMD: characters spin info
	struct TAG_TARTANTAZ		*tartanInfo;		// CMD: characters wallpaper info, see tartantaz.cpp
	struct TAG_TRANSFXINFO		*transportInfo;		// CMD: info for when taz transports

	class	TAG_EATING			*eating;			// TP: class to hold info about things being eaten
	struct TAG_DESTRUCTIBLEOBJECTINFO	*inMouth;	// TP: Destructible info of whats in mouth for spitting
	float						timer;				// TP: temp timer used for things like waiting for death
	BLINKEYES					*blinkEyes;			// TP: Blink eye actor for look around mode.

	TAZSHIELDINFO				*tazShieldInfo;		// CMD: info for shield only used in mechatweety level ... so far
	int							currentBurp;				// TP: number of burp cans collected
	int							rumbleHandle;
	RAPPERBLASTINFO				*rapperBlastInfo;	// CMD: info used by taz in the hood.

	SWINGINGINFO				swingingInfo;
	
	struct
	{
		TBVector				points[4];			// TP: the four points used to make the curve
		float					t;					// TP: t value across curve
		float					time;				// TP: time trip should take
		bool					fixedPathEnable;	// TP: flag to determing if using the fixed path projectile or not
	} projecting;

} CHARACTERINFO, _CHARACTERINFO BALIGN16;


/* --------------------------------------------------------------------------------
   Function : InitialiseCharacterInfo
   Purpose : initialise character info structure
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void InitialiseCharacterInfo(ACTORINSTANCE *actorInstance, uint32 createFlags=0);

/* --------------------------------------------------------------------------------
   Function : DestroyCharacterInfo
   Purpose : destroy character info structure
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void DestroyCharacterInfo(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : PrepCharacterInfo
   Purpose : prepare all character info
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void PrepCharacterInfo(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : PrepInstancesCharacterInfo
   Purpose : prepare all character info in a instance list
   Parameters : CHARACTERINFO
   Returns : 
   Info : prepare things like thought bubble etc.
*/
void PrepInstancesCharacterInfo(ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : DrawCharacterInfo
   Purpose : draw all character info
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void DrawCharacterInfo(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : DrawCharacterInfo
   Purpose : draw all character info
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void DrawCharacterInfo2(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : DrawInstancesCharacterInfo
   Purpose : draw all character info in a instance list
   Parameters : CHARACTERINFO
   Returns : 
   Info : draws things like thought bubbles etc.
*/
void DrawInstancesCharacterInfo(ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : DrawInstancesCharacterInfo
   Purpose : draw all character info in a instance list
   Parameters : CHARACTERINFO
   Returns : 
   Info : draws things like thought bubbles etc.
*/
void DrawInstancesCharacterInfo2(ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : UpdateCharacterInfo
   Purpose : Update all character info
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void UpdateCharacterInfo(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : UpdateInstancesCharacterInfo
   Purpose : Update all character info in a instance list
   Parameters : CHARACTERINFO
   Returns : 
   Info : Updates things like thought bubbles etc.
*/
void UpdateInstancesCharacterInfo(ACTORINSTANCELIST *actorInstanceList);


/* --------------------------------------------------------------------------------
   Function : CharacterRespondToSpinMovementInput
   Purpose : controls the movement of a character while spinning based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR, (opt/false) whether or not to lock character's Y angle during the spin
   Returns : 
   Info : Will modify a characters direction and speed
*/
void CharacterRespondToSpinMovementInput(struct TAG_PADCONTROLSTATUS *controller,
										 struct TAG_CAMERAINFO *cameraInfo,
										 ACTORINSTANCE *actorInstance,
										 const bool lockYAng=false);

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToSlideMovementInput
   Purpose : controls the movement of a character while sliding based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/
void CharacterRespondToSlideMovementInput(struct TAG_PADCONTROLSTATUS *controller, struct TAG_CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToSwingMovementInput
   Purpose : controls the movement of a character while swinging based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/
void CharacterRespondToSwingMovementInput(struct TAG_PADCONTROLSTATUS *controller, struct TAG_CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToMovementInput
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/

void CharacterRespondToPlungerMovementInput(struct TAG_PADCONTROLSTATUS *controller, struct TAG_CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToMovementInput
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/

void CharacterRespondToMovementInput(struct TAG_PADCONTROLSTATUS *controller, struct TAG_CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToMovementInput
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/

void CharacterRespondToMouseMovementInput(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance);


/* --------------------------------------------------------------------------------
   Function : CharacterRespondToMovementInput
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/

void CharacterRespondToGloverBallMovementInput(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToMovementInput
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/

void CharacterRespondToCrateMovementInput(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToMovementInput
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/

void CharacterRespondToPortalMovementInput(struct TAG_PADCONTROLSTATUS *controller, struct TAG_CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToMesmerisedMovementInput
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/

void CharacterRespondToMesmerisedMovementInput(struct TAG_PADCONTROLSTATUS *controller, struct TAG_CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToDebugMovementInput
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/

void CharacterRespondToDebugMovementInput(struct TAG_PADCONTROLSTATUS *controller, struct TAG_CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToWaterSlideMovementInput
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/

void CharacterRespondToWaterSlideMovementInput(struct TAG_PADCONTROLSTATUS *controller, struct TAG_CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToMovementInputOnIce
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/
void CharacterRespondToMovementInputOnIce(struct TAG_PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToMovementInputInSphere
   Purpose : controls the movement of a character based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTOR
   Returns : 
   Info : Will modify a characters direction and speed
*/
void CharacterRespondToMovementInputInSphere(struct TAG_PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance);


/* --------------------------------------------------------------------------------
   Function : CharacterRotateToAngle
   Purpose : Rotate a character to a given angle taking into account turn speeds
   Parameters : ACTORINSTANCE *actorInstance, float angle in rads to face - clockwise (0 Rads is +Z)
   Returns : 
   Info : Will modify a characters direction
*/
void CharacterRotateToAngle(ACTORINSTANCE *actorInstance,float xAngle,float yAngle,float zAngle, float speed);

/* --------------------------------------------------------------------------------
   Function : ChooseMovementAnimationForTaz
   Purpose : Selects the appropriate animation based on the current pad status
   Parameters : PADCONTROLSTATUS, ACTORINSTANCE
   Returns : the animation segment to play
   Info : 
*/
TBActorAnimSegment *ChooseMovementAnimationForTaz(struct TAG_PADCONTROLSTATUS *controller, ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ChooseMovementAnimationForTazInSphere
   Purpose : Selects the appropriate animation based on the current pad status
   Parameters : PADCONTROLSTATUS, ACTORINSTANCE
   Returns : the animation segment to play
   Info : 
*/
TBActorAnimSegment *ChooseMovementAnimationForTazInSphere(struct TAG_PADCONTROLSTATUS *controller, ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ChooseIdleAnimationForTaz
   Purpose : Selects the appropriate animation based on the current pad status
   Parameters : PADCONTROLSTATUS, ACTORINSTANCE
   Returns : the animation segment to play
   Info : 
*/

TBActorAnimSegment *ChooseIdleAnimationForTaz(struct TAG_PADCONTROLSTATUS *controller, ACTORINSTANCE *actorInstance, struct TAG_CAMERAINFO *cameraInfo);

/* --------------------------------------------------------------------------------
   Function : ChooseIdleAnimationForTaz
   Purpose : Selects the appropriate animation based on the current pad status
   Parameters : PADCONTROLSTATUS, ACTORINSTANCE
   Returns : the animation segment to play
   Info : 
*/

TBActorAnimSegment *ChooseIdleAnimationForCredits(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToMovementWhileJumping
   Purpose : controls the movement of a character while jumping based on pad input
   Parameters : PADCONTROLSTATUS, CAMERAINFO, ACTORINSTANCE, (opt/false) lock Y angle during jump
   Returns : 
   Info : Will modify a characters direction and speed
*/
void CharacterRespondToMovementWhileJumping(struct TAG_PADCONTROLSTATUS *controller,
											struct TAG_CAMERAINFO *cameraInfo,
											ACTORINSTANCE *actorInstance,
											const bool lockYAng=false);

/* --------------------------------------------------------------------------------
   Function : ResetCharacterAfterJump
   Purpose : called when a character lands after a jump move, resets the character
   Parameters : ACTORINSTANCE *actorInstance
   Returns : 
   Info : 
*/
void ResetCharacterAfterJump(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : TazBite
   Purpose : called each frame while Taz is performing a bite move.
   Parameters : ACTORINSTANCE *actorInstance
   Returns : 
   Info : 
*/
void TazBite(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function		: TazAttack
   Purpose		: called each frame while Taz is performing an attack move
   Parameters	: ACTORINSTANCE* to Taz, attack node name, (opt/NULL=none) pos to use in the absence of a node name
   Returns		: // PP: true if the attack has connected with someone/something, else false
   Info			: 
*/
bool TazAttack(ACTORINSTANCE *actorInstance, char *nodeName, TBVector pos=NULL);

/*  --------------------------------------------------------------------------------
	Function	: TazSpit
	Purpose		: Get taz to spit out whats in mouth
	Parameters	: ACTORINSTANCE
	Returns		: 
	Info		: 
*/
struct TAG_DEBRIS *TazSpit(ACTORINSTANCE *actorInstance, int32 item=0, TBVector position=NULL, TBVector velocity=NULL, float time=0.0f, float size=125.0f);

/* --------------------------------------------------------------------------------
   Function : TazScare
   Purpose : called each frame while Taz is performing a scare move.
   Parameters : ACTORINSTANCE *actorInstance
   Returns : 
   Info : 
*/
void TazScare(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function		: GetYAngleBetweenLinesOfSight
   Purpose		: calculates the y angle between two actor instances' lines of sight /
					determines to what extent two characters are facing each other.
   Parameters	: ptr to TBActorInstance 1, ptr to TBActorInstance 2
   Returns		: the smallest angle between the two lines of sight, in the range 0..PI:
					PI means they are facing each other
   Info			: // PP: this takes into account the XZ positions of the two actor instances.
					That's how it's different to just getting the angle between
					the two actor instances' Y angles.
					For example, this function is used in the Zoo boss game
					to determine if Taz and the elephant are facing each other when you
					try to do the scare move.
*/
float GetYAngleBetweenLinesOfSight(TBActorInstance* const actorInstance1,TBActorInstance* const actorInstance2);

/* --------------------------------------------------------------------------------
   Function		: GetYAngleFromLineOfSight
   Purpose		: calculates the Y angle from a character's line of sight to another character
   Parameters	: ptr to TBActorInstance 1, ptr to TBActorInstance 2
   Returns		: the smallest number of radians that the first character would have to turn
					in order to face the second character
   Info			: // PP: For example in the Zoo boss game, this tells me how far around the
						elephant Taz is, so I know whether he's going to get prodded
						by its tusks or not
*/
float GetYAngleFromLineOfSight(TBActorInstance* const actorInstance1, TBActorInstance* const actorInstance2);

/*  --------------------------------------------------------------------------------
	Function	: DrawBlinkEyes
	Purpose		: Spin through characterInstances list to see if there are any blink eyes to draw
	Parameters	: camera currently in use
	Returns		: 
	Info		: 
*/
void DrawBlinkEyes(CAMERAINFO *cameraInfo);

#endif