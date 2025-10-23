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
        bkPrintf("*** WARNING *** bTextureShutdown was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bdSetTexture was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bdEnableTextureMatrix was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bdDisableTextureMatrix was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bdSetTextureMatrix was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bdSetTextureLinearMatrix was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bdDisableTextureLinearMatrix was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bDeleteTexture was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bdUnlockTexture was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bMakeTextureSurface was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bdSetTextureFrame was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

/* --------------------------------------------------------------------------------
   Function : bResumeTextures
   Purpose : resume textures
   Parameters : 
   Returns : 
   Info : 
*/

void bResumeTextures()
{
	bkPrintf("*** WARNING *** bResumeTextures was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
}

/* --------------------------------------------------------------------------------
   Function : bSuspendTextures
   Purpose : suspend textures
   Parameters : 
   Returns : 
   Info : 
*/

void bSuspendTextures()
{
	bkPrintf("*** WARNING *** bSuspendTextures was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bGetTextureSurface was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bdGetTextureInfo was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bdSetNoTexture was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bFixupTexture was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bdScrollTexture was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}
