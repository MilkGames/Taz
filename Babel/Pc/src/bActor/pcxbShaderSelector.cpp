// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcxbShaderSelector.cpp
//   Purpose : platform specific shader selector functions
// Component : Xbox Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bInitShaderSelector
	Purpose : initialise shader selector
	Parameters : 
	Returns : 
	Info : 
*/
void bInitShaderSelector(void)
{
        bkPrintf("*** WARNING *** bInitShaderSelector was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bShutdownShaderSelector
	Purpose : shutdown shader selector
	Parameters : 
	Returns : 
	Info : 
*/
void bShutdownShaderSelector(void)
{
        bkPrintf("*** WARNING *** bShutdownShaderSelector was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baShaderSelect
	Purpose : shader select
	Parameters : instance, modeNum, mesh
	Returns : OK/FAIL
	Info : 
*/
int baShaderSelect(TBActorInstance * const instance, const int modeNum, TBMesh *mesh)
{
        bkPrintf("*** WARNING *** baShaderSelect was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : baShaderDeselect
	Purpose : shader deselect
	Parameters : instance, modeNum
	Returns : 
	Info : 
*/
void baShaderDeselect(TBActorInstance * const instance, const int modeNum)
{
        bkPrintf("*** WARNING *** baShaderDeselect was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baShaderEvaluateLightingState
	Purpose : shader evaluate lighting state
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/
int baShaderEvaluateLightingState(void)
{
        bkPrintf("*** WARNING *** baShaderEvaluateLightingState was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}
