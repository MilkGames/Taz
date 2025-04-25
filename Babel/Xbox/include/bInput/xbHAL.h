// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : xbHAL.h
//   Purpose : Xbox specific hardware input code
// Component : Xbox Input
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BINPUT_XBHAL_H__
#define __BINPUT_XBHAL_H__

// ********************************************************************************
// Constants and Macros

// maximum number of device ports
#define BNOOFINPUTPORTS		4

// maximum number of memory cards (MMUs)
#define BNOOFMEMCARDS		8

// number of rumble motors this platform supports
#define BNOOFRUMBLEMOTORS	2

// maximum number of pads this platform supports
#define BMAXPADS			4


// ********************************************************************************
// Types, Structures and Classes

// state for a port
typedef struct _TBInputPortState {
	int					portId;								// physical port ID (0..BNOOFINPUTPORTS-1)
	int					connected;							// TRUE if currently connected, FALSE otherwise
	HANDLE				handle;								// handle representing this pad and it's resources
	XINPUT_STATE		state;								// device state
} TBInputPortState;


// our DirectInput information container
typedef struct _TBInputInfo {
	TBInputPortState	portState[BNOOFINPUTPORTS];			// port state info
	int					noofConnectedDevices;				// number of currently connected devices
	TBInputPortState	memCardState[BNOOFMEMCARDS];		// memory card state info
	int					noofConnectedMemCards;				// number of currently connected memory cards
	TBInputPortState	*devicePortMap[BNOOFINPUTPORTS];	// map of connected logical ports to physical ports
	DWORD				lastDynamicTick;					// time last dynamic connection check was made
} TBInputInfo;


// ********************************************************************************
// Globals

extern TBInputInfo	bInputInfo;						// our input device information container


// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : bInitInputHAL
	Purpose : initialise the input module
	Parameters : bkInit flags
	Returns : 
	Info : 
*/

void bInitInputHAL(uint32 flags);


/*	--------------------------------------------------------------------------------
	Function : bShutdownInputHAL
	Purpose : shutdown the input module
	Parameters : 
	Returns : 
	Info : 
*/

void bShutdownInputHAL();


/*	--------------------------------------------------------------------------------
	Function : bReadPhysicalInputDevices
	Purpose : read the physical input devices
	Parameters : 
	Returns : 
	Info : 
*/

void bReadPhysicalInputDevices();


/*	--------------------------------------------------------------------------------
	Function : bReadInput
	Purpose : return the current value of a specific input
	Parameters : input ID, value behaviour, ptr to receive value
	Returns : OK / FAIL
	Info : 
*/

int bReadInput(int inputID, int behaviour, int *valuePtr);


/*	--------------------------------------------------------------------------------
	Function : bUpdateRumble
	Purpose : Ensure the rumble values on the pads are set correctly
	Parameters : 
	Returns : 
	Info : called ten times a second
*/

void bUpdateRumble();

#endif		// __BINPUT_XBHAL_H__
