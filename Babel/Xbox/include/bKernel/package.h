// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : package.h
//   Purpose : package file access functions
// Component : Generic Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BKERNEL_PACKAGE_H__
#define __BKERNEL_PACKAGE_H__


// ********************************************************************************
// Constants and macros

// package flags
#define BPACKAGEFLAGS_STATICBUFFER				0x00000001	// this package has been loaded into a static buffer
#define BPACKAGEFLAGS_SAMPLES_ONLY				0x00000002	// this package contains only samples

// close flags
#define BPACKAGECLOSEFLAGS_KEEPRESOURCES		0x00000001	// Keep all resources on close


// resource types
enum {
	BKGLOADRES_NONE,
	BKGLOADRES_PACKAGE,
	BKGLOADRES_TEXTURE,
	BKGLOADRES_ACTOR,
	BKGLOADRES_SAMPLE,
	BKGLOADRES_FONT,
	BKGLOADRES_STRINGTABLE,
	BKGLOADRES_SPLASH,
	BKGLOADRES_SET,
};

// background load flags (OR'ed with resource type above)
#define BKGLOADFLAG_MASKRESTYPE		0x000000ff
#define BKGLOADFLAG_STATICBUFFER	0x00000100


// ********************************************************************************
// Types, structures and classes


// ********************************************************************************
// Globals


// ********************************************************************************
// Prototypes


/* --------------------------------------------------------------------------------
   Function : bOpenPackage
   Purpose : open a package file
   Parameters : package filename
   Returns : handle to package (ptr to index)
   Info : 
*/

TBPackageIndex *bOpenPackage(char *filename);


/* --------------------------------------------------------------------------------
   Function : bLoadPackage
   Purpose : load a package file
   Parameters : parent package index (or NULL), package filename, memory location to load to
   Returns : handle to package (ptr to index)
   Info : 
*/

TBPackageIndex *bLoadPackage(TBPackageIndex *parentIndex, char *filename, uchar *dataPtr);


/* --------------------------------------------------------------------------------
   Function : bClosePackage
   Purpose : close a package file.  This works for both opened & loaded packages
   Parameters : package index ptr, flags
   Returns : 
   Info : 
*/

void bClosePackage(TBPackageIndex *index, int flags=0);


/* --------------------------------------------------------------------------------
   Function : bkPackageFileLength
   Purpose : get length of package file
   Parameters : package filename (without extension), flags (see BFILEOPENFLAG_*)
   Returns : package file length
   Info : 
*/

int bkPackageFileLength(char *filename, int flags=BFILEOPENFLAG_WITHSEARCH);


/* --------------------------------------------------------------------------------
   Function : bkPackageFileLength
   Purpose : get length of package file
   Parameters : package index ptr, package filename (without extension), flags (see BFILEOPENFLAG_*)
   Returns : package file length
   Info : 
*/

int bkPackageFileLength(TBPackageIndex *index, char *filename, int flags=BFILEOPENFLAG_WITHSEARCH);


/* --------------------------------------------------------------------------------
   Function : bFixupPackage
   Purpose : fixup a package file that has been loaded
   Parameters : Filename, resource type, data address
   Returns : index for the package
   Info : 
*/

TBPackageIndex *bFixupPackage(char *filename, int resType, uchar *dataPtr);


#endif		// __BKERNEL_PACKAGE_H__
