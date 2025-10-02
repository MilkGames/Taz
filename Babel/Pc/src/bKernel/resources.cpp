// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : resources.cpp
//   Purpose : resource management functions
// Component : Generic Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

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
    return;
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
    return 0;
}