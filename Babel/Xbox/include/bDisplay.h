// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : bDisplay.h
//   Purpose : include all Babel Display component headers
// Component : Xbox Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BDISPLAY_H__
#define __BDISPLAY_H__


// ********************************************************************************
// define mysteriously missing D3D function

#define D3DDevice_DrawPrimitive(PRIMTYPE, STARTVERTEX, PRIMCOUNT) bDisplayInfo.d3dDevice->DrawPrimitive(PRIMTYPE, STARTVERTEX, PRIMCOUNT)
#define D3DDevice_DrawPrimitiveUP(PRIMTYPE, PRIMCOUNT, VERTPTR, STRIDE) bDisplayInfo.d3dDevice->DrawPrimitiveUP(PRIMTYPE, PRIMCOUNT, VERTPTR, STRIDE)
#define D3DDevice_Present(source,dest,unused,unused2) bDisplayInfo.d3dDevice->Present(source,dest,unused,unused2)
#define D3DDevice_Swap(flag) bDisplayInfo.d3dDevice->Swap(flag)


// ********************************************************************************
// include all required headers

#include "bDisplay/xbDxdebug.h"
#include "bDisplay/xbDisplay.h"
#include "bDisplay/xbViewport.h"
#include "bDisplay/viewport.h"
#include "bDisplay/xbDrawprim.h"
#include "bDisplay/texture.h"
#include "bDisplay/xbTexture.h"
#include "bDisplay/render.h"
#include "bDisplay/xbRender.h"
#include "bDisplay/font.h"
#include "bDisplay/xbFont.h"
#include "bDisplay/xbSplash.h"
#include "bDisplay/xbVideo.h"
#include "bDisplay/frametimegraph.h"
#include "bDisplay/consoleWindow.h"
#endif		// __BDISPLAY_H__
