// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcGui.h
//   Purpose : platform specific GUI code
// Component : Xbox Gui
//   Comment : EXPECTED TO BE THE SAME AS xbGui.h, DIRECT REWRITE!
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BGUI_PCGUI_H__
#define __BGUI_PCGUI_H__

// ********************************************************************************
// Constants and Macros


// ********************************************************************************
// Types, Structures and Classes

// platform specific GUI info container
typedef struct _TBGuiHALInfo {
	uint32			*lpSurface;						// ptr to surface memory
	int				lPitch;							// screen pitch/4
} TBGuiHALInfo;


// ********************************************************************************
// Globals

extern TBGuiHALInfo		bGuiHalInfo;		// platform specific Gui info


// ********************************************************************************
// Prototypes


#endif		// __BGUI_PCGUI_H__
