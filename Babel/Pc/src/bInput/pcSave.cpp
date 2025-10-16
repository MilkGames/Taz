// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcSave.cpp
//   Purpose : Defines constants for max memory card slots, max memory card name length, and max media name length. Defines enums for game save mode, slot status, response values, and game save status
// Component : Babel Input
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bInitGameSave
	Purpose : initialise the game save module
	Parameters : 
	Returns : 
	Info : 
*/
int bInitGameSave()
{
        bkPrintf("*** WARNING *** bInitGameSave was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bShutdownGameSave
	Purpose : shutdown the game save module
	Parameters : 
	Returns : 
	Info : 
*/
void bShutdownGameSave()
{
        bkPrintf("*** WARNING *** bShutdownGameSave was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : bGameSavePoll
   Purpose : Proceed with the game save process
   Parameters :  pointer to save data, save name prefix, full save name, save size, dir entry list, number of
				dir entries to get, extra save info, file to delete
   Returns :
   Info : 
*/

void bGameSavePoll(void *saveDataPtr, ushort *saveNamePrefix, ushort *saveDataName, int saveDataSize, TBGameSaveDirEntry *dirEntries, int noofDirEntries, int *noofFiles, TBGameSaveInfo *saveInfo, int deleteFile)
{
        bkPrintf("*** WARNING *** bGameSavePoll was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : bGameSaveReset
   Purpose : reset all of the status flags
   Parameters : 
   Returns :
   Info : 
*/

void bGameSaveReset()
{
        bkPrintf("*** WARNING *** bGameSaveReset was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}


/* --------------------------------------------------------------------------------
   Function : biGameSaveGetCardSlotStatus
   Purpose : Get status of specified slot
   Parameters : card slot to check, pointer to recieve media size (bytes), pointer to recieve free size (bytes)
   Returns : indication if slot contains media, 
   Info : 
*/

EBGameSaveSlotStatus biGameSaveGetCardSlotStatus(int slot, long *size, long *free)
{
        bkPrintf("*** WARNING *** biGameSaveGetCardSlotStatus was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return BGAMESAVE_SLOT_NOMEDIA;
}


/* --------------------------------------------------------------------------------
   Function : biGameSaveGetPercentComplete
   Purpose : Get percent of read/write done
   Parameters : 
   Returns : percent of read/write complete
   Info : 
*/

int biGameSaveGetPercentComplete()
{
        bkPrintf("*** WARNING *** biGameSaveGetPercentComplete was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : biGameSaveSetActiveCardSlot
   Purpose : Set active card slot
   Parameters : slot to choose
   Returns : OK/FAIL
   Info : Returns fail if slot has no media or is not valid
*/

int biGameSaveSetActiveCardSlot(int slot)
{
        bkPrintf("*** WARNING *** biGameSaveSetActiveCardSlot was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : biGameSaveGetActiveCardSlot
   Purpose : Get current active card slot
   Parameters :
   Returns : Active slot
   Info : 
*/

int biGameSaveGetActiveCardSlot()
{
        bkPrintf("*** WARNING *** biGameSaveGetActiveCardSlot was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}