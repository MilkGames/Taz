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
        bkPrintf("*** WARNING *** CBAnimSplash::SetVisibleArea was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void CBAnimSplash::Render()
{
        bkPrintf("*** WARNING *** CBAnimSplash::Render was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void CBAnimSplash::SetAnim(float time, int render)
{
        bkPrintf("*** WARNING *** CBAnimSplash::SetAnim was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

int CBAnimSplash::Enter()
{
        bkPrintf("*** WARNING *** CBAnimSplash::Enter was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

void CBAnimSplash::Exit()
{
        bkPrintf("*** WARNING *** CBAnimSplash::Exit was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

int CBAnimSplash::AddFX(CBAnimSplashFX *fxPtr)
{
        bkPrintf("*** WARNING *** CBAnimSplash::AddFX was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

 return 0; }

// ********************************************************************************
// stock fragment generators
// ********************************************************************************

CBAnimSplashFragGen_Grid::CBAnimSplashFragGen_Grid(int xGridRes, int yGridRes, int xBreak) 
{
        bkPrintf("*** WARNING *** CBAnimSplashFragGen_Grid::CBAnimSplashFragGen_Grid was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

CBAnimSplashFragGen_Grid::~CBAnimSplashFragGen_Grid()
{
        bkPrintf("*** WARNING *** ~CBAnimSplashFragGen_Grid was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

int CBAnimSplashFragGen_Grid::MakeFragments(CBAnimSplash *splash)
{ 
        bkPrintf("*** WARNING *** CBAnimSplashFragGen_Grid::MakeFragments was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

// ********************************************************************************
// stock FX processors
// ********************************************************************************

CBAnimSplashFX_Fade::CBAnimSplashFX_Fade(EBAnimSplashFXFade rFade, EBAnimSplashFXFade gFade,
                                         EBAnimSplashFXFade bFade, EBAnimSplashFXFade aFade)
{
        bkPrintf("*** WARNING *** CBAnimSplashFX_Fade::CBAnimSplashFX_Fade was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void CBAnimSplashFX_Fade::Enter()
{
        bkPrintf("*** WARNING *** CBAnimSplashFX_Fade::Enter was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void CBAnimSplashFX_Fade::Exit()
{
        bkPrintf("*** WARNING *** CBAnimSplashFX_Fade::Exit was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void CBAnimSplashFX_Fade::Process(float time, CBAnimSplash *splash)
{
        bkPrintf("*** WARNING *** CBAnimSplashFX_Fade::Process was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

uint32 CBAnimSplashFX_Fade::SetAttribMask()
{
        bkPrintf("*** WARNING *** CBAnimSplashFX_Fade::SetAttribMask was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

uint32 CBAnimSplashFX_Fade::ModAttribMask()
{
        bkPrintf("*** WARNING *** CBAnimSplashFX_Fade::ModAttribMask was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

CBAnimSplashFX_SlideOn::CBAnimSplashFX_SlideOn()
{
        bkPrintf("*** WARNING *** CBAnimSplashFX_SlideOn::CBAnimSplashFX_SlideOn was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void CBAnimSplashFX_SlideOn::Enter()
{
        bkPrintf("*** WARNING *** CBAnimSplashFX_SlideOn::Enter was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void CBAnimSplashFX_SlideOn::Exit()
{
        bkPrintf("*** WARNING *** CBAnimSplashFX_SlideOn::Exit was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void CBAnimSplashFX_SlideOn::Process(float time, CBAnimSplash *splash)
{
        bkPrintf("*** WARNING *** CBAnimSplashFX_SlideOn::Process was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

uint32 CBAnimSplashFX_SlideOn::SetAttribMask()
{
        bkPrintf("*** WARNING *** CBAnimSplashFX_SlideOn::SetAttribMask was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

uint32 CBAnimSplashFX_SlideOn::ModAttribMask()
{
        bkPrintf("*** WARNING *** CBAnimSplashFX_SlideOn::ModAttribMask was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}
