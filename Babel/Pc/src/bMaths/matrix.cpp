// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : matrix.h
//   Purpose : matrix functions
// Component : General Maths
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/* --------------------------------------------------------------------------------
   Function : bmMatFactorXYZ()
   Purpose : calculates a set of 3 Euler angles that when passed to bmMatXYZRotation, will result in the given matrix
   Parameters : matrix to factor, floats to store the angles
   Returns : TRUE on unique solution, FALSE if not a unique solution
   Info : 
*/

int bmMatFactorXYZ(const TBMatrix mat,float *xRot,float *yRot,float *zRot)
{
        bkPrintf("*** WARNING *** bmMatFactorXYZ was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bmMatFactorXZY()
   Purpose : calculates a set of 3 Euler angles that when passed to bmMatXZYRotation, will result in the given matrix
   Parameters : matrix to factor, floats to store the angles
   Returns : TRUE on unique solution, FALSE if not a unique solution
   Info : 
*/

int bmMatFactorXZY(const TBMatrix mat,float *xRot,float *yRot,float *zRot)
{
        bkPrintf("*** WARNING *** bmMatFactorXZY was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bmMatFactorYXZ()
   Purpose : calculates a set of 3 Euler angles that when passed to bmMatYXZRotation, will result in the given matrix
   Parameters : matrix to factor, floats to store the angles
   Returns : TRUE on unique solution, FALSE if not a unique solution
   Info : 
*/

int bmMatFactorYXZ(const TBMatrix mat,float *xRot,float *yRot,float *zRot)
{
    const float angleX = bmASin(-mat[2][1]);

    if (xRot) *xRot = angleX;

    /* Thresholds near ±pi/2 */
    const float EPS    = 0.0005f;

    if (angleX > (-HALFPI + EPS) && angleX < (HALFPI - EPS))
    {
        /* General case: all three angles are well defined. */
        if (yRot)
        {
            const float y = bmATan2(mat[2][2], mat[2][0]);
            *yRot = y;
        }

        if (zRot)
        {
            const float z = bmATan2(mat[1][1], mat[0][1]);
            *zRot = z;
        }
        return TRUE;
    }
    else
    {
        /* Gimbal lock: Y and Z are not both uniquely defined. */
        if (yRot)
        {
            float y;
            if (angleX >= 0.0f) y = bmATan2(-mat[1][0], mat[0][0]);
            else y = -bmATan2(-mat[1][0], mat[0][0]);
            *yRot = y;
        }

        if (zRot) *zRot = 0.0f;

        return FALSE;
    }
}


/* --------------------------------------------------------------------------------
   Function : bmMatFactorYZX()
   Purpose : calculates a set of 3 Euler angles that when passed to bmMatYZXRotation, will result in the given matrix
   Parameters : matrix to factor, floats to store the angles
   Returns : TRUE on unique solution, FALSE if not a unique solution
   Info : 
*/

int bmMatFactorYZX(const TBMatrix mat,float *xRot,float *yRot,float *zRot)
{
        bkPrintf("*** WARNING *** bmMatFactorYZX was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bmMatFactorZXY()
   Purpose : calculates a set of 3 Euler angles that when passed to bmMatZXYRotation, will result in the given matrix
   Parameters : matrix to factor, floats to store the angles
   Returns : TRUE on unique solution, FALSE if not a unique solution
   Info : 
*/

int bmMatFactorZXY(const TBMatrix mat,float *xRot,float *yRot,float *zRot)
{
        bkPrintf("*** WARNING *** bmMatFactorZXY was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bmMatFactorZYX()
   Purpose : calculates a set of 3 Euler angles that when passed to bmMatZYXRotation, will result in the given matrix
   Parameters : matrix to factor, floats to store the angles
   Returns : TRUE on unique solution, FALSE if not a unique solution
   Info : 
*/

int bmMatFactorZYX(const TBMatrix mat,float *xRot,float *yRot,float *zRot)
{
        bkPrintf("*** WARNING *** bmMatFactorZYX was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}