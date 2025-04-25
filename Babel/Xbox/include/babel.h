// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : babel.h
//   Purpose : top level include file for Babel libraries
// Component : Xbox Babel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BABEL_H__
#define __BABEL_H__

// ********************************************************************************
// Additional header files

// include required system headers
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <CrtDbg.h>

#ifdef _DEBUG

	#define   malloc(s)         _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
	#define   calloc(c, s)      _calloc_dbg(c, s, _NORMAL_BLOCK, __FILE__, __LINE__)
	#define   realloc(p, s)     _realloc_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
	#define   _expand(p, s)     _expand_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
	#define   free(p)           _free_dbg(p, _NORMAL_BLOCK)
	#define   _msize(p)         _msize_dbg(p, _NORMAL_BLOCK)
	#define   _aligned_malloc(s, a)                 _aligned_malloc_dbg(s, a, __FILE__, __LINE__)
	#define   _aligned_realloc(p, s, a)             _aligned_realloc_dbg(p, s, a, __FILE__, __LINE__)
	#define   _aligned_offset_malloc(s, a, o)       _aligned_offset_malloc_dbg(s, a, o, __FILE__, __LINE__)
	#define   _aligned_offset_realloc(p, s, a, o)   _aligned_offset_realloc_dbg(p, s, a, o, __FILE__, __LINE__)
	#define   _aligned_free(p)  _aligned_free_dbg(p)

#endif

#include <xtl.h>
#include <xGraphics.h>
#include <dsstdfx.h>

//#include "c:/program files/BinkXbox/bink.h"

#ifdef _DEBUG
	#include <d3d8perf.h>
#endif


// ********************************************************************************
// Constants and Macros

// our supported platforms
#define PC			1
#define PS2			2
#define GIZMO		3
#define XBOX		4
#define GAMECUBE	5

// define our babel-wide platform symbol.  This is used to identify the platform being compiled for.
#define BPLATFORM	XBOX

// global success/failure constants
#define OK			1
#define FAIL		0

// global boolean constants
#define TRUE		1
#define FALSE		0

// define a global symbol if we are being compiled by Babel
#if	defined(_BDISPLAY_)		|| \
		defined(_BACTOR_)	|| \
		defined(_BMATHS_)	|| \
		defined(_BKERNEL_)	|| \
		defined(_BINPUT_)	|| \
		defined(_BGUI_)		|| \
		defined(_BSOUND)	|| \
		defined(_FCAMERA_)	|| \
		defined(_FGUI_)		|| \
		defined(_FSCENE_)	|| \
		defined(_FEFFECTS_)
	#define __BABEL__
#endif

// bkInit flags
#define BKINITFLAG_NONEXCLUSIVE		0x00000001			// unused by Xbox
#define BKINITFLAG_VERBOSERESOURCES	0x00000002			// use verbose resource mode
#define BKINITFLAG_MAXIMUMHEAP		0x00000004			// (consoles) allocate maximum amount possible for heap
#define BKINITFLAG_NODEBUGOUTPUT	0x00000008			// No debugger output
#define BKINITFLAG_INITCALLED		0x00000010			// Init has been called

// main function declaration
#define BDECLAREMAIN	void __cdecl main()

// main function return statement (infinite loop as we should never return)
#define BMAINRETURN		{ while (1); }

// Null function used to remove variable length functions in release mode
#define BNULLFUNCTION_RET			__noop	// __noop(stuff) removes stuff from final exe
#define BNULLFUNCTION				__noop	// __noop(stuff) removes stuff from final exe

// Xbox macro to align variables on 16byte boundaries (primarily for matrices so we can use SIMD code)
#define BXBALIGNVAR16	__declspec(align(16))

// Inline specifier macro
#define BINLINE			inline

// Fixup pointer to be offset from base
#define BFIXPTR(BASE, PTR, TYPE)  if (PTR) PTR = (TYPE*)((uint32)(BASE)+(uint32)(PTR))

// ********************************************************************************
// Types and Structures

// atomic types
typedef int					int32;
typedef __int64				int64;
typedef unsigned int		uint;
typedef unsigned int		uint32;
typedef unsigned __int64	uint64;
typedef long				long32;
typedef __int64				long64;
typedef unsigned long		ulong;
typedef unsigned long		ulong32;
typedef unsigned __int64	ulong64;
typedef unsigned short		ushort;
typedef unsigned char		uchar;


// ********************************************************************************
// Babel headers

// define the build number
#include "buildNumber.h"

// BOSS headers
#include "bKernel.h"
#include "bMaths.h"
#include "bDisplay.h"
#include "bInput.h"
#include "bActor.h"
#include "bSound.h"
#include "bGui.h"


// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bkInit
   Purpose : initialise all modules
   Parameters : heap base address (NULL for dynamic), size of heap, flags
   Returns : OK/FAIL
   Info : 
*/

int bkInit(uint base, uint size, uint32 flags);


/* --------------------------------------------------------------------------------
   Function : bkShutdown
   Purpose : shutdown all modules
   Parameters : 
   Returns : 
   Info : 
*/

void bkShutdown();


/* --------------------------------------------------------------------------------
   Function : bkUpdate
   Purpose : call the update function for each of the babel modules
   Parameters : module mask
   Returns : 
   Info : 
*/

void bkUpdate(int modules=BUPDATEMODULE_ALL);


#endif		// __BABEL_H__