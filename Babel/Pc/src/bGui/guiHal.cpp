// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : guiHal.cpp
//   Purpose : Declares platform-specific GUI HAL functions for locking/unlocking the screen, printing text, initializing/shutting down the HAL, filling rectangles, drawing frames, horizontal/vertical bars, and printing icons
// Component : Babel GUI
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>
//#include "../../include/bGui/defFont.h"

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bGuiLockScreen
	Purpose : lock GUI screen
	Parameters : 
	Returns : 
	Info : 
*/
void bGuiLockScreen()
{
        bkPrintf("*** WARNING *** bGuiLockScreen was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bGuiUnlockScreen
	Purpose : unlock GUI screen
	Parameters : 
	Returns : 
	Info : 
*/
void bGuiUnlockScreen()
{
        bkPrintf("*** WARNING *** bGuiUnlockScreen was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bGuiPrintText
	Purpose : print GUI text
	Parameters : font, xPos, yPos, text
	Returns : 
	Info : 
*/
void bGuiPrintText(struct _TBGuiFont *font, int xPos, int yPos, char *text)
{
        bkPrintf("*** WARNING *** bGuiPrintText was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bInitGuiHAL
	Purpose : initialise GUI HAL
	Parameters : 
	Returns : 
	Info : 
*/
void bInitGuiHAL()
{
        bkPrintf("*** WARNING *** bInitGuiHAL was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bShutdownGuiHAL
	Purpose : shutdown GUI HAL
	Parameters : 
	Returns : 
	Info : 
*/
void bShutdownGuiHAL()
{
        bkPrintf("*** WARNING *** bShutdownGuiHAL was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bGuiFillRect
	Purpose : fill GUI rectangle
	Parameters : xPos, yPos, width, height, r, g, b
	Returns : 
	Info : 
*/
void bGuiFillRect(int xPos, int yPos, int width, int height, int r, int g, int b)
{
        bkPrintf("*** WARNING *** bGuiFillRect was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bGuiDrawFrame
	Purpose : draw GUI frame
	Parameters : xPos, yPos, width, height, style, r, g, b
	Returns : 
	Info : 
*/
void bGuiDrawFrame(int xPos, int yPos, int width, int height, enum EBGuiFrameStyle style, int r, int g, int b)
{
        bkPrintf("*** WARNING *** bGuiDrawFrame was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bGuiDrawHBar
	Purpose : draw GUI horizontal bar
	Parameters : xPos, yPos, width, r, g, b
	Returns : 
	Info : 
*/
void bGuiDrawHBar(int xPos, int yPos, int width, int r, int g, int b)
{
        bkPrintf("*** WARNING *** bGuiDrawHBar was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bGuiDrawVBar
	Purpose : draw GUI vertical bar
	Parameters : xPos, yPos, height, r, g, b
	Returns : 
	Info : 
*/
void bGuiDrawVBar(int xPos, int yPos, int height, int r, int g, int b)
{
        bkPrintf("*** WARNING *** bGuiDrawVBar was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bGuiPrintIcon
	Purpose : print GUI icon
	Parameters : icon, xPos, yPos
	Returns : 
	Info : 
*/
void bGuiPrintIcon(enum EBGuiIcons icon, int xPos, int yPos)
{
        bkPrintf("*** WARNING *** bGuiPrintIcon was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}
