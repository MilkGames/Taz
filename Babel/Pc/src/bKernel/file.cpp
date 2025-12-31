// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : file.cpp
//   Purpose : file access functions
// Component : Babel Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Globals

uint32       bCRCtable[256];
EBLanguageID bLanguage;
char        *bFileSearchPath[FILE_SEARCHPATH_MAXNO];
int	         bFileSearchPaths;

// ********************************************************************************
// Locals

int					  bFileSearchFlags;
TBFilenameTableHeader bFilenameTable = {{0,0}, &bFilenameTable, &bFilenameTable, 0, 0, {0}};

// ********************************************************************************
// Helper Functions

static uchar* bEnsureAlloc(uchar* dataPtr, int needBytes)
{
    if (dataPtr == NULL) {
        char* group = (char*)bGetCurrentGroup();
        if (group == (char*)BDEFAULTGROUP) {
            group = "Package";
        }

        uchar* p = (uchar*)MALLOCEX((uint)needBytes,(uint32)group);

        if (p == NULL) {
            bkPrintf("EnsureAlloc: *** Out of memory on Babel heap for file (need %d bytes) ***\n",
                     needBytes);

            int maxBlock = 0;
            int available = bkHeapFreeSpace(&maxBlock);
            bkPrintf("EnsureAlloc: (only %d bytes available: %d more required, max %d bytes (%d short)) ***\n",
                     available,
                     needBytes - available,
                     maxBlock,
                     needBytes - maxBlock);
            return NULL;
        }
        return p;
    }

    if (bkHeapGetBlockSize(dataPtr) >= needBytes) {
        return dataPtr;
    }

    return NULL;
}

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
    // Store flags first
    bFileSearchFlags = flags;

    // Publish the count exactly as passed
    bFileSearchPaths = noofPaths;

    // Fill bFileSearchPath[0..noofPaths-1] from varargs (each arg is char*)
    if (noofPaths > 0) {
        va_list ap;
        va_start(ap, noofPaths);
        for (int i = 0; i < noofPaths; ++i) {
            bFileSearchPath[i] = va_arg(ap, char*);
        }
        va_end(ap);
    }
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
    for (int i = 0; i < 256; ++i) {
        uint32 u = ((uint32)i) << 24;
        for (int k = 0; k < 8; ++k) {
            if (u & 0x80000000u) u = (u << 1) ^ 0x04C11DB7u;
            else u <<= 1;
        }
        bCRCtable[i] = u;
    }

#ifdef PRINT_CRCTABLE
	bkPrintf("Printing CRC table contents...\n");
    bkPrintf("------------------------------\n");

    for (int y = 0; y < 256; ++y) {
        // Print the index and the hexadecimal value of each element
        bkPrintf("bCRCtable[%d] = 0x%08X\n", y, bCRCtable[y]);
    }

    bkPrintf("------------------------------\n");
#endif
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
	uchar *result;
	uint32 crc;

	if (indexPtr == NULL) {
		result = bLoadSingleFile(filename,dataPtr,retPtr,noofExtraBytes);
		return result;
	}
	crc = bkStringCRC(filename);
	result = bkLoadFileByCRC(indexPtr,crc,dataPtr,retPtr,tagInfo,noofExtraBytes);
	return result;
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
    // ----- Binary search by CRC over the index table (sorted ascending). -----
    TBFileIndex *base = index->index;         // ptr to TBFileIndex records
    int lo = 0;
    int hi = index->noofFiles - 1;
    TBFileIndex *fe = NULL;

    while (lo <= hi) {
        int mid = (lo + hi) >> 1;
        TBFileIndex *cur = base + mid;
        uint32 c = cur->crc;

        if (c == crc) {
            fe = cur;
            break;
        }
        if (c < crc) lo = mid + 1;
        else         hi = mid - 1;
    }

    if (fe == NULL) {
        bkPrintf("bkLoadFile: Could not find 0x%08x in package\n", crc);
        return NULL;
    }

    // ----- Return size and tag-info if requested. -----
    if (retSize) {
        *retSize = fe->size;
    }
    if (tagInfo) {
        // Tag pointer base is index->tags; align fe->tagOffset down to dword
        tagInfo->tags     = (uint32 *)((uchar*)index->tags + (fe->tagOffset & ~3));
        tagInfo->noofTags = fe->noofTags;
    }

    // Total required bytes in the destination buffer (payload + caller's extra tail)
    const int need = fe->size + noofExtraBytes;

    // ----- Opened (disk) vs Loaded (RAM) package. -----
    // Per header: 'loaded' is 1 for RAM-backed packages.
    if (!index->id.loaded) {
        // OPENED: allocate (or validate) and read from file handle.
        uchar *out = bEnsureAlloc(dataPtr, need);
        if (!out)
            return NULL;

        // Seek to start-of-file + pauSize*offset, then read exactly 'size' bytes
        bkSeekFile(index->fp, index->pauSize * fe->offset, EHOSTSEEK_SET);
        bkReadFromFile(index->fp, out, fe->size);
        return out;
    }

    // LOADED: either return direct pointer, or copy into user buffer
    if (dataPtr == NULL) {
        // Direct pointer into RAM-mapped package blob
        return (uchar *)(index->data + index->pauSize * fe->offset);
    }

    // Supplied a buffer: ensure capacity then copy
    if (bkHeapGetBlockSize(dataPtr) < need) {
        return NULL;
    }

    {
        const uchar *src = (const uchar *)(index->data + index->pauSize * fe->offset);
        memcpy(dataPtr, src, fe->size);
    }
    return dataPtr;
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
    while (size-- > 0) {
        accum = (accum << 8) ^ bCRCtable[((accum >> 24) ^ *data) & 0xFF];
        ++data;
    }
    return accum;
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
    uint32 crc = 0;
    while (*string) {
        unsigned char b = (unsigned char)*string++;
        crc = (crc << 8) ^ bCRCtable[((crc >> 24) ^ b) & 0xFF];
    }
    return crc;
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
        bkPrintf("*** WARNING *** bkStringLwrCRC was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bkStringUprCRC was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
    return index->index + ordinal;
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
        bkPrintf("*** WARNING *** bkGetPakFileTags was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bkGetPakFileTagsByCRC was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
		bkPrintf("*** WARNING *** bkGetPackageID was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bkEnumPackageContents was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
    char srcName[256];   // local copy of the input filename
    char pathBuild[256]; // intermediate sprintf("%s%s" / "%s%s%s")
    char openPath[256];  // final path we actually try to open
    char searchDir[256]; // current directory from the search-path list
    int  ok;

    // Exact local copy (the binary uses an explicit byte copy)
    strcpy(srcName, filename);

    // When WITHSEARCH flag is set and caller passed 'fullpath',
    // it zeroes the first byte early.
    if (fullpath && (flags & BFILEOPENFLAG_WITHSEARCH))
        *fullpath = '\0';

    // Absolute path with drive: "C:\..."
    if (srcName[1] == ':') {
        sprintf(pathBuild, "%s%s", srcName, BFILE_SEARCH_SUFFIX);
        strcpy(openPath, pathBuild);
        return bkOpenFileReadOnly(openPath, fpPtr);
    }

    // Absolute without drive: "\..."
    if (srcName[0] == '\\') {
        sprintf(pathBuild, "%s%s%s", bHomeDirectory, srcName, BFILE_SEARCH_SUFFIX);
        strcpy(openPath, pathBuild);
        ok = bkOpenFileReadOnly(openPath, fpPtr);
        if (ok == 1) goto FillFullpathReturn;
    } else {
        // Relative path, first shot depends on bFileSearchFlags:
        //   flags == 0 > try HomeDir+filename first
        if (bFileSearchFlags == 0) {
            sprintf(pathBuild, "%s%s%s", bHomeDirectory, srcName, BFILE_SEARCH_SUFFIX);
            strcpy(openPath, pathBuild);
            ok = bkOpenFileReadOnly(openPath, fpPtr);
            if (ok == 1) goto FillFullpathReturn;
        }

        // Iterate configured search paths (if any)
        if (bFileSearchPaths > 0) {
            for (int i = 0; i < bFileSearchPaths; ++i) {
                // Copy this entry into a local working buffer
                strcpy(searchDir, bFileSearchPath[i]);

                if (searchDir[1] == ':') {
                    // Has drive letter > "<SearchDir>\<filename><SUFFIX>"
                    sprintf(pathBuild, "%s\\%s%s", searchDir, srcName, BFILE_SEARCH_SUFFIX);
                } else {
                    // Relative search dir > "<HomeDir><SearchDir>\<filename><SUFFIX>"
                    sprintf(pathBuild, "%s%s\\%s%s", bHomeDirectory, searchDir, srcName, BFILE_SEARCH_SUFFIX);
                }

                strcpy(openPath, pathBuild);
                ok = bkOpenFileReadOnly(openPath, fpPtr);
                if (ok == 1) {
                    // On success via search list AND WITHSEARCH flag: return the matched directory only
                    if (fullpath && (flags & BFILEOPENFLAG_WITHSEARCH)) {
                        strncat(fullpath, searchDir, maxlen);
                    }
                    goto FillFullpathReturn;
                }
            }
        }

        // If bFileSearchFlags == 1 > last-chance attempt: "<HomeDir><filename><SUFFIX>"
        if (bFileSearchFlags == 1) {
            sprintf(pathBuild, "%s%s%s", bHomeDirectory, srcName, BFILE_SEARCH_SUFFIX);
            strcpy(openPath, pathBuild);
            ok = bkOpenFileReadOnly(openPath, fpPtr);
            if (ok != 1) return ok; // pass-through (usually 0)
            goto FillFullpathReturn;
        }
    }

    // Not found anywhere
    return FAIL;

FillFullpathReturn:
    // If caller requested full resolved path (not search-dir only), provide it.
    if (fullpath && !(flags & BFILEOPENFLAG_WITHSEARCH)) {
        *fullpath = '\0';
        strncat(fullpath, openPath, maxlen);
    }
    return OK;
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
    TBFileHandle f = fopen(filename, "rb");
    if (!f) return FAIL;

    *fpPtr = f;
    return TRUE;
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
    return fread(data,1,noofBytes,fp);
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
	if (mode == EHOSTSEEK_SET) {
		fseek(fp,position,0);
		return;
	}
	if (mode != EHOSTSEEK_CUR) {
		if (mode == EHOSTSEEK_END) {
			fseek(fp,position,2);
			return;
		}
		return;
	}
	fseek(fp,position,1);
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
        bkPrintf("*** WARNING *** bkTellFile was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
    fclose((FILE*)fp);
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
    long cur = ftell(fp);               // save current position
    fseek(fp, 0, SEEK_END);             // go to end
    long end = ftell(fp);               // length
    fseek(fp, cur, SEEK_SET);           // restore position

    return (int)end;
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
        bkPrintf("*** WARNING *** bkHostOpenFileReadOnly was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bkHostCreateFile was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bkHostReadFromFile was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bkHostWriteToFile was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bkHostSeekFile was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bkHostCloseFile was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bFindIndexFile was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bFindIndexFileByCRC was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
	bLanguage = languageId;
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
    // No filename table present in this package, nothing to do
    if (index->filenameTableOffset == 0) {
        return 0;
    }

    // Check if a filename table for this package is already cached.
    // The bitfield `id.crc` correctly compares only the 31-bit ID,
    // ignoring the `id.loaded` flag, which is exactly what the original
    // `& 0x7fffffff` mask did.
	if (bFilenameTable.next != &bFilenameTable) {
		TBFilenameTableHeader* node = bFilenameTable.next;
		while (node != &bFilenameTable) {
			if (node->package.crc == index->id.crc) {
				// Found it. Just increment the reference count and exit.
				node->refCount++;
				return 0;
			}
			node = node->next;
		}
	}

    // The table is not cached. We need to load it.
    // Allocate a single block for the header, the (crc, offset) pairs, and the raw name data.
    const uint32 pairsSize = (uint32)index->noofFiles * 8u;
    const uint32 totalSize = 0x114u + pairsSize + index->filenameTableSize;

    TBFilenameTableHeader* newNode =
        (TBFilenameTableHeader*)MALLOCEX(totalSize, (uint32)"Filename Table");

    if (!newNode) {
        bkPrintf("bkLoadFilenameTable: Out of memory\n");
        return 0;
    }

    // 1. Fill in the header fields.
    newNode->package   = index->id;
    newNode->noofFiles = (uint32)index->noofFiles;
    newNode->refCount  = 1;

    // 2. Link the new node at the end of the global doubly-linked list.
    newNode->prev = bFilenameTable.prev;
    newNode->next = &bFilenameTable;
    bFilenameTable.prev->next = newNode;
    bFilenameTable.prev       = newNode;

    // 3. Copy the package filename string into the header (at offset +0x14).
    // This is a simple strcpy.
    char* d = newNode->filename;
    const char* s = filename;
    while ((*d++ = *s++) != '\0') {/* empty */}

    // 4. Build the table of (crc, nameOffset) pairs. This table starts at offset +0x114.
    uint32* out_pairs = (uint32*)((char*)newNode + 0x114);
    TBFileIndex* file_indices = index->index;
    for (int i = 0; i < index->noofFiles; ++i) {
        *out_pairs++ = file_indices[i].crc;
        *out_pairs++ = file_indices[i].filenameOffset;
    }

    // 5. Copy the raw filename data. It goes right after the pairs table.
    char* dstNames = (char*)out_pairs; // Continue from where the pairs ended

    // The `id.loaded` bitfield cleanly replaces the original `(int)id < 0` check.
    if (index->id.loaded) {
        // This is a memory-resident package. Copy data from the RAM buffer.
        // We use `index->data`, as it's the designated pointer for RAM packages.
        // The Ghidra output using `index->pakFilename - 0x40` is considered a
        // decompiler error, as it makes no logical sense.
        const char* srcNames = (const char*)index->data + (index->pauSize * index->filenameTableOffset);

        // This optimized copy (dwords then bytes) correctly mimics the `rep movsd/movsb`
        // pattern seen in the original assembly.
        uint32 dwords = index->filenameTableSize >> 2;
        uint32 bytes  = index->filenameTableSize & 3;
        while (dwords--) {
            *(uint32*)dstNames = *(const uint32*)srcNames;
            dstNames += 4; srcNames += 4;
        }
        while (bytes--) {
            *dstNames++ = *srcNames++;
        }
    } else {
        // This is a disk-backed package. Read data from the file.
        bkSeekFile(index->fp, index->pauSize * index->filenameTableOffset, EHOSTSEEK_SET);
        bkReadFromFile(index->fp, dstNames, index->filenameTableSize);
    }

    // A new table was successfully loaded.
    return 1;
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
    if (id.crc == 0) {
        TBFilenameTableHeader* it = bFilenameTable.next;
        if (it != &bFilenameTable) {
            do {
                TBFilenameTableHeader* next = it->next;
                bkHeapFree(it);
                it = next;
            } while (it != &bFilenameTable);
        }
        bFilenameTable.prev = &bFilenameTable;
        bFilenameTable.next = &bFilenameTable;
        return 0;
    }

    {
        TBFilenameTableHeader* it = bFilenameTable.next;
        while (it != &bFilenameTable) {
            if (it->package.crc == id.crc) {
                int rc = --it->refCount;
                if (rc != 0)
                    return rc;

                // unlink and free
                it->prev->next = it->next;
                it->next->prev = it->prev;
                bkHeapFree(it);
                return 0;
            }
            it = it->next;
        }
    }
    return -1;
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
        bkPrintf("*** WARNING *** bkFilenameTableAddRef was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
    TBFilenameTableHeader* it = bFilenameTable.next;
    while (it != &bFilenameTable) {
        if (id.crc == 0 || it->package.crc == id.crc) {
            if (crc == 0) {
                return it->filename; // +0x14 in the header
            }

            // pairs region and names region layout
            const uint32* pairs = (const uint32*)((const char*)it + 0x114);
            const char* names  = (const char*)it + 0x114 + it->noofFiles * 8;

            uint32 i, n = it->noofFiles;
            for (i = 0; i < n; ++i) {
                if (pairs[i * 2 + 0] == crc) {
                    uint32 nameOff = pairs[i * 2 + 1];
                    return (char*)(names + nameOff);
                }
            }
        }
        it = it->next;
    }
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
    TBFileHandle fp;

    if (flags & BFILEOPENFLAG_WITHSEARCH) {
        if (!bkOpenFileReadOnlyWithSearch(filename, &fp, 0, 0, 0))
            return 0;
    } else {
        if (!bkOpenFileReadOnly(filename, &fp))
            return 0;
    }

    const int len = bFileLength(fp);
    bkCloseFile(fp);
    return len;
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
    if (!index) {
        TBFileHandle fp;
        if (flags & BFILEOPENFLAG_WITHSEARCH) {
            if (!bkOpenFileReadOnlyWithSearch(filename, &fp, 0, 0, 0))
                return 0;
        } else {
            if (!bkOpenFileReadOnly(filename, &fp))
                return 0;
        }
        // plain file length path
        {
            int len = bFileLength(fp);
            bkCloseFile(fp);
            return len;
        }
    }

    // package path: compute CRC(filename)
    {
        const uchar* s = (const uchar*)filename;
        uint crc = 0;
        while (*s) {
            uchar top = (uchar)(crc >> 24);
            crc = (crc << 8) ^ bCRCtable[(uchar)(*s ^ top)];
            ++s;
        }

        // binary search in package index by crc; entries are sorted by .crc
        {
            TBFileIndex* arr = index->index;     // base pointer to file index array
            int lo = 0;
            int hi = index->noofFiles - 1;       // number of files in the package

            while (lo <= hi) {
                int mid = (lo + hi) >> 1;
                unsigned int k = arr[mid].crc;   // field read matches [+4] in asm
                if (k == crc) {
                    return (int)arr[mid].size;   // field read matches [+8] in asm
                }
                if (k < crc) lo = mid + 1; else hi = mid - 1;
            }

            // not found: print the same two messages, then return 0
            bkPrintf("bkLoadFile: Could not find 0x%08x in package\n", crc);
            bkPrintf("Could not find '%s' in package\n", filename);
            return 0;
        }
    }
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
    char *originalFilename;
    int fileLen;
    uint size;
    uint32 group;
    int largest;
    int freeSpace;
    uchar *data;
    TBFileHandle fp;

    originalFilename = filename;

    if (bkOpenFileReadOnlyWithSearch(filename, &fp, NULL, 0, 0) == 0) {
        bkPrintf("LoadSingleFile: *** Could not load file '%s' ***\n", originalFilename);
        return NULL;
    }

    fileLen = bFileLength(fp);

    data = dataPtr;
    size = (uint)(fileLen + noofExtraBytes);

    if (dataPtr == NULL) {
        group = bGetCurrentGroup();
        if (group == (uint32)BDEFAULTGROUP) {
            group = (uint32)"Package";
        }

        data = (uchar *)MALLOCEX(size, group);
        if (data == NULL) {
            bkPrintf("EnsureAlloc: *** Out of memory on Babel heap for file (need %d bytes) ***\n", size);

            largest = 0;
            freeSpace = bkHeapFreeSpace(&largest);
            bkPrintf("EnsureAlloc: (only %d bytes available: %d more required, max %d bytes (%d short)) ***\n",
                     freeSpace, (int)size - freeSpace, largest, (int)size - largest);
            return NULL;
        }
    } else {
        if (bkHeapGetBlockSize(dataPtr) < (int)size) {
            return NULL;
        }
    }

    bkReadFromFile(fp, data, fileLen);
    fclose(fp);

    if (retSize != NULL) {
        *retSize = fileLen;
    }

    bkPrintf("LoadSingleFile: Loaded file '%s' of %d bytes OK\n", originalFilename, fileLen);
    return data;
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
        bkPrintf("*** WARNING *** bkFileFindFirst was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bkFileFindNext was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bkFileFindClose was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bkGetSystemLanguage was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return BLANGUAGEID_UK;
}