// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : xbDebug.h
//   Purpose : debugging functions
// Component : Xbox Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BKERNEL_XBDEBUG_H__
#define __BKERNEL_XBDEBUG_H__

// ********************************************************************************
// Constants and Macros

#ifdef _DEBUG
	#ifndef __BASSERT_H__		// For faster compilation
	#include "bassert.h"		// For bkBreakDisable
	#endif

	#define BSOFTBREAK()	{ \
		if (bBreakDisable) bError = BASSERT_SOFTBREAK; \
		else DebugBreak(); \
	}		// macro to throw control into the debugger
#else
	#define BSOFTBREAK()	DebugBreak()		// macro to throw control into the debugger

	// this is needed on the PC as the run-time headers do not define a dummy for it in release builds
	#define DXGetErrorString8A(X)
#endif		// #ifdef _DEBUG


// ********************************************************************************
// Types structures and classes

// information on a debug output stream
typedef struct _TBDebugStream {
	char			logFile[256];								// filename of log file
	uint32			flags;										// flags (see BDEBUGSTREAMFLAG_)
} TBDebugStream;


// ********************************************************************************
// Globals

extern TBDebugStream	*bCurrentDebugStream;		// current debug stream
extern int				bDebuggerPresent;			// 1=running under debugger, 0=not running under debugger
extern int				bPrintPause;				// time in milliseconds to pause between each print statement


// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bInitDebug
   Purpose : initialise the debugging system
   Parameters : 
   Returns : 
   Info : 
*/

void bInitDebug();


/* --------------------------------------------------------------------------------
   Function : bShutdownDebug
   Purpose : shutdown the debugging system
   Parameters : 
   Returns : 
   Info : 
*/

void bShutdownDebug();


/* --------------------------------------------------------------------------------
   Function : bkAlert
   Purpose : display an error message to the user
   Parameters : message
   Returns : 
   Info : 
*/

void bkAlert(char *message);


/* --------------------------------------------------------------------------------
   Function : bVPrintf
   Purpose : print a string to the current debug stream
   Parameters : format string, argument list pointer
   Returns : 
   Info : 
*/

void bVPrintf(char *format, va_list argp);

#endif		// __BKERNEL_XBDEBUG_H__
