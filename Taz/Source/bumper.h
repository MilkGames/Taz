#ifndef __H_BUMPER
#define __H_BUMPER

#include "actors.h"

typedef struct TAG_BUMPERINFO
{
	float		speedBoost;
	int			iPad[2];

	int			isUp;
	int			justKoiked;
	int			refereeNear;

	TBActorAnimSegment *ppAnimSeg[4];

}BUMPERINFO, _BUMPERINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateSpeedBoost
   Purpose : creates an object which gives an actor a boost in speed if collided with
   Parameters : 
   Returns : 
   Info : 
*/

int CreateBumper(ACTORINSTANCE *actorInstance, float speedBoost);

/* --------------------------------------------------------------------------------
   Function : CreateBumperInfo
   Purpose : does the same as all the other functions of this type
   Parameters : 
   Returns : 
   Info : 
*/

BUMPERINFO *CreateBumperInfo(void);

/* --------------------------------------------------------------------------------
   Function : RespondToBumperCollision
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToBumperCollision(ACTORINSTANCE *collider, ACTORINSTANCE *bumper);


void setBumperIgnoreCharacter(ACTORINSTANCE *p);

#endif