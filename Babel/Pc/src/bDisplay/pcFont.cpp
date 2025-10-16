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

// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bInitFont
   Purpose : initialise the font functions
   Parameters : 
   Returns : 
   Info : called by bdDisplayOpen
*/

void bInitFont()
{
        bkPrintf("*** WARNING *** bInitFont was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bShutdownFont
   Purpose : shutdown the font functions
   Parameters : 
   Returns : 
   Info : called by bdDisplayClose
*/

void bShutdownFont()
{
        bkPrintf("*** WARNING *** bShutdownFont was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bDeleteFont
   Purpose : delete a font resource
   Parameters : ptr to font
   Returns : 
   Info : 
*/

void bDeleteFont(TBFont *font)
{
        bkPrintf("*** WARNING *** bDeleteFont was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bLoadFontByCRC
   Purpose : load a font resource from a package
   Parameters : package index, crc
   Returns : ptr to font or NULL for failure
   Info : 
*/

TBFont *bLoadFontByCRC(TBPackageIndex *pakIndex, uint32 crc)
{
        bkPrintf("*** WARNING *** bLoadFontByCRC was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}


/* --------------------------------------------------------------------------------
   Function : bdPrintFont
   Purpose : print a string in a particular font
   Parameters : font, 16bit string, #chars in string not including NULL, formatting, colour, callback, callback context
   Returns : 
   Info : Backslash character denotes escape sequence :
				\\			   - insert a single backslash
				\[textureName] - insert a quad using the specified texture
				\(string)	   - callback based insertion
*/

void bdPrintFont(const TBFont *font, const ushort *string, int strLen, const TBFontFormatting *formatting, int r,int g,int b, int a,
																	TBPrintFontCallback callback, void *callbackContext)
{
        bkPrintf("*** WARNING *** bdPrintFont was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bdPrintFontLite
   Purpose : print a string in a particular font
   Parameters : font, 16bit string, #chars in string not including NULL, position, colour, callback, callback context
   Returns : 
   Info : 
*/

void bdPrintFontLite(const TBFont *font, const ushort *string, int strLen, float x, float y, int r,int g,int b, int a,
																	TBPrintFontCallback callback, void *callbackContext)
{
        bkPrintf("*** WARNING *** bdPrintFontLite was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : bdFontExtent
   Purpose : determine the extent of a string printed in a particular font
   Parameters : font, 16bit string, #chars in string not including NULL, ptr to receive width, ptr to receive height
				callback, callback context
   Returns : 
   Info : 
*/

void bdFontExtent(const TBFont *font, const ushort *string, int strLen, float *width, float *height, TBPrintFontCallback callback, void *callbackContext)
{
        bkPrintf("*** WARNING *** bdFontExtent was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bdFontFormattedRect
   Purpose : determine the rectangle that would be occupied by a formatted text string
   Parameters : rect for limits, font, 16bit string, #chars in string not including NULL, formatting, callback,
				callback context
   Returns : rect valid? TRUE/FALSE
   Info : rect is [0]=minx, [1]=miny, [2]=maxx, [3]=maxy
*/

int bdFontFormattedRect(float *rect, const TBFont *font, const ushort *string, int strLen, const TBFontFormatting *formatting,
																TBPrintFontCallback callback, void *callbackContext)
{
        bkPrintf("*** WARNING *** bdFontFormattedRect was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bPrintFontVerts
   Purpose : print a string in a particular font to a vertex array
   Parameters : font, 16bit string, #chars in string not including NULL, formatting, colour, callback, callback context
   Returns : #verts written
   Info :
*/

int bPrintFontVerts(TBPrimVertex *vertBase, TBFontLineInfo *line, const TBFont *font, const ushort *string, int strLen,
					 const TBFontFormatting *formatting, int r,int g,int b, int a, TBPrintFontCallback callback,
					 void *callbackContext)
{
        bkPrintf("*** WARNING *** bPrintFontVerts was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bdShowFont
   Purpose : show the font texture
   Parameters : font, x, y, width, height
   Returns : 
   Info : 
*/

void bdShowFont(const TBFont *font,float x, float y, float w, float h)
{
        bkPrintf("*** WARNING *** bdShowFont was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}