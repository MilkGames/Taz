// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : viewport.cpp
//   Purpose : Declares functions for pushing and popping projection modes onto a stack
// Component : Babel Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Locals

static int      bProjectionSP    = 0;
static uint32   bProjectionStack[16];

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bdPushProjectionMode
	Purpose : push projection mode
	Parameters : mode
	Returns : 
	Info : 
*/
void bdPushProjectionMode(const uint32 mode)
{
    // Save current projection only when 0 <= sp < 15, to avoid writing index 15 (matches JGE 0x0F)
    if (bProjectionSP >= 0 && bProjectionSP < 15) {
        // bViewInfo.projectionMode is the current mode to be restored later
        bProjectionStack[bProjectionSP] = bViewInfo.projectionMode;
    }
    // Increment stack pointer unconditionally (INC EAX)
    bProjectionSP++;

    // Call bdSetProjectionMode(mode, 0)
    bdSetProjectionMode(mode, 0);
}

/*	--------------------------------------------------------------------------------
	Function : bdPopProjectionMode
	Purpose : pop projection mode
	Parameters : 
	Returns : 
	Info : 
*/
void bdPopProjectionMode()
{
    // Decrement first (DEC EAX), store back
    bProjectionSP--;

    // If sp is negative or sp >= 15, do nothing (matches JS / JGE 0x0F)
    if ((-1 < bProjectionSP) && (bProjectionSP < 15)) {
        bdSetProjectionMode(bProjectionStack[bProjectionSP], 0);
    }

	return;
}
