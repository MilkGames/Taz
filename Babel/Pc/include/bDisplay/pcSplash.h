// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcSplash.h
//   Purpose : splash screen resource functionality
// Component : Xbox Display
//   Comment : EXPECTED TO BE THE SAME AS xbSplash.h, DIRECT REWRITE!
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BDISPLAY_PCSPLASH_H__
#define __BDISPLAY_PCSPLASH_H__


// ********************************************************************************
// Types, Structures and Classes

#define BSPLASHFLAG_SCALETOFIT		0x00000001	

// a splash resource
typedef struct _TBSplash {
    TBResourceInfo   resInfo;           // 0x00..0x1F (32 bytes). First dword used as sentinel on failure
    int32            xRes;              // 0x20
    int32            yRes;              // 0x24
    uchar*           data[2];           // 0x28
    int              count;             // 0x30
    uint32           crc[16];           // 0x34..0x73
    TBTexture*       texture[16];       // 0x74..0xB3 (filled at runtime)
} TBSplash; // total: 0xB4


// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bLoadSplashByCRC
   Purpose : Load a splash from a package, returning a handle to it
   Parameters : package index, splash CRC
   Returns : handle or NULL for failure
   Info : 
*/

TBSplash *bLoadSplashByCRC(TBPackageIndex *index, uint32 crc);


/* --------------------------------------------------------------------------------
   Function : bDeleteSplash
   Purpose : Delete a splash
   Parameters : splash handle
   Returns : 
   Info : 
*/

void bDeleteSplash(TBSplash *splash);


/* --------------------------------------------------------------------------------
   Function : bdDrawSplash
   Purpose : draw a 'splash screen' bitmap to the back buffer at a certain point
   Parameters : splash ptr, position relative to top-left corner (-1 to centre), flags
   Returns : 
   Info : 
*/

void bdDrawSplash(TBSplash *splash, int xPos, int yPos, int flags=0);


/* --------------------------------------------------------------------------------
   Function : bdFixSplashCRC
   Purpose : fix the CRC of a splash resource filename to take account of the screen format
   Parameters : splash CRC
   Returns : modified splash CRC
   Info : 
*/

uint32 bdFixSplashCRC(uint32 crc);

#endif		// __BDISPLAY_PCSPLASH_H__
