// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcDxdebug.cpp
//   Purpose : Defines a macro for converting DirectX error codes to strings. Declares bDXErrorToString
// Component : Babel Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bDXErrorToString
	Purpose : convert DirectX error to string
	Parameters : err
	Returns : error string
	Info : 
*/
char *bDXErrorToString(HRESULT err)
{
        bkPrintf("*** WARNING *** bDXErrorToString was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}

/* --------------------------------------------------------------------------------
   Function : bDXRuntimeLogInfo
   Purpose : dumps information about DX libraries into log file
   Parameters :
   Returns :
   Info : 
*/
void bDXRuntimeLogInfo()
{
        bkPrintf("*** WARNING *** bDXRuntimeLogInfo was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}
