// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : input.h
//   Purpose : primary input functions
// Component : Generic Input
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BINPUT_INPUT_H__
#define __BINPUT_INPUT_H__

// ********************************************************************************
// Constants and Macros

// maximum possible +ve or -ve channel value
#define BIMAX			32760

// special value indicating centred POV hat
#define BIPOV_CENTRE	32761

// channel types
typedef enum {
	BICHANNELTYPE_UNUSED = 0,		// channel is unused (i.e. is free to be reallocated)
	BICHANNELTYPE_DIGITAL,			// can return only 0 or BIMAX
	BICHANNELTYPE_ANALOGUE,			// returns any value between 0 and BIMAX
	BICHANNELTYPE_BIANALOGUE		// returns any value between -BIMAX and +BIMAX
} EBIChannelType;


// channel flags
#define BICHANNELFLAG_DEBOUNCE		0x00000001				// indicates the channel is set only once for each button press
#define BICHANNELFLAG_RECORD		0x00000002				// indicates the channel should be included in input recordings
#define BICHANNELFLAG_AUTOREPEAT	0x00000004				// indicates the channel will debounce but then auto-repeat after a short delay
#define BICHANNELFLAG_CANCELACC		0x00000008				// indicates opposite input will zero accumulation


// maximum number of bindings for a single channel
#define BIMAX_CHANNEL_BINDINGS		6


// maximum number of inputs for a single binding
#define BIMAX_BINDING_INPUTS		6


// maximum length of string for the channel name
#define BIMAX_NAME_LENGTH			16


// channel binding signs
enum {
	BICHANNELSIGN_POS,				// this input results in a +ve value
	BICHANNELSIGN_NEG,				// this input results in a -ve value
};


// input device port IDs
enum {
	BIPORT_GAME1=0,					// joystick/pad #1
	BIPORT_GAME2=1,					// joystick/pad #2
	BIPORT_GAME3=2,					// joystick/pad #3
	BIPORT_GAME4=3,					// joystick/pad #4
	BIPORT_GAME5=4,					// joystick/pad #5
	BIPORT_GAME6=5,					// joystick/pad #6
	BIPORT_GAME7=6,					// joystick/pad #7
	BIPORT_GAME8=7,					// joystick/pad #8

	BIPORT_KEYBOARD=256,			// PC keyboard
	BIPORT_MOUSE,					// PC mouse
};


// channel map flags
#define BICHANMAPFLAG_DISABLED		0x00000001			// map is currently disabled


// force feedback parameters
enum {
	BIFEEDBACK_PS2BUZZER,
	BIFEEDBACK_PS2RUMBLE,

	BIFEEDBACK_XBOXLEFTMOTOR,
	BIFEEDBACK_XBOXRIGHTMOTOR,
	BIFEEDBACK_XBOXBOTHMOTORS,

	BIFEEDBACK_GAMECUBERUMBLE,
};


// maximum Xbox motor speed
#define BXBOX_MOTORSPEED	65535


// types of input devices we understand
typedef enum EBIDeviceType {
	EBIDEVICETYPE_UNCONNECTED,
	EBIDEVICETYPE_UNKNOWN,
	EBIDEVICETYPE_PSDIGITAL,
	EBIDEVICETYPE_PSANALOGUE,
	EBIDEVICETYPE_PSDUALSHOCK,
	EBIDEVICETYPE_PS2DUALSHOCK2,
	EBIDEVICETYPE_XBOXSTANDARD,
	EBIDEVICETYPE_PCKEYBOARD,
	EBIDEVICETYPE_PCMOUSE,
	EBIDEVICETYPE_GCSTANDARD,
	EBIDEVICETYPE_GCWAVEBIRD,
	EBIDEVICETYPE_GCRECEIVER,
	EBIDEVICETYPE_GBA,
	EBIDEVICETYPE_N64CONTROLLER,
	EBIDEVICETYPE_N64MIC,
	EBIDEVICETYPE_N64KEYBOARD,
	EBIDEVICETYPE_N64MOUSE,
};


// rumble styles
typedef enum EBRumbleStyle {
	EBRUMBLESTYLE_SMOOTH,
	EBRUMBLESTYLE_ABSSIN,
	EBRUMBLESTYLE_SQUARE,
	EBRUMBLESTYLE_SAWTOOTH,
	EBRUMBLESTYLE_RANDOM,
};

// maximum number of simultaneous effects on a rumble controller
#define BMAXRUMBLEEFFECTS		8

// maximum rumble amplitude
#define BMAXRUMBLEAMP			0x7fff

// define a valid array size for the rumble array
#if BNOOFRUMBLEMOTORS == 0
	#define BRUMBLEARRAYSIZE	1
#else
	#define BRUMBLEARRAYSIZE	BNOOFRUMBLEMOTORS
#endif

// rumble motor ID's
#define BXBMOTOR_LEFT			0
#define BXBMOTOR_RIGHT			1
#define BPS2MOTOR				0
#define BGCMOTOR				0

// port status values (used with biIsDeviceConnected)
typedef enum {
	EBPORTSTATUS_INVALIDPORT,
	EBPORTSTATUS_CONNECTED,
	EBPORTSTATUS_DISCONNECTED,
} EBPortStatus;


// ********************************************************************************
// Types, Structures and Classes

// structure representing a single channel binding
typedef struct _TBIBinding {
	int32					inputID[BIMAX_BINDING_INPUTS];		// IDs of the physical inputs (see BIINPUT_ constants)
	int						noofInputs;							// number of active IDs
} TBIBinding;


// structure representing a single input channel
typedef struct _TBIInputChannel {
	char					name[BIMAX_NAME_LENGTH];			// human readable channel name for debugging purposes
	EBIChannelType			type;								// type of channel (see BICHANNELTYPE_ above)
	int32					value;								// current value of the channel (0+-BIMAX)
	uint32					flags;								// channel flags (see BICHANNELFLAG_ above)
	int32					maxValue;							// maximum channel value
	int32					duration;							// how many cycles has this channel been non-zero for?
	TBIBinding				bindings[BIMAX_CHANNEL_BINDINGS];	// bindings on this channel
	int						noofBindings;						// number of active bindings on this channel
	struct _TBIChannelMap	*map;								// map this channel belongs to
	TBTimerValue			startTime;							// time auto-repeat channel was first pressed
	uint32					autoRepeatStartDelay;				// delay before auto-repeat channels start repeating
	uint32					autoRepeatRepeatDelay;				// delay between each auto-repeat
	int32					accIncSpeed;						// accumulator increment speed (or zero if not an accumulator channel)
	int32					accDecSpeed;						// accumulator decrement speed (or zero if not an accumulator channel)
} TBIInputChannel;


// structure representing a single channel map
typedef struct _TBIChannelMap {
	TBIInputChannel			*channels;							// array of [maxChannels] channel structures
	int32					maxChannels;						// maximum number of channels this map can contain
	int32					noofChannels;						// current number of active channels in this map
	uint32					flags;								// map flags (see BICHANMAPFLAG_)

	struct _TBIChannelMap	*next, *prev;						// map list links
} TBIChannelMap;


// header for input recording buffers
typedef struct _TBInputRecording {
	int32				noofChannels;			// #channels that have been recorded
	int32				noofSamples;			// #samples that have been taken
	int32				maxSamples;				// max #samples that can be taken before buffer needs expanding
	int32				curSample;				// index of current sample
	int32				clientContextSize;		// size of the client context block in bytes
	int32				sampleSize;				// size of a sample in bytes
	int32				pad1, pad2;
} TBInputRecording;


// channel info index entry for input recording buffers
typedef struct _TBInputRecordingChannel {
	uint32				crc;					// CRC of channel name
	TBIInputChannel		*channel;				// ptr to the channel this index entry refers to
} TBInputRecordingChannel;


// global container for input state information
typedef struct _TBInputStateInfo {
	int					padsHaveChanged;		// TRUE if the connection states of one or more pads changed
} TBInputStateInfo;


// a rumble effect for batch setting (see biAddRumbleBatchEffect)
typedef struct _TBRumbleBatchEffect {
	int				motorId;					// motor ID
	int				amplitude;					// amplitude (0..BMAXRUMBLEAMP)
	int				duration;					// duration (ms)
	int				attack;						// percentage of time during rise to max amplitude
	int				sustain;					// percentage of time at max amplitude
	int				delay;						// delay before starting (ms)
	EBRumbleStyle	style;						// style (EBRUMBLESTYLE_)
} TBRumbleBatchEffect;


// a rumble controller effect
typedef struct _TBRumbleControllerEffect {
	int					amplitude;				// amplitude of effect (1..BMAXRUMBLEAMP), 0 if effect not used
	TBTimerValue		duration;				// duration of effect
	TBTimerValue		delay;					// delay before this effect starts (0 if started)
	int					attack;					// attack period
	int					decay;					// decay period
	EBRumbleStyle		style;					// effect style
	union {
		TBTimerValue	queuedTime;				// time this effect was queued
		TBTimerValue	startTime;				// time this effect was started
	};
} TBRumbleControllerEffect;


// a rumble controller
typedef struct _TBRumbleController {
	TBRumbleControllerEffect	effects[BMAXRUMBLEEFFECTS];	// list of effects
	int							noofEffects;				// number of effects active in list
} TBRumbleController;


// ********************************************************************************
// Globals

// global container for input state information
extern TBInputStateInfo		bInputState;									// general input state info
extern TBRumbleController	bRumbleControllers[BMAXPADS][BRUMBLEARRAYSIZE];	// rumble controller array
extern TBIChannelMap		*bInputMapList;									// root of the channel map list


// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : biCreateMap
	Purpose : Create a new channel map
	Parameters : maximum number of channels this map can contain
	Returns : ptr to new map or NULL for failure
	Info : 
*/

TBIChannelMap *biCreateMap(int maxChannels);


/*	--------------------------------------------------------------------------------
	Function : biDeleteMap
	Purpose : Delete a channel map
	Parameters : ptr to channel map to delete
	Returns : 
	Info : 
*/

void biDeleteMap(TBIChannelMap *map);


/*	--------------------------------------------------------------------------------
	Function : biDeleteAllMaps
	Purpose : Delete all channel maps
	Parameters : 
	Returns : 
	Info : 
*/

void biDeleteAllMaps();


/*	--------------------------------------------------------------------------------
	Function : biCreateChannel
	Purpose : Create a new input channel
	Parameters : ptr to map to add channel to, channel name, channel type, flags, max value (0=default),
																accumulator increment speed, accumulator decrement speed
	Returns : ptr to new channel or NULL for failure
	Info : 
*/

TBIInputChannel *biCreateChannel(TBIChannelMap *map, char *name, int32 type, uint32 flags, int32 maxValue,
																					int accIncSpeed=0, int accDecSpeed=0);


/*	--------------------------------------------------------------------------------
	Function : biGetChannelName
	Purpose : return the textual name given to the channel
	Parameters : ptr to map, channel number, pointer to name to fill in, positive binding, negative binding
	Returns : OK/FAIL
	Info : name should be at least BIMAX_NAME_LENGTH long
*/

int biGetChannelName(TBIChannelMap *map, int channelNumber, ushort *name, int *posInputID=0, int *negInputID=0);


/*	--------------------------------------------------------------------------------
	Function : biDeleteChannel
	Purpose : Delete an input channel
	Parameters : ptr to channel to delete
	Returns : 
	Info : 
*/

void biDeleteChannel(TBIInputChannel *channel);


/*	--------------------------------------------------------------------------------
	Function : biBindInput
	Purpose : Bind an input to a channel
	Parameters : ptr to channel to bind to, +ve input ID, -ve input ID
	Returns : OK/FAIL
	Info : 
*/

int biBindInput(TBIInputChannel *channel, int32 posInputID, int32 negInputID);


/*	--------------------------------------------------------------------------------
	Function : biBindMulti
	Purpose : Bind multiple inputs to a digital channel
	Parameters : ptr to channel to bind to, number of inputs to bind, [input IDs]
	Returns : OK/FAIL
	Info : 
*/

int biBindMulti(TBIInputChannel *channel, int32 noofInputs, ...);


/*	--------------------------------------------------------------------------------
	Function : biResetChannel
	Purpose : Remove all bindings from a channel
	Parameters : ptr to channel to reset
	Returns : 
	Info : 
*/

void biResetChannel(TBIInputChannel *channel);


/*	--------------------------------------------------------------------------------
	Function : biResetMap
	Purpose : Remove all channels from a map
	Parameters : ptr to map to reset
	Returns : 
	Info : 
*/

void biResetMap(TBIChannelMap *map);


/*	--------------------------------------------------------------------------------
	Function : biReadDevices
	Purpose : Read the input devices and update channel values
	Parameters : 
	Returns : TRUE if devices have been added/removed since last call, FALSE otherwise
	Info : 
*/

int biReadDevices();


/*	--------------------------------------------------------------------------------
	Function : biGetChar
	Purpose : Read the next character from the keyboard buffer
	Parameters : 
	Returns : ASCII code read or NULL for none
	Info : 
*/

int biGetChar();


/*	--------------------------------------------------------------------------------
	Function : biFlushKeyBuffer
	Purpose : Flush the keyboard buffer
	Parameters : 
	Returns : 
	Info : 
*/

void biFlushKeyBuffer();


/*	--------------------------------------------------------------------------------
	Function : biQueryDevice
	Purpose : Query for the presence/capabilities of an input device
	Parameters : physical port number or BIPORT_ constant, buffer for info
	Returns : device type connected
	Info : 
*/

EBIDeviceType biQueryDevice(int portID, void *infoBuffer);


/*	--------------------------------------------------------------------------------
	Function : biZeroMap
	Purpose : Zero all channels in a map
	Parameters : ptr to map
	Returns : 
	Info : 
*/

void biZeroMap(TBIChannelMap *map);


/*	--------------------------------------------------------------------------------
	Function : biForceDebounce
	Purpose : Force a channel to require key-up before returning a set state
	Parameters : ptr to channel
	Returns : 
	Info : 
*/

void biForceDebounce(TBIInputChannel *channelPtr);


/*	--------------------------------------------------------------------------------
	Function : biEnableMap
	Purpose : Enable/Disable a channel map
	Parameters : ptr to map, new state (TRUE=enable, FALSE=disable)
	Returns : 
	Info : 
*/

void biEnableMap(TBIChannelMap *map, int newState);


/*	--------------------------------------------------------------------------------
	Function : biStartRecording
	Purpose : start recording inputs
	Parameters : size of the client context record in bytes
	Returns : ptr to rec buffer or NULL for failure
	Info : 
*/

TBInputRecording *biStartRecording(int clientContextSize);


/*	--------------------------------------------------------------------------------
	Function : biRecordInputs
	Purpose : record the current input states
	Parameters : ptr to recording info, ptr to client context data
	Returns : ptr to recording info
	Info : 
*/

TBInputRecording *biRecordInputs(TBInputRecording *recInfo, void *clientContext);


/*	--------------------------------------------------------------------------------
	Function : biStopRecording
	Purpose : finish a recording session
	Parameters : ptr to recording info
	Returns : #bytes in recording
	Info : 
*/

int biStopRecording(TBInputRecording *recInfo);


/*	--------------------------------------------------------------------------------
	Function : biStartPlayback
	Purpose : start playback of an recorded input session
	Parameters : ptr to recording info
	Returns : 
	Info : 
*/

void biStartPlayback(TBInputRecording *recInfo);


/*	--------------------------------------------------------------------------------
	Function : biPlaybackInputs
	Purpose : playback a frame of inputs
	Parameters : ptr to recording info, buffer for client context data
	Returns : finished?
	Info : 
*/

int biPlaybackInputs(TBInputRecording *recInfo, void *contextBuffer);


/*	--------------------------------------------------------------------------------
	Function : biFindPad
	Purpose : find a pad from it's index
	Parameters : index of pad to look for (0=first pad, 1=second pad ...)
	Returns : device slot (0..3) or -1 for not found
	Info : 
*/

int biFindPad(int index);


/*	--------------------------------------------------------------------------------
	Function : biFFSetParm
	Purpose : Set force feedback parameters for a device
	Parameters : device slot, parameter to set, parameter values
	Returns : OK for sucess, FAIL for failure
	Info : 
*/

int biFFSetParm(int devId, int parm, ...);


/*	--------------------------------------------------------------------------------
	Function : biSetMapPhysicalPort
	Purpose : Set all channels in a map to use a specific physical device port
	Parameters : ptr to map, physical port ID (0..)
	Returns : 
	Info : 
*/

void biSetMapPhysicalPort(TBIChannelMap *map, int physicalPortId);


/*	--------------------------------------------------------------------------------
	Function : biSetChannelPhysicalPort
	Purpose : Set all bindings for a channel to use a specific physical device port
	Parameters : ptr to channel, physical port ID (0..)
	Returns : 
	Info : 
*/

void biSetChannelPhysicalPort(TBIInputChannel *channel, int physicalPortId);


/*	--------------------------------------------------------------------------------
	Function : biSwapMapPhysicalPort
	Purpose : Swap all channels in a map from one physical port to another
	Parameters : ptr to map, old physical port ID, new physical port ID
	Returns : 
	Info : 
*/

void biSwapMapPhysicalPort(TBIChannelMap *map, int oldPhysicalPortId, int newPhysicalPortId);


/*	--------------------------------------------------------------------------------
	Function : biSwapChannelPhysicalPort
	Purpose : Swap all bindings for a channel from one physical port to another
	Parameters : ptr to channel, old physical port ID, new physical port ID
	Returns : 
	Info : 
*/

void biSwapChannelPhysicalPort(TBIInputChannel *channel, int oldPhysicalPortId, int newPhysicalPortId);


/*	--------------------------------------------------------------------------------
	Function : bInitInput
	Purpose : initialise the input module
	Parameters : bkInit flags
	Returns : OK/FAIL
	Info : 
*/

void bInitInput(uint32 flags);


/*	--------------------------------------------------------------------------------
	Function : bShutdownInput
	Purpose : shutdown the input module
	Parameters : 
	Returns : 
	Info : 
*/

void bShutdownInput();


/*	--------------------------------------------------------------------------------
	Function : biFFStopAll
	Purpose : stop all force-feedback
	Parameters : 
	Returns : 
	Info : 
*/

void biFFStopAll();


/*	--------------------------------------------------------------------------------
	Function : biFFStop
	Purpose : stop all force-feedback
	Parameters : input id (-1 for all)
	Returns : 
	Info : 
*/

void biFFStop(int padId=-1);


/*	--------------------------------------------------------------------------------
	Function : biAddRumbleEffect
	Purpose : post a new effect to the rumble controller
	Parameters : pad ID, motor ID, amplitude (0..BMAXRUMBLEAMP), duration (ms), 
			     percentage of time getting to max amplitude, (-1 = ignore)
			     percentage of time at max amplitude,
				 delay before starting (ms), style (EBRUMBLESTYLE_)
	Returns : OK/FAIL
	Info : 
*/

int biAddRumbleEffect(int padId, int motorId, int amplitude, int duration, 
					  int attack = -1, int sustain = 50, int delay = 0,
					  EBRumbleStyle style = EBRUMBLESTYLE_SMOOTH);


/*	--------------------------------------------------------------------------------
	Function : biAddRumbleBatchEffect
	Purpose : post a batch of effects to the rumble controller
	Parameters : pad ID, #effects in batch, batch array
	Returns : 
	Info : 
*/

void biAddRumbleBatchEffect(int padId, int noofEffects, TBRumbleBatchEffect *effects);


/*	--------------------------------------------------------------------------------
	Function : biPauseRumble
	Purpose : pause the rumble controller
	Parameters : pad to pause
	Returns : new pause count
	Info : 
*/

int biPauseRumble();


/*	--------------------------------------------------------------------------------
	Function : biResumeRumble
	Purpose : resume the rumble controller from being paused
	Parameters : pad to resume
	Returns : new pause count
	Info : 
*/

int biResumeRumble();


/*	--------------------------------------------------------------------------------
	Function : biStopRumble
	Purpose : stop all rumble effects, flushing the effect list
	Parameters : pad to stop
	Returns : 
	Info : 
*/

void biStopRumble(int padId=-1);


/*	--------------------------------------------------------------------------------
	Function : biSetAutoRepeat
	Purpose : set the auto-repeat parameters for an input channel
	Parameters : channel to set the parameters on, delay before repeat starts (ms), how many repeats per second
	Returns : 
	Info : 
*/

void biSetAutoRepeat(TBIInputChannel *channel, int startDelay, int repeatsPerSecond);


/*	--------------------------------------------------------------------------------
	Function : biSetAutoRepeatMS
	Purpose : set the auto-repeat parameters for an input channel
	Parameters : channel to set the parameters on, delay before repeat starts (ms), delay between repeats (ms)
	Returns : 
	Info : 
*/

void biSetAutoRepeatMS(TBIInputChannel *channel, int startDelay, int repeatDelay);


/*	--------------------------------------------------------------------------------
	Function : bProcessRumble
	Purpose : process the rumble controllers
	Parameters : 
	Returns : 
	Info : 
*/

void bProcessRumble();


/*	--------------------------------------------------------------------------------
	Function : biGetPortStatus
	Purpose : return the status of a port
	Parameters : port ID (zero based)
	Returns : port status
	Info : 
*/

EBPortStatus biGetPortStatus(int portId);


/*	--------------------------------------------------------------------------------
	Function : bFixedPlayBackData
	Purpose : Fixup data for the specific platform 
	Parameters : ptr to recording info
	Returns : 
	Info : This routine is provided to fix up data output on a PC/Xbox or PS2 to work on a GameCube
		   NOTE: It will NOT fix up client context information !
*/

void bFixupPlayBackData(TBInputRecording *recInfo);


#endif		// __BINPUT_INPUT_H__
