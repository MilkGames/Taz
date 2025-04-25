#ifndef __H_LIGHTS
#define __H_LIGHTS


// PP: TEMP?: lighting ramps - each of these represents a different row on the ramp texture
// PP: I'm imagining it'd be nice instead to read in ramp tints from a file exported by Lust...
typedef enum _LightRamp
{
	RAMP_DARK			=   0,							// JW: Darker ramp for a slightly crispy Taz
	RAMP_TWOTONE		=	3,							// PP: The normal two-tone ramp we've been using from the start
	RAMP_SUNSET			=	5,							// PP: The sunset-coloured ramp used in the Zoo boss game
	RAMP_SUNSETSHADE	=	6,							// PP: The lowlight ramp corresponding to RAMP_SUNSET
	RAMP_SHADE			=	7,							// PP: The lowlight ramp corresponding to RAMP_TWOTONE

}LightRamp;

#define LIGHTS_DEFAULT_RAMP				RAMP_TWOTONE				// PP: The default ramp to use
										
extern TBVector							normalCartoonLightVec;		// PP: the cartoon light vector we normally use
										
extern TBLightsource					cartoonLight;
extern TBVector							cartoonLightVec;
extern TBActRenderMode_CartoonLit		cartoonParms;
extern TBActRenderMode_Outlined			lineParms;
extern TBActRenderMode_Outlined			invisLineParms;
extern TBActRenderMode_Outlined			redLineParms;

extern int lighting;

/* --------------------------------------------------------------------------------
   Function : InitCartoonLight
   Purpose : Setup the cartoon light system
   Parameters : 
   Returns : 
   Info : 
*/
void InitCartoonLight(char *rampTexture);

/* --------------------------------------------------------------------------------
   Function : UpdateCartoonLight
   Purpose : Change the cartoon light vector to match the view vector
   Parameters : CAMERAINFO
   Returns : 
   Info : 
*/
void UpdateCartoonLight(CAMERAINFO *cameraInfo);

/* --------------------------------------------------------------------------------
   Function		: UpdateCartoonLight
   Purpose		: set the direction and colour of the cartoon light
   Parameters	: direction vector of the light, colour of the light
   Returns		: 
   Info			: // PP: colour parameter has no effect yet, unfortunately
					// PP: This should not be called before InitCartoonLight
*/
void UpdateCartoonLight(const TBVector dir, const struct TAG_COLOUR& col);

/* --------------------------------------------------------------------------------
   Function		: SetLightRamp
   Purpose		: set the ramp to use for the cartoon shading
   Parameters	: index of the ramp row to use for the cartoon shading (RAMP_)
   Returns		: 
   Info			: // PP: This should not be called before InitCartoonLight
					// PP: change takes effect when EnableActorCartoonRenderMode is called
*/
void SetLightRamp(const LightRamp ramp);

/* --------------------------------------------------------------------------------
   Function : DestroyCartoonLight
   Purpose : Destroys the cartoon light system
   Parameters : 
   Returns : 
   Info : 
*/
void DestroyCartoonLight(void);


#endif