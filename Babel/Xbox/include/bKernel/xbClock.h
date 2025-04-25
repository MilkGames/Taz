// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : clock.h
//   Purpose : Real-time clock functionality
// Component : PC Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BKERNEL_CLOCK_H__
#define __BKERNEL_CLOCK_H__

// ********************************************************************************
// Constants and Macros


// ********************************************************************************
// Types and Structures

typedef struct _TBClock {
	uchar	second;	// 0 - 59
	uchar	minute;	// 0 - 59
	uchar	hour;	// 0 - 23
	uchar	day;	// 1 - 31
	uchar	month;	// 1 - 12
	uchar	year;	// 0 - 99
} TBClock;


// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bkReadClock
   Purpose : Get real-time clock value
   Parameters : pointer to TBClock struct
   Returns : OK/FAIL
   Info : 
*/

int bkReadClock(TBClock *clock);


#endif		// __BKERNEL_CLOCK_H__
