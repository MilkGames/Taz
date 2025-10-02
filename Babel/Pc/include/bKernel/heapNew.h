// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : heapNew.h
//   Purpose : memory management and heap control defines
// Component : Generic Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#ifndef __BKERNEL_HEAP_NEW_H__
//#define __BKERNEL_HEAP_NEW_H__


// ********************************************************************************
// Constants and Macros

// macros for memory allocation and release
#ifdef __BABEL__
	#if defined(_BACTOR_)
		#define MALLOC(SIZE)						bkHeapAlloc(SIZE, __FILE__, __LINE__, BALLOC_MALLOC | bActorModule)
		#define MALLOCSRC(SIZE, FILE, LINE)			bkHeapAlloc(SIZE, FILE, LINE, BALLOC_MALLOC | bActorModule)
		#define CALLOC(SIZE, VALUE)					bkHeapCalloc(SIZE, VALUE, __FILE__, __LINE__, BALLOC_MALLOC | bActorModule)
		#define CALLOCSRC(SIZE, VALUE, FILE, LINE)	bkHeapCalloc(SIZE, VALUE, FILE, LINE, BALLOC_MALLOC | bActorModule)
		#define new									new(__FILE__, __LINE__, BALLOC_NEW | bActorModule)
	#elif defined(_BDISPLAY_)
		#define MALLOC(SIZE)						bkHeapAlloc(SIZE, __FILE__, __LINE__, BALLOC_MALLOC | bDisplayModule)
		#define MALLOCSRC(SIZE, FILE, LINE)			bkHeapAlloc(SIZE, FILE, LINE, BALLOC_MALLOC | bDisplayModule)
		#define CALLOC(SIZE, VALUE)					bkHeapCalloc(SIZE, VALUE, __FILE__, __LINE__, BALLOC_MALLOC | bDisplayModule)
		#define CALLOCSRC(SIZE, VALUE, FILE, LINE)	bkHeapCalloc(SIZE, VALUE, FILE, LINE, BALLOC_MALLOC | bDisplayModule)
		#define new									new(__FILE__, __LINE__, BALLOC_NEW | bDisplayModule)
	#elif defined(_BGUI_)
		#define MALLOC(SIZE)						bkHeapAlloc(SIZE, __FILE__, __LINE__, BALLOC_MALLOC | bGUIModule)
		#define MALLOCSRC(SIZE, FILE, LINE)			bkHeapAlloc(SIZE, FILE, LINE, BALLOC_MALLOC | bGUIModule)
		#define CALLOC(SIZE, VALUE)					bkHeapCalloc(SIZE, VALUE, __FILE__, __LINE__, BALLOC_MALLOC | bGUIModule)
		#define CALLOCSRC(SIZE, VALUE, FILE, LINE)	bkHeapCalloc(SIZE, VALUE, FILE, LINE, BALLOC_MALLOC | bGUIModule)
		#define new									new(__FILE__, __LINE__, BALLOC_NEW | bGUIModule)
	#elif defined(_BINPUT_)
		#define MALLOC(SIZE)						bkHeapAlloc(SIZE, __FILE__, __LINE__, BALLOC_MALLOC | bInputModule)
		#define MALLOCSRC(SIZE, FILE, LINE)			bkHeapAlloc(SIZE, FILE, LINE, BALLOC_MALLOC | bInputModule)
		#define CALLOC(SIZE, VALUE)					bkHeapCalloc(SIZE, VALUE, __FILE__, __LINE__, BALLOC_MALLOC | bInputModule)
		#define CALLOCSRC(SIZE, VALUE, FILE, LINE)	bkHeapCalloc(SIZE, VALUE, FILE, LINE, BALLOC_MALLOC | bInputModule)
		#define new									new(__FILE__, __LINE__, BALLOC_NEW | bInputModule)
	#elif defined(_BKERNEL_)
		#define MALLOC(SIZE)						bkHeapAlloc(SIZE, __FILE__, __LINE__, BALLOC_MALLOC | bKernelModule)
		#define MALLOCSRC(SIZE, FILE, LINE)			bkHeapAlloc(SIZE, FILE, LINE, BALLOC_MALLOC | bKernelModule)
		#define CALLOC(SIZE, VALUE)					bkHeapCalloc(SIZE, VALUE, __FILE__, __LINE__, BALLOC_MALLOC | bKernelModule)
		#define CALLOCSRC(SIZE, VALUE, FILE, LINE)	bkHeapCalloc(SIZE, VALUE, FILE, LINE, BALLOC_MALLOC | bKernelModule)
		#define new									new(__FILE__, __LINE__, BALLOC_NEW | bKernelModule)
	#elif defined(_BMATHS_)
		#define MALLOC(SIZE)						bkHeapAlloc(SIZE, __FILE__, __LINE__, BALLOC_MALLOC | bMathsModule)
		#define MALLOCSRC(SIZE, FILE, LINE)			bkHeapAlloc(SIZE, FILE, LINE, BALLOC_MALLOC | bMathsModule)
		#define CALLOC(SIZE, VALUE)					bkHeapCalloc(SIZE, VALUE, __FILE__, __LINE__, BALLOC_MALLOC | bMathsModule)
		#define CALLOCSRC(SIZE, VALUE, FILE, LINE)	bkHeapCalloc(SIZE, VALUE, FILE, LINE, BALLOC_MALLOC | bMathsModule)
		#define new									new(__FILE__, __LINE__, BALLOC_NEW | bMathsModule)
	#elif defined(_BSOUND_)
		#define MALLOC(SIZE)						bkHeapAlloc(SIZE, __FILE__, __LINE__, BALLOC_MALLOC | bSoundModule)
		#define MALLOCSRC(SIZE, FILE, LINE)			bkHeapAlloc(SIZE, FILE, LINE, BALLOC_MALLOC | bSoundModule)
		#define CALLOC(SIZE, VALUE)					bkHeapCalloc(SIZE, VALUE, __FILE__, __LINE__, BALLOC_MALLOC | bSoundModule)
		#define CALLOCSRC(SIZE, VALUE, FILE, LINE)	bkHeapCalloc(SIZE, VALUE, FILE, LINE, BALLOC_MALLOC | bSoundModule)
		#define new									new(__FILE__, __LINE__, BALLOC_NEW | bSoundModule)
	#elif defined(_FCAMERA_)
		#define MALLOC(SIZE)						bkHeapAlloc(SIZE, __FILE__, __LINE__, BALLOC_MALLOC | fCameraModule)
		#define MALLOCSRC(SIZE, FILE, LINE)			bkHeapAlloc(SIZE, FILE, LINE, BALLOC_MALLOC | fCameraModule)
		#define CALLOC(SIZE, VALUE)					bkHeapCalloc(SIZE, VALUE, __FILE__, __LINE__, BALLOC_MALLOC | fCameraModule)
		#define CALLOCSRC(SIZE, VALUE, FILE, LINE)	bkHeapCalloc(SIZE, VALUE, FILE, LINE, BALLOC_MALLOC | fCameraModule)
		#define new									new(__FILE__, __LINE__, BALLOC_NEW | fCameraModule)
	#elif defined(_FGUI_)
		#define MALLOC(SIZE)						bkHeapAlloc(SIZE, __FILE__, __LINE__, BALLOC_MALLOC | fGUIModule)
		#define MALLOCSRC(SIZE, FILE, LINE)			bkHeapAlloc(SIZE, FILE, LINE, BALLOC_MALLOC | fGUIModule)
		#define CALLOC(SIZE, VALUE)					bkHeapCalloc(SIZE, VALUE, __FILE__, __LINE__, BALLOC_MALLOC | fGUIModule)
		#define CALLOCSRC(SIZE, VALUE, FILE, LINE)	bkHeapCalloc(SIZE, VALUE, FILE, LINE, BALLOC_MALLOC | fGUIModule)
		#define new									new(__FILE__, __LINE__, BALLOC_NEW | fGUIModule)
	#elif defined(_FSCENE_)
		#define MALLOC(SIZE)						bkHeapAlloc(SIZE, __FILE__, __LINE__, BALLOC_MALLOC | fSceneModule)
		#define MALLOCSRC(SIZE, FILE, LINE)			bkHeapAlloc(SIZE, FILE, LINE, BALLOC_MALLOC | fSceneModule)
		#define CALLOC(SIZE, VALUE)					bkHeapCalloc(SIZE, VALUE, __FILE__, __LINE__, BALLOC_MALLOC | fSceneModule)
		#define CALLOCSRC(SIZE, VALUE, FILE, LINE)	bkHeapCalloc(SIZE, VALUE, FILE, LINE, BALLOC_MALLOC | fSceneModule)
		#define new									new(__FILE__, __LINE__, BALLOC_NEW | fSceneModule)
	#elif defined(_FEFFECTS_)
		#define MALLOC(SIZE)						bkHeapAlloc(SIZE, __FILE__, __LINE__, BALLOC_MALLOC | fEffectsModule)
		#define MALLOCSRC(SIZE, FILE, LINE)			bkHeapAlloc(SIZE, FILE, LINE, BALLOC_MALLOC | fEffectsModule)
		#define CALLOC(SIZE, VALUE)					bkHeapCalloc(SIZE, VALUE, __FILE__, __LINE__, BALLOC_MALLOC | fEffectsModule)
		#define CALLOCSRC(SIZE, VALUE, FILE, LINE)	bkHeapCalloc(SIZE, VALUE, FILE, LINE, BALLOC_MALLOC | fEffectsModule)
		#define new									new(__FILE__, __LINE__, BALLOC_NEW | fEffectsModule)
	#else
		#pragma message("Warning: Unknown Babel Module! allocations in this module will be defined as external.\n")
		#define MALLOC(SIZE)						bkHeapAlloc(SIZE, __FILE__, __LINE__, BALLOC_MALLOC | bExternalModule)
		#define MALLOCSRC(SIZE, FILE, LINE)			bkHeapAlloc(SIZE, FILE, LINE, BALLOC_MALLOC | bExternalModule)
		#define CALLOC(SIZE, VALUE)					bkHeapCalloc(SIZE, VALUE, __FILE__, __LINE__, BALLOC_MALLOC | bExternalModule)
		#define CALLOCSRC(SIZE, VALUE, FILE, LINE)	bkHeapCalloc(SIZE, VALUE, FILE, LINE, BALLOC_MALLOC | bExternalModule)
		#define new									new(__FILE__, __LINE__, BALLOC_NEW | bExternalModule)
	#endif
#else // !__BABEL__
	#define MALLOC(SIZE)						bkHeapAlloc(SIZE, __FILE__, __LINE__, BALLOC_MALLOC | bUserModule)
	#define MALLOCALIGNED(SIZE, ALIGNMENT)		bkHeapAllocEx(SIZE, __FILE__, __LINE__, BALLOC_MALLOC | bUserModule, bGetCurrentGroup(), ALIGNMENT)
	#define MALLOCSRC(SIZE, FILE, LINE)			bkHeapAlloc(SIZE, FILE, LINE, BALLOC_MALLOC | bUserModule)
	#define CALLOC(SIZE, VALUE)					bkHeapCalloc(SIZE, VALUE, __FILE__, __LINE__, BALLOC_MALLOC | bUserModule)
	#define CALLOCSRC(SIZE, VALUE, FILE, LINE)	bkHeapCalloc(SIZE, VALUE, FILE, LINE, BALLOC_MALLOC | bUserModule)
	#define new									new(__FILE__, __LINE__, BALLOC_NEW | bUserModule)
#endif // !__BABEL__


//#endif		// __BKERNEL_HEAP_NEW_H__