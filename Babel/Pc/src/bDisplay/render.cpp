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
    return;
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
    return;
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
    return;
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
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdRestoreRenderStates
	Purpose : restore render states
	Parameters : saveBlock
	Returns : 
	Info : 
*/
void bdRestoreRenderStates(TBSavedRenderStates *saveBlock)
{
    return;
}
