// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : viewport.h
//   Purpose : viewport management functions
// Component : Generic Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BDISPLAY_VIEWPORT_H__
#define __BDISPLAY_VIEWPORT_H__

// ********************************************************************************
// Constants and Macros


// ********************************************************************************
// Types, Structures and Classes


// ********************************************************************************
// Globals


/* --------------------------------------------------------------------------------
   Function : bdPushProjectionMode
   Purpose : push the current value of a projection mode onto the stack and set a new mode
   Parameters : mode value
   Returns : 
   Info : 
*/

void bdPushProjectionMode(const uint32 mode);


/* --------------------------------------------------------------------------------
   Function : bdPopProjectionMode
   Purpose : pop a projection mode off the stack
   Parameters : 
   Returns : 
   Info : 
*/

void bdPopProjectionMode();


#endif		// __BDISPLAY_VIEWPORT_H__
