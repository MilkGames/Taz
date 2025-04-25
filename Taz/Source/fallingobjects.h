#ifndef __H_FALLINGOBJECTS
#define __H_FALLINGOBJECTS

enum
{
	FALLINGTYPE_ONCE,
	FALLINGTYPE_RANDOM,
	FALLINGTYPE_DESTROY,
	FALLINGTYPE_REMAIN,
	FALLINGTYPE_WAIT,
	FALLINGTYPE_REMAINANDFADE,
	FALLINGTYPE_WAITINGFORFADE,
	FALLINGTYPE_REMAINTODESTRUCTIBLE,
	FALLINGTYPE_REMAINTOSPARKY,
};

#define FALLINGOBJECTFLAG_CAPTURETAZ	(1<<0)

typedef struct TAG_FALLINGOBJECTINFO
{
	TBVector							startPos;
	float								initialHeight;
	struct	TAG_CARTOONSCALEINFO		*cartoonScaleInfo;
	struct TAG_TRIGGERINFO				*onDestruction;
	struct TAG_DESTRUCTIBLEDEBRISINFO	*debris;
	float								timeBeforeFade;
	uint32								instanceNameCRC;
	bool								destroy;
	bool								waitOver;
	bool								collision;
	char								type;
	char								impactType;
	ESpecialType						convertTo;
	ACTORINSTANCE						*owner;
	int									flags;
} FALLINGOBJECTINFO, _FALLINGOBJECTINFO BALIGN16;

typedef struct TAG_FALLINGOBJECTLISTENTRY
{
	TBVector					startPos;
	TBVector					scale;

	float						areaRadius;
	int							period;
	uint32						impactType;
	uint32						type;

	float						startTime;
	struct TAG_TRIGGERINFO		*onDestruction;
	TAG_FALLINGOBJECTLISTENTRY	*next;
	TAG_FALLINGOBJECTLISTENTRY	*prev;

	uint32						nameCRC;					//name of obe
	struct TAG_DESTRUCTIBLEDEBRISINFO		*debris;
	float						initialHeight;
	char						waitOver;					//used to signify when a waiting object should fall	

	uint32						instanceNameCRC;			//name of instance
	bool						invisible;
	bool						active;
} FALLINGOBJECTLISTENTRY, _FALLINGOBJECTLISTENTRY BALIGN16;

typedef struct TAG_FALLINGOBJECTLIST
{
	FALLINGOBJECTLISTENTRY		head;

	uint32						numEntries;
	uint32						pad[3];
} FALLINGOBJECTLIST, _FALLINGOBJECTLIST BALIGN16;

extern	FALLINGOBJECTLIST		fallingObjectList;

/* --------------------------------------------------------------------------------
   Function : InitFallingObjectList
   Purpose : guess what it does!
   Parameters : 
   Returns : 
   Info : 
*/

void InitFallingObjectList(void);

/* --------------------------------------------------------------------------------
   Function : AddEntryToFallingObjectList
   Purpose : Function to store information about objects that are to fall at some point
   Parameters : 
   Returns : 
   Info : 
*/

void AddEntryToFallingObjectList(char *name, char *instanceName, TBVector startPos, TBVector scale, float areaRadius, int period, uint32 impactType, uint32 type, struct TAG_TRIGGERINFO *triggerInfo, struct TAG_DESTRUCTIBLEDEBRISINFO *debris, bool invisible, bool active);

/* --------------------------------------------------------------------------------
   Function : RemoveObjectFromFallingList
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RemoveObjectFromFallingList(FALLINGOBJECTLISTENTRY *remPtr);

/* --------------------------------------------------------------------------------
   Function : UpdateFallingObjectList
   Purpose : updates falling objectlist and decides when to drop objects
   Parameters : FALLINGOBJECTLIST *list
   Returns : 
   Info : 
*/

void UpdateFallingObjectList(FALLINGOBJECTLIST *list);

/* --------------------------------------------------------------------------------
   Function : FreeAllFallingObjectsFromList
   Purpose : Removes all actors from the actor list and free the memory allocated completely
   Parameters : 
   Returns : 
   Info : 
*/
void FreeAllFallingObjectsFromList(void);

/* --------------------------------------------------------------------------------
   Function : DropObject
   Purpose : creates instance to be dropped
   Parameters : name of object, start position
   Returns : 
   Info : 
*/

struct TAG_ACTORINSTANCE *DropObject(ACTORINSTANCELIST *instanceList, uint32 nameCRC, uint32 instanceNameCRC, char *name, TBVector startPos, uint32 type, uint32 impactType, struct TAG_TRIGGERINFO *onDestruction, struct TAG_DESTRUCTIBLEDEBRISINFO *debris, float initHeight,bool invisible,float timeBeforeFade=0.0f,ACTORINSTANCE *owner=NULL, int flags=0);

/* --------------------------------------------------------------------------------
   Function : CreateActorInstancePathInfo
   Purpose : Creates and initialises the path info part of an actorInstance
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
int CreateFallingObjectInstance(ACTORINSTANCE *actorInstance, TBVector startPos);

/* --------------------------------------------------------------------------------
Function : UpdateFallingObject
Purpose : updates position and velocity of falling object
Parameters : actor instance list ptr
Returns : 
Info : 
*/

void UpdateFallingObject(ACTORINSTANCE *ptr);

/* --------------------------------------------------------------------------------
Function : UpdateFallingObject
Purpose : updates position and velocity of falling object
Parameters : actor instance list ptr
Returns : 
Info : 
*/

void UpdateFallingObjectInfo(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : RespondToActorCollision
   Purpose : Function to be called when a collision with an actor has occured
   Parameters : ACTORINSTANCE *actor in collision, ACTORINSTANCE *object hitting actor
   Returns : 
   Info : 
*/
int RespondToActorCollision(ACTORINSTANCE *actor, ACTORINSTANCE *collider);

/* --------------------------------------------------------------------------------
   Function : RespondToActorCollision
   Purpose : Function to be called when a collision with an actor has occured
   Parameters : ACTORINSTANCE *actor in collision, ACTORINSTANCE *object hitting actor
   Returns : 
   Info : 
*/

void FreeAllFallingObjectScaleInfo(ACTORINSTANCELIST *instanceList);

/* --------------------------------------------------------------------------------
   Function : ResetRemainTypeFallingObject
   Purpose : Function to be called when a collision with an actor has occured
   Parameters : ACTORINSTANCE *actor in collision, ACTORINSTANCE *object hitting actor
   Returns : 
   Info : 
*/

int ResetRemainTypeFallingObject(ACTORINSTANCE *actorInstance, ACTORINSTANCE *collider);

/*  --------------------------------------------------------------------------------
	Function	: SetFallingObjectState
	Purpose		: Sets the current state of a falling object generator
	Parameters	: crc of instance, true/false state
	Returns		: 
	Info		: 
*/
void SetFallingObjectState(uint32 crc, bool state);

/* --------------------------------------------------------------------------------
   Function : RemoveObjectFromFallingListByCRC
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/
int RemoveObjectFromFallingListByCRC(uint32 crc);


#endif