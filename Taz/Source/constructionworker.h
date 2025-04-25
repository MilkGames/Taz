#ifndef __H_CONSTRUCTIONWORKER
#define __H_CONSTRUCTIONWORKER

/* --------------------------------------------------------------------------------
   Function : ConstructionWorkerIdleSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ConstructionWorkerIdleSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ConstructionWorkerAwareSeq
   Purpose : Respond to requests for aware seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ConstructionWorkerAwareSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ConstructionWorkerAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ConstructionWorkerAttackSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ConstructionWorkerTooCloseSeq
   Purpose : Respond to requests for defend seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ConstructionWorkerTooCloseSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ConstructionWorkerAttackedSeq
   Purpose : Respond to requests for attacked seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ConstructionWorkerAttackedSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ConstructionWorkerOutOfRangeSeq
   Purpose : Respond to requests for out of range seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/

int ConstructionWorkerOutOfRangeSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ConstructionWorkerAttackCallback
   Purpose : Called each frame during an attack 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
int ConstructionWorkerAttackCallback(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ConstructionWorkerStateChangeCallback
   Purpose : Called when state changed 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void ConstructionWorkerStateChangeCallback(ACTORINSTANCE *actorInstance, int state, int lastState);

/* --------------------------------------------------------------------------------
   Function : ConstructionWorkerTazSquashedCallback
   Purpose : Called when state changed 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int ConstructionWorkerTazSquashedCallback(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ConstructionWorkerMessageHandler
   Purpose : Called when a message is sent in the sequence
   Parameters : ACTORINSTANCE *, char *message
   Returns : 
   Info : 
*/
int ConstructionWorkerMessageHandler(ACTORINSTANCE *actorInstance, char *message);

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceConstructionWorker
   Purpose : Setup the basic ai for a zookeeper
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceConstructionWorker(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList);

/*  --------------------------------------------------------------------------------
	Function	: ConstructionWorkerEvent
	Purpose		: Called with any event messages sent by the animation
	Parameters	: actorInstance, type
	Returns		: 
	Info		: 
*/
void ConstructionWorkerEvent(ACTORINSTANCE *actorInstance, int type);

#endif