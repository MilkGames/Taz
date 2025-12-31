// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcTexture.cpp
//   Purpose : Defines macros for texture width/height, texture projector control modes and flags. Defines TBTextureData and _TBTexture structs for texture resources, and _TBTextureProjector for texture projection
// Component : Babel Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Helper Functions

/*	--------------------------------------------------------------------------------
	Function : SuspendTexture
	Purpose : suspend A texture
	Parameters : 
	Returns : 
	Info : 
*/
void SuspendTexture(TBTexture *texture)
{
	bkPrintf("*** WARNING *** SuspendTexture was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : ResumeTexture
	Purpose : resume A texture
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/
void ResumeTexture(TBTexture *texture)
{
	bkPrintf("*** WARNING *** ResumeTexture was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : MakePalettedTexture16
	Purpose : yes
	Parameters : 
	Returns : 
	Info : 
*/
void MakePalettedTexture16(TBTexture *texture)
{
    if (!texture || texture->noofFrames == 0) return;

    // Local cursors into source streams
    const uchar* src = (const uchar*)texture->indexStream;
    const ulong*  pal = (const ulong*)texture->palHeapBase;

    // Iterate all frames
    for (uint frame = 0; frame < (uint)texture->noofFrames; ++frame)
    {
        IDirect3DTexture8* tex = texture->frames[frame];
        if (!tex) continue;

        // Base dimensions for this frame
        uint w = (uint)texture->xDim;
        uint h = (uint)texture->yDim;

        // Iterate mip levels
        for (unsigned int level = 0; level < (uint)texture->mipLevels; ++level)
        {
            D3DLOCKED_RECT lr; 
            HRESULT hr = tex->LockRect(level, &lr, NULL, 0);
            if (hr < 0) {
                bkPrintf("bLockTexture: Lock failed (%s)\n", (char*)DXGetErrorString8A(hr));
                bkPrintf("MakePalettedTexture: Could not lock texture surface\n");
                return;
            }
            if (!lr.pBits) {
                bkPrintf("MakePalettedTexture: Could not lock texture surface\n");
                return;
            }

            uchar* dstRow = (uchar*)lr.pBits;
            const uint rowPitch = (uint)lr.Pitch;

            for (unsigned int y = 0; y < h; ++y)
            {
                ulong* dst = (ulong*)dstRow;
                uint x = 0;
                // Each byte encodes two pixels: low nibble then high nibble
                for (; x + 1 < w; x += 2)
                {
                    const uchar b = *src++;
                    const uchar idx0 = (uchar)(b & 0x0F);
                    const uchar idx1 = (uchar)(b >> 4);
                    *dst++ = pal[idx0];
                    *dst++ = pal[idx1];
                }
                // If odd width, consume one nibble for the last pixel
                if (x < w)
                {
                    const uchar b = *src++;
                    const uchar idx0 = (uchar)(b & 0x0F);
                    *dst++ = pal[idx0];
                }

                dstRow += rowPitch;
            }

            tex->UnlockRect(level);

            // Next mip: halve dimensions, clamp to at least 1
            w = (w > 1) ? (w >> 1) : 1;
            h = (h > 1) ? (h >> 1) : 1;
        }
    }
}

/*	--------------------------------------------------------------------------------
	Function : MakePalettedTexture256
	Purpose : yes
	Parameters : 
	Returns : 
	Info : 
*/
void MakePalettedTexture256(TBTexture *texture)
{
    if (!texture || texture->noofFrames == 0) return;

    const uchar* src = (const uchar*)texture->indexStream;
    const ulong* pal = (const ulong*)texture->palHeapBase;

    for (uint frame = 0; frame < (uint)texture->noofFrames; ++frame)
    {
        IDirect3DTexture8* tex = texture->frames[frame];
        if (!tex) continue;

        uint w = (uint)texture->xDim;
        uint h = (uint)texture->yDim;

        for (uint level = 0; level < (uint)texture->mipLevels; ++level)
        {
            D3DLOCKED_RECT lr; 
            HRESULT hr = tex->LockRect(level, &lr, NULL, 0);
            if (hr < 0) {
                bkPrintf("bLockTexture: Lock failed (%s)\n", (char*)DXGetErrorString8A(hr));
                bkPrintf("MakePalettedTexture: Could not lock texture surface\n");
                return;
            }
            if (!lr.pBits) {
                bkPrintf("MakePalettedTexture: Could not lock texture surface\n");
                return;
            }

            uchar* dstRow = (uchar*)lr.pBits;
            const uint rowPitch = (uint)lr.Pitch;

            for (uint y = 0; y < h; ++y)
            {
                ulong* dst = (ulong*)dstRow;
                for (uint x = 0; x < w; ++x)
                {
                    const uchar idx = *src++;
                    *dst++ = pal[idx];
                }
                dstRow += rowPitch;
            }

            tex->UnlockRect(level);

            w = (w > 1) ? (w >> 1) : 1;
            h = (h > 1) ? (h >> 1) : 1;
        }
    }
}

static inline ushort READ_U16(const uchar* p) { return (ushort)( p[0] | (p[1] << 8) ); }
static inline uint32 READ_U32(const uchar* p) { return (uint32)( p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24) ); }

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
	bDeleteResourceGroup(BRESMASK_TEXTURE, BALLGROUPS);
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
    // Sentinel and stage-enable check
    if (handle == BNOTEXTURE || bRenderState.textureEnable[stage] == 0)
        return 1;

    if (handle && handle->curFrame) {
        TBTexture* old = bRenderState.currentTexture[stage];

        if (old == NULL) {
            // First bind on this stage -> enable 2X modulate on stage 0 if requested
            if (bDisplayInfo.modulate2X && stage == 0) {
                bDisplayInfo.d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
                bDisplayInfo.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE2X);
            }
        } else {
            // Unset callback of the previous texture (if present)
            if (old->unsetCallback) {
                old->unsetCallback(old, stage, old->unsetContext);
            }
        }

        // Set callback of the new texture (if present). If it returns 0 -> early out.
        if (handle->setCallback) {
            int ok = handle->setCallback(handle, stage, handle->setContext);
            if (ok == 0)
                return 1;
        }

        // Bind the texture on the device and remember it in render state
        bRenderState.currentTexture[stage] = handle;
        bDisplayInfo.d3dDevice->SetTexture(stage, handle->curFrame);
        return 1;
    } else {
        // Unbind path
        if (bRenderState.currentTexture[stage]) {
            if (bDisplayInfo.modulate2X && stage == 0) {
                // Return COLOROP to MODULATE when detaching from stage 0
                bDisplayInfo.d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
                // ALPHAOP stays at MODULATE2X per the dump
                bDisplayInfo.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE2X);
            }

            TBTexture* old = bRenderState.currentTexture[stage];
            if (old->unsetCallback) {
                old->unsetCallback(old, stage, old->unsetContext);
            }
        }

        bDisplayInfo.d3dDevice->SetTexture(stage, NULL);
        bRenderState.currentTexture[stage] = NULL;
        return 1;
    }
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
    bRenderState.forceTextureStage[stage] = 0;

    // Set identity texture matrix: D3DTS_TEXTURE0 + stage
    bDisplayInfo.d3dDevice->SetTransform((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + stage), (D3DMATRIX*)bIdentityMatrix);

    // Disable texture transform
    bDisplayInfo.d3dDevice->SetTextureStageState((DWORD)stage, D3DTSS_TEXTURETRANSFORMFLAGS, 0);

    // Texcoord source = "stage" (D3DTSS_TEXCOORDINDEX == 11 in D3D8)
    bDisplayInfo.d3dDevice->SetTextureStageState((DWORD)stage, D3DTSS_TEXCOORDINDEX, (DWORD)stage);

    return OK;
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

/* --------------------------------------------------------------------------------
   Function : bLoadTextureByCRC
   Purpose : Load a texture from a package, returning a handle to it
   Parameters : package index, texture CRC
   Returns : handle or NULL for failure
   Info : will return existing handle if texture already loaded
*/
TBTexture *bLoadTextureByCRC(TBPackageIndex *index, uint32 crc)
{
    // Request the raw file buffer; pass NULLs for optional outputs as per disassembly.
    uchar* fileData = bkLoadFileByCRC(index, crc, (uchar*)0, (int*)0, (TBFileTagInfo*)0, 0);
    if (!fileData) {
        return (TBTexture*)0; // BNOTEXTURE?
    }

    // Build a runtime texture object from the blob.
    TBTexture *tex = bFixupTexture((void*)fileData);

    // If the package ID indicates an "opened" package (loaded==0), free the blob now
    // Otherwise (loaded==1 => RAM/loaded package), store the blob for later lifetime management
    if (!index->id.loaded) {
        bkHeapFree((void*)fileData);
        return tex;
    }

    if (tex) {
        tex->loadDesc = (void*)fileData;
    }
    return tex;
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
    if (!handle) return;

    LPDIRECT3DDEVICE8 dev = bDisplayInfo.d3dDevice;

    // Unbind from the first two texture stages if this texture is currently bound
    for (uint stage = 0; stage < 2; ++stage) {
        if (bRenderState.currentTexture[stage] == handle) {
            bRenderState.currentTexture[stage] = 0;
            if (dev) {
                // IDirect3DDevice8::SetTexture(DWORD Stage, IDirect3DBaseTexture8* pTexture)
                dev->SetTexture(stage, 0);
            }
        }
    }

    // Free palette/system-memory buffer if present
    if (handle->palHeapBase) {
        bkHeapFree(handle->palHeapBase);
        handle->palHeapBase = 0;
    }

    // Release per-frame D3D textures (drain all references)
    const uint framesCount = (uint)(uchar)handle->noofFrames;
    for (uint i = 0; i < framesCount; ++i) {
        IDirect3DTexture8* tex = handle->frames[i];
        if (tex) {
            while (tex->Release() != 0) { /* drain refs to zero */ }
            handle->frames[i] = 0;
        }
    }

    // Free the texture header itself
    bkHeapFree(handle);
}

/* --------------------------------------------------------------------------------
   Function : bdLockTexture
   Purpose : Lock a texture for data/palette access
   Parameters :texture handle, frame number (0..), returned width, returned height, returned pitch, returned format, returned palette ptr
   Returns : ptr to data or NULL for failure
   Info : 
*/

void *bdLockTexture(TBTexture *texture, int frame, int *xDim, int *yDim, int *pitch, EBTextureFormat *format, uchar **palette)
{
	D3DLOCKED_RECT locked;
	HRESULT hr;

	*xDim = (int32)(uint32)texture->xDim;
	*yDim = (int32)(uint32)texture->yDim;
	*format = (EBTextureFormat)(uint32)texture->format;

	if (texture->format == BTEXTUREFORMAT_256PALETTE_8888ARGB) {
		*palette = (uchar *)texture->palHeapBase;
		*pitch = (int32)(uint32)texture->xDim;
		return (void *)((uchar *)texture->indexStream + ((int32)texture->totalArea * frame));
	}

	if (texture->format == BTEXTUREFORMAT_16PALETTE_8888ARGB) {
		*palette = (uchar *)texture->palHeapBase;
		*pitch = (int32)((uint32)texture->xDim >> 1);
		return (void *)((uchar *)texture->indexStream + (((int32)texture->totalArea * frame) / 2));
	}

	hr = texture->frames[frame]->LockRect(0, &locked, 0, 0);
	if (hr < 0) {
		bkPrintf("bLockTexture: Lock failed (%s)\n", (char *)DXGetErrorString8A(hr));
		return 0;
	}

	if (pitch != 0) {
		*pitch = locked.Pitch;
	}

	return locked.pBits;
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
    D3DFORMAT fmt;

    // EBTextureFormat -> D3DFORMAT
    switch (format) {
        case BTEXTUREFORMAT_8888ARGB:
        case BTEXTUREFORMAT_256PALETTE_8888ARGB:
        case BTEXTUREFORMAT_16PALETTE_8888ARGB:
            fmt = D3DFMT_A8R8G8B8;  // 0x15
            break;

        case BTEXTUREFORMAT_1555ARGB:
            fmt = D3DFMT_A1R5G5B5;  // 0x19
            break;

        case BTEXTUREFORMAT_4444ARGB:
            fmt = D3DFMT_A4R4G4B4;  // 0x1A
            break;

        default:
            bkPrintf("bMakeTextureSurface: Unknown texture format %d\n", (int)format);
            return FAIL;
    }

    // Creating texture for each frame
    for (int i = 0; i < noofFrames; ++i) {
        // in asm it's &handle->frame->header + i
        IDirect3DTexture8** ppTex = &handle->frames[i];

        HRESULT hr = bDisplayInfo.d3dDevice->CreateTexture(
            (UINT)xDim,
            (UINT)yDim,
            (UINT)mipLevels,
            0,                 // Usage
            fmt,
            D3DPOOL_MANAGED,   // 1
            ppTex
        );

        if (hr < 0) {
            const char* err = DXGetErrorString8A(hr);
            bkPrintf("bMakeTextureSurface: Could not create texture interface for (%dx%d), %d levels, %d format (%s)\n",
                     xDim, yDim, mipLevels, (int)format, err);
            return FAIL;
        }
    }

    return OK;
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
  uint uVar1;
  int iVar2;
  
  uVar1 = (uint)texturePtr->noofFrames;
  if (frameNumber < 0) {
    frameNumber = uVar1 - -frameNumber % (int)uVar1;
  }
  iVar2 = frameNumber % (int)uVar1;
  texturePtr->curFrame = texturePtr->frames[iVar2];
  texturePtr->currentFrame = (ushort)iVar2;
  return iVar2;
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
    // 1) All standalone textures (resource list 0)
    {
        TBResourceInfo* it = bLocalResourceList[EBRESTYPE_TEXTURE].localNext;
        TBResourceInfo* const root = &bLocalResourceList[EBRESTYPE_TEXTURE];

        while (it != root) {
            ResumeTexture((TBTexture*)it);
            it = it->localNext;
        }
    }

    // 2) Fonts (resource list 3) — resume the texture referenced by each font
    {
        TBResourceInfo* it = bLocalResourceList[EBRESTYPE_FONT].localNext;
        TBResourceInfo* const root = &bLocalResourceList[EBRESTYPE_FONT];

        while (it != root) {
            TBFont* font = (TBFont*)it;     // TBFont starts with TBResourceInfo
            ResumeTexture(font->texture);
            it = it->localNext;
        }
    }
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
    if (width != NULL) {
        *width = (uint)texture->xDim;
    }
    if (height != NULL) {
        *height = (uint)texture->yDim;
    }
    if (format != NULL) {
        *format = (EBTextureFormat)(uint)texture->format;
    }
    if (noofFrames != NULL) {
        *noofFrames = (uint)texture->noofFrames;
    }
    if (currentFrame != NULL) {
        *currentFrame = (uint)texture->currentFrame;
    }
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
TBTexture *bFixupTexture(void *fileData)
{
    if (!fileData) return BNOTEXTURE;

    const uchar* src = (const uchar*)fileData;

    // On-disk header (little-endian), as observed in disassembly:
    //  0x00: ushort xDim
    //  0x04: ushort yDim
    //  0x08: ushort format (EBTextureFormat)
    //  0x0C: ushort unknown
    //  0x0E: uchar  mipLevels
    //  0x0F: uchar  noofFrames
    //  0x10: ushort groupId
    //  0x14: uint32 loaderTag
    //  0x18: uchar  alphaBlendMode
    //  0x1C: uint32 frameMeta[noofFrames]
    const ushort xDim       = READ_U16(src + 0x00);
    const ushort yDim       = READ_U16(src + 0x04);
    const ushort format     = READ_U16(src + 0x08);
    const ushort unknown16  = READ_U16(src + 0x0C);
    const uchar  mipLevels  = *(src + 0x0E);
    const uchar  noofFrames = *(src + 0x0F);
    const ushort groupId    = READ_U16(src + 0x10);
    const uint32 loaderTag  = READ_U32(src + 0x14);
    const uchar  alphaMode  = *(src + 0x18);

    // Allocate TBTexture object with inline frames[] and frameMeta[] tables:
    // size = 0x60 + noofFrames*sizeof(void*) + noofFrames*4
    const unsigned framesBytes   = (unsigned)noofFrames * sizeof(void*);
    const unsigned frameMetaBytes= (unsigned)noofFrames * 4u;
    const unsigned handleBytes   = 0x60u + framesBytes + frameMetaBytes;

    TBTexture* handle = (TBTexture*)CALLOCEX(handleBytes, 0, (uint32)"Texture");
    if (!handle) return BNOTEXTURE;

    // Fill TBTexture header (field offsets per pcTexture.h)
    handle->resInfo.groupId = groupId;

    handle->format       = (ushort)format;
    handle->xDim         = xDim;
    handle->yDim         = yDim;
    handle->UNKNOWN      = unknown16;

    handle->mipLevels    = mipLevels;
    handle->noofFrames   = noofFrames;
    handle->loaderTag    = loaderTag;
    handle->alphaBlendMode = alphaMode;

    handle->loadDesc     = 0;
    handle->totalArea    = 0;

    // Inline tables placement
    uchar* base          = (uchar*)handle;
    handle->frames       = (IDirect3DTexture8**)(base + 0x60);
    handle->frameMeta    = (void*)(base + 0x60 + framesBytes);

    // Copy frameMeta from file (just after 0x1C)
    if (frameMetaBytes) {
        memcpy(handle->frameMeta, src + 0x1C, frameMetaBytes);
    }

    // totalArea = sum over all mip levels (no clamp to 1 — matches binary)
    {
        int w = (int)xDim;
        int h = (int)yDim;
        int m = (int)mipLevels;
        int total = 0;
        while (m-- > 0) { total += w * h; w >>= 1; h >>= 1; }
        handle->totalArea = total;
    }

    // Create D3D textures
    if (!bMakeTextureSurface(handle, (int)xDim, (int)yDim, (int)format, (int)mipLevels, (int)noofFrames)) {
        FREE(handle);
        return BNOTEXTURE;
    }

    // Initialize current frame fields
    handle->curFrame     = (handle->frames) ? handle->frames[0] : 0;
    handle->currentFrame = 0;

    // ---- Palettized formats ----
    if (format == BTEXTUREFORMAT_16PALETTE_8888ARGB) {
        // 4bpp, 32-bit palette (0x40 bytes)
        const unsigned bytes = (unsigned)((unsigned)noofFrames * (unsigned)handle->totalArea) / 2u + 0x40u;
        void* palBlock = MALLOCEX(bytes, (uint32)"Palette4");
        if (!palBlock) { FREE(handle); return BNOTEXTURE; }
        handle->palHeapBase = palBlock;
        handle->indexStream = (uchar*)palBlock + 0x40;

        const uchar* tail = src + 0x1C + frameMetaBytes;
        memcpy(palBlock, tail, bytes);

        MakePalettedTexture16(handle);
        return handle;
    }
    else if (format == BTEXTUREFORMAT_256PALETTE_8888ARGB) {
        // 8bpp, 32-bit palette (0x400 bytes)
        const unsigned bytes = (unsigned)((unsigned)noofFrames * (unsigned)handle->totalArea) + 0x400u;
        void* palBlock = MALLOCEX(bytes, (uint32)"Palette8");
        if (!palBlock) { FREE(handle); return BNOTEXTURE; }
        handle->palHeapBase = palBlock;
        handle->indexStream = (uchar*)palBlock + 0x400;

        const uchar* tail = src + 0x1C + frameMetaBytes;
        memcpy(palBlock, tail, bytes);

        MakePalettedTexture256(handle);
        return handle;
    }

    // ---- Direct pixel copy for non-palettized (16/32-bit) ----
    {
        const uchar* pixelSrc = src + 0x1C + frameMetaBytes;
        int fmt = (int)format;
        int bpp = (fmt == BTEXTUREFORMAT_8888ARGB) ? 4 : 2; // 32-bit vs 16-bit family

        int frame;
        for (frame = 0; frame < (int)noofFrames; ++frame) {
            int w = (int)xDim;
            int h = (int)yDim;

            int level;
            for (level = 0; level < (int)mipLevels; ++level) {
                D3DLOCKED_RECT lr;
                memset(&lr, 0, sizeof(lr));

                HRESULT hr = handle->frames[frame]->LockRect((UINT)level, &lr, 0, 0);
                if (hr < 0 || lr.pBits == 0) {
                    bkPrintf("bLockTexture: Lock failed (%s)\n", DXGetErrorString8A(hr));
                    bkPrintf("bdLoadTexture: Could not lock texture surface\n");
                    FREE(handle);
                    return BNOTEXTURE;
                }

                const int rowBytes = w * bpp;
                uchar* dst = (uchar*)lr.pBits;

                int y;
                for (y = 0; y < h; ++y) {
                    memcpy(dst, pixelSrc, (size_t)rowBytes);
                    dst      += lr.Pitch;
                    pixelSrc += rowBytes;
                }

                handle->frames[frame]->UnlockRect((UINT)level);

                w >>= 1; h >>= 1;
            }
        }
    }

    return handle;
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
    void *srcBits;
    void *dstBits;
    EBTextureFormat lockFormat;
    int32 xDim;
    int32 yDim;
    int32 srcPitch;
    int32 dstPitch;
    int32 yOffset;
    int32 xOffset;
    int32 row;
    uchar *srcRow;
    uchar *dstRow;

    srcBits = bdLockTexture(sourTexture, 0, &xDim, &yDim, &srcPitch, &lockFormat, (uchar **)0);
    dstBits = bdLockTexture(destTexture, 0, &xDim, &yDim, &dstPitch, &lockFormat, (uchar **)0);

    yOffset = (int32)((float)yDim * t);
    xOffset = (int32)((float)xDim * s);

    srcRow = (uchar *)srcBits + yOffset * srcPitch + xOffset * 2;
    dstRow = (uchar *)dstBits;

    row = yDim;
    if (row != 0) {
        do {
            uint32 dwordCount;
            uint32 byteCount;
            uint32 *src32;
            uint32 *dst32;

            dwordCount = (uint32)dstPitch >> 2;
            byteCount = (uint32)dstPitch & 3;
            src32 = (uint32 *)srcRow;
            dst32 = (uint32 *)dstRow;

            while (dwordCount != 0) {
                *dst32 = *src32;
                dst32++;
                src32++;
                dwordCount--;
            }

            {
                uchar *src8;
                uchar *dst8;

                src8 = (uchar *)src32;
                dst8 = (uchar *)dst32;

                while (byteCount != 0) {
                    *dst8 = *src8;
                    dst8++;
                    src8++;
                    byteCount--;
                }
            }

            srcRow += srcPitch;
            dstRow += dstPitch;
            row--;
        } while (row != 0);
    }

    if ((sourTexture->format != BTEXTUREFORMAT_256PALETTE_8888ARGB) &&
        (sourTexture->format != BTEXTUREFORMAT_16PALETTE_8888ARGB)) {
        (*sourTexture->frames)->UnlockRect(0);
    }

    if (destTexture->format != BTEXTUREFORMAT_256PALETTE_8888ARGB) {
        if (destTexture->format != BTEXTUREFORMAT_16PALETTE_8888ARGB) {
            (*destTexture->frames)->UnlockRect(0);
            return;
        }
        MakePalettedTexture16(destTexture);
        return;
    }

    MakePalettedTexture256(destTexture);
}
