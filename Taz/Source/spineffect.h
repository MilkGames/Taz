#ifndef __H_SPINEFFECT
#define __H_SPINEFFECT
 
#define SPINFX_NUMSTRIPS	(100)
#define SPINFX_NUMSTREAKS	(20)
#define SPINFX_MINANGVEL	(900.0f)
#define SPINFX_SEGMENTS		(5)
#define SPINFX_SEGMENTLEN	(RAD(180.0f))
#define SPINFX_KOIKTIME		(0.5f)
#define SPINFX_YSCALE		(1.0f)

typedef struct TAG_SPINPOLYS
{
	TBPrimVertex	verts[(SPINFX_SEGMENTS*2)+2];

	TBMatrix		localToWorld;
	TBMatrix		scaleAndTrans;
	TBMatrix		randomTilt;

	float			angVelocity;
	float			currentAngle;
	TBTexture		*texture;
	TBTexture		*invisibleTexture;

	float			radius;
	float			radiusIdle;
	float			height;
	int32			iPad;
} SPINPOLYS, _SPINPOLYS BALIGN16;

typedef struct TAG_SPINEFFECTINFO
{
	SPINPOLYS		polys[SPINFX_NUMSTRIPS];

	float			koikTimer;
	int				rumbleHandle;
	int				spinUpdated;
	int				iPad;
} SPINEFFECTINFO, _SPINEFFECTINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateSpinEffectForActor
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

SPINEFFECTINFO *CreateSpinEffectForActor(uint32 playerNo);

/* --------------------------------------------------------------------------------
   Function : CreateSpinEffectForCutScene
   Purpose : initialises the spin effect used in the cut scenes
   Parameters : none, the spineffectiinfo is global, and there can only be one
   Returns : 
   Info : 
*/ 

void CreateSpinEffectForCutScene(void);

/* --------------------------------------------------------------------------------
   Function : UpdateSpinEffectForActor
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateSpinEffectForActor(struct TAG_ACTORINSTANCE *actorInstance, float t=fTime);

/* --------------------------------------------------------------------------------
   Function : UpdateSpinEffectForActor
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateCutSceneSpinEffect(TBActorInstance *actorInstance, TBActorNodeInstance *node);

/* --------------------------------------------------------------------------------
   Function : DrawSpinEffectForActor
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void DrawSpinEffectForActor(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : DrawSpinEffectForActor
   Purpose :  SPINNING ... over and over ... SPINNING
   Parameters : 
   Returns : 
   Info : 
*/

void DrawCutSceneSpinEffect(void);

/* --------------------------------------------------------------------------------
   Function : CreateSpinEffectStrips
   Purpose : Creates strips of polys for use in spin effect
   Parameters : 
   Returns : 
   Info : 
*/

void CreateSpinEffectStrips(SPINPOLYS *polys, int colour[3], int noofSegments, float segmentLength);

#endif