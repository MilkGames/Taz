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
        bkPrintf("*** WARNING *** bInitTimer was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
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
        bkPrintf("*** WARNING *** bkTimerRead was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
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
        bkPrintf("*** WARNING *** bkTimerToFPS was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
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
        bkPrintf("*** WARNING *** bkTimerToScanlines was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
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
        bkPrintf("*** WARNING *** bkTimerDelta was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
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