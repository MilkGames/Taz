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
        bkPrintf("*** WARNING *** bmMatXRotation was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bmMatYRotation was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bmMatZRotation was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
    mat[0][0]=1.0f; mat[0][1]=0.0f; mat[0][2]=0.0f; mat[0][3]=0.0f;
    mat[1][0]=0.0f; mat[1][1]=1.0f; mat[1][2]=0.0f; mat[1][3]=0.0f;
    mat[2][0]=0.0f; mat[2][1]=0.0f; mat[2][2]=1.0f; mat[2][3]=0.0f;
    mat[3][0]=x;    mat[3][1]=y;    mat[3][2]=z;    mat[3][3]=1.0f;
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
    // zero all 16 elements
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            mat[r][c] = 0.0f;

    // set diagonal (row-major)
    mat[0][0] = x;     // _11
    mat[1][1] = y;     // _22
    mat[2][2] = z;     // _33
    mat[3][3] = 1.0f;  // _44
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
	// MG: there's no difference between this and bmMatMultiplyUnligned,
	// MG: because... erm... I don't care?
    TBMatrix A; /* src2 */
    TBMatrix B; /* src1 */
    /* Copy sources into temporaries to handle potential aliasing */
    for (int i = 0; i < 4; ++i) {
        for (int c = 0; c < 4; ++c) {
            A[i][c] = src2[i][c];
            B[i][c] = src1[i][c];
        }
    }
    /* Row-major multiplication: dest = A * B */
    for (int r = 0; r < 4; ++r) {
        const float a0 = A[r][0], a1 = A[r][1], a2 = A[r][2], a3 = A[r][3];
        dest[r][0] = a0*B[0][0] + a1*B[1][0] + a2*B[2][0] + a3*B[3][0];
        dest[r][1] = a0*B[0][1] + a1*B[1][1] + a2*B[2][1] + a3*B[3][1];
        dest[r][2] = a0*B[0][2] + a1*B[1][2] + a2*B[2][2] + a3*B[3][2];
        dest[r][3] = a0*B[0][3] + a1*B[1][3] + a2*B[2][3] + a3*B[3][3];
    }
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
    TBMatrix A; /* src2 */
    TBMatrix B; /* src1 */
    /* Copy sources into temporaries to handle potential aliasing */
    for (int i = 0; i < 4; ++i) {
        for (int c = 0; c < 4; ++c) {
            A[i][c] = src2[i][c];
            B[i][c] = src1[i][c];
        }
    }
    /* Row-major multiplication: dest = A * B */
    for (int r = 0; r < 4; ++r) {
        const float a0 = A[r][0], a1 = A[r][1], a2 = A[r][2], a3 = A[r][3];
        dest[r][0] = a0*B[0][0] + a1*B[1][0] + a2*B[2][0] + a3*B[3][0];
        dest[r][1] = a0*B[0][1] + a1*B[1][1] + a2*B[2][1] + a3*B[3][1];
        dest[r][2] = a0*B[0][2] + a1*B[1][2] + a2*B[2][2] + a3*B[3][2];
        dest[r][3] = a0*B[0][3] + a1*B[1][3] + a2*B[2][3] + a3*B[3][3];
    }
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
        bkPrintf("*** WARNING *** bmMatShow was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bmMatXYRotation was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bmMatYXRotation was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bmMatXZRotation was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bmMatZXRotation was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bmMatYZRotation was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bmMatZYRotation was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bmMatXYZRotation was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bmMatXZYRotation was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bmMatYXZRotation was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bmMatYZXRotation was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
    const float cx = (float)cos(x),  sx = (float)sin(x);
    const float cy = (float)cos(y),  sy = (float)sin(y);
    const float cz = (float)cos(z),  sz = (float)sin(z);

    // Row 0
    dest[0][0] =  cz*cy - sz*sx*sy;
    dest[0][1] = -sz*cx;
    dest[0][2] =  cz*sy + sz*sx*cy;
    dest[0][3] =  0.0f;

    // Row 1
    dest[1][0] =  sz*cy + cz*sx*sy;
    dest[1][1] =  cz*cx;
    dest[1][2] =  sz*sy - cz*sx*cy;
    dest[1][3] =  0.0f;

    // Row 2
    dest[2][0] = -cx*sy;
    dest[2][1] =  sx;
    dest[2][2] =  cx*cy;
    dest[2][3] =  0.0f;

    // Row 3
    dest[3][0] =  0.0f;
    dest[3][1] =  0.0f;
    dest[3][2] =  0.0f;
    dest[3][3] =  1.0f;
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
        bkPrintf("*** WARNING *** bmMatZYXRotation was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bmMatInverse was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
    const float x = vec[0];
    const float y = vec[1];
    const float z = vec[2];
    const float w = vec[3];

    /* Column 0 */
    const float r0 = x*mat[0][0] + y*mat[1][0] + z*mat[2][0] + w*mat[3][0];
    /* Column 1 */
    const float r1 = x*mat[0][1] + y*mat[1][1] + z*mat[2][1] + w*mat[3][1];
    /* Column 2 */
    const float r2 = x*mat[0][2] + y*mat[1][2] + z*mat[2][2] + w*mat[3][2];
    /* Column 3 */
    const float r3 = x*mat[0][3] + y*mat[1][3] + z*mat[2][3] + w*mat[3][3];

    vec[0] = r0;
    vec[1] = r1;
    vec[2] = r2;
    vec[3] = r3;
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
        bkPrintf("*** WARNING *** bmMatMultiplyVector2 was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bmMatTranspose was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bmMatDecompose was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bmMatTransOrScale was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bmMatTransOr was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bmMatLookAt was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}