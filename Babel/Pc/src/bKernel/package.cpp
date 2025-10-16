// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : package.cpp
//   Purpose : package file access functions
// Component : Babel Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/* --------------------------------------------------------------------------------
   Function : bOpenPackage
   Purpose : open a package file
   Parameters : package filename
   Returns : handle to package (ptr to index)
   Info : 
*/

TBPackageIndex *bOpenPackage(char *filename)
{
        bkPrintf("*** WARNING *** bOpenPackage was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}


/* --------------------------------------------------------------------------------
   Function : bLoadPackage
   Purpose : load a package file
   Parameters : parent package index (or NULL), package filename, memory location to load to
   Returns : handle to package (ptr to index)
   Info : 
*/

TBPackageIndex *bLoadPackage(TBPackageIndex *parentIndex, char *filename, uchar *dataPtr)
{
        bkPrintf("*** WARNING *** bLoadPackage was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}


/* --------------------------------------------------------------------------------
   Function : bClosePackage
   Purpose : close a package file.  This works for both opened & loaded packages
   Parameters : package index ptr, flags
   Returns : 
   Info : 
*/

void bClosePackage(TBPackageIndex *index, int flags)
{
        bkPrintf("*** WARNING *** bClosePackage was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkPackageFileLength
   Purpose : get length of package file
   Parameters : package filename (without extension), flags (see BFILEOPENFLAG_*)
   Returns : package file length
   Info : 
*/

int bkPackageFileLength(char *filename, int flags)
{
        bkPrintf("*** WARNING *** bkPackageFileLength was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkPackageFileLength
   Purpose : get length of package file
   Parameters : package index ptr, package filename (without extension), flags (see BFILEOPENFLAG_*)
   Returns : package file length
   Info : 
*/

int bkPackageFileLength(TBPackageIndex *index, char *filename, int flags)
{
        bkPrintf("*** WARNING *** bkPackageFileLength was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bFixupPackage
   Purpose : fixup a package file that has been loaded
   Parameters : Filename, resource type, data address
   Returns : index for the package
   Info : 
*/

TBPackageIndex *bFixupPackage(char *filename, int resType, uchar *dataPtr)
{
        bkPrintf("*** WARNING *** bFixupPackage was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}
