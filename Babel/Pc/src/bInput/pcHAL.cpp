// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcHAL.cpp
//   Purpose : Defines constants for max input devices, max axes, max buttons, max POVs, max sliders, max force feedback axes, max force feedback effects, and max force feedback axes per effect
// Component : Babel Input
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bInitInputHAL
	Purpose : initialise the input module
	Parameters : bkInit flags
	Returns : 
	Info : 
*/

void bInitInputHAL(uint32 flags) 
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bShutdownInputHAL
	Purpose : shutdown the input module
	Parameters : 
	Returns : 
	Info : 
*/

void bShutdownInputHAL()
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bReadPhysicalInputDevices
	Purpose : read the physical input devices
	Parameters : 
	Returns : 
	Info : 
*/

void bReadPhysicalInputDevices()
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bReadInput
	Purpose : return the current value of a specific input
	Parameters : input ID, value behaviour, ptr to receive value
	Returns : OK / FAIL
	Info : 
*/

int bReadInput(int inputID, int behaviour, int *valuePtr)
{
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bUpdateRumble
	Purpose : Ensure the rumble values on the pads are set correctly
	Parameters : 
	Returns : 
	Info : called ten times a second
*/

void bUpdateRumble()
{
    return;
}