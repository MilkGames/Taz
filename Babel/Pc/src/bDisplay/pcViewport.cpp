// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcViewport.cpp
//   Purpose : Defines enums for viewport rotation order and projection modes. Defines clip codes. Defines _TBViewInfo struct and TBObjectMatrixCallback function pointer
// Component : Babel Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Locals

D3DMATRIX  bOrthoProjectionMatrix;
D3DMATRIX  bOrthoProjectionWithClip;
D3DMATRIX  bPerspProjectionMatrix;
D3DMATRIX  bIdentityD3DMatrix = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};

TBObjectMatrixCallback bObjectMatrixCallback = NULL;
void*                  bObjectMatrixContext = NULL;

// ********************************************************************************
// Globals

TBViewInfo bViewInfo;
D3DMATRIX  bViewMatrix;
D3DMATRIX *bProjectionMatrix = &bPerspProjectionMatrix;
int        bObjectMatrixIsIdentity;

// ********************************************************************************
// Helper Functions

void bD3DMatMultiply(D3DMATRIX* dest, const D3DMATRIX* src1, const D3DMATRIX* src2)
{
    // dest = src2 * src1
    for (int r = 0; r < 4; ++r) {
        const float a0 = src2->m[r][0];
        const float a1 = src2->m[r][1];
        const float a2 = src2->m[r][2];
        const float a3 = src2->m[r][3];

        dest->m[r][0] = a0 * src1->m[0][0] + a1 * src1->m[1][0] + a2 * src1->m[2][0] + a3 * src1->m[3][0];
        dest->m[r][1] = a0 * src1->m[0][1] + a1 * src1->m[1][1] + a2 * src1->m[2][1] + a3 * src1->m[3][1];
        dest->m[r][2] = a0 * src1->m[0][2] + a1 * src1->m[1][2] + a2 * src1->m[2][2] + a3 * src1->m[3][2];
        dest->m[r][3] = a0 * src1->m[0][3] + a1 * src1->m[1][3] + a2 * src1->m[2][3] + a3 * src1->m[3][3];
    }
}

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bdSetViewport
	Purpose : set viewport
	Parameters : topLeftX, topLeftY, width, height
	Returns : 
	Info : 
*/
void bdSetViewport(const int topLeftX, const int topLeftY, const int width, const int height)
{
    int screenWidth = 0, screenHeight = 0;
    bdGetDisplayInfo(&screenWidth, &screenHeight, 0, 0);

    if (topLeftX >= screenWidth)  return;
    if (topLeftY >= screenHeight) return;

    const int right  = topLeftX + width;
    const int bottom = topLeftY + height;

    if (right  <= 0) return;
    if (bottom <= 0) return;

    // Fast path: fully inside screen bounds
    if (topLeftX >= 0 &&
        topLeftY >= 0 &&
        right  <= screenWidth &&
        bottom <= screenHeight)
    {
        D3DVIEWPORT8 vp;
        vp.X      = topLeftX;
        vp.Y      = topLeftY;
        vp.Width  = width;
        vp.Height = height;
        vp.MinZ   = 0.0f;
        vp.MaxZ   = 1.0f;

        bDisplayInfo.curRenderTarget->vpX      = topLeftX;
        bDisplayInfo.curRenderTarget->vpY      = topLeftY;
        bDisplayInfo.curRenderTarget->vpWidth  = width;
        bDisplayInfo.curRenderTarget->vpHeight = height;

        bDisplayInfo.d3dDevice->SetViewport(&vp);

        bViewInfo.width  = (float)width;
        bViewInfo.height = (float)height;

        // asm uses int division by 2 (SAR trick) then FILD -> float
        const int halfW_i = width  / 2;
        const int halfH_i = height / 2;

        bViewInfo.halfWidth  = (float)halfW_i;
        bViewInfo.halfHeight = (float)halfH_i;

        bViewInfo.xTopLeft = (float)topLeftX;
        bViewInfo.yTopLeft = (float)topLeftY;

        bViewInfo.xCentre = bViewInfo.xTopLeft + bViewInfo.halfWidth;
        bViewInfo.yCentre = bViewInfo.yTopLeft + bViewInfo.halfHeight;

        TBMatrix matScale;
        TBMatrix matTrans;

        bmMatScale(matScale, bViewInfo.halfWidth, -bViewInfo.halfHeight, 1.0f);
        bmMatTranslate(
            matTrans,
            bViewInfo.xTopLeft + bViewInfo.halfWidth,
            bViewInfo.yTopLeft + bViewInfo.halfHeight,
            0.0f
        );

        // multiplication is performed so src2 takes effect first
        bmMatMultiply(bViewInfo.viewportMatrix, matTrans, matScale);

        bdSetClipRectangle(topLeftX, topLeftY, width, height);
        return;
    }

    // Clipped path
    const int overflowLeft   = (topLeftX < 0) ? -topLeftX : 0;
    const int overflowRight  = (right  > screenWidth)  ? (right  - screenWidth)  : 0;
    const int overflowTop    = (topLeftY < 0) ? -topLeftY : 0;
    const int overflowBottom = (bottom > screenHeight) ? (bottom - screenHeight) : 0;

    D3DVIEWPORT8 vp;
    vp.X      = topLeftX + overflowLeft;
    vp.Y      = topLeftY + overflowTop;
    vp.Width  = width  - overflowLeft - overflowRight;
    vp.Height = height - overflowTop  - overflowBottom;
    vp.MinZ   = 0.0f;
    vp.MaxZ   = 1.0f;

    bDisplayInfo.curRenderTarget->vpX      = topLeftX;
    bDisplayInfo.curRenderTarget->vpY      = topLeftY;
    bDisplayInfo.curRenderTarget->vpWidth  = width;
    bDisplayInfo.curRenderTarget->vpHeight = height;

    bDisplayInfo.d3dDevice->SetViewport(&vp);

    bViewInfo.width      = (float)vp.Width;
    bViewInfo.height     = (float)vp.Height;
    bViewInfo.halfWidth  = bViewInfo.width  * 0.5f;
    bViewInfo.halfHeight = bViewInfo.height * 0.5f;

    bViewInfo.xTopLeft = (float)vp.X;
    bViewInfo.yTopLeft = (float)vp.Y;

    const float halfW = (float)width  * 0.5f;
    const float halfH = (float)height * 0.5f;

    bViewInfo.xCentre = (float)topLeftX + halfW;
    bViewInfo.yCentre = (float)topLeftY + halfH;

    TBMatrix vpScale;
    TBMatrix vpTrans;

    bmMatScale(vpScale, bViewInfo.halfWidth, -bViewInfo.halfHeight, 1.0f);
    bmMatTranslate(
        vpTrans,
        bViewInfo.xTopLeft + bViewInfo.halfWidth,
        bViewInfo.yTopLeft + bViewInfo.halfHeight,
        0.0f
    );
    bmMatMultiply(bViewInfo.viewportMatrix, vpTrans, vpScale);

    // store clipped rect into render target
    bDisplayInfo.curRenderTarget->clipXPos    = vp.X;
    bDisplayInfo.curRenderTarget->clipYPos    = vp.Y;
    bDisplayInfo.curRenderTarget->clipWidth   = vp.Width;
    bDisplayInfo.curRenderTarget->clipHeight  = vp.Height;

    const float clipCentreX =
        (float)bDisplayInfo.curRenderTarget->clipXPos +
        (float)bDisplayInfo.curRenderTarget->clipWidth * 0.5f;

    const float clipCentreY =
        (float)bDisplayInfo.curRenderTarget->clipYPos +
        (float)bDisplayInfo.curRenderTarget->clipHeight * 0.5f;

    TBMatrix clipTrans;
    TBMatrix clipScale;

    bmMatTranslate(
        clipTrans,
        (bViewInfo.xCentre - clipCentreX) / halfW,
        -((bViewInfo.yCentre - clipCentreY) / halfH),
        0.0f
    );

    bmMatScale(
        clipScale,
        (float)width  / bViewInfo.width,
        (float)height / bViewInfo.height,
        1.0f
    );

    // clipRectMatrix = translate * scale (per call order + "src2 takes effect first")
    bmMatMultiply(bViewInfo.clipRectMatrix, clipScale, clipTrans);

    bD3DMatMultiply(
        &bOrthoProjectionWithClip,
        (const D3DMATRIX*)bViewInfo.clipRectMatrix,
        &bOrthoProjectionMatrix
    );

    bCalcPerspProjectionMatrix();
    bUpdateViewFromCamera();

    TBMatrix viewTrans;
    bmMatTranslate(viewTrans, -bViewInfo.xPos, -bViewInfo.yPos, -bViewInfo.zPos);

    bD3DMatMultiply(
        &bViewMatrix,
        (const D3DMATRIX*)bViewInfo.rotMatrix,
        (const D3DMATRIX*)viewTrans
    );

    if (bViewInfo.projectionMode == BDISPLAY_PROJECTIONMODE3D)
    {
        bDisplayInfo.d3dDevice->SetTransform(D3DTS_VIEW, &bViewMatrix);
    }

    bmMatCopy(bViewInfo.worldToView, (const float*)&bViewMatrix);

    bmMatMultiply(bViewInfo.objectToView, bViewInfo.worldToView, bViewInfo.objectMatrix);

    {
        TBMatrix perspA;
        bmMatCopy(perspA, (const float*)&bPerspProjectionMatrix);

        bmMatMultiply(bViewInfo.objectToProjection, perspA, bViewInfo.objectToView);
        bmMatMultiply(bViewInfo.worldToProjection,  perspA, bViewInfo.worldToView);
        bmMatMultiply(bViewInfo.objectToScreen,     bViewInfo.viewportMatrix, bViewInfo.objectToProjection);
        bmMatMultiply(bViewInfo.viewportProjectionMatrix, bViewInfo.viewportMatrix, perspA);
    }

    // bdSetProjectionMode block
    if (bViewInfo.projectionMode == BDISPLAY_PROJECTIONMODE3D)
    {
        bDisplayInfo.d3dDevice->SetTransform(D3DTS_VIEW, &bViewMatrix);
        bDisplayInfo.d3dDevice->SetTransform(D3DTS_PROJECTION, &bPerspProjectionMatrix);
        bProjectionMatrix = &bPerspProjectionMatrix;
    }
    else
    {
        bDisplayInfo.d3dDevice->SetTransform(D3DTS_VIEW, &bIdentityD3DMatrix);
        bDisplayInfo.d3dDevice->SetTransform(D3DTS_PROJECTION, &bOrthoProjectionWithClip);
        bProjectionMatrix = &bOrthoProjectionWithClip;
    }

    bmMatMultiply(bViewInfo.objectToView, bViewInfo.worldToView, bViewInfo.objectMatrix);

    {
        TBMatrix perspB;
        bmMatCopy(perspB, (const float*)&bPerspProjectionMatrix);

        bmMatMultiply(bViewInfo.objectToProjection, perspB, bViewInfo.objectToView);
        bmMatMultiply(bViewInfo.worldToProjection,  perspB, bViewInfo.worldToView);
        bmMatMultiply(bViewInfo.objectToScreen,     bViewInfo.viewportMatrix, bViewInfo.objectToProjection);
        bmMatMultiply(bViewInfo.viewportProjectionMatrix, bViewInfo.viewportMatrix, perspB);
    }
}

/*	--------------------------------------------------------------------------------
	Function : bdSetViewPosition
	Purpose : set view position
	Parameters : xPos, yPos, zPos
	Returns : 
	Info : 
*/
void bdSetViewPosition(const float xPos, const float yPos, const float zPos)
{
	bViewInfo.xPos = xPos;
	bViewInfo.yPos = yPos;
	bViewInfo.zPos = zPos;
}

/*	--------------------------------------------------------------------------------
	Function : bdSetViewOrientation
	Purpose : set view orientation
	Parameters : xRot, yRot, zRot, order
	Returns : 
	Info : 
*/
void bdSetViewOrientation(const float xRot, const float yRot, const float zRot, const EBViewportRotationOrder order)
{
        bkPrintf("*** WARNING *** bdSetViewOrientation was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdSetViewOrientationFromQuaternion
	Purpose : set view orientation from quaternion
	Parameters : quat
	Returns : 
	Info : 
*/
void bdSetViewOrientationFromQuaternion(const TBQuaternion quat)
{
    bmQuatToMatrix(bViewInfo.rotMatrix, quat);
}

/*	--------------------------------------------------------------------------------
	Function : bdSetViewMatrix
	Purpose : set view matrix
	Parameters : m
	Returns : 
	Info : 
*/
void bdSetViewMatrix(const TBMatrix m)
{
        bkPrintf("*** WARNING *** bdSetViewMatrix was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdSetViewLookAt
	Purpose : set view look at
	Parameters : from, at, rollRadians
	Returns : 
	Info : 
*/
void bdSetViewLookAt(const TBVector from, const TBVector at, const float rollRadians)
{
        bkPrintf("*** WARNING *** bdSetViewLookAt was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdSetViewIdentity
	Purpose : set view identity
	Parameters : 
	Returns : 
	Info : 
*/
void bdSetViewIdentity()
{
        bkPrintf("*** WARNING *** bdSetViewIdentity was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdSetupView
	Purpose : setup view
	Parameters : 
	Returns : 
	Info : 
*/
void bdSetupView()
{
    // 1) Sync camera -> view globals
    bUpdateViewFromCamera();

    // 2) View matrix = R * T(-pos)
    TBMatrix tmpTranslate;
    bmMatTranslate(tmpTranslate, -bViewInfo.xPos, -bViewInfo.yPos, -bViewInfo.zPos);
    bD3DMatMultiply(&bViewMatrix, (D3DMATRIX*)bViewInfo.rotMatrix, (D3DMATRIX*)tmpTranslate);

    // 3) If 3D, push VIEW to device
    if (bViewInfo.projectionMode == BDISPLAY_PROJECTIONMODE3D)
        bDisplayInfo.d3dDevice->SetTransform(D3DTS_VIEW, &bViewMatrix);

    // 4) worldToView = viewMatrix
    bmMatCopy(bViewInfo.worldToView, (float*)&bViewMatrix);

    // 5) objectToView = worldToView * objectMatrix
    bmMatMultiply(bViewInfo.objectToView,
                         bViewInfo.worldToView,
                         bViewInfo.objectMatrix);

    // 6) Use the perspective projection
    TBMatrix tmpProj;
    bmMatCopy(tmpProj, bProjectionMatrix);

    // objectToProjection = proj * objectToView
    bmMatMultiply(bViewInfo.objectToProjection,
                         tmpProj,
                         bViewInfo.objectToView);

    // worldToProjection = proj * worldToView
    bmMatMultiply(bViewInfo.worldToProjection,
                         tmpProj,
                         bViewInfo.worldToView);

    // objectToScreen = viewportMatrix * objectToProjection
    bmMatMultiply(bViewInfo.objectToScreen,
                         bViewInfo.viewportMatrix,
                         bViewInfo.objectToProjection);

    // viewportProjectionMatrix = viewportMatrix * proj
    bmMatMultiply(bViewInfo.viewportProjectionMatrix,
                         bViewInfo.viewportMatrix,
                         tmpProj);
}

/*	--------------------------------------------------------------------------------
	Function : bdSetViewClipPlanes
	Purpose : set view clip planes
	Parameters : nearClip, farClip
	Returns : 
	Info : 
*/
void bdSetViewClipPlanes(const float nearClip, const float farClip)
{
    IDirect3DDevice8* dev;

    // Store near/far and rebuild perspective
    bViewInfo.nearClip = nearClip;
    bViewInfo.farClip  = farClip;
    bCalcPerspProjectionMatrix();

    // Set D3D transforms and select active projection pointer
    dev = bDisplayInfo.d3dDevice;
    if (bViewInfo.projectionMode == BDISPLAY_PROJECTIONMODE3D) {
        dev->SetTransform(D3DTS_VIEW,       &bViewMatrix);
        dev->SetTransform(D3DTS_PROJECTION, &bPerspProjectionMatrix);
        bProjectionMatrix = &bPerspProjectionMatrix;
    } else {
        dev->SetTransform(D3DTS_VIEW,       &bIdentityD3DMatrix);
        dev->SetTransform(D3DTS_PROJECTION, &bOrthoProjectionWithClip);
        bProjectionMatrix = &bOrthoProjectionWithClip;
    }

    // objectToView = worldToView * objectMatrix
    bmMatMultiply(bViewInfo.objectToView, bViewInfo.worldToView, bViewInfo.objectMatrix);

    TBMatrix tmpProj;
    bmMatCopy(tmpProj, bProjectionMatrix);

    // Derived composites use the temp perspective matrix
    bmMatMultiply(bViewInfo.objectToProjection, tmpProj, bViewInfo.objectToView);
    bmMatMultiply(bViewInfo.worldToProjection,  tmpProj, bViewInfo.worldToView);
    bmMatMultiply(bViewInfo.objectToScreen,     bViewInfo.viewportMatrix, bViewInfo.objectToProjection);
    bmMatMultiply(bViewInfo.viewportProjectionMatrix, bViewInfo.viewportMatrix, tmpProj);
}

/*	--------------------------------------------------------------------------------
	Function : bdSetObjectMatrixCallback
	Purpose : set object matrix callback
	Parameters : callback, context
	Returns : 
	Info : 
*/
void bdSetObjectMatrixCallback(TBObjectMatrixCallback callback, void *context)
{
        bkPrintf("*** WARNING *** bdSetObjectMatrixCallback was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdSetObjectMatrix
	Purpose : set object matrix
	Parameters : objMat
	Returns : 
	Info : 
*/
void bdSetObjectMatrix(const TBMatrix objMat)
{
    // Copy the object matrix into the view info
    bmMatCopy(bViewInfo.objectMatrix, objMat); 

    // Invoke callback if present: signature (TBMatrix objMat, void* context)
    if (bObjectMatrixCallback) {
        bObjectMatrixCallback(bViewInfo.objectMatrix, bObjectMatrixContext);
    }

    // Set the D3D world transform to the current object matrix
    bDisplayInfo.d3dDevice->SetTransform(D3DTS_WORLD,
            reinterpret_cast<const D3DMATRIX*>(bViewInfo.objectMatrix));

    // Rebuild cached transforms following the pipeline used in the disassembly
    bmMatMultiply(bViewInfo.objectToView,
                           bViewInfo.worldToView,
                           bViewInfo.objectMatrix);

    TBMatrix proj;
    bmMatCopy(proj, bProjectionMatrix);

    bmMatMultiply(bViewInfo.objectToProjection,
                           proj,
                           bViewInfo.objectToView);
    bmMatMultiply(bViewInfo.worldToProjection,
                           proj,
                           bViewInfo.worldToView);
    bmMatMultiply(bViewInfo.objectToScreen,
                           bViewInfo.viewportMatrix,
                           bViewInfo.objectToProjection);
    bmMatMultiply(bViewInfo.viewportProjectionMatrix,
                           bViewInfo.viewportMatrix,
                           proj);
}


/*	--------------------------------------------------------------------------------
	Function : bdSetViewLookPivot
	Purpose : set view look pivot
	Parameters : at, xRadians, yRadians, distance
	Returns : 
	Info : 
*/
void bdSetViewLookPivot(const TBVector at, const float xRadians, const float yRadians, const float distance)
{
        bkPrintf("*** WARNING *** bdSetViewLookPivot was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdSetProjectionMode
	Purpose : set projection mode
	Parameters : mode, force
	Returns : 
	Info : 
*/
void bdSetProjectionMode(const uint32 mode, int force)
{
    if (bViewInfo.projectionMode == mode && !force) // || is decompiler bug???
        return;

    bViewInfo.projectionMode = mode;

    // Set transforms and choose active projection matrix
    {
        IDirect3DDevice8* dev = bDisplayInfo.d3dDevice;
        if (mode == BDISPLAY_PROJECTIONMODE3D) {
            dev->SetTransform(D3DTS_VIEW,       &bViewMatrix);
            dev->SetTransform(D3DTS_PROJECTION, &bPerspProjectionMatrix);
            bProjectionMatrix = &bPerspProjectionMatrix;
        } else {
            dev->SetTransform(D3DTS_VIEW,       &bIdentityD3DMatrix);
            dev->SetTransform(D3DTS_PROJECTION, &bOrthoProjectionWithClip);
            bProjectionMatrix = &bOrthoProjectionWithClip;
        }
    }

    // objectToView = worldToView * objectMatrix
    bmMatMultiply(bViewInfo.objectToView, bViewInfo.worldToView, bViewInfo.objectMatrix);

    // Copy perspective matrix into a temporary TBMatrix (16 floats)
    TBMatrix tmpProj;
    bmMatCopy(tmpProj, bProjectionMatrix);

    // Derived composites
    bmMatMultiply(bViewInfo.objectToProjection, tmpProj, bViewInfo.objectToView);
    bmMatMultiply(bViewInfo.worldToProjection,  tmpProj, bViewInfo.worldToView);
    bmMatMultiply(bViewInfo.objectToScreen,     bViewInfo.viewportMatrix, bViewInfo.objectToProjection);
    bmMatMultiply(bViewInfo.viewportProjectionMatrix, bViewInfo.viewportMatrix, tmpProj);
}

/*	--------------------------------------------------------------------------------
	Function : bTransformVertices
	Purpose : transform vertices
	Parameters : dest, src, noofVerts
	Returns : 
	Info : 
*/
void bTransformVertices(struct _TBPrimVertex *dest, struct _TBPrimVertex *src, int noofVerts)
{
        bkPrintf("*** WARNING *** bTransformVertices was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bCalcOrthoProjectionMatrix
	Purpose : calculate ortho projection matrix
	Parameters : 
	Returns : 
	Info : 
*/
void bCalcOrthoProjectionMatrix()
{
    D3DMATRIX& M = bOrthoProjectionMatrix;
    memset(&M, 0, sizeof(M));

    const float sx = 1.0f / static_cast<float>(bDisplayInfo.xResHalf); // == 2.0f / width
    const float sy = 1.0f / static_cast<float>(bDisplayInfo.yResHalf); // == 2.0f / height

    M._11 = sx;
    M._22 = sy;
    M._33 = 1.0f;              // depth scale (orthographic)
    M._43 = bViewInfo.zBias;   // depth bias injected into projection
    M._44 = 1.0f;              // homogeneous w
}

/*	--------------------------------------------------------------------------------
	Function : bCalcPerspProjectionMatrix
	Purpose : calculate perspective projection matrix
	Parameters : 
	Returns : 
	Info : 
*/
void bCalcPerspProjectionMatrix()
{
    const float zn = bViewInfo.nearClip;
    const float zf = bViewInfo.farClip;

    // q = far / (far - near)
    const float q = zf / (zf - zn);

    // cot = 1 / tan(fov/2)
    const float cotX = 1.0f / (float)tan((double)(bViewInfo.xFov * 0.5f));
    const float cotY = 1.0f / (float)tan((double)(bViewInfo.yFov * 0.5f));

    D3DMATRIX P;
    memset(&P, 0, sizeof(P));

    P._11 = cotX;
    P._22 = cotY;

    // Depth terms
    P._33 = q;
    P._34 = 1.0f;
    P._43 = -(zn * q) + bViewInfo.zBias; // -near*q + zBias

    // Projection offsets
    P._41 = bViewInfo.projOffsetX;
    P._42 = bViewInfo.projOffsetY;

    bD3DMatMultiply(
        &bPerspProjectionMatrix,
        reinterpret_cast<const D3DMATRIX*>(&bViewInfo.clipRectMatrix),
        &P
    );
}

/*	--------------------------------------------------------------------------------
	Function : bdProjectClipVertices
	Purpose : project clip vertices
	Parameters : dest, src, noofVerts, andCode, orCode
	Returns : 
	Info : 
*/
void bdProjectClipVertices(TBVector dest, const TBVector src, int noofVerts, int * const andCode, int * const orCode)
{
        bkPrintf("*** WARNING *** bdProjectClipVertices was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdProjectVertices
	Purpose : project vertices
	Parameters : dest, src, noofVerts
	Returns : 
	Info : 
*/
void bdProjectVertices(TBVector dest, const TBVector src, int noofVerts)
{
    if (noofVerts == 0) return;

    do
    {
        bmMatMultiplyVector2(dest, bViewInfo.objectToScreen, src);

        float w = dest[3];
        float invW = (w == 0.0f) ? 1.0f : (1.0f / w);
        dest[3] = invW;

        float x = dest[0] * invW;
        float y = dest[1] * invW;

        dest[0] = x - bViewInfo.xCentre;
        dest[1] = bViewInfo.yCentre - y;
        dest[2] = dest[2] * invW;

        src  += 4;
        dest += 4;
        --noofVerts;
    } while (noofVerts != 0);
}

/*	--------------------------------------------------------------------------------
	Function : bdXFormWorldToView
	Purpose : transform world to view
	Parameters : dest, src, noofVerts
	Returns : 
	Info : 
*/
void bdXFormWorldToView(TBVector dest, const TBVector src, int noofVerts)
{
        bkPrintf("*** WARNING *** bdXFormWorldToView was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdSetFOV
	Purpose : set field of view
	Parameters : horzFOVRadians, vertFOVRadians
	Returns : 
	Info : 
*/
void bdSetFOV(const float horzFOVRadians, const float vertFOVRadians)
{
    // Store horizontal FOV
    bViewInfo.xFov = horzFOVRadians;

    // Decide vertical FOV without writing to the const parameter
    float yFov = vertFOVRadians;
    if (yFov == 0.0f)
    {
        const double t = tan(horzFOVRadians * 0.5);
        const double aspect = (double)bViewInfo.height / (double)bViewInfo.width;
        yFov = (float)(2.0 * atan(aspect * t));
    }
    bViewInfo.yFov = yFov;

    // Rebuild perspective projection (used for both modes below)
    bCalcPerspProjectionMatrix();

    // Set D3D transforms + select active projection matrix pointer
    IDirect3DDevice8* dev = bDisplayInfo.d3dDevice;
    if (bViewInfo.projectionMode == BDISPLAY_PROJECTIONMODE3D)
    {
        dev->SetTransform(D3DTS_VIEW,       &bViewMatrix);
        dev->SetTransform(D3DTS_PROJECTION, &bPerspProjectionMatrix);
        bProjectionMatrix = &bPerspProjectionMatrix;
    }
    else
    {
        dev->SetTransform(D3DTS_VIEW,       &bIdentityD3DMatrix);
        dev->SetTransform(D3DTS_PROJECTION, &bOrthoProjectionWithClip);
        bProjectionMatrix = &bOrthoProjectionWithClip;
    }

    // objectToView = worldToView * objectMatrix
    bmMatMultiply(bViewInfo.objectToView, bViewInfo.worldToView, bViewInfo.objectMatrix);

    // Copy perspective matrix into a temporary TBMatrix (16 floats)
    TBMatrix tmpProj;
	bmMatCopy(tmpProj, bProjectionMatrix);

    // Derived composites
    bmMatMultiply(bViewInfo.objectToProjection, tmpProj, bViewInfo.objectToView);
    bmMatMultiply(bViewInfo.worldToProjection,  tmpProj, bViewInfo.worldToView);
    bmMatMultiply(bViewInfo.objectToScreen,     bViewInfo.viewportMatrix, bViewInfo.objectToProjection);
    bmMatMultiply(bViewInfo.viewportProjectionMatrix, bViewInfo.viewportMatrix, tmpProj);
}

/*	--------------------------------------------------------------------------------
	Function : bdTestBBVisibility
	Purpose : test bounding box visibility
	Parameters : bbMatrix, xMin, xMax, yMin, yMax, zMin, zMax
	Returns : visibility
	Info : 
*/
int bdTestBBVisibility(const TBMatrix bbMatrix, const float xMin, const float xMax, const float yMin, const float yMax, const float zMin, const float zMax)
{
    TBMatrix clipMatrix;
    TBVector corner;
    TBVector points[8];

    // worldToProjection * bbMatrix
    bmMatMultiply(clipMatrix, bViewInfo.worldToProjection, bbMatrix);

    // (xMin, yMin, zMin)
    corner[0] = xMin;
    corner[1] = yMin;
    corner[2] = zMin;
    corner[3] = 1.0f;
    bmMatMultiplyVector2(points[0], clipMatrix, corner);

    // (xMin, yMin, zMax)
    corner[2] = zMax;
    bmMatMultiplyVector2(points[1], clipMatrix, corner);

    // (xMin, yMax, zMax)
    corner[1] = yMax;
    bmMatMultiplyVector2(points[2], clipMatrix, corner);

    // (xMin, yMax, zMin)
    corner[2] = zMin;
    bmMatMultiplyVector2(points[3], clipMatrix, corner);

    // (xMax, yMin, zMin)
    corner[0] = xMax;
    corner[1] = yMin;
    corner[2] = zMin;
    bmMatMultiplyVector2(points[4], clipMatrix, corner);

    // (xMax, yMin, zMax)
    corner[2] = zMax;
    bmMatMultiplyVector2(points[5], clipMatrix, corner);

    // (xMax, yMax, zMax)
    corner[1] = yMax;
    bmMatMultiplyVector2(points[6], clipMatrix, corner);

    // (xMax, yMax, zMin)
    corner[2] = zMin;
    bmMatMultiplyVector2(points[7], clipMatrix, corner);

    uchar andMask = 0x3f;
    uchar orMask  = 0;

    for (int i = 0; i < 8; ++i)
    {
        const float x = points[i][0];
        const float y = points[i][1];
        const float z = points[i][2];
        const float w = points[i][3];

        uchar code = 0;

        if (x < -w)  code |= 0x01; // left
        if (x >  w)  code |= 0x02; // right
        if (y < -w)  code |= 0x04; // bottom
        if (y >  w)  code |= 0x08; // top
        if (z <  0.0f) code |= 0x10; // near
        if (z >  w)  code |= 0x20; // far

        andMask &= code;
        orMask  |= code;
    }

    int result = 0;
    if (andMask == 0)
    {
        result = (orMask != 0) + 1;
    }

    return result;
}

/*	--------------------------------------------------------------------------------
	Function : bdTestSphereVisibility
	Purpose : test sphere visibility
	Parameters : centre, radius
	Returns : visibility
	Info : 
*/
int bdTestSphereVisibility(TBVector centre, const float radius)
{
        bkPrintf("*** WARNING *** bdTestSphereVisibility was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bdWorldToScreenSizeXY
	Purpose : world to screen size XY
	Parameters : basisPoint, xWorldLen, yWorldLen, xScreenLen, yScreenLen
	Returns : scale
	Info : 
*/
float bdWorldToScreenSizeXY(const TBVector basisPoint, const float xWorldLen, const float yWorldLen, float * const xScreenLen, float * const yScreenLen)
{
    float num =
        (bViewInfo.objectToView[0][2]  * basisPoint[0]) +
        (bViewInfo.objectToView[1][2]  * basisPoint[1]) +
        (bViewInfo.objectToView[2][2] * basisPoint[2]) +
        (bViewInfo.objectToView[3][2] * basisPoint[3]);

    float den =
        (bViewInfo.objectToView[0][3]  * basisPoint[0]) +
        (bViewInfo.objectToView[1][3]  * basisPoint[1]) +
        (bViewInfo.objectToView[2][3] * basisPoint[2]) +
        (bViewInfo.objectToView[3][3] * basisPoint[3]);

    float scale = num / den;

    if ((scale > bViewInfo.nearClip) && (scale < bViewInfo.farClip))
    {
        const float sx = (bPerspProjectionMatrix._11 * xWorldLen) / scale;
        const float sy = (bPerspProjectionMatrix._22 * yWorldLen) / scale;

        *xScreenLen = sx * (float)bDisplayInfo.curRenderTarget->clipWidth  * 0.5;
        *yScreenLen = sy * (float)bDisplayInfo.curRenderTarget->clipHeight * 0.5;

        return scale;
    }

    return 0.0f;
}

/*	--------------------------------------------------------------------------------
	Function : bdWorldToScreenSizeX
	Purpose : world to screen size X
	Parameters : basisPoint, xWorldLen, xScreenLen
	Returns : scale
	Info : 
*/
float bdWorldToScreenSizeX(const TBVector basisPoint, const float xWorldLen, float * const xScreenLen)
{
    float num =
        (bViewInfo.objectToView[0][2] * basisPoint[0]) +
        (bViewInfo.objectToView[1][2] * basisPoint[1]) +
        (bViewInfo.objectToView[2][2] * basisPoint[2]) +
        (bViewInfo.objectToView[3][2] * basisPoint[3]);

    float den =
        (bViewInfo.objectToView[0][3] * basisPoint[0]) +
        (bViewInfo.objectToView[1][3] * basisPoint[1]) +
        (bViewInfo.objectToView[2][3] * basisPoint[2]) +
        (bViewInfo.objectToView[3][3] * basisPoint[3]);

    float scale = num / den;

    if ((scale > bViewInfo.nearClip) && (scale < bViewInfo.farClip))
    {
        const float sx = (bPerspProjectionMatrix._11 * xWorldLen) / scale;

        *xScreenLen = sx * (float)bDisplayInfo.curRenderTarget->clipWidth * 0.5f;

        return scale;
    }

    return 0.0f;
}

/*	--------------------------------------------------------------------------------
	Function : bdWorldToScreenSizeY
	Purpose : world to screen size Y
	Parameters : basisPoint, yWorldLen, yScreenLen
	Returns : scale
	Info : 
*/
float bdWorldToScreenSizeY(const TBVector basisPoint, const float yWorldLen, float * const yScreenLen)
{
        bkPrintf("*** WARNING *** bdWorldToScreenSizeY was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0.0f;
}

/*	--------------------------------------------------------------------------------
	Function : bdSetZBias
	Purpose : set Z bias
	Parameters : zBias
	Returns : 
	Info : 
*/
void bdSetZBias(const float zBias)
{
    float delta = zBias - bViewInfo.zBias;
    bViewInfo.zBias = zBias;

    bOrthoProjectionWithClip._43 += delta;
    bPerspProjectionMatrix._43   += delta;

    IDirect3DDevice8* dev = bDisplayInfo.d3dDevice;

    if (bViewInfo.projectionMode == 1) {
        dev->SetTransform(D3DTS_VIEW,       &bViewMatrix);
        dev->SetTransform(D3DTS_PROJECTION, &bPerspProjectionMatrix);
        bProjectionMatrix = &bPerspProjectionMatrix;
    } else {
        dev->SetTransform(D3DTS_VIEW,       &bIdentityD3DMatrix);
        dev->SetTransform(D3DTS_PROJECTION, &bOrthoProjectionWithClip);
        bProjectionMatrix = &bOrthoProjectionWithClip;
    }

    bmMatMultiply(bViewInfo.objectToView, bViewInfo.worldToView, bViewInfo.objectMatrix);

    float projTmp[4][4];
    bmMatCopy(projTmp, bProjectionMatrix);

    bmMatMultiply(bViewInfo.objectToProjection, projTmp, bViewInfo.objectToView);
    bmMatMultiply(bViewInfo.worldToProjection,  projTmp, bViewInfo.worldToView);
    bmMatMultiply(bViewInfo.objectToScreen, bViewInfo.viewportMatrix, bViewInfo.objectToProjection);
    bmMatMultiply(bViewInfo.viewportProjectionMatrix, bViewInfo.viewportMatrix, projTmp);
}

/*	--------------------------------------------------------------------------------
	Function : bdScreenToWorldPoint
	Purpose : screen to world point
	Parameters : xScreen, yScreen, worldPos
	Returns : 
	Info : 
*/
void bdScreenToWorldPoint(float xScreen, float yScreen, TBVector worldPos)
{
        bkPrintf("*** WARNING *** bdScreenToWorldPoint was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdScreenToWorldPointOffset
	Purpose : screen to world point offset
	Parameters : xScreen, yScreen, zOffset, position
	Returns : 
	Info : 
*/
void bdScreenToWorldPointOffset(float xScreen, float yScreen, float zOffset, TBVector position)
{
        bkPrintf("*** WARNING *** bdScreenToWorldPointOffset was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdGetObjectMatrix
	Purpose : get object matrix
	Parameters : dest
	Returns : 
	Info : 
*/
void bdGetObjectMatrix(TBMatrix dest)
{
    bmMatCopy(dest, bViewInfo.objectMatrix);
}

/*	--------------------------------------------------------------------------------
	Function : bdSetClipRectangle
	Purpose : set clip rectangle
	Parameters : topLeftX, topLeftY, width, height
	Returns : 
	Info : 
*/
void bdSetClipRectangle(int topLeftX, int topLeftY, int width, int height)
{
    // Clamp to current viewport bounds
    int vx_i = (int)bViewInfo.xTopLeft;
    int vy_i = (int)bViewInfo.yTopLeft;
    int rx_i = (int)topLeftX;
    int ry_i = (int)topLeftY;
    int rw_i = (int)width;
    int rh_i = (int)height;

    if (rx_i < vx_i) { rw_i += (rx_i - vx_i); rx_i = vx_i; }
    if (ry_i < vy_i) { rh_i += (ry_i - vy_i); ry_i = vy_i; }

    {
        int maxX_i = (int)(bViewInfo.xTopLeft + bViewInfo.width);
        int maxY_i = (int)(bViewInfo.yTopLeft + bViewInfo.height);
        if (rx_i + rw_i > maxX_i) rw_i = maxX_i - rx_i;
        if (ry_i + rh_i > maxY_i) rh_i = maxY_i - ry_i;
    }

    if (rw_i <= 0 || rh_i <= 0)
        return;

    // Cache clip in current render target (ints)
    if (bDisplayInfo.curRenderTarget) {
        TBRenderTarget* rt = bDisplayInfo.curRenderTarget;
        rt->clipXPos   = rx_i;
        rt->clipYPos   = ry_i;
        rt->clipWidth  = rw_i;
        rt->clipHeight = rh_i;
    }

    // Build clipRectMatrix: identity fast path or S * T
    if ((float)rw_i == bViewInfo.width && (float)rh_i == bViewInfo.height) {
        // memcpy 16 floats
        float* dst = (float*)bViewInfo.clipRectMatrix;
        float* src = (float*)bIdentityMatrix;
        int i;
        for (i = 0; i < 16; ++i) dst[i] = src[i];
    } else {
        // clip center (pixels)
        float cx = (float)rx_i + (float)rw_i * 0.5f;
        float cy = (float)ry_i + (float)rh_i * 0.5f;

        // normalized shift from viewport center; note flipped
        float dx = (bViewInfo.xCentre - cx) / bViewInfo.halfWidth;
        float dy = -((bViewInfo.yCentre - cy) / bViewInfo.halfHeight);

        TBMatrix T, S;
        bmMatTranslate(T, dx, dy, 0.0f);
        bmMatScale(S, bViewInfo.width  / (float)rw_i,
                      bViewInfo.height / (float)rh_i,
                      1.0f);

        // clipRectMatrix = S * T
        bmMatMultiply(bViewInfo.clipRectMatrix, S, T);
    }

    // Build viewportMatrix from CLIP dimensions: (T * S)
    {
        TBMatrix Svp, Tvp;
        float sx = (float)rw_i * 0.5f;
        float sy = -((float)rh_i * 0.5f); /* flip Y */
        bmMatScale(Svp, sx, sy, 1.0f);

        float tx = (float)rx_i + (float)rw_i * 0.5f;
        float ty = (float)ry_i + (float)rh_i * 0.5f;
        bmMatTranslate(Tvp, tx, ty, 0.0f);

        bmMatMultiply(bViewInfo.viewportMatrix, Tvp, Svp);
    }

    // Submit D3D viewport
    {
        if (bDisplayInfo.d3dDevice) {
            D3DVIEWPORT8 vp;
            vp.X      = (DWORD)rx_i;
            vp.Y      = (DWORD)ry_i;
            vp.Width  = (DWORD)rw_i;
            vp.Height = (DWORD)rh_i;
            vp.MinZ   = 0.0f;
            vp.MaxZ   = 1.0f;
            bDisplayInfo.d3dDevice->SetViewport(&vp);
        }
    }

    // Ortho with clip: dest = left * right  => Ortho * Clip
    bD3DMatMultiply(&bOrthoProjectionWithClip,
                    (D3DMATRIX*)bViewInfo.clipRectMatrix,   /* right  */
                    &bOrthoProjectionMatrix);               /* left   */

    // Rebuild perspective with clip and set transforms/mode (first inline)
    bCalcPerspProjectionMatrix();

    if (bDisplayInfo.d3dDevice)
    {
        if (bViewInfo.projectionMode == BDISPLAY_PROJECTIONMODE3D)
        {
            bDisplayInfo.d3dDevice->SetTransform(D3DTS_VIEW,       &bViewMatrix);
            bDisplayInfo.d3dDevice->SetTransform(D3DTS_PROJECTION, &bPerspProjectionMatrix);
            bProjectionMatrix = &bPerspProjectionMatrix;
        }
        else // 2D Mode
        {
            bDisplayInfo.d3dDevice->SetTransform(D3DTS_VIEW,       &bIdentityD3DMatrix);
            bDisplayInfo.d3dDevice->SetTransform(D3DTS_PROJECTION, &bOrthoProjectionWithClip);
            bProjectionMatrix = &bOrthoProjectionWithClip;
        }
    }

    // PASS 1: derived matrices (use *bProjectionMatrix)
    bmMatMultiply(bViewInfo.objectToView, bViewInfo.worldToView, bViewInfo.objectMatrix);

    {
        TBMatrix tmpProj;
        bmMatCopy(tmpProj, bProjectionMatrix);

        bmMatMultiply(bViewInfo.objectToProjection, tmpProj, bViewInfo.objectToView);
        bmMatMultiply(bViewInfo.worldToProjection,  tmpProj, bViewInfo.worldToView);
        bmMatMultiply(bViewInfo.objectToScreen,     bViewInfo.viewportMatrix, bViewInfo.objectToProjection);
        bmMatMultiply(bViewInfo.viewportProjectionMatrix, bViewInfo.viewportMatrix, tmpProj);
    }

    // Update view from camera and rebuild view matrix: view = R * T(-pos)
    bUpdateViewFromCamera();
    {
        TBMatrix Tneg;
        bmMatTranslate(Tneg, -bViewInfo.xPos, -bViewInfo.yPos, -bViewInfo.zPos);

        // bD3DMatMultiply: dest = left * right; left = R, right = T(-pos)
        bD3DMatMultiply(&bViewMatrix,
                        (D3DMATRIX*)Tneg,                 /* right */
                        (D3DMATRIX*)bViewInfo.rotMatrix); /* left  */
    }

    // Second inline: only update VIEW when in 3D mode
    if (bViewInfo.projectionMode == BDISPLAY_PROJECTIONMODE3D) {
        if (bDisplayInfo.d3dDevice) {
            bDisplayInfo.d3dDevice->SetTransform(D3DTS_VIEW, &bViewMatrix);
        }
    }

    // worldToView = viewMatrix; PASS 2
    bmMatCopy(bViewInfo.worldToView, &bViewMatrix);

    bmMatMultiply(bViewInfo.objectToView, bViewInfo.worldToView, bViewInfo.objectMatrix);

    {
        TBMatrix tmpProj2;
        bmMatCopy(tmpProj2, bProjectionMatrix);

        bmMatMultiply(bViewInfo.objectToProjection, tmpProj2, bViewInfo.objectToView);
        bmMatMultiply(bViewInfo.worldToProjection,  tmpProj2, bViewInfo.worldToView);
        bmMatMultiply(bViewInfo.objectToScreen,     bViewInfo.viewportMatrix, bViewInfo.objectToProjection);
        bmMatMultiply(bViewInfo.viewportProjectionMatrix, bViewInfo.viewportMatrix, tmpProj2);
    }
}

/*	--------------------------------------------------------------------------------
	Function : bdCalcVerticalFOV
	Purpose : calculate vertical FOV
	Parameters : hFov, vpWidth, vpHeight
	Returns : vertical FOV
	Info : 
*/
float bdCalcVerticalFOV(float hFov, float vpWidth, float vpHeight)
{
        bkPrintf("*** WARNING *** bdCalcVerticalFOV was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0.0f;
}

/*	--------------------------------------------------------------------------------
	Function : bdSetIdentityObjectMatrix
	Purpose : set identity object matrix
	Parameters : 
	Returns : 
	Info : 
*/
void bdSetIdentityObjectMatrix()
{
    // WORLD = Identity
    IDirect3DDevice8* dev = bDisplayInfo.d3dDevice;
    if (dev) {
        dev->SetTransform(D3DTS_WORLD, (const D3DMATRIX*)bIdentityMatrix);
    }

    // objectMatrix = Identity
	bmMatCopy(bViewInfo.objectMatrix, bIdentityMatrix);

    // objectToView = worldToView * objectMatrix
    bmMatMultiply(bViewInfo.objectToView,
                         bViewInfo.worldToView,
                         bViewInfo.objectMatrix);

    // tmpProj = *bProjectionMatrix (current active projection)
    TBMatrix tmpProj;
    bmMatCopy(tmpProj, bProjectionMatrix);

    // objectToProjection = tmpProj * objectToView
    bmMatMultiply(bViewInfo.objectToProjection,
                         tmpProj,
                         bViewInfo.objectToView);

    // worldToProjection = tmpProj * worldToView
    bmMatMultiply(bViewInfo.worldToProjection,
                         tmpProj,
                         bViewInfo.worldToView);

    // objectToScreen = viewportMatrix * objectToProjection
    bmMatMultiply(bViewInfo.objectToScreen,
                         bViewInfo.viewportMatrix,
                         bViewInfo.objectToProjection);

    // viewportProjectionMatrix = viewportMatrix * tmpProj
    bmMatMultiply(bViewInfo.viewportProjectionMatrix,
                         bViewInfo.viewportMatrix,
                         tmpProj);
}
