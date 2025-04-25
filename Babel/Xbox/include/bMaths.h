// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : bMaths.h
//   Purpose : include all Babel Maths component headers
// Component : Xbox Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BMATHS_H__
#define __BMATHS_H__

// ********************************************************************************
// constants and macros

// maths constants
#define PI		3.1415926f		// PI
#define TWOPI	6.2831853f		// PI*2
#define HALFPI	1.5707963f		// PI/2

// macros to convert between degrees and radians
#define DEG(X)	((X)*57.29577951f)
#define RAD(X)	((X)*0.017453292f)

// often-used macros.
#define MAX(A, B) ( (A > B) ? (A) : (B) )
#define MIN(A, B) ( (A > B) ? (B) : (A) )

// ********************************************************************************
// include all required headers

#include "bMaths/xbMaths.h"
#include "bMaths/maths.h"
#include "bMaths/xbVector.h"
#include "bMaths/vector.h"
#include "bMaths/vector2D.h"
#include "bMaths/xbMatrix.h"
#include "bMaths/matrix.h"
#include "bMaths/xbQuaternion.h"
#include "bMaths/quaternion.h"
#include "bMaths/collision.h"
#include "bMaths/collision2D.h"
#include "bMaths/animatingCollision.h"
#include "bMaths/geometry.h"
#include "bMaths/physics.h"

// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : bInitMaths
	Purpose : initialise the maths module
	Parameters : 
	Returns : 
	Info : 
*/

void bInitMaths();


/*	--------------------------------------------------------------------------------
	Function : bShutdownMaths
	Purpose : shutdown the maths module
	Parameters : 
	Returns : 
	Info : 
*/

void bShutdownMaths();

#endif		// __BMATHS_H__
