#ifndef __H_COLLISION
#define __H_COLLISION

//  -----------------------------------------------------
//  File: collision.h
//  -----------------------------------------------------
//  by David Serafim (01/00-03/00)
//  
//  (C) ISL 2000
//  -----------------------------------------------------


#include "maths.h"					// PP: maths stuff


#define		COLLTYPE_NONE		0
#define		COLLTYPE_VERTEX		1
#define		COLLTYPE_EDGE		2
#define		COLLTYPE_FACE		3

// IH: TEMPORARY: Just do this for GameCube
#if BPLATFORM==GAMECUBE
#define		USE_SHORTS_FOR_COLLISION
#endif

// this is the structure that SphereMeshCollision/SphereFaceCollision use
typedef struct TAG_COLL_FACEINFO
{
	TBVector		nearestPoint;	// absolute position of nearest point to sphere center

	TBVector		npointNormal;	// normal of face or edge
	
	TBVector		faceNormal;		// face normal
	
	int32			collType;		// type of collision (currently COLLTYPE_FACE or COLLTYPE_EDGE)
	float			polyFriction;	// this polygons friction
	int32			index;			// index of poly within mesh
	uint32			pad;
} COLL_FACEINFO, _COLL_FACEINFO BALIGN16;

enum ECylinderType
{
	CYLINDER_FLATTOP,
};

typedef struct TAG_COLL_CYLINDER
{
	TBVector		pos;

	float		height;
	float		radius;
	int32		type;				// TP: Type of cylinder, CYLINDER_FLATTOP/CYLINDER_ROUNDTOP
	uint32		pad[1];
} COLL_CYLINDER, _COLL_CYLINDER BALIGN16;

typedef struct TAG_COLL_SPHERE
{
	TBVector	pos;

	float		radius;
	uint32		flags;
	uint32		pad[2];
} COLL_SPHERE, _COLLSPHERE BALIGN16;

typedef struct TAG_COLLPOLY
{
	short	i1,i2,i3,pad;			// TP: vert indices
	float	nx,ny,nz;			// TP: poly normal
#ifdef USE_SHORTS_FOR_COLLISION
	short	minx,miny,minz;		// TP: min bounding box
	short	maxx,maxy,maxz;		// TP: max bounding box
#else
	float	minx,miny,minz;		// TP: min bounding box
	float	maxx,maxy,maxz;		// TP: max bounding box
#endif
} COLLPOLY;

typedef struct TAG_COLLVERTEX
{
	float	x,y,z;
} COLLVERTEX;

typedef struct TAG_COLLVERTEXOLD
{
	float	x,y,z;
	float	nx,ny,nz;
} COLLVERTEXOLD;

#define COLLPOLY_GETV1(poly, vertex, sx, sy, sz, sw) { sx = vertex[poly.i1].x; \
														sy = vertex[poly.i1].y; \
														sz = vertex[poly.i1].z; \
														sw = 1.0f; }
#define COLLPOLY_SETV1(poly, vertex, sx, sy, sz, sw) { vertex[poly.i1].x = sx; \
														vertex[poly.i1].y = sy; \
														vertex[poly.i1].z = sz; }

#define COLLPOLY_GETV2(poly, vertex, sx, sy, sz, sw) { sx = vertex[poly.i2].x; \
														sy = vertex[poly.i2].y; \
														sz = vertex[poly.i2].z; \
														sw = 1.0f; }
#define COLLPOLY_SETV2(poly, vertex, sx, sy, sz, sw) { vertex[poly.i2].x = sx; \
														vertex[poly.i2].y = sy; \
														vertex[poly.i2].z = sz; }

#define COLLPOLY_GETV3(poly, vertex, sx, sy, sz, sw) { sx = vertex[poly.i3].x; \
														sy = vertex[poly.i3].y; \
														sz = vertex[poly.i3].z; \
														sw = 1.0f; }
#define COLLPOLY_SETV3(poly, vertex, sx, sy, sz, sw) { vertex[poly.i3].x = sx; \
														vertex[poly.i3].y = sy; \
														vertex[poly.i3].z = sz; }

#define COLLPOLY_SETNORMAL(poly, x, y, z)		{ poly.nx = x; \
												  poly.ny = y; \
												  poly.nz = z; }
#define COLLPOLY_GETNORMAL(poly, x, y, z)		{ x = poly.nx; \
												  y = poly.ny; \
												  z = poly.nz; }

#define COLLPOLY_SETMIN(poly, x, y, z)			{ poly.minx = x; \
													poly.miny = y; \
													poly.minz = z; }

#define COLLPOLY_SETMAX(poly, x, y, z)			{ poly.maxx = x; \
													poly.maxy = y; \
													poly.maxz = z; }

#define COLLPOLY_GETMIN(poly, x, y, z)			{ x = poly.minx; \
													y = poly.miny; \
													z = poly.minz; }
#define COLLPOLY_GETMAX(poly, x, y, z)			{ x = poly.maxx; \
													y = poly.maxy; \
													z = poly.maxz; }

#define COLLVERTEX_GETX(vertex, sx)					  sx = (vertex)->x;

#define COLLVERTEX_GETY(vertex, sy)					  sy = (vertex)->y;

#define COLLVERTEX_GETZ(vertex, sz)					  sz = (vertex)->z;

/*
#define COLLVERTEX_GETXYZW(vertex, sx, sy, sz, sw)	{ sx = (vertex)->x; \
												      sy = (vertex)->y; \
												      sz = (vertex)->z; \
												      sw = 1.0f; }
#define COLLVERTEX_SETXYZW(vertex, sx, sy, sz, sw)	{ (vertex)->x = sx; \
													  (vertex)->y = sy; \
												      (vertex)->z = sz; }
#define COLLVERTEX_SETNORMAL(vertex, x, y, z)		{ (vertex)->nx = x; \
												  (vertex)->ny = y; \
												  (vertex)->nz = z; }
#define COLLVERTEX_GETNORMAL(vertex, x, y, z)		{ x = (vertex)->nx; \
												  y = (vertex)->ny; \
												  z = (vertex)->nz; }
#define COLLVERTEX_GETX(vertex, sx)					  sx = (vertex)->x;

#define COLLVERTEX_GETY(vertex, sy)					  sy = (vertex)->y;

#define COLLVERTEX_GETZ(vertex, sz)					  sz = (vertex)->z;
*/
#define ROOTNODE(pointer) (pointer)


// function declarations
void SphereFaceCollision( struct TAG_COLLISIONLISTENTRY *meshlist, TBVector pos, float radius,
						    COLL_FACEINFO collFaceList[], int *maxFaces );
BOOL CirclePlaneCollision( TBVector *vert, TBVector norm, TBVector origpos, float radius, bool ignoreabove,float *newpos);

BOOL CircleFaceCollision( COLLVERTEXOLD *vertlist, int noofVertices, TBVector pos,
						    float radius, bool ignoreabove ,float *newpos);

/*  --------------------------------------------------------------------------------
	Function	: FillInMinMaxAndNormal
	Purpose		: Calcs the min max values and normal if needed and stores them in the COLLVERTEXOLD structure
	Parameters	: vertices, noofTriangles, true/false to recalulate normals
	Returns		: 
	Info		: 
*/
void FillInMinMaxAndNormal(COLLVERTEX *vertices, COLLPOLY *polys, int noofTriangles, bool recalcNorms);

/*  --------------------------------------------------------------------------------
	Function	: ResetCollStats
	Purpose		: reset the stats counters
	Parameters	: 
	Returns		: 
	Info		: 
*/
void ResetCollStats(void);

/*	--------------------------------------------------------------------------------
	Function : RayTriangleCollision
	Purpose : find closest point of intersection of a ray with a bunch of triangles
	Parameters :vertlist = pointer to a list of tprimvertex structures containing
						   mesh vertices info.
				noofvertices = total number of vertices in mesh
				start of ray,
				end of ray,
				intersect point to be filled in
	Returns : true - had collision / false - no collision
	Info : // PP: 
*/
bool RayTriangleCollision(struct TAG_COLLISIONLISTENTRY *meshlist, TBVector start, TBVector end, TBVector intersect, struct TAG_BOUNDINGBOX *testBox = NULL);


/*	--------------------------------------------------------------------------------
	Function : CylinderCylinderCollision
	Purpose : collision between two cylinders
	Parameters : two cylinder structures
	Returns : 
	Info : 
*/

int CylinderCylinderCollision( COLL_CYLINDER *cyl1, COLL_CYLINDER *cyl2 ,float *horzIntersection,float *vertIntersection);
/*	--------------------------------------------------------------------------------
	Function : SphereSphereCollision
	Purpose : collision between two spheres
	Parameters : two sphere structures
	Returns : 
	Info : 
*/

float SphereSphereCollision(COLL_SPHERE *sphere1, COLL_SPHERE *sphere2);

float SphereCylinderCollision( TBVector pos, float radius, COLL_CYLINDER *cyl );
float SphereSphereCollision( TBVector pos, float radius, TBVector pos2, float radius2 );


/*	--------------------------------------------------------------------------------
	Function 	: collideLinePlane
	Purpose 	: see if a finite line intersects a plane (in either direction)
	Parameters 	: line source, line dest, point on plane, normal of plane 
	Returns 	: whether or not the line intersects the plane (in either direction)
	Info 		: Three dot products and not a ha'penny more.
					This adheres to Babel naming conventions.
					It's inline, yes.  I'm never completely sure when to inline and
					when not to, but this function is used to reject landscape polys
					during realtime shadow casting, so I suppose it has to be shit-fast,
					doncha think?  Anything I can do to make it faster has got
					to be a good thing.  If I'm wrong, feel free to whip me.
					And besides, it's not like it's a towering bulk of code, is it?
*/
inline bool intersectRayPlane(const VEC& src, const VEC& dest, const VEC& point, const VEC& norm)
{
	float				pdist; // Where the plane is,
	float				sdist; // How far away from the plane we start
	float				edist; // The how far away from the plane we end
	int				slt, elt; // start & end less than plane

	pdist = dotProduct(point, norm); // Distance from origin to plane along normal.
	edist = dotProduct(dest, norm); // end distance from origin along normal.
	sdist = dotProduct(src, norm); // startend distance from origin along normal.

	// Use these to check if the points are on the same side of the plane
	slt = sdist < pdist;
	elt = edist < pdist;

	return (slt != elt);
}


#endif