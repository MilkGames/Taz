// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : xbTimer.h
//   Purpose : timer functions
// Component : Xbox Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BKERNEL_XBTIMER_H__
#define __BKERNEL_XBTIMER_H__

// ********************************************************************************
// Types and Structures

// value used by the timer API
typedef __int64 TBTimerValue;


// ********************************************************************************
// Include the generic timer header

#include "timer.h"


// ********************************************************************************
// Globals

extern TBTimerValue		bTimerFrequency;	// the timer frequency, or zero for no timer


// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bInitTimer
   Purpose : initialise the timer subsystem
   Parameters : 
   Returns : 
   Info : 
*/

void bInitTimer();


/* --------------------------------------------------------------------------------
   Function : bShutdownTimer
   Purpose : shutdown the debugging system
   Parameters : 
   Returns : 
   Info : 
*/

void bShutdownTimer();


/* --------------------------------------------------------------------------------
   Function : bkTimerRead
   Purpose : read the current timer value
   Parameters : 
   Returns : current timer value
   Info : 
*/

TBTimerValue bkTimerRead();


/* --------------------------------------------------------------------------------
   Function : bkTimerGetFrequency
   Purpose : return the number of timer ticks per second
   Parameters : 
   Returns : number of ticks per second
   Info : 
*/

#define bkTimerGetFrequency()	bTimerFrequency


/* --------------------------------------------------------------------------------
   Function : bkTimerToFPS
   Purpose : convert a timer delta value to frames per second
   Parameters : 
   Returns : #fps
   Info : 
*/

float bkTimerToFPS(TBTimerValue timerValue);


/* --------------------------------------------------------------------------------
   Function : bkTimerToScanlines
   Purpose : convert a timer delta value to a number of scanlines
   Parameters : timer value, display frequency (Hz), screen height (0=current)
   Returns : #scanlines
   Info : 
*/

float bkTimerToScanlines(TBTimerValue timerValue, int frequency, int yRes);


/* --------------------------------------------------------------------------------
   Function : bkTimerDelta
   Purpose : return the delta between two values accounting for timer wrap
   Parameters : start value, end value
   Returns : delta value
   Info : 
*/

TBTimerValue bkTimerDelta(TBTimerValue startValue, TBTimerValue endValue);


/* --------------------------------------------------------------------------------
   Function : bkSleep
   Purpose : Sleep for given number of miliseconds
   Parameters : miliseconds to sleep for
   Returns : 
   Info : 
*/

void bkSleep(int miliseconds);


#endif		// __BKERNEL_XBTIMER_H__
