// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : xbActor.h
//   Purpose : platform specific primary actor functions
// Component : Xbox Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BACTOR_XBOXACTOR_H__
#define __BACTOR_XBOXACTOR_H__

// ********************************************************************************
// Constants and Macros

#define BACTOR_BONES_COLUMNS_IN_MATRIX		4
#define BACTOR_BONES_ROWS_IN_MATRIX			3	// was 4 row matrix
#define BACTOR_BONES_MATRIX_SIZE			(BACTOR_BONES_ROWS_IN_MATRIX*BACTOR_BONES_COLUMNS_IN_MATRIX)


// ********************************************************************************
// Types, Structures and Classes

// type for mesh vertex buffers
typedef D3DVertexBuffer		TBMeshVertexBuffer;

// function pointer type for soft skin batch renderers
typedef void (*TBDrawSoftSkinCallback)(struct _TBActorInstance *actorInstance, struct _TSoftSkinRenderModeState *renderModeState);


// ********************************************************************************
// Globals

extern int						bForceHardSkinning;			// Force use of hard skinning (testing only)


// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : bDrawSoftSkin
	Purpose : draw a soft-skinned character
	Parameters : actor instance ptr
	Returns : 
	Info : 
*/
void bDrawSoftSkin(struct _TBActorInstance *actorInstance, int alpha);


/*	--------------------------------------------------------------------------------
	Function : baFlushVertexShaderCache
	Purpose : flush the vertex shader cache
	Parameters : 
	Returns : 
	Info : 
*/

void baFlushVertexShaderCache();


/*	--------------------------------------------------------------------------------
	Function : baOptimiseActorRenderModes
	Purpose : optimise the render mode setup for the gamecube
	Parameters : actor to optimise the render mode for
	Returns : passes
	Info : 
*/

int baOptimiseActorRenderModes(struct _TBActorInstance *actorInstance);


#endif		// __BACTOR_XBOXACTOR_H__
