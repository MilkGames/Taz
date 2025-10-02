// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcSplash.cpp
//   Purpose : Defines BSPLASHFLAG_SCALETOFIT and the _TBSplash struct. Declares functions for loading/deleting splashes, drawing splashes, and fixing splash CRCs
// Component : Babel Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bLoadSplashByCRC
	Purpose : load splash by CRC
	Parameters : index, crc
	Returns : splash
	Info : 
*/
TBSplash *bLoadSplashByCRC(TBPackageIndex *index, uint32 crc)
{
    return NULL;
}

/*	--------------------------------------------------------------------------------
	Function : bDeleteSplash
	Purpose : delete splash
	Parameters : splash
	Returns : 
	Info : 
*/
void bDeleteSplash(TBSplash *splash)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdDrawSplash
	Purpose : draw splash
	Parameters : splash, xPos, yPos, flags
	Returns : 
	Info : 
*/
void bdDrawSplash(TBSplash *splash, int xPos, int yPos, int flags)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdFixSplashCRC
	Purpose : fix splash CRC
	Parameters : crc
	Returns : fixed CRC
	Info : 
*/
uint32 bdFixSplashCRC(uint32 crc)
{
    return 0;
}
