// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcTimer.cpp
//   Purpose : timer functions
// Component : XBOX Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Globals

TBTimerValue		bTimerFrequency;
int					bTimerType;

// ********************************************************************************
// Locals

struct CPUIDRegs { unsigned int eax, ebx, edx, ecx; };
static CPUIDRegs g_cpuid_ver_info;

CPUIDRegs* __cdecl cpuid_Version_info(int leaf)
{
    __asm {
        mov  eax, leaf     // 1
        xor  ecx, ecx      // subleaf = 0
        cpuid
        mov  g_cpuid_ver_info.eax, eax
        mov  g_cpuid_ver_info.ebx, ebx
        mov  g_cpuid_ver_info.edx, edx
        mov  g_cpuid_ver_info.ecx, ecx
    }
    return &g_cpuid_ver_info;
}

unsigned __int64 __cdecl rdtsc(void)
{
    unsigned __int64 x;
    __asm {
        rdtsc
        mov dword ptr [x],    eax
        mov dword ptr [x+4],  edx
    }
    return x;
}

static void NormalizeFPU(void)
{
    __try {
        // Clear any pending x87 exception flags first (fnclex)
        _clearfp();

        // Mask ALL exceptions: INVALID, DENORMAL, ZERODIVIDE, OVERFLOW,
        // UNDERFLOW, INEXACT.
        _controlfp(_EM_INVALID   | _EM_DENORMAL   | _EM_ZERODIVIDE |
                   _EM_OVERFLOW  | _EM_UNDERFLOW  | _EM_INEXACT, _MCW_EM);

        // Restore default rounding mode (round to nearest) and 53-bit precision
        _controlfp(_RC_NEAR, _MCW_RC);
        _controlfp(_PC_53,  _MCW_PC);
    }
    __except (GetExceptionCode() == EXCEPTION_FLT_INEXACT_RESULT
                  ? EXCEPTION_EXECUTE_HANDLER
                  : EXCEPTION_CONTINUE_SEARCH)
    {
        // Swallow a single first-chance INEXACT if it sneaks in before masking.
        // After the handler, the code above will have set the proper mask.
    }
}

// ********************************************************************************
// Function Implementations

/* --------------------------------------------------------------------------------
   Function : bInitTimer
   Purpose : initialise the timer subsystem
   Parameters : 
   Returns : 
   Info : 
*/

void bInitTimer()
{
#ifdef NORMALIZEFPU
	NormalizeFPU();
#endif
    // Prefer TSC if the CPU reports it (CPUID.1: EDX bit 4)
    {
        CPUIDRegs* r = cpuid_Version_info(1);
        if (r && (r->edx & 0x10)) {
            LARGE_INTEGER qpf, t0, t1;
            QueryPerformanceFrequency(&qpf);

            // Take a TSC snapshot, then busy-wait ~0.5 s in QPC space
            const unsigned __int64 tsc0 = rdtsc();
            QueryPerformanceCounter(&t0);
            do {
                QueryPerformanceCounter(&t1);
            } while ((unsigned __int64)(t1.QuadPart - t0.QuadPart) <
                     (unsigned __int64)(qpf.QuadPart / 2));

            const unsigned __int64 tsc1 = rdtsc();

            // Frequency in Hz: delta_tsc * 2 (because waited ~0.5 s)
            bTimerFrequency = (TBTimerValue)((tsc1 - tsc0) * (unsigned __int64)2);
            bTimerType      = BTIMER_TYPE_RDTSC;

            bkPrintf("bInitTimer: Using *timestamp* timer (~%f Mhz)\n",
                     (double)bTimerFrequency * 1.0e-6);
            return;
        }
    }

    // Fallback to performance counter
    {
        LARGE_INTEGER freq;
        if (!QueryPerformanceFrequency(&freq)) {
            // Final fallback: 1 KHz ticker (GetTickCount)
            bTimerFrequency = (TBTimerValue)1000;
            bTimerType      = BTIMER_TYPE_MS;
            bkPrintf("bInitTimer: Using *ticker* timer (1 Khz)\n");
            return;
        }

        bTimerFrequency = (TBTimerValue)freq.QuadPart;
        bTimerType      = BTIMER_TYPE_QPC;
        bkPrintf("bInitTimer: Using *performance* timer (%.2f Khz)\n",
                 (double)((float)freq.QuadPart * 0.001f));
    }
}


/* --------------------------------------------------------------------------------
   Function : bShutdownTimer
   Purpose : shutdown the debugging system
   Parameters : 
   Returns : 
   Info : 
*/

void bShutdownTimer()
{
    return; // MG: empty, confirmed
}


/* --------------------------------------------------------------------------------
   Function : bkTimerRead
   Purpose : read the current timer value
   Parameters : 
   Returns : current timer value
   Info : 
*/

TBTimerValue bkTimerRead()
{
    if (bTimerType == BTIMER_TYPE_RDTSC) {
        return rdtsc();
    }
    if (bTimerType == BTIMER_TYPE_QPC) {
        LARGE_INTEGER qpc;
        QueryPerformanceCounter(&qpc);
        return (TBTimerValue)qpc.QuadPart;
    }
    // BTIMER_TYPE_MS
    return (TBTimerValue)(unsigned long)GetTickCount();
}


/* --------------------------------------------------------------------------------
   Function : bkTimerGetFrequency
   Purpose : return the number of timer ticks per second
   Parameters : 
   Returns : number of ticks per second
   Info : 
*/

// defined


/* --------------------------------------------------------------------------------
   Function : bkTimerToFPS
   Purpose : convert a timer delta value to frames per second
   Parameters : 
   Returns : #fps
   Info : 
*/

float bkTimerToFPS(TBTimerValue timerValue)
{
    if (timerValue == 0) return 0.0f;
    // FPS = ticks_per_second / ticks_per_frame
    return (float)((double)bTimerFrequency / (double)timerValue);
}


/* --------------------------------------------------------------------------------
   Function : bkTimerToScanlines
   Purpose : convert a timer delta value to a number of scanlines
   Parameters : timer value, display frequency (Hz), screen height (0=current)
   Returns : #scanlines
   Info : 
*/

float bkTimerToScanlines(TBTimerValue timerValue, int frequency, int yRes)
{
    if (yRes == 0) {
        int dummyX, dummyBpp, dummyZ;
        bdGetDisplayInfo(&dummyX, &yRes, &dummyBpp, &dummyZ);
    }

    // frames = ticks / (ticks_per_sec / Hz)
    const float frames = (float)timerValue / ((float)bTimerFrequency / (float)frequency);

    // scanlines = frames * vertical_resolution
    return frames * (float)yRes;
}


/* --------------------------------------------------------------------------------
   Function : bkTimerDelta
   Purpose : return the delta between two values accounting for timer wrap
   Parameters : start value, end value
   Returns : delta value
   Info : 
*/

TBTimerValue bkTimerDelta(TBTimerValue startValue, TBTimerValue endValue)
{
    if (bTimerType == BTIMER_TYPE_QPC) {
        // 64-bit path with the peculiar "+ -1" seen in the disasm
        if (endValue < startValue) {
            return (endValue - startValue) - (TBTimerValue)1;
        }
    } else if (endValue < startValue) {
        // TSC or MS path: add 0xffffffff on underflow (as in disasm)
        return (endValue - startValue) + (TBTimerValue)0xffffffff;
    }
    return endValue - startValue;
}


/* --------------------------------------------------------------------------------
   Function : bkSleep
   Purpose : Sleep for given number of miliseconds
   Parameters : miliseconds to sleep for
   Returns : 
   Info : 
*/

void bkSleep(int miliseconds)
{
	Sleep(miliseconds);
    return;
}