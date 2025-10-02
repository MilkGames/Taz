// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : gamesave.cpp
//   Purpose : Defines constants for max game saves and file version. Defines enums for game save mode, slot status, response values, and game save status
// Component : Babel Input
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : biGameSaveStart
	Purpose : start game save
	Parameters : loadSave, saveData, saveNamePrefix, saveSize, flags
	Returns : 
	Info : 
*/
void biGameSaveStart(EBGameSaveMode loadSave, void *saveData, ushort *saveNamePrefix, int saveSize, int flags)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : biGameSaveGetStatus
	Purpose : get game save status
	Parameters : dirEntries, numDirEntries, numFiles, saveInfo
	Returns : status
	Info : 
*/
EBGameSaveStatus biGameSaveGetStatus(TBGameSaveDirEntry *dirEntries, int numDirEntries, int *numFiles, struct _TBGameSaveInfo *saveInfo)
{
    return BGAMESAVE_NOSTATE;
}

/*	--------------------------------------------------------------------------------
	Function : biGameSaveSetStatus
	Purpose : set game save status
	Parameters : response, saveName, deleteFile, saveInfo
	Returns : 
	Info : 
*/
void biGameSaveSetStatus(EBGameSaveResponse response, ushort *saveName, int deleteFile, struct _TBGameSaveInfo *saveInfo)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : biGameSaveEnd
	Purpose : end game save
	Parameters : 
	Returns : 
	Info : 
*/
void biGameSaveEnd()
{
    return;
}
