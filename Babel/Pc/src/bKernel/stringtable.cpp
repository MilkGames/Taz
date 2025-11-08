// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : stringtable.cpp
//   Purpose : string table & localisation functions
// Component : Generic Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Locals

static const char* bLanguageCodes[] = {
    "uk", "f", "d", "e", "it", "nl", "sw", "fin", "n", "dk", "us", "jp"
};

// ********************************************************************************
// Helper Functions

ushort *bStringPrintFormat(ushort* dst, wchar_t conv, int width, int precision, int zero_pad, va_list* argp)
{
	// MG: Basically if you look at this function in Ghidra you'll see that it was most likely an
	// MG: assembly function. I'm not gonna waste my time on that, recreated this function with
	// MG: _snwprintf in mind.
    if (!dst) return dst;

    // Build a minimal wide printf format like L"%0*.*d"
    wchar_t fmt[16];
    wchar_t* f = fmt;
    *f++ = L'%';
    if (zero_pad) *f++ = L'0';
    *f++ = L'*';
    *f++ = L'.';
    *f++ = L'*';
    *f++ = conv;
    *f++ = 0;

    // Choose default precision for floats if not specified
    int p = precision;
    if (p < 0 && (conv==L'f'||conv==L'e'||conv==L'g'||conv==L'F'||conv==L'E'||conv==L'G')) p = 6;
    if (p < 0) p = 0;

    wchar_t tmp[256];
    tmp[0] = 0;

    switch (conv) {
        case L'd': case L'i': {
            // Default integral promotion is int; wide printf handles %d
            int v = va_arg(*argp, int);
            _snwprintf(tmp, sizeof(tmp)/sizeof(tmp[0]), fmt, width, p, v);
        } break;
        case L'u': case L'x': case L'X': {
            uint v = va_arg(*argp, uint);
            _snwprintf(tmp, sizeof(tmp)/sizeof(tmp[0]), fmt, width, p, v);
        } break;
        case L'f': case L'e': case L'g': case L'F': case L'E': case L'G': {
            double v = va_arg(*argp, double);
            _snwprintf(tmp, sizeof(tmp)/sizeof(tmp[0]), fmt, width, p, v);
        } break;
        default: {
            // Fallback: emit literal % + conv
            *dst++ = (ushort)L'%';
            if (conv) *dst++ = (ushort)conv;
            *dst = 0;
            return dst;
        }
    }

    const wchar_t* r = tmp;
    while (*r) *dst++ = (ushort)*r++;
    *dst = 0;
    return dst;
}


// ********************************************************************************
// Function Implementations

/* --------------------------------------------------------------------------------
   Function : bkFixStringTableCRC
   Purpose : fix the CRC of a stringtable resource filename to account for the language setting
   Parameters : splash CRC
   Returns : modified splash CRC
   Info : 
*/

uint32 bkFixStringTableCRC(uint32 crc)
{
    char buffer[8];
    const char* langCode = bLanguageCodes[bLanguage];
    sprintf(buffer, ".%s", langCode);
    return bkCRC32((uchar *)buffer, strlen(buffer), crc);
}


/* --------------------------------------------------------------------------------
   Function : bLoadStringTableByCRC
   Purpose : load a string table
   Parameters : package index, crc
   Returns : ptr to stringtable or NULL
   Info : 
*/

TBStringTable *bLoadStringTableByCRC(TBPackageIndex *pakIndex, uint32 crc)
{
    // Load raw blob: [TBStringTable header][TBStringTableString array][...strings block...]
    TBStringTable* stringTable = (TBStringTable*)bkLoadFileByCRC(pakIndex, crc, 0, 0, 0, 0);
    if (!stringTable)
        return 0;

    // Point to the array that immediately follows the header
    stringTable->strings = (TBStringTableString*)((char*)stringTable + sizeof(TBStringTable));

    // One-time fixups: convert stored offsets into absolute pointers
    if ((stringTable->flag & 1) == 0) {
        const char* base = (const char*)stringTable;
        TBStringTableString* s = stringTable->strings;
        const int count = stringTable->noofStrings;

        for (int i = 0; i < count; ++i) {
            // string: always an offset into this blob
            s[i].string = (ushort*)(base + (unsigned int)s[i].string);

            // audioFilename: fix only if present (0 means "none")
            if (s[i].audioFilename) {
                s[i].audioFilename = (char*)(base + (unsigned int)s[i].audioFilename);
            }
        }

        stringTable->flag |= 1; // mark as fixed
    }

    return stringTable;
}


/* --------------------------------------------------------------------------------
   Function : bDeleteStringTable
   Purpose : delete a string table
   Parameters : ptr to string table
   Returns : 
   Info : 
*/

void bDeleteStringTable(TBStringTable *tablePtr)
{
    if (!tablePtr) return;

    // Free only if not 'Loaded'
    if (!tablePtr->resInfo.packageId.loaded) {
        bkHeapFree(tablePtr);
    }
}


/* --------------------------------------------------------------------------------
   Function : bkString16to8
   Purpose : convert a 16bit string to 8bit
   Parameters : destination buffer, source buffer
   Returns : destination buffer
   Info : 
*/

char *bkString16to8(char *dest, const ushort *src)
{
    char* start = dest;

    if (*src == 0) {
        *dest = '\0';
        return start;
    }
    
    do {
        *dest = (char)*src;
        src++;
        dest++;
    } while (*src != 0);

    *dest = '\0';
    return start;
}


/* --------------------------------------------------------------------------------
   Function : bkString8to16
   Purpose : convert a 8bit string to 16bit
   Parameters : destination buffer, source buffer
   Returns : destination buffer
   Info : 
*/

ushort *bkString8to16(ushort *dest, const char * src)
{
    ushort* start = dest;

    while (*src != 0) {
        *dest = (ushort)(uchar)*src;
        src++;
        dest++;
    }

    *dest = 0;

    return start;
}

ushort *bkString8to16(ushort *dest, const uchar * src)
{
        bkPrintf("*** WARNING *** bkString8to16 #2 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkStringLength16
   Purpose : get the length of a 16bit string, excluding the terminating NULL
   Parameters : string buffer
   Returns : length of string
   Info : 
*/

int bkStringLength16(const ushort *str)
{
    const ushort* p = str;
    ushort c;

    do {
        c = *p;
        p++;
    } while (c != 0);

    return (((int)p - (int)str) / 2) - 1;
}


/* --------------------------------------------------------------------------------
   Function : bkStringCopy16
   Purpose : Copy a 16bit string, including the terminating NULL
   Parameters : dest buffer, source buffer
   Returns : destination string
   Info : 
*/

ushort *bkStringCopy16(ushort *dst, const ushort *src)
{
    ushort* start = dst;
    ushort c;

    do {
        c = *src;
        *dst = c;
        src++;
        dst++;
    } while (c != 0);

    return start;
}


/* --------------------------------------------------------------------------------
   Function : bkStringCompare16
   Purpose : Compare two 16bit strings, returning less than, equal to, or greater than
   Parameters : first string, second string, length to compare (or 0 for full length)
   Returns : -1 if src <  dst, 0 if src == dst, +1 if src >  dst
   Info : 
*/

int bkStringCompare16(const ushort *src, const ushort *dst, int length)
{
        bkPrintf("*** WARNING *** bkStringCompare16 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkStringVSprintf16
   Purpose : formatted printing to another 16 bit string 
   Parameters : 16 bit target string, 16 bit format string, va list args
   Returns : length of target string created.
   Info : 
*/

int bkStringVSprintf16(ushort *target, const ushort *format, va_list argp)
{
    if (!target || !format) return 0;
    const ushort* f = format;
    ushort* out = target;

    while (*f) {
        ushort ch = *f++;
        if (ch != (ushort)L'%') {
            *out++ = ch;
            continue;
        }

        // Parse flags: support only '0' (zero padding)
        int zero_pad = 0;
        if (*f == (ushort)L'0') { zero_pad = 1; ++f; }

        // Parse width
        int width = 0;
        while (*f >= (ushort)L'0' && *f <= (ushort)L'9') {
            width = width * 10 + (int)(*f - (ushort)L'0');
            ++f;
        }

        // Parse precision: .<digits>
        int precision = -1;
        if (*f == (ushort)L'.') {
            ++f;
            precision = 0;
            while (*f >= (ushort)L'0' && *f <= (ushort)L'9') {
                precision = precision * 10 + (int)(*f - (ushort)L'0');
                ++f;
            }
        }

        // Length modifier (kept for compatibility, but not used directly here)
        if (*f == (ushort)L'l' || *f == (ushort)L'h') {
            ++f; // ignore; bStringPrintFormat will read the promoted types
        }

        // Conversion
        wchar_t conv = (wchar_t)(*f ? *f++ : 0);

        switch (conv) {
            case L'%': {
                *out++ = (ushort)L'%';
            } break;

            case L'c': {
                int v = va_arg(argp, int);
                *out++ = (ushort)(v & 0xFFFF);
            } break;

            case L's': { // narrow string -> widen 1:1 into UTF-16
                const char* s = va_arg(argp, const char*);
                if (!s) s = "(null)";
                int n = 0;
                if (precision >= 0) {
                    while (*s && n < precision) { *out++ = (uchar)*s++; ++n; }
                } else {
                    while (*s) { *out++ = (uchar)*s++; }
                }
            } break;

            case L'S': { // already UTF-16
                const ushort* ws = va_arg(argp, const ushort*);
                if (!ws) { const wchar_t* wnull = L"(null)"; while (*wnull) *out++ = (ushort)*wnull++; break; }
                int n = 0;
                if (precision >= 0) {
                    while (*ws && n < precision) { *out++ = *ws++; ++n; }
                } else {
                    while (*ws) { *out++ = *ws++; }
                }
            } break;

            // numeric / floating-point go through the single-specifier formatter
            case L'd': case L'i': case L'u': case L'x': case L'X':
            case L'f': case L'e': case L'g': case L'F': case L'E': case L'G': {
                out = bStringPrintFormat(out, conv, width, precision, zero_pad, &argp);
            } break;

            default: {
                // Unknown specifier: emit verbatim
                *out++ = (ushort)L'%';
                if (conv) *out++ = (ushort)conv;
            } break;
        }
    }

    *out = 0; // null-terminate
    return (int)(out - target);
}



/* --------------------------------------------------------------------------------
   Function : bkStringSprintf16
   Purpose : formatted printing to another 16 bit string 
   Parameters : 16 bit target string, 16 bit format string, args (see info)
   Returns : length of target string created.
   Info : format supports %d, %ld, %f,
                      and %c, %s (8 bit), %S (16 bit),
                      and %i, %u, %lu, 
					  and %.2f, %2.f %2.2f
*/

int bkStringSprintf16(ushort *target, const ushort *format, ...)
{
        bkPrintf("*** WARNING *** bkStringSprintf16 #1 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkStringVSprintf16
   Purpose : formatted printing to another 16 bit string 
   Parameters : 16 bit target string, 8 bit format string, va list args 
   Returns : length of target string created.
   Info :
*/

int bkStringVSprintf16(ushort *target, const char *format, va_list argp)
{
        bkPrintf("*** WARNING *** bkStringVSprintf16 #2 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkStringSprintf16
   Purpose : formatted printing to another 16 bit string 
   Parameters : 16 bit target string, 8 bit format string, args (see info)
   Returns : length of target string created.
   Info : format supports %d, %ld, %f,
                      and %c, %s (8 bit), %S (16 bit),
                      and %i, %u, %lu, 
					  and %.2f, %2.f %2.2f
*/

int bkStringSprintf16(ushort *target, const char *format, ...)
{
        bkPrintf("*** WARNING *** bkStringSprintf16 #2 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkGetStringSample
   Purpose : Get sample from a string
   Parameters : string table, string number
   Returns : sample pointer
   Info : 
*/

struct _TBSample *bkGetStringSample(TBStringTable *strTable, int strId)
{
        bkPrintf("*** WARNING *** bkGetStringSample was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}

/* --------------------------------------------------------------------------------
   Function : bkGetStringSampleFilename
   Purpose : Get sample from a string
   Parameters : string table, string number, filename pointer, max length to copy
   Returns : 
   Info : assumes maxlen > 0
*/

void bkGetStringSampleFileName(TBStringTable *strTable, int strId, char *filename, int maxlen)
{
        bkPrintf("*** WARNING *** bkGetStringSampleFileName was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bStringTableInit
   Purpose : initialise the stringtable system
   Parameters : 
   Returns : 
   Info : 
*/

void bStringTableInit()
{
        bkPrintf("*** WARNING *** bStringTableInit was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bStringTableShutdown
   Purpose : shutdown the stringtable system
   Parameters : 
   Returns : 
   Info : 
*/

void bStringTableShutdown()
{
        bkPrintf("*** WARNING *** bStringTableShutdown was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkStringFindLetter16
   Purpose : find the occurance of letter in a 16 bit string
   Parameters : source buffer, letter to locate
   Returns : Offset to string or NULL if not found
   Info : 
*/

ushort *bkStringFindLetter16(const ushort *src, ushort letter)
{
        bkPrintf("*** WARNING *** bkStringFindLetter16 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

/* --------------------------------------------------------------------------------
   Function : bkStringFindLetterReverse16
   Purpose : find the occurance of letter in a 16 bit string searching from the end first
   Parameters : source buffer, letter to locate
   Returns : Offset to string or NULL if not found
   Info : 
*/

ushort *bkStringFindLetterReverse16(const ushort *src, ushort letter)
{
        bkPrintf("*** WARNING *** bkStringFindLetterReverse16 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}