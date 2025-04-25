#ifndef __H_ESCALTOR
#define __H_ESCALTOR

typedef struct TAG_ESCALATORINFO
{
	TBVector		direction;
	bool			active;
} ESCALATORINFO, _ESCALATORINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateEscalator
   Purpose : Creates escalator object
   Parameters : oreintation of object relative to world
   Returns : true/false
   Info : 
*/

int CreateEscalator(ACTORINSTANCE *actorInstance, TBQuaternion orientation, float speed);

/* --------------------------------------------------------------------------------
   Function : CreateEscalatorInfo
   Purpose : creates escalator info structure
   Parameters : 
   Returns : pointer to structure
   Info : 
*/

ESCALATORINFO *CreateEscalatorInfo(void);

/* --------------------------------------------------------------------------------
   Function : RespondToEscalatorCollision
   Purpose : guess
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToEscalatorCollision(ACTORINSTANCE *collider, ACTORINSTANCE *escalator);

/* --------------------------------------------------------------------------------
   Function : UpdateLastEscalator
   Purpose : updates taz as aresult of the last escalator collided with
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateLastEscalator(ACTORINSTANCE *actorInstance);

#endif