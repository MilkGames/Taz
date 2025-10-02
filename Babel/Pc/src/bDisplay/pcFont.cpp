// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcFont.cpp
//   Purpose : Defines the _TBFont struct for font resources. Declares functions for font initialization, shutdown, deletion, loading, printing (formatted and lite), extent calculation, formatted rectangle calculation, printing to vertex arrays, showing font textures, and ROM font handling
// Component : Babel Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bInitFont
	Purpose : initialise the font module
	Parameters : 
	Returns : 
	Info : 
*/
void bInitFont()
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bShutdownFont
	Purpose : shutdown the font module
	Parameters : 
	Returns : 
	Info : 
*/
void bShutdownFont()
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bDeleteFont
	Purpose : delete a font
	Parameters : font
	Returns : 
	Info : 
*/
void bDeleteFont(TBFont *font)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdFontExtent
	Purpose : get font extent
	Parameters : font, string, strLen, width, height, callback, callbackContext
	Returns : 
	Info : 
*/
void bdFontExtent(const TBFont *font, const ushort *string, int strLen, float *width, float *height, TBPrintFontCallback callback, void *callbackContext)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdShowFont
	Purpose : show font
	Parameters : font, x, y, w, h
	Returns : 
	Info : 
*/
void bdShowFont(const TBFont *font, float x, float y, float w, float h)
{
    return;
}
