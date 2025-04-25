// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2001 Blitz Games Ltd.
//
//      File : render.h
//   Purpose : render state functionality
// Component : Generic Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BDISPLAY_RENDER_H__
#define __BDISPLAY_RENDER_H__

// ********************************************************************************
// Constants and Macros

// display flags
#define BDISPLAYFLAG_OPEN				0x00000001u			// set if the display is currently open
#define BDISPLAYFLAG_NTSC				0x00000002u			// set if we are using NTSC (60Hz) mode
#define BDISPLAYFLAG_PAL				0x00000004u			// set if we are using PAL (50Hz) mode
#define BDISPLAYFLAG_VERBOSE			0x00000008u			// set to print extra debugging info
#define BDISPLAYFLAG_TRIPLEBUFFER		0x00000010u			// set if we are running triple buffered
#define BDISPLAYFLAG_WINDOW				0x00000020u			// set if we are to run in a window
#define BDISPLAYFLAG_ANTIALIASED		0x00000040u			// set if we are running an anti-aliased display
#define BDISPLAYFLAG_FIELD				0x00000080u			// run in field mode (<= 60Hz, PS2 only)
#define BDISPLAYFLAG_NOVSYNC			0x00000100u			// flip without waiting for the vsync (if supported)
#define BDISPLAYFLAG_ANTIALIASED4		0x00000200u			// use 4 sample anti-aliasing instead of 2 (Xbox)
#define BDISPLAYFLAG_OPENAUTODEPTHOFF	0x00000400u			// PC-ONLY: *don't* let Babel try to choose another depth on opendisplay failure
#define BDISPLAYFLAG_OPENAUTOSIZEOFF	0x00000800u			// PC-ONLY: *don't* let Babel try to choose another screen size on opendisplay failure
#define BDISPLAYFLAG_NOTEXTURESTATES	0x00001000u			// PC-ONLY: disable use of texture stage states (for compatibility)


// Render state constants
enum {
	BDRENDERSTATE_ANTIALIAS,			// Anti-aliasing on/off (default off)
	BDRENDERSTATE_TEXTUREFILTER,		// Texture filter mode (default LINEAR)
	BDRENDERSTATE_TEXTUREWRAP,			// Texture wrapping control (default wrap in u & v)
	BDRENDERSTATE_LIGHTING,				// Lighting on/off (default off)
	BDRENDERSTATE_BACKFACECULL,			// Cull back face polys (default BDCULLMODE_OFF)
	BDRENDERSTATE_ZWRITE,				// Z writing on/off (default on)
	BDRENDERSTATE_ALPHAENABLE,			// Alpha blend enable (default off)
	BDRENDERSTATE_ZTEST,				// Z testing mode (default BDZTESTMODE_LESSTHAN)
	BDRENDERSTATE_DISTANCEFOG,			// Distance fogging on/off (default OFF)
	BDRENDERSTATE_CLEAR,				// Screen/Z clearing on/off (default BDCLEARMODE_ALL)
	BDRENDERSTATE_ALPHABLENDMODE,		// set the alpha blending mode
	BDRENDERSTATE_FRAMEWRITE,			// enable/disable writing to the frame buffer
	BDRENDERSTATE_2DWORLDMATRIX,		// enable/disable the world matrix in 2D mode
	BDRENDERSTATE_TEXTUREENABLE,		// enable/disable the textures on a specific stage..
	BDRENDERSTATE_FORCECLIP,			// force polygon clipping on objects

	BDRENDERSTATE_NOOF					// number of supported render states
};


// texture filters
#define BDTEXTUREFILTER_POINT			1	// Point sampling
#define BDTEXTUREFILTER_LINEAR			2	// Bilinear sampling
#define BDTEXTUREFILTER_MIPPOINT		3	// Mip-mapping and point sampling
#define BDTEXTUREFILTER_MIPLINEAR		4	// Mip-mapping and bilinear sampling
#define BDTEXTUREFILTER_TRILINEAR		5	// Mip-map blending and bilinear sampling

// texture wrap modes
#define BDTEXTUREWRAP_WRAPU				1	// Wrap u co-ords
#define BDTEXTUREWRAP_WRAPV				2	// Wrap v co-ords
#define BDTEXTUREWRAP_CLAMPU			4	// Clamp u co-ords
#define BDTEXTUREWRAP_CLAMPV			8	// Clamp v co-ords

// Z testing modes
#define BDZTESTMODE_NEVER				0	// never pass the Z test
#define BDZTESTMODE_ALWAYS				1	// always pass the Z test
#define BDZTESTMODE_LESSEQUAL			2	// pass if closer or equal
#define BDZTESTMODE_LESSTHAN			3	// pass if closer
#define BDZTESTMODE_GREATEREQUAL		4	// pass if further or equal
#define BDZTESTMODE_GREATER				5	// pass if further
#define BDZTESTMODE_EQUAL				6	// pass if equal to
#define BDZTESTMODE_NOTEQUAL			7	// pass if not equal to

// clear modes
#define BDCLEARMODE_FRAME				0x01	// clear the frame buffer
#define BDCLEARMODE_Z					0x02	// clear the Z buffer
#define BDCLEARMODE_STENCIL				0x04	// clear the stencil buffer
#define BDCLEARMODE_ALL					0xff	// clear everything

// alpha blending modes
#define BDALPHABLENDMODE_BLEND			0		// alpha-blend mode
#define BDALPHABLENDMODE_ADDITIVE		1		// additive mode
#define BDALPHABLENDMODE_MULTIPLY		2		// multiply
#define BDALPHABLENDMODE_SUBTRACTIVE	3		// subtractive mode
#define BDALPHABLENDMODE_NONE			4		// no blending
#define BDALPHABLENDMODE_INVBLEND		5		// inverse alpha-blend
#define BDALPHABLENDMODE_INVADDITIVE	6		// inverse additive
#define BDALPHABLENDMODES				7		// Number of blend modes

// culling modes
#define BDCULLMODE_OFF					0		// no culling
#define BDCULLMODE_CCW					1		// cull counter-clockwise faces
#define BDCULLMODE_CW					2		// cull clockwise faces


// frame write flags
#define BDFRAMEWRITE_ALL				0x01
#define BDFRAMEWRITE_RED				0x02
#define BDFRAMEWRITE_GREEN				0x04
#define BDFRAMEWRITE_BLUE				0x08
#define BDFRAMEWRITE_ALPHA				0x10


// bdCreateRenderTarget flags
#define BCREATERENDERTARGET_PRIVATEZ	0x00000001		// use a private Z buffer rather than sharing the primary one
#define BCREATERENDERTARGET_LOCKABLE	0x00000002		// this render target will need to be locked at some point
#define BCREATERENDERTARGET_NODITHER	0x00000004		// don't dither when rendering to this target
#define BCREATERENDERTARGET_DYNAMIC		0x00000008		// create this target in memory suitable for dynamic manipulation

// bdSetRenderTarget flags
#define BSETRENDERTARGET_CLEARRGB		0x00000001		// clear the colour buffer
#define BSETRENDERTARGET_CLEARZ			0x00000002		// clear the Z buffer
#define BSETRENDERTARGET_CLEAR			(BSETRENDERTARGET_CLEARRGB | BSETRENDERTARGET_CLEARZ) // clear colour+Z


// render state stack depth
#define BRENDERSTATE_STACKDEPTH			16


// ********************************************************************************
// Types, Structures and Classes

// render state save block
typedef struct _TBSavedRenderStates {
	union {
		uint32					states[BDRENDERSTATE_NOOF][2];			// saved render states
		uint64					states64[BDRENDERSTATE_NOOF];			// combined 64bit version
	};
} TBSavedRenderStates;



// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bSetInitialRenderStates
   Purpose : Set the initial render states
   Parameters : number of stages
   Returns : 
   Info : 
*/

void bSetInitialRenderStates(int stages);


/* --------------------------------------------------------------------------------
   Function : bdGetRenderState
   Purpose : Get a render state
   Parameters : render state to get, ptr to fill with current value2
   Returns : current value1
   Info : 
*/

uint32 bdGetRenderState(uint32 renderState, uint32 *value2=NULL);


/* --------------------------------------------------------------------------------
   Function : bdPushRenderState
   Purpose : push the current value of a render state onto the stack and set a new state
   Parameters : render state to set, first value, second value
   Returns : 
   Info : 
*/

void bdPushRenderState(uint32 state, uint32 value1, uint32 value2=0);


/* --------------------------------------------------------------------------------
   Function : bdPopRenderState
   Purpose : pop a renderstate off the stack
   Parameters : render state to pop
   Returns : 
   Info : 
*/

void bdPopRenderState(uint32 state);


/* --------------------------------------------------------------------------------
   Function : bdSaveRenderStates
   Purpose : save the current status of all render states
   Parameters : block to save to
   Returns : 
   Info : 
*/

void bdSaveRenderStates(TBSavedRenderStates *saveBlock);


/* --------------------------------------------------------------------------------
   Function : bdRestoreRenderStates
   Purpose : restore the renderstates from a saved block
   Parameters : block to restore from
   Returns : 
   Info : only states that have changed are restored
*/

void bdRestoreRenderStates(TBSavedRenderStates *saveBlock);

#endif		// __BDISPLAY_RENDER_H__
