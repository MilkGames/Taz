#ifndef __H_DESTRUCT
#define __H_DESTRUCT

#define YELLOW_EFFECT	(0)
#define RED_EFFECT		(1)
#define GREEN_EFFECT	(2)
#define NORMAL_EFFECT	(3)
#define BLUE_EFFECT		(4)

extern char lastDestroyer;

void doDestructionGamePickup(ACTORINSTANCE *actorInstance, int type);

void SetPlayerOutlineColor(struct TAG_ACTORINSTANCE *actorInstance, TBActRenderMode_Outlined *Outline, int color);

int destructHandler(int);

/*  --------------------------------------------------------------------------------
	Function	: destructForceEnd
	Purpose		: Force the end of a destruction game
	Parameters	: win- true/false
	Returns		: 
	Info		: 
*/

void destructForceEnd(bool win);

/*  --------------------------------------------------------------------------------
	Function	: DestructibleGameDestroy
	Purpose		: Handle the destructions occuring in a destruction game
	Parameters	: player that destroyed, object destroyed
	Returns		: 
	Info		: 
*/

void DestructibleGameDestroy(ACTORINSTANCE *collider, ACTORINSTANCE *destructible);

#endif