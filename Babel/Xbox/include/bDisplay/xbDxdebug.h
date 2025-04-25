// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : xbDxdebug.h
//   Purpose : DirectX debug helpers
// Component : Xbox Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BDISPLAY_XBDXDEBUG_H__
#define __BDISPLAY_XBDXDEBUG_H__

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

#endif		// __BDISPLAY_XBDXDEBUG_H__
