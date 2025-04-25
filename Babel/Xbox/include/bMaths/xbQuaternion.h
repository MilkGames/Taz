// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : xbQuaternion.h
//   Purpose : quaternion functions
// Component : Xbox Maths
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#ifndef __BMATHS_XBQUATERNION_H__
#define __BMATHS_XBQUATERNION_H__


// ********************************************************************************
// Globals


// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : bmQuatToMatrix
	Purpose : convert a quaternion to a matrix
	Parameters : destination matrix, source quaternion
	Returns : 
	Info : 
*/

void bmQuatToMatrix(TBMatrix dest, const TBQuaternion src);


/*	--------------------------------------------------------------------------------
	Function : bmQuatToMatrix33
	Purpose : convert a quaternion to a matrix - only fill in the 3*3 part
	Parameters : destination matrix, source quaternion
	Returns : 
	Info : 
*/

void bmQuatToMatrix33(TBMatrix dest, const TBQuaternion src);


/*	--------------------------------------------------------------------------------
	Function : bmMatrixToQuat
	Purpose : convert a matrix to a quaternion
	Parameters : destination quaternion, source matrix
	Returns : 
	Info : 
*/

void bmMatrixToQuat(TBQuaternion dest, const TBMatrix src);


/* --------------------------------------------------------------------------------
   Function : bmQuatSlerp
   Purpose : interpolate between two quaternions
   Parameters : destination, source #1, source #2, fraction [0.0 -> 1.0]
   Returns : 
   Info : 
*/

void bmQuatSlerp(TBQuaternion dest, const TBQuaternion src1, const TBQuaternion src2, const float t);


/* --------------------------------------------------------------------------------
   Function : bmQuatMultiply
   Purpose : multiply two quaternions together
   Parameters : destination, source #1, source #2
   Returns : 
   Info : 
*/

void bmQuatMultiply(TBQuaternion dest, const TBQuaternion src1, const TBQuaternion src2);


/* --------------------------------------------------------------------------------
   Function : bmQuatDot
   Purpose : calculate the dot product of two quaternions
   Parameters : source#1, source #2
   Returns : 
   Info : 
*/

float bmQuatDot(const TBQuaternion src1, const TBQuaternion src2);


/* --------------------------------------------------------------------------------
   Function : bmQuatCopy
   Purpose : copy one quaternion to another
   Parameters : destination, source
   Returns : 
   Info : 
*/

__inline void bmQuatCopy(TBQuaternion dest, const TBQuaternion src)
{
	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = src[2];
	dest[3] = src[3];
}


/* --------------------------------------------------------------------------------
   Function : bmQuatSlerpLonger
   Purpose : interpolate between two quaternions
   Parameters : destination, source #1, source #2, fraction [0.0 -> 1.0]
   Returns : 
   Info : 
*/

void bmQuatSlerpLonger(TBQuaternion dest, const TBQuaternion src1, const TBQuaternion src2, const float t);


/*	--------------------------------------------------------------------------------
	Function : bmQuatXRotation
	Purpose : setup a quaternion as an X rotation
	Parameters : ptr to quaternion to set, rotation in radians
	Returns : 
	Info : 
*/

void bmQuatXRotation(TBQuaternion quat, const float radians);


/*	--------------------------------------------------------------------------------
	Function : bmQuatYRotation
	Purpose : setup a quaternion as a Y rotation
	Parameters : ptr to quaternion to set, rotation in radians
	Returns : 
	Info : 
*/

void bmQuatYRotation(TBQuaternion quat, const float radians);


/*	--------------------------------------------------------------------------------
	Function : bmQuatZRotation
	Purpose : setup a quaternion as a Z rotation
	Parameters : ptr to quaternion to set, rotation in radians
	Returns : 
	Info : 
*/

void bmQuatZRotation(TBQuaternion quat, const float radians);


/*	--------------------------------------------------------------------------------
	Function : bmQuatInverse
	Purpose : Invert the sense of a quaternion so that it effectively orients in the opposite direction
	Parameters : dest, source
	Returns : 
	Info : 
*/

inline void bmQuatInverse(TBQuaternion dest, const TBQuaternion src)
{
	dest[0] = -src[0];
	dest[1] = -src[1];
	dest[2] = -src[2];
	dest[3] = src[3];
}


#endif // __BMATHS_XBQUATERNION_H__
