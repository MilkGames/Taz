// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : input.cpp
//   Purpose : Defines constants for max channel value, special POV hat value, channel types, channel flags, max bindings/inputs, name length, binding signs, port IDs, force feedback parameters, Xbox motor speed, device types, rumble styles, max rumble effects/amplitude, rumble array size, motor IDs, and port status
// Component : Babel Input
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Locals

int stopSpam2 = 0;
int stopSpam5 = 0;
int stopSpam6 = 0;

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
        bkPrintf("*** WARNING *** biCreateMap was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biDeleteMap was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biDeleteAllMaps was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
	if (!stopSpam5) {
        bkPrintf("*** WARNING *** biCreateChannel was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
		stopSpam5++;
	}
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
        bkPrintf("*** WARNING *** biGetChannelName was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biDeleteChannel was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
	if (!stopSpam6) {
        bkPrintf("*** WARNING *** biBindInput was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
		stopSpam6++;
	}
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
        bkPrintf("*** WARNING *** biBindMulti was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biResetChannel was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biResetMap was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
    if (!stopSpam2) {
		bkPrintf("*** WARNING *** biReadDevices was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
		stopSpam2++;
	}
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
        bkPrintf("*** WARNING *** biGetChar was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biFlushKeyBuffer was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biQueryDevice was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biZeroMap was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biForceDebounce was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biEnableMap was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biStartRecording was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biRecordInputs was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biStopRecording was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biStartPlayback was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biPlaybackInputs was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biFindPad was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biFFSetParm was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biSetMapPhysicalPort was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biSetChannelPhysicalPort was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biSwapMapPhysicalPort was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biSwapChannelPhysicalPort was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bInitInput was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bShutdownInput was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biFFStopAll was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biFFStop was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biAddRumbleEffect was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biAddRumbleBatchEffect was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biPauseRumble was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biResumeRumble was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biStopRumble was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biSetAutoRepeat was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biSetAutoRepeatMS was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bProcessRumble was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** biGetPortStatus was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bFixupPlayBackData was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}
