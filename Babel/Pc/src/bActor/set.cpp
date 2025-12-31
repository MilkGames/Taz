// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games
//
//      File : set.cpp
//   Purpose : generic set functions
// Component : Generic Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Local Variables

int bCellsDrawnPerc = 0;

// ********************************************************************************
// Local Functions

typedef void (*TBSetFixupFn)(TBSet *set);
typedef void (*TBSetDeleteFn)(TBSet *set);
typedef void (*TBSetDrawFn)(TBSet *set, int alpha);

void bRegCellsFixUp(TBSet *set)
{
	TBSetRegCells *reg = (TBSetRegCells *)set;

	int32 cellsRemaining = reg->yNoofCells * reg->zNoofCells * reg->xNoofCells;

	TBRegCell *cell = (TBRegCell *)(reg + 1);
	TBRegCellPrim *prim = (TBRegCellPrim *)(cell + cellsRemaining);

	TBTexture *cachedTex0 = (TBTexture *)0;
	TBTexture *cachedTex1 = (TBTexture *)0;
	uint32 cachedTex0Crc = 0;
	uint32 cachedTex1Crc = 0;

	reg->cells = cell;

	while (cellsRemaining > 0)
	{
		cell->prims = prim;

		if (cell->noofPrims > 0)
		{
			int32 i = 0;
			while (i < cell->noofPrims)
			{
				uint32 tex0Crc = (uint32)(uint*)prim->texture;
				if (tex0Crc != cachedTex0Crc)
				{
					cachedTex0 = (TBTexture *)bkFindResourceByCRC(
						EBRESTYPE_TEXTURE,
						tex0Crc,
						reg->setInfo.resInfo.packageId,
						(uint32)reg->setInfo.resInfo.groupId,
						0);

					cachedTex0Crc = tex0Crc;
				}

				prim->texture = cachedTex0;

				{
					uint32 tex1Crc = (uint32)(uint*)prim->texture2;
					if (tex1Crc == cachedTex1Crc)
					{
						prim->texture2 = cachedTex1;
					}
					else
					{
						cachedTex1 = (TBTexture *)bkFindResourceByCRC(
							EBRESTYPE_TEXTURE,
							tex1Crc,
							reg->setInfo.resInfo.packageId,
							(uint32)reg->setInfo.resInfo.groupId,
							0);

						prim->texture2 = cachedTex1;
						cachedTex1Crc = tex1Crc;
					}
				}

				prim = prim + 1; // sizeof(TBRegCellPrim) == 0x18
				i = i + 1;
			}
		}

		cell = (TBRegCell *)((ushort *)cell + 0x40);
		cellsRemaining = cellsRemaining - 1;
	}

	if ((int32)reg->hal.totalVertices > 0)
	{
		uint32 *col = ((uint32 *)prim) + 6; // +0x18
		int32 v = 0;

		while (v < (int32)reg->hal.totalVertices)
		{
			uint32 c = *col;

			*col =
				bRColLUT[(c >> 16) & 0xFF] |
				bGColLUT[(c >> 8) & 0xFF] |
				bAColLUT[(c >> 24) & 0xFF] |
				bBColLUT[c & 0xFF];

			col = col + 9; // 9 dwords per vertex
			v = v + 1;
		}
	}

	{
		TBVertexBuffer *vb = (TBVertexBuffer *)0;

		if (bdVertexBufferCreate(&vb, (int32)reg->hal.totalVertices, 0x152, 0x10, 1) != 0)
		{
			if (prim != (TBRegCellPrim *)0)
			{
				void *dst = bdVertexBufferLock(vb, 0);
				if (dst != 0)
				{
					uint32 *d = (uint32 *)dst;
					uint32 *s = (uint32 *)prim;

					uint32 count = (uint32)reg->hal.totalVertices * 9;
					uint32 k = 0;
					while (k < count)
					{
						d[k] = s[k];
						k = k + 1;
					}

					bdVertexBufferUnlock(vb);
				}
			}
		}

		reg->hal.vertBuffer = vb;
	}

	{
		int32 y = reg->yNoofCells + 1;
		int32 x = reg->xNoofCells + 1;
		int32 z = reg->zNoofCells + 1;

		uint32 bytes = (uint32)(y * x * z) * 4;

		reg->hal.outCodes = (int32 *)MALLOC(bytes);
	}
}

void bRegCellsDelete(TBSet *set)
{
	TBSetRegCells *reg = (TBSetRegCells *)set;

	bdVertexBufferDestroy((reg->hal).vertBuffer);
	(reg->hal).vertBuffer = NULL;
	bkHeapFree((reg->hal).outCodes);
	(reg->hal).outCodes = NULL;
}

void bRegCellsDraw(TBSet *setBase, int alpha)
{
	TBSetRegCells *set = (TBSetRegCells *)setBase;

	if (alpha == 1)
	{
		TBVector v;
		TBVector p000;
		TBVector p001;
		TBVector p011;
		TBVector p010;
		TBVector p100;
		TBVector p101;
		TBVector p111;
		TBVector p110;

		v[0] = set->xBoundMin;
		v[1] = set->yBoundMin;
		v[2] = set->zBoundMin;
		v[3] = 1.0f;
		bmMatMultiplyVector2(p000, bViewInfo.objectToProjection, v);

		v[0] = set->xBoundMin;
		v[1] = set->yBoundMin;
		v[2] = set->zBoundMax;
		v[3] = 1.0f;
		bmMatMultiplyVector2(p001, bViewInfo.objectToProjection, v);

		v[0] = set->xBoundMin;
		v[1] = set->yBoundMax;
		v[2] = set->zBoundMax;
		v[3] = 1.0f;
		bmMatMultiplyVector2(p011, bViewInfo.objectToProjection, v);

		v[0] = set->xBoundMin;
		v[1] = set->yBoundMax;
		v[2] = set->zBoundMin;
		v[3] = 1.0f;
		bmMatMultiplyVector2(p010, bViewInfo.objectToProjection, v);

		v[0] = set->xBoundMax;
		v[1] = set->yBoundMin;
		v[2] = set->zBoundMin;
		v[3] = 1.0f;
		bmMatMultiplyVector2(p100, bViewInfo.objectToProjection, v);

		v[0] = set->xBoundMax;
		v[1] = set->yBoundMin;
		v[2] = set->zBoundMax;
		v[3] = 1.0f;
		bmMatMultiplyVector2(p101, bViewInfo.objectToProjection, v);

		v[0] = set->xBoundMax;
		v[1] = set->yBoundMax;
		v[2] = set->zBoundMax;
		v[3] = 1.0f;
		bmMatMultiplyVector2(p111, bViewInfo.objectToProjection, v);

		v[0] = set->xBoundMax;
		v[1] = set->yBoundMax;
		v[2] = set->zBoundMin;
		v[3] = 1.0f;
		bmMatMultiplyVector2(p110, bViewInfo.objectToProjection, v);

		float invX = 1.0f / (float)set->xNoofCells;
		float invY = 1.0f / (float)set->yNoofCells;
		float invZ = 1.0f / (float)set->zNoofCells;

		TBVector stepX000;
		TBVector stepX010;
		TBVector stepX001;
		TBVector stepX011;

		stepX000[0] = (p100[0] - p000[0]) * invX;
		stepX000[1] = (p100[1] - p000[1]) * invX;
		stepX000[2] = (p100[2] - p000[2]) * invX;
		stepX000[3] = (p100[3] - p000[3]) * invX;

		stepX010[0] = (p110[0] - p010[0]) * invX;
		stepX010[1] = (p110[1] - p010[1]) * invX;
		stepX010[2] = (p110[2] - p010[2]) * invX;
		stepX010[3] = (p110[3] - p010[3]) * invX;

		stepX001[0] = (p101[0] - p001[0]) * invX;
		stepX001[1] = (p101[1] - p001[1]) * invX;
		stepX001[2] = (p101[2] - p001[2]) * invX;
		stepX001[3] = (p101[3] - p001[3]) * invX;

		stepX011[0] = (p111[0] - p011[0]) * invX;
		stepX011[1] = (p111[1] - p011[1]) * invX;
		stepX011[2] = (p111[2] - p011[2]) * invX;
		stepX011[3] = (p111[3] - p011[3]) * invX;

		int32 x = 0;
		int32 *outCodes = set->hal.outCodes;

		if (set->xNoofCells >= 0)
		{
			do
			{
				TBVector x_p000;
				TBVector x_p001;
				TBVector x_p011;
				TBVector x_p010;

				x_p000[0] = p000[0];
				x_p000[1] = p000[1];
				x_p000[2] = p000[2];
				x_p000[3] = p000[3];

				x_p001[0] = p001[0];
				x_p001[1] = p001[1];
				x_p001[2] = p001[2];
				x_p001[3] = p001[3];

				x_p011[0] = p011[0];
				x_p011[1] = p011[1];
				x_p011[2] = p011[2];
				x_p011[3] = p011[3];

				x_p010[0] = p010[0];
				x_p010[1] = p010[1];
				x_p010[2] = p010[2];
				x_p010[3] = p010[3];

				TBVector stepY_z0;
				TBVector stepY_z1;

				stepY_z0[0] = (x_p010[0] - x_p000[0]) * invY;
				stepY_z0[1] = (x_p010[1] - x_p000[1]) * invY;
				stepY_z0[2] = (x_p010[2] - x_p000[2]) * invY;
				stepY_z0[3] = (x_p010[3] - x_p000[3]) * invY;

				stepY_z1[0] = (x_p011[0] - x_p001[0]) * invY;
				stepY_z1[1] = (x_p011[1] - x_p001[1]) * invY;
				stepY_z1[2] = (x_p011[2] - x_p001[2]) * invY;
				stepY_z1[3] = (x_p011[3] - x_p001[3]) * invY;

				int32 y = 0;

				if (set->yNoofCells >= 0)
				{
					do
					{
						TBVector y_p000;
						TBVector y_p001;

						y_p000[0] = x_p000[0];
						y_p000[1] = x_p000[1];
						y_p000[2] = x_p000[2];
						y_p000[3] = x_p000[3];

						y_p001[0] = x_p001[0];
						y_p001[1] = x_p001[1];
						y_p001[2] = x_p001[2];
						y_p001[3] = x_p001[3];

						TBVector stepZ;

						stepZ[0] = (y_p001[0] - y_p000[0]) * invZ;
						stepZ[1] = (y_p001[1] - y_p000[1]) * invZ;
						stepZ[2] = (y_p001[2] - y_p000[2]) * invZ;
						stepZ[3] = (y_p001[3] - y_p000[3]) * invZ;

						int32 z = 0;

						if (set->zNoofCells >= 0)
						{
							do
							{
								uint32 code = 0;

								float w = y_p000[3];
								float negW = -w;

								if (y_p000[0] < negW)
									code |= 0x01;
								if (w <= y_p000[0])
									code |= 0x02;

								if (y_p000[1] < negW)
									code |= 0x04;
								if (w <= y_p000[1])
									code |= 0x08;

								if (y_p000[2] < 0.0f)
									code |= 0x10;
								if (w <= y_p000[2])
									code |= 0x20;

								*outCodes = (int32)code;
								outCodes = outCodes + 1;

								y_p000[0] = y_p000[0] + stepZ[0];
								y_p000[1] = y_p000[1] + stepZ[1];
								y_p000[2] = y_p000[2] + stepZ[2];
								y_p000[3] = y_p000[3] + stepZ[3];

								z = z + 1;
							} while (z <= set->zNoofCells);
						}

						x_p000[0] = x_p000[0] + stepY_z0[0];
						x_p000[1] = x_p000[1] + stepY_z0[1];
						x_p000[2] = x_p000[2] + stepY_z0[2];
						x_p000[3] = x_p000[3] + stepY_z0[3];

						x_p001[0] = x_p001[0] + stepY_z1[0];
						x_p001[1] = x_p001[1] + stepY_z1[1];
						x_p001[2] = x_p001[2] + stepY_z1[2];
						x_p001[3] = x_p001[3] + stepY_z1[3];

						y = y + 1;
					} while (y <= set->yNoofCells);
				}

				p000[0] = p000[0] + stepX000[0];
				p000[1] = p000[1] + stepX000[1];
				p000[2] = p000[2] + stepX000[2];
				p000[3] = p000[3] + stepX000[3];

				p010[0] = p010[0] + stepX010[0];
				p010[1] = p010[1] + stepX010[1];
				p010[2] = p010[2] + stepX010[2];
				p010[3] = p010[3] + stepX010[3];

				p001[0] = p001[0] + stepX001[0];
				p001[1] = p001[1] + stepX001[1];
				p001[2] = p001[2] + stepX001[2];
				p001[3] = p001[3] + stepX001[3];

				p011[0] = p011[0] + stepX011[0];
				p011[1] = p011[1] + stepX011[1];
				p011[2] = p011[2] + stepX011[2];
				p011[3] = p011[3] + stepX011[3];

				x = x + 1;
			} while (x <= set->xNoofCells);
		}
	}

	IDirect3DDevice8 *dev = bDisplayInfo.d3dDevice;

	if (bGlobalActorAlphaScale != 0x100)
	{
		dev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		dev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
		dev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		dev->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
		dev->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	}

	if (alpha == 1)
	{
		if ((bRenderState.renderState[BDRENDERSTATE_ALPHAENABLE][0] != 0) && (bGlobalActorAlphaScale == 0x100))
		{
			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, 0);
		}
	}
	else if (alpha == 4)
	{
		if (bRenderState.renderState[BDRENDERSTATE_ALPHAENABLE][0] == 0)
		{
			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, 1);
		}
	}

	int32 cellsDrawn = 0;

	int32 zCells = set->zNoofCells;
	int32 yCells = set->yNoofCells;
	int32 xCells = set->xNoofCells;

	int32 yStride = zCells + 1;
	int32 xStride = (yCells + 1) * (zCells + 1);

	uint32 mipFilter = ((set->flags & REGCELLSFLAG_TRILINEAR) != 0) + 1;
	dev->SetTextureStageState(0, D3DTSS_MIPFILTER, mipFilter);

	bSetVertexShader(0x152, set->hal.vertBuffer);

	TBRegCell *cell = set->cells;
	uint32 *outPtr = (uint32 *)set->hal.outCodes;

	TBTexture *curTex0 = (TBTexture *)0;
	TBTexture *curTex1 = (TBTexture *)0;

	uint32 cachedWrapU = 0xFFFFFFFFu;
	uint32 cachedWrapV = 0xFFFFFFFFu;

	if (xCells > 0)
	{
		int32 x = 0;
		while (x < xCells)
		{
			int32 y = 0;
			while (y < yCells)
			{
				int32 z = 0;
				while (z < zCells)
				{
					uint32 common =
						outPtr[xStride + zCells + 2] &
						outPtr[xStride + zCells + 1] &
						outPtr[zCells + 2] &
						outPtr[zCells + 1] &
						outPtr[xStride + 1] &
						outPtr[xStride] &
						outPtr[1];

					if ((outPtr[0] & common) == 0)
					{
						cellsDrawn = cellsDrawn + 1;

						int32 startVert = 0;
						TBRegCellPrim *prim = (TBRegCellPrim *)0;
						int32 primCount = 0;

						if (alpha == 1)
						{
							startVert = cell->firstVert;
							prim = cell->prims;
							primCount = (int32)cell->noofSolidPrims;
						}
						else if (alpha == 2)
						{
							startVert = cell->firstCKeyVert;
							prim = cell->prims + cell->noofSolidPrims;
							primCount = (int32)cell->noofCKeyPrims;
						}
						else if (alpha == 4)
						{
							startVert = cell->firstAlphaVert;
							prim = cell->prims + cell->noofSolidPrims + cell->noofCKeyPrims;
							primCount = (int32)cell->noofAlphaPrims;
							dev->SetRenderState(D3DRS_ZWRITEENABLE, 0);
						}

						if (primCount > 0)
						{
							int32 i = 0;
							while (i < primCount)
							{
								if (prim->texture != curTex0)
								{
									curTex0 = prim->texture;
									bdSetTexture(0, curTex0);
								}

								if (prim->texture2 != curTex1)
								{
									curTex1 = prim->texture2;
									bdSetTexture(1, curTex1);
								}

								if (bRenderState.textureEnable[0] != 0)
								{
									uint32 wrapU = prim->flags & 1;
									uint32 wrapV = prim->flags & 2;

									if (wrapU != cachedWrapU)
									{
										cachedWrapU = wrapU;
										uint32 valU = (wrapU == 0) * 2 + 1;
										dev->SetTextureStageState(0, D3DTSS_ADDRESSU, valU);
									}

									if (wrapV != cachedWrapV)
									{
										cachedWrapV = wrapV;
										uint32 valV = (wrapV == 0) * 2 + 1;
										dev->SetTextureStageState(0, D3DTSS_ADDRESSV, valV);
									}
								}

								dev->DrawPrimitive((D3DPRIMITIVETYPE)prim->type, startVert, (uint32)prim->noofPrimVerts);

								startVert = startVert + prim->noofVerts;
								prim = prim + 1;
								i = i + 1;
							}
						}

						if (alpha == 4)
						{
							dev->SetRenderState(D3DRS_ZWRITEENABLE, 1);
						}
					}

					cell = cell + 1;
					outPtr = outPtr + 1;
					z = z + 1;
				}

				outPtr = outPtr + 1;
				y = y + 1;
			}

			outPtr = outPtr + yStride;
			x = x + 1;
		}
	}

	int32 denom = set->zNoofCells * set->xNoofCells * set->yNoofCells;
	bCellsDrawnPerc = (cellsDrawn * 100) / denom;

	if (alpha == 1)
	{
		if ((bRenderState.renderState[BDRENDERSTATE_ALPHAENABLE][0] != 0) && (bGlobalActorAlphaScale == 0x100))
		{
			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, 1);
		}
	}
	else if (alpha == 4)
	{
		if (bRenderState.renderState[BDRENDERSTATE_ALPHAENABLE][0] == 0)
		{
			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, 0);
		}
	}

	if (bGlobalActorAlphaScale != 0x100)
	{
		dev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		dev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	}
}

TBSetFixupFn bFixupSet[BSETMETHOD_NOOF] = { bRegCellsFixUp };
TBSetDeleteFn baDeleteSet[BSETMETHOD_NOOF] = { bRegCellsDelete };
TBSetDrawFn bDrawSet[BSETMETHOD_NOOF] = { bRegCellsDraw };

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bInitSet
	Purpose : initialise the set module
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/
int bInitSet()
{
    const int ok = bInitSetPlatformSpecific();
    return ok != 0;
}

/*	--------------------------------------------------------------------------------
	Function : bShutdownSet
	Purpose : shutdown the set module
	Parameters : 
	Returns : 
	Info : 
*/
void bShutdownSet()
{
    return; // empty, confirmed
}

/*	--------------------------------------------------------------------------------
	Function : bLoadSetByCRC
	Purpose : load a set from storage
	Parameters : package index, crc
	Returns : ptr to set or NULL for failure
	Info : 
*/

TBSet *bLoadSetByCRC(TBPackageIndex *index, uint32 crc)
{
    TBSet *set;
    float **pDataStream;
    int32 i;

    set = (TBSet *)bkLoadFileByCRC(index, crc, (uchar *)0, (int *)0, (TBFileTagInfo *)0, 0);
    if (set == NULL) {
        return NULL;
    }

    // Copy the package ID (raw 32-bit value).
    set->resInfo.packageId32 = *(uint32 *)&index->id;

    // Fix up storedGeometry.dataStreams[4] from file-relative offsets to pointers.
    pDataStream = set->storedGeometry.dataStreams;
    for (i = 0; i < 4; i++) {
        if (pDataStream[i] != NULL) {
            pDataStream[i] = (float *)((int)pDataStream[i] + (int)set);
        }
    }

    // Dispatch to the set-method specific fixup.
    (*bFixupSet[set->method])(set);
    return set;
}

/*	--------------------------------------------------------------------------------
	Function : bDeleteSet
	Purpose : delete a set
	Parameters : actor
	Returns : 
	Info : 
*/
void bDeleteSet(TBSet *actor)
{
    baDeleteSet[actor->method](actor);

	if ((actor->resInfo).packageId32 >= 0) bkHeapFree(actor);
}

/*	--------------------------------------------------------------------------------
	Function : baDrawSet
	Purpose : draw a set
	Parameters : set, alpha
	Returns : 
	Info : 
*/

void baDrawSet(TBSet *set, int alpha)
{
	bDrawSet[set->method](set, alpha);
}

/*	--------------------------------------------------------------------------------
	Function : baGetSetGeometry
	Purpose : retrieve the stored geometry from a set
	Parameters : set, stream number (0..3), ptr to fill with #vertices (or NULL), ptr to fill with #tris (or NULL)
	Returns : ptr to stream data
	Info : 
*/

void *baGetSetGeometry(TBSet *set, int stream, int *noofVertices, int *noofTris)
{
  if (noofVertices != (int *)0x0) {
    *noofVertices = (set->storedGeometry).noofVertices;
  }
  if (noofTris != (int *)0x0) {
    *noofTris = (set->storedGeometry).noofTriangles;
  }
  return (set->storedGeometry).dataStreams[stream] + 0x17;
}