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

TBPackageID     bNULLPackageId;

// ********************************************************************************
// Locals

typedef TBResourceInfo* (*TBResLoadFunc)(TBPackageIndex*, uint32);

static TBResLoadFunc bResLoadFunction[BNOOFRESTYPES] = {
    (TBResLoadFunc)bLoadTextureByCRC,
    (TBResLoadFunc)bLoadActorByCRC,
    (TBResLoadFunc)bLoadSampleByCRC,
    (TBResLoadFunc)bLoadFontByCRC,
    (TBResLoadFunc)bLoadStringTableByCRC,
    (TBResLoadFunc)bLoadSplashByCRC,
    (TBResLoadFunc)bLoadSetByCRC,
    (TBResLoadFunc)bLoadCollisionMeshByCRC,
    (TBResLoadFunc)bLoadAnimatingCollisionMeshByCRC,
    (TBResLoadFunc)bLoadAudioStreamByCRC,
    (TBResLoadFunc)bLoadLipSyncByCRC
};

typedef void (*TBResDeleteFunc)(TBResourceInfo* res);

static TBResDeleteFunc bResDeleteFunction[BNOOFRESTYPES] = {
    (TBResDeleteFunc)bDeleteTexture,                 /* 0: TEXR */
    (TBResDeleteFunc)bDeleteActor,                   /* 1: ACTR */
    (TBResDeleteFunc)bDeleteSample,                  /* 2: SAMP */
    (TBResDeleteFunc)bDeleteFont,                    /* 3: FONT */
    (TBResDeleteFunc)bDeleteStringTable,             /* 4: STAB */
    (TBResDeleteFunc)bDeleteSplash,                  /* 5: SPLA */
    (TBResDeleteFunc)bDeleteSet,                     /* 6: SET  */
    (TBResDeleteFunc)bDeleteCollisionMesh,           /* 7: CMES */
    (TBResDeleteFunc)bDeleteAnimatingCollisionMesh,  /* 8: ACOL */
    (TBResDeleteFunc)bDeleteAudioStream,             /* 9: ASTR */
    (TBResDeleteFunc)bDeleteLipSync                  /* 10: LIPS */
};

#define MAKE_TAG4(a,b,c,d) ( ((uint32)(uchar)(a))        | \
                             ((uint32)(uchar)(b) << 8)   | \
                             ((uint32)(uchar)(c) << 16)  | \
                             ((uint32)(uchar)(d) << 24) )

uint32 bResourceTypeTag[11] = {
    MAKE_TAG4('T','E','X','R'),
    MAKE_TAG4('A','C','T','R'),
    MAKE_TAG4('S','A','M','P'),
    MAKE_TAG4('F','O','N','T'),
    MAKE_TAG4('S','T','A','B'),
    MAKE_TAG4('S','P','L','A'),
    MAKE_TAG4('S','E','T',' '),
    MAKE_TAG4('C','M','E','S'),
    MAKE_TAG4('A','C','O','L'),
    MAKE_TAG4('A','S','T','R'),
    MAKE_TAG4('L','I','P','S')
};

const char* bResourceTypeNames[BNOOFRESTYPES - 1] = {
    "Texture",         // 0
    "Actor",           // 1
    "Sample",          // 2
    "Font",            // 3
    "StringTable",     // 4
    "Splash",          // 5
    "Set",             // 6
    "CollisionMesh",   // 7
    "AnimCollisionMesh", // 8
    "AudioStream",     // 9
};

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
    memset(&bGlobalResourceList, 0, sizeof(TBResourceInfo));
    bGlobalResourceList.globalPrev = &bGlobalResourceList;
    bGlobalResourceList.globalNext = &bGlobalResourceList;

    TBResourceInfo* p = bLocalResourceList;
    for (int i = 0; i < BNOOFRESTYPES; ++i, ++p) {
        memset(p, 0, sizeof(TBResourceInfo));
        p->localPrev = p;
        p->localNext = p;
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
    // package id + type
    resPtr->packageId = pakSrc->id;          // copy package ID from source index
    resPtr->type      = (uchar)type;

    // insert into GLOBAL intrusive list (tail-insert)
    resPtr->globalNext = &bGlobalResourceList;
    resPtr->globalPrev = bGlobalResourceList.globalPrev;
    bGlobalResourceList.globalPrev = resPtr;
    resPtr->globalPrev->globalNext = resPtr;

    // insert into LOCAL (per-type) intrusive list (tail-insert)
    {
        TBResourceInfo* root = &bLocalResourceList[type];
        resPtr->localNext = root;
        resPtr->localPrev = root->localPrev;
        root->localPrev   = resPtr;
        resPtr->localPrev->localNext = resPtr;
    }

    // reset runtime context
    resPtr->iContext = 0;

    // optional group override
    if (groupID != BDEFAULTGROUP) {
        resPtr->groupId = (ushort)groupID;
    }
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
    TBResourceInfo* res = bGlobalResourceList.globalNext;
    int idx = 0;

    while (res != &bGlobalResourceList)
    {
        const uchar type = (uchar)(res->type & 0x1F);
        if (typeMask & (1u << type))
        {
            char* fileName = bkFindFilenameByCRC(res->packageId, res->crc);
            if (!fileName) fileName = "<UNKNOWN>";

            char* pakName = bkFindFilenameByCRC(res->packageId, 0);
            if (!pakName) pakName = "<UNKNOWN>";

            // Status by ownership/loaded bit in packageId
            const char* status = res->packageId.loaded ? "Loaded" : "Opened";

            bkPrintf(
                "Resource #%03d, %13s, %s, PAK \"%s\" (0x%08x), group %d, File \"%s\" (0x%08x), context 0x%08x\n",
                idx,
                bResourceTypeNames[type],
                status,
                pakName,
                (uint)res->packageId.crc,
                (uint)res->groupId,
                fileName,
                (uint)res->crc,
                (uint)res->iContext
            );
        }

        res = res->globalNext;
        ++idx;
    }
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
    TBTimerValue start = bkTimerRead();
    int loadedCount = 0;

    if (!pak) {
        TBTimerValue end = bkTimerRead();
        TBTimerValue dt  = bkTimerDelta(end, start);
        ulong ms = (bTimerFrequency ? (ulong)(dt) * 1000UL / (ulong)bTimerFrequency : 0UL);
        bkPrintf("bLoadPackageResources took %d ms\n", (int)ms);
        return 0;
    }

    for (int type = 0; type < BNOOFRESTYPES; ++type) {
        const uint bit = (1u << type);
        if ((typeMask & bit) == 0)
            continue;

        if (pak->noofFiles <= 0)
            continue;

        for (int ordinal = 0; ordinal < pak->noofFiles; ++ordinal) {
            TBFileIndex *fe = bGetPackageIndexEntry(pak, ordinal);
            if (!fe)
                continue;

            const int tagCount = (int)fe->noofTags;
            if (tagCount <= 0)
                continue;

            uint32* tagsBase = (uint32*)((uchar*)pak->tags + (fe->tagOffset & ~3u));

            bool hasTypeTag  = false;
            bool hasMatchTag = (tagMatch == 0);
            for (int t = 0; t < tagCount; ++t) {
                const uint32 tval = tagsBase[t];
                if (tval == bResourceTypeTag[type]) hasTypeTag = true;
                if (tval == tagMatch)                hasMatchTag = true;
            }

            if (!hasTypeTag || !hasMatchTag)
                continue;

            TBResourceInfo *res = bResLoadFunction[type](pak, fe->crc);
            if (!res)
                continue;

            res->crc = fe->crc;
            bAddGlobalResource(res, pak, type, groupId);
            ++loadedCount;
        }
    }

    TBTimerValue end = bkTimerRead();
    TBTimerValue dt  = bkTimerDelta(end, start);
    ulong ms = (bTimerFrequency ? (ulong)(dt) * 1000UL / (ulong)bTimerFrequency : 0UL);
    bkPrintf("bLoadPackageResources took %d ms\n", (int)ms);

    return loadedCount;
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
    uint32 idToMatch;
    memcpy(&idToMatch, &packageId, 4);

    for (;;)
    {
        TBResourceInfo* res = bGlobalResourceList.globalNext;

        if (res == &bGlobalResourceList) {
            return;
        }

        while (res != &bGlobalResourceList)
        {
            const uint32 bit = 1u << (res->type & 31);

            if ((typeMask & bit) &&
                (((res->packageId32 ^ idToMatch) & 0x7FFFFFFFu) == 0))
            {
                res->globalNext->globalPrev = res->globalPrev;
                res->globalPrev->globalNext = res->globalNext;

                res->localNext->localPrev = res->localPrev;
                res->localPrev->localNext = res->localNext;

                bResDeleteFunction[res->type](res);

                break;
            }

            res = res->globalNext;
        }

        if (res == &bGlobalResourceList) {
            return;
        }
    }
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
    for (;;) {
        TBResourceInfo* res = bGlobalResourceList.globalNext;
        if (res == &bGlobalResourceList)
            return;

        while ( ((typeMask & (1u << (res->type & 0x1F))) == 0) ||
                (groupID != 0xFFFF && res->groupId != (ushort)groupID) )
        {
            res = res->globalNext;
            if (res == &bGlobalResourceList)
                return;
        }

        res->globalNext->globalPrev = res->globalPrev;
        res->globalPrev->globalNext = res->globalNext;
        res->localNext->localPrev   = res->localPrev;
        res->localPrev->localNext   = res->localNext;

        bResDeleteFunction[res->type](res);
    }
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
    while (bGlobalResourceList.globalNext != &bGlobalResourceList)
    {
        TBResourceInfo* res = bGlobalResourceList.globalNext;

        // Unlink from global intrusive list
        res->globalNext->globalPrev = res->globalPrev;
        res->globalPrev->globalNext = res->globalNext;

        // Unlink from package-local intrusive list
        res->localNext->localPrev = res->localPrev;
        res->localPrev->localNext = res->localNext;

        // Delete by resource type
        bResDeleteFunction[res->type](res);
    }
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
    TBResourceInfo* const res = (TBResourceInfo*)resPtr;

    // Unlink from global intrusive list
    res->globalNext->globalPrev = res->globalPrev;
    res->globalPrev->globalNext = res->globalNext;

    // Unlink from local intrusive list
    res->localNext->localPrev = res->localPrev;
    res->localPrev->localNext = res->localNext;

    // Delete by resource type (tailcall in asm via jmp)
    bResDeleteFunction[res->type](res);
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
    // name > CRC (+ type-specific fixups)
    uint32 crc = bkStringCRC(filename);
    if (resType == EBRESTYPE_STRINGTABLE) crc = bkFixStringTableCRC(crc);
    else if (resType == EBRESTYPE_SPLASH) crc = bdFixSplashCRC(crc);

    // no package > lookup in memory (local list for this type)
    if (index == 0) {
        TBResourceInfo* it   = bLocalResourceList[resType].localNext;
        TBResourceInfo* root = &bLocalResourceList[resType];
        while (it != root) {
            if (it->crc == crc) return it;
            it = it->localNext;
        }
        // not found in memory
        bkPrintf("bLoadResource: Could not find '%s' in memory\n", filename);
        return 0;
    }

    // load from package via jump table
    TBResLoadFunc fn = bResLoadFunction[resType];
    TBResourceInfo* p = fn(index, crc);
    if (!p) {
        bkPrintf("bLoadResource: Could not load '%s' from '%s'\n", filename, index->pakFilename);
        return 0;
    }

    // stamp CRC before handing off, then let resources core link/stamp everything else
    p->crc = crc;
    bAddGlobalResource(p, index, (int)resType, groupID);
    return p;
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
    if (index == 0) {
        TBResourceInfo* it   = bLocalResourceList[resType].localNext;
        TBResourceInfo* root = &bLocalResourceList[resType];
        while (it != root) {
            if (it->crc == crc) return it;
            it = it->localNext;
        }
        return 0;
    }

    TBResLoadFunc fn = bResLoadFunction[resType];
    TBResourceInfo* p = fn(index, crc);
    if (!p) return 0;

    p->crc = crc;
    bAddGlobalResource(p, index, (int)resType, groupID);
    return p;
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
    const uint32 packageId32 = packageId.crc;

    if ((uint32)resType == (uint32)EBRESTYPE_ANY)
    {
        TBResourceInfo* start = bGlobalResourceList.globalNext;
        TBResourceInfo* it = start;

        // Pass 1: (crc && packageId match, ignoring high bit)
        while (it != &bGlobalResourceList)
        {
            if (it->crc == crc)
            {
                uint32 x = it->packageId32 ^ packageId32;
                if ((x & 0x7fffffffU) == 0)
                    return it;
            }
            it = it->globalNext;
        }

        // if (flags & 1) return NULL
        if ((flags & 1U) != 0)
            return (TBResourceInfo*)0;

        // Pass 2: (crc && groupId match) or (groupId == 0xFFFF => accept any group)
        it = start;
        while (it != &bGlobalResourceList)
        {
            if (it->crc == crc)
            {
                if (groupId == 0xFFFFU)
                    return it;

                if ((uint32)it->groupId == groupId)
                    return it;
            }
            it = it->globalNext;
        }

        // Pass 3: (crc only)
        it = start;
        while (it != &bGlobalResourceList)
        {
            if (it->crc == crc)
                return it;

            it = it->globalNext;
        }

        return (TBResourceInfo*)0;
    }
    else
    {
        // Local list for this resType
        TBResourceInfo* head = &bLocalResourceList[(uint32)resType];
        TBResourceInfo* start = head->localNext;
        TBResourceInfo* it = start;

        // Pass 1: (crc && packageId match, ignoring high bit)
        while (it != head)
        {
            if (it->crc == crc)
            {
                uint32 x = it->packageId32 ^ packageId32;
                if ((x & 0x7fffffffU) == 0)
                    return it;
            }
            it = it->localNext;
        }

        // if (flags & 1) return NULL
        if ((flags & 1U) != 0)
            return (TBResourceInfo*)0;

        // Pass 2: (crc && groupId match) or (groupId == 0xFFFF => accept any group)
        it = start;
        while (it != head)
        {
            if (it->crc == crc)
            {
                if (groupId == 0xFFFFU)
                    return it;

                if ((uint32)it->groupId == groupId)
                    return it;
            }
            it = it->localNext;
        }

        // Pass 3: (crc only)
        it = start;
        while (it != head)
        {
            if (it->crc == crc)
                return it;

            it = it->localNext;
        }

        return (TBResourceInfo*)0;
    }
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
    if (resource == NULL)
    {
        TBResourceInfo* head = &bLocalResourceList[(int)resType];
        TBResourceInfo* next = head->localNext;
        return (next == head) ? NULL : next;
    }

    if ((EBResourceType)resource->type != resType) {
        return NULL;
    }

    TBResourceInfo* head = &bLocalResourceList[(int)resource->type];
    TBResourceInfo* next = resource->localNext;
    return (next == head) ? NULL : next;
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