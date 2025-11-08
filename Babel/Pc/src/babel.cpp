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
    return FAIL;
  }

  if (!bInitHeap(base,size)) {
    bShutdownKernel();
    return FAIL;
  }

  bInitCommandLine();
  // MG: placed it here so I wouldn't forget about it
  bkPrintf("*** WARNING *** bdConsoleWindowPrintf was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
  bDumpDXVersions();
  if (!bInitDisplay()) {
    bDumpDXVersions();
    bShutdownHeap();
    bShutdownKernel();
	return FAIL; // they forgot to add this
  }

  if (!bInitActor()) {
    bDumpDXVersions();
    bShutdownHeap();
    bShutdownInput();
    bShutdownDisplay();
    bShutdownKernel();
    return FAIL;
  }

  if (!bInitSound()) {
	bDumpDXVersions();
	bShutdownActor();
	bShutdownHeap();
	bShutdownInput();
	bShutdownDisplay();
	bShutdownKernel();
	return FAIL;
  }

  return OK;
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
    bKernelShutdownBkgLoad();
	bShutdownSound();
	bShutdownActor();
	bShutdownInput();
	bShutdownDisplay();
	bShutdownKernel();
	bShutdownCommandLine();
	bShutdownHeap();
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
	if (modules & BUPDATEMODULE_BKERNEL) {
		bPumpMessages();
	}
	
	if (modules & BUPDATEMODULE_BSOUND) {
		bUpdateSound();
		if (bNoofOSEvents != 0) {
			bHandleOSEvents();
		}
	}
}