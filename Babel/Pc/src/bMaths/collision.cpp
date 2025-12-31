// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : collision.h
//   Purpose : collision associated maths
// Component : Generic Maths
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Globals

float bCollisionEpsilon = 0.001f;

// ********************************************************************************
// Function Implementations

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

extern int bmCollideSphereSphere(const TBCollisionSphere * const movingSphere, const TBCollisionSphere * const staticSphere, const TBVector movementVector, TBCollisionInfo * const collisionInfo)
{
        bkPrintf("*** WARNING *** bmCollideSphereSphere was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


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

extern int bmCollideSphereSphere_Unit(const TBCollisionSphere * const movingSphere, const TBCollisionSphere * const staticSphere, const TBVector movementVector, const float movementVectorLen, TBCollisionInfo * const collisionInfo)
{
        bkPrintf("*** WARNING *** bmCollideSphereSphere_Unit was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


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

extern int bmCollideRaySphere(const TBCollisionRay * const ray, const TBCollisionSphere * const sphere, TBCollisionInfo * const collisionInfo)
{
        bkPrintf("*** WARNING *** bmCollideRaySphere was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


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

extern int bmCollideRaySphere_Unit(const TBCollisionRay * const ray, const TBCollisionSphere * const sphere, TBCollisionInfo * const collisionInfo)
{
        bkPrintf("*** WARNING *** bmCollideRaySphere_Unit was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


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

extern int bmCollideRayTriangle(const TBCollisionRay * const ray, const TBCollisionTriangle * const tri, TBCollisionInfo * const collisionInfo)
{		
    const TBCollisionTriangle *tri0 = tri;

    float e1x = tri->verts[1][0] - tri->verts[0][0];
    float e1y = tri->verts[1][1] - tri->verts[0][1];
    float e1z = tri->verts[1][2] - tri->verts[0][2];

    float e2x = tri->verts[2][0] - tri->verts[0][0];
    float e2y = tri->verts[2][1] - tri->verts[0][1];
    float e2z = tri->verts[2][2] - tri->verts[0][2];

    float tx = ray->start[0] - tri->verts[0][0];
    float ty = ray->start[1] - tri->verts[0][1];
    float tz = ray->start[2] - tri->verts[0][2];

    float qx = (ty * e1z) - (tz * e1y);
    float qy = (tz * e1x) - (e1z * tx);
    float qz = (e1y * tx) - (ty * e1x);

    float px = (e2z * ray->unitVector[1]) - (e2y * ray->unitVector[2]);
    float py = (e2x * ray->unitVector[2]) - (e2z * ray->unitVector[0]);
    float pz = (e2y * ray->unitVector[0]) - (e2x * ray->unitVector[1]);

    float det = (px * e1x) + (e1y * py) + (e1z * pz);

    float uNum;
    float vNum;

    if (det <= bCollisionEpsilon)
    {
        if (det >= -bCollisionEpsilon)
        {
            if (collisionInfo->flags & BMCOLLINFO_RETURNEXTENDEDINFO)
            {
                collisionInfo->extInfo = 0x800;
            }
            return 0;
        }

        uNum = (px * tx) + (ty * py) + (tz * pz);
        if ((uNum > 0.0f) || (uNum < det))
        {
            if (collisionInfo->flags & BMCOLLINFO_RETURNEXTENDEDINFO)
            {
                collisionInfo->extInfo = 0x800;
            }
            return 0;
        }

        vNum = (qz * ray->unitVector[2]) + (qy * ray->unitVector[1]) + (qx * ray->unitVector[0]);
        if ((vNum > 0.0f) || ((vNum + uNum) < det))
        {
            if (collisionInfo->flags & BMCOLLINFO_RETURNEXTENDEDINFO)
            {
                collisionInfo->extInfo = 0x800;
            }
            return 0;
        }
    }
    else
    {
        uNum = (px * tx) + (ty * py) + (tz * pz);
        if ((uNum < 0.0f) || (uNum > det))
        {
            if (collisionInfo->flags & BMCOLLINFO_RETURNEXTENDEDINFO)
            {
                collisionInfo->extInfo = 0x800;
            }
            return 0;
        }

        vNum = (qz * ray->unitVector[2]) + (qy * ray->unitVector[1]) + (qx * ray->unitVector[0]);
        if ((vNum < 0.0f) || ((vNum + uNum) > det))
        {
            if (collisionInfo->flags & BMCOLLINFO_RETURNEXTENDEDINFO)
            {
                collisionInfo->extInfo = 0x800;
            }
            return 0;
        }
    }

    {
        float invDet = 1.0f / det;

        float t = ((e2z * qz) + (e2y * qy) + (e2x * qx)) * invDet;
        if (t < 0.0f)
        {
            if (collisionInfo->flags & BMCOLLINFO_RETURNEXTENDEDINFO)
            {
                collisionInfo->extInfo = 0x200;
            }
            return 0;
        }

        if (t > 1.0f)
        {
            if (collisionInfo->flags & BMCOLLINFO_RETURNEXTENDEDINFO)
            {
                collisionInfo->extInfo = 0x100;
            }
            return 0;
        }

        float u = uNum * invDet;
        float v = vNum * invDet;

        {
            uint32 flags = collisionInfo->flags;

            if (flags & BMCOLLINFO_RETURNTIME)
            {
                collisionInfo->time = t;
            }

            if (flags & BMCOLLINFO_RETURNUV)
            {
                collisionInfo->u = u;
                collisionInfo->v = v;
            }

            if (flags & BMCOLLINFO_RETURNPOSITION)
            {
                float e1xu = e1x * u;
                float e1yu = e1y * u;
                float e1zu = e1z * u;

                float e2xv = e2x * v;
                float e2yv = e2y * v;
                float e2zv = e2z * v;

                collisionInfo->position[0] = (e2xv + e1xu) + tri0->verts[0][0];
                collisionInfo->position[1] = (e2yv + e1yu) + tri0->verts[0][1];
                collisionInfo->position[2] = (e2zv + e1zu) + tri0->verts[0][2];
                collisionInfo->position[3] = 1.0f;
            }

            if (flags & BMCOLLINFO_RETURNNORMAL)
            {
                float local_10;
                float local_c;
                float local_8;
                float local_4;

                local_10 = (e2z * e1y) - (e2y * e1z);
                local_c  = (e1z * e2x) - (e2z * e1x);
                local_8  = (e2y * e1x) - (e1y * e2x);

                {
                    TBVector n;
                    n[0] = local_10;
                    n[1] = local_c;
                    n[2] = local_8;
                    bmVectorNorm(n, n);
                    local_10 = n[0];
                    local_c  = n[1];
                    local_8  = n[2];
                }

                if (det < 0.0f)
                {
                    collisionInfo->normal[0] = -local_10;
                    collisionInfo->normal[1] = -local_c;
                    collisionInfo->normal[2] = -local_8;
                    return 1;
                }

                collisionInfo->normal[0] = local_10;
                collisionInfo->normal[1] = local_c;
                collisionInfo->normal[2] = local_8;
                collisionInfo->normal[3] = local_4;
            }
        }

        return 1;
    }
}


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

extern int bmCollideRayAABB(const TBCollisionRay * const ray, const TBCollisionAABB * const box, TBCollisionInfo * const collisionInfo)
{
        bkPrintf("*** WARNING *** bmCollideRayAABB was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


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

extern int bmCollideSphereTriangle(const TBCollisionSphere * const sphere, const TBCollisionTriangle * const tri, const TBVector movementVector, TBCollisionInfo * const collisionInfo)
{
        bkPrintf("*** WARNING *** bmCollideSphereTriangle was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


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

int bmCollideSphereLine(const TBCollisionSphere * const sphere, const TBCollisionLine * const line, const TBVector movementVector, TBCollisionInfo * const collisionInfo)
{
        bkPrintf("*** WARNING *** bmCollideSphereLine was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


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

int bmCollideSphereMesh(const TBCollisionSphere * const sphere, const TBCollisionMesh * const mesh, const TBVector movementVector, int * const collisionInfoCount, TBCollisionInfo * const collisionInfo)
{
        bkPrintf("*** WARNING *** bmCollideSphereMesh was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


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

int bmCollideRayMesh(const TBCollisionRay * const ray, const TBCollisionMesh * const mesh, int * const collisionInfoCount, TBCollisionInfo * const collisionInfo)
{
        bkPrintf("*** WARNING *** bmCollideRayMesh was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bmCollideRayCylinder
	Purpose : Test for the collision between a ray and a static cylinder.
	Parameters : [in] the ray to test (must be a unit-ray)
				[in] the cylinder to test against.
				[out] a structure giving information abouth the collision.
	Returns : 1 if a collision occured, 0 otherwise.
	Info : 
*/

int bmCollideRayCylinder(const TBCollisionRay * const ray, const TBCollisionCylinder * const cylinder, TBCollisionInfo * const collisionInfo)
{
        bkPrintf("*** WARNING *** bmCollideRayCylinder was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bmCollideRayBox
	Purpose : Collide a ray with an arbitrarily aligned box.
	Parameters : [in] the ray to test against the box.
				[in] the box to test the ray against
				[out] the information about the collision (if there was one)
	Returns : 1 if there was a collision, 0 otherwise
	Info : 
*/

int bmCollideRayBox(const TBCollisionRay * const ray, const TBCollisionBox * const box, TBCollisionInfo * const collInfo)
{
        bkPrintf("*** WARNING *** bmCollideRayBox was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


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

int bmCollideSphereAABB(const TBCollisionSphere * const movingSphere, const TBCollisionAABB * const box, const TBVector movementVector, TBCollisionInfo * const collisionInfo)
{
        bkPrintf("*** WARNING *** bmCollideSphereAABB was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


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

int bmCollideSphereBox(const TBCollisionSphere * const movingSphere, const TBCollisionBox * const box, const TBVector movementVector, TBCollisionInfo * const collisionInfo)
{
        bkPrintf("*** WARNING *** bmCollideSphereBox was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bLoadCollisionMeshByCRC
   Purpose : Load a collision mesh from a package returning a handle to it
   Parameters : package index, texture CRC
   Returns : handle or NULL for failure
   Info : 
*/

TBCollisionMesh *bLoadCollisionMeshByCRC(TBPackageIndex *index, uint32 crc)
{
        bkPrintf("*** WARNING *** bLoadCollisionMeshByCRC was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}


/* --------------------------------------------------------------------------------
   Function : bDeleteCollisionMesh
   Purpose : Delete a collision mesh
   Parameters : collision mesh handle
   Returns : 
   Info : 
*/

void bDeleteCollisionMesh(TBCollisionMesh *handle)
{
        bkPrintf("*** WARNING *** bDeleteCollisionMesh was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


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

int bmCollideTriangleSphere(const TBCollisionTriangle * const tri, const TBCollisionSphere * const sphere, const TBVector movementVector, TBCollisionInfo * const collisionInfo)
{
        bkPrintf("*** WARNING *** bmCollideTriangleSphere was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bIntersectSpherePlane
	Purpose : Collide a Sphere with a Plane.
	Parameters : [in] sphere
				[in] movement vector of sphere
				[in] plane (as in plane eqn. ax + by + cz + d == 0)
	Returns : <0 if behind the plane, 0 if straddling the plane, >0 if in front of the plane.
	Info : Internal because the parameters are not 'nice'
*/

int bIntersectSpherePlane(const TBCollisionSphere * const sphere, const TBVector movementVector, const TBVector plane)
{
        bkPrintf("*** WARNING *** bIntersectSpherePlane was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bIntersectRayPlane
	Purpose : Intersect a Ray with a Plane.
	Parameters : [in] ray
				[in] plane (as in plane eqn. ax + by + cz + d == 0)
	Returns : <0 if behind the plane, 0 if straddling the plane (within collision epsilon), >0 if in front of the plane.
	Info : Internal because the parameters are not 'nice'
*/

int bIntersectRayPlane(const TBCollisionRay * const ray, const TBVector plane)
{
        bkPrintf("*** WARNING *** bIntersectRayPlane was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bIntersectRayPlane_Unit
	Purpose : Intersect a unit ray with a plane.
	Parameters : [in] unit length ray
				[in] plane (as in plane eqn. ax + by + cz + d == 0)
	Returns : <0 if behind the plane, 0 if straddling the plane (within collision epsilon) , >0 if in front of the plane.
	Info : Internal because the parameters are not 'nice'
*/

int bIntersectRayPlane_Unit(const TBCollisionRay * const ray, const TBVector plane)
{
        bkPrintf("*** WARNING *** bIntersectRayPlane_Unit was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bmIntersectRaySphere_Unit
	Purpose : Intersect a ray with a sphere 
	Parameters : [in] the unit length ray
				[in] the sphere
	Returns : whether there was an intersection between the primitives.
	Info : 
*/

int bmIntersectRaySphere_Unit(const TBCollisionRay * const ray, const TBCollisionSphere * const sphere)
{
        bkPrintf("*** WARNING *** bmIntersectRaySphere_Unit was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/*	--------------------------------------------------------------------------------
	Function : bmIntersectRaySphere
	Purpose : Intersect a ray with a sphere 
	Parameters : [in] the ray
				[in] the sphere
	Returns : whether there was an intersection between the primitives.
	Info : 
*/

int bmIntersectRaySphere(const TBCollisionRay * const ray, const TBCollisionSphere * const sphere)
{
        bkPrintf("*** WARNING *** bmIntersectRaySphere was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}