#include <babel.h>

TBViewInfo		bViewInfo;					// view information container
D3DMATRIX		bViewMatrix;				// the current view matrix
D3DMATRIX		*bProjectionMatrix;			// current projection matrix
int				bObjectMatrixIsIdentity;	// TRUE if the object matrix is identity
