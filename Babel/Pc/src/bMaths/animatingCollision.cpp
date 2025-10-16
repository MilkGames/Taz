// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : animatingCollision.cpp
//   Purpose : animating collision associated maths
// Component : Generic Maths
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bmInitACMeshInstance
	Purpose : Initialise a new collision mesh instance.
	Parameters : [in] a pointer to the instance, or NULL to allocate one
				[in] the collision mesh to base the instance on
				[in] the actor instance to get the animation from
	Returns : a pointer to the animating collision mesh.
	Info : 
*/

TBACMeshInstance *bmInitACMeshInstance(TBACMeshInstance * const instancePtr, TBACMesh * const baseMesh, struct _TBActorInstance * const actorInstance)
{
        bkPrintf("*** WARNING *** bmInitACMeshInstance was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}


/*	--------------------------------------------------------------------------------
	Function : bmFreeACMeshInstance
	Purpose : Frees an actor instance.
	Parameters : [in] the instance to free.
	Returns : 
	Info : 
*/

void bmFreeACMeshInstance(TBACMeshInstance *instance)
{
        bkPrintf("*** WARNING *** bmFreeACMeshInstance was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bLoadAnimatingCollisionMeshByCRC
	Purpose : Load an animating collision mesh by its CRC from a package, returning a handle to it.
	Parameters : [in] package index
				[in] the crc to search for.
	Returns : Animating Collision Mesh handle.
	Info : 
*/

TBACMesh *bLoadAnimatingCollisionMeshByCRC(TBPackageIndex *index, uint32 crc)
{
        bkPrintf("*** WARNING *** bLoadAnimatingCollisionMeshByCRC was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}


/* --------------------------------------------------------------------------------
   Function : bDeleteAnimatingCollisionMesh
   Purpose : Delete an animating collision mesh
   Parameters : animating collision mesh handle
   Returns : 
   Info : 
*/

void bDeleteAnimatingCollisionMesh(TBACMesh *handle)
{
        bkPrintf("*** WARNING *** bDeleteAnimatingCollisionMesh was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


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

int bmCollideRotTrianglePoint(const TBCollisionTriangle * const tri, const TBVector collisionPoint, const TBQuaternion startQuat, const TBQuaternion endQuat, TBCollisionInfo * const collisionInfo)
{
        bkPrintf("*** WARNING *** bmCollideRotTrianglePoint was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


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

int bmCollideRotTriangleSphere(const TBCollisionTriangle * const tri, const TBCollisionSphere * const sphere, const TBQuaternion startQuat, const TBQuaternion endQuat, TBCollisionInfo * const collisionInfo)
{
        bkPrintf("*** WARNING *** bmCollideRotTriangleSphere was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bmCollideACMeshInstanceSphere
	Purpose : Collide an animating mesh instance with a sphere.
	Parameters : [in] the root node of the animating mesh.
				[in] the sphere to collide the mesh with.
				[out] any information about the collision that occured (if there was one)
	Returns : TBCollisionTpye enumeration
	Info : 
*/

TBCollisionType bmCollideACMeshInstanceSphere(const TBACMeshInstance * const mesh, const TBCollisionSphere * const sphere, TBAnimatingCollisionInfo * const collisionInfo)
{
        bkPrintf("*** WARNING *** bmCollideACMeshInstanceSphere was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return None;
}


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

int bmCollideSphereACMeshInstance(const TBCollisionSphere * const sphere, const TBACMeshInstance * const mesh, const TBVector movementVector, int * const infoCount, TBAnimatingCollisionInfo * const collisionInfo)
{
        bkPrintf("*** WARNING *** bmCollideSphereACMeshInstance was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


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

void bmUpdateACMeshInstance(TBACMeshInstance * const mesh)
{
        bkPrintf("*** WARNING *** bmUpdateACMeshInstance was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/*	--------------------------------------------------------------------------------
	Function : bmFindACMeshNodeByCRC
	Purpose : Find a node represented by a CRC in the animating collision mesh.
	Parameters : [in] the topmost node to test
				[in] the crc of the node to find.
	Returns : the node which was found or NULL on error
	Info : 
*/

TBACMeshInstanceNode *bmFindACMeshNodeByCRC(TBACMeshInstanceNode * const node, const uint32 crc)
{
        bkPrintf("*** WARNING *** bmFindACMeshNodeByCRC was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}

/*	--------------------------------------------------------------------------------
	Function : bmFindACMeshNode
	Purpose : Find a named node in the animating collision mesh.
	Parameters : [in] the topmost node to test
				[in] the name of the node to find.
	Returns : the node which was found or NULL on error
	Info :
*/

TBACMeshInstanceNode *bmFindACMeshNode(TBACMeshInstanceNode * const node, const char * const nodeName)
{
        bkPrintf("*** WARNING *** bmFindACMeshNode was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}