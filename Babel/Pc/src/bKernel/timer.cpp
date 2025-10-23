// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : timer.cpp
//   Purpose : timer functions
// Component : Generic Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/* --------------------------------------------------------------------------------
   Function : bkInitStopwatch
   Purpose : initialise a stopwatch
   Parameters : stopwatch, #counts to average
   Returns : 
   Info : 
*/

void bkInitStopwatch(TBStopwatch *stop, int maxCount)
{
    stop->count    = 0;
    stop->accum    = 0;
    stop->value    = 0;
    stop->maxCount = maxCount;
}


/* --------------------------------------------------------------------------------
   Function : bkStartStopwatch
   Purpose : start a stopwatch
   Parameters : stopwatch
   Returns : 
   Info : 
*/

void bkStartStopwatch(TBStopwatch *stop)
{
    stop->start = bkTimerRead();
}


/* --------------------------------------------------------------------------------
   Function : bkStopStopwatch
   Purpose : stop a stopwatch
   Parameters : stopwatch
   Returns : value updated?
   Info : 
*/

int bkStopStopwatch(TBStopwatch *stop)
{
    const TBTimerValue now = bkTimerRead();
    const TBTimerValue dt  = bkTimerDelta(stop->start, now);

    stop->accum += dt;

    // increment sample count
    const int cnt = ++stop->count;

    // when enough samples gathered, compute average and reset
    if (cnt == stop->maxCount) {
        // signed 64-bit division
        stop->value = stop->accum / (TBTimerValue)stop->maxCount;

        // reset accumulator and counter
        stop->accum = 0;
        stop->count = 0;
        return TRUE;
    }
    return FALSE;
}