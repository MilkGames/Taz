// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcTimer.cpp
//   Purpose : timer functions
// Component : XBOX Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

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
    return;
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
    return;
}