#define __BABEL__
#define _BACTOR_

#include <babel.h>
/*	--------------------------------------------------------------------------------
	Function : bInitLightsPlatformSpecific
	Purpose : platform specific light initialisation code
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/

int bInitLightsPlatformSpecific(){
    return 1;
}


/*	--------------------------------------------------------------------------------
	Function : bShutdownLightsPlatformSpecific
	Purpose : platform specific light shutdown code
	Parameters : 
	Returns : 
	Info : 
*/

void bShutdownLightsPlatformSpecific(){
}


/*	--------------------------------------------------------------------------------
	Function : bInitSpecLightsource
	Purpose : initialise platform specific data for a lightsource
	Parameters : lightsource ptr
	Returns : 
	Info : 
*/

void bInitSpecLightsource(struct _TBLightsource *lightPtr){
  TBLightsource *currentLight;
  int lightId;
  
  for (lightId = 0; lightId < 1000; lightId = lightId + 1) {
    for (currentLight = bInactiveLights.next;
        (currentLight != &bInactiveLights && ((currentLight->specInfo).id != lightId));
        currentLight = currentLight->next) {
    }
    if (currentLight == &bInactiveLights) {
      for (currentLight = bActiveLights.next;
          (currentLight != &bActiveLights && ((currentLight->specInfo).id != lightId));
          currentLight = currentLight->next) {
      }
      if (currentLight == &bActiveLights) break;
    }
  }
  (lightPtr->specInfo).id = lightId;
}


/*	--------------------------------------------------------------------------------
	Function : bSetSpecLightInfo
	Purpose : setup platform specific info for a light that has changed parameters
	Parameters : lightsource ptr
	Returns : 
	Info : 
*/

void bSetSpecLightInfo(struct _TBLightsource *lightPtr){
 
  D3DLIGHT8 light;
  EBLightsourceTypes type;
    
  memset(&light,0,0x68);
  light.Diffuse.r = bRGBNormLUT[lightPtr->colour[0]];
  light.Diffuse.g = bRGBNormLUT[lightPtr->colour[1]];
  light.Diffuse.b = bRGBNormLUT[lightPtr->colour[2]];
  light.Diffuse.a = 1.0f;
  if ((lightPtr->flags & BLIGHTFLAG_SPECULAR) == 0) {
    light.Specular.r = 0.0f;
    light.Specular.g = 0.0f;
    light.Specular.b = 0.0f;
    light.Specular.a = 0.0f;
  }
  else {
    light.Specular.r = bRGBNormLUT[lightPtr->colour[0]];
    light.Specular.g = bRGBNormLUT[lightPtr->colour[1]];
    light.Specular.b = bRGBNormLUT[lightPtr->colour[2]];
    light.Specular.a = 1.0f;
  }
  type = lightPtr->type;
  if (type == BLIGHTTYPE_DIRECTIONAL) {
    light.Type = D3DLIGHT_DIRECTIONAL;
    light.Direction.x = -(lightPtr->directional).direction[0];
    light.Direction.y = -(lightPtr->directional).direction[1];
    light.Direction.z = -(lightPtr->directional).direction[2];
  }
  else if (type == BLIGHTTYPE_POINT) {
    light.Type = D3DLIGHT_POINT;
    light.Position.x = (lightPtr->point).position[0];
    light.Position.y = (lightPtr->point).position[1];
    light.Position.z = (lightPtr->point).position[2];
    light.Range = 1000000.0f;
    light.Falloff = 1.0f;
    light.Attenuation1 = (lightPtr->point).attenuation;
  }
  else {
    if (type != BLIGHTTYPE_SPOT) {
      return;
    }
    light.Type = D3DLIGHT_SPOT;
    light.Position.x = (lightPtr->spot).position[0];
    light.Position.y = (lightPtr->spot).position[1];
    light.Position.z = (lightPtr->spot).position[2];
    light.Direction.x = (lightPtr->spot).direction[0];
    light.Direction.y = (lightPtr->spot).direction[1];
    light.Direction.z = (lightPtr->spot).direction[2];
    light.Range = 1000000.0f;
    light.Falloff = 1.0f;
    light.Attenuation1 = (lightPtr->spot).attenuation;
    light.Phi = (lightPtr->spot).outerCone;
  }
  D3DDevice_SetLight((lightPtr->specInfo).id,&light);
}


/*	--------------------------------------------------------------------------------
	Function : bEnableSpecLight
	Purpose : update platform specific info for a light that has changed state
	Parameters : lightsource ptr
	Returns : 
	Info : 
*/

void bEnableSpecLight(struct _TBLightsource *lightPtr){
  D3DDevice_LightEnable((lightPtr->specInfo).id,(uint)((lightPtr->flags & 1) != 0));
}


/*	--------------------------------------------------------------------------------
	Function : bSetAmbientSpecLight
	Purpose : set the ambient light level
	Parameters : 
	Returns : 
	Info : 
*/

void __cdecl bSetAmbientSpecLight(void)

{
  uint colour [3];
  
  if (bAmbientLight[0] < 0x80) {
    colour[2] = bAmbientLight[0] << 1;
  }
  else {
    colour[2] = 0xff;
  }
  if (bAmbientLight[1] < 0x80) {
    colour[1] = bAmbientLight[1] << 1;
  }
  else {
    colour[1] = 0xff;
  }
  if (bAmbientLight[2] < 0x80) {
    colour[0] = bAmbientLight[2] << 1;
  }
  else {
    colour[0] = 0xff;
  }
  D3DDevice_SetRenderState(
      D3DRS_AMBIENT,
      // D3DRS_BACKSPECULARMATERIALSOURCE,
      colour[2] << 0x10 | 0xff000000 | colour[1] << 8 | colour[0]
  );
  return;
}


/*	--------------------------------------------------------------------------------
	Function : bUpdateLightColour
	Purpose : called when the colour of a light has changed
	Parameters : lightsource ptr
	Returns : 
	Info : 
*/

void bUpdateLightColour(struct _TBLightsource *light){
  
  light->colour[0] = light->colour[0] << 1;
  light->colour[1] = light->colour[1] << 1;
  light->colour[2] = light->colour[2] << 1;
  //               ^- whats all this about?
  bSetSpecLightInfo(light);
}

/*	--------------------------------------------------------------------------------
	Function : bRestoreLights
	Purpose : restore lightsources after focus loss
	Parameters : 
	Returns : 
	Info : 
*/

void bRestoreLights(){
  TBLightsource *current;
  
  for (current = bActiveLights.next; current != &bActiveLights; current = current->next) {
    bSetSpecLightInfo(current);
    bEnableSpecLight(current);
  }
  for (current = bInactiveLights.next; current != &bInactiveLights; current = current->next) {
    bSetSpecLightInfo(current);
  }
}
