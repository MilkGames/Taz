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
    const float len = bmSqrt(src[0]*src[0] + src[1]*src[1] + src[2]*src[2] + src[3]*src[3]);
    if (len != 0.0f) {
        const float inv = 1.0f / len;
        dest[0] = src[0] * inv;
        dest[1] = src[1] * inv;
        dest[2] = src[2] * inv;
        dest[3] = src[3] * inv;
    }
    return len;
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
    const float dx = src1[0] - src2[0];
    const float dy = src1[1] - src2[1];
    const float dz = src1[2] - src2[2];

    return bmSqrt(dx * dx + dy * dy + dz * dz);
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
    const float dx = src1[0] - src2[0];
    const float dy = src1[1] - src2[1];
    const float dz = src1[2] - src2[2];

    return bmSqrtApprox(dx * dx + dy * dy + dz * dz);
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
    const float dx = src1[0] - src2[0];
    const float dy = src1[1] - src2[1];
    const float dz = src1[2] - src2[2];

    const float dz2 = dz * dz;
    const float dx2 = dx * dx;
    const float dy2 = dy * dy;

    return (dz2 + dx2) + dy2;
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
    const float x = src[0];
    const float y = src[1];
    const float z = src[2];

    const float len = bmSqrt(x * x + y * y + z * z);

    if (len == 0.0f) {
        dest[0] = 0.0f;
        dest[1] = 0.0f;
        dest[2] = 0.0f;
        dest[3] = 1.0f;
        return;
    }

    const float s = length / len;
    dest[0] = x * s;
    dest[1] = y * s;
    dest[2] = z * s;
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
    const float s = 1.0f - t;
    dest[0] = t * src2[0] + s * src1[0];
    dest[1] = t * src2[1] + s * src1[1];
    dest[2] = t * src2[2] + s * src1[2];
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
	const float edge1x = src2[0] - src1[0];
	const float edge1y = src2[1] - src1[1];
	const float edge1z = src2[2] - src1[2];

	const float edge2x = src3[0] - src2[0];
	const float edge2y = src3[1] - src2[1];
	const float edge2z = src3[2] - src2[2];

	const float normalX = edge1y * edge2z - edge1z * edge2y;
	const float normalY = edge1z * edge2x - edge1x * edge2z;
	const float normalZ = edge1x * edge2y - edge1y * edge2x;

	dest[0] = normalX;
	dest[1] = normalY;
	dest[2] = normalZ;

	const float length = bmSqrt(normalX * normalX + normalY * normalY + normalZ * normalZ);
	if (length == 0.0f)
	{
		dest[0] = 0.0f;
		dest[1] = 0.0f;
		dest[2] = 0.0f;
		return;
	}

	const float invLength = 1.0f / length;
	dest[0] = invLength * dest[0];
	dest[1] = invLength * dest[1];
	dest[2] = invLength * dest[2];
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
    const float x = src[0];
    const float y = src[1];
    const float z = src[2];

    const float c = (float)cos((double)rads);
    const float s = (float)sin((double)rads);

    dest[1] = y;
    dest[0] = c * x + s * z;
    dest[2] = c * z - s * x;
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
    float x = src[0];
    float y = src[1];
    float z = src[2];

    float c = bmCos(rads);
    float s = bmSin(rads);

    dest[0] = c * x + s * y;
    dest[1] = c * y - s * x;
	dest[2] = z;
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
    float sx = src[0];
    float sy = src[1];
    float sz = src[2];

    float cosX = (float)cos(xrads);
    float sinX = (float)sin(xrads);

    float tmpY = cosX * sy + sinX * sz;
    float tmpZ = cosX * sz - sinX * sy;

    float cosY = (float)cos(yrads);
    float sinY = (float)sin(yrads);

    float tmpX = cosY * sx + sinY * tmpZ;

    dest[0] = tmpX;
    dest[1] = tmpY;
    dest[2] = cosY * tmpZ - sinY * sx;
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