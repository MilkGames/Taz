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

    // Raw D3D8 state pokes — numeric IDs
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
        bkPrintf("*** WARNING *** bSetGlobalAlpha was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
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
        bkPrintf("*** WARNING *** bdCreateRenderTarget was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
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
        bkPrintf("*** WARNING *** bdDeleteRenderTarget was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
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
    IDirect3DDevice8* dev = bDisplayInfo.d3dDevice;
    HRESULT hr;

    // Select color/depth surfaces
    IDirect3DSurface8* colorSurf = NULL;
    IDirect3DSurface8* depthSurf = NULL;

    if (target)
    {
        colorSurf = target->d3dSurface; // color RT surface
        if (target->d3dZBuffer) {
            depthSurf = target->d3dZBuffer; // explicit z/stencil surface on the RT
        } else if (target->zDepth) {
            depthSurf = bDisplayInfo.depthStencilBuffer; // fall back to the device DS buffer
        }
    }
    else
    {
        // Back to the main backbuffer & device depth/stencil
        colorSurf = bDisplayInfo.backBuffer;
        depthSurf = bDisplayInfo.depthStencilBuffer;
    }

    // SetRenderTarget
    hr = dev->SetRenderTarget(colorSurf, depthSurf);
    if (FAILED(hr))
    {
        bkPrintf("bdSetRenderTarget: SetRenderTarget failed (%s)\n", DXGetErrorString8A(hr));
        return 0;
    }

    // Update current RT pointer: null -> head of list, otherwise the passed target
    bDisplayInfo.curRenderTarget = target ? target : &bRenderTargetList;

    // Restore viewport and clip rect that belong to this render target
    const TBRenderTarget* rt = bDisplayInfo.curRenderTarget;
    bdSetViewport(rt->vpX, rt->vpY, rt->vpWidth, rt->vpHeight);
    bdSetClipRectangle(rt->clipXPos, rt->clipYPos, rt->clipWidth, rt->clipHeight);

    // Optional clear according to flags
    // Flags (observed in disasm): bit 0 = color, bit 1 = zbuffer
    const bool clearColor = (flags & 0x1) != 0;
    const bool clearZ     = (flags & 0x2) != 0;

    if (clearColor || clearZ)
    {
        DWORD clr = 0;
        if (clearColor)
        {
            // Disasm packs 24-bit RGB (alpha not used here)
            clr = ((DWORD)(r & 0xFF))
                | ((DWORD)(g & 0xFF) << 8)
                | ((DWORD)(b & 0xFF) << 16);
        }

        DWORD d3dFlags = 0;
        if (clearColor) d3dFlags |= D3DCLEAR_TARGET;
        if (clearZ)     d3dFlags |= D3DCLEAR_ZBUFFER;

        // Stencil value is 0, Count/pRects are 0/NULL
        dev->Clear(0, NULL, d3dFlags, clr, depth, 0);
    }

    return 1;
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
        bkPrintf("*** WARNING *** bdGetRenderTargetInfo was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
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
        bkPrintf("*** WARNING *** bdSetRenderTargetAsTexture was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
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
        bkPrintf("*** WARNING *** bdLockRenderState was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
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
        bkPrintf("*** WARNING *** bdUnlockRenderState was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}