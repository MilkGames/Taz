// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : kernel.cpp
//   Purpose : kernel functions
// Component : Babel Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Globals

// ********************************************************************************
// Function Implementations

// ********************************************************************************
// NON-EXISTING ON XBOX FUNCTIONS


/* --------------------------------------------------------------------------------
   Function : bkGenerateUniqueFilename
   Purpose : Generate a unique filename based on frame number, time and date
   Parameters : buffer to fill, prefix name, file extension
   Returns : pointer to filename buffer
   Info : Filename buffer must be > strlen(prefix) + SCREENSHOT_FILENAME_GENERATED_LEN + strlen(suffix)
*/

char *bkGenerateUniqueFilename(char *filename, const char *prefix, const char *ext)
{
        bkPrintf("*** WARNING *** bkGenerateUniqueFilename was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}


/*	--------------------------------------------------------------------------------
	Function : bkSetMediaType
	Purpose : set the media type we are using
	Parameters : media type
	Returns :
	Info : PS2 implementation only, stubbed out on all other platforms
*/

//#define bkSetMediaType(media) already defined in kernel.h