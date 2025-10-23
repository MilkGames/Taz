// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : extras.cpp
//   Purpose : Defines constants, function pointer types, and the TBExtraSystem struct for managing extra effects systems
// Component : Generic Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Globals

TBExtraSystem bExtraSystemList;

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bInitExtras
	Purpose : initialise the Extras sub-module
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/

int bInitExtras()
{
	bExtraSystemList.prev = &bExtraSystemList;
	bExtraSystemList.next = &bExtraSystemList;
	return OK;
}


/*	--------------------------------------------------------------------------------
	Function : bShutdownExtras
	Purpose : shutdown the Extras sub-module
	Parameters : 
	Returns : 
	Info : 
*/

void bShutdownExtras()
{
    while (bExtraSystemList.next != &bExtraSystemList) {
        baDeleteExtraSystem(bExtraSystemList.next);
    }
}


/*	--------------------------------------------------------------------------------
	Function : baCreateExtraSystem
	Purpose : create a new extra system
	Parameters : ident, creation function, update function, delete function, render function, system init function, size of data block,
				 max simultaneous instances, flags, system data size
	Returns : ptr to system or NULL for failure
	Info : 
*/

TBExtraSystem *baCreateExtraSystem(char *ident, TBExtraCreateFunc create, TBExtraUpdateFunc update, TBExtraDeleteFunc del, 
								   TBExtraRenderFunc render, TBExtraSystemInitFunc sysInit, int32 dataSize, int32 maxInstances,
								   uint32 flags, int32 systemDataSize)
{
		bkPrintf("*** WARNING *** baCreateExtraSystem was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
	TBExtraSystem *example;
	return example;
}


/*	--------------------------------------------------------------------------------
	Function : baDeleteExtraSystem
	Purpose : delete a new extra system
	Parameters : ptr to system to delete or NULL for all systems
	Returns : 
	Info : 
*/

void baDeleteExtraSystem(TBExtraSystem *eSystem)
{
    if (eSystem == NULL) {
        // delete all systems
        while (bExtraSystemList.next != &bExtraSystemList) {
            baDeleteExtraSystem(bExtraSystemList.next);
        }
        return;
    }

    // if there are live instances, call the system’s deleteFunc on each one (last>first)
    if (eSystem->noofInstances > 0 && eSystem->deleteFunc) {
        while (eSystem->noofInstances > 0) {
            const int idx = eSystem->noofInstances - 1;
            eSystem->noofInstances = idx;
            // deleteFunc(TBExtraSystem*, uchar* data)
            eSystem->deleteFunc(eSystem, eSystem->dataPtrs[idx]);
        }

        // reset counters
        eSystem->noofInstances = 0;
        eSystem->nextInstance  = 0;

        // clear active flags for the full capacity.
        const int cap = (eSystem->maxInstances & 0x3fffffff);
        for (int i = 0; i < cap; ++i) {
            eSystem->activeFlags[i] = 0;
        }
        // the decompile had a second tiny loop for potential byte-tail; safe to ignore in C/C++
    }

    // call system init function with reason=1 (decompile passes 1 here)
    if (eSystem->sysInitFunc) {
        eSystem->sysInitFunc(eSystem, /*reason=*/1); // BEXSYSFLAG_NEWPRIORITY ??
    }

    // unlink from the global doubly-linked list
    eSystem->next->prev = eSystem->prev;
    eSystem->prev->next = eSystem->next;

    // free the system itself
    bkHeapFree(eSystem);
}


/*	--------------------------------------------------------------------------------
	Function : baFindExtraSystem
	Purpose : find an extra system from it's ident
	Parameters : ident
	Returns : ptr to system or NULL for failure
	Info : 
*/

TBExtraSystem *baFindExtraSystem(char *ident)
{
		bkPrintf("*** WARNING *** baFindExtraSystem was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
	TBExtraSystem *example;
	return example;
}


/*	--------------------------------------------------------------------------------
	Function : baCreateExtra
	Purpose : create a new extra instance
	Parameters : system ptr, parameters...
	Returns : ptr to parms for success or NULL for failure
	Info : 
*/

uchar *baCreateExtra(TBExtraSystem *eSystem, ...)
{
        bkPrintf("*** WARNING *** baCreateExtra was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}


/*	--------------------------------------------------------------------------------
	Function : baCloneExtra
	Purpose : create a new extra instance from an existing parameter template
	Parameters : system ptr, parameter template
	Returns : ptr to parms for success or NULL for failure
	Info : 
*/

uchar *baCloneExtra(TBExtraSystem *eSystem, void *parmTemplate)
{
        bkPrintf("*** WARNING *** baCloneExtra was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}


/*	--------------------------------------------------------------------------------
	Function : baResetExtraSystem
	Purpose : delete all extras currently active within a system
	Parameters : ptr to system or NULL for all systems
	Returns : 
	Info : 
*/

void baResetExtraSystem(TBExtraSystem *eSystem)
{
        bkPrintf("*** WARNING *** baResetExtraSystem was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : baUpdateExtraSystem
	Purpose : update all extras in a system or systems
	Parameters : ptr to system or NULL for all systems, 't' value to update by
	Returns : 
	Info : 
*/

void baUpdateExtraSystem(TBExtraSystem *eSystem, float t)
{
        bkPrintf("*** WARNING *** baUpdateExtraSystem was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : baRenderExtraSystem
	Purpose : render all extras in a system or systems
	Parameters : ptr to system or NULL for all systems, context value to pass to render function
	Returns : 
	Info : 
*/

void baRenderExtraSystem(TBExtraSystem *eSystem, void *context)
{
        bkPrintf("*** WARNING *** baRenderExtraSystem was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : baDeleteExtraInstance
	Purpose : delete an extra instance from a system
	Parameters : ptr to system, index of extra (0..)
	Returns : 1=ok, 0=fail
	Info : 
*/

int baDeleteExtraInstance(TBExtraSystem *eSystem, int instanceIndex)
{
        bkPrintf("*** WARNING *** baDeleteExtraInstance was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : baSetExtraSystemLimit
	Purpose : set a new max limit for an extras system
	Parameters : system ptr, new limit
	Returns : new system ptr or NULL for failure
	Info : will cause the system to be reset
*/

TBExtraSystem *baSetExtraSystemLimit(TBExtraSystem *eSystem, int newMaxInstances)
{
		bkPrintf("*** WARNING *** baSetExtraSystemLimit was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
	TBExtraSystem *example;
	return example;
}


/*	--------------------------------------------------------------------------------
	Function : baSetExtraSystemPosition
	Purpose : set the position of an extra system
	Parameters : ptr to system, position
	Returns : 
	Info : 
*/

void baSetExtraSystemPosition(TBExtraSystem *eSystem, float x, float y, float z)
{
        bkPrintf("*** WARNING *** baSetExtraSystemPosition was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : baSetExtraSystemOrientation
	Purpose : set the orientation of an extra system
	Parameters : ptr to system, orientation
	Returns : 
	Info : 
*/

void baSetExtraSystemOrientation(TBExtraSystem *eSystem, TBQuaternion orientation)
{
        bkPrintf("*** WARNING *** baSetExtraSystemOrientation was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : baSetExtraSystemAttachment
	Purpose : set the attachment for an extra system
	Parameters : ptr to system, actor instance, actor node instance
	Returns : 
	Info : 
*/

void baSetExtraSystemAttachment(TBExtraSystem *eSystem, TBActorInstance *actInstance, TBActorNodeInstance *nodeInstance)
{
        bkPrintf("*** WARNING *** baSetExtraSystemAttachment was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}
