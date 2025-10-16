// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcDrawprim.cpp
//   Purpose : Defines macros for incrementing poly count, primitive types, and setting/getting vertex attributes
// Component : Babel Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bdDrawPrimitive
	Purpose : draw primitive
	Parameters : primType, vertices, vertCount, vertType
	Returns : 
	Info : 
*/
void bdDrawPrimitive(uint32 primType, void *vertices, int vertCount, uint vertType)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBPrimVertex *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBPrimVertex2D *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBDualPrimVertex *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBDualPrimVertex2D *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBTriPrimVertex *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBTriPrimVertex2D *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBQuadPrimVertex *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBQuadPrimVertex2D *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBSpecularPrimVertex *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBSpecularPrimVertex2D *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBSpecularDualPrimVertex *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBSpecularDualPrimVertex2D *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBSpecularTriPrimVertex *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBSpecularTriPrimVertex2D *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBSpecularQuadPrimVertex *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBSpecularQuadPrimVertex2D *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : bdDrawPrimitiveIndexed
   Purpose : draw a primitive from an indexed array of vertices
   Parameters : primitive type, vertex array, number of vertices, index array, number of indices, vertex type
   Returns : 
   Info : 
*/

void bdDrawPrimitiveIndexed(uint32 primType, void *vertices, int vertCount, ushort *indices, int indexCount, uint vertType)
{
        bkPrintf("*** WARNING *** bdDrawPrimitiveIndexed was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bdDrawPrimitiveVB
   Purpose : draw a primitive from a vertex buffer
   Parameters : primitive type, vertex buffer, first vertex to use, number of vertices to use
   Returns : 
   Info : 
*/

void bdDrawPrimitiveVB(uint32 primType, TBVertexBuffer *vbPtr, int firstVertex, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitiveVB was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bdDrawPrimitiveIndexedVB
   Purpose : draw an indexed primitive from a vertex buffer
   Parameters : primitive type, vertex buffer, first vertex to use, number of vertices, index array, number of indices
   Returns : 
   Info : 
*/

void bdDrawPrimitiveIndexedVB(uint32 primType, TBVertexBuffer *vbPtr, int firstVertex, int vertCount, ushort *indices, int indexCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitiveIndexedVB was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bdVertexBufferCreate
   Purpose : create a new vertex buffer
   Parameters : ptr to fill with vertex buffer ptr, #vertices, vertex type, flags, #buffers
   Returns : OK/FAIL
   Info : 
*/

int bdVertexBufferCreate(TBVertexBuffer **vbPtr, int32 noofVertices, uint vertexType, uint32 flags, int noofBuffers)
{
        bkPrintf("*** WARNING *** bdVertexBufferCreate was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bdVertexBufferDestroy
   Purpose : destroy a vertex buffer
   Parameters : ptr to vertex buffer
   Returns : 
   Info : 
*/

void bdVertexBufferDestroy(TBVertexBuffer *vbPtr)
{
        bkPrintf("*** WARNING *** bdVertexBufferDestroy was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bdVertexBufferLock
   Purpose : lock a vertex buffer for access
   Parameters : ptr to vertex buffer, flags
   Returns : ptr to vertex data or NULL for failure
   Info : 
*/

void *bdVertexBufferLock(TBVertexBuffer *vbPtr, uint32 flags)
{
        bkPrintf("*** WARNING *** bdVertexBufferLock was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}


/* --------------------------------------------------------------------------------
   Function : bdVertexBufferUnlock
   Purpose : unlock a vertex buffer
   Parameters : ptr to vertex buffer
   Returns : 
   Info : 
*/

void bdVertexBufferUnlock(TBVertexBuffer *vbPtr)
{
        bkPrintf("*** WARNING *** bdVertexBufferUnlock was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bdVertexBufferGetSize
   Purpose : return the size of a vertex buffer
   Parameters : ptr to vertex buffer
   Returns : #verts that can fit in buffer
   Info : 
*/

int bdVertexBufferGetSize(TBVertexBuffer *vbPtr)
{
        bkPrintf("*** WARNING *** bdVertexBufferGetSize was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bdDrawFlatSprite
   Purpose : draw a camera-aligned sprite
   Parameters : object-space centre-point, xDim, yDim, texture, uFlip?, vFlip, colour, dest verts or NULL, zBias
   Returns : OK=valid, FAIL=clipped
   Info : 
*/

int bdDrawFlatSprite(TBVector centre, float xDim, float yDim, float ang, struct _TBTexture *texture, int32 uFlip, int32 vFlip,
					 int r,int g,int b,int a, TBPrimVertex2D *destVerts, float zBias)
{
        bkPrintf("*** WARNING *** bdDrawFlatSprite was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bdDrawGouraudSprite
   Purpose : draw a camera-aligned sprite
   Parameters : object-space centre-point, xDim, yDim, texture, uFlip?, vFlip, colours, dest verts or NULL, zBias
   Returns : OK=valid, FAIL=clipped
   Info : 
*/

int bdDrawGouraudSprite(TBVector centre, float xDim, float yDim, float ang, struct _TBTexture *texture, int32 uFlip, int32 vFlip,
						int *colour, TBPrimVertex2D *destVerts, float zBias)
{
        bkPrintf("*** WARNING *** bdDrawGouraudSprite was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bPrimCount
   Purpose : return the prim count from a prim type and vertex count
   Parameters : primitive type, number of vertices
   Returns : prim count
   Info : 
*/

int bPrimCount(uint32 primType, int vertCount)
{
        bkPrintf("*** WARNING *** bPrimCount was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bdDrawFixedFlatSprite
   Purpose : draw a camera-aligned sprite of constant size
   Parameters : object-space centre-point, xDim, yDim, texture, uFlip?, vFlip, colour, dest verts or NULL, zBias
   Returns : OK=valid, FAIL=clipped
   Info : dimensions are in pixels
*/

int bdDrawFixedFlatSprite(TBVector centre, float xDim, float yDim, float ang, struct _TBTexture *texture, int32 uFlip,
						  int32 vFlip, int r,int g,int b,int a, TBPrimVertex2D *destVerts, float zBias)
{
        bkPrintf("*** WARNING *** bdDrawFixedFlatSprite was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bdDrawFixedGouraudSprite
   Purpose : draw a camera-aligned sprite of constant size
   Parameters : object-space centre-point, xDim, yDim, texture, uFlip?, vFlip, colours, dest verts or NULL, zBias
   Returns : OK=valid, FAIL=clipped
   Info : dimensions are in pixels
*/

int bdDrawFixedGouraudSprite(TBVector centre, float xDim, float yDim, float ang, struct _TBTexture *texture, int32 uFlip,
							 int32 vFlip, int *colour, TBPrimVertex2D *destVerts, float zBias)
{
        bkPrintf("*** WARNING *** bdDrawFixedGouraudSprite was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bdDrawStreak
   Purpose : draw a camera-aligned streak-sprite
   Parameters : start point, start (xDim,yDim), start colour, end point, end (xDim,yDim), end colour, texture, 
				dest verts or NULL, zBias
   Returns : OK=valid, FAIL=clipped
   Info : positions are object space, dimensions are in world space
*/

int bdDrawStreak(TBVector startPos, float xStartDim, float yStartDim, int *startColour, TBVector endPos,
					  float xEndDim, float yEndDim, int *endColour, struct _TBTexture *texture, TBPrimVertex2D *destVerts,
					  float zBias)
{
        bkPrintf("*** WARNING *** bdDrawStreak was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bdDrawStreakClipped
   Purpose : draw a camera-aligned streak-sprite, clipped to projection plane
   Parameters : start point, start (xDim,yDim), start colour, end point, end (xDim,yDim), end colour, texture, 
				dest verts or NULL, zBias
   Returns : OK=valid, FAIL=clipped
   Info : positions are object space, dimensions are in world space
*/

int bdDrawStreakClipped(TBVector startPos, float xStartDim, float yStartDim, int *startColour, TBVector endPos,
					  float xEndDim, float yEndDim, int *endColour, struct _TBTexture *texture, TBPrimVertex2D *destVerts,
					  float zBias)
{
        bkPrintf("*** WARNING *** bdDrawStreakClipped was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bdDrawFixedStreak
   Purpose : draw a camera-aligned fixed size streak-sprite
   Parameters : start point, start (xDim,yDim), start colour, end point, end (xDim,yDim), end colour, texture,
				dest verts or NULL, zBias
   Returns : OK=valid, FAIL=clipped
   Info : positions are object space, dimensions are in pixels
*/

int bdDrawFixedStreak(TBVector startPos, float xStartDim, float yStartDim, int *startColour, TBVector endPos,
					  float xEndDim, float yEndDim, int *endColour, struct _TBTexture *texture, TBPrimVertex2D *destVerts,
					  float zBias)
{
        bkPrintf("*** WARNING *** bdDrawFixedStreak was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bdDrawMultiStreak
   Purpose : draw a camera-aligned multi-streak-sprite
   Parameters : noof points, point array, size array, screen size work array, colour array, dest verts, zBias
   Returns : number of verts used (can be zero)
   Info : positions are object space, dimensions are in world space,
		  all points must be within near/far limits or no drawing will result
*/

int bdDrawMultiStreak(int noofPoints, TBVector pointArray, float *sizeArray, float *screenSizeArray,
					  int *colourArray, TBPrimVertex2D *destVerts, float zBias)
{
        bkPrintf("*** WARNING *** bdDrawMultiStreak was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bdDrawMultiStreakClipped
   Purpose : draw a camera-aligned multi-streak-sprite
   Parameters : noof points, point array, size array, screen size work array, colour array, dest verts, zBias
   Returns : number of verts used (can be zero)
   Info : positions are object space, dimensions are in world space,
*/

int bdDrawMultiStreakClipped(int noofPoints, TBVector pointArray, float *sizeArray, float *screenSizeArray, int *colourArray,
					  TBPrimVertex2D *destVerts, float zBias)
{
        bkPrintf("*** WARNING *** bdDrawMultiStreakClipped was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bdDrawFixedMultiStreak
   Purpose : draw a camera-aligned fixed size multi-streak-sprite
   Parameters : noof points, point array, size array, colour array, dest verts, zBias
   Returns : number of verts used (can be zero)
   Info : positions are object space, dimensions are in screen space,
		  all points must be within near/far limits or no drawing will result
*/

int bdDrawFixedMultiStreak(int noofPoints, TBVector pointArray, float *sizeArray, int *colourArray,
						   TBPrimVertex2D *destVerts, float zBias)
{
        bkPrintf("*** WARNING *** bdDrawFixedMultiStreak was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bSetVertexShader
   Purpose : set the current vertex shader type
   Parameters : type of vertex shader to set, ptr to vertex buffer to set or NULL for none
   Returns : 
   Info : 
*/

void bSetVertexShader(int type, TBVertexBuffer *vertexBuffer)
{
        bkPrintf("*** WARNING *** bSetVertexShader was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bdGetPolyCount
   Purpose : return the approximate number of triangles draw this frame so far
   Parameters : 
   Returns : 
   Info : 
*/

int bdGetPolyCount()
{
        bkPrintf("*** WARNING *** bdGetPolyCount was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bGetVertexSize
   Purpose : determine the size of a vertex type
   Parameters : vertex type
   Returns : vertex size
   Info : 
*/

int bGetVertexSize(int vertType)
{
        bkPrintf("*** WARNING *** bGetVertexSize was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bdIndexBufferCreate
   Purpose : create a new index buffer
   Parameters : ptr to fill with index buffer ptr, #indices, flags (see BINDEXBUF_)
   Returns : OK/FAIL
   Info : 
*/

int bdIndexBufferCreate(TBIndexBuffer **ibPtr, int noofIndices, uint32 flags)
{
        bkPrintf("*** WARNING *** bdIndexBufferCreate was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bdIndexBufferDestroy
   Purpose : destroy a index buffer
   Parameters : ptr to index buffer
   Returns : 
   Info : 
*/

void bdIndexBufferDestroy(TBIndexBuffer *ibPtr)
{
        bkPrintf("*** WARNING *** bdIndexBufferDestroy was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bdIndexBufferLock
   Purpose : lock an index buffer for access
   Parameters : ptr to index buffer, flags (see BINDEXBUF_)
   Returns : ptr to index data or NULL for failure
   Info : 
*/

void *bdIndexBufferLock(TBIndexBuffer *ibPtr, uint32 flags)
{
        bkPrintf("*** WARNING *** bdIndexBufferLock was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}


/* --------------------------------------------------------------------------------
   Function : bdIndexBufferUnlock
   Purpose : unlock an index buffer
   Parameters : ptr to index buffer
   Returns : 
   Info : 
*/

void bdIndexBufferUnlock(TBIndexBuffer *ibPtr)
{
        bkPrintf("*** WARNING *** bdIndexBufferUnlock was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bdIndexBufferGetSize
   Purpose : return the size of an index buffer
   Parameters : ptr to index buffer
   Returns : #indices that can fit in buffer
   Info : 
*/

int bdIndexBufferGetSize(TBIndexBuffer *ibPtr)
{
        bkPrintf("*** WARNING *** bdIndexBufferGetSize was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bdDrawPointSpriteVB
	Purpose : Render a vertex buffer of points as a point sprite buffer. The points are rendered with the current texture.
	Parameters : [in] the point buffer to render
				[in] the starting vertex in the buffer to render.
				[in] the number of vertices in the buffer to render.
				[in] the size of the points to render.
	Returns : 
	Info : 
*/

void bdDrawPointSpriteVB(TBVertexBuffer *buffer, const int startVertex, const int vertexCount, const float pointSize)
{
        bkPrintf("*** WARNING *** bdDrawPointSpriteVB was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bdSetIndexBuffer
   Purpose : set an index buffer as current
   Parameters : ptr to index buffer (NULL to set none), base vertex offset
   Returns : 
   Info : 
*/

void bdSetIndexBuffer(TBIndexBuffer *ibPtr, int baseIndex)
{
        bkPrintf("*** WARNING *** bdSetIndexBuffer was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bdSetVertexBuffer
   Purpose : set a vertex buffer as current
   Parameters : ptr to vertex buffer (NULL to set none)
   Returns : 
   Info : 
*/

void bdSetVertexBuffer(TBVertexBuffer *vbPtr)
{
        bkPrintf("*** WARNING *** bdSetVertexBuffer was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}