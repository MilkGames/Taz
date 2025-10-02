// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : CBGuiWindow.cpp
//   Purpose : gui window base class
// Component : BFC Gui
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

CBGuiWindow::CBGuiWindow() {}

CBGuiWindow::CBGuiWindow(int iFlags) {}

CBGuiWindow::~CBGuiWindow() {}

void CBGuiWindow::CalcClip() {}

void CBGuiWindow::SetPosition(int iX, int iY, int iWidth, int iHeight) {}

void CBGuiWindow::Paint1() {}

void CBGuiWindow::Paint2() {}

CBGuiObject* CBGuiWindow::HitTest(int iX, int iY) { return nullptr; }

void CBGuiWindow::SetWindowFlag(int iFlag) {}

void CBGuiWindow::ClearWindowFlag(int iFlag) {}

int CBGuiWindow::SetWindowText(ushort *pText) { return 0; }

int CBGuiWindow::CanDrag() { return 0; }

void CBGuiWindow::MoveObject(int iX, int iY, int iDx, int iDy) {}

void CBGuiWindow::Activate() {}

void CBGuiWindow::GetChildExtents() {}

void CBGuiWindow::SetHorizScrollBar(float xProp, float xPos, float size) {}

void CBGuiWindow::SetVertScrollBar(float yProp, float yPos, float size) {}

