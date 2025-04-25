// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : vector.h
//   Purpose : vector functions
// Component : Generic Maths
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#ifndef __BMATHS_VECTOR_H__
#define __BMATHS_VECTOR_H__

// ********************************************************************************
// Constants and Macros


// ********************************************************************************
// Globals

// The identity vector
extern TBVector bIdentityVector;


// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : bmVectorMulAdd
	Purpose : multiply two vectors and add another vector (dest = src1*mul + src2)
	Parameters : destination, source1, vecmul, source2
	Returns : 
	Info : 
*/

BINLINE void bmVectorMulAdd(TBVector dest, const TBVector src, const TBVector vmul, const TBVector vadd)
{
	dest[0] = src[0]*vmul[0] + vadd[0];
	dest[1] = src[1]*vmul[1] + vadd[1];
	dest[2] = src[2]*vmul[2] + vadd[2];
}

/*	--------------------------------------------------------------------------------
	Function : bmVectorCMulAdd
	Purpose : multiply vector by constant value and add another vector (dest = src1*mul + src2)
	Parameters : destination, source1, mul, source2
	Returns : 
	Info : 
*/

BINLINE void bmVectorCMulAdd(TBVector dest, const TBVector src, const float fmul, const TBVector vadd)
{
	dest[0] = src[0]*fmul + vadd[0];
	dest[1] = src[1]*fmul + vadd[1];
	dest[2] = src[2]*fmul + vadd[2];
}

/*	--------------------------------------------------------------------------------
	Function : bmVectorMulCAdd
	Purpose : multiply two vectors and add a constant value (dest = src1*mul1 + value)
	Parameters : destination, source1, vecmul, value
	Returns : 
	Info : 
*/

BINLINE void bmVectorMulCAdd(TBVector dest, const TBVector src, const TBVector vmul, const float fadd)
{
	dest[0] = src[0]*vmul[0] + fadd;
	dest[1] = src[1]*vmul[1] + fadd;
	dest[2] = src[2]*vmul[2] + fadd;
}

/*	--------------------------------------------------------------------------------
	Function : bmVectorCMulCAdd
	Purpose : multiply vector by constant value and add a constant value (dest = src1*mul1 + value)
	Parameters : destination, source1, mul, value
	Returns : 
	Info : 
*/

BINLINE void bmVectorCMulCAdd(TBVector dest, const TBVector src, const float fmul, const float fadd)
{
	dest[0] = src[0]*fmul + fadd;
	dest[1] = src[1]*fmul + fadd;
	dest[2] = src[2]*fmul + fadd;
}

/*	--------------------------------------------------------------------------------
	Function : bmVectorMulAdd4
	Purpose : multiply two vectors and add another vector (dest = src1*mul + src2)
	Parameters : destination, source1, vecmul, source2
	Returns : 
	Info : 
*/

BINLINE void bmVectorMulAdd4(TBVector dest, const TBVector src, const TBVector vmul, const TBVector vadd)
{
	dest[0] = src[0]*vmul[0] + vadd[0];
	dest[1] = src[1]*vmul[1] + vadd[1];
	dest[2] = src[2]*vmul[2] + vadd[2];
	dest[3] = src[3]*vmul[3] + vadd[3];
}

/*	--------------------------------------------------------------------------------
	Function : bmVectorCMulAdd4
	Purpose : multiply vector by constant value and add another vector (dest = src1*mul + src2)
	Parameters : destination, source1, mul, source2
	Returns : 
	Info : 
*/

BINLINE void bmVectorCMulAdd4(TBVector dest, const TBVector src, const float fmul, const TBVector vadd)
{
	dest[0] = src[0]*fmul + vadd[0];
	dest[1] = src[1]*fmul + vadd[1];
	dest[2] = src[2]*fmul + vadd[2];
	dest[3] = src[3]*fmul + vadd[3];
}

/*	--------------------------------------------------------------------------------
	Function : bmVectorMulCAdd4
	Purpose : multiply two vectors and add a constant value (dest = src1*mul1 + value)
	Parameters : destination, source1, vecmul, value
	Returns : 
	Info : 
*/

BINLINE void bmVectorMulCAdd4(TBVector dest, const TBVector src, const TBVector vmul, const float fadd)
{
	dest[0] = src[0]*vmul[0] + fadd;
	dest[1] = src[1]*vmul[1] + fadd;
	dest[2] = src[2]*vmul[2] + fadd;
	dest[3] = src[3]*vmul[3] + fadd;
}

/*	--------------------------------------------------------------------------------
	Function : bmVectorCMulCAdd
	Purpose : multiply vector by constant value and add a constant value (dest = src1*mul1 + value)
	Parameters : destination, source1, mul, value
	Returns : 
	Info : 
*/

BINLINE void bmVectorCMulCAdd4(TBVector dest, const TBVector src, const float fmul, const float fadd)
{
	dest[0] = src[0]*fmul + fadd;
	dest[1] = src[1]*fmul + fadd;
	dest[2] = src[2]*fmul + fadd;
	dest[3] = src[3]*fmul + fadd;
}

/*	--------------------------------------------------------------------------------
	Function : bmVectorMulMulAdd
	Purpose : multiply two pairs of vectors and add another vector (dest = src1*mul1 + src2*mul2 + src3)
	Parameters : destination, source1, vecmul1, source2, vecmul2, source3
	Returns : 
	Info : 
*/

BINLINE void bmVectorMulMulAdd(TBVector dest, const TBVector src, const TBVector vmul, const TBVector src2, const TBVector vmul2, const TBVector vadd)
{
	dest[0] = src[0]*vmul[0] + src2[0]*vmul2[0] + vadd[0];
	dest[1] = src[1]*vmul[1] + src2[1]*vmul2[1] + vadd[1];
	dest[2] = src[2]*vmul[2] + src2[2]*vmul2[2] + vadd[2];
}

/*	--------------------------------------------------------------------------------
	Function : bmVectorCMulCMulAdd
	Purpose : multiply two vectors by two constants and add another vector (dest = src1*mul1 + src2*mul2 + src3)
	Parameters : destination, source1, mul1, source2, mul2, source3
	Returns : 
	Info : 
*/

BINLINE void bmVectorCMulCMulAdd(TBVector dest, const TBVector src, const float fmul, const TBVector src2, const float fmul2, const TBVector vadd)
{
	dest[0] = src[0]*fmul + src2[0]*fmul2 + vadd[0];
	dest[1] = src[1]*fmul + src2[1]*fmul2 + vadd[1];
	dest[2] = src[2]*fmul + src2[2]*fmul2 + vadd[2];
}

/*	--------------------------------------------------------------------------------
	Function : bmVectorMulMulCAdd
	Purpose : multiply two pairs of vectors and add a constant (dest = src1*mul1 + src2*mul2 + value)
	Parameters : destination, source1, vecmul1, source2, vecmul2, value
	Returns : 
	Info : 
*/

BINLINE void bmVectorMulMulCAdd(TBVector dest, const TBVector src, const TBVector vmul, const TBVector src2, const TBVector vmul2, const float fadd)
{
	dest[0] = src[0]*vmul[0] + src2[0]*vmul2[0] + fadd;
	dest[1] = src[1]*vmul[1] + src2[1]*vmul2[1] + fadd;
	dest[2] = src[2]*vmul[2] + src2[2]*vmul2[2] + fadd;
}

/*	--------------------------------------------------------------------------------
	Function : bmVectorCMulCMulCAdd
	Purpose : multiply two vectors by two constants and add a constant (dest = src1*mul1 + src2*mul2 + src3)
	Parameters : destination, source1, mul1, source2, mul2, source3
	Returns : 
	Info : 
*/

BINLINE void bmVectorCMulCMulCAdd(TBVector dest, const TBVector src, const float fmul, const TBVector src2, const float fmul2, const float fadd)
{
	dest[0] = src[0]*fmul + src2[0]*fmul2 + fadd;
	dest[1] = src[1]*fmul + src2[1]*fmul2 + fadd;
	dest[2] = src[2]*fmul + src2[2]*fmul2 + fadd;
}

/*	--------------------------------------------------------------------------------
	Function : bmVectorMulMulAdd4
	Purpose : multiply two pairs of vectors and add another vector (dest = src1*mul1 + src2*mul2 + src3)
	Parameters : destination, source1, vecmul1, source2, vecmul2, source3
	Returns : 
	Info : 
*/

BINLINE void bmVectorMulMulAdd4(TBVector dest, const TBVector src, const TBVector vmul, const TBVector src2, const TBVector vmul2, const TBVector vadd)
{
	dest[0] = src[0]*vmul[0] + src2[0]*vmul2[0] + vadd[0];
	dest[1] = src[1]*vmul[1] + src2[1]*vmul2[1] + vadd[1];
	dest[2] = src[2]*vmul[2] + src2[2]*vmul2[2] + vadd[2];
	dest[3] = src[3]*vmul[3] + src2[3]*vmul2[3] + vadd[3];
}

/*	--------------------------------------------------------------------------------
	Function : bmVectorCMulCMulAdd4
	Purpose : multiply two vectors by two constants and add another vector (dest = src1*mul1 + src2*mul2 + src3)
	Parameters : destination, source1, mul1, source2, mul2, source3
	Returns : 
	Info : 
*/

BINLINE void bmVectorCMulCMulAdd4(TBVector dest, const TBVector src, const float fmul, const TBVector src2, const float fmul2, const TBVector vadd)
{
	dest[0] = src[0]*fmul + src2[0]*fmul2 + vadd[0];
	dest[1] = src[1]*fmul + src2[1]*fmul2 + vadd[1];
	dest[2] = src[2]*fmul + src2[2]*fmul2 + vadd[2];
	dest[3] = src[3]*fmul + src2[3]*fmul2 + vadd[3];
}

/*	--------------------------------------------------------------------------------
	Function : bmVectorMulMulCAdd4
	Purpose : multiply two pairs of vectors and add a constant (dest = src1*mul1 + src2*mul2 + value)
	Parameters : destination, source1, vecmul1, source2, vecmul2, value
	Returns : 
	Info : 
*/

BINLINE void bmVectorMulMulCAdd4(TBVector dest, const TBVector src, const TBVector vmul, const TBVector src2, const TBVector vmul2, const float fadd)
{
	dest[0] = src[0]*vmul[0] + src2[0]*vmul2[0] + fadd;
	dest[1] = src[1]*vmul[1] + src2[1]*vmul2[1] + fadd;
	dest[2] = src[2]*vmul[2] + src2[2]*vmul2[2] + fadd;
	dest[3] = src[3]*vmul[3] + src2[3]*vmul2[3] + fadd;
}

/*	--------------------------------------------------------------------------------
	Function : bmVectorCMulCMulCAdd4
	Purpose : multiply two vectors by two constants and add a constant (dest = src1*mul1 + src2*mul2 + src3)
	Parameters : destination, source1, mul1, source2, mul2, source3
	Returns : 
	Info : 
*/

BINLINE void bmVectorCMulCMulCAdd4(TBVector dest, const TBVector src, const float fmul, const TBVector src2, const float fmul2, const float fadd)
{
	dest[0] = src[0]*fmul + src2[0]*fmul2 + fadd;
	dest[1] = src[1]*fmul + src2[1]*fmul2 + fadd;
	dest[2] = src[2]*fmul + src2[2]*fmul2 + fadd;
	dest[3] = src[3]*fmul + src2[3]*fmul2 + fadd;
}


#endif // __BMATHS_VECTOR_H__
