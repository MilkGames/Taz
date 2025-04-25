// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : quaternion.h
//   Purpose : quaternion functions
// Component : General Maths
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#ifndef __BMATHS_QUATERNION_H__
#define __BMATHS_QUATERNION_H__


// ********************************************************************************
// Globals

// the identity quaternion
extern TBQuaternion bIdentityQuaternion;


// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bmQuatToRotation
   Purpose : retrieve the orientation and axis from a quaternion
   Parameters : vector to fill, quaternion
   Returns : 
   Info : 
*/

void bmQuatToRotation(TBVector dest, const TBQuaternion src);


/* --------------------------------------------------------------------------------
   Function : bmRotationToQuat
   Purpose : set a quaternion from an axis and angle of rotation
   Parameters : quaternion to fill, vector representing rotation
   Returns : 
   Info : 
*/

void bmRotationToQuat(TBQuaternion dest, const TBVector src);


/*	--------------------------------------------------------------------------------
	Function : bmQuatToDirection
	Purpose : convert a quaternion to a vector
	Parameters : destination vector, source quaternion, returned tilt
	Returns : 
	Info : 
*/

void bmQuatToDirection(TBVector dest, const TBQuaternion src, float *tilt = 0);


/*	--------------------------------------------------------------------------------
	Function : bmDirectionToQuat
	Purpose : convert a direction vector (and tilt) to a quaternion.
	Parameters : [out] the output quaternion
				[in] the input direction vector.
				[in] the tilt around the vector.
	Returns : 
	Info : 
*/

void bmDirectionToQuat(TBQuaternion quat, const TBVector direction, const float tilt);


/*	--------------------------------------------------------------------------------
	Function : bmQuatNorm
	Purpose : normalise a quaternion
	Parameters : destination quaternion, source quaternion
	Returns : 
	Info : 
*/

void bmQuatNorm(TBQuaternion dest, const TBQuaternion source);


/*	--------------------------------------------------------------------------------
	Function : bmEulerToQuatYXZ
	Purpose : setup a quaternion from input euler angles
	Parameters : ptr to quaternion to set, x radians (pitch), y radians (yaw), z radians (roll)
	Returns : 
	Info : The quaternion equivalence to a set of Euler angles with a YXZ sequence 
*/

void bmEulerToQuatYXZ(TBQuaternion quat, float x, float y, float z);


/*	--------------------------------------------------------------------------------
	Function : bmEulerToQuatYZX
	Purpose : setup a quaternion from input euler angles
	Parameters : ptr to quaternion to set, x radians (pitch), y radians (yaw), z radians (roll)
	Returns : 
	Info : The quaternion equivalence to a set of Euler angles with a YZX sequence 
*/

void bmEulerToQuatYZX(TBQuaternion quat, float x, float y, float z);


/*	--------------------------------------------------------------------------------
	Function : bmEulerToQuatXYZ
	Purpose : setup a quaternion from input euler angles
	Parameters : ptr to quaternion to set, x radians (pitch), y radians (yaw), z radians (roll)
	Returns : 
	Info : The quaternion equivalence to a set of Euler angles with a XYZ sequence 
*/

void bmEulerToQuatXYZ(TBQuaternion quat, float x, float y, float z);


/*	--------------------------------------------------------------------------------
	Function : bmEulerToQuatXZY
	Purpose : setup a quaternion from input euler angles
	Parameters : ptr to quaternion to set, x radians (pitch), y radians (yaw), z radians (roll)
	Returns : 
	Info : The quaternion equivalence to a set of Euler angles with a XZY sequence 
*/

void bmEulerToQuatXZY(TBQuaternion quat, float x, float y, float z);


/*	--------------------------------------------------------------------------------
	Function : bmEulerToQuatZXY
	Purpose : setup a quaternion from input euler angles
	Parameters : ptr to quaternion to set, x radians (pitch), y radians (yaw), z radians (roll)
	Returns : 
	Info : The quaternion equivalence to a set of Euler angles with a ZXY sequence 
*/

void bmEulerToQuatZXY(TBQuaternion quat, float x, float y, float z);


/*	--------------------------------------------------------------------------------
	Function : bmEulerToQuatZYX
	Purpose : setup a quaternion from input euler angles
	Parameters : ptr to quaternion to set, x radians (pitch), y radians (yaw), z radians (roll)
	Returns : 
	Info : The quaternion equivalence to a set of Euler angles with a ZYX sequence 
*/

void bmEulerToQuatZYX(TBQuaternion quat, float x, float y, float z);


/*	--------------------------------------------------------------------------------
	Function : bmQuatToEulerYXZ
	Purpose : setup euler angles from quaternion input 
	Parameters : quaternion, ptr to x radians (pitch), ptr to y radians (yaw), ptr to z radians (roll)
	Returns : 
	Info :
*/

void bmQuatToEulerYXZ(TBQuaternion quat, float *x, float *y, float *z);


/*	--------------------------------------------------------------------------------
	Function : bmQuatToEulerYZX
	Purpose : setup euler angles from quaternion input 
	Parameters : quaternion, ptr to x radians (pitch), ptr to y radians (yaw), ptr to z radians (roll)
	Returns : 
	Info :
*/

void bmQuatToEulerYZX(TBQuaternion quat, float *x, float *y, float *z);


/*	--------------------------------------------------------------------------------
	Function : bmQuatToEulerYZX
	Purpose : setup euler angles from quaternion input 
	Parameters : quaternion, ptr to x radians (pitch), ptr to y radians (yaw), ptr to z radians (roll)
	Returns : 
	Info :
*/

void bmQuatToEulerXYZ(TBQuaternion quat, float *x, float *y, float *z);


/*	--------------------------------------------------------------------------------
	Function : bmQuatToEulerXZY
	Purpose : setup euler angles from quaternion input 
	Parameters : quaternion, ptr to x radians (pitch), ptr to y radians (yaw), ptr to z radians (roll)
	Returns : 
	Info :
*/

void bmQuatToEulerXZY(TBQuaternion quat, float *x, float *y, float *z);


/*	--------------------------------------------------------------------------------
	Function : bmQuatToEulerZXY
	Purpose : setup euler angles from quaternion input 
	Parameters : quaternion, ptr to x radians (pitch), ptr to y radians (yaw), ptr to z radians (roll)
	Returns : 
	Info :
*/

void bmQuatToEulerZXY(TBQuaternion quat, float *x, float *y, float *z);


/*	--------------------------------------------------------------------------------
	Function : bmQuatToEulerZYX
	Purpose : setup euler angles from quaternion input 
	Parameters : quaternion, ptr to x radians (pitch), ptr to y radians (yaw), ptr to z radians (roll)
	Returns : 
	Info :
*/

void bmQuatToEulerZYX(TBQuaternion quat, float *x, float *y, float *z);


/* --------------------------------------------------------------------------------
   Function : bmQuatSlerpAtFixedSpeed
   Purpose : interpolate between two quaternions at a fixed speed
   Parameters : destination, source #1, source #2, speed at which to move towards source #2
   Returns : the angle between the both source quaternions.
   Info : 
*/

float bmQuatSlerpAtFixedSpeed(TBQuaternion dest, const TBQuaternion src1, const TBQuaternion src2, const float speed);


#endif // __BMATHS_QUATERNION_H__