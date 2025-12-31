// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : input.cpp
//   Purpose : Defines constants for max channel value, special POV hat value, channel types, channel flags, max bindings/inputs, name length, binding signs, port IDs, force feedback parameters, Xbox motor speed, device types, rumble styles, max rumble effects/amplitude, rumble array size, motor IDs, and port status
// Component : Babel Input
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Globals

TBInputStateInfo	bInputState;									// general input state info
TBRumbleController	bRumbleControllers[BMAXPADS][BRUMBLEARRAYSIZE];	// rumble controller array
TBIChannelMap	   *bInputMapList;									// root of the channel map list

TBIChannelMap	   *bExclusiveMap;

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
    TBIChannelMap *map = (TBIChannelMap *)MALLOCEX(
        maxChannels * sizeof(TBIInputChannel) + sizeof(TBIChannelMap),
        (uint32)"Input Map");
    if (!map)
    {
        bkPrintf("biCreateMap: ERROR - Out of memory\n");
        return NULL;
    }

    map->maxChannels  = maxChannels;
    map->noofChannels = 0;
    map->channels     = (TBIInputChannel *)(map + 1);

    memset(map->channels, 0, maxChannels * sizeof(TBIInputChannel));

    map->flags = 0;

    if (bInputMapList)
    {
        TBIChannelMap *head = bInputMapList;
        TBIChannelMap *tail = head->prev;

        map->next = head;
        map->prev = tail;

        tail->next = map;
        head->prev = map;
    }
    else
    {
        bInputMapList = map;
        map->next     = map;
        map->prev     = map;
    }

    return map;
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
    TBIChannelMap *map = bInputMapList;

    while (map)
    {
        TBIChannelMap *next = map->next;
        TBIChannelMap *prev = map->prev;

        // Unlink current map from the circular doubly-linked list
        next->prev = prev;
        prev->next = next;

        // Update the list head if we are removing it
        if (bInputMapList == map)
        {
            if (next == map)
            {
                // Last element in the list
                bInputMapList = NULL;
            }
            else
            {
                // Move head to the next element
                bInputMapList = next;
            }
        }

        bkHeapFree(map);
        map = bInputMapList;
    }
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
    int32 maxChannels = map->maxChannels;

    // map full?
    if (map->noofChannels == maxChannels)
    {
        bkPrintf("biCreateChannel: ERROR - Map is full\n");
        return NULL;
    }

    // find first unused channel slot
    int index = 0;
    if (maxChannels > 0)
    {
        for (; index < maxChannels; ++index)
        {
            if (map->channels[index].type == BICHANNELTYPE_UNUSED)
                break;
        }
    }

    TBIInputChannel *channel = &map->channels[index];

    strcpy(channel->name, name);

    channel->type         = (EBIChannelType)type;
    channel->noofBindings = 0;
    channel->value        = 0;
    channel->flags        = flags;
    channel->map          = map;
    channel->duration     = 0;

    if (maxValue == 0)
    {
        maxValue = BIMAX;
    }
    channel->maxValue = maxValue;

    TBTimerValue startDelayTicks  = bTimerFrequency / 3;   // "start" delay
    TBTimerValue repeatDelayTicks = bTimerFrequency / 20;  // "repeat" delay

    channel->autoRepeatStartDelay  = (uint32)startDelayTicks;
    channel->autoRepeatRepeatDelay = (uint32)repeatDelayTicks;

    channel->accIncSpeed = accIncSpeed;
    channel->accDecSpeed = accDecSpeed;

    map->noofChannels++;

    return channel;
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
    // If both IDs do not use all devices (BIINPUT_ALLDEVS)
    if ( (posInputID & BIINPUT_DEVMASK) != BIINPUT_ALLDEVS &&
         (negInputID & BIINPUT_DEVMASK) != BIINPUT_ALLDEVS )
    {
        if (channel->noofBindings == BIMAX_CHANNEL_BINDINGS)
        {
            bkPrintf("biBindInput: ERROR - Channel '%s' is full\n", channel->name);
            return FAIL;
        }

        TBIBinding &binding = channel->bindings[channel->noofBindings];

        binding.inputID[0] = posInputID;
        binding.inputID[1] = negInputID;
        binding.noofInputs = 1;

        // Initialize the channel value with the current state of the bound inputs
        int posValue = 0;
        int negValue = 0;

        bReadInput(posInputID, channel->type, &posValue);
        bReadInput(negInputID, channel->type, &negValue);

        channel->noofBindings++;
        channel->value = (posValue | negValue);

        return OK;
    }

    // If at least one of the IDs uses BIINPUT_ALLDEVS, we multiply the binding by devices 0 and 1
    for (int i = 0; i < 2; ++i)
    {
        int32 expandedPosID = 0;
        int32 expandedNegID = 0;

        if (negInputID == 0)
            expandedNegID = 0;
        else
            expandedNegID = (negInputID & ~BIINPUT_DEVMASK) | BIINPUT_DEV(i);

        if (posInputID == 0)
            expandedPosID = 0;
        else
            expandedPosID = (posInputID & ~BIINPUT_DEVMASK) | BIINPUT_DEV(i);

        biBindInput(channel, expandedPosID, expandedNegID);
    }

    return OK;
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
    int32 i;
    int32 value;

    if (channel->noofBindings == 6) {
        bkPrintf("biBindMultiInput: ERROR - Channel is full\n");
        return FAIL;
    }

    if (noofInputs > 6) {
        bkPrintf("biBindMultiInput: ERROR - Too many inputs\n");
        return FAIL;
    }

    if (noofInputs > 0) {
        va_list args;
        va_start(args, noofInputs);

        for (i = 0; i < noofInputs; i++) {
            int32 inputID = va_arg(args, int32);

            channel->bindings[channel->noofBindings].inputID[i] = inputID;

            bReadInput(inputID, channel->type, &value);
            if (value != 0) {
                channel->value = value;
            }
        }

        va_end(args);
    }

    channel->bindings[channel->noofBindings].noofInputs = noofInputs;

    if ((channel->type == BICHANNELTYPE_BIANALOGUE) && (noofInputs == 1)) {
        channel->bindings[channel->noofBindings].inputID[1] = 0;
    }

    channel->noofBindings++;
    return OK;
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
    TBIChannelMap* mapHead = bInputMapList;
    TBIChannelMap* mapCursor = bInputMapList;

    // Read raw device state (keyboard, mouse, pads, etc.)
    bReadPhysicalInputDevices();

    if (mapHead != NULL)
    {
        do
        {
            TBIChannelMap* currentMap = mapCursor;

            // Skip disabled maps and maps that are not currently exclusive (if exclusivity is set)
            if ( (currentMap->flags & 0x1) == 0 &&
                 (bExclusiveMap == NULL || currentMap == bExclusiveMap) &&
                 currentMap->maxChannels > 0 )
            {
                int channelIndex = 0;

                while (channelIndex < currentMap->maxChannels)
                {
                    int channelIndexSnapshot = channelIndex;
                    TBIInputChannel* channelArray = currentMap->channels;
                    TBIInputChannel* channel = &channelArray[channelIndexSnapshot];

                    EBIChannelType channelType = channel->type;
                    int noofBindings = channel->noofBindings;

                    if (channelType != BICHANNELTYPE_UNUSED && noofBindings > 0)
                    {
                        // Common temporaries
                        int  bindingIndex;
                        int* bindingInputCountPtr;
                        unsigned int inputValue = 0;
                        unsigned int andMask;
                        bool bindingValueApplied = false;

                        // --- DIGITAL CHANNEL ---
                        if (channelType == BICHANNELTYPE_DIGITAL)
                        {
                            channel->value = 0;

                            for (bindingIndex = 0;
                                 bindingIndex < noofBindings;
                                 ++bindingIndex)
                            {
                                TBIBinding* binding = &channel->bindings[bindingIndex];
                                int inputsCount   = binding->noofInputs;

                                bool anyInputActive = false;
                                andMask             = 0x7ff8;

                                if (inputsCount > 0)
                                {
                                    // For digital: behaviour=1, combine all inputs via AND mask
                                    for (int inputIdx = 0; inputIdx < inputsCount; ++inputIdx)
                                    {
                                        if (bReadInput(binding->inputID[inputIdx], 1, (int*)&inputValue))
                                        {
                                            anyInputActive = true;
                                            andMask &= inputValue;
                                        }
                                    }

                                    if (anyInputActive && andMask != 0)
                                    {
                                        channel->value = channel->maxValue;
                                        break; // this binding fully defines the channel
                                    }
                                }
                            }
                        }
                        // --- ANALOG CHANNEL (single-axis, 0..maxValue) ---
                        else if (channelType == BICHANNELTYPE_ANALOGUE)
                        {
                            // If there is no acceleration, reset on each read
                            if (channel->accIncSpeed == 0)
                                channel->value = 0;

                            bindingValueApplied = false;

                            bindingInputCountPtr = &channel->bindings[0].noofInputs;
                            bindingIndex         = 0;

                            while (bindingIndex < noofBindings)
                            {
                                TBIBinding* binding = (TBIBinding*)(bindingInputCountPtr - 6);
                                int         inputsCount = *bindingInputCountPtr;

                                // Inputs beyond the first can be gating conditions (behaviour=1)
                                andMask = 0x7ff8;
                                if (inputsCount > 1)
                                {
                                    int* extraInputIDPtr = bindingInputCountPtr - 5;
                                    for (int inputIdx = 1; inputIdx < inputsCount; ++inputIdx)
                                    {
                                        if (bReadInput(*extraInputIDPtr, 1, (int*)&inputValue))
                                        {
                                            andMask &= inputValue;
                                        }
                                        ++extraInputIDPtr;
                                    }

                                    // If gating mask cuts everything to zero, stop scanning further bindings
                                    if (andMask == 0)
                                        break;
                                }

                                // Main input for this binding: behaviour=2 (analogue)
                                unsigned int axisValue = 0;
                                if (bReadInput(binding->inputID[0], 2, (int*)&axisValue) && axisValue != 0)
                                {
                                    // Scale to channel range
                                    int target = (int)( (channel->maxValue * (int)axisValue) / 0x7ff8 );

                                    if (channel->accIncSpeed == 0)
                                    {
                                        channel->value = target;
                                    }
                                    else
                                    {
                                        // Smooth towards target using accIncSpeed
                                        int current = channel->value;

                                        if (current < target)
                                        {
                                            current += channel->accIncSpeed;
                                            if (current > target)
                                                current = target;
                                            channel->value = current;
                                        }
                                        else
                                        {
                                            current -= channel->accIncSpeed;
                                            if (current < target)
                                                current = target;
                                            channel->value = current;
                                        }
                                    }

                                    // Optional invert bit (0x100) on first inputID
                                    if ((binding->inputID[0] & 0x100u) != 0u)
                                    {
                                        channel->value = -channel->value;
                                    }

                                    bindingValueApplied = true;
                                    break; // this binding controls the channel
                                }

                                ++bindingIndex;
                                bindingInputCountPtr += 7; // next TBIBinding (struct stride)
                            }

                            // Automatic decay if no binding updated the channel this frame
                            int decSpeed = channel->accDecSpeed;
                            if (decSpeed != 0 && !bindingValueApplied)
                            {
                                int v = channel->value;
                                if (v > 0)
                                {
                                    v -= decSpeed;
                                    if (v < 0)
                                        v = 0;
                                    channel->value = v;
                                }
                            }
                        }
                        // --- BIANALOG CHANNEL (bi-directional, -maxValue..maxValue) ---
                        else if (channelType == BICHANNELTYPE_BIANALOGUE)
                        {
                            if (channel->accIncSpeed == 0)
                                channel->value = 0;

                            bindingValueApplied = false;

                            bindingInputCountPtr = &channel->bindings[0].noofInputs;
                            bindingIndex         = 0;

                            while (bindingIndex < noofBindings)
                            {
                                TBIBinding* binding   = (TBIBinding*)(bindingInputCountPtr - 6);
                                int         inputsCnt = *bindingInputCountPtr;

                                // inputs from index >=2 are gating inputs (behaviour=1)
                                andMask = 0x7ff8;
                                if (inputsCnt > 2)
                                {
                                    int* extraInputIDPtr = bindingInputCountPtr - 4;
                                    for (int inputIdx = 2; inputIdx < inputsCnt; ++inputIdx)
                                    {
                                        if (bReadInput(*extraInputIDPtr, 1, (int*)&inputValue))
                                        {
                                            andMask &= inputValue;
                                        }
                                        ++extraInputIDPtr;
                                    }

                                    if (andMask == 0)
                                        break;
                                }

                                // If bindingInputCountPtr[-5] == 0 > single-axis, read as behaviour=3
                                if (bindingInputCountPtr[-5] == 0)
                                {
                                    unsigned int axisValue = 0;
                                    if (bReadInput(binding->inputID[0], 3, (int*)&axisValue) &&
                                        axisValue != 0)
                                    {
                                        int v = (int)( (channel->maxValue * (int)axisValue) / 0x7ff8 );
                                        channel->value = v;

                                        if ((binding->inputID[0] & 0x100u) != 0u)
                                            channel->value = -channel->value;

                                        bindingValueApplied = true;
                                        break;
                                    }
                                }
                                else
                                {
                                    // Two-axis binding: positive + negative (both behaviour=2)
                                    int positiveValue = 0;
                                    int negativeValue = 0;

                                    if (!bReadInput(binding->inputID[0], 2, &positiveValue))
                                        positiveValue = 0;
                                    if (!bReadInput(bindingInputCountPtr[-5], 2, &negativeValue))
                                        negativeValue = 0;

                                    int signedInput = positiveValue - negativeValue;
                                    if (signedInput != 0)
                                    {
                                        int target = (int)( (channel->maxValue * signedInput) / 0x7ff8 );

                                        if (channel->accIncSpeed == 0)
                                        {
                                            channel->value = target;
                                        }
                                        else
                                        {
                                            // If we need to cross zero and flag&8 is set, we can snap to 0
                                            if (target <= 0 && channel->value > 0)
                                            {
                                                if ((channel->flags & 0x8) != 0)
                                                    channel->value = 0;
                                            }
                                            else if (target >= 0 && channel->value < 0)
                                            {
                                                if ((channel->flags & 0x8) != 0)
                                                    channel->value = 0;
                                            }

                                            int current = channel->value;
                                            if (current < target)
                                            {
                                                current += channel->accIncSpeed;
                                                if (current > target)
                                                    current = target;
                                                channel->value = current;
                                            }
                                            else
                                            {
                                                current -= channel->accIncSpeed;
                                                if (current < target)
                                                    current = target;
                                                channel->value = current;
                                            }
                                        }

                                        if ((binding->inputID[0] & 0x100u) != 0u)
                                            channel->value = -channel->value;

                                        bindingValueApplied = true;
                                        break;
                                    }
                                }

                                ++bindingIndex;
                                bindingInputCountPtr += 7; // next TBIBinding
                            }

                            // Bi-analogue decay towards zero if nothing drove the value
                            int decSpeed = channel->accDecSpeed;
                            if (decSpeed != 0 && !bindingValueApplied)
                            {
                                int v = channel->value;
                                if (v > 0)
                                {
                                    v -= decSpeed;
                                    if (v < 0)
                                        v = 0;
                                    channel->value = v;
                                }
                                else if (v < 0)
                                {
                                    v += decSpeed;
                                    if (v > 0)
                                        v = 0;
                                    channel->value = v;
                                }
                            }
                        }

                        // --- duration / auto-repeat handling (common for all non-unused channels) ---
                        if (channel->value == 0)
                        {
                            channel->duration = 0;
                        }
                        else if ((channel->flags & 0x1) == 0) // not "single-shot"
                        {
                            int duration = channel->duration;

                            if ((channel->flags & 0x4) == 0)
                            {
                                // Simple frame-based hold counter
                                channel->duration = duration + 1;
                            }
                            else
                            {
                                // Auto-repeat
                                TBTimerValue nowTime;
                                TBTimerValue elapsedTime;

                                if (duration == 0)
                                {
                                    nowTime = bkTimerRead();
                                    channel->startTime = nowTime;
                                    channel->duration  = 1;
                                }
                                else if (duration == 1)
                                {
                                    nowTime     = bkTimerRead();
                                    elapsedTime = bkTimerDelta(channel->startTime, nowTime);

                                    // Start delay before auto-repeat kicks in
                                    if (elapsedTime < 0 ||
                                        (unsigned int)elapsedTime < channel->autoRepeatStartDelay)
                                    {
                                        channel->value = 0;
                                    }
                                    else
                                    {
                                        channel->duration  = 3;
                                        channel->startTime = nowTime;
                                    }
                                }
                                else if (duration == 3)
                                {
                                    nowTime     = bkTimerRead();
                                    elapsedTime = bkTimerDelta(channel->startTime, nowTime);

                                    // Repeat delay between repeated pulses
                                    if (elapsedTime >= 0 &&
                                        (unsigned int)elapsedTime >= channel->autoRepeatRepeatDelay)
                                    {
                                        channel->duration  = 3;
                                        channel->startTime = nowTime;
                                    }
                                    else
                                    {
                                        channel->value = 0;
                                    }
                                }
                                else
                                {
                                    // Any other duration state > drop value
                                    channel->value = 0;
                                }
                            }
                        }
                        else
                        {
                            // Single-shot channels: value is only valid for a short duration
                            int duration = channel->duration + 1;
                            channel->duration = duration;
                            if (duration > 1)
                            {
                                channel->value = 0;
                            }
                        }
                    }

                    ++channelIndex;
                    mapHead = bInputMapList; // keeps circular list termination in sync
                }
            }

            mapCursor = currentMap->next;
        } while (mapCursor != mapHead);
    }

    // Pad-change event generation
    TBInputStateInfo prevInputState;
    prevInputState.padsHaveChanged = bInputState.padsHaveChanged;

    if (bInputState.padsHaveChanged != 0)
    {
        bkGenerateEvent("_PadChange", NULL, NULL, 1);
    }

    bInputState.padsHaveChanged = 0;
    return prevInputState.padsHaveChanged;
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
    int32 idx;
    int32 ofs;

    idx = 0;
    if (0 < map->maxChannels)
    {
        ofs = 0;
        do
        {
            *(int32 *)((uchar *)map->channels + ofs + 0x14) = 0;
            idx = idx + 1;
            ofs = ofs + 0xF0;
        } while (idx < map->maxChannels);
    }
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
	channelPtr->value = 0;
	channelPtr->duration = 2;
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
	for (int i = 0; i < BMAXPADS; i++)
	{
		if (index == 0) return i;
		index--;
	}
	return -1;
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
    bInputState.padsHaveChanged = 0;
    bInitInputHAL(flags);
    bInitGameSave();
    memset(bRumbleControllers, 0, sizeof(bRumbleControllers));
    bkCreateEvent("_Reset");
    bkCreateEvent("_PadChange");
    bkCreateEvent("_MemCardChange");
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
    biDeleteAllMaps();
	bShutdownGameSave();
    bShutdownInputHAL();
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
