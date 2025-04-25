// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : event.h
//   Purpose : event functions
// Component : Generic Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BKERNEL_EVENT_H__
#define __BKERNEL_EVENT_H__

// ********************************************************************************
// Constants and Macros

// event client types
typedef enum EBEventClientType {
	EBEVENTCLIENTTYPE_CALLBACK,
	EBEVENTCLIENTTYPE_QUEUE,
};

// event queue flags
#define BEVENTQUEUEFLAG_FILO			0x00000001			// the queue runs as a FILO instead of the default FIFO behaviour
#define BEVENTQUEUEFLAG_NEWPRIORITY		0x00000002			// new events take priority kicking out old ones instead of vice versa

#define BMAXEVENTPARMLEN				256					// maximum length of an event parameter string
#define BMAXEVENTDATALEN				16					// maximum length of an data parameter string


// ********************************************************************************
// Types and Structures

// an event callback function
typedef void (*TBEventCallback)(char *name, char *parms, void *data, void *context);


// an event queue entry
typedef struct _TBEventEntry {
	char						parms[BMAXEVENTPARMLEN];	// parameter string
	uchar						data[BMAXEVENTDATALEN];		// data associated with this event
} TBEventEntry;


// a callback client's info
typedef struct _TBEventClientCallback {
	TBEventCallback				callback;			// callback function (NULL if this is a queue entry)
	void						*callbackContext;	// context for callback function
} TBEventClientCallback;


// a queue client's info
typedef struct _TBEventClientQueue {
	TBEventEntry				*queue;				// event entry queue
	int							size;				// #events in the queue
	int							maxSize;			// maximum size of queue
	uint32						flags;				// behaviour flags (see BEVENTQUEUEFLAG_)
} TBEventClientQueue;


// an event client
typedef struct _TBEventClient {
	struct _TBEventClient		*next, *prev;		// list links
	EBEventClientType			type;				// type of client
	struct _TBEvent				*event;				// list of events this client belongs to

	union {
		TBEventClientCallback	callback;			// info for callback clients
		TBEventClientQueue		queue;				// info for queue clients
	};
} TBEventClient;


// an event
typedef struct _TBEvent {
	char						name[32];			// event name
	uint32						crc;				// CRC of name
	struct _TBEvent				*next, *prev;		// list links
	TBEventClient				clients;			// root of client list
	int							noofQueues;			// number of client queues attached to this event
} TBEvent;

// ********************************************************************************
// Globals

extern int				bInsideEventCallback;	// Indication if we are inside and event callback


// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bInitEvents
   Purpose : initialise the event subsystem
   Parameters : 
   Returns : 
   Info : 
*/

void bInitEvents();


/* --------------------------------------------------------------------------------
   Function : bShutdownEvents
   Purpose : shutdown the event subsystem
   Parameters : 
   Returns : 
   Info : 
*/

void bShutdownEvents();


/* --------------------------------------------------------------------------------
   Function : bFindEvent
   Purpose : locate an event by name
   Parameters : crc for name of event
   Returns : ptr to event or NULL for failure
   Info : INTERNAL
*/

TBEvent *bFindEvent(uint32 crc);


/* --------------------------------------------------------------------------------
   Function : bkCreateEvent
   Purpose : create a new event type
   Parameters : name for event
   Returns : OK/FAIL
   Info : 
*/

int bkCreateEvent(char *eventName);


/* --------------------------------------------------------------------------------
   Function : bkTrapEventCallback
   Purpose : attach a callback to an event
   Parameters : event name, callback, callback context
   Returns : ptr to event client or NULL for failure
   Info : 
*/

TBEventClient *bkTrapEventCallback(char *eventName, TBEventCallback callback, void *context);


/* --------------------------------------------------------------------------------
   Function : bkTrapEventQueue
   Purpose : attach a queue to an event
   Parameters : event name, size of queue, behaviour flags
   Returns : ptr to event client or NULL for failure
   Info : 
*/

TBEventClient *bkTrapEventQueue(char *eventName, int queueSize, uint32 flags);


/* --------------------------------------------------------------------------------
   Function : bkPopEvent
   Purpose : pop an event (if there is one) from an event queue
   Parameters : event client, ptr to buffer for event parameter string, ptr to buffer for event data
   Returns : OK=popped event, FAIL=no event
   Info : parmBuffer and data can be NULL if we don't care
*/

int bkPopEvent(TBEventClient *client, char *parmBuffer=NULL, void *data=NULL);


/* --------------------------------------------------------------------------------
   Function : bkDeleteEvent
   Purpose : delete an event
   Parameters : event name or NULL for all events
   Returns : 
   Info : 
*/

void bkDeleteEvent(char *eventName);


/* --------------------------------------------------------------------------------
   Function : bkDeleteEventClient
   Purpose : delete an event client
   Parameters : ptr to event client
   Returns : 
   Info : 
*/

void bkDeleteEventClient(TBEventClient *client);


/* --------------------------------------------------------------------------------
   Function : bkGenerateEvent
   Purpose : generate an event
   Parameters : event name, parameter string, event data, take mutex ?
   Returns : OK/FAIL
   Info : parmString or data can be NULL
*/

int bkGenerateEvent(char *eventName, char *parmString = NULL, void *data = NULL, int takeMutex = 1);


/* --------------------------------------------------------------------------------
   Function : bkFlushEventQueue
   Purpose : flush an event queue
   Parameters : ptr to event queue
   Returns : 
   Info : The takeMutex parameter is for system use only, and should be left by client code
*/

void bkFlushEventQueue(TBEventClient *queue, int takeMutex = 1);


/* --------------------------------------------------------------------------------
   Function : bkFlushAllEventQueues
   Purpose : delete all queues attached to an event
   Parameters : event name or NULL for all events
   Returns : 
   Info : 
*/

void bkFlushAllEventQueues(char *eventName);


/* --------------------------------------------------------------------------------
   Function : bkHexToInt32
   Purpose : convert a 32bit (8 digit) hex string to an integer
   Parameters : string (only first 8 chars used)
   Returns : integer
   Info : 
*/

uint32 bkHexToInt32(char *string);


/* --------------------------------------------------------------------------------
   Function : bkDeleteEventTraps
   Purpose : delete all traps linked to an event
   Parameters : event name
   Returns : 
   Info : 
*/

void bkDeleteEventTraps(char *eventName);

#endif		// __BKERNEL_EVENT_H__