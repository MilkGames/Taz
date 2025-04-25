#ifndef __H_DESTRUCTIBLES
#define __H_DESTRUCTIBLES

#include "fireball.h"
#include "colour.h"
#include "maths.h"
#include "globalsettings.h"

#ifndef ABILITY_TO_SPELL
#define UpdateSpiningDestructibleObjects	UpdateSpinningDestructibleObjects
#endif // ndef ABILITY_TO_SPELL

#define	DESTRUCTIBLETYPE_INITIALSPINRATE	(RAD(3750.0f))
#define DESTRUCTIBLETYPE_SPINDOWNRATE		(RAD(1875.0f))

#define DESTRUCTIBLETYPE_SPARKTIME			(0.1f)				// CMD: time between no sparky sparks (sparky sparks are created at the rate of 1 per frame)

enum
{
	DESTRUCTIBLETYPE_NONE,
	DESTRUCTIBLETYPE_SPIN,
	DESTRUCTIBLETYPE_CONTACT,
	DESTRUCTIBLETYPE_RESPONDTO,
	DESTRUCTIBLETYPE_TRIGGER,
	DESTRUCTIBLETYPE_ANIMATE,
	DESTRUCTIBLETYPE_ANIMATECONTACT,
	DESTRUCTIBLETYPE_ANIMATELOOP,
	DESTRUCTIBLETYPE_OBJECTSPIN,
	DESTRUCTIBLETYPE_WBTNTCRATE,				// PP: exploding TNT crates for the West Boss game - each sub-object is projected outwards to bounce round the world.
	DESTRUCTIBLETYPE_SPARK,
	DESTRUCTIBLETYPE_SPIKEY,					// TP: indestructible unless chilli fired, spikey like lava otherwise (wow what a sentance of jibberish)
	DESTRUCTIBLETYPE_SPINNER,
};

enum 
{
	DESTRUCTIBLEFLAG_INDESTRUCTIBLE = 1<<0,			// TP: object is currently indestructible
	DESTRUCTIBLEFLAG_UPDATECOLLAFTERANIM = 1<<1,	// TP: for animated destructibles to update collision when destroy anim has finished
	DESTRUCTIBLEFLAG_DONTFADEOUT = 1<<2,			// TP: dont fade object out after final destroy anim has finished
	DESTRUCTIBLEFLAG_NOBOUNCE = 1<<3,				// TP: flag to disable the bounce operaters
	DESTRUCTIBLEFLAG_NODELAY = 1<<4,				// TP: no delay between collisions
	DESTRUCTIBLEFLAG_COUNT = 1<<5,					// TP: this destructible should be counted in the level destructible total
	DESTRUCTIBLEFLAG_NOSOUND = 1<<6,				// TP: destructible should not make any auto sounds
	DESTRUCTIBLEFLAG_DESTROYAFTERANIM = 1<<7,		// TP: after the last animation has been played the object should be destroyed
	DESTRUCTIBLEFLAG_NORANGEDATTACKS = 1<<8,		// TP: dont allow ranged attacks like spit, projectiles or rappa attack
	DESTRUCTIBLEFLAG_CONTAINSWANTED = 1<<9,			// TP: this destructible contains a wanted message
};

typedef struct TAG_DESTRUCTIBLEDEBRISINFO
{
	uint32		debrisCRC[7];
	uint32		numDebris;
} DESTRUCTIBLEDEBRISINFO, _DESTRUCTIBLEDEBRISINFO BALIGN16;

typedef struct TAG_DESTRUCTIBLEANIMS
{
	uint32			idle;
	uint32			first;
	uint32			penultimate;
	uint32			destroy;
}DESTRUCTIBLEANIMS, _DESTRUCTIBLEANIMS BALIGN16;

typedef struct TAG_SPARKINFO
{
	float		sparkClock;
	int32		numTextures;
	uint32		*sparkTextureCRC;
	TBTexture	**sparkTexture;
} SPARKINFO, _SPARKINFO BALIGN16;

typedef struct TAG_EXPLOSIONINFO
{
	int			type;
	COLOUR		expCols[FB_NUM_STAGE_COLOURS];
} EXPLOSIONINFO, _EXPLOSIONINFO;

typedef struct TAG_DESTRUCTIBLEOBJECTINFO
{
	short					type;				// destructible type SPIN / CONTACT / RESPONDTO
	short					flags;				// flags
	int						hits;				// number of hits need to destroy, default is 1
	int						maxHits;			// TP: actual number of hits when object was created
	float					time;
	DESTRUCTIBLEDEBRISINFO	*debris;			// crc of object to use for debris or 0 default

	uint32					destructibleVersion;// crc of object to use for destruction purposes or 0 for no object
	struct TAG_TRIGGERINFO  **hitTriggers;		// TP: messages for the different hits

	uint32					*animation;			// TP: array of animation crc's for the different hits
	uint32					idle;				// TP: idle animation
	int						money;				// TP: value of the destructible
	float					rotation;			// angle of rotation of object
	float					xOffset;			// CMD: max absolute value of debris offset

	float					yOffset;
	float					zOffset;
	int						componentDebris;
	uint32					platformCRC;		// TP: crc of platform to follow or 0
	float					bounce;				// TP: bounce value of destructible in meter per second

	EXPLOSIONINFO			*explosion;			// TP: string containing explosion mesage info	

	SPARKINFO				*sparkInfo;

} DESTRUCTIBLEOBJECTINFO, _DESTRUCTIBLEOBJECTINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateDestructibleObjectInstance
   Purpose : Create a destructibleObjectInstance
   Parameters : ACTORINSTANCE, type (DESTRUCTIBLETYPE_...), destructible verion or 0
   Returns : 
   Info : 
*/
int CreateDestructibleObjectInstance(ACTORINSTANCE *actorInstance, int type, DESTRUCTIBLEDEBRISINFO *debris, 
									 int hits, uint32 idle, uint32 firstHit, uint32 secondHit, uint32 destroy,char *destructibleVersion, 
									 float xOffset, float yOffset, float zOffset,int componentDebris, uint32 money=10, uint32 platformCRC = 0,short flags = 0,float bounce = METERTOUNIT(10.0f));

/* --------------------------------------------------------------------------------
   Function : PreResponToDestructibleCollision
   Purpose : Function to be called when a collision with a destructible has occured
   Parameters : ACTORINSTANCE object hitting rock, ACTORINSTANCE *destructible
   Returns : 
   Info : 
*/
int PreRespondToDestructibleCollision(ACTORINSTANCE *collider, ACTORINSTANCE *destructible, struct TAG_PRECOLLINFO *info);

/* --------------------------------------------------------------------------------
   Function : ResponToDestructibleCollision
   Purpose : Function to be called when a collision with a destructible has occured
   Parameters : ACTORINSTANCE object hitting rock, ACTORINSTANCE *destructible
   Returns : 
   Info : 
*/
int RespondToDestructibleCollision(ACTORINSTANCE *collider, ACTORINSTANCE *destructible);

/* --------------------------------------------------------------------------------
   Function : CopyDestructibleDebrisInfo
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/
DESTRUCTIBLEDEBRISINFO *CopyDestructibleDebrisInfo(DESTRUCTIBLEDEBRISINFO *debris);

/* --------------------------------------------------------------------------------
   Function : ChangeIntoDestructibleVersion
   Purpose : If the destructible has a special version for when its destroyed then change
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
int ChangeIntoDestructibleVersion(ACTORINSTANCE *destructible, BOOL destroyOldObject);

/* --------------------------------------------------------------------------------
   Function : CheckForFurtherDestructions
   Purpose : Look for destructible object touching destroyed object
   Parameters : ACTORINSTANCE, ACTORINSTANCELIST
   Returns : 
   Info : 
*/
void CheckForFurtherDestructions(ACTORINSTANCE *actorInstance,ACTORINSTANCE *destructible, ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : UpdateSpiningDestructibleObjects
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/
void UpdateSpiningDestructibleObjects(ACTORINSTANCE *ptr);

/*  --------------------------------------------------------------------------------
	Function	: SetupDestructible
	Purpose		: Create a destructible object from the map attribs
	Parameters	: currentmap, node, instanceAttribs, instance
	Returns		: 
	Info		: 
*/
void SetupDestructible(struct TAG_MAPINFO *currentMap, class CPMNode *node, class CPMNode *rootNode, struct TAG_MAPOBJECTATTRIBS *instanceAttribs, ACTORINSTANCE *tempInstance);

/*  --------------------------------------------------------------------------------
	Function	: BounceCharacter
	Purpose		: Reflect a characters velocity against a normal with an elasticity factor
	Parameters	: actorInstance, normal of plane to reflect against. 0-1 elasticity factor (1 - perfect reflection, 0 - stop dead
	Returns		: 
	Info		: 
*/
void BounceCharacter(ACTORINSTANCE *actorInstance, TBVector normal, float elasticity);

/*  --------------------------------------------------------------------------------
	Function	: FindTexturesFromMapUsedBySparkingObjects
	Purpose		: Once all of the map has been loaded we can look for the textures used by sparking objects
	Parameters	: none
	Returns		: nothing
	Info		: check out the long function name
*/

void FindTexturesFromMapUsedBySparkingObjects(void);

/*  --------------------------------------------------------------------------------
	Function	: BounceCharacter
	Purpose		: Reflect a characters velocity against a normal with an elasticity factor
	Parameters	: actorInstance, bounceActor. 0-1 elasticity factor (1 - perfect reflection, 0 - stop dead), hits left
	Returns		: 
	Info		: 
*/
void BounceCharacter(ACTORINSTANCE *actorInstance, ACTORINSTANCE *bounceActor, float speed, int hitsLeft=0);

/*  --------------------------------------------------------------------------------
	Function	: DestroyEverything
	Purpose		: Starts destroying everything on the level
	Parameters	: 
	Returns		: 
	Info		: 
*/

void DestroyEverything(void);

/*  --------------------------------------------------------------------------------
	Function	: UpdateDestroyEverything
	Purpose		: Called each frame to destroy level stuff
	Parameters	: ptr to destructible object
	Returns		: 
	Info		: 
*/

void UpdateDestroyEverything(ACTORINSTANCE *ptr);


#endif