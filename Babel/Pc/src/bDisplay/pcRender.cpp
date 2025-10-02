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

/*	--------------------------------------------------------------------------------
	Function : bdSetRenderState
	Purpose : set render state
	Parameters : renderState, value1, value2
	Returns : 
	Info : 
*/
void bdSetRenderState(uint32 renderState, uint32 value1, uint32 value2)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bSetDefaultRenderStates
	Purpose : set default render states
	Parameters : 
	Returns : 
	Info : 
*/
void bSetDefaultRenderStates()
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdSetFogRange
	Purpose : set fog range
	Parameters : nearDist, farDist
	Returns : 
	Info : 
*/
void bdSetFogRange(float nearDist, float farDist)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdSetFogColour
	Purpose : set fog colour
	Parameters : red, green, blue
	Returns : 
	Info : 
*/
void bdSetFogColour(int red, int green, int blue)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bSetGlobalAlpha
	Purpose : set global alpha
	Parameters : newAlphaScale
	Returns : 
	Info : 
*/
void bSetGlobalAlpha(int newAlphaScale)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdDeleteRenderTarget
	Purpose : delete render target
	Parameters : target
	Returns : 
	Info : 
*/
void bdDeleteRenderTarget(TBRenderTarget *target)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdSetRenderTarget
	Purpose : set render target
	Parameters : target, r, g, b, a, depth, flags
	Returns : OK/FAIL
	Info : 
*/
int bdSetRenderTarget(TBRenderTarget *target, int r, int g, int b, int a, float depth, uint32 flags)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bdGetRenderTargetInfo
	Purpose : get render target info
	Parameters : target, width, height, rgbDepth, zDepth
	Returns : 
	Info : 
*/
void bdGetRenderTargetInfo(TBRenderTarget *target, int *width, int *height, int *rgbDepth, int *zDepth)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdSetRenderTargetAsTexture
	Purpose : set render target as texture
	Parameters : target, stage
	Returns : OK/FAIL
	Info : 
*/
int bdSetRenderTargetAsTexture(TBRenderTarget *target, int stage)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bdLockRenderState
	Purpose : lock render state
	Parameters : renderState
	Returns : OK/FAIL
	Info : 
*/
int bdLockRenderState(int renderState)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bdUnlockRenderState
	Purpose : unlock render state
	Parameters : renderState
	Returns : OK/FAIL
	Info : 
*/
int bdUnlockRenderState(int renderState)
{
    return 0;
}
