// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Glare.cpp
//   Purpose : Light glare effects
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"					// PP: global stuff
#include "main.h"					// PP: main stuff
#include "maths.h"					// PP: maths stuff
#include "actors.h"					// PP: actor stuff
#include "camera.h"					// PP: camera stuff
#include "Rectangle.h"				// PP: handy rectangle class
#include "display.h"				// PP: display stuff

#include "Glare.h"					// PP: Light glare effects


/*	--------------------------------------------------------------------------------
	Function 	: Glare_drawDirectional
	Purpose 	: draw directional light glare, eg. from the sun
	Parameters 	: direction vector, (opt/default)straight-on glare colour,
					(opt/default)angle range of glare
	Returns 	: the angle difference between the opposite of the camera dir and the light dir
	Info 		: 
*/
float Glare_drawDirectional(const VEC& dir,
						   const COLOUR& col,
						   const float angleRange)
{
	// PP: before we do anything else, work out if the glare is within angular range...

	VEC					camDirNorm;
	VEC					dirNorm;
	float				angDiff;

	camDirNorm=VEC(0.0f, 0.0f, 1.0f).rotate(gameStatus.viewAng);
	dirNorm=dir.normalise();
	angDiff=(1.0f-dotProduct(camDirNorm, dirNorm))*HALFPI;

	if(angDiff > angleRange)
	{
		return angDiff;
	}

	// PP: Ok it's in range let's draw...

	TBPrimVertex		verts[4];
	float				intensity;
	COLOUR				finalCol;

	intensity= (1.0f-(angDiff/angleRange));

	finalCol=COLOUR(col.r, col.g, col.b, (unsigned char)(col.a*intensity));

	SETPROJECTION(2D);
	bdSetIdentityObjectMatrix();

/* PP: REMOUT: looks dodge										 
	#if BPLATFORM == PS2
		bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
	#endif	
*/
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);// PP: I seem to remember that an interesting effect can be got from toggling this (interesting effect on the shadows)
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);

	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	bdSetTexture(0,NULL);

	for(int v=0; v<4; v++)
	{
		BDVERTEX_SETCOLOUR(&verts[v], finalCol);
		BDVERTEX_SETXYZW(&verts[v], ((v&1)?(-1.0f):(1.0f))*GLARE_RECT_WIDTH, ((v/2)?(-1.0f):(1.0f))*GLARE_RECT_HEIGHT, 0.0f, 0.0f);
	}

	bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,verts,4,BVERTTYPE_SINGLE);

	return angDiff;
}