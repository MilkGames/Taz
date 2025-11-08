// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : render.cpp
//   Purpose : Defines display flags, render state constants, texture filters, texture wrap modes, Z testing modes, clear modes, alpha blending modes, culling modes, frame write flags, and render target creation/setting flags
// Component : Babel Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bSetInitialRenderStates
	Purpose : set initial render states
	Parameters : stages
	Returns : 
	Info : 
*/
void bSetInitialRenderStates(int stages)
{
    // Core defaults for the engine's state-cache
    bdSetRenderState(BDRENDERSTATE_ANTIALIAS,         0,   0);
    bdSetRenderState(BDRENDERSTATE_TEXTUREFILTER,     2,   0);
    bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP,       3,   0);
    bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE,    0,   0);
    bdSetRenderState(BDRENDERSTATE_LIGHTING,          0,   0);
    bdSetRenderState(BDRENDERSTATE_BACKFACECULL,      0,   0);
    bdSetRenderState(BDRENDERSTATE_ZWRITE,            1,   0);
    bdSetRenderState(BDRENDERSTATE_ZTEST,             3,   0);
    bdSetRenderState(BDRENDERSTATE_CLEAR,          0xff,   0);
    bdSetRenderState(BDRENDERSTATE_DISTANCEFOG,       0,   0);
    bdSetRenderState(BDRENDERSTATE_ALPHAENABLE,       0,   0);
    bdSetRenderState(BDRENDERSTATE_FRAMEWRITE,        1,   0);
    bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,     0,   0);

    // Enable first N texture stages (PC: N == 2; Xbox == 4)
    for (uint32 stage = 0; stage < (uint32)stages; ++stage) {
        bdSetRenderState(BDRENDERSTATE_TEXTUREENABLE, stage, 1);
    }
}

/*	--------------------------------------------------------------------------------
	Function : bdGetRenderState
	Purpose : get render state
	Parameters : renderState, value2
	Returns : value
	Info : 
*/
uint32 bdGetRenderState(uint32 renderState, uint32 *value2)
{
        bkPrintf("*** WARNING *** bdGetRenderState was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bdPushRenderState
	Purpose : push render state
	Parameters : state, value1, value2
	Returns : 
	Info : 
*/
void bdPushRenderState(uint32 state, uint32 value1, uint32 value2)
{
    // Read current stack depth for this render state.
    int depth = bRenderState.stateStackDepth[state];

    // Save current pair when -1 < depth < 15 (i.e., room in the stack).
    if (depth >= 0 && depth < (BRENDERSTATE_STACKDEPTH - 1)) {
        bRenderState.stateStack[state][depth][0] = bRenderState.renderState[state][0];
        bRenderState.stateStack[state][depth][1] = bRenderState.renderState[state][1];
    }

    // Increment depth unconditionally (INC).
    bRenderState.stateStackDepth[state] = depth + 1;

    // Apply the new state.
    bdSetRenderState(state, value1, value2);
}

/*	--------------------------------------------------------------------------------
	Function : bdPopRenderState
	Purpose : pop render state
	Parameters : state
	Returns : 
	Info : 
*/
void bdPopRenderState(uint32 state)
{
    // Old depth before pop
    int oldDepth = bRenderState.stateStackDepth[state];
    // DEC depth and store back
    int depth = oldDepth - 1;
    bRenderState.stateStackDepth[state] = depth;

    // If 0 <= depth < 15, restore from the saved entry at index (oldDepth - 1)
    if (depth >= 0 && depth < (BRENDERSTATE_STACKDEPTH - 1)) {
        uint32 v1 = bRenderState.stateStack[state][depth][0];
        uint32 v2 = bRenderState.stateStack[state][depth][1];
        bdSetRenderState(state, v1, v2);
    }
}

/*	--------------------------------------------------------------------------------
	Function : bdSaveRenderStates
	Purpose : save render states
	Parameters : saveBlock
	Returns : 
	Info : 
*/
void bdSaveRenderStates(TBSavedRenderStates *saveBlock)
{
    memcpy(saveBlock->states, bRenderState.renderState, sizeof(saveBlock->states));
}

/*	--------------------------------------------------------------------------------
	Function : bdRestoreRenderStates
	Purpose : restore render states
	Parameters : saveBlock
	Returns : 
	Info : 
*/
void bdRestoreRenderStates(TBSavedRenderStates* saveBlock)
{
    for (uint32 state = 0; state < BDRENDERSTATE_NOOF; ++state)
    {
        const uint32 v1 = saveBlock->states[state][0];
        const uint32 v2 = saveBlock->states[state][1];

        if (bRenderState.renderState[state][0] != v1 ||
            bRenderState.renderState[state][1] != v2)
        {
            bdSetRenderState(state, v1, v2);
        }
    }
}
