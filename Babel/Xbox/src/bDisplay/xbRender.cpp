#define __BABEL__
#define _BDISPLAY_

#include <babel.h>
#include <bKernel/heapNew.h>

#define TARGET_ALLOC()		(TBRenderTarget*)bkHeapAllocEx(sizeof(TBRenderTarget), __FILE__, __LINE__, BALLOC_MALLOC | bDisplayModule, bGetCurrentGroup(), 0)
#define BUFFER_ALLOC(SZ,ALIGNMENT) bkHeapAllocEx(SZ, __FILE__, __LINE__, BALLOC_MALLOC | bDisplayModule, bGetCurrentGroup(), ALIGNMENT)
// ********************************************************************************
// Globals

TBRenderStateInfo	bRenderState;			// record of current render states
TBRenderTarget		bRenderTargetList;		// list of render targets

const DWORD filterLUT[][3] = {
    { 0, 0, 0 }, // 0
    { 1, 1, 0 }, // 1
    { 2, 2, 0 }, // 2
    { 1, 1, 1 }, // 3
    { 2, 2, 1 }, // 4
    { 2, 2, 2 }, // 5
    // Todo: map these to named constants - D3DTEXF probably
};

// Maps BDCULLMODE to D3DCULL
const D3DCULL cullModeLUT[3] = {
    D3DCULL_NONE,
        D3DCULL_CCW,
        D3DCULL_CW
};

// Maps BDZTESTMODE to D3DCMPFUNC
const D3DCMPFUNC zfuncLUT[8] = {
    D3DCMP_NEVER,
        D3DCMP_ALWAYS,
        D3DCMP_LESSEQUAL,
        D3DCMP_LESS,
        D3DCMP_GREATEREQUAL,
        D3DCMP_GREATER,
        D3DCMP_EQUAL,
        D3DCMP_NOTEQUAL
};

const DWORD blendModes[BDALPHABLENDMODES * 3] = {
    D3DBLEND_SRCALPHA,     D3DBLEND_INVSRCALPHA, D3DBLENDOP_ADD,         // BLEND
        D3DBLEND_SRCALPHA,     D3DBLEND_ONE,         D3DBLENDOP_ADD,         // ADDITIVE
        D3DBLEND_DESTCOLOR,    D3DBLEND_ZERO,        D3DBLENDOP_ADD,         // MULTIPLY
        D3DBLEND_SRCALPHA,     D3DBLEND_ONE,         D3DBLENDOP_REVSUBTRACT, // SUBTRACTIVE
        D3DBLEND_ONE,          D3DBLEND_ZERO,        D3DBLENDOP_ADD,         // NONE
        D3DBLEND_INVDESTALPHA, D3DBLEND_DESTALPHA,   D3DBLENDOP_ADD,         // INVBLEND
        D3DBLEND_INVDESTALPHA, D3DBLEND_ONE,         D3DBLENDOP_ADD          // INVADDITIVE
};

ulong FtoDW(float floater)
{
    return (ulong)floater;
}

/* --------------------------------------------------------------------------------
Function : bdSetRenderState
Purpose : Set a render state
Parameters : render state to set, first value, second value
Returns : 
Info : 
*/

void bdSetRenderState(uint32 renderState, uint32 value1, uint32 value2){
    // Validate bounds
    if (renderState >= BDRENDERSTATE_NOOF)
        return;
    
    // Render state lock uses index (renderState) //does it now?
    if (bRenderState.renderStateLock[renderState /*  + 1 */ ] != 0)
        return;
    
    // Store the new render state values
    bRenderState.renderState[renderState][0] = value1;
    bRenderState.renderState[renderState][1] = value2;
    
    switch(renderState){
    case BDRENDERSTATE_TEXTUREFILTER:
        D3DDevice_SetTextureStageState(0,D3DTSS_MAGFILTER,filterLUT[value1][0]);
        D3DDevice_SetTextureStageState(0,D3DTSS_MINFILTER,filterLUT[value1][1]);
        D3DDevice_SetTextureStageState(0,D3DTSS_MIPFILTER,filterLUT[value1][2]);
        D3DDevice_SetTextureStageState(1,D3DTSS_MAGFILTER,filterLUT[value1][0]);
        D3DDevice_SetTextureStageState(1,D3DTSS_MINFILTER,filterLUT[value1][1]);
        D3DDevice_SetTextureStageState(1,D3DTSS_MIPFILTER,filterLUT[value1][2]);
        D3DDevice_SetTextureStageState(2,D3DTSS_MAGFILTER,filterLUT[value1][0]);
        D3DDevice_SetTextureStageState(2,D3DTSS_MINFILTER,filterLUT[value1][1]);
        D3DDevice_SetTextureStageState(2,D3DTSS_MIPFILTER,filterLUT[value1][2]);
        D3DDevice_SetTextureStageState(3,D3DTSS_MAGFILTER,filterLUT[value1][0]);
        D3DDevice_SetTextureStageState(3,D3DTSS_MINFILTER,filterLUT[value1][1]);
        D3DDevice_SetTextureStageState(3,D3DTSS_MIPFILTER,filterLUT[value1][2]);
        break;
    case BDRENDERSTATE_TEXTUREWRAP:
        // Determine U address mode
        {
            UINT addressU;
            UINT addressV;
            
            if((value1 & BDTEXTUREWRAP_WRAPU) != 0){
                addressU = D3DTADDRESS_MIRROR;
            }else {
                addressU = D3DTADDRESS_CLAMP;
            }
            if((value1 & BDTEXTUREWRAP_WRAPV) != 0){
                addressV = D3DTADDRESS_MIRROR;
            }else {
                addressV = D3DTADDRESS_CLAMP;
            }
            
            D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSU, addressU);
            D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSV, addressV);
        }
        break;
    case BDRENDERSTATE_LIGHTING:
        D3DDevice_SetRenderState(D3DRS_LIGHTING,value1);
        //D3DDevice_SetRenderState(D3DRS_FOGTABLEMODE,value1);
        break;
    case BDRENDERSTATE_BACKFACECULL:
        D3DDevice_SetRenderState(D3DRS_CULLMODE, (DWORD) cullModeLUT[value1]);
        // D3DDevice_SetRenderState(D3DRS_DEFERRED_UNUSED7, (DWORD) cullModeLUT[value1]);
        break;
    case BDRENDERSTATE_ZWRITE:           
        D3DDevice_SetRenderState(D3DRS_ZWRITEENABLE,value1);
        break;
    case BDRENDERSTATE_ALPHAENABLE:
        D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE,value1);
        break;
    case BDRENDERSTATE_ZTEST:           
        D3DDevice_SetRenderState(D3DRS_ZFUNC, zfuncLUT[value1]);
        break;
    case BDRENDERSTATE_DISTANCEFOG:
        D3DDevice_SetRenderState(D3DRS_FOGENABLE,0); // Dirty hack for now - fog seems broken?
        //D3DDevice_SetRenderState(D3DRS_FOGENABLE,value1);
        //D3DDevice_SetRenderState(D3DRS_STIPPLEENABLE,value1);
        break;
    case BDRENDERSTATE_ALPHABLENDMODE:
        D3DDevice_SetRenderState(D3DRS_BLENDOP,blendModes[value1 * 3 + 2]);
        D3DDevice_SetRenderState(D3DRS_SRCBLEND,blendModes[value1 * 3]);
        D3DDevice_SetRenderState(D3DRS_DESTBLEND,blendModes[value1 * 3 + 1]);
        break;
    case BDRENDERSTATE_FRAMEWRITE:
        {
            DWORD colorWriteMask = 0;
            
            if ((value1 & BDFRAMEWRITE_BLUE) || (value1 & BDFRAMEWRITE_ALL)) {
                colorWriteMask |= D3DCOLORWRITEENABLE_BLUE;
            }
            if ((value1 & BDFRAMEWRITE_GREEN) || (value1 & BDFRAMEWRITE_ALL)) {
                colorWriteMask |= D3DCOLORWRITEENABLE_GREEN;
            }
            if ((value1 & BDFRAMEWRITE_RED) || (value1 & BDFRAMEWRITE_ALL)) {
                colorWriteMask |= D3DCOLORWRITEENABLE_RED;
            }
            if ((value1 & BDFRAMEWRITE_ALPHA) || (value1 & BDFRAMEWRITE_ALL)) {
                colorWriteMask |= D3DCOLORWRITEENABLE_ALPHA;
            }
            
            D3DDevice_SetRenderState(D3DRS_COLORWRITEENABLE, colorWriteMask);
        }
        break;
    case BDRENDERSTATE_2DWORLDMATRIX:
        if ((value1 == 0) || (bObjectMatrixIsIdentity != 0)) {
            D3DDevice_SetTransform(D3DTS_WORLD,(D3DMATRIX*)bIdentityMatrix);
        }
        else {
            D3DDevice_SetTransform(D3DTS_WORLD,(D3DMATRIX*)bViewInfo.objectMatrix);
        }
        break;
        
    case BDRENDERSTATE_TEXTUREENABLE:
        bRenderState.textureEnable[value1] = value2;
        break;
    }
}


/* --------------------------------------------------------------------------------
Function : bSetDefaultRenderStates
Purpose : Set the default render states
Parameters : 
Returns : 
Info : 
*/
void bSetDefaultRenderStates(){
    static float one = 1.0f;
    ulong tmp;

    
    memset(&bRenderState,0,0xae8); // sizeof(TBRenderStateInfo) + 8 ?
    
    bRenderState.fogNear = 10.0;
    bRenderState.fogFar = 10000.0;
    bRenderState.fogRange = 1.0f / 9990.0f;
    
    bSetInitialRenderStates(4);
    
    D3DDevice_SetRenderState(D3DRS_FOGDENSITY,*((DWORD*) (&one)));
    D3DDevice_SetRenderState(D3DRS_DITHERENABLE,1);
    D3DDevice_SetRenderState(D3DRS_SPECULARENABLE,1);
    D3DDevice_SetRenderState(D3DRS_WRAP3,0);
    D3DDevice_SetRenderState(D3DRS_ALPHATESTENABLE,1);
    D3DDevice_SetRenderState(D3DRS_ALPHAFUNC,0x205);
    D3DDevice_SetRenderState(D3DRS_ALPHAREF,0);     
    
    D3DDevice_SetTextureStageState(0,D3DTSS_ALPHAOP,5);
    D3DDevice_SetTextureStageState(0,D3DTSS_ALPHAARG1,2);
    D3DDevice_SetTextureStageState(0,D3DTSS_ALPHAARG2,0);
    D3DDevice_SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);
    D3DDevice_SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,1);
    
    //todo: see what this does:
    //D3DDevice_SetRenderState(D3DRS_SIMPLE_UNUSED8,3);
    
    tmp = FtoDW(0.0);
    D3DDevice_SetRenderState(D3DRS_BACKAMBIENTMATERIALSOURCE,tmp);
    tmp = FtoDW(0.0);
    D3DDevice_SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,tmp);
    tmp = FtoDW(0.0);
    D3DDevice_SetRenderState(D3DRS_AMBIENTMATERIALSOURCE,tmp);
    tmp = FtoDW(1.0);
    D3DDevice_SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE,tmp);
    return;
}

/* --------------------------------------------------------------------------------
Function : bdSetFogRange
Purpose : Set the ranges for distance fog
Parameters : near distance, far distance
Returns : 
Info : 
*/

void bdSetFogRange(float nearDist, float farDist){
    // D3DDevice_SetRenderState(D3DRS_SIMPLE_UNUSED7,(DWORD)nearDist);
    // D3DDevice_SetRenderState(D3DRS_SIMPLE_UNUSED6,(DWORD)farDist);
    D3DDevice_SetRenderState(D3DRS_FOGSTART,*((DWORD*)&nearDist));
    D3DDevice_SetRenderState(D3DRS_FOGEND,*((DWORD*)&farDist));
    bRenderState.fogNear = nearDist;
    bRenderState.fogFar = farDist;
    bRenderState.fogRange = 1.0f / (farDist - nearDist);
    return;
}


/* --------------------------------------------------------------------------------
Function : bdSetFogColour
Purpose : Set the colour for distance fog
Parameters : red, green, blue (each 0..255)
Returns : 
Info : 
*/

void bdSetFogColour(int red, int green, int blue){
    
    //D3DDevice_SetRenderState
    //          (D3DRS_POINTSCALE_A,  // <-this is probably wrong, given the context
    //           (red & 0xffU) << 0x10 | 0xff000000 | (green & 0xffU) << 8 | blue & 0xffU);
    
    D3DDevice_SetRenderState
        (D3DRS_FOGCOLOR,
        (red & 0xffU) << 0x10 | 0xff000000 | (green & 0xffU) << 8 | blue & 0xffU);
    bRenderState.rFog = red;
    bRenderState.gFog = green;
    bRenderState.bFog = blue;
    return;
}


/*	--------------------------------------------------------------------------------
Function : bSetGlobalAlpha
Purpose : set the global alpha scale value for actor drawing
Parameters : new alpha scale value (256=solid)
Returns : 
Info : platform specific code called from baSetGlobalAlpha
*/

void bSetGlobalAlpha(int newAlphaScale){
    
    if (newAlphaScale >= 0x100) {
        newAlphaScale = 0xff;
    }
    
    D3DDevice_SetRenderState(D3DRS_TEXTUREFACTOR, newAlphaScale << 24);
    // D3DDevice_SetRenderState(D3DRS_DEFERRED_UNUSED6,newAlphaScale << 0x18);
    return;
}



/*	--------------------------------------------------------------------------------
Function : bdCreateRenderTarget
Purpose : create an off-screen render target
Parameters : width in pixels, height in pixels, rgb depth in bits, Z depth in bits, flags (see BCREATERENDERTARGET_)
Returns : ptr to target or NULL for failure
Info : 
*/

TBRenderTarget *bdCreateRenderTarget(int width, int height, int rgbBits, int zBits, uint32 flags){
    TBRenderTarget *target;
    unsigned int sz;
    DWORD pitch;
    int i;
    D3DTexture *d3dTex;
    TBTextureData *textureData;
    D3DFORMAT zFormat;
    void *texData;
    void *zData;
    D3DFORMAT format;
    // todo here
    
    zData = NULL;
    /*target = (TBRenderTarget *)bkHeapAllocEx(
    sizeof(TBRenderTarget),
    __FILE__,__LINE__,
    0x2001,
    0x2c00,
    0
    );*/
    target = TARGET_ALLOC();
    if (target == NULL) {
        return NULL;
    }
    
    memset(&(target->dummyTexture), 0, sizeof(TBTexture));
    
    (target->dummyTexture).xDim = (ushort)width;
    (target->dummyTexture).yDim = (ushort)height;
    
    if ((flags & BCREATERENDERTARGET_PRIVATEZ) == 0) {
        // No private Z Buffer
        zBits = 32;
        rgbBits = 32;
        
        if (bDisplayInfo.xRes < width) {
            bkPrintf(
                "bdCreateRenderTarget: *** specified width (%d) exceeds screen width (%d) and cannot be used without a private Z buffer, clamping to %d ***\n",
                width,
                bDisplayInfo.xRes,
                bDisplayInfo.xRes
                );
            width = bDisplayInfo.xRes;
        }
        if (bDisplayInfo.yRes < height) {
            bkPrintf(
                "bdCreateRenderTarget: *** specified height (%d) exceeds screen height (%d) and cannot be used without a private Z buffer, clamping to %d ***\n",
                height,
                bDisplayInfo.yRes,
                bDisplayInfo.yRes
                );
            height = bDisplayInfo.yRes;
        }
        
        sz = ((width * height * 4 + 0xfff) >> 12) << 12; // <- align mem
        //texData = bkHeapAllocEx(sz,__FILE__,__LINE__,0x2003,0x2d60,0x1000);
        texData = BUFFER_ALLOC(sz,0x1000);
        if (texData == NULL) {
            bkPrintf("bdCreateRenderTarget: Out of memory\n");
            bkHeapFree(target,"target");
            return NULL;
        }
        
        if ((flags & BCREATERENDERTARGET_DYNAMIC) == 0) {
            XPhysicalProtect(
                texData,
                sz,
                PAGE_READWRITE | PAGE_WRITECOMBINE
                );
        }
        
        XGSetTextureHeader(
            width,
            height,
            1, // levels
            0, // usage (ignored)
            D3DFMT_LIN_A8R8G8B8, // format
            0, // pool (ignored)
            &target->d3dTexture, // pTexture
            0, // Data
            width << 2 // pitch
            ); 
        
        target->d3dTexture.Register(texData);
        (target->d3dTexture).Common |=  0x80000;
        
        format = D3DFMT_LIN_A8R8G8B8;
        zFormat = D3DFMT_UNKNOWN;
    }
    else if (zBits == 0) {
        // No Z-buffer, I guess
        if (rgbBits == 0x16) {
            format = D3DFMT_LIN_R5G6B5;
        }
        else if (rgbBits == 8) {
            format = D3DFMT_LIN_L8;
        }
        else {
            format = D3DFMT_LIN_A8R8G8B8;
        }
        
        sz = (((width * height * rgbBits >> 3) + 0xfff) >> 0xc) << 0xc;
        //texData = bkHeapAllocEx(sz,__FILE__,__LINE__,0x2003,0x2f14,0x1000);
        texData = BUFFER_ALLOC(sz, 0x1000);
        if (texData == NULL) {
            bkPrintf("bdCreateRenderTarget: Out of memory\n");
            bkHeapFree(target, "target");
            return (TBRenderTarget *)NULL;
        }
        
        if ((flags & BCREATERENDERTARGET_DYNAMIC) == 0) {
            XPhysicalProtect(
                texData,
                sz,
                PAGE_READWRITE | PAGE_WRITECOMBINE
                );
        }
        
        XGSetTextureHeader(
            width,
            height,
            1,
            0,
            format,
            0,
            &target->d3dTexture,
            0,
            width * rgbBits >> 3
            );
        
        target->d3dTexture.Register(texData);
        (target->d3dTexture).Common |=  0x80000;
        
        zFormat = D3DFMT_UNKNOWN;
    }
    else {
        if (rgbBits != zBits) {
            bkPrintf(
                "bdCreateRenderTarget: *** RGB and Z depths do not match (rgb %d, z %d), forcing both to %d\n",
                rgbBits,
                zBits,
                rgbBits
                );
            zBits = rgbBits;
        }
        
        if (rgbBits == 16) {
            format = D3DFMT_LIN_R5G6B5;
            zFormat = D3DFMT_LIN_D16;
        }
        else {
            format = D3DFMT_LIN_A8R8G8B8;
            zFormat = D3DFMT_LIN_D24S8;
        }
        
        sz = ((((width * height * rgbBits) >> 3) + 0xfff) >> 12) << 12;
        //texData = bkHeapAllocEx(0x1000,__FILE__,__LINE__,0x2003,0x2e2c,0x1000);
        texData = BUFFER_ALLOC(sz,0x1000);
        if (texData == NULL) {
            bkPrintf("bdCreateRenderTarget: Out of memory\n");
            bkHeapFree(target,"target");
            return NULL;
        }
        
        if ((flags & BCREATERENDERTARGET_DYNAMIC) == 0) {
            XPhysicalProtect(
                texData,
                sz,
                PAGE_READWRITE | PAGE_WRITECOMBINE
                );
        }
        
        pitch = width * rgbBits >> 3;
        XGSetTextureHeader(
            width,
            height,
            1,
            0,
            format,
            0,
            &target->d3dTexture,
            0,
            width * rgbBits >> 3);
        
        target->d3dTexture.Register(texData);
        (target->d3dTexture).Common |=  0x80000;
        
        /*zData = bkHeapAllocEx( 
        pitch,
        __FILE__, __LINE__,
        0x2003,
        0x2e9c,
        0x1000
        );*/
        zData = BUFFER_ALLOC(sz, 0x1000);
        if (zData == NULL) {
            bkPrintf("bdCreateRenderTarget: Out of memory\n");
            XPhysicalProtect(texData,sz,4);
            bkHeapFree(texData,"texData");
            bkHeapFree(target,"target");
            return NULL;
        }
        if ((flags & BCREATERENDERTARGET_DYNAMIC) == 0) {
            XPhysicalProtect(zData,
                sz,
                PAGE_READWRITE | PAGE_WRITECOMBINE);
        }
        (target->d3dZBuffer).Data = 0;
        (target->d3dZBuffer).Common = 0xd0001;
        (target->d3dZBuffer).Lock = 0;
        (target->d3dZBuffer).Format = zFormat << 8 | 0x1002a; // ?
        (target->d3dZBuffer).Size =
            (((int)((width * rgbBits >> 3) + (width * rgbBits >> 0x1f & 0x3fU)) >> 6) + -1) * 0x1000000
            | (height + -1) * 0x1000 | width - 1U; // ?? todo: fix math
        (target->d3dZBuffer).Parent = (D3DBaseTexture *)0x0;
        target->d3dZBuffer.Register(zData);
    }
    target->d3dTexture.GetSurfaceLevel(0, &target->d3dSurface);
    target->rgbFormat = format;
    target->zFormat = zFormat;
    target->texData = texData;
    target->zData = zData;
    target->textureBytes = sz;
    target->vpY = 0;
    target->vpX = 0;
    target->width = width;
    target->vpWidth = width;
    target->height = height;
    target->vpHeight = height;
    target->rgbDepth = rgbBits;
    target->zDepth = zBits;
    target->flags = flags;
    target->clipYPos = 0;
    target->clipXPos = 0;
    target->clipWidth = width;
    target->clipHeight = height;
    (target->dummyTexture).curFrame = &target->d3dTexture;
    (target->dummyTexture).frame = &target->dummyTextureData;
    d3dTex = &target->d3dTexture;
    textureData = &target->dummyTextureData;
    for (i = 5; i != 0; --i) {
        (textureData->header).Common = d3dTex->Common;
        d3dTex = (D3DTexture *)&d3dTex->Data;
        textureData = (TBTextureData *)&(textureData->header).Data;
    }
    (target->dummyTexture).flags = 0x20;
    target->next = (TBRenderTarget *)&bRenderTargetList;
    target->prev = bRenderTargetList.prev;
    target->prev->next = target;
    target->next->prev = target;
    return target;
}


/*	--------------------------------------------------------------------------------
Function : bdDeleteRenderTarget
Purpose : delete an off-screen render target
Parameters : ptr to render target or NULL to delete all render targets
Returns : 
Info : 
*/

void bdDeleteRenderTarget(TBRenderTarget *target){
    int busy;
    int textureIdx;
    
    if (target == NULL) {
        while (bRenderTargetList.next != &bRenderTargetList) {
            bdDeleteRenderTarget(bRenderTargetList.next);
        }
    }
    else {
        busy = D3DResource_IsBusy((D3DResource *)target->d3dSurface);
        if (busy != 0) {
            D3DResource_BlockUntilNotBusy((D3DResource *)target->d3dSurface);
        }
        
        if ((TBRenderTarget *)bDisplayInfo.curRenderTarget == target) {
            bdSetRenderTarget(NULL,0,0,0,0,1.0,0);
        }
        
        for (textureIdx = 0; textureIdx < 4; textureIdx = textureIdx + 1) {
            if ((TBRenderTarget *)bRenderState.currentTexture[textureIdx] == target) {
                bdSetTexture(textureIdx,NULL);
            }
        }
        
        target->next->prev = target->prev;
        target->prev->next = target->next;
        
        if (target->texData != NULL) {
            XPhysicalProtect(target->texData,target->textureBytes,4);
            bkHeapFree(target->texData,"target->texData");
            target->texData = NULL;
        }
        
        if (target->zData != NULL) {
            XPhysicalProtect(target->zData,target->textureBytes,4);
            bkHeapFree(target->zData,"target->zData");
            target->zData = NULL;
        }
        bkHeapFree(target,"target");
    }
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

int bdSetRenderTarget(TBRenderTarget *target, int r,int g,int b, int a,float depth, uint32 flags){
    
    int result;
    char *errStr;
    D3DSurface *newZStencil;
    int check;
    static int first = 1;
    static TBPrimVertex2D verts[4];
    
    
    if(((target == NULL) || (target->rgbFormat == D3DFMT_LIN_A8R8G8B8)) || (a == 0xFF) ){
        check = 1;
    } else {
        check = 0;
    }
    bkAssert(
        BASSERT_RENDERTARGET_PARAM_INVALID,
        check,
        "bdSetRenderTarget: Invalid to specify alpha levels on a non 32 bit render target (%d!=255)\n",
        a
        );
    
    if (target == NULL) {
        // Target is the display
        D3DDevice_SetRenderTarget(
            (D3DSurface *)bDisplayInfo.backBuffer,
            (D3DSurface *)bDisplayInfo.depthStencilBuffer
            );
        target = &bRenderTargetList;
    } else {
        if(target->zDepth == 0){
            newZStencil = NULL;
        } else if (target->zData == NULL) {
            newZStencil = bDisplayInfo.depthStencilBuffer;
        } else {
            newZStencil = &target->d3dZBuffer;
        }
        
        result = D3DDevice::SetRenderTarget(target->d3dSurface, newZStencil);
        if(result < 0){
            // Idk I think SetRenderTarget always returns S_OK
            // so will this ever run?
            errStr = bDXErrorToString(result);
            bkPrintf("bdSetRenderTarget: SetRenderTarget FAILED (%s)\n", errStr);
            BSOFTBREAK();
        }
    }
    
    bDisplayInfo.curRenderTarget = target;
    
    
    bdSetViewport(target->vpX,target->vpY,target->vpWidth,target->vpHeight);                                                                          
    bdSetClipRectangle(target->clipXPos,target->clipYPos,target->clipWidth,target->clipHeight);
    if (target->rgbDepth == 8) {
        if(first != 0){
            
            verts[3].z = 0.1f;
            verts[2].z = 0.1f;
            verts[1].z = 0.1f;
            verts[0].z = 0.1f;
            verts[0].rhw = 1.0f;
            verts[2].x = 0.0f;
            verts[1].y = 0.0f;
            verts[0].y = 0.0f;
            verts[0].x = 0.0f;
            first = 0;
        }
        verts[0].colour = b & 0xffU | 0xff000000;
        verts[1].x = (float)target->vpWidth;
        verts[2].y = (float)target->vpHeight;
        verts[3].x = (float)target->vpWidth;
        verts[3].y = (float)target->vpHeight;
        verts[1].colour = verts[0].colour;
        verts[2].colour = verts[0].colour;
        verts[3].colour = verts[0].colour;
        
        bdDrawPrimitive(4,verts,4);
    }  else if (((flags & 1) == 0) || ((flags & 2) == 0)) {
        if ((flags & 1) == 0) {
            if ((flags & 2) != 0) {
                D3DDevice_Clear(0,NULL,3,0,depth,0);
            }
        }
        else {
            D3DDevice_Clear(0,NULL,0xf0,
                a << 0x18 | (r & 0xffU) << 0x10 | (g & 0xffU) << 8 | b & 0xffU,depth,0)
                ;
        }
    }
    else {
        D3DDevice_Clear(0,NULL,0xf3,
            a << 0x18 | (r & 0xffU) << 0x10 | (g & 0xffU) << 8 | b & 0xffU,depth,0);
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

void bdGetRenderTargetInfo(TBRenderTarget *target, int *width, int *height, int *rgbDepth, int *zDepth){
    if (width != (int *)0x0) {
        *width = target->width;
    }
    if (height != (int *)0x0) {
        *height = target->height;
    }
    if (rgbDepth != (int *)0x0) {
        *rgbDepth = target->rgbDepth;
    }
    if (zDepth != (int *)0x0) {
        *zDepth = target->zDepth;
    }
}


/*	--------------------------------------------------------------------------------
Function : bdSetRenderTargetAsTexture
Purpose : set a render target as a texture
Parameters : ptr to render target, zero-based texture stage
Returns : OK/FAIL
Info : 
*/

int bdSetRenderTargetAsTexture(TBRenderTarget *target, int stage){
    TBMatrix texMatrix;
    
    // Set default texture stage state if no texture bound and stage == 0
    if (bRenderState.currentTexture[stage] == NULL && stage == 0)
    {
        D3DDevice_SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE); // 0x5
        D3DDevice_SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE); // 0x5
    }
    
    // If target has flag 0x8, flush write-combined CPU cache
    if (target->flags & 0x8)
    {
        __asm {
            wbinvd
        }
    }
    
    // Bind dummy texture to render state tracker
    bRenderState.currentTexture[stage] = &target->dummyTexture;
    
    // Set actual texture on the D3D device
    D3DDevice_SetTexture(stage, (D3DBaseTexture*)&target->d3dTexture);
    
    // Initialize texture matrix to identity / zero
    ZeroMemory(&texMatrix, sizeof(TBMatrix));
    
    // Set up basic scaling matrix based on target width and height
    texMatrix[0][0] = (float)target->width;
    texMatrix[1][1] = (float)target->height;
    texMatrix[2][2] = 1.0f;
    texMatrix[3][3] = 1.0f;
    
    // Upload the transform matrix to the GPU
    bdSetTextureLinearMatrix(stage, texMatrix);
    
    // Configure texture stage transform (legacy fixed-function pipeline)
    D3DDevice_SetTextureStageState(stage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
    D3DDevice_SetTextureStageState(stage, D3DTSS_TEXCOORDINDEX, stage);
    
    // Mark the texture stage as using linear texture transform
    bDisplayInfo.linearTexture[stage] = 1;
    
    return 1;
}



/*	--------------------------------------------------------------------------------
Function : bdLockRenderState
Purpose : increment the lock count on a render state
Parameters : render state
Returns : new lock count
Info : 
*/

int bdLockRenderState(int renderState){
    bRenderState.renderStateLock[renderState + 1] = bRenderState.renderStateLock[renderState + 1] + 1;
    return bRenderState.renderStateLock[renderState + 1];
}


/*	--------------------------------------------------------------------------------
Function : bdUnlockRenderState
Purpose : decrement the lock count on a render state
Parameters : render state
Returns : new lock count
Info : 
*/

int bdUnlockRenderState(int renderState){
    bRenderState.renderStateLock[renderState + 1] = bRenderState.renderStateLock[renderState + 1] + 1;
    return bRenderState.renderStateLock[renderState + 1];
}