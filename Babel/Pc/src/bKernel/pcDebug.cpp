// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcDebug.cpp
//   Purpose : debugging functions
// Component : Xbox Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Globals

TBDebugStream *bCurrentDebugStream;		// current debug stream
int			   bDebuggerPresent;		// 1=running under debugger, 0=not running under debugger
int			   bPrintPause         = 0; // time in milliseconds to pause between each print statement

// ********************************************************************************
// Locals

TBMutex        bDebugMutex;
// select your debug system here, meow
TBDebugStream  bDefaultDebugStream;
char		   _debugBuffer[1024];

// ********************************************************************************
// Helper Functions

void bInitDbgHelp()
{
        bkPrintf("*** WARNING *** bInitDbgHelp was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

// ********************************************************************************
// Function Implementations

// ********************************************************************************
// debug.h

/* --------------------------------------------------------------------------------
   Function : bkCreateDebugStream
   Purpose : create a new debug stream
   Parameters : ptr to stream block or NULL for dynamic, log filename, flags (see BDEBUGSTREAMFLAG_)
   Returns : ptr to stream block
   Info : 
*/

TBDebugStream *bkCreateDebugStream(TBDebugStream *stream, char *filename, uint32 flags)
{
    // Allocate structure if not provided (matches original callsite: size 0x108 and flags 0x2006)
    if (stream == NULL)
    {
        stream = (_TBDebugStream*)bkHeapAlloc(
            0x108,
            "C:\\Babel\\PC\\Src\\bKernel\\pcDebug.cpp",
            0xC6,
            0x2006
        );
        if (stream == NULL)
            return NULL;

        // Bit 0 indicates the stream object was allocated here (ownership)
        flags |= 0x1;
    }

    // Build the final path inside stream->logFile
    if (filename == NULL || *filename == '\0')
    {
        // No file requested ? leave disabled
        stream->logFile[0] = '\0';
    }
    else if (filename[1] == ':')
    {
        // Absolute path with drive letter copy as-is
        // (Ghidra showed a pointer trick; the struct begins with logFile, so plain copy is fine.)
        strncpy(stream->logFile, filename, sizeof(stream->logFile) - 1);
        stream->logFile[sizeof(stream->logFile) - 1] = '\0';
    }
    else
    {
        // Relative path prepend application path
        // (Original used "%s%s" with &bAppPath; here just use bAppPath directly.)
        _snprintf(stream->logFile, sizeof(stream->logFile), "%s%s", bAppPath, filename);
        stream->logFile[sizeof(stream->logFile) - 1] = '\0';
    }

    stream->flags = flags;

    // If no path, keep the stream closed
    if (stream->logFile[0] == '\0')
    {
        stream->file = NULL;
        return stream;
    }

    // Open mode: append by default; truncate/create if bit 2 set
    stream->file = fopen(stream->logFile, (flags & 4) ? "w" : "a");
    return stream;
}

/* --------------------------------------------------------------------------------
   Function : bkPrintf
   Purpose : print a string to the debug stream
   Parameters : as for printf
   Returns : 
   Info : 
*/

void bkPrintf(char *format, ...)
{
    bkWaitMutex(&bDebugMutex);

    char* out = _debugBuffer;

    if ( (bCurrentDebugStream->flags & BDEBUGSTREAMFLAG_TIMESTAMP) != 0 ) {
        SYSTEMTIME st; GetLocalTime(&st);
        out += sprintf(out, "%02u/%02u/%02u %02u:%02u:%02u.%03u ",
                       (unsigned)st.wDay,
                       (unsigned)st.wMonth,
                       (unsigned)(st.wYear % 100),
                       (unsigned)st.wHour,
                       (unsigned)st.wMinute,
                       (unsigned)st.wSecond,
                       (unsigned)st.wMilliseconds);
    }

    va_list ap;
    va_start(ap, format);
    vsprintf(out, format, ap);
    va_end(ap);

    if ( (bCurrentDebugStream->flags & BDEBUGSTREAMFLAG_TODEBUGGER) != 0 ) {
        OutputDebugStringA(_debugBuffer);
    }

    if ( (bCurrentDebugStream->flags & BDEBUGSTREAMFLAG_TOCONSOLEWINDOW) != 0 ) {
        bdConsoleWindowPrintf("%s", _debugBuffer);
    }

    if (bCurrentDebugStream->file) {
        fprintf(bCurrentDebugStream->file, "%s", _debugBuffer);
        fflush(bCurrentDebugStream->file);
    }

    if (bPrintPause != 0) {
        const unsigned __int64 waitTicks =
			((unsigned __int64)(unsigned)bPrintPause * (unsigned __int64)bTimerFrequency) / (unsigned __int64)1000;

        const unsigned __int64 t0 = (unsigned __int64)bkTimerRead();
        while ( (unsigned __int64)bkTimerDelta((TBTimerValue)t0, bkTimerRead()) < waitTicks ) {
            // just waiting
        }
    }

    bkReleaseMutex(&bDebugMutex);
}

// ********************************************************************************

/* --------------------------------------------------------------------------------
   Function : bInitDebug
   Purpose : initialise the debugging system
   Parameters : 
   Returns : 
   Info : 
*/

void bInitDebug(void)
{
    bkCreateMutex(&bDebugMutex);

    HMODULE hK32 = GetModuleHandleA("kernel32.dll");
    if (hK32) {
        typedef BOOL (WINAPI *PFN_IsDebuggerPresent)(VOID);
        PFN_IsDebuggerPresent pIsDbg = (PFN_IsDebuggerPresent)GetProcAddress(hK32, "IsDebuggerPresent");
        if (pIsDbg) {
            bDebuggerPresent = pIsDbg() ? 1 : 0;
        }
    }

    // "[.exe path]\\debugLog.txt"
    char modulePath[MAX_PATH];
    GetModuleFileNameA(NULL, modulePath, sizeof(modulePath));
    char* slash = strrchr(modulePath, '\\');
    if (!slash) {
        lstrcpyA(modulePath, "debugLog.txt");
    } else {
        ++slash; // on position after '\'
        lstrcpyA(slash, "debugLog.txt");
    }

    uint32 dbgFlags = ( (bBkInitFlags & BKINITFLAG_NODEBUGOUTPUT) == 0 )
                    ? (BDEBUGSTREAMFLAG_TODEBUGGER | BDEBUGSTREAMFLAG_FLUSH)
                    : 0;

    bkCreateDebugStream(&bDefaultDebugStream, modulePath, dbgFlags);
    bCurrentDebugStream = &bDefaultDebugStream;

    bkPrintf("\n-----------------------------------------\n");
    bkPrintf("Babel Execution Log, (c) 2001 Blitz Games\n");
    bkPrintf("  Babel " BVERSION ", Wed Mar 20 15:46:19 2002\n");
#ifdef DISCLAIMER
	bkPrintf("-----------------------------------------\n");
	bkPrintf("This Babel library was reverse engineered\n");
	bkPrintf("by MilkGames in 2025 using Release version\n");
	bkPrintf("of the Taz: Wanted game AND .h files from\n");
	bkPrintf("Taz Source Delivery DVD on archive.org.\n");
	bkPrintf("Even though both Taz: Wanted and Babel\n");
	bkPrintf("are abandonware it doesn't exclude\n");
	bkPrintf("the possibility, for example, that other\n");
	bkPrintf("companies may have legal rights to these\n");
	bkPrintf("products after the liquidation of Blitz Games.\n");
	bkPrintf("The use of this software for commercial\n");
	bkPrintf("purposes is strictly prohibited.\n\n");
    bkPrintf("We don't guarantee that this\n");
	bkPrintf("software will work and that it won't\n");
	bkPrintf("cause damage to your computer.\n");
#ifdef SHOWREALBUILDDATE
	bkPrintf("-----------------------------------------\n");
	bkPrintf("Real Babel build date:\n");
	bkPrintf(__TIMESTAMP__"\n");
#endif
#endif
    bkPrintf("-----------------------------------------\n");

    TBClock clk;
    if (!bkReadClock(&clk)) {
        bkPrintf("Current time is unavailable\n");
    } else {
        bkPrintf("Current time is %02u:%02u:%02u, %02u/%02u/%02u\n",
                 (unsigned)clk.hour,
                 (unsigned)clk.minute,
                 (unsigned)clk.second,
                 (unsigned)clk.day,
                 (unsigned)clk.month,
                 (unsigned)clk.year);
    }

    bkPrintf("Debugger %s present\n", bDebuggerPresent ? "is" : "is not");

    const char* kOsNames[4] = { "Win95", "Win98", "NT4", "Win2000" };

    char computerName[256] = {0};
    DWORD n = sizeof(computerName);
    GetComputerNameA(computerName, &n);

    char userName[256] = {0};
    n = sizeof(userName);
    GetUserNameA(userName, &n);

    OSVERSIONINFOA vi; ZeroMemory(&vi, sizeof(vi));
    vi.dwOSVersionInfoSize = sizeof(vi);
    GetVersionExA(&vi);

    int bOSIndex = 0; // 0=95, 1=98, 2=NT4, 3=2000
    if (vi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
        if (vi.dwMinorVersion != 0) bOSIndex = 1;
    } else if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT) {
        bOSIndex = (vi.dwMajorVersion == 5) ? 3 : 2;
    }

    MEMORYSTATUS ms; GlobalMemoryStatus(&ms);
    const DWORD physMB = ms.dwTotalPhys >> 20;

    bkPrintf("PlatformInfo: computer '%s', user '%s', %s build %u.%u.%u, %u MB RAM\n",
             computerName,
             userName,
             kOsNames[bOSIndex],
             (vi.dwBuildNumber & 0xffff),
             (vi.dwBuildNumber >>  8) & 0xff,
             (vi.dwBuildNumber >> 16) & 0xff,
             physMB);

    _controlfp(0, _MCW_EM);

    bInitDbgHelp();
}

/* --------------------------------------------------------------------------------
   Function : bShutdownDebug
   Purpose : shutdown the debugging system
   Parameters : 
   Returns : 
   Info : 
*/

void bShutdownDebug()
{
        bkPrintf("*** WARNING *** bShutdownDebug was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : bkAlert
   Purpose : display an error message to the user
   Parameters : message
   Returns : 
   Info : 
*/

void bkAlert(char *message)
{
    bkPrintf("bkAlert: %s", message);
    MessageBoxA(NULL, message, "Alert", MB_ICONHAND /* 0x10 */);
    return;
}


/* --------------------------------------------------------------------------------
   Function : bVPrintf
   Purpose : print a string to the current debug stream
   Parameters : format string, argument list pointer
   Returns : 
   Info : 
*/

void bVPrintf(char *format, va_list argp)
{
        bkPrintf("*** WARNING *** bVPrintf was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}