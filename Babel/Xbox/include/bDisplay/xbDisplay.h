// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : xbDisplay.h
//   Purpose : display functions
// Component : Xbox Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BDISPLAY_XBDISPLAY_H__
#define __BDISPLAY_XBDISPLAY_H__

// ********************************************************************************
// Constants and Macros


// macro to safely release an interface
#define RELEASE(PTR) { if (PTR) { while ((PTR)->Release()); (PTR) = NULL; } }

// macro to make a 32bit RGB value from a triple
#define RGB32(r, g, b) ((uint32) (((r) << 16) | ((g) << 8) | (b)))

// macro to make a 32bit RGBA value from a quad
#define RGBA32(r, g, b, a) ((uint32) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b)))

// pixel formats
enum EBDPixelFormat {
	BDPIXFORMAT_555,
	BDPIXFORMAT_565,
	BDPIXFORMAT_24,
	BDPIXFORMAT_32,
};

// D3D memory usage types
typedef enum {
	EBD3DMEMORYUSAGE_VERTEXBUFFER,
	EBD3DMEMORYUSAGE_INDEXBUFFER,
	EBD3DMEMORYUSAGE_TEXTURE,
	EBD3DMEMORYUSAGE_PALETTE
} EBD3DMemoryUsage;


// ********************************************************************************
// Include generic header

#include "display.h"


// ********************************************************************************
// Types, Structures and Classes

// display information container
typedef struct _TBDisplayInfo {
	uint32					flags;					// display flags
	int						xRes, yRes;				// screen dimensions
	int						xResHalf, yResHalf;		// half screen dimensions (width/2,height/2)
	int						bpp, zDepth;			// bit depth/Z depth (should always be 32:24)
	int						stencilDepth;			// depth of stencil buffer (should always be 8)
	float					xAspect, yAspect;		// 3D projection aspect scales
	int						tnlHardware;			// TRUE if running on a TnL hardware card
	int						tnlActive;				// TRUE if we are currently running in TnL mode
	int						hwVertexShaders;		// TRUe if hardware vertex shaders are available
	int						pixelFormat;			// screen pixel format (see BDPIXFORMAT_)
	int						started;				// is the display started?
	int						linearTexture[8];		// 'using linear texture on stage' flags
	struct _TBRenderTarget	*curRenderTarget;		// current render target
	int						inScene;				// TRUE if within BeginScene/EndScene pair
	int						xAAMul, yAAMul;			// anti-alias multipliers
	D3DMULTISAMPLE_TYPE		aaType;					// anti-alias type

	LPDIRECT3D8				d3d;					// Direct3D object
	LPDIRECT3DDEVICE8		d3dDevice;				// Direct3D device
	LPDIRECT3DSURFACE8		backBuffer;				// back buffer surface
	LPDIRECT3DSURFACE8		depthStencilBuffer;		// depth/stencil surface
	LPDIRECT3DSURFACE8		frontBuffer;			// front buffer

	int						noofPolysProcessed;		// number of triangles drawn this frame
} TBDisplayInfo;


// Babel Vsync callback function
typedef void (*TBVsyncCallback)(void *context);


// Pre flip (post anti aliasing) callback)
typedef int (*TBPreFlipCallback)(void *context);


// ********************************************************************************
// Globals

extern TBDisplayInfo	bDisplayInfo;		// display information container
extern uint				bRColLUT[256];		// colour lookup for Red
extern uint				bGColLUT[256];		// colour lookup for Green
extern uint				bBColLUT[256];		// colour lookup for Blue
extern uint				bAColLUT[256];		// colour lookup for Alpha
extern uint				bRGBInvColLUT[256];	// inverse colour lookup for Red,Green and Blue
extern uint				bAInvColLUT[256];	// inverse colour lookup for Alpha
extern float			bRGBNormLUT[256];	// colour lookup to normalise 0..127 into 0..1
extern uint				bFlipCount;			// the number of calls to bdFlip since the program started.
extern TBTimerValue		bLastFrameDuration;	// duration of last frame
extern float			bFPS;				// last average frame rate


// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bInitDisplay
   Purpose : initialise the display module
   Parameters : 
   Returns : OK/FAIL
   Info : 
*/

int bInitDisplay();


/* --------------------------------------------------------------------------------
   Function : bShutdownDisplay
   Purpose : shutdown the display module
   Parameters : 
   Returns : 
   Info : 
*/

void bShutdownDisplay();


/* --------------------------------------------------------------------------------
   Function : bdOpenDisplay
   Purpose : setup the display for rendering
   Parameters : xRes, yRes, bit depth, Z buffer depth, flags
   Returns : OK/FAIL
   Info : 
*/

int bdOpenDisplay(int xRes, int yRes, int bpp, int zDepth, uint32 flags);


/* --------------------------------------------------------------------------------
   Function : bdCloseDisplay
   Purpose : shutdown the open display
   Parameters : 
   Returns : 
   Info : 
*/

void bdCloseDisplay();


/* --------------------------------------------------------------------------------
   Function : bdGetDisplayInfo
   Purpose : retrieve display parameters
   Parameters : ptr for xRes, ptr for yRes, ptr for bpp, ptr for zDepth
   Returns : 
   Info : 
*/

void bdGetDisplayInfo(int *xRes, int *yRes, int *bpp, int *zDepth);


/* --------------------------------------------------------------------------------
   Function : bdSetVsyncCallback
   Purpose : Set a callback function to be called once per Vsync
   Parameters : callback function, context
   Returns : OK/FAIL
   Info : 
*/

int bdSetVsyncCallback(TBVsyncCallback callback, void *context);


/* --------------------------------------------------------------------------------
   Function : bdSetPreFlipCallback
   Purpose : Set callback just before the flip - and after the anti aliasing
   Parameters : callback function, user context
   Returns : OK/FAIL
   Info : 
*/

int bdSetPreFlipCallback(TBPreFlipCallback callback, void *context);


/* --------------------------------------------------------------------------------
   Function : bdFlip
   Purpose : page-flip the display and clear the viewport
   Parameters : colour for viewport clearing, update flags
   Returns : 
   Info : 
*/

void bdFlip(int red, int green, int blue, int alpha, int flags=BFLIPFLAG_UPDATE);


/* --------------------------------------------------------------------------------
   Function : bdBeginScene
   Purpose : prepare for rendering
   Parameters : 
   Returns : OK/FAIL
   Info : 
*/

int bdBeginScene();


/* --------------------------------------------------------------------------------
   Function : bdEndScene
   Purpose : finish rendering
   Parameters : 
   Returns : 
   Info : 
*/

void bdEndScene();


/* --------------------------------------------------------------------------------
   Function : bdGetScanline
   Purpose : return the current scanline number
   Parameters : 
   Returns : scanline number or -1 for failure
   Info : 
*/

int bdGetScanline();


/* --------------------------------------------------------------------------------
   Function : bdScreenshot
   Purpose : save a screenshot as a BMP file
   Parameters : filename relative to home directory
   Returns : 1/0
   Info : 
*/

int bdScreenshot(char *filename);


/* --------------------------------------------------------------------------------
   Function : bRestoreAllSurfaces
   Purpose : attempt to restore all our DD surfaces
   Parameters : 
   Returns : 
   Info : 
*/

void bRestoreAllSurfaces();


/* --------------------------------------------------------------------------------
   Function : bStartDisplay
   Purpose : try to startup the display for the current size/bpp/z/mode
   Parameters : 
   Returns : OK/FAIL
   Info : 
*/

int bStartDisplay();


/* --------------------------------------------------------------------------------
   Function : bStopDisplay
   Purpose : cleanup the display
   Parameters : in shutdown mode?
   Returns : 
   Info : 
*/

void bStopDisplay(int shutdown);


/* --------------------------------------------------------------------------------
   Function : bGrabScreen
   Purpose : grab the back-buffer as a 24bit RGB buffer
   Parameters : 
   Returns : 
   Info : 
*/

uchar *bGrabScreen();


/* --------------------------------------------------------------------------------
   Function : bLockSurface
   Purpose : lock a surface returning it's parameters
   Parameters : surface, ptrs for width, height, bit depth, 555 flag, pitch
   Returns : ptr to surface data or NULL for failure
   Info : 
*/

void *bLockSurface(LPDIRECT3DSURFACE8 surf, int *xDim, int *yDim, int *bpp, int *rgb555, int *pitch);


/* --------------------------------------------------------------------------------
   Function : bUnlockSurface
   Purpose : unlock a surface
   Parameters : surface
   Returns : 
   Info : 
*/

void bUnlockSurface(LPDIRECT3DSURFACE8 surf);


/* --------------------------------------------------------------------------------
   Function : bdSetScreenOffset
   Purpose : set screen x/y offset
   Parameters : x offset , y offset
   Returns : 
   Info : no effect yet on Xbox
*/

void bdSetScreenOffset(int x, int y);


/* --------------------------------------------------------------------------------
   Function : bdClear
   Purpose : clear the current clip rectangle of the current viewport
   Parameters : colour for viewport clearing, depth for Z clearing, value for stencil clearing, flags (see BDCLEARMODE_)
   Returns : 
   Info : 
*/

void bdClear(int red, int green, int blue, int alpha, float z, int32 stencil, uint32 flags);


/* --------------------------------------------------------------------------------
   Function : bGetDisplayFrequency
   Purpose : return the display frequency in Hz
   Parameters : 
   Returns : display frequency in Hz
   Info : 
*/

int bGetDisplayFrequency();


/* --------------------------------------------------------------------------------
   Function : bdDoDummyPresent
   Purpose : perform a dummy present call to retrieve the memory reserved by a previous applications PersistDisplay call
   Parameters : 
   Returns : 
   Info : this call *must* only be used before bdOpenDisplay
*/

void bdDoDummyPresent();

#endif		// __BDISPLAY_XBDISPLAY_H__
