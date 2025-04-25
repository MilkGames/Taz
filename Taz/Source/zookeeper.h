#ifndef __H_ZOOKEEPER
#define __H_ZOOKEEPER

enum EZookeeperType
{
	ZOOKEEPER_SCOUT,
	ZOOKEEPER_SLEEPER,
	ZOOKEEPER_DEFENDER,
};

typedef struct TAG_ZOOKEEPERINFO
{
	float			timeWithTazTrapped;
	int				count;
	EZookeeperType	type;					// TP: zookeeper type ZOOKEEPER_..,
	uint32			defend;
	int				state;
	float			dragFactor;
} ZOOKEEPERINFO,_ZOOKEEPERINFO BALIGN16;


/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceZookeeper
   Purpose : Setup the basic ai for a zookeeper
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceZookeeper(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList, EZookeeperType type = ZOOKEEPER_SCOUT, uint32 defend=0);

/* --------------------------------------------------------------------------------
   Function : ZookeeperAttackCallback
   Purpose : Called each frame during an attack 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
int ZookeeperAttackCallback(struct TAG_ACTORINSTANCE *actorInstance);

/*  --------------------------------------------------------------------------------
	Function	: ProcessCharacterInNet
	Purpose		: Control for the Taz after he has been netted
	Parameters	: Taz
	Returns		: 
	Info		: 
*/

void ProcessCharacterInNet(ACTORINSTANCE *actorInstance);

#endif