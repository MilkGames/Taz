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
// Locals

int stopSpam17 = 0;
int stopSpam18 = 0;
int stopSpam19 = 0;
int stopSpam20 = 0;

// ********************************************************************************
// Local Functions

void CalculateSystemXForm(TBExtraSystem *eSystem)
{
    TBMatrix local_c0;
    TBMatrix local_80;
    TBMatrix local_40;

    eSystem->flags &= ~BEXSYSFLAG_XFORMDIRTY;

    bmMatTranslate(local_c0,
                   eSystem->position[0],
                   eSystem->position[1],
                   eSystem->position[2]);

    bmQuatToMatrix(local_40, eSystem->orientation);

    if (eSystem->actorAttachedTo != NULL)
    {
        bmMatMultiply(local_80, local_c0, local_40);

        if (eSystem->nodeAttachedTo != NULL)
        {
            bmMatMultiply(local_c0,
                          eSystem->actorAttachedTo->objectToWorld,
                          eSystem->nodeAttachedTo->nodeToLocalWorld);

            bmMatMultiply(eSystem->systemToWorld, local_c0, local_80);
            return;
        }

        bmMatMultiply(eSystem->systemToWorld,
                      eSystem->actorAttachedTo->objectToWorld,
                      local_80);
        return;
    }

    bmMatMultiply(eSystem->systemToWorld, local_c0, local_40);
}

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
    uint32 dataSizeAligned = ((uint32)dataSize + 0x0Fu) & 0xFFFFFFF0u;
    uint32 maxInstancesEven = ((uint32)maxInstances + 1u) & 0xFFFFFFFEu;
    uint32 sysDataSizeAligned = ((uint32)systemDataSize + 0x0Fu) & 0xFFFFFFF0u;

    uint32 totalSize = (dataSizeAligned + 8u) * maxInstancesEven + 0xC0u + sysDataSizeAligned;

    TBExtraSystem *eSystem = (TBExtraSystem *)CALLOCEX(totalSize, 0, (uint32) "Extra System");
    if (eSystem == NULL)
        return NULL;

    {
        uchar *p = (uchar *)eSystem + 0xC0;

        eSystem->sysData = p;
        p += sysDataSizeAligned;

        eSystem->activeFlags = (int32 *)p;
        p += maxInstancesEven * 4u;

        eSystem->dataPtrs = (uchar **)p;
        p += maxInstancesEven * 4u;

        eSystem->data = p;
    }

    eSystem->prev = bExtraSystemList.prev;
    eSystem->next = &bExtraSystemList;
    bExtraSystemList.prev->next = eSystem;
    bExtraSystemList.prev = eSystem;

    eSystem->crc = bkStringCRC(ident);

    eSystem->flags = flags;
    eSystem->maxInstances = (int32)maxInstancesEven;
    eSystem->dataSize = (int32)dataSizeAligned;

    eSystem->createFunc = create;
    eSystem->updateFunc = update;
    eSystem->deleteFunc = del;
    eSystem->renderFunc = render;
    eSystem->sysInitFunc = sysInit;

    eSystem->noofInstances = 0;
    eSystem->nextInstance = 0;

    eSystem->sysDataSize = (int32)sysDataSizeAligned;
    eSystem->highestNoofInstances = 0;

    eSystem->position[0] = 0.0f;
    eSystem->position[1] = 0.0f;
    eSystem->position[2] = 0.0f;
    eSystem->position[3] = 1.0f;

    eSystem->orientation[0] = 0.0f;
    eSystem->orientation[1] = 0.0f;
    eSystem->orientation[2] = 0.0f;
    eSystem->orientation[3] = 1.0f;

    eSystem->actorAttachedTo = NULL;
    eSystem->nodeAttachedTo = NULL;

    if (sysInit != NULL)
        sysInit(eSystem, 0);

    CalculateSystemXForm(eSystem);
    return eSystem;
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
    if (eSystem == NULL)
    {
        if (bExtraSystemList.next == &bExtraSystemList)
            return;

        do
        {
            baDeleteExtraSystem(bExtraSystemList.next);
        } while (bExtraSystemList.next != &bExtraSystemList);

        return;
    }

    if (eSystem->noofInstances != 0)
    {
        if (eSystem->deleteFunc != NULL)
        {
            do
            {
                eSystem->noofInstances--;
                eSystem->deleteFunc(eSystem, eSystem->dataPtrs[eSystem->noofInstances]);
            } while (eSystem->noofInstances != 0);
        }

        eSystem->noofInstances = 0;
        eSystem->nextInstance = 0;

        {
            uint32 count = (uint32)eSystem->maxInstances;
            int32* p = eSystem->activeFlags;

            while (count != 0)
            {
                *p = 0;
                ++p;
                --count;
            }
        }
    }

    if (eSystem->sysInitFunc != NULL)
    {
        eSystem->sysInitFunc(eSystem, 1);
    }

    eSystem->next->prev = eSystem->prev;
    eSystem->prev->next = eSystem->next;

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
    int32 i;
    int32 idx;

    if (eSystem->noofInstances == eSystem->maxInstances)
    {
        if (eSystem->maxInstances == 0)
            return (uchar *)0;

        if (((eSystem->flags & 1) == 0) || (eSystem->noofInstances <= 0))
            return (uchar *)0;

        idx = 0;

        if (eSystem->deleteFunc != (TBExtraDeleteFunc)0)
        {
            while (idx < eSystem->noofInstances)
            {
                if (eSystem->deleteFunc(eSystem, eSystem->dataPtrs[idx]) != 0)
                    break;
                idx++;
            }

            if (idx >= eSystem->noofInstances)
                return (uchar *)0;
        }
        else
        {
            if (eSystem->noofInstances <= 0)
                return (uchar *)0;
        }

        i = (int32)((int32)(eSystem->dataPtrs[idx] - eSystem->data) / eSystem->dataSize);
        eSystem->activeFlags[i] = 0;

        if (idx != (eSystem->noofInstances - 1))
        {
            memmove(&eSystem->dataPtrs[idx],
                    &eSystem->dataPtrs[idx + 1],
                    (uint32)(eSystem->noofInstances - (idx + 1)) * 4);
        }

        eSystem->noofInstances -= 1;
    }

    while (eSystem->activeFlags[eSystem->nextInstance] != 0)
    {
        eSystem->nextInstance = (eSystem->nextInstance + 1) % eSystem->maxInstances;
    }

    eSystem->dataPtrs[eSystem->noofInstances] =
        eSystem->data + (eSystem->dataSize * eSystem->nextInstance);

    eSystem->activeFlags[eSystem->nextInstance] = 1;
    eSystem->noofInstances += 1;

    if (eSystem->noofInstances > eSystem->highestNoofInstances)
        eSystem->highestNoofInstances = eSystem->noofInstances;

    if (eSystem->createFunc != (TBExtraCreateFunc)0)
    {
        va_list args;
        va_start(args, eSystem);
        eSystem->createFunc(eSystem,
                            eSystem->dataPtrs[eSystem->noofInstances - 1],
                            args);
        va_end(args);
    }

    eSystem->nextInstance = (eSystem->nextInstance + 1) % eSystem->maxInstances;

    return (uchar *)eSystem->dataPtrs[eSystem->noofInstances - 1];
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
    if (eSystem == 0)
    {
        TBExtraSystem *cur = bExtraSystemList.next;
        while (cur != &bExtraSystemList)
        {
            baResetExtraSystem(cur);
            cur = cur->next;
        }
        return;
    }

    if (eSystem->noofInstances == 0)
        return;

    if (eSystem->deleteFunc != 0)
    {
        while (eSystem->noofInstances != 0)
        {
            eSystem->noofInstances -= 1;
            eSystem->deleteFunc(eSystem, eSystem->dataPtrs[eSystem->noofInstances]);
        }
    }

    eSystem->noofInstances = 0;
    eSystem->nextInstance  = 0;

    {
        int32 i;
        for (i = 0; i < eSystem->maxInstances; ++i)
            eSystem->activeFlags[i] = 0;
    }
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
    if (eSystem == NULL)
    {
        TBExtraSystem *it = bExtraSystemList.next;
        if (it == &bExtraSystemList)
            return;

        do
        {
            baUpdateExtraSystem(it, t);
            it = it->next;
        } while (it != &bExtraSystemList);

        return;
    }

    if (((eSystem->flags & BEXSYSFLAG_XFORMDIRTY) != 0) || (eSystem->actorAttachedTo != NULL))
    {
        CalculateSystemXForm(eSystem);
    }

    if ((eSystem->updateFunc != NULL) && (eSystem->noofInstances != 0))
    {
        eSystem->updateFunc(eSystem, eSystem->noofInstances, eSystem->dataPtrs, t);
    }
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
    if (eSystem == NULL)
    {
        TBExtraSystem *it = bExtraSystemList.next;
        if (it == &bExtraSystemList)
            return;

        do
        {
            baRenderExtraSystem(it, context);
            it = it->next;
        } while (it != &bExtraSystemList);

        return;
    }

    if (eSystem->renderFunc == NULL)
        return;

    if (eSystem->noofInstances == 0)
        return;

    if ((eSystem->flags & BEXSYSFLAG_GLOBALEXTRAS) != 0)
        bdSetObjectMatrix(bIdentityMatrix);
    else
        bdSetObjectMatrix(eSystem->systemToWorld);

    eSystem->renderFunc(eSystem, eSystem->noofInstances, eSystem->dataPtrs, context);
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
    int32 ok;

    if (eSystem->deleteFunc != NULL)
    {
        ok = eSystem->deleteFunc(eSystem, eSystem->dataPtrs[instanceIndex]);
        if (ok == 0)
        {
            return 0;
        }
    }

    /* activeFlags[((dataPtrs[i] - data) / dataSize)] = 0 */
    {
        int32 diff = (int32)((uchar *)eSystem->dataPtrs[instanceIndex] - (uchar *)eSystem->data);
        int32 slot = diff / eSystem->dataSize;
        eSystem->activeFlags[slot] = 0;
    }

    if (instanceIndex != (eSystem->noofInstances - 1))
    {
        uint32 moveBytes = (uint32)((eSystem->noofInstances - (instanceIndex + 1)) * 4);
        memmove(&eSystem->dataPtrs[instanceIndex],
                &eSystem->dataPtrs[instanceIndex + 1],
                moveBytes);
    }

    eSystem->noofInstances -= 1;
    return 1;
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
