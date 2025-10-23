// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcImagehlp.h
//   Purpose : debugging images
// Component : PC Kernel
//   Comment : Doesn't exist in Xbox Babel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BKERNEL_PCIMAGEHLP_H__
#define __BKERNEL_PCIMAGEHLP_H__

// ********************************************************************************
// Constants and Macros

// well-known DirectX DLL names we probe for version info
// yep, they're longer, but they look cool!
#define BK_DX_DLL_DDRAW   "DDRAW.DLL"
#define BK_DX_DLL_D3D     "D3D.DLL"
#define BK_DX_DLL_D3D8    "D3D8.DLL"
#define BK_DX_DLL_DINPUT  "DINPUT.DLL"
#define BK_DX_DLL_DSOUND  "DSOUND.DLL"
#define BK_DX_DLL_DMUSIC  "DMUSIC.DLL"
#define BK_DX_DLL_DPLAY   "DPLAY.DLL"

// ********************************************************************************
// Types structures and classes



// ********************************************************************************
// Globals



// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bInitDbgHelp
   Purpose  : Load DbgHelp.dll (next to EXE), resolve functions and initialise
   Parameters : 
   Returns : 
   Info    : Logs status via bkPrintf/SafeDebugPrint
*/

void bInitDbgHelp();


/* --------------------------------------------------------------------------------
   Function : bShutdownDbgHelp
   Purpose  : Shutdown symbol handler and optionally unload DbgHelp.dll
   Parameters : 
   Returns : 
   Info    : 
*/

void bShutdownDbgHelp();

/* --------------------------------------------------------------------------------
   Function : bDumpDXVersions
   Purpose  : Print DirectX runtime + key DLL version info to the debug stream
   Parameters : 
   Returns : 
   Info    : 
*/

void bDumpDXVersions();

/* --------------------------------------------------------------------------------
   Function : bCheckDirectXRuntime
   Purpose  : Probe DirectX runtime by creating minimal components
   Parameters : 
   Returns : Packed version (major<<8 | minor), or error code as used by PC Release
   Info    : 
*/
unsigned long bCheckDirectXRuntime(void);

#endif // __BKERNEL_PCIMAGEHLP_H__