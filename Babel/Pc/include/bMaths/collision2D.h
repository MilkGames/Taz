// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : collision2D.h
//   Purpose : 2D collision associated maths
// Component : Generic Maths
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BMATHS_COLLISION2D_H__
#define __BMATHS_COLLISION2D_H__

typedef struct _TBCollisionPolygon2D
{
	uint32				vertexCount;
	const TBVector2D	*verts;
} TBCollisionPolygon2D;

typedef struct _TBCollisionLine2D
{
	TBVector2D			start;
	TBVector2D			end;
} TBCollisionLine2D;

typedef struct _TBCollisionCircle2D
{
	TBVector2D			center;
	float				radius;
} TBCollisionCircle2D;

// TW: Flags for the collision info struct.
#define BMCOLLINFO2D_RETURNTIME			0x00000001
#define BMCOLLINFO2D_RETURNU			0x00000002
#define BMCOLLINFO2D_RETURNNORMAL		0x00000004
#define BMCOLLINFO2D_RETURNPOSITION		0x00000008
#define BMCOLLINFO2D_RETURNEXTENDEDINFO	0x00000010

typedef struct _TBCollisionInfo2D
{
	TBVector2D					normal;		// TW: The normal of the point of the collision on the primitive
	TBVector2D					position;	// TW: The position that the collision occurred at.

	uint32						flags;		// TW: Bit flags telling the collision functions which values to fill in correctly.
	float						time;		// TW: The time along the movement vector that was passed in that the collision took place
	float						u;			// TW: Surface coordinates on the primitive where the collision took place
	uint32						extInfo;	// Collision routine specific information.
} TBCollisionInfo2D BALIGN16;


/*	--------------------------------------------------------------------------------
	Function : bmCollideLineCircle2D
	Purpose : Check for collision between a circle and a line (effectivley a moving point).
	Parameters : [in] the circle to check against
				[in] the line(point) to check
				[in/out] the information requested/retrieved from the collision (if there was one)
	Returns : Collision(1) / No Collision(0)
	Info : 
*/

extern int bmCollideLineCircle2D(const TBCollisionLine2D * const line, const TBCollisionCircle2D * const circle, TBCollisionInfo2D * const collisionInfo);


/*	--------------------------------------------------------------------------------
	Function : bmCollideLineLine2D
	Purpose : Check for collision between a line and a second line.
	Parameters : [in] the first line to check (also considered as the 'moving point' and the subject of collisionInfo's data)
				[in] the second line to check
				[in/out] the information requested/retrieved from the collision (if there was one)
	Returns : Collision(1) / No Collision(0)
	Info : Derived From Graphics Gems II (Intersection of Line Segments) pg7
*/

extern int bmCollideLineLine2D(const TBCollisionLine2D * const line1, const TBCollisionLine2D * const line2, TBCollisionInfo2D * const collisionInfo);


/*	--------------------------------------------------------------------------------
	Function : bmCollideCircleLine2D
	Purpose : Check for collision between a moving circle and a line.
	Parameters : [in] the moving circle to check
				[in] the line to check against
				[in] the movement vector of the circle.
				[in/out] the information requested/retrieved from the collision (if there was one)
	Returns : Collision(1) / No Collision(0)
	Info : 
*/

extern int bmCollideCircleLine2D(const TBCollisionCircle2D * const circle, const TBCollisionLine2D * const line, const TBVector2D movementVector, TBCollisionInfo2D * const collisionInfo);


/*	--------------------------------------------------------------------------------
	Function : bmCollideLinePolygon2D
	Purpose : Check for collision between a line and a Polygon.
	Parameters : [in] the line to check
				[in] the polygon to check
				[in/out] the information requested/retrieved from the collision (if there was one)
	Returns : Collision(1) / No Collision(0)
	Info : 
*/

extern int bmCollideLinePolygon2D(const TBCollisionLine2D * const line, const TBCollisionPolygon2D * const poly, TBCollisionInfo2D * const collisionInfo);


/*	--------------------------------------------------------------------------------
	Function : bmCollideCirclePolygon2D
	Purpose : Check for collision between a circle and a polygon.
	Parameters : [in] the circle to check
				[in] the polygon to check
				[in] the movementVector of the circle.
				[in/out] the information requested/retrieved from the collision (if there was one)
	Returns : Collision(1) / No Collision(0)
	Info : 
*/

extern int bmCollideCirclePolygon2D(const TBCollisionCircle2D * const circle, const TBCollisionPolygon2D * const poly, const TBVector2D movementVector, TBCollisionInfo2D * const collisionInfo);


#endif /* __BMATHS_COLLISION2D_H__ */