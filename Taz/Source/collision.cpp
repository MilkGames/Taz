
//  -----------------------------------------------------
//  File: collision.cpp
//  -----------------------------------------------------
//  by David Serafim (01/00-03/00)
//  
//  (C) ISL 2000
//  -----------------------------------------------------


#include "global.h"
#include "collision.h"
#include "maths.h"					// PP: Maths stuff
#include "collisioncache.h"

typedef struct TAG_SPHEREPOLYCOLLSTATS
{
	int	numPolysPassed;
	int	numPolysPastBBTest;

} SPHEREPOLYCOLLSTATS;

SPHEREPOLYCOLLSTATS spherePolyCollStats;

// friction values for poly terrain types. 0 = no friction (slippery), 1 = max fric. (sticky)
float terrainTable[2] = { 0.0f, 1.0f };

#ifdef	DEBUGGING
void colourmesh(TBSegmentedMesh *mesh)
{
	int noofTris;
	TBPolyPrim *prim = mesh->primList;
	int l,c;

	for(l = 0; l < mesh->noofPrimitives; l++)
	{
		if (prim->primType == BDPRIMTYPE_TRIANGLELIST)
			noofTris = prim->noofVertices/3;
		else if (prim->primType == BDPRIMTYPE_TRIANGLESTRIP)
			noofTris = prim->noofVertices-2;
		else
			noofTris = 0;
		for(c=0;c<noofTris;c++)
		{
			prim->colours[c*3] = RGB( 255,0,0) + (255L<<24);
			prim->colours[c*3+1] = RGB( 0,255,0) + (255L<<24);
			prim->colours[c*3+2] = RGB( 0,0,255) + (255L<<24);
		}
		prim++;
	}
}

TBPrimVertex		line[20][2];
int linecnt=0;

void addline(TBVector pos,TBVector dir,float scale,int r1,int g1, int b1,int r2,int g2, int b2)
{
	TBVector vec;

	BDVERTEX_SETRGBA(&line[linecnt][0], r1, g1, b1, 255);
	BDVERTEX_SETRGBA(&line[linecnt][1], r2, g2, b2, 255);
	BDVERTEX_SETXYZW(&line[linecnt][0], pos[0], pos[1], pos[2], 1.0f);
	bmVectorScaleToLength(vec,dir,scale);
	BDVERTEX_SETXYZW(&line[linecnt][1], pos[0]+vec[0], pos[1]+vec[1], pos[2]+vec[2], 1.0f);
	linecnt++;
}
#endif

void copyTPrimTVec( COLLVERTEXOLD tpv, TBVector tv)
{
	COLLVERTEX_GETX(&tpv, tv[0]);
	COLLVERTEX_GETY(&tpv, tv[1]);
	COLLVERTEX_GETZ(&tpv, tv[2]);
	tv[0] = tpv.x;
	tv[1] = tpv.y;
	tv[2] = tpv.z;
	tv[3] = 1.0f;
	//tv[0] = tpv.x;
	//tv[1] = tpv.y;
	//tv[2] = tpv.z;
	//tv[3] = 0;
}

inline void findNearest( TBVector nearest, TBVector vec, TBVector vec2, TBVector vert, TBVector vert2, TBVector edge )
{
	float dota,dotb;
	TBVector nedge;

	dota = bmVectorDot(vec,edge);
	if ( dota <= 0.0f )
	{
		nearest[0] = vert[0];
		nearest[1] = vert[1];
		nearest[2] = vert[2];
		nearest[3] = 0.0f;
		return;
	}
	bmVectorNegate(nedge,edge);
	dotb = bmVectorDot(vec2,nedge);
	if ( dotb <= 0.0f )
	{
		nearest[0] = vert2[0];
		nearest[1] = vert2[1];
		nearest[2] = vert2[2];
		nearest[3] = 0.0f;
		return;
	}
	dotb = 1.0f / (dotb + dota);
	nearest[0] = vert[0] + (edge[0] * dota) * dotb;
	nearest[1] = vert[1] + (edge[1] * dota) * dotb;
	nearest[2] = vert[2] + (edge[2] * dota) * dotb;
	nearest[3] = 0.0f;
}

/*  --------------------------------------------------------------------------------
	Function	: ResetCollStats
	Purpose		: reset the stats counters
	Parameters	: 
	Returns		: 
	Info		: 
*/
void ResetCollStats(void)
{
	spherePolyCollStats.numPolysPassed = 0;
	spherePolyCollStats.numPolysPastBBTest = 0;
}


/*	--------------------------------------------------------------------------------
	Function : SphereFaceCollision
	Purpose : test if the sphere collided with mesh and return a list of faces (triangles) if so.
	Parameters :vertlist = pointer to a list of tprimvertex structures containing
						   mesh vertices info.
				vertindices = face vertex indices.
				noofvertices = total number of vertices in mesh
				radius = sphere radius.
				pos = sphere position in world coordinates.
				collfacelist = array of COLLFACEINFO that will be filled with faces
							   from mesh that collided with sphere. this array must be
							   allocated by the caller.
				maxfaces = maximum number of faces to return in collfacelist.
	Returns : 
	Info : unrolled loops for better performance
*/

void SphereFaceCollision( COLLISIONLISTENTRY *meshlist, TBVector pos, float radius,
						    COLL_FACEINFO collFaceList[], int *maxFaces )
{
	int			collFaces = 0;
	TBVector	verts[3];
	TBVector	nearest[3];
	TBVector	edges[3];
	TBVector	vecs[3];
	int32		vertIndices=0;
	int			findex = 0;
	int			fc;
	TBVector	facenorm;
	float		nearMag;
	float		m,m2,m3;
	TBVector	vec;
	float		totang;
	float		distToPlane;
	int			vc;
	COLLVERTEXOLD	*vertPtr;
	float		tempDot;						// PP: temporary dot-product result
	COLLPOLY	*poly;

#ifdef USE_SHORTS_FOR_COLLISION
	short		posMin[3],posMax[3];
	TBVector	posMinTemp,posMaxTemp;
#else
	register TBVector	posMin,posMax;
#endif

#ifdef USE_SHORTS_FOR_COLLISION
	// IH: Scale Taz bounding box down and 
	bmVectorCSub(posMinTemp,pos,radius);
	bmVectorCAdd(posMaxTemp,pos,radius);

	posMin[X]=(((short)posMinTemp[X])>>3)-1;
	posMin[Y]=(((short)posMinTemp[Y])>>3)-1;
	posMin[Z]=(((short)posMinTemp[Z])>>3)-1;

	posMax[X]=(((short)posMaxTemp[X])>>3)+1;
	posMax[Y]=(((short)posMaxTemp[Y])>>3)+1;
	posMax[Z]=(((short)posMaxTemp[Z])>>3)+1;
#else
	bmVectorCSub(posMin,pos,radius);
	bmVectorCAdd(posMax,pos,radius);
#endif

	spherePolyCollStats.numPolysPassed += meshlist->numPolys;		

	poly = meshlist->polys;

	for (fc = meshlist->numPolys-1;fc>=0;fc--,findex++,poly++)
	{
		// bounding box collision
		if ((posMax[X] < poly->minx)||(posMax[Z] < poly->minz)||(posMax[Y] < poly->miny))
		{
			continue;
		}

		// TP: check min bbox test passes
		if ((posMin[X] > poly->maxx)||(posMin[Z] > poly->maxz)||(posMin[Y] > poly->maxy))
		{
			continue;
		}

		spherePolyCollStats.numPolysPastBBTest++;		

		nearMag=1000000.0f;

		// TP: Get first vertex
		COLLPOLY_GETV1(poly[0], meshlist->vertices, verts[0][X], verts[0][Y], verts[0][Z], verts[0][W]);

		// allways get normal from first vertex in poly
		COLLPOLY_GETNORMAL(poly[0],facenorm[X],facenorm[Y],facenorm[Z]);

		distToPlane = bmVectorDot(verts[0],facenorm) - bmVectorDot(pos,facenorm);

		if (distToPlane > 0.0f)
		{
			continue;
		}

		m = bmFAbs(distToPlane);

		// only consider polys that face us and are at smaller distance than sphere radius
		if (m > radius)
		{
			continue;
		}

		COLLPOLY_GETV2(poly[0], meshlist->vertices, verts[1][X], verts[1][Y], verts[1][Z], verts[1][W]);
		COLLPOLY_GETV3(poly[0], meshlist->vertices, verts[2][X], verts[2][Y], verts[2][Z], verts[2][W]);

		bmVectorCMul(vec, facenorm, distToPlane);
		bmVectorAdd(vec, vec, pos);

		bmVectorSub(vecs[0],vec,verts[0]);
		bmVectorNorm(vecs[0],vecs[0]);

		bmVectorSub(vecs[1],vec,verts[1]);
		bmVectorNorm(vecs[1],vecs[1]);

		bmVectorSub(vecs[2],vec,verts[2]);
		bmVectorNorm(vecs[2],vecs[2]);

		DOT_UNIT(tempDot, vecs[0],vecs[2]);// PP: clamp dot product into -1..1 (might otherwise be slightly out because of inaccuracies - would be invalid input for bmACos)
		totang = bmACos(tempDot);

		DOT_UNIT(tempDot, vecs[1],vecs[0]);// PP: clamp dot product into -1..1 (might otherwise be slightly out because of inaccuracies - would be invalid input for bmACos)
		totang += bmACos(tempDot);

		DOT_UNIT(tempDot, vecs[2],vecs[1]);// PP: clamp dot product into -1..1 (might otherwise be slightly out because of inaccuracies - would be invalid input for bmACos)
		totang += bmACos(tempDot);
		
		// is center of sphere within poly surface?
		// DaveS ate all the pies
		if ( totang >= 6.28318f) // carefull, the inaccuracy of the pie is crucial to a satisfactory collision	// PP: Mummy I'm scared!  Am I going to fall through the ground Mummy?  Hold my hand Mummy!  I don't want to fall down into the devil place!
		{
			collFaceList[collFaces].collType = COLLTYPE_FACE;
			bmVectorCopy(collFaceList[collFaces].nearestPoint,vec);
			bmVectorCopy(collFaceList[collFaces].npointNormal,facenorm);
			bmVectorCopy(collFaceList[collFaces].faceNormal,facenorm);
			collFaceList[collFaces].index = findex;
			// index value for terrainTable should be REPLACED by poly terrain type
			// when available
			collFaceList[collFaces++].polyFriction = terrainTable[0];
			nearMag = m;
		}
		else
		{
			// edge collision
			bmVectorSub(edges[0],verts[1],verts[0]);
			bmVectorSub(edges[1],verts[2],verts[1]);
			bmVectorSub(edges[2],verts[0],verts[2]);
			bmVectorSub(vecs[0],pos,verts[0]);
			bmVectorSub(vecs[1],pos,verts[1]);
			bmVectorSub(vecs[2],pos,verts[2]);
			findNearest(nearest[0],vecs[0],vecs[1],verts[0],verts[1],edges[0]);
			findNearest(nearest[1],vecs[1],vecs[2],verts[1],verts[2],edges[1]);
			findNearest(nearest[2],vecs[2],vecs[0],verts[2],verts[0],edges[2]);
			bmVectorSub(vec,nearest[0],pos);
			m = bmVectorLen(vec);
			bmVectorSub(vec,nearest[1],pos);
			m2 = bmVectorLen(vec);
			bmVectorSub(vec,nearest[2],pos);
			m3 = bmVectorLen(vec);
			vc = -1;
			if ( m <= radius && m < nearMag )
			{
				nearMag = m;
				vc = 0;
			}
			if ( m2 <= radius && m2 < nearMag )
			{
				nearMag = m2;
				vc = 1;
			}
			if ( m3 <= radius && m3 < nearMag )
			{
				nearMag = m3;
				vc = 2;
			}

			if ( vc >= 0 )
			{
				collFaceList[collFaces].collType = COLLTYPE_EDGE;
				bmVectorCopy(collFaceList[collFaces].nearestPoint,nearest[vc]);
				bmVectorSub(collFaceList[collFaces].npointNormal,pos,nearest[vc]);
				bmVectorNorm(collFaceList[collFaces].npointNormal,collFaceList[collFaces].npointNormal);
				bmVectorCopy(collFaceList[collFaces].faceNormal,facenorm);
				collFaceList[collFaces].index = findex;
				// index value for terrainTable should be REPLACED by poly terrain type
				// when available
				collFaceList[collFaces++].polyFriction = terrainTable[0];
			}
		}

//		if ( nearMag < 1000000.0f )
//			collFaces++;

		if ( collFaces == *maxFaces )
			break;
	}

	*maxFaces = collFaces;
}

/*	--------------------------------------------------------------------------------
	Function : CirclePlaneCollision
	Purpose : find nearest circle point in polygon
	Parameters :vert = trangle vertices
				radius = circle radius.
				pos = circle position in world coordinates.
				ignoreabove = will ignore any collision points that are above pos point
	Returns : TRUE - had collision / FALSE - no collision
	Info : if radius equals 0 than it's ignored
*/
/*
BOOL CirclePlaneCollision( COLLVERTEXOLD *cvert,TBVector origpos, float radius, bool ignoreabove,float *newposy)
{
	float totang,scale;
	TBVector max,min;
	TBVector vecs[3];
	TBVector norm;
	TBVector pos;
	TBVector tnorm;
	TBVector	newpos;
	TBVector vert[3];

	COLLPOLY_GETNORMAL(&cvert[0],norm[X],norm[Y],norm[Z]);
	
	if (ignoreabove)
	{
		if (norm[1] <= 0.0f)
			return NULL;
	}
	else
	{
		if ( norm[1] == 0.0f )
			return NULL;
	}
	// bounding box test if no radius
	if ( radius == 0.0f )
	{
		COLLVERTEX_GETNORMAL(&cvert[1],min[X],min[Y],min[Z]);
		COLLVERTEX_GETNORMAL(&cvert[2],max[X],max[Y],max[Z]);
		if ( !(origpos[0]-radius <= max[0] && origpos[0]+radius >= min[0] &&
			   origpos[2]-radius <= max[2] && origpos[2]+radius >= min[2] ) )
			return NULL;
	}
	COLLVERTEX_GETXYZW(&cvert[0],vert[0][X],vert[0][Y],vert[0][Z],vert[0][W]);
	COLLVERTEX_GETXYZW(&cvert[1],vert[1][X],vert[1][Y],vert[1][Z],vert[1][W]);
	COLLVERTEX_GETXYZW(&cvert[2],vert[2][X],vert[2][Y],vert[2][Z],vert[2][W]);

	bmVectorCopy(pos,origpos);

	if ( radius > 0.0f )
	{
		// find circle closest point to polygon surface
		scale = bmVectorDot(vert[0],norm) - bmVectorDot(pos,norm);
		if ( (ignoreabove && scale > 0.0f) || -scale > radius )
		{
//			FREE(newpos);
			return FALSE;
		}

		memset(tnorm,0,sizeof(tnorm));
		if ( norm[0] )
			tnorm[0] = (float)bmFAbs(scale)*(1.0f/norm[0])/2.0f;
		if ( norm[2] )
			tnorm[2] = (float)bmFAbs(scale)*(1.0f/norm[2])/2.0f;

		bmVectorSub(pos,pos,tnorm);
#ifdef	DEBUGGING
//		addline(pos,tnorm,200, 0,0,0, 255,255,0 );
#endif
		bmVectorScaleToLength(tnorm,tnorm,radius);
		bmVectorAdd(newpos,pos,tnorm);
	}
	else
		bmVectorCopy(newpos,pos);

	// is circle point within polygon surface?
//	memset(vecs,0,sizeof(vecs[0])*3);

	bmVectorSub(vecs[0],pos,vert[0]);
	vecs[0][1] = 0;
	bmVectorNorm(vecs[0],vecs[0]);

	bmVectorSub(vecs[1],pos,vert[1]);
	vecs[1][1] = 0;
	bmVectorNorm(vecs[1],vecs[1]);
	
	bmVectorSub(vecs[2],pos,vert[2]);
	vecs[2][1] = 0;
	bmVectorNorm(vecs[2],vecs[2]);
	
	totang = (float)bmACos(bmVectorDot(vecs[0],vecs[2]));
	totang += (float)bmACos(bmVectorDot(vecs[1],vecs[0]));
	totang += (float)bmACos(bmVectorDot(vecs[2],vecs[1]));
	if ( totang < 3.14159f*2.0f )
	{
//		FREE(newpos);
		return FALSE;
	}

	// find the right altitude
	pos[1] = 0.0f;
	pos[0] = -pos[0];
	pos[2] = -pos[2];
	newpos[Y] = (bmVectorDot(pos,norm) + bmVectorDot(vert[0],norm)) / norm[1];

	*newposy = newpos[Y];

	return TRUE;
}
*/

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
bool RayTriangleCollision(COLLISIONLISTENTRY *meshList, TBVector start, TBVector end, TBVector intersect, BOUNDINGBOX *testBox)
{
	bool					hit=false;				// PP: did we hit any of the triangles?
	float					minTime;				// PP: the min time (distance) to a collision
	TBCollisionRay			ray;					// PP: collision ray between start & end points
	TBCollisionTriangle		tri;					// PP: collision triangle
	TBCollisionInfo			info;					// PP: collision info
	int						fc = meshList->numPolys;
	COLLPOLY				*poly;

	// PP: SET UP COLLISION RAY
	bmVectorCopy(ray.start, start);
	ray.start[W]=1.0f;
	bmVectorSub(ray.unitVector, end, start);// PP: doesn't need to be unit
	ray.unitVector[W]=1.0f;
	ray.length=0.0f;// PP: ignored

	info.flags=(BMCOLLINFO_RETURNPOSITION|BMCOLLINFO_RETURNTIME);

	// IH: Are we using the small collision stuff??
#ifdef USE_SHORTS_FOR_COLLISION
	short		posMin[3],posMax[3];

	if (testBox)
	{
		posMin[X]=(((short)testBox->xmin)>>3)-1;
		posMin[Y]=(((short)testBox->ymin)>>3)-1;
		posMin[Z]=(((short)testBox->zmin)>>3)-1;

		posMax[X]=(((short)testBox->xmax)>>3)+1;
		posMax[Y]=(((short)testBox->ymax)>>3)+1;
		posMax[Z]=(((short)testBox->zmax)>>3)+1;
	}

	for (fc = 0; fc < meshList->numPolys;fc++)
	{
		// bounding box collision
		if (testBox)
		{
			poly = &meshList->polys[fc];
			if ((posMax[X] < poly->minx)||(posMax[Z] < poly->minz)||(posMax[Y] < poly->miny))
			{
				continue;
			}

			// TP: check min bbox test passes
			if ((posMin[X] > poly->maxx)||(posMin[Z] > poly->maxz)||(posMin[Y] > poly->maxy))
			{
				continue;
			}
		}

#else

	for (fc = 0; fc < meshList->numPolys;fc++)
	{
		// bounding box collision
		if (testBox)
		{
			poly = &meshList->polys[fc];
			if ((testBox->xmax < poly->minx)||(testBox->zmax < poly->minz)||(testBox->ymax < poly->miny))
			{
				continue;
			}

			// TP: check min bbox test passes
			if ((testBox->xmin > poly->maxx)||(testBox->zmin > poly->maxz)||(testBox->ymin > poly->maxy))
			{
				continue;
			}
		}
#endif
		// PP: SET UP COLLISION TRIANGLE
		COLLPOLY_GETV1(meshList->polys[fc], meshList->vertices, tri.verts[0][X], tri.verts[0][Y], tri.verts[0][Z], tri.verts[0][W]);
		COLLPOLY_GETV2(meshList->polys[fc], meshList->vertices, tri.verts[1][X], tri.verts[1][Y], tri.verts[1][Z], tri.verts[1][W]);
		COLLPOLY_GETV3(meshList->polys[fc], meshList->vertices, tri.verts[2][X], tri.verts[2][Y], tri.verts[2][Z], tri.verts[2][W]);

		// PP: PERFORM COLLISION TEST
		if(bmCollideRayTriangle(&ray, &tri, &info))
		{
			if(!hit)
			{
				// PP: first time we've hit something - this is therefore the closest hit so far
				minTime=info.time;
				hit=true;
			}
			else
			{
				if(info.time < minTime)
				{
					// PP: this is the closest hit so far
					minTime=info.time;
				}
			}

			// PP: fill in intersect point
			bmVectorCopy(intersect, info.position);
		}
	}

	return hit;
}


/*	--------------------------------------------------------------------------------
	Function : CircleFaceCollision
	Purpose : find nearest circle point in polygons
	Parameters :vertlist = pointer to a list of tprimvertex structures containing
						   mesh vertices info.
				vertindices = face vertex indices.
				noofvertices = total number of vertices in mesh
				pos = sphere position in world coordinates.
				radius = sphere radius.
				ignoreabove = will ignore any collision points that are above pos point
	Returns : TRUE - had collision / FALSE - no collision
	Info : 
*/
#if 0
/*
BOOL CircleFaceCollision( COLLVERTEXOLD *vertlist, int noofVertices, TBVector pos,
						    float radius, bool ignoreabove ,float *newposy)
{
	float np;
	int		value=0;
	TBVector	newpos;

	int32 vertIndices=0;
	int fc = noofVertices/3;

	while ( fc-- )
	{
/*		COLLVERTEX_GETNORMAL(&vertlist[vertIndices],norm[X],norm[Y],norm[Z]);
		for(int v=0;v<3;v++)
		{
			COLLVERTEX_GETXYZW(&vertlist[vertIndices],verts[v][X],verts[v][Y],verts[v][Z],w);
			vertIndices++;
		}
*/		if ( CirclePlaneCollision(&vertlist[vertIndices+=3],pos,radius,ignoreabove,&np))
		{
			if ( !ignoreabove || np <= pos[1] )
			{
				if (value==0)
				{
					newpos[Y] = np;
					*newposy = newpos[Y];
					value=1;
				}
				else 
				{
					if ( np > newpos[Y] )
					{
						newpos[Y] = np;
						*newposy = newpos[Y];
					}
				}
			}
		}
	}

	return value;
}
*/
#endif
/*	--------------------------------------------------------------------------------
	Function : CylinderCylinderCollision
	Purpose : collision between two cylinders
	Parameters : two cylinder structures
	Returns : 
	Info : 
*/

int CylinderCylinderCollision( COLL_CYLINDER *cyl1, COLL_CYLINDER *cyl2 ,float *horzIntersection,float *vertIntersection)
{
	float		cyl1HalfHeight = cyl1->height/2;
	float		cyl2HalfHeight = cyl2->height/2;
	float		cyl1Top = cyl1HalfHeight + cyl1->pos[Y];
	float		cyl2Top = cyl2HalfHeight + cyl2->pos[Y];
	float		cyl1Bottom = cyl1->pos[Y] - cyl1HalfHeight;
	float		cyl2Bottom = cyl2->pos[Y] - cyl2HalfHeight;
	float		dist;
	TBVector	tempVector,pos1,pos2;

	if (cyl1Top < cyl2Bottom) return FALSE;
	if (cyl1Bottom > cyl2Top) return FALSE;

	bmVectorCopy(pos1,cyl1->pos);
	pos1[Y] = 0.0f;
	bmVectorCopy(pos2,cyl2->pos);
	pos2[Y] = 0.0f;
		
	bmVectorSub(tempVector,pos1,pos2);
	dist = bmVectorLen(tempVector);

	if (dist < (cyl1->radius + cyl2->radius))
	{
		*horzIntersection = (cyl1->radius + cyl2->radius) - dist;
		if (cyl1->pos[Y] > cyl2->pos[Y])
		{
			*vertIntersection = cyl2Top - cyl1Bottom;
		}
		else
		{
			*vertIntersection = cyl1Top - cyl2Bottom;
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*	--------------------------------------------------------------------------------
	Function : SphereSphereCollision
	Purpose : collision between two spheres
	Parameters : two sphere structures
	Returns : 
	Info : 
*/

float SphereSphereCollision(COLL_SPHERE *sphere1, COLL_SPHERE *sphere2)
{
	float		distBetSph,totRadius;
	TBVector	tempVector;
	
	totRadius = sphere1->radius + sphere2->radius;

	bmVectorSub(tempVector,sphere1->pos, sphere2->pos);
	
	distBetSph = bmVectorLen(tempVector);

	if (distBetSph < totRadius)
	{
		return (totRadius-distBetSph);	// return intersection distance
	}
	else
	{
		return 0.0f;
	}
}

/*	--------------------------------------------------------------------------------
	Function : SphereCylinderCollision
	Purpose : collision between a sphere and a cylinder
	Parameters : the sphere position and radius and a cylinder structure
	Returns : 
	Info : actually this is a sphere-capsule collision rotine (which ends up being the same thing)
*/

float SphereCylinderCollision( TBVector pos, float radius, COLL_CYLINDER *cyl )
{
	TBVector tvec;
	float cylh = cyl->height/2;
	
	if ( pos[1] > cyl->pos[1]+cylh )
		tvec[1] = cyl->pos[1]+cylh;
	else if ( pos[1] < cyl->pos[1]-cylh )
		tvec[1] = cyl->pos[1]-cylh;
	else
		tvec[1] = pos[1];
	tvec[0] = cyl->pos[0];
	tvec[2] = cyl->pos[2];
	SphereSphereCollision(pos, radius, tvec, cyl->radius);

	return SphereSphereCollision(pos, radius, tvec, cyl->radius);
}

/*	--------------------------------------------------------------------------------
	Function : SphereSphereCollision
	Purpose : collision between two spheres
	Parameters : two sphere positions and radius
	Returns : 
	Info : 
*/

float SphereSphereCollision( TBVector pos, float radius, TBVector pos2, float radius2 )
{
	TBVector tvec;
	bmVectorSub(tvec,pos,pos2);
	float avradius = radius + radius2;
	float vlen = bmVectorLen(tvec);
	
	if ( vlen > avradius )
		return 0;

	return avradius - vlen;
}

