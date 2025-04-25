#ifndef __H_MIRRORBALL
#define __H_MIRRORBALL

#define MIRRORBALL_SWITCHTIME		(0.1f)
#define MIRRORBALL_SCROLLSPEED		(0.5f)


typedef struct TAG_MIRRORBALLINFO
{
	TBLightsource		red;
	TBLightsource		blue;
	TBLightsource		green;
	TBLightsource		yellow;
	TBLightsource		bottom;
	float				clock;
}MIRRORBALLINFO, _MIRRORBALLINFO;

extern MIRRORBALLINFO	*tazboss1Mirrorball;

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

MIRRORBALLINFO *CreateMirrorballLights(void);

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateMirrorball(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int DrawMirrorball(ACTORINSTANCELIST *actorInstanceList);

#endif