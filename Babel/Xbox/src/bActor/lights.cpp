#define __BABEL__
#define _BACTOR_

#include <babel.h>
#include <bKernel/heapNew.h>


TBLightsource		bActiveLights, bInactiveLights;	// list of active/inactive lightsources
int					bAmbientLight[3];				// current ambient light level
int					bNoofActiveLights;				// current number of active lights
int					bMaxActiveLights;				// max number of simultaneously active lightsources


/*	--------------------------------------------------------------------------------
	Function : bInitLights
	Purpose : initialise the lights sub-modulef
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/

int bInitLights(){
  int result;
  
  result = bInitLightsPlatformSpecific();
  if (result != 0) {
    bActiveLights.prev = &bActiveLights;
    bActiveLights.next = &bActiveLights;
    bInactiveLights.prev = &bInactiveLights;
    bInactiveLights.next = &bInactiveLights;
  }
  return (uint)(result != 0);
}


/*	--------------------------------------------------------------------------------
	Function : bShutdownLights
	Purpose : shutdown the lights sub-module
	Parameters : 
	Returns : 
	Info : 
*/

void bShutdownLights()
{
    bShutdownLightsPlatformSpecific();
}


/*	--------------------------------------------------------------------------------
	Function : baCreateLight
	Purpose : create a new lightsource
	Parameters : light ptr to fill (or NULL), flags
	Returns : ptr to new light 
	Info : 
*/

TBLightsource *baCreateLight(TBLightsource *lightPtr, uint32 flags){
  if (lightPtr == NULL) {
    lightPtr = (TBLightsource*) MALLOC(sizeof(TBLightsource));
    if (lightPtr == NULL) {
      return NULL;
    }
    lightPtr->flags = flags | 2;
  }
  else {
    lightPtr->flags = flags;
  }
  lightPtr->type = BLIGHTTYPE_INVALID;
  lightPtr->colour[2] = 0x7f;
  lightPtr->colour[1] = 0x7f;
  lightPtr->colour[0] = 0x7f;
  lightPtr->prev = bInactiveLights.prev;
  lightPtr->next = &bInactiveLights;
  (bInactiveLights.prev)->next = lightPtr;
  bInactiveLights.prev = lightPtr;
  bInitSpecLightsource(lightPtr);
  lightPtr->specularIntensity = 0.5;
  return lightPtr;
}


/*	--------------------------------------------------------------------------------
	Function : baDeleteLight
	Purpose : delete a lightsource
	Parameters : light ptr to delete
	Returns : ptr to new light 
	Info : 
*/

void baDeleteLight(TBLightsource *lightPtr){
  baEnableLight(lightPtr,0);
  lightPtr->next->prev = lightPtr->prev;
  lightPtr->prev->next = lightPtr->next;
  if ((lightPtr->flags & 2) != 0) {
    bkHeapFree(lightPtr,"lightPtr");
  }
}


/*	--------------------------------------------------------------------------------
	Function : baSetLightFlags
	Purpose : update the flags for a lightsource
	Parameters : light ptr, new flag values, mask of flags to change
	Returns : 
	Info : 
*/

void baSetLightFlags(TBLightsource *lightPtr, uint32 flags, uint32 flagMask){
  lightPtr->flags = lightPtr->flags & ~flagMask | flags;
  bSetSpecLightInfo(lightPtr);
}


/*	--------------------------------------------------------------------------------
	Function : baSetDirectionalLight
	Purpose : setup/update a lightsource as a directional light
	Parameters : light ptr to set, direction light is coming from
	Returns : 
	Info : 
*/

void baSetDirectionalLight(TBLightsource *lightPtr, TBVector lightVec){
  lightPtr->type = BLIGHTTYPE_DIRECTIONAL;
  bmVectorCopy(lightPtr->directional.direction, lightVec);
  bmVectorNorm(lightPtr->directional.unitDirection, lightVec);
  lightPtr->directional.direction[3] = 0.0;
  lightPtr->directional.unitDirection[3] = 0.0;
  bSetSpecLightInfo(lightPtr);
  return;
}


/*	--------------------------------------------------------------------------------
	Function : baSetPointLight
	Purpose : setup/update a lightsource as a point light
	Parameters : light ptr to set, position, attenuation factor
	Returns : 
	Info : 
*/

void baSetPointLight(TBLightsource *lightPtr, TBVector position, float attenuation){
     
  lightPtr->type = BLIGHTTYPE_POINT;
  bmVectorCopy(lightPtr->point.position, position);
  lightPtr->point.attenuation = attenuation;
  bSetSpecLightInfo(lightPtr);
  return;
}


/*	--------------------------------------------------------------------------------
	Function : baSetSpotLight
	Purpose : setup/update a lightsource as a spot light
	Parameters : light ptr to set, position, focus, attenuation factor, inner cone radians, cone angle in radians
	Returns : 
	Info : 
*/

void baSetSpotLight(TBLightsource *lightPtr, TBVector position, TBVector focus, float attenuation, float coneAngle){
  float outerCos;
  
  lightPtr->type = BLIGHTTYPE_SPOT;
  bmVectorCopy(lightPtr->spot.position, position);
  bmVectorSub(lightPtr->spot.direction, focus, position);
  (lightPtr->spot).direction[3] = 0.0;
  bmVectorNorm(lightPtr->spot.unitDirection,lightPtr->spot.direction);
  (lightPtr->spot).unitDirection[3] = 0.0;
  (lightPtr->spot).attenuation = attenuation;
  (lightPtr->spot).outerCone = coneAngle;
  outerCos = bmCos(coneAngle * 0.5f );
  (lightPtr->spot).outerConeCos = outerCos;
  bSetSpecLightInfo(lightPtr);
  return;
}


/*	--------------------------------------------------------------------------------
	Function : baEnableLight
	Purpose : enable/disable a lightsource
	Parameters : light ptr to change, new state
	Returns : 
	Info : 
*/

void baEnableLight(TBLightsource *lightPtr, int newState){
  if ((((lightPtr->flags & 1) == 0) || (newState == 0)) &&
     (((lightPtr->flags & 1) != 0 || (newState != 0)))) {
    lightPtr->next->prev = lightPtr->prev;
    lightPtr->prev->next = lightPtr->next;
    if (newState == 0) {
      bNoofActiveLights = bNoofActiveLights + -1;
      lightPtr->flags = lightPtr->flags & 0xfffffffe;
      lightPtr->prev = bInactiveLights.prev;
      lightPtr->next = &bInactiveLights;
      (bInactiveLights.prev)->next = lightPtr;
      bInactiveLights.prev = lightPtr;
    }
    else {
      if (bNoofActiveLights == bMaxActiveLights) {
        bkPrintf("baEnableLight: ERROR - max of %d lights already active\n",bMaxActiveLights);
        return;
      }
      lightPtr->prev = bActiveLights.prev;
      lightPtr->next = &bActiveLights;
      (bActiveLights.prev)->next = lightPtr;
      bActiveLights.prev = lightPtr;
      lightPtr->flags = lightPtr->flags | 1;
      bNoofActiveLights = bNoofActiveLights + 1;
    }
    bEnableSpecLight(lightPtr);
  }
}


/*	--------------------------------------------------------------------------------
	Function : baSetAmbientLight
	Purpose : set the ambient light level
	Parameters : red, green, blue
	Returns : 
	Info : 
*/

void baSetAmbientLight(int red, int green, int blue){
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

void baSetLightColour(TBLightsource *light, int red, int green, int blue){
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

void baSetSpecularIntensity(TBLightsource *light, float intensity){
  light->specularIntensity = intensity;
  bSetSpecLightInfo(light);
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

void baEvaluateLightAtPoint(const TBVector point, const TBVector normal, int32 colour[3]){
// TODO

}


/*	--------------------------------------------------------------------------------
	Function : baGetNumberOfActiveLights
	Purpose : Get number of active lights
	Parameters : 
	Returns : number of active lights
	Info : 
*/

//BINLINE int baGetNumberOfActiveLights()	{return bNoofActiveLights;}

