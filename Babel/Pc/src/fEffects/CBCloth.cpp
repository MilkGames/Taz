// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2001 Blitz Games Ltd.
//
//      File : CBCloth.cpp
//   Purpose : cloth base class
// Component : Generic BFC Effects
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

CBCloth::CBCloth(float iWidth, float iHeight, int iXSegments, int iYSegments,
                 float density, TBTexture *texturePtr, TBMatrix m,
                 float tension, float shear, float damping) {}

CBCloth::~CBCloth() {}

void CBCloth::InitPosition(TBMatrix m) {}

void CBCloth::Render() {}

void CBCloth::Update(float timestep) {}

void CBCloth::SetWindVector(float x, float y, float z) {}

void CBCloth::SetParticleFlag(int x, int y, int flag) {}

void CBCloth::SetAllParticleFlags(int flag) {}

int CBCloth::CheckForCollisionsWithGround(float height) { return 0; }

int CBCloth::CheckForCollisionsWithSphere(TBVector pos, float radius) { return 0; }

void CBCloth::ResolveCollisions() {}
