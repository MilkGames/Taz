// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : xbTexture.h
//   Purpose : texture functionality
// Component : Xbox Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BDISPLAY_XBTEXTURE_H__
#define __BDISPLAY_XBTEXTURE_H__

// ********************************************************************************
// Constants and Macros

// retrieve width and height of textures (not API macros)
#define BGET_TEXTURE_WIDTH(TEX)		((TEX)->xDim)
#define BGET_TEXTURE_HEIGHT(TEX)	((TEX)->yDim)

// texture projector control mode
typedef enum EBTexProjectControl {
	EBTEXPROJECTCONTROL_POSITIONAL,						// projector is specified as position in space
	EBTEXPROJECTCONTROL_ORIENTATION,					// projector is specified as orientation around focus
};

// texture projector flags
#define BTEXPROJECTOR_DYNAMIC				0x00000001			// projector structure was dynamically allocated
#define BTEXPROJECTOR_STICKYFOCUS			0x00000002			// projector focus should be dynamically updated from attached actor
#define BTEXPROJECTOR_STICKYEULER			0x00000004			// sticky euler angle orientation
#define BTEXPROJECTOR_STICKYQUAT			0x00000008			// sticky quaternion orientation
#define BTEXPROJECTOR_STICKYMATRIX			0x00000010			// sticky matrix orientation
#define BTEXPROJECTOR_STICKYORIENTATIONMASK	(BTEXPROJECTOR_STICKYEULER | BTEXPROJECTOR_STICKYQUAT | BTEXPROJECTOR_STICKYMATRIX)			// mask for sticky orientation flags
#define BTEXPROJECTOR_TEXTURE				0x00000020			// projector is projecting a texture
#define BTEXPROJECTOR_RENDERTARGET			0x00000040			// projector is projecting a render target
#define BTEXPROJECTOR_SHADOWMAP				0x00000080			// projector is being used for shadow maps
#define BTEXPROJECTOR_USESTENCIL			0x00000100			// use stencil buffer to limit projection area


// bdDrawActorInstanceToProjector() flags
#define BDRAWTOPROJECTOR_PRIMARYTARGET		0x00000001			// restore primary target when done

// macros to set palette entries
#define BSET1555PALETTEENTRY(palette, entry, r, g, b, a) {}
#define BSET8888PALETTEENTRY(palette, entry, r, g, b, a) { uint32 *_pal = (uint32 *)palette; _pal[entry] = (a << 24) | (r << 16) | (g << 8) | (b); }
#define BSET565PALETTEENTRY(palette, entry, r, g, b, a)  {}
#define BSET5A3PALETTEENTRY(palette, entry, r, g, b, a)  {}

typedef enum EBTextureMatrixType 
{
	BTEXTUREMATRIXTYPE_NONE,						// No matrix used
	BTEXTUREMATRIXTYPE_OBJECTSPACEPOSITION,			// [GameCube only] Input to matrix is the position in object space
	BTEXTUREMATRIXTYPE_OBJECTSPACENORMAL,			// [GameCube only] Input to matrix is the normal in object space
	BTEXTUREMATRIXTYPE_OBJECTSPACEPOSITION_ANDPOST,	// [GameCube only] position in object space + with post transform matrix too
	BTEXTUREMATRIXTYPE_OBJECTSPACENORMAL_ANDPOST,	// [GameCube only] normal in object space + with post transform matrix too
	BTEXTUREMATRIXTYPE_TEXTURECOORD,				// [GameCube only] Input to matrix is texture coord
	BTEXTUREMATRIXTYPE_TEXTURECOORD_ANDPOST,		// [GameCube only] texture coord + with post transform matrix too
	BTEXTUREMATRIXTYPE_TEXTURECOORD_NOMATRIX,		// [GameCube only] texture coord (untransformed)
	BTEXTUREMATRIXTYPE_CAMERASPACEPOSITION,			// [Not GameCube] Input to matrix is the position in camera space
};

#define	BTEXMATRIX_LINEAR					0x00000001	// A linear texture is in operation
#define	BTEXMATRIX_SCALEFORLINEAR			0x00000002	// Flag that we must scale texture transform to account for linear texture


// ********************************************************************************
// Types, Structures and Classes

// texture data container
typedef struct {
	D3DTexture				header;					// texture header
	int						delay;					// anim delay for this frame
} TBTextureData;


// a texture record
typedef struct _TBTexture {
	TBResourceInfo			resInfo;				// global resource info

	D3DTexture				*curFrame;				// current frame header
	TBTextureData			*frame;					// textures for each frame
	int32					totalArea;				// total area of a single frame (including mips)
	ushort					format;					// texture format
	ushort					xDim;					// texture width

	ushort					yDim;					// texture height
	ushort					flags;					// flags
	ushort					curFrameNumber;			// current frame number
	uchar					mipLevels;				// mipmap levels
	uchar					noofFrames;				// #frames
	int32					animDuration;			// total anim duration
	uchar					alphaBlendMode;			// alpha blend mode (see BDALPHABLENDMODE_)
	uchar					bpp;					// bits per pixel
	uchar					pad[2];					// important for srtucture alignment

	D3DPalette				d3dPalette;				// D3D palette interface (paletted formats only, 12 bytes)
	TBTextureCallback		setCallback;			// called when this texture is set

	void					*setContext;			// context for setCallback function
	TBTextureCallback		unsetCallback;			// called when this texture is unset
	void					*unsetContext;			// context for unsetCallback function
	void					*lastLockPtr;			// ptr from last lock

	void					*lastPaletteLockPtr;	// ptr to last locked palette
	uchar					pad4[12];

	uchar					pad5[16];				// pad to 128 bytes to keep texture data alignment
} TBTexture;


// a texture projector
typedef struct _TBTextureProjector {
	EBTexProjectControl		controlMode;			// control mode
	TBVector				focus;					// destination (i.e. focus) position
	TBVector				position;				// source position (if used in positional mode)
	TBMatrix				orientation;			// orientation matrix (if used in orientation mode) (rotates world space points infront of the projectortakes points from world space into camera space when coupled with positional translation)
	TBMatrix				invOrientation;			// inverse orientation matrix (rotates points from projector space into world space)
	float					distance;				// distance of projector from focus (if used in orientation mode)
	TBTexture				*texture;				// texture being projected or NULL if using a target
	struct _TBRenderTarget	*target;				// render target being projected or NULL if using texture
	uint32					flags;					// flags bitfield (see BTEXPROJECTOR_)
	struct _TBActorInstance	*focusActor;			// actor we are focused on
	TBVector				focusOffset;			// offset for focal point from actor origin
	float					*focusTrackVector;		// vector focus point is tracking
	union {
		float				*stickyQuat;			// ptr to quaternion our orientation is tracking
		float				*stickyMatrix;			// ptr to matrix our orientation is tracking
		float				*stickyEuler[3];		// ptr to euler angles our orientation is tracking
	};
	float					xFov, yFov;				// X and Y field of view angles in radians
	float					nearClip, farClip;		// near and far clip planes for the projector
	int						shadowDensity;			// shadow density if being used for shadow maps (0=solidblack..255=none)
	TBMatrix				projProjection;			// projector projection matrix
} TBTextureProjector;


// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bTextureInit
   Purpose : Initialise the texture manager
   Parameters :
   Returns : 
   Info : this is an internal Babel function and should not be called by client code
*/

int bTextureInit();


/* --------------------------------------------------------------------------------
   Function : bTextureShutdown
   Purpose : Shutdown the texture manager
   Parameters :
   Returns : 
   Info : this is an internal Babel function and should not be called by client code
*/

void bTextureShutdown();


/* --------------------------------------------------------------------------------
   Function : bdSetTexture
   Purpose : Set the current texture to draw with
   Parameters : texture stage (0..), texture handle
   Returns : OK/FAIL
   Info : 
*/

int bdSetTexture(int stage, TBTexture *handle);


/* --------------------------------------------------------------------------------
   Function : bdEnableTextureMatrix
   Purpose : Enable the texture matrix on specified stage
   Parameters : texture stage, texture matrix type
   Returns : OK/FAIL
   Info : 
*/

int bdEnableTextureMatrix(int stage, EBTextureMatrixType type);


/* --------------------------------------------------------------------------------
   Function : bdDisableTextureMatrix
   Purpose : Disable the texture matrix on specified stage
   Parameters : texture stage
   Returns : OK/FAIL
   Info : 
*/

int bdDisableTextureMatrix(int stage);


/* --------------------------------------------------------------------------------
   Function : bdSetTextureMatrix
   Purpose : Set the texture matrix on specified stage
   Parameters : texture stage, texture matrix
   Returns : OK/FAIL
   Info : Texture stage must be enabled with bdEnableTextureMatrix
*/

int bdSetTextureMatrix(int stage, const TBMatrix m);


/* --------------------------------------------------------------------------------
   Function : bSetTextureLinearMatrix
   Purpose : Set the texture matrix specific for a linear texture on specified stage
   Parameters : texture stage, texture matrix
   Returns : OK/FAIL
   Info : Texture stage must be enabled with bdEnableTextureMatrix
*/

int bdSetTextureLinearMatrix(int stage, const TBMatrix m);


/* --------------------------------------------------------------------------------
   Function : bDisableTextureLinearMatrix
   Purpose : Disable the texture matrix specific for a linear texture on specified stage
   Parameters : texture stage
   Returns : OK/FAIL
   Info : 
*/

int bdDisableTextureLinearMatrix(int stage);


/* --------------------------------------------------------------------------------
   Function : bLoadTextureByCRC
   Purpose : Load a texture from a package, returning a handle to it
   Parameters : package index, texture CRC
   Returns : handle or NULL for failure
   Info : will return existing handle if texture already loaded
*/

TBTexture *bLoadTextureByCRC(TBPackageIndex *index, uint32 crc);


/* --------------------------------------------------------------------------------
   Function : bDeleteTexture
   Purpose : Delete a texture
   Parameters : texture handle
   Returns : 
   Info : 
*/

void bDeleteTexture(TBTexture *handle);


/* --------------------------------------------------------------------------------
   Function : bdLockTexture
   Purpose : Lock a texture for data/palette access
   Parameters :texture handle, frame number (0..), returned width, returned height, returned pitch, returned format, returned palette ptr
   Returns : ptr to data or NULL for failure
   Info : 
*/

void *bdLockTexture(TBTexture *texture, int frame, int *xDim, int *yDim, int *pitch, EBTextureFormat *format, uchar **palette);


/* --------------------------------------------------------------------------------
   Function : bdUnlockTexture
   Purpose : Unlock a texture after data/palette access
   Parameters :texture handle, frame number (0..), flags
   Returns : ptr to data or NULL for failure
   Info : 
*/

void bdUnlockTexture(TBTexture *texture, int frame, uint32 flags);


/* --------------------------------------------------------------------------------
   Function : bMakeTextureSurface
   Purpose : make the texture surface for a texture
   Parameters : texture handle, dimensions (x,y), format, #mipmap levels (1+)
   Returns : OK/FAIL
   Info : 
*/

int bMakeTextureSurface(TBTexture *handle, int xDim, int yDim, int format, int mipLevels, int noofFrames);


/* --------------------------------------------------------------------------------
   Function : bdSetTextureFrame
   Purpose : set the current frame for a texture
   Parameters : ptr to texture, frame number (0..noofFrames-1)
   Returns : frame set to
   Info : 
*/

int bdSetTextureFrame(TBTexture *texturePtr, int frameNumber);


/* --------------------------------------------------------------------------------
   Function : bGetTextureSurface
   Purpose : retrieve the texture surface associated with a particular mip level of a texture
   Parameters : texture, mip level (0..)
   Returns : ptr to surface or NULL for failure
   Info : 
*/

LPDIRECT3DSURFACE8 bGetTextureSurface(LPDIRECT3DTEXTURE8 texture, int level);


/*	--------------------------------------------------------------------------------
	Function : bdCreateTexture
	Purpose : Create a blank texture of the specified size, with mipmapping and animation frames. Texture flags can also be specified.
	Parameters : [in] the x dimention of the texture.
				[in] the y dimention of the texture.
				[in] the number of mip-map levels to the texture.
				[in] the number of frames in the texture.
				[in] flags specifing hints for the management of the texture.
				[in] format to create
	Returns : A pointer to a texture handle (needs to be FREE()'d later) or NULL on error.
	Info : 
*/

TBTexture *bdCreateTexture(const int xDim, const int yDim, const int mipLevels, const int noofFrames, const int flags, int format=BTEXTUREFORMAT_INVALID);


/* --------------------------------------------------------------------------------
   Function : bdGetTextureInfo
   Purpose : retrieve information about a texture
   Parameters : texture, ptr for width, ptr for height, ptr for format, ptr for #frames, ptr for current frame (0..)
   Returns : 
   Info : any pointer can be NULL if that information is not required
*/

void bdGetTextureInfo(TBTexture *texture, int *width, int *height, EBTextureFormat *format, int *noofFrames, int *currentFrame);


/* --------------------------------------------------------------------------------
   Function : bdSetNoTexture
   Purpose : Set no texture on a texture stage
   Parameters : stage to set
   Returns : 
   Info : 
*/

void bdSetNoTexture(int stage);


/* --------------------------------------------------------------------------------
   Function : bFixupTexture
   Purpose : Fixup a loaded texture structure
   Parameters : ptr to texture
   Returns : 
   Info : 
*/

void bFixupTexture(TBTexture *texture);


/* --------------------------------------------------------------------------------
   Function : bSetTextureLite
   Purpose : Internal function for limited-functionality texture setting
   Parameters : stage to set, texture handle
   Returns : 
   Info : 
*/

__inline void bSetTextureLite(int stage, TBTexture *handle)
{
	if (handle)
	{
		D3DDevice_SetTexture(stage, handle->curFrame);
		if (handle->format == BTEXTUREFORMAT_256PALETTE_8888ARGB)
			D3DDevice_SetPalette(stage, &handle->d3dPalette);
	}
	else
	{
		D3DDevice_SetTexture(stage, 0);
	}
}


/* --------------------------------------------------------------------------------
   Function : bGetTextureFrameDelay
   Purpose : return the frame delay for a texture frame
   Parameters : ptr to texture, frame number (0..noofFrames-1)
   Returns : delay value
   Info : 
*/

BINLINE int bGetTextureFrameDelay(TBTexture *texturePtr, int frameNumber)
{
	BASSERT(BASSERT_GENERAL, texturePtr != NULL && frameNumber < texturePtr->noofFrames, "bGetTextureFrameDelay: texturePtr == NULL (%#x) or frameNumber specified (%d) >= texturePtr->noofFrames (%d)\n", texturePtr, frameNumber, texturePtr->noofFrames);

	return texturePtr->frame[frameNumber].delay;
}


/* --------------------------------------------------------------------------------
   Function : bdScrollTexture
   Purpose : scroll source texture, physically, into destination
   Parameters : source and destination texture pointer, st offsets (0..1)
   Returns : 
   Info : source and destination texture formats should match
*/

void bdScrollTexture(struct _TBTexture *sourTexture, struct _TBTexture *destTexture, float s, float t);


#endif		// __BDISPLAY_XBTEXTURE_H__
