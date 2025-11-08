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

int stopSpam1 = 0;

// ********************************************************************************
// Helper Functions

void EvalCameraControl(TBVector outPos, float* outRoll, TBCameraInstanceControl* ctrl)
{
    switch (ctrl->ctrlMode) {
        case BCAMERACTRL_POSITION: {
            outPos[0] = ctrl->staticParms.x;
            outPos[1] = ctrl->staticParms.y;
            outPos[2] = ctrl->staticParms.z;
            *outRoll  = ctrl->staticParms.roll;
            return;
        }

        case BCAMERACTRL_TRACKING_POSITION: {
            const float* v = ctrl->vectorParms.vector;
            outPos[0] = v[0];
            outPos[1] = v[1];
            outPos[2] = v[2];
            *outRoll  = ctrl->vectorParms.roll;
            return;
        }

        case BCAMERACTRL_ACTOR: {
            TBVector local;  // (x,y,z,1)
            local[0] = ctrl->actorParms.x;
            local[1] = ctrl->actorParms.y;
            local[2] = ctrl->actorParms.z;
            local[3] = 1.0f;

            TBVector tmp;
            if (ctrl->actorParms.nodeInstance) {
                bmMatMultiplyVector2(tmp, ctrl->actorParms.nodeInstance->nodeToLocalWorld, local);
            } else {
                tmp[0] = local[0]; tmp[1] = local[1]; tmp[2] = local[2]; tmp[3] = 1.0f;
            }

            // Actor instance begins with its object->world matrix
            // Cast to pointer-to-matrix and dereference to pass TBMatrix (array) by value
            bmMatMultiplyVector2(outPos, *(const TBMatrix*)ctrl->actorParms.actorInstance, tmp);
            *outRoll = ctrl->actorParms.roll;
            return;
        }

        case BCAMERACTRL_CUSTOM: {
            ctrl->customParms.callback(outPos, outRoll, ctrl->customParms.context);
            return;
        }
    }

    // No default: rotation-only modes are handled by bUpdateViewFromCamera, not here.
}


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
    if (!bCurrentCamera) {
        return 0;
    }

    TBVector from, at;
    float rollFrom = 0.0f;
    float rollAt   = 0.0f;

    // Decide orientation path based on TARGET control mode.
    switch (bCurrentCamera->target.ctrlMode) {
        // Target provides a point to look at (POSITION / VECTOR / ACTOR / CUSTOM)
        case BCAMERACTRL_POSITION:
        case BCAMERACTRL_TRACKING_POSITION:
        case BCAMERACTRL_ACTOR:
        case BCAMERACTRL_CUSTOM: {
            // Evaluate target (at) first to capture its roll for the look-at roll
            EvalCameraControl(at,   &rollAt,   &bCurrentCamera->target);
            // Then evaluate source (from)
            EvalCameraControl(from, &rollFrom, &bCurrentCamera->source);
            // Apply view
            bdSetViewLookAt(from, at, rollAt);
            break;
        }

        // Quaternion orientation
        case BCAMERACTRL_QUATERNION: {
            EvalCameraControl(from, &rollFrom, &bCurrentCamera->source);
            bdSetViewPosition(from[0], from[1], from[2]);
            bmQuatToMatrix(bViewInfo.rotMatrix, bCurrentCamera->target.quaternionParms.quaternion);
            break;
        }
        case BCAMERACTRL_TRACKING_QUATERNION: {
            EvalCameraControl(from, &rollFrom, &bCurrentCamera->source);
            bdSetViewPosition(from[0], from[1], from[2]);
            // Tracking quaternion via pointer
            bmQuatToMatrix(bViewInfo.rotMatrix, *(const TBQuaternion*)bCurrentCamera->target.tQuaternionParms.quaternion);
            break;
        }

        // Euler orientation
        case BCAMERACTRL_EULER: {
            EvalCameraControl(from, &rollFrom, &bCurrentCamera->source);
            bdSetViewPosition(from[0], from[1], from[2]);
            const float xr = bCurrentCamera->target.eulerParms.xr;
            const float yr = bCurrentCamera->target.eulerParms.yr;
            const float zr = bCurrentCamera->target.eulerParms.zr;
            const EBViewportRotationOrder order = bCurrentCamera->target.eulerParms.order;
            bdSetViewOrientation(xr, yr, zr, order);
            break;
        }
        case BCAMERACTRL_TRACKING_EULER: {
            EvalCameraControl(from, &rollFrom, &bCurrentCamera->source);
            bdSetViewPosition(from[0], from[1], from[2]);
            const float* ang = bCurrentCamera->target.tEulerParms.angle;
            const EBViewportRotationOrder order = bCurrentCamera->target.tEulerParms.order;
            bdSetViewOrientation(ang[0], ang[1], ang[2], order);
            break;
        }

        default: {
            // No orientation update path; leave as-is.
            break;
        }
    }

    // Sync FOV if needed
    if ((bCurrentCamera->xFov != bViewInfo.xFov) || (bCurrentCamera->yFov != bViewInfo.yFov)) {
        bdSetFOV(bCurrentCamera->xFov, bCurrentCamera->yFov);
    }
    // Sync near/far if needed
    if ((bCurrentCamera->nearClip != bViewInfo.nearClip) ||
        (bCurrentCamera->farClip  != bViewInfo.farClip)) {
        bdSetViewClipPlanes(bCurrentCamera->nearClip, bCurrentCamera->farClip);
    }

    return 1;
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
