#ifndef __H_PROJECTILE
#define __H_PROJECTILE

#include "trigger.h"


// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// PP: effects triggered by the SPECIALTYPE_PROJECTILE objects (cannons and what-have-you)
enum
{
	CANNONEFFECT_NONE			=-1,				// PP: no effect: the default
													
	CANNONEFFECT_CANNON,							// PP: explosion and smoke, for proper cannons
	CANNONEFFECT_CEMENTMIXER,						// PP: puff of cement-type dust, and little chunks of cement maybe?
													
	NUM_CANNON_EFFECTS								// PP: number of cannon effects
};

// PP: by default, SPECIALTYPE_PROJECTILE objects trigger no effect when they fire
#define DEFAULT_CANNON_EFFECT						CANNONEFFECT_NONE

// PP: names used to specify cannon effects (in Max)
extern const char* const							cannonEffectNames[NUM_CANNON_EFFECTS];

// PP: type for cannon effect functions
typedef void(*CannonEffectFunction)(ACTORINSTANCE* const actorInstance);

// PP: array of cannon effect functions
extern const CannonEffectFunction					cannonEffectFunctions[NUM_CANNON_EFFECTS];

// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#define CANNONFLAG_INACTIVE		(1<<0)
#define CANNONFLAG_NOSNAPTOORIGIN (1<<1)
#define CANNONFLAG_NOCENTERORIGIN (1<<2)
#define CANNONFLAG_FIXEDPATH	(1<<3)

typedef struct TAG_PROJECTILEINFO
{
	uint32				type;
	uint32				landPointCRC;
	float				angle;
	uint32				collision;

	ACTORINSTANCE		*collider;
	uint32				fireFlag;
	TRIGGERINFO			*onContact;
	TRIGGERINFO			*onAim;

	TRIGGERINFO			*onFire;
	TRIGGERINFO			*onRecoil;
	uint32				projectOnContact;
	int					soundHandle;

	uint32				flags;
	int32				cannonEffect;				// PP: index of effect triggered as the "cannon" fires (see CANNONEFFECT_... enum)				
} PROJECTILEINFO BALIGN16;

extern TBVector			initialPos;

/* --------------------------------------------------------------------------------
   Function : CreateActorInstancePathInfo
   Purpose : Creates and initialises the path info part of an actorInstance
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
int CreateActorInstanceProjectile(ACTORINSTANCE *actorInstance, uint32 landPointCRC, float angle, ESpecialType type, int projectOnContact,bool collision=true,char liveUpdate=FALSE, const int cannonEffect=DEFAULT_CANNON_EFFECT,int flags = 0);

/* --------------------------------------------------------------------------------
   Function : RespondToCannonCollision
   Purpose : Function to be called when a collision with a cannon has occured
   Parameters : ACTORINSTANCE object hitting cannon, ACTORINSTANCE *cannon
   Returns : 
   Info : 
*/
int RespondToCannonCollision(ACTORINSTANCE *collider, ACTORINSTANCE *cannon);

/* --------------------------------------------------------------------------------
   Function : UpdateCannonState
   Purpose : Updates cannons on an instance list
   Parameters : ACTORINSTANCELIST *
   Returns : 
   Info :
*/

void UpdateCannonState(ACTORINSTANCE *ptr);

/* --------------------------------------------------------------------------------
   Function : LaunchActor
   Purpose : entry point for proj motion calculations. launches actor from actor at position A, 
				to another actor at position B.
   Parameters : Actor Instance x3;
   Returns : 
   Info :
*/

void LaunchActor(ACTORINSTANCE *projectile, ACTORINSTANCE *cannon);

/* --------------------------------------------------------------------------------
   Function : CalculateProjectileInitialVelocity
   Purpose : calculates the initial velocity required given two points, and an angle
   Parameters :
   Returns : 
   Info :
*/
float CalculateProjectileInitialVelocity(TBVector initialVelocity, TBVector currentPos, TBVector landingPos, float theta);
//void CalculateProjectileInitialVelocity(TBVector initialVelocity, TBVector currentPos, TBVector landingPos, float theta);

/* --------------------------------------------------------------------------------
   Function : UpdateProjectilePitch
   Purpose : rotatates the projectile to point in direction of arc
   Parameters : Projectile Actor Instance;
   Returns : 
   Info :
*/

void UpdateProjectilePitch( ACTORINSTANCE *projectile );

/* --------------------------------------------------------------------------------
   Function : UpdateProjectilePitch
   Purpose : rotatates the projectile to point feet first in direction of arc
   Parameters : Projectile Actor Instance;
   Returns : 
   Info :
*/
void UpdateProjectilePitch(ACTORINSTANCE *projectile,float xAngle);

void	LaunchStateChangeCallback(struct TAG_ACTORINSTANCE *actorInstance);

/*  --------------------------------------------------------------------------------
	Function	: FollowFixedPath
	Purpose		: Get a projectile to follow the curve
	Parameters	: ACTORINSTANCE for projectile
	Returns		: 
	Info		: 
*/

void FollowFixedPath(ACTORINSTANCE *actorInstance);

#endif