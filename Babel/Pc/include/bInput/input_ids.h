// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : input_ids.h
//   Purpose : list of input device constants
// Component : Generic Input
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BINPUT_INPUT_IDS_H__
#define __BINPUT_INPUT_IDS_H__

// ********************************************************************************
// Constants and Macros

// physical input ID device modifiers + masks
#define BIINPUT_DEV1		0x00000000					// use physical input port #1
#define BIINPUT_DEV2		0x00010000					// use physical input port #2
#define BIINPUT_DEV3		0x00020000					// use physical input port #3
#define BIINPUT_DEV4		0x00030000					// use physical input port #4
#define BIINPUT_DEV5		0x00040000					// use physical input port #1
#define BIINPUT_DEV6		0x00050000					// use physical input port #2
#define BIINPUT_DEV7		0x00060000					// use physical input port #3
#define BIINPUT_DEV8		0x00070000					// use physical input port #4
#define BIINPUT_ALLDEVS		0x000e0000					// symbolic constant indicating all ports
#define BIINPUT_DEVMASK		0x000f0000					// flag to mask device ID bits
#define BIINPUT_NODEV		BIINPUT_DEVMASK				// symbolic constant identifying no port
#define BIINPUT_DEV(ID)     ((ID)<<16)					// macro to shift device ID to correct bit position
#define BIINPUT_IDMASK		0x000000ff					// mask to clear the ID number bits
#define BIINPUT_GAINSHIFT	24							// number of bits to shift the gain
#define BIINPUT_GAIN(X)		((X)<<BIINPUT_GAINSHIFT)	// macro to shift gain value correct number of bits
#define BIINPUT_GAINMASK	0x7f000000					// value to mask gain bits
#define BIINPUT_INVERT		0x00000100					// flag indicating this channel should be inverted


// physical input IDs
typedef enum EBInputID {
	// invalid value
	BIINPUT_NULL,

	// PC Mouse
	BIINPUT_MOUSE_FIRSTID,
	BIINPUT_MOUSE_B1,				// button #1 (left)
	BIINPUT_MOUSE_B2,				// button #2 (right)
	BIINPUT_MOUSE_B3,				// button #3 (middle)
	BIINPUT_MOUSE_X,				// X axis
	BIINPUT_MOUSE_Y,				// Y axis
	BIINPUT_MOUSE_Z,				// Z axis (wheel)
	BIINPUT_MOUSE_LASTID,

	// PC Joysticks
	BIINPUT_PCJOY_FIRSTID,
	BIINPUT_PCJOY_B1,				// fire button #1
	BIINPUT_PCJOY_B2,				// fire button #2
	BIINPUT_PCJOY_B3,				// fire button #3
	BIINPUT_PCJOY_B4,				// fire button #4
	BIINPUT_PCJOY_B5,				// fire button #5
	BIINPUT_PCJOY_B6,				// fire button #6
	BIINPUT_PCJOY_B7,				// fire button #7
	BIINPUT_PCJOY_B8,				// fire button #8
	BIINPUT_PCJOY_B9,				// fire button #9
	BIINPUT_PCJOY_B10,				// fire button #10
	BIINPUT_PCJOY_B11,				// fire button #11
	BIINPUT_PCJOY_B12,				// fire button #12
	BIINPUT_PCJOY_B13,				// fire button #13
	BIINPUT_PCJOY_B14,				// fire button #14
	BIINPUT_PCJOY_B15,				// fire button #15
	BIINPUT_PCJOY_B16,				// fire button #16
	BIINPUT_PCJOY_B17,				// fire button #17
	BIINPUT_PCJOY_B18,				// fire button #18
	BIINPUT_PCJOY_B19,				// fire button #19
	BIINPUT_PCJOY_B20,				// fire button #20
	BIINPUT_PCJOY_B21,				// fire button #21
	BIINPUT_PCJOY_B22,				// fire button #22
	BIINPUT_PCJOY_B23,				// fire button #23
	BIINPUT_PCJOY_B24,				// fire button #24
	BIINPUT_PCJOY_B25,				// fire button #25
	BIINPUT_PCJOY_B26,				// fire button #26
	BIINPUT_PCJOY_B27,				// fire button #27
	BIINPUT_PCJOY_B28,				// fire button #28
	BIINPUT_PCJOY_B29,				// fire button #29
	BIINPUT_PCJOY_B30,				// fire button #30
	BIINPUT_PCJOY_B31,				// fire button #31
	BIINPUT_PCJOY_B32,				// fire button #32
	BIINPUT_PCJOY_X,				// X axis
	BIINPUT_PCJOY_Y,				// Y axis
	BIINPUT_PCJOY_Z,				// Z axis
	BIINPUT_PCJOY_XR,				// X rotation axis
	BIINPUT_PCJOY_YR,				// Y rotation axis
	BIINPUT_PCJOY_ZR,				// Z rotation axis (rudder)
	BIINPUT_PCJOY_POV1,				// angle of POV hat #1
	BIINPUT_PCJOY_POV2,				// angle of POV hat #2
	BIINPUT_PCJOY_POV3,				// angle of POV hat #3
	BIINPUT_PCJOY_POV4,				// angle of POV hat #4
	BIINPUT_PCJOY_LASTID,

	// PC keyboard
	BIINPUT_KEY_FIRSTID,
	BIINPUT_KEY_ESCAPE,
	BIINPUT_KEY_1,					// On main keyboard 
	BIINPUT_KEY_2,					// On main keyboard 
	BIINPUT_KEY_3,					// On main keyboard 
	BIINPUT_KEY_4,					// On main keyboard 
	BIINPUT_KEY_5,					// On main keyboard 
	BIINPUT_KEY_6,					// On main keyboard 
	BIINPUT_KEY_7,					// On main keyboard 
	BIINPUT_KEY_8,					// On main keyboard 
	BIINPUT_KEY_9,					// On main keyboard 
	BIINPUT_KEY_0,					// On main keyboard 
	BIINPUT_KEY_MINUS,				// On main keyboard 
	BIINPUT_KEY_EQUALS,				// On main keyboard 
	BIINPUT_KEY_BACK,				// The BACKSPACE key  
	BIINPUT_KEY_TAB,
	BIINPUT_KEY_A,
	BIINPUT_KEY_B,
	BIINPUT_KEY_C,
	BIINPUT_KEY_D,
	BIINPUT_KEY_E,
	BIINPUT_KEY_F,
	BIINPUT_KEY_G,
	BIINPUT_KEY_H,
	BIINPUT_KEY_I,
	BIINPUT_KEY_J,
	BIINPUT_KEY_K,
	BIINPUT_KEY_L,
	BIINPUT_KEY_M,
	BIINPUT_KEY_N,
	BIINPUT_KEY_O,
	BIINPUT_KEY_P,
	BIINPUT_KEY_Q,
	BIINPUT_KEY_R,
	BIINPUT_KEY_S,
	BIINPUT_KEY_T,
	BIINPUT_KEY_U,
	BIINPUT_KEY_V,
	BIINPUT_KEY_W,
	BIINPUT_KEY_X,
	BIINPUT_KEY_Y,
	BIINPUT_KEY_Z,
	BIINPUT_KEY_LBRACKET,			// The [ key  
	BIINPUT_KEY_RBRACKET,			// The ] key  
	BIINPUT_KEY_RETURN,				// ENTER key on main keyboard  
	BIINPUT_KEY_SEMICOLON,
	BIINPUT_KEY_APOSTROPHE,
	BIINPUT_KEY_BACKSLASH,
	BIINPUT_KEY_GRAVE,				// Grave accent (`) key  
	BIINPUT_KEY_LCONTROL,			// Left CTRL key  
	BIINPUT_KEY_RCONTROL,			// Right CTRL key  
	BIINPUT_KEY_LSHIFT,				// Left SHIFT key  
	BIINPUT_KEY_RSHIFT,				// Right SHIFT key  
	BIINPUT_KEY_LALT,				// Left ALT key  
	BIINPUT_KEY_RALT,				// Right ALT key  
	BIINPUT_KEY_COMMA,
	BIINPUT_KEY_PERIOD,				// On main keyboard  
	BIINPUT_KEY_SLASH,				// Forward slash on main keyboard 
	BIINPUT_KEY_SPACE,				// SPACEBAR  
	BIINPUT_KEY_F1,
	BIINPUT_KEY_F2,
	BIINPUT_KEY_F3,
	BIINPUT_KEY_F4,
	BIINPUT_KEY_F5,
	BIINPUT_KEY_F6,
	BIINPUT_KEY_F7,
	BIINPUT_KEY_F8,
	BIINPUT_KEY_F9,
	BIINPUT_KEY_F10,
	BIINPUT_KEY_F11,
	BIINPUT_KEY_F12,
	BIINPUT_KEY_F13,
	BIINPUT_KEY_F14,
	BIINPUT_KEY_F15,
	BIINPUT_KEY_NUMLOCK,
	BIINPUT_KEY_SCROLLLOCK,			// SCROLL LOCK 
	BIINPUT_KEY_CAPSLOCK,			// CAPS LOCK key  
	BIINPUT_KEY_NUMPAD0,
	BIINPUT_KEY_NUMPAD1,
	BIINPUT_KEY_NUMPAD2,
	BIINPUT_KEY_NUMPAD3,
	BIINPUT_KEY_NUMPAD4,
	BIINPUT_KEY_NUMPAD5,
	BIINPUT_KEY_NUMPAD6,
	BIINPUT_KEY_NUMPAD7,
	BIINPUT_KEY_NUMPAD8,
	BIINPUT_KEY_NUMPAD9,
	BIINPUT_KEY_DECIMAL,			// PERIOD (decimal point) on numeric keypad  
	BIINPUT_KEY_MULTIPLY,			// The * key on numeric keypad  
	BIINPUT_KEY_SUBTRACT,			// MINUS SIGN on numeric keypad  
	BIINPUT_KEY_ADD,				// PLUS SIGN on numeric keypad  
	BIINPUT_KEY_DIVIDE,				// Forward slash on numeric keypad  
	BIINPUT_KEY_NUMPADENTER,
	BIINPUT_KEY_SYSRQ,				// system rq
	BIINPUT_KEY_PAUSE,
	BIINPUT_KEY_HOME,
	BIINPUT_KEY_END,
	BIINPUT_KEY_PRIOR,				// PAGE UP  
	BIINPUT_KEY_NEXT,				// PAGE DOWN  
	BIINPUT_KEY_INSERT,
	BIINPUT_KEY_DELETE,
	BIINPUT_KEY_UP,					// UP ARROW  
	BIINPUT_KEY_DOWN,				// DOWN ARROW  
	BIINPUT_KEY_LEFT,				// LEFT ARROW  
	BIINPUT_KEY_RIGHT,				// RIGHT ARROW  
	BIINPUT_KEY_LWIN,				// Left Windows key  
	BIINPUT_KEY_RWIN,				// Right Windows key  
	BIINPUT_KEY_APPS,				// Application key
	BIINPUT_KEY_LASTID,

	// Playstation pad
	BIINPUT_PS_FIRSTID,
	BIINPUT_PS_LEFT,				// D-Pad left
	BIINPUT_PS_RIGHT,				// D-Pad right
	BIINPUT_PS_UP,					// D-Pad up
	BIINPUT_PS_DOWN,				// D-Pad down
	BIINPUT_PS_X1,					// left stick X axis
	BIINPUT_PS_Y1,					// left stick Y axis
	BIINPUT_PS_X2,					// right stick X axis
	BIINPUT_PS_Y2,					// right stick Y axis
	BIINPUT_PS_CROSS,
	BIINPUT_PS_TRIANGLE,
	BIINPUT_PS_CIRCLE,
	BIINPUT_PS_SQUARE,
	BIINPUT_PS_L1,					// left shoulder #1
	BIINPUT_PS_L2,					// left shoulder #2
	BIINPUT_PS_L3,					// left stick button
	BIINPUT_PS_R1,					// right shoulder #1
	BIINPUT_PS_R2,					// right shoulder #2
	BIINPUT_PS_R3,					// right stick button
	BIINPUT_PS_SELECT,
	BIINPUT_PS_START,
	BIINPUT_PS_LASTID,


	// Xbox pad
	BIINPUT_XBOX_FIRSTID,

	// joysticks
	BIINPUT_XBOX_X1,								// left stick X axis (ANALOGUE)
	BIINPUT_XBOX_Y1,								// left stick Y axis (ANALOGUE)
	BIINPUT_XBOX_X2,								// right stick X axis (ANALOGUE)
	BIINPUT_XBOX_Y2,								// right stick Y axis (ANALOGUE)

	// digital buttons
	BIINPUT_XBOX_UP,								// D-Pad up
	BIINPUT_XBOX_DOWN,								// D-Pad down
	BIINPUT_XBOX_LEFT,								// D-Pad left
	BIINPUT_XBOX_RIGHT,								// D-Pad right
	BIINPUT_XBOX_START,								// start
	BIINPUT_XBOX_BACK,								// back
	BIINPUT_XBOX_STICK1,							// left thumb stick button
	BIINPUT_XBOX_STICK2,							// right thumb stick button

	// analogue buttons
	BIINPUT_XBOX_A,									// 'A' button
	BIINPUT_XBOX_B,									// 'B' button
	BIINPUT_XBOX_X,									// 'X' button
	BIINPUT_XBOX_Y,									// 'Y' button
	BIINPUT_XBOX_BLACK,								// black secondary button
	BIINPUT_XBOX_WHITE,								// white secondary button
	BIINPUT_XBOX_LTRIGGER,							// left trigger button
	BIINPUT_XBOX_RTRIGGER,							// right trigger button
	BIINPUT_XBOX_LASTID,


	// GameCube pad
	BIINPUT_GAMECUBE_FIRSTID,
	BIINPUT_GAMECUBE_LEFT,							// D-Pad left
	BIINPUT_GAMECUBE_RIGHT,							// D-Pad right
	BIINPUT_GAMECUBE_UP,							// D-Pad up
	BIINPUT_GAMECUBE_DOWN,							// D-Pad down
	BIINPUT_GAMECUBE_X1,							// left stick X axis
	BIINPUT_GAMECUBE_Y1,							// left stick Y axis
	BIINPUT_GAMECUBE_X2,							// right stick X axis
	BIINPUT_GAMECUBE_Y2,							// right stick Y axis
	BIINPUT_GAMECUBE_A,								// 'A' button (analog: but digital on standard pad !)
	BIINPUT_GAMECUBE_B,								// 'B' button (analog: but digital on standard pad !)
	BIINPUT_GAMECUBE_APRESS,						// 'A' button (digital buton)
	BIINPUT_GAMECUBE_BPRESS,						// 'B' button (digital buton)
	BIINPUT_GAMECUBE_L,								// 'L' button (analog)
	BIINPUT_GAMECUBE_R,								// 'R' button (analog)
	BIINPUT_GAMECUBE_LTRIGGER,						// 'L' button (digital buton)
	BIINPUT_GAMECUBE_RTRIGGER,						// 'R' button (digital buton)
	BIINPUT_GAMECUBE_X,								// 'X' button
	BIINPUT_GAMECUBE_Y,								// 'Y' button
	BIINPUT_GAMECUBE_Z,								// 'Z' button
	BIINPUT_GAMECUBE_START,							// 'Start/Pause' button
	BIINPUT_GAMECUBE_LASTID,

	
	NOOFBIINPUTS
};

#endif		// __BINPUT_INPUT_IDS_H__
