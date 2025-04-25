// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : maths.cpp
//   Purpose : maths functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "maths.h"


/*	--------------------------------------------------------------------------------
	Function 	: Aabs()
	Purpose 	: calculates a valid angle (makes sure it is between 0 and PI*2)
	Parameters 	: angle
	Returns 	: absolute angle
	Info 		:
*/
float Aabs(float angle)
{
	angle = angle > (2.0f * PI) ? angle - 2.0f * PI : angle;
	angle = angle < 0.0f ? angle += 2.0f * PI : angle;

	return angle;
}

/*	--------------------------------------------------------------------------------
	Function 	: GetAngleAroundYFromQuat
	Purpose 	: returns the angle around Y of a quaternion
	Parameters 	: Quaternion
	Returns 	: angle
	Info 		:
*/
float GetAngleAroundYFromQuat(TBQuaternion quat)
{
	//*

	TBVector	tempVector;
	TBMatrix tempMatrix;
	float	angle;

	tempVector[X] = 0.0f;
	tempVector[Y] = 0.0f;
	tempVector[Z] = 1.0f;
	tempVector[A] = 1.0f;

	bmQuatToMatrix(tempMatrix,quat);
	bmMatMultiplyVector(tempMatrix,tempVector);

	tempVector[Y] = 0.0f;
	tempVector[W] = 1.0f;
	angle = (float)bmATan2(-tempVector[X],tempVector[Z]);

	return angle;

	/*/

	TBVector	rotVec;

	// PP: what a thoroughly confusing function!  doesn't return a rotation vector in the sense I'm used to at all.
	// PP: needless to say this had me questioning my sanity for a good five hours!  Ah, all better now
	bmQuatToRotation(rotVec, this->ang());

	return Aabs(rotVec[W]*rotVec[Y]);	

	//*/
}

/*	--------------------------------------------------------------------------------
	Function 	: FindShortestAngle()
	Purpose 	: returns the smallest angle between 2 directions (rads)
	Parameters 	: angle1, angle2
	Returns 	: angle
	Info 		:
*/
float FindShortestAngle(float val1, float val2)
{
	float temp;

	if(bmFAbs(val1 - val2) > PI)	
	{
		if(val1 > val2)
		{
			temp = val2 - val1;
		}
		else
		{
			temp = val1 - val2;
		}
		temp += PI * 2.0f;
		return temp;
	}
	else
	{
	   return (float)bmFAbs(val1 - val2);
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: FindShortestAngleSigned
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
float FindShortestAngleSigned(float ang1,float ang2)
{
	float temp = ang1 - ang2;
	if((temp > -PI) && (temp <= PI))
		return temp;
	else if(temp > PI)
		return ang1 - ang2 - 2.0f*PI;
	else
		return ang1 + 2.0f*PI - ang2;
}

/*	--------------------------------------------------------------------------------
	Function 	: GetAngleAroundYFromVector
	Purpose 	: returns the angle around the y axis the vector points in
	Parameters 	: TBVector vector
	Returns 	: angle
	Info 		:
*/
float GetAngleAroundYFromVector(TBVector vector)
{
	if ((vector[X]==0.0f)&&(vector[Z]==0.0f))
	{
		// TP: just in case
		return 0.0f;
	}
	return( Aabs((float)bmATan2(-vector[X], vector[Z])));
}

/*	--------------------------------------------------------------------------------
	Function 	: IsPointInPolyXZ
	Purpose 	: test if a point is inside a polygon
	Parameters 	: TBVector point, 3 verts
	Returns 	: TRUE / FALSE
	Info 		:
*/
BOOL IsPointInPolyXZ(TBVector point,TBVector vert1,TBVector vert2,TBVector vert3)
{
	TBVector	distantPoint;
	int		count=0;

	//must create point outside polygon
	distantPoint[X] = vert1[X] + METERTOUNIT(100.0f);
	distantPoint[Y] = 0.0f;
	distantPoint[Z] = 0.0f;

	if (LineIntersectXZ(point,distantPoint,vert1,vert2)) count++;
	if (LineIntersectXZ(point,distantPoint,vert2,vert3)) count++;
	if (LineIntersectXZ(point,distantPoint,vert3,vert1)) count++;

	if (count&1) return TRUE;
	return FALSE;
}

/*	--------------------------------------------------------------------------------
	Function 	: LineIntersect
	Purpose 	: Detect the intersection of lines and return true / false
	Parameters 	: line p1 - p2, line l1-l2
	Returns 	: YES/NO for intersection
	Info 		:
*/
BOOL LineIntersectXZ(TBVector p1,TBVector p2,TBVector l1,TBVector l2)
{
	float	ax,ay,bx,by,cx,cy;
	float	d,f,e;

	ax = p2[X]-p1[X];
	ay = p2[Z]-p1[Z];

	bx = l1[X]-l2[X];
	by = l1[Z]-l2[Z];

	// test to see if the lines intersect
	cx = p1[X]-l1[X];
	cy = p1[Z]-l1[Z];
	d = by*cx - bx*cy;
	f = ay*bx - ax*by;
	if (f > 0.0f)
	{
		if ((d < 0.0f) || (d > f))
			return FALSE;
	}
	else 
	{
		if ((d > 0.0f) || (d < f))
		return FALSE;
	}
	
	e = ax*cy - ay*cx;
	if (f > 0.0f)
	{
		if ((e < 0.0f) || (e > f))
			return FALSE;
	}
	else 
	{
		if ((e > 0.0f) || (e < f))
		return FALSE;
	}
	
	// test if lines are collinear
	if (f == 0.0f)
		return FALSE;

	// lines do intersect, so find the coordinates
	return TRUE;
}

/*	--------------------------------------------------------------------------------
	Function 	: ClipVector
	Purpose 	: Clip a vector to run along a plane
	Parameters 	: result,vector input, normal of plane
	Returns 	: 
	Info 		:
*/
void ClipVector(TBVector result, TBVector vector, TBVector normal, float bounce)
{
	float		value;
	TBVector	tempVector;

	normal[W] = 1.0f;
	vector[W] = 1.0f;
	value = bmVectorDot(vector,normal);

	bmVectorCMul(tempVector,normal,value*bounce);
	bmVectorSub(result,vector,tempVector);
}
/*
void PM_ClipVelocity( vec3_t in, vec3_t normal, vec3_t out, float overbounce ) {
	float	backoff;
	float	change;
	int		i;
	
	backoff = DotProduct (in, normal);
	
	if ( backoff < 0 ) {
		backoff *= overbounce;
	} else {
		backoff /= overbounce;
	}

	for ( i=0 ; i<3 ; i++ ) {
		change = normal[i]*backoff;
		out[i] = in[i] - change;
	}
}*/

void CreateQuatFromPoints(TBQuaternion quat, TBVector start, TBVector end, float tilt)
{
	TBMatrix			lookAtXForm;
	TBVector			localPoint;
	float			xAngle, yAngle, len;
	TBQuaternion		lookAtQuat;

	localPoint[0] = end[0]-start[0];
	localPoint[1] = end[1]-start[1];
	localPoint[2] = end[2]-start[2];
	localPoint[3] = 1.0f;
	
	// find angle around the Y axis (look around)
	yAngle = ((float)bmATan2(localPoint[2], localPoint[0]))+HALFPI;
	if (yAngle > PI)
		yAngle -= TWOPI;
	yAngle *= -1.0f;

	// find angle around the X axis (look up/down)
	len = bmSqrt(localPoint[0]*localPoint[0] + localPoint[2]*localPoint[2]);
	xAngle = ((float)bmATan2(localPoint[1], len));
//	xAngle = ((float)bmASin(localPoint[1]/len));

	// calculate quaternion representing looking at the point
	bmMatYXZRotation(lookAtXForm, xAngle, yAngle, /*yAngle**/tilt);
	bmMatrixToQuat(lookAtQuat, lookAtXForm);
	bmQuatCopy(quat,lookAtQuat);
}

/* --------------------------------------------------------------------------------
   Function : C
   Purpose : Don't know stole from John
   Parameters : 
   Returns : 
   Info : 
*/
static	float C(int i)
{
	int		j,a;

	a=1;
	for(j = i+1; j < 4; j++)
		a *= j;
	for(j = 1; j < 4-i; j++)
		a /= j;

	return ((float)a);
}

/* --------------------------------------------------------------------------------
   Function : bblend
   Purpose : Don't know stole from John
   Parameters : 
   Returns : 
   Info : 
*/
static float bblend(int i, float u)
{

	int		j;
	float	v;

	v = C(i);
	for(j = 1; j <= i; j++)
		v *= u;
	for(j = 1; j < 4-i; j++)
		v *= 1.0f-u;

	if (bmFAbs(v) < 1.0E-15)
		return 0.0f;
	else
    	return v;
}

/* --------------------------------------------------------------------------------
   Function : CalcCurve
   Purpose : Return numPoints on curve defined by 4 control points
   Parameters : point index (0..3), 't' value
   Returns : blending factor
   Info : 
*/
void CalcCurve(TBVector start, TBVector control1, TBVector control2, TBVector end, int points, TBVector table[])
{

	float		b,fx,fy,fz,x1,y1,z1, x2,y2,z2, cp1x,cp1y,cp1z, cp2x,cp2y,cp2z,t,inc;
	TBVector	pos;
	short		tab,count;

	x1 = start[X];
	y1 = start[Y];
	z1 = start[Z];

	cp1x = control1[X];
	cp1y = control1[Y];
	cp1z = control1[Z];

	cp2x = control2[X];
	cp2y = control2[Y];
	cp2z = control2[Z];

	x2 = end[X];
	y2 = end[Y];
	z2 = end[Z];

	inc = 1.0f/((float)(points)-1.0f);

	tab=0;
	t = 0;
	// move 't' along the curve at desired speed (0..1)
//	for(t = 0.0f; t <= 1.0f; t += inc)
	for(count=0; count<points; count++)
	{
		fx = 0.0f;	// 'X' accumulator
		fy = 0.0f;	// 'Y' accumulator
		fz = 0.0f;

		// calculate weight for start point
		b = bblend(0, t);
		fx += x1*b;
        fy += y1*b;
        fz += z1*b;

		// calculate weight for control point #1
		b = bblend(1, t);
		fx += cp1x*b;
        fy += cp1y*b;
        fz += cp1z*b;

		// calculate weight for control point #2
		b = bblend(2, t);
		fx += cp2x*b;
        fy += cp2y*b;
        fz += cp2z*b;

		// calculate weight for end point
		b = bblend(3, t);
		fx += x2*b;
        fy += y2*b;
        fz += z2*b;

		pos[X] = fx;
		pos[Y] = fy;
		pos[Z] = fz;
		if (count==(points-1))
		{
			table[tab][X] = end[X];
			table[tab][Y] = end[Y];
			table[tab][Z] = end[Z];
			table[tab++][W] = 1.0f;
		}
		else
		{
			table[tab][X] = pos[X];
			table[tab][Y] = pos[Y];
			table[tab][Z] = pos[Z];
			table[tab++][W] = 1.0f;
		}

		t+=inc;
	}
}

/* --------------------------------------------------------------------------------
   Function : CalcCurve
   Purpose : Return numPoints on curve defined by 4 control points
   Parameters : point index (0..3), 't' value
   Returns : blending factor
   Info : 
*/

void FindPointOnCurve(TBVector dest, TBVector start, TBVector control1, TBVector control2, TBVector end, float t)
{
	float		b,fx,fy,fz,x1,y1,z1, x2,y2,z2, cp1x,cp1y,cp1z, cp2x,cp2y,cp2z;
	short		tab;

	x1 = start[X];
	y1 = start[Y];
	z1 = start[Z];

	cp1x = control1[X];
	cp1y = control1[Y];
	cp1z = control1[Z];

	cp2x = control2[X];
	cp2y = control2[Y];
	cp2z = control2[Z];

	x2 = end[X];
	y2 = end[Y];
	z2 = end[Z];

	tab=0;

	fx = 0.0f;	// 'X' accumulator
	fy = 0.0f;	// 'Y' accumulator
	fz = 0.0f;

	// calculate weight for start point
	b = bblend(0, t);
	fx += x1*b;
    fy += y1*b;
    fz += z1*b;

	// calculate weight for control point #1
	b = bblend(1, t);
	fx += cp1x*b;
    fy += cp1y*b;
    fz += cp1z*b;

	// calculate weight for control point #2
	b = bblend(2, t);
	fx += cp2x*b;
    fy += cp2y*b;
    fz += cp2z*b;

	// calculate weight for end point
	b = bblend(3, t);
	fx += x2*b;
    fy += y2*b;
    fz += z2*b;

	dest[X] = fx;
	dest[Y] = fy;
	dest[Z] = fz;
	dest[W] = 1.0f;

}

/*  --------------------------------------------------------------------------------
	Function	: VectorQuatMultiply
	Purpose		: Multiply a vector by a quaternion
	Parameters	: vector, quat
	Returns		: vector contains new result
	Info		: // PP: had to remove const on 'quat' in order to prevent a crash situation, sorry
*/
void VectorQuatMultiply(TBVector vector, /*const */TBQuaternion quat)
{
	TBMatrix		tempMatrix;

	// PP: set the W to 1 if each & every element of the quat equals zero
	// PP: NOTE: THIS CORRECTION IS DONE IN A COUPLE OF OTHER PLACES TOO
	// PP: I DON'T LIKE IT
	if((quat[0] == 0.0f)
		&&(quat[1] == 0.0f)
		&&(quat[2] == 0.0f)
		&&(quat[3] == 0.0f))
	{
		quat[3] = 1.0f;
	}

	bmQuatToMatrix(tempMatrix,quat);		// PP: this'll crash if W==0
	bmMatMultiplyVector(tempMatrix,vector);
}

/*  --------------------------------------------------------------------------------
	Function	: SafeScaleToLength
	Purpose		: scale a vector to length safely
	Parameters	: 
	Returns		: 
	Info		: 
*/

void SafeScaleToLength(TBVector dest, TBVector src, float lenIn)
{
	float len = bmVectorLen(src);

	if (!len)
	{
		bkPrintf("*** WARNING *** attempt to scale to length a zero sized vector.\n");
		bmVectorCopy(dest, src);
		return;
	}

	bmVectorCMul(dest, src, lenIn/len);
}

/*  --------------------------------------------------------------------------------
	Function	: QuatSlerpAtFixedSpeed
	Purpose		: wrapper for bmQuatSlerpAtFixedSpeed which has a convergeance bug
	Parameters	: 
	Returns		: 
	Info		: 
*/

int QuatSlerpAtFixedSpeed(TBQuaternion dest, const TBQuaternion src1, const TBQuaternion src2, const float speed)
{
#define QUATSLERP_THRESHOLD		(0.009f)
	int			i;

	bmQuatSlerpAtFixedSpeed(dest, src1, src2, speed);

	// CMD: now check to see if dest quaternion is near enough to src2
	for(i = 3;i >= 0;--i)
	{
		if(bmFAbs(dest[i] - src2[i]) > QUATSLERP_THRESHOLD) return FALSE;
	}
	// CMD: tidy everything up by copying src2 to dest
	bmQuatCopy(dest, src2);
	return TRUE;
}