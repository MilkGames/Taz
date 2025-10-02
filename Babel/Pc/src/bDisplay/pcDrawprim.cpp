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
    return;
}

void bdDrawPrimitive(uint32 primType, TBPrimVertex *vertices, int vertCount)
{
    return;
}

void bdDrawPrimitive(uint32 primType, TBPrimVertex2D *vertices, int vertCount)
{
    return;
}

void bdDrawPrimitive(uint32 primType, TBDualPrimVertex *vertices, int vertCount)
{
    return;
}

void bdDrawPrimitive(uint32 primType, TBDualPrimVertex2D *vertices, int vertCount)
{
    return;
}

void bdDrawPrimitive(uint32 primType, TBTriPrimVertex *vertices, int vertCount)
{
    return;
}

void bdDrawPrimitive(uint32 primType, TBTriPrimVertex2D *vertices, int vertCount)
{
    return;
}

void bdDrawPrimitive(uint32 primType, TBQuadPrimVertex *vertices, int vertCount)
{
    return;
}

void bdDrawPrimitive(uint32 primType, TBQuadPrimVertex2D *vertices, int vertCount)
{
    return;
}

void bdDrawPrimitive(uint32 primType, TBSpecularPrimVertex *vertices, int vertCount)
{
    return;
}

void bdDrawPrimitive(uint32 primType, TBSpecularPrimVertex2D *vertices, int vertCount)
{
    return;
}

void bdDrawPrimitive(uint32 primType, TBSpecularDualPrimVertex *vertices, int vertCount)
{
    return;
}

void bdDrawPrimitive(uint32 primType, TBSpecularDualPrimVertex2D *vertices, int vertCount)
{
    return;
}

void bdDrawPrimitive(uint32 primType, TBSpecularTriPrimVertex *vertices, int vertCount)
{
    return;
}

void bdDrawPrimitive(uint32 primType, TBSpecularTriPrimVertex2D *vertices, int vertCount)
{
    return;
}

void bdDrawPrimitive(uint32 primType, TBSpecularQuadPrimVertex *vertices, int vertCount)
{
    return;
}

void bdDrawPrimitive(uint32 primType, TBSpecularQuadPrimVertex2D *vertices, int vertCount)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdDrawPrimitiveIndexed
	Purpose : draw indexed primitive
	Parameters : primType, vertices, vertCount, indices, indexCount, vertType
	Returns : 
	Info : 
*/
void bdDrawPrimitiveIndexed(uint32 primType, void *vertices, int vertCount, ushort *indices, int indexCount, uint vertType)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdDrawPrimitiveVB
	Purpose : draw primitive from vertex buffer
	Parameters : primType, vbPtr, firstVertex, vertCount
	Returns : 
	Info : 
*/
void bdDrawPrimitiveVB(uint32 primType, TBVertexBuffer *vbPtr, int firstVertex, int vertCount)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdDrawPrimitiveIndexedVB
	Purpose : draw indexed primitive from vertex buffer
	Parameters : primType, vbPtr, firstVertex, vertCount, indices, indexCount
	Returns : 
	Info : 
*/
void bdDrawPrimitiveIndexedVB(uint32 primType, TBVertexBuffer *vbPtr, int firstVertex, int vertCount, ushort *indices, int indexCount)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdVertexBufferCreate
	Purpose : create vertex buffer
	Parameters : vbPtr, noofVertices, vertexType, flags, noofBuffers
	Returns : OK/FAIL
	Info : 
*/
int bdVertexBufferCreate(TBVertexBuffer **vbPtr, int32 noofVertices, uint vertexType, uint32 flags, int noofBuffers)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bdVertexBufferDestroy
	Purpose : destroy vertex buffer
	Parameters : vbPtr
	Returns : 
	Info : 
*/
void bdVertexBufferDestroy(TBVertexBuffer *vbPtr)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdVertexBufferLock
	Purpose : lock vertex buffer
	Parameters : vbPtr, flags
	Returns : vertex data
	Info : 
*/
void *bdVertexBufferLock(TBVertexBuffer *vbPtr, uint32 flags)
{
    return NULL;
}

/*	--------------------------------------------------------------------------------
	Function : bdVertexBufferUnlock
	Purpose : unlock vertex buffer
	Parameters : vbPtr
	Returns : 
	Info : 
*/
void bdVertexBufferUnlock(TBVertexBuffer *vbPtr)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdVertexBufferGetSize
	Purpose : get vertex buffer size
	Parameters : vbPtr
	Returns : size
	Info : 
*/
int bdVertexBufferGetSize(TBVertexBuffer *vbPtr)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bPrimCount
	Purpose : get primitive count
	Parameters : primType, vertCount
	Returns : count
	Info : 
*/
int bPrimCount(uint32 primType, int vertCount)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bSetVertexShader
	Purpose : set vertex shader
	Parameters : type, vertexBuffer
	Returns : 
	Info : 
*/
void bSetVertexShader(int type, TBVertexBuffer *vertexBuffer)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdGetPolyCount
	Purpose : get polygon count
	Parameters : 
	Returns : count
	Info : 
*/
int bdGetPolyCount()
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bGetVertexSize
	Purpose : get vertex size
	Parameters : vertType
	Returns : size
	Info : 
*/
int bGetVertexSize(int vertType)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bdIndexBufferCreate
	Purpose : create index buffer
	Parameters : ibPtr, noofIndices, flags
	Returns : OK/FAIL
	Info : 
*/
int bdIndexBufferCreate(TBIndexBuffer **ibPtr, int noofIndices, uint32 flags)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bdIndexBufferDestroy
	Purpose : destroy index buffer
	Parameters : ibPtr
	Returns : 
	Info : 
*/
void bdIndexBufferDestroy(TBIndexBuffer *ibPtr)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdIndexBufferLock
	Purpose : lock index buffer
	Parameters : ibPtr, flags
	Returns : index data
	Info : 
*/
void *bdIndexBufferLock(TBIndexBuffer *ibPtr, uint32 flags)
{
    return NULL;
}

/*	--------------------------------------------------------------------------------
	Function : bdIndexBufferUnlock
	Purpose : unlock index buffer
	Parameters : ibPtr
	Returns : 
	Info : 
*/
void bdIndexBufferUnlock(TBIndexBuffer *ibPtr)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdIndexBufferGetSize
	Purpose : get index buffer size
	Parameters : ibPtr
	Returns : size
	Info : 
*/
int bdIndexBufferGetSize(TBIndexBuffer *ibPtr)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bdDrawPointSpriteVB
	Purpose : draw point sprite from vertex buffer
	Parameters : buffer, startVertex, vertexCount, pointSize
	Returns : 
	Info : 
*/
void bdDrawPointSpriteVB(TBVertexBuffer *buffer, const int startVertex, const int vertexCount, const float pointSize)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdSetIndexBuffer
	Purpose : set index buffer
	Parameters : ibPtr, baseIndex
	Returns : 
	Info : 
*/
void bdSetIndexBuffer(TBIndexBuffer *ibPtr, int baseIndex)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdSetVertexBuffer
	Purpose : set vertex buffer
	Parameters : vbPtr
	Returns : 
	Info : 
*/
void bdSetVertexBuffer(TBVertexBuffer *vbPtr)
{
    return;
}
