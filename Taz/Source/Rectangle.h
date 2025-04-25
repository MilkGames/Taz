// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Rectangle.h
//   Purpose : Handy rectangle class
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_RECTANGLE
#define __H_RECTANGLE

#include "global.h"			// PP: global stuff
#include "main.h"			// PP: main stuff I suppose
#include "Colour.h"			// PP: handy colour class


// PP: draw styles/draw flags
enum
{
	OUTLINE		=1,							// PP: outline is drawn with the colour specified
	SOLIDFILL	=(1<<1),					// PP: filled rectangle is drawn with the colour specified
	ALPHAFILL	=(1<<2)						// PP: filled rectangle is drawn with the colour specified, only less opaque
};

#define RECTANGLE_DEFAULT_DRAW_FLAGS		(OUTLINE|ALPHAFILL)		// PP: default flags for drawing rectangles

#define RECTANGLE_DEFAULT_DRAW_COLOUR		COLOUR(0,255,0,255)		// PP: default colour for drawing rectangles

#define RECTANGLE_DEFAULT_LEFT		(-100)
#define RECTANGLE_DEFAULT_RIGHT		100
#define RECTANGLE_DEFAULT_TOP		100
#define RECTANGLE_DEFAULT_BOTTOM	(-100)

typedef struct TAG_RECTANGLE
{
	float	left;
	float	right;
	float	top;
	float	bottom;


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::init
		Purpose 	: initialise the rectangle
		Parameters 	: (opt/default)left pos, (opt/default)right pos, (opt/default)bottom pos, (opt/default)top pos,
		Returns 	: 
		Info 		: 
	*/
	inline void init(const float leftIn=RECTANGLE_DEFAULT_LEFT, const float rightIn=RECTANGLE_DEFAULT_RIGHT,
					const float bottomIn=RECTANGLE_DEFAULT_BOTTOM, const float topIn=RECTANGLE_DEFAULT_TOP)
	{
		this->left=leftIn;
		this->right=rightIn;
		this->bottom=bottomIn;
		this->top=topIn;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::init
		Purpose 	: initialise the rectangle
		Parameters 	: left pos, right pos, bottom pos, top pos,
		Returns 	: 
		Info 		: takes ints - just saves some warnings/casts
	*/
	inline void init(const int leftIn, const int rightIn,
					const int bottomIn, const int topIn)
	{
		this->left=(float)leftIn;
		this->right=(float)rightIn;
		this->bottom=(float)bottomIn;
		this->top=(float)topIn;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::TAG_RECTANGLE
		Purpose 	: [default] constructor
		Parameters 	: (opt/default)left pos, (opt/default)right pos, (opt/default)bottom pos, (opt/default)top pos,
		Returns 	: new RECTANGLE instance
		Info 		: 
	*/
	inline TAG_RECTANGLE(const float left=RECTANGLE_DEFAULT_LEFT, const float right=RECTANGLE_DEFAULT_RIGHT,
					const float bottom=RECTANGLE_DEFAULT_BOTTOM, const float top=RECTANGLE_DEFAULT_TOP)
	{
		this->init(left, right, bottom, top);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::TAG_RECTANGLE
		Purpose 	: [default] constructor
		Parameters 	: left pos, right pos, bottom pos, top pos,
		Returns 	: new RECTANGLE instance
		Info 		: takes ints - just saves some warnings/casts
	*/
	inline TAG_RECTANGLE(const int left, const int right,
					const int bottom, const int top)
	{
		this->init(left, right, bottom, top);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::width
		Purpose 	: get the width of the rectangle
		Parameters 	: 
		Returns 	: the width of the rectangle
		Info 		: 
	*/
	inline float width(void) const
	{
		return this->right-this->left;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::widthAbs
		Purpose 	: get the positive width of the rectangle
		Parameters 	: 
		Returns 	: the positive width of the rectangle
		Info 		: 
	*/
	inline float widthAbs(void) const
	{
		return bmFAbs(this->right-this->left);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::setWidth
		Purpose 	: set the width of the rectangle
		Parameters 	: new width for the rectangle
		Returns 	: 
		Info 		: 
	*/
	void setWidth(const float newWidth);


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::setHeight
		Purpose 	: set the height of the rectangle
		Parameters 	: new height for the rectangle
		Returns 	: 
		Info 		: 
	*/
	void setHeight(const float newHeight);


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::mulWidth
		Purpose 	: multiply the width of the rectangle
		Parameters 	: factor by which to multiply the width for the rectangle
		Returns 	: 
		Info 		: this should be a teensy more efficient than saying "rect.setWidth(rect.width()*1.5f)"
	*/
	void mulWidth(const float factor);


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::mulHeight
		Purpose 	: multiply the height of the rectangle
		Parameters 	: factor by which to multiply the height for the rectangle
		Returns 	: 
		Info 		: this should be a teensy more efficient than saying "rect.setHeight(rect.height()*1.5f)"
	*/
	void mulHeight(const float factor);


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::height
		Purpose 	: get the height of the rectangle
		Parameters 	: 
		Returns 	: the height of the rectangle
		Info 		: 
	*/
	inline float height(void) const
	{
		//bkPrintf("RECTANGLE::height returning %5.2f\n", this->top-this->bottom);
		return this->top-this->bottom;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::heightAbs
		Purpose 	: get the positive height of the rectangle
		Parameters 	: 
		Returns 	: the positive height of the rectangle
		Info 		: 
	*/
	inline float heightAbs(void) const
	{
		return bmFAbs(this->top-this->bottom);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::midX
		Purpose 	: get the middle X position of the rectangle
		Parameters 	: 
		Returns 	: the middle X position of the rectangle
		Info 		: 
	*/
	inline float midX(void) const
	{
		return this->left+(this->width()*0.5f);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::midY
		Purpose 	: get the middle Y position of the rectangle
		Parameters 	: 
		Returns 	: the middle Y position of the rectangle
		Info 		: 
	*/
	inline float midY(void) const
	{
	//	bkPrintf("RECTANGLE::midY returning %5.2f\n", this->bottom+(this->height()*0.5f));
		return this->bottom+(this->height()*0.5f);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::leftHalf
		Purpose 	: get the left half of the rectangle
		Parameters 	: 
		Returns 	: new RECTANGLE representing the left half of the rectangle
		Info 		: 
	*/
	inline TAG_RECTANGLE leftHalf(void) const
	{
		return TAG_RECTANGLE(this->left, this->midX(), this->bottom, this->top);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::rightHalf
		Purpose 	: get the right half of the rectangle
		Parameters 	: 
		Returns 	: new RECTANGLE representing the right half of the rectangle
		Info 		: 
	*/
	inline TAG_RECTANGLE rightHalf(void) const
	{
		return TAG_RECTANGLE(this->midX(), this->right, this->bottom, this->top);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::bottomHalf
		Purpose 	: get the bottom half of the rectangle
		Parameters 	: 
		Returns 	: new RECTANGLE representing the bottom half of the rectangle
		Info 		: 
	*/
	inline TAG_RECTANGLE bottomHalf(void) const
	{
		return TAG_RECTANGLE(this->left, this->right, this->bottom, this->midY());
	}


	/*	-------------------------------------------------------------------------------
		Function 	: RECTANGLE::topHalf
		Purpose 	: get the top half of the rectangle
		Parameters 	: 
		Returns 	: new RECTANGLE representing the top half of the rectangle
		Info 		: 
	*/
	inline TAG_RECTANGLE topHalf(void) const
	{
		return TAG_RECTANGLE(this->left, this->right, this->midY(), this->top);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::leftPortion
		Purpose 	: get a left portion of the rectangle
		Parameters 	: fraction of rectangle to use
		Returns 	: new RECTANGLE representing the specified left portion of the rectangle
		Info 		: 
	*/
	inline TAG_RECTANGLE leftPortion(const float portion) const
	{
		return TAG_RECTANGLE(this->left, this->left+(portion*this->width()), this->bottom, this->top);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::rightPortion
		Purpose 	: get a right portion of the rectangle
		Parameters 	: fraction of rectangle to use
		Returns 	: new RECTANGLE representing the specified right portion of the rectangle
		Info 		: 
	*/
	inline TAG_RECTANGLE rightPortion(const float portion) const
	{
		return TAG_RECTANGLE(this->right-(portion*this->width()), this->right, this->bottom, this->top);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::bottomPortion
		Purpose 	: get a bottom portion of the rectangle
		Parameters 	: fraction of rectangle to use
		Returns 	: new RECTANGLE representing the specified bottom portion of the rectangle
		Info 		: 
	*/
	inline TAG_RECTANGLE bottomPortion(const float portion) const
	{
		return TAG_RECTANGLE(this->left, this->right, this->bottom, this->bottom+(portion*this->height()));
	}


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::topPortion
		Purpose 	: get a top portion of the rectangle
		Parameters 	: fraction of rectangle to use
		Returns 	: new RECTANGLE representing the specified top portion of the rectangle
		Info 		: 
	*/
	inline TAG_RECTANGLE topPortion(const float portion) const
	{
		return TAG_RECTANGLE(this->left, this->right, this->top-(portion*this->height()), this->top);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::vCentrePortion
		Purpose 	: get a rectangle with the specified portion of this rectangle's height, and the same centre point
		Parameters 	: portion of height to use
		Returns 	: a rectangle with the specified portion of this rectangle's height, and the same centre point!
		Info 		: 
	*/
	TAG_RECTANGLE vCentrePortion(const float portion) const;


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::hCentrePortion
		Purpose 	: get a rectangle with the specified portion of this rectangle's width, and the same centre point
		Parameters 	: portion of height to use
		Returns 	: a rectangle with the specified portion of this rectangle's width, and the same centre point!
		Info 		: 
	*/
	TAG_RECTANGLE hCentrePortion(const float portion) const;


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::contain
		Purpose 	: contain a 2D point within the rectangle
		Parameters 	: 2D position to contain, (opt/0.0f) radius of point to contain
		Returns 	: ref to contained 2D position
		Info 		: 
	*/
	inline TBVector2D& contain(TBVector2D& pos, const float radius=0.0f) const
	{
		if(pos[X]-radius < this->left)
		{
			pos[X]=this->left+radius;
		}
		else if(pos[X]+radius > this->right)
		{
			pos[X]=this->right-radius;
		}

		if(pos[Y]-radius < this->bottom)
		{
			pos[Y]=this->bottom+radius;
		}
		else if(pos[Y]+radius > this->top)
		{
			pos[Y]=this->top-radius;
		}

		return pos;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::contain
		Purpose 	: contain a 3D point within the rectangle
		Parameters 	: 3D position to contain, (opt/0.0f) radius of point to contain,
						axis represented by the rectangle's width,
						axis represented by the rectangle's height
		Returns 	: ref to contained 3D position
		Info 		: 
	*/
	inline TBVector& contain(TBVector& pos, const float radius=0.0f, const int axis1=X, const int axis2=Z) const
	{
		if(pos[axis1]-radius < this->left)
		{
			pos[axis1]=this->left+radius;
		}
		else if(pos[axis1]+radius > this->right)
		{
			pos[axis1]=this->right-radius;
		}

		if(pos[axis2]-radius < this->bottom)
		{
			pos[axis2]=this->bottom+radius;
		}
		else if(pos[axis2]+radius > this->top)
		{
			pos[axis2]=this->top-radius;
		}

		return pos;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::contains
		Purpose 	: determine whether or not the rectangle contains a specified 2D position
		Parameters 	: x position, y position
		Returns 	: whether or not the rectangle contains a specified 2D position
		Info 		: DON'T CONFUSE THIS WITH RECTANGLE::contain WILL YOU!
	*/
	inline bool contains(const float xPos, const float yPos) const
	{
		return	(
						(xPos>this->left)
					&&	(xPos<this->right)
					&&	(yPos>this->bottom)
					&&	(yPos<this->top)
				);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::closestPointToLine
		Purpose 	: determine the closest point, on the perimeter of the rectangle, to a line.
		Parameters 	: ptr to 2d line to test, 2D vector to fill in with result
		Returns 	: 
		Info 		: // PP: NOTE: the test line is treated as inifinite!
	*/
	void getClosestPointToLine(	const TBCollisionLine2D* const	line,
								TBVector2D						closestPoint) const;


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::removeMargin
		Purpose 	: get a version of this rectangle with the specified margin stripped out
		Parameters 	: margin to strip out
		Returns 	: version of this rectangle with the specified margin stripped out
		Info 		: assumes that left<right & bottom<top
	*/
	inline TAG_RECTANGLE removeMargin(const float margin) const
	{
		return TAG_RECTANGLE(this->left+margin, this->right-margin, this->bottom+margin, this->top-margin);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::toOrigin
		Purpose 	: get a version of this rectangle which is centred about 0,0
		Parameters 	: 
		Returns 	: a version of this rectangle which is centred about 0,0
		Info 		: 
	*/
	TAG_RECTANGLE toOrigin(void) const;


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::draw
		Purpose 	: simple drawing of the rectangle
		Parameters 	: (opt/default)colour, (opt/default)flags, (opt/X)width axis, (opt/Y)height axis, (opt/0.0f)depth
		Returns 	: 
		Info 		: I don't plan to make this any more complicated - it's more for debugging purposes
	*/
	void draw(	const COLOUR& colour=RECTANGLE_DEFAULT_DRAW_COLOUR,
				const int flags=RECTANGLE_DEFAULT_DRAW_FLAGS,
				const int widthAxis=X,
				const int heightAxis=Y,
				const float depth=0.0f) const;


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::moveX
		Purpose 	: create a version of the rectangle moved by the specified distance along the X
		Parameters 	: distance along the X by which to move
		Returns 	: a version of the rectangle moved by the specified distance along the X
		Info 		: doesn't alter *this
	*/
	inline TAG_RECTANGLE moveX(const float offset) const
	{
		return TAG_RECTANGLE(	this->left+offset,
								this->right+offset,
								this->bottom,
								this->top);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::moveY
		Purpose 	: create a version of the rectangle moved by the specified distance along the Y
		Parameters 	: distance along the Y by which to move
		Returns 	: a version of the rectangle moved by the specified distance along the Y
		Info 		: doesn't alter *this
	*/
	inline TAG_RECTANGLE moveY(const float offset) const
	{
		return TAG_RECTANGLE(	this->left,
								this->right,
								this->bottom+offset,
								this->top+offset);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: RECTANGLE::moveY
		Purpose 	: create a version of the rectangle moved by the specified X and Y distances
		Parameters 	: distance along the X by which to move, distance along the Y by which to move, 
		Returns 	: a version of the rectangle moved by the specified distance along the Y
		Info 		: doesn't alter *this
	*/
	inline TAG_RECTANGLE move(const float xOffset, const float yOffset) const
	{
		return this->moveX(xOffset).moveY(yOffset);
	}


	// PP: RECTANGLE multiplication by a constant
	friend struct TAG_RECTANGLE operator * (const struct TAG_RECTANGLE& rect, const float factor);

	// PP: RECTANGLE multiplication by a constant, with assignment
	friend struct TAG_RECTANGLE operator *= (struct TAG_RECTANGLE& rect, const float factor);

	// PP: RECTANGLE division by a constant
	friend struct TAG_RECTANGLE operator / (const struct TAG_RECTANGLE& rect, const float factor);

	// PP: RECTANGLE division by a constant, with assignment
	friend struct TAG_RECTANGLE operator /= (struct TAG_RECTANGLE& rect, const float factor);


}RECTANGLE, _RECTANGLE BALIGN16;


// PP: RECTANGLE multiplication by a constant
inline RECTANGLE operator * (const RECTANGLE& rect, const float factor)
{
	return RECTANGLE(	rect.left*factor,
						rect.right*factor,
						rect.bottom*factor,
						rect.top*factor);
}

// PP: RECTANGLE multiplication by RECTANGLE
RECTANGLE operator * (const RECTANGLE& rect, const RECTANGLE& rect2);

// PP: RECTANGLE multiplication by TBMatrix
// PP: moved it into Rectangle.cpp
RECTANGLE operator * (const RECTANGLE& rect, const TBMatrix mat);

// PP: RECTANGLE multiplication by a TBMatrix, with assignment
inline RECTANGLE operator *= (RECTANGLE& rect, const TBMatrix mat)
{
	return (rect=rect*mat);
}

// PP: RECTANGLE multiplication by a constant, with assignment
inline RECTANGLE operator *= (RECTANGLE& rect, const float factor)
{
	return (rect=rect*factor);
}

// PP: RECTANGLE multiplication by RECTANGLE, with assignment
// PP: I can't think how to describe this right now - ask me if you're confused.
inline RECTANGLE operator *= (RECTANGLE& rect, const RECTANGLE& rect2)
{
	return (rect=rect*rect2);
}

// PP: RECTANGLE division by a constant
inline RECTANGLE operator / (const RECTANGLE& rect, const float factor)
{
	return RECTANGLE(	rect.left/factor,
						rect.right/factor,
						rect.bottom/factor,
						rect.top/factor);
}

// PP: RECTANGLE division by a constant, with assignment
inline RECTANGLE operator /= (RECTANGLE& rect, const float factor)
{
	return (rect=rect/factor);
}


// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// PP: Some handy pre-defined rectangles...
// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


// PP: a 1*1 square with its bottom-left corner at the origin
#define URECT		RECTANGLE(0.0f, 1.0f, 0.0f, 1.0f)

// PP: a 1*1 square centred around the origin
#define UORECT		RECTANGLE(-0.5f, 0.5f, -0.5f, 0.5f)


#endif // PP: ndef __H_RECTANGLE