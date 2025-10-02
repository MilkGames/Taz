// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : lights.h
//   Purpose : generic lighting functions
// Component : Generic Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BACTOR_LIGHTS_H__
#define __BACTOR_LIGHTS_H__

// ********************************************************************************
// Constants and Macros

// lightsource types
enum EBLightsourceTypes {
	BLIGHTTYPE_INVALID,

	BLIGHTTYPE_DIRECTIONAL,
	BLIGHTTYPE_POINT,
	BLIGHTTYPE_SPOT,

	BLIGHTTYPE_FORCE32 = 0x7fffffff
}; 


// lightsource flags
#define BLIGHTFLAG_ENABLED		0x00000001				// set if the lightsource is currently enabled
#define BLIGHTFLAG_DYNAMIC		0x00000002				// light structure has been dynamically allocated
#define BLIGHTFLAG_SPECULAR		0x00000004				// light generates specular highlights


// ********************************************************************************
// Types, Structures and Classes

// parameters for a directional lightsource
typedef struct _TBLightParmsDirectional {
	TBVector				direction;					// direction towards light in world space
	TBVector				unitDirection;				// normalised direction towards light in world space

	float					pad[8];
} TBLightParmsDirectional BALIGN16;


// parameters for a point lightsource
typedef struct _TBLightParmsPoint {
	TBVector				position;					// position of light in world space

	float					attenuation;				// attenuationf factor
	float					padf[3];

	float					pad[8];
} TBLightParmsPoint BALIGN16;


// parameters for a spot lightsource
typedef struct _TBLightParmsSpot {
	TBVector				position;					// position of light in world space
	TBVector				direction;					// direction light is shining in
	TBVector				unitDirection;				// direction light is shining in (unit vector)

	float					attenuation;				// attenuation factor
	float					outerCone;					// angle of outer cone
	float					outerConeCos;				// cos(outerCone/2)
	int32					pad1;
} TBLightParmsSpot BALIGN16;


// a lightsource
typedef struct _TBLightsource {
	EBLightsourceTypes			type;					// type of lightsource (see BLIGHTTYPE_)
	struct _TBLightsource		*next, *prev;			// list links
	float						specularIntensity;		// intensity of specular light

	uint32						flags;					// flags (see BLIGHTFLAG_)
	int32						colour[3];				// light colour

	TBLightsourceSpecInfo		specInfo;				// platform specific info

	// parameters
	union {
		TBLightParmsDirectional	directional;			// parameters for directional lights
		TBLightParmsPoint		point;					// parameters for point lights
		TBLightParmsSpot		spot;					// parameters for spot lights
	};
} TBLightsource  BALIGN16;


// ********************************************************************************
// Globals

extern TBLightsource		bActiveLights, bInactiveLights;	// list of active/inactive lightsources
extern int					bAmbientLight[3];				// current ambient light level
extern int					bNoofActiveLights;				// current number of active lights
extern int					bMaxActiveLights;				// max number of simultaneously active lightsources

// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : bInitLights
	Purpose : initialise the lights sub-module
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/

int bInitLights();


/*	--------------------------------------------------------------------------------
	Function : bShutdownLights
	Purpose : shutdown the lights sub-module
	Parameters : 
	Returns : 
	Info : 
*/

void bShutdownLights();


/*	--------------------------------------------------------------------------------
	Function : baCreateLight
	Purpose : create a new lightsource
	Parameters : light ptr to fill (or NULL), flags
	Returns : ptr to new light 
	Info : 
*/

TBLightsource *baCreateLight(TBLightsource *lightPtr, uint32 flags);


/*	--------------------------------------------------------------------------------
	Function : baDeleteLight
	Purpose : delete a lightsource
	Parameters : light ptr to delete
	Returns : ptr to new light 
	Info : 
*/

void baDeleteLight(TBLightsource *lightPtr);


/*	--------------------------------------------------------------------------------
	Function : baSetLightFlags
	Purpose : update the flags for a lightsource
	Parameters : light ptr, new flag values, mask of flags to change
	Returns : 
	Info : 
*/

void baSetLightFlags(TBLightsource *lightPtr, uint32 flags, uint32 flagMask);


/*	--------------------------------------------------------------------------------
	Function : baSetDirectionalLight
	Purpose : setup/update a lightsource as a directional light
	Parameters : light ptr to set, direction light is coming from
	Returns : 
	Info : 
*/

void baSetDirectionalLight(TBLightsource *lightPtr, TBVector lightVec);


/*	--------------------------------------------------------------------------------
	Function : baSetPointLight
	Purpose : setup/update a lightsource as a point light
	Parameters : light ptr to set, position, attenuation factor
	Returns : 
	Info : 
*/

void baSetPointLight(TBLightsource *lightPtr, TBVector position, float attenuation);


/*	--------------------------------------------------------------------------------
	Function : baSetSpotLight
	Purpose : setup/update a lightsource as a spot light
	Parameters : light ptr to set, position, focus, attenuation factor, inner cone radians, cone angle in radians
	Returns : 
	Info : 
*/

void baSetSpotLight(TBLightsource *lightPtr, TBVector position, TBVector focus, float attenuation, float coneAngle);


/*	--------------------------------------------------------------------------------
	Function : baEnableLight
	Purpose : enable/disable a lightsource
	Parameters : light ptr to change, new state
	Returns : 
	Info : 
*/

void baEnableLight(TBLightsource *lightPtr, int newState);


/*	--------------------------------------------------------------------------------
	Function : baSetAmbientLight
	Purpose : set the ambient light level
	Parameters : red, green, blue
	Returns : 
	Info : 
*/

void baSetAmbientLight(int red, int green, int blue);


/*	--------------------------------------------------------------------------------
	Function : baSetLightColour
	Purpose : set the colour of a lightsource
	Parameters : lightsource, red, green, blue
	Returns : 
	Info : 
*/

void baSetLightColour(TBLightsource *light, int red, int green, int blue);


/*	--------------------------------------------------------------------------------
	Function : baSetSpecularIntensity
	Purpose : set the specular intensity of a lightsource
	Parameters : lightsource, intensity (0 to 1)
	Returns : 
	Info : Note the BLIGHTFLAG_SPECULAR flag must be set for this to have any effect
*/

void baSetSpecularIntensity(TBLightsource *light, float intensity);


/*	--------------------------------------------------------------------------------
	Function : baEvaluateLightAtPoint
	Purpose : Evaluate a point in 3D space with respect to the light sources in its vicinity.
	Parameters : [in] point to evaluate.
				[in] the unit-normal that we use for the reflection factor.
				[out] array representing the R, G and B components at that point.
	Returns : 
	Info : Since we are only evaluating a POINT, the normal is ignored. Treat this as a "best case" test.
*/

void baEvaluateLightAtPoint(const TBVector point, const TBVector normal, int32 colour[3]);


/*	--------------------------------------------------------------------------------
	Function : baGetNumberOfActiveLights
	Purpose : Get number of active lights
	Parameters : 
	Returns : number of active lights
	Info : 
*/

BINLINE int baGetNumberOfActiveLights()	{return bNoofActiveLights;}


#endif		// __BACTOR_LIGHTS_H__
