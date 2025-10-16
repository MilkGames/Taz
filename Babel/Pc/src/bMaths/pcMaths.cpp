// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcMaths.h
//   Purpose : general accelerated maths functions
// Component : Xbox Maths
//   Comment : EXPECTED TO BE THE SAME AS xbMaths.h, DIRECT REWRITE!
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Globals

// MG: TEMP LINKING CODE
uint32 bHoldRand = 0x1234ABCD;

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bmSRand
	Purpose : Set the seed for the random number generator
	Parameters : seed
	Returns : 
	Info :
*/

void bmSRand(uint32 seed)
{
        bkPrintf("*** WARNING *** bmSRand was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bmACosApprox
	Purpose : calculate the approximate arccosine of a number
	Parameters : floating point number
	Returns : approximate arccosine of number
	Info : 
*/

// defined

/*	--------------------------------------------------------------------------------
	Function : bmASinApprox
	Purpose : calculate the approximate arcsine of a number
	Parameters : floating point number
	Returns : approximate arcsine of number
	Info : 
*/

// defined

/*	--------------------------------------------------------------------------------
	Function : bmFloor
	Purpose : floating-point value representing the largest integer that is less than or equal to x
	Parameters : value
	Returns : floored value
	Info : 
*/

// defined

/*	--------------------------------------------------------------------------------
	Function : bmSqrtApprox
	Purpose : Approximate square root of a number
	Parameters : floating point number
	Returns : approximate square root of number
	Info : 
*/

float bmSqrtApprox(float x)
{
        bkPrintf("*** WARNING *** bmSqrtApprox was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}