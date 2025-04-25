// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : trigger.cpp
//   Purpose : trigger functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"

#include "trigger.h"
#include "collisioncache.h"
#include "main.h"
#include "physics.h"


/* --------------------------------------------------------------------------------
   Function : CreateTriggerObjectInstance
   Purpose : Create trigger object
   Parameters : 
   Returns : 
   Info : 
*/
int	CreateTriggerObjectInstance(ACTORINSTANCE *actorInstance)
{
	TRIGGEROBJECTINFO	*triggerInfo;

	if (triggerInfo = (TRIGGEROBJECTINFO*)ZALLOC(sizeof(TRIGGEROBJECTINFO)))
	{
		actorInstance->special = SPECIALTYPE_TRIGGER;
		actorInstance->specialType = (void*)triggerInfo;
		triggerInfo->onContact = NULL;
		triggerInfo->enterRadius = NULL;
		triggerInfo->exitRadius = NULL;
		triggerInfo->radius = 0.0f;
		triggerInfo->active = false;
		triggerInfo->disabled = false;
		triggerInfo->deactivateOnExit = false;
		triggerInfo->maskNextMessage = false;
		return TRUE;
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : TriggerCallback
   Purpose : send messages upon collision
   Parameters : 
   Returns : 
   Info : 
*/
int TriggerCallback(ACTORINSTANCE *collider, ACTORINSTANCE *triggerObject, PRECOLLINFO *preCollInfo)
{
	if ((collider->genericAI)||(collider->flags&ACTORFLAG_DEBRISACTOR)) return FALSE;

	if ((triggerObject->triggerObjectInfo->onContact)&&(!triggerObject->triggerObjectInfo->disabled))
	{
		SendTrigger(triggerObject->triggerObjectInfo->onContact);
	}
	return FALSE;
}

/*  --------------------------------------------------------------------------------
	Function	: IsTriggerActive
	Purpose		: 
	Parameters	: crc of instance, triggerlist
	Returns		: 
	Info		: 
*/
bool IsTriggerActive(uint32	crc, ACTORINSTANCELIST *triggerList)
{
	ACTORINSTANCE		*ptr;

	for (ptr = triggerList->head.next;ptr != &triggerList->head;ptr = ptr->next)
	{
		if (ptr->special==SPECIALTYPE_TRIGGER)
		{
			if (crc == ptr->instanceCRC)
			{
				return ptr->triggerObjectInfo->active;
			}
		}
	}

	return false;
}

/* --------------------------------------------------------------------------------
   Function : CheckForAreaTriggers
   Purpose : check for character entering and exiting area based triggers
   Parameters : actor for test, triggerlist
   Returns : 
   Info : 
*/
void CheckForAreaTriggers(ACTORINSTANCE *actorInstance,ACTORINSTANCELIST *triggerList)
{
	ACTORINSTANCE		*ptr;
	TRIGGEROBJECTINFO	*triggerObjectInfo;
	TBVector			tempVector,pos;
	char				message[64];
	float				len,len2;
	TBVector			position;
	TBVector			position2;

	if (actorInstance->actorBehaviour)
	{
		bmVectorAdd(position, actorInstance->actorInstance.position, actorInstance->actorBehaviour->collSphere.offset);
	}
	else
	{
		bmVectorCopy(position, actorInstance->actorInstance.position);
	}

	if (gameStatus.player2)
	{
		if(gameStatus.player2->actorBehaviour)// PP: surely this wasn't right(?):		(actorInstance->actorBehaviour)
		{
			bmVectorAdd(position2, gameStatus.player2->actorInstance.position, gameStatus.player2->actorBehaviour->collSphere.offset);	
		}
		else
		{
			bmVectorCopy(position2, gameStatus.player2->actorInstance.position);
		}
	}

	for (ptr = triggerList->head.next;ptr != &triggerList->head;ptr = ptr->next)
	{
		if (ptr->special==SPECIALTYPE_TRIGGER)
		{
			triggerObjectInfo = ptr->triggerObjectInfo;

			if (!triggerObjectInfo->disabled)	// TP: if the trigger is not disabled
			{
				if (triggerObjectInfo->radius!=0.0f)	// TP: is it a radius based trigger
				{
					// radius based trigger
					bmVectorSub(tempVector,position,ptr->actorInstance.position);
					len = bmVectorLen(tempVector);
					if (gameStatus.player2)
					{
						bmVectorSub(tempVector,position2,ptr->actorInstance.position);
						len2 = bmVectorLen(tempVector);
						if (len2 < len) len = len2;
					}

					if (triggerObjectInfo->active)
					{
						// TP: character was inside the trigger so check for leaving
						if (len > triggerObjectInfo->radius)
						{
							// TP: character has left the radius so send messages and process deactivate on exit if need be
							if (triggerObjectInfo->exitRadius)
							{
								if (triggerObjectInfo->maskNextMessage)
								{
									triggerObjectInfo->maskNextMessage = false;
								}
								else
								{
									SendTrigger(triggerObjectInfo->exitRadius, ptr->actorInstance.position);
								}
							}
							triggerObjectInfo->active = FALSE;
							if (triggerObjectInfo->deactivateOnExit) triggerObjectInfo->disabled = TRUE;
						}
					}
					else
					{
						if (triggerObjectInfo->deactivateOnExit) 
						{
							triggerObjectInfo->disabled = TRUE;
							continue;
						}
						// TP: character was not inside radius so check for entering
						if (len <= triggerObjectInfo->radius)
						{
							if (triggerObjectInfo->enterRadius) 
							{
								if (triggerObjectInfo->maskNextMessage)
								{
									triggerObjectInfo->maskNextMessage = false;
								}
								else
								{
									SendTrigger(triggerObjectInfo->enterRadius, ptr->actorInstance.position);
								}
							}
							triggerObjectInfo->active = TRUE;
						}
					}
				}
				else
				{
					// box trigger
					if (triggerObjectInfo->active)
					{
						// TP: character was inside box so check for leaving
						if (IsPointWithinBox(ptr,position)==FALSE)
						{
							// TP: character has left
							if (triggerObjectInfo->exitRadius)
							{
								if (gameStatus.player2&&(IsPointWithinBox(ptr,position2)==TRUE))
								{
								}
								else
								{
									if (triggerObjectInfo->maskNextMessage)
									{
										triggerObjectInfo->maskNextMessage = false;
									}
									else
									{
										SendTrigger(triggerObjectInfo->exitRadius, ptr->actorInstance.position);
									}
								}
							}
							triggerObjectInfo->active = FALSE;
							if (triggerObjectInfo->deactivateOnExit) triggerObjectInfo->disabled = TRUE;
						}
					}
					else
					{
						if (triggerObjectInfo->deactivateOnExit) 
						{
							triggerObjectInfo->disabled = TRUE;
							continue;
						}

						if (IsPointWithinBox(ptr,position))
						{
							if (triggerObjectInfo->enterRadius) 
							{
								if (triggerObjectInfo->maskNextMessage)
								{
									triggerObjectInfo->maskNextMessage = false;
								}
								else
								{
									SendTrigger(triggerObjectInfo->enterRadius, ptr->actorInstance.position);
								}
							}
							triggerObjectInfo->active = TRUE;
						}
						else
						{
							if (gameStatus.player2)
							{
								if (IsPointWithinBox(ptr,position2))
								{
									if (triggerObjectInfo->enterRadius) 
									{
										if (triggerObjectInfo->maskNextMessage)
										{
											triggerObjectInfo->maskNextMessage = false;
										}
										else
										{
											SendTrigger(triggerObjectInfo->enterRadius, ptr->actorInstance.position);
										}
									}
									triggerObjectInfo->active = TRUE;
								}
							}
						}
					}
				}
			}
		}
	}
}
