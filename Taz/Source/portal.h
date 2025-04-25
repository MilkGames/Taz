#ifndef __H_PORTAL
#define __H_PORTAL


/* --------------------------------------------------------------------------------
   Function : CreatePortalInstance
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreatePortalWorldInstance(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CreatePortalInstance
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToPortalCollision(struct TAG_ACTORINSTANCE *collider, struct TAG_ACTORINSTANCE *portal);

/* --------------------------------------------------------------------------------
   Function : CreatePortalInstance
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void CalculatePortalWorldGravity(struct TAG_ACTORINSTANCE *actorInstance);

#endif