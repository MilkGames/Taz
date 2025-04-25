#ifndef __H_DEBRIS
#define __H_DEBRIS

#include "physics.h"
#include "precipitation.h"

#define MAX_DEBRIS	(500)
#define MAX_SMOKE	(200)
#define MAX_BUBBLES	(250)

#define DEBRISFLAG_GRAVITY			(1<<0)
#define DEBRISFLAG_SNOW				(1<<1)
#define	DEBRISFLAG_IMPACTCLOUD		(1<<2)
#define	DEBRISFLAG_PHYSICS			(1<<3)
#define DEBRISFLAG_EXPLOSIONSMOKE	(1<<4)
#define DEBRISFLAG_RAIN				(1<<5)
#define DEBRISFLAG_ROTATESPRITE		(1<<6)
#define DEBRISFLAG_BUBBLES			(1<<7)
#define DEBRISFLAG_AFFECTOBJECTS	(1<<8)	// TP: tells the collision system to process respond to collision callbacks
#define DEBRISFLAG_CHILLISMOKE		(1<<9)
#define DEBRISFLAG_CHILLIFIRE		(1<<10)
#define DEBRISFLAG_WHITEBUBBLE		(1<<11)
#define DEBRISFLAG_GREENBUBBLE		(1<<12)
#define DEBRISFLAG_POWERUPGLOW		(1<<13)
#define DEBRISFLAG_TWINKLE			(1<<14)
#define	DEBRISFLAG_SHOWER			(1<<15)
#define DEBRISFLAG_STICKY			(1<<16)	// TP: debris will stick to objects
#define DEBRISFLAG_INCEDENTALFIRE	(1<<17)
#define DEBRISFLAG_LOWGRAVITY		(1<<18)
#define DEBRISFLAG_SPRITE			(1<<19)
#define DEBRISFLAG_PRECIPITATION	(1<<20)
#define DEBRISFLAG_POINTINDIRECTION (1<<21)
#define DEBRISFLAG_DONTDOCOLLISION	(1<<22)
#define DEBRISFLAG_FOODSHOWER		(1<<23)
#define DEBRISFLAG_FINGERSMOKE		(1<<24)
#define DEBRISFLAG_SCALEOUT			(1<<25)		// TP: object will scale out quickly
#define DEBRISFLAG_SCALEUP			(1<<26)		// TP: object will scale up quickly, note the actorInstance scale must be set to 0 by user after creating the debris
#define DEBRISFLAG_DOCOLLISION		(1<<27)
#define DEBRISFLAG_TWINKLEGLOW		(1<<28)

typedef struct TAG_BUBBLESEXTRA
{
	TBVector	position;
	TBVector	velocity;

	TBTexture	*texture;
	float		life;
	float		size;
	float		rotation;

	struct
	{
		int32	r, g, b;
		float	a;
	}colour;

	uint32		flags;
	float		ground;
	float		yScale;
	float		xScale;
	float		scaleClock;
}BUBBLESEXTRA, _BUBBLESEXTRA BALIGN16;

typedef struct TAG_SPRITEDEBRIS
{
	TBTexture			*texture;
	float				xDim;				// x dimension of texture
	float				yDim;
	int					alpha;

	float				rotSpeed;			// CMD: rotation speed of sprite, only used if rotate sprite flag set.
	int					colour[3];

	float				ground;				// CMD: world y coords of ground
} SPRITEDEBRIS, _SPRITEDEBRIS BALIGN16;

// TP: impactcallback for collisions involving debris return value is 0 for destroy or 1 to continue
typedef int (*ImpactCallback)(struct TAG_DEBRIS *debris, COLL_INFO *collInfo);

typedef void (*LifeOverCallback)(struct TAG_DEBRIS *debris);

typedef struct TAG_DEBRIS
{
	TBVector			position;		

	TBVector			velocity;

	TBVector			rotationSpeed;		// rotation speed in rads per second

	SPRITEDEBRIS					sprite;			// if debris is a sprite / null if not
	struct TAG_PRECIPITATION		precipitation;		// if debris is precipitation / null if not

	TBActorInstance					*actorInstance;		// if debris is an actor / null if not
	struct TAG_ACTORINSTANCE		*actorStructure;	// if debris need physics/collision / null if doesn't

	ImpactCallback		impactCallback;		// TP: function to call when the debris collides with something
	LifeOverCallback	lifeOverCallback;	// TP: called when the object is destroyed

	float				life;				// life in seconds - 0 if infinite
	uint32				flags;
	float				scale;				// scale of object
	int					playerNo;			// player number, only used for chillipepper fire ;->
} DEBRIS, _DEBRIS BALIGN16;

typedef struct TAG_SMOKEEXTRAINFO
{
	TBVector			position;
	TBVector			velocity;
	TBVector			result;

	TBTexture			*texture;
	float				life;
	float				size;
	float				rotAngle;

	uint32				flags;
	float				alpha;
	float				resistence;
	float				clock;
} SMOKEEXTRAINFO, _SMOKEEXTRAINFO BALIGN16;

extern TBExtraSystem	*debrisSystem;
extern TBExtraSystem	*smokeSystem;
extern TBExtraSystem	*chimneyPuffSystem;
extern TBExtraSystem	*precipitationSystem;
extern TBExtraSystem	*bubbleSystem;
extern int			numRain;

/* --------------------------------------------------------------------------------
   Function : InitDebrisSystem
   Purpose : call this function to create the collectible extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitDebrisSystem(void);

/* --------------------------------------------------------------------------------
   Function : CreateDebrisActor
   Purpose : Call this function to create a collectilble actor
   Parameters : actor to create collectible from
   Returns : TRUE /FALSE
   Info : 
*/
struct TAG_DEBRIS *CreateDebrisActor(char *actorName, TBVector position, TBVector velocity, TBVector rotationSpeed, float life, float scale, float xDim, float yDim, int alpha, uint32 flags, int xNumBoxes, int zNumBoxes,float spriteRotVel);

/* --------------------------------------------------------------------------------
   Function : CreateDebrisActor
   Purpose : Call this function to create a collectilble actor
   Parameters : actor to create debris from,pos,life,radius,type
   Returns : TRUE /FALSE
   Info : 
*/

struct TAG_DEBRIS *CreateDebrisActor(uint32 actorName, TBVector position, TBVector velocity, TBVector rotationSpeed, float life, float scale, float xDim, float yDim, int alpha, uint32 flags, int xNumBoxes, int zNumBoxes,float spriteRotVel);

/* --------------------------------------------------------------------------------
   Function : CreateDebrisActor
   Purpose : Call this function to create a collectilble actor
   Parameters : actor to create debris from,pos,life,radius,type
   Returns : TRUE /FALSE
   Info : 
*/

struct TAG_DEBRIS *CreateDebrisActor(TBTexture *texture, TBVector position, TBVector velocity, TBVector rotationSpeed, float life, float scale, float xDim, float yDim, int alpha, uint32 flags, int xNumBoxes, int zNumBoxes,float spriteRotVel);


/* --------------------------------------------------------------------------------
   Function : CreateDebrisActor
   Purpose : Call this function to create a collectilble actor
   Parameters : actor to create debris from,pos,life,radius,type
   Returns : TRUE /FALSE
   Info : 
*/

//struct TAG_DEBRIS *CreateDebrisActor(struct TAG_PRECIPITATION *precipitation, TBVector position, TBVector velocity, TBVector rotationSpeed, float life, float scale, float xDim, float yDim, int alpha, uint32 flags, int xNumBoxes, int zNumBoxes,float spriteRotVel);

/* --------------------------------------------------------------------------------
   Function : DebrisCreateCallback
   Purpose : This function is called when a collectible is created
   Parameters : 
   Returns : 
   Info : 
*/
void DebrisExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms);

/* --------------------------------------------------------------------------------
   Function : SmokeExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void SmokeExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms);

/* --------------------------------------------------------------------------------
   Function : DebrisExtraDeleteCallback
   Purpose : This function is called when a collectible is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int DebrisExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata);

/* --------------------------------------------------------------------------------
   Function : SmokeExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int SmokeExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata);

/* --------------------------------------------------------------------------------
   Function : DebrisExtraUpdateCallback
   Purpose : This function is called each frame to update the collectibles
   Parameters : 
   Returns : 
   Info : 
*/
void DebrisExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);

/* --------------------------------------------------------------------------------
   Function : SmokeExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void SmokeExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);

/* --------------------------------------------------------------------------------
   Function : DebrisExtraUpdateCallback
   Purpose : This function is called each frame to render the collectibles
   Parameters : 
   Returns : 
   Info : 
*/
void DebrisExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);

/* --------------------------------------------------------------------------------
   Function : DebrisExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/

void SmokeExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);

/* --------------------------------------------------------------------------------
   Function : BubblesExtraSystemRenderCallback
   Purpose : This function is called each frame to render the bubbles
   Parameters : 
   Returns : 
   Info : 
*/

void BubblesExtraSystemRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);

/* --------------------------------------------------------------------------------
   Function : DebrisExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/

void ChimneyPuffExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);

/* --------------------------------------------------------------------------------
   Function : DebrisExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/
void DebrisExtraSystemInitCallback(TBExtraSystem *eSystem, int reason);

/* --------------------------------------------------------------------------------
   Function : SmokeExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/

void SmokeExtraSystemInitCallback(TBExtraSystem *eSystem, int reason);

/* --------------------------------------------------------------------------------
   Function : AddActorInstanceStructureToDebris
   Purpose : Adds ACTORINSTANCE structure to debris extra actor
   Parameters : 
   Returns : 
   Info : 
*/

struct TAG_ACTORINSTANCE *AddActorInstanceStructureToDebris(DEBRIS *debrisInstance, char *name);

/* --------------------------------------------------------------------------------
   Function : AddActorBehaviourToDebris
   Purpose : Adds actorBehaviour structure to debris extra actor
   Parameters : 
   Returns : 
   Info : 
*/

int AddActorBehaviourToDebris(DEBRIS *debrisInstance, struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : BubblesExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void BubblesExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);

/* --------------------------------------------------------------------------------
   Function : BubblesExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int BubblesExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata);

/* --------------------------------------------------------------------------------
   Function : BubblesExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/

void BubblesExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms);

/* --------------------------------------------------------------------------------
   Function : DebrisExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/

void BubblesExtraSystemInitCallback(TBExtraSystem *eSystem, int reason);


#endif