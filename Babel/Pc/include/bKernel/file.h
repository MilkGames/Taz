// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : file.h
//   Purpose : file access functions
// Component : Generic Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BKERNEL_FILE_H__
#define __BKERNEL_FILE_H__


// ********************************************************************************
// Constants and macros

// file flags
#define BFILEOPENFLAG_WITHSEARCH				0x00000001	// use search rather than basic path

// file read search flags
#define BFILESEARCHFLAG_RETURNDIRECTORY			0x00000001	// when opening file with search and returning full path, return directory
															// in which the file was found

// macro to make a uint32 four-character-code value from a string
#define FOURCC(TAG)								(*((uint32*)(TAG)))

// Sizes for filesearch paths
#define FILE_SEARCHPATH_MAXNO					4

#define FILE_SEARCHPATHFLAGS_DEFAULTPATHFIRST	0	// normal location before paths (default)
#define FILE_SEARCHPATHFLAGS_DEFAULTPATHLAST	1	// normal location after paths
#define FILE_SEARCHPATHFLAGS_DEFAULTPATHIGNORE	2	// Ignore normal location

// host file seek modes
typedef enum EBHostSeekMode {
	EHOSTSEEK_SET,
	EHOSTSEEK_CUR,
	EHOSTSEEK_END
};


// language IDs
typedef enum {
	BLANGUAGEID_UK,			// UK Engligh
	BLANGUAGEID_F,			// French
	BLANGUAGEID_D,			// German
	BLANGUAGEID_E,			// Spanish
	BLANGUAGEID_IT,			// Italian
	BLANGUAGEID_NL,			// Dutch
	BLANGUAGEID_SW,			// Swedish
	BLANGUAGEID_FIN,		// Finish
	BLANGUAGEID_N,			// Norwegian
	BLANGUAGEID_DK,			// Danish
	BLANGUAGEID_US,			// US English
	BLANGUAGEID_JP,			// Japanese
	BLANGUAGEID_PG,			// Portugese
	BLANGUAGEID_BR,			// Brazilian

	BLANGUAGEID_UNKNOWN,	// *Unknown* Added for the bkGetSystemLanguage() function
} EBLanguageID;


// ********************************************************************************
// Types, structures and classes

// ID structure for packages
typedef struct _TBPackageID {
	uint32					crc : 31;				// CRC of package
	uint32					loaded : 1;				// set if from a 'loaded' package, clear if from an 'opened' package
} TBPackageID;


// standard index block for a file within a package
typedef struct _TBFileIndex {
	int32					offset;					// offset to file data in bytes from the start of the file
	uint32					crc;					// CRC for filename relative to project root (includes extension, in lower case)
	int32					size;					// size of the file in bytes
	uint32					filenameOffset;			// offset to filename

	uint32					noofTags;				// number of tags for this file (4 bytes each)
	uint32					tagOffset;				// offset from start of format tag block to tags for this file
	uint64					fileTime;				// filetime for incremental builds
} TBFileIndex;


// an index structure for a package file
typedef struct _TBPackageIndex {
	TBPackageID				id;						// package ID
	uint32					pauSize;				// size of a Package Allocation Unit (i.e. granularity)
	uint32					flags;					// package flags (see PAKFLAG_)
	int32					noofFiles;				// number of files in index

	union {
		TBFileIndex			*index;					// ptr to index records (BABEL)
		uint32				indexOffset;			// offset to the file index in PAUs (PACKAGER)
	};
	union {
		uint32				*tags;					// tag buffer (BABEL)
		uint32				tagOffset;				// offset to the tag buffer in PAUs (PACKAGER)
	};
	union {
		TBFileHandle		fp;						// file pointer for this package (BABEL)
		uchar				*data;					// data for RAM packages (BABEL)
	};
	int32					noofTags;				// number of tags in the tag buffer

	int32					blockMapOffset;			// offset to block-map in PAUs
	int32					blockMapSize;			// number of entries in block-map
	uint32					filenameTableOffset;	// offset to filename table in PAUs
	uint32					filenameTableSize;		// size of filename table in bytes

	uint32					indexSize;				// size of index table in bytes
	uint32					startSector;			// starting sector on disc
	uint32					buildNumber;			// babel build number this package was built for
	uint32					noofFilesUsingDMA;		// number of files still being DMA'ed somewhere (used for GameCube ARAM transfer indication)

	char					pakFilename[256];		// package filename
} TBPackageIndex;


// tag information for a file
typedef struct _TBFileTagInfo {
	uint32					*tags;					// ptr to tags
	int32					noofTags;				// #tags for this file (can be zero)
} TBFileTagInfo;


// filename table header
typedef struct _TBFilenameTableHeader
{
	TBPackageID						package;		// package ID
	struct _TBFilenameTableHeader	*prev, *next;	// list links
	int								refCount;		// reference count for use
	uint32							noofFiles;		// #files in filename table
	char							filename[256];	// filename of package
} TBFilenameTableHeader;


// ********************************************************************************
// Globals

// CRC table
extern uint32	bCRCtable[256];

// global language ID
extern EBLanguageID bLanguage;

// path to search for files if not found in base location
extern char		*bFileSearchPath[FILE_SEARCHPATH_MAXNO];
extern int		bFileSearchPaths;


// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bkSetFileSearchPath
   Purpose : set the search path for the file routines
   Parameters : search order flags, number of paths, search paths
   Returns : 
   Info : 
*/

void bkSetFileSearchPath(int flags, int noofPaths, ...);

/*	--------------------------------------------------------------------------------
	Function : bInitCRCTable
	Purpose : Initialise the CRC table
	Parameters : 
	Returns :
 	Info : 
*/

void bInitCRCTable();


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
																									int noofExtraBytes);


/* --------------------------------------------------------------------------------
   Function : bkLoadFileByCRC
   Purpose : load a file from a package
   Parameters : package index ptr, CRC, ptr to buffer (or NULL for dynamic), ptr for returned size (can be NULL),
				ptr for tag info (or NULL), number of extra bytes to allocate (only for files from an opened package)
   Returns : ptr to buffer or NULL for failure
   Info : 
*/

uchar *bkLoadFileByCRC(TBPackageIndex *index, uint32 crc, uchar *dataPtr, int *retSize, TBFileTagInfo *tagInfo,
																									int noofExtraBytes);


/*	--------------------------------------------------------------------------------
	Function : bkCRC32
	Purpose : calculate a 32bit CRC for some data
	Parameters : data ptr, size of data, running total
	Returns : new CRC
	Info :
*/

uint32 bkCRC32(const uchar *data, int size, uint32 accum);


/*	--------------------------------------------------------------------------------
	Function : bkStringCRC
	Purpose : calculate a 32bit CRC for a string
	Parameters : string
	Returns : CRC
	Info :
*/

uint32 bkStringCRC(const char *string);


/*	--------------------------------------------------------------------------------
	Function : bkStringLwrCRC
	Purpose : calculate a 32bit CRC for a string (forced into lower case)
	Parameters : string
	Returns : CRC
	Info :
*/

uint32 bkStringLwrCRC(const char *data);


/*	--------------------------------------------------------------------------------
	Function : bkStringUprCRC
	Purpose : calculate a 32bit CRC for a string (forced into upper case)
	Parameters : string
	Returns : CRC
	Info :
*/

uint32 bkStringUprCRC(const char *data);


/* --------------------------------------------------------------------------------
   Function : bGetPackageIndexEntry
   Purpose : return a ptr to an index entry for a package
   Parameters : package ptr, file ordinal (0..nooffiles-1)
   Returns : ptr to index entry
   Info : 
*/

TBFileIndex *bGetPackageIndexEntry(TBPackageIndex *index, int ordinal);


/* --------------------------------------------------------------------------------
	Function : bkGetPakFileTags
	Purpose : return the tags for a file in a package
	Parameters : package ptr, filename, tag info struct
	Returns : OK/FAIL
	Info : 
*/

int bkGetPakFileTags(TBPackageIndex *index, char *filename, TBFileTagInfo *tagInfo);


/*	--------------------------------------------------------------------------------
	Function : bkGetPakFileTagsByCRC
	Purpose : return the tags for a file in a package
	Parameters : package ptr, crc of filename, tag info struct.
	Returns : OK/FAIL
	Info : 
*/

int bkGetPakFileTagsByCRC(TBPackageIndex *index, uint32 crc, TBFileTagInfo *tagInfo);


/* --------------------------------------------------------------------------------
   Function : bkGetPackageID
   Purpose : return the ID of the package
   Parameters : package ptr
   Returns : ID
   Info : 
*/

TBPackageID bkGetPackageID(TBPackageIndex *index);


/* --------------------------------------------------------------------------------
   Function : bkEnumPackageContents
   Purpose : enumerate the contents of a package file
   Parameters : pak index, last enum handle (0=first), tag to match (NULL=all), returned file CRC, returned tag info (or NULL)
   Returns : enum handle or NULL for none
   Info : 
*/

int32 bkEnumPackageContents(TBPackageIndex *index, int32 lastHandle, uint32 matchTag, uint32 *crc, TBFileTagInfo *tagInfo);


/* --------------------------------------------------------------------------------
   Function : bkOpenFileReadOnlyWithSearch
   Purpose : locate file on disk and provide a handle to it
   Parameters : filename, ptr to file handle, ptr to full path to return, maxlength of path, [BFILESEARCHFLAG_ flags = 0]
   Returns : OK/FAIL
   Info : Full path can be left as NULL if this is not required.
*/

int bkOpenFileReadOnlyWithSearch(char *filename, TBFileHandle *fpPtr, char *fullpath, int maxlen, int flags = 0);


/* --------------------------------------------------------------------------------
   Function : bkOpenFileReadOnly
   Purpose : Open a file in read only mode
   Parameters : filename, ptr to handle to set
   Returns : OK/FAIL
   Info : 
*/

int bkOpenFileReadOnly(char *filename, TBFileHandle *fpPtr);


/* --------------------------------------------------------------------------------
   Function : bkReadFromFile
   Purpose : read data from a file
   Parameters : file handle, data ptr, #bytes
   Returns : number of bytes read
   Info : 
*/

int bkReadFromFile(TBFileHandle fp, void *data, int noofBytes);


/* --------------------------------------------------------------------------------
   Function : bkSeekFile
   Purpose : seek within a file
   Parameters : file handle, position to seek to, mode
   Returns : 
   Info : 
*/

void bkSeekFile(TBFileHandle fp, int position, EBHostSeekMode mode);


/* --------------------------------------------------------------------------------
   Function : bkTellFile
   Purpose : get position of file pointer
   Parameters : file handle
   Returns : position of file pointer
   Info : 
*/

int bkTellFile(TBFileHandle fp);


/* --------------------------------------------------------------------------------
   Function : bkCloseFile
   Purpose : close a file
   Parameters : file handle
   Returns : 
   Info : 
*/

void bkCloseFile(TBFileHandle fp);


/* --------------------------------------------------------------------------------
   Function : bkFileLength
   Purpose : get length of file
   Parameters : filename
   Returns : file length
   Info : Platform specific version
*/

int bFileLength(TBFileHandle fp);


/* --------------------------------------------------------------------------------
   Function : bkHostOpenFileReadOnly
   Purpose : open a file for reading on the host machine (i.e. the dev machine)
   Parameters : filename, ptr to handle to set
   Returns : OK/FAIL
   Info : 
*/

int bkHostOpenFileReadOnly(char *filename, TBHostFileHandle *fpPtr);


/* --------------------------------------------------------------------------------
   Function : bkHostCreateFile
   Purpose : open a file for writing on the host machine (i.e. the dev machine)
   Parameters : filename, ptr to handle to set
   Returns : OK/FAIL
   Info : 
*/

int bkHostCreateFile(char *filename, TBHostFileHandle *fpPtr);


/* --------------------------------------------------------------------------------
   Function : bkHostReadFromFile
   Purpose : read data from a host file
   Parameters : file handle, data ptr, #bytes
   Returns : number of bytes read
   Info : 
*/

int bkHostReadFromFile(TBHostFileHandle fp, void *data, int noofBytes);


/* --------------------------------------------------------------------------------
   Function : bkHostWriteToFile
   Purpose : write data to a host file
   Parameters : file handle, data ptr, #bytes
   Returns : number of bytes written
   Info : 
*/

int bkHostWriteToFile(TBHostFileHandle fp, void *data, int noofBytes);


/* --------------------------------------------------------------------------------
   Function : bkHostSeekFile
   Purpose : seek within a host file
   Parameters : file handle, position to seek to, mode
   Returns : 
   Info : 
*/

void bkHostSeekFile(TBHostFileHandle fp, int position, EBHostSeekMode mode);


/* --------------------------------------------------------------------------------
   Function : bkHostCloseFile
   Purpose : close a host file
   Parameters : file handle
   Returns : 
   Info : 
*/

void bkHostCloseFile(TBHostFileHandle fp);


/* --------------------------------------------------------------------------------
   Function : bFindIndexFile
   Purpose : locate the position of a file within a package
   Parameters : package index, filename
   Returns : ptr to file index
   Info : 
*/

TBFileIndex *bFindIndexFile(TBPackageIndex *index, char *filename);


/* --------------------------------------------------------------------------------
   Function : bFindIndexFileByCRC
   Purpose : locate the position of a file within a package
   Parameters : package index, crc
   Returns : ptr to file index
   Info : 
*/

TBFileIndex *bFindIndexFileByCRC(TBPackageIndex *index, uint32 crc);


/* --------------------------------------------------------------------------------
   Function : bkSetLanguage
   Purpose : set the language to use for language specific resources
   Parameters : language ID
   Returns : 
   Info : 
*/

void bkSetLanguage(EBLanguageID languageId);


/*	--------------------------------------------------------------------------------
	Function : bkLoadFilenameTable
	Purpose : Load a table of filenames from the package index.
	Parameters : [in] the index of the package, [in] package filename
	Returns : 1 / 0
	Info : 
*/

int bkLoadFilenameTable(TBPackageIndex *index, char *filename);


/*	--------------------------------------------------------------------------------
	Function : bkDeleteFilenameTable
	Purpose : Remove a table of filenames from the package id. (or all filename tables if id == 0)
	Parameters : [in] the id of the package that you wish to remove the filename table for.
	Returns : new ref count or zero for deleted
	Info : 
*/

int bkDeleteFilenameTable(TBPackageID id);


/*	--------------------------------------------------------------------------------
	Function : bkFilenameTableAddRef
	Purpose : increment the reference count on a filename table to keep it in memory
	Parameters : the id of the package the filename table refers to
	Returns : new ref count
	Info : 
*/

int bkFilenameTableAddRef(TBPackageID id);


/*	--------------------------------------------------------------------------------
	Function : bkFindFilenameByCRC
	Purpose : Find a filenames from the <crc> in the package <id>. (or all filename tables if id == 0)
	Parameters : [in] the id of the package that you wish to find the filename in (can be 0)
				[in] the crc of the filename to look for.
	Returns : The filename associated with CRC or NULL on error.
	Info : 
*/

char *bkFindFilenameByCRC(TBPackageID id, uint32 crc);


/* --------------------------------------------------------------------------------
   Function : bkFileLength
   Purpose : get length of file
   Parameters : filename, flags (see BFILEOPENFLAG_*)
   Returns : file length
   Info : 
*/

int bkFileLength(char *filename, int flags=BFILEOPENFLAG_WITHSEARCH);


/* --------------------------------------------------------------------------------
   Function : bkFileLength
   Purpose : get length of file within package
   Parameters : package index ptr, filename, flags (see BFILEOPENFLAG_*)
   Returns : file length
   Info : 
*/

int bkFileLength(TBPackageIndex *index, char *filename, int flags=BFILEOPENFLAG_WITHSEARCH);


/* --------------------------------------------------------------------------------
   Function : bLoadSingleFile
   Purpose : load a single file from disc
   Parameters : filename, data ptr (or NULL), ptr for returned size, #extra bytes to allocate
   Returns : ptr to buffer or NULL for failure
   Info : Internal function 
*/

uchar *bLoadSingleFile(char *filename, uchar *dataPtr, int *retSize, int noofExtraBytes);


/* --------------------------------------------------------------------------------
   Function : bkFileFindFirst
   Purpose : Prepare for finding files in the specified location
   Parameters : pointer to find file info structure, directory name, returned file name, max filename length
   Returns : OK/FAIL
   Info : 
*/

int bkFileFindFirst(TBFindFileInfo *info, char *dirname, char *filename, int maxlen);


/* --------------------------------------------------------------------------------
   Function : bkFileFindNext
   Purpose : Find the next file
   Parameters : pointer to find file info structure, returned file name, max filename length
   Returns : OK/FAIL
   Info : Filename is only valid if function succeeds.
*/

int bkFileFindNext(TBFindFileInfo *info, char *filename, int maxlen);


/* --------------------------------------------------------------------------------
   Function : bkFileFindClose
   Purpose : End the find file action
   Parameters : pointer to find file info structure
   Returns : OK/FAIL
   Info : 
*/

int bkFileFindClose(TBFindFileInfo *info);


/*	--------------------------------------------------------------------------------
	Function : bkGetSystemLanguage
	Purpose : gets the language setting from the system configuration
	Parameters : 
	Returns : EBKernelSysLangType language type
	Info : 
*/

EBLanguageID bkGetSystemLanguage();


#endif		// __BKERNEL_FILE_H__
