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
    UINT stride;

    if (!bDisplayInfo.started)
        return;

    // Set FVF (as "vertex shader") and adjust VP mode if needed
    bSetVertexShader((int)vertType, (_TBVertexBuffer*)0);

    // Special case: engine QUAD list (primType == 6) is drawn as batched TRIANGLESTRIPs of 4 verts
    if (primType == 6) {
        // Stride selection (matches disassembly table)
        switch ((unsigned)vertType) {
            case 0x0152: stride = 0x24; break; // TBPrimVertex: XYZ+N+DIFFUSE+TEX1
            case 0x01C4: stride = 0x20; break; // TBSpecularPrimVertex2D: XYZRHW+DIFFUSE+SPECULAR+TEX1
            case 0x0252: stride = 0x2C; break;
            case 0x02C4: stride = 0x28; break;
            case 0x03C4: stride = 0x30; break;
            case 0x0352: stride = 0x34; break;
            case 0x115C: stride = 0x34; break;
            case 0x04C4: stride = 0x38; break;
            case 0x0452: stride = 0x3C; break;
            case 0x125C: stride = 0x40; break;
            case 0x135C: stride = 0x48; break;
            case 0x145C: stride = 0x50; break;
            case 0x0042: stride = 0x10; break; // point/compact
            default:     stride = 0;    break;
        }

        if (vertCount > 0 && stride != 0) {
            // Number of quads = ceil(vertCount / 4)
            int batches = ((vertCount - 1) >> 2) + 1;
            BYTE* ptr   = (BYTE*)vertices;
            const UINT step = stride * 4;

            while (batches-- > 0) {
                // Draw one quad as TRIANGLESTRIP with 2 primitives
                bDisplayInfo.d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, (const void*)ptr, stride);
                ptr += step;
            }
        }
        return;
    }

    // === Generic path: pass-through to D3D with adjusted primitive counts ===

    // Stride selection (matches disassembly table)
    switch ((unsigned)vertType) {
        case 0x0152: stride = 0x24; break; // TBPrimVertex: XYZ+N+DIFFUSE+TEX1
        case 0x01C4: stride = 0x20; break; // TBSpecularPrimVertex2D: XYZRHW+DIFFUSE+SPECULAR+TEX1
        case 0x0252: stride = 0x2C; break;
        case 0x02C4: stride = 0x28; break;
        case 0x03C4: stride = 0x30; break;
        case 0x0352: stride = 0x34; break;
        case 0x115C: stride = 0x34; break;
        case 0x04C4: stride = 0x38; break;
        case 0x0452: stride = 0x3C; break;
        case 0x125C: stride = 0x40; break;
        case 0x135C: stride = 0x48; break;
        case 0x145C: stride = 0x50; break;
        case 0x0042: stride = 0x10; break; // point/compact
        default:     stride = 0;    break;
    }

    // Convert vertex count -> primitive count per primitive type
    UINT primCount;
    switch (primType) {
        case 0: // POINTLIST: one primitive per vertex
            primCount = (vertCount < 0) ? 0 : (UINT)vertCount;
            break;
        case 1: // LINELIST: two vertices per primitive
            primCount = (vertCount < 0) ? 0 : (UINT)(vertCount >> 1);
            break;
        case 2: // LINESTRIP: N-1 primitives
            primCount = (vertCount <= 0) ? 0u : (UINT)(vertCount - 1);
            break;
        case 3: // TRIANGLELIST: three vertices per primitive
            primCount = (vertCount < 0) ? 0 : (UINT)(vertCount / 3);
            break;
        default: // TRIANGLESTRIP/FAN-like: N-2 primitives
            primCount = (vertCount <= 0) ? 0u : (UINT)(vertCount - 2);
            break;
    }

    if (primCount == 0 || stride == 0)
        return;

    // Engine primType maps to D3DPT by +1 (0..3 -> 1..4, strip/fan -> 5/6 as defined upstream)
    bDisplayInfo.d3dDevice->DrawPrimitiveUP((D3DPRIMITIVETYPE)(primType + 1), primCount, vertices, stride);
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
    if (!bDisplayInfo.started) {
        return;
    }

    // Bind the vertex shader / FVF & stream according to the buffer
    bSetVertexShader(vbPtr->vertexType, vbPtr);

    // Special handling for engine QUADLIST (primType == 6 in retail numbering)
    if (primType == 6U) {
        int i = 0;
        if (vertCount > 0) {
            do {
                // Render a quad (4 verts) as a triangle strip of 2 primitives
                bDisplayInfo.d3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, (UINT)i, (UINT)2);
                i += 4;
            } while (i < vertCount);
        }
        return;
    }

    // Map vertex count -> primitive count per primitive topology
    int primCount;
    switch (primType) {
        case 0: // point list
            primCount = vertCount;
            break;
        case 1: // line list
            primCount = vertCount / 2;
            break;
        case 2: // line strip
            primCount = vertCount - 1;
            break;
        case 3: // triangle list
            primCount = vertCount / 3;
            break;
        default: // triangle strip / triangle fan (engine maps both here)
            primCount = vertCount - 2;
            break;
    }

    // Device call matches the retail (+1 mapping from engine primType to D3D enum)
    bDisplayInfo.d3dDevice->DrawPrimitive((D3DPRIMITIVETYPE)(primType + 1U),
                                          (UINT)firstVertex,
                                          (UINT)primCount);
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
    // Guard: display must be started
    if (!bDisplayInfo.started) {
        return 0;
    }

    // Avoid zero-count
    if (noofVertices == 0) {
        noofVertices = 1;
    }

    // Allocate engine-side VB handle block (size matches TBVertexBuffer)
    TBVertexBuffer* blk = (_TBVertexBuffer*)MALLOCEX(sizeof(TBVertexBuffer), (uint32)"Vertex Buffer");

    if (!blk) {
        return 0;
    }

    // Initialize basic fields
    blk->noofVertices = noofVertices;
    blk->shadowData   = NULL;
    blk->flags        = flags;
    blk->vertexType   = vertexType;

    // Derive per-vertex stride in bytes (engine canonical mapping)
    // (Disassembly encodes a big table; bGetVertexSize centralizes the same mapping.)
    const int stride = bGetVertexSize((int)vertexType);
    blk->vertexSize  = stride;

    // For specific vertex types, set internal flag bit 0x20 (matches disasm behavior)
    // Set {0x125C, 0x0042, 0x115C, 0x135C, 0x145C}
    if (vertexType == 0x125Cu || vertexType == 0x0042u ||
        vertexType == 0x115Cu || vertexType == 0x135Cu ||
        vertexType == 0x145Cu)
    {
        blk->flags = flags | 0x20u;
    }

    // Decide pool from flags (per disasm): pool = ((~flags >> 6) & 1) ? MANAGED : DEFAULT;
    const D3DPOOL pool = (((~blk->flags) >> 6) & 1u) ? D3DPOOL_MANAGED : D3DPOOL_DEFAULT;

    // Usage bit: if no HW vertex shaders but flag 0x20 is requested, set SOFTWAREPROCESSING
    DWORD usage = 0;
    if (!bDisplayInfo.hwVertexShaders && (blk->flags & 0x20u)) {
        usage |= D3DUSAGE_SOFTWAREPROCESSING;
    }

    // Create the D3D8 vertex buffer; length = stride * noofVertices
    const UINT length = (stride > 0 && noofVertices > 0) ? (UINT)(stride * (UINT)noofVertices) : 0u;
    IDirect3DVertexBuffer8** ppVB = (IDirect3DVertexBuffer8**)blk; // struct begins with the COM pointer

    IDirect3DDevice8* dev = bDisplayInfo.d3dDevice;
    if (!dev || length == 0) {
        bkHeapFree(blk);
        return 0;
    }

    HRESULT hr = dev->CreateVertexBuffer(length, usage, (DWORD)vertexType, pool, ppVB);
    if (FAILED(hr)) {
        const char* err = DXGetErrorString8A(hr);
        bkPrintf("bdVertexBufferCreate: *** ERROR : Failed to create vertex buffer (%s) ***\n", err);
        bkHeapFree(blk);
        return 0;
    }

    // Link into global list tail (intrusive doubly-linked list with sentinel bVertexBufferList)
    blk->prev = bVertexBufferList.prev;
    blk->next = &bVertexBufferList;
    bVertexBufferList.prev->next = blk;
    bVertexBufferList.prev = blk;

    // Out
    *vbPtr = blk;
    (void)noofBuffers; // parameter present in signature, unused in implementation
    return 1;
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
    if (!vbPtr) return;

    // If this VB is currently bound, reset stream by setting a default vertex shader (0x0152)
    if (vbPtr == bLastVertexBuffer) {
        bSetVertexShader(0x0152, (TBVertexBuffer*)0);
    }

    // Unlink from intrusive list
    vbPtr->next->prev = vbPtr->prev;
    vbPtr->prev->next = vbPtr->next;

    // Release D3D buffer completely (loop until Release() returns 0)
    if (vbPtr->vertexBuffer) {
        while (vbPtr->vertexBuffer->Release() != 0) {
            // keep releasing until refcount hits zero
        }
        vbPtr->vertexBuffer = 0;
    }

    // Free shadow/system-memory mirror if present
    if (vbPtr->shadowData) {
        free(vbPtr->shadowData);
        vbPtr->shadowData = 0;
    }

    // Free the container
    bkHeapFree(vbPtr);
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
    IDirect3DVertexBuffer8 *vb = vbPtr ? vbPtr->vertexBuffer : 0;
    if (!vb) {
        // No D3D VB: fall back to shadow system memory.
        return vbPtr ? vbPtr->shadowData : 0;
    }

    DWORD lockFlags = (DWORD)(((((flags & 4) << 2) | (flags & 8)) << 6) | (flags & 2)) << 3;

    void *data = 0;
    HRESULT hr = vb->Lock(0, 0, (BYTE**)&data, lockFlags);
    if (hr < 0) {
        const char *err = DXGetErrorString8A(hr);
        bkPrintf("bdVertexBufferLock : *** ERROR *** Lock failed '%s'\n", err);
        return 0;
    }
    return data;
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
    IDirect3DVertexBuffer8 *vb = vbPtr ? vbPtr->vertexBuffer : 0;
    if (vb) {
        vb->Unlock();
    }
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
    // Count list items
    int count = 0;
    TBVertexBuffer* it = bVertexBufferList.next;
    if (it != &bVertexBufferList) {
        do { it = it->next; ++count; } while (it != &bVertexBufferList);
    }
    bkPrintf("Resuming %d vertex buffers...\n", count);

    TBVertexBuffer* vb = bVertexBufferList.next;
    if (vb == &bVertexBufferList) return;

    do {
        // usage = (!hwVS && (flags & 0x20)) ? D3DUSAGE_SOFTWAREPROCESSING : 0
        DWORD usage = 0;
        if (!bDisplayInfo.hwVertexShaders && (vb->flags & 0x20))
            usage = 0x10; // D3DUSAGE_SOFTWAREPROCESSING

        // pool = ((~flags >> 6) & 1) ? MANAGED : DEFAULT
        D3DPOOL pool = ((((~vb->flags) >> 6) & 1) ? D3DPOOL_MANAGED : D3DPOOL_DEFAULT);

        const UINT length = (UINT)vb->vertexSize * (UINT)vb->noofVertices;

        HRESULT hr = bDisplayInfo.d3dDevice->CreateVertexBuffer(
            length,
            usage,
            (DWORD)vb->vertexType,   // FVF
            pool,
            &vb->vertexBuffer        // first field in TBVertexBuffer
        );

        if (hr < 0) {
            bkPrintf("bResumeVertexBuffers: * ERROR * Could not recreate vertex buffer\n");
        } else if (vb->shadowData) {
            // Attempt to lock and upload the shadow copy
            BYTE* dst = 0;
            HRESULT hrL = E_FAIL;

            if (vb->vertexBuffer) {
                hrL = vb->vertexBuffer->Lock(0, 0, (BYTE**)&dst, 0);
            }

            if (!vb->vertexBuffer || hrL >= 0) {
                if (!dst) {
                    bkPrintf("bResumeVertexBuffers: * ERROR * Could not lock vertex buffer\n");
                } else {
                    // Copy exactly like the disasm (dwords then tail bytes)
                    const BYTE* srcB = (const BYTE*)vb->shadowData;
                    BYTE*       dstB = (BYTE*)dst;
                    const DWORD* srcD = (const DWORD*)srcB;
                    DWORD*       dstD = (DWORD*)dstB;

                    UINT dwords = length >> 2;
                    while (dwords--) *dstD++ = *srcD++;

                    srcB = (const BYTE*)srcD;
                    dstB = (BYTE*)dstD;
                    UINT tail = length & 3;
                    while (tail--) *dstB++ = *srcB++;

                    if (vb->vertexBuffer) vb->vertexBuffer->Unlock();
                }
            } else {
                const char* err = DXGetErrorString8A(hrL);
                bkPrintf("bdVertexBufferLock : *** ERROR *** Lock failed '%s'\n", err);
                bkPrintf("bResumeVertexBuffers: * ERROR * Could not lock vertex buffer\n");
            }

            free(vb->shadowData);
            vb->shadowData = 0;
        }

        vb = vb->next;
    } while (vb != &bVertexBufferList);
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
	if (bdWorldToScreenSizeXY(centre,xDim,yDim,&xDim,&yDim) == 0.0) return 0;
	return bdDrawFixedFlatSprite(centre,xDim,yDim,ang,texture,uFlip,vFlip,r,g,b,a,destVerts,zBias);
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
	TBSpecularPrimVertex2D localVerts[4];
	TBSpecularPrimVertex2D *verts = (destVerts != NULL) ? (TBSpecularPrimVertex2D *)destVerts : localVerts;

	TBVector proj;
	bdProjectVertices(proj, centre, 1);

	const float zProj = proj[2];

	// matches x87 behavior for NaN too
	if (!(zProj >= 0.05f) || !(zProj <= 1.0f)) {
		return 0;
	}

	// fog in specular alpha (0xAA000000)
	uint32 specular = 0xff000000;
	if (bRenderState.renderState[8][0] != 0) {
		const float fogZ = zProj / proj[3];

		if (!(fogZ > bRenderState.fogNear)) {
			specular = 0xff000000;
		} else if (!(fogZ <= bRenderState.fogFar)) {
			specular = 0x00000000;
		} else {
			const float t = (fogZ - bRenderState.fogNear) * bRenderState.fogRange * 255.0f;
			const int32 it = (int32)t;            // trunc like __ftol
			const int32 tmp = 0xff - it;
			specular = ((uint32)tmp) << 24;
		}
	}

	const float xC = bViewInfo.xCentre + proj[0];
	const float yC = bViewInfo.yCentre - proj[1];
	const float z   = zProj + zBias;

	const uint32 colour = (uint32)bRColLUT[r] | (uint32)bGColLUT[g] | (uint32)bBColLUT[b] | (uint32)bAColLUT[a];

	const float uRight  = (uFlip == 0) ? 1.0f : 0.0f;
	const float uLeft   = (uFlip == 0) ? 0.0f : 1.0f;
	const float vTop    = (vFlip == 0) ? 0.0f : 1.0f;
	const float vBottom = (vFlip == 0) ? 1.0f : 0.0f;

	// common per-vertex fields
	for (int32 i = 0; i < 4; i++) {
		verts[i].z        = z;
		verts[i].rhw      = 1.0f;
		verts[i].colour   = colour;
		verts[i].specular = specular;
	}

	// UV layout (triangle strip): right-top, right-bottom, left-top, left-bottom
	verts[0].u = uRight; verts[0].v = vTop;
	verts[1].u = uRight; verts[1].v = vBottom;
	verts[2].u = uLeft;  verts[2].v = vTop;
	verts[3].u = uLeft;  verts[3].v = vBottom;

	if (ang == 0.0f) {
		verts[0].x = xC + xDim; verts[0].y = yC - yDim;
		verts[1].x = xC + xDim; verts[1].y = yC + yDim;
		verts[2].x = xC - xDim; verts[2].y = yC - yDim;
		verts[3].x = xC - xDim; verts[3].y = yC + yDim;
	} else {
		const float c = (float)cos((double)ang);
		const float s = (float)sin((double)ang);

		// clockwise rotation (matches asm formulas):
		// x' = x*c + y*s
		// y' = -x*s + y*c
		const float x0 =  (xDim * c) + ((-yDim) * s);
		const float y0 =  (-xDim * s) + ((-yDim) * c);

		const float x1 =  (xDim * c) + (( yDim) * s);
		const float y1 =  (-xDim * s) + (( yDim) * c);

		const float x2 = ((-xDim) * c) + ((-yDim) * s);
		const float y2 = -((-xDim) * s) + ((-yDim) * c);

		const float x3 = ((-xDim) * c) + (( yDim) * s);
		const float y3 = -((-xDim) * s) + (( yDim) * c);

		verts[0].x = xC + x0; verts[0].y = yC + y0;
		verts[1].x = xC + x1; verts[1].y = yC + y1;
		verts[2].x = xC + x2; verts[2].y = yC + y2;
		verts[3].x = xC + x3; verts[3].y = yC + y3;
	}

	// if caller provided a buffer, only fill it
	if (destVerts != NULL) {
		return 1;
	}

	bdSetTexture(0, texture);

	if (bDisplayInfo.started != 0) {
		bSetVertexShader(0x1c4, (TBVertexBuffer *)0);
		bDisplayInfo.d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, localVerts, 0x20);
	}

	return 1;
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
	float *tmpPoints = (float *)MALLOC((uint32)(noofPoints << 5));      // 2 * noofPoints * 4 floats
	float *tmpSizes  = (float *)MALLOC((uint32)(noofPoints * 8));       // 2 * noofPoints floats
	int32 *tmpCols   = (int32 *)MALLOC((uint32)(noofPoints << 5));      // 2 * noofPoints * 4 ints

	float *tmpPointsW = tmpPoints;
	float *tmpSizesW  = tmpSizes;
	int32 *tmpColsW   = tmpCols;

	int32 runCount = 0;
	int32 totalVerts = 0;

	TBSpecularPrimVertex2D *outVerts = (TBSpecularPrimVertex2D *)destVerts;

	// Remaining segments = noofPoints - 1. The binary uses a decrementing counter.
	int32 remaining = (int32)(noofPoints - 1);

	for (int32 i = 0; i < (int32)(noofPoints - 1); ++i)
	{
		// Load endpoints (object space).
		const float *p0 = &pointArray[i * 4];
		const float *p1 = &pointArray[(i + 1) * 4];

		float x0 = p0[0];
		float y0 = p0[1];
		float z0w = p0[2];
		float w0 = p0[3];

		float x1 = p1[0];
		float y1 = p1[1];
		float z1w = p1[2];
		float w1 = p1[3];

		float size0 = sizeArray[i];
		float size1 = sizeArray[i + 1];

		float c0[4];
		float c1[4];
		const int32 c0i = (int32)(i * 4);
		const int32 c1i = (int32)((i + 1) * 4);
		c0[0] = (float)colourArray[c0i + 0];
		c0[1] = (float)colourArray[c0i + 1];
		c0[2] = (float)colourArray[c0i + 2];
		c0[3] = (float)colourArray[c0i + 3];
		c1[0] = (float)colourArray[c1i + 0];
		c1[1] = (float)colourArray[c1i + 1];
		c1[2] = (float)colourArray[c1i + 2];
		c1[3] = (float)colourArray[c1i + 3];

		// Project both endpoints to determine clip classification (projection-space Z).
		float proj0[4];
		float proj1[4];
		float vec0[4] = { x0, y0, z0w, w0 };
		float vec1[4] = { x1, y1, z1w, w1 };
		bmMatMultiplyVector2(proj0, bViewInfo.objectToProjection, vec0);
		bmMatMultiplyVector2(proj1, bViewInfo.objectToProjection, vec1);

		const float clipZ = 3.0f;
		const int32 in0 = (proj0[2] > clipZ);
		const int32 in1 = (proj1[2] > clipZ);

		// local_94 equivalent:
		// 0 = both outside
		// 1 = entering (add 2 points)
		// 2 = leaving  (add 1 point, then flush if more segments remain)
		// 3 = inside   (add 1 point)
		int32 segMode = 0;

		if (!in0)
		{
			if (in1)
			{
				// entering: move p0 to intersection
				const float t = (proj1[2] - clipZ) / (proj1[2] - proj0[2]);
				x0 = (x0 - x1) * t + x1;
				y0 = (y0 - y1) * t + y1;
				z0w = (z0w - z1w) * t + z1w;
				w0 = (w0 - w1) * t + w1;
				c0[0] = (c0[0] - c1[0]) * t + c1[0];
				c0[1] = (c0[1] - c1[1]) * t + c1[1];
				c0[2] = (c0[2] - c1[2]) * t + c1[2];
				c0[3] = (c0[3] - c1[3]) * t + c1[3];
				size0 = (size0 - size1) * t + size1;
				segMode = 1;
			}
			else
			{
				segMode = 0;
			}
		}
		else
		{
			if (in1)
			{
				segMode = 3;
			}
			else
			{
				// leaving: move p1 to intersection
				const float t = (proj0[2] - clipZ) / (proj0[2] - proj1[2]);
				x1 = (x1 - x0) * t + x0;
				y1 = (y1 - y0) * t + y0;
				z1w = (z1w - z0w) * t + z0w;
				w1 = (w1 - w0) * t + w0;
				c1[0] = (c1[0] - c0[0]) * t + c0[0];
				c1[1] = (c1[1] - c0[1]) * t + c0[1];
				c1[2] = (c1[2] - c0[2]) * t + c0[2];
				c1[3] = (c1[3] - c0[3]) * t + c0[3];
				size1 = (size1 - size0) * t + size0;
				segMode = 2;
			}
		}

		// Decrement remaining *after* classification (matches the binary's DEC).
		remaining--;

		// If we're inside and the run has not started yet, emit the start point.
		if ((segMode == 2 || segMode == 3) && runCount == 0)
		{
			tmpPointsW[0] = x0;
			tmpPointsW[1] = y0;
			tmpPointsW[2] = z0w;
			tmpPointsW[3] = w0;
			tmpPointsW += 4;
			*tmpSizesW++ = size0;
			// Truncate-to-zero (matches __ftol behaviour for in-range values).
			tmpColsW[0] = (int32)c0[0];
			tmpColsW[1] = (int32)c0[1];
			tmpColsW[2] = (int32)c0[2];
			tmpColsW[3] = (int32)c0[3];
			tmpColsW += 4;
			runCount = 1;
		}

		if (segMode == 1)
		{
			// entering: emit intersection + end point
			tmpPointsW[0] = x0;
			tmpPointsW[1] = y0;
			tmpPointsW[2] = z0w;
			tmpPointsW[3] = w0;
			tmpPointsW[4] = x1;
			tmpPointsW[5] = y1;
			tmpPointsW[6] = z1w;
			tmpPointsW[7] = w1;
			tmpPointsW += 8;

			*tmpSizesW++ = size0;
			*tmpSizesW++ = size1;

			tmpColsW[0] = (int32)c0[0];
			tmpColsW[1] = (int32)c0[1];
			tmpColsW[2] = (int32)c0[2];
			tmpColsW[3] = (int32)c0[3];
			tmpColsW[4] = (int32)c1[0];
			tmpColsW[5] = (int32)c1[1];
			tmpColsW[6] = (int32)c1[2];
			tmpColsW[7] = (int32)c1[3];
			tmpColsW += 8;

			runCount += 2;
		}
		else if (segMode == 2)
		{
			// leaving: emit clipped end point
			tmpPointsW[0] = x1;
			tmpPointsW[1] = y1;
			tmpPointsW[2] = z1w;
			tmpPointsW[3] = w1;
			tmpPointsW += 4;

			*tmpSizesW++ = size1;
			tmpColsW[0] = (int32)c1[0];
			tmpColsW[1] = (int32)c1[1];
			tmpColsW[2] = (int32)c1[2];
			tmpColsW[3] = (int32)c1[3];
			tmpColsW += 4;

			runCount += 1;

			// If there are more segments remaining, flush this run now.
			if (remaining > 0)
			{
				int32 drawCount = 0;
				for (int32 j = 0; j < runCount; ++j)
				{
					float *p = &tmpPoints[j * 4];
					const float worldSize = tmpSizes[j];
					if (bdWorldToScreenSizeX(p, worldSize, &screenSizeArray[j]) == 0.0f)
					{
						drawCount = 0;
						goto flush_done_a;
					}
				}
				drawCount = bdDrawFixedMultiStreak(runCount, tmpPoints, screenSizeArray, (int *)tmpCols,
								   (TBPrimVertex2D *)(outVerts + 1), zBias);
			flush_done_a:
				// Build degenerate brackets (positions only).
				if (drawCount > 0)
				{
					outVerts[0].x = outVerts[1].x;
					outVerts[0].y = outVerts[1].y;
					outVerts[0].z = outVerts[1].z;
					outVerts[0].rhw = outVerts[1].rhw;
				}
				else
				{
					outVerts[0].x = 0.0f;
					outVerts[0].y = 0.0f;
					outVerts[0].z = 0.0f;
					outVerts[0].rhw = 0.0f;
				}
				outVerts[drawCount + 1].x = outVerts[drawCount].x;
				outVerts[drawCount + 1].y = outVerts[drawCount].y;
				outVerts[drawCount + 1].z = outVerts[drawCount].z;
				outVerts[drawCount + 1].rhw = outVerts[drawCount].rhw;

				totalVerts += (drawCount + 2);
				outVerts += (drawCount + 2);

				// Reset run buffers.
				tmpPointsW = tmpPoints;
				tmpSizesW = tmpSizes;
				tmpColsW = tmpCols;
				runCount = 0;
			}
		}
		else if (segMode == 3)
		{
			// inside: emit end point
			tmpPointsW[0] = x1;
			tmpPointsW[1] = y1;
			tmpPointsW[2] = z1w;
			tmpPointsW[3] = w1;
			tmpPointsW += 4;

			*tmpSizesW++ = size1;
			tmpColsW[0] = (int32)c1[0];
			tmpColsW[1] = (int32)c1[1];
			tmpColsW[2] = (int32)c1[2];
			tmpColsW[3] = (int32)c1[3];
			tmpColsW += 4;

			runCount += 1;
		}
	}

	// Flush any final run.
	if (runCount != 0)
	{
		int32 drawCount = 0;
		for (int32 j = 0; j < runCount; ++j)
		{
			float *p = &tmpPoints[j * 4];
			const float worldSize = tmpSizes[j];
			if (bdWorldToScreenSizeX(p, worldSize, &screenSizeArray[j]) == 0.0f)
			{
				drawCount = 0;
				goto flush_done_b;
			}
		}
		drawCount = bdDrawFixedMultiStreak(runCount, tmpPoints, screenSizeArray, (int *)tmpCols,
							   (TBPrimVertex2D *)(outVerts + 1), zBias);
	flush_done_b:
		if (drawCount > 0)
		{
			outVerts[0].x = outVerts[1].x;
			outVerts[0].y = outVerts[1].y;
			outVerts[0].z = outVerts[1].z;
			outVerts[0].rhw = outVerts[1].rhw;
		}
		else
		{
			outVerts[0].x = 0.0f;
			outVerts[0].y = 0.0f;
			outVerts[0].z = 0.0f;
			outVerts[0].rhw = 0.0f;
		}
		outVerts[drawCount + 1].x = outVerts[drawCount].x;
		outVerts[drawCount + 1].y = outVerts[drawCount].y;
		outVerts[drawCount + 1].z = outVerts[drawCount].z;
		outVerts[drawCount + 1].rhw = outVerts[drawCount].rhw;
		totalVerts += (drawCount + 2);
	}

	bkHeapFree(tmpCols);
	bkHeapFree(tmpSizes);
	bkHeapFree(tmpPoints);

	return (int)totalVerts;
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
	// Note: the original prototype uses TBPrimVertex2D*, but the vertex layout written
	// by the original code is TBSpecularPrimVertex2D (0x20 bytes per vertex).
	TBSpecularPrimVertex2D *verts = (TBSpecularPrimVertex2D *)destVerts;

	TBVector proj0;
	TBVector proj1;

	bdProjectVertices(proj0, pointArray, 1);

	// match x87 behavior for NaN too
	if (!(proj0[2] >= 0.05f) || !(proj0[2] <= 1.0f))
	{
		return 0;
	}

	float x0 = bViewInfo.xCentre + proj0[0];
	float y0 = bViewInfo.yCentre - proj0[1];
	float z0 = proj0[2];

	bdProjectVertices(proj1, pointArray + 4, 1);

	if (!(proj1[2] >= 0.05f) || !(proj1[2] <= 1.0f))
	{
		return 0;
	}

	float x1 = bViewInfo.xCentre + proj1[0];
	float y1 = bViewInfo.yCentre - proj1[1];
	float z1 = proj1[2];

	// direction from point0 to point1
	float dxPrev = x1 - x0;
	float dyPrev = y1 - y0;
	float lenPrev = bmSqrtApprox(dxPrev * dxPrev + dyPrev * dyPrev);

	if (lenPrev > 0.0f)
	{
		dxPrev /= lenPrev;
		dyPrev /= lenPrev;
	}

	const uint32 specular = 0xff000000;
	const uint32 colour0 =
		bRColLUT[colourArray[0]] | bGColLUT[colourArray[1]] | bBColLUT[colourArray[2]] | bAColLUT[colourArray[3]];

	const float s0 = sizeArray[0];
	const float offY0 = dyPrev * s0;
	const float offX0 = dxPrev * s0;
	const float z0b = z0 + zBias;

	// start cap (v=0.0): take the strip pair and shift it backward by (-offX0, -offY0)
	verts[0].x = (x0 - offY0) - offX0;
	verts[0].y = (y0 + offX0) - offY0;
	verts[0].z = z0b;
	verts[0].rhw = 1.0f;
	verts[0].colour = colour0;
	verts[0].specular = specular;
	verts[0].u = 0.0f;
	verts[0].v = 0.0f;

	verts[1].x = (x0 + offY0) - offX0;
	verts[1].y = (y0 - offX0) - offY0;
	verts[1].z = z0b;
	verts[1].rhw = 1.0f;
	verts[1].colour = colour0;
	verts[1].specular = specular;
	verts[1].u = 1.0f;
	verts[1].v = 0.0f;

	// strip start at point0 (v=0.5)
	verts[2].x = x0 - offY0;
	verts[2].y = y0 + offX0;
	verts[2].z = z0b;
	verts[2].rhw = 1.0f;
	verts[2].colour = colour0;
	verts[2].specular = specular;
	verts[2].u = 0.0f;
	verts[2].v = 0.5f;

	verts[3].x = x0 + offY0;
	verts[3].y = y0 - offX0;
	verts[3].z = z0b;
	verts[3].rhw = 1.0f;
	verts[3].colour = colour0;
	verts[3].specular = specular;
	verts[3].u = 1.0f;
	verts[3].v = 0.5f;

	int32 outCount = 4;

	// iterate middle points (1..noofPoints-2) writing 2 verts per point at v=0.5
	float xCur = x1;
	float yCur = y1;
	float zCur = z1;

	for (int32 i = 1; i < (int32)noofPoints - 1; i++)
	{
		TBVector projNext;
		bdProjectVertices(projNext, pointArray + ((i + 1) * 4), 1);

		if (!(projNext[2] >= 0.05f) || !(projNext[2] <= 1.0f))
		{
			return 0;
		}

		float xNext = bViewInfo.xCentre + projNext[0];
		float yNext = bViewInfo.yCentre - projNext[1];
		float zNext = projNext[2];

		float dxNext = xNext - xCur;
		float dyNext = yNext - yCur;
		float lenNext = bmSqrtApprox(dxNext * dxNext + dyNext * dyNext);

		if (lenNext > 0.0f)
		{
			dxNext /= lenNext;
			dyNext /= lenNext;
		}

		const float s = sizeArray[i];
		const float avgDy = (dyNext + dyPrev) * 0.5f;
		const float avgDx = (dxNext + dxPrev) * 0.5f;

		// perpendicular scaled by size for this point
		const float offY = avgDy * s;
		const float offX = -avgDx * s;

		const float zB = zCur + zBias;

		const int *col = colourArray + (i * 4);
		const uint32 colour =
			bRColLUT[col[0]] | bGColLUT[col[1]] | bBColLUT[col[2]] | bAColLUT[col[3]];

		verts[outCount + 0].x = xCur - offY;
		verts[outCount + 0].y = yCur - offX;
		verts[outCount + 0].z = zB;
		verts[outCount + 0].rhw = 1.0f;
		verts[outCount + 0].colour = colour;
		verts[outCount + 0].specular = specular;
		verts[outCount + 0].u = 0.0f;
		verts[outCount + 0].v = 0.5f;

		verts[outCount + 1].x = xCur + offY;
		verts[outCount + 1].y = yCur + offX;
		verts[outCount + 1].z = zB;
		verts[outCount + 1].rhw = 1.0f;
		verts[outCount + 1].colour = colour;
		verts[outCount + 1].specular = specular;
		verts[outCount + 1].u = 1.0f;
		verts[outCount + 1].v = 0.5f;

		outCount += 2;

		// advance
		dxPrev = dxNext;
		dyPrev = dyNext;

		xCur = xNext;
		yCur = yNext;
		zCur = zNext;
	}

	// final cap at last point (index = noofPoints-1)
	const int32 last = (int32)noofPoints - 1;
	const float sL = sizeArray[last];
	const float offYL = dyPrev * sL;
	const float offXL = dxPrev * sL;
	const float zL = zCur + zBias;

	const int *colL = colourArray + (last * 4);
	const uint32 colourL =
		bRColLUT[colL[0]] | bGColLUT[colL[1]] | bBColLUT[colL[2]] | bAColLUT[colL[3]];

	// final strip pair at last point (v=0.5)
	verts[outCount + 0].x = xCur - offYL;
	verts[outCount + 0].y = yCur + offXL;
	verts[outCount + 0].z = zL;
	verts[outCount + 0].rhw = 1.0f;
	verts[outCount + 0].colour = colourL;
	verts[outCount + 0].specular = specular;
	verts[outCount + 0].u = 0.0f;
	verts[outCount + 0].v = 0.5f;

	verts[outCount + 1].x = xCur + offYL;
	verts[outCount + 1].y = yCur - offXL;
	verts[outCount + 1].z = zL;
	verts[outCount + 1].rhw = 1.0f;
	verts[outCount + 1].colour = colourL;
	verts[outCount + 1].specular = specular;
	verts[outCount + 1].u = 1.0f;
	verts[outCount + 1].v = 0.5f;

	// end cap (v=1.0) is the last-point strip pair shifted forward by (offXL, offYL)
	verts[outCount + 2].x = (xCur - offYL) + offXL;
	verts[outCount + 2].y = (yCur + offXL) + offYL;
	verts[outCount + 2].z = zL;
	verts[outCount + 2].rhw = 1.0f;
	verts[outCount + 2].colour = colourL;
	verts[outCount + 2].specular = specular;
	verts[outCount + 2].u = 0.0f;
	verts[outCount + 2].v = 1.0f;

	verts[outCount + 3].x = (xCur + offYL) + offXL;
	verts[outCount + 3].y = (yCur - offXL) + offYL;
	verts[outCount + 3].z = zL;
	verts[outCount + 3].rhw = 1.0f;
	verts[outCount + 3].colour = colourL;
	verts[outCount + 3].specular = specular;
	verts[outCount + 3].u = 1.0f;
	verts[outCount + 3].v = 1.0f;

	outCount += 4;
	return outCount;
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
    // if (type != bCurrentVertexShader) ...
    if (type != bCurrentVertexShader)
    {
        // ---- Decide T&L mode by type (the exact cases that jump to LAB_00572224) ----
        const int wantsHW =
            (type == 0x0152) || // BVERTTYPE_SINGLE
            (type == 0x01C4) || // BVERTTYPE_SPECULAR_SINGLE2D
            (type == 0x0252) || // BVERTTYPE_DUAL
            (type == 0x02C4) || // BVERTTYPE_SPECULAR_DUAL2D
            (type == 0x0352) || // (3D extended)
            (type == 0x03C4) || // SPECULAR_TRI2D
            (type == 0x0452);   // *** present in disasm ***

        if (wantsHW)
        {
            // if (bDisplayInfo.tnlHardware && !bDisplayInfo.tnlActive) -> enable HW T&L
            if (bDisplayInfo.tnlHardware && !bDisplayInfo.tnlActive)
            {
                // D3DRS_SOFTWAREVERTEXPROCESSING = 0
                bDisplayInfo.d3dDevice->SetRenderState((D3DRENDERSTATETYPE)0x99, 0);
                bDisplayInfo.tnlActive = 1;
                // force stream rebind (sentinel = 1)
                bLastVertexBuffer = (TBVertexBuffer*)1;
            }
        }
        else
        {
            // if (!hwVertexShaders && tnlActive) -> switch to SWVP
            if (!bDisplayInfo.hwVertexShaders && bDisplayInfo.tnlActive)
            {
                // D3DRS_SOFTWAREVERTEXPROCESSING = 1
                bDisplayInfo.d3dDevice->SetRenderState((D3DRENDERSTATETYPE)0x99, 1);
                bDisplayInfo.tnlActive = 0;
                // force stream rebind (sentinel = 1)
                bLastVertexBuffer = (TBVertexBuffer*)1;
            }
        }

        // Set FVF-as-vertex-shader and remember current
        bDisplayInfo.d3dDevice->SetVertexShader((DWORD)type);
        bCurrentVertexShader = type;
    }

    // ---- Stream-source binding path ----
    if (vertexBuffer == bLastVertexBuffer)
        return;

    if (vertexBuffer)
    {
        // IDirect3DDevice8::SetStreamSource(0, vb, stride)
        bDisplayInfo.d3dDevice->SetStreamSource(
            0,
            vertexBuffer->vertexBuffer,           // [EDI+0x00] in disasm
            (UINT)vertexBuffer->vertexSize        // [EDI+0x0C] in disasm
        );
        bLastVertexBuffer = vertexBuffer;
        return;
    }

    // vertexBuffer == NULL -> SetStreamSource(0, NULL, stride) with stride by 'type'
    UINT stride;
    switch (type)
    {
        case 0x0042: stride = 0x10; break;
        case 0x0152: stride = 0x24; break;
        case 0x01C4: stride = 0x20; break;
        case 0x0252: stride = 0x2C; break;
        case 0x02C4: stride = 0x28; break;
        case 0x03C4: stride = 0x30; break;
        case 0x0352: stride = 0x34; break;
        case 0x0452: stride = 0x3C; break;
        case 0x04C4: stride = 0x38; break;
        case 0x115C: stride = 0x34; break;
        case 0x125C: stride = 0x40; break;
        case 0x135C: stride = 0x48; break;
        case 0x145C: stride = 0x50; break;
        default:     stride = 0;    break;
    }

    bDisplayInfo.d3dDevice->SetStreamSource(0, NULL, stride);
    // Disasm stores EDI into bLastVertexBuffer; here EDI == vertexBuffer == NULL
    bLastVertexBuffer = vertexBuffer;
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
    // Count buffers first
    int count = 0;
    TBIndexBuffer* it = bIndexBufferList.next;
    if (it != &bIndexBufferList) {
        do { it = it->next; ++count; } while (it != &bIndexBufferList);
    }
    bkPrintf("Resuming %d index buffers...\n", count);

    TBIndexBuffer* ib = bIndexBufferList.next;
    if (ib == &bIndexBufferList) return;

    do {
        // Compute total byte length: (indexBits >> 3) * noofIndices
        UINT lengthBytes = (UINT)((ib->indexBits >> 3) * ib->noofIndices);

        // Create with Usage=0, Pool=DEFAULT (per disasm), Format from buffer
        HRESULT hr = bDisplayInfo.d3dDevice->CreateIndexBuffer(
            lengthBytes,
            0,                              // Usage
            ib->indexFormat,                // D3DFMT_INDEX16 / D3DFMT_INDEX32
            D3DPOOL_DEFAULT,                // Pool
            &ib->d3dIdxBuffer               // out
        );

        if (hr < 0) {
            const char* estr = DXGetErrorString8A(hr);
            bkPrintf("bResumeIndexBuffers: * ERROR * Could not recreate index buffer (%s)\n", estr);
        } else if (ib->shadowData) {
            // Lock and upload shadow copy
            BYTE* dst = 0;
            HRESULT hrl = E_FAIL;

            if (ib->d3dIdxBuffer) {
                hrl = ib->d3dIdxBuffer->Lock(0, 0, &dst, 0);
            }

            if (!ib->d3dIdxBuffer || hrl >= 0) {
                if (!dst) {
                    bkPrintf("bResumeIndexBuffers: * ERROR * Could not lock index buffer\n");
                } else {
                    // Copy dwords then tail bytes (match disasm behavior)
                    const BYTE* srcB = (const BYTE*)ib->shadowData;
                    BYTE*       dstB = (BYTE*)dst;

                    const DWORD* srcD = (const DWORD*)srcB;
                    DWORD*       dstD = (DWORD*)dstB;

                    UINT dwords = lengthBytes >> 2;
                    while (dwords--) { *dstD++ = *srcD++; }

                    srcB = (const BYTE*)srcD;
                    dstB = (BYTE*)dstD;

                    UINT tail = lengthBytes & 3;
                    while (tail--) { *dstB++ = *srcB++; }

                    if (ib->d3dIdxBuffer) ib->d3dIdxBuffer->Unlock();
                }
            } else {
                const char* estr = DXGetErrorString8A(hrl);
                bkPrintf("bdIndexBufferLock : *** ERROR *** Lock failed '%s'\n", estr);
                bkPrintf("bResumeIndexBuffers: * ERROR * Could not lock index buffer\n");
            }

            free(ib->shadowData);
            ib->shadowData = 0;
        }

        ib = ib->next;
    } while (ib != &bIndexBufferList);
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