#ifndef __H_MAILBOX
#define __H_MAILBOX

enum
{
	MAILBOX_FLAGUP,
	MAILBOX_FLAGDOWN,
	MAILBOX_EXCITED,
	MAILBOX_VERYEXCITED,
};

typedef struct TAG_MAILBOXINFO
{
	uint32			currentState;
	int32			mailboxNumber;
	uint32			triggeredPlayer1;
	uint32			triggeredPlayer2;

	struct TAG_TRIGGERINFO		*onContact;
	struct TAG_TRIGGERINFO		*onExcited;
	struct TAG_TRIGGERINFO		*onVeryExcited;
	uint32			pad;
} MAILBOXINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateMailboxInstance
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateMailboxInstance(ACTORINSTANCE *mailbox, TBVector position, int32 mailboxNumber, uint32 state);

/* --------------------------------------------------------------------------------
   Function : CreateSpringyActorInstanceInfo
   Purpose : Creates and initialises springy info part of object
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int CreateMailboxInstanceInfo(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : UpdateMailboxes
   Purpose : updates mailboxes
   Parameters : ACTORINSTANCELIST *
   Returns : 
   Info : 
*/

void UpdateMailboxes(ACTORINSTANCE *ptr);

/* --------------------------------------------------------------------------------
   Function : UpdateIndividualMailbox
   Purpose : updates individual mailboxes
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void UpdateIndividualMailbox(ACTORINSTANCE *mailbox, ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CubeDistanceTest
   Purpose : checks to see iftwo objects occupy cube of side length
   Parameters : TBVector, TBVector, float
   Returns : true, false
   Info : 
*/

int CubeDistanceTest(TBVector position1, TBVector position2, float distance);


#endif