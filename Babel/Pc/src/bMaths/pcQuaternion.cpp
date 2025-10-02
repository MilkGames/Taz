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
    return;
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
    return;
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
    return;
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
    return;
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
    return 0;
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
    return;
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
    return;
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
    return;
}

