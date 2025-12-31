// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcxbShaderSelectorInternal.cpp
//   Purpose : platform specific shader selector internal functions
// Component : Xbox Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Globals

TBShaderLightingDesc	   *vtxShaderCache;						// the global pointer to the lighting states (and via them the actor modes)
TBShaderLightingDesc	   *currentLightingState;				// the lighting state that represents the current state of the system.
TBShaderActorDesc		   *currentMode;						// the current shader.
int							vtxShaderLightStateCount;			// the current number of lighting states filled in.
int							vtxShaderLightStateMax;				// maxmimum number of lighting states
int							vtxShaderModeMax;					// maximum number of shader modes per lighting state.
char						codeBuffer[4096];					// 4k buffer for the shader compiler.
int							directionalLightNum;				// the number of directional lights processed so far.
int							pointLightNum;						// the number of point lights processed so far.
int							spotLightNum;						// the number of spot lights processed so far.
CBShaderFragment		   *codeFragments[bcfCount];

// ********************************************************************************
// Function Implementations

uint32 TBShaderFragmentList::AppendFragment(const struct _TBShaderActorDesc * const actor, CBShaderFragment * const newFragment)
{
    const uint32 idx = fragmentCount;
    if (idx == 0x20)
        return 0xFFFFFFFF;

    fragment[idx] = newFragment;

    if (idx == 0)
    {
        startingReg[0] = (actor->boneCount > 0) ? 0x30 : 0x2;
        fragmentCount = 1;
        return startingReg[0];
    }

    const uint32 prevUsed = fragment[idx - 1]->ConstantRegistersUsed();
    startingReg[idx] = startingReg[idx - 1] + prevUsed;

    fragmentCount = idx + 1;
    return startingReg[idx];
}

/*	--------------------------------------------------------------------------------
	Function : bInitShaderSelectorPlatformSpecific
	Purpose : initialise shader selector platform specific
	Parameters : 
	Returns : 
	Info : 
*/
void bInitShaderSelectorPlatformSpecific(void)
{
    vtxShaderLightStateCount = 0;
    vtxShaderLightStateMax   = 64;
    vtxShaderModeMax         = 16;
    currentLightingState     = NULL;

    vtxShaderCache = (TBShaderLightingDesc*) MALLOCEX(vtxShaderLightStateMax * sizeof(TBShaderLightingDesc), (uint32)"Vertex Shader Cache");
    TBShaderActorDesc* modeList = (TBShaderActorDesc*) MALLOCEX(vtxShaderLightStateMax * vtxShaderModeMax * sizeof(TBShaderActorDesc), (uint32)"Vertex Shader Mode List");

    vtxShaderCache->modeList = modeList;

    for (int lightIndex = 0; lightIndex < vtxShaderLightStateMax; ++lightIndex)
    {
        TBShaderLightingDesc* lighting = &vtxShaderCache[lightIndex];

        lighting->modeList  = modeList + lightIndex * vtxShaderModeMax;
        lighting->modeCount = 0;
    }
}

/*	--------------------------------------------------------------------------------
	Function : bShutdownShaderSelectorPlatformSpecific
	Purpose : shutdown shader selector platform specific
	Parameters : 
	Returns : 
	Info : 
*/
void bShutdownShaderSelectorPlatformSpecific(void)
{
        bkPrintf("*** WARNING *** bShutdownShaderSelectorPlatformSpecific was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}
