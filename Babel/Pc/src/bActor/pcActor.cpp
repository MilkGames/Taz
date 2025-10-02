// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcActor.cpp
//   Purpose : platform specific primary actor functions
// Component : Xbox Actor
//   Comment : EXPECTED TO BE THE SAME AS xbActor.h, DIRECT REWRITE!
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bDrawSoftSkin
	Purpose : draw a soft-skinned character
	Parameters : actor instance ptr
	Returns : 
	Info : 
*/
void bDrawSoftSkin(struct _TBActorInstance *actorInstance, int alpha)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baFlushVertexShaderCache
	Purpose : flush the vertex shader cache
	Parameters : 
	Returns : 
	Info : 
*/
void baFlushVertexShaderCache()
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baOptimiseActorRenderModes
	Purpose : optimise the render mode setup for the gamecube
	Parameters : actor to optimise the render mode for
	Returns : passes
	Info : 
*/
int baOptimiseActorRenderModes(struct _TBActorInstance *actorInstance)
{
    return 0;
}
