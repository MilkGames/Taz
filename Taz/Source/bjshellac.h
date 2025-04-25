#ifndef __H_BJSHELLAC
#define __H_BJSHELLAC

typedef struct TAG_BJSHELLACINFO
{
	int	type;
} BJSHELLACINFO;

/* --------------------------------------------------------------------------------
   Function : BJShellacIdleSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int BJShellacIdleSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : BJShellacAwareSeq
   Purpose : Respond to requests for aware seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int BJShellacAwareSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : BJShellacAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int BJShellacAttackSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : BJShellacAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int BJShellacPursuitAttackSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : BJShellacTooCloseSeq
   Purpose : Respond to requests for defend seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int BJShellacTooCloseSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : BJShellacAttackedSeq
   Purpose : Respond to requests for attacked seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int BJShellacAttackedSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : BJShellacOutOfRangeSeq
   Purpose : Respond to requests for out of range seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int BJShellacOutOfRangeSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : BJShellacAttackCallback
   Purpose : Called each frame during an attack 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
int BJShellacAttackCallback(struct TAG_ACTORINSTANCE *actorInstance);

/*  --------------------------------------------------------------------------------
	Function	: ChangeState
	Purpose		: Called when the character changes state
	Parameters	: actorInstance, new state, last state
	Returns		: 
	Info		: 
*/
static void myChangeState(struct TAG_ACTORINSTANCE *actorInstance,int state,int lastState);

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceBJShellac
   Purpose : Setup the basic ai for a BJShellac
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceBJShellac(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList,int type);

/* --------------------------------------------------------------------------------
   Function : BJShellacFire
   Purpose : Called when the anvil guy sends an fireanvil command
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
void BJShellacFire(ACTORINSTANCE *actorInstance, int type);

/*  --------------------------------------------------------------------------------
	Function	: CreateDynamite
	Purpose		: Creates a debris actor for dynamite and sets it ready to go
	Parameters	: actorInstance,initial position, initial velocity, time-life of debris
	Returns		: 
	Info		: 
*/
void CreateDynamite(ACTORINSTANCE *actorInstance, TBVector pos, TBVector velocity, float time);

#endif