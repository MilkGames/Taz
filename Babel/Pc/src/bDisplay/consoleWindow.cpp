// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : consoleWindow.cpp
//   Purpose : Defines functions for managing a console window, including enabling/disabling, printing, resetting, formatting, setting font, and showing/hiding
// Component : Babel Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Locals

int stopSpam4 = 0;
int stopSpam8 = 0;

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bdEnableConsoleWindow
	Purpose : enable/disable console window
	Parameters : enable
	Returns : 
	Info : 
*/
void bdEnableConsoleWindow(int enable)
{
        bkPrintf("*** WARNING *** bdEnableConsoleWindow was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdConsoleWindowPrintf
	Purpose : print to console window
	Parameters : format, ...
	Returns : 
	Info : 
*/
void bdConsoleWindowPrintf(char *format, ...)
{
        //bkPrintf("*** WARNING *** bdConsoleWindowPrintf was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdResetConsoleWindow
	Purpose : reset console window
	Parameters : 
	Returns : 
	Info : 
*/
void bdResetConsoleWindow()
{
        bkPrintf("*** WARNING *** bdResetConsoleWindow was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdFormatConsoleWindow
	Purpose : format console window
	Parameters : xp, yp, height
	Returns : 
	Info : 
*/
void bdFormatConsoleWindow(int xp, int yp, int height)
{
        bkPrintf("*** WARNING *** bdFormatConsoleWindow was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdSetConsoleWindowFont
	Purpose : set console window font
	Parameters : font, scaler, r, g, b, a
	Returns : 
	Info : 
*/
void bdSetConsoleWindowFont(TBFont *font, float scaler, int r, int g, int b, int a)
{
        bkPrintf("*** WARNING *** bdSetConsoleWindowFont was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdShowConsoleWindow
	Purpose : show/hide console window
	Parameters : show
	Returns : 
	Info : 
*/
void bdShowConsoleWindow(int show)
{
        bkPrintf("*** WARNING *** bdShowConsoleWindow was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bRenderConsoleWindow
	Purpose : render console window
	Parameters : 
	Returns : 
	Info : 
*/
void bRenderConsoleWindow()
{
	if (!stopSpam4) {
        bkPrintf("*** WARNING *** bRenderConsoleWindow was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
		stopSpam4++;
	}
    return;
}
