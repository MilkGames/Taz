// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcRender.cpp
//   Purpose : Defines TBRenderStateInfo and TBRenderTarget structs. Declares global render state and render target lists. Provides prototypes for setting render states, default render states, fog parameters, global alpha, creating/deleting/setting render targets, getting render target info, setting render target as texture, and locking/unlocking render states
// Component : Babel Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Globals

TBRenderStateInfo bRenderState;
TBRenderTarget    bRenderTargetList;

// ********************************************************************************
// Locals Structures

// Triplet of MAG/MIN/MIP filters for a texture filter preset.
typedef struct BDFilterTriple
{
    D3DTEXTUREFILTERTYPE mag; // -> D3DTSS_MAGFILTER
    D3DTEXTUREFILTERTYPE min; // -> D3DTSS_MINFILTER
    D3DTEXTUREFILTERTYPE mip; // -> D3DTSS_MIPFILTER
} BDFilterTriple;

// Pair of blend factors for ALPHABLENDMODE (PC path sets only SRC/DST on DX8).
typedef struct BDBlendPair
{
    D3DBLEND src; // -> D3DRS_SRCBLEND
    D3DBLEND dst; // -> D3DRS_DESTBLEND
} BDBlendPair;

// ********************************************************************************
// Local variables

// Used in BDRENDERSTATE_TEXTUREFILTER
static const BDFilterTriple bFilterLUT[] = {
    /* 0 */ { D3DTEXF_NONE,   D3DTEXF_NONE,   D3DTEXF_NONE   }, // (none/none/none) - service entry
    /* 1 */ { D3DTEXF_POINT,  D3DTEXF_POINT,  D3DTEXF_NONE   }, // Point (no mip)
    /* 2 */ { D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_NONE   }, // Bilinear (no mip)
    /* 3 */ { D3DTEXF_POINT,  D3DTEXF_POINT,  D3DTEXF_POINT  }, // MipPoint
    /* 4 */ { D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_POINT  }, // MipLinear
    /* 5 */ { D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_LINEAR }, // Trilinear
};

// Used for D3DRS_ZFUNC in BDRENDERSTATE_ZTEST
static const D3DCMPFUNC bZFuncLUT[] = {
    /* 0 */ D3DCMP_NEVER,
    /* 1 */ D3DCMP_ALWAYS,
    /* 2 */ D3DCMP_LESSEQUAL,
    /* 3 */ D3DCMP_LESS,
    /* 4 */ D3DCMP_GREATEREQUAL,
    /* 5 */ D3DCMP_GREATER,
    /* 6 */ D3DCMP_EQUAL,
    /* 7 */ D3DCMP_NOTEQUAL,
};

// Used in BDRENDERSTATE_ALPHABLENDMODE
static const BDBlendPair bBlendModes[] = {
    /* 0 */ { D3DBLEND_SRCALPHA,   D3DBLEND_INVSRCALPHA  }, // classic alpha blend
    /* 1 */ { D3DBLEND_SRCALPHA,   D3DBLEND_ONE          }, // additive (alpha-scaled src)
    /* 2 */ { D3DBLEND_DESTCOLOR,  D3DBLEND_SRCCOLOR     }, // modulate-ish
    /* 3 */ { D3DBLEND_ZERO,       D3DBLEND_INVSRCCOLOR  }, // dest * (1 - srcColor)
    /* 4 */ { D3DBLEND_ONE,        D3DBLEND_ZERO         }, // copy source (opaque)
    /* 5 */ { D3DBLEND_DESTALPHA,  D3DBLEND_INVDESTALPHA }, // dest-alpha based
    /* 6 */ { D3DBLEND_DESTALPHA,  D3DBLEND_ONE          }, // additive weighted by dest alpha
};

// Used in BDRENDERSTATE_BACKFACECULL
static const D3DCULL bCullModeLUT[] = {
    /* 0 */ D3DCULL_NONE,
    /* 1 */ D3DCULL_CCW,
    /* 2 */ D3DCULL_CW,
};

// ********************************************************************************
// Local Functions



// ********************************************************************************
// Function Implementations

/* --------------------------------------------------------------------------------
   Function : bdSetRenderState
   Purpose : Set a render state
   Parameters : render state to set, first value, second value
   Returns : 
   Info : 
*/

void bdSetRenderState(uint32 renderState, uint32 value1, uint32 value2)
{
    if (bRenderState.renderStateLock[renderState] != 0)
        return;

    // cache the requested values in the renderer state
    bRenderState.renderState[renderState][0] = value1;
    bRenderState.renderState[renderState][1] = value2;

    switch (renderState)
    {
        case BDRENDERSTATE_TEXTUREFILTER:
        {
            // apply MAG/MIN/MIP for stages 0 and 1 per LUT
            const BDFilterTriple& f = bFilterLUT[value1];
            bDisplayInfo.d3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, f.mag);
            bDisplayInfo.d3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, f.min);
            bDisplayInfo.d3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, f.mip);

            bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_MAGFILTER, f.mag);
            bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_MINFILTER, f.min);
            bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_MIPFILTER, f.mip);
            return;
        }

		case BDRENDERSTATE_TEXTUREWRAP:
		{
			const DWORD addrU = (value1 & BDTEXTUREWRAP_WRAPU) ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP; // 1 or 3
			const DWORD addrV = (value1 & BDTEXTUREWRAP_WRAPV) ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP; // 1 or 3

			IDirect3DDevice8* dev = bDisplayInfo.d3dDevice;
			dev->SetTextureStageState(0, D3DTSS_ADDRESSU, addrU);
			dev->SetTextureStageState(0, D3DTSS_ADDRESSV, addrV);
			return;
		}

        case BDRENDERSTATE_LIGHTING:
            bDisplayInfo.d3dDevice->SetRenderState(D3DRS_LIGHTING, value1);
            return;

        case BDRENDERSTATE_BACKFACECULL:
            bDisplayInfo.d3dDevice->SetRenderState(D3DRS_CULLMODE, bCullModeLUT[value1]);
            return;

        case BDRENDERSTATE_ZWRITE:
            bDisplayInfo.d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, value1);
            return;

        case BDRENDERSTATE_ALPHAENABLE:
            bDisplayInfo.d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, value1);
            return;

        case BDRENDERSTATE_ZTEST:
            bDisplayInfo.d3dDevice->SetRenderState(D3DRS_ZFUNC, bZFuncLUT[value1]);
            return;

        case BDRENDERSTATE_DISTANCEFOG:
            bDisplayInfo.d3dDevice->SetRenderState(D3DRS_FOGENABLE, value1);
            return;

        case BDRENDERSTATE_ALPHABLENDMODE:
        {
            const BDBlendPair& bp = bBlendModes[value1];
            bDisplayInfo.d3dDevice->SetRenderState(D3DRS_SRCBLEND,  bp.src);
            bDisplayInfo.d3dDevice->SetRenderState(D3DRS_DESTBLEND, bp.dst);
            return;
        }

        case BDRENDERSTATE_FRAMEWRITE:
        {
			// Build COLORWRITEENABLE mask
			DWORD m = 0;
			if (value1 & 0x01) {
				m = D3DCOLORWRITEENABLE_RED
					| D3DCOLORWRITEENABLE_GREEN
					| D3DCOLORWRITEENABLE_BLUE
					| D3DCOLORWRITEENABLE_ALPHA;
			} else {
				if (value1 & 0x02) m |= D3DCOLORWRITEENABLE_RED;
				if (value1 & 0x04) m |= D3DCOLORWRITEENABLE_GREEN;
				if (value1 & 0x08) m |= D3DCOLORWRITEENABLE_BLUE;
				if (value1 & 0x10) m |= D3DCOLORWRITEENABLE_ALPHA;
			}

            // D3D expects the D3DCOLORWRITEENABLE_* bitfield (low 4 bits map to RGBA)
            // The decompiled bVar1 accumulates in the same low 4 bits
            bDisplayInfo.d3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, m);
            return;
        }

        case BDRENDERSTATE_2DWORLDMATRIX:
		{
            const D3DMATRIX* world =
				(value1 != 0)
				? (const D3DMATRIX*)(void*)&bViewInfo.objectMatrix
				: (const D3DMATRIX*)(void*)&bIdentityMatrix;

			bDisplayInfo.d3dDevice->SetTransform(D3DTS_WORLD, world);
            return;
		}

        case BDRENDERSTATE_TEXTUREENABLE:
		{
            bRenderState.textureEnable[value1] = value2;
            return;
		}
    }
}


/* --------------------------------------------------------------------------------
   Function : bSetDefaultRenderStates
   Purpose : Set the default render states
   Parameters : 
   Returns : 
   Info : 
*/

void bSetDefaultRenderStates()
{
    // Zero the whole cache
    memset(&bRenderState, 0, sizeof(TBRenderStateInfo));

    // Engine defaults (enable 2 texture stages on PC)
    bSetInitialRenderStates(BMAX_TEXTURE_STAGES);

    // Raw D3D8 state pokes Ч numeric IDs
    LPDIRECT3DDEVICE8 dev = bDisplayInfo.d3dDevice;
    dev->SetRenderState((D3DRENDERSTATETYPE)0x1A, 1);          // DITHERENABLE = TRUE
    dev->SetRenderState((D3DRENDERSTATETYPE)0x93, 1);
    dev->SetRenderState((D3DRENDERSTATETYPE)0x92, 0);
    dev->SetRenderState((D3DRENDERSTATETYPE)0x0F, 1);          // ALPHATESTENABLE = TRUE
    dev->SetRenderState((D3DRENDERSTATETYPE)0x19, 5);
    dev->SetRenderState((D3DRENDERSTATETYPE)0x18, 0);

    dev->SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE)4, 4);
    dev->SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE)5, 2);
    dev->SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE)6, 0);

    dev->SetRenderState((D3DRENDERSTATETYPE)0x8C, 3);

    dev->SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE)0x0B, 0);
    dev->SetTextureStageState(1, (D3DTEXTURESTAGESTATETYPE)0x0B, 1);

    dev->SetRenderState((D3DRENDERSTATETYPE)0x9B, 0);
    dev->SetRenderState((D3DRENDERSTATETYPE)0x9E, 0);
    dev->SetRenderState((D3DRENDERSTATETYPE)0x9F, 0);
    dev->SetRenderState((D3DRENDERSTATETYPE)0xA0, 0x3f800000); // 1.0f as DWORD
}


/* --------------------------------------------------------------------------------
   Function : bdSetFogRange
   Purpose : Set the ranges for distance fog
   Parameters : near distance, far distance
   Returns : 
   Info : 
*/

void bdSetFogRange(float nearDist, float farDist)
{
    IDirect3DDevice8* dev = bDisplayInfo.d3dDevice;

    // Device states: FOGSTART (0x24), FOGEND (0x25)
    if (dev) {
        // Pass float bits as DWORD (D3D8 expects DWORD payload)
        dev->SetRenderState(D3DRS_FOGSTART, *(const DWORD*)&nearDist);
        dev->SetRenderState(D3DRS_FOGEND,   *(const DWORD*)&farDist);
    }

    // Mirror into our software state and precompute reciprocal range
    bRenderState.fogFar   = farDist;
    bRenderState.fogNear  = nearDist;
    bRenderState.fogRange = 1.0f / (farDist - nearDist);
}


/* --------------------------------------------------------------------------------
   Function : bdSetFogColour
   Purpose : Set the colour for distance fog
   Parameters : red, green, blue (each 0..255)
   Returns : 
   Info : 
*/

void bdSetFogColour(int red, int green, int blue)
{
    IDirect3DDevice8* dev = bDisplayInfo.d3dDevice;

    // Build ARGB = 0xFFRRGGBB (alpha forced to 0xFF)
    DWORD color =
        (0xFFu << 24) |
        ((DWORD)(red   & 0xFF) << 16) |
        ((DWORD)(green & 0xFF) <<  8) |
        ((DWORD)(blue  & 0xFF) <<  0);

    if (dev) {
        dev->SetRenderState(D3DRS_FOGCOLOR, color);
    }

    // Mirror to our software state
    bRenderState.rFog = red;
    bRenderState.gFog = green;
    bRenderState.bFog = blue;
}


/*	--------------------------------------------------------------------------------
	Function : bSetGlobalAlpha
	Purpose : set the global alpha scale value for actor drawing
	Parameters : new alpha scale value (256=solid)
	Returns : 
	Info : platform specific code called from baSetGlobalAlpha
*/

void bSetGlobalAlpha(int newAlphaScale)
{
    if (newAlphaScale == 256) newAlphaScale = 255;

    bDisplayInfo.d3dDevice->SetRenderState(
        D3DRS_TEXTUREFACTOR,
        static_cast<DWORD>(newAlphaScale) << 24
    );
}


/*	--------------------------------------------------------------------------------
	Function : bdCreateRenderTarget
	Purpose : create an off-screen render target
	Parameters : width in pixels, height in pixels, rgb depth in bits, Z depth in bits, flags (see BCREATERENDERTARGET_)
	Returns : ptr to target or NULL for failure
	Info : 
*/

TBRenderTarget *bdCreateRenderTarget(int width, int height, int rgbBits, int zBits, uint32 flags)
{
    TBRenderTarget *target;
    uint bitMask;
    const char *formatDepthStr;
    HRESULT hr;
    D3DFORMAT depthFormat;
    D3DFORMAT colorFormat;
    D3DFORMAT rgbDepthFormat;
    int hasDepthStencil;
    const char *formatColorStr;
    int mipLevelCount;
    int mipLevelIndex;
    IDirect3DTexture8 **outTexturePtr;
    IDirect3DSurface8 *surface;
    IDirect3DTexture8 *texture;

    // Allocate render target object
    target = (TBRenderTarget *)MALLOC(sizeof(TBRenderTarget));
    if (!target)
    {
        return NULL;
    }

    // Original code zeroed first 0x18 dwords (0x60 bytes) via REP STOSD.
    // That matches clearing the "header" of TBRenderTarget.
    memset(target, 0, 0x18 * sizeof(uint32));

    // ===== Enforce power-of-two size =====
    {
        int origWidth  = width;
        int origHeight = height;

        // round width down to nearest power of two
        bitMask = 0x80000000u;
        while (bitMask && (width & bitMask) == 0)
        {
            bitMask >>= 1;
        }
        if (bitMask == 0)
            bitMask = 0x20; // fallback
        else
        {
            
        }
        if (width != (int)bitMask)
            width = (int)bitMask;

        // round height down to nearest power of two
        bitMask = 0x80000000u;
        while (bitMask && (height & bitMask) == 0)
        {
            bitMask >>= 1;
        }
        if (bitMask == 0)
            bitMask = 0x20;
        else
        {
            
        }
        if (height != (int)bitMask)
            height = (int)bitMask;

        if (width != origWidth || height != origHeight)
        {
            bkPrintf(
                "bdCreateRenderTarget: PC Render targets must be powers of two in each dimension, "
                "rounding down to (%dx%d)\n",
                width, height);
        }
    }

    // Common locals init
    depthFormat     = D3DFMT_UNKNOWN;
    colorFormat     = D3DFMT_UNKNOWN;
    rgbDepthFormat  = D3DFMT_UNKNOWN;
    mipLevelIndex   = 0;
    mipLevelCount   = 1;
    outTexturePtr   = &target->d3dTexture;
    hasDepthStencil = 0;

    // ========================================================================
    //  CASE 1: (flags & 1) == 0   > no private Z-buffer, clamp to screen size
    // ========================================================================
    if ((flags & 1) == 0)
    {
        // Clamp to screen resolution
        if (width > (int)bDisplayInfo.xRes)
        {
            bkPrintf(
                "bdCreateRenderTarget: *** specified width (%d) exceeds screen width (%d) "
                "and cannot be used without a private Z buffer, clamping to %d ***\n",
                width, bDisplayInfo.xRes, bDisplayInfo.xRes);
            width = (int)bDisplayInfo.xRes;
        }

        if (height > (int)bDisplayInfo.yRes)
        {
            bkPrintf(
                "bdCreateRenderTarget: *** specified height (%d) exceeds screen height (%d) "
                "and cannot be used without a private Z buffer, clamping to %d ***\n",
                height, bDisplayInfo.yRes, bDisplayInfo.yRes);
            height = (int)bDisplayInfo.yRes;
        }

        // In this mode we just share the screen depth buffer, so only color RT texture.
        rgbDepthFormat = bDisplayInfo.displayFormat;

        hr = bDisplayInfo.d3dDevice->CreateTexture(
            width,
            height,
            1,                               // Levels
            D3DUSAGE_RENDERTARGET,           // Usage
            bDisplayInfo.displayFormat,      // Format (same as display)
            D3DPOOL_DEFAULT,                 // Pool
            &target->d3dTexture);

        if (FAILED(hr))
        {
            formatDepthStr = DXGetErrorString8A(hr);
            bkPrintf("bdCreateRenderTarget: CreateTexture failure (%s)\n", formatDepthStr);
            bkHeapFree(target);
            return NULL;
        }

        target->d3dSurface = NULL;
        target->rgbFormat  = bDisplayInfo.displayFormat;
        // depthFormat остаЄтс€ D3DFMT_UNKNOWN (share screen depth)
    }
    else
    {
        // ====================================================================
        //  CASE 2: private render target (flags & 1) != 0
        //          2a) zBits == 0 > no depth/stencil
        //          2b) zBits != 0 > private depth/stencil surface
        // ====================================================================

        if (zBits == 0)
        {
            // ------------------ Color-only private render target ------------------

            colorFormat    = D3DFMT_UNKNOWN;
            rgbDepthFormat = D3DFMT_UNKNOWN;

            // Select color format by rgbBits
            if (rgbBits == 16)
            {
                // Try 16-bit color formats in order: R5G6B5, X1R5G5B5, A1R5G5B5
                if (SUCCEEDED(bDisplayInfo.d3d->CheckDeviceFormat(
                        0,
                        D3DDEVTYPE_HAL,
                        bDisplayInfo.displayFormat,
                        D3DUSAGE_RENDERTARGET,
                        D3DRTYPE_TEXTURE,
                        D3DFMT_R5G6B5)))
                {
                    colorFormat = D3DFMT_R5G6B5;
                }
                else if (SUCCEEDED(bDisplayInfo.d3d->CheckDeviceFormat(
                             0,
                             D3DDEVTYPE_HAL,
                             bDisplayInfo.displayFormat,
                             D3DUSAGE_RENDERTARGET,
                             D3DRTYPE_TEXTURE,
                             D3DFMT_X1R5G5B5)))
                {
                    colorFormat = D3DFMT_X1R5G5B5;
                }
                else if (SUCCEEDED(bDisplayInfo.d3d->CheckDeviceFormat(
                             0,
                             D3DDEVTYPE_HAL,
                             bDisplayInfo.displayFormat,
                             D3DUSAGE_RENDERTARGET,
                             D3DRTYPE_TEXTURE,
                             D3DFMT_A1R5G5B5)))
                {
                    colorFormat = D3DFMT_A1R5G5B5;
                }
                else
                {
                    // fall back to 32-bit
                    bkPrintf(
                        "bdCreateRenderTarget: Could not find a compatible 16bit render/texture format, "
                        "trying 32bit\n");
                }
            }

            if (colorFormat == D3DFMT_UNKNOWN)
            {
                // Either rgbBits was 32, or 16-bit formats failed. Try 32-bit formats.
                if (SUCCEEDED(bDisplayInfo.d3d->CheckDeviceFormat(
                        0,
                        D3DDEVTYPE_HAL,
                        bDisplayInfo.displayFormat,
                        D3DUSAGE_RENDERTARGET,
                        D3DRTYPE_TEXTURE,
                        D3DFMT_A8R8G8B8)))
                {
                    colorFormat = D3DFMT_A8R8G8B8;
                }
                else if (SUCCEEDED(bDisplayInfo.d3d->CheckDeviceFormat(
                             0,
                             D3DDEVTYPE_HAL,
                             bDisplayInfo.displayFormat,
                             D3DUSAGE_RENDERTARGET,
                             D3DRTYPE_TEXTURE,
                             D3DFMT_X8R8G8B8)))
                {
                    colorFormat = D3DFMT_X8R8G8B8;
                }
                else
                {
                    bkHeapFree(target);
                    bkPrintf(
                        "bdCreateRenderTarget: Could not find a compatible 32bit render/texture format\n");
                    return NULL;
                }
            }

            rgbDepthFormat = colorFormat;

            formatColorStr = bDXFormatToString(colorFormat);
            formatDepthStr = bDXFormatToString(D3DFMT_UNKNOWN);
            bkPrintf(
                "bdCreateRenderTarget: Creating (%dx%d) render target with surface format %s, "
                "no depth/stencil buffer\n",
                width, height, formatColorStr, formatDepthStr);

            hr = bDisplayInfo.d3dDevice->CreateTexture(
                width,
                height,
                1,
                D3DUSAGE_RENDERTARGET,
                colorFormat,
                D3DPOOL_DEFAULT,
                &target->d3dTexture);

            if (FAILED(hr))
            {
                formatDepthStr = DXGetErrorString8A(hr);
                bkPrintf(
                    "bdCreateRenderTarget: CreateTexture failure on render target (%s)\n",
                    formatDepthStr);
                bkHeapFree(target);
                return NULL;
            }

            target->d3dSurface = NULL;
            target->rgbFormat  = colorFormat;
            depthFormat        = D3DFMT_UNKNOWN;
        }
        else
        {
            // ------------------ Color + private depth/stencil ------------------

            // if rgbBits != zBits, force both to same value and warn
            if (rgbBits != zBits)
            {
                bkPrintf(
                    "bdCreateRenderTarget: *** RGB and Z depths do not match (rgb %d, z %d), "
                    "forcing both to %d\n",
                    rgbBits, zBits, rgbBits);
                zBits = rgbBits;
            }

            depthFormat    = D3DFMT_UNKNOWN;
            colorFormat    = D3DFMT_UNKNOWN;
            rgbDepthFormat = D3DFMT_UNKNOWN;

            // First try 16-bit pair (color + D16) if rgbBits==16
            if (rgbBits == 16)
            {
                // Choose 16-bit color format as before
                if (SUCCEEDED(bDisplayInfo.d3d->CheckDeviceFormat(
                        0,
                        D3DDEVTYPE_HAL,
                        bDisplayInfo.displayFormat,
                        D3DUSAGE_RENDERTARGET,
                        D3DRTYPE_TEXTURE,
                        D3DFMT_R5G6B5)))
                {
                    colorFormat = D3DFMT_R5G6B5;
                }
                else if (SUCCEEDED(bDisplayInfo.d3d->CheckDeviceFormat(
                             0,
                             D3DDEVTYPE_HAL,
                             bDisplayInfo.displayFormat,
                             D3DUSAGE_RENDERTARGET,
                             D3DRTYPE_TEXTURE,
                             D3DFMT_X1R5G5B5)))
                {
                    colorFormat = D3DFMT_X1R5G5B5;
                }
                else if (SUCCEEDED(bDisplayInfo.d3d->CheckDeviceFormat(
                             0,
                             D3DDEVTYPE_HAL,
                             bDisplayInfo.displayFormat,
                             D3DUSAGE_RENDERTARGET,
                             D3DRTYPE_TEXTURE,
                             D3DFMT_A1R5G5B5)))
                {
                    colorFormat = D3DFMT_A1R5G5B5;
                }

                if (colorFormat != D3DFMT_UNKNOWN &&
                    SUCCEEDED(bDisplayInfo.d3d->CheckDeviceFormat(
                        0,
                        D3DDEVTYPE_HAL,
                        bDisplayInfo.displayFormat,
                        D3DUSAGE_DEPTHSTENCIL,
                        D3DRTYPE_SURFACE,
                        D3DFMT_D16)))
                {
                    depthFormat = D3DFMT_D16;
                }
                else
                {
                    if (colorFormat == D3DFMT_UNKNOWN)
                    {
                        bkPrintf(
                            "bdCreateRenderTarget: Could not find a compatible 16bit render/texture format, "
                            "trying 32bit\n");
                    }
                    else
                    {
                        bkPrintf(
                            "bdCreateRenderTarget: Could not find a compatible 16bit depth/stencil format, "
                            "trying 32bit\n");
                    }
                    colorFormat = D3DFMT_UNKNOWN;
                    depthFormat = D3DFMT_UNKNOWN;
                }
            }

            // If 16-bit pair not selected, try 32-bit pair (color + 32-bit depth)
            if (colorFormat == D3DFMT_UNKNOWN || depthFormat == D3DFMT_UNKNOWN)
            {
                // Color 32-bit
                if (SUCCEEDED(bDisplayInfo.d3d->CheckDeviceFormat(
                        0,
                        D3DDEVTYPE_HAL,
                        bDisplayInfo.displayFormat,
                        D3DUSAGE_RENDERTARGET,
                        D3DRTYPE_TEXTURE,
                        D3DFMT_A8R8G8B8)))
                {
                    colorFormat = D3DFMT_A8R8G8B8;
                }
                else if (SUCCEEDED(bDisplayInfo.d3d->CheckDeviceFormat(
                             0,
                             D3DDEVTYPE_HAL,
                             bDisplayInfo.displayFormat,
                             D3DUSAGE_RENDERTARGET,
                             D3DRTYPE_TEXTURE,
                             D3DFMT_X8R8G8B8)))
                {
                    colorFormat = D3DFMT_X8R8G8B8;
                }
                else
                {
                    bkHeapFree(target);
                    bkPrintf(
                        "bdCreateRenderTarget: Could not find a compatible 32bit render/texture format\n");
                    return NULL;
                }

                // Depth 32-bit: first D32, then D24S8
                if (SUCCEEDED(bDisplayInfo.d3d->CheckDeviceFormat(
                        0,
                        D3DDEVTYPE_HAL,
                        bDisplayInfo.displayFormat,
                        D3DUSAGE_DEPTHSTENCIL,
                        D3DRTYPE_SURFACE,
                        D3DFMT_D32)))
                {
                    depthFormat = D3DFMT_D32;
                }
                else if (SUCCEEDED(bDisplayInfo.d3d->CheckDeviceFormat(
                             0,
                             D3DDEVTYPE_HAL,
                             bDisplayInfo.displayFormat,
                             D3DUSAGE_DEPTHSTENCIL,
                             D3DRTYPE_SURFACE,
                             D3DFMT_D24S8)))
                {
                    depthFormat = D3DFMT_D24S8;
                }
                else
                {
                    bkHeapFree(target);
                    bkPrintf(
                        "bdCreateRenderTarget: Could not find a compatible 32bit depth/stencil format\n");
                    return NULL;
                }
            }

            rgbDepthFormat = colorFormat;
            hasDepthStencil = 1;

            formatColorStr = bDXFormatToString(colorFormat);
            formatDepthStr = bDXFormatToString(depthFormat);
            bkPrintf(
                "bdCreateRenderTarget: Creating (%dx%d) render target with surface format %s, "
                "depth/stencil format %s...\n",
                width, height, formatColorStr, formatDepthStr);

            // Create color render target texture
            hr = bDisplayInfo.d3dDevice->CreateTexture(
                width,
                height,
                1,
                D3DUSAGE_RENDERTARGET,
                colorFormat,
                D3DPOOL_DEFAULT,
                &target->d3dTexture);

            if (FAILED(hr))
            {
                formatDepthStr = DXGetErrorString8A(hr);
                bkPrintf(
                    "bdCreateRenderTarget: CreateTexture failure on render target (%s)\n",
                    formatDepthStr);
                bkHeapFree(target);
                return NULL;
            }

            // Create depth/stencil surface
            hr = bDisplayInfo.d3dDevice->CreateDepthStencilSurface(
                width,
                height,
                depthFormat,
                D3DMULTISAMPLE_NONE,
                &target->d3dSurface);

            if (FAILED(hr))
            {
                formatDepthStr = DXGetErrorString8A(hr);
                bkPrintf(
                    "bdCreateRenderTarget: CreateTexture failure on depth/stencil buffer (%s)\n",
                    formatDepthStr);

                texture = target->d3dTexture;
                if (texture)
                {
                    texture->Release();
                }

                bkHeapFree(target);
                return NULL;
            }

            target->zFormat   = depthFormat;
            target->rgbFormat = colorFormat;
        }
    }

    // ========================================================================
    //  Common success path: get color surface level 0 and set up TBRenderTarget
    // ========================================================================

    texture = target->d3dTexture;
    hr = texture->GetSurfaceLevel(0, &target->d3dZBuffer);
    if (FAILED(hr))
    {
        formatDepthStr = DXGetErrorString8A(hr);
        bkPrintf(
            "bdCreateRenderTarget: GetSurfaceLevel failure (%s)\n",
            formatDepthStr);

        // Release texture
        if (texture)
        {
            texture->Release();
        }

        // Release depth surface if it exists
        surface = target->d3dSurface;
        if (surface)
        {
            surface->Release();
        }

        bkHeapFree(target);
        return NULL;
    }

    // Fill out render target fields
    target->rgbDepth   = rgbDepthFormat;
    target->zDepth     = mipLevelIndex;
    target->vpX        = 0;
    target->vpY        = 0;
    target->width      = width;
    target->vpWidth    = width;
    target->flags      = flags;
    target->clipXPos   = 0;
    target->clipYPos   = 0;
    target->clipWidth  = width;
    target->height     = height;
    target->vpHeight   = height;
    target->clipHeight = height;

    // Insert into bRenderTargetList (double-linked list, tail insertion)
    target->next = &bRenderTargetList;
    target->prev = bRenderTargetList.prev;
    bRenderTargetList.prev->next = target;
    bRenderTargetList.prev       = target;

    return target;
}


/*	--------------------------------------------------------------------------------
	Function : bdDeleteRenderTarget
	Purpose : delete an off-screen render target
	Parameters : ptr to render target or NULL to delete all render targets
	Returns : 
	Info : 
*/

void bdDeleteRenderTarget(TBRenderTarget *target)
{
    uint32 stage;
    IDirect3DSurface8* surface;

    if (target == NULL)
    {
        while (bRenderTargetList.next != &bRenderTargetList)
        {
            bdDeleteRenderTarget(bRenderTargetList.next);
        }
        return;
    }

    if (bDisplayInfo.curRenderTarget == target)
    {
        bdSetRenderTarget(NULL, 0, 0, 0, 0, 1.0f, 0);
    }

    stage = 0;
    do
    {
        if ((void*)bRenderState.currentTexture[stage] == (void*)target)
        {
            bdSetTexture(stage, NULL);
        }
        stage++;
    } while (stage < 2);

    target->next->prev = target->prev;
    target->prev->next = target->next;

    target->d3dZBuffer->Release();
    target->d3dTexture->Release();

    surface = target->d3dSurface;
    if (surface != NULL)
    {
        surface->Release();
    }

    bkHeapFree(target);
}


/*	--------------------------------------------------------------------------------
	Function : bdSetRenderTarget
	Purpose : set the current render target
	Parameters : ptr to render target or NULL to use default screen, clear colour, clear depth,
																						flags (see BSETRENDERTARGET_)
	Returns : OK/FAIL
	Info : 
*/

int bdSetRenderTarget(TBRenderTarget *target, int r,int g,int b, int a,float depth, uint32 flags)
{
    IDirect3DDevice8 *dev = bDisplayInfo.d3dDevice;
    HRESULT hr;

    IDirect3DSurface8 *colorSurf;
    IDirect3DSurface8 *depthSurf;
    TBRenderTarget *rt;

    if (target != NULL)
    {
        // disasm: push [target+0x84] as depth, push [target+0x80] as color
        colorSurf = target->d3dZBuffer;

        if (target->d3dSurface != NULL)
            depthSurf = target->d3dSurface;
        else if (target->zDepth != 0)
            depthSurf = bDisplayInfo.depthStencilBuffer;
        else
            depthSurf = NULL;

        rt = target;
    }
    else
    {
        colorSurf = bDisplayInfo.backBuffer;
        depthSurf = bDisplayInfo.depthStencilBuffer;
        rt = &bRenderTargetList;
    }

    hr = dev->SetRenderTarget(colorSurf, depthSurf);
    if (FAILED(hr))
    {
        bkPrintf("bdSetRenderTarget: SetRenderTarget failed (%s)\n", DXGetErrorString8A(hr));
        return FAIL;
    }

    bDisplayInfo.curRenderTarget = rt;

    bdSetViewport(rt->vpX, rt->vpY, rt->vpWidth, rt->vpHeight);
    bdSetClipRectangle(rt->clipXPos, rt->clipYPos, rt->clipWidth, rt->clipHeight);

    // disasm: flags bit0=color, bit1=z; passes 1/2/3 directly to Clear
    {
        const uint32 clearFlags = (flags & 0x3);
        if (clearFlags != 0)
        {
            uint32 clr = 0;
            if ((clearFlags & 0x1) != 0)
            {
                clr = D3DCOLOR_ARGB((a & 0xFF), (r & 0xFF), (g & 0xFF), (b & 0xFF));
            }

            dev->Clear(0, NULL, clearFlags, (D3DCOLOR)clr, depth, 0);
        }
    }

    return OK;
}


/*	--------------------------------------------------------------------------------
	Function : bdGetRenderTargetInfo
	Purpose : return information for a render target
	Parameters : ptr to render target, ptr for width, ptr for height, ptr for RGB depth, ptr for Z depth
	Returns : 
	Info : width, height or depth ptrs can be NULL
*/

void bdGetRenderTargetInfo(TBRenderTarget *target, int *width, int *height, int *rgbDepth, int *zDepth)
{
	if (width != NULL) *width = target->width;
	if (height != NULL) *height = target->height;
	if (rgbDepth != NULL) *rgbDepth = target->rgbDepth;
	if (zDepth != NULL) *zDepth = target->zDepth;
}


/*	--------------------------------------------------------------------------------
	Function : bdSetRenderTargetAsTexture
	Purpose : set a render target as a texture
	Parameters : ptr to render target, zero-based texture stage
	Returns : OK/FAIL
	Info : 
*/

int bdSetRenderTargetAsTexture(TBRenderTarget *target, int stage)
{
    if (target == NULL)
        return FAIL;

    if ((bRenderState.currentTexture[stage] == NULL) && (bDisplayInfo.modulate2X != 0) && (stage == 0))
    {
        bDisplayInfo.d3dDevice->SetTextureStageState((DWORD)stage, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
        bDisplayInfo.d3dDevice->SetTextureStageState((DWORD)stage, D3DTSS_ALPHAOP, D3DTOP_MODULATE2X);
    }

    LPDIRECT3DDEVICE8 dev = bDisplayInfo.d3dDevice;

    bRenderState.currentTexture[stage] = &target->dummyTexture;

    const HRESULT hr = dev->SetTexture((DWORD)stage, target->d3dTexture);
    if (hr < 0)
    {
        const char* err = DXGetErrorString8A(hr);
        bkPrintf("bdSetRenderTargetAsTexture: SetTexture failed (%s)\n", err);
        return FAIL;
    }

    return OK;
}

/* --------------------------------------------------------------------------------
   Function : bSuspendRenderTargets
   Purpose : suspend render buffers
   Parameters : 
   Returns : 
   Info : 
*/

void bSuspendRenderTargets()
{
	bkPrintf("*** WARNING *** bSuspendRenderTargets was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
}

/* --------------------------------------------------------------------------------
   Function : bResumeRenderTargets
   Purpose : resume render buffers
   Parameters : 
   Returns : 
   Info : 
*/

void bResumeRenderTargets()
{
    TBRenderTarget* rt = bRenderTargetList.next;
    if (rt == &bRenderTargetList) return;

    do
    {
        // Create the color render-target texture: 1 level, RENDER_TARGET usage, DEFAULT pool.
        HRESULT hr = bDisplayInfo.d3dDevice->CreateTexture(
            (UINT)rt->width,
            (UINT)rt->height,
            1,                               // Levels
            D3DUSAGE_RENDERTARGET,           // Usage
            (D3DFORMAT)rt->rgbFormat,        // Color format
            D3DPOOL_DEFAULT,                 // Pool
            &rt->d3dTexture                  // Out
        );

        if (hr < 0)
        {
            const char* es = DXGetErrorString8A(hr);
            bkPrintf("bResumeRenderTargets: CreateTexture failure on render target (%s)\n", es);
        }
        else
        {
            // If a depth/stencil was in use before, recreate it.
            if (rt->d3dSurface) // note: matches the non-null check in the disasm
            {
                hr = bDisplayInfo.d3dDevice->CreateDepthStencilSurface(
                    (UINT)rt->width,
                    (UINT)rt->height,
                    (D3DFORMAT)rt->zFormat,
                    D3DMULTISAMPLE_NONE,
                    &rt->d3dSurface
                );

                if (hr < 0)
                {
                    const char* es = DXGetErrorString8A(hr);
                    bkPrintf("bResumeRenderTargets: CreateTexture failure on depth/stencil buffer (%s)\n", es);
                }
            }

            // Grab surface level 0 from the color RT texture.
            if (rt->d3dTexture)
            {
                hr = rt->d3dTexture->GetSurfaceLevel(0, &rt->d3dZBuffer);
                if (hr < 0)
                {
                    const char* es = DXGetErrorString8A(hr);
                    bkPrintf("bResumeRenderTargets: GetSurfaceLevel failure (%s)\n", es);
                }
            }
        }

        rt = rt->next;
    }
    while (rt != &bRenderTargetList);
}

/*	--------------------------------------------------------------------------------
	Function : bdLockRenderState
	Purpose : increment the lock count on a render state
	Parameters : render state
	Returns : new lock count
	Info : 
*/

int bdLockRenderState(int renderState)
{
    return ++bRenderState.renderStateLock[renderState];
}


/*	--------------------------------------------------------------------------------
	Function : bdUnlockRenderState
	Purpose : decrement the lock count on a render state
	Parameters : render state
	Returns : new lock count
	Info : 
*/

int bdUnlockRenderState(int renderState)
{
    return --bRenderState.renderStateLock[renderState];
}