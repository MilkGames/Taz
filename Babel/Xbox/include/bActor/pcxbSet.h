// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games
//
//      File : pcxbSet.h
//   Purpose : platform specific set functions
// Component : PC/Xbox Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BACTOR_PCSET_H__
#define __BACTOR_PCSET_H__


// ********************************************************************************
// Types and Structures

// PC specific info for the "Regular Cells" set
typedef struct _TBSetRegCellHAL {
	TBVertexBuffer				*vertBuffer;			// vertex buffer for cell verts
	int							*outCodes;				// outcode buffer
	uint32						totalVertices;			// total #vertices in set
	uint32						pad1;
} TBSetRegCellHAL;


// a cell primitive
typedef struct _TBRegCellPrim {
	int32						type;					// type of primitive (BDPRIMTYPE_TRIANGLELIST or BDPRIMTYPE_TRIANGLESTRIP)
	TBTexture					*texture;				// texture (crc in file, ptr in memory)
	int32						noofPrimVerts;			// #prim verts in this prim
	ushort						noofVerts;				// #verts used by this prim
	ushort						flags;					// flags (see BPRIMFLAG_)

	TBTexture					*texture2;				// texture #2 (crc in file, ptr in memory)
	uint32						pad;
} TBRegCellPrim;


// a cell
typedef struct _TBRegCell {
	int							noofPrims;				// #primitives in this cell
	int							noofVerts;				// #verts in this cell
	TBRegCellPrim				*prims;					// ptr to prims (offset in file)
	int							firstVert;				// ID of first vert

	int32						noofSolidPrims;			// # solid prims
	int32						noofCKeyPrims;			// # colour-key prims
	int32						noofAlphaPrims;			// # alpha prims
	int32						firstCKeyVert;			// ID of first colour-key prim vert

	int32						firstAlphaVert;			// ID of first alpha prim vert
	uint32						pad1;
	uint32						pad2;
	uint32						pad3;

	uint32						pad4;
	uint32						pad5;
	uint32						pad6;
	uint32						pad7;
} TBRegCell;


// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : bInitSetPlatformSpecific
	Purpose : platform specific set init code
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/

int bInitSetPlatformSpecific();


/*	--------------------------------------------------------------------------------
	Function : bShutdownSetPlatformSpecific
	Purpose : platform specific set shutdown code
	Parameters : 
	Returns : 
	Info : 
*/

void bShutdownSetPlatformSpecific();

#endif		// __BACTOR_PCSET_H__
