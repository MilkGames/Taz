// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : babel.h
//   Purpose : top level include file for Babel libraries
// Component : Xbox Babel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/* --------------------------------------------------------------------------------
   Function : bkInit
   Purpose : initialise all modules
   Parameters : heap base address (NULL for dynamic), size of heap, flags
   Returns : OK/FAIL
   Info : 
*/

int bkInit(uint base, uint size, uint32 flags)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bkShutdown
   Purpose : shutdown all modules
   Parameters : 
   Returns : 
   Info : 
*/

void bkShutdown()
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bkUpdate
   Purpose : call the update function for each of the babel modules
   Parameters : module mask
   Returns : 
   Info : 
*/

void bkUpdate(int modules)
{
    return;
}