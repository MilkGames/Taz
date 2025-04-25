// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : xbFont.h
//   Purpose : Font management and printing
// Component : Xbox Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BDISPLAY_XBFONT_H__
#define __BDISPLAY_XBFONT_H__

// ********************************************************************************
// Constants and Macros


// ********************************************************************************
// Types, Structures and Classes

// font resource
typedef struct _TBFont {
	TBResourceInfo		resInfo;				// resource info header

	int					fontType;
	TBTexture			*texture;				// texture font lives on
	float				lineHeight;				// height of a character
	int32				pad;

	TBFontGlyphInfo		glyph[BFONT_CHARACTERS_IN_BASIC_FONT];		// array of glyph infos
} TBFont;


// ********************************************************************************
// Globals



// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bInitFont
   Purpose : initialise the font functions
   Parameters : 
   Returns : 
   Info : called by bdDisplayOpen
*/

void bInitFont();


/* --------------------------------------------------------------------------------
   Function : bShutdownFont
   Purpose : shutdown the font functions
   Parameters : 
   Returns : 
   Info : called by bdDisplayClose
*/

void bShutdownFont();


/* --------------------------------------------------------------------------------
   Function : bDeleteFont
   Purpose : delete a font resource
   Parameters : ptr to font
   Returns : 
   Info : 
*/

void bDeleteFont(TBFont *font);


/* --------------------------------------------------------------------------------
   Function : bLoadFontByCRC
   Purpose : load a font resource from a package
   Parameters : package index, crc
   Returns : ptr to font or NULL for failure
   Info : 
*/

TBFont *bLoadFontByCRC(TBPackageIndex *pakIndex, uint32 crc);


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
																	TBPrintFontCallback callback, void *callbackContext);


/* --------------------------------------------------------------------------------
   Function : bdPrintFontLite
   Purpose : print a string in a particular font
   Parameters : font, 16bit string, #chars in string not including NULL, position, colour, callback, callback context
   Returns : 
   Info : 
*/

void bdPrintFontLite(const TBFont *font, const ushort *string, int strLen, float x, float y, int r,int g,int b, int a,
																	TBPrintFontCallback callback, void *callbackContext);

/* --------------------------------------------------------------------------------
   Function : bdFontExtent
   Purpose : determine the extent of a string printed in a particular font
   Parameters : font, 16bit string, #chars in string not including NULL, ptr to receive width, ptr to receive height
				callback, callback context
   Returns : 
   Info : 
*/

void bdFontExtent(const TBFont *font, const ushort *string, int strLen, float *width, float *height, TBPrintFontCallback callback=0, void *callbackContext=0);


/* --------------------------------------------------------------------------------
   Function : bdFontFormattedRect
   Purpose : determine the rectangle that would be occupied by a formatted text string
   Parameters : rect for limits, font, 16bit string, #chars in string not including NULL, formatting, callback,
				callback context
   Returns : rect valid? TRUE/FALSE
   Info : rect is [0]=minx, [1]=miny, [2]=maxx, [3]=maxy
*/

int bdFontFormattedRect(float *rect, const TBFont *font, const ushort *string, int strLen, const TBFontFormatting *formatting,
																TBPrintFontCallback callback, void *callbackContext);


/* --------------------------------------------------------------------------------
   Function : bPrintFontVerts
   Purpose : print a string in a particular font to a vertex array
   Parameters : font, 16bit string, #chars in string not including NULL, formatting, colour, callback, callback context
   Returns : #verts written
   Info :
*/

int bPrintFontVerts(TBPrimVertex *vertBase, TBFontLineInfo *line, const TBFont *font, const ushort *string, int strLen,
					 const TBFontFormatting *formatting, int r,int g,int b, int a, TBPrintFontCallback callback,
					 void *callbackContext);


/* --------------------------------------------------------------------------------
   Function : bdShowFont
   Purpose : show the font texture
   Parameters : font, x, y, width, height
   Returns : 
   Info : 
*/

void bdShowFont(const TBFont *font,float x, float y, float w, float h);


/* --------------------------------------------------------------------------------
   Function : bdLoadROMFont
   Purpose : load a font from the ROM
   Parameters : 
   Returns : OK/FAIL
   Info : 
*/

inline int bdLoadROMFont() {return FAIL;}


/* --------------------------------------------------------------------------------
   Function : bdPrintROMFont
   Purpose : Draw the specified string with ROM font
   Parameters : x,y,z, red,green,blue,alpha, string, character width (size), height (to return)
   Returns : total string width in texels
   Info : 
*/

inline int bdPrintROMFont(ushort* string, int len, int x, int y, int r, int g, int b, int a, int cwidth, int *cheight) {return 0;}


/* --------------------------------------------------------------------------------
   Function : bdFreeROMFont
   Purpose : free ROM font from main memory (which would have been inserted as required by bFontROMPrint)
   Parameters : 
   Returns : 
   Info : 
*/

inline void bdFreeROMFont()  {}


#endif		// __BDISPLAY_XBFONT_H__
