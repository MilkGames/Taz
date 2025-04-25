#ifndef __H_TARTANTAZ
#define __H_TARTANTAZ

#include "actors.h"

typedef struct TAG_TARTANTAZ
{
	TBRenderTarget	*target;
	TBTexture		*texture;
	float			tileU, tileV;
	float			velU, velV, deltaU, deltaV;

	int				width, height, rgbBits, zBits;

	float			actorAlpha;
	float			textureAlpha;
}TARTANTAZ, _TARTANTAZ;

/* --------------------------------------------------------------------------------
   Function : EnableTartanTazRenderMode
   Purpose : 
   Parameters : 
   Returns : 
   Info : must have characterinfo structure
*/

int EnableTartanTazRenderMode(ACTORINSTANCE *actorInstance, float tileU, float tileV);

/* --------------------------------------------------------------------------------
   Function : DisableTartanTazRenderMode
   Purpose : 
   Parameters : 
   Returns : 
   Info : must have characterinfo structure
*/

void DisableTartanTazRenderMode(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : DisableTartanTazRenderMode
   Purpose : 
   Parameters : 
   Returns : 
   Info : must have characterinfo structure
*/

char UpdateTartanTazRenderTarget(ACTORINSTANCE *actorInstance);

#endif