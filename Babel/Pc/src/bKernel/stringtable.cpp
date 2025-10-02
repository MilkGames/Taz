// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : stringtable.cpp
//   Purpose : string table & localisation functions
// Component : Generic Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

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
    return 0;
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
    return NULL;
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
    return;
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
    return NULL;
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
    return 0;
}

ushort *bkString8to16(ushort *dest, const uchar * src)
{
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
    return 0;
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
    return 0;
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
    return 0;
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
    return 0;
}