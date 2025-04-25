#ifndef __H_VOODOOSPELLEFFECT
#define __H_VOODOOSPELLEFFECT

#include "actors.h"
#include "maths.h"

typedef struct TAG_VOODOOSPELLFXINFO
{
	ACTORINSTANCE		*actorInstance;
	TBActorNodeInstance	*node;
	float				duration;
	float				size;

	float				alpha;
	TBTexture			*rearTexture;
	int32				iPod[2];

	TBTexture			*texture;
	float				rotAngle[2];
	float				rotAngSpeed;
} VOODOOSPELLFXINFO, _VOODOOSPELLFXINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

VOODOOSPELLFXINFO *CreateVoodooSpellEffect(ACTORINSTANCE *actorInstance, char *node, float duration, float size=METERTOUNIT(0.5f));

/* --------------------------------------------------------------------------------
   Function : UpdateVoodooSpellEffect
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

VOODOOSPELLFXINFO *UpdateVoodooSpellEffect(VOODOOSPELLFXINFO *info);

/* --------------------------------------------------------------------------------
   Function : FreeVoodooSpellEffect
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

VOODOOSPELLFXINFO *FreeVoodooSpellEffect(VOODOOSPELLFXINFO *info);

/* --------------------------------------------------------------------------------
   Function : FreeVoodooSpellEffect
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void DrawVoodooSpellEffect(VOODOOSPELLFXINFO *info);

#endif