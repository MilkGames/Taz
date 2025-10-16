// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : fGui.cpp
//   Purpose : Includes GUI component headers. Defines TBGuiInfoStruct, FGUI_MAXCHARS, and declares several GUI-related functions
// Component : Babel Foundation Classes GUI
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : fgInitialise
	Purpose : Initialise the GUI system
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/
int fgInitialise()
{
        bkPrintf("*** WARNING *** fgInitialise was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : fgInitSpecific
	Purpose : Platform specific gui initialisation
	Parameters : 
	Returns : 
	Info : 
*/
int fgInitSpecific()
{
        bkPrintf("*** WARNING *** fgInitSpecific was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : fgShutdown
	Purpose : Shutdown the GUI system
	Parameters : 
	Returns : 
	Info : 
*/
void fgShutdown()
{
        bkPrintf("*** WARNING *** fgShutdown was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : fgShutdownSpecific
	Purpose : Platform specific gui shutdown
	Parameters : 
	Returns : 
	Info : 
*/
void fgShutdownSpecific()
{
        bkPrintf("*** WARNING *** fgShutdownSpecific was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : fgPoll
   Purpose : poll the GUI
   Parameters : 
   Returns : 
   Info : 
*/
void fgPoll()
{
        bkPrintf("*** WARNING *** fgPoll was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : fgRender
   Purpose : render the GUI
   Parameters : 
   Returns : 
   Info : 
*/
void fgRender()
{
        bkPrintf("*** WARNING *** fgRender was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : fgSetFont
   Purpose : set the GUI font
   Parameters : font pointer
   Returns : 
   Info : 
*/
void fgSetFont(TBFont *font)
{
        bkPrintf("*** WARNING *** fgSetFont was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : fgSetPointer
   Purpose : set the GUI pointer texture
   Parameters : texture pointer
   Returns : 
   Info : 
*/
void fgSetPointer(TBTexture *texture)
{
        bkPrintf("*** WARNING *** fgSetPointer was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : fgSetViewport
   Purpose : set the GUI viewport
   Parameters : x1, y1, x2, y2
   Returns : 
   Info : 
*/
void fgSetViewport(int x1, int y1, int x2, int y2)
{
        bkPrintf("*** WARNING *** fgSetViewport was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : fgDrawRect
   Purpose : draw a GUI rectangle
   Parameters : topLeftX, topLeftY, width, height, colour, texture
   Returns : 
   Info : 
*/
void fgDrawRect(int topLeftX, int topLeftY, int width, int height, int colour, TBTexture *texture)
{
        bkPrintf("*** WARNING *** fgDrawRect was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : fgDrawRect2
   Purpose : draw a GUI rectangle with two colours
   Parameters : topLeftX, topLeftY, width, height, colour1, colour2, texture
   Returns : 
   Info : 
*/
void fgDrawRect2(int topLeftX, int topLeftY, int width, int height, int colour1, int colour2, TBTexture *texture)
{
        bkPrintf("*** WARNING *** fgDrawRect2 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : fgDrawLine
   Purpose : draw a GUI line
   Parameters : x1, y1, x2, y2, colour
   Returns : 
   Info : 
*/
void fgDrawLine(int x1, int y1, int x2, int y2, int colour)
{
        bkPrintf("*** WARNING *** fgDrawLine was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : fgDrawText
   Purpose : draw GUI text
   Parameters : text, x, y, width, height, colour
   Returns : 
   Info : 
*/
void fgDrawText(ushort *text, int x, int y, int width, int height, int colour)
{
        bkPrintf("*** WARNING *** fgDrawText was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : fgTextExtent
   Purpose : get GUI text extent
   Parameters : text, width, height
   Returns : 
   Info : 
*/
void fgTextExtent(ushort *text, float *width, float *height)
{
        bkPrintf("*** WARNING *** fgTextExtent was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : fgDrawPointer
   Purpose : draw GUI pointer
   Parameters : 
   Returns : 
   Info : 
*/
void fgDrawPointer()
{
        bkPrintf("*** WARNING *** fgDrawPointer was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : fgColourDelta
   Purpose : get GUI colour delta
   Parameters : colour, delta
   Returns : 
   Info : 
*/
int fgColourDelta(int colour, int delta)
{
        bkPrintf("*** WARNING *** fgColourDelta was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}
