#include <windows.h>
#include "bink_dynload.h"

static HMODULE hBink = NULL;

PFN_BinkDX8SurfaceType   pBinkDX8SurfaceType;
PFN_BinkOpen             pBinkOpen;
PFN_BinkClose            pBinkClose;
PFN_BinkWait             pBinkWait;
PFN_BinkDoFrame          pBinkDoFrame;
PFN_BinkCopyToBuffer     pBinkCopyToBuffer;
PFN_BinkNextFrame        pBinkNextFrame;
PFN_BinkOpenDirectSound  pBinkOpenDirectSound;
PFN_BinkSetSoundSystem   pBinkSetSoundSystem;

static FARPROC gp(const char* a, const char* b) {
    FARPROC p = GetProcAddress(hBink, a);
    if (!p && b) p = GetProcAddress(hBink, b);
    return p;
}

int Bink_LoadDLL(const char* path) {
    hBink = LoadLibraryA(path ? path : "binkw32.dll");
    if (!hBink) return 0;

    pBinkDX8SurfaceType  = (PFN_BinkDX8SurfaceType)  gp("BinkDX8SurfaceType",  "_BinkDX8SurfaceType@4");
    pBinkOpen            = (PFN_BinkOpen)            gp("BinkOpen",            "_BinkOpen@8");
    pBinkClose           = (PFN_BinkClose)           gp("BinkClose",           "_BinkClose@4");
    pBinkWait            = (PFN_BinkWait)            gp("BinkWait",            "_BinkWait@4");
    pBinkDoFrame         = (PFN_BinkDoFrame)         gp("BinkDoFrame",         "_BinkDoFrame@4");
    pBinkCopyToBuffer    = (PFN_BinkCopyToBuffer)    gp("BinkCopyToBuffer",    "_BinkCopyToBuffer@28");
    pBinkNextFrame       = (PFN_BinkNextFrame)       gp("BinkNextFrame",       "_BinkNextFrame@4");
    pBinkOpenDirectSound = (PFN_BinkOpenDirectSound) gp("BinkOpenDirectSound", "_BinkOpenDirectSound@4");
    pBinkSetSoundSystem  = (PFN_BinkSetSoundSystem)  gp("BinkSetSoundSystem",  "_BinkSetSoundSystem@8");

    return pBinkDX8SurfaceType && pBinkOpen && pBinkClose && pBinkWait &&
           pBinkDoFrame && pBinkCopyToBuffer && pBinkNextFrame &&
           pBinkOpenDirectSound && pBinkSetSoundSystem;
}

void Bink_UnloadDLL(void) {
    if (hBink) { FreeLibrary(hBink); hBink = NULL; }
    pBinkDX8SurfaceType  = NULL; pBinkOpen = NULL; pBinkClose = NULL;
    pBinkWait = NULL; pBinkDoFrame = NULL; pBinkCopyToBuffer = NULL;
    pBinkNextFrame = NULL; pBinkOpenDirectSound = NULL; pBinkSetSoundSystem = NULL;
}