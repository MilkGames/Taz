// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : resources.cpp
//   Purpose : resource management functions
// Component : Generic Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Globals

TBResourceInfo	bGlobalResourceList;
TBResourceInfo	bLocalResourceList[BNOOFRESTYPES];

// ********************************************************************************
// Locals



// ********************************************************************************
// Function Implementations

/* --------------------------------------------------------------------------------
   Function : bInitResources
   Purpose : initialise the resource lists
   Parameters : 
   Returns : 
   Info : 
*/

void bInitResources()
{
    // zero and init global anchor
    memset(&bGlobalResourceList, 0, sizeof(TBResourceInfo));
    bGlobalResourceList.globalPrev = &bGlobalResourceList;
    bGlobalResourceList.globalNext = &bGlobalResourceList;

    // zero and init each local anchor (one per resource type)
    for (int i = 0; i < BNOOFRESTYPES; ++i) {
        memset(&bLocalResourceList[i], 0, sizeof(TBResourceInfo));
        bLocalResourceList[i].localPrev = &bLocalResourceList[i];
        bLocalResourceList[i].localNext = &bLocalResourceList[i];
    }
}


/* --------------------------------------------------------------------------------
   Function : bAddGlobalResource
   Purpose : add a new resource to the global list
   Parameters : ptr to resource, package it came from, type of resource, group ID
   Returns : 
   Info : 
*/

void bAddGlobalResource(TBResourceInfo *resPtr, TBPackageIndex *pakSrc, int type, int groupID)
{
        bkPrintf("*** WARNING *** bAddGlobalResource was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bDeleteGlobalResource
   Purpose : delete a resource from the global list
   Parameters : ptr to resource
   Returns : 
   Info : 
*/

void bDeleteGlobalResource(void *resPtr)
{
        bkPrintf("*** WARNING *** bDeleteGlobalResource was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkListResources
   Purpose : list all resources that have been loaded
   Parameters : bitmask of resource types (see BRESMASK_)
   Returns : 
   Info : 
*/

void bkListResources(uint typeMask)
{
        bkPrintf("*** WARNING *** bkListResources was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkLoadPackageResources
   Purpose : list all resources from a package
   Parameters : package index, bitmask of resource types (see BRESMASK_), group ID to assign, tag to match
   Returns : #resources loaded
   Info : 
*/

int bLoadPackageResources(TBPackageIndex *pak, uint typeMask, int groupId, uint32 tagMatch)
{
        bkPrintf("*** WARNING *** bLoadPackageResources was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bDeletePackageResources
   Purpose : delete all resources that came from a package
   Parameters : package ID, bitmask of resource types (see BRESMASK_)
   Returns : 
   Info : 
*/

void bDeletePackageResources(TBPackageID packageId, uint typeMask)
{
        bkPrintf("*** WARNING *** bDeletePackageResources was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : bDeleteResourceGroup
   Purpose : delete all resources with a specific group
   Parameters : bitmask of resource types (see BRESMASK_), group ID
   Returns : 
   Info : 
*/

void bDeleteResourceGroup(uint typeMask, uint groupID)
{
        bkPrintf("*** WARNING *** bDeleteResourceGroup was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : bDeleteAllResources
   Purpose : delete all resources
   Parameters : 
   Returns : 
   Info : 
*/

void bDeleteAllResources()
{
        bkPrintf("*** WARNING *** bDeleteAllResources was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : bDeleteResource
   Purpose : delete a single resource
   Parameters : ptr to resource
   Returns : 
   Info : 
*/

void bDeleteResource(void *resPtr)
{
        bkPrintf("*** WARNING *** bDeleteResource was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : bLoadResource
   Purpose : load a single resource
   Parameters : ptr to package index (or NULL to search RAM), filename, resource type, group ID
   Returns : ptr to resource or NULL for failure
   Info : 
*/

TBResourceInfo *bLoadResource(TBPackageIndex *index, char *filename, EBResourceType resType, int groupID)
{
        bkPrintf("*** WARNING *** bLoadResource was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}

/* --------------------------------------------------------------------------------
   Function : bLoadResourceByCRC
   Purpose : load a single resource from it's CRC
   Parameters : ptr to package index (or NULL to search RAM), crc, resource type, group ID
   Returns : ptr to resource or NULL for failure
   Info : 
*/

TBResourceInfo *bLoadResourceByCRC(TBPackageIndex *index, uint crc, EBResourceType resType, int groupID)
{
        bkPrintf("*** WARNING *** bLoadResourceByCRC was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}

/* --------------------------------------------------------------------------------
   Function : bkFindResourceByCRC
   Purpose : find a loaded resource
   Parameters : resource type, crc, package id, group id
   Returns : ptr to resource or NULL for failure
   Info : 
*/

TBResourceInfo *bkFindResourceByCRC(EBResourceType resType, uint32 crc, TBPackageID packageId, uint32 groupId, uint32 flags)
{
        bkPrintf("*** WARNING *** bkFindResourceByCRC was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}


/* --------------------------------------------------------------------------------
   Function : bkWalkResourceList
   Purpose : walk the local resource list
   Parameters : resource type, resource to walk from/NULL to start
   Returns : ptr to next resource of that type, or NULL if none/end of list
   Info : 
*/

TBResourceInfo *bkWalkResourceList(EBResourceType resType, TBResourceInfo *resource)
{
        bkPrintf("*** WARNING *** bkWalkResourceList was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}


/* --------------------------------------------------------------------------------
   Function : bkWalkGlobalResourceList
   Purpose : walk the global resource list
   Parameters : resource to walk from/NULL to start
   Returns : ptr to next resource, or NULL if none/end of list
   Info : 
*/

TBResourceInfo *bkWalkGlobalResourceList(TBResourceInfo *resource)
{
        bkPrintf("*** WARNING *** bkWalkGlobalResourceList was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}


/* --------------------------------------------------------------------------------
   Function : bkTakeResourceSnapshot
   Purpose : take a snapshot of the current resource list of a specified type(s)
   Parameters : walk the global resource list
   Returns : ptr to snapshot
   Info : 
*/

TBResourceSnapshot *bkTakeResourceSnapshot(uint typeMask)
{
        bkPrintf("*** WARNING *** bkTakeResourceSnapshot was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}


/* --------------------------------------------------------------------------------
   Function : bkDeleteResourceSnapshot
   Purpose : delete a resource snapshot
   Parameters : ptr to snapshot
   Returns : 
   Info : 
*/

void bkDeleteResourceSnapshot(TBResourceSnapshot *snapshot)
{
        bkPrintf("*** WARNING *** bkDeleteResourceSnapshot was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkCompareResourceSnapshot
   Purpose : compare a resource snapshot against the current resource list
   Parameters : snapshot
   Returns : 1=there were differences, 0=they were identical
   Info : 
*/

int bkCompareResourceSnapshot(TBResourceSnapshot *snap)
{
        bkPrintf("*** WARNING *** bkCompareResourceSnapshot was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkIsResource
   Purpose : determine if a pointer is a valid resource
   Parameters : resource type (can be EBRESTYPE_ANY), ptr
   Returns : TRUE if a valid resource, FALSE otherwise
   Info : 
*/

int bkIsResource(EBResourceType resType, void *ptr)
{
        bkPrintf("*** WARNING *** bkIsResource was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}