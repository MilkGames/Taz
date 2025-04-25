#ifndef __H_LIGHTNING
#define __H_LIGHTNING

#define LIGHTNING_NODE1LENGTH		(0.25f)
#define LIGHTNING_NODE2LENGTH		(0.33f)
#define LIGHTNING_NODE3LENGTH		(0.5f)
#define LIGHTNING_NODE4LENGTH		(0.67f)
#define	LIGHTNING_RADIUSRATIO		(0.25f)				//ratio of node one radius to length
#define LIGHTNING_NUMNODES			(6)

typedef struct TAG_LIGHTNINGINFO
{
	TBVector			currentStartPos;

	struct TAG_ELECTRICITYPATHINFO	*startPoints;
	TBTexture			*texture;
	float				clock;
	int					state;							// on or off

	TBVector			nodes[LIGHTNING_NUMNODES];
	float				size[LIGHTNING_NUMNODES];
	int					calcStartPoint;
	float				pad;
} LIGHTNINGINFO, _LIGHTNINGINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateLightning
   Purpose : creates lightning, for it is the function of the gods
   Parameters : 
   Returns : 
   Info : 
*/

int CreateLightning(char *objectName, char *name, CPMNode *node, CPMNode *rootNode);

/* --------------------------------------------------------------------------------
   Function : CreateLightningInfo
   Purpose : Creates lightning info structure
   Parameters : 
   Returns : 
   Info : 
*/

LIGHTNINGINFO *CreateLightningInfo(void);

/* --------------------------------------------------------------------------------
   Function : UpdateLightning
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateLightning(ACTORINSTANCE *ptr, ACTORINSTANCE *collider);

/* --------------------------------------------------------------------------------
   Function : CreateLightningNodePoints
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateLightningNodePoints(LIGHTNINGINFO *lightningInfo, ACTORINSTANCE *collider);

/* --------------------------------------------------------------------------------
   Function : DrawLightning
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void DrawLightning(ACTORINSTANCELIST *actorInstanceList);

#endif