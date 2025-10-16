// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : babel.h
//   Purpose : top level include file for Babel libraries
// Component : Xbox Babel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Globals

uint32 bBkInitFlags = 0; // set your flags here

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
  bBkInitFlags = flags;

  bInitMaths();
  if (!bInitKernel()) {
    return 0;
  }

  if (!bInitHeap(base,size)) {
    bShutdownKernel();
    return 0;
  }

  bInitCommandLine();
  bDXRuntimeLogInfo();
  if (!bInitDisplay()) {
    bDXRuntimeLogInfo();
    bShutdownHeap();
    bShutdownKernel();
  }

  if (!bInitActor()) {
    bDXRuntimeLogInfo();
    bShutdownHeap();
    bShutdownInput();
    bShutdownDisplay();
    bShutdownKernel();
    return 0;
  }

  if (!bInitSound()) {
    bDXRuntimeLogInfo();
    bShutdownActor();
    bShutdownHeap();
    bShutdownInput();
    bShutdownDisplay();
    bShutdownKernel();
    return 0;
  }

  return 1;
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
        bkPrintf("*** WARNING *** bkShutdown was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bkUpdate was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}