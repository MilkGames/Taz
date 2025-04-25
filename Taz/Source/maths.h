// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Maths.h
//   Purpose : Maths stuff
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_MATHS
#define __H_MATHS

// PP: Babel's random number range - is this defined somewhere in Babel itself?
#define RANDMAX								32767.0f

// PP: good old ABS - I can't believe we haven't had this defined till now (?!)
#define ABS(_val)							(((_val)>=0)?(_val):(-(_val)))

// PP: Square macro, cos I'm very lazy.  Too lazy to spell 'because' properly for a start.
#define SQR(_val)							((_val)*(_val))

// PP: The square root of 2 - always handy
#define SQRT2								1.4142135623730950488016887242097f

// PP: Hypotenuse length from lengths of other two sides of a right-angled triangle
#define HYPOT(_a, _b)						(bmSqrt(SQR(_a)+SQR(_b)))

// PP: Convert an angle in the range 0..TWOPI into the range -PI..PI
#define FORCE_SIGN(_ang)					bmAngularLimit(_ang)// PP: REMOUT: bmAngularLimit might be faster		((_ang>PI)?(0.0f-(TWOPI-(_ang))):(_ang))

// PP: Random angle between 0.0f and TWOPI
#define RANDOMANG							(bmRandf()*TWOPI)

// PP: Maximum signed float value (32 bit)
#define FLOATMAX							(3.402823466e+38F)

// PP: Minimum signed float value (32 bit)
#define FLOATMIN							(1.175494351e-38F)

// PP: Maximum signed int value (32 bit)
#define INTMAX								2147483647			

// PP: Minimum signed int value (32 bit)
#define INTMIN								(-2147483648)

// PP: Maximum TBVector
#define VECMAX								{FLOATMAX, FLOATMAX, FLOATMAX, 0.0f}

// PP: Minimum TBVector
#define VECMIN								{FLOATMIN, FLOATMIN, FLOATMIN, 0.0f}

#define METERTOUNIT(METER)					(METER*250.0f)
#define UNITTOMETER(UNIT)					(UNIT*0.004f)

// PP: centimetres to units
#define CMTOUNIT(_centimetres)				(_centimetres*2.5f)

// PP: units to centimetres
#define UNITTOCM(_units)					(_units*0.4f)

#define ZEROVECTOR(vec)						bmVectorZero(vec)// PP: REMOUT: bmVectorZero can be faster		{vec[X]=0.0f;vec[Y]=0.0f;vec[Z]=0.0f;vec[W]=1.0f;}

#define SETSIGN(a,b)						(a = (b>=0 ? (a<0?-a:a) : (a>=0?-a:a)))

#define OPPOSESIGN(a)						(a = (-a>=0 ? (a<0?-a:a) : (a>=0?-a:a)))

// PP: dot-product of two vectors, with the result clamped into the range -1.0f..1.0f inclusive (ONLY SUITABLE FOR UNIT VECTORS, generally speaking)
#define DOT_UNIT(_dot, _vec1, _vec2)		_dot=bmVectorDot(_vec1, _vec2); \
											if((_dot)>1.0f) \
											{ \
												(_dot)=1.0f; \
											} \
											else if((_dot)<(-1.0f)) \
											{ \
												(_dot)=(-1.0f); \
											}

// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ CONSTRAINT MACROS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* PP: CONSTRAIN evaluates to:
								_val if _val lies between _min and _max,
								_max if _val is greater than _max,
								or _min if _val is less than _min
*/

#define CONSTRAIN(_val, _min, _max)			(																							\
												((_val)>(_max))																			\
																?																		\
																(_max)																	\
																:																		\
																(																		\
																	((_val)<(_min))														\
																	?																	\
																	(_min)																\
																	:																	\
																	(_val)																\
																)																		\
											)


/* PP: LOOPCONSTRAIN constrains _val to the inclusive limits of _min and _max,
	but does so by 'looping' _val between these limits rather than simply clipping it.
	It only works with integer arguments.
*/
#define LOOPCONSTRAIN(_val, _min, _max)		(																							\
												((_val)>(_max))																			\
																?																		\
																((_min)+(((_val)-(_min))%(((_max)-(_min))+1)))							\
																:																		\
																(																		\
																	((_val)<(_min))														\
																					?													\
																					((_max)+1-(((_min)-(_val))%(((_max)-(_min))+1)))	\
																					:													\
																					(_val)												\
																)																		\
											)

// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ROUNDING MACROS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// PP: round a real value up to the next integer value.  If the value is already a whole number just cast it to int instead
#define ROUNDUP(_val)		(										\
								((_val)==((int)(_val)))				\
								?									\
								((int)(_val))						\
								:									\
								(((int)(_val))+1)					\
							)

// PP: round a real value down to the next integer value
#define ROUNDDOWN(_val)		((int)(_val))

// PP: round a real value to the nearest integer value, rounding up from a fractional part of 0.5 upwards
#define ROUND(_val)			(										\
								(((_val)-ROUNDDOWN(_val))>=0.5f)	\
								?									\
								(ROUNDDOWN(_val)+1)					\
								:									\
								ROUNDDOWN(_val)						\
							)

// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// PP: integer versions of the MAX and MIN macros defined in Babel
#define MAXINT(_a,_b)		((_a)>(_b)?(_a):(_b))
#define MININT(_a,_b)		((_a)<(_b)?(_a):(_b))

// PP: find the maximum of three values
#define MAX3(_a, _b, _c)	(((_a)>(_b)) ? MAX(_a, _c) : MAX(_b, _c))

// PP: find the minimum of three values
#define MIN3(_a, _b, _c)	(((_a)<(_b)) ? MIN(_a, _c) : MIN(_b, _c))

#define SETVECTOR					bmVectorSet											// PP: Might be faster on some platforms than individually assigning the xyzw values?
#define	SETQUAT(quat,x,y,z,w)		{quat[X] = x;quat[Y] = y;quat[Z]=z;quat[W]=w;}

void CreateQuatFromPoints(TBQuaternion quat, TBVector start, TBVector end,float tilt);

/*	--------------------------------------------------------------------------------
	Function 	: Aabs()
	Purpose 	: calculates a valid angle (makes sure it is between 0 and PI*2)
	Parameters 	: angle
	Returns 	: absolute angle
	Info 		:
*/
float Aabs(float angle);

/*	--------------------------------------------------------------------------------
	Function 	: GetAngleAroundYFromQuat
	Purpose 	: returns the angle around Y of a quaternion
	Parameters 	: Quaternion
	Returns 	: angle
	Info 		:
*/
float GetAngleAroundYFromQuat(TBQuaternion quat);

/*	--------------------------------------------------------------------------------
	Function 	: FindShortestAngle()
	Purpose 	: returns the smallest angle between 2 directions (rads)
	Parameters 	: angle1, angle2
	Returns 	: angle
	Info 		:
*/
float FindShortestAngle(float val1, float val2);

/*	--------------------------------------------------------------------------------
	Function 	: FindShortestAngleSigned
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
float FindShortestAngleSigned(float ang1,float ang2);

/*	--------------------------------------------------------------------------------
	Function 	: FindPositiveAngle
	Purpose 	: find the positive difference between two angles
	Parameters 	: angle 1, angle 2
	Returns 	: the positive difference between the two angles
	Info 		: // PP: I put this in for the zoo boss game, for the case where
							I wanted the elephant always to turn anti-clockwise to its destination angle
*/
inline float FindPositiveAngle(const float ang1, const float ang2)
{
	float		diff=ang1-ang2;

	if(diff > 0.0f)
	{
		return diff;
	}
	else
	{
		return TWOPI+diff;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: FindNegativeAngle
	Purpose 	: find the negative difference between two angles
	Parameters 	: angle 1, angle 2
	Returns 	: the negative difference between the two angles
	Info 		: // PP: I put this in for the zoo boss game, for the case where
							I wanted the elephant always to turn clockwise to its destination angle
*/
inline float FindNegativeAngle(const float ang1, const float ang2)
{
	float		diff=ang1-ang2;

	if(diff < 0.0f)
	{
		return diff;
	}
	else
	{
		return TWOPI-diff;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: FindRandomlySignedAngle
	Purpose 	: randomly return either the positive or negative difference between two angles
	Parameters 	: angle 1, angle 2
	Returns 	: either the positive or negative difference between two angles, determined by the sales of Swatch watches in spa towns
	Info 		: // PP: I put this in for the zoo boss game, for the case where
							I wanted the elephant to randomly either turn clockwise or anti-clockwise
*/
inline float FindRandomlySignedAngle(const float ang1, const float ang2)
{
	return ((bmRand()&1)?FindPositiveAngle(ang1, ang2):FindNegativeAngle(ang1, ang2));
}

/*	--------------------------------------------------------------------------------
	Function 	: GetAngleAroundYFromVector
	Purpose 	: returns the angle around the y axis the vector points in
	Parameters 	: TBVector vector
	Returns 	: angle
	Info 		:
*/
float GetAngleAroundYFromVector(TBVector vector);

/*	--------------------------------------------------------------------------------
	Function 	: IsPointInPolyXZ
	Purpose 	: test if a point is inside a polygon
	Parameters 	: TBVector point, 3 verts
	Returns 	: TRUE / FALSE
	Info 		:
*/
BOOL IsPointInPolyXZ(TBVector point,TBVector vert1,TBVector vert2,TBVector vert3);

/*	--------------------------------------------------------------------------------
	Function 	: LineIntersect
	Purpose 	: Detect the intersection of lines and return true / false
	Parameters 	: line p1 - p2, line l1-l2
	Returns 	: YES/NO for intersection
	Info 		:
*/
BOOL LineIntersectXZ(TBVector p1,TBVector p2,TBVector l1,TBVector l2);

/*	--------------------------------------------------------------------------------
	Function 	: ClipVector
	Purpose 	: Clip a vector to run along a plane
	Parameters 	: result,vector input, normal of plane
	Returns 	: 
	Info 		:
*/
void ClipVector(TBVector result, TBVector vector, TBVector normal,float bounce = 1.0f);

/* --------------------------------------------------------------------------------
   Function : CalcCurve
   Purpose : Return numPoints on curve defined by 4 control points
   Parameters : point index (0..3), 't' value
   Returns : blending factor
   Info : 
*/
void CalcCurve(TBVector start, TBVector control1, TBVector control2, TBVector end, int points, TBVector table[]);

/* --------------------------------------------------------------------------------
   Function : CalcCurve
   Purpose : Return numPoints on curve defined by 4 control points
   Parameters : point index (0..3), 't' value
   Returns : blending factor
   Info : 
*/

void FindPointOnCurve(TBVector dest, TBVector start, TBVector control1, TBVector control2, TBVector end, float t);

/*  --------------------------------------------------------------------------------
	Function	: VectorQuatMultiply
	Purpose		: Multiply a vector by a quaternion
	Parameters	: vector, quat
	Returns		: vector contains new result
	Info		: // PP: had to remove const on 'quat' in order to prevent a crash situation, sorry
*/
void VectorQuatMultiply(TBVector vector, /*const */TBQuaternion quat);

/*  --------------------------------------------------------------------------------
	Function	: SafeScaleToLength
	Purpose		: scale a vector to length safely
	Parameters	: 
	Returns		: 
	Info		: 
*/

void SafeScaleToLength(TBVector dest, TBVector src, float lenIn);

/*  --------------------------------------------------------------------------------
	Function	: QuatSlerpAtFixedSpeed
	Purpose		: wrapper for bmQuatSlerpAtFixedSpeed which has a convergeance bug
	Parameters	: 
	Returns		: 
	Info		: 
*/

int QuatSlerpAtFixedSpeed(TBQuaternion dest, const TBQuaternion src1, const TBQuaternion src2, const float speed);


// PP: ~~~~~~~~~~~~~~~


#include "Vec.h"				// PP: TBVector wrapper WITH OPERATORS - YAY!!!!


#endif// PP: ndef __H_MATHS