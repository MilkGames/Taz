// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games
//
//      File : xbSave.h
//   Purpose : Xbox specific loading & saving of game data
// Component : Xbox Input
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BINPUT_XBSAVE_H__
#define __BINPUT_XBSAVE_H__


// ********************************************************************************
// Constants and Macros

#define BGAMESAVE_MAX_CARDSLOTS					9	// Number of memory card slots (8+HD)

#define BGAMESAVE_CARDSLOT_HD					(BGAMESAVE_MAX_CARDSLOTS-1)

#define BGAMESAVE_MAX_CARDNAME_LEN				(MAX_MUNAME+1)

// ********************************************************************************
// Types and Structures

typedef struct _TBGameSaveInfo {
	ushort		cardName[BGAMESAVE_MAX_CARDNAME_LEN];
	TBTexture	*icon;	// 64*64
} TBGameSaveInfo;


// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bInitGameSave
   Purpose : Initialise gamesave module
   Parameters : 
   Returns : OK/FAIL
   Info : 
*/

int bInitGameSave();


/* --------------------------------------------------------------------------------
   Function : bShutdownGameSave
   Purpose : Shutdown gamesave module
   Parameters : 
   Returns :
   Info : 
*/

void bShutdownGameSave();


/* --------------------------------------------------------------------------------
   Function : bGameSavePoll
   Purpose : Proceed with the game save process
   Parameters :  pointer to save data, save name prefix, full save name, save size, dir entry list, number of
				dir entries to get, extra save info, file to delete
   Returns :
   Info : 
*/

void bGameSavePoll(void *saveDataPtr, ushort *saveNamePrefix, ushort *saveDataName, int saveDataSize, TBGameSaveDirEntry *dirEntries, int noofDirEntries, int *noofFiles, TBGameSaveInfo *saveInfo, int deleteFile);


/* --------------------------------------------------------------------------------
   Function : bGameSaveReset
   Purpose : reset all of the status flags
   Parameters : 
   Returns :
   Info : 
*/

void bGameSaveReset();


/* --------------------------------------------------------------------------------
   Function : biGameSaveGetCardSlotStatus
   Purpose : Get status of specified slot
   Parameters : card slot to check, pointer to recieve media size (bytes), pointer to recieve free size (bytes)
   Returns : indication if slot contains media, 
   Info : 
*/

EBGameSaveSlotStatus biGameSaveGetCardSlotStatus(int slot, long *size=0, long *free=0);


/* --------------------------------------------------------------------------------
   Function : biGameSaveGetPercentComplete
   Purpose : Get percent of read/write done
   Parameters : 
   Returns : percent of read/write complete
   Info : 
*/

int biGameSaveGetPercentComplete();


/* --------------------------------------------------------------------------------
   Function : biGameSaveSetActiveCardSlot
   Purpose : Set active card slot
   Parameters : slot to choose
   Returns : OK/FAIL
   Info : Returns fail if slot has no media or is not valid
*/

int biGameSaveSetActiveCardSlot(int slot);


/* --------------------------------------------------------------------------------
   Function : biGameSaveGetActiveCardSlot
   Purpose : Get current active card slot
   Parameters :
   Returns : Active slot
   Info : 
*/

int biGameSaveGetActiveCardSlot();


#endif // __BINPUT_XBSAVE_H__