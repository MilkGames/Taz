#ifndef __H_ANVILGANG
#define __H_ANVILGANG

/* --------------------------------------------------------------------------------
   Function : AnvilGangIdleSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int AnvilGangIdleSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : AnvilGangAwareSeq
   Purpose : Respond to requests for aware seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int AnvilGangAwareSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : AnvilGangAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int AnvilGangAttackSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : AnvilGangAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int AnvilGangPursuitAttackSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : AnvilGangTooCloseSeq
   Purpose : Respond to requests for defend seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int AnvilGangTooCloseSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : AnvilGangAttackedSeq
   Purpose : Respond to requests for attacked seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int AnvilGangAttackedSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : AnvilGangOutOfRangeSeq
   Purpose : Respond to requests for out of range seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int AnvilGangOutOfRangeSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : AnvilGangAttackCallback
   Purpose : Called each frame during an attack 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
int AnvilGangAttackCallback(struct TAG_ACTORINSTANCE *actorInstance);

/*  --------------------------------------------------------------------------------
	Function	: ChangeState
	Purpose		: Called when the character changes state
	Parameters	: actorInstance, new state, last state
	Returns		: 
	Info		: 
*/
static void myChangeState(struct TAG_ACTORINSTANCE *actorInstance,int state,int lastState);

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceAnvilGang
   Purpose : Setup the basic ai for a AnvilGang
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceAnvilGang(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList);

/* --------------------------------------------------------------------------------
   Function : AnvilGangFire
   Purpose : Called when the anvil guy sends an fireanvil command
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
void AnvilGangFire(ACTORINSTANCE *actorInstance);

#endif
