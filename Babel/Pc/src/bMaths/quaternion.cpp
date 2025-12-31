// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : quaternion.cpp
//   Purpose : quaternion functions
// Component : General Maths
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Globals

TBQuaternion bIdentityQuaternion = {0.0f, 0.0f, 0.0f, 1.0f};

// ********************************************************************************
// Locals



// ********************************************************************************
// Function Implementations

/* --------------------------------------------------------------------------------
   Function : bmQuatToRotation
   Purpose : retrieve the orientation and axis from a quaternion
   Parameters : vector to fill, quaternion
   Returns : 
   Info : 
*/

void bmQuatToRotation(TBVector dest, const TBQuaternion src)
{
    float w = src[3];

    if (w > 1.0f)
        w = 1.0f;
    else if (w < -1.0f)
        w = -1.0f;

    const float half = bmACos(w);
    const float angle = half + half;

    dest[3] = angle;

    const float s = bmSin(angle * 0.5f);

    if (s == 0.0f)
    {
        dest[0] = 0.0f;
        dest[1] = 1.0f;
        dest[2] = 0.0f;
        return;
    }

    dest[0] = src[0] / s;
    dest[1] = src[1] / s;
    dest[2] = src[2] / s;

    const float len = bmSqrt(dest[0] * dest[0] + dest[1] * dest[1] + dest[2] * dest[2]);
    if (len != 0.0f)
    {
        const float inv = 1.0f / len;
        dest[0] *= inv;
        dest[1] *= inv;
        dest[2] *= inv;
    }
}


/* --------------------------------------------------------------------------------
   Function : bmRotationToQuat
   Purpose : set a quaternion from an axis and angle of rotation
   Parameters : quaternion to fill, vector representing rotation
   Returns : 
   Info : 
*/

void bmRotationToQuat(TBQuaternion dest, const TBVector src)
{
    const float half = src[3] * 0.5f;

    const float s = bmSin(half);
    const float c = bmCos(half);

    dest[3] = c;
    dest[0] = src[0] * s;
    dest[1] = src[1] * s;
    dest[2] = src[2] * s;
}


/*	--------------------------------------------------------------------------------
	Function : bmQuatToDirection
	Purpose : convert a quaternion to a vector
	Parameters : destination vector, source quaternion, returned tilt
	Returns : 
	Info : 
*/

void bmQuatToDirection(TBVector dest, const TBQuaternion src, float *tilt)
{
        bkPrintf("*** WARNING *** bmQuatToDirection was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmDirectionToQuat
	Purpose : convert a direction vector (and tilt) to a quaternion.
	Parameters : [out] the output quaternion
				[in] the input direction vector.
				[in] the tilt around the vector.
	Returns : 
	Info : 
*/

void bmDirectionToQuat(TBQuaternion quat, const TBVector direction, const float tilt)
{
    const float dx = direction[0];
    const float dy = direction[1];
    const float dz = direction[2];
    float yaw = bmATan2(dz, dx);

    yaw += HALFPI;
    while (yaw > PI) yaw -= TWOPI;
    yaw = -yaw;

    const float horizLenSq = dx * dx + dz * dz;
    const float horizLen   = bmSqrt(horizLenSq);

    float pitch = bmATan2(dy, horizLen);
    while (pitch > PI) pitch -= TWOPI;

    TBMatrix m;
    bmMatYXZRotation(m, pitch, yaw, tilt);
    bmMatrixToQuat(quat, m);
}

/*	--------------------------------------------------------------------------------
	Function : bmQuatNorm
	Purpose : normalise a quaternion
	Parameters : destination quaternion, source quaternion
	Returns : 
	Info : 
*/

void bmQuatNorm(TBQuaternion dest, const TBQuaternion source)
{
        bkPrintf("*** WARNING *** bmQuatNorm was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmEulerToQuatYXZ
	Purpose : setup a quaternion from input euler angles
	Parameters : ptr to quaternion to set, x radians (pitch), y radians (yaw), z radians (roll)
	Returns : 
	Info : The quaternion equivalence to a set of Euler angles with a YXZ sequence 
*/

void bmEulerToQuatYXZ(TBQuaternion quat, float x, float y, float z)
{
        bkPrintf("*** WARNING *** bmEulerToQuatYXZ was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmEulerToQuatYZX
	Purpose : setup a quaternion from input euler angles
	Parameters : ptr to quaternion to set, x radians (pitch), y radians (yaw), z radians (roll)
	Returns : 
	Info : The quaternion equivalence to a set of Euler angles with a YZX sequence 
*/

void bmEulerToQuatYZX(TBQuaternion quat, float x, float y, float z)
{
        bkPrintf("*** WARNING *** bmEulerToQuatYZX was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmEulerToQuatXYZ
	Purpose : setup a quaternion from input euler angles
	Parameters : ptr to quaternion to set, x radians (pitch), y radians (yaw), z radians (roll)
	Returns : 
	Info : The quaternion equivalence to a set of Euler angles with a XYZ sequence 
*/

void bmEulerToQuatXYZ(TBQuaternion quat, float x, float y, float z)
{
        bkPrintf("*** WARNING *** bmEulerToQuatXYZ was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bmEulerToQuatXZY
	Purpose : setup a quaternion from input euler angles
	Parameters : ptr to quaternion to set, x radians (pitch), y radians (yaw), z radians (roll)
	Returns : 
	Info : The quaternion equivalence to a set of Euler angles with a XZY sequence 
*/

void bmEulerToQuatXZY(TBQuaternion quat, float x, float y, float z)
{
        bkPrintf("*** WARNING *** bmEulerToQuatXZY was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmEulerToQuatZXY
	Purpose : setup a quaternion from input euler angles
	Parameters : ptr to quaternion to set, x radians (pitch), y radians (yaw), z radians (roll)
	Returns : 
	Info : The quaternion equivalence to a set of Euler angles with a ZXY sequence 
*/

void bmEulerToQuatZXY(TBQuaternion quat, float x, float y, float z)
{
    const float halfX = x / 2.0f;
    const float halfY = y / 2.0f;
    const float halfZ = z / 2.0f;

    const float sx = bmSin(halfX);
    const float cx = bmCos(halfX);
    const float sy = bmSin(halfY);
    const float cy = bmCos(halfY);
    const float sz = bmSin(halfZ);
    const float cz = bmCos(halfZ);

    quat[0] =  cy * cz * sx - cx * sy * sz;   /* x */
    quat[1] =  cx * cz * sy + cy * sx * sz;   /* y */
    quat[2] =  cx * cy * sz + cz * sx * sy;   /* z */
    quat[3] =  cx * cy * cz - sx * sy * sz;   /* w */
}


/*	--------------------------------------------------------------------------------
	Function : bmEulerToQuatZYX
	Purpose : setup a quaternion from input euler angles
	Parameters : ptr to quaternion to set, x radians (pitch), y radians (yaw), z radians (roll)
	Returns : 
	Info : The quaternion equivalence to a set of Euler angles with a ZYX sequence 
*/

void bmEulerToQuatZYX(TBQuaternion quat, float x, float y, float z)
{
        bkPrintf("*** WARNING *** bmEulerToQuatZYX was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmQuatToEulerYXZ
	Purpose : setup euler angles from quaternion input 
	Parameters : quaternion, ptr to x radians (pitch), ptr to y radians (yaw), ptr to z radians (roll)
	Returns : 
	Info :
*/

void bmQuatToEulerYXZ(TBQuaternion quat, float *x, float *y, float *z)
{
    TBMatrix m;

    bmQuatToMatrix(m, quat);
    bmMatFactorYXZ(m, x, y, z);
}


/*	--------------------------------------------------------------------------------
	Function : bmQuatToEulerYZX
	Purpose : setup euler angles from quaternion input 
	Parameters : quaternion, ptr to x radians (pitch), ptr to y radians (yaw), ptr to z radians (roll)
	Returns : 
	Info :
*/

void bmQuatToEulerYZX(TBQuaternion quat, float *x, float *y, float *z)
{
        bkPrintf("*** WARNING *** bmQuatToEulerYZX was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmQuatToEulerYZX
	Purpose : setup euler angles from quaternion input 
	Parameters : quaternion, ptr to x radians (pitch), ptr to y radians (yaw), ptr to z radians (roll)
	Returns : 
	Info :
*/

void bmQuatToEulerXYZ(TBQuaternion quat, float *x, float *y, float *z)
{
        bkPrintf("*** WARNING *** bmQuatToEulerXYZ was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmQuatToEulerXZY
	Purpose : setup euler angles from quaternion input 
	Parameters : quaternion, ptr to x radians (pitch), ptr to y radians (yaw), ptr to z radians (roll)
	Returns : 
	Info :
*/

void bmQuatToEulerXZY(TBQuaternion quat, float *x, float *y, float *z)
{
        bkPrintf("*** WARNING *** bmQuatToEulerXZY was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmQuatToEulerZXY
	Purpose : setup euler angles from quaternion input 
	Parameters : quaternion, ptr to x radians (pitch), ptr to y radians (yaw), ptr to z radians (roll)
	Returns : 
	Info :
*/

void bmQuatToEulerZXY(TBQuaternion quat, float *x, float *y, float *z)
{
        bkPrintf("*** WARNING *** bmQuatToEulerZXY was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmQuatToEulerZYX
	Purpose : setup euler angles from quaternion input 
	Parameters : quaternion, ptr to x radians (pitch), ptr to y radians (yaw), ptr to z radians (roll)
	Returns : 
	Info :
*/

void bmQuatToEulerZYX(TBQuaternion quat, float *x, float *y, float *z)
{
        bkPrintf("*** WARNING *** bmQuatToEulerZYX was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bmQuatSlerpAtFixedSpeed
   Purpose : interpolate between two quaternions at a fixed speed
   Parameters : destination, source #1, source #2, speed at which to move towards source #2
   Returns : the angle between the both source quaternions.
   Info : 
*/

float bmQuatSlerpAtFixedSpeed(TBQuaternion dest, const TBQuaternion src1, const TBQuaternion src2, const float speed)
{
    float dot = bmQuatDot(src1, src2);

    if (dot > 1.0f)
        dot = 1.0f;
    else if (dot < -1.0f)
        dot = -1.0f;

    const double a = acos((double)dot);
    const float angle = (float)fabs(a + a);

    if (angle <= speed)
    {
        dest[0] = src2[0];
        dest[1] = src2[1];
        dest[2] = src2[2];
        dest[3] = src2[3];
        return angle;
    }

    bmQuatSlerp(dest, src1, src2, speed / angle);
    return angle;
}