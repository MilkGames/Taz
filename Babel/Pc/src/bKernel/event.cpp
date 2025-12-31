// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : event.cpp
//   Purpose : Event functions
// Component : Babel Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Globals

int bInsideEventCallback; // I have no idea why is this global - do you?

// ********************************************************************************
// Locals

TBEvent bEventRoot;          // global sentinel (circular list of events)
TBMutex bEventMutex;         // global mutex for event lists

// ********************************************************************************
// Helper Functions

// Local helper seen as a standalone function on Xbox, but inlined on PC
// and can be seen in Taz.map on PC:
//   - remove and free all clients of 'evt'
//   - unlink 'evt' from the global event ring
//   - free 'evt' itself
void DeleteEvent(TBEvent* evt)
{
    if (!evt) return;

    // Hold the mutex while we mutate client and event rings.
    bkWaitMutex(&bEventMutex);

    // --- delete all clients of this event (exact traversal pattern from decomp) ---
    TBEventClient* root = &evt->clients;   // clients list sentinel stored inside TBEvent
    TBEventClient* it   = root->next;

    // Important: step forward first, then free the node we just passed.
    // Safe because each TBEventClient (and its queue buffer, if present) is one contiguous block.
    while (it != root) {
        it = it->next;               // step forward
        bkHeapFree(it->prev);        // free the node we just passed
        it->prev = 0;                // PC decomp zeroes prev of the current node (cosmetic)
    }

    // Reset clients list to an empty self-loop.
    root->next = root;
    root->prev = root;

    // --- unlink the event node from the global event ring ---
    TBEvent* nextEvt = evt->next;
    TBEvent* prevEvt = evt->prev;
    nextEvt->prev = prevEvt;
    prevEvt->next = nextEvt;

    bkReleaseMutex(&bEventMutex);

    // Free the event node itself.
    bkHeapFree(evt);
}

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
    bInsideEventCallback = 0;
    bEventRoot.prev = &bEventRoot;   // empty ring: root points to itself
    bEventRoot.next = &bEventRoot;
    bkCreateMutex(&bEventMutex);
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
    bInsideEventCallback = 0;

    // Iterator over the event ring. We keep the "step-first, use-prev" pattern
    // to match the exact PC decomp behavior and remain safe during deletes
    TBEvent* it = bEventRoot.next;
    if (it != &bEventRoot)
    {
        do {
            it = it->next;           // step forward
            TBEvent* evt = it->prev; // the node we just passed is the one to delete
            DeleteEvent(evt);
        } while (it != &bEventRoot);
    }

    bkDeleteMutex(&bEventMutex);
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
    // Compute CRC for the provided name
    const uint32 crc = bkStringCRC(eventName);

    // Search the global circular doubly-linked list for an existing event
    TBEvent *it = bEventRoot.next;
    while (it != &bEventRoot) {
        if (it->crc == crc)
            return OK; // already exists
        it = it->next;
    }

    // Allocate a new event node
    TBEvent *evt = (TBEvent*)MALLOCEX(sizeof(TBEvent), (uint32)"Event");
    if (!evt)
        return FAIL; // oh-oh, out of memory

    // Insert at the tail of the global ring: [tail] <-> [evt] <-> [root]
    TBEvent *tail = bEventRoot.prev;
    evt->prev = tail;
    evt->next = &bEventRoot;
    tail->next = evt;
    bEventRoot.prev = evt;

    // Byte-wise copy of the name into the struct field
    {
        const char *s = eventName;
        char *d = evt->name;
        char ch;
        do { ch = *s++; *d++ = ch; } while (ch != '\0');
    }

	// Initialize the clients list as a self-referential sentinel
    evt->clients.prev = &evt->clients;
    evt->clients.next = &evt->clients;

	// Initialize metadata
    evt->crc        = crc;
    evt->noofQueues = 0;

    return OK;
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
    const uint32 crc = bkStringCRC(eventName);

    // p = bEventRoot.next; while (p != &bEventRoot) { ...; p = p->next; }
    TBEvent *evt = bEventRoot.next;
    while (evt != &bEventRoot) {
        if (evt->crc == crc) {
            // MUST allocate via MALLOCEX with the exact group id (0x655858 -> "Event Client (Callback)").
            TBEventClient *client = (TBEventClient*)MALLOCEX(sizeof(TBEventClient), (uint32)"Event Client (Callback)");
            if (client == NULL)
                return NULL;

            // Link into the event’s client ring
            TBEventClient *prev = evt->clients.prev;
            client->next = &evt->clients;
            client->prev = prev;
            prev->next   = client;
            client->next->prev = client;

            // Payload init
            client->event = evt;
            client->type  = EBEVENTCLIENTTYPE_CALLBACK;
            client->callback.callback        = callback;
            client->callback.callbackContext = context;

            return client;
        }
        evt = evt->next;
    }

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
    const uint32 crc = bkStringCRC(eventName);

    // Traverse global event ring: evt = bEventRoot.next; while (evt != &bEventRoot) ...
    TBEvent *evt = bEventRoot.next;
    while (evt != &bEventRoot)
    {
        if (evt->crc == crc)
        {
            // Allocate TBEventClient + queue buffer in one contiguous block
            const size_t totalSize =
                sizeof(TBEventClient) + (size_t)queueSize * sizeof(TBEventEntry);

            TBEventClient *client =
                (TBEventClient *)MALLOCEX(totalSize, (uint32)"Event Client (Queue)");
            if (!client)
                return NULL;

            // Link into event's client ring (doubly-linked list)
            TBEventClient *prev = evt->clients.prev;
            client->next = &evt->clients;
            client->prev = prev;
            prev->next   = client;
            client->next->prev = client;

            // Basic client fields
            client->event = evt;
            client->type  = EBEVENTCLIENTTYPE_QUEUE;

            // Queue payload
            client->queue.queue   = (TBEventEntry *)((char *)client + sizeof(TBEventClient));
            client->queue.size    = 0;
            client->queue.maxSize = queueSize;
            client->queue.flags   = flags;

            // Increment queue count on this event
            evt->noofQueues++;

            return client;
        }

        evt = evt->next;
    }

    // Event with given name not found
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
    // No client or no events in queue
    if (!client || client->queue.size == 0)
        return FAIL;

    bkWaitMutex(&bEventMutex);

    TBEventEntry *queue = client->queue.queue;
    if (!queue)
    {
        bkReleaseMutex(&bEventMutex);
        return FAIL;
    }

    // FILO mode (stack-like behaviour)
    if (client->queue.flags & BEVENTQUEUEFLAG_FILO)
    {
        int index = --client->queue.size;    // pre-decrement size, use last entry
        TBEventEntry *entry = queue + index;

        // Copy parameter string (null-terminated)
        if (parmBuffer)
        {
            const char *src = entry->parms;
            char c;
            do
            {
                c = *src++;
                *parmBuffer++ = c;
            } while (c != '\0');
        }

        // Copy event data block (16 bytes)
        if (data)
        {
            memcpy(data, entry->data, BMAXEVENTDATALEN);
        }

        bkReleaseMutex(&bEventMutex);
        return OK;
    }
    else
    {
        // FIFO mode (queue behaviour)
        TBEventEntry *entry = queue;

        // Copy parameter string from first entry
        if (parmBuffer)
        {
            const char *src = entry->parms;
            char c;
            do
            {
                c = *src++;
                *parmBuffer++ = c;
            } while (c != '\0');
        }

        // Copy event data from first entry
        if (data)
        {
            memcpy(data, entry->data, BMAXEVENTDATALEN);
        }

        // Shift remaining entries down if more than one
        int count = client->queue.size;
        if (count > 1)
        {
            memmove(queue,
                    queue + 1,
                    static_cast<size_t>(count - 1) * sizeof(TBEventEntry));
        }

        --client->queue.size;

        bkReleaseMutex(&bEventMutex);
        return OK;
    }
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
    if (eventName != NULL)
    {
        uint32 wantCRC = bkStringCRC(eventName);

        // walk global event ring (no mutex taken in this search in the disasm)
        TBEvent* it = bEventRoot.next;
        while (it != &bEventRoot)
        {
            if (it->crc == wantCRC)
            {
                // delete this single event (inline in asm)
                bkWaitMutex(&bEventMutex);

                TBEventClient* head = &it->clients;
                TBEventClient* c = head->next;

                if (c != head)
                {
                    do
                    {
                        c = c->next;
                        bkHeapFree(c->prev);
                        c->prev = NULL;
                    } while (c != head);
                }

                head->next = head;
                head->prev = head;

                it->next->prev = it->prev;
                it->prev->next = it->next;

                bkReleaseMutex(&bEventMutex);

                bkHeapFree(it);
                return;
            }

            it = it->next;
        }

        return;
    }

    // eventName == NULL: delete all events (step-first, delete-prev pattern)
    TBEvent* iter = bEventRoot.next;
    if (iter == &bEventRoot) {
        return;
    }

    do
    {
        iter = iter->next;
        TBEvent* evt = iter->prev;

        bkWaitMutex(&bEventMutex);

        TBEventClient* head = &evt->clients;
        TBEventClient* c = head->next;

        if (c != head)
        {
            do
            {
                c = c->next;
                bkHeapFree(c->prev);
                c->prev = NULL;
            } while (c != head);
        }

        head->next = head;
        head->prev = head;

        evt->next->prev = evt->prev;
        evt->prev->next = evt->next;

        bkReleaseMutex(&bEventMutex);

        bkHeapFree(evt);

    } while (iter != &bEventRoot);
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
    bkWaitMutex(&bEventMutex);

    // unlink from doubly-linked list
    client->prev->next = client->next;
    client->next->prev = client->prev;

    // type == 1
    if (client->type == EBEVENTCLIENTTYPE_QUEUE) {
        client->event->noofQueues--;
    }

    bkHeapFree(client);

    bkReleaseMutex(&bEventMutex);
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
    if (!eventName) return FAIL;

    if (takeMutex) bkWaitMutex(&bEventMutex);

    const uint32 wantCRC = bkStringCRC(eventName);

    // find event: walk from bEventRoot.next up to &bEventRoot (sentinel)
    TBEvent *evt = bEventRoot.next;
    while (evt != &bEventRoot) {
        if (evt->crc == wantCRC) break;
        evt = evt->next;
    }
    if (evt == &bEventRoot) {
        if (takeMutex) bkReleaseMutex(&bEventMutex);
        return FAIL;
    }

    // iterate clients
    TBEventClient *head = &evt->clients;
    for (TBEventClient *c = head->next; c != head; c = c->next)
    {
        if (c->type == EBEVENTCLIENTTYPE_CALLBACK) {
            bInsideEventCallback = 1;
            c->callback.callback(eventName,
                                 parmString ? parmString : (char*)"",
                                 data,
                                 c->callback.callbackContext);
            bInsideEventCallback = 0;
            continue;
        }

        // queue client
        TBEventClientQueue *q = &c->queue;

        // full?
        if (q->size >= q->maxSize && q->maxSize > 0) {
            if (q->flags & BEVENTQUEUEFLAG_NEWPRIORITY) {
                // drop oldest: shift left by 1 entry
                if (q->maxSize > 1) {
                    const size_t bytes = (size_t)(q->maxSize - 1) * sizeof(TBEventEntry);
                    memmove(q->queue, q->queue + 1, bytes);
                }
                if (q->size > 0) q->size -= 1;
            } else {
                const char *ps = parmString ? parmString : "[NULL]";
                const char *ds = data ? (const char*)data : "[NULL]";
                bkPrintf("bkGenerateEvent: *** WARNING *** Event '%s' lost parameters '%s' '%s' due to queue overflow ***\n",
                         eventName, ps, ds);
                continue;
            }
        }

        // append at tail
        int idx = q->size;
        if (idx < 0) idx = 0;
        if (idx >= q->maxSize) idx = q->maxSize - 1;

        TBEventEntry *e = &q->queue[idx];

        // copy parameter string (ASCIIZ) (byte-by-byte until '\0')
        if (parmString) {
            char *dst = e->parms;
            const char *src = parmString;
            while ((*dst++ = *src++) != '\0') { /* no-op */ }
        } else {
            e->parms[0] = '\0';
        }

        // copy 16-byte payload (4 dwords)
        if (data) {
            const uint32 *src = (const uint32*)data;
            ((uint32*)e->data)[0] = src[0];
            ((uint32*)e->data)[1] = src[1];
            ((uint32*)e->data)[2] = src[2];
            ((uint32*)e->data)[3] = src[3];
        } else {
            ((uint32*)e->data)[0] = 0;
            ((uint32*)e->data)[1] = 0;
            ((uint32*)e->data)[2] = 0;
            ((uint32*)e->data)[3] = 0;
        }

        if (q->size < q->maxSize) q->size += 1;
    }

    if (takeMutex) bkReleaseMutex(&bEventMutex);
    return OK;
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