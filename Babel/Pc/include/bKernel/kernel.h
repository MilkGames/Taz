// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : kernel.h
//   Purpose : kernel functions
// Component : Generic Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BKERNEL_KERNEL_H__
#define __BKERNEL_KERNEL_H__

// ********************************************************************************
// Constants and macros

#define SCREENSHOT_FILENAME_GENERATED_LEN	((5)+1+(2+2)+1+(3*2)+1) // +1 for NULL terminator

// flags for the bkUpdate function
#define BUPDATEMODULE_BDISPLAY	0x00000001
#define BUPDATEMODULE_BACTOR	0x00000002
#define BUPDATEMODULE_BMATHS	0x00000004
#define BUPDATEMODULE_BKERNEL	0x00000008
#define BUPDATEMODULE_BINPUT	0x00000010
#define BUPDATEMODULE_BGUI		0x00000020
#define BUPDATEMODULE_BSOUND	0x00000040
#define BUPDATEMODULE_FCAMERA	0x00000080
#define BUPDATEMODULE_FGUI		0x00000100
#define BUPDATEMODULE_FSCENE	0x00000200
#define BUPDATEMODULE_FEFFECTS	0x00000400
#define BUPDATEMODULE_ALL		0x0000FFFF

// ********************************************************************************
// Types, structures and classes


// ********************************************************************************
// Globals



// ********************************************************************************
// Prototypes


/* --------------------------------------------------------------------------------
   Function : bkGenerateUniqueFilename
   Purpose : Generate a unique filename based on frame number, time and date
   Parameters : buffer to fill, prefix name, file extension
   Returns : pointer to filename buffer
   Info : Filename buffer must be > strlen(prefix) + SCREENSHOT_FILENAME_GENERATED_LEN + strlen(suffix)
*/

char *bkGenerateUniqueFilename(char *filename, const char *prefix, const char *ext);


#if BPLATFORM != PS2
/*	--------------------------------------------------------------------------------
	Function : bkSetMediaType
	Purpose : set the media type we are using
	Parameters : media type
	Returns :
	Info : PS2 implementation only, stubbed out on all other platforms
*/

#define bkSetMediaType(media)
#endif // BPLATFORM != PS2


#endif // __BKERNEL_KERNEL_H__