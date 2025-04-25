#ifndef __H_NASTYCAN
#define __H_NASTYCAN

#include "linkedlist.h"

/* --------------------------------------------------------------------------------
   Function : NastyCanIdleSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int NastyCanIdleSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : NastyCanAwareSeq
   Purpose : Respond to requests for aware seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int NastyCanAwareSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : NastyCanAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int NastyCanAttackSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : NastyCanAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int NastyCanPursuitAttackSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : NastyCanTooCloseSeq
   Purpose : Respond to requests for defend seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int NastyCanTooCloseSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : NastyCanAttackedSeq
   Purpose : Respond to requests for attacked seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int NastyCanAttackedSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : NastyCanOutOfRangeSeq
   Purpose : Respond to requests for out of range seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int NastyCanOutOfRangeSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : NastyCanAttackCallback
   Purpose : Called each frame during an attack 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
int NastyCanAttackCallback(struct TAG_ACTORINSTANCE *actorInstance);

/*  --------------------------------------------------------------------------------
	Function	: ChangeState
	Purpose		: Called when the character changes state
	Parameters	: actorInstance, new state, last state
	Returns		: 
	Info		: 
*/
static void myChangeState(struct TAG_ACTORINSTANCE *actorInstance,int state,int lastState);

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceNastyCan
   Purpose : Setup the basic ai for a NastyCan
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceNastyCan(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList);

/*  --------------------------------------------------------------------------------
	Function	: NastyCanHit
	Purpose		: Called when Nasty can has done a gun thump move
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
void NastyCanHit(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : NastyCanFire
   Purpose : Called when the anvil guy sends an fireanvil command
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
void NastyCanFire(ACTORINSTANCE *actorInstance);

typedef class TAG_CACTUS
{
	static			LINKEDLIST<TAG_CACTUS>	list;
	float			life;
	ACTORINSTANCE	*actorInstance;

	void Init(float life, TBVector position, TBVector scale, TBQuaternion orientation);

	void Update();

public:

	TAG_CACTUS(float life, TBVector position, TBVector scale, TBQuaternion orientation)
	{
		Init(life, position, scale, orientation);
	}

	~TAG_CACTUS()
	{
		list.remove(this);
	}

	friend void UpdateAllCactie(void);

	friend void FreeAllCactie(void)
	{
		int			i;
		TAG_CACTUS	*cactus;

		for (i=TAG_CACTUS::list.getSize()-1;i>=0;i--)
		{
			cactus = TAG_CACTUS::list.get(i);

			delete cactus;
		}
	}

	/*  --------------------------------------------------------------------------------
		Function	: IsActorInstanceACactus
		Purpose		: Returns true if the actor is a cactus
		Parameters	: actorInstance to test
		Returns		: 
		Info		: 
	*/
	friend bool	IsActorInstanceACactus(ACTORINSTANCE *actorInstance);

} CACTUS;


#endif