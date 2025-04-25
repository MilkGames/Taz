#ifndef __H_ICE
#define __H_ICE



/*	--------------------------------------------------------------------------------
	Function 	: CreateIceInstance
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void CreateIceInstance(ACTORINSTANCE *iceInstance,char *animation,char liveUpdate);

/* --------------------------------------------------------------------------------
   Function : RespondToIceCollision
   Purpose : calculates response when an actor collides
   Parameters : ACTORINSTANCE *spring, ACTORINSTANCE *collider
   Returns : 
   Info : 
*/
int RespondToIceCollision(ACTORINSTANCE *collider, ACTORINSTANCE *ice);

void IceStateChangeCallback(ACTORINSTANCE *actorInstance);

#endif