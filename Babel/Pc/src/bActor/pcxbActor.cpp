// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcxbActor.cpp
//   Purpose : platform specific primary actor functions
// Component : Xbox Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Locals

uint64 bNoofSkinnedVertsProcessed = 0;
uint64 bNoofMeshVertsProcessed;
uint64 bNoofSkinnedActorsDrawn = 0;
uint64 bNoofMeshActorsDrawn = 0;

TBDrawMeshCallback bMeshDrawUserCallback = NULL;

int bMeshVertTypeLUT[] =
{
    0x0152, 0x0252, 0x0352, 0x0452,
    0x0000, 0x0000, 0x0000, 0x0000,
    0x0152, 0x0252, 0x0352, 0x0452
};

int bSkinVertTypeLUT[] =
{
    0x115C, 0x125C, 0x135C, 0x145C,
    0x0000, 0x0000, 0x0000, 0x0000,
    0x115C, 0x125C, 0x135C, 0x145C
};

uint32 bPrimTextureAddressLUT[4] =
{
    D3DTADDRESS_CLAMP, // [0]
    D3DTADDRESS_WRAP,  // [1]
    D3DTADDRESS_WRAP,  // [2]
    D3DTADDRESS_WRAP   // [3]
};

// ********************************************************************************
// Local Functions

void DrawMeshBatches_NoTextureNotComplex (struct _TBActorInstance *actorInstance, int noofBatches, TBMeshBatch *batchPtr, TBMeshPrim *prim, int startVert, int isAlphaPass)
{
    (void)actorInstance;
    (void)isAlphaPass;

    bdSetTexture(0, NULL);
    bdSetTexture(1, NULL);

    if (noofBatches == 0)
        return;

    do
    {
        int primCount = batchPtr->noofPrims;

        while (primCount != 0)
        {
            bDisplayInfo.d3dDevice->DrawPrimitive(
                (D3DPRIMITIVETYPE)prim->primType,
                (UINT)startVert,
                (UINT)prim->noofDrawPrims
            );

            startVert += (uint32)prim->noofVertices;
            ++prim;
            --primCount;
        }

        ++batchPtr;
        --noofBatches;
    } while (noofBatches != 0);
}

void DrawMeshBatches_NoTextureComplex (struct _TBActorInstance *actorInstance, int noofBatches, TBMeshBatch *batchPtr, TBMeshPrim *prim, int startVert, int isAlphaPass)
{
    (void)actorInstance;
    (void)isAlphaPass;

    bdSetTexture(0, NULL);
    bdSetTexture(1, NULL);

    if (noofBatches == 0)
        return;

    do
    {
        int primCount = batchPtr->noofPrims;

        while (primCount != 0)
        {
            bDisplayInfo.d3dDevice->DrawPrimitive(
                (D3DPRIMITIVETYPE)prim->primType,
                (UINT)startVert,
                (UINT)prim->noofDrawPrims
            );

            startVert += (uint32)prim->noofVertices;
            ++prim;
            --primCount;
        }

        ++batchPtr;
        --noofBatches;
    } while (noofBatches != 0);
}

void DrawMeshBatches_Texture1NotComplex (struct _TBActorInstance *actorInstance, int noofBatches, TBMeshBatch *batchPtr, TBMeshPrim *prim, int startVert, int isAlphaPass)
{
    (void)actorInstance;

    bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, 0x0C, 0);
    bdSetTexture(1, NULL);

    if (isAlphaPass == 0)
    {
        while (noofBatches-- != 0)
        {
            bdSetTexture(0, batchPtr->texture1);

            for (int i = batchPtr->noofPrims; i != 0; --i, ++prim)
            {
                bDisplayInfo.d3dDevice->DrawPrimitive(
                    (D3DPRIMITIVETYPE)prim->primType,
                    startVert,
                    prim->noofDrawPrims);

                startVert += prim->noofVertices;
            }

            ++batchPtr;
        }
        return;
    }

    while (noofBatches-- != 0)
    {
        if (batchPtr->texture1 == NULL)
        {
            bdSetTexture(0, NULL);
        }
        else
        {
            bdSetTexture(0, batchPtr->texture1);

            const ushort alphaBlendMode = batchPtr->texture1->alphaBlendMode;
            if (alphaBlendMode != 0xFF)
            {
                bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, (uint32)alphaBlendMode, 0);
            }
        }

        for (int i = batchPtr->noofPrims; i != 0; --i, ++prim)
        {
            bDisplayInfo.d3dDevice->DrawPrimitive(
                (D3DPRIMITIVETYPE)prim->primType,
                startVert,
                prim->noofDrawPrims);

            startVert += prim->noofVertices;
        }

        ++batchPtr;
    }
}

void DrawMeshBatches_Texture1Complex (struct _TBActorInstance *actorInstance, int noofBatches, TBMeshBatch *batchPtr, TBMeshPrim *prim, int startVert, int isAlphaPass)
{
    (void)actorInstance;

    uint32 addressU = 0x7fffffff;
    uint32 addressV = 0x7fffffff;

    bdSetTexture(1, NULL);

    while (noofBatches-- != 0)
    {
        const int primCount = batchPtr->noofPrims;

        if (batchPtr->texture1 == NULL)
        {
            bdSetTexture(0, NULL);

            for (int i = primCount; i != 0; --i, ++prim)
            {
                bDisplayInfo.d3dDevice->DrawPrimitive(
                    (D3DPRIMITIVETYPE)prim->primType,
                    startVert,
                    prim->noofDrawPrims);

                startVert += prim->noofVertices;
            }

            ++batchPtr;
            continue;
        }

        bdSetTexture(0, batchPtr->texture1);

        if (isAlphaPass != 0)
        {
            const ushort alphaBlendMode = batchPtr->texture1->alphaBlendMode;
            if (alphaBlendMode != 0xFF)
            {
                bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, (uint32)alphaBlendMode, 0);
            }
        }

        if ((batchPtr->flags & 1) == 0)
        {
            for (int i = primCount; i != 0; --i, ++prim)
            {
                const ushort primFlags = prim->flags;

                const uint32 newU = bPrimTextureAddressLUT[primFlags & 1];
                if (newU != addressU)
                {
                    bDisplayInfo.d3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, newU);
                    addressU = newU;
                }

                const uint32 newV = bPrimTextureAddressLUT[primFlags & 2];
                if (newV != addressV)
                {
                    bDisplayInfo.d3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, newV);
                    addressV = newV;
                }

                bDisplayInfo.d3dDevice->DrawPrimitive(
                    (D3DPRIMITIVETYPE)prim->primType,
                    startVert,
                    prim->noofDrawPrims);

                startVert += prim->noofVertices;
            }
        }
        else
        {
            if (addressU != D3DTADDRESS_CLAMP)
            {
                bDisplayInfo.d3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
                addressU = D3DTADDRESS_CLAMP;
            }
            if (addressV != D3DTADDRESS_CLAMP)
            {
                bDisplayInfo.d3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
                addressV = D3DTADDRESS_CLAMP;
            }

            for (int i = primCount; i != 0; --i, ++prim)
            {
                bDisplayInfo.d3dDevice->DrawPrimitive(
                    (D3DPRIMITIVETYPE)prim->primType,
                    startVert,
                    prim->noofDrawPrims);

                startVert += prim->noofVertices;
            }
        }

        ++batchPtr;
    }
}

void DrawMeshBatches_Texture1and2NotComplex (struct _TBActorInstance *actorInstance, int noofBatches, TBMeshBatch *batchPtr, TBMeshPrim *prim, int startVert, int isAlphaPass)
{
    (void)actorInstance;

    bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, 0xC, 0);

    if (noofBatches == 0)
        return;

    if (isAlphaPass == 0)
    {
        do
        {
            bdSetTexture(0, batchPtr->texture1);
            bdSetTexture(1, batchPtr->texture2);

            for (int i = batchPtr->noofPrims; i != 0; --i, ++prim)
            {
                bDisplayInfo.d3dDevice->DrawPrimitive(
                    (D3DPRIMITIVETYPE)prim->primType,
                    startVert,
                    prim->noofDrawPrims);

                startVert += prim->noofVertices;
            }

            ++batchPtr;
        } while (--noofBatches != 0);

        return;
    }

    do
    {
        if (batchPtr->texture1 == NULL)
        {
            bdSetTexture(0, NULL);
        }
        else
        {
            bdSetTexture(0, batchPtr->texture1);

            {
                const ushort alphaBlendMode = batchPtr->texture1->alphaBlendMode;
                if (alphaBlendMode != 0xFF)
                {
                    bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, (uint32)alphaBlendMode, 0);
                }
            }
        }

        bdSetTexture(1, batchPtr->texture2);

        for (int i = batchPtr->noofPrims; i != 0; --i, ++prim)
        {
            bDisplayInfo.d3dDevice->DrawPrimitive(
                (D3DPRIMITIVETYPE)prim->primType,
                startVert,
                prim->noofDrawPrims);

            startVert += prim->noofVertices;
        }

        ++batchPtr;
    } while (--noofBatches != 0);
}

void DrawMeshBatches_Texture1and2Complex (struct _TBActorInstance *actorInstance, int noofBatches, TBMeshBatch *batchPtr, TBMeshPrim *prim, int startVert, int isAlphaPass)
{
    uint32 cachedAddrU = 0x7FFFFFFF;
    uint32 cachedAddrV = 0x7FFFFFFF;

    (void)actorInstance;

    if (noofBatches == 0)
        return;

    if (isAlphaPass != 0)
    {
        do
        {
            if (batchPtr->texture1 != NULL)
            {
                bdSetTexture(0, batchPtr->texture1);

                {
                    const ushort alphaBlendMode = batchPtr->texture1->alphaBlendMode;
                    if (alphaBlendMode != 0xFF)
                        bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, (uint32)alphaBlendMode, 0);
                }
            }
            else
            {
                bdSetTexture(0, NULL);
            }

            bdSetTexture(1, batchPtr->texture2);

            for (int i = batchPtr->noofPrims; i != 0; --i, ++prim)
            {
                const ushort flags = prim->flags;

                uint32 addrU = bPrimTextureAddressLUT[flags & 1];
                if (addrU != cachedAddrU)
                {
                    bDisplayInfo.d3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, addrU);
                    cachedAddrU = addrU;
                }

                uint32 addrV = bPrimTextureAddressLUT[flags & 2];
                if (addrV != cachedAddrV)
                {
                    bDisplayInfo.d3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, addrV);
                    cachedAddrV = addrV;
                }

                bDisplayInfo.d3dDevice->DrawPrimitive(
                    (D3DPRIMITIVETYPE)prim->primType,
                    startVert,
                    prim->noofDrawPrims);

                startVert += prim->noofVertices;
            }

            ++batchPtr;
        } while (--noofBatches != 0);

        return;
    }

    do
    {
        bdSetTexture(0, batchPtr->texture1);
        bdSetTexture(1, batchPtr->texture2);

        for (int i = batchPtr->noofPrims; i != 0; --i, ++prim)
        {
            const ushort flags = prim->flags;

            uint32 addrU = bPrimTextureAddressLUT[flags & 1];
            if (addrU != cachedAddrU)
            {
                bDisplayInfo.d3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, addrU);
                cachedAddrU = addrU;
            }

            uint32 addrV = bPrimTextureAddressLUT[flags & 2];
            if (addrV != cachedAddrV)
            {
                bDisplayInfo.d3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, addrV);
                cachedAddrV = addrV;
            }

            bDisplayInfo.d3dDevice->DrawPrimitive(
                (D3DPRIMITIVETYPE)prim->primType,
                startVert,
                prim->noofDrawPrims);

            startVert += prim->noofVertices;
        }

        ++batchPtr;
    } while (--noofBatches != 0);
}

static TBDrawMeshCallback _drawFuncLUT[6] =
{
    DrawMeshBatches_NoTextureNotComplex,   // [0]
    DrawMeshBatches_NoTextureComplex,      // [1]
    DrawMeshBatches_Texture1NotComplex,    // [2]
    DrawMeshBatches_Texture1Complex,       // [3]
    DrawMeshBatches_Texture1and2NotComplex,// [4]
    DrawMeshBatches_Texture1and2Complex    // [5]
};

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bFixupAsyncMesh
	Purpose : fixup async mesh
	Parameters : asyncMesh, basePtr
	Returns : 
	Info : 
*/
void bFixupAsyncMesh(TBAsyncMesh *asyncMesh, uchar *basePtr)
{
    return; // empty, confirmed
}

/*	--------------------------------------------------------------------------------
	Function : bDrawMesh
	Purpose : draw mesh
	Parameters : mesh, actorInstance, alpha, nodeInstance
	Returns : 
	Info : 
*/
void bDrawMesh(TBMesh *mesh, struct _TBActorInstance *actorInstance, int alpha, struct _TBActorNodeInstance *nodeInstance)
{
    TBMatrix local_40;
    TBMatrix local_80;
    TBMatrix local_c0;

    EBActRenderMode *renderModePtr = actorInstance->renderMode;
    int renderPassCount;
    int renderPassIndex;
    int usesShaderPath;

    if ((actorInstance->renderMode[0] == BACTRENDERMODE_NORMAL) &&
        (actorInstance->renderMode[1] == BACTRENDERMODE_NONE) &&
        (actorInstance->renderMode[2] == BACTRENDERMODE_NONE))
    {
        usesShaderPath = 0;
        bmMatMultiply(local_40, actorInstance->objectToWorld, nodeInstance->nodeToLocalWorld);
        bdSetObjectMatrix(local_40);
        renderPassCount = 1;
    }
    else
    {
        usesShaderPath = 1;
        bmMatMultiply(local_c0, actorInstance->objectToWorld, nodeInstance->nodeToLocalWorld);
        bMatMultiplyTranspose((float *)local_80, bViewInfo.worldToProjection, local_c0, 4);
        bDisplayInfo.d3dDevice->SetVertexShaderConstant(0x5c, local_80, 4);
        renderPassCount = 3;
    }

    for (renderPassIndex = 0; renderPassIndex < renderPassCount; renderPassIndex++, renderModePtr++)
    {
        TBDrawMeshCallback drawFunc = bMeshDrawUserCallback;

        if (usesShaderPath)
        {
            if (*renderModePtr == BACTRENDERMODE_NONE)
                continue;

            if (baShaderSelect(actorInstance, renderPassIndex, mesh) == 0)
                continue;
        }
        else
        {
            bSetVertexShader(bMeshVertTypeLUT[actorInstance->actor->vertexType], (TBVertexBuffer *) mesh->vertexBuffer);
        }

        // Global actor alpha scaling uses stage 1 to scale alpha only (exact values from disasm).
        if (bGlobalActorAlphaScale != 0x100)
        {
            bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
            bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
            bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
            bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
            bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
        }

        bNoofMeshActorsDrawn++;

        if (drawFunc == NULL)
        {
            uint32 meshFlags = mesh->flags;
            int textureMode;
            int complexMode;

            if (meshFlags & 4)
            {
                textureMode = 0;
            }
            else
            {
                if ((bRenderState.textureEnable[0] == 0) && (bRenderState.shadowActive == 0))
                    textureMode = 0;
                else
                    textureMode = 1;

                if ((((meshFlags & 2) == 0) && (bRenderState.textureEnable[1] != 0)) ||
                    (bRenderState.UNKNOWN != 0))
                {
                    textureMode++;
                }
            }

            if (((meshFlags & 1) == 0) || (bRenderState.renderStateLock[2] != 0))
                complexMode = 0;
            else
                complexMode = 1;

            drawFunc = _drawFuncLUT[complexMode + (textureMode * 2)];
        }

        // alpha mask: 1=solid, 2=ckey, 4=alpha
        for (int passBit = 1; passBit < 8; passBit <<= 1)
        {
            if ((alpha & passBit) == 0)
                continue;

            if (passBit == 1)
            {
                if (mesh->noofSolidBatches != 0)
                {
                    bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, (uint)(bGlobalActorAlphaScale != 0x100), 0);
                    drawFunc(actorInstance,
                             mesh->noofSolidBatches,
                             mesh->batches,
                             mesh->primitives,
                             0,
                             0);
                }
            }
            else if (passBit == 2)
            {
                if (mesh->noofCKeyBatches != 0)
                {
                    bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, (uint)(bGlobalActorAlphaScale != 0x100), 0);
                    drawFunc(actorInstance,
                             mesh->noofCKeyBatches,
                             mesh->batches + mesh->noofSolidBatches,
                             mesh->primitives + mesh->firstCKeyPrim,
                             mesh->firstCKeyPrimVert,
                             0);
                }
            }
            else if (passBit == 4)
            {
                if (mesh->noofAlphaBatches != 0)
                {
                    bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, 1, 0);
                    bdSetRenderState(BDRENDERSTATE_ZWRITE, 0, 0);

                    drawFunc(actorInstance,
                             mesh->noofAlphaBatches,
                             mesh->batches + (mesh->noofSolidBatches + mesh->noofCKeyBatches),
                             mesh->primitives + mesh->firstAlphaPrim,
                             mesh->firstAlphaPrimVert,
                             1);

                    bdSetRenderState(BDRENDERSTATE_ZWRITE, 1, 0);
                }
            }
        }

        if (bGlobalActorAlphaScale != 0x100)
        {
            bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
            bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
        }

        if (usesShaderPath)
        {
            baShaderDeselect(actorInstance, renderPassIndex);
        }
    }
}

/*	--------------------------------------------------------------------------------
	Function : bInitActorPlatformSpecific
	Purpose : initialise actor platform specific
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/
int bInitActorPlatformSpecific()
{
    return OK; // empty, confirmed
}

/*	--------------------------------------------------------------------------------
	Function : bShutdownActorPlatformSpecific
	Purpose : shutdown actor platform specific
	Parameters : 
	Returns : 
	Info : 
*/
void bShutdownActorPlatformSpecific()
{
    return; // empty, confirmed
}

/*	--------------------------------------------------------------------------------
	Function : bSetupActorNodeList
	Purpose : setup actor node list
	Parameters : rootNode, actorInstance, alpha
	Returns : 
	Info : 
*/
void bSetupActorNodeList(struct _TBActorNodeInstance *rootNode, struct _TBActorInstance *actorInstance, int alpha)
{
    TBActorNodeInstance *node = rootNode;

    do
    {
        if ((node->flags & 0x10) == 0)
        {
            if (node->actorNode->type == 2)
            {
                bDrawMesh(&node->actorNode->mesh.mesh, actorInstance, alpha, node);
            }
        }

        if (node->children != NULL)
        {
            bSetupActorNodeList(node->children, actorInstance, alpha);
        }

        node = node->next;
    } while (node != rootNode);
}

/*	--------------------------------------------------------------------------------
	Function : bFixupSoftSkin
	Purpose : fixup soft skin
	Parameters : actor
	Returns : 
	Info : 
*/
void bFixupSoftSkin(struct _TBActor *actor)
{
	int vertexStrideBytes;
	int batchOffsetBytes;
	int batchIndex;

	actor = (TBActor *)actor;

	batchOffsetBytes = 0;

	/* fixup softskin pointers (actor-relative offsets) */
	if (actor->softSkin.vertices != NULL)
		actor->softSkin.vertices = actor->softSkin.vertices + (int)actor;

	if (actor->softSkin.primitives != NULL)
		actor->softSkin.primitives = (TBSoftSkinPrim *)((int)actor + (int)actor->softSkin.primitives);

	if (actor->softSkin.batches != NULL)
		actor->softSkin.batches = (TBSoftSkinBatch *)((int)actor + (int)actor->softSkin.batches);

	vertexStrideBytes = bGetVertexSize(bSkinVertTypeLUT[actor->vertexType]);

	/* per-batch fixup */
	if (actor->softSkin.batches != NULL)
	{
		TBSoftSkinPrim *primScanCursor = actor->softSkin.primitives; /* ESI in disasm */
		TBSoftSkinPrim *primColorCursor = actor->softSkin.primitives; /* stack var "actor" in disasm */
		uchar *vertexPtr = actor->softSkin.vertices; /* EDI in disasm */

		actor->flags |= 0x10;
		actor->softSkin.flags = 1;

		batchIndex = 0;

		if (actor->softSkin.noofBatches != 0)
		{
			do
			{
				TBSoftSkinBatch *batch = (TBSoftSkinBatch *)((uchar *)actor->softSkin.batches + batchOffsetBytes);
				TBResourceInfo *texRes;

				// temp debug code
				//uint32 crc0 = *(uint32 *)((uchar *)actor->softSkin.batches + batchOffsetBytes + 4);
				//uint32 crc1 = *(uint32 *)((uchar *)actor->softSkin.batches + batchOffsetBytes + 8);

				//bkPrintf(
					//"SoftSkinFixup: act=0x%08X batch=%u off=0x%X pkg=0x%08X loaded=%u grp=%u crc0=0x%08X crc1=0x%08X\n",
					//actor->resInfo.crc,
					//(uint32)batchIndex,
					//(uint32)batchOffsetBytes,
					//(uint32)actor->resInfo.packageId.crc,
					//(uint32)actor->resInfo.packageId.loaded,
					//(uint32)actor->resInfo.groupId,
					//crc0, crc1
				//);

				/* resolve texture1 */
				texRes = bkFindResourceByCRC(
					EBRESTYPE_TEXTURE,
					batch->textureCRC1,
					actor->resInfo.packageId,
					actor->resInfo.groupId,
					0);
				batch->texture1 = (TBTexture *)texRes;

				/* resolve texture2 */
				texRes = bkFindResourceByCRC(
					EBRESTYPE_TEXTURE,
					batch->textureCRC2,
					actor->resInfo.packageId,
					actor->resInfo.groupId,
					0);
				batch->texture2 = (TBTexture *)texRes;

				// temp debug code
				//TBTexture *t0 = (TBTexture *)(*(uint32 *)((uchar *)actor->softSkin.batches + batchOffsetBytes + 4));
				//TBTexture *t1 = (TBTexture *)(*(uint32 *)((uchar *)actor->softSkin.batches + batchOffsetBytes + 8));
				
				//bkPrintf(
					//"SoftSkinFixup:   tex0=%p cur0=%p frames0=%p n0=%u  tex1=%p cur1=%p frames1=%p n1=%u\n",
					//t0, (t0 ? t0->curFrame : NULL), (t0 ? t0->frames : NULL), (t0 ? (uint32)t0->noofFrames : 0),
					//t1, (t1 ? t1->curFrame : NULL), (t1 ? t1->frames : NULL), (t1 ? (uint32)t1->noofFrames : 0)
				//);

				/* if second texture exists, clear 0x10 on actor flags */
				if (batch->texture2 != NULL)
					actor->flags &= 0xFFFFFFEF;

				/* scan prim flags: if any (flags & 3) then clear NOTILING (bit0) */
				if ((actor->softSkin.flags & 1) != 0)
				{
					int primsInBatch = batch->noofPrims;
					if (primsInBatch != 0)
					{
						do
						{
							primsInBatch--;

							if ((primScanCursor->flags & 3) != 0)
							{
								actor->softSkin.flags &= 0xFFFFFFFE;
								break;
							}

							primScanCursor = (TBSoftSkinPrim *)((uchar *)primScanCursor + 0x14);
						}
						while (primsInBatch != 0);
					}
				}

				/* per-vertex colour LUT fixup */
				if (vertexPtr != NULL && batch->noofPrims != 0)
				{
					int primsLeftInBatch = batch->noofPrims;

					do
					{
						if (batch->texture1 == NULL)
						{
							vertexPtr += (int)primColorCursor->noofVertices * vertexStrideBytes;
						}
						else
						{
							int vertexIndexInPrim = 0;
							int vcount = (int)primColorCursor->noofVertices;

							if (vcount > 0)
							{
								do
								{
									uint32 c = *(uint32 *)(vertexPtr + 0x28);
									uint32 out;

									/* match disasm exactly */
									out  = bRColLUT[(c >> 16) & 0xFF];
									out |= bGColLUT[c & 0xFF];
									out |= bAColLUT[(c >> 24) & 0xFF];
									out |= bBColLUT[c & 0xFF];

									*(uint32 *)(vertexPtr + 0x28) = out;

									vertexPtr += vertexStrideBytes;
									vertexIndexInPrim++;
								}
								while (vertexIndexInPrim < vcount);
							}
						}

						primColorCursor = (TBSoftSkinPrim *)((uchar *)primColorCursor + 0x14);
						primsLeftInBatch--;
					}
					while (primsLeftInBatch != 0);
				}

				batchIndex++;
				batchOffsetBytes += 0x10;
			}
			while (batchIndex < (int)actor->softSkin.noofBatches);
		}
	}

	/* create/fill VB */
	if (actor->softSkin.vertices == NULL)
	{
		actor->softSkin.vertexBuffer = NULL;
	}
	else
	{
		TBVertexBuffer *vertexBuffer;
		uchar *vbPtr;
		uint bytesToCopy;
		uint dwords;
		uint tail;

		if (bdVertexBufferCreate(&vertexBuffer,
								(uint)actor->softSkin.noofVertices,
								bSkinVertTypeLUT[actor->vertexType],
								0, 1) == 0)
		{
			bkPrintf("bFixupSoftSkin: *** ERROR  Could not create vertex buffer for soft skin vertices ***\n");
			return;
		}

		vbPtr = (uchar *)bdVertexBufferLock(vertexBuffer, 0);

		bytesToCopy = (uint)actor->softSkin.noofVertices * (uint)vertexStrideBytes;

		{
			uchar *src = actor->softSkin.vertices;
			uchar *dst = vbPtr;

			dwords = bytesToCopy >> 2;
			while (dwords != 0)
			{
				*(uint32 *)dst = *(uint32 *)src;
				dst += 4;
				src += 4;
				dwords--;
			}

			tail = bytesToCopy & 3;
			while (tail != 0)
			{
				*dst++ = *src++;
				tail--;
			}
		}

		bdVertexBufferUnlock(vertexBuffer);
		actor->softSkin.vertexBuffer = vertexBuffer;
	}

	/* build CPU original verts (pos + normal) */
	if (actor->softSkin.vertices == NULL)
	{
		actor->softSkin.originalVerts = NULL;
	}
	else
	{
		TBSoftSkinVertexCPU *cpuVerts;
		uchar *src;
		uchar *dst;
		int i;

		cpuVerts = (TBSoftSkinVertexCPU *)MALLOC((uint)actor->softSkin.noofVertices * sizeof(TBSoftSkinVertexCPU));
		actor->softSkin.originalVerts = cpuVerts;

		src = actor->softSkin.vertices;
		dst = (uchar *)cpuVerts;

		for (i = 0; i < (int)actor->softSkin.noofVertices; i++)
		{
			/* copy as dwords to match disasm */
			*(uint32 *)(dst + 0x00) = *(uint32 *)(src + 0x00);
			*(uint32 *)(dst + 0x04) = *(uint32 *)(src + 0x04);
			*(uint32 *)(dst + 0x08) = *(uint32 *)(src + 0x08);

			*(uint32 *)(dst + 0x0C) = *(uint32 *)(src + 0x1C);
			*(uint32 *)(dst + 0x10) = *(uint32 *)(src + 0x20);
			*(uint32 *)(dst + 0x14) = *(uint32 *)(src + 0x24);

			src += vertexStrideBytes;
			dst += 0x18;
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function : bMatMultiplyTranspose
	Purpose : matrix multiply transpose
	Parameters : dest, src1, src2, rows
	Returns : 
	Info : 
*/
void bMatMultiplyTranspose(float *dest, const TBMatrix src1, const TBMatrix src2, int rows)
{ 
    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {
            float val = src1[0][r] * src2[c][0] +
                        src1[1][r] * src2[c][1] +
                        src1[2][r] * src2[c][2] +
                        src1[3][r] * src2[c][3];
            
            dest[r * 4 + c] = val;
        }
    }
}

/*	--------------------------------------------------------------------------------
	Function : bSetMatrixPaletteEntry
	Purpose : set matrix palette entry
	Parameters : actorInstance, nodeInstance
	Returns : 
	Info : 
*/
void bSetMatrixPaletteEntry(struct _TBActorInstance *actorInstance, struct _TBActorNodeInstance *nodeInstance)
{
	int32 matrixPaletteIndex;

	matrixPaletteIndex = nodeInstance->actorNode->softBone.matrixPaletteIndex;

	// Multiply node matrix by inverse bone reference, transpose and store as a 3x4 palette entry.
	bMatMultiplyTranspose(
		actorInstance->matrixPalette + (matrixPaletteIndex * 12),
		nodeInstance->nodeToLocalWorld,
		nodeInstance->actorNode->softBone.invBoneReference,
		3);
}

/*	--------------------------------------------------------------------------------
	Function : bFixupMesh
	Purpose : fixup mesh
	Parameters : basePtr, node, actor
	Returns : 
	Info : 
*/
void bFixupMesh(uchar *basePtr, struct _TBActorNode *node, struct _TBActor *actor)
{
	// ---------------------------------------------------------------------
	// Fix all offset-based pointers within the mesh block
	// ---------------------------------------------------------------------

	// Batches (array of TBMeshBatch)
	if (node->mesh.mesh.batches != NULL)
		node->mesh.mesh.batches = (TBMeshBatch *)(basePtr + (int)node->mesh.mesh.batches);

	// Primitive table
	if (node->mesh.mesh.primitives != NULL)
		node->mesh.mesh.primitives = (TBMeshPrim *)(basePtr + (int)node->mesh.mesh.primitives);

	// Vertex data
	if (node->mesh.mesh.vertices != NULL)
		node->mesh.mesh.vertices = basePtr + (int)node->mesh.mesh.vertices;

	// Shadow volume vertices
	if (node->mesh.mesh.svVerts != NULL)
		node->mesh.mesh.svVerts = (float *)(basePtr + (int)node->mesh.mesh.svVerts);

	// Shadow volume faces
	if (node->mesh.mesh.svFaces != NULL)
		node->mesh.mesh.svFaces = (TBShadowVolumeFace *)(basePtr + (int)node->mesh.mesh.svFaces);

	// Shadow volume edges
	if (node->mesh.mesh.svEdges != NULL)
		node->mesh.mesh.svEdges = (TBShadowVolumeEdge *)(basePtr + (int)node->mesh.mesh.svEdges);

	// ---------------------------------------------------------------------
	// Per-vertex color LUT fixup
	// ---------------------------------------------------------------------

	const int vertexStride = bGetVertexSize(bMeshVertTypeLUT[actor->vertexType]);

	if (node->mesh.mesh.vertices && node->mesh.mesh.noofVertices > 0)
	{
		unsigned char* ptr = node->mesh.mesh.vertices + 0x18; // color field offset
		for (int i = 0; i < node->mesh.mesh.noofVertices; ++i)
		{
			uint32 c = *(uint32*)ptr;
			uint32 r = bRColLUT[(c >> 16) & 0xFF];
			uint32 g = bGColLUT[(c >> 8) & 0xFF];
			uint32 a = bAColLUT[(c >> 24) & 0xFF];
			uint32 b = bBColLUT[c & 0xFF];
			*(uint32*)ptr = r | g | a | b;
			ptr += vertexStride;
		}
	}

	// ---------------------------------------------------------------------
	// Texture fixup and primitive flags
	// ---------------------------------------------------------------------

	int texturesStage1 = 0;
	int texturesStage2 = 0;
	uint32 nodeFlags = 0;
	TBMeshPrim *primCursor = node->mesh.mesh.primitives;

	if (node->mesh.mesh.batches)
	{
		TBMeshBatch* batch = node->mesh.mesh.batches;

		if (node->mesh.mesh.noofBatches <= 0)
		{
			nodeFlags |= 4;
		}
		else
		{
			for (int batchIndex = 0; batchIndex < node->mesh.mesh.noofBatches; ++batchIndex, ++batch)
			{
				TBTexture* tex1 = bkFindTextureByCRC(batch->textureCRC1,
					actor->resInfo.packageId, actor->resInfo.groupId, 0);
				TBTexture* tex2 = bkFindTextureByCRC(batch->textureCRC2,
					actor->resInfo.packageId, actor->resInfo.groupId, 0);

				batch->texture1 = tex1;
				batch->texture2 = tex2;
				batch->flags = 1;

				if (tex1) ++texturesStage1;
				if (tex2) ++texturesStage2;

				if (batch->noofPrims > 0)
				{
					for (int primIndex = 0; primIndex < batch->noofPrims; ++primIndex)
					{
						if ((primCursor->flags & 3) != 0)
						{
							nodeFlags |= 1;
							batch->flags = 0;
							break;
						}
						++primCursor;
					}
				}
			}

			if (texturesStage1 == 0 && texturesStage2 == 0)
				nodeFlags |= 4;
			else if (texturesStage1 != 0 && texturesStage2 == 0)
				nodeFlags |= 2;
		}

		node->mesh.mesh.flags |= nodeFlags;
	}

	// ---------------------------------------------------------------------
	// Vertex buffer creation
	// ---------------------------------------------------------------------

	uint32 vbFlags = (actor->vertexType > 3 && actor->vertexType < 8) ? 0x30 : 0x10;

	if (node->mesh.mesh.vertices == NULL)
	{
		node->mesh.mesh.vertices = NULL;
		return;
	}

	TBVertexBuffer* vb = NULL;
	if (bdVertexBufferCreate(&vb,
		node->mesh.mesh.noofVertices,
		bMeshVertTypeLUT[actor->vertexType],
		vbFlags, 1))
	{
		uint32 totalSize = node->mesh.mesh.noofVertices * vertexStride;
		unsigned char* dst = (unsigned char*)bdVertexBufferLock(vb, 0);
		const unsigned char* src = node->mesh.mesh.vertices;

		memcpy(dst, src, totalSize);
		bdVertexBufferUnlock(vb);
		node->mesh.mesh.vertexBuffer = (TBMeshVertexBuffer *)vb;
	}
	else
	{
		bkPrintf("bFixupMesh: failed to create vertex buffer\n");
	}
}

/*	--------------------------------------------------------------------------------
	Function : bPrepActorNodeList
	Purpose : prep actor node list
	Parameters : rootNode, parentXForm, parentScale, actorInstance
	Returns : 
	Info : 
*/
void bPrepActorNodeList(struct _TBActorNodeInstance *rootNode, TBMatrix parentXForm, TBVector parentScale, struct _TBActorInstance *actorInstance)
{
    TBActorNodeInstance *node = rootNode;

    do
    {
        TBMatrix rotMat;
        TBMatrix orientRefMat;
        TBMatrix localMat;

        float xTrans, yTrans, zTrans;

        bmQuatToMatrix(rotMat, node->animState.orientation);
        bmMatMultiply(orientRefMat, rotMat, node->referenceMatrix);

        xTrans = node->animState.position[0] * parentScale[0];
        yTrans = node->animState.position[1] * parentScale[1];
        zTrans = node->animState.position[2] * parentScale[2];

        xTrans += node->referenceMatrix[3][0] * parentScale[0];
        yTrans += node->referenceMatrix[3][1] * parentScale[1];
        zTrans += node->referenceMatrix[3][2] * parentScale[2];

        bmMatTransOrScale(
            localMat,
            xTrans, yTrans, zTrans,
            orientRefMat,
            node->animState.scale[0],
            node->animState.scale[1],
            node->animState.scale[2]
        );

        bmMatMultiply(node->nodeToLocalWorld, parentXForm, localMat);

        if (node->actorNode != NULL && node->actorNode->type == 1)
        {
            float *dst = actorInstance->matrixPalette
                       + (node->actorNode->softBone.matrixPaletteIndex * BACTOR_BONES_MATRIX_SIZE);

            bMatMultiplyTranspose(
                dst,
                node->nodeToLocalWorld,
                node->actorNode->softBone.invBoneReference,
                BACTOR_BONES_ROWS_IN_MATRIX
            );
        }

        if (node->children != NULL)
        {
            if (node != actorInstance->motionBone)
            {
				orientRefMat[3][0] = xTrans;
				orientRefMat[3][1] = yTrans;
				orientRefMat[3][2] = zTrans;
                bmMatMultiply(node->inheritedXForm, parentXForm, orientRefMat);
            }
            else
            {
                bmMatCopy(node->inheritedXForm, bIdentityMatrix);
            }

            bPrepActorNodeList(node->children, node->inheritedXForm, node->animState.scale, actorInstance);
        }

        node = node->next;
    } while (node != rootNode);
}

/*	--------------------------------------------------------------------------------
	Function : bDeleteMesh
	Purpose : delete mesh
	Parameters : meshNode
	Returns : 
	Info : 
*/
void bDeleteMesh(struct _TBActorNode *meshNode)
{
    TBVertexBuffer* vb = (TBVertexBuffer*)meshNode->mesh.mesh.vertexBuffer;
    if (vb != NULL) {
        bdVertexBufferDestroy(vb);
    }
}

/*	--------------------------------------------------------------------------------
	Function : bDeleteAsyncMesh
	Purpose : delete async mesh
	Parameters : meshNode
	Returns : 
	Info : 
*/
void bDeleteAsyncMesh(struct _TBActorNode *meshNode)
{
    return; // empty, confirmed
}

/*	--------------------------------------------------------------------------------
	Function : bDeleteSoftSkin
	Purpose : delete soft skin
	Parameters : actor
	Returns : 
	Info : 
*/
void bDeleteSoftSkin(struct _TBActor *actor)
{
    TBVertexBuffer* vb = (TBVertexBuffer*)actor->softSkin.vertexBuffer;
    if (vb != NULL) {
        bdVertexBufferDestroy(vb);
    }

    TBSoftSkinVertexCPU* verts = actor->softSkin.originalVerts;
    if (verts != NULL) {
        bkHeapFree(verts);
        actor->softSkin.originalVerts = NULL;
    }
}

/*	--------------------------------------------------------------------------------
	Function : baSetActorRenderCallbacks
	Purpose : set actor render callbacks
	Parameters : meshDraw, softSkinDraw
	Returns : OK/FAIL
	Info : 
*/
int baSetActorRenderCallbacks(TBDrawMeshCallback meshDraw, TBDrawSoftSkinCallback softSkinDraw)
{
        bkPrintf("*** WARNING *** baSetActorRenderCallbacks was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}
