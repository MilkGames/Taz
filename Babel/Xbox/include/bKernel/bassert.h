// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcFile.cpp
//   Purpose : file access functions
// Component : Generic Kernel
//      Info : In release mode the assert code is commented out using 0(...)
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BASSERT_H__
#define __BASSERT_H__

// ********************************************************************************
// Constants and Macros

// Error values (Need to be available for release compilation even though not finally used)
enum
{
	BASSERT_GENERAL = -1,	// General assertions
	BASSERT_OK = 0,			// No assertion failure

	// BSOFTBREAK triggered
	BASSERT_SOFTBREAK,

	// Specific input assertions
	BASSERT_INPUT_CHANNEL_NAME_TOO_LONG,

	// Specific file assertions
	BASSERT_FILE_SEARCHPATHS_TOO_MANY,
	BASSERT_FILE_READ_ERROR,
	BASSERT_FILE_WRITE_ERROR,
	BASSERT_FILE_SEEK_ERROR,
	BASSERT_FILE_ALLOCATED_DATAAREA_TOO_SMALL,				// read area is too small for requested file

	// Specific matrix assertions
	BASSERT_MATRIX_SRCANDDST_MATCH,							// src = dest where not permitted

	// Specific quaternion assertions
	BASSERT_QUATERNION_SRCANDDST_MATCH,						// src = dest where not permitted

	// Specific primitive assertions
	BASSERT_PRIMITIVE_VERTICES_INVALID,						// invalid multiple

	// Specific texture assertions
	BASSERT_TEXTURE_STAGE_INVALID,
	BASSERT_TEXTURE_ALIGNED,

	// Specific texture matrix assertions
	BASSERT_TEXTUREMATRIX_TYPE_INVALID,						// texture matrix type is not valid
	BASSERT_TEXTUREMATRIX_NOTENABLED,						// texture matrix is not enabled for this stage by babel - maybe user enabled though
	BASSERT_TEXTUREMATRIX_INUSEALREADY,						// attempting to use texture matrix allready in use

	// Specific actor assertions
	BASSERT_ACTOR_BONE_COUNT_INVALID,						// Not enought bones (<=0)
	BASSERT_ACTOR_BONE_COUNT_TOOLARGE,						// Too many bones 
	BASSERT_ACTOR_BONE_ID_OUTOFRANGE,						// Bone id out of valid range
	BASSERT_ACTOR_MATRIX_LIST_TOO_LARGE,					// To many matrices for hardware
	BASSERT_ACTOR_DISPLAYLIST_ALIGNMENT,					// Misaligned display list
	BASSERT_ACTOR_RENDERMODE_UNSUPPORTED,					// Unsupported render setup
	BASSERT_ACTOR_NOVERTEXDATA,								// No vertex data available for drawing
	BASSERT_ACTOR_NOANIMDATA,								// No animation data available

	// Specific shader assertions
	BASSERT_SHADER_INVALID,									// Invalid shader setup

	// Specific font assertions
	BASSERT_FONT_GLYPH_MISSING,
	BASSERT_FONT_SPRINTF_FORMATING_INVALID,

	// Specific background loader assertions
	BASSERT_BKGLOAD_NOFILENAME_OR_HANDLE,					// No filename or handle specified
	BASSERT_BKGLOAD_FILEHANDLE_INVALID,						// File handle not valid
	BASSERT_BKGLOAD_OFFSET_NOT_ALIGNED,						// Must be 2K aligned in package
	BASSERT_BKGLOAD_FLAGS_INVALID,							// The flags settings are invalid
	BASSERT_BKGLOAD_INSUFFICIENTDATA,						// Not enough data to load
	BASSERT_BKGLOAD_READADDRESS_NULL,						// The read address is not malloced

	// Specific cache assertions
	BASSERT_CACHE_FILE_DELETE_FAILED,						// Cannot delete a file in the cache
	BASSERT_CACHE_CRC_COLLISION,							// CRC conflict detected

	// Specific sound assertions
	BASSERT_SOUND_AUDIOSTREAM_OFFSET_NOT_ALIGNED,
	BASSERT_SOUND_AUDIOSTREAM_VOLUME_OUT_OF_RANGE,			// Volume to high or low
	BASSERT_SOUND_AUDIOSTREAM_PAN_OUT_OF_RANGE,				// Pan to high or low
	BASSERT_SOUND_AUDIOSTREAM_FREQ_OUT_OF_RANGE,			// Frequency to high or low
	BASSERT_SOUND_AUDIOSTREAM_TO_MANY_STREAMS,				// Record of audio streams is corrupt
	BASSERT_SOUND_AUDIOSTREAM_FORMAT_INCORRECT,				// Wrong function called for audio stream format
	BASSERT_SOUND_AUDIOSTREAM_DIRECTX_FAILURE,				// DirectX call failed
	BASSERT_SOUND_AUDIOSTREAM_BACKGROUND_LOAD_INCOMPLETE,	// Background loading not complete by time decode requested
	BASSERT_SOUND_AUDIOSTREAM_MEMORY_INVALID,				// Memory structure is not malloced
	BASSERT_SOUND_CHANNEL_BUSY,								// trying to play on a channel that is already playing

	// Playlist assertions
	BASSERT_SOUND_PLAYLIST_INVALID_POINTER,					// Main playlist pointer not valid
	BASSERT_SOUND_PLAYLIST_SONG_OUTOFRANGE,					// Chosen song index out of range

	// CBShadowProjector assertions
	BASSERT_CBSHADOWPROJECTOR_INVALID_STAGE,				// shadow style used on invalid texture stage

	// rumble controller assertions
	BASSERT_RUMBLECTRL_EFFECTSFULL,							// the effect list is full but the effect number is not maxxed out
	BASSERT_RUMBLECTRL_PARAM_INVALID,						// the attack/sustain values are too high/low

	// render assertions
	BASSERT_RENDER_RSSTACKOVERFLOW,							// a render state stack has overflowed
	BASSERT_RENDER_RSSTACKUNDERFLOW,						// a render state stack has underflowed
	BASSERT_RENDER_RSLOCKUNDERFLOW,							// a render state lock count has underflowed
	BASSERT_RENDER_RSALPHABLEND_INVALID,					// invalid alpha blend renderstate
	BASSERT_RENDER_RSBACKFACECULL_INVALID,					// invalid backface culling renderstate
	BASSERT_RENDER_RSZTEST_INVALID,							// invalid z test renderstate
	BASSERT_RENDER_RSTEXTUREFILTER_INVALID,					// invalid texture filter renderstate
	BASSERT_RENDER_NOALPHA_AVAILABLE,						// the frame buffer contains no alpha data
	BASSERT_RENDER_MIPBIAS_INVALID,							// the mip bias value is invalid
	BASSERT_RENDERTARGET_PARAM_INVALID,						// invalid parameters to the render target function

	// Specific math function input assertions
	BASSERT_MATH_INVALID_INPUT,								// invalid asin/acos input range

	// Specific physics function input assertions
	BASSERT_PHYSIC_INVALID_BODYTYPE,						// invalid body type for moi function

	// Utility functions
	BASSERT_HSVTORGB_FAILED,								// function failed - case missing from switch
};


#define BASSERT_ALWAYS						0			// Assertion check that aways fails
#define BASSERT_MAX_STRING_LEN				512

// Debugging code (only used if we are in debug mode)
#if defined _DEBUG || defined BASSERTIONS
	#define	BASSERT_ACTIVE					1

	#define BASSERT							bkAssert	// errno, assert, message
	#define BASSERT_RET(e,a,m)				bkAssert(e,a,m); if (!(a)) return;
	#define BASSERT_RET0(e,a,m)				bkAssert(e,a,m); if (!(a)) return 0;

	// Testing the error status (used in test code)
	#define	BASSERT_ERROR_CLEAR()			(bError = 0)
	#define BASSERT_ERROR_CHECK()			(bError)

	#define	BASSERT_BREAK_DISABLE()			(bBreakDisable = 1)
	#define	BASSERT_BREAK_ENABLE()			(bBreakDisable = 0)
	#define	BASSERT_BREAK_STATUS()			(!bBreakDisable)

	#define	BASSERT_SHOW_MULTIPLE_ON()		(bShowMultiple = 1)
	#define	BASSERT_SHOW_MULTIPLE_OFF()		(bShowMultiple = 0)
	#define	BASSERT_SHOW_MULTIPLE_STATUS()	(bShowMultiple)
#else
	// Remove all assertion checks in non debug mode
	// NOTE: Contents of assert check still need to be available
	#define BASSERT							BNULLFUNCTION
	#define BASSERT_RET(e,a,m)
	#define BASSERT_RET0(e,a,m)

	// Testing the error status - Not available in release mode so always passes
	#define	BASSERT_ERROR_CLEAR()
	#define BASSERT_ERROR_CHECK()			0

	#define	BASSERT_BREAK_DISABLE()		
	#define	BASSERT_BREAK_ENABLE()
	#define	BASSERT_BREAK_STATUS()			1

	#define	BASSERT_SHOW_MULTIPLE_ON()
	#define	BASSERT_SHOW_MULTIPLE_OFF()
	#define	BASSERT_SHOW_MULTIPLE_STATUS()	0
#endif


// ********************************************************************************
// Globals

#if defined _DEBUG || defined BASSERTIONS
extern int bError;	// (header) Set when a debug assert fails in debug mode (_DEBUG flag set)
extern int bBreakDisable;		// For tests - SOFTBREAK break = record error only
extern int bShowMultiple;		// Show multiple asserts - more verbose output
#endif


// ********************************************************************************
// Prototypes

#if defined _DEBUG || defined BASSERTIONS
/* --------------------------------------------------------------------------------
   Function : bkAssert
   Purpose : assert function only used in debug mode (_DEBUG flag set)
   Parameters : error number, code to assert is true, output text
   Returns : 
   Info : Use the BASSERT macro so this is #defined out in release mode
		  Records and displays message if any assertion checks fail	
*/
extern void bkAssert(int errorno, int test, char *format, ...);

#endif

#endif // __BASSERT_H__
