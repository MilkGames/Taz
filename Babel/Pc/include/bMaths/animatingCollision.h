// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : animatingCollision.h
//   Purpose : animating collision associated maths
// Component : Generic Maths
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BANIMATINGCOLLISION_H__
#define __BANIMATINGCOLLISION_H__

// ********************************************************************************
// Constants and Macros

#define TBACMESHINSTFLAG_DYNAMIC		0x00000001			// the instance was dynamically allocated.

// TBAnimatingCollisionInfo::flags
#define BMANIMCOLLINFO_RETURNTIME			0x00000001
#define BMANIMCOLLINFO_RETURNUV				0x00000002
#define BMANIMCOLLINFO_RETURNNORMAL			0x00000004
#define BMANIMCOLLINFO_RETURNPOSITION		0x00000008
#define BMANIMCOLLINFO_RETURNEXTENDEDINFO	0x00000010
#define BMANIMCOLLINFO_RETURNACTORNODE		0x00000020
#define BMANIMCOLLINFO_RETURNMESHNODE		0x00000040


// ********************************************************************************
// Globals


// ********************************************************************************
// Types, Structures and Classes

// an animating collision mesh hierarchy node
typedef struct _TBACMeshNode {
	struct _TBACMeshNode				*next, *prev;		// sibling links
	struct _TBACMeshNode				*parent, *children;	// hierarchy links

	uint32								crc;				// node CRC
	float								maxRadius;			// maximum radius
	// the next 8 bytes are kept in the next block.

	float								boxExtents[3];		// The half-extents of the box.
	float								basePosition[3];	// The center of the box in object coordinates

	uint32								noofVerts;			// number of vertices in this node.
	float								*verts;				// verts stored in an array of floats - (x,y,z), ...
	uint32								noofTris;			// number of triangles in this node.
	ushort								*tris;				// indices stored as shorts into the vertex array - (v1,v2,v3), ...
} TBACMeshNode;


// an animating collision mesh - information that is shared by all instances of an animating collision mesh
typedef struct _TBACMesh {
	TBResourceInfo						resInfo;			// global resource info

	TBACMeshNode						*rootNode;			// root of the node heirarchy
	uint32								flags;				// flags for this collision mesh
	uint32								lastFrame;			// last frame of animation

	float								maxRadius;			// maximum radius
	float								boxExtents[3];		// The half-extents of the box.

	float								basePosition[3];	// The center of the box in object coordinates
	uint32								pad1;
} TBACMesh;


typedef struct _TBACMeshInstanceNode
{
	struct _TBACMeshInstanceNode		*next, *prev;		// sibling links
	struct _TBACMeshInstanceNode		*parent, *children;	// hierarchy links

	TBVector							position;			// the last position of the collision mesh tested against.
	TBQuaternion						orientation;		// the last orientation of the collision mesh tested against.
	TBMatrix							transform;			// the last transformation that was used by this mesh instance.

	TBACMeshNode						*meshNode;			// the node in the mesh that this operates on.
	struct _TBActorNodeInstance			*actorNode;			// the node in the actor that this uses.
} TBACMeshInstanceNode;


typedef struct _TBACMeshInstance
{
	TBACMesh							*mesh;				// the mesh that this is an instance of
	struct _TBActorInstance				*actorInstance;		// the actor instance that controls this instance.
	TBACMeshInstanceNode				*rootNode;			// the root node of the mesh instance
	uint32								flags;
} TBACMeshInstance;


typedef enum _TBCollisionType
{
	None,
	Translate,
	Rotate,

	TBCollisionType_DWORD = 0x7fffffff
} TBCollisionType;

typedef struct _TBAnimatingCollisionInfo
{
	uint32								flags;
	TBCollisionType						type;
	float								time;
	uint32								extInfo;

	float								u, v;
	const TBACMeshInstanceNode			*meshNode;
	struct _TBActorNodeInstance			*actorNode;

	TBVector							normal;
	TBVector							position;
	TBVector							nodePosition;
} TBAnimatingCollisionInfo;

// ********************************************************************************
// Prototypes


/*	--------------------------------------------------------------------------------
	Function : bmInitACMeshInstance
	Purpose : Initialise a new collision mesh instance.
	Parameters : [in] a pointer to the instance, or NULL to allocate one
				[in] the collision mesh to base the instance on
				[in] the actor instance to get the animation from
	Returns : a pointer to the animating collision mesh.
	Info : 
*/

TBACMeshInstance *bmInitACMeshInstance(TBACMeshInstance * const instancePtr, TBACMesh * const baseMesh, struct _TBActorInstance * const actorInstance);


/*	--------------------------------------------------------------------------------
	Function : bmFreeACMeshInstance
	Purpose : Frees an actor instance.
	Parameters : [in] the instance to free.
	Returns : 
	Info : 
*/

void bmFreeACMeshInstance(TBACMeshInstance *instance);


/*	--------------------------------------------------------------------------------
	Function : bLoadAnimatingCollisionMeshByCRC
	Purpose : Load an animating collision mesh by its CRC from a package, returning a handle to it.
	Parameters : [in] package index
				[in] the crc to search for.
	Returns : Animating Collision Mesh handle.
	Info : 
*/

TBACMesh *bLoadAnimatingCollisionMeshByCRC(TBPackageIndex *index, uint32 crc);


/* --------------------------------------------------------------------------------
   Function : bDeleteAnimatingCollisionMesh
   Purpose : Delete an animating collision mesh
   Parameters : animating collision mesh handle
   Returns : 
   Info : 
*/

void bDeleteAnimatingCollisionMesh(TBACMesh *handle);


/*	--------------------------------------------------------------------------------
	Function : bmCollideRotTrianglePoint
	Purpose : Collide a rotating triangle with a point in world space.
	Parameters : [in] the triangle to rotate
				[in] the desired point of collision
				[in] the starting orientation of the triangle
				[in] the final orientation of the triangle
				[out] requested information about the collision, if there was one.
	Returns : 1 if there was a collision, 0 otherwise
	Info : 
*/

int bmCollideRotTrianglePoint(const TBCollisionTriangle * const tri, const TBVector collisionPoint, const TBQuaternion startQuat, const TBQuaternion endQuat, TBCollisionInfo * const collisionInfo);


/*	--------------------------------------------------------------------------------
	Function : bmCollideRotTriangleSphere
	Purpose : Collide a rotating triangle with a sphere.
	Parameters : [in] the triangle that is rotating.
				[in] the sphere to collide the triangle with.
				[in] the starting orientation of the triangle.
				[in] the orientation the triangle ends at.
				[out] any information about the collision (if there was one)
	Returns : 1 if there was a collision, 0 otherwise.
	Info : 
*/

int bmCollideRotTriangleSphere(const TBCollisionTriangle * const tri, const TBCollisionSphere * const sphere, const TBQuaternion startQuat, const TBQuaternion endQuat, TBCollisionInfo * const collisionInfo);


/*	--------------------------------------------------------------------------------
	Function : bmCollideACMeshInstanceSphere
	Purpose : Collide an animating mesh instance with a sphere.
	Parameters : [in] the root node of the animating mesh.
				[in] the sphere to collide the mesh with.
				[out] any information about the collision that occured (if there was one)
	Returns : TBCollisionTpye enumeration
	Info : 
*/

TBCollisionType bmCollideACMeshInstanceSphere(const TBACMeshInstance * const mesh, const TBCollisionSphere * const sphere, TBAnimatingCollisionInfo * const collisionInfo);


/*	--------------------------------------------------------------------------------
	Function : bmCollideSphereACMeshInstance
	Purpose : Collide a moving sphere with the current state of the collision mesh instance.
	Parameters : [in] the moving sphere
				[in] the static animating collision mesh :)
				[in] the movement vector of the sphere
				[in/out] the number of information structures passed in / filled in.
				[in/out] any information about the collision (if there was any)
	Returns : 1 if there was a collision, 0 otherwise.
	Info : 
*/

int bmCollideSphereACMeshInstance(const TBCollisionSphere * const sphere, const TBACMeshInstance * const mesh, const TBVector movementVector, int * const infoCount, TBAnimatingCollisionInfo * const collisionInfo);


/*	--------------------------------------------------------------------------------
	Function : bmCollideRayACMeshInstance
	Purpose : Collide a ray with the current state of the collision mesh instance.
	Parameters : [in] the ray
				[in] the static animating collision mesh :)
				[in/out] the number of information structures passed in / filled in.
				[in/out] any information about the collision (if there was any)
	Returns : 1 if there was a collision, 0 otherwise.
	Info : 
*/

int bmCollideRayACMeshInstance(const TBCollisionRay * const ray, const TBACMeshInstance * const mesh, int * const infoCount, TBAnimatingCollisionInfo * const collisionInfo);


/*	--------------------------------------------------------------------------------
	Function : bmUpdateACMesh
	Purpose : Update an animating collision mesh with the curent status of the actor it is bound to.
	Parameters : [in] the mesh to update.
	Returns : 
	Info : 
*/

void bmUpdateACMeshInstance(TBACMeshInstance * const mesh);


/*	--------------------------------------------------------------------------------
	Function : bmFindACMeshNodeByCRC
	Purpose : Find a node represented by a CRC in the animating collision mesh.
	Parameters : [in] the topmost node to test
				[in] the crc of the node to find.
	Returns : the node which was found or NULL on error
	Info : 
*/

TBACMeshInstanceNode *bmFindACMeshNodeByCRC(TBACMeshInstanceNode * const node, const uint32 crc);


/*	--------------------------------------------------------------------------------
	Function : bmFindACMeshNode
	Purpose : Find a named node in the animating collision mesh.
	Parameters : [in] the topmost node to test
				[in] the name of the node to find.
	Returns : the node which was found or NULL on error
	Info :
*/

TBACMeshInstanceNode *bmFindACMeshNode(TBACMeshInstanceNode * const node, const char * const nodeName);


#endif /* __BANIMATINGCOLLISION_H__ */