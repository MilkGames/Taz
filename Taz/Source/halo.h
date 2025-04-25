#ifndef __H_HALO
#define __H_HALO

#include "actors.h"

#define HALO_MAXFLIES		(5)

typedef struct TAG_HALOINFO
{
	float			size;
	TBTexture		*texture;
	uint32			numFireflies;
	TBTexture		*flyTexture;

	TBVector		*flyPositions;
	TBVector		*flyVelocities;
	TBQuaternion	*flyOrientations;
	float			flySize;

	TBActorNodeInstance	*followNode;
	int				r, g, b;
}HALOINFO, _HALOINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateHaloObject
   Purpose : create an object which controls a bank of mist
   Parameters : 
   Returns : 
   Info : 
*/

int CreateHaloObject(ACTORINSTANCE *actorInstance, float size, uint32 textureCRC, uint32 nodeCRC, int red, int green, int blue);

/* --------------------------------------------------------------------------------
   Function : UpdateHaloObject
   Purpose : create an object which controls a bank of mist
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateHaloObject(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CreateHaloObject
   Purpose : create an object which controls a bank of mist
   Parameters : 
   Returns : 
   Info : 
*/

void DrawHaloObject(ACTORINSTANCELIST *instanceList);

/* --------------------------------------------------------------------------------
   Function : CreateHaloObject
   Purpose : create an object which controls a bank of mist
   Parameters : 
   Returns : 
   Info : 
*/

void FreeHaloObjectMemory(ACTORINSTANCE *actorInstance);

#endif