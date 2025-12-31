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
	TBLightsource *inactiveFirst;
	TBLightsource *activeFirst;
	int id;

	inactiveFirst = bInactiveLights.next;
	activeFirst = bActiveLights.next;

	id = 0;
	do
	{
		TBLightsource *p = inactiveFirst;
		while (p != &bInactiveLights)
		{
			if (p->specInfo.id == id)
				goto used_id;
			p = p->next;
		}

		p = activeFirst;
		if (p == &bActiveLights)
		{
			lightPtr->specInfo.id = id;
			return;
		}

		while (p != &bActiveLights)
		{
			if (p->specInfo.id == id)
				goto used_id;
			p = p->next;
		}

		lightPtr->specInfo.id = id;
		return;

used_id:
		id = id + 1;
	}
	while (id < 1000);

	lightPtr->specInfo.id = id;
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
    D3DLIGHT8 lightDesc;
	EBLightsourceTypes lightType;

    memset(&lightDesc, 0, sizeof(D3DLIGHT8));

    // Diffuse color from lightPtr->colour[0..2]
    lightDesc.Diffuse.r = bRGBNormLUT[lightPtr->colour[0]];
    lightDesc.Diffuse.g = bRGBNormLUT[lightPtr->colour[1]];
    lightDesc.Diffuse.b = bRGBNormLUT[lightPtr->colour[2]];
    lightDesc.Diffuse.a = 1.0f;

    // Specular enabled flag
    if (lightPtr->flags & BLIGHTFLAG_SPECULAR)
    {
        lightDesc.Specular.r = bRGBNormLUT[lightPtr->colour[0]];
        lightDesc.Specular.g = bRGBNormLUT[lightPtr->colour[1]];
        lightDesc.Specular.b = bRGBNormLUT[lightPtr->colour[2]];
        lightDesc.Specular.a = 1.0f;
    }
    else
    {
        lightDesc.Specular.r = 0.0f;
        lightDesc.Specular.g = 0.0f;
        lightDesc.Specular.b = 0.0f;
        lightDesc.Specular.a = 0.0f;
    }

    lightType = lightPtr->type;

    if (lightType == BLIGHTTYPE_DIRECTIONAL)
    {
        // Directional light
        lightDesc.Type = D3DLIGHT_DIRECTIONAL;
        lightDesc.Direction.x = -(lightPtr->directional.direction[0]);
        lightDesc.Direction.y = -(lightPtr->directional.direction[1]);
        lightDesc.Direction.z = -(lightPtr->directional.direction[2]);
    }
    else if (lightType == BLIGHTTYPE_POINT)
    {
        // Point light
        lightDesc.Position.x =  lightPtr->directional.direction[0];
        lightDesc.Position.y =  lightPtr->directional.direction[1];
        lightDesc.Position.z =  lightPtr->directional.direction[2];

        lightDesc.Attenuation1 = lightPtr->directional.unitDirection[0];

        lightDesc.Type    = D3DLIGHT_POINT;
        lightDesc.Range   = 1000000.0f;
        lightDesc.Falloff = 1.0f;
    }
    else if (lightType == BLIGHTTYPE_SPOT)
    {
        // Spot light
        lightDesc.Position.x =  lightPtr->directional.direction[0];
        lightDesc.Position.y =  lightPtr->directional.direction[1];
        lightDesc.Position.z =  lightPtr->directional.direction[2];

        lightDesc.Direction.x = lightPtr->directional.unitDirection[0];
        lightDesc.Direction.y = lightPtr->directional.unitDirection[1];
        lightDesc.Direction.z = lightPtr->directional.unitDirection[2];

        lightDesc.Attenuation1 = lightPtr->directional.pad[4];
        lightDesc.Phi          = lightPtr->directional.pad[5];

        lightDesc.Type    = D3DLIGHT_SPOT;
        lightDesc.Range   = 1000000.0f;
        lightDesc.Falloff = 1.0f;
    }
    else
    {
        // Unknown type: do nothing
        return;
    }

    bDisplayInfo.d3dDevice->SetLight(lightPtr->specInfo.id, &lightDesc);
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
	bDisplayInfo.d3dDevice->LightEnable(
		lightPtr->specInfo.id,
		(lightPtr->flags & BLIGHTFLAG_ENABLED) ? TRUE : FALSE
	);
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
	int r, g, b;
	uint32 rr, gg, bb;
	uint32 color;

	r = bAmbientLight[0];
	if (r > 0x7f)
		rr = 0xff;
	else
		rr = (uint32)(r + r);

	g = bAmbientLight[1];
	gg = (uint32)(g + g);
	if (g > 0x7f)
		gg = 0xff;

	b = bAmbientLight[2];
	bb = (uint32)(b + b);
	if (b > 0x7f)
		bb = 0xff;

	color  = rr | 0xffffff00u;
	color <<= 8;
	color |= gg;
	color <<= 8;
	color |= bb;

	bDisplayInfo.d3dDevice->SetRenderState((D3DRENDERSTATETYPE)0x8b, (DWORD)color);
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
    light->colour[0] <<= 1;
    light->colour[1] <<= 1;
    light->colour[2] <<= 1;

    bSetSpecLightInfo(light);
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
    TBLightsource *lightPtr;

    // Active lights: update D3D light desc + enable/disable in device
    for (lightPtr = bActiveLights.next; lightPtr != &bActiveLights; lightPtr = lightPtr->next)
    {
        bSetSpecLightInfo(lightPtr);
        bDisplayInfo.d3dDevice->LightEnable(lightPtr->specInfo.id, lightPtr->flags & BLIGHTFLAG_ENABLED);
    }

    // Inactive lights: just update D3D light desc, leave them disabled
    for (lightPtr = bInactiveLights.next; lightPtr != &bInactiveLights; lightPtr = lightPtr->next)
    {
        bSetSpecLightInfo(lightPtr);
    }
}
