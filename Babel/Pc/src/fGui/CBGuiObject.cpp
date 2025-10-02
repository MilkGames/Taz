// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : CBGuiObject.cpp
//   Purpose : gui object base class
// Component : BFC Gui
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

CBGuiObject::CBGuiObject() {}

CBGuiObject::~CBGuiObject() {}

int CBGuiObject::AddChild(CBGuiObject *pChild) { return 0; }

int CBGuiObject::RemoveChild(CBGuiObject *pChild) { return 0; }

void CBGuiObject::SetPosition(int iX, int iY, int iWidth, int iHeight) {}

void CBGuiObject::MoveObject(int iX, int iY, int iDx, int iDy) {}

void CBGuiObject::CalcClip() {}

void CBGuiObject::PaintAll() {}

void CBGuiObject::SetObjectFlag(int flag) {}

void CBGuiObject::ClearObjectFlag(int flag) {}

CBGuiObject* CBGuiObject::HitTest(int iX, int iY) { return nullptr; }

int CBGuiObject::CanDrag() { return 0; }

void CBGuiObject::BringToTop() {}

void CBGuiObject::Deactivate() {}

void CBGuiObject::Activate() {}

void CBGuiObject::SetCallback(TBGuiCallback pFunc) {}

void CBGuiObject::SetId(int iId) {}

