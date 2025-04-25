// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Vec.cpp
//   Purpose : TBVector wrapper WITH OPERATORS - YAY!!!!
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"			// PP: global stuff
#include "Maths.h"			// PP: maths stuff

// PP: Am I just being silly here?
// PP: I wondered if all those little functions in the header might not inline because of them each having a local return VEC(?)
// PP: PLEASE LET ME KNOW!
struct TAG_VEC		VEC_gReturnVec(0,0,0,1);// PP: gotter initialise that pesky W value


// PP: THE FOLLOWING METHODS ARE IN HERE BECAUSE THEY MUST BE DEFINED AFTER THE MINUS OPERATOR...


/*	--------------------------------------------------------------------------------
	Function 	: VEC::xAng
	Purpose 	: get the X angle from this (position) VEC to another (position) VEC
	Parameters 	: destination VEC
	Returns 	: the X angle from this (position) VEC to the destination (position) VEC
	Info 		: only use this on position vectors!
*/
float TAG_VEC::xAng(const struct TAG_VEC& destVec) const
{
	return (destVec-(*this)).xAng();
}


/*	--------------------------------------------------------------------------------
	Function 	: VEC::yAng
	Purpose 	: get the Y angle from this (position) VEC to another (position) VEC
	Parameters 	: destination VEC
	Returns 	: the Y angle from this (position) VEC to the destination (position) VEC
	Info 		: only use this on position vectors!
*/
float TAG_VEC::yAng(const struct TAG_VEC& destVec) const
{
	return (destVec-(*this)).yAng();
}


/*	--------------------------------------------------------------------------------
	Function 	: VEC::yAng
	Purpose 	: get the Y angle from this (position) VEC to a (position) Babel vector
	Parameters 	: destination  vector
	Returns 	: the Y angle from this (position) VEC to the destination (position) Babel vector
	Info 		: only use this on position vectors!
*/
float TAG_VEC::yAng(const TBVector destVec) const
{
	return (destVec-(*this)).yAng();
}


/*	--------------------------------------------------------------------------------
	Function 	: VEC::yAng
	Purpose 	: get the angle from this (position) VEC to another (position) VEC
	Parameters 	: destination VEC
	Returns 	: the angle from this (position) VEC to the destination (position) VEC, as a new rotation VEC
	Info 		: only use this on position vectors!
*/
struct TAG_VEC TAG_VEC::ang(const struct TAG_VEC& destVec) const
{
	return (destVec-(*this)).ang();
}



/*	--------------------------------------------------------------------------------
	Function 	: VEC::distFromLine
	Purpose 	: calculate the minimum distance from this VEC to the specified line
	Parameters 	: line start VEC, line end VEC, (opt/false)infinite line
	Returns 	: the minimum distance from this VEC to the specified line
	Info 		: 
*/
float TAG_VEC::distFromLine(const struct TAG_VEC& start, const struct TAG_VEC& end, const bool infinite)
{
	TBCollisionLine		line;
	TBVector			closestPoint;		// PP: not used / not used yet

	bmVectorCopy(line.start, start);
	bmVectorCopy(line.end, end);

	if(infinite)
	{
		return bmGeomDistanceFromInfiniteLine(&line, this->v, closestPoint);
	}
	else
	{
		return bmGeomDistanceFromLine(&line, this->v, closestPoint);
	}		
}
