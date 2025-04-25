// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : util.h
//   Purpose : Utility functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_UTIL
#define __H_UTIL

#include "maths.h"						// PP: Maths stuff
#include "Colour.h"						// PP: handy colour struct
#include "collisioncache.h"				// PP: collision cache stuff


#define UTILFONTPRINT_LEFTALIGN		(0)
#define UTILFONTPRINT_RIGHTALIGN	(1)
#define UTILFONTPRINT_CENTRE		(2)

// CPW: move from here eventually
void utilFontPrint(char *s, int x, int y, float scale, int just);
int utilGetPrettyTime(char *s, float time, char *f = NULL);


#define BASE_BINARY		(2)
#define BASE_OCTAL		(8)
#define BASE_DECIMAL    (10)
#define BASE_HEX		(16)

uint32 utilBaseStringtoLong(char *s, int base, int len);

int utilLongtoBaseString(char *s, int32 num, int base);

int utilFindChar(char *s, char c);


/*	--------------------------------------------------------------------------------
	Function 	: util3DText
	Purpose 	: display a text string at a position in 3D
	Parameters 	: string, position
	Returns 	: 
	Info 		: 
*/
void util3DText(char* const string, const TBVector pos);


/*	--------------------------------------------------------------------------------
	Function 	: util3DText
	Purpose 	: display a text string at a position in 3D
	Parameters 	: position (TBVector or ref to VEC), format string, argument list
	Returns 	: 
	Info 		: 
*/
void util3DText(const VEC& pos, const char * const fmt, ...);


/*	--------------------------------------------------------------------------------
	Function 	: utilWorkInProgress
	Purpose 	: display a "work in progress" sign at a position in 3D
	Parameters 	: position
	Returns 	: 
	Info 		: 
*/
inline void utilWorkInProgress(TBVector pos)
{
	util3DText("work\nin\nprogress", pos);
}


/*	--------------------------------------------------------------------------------
	Function 	: utilWait
	Purpose 	: delay the game for a specified length of time
	Parameters 	: (opt/one second) length of time for which to delay the game
	Returns 	: 
	Info 		: 
*/
void utilWait(const float time=1.0f);


/*	--------------------------------------------------------------------------------
	Function 	: utilDraw2DSprite
	Purpose 	: draw a 2D icon
	Parameters 	: Suree Bob, we got parameters - big ones!  We got a whole field full of parameters out back.  Wanna see 'em?
					texture ptr, 2D pos (TBVector2D!), (opt/ICON_WIDTH)width, (opt/0.0f=width)height,
					(opt/0.0f)ang, (opt/solid white)colour, (opt/false)u flip,
					(opt/false)v flip, (opt/NULL)verts to fill in, (opt/0.0f)zBias,centerOffx -1 -> +1,centerOffy -1 -> +1
	Returns 	: 
	Info 		: this replaces bdDrawFlatSprite which doesn't work in 2D projection mode.
					***** PLEASE USE THIS NOW INSTEAD OF 'DRAW2DSPRITE' *****
*/
void utilDraw2DSprite(	TBTexture* const	texture,
						const TBVector2D	pos,
						const float			width,
						float				height		=0.0f,
						const float			ang			=0.0f,
						const COLOUR&		col			=COLOUR(128,128,128,128),
						const bool			uFlip		=false,
						const bool			vFlip		=false,
						float* const		destVerts	=NULL,
						const float			zBias		=0.0f,
						const float			centerOffx	=0.0f,
						const float			centerOffy	=0.0f);


#define CIRCLE_NUM_LINES		100
#define CIRCLE_DEFAULT_COLOUR	COLOUR(128, 128, 128)
#define CIRCLE_DEFAULT_RADIUS	METERTOUNIT(0.2f)

/*	--------------------------------------------------------------------------------
	Function 	: utilDrawSphere
	Purpose 	: draw a sphere
	Parameters 	: pos VEC, (opt/small)radius, (opt/grey) colour
	Returns 	: 
	Info 		: taken from Xed
*/
void utilDrawSphere(const VEC& pos, const float radius=CIRCLE_DEFAULT_RADIUS, const COLOUR& col=CIRCLE_DEFAULT_COLOUR);


/*	--------------------------------------------------------------------------------
	Function 	: utilDrawSparkler
	Purpose 	: draw a funky sparkler thingy
	Parameters 	: pos VEC, (opt/default)radius, (opt/default)colour
	Returns 	: 
	Info 		: taken from Xed
*/

#define SPARKLER_DEFAULT_COLOUR		WHITE
#define SPARKLER_DEFAULT_RADIUS		220.0f

void utilDrawSparkler(const VEC& pos, const float radius=SPARKLER_DEFAULT_RADIUS, const COLOUR& col=SPARKLER_DEFAULT_COLOUR);


/*	--------------------------------------------------------------------------------
	Function 	: utilDrawArrow
	Purpose 	: draw a 3D arrow
	Parameters 	: pos VEC, (opt/straight along Z) ang VEC, (opt/default)length, (opt/default) colour
	Returns 	: 
	Info 		: taken from LUST
*/

#define ARROW_DEFAULT_COLOUR		WHITE
#define ARROW_DEFAULT_ANGLE			ZVEC
#define ARROW_DEFAULT_LENGTH		1100.0f

void utilDrawArrow(const VEC& pos, const VEC& ang=ARROW_DEFAULT_ANGLE, const float length=ARROW_DEFAULT_LENGTH, const COLOUR& col=ARROW_DEFAULT_COLOUR);


/*	--------------------------------------------------------------------------------
	Function 	: utilDrawSelectionMarker
	Purpose 	: draw a LUST-style selection marker
	Parameters 	: pos VEC, (opt/default) radius, (opt/false) feint and flickering
	Returns 	: 
	Info 		: parm1 can't be const cos of babel
*/

#define SELECTIONMARKER_DEFAULT_RADIUS	150.0f

void utilDrawSelectionMarker(VEC& pos, const float radius=SELECTIONMARKER_DEFAULT_RADIUS, const bool feint=false);


/*	--------------------------------------------------------------------------------
	Function 	: utilDrawLine
	Purpose 	: draw a line
	Parameters 	: 3d start pos, 3d end pos, (opt/GREEN)ref to colour at start pos, (opt/NULL=start colour) ptr to colour at end pos
	Returns 	: 
	Info 		: 
*/
void utilDrawLine(const TBVector start, const TBVector end, const COLOUR& startCol=GREEN, const COLOUR* const endCol=NULL);

/*	--------------------------------------------------------------------------------
	Function 	: utilDrawThickLine
	Purpose 	: draw a line of a set thickness
	Parameters 	: 3d start pos, 3d end pos, thickness of line, (opt/NULL=start colour) ptr to colour at end pos
	Returns 	: 
	Info 		: 
*/
void utilDrawThickLine(const TBVector start, const TBVector end, const float thickness, const COLOUR& Col);

/*	--------------------------------------------------------------------------------
	Function 	: utilDrawCircle
	Purpose 	: draw a horizontal circle in 3D space
	Parameters 	: 3D position of centre point, (opt/small)radius, (opt/grey) colour
	Returns 	: 
	Info 		: 
*/
void utilDrawCircle(const TBVector pos, const float radius=CIRCLE_DEFAULT_RADIUS, const COLOUR& col=CIRCLE_DEFAULT_COLOUR);


/*	--------------------------------------------------------------------------------
	Function 	: utilDrawCircle
	Purpose 	: draw a horizontal circle sector in 3D space
	Parameters 	: 3D position of centre point, (opt/0.0f)facing angle, (opt/few metres) radius, (opt/small) field in either direction,
					(opt/grey) colour
	Returns 	: 
	Info 		: 
*/
#define CIRCLE_SECTOR_NUM_LINES			100
#define CIRCLE_SECTOR_DEFAULT_COLOUR	COLOUR(128, 128, 128)
#define CIRCLE_SECTOR_DEFAULT_RADIUS	METERTOUNIT(3)
#define CIRCLE_SECTOR_DEFAULT_FIELD		0.4f
#define CIRCLE_SECTOR_DEFAULT_ANGLE		0.0f

void utilDrawCircleSector(const TBVector	pos,
						  const float		ang=CIRCLE_SECTOR_DEFAULT_ANGLE,
						  const float		radius=CIRCLE_SECTOR_DEFAULT_RADIUS,
						  const float		field=CIRCLE_SECTOR_DEFAULT_FIELD,
						  const COLOUR&		col=CIRCLE_SECTOR_DEFAULT_COLOUR);


/*	--------------------------------------------------------------------------------
	Function 	: utilDrawBoundingBox
	Purpose 	: draw a bounding box
	Parameters 	: (opt/GREEN) colour in which to draw the bounding box
	Returns 	: 
	Info 		: // PP: This is a debug facility, and not guaranteed to be fast, so check
						what it does if you're going to use it in the final game for any reason.
						That applies to all the Util draw functions!
						Hell, in fact it applies to ALL my code! heheh ;o)
*/
void utilDrawBoundingBox(const BOUNDINGBOX& box, const COLOUR& col=BOUNDINGBOX_DEFAULT_COLOUR);

void utilNormaliseQuaternion(TBQuaternion q);

/*	--------------------------------------------------------------------------------
	Function 	: utilGenerateRandomNonZeroVector
	Purpose 	: Turn a number into a formatted text string with ,'s
	Parameters 	: number to convert (integer), string to convert to (char *)
	Returns 	: nowt
	Info 		: If anyone thinks of a better way to do it they're welcome to take a shot
*/

void utilFormatPrettyNumbers(int value, char *string);

void utilGenerateRandomNonZeroVector(TBVector v, float range, int negative);

/*	--------------------------------------------------------------------------------
	Function 	: UtilGenerateRandomNumber
	Purpose 	: Generates a random number between given bounds quickly
	Parameters 	: upper, lower bounds
	Returns 	: the number that you're after
*/

float UtilGenerateRandomNumber(float upperLimit, float lowerLimit);


/*	--------------------------------------------------------------------------------
	Function 	: utilFindStringInArray
	Purpose 	: finds a string in an array of strings
	Parameters 	: array in which to search, number of elements in array, string to find
	Returns 	: array index of string if found, or -1 if not found
	Info 		: 
*/
int utilFindStringInArray(const char *const *const array, const int arraySize, const char* const string);


/*	--------------------------------------------------------------------------------
	Function 	: utilSwap
	Purpose 	: swap two elements of an int32 array
	Parameters	: ptr to array of int32, two indices to swap
	Returns		: 
	Info		: used by utilQuickSort
*/
void utilSwap(int32* const v, const int32 i, const int32 j);


/*	--------------------------------------------------------------------------------
	Function 	: utilQuickSort
	Purpose 	: sort an array of int32's using a quick-sort algorithm
	Parameters	: ptr to array of int32s, left index of array (0), right index of array (arraySize-1)
	Returns		: 
	Info		: sorts into ascending order
	Shoe-horn	: This was such a Hoare to get working
*/
void utilQuickSort(int32* const a, const int32 left, const int32 right);


#endif // PP: ndef __H_UTIL