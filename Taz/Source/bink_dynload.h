#include "babel.h" // MG: d3d8 was already called in babel, think it'd confict, don't wanna check
#include "bink.h"

typedef U32 (__stdcall *PFN_BinkDX8SurfaceType)(IDirect3DSurface8*);
typedef HBINK(__stdcall *PFN_BinkOpen)(const char*, U32);
typedef void (__stdcall *PFN_BinkClose)(HBINK);
typedef S32 (__stdcall *PFN_BinkWait)(HBINK);
typedef void (__stdcall *PFN_BinkDoFrame)(HBINK);
typedef U32 (__stdcall *PFN_BinkCopyToBuffer)(HBINK, void*, S32, U32, U32, U32, U32);
typedef void (__stdcall *PFN_BinkNextFrame)(HBINK);
typedef S32 (__stdcall *PFN_BinkOpenDirectSound)(void*);
typedef S32 (__stdcall *PFN_BinkSetSoundSystem)(PFN_BinkOpenDirectSound, void*);

extern PFN_BinkDX8SurfaceType   pBinkDX8SurfaceType;
extern PFN_BinkOpen             pBinkOpen;
extern PFN_BinkClose            pBinkClose;
extern PFN_BinkWait             pBinkWait;
extern PFN_BinkDoFrame          pBinkDoFrame;
extern PFN_BinkCopyToBuffer     pBinkCopyToBuffer;
extern PFN_BinkNextFrame        pBinkNextFrame;
extern PFN_BinkOpenDirectSound  pBinkOpenDirectSound;
extern PFN_BinkSetSoundSystem   pBinkSetSoundSystem;

int  Bink_LoadDLL(const char* path);
void Bink_UnloadDLL(void);

#define BinkDX8SurfaceType     pBinkDX8SurfaceType
#define BinkOpen               pBinkOpen
#define BinkClose              pBinkClose
#define BinkWait               pBinkWait
#define BinkDoFrame            pBinkDoFrame
#define BinkCopyToBuffer       pBinkCopyToBuffer
#define BinkNextFrame          pBinkNextFrame
#define BinkOpenDirectSound    pBinkOpenDirectSound
#define BinkSetSoundSystem     pBinkSetSoundSystem