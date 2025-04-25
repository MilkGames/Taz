#ifndef __H_HICCUP
#define __H_HICCUP

#define	HICCUPBUBBLE_LIFE	(3.0f)
#define POWERUPTIME_HICCUP	(20.0f)


/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/
void UpdateActorHiccup(struct TAG_ACTORINSTANCE *actorInstance);

/*  --------------------------------------------------------------------------------
	Function	: FinishHiccup
	Purpose		: End the hiccup powerup
	Parameters	: actorinstance
	Returns		: 
	Info		: 
*/
void FinishHiccup(struct TAG_ACTORINSTANCE *actorInstance);

#endif