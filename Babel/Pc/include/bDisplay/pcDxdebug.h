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



// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bDXFormatToString
   Purpose : convert a DirectX format code to a string
   Parameters : format code
   Returns : string
   Info : 
*/

char *bDXFormatToString(D3DFORMAT format);

#endif		// __BDISPLAY_PCDXDEBUG_H__
