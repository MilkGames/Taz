#ifndef	__H_FLOCKING
#define __H_FLOCKING

#define FLOCK_TURNRATE			(RAD(180.0f))
#define FLOCK_SPEED				(METERTOUNIT(2.0f))

enum EFlockingState
{
	FLOCKER_FLOCKING,
	FLOCKER_HIT,
	FLOCKER_DEAD,
};

typedef struct TAG_FLOCKERS
{
	TBActorInstance		actorInstance;

	int					onScreen;
	EFlockingState		state;
	float				alpha;
	float				deathClock;

	uint32				deadFlockerCRC;
	uint32				sprayTextureCRC;
	int					iPad[2];
} FLOCKERS, _FLOCKERS BALIGN16;

typedef struct TAG_FLOCKINGINFO
{
	struct TAG_BOUNDINGBOX	boundingBox;

	TBVector	commonCentre;

	FLOCKERS	**flockers;
	float		safetyRange;
	int			numFlockers;
	uint32		deadFlockerCRC;
} FLOCKINGINFO, _FLOCKINGINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateFlockers
   Purpose : creates flocking objects
   Parameters : bounding box is in world space
   Returns : true/false dependant on success
   Info : 
*/

int CreateFlockers(struct TAG_ACTORINSTANCE *actorInstance, struct TAG_BOUNDINGBOX *boundingBox, char *name, float safetyRange, int number, uint32 deadFlockerCRC, uint32 sprayCRC);

/* --------------------------------------------------------------------------------
   Function : CreateFlockStructure
   Purpose : creates structures to contain info on flocking objects
   Parameters : 
   Returns : 
   Info : 
*/

FLOCKINGINFO *CreateFlockStructure(int numFlockers);

/* --------------------------------------------------------------------------------
   Function : PrepFlockers
   Purpose : preps flocking objects within a flocking specialtype
   Parameters : 
   Returns : 
*/

void PrepFlockers(struct TAG_ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : UpdateFlockers
   Purpose : updates flocking objects within a flocking specialtype
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateFlockers(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : DrawFlockers
   Purpose : draws flocking objects within a flocking specialtype
   Parameters : 
   Returns : 
   Info : 
*/

void DrawFlockers(struct TAG_ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : DrawFlockers
   Purpose : draws flocking objects within a flocking specialtype
   Parameters : 
   Returns : 
   Info : 
*/

void DrawFlockers2(struct TAG_ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : CheckForFlockerCollidingWithTazSpitObject
   Purpose : checks to see if an object that taz has spat, is close enough to count as a collision
   Parameters : 
   Returns : TRUE if hit, FALSE if not.
   Info : 
*/

void CheckForFlockerCollidingWithTazSpitObject(struct TAG_DEBRIS *debris);

#endif