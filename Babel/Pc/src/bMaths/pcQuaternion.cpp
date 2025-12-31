// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcQuaternion.cpp
//   Purpose : quaternion functions
// Component : Xbox Maths
//   Comment : EXPECTED TO BE THE SAME AS xbQuaternion.h, DIRECT REWRITE!
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bmQuatToMatrix
	Purpose : convert a quaternion to a matrix
	Parameters : destination matrix, source quaternion
	Returns : 
	Info : 
*/

void bmQuatToMatrix(TBMatrix dest, const TBQuaternion src)
{
    const float x = src[0];
    const float y = src[1];
    const float z = src[2];
    const float w = src[3];

    /* s = 2.0f / (x*x + y*y + z*z + w*w); */
    const float normSq = x * x + y * y + z * z + w * w;
    const float s      = 2.0f / normSq;

    const float sx = s * x;
    const float sy = s * y;
    const float sz = s * z;

    const float twx = sx * w;          /* s * x * w */
    const float twy = sy * w;          /* s * y * w */
    const float twz = sz * w;          /* s * z * w */

    const float sxx = sx * x;          /* s * x * x */
    const float sxy = sx * y;          /* s * x * y */
    const float sxz = sx * z;          /* s * x * z */

    const float syy = sy * y;          /* s * y * y */
    const float syz = sy * z;          /* s * y * z */

    const float szz = sz * z;          /* s * z * z */

    /* First row */
    dest[0][0] = 1.0f - (syy + szz);           /* 1 - (sy2 + sz2) */
    dest[0][1] = sxy - twz;                   /* s*x*y - s*z*w */
    dest[0][2] = sxz + twy;                   /* s*x*z + s*y*w */
    dest[0][3] = 0.0f;

    /* Second row */
    dest[1][0] = sxy + twz;                   /* s*x*y + s*z*w */
    dest[1][1] = 1.0f - (sxx + szz);          /* 1 - (sx2 + sz2) */
    dest[1][2] = syz - twx;                   /* s*y*z - s*x*w */
    dest[1][3] = 0.0f;

    /* Third row */
    dest[2][0] = sxz - twy;                   /* s*x*z - s*y*w */
    dest[2][1] = syz + twx;                   /* s*y*z + s*x*w */
    dest[2][2] = 1.0f - (sxx + syy);          /* 1 - (sx2 + sy2) */
    dest[2][3] = 0.0f;

    /* Fourth row (homogeneous) */
    dest[3][0] = 0.0f;
    dest[3][1] = 0.0f;
    dest[3][2] = 0.0f;
    dest[3][3] = 1.0f;
}


/*	--------------------------------------------------------------------------------
	Function : bmQuatToMatrix33
	Purpose : convert a quaternion to a matrix - only fill in the 3*3 part
	Parameters : destination matrix, source quaternion
	Returns : 
	Info : 
*/

void bmQuatToMatrix33(TBMatrix dest, const TBQuaternion src)
{
        bkPrintf("*** WARNING *** bmQuatToMatrix33 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmMatrixToQuat
	Purpose : convert a matrix to a quaternion
	Parameters : destination quaternion, source matrix
	Returns : 
	Info : 
*/

void bmMatrixToQuat(TBQuaternion dest, const TBMatrix src)
{
    float m00 = src[0][0];
    float m01 = src[0][1];
    float m02 = src[0][2];

    float m10 = src[1][0];
    float m11 = src[1][1];
    float m12 = src[1][2];

    float m20 = src[2][0];
    float m21 = src[2][1];
    float m22 = src[2][2];

    float trace = m00 + m11 + m22;

    if (trace > 0.0f)
    {
        /* s' = sqrt(1 + trace); w = 0.5 * s'; inv = 0.5 / s' */
        float sp = bmSqrt(trace + 1.0f);
        float w  = 0.5f * sp;
        float inv = 0.5f / sp;

        dest[0] = (m21 - m12) * inv;  /* x */
        dest[1] = (m02 - m20) * inv;  /* y */
        dest[2] = (m10 - m01) * inv;  /* z */
        dest[3] = w;                  /* w */
    }
    else
    {
        if (m11 > m00 && m11 >= m22)
        {
            /* s' = sqrt(1 + m11 - m00 - m22) */
            float sp = bmSqrt(1.0f + m11 - m00 - m22);
            float y  = 0.5f * sp;
            float inv = 0.5f / sp;

            dest[0] = (m10 + m01) * inv;  /* x = (m10 + m01) / (2*s') */
            dest[1] = y;                  /* y */
            dest[2] = (m21 + m12) * inv;  /* z = (m21 + m12) / (2*s') */
            dest[3] = (m02 - m20) * inv;  /* w = (m02 - m20) / (2*s') */
        }
        else if (m22 > m00 && m22 > m11)
        {
            /* s' = sqrt(1 + m22 - m00 - m11) */
            float sp = bmSqrt(1.0f + m22 - m00 - m11);
            float z  = 0.5f * sp;
            float inv = 0.5f / sp;

            dest[0] = (m02 + m20) * inv;  /* x = (m02 + m20) / (2*s') */
            dest[1] = (m21 + m12) * inv;  /* y = (m21 + m12) / (2*s') */
            dest[2] = z;                  /* z */
            dest[3] = (m10 - m01) * inv;  /* w = (m10 - m01) / (2*s') */
        }
        else
        {
            float sp = bmSqrt(1.0f + m00 - m11 - m22);
            float x  = 0.5f * sp;
            float inv = 0.5f / sp;

            dest[0] = x;                  /* x */
            dest[1] = (m10 + m01) * inv;  /* y = (m10 + m01) / (2*s') */
            dest[2] = (m20 + m02) * inv;  /* z = (m20 + m02) / (2*s') */
            dest[3] = (m21 - m12) * inv;  /* w = (m21 - m12) / (2*s') */
        }
    }
}


/* --------------------------------------------------------------------------------
   Function : bmQuatSlerp
   Purpose : interpolate between two quaternions
   Parameters : destination, source #1, source #2, fraction [0.0 -> 1.0]
   Returns : 
   Info : 
*/

void bmQuatSlerp(TBQuaternion dest, const TBQuaternion src1, const TBQuaternion src2, const float t)
{
    float sx = src2[0];
    float sy = src2[1];
    float sz = src2[2];
    float sw = src2[3];

    /* choose shortest path: compare |src1 - src2|^2 and |src1 + src2|^2 */

    const float dx = src1[0] - sx;
    const float dy = src1[1] - sy;
    const float dz = src1[2] - sz;
    const float dw = src1[3] - sw;

    const float diffLen2 = dx * dx + dy * dy + dz * dz + dw * dw;

    const float sxp = sx + src1[0];
    const float syp = sy + src1[1];
    const float szp = sz + src1[2];
    const float swp = sw + src1[3];

    const float sumLen2 = sxp * sxp + syp * syp + szp * szp + swp * swp;

    if (diffLen2 >= sumLen2)
    {
        sx = -sx;
        sy = -sy;
        sz = -sz;
        sw = -sw;
    }

    /* dot product between src1 and (possibly flipped) src2 */
    const float dot = sx * src1[0] +
                      sy * src1[1] +
                      sz * src1[2] +
                      sw * src1[3];

    /* handle three cases: near -1, near +1, and general slerp */
    if (dot + 1.0f > 1.0e-5f)
    {
        float w0, w1;

        /* very small angle – use lerp to avoid division by tiny sin */
        if (1.0f - dot <= 1.0e-5f)
        {
            w0 = 1.0f - t;
            w1 = t;
        }
        else
        {
            /* main slerp branch */
            const float angle    = bmACos(dot);
            const float sinAngle = bmSin(angle);

            const float s0 = bmSin((1.0f - t) * angle);
            w0 = s0 / sinAngle;

            const float s1 = bmSin(t * angle);
            w1 = s1 / sinAngle;
        }

        dest[0] = w0 * src1[0] + w1 * sx;
        dest[1] = w0 * src1[1] + w1 * sy;
        dest[2] = w0 * src1[2] + w1 * sz;
        dest[3] = w0 * src1[3] + w1 * sw;
    }
    else
    {
        /* near 180-degree rotation – special handling */
        TBQuaternion q;

        q[0] = -src1[1];
        q[1] =  src1[0];
        q[2] = -src1[3];
        q[3] =  src1[2];

        const float s0 = bmSin((1.0f - t) * HALFPI);
        const float s1 = bmSin(t * HALFPI);

        dest[0] = s0 * src1[0] + s1 * q[0];
        dest[1] = s1 * q[1]   + s0 * src1[1];
        dest[2] = s1 * q[2]   + s0 * src1[2];
        dest[3] = q[3];
    }
}


/* --------------------------------------------------------------------------------
   Function : bmQuatMultiply
   Purpose : multiply two quaternions together
   Parameters : destination, source #1, source #2
   Returns : 
   Info : 
*/

void bmQuatMultiply(TBQuaternion dest, const TBQuaternion src1, const TBQuaternion src2)
{
    const float x1 = src1[0];
    const float y1 = src1[1];
    const float z1 = src1[2];
    const float w1 = src1[3];

    const float x2 = src2[0];
    const float y2 = src2[1];
    const float z2 = src2[2];
    const float w2 = src2[3];

    dest[0] =  w1 * x2 + x1 * w2 + y1 * z2 - z1 * y2;
    dest[1] =  w1 * y2 - x1 * z2 + y1 * w2 + z1 * x2;
    dest[2] =  w1 * z2 + x1 * y2 - y1 * x2 + z1 * w2;
    dest[3] =  w1 * w2 - (x1 * x2 + y1 * y2 + z1 * z2);
}

/* --------------------------------------------------------------------------------
   Function : bmQuatDot
   Purpose : calculate the dot product of two quaternions
   Parameters : source#1, source #2
   Returns : 
   Info : 
*/

float bmQuatDot(const TBQuaternion src1, const TBQuaternion src2)
{
    const float dx = src1[0] - src2[0];
    const float dy = src1[1] - src2[1];
    const float dz = src1[2] - src2[2];
    const float dw = src1[3] - src2[3];

    const float diffSq = dx*dx + dy*dy + dz*dz + dw*dw;

    const float sx = src1[0] + src2[0];
    const float sy = src1[1] + src2[1];
    const float sz = src1[2] + src2[2];
    const float sw = src1[3] + src2[3];

    const float sumSq = sx*sx + sy*sy + sz*sz + sw*sw;

    float dot = src1[0]*src2[0] + src1[1]*src2[1] + src1[2]*src2[2] + src1[3]*src2[3];

    if (sumSq <= diffSq) {
        dot = -dot;
    }

    return dot;
}

/* --------------------------------------------------------------------------------
   Function : bmQuatSlerpLonger
   Purpose : interpolate between two quaternions
   Parameters : destination, source #1, source #2, fraction [0.0 -> 1.0]
   Returns : 
   Info : 
*/

void bmQuatSlerpLonger(TBQuaternion dest, const TBQuaternion src1, const TBQuaternion src2, const float t)
{
        bkPrintf("*** WARNING *** bmQuatSlerpLonger was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmQuatXRotation
	Purpose : setup a quaternion as an X rotation
	Parameters : ptr to quaternion to set, rotation in radians
	Returns : 
	Info : 
*/

void bmQuatXRotation(TBQuaternion quat, const float radians)
{
	const float halfRadians = radians * 0.5f;

	quat[0] = bmSin(halfRadians);
	quat[1] = 0.0f;
	quat[2] = 0.0f;
	quat[3] = bmCos(halfRadians);
}


/*	--------------------------------------------------------------------------------
	Function : bmQuatYRotation
	Purpose : setup a quaternion as a Y rotation
	Parameters : ptr to quaternion to set, rotation in radians
	Returns : 
	Info : 
*/

void bmQuatYRotation(TBQuaternion quat, const float radians)
{
	const float halfRadians = radians * 0.5f;

	quat[0] = 0.0f;
	quat[1] = bmSin(halfRadians);
	quat[2] = 0.0f;
	quat[3] = bmCos(halfRadians);
}


/*	--------------------------------------------------------------------------------
	Function : bmQuatZRotation
	Purpose : setup a quaternion as a Z rotation
	Parameters : ptr to quaternion to set, rotation in radians
	Returns : 
	Info : 
*/

void bmQuatZRotation(TBQuaternion quat, const float radians)
{
	const float halfRadians = radians * 0.5f;

	quat[0] = 0.0f;
	quat[1] = 0.0f;
	quat[2] = bmSin(halfRadians);
	quat[3] = bmCos(halfRadians);
}

