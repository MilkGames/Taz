#ifndef __H_BEARS
#define __H_BEARS

enum 
{
	POLARBEAR,
	BROWNBEAR,
};

typedef struct TAG_BEARINFO
{
	int	type;
} BEARINFO;

/* --------------------------------------------------------------------------------
   Function : SnowballCollision
   Purpose : Called when snowball collides
   Parameters : ACTORINSTANCE *, 
   Returns : 
   Info : 
*/
int SnowballCollision(ACTORINSTANCE *collider,ACTORINSTANCE *objectHit);

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceBear
   Purpose : Setup the basic ai for a bear
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceBear(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList,int type);

/*  --------------------------------------------------------------------------------
	Function	: PolarBearSlap
	Purpose		: Called when the bear does a slap move
	Parameters	: actorInstance of bear.
	Returns		: 
	Info		: 
*/
void PolarBearSlap(ACTORINSTANCE *actorInstance);

/*  --------------------------------------------------------------------------------
	Function	: BrownBearHit
	Purpose		: Called when the bear does a hit move
	Parameters	: actorInstance of bear, type of hit
	Returns		: 
	Info		: 
*/
void BrownBearHit(ACTORINSTANCE *actorInstance, int type);


#endif