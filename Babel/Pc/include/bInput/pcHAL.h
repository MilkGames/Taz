// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcHAL.h
//   Purpose : Xbox specific hardware input code
// Component : Xbox Input
//   Comment : EXPECTED TO BE THE SAME AS xbHAL.h, DIRECT REWRITE!
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BINPUT_PCHAL_H__
#define __BINPUT_PCHAL_H__

// ********************************************************************************
// Constants and Macros

// bDirectInputKeyMap size
#define BDIRECTINPUTKEYMAPSIZE 107

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

// Device info used by input system
typedef struct _TBDeviceInfo
{
    uint32                flags;             // 0x00  device flags
    GUID                  guid;              // 0x04  device instance GUID
    IDirectInputDevice8A *device;            // 0x14  DirectInput device pointer

    uint32                numButtons;        // 0x18  number of buttons
    uint32                numAxes;           // 0x1C  number of axes
    uint32                numPOVs;           // 0x20  number of POV hats

    uint32                axisPresentMask;   // 0x24  bitmask of present axes

    LONG                  axisMin[6];        // 0x28  per-axis min
    LONG                  axisMax[6];        // 0x40  per-axis max
    LONG                  axisCenter[6];     // 0x58  per-axis center
    LONG                  axisHalfRange[6];  // 0x70  per-axis half-range
    LONG                  axisScaled[6];     // 0x88  per-axis scaled half-range

    uint32                capsSize;          // 0xA0  sizeof caps struct
} TBDeviceInfo; // sizeof(TBDeviceInfo) == 0xA4


// our DirectInput information container
typedef struct _TBInputInfo {
    IDirectInput8A* pDI;                 // 0x000: DirectInput8 main interface
    TBDeviceInfo    devices[6];           // 0x004: 6 devices (0–3 pads, 4 keyboard, 5 mouse)
    uint            joystickCount;       // 0x3DC: number of active joysticks/gamepads
    uchar           keyboardState[256];  // 0x3E0: raw keyboard state (GetDeviceState + c_dfDIKeyboard)
    DIMOUSESTATE    mouseFiltered;       // 0x4E0: filtered mouse state (smoothed / throttled)
    DIMOUSESTATE    mouseRaw;            // 0x4F0: raw mouse state (GetDeviceState + c_dfDIMouse)
    uint            UNKNOWN1;            // 0x500: unknown
    DIJOYSTATE      joyState[BMAXPADS];  // 0x504: raw DIJOYSTATE for up to BMAXPADS pads
    int32           joyAxis[BMAXPADS][6];// 0x644: calibrated axes [pad][0..5] = X,Y,Z,Rx,Ry,Rz
    uint32          UNKNOWN2[228];       // 0x6A4: unknown
    uint            initialized;         // 0xA34: non-zero when input system is initialized
} TBInputInfo;


// ********************************************************************************
// Globals

extern TBInputInfo	bInputInfo;						// our input device information container

extern GUID *bJoystickInstanceGuid[BMAXPADS];
extern int bDirectInputKeyMap[BDIRECTINPUTKEYMAPSIZE];

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

#endif		// __BINPUT_PCHAL_H__
