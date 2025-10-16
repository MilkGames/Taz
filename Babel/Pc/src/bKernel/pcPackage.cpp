// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcPackage.cpp
//   Purpose : package file access functions
// Component : XBOX Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
        bkPrintf("*** WARNING *** bkOpenPackage was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
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
        bkPrintf("*** WARNING *** bkLoadPackage was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
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
        bkPrintf("*** WARNING *** bkClosePackage was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}