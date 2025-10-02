// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcDisplay.cpp
//   Purpose : Contains PC/Xbox specific display functions. Includes DirectX headers, defines macros for D3DDevice functions, pixel formats, and D3D memory usage types
// Component : Babel Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/* --------------------------------------------------------------------------------
   Function : bInitDisplay
   Purpose : initialise the display module
   Parameters : 
   Returns : OK/FAIL
   Info : 
*/
int bInitDisplay()
{
    return 0;
}

/* --------------------------------------------------------------------------------
   Function : bShutdownDisplay
   Purpose : shutdown the display module
   Parameters : 
   Returns : 
   Info : 
*/
void bShutdownDisplay()
{
    return;
}

/* --------------------------------------------------------------------------------
   Function : bdOpenDisplay
   Purpose : setup the display for rendering
   Parameters : xRes, yRes, bit depth, Z buffer depth, flags
   Returns : OK/FAIL
   Info : 
*/
int bdOpenDisplay(int xRes, int yRes, int bpp, int zDepth, uint32 flags)
{
    return 0;
}

/* --------------------------------------------------------------------------------
   Function : bdCloseDisplay
   Purpose : shutdown the open display
   Parameters : 
   Returns : 
   Info : 
*/
void bdCloseDisplay()
{
    return;
}

/* --------------------------------------------------------------------------------
   Function : bdGetDisplayInfo
   Purpose : retrieve display parameters
   Parameters : xRes, yRes, bpp, zDepth
   Returns : 
   Info : 
*/
void bdGetDisplayInfo(int *xRes, int *yRes, int *bpp, int *zDepth)
{
    return;
}

/* --------------------------------------------------------------------------------
   Function : bdSetVsyncCallback
   Purpose : set vsync callback
   Parameters : callback, context
   Returns : OK/FAIL
   Info : 
*/
int bdSetVsyncCallback(TBVsyncCallback callback, void *context)
{
    return 0;
}

/* --------------------------------------------------------------------------------
   Function : bdSetPreFlipCallback
   Purpose : set pre-flip callback
   Parameters : callback, context
   Returns : OK/FAIL
   Info : 
*/
int bdSetPreFlipCallback(TBPreFlipCallback callback, void *context)
{
    return 0;
}

/* --------------------------------------------------------------------------------
   Function : bdFlip
   Purpose : flip the display
   Parameters : red, green, blue, alpha, flags
   Returns : 
   Info : 
*/
void bdFlip(int red, int green, int blue, int alpha, int flags)
{
    return;
}

/* --------------------------------------------------------------------------------
   Function : bdBeginScene
   Purpose : begin scene
   Parameters : 
   Returns : OK/FAIL
   Info : 
*/
int bdBeginScene()
{
    return 0;
}

/* --------------------------------------------------------------------------------
   Function : bdEndScene
   Purpose : end scene
   Parameters : 
   Returns : 
   Info : 
*/
void bdEndScene()
{
    return;
}

/* --------------------------------------------------------------------------------
   Function : bdGetScanline
   Purpose : get scanline
   Parameters : 
   Returns : scanline
   Info : 
*/
int bdGetScanline()
{
    return 0;
}

/* --------------------------------------------------------------------------------
   Function : bdScreenshot
   Purpose : take screenshot
   Parameters : filename
   Returns : OK/FAIL
   Info : 
*/
int bdScreenshot(char *filename)
{
    return 0;
}

/* --------------------------------------------------------------------------------
   Function : bRestoreAllSurfaces
   Purpose : restore all surfaces
   Parameters : 
   Returns : 
  Info : 
*/
void bRestoreAllSurfaces()
{
    return;
}

/* --------------------------------------------------------------------------------
   Function : bStartDisplay
   Purpose : start display
   Parameters : 
   Returns : OK/FAIL
   Info : 
*/
int bStartDisplay()
{
    return 0;
}

/* --------------------------------------------------------------------------------
   Function : bStopDisplay
   Purpose : stop display
   Parameters : shutdown
   Returns : 
   Info : 
*/
void bStopDisplay(int shutdown)
{
    return;
}

/* --------------------------------------------------------------------------------
   Function : bGrabScreen
   Purpose : grab screen
   Parameters : 
   Returns : screen data
   Info : 
*/
uchar *bGrabScreen()
{
    return NULL;
}

/* --------------------------------------------------------------------------------
   Function : bLockSurface
   Purpose : lock surface
   Parameters : surf, xDim, yDim, bpp, rgb555, pitch
   Returns : surface data
   Info : 
*/
void *bLockSurface(LPDIRECT3DSURFACE8 surf, int *xDim, int *yDim, int *bpp, int *rgb555, int *pitch)
{
    return NULL;
}

/* --------------------------------------------------------------------------------
   Function : bUnlockSurface
   Purpose : unlock surface
   Parameters : surf
   Returns : 
   Info : 
*/
void bUnlockSurface(LPDIRECT3DTEXTURE8 surf)
{
    return;
}

/* --------------------------------------------------------------------------------
   Function : bdSetScreenOffset
   Purpose : set screen offset
   Parameters : x, y
   Returns : 
   Info : 
*/
void bdSetScreenOffset(int x, int y)
{
    return;
}

/* --------------------------------------------------------------------------------
   Function : bdClear
   Purpose : clear display
   Parameters : red, green, blue, alpha, z, stencil, flags
   Returns : 
   Info : 
*/
void bdClear(int red, int green, int blue, int alpha, float z, int32 stencil, uint32 flags)
{
    return;
}

/* --------------------------------------------------------------------------------
   Function : bGetDisplayFrequency
   Purpose : get display frequency
   Parameters : 
   Returns : frequency
   Info : 
*/
int bGetDisplayFrequency()
{
    return 0;
}

/* --------------------------------------------------------------------------------
   Function : bdDoDummyPresent
   Purpose : do dummy present
   Parameters : 
   Returns : 
   Info : 
*/
void bdDoDummyPresent()
{
    return;
}
