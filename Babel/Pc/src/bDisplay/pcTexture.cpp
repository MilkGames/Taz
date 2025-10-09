// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcTexture.cpp
//   Purpose : Defines macros for texture width/height, texture projector control modes and flags. Defines TBTextureData and _TBTexture structs for texture resources, and _TBTextureProjector for texture projection
// Component : Babel Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bTextureInit
	Purpose : initialise texture manager
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/
int bTextureInit()
{
    return OK; // empty, confirmed
}

/*	--------------------------------------------------------------------------------
	Function : bTextureShutdown
	Purpose : shutdown texture manager
	Parameters : 
	Returns : 
	Info : 
*/
void bTextureShutdown()
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdSetTexture
	Purpose : set texture
	Parameters : stage, handle
	Returns : OK/FAIL
	Info : 
*/
int bdSetTexture(int stage, TBTexture *handle)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bdEnableTextureMatrix
	Purpose : enable texture matrix
	Parameters : stage, type
	Returns : OK/FAIL
	Info : 
*/
int bdEnableTextureMatrix(int stage, EBTextureMatrixType type)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bdDisableTextureMatrix
	Purpose : disable texture matrix
	Parameters : stage
	Returns : OK/FAIL
	Info : 
*/
int bdDisableTextureMatrix(int stage)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bdSetTextureMatrix
	Purpose : set texture matrix
	Parameters : stage, m
	Returns : OK/FAIL
	Info : 
*/
int bdSetTextureMatrix(int stage, const TBMatrix m)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bdSetTextureLinearMatrix
	Purpose : set texture linear matrix
	Parameters : stage, m
	Returns : OK/FAIL
	Info : 
*/
int bdSetTextureLinearMatrix(int stage, const TBMatrix m)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bdDisableTextureLinearMatrix
	Purpose : disable texture linear matrix
	Parameters : stage
	Returns : OK/FAIL
	Info : 
*/
int bdDisableTextureLinearMatrix(int stage)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bDeleteTexture
	Purpose : delete texture
	Parameters : handle
	Returns : 
	Info : 
*/
void bDeleteTexture(TBTexture *handle)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdUnlockTexture
	Purpose : unlock texture
	Parameters : texture, frame, flags
	Returns : 
	Info : 
*/
void bdUnlockTexture(TBTexture *texture, int frame, uint32 flags)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bMakeTextureSurface
	Purpose : make texture surface
	Parameters : handle, xDim, yDim, format, mipLevels, noofFrames
	Returns : OK/FAIL
	Info : 
*/
int bMakeTextureSurface(TBTexture *handle, int xDim, int yDim, int format, int mipLevels, int noofFrames)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bdSetTextureFrame
	Purpose : set texture frame
	Parameters : texturePtr, frameNumber
	Returns : OK/FAIL
	Info : 
*/
int bdSetTextureFrame(TBTexture *texturePtr, int frameNumber)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bGetTextureSurface
	Purpose : get texture surface
	Parameters : texture, level
	Returns : surface
	Info : 
*/
LPDIRECT3DSURFACE8 bGetTextureSurface(LPDIRECT3DTEXTURE8 texture, int level)
{
    return NULL;
}

/*	--------------------------------------------------------------------------------
	Function : bdGetTextureInfo
	Purpose : get texture info
	Parameters : texture, width, height, format, noofFrames, currentFrame
	Returns : 
	Info : 
*/
void bdGetTextureInfo(TBTexture *texture, int *width, int *height, EBTextureFormat *format, int *noofFrames, int *currentFrame)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdSetNoTexture
	Purpose : set no texture
	Parameters : stage
	Returns : 
	Info : 
*/
void bdSetNoTexture(int stage)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bFixupTexture
	Purpose : fixup texture
	Parameters : texture
	Returns : 
	Info : 
*/
void bFixupTexture(TBTexture *texture)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdScrollTexture
	Purpose : scroll texture
	Parameters : sourTexture, destTexture, s, t
	Returns : 
	Info : 
*/
void bdScrollTexture(struct _TBTexture *sourTexture, struct _TBTexture *destTexture, float s, float t)
{
    return;
}
