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

/*	--------------------------------------------------------------------------------
	Function : bInitLights
	Purpose : initialise the lights submodule
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/
int bInitLights()
{
    return 0;
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
    return;
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
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baClearLightFlags
	Purpose : clear light flags
	Parameters : light ptr, flags
	Returns : 
	Info : 
*/
void baClearLightFlags(TBLightsource *lightPtr, uint32 flags)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baSetLightPosition
	Purpose : set light position
	Parameters : light ptr, position
	Returns : 
	Info : 
*/
void baSetLightPosition(TBLightsource *lightPtr, TBVector position)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baSetLightDirection
	Purpose : set light direction
	Parameters : light ptr, direction
	Returns : 
	Info : 
*/
void baSetLightDirection(TBLightsource *lightPtr, TBVector direction)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baSetLightColour
	Purpose : set light colour
	Parameters : light ptr, red, green, blue
	Returns : 
	Info : 
*/
void baSetLightColour(TBLightsource *lightPtr, int red, int green, int blue)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baSetLightRange
	Purpose : set light range
	Parameters : light ptr, range
	Returns : 
	Info : 
*/
void baSetLightRange(TBLightsource *lightPtr, float range)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baSetLightAttenuation
	Purpose : set light attenuation
	Parameters : light ptr, constant, linear, quadratic
	Returns : 
	Info : 
*/
void baSetLightAttenuation(TBLightsource *lightPtr, float constant, float linear, float quadratic)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baSetLightCone
	Purpose : set light cone
	Parameters : light ptr, inner cone, outer cone
	Returns : 
	Info : 
*/
void baSetLightCone(TBLightsource *lightPtr, float innerCone, float outerCone)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baEvalLightAtPoint
	Purpose : evaluate light at point
	Parameters : light ptr, point, colour
	Returns : 
	Info : 
*/
void baEvalLightAtPoint(TBLightsource *lightPtr, TBVector point, TBVector colour)
{
    return;
}
