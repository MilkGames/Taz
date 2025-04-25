// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : texture.h
//   Purpose : texture functionality
// Component : Generic Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BDISPLAY_TEXTURE_H__
#define __BDISPLAY_TEXTURE_H__

// ********************************************************************************
// Constants and Macros

// texture unlock flags
#define BTEXTUREUNLOCK_DATA		0x00000001		// the texture data was modified
#define BTEXTUREUNLOCK_PALETTE	0x00000002		// the texture palette was modified

// texture data formats
typedef enum EBTextureFormat {
	BTEXTUREFORMAT_INVALID,						// Invalid
	BTEXTUREFORMAT_8888ARGB,					// PC 32-bit
	BTEXTUREFORMAT_8888ABGR,					// PS2 32-bit
	BTEXTUREFORMAT_1555ARGB,					// PC 16-bit
	BTEXTUREFORMAT_1555ABGR,					// PS2 16-bit
	BTEXTUREFORMAT_256PALETTE_8888ARGB,			// PC 8-bit
	BTEXTUREFORMAT_256PALETTE_1555ABGR,			// PS2 8-bit (16-bit palette)
	BTEXTUREFORMAT_16PALETTE_8888ARGB,			// PC 4-bit
	BTEXTUREFORMAT_16PALETTE_1555ABGR,			// PS2 4-bit (16-bit palette)
	BTEXTUREFORMAT_4444ARGB,					// PC 16-bit (internal use only)
	BTEXTUREFORMAT_256PALETTE_8888ABGR,			// PS2 8-bit (32-bit palette)
	BTEXTUREFORMAT_16PALETTE_8888ABGR,			// PS2 4-bit (32-bit palette)
	BTEXTUREFORMAT_8888ARGB_LINEAR,				// Xbox linear 32-bit (internal use only)
	BTEXTUREFORMAT_565RGB_LINEAR,				// Xbox linear 16-bit (internal use only)
	BTEXTUREFORMAT_1555ARGB_LINEAR,				// Xbox linear 16-bit (with alpha)
	BTEXTUREFORMAT_TILE_8888ARGB,				// GameCube 32-bit
	BTEXTUREFORMAT_TILE_5A3ARGB,				// GameCube 16-bit
	BTEXTUREFORMAT_TILE_256PALETTE_RGB565,		// GameCube 8-bit (16-bit palette, no alpha)
	BTEXTUREFORMAT_TILE_256PALETTE_RGB5A3,		// GameCube 8-bit (16-bit palette with alpha)
	BTEXTUREFORMAT_TILE_16PALETTE_RGB565,		// GameCube 4-bit (16-bit palette, no alpha)
	BTEXTUREFORMAT_TILE_16PALETTE_RGB5A3,		// GameCube 4-bit (16-bit palette with alpha)
	BTEXTUREFORMAT_TILE_S3TC,					// GameCube Compressed format
	BTEXTUREFORMAT_TILE_I4,						// GameCube 4 bit (intensity) (internal use only)
	BTEXTUREFORMAT_TILE_565RGB,					// GameCube 16-bit (internal use)
//	BTEXTUREFORMAT_S3TC,						// Xbox Compressed format DXT1
	BTEXTUREFORMAT_DXT1,						// Xbox Compressed format DXT1
	BTEXTUREFORMAT_DXT2,						// Xbox Compressed format DXT2
	BTEXTUREFORMAT_DXT3,						// Xbox Compressed format DXT3
	BTEXTUREFORMAT_DXT4,						// Xbox Compressed format DXT4
	BTEXTUREFORMAT_DXT5,						// Xbox Compressed format DXT5
} EBTextureFormat;

// texture flags
#define BTEXTUREFLAG_DYNAMIC		0x00000001			// this texture is changed frequently
#define BTEXTUREFLAG_STATIC			0x00000002			// this texture is never changed
#define BTEXTUREFLAG_RESIDENT		0x00000004			// this texture should always stay in local VRAM
#define BTEXTUREFLAG_PRIORITY		0x00000008			// this texture has priority over normal ones for caching
#define BTEXTUREFLAG_MIPMAP			0x00000010			// this texture should be mip-mapped
#define BTEXTUREFLAG_RENDERTARGET	0x00000020			// this texture is actually a render target

// bdSetTextureCallback types
typedef enum EBTextureCallback {
	EBTEXTURECALLBACK_SET,								// called when the texture is set
	EBTEXTURECALLBACK_UNSET								// called when the texture is unset
};

// constant denoting no texture
#define BNOTEXTURE		((TBTexture*)1)


// ********************************************************************************
// Types, Structures and Classes


// texture callback prototype
typedef int (*TBTextureCallback)(struct _TBTexture *texture, int stage, void *context);


// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bdGetTextureFrameByTime
   Purpose : get the frame a texture would be set to using bdSetTextureFrameByTime
   Parameters : ptr to texture, time in ms
   Returns : frame number that would be set
   Info : 
*/

int bdGetTextureFrameByTime(struct _TBTexture *texturePtr, int milliseconds);


/* --------------------------------------------------------------------------------
   Function : bdSetTextureFrameByTime
   Purpose : set the current frame for a texture by time
   Parameters : ptr to texture, time in ms
   Returns : frame number that has been set
   Info : 
*/

int bdSetTextureFrameByTime(struct _TBTexture *texturePtr, int milliseconds);


/* --------------------------------------------------------------------------------
   Function : bdSetTextureCallback
   Purpose : set a callback function for a texture
   Parameters : texture, type of callback, callback function, context for callback
   Returns : 
   Info : 
*/

void bdSetTextureCallback(struct _TBTexture *texture, EBTextureCallback cbType, TBTextureCallback callback=NULL, void *context=NULL);


#endif		// __BDISPLAY_TEXTURE_H__
