#ifndef __H_SMASHCAMERA
#define __H_SMASHCAMERA

#include "camera.h"

#define		CAMERASMASH_FADERATE	(100.0f)						// CMD: rate per second
enum ESmashType
{
	CAMERASMASH_PERMANENT,
	CAMERASMASH_FADE,
	CAMERASMASH_INITIALWOBBLE,
	CAMERASMASH_SPREAD,
	CAMERASMASH_STATIC,
};

typedef struct TAG_SCREENSMASHINFO
{
	struct
	{
		float		x, y;
		float		deltaX, deltaY;
		float		sizeX, sizeY;
		float		drawSizeFactor;
		float		maxDelta;
	} screenCoords;

	TBTexture		*texture;
	float			wobble;
	float			tilt;
	float			clock;

	float			alpha;
	ESmashType		type;
	ESmashType		state;
	CAMERAINFO		*camera;
} SCREENSMASHINFO, _SCREENSMASHINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : SmashCamera
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int SmashCamera(CAMERAINFO *camera, int xPosition, int yPosition, int width, int height, ESmashType type, float time, TBTexture *texture);

/* --------------------------------------------------------------------------------
   Function : UpdateScreenSmashEffect
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int UpdateScreenSmashEffect(SCREENSMASHINFO *info);

/* --------------------------------------------------------------------------------
   Function : DrawScreenSmashEffect
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void DrawScreenSmashEffect(SCREENSMASHINFO *info);

/* --------------------------------------------------------------------------------
   Function : DrawScreenSmashEffect
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void FreeScreenSmashEffect(CAMERAINFO *camera);

#endif