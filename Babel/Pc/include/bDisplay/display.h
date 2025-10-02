// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : display.h
//   Purpose : display functions
// Component : Generic Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BDISPLAY_DISPLAY_H__
#define __BDISPLAY_DISPLAY_H__


// ********************************************************************************
// Constants and Macros

// special character code indicating a non-escape-sequence '@' symbol
#define BNONESCAPE_ATSYMBOL		1

// special character code indicating a non-escape-sequence backslash
#define BNONESCAPE_BACKSLASH	2

// Flags for the bdFlip function
#define BFLIPFLAG_UPDATE		0x00000001		// Update all the babel modules


// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bdRGBToHSV
   Purpose : convert an RGB colour to HSV space
   Parameters : RGB inputs, HSV output ptrs
   Returns : 
   Info : 
*/

void bdRGBToHSV(int r, int g, int b, int *hp, int *sp, int *vp);


/* --------------------------------------------------------------------------------
   Function : bdHSVToRGB
   Purpose : convert an HSV colour to RGB space
   Parameters : HSV inputs, RGB output ptrs
   Returns : 
   Info : 
*/

void bdHSVToRGB(int h, int s, int v, int *rp, int *gp, int *bp);


/* --------------------------------------------------------------------------------
   Function : bdRGBToYCbCr
   Purpose : convert an RGB colour to YCbCr space
   Parameters : RGB inputs, YCbCr output ptrs
   Returns : 
   Info : 
*/

void bdRGBToYCbCr(int r, int g, int b, int *y, int *cb, int *cr);


/* --------------------------------------------------------------------------------
   Function : bdYCbCrToRGB
   Purpose : convert an YCbCr colour to RGB space
   Parameters : YCbCr inputs, RGB output ptrs
   Returns : 
   Info : 
*/

void bdYCbCrToRGB(int y, int cb, int cr, int *rp, int *gp, int *bp);

#endif		// __BDISPLAY_DISPLAY_H__
