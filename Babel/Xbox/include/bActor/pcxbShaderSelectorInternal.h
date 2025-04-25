// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000-2001 Blitz Games Ltd.
//
//      File : pcxbShaderSelectorInternal.h
//   Purpose : internal information shared between the platform-specific and generic shader selector files
// Component : PC/XBox Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __PCXBSHADERSELECTORINTERNAL_H__
#define __PCXBSHADERSELECTORINTERNAL_H__

// ********************************************************************************
// Constants and Macros

#define ACTORDESC_COMPARESIZE			((unsigned int)(&((TBShaderActorDesc *)NULL)->type))
#define LIGHTINGDESC_FULLCOMPARESIZE	((unsigned int)(&((TBShaderLightingDesc *)NULL)->modeList))
#define CODEFRAGMENT_MAXPARAMS			8 // maximum number of parameters allowed to be passed to a code fragment. Over 9 and you'll have to rewrite the concatenation function.
#define OPCODE_LIMIT					128 // maximum number of opcodes per shader
#define SHADER_DATA_MAX					64 // maximum number of pieces of data registered per shader.
#define MAX_FRAGMENTS					32 // maximum number of fragments per shader.
#define RECIP255						0.00392156862745f // 1 / 255


// ********************************************************************************
// Types, Structures and Classes


typedef struct _TBShaderFragmentList
{
	uint32								fragmentCount;					// 0 - MAX_FRAGMENTS.
	CBShaderFragment					*fragment[MAX_FRAGMENTS];		// the classes representing the construction of the shader.
	uint32								startingReg[MAX_FRAGMENTS];		// the constant register that the above data starts off from if it uses constant registers.

	uint32								AppendFragment(const struct _TBShaderActorDesc * const actor, CBShaderFragment * const newFragment);
} TBShaderFragmentList;


typedef struct _TBShaderActorDesc
{
	int									boneCount;						// the number of bones that this shader supports
	int									renderMode;						// the render mode of the actor.
	int									isDrawingActor : 1;				// does this shader actually rasterize the model, or something BASED on the model?
	int									isCartoonLit : 1;				// does this shader cartoon light the model?
	int									isOutlined : 1;					// does this shader outline the model?
	int									isShell : 1;					// does this shader put a shell around the model?
	int									isZOnly : 1;					// does this shader fill in the Z Buffer, but not draw the polygons?
	int									isSphereMap : 1;				// does this shader use a sphere map on the second texture stage?
	int									isShadow : 1;					// does this shader generate a shadow?
	int									: 0;							// pad to the next 32 bit boundary.	

	int									isCustom;						// does this shader use a custom shader? (need 32bits as a pointer is used to make this unique to each shader)

	// TW: These must not be compared to the 'current' descriptions in the evaluation functions. insert such data after 'type'
	ulong								type;							// the handle for the shader.
	TBShaderFragmentList				fragmentList;
} TBShaderActorDesc;


// ********************************************************************************
// Globals

extern TBShaderLightingDesc			*vtxShaderCache;					// the global pointer to the lighting states (and via them the actor modes)
extern TBShaderLightingDesc			*currentLightingState;				// the lighting state that represents the current state of the system.
extern TBShaderActorDesc			*currentMode;						// the current shader.
extern int							vtxShaderLightStateCount;			// the current number of lighting states filled in.
extern int							vtxShaderLightStateMax;				// maxmimum number of lighting states
extern int							vtxShaderModeMax;					// maximum number of shader modes per lighting state.
extern char							codeBuffer[4096];					// 4k buffer for the shader compiler.
extern int							directionalLightNum;				// the number of directional lights processed so far.
extern int							pointLightNum;						// the number of point lights processed so far.
extern int							spotLightNum;						// the number of spot lights processed so far.
extern CBShaderFragment				*codeFragments[bcfCount];



/*	--------------------------------------------------------------------------------
	Function : bInitShaderSelectorPlatformSpecific
	Purpose : perform platform specific shader selector init.
	Parameters : 
	Returns : 
	Info : 
*/

void bInitShaderSelectorPlatformSpecific(void);


/*	--------------------------------------------------------------------------------
	Function : bShutdownShaderSelectorPlatformSpecific
	Purpose : 
	Parameters : 
	Returns : 
	Info : 
*/

void bShutdownShaderSelectorPlatformSpecific(void);


#endif /* __PCXBSHADERSELECTORINTERNAL_H__ */