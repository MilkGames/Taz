// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : xbGui.h
//   Purpose : platform specific GUI code
// Component : Xbox Gui
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BGUI_XBGUI_H__
#define __BGUI_XBGUI_H__

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


#endif		// __BGUI_XBGUI_H__
