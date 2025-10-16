// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcVector.cpp
//   Purpose : vector functions
// Component : Xbox Maths
//   Comment : EXPECTED TO BE THE SAME AS xbVector.h, DIRECT REWRITE!
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bmVectorNorm
	Purpose : normalise a vector
	Parameters : destination, source
	Returns : length of source vector
	Info : 
*/

float bmVectorNorm(TBVector dest, const TBVector src)
{
    const float x = src[0];
    const float y = src[1];
    const float z = src[2];

    const float len = bmSqrt(x*x + y*y + z*z);
    if (len != 0.0f) {
        const float inv = 1.0f / len;
        dest[0] = x * inv;
        dest[1] = y * inv;
        dest[2] = z * inv;
    }
    return len;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorNorm4
	Purpose : normalise a vector
	Parameters : destination, source
	Returns : length of source vector
	Info : 
*/

float bmVectorNorm4(TBVector dest, const TBVector src)
{
        bkPrintf("*** WARNING *** bmVectorNorm4 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bmVectorDistance
	Purpose : distance between two vector
	Parameters : source1, source2
	Returns : distance
	Info : 
*/

float bmVectorDistance(const TBVector src1, const TBVector src2)
{
        bkPrintf("*** WARNING *** bmVectorDistance was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorDistanceApprox
	Purpose : approximate distance between two vectors
	Parameters : source1, source2
	Returns : distance
	Info : 
*/

float bmVectorDistanceApprox(const TBVector src1, const TBVector src2)
{
        bkPrintf("*** WARNING *** bmVectorDistanceApprox was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorDistance4
	Purpose : distance between two vector
	Parameters : source1, source2
	Returns : distance
	Info : 
*/

float bmVectorDistance4(const TBVector src1, const TBVector src2)
{
        bkPrintf("*** WARNING *** bmVectorDistance4 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorSquaredDistance
	Purpose : squared distance between two vector
	Parameters : source1, source2
	Returns : squared distance
	Info : 
*/

float bmVectorSquaredDistance(const TBVector src1, const TBVector src2)
{
        bkPrintf("*** WARNING *** bmVectorSquaredDistance was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorSquaredDistance4
	Purpose : squared distance between two vector
	Parameters : source1, source2
	Returns : squared distance
	Info : 
*/

float bmVectorSquaredDistance4(const TBVector src1, const TBVector src2)
{
        bkPrintf("*** WARNING *** bmVectorSquaredDistance4 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bmVectorScaleToLength
	Purpose : scale vector to specified length
	Parameters : destination, source, length
	Returns : 
	Info : 
*/

void bmVectorScaleToLength(TBVector dest, const TBVector src, const float length)
{
        bkPrintf("*** WARNING *** bmVectorScaleToLength was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorScaleToLength4
	Purpose : scale vector to specified length
	Parameters : destination, source, length
	Returns : 
	Info : 
*/

void bmVectorScaleToLength4(TBVector dest, const TBVector src, const float length)
{
        bkPrintf("*** WARNING *** bmVectorScaleToLength4 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorLerp
	Purpose : interpolate between two vectors
	Parameters : destination, source1, source2, t
	Returns : 
	Info : 
*/

void bmVectorLerp(TBVector dest, const TBVector src1, const TBVector src2, const float t)
{
        bkPrintf("*** WARNING *** bmVectorLerp was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorLerp4
	Purpose : interpolate between two vectors
	Parameters : destination, source1, source2, t
	Returns : 
	Info : 
*/

void bmVectorLerp4(TBVector dest, const TBVector src1, const TBVector src2, const float t)
{
        bkPrintf("*** WARNING *** bmVectorLerp4 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorCombine
	Purpose : combine two vectors (dest = source1 * factor1 + source2 * factor2)
	Parameters : destination, source1, factor1, source2, factor2
	Returns : 
	Info : 
*/

void bmVectorCombine(TBVector dest, const TBVector src1, const float factor1, const TBVector src2, const float factor2)
{
        bkPrintf("*** WARNING *** bmVectorCombine was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorCombine4
	Purpose : combine two vectors (dest = source1 * factor1 + source2 * factor2)
	Parameters : destination, source1, factor1, source2, factor2
	Returns : 
	Info : 
*/

void bmVectorCombine4(TBVector dest, const TBVector src1, const float factor1, const TBVector src2, const float factor2)
{
        bkPrintf("*** WARNING *** bmVectorCombine4 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorCalcNormal
	Purpose : Calculate normal of plane given three points on it
	Parameters : destination, source1, source2, source3
	Returns : 
	Info : 
*/

void bmVectorCalcNormal(TBVector dest, const TBVector src1, const TBVector src2, const TBVector src3)
{
        bkPrintf("*** WARNING *** bmVectorCalcNormal was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateX
	Purpose : rotate vector in x axis
	Parameters : destination, source, radians
	Returns : 
	Info : 
*/

void bmVectorRotateX(TBVector dest, const TBVector src, const float rads)
{
        bkPrintf("*** WARNING *** bmVectorRotateX was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateY
	Purpose : rotate vector in y axis
	Parameters : destination, source, radians
	Returns : 
	Info : 
*/

void bmVectorRotateY(TBVector dest, const TBVector src, const float rads)
{
        bkPrintf("*** WARNING *** bmVectorRotateY was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateZ
	Purpose : rotate vector in z axis
	Parameters : destination, source, radians
	Returns : 
	Info : 
*/

void bmVectorRotateZ(TBVector dest, const TBVector src, const float rads)
{
        bkPrintf("*** WARNING *** bmVectorRotateZ was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateXY
	Purpose : rotate vector in x, then y axis
	Parameters : destination, source, x radians, y radians
	Returns : 
	Info : 
*/

void bmVectorRotateXY(TBVector dest, const TBVector src, const float xrads, const float yrads)
{
        bkPrintf("*** WARNING *** bmVectorRotateXY was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateXZ
	Purpose : rotate vector in x, then z axis
	Parameters : destination, source, x radians, z radians
	Returns : 
	Info : 
*/

void bmVectorRotateXZ(TBVector dest, const TBVector src, const float xrads, const float zrads)
{
        bkPrintf("*** WARNING *** bmVectorRotateXZ was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateYX
	Purpose : rotate vector in y, then x axis
	Parameters : destination, source, y radians, x radians
	Returns : 
	Info : 
*/

void bmVectorRotateYX(TBVector dest, const TBVector src, const float yrads, const float xrads)
{
        bkPrintf("*** WARNING *** bmVectorRotateYX was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateYZ
	Purpose : rotate vector in y, then z axis
	Parameters : destination, source, y radians, z radians
	Returns : 
	Info : 
*/

void bmVectorRotateYZ(TBVector dest, const TBVector src, const float yrads, const float zrads)
{
        bkPrintf("*** WARNING *** bmVectorRotateYZ was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateZX
	Purpose : rotate vector in z, then x axis
	Parameters : destination, source, z radians, x radians
	Returns : 
	Info : 
*/

void bmVectorRotateZX(TBVector dest, const TBVector src, const float zrads, const float xrads)
{
        bkPrintf("*** WARNING *** bmVectorRotateZX was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateZY
	Purpose : rotate vector in z, then y axis
	Parameters : destination, source, z radians, y radians
	Returns : 
	Info : 
*/

void bmVectorRotateZY(TBVector dest, const TBVector src, const float zrads, const float yrads)
{
        bkPrintf("*** WARNING *** bmVectorRotateZY was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateXYZ
	Purpose : rotate vector in x, then y , then z axis
	Parameters : destination, source, x radians, y radians, z radians
	Returns : 
	Info : 
*/

void bmVectorRotateXYZ(TBVector dest, const TBVector src, const float xrads, const float yrads, const float zrads)
{
        bkPrintf("*** WARNING *** bmVectorRotateXYZ was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateXZY
	Purpose : rotate vector in x, then z , then y axis
	Parameters : destination, source, x radians, z radians, y radians
	Returns : 
	Info : 
*/

void bmVectorRotateXZY(TBVector dest, const TBVector src, const float xrads, const float zrads, const float yrads)
{
        bkPrintf("*** WARNING *** bmVectorRotateXZY was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateYXZ
	Purpose : rotate vector in y, then x , then z axis
	Parameters : destination, source, y radians, x radians, z radians
	Returns : 
	Info : 
*/

void bmVectorRotateYXZ(TBVector dest, const TBVector src, const float yrads, const float xrads, const float zrads)
{
        bkPrintf("*** WARNING *** bmVectorRotateYXZ was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateYZX
	Purpose : rotate vector in y, then z , then x axis
	Parameters : destination, source, y radians, z radians, x radians
	Returns : 
	Info : 
*/

void bmVectorRotateYZX(TBVector dest, const TBVector src, const float yrads, const float zrads, const float xrads)
{
        bkPrintf("*** WARNING *** bmVectorRotateYZX was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateZXY
	Purpose : rotate vector in z, then x , then y axis
	Parameters : destination, source, z radians, x radians, y radians
	Returns : 
	Info : 
*/

void bmVectorRotateZXY(TBVector dest, const TBVector src, const float zrads, const float xrads, const float yrads)
{
        bkPrintf("*** WARNING *** bmVectorRotateZXY was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmVectorRotateZYX
	Purpose : rotate vector in z, then y , then x axis
	Parameters : destination, source, z radians, y radians, x radians
	Returns : 
	Info : 
*/

void bmVectorRotateZYX(TBVector dest, const TBVector src, const float zrads, const float yrads, const float xrads)
{
        bkPrintf("*** WARNING *** bmVectorRotateZYX was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}