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
	uint32 savedZTest;          // 0x00: bRenderState.renderState[ZTEST][0]
	uint32 savedTextureFilter;  // 0x04: bRenderState.renderState[TEXTUREFILTER][0]
	uint32 savedAlphaEnable;    // 0x08: bRenderState.renderState[ALPHAENABLE][0]
	uint32 savedAlphaBlendMode; // 0x0C: bRenderState.renderState[ALPHABLENDMODE][0]
	int32  savedProjectionMode; // 0x10: bViewInfo.projectionMode
	int32  locked;              // 0x14: "already locked?" guard
} TBGuiHALInfo; // sizeof(TBGuiHALInfo) == 0x18


// ********************************************************************************
// Globals

extern TBGuiHALInfo		bGuiHalInfo;		// platform specific Gui info


// ********************************************************************************
// Prototypes


#endif		// __BGUI_PCGUI_H__
