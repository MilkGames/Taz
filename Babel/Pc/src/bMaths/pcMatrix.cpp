// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcMatrix.h
//   Purpose : matrix functions
// Component : Xbox Maths
//   Comment : EXPECTED TO BE THE SAME AS xbMatrix.h, DIRECT REWRITE!
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Globals

float bIdentityMatrix[4][4] = { 
    {1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1} 
};

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bmMatXRotation
	Purpose : setup a matrix as an X rotation
	Parameters : ptr to matrix to set, rotation in radians
	Returns : 
	Info : 
*/

void bmMatXRotation(TBMatrix mat, const float radians)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : bmMatYRotation
	Purpose : setup a matrix as a Y rotation
	Parameters : ptr to matrix to set, rotation in radians
	Returns : 
	Info : 
*/

void bmMatYRotation(TBMatrix mat, const float radians)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : bmMatZRotation
	Purpose : setup a matrix as a Z rotation
	Parameters : ptr to matrix to set, rotation in radians
	Returns : 
	Info : 
*/

void bmMatZRotation(TBMatrix mat, const float radians)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : bmMatTranslate
	Purpose : setup a matrix as a translation
	Parameters : ptr to matrix to set, x,y,z translations
	Returns : 
	Info : 
*/

void bmMatTranslate(TBMatrix mat, const float x, const float y, const float z)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : bmMatScale
	Purpose : setup a matrix as a scaler
	Parameters : ptr to matrix to set, x,y,z scales
	Returns : 
	Info : 
*/

void bmMatScale(TBMatrix mat, const float x, const float y, const float z)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : bmMatMultiplyAligned
	Purpose : matrix multiplication for 16byte aligned matrices
	Parameters : ptr to destination matrix, ptr to source #1, ptr to source #2
	Returns : 
	Info : multiplication is performed so source#2 takes effect first
*/

void bmMatMultiplyAligned(TBMatrix dest, const TBMatrix src1, const TBMatrix src2)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : bmMatMultiplyUnaligned
	Purpose : matrix multiplication for unaligned matrices
	Parameters : ptr to destination matrix, ptr to source #1, ptr to source #2
	Returns : 
	Info : multiplication is performed so source#2 takes effect first
*/

void bmMatMultiplyUnaligned(TBMatrix dest, const TBMatrix src1, const TBMatrix src2)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : bmMatMultiply
	Purpose : matrix multiplication.
	Parameters : ptr to destination matrix, ptr to source #1, ptr to source #2
	Returns : 
	Info : multiplication is performed so source#2 takes effect first
*/

// defined


/*	--------------------------------------------------------------------------------
	Function : bmMatShow
	Purpose : build a visual representation of a matrix ready for printing
	Parameters : matrix to show, buffer to fill
	Returns : ptr to buffer
	Info : 
*/

char *bmMatShow(const TBMatrix mat, char * const buf)
{
	return NULL;
}


/*	--------------------------------------------------------------------------------
	Function : bmMatXYRotation
	Purpose : setup a matrix as a general rotation
	Parameters : ptr to matrix to set, x,y rotations in radians
	Returns : 
	Info : Matrices are concatenated in XY form (so Y rotation acts first)
*/

void bmMatXYRotation(TBMatrix dest, const float x, const float y)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : bmMatYXRotation
	Purpose : setup a matrix as a general rotation
	Parameters : ptr to matrix to set, x,y rotations in radians
	Returns : 
	Info : Matrices are concatenated in YX form (so X rotation acts first)
*/

void bmMatYXRotation(TBMatrix dest, const float x, const float y)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : bmMatXZRotation
	Purpose : setup a matrix as a general rotation
	Parameters : ptr to matrix to set, x,z rotations in radians
	Returns : 
	Info : Matrices are concatenated in XZ form (so Z rotation acts first)
*/

void bmMatXZRotation(TBMatrix dest, const float x, const float z)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : bmMatZXRotation
	Purpose : setup a matrix as a general rotation
	Parameters : ptr to matrix to set, x,z rotations in radians
	Returns : 
	Info : Matrices are concatenated in ZX form (so X rotation acts first)
*/

void bmMatZXRotation(TBMatrix dest, const float x, const float z)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : bmMatYZRotation
	Purpose : setup a matrix as a general rotation
	Parameters : ptr to matrix to set, y,z rotations in radians
	Returns : 
	Info : Matrices are concatenated in YZ form (so Z rotation acts first)
*/

void bmMatYZRotation(TBMatrix dest, const float y, const float z)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : bmMatZYRotation
	Purpose : setup a matrix as a general rotation
	Parameters : ptr to matrix to set, y,z rotations in radians
	Returns : 
	Info : Matrices are concatenated in ZY form (so Y rotation acts first)
*/

void bmMatZYRotation(TBMatrix dest, const float y, const float z)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : bmMatXYZRotation
	Purpose : setup a matrix as a general rotation
	Parameters : ptr to matrix to set, x,y,z rotations in radians
	Returns : 
	Info : Matrices are concatenated in XYZ form (so Z rotation acts first)
*/

void bmMatXYZRotation(TBMatrix dest, const float xRadians, const float yRadians, const float zRadians)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : bmMatXZYRotation
	Purpose : setup a matrix as a general rotation
	Parameters : ptr to matrix to set, x,y,z rotations in radians
	Returns : 
	Info : Matrices are concatenated in XZY form (so Y rotation happens first)
*/

void bmMatXZYRotation(TBMatrix dest, const float x, const float y, const float z)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : bmMatYXZRotation
	Purpose : setup a matrix as a general rotation
	Parameters : ptr to matrix to set, x,y,z rotations in radians
	Returns : 
	Info : Matrices are concatenated in YXZ form (so Z rotation happens first)
*/

void bmMatYXZRotation(TBMatrix dest, const float x, const float y, const float z)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : bmMatYZXRotation
	Purpose : setup a matrix as a general rotation
	Parameters : ptr to matrix to fill, x,y,z rotations in radians
	Returns : 
	Info : Matrices are concatenated in YZX form (so X rotation happens first)
*/

void bmMatYZXRotation(TBMatrix dest, const float x, const float y, const float z)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : bmMatZXYRotation
	Purpose : setup a matrix as a general rotation
	Parameters : ptr to matrix to set, x,y,z rotations in radians
	Returns : 
	Info : Matrices are concatenated in ZXY form (so Y rotation happens first)
*/

void bmMatZXYRotation(TBMatrix dest, const float x, const float y, const float z)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : bmMatZYXRotation
	Purpose : setup a matrix as a general rotation
	Parameters : ptr to matrix to set, x,y,z rotations in radians
	Returns : 
	Info : Matrices are concatenated in ZYX form (so X rotation happens first)
*/

void bmMatZYXRotation(TBMatrix dest, const float x, const float y, const float z)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : bmMatInverse
	Purpose : setup a matrix as the inverse of another
	Parameters : ptr to dest matrix, ptr to matrix to invert
	Returns : 1 for success, 0 if singular
	Info : 
*/

int bmMatInverse(TBMatrix dest, const TBMatrix in)
{
	return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bmMatMultiplyVector
	Purpose : Multiply a vector by a matrix
	Parameters : matrix ptr, vector to multiply
	Returns : 
	Info : 
*/

void bmMatMultiplyVector(const TBMatrix mat, TBVector vec)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : bmMatMultiplyVector2
	Purpose : Multiply a vector by a matrix
	Parameters : dest, matrix ptr, vector to multiply
	Returns : 
	Info : 
*/

void bmMatMultiplyVector2(TBVector dest, const TBMatrix mat, const TBVector vec)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : bmMatTranspose
	Purpose : make a matrix the transpose of another
	Parameters : dest matrix, source matrix
	Returns : 
	Info : 
*/

void bmMatTranspose(TBMatrix dest, const TBMatrix src)
{
	return;
}

/*	--------------------------------------------------------------------------------
	Function : bmMatDecompose
	Purpose : decompose a matrix into position, scale and orientation
	Parameters : matrix to decompose, vector for pos, vector for scale, quat for orientation
	Returns : 
	Info : 
*/

void bmMatDecompose(TBMatrix mat, TBVector pos, TBVector scale, TBQuaternion orientation)
{
	return;
}

/*	--------------------------------------------------------------------------------
	Function : bmMatTransOrScale
	Purpose : setup a matrix that performs a scale, orientation and translation in that order
	Parameters : ptr to matrix to set, xTrans,yTrans,zTrans, orientation, xScale,yScale,zScale
	Returns : 
	Info : Matrices are concatenated in 'Trans.Or.Scale' form (so scale acts first)
*/

void bmMatTransOrScale(TBMatrix dest, float xTrans,float yTrans,float zTrans, TBMatrix orientation,
																				float xScale,float yScale,float zScale)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : bmMatTransOr
	Purpose : setup a matrix that performs an orientation and translation in that order
	Parameters : ptr to matrix to set, xTrans,yTrans,zTrans, orientation
	Returns : 
	Info : Matrices are concatenated in 'Trans.Or' form (so orientation acts first)
*/

void bmMatTransOr(TBMatrix dest, float xTrans,float yTrans,float zTrans, TBMatrix orientation)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : bmMatLookAt
	Purpose : setup a matrix that makes a vector along the +ve Z axis look at a point
	Parameters : matrix to fill, position vector, focus point vector, roll around axis (radians)
	Returns : 
	Info : 
*/

void bmMatLookAt(TBMatrix mat, TBVector from, TBVector at, float rollRadians)
{
	return;
}