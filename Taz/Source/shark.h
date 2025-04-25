#ifndef __H_SHARK
#define __H_SHARK

typedef struct TAG_SHARKINFO
{
	TBVector	patrolPoint[2];		// TP: the two control points defined by the box
	TBVector	start,end,p1,p2;
	TBVector	resetPoint;			// TP: if taz had been eaten by the shark then this should contain the nearest reset point
	bool		gotResetPoint;		// TP: did we find a reset point?
	int			state;
	float		jumpT;
	bool		gotTaz;
	bool 		costumeOk;			// TP: would it be ok to switch to the costume idle
	ACTORINSTANCE	*myWater;		// TP: pointer to the water the shark is swimming in
} SHARKINFO;

/* --------------------------------------------------------------------------------
   Function : SharkIdleSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int SharkIdleSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : SharkAwareSeq
   Purpose : Respond to requests for aware seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int SharkAwareSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : SharkAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int SharkAttackSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : SharkTooCloseSeq
   Purpose : Respond to requests for defend seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int SharkTooCloseSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : SharkAttackedSeq
   Purpose : Respond to requests for attacked seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int SharkAttackedSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : SharkOutOfRangeSeq
   Purpose : Respond to requests for out of range seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int SharkOutOfRangeSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : SharkAttackCallback
   Purpose : Called each frame during an attack 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
int SharkAttackCallback(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : SharkStateChangeCallback
   Purpose : Called when state changed 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void SharkStateChangeCallback(ACTORINSTANCE *actorInstance, int state, int lastState);

/* --------------------------------------------------------------------------------
   Function : SharkTazSquashedCallback
   Purpose : Called when state changed 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int SharkTazSquashedCallback(ACTORINSTANCE *actorInstance);


/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceShark
   Purpose : Setup the basic ai for a zookeeper
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/

int CreateActorInstanceShark(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList);


/*  --------------------------------------------------------------------------------
	Function	: FindPatrolPoints
	Purpose		: Find two patrol points inside the box
	Parameters	: ACTORINSTANCE of shark
	Returns		: 
	Info		: 
*/

void FindPatrolPoints(ACTORINSTANCE *actorInstance);


#endif