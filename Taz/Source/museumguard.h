#ifndef __H_MUSEUMGUARD
#define __H_MUSEUMGUARD


/* --------------------------------------------------------------------------------
   Function : MuseumGuardIdleSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int MuseumGuardIdleSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : MuseumGuardAwareSeq
   Purpose : Respond to requests for aware seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int MuseumGuardAwareSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : MuseumGuardAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int MuseumGuardAttackSeq(struct TAG_ACTORINSTANCE *actorInstance);

/*  --------------------------------------------------------------------------------
	Function	: MuseumGuardAttack
	Purpose		: Called when the museum guard does an attack move
	Parameters	: actorInstance, type
	Returns		: 
	Info		: 
*/
void MuseumGuardAttack(struct TAG_ACTORINSTANCE *actorInstance,int type);

/* --------------------------------------------------------------------------------
   Function : MuseumGuardTooCloseSeq
   Purpose : Respond to requests for defend seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int MuseumGuardTooCloseSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : MuseumGuardPuzzledSeq
   Purpose : Respond to requests for puzzled seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/

int MuseumGuardPuzzledSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : MuseumGuardAttackedSeq
   Purpose : Respond to requests for attacked seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int MuseumGuardAttackedSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : MuseumGuardOutOfRangeSeq
   Purpose : Respond to requests for out of range seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int MuseumGuardOutOfRangeSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : MuseumGuardAttackCallback
   Purpose : Called each frame during an attack 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
int MuseumGuardAttackCallback(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : MuseumGuardStateChangeCallback
   Purpose : Called when state changed 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void MuseumGuardStateChangeCallback(ACTORINSTANCE *actorInstance, int state, int lastState);

/* --------------------------------------------------------------------------------
   Function : MuseumGuardTazSquashedCallback
   Purpose : Called when state changed 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int MuseumGuardTazSquashedCallback(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : MuseumGuardMessageHandler
   Purpose : Called when a message is sent in the sequence
   Parameters : ACTORINSTANCE *, char *message
   Returns : 
   Info : 
*/
int MuseumGuardMessageHandler(ACTORINSTANCE *actorInstance, char *message);

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceMuseumGuard
   Purpose : Setup the basic ai for a zookeeper
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceMuseumGuard(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList);

#endif