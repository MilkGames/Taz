#ifndef	__H_DOORS
#define	__H_DOORS

#include "global.h"
#include "main.h"
#include "glowaroundplinth.h"

enum
{
	DOORTYPE_REVOLVING,
	DOORTYPE_SPRINGY,
};

enum
{
	DOOR_ROTATION_CLOCKWISE,
	DOOR_ROTATION_ANTICLOCKWISE,
	DOOR_NO_UPDATE,
	DOOR_SPIN_UPDATE,
};

typedef struct TAG_DOORINFO
{
	TBQuaternion	defaultOrientation;

	float			springyness;
	float			stiffness;
	float			timeSinceLastCollision;
	char			update;
	char			collision;
	char			rotation;
	char			state;

	float			currentAmplitude;
	int32			type;
	float			oscillationTime;
	float			currentAngle;
} DOORINFO, _DOORINFO BALIGN16;

extern	struct TAG_BOOK	*XDoorBook;

/* --------------------------------------------------------------------------------
   Function : CreateSwingingDoor
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateDoor(ACTORINSTANCE *actorInstance, float springyness, float stiffness, int32 type);

/* --------------------------------------------------------------------------------
   Function : CreateSwingingDoor
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateRevolvingDoor(ACTORINSTANCE *actorInstance, float springyness, float stiffness, int32 type);

/* --------------------------------------------------------------------------------
   Function : CreateSpringyDoor
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateSpringyDoor(ACTORINSTANCE *actorInstance, float springyness, float stiffness, int32 type);

/* --------------------------------------------------------------------------------
   Function : CreateDoorInfo
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

DOORINFO *CreateDoorInfo(void);

/* --------------------------------------------------------------------------------
   Function : UpdateDoors
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateDoors(ACTORINSTANCE *ptr);

/* --------------------------------------------------------------------------------
   Function : UpdateDoorOrientation
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateRevolvingDoorOrientation(ACTORINSTANCE *door);

/* --------------------------------------------------------------------------------
   Function : RespondToDoorCollision
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateSpringyDoorOrientation(ACTORINSTANCE *door);

/* --------------------------------------------------------------------------------
   Function : RespondToDoorCollision
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToRevolvingDoorCollision(ACTORINSTANCE *collider, ACTORINSTANCE *door);

/* --------------------------------------------------------------------------------
   Function : RespondToDoorCollision
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToSpringyDoorCollision(ACTORINSTANCE *collider, ACTORINSTANCE *door);


/* TP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PLANET X DOOR

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

enum EXDoorState
{
	XDOOR_INACTIVE,
	XDOOR_ENTER,
	XDOOR_LIMBO,
	XDOOR_EXIT,
};

typedef struct TAG_XDOORINFO
{
	uint32						destDoorCRC;			// TP: crc of the destination door or 0 if door is terminal
	struct	TAG_TRIGGERINFO		*startEnter;			// TP: messages sent when you start to enter the door
	struct	TAG_TRIGGERINFO		*finishedEnter;			// TP: messages sent when you finish entering the door
	struct	TAG_TRIGGERINFO		*startExit;				// TP: messages sent when you start to exit the door
	struct	TAG_TRIGGERINFO		*finishedExit;			// TP: messages sent when you finish exiting the door
	EXDoorState					state;					// TP: current state of the door
	int							level;					// TP: level to enter if no dest door
	float						delayToNextDoor;		// TP: delay to set the dest door to
	float						delay;					// TP: delay before exiting
	PLINTHGLOWINFO				*glowInfo;
} XDOORINFO;

/*  --------------------------------------------------------------------------------
	Function	: UpdateXDoorState
	Purpose		: Update called during the x door move
	Parameters	: actorInstance of character using door
	Returns		: true if move finished
	Info		: 
*/
bool UpdateXDoorState(ACTORINSTANCE *actorInstance);

/*  --------------------------------------------------------------------------------
	Function	: CreateActorInstanceXDoor
	Purpose		: Creates and initialises a x door
	Parameters	: name of object, instance name, position, orientation, scale, rootNode, node
	Returns		: true if created, fals if not
	Info		: 
*/
bool CreateActorInstanceXDoor(char *name, char *instance, uint32 destDoor, TBVector position, TBVector orientation, TBVector scale, CPMNode *rootNode, CPMNode *node);

/*  --------------------------------------------------------------------------------
	Function	: EnterSceneUsingXDoor
	Purpose		: Character has entered the scene using a x door
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
bool EnterSceneUsingXDoor(ACTORINSTANCE *actorInstance, uint32 xDoorCRC);

/*  --------------------------------------------------------------------------------
	Function	: FreeXDoor
	Purpose		: Free an x door
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
void FreeXDoor(ACTORINSTANCE *actorInstance);

/*  --------------------------------------------------------------------------------
	Function	: UpdateXDoorSparkles
	Purpose		: 
	Parameters	: actorInstance of door
	Returns		: 
	Info		: 
*/

void UpdateXDoorSparkles(ACTORINSTANCE *actorInstance);

#endif