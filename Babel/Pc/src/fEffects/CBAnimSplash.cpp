// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2001 Blitz Games Ltd.
//
//      File : CBAnimSplash.cpp
//   Purpose : animated splash screen base class
// Component : Generic BFC Effects
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

CBAnimSplash::CBAnimSplash(TBTexture *texturePtr, CBAnimSplashFragGen *fragGenPtr) {}

CBAnimSplash::~CBAnimSplash() {}

void CBAnimSplash::SetVisibleArea(int texWidth, int texHeight)
{
    return;
}

void CBAnimSplash::Render()
{
    return;
}

void CBAnimSplash::SetAnim(float time, int render)
{
    return;
}

int CBAnimSplash::Enter()
{
    return 0;
}

void CBAnimSplash::Exit()
{
    return;
}

int CBAnimSplash::AddFX(CBAnimSplashFX *fxPtr)
{
    return 0;
}

int CBAnimSplash::Alloc(int noofAttribs, int noofFragments) { return 0; }

// ********************************************************************************
// stock fragment generators
// ********************************************************************************

CBAnimSplashFragGen_Grid::CBAnimSplashFragGen_Grid(int xGridRes, int yGridRes, int xBreak) 
{
    return;
}

CBAnimSplashFragGen_Grid::~CBAnimSplashFragGen_Grid()
{
    return;
}

int CBAnimSplashFragGen_Grid::MakeFragments(CBAnimSplash *splash)
{ 
    return 0;
}

// ********************************************************************************
// stock FX processors
// ********************************************************************************

CBAnimSplashFX_Fade::CBAnimSplashFX_Fade(EBAnimSplashFXFade rFade, EBAnimSplashFXFade gFade,
                                         EBAnimSplashFXFade bFade, EBAnimSplashFXFade aFade)
{
    return;
}

void CBAnimSplashFX_Fade::Enter()
{
    return;
}

void CBAnimSplashFX_Fade::Exit()
{
    return;
}

void CBAnimSplashFX_Fade::Process(float time, CBAnimSplash *splash)
{
    return;
}

uint32 CBAnimSplashFX_Fade::SetAttribMask()
{
    return 0;
}

uint32 CBAnimSplashFX_Fade::ModAttribMask()
{
    return 0;
}

CBAnimSplashFX_SlideOn::CBAnimSplashFX_SlideOn()
{
    return;
}

void CBAnimSplashFX_SlideOn::Enter()
{
    return;
}

void CBAnimSplashFX_SlideOn::Exit()
{
    return;
}

void CBAnimSplashFX_SlideOn::Process(float time, CBAnimSplash *splash)
{
    return;
}

uint32 CBAnimSplashFX_SlideOn::SetAttribMask()
{
    return 0;
}

uint32 CBAnimSplashFX_SlideOn::ModAttribMask()
{
    return 0;
}
