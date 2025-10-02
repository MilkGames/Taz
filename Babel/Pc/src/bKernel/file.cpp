// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : file.cpp
//   Purpose : file access functions
// Component : Babel Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/* --------------------------------------------------------------------------------
   Function : bkSetFileSearchPath
   Purpose : set the search path for the file routines
   Parameters : search order flags, number of paths, search paths
   Returns : 
   Info : 
*/

void bkSetFileSearchPath(int flags, int noofPaths, ...)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bInitCRCTable
	Purpose : Initialise the CRC table
	Parameters : 
	Returns :
 	Info : 
*/

void bInitCRCTable()
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkLoadFile
   Purpose : load a file from a package / disc
   Parameters : package index ptr, filename, ptr to buffer (or NULL for dynamic), ptr for returned size (or NULL),
								ptr for tag info (or NULL),
								number of extra bytes to allocate (only for files off disc or from an opened package)
   Returns : ptr to buffer or NULL for failure
   Info : package index = NULL for non-package files
*/

uchar *bkLoadFile(TBPackageIndex *indexPtr, char *filename, uchar *dataPtr, int *retPtr, TBFileTagInfo *tagInfo,
																									int noofExtraBytes)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkLoadFileByCRC
   Purpose : load a file from a package
   Parameters : package index ptr, CRC, ptr to buffer (or NULL for dynamic), ptr for returned size (can be NULL),
				ptr for tag info (or NULL), number of extra bytes to allocate (only for files from an opened package)
   Returns : ptr to buffer or NULL for failure
   Info : 
*/

uchar *bkLoadFileByCRC(TBPackageIndex *index, uint32 crc, uchar *dataPtr, int *retSize, TBFileTagInfo *tagInfo,
																									int noofExtraBytes)
{
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bkCRC32
	Purpose : calculate a 32bit CRC for some data
	Parameters : data ptr, size of data, running total
	Returns : new CRC
	Info :
*/

uint32 bkCRC32(const uchar *data, int size, uint32 accum)
{
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bkStringCRC
	Purpose : calculate a 32bit CRC for a string
	Parameters : string
	Returns : CRC
	Info :
*/

uint32 bkStringCRC(const char *string)
{
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bkStringLwrCRC
	Purpose : calculate a 32bit CRC for a string (forced into lower case)
	Parameters : string
	Returns : CRC
	Info :
*/

uint32 bkStringLwrCRC(const char *data)
{
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bkStringUprCRC
	Purpose : calculate a 32bit CRC for a string (forced into upper case)
	Parameters : string
	Returns : CRC
	Info :
*/

uint32 bkStringUprCRC(const char *data)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bGetPackageIndexEntry
   Purpose : return a ptr to an index entry for a package
   Parameters : package ptr, file ordinal (0..nooffiles-1)
   Returns : ptr to index entry
   Info : 
*/

TBFileIndex *bGetPackageIndexEntry(TBPackageIndex *index, int ordinal)
{
    return NULL;
}


/* --------------------------------------------------------------------------------
	Function : bkGetPakFileTags
	Purpose : return the tags for a file in a package
	Parameters : package ptr, filename, tag info struct
	Returns : OK/FAIL
	Info : 
*/

int bkGetPakFileTags(TBPackageIndex *index, char *filename, TBFileTagInfo *tagInfo)
{
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bkGetPakFileTagsByCRC
	Purpose : return the tags for a file in a package
	Parameters : package ptr, crc of filename, tag info struct.
	Returns : OK/FAIL
	Info : 
*/

int bkGetPakFileTagsByCRC(TBPackageIndex *index, uint32 crc, TBFileTagInfo *tagInfo)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkGetPackageID
   Purpose : return the ID of the package
   Parameters : package ptr
   Returns : ID
   Info : 
*/

TBPackageID bkGetPackageID(TBPackageIndex *index)
{
	TBPackageID id;
    return id;
}


/* --------------------------------------------------------------------------------
   Function : bkEnumPackageContents
   Purpose : enumerate the contents of a package file
   Parameters : pak index, last enum handle (0=first), tag to match (NULL=all), returned file CRC, returned tag info (or NULL)
   Returns : enum handle or NULL for none
   Info : 
*/

int32 bkEnumPackageContents(TBPackageIndex *index, int32 lastHandle, uint32 matchTag, uint32 *crc, TBFileTagInfo *tagInfo)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkOpenFileReadOnlyWithSearch
   Purpose : locate file on disk and provide a handle to it
   Parameters : filename, ptr to file handle, ptr to full path to return, maxlength of path, [BFILESEARCHFLAG_ flags = 0]
   Returns : OK/FAIL
   Info : Full path can be left as NULL if this is not required.
*/

int bkOpenFileReadOnlyWithSearch(char *filename, TBFileHandle *fpPtr, char *fullpath, int maxlen, int flags)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkOpenFileReadOnly
   Purpose : Open a file in read only mode
   Parameters : filename, ptr to handle to set
   Returns : OK/FAIL
   Info : 
*/

int bkOpenFileReadOnly(char *filename, TBFileHandle *fpPtr)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkReadFromFile
   Purpose : read data from a file
   Parameters : file handle, data ptr, #bytes
   Returns : number of bytes read
   Info : 
*/

int bkReadFromFile(TBFileHandle fp, void *data, int noofBytes)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkSeekFile
   Purpose : seek within a file
   Parameters : file handle, position to seek to, mode
   Returns : 
   Info : 
*/

void bkSeekFile(TBFileHandle fp, int position, EBHostSeekMode mode)
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkTellFile
   Purpose : get position of file pointer
   Parameters : file handle
   Returns : position of file pointer
   Info : 
*/

int bkTellFile(TBFileHandle fp)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkCloseFile
   Purpose : close a file
   Parameters : file handle
   Returns : 
   Info : 
*/

void bkCloseFile(TBFileHandle fp)
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkFileLength
   Purpose : get length of file
   Parameters : filename
   Returns : file length
   Info : Platform specific version
*/

int bFileLength(TBFileHandle fp)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkHostOpenFileReadOnly
   Purpose : open a file for reading on the host machine (i.e. the dev machine)
   Parameters : filename, ptr to handle to set
   Returns : OK/FAIL
   Info : 
*/

int bkHostOpenFileReadOnly(char *filename, TBHostFileHandle *fpPtr)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkHostCreateFile
   Purpose : open a file for writing on the host machine (i.e. the dev machine)
   Parameters : filename, ptr to handle to set
   Returns : OK/FAIL
   Info : 
*/

int bkHostCreateFile(char *filename, TBHostFileHandle *fpPtr)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkHostReadFromFile
   Purpose : read data from a host file
   Parameters : file handle, data ptr, #bytes
   Returns : number of bytes read
   Info : 
*/

int bkHostReadFromFile(TBHostFileHandle fp, void *data, int noofBytes)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkHostWriteToFile
   Purpose : write data to a host file
   Parameters : file handle, data ptr, #bytes
   Returns : number of bytes written
   Info : 
*/

int bkHostWriteToFile(TBHostFileHandle fp, void *data, int noofBytes)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkHostSeekFile
   Purpose : seek within a host file
   Parameters : file handle, position to seek to, mode
   Returns : 
   Info : 
*/

void bkHostSeekFile(TBHostFileHandle fp, int position, EBHostSeekMode mode)
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkHostCloseFile
   Purpose : close a host file
   Parameters : file handle
   Returns : 
   Info : 
*/

void bkHostCloseFile(TBHostFileHandle fp)
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bFindIndexFile
   Purpose : locate the position of a file within a package
   Parameters : package index, filename
   Returns : ptr to file index
   Info : 
*/

TBFileIndex *bFindIndexFile(TBPackageIndex *index, char *filename)
{
    return NULL;
}


/* --------------------------------------------------------------------------------
   Function : bFindIndexFileByCRC
   Purpose : locate the position of a file within a package
   Parameters : package index, crc
   Returns : ptr to file index
   Info : 
*/

TBFileIndex *bFindIndexFileByCRC(TBPackageIndex *index, uint32 crc)
{
    return NULL;
}


/* --------------------------------------------------------------------------------
   Function : bkSetLanguage
   Purpose : set the language to use for language specific resources
   Parameters : language ID
   Returns : 
   Info : 
*/

void bkSetLanguage(EBLanguageID languageId)
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bkLoadFilenameTable
	Purpose : Load a table of filenames from the package index.
	Parameters : [in] the index of the package, [in] package filename
	Returns : 1 / 0
	Info : 
*/

int bkLoadFilenameTable(TBPackageIndex *index, char *filename)
{
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bkDeleteFilenameTable
	Purpose : Remove a table of filenames from the package id. (or all filename tables if id == 0)
	Parameters : [in] the id of the package that you wish to remove the filename table for.
	Returns : new ref count or zero for deleted
	Info : 
*/

int bkDeleteFilenameTable(TBPackageID id)
{
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bkFilenameTableAddRef
	Purpose : increment the reference count on a filename table to keep it in memory
	Parameters : the id of the package the filename table refers to
	Returns : new ref count
	Info : 
*/

int bkFilenameTableAddRef(TBPackageID id)
{
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bkFindFilenameByCRC
	Purpose : Find a filenames from the <crc> in the package <id>. (or all filename tables if id == 0)
	Parameters : [in] the id of the package that you wish to find the filename in (can be 0)
				[in] the crc of the filename to look for.
	Returns : The filename associated with CRC or NULL on error.
	Info : 
*/

char *bkFindFilenameByCRC(TBPackageID id, uint32 crc)
{
    return NULL;
}


/* --------------------------------------------------------------------------------
   Function : bkFileLength
   Purpose : get length of file
   Parameters : filename, flags (see BFILEOPENFLAG_*)
   Returns : file length
   Info : 
*/

int bkFileLength(char *filename, int flags)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkFileLength
   Purpose : get length of file within package
   Parameters : package index ptr, filename, flags (see BFILEOPENFLAG_*)
   Returns : file length
   Info : 
*/

int bkFileLength(TBPackageIndex *index, char *filename, int flags)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bLoadSingleFile
   Purpose : load a single file from disc
   Parameters : filename, data ptr (or NULL), ptr for returned size, #extra bytes to allocate
   Returns : ptr to buffer or NULL for failure
   Info : Internal function 
*/

uchar *bLoadSingleFile(char *filename, uchar *dataPtr, int *retSize, int noofExtraBytes)
{
    return NULL;
}


/* --------------------------------------------------------------------------------
   Function : bkFileFindFirst
   Purpose : Prepare for finding files in the specified location
   Parameters : pointer to find file info structure, directory name, returned file name, max filename length
   Returns : OK/FAIL
   Info : 
*/

int bkFileFindFirst(TBFindFileInfo *info, char *dirname, char *filename, int maxlen)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkFileFindNext
   Purpose : Find the next file
   Parameters : pointer to find file info structure, returned file name, max filename length
   Returns : OK/FAIL
   Info : Filename is only valid if function succeeds.
*/

int bkFileFindNext(TBFindFileInfo *info, char *filename, int maxlen)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkFileFindClose
   Purpose : End the find file action
   Parameters : pointer to find file info structure
   Returns : OK/FAIL
   Info : 
*/

int bkFileFindClose(TBFindFileInfo *info)
{
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bkGetSystemLanguage
	Purpose : gets the language setting from the system configuration
	Parameters : 
	Returns : EBKernelSysLangType language type
	Info : 
*/

EBLanguageID bkGetSystemLanguage()
{
    return BLANGUAGEID_UK;
}