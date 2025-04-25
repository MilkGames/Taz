#ifndef __H_GLOBAL
#define __H_GLOBAL

#pragma warning( disable : 4101)

#define CHRISCAM
#define CHRISCAM_DEFAULTSTATE 1

#include "babel.h"
#include "bgui.h"

// NH: If I've built a version on my secondary PC set infogrames
#ifdef NHIGGS_RELEASE
#define INFOGRAMES
// NH: For American version
//#define AMERICAN
#endif

#define INFOGRAMES

// NH: For master releases with absolutely NO debug, testing or infogrames stuff included (ie. GUI and screenshots)
#define _MASTER

#ifdef CONSUMERDEMO
	#define INFOGRAMES
	#define _MASTER			// PP: makes sense no?
#endif



#ifdef CONSOLEDEBUG
	void PrintDebugVal(char *info, ...);
	#define bkPrintf PrintDebugVal
#endif

#ifdef INFOGRAMES
#define HENRIK
#endif// PP: def INFOGRAMES

#if BPLATFORM == PS2
#define BOOL int
#endif

// PP: graphics hardware capabilities... ~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if(BPLATFORM == PS2)

#define COLOUR_CHANNEL_MASKING_SUPPORTED			true
#define ZTEST_EQUAL_SUPPORTED						false
#define ZTEST_GREATER_SUPPORTED						false
#define ZTEST_GREATEREQUAL_SUPPORTED				false
#define ZTEST_LESSEQUAL_SUPPORTED					false
													
#elif(BPLATFORM == XBOX)							
													
#define COLOUR_CHANNEL_MASKING_SUPPORTED			true
#define ZTEST_EQUAL_SUPPORTED						true
#define ZTEST_GREATER_SUPPORTED						true
#define ZTEST_GREATEREQUAL_SUPPORTED				true
#define ZTEST_LESSEQUAL_SUPPORTED					true
													
#elif(BPLATFORM == GAMECUBE)							
													
#define COLOUR_CHANNEL_MASKING_SUPPORTED			true
#define ZTEST_EQUAL_SUPPORTED						true	// PP: is it? LH: yup
#define ZTEST_GREATER_SUPPORTED						true	// PP: is it? LH: yup
#define ZTEST_GREATEREQUAL_SUPPORTED				true	// PP: is it? LH: yup
#define ZTEST_LESSEQUAL_SUPPORTED					true	// PP: is it? LH: yup
													
#elif(BPLATFORM == PC)								
													
#define COLOUR_CHANNEL_MASKING_SUPPORTED			(bDisplayInfo.devCaps.PrimitiveMiscCaps & D3DPMISCCAPS_COLORWRITEENABLE)
#define ZTEST_EQUAL_SUPPORTED						true	// PP: is it?
#define ZTEST_GREATER_SUPPORTED						true	// PP: is it?
#define ZTEST_GREATEREQUAL_SUPPORTED				true	// PP: is it?
#define ZTEST_LESSEQUAL_SUPPORTED					true	// PP: is it?

#else// PP: unsupported platform

#error please define the following capabilities for this platform...
// PP: #define COLOUR_CHANNEL_MASKING_SUPPORTED		true/false?
// PP: #define ZTEST_EQUAL_SUPPORTED				true/false?
// PP: #define ZTEST_GREATER_SUPPORTED				true/false?
// PP: #define ZTEST_GREATEREQUAL_SUPPORTED			true/false?
// PP: #define ZTEST_LESSEQUAL_SUPPORTED			true/false?

#endif// PP: platform

// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// PP: at present, cel shading and depth-of-field do not mix well!
//#define CARTOON_DOF

#define X (0)
#define Y (1)
#define Z (2)
#define A (3)
#define W (3)
#define U (0)
#define V (1)
#define EXTRA_SOLID		(1)
#define EXTRA_ALPHA		(2)

extern float fps;
extern float rawFPS;							// PP: frames-per-second before game speed adjustments are applied
extern float speedRatio;
extern float fTime;
extern float rawFTime;							// PP: frame length before game speed adjustments are applied
extern int32 showDebugInfo;


#include "debug.h"


#endif
