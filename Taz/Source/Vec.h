// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Vec.cpp
//   Purpose : TBVector wrapper WITH OPERATORS - YAY!!!!
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_VEC
#define __H_VEC


// PP: zero VEC
#define ZVEC								VEC(0.f,0.f,0.f,1.f)

// PP: macro to construct a VEC in metres rather than units
#define VECM(_x, _y, _z)					VEC(METERTOUNIT(_x), METERTOUNIT(_y), METERTOUNIT(_z))

// PP: macro to construct a VEC in degrees rather than radians
#define VECD(_x, _y, _z)					VEC(RAD(_x), RAD(_y), RAD(_z))

// PP: macro form a vector with a random value between -(_max) and (_max) on each axis
#define RANDOMVEC(_max)						VEC(bmRandfSigned()*(_max), bmRandfSigned()*(_max), bmRandfSigned()*(_max))

// PP: macros to print out a VEC, or a Babel vector for that matter
#define PRINTVEC(_vec)						{VEC _vecCopy=_vec; bkPrintf(#_vec##": X %5.2f, Y %5.2f, Z %5.2f\n", (_vecCopy)[X], (_vecCopy)[Y], (_vecCopy)[Z]);}

#define PRINTVEC2D(_vec)					{VEC _vecCopy=_vec; bkPrintf(#_vec##": X %5.2f, Y %5.2f\n", (_vecCopy)[X], (_vecCopy)[Y]);}

// PP: Am I just being silly here?
// PP: I wondered if all these little functions might not inline because of them each having a local return VEC(?)
// PP: PLEASE LET ME KNOW!
extern struct TAG_VEC		VEC_gReturnVec;

// PP: This should save a hell of a lot of time and effort.
// PP: Don't say I'm not good to you!
typedef struct TAG_VEC
{
	TBVector				v;				// PP: Babel four-element vector


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::TAG_VEC
		Purpose 	: default constructor - does bugger all
		Parameters 	: 
		Returns 	: a new VEC instance containing uninitialised elements
		Info 		: I DON'T WANT ANY PROCESSING TO BE DONE IN THE DEFAULT CONSTRUCTOR!
	*/
	inline TAG_VEC(void)	{};

	
	/*	--------------------------------------------------------------------------------
		Function 	: VEC::operator float*()
		Purpose 	: cast the VEC to a float pointer (ie. a TBVector or TBVector2D)
		Parameters 	: 
		Returns 	: a pointer to the VEC's Babel vector
		Info 		: // PP: NOTE: this allows you to pass a VEC straight into a function that expects a TBVector or TBVector2D
						// PP: NOTE: the cast will occur implicitly; you don't have to say (TBVector)myVec!  What would be the point in that?!
	*/
	inline operator float*()
	{
		return this->v;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::operator const float*()
		Purpose 	: cast the VEC to a float pointer (ie. a TBVector or TBVector2D)
		Parameters 	: 
		Returns 	: a pointer to the VEC's Babel vector
		Info 		: // PP: NOTE: this allows you to pass a VEC straight into a function that expects a const TBVector or const TBVector2D
						// PP: NOTE: the cast will occur implicitly; you don't have to say (const TBVector)myVec!  What would be the point in that?!
	*/
	inline operator const float*() const
	{
		return this->v;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::TAG_VEC
		Purpose 	: [default] constructor - creates & initialises a new VEC instance
		Parameters 	: x value, (opt/0.0f)y value, (opt/0.0f)z value, (opt/1.0f)w value
		Returns 	: a new VEC instance
		Info 		: 
	*/
	inline TAG_VEC(const float xVal, const float yVal=0.0f, const float zVal=0.0f, const float wVal=1.0f)
	{
		bmVectorSet(this->v, xVal, yVal, zVal, wVal);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::TAG_VEC
		Purpose 	: constructor - creates & initialises a new VEC instance
		Parameters 	: Babel vector on which to base the VEC
		Returns 	: a new VEC instance
		Info 		: 
	*/
	inline TAG_VEC(const TBVector vector)
	{
		bmVectorCopy(this->v, vector);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::TAG_VEC
		Purpose 	: constructor - creates & initialises a new VEC instance
		Parameters 	: ptr to Babel vector on which to base the VEC
		Returns 	: a new VEC instance
		Info 		: 
	*/
	inline TAG_VEC(const TBVector* const vector)
	{
		bmVectorCopy(this->v, *vector);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::x
		Purpose 	: get the X component of the vector (can be written to)
		Parameters 	: 
		Returns 	: ref to the X component of the vector
		Info 		: interchangeable with [X] - use whichever you prefer
	*/
	inline float& x(void)
	{
		return this->v[X];
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::x
		Purpose 	: get the X component of the vector (cannot be written to)
		Parameters 	: 
		Returns 	: ref to the X component of the vector
		Info 		: interchangeable with [X] - use whichever you prefer
	*/
	inline const float& x(void) const
	{
		return this->v[X];
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::y
		Purpose 	: get the Y component of the vector (can be written to)
		Parameters 	: 
		Returns 	: ref to the Y component of the vector
		Info 		: interchangeable with [Y] - use whichever you prefer
	*/
	inline float& y(void)
	{
		return this->v[Y];
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::y
		Purpose 	: get the Y component of the vector (cannot be written to)
		Parameters 	: 
		Returns 	: ref to the Y component of the vector
		Info 		: interchangeable with [Y] - use whichever you prefer
	*/
	inline const float& y(void) const
	{
		return this->v[Y];
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::z
		Purpose 	: get the Z component of the vector (can be written to)
		Parameters 	: 
		Returns 	: ref to the Z component of the vector
		Info 		: interchangeable with [Z] - use whichever you prefer
	*/
	inline float& z(void)
	{
		return this->v[Z];
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::z
		Purpose 	: get the Z component of the vector (cannot be written to)
		Parameters 	: 
		Returns 	: ref to the Z component of the vector
		Info 		: interchangeable with [Z] - use whichever you prefer
	*/
	inline const float& z(void) const
	{
		return this->v[Z];
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::w
		Purpose 	: get the W component of the vector (can be written to)
		Parameters 	: 
		Returns 	: ref to the W component of the vector
		Info 		: unnamed structs don't work on all platforms, so I have to do this
						as a method, rather than as a member unioned with a component
						of the vector :o(
	*/
/* PP: REMOUT: not needed		inline float& w(void)
	{
		return this->v[W];
	}*/


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::zero
		Purpose 	: zero the vector (set it to 0,0,0,1)
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	inline void zero(void)
	{
		bmVectorZero(this->v);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::set
		Purpose 	: set the X,Y and Z components of the vector
		Parameters 	: x,y,z
		Returns 	: 
		Info 		: 
	*/
	inline void set(const float xIn, const float yIn, const float zIn)
	{
		bmVectorSet(this->v, xIn, yIn, zIn, 1.0f);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::setX
		Purpose 	: get a copy of the VEC, but with the specified X value set
		Parameters 	: x value for new vector
		Returns 	: a new VEC instance
		Info 		: THIS DOES *NOT* ALTER THE VECTOR!
	*/
	inline struct TAG_VEC setX(const float xIn) const
	{
		return struct TAG_VEC(xIn, this->y(), this->z()); 
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::setY
		Purpose 	: get a copy of the VEC, but with the specified Y value set
		Parameters 	: Y value for new vector
		Returns 	: a new VEC instance
		Info 		: THIS DOES *NOT* ALTER THE VECTOR!
	*/
	inline struct TAG_VEC setY(const float yIn) const
	{
		return struct TAG_VEC(this->x(), yIn, this->z()); 
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::setZ
		Purpose 	: get a copy of the VEC, but with the specified Z value set
		Parameters 	: Z value for new vector
		Returns 	: a new VEC instance
		Info 		: THIS DOES *NOT* ALTER THE VECTOR!
	*/
	inline struct TAG_VEC setZ(const float zIn) const
	{
		return struct TAG_VEC(this->x(), this->y(), zIn);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::moveX
		Purpose 	: get a copy of the VEC, but with the specified X offset added
		Parameters 	: X offset for new vector
		Returns 	: a new VEC instance
		Info 		: THIS DOES *NOT* ALTER THE VECTOR!
	*/
	inline struct TAG_VEC moveX(const float xOffset) const
	{
		return struct TAG_VEC(this->x()+xOffset, this->y(), this->z());
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::moveY
		Purpose 	: get a copy of the VEC, but with the specified Y offset added
		Parameters 	: Y offset for new vector
		Returns 	: a new VEC instance
		Info 		: THIS DOES *NOT* ALTER THE VECTOR!
	*/
	inline struct TAG_VEC moveY(const float yOffset) const
	{
		return struct TAG_VEC(this->x(), this->y()+yOffset, this->z());
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::moveZ
		Purpose 	: get a copy of the VEC, but with the specified Z offset added
		Parameters 	: Z offset for new vector
		Returns 	: a new VEC instance
		Info 		: THIS DOES *NOT* ALTER THE VECTOR!
	*/
	inline struct TAG_VEC moveZ(const float zOffset) const
	{
		return struct TAG_VEC(this->x(), this->y(), this->z()+zOffset);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::magnitude
		Purpose 	: get the magnitude of the vector
		Parameters 	: 
		Returns 	: the magnitude of the vector
		Info 		: This is just the same as magnitude(VEC&), so use whichever you prefer
	*/
	inline float magnitude(void) const
	{
		return bmVectorLen(this->v);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::squareMagnitude
		Purpose 	: get the square magnitude of the vector
		Parameters 	: 
		Returns 	: the square magnitude of the vector
		Info 		: This is just the same as squareMagnitude(VEC&), so use whichever you prefer
	*/
	inline float squareMagnitude(void) const
	{
		return bmVectorSquaredLen(this->v);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::distance
		Purpose 	: get the scalar distance from this to another VEC
		Parameters 	: target vector
		Returns 	: the scalar distance from this to another VEC
		Info 		: 
	*/
	inline float distance(const struct TAG_VEC &targetVec) const
	{
		return bmVectorDistance(this->v, targetVec);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::squareDistance
		Purpose 	: get the square distance from this to another VEC
		Parameters 	: target 
		Returns 	: get the square distance from this to another VEC
		Info 		: 
	*/
	inline float squaredDistance(const struct TAG_VEC &targetVec) const
	{
		return bmVectorSquaredDistance(this->v, targetVec);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::distance
		Purpose 	: get the scalar distance from this VEC to a Babel vector
		Parameters 	: target vector
		Returns 	: the scalar distance from this VEC to a Babel vector
		Info 		: 
	*/
	inline float distance(const TBVector targetVec) const
	{
		return bmVectorDistance(this->v, targetVec);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::squaredDistance
		Purpose 	: get the square distance from this VEC to a Babel vector
		Parameters 	: target vector
		Returns 	: get the square distance from this VEC to a Babel vector
		Info 		: 
	*/
	inline float squaredDistance(const TBVector targetVec) const
	{
		return bmVectorSquaredDistance(this->v, targetVec);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::normalise
		Purpose 	: get a unit-length version of the vector
		Parameters 	: 
		Returns 	: a unit-length version of the vector
		Info 		: THIS DOES *NOT* ALTER THE VECTOR!
						There was something about that "zee" that didn't sit right.
					
	*/
	inline TAG_VEC normalise(void) const
	{
		bmVectorNorm(VEC_gReturnVec, this->v);

		return VEC_gReturnVec;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::scale
		Purpose 	: get a version of the vector that is scaled to the specified length
		Parameters 	: length to which to scale the vector
		Returns 	: a version of the vector that is scaled to the specified length
		Info 		: THIS DOES *NOT* ALTER THE VECTOR!
	*/
	inline TAG_VEC scale(const float length) const
	{
		bmVectorScaleToLength(VEC_gReturnVec, this->v, length);

		return VEC_gReturnVec;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::normalise
		Purpose 	: scale the vector to unit length
		Parameters 	: ref to the vector, having been normalised
		Returns 	: a unit-length version of the vector
		Info 		: THIS *DOES* ALTER THE VECTOR!
						There was something about that "zee" that didn't sit right.
					
	*/
	inline TAG_VEC& normalise2(void)
	{
		bmVectorNorm(this->v, this->v);

		return *this;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::scale
		Purpose 	: scale the vector to a specified length
		Parameters 	: length to which to scale the vector
		Returns 	: ref to the vector, having been scaled
		Info 		: THIS *DOES* ALTER THE VECTOR!
	*/
	inline struct TAG_VEC& scale2(const float length)
	{
		bmVectorScaleToLength(this->v, this->v, length);

		return *this;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::dotProduct
		Purpose 	: get the dot of this and another VEC
		Parameters 	: vector with which to dot this vector
		Returns 	: the dot product of this and the other VEC
		Info 		: 
	*/
	inline float dotProduct(const struct TAG_VEC& vec) const
	{
		return bmVectorDot(this->v, vec);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::xAng
		Purpose 	: get the X angle of the (distance) vector
		Parameters 	: 
		Returns 	: the X angle of the (distance) vector, in radians
		Info 		: only use this on distance vectors!
						To get the X angle of a rotation vector, simply use [X] or .x()
	*/
	inline float xAng(void) const
	{
		return -bmATan2(this->y(), HYPOT(this->x(), this->z()));
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::xAng
		Purpose 	: get the X angle from this (position) VEC to another (position) VEC
		Parameters 	: destination VEC
		Returns 	: the X angle from this (position) VEC to the destination (position) VEC
		Info 		: only use this on position vectors!
	*/
	float xAng(const struct TAG_VEC& destVec) const;


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::xAng
		Purpose 	: get the X angle of the (distance) vector
		Parameters 	: 
		Returns 	: the X angle of the (distance) vector, in radians
		Info 		: only use this on distance vectors!
						To get the X angle of a rotation vector, simply use [X] or .x()
	*/
	inline float yAng(void) const
	{
		return bmATan2(this->x(), this->z());
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::yAng
		Purpose 	: get the Y angle from this (position) VEC to another (position) VEC
		Parameters 	: destination VEC
		Returns 	: the Y angle from this (position) VEC to the destination (position) VEC
		Info 		: only use this on position vectors!
	*/
	float yAng(const struct TAG_VEC& destVec) const;


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::yAng
		Purpose 	: get the Y angle from this (position) VEC to a (position) Babel vector
		Parameters 	: destination  vector
		Returns 	: the Y angle from this (position) VEC to the destination (position) Babel vector
		Info 		: only use this on position vectors!
	*/
	float yAng(const TBVector destVec) const;


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::ang
		Purpose 	: get the angle of the vector
		Parameters 	: 
		Returns 	: the angle of the vector, as a new rotation VEC
		Info 		: assumes the vector has no Z angle, y'know, like ye do
	*/
	inline struct TAG_VEC ang(void) const
	{
		return struct TAG_VEC(this->xAng(), this->yAng());
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::yAng
		Purpose 	: get the angle from this (position) VEC to another (position) VEC
		Parameters 	: destination VEC
		Returns 	: the angle from this (position) VEC to the destination (position) VEC, as a new rotation VEC
		Info 		: only use this on position vectors!
	*/
	struct TAG_VEC ang(const struct TAG_VEC& destVec) const;


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::abs
		Purpose 	: get a version of the vector in which all the components are positive
		Parameters 	: VEC
		Returns 	: a version of the vector in which all the components are positive
		Info 		: does nothing with W.  THIS DOES *NOT* ALTER THE VECTOR!
						
	*/
	inline struct TAG_VEC abs(void) const
	{
		return struct TAG_VEC(bmFAbs(this->x()), bmFAbs(this->y()), bmFAbs(this->z()));
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::max
		Purpose 	: get the value of the largest component of the vector (excluding W)
		Parameters 	: VEC
		Returns 	: the value of the largest component of the vector (excluding W)
		Info 		: 
	*/

#undef max	// PP: hmmm, not sure where this is defined or where if at all it's used.  Silly macro name if you ask me

	inline float max(void) const
	{
		return MAX3(this->x(), this->y(), this->z());
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::min
		Purpose 	: get the value of the smallest component of the vector (excluding W)
		Parameters 	: VEC
		Returns 	: the value of the smallest component of the vector (excluding W)
		Info 		: 
	*/

#undef min	// PP: hmmm, not sure where this is defined or where if at all it's used.  Silly macro name if you ask me

	inline float min(void) const
	{
		return MIN3(this->x(), this->y(), this->z());
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::maxAbs
		Purpose 	: get the value of the largest absolute component of the vector (excluding W)
		Parameters 	: VEC
		Returns 	: the value of the largest absolute component of the vector (excluding W)
		Info 		: 
	*/
	inline float maxAbs(void) const
	{
		return this->abs().max();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::minAbs
		Purpose 	: get the value of the smallest absolute component of the vector (excluding W)
		Parameters 	: VEC
		Returns 	: the value of the smallest absolute component of the vector (excluding W)
		Info 		: 
	*/
	inline float minAbs(void) const
	{
		return this->abs().min();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::operator =
		Purpose 	: assign a Babel vector to the VEC
		Parameters 	: Babel vector
		Returns 	: ref to the VEC, having had the Babel vector assigned to it
		Info 		: 
	*/
	inline struct TAG_VEC& operator = (const TBVector vec)
	{
		bmVectorCopy(this->v, vec);

		return (*this);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::rotate<axis1>[<axis2>[<axis3>]]
		Purpose 	: rotate a VEC by the specified radian angle(s) around the specified axis or axes
		Parameters 	: first rotation[, second rotation[, third rotation]]
		Returns 	: a version of this VEC that is rotated as specified
		Info 		: eg. VEC::rotateXYZ(xAng, yAng, zAng)
						THIS DOES *NOT* ALTER THE VECTOR!
						
	*/
	// PP: ALL 21 VARIATIONS DEFINED IN MACROS BELOW


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::rotate<axis1><axis2>[<axis3>]
		Purpose 	: rotate a VEC by the angles held by a rotation vector, around the specified axes
		Parameters 	: rotation VEC, whose rotations are applied to this according to the axis order specified
		Returns 	: a version of this VEC that is rotated as specified
		Info 		: eg. VEC::rotateXYZ(angVec)
						THIS DOES *NOT* ALTER THE VECTOR!
						
	*/
	// PP: ALL 18 VARIATIONS DEFINED IN MACROS BELOW


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::rotate<axis1>[<axis2>[<axis3>]]2
		Purpose 	: rotate a VEC by the specified radian angle(s) around the specified axis or axes
		Parameters 	: Babel vector
		Returns 	: ref to this VEC, having been rotated as specified
		Info 		: eg. VEC::rotateXYZ(xAng, yAng, zAng)
						THIS *DOES* ALTER THE VECTOR!
	*/
	// PP: ALL 21 VARIATIONS DEFINED IN MACROS BELOW


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::rotate<axis1><axis2>[<axis3>]2
		Purpose 	: rotate a VEC by the angles held by a rotation vector, around the specified axes
		Parameters 	: rotation VEC, whose rotations are applied to this according to the axis order specified
		Returns 	: ref to this VEC, having been rotated as specified
		Info 		: eg. VEC::rotateXYZ(angVec)
						THIS *DOES* ALTER THE VECTOR!
						
	*/
	// PP: ALL 18 VARIATIONS DEFINED IN MACROS BELOW


// PP: one-axis rotation methods...

#define VEC_ROTATION_METHODS(_axis)						inline struct TAG_VEC rotate##_axis(const float ang) const \
														{bmVectorRotate##_axis(VEC_gReturnVec, this->v, ang); return VEC_gReturnVec;} \
														\
														inline struct TAG_VEC& rotate##_axis##2(const float ang) \
														{bmVectorRotate##_axis(this->v, this->v, ang); return *this;}

	VEC_ROTATION_METHODS(X);
	VEC_ROTATION_METHODS(Y);
	VEC_ROTATION_METHODS(Z);

// PP: two-axis rotation methods...

#define VEC_ROTATION_METHODS2(_axis1, _axis2)			inline struct TAG_VEC rotate##_axis1##_axis2(const float ang1, const float ang2) const \
														{bmVectorRotate##_axis1##_axis2(VEC_gReturnVec, this->v, ang1, ang2); return VEC_gReturnVec;} \
														\
														inline struct TAG_VEC rotate##_axis1##_axis2(const struct TAG_VEC& angVec) const \
														{return this->rotate##_axis1##_axis2(angVec[_axis1], angVec[_axis2]);} \
														\
														inline struct TAG_VEC& rotate##_axis1##_axis2##2(const float ang1, const float ang2) \
														{bmVectorRotate##_axis1##_axis2(this->v, this->v, ang1, ang2); return *this;} \
														\
														inline struct TAG_VEC& rotate##_axis1##_axis2##2(const struct TAG_VEC& angVec) \
														{return this->rotate##_axis1##_axis2##2(angVec[_axis1], angVec[_axis2]);}

	VEC_ROTATION_METHODS2(X,Y);
	VEC_ROTATION_METHODS2(X,Z);
	VEC_ROTATION_METHODS2(Y,X);
	VEC_ROTATION_METHODS2(Y,Z);
	VEC_ROTATION_METHODS2(Z,X);
	VEC_ROTATION_METHODS2(Z,Y);

// PP: three-axis rotation methods...

#define VEC_ROTATION_METHODS3(_axis1, _axis2, _axis3)	inline struct TAG_VEC rotate##_axis1##_axis2##_axis3(const float ang1, const float ang2, const float ang3) const \
														{bmVectorRotate##_axis1##_axis2##_axis3(VEC_gReturnVec, this->v, ang1, ang2, ang3); return VEC_gReturnVec;} \
														\
														inline struct TAG_VEC rotate##_axis1##_axis2##_axis3(const struct TAG_VEC& angVec) const \
														{return this->rotate##_axis1##_axis2##_axis3(angVec[_axis1], angVec[_axis2], angVec[_axis3]);} \
														\
														inline struct TAG_VEC& rotate##_axis1##_axis2##_axis3##2(const float ang1, const float ang2, const float ang3) \
														{bmVectorRotate##_axis1##_axis2##_axis3(this->v, this->v, ang1, ang2, ang3); return *this;} \
														\
														inline struct TAG_VEC& rotate##_axis1##_axis2##_axis3##2(const struct TAG_VEC& angVec) \
														{return this->rotate##_axis1##_axis2##_axis3##2(angVec[_axis1], angVec[_axis2], angVec[_axis3]);}

	VEC_ROTATION_METHODS3(X,Y,Z);
	VEC_ROTATION_METHODS3(X,Z,Y);
	VEC_ROTATION_METHODS3(Y,X,Z);
	VEC_ROTATION_METHODS3(Y,Z,X);
	VEC_ROTATION_METHODS3(Z,X,Y);
	VEC_ROTATION_METHODS3(Z,Y,X);


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::rotate
		Purpose 	: rotate a VEC by the X & Y angles specified, X first then Y
		Parameters 	: X angle, Y angle
		Returns 	: a version of this VEC that is rotated as specified
		Info 		: THIS DOES *NOT* ALTER THE VECTOR!
						I just find that this XY rotation is particularly useful, that's all.
						I'm a lazy swine me.
	*/
	inline struct TAG_VEC rotate(const float xAngIn, const float yAngIn)
	{
		return this->rotateXY(xAngIn, yAngIn);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::rotate2
		Purpose 	: rotate a VEC by the X & Y angles specified, X first then Y
		Parameters 	: X angle, Y angle
		Returns 	: ref to this VEC, having been rotated as specified
		Info 		: THIS *DOES* ALTER THE VECTOR!
						I just find that this XY rotation is particularly useful, that's all.
						I'm a lazy swine me.
	*/
	inline struct TAG_VEC& rotate2(const float xAngIn, const float yAngIn)
	{
		return this->rotateXY2(xAngIn, yAngIn);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::rotate
		Purpose 	: rotate a VEC by the X & Y angles held by a rotation vector, around X first then Y
		Parameters 	: rotation VEC, whose rotations are applied to this according to the axis order specified
		Returns 	: a version of this VEC that is rotated as specified
		Info 		: THIS DOES *NOT* ALTER THE VECTOR!
						I just find that this XY rotation by an angle VEC is particularly useful, that's all.
						I'm a lazy swine me.
	*/
	inline struct TAG_VEC rotate(const struct TAG_VEC& angVec)
	{
		return this->rotateXY(angVec);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::rotate2
		Purpose 	: rotate a VEC by the X & Y angles held by a rotation vector, around X first then Y
		Parameters 	: rotation VEC, whose rotations are applied to this according to the axis order specified
		Returns 	: ref to this VEC, having been rotated as specified
		Info 		: THIS *DOES* ALTER THE VECTOR!
						I just find that this XY rotation by an angle VEC is particularly useful, that's all.
						I'm a lazy swine me.
	*/
	inline struct TAG_VEC& rotate2(const struct TAG_VEC& angVec)
	{
		return this->rotateXY2(angVec);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::distFromLine
		Purpose 	: calculate the minimum distance from this VEC to the specified line
		Parameters 	: line start VEC, line end VEC, (opt/false)infinite line
		Returns 	: the minimum distance from this VEC to the specified line
		Info 		: 
	*/
	float distFromLine(const struct TAG_VEC& start, const struct TAG_VEC& end, const bool infinite=false);


	/*	--------------------------------------------------------------------------------
		Function 	: VEC::lerp
		Purpose 	: Linearly interpolate between two vectors
		Parameters 	: vector1, vector2, (opt/0.5f) mix value 0.0f=vector1..1.0f=vector2
		Returns 	: interpolated vector between the two vectors you pass in
		Info 		: 
	*/
	inline struct TAG_VEC lerp(const struct TAG_VEC &targetVec, const float mix=0.5f) const
	{
		bmVectorLerp(VEC_gReturnVec, this->v, targetVec, mix);

		// PP: bmVectorLerp buggers up W component; best fix it here
		VEC_gReturnVec[W]=1.0f;

		return VEC_gReturnVec;
	}


}VEC BALIGN16;


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) normalise
	Purpose 	: get a unit-length version of the vector
	Parameters 	: 
	Returns 	: a unit-length version of the vector
	Info 		: THIS DOES *NOT* ALTER THE VECTOR!
					There was something about that "zee" that didn't sit right.
					This is just the same as VEC::normalise, so use whichever you prefer
				
*/
inline TAG_VEC normalise(const VEC& vec)
{
	bmVectorNorm((float*)VEC_gReturnVec, (const float*)vec);

	return VEC_gReturnVec;
}


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) dotProduct
	Purpose 	: get the dot product of two VECs
	Parameters 	: vector 1, vector 2
	Returns 	: the dot product of the two VECs
	Info 		: This is just the same as VEC::dotProduct, so use whichever you prefer
*/
inline float dotProduct(const VEC &v1, const VEC &v2)
{
	return bmVectorDot(v1, v2);
}


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) crossProduct
	Purpose 	: get the cross product of two VECs
	Parameters 	: vector 1, vector 2
	Returns 	: the cross product of the two VECs
	Info 		: 
*/
inline VEC crossProduct(const VEC &v1, const VEC &v2)
{
	bmVectorCross(VEC_gReturnVec, v1, v2);

	return VEC_gReturnVec;
}


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) magnitude
	Purpose 	: get the magnitude of a vector
	Parameters 	: vector
	Returns 	: the magnitude of the vector
	Info 		: This is just the same as VEC::magnitude, so use whichever you prefer
*/
inline float magnitude(const VEC &vec)
{
	return vec.magnitude();
}


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) squareMagnitude
	Purpose 	: get the square magnitude of a vector
	Parameters 	: vector
	Returns 	: the square magnitude of the vector
	Info 		: This is just the same as VEC::squareMagnitude, so use whichever you prefer
*/
inline float squareMagnitude(const VEC &vec)
{
	return vec.squareMagnitude();
}


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) distance
	Purpose 	: get the scalar distance between two VECs
	Parameters 	: vector1, vector2
	Returns 	: the scalar distance between the two VECs
	Info 		: 
*/
inline float distance(const VEC &vec1, const VEC &vec2)
{
	return bmVectorDistance(vec1, vec2);
}


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) squaredDistance
	Purpose 	: get the square distance between two VECs
	Parameters 	: vector1, vector2
	Returns 	: the square distance between the two VECs
	Info 		: 
*/
inline float squaredDistance(const VEC &vec1, const VEC &vec2)
{
	return bmVectorSquaredDistance(vec1, vec2);
}


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) lerp
	Purpose 	: Linearly interpolate between two vectors
	Parameters 	: vector1, vector2, (opt/0.5f) mix value 0.0f=vector1..1.0f=vector2
	Returns 	: interpolated vector between the two vectors you pass in
	Info 		: 
*/
inline VEC lerp(const VEC &vec1, const VEC &vec2, const float mix=0.5f)
{
	bmVectorLerp(VEC_gReturnVec, vec1, vec2, mix);

	return VEC_gReturnVec;
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			ADDITION
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) operator +
	Purpose 	: add two VECs
	Parameters 	: vector 1, vector 2
	Returns 	: new VEC instance
	Info 		: 
*/
inline VEC operator + (const VEC& v1, const VEC& v2)
{
	bmVectorAdd(VEC_gReturnVec, v1, v2);

	return VEC_gReturnVec;
}


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) operator +
	Purpose 	: add a TBVector to a VEC
	Parameters 	: VEC, TBVector
	Returns 	: new VEC instance
	Info 		: 
*/
inline VEC operator + (const VEC& v1, const TBVector v2)
{
	bmVectorAdd(VEC_gReturnVec, v1, v2);

	return VEC_gReturnVec;
}


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) operator +
	Purpose 	: add a VEC to a TBVector
	Parameters 	: TBVector, VEC
	Returns 	: new VEC instance
	Info 		: 
*/
inline VEC operator + (const TBVector v1, const VEC& v2)
{
	bmVectorAdd(VEC_gReturnVec, v1, v2);

	return VEC_gReturnVec;
}


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) operator +=
	Purpose 	: add two VECs and store the result in the left-hand side
	Parameters 	: vector 1, vector 2
	Returns 	: ref to vector 1, having been added to vector 2
	Info 		: 
*/
inline VEC& operator += (VEC& v1, VEC& v2)
{
	bmVectorAdd(v1.v, v1.v, v2.v);

	return v1;
}


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) operator +=
	Purpose 	: add a TBVector to a VEC and store the result in the VEC
	Parameters 	: VEC, TBVector
	Returns 	: ref to VEC, having been added to TBVector
	Info 		: 
*/
inline VEC& operator += (VEC& v1, const TBVector v2)
{
	bmVectorAdd(v1.v, v1.v, v2);

	return v1;
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			SUBTRACTION
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) operator -
	Purpose 	: subtract a VEC from another VEC
	Parameters 	: vector from which to subtract, vector to subtract
	Returns 	: new VEC instance
	Info 		: 
*/
inline VEC operator - (const VEC& v1, const VEC& v2)
{
	bmVectorSub(VEC_gReturnVec, v1, v2);

	return VEC_gReturnVec;
}


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) operator -
	Purpose 	: subtract a TBVector from a VEC
	Parameters 	: VEC, TBVector
	Returns 	: new VEC instance
	Info 		: 
*/
inline VEC operator - (const VEC& v1, const TBVector v2)
{
	bmVectorSub(VEC_gReturnVec, v1, v2);

	return VEC_gReturnVec;
}


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) operator -
	Purpose 	: subtract a VEC from a TBVector
	Parameters 	: TBVector, VEC
	Returns 	: new VEC instance
	Info 		: 
*/
inline VEC operator - (const TBVector v1, const VEC& v2)
{
	bmVectorSub(VEC_gReturnVec, v1, v2);

	return VEC_gReturnVec;
}


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) operator -=
	Purpose 	: subtract a VEC from another VEC and store the result in the left-hand side
	Parameters 	: vector 1, vector 2
	Returns 	: ref to vector 1, having had vector 2 subtracted from it
	Info 		: 
*/
inline VEC& operator -= (VEC& v1, const VEC& v2)
{
	bmVectorSub(v1.v, v1.v, v2.v);

	return v1;
}


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) operator -=
	Purpose 	: subtract a TBVector from a VEC and store the result in the VEC
	Parameters 	: VEC, TBVector
	Returns 	: ref to VEC, having been had TBVector subtracted from it
	Info 		: 
*/
inline VEC& operator -= (VEC& v1, const TBVector v2)
{
	bmVectorSub(v1.v, v1.v, v2);

	return v1;
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			MULTIPLICATION
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) operator *
	Purpose 	: multiply two VECs
	Parameters 	: vector 1, vector 2
	Returns 	: new VEC instance
	Info 		: 
*/
inline VEC operator * (const VEC& v1, const VEC& v2)
{
	bmVectorMul(VEC_gReturnVec, v1, v2);

	return VEC_gReturnVec;
}


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) operator *
	Purpose 	: multiply a VEC by a constant
	Parameters 	: vector, constant
	Returns 	: new VEC instance
	Info 		: 
*/
inline VEC operator * (const VEC& vec, const float constant)
{
	bmVectorCMul(VEC_gReturnVec, vec, constant);

	return VEC_gReturnVec;
}


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) operator *
	Purpose 	: multiply a VEC by a matrix
	Parameters 	: vector, matrix
	Returns 	: new VEC instance
	Info 		: parm1 can't be const because of Babel
*/
inline VEC operator * (VEC& vec, const TBMatrix mat)
{
	bmMatMultiplyVector2(VEC_gReturnVec, mat, vec.v);

	return VEC_gReturnVec;
}


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) operator *=
	Purpose 	: multiply two VECs and store the result in the left-hand side
	Parameters 	: vector 1, vector 2
	Returns 	: ref to vector 1, having been multiplied by vector 2
	Info 		: 
*/
inline VEC& operator *= (VEC& v1, const VEC& v2)
{
	bmVectorMul(v1.v, v1.v, v2.v);

	return v1;
}


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) operator *=
	Purpose 	: multiply a VEC by a constant and store the result in the VEC
	Parameters 	: vector 1, vector 2
	Returns 	: ref to the vector, having been multiplied by the constant
	Info 		: 
*/
inline VEC& operator *= (VEC& vec, const float constant)
{
	bmVectorCMul(vec.v, vec.v, constant);

	return vec;
}


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) operator *=
	Purpose 	: multiply a VEC by a matrix and store the result in the VEC
	Parameters 	: vector, matrix
	Returns 	: ref to the vector, having been multiplied by the matrix
	Info 		: 
*/
inline VEC operator *= (VEC& vec, const TBMatrix mat)
{
	bmMatMultiplyVector(mat, vec);

	return vec;
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			DIVISION
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) operator /
	Purpose 	: divide a VEC by another VEC
	Parameters 	: vector to divide, vector by which to divide
	Returns 	: new VEC instance
	Info 		: 
*/
inline VEC operator / (const VEC& v1, const VEC& v2)
{
	bmVectorDiv(VEC_gReturnVec, v1, v2);

	return VEC_gReturnVec;
}


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) operator /
	Purpose 	: divide a VEC by a constant
	Parameters 	: vector, constant
	Returns 	: new VEC instance
	Info 		: 
*/
inline VEC operator / (const VEC& vec, const float constant)
{
	bmVectorCDiv(VEC_gReturnVec, vec, constant);

	return VEC_gReturnVec;
}


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) operator /=
	Purpose 	: divide a VEC by another VEC and store the result in the left-hand side
	Parameters 	: vector 1, vector 2
	Returns 	: ref to vector 1, having been divided by vector 2
	Info 		: 
*/
inline VEC& operator /= (VEC& v1, const VEC& v2)
{
	bmVectorDiv(v1.v, v1.v, v2.v);

	return v1;
}


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) operator /=
	Purpose 	: divide a VEC by a constant and store the result in the VEC
	Parameters 	: vector, constant
	Returns 	: ref to the vector, having been divided by the constant
	Info 		: 
*/
inline VEC& operator /= (VEC& vec, const float constant)
{
	bmVectorCDiv(vec.v, vec.v, constant);

	return vec;
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			NEGATION
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: (VEC) operator -
	Purpose 	: get the negative of a vector
	Parameters 	: vector
	Returns 	: new VEC instance
	Info 		: 
*/
inline VEC operator - (const VEC& vec)
{
	bmVectorNegate(VEC_gReturnVec, vec);

	return VEC_gReturnVec;
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			EQUALITY
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: VEC::operator ==
	Purpose 	: determine if a VEC is equal to a Babel vector
	Parameters 	: VEC, Babel vector
	Returns 	: whether or not the VEC is equal to the Babel vector
	Info 		: 
*/
inline int operator == (VEC& v1, TBVector v2)
{
	return bmVectorEqual(v1, v2);
}


/*	--------------------------------------------------------------------------------
	Function 	: VEC::operator ==
	Purpose 	: determine if a Babel vector is equal to a VEC
	Parameters 	: Babel vector, VEC
	Returns 	: whether or not the Babel vector is equal to the VEC.
	Info 		: 
*/
inline int operator == (TBVector v1, VEC& v2)
{
	return bmVectorEqual(v2, v1);
}


/*	--------------------------------------------------------------------------------
	Function 	: VEC::operator ==
	Purpose 	: determine if a VEC is equal to another VEC
	Parameters 	: VEC 1, VEC 2
	Returns 	: whether or not the two VECs are equal
	Info 		: 
*/
inline int operator == (VEC& v1, VEC& v2)
{
	return bmVectorEqual(v1, v2);// PP: because I might apply an (Epsilon - is that what you call it?) at some point
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			INEQUALITY
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: VEC::operator !=
	Purpose 	: determine if a VEC is not equal to a Babel vector
	Parameters 	: VEC, Babel vector
	Returns 	: whether or not the VEC is not equal to the Babel vector
	Info 		: 
*/
inline bool operator != (VEC& v1, TBVector v2)
{
	return !bmVectorEqual(v1, v2);
}


/*	--------------------------------------------------------------------------------
	Function 	: VEC::operator !=
	Purpose 	: determine if a Babel vector is not equal to a VEC
	Parameters 	: Babel vector, VEC
	Returns 	: whether or not the Babel vector is not equal to the VEC.
	Info 		: 
*/
inline bool operator != (TBVector v1, VEC& v2)
{
	return !bmVectorEqual(v2, v1);
}


/*	--------------------------------------------------------------------------------
	Function 	: VEC::operator !=
	Purpose 	: determine if a VEC is not equal to another VEC
	Parameters 	: VEC 1, VEC 2
	Returns 	: whether or not the two VECs are not equal
	Info 		: 
*/
inline bool operator != (VEC& v1, VEC& v2)
{
	return !bmVectorEqual(v1, v2);
}


#endif// PP: ndef __H_VEC