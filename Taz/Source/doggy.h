#ifndef __H_DOGGY
#define __H_DOGGY

enum
{
	DOGGY,
	DEMONDOG,
};


typedef struct TAG_DOGGYINFO
{
	int	type;
} DOGGYINFO;


/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceDoggy
   Purpose : Setup the basic ai for a dog
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceDoggy(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList, int type);

/*  --------------------------------------------------------------------------------
	Function	: DoggyBite
	Purpose		: Called when the doggy does a bite move
	Parameters	: actorInstance of doggy, type of hit
	Returns		: 
	Info		: 
*/
void DoggyBite(ACTORINSTANCE *actorInstance, int type);

/*  --------------------------------------------------------------------------------
	Function	: DoggyBiteChangeCallback
	Purpose		: standard callback thingy
	Parameters	: 
	Returns		: 
	Info		: 
*/
void DoggyBiteChangeCallback(ACTORINSTANCE *actorInstance);

#endif