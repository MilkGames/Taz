// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : xbViewport.h
//   Purpose : viewport management functions
// Component : Xbox Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BDISPLAY_XBVIEWPORT_H__
#define __BDISPLAY_XBVIEWPORT_H__

// ********************************************************************************
// Constants and Macros

// orders of rotation for viewports
typedef enum EBViewportRotationOrder {
	BVPROTATE_XYZ,
	BVPROTATE_XZY,
	BVPROTATE_YXZ,
	BVPROTATE_YZX,
	BVPROTATE_ZXY,
	BVPROTATE_ZYX,
};

// projection modes
#define BDISPLAY_PROJECTIONMODE2D		0
#define BDISPLAY_PROJECTIONMODE3D		1
#define BDISPLAY_PROJECTIONMODESPRITE	BDISPLAY_PROJECTIONMODE3D


// clip codes
#define BCLIPCODE_XMIN		0x00000001
#define BCLIPCODE_XMAX		0x00000002
#define BCLIPCODE_YMIN		0x00000004
#define BCLIPCODE_YMAX		0x00000008
#define BCLIPCODE_ZMIN		0x00000010
#define BCLIPCODE_ZMAX		0x00000020
#define BCLIPCODE_ALL		0x0000003f


// ********************************************************************************
// Types, Structures and Classes

// view information container
typedef struct _TBViewInfo {
	TBMatrix				rotMatrix;				// orientation matrix
	TBMatrix				worldToView;			// world to view matrix
	TBMatrix				worldToProjection;		// world to projection matrix (*** NOT ALWAYS VALID ***)
	TBMatrix				objectMatrix;			// current object matrix (objectToWorld)
	TBMatrix				objectToView;			// object->view matrix
	TBMatrix				objectToProjection;		// object->projection matrix
	TBMatrix				objectToScreen;			// object->screen matrix (*** NOT ALWAYS VALID! ***)
	TBMatrix				viewportMatrix;			// viewport scale/translation matrix
	TBMatrix				clipMatrix;				// matrix to convert projected verts into clip space

	float					xPos, yPos, zPos;		// view position
	int						projectionMode;			// current projection mode (see BDISPLAY_PROJECTIONMODE)
	float					nearClip, farClip;		// clipping distances
	float					xFov, yFov;				// field of view values
	float					width,height;			// viewport dimensions
	float					halfWidth, halfHeight;	// half viewport dimensions
	float					xCentre, yCentre;		// centre position
	float					xTopLeft, yTopLeft;		// top-left corner
	float					zBias;					// offset on Z for polygon projection.
	float					clipXPos, clipYPos;		// clip rectangle position
	float					clipWidth, clipHeight;	// clip rectangle width and height
	int						objectToScreenValid;	// TRUE if objectToScreen matrix is valid
	int						worldToProjectionValid;	// TRUE if worldToProjection matrix is valid
} TBViewInfo;

// callback when the object matrix is updated
typedef void (*TBObjectMatrixCallback)(TBMatrix objMat, void *context);


// ********************************************************************************
// Globals

extern TBViewInfo		bViewInfo;					// view information container
extern D3DMATRIX		bViewMatrix;				// the current view matrix
extern D3DMATRIX		*bProjectionMatrix;			// current projection matrix
extern int				bObjectMatrixIsIdentity;	// TRUE if the object matrix is identity


// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bdSetViewport
   Purpose : set the viewport parameters
   Parameters : top-left (x,y) relative to centre, width, height
   Returns : 
   Info : 
*/

void bdSetViewport(const int topLeftX, const int topLeftY, const int width, const int height);


/* --------------------------------------------------------------------------------
   Function : bdSetViewPosition
   Purpose : set the position of the camera
   Parameters : camera (x,y,z)
   Returns : 
   Info : 
*/

void bdSetViewPosition(const float xPos, const float yPos, const float zPos);


/* --------------------------------------------------------------------------------
   Function : bdSetViewOrientation
   Purpose : set the orientation of the camera
   Parameters : camera (x,y,z) orientation, order to apply them in
   Returns : 
   Info : 
*/

void bdSetViewOrientation(const float xRot, const float yRot, const float zRot, const EBViewportRotationOrder order);


/* --------------------------------------------------------------------------------
   Function : bdSetViewOrientationFromQuaternion
   Purpose : set the orientation of the camera from a quaternion
   Parameters : quaternion representing desired camera orientation
   Returns : 
   Info : 
*/

void bdSetViewOrientationFromQuaternion(const TBQuaternion quat);


/* --------------------------------------------------------------------------------
   Function : bdSetViewMatrix
   Purpose : set the camera matrix
   Parameters : matrix representing desired camera orientation
   Returns : 
   Info : 
*/

void bdSetViewMatrix(const TBMatrix m);


/* --------------------------------------------------------------------------------
   Function : bdSetViewLookAt
   Purpose : set the position and orientation of the camera to look at a point
   Parameters : from (x,y,z), at (x,y,z), roll in radians
   Returns : 
   Info : 
*/

void bdSetViewLookAt(const  TBVector from, const  TBVector at, const float rollRadians);


/* --------------------------------------------------------------------------------
   Function : bdSetViewIdentity
   Purpose : set the view matrix to be identity
   Parameters : 
   Returns : 
   Info : 
*/

void bdSetViewIdentity();


/* --------------------------------------------------------------------------------
   Function : bdSetupView
   Purpose : prepare the view matrix
   Parameters : 
   Returns : 
   Info : 
*/

void bdSetupView();


/* --------------------------------------------------------------------------------
   Function : bdSetViewClipPlanes
   Purpose : set the view near and far clip-planes
   Parameters : near clip distance, far clip distance
   Returns : 
   Info : 
*/

void bdSetViewClipPlanes(const float nearClip, const float farClip);


/* --------------------------------------------------------------------------------
   Function : bdSetObjectMatrixCallback
   Purpose : set the user callback to be called when the object matrix is updated
   Parameters : callback, context;
   Returns : 
   Info : 
*/

void bdSetObjectMatrixCallback(TBObjectMatrixCallback callback, void *context);


/* --------------------------------------------------------------------------------
   Function : bdSetObjectMatrix
   Purpose : set the object matrix for rendering
   Parameters : matrix to set
   Returns : 
   Info : 
*/

void bdSetObjectMatrix(const TBMatrix objMat);


/* --------------------------------------------------------------------------------
   Function : bdSetViewLookPivot
   Purpose : set the position and orientation of the camera to look at a point from a distance
   Parameters : at (x,y,z), X rotation, Y rotation, distance
   Returns : 
   Info : 
*/

void bdSetViewLookPivot(const TBVector at, const float xRadians, const float yRadians, const float distance);


/* --------------------------------------------------------------------------------
   Function : bdSetProjectionMode
   Purpose : set the projection mode to either 2d or 3d
   Parameters : mode value, force it to be updated
   Returns : 
   Info : 
*/

void bdSetProjectionMode(const uint32 mode, int force=0);


/* --------------------------------------------------------------------------------
   Function : bTransformVertices
   Purpose : transform an array of vertices by the current matrix
   Parameters : dest buffer, source buffer, #vertices
   Returns : 
   Info : 
*/

void bTransformVertices(struct _TBPrimVertex *dest, struct _TBPrimVertex *src, int noofVerts);


/* --------------------------------------------------------------------------------
   Function : bCalcOrthoProjectionMatrix
   Purpose : calculate the orthographic projection matrix
   Parameters : 
   Returns : 
   Info : 
*/

void bCalcOrthoProjectionMatrix();


/* --------------------------------------------------------------------------------
   Function : bCalcPerspProjectionMatrix
   Purpose : calculate the perspective projection matrix
   Parameters : 
   Returns : 
   Info : the perspective projection depends on farClip, nearClip, xFov and yFov only - not screen res
*/

void bCalcPerspProjectionMatrix();


/* --------------------------------------------------------------------------------
   Function : bdProjectClipVertices
   Purpose : transform vertices and set clip flags
   Parameters : dest buffer, source buffer, #vertices, ptr for AND code, ptr for OR code
   Returns : 
   Info : 
*/

void bdProjectClipVertices(TBVector dest, const TBVector src, int noofVerts, int * const andCode, int * const orCode);


/* --------------------------------------------------------------------------------
   Function : bdProjectVertices
   Purpose : transform vertices
   Parameters : dest buffer, source buffer, #vertices
   Returns : 
   Info : 
*/

void bdProjectVertices(TBVector dest, const TBVector src, int noofVerts);


/* --------------------------------------------------------------------------------
   Function : bdXFormWorldToView
   Purpose : transform vertices from world to view space
   Parameters : dest buffer, source buffer, #vertices
   Returns : 
   Info : 
*/

void bdXFormWorldToView(TBVector dest, const TBVector src, int noofVerts);


/* --------------------------------------------------------------------------------
   Function : bdSetFOV
   Purpose : set the current field of view
   Parameters : horizontal FOV in radians, vertical FOV in radians
   Returns : 
   Info : both FOV values are (PI > fov > 0), set vertical FOV to zero to calculate automatically
*/

void bdSetFOV(const float horzFOVRadians, const float vertFOVRadians = 0.0f);


/* --------------------------------------------------------------------------------
   Function : bdTestBBVisibility
   Purpose : test the visibility of a bounding box
   Parameters : bounding box->world matrix, box limits
   Returns : 0=not visible, 1=entirely visible, 2=partly visible
   Info : 
*/

int bdTestBBVisibility(const TBMatrix bbMatrix, const float xMin, const float xMax, const float yMin, const float yMax, const float zMin, const float zMax);


/* --------------------------------------------------------------------------------
   Function : bdTestSphereVisibility
   Purpose : test the visibility of a bounding sphere
   Parameters : sphere centre in world space, radius in world space
   Returns : 0=not visible, 1=at least partly visible
   Info : 
*/

int bdTestSphereVisibility(TBVector centre, const float radius);


/* --------------------------------------------------------------------------------
   Function : bdWorldToScreenSizeXY
   Purpose : convert a length in world space into screen pixels
   Parameters : basis point, world X length, world Y length, ptr for screen X length, ptr for screen Y length
   Returns : z distance or zero for clipped by near/far
   Info : 
*/

float bdWorldToScreenSizeXY(const TBVector basisPoint, const float xWorldLen, const float yWorldLen, float * const xScreenLen, float * const yScreenLen);


/* --------------------------------------------------------------------------------
   Function : bdWorldToScreenSizeX
   Purpose : convert a X axis only length in world space into screen pixels
   Parameters : basis point, world X length, world Y length, ptr for screen X length, ptr for screen Y length
   Returns : z distance or zero for clipped by near/far
   Info : 
*/

float bdWorldToScreenSizeX(const TBVector basisPoint, const float xWorldLen, float * const xScreenLen);


/* --------------------------------------------------------------------------------
   Function : bdWorldToScreenSizeXY
   Purpose : convert a Y axis only length in world space into screen pixels
   Parameters : basis point, world Y length, ptr for screen Y length
   Returns : z distance or zero for clipped by near/far
   Info : 
*/

float bdWorldToScreenSizeY(const TBVector basisPoint, const float yWorldLen, float * const yScreenLen);


/*	--------------------------------------------------------------------------------
	Function : bdSetZBias
	Purpose : Set a Z bias on the projection so that polygons are drawn with a different Z value.
	Parameters : [in] the bias to set
	Returns : void
	Info : 
*/

void bdSetZBias(const float zBias);


/* --------------------------------------------------------------------------------
   Function : bdScreenToWorldPoint
   Purpose : transform a point from screen (x,y) to world space
   Parameters : screen (x,y) from top-left in pixels, vector for world point
   Returns : 
   Info : 
*/

void bdScreenToWorldPoint(float xScreen, float yScreen, TBVector worldPos);


/*	--------------------------------------------------------------------------------
	Function : bdScreenToWorldPointOffset
	Purpose : transform screen coordinates to an object matrix.
	Parameters : [in] x, y screen coordinates.
				[in] the offset from the screen plane in world coordinates.
				[out] the position in world space
	Returns : 
	Info : 
*/

void bdScreenToWorldPointOffset(float xScreen, float yScreen, float zOffset, TBVector position);


/*	--------------------------------------------------------------------------------
	Function : bdGetObjectMatrix
	Purpose : Retrieve a copy of the current object matrix
	Parameters : matrix to copy object matrix into
	Returns : 
	Info : 
*/

void bdGetObjectMatrix(TBMatrix dest);


/*	--------------------------------------------------------------------------------
	Function : bdSetClipRectangle
	Purpose : Clips the geometry of the scene into the screen space rectangle specified.
	Parameters : [in] the x coordinate of the top left of the screen space rectangle
				[in] the y coordinate of the top left of the screen space rectangle
				[in] the width of the screen space rectangle
				[in] the height of the screen space rectangle
	Returns : 
	Info : 
*/

void bdSetClipRectangle(int topLeftX, int topLeftY, int width, int height);


/* --------------------------------------------------------------------------------
   Function : bdCalcVerticalFOV
   Purpose : calculate the correct vertical FOV to give 1:1 rendering
   Parameters : horizontal FOV, viewport width, viewport height
   Returns : vertical FOV
   Info : can pass width of zero to use current widths
*/

float bdCalcVerticalFOV(float hFov, float vpWidth = 0.0f, float vpHeight = 0.0f);


/* --------------------------------------------------------------------------------
   Function : bdSetIdentityObjectMatrix
   Purpose : set the object matrix to identity
   Parameters : 
   Returns : 
   Info : 
*/

void bdSetIdentityObjectMatrix();

#endif		// __BDISPLAY_XBVIEWPORT_H__
