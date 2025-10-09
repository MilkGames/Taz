// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcKernel.cpp
//   Purpose : kernel functions
// Component : Xbox Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Globals

int   bActive        = 1;          // exe default: active on start
int   bFocus         = 1;          // allow auto-suspend on focus loss
HWND  bMainWindow    = NULL;
char  bAppPath[256];

// ********************************************************************************
// Locals

// purely for fidelity with the exe; not observed being read elsewhere
static int bMainRunning = 0;

// In the exe this is just a pointer that may be NULL.
// If NULL, we show literal "Babel (Running)/(Suspended)".
static char *bAppName = NULL;

static int bCPUSimdFlags;

// ********************************************************************************
// Local Helpers

static void cpuid_regs(unsigned int leaf, unsigned int regs[4])
{
    __asm {
        mov     eax, leaf
        xor     ecx, ecx            // subleaf 0
        push    ebx                 // EBX is non-volatile in MSVC
        cpuid
        mov     edi, regs
        mov     [edi+0],  eax       // EAX
        mov     [edi+4],  ebx       // EBX
        mov     [edi+8],  edx       // EDX
        mov     [edi+12], ecx       // ECX
        pop     ebx
    }
}

// ********************************************************************************
// Local Functions

/* --------------------------------------------------------------------------------
   Function : BabelWndProc
   Purpose : PC window proc
   Parameters : 
   Returns : LRESULT
   Info : reproduced from the exe with the same branching & messages, a bit weird
*/
static LRESULT CALLBACK BabelWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // Resume on click if allowed and we are currently inactive
    if (msg <= WM_LBUTTONUP) {
        if (msg == WM_LBUTTONUP) {
            if ( ( (bDisplayInfo.flags & 0x20) == 0 ) || (bActive != 0) ) {
                // fallthrough to DefWindowProcA
            } else {
                if (bAppName == NULL) {
                    SetWindowTextA(bMainWindow, "Babel (Running)");
                }
                bActive = 1;
                // fallthrough to DefWindowProcA
            }
        }
        else if (msg == WM_ACTIVATEAPP) {
            if (wParam == 1) { // activated
                if (bAppName == NULL) {
                    SetWindowTextA(bMainWindow, "Babel (Running)");
                }
                bActive = 1;
            }
            else {
                // Deactivation is handled via BK_WM_SUSPEND branch below
                if (bFocus != 0) {
                    if (bAppName == NULL) {
                        SetWindowTextA(bMainWindow, "Babel (Suspended)");
                    }
                    bActive = 0;
                }
            }
        }
        else if (msg == WM_SYSCOMMAND && wParam == SC_SCREENSAVE) {
            return 1; // block screensaver
        }
    }
    else {
        // Engine-private messages
        if (msg == BK_WM_QUIT) {
            PostQuitMessage(0);
        }
        else if (msg == BK_WM_SUSPEND) {
            if (bFocus != 0) {
                if (bAppName == NULL) {
                    SetWindowTextA(bMainWindow, "Babel (Suspended)");
                }
                bActive = 0;
            }
        }
    }

    return DefWindowProcA(hWnd, msg, wParam, lParam);
}

/* --------------------------------------------------------------------------------
   Function : TestCPU
   Purpose : well, test CPU...
   Parameters : 
   Returns :
   Info :
*/
void TestCPU()
{
    unsigned int regs[4];           // [EAX, EBX, EDX, ECX]
    unsigned int eax1;
    char vendor[16];
    const char *mmxMsg, *simdMsg;

    // Leaf 0: vendor string = EBX || EDX || ECX
    cpuid_regs(0, regs);
    ((unsigned int*)vendor)[0] = regs[1];  // EBX
    ((unsigned int*)vendor)[1] = regs[2];  // EDX
    ((unsigned int*)vendor)[2] = regs[3];  // ECX
    vendor[12] = '\0';

    // Leaf 1: version/info (EAX) and feature flags (EDX)
    cpuid_regs(1, regs);
    eax1 = regs[0];                         // EAX
    // keep the original flag expression exactly as in the decomp
    bCPUSimdFlags = ((regs[2] >> 1) & 0x01000000 | (regs[2] & 0x00800000)) >> 23;

    // Output
    bkPrintf("--------------------------------------------------------------------------------------------------\n");
    bkPrintf("TestCPU: Testing for SIMD support, benign 'illegal instruction' first-chance exception possible...\n");

    simdMsg = (bCPUSimdFlags & 2) ? "SIMD Detected" : "No SIMD support";
    mmxMsg  = (bCPUSimdFlags & 1) ? "MMX Detected"  : "No MMX support";

    bkPrintf("TestCPU: ID '%s', family %d, model %d, stepping %d, %s, %s\n",
             vendor, (eax1 >> 8) & 0xF, (eax1 >> 4) & 0xF, eax1 & 0xF, mmxMsg, simdMsg);

    bkPrintf("--------------------------------------------------------------------------------------------------\n");
}

// ********************************************************************************
// Function Implementations

/* --------------------------------------------------------------------------------
   Function : bInitKernel
   Purpose : initialise the kernel module
   Parameters : 
   Returns : OK/FAIL
   Info : 
*/

int bInitKernel()
{
    // Core subsystems first
    bInitDebug();
    bInitTimer();
    bInitEvents();
    bInitCRCTable();
    bInitResources();

    // PC-only probe
    TestCPU();

    // Build executable directory path into bAppPath
    // (buffer is 256 bytes in the original; keep the same limit 0x100)
    GetModuleFileNameA(NULL, bAppPath, 0x100);

    // Trim everything after the last backslash (including the slash itself)
    if (char* lastSlash = strrchr(bAppPath, '\\'))
        *lastSlash = '\0';

    // Ensure the path ends with a trailing backslash
    // (the decomp does this via *(uint16*)(end-1) = '\\\0')
    {
        const size_t len = strlen(bAppPath);
        if (len == 0 || bAppPath[len - 1] != '\\') {
            bAppPath[len]     = '\\';
            bAppPath[len + 1] = '\0';
        }
    }

    // Background loader bootstrap
    bKernelInitBkgLoad();

    return OK;
}

/* --------------------------------------------------------------------------------
   Function : bShutdownKernel
   Purpose : shutdown the kernel module
   Parameters : 
   Returns : 
   Info : 
*/

void bShutdownKernel()
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bAddOSEvent
   Purpose : add an OS event
   Parameters : callback, context for callback, event description
   Returns : event handle or NULL for failure
   Info : 
*/

HANDLE bAddOSEvent(TBOSEventCallback callback, void *context, char *description)
{
    return NULL;
}


/* --------------------------------------------------------------------------------
   Function : bDeleteOSEvent
   Purpose : delete an OS event
   Parameters : event to delete
   Returns : 
   Info : 
*/

void bDeleteOSEvent(HANDLE event)
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bHandleOSEvents
   Purpose : handle any active OS events
   Parameters : 
   Returns : 
   Info : 
*/

void bHandleOSEvents()
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkRun
   Purpose : run the babel program
   Parameters : main function, context
   Returns : 
   Info : 
*/

void bkRun(TBabelMainFunction mainFunc, void *context)
{
    // window class setup
    WNDCLASSA wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.style         = 0;
    wc.lpfnWndProc   = BabelWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = GetModuleHandleA(NULL);
    wc.hIcon         = LoadIconA(wc.hInstance, "ApplicationIcon");
    wc.hCursor       = LoadCursorA(NULL, IDC_ARROW);               // 0x00007F00
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);        // 5
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "BabelApplication";

    if (!RegisterClassA(&wc)) {
        MessageBoxA(NULL, "Could not register window class", "babRun()", MB_ICONERROR);
        return;
    }

    // window rect & creation
    RECT rc = { 100, 100, 0x2e4, 0x244 }; // kept literal sizes to mirror the binary
    const DWORD style = 0x90C00000; // kept this as well (whatever this is)
    AdjustWindowRect(&rc, style, FALSE);

    const char* title = (bAppName != NULL) ? bAppName : "Babel (Running)";

    bMainWindow = CreateWindowExA(
        WS_EX_APPWINDOW,         // (0x00040000)
        "BabelApplication",
        title,
        style,
        0, 0,
        rc.right - rc.left,
        rc.bottom - rc.top,
        NULL, NULL,
        wc.hInstance,
        NULL);

    if (!bMainWindow) {
        MessageBoxA(NULL, "Could not create main window", "babRun()", MB_ICONERROR);
        return;
    }

    SetForegroundWindow(bMainWindow);
    SetWindowPos(bMainWindow, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW); // 0x43

    // drain pending messages before entering game main
    MSG msg;
    while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message != WM_QUIT) {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
    }

    bMainRunning = 1;
    mainFunc(context); // callin' main function (Main in Taz)
    bMainRunning = 0;
}


/* --------------------------------------------------------------------------------
   Function : bkCreateMutex
   Purpose : Creates a mutex object
   Parameters : mutex object pointer
   Returns : mutex object pointer, or NULL for failure
   Info : 
*/

TBMutex *bkCreateMutex(TBMutex *mutex)
{
    HANDLE h;
    h = CreateMutexA((LPSECURITY_ATTRIBUTES)0x0, 0, (LPCSTR)0x0);
    *mutex = h;
    return mutex;
}


/* --------------------------------------------------------------------------------
   Function : bkWaitMutex
   Purpose : Waits for a mutex object to become available and gains control
   Parameters : Mutex object pointer
   Returns : OK/FAIL
   Info : 
*/

int bkWaitMutex(TBMutex *mutex)
{
    DWORD r;
    r = WaitForSingleObject(*mutex, 0xffffffff);
    return (uint)(r == 0);
}

/* --------------------------------------------------------------------------------
   Function : bkReleaseMutex
   Purpose : Release a mutex object
   Parameters : Mutex object pointer
   Returns : OK/FAIL
   Info : 
*/

int bkReleaseMutex(TBMutex *mutex)
{
    BOOL ok;
    ok = ReleaseMutex(*mutex);
    return (uint)(ok != 0);
}

/* --------------------------------------------------------------------------------
   Function : bkDeleteMutex
   Purpose : Delete a mutex object
   Parameters : Mutex object pointer
   Returns : OK/FAIL
   Info : 
*/

int bkDeleteMutex(TBMutex *mutex)
{
    BOOL ok;
    ok = CloseHandle(*mutex);
    return (uint)(ok != 0);
}

/* --------------------------------------------------------------------------------
   Function : bInitCommandLine
   Purpose : read and parse command line
   Parameters : 
   Returns : 1 pass/ 0 fail
   Info : 
*/

int bInitCommandLine()
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bShutdownCommandLine
   Purpose : free command line data
   Parameters : 
   Returns :
   Info : 
*/

void bShutdownCommandLine()
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkGetCommandLine
   Purpose : get command line
   Parameters : ptr to number of arguments, ptr to argument list
   Returns : 
   Info : Cannot be called before bkInit
*/

void bkGetCommandLine(int *argc, char **argv[])
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bFlushCPUCache
   Purpose : flush the CPU L1 and L2 caches
   Parameters : 
   Returns : 
   Info : 
*/

void bFlushCPUCache()
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bFlushWrites
   Purpose : flush all pending writes for an area of memory from the CPU cache
   Parameters : start of area, #dwords to flush
   Returns : 
   Info : 
*/

void bFlushWrites(uint32 *ptr, int noofDwords)
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkSetAppName
   Purpose : set the application name
   Parameters : name for application
   Returns : 
   Info : 
*/

void bkSetAppName(char *appName)
{
    bAppName = appName;
    if (bMainWindow)
        SetWindowTextA(bMainWindow, appName ? appName : "");
}


/* --------------------------------------------------------------------------------
   Function : bCheckMemAgainstFile
   Purpose : check a memory buffer against the contents of a file
   Parameters : memory buffer, filename, #bytes to compare
   Returns : TRUE if same, FALSE otherwise, -1=error
   Info : 
*/

int bCheckMemAgainstFile(void *ptr, char *fname, int noofBytes)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bCountSystemAllocs
   Purpose : count the number of system allocations
   Parameters : 
   Returns : number of system allocations
   Info : 
*/

int bCountSystemAllocs()
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bDumpSystemAllocs
   Purpose : dump a list of system allocs to the debug stream
   Parameters : 
   Returns : 
   Info : 
*/

void bDumpSystemAllocs()
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bActivateHeapCheckerThread
   Purpose : activate/deactivate the heap checker thread
   Parameters : TRUE to activate, FALSE to deactivate
   Returns : 
   Info : 
*/

void bActivateHeapCheckerThread(int status)
{
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bkSetModulePath
	Purpose : set path to runtime modules
	Parameters : path name (default: "c:/babel/ps2/modules/" or "\\MODULES\\")
	Returns :
	Info : Currently only required on PlayStation 2
*/

void bkSetModulePath(char *pathName){
	return; // MG: confirmed, isn't used and blank
}