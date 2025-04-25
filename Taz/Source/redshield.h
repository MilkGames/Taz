#ifndef __H_REDSHIELD
#define __H_REDSHIELD

#include "actors.h"

#define					NUM_SHIELD_POINTS		(8)
#define					NUM_SHIELD_SPAGHETTI	(20)
#define					MAX_SPAGHETTI_SPEED		(RAD(360.0f))
#define					MIN_SPAGHETTI_SPEED		(RAD(100.0f))
#define					NUM_RING_POINTS			(20)
#define					SHIELD_RING_WIDTH		(METERTOUNIT(0.05f))


// CMD: flags
#define					REDSHIELD_DRAWACTOR		(1<<0)
#define					REDSHIELD_FLICKER		(1<<1)

enum EShieldState
{
	REDSHIELD_ACTIVE,
	REDSHIELD_INACTIVE,
	REDSHIELD_FADEUP,
	REDSHIELD_FADEDOWN,
};

typedef struct TAG_SHIELDPASTAINFO
{
	TBVector		startPoint;
	TBVector		ctrlPts[3];
	TBQuaternion	ctrlQuats[3];
	TBQuaternion	ctrlTargetQuats[3];
	float			ctrlRadius[3];
	float			ctrlInitRad[3];
	float			ctrlRotSpeed[3];

	TBVector		points[NUM_SHIELD_POINTS];
	float			size[NUM_SHIELD_POINTS];
	int				colour[NUM_SHIELD_POINTS][4];

	float			clock;
} SHIELDPASTAINFO, _SHIELDPASTAINFO BALIGN16;

typedef struct TAG_SHIELDRINGSINFO
{
	float			clock;
	float			rotation;
	float			rotSpeed;

	TBVector		points[NUM_RING_POINTS];
	TBVector		initPoints[NUM_RING_POINTS];
	float			size[NUM_RING_POINTS];
	int				colour[NUM_RING_POINTS][4];

// CMD: random blob info
	float			blobClock;
	int32			blobPoint;
	float			blobSize;
	float			initialBlobSize;
} SHIELDRINGSINFO, _SHIELDRINGSINFO BALIGN16;

typedef struct TAG_REDSHIELDINFO
{
	TBActorInstance		*actorInstance;
	float				alpha;
	int32				handle;
	uint32				state;

	float				clock;
	float				scale;
	float				initialScale;
	TBActorNodeInstance	*node;

	uint32				drawShield;
	int					soundHandle;
	ACTORINSTANCE		*shieldOwner;
	uint32				flags;

	SHIELDPASTAINFO		spaghetti[NUM_SHIELD_SPAGHETTI];
	SHIELDRINGSINFO		shieldRings[3];
} REDSHIELDINFO, _REDSHIELDINFO;

typedef struct TAG_TAZSHIELDINFO
{
	int32			tazShieldHandle;
	TBTexture		*shieldGif;
	uint32			shieldGifFrame;
	int32			health;

	float			initialScale;
	float			currentScale;
	int32			iPad[2];
}TAZSHIELDINFO, _TAZSHIELDINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateRedShieldArroundActor
   Purpose : creates
   Parameters : 
   Returns : 
   Info : 
*/

REDSHIELDINFO *CreateRedShieldArroundActor(ACTORINSTANCE *actorInstance, char *nodeName, float size);

/* --------------------------------------------------------------------------------
   Function : CreateRedShieldArroundActor
   Purpose : creates
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateRedShieldArroundActor(ACTORINSTANCE *actorInstance, REDSHIELDINFO *info);

/* --------------------------------------------------------------------------------
   Function : DrawRedShieldSpaghetti
   Purpose : creates
   Parameters : 
   Returns : 
   Info : 
*/

void DrawRedShieldSpaghetti(ACTORINSTANCE *actorInstance, REDSHIELDINFO *info);

/* --------------------------------------------------------------------------------
   Function : RequestRedShieldStateChange
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void RequestRedShieldStateChange(REDSHIELDINFO *info, EShieldState newState);

/* --------------------------------------------------------------------------------
   Function : CreateRedShieldArroundActor
   Purpose : creates
   Parameters : 
   Returns : 
   Info : 
*/

void DestroyRedShieldArroundActor(REDSHIELDINFO *info);


/* --------------------------------------------------------------------------------
   Function : CreateTazShield
   Purpose : creates
   Parameters : 
   Returns : 
   Info : 
*/

void CreateTazShield(ACTORINSTANCE *attachee, int32 initialHealth=9);

/* --------------------------------------------------------------------------------
   Function : ReattachTazShield
   Purpose : creates
   Parameters : 
   Returns : 
   Info : 
*/

void ReattachTazShield(ACTORINSTANCE *attachee, TAZSHIELDINFO *shieldInfo);

/* --------------------------------------------------------------------------------
   Function : ReInitialiseShieldHealth
   Purpose : puts helmet back to its initial state
   Parameters : 
   Returns :
   Info : 
*/

void ReInitialiseShieldHealth(ACTORINSTANCE *attachee, int32 initialHealth);

/* --------------------------------------------------------------------------------
   Function : ReduceShieldHealthByOne
   Purpose : creates
   Parameters : 
   Returns : 
   Info : 
*/

int32 ReduceShieldHealthByOne(ACTORINSTANCE *attachee);

/* --------------------------------------------------------------------------------
   Function : FreeTazShield
   Purpose : creates
   Parameters : 
   Returns : 
   Info : 
*/

TAZSHIELDINFO *FreeTazShield(TAZSHIELDINFO *tazShieldInfo);

#endif