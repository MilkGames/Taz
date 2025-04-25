// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : consoleWindow.cpp
//   Purpose : encapsulated console  window
// Component : Generic display functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BDISPLAY_CONSOLEWINDOW_H__
#define __BDISPLAY_CONSOLEWINDOW_H__


// ********************************************************************************
// Constants and Macros


// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bdEnableConsoleWindow
   Purpose : enable / disable the console window
   Parameters : TRUE to enable, FALSE to disable
   Returns : 
   Info : 
*/

void bdEnableConsoleWindow(int enable);


/* --------------------------------------------------------------------------------
   Function : bdConsoleWindowPrintf
   Purpose : printf into console window
   Parameters : printf args
   Returns : 
   Info : 
*/

void bdConsoleWindowPrintf(char *format, ...);


/* --------------------------------------------------------------------------------
   Function : bdResetConsoleWindow
   Purpose : resets the console window
   Parameters : 
   Returns : 
   Info : clears buffers
*/

void bdResetConsoleWindow();


/* --------------------------------------------------------------------------------
   Function : bdFormatConsoleWindow
   Purpose : format the console window
   Parameters : xp, yp, [height (-1 ignore) = -1]
   Returns : 
   Info : origin 0,0 at centre of screen with x+ right and +y up screen
*/

void bdFormatConsoleWindow(int xp, int yp, int height=-1);


/* --------------------------------------------------------------------------------
   Function : bdSetConsoleWindowFont
   Purpose : set font to use
   Parameters : font pointer, [font scaler = 1.0f, r,g,b,a (0-127 or -1 ignore) = -1]
   Returns : 
   Info : 
*/

void bdSetConsoleWindowFont(TBFont *font, float scaler = 1.0f, int r=-1, int g=-1, int b=-1, int a=-1);


/* --------------------------------------------------------------------------------
   Function : bRenderConsoleWindow
   Purpose : render console window into screen
   Parameters : 
   Returns : 
   Info : not called by user
*/

void bRenderConsoleWindow();

#endif // __BDISPLAY_CONSOLEWINDOW_H__
