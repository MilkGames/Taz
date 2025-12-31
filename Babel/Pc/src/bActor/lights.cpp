// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : lights.cpp
//   Purpose : Defines lightsource types, flags, and structures. Declares global variables and prototypes for initializing/shutting down lights, creating/deleting lights, setting flags, configuring various light types, and evaluating light at a point
// Component : Generic Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Globals

TBLightsource	bActiveLights, bInactiveLights;
int				bAmbientLight[3] = {0x1E, 0x1E, 0x1E};
int				bNoofActiveLights = 0;
int				bMaxActiveLights = 0;

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
	TBLightsource *p;

	p = lightPtr;

	if (p == NULL)
	{
		p = (_TBLightsource *)MALLOC(0x70);
		if (p == NULL)
			return NULL;

		p->flags = flags | 2;
	}
	else
	{
		p->flags = flags;
	}

	p->type = BLIGHTTYPE_INVALID;

	p->colour[2] = 0x7f;
	p->colour[1] = 0x7f;
	p->colour[0] = 0x7f;

	p->prev = bInactiveLights.prev;
	p->next = &bInactiveLights;

	bInactiveLights.prev->next = p;
	bInactiveLights.prev = p;

	bInitSpecLightsource(p);

	p->specularIntensity = 0.5f;

	return p;
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
	baEnableLight(lightPtr, 0);

	lightPtr->next->prev = lightPtr->prev;
	lightPtr->prev->next = lightPtr->next;

	if (lightPtr->flags & 0x2)
	{
		bkHeapFree(lightPtr);
	}
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
	lightPtr->type = BLIGHTTYPE_DIRECTIONAL;

	lightPtr->directional.direction[0] = lightVec[0];
	lightPtr->directional.direction[1] = lightVec[1];
	lightPtr->directional.direction[2] = lightVec[2];
	lightPtr->directional.direction[3] = lightVec[3];

	bmVectorNorm(lightPtr->directional.unitDirection, lightVec);

	lightPtr->directional.direction[3] = 0.0f;
	lightPtr->directional.unitDirection[3] = 0.0f;

	bSetSpecLightInfo(lightPtr);
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
	uint32 wasEnabled;

	wasEnabled = (lightPtr->flags & BLIGHTFLAG_ENABLED);

	if (wasEnabled && newState)
		return;

	if (!wasEnabled && !newState)
		return;

	lightPtr->next->prev = lightPtr->prev;
	lightPtr->prev->next = lightPtr->next;

	if (newState)
	{
		if (bNoofActiveLights == bMaxActiveLights)
		{
			bkPrintf("baEnableLight: ERROR - max of %d lights already active\n", bMaxActiveLights);
			return;
		}

		lightPtr->prev = bActiveLights.prev;
		lightPtr->next = &bActiveLights;
		bActiveLights.prev->next = lightPtr;
		bActiveLights.prev = lightPtr;

		lightPtr->flags |= BLIGHTFLAG_ENABLED;
		bNoofActiveLights = bNoofActiveLights + 1;

		bEnableSpecLight(lightPtr);
		return;
	}
	bNoofActiveLights = bNoofActiveLights - 1;
	lightPtr->flags &= ~BLIGHTFLAG_ENABLED;

	lightPtr->prev = bInactiveLights.prev;
	lightPtr->next = &bInactiveLights;
	bInactiveLights.prev->next = lightPtr;
	bInactiveLights.prev = lightPtr;

	bEnableSpecLight(lightPtr);
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
	bAmbientLight[0] = red;
	bAmbientLight[1] = green;
	bAmbientLight[2] = blue;

	bSetAmbientSpecLight();
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
	light->colour[0] = red;
	light->colour[1] = green;
	light->colour[2] = blue;
	bUpdateLightColour(light);
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
