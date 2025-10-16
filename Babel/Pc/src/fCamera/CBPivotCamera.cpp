// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : CBPivotCamera.cpp
//   Purpose : pivot around a point camera class
// Component : Generic Camera
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

CBPivotCamera::CBPivotCamera() {}

CBPivotCamera::~CBPivotCamera() {}

void CBPivotCamera::SetInfo(float x, float y, float z, float xRot, float yRot,
                            float collisionRadius, float speed, uint32 flags)
{
        bkPrintf("*** WARNING *** CBPivotCamera::SetInfo was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void CBPivotCamera::SetFocus(float x, float y, float z)
{
        bkPrintf("*** WARNING *** CBPivotCamera::SetFocus was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void CBPivotCamera::SetOrientation(float xRot, float yRot, float zRot)
{
        bkPrintf("*** WARNING *** CBPivotCamera::SetOrientation was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void CBPivotCamera::SetTilt(float vZRot)
{
        bkPrintf("*** WARNING *** CBPivotCamera::SetTilt was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void CBPivotCamera::SetDistance(float distance, float minDistance, float maxDistance)
{
        bkPrintf("*** WARNING *** CBPivotCamera::SetDistance was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void CBPivotCamera::SetChannels(TBIInputChannel *chLeftRight, TBIInputChannel *chUpDown,
                                TBIInputChannel *chForwardBack, TBIInputChannel *chTurbo,
                                TBIInputChannel *chSlow)
{
        bkPrintf("*** WARNING *** CBPivotCamera::SetChannels was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void CBPivotCamera::SetupAutoBindings(int padId)
{
        bkPrintf("*** WARNING *** CBPivotCamera::SetupAutoBindings was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void CBPivotCamera::Update(const float t)
{
        bkPrintf("*** WARNING *** CBPivotCamera::Update was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}