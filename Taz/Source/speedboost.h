#ifndef __H_SPEEDBOOST
#define __H_SPEEDBOOST

#include "trigger.h"

typedef struct TAG_SPEEDBOOSTINFO
{
	TBVector		direction;
	TRIGGERINFO		*onBoost;
	float			speedboost;
	uint32			targetCRC;
	bool			active;
} SPEEDBOOSTINFO, _SPEEDBOOSTINFO BALIGN16;
/* --------------------------------------------------------------------------------
   Function : CreateSpeedBoost
   Purpose : creates an object which gives an actor a boost in speed if collided with
   Parameters : 
   Returns : 
   Info : 
*/

int CreateSpeedBoost(ACTORINSTANCE *speedboostInstance, TBVector position, TBQuaternion direction, float speed, uint32 target, bool active = true, bool collision = true);

/* --------------------------------------------------------------------------------
   Function : CreateSpeedBoostInfo
   Purpose : does the same as all the other functions of this type
   Parameters : 
   Returns : 
   Info : 
*/

SPEEDBOOSTINFO *CreateSpeedBoostInfo(void);

/* --------------------------------------------------------------------------------
   Function : RespondToSpeedboostCollision
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToSpeedboostCollision(ACTORINSTANCE *collider, ACTORINSTANCE *speedboost);

#endif