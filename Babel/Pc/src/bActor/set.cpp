// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games
//
//      File : set.cpp
//   Purpose : generic set functions
// Component : Generic Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bInitSet
	Purpose : initialise the set module
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/
int bInitSet()
{
        bkPrintf("*** WARNING *** bInitSet was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bShutdownSet
	Purpose : shutdown the set module
	Parameters : 
	Returns : 
	Info : 
*/
void bShutdownSet()
{
    return; // empty, confirmed
}

/*	--------------------------------------------------------------------------------
	Function : bLoadSetByCRC
	Purpose : load a set from storage
	Parameters : package index, crc
	Returns : ptr to set or NULL for failure
	Info : 
*/

TBSet *bLoadSetByCRC(TBPackageIndex *index, uint32 crc)
{
        bkPrintf("*** WARNING *** bLoadSetByCRC was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}

/*	--------------------------------------------------------------------------------
	Function : bDeleteSet
	Purpose : delete a set
	Parameters : actor
	Returns : 
	Info : 
*/
void bDeleteSet(TBSet *actor)
{
        bkPrintf("*** WARNING *** bDeleteSet was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baDrawSet
	Purpose : draw a set
	Parameters : set, alpha
	Returns : 
	Info : 
*/
void baDrawSet(TBSet *set, int alpha)
{
        bkPrintf("*** WARNING *** baDrawSet was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baGetSetGeometry
	Purpose : retrieve the stored geometry from a set
	Parameters : set, stream number (0..3), ptr to fill with #vertices (or NULL), ptr to fill with #tris (or NULL)
	Returns : ptr to stream data
	Info : 
*/

void *baGetSetGeometry(TBSet *set, int stream, int *noofVertices, int *noofTris)
{
        bkPrintf("*** WARNING *** baGetSetGeometry was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}