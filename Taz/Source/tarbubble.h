#ifndef	__H_TARBUBBLE
#define __H_TARBUBBLE

#include "actors.h"

#define MAX_TARBUBBLES	(30)
#define ITS_BUBBLE_TIME	(3.0f)

typedef struct TAG_TARBUBBLEINFO
{
	TBTexture		*texture;
	float			period;
	float			clock;
	float			height;

	float			width;
	uint32			textureCRC;
	uint32			active;
	int32			iPad;
}TARBUBBLEINFO, _TARBUBBLEINFO BALIGN16;

// CMD: extra structure
typedef struct TAG_TARBUBBLE
{
	TBVector		position;
	TBVector		drawPos;

	struct
	{
		int		r, g, b;
		float	a, drawAlpha;
	}colour;

	float			height;
	float			width;
	float			clock;
	TBTexture		*texture;

	float			drawSize;
	float			rotation;
	int32			iPad[2];
}TARBUBBLE, _TARBUBBLE BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateTarBubbleEmmiter
   Purpose : 
   Parameters :
   Returns : 
   Info : 
*/

int CreateTarBubbleEmmiter(ACTORINSTANCE *actorInstance, char *textureName, float height, float size, uint32 active);

/*  --------------------------------------------------------------------------------
	Function	: FindTexturesFromMapUsedByTarBubbleObjects
	Purpose		: Once all of the map has been loaded we can look for the textures used by sparking objects
	Parameters	: none
	Returns		: nothing
	Info		: check out the long function name
*/

void FindTexturesFromMapUsedByTarBubbleObjects(void);

/* --------------------------------------------------------------------------------
   Function : UpdateTarBubbleEmmiter
   Purpose : 
   Parameters :
   Returns : 
   Info : 
*/

void UpdateTarBubbleEmmiter(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : InitTarBubbleExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitTarBubbleExtraSystem(void);

/* --------------------------------------------------------------------------------
   Function : TarBubbleExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/

void TarBubbleExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms);

/* --------------------------------------------------------------------------------
   Function : TarBubbleExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/

int TarBubbleExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata);

/* --------------------------------------------------------------------------------
   Function : TarBubbleExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/

void TarBubbleExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);

/* --------------------------------------------------------------------------------
   Function : TarBubbleExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/

void TarBubbleExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);

/* --------------------------------------------------------------------------------
   Function : TarBubbleExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/

void TarBubbleExtraSystemInitCallback(TBExtraSystem *eSystem, int reason);

#endif