#define __BABEL__
#define _BDISPLAY_

#include <babel.h>


// TMP: MOVE TO XBViewport:


// ********************************************************************************
// Globals

extern TBViewInfo		bViewInfo;					// view information container
extern D3DMATRIX		bViewMatrix;				// the current view matrix
extern D3DMATRIX		*bProjectionMatrix;			// current projection matrix
extern int				bObjectMatrixIsIdentity;	// TRUE if the object matrix is identity
//


// ********************************************************************************
// Globals

TBDisplayInfo	bDisplayInfo;		// display information container
uint				bRColLUT[256];		// colour lookup for Red
uint				bGColLUT[256];		// colour lookup for Green
uint				bBColLUT[256];		// colour lookup for Blue
uint				bAColLUT[256];		// colour lookup for Alpha
uint				bRGBInvColLUT[256];	// inverse colour lookup for Red,Green and Blue
uint				bAInvColLUT[256];	// inverse colour lookup for Alpha
float			bRGBNormLUT[256];	// colour lookup to normalise 0..127 into 0..1
uint				bFlipCount;			// the number of calls to bdFlip since the program started.
TBTimerValue		bLastFrameDuration;	// duration of last frame
float			bFPS;				// last average frame rate

// tomatoes: These are defined elsewhere:
//int bCurrentVertexShader; //in xbDrawPrim
//char bHomeDirectory[256]; //in xbKernel
//TBMatrix bIdentityMatrix; //in xbMatrix

// Tomatoes: these were not defined elsewhere it seems:
TBVertexBuffer bVertexBufferList;
TBIndexBuffer bIndexBufferList;
//TBRenderTarget bRenderTargetList; //actually was in xbRender lol

int _displayInit;
float _dummyPresentDuration;

TBPreFlipCallback _preFlipCallback;
void* _preFlipContext;

TBTimerValue _frameStartTime = 0;
TBStopwatch _fpsStopwatch;

// Private function to save the surface
int bSaveSurfaceAsBMP(D3DSurface *surface,char *filename){
	FILE* bmpFile;
	void* pBits;
	int xDim, yDim;
	int pitch;
	int rgb555;
	int bpp;

	if(bDisplayInfo.started == 0){
		return 0;
	}

	bmpFile = fopen(filename, "wb");
	if(bmpFile == NULL){
		bkPrintf("bSaveSurfaceAsBMP: Could not write file '%s'\n", filename);
		return 0;
	}

	pBits = bLockSurface(surface, &xDim, &yDim, &bpp, &rgb555, &pitch);
	if(pBits == NULL){
		bkPrintf("bSaveSurfaceAsBMP: Could not lock/describe back buffer\n");
		fclose(bmpFile);
		return 0;
	}

	

	// Tomatoes: TODO: finish this function (it's a wee bit nasty);
	// Until then:
	fclose(bmpFile);
	return 0;
}

/* --------------------------------------------------------------------------------
   Function : bInitDisplay
   Purpose : initialise the display module
   Parameters : 
   Returns : OK/FAIL
   Info : 
*/

int bInitDisplay(){
  int result;
  
  memset(&bDisplayInfo, 0, sizeof(TBDisplayInfo));
  memset(&bViewInfo, 0, sizeof(TBViewInfo));
  bFlipCount = 0;

  bViewInfo.xFov = 1.2217305f;
  bViewInfo.yFov = 0.91629785f;
  bViewInfo.nearClip = 40.0f;
  bViewInfo.farClip = 10000.0f;

  bDisplayInfo.noofPolysProcessed = 0;

  _displayInit = 1;

  bVertexBufferList.prev = &bVertexBufferList;
  bVertexBufferList.next = &bVertexBufferList;
  bIndexBufferList.prev = &bIndexBufferList;
  bIndexBufferList.next = &bIndexBufferList;
  bRenderTargetList.prev = &bRenderTargetList;
  bRenderTargetList.next = &bRenderTargetList;

  result = bTextureInit();
  if (result != 0) {
    bDisplayInfo.curRenderTarget = &bRenderTargetList;
    bkInitStopwatch(&_fpsStopwatch, 30);
    memcpy(&bViewInfo, bIdentityMatrix, sizeof(TBMatrix));
  }
  return (int)(result != 0);
}


/* --------------------------------------------------------------------------------
   Function : bShutdownDisplay
   Purpose : shutdown the display module
   Parameters : 
   Returns : 
   Info : 
*/

void bShutdownDisplay(){
  bTextureShutdown();
  while (bVertexBufferList.next != &bVertexBufferList) {
    bdVertexBufferDestroy(bVertexBufferList.next);
  }
  while (bIndexBufferList.next != &bIndexBufferList) {
    bdIndexBufferDestroy(bIndexBufferList.next);
  }
  bdDeleteRenderTarget(NULL);
  return;
}


/* --------------------------------------------------------------------------------
   Function : bdOpenDisplay
   Purpose : setup the display for rendering
   Parameters : xRes, yRes, bit depth, Z buffer depth, flags
   Returns : OK/FAIL
   Info : 
*/

int bdOpenDisplay(int xRes, int yRes, int bpp, int zDepth, uint32 flags){
  int result;
  
  if ((bDisplayInfo.flags & BDISPLAYFLAG_OPEN) == 0) {
    bDisplayInfo.xRes = xRes;
    bDisplayInfo.yRes = yRes;
    bDisplayInfo.bpp = 32;
    bDisplayInfo.zDepth = 24; // The PC version probably takes the values from the parameters
    bDisplayInfo.stencilDepth = 8;
    bDisplayInfo.flags = flags;
    while (result = bStartDisplay(), result == 0) {
      bkPrintf(
		  "bdOpenDisplay: *** WARNING *** Failed to start display @ (%d x %d), %d bpp, %d Z\n",
		  bDisplayInfo.xRes,
		  bDisplayInfo.yRes,
		  bDisplayInfo.bpp,
		  bDisplayInfo.zDepth
	  );
	  // Seems here if we didn't initialize the display properly the first time
	  // we keep trying smaller resolutions until something works
	  // or if we get smaller than 640x480, give up.
      if (bDisplayInfo.xRes < 1281) {
        if (bDisplayInfo.xRes < 1153) {
          if (bDisplayInfo.xRes < 1025) {
            if (bDisplayInfo.xRes < 801) {
              if (bDisplayInfo.xRes < 641) {
                bkPrintf("bdOpenDisplay: *** FATAL ERROR *** Could not open display in any resolution or depth combination\n");
                return 0;
              }
              bDisplayInfo.xRes = 640;
              bDisplayInfo.yRes = 480;
            }
            else {
              bDisplayInfo.xRes = 800;
              bDisplayInfo.yRes = 600;
            }
          }
          else {
            bDisplayInfo.xRes = 1024;
            bDisplayInfo.yRes = 768;
          }
        }
        else {
          bDisplayInfo.xRes = 1152;
          bDisplayInfo.yRes = 864;
        }
      }
      else {
        bDisplayInfo.xRes = 1280;
        bDisplayInfo.yRes = 1024;
      }
    }
    bInitFont();
    bkPrintf(
		"Display established at (%d x %d) in 32 bpp with a 24 bit Z and 8 bit stencil\n",
		bDisplayInfo.xRes,
		bDisplayInfo.yRes
	);
    bdSetViewClipPlanes(10.0,10000.0);

	//1.22173 rad is 70.0° (horiz fov)
    //0.9162979 rad is 52.5° (vertical fov)

    bdSetFOV(1.22173f, 0.9162979f);
    _controlfp(0,8);
    bCheckSignature();
    result = 1;
  }
  else {
    bkPrintf("bdOpenDisplay: Display is already open\n");
    result = 0;
  }
  return result;
}


/* --------------------------------------------------------------------------------
   Function : bdCloseDisplay
   Purpose : shutdown the open display
   Parameters : 
   Returns : 
   Info : 
*/

void bdCloseDisplay(){
  if ((bDisplayInfo.flags & BDISPLAYFLAG_OPEN) == 0) {
    bkPrintf("bdCloseDisplay: Display is not open\n");
  }
  else {
    bShutdownFont();
    bStopDisplay(1);
    bDisplayInfo.flags = bDisplayInfo.flags & 0xfffffffe;
  }
}


/* --------------------------------------------------------------------------------
   Function : bdGetDisplayInfo
   Purpose : retrieve display parameters
   Parameters : ptr for xRes, ptr for yRes, ptr for bpp, ptr for zDepth
   Returns : 
   Info : 
*/

void bdGetDisplayInfo(int *xRes, int *yRes, int *bpp, int *zDepth){
 
  if (xRes != NULL) {
    *xRes = bDisplayInfo.xRes;
  }
  if (yRes != NULL) {
    *yRes = bDisplayInfo.yRes;
  }
  if (bpp != NULL) {
    *bpp = bDisplayInfo.bpp;
  }
  if (zDepth != NULL) {
    *zDepth = bDisplayInfo.zDepth;
  }
}

/* --------------------------------------------------------------------------------
   Function : bdSetVsyncCallback
   Purpose : Set a callback function to be called once per Vsync
   Parameters : callback function, context
   Returns : OK/FAIL
   Info : 
*/

int bdSetVsyncCallback(TBVsyncCallback callback, void *context){
	// Seems to do nothing on Xbox
	return 0;
}


/* --------------------------------------------------------------------------------
   Function : bdSetPreFlipCallback
   Purpose : Set callback just before the flip - and after the anti aliasing
   Parameters : callback function, user context
   Returns : OK/FAIL
   Info : 
*/

int bdSetPreFlipCallback(TBPreFlipCallback callback, void *context){ 
  _preFlipCallback = callback;
  _preFlipContext = context;
  return 1;
}


/* --------------------------------------------------------------------------------
   Function : bdFlip
   Purpose : page-flip the display and clear the viewport
   Parameters : colour for viewport clearing, update flags
   Returns : 
   Info : 
*/

uint tomatoes_bdClearMode = BDCLEARMODE_FRAME | BDCLEARMODE_Z | BDCLEARMODE_STENCIL;

void bdFlip(int red, int green, int blue, int alpha, int flags){
  int result;
  TBTimerValue lastFrameDurationLocal;
  DWORD d3dClearFlags = 0;
  D3DCOLOR d3dColor = 0;
  
  lastFrameDurationLocal = bLastFrameDuration;
  if (bDisplayInfo.started != 0) {
    if ((TBRenderTarget *)bDisplayInfo.curRenderTarget != &bRenderTargetList) {
      bdSetRenderTarget((_TBRenderTarget *)0x0,0,0,0,0,1.0,0);
    }
    if ((flags & BFLIPFLAG_UPDATE) != 0) {
      bkUpdate(BUPDATEMODULE_ALL);
    }
    D3DDevice::Swap(D3DSWAP_COPY);
    if (_preFlipCallback != NULL) {
      (*_preFlipCallback)(_preFlipContext);
    }
    D3DDevice::Swap(D3DSWAP_FINISH); 
    lastFrameDurationLocal = bkTimerRead();
    bLastFrameDuration =
         bkTimerDelta(_frameStartTime,
                      lastFrameDurationLocal);
    result = bkStopStopwatch(&_fpsStopwatch);
    if (result != 0) {
      bFPS = bkTimerToFPS(_fpsStopwatch.value);
    }
    bkStartStopwatch(&_fpsStopwatch);
    _frameStartTime = lastFrameDurationLocal;

	// Tomatoes: This section I'm unsure about
	// ghidra decompiled if (__fclose != 0) 
	// but that doesn't make sense.
	// Seems it's checking some var for a BDCLEARMODE
	// but cannot find the variable itself/what it's called.
    if (tomatoes_bdClearMode != 0) { // TODO better figure out what's actually happening here
		if (tomatoes_bdClearMode & BDCLEARMODE_FRAME)
            d3dClearFlags |= D3DCLEAR_TARGET;

        if (tomatoes_bdClearMode & BDCLEARMODE_Z)
            d3dClearFlags |= D3DCLEAR_ZBUFFER;

        if (tomatoes_bdClearMode & BDCLEARMODE_STENCIL)
            d3dClearFlags |= D3DCLEAR_STENCIL;
        d3dColor = ((alpha & 0xFF) << 24) | ((red & 0xFF) << 16) | ((green & 0xFF) << 8) | (blue & 0xFF);
		
		D3DDevice_Clear(0,0,d3dClearFlags,
                          d3dColor,1.0f,0);
    }
	// End unsure
    bFlipCount = bFlipCount + 1;
    lastFrameDurationLocal = bLastFrameDuration;
  }
  bLastFrameDuration = lastFrameDurationLocal;
  return;
}

/* --------------------------------------------------------------------------------
   Function : bdBeginScene
   Purpose : prepare for rendering
   Parameters : 
   Returns : OK/FAIL
   Info : 
*/

int bdBeginScene(){
  bool started;
  
  started = bDisplayInfo.started != 0;
  if (started) {
    D3DDevice_BeginScene();
    bDisplayInfo.inScene = 1;
  }
  return (uint)started;
}


/* --------------------------------------------------------------------------------
   Function : bdEndScene
   Purpose : finish rendering
   Parameters : 
   Returns : 
   Info : 
*/

void bdEndScene(){
  bRenderConsoleWindow();
  bRenderGraphs();
  D3DDevice_EndScene();
  bDisplayInfo.inScene = 0;
  return;
}

/* --------------------------------------------------------------------------------
   Function : bdGetScanline
   Purpose : return the current scanline number
   Parameters : 
   Returns : scanline number or -1 for failure
   Info : 
*/

int bdGetScanline(){
  D3DRASTER_STATUS status;
  
  D3DDevice_GetRasterStatus(&status);
  return status.ScanLine;
}

/* --------------------------------------------------------------------------------
   Function : bdScreenshot
   Purpose : save a screenshot as a BMP file
   Parameters : filename relative to home directory
   Returns : 1/0
   Info : 
*/

int bdScreenshot(char *filename){
	char buffer[0x100];
    sprintf(buffer, "%s%s", bHomeDirectory, filename);
    return bSaveSurfaceAsBMP((D3DSurface *)bDisplayInfo.frontBuffer, buffer);
}


/* --------------------------------------------------------------------------------
   Function : bRestoreAllSurfaces
   Purpose : attempt to restore all our DD surfaces
   Parameters : 
   Returns : 
   Info : 
*/

void bRestoreAllSurfaces(){
	// TODO: this seems to be missing in the static lib, idk if it's even used.
	// Taz does not call it directly, at least.
	// Time will tell if babel uses it somewheres.
	// until then:
	bkPrintf("Tomatoes: We actually used bRestoreAllSurfaces\n");
}


/* --------------------------------------------------------------------------------
   Function : bStartDisplay
   Purpose : try to startup the display for the current size/bpp/z/mode
   Parameters : 
   Returns : OK/FAIL
   Info : 
*/
int bStartDisplay(){
  int result;
  int freeMem;
  int hResult;
  char *errMessage;
  int freeMemAfterOpen;
  ulong d3dResult;
  int i_lut;
  D3DMATERIAL8 material;
  D3DPRESENT_PARAMETERS present_params;
  D3DCAPS8 caps;
  float tmpFloat;
  
  if (bDisplayInfo.started == 0) {
    bkPrintf(
		"bStartDisplay: Trying (%dx%d) 32 bpp 24:8 Z/stencil\n", 
		bDisplayInfo.xRes,
		bDisplayInfo.yRes
	);
    bDisplayInfo.d3d = (LPDIRECT3D8) Direct3DCreate8(0);
    if (bDisplayInfo.d3d == (void *)0x0) {
      bkPrintf("bStartDisplay: *** ERROR : Direct3DCreate8 Failed ***\n");
      result = 0;
    }
    else {
      if ((bDisplayInfo.flags & BDISPLAYFLAG_ANTIALIASED) == 0) {
        if ((bDisplayInfo.flags & BDISPLAYFLAG_ANTIALIASED4) == 0) {
          bDisplayInfo.xAAMul = 1;
          bDisplayInfo.yAAMul = 1;
          bDisplayInfo.aaType = D3DMULTISAMPLE_NONE;
        }
        else {
          bDisplayInfo.xAAMul = 2;
          bDisplayInfo.yAAMul = 2;
          bDisplayInfo.aaType = D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN;
        }
      }
      else {
        bDisplayInfo.xAAMul = 2;
        bDisplayInfo.yAAMul = 1;
        bDisplayInfo.aaType = D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX;
      }
      memset(&present_params,0,sizeof(D3DPRESENT_PARAMETERS));
      present_params.Windowed = 0;
      present_params.BackBufferFormat = D3DFMT_X8L8V8U8;
      present_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
      present_params.BackBufferWidth = bDisplayInfo.xRes;
      present_params.BackBufferHeight = bDisplayInfo.yRes;
      present_params.BackBufferCount = 1;
      present_params.EnableAutoDepthStencil = 1;
      present_params.AutoDepthStencilFormat = D3DFMT_D24S8;
      present_params.Flags = 0;
      present_params.MultiSampleType = bDisplayInfo.aaType;
	  if((bDisplayInfo.flags & BDISPLAYFLAG_NOVSYNC) != 0){
		// NOVSYNC FLAG IS PRESENT
		  present_params.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	  } else {
		  present_params.FullScreen_PresentationInterval = 1;
	  }
      result = Direct3D_GetDeviceCaps(0, D3DDEVTYPE_HAL, &caps);
      if (result < 0) {
        bDXErrorToString(result);
        bkPrintf("bStartDisplay: GetDeviceCaps failed (%s)\n");
        result = 0;
      }
      else {
        bkPrintf(
			"bStartDisplay: Device supports Vertex Shaders v%d.%d with %d constant registers\n",
			caps.PixelShaderVersion >> 8 & 0xff,
			caps.PixelShaderVersion & 0xff,
            caps.MaxPixelShaderValue
		);
        bkPrintf(
			"bStartDisplay: There are %d texture blend stages with %d simultaneous textures\n",
			caps.VertexProcessingCaps,
			caps.MaxActiveLights
		);
        bkPrintf(
			"bStartDisplay: Max point size is %.2f\n",
			(double)(float)caps.MaxVertexIndex
		);
        bDisplayInfo.tnlHardware = 1;
        bDisplayInfo.tnlActive = 1;
        bDisplayInfo.hwVertexShaders = (int)(caps.PixelShaderVersion != 0);
        freeMem = bGetFreePhysicalMemory();
        hResult = Direct3D_CreateDevice(0, D3DDEVTYPE_HAL,0,0x40,&present_params,&(bDisplayInfo.d3dDevice));
        if (hResult < 0) {
          errMessage = bDXErrorToString(hResult);
          bkPrintf("bStartDisplay: CreateDevice failed (%s)\n",errMessage);
          result = 0;
        }
        else {
          freeMemAfterOpen = bGetFreePhysicalMemory();

          bkPrintf(
			  "bStartDisplay: Frame buffers use %d Kb of RAM\n",
			  (freeMem - freeMemAfterOpen) >> 10
		  );
          
		  D3DDevice_GetBackBuffer(0,0,&(bDisplayInfo.backBuffer));
          result = D3DDevice_GetDepthStencilSurface(&(bDisplayInfo.depthStencilBuffer));
          if (result < 0) {
			// Failed to get depth stencil surface
			// Free resources.
            errMessage = bDXErrorToString(result);
            bkPrintf("bStartDisplay: GetDepthStencilBuffer failed (%s)\n", errMessage);
            if (bDisplayInfo.d3dDevice != NULL) {
              do {
                d3dResult = D3DDevice::Release();
              } while (d3dResult != 0);
              bDisplayInfo.d3dDevice = NULL;
            }
            if (bDisplayInfo.d3d != NULL) {
              do {
                d3dResult = Direct3D::Release();
              } while (d3dResult != 0);
              bDisplayInfo.d3d = NULL;
            }
            result = 0;
          }
          else {
            bDisplayInfo.depthStencilBuffer->Release();
            D3DDevice_GetBackBuffer(0xffffffff,0,&(bDisplayInfo.frontBuffer));
            bCurrentVertexShader = 1;
            bLastVertexBuffer = (_TBVertexBuffer *)0x1;
            bSetVertexShader(0x152,(_TBVertexBuffer *)0x0); // todo figure out what 0x152 means in the type parameter
            memset(&caps,0,sizeof(D3DCAPS8));
            D3DDevice_GetDeviceCaps(&caps);
            bMaxActiveLights = caps.MaxVertexBlendMatrices;
            bkPrintf("Device supports up to %d active HW light sources\n", caps.MaxVertexBlendMatrices);
            for (i_lut = 0; i_lut < 0x100; i_lut = i_lut + 1) {
              bRColLUT[i_lut] = i_lut << 0x10;
              bGColLUT[i_lut] = i_lut << 8;
              bBColLUT[i_lut] = i_lut;
              bAColLUT[i_lut] = i_lut << 0x18;
              if (i_lut < 0x80) {
                tmpFloat = (float)(i_lut * 2) / 255.0f;
              }
              else {
                tmpFloat = 1.0f;
              }
              bRGBNormLUT[i_lut] = tmpFloat;
              bRGBInvColLUT[i_lut] = i_lut;
              bAInvColLUT[i_lut] = i_lut;
            }
            bDisplayInfo.xResHalf = bDisplayInfo.xRes / 2;
            bDisplayInfo.yResHalf = bDisplayInfo.yRes / 2;
            bDisplayInfo.xAspect = 0.5;
            bDisplayInfo.yAspect =
                 ((float)bDisplayInfo.yRes / (float)bDisplayInfo.xRes) * 0.5f;
            bDisplayInfo.pixelFormat = 3;
            bCalcOrthoProjectionMatrix();
            bdSetViewport(0,0,bDisplayInfo.xRes,bDisplayInfo.yRes);
            memset(&material,0,0x44);
            material.Ambient.r = 0.0;
            material.Ambient.g = 0.0;
            material.Ambient.b = 0.0;
            material.Diffuse.r = 1.0;
            material.Diffuse.g = 1.0;
            material.Diffuse.b = 1.0;
            material.Specular.r = 0.0;
            material.Specular.g = 0.0;
            material.Specular.b = 0.0;
            material.Specular.a = 127.0;
            material.Emissive.r = 0.0;
            material.Emissive.g = 0.0;
            material.Emissive.b = 0.0;
            D3DDevice_SetMaterial(&material);
            bDisplayInfo.flags = bDisplayInfo.flags | 1;
            bSetDefaultRenderStates();
            bCalcPerspProjectionMatrix();
            bdSetProjectionMode(1,0);
            bdSetViewClipPlanes(1.0,10000.0);
            bdSetFogRange(8000.0,10000.0);
            bdSetFogColour(0,0,0);
            bInitShaderSelector();
            D3DDevice_Clear(0,0,0xf3,0,1.0f,0);
            D3DDevice::Present(NULL,NULL,NULL,NULL);
            D3DDevice_Clear(0,0,0xf3,0,1.0f,0);
            D3DDevice::Present(NULL,NULL,NULL,NULL);
            bDisplayInfo.inScene = 0;
            bDisplayInfo.started = 1;
           // bkPrintf("Dummy present took %f ms\n",(double)_dummyPresentDuration);
            result = 1;
          }
        }
      }
    }
  }
  else {
    result = 0;
  }
  return result;
}

/* --------------------------------------------------------------------------------
   Function : bStopDisplay
   Purpose : cleanup the display
   Parameters : in shutdown mode?
   Returns : 
   Info : 
*/

void bStopDisplay(int shutdown){
	ULONG result;

	if(bDisplayInfo.started == 0){
		return;
	}		
	
	bShutdownShaderSelector();
	bDisplayInfo.backBuffer->Release();
	
	if(bDisplayInfo.d3dDevice != NULL){
		do{
			result = D3DDevice_Release();
		} while( result != 0 );
		bDisplayInfo.d3dDevice = NULL;
	}
	if(bDisplayInfo.d3d != NULL){
		do{
			result = Direct3D_Release();
		} while( result != 0 );
		bDisplayInfo.d3d = NULL;
	}
	bDisplayInfo.started = 0;
}


/* --------------------------------------------------------------------------------
   Function : bGrabScreen
   Purpose : grab the back-buffer as a 24bit RGB buffer
   Parameters : 
   Returns : 
   Info : 
*/

uchar *bGrabScreen(){
	// Tomatoes: Also seems to be broken / Missing
	bkPrintf("Tomatoes: bGrabScreen was actually called\n");
	return NULL;

}


/* --------------------------------------------------------------------------------
   Function : bLockSurface
   Purpose : lock a surface returning it's parameters
   Parameters : surface, ptrs for width, height, bit depth, 555 flag, pitch
   Returns : ptr to surface data or NULL for failure
   Info : 
*/

void *bLockSurface(LPDIRECT3DSURFACE8 surf, int *xDim, int *yDim, int *bpp, int *rgb555, int *pitch){
	// Tomatoes: Ghidra's decompilation of this was absolutely cursed
	// so I've done it a little differently
	// but hopefully it has the same effect


	D3DSURFACE_DESC desc;
	D3DLOCKED_RECT lockedRect;
	tagRECT lockRect = {0,0,0,0};
	DWORD lockFlags = 0;
	HRESULT hResult;
	
	hResult = surf->GetDesc(&desc);
	if(FAILED(hResult)){
		// Tomatoes: according to the inline source of D3DSurface::GetDesc()
		// it will always return success? Unless I'm missing something this will 
		// never be exec'd
        const char *errorString = bDXErrorToString(hResult);
        bkPrintf("bLockSurface: GetDesc failed (%s)\n", errorString);
        return NULL;
	}

	hResult = surf->LockRect(&lockedRect, &lockRect, lockFlags);
	if(FAILED(hResult)){
		
        const char *errorString = bDXErrorToString(hResult);
        bkPrintf("bLockSurface: Lock failed (%s)\n", errorString);
        return NULL;
	}


    // Populate output parameters
    if (xDim) *xDim = desc.Width;
    if (yDim) *yDim = desc.Height;
    if (pitch) *pitch = lockedRect.Pitch;
    if (rgb555) *rgb555 = 0;
    if (bpp) {
        switch (desc.Format) {
            case D3DFMT_A1R5G5B5:
            case D3DFMT_X1R5G5B5:
            case D3DFMT_LIN_A1R5G5B5:
            case D3DFMT_LIN_X1R5G5B5:
                *bpp = 16;
                if (rgb555) *rgb555 = 1;
                break;
            case D3DFMT_R5G6B5:
            case D3DFMT_LIN_R5G6B5:
                *bpp = 16;
                break;
            case D3DFMT_A8R8G8B8:
            case D3DFMT_X8L8V8U8:
            case D3DFMT_LIN_A8R8G8B8:
            case D3DFMT_LIN_X8L8V8U8:
                *bpp = 32;
                break;
            default:
                *bpp = 0; // Unknown or unsupported format
                break;
        }
    }

    return lockedRect.pBits;
}


/* --------------------------------------------------------------------------------
   Function : bUnlockSurface
   Purpose : unlock a surface
   Parameters : surface
   Returns : 
   Info : 
*/

void bUnlockSurface(LPDIRECT3DSURFACE8 surf){    
	long hr = surf->UnlockRect();
    if (hr < 0) {
        bkPrintf("bUnlockSurface: *** Unlock surface FAILED (%s) ***\n", bDXErrorToString(hr));
    }
}


/* --------------------------------------------------------------------------------
   Function : bdSetScreenOffset
   Purpose : set screen x/y offset
   Parameters : x offset , y offset
   Returns : 
   Info : no effect yet on Xbox
*/

void bdSetScreenOffset(int x, int y){
	return;
}


/* --------------------------------------------------------------------------------
   Function : bdClear
   Purpose : clear the current clip rectangle of the current viewport
   Parameters : colour for viewport clearing, depth for Z clearing, value for stencil clearing, flags (see BDCLEARMODE_)
   Returns : 
   Info : 
*/

void bdClear(int red, int green, int blue, int alpha, float z, int32 stencil, uint32 flags){
	DWORD d3dClearFlags = 0;
    DWORD color;
    if (flags & BDCLEARMODE_FRAME)
        d3dClearFlags |= D3DCLEAR_TARGET; // 0xf0 = RGBA

    if (flags & BDCLEARMODE_Z)
        d3dClearFlags |= D3DCLEAR_ZBUFFER;

    if ((flags & BDCLEARMODE_STENCIL) && bDisplayInfo.stencilDepth != 0)
        d3dClearFlags |= D3DCLEAR_STENCIL;

    color =
        ((alpha & 0xff) << 24) |
        ((red   & 0xff) << 16) |
        ((green & 0xff) <<  8) |
        (blue   & 0xff);

    D3DDevice_Clear(0, 0, d3dClearFlags, color, z, stencil);
}


/* --------------------------------------------------------------------------------
   Function : bGetDisplayFrequency
   Purpose : return the display frequency in Hz
   Parameters : 
   Returns : display frequency in Hz
   Info : 
*/

int bGetDisplayFrequency(){    
	
	DWORD videoStandard = XGetVideoStandard();

    // PAL-I runs at 50Hz; all others (NTSC-M, NTSC-J) run at 60Hz
    return (videoStandard == XC_VIDEO_STANDARD_PAL_I) ? 50 : 60;
}


/* --------------------------------------------------------------------------------
   Function : bdDoDummyPresent
   Purpose : perform a dummy present call to retrieve the memory reserved by a previous applications PersistDisplay call
   Parameters : 
   Returns : 
   Info : this call *must* only be used before bdOpenDisplay
*/

void bdDoDummyPresent(){
  D3DPRESENT_PARAMETERS params;
  D3DDevice* device;
  
  Direct3DCreate8(0);
  memset(&params,0,sizeof(D3DPRESENT_PARAMETERS));
  params.BackBufferWidth = 0x280;
  params.BackBufferHeight = 0x1e0;
  params.BackBufferFormat = D3DFMT_X8L8V8U8;
  params.BackBufferCount = 1;
  params.EnableAutoDepthStencil = 1;
  params.AutoDepthStencilFormat = D3DFMT_D24S8;
  params.SwapEffect = D3DSWAPEFFECT_DISCARD;
  Direct3D_CreateDevice(
	  0,
      D3DDEVTYPE_HAL,
	  NULL,
	  D3DCREATE_HARDWARE_VERTEXPROCESSING,
	  &params,
	  &device);
  D3DDevice::Clear(0,(_D3DRECT *)0x0,0xf3,0,1.0,0);
  D3DDevice::Present((tagRECT *)0x0,(tagRECT *)0x0,(void *)0x0,(void *)0x0);
  D3DDevice::Release();
  Direct3D::Release();
  return;
}