// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcxbLights.cpp
//   Purpose : platform specific lighting functions
// Component : Xbox Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bInitLightsPlatformSpecific
	Purpose : initialise lights platform specific
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/
int bInitLightsPlatformSpecific()
{
    return OK; // empty, confirmed
}

/*	--------------------------------------------------------------------------------
	Function : bShutdownLightsPlatformSpecific
	Purpose : shutdown lights platform specific
	Parameters : 
	Returns : 
	Info : 
*/
void bShutdownLightsPlatformSpecific()
{
    return; // maybe empty or even not called, confirmed by taz.exe and xbox babel
}

/*	--------------------------------------------------------------------------------
	Function : bInitSpecLightsource
	Purpose : initialise spec lightsource
	Parameters : lightPtr
	Returns : 
	Info : 
*/
void bInitSpecLightsource(struct _TBLightsource *lightPtr)
{
        bkPrintf("*** WARNING *** bInitSpecLightsource was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bSetSpecLightInfo
	Purpose : set spec light info
	Parameters : lightPtr
	Returns : 
	Info : 
*/
void bSetSpecLightInfo(struct _TBLightsource *lightPtr)
{
        bkPrintf("*** WARNING *** bSetSpecLightInfo was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bEnableSpecLight
	Purpose : enable spec light
	Parameters : lightPtr
	Returns : 
	Info : 
*/
void bEnableSpecLight(struct _TBLightsource *lightPtr)
{
        bkPrintf("*** WARNING *** bEnableSpecLight was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bSetAmbientSpecLight
	Purpose : set ambient spec light
	Parameters : 
	Returns : 
	Info : 
*/
void bSetAmbientSpecLight()
{
        bkPrintf("*** WARNING *** bSetAmbientSpecLight was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bUpdateLightColour
	Purpose : update light colour
	Parameters : light
	Returns : 
	Info : 
*/
void bUpdateLightColour(struct _TBLightsource *light)
{
        bkPrintf("*** WARNING *** bUpdateLightColour was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bRestoreLights
	Purpose : restore lights
	Parameters : 
	Returns : 
	Info : 
*/
void bRestoreLights()
{
        bkPrintf("*** WARNING *** bRestoreLights was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}
