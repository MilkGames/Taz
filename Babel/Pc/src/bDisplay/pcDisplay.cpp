// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcDisplay.cpp
//   Purpose : Contains PC/Xbox specific display functions. Includes DirectX headers, defines macros for D3DDevice functions, pixel formats, and D3D memory usage types
// Component : Babel Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Globals

// MG: TEMP LINKING CODE
uint bRColLUT[256], bGColLUT[256], bBColLUT[256], bAColLUT[256];
uint bRGBInvColLUT[256], bAInvColLUT[256];
float bRGBNormLUT[256];

GUID  bDisplayAdapterGUIDBuffer;
GUID *bDisplayAdapterGuid = NULL;
GUID *bSoundHardwareGuid = NULL;               
unsigned char bDirectInputKeyMap[256];    
TBInputInfo bInputInfo;                   
float bFPS = 0.0f;

TBDisplayInfo bDisplayInfo;
uint bFlipCount;
TBStopwatch bFpsStopwatch;

// ********************************************************************************
// Locals

static int bSceneStarted = 0;
static int bDisplayInit;

// Saved render/texture stage states
const D3DRENDERSTATETYPE bSavedRS_IDs[56] = {
    D3DRS_ZENABLE,                // 7
    D3DRS_FILLMODE,               // 8
    D3DRS_SHADEMODE,              // 9
    D3DRS_LINEPATTERN,            // 10
    D3DRS_ZWRITEENABLE,           // 14
    D3DRS_ALPHATESTENABLE,        // 15
    D3DRS_LASTPIXEL,              // 16
    D3DRS_SRCBLEND,               // 19
    D3DRS_DESTBLEND,              // 20
    D3DRS_CULLMODE,               // 22
    D3DRS_ZFUNC,                  // 23
    D3DRS_ALPHAREF,               // 24
    D3DRS_ALPHAFUNC,              // 25
    D3DRS_DITHERENABLE,           // 26
    D3DRS_ALPHABLENDENABLE,       // 27
    D3DRS_FOGENABLE,              // 28
    D3DRS_SPECULARENABLE,         // 29
    D3DRS_ZVISIBLE,               // 30
    D3DRS_FOGCOLOR,               // 34
    D3DRS_FOGTABLEMODE,           // 35
    D3DRS_FOGSTART,               // 36
    D3DRS_FOGEND,                 // 37
    D3DRS_FOGDENSITY,             // 38
    D3DRS_EDGEANTIALIAS,          // 40
    D3DRS_ZBIAS,                  // 47
    D3DRS_RANGEFOGENABLE,         // 48
    D3DRS_STENCILENABLE,          // 52
    D3DRS_STENCILFAIL,            // 53
    D3DRS_STENCILZFAIL,           // 54
    D3DRS_STENCILPASS,            // 55
    D3DRS_STENCILFUNC,            // 56
    D3DRS_STENCILREF,             // 57
    D3DRS_STENCILMASK,            // 58
    D3DRS_STENCILWRITEMASK,       // 59
    D3DRS_TEXTUREFACTOR,          // 60
    D3DRS_WRAP0,                  // 128
    D3DRS_WRAP1,                  // 129
    D3DRS_WRAP2,                  // 130
    D3DRS_WRAP3,                  // 131
    D3DRS_WRAP4,                  // 132
    D3DRS_WRAP5,                  // 133
    D3DRS_WRAP6,                  // 134
    D3DRS_WRAP7,                  // 135
    D3DRS_CLIPPING,               // 136
    D3DRS_LIGHTING,               // 137
    D3DRS_AMBIENT,                // 139
    D3DRS_FOGVERTEXMODE,          // 140
    D3DRS_COLORVERTEX,            // 141
    D3DRS_LOCALVIEWER,            // 142
    D3DRS_NORMALIZENORMALS,       // 143
    D3DRS_DIFFUSEMATERIALSOURCE,  // 145
    D3DRS_SPECULARMATERIALSOURCE, // 146
    D3DRS_AMBIENTMATERIALSOURCE,  // 147
    D3DRS_EMISSIVEMATERIALSOURCE, // 148
    D3DRS_VERTEXBLEND,            // 151
    D3DRS_CLIPPLANEENABLE         // 152
};

const D3DTEXTURESTAGESTATETYPE bSavedTSS_IDs[23] = {
    D3DTSS_COLOROP,               // 1
    D3DTSS_COLORARG1,             // 2
    D3DTSS_COLORARG2,             // 3
    D3DTSS_ALPHAOP,               // 4
    D3DTSS_ALPHAARG1,             // 5
    D3DTSS_ALPHAARG2,             // 6
    D3DTSS_BUMPENVMAT00,          // 7
    D3DTSS_BUMPENVMAT01,          // 8
    D3DTSS_BUMPENVMAT10,          // 9
    D3DTSS_BUMPENVMAT11,          // 10
    D3DTSS_TEXCOORDINDEX,         // 11
    D3DTSS_ADDRESSU,              // 13
    D3DTSS_ADDRESSV,              // 14
    D3DTSS_BORDERCOLOR,           // 15
    D3DTSS_MAGFILTER,             // 16
    D3DTSS_MINFILTER,             // 17
    D3DTSS_MIPFILTER,             // 18
    D3DTSS_MIPMAPLODBIAS,         // 19
    D3DTSS_MAXMIPLEVEL,           // 20
    D3DTSS_MAXANISOTROPY,         // 21
    D3DTSS_BUMPENVLSCALE,         // 22
    D3DTSS_BUMPENVLOFFSET,        // 23
    D3DTSS_TEXTURETRANSFORMFLAGS  // 24
};

DWORD                     bSavedRS_Values[56];
DWORD                     bSavedTSS_Values[23];
int                       bHaveSavedStates; // becomes 1 after bdCloseDisplay() preserves states

// ********************************************************************************
// Helper Functions

static void RestoreStates()
{
    if (!bHaveSavedStates) return;

    for (int i = 0; i < 0x38; ++i)
        bDisplayInfo.d3dDevice->SetRenderState(bSavedRS_IDs[i], bSavedRS_Values[i]);

    for (int y = 0; y < 0x17; ++y)
        bDisplayInfo.d3dDevice->SetTextureStageState(0, bSavedTSS_IDs[y], bSavedTSS_Values[y]);
}

static int FormatToBPP(D3DFORMAT f)
{
    switch (f) {
        case D3DFMT_R8G8B8:  return 24;
        case D3DFMT_A8R8G8B8: // fall
        case D3DFMT_X8R8G8B8: return 32;
        case D3DFMT_R5G6B5: // fall
        case D3DFMT_X1R5G5B5: // fall
        case D3DFMT_A1R5G5B5: return 16;
        default: return -1; // unknown lol
    }
}

static EBDPixelFormat FormatToPixFormat(D3DFORMAT f)
{
    switch (f) {
        case D3DFMT_R8G8B8: return BDPIXFORMAT_24;
        case D3DFMT_A8R8G8B8: // fall
        case D3DFMT_X8R8G8B8: return BDPIXFORMAT_32;
        case D3DFMT_R5G6B5: return BDPIXFORMAT_565;
        case D3DFMT_X1R5G5B5: // fall
        case D3DFMT_A1R5G5B5: return BDPIXFORMAT_555;
        default: return (EBDPixelFormat)4; // unknown (out of range)
    }
}

static void PreserveStates()
{
    // 56 RenderStates
    for (int i = 0; i < 0x38; ++i) {
        DWORD val = 0;
        bDisplayInfo.d3dDevice->GetRenderState(bSavedRS_IDs[i], &val);
        bSavedRS_Values[i] = val;
    }
    // 23 TextureStageStates (stage 0)
    for (int y = 0; y < 0x17; ++y) {
        DWORD val = 0;
        bDisplayInfo.d3dDevice->GetTextureStageState(0, bSavedTSS_IDs[y], &val);
        bSavedTSS_Values[y] = val;
    }

    bHaveSavedStates = 1;
}

static int BeginScene(void) {
    if (!bDisplayInfo.started) return 0;
    HRESULT hr = bDisplayInfo.d3dDevice->BeginScene(); // vtbl +0x88
    if (FAILED(hr)) {
        bkPrintf("bdBeginScene: BeginScene() FAILED");
        return 0;
    }
    bDisplayInfo.inScene = 1;
    return 1;
}

static void EndScene(void) {
    if (bDisplayInfo.started) {
        bRenderConsoleWindow();
        bRenderGraphs();
        HRESULT hr = bDisplayInfo.d3dDevice->EndScene(); // vtbl +0x8C
        if (FAILED(hr)) {
            bkPrintf("bdEndScene: EndScene() FAILED");
        }
        bDisplayInfo.inScene = 0;
    }
}

// ********************************************************************************
// Function Implementations

/* --------------------------------------------------------------------------------
   Function : bInitDisplay
   Purpose : initialise the display module
   Parameters : 
   Returns : OK/FAIL
   Info : 
*/
int bInitDisplay()
{
    memset(&bDisplayInfo, 0, sizeof(TBDisplayInfo));  // SHOULD BE 0x170
    memset(&bViewInfo,    0, sizeof(TBViewInfo));     // SHOULD BE 0x2DC

    bFlipCount = 0;
    bDisplayInfo.noofPolysProcessed = 0;

    bViewInfo.xFov    = 1.2217305f;
    bViewInfo.yFov    = 0.91629785f;
    bViewInfo.nearClip = 40.0f;
    bViewInfo.farClip  = 10000.0f;

    bVertexBufferList.prev = &bVertexBufferList;
    bVertexBufferList.next = &bVertexBufferList;

    bIndexBufferList.prev  = &bIndexBufferList;
    bIndexBufferList.next  = &bIndexBufferList;

    bRenderTargetList.prev = &bRenderTargetList;
    bRenderTargetList.next = &bRenderTargetList;

    bDisplayInit = OK;

    if (bTextureInit() == FAIL)
        return FAIL;

    bDisplayInfo.curRenderTarget = &bRenderTargetList;

    bkInitStopwatch(&bFpsStopwatch, 30);

    // rotMatrix := identity (copy 16 float)
    memcpy(&bViewInfo.rotMatrix[0][0], &bIdentityMatrix[0][0], sizeof(TBMatrix));

    bkCreateEvent("_Resume");

    return OK;
}

/* --------------------------------------------------------------------------------
   Function : bShutdownDisplay
   Purpose : shutdown the display module
   Parameters : 
   Returns : 
   Info : 
*/
void bShutdownDisplay()
{
	bdEnableFrameTimeGraph(0);
	bdEnableMemoryGraph(0,EBMEMORYGRAPHTYPE_BABEL,0x1400,0);
	bTextureShutdown();
	if (bVertexBufferList.next != &bVertexBufferList) {
		do {
		bdVertexBufferDestroy(bVertexBufferList.next);
		} while (bVertexBufferList.next != &bVertexBufferList);
	}
	while (bIndexBufferList.next != &bIndexBufferList) {
		bdIndexBufferDestroy(bIndexBufferList.next);
	}
	return;
}

/* --------------------------------------------------------------------------------
   Function : bdOpenDisplay
   Purpose : setup the display for rendering
   Parameters : xRes, yRes, bit depth, Z buffer depth, flags
   Returns : OK/FAIL
   Info : 
*/
int bdOpenDisplay(int xRes, int yRes, int bpp, int zDepth, uint32 flags)
{
    // already open? if nope -> proceed
    if ((bDisplayInfo.flags & BDISPLAYFLAG_OPEN) != 0) {
        bkPrintf("bbOpenDisplay: Display is already open\n");
        return FAIL;
    }

    // number of attempts: 1 without adapter GUID, 2 with adapter GUID
    const int attempts = (bDisplayAdapterGuid != NULL) ? 2 : 1;

    int ok = FAIL;
    for (int attempt = 0; attempt < attempts; ++attempt)
    {
        // apply requested params to the global before trying to start
        bDisplayInfo.xRes   = xRes;
        bDisplayInfo.yRes   = yRes;
        bDisplayInfo.bpp    = bpp;
        bDisplayInfo.zDepth = zDepth;
        bDisplayInfo.flags  = flags;

        // set one control bit before starting the device
        _controlfp(8, 8);

        ok = bStartDisplay();
		
		// if bStartDisplay failed (womp_womp_sound_effect.wav)
        if (!ok)
        {
            // retry loop: depth reconcile and/or size ladder until success or fatal
            for (;;)
            {
                // print a warning on each failure with current params
                bkPrintf(
                    "bdOpenDisplay: *** WARNING *** Failed to start display @ (%d x %d), %d bpp, %d Z\n",
                    bDisplayInfo.xRes, bDisplayInfo.yRes, bDisplayInfo.bpp, bDisplayInfo.zDepth
                );

                // fullscreen path
                if ((flags & BDISPLAYFLAG_WINDOW) == 0)
                {
                    // if auto depth is ->OFF<-, skip to size ladder
                    if (flags & BDISPLAYFLAG_OPENAUTODEPTHOFF) goto SIZE_LADDER;

                    // reconcile color/depth by stepping 8bpp at a time
                    if (bDisplayInfo.zDepth < bDisplayInfo.bpp) {
                        bDisplayInfo.bpp -= 8;
                    } else if (bDisplayInfo.bpp < bDisplayInfo.zDepth) {
                        bDisplayInfo.zDepth -= 8;
                    } else {
                        // equal; if already too small, go to size ladder
                        if (bDisplayInfo.bpp < 17) goto SIZE_LADDER;
                        bDisplayInfo.bpp    -= 8;
                        bDisplayInfo.zDepth -= 8;
                    }

                    // try again after depth tweak
                    ok = bStartDisplay();
                    if (ok) break;
                    continue;
                }
                else
                {
                    // windowed: if auto depth is ->ON<- and depths differ, force equal
                    if ((flags & BDISPLAYFLAG_OPENAUTODEPTHOFF) == 0 &&
                        bDisplayInfo.zDepth != bDisplayInfo.bpp)
                    {
                        bDisplayInfo.zDepth = bDisplayInfo.bpp;
                        ok = bStartDisplay();
                        if (ok) break;
                        // if still fails, proceed to size ladder
                    }
                }

SIZE_LADDER:
                // if auto size OFF -> fatal in this path
                if (flags & BDISPLAYFLAG_OPENAUTOSIZEOFF) {
                    bkPrintf("bdOpenDisplay: *** FATAL ERROR *** Could not open display in any resolution or depth combination\n");
                    // decompile clears adapter guid in this fatal branch
                    if (bDisplayAdapterGuid != NULL) bDisplayAdapterGuid = NULL;
                    break;
                }

                // resolution ladder
                if (bDisplayInfo.xRes < 1281) {
                    if (bDisplayInfo.xRes < 1153) {
                        if (bDisplayInfo.xRes < 1025) {
                            if (bDisplayInfo.xRes < 801) {
                                if (bDisplayInfo.xRes < 641) {
                                    bkPrintf("bdOpenDisplay: *** FATAL ERROR *** Could not open display in any resolution or depth combination\n");
                                    if (bDisplayAdapterGuid != NULL) bDisplayAdapterGuid = NULL;
                                    break;
                                }
                                bDisplayInfo.xRes = 640;  bDisplayInfo.yRes = 480;
                            } else {
                                bDisplayInfo.xRes = 800;  bDisplayInfo.yRes = 600;
                            }
                        } else {
                            bDisplayInfo.xRes = 1024; bDisplayInfo.yRes = 768;
                        }
                    } else {
                        bDisplayInfo.xRes = 1152; bDisplayInfo.yRes = 864;
                    }
                } else {
                    bDisplayInfo.xRes = 1280; bDisplayInfo.yRes = 1024;
                }

                // try again after size change
                ok = bStartDisplay();
                if (ok) break;
            } // end retry loop
        }

        // clear the bit after the attempt
        _controlfp(0, 8);

        if (ok) break; // success ends the outer attempts loop
    }

    if (ok)
    {
        bInitFont();

        if (bDisplayInfo.stencilDepth == 0) {
            bkPrintf("Display established at (%d x %d) in %d bpp with a %d bit Z buffer and no stencil buffer\n",
                     bDisplayInfo.xRes, bDisplayInfo.yRes,
                     bDisplayInfo.bpp, bDisplayInfo.zDepth);
        } else {
            bkPrintf("Display established at (%d x %d) in %d bpp with a %d bit Z buffer and %d bit stencil buffer\n",
                     bDisplayInfo.xRes, bDisplayInfo.yRes,
                     bDisplayInfo.bpp, bDisplayInfo.zDepth, bDisplayInfo.stencilDepth);
        }

        // default clip planes and FOV
        bdSetViewClipPlanes(10.0f, 10000.0f);
        bdSetFOV(1.2217305f, 0.0f);

        // the decompile also calls this after the success block
        _controlfp(0, 8);

        bCheckSignature();
        bInitInput(bBkInitFlags);
        return OK;
    }

    // final fatal if no attempt succeeded
    bkPrintf("bdOpenDisplay: *** FATAL ERROR  Could not open display ***\n");
    return FAIL;
}

/* --------------------------------------------------------------------------------
   Function : bdCloseDisplay
   Purpose : shutdown the open display
   Parameters : 
   Returns : 
   Info : 
*/
void bdCloseDisplay()
{
        bkPrintf("*** WARNING *** bdCloseDisplay was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : bdGetDisplayInfo
   Purpose : retrieve display parameters
   Parameters : xRes, yRes, bpp, zDepth
   Returns : 
   Info : 
*/
void bdGetDisplayInfo(int *xRes, int *yRes, int *bpp, int *zDepth)
{
    if (xRes)   *xRes   = bDisplayInfo.xRes;
    if (yRes)   *yRes   = bDisplayInfo.yRes;
    if (bpp)    *bpp    = bDisplayInfo.bpp;
    if (zDepth) *zDepth = bDisplayInfo.zDepth;
}

/* --------------------------------------------------------------------------------
   Function : bdSetVsyncCallback
   Purpose : set vsync callback
   Parameters : callback, context
   Returns : OK/FAIL
   Info : 
*/
int bdSetVsyncCallback(TBVsyncCallback callback, void *context)
{
        bkPrintf("*** WARNING *** bdSetVsyncCallback was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

/* --------------------------------------------------------------------------------
   Function : bdSetPreFlipCallback
   Purpose : set pre-flip callback
   Parameters : callback, context
   Returns : OK/FAIL
   Info : 
*/
int bdSetPreFlipCallback(TBPreFlipCallback callback, void *context)
{
        bkPrintf("*** WARNING *** bdSetPreFlipCallback was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

/* --------------------------------------------------------------------------------
   Function : bdFlip
   Purpose : flip the display
   Parameters : red, green, blue, alpha, flags
   Returns : 
   Info : 
*/
void bdFlip(int red, int green, int blue, int alpha, int flags)
{
        bkPrintf("*** WARNING *** bdFlip was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : bdBeginScene
   Purpose : begin scene
   Parameters : 
   Returns : OK/FAIL
   Info : 
*/
int bdBeginScene()
{
    if (bSceneStarted) return OK;

    if (bDisplayInfo.inScene) {
        bSceneStarted = 1;
        return OK;
    }

    if (BeginScene()) {
        bSceneStarted = 1;
        return OK;
    }
    return FAIL;
}

/* --------------------------------------------------------------------------------
   Function : bdEndScene
   Purpose : end scene
   Parameters : 
   Returns : 
   Info : 
*/
void bdEndScene()
{
    if (!bSceneStarted) return;
    bSceneStarted = 0;

    if (bDisplayInfo.inScene) {
        EndScene();
    }
}

/* --------------------------------------------------------------------------------
   Function : bdGetScanline
   Purpose : get scanline
   Parameters : 
   Returns : scanline
   Info : 
*/
int bdGetScanline()
{
        bkPrintf("*** WARNING *** bdGetScanline was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

/* --------------------------------------------------------------------------------
   Function : bdScreenshot
   Purpose : take screenshot
   Parameters : filename
   Returns : OK/FAIL
   Info : 
*/
int bdScreenshot(char *filename)
{
        bkPrintf("*** WARNING *** bdScreenshot was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

/* --------------------------------------------------------------------------------
   Function : bRestoreAllSurfaces
   Purpose : restore all surfaces
   Parameters : 
   Returns : 
  Info : 
*/
void bRestoreAllSurfaces()
{
        bkPrintf("*** WARNING *** bRestoreAllSurfaces was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : bStartDisplay
   Purpose : start display
   Parameters : 
   Returns : OK/FAIL
   Info : 
*/
int bStartDisplay()
{
    if (bDisplayInfo.started)
        return 0;

    bkPrintf("bStartDisplay: Trying (%dx%d) %d bpp %d Z\n",
             bDisplayInfo.xRes, bDisplayInfo.yRes, bDisplayInfo.bpp, bDisplayInfo.zDepth);

    bDisplayInfo.d3d = Direct3DCreate8(D3D_SDK_VERSION);
    if (!bDisplayInfo.d3d) {
        bkPrintf("bStartDisplay: *** ERROR : Direct3DCreate8 Failed ***\n");
        return 0;
    }

    // Windowed vs Fullscreen
#ifdef FORCE_WINDOWED
    BOOL isWindowed = TRUE;
	bDisplayInfo.flags |= BDISPLAYFLAG_WINDOW;
	bkPrintf("bStartDisplay: FORCE_WINDOWED is defined -> running in windowed mode\n");
#else
	BOOL isWindowed = (bDisplayInfo.flags & BDISPLAYFLAG_WINDOW) ? TRUE : FALSE;
#endif
    UINT adapterCount = bDisplayInfo.d3d->GetAdapterCount();
    bkPrintf("bStartDisplay: Found %d display adapters\n", adapterCount);

    UINT firstAdapter = isWindowed ? 0u : 0u;
    UINT lastAdapter  = isWindowed ? 0u : (adapterCount ? (adapterCount - 1) : 0);

    // Try adapters
    UINT adapter = 0;
    for (adapter = firstAdapter; adapter <= lastAdapter; ++adapter)
    {
        D3DFORMAT rtFmt = D3DFMT_UNKNOWN;
        D3DDISPLAYMODE dm; ZeroMemory(&dm, sizeof(dm));

        if (isWindowed) {
            if (FAILED(bDisplayInfo.d3d->GetAdapterDisplayMode(adapter, &dm))) {
                bkPrintf("bStartDisplay: *** ERROR : GetAdapterDisplayMode failed ***\n");
                return 0;
            }
            rtFmt = dm.Format;
            bDisplayInfo.bpp = FormatToBPP(rtFmt);
            bkPrintf("bStartDisplay: Windowed screen mode is (%dx%d), %dHz, %s\n",
                     bDisplayInfo.xRes, bDisplayInfo.yRes, dm.RefreshRate, bDXFormatToString(rtFmt));
        } else {
            // Enumerate modes and find exact match
            UINT modeCount = bDisplayInfo.d3d->GetAdapterModeCount(adapter);
            UINT i; BOOL found = FALSE;
            for (i = 0; i < modeCount; ++i) {
                D3DDISPLAYMODE mode; ZeroMemory(&mode, sizeof(mode));
                bDisplayInfo.d3d->EnumAdapterModes(adapter, i, &mode);
                if (mode.Width  == (UINT)bDisplayInfo.xRes &&
                    mode.Height == (UINT)bDisplayInfo.yRes &&
                    FormatToBPP(mode.Format) == bDisplayInfo.bpp)
                {
                    rtFmt = mode.Format;
                    found = TRUE;
                    break;
                }
            }
            if (!found) {
                bkPrintf("bStartDisplay: Could not find fullscreen mode to match %dx%d %dbpp\n",
                         bDisplayInfo.xRes, bDisplayInfo.yRes, bDisplayInfo.bpp);
                continue;
            }
            bkPrintf("bStartDisplay: Found compatible fullscreen mode with format %s\n",
                     bDXFormatToString(rtFmt));
        }

        // Adapter identifier + optional GUID filter
        D3DADAPTER_IDENTIFIER8 id; ZeroMemory(&id, sizeof(id));
        if (SUCCEEDED(bDisplayInfo.d3d->GetAdapterIdentifier(adapter, 0, &id))) {
            if (bDisplayAdapterGuid) {
                if (memcmp(&id.DeviceIdentifier, &bDisplayAdapterGUIDBuffer, sizeof(GUID)) != 0) {
                    bkPrintf("bStartDisplay: Rejecting display device due to clients GUID requirements\n");
                    continue;
                }
            }
        }

        // ---- Depth/Stencil selection ----
        D3DFORMAT zFmt = D3DFMT_D16;
        int chosenStencilBits = 0;
        if (bDisplayInfo.zDepth > 16) {
            if (SUCCEEDED(bDisplayInfo.d3d->CheckDeviceFormat(adapter, D3DDEVTYPE_HAL, rtFmt,
                                                              D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24S8)) &&
                SUCCEEDED(bDisplayInfo.d3d->CheckDepthStencilMatch(adapter, D3DDEVTYPE_HAL, rtFmt, rtFmt, D3DFMT_D24S8)))
            { zFmt = D3DFMT_D24S8; chosenStencilBits = 8; }
            else if (SUCCEEDED(bDisplayInfo.d3d->CheckDeviceFormat(adapter, D3DDEVTYPE_HAL, rtFmt,
                                                                   D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D32)) &&
                     SUCCEEDED(bDisplayInfo.d3d->CheckDepthStencilMatch(adapter, D3DDEVTYPE_HAL, rtFmt, rtFmt, D3DFMT_D32)))
            { zFmt = D3DFMT_D32; chosenStencilBits = 0; }
            else if (SUCCEEDED(bDisplayInfo.d3d->CheckDeviceFormat(adapter, D3DDEVTYPE_HAL, rtFmt,
                                                                   D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X8)) &&
                     SUCCEEDED(bDisplayInfo.d3d->CheckDepthStencilMatch(adapter, D3DDEVTYPE_HAL, rtFmt, rtFmt, D3DFMT_D24X8)))
            { zFmt = D3DFMT_D24X8; chosenStencilBits = 0; }
        }
        bkPrintf("bStartDisplay: Trying depth format %s\n", bDXFormatToString(zFmt));
        if (FAILED(bDisplayInfo.d3d->CheckDepthStencilMatch(adapter, D3DDEVTYPE_HAL, rtFmt, rtFmt, zFmt))) {
            bkPrintf("bStartDisplay: Depth format is not compatible with render target (%s)\n", bDXFormatToString(zFmt));
            continue;
        }

		if (bDisplayInfo.devCaps.PrimitiveMiscCaps & D3DPMISCCAPS_BLENDOP)
			bkPrintf("bStartDisplay: BlendOp alpha blending IS supported\n");
		else
			bkPrintf("bStartDisplay: BlendOp alpha blending IS NOT supported\n");

		if (bDisplayInfo.devCaps.PrimitiveMiscCaps & D3DPMISCCAPS_COLORWRITEENABLE) 
			bkPrintf("bStartDisplay: FrameWrite settings ARE supported\n");
		else 
			bkPrintf("bStartDisplay: FrameWrite settings ARE NOT supported\n");

        // Device caps via D3D8
        if (FAILED(bDisplayInfo.d3d->GetDeviceCaps(adapter, D3DDEVTYPE_HAL, &bDisplayInfo.devCaps))) {
            bkPrintf("bStartDisplay: GetDeviceCaps failed\n");
            continue;
        }
        bkPrintf("bStartDisplay: Device supports Vertex Shaders v%d.%d with %d constant registers\n",
                 (bDisplayInfo.devCaps.VertexShaderVersion >> 8) & 0xFF,
                 (bDisplayInfo.devCaps.VertexShaderVersion     ) & 0xFF,
                 bDisplayInfo.devCaps.MaxVertexShaderConst);
		if (bDisplayInfo.devCaps.RasterCaps & D3DPRASTERCAPS_ANTIALIASEDGES)
			bkPrintf("bStartDisplay: Edge anti-aliasing IS supported\n");
		else
			bkPrintf("bStartDisplay: Edge anti-aliasing IS NOT supported\n");
        bkPrintf("bStartDisplay: There are %d texture blend stages with %d simultaneous textures\n",
                 bDisplayInfo.devCaps.MaxTextureBlendStages, bDisplayInfo.devCaps.MaxSimultaneousTextures);
        bkPrintf("bStartDisplay: Max point size is %.2f\n", (double)bDisplayInfo.devCaps.MaxPointSize);

        // ---- MSAA probe and choice ----
        D3DMULTISAMPLE_TYPE msaa = D3DMULTISAMPLE_NONE;
        if (bDisplayInfo.flags & BDISPLAYFLAG_ANTIALIASED4) msaa = D3DMULTISAMPLE_4_SAMPLES;
        else if (bDisplayInfo.flags & BDISPLAYFLAG_ANTIALIASED) msaa = D3DMULTISAMPLE_2_SAMPLES;

        BOOL sawAnyMsaa = FALSE;
        int s;
        for (s = 2; s <= 16; ++s) {
            if (SUCCEEDED(bDisplayInfo.d3d->CheckDeviceMultiSampleType(adapter, D3DDEVTYPE_HAL, rtFmt, isWindowed, (D3DMULTISAMPLE_TYPE)s))) {
                bkPrintf("bStartDisplay: %d sample multisampling IS supported\n", s);
                sawAnyMsaa = TRUE;
                if (msaa != D3DMULTISAMPLE_NONE && s == (int)msaa) break;
            }
        }
        if (!sawAnyMsaa) {
            bkPrintf("bStartDisplay: No multisampling capability supported\n");
            bDisplayInfo.flags &= ~BDISPLAYFLAG_ANTIALIASED;
            bDisplayInfo.flags &= ~BDISPLAYFLAG_ANTIALIASED4;
            msaa = D3DMULTISAMPLE_NONE;
        }

        // Adapter info line
        bkPrintf("bStartDisplay: Using adapter '%s' '%s', product %u, version %u.%u build %u\n",
			id.Driver, id.Description,
			HIWORD(id.DriverVersion.u.HighPart),
			LOWORD(id.DriverVersion.u.HighPart), HIWORD(id.DriverVersion.u.LowPart),
			LOWORD(id.DriverVersion.u.LowPart));

        // ---- Present params ----
        D3DPRESENT_PARAMETERS* ppp = &bDisplayInfo.presentParams;
        ZeroMemory(ppp, sizeof(*ppp));
        ppp->BackBufferWidth  = bDisplayInfo.xRes;
        ppp->BackBufferHeight = bDisplayInfo.yRes;
        ppp->BackBufferFormat = rtFmt;
        ppp->BackBufferCount  = 1;
        ppp->MultiSampleType  = msaa;
        ppp->EnableAutoDepthStencil = TRUE;
        ppp->AutoDepthStencilFormat = zFmt;
        ppp->Windowed = isWindowed;
        if ((bDisplayInfo.flags & BDISPLAYFLAG_ANTIALIASED) || (bDisplayInfo.flags & BDISPLAYFLAG_ANTIALIASED4))
            ppp->SwapEffect = D3DSWAPEFFECT_DISCARD;
        else
            ppp->SwapEffect = isWindowed ? D3DSWAPEFFECT_COPY : D3DSWAPEFFECT_FLIP;
        if (!isWindowed) {
            BOOL noVSync = (bDisplayInfo.flags & BDISPLAYFLAG_NOVSYNC) ? TRUE : FALSE;
            ppp->FullScreen_PresentationInterval = noVSync ? D3DPRESENT_INTERVAL_IMMEDIATE : D3DPRESENT_INTERVAL_ONE;
        }

        // VP flags and CreateDevice (retry w/ SWVP if needed)
        BOOL hasHwTnL = (bDisplayInfo.devCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) ? TRUE : FALSE;
        DWORD vpFlags = hasHwTnL ? D3DCREATE_HARDWARE_VERTEXPROCESSING : D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        bDisplayInfo.tnlHardware = hasHwTnL ? 1 : 0;
        bDisplayInfo.tnlActive   = bDisplayInfo.tnlHardware;
        bDisplayInfo.hwVertexShaders = 0;

        IDirect3DDevice8* dev = 0;
        HRESULT hr = bDisplayInfo.d3d->CreateDevice(adapter, D3DDEVTYPE_HAL, bMainWindow, vpFlags, ppp, &dev);
        if (FAILED(hr)) {
            bkPrintf("bStartDisplay: CreateDevice failed\n");
            if (hasHwTnL) {
                bkPrintf("bStartDisplay: Disabling TnL and trying again...\n");
                vpFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
                bDisplayInfo.tnlHardware = 0;
                bDisplayInfo.tnlActive   = 0;
                hr = bDisplayInfo.d3d->CreateDevice(adapter, D3DDEVTYPE_HAL, bMainWindow, vpFlags, ppp, &dev);
                if (FAILED(hr)) {
                    bkPrintf("bStartDisplay: CreateDevice failed again so we are terminal\n");
                    continue;
                }
                bkPrintf("bStartDisplay: CreateDevice succeeded this time so proceeding with software vertex processing...\n");
            } else {
                continue;
            }
        }
        bDisplayInfo.d3dDevice = dev;

        bkPrintf("bStartDisplay: TnL hardware %s available\n", bDisplayInfo.tnlHardware ? "IS" : "IS NOT");

        if (FAILED(dev->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &bDisplayInfo.backBuffer))) {
            bkPrintf("bStartDisplay: GetBackBuffer failed\n");
            while (bDisplayInfo.d3dDevice && bDisplayInfo.d3dDevice->Release());
            bDisplayInfo.d3dDevice = 0;
            continue;
        }
        if (FAILED(dev->GetDepthStencilSurface(&bDisplayInfo.depthStencilBuffer))) {
            bkPrintf("bStartDisplay: GetDepthStencilBuffer failed\n");
            while (bDisplayInfo.d3dDevice && bDisplayInfo.d3dDevice->Release());
            bDisplayInfo.d3dDevice = 0;
            continue;
        }

        ZeroMemory(&bDisplayInfo.devCaps, sizeof(bDisplayInfo.devCaps));
        dev->GetDeviceCaps(&bDisplayInfo.devCaps);

        // MaxActiveLights
        if ((int)bDisplayInfo.devCaps.MaxActiveLights < 1) {
            bMaxActiveLights = 32;
            bkPrintf("bStartDisplay: Device supports up to %d active SW light sources\n", bMaxActiveLights);
        } else {
            bMaxActiveLights = (int)bDisplayInfo.devCaps.MaxActiveLights;
            bkPrintf("bStartDisplay: Device supports up to %d active HW light sources\n", bMaxActiveLights);
        }

        // ---- "FrameWrite" selection and LUT filling ----
        if ((bDisplayInfo.flags & BDISPLAYFLAG_NOTEXTURESTATES) == 0 &&
            (bDisplayInfo.devCaps.TextureOpCaps & D3DTEXOPCAPS_MODULATE2X))
        {
            // Modulate2X IS supported
            bDisplayInfo.modulate2X = 1;
            bkPrintf("bStartDisplay: Modulate2X IS supported on this card\n");
            int acc = 0;
            unsigned i;
            for (i = 0; i < 256u; ++i, acc += 2) {
                bRColLUT[i] = (i << 16);
                bGColLUT[i] = (i << 8);
                bBColLUT[i] = (i);
                bAColLUT[i] = (i << 24);
                bRGBNormLUT[i] = (acc < 0xFF) ? (float)acc / 255.0f : 1.0f;
                bRGBInvColLUT[i] = i;
                bAInvColLUT[i]   = i;
            }
            dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_ADD);
        } else {
            // Modulate2X IS NOT supported
            bDisplayInfo.modulate2X = 0;
            bkPrintf("bStartDisplay: Modulate2X IS NOT supported on this card\n");
            int acc = 0;
            unsigned i;
            for (i = 0; i < 256u; ++i, acc += 2) {
                bRColLUT[i] = (acc < 0xFF) ? (i << 16) : 0x00FF0000;
                bGColLUT[i] = (acc < 0xFF) ? (i << 8)  : 0x0000FF00;
                bBColLUT[i] = (acc <= 0xFE) ? (i * 2)  : 0x000000FF;
                bAColLUT[i] = (acc < 0xFF) ? (i << 24) : 0xFF000000;
                bRGBNormLUT[i] = (acc < 0xFF) ? (float)acc / 255.0f : 1.0f;
                bRGBInvColLUT[i] = (i >> 1);
                bAInvColLUT[i]   = (i >> 1);
            }
        }

        // Texture memory + NPOT logs
        {
            UINT texMem = dev->GetAvailableTextureMem();
            bkPrintf("bStartDisplay: Found %d Mb of texture memory\n", texMem >> 20);
            bDisplayInfo.textureCapsMirror = bDisplayInfo.devCaps.TextureCaps;
            bkPrintf("bStartDisplay: Global Non-POW2 textures %s supported\n",
                     (bDisplayInfo.devCaps.TextureCaps & D3DPTEXTURECAPS_POW2) ? "ARE NOT" : "ARE");
            bkPrintf("bStartDisplay: Conditional Non-POW2 textures %s supported\n",
                     (bDisplayInfo.devCaps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL) ? "ARE" : "ARE NOT");
        }

        // Bookkeeping
        bDisplayInfo.adapterOrdinal = adapter;
        bDisplayInfo.stencilDepth   = chosenStencilBits;
        bDisplayInfo.xResHalf       = bDisplayInfo.xRes / 2;
        bDisplayInfo.yResHalf       = bDisplayInfo.yRes / 2;
        bDisplayInfo.xAspect        = 0.5f;
        bDisplayInfo.yAspect        = (float)bDisplayInfo.yRes / (float)bDisplayInfo.xRes * 0.5f;
        bDisplayInfo.pixelFormat    = FormatToPixFormat(rtFmt);
        bDisplayInfo.displayFormat  = rtFmt;

		// so for some reason there's a problem with zDepth
		// decompiler bug???
		switch (zFmt) {
			case D3DFMT_D16:   bDisplayInfo.zDepth = 16; break;
			case D3DFMT_D24S8: bDisplayInfo.zDepth = 24; break;
			case D3DFMT_D24X8: bDisplayInfo.zDepth = 24; break;
			case D3DFMT_D32:   bDisplayInfo.zDepth = 32; break;
			default: break;
		}

        // Basic render init
        bCalcOrthoProjectionMatrix();
        bdSetViewport(0, 0, bDisplayInfo.xRes, bDisplayInfo.yRes);

        // Material = 1.0
        {
            D3DMATERIAL8 mat; ZeroMemory(&mat, sizeof(mat));
            mat.Diffuse.r = mat.Diffuse.g = mat.Diffuse.b = mat.Diffuse.a = 1.0f;
            dev->SetMaterial(&mat);
        }

        bDisplayInfo.flags |= BDISPLAYFLAG_OPEN;
        bSetDefaultRenderStates();
        bCalcPerspProjectionMatrix();
        bdSetProjectionMode(1, 0);
        bdSetViewClipPlanes(10.0f, 10000.0f);
        bdSetFogRange(8000.0f, 10000.0f);
        bdSetFogColour(0, 0, 0);

        // Prime the swapchain
        dev->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
        dev->Present(0, 0, 0, 0);

        // Resume engine-managed resources
        bResumeVertexBuffers();
        bResumeIndexBuffers();
        bResumeRenderTargets();
        bResumeTextures();

        // Restore RS/TSS if previously preserved
        RestoreStates();

        // Lights and shaders
        bRestoreLights();
        bInitShaderSelector();

        bDisplayInfo.started = 1;
        bDisplayInfo.inScene = 0;
        bDisplayAdapterGuid  = &bDisplayAdapterGUIDBuffer; // mirrors decompile tail

        return 1; // success
    }

    // No compatible adapter found
    bkPrintf("bStartDisplay: Could not find a compatible adapter\n");
    if (bDisplayInfo.d3d) {
        while (bDisplayInfo.d3d->Release());
        bDisplayInfo.d3d = 0;
    }
    return 0;
}

/* --------------------------------------------------------------------------------
   Function : bStopDisplay
   Purpose : stop display
   Parameters : shutdown
   Returns : 
   Info : 
*/
void bStopDisplay(int shutdown)
{
        bkPrintf("*** WARNING *** bStopDisplay was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : bGrabScreen
   Purpose : grab screen
   Parameters : 
   Returns : screen data
   Info : 
*/
uchar *bGrabScreen()
{
        bkPrintf("*** WARNING *** bGrabScreen was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}

/* --------------------------------------------------------------------------------
   Function : bLockSurface
   Purpose : lock surface
   Parameters : surf, xDim, yDim, bpp, rgb555, pitch
   Returns : surface data
   Info : 
*/
void *bLockSurface(LPDIRECT3DSURFACE8 surf, int *xDim, int *yDim, int *bpp, int *rgb555, int *pitch)
{
        bkPrintf("*** WARNING *** bLockSurface was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}

/* --------------------------------------------------------------------------------
   Function : bUnlockSurface
   Purpose : unlock surface
   Parameters : surf
   Returns : 
   Info : 
*/
void bUnlockSurface(LPDIRECT3DTEXTURE8 surf)
{
        bkPrintf("*** WARNING *** bUnlockSurface was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : bdSetScreenOffset
   Purpose : set screen offset
   Parameters : x, y
   Returns : 
   Info : 
*/
void bdSetScreenOffset(int x, int y)
{
        bkPrintf("*** WARNING *** bdSetScreenOffset was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : bdClear
   Purpose : clear display
   Parameters : red, green, blue, alpha, z, stencil, flags
   Returns : 
   Info : 
*/
void bdClear(int red, int green, int blue, int alpha, float z, int32 stencil, uint32 flags)
{
        bkPrintf("*** WARNING *** bdClear was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : bGetDisplayFrequency
   Purpose : get display frequency
   Parameters : 
   Returns : frequency
   Info : 
*/
int bGetDisplayFrequency()
{
        bkPrintf("*** WARNING *** bGetDisplayFrequency was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

/* --------------------------------------------------------------------------------
   Function : bdDoDummyPresent
   Purpose : do dummy present
   Parameters : 
   Returns : 
   Info : 
*/
void bdDoDummyPresent()
{
        bkPrintf("*** WARNING *** bdDoDummyPresent was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}
