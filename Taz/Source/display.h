#ifndef __H_DISPLAY
#define __H_DISPLAY

#include "main.h"							// PP: main stuff
#include "Colour.h"							// PP: handy colour struct
#include "Rectangle.h"						// PP: Handy rectangle class

// PP: far clipping distance
// PP: I upped this so that the env-map could be scaled right up, so that it writes to the back of the zbuffer and thus defocuses properly
// NH: Moved here as I wanted to access it from other files
#define FARCLIP		METERTOUNIT(300)// PP: METERTOUNIT(120)

// PP: ~~~~~~~~ FIELDS OF VIEW ~~~~~~~~
// PP: NB - THESE ARE ALL VERTICAL FOVS

#define NORMAL_INGAME_FOV			RAD(67.5f)	// PP: (90*3/4 as before) // PP: field of view used normally while playing the game

// PP: NB - I'm currently assuming 1:1 scale-on-screen in both splitscreen modes, apart from when widescreen is on...

#define HORIZONTAL_SPLITSCREEN_FOV	RAD(35)		// PP: field of view used in vertical splitscreen mode
#define VERTICAL_SPLITSCREEN_FOV	RAD(67.5f)	// PP: field of view used in horizontal splitscreen mode

#define	OBJECT_VIEWER_FOV			RAD(53.5f)	// PP: (70*3/4 as before) // PP: field of view used for the object viewer

#define XASPECT_NORMAL				4			// PP: * DO * NOT * TOUCH! * 'normal' x aspect, used to adjust FOVs for different aspect ratios
#define YASPECT_NORMAL				3			// PP: * DO * NOT * TOUCH! * 'normal' y aspect, used to adjust FOVs for different aspect ratios

#define XASPECT_WIDESCREEN			16			// PP: 16, as in 16:9 - what more can I say?!
#define YASPECT_WIDESCREEN			9			// PP: 9, as in 16:9 - what more can I say?!

// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// PP: ~~~~~~ FRAMEWRITE STUFF ~~~~~~~~

// PP: PLEASE USE THESE FRAMEWRITE FLAG COMBOS INSTEAD OF THE FLAGS THEMSELVES
// PP: If you don't, stereo vision will get buggered and that would be unpleasant.
// PP: Stereo vision is my sweet sweet precious baby.  LISTEN DON'T YOU TOUCH MA WEAN HEN!
// PP: I could redefine bdSetRenderState as a macro that enforces this, but I can imagine the overhead!
extern int				rgbFramewriteChannels;
// PP: REMOUT: so's people don't use it by accident	(more often than not, we shouldn't write to the alpha channel)		extern int				allFramewriteChannels;

// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

extern char numVideos;
extern char *videos[];

// PP: ~~~~~ PS2 SCREEN DIMENSIONS ~~~~

#define XSCREEN_16BIT	640					// PP: width of the screen in 16 bit
#define YSCREEN_16BIT	448					// PP: height of the screen in 16 bit

#define XSCREEN_NORMAL	XSCREEN_16BIT		// PP: 'normal' width of the screen, used to adjust FOVs for resolutions with different width:height ratios
#define YSCREEN_NORMAL	YSCREEN_16BIT		// PP: 'normal' height of the screen, used to adjust FOVs for resolutions with different width:height ratios

#define XSCREEN_32BIT	512					// PP: width of the screen in 32 bit
#define YSCREEN_32BIT	448					// PP: height of the screen in 32 bit

// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// PP: determine if the current viewport is one that fills the whole screen
#define CURRENTLY_DRAWING_FULL_SCREEN		(((gameStatus.multiplayer.numScreens == 1)&&(gameStatus.multiplayer.currentScreen == 0))||(gameStatus.multiplayer.currentScreen == -1))

// PP: macro evaluating to the current "screen" struct
#define CURRENT_SCREEN						((gameStatus.multiplayer.currentScreen == -1) ? \
											videoMode.screens[0] : \
											videoMode.screens[gameStatus.multiplayer.currentScreen])

// PP: macro evaluating to the current viewport
#define CURRENT_VIEWPORT					((gameStatus.multiplayer.currentScreen == -1) ? \
											RECTANGLE(0.0f, (float)videoMode.xScreen, 0.0f, (float)videoMode.yScreen) : \
											videoMode.screens[gameStatus.multiplayer.currentScreen].viewport)

// PP: "screen" struct - one per player usually
struct SCREEN
{
	RECTANGLE			viewport;			// PP: viewport rectangle - gets set up by setNumScreens
	float				fov;				// PP: field of view (?)
	float				horzFov;			// PP: horizontal field of view
	float				vertFov;			// PP: vertical field of view
	float				targetfov;			// PP: target field of view
	float				prevfov;			// PP: previous field of view
	float				timescale;			// PP: timescale for FOV transition
	float				currentTime;		// PP: current time into FOV transition
	float				tanFov;				// PP: the tangent of the field of view

}BALIGN16;

typedef struct TAG_DISPLAYSETTINGS
{
	int32				xScreen;			// total screen width
	int32				yScreen;			// total screen height
	int32				bppScreen;			// bits per pixel for screen (16,24,32)
	int32				zScreen;			// z buffer depth (16,32)
	unsigned char		xAspect;			// PP: x component of aspect ratio
	unsigned char		yAspect;			// PP: y component of aspect ratio
	float				safeHeightPortion;	// PP: 'safe' height portion of the screen, within which all text and overlays must be contained
	float				safeWidthPortion;	// PP: 'safe' width portion of the screen, within which all text and overlays must be contained
	
	SCREEN				screens[2];			// PP: the "screens" (usually one per player)
											
	uint32				flags;				// video mode - DISPLAYFLAG_NTSC,DISPLAYFLAG_PAL
	uint32				flagRequest;		// TP: flags to change to upon reloading scene (to fix PAL - NTSC problem)
	float				nearPlane;			// PP: near clip distance
	float				farPlane;			// PP: far clip distance
	
	bool				filmBorders;		// PP: are film borders on?

	// PP: Repeat-draw info ("for times when one draw loop just isn't enough™" eg. stereo vision.)
	// PP: This keeps the code tidier and more efficient...

	int					numDraws;			// PP: number of times to go through the draw loop
	int					drawNum;			// PP: zero-based index of current draw

} DISPLAYSETTINGS;


/* --------------------------------------------------------------------------------
   Function		: initDisplay
   Purpose		: initialise display settings at the beginning of the game
   Parameters	: 
   Returns		: 
   Info			: // PP: moved this here from main.cpp to keep things tidy - hope you don't mind
*/
void initDisplay(void);


/* --------------------------------------------------------------------------------
   Function : SetupDisplay
   Purpose : Create and setup display
   Parameters : 
   Returns : 
   Info :
*/
int SetupDisplay(DISPLAYSETTINGS *displaySettings);

/* --------------------------------------------------------------------------------
   Function : SwitchBetweenScreenModes
   Purpose : Change between windowed and fullscreen
   Parameters : 
   Returns : 
   Info : 
*/
void SwitchBetweenScreenModes(DISPLAYSETTINGS *displaySettings);

/* --------------------------------------------------------------------------------
   Function : FadeFieldOfView
   Purpose : update field-of-view-fading for as many screens as are active
   Parameters : 
   Returns : 
   Info : // PP: removed displaysettings param - not needed really
			// PP: removed return value - not used and not multiscreen-friendly
*/
void FadeFieldOfView(void);

/*  --------------------------------------------------------------------------------
	Function	: UpdateDisplay
	Purpose		: updates the display, eg. updates FOV fading
	Parameters	: 
	Returns		: 
	Info		: // PP: operates on 'videoMode'
*/
inline void UpdateDisplay(void)
{
	// PP: update any FOV fading
	FadeFieldOfView();
}

/* --------------------------------------------------------------------------------
   Function : SetFadeFieldOfView
   Purpose : 
   Parameters : (opt/NORMAL_INGAME_FOV)target fov, (opt/0.0f=instant change) timescale
   Returns : true if has reached target
   Info : // PP: removed displaysettings param - not needed really
		// PP: **NOTE** switched the other 2 params!
*/
void SetFadeFieldOfView(float targetFOV=NORMAL_INGAME_FOV, float timescale=0.0f);

/*	--------------------------------------------------------------------------------
	Function 	: DrawRectangle
	Purpose 	: Draw a rectangle
	Parameters 	: rect, colour
	Returns 	: 
	Info 		: 
*/
void DrawRectangle(const struct TAG_RECTANGLE &rect, const struct TAG_COLOUR &colour);

/*	--------------------------------------------------------------------------------
	Function 	: DrawLine
	Purpose 	: Draw a line using triangle strips
	Parameters 	: x1,y1,x2,y2, COLOUR
	Returns 	: 
	Info 		: 
*/
void DrawLine(const float x1,const float y1,const float x2,const float y2,const struct TAG_COLOUR &colour);

/*	--------------------------------------------------------------------------------
	Function 	: DrawHollowRect
	Purpose 	: Draw hollow rectangle Pahhh
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void DrawHollowRect(const struct TAG_RECTANGLE &rect, const struct TAG_COLOUR &colour);


/*	--------------------------------------------------------------------------------
	Function 	: applyFOV
	Purpose 	: apply the fields of view for the current screen
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void applyFOV(void);


/*	--------------------------------------------------------------------------------
	Function 	: setFOV
	Purpose 	: prepares the x&y fovs for the specified screen,
					taking into account screen dimensions, viewport dimensions and desired aspect ratio
	Parameters 	: vertical fov (the one that corresponds to screen height), (opt/NULL=current) screen whose fov to set
	Returns 	: 
	Info 		: the fovs are applied to the projection matrix by applyFOV next frame
*/
void setFOV(const float fov, SCREEN* const screen= &CURRENT_SCREEN);


/*	--------------------------------------------------------------------------------
	Function 	: setViewport
	Purpose 	: Set the viewport.  Use this instead of bdSetViewport if you please
	Parameters 	: left X pos, top Y pos, width, height
	Returns 	: 
	Info 		: // PP: 
*/
/*void setViewport(const int x,
				 const int y,
				 const int width,
				 const int height);*/


/*	--------------------------------------------------------------------------------
	Function 	: setViewport
	Purpose 	: Re-apply the current game viewport
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: call this after you've used a strange viewport for some reason,
					eg. rendering to some texture or another
*/
/*inline void setViewport(void)
{
	setViewport(CURRENT_VIEWPORT.left, CURRENT_VIEWPORT.top, CURRENT_VIEWPORT.width(), CURRENT_VIEWPORT.height());
}*/


/*	--------------------------------------------------------------------------------
	Function 	: setNumScreens
	Purpose 	: set the number of screens
	Parameters 	: number of screens, (opt/previous)
	Returns 	: 
	Info 		: // PP: TODO?: some effect(s) for when the number of screens changes
*/
void setNumScreens(const int numScreens, const ESplitScreenMode split = gameStatus.multiplayer.splitScreenMode);


/*	--------------------------------------------------------------------------------
	Function 	: toggleSplitScreenMode
	Purpose 	: toggle the split-screen mode
	Parameters 	: 
	Returns 	: new split-screen mode
	Info 		: // PP: TODO?: some effect(s) for when the split-screen mode changes
*/
ESplitScreenMode toggleSplitScreenMode(void);


/*  --------------------------------------------------------------------------------
	Function	: SetFog
	Purpose		: Set the current fog settings
	Parameters	: min and max values for the fog, colour for the fog
	Returns		: 
	Info		: 
*/
void SetFog(float min, float max, COLOUR colour);

/*  --------------------------------------------------------------------------------
	Function	: DisableFog
	Purpose		: Like it sounds
	Parameters	: 
	Returns		: 
	Info		: 
*/
void DisableFog(void);


/*	--------------------------------------------------------------------------------
	Function 	: setWidescreen
	Purpose 	: set widescreen mode on/off
	Parameters 	: true=on, false=off
	Returns 	: 
	Info 		: // PP: this sets gameStatus.wideScreenWilson and videoMode.xAspect&yAspect + ticks/unticks gui item
					// PP: TODO?: some effect for when widescreen toggles(?)
					// PP: NB - I'm not in the habit of using silly variable names!
*/
void setWidescreen(const bool Wilson);


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
void overrideWidescreen(const bool override=true, const bool immediate=false);


/*  --------------------------------------------------------------------------------
	Function	: overrideStereoscopy
	Purpose		: override or un-override stereoscopic mode
	Parameters	: (opt/true)true=override/false=un-override
	Returns		: 
	Info		: // PP: use this to temporarily override stereoscopy,
					for things that don't want it, eg. paper books
*/
void overrideStereoscopy(const bool override);


/*  --------------------------------------------------------------------------------
	Function	: toggleWidescreen
	Purpose		: toggles widescreen mode on/off
	Parameters	: 
	Returns		: 
	Info		: // PP: this sets gameStatus.wideScreenWilson and videoMode.xAspect&yAspect + ticks/unticks gui item
*/
inline void toggleWidescreen(void)
{
	setWidescreen(!gameStatus.wideScreenWilson);
}


/* --------------------------------------------------------------------------------
   Function		: setFilmBorders
   Purpose		: set film-borders on/of
   Parameters	: true to enable or false to disable, (opt/true)toggle borders instantly, rather than move them on/off over time (Zelda stylee)
   Returns		: 
   Info			: instant flag is not supported yet (always true) - if you find a use for it, please implement it
*/
void setFilmBorders(const bool borders, const bool instant=true);


/* --------------------------------------------------------------------------------
   Function		: drawFilmBorders
   Purpose		: draw film borders
   Parameters	: 
   Returns		: 
   Info			: check (videoMode.filmBorders) and (CURRENTLY_DRAWING_FULL_SCREEN) before calling this
*/
void drawFilmBorders(void);


/* --------------------------------------------------------------------------------
   Function		: pushRenderTarget
   Purpose		: push a render target onto the render target stack and target rendering to it
   Parameters	: target to push, (opt/255)r,(opt/0)g,(opt/255)b,(opt/255)a,(opt/1.0f)zdepth to which to clear target (depending on flags), (opt/0)combination of BSETRENDERTARGET_ flags
   Returns		: OK/FAIL
   Info			: 
*/
int pushRenderTarget(	TBRenderTarget *const	target,
						const int				r		= 255,
						const int				g		= 0,
						const int				b		= 255,
						const int				a		= 255,
						const float				depth	= 1.0f,
						const uint32			flags	= 0);


/* --------------------------------------------------------------------------------
   Function		: popRenderTarget
   Purpose		: pop the current render target off the render target stack, retargeting rendering to the target below it
   Parameters	: (opt/255)r,(opt/0)g,(opt/255)b,(opt/255)a,(opt/1.0f)zdepth to which to clear target (depending on flags), (opt/0)combination of BSETRENDERTARGET_ flags
   Returns		: OK/FAIL
   Info			: 
*/
int popRenderTarget(	const int		r		= 255,
						const int		g		= 0,
						const int		b		= 255,
						const int		a		= 255,
						const float		depth	= 1.0f,
						const uint32	flags	= 0);

/*  --------------------------------------------------------------------------------
	Function	: ScaleScreenVectorFromPS2
	Purpose		: Scales a screen position vector from the standard 640 x 448 screen to whatever is set in videoMode.  Helps with fixing old code without making to large a change
	Parameters	: 
	Returns		: 
	Info		: Did a search and a lot of stuff seemed to be hard coded for this screen size, so I thought something like this may help
*/

void ScaleScreenVectorFromPS2(TBVector unscaledVector);


/*  --------------------------------------------------------------------------------
	Function	: setSpriteObjMat
	Purpose		: set an object matrix to correct the appearance of sprites
	Parameters	: 
	Returns		: 
	Info		: // PP: corrects PS2 splitscreen distortions
*/
#if(PLATFORM == PS2)

void setSpriteObjMat(void);

#else// PP: !ps2

#define setSpriteObjMat		bdSetIdentityObjectMatrix

#endif// PP: !ps2


#endif// PP: ndef __H_DISPLAY
