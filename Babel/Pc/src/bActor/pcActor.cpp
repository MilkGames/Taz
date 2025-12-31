// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcActor.cpp
//   Purpose : platform specific primary actor functions
// Component : Xbox Actor
//   Comment : EXPECTED TO BE THE SAME AS xbActor.h, DIRECT REWRITE!
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Globals

int bForceHardSkinning = 0;

// ********************************************************************************
// Locals

typedef struct _TSoftSkinRenderModeState
{
    int tex0;     // [0]
    int tex1;     // [4]
    int variant;  // [8]
} TSoftSkinRenderModeState;

TSoftSkinRenderModeState _softSkinRenderModeState[BACTRENDERMODE_NOOF] =
{
    /* [0] */ { 0, 0, 0 },
    /* [1] */ { 1, 1, 0 },
    /* [2] */ { 1, 0, 0 },
    /* [3] */ { 0, 0, 0 },
    /* [4] */ { 0, 0, 1 },
    /* [5] */ { 0, 0, 0 },
    /* [6] */ { 1, 1, 0 },
    /* [7] */ { 1, 1, 0 },
    /* [8] */ { 0, 0, 0 },
    /* [9] */ { 0, 0, 0 },
};

TBDrawSoftSkinCallback bSoftSkinDrawUserCallback = NULL;

// ********************************************************************************
// Local Functions

void DrawSoftSkinBatches(TBActorInstance *actorInstance, TSoftSkinRenderModeState *modeState)
{
	TBSoftSkinBatch		*batch;
	TBSoftSkinPrim		*prim;
	uint32				baseVertex;
	int					batchIndex;
	int					primIndex;
	uchar				useWrapStates;
	uint32				cachedU;
	uint32				cachedV;

	cachedU = 0xffffffff;
	cachedV = 0xffffffff;

	// temp debug
	//bkPrintf("SoftSkinBatch: inst=%p actor=%p mode=%p tex01=[%d,%d] var=%d batches=%p prims=%p nb=%d\n",
	//actorInstance,
	//actorInstance->actor,
	//modeState,
	//modeState->tex0, modeState->tex1,
	//modeState->variant,
	//actorInstance->actor->softSkin.batches,
	//actorInstance->actor->softSkin.primitives,
	//actorInstance->actor->softSkin.noofBatches);

	useWrapStates = 0;
	if ((modeState->tex0 != 0) || (modeState->tex1 != 0))
	{
		useWrapStates = 1;
	}

	batch = actorInstance->actor->softSkin.batches;
	prim = actorInstance->actor->softSkin.primitives;

	baseVertex = 0;
	batchIndex = 0;

	if (actorInstance->actor->softSkin.noofBatches != 0)
	{
		// temp debug
		//bkPrintf("SoftSkinBatch: b=%d noPrims=%d tex1=%p tex2=%p\n",
	//batchIndex,
	//batch->noofPrims,
	//batch->texture1,
	//batch->texture2);
		do
		{
			if (modeState->tex0 != 0)
			{
				bdSetTexture(0, batch->texture1);
			}

			if (modeState->tex1 != 0)
			{
				bdSetTexture(1, batch->texture2);
			}

			if ((modeState->tex0 != 0) && (batch->texture1 != NULL))
			{
				uchar abm;

				abm = batch->texture1->alphaBlendMode;
				if (abm != 0xff)
				{
					bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, (uint32)abm, 0);
				}
			}

			primIndex = 0;
			if (0 < batch->noofPrims)
			{
				do
				{
					ushort vcount;

					if ((modeState->variant == 0) || ((prim->flags & 4) == 0))
					{
						if ((useWrapStates != 0) && (bRenderState.renderStateLock[2] == 0))
						{
							uint32 u;

							u = (uint32)(prim->flags & 1);
							if (u != cachedU)
							{
								bDisplayInfo.d3dDevice->SetTextureStageState(
									0,
									D3DTSS_ADDRESSU,
									((u == 0) ? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP)
								);
								cachedU = u;
							}

							u = (uint32)(prim->flags & 2);
							if (u != cachedV)
							{
								bDisplayInfo.d3dDevice->SetTextureStageState(
									0,
									D3DTSS_ADDRESSV,
									((u == 0) ? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP)
								);
								cachedV = u;
							}
						}

						if (prim->noofMatrices != 0)
						{
							int m;
							int reg;

							m = 0;
							reg = 0;
							do
							{
								uchar idx;

								idx = prim->matrixIndices[m];
								bDisplayInfo.d3dDevice->SetVertexShaderConstant(
									reg,
									actorInstance->matrixPalette + ((uint32)idx * 12),
									3
								);

								m++;
								reg += 3;
							} while (m < (int)prim->noofMatrices);
						}

						bDisplayInfo.d3dDevice->DrawPrimitive(
							(D3DPRIMITIVETYPE)prim->primType,
							baseVertex,
							prim->noofDrawPrims
						);

						vcount = prim->noofVertices;
					}
					else
					{
						vcount = prim->noofVertices;
					}

					baseVertex += (uint32)vcount;
					prim++;
					primIndex++;
				} while (primIndex < batch->noofPrims);
			}

			batch++;
			batchIndex++;
		} while (batchIndex < (int)actorInstance->actor->softSkin.noofBatches);
	}

	bNoofSkinnedVertsProcessed += (int32)baseVertex;
}

static TBDrawSoftSkinCallback _drawFuncLUT[6] =
{
    DrawSoftSkinBatches,
    DrawSoftSkinBatches,
    DrawSoftSkinBatches,
    DrawSoftSkinBatches,
    DrawSoftSkinBatches,
    DrawSoftSkinBatches
}; // LOL, useful array

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bDrawSoftSkin
	Purpose : draw a soft-skinned character
	Parameters : actor instance ptr
	Returns : 
	Info : 
*/
void bDrawSoftSkin(struct _TBActorInstance *actorInstance, int alpha)
{
	float	matrix[16];
	int		stage;

	if ((alpha & 1) == 0)
	{
		return;
	}

	// temp debug
	//bkPrintf("SoftSkinDraw: inst=%p actor=%p alpha=0x%X rm=[%d,%d,%d] palette=%p shadow=%d texEn=[%d,%d] lock2=%d unk=%d\n",
	//actorInstance,
	//actorInstance->actor,
	//alpha,
	//actorInstance->renderMode[0], actorInstance->renderMode[1], actorInstance->renderMode[2],
	//actorInstance->matrixPalette,
	//bRenderState.shadowActive,
	//bRenderState.textureEnable[0], bRenderState.textureEnable[1],
	//bRenderState.renderStateLock[2],
	//bRenderState.UNKNOWN);

	bNoofSkinnedActorsDrawn++;

	bMatMultiplyTranspose(matrix, bViewInfo.worldToProjection, actorInstance->objectToWorld, 4);
	bDisplayInfo.d3dDevice->SetVertexShaderConstant(0x5c, matrix, 4);

	for (stage = 0; stage < 3; stage++)
	{
		EBActRenderMode				rm;
		TSoftSkinRenderModeState	*modeState;
		int							tex0;
		int							tex1;
		int							variant;
		TBDrawSoftSkinCallback		drawFunc;

		rm = actorInstance->renderMode[stage];
		if (rm == BACTRENDERMODE_NONE)
		{
			continue;
		}

		// temp debug
		//bkPrintf("SoftSkinDraw: stage=%d renderMode=%d\n",
	//stage,
	//actorInstance->renderMode[stage]);

		if (baShaderSelect(actorInstance, stage, NULL) == 0)
		{
			continue;
		}

		modeState = &_softSkinRenderModeState[rm];

		if (bSoftSkinDrawUserCallback != NULL)
		{
			drawFunc = bSoftSkinDrawUserCallback;
		}
		else
		{
			if (bRenderState.shadowActive != 0)
			{
				tex0 = 1;
			}
			else
			{
				if (bRenderState.textureEnable[0] != 0)
				{
					tex0 = modeState->tex0;
				}
				else
				{
					tex0 = 0;
				}
			}

			if (bRenderState.UNKNOWN != 0)
			{
				tex1 = 1;
			}
			else
			{
				if (((actorInstance->actor->flags & 0x10) == 0) && (bRenderState.textureEnable[1] != 0))
				{
					tex1 = modeState->tex1;
				}
				else
				{
					tex1 = 0;
				}
			}

			if (tex0 == 0)
			{
				bdSetTexture(0, NULL);
			}

			if (tex1 == 0)
			{
				bdSetTexture(1, NULL);
			}

			if ((modeState->variant == 0) && (bRenderState.renderStateLock[2] == 0))
			{
				variant = 0;
			}
			else
			{
				variant = 1;
			}

			drawFunc = _drawFuncLUT[variant + (tex1 + tex0) * 2];
		}

		drawFunc(actorInstance, modeState);
		baShaderDeselect(actorInstance, stage);
	}
}

/*	--------------------------------------------------------------------------------
	Function : bDrawSoftSkinCPU
	Purpose : draw a soft-skinned character via CPU
	Parameters : actor instance ptr
	Returns : 
	Info : 
*/
void bDrawSoftSkinCPU(struct _TBActorInstance *actorInstance, int alpha)
{
		bkPrintf("*** WARNING *** bDrawSoftSkinCPU was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baFlushVertexShaderCache
	Purpose : flush the vertex shader cache
	Parameters : 
	Returns : 
	Info : 
*/
void baFlushVertexShaderCache()
{
        bkPrintf("*** WARNING *** baFlushVertexShaderCache was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baOptimiseActorRenderModes
	Purpose : optimise the render mode setup for the gamecube
	Parameters : actor to optimise the render mode for
	Returns : passes
	Info : 
*/
int baOptimiseActorRenderModes(struct _TBActorInstance *actorInstance)
{
        bkPrintf("*** WARNING *** baOptimiseActorRenderModes was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}
