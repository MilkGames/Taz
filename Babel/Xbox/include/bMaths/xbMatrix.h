// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : xbMatrix.h
//   Purpose : matrix functions
// Component : Xbox Maths
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BMATHS_XBMATRIX_H__
#define __BMATHS_XBMATRIX_H__

// ********************************************************************************
// Constants and Macros

// set a matrix to identity
#define bmMatIdentity(MAT)		memcpy(MAT, bIdentityMatrix, 64)

// set a matrix to zero
#define bmMatZero(MAT)			memcpy(MAT, bZeroMatrix, 64)

// copy a matrix
#define bmMatCopy(DEST, SRC)	memcpy(DEST, SRC, 64)


// ********************************************************************************
// Types, Structures and Classes

// a matrix
typedef float	TBMatrix[4][4];


// ********************************************************************************
// Globals

extern TBMatrix bIdentityMatrix;		// the identity matrix
extern TBMatrix bZeroMatrix;			// the zero matrix


// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : bmMatXRotation
	Purpose : setup a matrix as an X rotation
	Parameters : ptr to matrix to set, rotation in radians
	Returns : 
	Info : 
*/

void bmMatXRotation(TBMatrix mat, const float radians);


/*	--------------------------------------------------------------------------------
	Function : bmMatYRotation
	Purpose : setup a matrix as a Y rotation
	Parameters : ptr to matrix to set, rotation in radians
	Returns : 
	Info : 
*/

void bmMatYRotation(TBMatrix mat, const float radians);


/*	--------------------------------------------------------------------------------
	Function : bmMatZRotation
	Purpose : setup a matrix as a Z rotation
	Parameters : ptr to matrix to set, rotation in radians
	Returns : 
	Info : 
*/

void bmMatZRotation(TBMatrix mat, const float radians);


/*	--------------------------------------------------------------------------------
	Function : bmMatTranslate
	Purpose : setup a matrix as a translation
	Parameters : ptr to matrix to set, x,y,z translations
	Returns : 
	Info : 
*/

void bmMatTranslate(TBMatrix mat, const float x, const float y, const float z);


/*	--------------------------------------------------------------------------------
	Function : bmMatScale
	Purpose : setup a matrix as a scaler
	Parameters : ptr to matrix to set, x,y,z scales
	Returns : 
	Info : 
*/

void bmMatScale(TBMatrix mat, const float x, const float y, const float z);


/*	--------------------------------------------------------------------------------
	Function : bmMatMultiplyAligned
	Purpose : matrix multiplication for 16byte aligned matrices
	Parameters : ptr to destination matrix, ptr to source #1, ptr to source #2
	Returns : 
	Info : multiplication is performed so source#2 takes effect first
*/

void bmMatMultiplyAligned(TBMatrix dest, const TBMatrix src1, const TBMatrix src2);


/*	--------------------------------------------------------------------------------
	Function : bmMatMultiplyUnaligned
	Purpose : matrix multiplication for unaligned matrices
	Parameters : ptr to destination matrix, ptr to source #1, ptr to source #2
	Returns : 
	Info : multiplication is performed so source#2 takes effect first
*/

void bmMatMultiplyUnaligned(TBMatrix dest, const TBMatrix src1, const TBMatrix src2);


/*	--------------------------------------------------------------------------------
	Function : bmMatMultiply
	Purpose : matrix multiplication.
	Parameters : ptr to destination matrix, ptr to source #1, ptr to source #2
	Returns : 
	Info : multiplication is performed so source#2 takes effect first
*/

#define bmMatMultiply(DEST, SRC1, SRC2) if ((((int)SRC1) | ((int)SRC2) | ((int)DEST))&0x0f) bmMatMultiplyUnaligned(DEST, SRC1, SRC2); else bmMatMultiplyAligned(DEST, SRC1, SRC2)


/*	--------------------------------------------------------------------------------
	Function : bmMatShow
	Purpose : build a visual representation of a matrix ready for printing
	Parameters : matrix to show, buffer to fill
	Returns : ptr to buffer
	Info : 
*/

char *bmMatShow(const TBMatrix mat, char * const buf);


/*	--------------------------------------------------------------------------------
	Function : bmMatXYRotation
	Purpose : setup a matrix as a general rotation
	Parameters : ptr to matrix to set, x,y rotations in radians
	Returns : 
	Info : Matrices are concatenated in XY form (so Y rotation acts first)
*/

void bmMatXYRotation(TBMatrix dest, const float x, const float y);


/*	--------------------------------------------------------------------------------
	Function : bmMatYXRotation
	Purpose : setup a matrix as a general rotation
	Parameters : ptr to matrix to set, x,y rotations in radians
	Returns : 
	Info : Matrices are concatenated in YX form (so X rotation acts first)
*/

void bmMatYXRotation(TBMatrix dest, const float x, const float y);


/*	--------------------------------------------------------------------------------
	Function : bmMatXZRotation
	Purpose : setup a matrix as a general rotation
	Parameters : ptr to matrix to set, x,z rotations in radians
	Returns : 
	Info : Matrices are concatenated in XZ form (so Z rotation acts first)
*/

void bmMatXZRotation(TBMatrix dest, const float x, const float z);


/*	--------------------------------------------------------------------------------
	Function : bmMatZXRotation
	Purpose : setup a matrix as a general rotation
	Parameters : ptr to matrix to set, x,z rotations in radians
	Returns : 
	Info : Matrices are concatenated in ZX form (so X rotation acts first)
*/

void bmMatZXRotation(TBMatrix dest, const float x, const float z);


/*	--------------------------------------------------------------------------------
	Function : bmMatYZRotation
	Purpose : setup a matrix as a general rotation
	Parameters : ptr to matrix to set, y,z rotations in radians
	Returns : 
	Info : Matrices are concatenated in YZ form (so Z rotation acts first)
*/

void bmMatYZRotation(TBMatrix dest, const float y, const float z);


/*	--------------------------------------------------------------------------------
	Function : bmMatZYRotation
	Purpose : setup a matrix as a general rotation
	Parameters : ptr to matrix to set, y,z rotations in radians
	Returns : 
	Info : Matrices are concatenated in ZY form (so Y rotation acts first)
*/

void bmMatZYRotation(TBMatrix dest, const float y, const float z);


/*	--------------------------------------------------------------------------------
	Function : bmMatXYZRotation
	Purpose : setup a matrix as a general rotation
	Parameters : ptr to matrix to set, x,y,z rotations in radians
	Returns : 
	Info : Matrices are concatenated in XYZ form (so Z rotation acts first)
*/

void bmMatXYZRotation(TBMatrix dest, const float xRadians, const float yRadians, const float zRadians);


/*	--------------------------------------------------------------------------------
	Function : bmMatXZYRotation
	Purpose : setup a matrix as a general rotation
	Parameters : ptr to matrix to set, x,y,z rotations in radians
	Returns : 
	Info : Matrices are concatenated in XZY form (so Y rotation happens first)
*/

void bmMatXZYRotation(TBMatrix dest, const float x, const float y, const float z);


/*	--------------------------------------------------------------------------------
	Function : bmMatYXZRotation
	Purpose : setup a matrix as a general rotation
	Parameters : ptr to matrix to set, x,y,z rotations in radians
	Returns : 
	Info : Matrices are concatenated in YXZ form (so Z rotation happens first)
*/

void bmMatYXZRotation(TBMatrix dest, const float x, const float y, const float z);


/*	--------------------------------------------------------------------------------
	Function : bmMatYZXRotation
	Purpose : setup a matrix as a general rotation
	Parameters : ptr to matrix to fill, x,y,z rotations in radians
	Returns : 
	Info : Matrices are concatenated in YZX form (so X rotation happens first)
*/

void bmMatYZXRotation(TBMatrix dest, const float x, const float y, const float z);


/*	--------------------------------------------------------------------------------
	Function : bmMatZXYRotation
	Purpose : setup a matrix as a general rotation
	Parameters : ptr to matrix to set, x,y,z rotations in radians
	Returns : 
	Info : Matrices are concatenated in ZXY form (so Y rotation happens first)
*/

void bmMatZXYRotation(TBMatrix dest, const float x, const float y, const float z);


/*	--------------------------------------------------------------------------------
	Function : bmMatZYXRotation
	Purpose : setup a matrix as a general rotation
	Parameters : ptr to matrix to set, x,y,z rotations in radians
	Returns : 
	Info : Matrices are concatenated in ZYX form (so X rotation happens first)
*/

void bmMatZYXRotation(TBMatrix dest, const float x, const float y, const float z);


/*	--------------------------------------------------------------------------------
	Function : bmMatInverse
	Purpose : setup a matrix as the inverse of another
	Parameters : ptr to dest matrix, ptr to matrix to invert
	Returns : 1 for success, 0 if singular
	Info : 
*/

int bmMatInverse(TBMatrix dest, const TBMatrix in);


/*	--------------------------------------------------------------------------------
	Function : bmMatMultiplyVector
	Purpose : Multiply a vector by a matrix
	Parameters : matrix ptr, vector to multiply
	Returns : 
	Info : 
*/

void bmMatMultiplyVector(const TBMatrix mat, TBVector vec);


/*	--------------------------------------------------------------------------------
	Function : bmMatMultiplyVector2
	Purpose : Multiply a vector by a matrix
	Parameters : dest, matrix ptr, vector to multiply
	Returns : 
	Info : 
*/

void bmMatMultiplyVector2(TBVector dest, const TBMatrix mat, const TBVector vec);


/*	--------------------------------------------------------------------------------
	Function : bmMatTranspose
	Purpose : make a matrix the transpose of another
	Parameters : dest matrix, source matrix
	Returns : 
	Info : 
*/

void bmMatTranspose(TBMatrix dest, const TBMatrix src);


/*	--------------------------------------------------------------------------------
	Function : bmMatDet33
	Purpose : Calculate the determinant of the upper left 3x3 part of the matrix src
	Parameters : [in] source matrix
	Returns : determinant of the matrix
	Info : 
*/

inline float bmMatDet33(const TBMatrix src)
{
	return (src[0][0] * (src[1][1]*src[2][2] - src[1][2]*src[2][1])) -
			(src[0][1] * (src[1][0]*src[2][2] - src[1][2]*src[2][0])) +
			(src[0][2] * (src[1][0]*src[2][1] - src[1][1]*src[2][0]));
}


/*	--------------------------------------------------------------------------------
	Function : bmMatDecompose
	Purpose : decompose a matrix into position, scale and orientation
	Parameters : matrix to decompose, vector for pos, vector for scale, quat for orientation
	Returns : 
	Info : 
*/

void bmMatDecompose(TBMatrix mat, TBVector pos, TBVector scale, TBQuaternion orientation);


/*	--------------------------------------------------------------------------------
	Function : bmMatGetColumn
	Purpose : Get the specified column of the matrix.
	Parameters : [in] the zero-based column index.
				[out] the column you requested.
	Returns : 
	Info : 
*/

inline void bmMatGetColumn(const int column, const TBMatrix mat, TBVector result)
{
	bmVectorSet(result, mat[0][column], mat[1][column], mat[2][column], mat[3][column]);
}


/*	--------------------------------------------------------------------------------
	Function : bmMatGetRow
	Purpose : Get the specified row of the matrix.
	Parameters : [in] the zero-based row index.
				[in] the matrix
				[out] the row you requested.
	Returns : 
	Info : 
*/

inline void bmMatGetRow(const int row, const TBMatrix mat, TBVector result)
{
	bmVectorSet(result, mat[row][0], mat[row][1], mat[row][2], mat[row][3]);
}


/*	--------------------------------------------------------------------------------
	Function : bmMatTransOrScale
	Purpose : setup a matrix that performs a scale, orientation and translation in that order
	Parameters : ptr to matrix to set, xTrans,yTrans,zTrans, orientation, xScale,yScale,zScale
	Returns : 
	Info : Matrices are concatenated in 'Trans.Or.Scale' form (so scale acts first)
*/

void bmMatTransOrScale(TBMatrix dest, float xTrans,float yTrans,float zTrans, TBMatrix orientation,
																				float xScale,float yScale,float zScale);


/*	--------------------------------------------------------------------------------
	Function : bmMatTransOr
	Purpose : setup a matrix that performs an orientation and translation in that order
	Parameters : ptr to matrix to set, xTrans,yTrans,zTrans, orientation
	Returns : 
	Info : Matrices are concatenated in 'Trans.Or' form (so orientation acts first)
*/

void bmMatTransOr(TBMatrix dest, float xTrans,float yTrans,float zTrans, TBMatrix orientation);


/*	--------------------------------------------------------------------------------
	Function : bmMatLookAt
	Purpose : setup a matrix that makes a vector along the +ve Z axis look at a point
	Parameters : matrix to fill, position vector, focus point vector, roll around axis (radians)
	Returns : 
	Info : 
*/

void bmMatLookAt(TBMatrix mat, TBVector from, TBVector at, float rollRadians);

#endif		// __BMATHS_XBMATRIX_H__
