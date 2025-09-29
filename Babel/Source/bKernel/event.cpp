#include <babel.h>
//#define FOOBAR

#ifdef FOOBAR

TBEvent _events;
TBMutex _eventMutex;

int bInsideEventCallback;

void bInitEvents(){
	bInsideEventCallback = 0;
	_events.prev = &_events;
	_events.next = &_events;
	bkCreateMutex(&_eventMutex);
}

void bShutdownEvents(){
	bInsideEventCallback = 0;
}


TBEvent *bFindEvent(uint32 crc){

	TBEvent* ev;
	ev = _events.next;
	while(ev != NULL){
		if(ev == &_events){
			ev = NULL;
			break;
		}
		if(ev->crc == crc){
			break;
		}
		ev = ev->next;
	}
	return ev;

}

int bkCreateEvent(char *eventName){
	uint32 crc;
	TBEvent* ev;
	int result;

	crc = bkStringCRC(eventName);
	ev = bFindEvent(crc);
	if(ev == NULL){

		ev = (TBEvent*) bkHeapAllocEx(
			sizeof(TBEvent),
			__FILE__, __LINE__, 
			BALLOC_MALLOC | bUserModule, 
			0x2100, // Todo: figure out what this group parameter means
			0
		);
		
		if(ev == NULL){
			result = 0;
		} else {
			ev->prev = _events.prev;
			ev->next = &_events;
			ev->prev->next = ev;
			ev->next->prev = ev;
			strncpy(ev->name, eventName, 32);
			ev->clients.prev = &(ev->clients);
			ev->clients.next = &(ev->clients);
			ev->crc = crc;
			ev->noofQueues = 0;
			result = 1;
		}

	} else {
		result =  1;
	}
	return result;
}

/* --------------------------------------------------------------------------------
   Function : bkTrapEventCallback
   Purpose : attach a callback to an event
   Parameters : event name, callback, callback context
   Returns : ptr to event client or NULL for failure
   Info : 
*/

TBEventClient *bkTrapEventCallback(char *eventName, TBEventCallback callback, void *context){
	uint32 crc;
	TBEvent* ev;
	TBEventClient* evClient;
	
	crc = bkStringCRC(eventName);
	ev = bFindEvent(crc);
	if(ev == (TBEvent*) 0x0){
		evClient = (TBEventClient*) 0x0;
	} else {
		evClient = (TBEventClient*) bkHeapAllocEx(
			sizeof(TBEventClient), 
			__FILE__, __LINE__,
			BALLOC_MALLOC | bUserModule,
			0x212c, 
			0
		);
		if(evClient != (TBEventClient*) 0x0){
			evClient->prev = (ev->clients).prev;
			evClient->next = &(ev->clients);
			evClient->prev->next = evClient;
			evClient->next->prev = evClient;
			evClient->type = EBEVENTCLIENTTYPE_CALLBACK;
			evClient->callback.callback = callback;
			evClient->callback.callbackContext = context;
			evClient->event = ev;
		}
	}


	return evClient;
}

/* --------------------------------------------------------------------------------
   Function : bkTrapEventQueue
   Purpose : attach a queue to an event
   Parameters : event name, size of queue, behaviour flags
   Returns : ptr to event client or NULL for failure
   Info : 
*/

TBEventClient *bkTrapEventQueue(char *eventName, int queueSize, uint32 flags){
	uint32 crc;
	TBEvent* ev;
	TBEventClient* evClient;

	crc = bkStringCRC(eventName);
	ev = bFindEvent(crc);

	if(ev == (TBEvent*) 0x0){
		evClient = (TBEventClient*) 0x0;
	} else {
		evClient = (TBEventClient*) bkHeapAllocEx(
			sizeof(TBEventClient) + (sizeof(TBEventEntry) * queueSize),
			__FILE__,__LINE__,
			BALLOC_MALLOC | bUserModule,
			0x2168, 0
		);
		if(evClient != (TBEventClient*) 0x0){
			evClient->prev = (ev->clients).prev;
			evClient->next = &(ev->clients);
			evClient->prev->next = evClient;
			evClient->next->prev = evClient;
			evClient->type = EBEVENTCLIENTTYPE_QUEUE;
			evClient->queue.queue = (TBEventEntry*) (evClient + 1);
			evClient->queue.size = 0;
			evClient->queue.maxSize = queueSize;
			evClient->queue.flags = flags;
			evClient->event = ev;
			ev->noofQueues += 1;

		}
	}

	return evClient;

}


/* --------------------------------------------------------------------------------
   Function : bkPopEvent
   Purpose : pop an event (if there is one) from an event queue
   Parameters : event client, ptr to buffer for event parameter string, ptr to buffer for event data
   Returns : OK=popped event, FAIL=no event
   Info : parmBuffer and data can be NULL if we don't care
*/

int bkPopEvent(TBEventClient *client, char *parmBuffer, void *data){
	int result;
	
	if(client->queue.size == 0){
		result = 0;
	} else {
		bkWaitMutex(&_eventMutex);
		if((client->queue.flags & BEVENTQUEUEFLAG_FILO) == 0){
			if(parmBuffer != NULL && client->queue.queue != NULL){
			  strncpy(parmBuffer, client->queue.queue->parms, BMAXEVENTPARMLEN);
			}
			if(data != NULL && client->queue.queue != NULL
				&& client->queue.queue != (TBEventEntry*)0xffffff00){
			  memcpy(data, client->queue.queue->data, BMAXEVENTDATALEN);
			}
			if( 1 < client->queue.size ) {
			  memmove(client->queue.queue, client->queue.queue + 1, (client->queue.size - 1) * sizeof(TBEventEntry));
			}
			client->queue.size -= 1;
		} else {
			client->queue.size -= 1;
			if(parmBuffer != NULL && 
				((client->queue.queue + client->queue.size) != NULL)){
				strncpy(parmBuffer,client->queue.queue[client->queue.size].parms, BMAXEVENTPARMLEN);
			}
			if(data != NULL &&
				(client->queue.queue[client->queue.size].data != NULL))
				// ^ ghidra says this is what the condition is but I don't know that it makes sense
			{
				memcpy(data, client->queue.queue[client->queue.size].data, BMAXEVENTDATALEN);
			}
		}
		bkReleaseMutex(&_eventMutex);
		result = 1;
	}


	return result;

}

void DeleteEvent(TBEvent* event){
	
	TBEventClient* client;

	bkWaitMutex(&_eventMutex);

	client = event->clients.next;
	while(client != event->clients){
		client = client->next;
		bkHeapFree(client->prev);
		client->prev = NULL;
	}
	bkReleaseMutex(&_eventMutex);
	bkHeapFree(event, "event");

}

void bkDeleteEvent(char* eventName){
	uint32 crc;
	TBEvent* ev;
	if(eventName == NULL){
		ev = _events->next;
		while(ev != &events){
			ev = ev->next;
			DeleteEvent(ev->prev);
		}
	}else{
		crc = bkStringCRC(eventName);
		ev = bFindEvent(crc);
		if(ev != NULL){
			DeleteEvent(ev);
		}
	}

}


void bkDeleteEventClient(TBEventClient *client){
	bkWaitMutex(&_eventMutex);
	client->next->prev = client->prev;
	client->prev->next = client->next;
	if(client->type == EBEVENTCLIENTTYPE_QUEUE){
		client->event->noofQueues -= 1;
	}
	bkHeapFree("client");
	bkReleaseMutex(&_eventMutex);
}

int bkGenerateEvent(char* eventName, char* parmString, void* data, int takeMutex){
	uint32 crc;
	TBEvent* ev;
	int result;
	char* parmstr;
	char* databuf;
	TBEventClient* client;

	if( takeMutex != 0){
		bkWaitMutex(&_eventMutex);
	}
	crc = bkStringCRC(eventName);
	ev = bFindEvent(crc);
	if(ev == NULL){
		if (takeMutex != 0){
			bkReleaseMutex(&_eventMutex);
		}
		result = 0;
	} else {
		for(client = ev->clients.next; client != &ev->clients; client = client->next){
			if(client->queue.size == client->queue.maxSize){
			//todo tomorro

			}
		}
	}

	return result;

}
#endif