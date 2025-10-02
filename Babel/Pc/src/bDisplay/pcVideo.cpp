// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcVideo.cpp
//   Purpose : Defines flags for video playback, reason codes for video callbacks, and possible video playback results. Defines structs for video callback data
// Component : Babel Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bdVideoPlayStream
	Purpose : play video stream
	Parameters : filename, flags, callback, context
	Returns : video play stream result
	Info : 
*/
EBVideoPlayStream bdVideoPlayStream(char *filename, uint32 flags, TBVideoCallback callback, void *context)
{
    return EBVIDEOPLAYSTREAM_FINISHED;
}
