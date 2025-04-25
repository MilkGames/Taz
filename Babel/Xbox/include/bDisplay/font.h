// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : font.h
//   Purpose : Font management and printing
// Component : General Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BDISPLAY_FONT_H__
#define __BDISPLAY_FONT_H__

// ********************************************************************************
// Constants and Macros

// font callback reason codes
typedef enum EBFontCallbackReason {
	EBFONTCALLBACKREASON_POLY,					// a character has been output
	EBFONTCALLBACKREASON_ESCAPE,				// a callback escape sequence has been encountered
	EBFONTCALLBACKREASON_TEXTURE,				// a texture escape sequence has been encountered
};


// font formatting options
typedef enum EBFontFormatting {
	EBFONTFORMATTING_LEFT,						// left-justify text
	EBFONTFORMATTING_TOP,						// top-justify text
	EBFONTFORMATTING_CENTRE,					// centre text
	EBFONTFORMATTING_RIGHT,						// right justify text
	EBFONTFORMATTING_BOTTOM,					// bottom justify text
	EBFONTFORMATTING_JUSTIFY,					// full justified text
} EBFontFormatting;

#define BMAX_PRINT_CHARS				1024	// the longest string we can handle

#define BFONT_FIRST_STORED_CHARACTER	32		// A space

#define BFONT_MISSING_CHAR				0xA0	// Empty box for Xbox TRC (Overwrites hard space)

#define BFONT_CHARACTERS_IN_BASIC_FONT	224		// 256-BFONT_FIRST_STORED_CHARACTER


// ********************************************************************************
// Types, Structures and Classes

// glyph info
typedef struct _TBFontGlyphInfo {
	short				xPos, yPos;				// position of glyph on texture
	short				width, height;			// width and height of glyph
} TBFontGlyphInfo;


typedef struct _TBPrintFontGlyph {
	TBTexture			*texture;				// texture for glyph
	float				width, height;			// width and height
	float				u[4],v[4];				// texture coordinates
	int					charCode;				// character code for non-escape glyphs
} TBPrintFontGlyph;


// info struct for font callback
typedef struct _TBFontCallbackInfo {
	TBPrimVertex		*vertexBase;			// ptr to start of vertices
	TBPrimVertex		*curVertex;				// ptr to current vertex quad
	TBPrintFontGlyph	*glyphs;				// glyph array
	TBPrintFontGlyph	*curGlyph;				// current glyph

	char				*escapeString;			// ptr to escape string for callback escape sequences
	int32				lineNumber;				// zero-based index of line number being processed (after formatting)
	int32				glyphNumber;			// zero-based index of glyph being processed (relative to start of line 'lineNumber')
	int32				glyphIndex;				// zero-based index of glyph being processed (relative to start of text)

	void				*context;				// callback context
} TBFontCallbackInfo;


// font print callback
typedef int (*TBPrintFontCallback)(EBFontCallbackReason reason, TBFontCallbackInfo *info);


typedef struct _TBFontFormatting {
	float				x, y;					// top-left position of formatting rectangle
	float				width, height;			// width and height of formatting rectangle
	EBFontFormatting	horzFormat;				// horizontal formatting
	EBFontFormatting	vertFormat;				// vertical formatting
	int					wrap;					// 1=use word wrapping, 0=don't use word wrapping
	float				lineSpace;				// extra spacing between lines (-ve to reduce space)
} TBFontFormatting;


// information about a formatted line of text
typedef struct _TBFontLineInfo {
	TBPrintFontGlyph			*glyph;			// ptr to first glyph in line
	int							noofGlyphs;		// #glyphs in line
	float						width, height;	// width and height of line
} TBFontLineInfo;


// ********************************************************************************
// Globals

#endif		// __BDISPLAY_FONT_H__
