// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : vector2D.h
//   Purpose : 2D vector functions
// Component : Generic Maths
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BMATHS_VECTOR2D_H__
#define __BMATHS_VECTOR2D_H__

// ********************************************************************************
// Constants and Macros


// ********************************************************************************
// Types, Structures and Classes

// our vector type
typedef float	TBVector2D[2];


// ********************************************************************************
// Prototypes


/*	--------------------------------------------------------------------------------
	Function : bmVector2DAdd
	Purpose : Add two 2D vectors together (dest = src1 + src2)
	Parameters : [out] destination
				[in] source1
				[in] source2
	Returns : 
	Info : 
*/

inline void bmVector2DAdd(TBVector2D dest, const TBVector2D source1, const TBVector2D source2)
{
	dest[0] = source1[0] + source2[0];
	dest[1] = source1[1] + source2[1];
}


/*	--------------------------------------------------------------------------------
	Function : bmVector2DSub
	Purpose : Subtract two 2D vectors from each other (dest = src1 - src2)
	Parameters : [out] destination
				[in] source1
				[in] source2
	Returns : 
	Info : 
*/

inline void bmVector2DSub(TBVector2D dest, const TBVector2D source1, const TBVector2D source2)
{
	dest[0] = source1[0] - source2[0];
	dest[1] = source1[1] - source2[1];
}


/*	--------------------------------------------------------------------------------
	Function : bmVector2DMul
	Purpose : Multiply two 2D vectors together (dest = src1 * src2)
	Parameters : [out] destination
				[in] source1
				[in] source2
	Returns : 
	Info : 
*/

inline void bmVector2DMul(TBVector2D dest, const TBVector2D source1, const TBVector2D source2)
{
	dest[0] = source1[0] * source2[0];
	dest[1] = source1[1] * source2[1];
}


/*	--------------------------------------------------------------------------------
	Function : bmVector2DDiv
	Purpose : Divide source1 by source2 (dest = src1 / src2)
	Parameters : [out] destination
				[in] source1
				[in] source2
	Returns : 
	Info : 
*/

inline void bmVector2DDiv(TBVector2D dest, const TBVector2D source1, const TBVector2D source2)
{
	dest[0] = source1[0] / source2[0];
	dest[1] = source1[1] / source2[1];
}


/*	--------------------------------------------------------------------------------
	Function : bmVector2DCAdd
	Purpose : Add constant value to vector (dest = source1 + constant)
	Parameters : [out] destination
				[in] source1
				[in] constant value
	Returns : 
	Info : 
*/

inline void bmVector2DCAdd(TBVector2D dest, const TBVector2D source1, const float constant)
{
	dest[0] = source1[0] + constant;
	dest[1] = source1[1] + constant;
}


/*	--------------------------------------------------------------------------------
	Function : bmVector2DCSub
	Purpose : Subtract constant value from a vector (dest = source1 - constant)
	Parameters : [out] destination
				[in] source1
				[in] constant value
	Returns : 
	Info : 
*/

inline void bmVector2DCSub(TBVector2D dest, const TBVector2D source1, const float constant)
{
	dest[0] = source1[0] - constant;
	dest[1] = source1[1] - constant;
}


/*	--------------------------------------------------------------------------------
	Function : bmVector2DCMul
	Purpose : Multiply vector by a constant value (dest = source1 * constant)
	Parameters : [out] destination
				[in] source1
				[in] constant value
	Returns : 
	Info : 
*/
inline void bmVector2DCMul(TBVector2D dest, const TBVector2D source1, const float constant)
{
	dest[0] = source1[0] * constant;
	dest[1] = source1[1] * constant;
}


/*	--------------------------------------------------------------------------------
	Function : bmVector2DCDiv
	Purpose : Divide vector by a constant value (dest = source1 / constant)
	Parameters : [out] destination
				[in] source1
				[in] constant value
	Returns : 
	Info : 
*/

inline void bmVector2DCDiv(TBVector2D dest, const TBVector2D source1, const float constant)
{
	dest[0] = source1[0] / constant;
	dest[1] = source1[1] / constant;
}


/*	--------------------------------------------------------------------------------
	Function : bmVector2DNegate
	Purpose : Negates the vector
	Parameters : [out] destination
				[in] source
	Returns : 
	Info : 
*/

inline void bmVector2DNegate(TBVector2D dest, const TBVector2D source)
{
	dest[0] = -source[0];
	dest[1] = -source[1];
}


/*	--------------------------------------------------------------------------------
	Function : bmVector2DLen
	Purpose : Finds the length of the vector
	Parameters : [in] source
	Returns : length of the vector
	Info : 
*/

inline float bmVector2DLen(const TBVector2D source)
{
	return bmSqrt( (source[0] * source[0]) + (source[1] * source[1]) );
}


/*	--------------------------------------------------------------------------------
	Function : bmVector2DSquaredLen
	Purpose : Finds the squared length of the vector
	Parameters : [in] source
	Returns : squared length of the vector
	Info : 
*/

inline float bmVector2DSquaredLen(const TBVector2D source)
{
	return (source[0] * source[0]) + (source[1] * source[1]);
}


/*	--------------------------------------------------------------------------------
	Function : bmVector2DNorm
	Purpose : Normalises the vector
	Parameters : [out] destination
				[in] source
	Returns : length of the source vector
	Info : 
*/

inline float bmVector2DNorm(TBVector2D dest, const TBVector2D source)
{
	float				len;

	if ((len = bmVector2DLen(source)) == 0.0f)
		return len;

	dest[0] = source[0] / len;
	dest[1] = source[1] / len;

	return len;
}


/*	--------------------------------------------------------------------------------
	Function : bmVector2DScaleToLength
	Purpose : Scales the vector to <length>
	Parameters : [out] destination
				[in] source
				[in] desired length
	Returns : 
	Info : 
*/

inline void bmVector2DScaleToLength(TBVector2D dest, const TBVector2D source, const float length)
{
	float				len;

	len = length / bmVector2DLen(source);

	dest[0] = source[0] * len;
	dest[1] = source[1] * len;
}


/*	--------------------------------------------------------------------------------
	Function : bmVector2DCopy
	Purpose : Copy from one vector to another
	Parameters : [out] destination
				[in] source
	Returns : 
	Info : 
*/

inline void bmVector2DCopy(TBVector2D dest, const TBVector2D source)
{
	dest[0] = source[0];
	dest[1] = source[1];
}


/*	--------------------------------------------------------------------------------
	Function : bmVector2DDot
	Purpose : Calculate the Dot product of two 2D vectors.
	Parameters : [in] source1
				[in] source2
	Returns : 2D Dot Product of the vectors.
	Info : 
*/

inline float bmVector2DDot(const TBVector2D source1, const TBVector2D source2)
{
	return (source1[0] * source2[0]) + (source1[1] * source2[1]);
}


/*	--------------------------------------------------------------------------------
	Function : bmVector2DDistance
	Purpose : Finds the distance between two vectors
	Parameters : [in] source1
				[in] source2
	Returns : distance between two vectors
	Info : 
*/

inline float bmVector2DDistance(const TBVector2D source1, const TBVector2D source2)
{
	TBVector2D					difference;
	
	bmVector2DSub(difference, source1, source2);

	return bmSqrt( (difference[0] * difference[0]) + (difference[1] * difference[1]) );
}


/*	--------------------------------------------------------------------------------
	Function : bmVector2DSquaredDistance
	Purpose : Finds the squared distance between two vectors
	Parameters : [in] source1
				[in] source2
	Returns : Squared distance between two vectors
	Info : 
*/

inline float bmVector2DSquaredDistance(const TBVector2D source1, const TBVector2D source2)
{
	TBVector2D					difference;
	
	bmVector2DSub(difference, source1, source2);

	return (difference[0] * difference[0]) + (difference[1] * difference[1]);
}


/*	--------------------------------------------------------------------------------
	Function : bmVector2DLerp
	Purpose : interpolate between two 2D vectors
	Parameters : [out] destination
				[in] source1
				[in] source2
				[in] ratio of source1 to source2
	Returns : 
	Info : 
*/

inline void bmVector2DLerp(TBVector2D dest, const TBVector2D src1, const TBVector2D src2, const float t)
{
	float oneMinusT = 1.0f - t;

	dest[0] = oneMinusT * src1[0] + t * src2[0];
	dest[1] = oneMinusT * src1[1] + t * src2[1];
}


/*	--------------------------------------------------------------------------------
	Function : bmVector2DCombine
	Purpose : combine two vectors (dest = source1 * factor1 + source2 * factor2)
	Parameters : [out] destination
				[in] source1
				[in] factor1
				[in] source2
				[in] factor2
	Returns : 
	Info : 
*/

inline void bmVector2DCombine(TBVector2D dest, const TBVector2D src1, const float factor1, const TBVector2D src2, const float factor2)
{
	dest[0] = src1[0] * factor1 + src2[0] * factor2;
	dest[1] = src1[1] * factor1 + src2[1] * factor2;
}


#endif // __BMATHS_VECTOR2D_H__
