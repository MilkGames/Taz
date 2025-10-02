// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcxbLights.h
//   Purpose : platform specific lighting code
// Component : PC/Xbox Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BACTOR_PCLIGHTS_H__
#define __BACTOR_PCLIGHTS_H__

// ********************************************************************************
// Constants and Macros


// ********************************************************************************
// Types, Structures and Classes

// platform specific data for lightsources
typedef struct _TBLightsourceSpecInfo {
	int32					id;						// lightsource ID
	int32					pad1, pad2, pad3;
} TBLightsourceSpecInfo;


// ********************************************************************************
// Globals



// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : bInitLightsPlatformSpecific
	Purpose : platform specific light initialisation code
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/

int bInitLightsPlatformSpecific();


/*	--------------------------------------------------------------------------------
	Function : bShutdownLightsPlatformSpecific
	Purpose : platform specific light shutdown code
	Parameters : 
	Returns : 
	Info : 
*/

void bShutdownLightsPlatformSpecific();


/*	--------------------------------------------------------------------------------
	Function : bInitSpecLightsource
	Purpose : initialise platform specific data for a lightsource
	Parameters : lightsource ptr
	Returns : 
	Info : 
*/

void bInitSpecLightsource(struct _TBLightsource *lightPtr);


/*	--------------------------------------------------------------------------------
	Function : bSetSpecLightInfo
	Purpose : setup platform specific info for a light that has changed parameters
	Parameters : lightsource ptr
	Returns : 
	Info : 
*/

void bSetSpecLightInfo(struct _TBLightsource *lightPtr);


/*	--------------------------------------------------------------------------------
	Function : bEnableSpecLight
	Purpose : update platform specific info for a light that has changed state
	Parameters : lightsource ptr
	Returns : 
	Info : 
*/

void bEnableSpecLight(struct _TBLightsource *lightPtr);


/*	--------------------------------------------------------------------------------
	Function : bSetAmbientSpecLight
	Purpose : set the ambient light level
	Parameters : 
	Returns : 
	Info : 
*/

void bSetAmbientSpecLight();


/*	--------------------------------------------------------------------------------
	Function : bUpdateLightColour
	Purpose : called when the colour of a light has changed
	Parameters : lightsource ptr
	Returns : 
	Info : 
*/

void bUpdateLightColour(struct _TBLightsource *light);


/*	--------------------------------------------------------------------------------
	Function : bRestoreLights
	Purpose : restore lightsources after focus loss
	Parameters : 
	Returns : 
	Info : 
*/

void bRestoreLights();

#endif		// __BACTOR_PCLIGHTS_H__
