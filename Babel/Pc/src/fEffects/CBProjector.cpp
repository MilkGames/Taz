// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2001 Blitz Games Ltd.
//
//      File : CBProjector.h
//   Purpose : projector base class class
// Component : Generic BFC Effects
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

CBProjector::CBProjector(EBProjectorControl control, uint32 flags) {}

CBProjector::~CBProjector() {}

void CBProjector::FocusOnActor(TBActorInstance *actInstance, TBVector offset, int sticky) {}

void CBProjector::SetFocus(TBVector focalPoint, int sticky) {}

void CBProjector::SetFocus(float x,float y,float z) {}

void CBProjector::SetPosition(TBVector pos, int sticky) {}

void CBProjector::SetPosition(float x, float y, float z) {}

void CBProjector::SetOrientation(float *xAngRadians, float *yAngRadians, float *zAngRadians, float dist) {}

void CBProjector::SetOrientation(float xAngRadians, float yAngRadians, float zAngRadians, float dist) {}

void CBProjector::SetOrientation(TBQuaternion orientation, float dist, int sticky) {}

void CBProjector::SetOrientation(TBMatrix orientation, float dist, int sticky) {}

void CBProjector::Update() {}

void CBProjector::SetViewInfo(float xFovVal, float yFovVal, float nearClipVal, float farClipVal) {}

void CBProjector::Show() {}

void CBProjector::SetDistance(float distance) {}
