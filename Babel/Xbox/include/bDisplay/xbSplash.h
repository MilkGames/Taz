// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : xbSplash.h
//   Purpose : splash screen resource functionality
// Component : Xbox Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BDISPLAY_XBSPLASH_H__
#define __BDISPLAY_XBSPLASH_H__


// ********************************************************************************
// Types, Structures and Classes

#define BSPLASHFLAG_SCALETOFIT		0x00000001	

// a splash resource
typedef struct _TBSplash {
	TBResourceInfo			resInfo;			// resource info header

	int32					xDim;				// image width
	int32					yDim;				// image height
	uchar					*data[2];			// data fields
} TBSplash;


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

#endif		// __BDISPLAY_XBSPLASH_H__
