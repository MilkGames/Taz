// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcDebug.cpp
//   Purpose : debugging functions
// Component : Xbox Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Locals

TBMutex        bDebugMutex = NULL;
TBDebugStream  bDefaultDebugStream = { "", BDEBUGSTREAMFLAG_TODEBUGGER, NULL };
char		   _debugBuffer[2048] = "";

static const char* months[13] =
{ "", "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec" };

#ifdef FIX_PLATFORMINFO

// Minimal RTL_OSVERSIONINFOEXW for RtlGetVersion
typedef struct _RTL_OSVERSIONINFOEXW_ {
    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformId;
    WCHAR szCSDVersion[128];
} RTL_OSVERSIONINFOEXW_;

typedef LONG (WINAPI *PFN_RtlGetVersion)(RTL_OSVERSIONINFOEXW_*);

#endif // FIX_PLATFORMINFO

// ********************************************************************************
// Globals

TBDebugStream *bCurrentDebugStream = &bDefaultDebugStream; // current debug stream
int			   bDebuggerPresent    = 0;					   // 1=running under debugger, 0=not running under debugger
int			   bPrintPause         = 0;					   // time in milliseconds to pause between each print statement

// ********************************************************************************
// Local Functions

#ifdef FIX_PLATFORMINFO

// Try to get true version (Win10/11) even without manifest.
static BOOL GetAccurateVersion(DWORD* maj, DWORD* min, DWORD* bld, DWORD* plat)
{
    HMODULE hNt = GetModuleHandleA("ntdll.dll");
    if (!hNt) return FALSE;
    PFN_RtlGetVersion p = (PFN_RtlGetVersion)GetProcAddress(hNt, "RtlGetVersion");
    if (!p) return FALSE;

    RTL_OSVERSIONINFOEXW_ vi;
    ZeroMemory(&vi, sizeof(vi));
    vi.dwOSVersionInfoSize = sizeof(vi);
    if (p(&vi) != 0) return FALSE;

    if (maj)  *maj  = vi.dwMajorVersion;
    if (min)  *min  = vi.dwMinorVersion;
    if (bld)  *bld  = vi.dwBuildNumber;
    if (plat) *plat = vi.dwPlatformId;
    return TRUE;
}

static const char* NameForWindows(DWORD plat, DWORD maj, DWORD min, DWORD build)
{
    // Win9x line uses VER_PLATFORM_WIN32_WINDOWS (1)
    if (plat == VER_PLATFORM_WIN32_WINDOWS) {
        if (min >= 90) return "WinME";
        if (min >= 10) return "Win98";
        return "Win95";
    }

    // NT line
    if (maj < 5)            return "NT4";
    if (maj == 5 && min==0) return "Win2000";
    if (maj == 5 && min==1) return "WinXP";
    if (maj == 5 && min==2) return "Win2003";  // includes XP x64

    if (maj == 6 && min==0) return "Vista";
    if (maj == 6 && min==1) return "Win7";
    if (maj == 6 && min==2) return "Win8";
    if (maj == 6 && min==3) return "Win8.1";

    // Windows 10/11 both report major==10; Win11 has build >= 22000
    if (maj >= 10) {
        if (build >= 22000) return "Win11";
        return "Win10";
    }

    // Fallback
    return "NT";
}

#endif // FIX_PLATFORMINFO

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
    // Allocate structure if not provided (matches original callsite: size 0x108 and group 0x2006)
    if (stream == NULL)
    {
        stream = (_TBDebugStream*)bkHeapAlloc(
            0x108, // 264?
            __FILE__,
            __LINE__,
            0x2006 // 8198???
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
        // (Original used "%s%s" with &bHomeDirectory; here just use bHomeDirectory directly.)
        _snprintf(stream->logFile, sizeof(stream->logFile), "%s%s", bHomeDirectory, filename);
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
#ifdef FILTER_STUB_SPAM
	if (strstr(format, "wasn't implemented") != NULL ||
		strstr(format, "bLoadResource: Could not find") != NULL)
	{
		static uint32 warned_crcs[512];
		static int	  warned_count = 0;

		uint32 msgCRC = bkStringCRC(format);

		for (int i = 0; i < warned_count; i++) {
			if (warned_crcs[i] == msgCRC) return;
		}

		if (warned_count < 512) warned_crcs[warned_count++] = msgCRC;
	}
#endif

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
#ifdef PRINT_DISCLAIMER
	bkPrintf("-----------------------------------------\n");
	bkPrintf("This Babel library was reverse-engineered\n");
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
#ifdef PRINT_REALBUILDDATE
	bkPrintf("-----------------------------------------\n");
	bkPrintf("Real Babel build date:\n");
	bkPrintf(__TIMESTAMP__"\n");
#ifdef PRINT_CREDITS
	bkPrintf("-----------------------------------------\n");
	bkPrintf("                Credits:                 \n");
	bkPrintf("  Reverse Engineering & Implementation:  \n");
    bkPrintf("   MilkGames                             \n");
    bkPrintf("\n");
    bkPrintf("  Quality Assurance & Testing:           \n");
    bkPrintf("   MuxaJlbl4 (MuLLlaH9!)                 \n");
    bkPrintf("   CycloneFN                             \n");
    bkPrintf("\n");
    bkPrintf("  Special Thanks:                        \n");
    bkPrintf("   To the original team at Blitz Games:  \n");
    bkPrintf("   Thank you for the childhood memories  \n");
    bkPrintf("   and the incredible tech you built.    \n");
	bkPrintf("   Your code lives on.                   \n");
	bkPrintf("-----------------------------------------\n");
	bkPrintf("This reverse-engineered library uses AI  \n");
	bkPrintf("resources to declare pre-built variables \n");
	bkPrintf("and functions, as well as implement some \n");
	bkPrintf("particularly complex functions for direct\n");
	bkPrintf("use/reference. All functions (sooner or  \n");
	bkPrintf("later) undergo a manual thorough check   \n");
	bkPrintf("against disassembled code; the status of each\n");
	bkPrintf("function can be viewed on the Taz: Wanted\n");
	bkPrintf("Preservation Community Discord server.   \n");
#endif
#endif
#endif
    bkPrintf("-----------------------------------------\n");

    TBClock clk;
    if (!bkReadClock(&clk)) {
        bkPrintf("Current time is unavailable\n");
    } else {
        bkPrintf("Current time is %02d:%02d:%02d, %d %s 20%02d\n",
                 (unsigned)clk.hour,
                 (unsigned)clk.minute,
                 (unsigned)clk.second,
                 (unsigned)clk.day,
                 months[(unsigned)clk.month],
                 (unsigned)clk.year);
    }

    bkPrintf("Debugger %s present\n", bDebuggerPresent ? "is" : "is not");

#ifndef FIX_PLATFORMINFO
    // ----------------------- AUTHENTIC (original) -----------------------
    // OS labels as in the original: Win95/Win98/NT4/Win2000
    {
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

        // build from LOWORD, "version" from HIWORD bytes -> matches release (v0.0 on NT)
        const unsigned build  = (unsigned)(vi.dwBuildNumber & 0xFFFF);
        const unsigned vMajor = (unsigned)((vi.dwBuildNumber >> 24) & 0xFF);
        const unsigned vMinor = (unsigned)((vi.dwBuildNumber >> 16) & 0xFF);

#ifdef SHORT_PLATFORMINFO
		bkPrintf("SHORT_PLATFORMINFO is defined -> hiding computer name and user name\n");
        bkPrintf("PlatformInfo: OS %s build #%u (v%u.%u), physical memory %u Mb\n",
                 kOsNames[bOSIndex],
                 build, vMajor, vMinor, (unsigned)physMB);
#else
		bkPrintf("PlatformInfo: computer '%s', user '%s', OS %s build #%u (v%u.%u), physical memory %u Mb\n",
                 computerName, userName,
                 kOsNames[bOSIndex],
                 build, vMajor, vMinor, (unsigned)physMB);
#endif
    }
#else
    // ----------------------- FIXED (modern-correct) ---------------------
    {
        char computerName[256] = {0};
        DWORD n = sizeof(computerName);
        GetComputerNameA(computerName, &n);

        char userName[256] = {0};
        n = sizeof(userName);
        GetUserNameA(userName, &n);

        DWORD maj=0, min=0, bld=0, plat=0;

        // Prefer accurate numbers (Win10/11)
        if (!GetAccurateVersion(&maj, &min, &bld, &plat)) {
            OSVERSIONINFOA vi; ZeroMemory(&vi, sizeof(vi));
            vi.dwOSVersionInfoSize = sizeof(vi);
            GetVersionExA(&vi);
            maj  = vi.dwMajorVersion;
            min  = vi.dwMinorVersion;
            bld  = (vi.dwBuildNumber & 0xFFFF);
            plat = vi.dwPlatformId;
        }

        // If fallback path returned build in LOWORD only, keep it; else bld is real build.
        const char* osName = NameForWindows(plat, maj, min, bld);

        MEMORYSTATUS ms; GlobalMemoryStatus(&ms);
        const DWORD physMB = ms.dwTotalPhys >> 20;
#ifdef SHORT_PLATFORMINFO
		bkPrintf("SHORT_PLATFORMINFO is defined -> hiding computer name and user name\n");
        bkPrintf("PlatformInfo: OS %s build #%lu (v%lu.%lu), physical memory %lu Mb\n",
                 osName,
                 (unsigned long)bld,
                 (unsigned long)maj, (unsigned long)min,
                 (unsigned long)physMB);
#else
		bkPrintf("PlatformInfo: computer '%s', user '%s', OS %s build #%lu (v%lu.%lu), physical memory %lu Mb\n",
                 computerName, userName,
                 osName,
                 (unsigned long)bld,
                 (unsigned long)maj, (unsigned long)min,
                 (unsigned long)physMB);
#endif
    }
#endif // FIX_PLATFORMINFO

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
    // Shutdown DbgHelp bindings first
    bShutdownDbgHelp();

    // Release the debug mutex used by the logging system
    bkDeleteMutex(&bDebugMutex);

    // Keep the current stream pointer sane
    if (bCurrentDebugStream == &bDefaultDebugStream) {
        bCurrentDebugStream = &bDefaultDebugStream;
    }

    // Close the log file if it is open
    if (bDefaultDebugStream.file != NULL) {
        fclose(bDefaultDebugStream.file);
        bDefaultDebugStream.file = NULL;
    }

    // If the default stream block was heap-allocated, free it
    if ((bDefaultDebugStream.flags & BDEBUGSTREAMFLAG_DYNAMIC) != 0) { // flags per debug.h
        bkHeapFree(&bDefaultDebugStream);
    }
}

/* --------------------------------------------------------------------------------
   Function : bkStreamPrintf
   Purpose : print a string to a debug stream
   Parameters : debug stream, as for printf
   Returns : 
   Info : 
*/

void bkStreamPrintf(TBDebugStream *stream, char *format, ...)
{
	if (!stream) return;

	va_list args;
	va_start(args, format);
	
	if (stream->flags & BDEBUGSTREAMFLAG_TIMESTAMP) {
		SYSTEMTIME st;
		GetLocalTime(&st);

		sprintf(_debugBuffer, "%02d/%02d/%02d %02d:%02d:%02d.%03d ",
            st.wDay,
            st.wMonth,
            st.wYear - 2000,
            st.wHour,
            st.wMinute,
            st.wSecond,
            st.wMilliseconds
        );

		vsprintf(_debugBuffer + 0x16, format, args);
	}
	else {
		vsprintf(_debugBuffer, format, args);
	}

	va_end(args);

	if (stream->file != NULL) {
		fprintf(stream->file, "%s", _debugBuffer);
		fflush(stream->file);
	}
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
    MessageBoxA(NULL, message, "Alert", MB_ICONHAND);
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