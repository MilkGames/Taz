// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : guiHal.h
//   Purpose : platform specific HAL interface declaration
// Component : Generic Gui
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BGUI_GUIHAL_H__
#define __BGUI_GUIHAL_H__

// ********************************************************************************
// Constants and Macros


// ********************************************************************************
// Types, Structures and Classes


// ********************************************************************************
// Globals


// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bGuiLockScreen
   Purpose : lock the screen for gui access
   Parameters : 
   Returns : 
   Info : 
*/

void bGuiLockScreen();


/* --------------------------------------------------------------------------------
   Function : bGuiUnlockScreen
   Purpose : unlock the screen for gui access
   Parameters : 
   Returns : 
   Info : 
*/

void bGuiUnlockScreen();


/* --------------------------------------------------------------------------------
   Function : bGuiPrintText
   Purpose : print some text
   Parameters : font, xpos, ypos, text
   Returns : 
   Info : 
*/

void bGuiPrintText(struct _TBGuiFont *font, int xPos, int yPos, char *text);


/* --------------------------------------------------------------------------------
   Function : bInitGuiHAL
   Purpose : initialise hardware specific GUI
   Parameters : 
   Returns : 
   Info : 
*/

void bInitGuiHAL();


/* --------------------------------------------------------------------------------
   Function : bShutdownGuiHAL
   Purpose : shutdown hardware specific GUI
   Parameters : 
   Returns : 
   Info : 
*/

void bShutdownGuiHAL();


/* --------------------------------------------------------------------------------
   Function : bGuiFillRect
   Purpose : fill a rectangle with a colour
   Parameters : pos, size, colour
   Returns : 
   Info : 
*/

void bGuiFillRect(int xPos,int yPos, int width, int height, int r, int g, int b);


/* --------------------------------------------------------------------------------
   Function : bGuiDrawFrame
   Purpose : draw a GUI frame
   Parameters : pos, size, style, base colour
   Returns : 
   Info : 
*/

void bGuiDrawFrame(int xPos,int yPos, int width, int height, enum EBGuiFrameStyle style, int r, int g, int b);


/* --------------------------------------------------------------------------------
   Function : bGuiDrawHBar
   Purpose : draw a horizontal bar
   Parameters : pos, width, base colour
   Returns : 
   Info : 
*/

void bGuiDrawHBar(int xPos,int yPos, int width, int r, int g, int b);


/* --------------------------------------------------------------------------------
   Function : bGuiDrawVBar
   Purpose : draw a vertical bar
   Parameters : pos, height, base colour
   Returns : 
   Info : 
*/

void bGuiDrawVBar(int xPos,int yPos, int height, int r, int g, int b);


/* --------------------------------------------------------------------------------
   Function : bGuiPrintIcon
   Purpose : print a GUI icon
   Parameters : icon ID, xpos, ypos
   Returns : 
   Info : 
*/

void bGuiPrintIcon(enum EBGuiIcons icon, int xPos, int yPos);

#endif		// __BGUI_GUIHAL_H__
