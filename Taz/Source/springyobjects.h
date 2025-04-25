#ifndef __H_SPRINGYOBJECTS
#define __H_SPRINGYOBJECTS

#include <math.h>
#include "trigger.h"

typedef struct TAG_SPRINGYOBJECTINFO
{
	int				type;					//type of spring
	float			springRadius;			//radius of springyness
	float			maxHeight;				//maximum height of spring
	TRIGGERINFO		*onContact;

	int				alwaysBounce;
	bool			active;
} SPRINGYOBJECTINFO, _SPRINGYOBJECTINFO BALIGN16;

extern float radius;

/* --------------------------------------------------------------------------------
   Function : CreateSpringyObject
   Purpose : Creates and initialises actorInstances that act as springs
   Parameters : char *name, TBVector init position, float max height
   Returns : 
   Info : 
*/
int CreateSpringyObject(ACTORINSTANCE *springyObjectInstance, TBVector initPosition, float maxHeight, ESpecialType type, float radius, int alwaysBounce,int collFlags,bool state,bool collision=true);

/* --------------------------------------------------------------------------------
   Function : CreateSpringyActorInstanceInfo
   Purpose : Creates and initialises springy info part of object
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int CreateSpringyActorInstanceInfo( ACTORINSTANCE *actorInstance );

/* --------------------------------------------------------------------------------
   Function : RespondToActorSpringCollision
   Purpose : calculates response when an actor collides
   Parameters : ACTORINSTANCE *spring, ACTORINSTANCE *collider
   Returns : 
   Info : 
*/

int RespondToSpringCollision( ACTORINSTANCE *spring, ACTORINSTANCE *collider );

/* --------------------------------------------------------------------------------
   Function : CalculateInitialVelocity
   Purpose : calculates initial velocity
   Parameters : ACTORINSTANCE *spring, ACTORINSTANCE *collider
   Returns : 
   Info : 
*/

float CalculateInitialVelocity( ACTORINSTANCE *spring, ACTORINSTANCE *collider );

#endif