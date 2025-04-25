// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games
//
//      File : set.h
//   Purpose : generic set functions
// Component : Generic Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BACTOR_SET_H__
#define __BACTOR_SET_H__

// ********************************************************************************
// Constants and Macros

// set construction methods
enum {
	BSETMETHOD_REGCELLS,									// regular cell array

	BSETMETHOD_NOOF											// #set construction methods
};

// regular cells flags
#define REGCELLSFLAG_TRILINEAR		0x00000001				// trilinear is enabled
#define REGCELLSFLAG_RESOLUTION		0x00000002				// specifying grid by resolution instead of cell size

// alpha styles (set's can only use one style at a time)
#define BSETPRIMALPHATYPE_SOLID		0x00000001				// prims with no alpha at all - these are drawn first
#define BSETPRIMALPHATYPE_CKEY		0x00000002				// prims with colour key but no variable texture alpha or vertex alpha - these are drawn next
#define BSETPRIMALPHATYPE_ALPHA		0x00000004				// prims with variable texture alpha or vertex alpha - these are drawn last


// ********************************************************************************
// Types, Structures and Classes

// base structure for Set resources (base struct for all others so Sets are polymorphic with this info)
typedef struct _TBSet {
	TBResourceInfo			resInfo;						// global resource info

	int32					method;							// construction method (see BSETMETHOD_ above)
	int32					pad1, pad2, pad3;

	TBStoredGeometry		storedGeometry;					// data streams stored with this set
} TBSet;



// --------------------------------------------------------------------------------
// "Regular Cells" construction method

// "Regular Cells" set
typedef struct _TBSetRegCells {
	TBSet					setInfo;								// standard base Set info (resource info, construction method etc.)

	int32					xNoofCells, yNoofCells, zNoofCells;		// #cells in each axis
	float					xBoundMin;								// X min bound
	
	float					xBoundMax;								// X max Bound
	float					yBoundMin, yBoundMax;					// Y Bounds
	float					zBoundMin;								// Z min bound
	
	float					zBoundMax;								// Z max Bound
	TBRegCell				*cells;									// ptr to cells (offset in file)
	uint32					flags;									// flags (see REGCELLSFLAG_)
	uint32					pad2;									

	TBSetRegCellHAL			hal;									// platform specific info
} TBSetRegCells;

// end of "Regular Cells" construction method
// --------------------------------------------------------------------------------


// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : bInitSet
	Purpose : initialise the set module
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/

int bInitSet();


/*	--------------------------------------------------------------------------------
	Function : bShutdownSet
	Purpose : shutdown the set module
	Parameters : 
	Returns : 
	Info : 
*/

void bShutdownSet();


/*	--------------------------------------------------------------------------------
	Function : bLoadSetByCRC
	Purpose : load a set from storage
	Parameters : package index, crc
	Returns : ptr to set or NULL for failure
	Info : 
*/

TBSet *bLoadSetByCRC(TBPackageIndex *index, uint32 crc);


/*	--------------------------------------------------------------------------------
	Function : bDeleteSet
	Purpose : delete a set
	Parameters : set ptr
	Returns : 
	Info : 
*/

void bDeleteSet(TBSet *actor);


/*	--------------------------------------------------------------------------------
	Function : baDrawSet
	Purpose : draw a set
	Parameters : set ptr, alpha type mask
	Returns : 
	Info : 
*/

void baDrawSet(TBSet *set, int alpha = BSETPRIMALPHATYPE_SOLID);

#define baDrawSetAlpha(set) { baDrawSet((set), EBSETPRIMALPHATYPE_CKEY); baDrawSet((set), EBSETPRIMALPHATYPE_ALPHA); }


/*	--------------------------------------------------------------------------------
	Function : baGetSetGeometry
	Purpose : retrieve the stored geometry from a set
	Parameters : set, stream number (0..3), ptr to fill with #vertices (or NULL), ptr to fill with #tris (or NULL)
	Returns : ptr to stream data
	Info : 
*/

void *baGetSetGeometry(TBSet *set, int stream, int *noofVertices, int *noofTris);


#endif		// __BACTOR_SET_H__
