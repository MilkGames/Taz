// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : display.cpp
//   Purpose : provide basic display setup and managment functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "main.h"
#include "maths.h"
#include "display.h"
#include "textfx.h"
#include "colour.h"
#include "Stack.h"		 		// PP: Stack class - please use it!
#include "Cheats.h"				// PP: Cheats
#include "VideoFX.h"			// PP: Video effects - motion blur, defocus, whatever.  Top-of-the-Pops-TASTIC, Mate!



// TP: Choose your video mode here
#ifdef CONSUMERDEMO

	#define VIDEOMODE	(BDISPLAYFLAG_PAL)

#else

	#define VIDEOMODE	(BDISPLAYFLAG_NTSC)

#endif



// PP: PLEASE USE THESE FRAMEWRITE FLAG COMBOS INSTEAD OF THE FLAGS THEMSELVES
// PP: If you don't, stereo vision will get buggered and that would be unpleasant.
// PP: Stereo vision is my sweet sweet precious baby.  DON'T YOU TOUCH MA WEAN HEN!
// PP: I could redefine bdSetRenderState as a macro that enforces this, but I can imagine the overhead!
int								rgbFramewriteChannels		=BDFRAMEWRITE_RED|BDFRAMEWRITE_GREEN|BDFRAMEWRITE_BLUE;
// PP: REMOUT: so's people don't use it by accident	(more often than not, we shouldn't write to the alpha channel)		int								allFramewriteChannels		=BDFRAMEWRITE_ALL;

// PP: to save you calling setFOV yourself every time you change number of screens...

static float					singleScreenFOV				=NORMAL_INGAME_FOV;
static float					horizontalSplitscreenFOV	=HORIZONTAL_SPLITSCREEN_FOV;
static float					verticalSplitscreenFOV		=VERTICAL_SPLITSCREEN_FOV;

// PP: flag specifying that widescreen projection is to be overridden (forcing normal 4:3)
// PP: set by overrideWidescreen
static bool						g_overrideWidescreen		=false;

// PP: flag specifying that stereoscopy is to be overridden
// PP: set by overrideStereoscopy
bool							g_overrideStereoscopy		=false;

// PP: size of render target stack
#define RENDER_TARGET_STACK_DEPTH	5	// PP: more than enough

// PP: stack of render targets, controlled by pushRenderTarget and popRenderTarget
static STACK<TBRenderTarget, RENDER_TARGET_STACK_DEPTH>		renderTargetStack("renderTargetStack");

// PP: define ORTSTACK_DEBUG to get debug messages about the stacking of render targets
//#define ORTSTACK_DEBUG


// TP: Choose your videos, moved here to tidy up main
#ifdef INFOGRAMES
	#if BPLATFORM == PC
		char	*videos[] = {"videos\\infoident_taz.bik", "videos\\warnerident_taz.bik", "videos\\blitzident_taz.bik"};
		char	numVideos = 3;
	#elif(BPLATFORM == XBOX)
		char	*videos[] = 
#ifdef CONSUMERDEMO// PP: TEMP: gotter account for subfolder paths (Ash is gonna do some fix for this)
		{"d:\\application\\infoident_tazhi.wmv", "d:\\application\\warnerident_tazhi.wmv", "d:\\application\\blitzident_taz.wmv"};
#else// PP: not demo*/
		{"d:\\infoident_tazhi.wmv", "d:\\warnerident_tazhi.wmv", "d:\\blitzident_taz.wmv"};
#endif// PP: not demo
		char	numVideos = 3;
	#else
		#ifndef	USE_HOST0
			#ifdef CONSUMERDEMO
				#ifdef	STANDALONE
					char	*videos[] = {"cdrom0:\\INFOID~1.PSS", "cdrom0:\\WARNER~1.PSS", "cdrom0:\\BLITZI~1.PSS"};
				#else
					char	*videos[] = {"cdrom0:\\TAZDEMO\\INFOID~1.PSS", "cdrom0:\\TAZDEMO\\WARNER~1.PSS", "cdrom0:\\TAZDEMO\\BLITZI~1.PSS"};
				#endif
			#else
				char	*videos[] = {"cdrom0:\\INFOID~1.PSS", "cdrom0:\\WARNER~1.PSS", "cdrom0:\\BLITZI~1.PSS"};
			#endif	
		#else
			char	*videos[] = {"host0:\\taz\\InfoIdent_Taz_ntsc.pss", "host0:\\taz\\WarnerIdent_Taz_ntsc.pss", "host0:\\taz\\BlitzIdent_Taz_ntsc.pss"};
		#endif
		char	numVideos = 3;
	#endif
#else
	#if BPLATFORM == PC
		char	*videos[] = {"videos\\infoident_taz.bik", "videos\\warnerident_taz.bik", "videos\\blitzident_taz.bik"};
		char	numVideos = 3;
	#else
		char	*videos[] = {"blitz.pss"};
		char	numVideos = 1;
	#endif
	
#endif



/* --------------------------------------------------------------------------------
   Function		: initDisplay
   Purpose		: initialise display settings at the beginning of the game
   Parameters	: 
   Returns		: 
   Info			: // PP: moved this here from main.cpp to keep things tidy - hope you don't mind
*/

void initDisplay(void)
{
	// PP: ~~~~~~~~~~~ SAFE AREAS (portions of the screen width/height within which all text and overlays must be contained)...

	// PP: unless there's a TRC about this we should just use our own judgement - it was previously 0.95 but that's a bit too generous
#define PS2_SAFE_AREA				0.87f

	// PP: unless there's a TRC about this we should just use our own judgement
#define PC_SAFE_AREA				0.95f

	// PP: from the TCR
	// PP: NOTE: I've shrunk these in a wee bit because, much as it hurts me to say it, I think there was a teensy mis-fitting.
	// PP: Maybe outlines/italics/glyphs aren't being taken into account?  Who knows.
	// PP: Maybe on the next game I'll have time to be more perfectionistic about it.
	// PP: For just now though, this is a sound solution.
	// PP: Just thank the stars that we can adjust this with a one-line tweak anyway!!
	// PP: See that?  Foresight.  Bruce Foresight.  Nice to foresee you, to foresee you, nice!
#define XBOX_HDTV_SAFE_AREA			0.85f// PP: MAX 0.9f
#define XBOX_NON_HDTV_SAFE_AREA		0.81f// PP: MAX 0.85f

	// PP: unless there's a TRC about this we should just use our own judgement
#define GAMECUBE_SAFE_AREA			PS2_SAFE_AREA

	// PP: ~~~~~~~~~~~

#if BPLATFORM == PS2

	videoMode.xScreen = 640;
	videoMode.yScreen = 448;
	videoMode.bppScreen = 16;
	videoMode.zScreen = 32;
	videoMode.nearPlane = 50;
	videoMode.farPlane = FARCLIP;
	videoMode.safeWidthPortion = PS2_SAFE_AREA;
	videoMode.safeHeightPortion = PS2_SAFE_AREA;

#if (VIDEOMODE==BDISPLAYFLAG_PAL)
	videoMode.yScreen = 512;	
	videoMode.flags = BDISPLAYFLAG_PAL|BDISPLAYFLAG_ANTIALIASED|BDISPLAYFLAG_FIELD;
#else
	videoMode.flags = BDISPLAYFLAG_NTSC|BDISPLAYFLAG_ANTIALIASED|BDISPLAYFLAG_FIELD;
#endif

#elif BPLATFORM == PC

	videoMode.xScreen = 640;
	videoMode.yScreen = 480;
	videoMode.bppScreen = 32;
	videoMode.zScreen = 32;
	videoMode.nearPlane = 50;
	videoMode.farPlane = FARCLIP;
	videoMode.flags = VIDEOMODE;
	videoMode.safeWidthPortion = PC_SAFE_AREA;
	videoMode.safeHeightPortion = PC_SAFE_AREA;

#elif BPLATFORM == XBOX

	// PP: SET UP STUFF WE'RE SURE ABOUT...

	videoMode.bppScreen = 32;
	videoMode.zScreen = 32;
	videoMode.nearPlane = 50;
	videoMode.farPlane = FARCLIP;
	videoMode.flags = BDISPLAYFLAG_VERBOSE;

	// PP: DETERMINE BEST POSSIBLE RES ETC...

	DWORD	videoFlags		=XGetVideoFlags();
	DWORD	videoStandard	=XGetVideoStandard();

	// PP: right - first off: PAL or NTSC?

// PP: XBOX TCR SAYS NO WIDESCREEN AND NO HDTV IN A CONSUMER DEMO
#ifndef CONSUMERDEMO
// PP: REMOUT: unable to test it since I don't have an HDTV		#define XBOX_SUPPORT_HDTV
#endif// PP: ndef CONSUMERDEMO

#ifdef XBOX_SUPPORT_HDTV

	if(videoFlags & XC_VIDEO_FLAGS_HDTV_480p)
	{
		// PP: TODO: use D3DPRESENTFLAG_PROGRESSIVE

		videoMode.xScreen=720;
		videoMode.yScreen=480;

		videoMode.safeHeightPortion=XBOX_HDTV_SAFE_AREA;
		videoMode.safeWidthPortion=XBOX_HDTV_SAFE_AREA;
	}
	else if(videoFlags & XC_VIDEO_FLAGS_HDTV_720p)
	{
		// PP: TODO: use D3DPRESENTFLAG_PROGRESSIVE

		videoMode.xScreen=1280;
		videoMode.yScreen=720;

		videoMode.safeHeightPortion=XBOX_HDTV_SAFE_AREA;
		videoMode.safeWidthPortion=XBOX_HDTV_SAFE_AREA;
	}
	else if(videoFlags & XC_VIDEO_FLAGS_HDTV_1080i)
	{
		// PP: TODO: use D3DPRESENTFLAG_INTERLACED

		videoMode.xScreen=1920;
		videoMode.yScreen=1080;

		videoMode.safeHeightPortion=XBOX_HDTV_SAFE_AREA;
		videoMode.safeWidthPortion=XBOX_HDTV_SAFE_AREA;
	}
	else

#endif// PP: def XBOX_SUPPORT_HDTV

	{
		// PP: NORMAL TELLY

		videoMode.safeHeightPortion=XBOX_NON_HDTV_SAFE_AREA;
		videoMode.safeWidthPortion=XBOX_NON_HDTV_SAFE_AREA;

		// PP: TODO: use D3DPRESENTFLAG_INTERLACED

		if(videoStandard == XC_VIDEO_STANDARD_PAL_I)
		{
			// PP: PAL

			videoMode.flags |= BDISPLAYFLAG_PAL;

			videoMode.xScreen=720;
			videoMode.yScreen=576;

			// PP: 60Hz?  (NOT THAT WE CARE)

			if(videoFlags & XC_VIDEO_FLAGS_PAL_60Hz)
			{
				// PP: SO WHAT
				int x=0;
			}
		}
		else
		{
			// PP: NTSC

			videoMode.flags |= BDISPLAYFLAG_NTSC;
			videoMode.flags |= BDISPLAYFLAG_ANTIALIASED;

			// PP: RIGHT THEN.
			// PP: On a ***DISK***, you can only antialias at 640x480.  720x480 cannot be antialiased on a ***DISK***.
			// PP: I have no idea why - do you?

			// PP: NOTE: 640x480 is too low a res not to use antialiasing, it'd look nasty

			videoMode.xScreen=640;
			videoMode.yScreen=480;
		}
	}

    // Adjust presentation parameters
	/* PP: NOTE: there's currently no way for me to get these flags applied in bdOpenDisplay

    m_d3dpp.Flags = displayModes[ currentMode ].fProgressive ? D3DPRESENTFLAG_PROGRESSIVE : D3DPRESENTFLAG_INTERLACED;
    m_d3dpp.Flags |= displayModes[ currentMode ].fWideScreen ? D3DPRESENTFLAG_WIDESCREEN : 0;
	*/

#elif BPLATFORM == GAMECUBE
	videoMode.xScreen = 640;
	videoMode.yScreen = 448;
	videoMode.bppScreen = 24;
	videoMode.zScreen = 24;
	videoMode.nearPlane = 50;
	videoMode.farPlane = FARCLIP;
	videoMode.flags = VIDEOMODE; //|BDISPLAYFLAG_ANTIALIASED|BDISPLAYFLAG_NOVSYNC;
	videoMode.safeWidthPortion=GAMECUBE_SAFE_AREA;
	videoMode.safeHeightPortion=GAMECUBE_SAFE_AREA;
#else
	#error "Must setup screen for this platform"
#endif

	// PP: no film borders yet ta
	setFilmBorders(false);

#if(BPLATFORM == XBOX)

#ifdef CONSUMERDEMO// PP: XBOX TCR SAYS NO WIDESCREEN AND NO HDTV IN A CONSUMER DEMO
	// PP: widescreen defaults to off
	setWidescreen(false);
#else// PP: not demo
	// PP: determine if widescreen has been turned on from the dashboard
	setWidescreen(XGetVideoFlags() & XC_VIDEO_FLAGS_WIDESCREEN);
#endif// PP: demo

#else// PP: not xbox

	// PP: widescreen defaults to off
	setWidescreen(false);

#endif// PP: not xbox

	// PP: normal field-of-view please
	setFOV(NORMAL_INGAME_FOV);

	// PP: initialise screens at very beginning of game...
	for(int i=0; i<2; i++)
	{
		videoMode.screens[i].prevfov
		=videoMode.screens[i].targetfov
		=NORMAL_INGAME_FOV;

		videoMode.screens[i].currentTime=0.0f;
		videoMode.screens[i].timescale=0.0f;
	}

	// PP: Initialise number of draws and current draw index
	videoMode.numDraws=1;
	videoMode.drawNum=0;

	// PP: initialise requested flags to be current flags
	videoMode.flagRequest = videoMode.flags;
}


/* --------------------------------------------------------------------------------
   Function : SwitchBetweenScreenModes
   Purpose : Change between windowed and fullscreen
   Parameters : 
   Returns : 
   Info : 
*/
void SwitchBetweenScreenModes(DISPLAYSETTINGS *displaySettings)
{
	bdCloseDisplay();
	if (displaySettings->flags&BDISPLAYFLAG_WINDOW)
	{
		displaySettings->flags &= ~BDISPLAYFLAG_WINDOW;
	}
	else
	{
		displaySettings->flags |= BDISPLAYFLAG_WINDOW;
	}
	SetupDisplay(displaySettings);
}

/* --------------------------------------------------------------------------------
   Function : SetupDisplay
   Purpose : Create and setup display
   Parameters : 
   Returns : 
   Info : 
*/
int SetupDisplay(DISPLAYSETTINGS *displaySettings)
{
	bkHeapStats();
	if (!bdOpenDisplay(displaySettings->xScreen,displaySettings->yScreen, 
		displaySettings->bppScreen, displaySettings->zScreen, displaySettings->flags))
	{
		bkAlert("Could not re-open display.");
		return FALSE;
	}
	bkPrintf( "--- --- Created Display --- ---\n" );
	int xRes, yRes, bpp, zDepth;
	bdGetDisplayInfo(&xRes, &yRes, &bpp, &zDepth);
	bkPrintf( "xRes: %d, yRes: %d, bpp: %d, zDepth: %d\n", xRes, yRes, bpp, zDepth );
	bkHeapStats();

	// PP: TEMPORARY BABEL WORKAROUND ~~~~~~~~~~~~
	bdSetViewport(int(videoMode.screens[0].viewport.left), int(videoMode.screens[0].viewport.top), int(videoMode.screens[0].viewport.widthAbs()), int(videoMode.screens[0].viewport.heightAbs()));
	// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	bdSetViewClipPlanes(displaySettings->nearPlane, displaySettings->farPlane);
	
	bdSetRenderState(BDRENDERSTATE_ANTIALIAS, FALSE, FALSE);
//	bdSetRenderState(BDRENDERSTATE_VSYNCFLIP, TRUE, FALSE);
//	bdSetRenderState(BDRENDERSTATE_CLIPPING, TRUE, FALSE);
	if (!map.fogState)
	{
		bdSetRenderState(BDRENDERSTATE_DISTANCEFOG, FALSE, FALSE);
		bdSetFogRange(METERTOUNIT(100.0f),METERTOUNIT(120.0f));
	}
	else
	{
		SetFog(map.fogMin,map.fogMax,map.fogColour);
	}
	bdSetRenderState(BDRENDERSTATE_BACKFACECULL, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_TEXTUREFILTER, BDTEXTUREFILTER_TRILINEAR, FALSE);

	bdSetIdentityObjectMatrix();
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);

	return TRUE;
}

/*  --------------------------------------------------------------------------------
	Function	: SetFog
	Purpose		: Set the current fog settings
	Parameters	: min and max values for the fog, colour for the fog
	Returns		: 
	Info		: 
*/
void SetFog(float min, float max, COLOUR colour)
{
	map.fogState = true;

#if BPLATFORM == PC
	float newMin, newMax;
	float minsPercentageofMax;

	minsPercentageofMax = min/(max/100.0f);
	
	max = videoMode.farPlane;
	min = minsPercentageofMax * (max/100.0f);
#endif // NH: BPLATFORM

	map.fogMin = min;
	map.fogMax = max;
	map.fogColour = colour;
	bdSetRenderState(BDRENDERSTATE_DISTANCEFOG, TRUE, FALSE);
	bdSetFogRange(map.fogMin,map.fogMax);
	bdSetFogColour(map.fogColour.r,map.fogColour.g,map.fogColour.b);
}

/*  --------------------------------------------------------------------------------
	Function	: DisableFog
	Purpose		: Like it sounds
	Parameters	: 
	Returns		: 
	Info		: 
*/
void DisableFog(void)
{
	map.fogState = false;
	bdSetRenderState(BDRENDERSTATE_DISTANCEFOG, FALSE, FALSE);
	bdSetFogRange(METERTOUNIT(100.0f),METERTOUNIT(120.0f));
}


/* --------------------------------------------------------------------------------
   Function : FadeFieldOfView
   Purpose : update field-of-view-fading for as many screens as are active
   Parameters : 
   Returns : 
   Info : // PP: removed displaysettings param - not needed really
			// PP: removed return value - not used and not multiscreen-friendly
*/
void FadeFieldOfView(void)
{
	float		rate;
	SCREEN*		screen;

	for(int i=0; i<gameStatus.multiplayer.numScreens; i++)
	{
		screen= &(videoMode.screens[i]);

		if((screen->currentTime -= fTime) > 0.0f)
		{
			/* PP: REMOUT: WHAT?!
			rate = (screen->targetfov - screen->prevfov) / screen->timescale;
			screen->fov = screen->targetfov - (rate*screen->currentTime);*/

			screen->fov=screen->prevfov+((screen->targetfov-screen->prevfov)*(screen->currentTime/screen->timescale));

			// PP: TEST
			setFOV(screen->fov, screen);
		}
		else
		{
			screen->fov = screen->targetfov;
			screen->prevfov = screen->fov;
		}
	}
}


/* --------------------------------------------------------------------------------
   Function : SetFadeFieldOfView
   Purpose : 
   Parameters : target fov, (opt/0.0f=instant change) timescale
   Returns : true if has reached target
   Info : // PP: removed displaysettings param - not needed really
		// PP: **NOTE** switched the other 2 params!
*/
void SetFadeFieldOfView(float targetFOV, float timescale)
{
	SCREEN*		screen;

	screen= &(videoMode.screens[0]);// PP: temp

	if(timescale >= 0.0f)
	{
		screen->timescale = timescale;
		screen->currentTime = timescale;
	}
	if(targetFOV < RAD(180.0f)) screen->targetfov = targetFOV;

	// PP: TEST
	setFOV(screen->fov, screen);
}

/*	--------------------------------------------------------------------------------
	Function 	: DrawRectangle
	Purpose 	: Draw a rectangle
	Parameters 	: rect, colour
	Returns 	: 
	Info 		: 
*/
void DrawRectangle(const RECTANGLE &rect, const COLOUR &colour)
{
	int i;
	TBPrimVertex	vert[4];

	BDVERTEX_SETXYZW(&vert[0],rect.left,rect.bottom,0.0f,1.0f);
	BDVERTEX_SETXYZW(&vert[1],rect.left,rect.top,0.0f,1.0f);
	BDVERTEX_SETXYZW(&vert[3],rect.right,rect.top,0.0f,1.0f);
	BDVERTEX_SETXYZW(&vert[2],rect.right,rect.bottom,0.0f,1.0f);

	for (i=0;i<4;i++) BDVERTEX_SETRGBA(&vert[i],colour.r,colour.g,colour.b,colour.a);

	bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,vert,4,BVERTTYPE_SINGLE);
}

/*	--------------------------------------------------------------------------------
	Function 	: DrawHollowRect
	Purpose 	: Draw hollow rectangle Pahhh
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void DrawHollowRect(const RECTANGLE &rect, const COLOUR &colour)
{
	DrawLine(rect.left, rect.top, rect.right, rect.top, colour);
	DrawLine(rect.right, rect.top, rect.right, rect.bottom, colour);
	DrawLine(rect.right, rect.bottom, rect.left, rect.bottom, colour);
	DrawLine(rect.left, rect.bottom, rect.left, rect.top, colour);
}

/*	--------------------------------------------------------------------------------
	Function 	: DrawLine
	Purpose 	: Draw a line using triangle strips
	Parameters 	: x1,y1,x2,y2, COLOUR
	Returns 	: 
	Info 		: 
*/
void DrawLine(const float x1,const float y1,const float x2,const float y2,const COLOUR &colour)
{
	TBPrimVertex	vert[4];
	int				i;

	for (i=3;i>=0;i--)
	{
		BDVERTEX_SETRGBA(&vert[i],colour.r,colour.g,colour.b,colour.a);
	}
	BDVERTEX_SETXYZW(&vert[0],x1,y1,0.0f,1.0f);
	BDVERTEX_SETXYZW(&vert[1],(x1+1),(y1-1),0.0f,1.0f);
	BDVERTEX_SETXYZW(&vert[2],x2,y2,0.0f,1.0f);
	BDVERTEX_SETXYZW(&vert[3],(x2+1),(y2-1),0.0f,1.0f);

	bdSetTexture(0,NULL);
	bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,vert,4,BVERTTYPE_SINGLE);
}


/*	--------------------------------------------------------------------------------
	Function 	: applyFOV
	Purpose 	: apply the fields of view for the current screen
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void applyFOV(void)
{
	SCREEN*		screen;

	screen= &CURRENT_SCREEN;

	bdSetFOV(screen->horzFov, screen->vertFov);
}


/*  --------------------------------------------------------------------------------
	Function	: overrideWidescreen
	Purpose		: override or un-override widescreen mode
	Parameters	: (opt/true)true=override/false=un-override,
					(opt/false)apply the override/un-override immediately
					(ie. don't wait till the end of this frame)
	Returns		: 
	Info		: // PP: Use this to force normal 4:3 aspect ratio, to correct
					problems with things that won't work in 16:9.
					Examples of this are 3D elements that have to line up exactly
					with a 4:3 splash screen (paperbooks, language select),
					and also things like the blink-eyes overlay, that also
					relies on 4:3 projection in order to fit the screen properly.
*/
void overrideWidescreen(const bool override, const bool immediate)
{
	bool	oldOverride;

	oldOverride=g_overrideWidescreen;

	g_overrideWidescreen=override;

	if(!gameStatus.wideScreenWilson)
	{
		// PP: dis don't apply man - skip all dem fov calcs - a'ight?
		return;
	}

	if(g_overrideWidescreen == oldOverride)
	{
		// PP: no change
		return;
	}

	for(int i=0; i<gameStatus.multiplayer.numScreens; i++)
	{
		// PP: reapply fov with the override flag in its (presumably) new state
		setFOV(videoMode.screens[i].fov, &videoMode.screens[i]);
	}

	if(immediate)
	{
		// PP: call applyFOV to effect the FOV alteration right now (mid-frame)
		applyFOV();
	}
}


/*  --------------------------------------------------------------------------------
	Function	: overrideStereoscopy
	Purpose		: override or un-override stereoscopic mode
	Parameters	: (opt/true)true=override/false=un-override
	Returns		: 
	Info		: // PP: use this to temporarily override stereoscopy,
					for things that don't want it, eg. paper books
*/
void overrideStereoscopy(const bool override)
{
	if(override)
	{
		if(!g_overrideStereoscopy)
		{
			g_overrideStereoscopy=true;

			videoMode.numDraws=1;

			// PP: re-enable all framewrite channels
			rgbFramewriteChannels=BDFRAMEWRITE_RED|BDFRAMEWRITE_GREEN|BDFRAMEWRITE_BLUE;
			bdSetRenderState(BDRENDERSTATE_FRAMEWRITE, rgbFramewriteChannels, 0);
			bdSetRenderState(BDRENDERSTATE_FRAMEWRITE, rgbFramewriteChannels, 1);

			videoMode.drawNum=0;	// PP: just in case
		}
	}
	else
	{
		if(g_overrideStereoscopy)
		{
			g_overrideStereoscopy=false;

			if(VFX_effectActive(VFX_STEREO))
			{
				videoMode.numDraws=2;
			}
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: setFOV
	Purpose 	: prepares the x&y fovs for the specified screen,
					taking into account screen dimensions, viewport dimensions and desired aspect ratio
	Parameters 	: vertical fov (the one that corresponds to screen height), (opt/NULL=current) screen whose fov to set
	Returns 	: 
	Info 		: the fovs are applied to the projection matrix by applyFOV next frame
					// PP: 'PANORAMIC' TYPE 16:9 IMPLEMENTED HERE - WIDESCREEN AS WILSON INTENDED IT!
*/
void setFOV(const float fov, SCREEN* const screen)
{
	float		ratio;

	screen->prevfov = screen->fov;
	screen->fov=screen->targetfov=fov;

	// PP: pre-calculate the tangent of the fov
	screen->tanFov=bmTan(screen->fov);

	// PP: get the ratio of the tangents of the desired horizontal & vertical fov's...

	float		x,y;
	float		finalXAspect, finalYAspect;

	if(g_overrideWidescreen)
	{
		finalXAspect=XASPECT_NORMAL;// PP: 4
		finalYAspect=YASPECT_NORMAL;// PP: 3		
	}
	else
	{
		finalXAspect=videoMode.xAspect;
		finalYAspect=videoMode.yAspect;
	}

	// PP: apply splitscreen
	x=(screen->viewport.widthAbs()/videoMode.xScreen);
	y=(screen->viewport.heightAbs()/videoMode.yScreen);

	// PP: apply desired aspect ratio (4:3 / 16:9)
	x*=((videoMode.xScreen/XASPECT_NORMAL)*finalXAspect);
	y*=((videoMode.yScreen/YASPECT_NORMAL)*finalYAspect);

	// PP: correct aspect ratio for resolutions other than the norm
	x/=(videoMode.xScreen/float(XSCREEN_NORMAL));
	y/=(videoMode.yScreen/float(YSCREEN_NORMAL));

	// PP: divide x by y ;)
	ratio=(x/y);

	// PP: vertical FOV is just 'fov'	// PP: TODO: pray that this should never change
	screen->vertFov = screen->fov;

	// PP: calculate horizontal FOV
	screen->horzFov=bmATan(bmTan(screen->vertFov*0.5f)*(ratio))*2.0f;

	// PP: record this FOV for the next time we switch to this screen configuration...

	if(gameStatus.multiplayer.numScreens == 1)
	{
		singleScreenFOV=fov;
	}
	else
	{
		if(gameStatus.multiplayer.splitScreenMode == SPLITSCREEN_HORIZONTAL)
		{
			horizontalSplitscreenFOV=fov;
		}
		else
		{
			verticalSplitscreenFOV=fov;
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: setViewport
	Purpose 	: Set the viewport.  Use this instead of bdSetViewport if you please
	Parameters 	: (opt/last)left X pos, (opt/last)top Y pos, (opt/last)width, (opt/last)height
	Returns 	: 
	Info 		: // PP: 
*/
/*void setViewport(const int x, const int y, const int width, const int height)
{
	// PP: MEESA THINKING THIS FUNCTION BEING...OUTTA DATE
*/
/*	videoMode.vpX=x;
	videoMode.vpY=y;
	videoMode.vpWidth=width;
	videoMode.vpHeight=height;*/
/*
	bdSetViewport(x, y, width, height);
}*/


/*	--------------------------------------------------------------------------------
	Function 	: setNumScreens
	Purpose 	: set the number of screens
	Parameters 	: number of screens, (opt/previous) split screen mode
	Returns 	: 
	Info 		: // PP: TODO?: some effect(s) for when the number of screens changes
*/
void setNumScreens(const int numScreens, const ESplitScreenMode split)
{
	gameStatus.multiplayer.numScreens=numScreens;
	gameStatus.multiplayer.splitScreenMode=split;

	if(numScreens == 1)
	{
		// PP: use full screen
		videoMode.screens[0].viewport=RECTANGLE(0, videoMode.xScreen, videoMode.yScreen, 0);

		// PP: set default fields of view
		setFOV(singleScreenFOV, &videoMode.screens[0]);

		// PP: IMPORTANT: set current screen index to be valid (it might currently be 1 = second screen, which isn't valid and could cause some bad shit later on in this frame)
		gameStatus.multiplayer.currentScreen=0;
	}
	else// PP: if(numScreen == 2)
	{
		// PP: no split-screen sterescopy!
		// PP: Disabling the cheat altogether is the easy option all-round - much less complicated than trying to keep it overridden in 2 player
		DeactivateCheat(CHEAT_COLOURSTEREOSCOPIC);

		if(split == SPLITSCREEN_HORIZONTAL)
		{
			// PP: split the screen into top & bottom halves
			videoMode.screens[0].viewport=RECTANGLE(0, videoMode.xScreen, (videoMode.yScreen/2), 0);
			videoMode.screens[1].viewport=RECTANGLE(0, videoMode.xScreen, videoMode.yScreen, (videoMode.yScreen/2));

			// PP: set default fields of view
			setFOV(horizontalSplitscreenFOV, &videoMode.screens[0]);
			setFOV(horizontalSplitscreenFOV, &videoMode.screens[1]);
		}
		else// PP: if(split == SPLITSCREENMODE_VERTICAL)
		{
			// PP: split the screen into left & right halves
			videoMode.screens[0].viewport=RECTANGLE(0, (videoMode.xScreen/2), videoMode.yScreen, 0);
			videoMode.screens[1].viewport=RECTANGLE((videoMode.xScreen/2), videoMode.xScreen, videoMode.yScreen, 0);

			// PP: set default fields of view
			setFOV(verticalSplitscreenFOV, &videoMode.screens[0]);
			setFOV(verticalSplitscreenFOV, &videoMode.screens[1]);
		}

		// PP: if we're going into 2-screen mode, we need to initialise the second camera before it gets used (this was causing a crash)
		// TP: InitCamera(&camera[1], ZVEC.v, ZVEC.v, false);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: toggleSplitScreenMode
	Purpose 	: toggle the split-screen mode
	Parameters 	: 
	Returns 	: new split-screen mode
	Info 		: // PP: TODO?: some effect(s) for when the split-screen mode changes
*/
ESplitScreenMode toggleSplitScreenMode(void)
{
	if(gameStatus.multiplayer.numScreens == 2)
	{
		setNumScreens(2, ESplitScreenMode(gameStatus.multiplayer.splitScreenMode^1));
	}
	else
	{
		bkPrintf("toggleSplitScreenMode:	OI! Ye're only in single-screen mode!\n");
	}

	return ESplitScreenMode(gameStatus.multiplayer.splitScreenMode);
}


/*	--------------------------------------------------------------------------------
	Function 	: setWidescreen
	Purpose 	: set widescreen mode on/off
	Parameters 	: true=on, false=off
	Returns 	: 
	Info 		: // PP: this sets gameStatus.wideScreenWilson and videoMode.xAspect&yAspect + ticks/unticks gui item
					// PP: TODO?: some effect for when widescreen toggles(?)
					// PP: NB - I'm not in the habit of using silly variable names!
*/
void setWidescreen(const bool Wilson)
{
	extern TBGuiMenuItem		*m16x9;		// PP: the widescreen gui item, in gui.cpp

	gameStatus.wideScreenWilson = Wilson;

	if(gameStatus.wideScreenWilson)	// PP: 16:9
	{
		// PP: tick the widescreen gui item
		if(m16x9 != NULL)
		{
			bgSetMenuItem(m16x9, NULL,NULL,NULL, BGUIMENUITEMFLAG_TICKED, BGUIMENUITEMFLAG_TICKED);
		}

		videoMode.xAspect=XASPECT_WIDESCREEN;// PP: 16
		videoMode.yAspect=YASPECT_WIDESCREEN;// PP: 9
	}
	else							// PP: 4:3
	{
		// PP: un-tick the widescreen gui item
		if(m16x9 != NULL)
		{
			bgSetMenuItem(m16x9, NULL,NULL,NULL, 0, BGUIMENUITEMFLAG_TICKED);
		}

		videoMode.xAspect=XASPECT_NORMAL;// PP: 4
		videoMode.yAspect=YASPECT_NORMAL;// PP: 3
	}

	// PP: setFOV again, to effect aspect ratio change
	setFOV(videoMode.screens[0].fov, &videoMode.screens[0]);

	if(gameStatus.multiplayer.numScreens == 2)
	{
		setFOV(videoMode.screens[1].fov, &videoMode.screens[1]);
	}
}


/* --------------------------------------------------------------------------------
   Function		: setFilmBorders
   Purpose		: set film-borders on/of
   Parameters	: true to enable or false to disable, (opt/true)toggle borders instantly, rather than move them on/off over time (Zelda stylee)
   Returns		: 
   Info			: instant flag is not supported yet (always true) - if you find a use for it, please implement it
*/
void setFilmBorders(const bool borders, const bool instant)
{
	// PP: don't use film borders in 16:9
	if(!gameStatus.wideScreenWilson)
	{
		videoMode.filmBorders=borders;
	}
}


/* --------------------------------------------------------------------------------
   Function		: drawFilmBorders
   Purpose		: draw film borders
   Parameters	: 
   Returns		: 
   Info			: check (videoMode.filmBorders) and (CURRENTLY_DRAWING_FULL_SCREEN) before calling this
*/

#define FILM_BORDER_HEIGHT_PORTION		0.125f		// PP: height of a film border as a fraction of the screen height// PP: NOTE: 0.125f is absolutely correct for creating a 16:9 letterbox on a 4:3 screen

void drawFilmBorders(void)
{
	static RECTANGLE	rect;

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdSetIdentityObjectMatrix();

	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	bdSetTexture(0,NULL);

	rect=CURRENT_VIEWPORT.toOrigin();

	rect.topPortion(FILM_BORDER_HEIGHT_PORTION).draw(BLACK, SOLIDFILL);// PP: not tremendously fast
	rect.bottomPortion(FILM_BORDER_HEIGHT_PORTION).draw(BLACK, SOLIDFILL);// PP: not tremendously fast

	// PP: back to 3d mode again
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
}


/* --------------------------------------------------------------------------------
   Function		: pushRenderTarget
   Purpose		: push a render target onto the render target stack and target rendering to it
   Parameters	: target to push, (opt/255)r,(opt/0)g,(opt/255)b,(opt/255)a,(opt/1.0f)zdepth to which to clear target (depending on flags), (opt/0)combination of BSETRENDERTARGET_ flags
   Returns		: OK/FAIL
   Info			: 
*/
int pushRenderTarget(	TBRenderTarget *const	target,
						const int				r,
						const int				g,
						const int				b,
						const int				a,
						const float				depth,
						const uint32			flags)
{
	int		rtn;

	if(target == NULL)
	{
		bkPrintf("pushRenderTarget: warning - pushing a NULL target (this should happen only once, at the beginning of the game.)\n");
	}

	// PP: gotter check for success before PUSHing, because in some cases we only POP if the PUSH was successful
	if((rtn=bdSetRenderTarget(target, r, g, b, a, depth, flags)) == OK)
	{
		renderTargetStack.push(target);

#ifdef ORTSTACK_DEBUG
		bkPrintf("pushRenderTarget: stack depth %d\n", renderTargetStack.getDepth());
#endif// PP: def ORTSTACK_DEBUG
	}
#ifdef ORTSTACK_DEBUG
	else
	{
		bkPrintf("pushRenderTarget: *** WARNING *** COULD NOT SET RENDER TARGET %08x\n", target);
	}
#endif// PP: def ORTSTACK_DEBUG

	return rtn;
}


/* --------------------------------------------------------------------------------
   Function		: popRenderTarget
   Purpose		: pop the current render target off the render target stack, retargeting rendering to the target below it
   Parameters	: (opt/255)r,(opt/0)g,(opt/255)b,(opt/255)a,(opt/1.0f)zdepth to which to clear target (depending on flags), (opt/0)combination of BSETRENDERTARGET_ flags
   Returns		: OK/FAIL
   Info			: 
*/
int popRenderTarget(const int		r,
					const int		g,
					const int		b,
					const int		a,
					const float		depth,
					const uint32	flags)
{
	renderTargetStack.pop();

#ifdef ORTSTACK_DEBUG
	bkPrintf("popRenderTarget: stack depth %d\n", renderTargetStack.getDepth());
#endif// PP: def ORTSTACK_DEBUG

	return bdSetRenderTarget(renderTargetStack.getTopItem(), r, g, b, a, depth, flags);
}

/*  --------------------------------------------------------------------------------
	Function	: ScaleScreenVectorFromPS2
	Purpose		: Scales a screen position vector from the standard 640 x 448 screen to whatever is set in videoMode.  Helps with fixing old code without making to large a change
	Parameters	: 
	Returns		: 
	Info		: 
*/

void ScaleScreenVectorFromPS2(TBVector unscaledVector)
{
	TBVector scaledVector;

	SETVECTOR(scaledVector, (unscaledVector[X]/640.0f)*videoMode.xScreen, (unscaledVector[Y]/448.0f)*videoMode.yScreen, unscaledVector[Z], unscaledVector[W]);
	
	bmVectorCopy(unscaledVector, scaledVector);
}


#if(PLATFORM == PS2)
/*  --------------------------------------------------------------------------------
	Function	: setSpriteObjMat
	Purpose		: set an object matrix to correct the appearance of sprites
	Parameters	: 
	Returns		: 
	Info		: // PP: corrects PS2 splitscreen distortions
*/
void setSpriteObjMat(void)
{
	VEC			scale;
	TBMatrix	scaleMat;

	// PP: On PS2, correct split-screen distortions
	// PP: NOTE: Tech Team are due to fix this at some point

	if(gameStatus.multiplayer.numScreens == 2)
	{
		if(gameStatus.multiplayer.splitScreenMode == SPLITSCREEN_HORIZONTAL)
		{
			scale.set(1.0f, 2.0f, 1.0f);		// PP: double the height of the sprites
		}
		else// PP: SPLITSCREEN_VERTICAL
		{
			scale.set(2.0f, 1.0f, 1.0f);		// PP: double the width of the sprites
		}

		bmMatScale(scaleMat, scale[X], scale[Y], scale[Z]);

		bdSetObjectMatrix(scaleMat);
	}
	else
	{
		bdSetIdentityObjectMatrix();
	}
}
#endif// PP: ps2