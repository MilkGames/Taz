// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : meshHelper.h
//   Purpose : generic mesh drawing(helping) functions
// Component : Generic Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BACTOR_MESHHELPER_H__
#define __BACTOR_MESHHELPER_H__

// ********************************************************************************
// Constants and Macros


// ********************************************************************************
// Types, Structures and Classes
typedef struct _TBMeshColour
{
	unsigned char			r, g, b, a; // one of these, instead of 4 chars passed.
} TBMeshColour;

// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : baDrawCollisionMeshCell
	Purpose : Draw a TBCollisionMeshCell structure (in the context of the TBCollisionMesh it is associated with)
	Parameters : [in] the mesh 'context' that the cell takes it's data from
				[in] the cell that we draw.
				[in] the colour of the mesh
	Returns : void
	Info : 
*/

void baDrawCollisionMeshCell(const TBCollisionMesh * const mesh, const TBCollisionMeshCell * const cell, const TBMeshColour col);


/*	--------------------------------------------------------------------------------
	Function : baDrawCollisionMesh
	Purpose : Draws all of the cells that a TBCollisionMesh structure contains.
	Parameters : [in] the mesh to draw.
				[in] the colour of the mesh
	Returns : void
	Info : 
*/

void baDrawCollisionMesh(const TBCollisionMesh * const mesh, const TBMeshColour);


/*	--------------------------------------------------------------------------------
	Function : baDrawCylinder
	Purpose : Draw a cylinder, from a TBCollisionCylinder structure.
	Parameters : [in] the cylinder definition.
				[in] the number of sides to draw.
				[in] the colour of the mesh
	Returns : void
	Info : 
*/

void baDrawCylinder(const TBCollisionCylinder * const cylinder, const int sideCount, const TBMeshColour col);


/*	--------------------------------------------------------------------------------
	Function : baDrawLine
	Purpose : Draw a line, from a TBCollisionLine structure.
	Parameters : [in] the line definition.
				[in] the colour of the mesh
	Returns : void
	Info : 
*/

void baDrawLine(const TBCollisionLine * const line, const TBMeshColour colour);


/*	--------------------------------------------------------------------------------
	Function : baDrawPoint
	Purpose : Draw a point, from a TBVector structure.
	Parameters : [in] the point definition.
				[in] the colour of the mesh
	Returns : void
	Info : 
*/

void baDrawPoint(const TBVector point, const TBMeshColour colour);


/*	--------------------------------------------------------------------------------
	Function : baDrawBox
	Purpose : Draw a box from a TBCollisionBox structure.
	Parameters : [in] the box
				[in] the colour of the box.
	Returns : 
	Info : 
*/

void baDrawBox(const TBCollisionBox * const box, const TBMeshColour colour, const TBMatrix baseTransform=bIdentityMatrix);


/*	--------------------------------------------------------------------------------
	Function : baDrawAnimatingCollisionMesh
	Purpose : Draw an animating collision mesh instance in the specified colour.
	Parameters : [in] the mesh to draw.
				[in] the flags to alter the behaviour of the mesh. 0x1 to draw the mesh, 0x2 to draw the bounding boxes.
				[in] the colour to draw the mesh in.
	Returns : whether the draw was successful or not (1 or 0)
	Info : sets texture to NULL on stage 0.
*/

int baDrawAnimatingCollisionMesh(const TBACMeshInstance * const mesh, const uint32 flags, const TBMeshColour col);


#endif /* __BACTOR_MESHHELPER_H__ */