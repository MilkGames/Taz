#ifndef __H_TUMBLEWEED
#define __H_TUMBLEWEED

#include "actors.h"

typedef struct TAG_TUMBLEWEEDINFO
{
	uint32				animCRC;
	TBActorNodeInstance	*node[3];
} TUMBLEWEEDINFO, _TUMBLEWEEDINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateTumbleWeedInfo
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

TUMBLEWEEDINFO *CreateTumbleWeedInfo(ACTORINSTANCE *actorInstance, uint32 animCRC, uint32 nodeCRC);

/* --------------------------------------------------------------------------------
   Function : CreateTumbleWeedInfo
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateTumbleweed(ACTORINSTANCE *actorInstance);

#endif