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
    if (!index) return;

    // Drop filename table (always)
    bkDeleteFilenameTable(index->id);

    // Two paths based on package id sign
    if (index->id.loaded == 0)
    {
        // Regular file-backed package: close the file and free tables
        if (index->fp)
            bkCloseFile(index->fp);

        if (index->index) {
            bkHeapFree(index->index);
            index->index = (TBFileIndex*)0;
        }

        if (index->tags) {
            bkHeapFree(index->tags);
            index->tags = (uint32*)0;
        }
    }
    else
    {
        if ((flags & 1) == 0) {
            bDeletePackageResources(index->id, BALLGROUPS); // MG: todo - confirm it
        }
    }

    if ((index->flags & 1) == 0) { // I'm not too sure what is "1" yet, doesn't seem to be used
        bkHeapFree(index);
    }
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
    char path[256];
    sprintf(path, "%s%s", filename, BPACKAGE_EXT);
    return bkFileLength(path, flags);
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
    char path[256];
    sprintf(path, "%s%s", filename, BPACKAGE_EXT);
    return bkFileLength(index, path, flags);
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
    uchar* base = dataPtr;

    // If KEEPBUFFER flag in resType -> mark buffer as Static (bit0 in byte at +0x08)
    if (resType & 0x100) {
        *(uint*)(base + 0x08) |= 0x01;
    }

    // Warn if package build differs from expected 0x40 (64)
    const int build = *(int*)(base + 0x38);
    if (build != 0x40) {
        bkPrintf(
            "bFixupPackage: *** WARNING : package '%s' from build %d used with build %d (package should be rebuilt) ***\n\n",
            filename, build, 0x40);
    }

    // Fix pointers from offsets using pauSize at +0x04
    const int pauSize = *(int*)(base + 0x04);
    *(uchar**)(base + 0x10) = base + (*(int*)(base + 0x10)) * pauSize;

    // Mark id/flags dword (+0x00) with high bit (RAM/owned)
    *(uint*)(base + 0x00) |= 0x80000000u;

    // If tags present (non-zero at +0x1C), fix their pointer too
    if (*(int*)(base + 0x1C) != 0) {
        *(uchar**)(base + 0x14) = base + (*(int*)(base + 0x14)) * pauSize;
    }

    // Self pointer (as in retail)
    *(uchar**)(base + 0x18) = base;

    // Build filename table
    bkLoadFilenameTable((TBPackageIndex*)base, filename);

    // Log: Static vs Dynamic buffer (bit0 at +0x08)
    const char* bufKind = ( (*(uchar*)(base + 0x08) & 0x01) != 0 ) ? "Static" : "Dynamic";
    bkPrintf("bFixupPackage: Loaded package '%s' OK (%d files, %s buffer)\n",
             filename, *(int*)(base + 0x0C), bufKind);

    // Copy filename into header (+0x40) and strip extension
    char* dst = (char*)(base + 0x40);
    // safe copy
    for (const char* s = filename; (*dst++ = *s++) != '\0'; ) {}
    char* dot = strrchr((char*)(base + 0x40), '.');
    if (dot) *dot = '\0';

    // Empty package handling
    if (*(int*)(base + 0x0C) == 0) {
        if ( (*(uchar*)(base + 0x08) & 0x01) == 0 ) {
            bkHeapFree(base);
        }
        bkPrintf("bFixupPackage: Package is empty! '%s'\n", filename);
        return NULL;
    }

    return (TBPackageIndex*)base;
}
