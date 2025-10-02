// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2001 Blitz Games Ltd.
//
//      File : CBShadowProjector.cpp
//   Purpose : shadowmap projector class
// Component : Generic BFC Effects
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

CBShadowProjector::CBShadowProjector(EBProjectorControl control, int xDim, int yDim, uint32 flags, int bpp) {}

CBShadowProjector::~CBShadowProjector() {}

void CBShadowProjector::DrawActorInstance(TBActorInstance *actInstance, uint32 alphaFlags, uint32 flags) {}

void CBShadowProjector::SetShadowDensity(int density) {}

void CBShadowProjector::Enable(int stage) {}

void CBShadowProjector::Enable(int stage, EBShadowProjectorStyle renderStyle) {}

void CBShadowProjector::SetRecievingGeometryMatrix(const TBMatrix m) {}

void CBShadowProjector::Disable(int stage) {}

void CBShadowProjector::Blur(int noofPasses) {}
