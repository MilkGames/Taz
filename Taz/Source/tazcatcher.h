#ifndef __H_TAZCATCHER
#define __H_TAZCATCHER

typedef struct TAG_TAZCATCHERINFO
{
	float		timeSinceLastAttack;
	float		pad[3];
} TAZCATCHERINFO, _TAZCATCHERINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : TazCatcherIdleSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int TazCatcherIdleSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : TazCatcherAwareSeq
   Purpose : Respond to requests for aware seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int TazCatcherAwareSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : TazCatcherAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int TazCatcherAttackSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : TazCatcherDefendSeq
   Purpose : Respond to requests for defend seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int TazCatcherDefendSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : TazCatcherAttackedSeq
   Purpose : Respond to requests for attacked seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int TazCatcherAttackedSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : TazCatcherOutOfRangeSeq
   Purpose : Respond to requests for out of range seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int TazCatcherOutOfRangeSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : TazCatcherAttackCallback
   Purpose : Called each frame during an attack 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
int TazCatcherAttackCallback(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : TazCatcherStateChangeCallback
   Purpose : Called when state changed 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void TazCatcherStateChangeCallback(struct TAG_ACTORINSTANCE *actorInstance, int state, int lastState);

/* --------------------------------------------------------------------------------
   Function : TazCatcherStateChangeCallback
   Purpose : Called when state changed 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int TazCatcherTazSquashedCallback(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceTazCatcher
   Purpose : Setup the basic ai for a zookeeper
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceTazCatcher(struct TAG_ACTORINSTANCE *actorInstance, struct TAG_ACTORINSTANCELIST *activeList,struct TAG_ACTORINSTANCELIST *inactiveList);

#endif