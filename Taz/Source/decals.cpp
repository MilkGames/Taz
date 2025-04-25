// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : decals.cpp
//   Purpose : Functions for drawing decals using the shadow information
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "actors.h"
#include "linkedlist.h"
#include "shadow.h"
#include "decals.h"
#include "maths.h"

// for tests only
#include "control.h"
#include "main.h"
#include "cheats.h"

#include "VideoFX.h"		// PP: Video effects - motion blur, defocus, whatever.  Top-of-the-Pops-TASTIC, Mate!

LINKEDLIST<DECAL>	decalList("decallist");


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
int CreateDecal(SHADOW *shadow,
				TBTexture *texture,
				float yrot,
				float life,
				uchar r,
				uchar g,
				uchar b,
				uchar a,
				const int blend,
				const float fadeIn,
				const float fadeOut)
{
	DECAL		*decal;
	int			i;
	TBVector	tempVector;

	// TP: malloc decal
	decal = (DECAL*)ZALLOC(sizeof(DECAL));
	if (!decal) return FALSE;

	// TP: malloc room for the verts
	decal->numVerts = shadow->numPolys*3;
	decal->vertices = (TBPrimVertex*)ZALLOC(sizeof(TBPrimVertex)*decal->numVerts);
	if (!decal->vertices)
	{
		FREE(decal);
		return FALSE;
	}

	// PP: set this decal's blend mode
	decal->blendMode=blend;

	// PP: set the decal's primitive type
	decal->primType=BDPRIMTYPE_TRIANGLELIST;

	// PP: add the decal to the list
	decalList.append(decal);

	// TP: Copy vertices across
	fmemcpy(decal->vertices,shadow->vertices,sizeof(TBPrimVertex)*decal->numVerts);
	decal->texture = texture;
	decal->yrot = yrot;
	decal->life = life;

	// PP: set decal's age
	decal->age=0.0f;

	// PP: set the decal's fade-in & fade-out durations...

	decal->fadeInTime=fadeIn;
	
	if(fadeOut<0.0f)
	{
		// PP: fade out over whole lifetime (after fading in)
		decal->fadeOutTime=decal->life - decal->fadeInTime;
	}
	else
	{
		decal->fadeOutTime=fadeOut;
	}

	tempVector[Y] = 0.0f;
	tempVector[W] = 1.0f;

	// PP: store full colour for the decal, to allow for fading in & out
	decal->colour=COLOUR(r,g,b,a);

	// TP: Set texture coords...

	// PP: correct the decal size (the shadow will be a different size if it's a shadow map)...

	float	sizeCorrection;
	float	halfWidth=0.5f;

	if(VFX_effectActive(VFX_SHADOWMAP))
	{
		sizeCorrection=shadow->shadowRadius/shadow->shadowMapRadius;
	}

	for (i=0;i<decal->numVerts;i++)
	{
		// TP: rotate verts
		BDVERTEX_SETRGBA(&decal->vertices[i],r,g,b,a);
		BDVERTEX_GETUV(&decal->vertices[i],tempVector[X],tempVector[Z]);

		tempVector[X] -= 0.5f;
		tempVector[Z] -= 0.5f;

		bmVectorRotateY(tempVector,tempVector,-yrot);
		
		// PP: correct the decal size (the shadow will be a different size if it's a shadow map)...
		if(VFX_effectActive(VFX_SHADOWMAP))
		{
			tempVector[X] /= sizeCorrection;
			tempVector[Z] /= sizeCorrection;
		}

		tempVector[X] += 0.5f;
		tempVector[Z] += 0.5f;

		BDVERTEX_SETUV(&decal->vertices[i],tempVector[X],tempVector[Z]);
	}
	return TRUE;
}


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
						const COLOUR& col,
						const int blend,
						const COLOUR& col2,
						TBVector lightDir,
						const float fadeIn,
						const float fadeOut)
{
	DECAL		*decal;							// PP: ptr to decal being created
	TBVector	tempVector, tempVector2;		// PP: temporary vectors: 'ey, what can I do?
	int			v;								// PP: loop counter
	COLOUR		subCol;							// PP: colour or subtractive colour
	COLOUR		addCol;							// PP: additive colour

	// TP: malloc decal
	decal = (DECAL*)ZALLOC(sizeof(DECAL));
	if (!decal) return FALSE;

	// PP: set num verts
	if((blend == DECALBLEND_IMPRESS) || (blend == DECALBLEND_RELIEVE))
	{
		decal->numVerts=4*4;
	}
	else
	{
		decal->numVerts=4;
	}

	// TP: malloc room for the verts
	decal->vertices = (TBPrimVertex*)ZALLOC(sizeof(TBPrimVertex)*decal->numVerts);
	if (!decal->vertices)
	{
		FREE(decal);
		return FALSE;
	}

	// PP: set the decal's blend mode
	decal->blendMode=blend;

	// PP: set up verts...

	TBVector2D	xzLightSource;					// PP: 2D position of the "light source"
	TBVector2D	tempVec2D;						// PP: 'ey.
	float		length;							// PP: length of light ray
	float		fraction;						// PP: fraction of colour to apply to a vert

	// PP: get the 2D position of the "light source"...

	tempVec2D[X]= lightDir[X];
	tempVec2D[Y]= lightDir[Z];

	length=HYPOT(size, size);

	bmVector2DScaleToLength(xzLightSource, tempVec2D, length);

	for(v=0; v<4; v++)
	{
		// PP: calculate vertex offset
		SETVECTOR(tempVector, (((v&1)?(-1):1)*size), 0.0f, (((v/2)?(-1):1)*size), 0.0f);

		// PP: if we've specified a fake-bump-mapping mode, get the additive & subtractive colours of the vert...
		if((blend == DECALBLEND_IMPRESS) || (blend == DECALBLEND_RELIEVE))
		{
			// PP: rotate by y angle minus light angle...

			float	lightAngle=bmATan2Approx(xzLightSource[X], xzLightSource[Y]);

			bmVectorRotateY(tempVector2, tempVector, (-yRot)-lightAngle);

			// PP: then use Z coord to determine lightness

			if(decal->blendMode == DECALBLEND_IMPRESS)
			{
				fraction=(tempVector2[Z]+length)/(length*2.0f);
			}
			else// PP:  if(decal->blendMode == DECALBLEND_RELIEVE)
			{
				fraction= 1.0f-((tempVector2[Z]+length)/(length*2.0f));
			}

			// PP: calculate subtractive & additive colours
			subCol=col*fraction;
			addCol=col2*(1.0f-fraction);
		}
		else
		{
			// PP: ...otherwise we'll only use this first colour
			subCol=col;	
			
			// PP: store full colour for the decal, to allow for fading in & out
			decal->colour=subCol;
		}

		// PP: rotate offset
		bmVectorRotateY(tempVector, tempVector, -yRot);

		// PP: set texture coords for prim vertex
		BDVERTEX_SETUV(&decal->vertices[v], (v&1)?0.0f:1.0f, (v/2)?0.0f:1.0f);

		// PP: add centre pos
		bmVectorAdd(tempVector, tempVector, pos);

		// PP: apply position to prim vertex
		BDVERTEX_SETPOS(&decal->vertices[v], tempVector);

		// PP: apply colour to prim vertex
		BDVERTEX_SETCOLOUR(&decal->vertices[v], subCol);
		
		// PP: if we've specified a fake-bump-mapping mode, set up the additive vert corresponding to this subtractive one
		if((blend == DECALBLEND_IMPRESS) || (blend == DECALBLEND_RELIEVE))
		{
			decal->vertices[v+4]=decal->vertices[v];
			BDVERTEX_SETCOLOUR(&decal->vertices[v+4], addCol);
		}
	}

	// PP: copy all the verts we've filled-in to the upper half of the vert array for safe-keeping
	fmemcpy(&decal->vertices[decal->numVerts/2], decal->vertices, sizeof(TBPrimVertex)*(decal->numVerts/2));
	
	// PP: set the decal's texture
	decal->texture=texture;

	// PP: set the decal's y angle
	decal->yrot=yRot;

	// PP: set the decal's primitive type
	decal->primType=BDPRIMTYPE_TRIANGLESTRIP;

	// PP: set the decal's life
	decal->life=life;

	// PP: set decal's age
	decal->age=0.0f;

	// PP: set the decal's fade-in & fade-out durations...

	decal->fadeInTime=fadeIn;
	
	if(fadeOut<0.0f)
	{
		// PP: fade out over whole lifetime (after fading in)
		decal->fadeOutTime=decal->life - decal->fadeInTime;
	}
	else
	{
		decal->fadeOutTime=fadeOut;
	}

	// PP: add the decal to the list
	decalList.append(decal);

	return TRUE;
}


/* --------------------------------------------------------------------------------
   Function : DeleteDecal
   Purpose : Delete the decal structure
   Parameters : DECAL
   Returns : 
   Info : 
*/
void DeleteDecal(DECAL *decal)
{
	decalList.remove(decal);
	FREE(decal->vertices);
	FREE(decal);
}

/* --------------------------------------------------------------------------------
   Function : DeleteAllDecals
   Purpose : Deletes the entire decal list
   Parameters : 
   Returns : 
   Info : 
*/
void DeleteAllDecals(void)
{
	DECAL	*decal;
	int		i;

	for (i=decalList.getSize()-1;i>=0;i--)
	{
		decal = decalList.get(i);
		DeleteDecal(decal);
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateDecals
   Purpose : Update decals and remove old ones
   Parameters : 
   Returns : 
   Info : 
*/
void UpdateDecals(void)
{
	DECAL		*decal;
	int			i;
	int			v;
	COLOUR		newCol;
	float		fraction;
	bool		fading;

	for (i=decalList.getSize()-1;i>=0;i--)
	{
		decal = decalList.get(i);

		decal->life -= fTime;
		decal->age += fTime;

		if (decal->life <= 0.0f)
		{
			DeleteDecal(decal);
		}
		else
		{
			// PP: UPDATE FADES...

			if((decal->fadeInTime > 0.0f)&&(decal->age < decal->fadeInTime))// PP: fade-in
			{
				fading=true;
				fraction=decal->age/decal->fadeInTime;
			}
			else if((decal->fadeOutTime > 0.0f)&&(decal->life < decal->fadeOutTime))// PP: fade-out
			{
				fading=true;
				fraction=decal->life/decal->fadeOutTime;
			}
			else
			{
				fading=false;
			}

			if(fading)
			{
				if((decal->blendMode == DECALBLEND_IMPRESS)||(decal->blendMode == DECALBLEND_RELIEVE))
				{
					for(v=0; v<decal->numVerts/4; v++)
					{
						BDVERTEX_GETCOLOUR(&decal->vertices[decal->numVerts/2+v], newCol);

						newCol.r=(unsigned char)(newCol.r*fraction);
						newCol.g=(unsigned char)(newCol.g*fraction);
						newCol.b=(unsigned char)(newCol.b*fraction);

						BDVERTEX_SETCOLOUR(&decal->vertices[v], newCol);
					}

					for(v=decal->numVerts/4; v<decal->numVerts/2; v++)
					{
						BDVERTEX_GETCOLOUR(&decal->vertices[decal->numVerts/2+v], newCol);

						newCol.r=(unsigned char)(newCol.r*fraction);
						newCol.g=(unsigned char)(newCol.g*fraction);
						newCol.b=(unsigned char)(newCol.b*fraction);

						BDVERTEX_SETCOLOUR(&decal->vertices[v], newCol);
					}
				}
				else
				{
					newCol=COLOUR(	decal->colour.r,
									decal->colour.g,
									decal->colour.b,
									(unsigned char)(decal->colour.a*fraction));

					for(v=0; v<decal->numVerts; v++)
					{
						BDVERTEX_SETCOLOUR(&decal->vertices[v], newCol);
					}
				}
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawDecal
   Purpose : Draws the given decal.
   Parameters : DECAL
   Returns : 
   Info : This routine does not setup the renderstates needed to draw
			// PP: apart from blend mode
*/
void DrawDecal(DECAL *decal)
{
#if BPLATFORM == PS2
	bdSetZBias(-9000.0f);// PP: this will need tweaking - I have no idea what sort of value to use
#else
	bdSetZBias(-0.3f);
#endif

	if (CheatActive(CHEAT_QWILSON))
	{
		decal->texture = QwilsonTexture;
	}
	bdSetTexture(0,decal->texture);

	if((decal->blendMode == DECALBLEND_IMPRESS)||(decal->blendMode == DECALBLEND_RELIEVE))
	{
		// PP: draw subtractive part
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_SUBTRACTIVE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_SUBTRACTIVE, 1);

		if (decal->numVerts/4)
		{
			bdDrawPrimitive(decal->primType,decal->vertices,decal->numVerts/4,BVERTTYPE_SINGLE);
		}

		// PP: draw additive part
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_ADDITIVE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_ADDITIVE, 1);

		if (decal->numVerts/4)
		{
			bdDrawPrimitive(decal->primType,&(decal->vertices[decal->numVerts/4]),decal->numVerts/4,BVERTTYPE_SINGLE);
		}
	}
	else
	{
		// PP: set blend mode used by the particular decal
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, decal->blendMode, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, decal->blendMode, 1);
		
		if (decal->numVerts)
		{
			bdDrawPrimitive(decal->primType,decal->vertices,decal->numVerts,BVERTTYPE_SINGLE);
		}
	}

	bdSetZBias(0.0f);
}

/* --------------------------------------------------------------------------------
   Function : DrawAllDecals
   Purpose : Draws all the decals in the decal list
   Parameters : 
   Returns : 
   Info : 
*/
void DrawAllDecals(void)
{
	DECAL	*decal;
	int		i;

	bdSetIdentityObjectMatrix();
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSEQUAL, FALSE);

	// PP: TEMP TEST?
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);

	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_CLAMPU|BDTEXTUREWRAP_CLAMPV, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);

// PP: REMOUT: Doesn't seem to exist hmm			bdSetRenderState(BDRENDERSTATE_CLIPPING, TRUE, FALSE);

	for (i=decalList.getSize()-1;i>=0;i--)
	{
		decal = decalList.get(i);
		DrawDecal(decal);
	}
	
	// PP: set normal blend mode back in case any subsequent rendering depends on it
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
}