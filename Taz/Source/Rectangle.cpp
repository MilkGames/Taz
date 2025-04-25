// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Rectangle.h
//   Purpose : Handy rectangle class
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"				// PP: global stuff
#include "display.h"			// PP: display stuff
#include "Rectangle.h"			// PP: Handy rectangle class


/*	--------------------------------------------------------------------------------
	Function 	: RECTANGLE::setWidth
	Purpose 	: set the width of the rectangle
	Parameters 	: new width for the rectangle
	Returns 	: 
	Info 		: 
*/
void RECTANGLE::setWidth(const float newWidth)
{
	float		mid;
	float		halfWidth;

	mid=this->midX();

	halfWidth=newWidth*0.5f;

	this->left=(mid-halfWidth);
	this->right=(mid+halfWidth);
}


/*	--------------------------------------------------------------------------------
	Function 	: RECTANGLE::setHeight
	Purpose 	: set the height of the rectangle
	Parameters 	: new height for the rectangle
	Returns 	: 
	Info 		: 
*/
void RECTANGLE::setHeight(const float newHeight)
{
	float		mid;
	float		halfHeight;

	mid=this->midY();

	halfHeight=newHeight*0.5f;

	this->bottom=(mid-halfHeight);
	this->top=(mid+halfHeight);
}


/*	--------------------------------------------------------------------------------
	Function 	: RECTANGLE::mulWidth
	Purpose 	: multiply the width of the rectangle
	Parameters 	: factor by which to multiply the width for the rectangle
	Returns 	: 
	Info 		: this should be a teensy more efficient than saying "rect.setWidth(rect.width()*1.5f)"
*/
void RECTANGLE::mulWidth(const float factor)
{
	float	halfWidth;
	float	mid;

	halfWidth=this->width()/2.0f;
	mid=this->left+halfWidth;

	halfWidth*= factor;

	this->left=(mid-halfWidth);
	this->right=(mid+halfWidth);
}


/*	--------------------------------------------------------------------------------
	Function 	: RECTANGLE::mulHeight
	Purpose 	: multiply the height of the rectangle
	Parameters 	: factor by which to multiply the height for the rectangle
	Returns 	: 
	Info 		: this should be a teensy more efficient than saying "rect.setHeight(rect.height()*1.5f)"
*/
void RECTANGLE::mulHeight(const float factor)
{
	float	halfHeight;
	float	mid;

	halfHeight=this->height()/2.0f;
	mid=this->bottom+halfHeight;

	halfHeight *= factor;

	this->bottom=(mid-halfHeight);
	this->top=(mid+halfHeight);
}


/*	--------------------------------------------------------------------------------
	Function 	: RECTANGLE::vCentrePortion
	Purpose 	: get a rectangle with the specified portion of this rectangle's height, and the same centre point
	Parameters 	: portion of height to use
	Returns 	: a rectangle with the specified portion of this rectangle's height, and the same centre point!
	Info 		: 
*/
TAG_RECTANGLE RECTANGLE::vCentrePortion(const float portion) const
{
	float	gap=(this->height()*(1.0f-portion))*0.5f;

	return TAG_RECTANGLE(this->left, this->right, this->bottom+gap, this->top-gap);
}


/*	--------------------------------------------------------------------------------
	Function 	: RECTANGLE::hCentrePortion
	Purpose 	: get a rectangle with the specified portion of this rectangle's width, and the same centre point
	Parameters 	: portion of height to use
	Returns 	: a rectangle with the specified portion of this rectangle's width, and the same centre point!
	Info 		: 
*/
TAG_RECTANGLE RECTANGLE::hCentrePortion(const float portion) const
{
	float	gap;

	gap=(this->width()*(1.0f-portion))*0.5f;

	return TAG_RECTANGLE(this->left+gap, this->right-gap, this->bottom, this->top);
}


/*	--------------------------------------------------------------------------------
	Function 	: RECTANGLE::closestPointToLine
	Purpose 	: determine the closest point, on the perimeter of the rectangle, to a line.
	Parameters 	: ptr to 2d line to test, 2D vector to fill in with result
	Returns 	: 
	Info 		: // PP: NOTE: the test line is treated as inifinite!
*/
void RECTANGLE::getClosestPointToLine(	const TBCollisionLine2D* const	line,
										TBVector2D						closestPoint) const
{
	// PP: I'm not convinced this is the quickest way of doing it.
	// PP: Hey, if you know a quicker route, by all means change the function!

	TBCollisionInfo2D		collisionInfo;
	TBCollisionLine2D		topLine, rightLine, bottomLine, leftLine;

	topLine.start[X]=leftLine.end[X]=this->left;
	topLine.start[Y]=leftLine.end[Y]=this->top;
	topLine.end[X]=rightLine.start[X]=this->right;
	topLine.end[Y]=rightLine.start[Y]=this->top;
	rightLine.end[X]=bottomLine.start[X]=this->right;
	rightLine.end[Y]=bottomLine.start[Y]=this->bottom;
	bottomLine.end[X]=leftLine.start[X]=this->left;
	bottomLine.end[Y]=leftLine.start[Y]=this->bottom;

	collisionInfo.flags=BMCOLLINFO2D_RETURNPOSITION;

	// PP: test for an intersection first (an intersection point will of course be the closest point)...

	if(bmCollideLineLine2D(line, &topLine, &collisionInfo))
	{
		closestPoint[X]=collisionInfo.position[X];
		closestPoint[Y]=collisionInfo.position[Y];

		return;
	}
	else if(bmCollideLineLine2D(line, &rightLine, &collisionInfo))
	{
		closestPoint[X]=collisionInfo.position[X];
		closestPoint[Y]=collisionInfo.position[Y];

		return;
	}
	else if(bmCollideLineLine2D(line, &bottomLine, &collisionInfo))
	{
		closestPoint[X]=collisionInfo.position[X];
		closestPoint[Y]=collisionInfo.position[Y];

		return;
	}
	else if(bmCollideLineLine2D(line, &leftLine, &collisionInfo))
	{
		closestPoint[X]=collisionInfo.position[X];
		closestPoint[Y]=collisionInfo.position[Y];

		return;
	}
	else
	{
		// PP: TEMP TEST!!!!!!
	/*	closestPoint[X]=FLOATMAX;
		closestPoint[Y]=FLOATMAX;
		return;*/

		// PP: no intersections; return the position of the closest corner point to the line...

		float			dist, minDist;
		TBVector2D*		closestStartPoint= &topLine.start;
		TBVector2D		tempClosestPoint;// PP: note: this is of no use to me

		// PP: top

		minDist=bmGeomDistanceFromLine2D(line, topLine.start, tempClosestPoint);

		if((dist=bmGeomDistanceFromLine2D(line, rightLine.start, tempClosestPoint)) < minDist)
		{
			minDist=dist;
			closestStartPoint= &rightLine.start;
		}

		if((dist=bmGeomDistanceFromLine2D(line, bottomLine.start, tempClosestPoint)) < minDist)
		{
			minDist=dist;
			closestStartPoint= &bottomLine.start;
		}

		if((dist=bmGeomDistanceFromLine2D(line, leftLine.start, tempClosestPoint)) < minDist)
		{
			closestPoint[X]=leftLine.start[X];
			closestPoint[Y]=leftLine.start[Y];

			return;
		}
		else
		{
			closestPoint[X]= (*closestStartPoint)[X];
			closestPoint[Y]= (*closestStartPoint)[Y];
			
			return;
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: RECTANGLE::toOrigin
	Purpose 	: get a version of this rectangle which is centred about 0,0
	Parameters 	: 
	Returns 	: a version of this rectangle which is centred about 0,0
	Info 		: 
*/
TAG_RECTANGLE RECTANGLE::toOrigin(void) const
{
	float		midXcopy=this->midX();
	float		midYcopy=this->midY();

	return TAG_RECTANGLE(this->left-midXcopy, this->right-midXcopy, this->bottom-midYcopy, this->top-midYcopy);
}


/*	--------------------------------------------------------------------------------
	Function 	: RECTANGLE::draw
	Purpose 	: simple drawing of the rectangle
	Parameters 	: (opt/default)colour, (opt/default)flags, (opt/X)width axis, (opt/Y)height axis, (opt/0.0f)depth 
	Returns 	: 
	Info 		: I don't plan to make this any more complicated - it's more for debugging purposes
*/
#define RECTANGLE_ALPHAFILL_OPACITY			0.3f

void RECTANGLE::draw(const COLOUR& colour, const int flags, const int widthAxis, const int heightAxis, const float depth) const
{
	int				otherAxis=X+Y+Z-(widthAxis+heightAxis);

	// PP: DRAW FILL...

/* PP: REMOUT: NEW TEST FOR SHADOW MAPS	
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);*/
	bdSetTexture(0,NULL);

	COLOUR			fillColour;
	bool			doneOutlineVerts=false;
	int				v;

	if(flags&ALPHAFILL)
	{
		fillColour=COLOUR(colour.r, colour.g, colour.b, (unsigned char)(colour.a*RECTANGLE_ALPHAFILL_OPACITY));
	}
	else
	{
		fillColour=colour;
	}

	TBPrimVertex	fillVerts[4];
	float			pos;

	for(v=0; v<4; v++)
	{
		BDVERTEX_SETCOLOUR(&fillVerts[v], fillColour);
		BDVERTEX_SETW(&fillVerts[v], 1.0f);

		pos=(v&1)?this->left:this->right;

		if(widthAxis == X)
		{
			BDVERTEX_SETX(&fillVerts[v], pos);
		}
		else if(widthAxis == Y)
		{
			BDVERTEX_SETY(&fillVerts[v], pos);
		}
		else// PP: if(widthAxis == Z)
		{
			BDVERTEX_SETZ(&fillVerts[v], pos);
		}

		pos=(v/2)?this->bottom:this->top;

		if(heightAxis == X)
		{
			BDVERTEX_SETX(&fillVerts[v], pos);
		}
		else if(heightAxis == Y)
		{
			BDVERTEX_SETY(&fillVerts[v], pos);
		}
		else// PP: if(widthAxis == Z)
		{
			BDVERTEX_SETZ(&fillVerts[v], pos);
		}

		if(otherAxis == X)
		{
			BDVERTEX_SETX(&fillVerts[v], depth);
		}
		else if(otherAxis == Y)
		{
			BDVERTEX_SETY(&fillVerts[v], depth);
		}
		else// PP: if(otherAxis == Z)
		{
			BDVERTEX_SETZ(&fillVerts[v], depth);
		}
	}

	if(flags&(SOLIDFILL|ALPHAFILL))
	{
		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,fillVerts,4,BVERTTYPE_SINGLE);
	}

	// PP: DRAW OUTLINE...

	TBPrimVertex	outlineVerts[8];

	if(flags & OUTLINE)
	{
		outlineVerts[0]=outlineVerts[7]=fillVerts[0];
		outlineVerts[1]=outlineVerts[2]=fillVerts[1];
		outlineVerts[3]=outlineVerts[4]=fillVerts[3];
		outlineVerts[5]=outlineVerts[6]=fillVerts[2];

		for(v=0; v<8; v++)
		{
			BDVERTEX_SETCOLOUR(&outlineVerts[v], colour);
		}

#if 0// PP: REMOUT: I'll try the PS2 lines again		BPLATFORM == PS2
		float x1,y1,z1,w1,x2,y2,z2,w2;

		for (int i=0;i<4;i++)
		{
			BDVERTEX_GETXYZW(&outlineVerts[(i*2)+0],x1,y1,z1,w1);
			BDVERTEX_GETXYZW(&outlineVerts[(i*2)+1],x2,y2,z2,w2);
			DrawLine(x1,y1,x2,y2,colour);
		}
#else
		bdDrawPrimitive(BDPRIMTYPE_LINELIST,outlineVerts,8,BVERTTYPE_SINGLE);
#endif
	}
}


// PP: RECTANGLE multiplication by RECTANGLE
RECTANGLE operator * (const RECTANGLE& rect, const RECTANGLE& rect2)
{
	float	rectWidth=rect.width();
	float	rectHeight=rect.height();

	return RECTANGLE(rect.left+(rect2.left*rectWidth),
						rect.left+(rect2.right*rectWidth),
						rect.bottom+(rect2.bottom*rectHeight),
						rect.bottom+(rect2.top*rectHeight)
						);
}


// PP: RECTANGLE multiplication by TBMatrix
RECTANGLE operator * (const RECTANGLE& rect, const TBMatrix mat)
{
	VEC		bottomLeft;
	VEC		topRight;

	bottomLeft=VEC(rect.left, rect.bottom);
	topRight=VEC(rect.right, rect.top);

	bottomLeft*=mat;
	topRight*=mat;

	return RECTANGLE(bottomLeft[X], topRight[X], bottomLeft[Y], topRight[Y]);
}