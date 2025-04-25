#ifndef __H_DECALS
#define __H_DECALS

#include "Colour.h"				// PP: handy colour struct

// PP: blend modes for decals
enum
{
	DECALBLEND_BLEND		=BDALPHABLENDMODE_BLEND,				// PP: normal alpha blending
	DECALBLEND_ADD			=BDALPHABLENDMODE_ADDITIVE,				// PP: additive colour blending
	DECALBLEND_SUBTRACT		=BDALPHABLENDMODE_SUBTRACTIVE,			// PP: subtractive colour blending
	DECALBLEND_IMPRESS,												// PP: combination of additive & subtractive that simulates impression
	DECALBLEND_RELIEVE,												// PP: combination of additive & subtractive that simulates relief

	DECALBLEND_DEFAULT		=DECALBLEND_BLEND						// PP: default blend mode to use for decals
};

typedef struct TAG_DECAL
{
	int						blendMode;			// PP: blend mode used when drawing the decal (DECALBLEND_...)
	TBPrimVertex			*vertices;			// TP: vertices used by decal
	TBTexture				*texture;			// TP: texture to apply to decal
	int32					numVerts;			// TP: number of vertices
	float					yrot;				// TP: y orientation of texture
	float					life;				// TP: remaining life in seconds of the decal
	float					age;				// PP: how long the decal has existed, in seconds
	int						primType;			// PP: primitive type used to draw the decal's verts
	COLOUR					colour;				// PP: colour, kept safe to allow fading
	float					fadeInTime;			// PP: duration of fade-in
	float					fadeOutTime;		// PP: duration of fade-out

} DECAL, _DECAL BALIGN16;

#define DECAL_DEFAULT_FADEIN	0.05f			// PP: default fade-in duration for decals
#define DECAL_DEFAULT_FADEOUT	(-1.0f)			// PP: default fade-out duration for decals - negative number means it starts to fade out as soon as it has faded in, and finishes fading out exactly at the end of its life

/* --------------------------------------------------------------------------------
   Function : CreateDecal
   Purpose : Creates a decal using the given texture and shadow information
   Parameters : shadow to use poly information from, texture to apply,
				orientation around Y(rads), life in seconds, r,g,b,a, (opt/BLEND) blend mode (DECALBLEND_...),
				(opt/default=quick)fade-in time, (opt/default=life-fadein)fade-out time
   Returns : 
   Info : This routine uses the vertices provided by the shadow for the decal
			// PP: Fake-bump-map modes (IMPRESS & RELIEVE) aren't supported by this function yet
*/
int CreateDecal(struct TAG_SHADOW *shadow,
				TBTexture *texture,
				float yrot,
				float life,
				uchar r,
				uchar g,
				uchar b,
				uchar a,
				const int blend=DECALBLEND_DEFAULT,
				const float fadeIn=DECAL_DEFAULT_FADEIN,
				const float fadeOut=DECAL_DEFAULT_FADEOUT);

/*	--------------------------------------------------------------------------------
	Function 	: CreateFloorDecal
	Purpose 	: create a decal on the X-Z plane at a specified position
	Parameters 	: texture ptr, position, y rotation, size, life in seconds, (opt/opque white) main/subtractive colour, (opt/opque white) additive colour,
					(opt/BLEND) blend mode (DECALBLEND_...), (opt/NULL) light direction vector for IMPRESS & RELIEVE modes,
					(opt/default=quick)fade-in time, (opt/default=life-fadein)fade-out time
	Returns 	: same as CreateDecal: TRUE for success, FALSE for failure
	Info 		: // PP: this one allows you to create floor decals without relying on a shadow struct for texture coords
*/
int CreateFloorDecal(	TBTexture *texture,
						TBVector pos,
						const float yRot,
						const float size,
						const float life,
						const COLOUR& col=COLOUR(128, 128, 128, 128),
						const int blend=DECALBLEND_DEFAULT,
						const COLOUR& col2=COLOUR(128, 128, 128, 128),
						TBVector lightDir=NULL,
						const float fadeIn=DECAL_DEFAULT_FADEIN,
						const float fadeOut=DECAL_DEFAULT_FADEOUT);

/* --------------------------------------------------------------------------------
   Function : DeleteDecal
   Purpose : Delete the decal structure
   Parameters : DECAL
   Returns : 
   Info : 
*/
void DeleteDecal(DECAL *decal);

/* --------------------------------------------------------------------------------
   Function : DeleteAllDecals
   Purpose : Deletes the entire decal list
   Parameters : 
   Returns : 
   Info : 
*/
void DeleteAllDecals(void);

/* --------------------------------------------------------------------------------
   Function : UpdateDecals
   Purpose : Update decals and remove old ones
   Parameters : 
   Returns : 
   Info : 
*/
void UpdateDecals(void);

/* --------------------------------------------------------------------------------
   Function : DrawDecal
   Purpose : Draws the given decal.
   Parameters : DECAL
   Returns : 
   Info : This routine does not setup the renderstates needed to draw
			// PP: apart from blend mode
*/
void DrawDecal(DECAL *decal);

/* --------------------------------------------------------------------------------
   Function : DrawAllDecals
   Purpose : Draws all the decals in the decal list
   Parameters : 
   Returns : 
   Info : 
*/
void DrawAllDecals(void);

#endif