// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcxbShaderSelector.h
//   Purpose : vertex shader munger header. 
// Component : PC/Xbox Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BACTOR_SHADERSELECTOR_H__
#define __BACTOR_SHADERSELECTOR_H__

// ********************************************************************************
// Constants and Macros

// macro to define a shader fragment class
#define BDEFINE_SHADERFRAGMENT(_NAME, _PARAMCOUNT, _CONSTANTREGSUSED, _CODEFRAGMENT) \
	class _NAME : public CBShaderFragment { \
	public: \
									_NAME() : CBShaderFragment(_PARAMCOUNT, _CONSTANTREGSUSED, _CODEFRAGMENT) {} \
		void						RegisterData(const struct _TBShaderContext * const context); \
		void						ActivateFragment(const struct _TBShaderContext * const context); \
	};

// extended macro to define a shader fragment class.
#define BDEFINE_SHADERFRAGMENTEX(_NAME, _PARAMCOUNT, _CONSTANTREGSUSED, _CODEFRAGMENT) \
	class _NAME : public CBShaderFragment { \
	public: \
									_NAME() : CBShaderFragment(_PARAMCOUNT, _CONSTANTREGSUSED, _CODEFRAGMENT) {}

// default shader fragments.
enum {
	bcfRaw2,			// e.g. "mov a0.x, v2.z"
	bcfRaw3,			// e.g. "mul r2, r2, v1.x"
	bcfRaw4,			// e.g. "mad r2, r1, v1.x, r2"
	bcfLoadA0,			// a0.x = <0>
	bcfBlend,			// <0> = (<0> * <1>) + (<2> * <3>)
	bcfBoneVertex,		// <0> = XForm(INPUT_POSITION, c[a0.x])
	bcfBoneNormal,		// <0> = XForm_NoTranslate(INPUT_NORMAL, c[a0.x])
	bcfCartoonShade,	// alter the "u" texture coordinate for stage 1. Param(lightVector)
	bcfOutline,			// grow the model along its normal by %1.w and recolour it to %0. Backface culling is reversed.
	bcfShell,			// alter the position and the colour of the vertex to appear as a shell of the model if drawn afterwards. This can be used for outlining by turning culling around.
	bcfZOnly,			// alter the colour (and alpha) to make the polygons appear invisible (but still written to the Z Buffer)
	bcfSphereMap,		// alter the stage1 texture coordinates using the camera-space normal to index into the sphere map texture.
	bcfAmbientLight,	// set up the lighting colour at the ambient light.
	bcfDirectionalLight,// alter the colour according to a directional light: direction: <0>, colour: <1>
	bcfPointLight,		// alter the colour according to a point light: position: <0>, colour: <1>, [maxRange, 1-falloff]: <2>
	bcfSpotLight,		// alter the colour according to a spot light: position: <0>, colour: <1>, direction: <2>, [maxRange, 1-falloff, innerCone, outerCone]: <3>
	bcfFog,				// set up the fog value according to the distance from the eye.
	bcfOutPosition,		// oPos = XForm(<0>, object->projection)
	bcfOutColour,		// oD0 = <0>
	bcfOutTex0,			// oT0 = <0>
	bcfShadow,			// render all black with alpha based on dot product
	bcfNormaliseNormal,	// normalise the normal in case the bone xforms had scale in them

	bcfCount
};


// ********************************************************************************
// Types, Structures and Classes

typedef struct _TBShaderLightingDesc
{
	int									fogEnabled;					// set if the fogging is enabled.
	int									lightingEnabled;			// set if lighting is enabled.
	int									directionalLightCount;		// number of directional light sources in this state.
	int									pointLightCount;			// number of point light sources in this state.
	int									spotLightCount;				// number of spot light sources in this state.

	// TW: Anything after this point is no compared in the evaluation functions.
	struct _TBShaderActorDesc			*modeList;					// the actor modes that this lighting description handles.
	int									modeCount;					// the number of modes that the list contains.

	uint								lastUsed;					// the time that this lighting state was last selected.
	uint								useCount;					// the number of times that the state has been used.
} TBShaderLightingDesc;


// base class for vertex shader fragments
typedef class CBShaderFragment
{
private:
	uint32								opcodeCount;
	uint32								paramCount;
	uint32								constantRegsUsed;
	const char							*codeFragment;

public:
										CBShaderFragment(const uint32 _pC, const uint32 constantRegsUsed, const char * const _cF);

	uint32								AppendFragment(struct _TBShaderActorDesc * const actor, char * const buffer, ...);

	virtual void						RegisterData(const struct _TBShaderContext * const context) {};
	virtual void						ActivateFragment(const struct _TBShaderContext * const context) {};
	virtual void						DeactivateFragment(const struct _TBShaderContext * const context) {};
	virtual uint32						ConstantRegistersUsed(void) { return constantRegsUsed; }
} CBShaderFragment;


// context information for a shader fragment
typedef struct _TBShaderContext
{
	TBActorInstance						*instance;
	uint32								renderStage;
	uint32								startingRegister;
	void								*userContext;
} TBShaderContext;


/*	--------------------------------------------------------------------------------
	Function : bInitShaderSelectorPlatformSpecific
	Purpose : shader selector init.
	Parameters : 
	Returns : 
	Info : 
*/

void bInitShaderSelector(void);


/*	--------------------------------------------------------------------------------
	Function : bShutdownShaderSelectorPlatformSpecific
	Purpose : 
	Parameters : 
	Returns : 
	Info : 
*/

void bShutdownShaderSelector(void);


/*	--------------------------------------------------------------------------------
	Function : baShaderSelect
	Purpose : Selects the appropriate shader for the actor instance that was passed.
	Parameters : [in] the instance to evaluate. (the object->world matrix may be validated)
				[in] the render mode of that instance to evaluate.
	Returns : TRUE if there will be a point to rendering,
				FALSE if there is no effect from rendering (or rendering will produce undefined results)
	Info : 
*/

int baShaderSelect(TBActorInstance * const instance, const int modeNum, TBMesh *mesh=NULL);


/*	--------------------------------------------------------------------------------
	Function : baShaderDeselect
	Purpose : Deselect the shader for the actor instance at the render mode.
	Parameters : [in] the actor instance that we're playing with
				[in] the render stage that we're on
	Returns : 
	Info : 
*/

void baShaderDeselect(TBActorInstance * const instance, const int modeNum);


/*	--------------------------------------------------------------------------------
	Function : baShaderEvaluateLightingState
	Purpose : checks the list of lighting states available and matches to the current lighting state.
				if a previous state wasn't found, create it.
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/

int baShaderEvaluateLightingState(void);


/*	--------------------------------------------------------------------------------
	Function : baShaderAddLightingState
	Purpose : Add a lighting state to the cache, and generate the shaders for the actor modes that are cached.
	Parameters : [in] the description of this mode.
	Returns : pointer to the newly cached lighting state, or NULL.
	Info : 
*/

TBShaderLightingDesc *baShaderAddLightingState(const TBShaderLightingDesc * const desc);


/*	--------------------------------------------------------------------------------
	Function : baGenerateShader
	Purpose : Generate a shader for the lighting description for the actor description.
	Parameters : [in/out] the lighting description to place the new shader into
				[in] the actor description that helps,
				actor instance,
				mode number
	Returns : pointer to the new TBShaderActorDesc or NULL on error.
	Info : 
*/

struct _TBShaderActorDesc *baGenerateShader(TBShaderLightingDesc * const lighting,
																	const struct _TBShaderActorDesc * const actorDesc,
																	TBActorInstance * const instance, const int modeNum);

#endif /* __BACTOR_SHADERSELECTOR_H__ */