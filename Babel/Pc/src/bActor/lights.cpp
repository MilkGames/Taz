// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : lights.cpp
//   Purpose : Defines lightsource types, flags, and structures. Declares global variables and prototypes for initializing/shutting down lights, creating/deleting lights, setting flags, configuring various light types, and evaluating light at a point
// Component : Generic Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

TBLightsource	bActiveLights, bInactiveLights;
int				bMaxActiveLights;

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bInitLights
	Purpose : initialise the lights submodule
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/
int bInitLights()
{
	if (!bInitLightsPlatformSpecific()) {
		return FAIL;
	}

	bActiveLights.prev = &bActiveLights;
	bActiveLights.next = &bActiveLights;
	bInactiveLights.prev = &bInactiveLights;
	bInactiveLights.next = &bInactiveLights;
	return OK;
}

/*	--------------------------------------------------------------------------------
	Function : bShutdownLights
	Purpose : shutdown the lights submodule
	Parameters : 
	Returns : 
	Info : 
*/
void bShutdownLights()
{
    return; // empty, confirmed
}

/*	--------------------------------------------------------------------------------
	Function : baCreateLight
	Purpose : create a light
	Parameters : light ptr, flags
	Returns : light ptr
	Info : 
*/
TBLightsource *baCreateLight(TBLightsource *lightPtr, uint32 flags)
{
        bkPrintf("*** WARNING *** baCreateLight was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}

/*	--------------------------------------------------------------------------------
	Function : baDeleteLight
	Purpose : delete a light
	Parameters : light ptr
	Returns : 
	Info : 
*/
void baDeleteLight(TBLightsource *lightPtr)
{
        bkPrintf("*** WARNING *** baDeleteLight was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baSetLightFlags
	Purpose : set light flags
	Parameters : light ptr, flags
	Returns : 
	Info : 
*/
void baSetLightFlags(TBLightsource *lightPtr, uint32 flags)
{
        bkPrintf("*** WARNING *** baSetLightFlags was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baSetDirectionalLight
	Purpose : setup/update a lightsource as a directional light
	Parameters : light ptr to set, direction light is coming from
	Returns : 
	Info : 
*/

void baSetDirectionalLight(TBLightsource *lightPtr, TBVector lightVec)
{
        bkPrintf("*** WARNING *** baSetDirectionalLight was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}



/*	--------------------------------------------------------------------------------
	Function : baSetPointLight
	Purpose : setup/update a lightsource as a point light
	Parameters : light ptr to set, position, attenuation factor
	Returns : 
	Info : 
*/

void baSetPointLight(TBLightsource *lightPtr, TBVector position, float attenuation)
{
        bkPrintf("*** WARNING *** baSetPointLight was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}



/*	--------------------------------------------------------------------------------
	Function : baSetSpotLight
	Purpose : setup/update a lightsource as a spot light
	Parameters : light ptr to set, position, focus, attenuation factor, inner cone radians, cone angle in radians
	Returns : 
	Info : 
*/

void baSetSpotLight(TBLightsource *lightPtr, TBVector position, TBVector focus, float attenuation, float coneAngle)
{
        bkPrintf("*** WARNING *** baSetSpotLight was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}



/*	--------------------------------------------------------------------------------
	Function : baEnableLight
	Purpose : enable/disable a lightsource
	Parameters : light ptr to change, new state
	Returns : 
	Info : 
*/

void baEnableLight(TBLightsource *lightPtr, int newState)
{
        bkPrintf("*** WARNING *** baEnableLight was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}



/*	--------------------------------------------------------------------------------
	Function : baSetAmbientLight
	Purpose : set the ambient light level
	Parameters : red, green, blue
	Returns : 
	Info : 
*/

void baSetAmbientLight(int red, int green, int blue)
{
        bkPrintf("*** WARNING *** baSetAmbientLight was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}



/*	--------------------------------------------------------------------------------
	Function : baSetLightColour
	Purpose : set the colour of a lightsource
	Parameters : lightsource, red, green, blue
	Returns : 
	Info : 
*/

void baSetLightColour(TBLightsource *light, int red, int green, int blue)
{
        bkPrintf("*** WARNING *** baSetLightColour was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}



/*	--------------------------------------------------------------------------------
	Function : baSetSpecularIntensity
	Purpose : set the specular intensity of a lightsource
	Parameters : lightsource, intensity (0 to 1)
	Returns : 
	Info : Note the BLIGHTFLAG_SPECULAR flag must be set for this to have any effect
*/

void baSetSpecularIntensity(TBLightsource *light, float intensity)
{
        bkPrintf("*** WARNING *** baSetSpecularIntensity was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}



/*	--------------------------------------------------------------------------------
	Function : baEvaluateLightAtPoint
	Purpose : Evaluate a point in 3D space with respect to the light sources in its vicinity.
	Parameters : [in] point to evaluate.
				[in] the unit-normal that we use for the reflection factor.
				[out] array representing the R, G and B components at that point.
	Returns : 
	Info : Since we are only evaluating a POINT, the normal is ignored. Treat this as a "best case" test.
*/

void baEvaluateLightAtPoint(const TBVector point, const TBVector normal, int32 colour[3])
{
        bkPrintf("*** WARNING *** baEvaluateLightAtPoint was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}
