#ifndef __H_TRIGGER
#define __H_TRIGGER

#include "events.h"

typedef struct TAG_TRIGGEROBJECTINFO
{
	TRIGGERINFO	*onContact;
	TRIGGERINFO *enterRadius;
	TRIGGERINFO *exitRadius;
	float		radius;
	bool		active;			// is trigger currently active (for area triggers)
	bool		disabled;			// TP: is trigger disabled or not
	bool		deactivateOnExit;	// TP: if this is true the trigger box will deactive when character exits
	bool		maskNextMessage;	// TP: the next time this trigger changes state it will not send any messages
} TRIGGEROBJECTINFO;

int	CreateTriggerObjectInstance(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : TriggerCallback
   Purpose : send messages upon collision
   Parameters : 
   Returns : 
   Info : 
*/
int TriggerCallback(ACTORINSTANCE *collider, ACTORINSTANCE *triggerObject, struct TAG_PRECOLLINFO *preCollInfo);

/* --------------------------------------------------------------------------------
   Function : CheckForAreaTriggers
   Purpose : check for character entering and exiting area based triggers
   Parameters : actor for test, triggerlist
   Returns : 
   Info : 
*/
void CheckForAreaTriggers(ACTORINSTANCE *actor,ACTORINSTANCELIST *triggerList);

/*  --------------------------------------------------------------------------------
	Function	: IsTriggerActive
	Purpose		: 
	Parameters	: crc of instance
	Returns		: 
	Info		: 
*/
bool IsTriggerActive(uint32	crc, ACTORINSTANCELIST *triggerList);

#endif