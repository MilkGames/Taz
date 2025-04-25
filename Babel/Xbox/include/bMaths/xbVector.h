// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : xbVector.h
//   Purpose : vector functions
// Component : Xbox Maths
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#ifndef __BMATHS_XBVECTOR_H__
#define __BMATHS_XBVECTOR_H__

// ********************************************************************************
// Constants and Macros


// ********************************************************************************
// Types, Structures and Classes

// our vector type
typedef float	TBVector[4];

// our quaternion type
typedef float	TBQuaternion[4];


// ********************************************************************************
// Globals


// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : bmVectorAdd
	Purpose : add two vectors together (dest = src1 + src2)
	Parameters : destination, source1, source2
	Returns : 
	Info : 
*/

__inline void bmVectorAdd(TBVector dest, const TBVector src1, const TBVector src2)
{
	dest[0] = src1[0] + src2[0];
	dest[1] = src1[1] + src2[1];
	dest[2] = src1[2] + src2[2];
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorAdd4
	Purpose : add two vectors together (dest = src1 + src2)
	Parameters : destination, source1, source2
	Returns : 
	Info : 
*/

__inline void bmVectorAdd4(TBVector dest, const TBVector src1, const TBVector src2)
{
	dest[0] = src1[0] + src2[0];
	dest[1] = src1[1] + src2[1];
	dest[2] = src1[2] + src2[2];
	dest[3] = src1[3] + src2[3];
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorSub
	Purpose : subtract src2 from src1 (dest = src1 - src2)
	Parameters : destination, source1, source2
	Returns : 
	Info : 
*/

__inline void bmVectorSub(TBVector dest, const TBVector src1, const TBVector src2)
{
	dest[0] = src1[0] - src2[0];
	dest[1] = src1[1] - src2[1];
	dest[2] = src1[2] - src2[2];
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorSub
	Purpose : subtract src2 from src1 (dest = src1 - src2)
	Parameters : destination, source1, source2
	Returns : 
	Info : 
*/

__inline void bmVectorSub4(TBVector dest, const TBVector src1, const TBVector src2)
{
	dest[0] = src1[0] - src2[0];
	dest[1] = src1[1] - src2[1];
	dest[2] = src1[2] - src2[2];
	dest[3] = src1[3] - src2[3];
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorMul
	Purpose : multiply two vectors together (dest = src1 * src2)
	Parameters : destination, source1, source2
	Returns : 
	Info : 
*/

__inline void bmVectorMul(TBVector dest, const TBVector src1, const TBVector src2)
{
	dest[0] = src1[0] * src2[0];
	dest[1] = src1[1] * src2[1];
	dest[2] = src1[2] * src2[2];
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorMul4
	Purpose : multiply two vectors together (dest = src1 * src2)
	Parameters : destination, source1, source2
	Returns : 
	Info : 
*/

__inline void bmVectorMul4(TBVector dest, const TBVector src1, const TBVector src2)
{
	dest[0] = src1[0] * src2[0];
	dest[1] = src1[1] * src2[1];
	dest[2] = src1[2] * src2[2];
	dest[3] = src1[3] * src2[3];
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorDiv
	Purpose : divide src1 by src2 (dest = src1 / src2)
	Parameters : destination, source1, source2
	Returns : 
	Info : 
*/

__inline void bmVectorDiv(TBVector dest, const TBVector src1, const TBVector src2)
{
	dest[0] = src1[0] / src2[0];
	dest[1] = src1[1] / src2[1];
	dest[2] = src1[2] / src2[2];
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorDiv4
	Purpose : divide src1 by src2 (dest = src1 / src2)
	Parameters : destination, source1, source2
	Returns : 
	Info : 
*/

__inline void bmVectorDiv4(TBVector dest, const TBVector src1, const TBVector src2)
{
	dest[0] = src1[0] / src2[0];
	dest[1] = src1[1] / src2[1];
	dest[2] = src1[2] / src2[2];
	dest[3] = src1[3] / src2[3];
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorCAdd
	Purpose : add constant value to a vector
	Parameters : destination, source, value
	Returns : 
	Info : 
*/

__inline void bmVectorCAdd(TBVector dest, const TBVector src, const float val)
{
	dest[0] = src[0] + val;
	dest[1] = src[1] + val;
	dest[2] = src[2] + val;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorCAdd4
	Purpose : add constant value to a vector
	Parameters : destination, source, value
	Returns : 
	Info : 
*/

__inline void bmVectorCAdd4(TBVector dest, const TBVector src, const float val)
{
	dest[0] = src[0] + val;
	dest[1] = src[1] + val;
	dest[2] = src[2] + val;
	dest[3] = src[3] + val;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorCSub
	Purpose : subtract constant value from a vector
	Parameters : destination, source, value
	Returns : 
	Info : 
*/

__inline void bmVectorCSub(TBVector dest, const TBVector src, const float val)
{
	dest[0] = src[0] - val;
	dest[1] = src[1] - val;
	dest[2] = src[2] - val;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorCSub4
	Purpose : subtract constant value from a vector
	Parameters : destination, source, value
	Returns : 
	Info : 
*/

__inline void bmVectorCSub4(TBVector dest, const TBVector src, const float val)
{
	dest[0] = src[0] - val;
	dest[1] = src[1] - val;
	dest[2] = src[2] - val;
	dest[3] = src[3] - val;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorCMul
	Purpose : multiply vector by constant value
	Parameters : destination, source, value
	Returns : 
	Info : 
*/

__inline void bmVectorCMul(TBVector dest, const TBVector src, const float val)
{
	dest[0] = src[0] * val;
	dest[1] = src[1] * val;
	dest[2] = src[2] * val;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorCMul4
	Purpose : multiply vector by constant value
	Parameters : destination, source, value
	Returns : 
	Info : 
*/

__inline void bmVectorCMul4(TBVector dest, const TBVector src, const float val)
{
	dest[0] = src[0] * val;
	dest[1] = src[1] * val;
	dest[2] = src[2] * val;
	dest[3] = src[3] * val;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorCDiv
	Purpose : divide vector by constant value
	Parameters : destination, source, value
	Returns : 
	Info : 
*/

__inline void bmVectorCDiv(TBVector dest, const TBVector src, const float val)
{
	dest[0] = src[0] / val;
	dest[1] = src[1] / val;
	dest[2] = src[2] / val;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorCDiv4
	Purpose : divide vector by constant value
	Parameters : destination, source, value
	Returns : 
	Info : 
*/

__inline void bmVectorCDiv4(TBVector dest, const TBVector src, const float val)
{
	dest[0] = src[0] / val;
	dest[1] = src[1] / val;
	dest[2] = src[2] / val;
	dest[3] = src[3] / val;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorNegate
	Purpose : Negates a vector
	Parameters : dest, source
	Returns :
	Info : 
*/

__inline void bmVectorNegate(TBVector dest, const TBVector src)
{
	dest[0] = -src[0];
	dest[1] = -src[1];
	dest[2] = -src[2];
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorNegate4
	Purpose : Negates a vector
	Parameters : dest, source
	Returns :
	Info : 
*/

__inline void bmVectorNegate4(TBVector dest, const TBVector src)
{
	dest[0] = -src[0];
	dest[1] = -src[1];
	dest[2] = -src[2];
	dest[3] = -src[3];
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorToUByte4
	Purpose : convert a normalised vector to a 32bit ubyte array
	Parameters : source vector
	Returns : ubyte values (0xFF<R><G><B>)
	Info : 
*/

__inline uint32 bmVectorToUByte4(TBVector src)
{
	return ((127+((int)(src[0]*127.0f)))<<16) | ((127+((int)(src[1]*127.0f)))<<8) | (127+((int)(src[2]*127.0f)))
																											| 0xff000000;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorNorm
	Purpose : normalise a vector
	Parameters : destination, source
	Returns : length of source vector
	Info : 
*/

float bmVectorNorm(TBVector dest, const TBVector src);


/*	--------------------------------------------------------------------------------
	Function : bmVectorNorm4
	Purpose : normalise a vector
	Parameters : destination, source
	Returns : length of source vector
	Info : 
*/

float bmVectorNorm4(TBVector dest, const TBVector src);


/*	--------------------------------------------------------------------------------
	Function : bmVectorLen
	Purpose : return the length of a vector
	Parameters : source
	Returns : length
	Info : 
*/

BINLINE float bmVectorLen(const TBVector src)
{
	return bmSqrt(src[0] * src[0] + src[1] * src[1] + src[2] * src[2]);
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorLenApprox
	Purpose : return the approximate length of a vector
	Parameters : source
	Returns : length
	Info : 
*/

BINLINE float bmVectorLenApprox(const TBVector src)
{
	return bmSqrtApprox(src[0] * src[0] + src[1] * src[1] + src[2] * src[2]);
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorLen4
	Purpose : return the length of a vector
	Parameters : source
	Returns : length
	Info : 
*/

BINLINE float bmVectorLen4(const TBVector src)
{
	return bmSqrt(src[0] * src[0] + src[1] * src[1] + src[2] * src[2] + src[3] * src[3]);
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorSquaredLen
	Purpose : return the squared length of a vector
	Parameters : source
	Returns : squared length
	Info : 
*/

__inline float bmVectorSquaredLen(const TBVector src)
{
	return src[0] * src[0] + src[1] * src[1] + src[2] * src[2];
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorSquaredLen4
	Purpose : return the squared length of a vector
	Parameters : source
	Returns : squared length
	Info : 
*/

__inline float bmVectorSquaredLen4(const TBVector src)
{
	return src[0] * src[0] + src[1] * src[1] + src[2] * src[2] + src[3] * src[3];
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorDistance
	Purpose : distance between two vector
	Parameters : source1, source2
	Returns : distance
	Info : 
*/

float bmVectorDistance(const TBVector src1, const TBVector src2);


/*	--------------------------------------------------------------------------------
	Function : bmVectorDistanceApprox
	Purpose : approximate distance between two vectors
	Parameters : source1, source2
	Returns : distance
	Info : 
*/

float bmVectorDistanceApprox(const TBVector src1, const TBVector src2);


/*	--------------------------------------------------------------------------------
	Function : bmVectorDistance4
	Purpose : distance between two vector
	Parameters : source1, source2
	Returns : distance
	Info : 
*/

float bmVectorDistance4(const TBVector src1, const TBVector src2);


/*	--------------------------------------------------------------------------------
	Function : bmVectorSquaredDistance
	Purpose : squared distance between two vector
	Parameters : source1, source2
	Returns : squared distance
	Info : 
*/

float bmVectorSquaredDistance(const TBVector src1, const TBVector src2);


/*	--------------------------------------------------------------------------------
	Function : bmVectorSquaredDistance4
	Purpose : squared distance between two vector
	Parameters : source1, source2
	Returns : squared distance
	Info : 
*/

float bmVectorSquaredDistance4(const TBVector src1, const TBVector src2);


/*	--------------------------------------------------------------------------------
	Function : bmVectorCross
	Purpose : cross product of two vectors
	Parameters : destination, source1, source2
	Returns : 
	Info : 
*/

__inline void bmVectorCross(TBVector dest, const TBVector src1, const TBVector src2)
{
	dest[0] = src1[1] * src2[2] - src1[2] * src2[1];
	dest[1] = src1[2] * src2[0] - src1[0] * src2[2];
	dest[2] = src1[0] * src2[1] - src1[1] * src2[0];
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorDot
	Purpose : dot product of two vectors
	Parameters : source1, source2
	Returns : dot product
	Info : 
*/

__inline float bmVectorDot(const TBVector src1, const TBVector src2)
{
	return src1[0] * src2[0] + src1[1] * src2[1] + src1[2] * src2[2];
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorDot4
	Purpose : dot product of two vectors
	Parameters : source1, source2
	Returns : dot product
	Info : 
*/

__inline float bmVectorDot4(const TBVector src1, const TBVector src2)
{
	return src1[0] * src2[0] + src1[1] * src2[1] + src1[2] * src2[2] + src1[3] * src2[3];
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorScaleToLength
	Purpose : scale vector to specified length
	Parameters : destination, source, length
	Returns : 
	Info : 
*/

void bmVectorScaleToLength(TBVector dest, const TBVector src, const float length);


/*	--------------------------------------------------------------------------------
	Function : bmVectorScaleToLength4
	Purpose : scale vector to specified length
	Parameters : destination, source, length
	Returns : 
	Info : 
*/

void bmVectorScaleToLength4(TBVector dest, const TBVector src, const float length);


/*	--------------------------------------------------------------------------------
	Function : bmVectorLerp
	Purpose : interpolate between two vectors
	Parameters : destination, source1, source2, t
	Returns : 
	Info : 
*/

void bmVectorLerp(TBVector dest, const TBVector src1, const TBVector src2, const float t);


/*	--------------------------------------------------------------------------------
	Function : bmVectorLerp4
	Purpose : interpolate between two vectors
	Parameters : destination, source1, source2, t
	Returns : 
	Info : 
*/

void bmVectorLerp4(TBVector dest, const TBVector src1, const TBVector src2, const float t);


/*	--------------------------------------------------------------------------------
	Function : bmVectorCombine
	Purpose : combine two vectors (dest = source1 * factor1 + source2 * factor2)
	Parameters : destination, source1, factor1, source2, factor2
	Returns : 
	Info : 
*/

void bmVectorCombine(TBVector dest, const TBVector src1, const float factor1, const TBVector src2, const float factor2);


/*	--------------------------------------------------------------------------------
	Function : bmVectorCombine4
	Purpose : combine two vectors (dest = source1 * factor1 + source2 * factor2)
	Parameters : destination, source1, factor1, source2, factor2
	Returns : 
	Info : 
*/

void bmVectorCombine4(TBVector dest, const TBVector src1, const float factor1, const TBVector src2, const float factor2);


/*	--------------------------------------------------------------------------------
	Function : bmVectorCalcNormal
	Purpose : Calculate normal of plane given three points on it
	Parameters : destination, source1, source2, source3
	Returns : 
	Info : 
*/

void bmVectorCalcNormal(TBVector dest, const TBVector src1, const TBVector src2, const TBVector src3);


/*	--------------------------------------------------------------------------------
	Function : bmVectorCopy
	Purpose : copy contents of a vector
	Parameters : destination, source
	Returns : 
	Info : 
*/

__inline void bmVectorCopy(TBVector dest, const TBVector src)
{
	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = src[2];
	dest[3] = src[3];
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorSet
	Purpose : set the contents of a vector
	Parameters : destination, x, y, z, w
	Returns : 
	Info : 
*/

__inline void bmVectorSet(TBVector dest, float x, float y, float z, float w)
{
	dest[0] = x;
	dest[1] = y;
	dest[2] = z;
	dest[3] = w;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorZero
	Purpose : set a vector to (0,0,0, 1)
	Parameters : destination
	Returns : 
	Info : 
*/

__inline void bmVectorZero(TBVector dest)
{
	dest[0] = 0.0f;
	dest[1] = 0.0f;
	dest[2] = 0.0f;
	dest[3] = 1.0f;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateX
	Purpose : rotate vector in x axis
	Parameters : destination, source, radians
	Returns : 
	Info : 
*/

void bmVectorRotateX(TBVector dest, const TBVector src, const float rads);


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateY
	Purpose : rotate vector in y axis
	Parameters : destination, source, radians
	Returns : 
	Info : 
*/

void bmVectorRotateY(TBVector dest, const TBVector src, const float rads);


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateZ
	Purpose : rotate vector in z axis
	Parameters : destination, source, radians
	Returns : 
	Info : 
*/

void bmVectorRotateZ(TBVector dest, const TBVector src, const float rads);


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateXY
	Purpose : rotate vector in x, then y axis
	Parameters : destination, source, x radians, y radians
	Returns : 
	Info : 
*/

void bmVectorRotateXY(TBVector dest, const TBVector src, const float xrads, const float yrads);


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateXZ
	Purpose : rotate vector in x, then z axis
	Parameters : destination, source, x radians, z radians
	Returns : 
	Info : 
*/

void bmVectorRotateXZ(TBVector dest, const TBVector src, const float xrads, const float zrads);


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateYX
	Purpose : rotate vector in y, then x axis
	Parameters : destination, source, y radians, x radians
	Returns : 
	Info : 
*/

void bmVectorRotateYX(TBVector dest, const TBVector src, const float yrads, const float xrads);


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateYZ
	Purpose : rotate vector in y, then z axis
	Parameters : destination, source, y radians, z radians
	Returns : 
	Info : 
*/

void bmVectorRotateYZ(TBVector dest, const TBVector src, const float yrads, const float zrads);


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateZX
	Purpose : rotate vector in z, then x axis
	Parameters : destination, source, z radians, x radians
	Returns : 
	Info : 
*/

void bmVectorRotateZX(TBVector dest, const TBVector src, const float zrads, const float xrads);


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateZY
	Purpose : rotate vector in z, then y axis
	Parameters : destination, source, z radians, y radians
	Returns : 
	Info : 
*/

void bmVectorRotateZY(TBVector dest, const TBVector src, const float zrads, const float yrads);


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateXYZ
	Purpose : rotate vector in x, then y , then z axis
	Parameters : destination, source, x radians, y radians, z radians
	Returns : 
	Info : 
*/

void bmVectorRotateXYZ(TBVector dest, const TBVector src, const float xrads, const float yrads, const float zrads);


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateXZY
	Purpose : rotate vector in x, then z , then y axis
	Parameters : destination, source, x radians, z radians, y radians
	Returns : 
	Info : 
*/

void bmVectorRotateXZY(TBVector dest, const TBVector src, const float xrads, const float zrads, const float yrads);


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateYXZ
	Purpose : rotate vector in y, then x , then z axis
	Parameters : destination, source, y radians, x radians, z radians
	Returns : 
	Info : 
*/

void bmVectorRotateYXZ(TBVector dest, const TBVector src, const float yrads, const float xrads, const float zrads);


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateYZX
	Purpose : rotate vector in y, then z , then x axis
	Parameters : destination, source, y radians, z radians, x radians
	Returns : 
	Info : 
*/

void bmVectorRotateYZX(TBVector dest, const TBVector src, const float yrads, const float zrads, const float xrads);


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateZXY
	Purpose : rotate vector in z, then x , then y axis
	Parameters : destination, source, z radians, x radians, y radians
	Returns : 
	Info : 
*/

void bmVectorRotateZXY(TBVector dest, const TBVector src, const float zrads, const float xrads, const float yrads);


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateZYX
	Purpose : rotate vector in z, then y , then x axis
	Parameters : destination, source, z radians, y radians, x radians
	Returns : 
	Info : 
*/

void bmVectorRotateZYX(TBVector dest, const TBVector src, const float zrads, const float yrads, const float xrads);


/*	--------------------------------------------------------------------------------
	Function : bmVectorEqual
	Purpose : test two vectors for being equal
	Parameters : vec1, vec2
	Returns : TRUE if equal, FALSE otherwise
	Info : 
*/

__inline int bmVectorEqual(TBVector vec1, TBVector vec2)
{
	return ((vec1[0] == vec2[0]) && (vec1[1] == vec2[1]) && (vec1[2] == vec2[2]) && (vec1[3] == vec2[3]));
}

#endif // __BMATHS_XBVECTOR_H__
