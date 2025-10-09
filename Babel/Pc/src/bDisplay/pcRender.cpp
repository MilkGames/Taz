// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcRender.cpp
//   Purpose : Defines TBRenderStateInfo and TBRenderTarget structs. Declares global render state and render target lists. Provides prototypes for setting render states, default render states, fog parameters, global alpha, creating/deleting/setting render targets, getting render target info, setting render target as texture, and locking/unlocking render states
// Component : Babel Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/* --------------------------------------------------------------------------------
   Function : bdSetRenderState
   Purpose : Set a render state
   Parameters : render state to set, first value, second value
   Returns : 
   Info : 
*/

void bdSetRenderState(uint32 renderState, uint32 value1, uint32 value2)
{
	return;
}


/* --------------------------------------------------------------------------------
   Function : bSetDefaultRenderStates
   Purpose : Set the default render states
   Parameters : 
   Returns : 
   Info : 
*/

void bSetDefaultRenderStates()
{
	return;
}


/* --------------------------------------------------------------------------------
   Function : bdSetFogRange
   Purpose : Set the ranges for distance fog
   Parameters : near distance, far distance
   Returns : 
   Info : 
*/

void bdSetFogRange(float nearDist, float farDist)
{
	return;
}


/* --------------------------------------------------------------------------------
   Function : bdSetFogColour
   Purpose : Set the colour for distance fog
   Parameters : red, green, blue (each 0..255)
   Returns : 
   Info : 
*/

void bdSetFogColour(int red, int green, int blue)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : bSetGlobalAlpha
	Purpose : set the global alpha scale value for actor drawing
	Parameters : new alpha scale value (256=solid)
	Returns : 
	Info : platform specific code called from baSetGlobalAlpha
*/

void bSetGlobalAlpha(int newAlphaScale)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : bdCreateRenderTarget
	Purpose : create an off-screen render target
	Parameters : width in pixels, height in pixels, rgb depth in bits, Z depth in bits, flags (see BCREATERENDERTARGET_)
	Returns : ptr to target or NULL for failure
	Info : 
*/

TBRenderTarget *bdCreateRenderTarget(int width, int height, int rgbBits, int zBits, uint32 flags)
{
	return NULL;
}


/*	--------------------------------------------------------------------------------
	Function : bdDeleteRenderTarget
	Purpose : delete an off-screen render target
	Parameters : ptr to render target or NULL to delete all render targets
	Returns : 
	Info : 
*/

void bdDeleteRenderTarget(TBRenderTarget *target)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : bdSetRenderTarget
	Purpose : set the current render target
	Parameters : ptr to render target or NULL to use default screen, clear colour, clear depth,
																						flags (see BSETRENDERTARGET_)
	Returns : OK/FAIL
	Info : 
*/

int bdSetRenderTarget(TBRenderTarget *target, int r,int g,int b, int a,float depth, uint32 flags)
{
	return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bdGetRenderTargetInfo
	Purpose : return information for a render target
	Parameters : ptr to render target, ptr for width, ptr for height, ptr for RGB depth, ptr for Z depth
	Returns : 
	Info : width, height or depth ptrs can be NULL
*/

void bdGetRenderTargetInfo(TBRenderTarget *target, int *width, int *height, int *rgbDepth, int *zDepth)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : bdSetRenderTargetAsTexture
	Purpose : set a render target as a texture
	Parameters : ptr to render target, zero-based texture stage
	Returns : OK/FAIL
	Info : 
*/

int bdSetRenderTargetAsTexture(TBRenderTarget *target, int stage)
{
	return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bdLockRenderState
	Purpose : increment the lock count on a render state
	Parameters : render state
	Returns : new lock count
	Info : 
*/

int bdLockRenderState(int renderState)
{
	return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bdUnlockRenderState
	Purpose : decrement the lock count on a render state
	Parameters : render state
	Returns : new lock count
	Info : 
*/

int bdUnlockRenderState(int renderState)
{
	return 0;
}