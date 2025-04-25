#ifndef __H_MINERBOT
#define __H_MINERBOT

/* --------------------------------------------------------------------------------
   Function : MinerBotIdleSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int MinerBotIdleSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : MinerBotAwareSeq
   Purpose : Respond to requests for aware seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int MinerBotAwareSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : MinerBotAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int MinerBotAttackSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : MinerBotDefendSeq
   Purpose : Respond to requests for defend seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int MinerBotDefendSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : MinerBotAttackedSeq
   Purpose : Respond to requests for attacked seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int MinerBotAttackedSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : MinerBotOutOfRangeSeq
   Purpose : Respond to requests for out of range seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int MinerBotOutOfRangeSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : MinerBotGuardPuzzledSeq
   Purpose : Respond to requests for puzzled seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/

int MinerBotPuzzledSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : MinerBotAttackCallback
   Purpose : Called each frame during an attack 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
int MinerBotAttackCallback(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : MinerBotStateChangeCallback
   Purpose : Called when state changed 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void MinerBotStateChangeCallback(ACTORINSTANCE *actorInstance, int state, int lastState);

/*  --------------------------------------------------------------------------------
	Function	: MinerBotAttack
	Purpose		: Called when the security bot does an attack move
	Parameters	: actorInstance, type of attack
	Returns		: 
	Info		: 
*/
void MinerBotAttack(ACTORINSTANCE *actorInstance, int type);

/* --------------------------------------------------------------------------------
   Function : MinerBotTazSquashedCallback
   Purpose : Called when state changed 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int MinerBotTazSquashedCallback(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : MinerBotMessageHandler
   Purpose : Called when a message is sent in the sequence
   Parameters : ACTORINSTANCE *, char *message
   Returns : 
   Info : 
*/
int MinerBotMessageHandler(ACTORINSTANCE *actorInstance, char *message);

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceMinerBot
   Purpose : Setup the basic ai for a zookeeper
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceMinerBot(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList);

/*  --------------------------------------------------------------------------------
	Function	: MinerBotAttack
	Purpose		: Called when the security bot does an attack move
	Parameters	: actorInstance, type of attack
	Returns		: 
	Info		: 
*/
void MinerBotAttack(ACTORINSTANCE *actorInstance, int type);

#endif