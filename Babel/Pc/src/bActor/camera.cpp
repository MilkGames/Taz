// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : camera.cpp
//   Purpose : Defines camera control modes, flags, and structures. Declares functions for camera initialization, shutdown, view updates, and managing camera instances
// Component : Generic Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Locals

TBCameraInstance	bCameraList;
TBCameraInstance   *bCurrentCamera;

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
	bCameraList.prev = &bCameraList;
	bCameraList.next = &bCameraList;
	return OK;
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
    while (bCameraList.next != &bCameraList) {
        baDeleteCameraInstance(bCameraList.next);
    }
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
    if (camera == NULL) {
		// deleting all cameras
        while (bCameraList.next != &bCameraList) {
            baDeleteCameraInstance(bCameraList.next);
        }
        return;
    }

    // if this is the current camera, clear the current pointer
    if (camera == bCurrentCamera) {
        bCurrentCamera = NULL;
    }

    // unlink from the global doubly-linked list
    camera->next->prev = camera->prev;
    camera->prev->next = camera->next;

    // only free if it was dynamically allocated
    if (camera->flags & BCAMERAFLAG_DYNAMIC) {
        bkHeapFree(camera);
    }
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
