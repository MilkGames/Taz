// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : timer.h
//   Purpose : timer functions
// Component : Generic Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BKERNEL_TIMER_H__
#define __BKERNEL_TIMER_H__

// ********************************************************************************
// Types and Structures

// stopwatch info
typedef struct _TBStopwatch {
	TBTimerValue		value;
	TBTimerValue		accum;
	TBTimerValue		start;
	int					count;
	int					maxCount;
} TBStopwatch;


// ********************************************************************************
// Globals


// ********************************************************************************
// Prototypes


/* --------------------------------------------------------------------------------
   Function : bkInitStopwatch
   Purpose : initialise a stopwatch
   Parameters : stopwatch, #counts to average
   Returns : 
   Info : 
*/

void bkInitStopwatch(TBStopwatch *stop, int maxCount);


/* --------------------------------------------------------------------------------
   Function : bkStartStopwatch
   Purpose : start a stopwatch
   Parameters : stopwatch
   Returns : 
   Info : 
*/

void bkStartStopwatch(TBStopwatch *stop);


/* --------------------------------------------------------------------------------
   Function : bkStopStopwatch
   Purpose : stop a stopwatch
   Parameters : stopwatch
   Returns : value updated?
   Info : 
*/

int bkStopStopwatch(TBStopwatch *stop);

#endif		// __BKERNEL_TIMER_H__