// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : input.cpp
//   Purpose : Defines constants for max channel value, special POV hat value, channel types, channel flags, max bindings/inputs, name length, binding signs, port IDs, force feedback parameters, Xbox motor speed, device types, rumble styles, max rumble effects/amplitude, rumble array size, motor IDs, and port status
// Component : Babel Input
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : biCreateMap
	Purpose : create input map
	Parameters : maxChannels
	Returns : map
	Info : 
*/
TBIChannelMap *biCreateMap(int maxChannels)
{
    return NULL;
}

/*	--------------------------------------------------------------------------------
	Function : biDeleteMap
	Purpose : delete input map
	Parameters : map
	Returns : 
	Info : 
*/
void biDeleteMap(TBIChannelMap *map)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : biDeleteAllMaps
	Purpose : delete all input maps
	Parameters : 
	Returns : 
	Info : 
*/
void biDeleteAllMaps()
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : biCreateChannel
	Purpose : create input channel
	Parameters : map, name, type, flags, maxValue, accIncSpeed, accDecSpeed
	Returns : channel
	Info : 
*/
TBIInputChannel *biCreateChannel(TBIChannelMap *map, char *name, int32 type, uint32 flags, int32 maxValue, int accIncSpeed, int accDecSpeed)
{
    return NULL;
}

/*	--------------------------------------------------------------------------------
	Function : biGetChannelName
	Purpose : get channel name
	Parameters : map, channelNumber, name, posInputID, negInputID
	Returns : OK/FAIL
	Info : 
*/
int biGetChannelName(TBIChannelMap *map, int channelNumber, ushort *name, int *posInputID, int *negInputID)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : biDeleteChannel
	Purpose : delete input channel
	Parameters : channel
	Returns : 
	Info : 
*/
void biDeleteChannel(TBIInputChannel *channel)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : biBindInput
	Purpose : bind input
	Parameters : channel, posInputID, negInputID
	Returns : OK/FAIL
	Info : 
*/
int biBindInput(TBIInputChannel *channel, int32 posInputID, int32 negInputID)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : biBindMulti
	Purpose : bind multiple inputs
	Parameters : channel, noofInputs, ...
	Returns : OK/FAIL
	Info : 
*/
int biBindMulti(TBIInputChannel *channel, int32 noofInputs, ...)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : biResetChannel
	Purpose : reset input channel
	Parameters : channel
	Returns : 
	Info : 
*/
void biResetChannel(TBIInputChannel *channel)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : biResetMap
	Purpose : reset input map
	Parameters : map
	Returns : 
	Info : 
*/
void biResetMap(TBIChannelMap *map)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : biReadDevices
	Purpose : read input devices
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/
int biReadDevices()
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : biGetChar
	Purpose : get character
	Parameters : 
	Returns : character
	Info : 
*/
int biGetChar()
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : biFlushKeyBuffer
	Purpose : flush key buffer
	Parameters : 
	Returns : 
	Info : 
*/
void biFlushKeyBuffer()
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : biQueryDevice
	Purpose : query input device
	Parameters : portID, infoBuffer
	Returns : device type
	Info : 
*/
EBIDeviceType biQueryDevice(int portID, void *infoBuffer)
{
    return EBIDEVICETYPE_UNCONNECTED;
}

/*	--------------------------------------------------------------------------------
	Function : biZeroMap
	Purpose : zero input map
	Parameters : map
	Returns : 
	Info : 
*/
void biZeroMap(TBIChannelMap *map)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : biForceDebounce
	Purpose : force debounce
	Parameters : channelPtr
	Returns : 
	Info : 
*/
void biForceDebounce(TBIInputChannel *channelPtr)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : biEnableMap
	Purpose : enable input map
	Parameters : map, newState
	Returns : 
	Info : 
*/
void biEnableMap(TBIChannelMap *map, int newState)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : biStartRecording
	Purpose : start input recording
	Parameters : clientContextSize
	Returns : recording info
	Info : 
*/
TBInputRecording *biStartRecording(int clientContextSize)
{
    return NULL;
}

/*	--------------------------------------------------------------------------------
	Function : biRecordInputs
	Purpose : record inputs
	Parameters : recInfo, clientContext
	Returns : recording info
	Info : 
*/
TBInputRecording *biRecordInputs(TBInputRecording *recInfo, void *clientContext)
{
    return NULL;
}

/*	--------------------------------------------------------------------------------
	Function : biStopRecording
	Purpose : stop input recording
	Parameters : recInfo
	Returns : OK/FAIL
	Info : 
*/
int biStopRecording(TBInputRecording *recInfo)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : biStartPlayback
	Purpose : start input playback
	Parameters : recInfo
	Returns : 
	Info : 
*/
void biStartPlayback(TBInputRecording *recInfo)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : biPlaybackInputs
	Purpose : playback inputs
	Parameters : recInfo, contextBuffer
	Returns : OK/FAIL
	Info : 
*/
int biPlaybackInputs(TBInputRecording *recInfo, void *contextBuffer)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : biFindPad
	Purpose : find input pad
	Parameters : index
	Returns : pad ID
	Info : 
*/
int biFindPad(int index)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : biFFSetParm
	Purpose : set force feedback parameter
	Parameters : devId, parm, ...
	Returns : OK/FAIL
	Info : 
*/
int biFFSetParm(int devId, int parm, ...)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : biSetMapPhysicalPort
	Purpose : set map physical port
	Parameters : map, physicalPortId
	Returns : 
	Info : 
*/
void biSetMapPhysicalPort(TBIChannelMap *map, int physicalPortId)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : biSetChannelPhysicalPort
	Purpose : set channel physical port
	Parameters : channel, physicalPortId
	Returns : 
	Info : 
*/
void biSetChannelPhysicalPort(TBIInputChannel *channel, int physicalPortId)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : biSwapMapPhysicalPort
	Purpose : swap map physical port
	Parameters : map, oldPhysicalPortId, newPhysicalPortId
	Returns : 
	Info : 
*/
void biSwapMapPhysicalPort(TBIChannelMap *map, int oldPhysicalPortId, int newPhysicalPortId)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : biSwapChannelPhysicalPort
	Purpose : swap channel physical port
	Parameters : channel, oldPhysicalPortId, newPhysicalPortId
	Returns : 
	Info : 
*/
void biSwapChannelPhysicalPort(TBIInputChannel *channel, int oldPhysicalPortId, int newPhysicalPortId)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bInitInput
	Purpose : initialise input
	Parameters : flags
	Returns : 
	Info : 
*/
void bInitInput(uint32 flags)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bShutdownInput
	Purpose : shutdown input
	Parameters : 
	Returns : 
	Info : 
*/
void bShutdownInput()
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : biFFStopAll
	Purpose : stop all force feedback
	Parameters : 
	Returns : 
	Info : 
*/
void biFFStopAll()
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : biFFStop
	Purpose : stop force feedback
	Parameters : padId
	Returns : 
	Info : 
*/
void biFFStop(int padId)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : biAddRumbleEffect
	Purpose : add rumble effect
	Parameters : padId, motorId, amplitude, duration, attack, sustain, delay, style
	Returns : OK/FAIL
	Info : 
*/
int biAddRumbleEffect(int padId, int motorId, int amplitude, int duration, int attack, int sustain, int delay, EBRumbleStyle style)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : biAddRumbleBatchEffect
	Purpose : add rumble batch effect
	Parameters : padId, noofEffects, effects
	Returns : 
	Info : 
*/
void biAddRumbleBatchEffect(int padId, int noofEffects, TBRumbleBatchEffect *effects)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : biPauseRumble
	Purpose : pause rumble
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/
int biPauseRumble()
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : biResumeRumble
	Purpose : resume rumble
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/
int biResumeRumble()
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : biStopRumble
	Purpose : stop rumble
	Parameters : padId
	Returns : 
	Info : 
*/
void biStopRumble(int padId)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : biSetAutoRepeat
	Purpose : set auto repeat
	Parameters : channel, startDelay, repeatsPerSecond
	Returns : 
	Info : 
*/
void biSetAutoRepeat(TBIInputChannel *channel, int startDelay, int repeatsPerSecond)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : biSetAutoRepeatMS
	Purpose : set auto repeat milliseconds
	Parameters : channel, startDelay, repeatDelay
	Returns : 
	Info : 
*/
void biSetAutoRepeatMS(TBIInputChannel *channel, int startDelay, int repeatDelay)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bProcessRumble
	Purpose : process rumble
	Parameters : 
	Returns : 
	Info : 
*/
void bProcessRumble()
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : biGetPortStatus
	Purpose : get port status
	Parameters : portId
	Returns : port status
	Info : 
*/
EBPortStatus biGetPortStatus(int portId)
{
    return EBPORTSTATUS_INVALIDPORT;
}

/*	--------------------------------------------------------------------------------
	Function : bFixupPlayBackData
	Purpose : fixup playback data
	Parameters : recInfo
	Returns : 
	Info : 
*/
void bFixupPlayBackData(TBInputRecording *recInfo)
{
    return;
}
