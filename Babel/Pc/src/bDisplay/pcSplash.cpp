// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcSplash.cpp
//   Purpose : Defines BSPLASHFLAG_SCALETOFIT and the _TBSplash struct. Declares functions for loading/deleting splashes, drawing splashes, and fixing splash CRCs
// Component : Babel Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Helper Functions

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bLoadSplashByCRC
	Purpose : load splash by CRC
	Parameters : index, crc
	Returns : splash
	Info : 
*/
TBSplash *bLoadSplashByCRC(TBPackageIndex *index, uint32 crc)
{
    // Request blob (optional outputs are NULL)
    uchar* base = bkLoadFileByCRC(index, crc, (uchar*)0, (int*)0, (TBFileTagInfo*)0, 0);
    if (!base) return (TBSplash*)0;

    TBSplash* splash = (TBSplash*)base;

    const int n = splash->count;
    if (n > 0) {
        for (int i = 0; i < n; ++i) {
            TBTexture* t = bLoadTextureByCRC(index, splash->crc[i]);
            splash->texture[i] = t;
            if (!t) {
                // Free-on-failure only if the first dword (part of TBResourceInfo) is negative
                if ( *(int*)splash < 0 ) {
                    bkHeapFree((void*)splash);
                }
                return (TBSplash*)0;
            }
        }
    }

    return splash; // in-place object
}


/*	--------------------------------------------------------------------------------
	Function : bDeleteSplash
	Purpose : delete splash
	Parameters : splash
	Returns : 
	Info : 
*/
void bDeleteSplash(TBSplash *splash)
{
    if (!splash) return;

    // Release all frame textures and null out pointers
    for (uint i = 0; i < (uint)(unsigned int)splash->count; ++i) {
        struct _TBTexture** pp = &splash->texture[i];
        if (*pp) {
            bDeleteTexture(*pp);
            *pp = 0;
        }
    }

    // Free header only if the resource is not marked as 'Loaded' (i.e., not owned by a package file)
    if (!splash->resInfo.packageId.loaded) {
        bkHeapFree(splash);
    }
}

/*	--------------------------------------------------------------------------------
	Function : bdDrawSplash
	Purpose : draw splash
	Parameters : splash, xPos, yPos, flags
	Returns : 
	Info : 
*/
void bdDrawSplash(TBSplash *splash, int xPos, int yPos, int flags)
{
    int i;
    if (!splash) return;

    // Validate textures
    for (i = 0; i < splash->count; ++i) {
        if (!splash->texture[i]) return;
    }

    // Begin scene (wrapper)
    bdBeginScene();

    // Target size
    const float tgtW = (flags & BSPLASHFLAG_SCALETOFIT) ? (float)bDisplayInfo.xRes : (float)splash->xRes;
    const float tgtH = (flags & BSPLASHFLAG_SCALETOFIT) ? (float)bDisplayInfo.yRes : (float)splash->yRes;

    // Center-based placement (+Y up)
    float originX, originY;
    if (xPos == -1) {
        originX = -tgtW * 0.5f;
        originY =  tgtH * 0.5f;
    } else {
        originX = (float)(xPos - bDisplayInfo.xResHalf);
        originY = (float)(bDisplayInfo.yResHalf - yPos);
    }

    // Grid from splash->data[0/1] (fallbacks if missing)
    int cellW = (int)(size_t)splash->data[0];
    int cellH = (int)(size_t)splash->data[1];
    if (cellW <= 0 || cellH <= 0) {
        TBTexture* first = splash->texture[0];
        if (first) { cellW = (int)first->xDim; cellH = (int)first->yDim; }
        if (cellW <= 0) cellW = splash->xRes;
        if (cellH <= 0) cellH = splash->yRes;
    }
    const int cols = (splash->xRes + cellW - 1) / cellW;
    const int rows = (splash->yRes + cellH - 1) / cellH;

    // Local render state
    bdPushRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_CLAMPU, 0);
    bdPushProjectionMode(BDISPLAY_PROJECTIONMODE2D);
    bdSetObjectMatrix(bIdentityMatrix);
    bdPushRenderState(BDRENDERSTATE_ZTEST,        BDZTESTMODE_ALWAYS, 0);
    bdPushRenderState(BDRENDERSTATE_LIGHTING,     0,                  0);
    bdPushRenderState(BDRENDERSTATE_BACKFACECULL, BDCULLMODE_OFF,     0);

    // Draw tiles (row-major)
    float yTop = originY;
    TBTexture** texPtr = splash->texture;
    int remaining = splash->count;
    
    for (int r = 0; r < rows && remaining > 0; ++r) {
        // Use the first tile of the row to compute row height
        TBTexture* rowTex = *texPtr;
        const float tileH = ((float)rowTex->yDim * tgtH) / (float)splash->yRes;
        const float yBot  = yTop - tileH;

        float xLeft = originX;
        for (int c = 0; c < cols && remaining > 0; ++c) {
            TBTexture* handle = *texPtr++;
            --remaining;

            const float tileW = ((float)handle->xDim * tgtW) / (float)splash->xRes;
            const float xRight = xLeft + tileW;

            // Bind and build quad (TRIANGLESTRIP: TL, BL, TR, BR), Z=0
            bdSetTexture(0, handle);

            TBPrimVertex v[4];
            for (i = 0; i < 4; ++i) {
                v[i].xn = 0.0f; v[i].yn = 0.0f; v[i].zn = 1.0f;
                BDVERTEX_SETRGBA(&v[i], 127, 127, 127, 127); // matches LUT path in disasm
            }
            v[0].x = xLeft;  v[0].y = yTop;  v[0].z = 0.0f; v[0].u = 0.0f; v[0].v = 0.0f; // TL
            v[1].x = xLeft;  v[1].y = yBot;  v[1].z = 0.0f; v[1].u = 0.0f; v[1].v = 1.0f; // BL
            v[2].x = xRight; v[2].y = yTop;  v[2].z = 0.0f; v[2].u = 1.0f; v[2].v = 0.0f; // TR
            v[3].x = xRight; v[3].y = yBot;  v[3].z = 0.0f; v[3].u = 1.0f; v[3].v = 1.0f; // BR

            bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, (void*)v, 4, BVERTTYPE_SINGLE);

            xLeft = xRight;
        }
        yTop = yBot;
    }

    // Restore (order per original)
    bdPopProjectionMode();
    bdPopRenderState(BDRENDERSTATE_ZTEST);
    bdPopRenderState(BDRENDERSTATE_LIGHTING);
    bdPopRenderState(BDRENDERSTATE_BACKFACECULL);
    bdPopRenderState(BDRENDERSTATE_TEXTUREWRAP);

    bdEndScene();
}

/*	--------------------------------------------------------------------------------
	Function : bdFixSplashCRC
	Purpose : fix splash CRC
	Parameters : crc
	Returns : fixed CRC
	Info : 
*/
uint32 bdFixSplashCRC(uint32 crc)
{
    return crc; // metal_pipe_falling_sound.wav
}
