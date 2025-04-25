// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : xbMaths.h
//   Purpose : general accelerated maths functions
// Component : Xbox Maths
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#ifndef __BMATHS_XBMATHS_H__
#define __BMATHS_XBMATHS_H__

// ********************************************************************************
// Constants and Macros

// maximum random value
#define BRAND_MAX		32767

// 1/BRAND_MAX, used in bmFRand() to scale random to [0, 1]
#define BFRAND_RECIP	0.000030518509f

// 2/BRAND_MAX, used in bmFRandSigned() to scale random to [-1, 1]
#define BFRAND_SIGRECIP	0.000061037019f


// ********************************************************************************
// Globals

extern uint32 bHoldRand;			// random number generator held value


// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : bmFloatToInt
	Purpose : Convert a float to an int
	Parameters : floating point number
	Returns : integer number
	Info : 
*/

__inline int bmFloatToInt(float val)
{
	return (int)val;
}


/*	--------------------------------------------------------------------------------
	Function : bmRand
	Purpose : Generate a psuedo-random number from 0 - 32767
	Parameters : 
	Returns : psuedo-random number
	Info :
*/

__inline int bmRand()
{
	return (((bHoldRand = bHoldRand * 214013L + 2531011L) >> 16) & 0x7fff);
}


/*	--------------------------------------------------------------------------------
	Function : bmSRand
	Purpose : Set the seed for the random number generator
	Parameters : seed
	Returns : 
	Info :
*/

void bmSRand(uint32 seed);


/*	--------------------------------------------------------------------------------
	Function : bmCos
	Purpose : calculate the cosine of a number
	Parameters : floating point number
	Returns : cosine of number
	Info : 
*/

__inline float bmCos(float val)
{
	return (float)cos(val);
}


/*	--------------------------------------------------------------------------------
	Function : bmCosApprox
	Purpose : calculate the approximate cosine of a number
	Parameters : floating point number
	Returns : approximate cosine of number
	Info : 
*/

__inline float bmCosApprox(float val)
{
	return (float)cos(val);
}


/*	--------------------------------------------------------------------------------
	Function : bmSin
	Purpose : calculate the sine of a number
	Parameters : floating point number
	Returns : sine of number
	Info : 
*/

__inline float bmSin(float val)
{
	return (float)sin(val);
}


/*	--------------------------------------------------------------------------------
	Function : bmSinApprox
	Purpose : calculate the approximate sine of a number
	Parameters : floating point number
	Returns : approximate sine of number
	Info : 
*/

__inline float bmSinApprox(float val)
{
	return (float)sin(val);
}


/*	--------------------------------------------------------------------------------
	Function : bmTan
	Purpose : calculate the tan of a number
	Parameters : floating point number
	Returns : tan of number
	Info : 
*/

__inline float bmTan(float val)
{
	return (float)tan(val);
}


/*	--------------------------------------------------------------------------------
	Function : bmTanApprox
	Purpose : calculate the approximate tan of a number
	Parameters : floating point number
	Returns : approximate tan of number
	Info : 
*/

__inline float bmTanApprox(float val)
{
	return (float)tan(val);
}


/*	--------------------------------------------------------------------------------
	Function : bmACos
	Purpose : calculate the arccosine of a number
	Parameters : floating point number
	Returns : arccosine of number
	Info : 
*/

__inline float bmACos(float val)
{
	BASSERT(BASSERT_MATH_INVALID_INPUT, val >= -1.0f && val <= 1.0f, "bmACos: invalid input %0.4f\n", val);

	return (float)acos(val);
}


/*	--------------------------------------------------------------------------------
	Function : bmACosApprox
	Purpose : calculate the approximate arccosine of a number
	Parameters : floating point number
	Returns : approximate arccosine of number
	Info : 
*/

#define bmACosApprox(X) bmACos(X)


/*	--------------------------------------------------------------------------------
	Function : bmASin
	Purpose : calculate the arcsine of a number
	Parameters : floating point number
	Returns : arcsine of number
	Info : 
*/

__inline float bmASin(float val)
{
	BASSERT(BASSERT_MATH_INVALID_INPUT, val >= -1.0f && val <= 1.0f, "bmASin: invalid input %0.4f\n", val);

	return (float)asin(val);
}


/*	--------------------------------------------------------------------------------
	Function : bmASinApprox
	Purpose : calculate the approximate arcsine of a number
	Parameters : floating point number
	Returns : approximate arcsine of number
	Info : 
*/

#define bmASinApprox(X) bmASin(X)


/*	--------------------------------------------------------------------------------
	Function : bmATan
	Purpose : calculate the arctan of a number
	Parameters : floating point number
	Returns : arctan of number
	Info : 
*/

__inline float bmATan(float val)
{
	return (float)atan(val);
}


/*	--------------------------------------------------------------------------------
	Function : bmATanApprox
	Purpose : calculate the approximate tan of a number
	Parameters : floating point number
	Returns : approximate tan of number
	Info : 
*/

__inline float bmATanApprox(float val)
{
	return (float)atan(val);
}


/*	--------------------------------------------------------------------------------
	Function : bmATan2
	Purpose : calculate the arctan of y/x
	Parameters : 'y' value, 'x' value
	Returns : arctan of y/x
	Info : 
*/

__inline float bmATan2(float y, float x)
{
	return (float)atan2(y, x);
}


/*	--------------------------------------------------------------------------------
	Function : bmATan2Approx
	Purpose : calculate the approximate arctan of y/x
	Parameters : 'y' value, 'x' value
	Returns : approximate arctan of y/x
	Info : 
*/

__inline float bmATan2Approx(float y, float x)
{
	return (float)atan2(y, x);
}


/*	--------------------------------------------------------------------------------
	Function : bmFAbs
	Purpose : calculate the absolute value of a floating point number
	Parameters : value
	Returns : absolute value
	Info : 
*/

__inline float bmFAbs(float x)
{
	__asm
	{
		and [x], 07FFFFFFFh
	};
	return(x);
}


/*	--------------------------------------------------------------------------------
	Function : bmFMod
	Purpose : calculate the floating point remainder after dividing by the divisor
	Parameters : value, divisor
	Returns : remainer after a divide
	Info : 
*/

__inline float bmFMod(float x, float y)
{
	int		idiv;
	float	px,py,v;

	if (x==0)
		return x;
	if (y==0)
		return x;

	px = bmFAbs(x);
	py = bmFAbs(y);

	// All +ve values
	idiv = (int)(px/py);	// round down to integer
	v    = py*(float)idiv;	// whole number of y values in x

	return (x>0) ? (x-v) : (x+v);
}


/*	--------------------------------------------------------------------------------
	Function : bmFloor
	Purpose : floating-point value representing the largest integer that is less than or equal to x
	Parameters : value
	Returns : floored value
	Info : 
*/

#define bmFloor(X) ((float)floor(X))


/*	--------------------------------------------------------------------------------
	Function : bmSqrt
	Purpose : Square root of a number
	Parameters : floating point number
	Returns : square root of number
	Info : 
*/

__inline float bmSqrt(float x)
{
	return (float)sqrt(x);
}


/*	--------------------------------------------------------------------------------
	Function : bmSqrtApprox
	Purpose : Approximate square root of a number
	Parameters : floating point number
	Returns : approximate square root of number
	Info : 
*/

float bmSqrtApprox(float x);


/*	--------------------------------------------------------------------------------
	Function : bmOneOverSqrt
	Purpose : calculate one over the square root of a number
	Parameters : floating point number
	Returns : 1/square root of number
	Info : 
*/

__inline float bmOneOverSqrt(float x)
{
	return 1.0f/(float)sqrt(x);
}


/*	--------------------------------------------------------------------------------
	Function : bmOneOverSqrtApprox
	Purpose : Approximate one over square root of a number
	Parameters : floating point number
	Returns : approximate 1/square root of number
	Info : 
*/

__inline float bmOneOverSqrtApprox(float x)
{
	return 1.0f/bmSqrtApprox(x);
}


/*	--------------------------------------------------------------------------------
	Function : bmCosToSinApprox
	Purpose : Convert from the cosine of an angle to the sine of an angle.
	Parameters : [in] the cosine of an angle (radians)
	Returns : the sine of the same angle.
	Info : uses the identity cos^2 A + sin^2 A == 1, and uses the APPROXIMATE square root for that.
*/

inline float bmCosToSinApprox(float cos)
{
	return bmSqrtApprox(1 - (cos * cos));
}


/*	--------------------------------------------------------------------------------
	Function : bmCosToSin
	Purpose : Convert from the cosine of an angle to the sine of an angle.
	Parameters : [in] the cosine of an angle (radians)
	Returns : the sine of the same angle.
	Info : uses the identity cos^2 A + sin^2 A == 1
*/

inline float bmCosToSin(float cos)
{
	return bmSqrt(1 - (cos * cos));
}


/*	--------------------------------------------------------------------------------
	Function : bmSinToCosApprox
	Purpose : Convert from the sine of an angle to the cosine of an angle.
	Parameters : [in] the sine of an angle (radians)
	Returns : the cosine of the same angle.
	Info : uses the identity cos^2 A + sin^2 A == 1, and uses the APPROXIMATE square root for that.
*/

inline float bmSinToCosApprox(float sin)
{
	return bmSqrtApprox(1 - (sin * sin));
}


/*	--------------------------------------------------------------------------------
	Function : bmSinToCos
	Purpose : Convert from the cosine of an angle to the sine of an angle.
	Parameters : [in] the cosine of an angle (radians)
	Returns : the sine of the same angle.
	Info : uses the identity cos^2 A + sin^2 A == 1
*/

inline float bmSinToCos(float sin)
{
	return bmSqrt(1 - (sin * sin));
}


/*	--------------------------------------------------------------------------------
	Function : bmLog
	Purpose : Calculate the natural logarithm of a number
	Parameters : number to calculate logarithm of
	Returns : logarithm of value
	Info : 
*/

inline float bmLog(float value)
{
	return (float)log(value);
}


/*	--------------------------------------------------------------------------------
	Function : bmLog10
	Purpose : Calculate the base-10 logarithm of a number
	Parameters : number to calculate logarithm of
	Returns : logarithm of value
	Info : 
*/

inline float bmLog10(float value)
{
	return (float)log10(value);
}


/*	--------------------------------------------------------------------------------
	Function : bmPow
	Purpose : Calculate X to the power Y
	Parameters : X, Y
	Returns : X to the power of Y
	Info : 
*/

inline float bmPow(float x, float y)
{
	return (float)pow(x, y);
}


/*	--------------------------------------------------------------------------------
	Function : bmExp
	Purpose : Calculate exponential of a value
	Parameters : value
	Returns : exponential of value
	Info : 
*/

inline float bmExp(float x)
{
	return (float)exp(x);
}


/*	--------------------------------------------------------------------------------
	Function : bmCosh
	Purpose : calculate the hyperbolic cosine of a number
	Parameters : floating point number
	Returns : hyperbolic cosine of number
	Info : 
*/

__inline float bmCosh(float val)
{
	return (float)cosh(val);
}


/*	--------------------------------------------------------------------------------
	Function : bmSinh
	Purpose : calculate the hyperbolic sine of a number
	Parameters : floating point number
	Returns : hyperbolic sine of number
	Info : 
*/

__inline float bmSinh(float val)
{
	return (float)sinh(val);
}


/*	--------------------------------------------------------------------------------
	Function : bmTanh
	Purpose : calculate the hyperbolic tangent of a number
	Parameters : floating point number
	Returns : hyperbolic tangent of number
	Info : 
*/

__inline float bmTanh(float val)
{
	return (float)tanh(val);
}


/*	--------------------------------------------------------------------------------
	Function : bmRandf
	Purpose : generate a psuedo-random floating point number between zero and one
	Parameters : 
	Returns : pseudo-random floating point number in the range [0, 1]
	Info : 
*/

__inline float bmRandf()
{
	return BFRAND_RECIP*(float)bmRand();
}


/*	--------------------------------------------------------------------------------
	Function : bmRandfSigned
	Purpose : generate a psuedo-random floating point number between +- one
	Parameters : 
	Returns : pseudo-random floating point number in the range [-1, 1]
	Info : 
*/

__inline float bmRandfSigned()
{
	return (BFRAND_SIGRECIP*(float)bmRand())-1;
}


/*	--------------------------------------------------------------------------------
	Function : bmFSnap
	Purpose : Snap a floating point value to an expected minimum or maximum.
	Parameters : [in] the value to snap
				[in] the value to snap TO.
				[in(opt)] the epsilon value (integer!)
	Returns : val, if the difference was too great, snapVal if the difference was miniscule.
	Info : This function coverts to ints, because I can't rely on the exponent being constant, and therefore can't create a decent epsilon value. Floats, eh?
*/

__inline float bmFSnap(const float val, const float snapVal, const int32 snapRange=8)
{
	int32					ival, isnapVal;

	ival = *(int32 *)&val;
	isnapVal = *(int32 *)&snapVal;

	if (abs(ival - isnapVal) <= snapRange)
		return snapVal;

	return val;
}


#endif //__BMATHS_XBMATHS_H__
