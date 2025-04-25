#ifndef __H_CRATEATTACK
#define __H_CRATEATTACK

/* --------------------------------------------------------------------------------
   Function : RespondToMTweetyCrateCollision
   Purpose : called when mecha tweety hits taz with a crate
   Parameters : 
   Returns : 
   Info : 
*/

int RespondToMTweetyCrateCollision(ACTORINSTANCE *collider, ACTORINSTANCE *crate);

/* --------------------------------------------------------------------------------
   Function : PreRespondToMTweetyCrateCollision
   Purpose : called when mecha tweety hits taz with a crate but before response has been processed
   Parameters : 
   Returns : 
   Info : 
*/
int PreRespondToMTweetyCrateCollision(ACTORINSTANCE *collider, ACTORINSTANCE *crate, PRECOLLINFO *info);

#endif