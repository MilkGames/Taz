// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : maths.h
//   Purpose : general accelerated maths functions
// Component : Generic Maths
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#ifndef __BMATHS_MATHS_H__
#define __BMATHS_MATHS_H__

// ********************************************************************************
// Constants and Macros

// Limitation: Can't use an expression in b
#define BFASTSWAP(a,b) ((a) ^= (b); (b) ^= (a); (a) ^= (b))

// Round up to multiple of 2
#define BROUNDUP_POW2(x,n)      (((uint32)(x) + (n) - 1) & ~((n) - 1))


// ********************************************************************************
// Globals


// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bmAngularLimit
   Purpose : limit an angle from -PI to PI
   Parameters : 
   Returns : 
   Info : 
*/

float bmAngularLimit(float angle);


#endif //__BMATHS_MATHS_H__
