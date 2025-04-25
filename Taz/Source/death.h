#ifndef __H_DEATH
#define __H_DEATH

#include "actors.h"

typedef struct TAG_DEATHOBJECTINFO
{
	struct TAG_WATERRESETINFO	*resetInfo;
} DEATHOBJECTINFO, _DEATHOBJECTINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateDeathObject
   Purpose : Creates an object which will kill taz immediately on contact
   Parameters : 
   Returns : 
   Info : 
*/

int CreateDeathObject(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : RespondToDeathCollision
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToDeathCollision(struct TAG_ACTORINSTANCE *collider, struct TAG_ACTORINSTANCE *deathObject);

/* --------------------------------------------------------------------------------
   Function : CalculateDeathObjectResetPoint
   Purpose : calculates reset point
   Parameters : ACTORINSTANCE *drowner, ACTORINSTANCE *water
   Returns : 
   Info : 
*/

void CalculateDeathObjectResetPoint(struct TAG_ACTORINSTANCE *collider, struct TAG_ACTORINSTANCE *deathObject);

/* --------------------------------------------------------------------------------
   Function : DeathStateChangeCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void DeathStateChangeCallback(class TAG_ACTORINSTANCE *actorInstance);


/* --------------------------------------------------------------------------------
   Function : FinishedFadeFromWater
   Purpose : 
   Parameters : FADE *
   Returns : 
   Info : 
*/

void FinishedFadeFromDeathPlane(class TAG_FADE *fade);

/* --------------------------------------------------------------------------------
   Function : ResetActorAtSideOfDeathPlane
   Purpose : resets actorInstance at designated point, nearest to water entry point
   Parameters : ACTORINSTANCE *drowner, ACTORINSTANCE *water
   Returns : 
   Info : 
*/

void ResetActorAtSideOfDeathPlane(ACTORINSTANCE *drowner, TBVector resetPoint, TBQuaternion resetQuat);

#endif