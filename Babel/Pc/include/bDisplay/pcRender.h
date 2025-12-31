// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcRender.h
//   Purpose : render state functionality
// Component : Xbox Display
//   Comment : EXPECTED TO BE THE SAME AS xbRender.h, DIRECT REWRITE!
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BDISPLAY_PCRENDER_H__
#define __BDISPLAY_PCRENDER_H__

// ********************************************************************************
// Constants and Macros

// max simultaneous texture stages
#define BMAX_TEXTURE_STAGES	2


// ********************************************************************************
// Types, Structures and Classes

// information on which render states are valid
typedef struct _TBRenderStateInfo {
	struct _TBTexture *currentTexture[BMAX_TEXTURE_STAGES];	  // current texture per stage               [0x00] (2 * 4 = 8)

	float    fogNear;                                         // fog near                                [0x08]
	float    fogFar;                                          // fog far                                 [0x0C]
	float    fogRange;                                        // fog range (derived or cached)           [0x10]
	int      rFog;                                            // fog colour R (0..255)                   [0x14]
	int      gFog;                                            // fog colour G (0..255)                   [0x18]
	int      bFog;                                            // fog colour B (0..255)                   [0x1C]

	union {                                                   // current render state values             [0x20]
		uint32 renderState[BDRENDERSTATE_NOOF][2];            //   lo/hi 32-bit components               (0x20..0x97)
		uint64 renderState64[BDRENDERSTATE_NOOF];             //   combined 64-bit view
	};                                                        //   size: 0x78 bytes

	union {                                                   // render state stack                      [0x98]
		uint32 stateStack[BDRENDERSTATE_NOOF]
		                  [BRENDERSTATE_STACKDEPTH][2];       //   per-state stack (lo/hi 32-bit)
		uint64 stateStack64[BDRENDERSTATE_NOOF]
		                    [BRENDERSTATE_STACKDEPTH];        //   combined 64-bit view
	};                                                        //   size: 0x780 bytes  (0x98..0x817)

	int32    stateStackDepth[BDRENDERSTATE_NOOF];             // depth of stacks per render state        [0x818] (15 * 4 = 0x3C)
	int32    textureEnable[BMAX_TEXTURE_STAGES];              // current texture enable states per stage [0x854] (2 * 4 = 8)
	int32    renderStateLock[BDRENDERSTATE_NOOF];             // lock count on render states             [0x85C] (15 * 4 = 0x3C)
	int      forceTextureStage[BMAX_TEXTURE_STAGES];          // ref-counts to force actor texture usage [0x898] (2 * 4 = 8)
	int      shadowActive;                                    // skinned actors receiving-shadow hazard  [0x8A0]

	int		 UNKNOWN;                                         // unknown (see bDrawSoftSkinCPU)			 [0x8A4]
	uchar    pad[8];										  // padding								 [0x8A8]
} TBRenderStateInfo; // sizeof(TBRenderStateInfo) == 0x8B0 with alignment: 0x8


// a render target control structure
typedef struct _TBRenderTarget {
	TBTexture               dummyTexture;           // so we can use this struct       [0x00]

	// linkage
	struct _TBRenderTarget*	next;					// LRU/owner list                  [0x60]
	struct _TBRenderTarget*	prev;					// LRU/owner list                  [0x64]

	// logical size
	int						width;					// target width                    [0x68]
	int						height;					// target height                   [0x6C]

	// depth in bits
	int						rgbDepth;				// color depth (bits)              [0x70]
	int						zDepth;					// depth-stencil depth (bits)      [0x74]

	// usage flags
	uint32					flags;					// creation/runtime flags          [0x78]

	// backing objects
	IDirect3DTexture8*	    d3dTexture;				// color as texture (if allocated) [0x7C]
	IDirect3DSurface8*		d3dZBuffer;				// depth-stencil surface           [0x80]
	IDirect3DSurface8*		d3dSurface;				// color render surface            [0x84]

	// viewport (cached)
	int						vpX;					// viewport x                      [0x88]
	int						vpY;					// viewport y                      [0x8C]
	int						vpWidth;				// viewport width                  [0x90]
	int						vpHeight;				// viewport height                 [0x94]

	// formats
	D3DFORMAT				rgbFormat;				// color format                    [0x98]
	D3DFORMAT				zFormat;				// depth format                    [0x9C]

	// clip rect (cached)
	int						clipXPos;				// clip x                          [0xA0]
	int						clipYPos;				// clip y                          [0xA4]
	int						clipWidth;				// clip w                          [0xA8]
	int						clipHeight;				// clip h                          [0xAC]
} TBRenderTarget;


// ********************************************************************************
// Globals

extern TBRenderStateInfo	bRenderState;			// record of current render states
extern TBRenderTarget		bRenderTargetList;		// list of render targets


// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bdSetRenderState
   Purpose : Set a render state
   Parameters : render state to set, first value, second value
   Returns : 
   Info : 
*/

void bdSetRenderState(uint32 renderState, uint32 value1, uint32 value2=0);


/* --------------------------------------------------------------------------------
   Function : bSetDefaultRenderStates
   Purpose : Set the default render states
   Parameters : 
   Returns : 
   Info : 
*/

void bSetDefaultRenderStates();


/* --------------------------------------------------------------------------------
   Function : bdSetFogRange
   Purpose : Set the ranges for distance fog
   Parameters : near distance, far distance
   Returns : 
   Info : 
*/

void bdSetFogRange(float nearDist, float farDist);


/* --------------------------------------------------------------------------------
   Function : bdSetFogColour
   Purpose : Set the colour for distance fog
   Parameters : red, green, blue (each 0..255)
   Returns : 
   Info : 
*/

void bdSetFogColour(int red, int green, int blue);


/*	--------------------------------------------------------------------------------
	Function : bSetGlobalAlpha
	Purpose : set the global alpha scale value for actor drawing
	Parameters : new alpha scale value (256=solid)
	Returns : 
	Info : platform specific code called from baSetGlobalAlpha
*/

void bSetGlobalAlpha(int newAlphaScale);


/*	--------------------------------------------------------------------------------
	Function : bdCreateRenderTarget
	Purpose : create an off-screen render target
	Parameters : width in pixels, height in pixels, rgb depth in bits, Z depth in bits, flags (see BCREATERENDERTARGET_)
	Returns : ptr to target or NULL for failure
	Info : 
*/

TBRenderTarget *bdCreateRenderTarget(int width, int height, int rgbBits=16, int zBits=16, uint32 flags=0);


/*	--------------------------------------------------------------------------------
	Function : bdDeleteRenderTarget
	Purpose : delete an off-screen render target
	Parameters : ptr to render target or NULL to delete all render targets
	Returns : 
	Info : 
*/

void bdDeleteRenderTarget(TBRenderTarget *target=NULL);


/*	--------------------------------------------------------------------------------
	Function : bdSetRenderTarget
	Purpose : set the current render target
	Parameters : ptr to render target or NULL to use default screen, clear colour, clear depth,
																						flags (see BSETRENDERTARGET_)
	Returns : OK/FAIL
	Info : 
*/

int bdSetRenderTarget(TBRenderTarget *target=NULL, int r=0,int g=0,int b=0, int a=0,float depth=1.0f, uint32 flags=0);


/*	--------------------------------------------------------------------------------
	Function : bdGetRenderTargetInfo
	Purpose : return information for a render target
	Parameters : ptr to render target, ptr for width, ptr for height, ptr for RGB depth, ptr for Z depth
	Returns : 
	Info : width, height or depth ptrs can be NULL
*/

void bdGetRenderTargetInfo(TBRenderTarget *target, int *width=NULL, int *height=NULL, int *rgbDepth=NULL, int *zDepth=NULL);


/*	--------------------------------------------------------------------------------
	Function : bdSetRenderTargetAsTexture
	Purpose : set a render target as a texture
	Parameters : ptr to render target, zero-based texture stage
	Returns : OK/FAIL
	Info : 
*/

int bdSetRenderTargetAsTexture(TBRenderTarget *target, int stage=0);

/* --------------------------------------------------------------------------------
   Function : bSuspendRenderTargets
   Purpose : suspend render buffers
   Parameters : 
   Returns : 
   Info : 
*/

void bSuspendRenderTargets();

/* --------------------------------------------------------------------------------
   Function : bResumeRenderTargets
   Purpose : resume render buffers
   Parameters : 
   Returns : 
   Info : 
*/

void bResumeRenderTargets();

/*	--------------------------------------------------------------------------------
	Function : bdLockRenderState
	Purpose : increment the lock count on a render state
	Parameters : render state
	Returns : new lock count
	Info : 
*/

int bdLockRenderState(int renderState);


/*	--------------------------------------------------------------------------------
	Function : bdUnlockRenderState
	Purpose : decrement the lock count on a render state
	Parameters : render state
	Returns : new lock count
	Info : 
*/

int bdUnlockRenderState(int renderState);

#endif		// __BDISPLAY_PCRENDER_H__
