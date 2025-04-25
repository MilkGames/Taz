#ifndef __H_ROCKETSCIENCE
#define __H_ROCKETSCIENCE

#include "actors.h"

#define	ROCKETPROPS_SPEED		(METERTOUNIT(13.0f))
#define	ROCKETPROPS_LIFE		(4.0f)

typedef struct TAG_ROCKETINFO
{
	TBVector		velocity;			// CMD: velocity, which is speed in the direction of the orientation

	float			speed;
	float			clock;
	ACTORINSTANCE	*target;
	int				soundHandle;
} ROCKETINFO, _ROCKETINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : FireSeekingRocketFromActorNode
   Purpose : fires a rocket from a node on an actor, which then seeks another actor
   Parameters : offset represents the offset from the node to fire from
   Returns : 
   Info : 
*/

ACTORINSTANCE *FireSeekingRocketFromActorNode(ACTORINSTANCE *instance, ACTORINSTANCE *target, char *nodeName, TBVector offset);

/* --------------------------------------------------------------------------------
   Function : UpdateSeekingRockets
   Purpose : Updates rocket that seek their target
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateSeekingRockets(ACTORINSTANCE *rocket);

/* --------------------------------------------------------------------------------
   Function : DestroySeekingRocketWithLoudBang
   Purpose : destroys and frees the rocket when an explosion is required
   Parameters : 
   Returns : 
   Info : 
*/

void DestroySeekingRocketWithLoudBang(ACTORINSTANCE *rocket);

/* --------------------------------------------------------------------------------
   Function : DestroySeekingRocketQuietly
   Purpose : destroys, and frees the rocket instance when an explosion is not required
   Parameters : 
   Returns : 
   Info : 
*/

void DestroySeekingRocketQuietly(ACTORINSTANCE *rocket);

/* --------------------------------------------------------------------------------
   Function : EatRocket
   Purpose : for when taz eats a rocket
   Parameters : 
   Returns : 
   Info : 
*/

void EatRocket(ACTORINSTANCE *rocket);

#endif