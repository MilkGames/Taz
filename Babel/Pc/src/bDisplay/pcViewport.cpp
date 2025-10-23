// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcViewport.cpp
//   Purpose : Defines enums for viewport rotation order and projection modes. Defines clip codes. Defines _TBViewInfo struct and TBObjectMatrixCallback function pointer
// Component : Babel Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Globals

TBViewInfo bViewInfo;
int        bObjectMatrixIsIdentity;

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
        bkPrintf("*** WARNING *** bdSetViewport was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
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
        bkPrintf("*** WARNING *** bdSetViewOrientationFromQuaternion was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
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
        bkPrintf("*** WARNING *** bdSetupView was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
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
        bkPrintf("*** WARNING *** bdSetViewClipPlanes was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
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
        bkPrintf("*** WARNING *** bdSetObjectMatrix was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
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
        bkPrintf("*** WARNING *** bdSetProjectionMode was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
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
        bkPrintf("*** WARNING *** bCalcOrthoProjectionMatrix was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
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
        bkPrintf("*** WARNING *** bCalcPerspProjectionMatrix was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
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
        bkPrintf("*** WARNING *** bdProjectVertices was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
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
        bkPrintf("*** WARNING *** bdSetFOV was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
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
        bkPrintf("*** WARNING *** bdTestBBVisibility was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
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
        bkPrintf("*** WARNING *** bdWorldToScreenSizeXY was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bdWorldToScreenSizeX was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bdSetZBias was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
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
        bkPrintf("*** WARNING *** bdGetObjectMatrix was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
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
        bkPrintf("*** WARNING *** bdSetClipRectangle was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
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
        bkPrintf("*** WARNING *** bdSetIdentityObjectMatrix was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}
