// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : fGui.h
//   Purpose : include all Gui component headers
// Component : BFC Gui
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __FGUI_H__
#define __FGUI_H__

// ********************************************************************************
// Types, Structures and Classes

// forward declaration of the object base class, so we can typedef the callback
class CBGuiObject;

// callback function
typedef void (*TBGuiCallback)(int iId, CBGuiObject *pObject);


// ********************************************************************************
// include all required headers

#include "fGui/CBGuiObject.h"
#include "fGui/CBGuiScrollBar.h"
#include "fGui/CBGuiWindow.h"
#include "fGui/CBGuiButton.h"
#include "fGui/CBGuiMenuItem.h"
#include "fGui/CBGuiMenu.h"


// ********************************************************************************
// Types, Structures and Classes

typedef struct _TBGuiInfoStruct {
	
	// gui is active/not active
	int				active;
	
	// our main "desktop" window, doesn't actually get drawn, but all top level objects get added as children
	// of this object
	CBGuiWindow		*systemWindow;

	// the font which we will use to render our gui text, also defines the size of title bars etc.
	TBFont			*systemFont;

	// pointer texture
	TBTexture		*systemPointer;

	// window background texture
	TBTexture		*windowBackground;	

	// pointer x & y co-ords
	int				pointerX, pointerY;
	float			fPointerX, fPointerY;

	// gui input map
	TBIChannelMap	*inputMap;

	// input channels
	TBIInputChannel	*inputXAxis, *inputYAxis, *inputClick, *inputClickDebounced;

	// default object, text and title bar colours
	int				defaultObjectColour, defaultObjectTextColour;
	int				defaultTitleColour[2], defaultTitleTextColour;
	int				defaultTitleColour2[2], defaultTitleTextColour2;

	// height of font in pixels, defines size of default widgets, like scroll bars and resize button
	int				textHeight;

	CBGuiObject		*lastObjectClicked;

} TBGuiInfoStruct;


// ********************************************************************************
// Globals

extern TBGuiInfoStruct fGuiInfo;



#define FGUI_MAXCHARS 1024


// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : fgInitialise
	Purpose : Initialise the GUI system
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/

int fgInitialise();


/*	--------------------------------------------------------------------------------
	Function : fgInitSpecific
	Purpose : Platform specific gui initialisation
	Parameters : 
	Returns : 
	Info : 
*/

int fgInitSpecific();


/*	--------------------------------------------------------------------------------
	Function : fgShutdown
	Purpose : Shutdown the GUI system
	Parameters : 
	Returns : 
	Info : 
*/

void fgShutdown();


/*	--------------------------------------------------------------------------------
	Function : fgShutdownSpecific
	Purpose : Platform specific gui shutdown
	Parameters : 
	Returns : 
	Info : 
*/

void fgShutdownSpecific();


/* --------------------------------------------------------------------------------
   Function : fgPoll
   Purpose : poll the GUI
   Parameters : 
   Returns : 
   Info : 
*/

void fgPoll();


/*	--------------------------------------------------------------------------------
	Function : fgRender
	Purpose : Render the GUI
	Parameters : 
	Returns : 
	Info : 
*/

void fgRender();


/*	--------------------------------------------------------------------------------
	Function : fgSetFont
	Purpose : Set font for the gui to use
	Parameters : Pointer to font resource
	Returns : 
	Info : 
*/

void fgSetFont(TBFont *font);


/*	--------------------------------------------------------------------------------
	Function : fgSetPointer
	Purpose : Set texture for the gui to use as pointer
	Parameters : Pointer to texture
	Returns : 
	Info : 
*/

void fgSetPointer(TBTexture *texture);


/*	--------------------------------------------------------------------------------
	Function : fgSetViewport
	Purpose : Set the drawing area
	Parameters : left, top, right, bottom
	Returns : 
	Info : 
*/

void fgSetViewport(int x1, int y1, int x2, int y2);


/*	--------------------------------------------------------------------------------
	Function : fgDrawRect
	Purpose : Draw rectangle
	Parameters : top left x, top left y, width, height, colour, texture
	Returns : 
	Info : 
*/

void fgDrawRect(int topLeftX, int topLeftY, int width, int height, int colour, TBTexture *texture);


/*	--------------------------------------------------------------------------------
	Function : fgDrawRect2
	Purpose : Draw rectangle
	Parameters : top left x, top left y, width, height, colour1, colour 2, texture
	Returns : 
	Info : 
*/

void fgDrawRect2(int topLeftX, int topLeftY, int width, int height, int colour1, int colour2, TBTexture *texture);


/*	--------------------------------------------------------------------------------
	Function : fgDrawLine
	Purpose : Draw a line
	Parameters : start x, start y, end x, end y, colour
	Returns : 
	Info : 
*/

void fgDrawLine(int x1, int y1, int x2, int y2, int colour);


/*	--------------------------------------------------------------------------------
	Function : fgDrawText
	Purpose : Draw text
	Parameters : text string, x, y, width, height, colour
	Returns : 
	Info : 
*/

void fgDrawText(ushort *text, int x, int y, int width, int height, int colour);


/* --------------------------------------------------------------------------------
   Function : fgTextExtent
   Purpose : determine the extent of a string printed in a particular font
   Parameters : 16bit string, ptr to receive width, ptr to receive height
   Returns : 
   Info : 
*/

void fgTextExtent(ushort *text, float *width, float *height);


/* --------------------------------------------------------------------------------
   Function : fgDrawPointer
   Purpose : Draw our "mouse pointer"
   Parameters : 
   Returns : 
   Info : 
*/

void fgDrawPointer();


/*	--------------------------------------------------------------------------------
	Function : fgColourDelta
	Purpose : Calculate proportion of a colour
	Parameters : packed colour, delta (0 - 256 = 0.0 - 1.0, etc.)
	Returns : new colour
	Info : 
*/

int fgColourDelta(int colour, int delta);


#endif		//__FGUI_H__
