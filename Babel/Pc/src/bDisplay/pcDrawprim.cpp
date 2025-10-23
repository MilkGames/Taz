// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcDrawprim.cpp
//   Purpose : Defines macros for incrementing poly count, primitive types, and setting/getting vertex attributes
// Component : Babel Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Globals

TBVertexBuffer  bVertexBufferList;
int				bCurrentVertexShader;
TBVertexBuffer* bLastVertexBuffer = (TBVertexBuffer*)1;
TBIndexBuffer   bIndexBufferList;

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
        bkPrintf("*** WARNING *** bdDrawPrimitive #1 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBPrimVertex *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive #2 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBPrimVertex2D *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive #3 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBDualPrimVertex *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive #4 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBDualPrimVertex2D *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive #5 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBTriPrimVertex *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive #6 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBTriPrimVertex2D *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive #7 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBQuadPrimVertex *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive #8 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBQuadPrimVertex2D *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive #9 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBSpecularPrimVertex *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive #10 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBSpecularPrimVertex2D *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive #11 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBSpecularDualPrimVertex *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive #12 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBSpecularDualPrimVertex2D *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive #13 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBSpecularTriPrimVertex *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive #14 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBSpecularTriPrimVertex2D *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive #15 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBSpecularQuadPrimVertex *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive #16 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void bdDrawPrimitive(uint32 primType, TBSpecularQuadPrimVertex2D *vertices, int vertCount)
{
        bkPrintf("*** WARNING *** bdDrawPrimitive #17 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
    {
    if (bDisplayInfo.started == 0) {
        return 0;
    }

    if (noofVertices == 0) {
        noofVertices = 1;
    }

    // Allocate TBVertexBuffer via engine heap (must use MALLOCEX).
    // (0x65739C -> "Vertex Buffer")
    _TBVertexBuffer *blk = (_TBVertexBuffer*)MALLOCEX(sizeof(_TBVertexBuffer), (uint32)"Vertex Buffer");
    if (!blk) {
        return 0;
    }

    // Initialize engine-side fields
    blk->noofVertices = noofVertices;
    blk->shadowData   = NULL;
    blk->flags        = flags;
    blk->vertexType   = (int)vertexType;

    // Determine vertex size (mirrors the decompiled cascade)
    int vsize = bGetVertexSize((int)vertexType);
    blk->vertexSize = vsize;

    // If certain vertex types are used, force SW processing flag (bit 0x20)
    // Matches the decompiled if-chain:
    //   types: 0x125C, 0x0042, 0x115C, 0x135C, 0x145C
    if ((vertexType < 0x125D &&
         (vertexType == 0x125C || vertexType == 0x0042 || vertexType == 0x115C)) ||
        (vertexType == 0x135C || vertexType == 0x145C))
    {
        blk->flags = flags | 0x20; // BVERTBUF_NEEDVERTEXSHADER?? can't tell for sure
    }

    // Build D3D usage/pool from flags and device capabilities
    DWORD usage = 0;
    if (bDisplayInfo.hwVertexShaders == 0 && (blk->flags & 0x20)) {
        usage |= D3DUSAGE_SOFTWAREPROCESSING; // (-cVar1 & 0x10) in the decompile
    }

    // Pool: (~flags >> 6) & 1 -> 0 (DEFAULT) or 1 (MANAGED)
    const DWORD poolBit = ((~blk->flags) >> 6) & 1;
    D3DPOOL pool = (poolBit != 0) ? D3DPOOL_MANAGED : D3DPOOL_DEFAULT;

    // Create the D3D8 vertex buffer. Length = vertexSize * noofVertices
    HRESULT hr = bDisplayInfo.d3dDevice->CreateVertexBuffer(
        vsize * noofVertices,
        usage,
        vertexType,                // FVF
        pool,
        &blk->vertexBuffer
    );

    if (SUCCEEDED(hr)) {
        // Insert into the global intrusive list
        blk->prev = bVertexBufferList.prev;
        blk->next = &bVertexBufferList;
        bVertexBufferList.prev->next = blk;
        bVertexBufferList.prev       = blk;

        // Out parameter
        *vbPtr = blk;
        return 1;
    }

    // Error path: report and free
    bkPrintf("bdVertexBufferCreate: *** ERROR : Failed to create vertex buffer (%s) ***\n", DXGetErrorString8A(hr));
    bkHeapFree(blk);
    return 0;
}
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
   Function : bSuspendVertexBuffers
   Purpose : suspend vertex buffers
   Parameters : 
   Returns : 
   Info : 
*/

void bSuspendVertexBuffers()
{
	bkPrintf("*** WARNING *** bSuspendVertexBuffers was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
}

/* --------------------------------------------------------------------------------
   Function : bResumeVertexBuffers
   Purpose : resume vertex buffers
   Parameters : 
   Returns : 
   Info : 
*/

void bResumeVertexBuffers()
{
	bkPrintf("*** WARNING *** bResumeVertexBuffers was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
    // If FVF changes, (re)configure HW/SW T&L and set the FVF.
    if (type != bCurrentVertexShader)
    {
        bool forceHW = false;

        // ---- decide whether to force HW T&L ----
        if (type < 0x353) {
            if (type == 0x352) {
                forceHW = true;
            } else if (type > 0x252) {
                // 0x2C4
                forceHW = (type == 0x2C4);
            } else if (type == 0x252) {
                forceHW = true;
            } else if ((type - 0x152) == 0) { // 0x152
                forceHW = true;
            }
        } else {
            if (type == 0x3C4) {
                forceHW = true;
            } else {
                const int delta = type - 0x452; // 0x452, 0x4C4 (= 0x452 + 0x72)
                if (delta == 0 || delta == 0x72) {
                    forceHW = true;
                }
            }
        }

        // ---- toggle SW/HW vertex processing exactly like decompiled branches ----
        if (forceHW) {
            if (bDisplayInfo.tnlHardware != 0 && bDisplayInfo.tnlActive == 0) {
                bDisplayInfo.d3dDevice->SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING, FALSE);
                bDisplayInfo.tnlActive = 1;
                // force rebind of stream on next part (sentinel == 1)
                bLastVertexBuffer = (TBVertexBuffer*)1;
            }
        } else {
            if (bDisplayInfo.hwVertexShaders == 0 && bDisplayInfo.tnlActive != 0) {
                bDisplayInfo.d3dDevice->SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING, TRUE);
                bDisplayInfo.tnlActive = 0;
                bLastVertexBuffer = (TBVertexBuffer*)1;
            }
        }

        // Set the FVF (in D3D8 this is SetVertexShader with a DWORD FVF)
        bDisplayInfo.d3dDevice->SetVertexShader((DWORD)type);
        bCurrentVertexShader = type;
    }

    // Already bound? nothing to do
    if (vertexBuffer == bLastVertexBuffer)
        return;

    if (vertexBuffer != NULL)
    {
        // Bind real buffer + stride from buffer
        bDisplayInfo.d3dDevice->SetStreamSource(
            /*Stream*/0,
            vertexBuffer->vertexBuffer,
            (UINT)vertexBuffer->vertexSize
        );
        bLastVertexBuffer = vertexBuffer;
        return;
    }

    // vertexBuffer == nullptr -> match decompile: SetStreamSource(0, NULL, stride_for_type)
    const int stride = bGetVertexSize(type);
    bDisplayInfo.d3dDevice->SetStreamSource(/*Stream*/0, /*pVB*/NULL, (UINT)stride);
    bLastVertexBuffer = NULL;
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
    if (vertType < 0x03C5)
    {
        if (vertType == 0x03C4)      /* BVERTTYPE_SPECULAR_TRI2D */
            return 0x30;             /* 48 */
        if (vertType < 0x0253)
        {
            if (vertType == 0x0252)  /* BVERTTYPE_DUAL */
                return 0x2C;         /* 44 */
            if (vertType == 0x0042)  /* BVERTTYPE_POINTSPRITE */
                return 0x10;         /* 16 */
            if (vertType == 0x0152)  /* BVERTTYPE_SINGLE */
                return 0x24;         /* 36 */
            if (vertType == 0x01C4)  /* BVERTTYPE_SPECULAR_SINGLE2D */
                return 0x20;         /* 32 */
        }
        else
        {
            if (vertType == 0x02C4)  /* BVERTTYPE_SPECULAR_DUAL2D */
                return 0x28;         /* 40 */
            if (vertType == 0x0352)  /* BVERTTYPE_TRI */
                return 0x34;         /* 52 */
        }
    }
    else if (vertType < 0x125D)
    {
        if (vertType == 0x125C)      /* BVERTTYPE_BLENDEDDUAL (+0x1000 flag family) */
            return 0x40;             /* 64 */
        if (vertType == 0x0452)      /* BVERTTYPE_QUAD */
            return 0x3C;             /* 60 */
        if (vertType == 0x04C4)      /* BVERTTYPE_SPECULAR_QUAD2D */
            return 0x38;             /* 56 */
        if (vertType == 0x115C)      /* BVERTTYPE_BLENDEDSINGLE (+0x1000 flag family) */
            return 0x34;             /* 52 */
    }
    else
    {
        if (vertType == 0x135C)      /* BVERTTYPE_BLENDEDTRI (+0x1000 flag family) */
            return 0x48;             /* 72 */
        if (vertType == 0x145C)      /* BVERTTYPE_BLENDEDQUAD (+0x1000 flag family) */
            return 0x50;             /* 80 */
    }

    return 0;                         /* unknown/unsupported */
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

/* --------------------------------------------------------------------------------
   Function : bSuspendIndexBuffers
   Purpose : suspend index buffers
   Parameters : 
   Returns : 
   Info : 
*/

void bSuspendIndexBuffers()
{
	bkPrintf("*** WARNING *** bSuspendIndexBuffers was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
}

/* --------------------------------------------------------------------------------
   Function : bResumeIndexBuffers
   Purpose : resume index buffers
   Parameters : 
   Returns : 
   Info : 
*/

void bResumeIndexBuffers()
{
	bkPrintf("*** WARNING *** bResumeIndexBuffers was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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