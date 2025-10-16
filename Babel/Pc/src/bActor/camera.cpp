// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : camera.cpp
//   Purpose : Defines camera control modes, flags, and structures. Declares functions for camera initialization, shutdown, view updates, and managing camera instances
// Component : Generic Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bInitCamera
	Purpose : initialise the camera submodule
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/
int bInitCamera()
{
        bkPrintf("*** WARNING *** bInitCamera was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bShutdownCamera
	Purpose : shutdown the camera submodule
	Parameters : 
	Returns : 
	Info : 
*/
void bShutdownCamera()
{
        bkPrintf("*** WARNING *** bShutdownCamera was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bUpdateViewFromCamera
	Purpose : update the view properties from the current camera (if there is one)
	Parameters : 
	Returns : 1=set, 0=no camera set
	Info : 
*/
int bUpdateViewFromCamera()
{
        bkPrintf("*** WARNING *** bUpdateViewFromCamera was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : baDeleteCameraInstance
	Purpose : delete a camera instance
	Parameters : camera instance
	Returns : 
	Info : 
*/
void baDeleteCameraInstance(TBCameraInstance *camera)
{
        bkPrintf("*** WARNING *** baDeleteCameraInstance was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baSetCameraInstanceSource
	Purpose : set camera instance source
	Parameters : camera instance, ...
	Returns : 
	Info : 
*/
void baSetCameraInstanceSource(TBCameraInstance *camera, ...)
{
        bkPrintf("*** WARNING *** baSetCameraInstanceSource was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baSetCameraInstanceTarget
	Purpose : set camera instance target
	Parameters : camera instance, ...
	Returns : 
	Info : 
*/
void baSetCameraInstanceTarget(TBCameraInstance *camera, ...)
{
        bkPrintf("*** WARNING *** baSetCameraInstanceTarget was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}
