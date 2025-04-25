#ifndef __H_SECURITYBOX
#define __H_SECURITYBOX

#include "linkedlist.h"

#define SECBOXARMSEGS (20)
#define SECBOXARMSEGSF (20.0f)

enum ESecurityBoxState
{
	SECBOX_INITIALISING,							// TP: Security box has just been created
	SECBOX_IDLE,									// TP: Security box is just idling
	SECBOX_STARTATTACK,								// TP: Security box has been violated, must retaliate
	SECBOX_ATTACKING,								// TP: Security box fights back like a girl
	SECBOX_RETRACT,									// TP: Security box runs away crying
	SECBOX_SHUTDOWN,								// TP: Security box is dissabled, destroy...
};

typedef class TAG_SECURITYBOX
{
private:
	static LINKEDLIST<TAG_SECURITYBOX>	list;						// TP: List of all security boxes in existance
	
	ACTORINSTANCE						*actorInstance;				// TP: ACTORINSTANCE for security box to use
	ESecurityBoxState					state;						// TP: current state of the security box

	ACTORINSTANCE						*weapon;					// TP: weapon on end of arm

	uint32								triggerCRC;					// TP: crc of trigger box instance to use
	ACTORINSTANCE						*triggerBox;				// TP: trigger box to activate security box
									
	struct TAG_PLAYER					*playerAttacking;			// TP: Pointer to the player box is attacking
									
	TBVector							armPoints[SECBOXARMSEGS];	// TP: guide points on security box arm, these are only valid during attacks
	float								armT;						// TP: 0-1, length of arm
	float								shutdownTime;				// TP: time for security box to shutdown for

	/*  --------------------------------------------------------------------------------
		Function	: SECURITYBOX::Init
		Purpose		: Create a security box
		Parameters	: initial position, initial orientation, crc of trigger instance
		Returns		: TRUE/FALSE
		Info		: 
	*/
	int Init(TBVector position, TBQuaternion orientation, uint32 triggerCRC, class CPMNode *rootNode = NULL, class CPMNode *node = NULL);

	/*  --------------------------------------------------------------------------------
		Function	: SECURITYBOX::reactivate
		Purpose		: re-activate, eg. when the timer has run out
		Parameters	: 
		Returns		: 
		Info		: // PP: made this into a method cos it needs to be cheat-controlled as well
	*/
	void Reactivate(void);

	/*  --------------------------------------------------------------------------------
		Function	: SECURITYBOX::Update
		Purpose		: Perform and update
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	void Update();

	/*  --------------------------------------------------------------------------------
		Function	: SECURITYBOX::Draw
		Purpose		: Draw any extras that the security box has
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	void Draw();

	/*  --------------------------------------------------------------------------------
		Function	: SECURITYBOX::CalculateArm
		Purpose		: Calculates points along the arm to use in draw streak
		Parameters	: 
		Returns		: 
		Info		: Fills in armPoints ready for drawing
	*/
	void CalculateArm();

	/*  --------------------------------------------------------------------------------
		Function	: SECURITYBOX::CalculateTextureCoords
		Purpose		: Fill in texture coords of arm for use with extendy arm texture
		Parameters	: prims, numPrims
		Returns		: new uv's in prims
		Info		: 
	*/
	void CalculateTextureCoords(TBPrimVertex2D *prims, int numPrims);

	/*  --------------------------------------------------------------------------------
		Function	: SECURITYBOX::Shutdown
		Purpose		: Shutdown the security box for given time
		Parameters	: (// PP: opt/-1.0f=forever) time to shutdown for
		Returns		: 
		Info		: 
	*/
	void Shutdown(float time = -1.0f);

	/*  --------------------------------------------------------------------------------
		Function	: SECURITYBOX::SelectWeapon
		Purpose		: Choose a weapon and create the instance
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	void SelectWeapon(void);

	/*  --------------------------------------------------------------------------------
		Function	: SECURITYBOX::FreeWeapon
		Purpose		: Get rid of the weapon used
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	void FreeWeapon(void);
	
public:

	/*  --------------------------------------------------------------------------------
		Function	: GetState
		Purpose		: Return the current state of the security box
		Parameters	: 
		Returns		: 
		Info		: 
	*/

	ESecurityBoxState GetState()
	{
		return state;
	}

	/*  --------------------------------------------------------------------------------
		Function	: KillSecurityBoxes
		Purpose		: Removes all security boxes from a level
		Parameters	: void
		Returns		: 
		Info		: 
	*/
	friend void KillSecurityBoxes(void);

	/*  --------------------------------------------------------------------------------
		Function	: UpdateSecurityBoxes
		Purpose		: Permorms an update on each security box
		Parameters	: void
		Returns		: 
		Info		: 
	*/
	friend void UpdateSecurityBoxes(void);

	/*  --------------------------------------------------------------------------------
		Function	: DrawSecurityBoxes
		Purpose		: Draws any extras that the security box has, for example the extendy arm
		Parameters	: void
		Returns		: 
		Info		: 
	*/
	friend void DrawSecurityBoxes(void);

	/*  --------------------------------------------------------------------------------
		Function	: ShutdownSecurityBoxes
		Purpose		: Shutdown all security boxes
		Parameters	: // PP: (opt/false) shutdown indefinitely?
		Returns		: 
		Info		: 
	*/
	friend void ShutdownSecurityBoxes(const bool forever=false);

	/*  --------------------------------------------------------------------------------
		Function	: ReactivateSecurityBoxes
		Purpose		: Re-activate all security boxes
		Parameters	: 
		Returns		: 
		Info		: // PP: 
	*/
	friend void ReactivateSecurityBoxes(void);

	/*  --------------------------------------------------------------------------------
		Function	: RespondToSecurityBoxCollision
		Purpose		: Destroy security box
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	friend int RespondToSecurityBoxCollision(struct TAG_ACTORINSTANCE *collider, struct TAG_ACTORINSTANCE *secBox);

	/*  --------------------------------------------------------------------------------
		Function	: SECURITYBOX::TAG_SECURITYBOX
		Purpose		: Default constructor
		Parameters	: Initial position, initial orientation, crc of trigger instance
		Returns		: 
		Info		: 
	*/
	TAG_SECURITYBOX(TBVector position = NULL, TBQuaternion orientation = NULL, uint32 triggerCRC = 0, class CPMNode *rootNode = NULL, class CPMNode *node = NULL)
	{
		Init(position, orientation, triggerCRC, rootNode, node);
	}

	/*  --------------------------------------------------------------------------------
		Function	: SECURITYBOX::~TAG_SECURITYBOX
		Purpose		: Default destructor
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	~TAG_SECURITYBOX();

	/*  --------------------------------------------------------------------------------
		Function	: SECURITYBOX::SetPosition
		Purpose		: Set the current position for the security box
		Parameters	: position of box
		Returns		: 
		Info		: 
	*/
	void SetPosition(TBVector position)
	{
		if (actorInstance)	bmVectorCopy(actorInstance->actorInstance.position, position);
	}

	/*  --------------------------------------------------------------------------------
		Function	: SECURITYBOX::SetOrientation
		Purpose		: Set the current orientation for the security box
		Parameters	: position of box
		Returns		: 
		Info		: 
	*/
	void SetOrientation(TBQuaternion orientation)
	{
		if (actorInstance)	bmVectorCopy(actorInstance->actorInstance.orientation, orientation);
	}

} SECURITYBOX, _SECURITYBOX BALIGN16;

enum ESpinPadState
{
	SPINPAD_INITIALISING,								// TP: Spin pad is initialising
	SPINPAD_IDLE,										// TP: Spin pad is in idle mode
	SPINPAD_SPINDOWN,									// TP: Player is spinning on pad
	SPINPAD_COUNTDOWN,									// TP: Security box is shut down, start count down
};

typedef class TAG_SPINPAD
{
private:
	static LINKEDLIST<TAG_SPINPAD>	list;						// TP: List of all spin pads in existance
	
	ACTORINSTANCE					*actorInstance;				// TP: ACTORINSTANCE for spin pad to use
									
	ESpinPadState					state;						// TP: Current state of spin pad

	float							spinOnCounter;				// TP: Current time Taz has been spinning on pad

public:
	static int						soundHandle;				// TP: sound handle only one needed
	static char						spinPadsActive;				// TP: counter of the number of spin pads that have been activated

	/*  --------------------------------------------------------------------------------
		Function	: SPINPAD::Init
		Purpose		: Create and setup a spin pad
		Parameters	: position, orientation
		Returns		: 
		Info		: 
	*/
	int Init(TBVector position, TBQuaternion orientation, int flags);

	/*  --------------------------------------------------------------------------------
		Function	: SPINPAD::Update
		Purpose		: Update a spin pad
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	void Update();

	/*  --------------------------------------------------------------------------------
		Function	: SPINPAD::PlayerSpinOn
		Purpose		: Called each frame player is spinning on pad
		Parameters	: player spinning on pad
		Returns		: 
		Info		: 
	*/
	void PlayerSpinOn(struct TAG_PLAYER *player);

public:
	/*  --------------------------------------------------------------------------------
		Function	: SpinPadCollisionCallback
		Purpose		: Called in responce to collision
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	friend int SpinPadCollisionCallback(struct TAG_ACTORINSTANCE *collider, struct TAG_ACTORINSTANCE *spinPadActor);

	/*  --------------------------------------------------------------------------------
		Function	: KillSpinPads
		Purpose		: Removes all spin pads from a level
		Parameters	: void
		Returns		: 
		Info		: 
	*/
	friend void KillSpinPads(void);

	/*  --------------------------------------------------------------------------------
		Function	: UpdateSpinPads
		Purpose		: Permorms an update on each spin pad
		Parameters	: void
		Returns		: 
		Info		: 
	*/
	friend void UpdateSpinPads(void);

	/*  --------------------------------------------------------------------------------
		Function	: SPINPAD::TAG_SPINPAD
		Purpose		: Default constructor
		Parameters	: position and orientation
		Returns		: 
		Info		: 
	*/
	TAG_SPINPAD(TBVector position = NULL, TBQuaternion orientation = NULL, int flags = 0)
	{
		Init(position, orientation, flags);
	}

	/*  --------------------------------------------------------------------------------
		Function	: SPINPAD::~TAG_SPINPAD
		Purpose		: Default destructor, kills the spin pad
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	~TAG_SPINPAD();

	/*  --------------------------------------------------------------------------------
		Function	: SPINPAD::SetPosition
		Purpose		: Set the current position for the spin pad
		Parameters	: position of box
		Returns		: 
		Info		: 
	*/
	void SetPosition(TBVector position)
	{
		if (actorInstance)	bmVectorCopy(actorInstance->actorInstance.position, position);
	}

	/*  --------------------------------------------------------------------------------
		Function	: SPINPAD::SetOrientation
		Purpose		: Set the current orientation for the spin pad
		Parameters	: position of box
		Returns		: 
		Info		: 
	*/
	void SetOrientation(TBQuaternion orientation)
	{
		if (actorInstance)	bmVectorCopy(actorInstance->actorInstance.orientation, orientation);
	}


} SPINPAD, _SPINPAD BALIGN16;

#endif