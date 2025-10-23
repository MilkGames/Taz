// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcImagehlp.cpp
//   Purpose : debugging images
// Component : PC Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

#include <ddraw.h>
#include <dmusici.h>
#include <objbase.h>

// ********************************************************************************
// Globals



// ********************************************************************************
// Locals

// AI CODE! PREPARE THYSELF!!!

extern "C" __declspec(dllimport) BOOL WINAPI IsDebuggerPresent(void);

// DbgHelp handle + resolved procs (old ImageHlp-style names from retail PC build)
static HMODULE sDbgHelp = NULL;

// Use modern prototypes; they map to same exports on Win2k/XP dbghelp.dll
typedef BOOL  (WINAPI *PFN_SymInitialize)(HANDLE, const char*, BOOL);
typedef BOOL  (WINAPI *PFN_SymCleanup)   (HANDLE);
typedef DWORD (WINAPI *PFN_SymGetOptions)(VOID);
typedef DWORD (WINAPI *PFN_SymSetOptions)(DWORD);
typedef DWORD (WINAPI *PFN_SymLoadModule)(HANDLE, HANDLE, const char*, const char*, DWORD, DWORD);

static PFN_SymInitialize  pSymInitialize  = NULL;
static PFN_SymCleanup     pSymCleanup     = NULL;
static PFN_SymGetOptions  pSymGetOptions  = NULL;
static PFN_SymSetOptions  pSymSetOptions  = NULL;
static PFN_SymLoadModule  pSymLoadModule  = NULL;

// Cached EXE info
static BY_HANDLE_FILE_INFORMATION sExeInfo;
static char sExePath[MAX_PATH];
static int  sDbgReady = 0;

// ********************************************************************************
// Helper Functions

// print into current debug stream file safely (mimics retail PC build)
static void SafeDebugPrint(const char* message, ...)
{
	char buf[1024];
	buf[0] = '\0';

	va_list ap;
	va_start(ap, message);
	wvsprintfA(buf, message, ap);
	va_end(ap);

	if (!bCurrentDebugStream || !bCurrentDebugStream->logFile[0]) {
		// Fallback into debugger if stream is not ready
		OutputDebugStringA(buf);
		return;
	}

	HANDLE h = CreateFileA(bCurrentDebugStream->logFile,
						   GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
						   FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (h != INVALID_HANDLE_VALUE) {
		SetFilePointer(h, 0, 0, FILE_END);
		DWORD written = 0;
		const char* p = buf;
		DWORD len = 0;
		while (p[len] != '\0') ++len;
		if (len) WriteFile(h, buf, len, &written, NULL);
		CloseHandle(h);
	}
}

// format last Win32 error into a caller-provided buffer
static DWORD FormatLastErrorTo(char* outBuf, int outCap)
{
    if (!outBuf || outCap <= 0) return GetLastError();
    outBuf[0] = '\0';

    DWORD id = GetLastError();
    LPSTR sys = NULL;
    DWORD ok = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                              FORMAT_MESSAGE_FROM_SYSTEM    |
                              FORMAT_MESSAGE_IGNORE_INSERTS,
                              NULL, id, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                              (LPSTR)&sys, 0, NULL);
    if (ok && sys) {
        // Copy byte-by-byte (mirrors the decompiled behaviour)
        const char* s = sys;
        int cap = outCap;
        while (*s && cap > 1) { *outBuf++ = *s++; --cap; }
        *outBuf = '\0';
        LocalFree(sys);
    }
    return id;
}

// Resolve a single export and store into target pointer
template<typename T>
static void Resolve(HMODULE m, const char* name, T& fn)
{
    fn = (T)GetProcAddress(m, name);
}

// ********************************************************************************
// Function Implementations

/* --------------------------------------------------------------------------------
   Function : bInitDbgHelp
   Purpose  : Load DbgHelp.dll (next to EXE), resolve functions and initialise
   Parameters : 
   Returns : 
   Info    : Logs status via bkPrintf/SafeDebugPrint
*/

void bInitDbgHelp()
{
	// Build "<exe_dir>\\DbgHelp.dll"
    char dllPath[MAX_PATH];
    dllPath[0] = '\0';
    GetModuleFileNameA(NULL, dllPath, sizeof(dllPath));
    char* slash = strrchr(dllPath, '\\');
    if (slash) lstrcpyA(slash + 1, "DbgHelp.dll");
    else       lstrcpyA(dllPath, "DbgHelp.dll");

    sDbgHelp = LoadLibraryA(dllPath);
    if (!sDbgHelp) {
        bkPrintf("bInitDbgHelp: *** Could not load DbgHelp.dll\n");
        return;
    }
    // Retail PC string says "ImageHelp.dll" – keep it for compatibility with logs
    bkPrintf("bInitDebug: Loaded ImageHelp.dll\n");

    // Resolve required functions
    Resolve(sDbgHelp, "SymInitialize",  pSymInitialize);
	Resolve(sDbgHelp, "SymCleanup",     pSymCleanup);
	Resolve(sDbgHelp, "SymGetOptions",  pSymGetOptions);
	Resolve(sDbgHelp, "SymSetOptions",  pSymSetOptions);
	Resolve(sDbgHelp, "SymLoadModule",  pSymLoadModule);

    if (pSymGetOptions) {
        DWORD opts = pSymGetOptions();
        // Keep defaults; optionally ensure line loading
        if (pSymSetOptions) pSymSetOptions(opts /*| SYMOPT_LOAD_LINES*/);
    }

    if (!pSymInitialize || !pSymCleanup) {
        bkPrintf("bInitImageHlp: symInitialize FAILED\n");
        return;
    }

    // Initialise for current process; invade to auto-load modules
    if (!pSymInitialize(GetCurrentProcess(), NULL, TRUE)) {
        bkPrintf("bInitImageHlp: symInitialize FAILED\n");
        return;
    }
    bkPrintf("bInitDbgHelp: symInitialize OK\n");

    // Optionally load main module explicitly when debugger is present
    const int dbg = bDebuggerPresent ? bDebuggerPresent : (IsDebuggerPresent() ? 1 : 0);
    if (dbg && pSymLoadModule) {
        char modPath[MAX_PATH];
        GetModuleFileNameA(NULL, modPath, sizeof(modPath));
        DWORD base = pSymLoadModule(GetCurrentProcess(), NULL, modPath, NULL, 0, 0);
        if (base == 0) {
            char msg[512];
            FormatLastErrorTo(msg, sizeof(msg));
            bkPrintf("SymLoadModule FAILED (%s)\n", msg);
        } else {
            bkPrintf("SymLoadModule OK (0x%08x)\n", (unsigned int)base);
        }
    }

    // Cache EXE path + BY_HANDLE_FILE_INFORMATION (mirrors retail init)
    ZeroMemory(&sExeInfo, sizeof(sExeInfo));
    ZeroMemory(sExePath, sizeof(sExePath));

    GetModuleFileNameA(NULL, sExePath, sizeof(sExePath));
    HANDLE hExe = CreateFileA(sExePath, GENERIC_READ, FILE_SHARE_READ,
                              NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hExe == INVALID_HANDLE_VALUE) {
        char msg[512];
        FormatLastErrorTo(msg, sizeof(msg));
        bkPrintf("CreateFile for EXE info FAILED (%s)\n", msg);
        sDbgReady = 1;
        return;
    }
    GetFileInformationByHandle(hExe, &sExeInfo);
    CloseHandle(hExe);

    sDbgReady = 1;
}


/* --------------------------------------------------------------------------------
   Function : bShutdownDbgHelp
   Purpose  : Shutdown symbol handler and optionally unload DbgHelp.dll
   Parameters : 
   Returns : 
   Info    : 
*/

void bShutdownDbgHelp()
{
	if (pSymCleanup) {
        pSymCleanup(GetCurrentProcess());
    }
    if (sDbgHelp) {
        FreeLibrary(sDbgHelp);
        sDbgHelp = NULL;
    }
}

/* --------------------------------------------------------------------------------
   Function : bDumpDXVersions
   Purpose  : Print DirectX runtime + key DLL version info to the debug stream
   Parameters : 
   Returns : 
   Info    : 
*/

void bDumpDXVersions()
{
	// Header + runtime version
    SafeDebugPrint("DirectX runtime info:\n");

    // bCheckDirectXRuntime returns packed version (major<<8 | minor) in retail PC build
    {
        unsigned long v = bCheckDirectXRuntime();
        SafeDebugPrint("DirectX version %d.%d\n", (int)(v >> 8), (int)(v & 0xff));
    }

    // Helper to dump one DLL version block
    const char* dlls[] = {
        BK_DX_DLL_DDRAW,  // "DDRAW.DLL"
        BK_DX_DLL_D3D,    // "D3D.DLL"
        BK_DX_DLL_D3D8,   // "D3D8.DLL"
        BK_DX_DLL_DINPUT, // "DINPUT.DLL"
        BK_DX_DLL_DSOUND, // "DSOUND.DLL"
        BK_DX_DLL_DMUSIC, // "DMUSIC.DLL"
        BK_DX_DLL_DPLAY   // "DPLAY.DLL"
    };
    const char* fmt[] = {
        "DDRAW.DLL version is %d.%d.%d.%d\n",
        "D3D.DLL version is %d.%d.%d.%d\n",
        "D3D8.DLL version is %d.%d.%d.%d\n",
        "DINPUT.DLL version is %d.%d.%d.%d\n",
        "DSOUND.DLL version is %d.%d.%d.%d\n",
        "DMUSIC.DLL version is %d.%d.%d.%d\n",
        "DPLAY.DLL version is %d.%d.%d.%d\n"
    };

    for (int i = 0; i < (int)(sizeof(dlls)/sizeof(dlls[0])); ++i) {
        DWORD handle = 0;
        DWORD sz = GetFileVersionInfoSizeA((LPSTR)dlls[i], NULL);
        DWORD ms = 0, ls = 0;

		if (sz != 0) {
			void* mem = malloc(sz);
			if (mem) {
				if (GetFileVersionInfoA((LPSTR)dlls[i], 0, sz, mem)) {
					VS_FIXEDFILEINFO* ffi = NULL;
					UINT ffiLen = 0;
					if (VerQueryValueA(mem, "\\", (LPVOID*)&ffi, &ffiLen) && ffi && ffiLen >= sizeof(VS_FIXEDFILEINFO)) {
						ms = ffi->dwFileVersionMS;
						ls = ffi->dwFileVersionLS;
					}
				}
			free(mem);
			}
		}

        SafeDebugPrint(fmt[i],
            (int)(ms >> 16), (int)(ms & 0xffff),
            (int)(ls >> 16), (int)(ls & 0xffff));
    }
}

/* --------------------------------------------------------------------------------
   Function : bCheckDirectXRuntime
   Purpose  : Probe DirectX runtime by creating minimal components
   Parameters : 
   Returns : Packed version (major<<8 | minor), or error code as used by PC Release
   Info    : 
*/
unsigned long bCheckDirectXRuntime(void)
{
	// --- DDraw base check ---
    HMODULE hDD = LoadLibraryA(BK_DX_DLL_DDRAW);
    if (!hDD) { OutputDebugStringA("Couldn't LoadLibrary DDraw\n"); return 0; }

    typedef HRESULT (WINAPI *PFN_DirectDrawCreate)(GUID*, IDirectDraw**, IUnknown*);
    PFN_DirectDrawCreate pDirectDrawCreate =
        (PFN_DirectDrawCreate)GetProcAddress(hDD, "DirectDrawCreate");
    if (!pDirectDrawCreate) {
        FreeLibrary(hDD);
        OutputDebugStringA("Couldn't GetProcAddress DirectDrawCreate\n");
        return 0;
    }

    IDirectDraw* dd = NULL;
    if (FAILED(pDirectDrawCreate(NULL, &dd, NULL))) {
        FreeLibrary(hDD);
        OutputDebugStringA("Couldn't create DDraw\n");
        return 0;
    }

    // QI to IDirectDraw2
    IDirectDraw2* dd2 = NULL;
    if (FAILED(dd->QueryInterface(IID_IDirectDraw2, (void**)&dd2))) {
        dd->Release();
        FreeLibrary(hDD);
        OutputDebugStringA("Couldn't QI DDraw2\n");
        return 0x100;
    }

    // Cooperative level (normal)
    if (FAILED(dd2->SetCooperativeLevel(NULL, DDSCL_NORMAL))) {
        dd2->Release(); dd->Release(); FreeLibrary(hDD);
        OutputDebugStringA("Couldn't Set coop level\n");
        return 0;
    }

    // Create primary surface
    DDSURFACEDESC ddsd; ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize  = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    IDirectDrawSurface* s1 = NULL;
    if (FAILED(dd2->CreateSurface(&ddsd, &s1, NULL))) {
        dd2->Release(); dd->Release(); FreeLibrary(hDD);
        OutputDebugStringA("Couldn't CreateSurface\n");
        return 0;
    }

    // QI surface3
    IDirectDrawSurface3* s3 = NULL;
    if (FAILED(s1->QueryInterface(IID_IDirectDrawSurface3, (void**)&s3))) {
        s1->Release(); dd2->Release(); dd->Release(); FreeLibrary(hDD);
        OutputDebugStringA("Couldn't QI DDS3\n");
        return 0x300;
    }

    // QI surface4
    IDirectDrawSurface4* s4 = NULL;
    if (FAILED(s3->QueryInterface(IID_IDirectDrawSurface4, (void**)&s4))) {
        s3->Release(); s1->Release(); dd2->Release(); dd->Release(); FreeLibrary(hDD);
        OutputDebugStringA("Couldn't QI DDS4\n");
        return 0x500;
    }

    // Release intermediates
    s4->Release();
    s3->Release();
    s1->Release();

    // --- DInput presence ---
    HMODULE hDI = LoadLibraryA(BK_DX_DLL_DINPUT);
    if (!hDI) {
        dd2->Release(); dd->Release(); FreeLibrary(hDD);
        OutputDebugStringA("Couldn't LoadLibrary DInput\n");
        return 0x200;
    }
    FARPROC pDICreate = GetProcAddress(hDI, "DirectInputCreateA");
    FreeLibrary(hDI);
    if (!pDICreate) {
        dd2->Release(); dd->Release(); FreeLibrary(hDD);
        OutputDebugStringA("Couldn't GetProcAddress DInputCreate\n");
        return 0x200;
    }

    // --- DirectMusic via COM ---
    CoInitialize(NULL);
    IUnknown* pDM = NULL;
    HRESULT hrDM = CoCreateInstance(CLSID_DirectMusic, NULL, CLSCTX_INPROC_SERVER,
                                    IID_IDirectMusic, (void**)&pDM);
    if (SUCCEEDED(hrDM) && pDM) { pDM->Release(); }
    CoUninitialize();
    if (FAILED(hrDM)) {
        dd2->Release(); dd->Release(); FreeLibrary(hDD);
        OutputDebugStringA("Couldn't create CLSID_DirectMusic\n");
        return 0x600;
    }

    // --- DirectDrawCreateEx (DDraw7+) ---
    typedef HRESULT (WINAPI *PFN_DirectDrawCreateEx)(GUID*, VOID**, REFIID, IUnknown*);
    PFN_DirectDrawCreateEx pDDCreateEx =
        (PFN_DirectDrawCreateEx)GetProcAddress(hDD, "DirectDrawCreateEx");
    if (!pDDCreateEx) {
        dd2->Release(); dd->Release(); FreeLibrary(hDD);
        OutputDebugStringA("Couldn't GetProcAddress DirectDrawCreateEx\n");
        return 0x601;
    } else {
        // Probe creation with IDirectDraw7
        IDirectDraw7* dd7 = NULL;
        HRESULT hr = pDDCreateEx(NULL, (VOID**)&dd7, IID_IDirectDraw7, NULL);
        if (FAILED(hr) || !dd7) {
            dd2->Release(); dd->Release(); FreeLibrary(hDD);
            OutputDebugStringA("Couldn't DirectDrawCreateEx\n");
            return 0x601;
        }
        dd7->Release();
    }

    // --- D3D8.DLL presence ---
    HMODULE hD3D8 = LoadLibraryA(BK_DX_DLL_D3D8);
    if (!hD3D8) {
        dd2->Release(); dd->Release(); FreeLibrary(hDD);
        OutputDebugStringA("Couldn't LoadLibrary D3D8.DLL\n");
        return 0x700;
    }
    FreeLibrary(hD3D8);

    // --- dpnhpast.dll presence (DirectPlay NAT helper) ---
    HMODULE hNAT = LoadLibraryA("dpnhpast.dll");
    if (!hNAT) {
        dd2->Release(); dd->Release(); FreeLibrary(hDD);
        OutputDebugStringA("Couldn't LoadLibrary dpnhpast.dll\n");
        return 0x800;
    }
    FreeLibrary(hNAT);

    // Cleanup DDraw
    dd2->Release();
    dd->Release();
    FreeLibrary(hDD);

    // Success: encode as 8.1 like retail build (prints "8.1")
    return 0x801;
}