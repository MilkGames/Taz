#ifndef __H_WATERRINGS
#define __H_WATERRINGS

#define WATER_RING_SPEED				(METERTOUNIT(1.0f))
#define TIME_BETWEEN_WATER_RINGS		(0.5f)

#define TIME_BETWEEN_MAP_WATER_RINGS	(2.0f)

#define WATERWIBBLE_SCALERATE			(0.5f)
#define WATERWIBBLE_SPEED				(METERTOUNIT(0.5f))

// CMD: flags
#define WATERRING_NOHEIGHTOFPOINT		(1<<0)

enum
{
	WATEREFFECT_ALL,
	WATEREFFECT_RING,
	WATEREFFECT_RIPPLE,
};

typedef struct TAG_WATERRING
{
	TBPrimVertex		vertices[4];

	TBVector			initialPosition;
	TBVector			currentScale;

	TBTexture			*texture;
	float				maxSize;
	TAG_WATERRING		*next;
	TAG_WATERRING		*prev;

	float				currentSize;
	uint32				drawRing;
	float				alpha;
	uint32				type;

	float				life;
	float				rippleRot;
	float				alphaRate;
	float				rotation;

	TBVector			rippleVelocity;

	ACTORINSTANCE		*mapInstance;
	int32				iPad[3];
} WATERRING, _WATERRING BALIGN16;

typedef struct TAG_WATERRINGLIST
{
	WATERRING			head;

	uint32				numEntries;
	uint32				pad[3];
} WATERRINGLIST, _WATERRINGLIST BALIGN16;

typedef struct TAG_WATERRINGOBJECTINFO
{
	float		clock;
	float		maxScale;
	float		minScale;
	float		rotation;
	TBTexture	*texture;
}WATERRINGOBJECTINFO, _WATERRINGOBJECTINFO;

extern	WATERRINGLIST		waterRingList;

/* --------------------------------------------------------------------------------
   Function : InitialiseWaterRingList
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void InitialiseWaterRingList(void);

/* --------------------------------------------------------------------------------
   Function : CreateWaterRing
   Purpose : creates a water ring, which consists of a texture mapped onto a couple of poly's
   Parameters : 
   Returns : 
   Info : 
*/

WATERRING *CreateWaterRing(TBVector position, TBTexture *textureName, float maxScale, float minScale=0.0f, float rotation=0.0f, uint32 flags=0);

/* --------------------------------------------------------------------------------
   Function : AddEntryToWaterRingList
   Purpose : does as the name suggests
   Parameters : 
   Returns : 
   Info : 
*/

WATERRING *AddEntryToWaterRingList(void);

/* --------------------------------------------------------------------------------
   Function : UpdateWaterRing
   Purpose : scales ring until big enough for taz
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateWaterRing(void);

/* --------------------------------------------------------------------------------
   Function : DrawWaterRing
   Purpose : scales ring until big enough for taz
   Parameters : 
   Returns : 
   Info : 
*/

void DrawWaterRing(void);

/* --------------------------------------------------------------------------------
   Function : DeleteWaterRingFromList
   Purpose : deletes a ring from the list
   Parameters : 
   Returns : 
   Info : 
*/

int DeleteWaterRingFromList(WATERRING *ring);

/* --------------------------------------------------------------------------------
   Function : FreeWaterRingList
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void FreeWaterRingList(uint32 type);

/* --------------------------------------------------------------------------------
   Function : CreateWaterRipple
   Purpose : creates and object (from the map) that emits water rings
   Parameters : 
   Returns : 
   Info : 
*/

WATERRINGOBJECTINFO *CreateWaterRingObject(ACTORINSTANCE *actorInstance, float maxScale, float minScale=0.0f, uint32 textureNameCRC=0, float rotation=0.0f);

/* --------------------------------------------------------------------------------
   Function : CreateWaterRipple
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateWaterRingObject(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CreateWaterRipple
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void FixUpWaterRingTextures(ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : CreateWaterRipple
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateWaterRipple(ACTORINSTANCE *actorInstance, TBTexture *texture, float maxScale);

/* --------------------------------------------------------------------------------
   Function : CheckWaterWibblePositionAgainstOthers
   Purpose : makes sure that positioin isn't going to interfere with any other wibbles
   Parameters : 
   Returns : TRUE if position is ok
   Info : This assumes there is only one water rin list, can't think why another would be needed
*/

int CheckWaterWibblePositionAgainstOthers(TBVector position);

#endif