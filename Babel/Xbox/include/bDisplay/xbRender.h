// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : xbRender.h
//   Purpose : render state functionality
// Component : Xbox Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BDISPLAY_XBRENDER_H__
#define __BDISPLAY_XBRENDER_H__

// ********************************************************************************
// Constants and Macros

// max simultaneous texture stages
#define BMAX_TEXTURE_STAGES	4


// ********************************************************************************
// Types, Structures and Classes

// information on which render states are valid
typedef struct _TBRenderStateInfo {
	int						antialiasEnableState;					// what can this card do with antialiasing?
	struct _TBTexture		*currentTexture[BMAX_TEXTURE_STAGES];	// current texture
	float					fogNear, fogFar, fogRange;				// fog parameters
	int						rFog,gFog,bFog;							// fog colour

	union {
		uint32				renderState[BDRENDERSTATE_NOOF][2];		// current render state values
		uint64				renderState64[BDRENDERSTATE_NOOF];		// combined 64bit version
	};

	union {
		uint32				stateStack[BDRENDERSTATE_NOOF][BRENDERSTATE_STACKDEPTH][2];		// render state stack
		uint64				stateStack64[BDRENDERSTATE_NOOF][BRENDERSTATE_STACKDEPTH];		// combined 64bit version
	};

	int32					stateStackDepth[BDRENDERSTATE_NOOF];		// depth of stacks
	int32					textureEnable[BMAX_TEXTURE_STAGES];			// current texture enable states
	int32					renderStateLock[BDRENDERSTATE_NOOF];		// lock count on render states (states can only be set when it is zero)
	int32					textureMatrix[BMAX_TEXTURE_STAGES];			// texture stage using texture matrix ?
	int32					textureMatrixFlags[BMAX_TEXTURE_STAGES];	// texture matrix flags for specific stages
	TBMatrix				textureMatrixLinear[BMAX_TEXTURE_STAGES];
	TBMatrix				textureMatrixUser[BMAX_TEXTURE_STAGES];
	int						forceTextureStage[BMAX_TEXTURE_STAGES];		// ref counts to force actor texture use on particular stages
	int						shadowActive;								// used on PC to warn about problems with skinned actors as recieving geometry
} TBRenderStateInfo;


// a render target control structure
typedef struct _TBRenderTarget {
	TBTexture				dummyTexture;							// just so we can use this structure 
	TBTextureData			dummyTextureData;						// data for the dummy texture

	struct _TBRenderTarget	*next, *prev;							// list links
	int						width, height;							// width and height in pixels
	int						rgbDepth;								// rgb depth in bits
	int						zDepth;									// z depth in bits
	uint32					flags;									// flags (see BCREATERENDERTARGET_)
	int						vpX, vpY;								// top-left viewport coord
	int						vpWidth, vpHeight;						// viewport width and height
	int						clipXPos, clipYPos;						// clip rectangle position
	int						clipWidth, clipHeight;					// clip rectangle width and height
	void					*texData;								// WC texture data
	void					*zData;									// WC z-buffer data
	int						textureBytes;							// size of texture/zbuffer data
	D3DTexture				d3dTexture;								// our D3D texture info
	D3DSurface				d3dZBuffer;								// our D3D Z buffer
	D3DSurface				*d3dSurface;							// the surface of our texture
	D3DFORMAT				rgbFormat,zFormat;						// buffer formats
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

#endif		// __BDISPLAY_XBRENDER_H__
