// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcClock.cpp
//   Purpose : Real-time clock functionality
// Component : Xbox Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/* --------------------------------------------------------------------------------
   Function : bkReadClock
   Purpose : Get real-time clock value
   Parameters : pointer to TBClock struct
   Returns : OK/FAIL
   Info : 
*/

int bkReadClock(TBClock *clock)

{
  _SYSTEMTIME systemTime;
  
  GetLocalTime(&systemTime);
  clock->minute = (uchar)systemTime.wMinute;
  clock->second = (uchar)systemTime.wSecond;
  clock->day = (uchar)systemTime.wDay;
  clock->hour = (uchar)systemTime.wHour;
  clock->month = (uchar)systemTime.wMonth;
  clock->year = (uchar)(systemTime.wYear % 100);
  
  return OK;
}
