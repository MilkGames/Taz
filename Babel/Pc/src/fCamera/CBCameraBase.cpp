// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : CBCameraBase.cpp
//   Purpose : base class for camera systems
// Component : Generic Camera
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

CBCameraBase::CBCameraBase() {}

CBCameraBase::~CBCameraBase() {}

void CBCameraBase::Update(const float t)
{
    return; 
}

TBCameraInstance* CBCameraBase::Activate() 
{
    return NULL
}