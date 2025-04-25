// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : lights.cpp
//   Purpose : lighting code
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "camera.h"
#include "lights.h"
#include "TextFX.h"			// PP: for COLOUR struct		// PP: Text effects, eg. button glyphs, coloured text
#include "VideoFX.h"		// PP: Video effects - motion blur, defocus, whatever.  Top-of-the-Pops-TASTIC, Mate!
#include "shadow.h"			// PP: shadow stuff

// PP: the normal cartoon light vector we use{1.0f, -0.3f, -1.0f, 1.0f};

TBVector						normalCartoonLightVec BALIGN16 = {1.0f, 0.3f, -1.0f, 1.0f};

TBLightsource					cartoonLight BALIGN16;
TBVector						cartoonLightVec;
TBActRenderMode_CartoonLit		cartoonParms BALIGN16;
TBActRenderMode_Outlined		lineParms BALIGN16;
TBActRenderMode_Outlined		redLineParms BALIGN16;
TBActRenderMode_Outlined		invisLineParms BALIGN16;

int	lighting = FALSE;

/* --------------------------------------------------------------------------------
   Function : InitCartoonLight
   Purpose : Setup the cartoon light system
   Parameters : 
   Returns : 
   Info : 
*/
void InitCartoonLight(char *textureRamp)
{
	// PP: use the normal cartoon light vector to start with
	bmVectorCopy(cartoonLightVec, normalCartoonLightVec);

	baCreateLight(&cartoonLight,0);

	baSetDirectionalLight(&cartoonLight, cartoonLightVec);
	// PP: TEMP BABEL WORKAROUND - fixes light direction problem YAY!!!!
#if BPLATFORM==XBOX
	cartoonLight.directional.unitDirection[W]=1.0f;
#endif

	baEnableLight(&cartoonLight,TRUE);
	baSetLightColour(&cartoonLight,60,40,20);

	// PP: NOTE: this has no effect on cel-shaded stuff
	baSetAmbientLight(127,127,127); 

	cartoonParms.rampTexture = bkLoadTexture(NULL, textureRamp, 0);
	cartoonParms.rampNumber = (int)LIGHTS_DEFAULT_RAMP;				// PP: that's the same white & grey one we've always used
	cartoonParms.lightsource = &cartoonLight;

/*	cartoonParms.lineThickness = 3.5f;
	cartoonParms.rLine = 0;
	cartoonParms.gLine = 0;
	cartoonParms.bLine = 0;
	cartoonParms.aLine = 255;
*/
	lineParms.thickness = 3.5f;
	lineParms.r = 0;
	lineParms.g = 0;
	lineParms.b = 0;
	lineParms.a = 255;

	redLineParms.thickness = 3.5f;
	redLineParms.r = 30;
	redLineParms.g = 30;
	redLineParms.b = 255;
	redLineParms.a = 255;

	invisLineParms.thickness = 3.5f;
	invisLineParms.r = 128;
	invisLineParms.g = 128;
	invisLineParms.b = 128;
	invisLineParms.a = 128;
}

/* --------------------------------------------------------------------------------
   Function : UpdateCartoonLight
   Purpose : Change the cartoon light vector to match the view vector but offset by PI/8
   Parameters : CAMERAINFO
   Returns : 
   Info : 
*/
void UpdateCartoonLight(CAMERAINFO *cameraInfo)
{
	return;		// TP: Disable cartoon light rotate with camera
	bmVectorSub(cartoonLightVec,cameraInfo->pos,cameraInfo->lookAt);
	bmVectorRotateY(cartoonLightVec,cartoonLightVec,PI/6.0f);
	baSetDirectionalLight(&cartoonLight, cartoonLightVec);
	// PP: TEMP BABEL WORKAROUND - fixes light direction problem YAY!!!!
#if BPLATFORM==XBOX
	cartoonLight.directional.unitDirection[W]=1.0f;
#endif
}


/* --------------------------------------------------------------------------------
   Function		: UpdateCartoonLight
   Purpose		: set the direction and colour of the cartoon light
   Parameters	: direction vector of the light, colour of the light
   Returns		: 
   Info			: // PP: colour parameter has no effect yet, unfortunately
					// PP: This should not be called before InitCartoonLight
*/
void UpdateCartoonLight(const TBVector dir, const COLOUR& col)
{
	// PP: set the new direction vector for the light
	bmVectorCopy(cartoonLightVec, dir);

	// PP: set the new colour for the light
	cartoonLight.colour[0] = col.r;
	cartoonLight.colour[1] = col.g;
	cartoonLight.colour[2] = col.b;

	baSetDirectionalLight(&cartoonLight, cartoonLightVec);
	// PP: TEMP BABEL WORKAROUND - fixes light direction problem YAY!!!!
#if BPLATFORM==XBOX
	cartoonLight.directional.unitDirection[W]=1.0f;
#endif
}

/* --------------------------------------------------------------------------------
   Function		: SetLightRamp
   Purpose		: set the ramp to use for the cartoon shading
   Parameters	: index of the ramp row to use for the cartoon shading (RAMP_)
   Returns		: 
   Info			: // PP: This should not be called before InitCartoonLight
					// PP: change takes effect when EnableActorCartoonRenderMode is called
*/
void SetLightRamp(const LightRamp ramp)
{
	cartoonParms.rampNumber = (int)ramp;
}

/* --------------------------------------------------------------------------------
   Function : DestroyCartoonLight
   Purpose : Destroys the cartoon light system
   Parameters : 
   Returns : 
   Info : 
*/
void DestroyCartoonLight(void)
{
	baDeleteLight(&cartoonLight);
}
