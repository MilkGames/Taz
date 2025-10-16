// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : CBFlyAroundCamera.cpp
//   Purpose : fly around camera class
// Component : Generic Camera
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

CBFlyAroundCamera::CBFlyAroundCamera() {}

CBFlyAroundCamera::~CBFlyAroundCamera() {}

void CBFlyAroundCamera::SetInfo(float x, float y, float z, float xRot, float yRot, float collisionRadius,
                                float speed, uint32 flags) 
{
        bkPrintf("*** WARNING *** CBFlyAroundCamera::SetInfo was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void CBFlyAroundCamera::SetPosition(float x, float y, float z)
{
        bkPrintf("*** WARNING *** CBFlyAroundCamera::SetPosition was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void CBFlyAroundCamera::SetOrientation(float xRot, float yRot, float zRot)
{
        bkPrintf("*** WARNING *** CBFlyAroundCamera::SetOrientation was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void CBFlyAroundCamera::SetTilt(float vZRot)
{
        bkPrintf("*** WARNING *** CBFlyAroundCamera::SetTilt was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void CBFlyAroundCamera::SetChannels(TBIInputChannel *chLeftRight, TBIInputChannel *chUpDown,
                                    TBIInputChannel *chForwardBack, TBIInputChannel *chStrafe,
                                    TBIInputChannel *chTurbo, TBIInputChannel *chSlow)
{
        bkPrintf("*** WARNING *** CBFlyAroundCamera::SetChannels was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void CBFlyAroundCamera::SetupAutoBindings(int padId)
{
        bkPrintf("*** WARNING *** CBFlyAroundCamera::SetupAutoBindings was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void CBFlyAroundCamera::Update(const float t)
{
        bkPrintf("*** WARNING *** CBFlyAroundCamera::Update was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}
