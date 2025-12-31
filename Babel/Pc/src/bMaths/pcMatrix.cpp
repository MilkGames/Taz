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

TBMatrix bIdentityMatrix = { 
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
    const float c = bmCos(radians);
    const float s = bmSin(radians);

    // First row
    mat[0][0] = 1.0f;
    mat[0][1] = 0.0f;
    mat[0][2] = 0.0f;
    mat[0][3] = 0.0f;

    // Second row
    mat[1][0] = 0.0f;
    mat[1][1] = c;
    mat[1][2] = s;
    mat[1][3] = 0.0f;

    // Third row
    mat[2][0] = 0.0f;
    mat[2][1] = -s;
    mat[2][2] = c;
    mat[2][3] = 0.0f;

    // Fourth row
    mat[3][0] = 0.0f;
    mat[3][1] = 0.0f;
    mat[3][2] = 0.0f;
    mat[3][3] = 1.0f;
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
    const float c = bmCos(radians);
    const float s = bmSin(radians);

    // First row
    mat[0][0] = c;
    mat[0][1] = 0.0f;
    mat[0][2] = -s;
    mat[0][3] = 0.0f;

    // Second row
    mat[1][0] = 0.0f;
    mat[1][1] = 1.0f;
    mat[1][2] = 0.0f;
    mat[1][3] = 0.0f;

    // Third row
    mat[2][0] = s;
    mat[2][1] = 0.0f;
    mat[2][2] = c;
    mat[2][3] = 0.0f;

    // Fourth row
    mat[3][0] = 0.0f;
    mat[3][1] = 0.0f;
    mat[3][2] = 0.0f;
    mat[3][3] = 1.0f;
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
    const float c = bmCos(radians);
    const float s = bmSin(radians);

    // First row
    mat[0][0] = c;
    mat[0][1] = s;
    mat[0][2] = 0.0f;
    mat[0][3] = 0.0f;

    // Second row
    mat[1][0] = -s;
    mat[1][1] = c;
    mat[1][2] = 0.0f;
    mat[1][3] = 0.0f;

    // Third row
    mat[2][0] = 0.0f;
    mat[2][1] = 0.0f;
    mat[2][2] = 1.0f;
    mat[2][3] = 0.0f;

    // Fourth row
    mat[3][0] = 0.0f;
    mat[3][1] = 0.0f;
    mat[3][2] = 0.0f;
    mat[3][3] = 1.0f;
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
	/* Row-major multiplication with explicit ordering: dest = src2 * src1 */
	for (int r = 0; r < 4; ++r)
	{
		const float a0 = src2[r][0];
		const float a1 = src2[r][1];
		const float a2 = src2[r][2];
		const float a3 = src2[r][3];

		dest[r][0] = a0 * src1[0][0] + a1 * src1[1][0] + a2 * src1[2][0] + a3 * src1[3][0];
		dest[r][1] = a0 * src1[0][1] + a1 * src1[1][1] + a2 * src1[2][1] + a3 * src1[3][1];
		dest[r][2] = a0 * src1[0][2] + a1 * src1[1][2] + a2 * src1[2][2] + a3 * src1[3][2];
		dest[r][3] = a0 * src1[0][3] + a1 * src1[1][3] + a2 * src1[2][3] + a3 * src1[3][3];
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
    const float cx = bmCos(xRadians);
    const float sx = bmSin(xRadians);
    const float cy = bmCos(yRadians);
    const float sy = bmSin(yRadians);
    const float cz = bmCos(zRadians);
    const float sz = bmSin(zRadians);

    const float czSy = cz * sy;
    const float szSy = sz * sy;

    dest[0][0] = cz * cy;
    dest[0][1] = (czSy * sx) + (sz * cx);
    dest[0][2] = (sz * sx) - (czSy * cx);
    dest[0][3] = 0.0f;

    dest[1][0] = -(sz * cy);
    dest[1][1] = (cz * cx) - (szSy * sx);
    dest[1][2] = (szSy * cx) + (cz * sx);
    dest[1][3] = 0.0f;

    dest[2][0] = sy;
    dest[2][1] = -(cy * sx);
    dest[2][2] = cy * cx;
    dest[2][3] = 0.0f;

    dest[3][0] = 0.0f;
    dest[3][1] = 0.0f;
    dest[3][2] = 0.0f;
    dest[3][3] = 1.0f;
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
    float sx = (float)sin(x);
    float cx = (float)cos(x);
    float sy = (float)sin(y);
    float cy = (float)cos(y);
    float sz = (float)sin(z);
    float cz = (float)cos(z);

    dest[0][0] = cy * cz + sx * sy * sz;
    dest[0][1] = cx * sz;
    dest[0][2] = sx * sz * cy - sy * cz;
    dest[0][3] = 0.0f;

    dest[1][0] = sx * sy * cz - cy * sz;
    dest[1][1] = cx * cz;
    dest[1][2] = sx * cz * cy + sy * sz;
    dest[1][3] = 0.0f;

    dest[2][0] = sy * cx;
    dest[2][1] = -sx;
    dest[2][2] = cy * cx;
    dest[2][3] = 0.0f;

    dest[3][0] = 0.0f;
    dest[3][1] = 0.0f;
    dest[3][2] = 0.0f;
    dest[3][3] = 1.0f;
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
    const float cx = bmCos(x);
    const float sx = bmSin(x);
    const float cy = bmCos(y);
    const float sy = bmSin(y);
    const float cz = bmCos(z);
    const float sz = bmSin(z);

    dest[0][0] =  cz * cy;
    dest[0][1] =  sz * cy;
    dest[0][2] = -sy;
    dest[0][3] =  0.0f;

    dest[1][0] =  cz * sy * sx - sz * cx;
    dest[1][1] =  cz * cx + sz * sy * sx;
    dest[1][2] =  cy * sx;
    dest[1][3] =  0.0f;

    dest[2][0] =  sz * sx + cz * sy * cx;
    dest[2][1] =  sz * sy * cx - cz * sx;
    dest[2][2] =  cy * cx;
    dest[2][3] =  0.0f;

    dest[3][0] =  0.0f;
    dest[3][1] =  0.0f;
    dest[3][2] =  0.0f;
    dest[3][3] =  1.0f;
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
    float pos = 0.0f;
    float neg = 0.0f;

    float term;

    // aei
    term = in[2][2] * in[0][0] * in[1][1];
    if (0.0f < term) pos += term; else neg += term;

    // bfg
    term = in[0][1] * in[2][0] * in[1][2];
    if (0.0f < term) pos += term; else neg += term;

    // cdh
    term = in[2][1] * in[1][0] * in[0][2];
    if (0.0f < term) pos += term; else neg += term;

    // -ceg
    term = -(in[2][0] * in[0][2] * in[1][1]);
    if (0.0f < term) pos += term; else neg += term;

    // -bdi
    term = -(in[2][2] * in[0][1] * in[1][0]);
    if (0.0f < term) pos += term; else neg += term;

    // -afh
    term = -(in[2][1] * in[1][2] * in[0][0]);
    if (0.0f < term) pos += term; else neg += term;

    const float det = neg + pos;
    if (det == 0.0f)
        return 0;

    // Robust singularity test: abs(det / (pos - neg)) >= 1e-15
    {
        const double denom = (double)(pos - neg);
        const double ratio = (double)det / denom;
        const double ar = fabs(ratio);
        if (!(ar >= 1.0000000036274937e-15))
            return 0;
    }

    const float invDet = 1.0f / det;

    // Inverse of upper-left 3x3
    dest[0][0] = (in[2][2] * in[1][1] - in[2][1] * in[1][2]) * invDet;
    dest[1][0] = -((in[2][2] * in[1][0] - in[2][0] * in[1][2]) * invDet);
    dest[2][0] = (in[2][1] * in[1][0] - in[2][0] * in[1][1]) * invDet;

    dest[0][1] = -((in[2][2] * in[0][1] - in[2][1] * in[0][2]) * invDet);
    dest[1][1] = (in[2][2] * in[0][0] - in[2][0] * in[0][2]) * invDet;
    dest[2][1] = -((in[2][1] * in[0][0] - in[0][1] * in[2][0]) * invDet);

    dest[0][2] = (in[0][1] * in[1][2] - in[0][2] * in[1][1]) * invDet;
    dest[1][2] = -((in[1][2] * in[0][0] - in[1][0] * in[0][2]) * invDet);
    dest[2][2] = (in[0][0] * in[1][1] - in[0][1] * in[1][0]) * invDet;

    // Affine form
    dest[0][3] = 0.0f;
    dest[1][3] = 0.0f;
    dest[2][3] = 0.0f;

    // Translation (row 3)
    dest[3][0] = -(in[3][0] * dest[0][0] + in[3][1] * dest[1][0] + in[3][2] * dest[2][0]);
    dest[3][1] = -(in[3][0] * dest[0][1] + in[3][1] * dest[1][1] + in[3][2] * dest[2][1]);
    dest[3][2] = -(in[3][0] * dest[0][2] + in[3][1] * dest[1][2] + in[3][2] * dest[2][2]);
    dest[3][3] = 1.0f;

    return 1;
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
    const float x = vec[0];
    const float y = vec[1];
    const float z = vec[2];
    const float w = vec[3];

    dest[0] = x * mat[0][0] + y * mat[1][0] + z * mat[2][0] + w * mat[3][0];
    dest[1] = x * mat[0][1] + y * mat[1][1] + z * mat[2][1] + w * mat[3][1];
    dest[2] = x * mat[0][2] + y * mat[1][2] + z * mat[2][2] + w * mat[3][2];
    dest[3] = x * mat[0][3] + y * mat[1][3] + z * mat[2][3] + w * mat[3][3];
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
    float r0x = mat[0][0], r0y = mat[0][1], r0z = mat[0][2];
    float r1x = mat[1][0], r1y = mat[1][1], r1z = mat[1][2];
    float r2x = mat[2][0], r2y = mat[2][1], r2z = mat[2][2];

    pos[0] = mat[3][0];
    pos[1] = mat[3][1];
    pos[2] = mat[3][2];
    pos[3] = 1.0f;

    float sx = bmSqrtApprox(r0x*r0x + r0y*r0y + r0z*r0z);
    scale[0] = sx;
    if (sx != 0.0f) {
        float inv = 1.0f / sx;
        r0x *= inv; r0y *= inv; r0z *= inv;
    }

    float t = -(r1x*r0x + r1y*r0y + r1z*r0z);
    r1x += t*r0x; r1y += t*r0y; r1z += t*r0z;

    float sy = bmSqrtApprox(r1x*r1x + r1y*r1y + r1z*r1z);
    scale[1] = sy;
    if (sy != 0.0f) {
        float inv = 1.0f / sy;
        r1x *= inv; r1y *= inv; r1z *= inv;
    }

    t = -(r2x*r0x + r2y*r0y + r2z*r0z);
    r2x += t*r0x; r2y += t*r0y; r2z += t*r0z;

    t = -(r2x*r1x + r2y*r1y + r2z*r1z);
    r2x += t*r1x; r2y += t*r1y; r2z += t*r1z;

    float sz = bmSqrtApprox(r2x*r2x + r2y*r2y + r2z*r2z);
    scale[2] = sz;
    if (sz != 0.0f) {
        float inv = 1.0f / sz;
        r2x *= inv; r2y *= inv; r2z *= inv;
    }

    float cx = r1y*r2z - r1z*r2y;
    float cy = r1z*r2x - r1x*r2z;
    float cz = r1x*r2y - r1y*r2x;

    if (cx*r0x + cy*r0y + cz*r0z < 0.0f) {
        scale[0] = -scale[0]; r0x = -r0x; r0y = -r0y; r0z = -r0z;
        scale[1] = -scale[1]; r1x = -r1x; r1y = -r1y; r1z = -r1z;
        scale[2] = -scale[2]; r2x = -r2x; r2y = -r2y; r2z = -r2z;
    }

    scale[3] = 1.0f;

    TBMatrix rot;
    rot[0][0]=r0x; rot[0][1]=r0y; rot[0][2]=r0z; rot[0][3]=0.0f;
    rot[1][0]=r1x; rot[1][1]=r1y; rot[1][2]=r1z; rot[1][3]=0.0f;
    rot[2][0]=r2x; rot[2][1]=r2y; rot[2][2]=r2z; rot[2][3]=0.0f;
    rot[3][0]=0.0f; rot[3][1]=0.0f; rot[3][2]=0.0f; rot[3][3]=1.0f;

    bmMatrixToQuat(orientation, rot);
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
	float *d = &dest[0][0];
	float *o = &orientation[0][0];

	// scale * orientation
	d[0]  = o[0]  * xScale;
	d[1]  = o[1]  * xScale;
	d[2]  = o[2]  * xScale;
	d[3]  = 0.0f;

	d[4]  = o[4]  * yScale;
	d[5]  = o[5]  * yScale;
	d[6]  = o[6]  * yScale;
	d[7]  = 0.0f;

	d[8]  = o[8]  * zScale;
	d[9]  = o[9]  * zScale;
	d[10] = o[10] * zScale;
	d[11] = 0.0f;

	// translation
	d[12] = xTrans;
	d[13] = yTrans;
	d[14] = zTrans;
	d[15] = 1.0f;
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
    // If dest aliases orientation, only update translation
    if (dest != orientation)
    {
        dest[0][0] = orientation[0][0];
        dest[1][0] = orientation[1][0];
        dest[2][0] = orientation[2][0];

        dest[0][1] = orientation[0][1];
        dest[1][1] = orientation[1][1];
        dest[2][1] = orientation[2][1];

        dest[0][2] = orientation[0][2];
        dest[1][2] = orientation[1][2];
        dest[2][2] = orientation[2][2];

        dest[3][0] = xTrans;
        dest[3][1] = yTrans;
        dest[3][2] = zTrans;

        dest[0][3] = 0.0f;
        dest[1][3] = 0.0f;
        dest[2][3] = 0.0f;
        dest[3][3] = 1.0f;
        return;
    }

    dest[3][0] = xTrans;
    dest[3][1] = yTrans;
    dest[3][2] = zTrans;
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