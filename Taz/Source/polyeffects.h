#ifndef	__H_POLYEXPLODE
#define	__H_POLYEXPLODE

#define MARIORIPPLE_MAXAMP		(METERTOUNIT(0.15f))
#define MARIORIPPLE_AMPDEC		(METERTOUNIT(0.03f))

#define	POLYFX_DROP		(1<<0)
#define	POLYFX_EXPLODE	(1<<1)
#define POLYFX_POPBALL	(1<<2)
#define POLYFX_RIPPLE	(1<<3)

typedef struct TAG_TEXCOORDS
{
	float	u0, v0, u1, v1, u2, v2;
}TEXCOORDS, _TEXCOORDS;

typedef struct TAG_POLYEFFECTSINFO
{
	TBPrimVertex	vertices[3];
	TBVector		offset[3];
	TBVector		normals[3];

	TBVector		worldPosition;

	TBVector		localPosition;

	TBVector		velocity;

	TBVector		orientation;

	TBVector		rotation;

	float			scale;
	uint32			flags;
	struct TAG_POLYEFFECTSINFO	*next;
	struct TAG_POLYEFFECTSINFO	*prev;

	float			height;
	TBTexture		*texture;
	float			clock;
	float			fPad;

	float			amp[3];
	int				iPad;
} POLYEFFECTSINFO, _POLYEFFECTSINFO BALIGN16;

typedef struct TAG_POLYEFFECTSLIST
{
	POLYEFFECTSINFO		head;

	uint32				numEntries;
	uint32				pad[3];
} POLYEFFECTSLIST, _POLYEFFECTSLIST BALIGN16;

extern POLYEFFECTSLIST		polyEffectsList;
extern TBLightsource		polyEffectsLight;

/* --------------------------------------------------------------------------------
   Function : CreatePolyLevelCopyOfActorInstance
   Purpose : Create a copy of actor and place in primlist
   Parameters : 
   Returns : success/fail
   Info : 
*/

ACTORINSTANCE *CreatePolyLevelCopyOfActorInstance(struct TAG_COLLISIONCACHE *collisionCache, struct TAG_POLYEFFECTSLIST *polyEffectsList, struct TAG_ACTORINSTANCE *actorInstance, TBVector offset, int destroyOriginal, TBTexture *texture, uint32 flags);

/* --------------------------------------------------------------------------------
   Function : InitPolyEffectsList
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void InitPolyEffectsList(struct TAG_POLYEFFECTSLIST *polyEffectsList);

/* --------------------------------------------------------------------------------
   Function : PrepPolyEffects
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void PrepPolyEffects(struct TAG_POLYEFFECTSLIST *polyEffectsList);

/* --------------------------------------------------------------------------------
   Function : UpdatePolyEffects
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdatePolyEffects(struct TAG_POLYEFFECTSLIST *polyEffectsList);

/* --------------------------------------------------------------------------------
   Function : RemoveObjectFromPolyEffectsList
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RemoveObjectFromPolyEffectsList(struct TAG_POLYEFFECTSLIST *polyEffectsList, struct TAG_POLYEFFECTSINFO *remPtr);

/* --------------------------------------------------------------------------------
   Function : FreeAllPolyEffectsFromList
   Purpose : Removes all actors from the actor list and free the memory allocated completely
   Parameters : 
   Returns : 
   Info : 
*/
void FreeAllPolyEffectsFromList(struct TAG_POLYEFFECTSLIST *polyEffectsList, uint32 flags=POLYFX_DROP|POLYFX_EXPLODE|POLYFX_POPBALL|POLYFX_RIPPLE);

/* --------------------------------------------------------------------------------
   Function : RemoveObjectFromPolyEffectsList
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void DrawPolyEffects(struct TAG_POLYEFFECTSLIST *polyEffectsList);

#endif