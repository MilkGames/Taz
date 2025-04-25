#ifndef __H_SPLATOBJECTS
#define __H_SPLATOBJECTS


/* --------------------------------------------------------------------------------
   Function : CreateSplatObject
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateSplatObject(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : RespondToSplatCollision
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToSplatCollision(struct TAG_ACTORINSTANCE *collider, struct TAG_ACTORINSTANCE *wall);

/* --------------------------------------------------------------------------------
   Function : SplatCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void SplatCallback(struct TAG_ACTORINSTANCE *actorInstance);

#endif