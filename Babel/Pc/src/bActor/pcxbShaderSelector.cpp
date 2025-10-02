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
    return 0;
}
