#ifndef __H_COLLISIONCACHE
#define __H_COLLISIONCACHE

#include "Maths.h"				// PP: Maths stuff
#include "Colour.h"				// PP: handy colour struct
#include "actors.h"


enum ECacheStatus{
	CACHESTATUS_RESIDENT,
	CACHESTATUS_RESTORING,
	CACHESTATUS_DORMANT,
};

#define CACHEFLAGS_LIVEUPDATE	(1<<0)
#define CACHEFLAGS_STAYRESIDENT (1<<1)
#define CACHEFLAGS_SUB			(1<<2)
#define CACHEFLAGS_DOUBLESIDED	(1<<3)
#define CACHEFLAGS_NEEDSUPDATE	(1<<4)
#define CACHEFLAGS_LIVESCALEUPDATE (1<<5)
#define CACHEFLAGS_DONTFREEVERTS (1<<6)

#define PROCESSCOLLFLAG_DONTAFFECTOBJECTS (1<<0)

#define BOXFLAG_XTEST	(1<<0)
#define BOXFLAG_YTEST	(1<<1)
#define BOXFLAG_ZTEST	(1<<2)


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														BOUNDINGBOX

											Cuboid bounding boxes, very handy.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


// PP: visibility states for a bounding box.
// PP: not-visible is guaranteed to be 0, and at-least-partly-visible is guaranteed to be non-zero
enum BBVisibility
{
	BBVIS_NO	=0,	// PP: completely out of view
	BBVIS_KINDA	=1,	// PP: partly in view
	BBVIS_YES	=2	// PP: completely in view
};


typedef struct TAG_BOUNDINGBOX
{
	float	xmin,xmax;
	float	ymin,ymax;
	float	zmin,zmax;
	// TP: float	pad[2];


	/*	--------------------------------------------------------------------------------
		Function 	: BOUNDINGBOX::TAG_BOUNDINGBOX
		Purpose 	: default constructor: calls init
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: 
	*/
	TAG_BOUNDINGBOX(void);


	/*	--------------------------------------------------------------------------------
		Function 	: BOUNDINGBOX::init
		Purpose 	: initialise the bounding box
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: sets extents to values such that the box will need to reshape to contain any point you give it
	*/
	void init(void);


	/*	--------------------------------------------------------------------------------
		Function 	: BOUNDINGBOX::visible
		Purpose 	: initialise the bounding box
		Parameters 	: (opt/identity)object matrix by which to multiply bounding box for test
		Returns 	: BBVIS_YES or BBVIS_NO or BBVIS_KINDA, which are self-explanatory yes?
		Info 		: not-visible is guaranteed to be 0, and at-least-partly-visible is guaranteed to be non-zero
	*/
	inline BBVisibility visible(const TBMatrix mat=bIdentityMatrix) const
	{
		int		rtn;

		rtn=bdTestBBVisibility(	mat,
								this->xmin,
								this->xmax,
								this->ymin,
								this->ymax,
								this->zmin,
								this->zmax);

		// PP: interpret return value...

		if(rtn == 0)
		{
			// PP: not visible
			return BBVIS_NO;
		}
		else if(rtn == 1)
		{
			// PP: fully visible
			return BBVIS_YES;
		}
		else// PP: if(rtn == 2)
		{
			// PP: partly visible
			return BBVIS_KINDA;
		}
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BOUNDINGBOX::minVert
		Purpose 	: get the minimum x,y&z coordinates of the box
		Parameters 	: 
		Returns 	: a VEC containing the minimum x,y&z coordinates of the box
		Info 		: // PP: 
	*/
	inline VEC minVert(void) const
	{
		return VEC(this->xmin, this->ymin, this->zmin);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BOUNDINGBOX::maxVert
		Purpose 	: get the maximum x,y&z coordinates of the box
		Parameters 	: 
		Returns 	: a VEC containing the maximum x,y&z coordinates of the box
		Info 		: // PP: 
	*/
	inline VEC maxVert(void) const
	{
		return VEC(this->xmax, this->ymax, this->zmax);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BOUNDINGBOX::dimensions
		Purpose 	: get the width, height & depth of the bounding box
		Parameters 	: 
		Returns 	: a VEC containing the x,y,&z dimensions of the box
		Info 		: // PP: 
	*/
	inline VEC dimensions(void) const
	{
		return (this->maxVert()-this->minVert());
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BOUNDINGBOX::pos
		Purpose 	: get the centre position of the bounding box
		Parameters 	: 
		Returns 	: the centre position of the bounding box!
		Info 		: // PP: 
	*/
	inline VEC pos(void) const
	{
		return this->minVert()+(this->dimensions()/2.0f);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BOUNDINGBOX::minDist
		Purpose 	: get the minimum distance between the bounding box and a point
		Parameters 	: 
		Returns 	: the minimum distance between the bounding box and the point!
		Info 		: // PP: 
	*/
	float minDist(const VEC& point) const;


	/*	--------------------------------------------------------------------------------
		Function 	: BOUNDINGBOX::expandToContain
		Purpose 	: reshape as neccessary to contain a specified position
		Parameters 	: TBVector position to contain
		Returns 	: true if the bounding box changed, else false
		Info 		: // PP: 
	*/
	bool expandToContain(const TBVector pos);
	
	
	/*	--------------------------------------------------------------------------------
		Function 	: BOUNDINGBOX::expandToContain
		Purpose 	: reshape as neccessary to contain a specified position
		Parameters 	: VEC position to contain
		Returns 	: true if the bounding box changed, else false
		Info 		: // PP: 
	*/
	bool expandToContain(const VEC& pos);


	/*	--------------------------------------------------------------------------------
		Function 	: BOUNDINGBOX::draw
		Purpose 	: draw the bounding box
		Parameters 	: (opt/GREEN) colour in which to draw the bounding box
		Returns 	: 
		Info 		: // PP: This is a debug facility, and not guaranteed to be fast, so check
						what it does if you're going to use it in the final game for any reason
	*/

#define BOUNDINGBOX_DEFAULT_COLOUR		GREEN	// PP: the old favourite, nice & techy

	void draw(const COLOUR& col=BOUNDINGBOX_DEFAULT_COLOUR) const;


	/*	--------------------------------------------------------------------------------
		Function 	: BOUNDINGBOX::operator =
		Purpose 	: convert a TBCollisionAABB into the BOUNDINGBOX
		Parameters 	: ref to TBCollisionAABB to convert
		Returns 	: BOUNDINGBOX representing the TBCollisionAABB
		Info 		: 
	*/
	struct TAG_BOUNDINGBOX& operator = (const TBCollisionAABB& box);


} BOUNDINGBOX, _BOUNDINGBOX;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												GENERAL COLLISION CACHE FUNCTIONS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


typedef struct TAG_LIVEUPDATEINFO
{
	TBVector		position;
	TBVector		scale;
	TBQuaternion	orientation;
} LIVEUPDATEINFO, _LIVEUPDATEINFO;

typedef struct TAG_COLLISIONLISTENTRY
{
	struct TAG_COLLISIONLISTENTRY	*next,*prev;

	struct TAG_COLLVERTEX			*vertices;
	struct TAG_COLLPOLY				*polys;
	int32							numVertices;
	int32							numPolys;

	struct TAG_ACTORINSTANCE		*actorInstance;
	float							timeSinceLastUsed;
	BOUNDINGBOX						boundingBox;
	BOUNDINGBOX						localBoundingBox;
	int32							memoryNeeded;
	uint32							flags;
	LIVEUPDATEINFO					*liveUpdateInfo;		// TP: optional extra info for live updates
	char							cacheStatus;
} COLLISIONLISTENTRY, _COLLISIONLISTENTRY;

typedef struct TAG_COLLISIONLIST
{
	COLLISIONLISTENTRY	head;
	int				numEntries;
} COLLISIONLIST, _COLLISIONLIST;

typedef struct TAG_COLLISIONCACHE
{
	COLLISIONLIST	collisionList;			// list of collision actors
	COLLISIONLIST	cylinderList;			// list of collision cylinder actors
	COLLISIONLIST	sphereList;				// list of collision spheres
	int32			memoryUsed;				// current memory usage of cache
	int32			maxMemoryAvailable;		// maximum amount of memory available to cache
	int32			peakMemoryUsed;			// peak usage of memory
	int32			hitsPerFrame;
	int32			missesPerFrame;
	int32			hitsPerScene;
	int32			missesPerScene;
} COLLISIONCACHE, _COLLISIONCACHE;

extern BOUNDINGBOX	worldBoundingBox;

// external functions
/* --------------------------------------------------------------------------------
   Function : NumberOfVertsInSet
   Purpose : Return the number of verts in an actor
   Parameters : TBSet
   Returns : number of verts
   Info : 
*/
int NumberOfVertsInSet(TBSet *set);

/* --------------------------------------------------------------------------------
   Function : AddActorToCollisionCache
   Purpose : add an actorInstance to the collision cache system 
   Parameters : COLLISIONCACHE, ACTORINSTANCE, flags
   Returns : collisionListEntry/FALSE
   Info : SET VERSION
*/
COLLISIONLISTENTRY *AddActorToCollisionCache(COLLISIONCACHE *collisionCache, TBSet *set, uint32 flags);

/* --------------------------------------------------------------------------------
   Function : CreateCollisionCache
   Purpose : Creates and intialises a collision cache
   Parameters : COLLISIONCACHE, cache size in bytes
   Returns : TRUE/FALSE
   Info : 
*/
BOOL CreateCollisionCache(COLLISIONCACHE *collisionCache,int maxSize);

/* --------------------------------------------------------------------------------
   Function : UpdateCollisionCache
   Purpose : updates the collision cache freeing and creating collision meshes as needed
   Parameters : COLLISIONCACHE
   Returns : TRUE/FALSE
   Info : should be called once per frame
*/
void UpdateCollisionCache(COLLISIONCACHE *collisionCache);

/* --------------------------------------------------------------------------------
   Function : UpdateCollisionBoundingBox
   Purpose : updates the world bounding box based on the position of the actor and the local bounding box
   Parameters : COLLISIONLISTENTRY
   Returns : 
   Info : 
*/
void UpdateCollisionBoundingBox(COLLISIONLISTENTRY *collisionListEntry);

/* --------------------------------------------------------------------------------
   Function : FreeCollisionCache
   Purpose : removes the collision cache and all collision entries in its list
   Parameters : COLLISIONCACHE
   Returns : 
   Info : 
*/
void FreeCollisionCache(COLLISIONCACHE *collisionCache);

/* --------------------------------------------------------------------------------
   Function : AddActorToCollisionCache
   Purpose : add an actorInstance to the collision cache system 
   Parameters : COLLISIONCACHE, ACTORINSTANCE, flags
   Returns : collisionListEntry/FALSE
   Info : 
*/
COLLISIONLISTENTRY *AddActorToCollisionCache(COLLISIONCACHE *collisionCache, struct TAG_ACTORINSTANCE *actorInstance, uint32 flags);

/* --------------------------------------------------------------------------------
   Function : AddActorToCollisionCylinderCache
   Purpose : add an actorInstance to the collision cylinder cache system 
   Parameters : COLLISIONCACHE, ACTORINSTANCE, flags
   Returns : TRUE/FALSE
   Info : 
*/
BOOL AddActorToCollisionCylinderCache(COLLISIONCACHE *collisionCache,struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : AddActorToCollisionSphereCache
   Purpose : add an actorInstance to the collision cylinder cache system 
   Parameters : COLLISIONCACHE, ACTORINSTANCE, flags
   Returns : TRUE/FALSE
   Info : 
*/

BOOL AddActorToCollisionSphereCache(COLLISIONCACHE *collisionCache, struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : RemoveActorFromCollisionCache
   Purpose : remove an actor from the collision cache
   Parameters : COLLISIONCACHE, ACTORINSTANCE
   Returns : TRUE/FALSE
   Info : 
*/
BOOL RemoveActorFromCollisionCache(COLLISIONCACHE *collisionCache, struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : MakeCollisionEntryDoubleSided
   Purpose : Create a second side to existing verts
   Parameters : COLLISIONLISTENTRY
   Returns : 
   Info : 
*/
void MakeCollisionEntryDoubleSided(COLLISIONLISTENTRY *collisionListEntry);

/* --------------------------------------------------------------------------------
   Function : DrawResidentCollisionEntries
   Purpose : Draw all collision entrys that are resident except the landscape
   Parameters : COLLISIONLIST
   Returns : 
   Info : 
*/
void DrawResidentCollisionEntries(COLLISIONCACHE *collisionCache);

/* --------------------------------------------------------------------------------
   Function : MakeEntireCollisionCacheDormant
   Purpose : Make every entry in the collision cache dormant and frees the memory
   Parameters : COLLISIONCACHE
   Returns : 
   Info : 
*/
void MakeEntireCollisionCacheDormant(COLLISIONCACHE *collisionCache);

/* --------------------------------------------------------------------------------
   Function : ProcessCollisionForActor
   Purpose : Process collision and physics for an actor using the collision cache
   Parameters : COLLISIONCACHE, ACTORINSTANCE
   Returns : 
   Info : 
*/
int ProcessCollisionForActor(COLLISIONCACHE *collisionCache, struct TAG_ACTORINSTANCE *actorInstance,uint32 flags=0, struct TAG_COLL_MESHINFO **retCollInfo=NULL);

/* --------------------------------------------------------------------------------
   Function : ProcessCollisionForActors
   Purpose : Process collision and physics for an actor list using the collision cache
   Parameters : COLLISIONCACHE, ACTORINSTANCELIST
   Returns : 
   Info : 
*/
void ProcessCollisionForActors(COLLISIONCACHE *collisionCache, struct TAG_ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : ProcessCollisionForActor
   Purpose : Process collision and physics for an actor using the collision cache
   Parameters : COLLISIONCACHE, ACTORINSTANCE
   Returns : 
   Info : 
*/
//int ProcessCollisionForActor(COLLISIONCACHE *collisionCache, struct TAG_ACTORINSTANCE *actorInstance, struct TAG_COLL_INFO **retCollInfo=NULL);

#define PHOP_EXCLUDEOBJECTS	(1<<0)

/* --------------------------------------------------------------------------------
   Function : ProcessHeightOfPoint
   Purpose : Run through collision cache and return point on ground below given coord
   Parameters : COLLISIONCACHE, TBVector point, TBVector pointOnGround
   Returns : TRUE - found point, FALSE - no point found
   Info : 
*/
BOOL ProcessHeightOfPoint(COLLISIONCACHE *collisionCache, TBVector point, float *pointOnGround, int flags=0, ESpecialType specialType=(ESpecialType)-1);

// internal functions

/* --------------------------------------------------------------------------------
   Function : CreateAndAddCollisionListEntry
   Purpose : Creates a collision entry for the specified actor instance
   Parameters : COLLISIONCACHE, ACTORINSTANCE
   Returns : COLLISIONLISTENTRY/FALSE
   Info : 
*/
COLLISIONLISTENTRY* CreateAndAddCollisionListEntry(COLLISIONLIST *collisionList, struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : RemoveAndFreeCollisionListEntry
   Purpose : Creates a collision entry for the specified actor instance
   Parameters : COLLISIONCACHE, ACTORINSTANCE
   Returns : TRUE/FALSE
   Info : 
*/
BOOL RemoveAndFreeCollisionListEntry(COLLISIONCACHE *collisionCache, struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : RemoveAndFreeCollisionCylinderListEntry
   Purpose : Creates a collision entry for the specified actor instance
   Parameters : COLLISIONLIST, ACTORINSTANCE
   Returns : TRUE/FALSE
   Info : 
*/
BOOL RemoveAndFreeCollisionCylinderListEntry(COLLISIONCACHE *collisionCache, struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : RemoveAndFreeCollisionSphereListEntry
   Purpose : Creates a collision entry for the specified actor instance
   Parameters : COLLISIONLIST, ACTORINSTANCE
   Returns : TRUE/FALSE
   Info : 
*/
BOOL RemoveAndFreeCollisionSphereListEntry(COLLISIONCACHE *collisionCache, struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : CalculateBoundingBox
   Purpose : calculates a bounding box for a mesh
   Parameters : current world box, local box, actorinstance,verts/NULL,numverts
   Returns : 
   Info : 
*/
void CalculateBoundingBox(BOUNDINGBOX *boundingBox,BOUNDINGBOX *localBoundingBox,struct TAG_ACTORINSTANCE *actorInstance, struct TAG_COLLVERTEX *svertices,int numberVerts);

/* --------------------------------------------------------------------------------
   Function : DrawCollisionListEntry
   Purpose : Draw the collision list entry for debug purposes
   Parameters : COLLISIONLISTENTRY
   Returns : 
   Info : 
*/
void DrawCollisionEntry(COLLISIONLISTENTRY *collisionListEntry);

/* --------------------------------------------------------------------------------
   Function : MakeEntryResident
   Purpose : If the entry is not already resident in memory this routine will load it
   Parameters : COLLISIONCACHE, COLLISIONLISTENTRY
   Returns : TRUE - loaded / FALSE - could not make resident
   Info : 
*/
BOOL MakeEntryResident(COLLISIONCACHE *collisionCache,COLLISIONLISTENTRY *collisionListEntry);

/* --------------------------------------------------------------------------------
   Function : MakeEntryDormant
   Purpose : Remove a collision entry from memory
   Parameters : COLLISIONCACHE, COLLISIONLISTENTRY
   Returns : 
   Info : 
*/
void MakeEntryDormant(COLLISIONCACHE *collisionCache,COLLISIONLISTENTRY *collisionListEntry,bool freeMem=true);

/* --------------------------------------------------------------------------------
   Function : ReturnOldestCollisionEntry
   Purpose : pick out the least used collision entry
   Parameters : COLLISIONCACHE
   Returns : COLLISIONLISTENTRY
   Info : 
*/
COLLISIONLISTENTRY *ReturnOldestCollisionEntry(COLLISIONCACHE *collisionCache);

/* --------------------------------------------------------------------------------
   Function : ConvertCollisionVertsByMatrix
   Purpose : Converts and set of vertices by a matrix and creates extra coll info
   Parameters : vertices, matrix, number of vertices
   Returns : 
   Info : 
*/
void ConvertCollisionVertsByMatrix(struct TAG_COLLVERTEX *vertices,TBMatrix matrix,int number);

/* --------------------------------------------------------------------------------
   Function : GetCollisionMesh
   Purpose : Spin though an actor extracting the mesh (including children)
   Parameters : root node, vertices, matrix
   Returns : new vertex pointer
   Info : 
*/
bool GetCollisionMesh(TBActorInstance *actorInstance,struct TAG_COLLVERTEX **vertices, struct TAG_COLLPOLY **polys, TBMatrix matrix, bool scale=false);

/* --------------------------------------------------------------------------------
   Function : NumberOfVertsInMesh
   Purpose : Return the number of verts in a mesh
   Parameters : TBSegmentedMesh
   Returns : number of verts
   Info : 
*/
int NumberOfVertsInMesh(TBActorInstance *actorInstance);

/* --------------------------------------------------------------------------------
   Function : NumberOfTrisInMesh
   Purpose : Return the number of tris in a mesh
   Parameters : TBSegmentedMesh
   Returns : number of verts
   Info : 
*/
int NumberOfTrisInMesh(TBActorInstance *actorInstance);

/* --------------------------------------------------------------------------------
   Function : NumberOfVertsInActor
   Purpose : Return the number of verts in an actor
   Parameters : TBActorInstance
   Returns : number of verts
   Info : 
*/
int NumberOfVertsInActor(TBActorInstance *actorInstance);

/* --------------------------------------------------------------------------------
   Function : NumberOfTrisInActor
   Purpose : Return the number of tris in an actor
   Parameters : TBActorInstance
   Returns : number of verts
   Info : 
*/
int NumberOfTrisInActor(TBActorInstance *actorInstance);

/* --------------------------------------------------------------------------------
   Function : TestBoxCollision
   Purpose : test for collision between two boxes
   Parameters : BOUNDINGBOX, BOUNDINGBOX
   Returns : TRUE/FALSE
   Info : 
*/
BOOL TestBoxCollision(BOUNDINGBOX *box1,BOUNDINGBOX *box2);

/* --------------------------------------------------------------------------------
   Function : TestBoxCollisionXZ
   Purpose : test for collision between two boxes in X and Z directions only
   Parameters : BOUNDINGBOX, BOUNDINGBOX
   Returns : TRUE/FALSE
   Info : 
*/
BOOL TestBoxCollisionXZ(BOUNDINGBOX *box1,BOUNDINGBOX *box2);

/* --------------------------------------------------------------------------------
   Function : FindMinMaxFromPoly
   Purpose : Find the min and max coords from a polygon
   Parameters : min, max, point1, point1, point1
   Returns : 
   Info : 
*/
#ifdef USE_SHORTS_FOR_COLLISION
void FindMinMaxFromPoly(short min[3], short max[3], TBVector point1, TBVector point2, TBVector point3);
#else
void FindMinMaxFromPoly(TBVector min, TBVector max, TBVector point1, TBVector point2, TBVector point3);
#endif

/* --------------------------------------------------------------------------------
   Function : GetBoxBoundsAndMatrix
   Purpose : Return the axis aligned bounding box and inverted matrix for test
   Parameters : ACTORINSTANCE, BOUNDINGBOX, matrix
   Returns : TRUE/FALSE
   Info : Returns a axis aligned bounding box with matrix for testing point in box
*/
int GetBoxBoundsAndMatrix(struct TAG_ACTORINSTANCE *actorInstance, BOUNDINGBOX *box, TBMatrix matrix);

/* --------------------------------------------------------------------------------
   Function : GetMinMaxFromVerts
   Purpose : Return a bounding box from verts
   Parameters : BOUNDINGBOX, COLLVERTEX, numberOfVerts
   Returns : 
   Info : 
*/
void GetMinMaxFromVerts(struct TAG_BOUNDINGBOX *box, struct TAG_COLLVERTEX *verts, int numberOfVerts);

/* --------------------------------------------------------------------------------
   Function : IsPointWithinBox
   Purpose : Is point within box
   Parameters : ACTORINSTANCE, point
   Returns : TRUE/FALSE
   Info : 
*/
int	IsPointWithinBox(struct TAG_ACTORINSTANCE *actorInstance, TBVector point);

/* --------------------------------------------------------------------------------
   Function : IsPointWithinBox
   Purpose : Is point within box
   Parameters : ACTORINSTANCE, point
   Returns : TRUE/FALSE
   Info : 
*/
int	IsPointWithinBox(const BOUNDINGBOX *box, const TBMatrix matrix, const TBVector point, int flags = BOXFLAG_XTEST|BOXFLAG_YTEST|BOXFLAG_ZTEST);

/* --------------------------------------------------------------------------------
   Function : RefreshCollisionEntry
   Purpose : Refresh current collision for actor to take account of animation changes
   Parameters : ACTORINSTANCE *actorInstance,
   Returns : 
   Info : 
*/
void RefreshCollisionEntry(COLLISIONCACHE *collisionCache,struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ActivateLiveUpdate
   Purpose : turn on live update for an collision entry
   Parameters : COLLISIONLISTENTRY
   Returns : 
   Info : 
*/
void ActivateLiveUpdate(COLLISIONLISTENTRY *collisionEntry);

/* --------------------------------------------------------------------------------
   Function : DeactiveLiveUpdate
   Purpose : stop the live update for the collisionentry
   Parameters : COLLISIONLISTENTRY
   Returns : 
   Info : 
*/
void DeactivateLiveUpdate(COLLISIONLISTENTRY *collisionEntry);

/* --------------------------------------------------------------------------------
   Function : TestBoxPointCollision
   Purpose : test for collision between two boxes
   Parameters : BOUNDINGBOX, TBVector
   Returns : TRUE/FALSE
   Info : 
*/
BOOL TestPointBoxCollision(struct TAG_BOUNDINGBOX *box1,TBVector point);

/*  --------------------------------------------------------------------------------
	Function	: GetCollisionEntry
	Purpose		: Get a pointer to the collision list entry for an actor
	Parameters	: COLLISIONCACHE, ACTORINSTANCE to find collision for
	Returns		: COLLISIONLISTENTRY*
	Info		: 
*/
COLLISIONLISTENTRY *GetCollisionEntry(COLLISIONCACHE *collisionCache, struct TAG_ACTORINSTANCE *actorInstance);

/*  --------------------------------------------------------------------------------
	Function	: GetCollSphereDetails
	Purpose		: Find out the current bounding radius and centre pos of an ACTORINSTANCE
	Parameters	: ACTORINSTANCE, ptr to float to receive radius,
					(opt/NULL=don't care) TBVector to receive centre pos
	Returns		: true if the actor was found in the collision cache, false if not
	Info		: // PP: ***** UNTESTED ***** 
*/
bool GetCollSphereDetails(struct TAG_ACTORINSTANCE* actorInstance, float * const radiusOut, TBVector centrePos=NULL);

/* --------------------------------------------------------------------------------
   Function : NumberOfTrisInSet
   Purpose : Return the number of tris in an actor
   Parameters : TBSet
   Returns : number of verts
   Info : 
*/
int NumberOfTrisInSet(TBSet *set);


/* --------------------------------------------------------------------------------
   Function : RayTest
   Purpose : Test if the ray hits any polys in the collision cache
   Parameters : COLLISIONCACHE, TBVector start, TBVector end
   Returns : TRUE - found point, FALSE - no point found
   Info : 
*/

bool RayTest(COLLISIONCACHE *collisionCache, TBVector start, TBVector end);


#endif