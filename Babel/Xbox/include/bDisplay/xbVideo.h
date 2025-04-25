// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games
//
//      File : xbVideo.h
//   Purpose : Video playback functions
// Component : Xbox Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BDISPLAY_XBVIDEO_H__
#define __BDISPLAY_XBVIDEO_H__

// ********************************************************************************
// Constants and Macros

// bdVideoPlayStream flags
#define BVIDEOPLAY_NOAUTOQUIT			0x00000001			// don't provide default abort controls
#define BVIDEOPLAY_SUMMARY				0x00000002			// send a summary to the debug window at the end of playback (Not PS2)
#define BVIDEOPLAY_NOEXCLUSIVEINPUT		0x00000004			// don't take exclusive control of the input maps
#define BVIDEOPLAY_QUITONPADEVENT		0x00000008			// quit if pads or memory cards are added/removed (PS2 & Xbox)
#define BVIDEOPLAY_TOTEXTURE			0x00000010			// Render to texture (Not PS2)
#define BVIDEOPLAY_PLAYPALSTREAM		0x00000020			// play a PAL video stream. (PS2 - Can't get info from bdOpenDisplay as bdVideoPlayStream may be called before)

// TBVideoCallback reason codes
typedef enum {
	EBVIDEOREASONCODE_INIT,				// called before playback starts
	EBVIDEOREASONCODE_SHUTDOWN,			// called after playback stops
	EBVIDEOREASONCODE_RENDER,			// called when a frame needs to be rendered
	EBVIDEOREASONCODE_TESTQUIT,			// called to see if we should quit
} EBVideoReasonCode;


// possible video playback results
typedef enum {
	EBVIDEOPLAYSTREAM_FINISHED,
	EBVIDEOPLAYSTREAM_MEMCARDCHANGE,
	EBVIDEOPLAYSTREAM_PADCHANGE,
	EBVIDEOPLAYSTREAM_USERQUIT,
	EBVIDEOPLAYSTREAM_CALLBACKQUIT,
	EBVIDEOPLAYSTREAM_INITFAILED,
	EBVIDEOPLAYSTREAM_ERROR,
} EBVideoPlayStream;


// ********************************************************************************
// Types, Structures and Classes

// callback data block for EBVIDEOREASONCODE_INIT reason code
// return code :
//		TRUE if okay for playback or FALSE to abort
typedef struct {
	int					xDim, yDim;				// video dimensions
	int					noofFrames;				// video #frames
} TBVideoData_Init;


// callback data block for EBVIDEOREASONCODE_SHUTDOWN reason code
// return code : 
//		none
typedef struct {
	int					xDim, yDim;				// video dimensions
	int					noofFrames;				// video #frames
} TBVideoData_Shutdown;


// callback data block for EBVIDEOREASONCODE_RENDER reason code
// return code : 
//		TRUE for default rendering, FALSE if callback took care of it
typedef struct {
	TBTexture			*texture;				// texture containing frame (linear)
	int					xDim, yDim;				// video dimensions
	int					noofFrames;				// video #frames
} TBVideoData_Render;


// callback data block for EBVIDEOREASONCODE_TESTQUIT reason code
// return code : 
//		TRUE to keep playing or FALSE to abort
typedef struct {
	int					xDim, yDim;				// video dimensions
	int					noofFrames;				// video #frames
} TBVideoData_TestQuit;


// callback for bdVideoPlaybackStream
typedef int (*TBVideoCallback)(EBVideoReasonCode reason, int frame, void *data, void *context);


// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bdVideoPlayStream
   Purpose : play a video file
   Parameters : filename, flags (see BVIDEOPLAY_), callback handler, context for callback handler
   Returns : return code
   Info : 
*/

EBVideoPlayStream bdVideoPlayStream(char *filename, uint32 flags=0, TBVideoCallback callback=NULL, void *context=NULL);


#endif		// __BDISPLAY_XBVIDEO_H__
