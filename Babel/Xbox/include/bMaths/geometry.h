// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : geometry.h
//   Purpose : some simple, common geometric functions
// Component : Generic Maths
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BMATHS_GEOMETRY_H__
#define __BMATHS_GEOMETRY_H__

/*	--------------------------------------------------------------------------------
	Function : bmGeomWindingOrder2D
	Purpose : Test three 2D vertices for their winding order.
	Parameters : [in] First vector to test
				[in] Second vector to test
				[in] Third vector to test
	Returns : (-1 == Counter-Clockwise) (0 == Coplanar) (1 == Clockwise)
	Info : Derived from "Algorithms" by Robert Sedgewick (Second Edition) Chapter 24 - "Line Segment Intersection"
*/

extern int bmGeomWindingOrder2D(const TBVector2D point0, const TBVector2D point1, const TBVector2D point2);


/*	--------------------------------------------------------------------------------
	Function : bmGeomDistanceFromLine2D
	Purpose : Find the smallest distance from line to point.
	Parameters : [in] The line to test against.
				[in] The point.
				[out] the closest point along the line to the 
	Returns : Distance from the point to the line.
	Info : 
*/

extern float bmGeomDistanceFromLine2D(const TBCollisionLine2D * const line, const TBVector2D point, TBVector2D closestPoint);


/*	--------------------------------------------------------------------------------
	Function : bmGeomLengthAlongVector
	Purpose : Finds the length that v2 travels in the direction of v1 (both positive and negative)
	Parameters : [in] basis vector
				[in] test vector
	Returns : the distance that the test vector travels along the basis vector, relative to the length of the basis vector.
	Info : This function is just 2 dot products. But it's nice to put a wrapper like this so you get the meaning when developing..
*/

float bmGeomLengthAlongVector(const TBVector basis, const TBVector test);


/*	--------------------------------------------------------------------------------
	Function : bmGeomDistanceFromLine
	Purpose : Find the smallest distance from line to point.
	Parameters : [in] The line to test against.
				[in] The point.
				[out] the closest point along the line to the 
	Returns : Distance from the point to the line.
	Info : 
*/

extern float bmGeomDistanceFromLine(const TBCollisionLine * const line, const TBVector point, TBVector closestPoint);


/*	--------------------------------------------------------------------------------
	Function : bmGeomDistanceFromInfiniteLine
	Purpose : Get the perpendicular distance from an infinite line (the start and end points are not used as TERMINATORS of the line)
	Parameters : [in] The line to test against.
				[in] The point.
				[out] the closest point along the line to the 
	Returns : Perpendicular distance from the point to the line.
	Info : 
*/

float bmGeomDistanceFromInfiniteLine(const TBCollisionLine * const line, const TBVector point, TBVector closestPoint);


/*	--------------------------------------------------------------------------------
	Function : bmGeomIntersectLines
	Purpose : Tests for an intersection between line1 & line2. If there was no intersection, but the lines are not parallel,
				returns the point of closest approach (may be outside bounds of the lines)
	Parameters : [in] the first line to test
				[in] the second line to test
				[out] the time along line1 that the closest point was found
				[out] the time along line2 that the closest point was found
	Returns : 0 if the lines are parallel, 1 if the point of closest approach was determined.
	Info : t&s can be out of the range 0-1 (start - end). If this occurs, it is a matter for the user to resolve.
*/

int bmGeomIntersectLines(const TBCollisionLine * const line1, const TBCollisionLine * const line2, float * const t, float * const s);

#endif /* __BMATHS_GEOMETRY_H__ */
