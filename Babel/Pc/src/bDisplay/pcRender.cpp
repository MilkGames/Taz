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
            // Well, just keep the decompiled bit math to match behavior 1:1.
            // Note: This maps to D3DTADDRESS_WRAP (1) vs D3DTADDRESS_MIRROR (2).
            // value1 bit assignment comes from BDTEXTUREWRAP_* flags in render.h.
			// MG: TODO on this
            DWORD addrU = ((~value1) & BDTEXTUREWRAP_WRAPU) ? 2 : 1; // ((~v & 1) << 1) | 1  -> 2 or 1
            DWORD addrV = ((~value1) & BDTEXTUREWRAP_WRAPV) ? 2 : 1; // ((~v & 2)     ) | 1  -> 3? (but with these flags becomes 2 or 1)
            bDisplayInfo.d3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, addrU);
            bDisplayInfo.d3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, addrV);
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
            BYTE m = ((value1 & 0x03) != 0) ? 1 : 0; // initial from (value1 & 3)
            if ((value1 & 0x05) != 0) m |= 2;
            if ((value1 & 0x09) != 0) m |= 4;
            if ((value1 & 0x11) != 0) m |= 8;

            // D3D expects the D3DCOLORWRITEENABLE_* bitfield (low 4 bits map to RGBA).
            // The decompiled bVar1 accumulates in the same low 4 bits.
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
        bkPrintf("*** WARNING *** bdSetFogRange was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
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
        bkPrintf("*** WARNING *** bdSetFogColour was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
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
        bkPrintf("*** WARNING *** bdSetRenderTarget was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
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
	bkPrintf("*** WARNING *** bResumeRenderTargets was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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