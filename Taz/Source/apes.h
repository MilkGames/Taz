#ifndef	__H_APES
#define __H_APES

#define	APE_TIME	(1.0f)
#define	APE_BREATH	(1.5f)

#include "linkedlist.h"

typedef class TAG_BANANASKIN
{
private:
	static LINKEDLIST<TAG_BANANASKIN>	list;			// TP: List of all bananas in existance
	struct TAG_ACTORINSTANCE		*owner;				// TP: ape the banana belongs to
	float							life;				// TP: how long banana has been alive for

	/*  --------------------------------------------------------------------------------
		Function	: Update	
		Purpose		: Perform update on banana skin
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	bool Update(void);

public:

	struct TAG_ACTORINSTANCE		*actorInstance;		// TP: banana instance

	/*  --------------------------------------------------------------------------------
		Function	: BANANASKIN::TAG_BANANASKIN
		Purpose		: Create a banana skin with a limited life
		Parameters	: position, orientation, owner of banana, life of banana skin
		Returns		: 
		Info		: 
	*/
	TAG_BANANASKIN(TBVector pos, TBQuaternion orientation, struct TAG_ACTORINSTANCE *owner, float life, char *actorName="banana.obe");

	/*  --------------------------------------------------------------------------------
		Function	: BANANASKIN::~TAG_BANANASKIN
		Purpose		: Destructor
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	~TAG_BANANASKIN();

	/*  --------------------------------------------------------------------------------
		Function	: KillAllBananaSkins
		Purpose		: Remove all banana skins from the level
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	friend void KillAllBananaSkins(void)
	{
//		list.kill();
		TAG_BANANASKIN	*bananaSkin;
		int			i;

		for (i=TAG_BANANASKIN::list.getSize()-1;i>=0;i--)
		{
			bananaSkin = TAG_BANANASKIN::list.get(i);

			SAFE_DELETE(bananaSkin);
		}
	}

	/*  --------------------------------------------------------------------------------
		Function	: UpdateAllBananaSkins
		Purpose		: Perform a standard update on all the banana skins
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	friend void UpdateAllBananaSkins(void);

} BANANASKIN;

/*  --------------------------------------------------------------------------------
	Function	: PirateApeThrowBanana
	Purpose		: Throw a banana skin
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
void PirateApeThrowBanana(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ApeIdleSeq
   Purpose : Respond to requests for idle seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ApeIdleSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ApeAwareSeq
   Purpose : Respond to requests for aware seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ApeAwareSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ApeAttackSeq
   Purpose : Respond to requests for attack seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ApeAttackSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ApeTooCloseSeq
   Purpose : Respond to requests for defend seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ApeTooCloseSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ApeAttackedSeq
   Purpose : Respond to requests for attacked seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ApeAttackedSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ApeOutOfRangeSeq
   Purpose : Respond to requests for out of range seqs
   Parameters : ACTORINSTANCE *
   Returns : seq to follow
   Info : 
*/
int ApeOutOfRangeSeq(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ApeAttackCallback
   Purpose : Called each frame during an attack 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/
int ApeAttackCallback(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ApeStateChangeCallback
   Purpose : Called when state changed 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void ApeStateChangeCallback(ACTORINSTANCE *actorInstance, int state, int lastState);

/*  --------------------------------------------------------------------------------
	Function	: PirateApeAttack
	Purpose		: Pirate ape attack
	Parameters	: actorInstance, type
	Returns		: 
	Info		: 
*/
void PirateApeAttack(ACTORINSTANCE *actorInstance, int type);

/* --------------------------------------------------------------------------------
   Function : ApeTazSquashedCallback
   Purpose : Called when state changed 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int ApeTazHitCallback(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ApeMessageHandler
   Purpose : Called when a message is sent in the sequence
   Parameters : ACTORINSTANCE *, char *message
   Returns : 
   Info : 
*/
int ApeMessageHandler(ACTORINSTANCE *actorInstance, char *message);

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceSecurityBot
   Purpose : Setup the basic ai for a zookeeper
   Parameters : ACTORINSTANCE *, activelist,inactiveList
   Returns : 
   Info : Should be called after CreateActorInstance
*/
int CreateActorInstanceApe(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList);

#endif