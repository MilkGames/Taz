// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : event.cpp
//   Purpose : Event functions
// Component : Babel Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/* --------------------------------------------------------------------------------
   Function : bInitEvents
   Purpose : initialise the event subsystem
   Parameters : 
   Returns : 
   Info : 
*/

void bInitEvents()
{
        bkPrintf("*** WARNING *** bInitEvents was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bShutdownEvents
   Purpose : shutdown the event subsystem
   Parameters : 
   Returns : 
   Info : 
*/

void bShutdownEvents()
{
        bkPrintf("*** WARNING *** bShutdownEvents was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bFindEvent
   Purpose : locate an event by name
   Parameters : crc for name of event
   Returns : ptr to event or NULL for failure
   Info : INTERNAL
*/

TBEvent *bFindEvent(uint32 crc)
{
        bkPrintf("*** WARNING *** bFindEvent was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}


/* --------------------------------------------------------------------------------
   Function : bkCreateEvent
   Purpose : create a new event type
   Parameters : name for event
   Returns : OK/FAIL
   Info : 
*/

int bkCreateEvent(char *eventName)
{
        bkPrintf("*** WARNING *** bkCreateEvent was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkTrapEventCallback
   Purpose : attach a callback to an event
   Parameters : event name, callback, callback context
   Returns : ptr to event client or NULL for failure
   Info : 
*/

TBEventClient *bkTrapEventCallback(char *eventName, TBEventCallback callback, void *context)
{
        bkPrintf("*** WARNING *** bkTrapEventCallback was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}


/* --------------------------------------------------------------------------------
   Function : bkTrapEventQueue
   Purpose : attach a queue to an event
   Parameters : event name, size of queue, behaviour flags
   Returns : ptr to event client or NULL for failure
   Info : 
*/

TBEventClient *bkTrapEventQueue(char *eventName, int queueSize, uint32 flags)
{
        bkPrintf("*** WARNING *** bkTrapEventQueue was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}


/* --------------------------------------------------------------------------------
   Function : bkPopEvent
   Purpose : pop an event (if there is one) from an event queue
   Parameters : event client, ptr to buffer for event parameter string, ptr to buffer for event data
   Returns : OK=popped event, FAIL=no event
   Info : parmBuffer and data can be NULL if we don't care
*/

int bkPopEvent(TBEventClient *client, char *parmBuffer, void *data)
{
        bkPrintf("*** WARNING *** bkPopEvent was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkDeleteEvent
   Purpose : delete an event
   Parameters : event name or NULL for all events
   Returns : 
   Info : 
*/

void bkDeleteEvent(char *eventName)
{
        bkPrintf("*** WARNING *** bkDeleteEvent was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkDeleteEventClient
   Purpose : delete an event client
   Parameters : ptr to event client
   Returns : 
   Info : 
*/

void bkDeleteEventClient(TBEventClient *client)
{
        bkPrintf("*** WARNING *** bkDeleteEventClient was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkGenerateEvent
   Purpose : generate an event
   Parameters : event name, parameter string, event data, take mutex ?
   Returns : OK/FAIL
   Info : parmString or data can be NULL
*/

int bkGenerateEvent(char *eventName, char *parmString, void *data, int takeMutex)
{
        bkPrintf("*** WARNING *** bkGenerateEvent was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkFlushEventQueue
   Purpose : flush an event queue
   Parameters : ptr to event queue
   Returns : 
   Info : The takeMutex parameter is for system use only, and should be left by client code
*/

void bkFlushEventQueue(TBEventClient *queue, int takeMutex)
{
        bkPrintf("*** WARNING *** bkFlushEventQueue was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkFlushAllEventQueues
   Purpose : delete all queues attached to an event
   Parameters : event name or NULL for all events
   Returns : 
   Info : 
*/

void bkFlushAllEventQueues(char *eventName)
{
        bkPrintf("*** WARNING *** bkFlushAllEventQueues was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkHexToInt32
   Purpose : convert a 32bit (8 digit) hex string to an integer
   Parameters : string (only first 8 chars used)
   Returns : integer
   Info : 
*/

uint32 bkHexToInt32(char *string)
{
        bkPrintf("*** WARNING *** bkHexToInt32 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkDeleteEventTraps
   Purpose : delete all traps linked to an event
   Parameters : event name
   Returns : 
   Info : 
*/

void bkDeleteEventTraps(char *eventName)
{
        bkPrintf("*** WARNING *** bkDeleteEventTraps was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}