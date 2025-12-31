// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcPackage.cpp
//   Purpose : package file access functions
// Component : XBOX Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define _BKERNEL_

#include <babel.h>

// ********************************************************************************
// Function Implementations

/* --------------------------------------------------------------------------------
   Function : bkOpenPackage
   Purpose : open a package file
   Parameters : package filename
   Returns : handle to package (ptr to index)
   Info : 
*/

TBPackageIndex *bkOpenPackage(char *filename)
{
    char pathWithExt[256];   // "<filename><.pc>"
    char foundDir[256];      // directory returned by bkOpenFileReadOnlyWithSearch
    TBFileHandle fp;
    TBPackageIndex* index;

    // Build "<filename><ext>"
    sprintf(pathWithExt, "%s%s", filename, BPACKAGE_EXT);

    // Allocate package header
    index = (TBPackageIndex*)MALLOC(sizeof(TBPackageIndex));
    if (!index) {
        int largest = 0;
        int avail   = bkHeapFreeSpace(&largest);
        int wanted  = (int)sizeof(TBPackageIndex);
        int more    = (wanted > avail) ? (wanted - avail) : 0;

        bkPrintf("bkOpenPackage: Out of memory '%s' (%d bytes wanted)\n", filename, wanted);
        bkPrintf("bkOpenPackage: (only %d bytes available: %d more required) ***\n", avail, more);
        return 0;
    }

    // Open with search; on success 'foundDir' holds the directory we matched
    if (!bkOpenFileReadOnlyWithSearch(pathWithExt, &fp, foundDir, sizeof(foundDir), BFILEOPENFLAG_WITHSEARCH)) {
        bkPrintf("bkOpenPackage: Could not open file '%s'\n", pathWithExt);
        bkHeapFree(index);
        return 0;
    }

    // Read the on-disk header straight into the struct
    bkReadFromFile(fp, index, sizeof(TBPackageIndex));

    // Store a human-readable package path: "%s%s%s" (dir, sep, filename)
    sprintf(index->pakFilename, "%s%s%s", foundDir, (foundDir[0] ? "\\" : ""), filename);

    // Keep the file handle in the runtime union
    index->fp = fp;

    // Empty package guard
    if (index->noofFiles == 0) {
        fclose((FILE*)fp);
        bkPrintf("bkOpenPackage: Package is empty! '%s'\n", pathWithExt);
        bkHeapFree(index);
        return 0;
    }

    // Version warning (package built against different Babel build)
    if ((int)index->buildNumber != (int)BBUILDNUMBER) {
        bkPrintf("\nbkOpenPackage: *** WARNING : package '%s' from build %d used with build %d (package should be rebuilt) ***\n\n",
                 pathWithExt, (int)index->buildNumber, (int)BBUILDNUMBER);
    }

    // Clear "loaded" flag (opened-from-disk case)
    index->id.loaded = 0;

    // Seek to file index and load it
    bkSeekFile(index->fp, index->indexOffset * index->pauSize, EHOSTSEEK_SET);

    {
        int bytes = index->noofFiles * (int)sizeof(TBFileIndex);
        index->index = (TBFileIndex*)MALLOC(bytes);
        if (!index->index) {
            int largest = 0;
            int avail   = bkHeapFreeSpace(&largest);
            int more    = (bytes > avail) ? (bytes - avail) : 0;

            fclose((FILE*)index->fp);
            bkHeapFree(index);

            bkPrintf("bkOpenPackage: Out of memory for index '%s' (wanted %d bytes)\n", filename, bytes);
            bkPrintf("bkOpenPackage: (only %d bytes available: %d more required) ***\n", avail, more);
            return 0;
        }
        bkReadFromFile(index->fp, index->index, bytes);
    }

    // Optional tag buffer
    if (index->noofTags != 0) {
        bkSeekFile(index->fp, index->tagOffset * index->pauSize, EHOSTSEEK_SET);

        {
            int tbytes = index->noofTags * (int)sizeof(uint32);
            index->tags = (uint32*)MALLOC(tbytes);
            if (index->tags) {
                bkReadFromFile(index->fp, index->tags, tbytes);
            }
        }
    }

    // Build filename table from the resolved "<filename><ext>"
    bkLoadFilenameTable(index, pathWithExt);

    bkPrintf("Opened package '%s' OK (%d files)\n", filename, index->noofFiles);
    return index;
}


/* --------------------------------------------------------------------------------
   Function : bkLoadPackage
   Purpose : load a package file
   Parameters : parent package index (or NULL), package filename, memory location to load to
   Returns : handle to package (ptr to index)
   Info : 
*/

TBPackageIndex *bkLoadPackage(TBPackageIndex *parentIndex, char *filename, uchar *dataPtr)
{
    char  local_100[256];
    uchar *fileData;
    uint32 fixupFlags;

    sprintf(local_100, "%s%s", filename, BPACKAGE_EXT);

    fileData = bkLoadFile(parentIndex, local_100, dataPtr, NULL, NULL, 0);
    if (fileData == NULL)
    {
        bkPrintf("bkLoadPackage: Could not load file '%s'\n", local_100);
        return NULL;
    }

    fixupFlags = (dataPtr != NULL) ? 0x100u : 0u;
    return bFixupPackage(filename, (int)fixupFlags, fileData);
}


/* --------------------------------------------------------------------------------
   Function : bkFreePackageMemory
   Purpose : free unneeded package memory - all samples on GameCube/PS2 where they are in sound memory
   Parameters : pointer to "package index ptr" - Note that this is updated
   Returns : OK / FAIL (this will fail on the GameCube if the ARAM DMA is still in progress)
   Info : platform specific, GameCube/PS2 will free memory Xbox/PC will not
*/

int bkFreePackageMemory(TBPackageIndex **index)
{
        bkPrintf("*** WARNING *** bkFreePackageMemory was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkClosePackage
   Purpose : close a package file
   Parameters : package index ptr
   Returns : 
   Info : 
*/

void bkClosePackage(TBPackageIndex *indexPtr)
{
    bClosePackage(indexPtr,0);
}