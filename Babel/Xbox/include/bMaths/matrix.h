// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : matrix.h
//   Purpose : matrix functions
// Component : General Maths
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BMATHS_MATRIX_H__
#define __BMATHS_MATRIX_H__

// ********************************************************************************
// Constants and Macros


// ********************************************************************************
// Types, Structures and Classes


// ********************************************************************************
// Globals


// ********************************************************************************
// Prototypes


/* --------------------------------------------------------------------------------
   Function : bmMatFactorXYZ()
   Purpose : calculates a set of 3 Euler angles that when passed to bmMatXYZRotation, will result in the given matrix
   Parameters : matrix to factor, floats to store the angles
   Returns : TRUE on unique solution, FALSE if not a unique solution
   Info : 
*/

int bmMatFactorXYZ(const TBMatrix mat,float *xRot,float *yRot,float *zRot);


/* --------------------------------------------------------------------------------
   Function : bmMatFactorXZY()
   Purpose : calculates a set of 3 Euler angles that when passed to bmMatXZYRotation, will result in the given matrix
   Parameters : matrix to factor, floats to store the angles
   Returns : TRUE on unique solution, FALSE if not a unique solution
   Info : 
*/

int bmMatFactorXZY(const TBMatrix mat,float *xRot,float *yRot,float *zRot);


/* --------------------------------------------------------------------------------
   Function : bmMatFactorYXZ()
   Purpose : calculates a set of 3 Euler angles that when passed to bmMatYXZRotation, will result in the given matrix
   Parameters : matrix to factor, floats to store the angles
   Returns : TRUE on unique solution, FALSE if not a unique solution
   Info : 
*/

int bmMatFactorYXZ(const TBMatrix mat,float *xRot,float *yRot,float *zRot);


/* --------------------------------------------------------------------------------
   Function : bmMatFactorYZX()
   Purpose : calculates a set of 3 Euler angles that when passed to bmMatYZXRotation, will result in the given matrix
   Parameters : matrix to factor, floats to store the angles
   Returns : TRUE on unique solution, FALSE if not a unique solution
   Info : 
*/

int bmMatFactorYZX(const TBMatrix mat,float *xRot,float *yRot,float *zRot);


/* --------------------------------------------------------------------------------
   Function : bmMatFactorZXY()
   Purpose : calculates a set of 3 Euler angles that when passed to bmMatZXYRotation, will result in the given matrix
   Parameters : matrix to factor, floats to store the angles
   Returns : TRUE on unique solution, FALSE if not a unique solution
   Info : 
*/

int bmMatFactorZXY(const TBMatrix mat,float *xRot,float *yRot,float *zRot);


/* --------------------------------------------------------------------------------
   Function : bmMatFactorZYX()
   Purpose : calculates a set of 3 Euler angles that when passed to bmMatZYXRotation, will result in the given matrix
   Parameters : matrix to factor, floats to store the angles
   Returns : TRUE on unique solution, FALSE if not a unique solution
   Info : 
*/

int bmMatFactorZYX(const TBMatrix mat,float *xRot,float *yRot,float *zRot);


/*	--------------------------------------------------------------------------------
	Function : bmMatGetRight
	Purpose : Get the right vector from the matrix.
	Parameters : [in] the matrix [out] the right vector.
	Returns : 
	Info : 
*/

inline void bmMatGetRight(const TBMatrix mat, TBVector result)		{bmMatGetRow(0,mat,result);}


/*	--------------------------------------------------------------------------------
	Function : bmMatGetUp
	Purpose : Get the up vector from the matrix.
	Parameters : [in] the matrix [out] the up vector.
	Returns : 
	Info : 
*/

inline void bmMatGetUp(const TBMatrix mat, TBVector result)			{bmMatGetRow(1,mat,result);}


/*	--------------------------------------------------------------------------------
	Function : bmMatGetForwards
	Purpose : Get the forward vector from the matrix.
	Parameters : [in] the matrix [out] the forward vector.
	Returns : 
	Info : 
*/

inline void bmMatGetForwards(const TBMatrix mat, TBVector result)	{bmMatGetRow(2,mat,result);}


#endif		// __BMATHS_MATRIX_H__
