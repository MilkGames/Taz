// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : collision.h
//   Purpose : collision associated maths
// Component : Generic Maths
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BMATHS_COLLISION_H__
#define __BMATHS_COLLISION_H__

// ********************************************************************************
// Constants and Macros

// TBCollisionInfo::flags
#define BMCOLLINFO_RETURNTIME			0x00000001
#define BMCOLLINFO_RETURNUV				0x00000002
#define BMCOLLINFO_RETURNNORMAL			0x00000004
#define BMCOLLINFO_RETURNPOSITION		0x00000008
#define BMCOLLINFO_RETURNEXTENDEDINFO	0x00000010

// TBCollisionInfo::extInfo
// Global 'Failure Flags'
#define BMCOLLINFO_FAILTOOFAR			0x00000100
#define BMCOLLINFO_FAILWRONGDIRECTION	0x00000200
#define BMCOLLINFO_FAILALREADYINSIDE	0x00000400
#define BMCOLLINFO_FAILMISSED			0x00000800
#define BMCOLLINFO_FAILINVALIDINPUT		0x00001000

// Sphere v Triangle tests
#define BMCOLLINFO_FACECOLLISION		0x00000001
#define BMCOLLINFO_EDGECOLLISION		0x00000002
// bmCollideRayCylinder
#define BMCOLLINFO_SIDECOLLISION		0x00000001
#define BMCOLLINFO_BASECOLLISION		0x00000002
#define BMCOLLINFO_TOPCOLLISION			0x00000003

// ********************************************************************************
// Globals

extern float bCollisionEpsilon;			// used to get around floating point inaccuracies change depending on scale.
extern float bCollisionSquaredEpsilon;	// used to get around floating point inaccuracies in squared values. change depending on scale.

// ********************************************************************************
// Types, Structures and Classes

typedef struct _TBCollisionSphere
{
	TBVector					position;		// The center position of the sphere in world space
	float						radius;			// The radius of the sphere.
} TBCollisionSphere;

typedef struct _TBCollisionTriangle
{
	TBVector					verts[3];		// The three points that make the triangle in world space
} TBCollisionTriangle;

typedef struct _TBCollisionRay
{
	TBVector					start;			// Start point of the ray in world space
	TBVector					unitVector;		// The vector along which the ray travels
	float						length;			// The length of the ray.
} TBCollisionRay;

typedef struct _TBCollisionLine
{
	TBVector					start;			// Start point of the line in world space
	TBVector					end;			// End point of the line in world space
} TBCollisionLine;

typedef struct _TBCollisionAABB
{
	TBVector					minBox;			// The minimum points on the box in world space
	TBVector					maxBox;			// The maximum points on the box in world space
} TBCollisionAABB;

typedef struct _TBCollisionCylinder
{
	TBVector					base;			// The base position of the clyinder. The bottom-center.
	TBVector					unitDirection;	// A unit vector specifying the direction that the cylinder is projected along (e.g. 0 1 0)
	float						radius;			// The radius of the cylinder.
	float						height;			// The height of the cylinder.
} TBCollisionCylinder;

typedef struct _TBCollisionBox
{
	TBVector					position;		// the position of the box in world space
	TBQuaternion				orientation;	// the oriantation of the box expressed as a quaternion
	TBVector					halfSize;		// the half-diagonal (from the position (center) to the maximum extents of the box)
} TBCollisionBox;

typedef struct _TBCollisionInfo
{
	uint32						flags;			// Bit flags telling the collision functions which values to fill in correctly.
	float						time;			// The time along the movement vector that was passed in that the collision took place
	float						u, v;			// Surface coordinates on the primitive where the collision took place
	TBVector					normal;			// The normal of the point of the collision on the primitive
	TBVector					position;		// The position that the collision occurred at.
	uint32						extInfo;		// Collision routine specific information.
} TBCollisionInfo;


// a collision mesh cell record
typedef struct _TBCollisionMeshCell {
	int32							noofReferences;						// number of tri references in this cell
	ushort							*references;						// ptr to tri references (3*shorts each)
	int32							indexBase;							// base tri index (references are relative to this)
	uint32							pad1;
} TBCollisionMeshCell;


// a collision mesh resource
typedef struct _TBCollisionMesh {
	TBResourceInfo					resInfo;							// standard resource info
						
	int32							cellRes[3];							// cell resolution
	float							*vertices;							// ptr to vertex positions

	float							cellSize[3];						// cell dimensions
	ushort							*tris;								// ptr to tri indices

	float							min[3];								// minimum limits on (x, y, z)
	TBCollisionMeshCell				*cells;								// ptr to cell records 'cells[x][y][z]' order

	float							max[3];								// minimum limits on (x, y, z)
	uint32							pad1;
} TBCollisionMesh;


// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : bmCollideSphereSphere
	Purpose : Checks collision between two spheres, one of which has an accociated movement vector
	Parameters : [in] the sphere which is moving (this contains the starting position of this sphere)
				[in] the sphere which is static
				[in] the vector along which the sphere moves
				[in/out] the information requested by the user and the results of the collision (if there was one)
	Returns : 1 if a collision occured, 0 if no collision occured
	Info : This is a wrapper function for bmCollideSphereSphere_Unit - if you have access to the unit movement vector and the length of the vector, use that instead.
*/

extern int bmCollideSphereSphere(const TBCollisionSphere * const movingSphere, const TBCollisionSphere * const staticSphere, const TBVector movementVector, TBCollisionInfo * const collisionInfo);


/*	--------------------------------------------------------------------------------
	Function : bmCollideSphereSphere_Unit
	Purpose : Check collision between 2 spheres (one of which has an accociated movement vector)
	Parameters : [in] the sphere which is moving (this contains the starting position of this sphere)
				[in] the sphere which is static
				[in] the vector along which the sphere moves
				[in/out] the information requested by the user and the results of the collision (if there was one)
	Returns : 1 if a collision occured, 0 if no collision occured
	Info : This is just a wrapper for bmCollideRaySphere_Unit - all it does is replace the moving sphere with a ray, and add the movingSphere radius onto the staticSphere's
*/

extern int bmCollideSphereSphere_Unit(const TBCollisionSphere * const movingSphere, const TBCollisionSphere * const staticSphere, const TBVector movementVector, const float movementVectorLen, TBCollisionInfo * const collisionInfo);


/*	--------------------------------------------------------------------------------
	Function : bmCollideRaySphere
	Purpose : Checks intersection between a ray (using the movementVector) and a static sphere
	Parameters : [in] the ray to test against the sphere
				[in] the sphere to test
				[in] the movement vector of the ray
				[in/out] the information requested by the user and the results of the collision (if there was one)
	Returns : 1 if a collision occured, 0 otherwise
	Info : This is a wrapper function for bmCollideRaySphere_Unit - if you have access to the unit movement vector and the length of the vector, use that instead.
*/

extern int bmCollideRaySphere(const TBCollisionRay * const ray, const TBCollisionSphere * const sphere, TBCollisionInfo * const collisionInfo);


/*	--------------------------------------------------------------------------------
	Function : bmCollideRaySphere_Unit
	Purpose : Checks intersection between a ray (using the movementVector) and a static sphere
	Parameters : [in] the ray to test against the sphere
				[in] the sphere to test
				[in] the unit movement vector of the ray
				[in] the length of the movement vector (i.e. the range of the ray)
				[in/out] the information requested by the user and the results of the collision (if there was one)
	Returns : 1 if a collision occured, 0 otherwise
	Info : This was basically copied directly from graphics gems.
*/

extern int bmCollideRaySphere_Unit(const TBCollisionRay * const ray, const TBCollisionSphere * const sphere, TBCollisionInfo * const collisionInfo);


/*	--------------------------------------------------------------------------------
	Function : bmCollideRayTriangle
	Purpose : Checks the intersection between a ray and a triangle
	Parameters : [in] the ray to test
				[in] the triangle to test
				[in] the vector that represents the movement of the ray
				[in/out] the information requested by the user and the results of the collision (if there was one)
	Returns : 1 if a collision occured, 0 otherwise
	Info : This is using the same technique as described in Dr. Dobbs Journal issue #315 (August 2000)
*/

extern int bmCollideRayTriangle(const TBCollisionRay * const ray, const TBCollisionTriangle * const tri, TBCollisionInfo * const collisionInfo);


/*	--------------------------------------------------------------------------------
	Function : bmCollideRayAABB
	Purpose : Checks the intersection between a ray and an axis aligned bounding box
	Parameters : [in] the ray to test
				[in] the bounding box to test
				[in] the vector that represents the movement of the ray
				[in/out] the information requested by the user and the results of the collision (if there was one)
	Returns : 1 if a collision occured, 0 otherwise
	Info : 
*/

extern int bmCollideRayAABB(const TBCollisionRay * const ray, const TBCollisionAABB * const box, TBCollisionInfo * const collisionInfo);


/*	--------------------------------------------------------------------------------
	Function : bmCollideSphereTriangle
	Purpose : Checks the intersection between a moving sphere and triangle
	Parameters : [in] the ray to test
				[in] the triangle to test
				[in] the vector that represents the movement of the sphere
				[in/out] the information requested by the user and the results of the collision (if there was one)
	Returns : 1 if a collision occured, 0 otherwise
	Info : 
*/

extern int bmCollideSphereTriangle(const TBCollisionSphere * const sphere, const TBCollisionTriangle * const tri, const TBVector movementVector, TBCollisionInfo * const collisionInfo);


/*	--------------------------------------------------------------------------------
	Function : bmCollideSphereLine
	Purpose : Checks the intersection between a moving sphere and a line.
	Parameters : [in] the sphere to check.
				[in] the line to check against.
				[in] the movementVector of the sphere.
				[out] any information the user requested about the collision (if there was one)
	Returns : 1 if there was a collision between the primitives, 0 otherwise.
	Info : uses bmGeomIntersectLines.
*/

int bmCollideSphereLine(const TBCollisionSphere * const sphere, const TBCollisionLine * const line, const TBVector movementVector, TBCollisionInfo * const collisionInfo);


/*	--------------------------------------------------------------------------------
	Function : bmCollideSphereMesh
	Purpose : Collide a moving sphere with a static mesh
	Parameters : [in] the sphere to collide with the set
				[in] the mesh to collide the sphere with.
				[in] the movementVector of the sphere
				[in/out] the number of collision structures available / the number that were filled in.
				[out] a series of information structures detailing the triangles that the sphere collided with.
	Returns : 1 if a collision occured, 0 otherwise.
	Info : 
*/

int bmCollideSphereMesh(const TBCollisionSphere * const sphere, const TBCollisionMesh * const mesh, const TBVector movementVector, int * const collisionInfoCount, TBCollisionInfo * const collisionInfo);


/*	--------------------------------------------------------------------------------
	Function : bmCollideRayMesh
	Purpose : Collide a ray with a static mesh
	Parameters : [in] the ray to collide with the mesh
				[in] the mesh to collide the ray with.
				[in/out] the number of collision structures available / the number that were filled in.
				[out] a series of information structures detailing the triangles that the ray collided with.
	Returns : 1 if a collision occured, 0 otherwise.
	Info : The collision flags are specified by "collisionInfo[0].flags", and nothing else. The extended information is always returned.
*/

int bmCollideRayMesh(const TBCollisionRay * const ray, const TBCollisionMesh * const mesh, int * const collisionInfoCount, TBCollisionInfo * const collisionInfo);


/*	--------------------------------------------------------------------------------
	Function : bmCollideRayCylinder
	Purpose : Test for the collision between a ray and a static cylinder.
	Parameters : [in] the ray to test (must be a unit-ray)
				[in] the cylinder to test against.
				[out] a structure giving information abouth the collision.
	Returns : 1 if a collision occured, 0 otherwise.
	Info : 
*/

int bmCollideRayCylinder(const TBCollisionRay * const ray, const TBCollisionCylinder * const cylinder, TBCollisionInfo * const collisionInfo);


/*	--------------------------------------------------------------------------------
	Function : bmCollideRayBox
	Purpose : Collide a ray with an arbitrarily aligned box.
	Parameters : [in] the ray to test against the box.
				[in] the box to test the ray against
				[out] the information about the collision (if there was one)
	Returns : 1 if there was a collision, 0 otherwise
	Info : 
*/

int bmCollideRayBox(const TBCollisionRay * const ray, const TBCollisionBox * const box, TBCollisionInfo * const collInfo);


/*	--------------------------------------------------------------------------------
	Function : bmCollideSphereAABB
	Purpose : Collide a moving sphere with an axis aligned bounding box.
	Parameters : [in] the sphere that moves.
				[in] the box it collides with
				[in] the movement vector of the sphere.
				[out] the information about the collision (if there was one)
	Returns : 1 if there was a collision, 0 otherwise.
	Info : 
*/

int bmCollideSphereAABB(const TBCollisionSphere * const movingSphere, const TBCollisionAABB * const box, const TBVector movementVector, TBCollisionInfo * const collisionInfo);


/*	--------------------------------------------------------------------------------
	Function : bmCollideSphereBox
	Purpose : Collide a moving sphere with a bounding box.
	Parameters : [in] the sphere that moves.
				[in] the box it collides with
				[in] the movement vector of the sphere.
				[out] the information about the collision (if there was one)
	Returns : 1 if there was a collision, 0 otherwise.
	Info : 
*/

int bmCollideSphereBox(const TBCollisionSphere * const movingSphere, const TBCollisionBox * const box, const TBVector movementVector, TBCollisionInfo * const collisionInfo);


/* --------------------------------------------------------------------------------
   Function : bLoadCollisionMeshByCRC
   Purpose : Load a collision mesh from a package returning a handle to it
   Parameters : package index, texture CRC
   Returns : handle or NULL for failure
   Info : 
*/

TBCollisionMesh *bLoadCollisionMeshByCRC(TBPackageIndex *index, uint32 crc);


/* --------------------------------------------------------------------------------
   Function : bDeleteCollisionMesh
   Purpose : Delete a collision mesh
   Parameters : collision mesh handle
   Returns : 
   Info : 
*/

void bDeleteCollisionMesh(TBCollisionMesh *handle);


/*	--------------------------------------------------------------------------------
	Function : bmCollideTriangleSphere
	Purpose : collide a moving triangle with a static sphere.
	Parameters : [in] the moving triangle.
				[in] the sphere
				[in] the movement vector of the triangle.
				[out] the information about the collision (if there was one)
	Returns : 1 if a collision occured, 0 otherwise.
	Info : this is a wrapper function for the bmCollideSphereTriangle function.
*/

int bmCollideTriangleSphere(const TBCollisionTriangle * const tri, const TBCollisionSphere * const sphere, const TBVector movementVector, TBCollisionInfo * const collisionInfo);


/*	--------------------------------------------------------------------------------
	Function : bIntersectSpherePlane
	Purpose : Collide a Sphere with a Plane.
	Parameters : [in] sphere
				[in] movement vector of sphere
				[in] plane (as in plane eqn. ax + by + cz + d == 0)
	Returns : <0 if behind the plane, 0 if straddling the plane, >0 if in front of the plane.
	Info : Internal because the parameters are not 'nice'
*/

int bIntersectSpherePlane(const TBCollisionSphere * const sphere, const TBVector movementVector, const TBVector plane);


/*	--------------------------------------------------------------------------------
	Function : bIntersectRayPlane
	Purpose : Intersect a Ray with a Plane.
	Parameters : [in] ray
				[in] plane (as in plane eqn. ax + by + cz + d == 0)
	Returns : <0 if behind the plane, 0 if straddling the plane (within collision epsilon), >0 if in front of the plane.
	Info : Internal because the parameters are not 'nice'
*/

int bIntersectRayPlane(const TBCollisionRay * const ray, const TBVector plane);


/*	--------------------------------------------------------------------------------
	Function : bIntersectRayPlane_Unit
	Purpose : Intersect a unit ray with a plane.
	Parameters : [in] unit length ray
				[in] plane (as in plane eqn. ax + by + cz + d == 0)
	Returns : <0 if behind the plane, 0 if straddling the plane (within collision epsilon) , >0 if in front of the plane.
	Info : Internal because the parameters are not 'nice'
*/

int bIntersectRayPlane_Unit(const TBCollisionRay * const ray, const TBVector plane);


/*	--------------------------------------------------------------------------------
	Function : bmIntersectRaySphere_Unit
	Purpose : Intersect a ray with a sphere 
	Parameters : [in] the unit length ray
				[in] the sphere
	Returns : whether there was an intersection between the primitives.
	Info : 
*/

int bmIntersectRaySphere_Unit(const TBCollisionRay * const ray, const TBCollisionSphere * const sphere);


/*	--------------------------------------------------------------------------------
	Function : bmIntersectRaySphere
	Purpose : Intersect a ray with a sphere 
	Parameters : [in] the ray
				[in] the sphere
	Returns : whether there was an intersection between the primitives.
	Info : 
*/

int bmIntersectRaySphere(const TBCollisionRay * const ray, const TBCollisionSphere * const sphere);


#endif /* __BMATHS_COLLISION_H__ */