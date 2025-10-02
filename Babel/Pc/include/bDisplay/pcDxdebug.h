// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcDxdebug.h
//   Purpose : DirectX debug helpers
// Component : Xbox Display
//   Comment : EXPECTED TO BE THE SAME AS xbDxdebug.h, DIRECT REWRITE!
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BDISPLAY_PCDXDEBUG_H__
#define __BDISPLAY_PCDXDEBUG_H__

// ********************************************************************************
// Macros and constants

#define DXGetErrorString8(ERR)		bDXErrorToString(ERR)


// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bDXErrorToString
   Purpose : convert a DirectX error code to a string
   Parameters : error code
   Returns : string
   Info : 
*/

char *bDXErrorToString(HRESULT err);

#endif		// __BDISPLAY_PCDXDEBUG_H__
